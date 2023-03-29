/****************************************************************************
  Description   : Main Initializing Module for INGRAM
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corporation.
                  Copyright (c) 2010 All Rights Reserved

  Project       : INGRAM

----------------------------------------------------------------------------
  Changes		:
	Name   Date          Description
  ------------------------------------------------------------------------
	I.M.   2011.02.01     re-created

****************************************************************************/
#include "Basedef.h"
#include "MercuryGlobals.h"
#include "NetworkIf.h"
#include "HwDeviceIf.h"
#include "JL086ServoIpHw.h"
#include "BaseControlMain.h"
#include "SerialEncIf.h"
#include "MicroScanAIf.h"
#include "MicroPrmCalc.h"
#include "SysConfig.h"
#include "SequenceMain.h"
#include "CheckAlarm.h"
#include "SequenceIo.h"
#include "GainChange.h"
#include "FunListDef.h"
#include "AutoNotchSet.h"
#include "DataTraceManager.h"
#include "PnPrmListTbl.h"
#include "PosManager.h"
#include "PrmAccessIf.h"
#include "CtrlPrmTransducer.h"
#include "KLib.h"
#include "DetectTemperature.h" /* <S03A> */
#include "MotorLessTestStr.h"	/* <S1B7> */
#include "MotorLessTest.h"		/* <S1B7> */

#if CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE
#include "PnlOp.h"
#else
/* TODO: AP�p�p�l�I�y */
#define PnlOpInitialize( ) {;}
#endif
/* ---futami---> */
//#include "DetectTemperature.h"
//#include "OverheatManager.h"
/* <---futami---*/
#include "Interrupt.h"

#if 0 /* 2012.08.18 Y.Oka Make Mercury Base Software */
#include "FileDownload.h"
#endif /* 2012.08.18 Y.Oka Make Mercury Base Software */

#ifndef _VC_DEBUG_ /* 2013.05.31 Y.Oka */
#include "KnlApi.h"				/* TODO LCD Operator */
#endif

/* --> <USB-FUNCTION> 2013.05.13 K.Sakamoto */
//#include "config.h"
//#include "nbpfahb32vic128.h"
//#include "..\..\..\Board\MICROSSP\driver\usb_function\usb_f.h"
//#include "..\..\..\Board\MICROSSP\driver\usb_function\msc\msc.h"
/* <-- <USB-FUNCTION> 2013.05.13 K.Sakamoto */

#if defined(_TEST_BOARD_) && !defined(_VC_DEBUG_)
extern	void	InitializeInterruptCpu( void );
extern	void	SetEnableInterrupt( void );
extern	void	ClearM3InterruptState( void );
#endif


/* 2012.12.27 Y.Oka �b��Ή��B��`�ꏊ�����K�v */
void FunCmnDataInitialize( FUN_CMN_CONTROL *FnCmnCtrl );


/* �O���֐� */
extern void InitInterruptStart( void );
extern void KpxInitMsgInterface( AXIS_HANDLE *pAxRsc, MEMOBUS_ADDR_MAP *AxisMap );
extern void KpxInitUsbMsgIf( LONG DriveID, LONG ComAddr, LONG AxisNumber );	
extern void NpiScanNInitMain( SV_OPTION_IF *psvif );
extern void RpxInitMonManager( void );
extern void RpxInitPrmManager( void );
extern void RpxInitFunManager( void );
extern void RpxInitOpeManager( void );
extern void RpxInitInfManager( void );
extern void RpxInitMemManager( void *pBuf, long size );
//extern void RpxLcdOpInitDispInfo( void );
extern void RpxLcdOpInitDispInfo( AXIS_HANDLE* );		/* <S213> */
extern void SysInitSafetyDevice( USHORT, DET_HWBB_STS*, USHORT, TEST_PULSE_DIAG* );

/*-----------------------------------------------------------------------
;		INTERN LIST
;----------------------------------------------------------------------*/
static void	SysInitAxisParameters( AXIS_HANDLE *AxisRsc, BOOL UnitCommonCalc);
static void	SysCheckAmpMotType(AXIS_HANDLE *AxisRsc);
static void	SysSetMotorEncoderError(SENC_ERROR err, ALMMAN_HANDLE *AlmMngr );
static void	SysConnectEncoders( AXIS_HANDLE *AxisRscI, INT AxisRscNum );
static void	SysConnectFcEncoders( AXIS_HANDLE *AxisRsc );/* <S04D> */
static void	SysLoadEepromParameters( AXIS_HANDLE *AxisRscI );
static void	SysModuleConfiguration(AXIS_HANDLE *AxisRsc, MEMOBUS_ADDR_MAP *AddrMap);
#if (CSW_MAICRO_DMA_TRANSFER != 0)
static void SysInitMicroDMATransfers(AXIS_HANDLE *AxisRsc, LONG AxRscNum);
#endif
static void	SysInitializeScanBcycle( AXIS_HANDLE *AxisRscB, ULONG SBcycle );
static	BOOL	SysPrstSelectMotorEncoder( AXIS_HANDLE *AxisRsc );/*<S0C7>*/
static void	SysInitSelectMotorEncoder( AXIS_HANDLE *AxisRscI, INT AxisRscNum );/* <S04D> */
static	void	IniVelRippleCompPrmE( AXIS_HANDLE *Axis );		/* <S14B> */

#if 0 /* ��-7�s�g�p *//* <S0DF> */
/* const table for the Axis address parameter */
const CPPRM_ATTR_T CprmAxisAddrDef[8] = {
	&pndef_AxisAddress0,
	&pndef_AxisAddress1,
	&pndef_AxisAddress2,
	&pndef_AxisAddress3,
	&pndef_AxisAddress4,
	&pndef_AxisAddress5,
	&pndef_AxisAddress6,
	&pndef_AxisAddress7,
};
#endif

/* --> <USB-FUNCTION> 2013.05.13 K.Sakamoto */
//void u2f_int_intu2f( void );
//void u2f_int_intu2fepc( void );
//
//static tINTC_REGIST intc_regist_table_u2f[2] =
//const tINTC_REGIST intc_regist_table_u2f[2] =
//{
//	{INTC_IDX_U2F_INT,		u2f_int_intu2f,		INTC_LEVEL_HI,	INTC_PRL_LVL7, 0},
//	{INTC_IDX_U2F_EPC_INT,	u2f_int_intu2fepc,	INTC_LEVEL_HI,	INTC_PRL_LVL7, 0}
//};
/* <-- <USB-FUNCTION> 2013.05.13 K.Sakamoto */

#ifdef	_ASIP_DEBUG_BUF_USE_
extern USHORT		DebugBuf1[100];		/* �f�o�b�O�p											 *//* <TempSensorIC> */
extern USHORT		DebugBuf2[100];		/* �f�o�b�O�p											 *//* <TempSensorIC> */
extern USHORT		DebugBuf3[100];		/* �f�o�b�O�p											 *//* <TempSensorIC> */
extern USHORT		DebugBuf4[100];		/* �f�o�b�O�p											 *//* <TempSensorIC> */
extern USHORT		DebugBuf5[100];		/* �f�o�b�O�p											 *//* <TempSensorIC> */
extern USHORT		DebugBuf6[100];		/* �f�o�b�O�p											 *//* <TempSensorIC> */
extern USHORT		DebugBuf7[100];		/* �f�o�b�O�p											 *//* <TempSensorIC> */
extern USHORT		DebugBuf8[100];		/* �f�o�b�O�p											 *//* <TempSensorIC> */
extern USHORT		DebugBuf9[100];		/* �f�o�b�O�p											 *//* <TempSensorIC> */
extern USHORT		DebugBuf10[100];		/* �f�o�b�O�p											 *//* <TempSensorIC> */
extern USHORT		DebugBuf11[100];		/* �f�o�b�O�p											 *//* <TempSensorIC> */
extern USHORT		DebugBuf12[100];		/* �f�o�b�O�p											 *//* <TempSensorIC> */
extern USHORT		DebugBuf13[100];		/* �f�o�b�O�p											 *//* <TempSensorIC> */
extern USHORT		DebugBuf14[100];		/* �f�o�b�O�p											 *//* <TempSensorIC> */
extern INT			DebugCount;				/* �f�o�b�O�p											 *//* <TempSensorIC> */
extern INT 			Debug_i;				/* wait�p												 *//* <TempSensorIC> */
extern INT 			Debug_j;				/* 														 *//* <TempSensorIC> */
#endif	//#ifdef	_ASIP_DEBUG_BUF_USE_

/* for debug */
extern	LONG		DebugVariable[10];
/* for debug */

#if 0
/*****************************************************************************
*  Initialize Servo ASIC and Search AMP Axis module <S053>
*****************************************************************************/
static LONG SysInitializeASICs(UINT ax_cnt, ASICS *SvAsicRegs, ASIC_HW_IF_STR *AsicHwReg, EEP_DEVICE_HANDLE *hEepDev )
{

	/* Check the Number of Axes by reading JL-077 ID */
	AsicHwReg->AREG_IOSTATX = 0;
	if(JL076B_ASICID_CODE == (AsicHwReg->AREG_IOSTATX & 0xFF00))
	{
		/* Initialize JL-077 */
		SHal_InitAsicA(AsicHwReg);
#if defined(CSW_HW_SDA_EEPROM_I2C_SINGLE)
		EepdevInitialize(hEepDev, (void*)HW_SVASIC_ADDRESS, NULL);
#else
		EepdevInitialize(hEepDev, AsicHwReg, NULL);
#endif

		/* Check AMP module */
		{
#if defined(CSW_HW_SDA_EEPROM_I2C) || defined(CSW_HW_SDA_EEPROM_MICROWIRE)
		EEP_DEVICE_HANDLE EepDev;
		USHORT	dummy;
		EepDev.pvAmpDevice = AsicHwReg;
		if(0 == EepdevReadValues(&EepDev, PRMDEF_EEPROM_SDA, 0, &dummy, 1))
#endif
		{
			SvAsicRegs->AsicHwReg	= AsicHwReg;
			SvAsicRegs->AsicMicroReg = (ASIC_U_IF_STR*)((UCHAR*)AsicHwReg + JL076B_MICRO_OFS);

			ax_cnt++;
		}
		}
	}

	return ax_cnt;
}
#endif


/*****************************************************************************
*  Description :Mercury�����������imain�j(�S��)
*  Parameter   :
*  Return      :
*****************************************************************************/
MEMOBUS_ADDR_MAP AddrMap[MAX_AXIS_NUMBER];			/* <S167> */
void SysInitMain( AXIS_HANDLE *AxisRscI )
{
	LONG			ax_no;
	BOOL			UnitCmnFlag;
	ULONG			time;
	ULONG			nodeAddr;
	ULONG			AlmOLInfo;
//	UINT			ax_cnt;						/* <S053> *//*<S0C7>*/
	AXIS_HANDLE		*AxisRsc;
#if 0 /* 2012.08.06 Y.Oka Make Mercury Base Software */
	ASIC_HW_IF_STR	*HwAddress1, *HwAddress2;
#endif /* 2012.08.06 Y.Oka Make Mercury Base Software */
//<S150>	MEMOBUS_ADDR_MAP AddrMap[MAX_AXIS_NUMBER];
//<S167>	static MEMOBUS_ADDR_MAP AddrMap[MAX_AXIS_NUMBER];
	ALMMAN_HANDLE	*AlmMngrHdl[MAX_AXIS_NUMBER];
	INT i;
#if ( CSW_SVCF_LIFEPRED_USE == TRUE )
	SHORT			InternalTemperature;	/* < S154 > */
#endif
	ADATV	*AdatV;		/* <S145> */
	ADATP	*AdatP;
																									/* <TempSensorIC> */

	/****************************************************************************/
	/*	Initialize the variables 												*/
	/****************************************************************************/
	/* Disable JL077 synchronization in Kernel module */
//	HALdrv_EnableServoAsicSync(KPI_SBCYCLE_BASE_NS, 0); /* �s�g�p <S1D2> */

	/* �I�v�V����IF�A�v�������� */
	OpiInitialize( AxisRscI );

	/* Initialize Servo Resources */
	for( ax_no = 0; ax_no < MAX_AXIS_NUMBER; ax_no++ )
	{
		AxisRsc = &AxisRscI[ax_no];

		/* Alarm Manager */
		AlmMngrHdl[ax_no] = AxisRsc->AlmManager;
		ALMInitAlarmManager( AxisRsc->AlmManager,
							 servo_alm_tbl,
							 AlmDefTblEntNum,
							 ax_no ); /* <S053> */
//<S053>					 (AxisRsc->ModuleID->AsicNoMap)->asicNo[ax_no] );

		AxisRsc->AlmManager->hAlmLatchHndl = (void*)AxisRsc->hAlmLatchHndl;		/* <S212> */
		/* Fun Manager */
		FunInitialize( AxisRsc->FnManager, AxisRsc->RegManager,
					AxisRsc->FnAxCommons, FnFunTbl, FnFunTblEntNum );
		/* AAT <S053> */
		AdatSetStrPtr( (void*)AxisRsc );

		/* Initialize Function Control Common Servo Data */
		FunCmnDataInitialize( AxisRsc->FnCmnCtrl );

		/* Load System default values */
		PrmLoadSystemDefaultValues( AxisRsc->RegManager->hPnReg );	/* Parameter */
		PrmLoadSystemDefaultValues( AxisRsc->RegManager->hFnReg );	/* OpeRegister */

#if 0 /* 2012.08.19 Y.Oka Make Mercury Base Software �t�@�C���_�E�����[�h�p�H */
		/* Motor Parameter Download */
		FPPRG_InitMtPrmInfo(AxisRsc->MencV, ax_no);
#endif /* 2012.08.19 Y.Oka Make Mercury Base Software */

		/* Initialize the Option Module Variables *//* <S04D> */
		OpCardInitOptionVariables( AxisRsc->OptCardHdl, AxisRsc->UniPrms->Prm );
	}

	/* Initialize the Alarm Manager Global handle */
	ALMInitGlobalHandle( AlmMngrHdl, NumberOfAxes);


	/****************************************************************************/
	/*	Initializations of the Hardware driver									*/
	/****************************************************************************/
	/* Initialize Klib  								*/
	/* !!Timer�������g���ꍇ�͂��̏�������ōs�Ȃ�����!!		*/
/* ===>> change <TimerAPI> 2013.08.23 K.Sakamoto */
#if 0
	KlibInitialize( AxisRscI[0].SvAsicRegs );
#else
	KlibInitialize();
#endif
/* <<=== change <TimerAPI> 2013.08.23 K.Sakamoto */

	/* JL-086 Software Reset *//* <S0E4> */
	SHal_AsicSwReset( (ASIC_HW_IF_STR*)(HW_SVASIC_ADDRESS));
	 for( ax_no = 0; ax_no < MAX_AXIS_NUMBER /* <S053> (AxisRscI->ModuleID->AsicNoMap)->asicNum */; ax_no++ )
	{	/* JL-086 ����������A (�p�����[�^��ˑ�) */
			SHal_InitAsicA( (ASIC_HW_IF_STR*)(HW_SVASIC_ADDRESS + (JL086_SVIP_OFS * ax_no))
						/*	<S053> (ASIC_HW_IF_STR*)((AxisRscI->ModuleID->AsicNoMap)->asicAddr[ax_no])  */
							);
	}
	/*	JL-076 ASIC�}�C�N���v���O�����̃_�E�����[�h�������������� */
	if( FALSE != SHal_InitMicroProgram( AxisRscI->SvAsicRegs ) )
	{	/* A.BF2 : �V�X�e���A���[���Q(Micro�ُ�) */
		ALMSetGlobalAlarm( ALM_UPROG );
	}

	/* I/O Device Initialize */
	HALdrv_InitIODevices();
	/* LED Output Initialize */
//	HALdrv_OutputLED(LED_DEFAULT_OUT); /* �s�g�p <Oka02> */
	/* Initialize Check Post */
	HALdrv_InitCheckPost( );
	/* 5V Tolerant Buffer Standby Clear <S01F> */
	HALdrv_Clear5VTBStandby( );



#if 0 /* 2012.08.06 Y.Oka Make Mercury Base Software (for INGRAM) */
	/* Initialize EEPROM devices */
	//HwAddress1 = (AxisRscI->ModuleID->AsicNoMap)->asicAddr[COA_SIO_AXIS];
	//HwAddress2 = (AxisRscI->ModuleID->AsicNoMap)->asicAddr[FBA_SIO_AXIS];
	//EepdevInitialize((AxisRscI->RegManager->hPnReg)->hEepDev, HwAddress1, HwAddress2);
#endif /* 2012.08.06 Y.Oka Make Mercury Base Software (for INGRAM) */
/*--------------------------------------------------------------------------------------------------*/
/*		I2C�o�X I/F�̏�����																			*/
/*--------------------------------------------------------------------------------------------------*/
	/* ���������[�v */	/* <S19B> */
	for( ax_no = 0; ax_no < MAX_AXIS_NUMBER; ax_no++ )
	{
		AxisRsc = &AxisRscI[ax_no];
		if( I2C_DRV_SUCCESS != I2CdrvInitCpuI2CBusIF( ) )												  /* <S097> */
		{ /* A.BF6 : �V�X�e���A���[���U(I2C�h���C�o�ُ�) */
			ALMSetServoAlarm( AxisRsc->AlmManager, ALM_I2C_DRVERR );
		}
//		EepdevInitialize(NULL, NULL, NULL, AxisRscI->pRwAllMng);	/* <S00C> EEPROM ������				*/
//		EepdevInitialize(NULL, NULL, NULL, AxisRscI->pRwAllMng, AxisRscI->pRwPdlMng);	/* <S031> 		*/
		EepdevInitialize(NULL, NULL, AxisRsc->pRwAllMng, AxisRsc->pRwPdlMng);	/* <S19B> *//* <S1D2> */
	}
	/* 0���֖߂� */
	AxisRsc = &AxisRscI[0];	/* <S19B> */
/*--------------------------------------------------------------------------------------------------*/


	/****************************************************************************/
	/*	Loading Parameters from EEPROM											*/
	/****************************************************************************/
#if !defined(_NON_EEPROM_) /* 2012.11.06 Y.Oka for TestBoard */
	/* Load parameters from EEPROM devices */
	SysLoadEepromParameters( AxisRscI );
#endif

#if 0/* <S0D6> */
	{ /*<S011>*/
/*--------------------------------------------------------------------------------------------------*/
	const UCHAR CProductName[] = { 	'G','S',  'V','D',  'M','-',  'A','D',   /* @@ �e�X�g�p�b��^�� */
									'2','0',  '8','E',  '3','M',  ' ','A',  };
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
	MlibCopyByteMemory( AxisRscI->ModuleID->IfInfo->ProductName, CProductName, sizeof(CProductName) );
/*--------------------------------------------------------------------------------------------------*/
	}
#endif

	time = 0;
	/* ���������[�v */	/* <S19B> */
	for( ax_no = 0; ax_no < MAX_AXIS_NUMBER; ax_no++ )
	{
		AxisRsc = &AxisRscI[ax_no];
//		EepdevReadValues(AxisRscI->AlmManager->EepDevice, 0,
		EepdevReadValues( 0,	/* <S19B> *//* <S1D2> */
						  //EEP_NOSUM_ADDRESS(0, EEP_TIMESTAMP_OFS),							/* <S04E>:DELL */
						  EEP_NOSUMCMN_ADDRESS(EEP_TIMESTAMP_OFS),							/* <S04E>:ADD */
						  (USHORT*)&time, 2 );
	}
	/* 0���֖߂� */
	AxisRsc = &AxisRscI[0];	/* <S19B> */

	/* Set last time stamp value */
	KlibInitializeTimeStamp( time );
	/* <S154> Start */
#if ( CSW_SVCF_LIFEPRED_USE == TRUE )
	/* ���ϓ������x�̒l��EEPROM����ǂ�(EEPROM���g�p���ω��x�擾)  */
	EepdevReadValues(AxisRscI->AlmManager->EepDevice, 0,
					 EEP_NOSUMCMN_ADDRESS(EEP_AVRINTERTMPR_OFS),
					 (USHORT*)&InternalTemperature, 1 );
	/* ���ϓ������x�Z�o���������� */
	AvgInternalTemperatureCalInit(&(AxisRscI->hLifeTime->AvgTmprHdl), time, InternalTemperature );

	/* ��������񐔏����� */
	LifeTimeSwithingCountInit(AxisRscI->hLifeTime);
#endif
	/* <S154> End */
#if	(CSW_DETECT_TEMPERATURE == TRUE) /* <S03A> */
	/* Initialize Temperature Detect IC */	/* <S00D> */
	DetTempInitialize( AxisRscI->TempManager );
/* for debug */
	/* Get Current Temperature from Detect Temp. IC */
//	DetTempManager( );
//	DetTempManager( );
/* for debug */
#endif

	/* ���������[�v */	/* <S19B> */
	for( ax_no = 0; ax_no < MAX_AXIS_NUMBER; ax_no++ )
	{
		AxisRsc = &AxisRscI[ax_no];
		/* Read EEPROM Option ID */
//		OpCardSetLastOptionId( AxisRscI->OptCardHdl, (AxisRscI->RegManager->hPnReg)->hEepDev );	/* <S04D> */
		OpCardSetLastOptionId( AxisRsc->OptCardHdl ); /* <S19B> *//* <S1D2> */
	}
	/* 0���֖߂� */
	AxisRsc = &AxisRscI[0];	/* <S19B> */

#if 0 /* 2012.08.06 Y.Oka Make Mercury Base Software (for INGRAM) */
	/* Set A.230 alarm mask, if COC module is not connected. */
	if(0x0300 != (AxisRscI->ModuleID->CoaInfo->BoardID & 0x0300))
	{
		for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
		{
			ALMSetMask( AxisRscI[ax_no].AlmManager, ALM_RLYON_ACK, TRUE );
		}
	}
#endif /* 2012.08.06 Y.Oka Make Mercury Base Software (for INGRAM) */



	/****************************************************************************
		Module Configuration
		���ӁF���̏����ŁANumberOfAxes��AxisPointer�̒��g�̓���ւ������{)
		����ȍ~�̏����ł́A�����I�Ȏ��ʒu�Ɣz��ԍ����قȂ�̂Œ��ӂ��邱��
	****************************************************************************/
	SysModuleConfiguration( AxisRscI, AddrMap );

	/* Re-initialize the Alarm Manager Global handle */
	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
		AlmMngrHdl[ax_no] = AxisRscI[ax_no].AlmManager;
	}
	ALMInitGlobalHandle(AlmMngrHdl, NumberOfAxes);

	/****************************************************************************/
	/*		Servo H/W Initializations: Encoder, Communication devices, etc,,	*/
	/****************************************************************************/
	/* Initialize JL077 DMA transfers */
#if (CSW_MICRO_DMA_TRANSFER != 0)
	SysInitMicroDMATransfers(AxisRscI, NumberOfAxes);
#endif

	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
		AxisRsc = &AxisRscI[ax_no];
		/* �I�v�V�����J�[�h���o����	*//* <S04D> */
		OpCardDetectOptions( AxisRsc->UniPrms->Prm, AxisRsc->OptCardHdl
									, AxisRsc->AlmManager, AxisRsc->SvAsicRegs->AsicHwReg);
		/* Write EEPROM Option ID	*//* <S04D> */
		OpCardWriteEepromOptionId( AxisRsc->OptCardHdl, ax_no );/* <S1D2> */

		/* �f�f������������<S1F6> */
		SysInitSafetyDevice( AxisRsc->UniPrms->Prm->opdetsw, &AxisRsc->IoSignals->DetHwbbSts,
								AxisRsc->UniPrms->Prm->tplsfil, AxisRsc->IoSignals->TestPulseDiag );
	}

	/* ���[�^���X�e�X�g���[�h�ݒ� *//* <S1B7> */
	MltSetMotorLessTestMode( AxisRscI, NumberOfAxes );

	/* �G���R�[�_������ */
	SysInitSelectMotorEncoder(AxisRscI, NumberOfAxes);	/* ���[�^���G���R�[�_�I��			*//* <S04D> */
	SysConnectEncoders(AxisRscI, NumberOfAxes);			/* �V���A���G���R�[�_�ڑ�����(�S��) */
#if	(CSW_SVCF_FULC_MODE != CSW_FULC_NO_SUPPORT)			/* <S04D> */
	if(AxisRscI->UniPrms->Bprm->FencUse != FALSE)
	{/*̨����ޯ���߼�݌��o�ς݂ŁA�O���ݺ��ނ̎g�p���@���ݒ肳��Ă���B 					*/
		SysConnectFcEncoders(AxisRscI);					/* �t���N���[�Y�h�ڑ�����			*/
	}
#endif/* CSW_SVCF_FULC_MODE != CSW_FULC_NO_SUPPORT*/

	/* �I�v�V����IF�f�o�C�X������ */
	OpiInitDevice( AxisRscI, (VOID*)HW_NETLSI_BASE_ADDRESS );


	/****************************************************************************/
	/*		Parameter Configurations											*/
	/****************************************************************************/
	UnitCmnFlag = TRUE;
	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
		AxisRsc = &AxisRscI[ax_no];
		/* ��������ݒ� */
		if(AxisRsc->UniPrms->Cprm->CycleSwitch != 0)
		{/* ScanB:125us */
			BprmSetServoCycle( AxisRsc->UniPrms->Bprm,
								KPI_SACYCLENS, KPI_SBCYCLE_BASE_NS );
		}
		else
		{/* ScanB:250us */
			BprmSetServoCycle( AxisRsc->UniPrms->Bprm,
								KPI_SACYCLENS, KPI_SBCYCLE_BASE_NS*2 );
		}

#ifdef CSW_SCANA_ENABLE
		/* �x�[�X���[�v������ */
		/* ���|�C���^�ݒ�̂݁B���������������遚 */
		SysScanAInit( AxisRsc->BaseLoops,
					  AxisRsc->MencV,
					  AxisRsc->FencV,/* <S04D> */
					  AxisRsc->UniPrms->Bprm,
					  AxisRsc->MotSpdMafil,
					  AxisRsc->BaseControls,
					  &(AxisRsc->BaseControls)->GainChange.GselGains[0],
					  AxisRsc->SvAsicRegs->AsicHwReg,
					  AxisRsc->AlmManager,/* <S018> */
					  AxisRsc->MotorLessTest );	/* <S1B7> */
#endif /* CSW_SCANA_ENABLE */

		/* ���[�^���X�e�X�gIF��`�̈揉���� *//* <S1B7> */
		ApxInitMotorLessTestMode( AxisRsc );

		/* �x�[�X�R���g���[���f�[�^������(��������ˑ��A�p�����[�^��ˑ�) */
		InitScanBData( AxisRsc );

		/* �����p�����[�^�v�Z */
		SysInitAxisParameters( AxisRsc, UnitCmnFlag );
		UnitCmnFlag = FALSE;
	}
	SysInitializeScanBcycle(AxisRscI, AxisRsc->UniPrms->Bprm->SvCycleUs);

	/****************************************************************************/
	/*		Final Initializations												*/
	/* 			(�e�X�L�����p�����[�^,�G���R�[�_�ˑ��f�[�^������)				*/
	/****************************************************************************/
//	HALdrv_EnableServoAsicSync(KPI_SBCYCLENS, 6/KPI_SBCYCLE_TIME);
	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
		AxisRsc = &AxisRscI[ax_no];

		/* �A���[���Ǘ��X�^�[�g����(AlmOLInfo���g���l�͂��̌�łȂ���NG) */
		AlmOLInfo = ALMStartupProcedure( AxisRsc->AlmManager,
							 (UCHAR)(AxisRsc->BeSequence->AlmStop.conf.StopMode),
							  AxisRsc->MencV->AxisMotType, AxisRsc->MencV->AbsoEncType );
		/* �T�[�{�Q�C���ؑ֗p�ϐ������� */
		GselInitServoGainChange( &(AxisRsc->BaseControls->GainChange) );
		/* Reset Servo Sequence Variables */
		SyscInitServoSequence( AxisRsc );
		/* <S154> Start */
		/* �A���[�����b�`���������� */
		/* �A���[�����b�`���j�^�̒l��EEPROM�֓ǂ� */
		EepdevReadValues( 0, /* <Oka02> */
						  EEP_NOSUM_ADDRESS(ax_no, EEP_ALMMON_OFS),
						  (USHORT*)&AxisRsc->hAlmLatchHndl->LatchMon , (EEPSIZE_ALMMON >> 1) );
		AlmLatchMonitorInitial( AxisRsc->hAlmLatchHndl );
		/* <S154> End */
		/* �A���[���Ď��p�ϐ������� */
		AdetInitCheckAlarm( AxisRsc->CheckAlarm, AlmOLInfo );
		/* �T�[�{�����ԊĎ��ϐ����������� */
		DetVibInitialize(AxisRsc->DetVib );
		/* JL076B �����o�͊J�n */
//		(AxisRsc->SvAsicRegs->AsicHwReg)->AREG_IOSTAT1 |= BIT3;
		/* �����m�b�`�ݒ�V�[�P���X�p�ϐ������� */
		AutoNotchInitialize( AxisRsc->ANotchSeq );
		/* AAT Initialize */
//<S053>		AdatSetStrPtr( (void*)AxisRsc );

		/* ����p�ϐ��ꊇ�ϊ��T�[�r�X�ϊ����[�h�ύX */
		TranlateCtrlSetIntMode( &(AxisRsc->BaseControls->TransduceCtrl) );

		/* Initialize the Operation register */
		PrmRecalcAllParameters( AxisRsc->RegManager->hFnReg, FALSE );

		/* �ݒu�����j�^�ϐ������� *//* <S0ED> */
		EnvMonInitialize( AxisRsc->EnvMonitor );


#if (CSW_POWER_MONITOR_USE == TRUE) /* <S020> */
		/* ����d�͉��Z�p�ϐ������� */
		PrmCalcPowerMonitor( AxisRsc->CalcWattage,
							 AxisRsc->UniPrms->Prm->imaxs,
							 AxisRsc->UniPrms->Prm->MencP.imax );
#endif

#if (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE)/* �����I�ɂ́AAnalogIF�ł����L�����s���� *//* <S0A1> */
		/* �l�b�g���[�N�`�p�l�I�y������ */
		PnlOpInitialize( AxisRsc->PnlOpLedDataHndl );
#endif/* (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE)	*/
		/* <S180> Start */
		/* �����\���@�\ DB����񐔏����� */
		LifeTimeDBSwithingCountInit(AxisRsc->hDBLifeTime);
		/* <S180> End */
	}

	/* Initialize the Operation register */
	PrmRecalcAllParameters( AxisRsc->RegManager->hFnReg, TRUE );

#if (CSW_POWER_MONITOR_USE == TRUE) /* <S020> */
	/* ����d�͉��Z�p�ϐ������� */
	InitializePowerMonitor( AxisRsc->PowerMonitor );
#endif

	/* �d���Ǘ��p�ϐ������� */
	PcmInitPowerManager( AxisRscI->PowerManager,
						 AxisRscI->AlmManager->Status.AlmOLInfo );	/* <S0FF> */

	/* �I�[�o�[�q�[�g�Ď��p�ϐ������� *//* <S0ED> */
	OhmInitialize( AxisRscI->OverHeat );

/* <S14B> Start */
	/* ���x���b�v���⏞���� �d�C�p(PnFA0-PnFB2)�̍Čv�Z */
	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
		AxisRsc = &AxisRscI[ax_no];
		IniVelRippleCompPrmE( AxisRsc );		/* ���x���b�v���⏞���� �d�C�p(PnFA0-PnFB2)	*/
	}
/* <S14B> End */
	/****************************************************************************/
	/*		Stet Init Register 													*/
	/****************************************************************************//* <S04D> */
	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
		AxisRsc = &AxisRscI[ax_no];
#if CSW_OPT_CARD_FBOPTION != CSW_FBOPTION_NO_SUPPORT    /* <S19A>*/
		FbOptInitRegister(&AxisRsc->OptCardHdl->FbkOpt, AxisRsc->FencV, AxisRsc->UniPrms->Prm, AxisRsc->UniPrms->Bprm );
#endif
	}
	/****************************************************************************/
	/*		Start Hardware devices												*/
	/****************************************************************************/
	/* JL-100 �ʐM�h���C�o���s�J�n���� */
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W�i2���Ή��j�̂�	*//* <S0E4> */
	/* M-III MicroProgram Download Setting (PROGSEL Control) */
	HALdrv_SetM3PrgSel( CSW_M3_MICRO_DL );
#endif

#if (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE)/* �����I�ɂ́AAnalogIF�ł����L�����s����B	*/
	OpiStartDevice( AxisRscI, (VOID*)HW_NETLSI_BASE_ADDRESS );
#endif/* (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE)	*/

	/* Detect DC Volt AD Convert Start */
	HALdrv_StartAdConvert( );

/* --> <USB-FUNCTION> 2013.05.18 K.Sakamoto */
//#ifdef _VC_DEBUG_
/* <-- <USB-FUNCTION> 2013.05.18 K.Sakamoto */
	/****************************************************************************/
	/*		Initialize and start MEMOBUS communications							*/
	/****************************************************************************/
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE) /* <S069> */
	nodeAddr = (AxisRscI->UniPrms->Cprm->AxisAddress[0] & 0xFF);
#else
	nodeAddr = (AxisRscI->UniPrms->Prm->nodeAdr & 0xFF);
#endif
	if( (nodeAddr == 0) || (nodeAddr > 0x007F) )
	{ /* ���A�h���X�L���͈� 0x01 �` 0x7F */
		/* �͈͊O�̏ꍇ��"0x01"��ݒ� */
		nodeAddr = 0x01;
	}
//	MsgIfInitialize(AxisRscI->NetMsgIf,
//					AddrMap, MAX_AXIS_NUMBER, nodeAddr,		/* <S10E> */
//					(AxisRscI->UniPrms->Cprm)->IP_Port);
// �����g�p���Ȃ��Ȃ������ߍ폜 <S150>
//	MsgIfInitialize(AxisRscI->NetMsgIf,
//					AddrMap, MAX_AXIS_NUMBER, nodeAddr);

/* --> <USB-FUNCTION> 2013.05.18 K.Sakamoto */
//#endif
/* <-- <USB-FUNCTION> 2013.05.18 K.Sakamoto */

	/* --> <USB-FUNCTION> 2013.05.13 K.Sakamoto */
//#ifdef	_USB_FUNC_USE_		/*<S008>*//*<S138>*/
//	for( i = 0; i < (INT)(sizeof(intc_regist_table_u2f)/sizeof(tINTC_REGIST)); i++ )
//	{
//		intc_set_handler((tINTC_REGIST*)&intc_regist_table_u2f[i]);
//		intc_enable_interrupt(intc_regist_table_u2f[i].index);
//	}
//	u2f_init();							/* �s�v�ȏ���<S026> */
//#endif	//#ifdef	_USB_FUNC_USE_		/*<S008>*//*<S138>*/
	/* <-- <USB-FUNCTION> 2013.05.13 K.Sakamoto */




	/*----------------------------------------------------------------------------------------------*/
	/** 
	 * Initialize Round Resource Manager
	 * (Monitor, Parameter, Function, Register, etc)
	 *
	 */
	/*----------------------------------------------------------------------------------------------*/
	RpxInitMonManager( );										/* Initialize Un Monitor <XXX> */
	RpxInitPrmManager( );										/* Initialize Parameter <XXX> */
	RpxInitFunManager( );										/* Initialize Function <XXX> */
	RpxInitOpeManager( );										/* Initialize Operation <XXX> */
	RpxInitInfManager( );										/* Initialize Information <XXX> */
	RpxInitMemManager( AxisRscI->TraceBuffer, TRC_BUF_SIZE );	/* Initialize Trace Memory <S00C> *//* <S04C> */
	/*----------------------------------------------------------------------------------------------*/
	/*	Initialize LCD Operator Modules	<XXX>														*/
	/*  TODO: ���A�h���X�Ή�	 																	*/
	/*  TODO: �����Ή�																				*/
	/*----------------------------------------------------------------------------------------------*/
	KpxInitCpuDMA( );  											/* Initialize CPU DMA <XXX> */
	KpxInitCpuSerialIF( COM0_UART, 19200, 8, 2, 1);				/* Initialize CPU Uart if <XXX> */
//	KpxInitCpuSerialIF( COM0_UART, 38400, 8, 2, 1);				/* Initialize CPU Uart if <XXX> */
//	KpxInitSifMsgManager( COM0_UART, SIFCOM0, 1, (void*)&(AxisRscI->NetMsgIf->UartMsg) ); /* initialize UART Ch.0 *//* TODO Zantei */			/* <S061> : ��No.= 1��p�~ */
//<S150>		KpxInitSifMsgManager( COM0_UART, SIFCOM0, nodeAddr, (void*)&(AxisRscI->NetMsgIf->UartMsg) ); /* initialize UART Ch.0 *//* TODO Zantei */	/* <S061> : ���[�^��SW�Őݒ肵����No.���g�p���� */
	KpxInitSifMsgManager( COM0_UART, SIFCOM0, nodeAddr ); /* initialize UART Ch.0 *//* <S150> NetMsgIf�Ƃ̘A�g�p�~ */
	KpxInitVirSifManager( VCOM0_PANEL, 1 );						/* Initialize Panel Operator <XXX>  */
//<S150>	KpxInitUsbMsgIf( 0, 0, 1, &(AxisRscI->NetMsgIf->UsbMbusServer) );/* TODO: Zantei */
	KpxInitUsbMsgIf( 0, nodeAddr, 1 );							/* Initialize USB */
#ifdef CSW_NETIF_M3_TYPE	/* <S150> */
	KpxInitMlinkMsgIf( 0, 0, nodeAddr, 0 );						/* Initialize M3 Message */
	KpxInitMlinkMsgIf( 0, 0, nodeAddr, 1 );						/* Initialize M3 Message */
#endif	/* <SAKA_TEST> */
//	KpxInitMsgInterface( &AxisRsc[0] );                         /* Initialize Message if <XXX> *//* <S0E4> */
//<S150>	KpxInitMsgInterface( AxisRscI/* <S0E4> �b���1���� &AxisRsc[0]*/ );                         /* Initialize Message if <XXX> *//* <S0E4> */
	KpxInitMsgInterface( AxisRscI/* <S0E4> �b���1���� &AxisRsc[0]*/, AddrMap );                         /* Initialize Message if <XXX> *//* <S0E4> */
	/*----------------------------------------------------------------------------------------------*/
//	RpxLcdOpInitDispInfo( ); 									/* Initialize LCD Operator Display Info<XXX> */
	RpxLcdOpInitDispInfo( AxisRscI );			/* Initialize LCD Operator Display Info<XXX> 	*/ /* <S214> */
	/*----------------------------------------------------------------------------------------------*/
#if (SVFSEL_ADC_USE == 1)||(SVFSEL_ONLY_OH_ADC_USE == 1)		/* <S09F> *//* <S0E1>:SVFSEL_ONLY_OH_ADC_USE�ǉ� */
	KpxInitAdc( AxisRscI );										/* TODO: �ǂ����Ɉړ� */
#endif            /* <S09F> */
	KpxInitPls( AxisRscI );										/* TODO: �ǂ����Ɉړ� */
	/*----------------------------------------------------------------------------------------------*/
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)
	NpiScanNInitMain( &(AxisRscI->SvControlIf->CmdOptionIf) );	/* Message I/F ��������ɌĂԂ��� */
#endif

	/****************************************************************************/
	/*		Start Data Trace Manager Initialize									*/
	/****************************************************************************/
	DtrcInitDtrcManager( AxisRscI->TrcHndl, AxisRscI->TraceBuffer, AddrMap );

	/****************************************************************************/
	/*		Start ScanA Interrupt												*/
	/****************************************************************************/
#if defined(_TEST_BOARD_) && !defined(_VC_DEBUG_)
	/* �b�菈�u */
	InitializeInterruptCpu( );
	InitializeInterruptTiming( );
#else /* �{��p */

	/* DO NOTHING */

#endif

	/****************************************************************************/
	/*		Start Panel Operator Initialize										*/
	/****************************************************************************/
	for( i = 0; i < 10; i++ )
	{
		DebugVariable[i] = 0;
	}


//	PnlOpInitialize( ); /* <S0A1> */

	/****************************************************************************/
	/*		Start ASIC Current-A/D, Reference-A/D								*/
	/****************************************************************************/
	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
		AxisRsc = &AxisRscI[ax_no];

		/* Enable Encoder Interrupt for ASIC */
//		SHalDrv_EnableEncoderInterrupt( AxisRsc->SvAsicRegs->AsicHwReg );/* <S004> */
		/* Sync the Encoder Communication with CPU Operation */
		SHalDrv_SyncEncoderCommunication( AxisRsc->SvAsicRegs->AsicHwReg, (USHORT*)AxisRsc->MencV->EncIf.RxFlg );

		/* micro program start����	*/
		AxisRsc->MencV->EncDisable = FALSE;	/* ScanA�X�^�[�g */
		SHal_StartMicroProgram( AxisRsc->SvAsicRegs );

#if 0 /* 2012.08.23 Y.Oka Make Mercury Base Software */
		/* for Micro debug */
		(AxisRsc->SvAsicRegs->AsicHwReg)->AREG_PIOSET = 0x1000;
#endif /* 2012.08.23 Y.Oka Make Mercury Base Software */

		AdatP = &(AxisRsc->AdatHndl->AdatP);/* <S145> */
		AdatV = &(AxisRsc->AdatHndl->AdatV);

	    AxisRsc->AdatHndl->AdatReg.AdatMovep.Long = AdatPcalDefautDistance( AdatP,
													 AdatP->MencV->AxisMotType,
	                                                 AdatP->MencV->LowSpdMotor ) / 1000;
	}

#ifdef	_ASIP_DEBUG_BUF_USE_
	for(i = 0; i < 100; i++)
	{
		DebugBuf1[ i ] = 0;		/* �z���������									*//* <TempSensorIC> */
		DebugBuf2[ i ] = 0;		/* �z���������									*//* <TempSensorIC> */
		DebugBuf3[ i ] = 0;		/* �z���������									*//* <TempSensorIC> */
		DebugBuf4[ i ] = 0;		/* �z���������									*//* <TempSensorIC> */
		DebugBuf5[ i ] = 0;		/* �z���������									*//* <TempSensorIC> */
		DebugBuf6[ i ] = 0;		/* �z���������									*//* <TempSensorIC> */
		DebugBuf7[ i ] = 0;		/* �z���������									*//* <TempSensorIC> */
		DebugBuf8[ i ] = 0;		/* �z���������									*//* <TempSensorIC> */
		DebugBuf9[ i ] = 0;		/* �z���������									*//* <TempSensorIC> */
		DebugBuf10[ i ] = 0;	/* �z���������									*//* <TempSensorIC> */
		DebugBuf11[ i ] = 0;	/* �z���������									*//* <TempSensorIC> */
		DebugBuf12[ i ] = 0;	/* �z���������									*//* <TempSensorIC> */
		DebugBuf13[ i ] = 0;	/* �z���������									*//* <TempSensorIC> */
		DebugBuf14[ i ] = 0;	/* �z���������									*//* <TempSensorIC> */
	}
	DebugCount = 0;				/* ������										*//* <TempSensorIC> */
	Debug_i = 0;				/* ������										*//* <TempSensorIC> */
	Debug_j = 0;				/* ������										*//* <TempSensorIC> */
//	AxisRsc->BaseCtrlOut->CmdSeqBit = 0;	/* ������									*//* <TempSensorIC> */
//	AxisRsc->SeqCtrlOut->BeComplete = 0;	/* ������									*//* <TempSensorIC> */
#endif	//#ifdef	_ASIP_DEBUG_BUF_USE_

//#ifdef SVFIF_CMDOPT		/* for debug EtherCAT���	*/		/*<S008>*/
//	HALdrv_OutputDebugSP( CoE_SRST );		/* for netX Soft Reset debug */
//#endif		//#ifdef SVFIF_CMDOPT		/* for debug EtherCAT���	*/		/*<S008>*/
	HALdrv_OutputDebugSP( OP_SWRST );		/* <S106> */

	/****************************************************************************/
	/*		Reset P1Mode before Enable Interrupt						<S1D3>	*/
	/****************************************************************************/
	SencChangeP1Mode( AxisRscI->MencV );

#if !defined(_VC_DEBUG_)
//	KPI_EI();										//SetEnableInterrupt( ); /* @@ Enable Interrupt */
	SHalDrv_KickWDT( AxisRscI->SvAsicRegs->AsicHwReg, FALSE );		/* <S0CC> */
	InitInterruptStart();
#endif
}


/*****************************************************************************
*  Description :�T�[�{�A���v���T�[�{���[�^�̃^�C�v�`�F�b�N(���P��)
*
*	�A���}�b�`����
*	(1) ���[�^�ƃA���v�̓��͓d���̕s��v
*	(2) ���[�^�ƃA���v�Ƃ̗e�ʍ���4�{�ȏ�
*
*  Parameter   :
*  Return      :
*****************************************************************************/
static void SysCheckAmpMotType(AXIS_HANDLE *AxisRsc)
{
	PRMDATA 	*Prm;
	BPRMDAT 	*Bprm;

	Prm = AxisRsc->UniPrms->Prm;
	Bprm = AxisRsc->UniPrms->Bprm;

	/*	�A���[���}�X�N�`�F�b�N	*/
	if(0 == (Prm->almmsk1 & 0x0001))
	{
		/*	�d���R�[�h�`�F�b�N	*/
		if( Bprm->AmpOutVcode != Bprm->MotVcode )
		{	/* A.050 : ���[�^/�T�[�{�e�ʃA���}�b�` */
			ALMSetServoAlarm(AxisRsc->AlmManager, ALM_UNMATCH );
		}
		/*	�e�ʔ�`�F�b�N */
		if( ( Bprm->AmpMotWatRatio <= 0x4000 ) ||
			((Bprm->AmpMotWatRatio >= 0x40000) && (Bprm->AmpMotImaxRatio > 0x40000)) )
		{	/* A.050 : ���[�^/�T�[�{�e�ʃA���}�b�` */
			ALMSetServoAlarm(AxisRsc->AlmManager, ALM_UNMATCH );
		}
	}
}


/*****************************************************************************
*  Description :�V���A���G���R�[�_�̏�����(���P��)
*  Parameter   :
*  Return      :
*****************************************************************************/
static void	SysSetMotorEncoderError(SENC_ERROR err, ALMMAN_HANDLE *AlmMngr )
{
	BOOL mask;

	mask = FALSE;
	/*----------------------------------------------------------------------*/
	/* �V���A���G���R�[�_�ڑ��`�F�b�N */
	/*----------------------------------------------------------------------*/
	if(err != SENCERR_SUCCESS)
	{
		if( SENCERR_PGECHO & err )
		{	/* A.CB0 : �G���R�[�_�G�R�[�o�b�N�ُ� */
			ALMSetServoAlarm( AlmMngr, ALM_PGECHO );
			mask = TRUE;
		}

		if( SENCERR_PGCOM & err )
		{	/* A.C90 : �G���R�[�_�ʐM�ُ� */
			ALMSetServoAlarm( AlmMngr, ALM_PGCOM );
			mask = TRUE;
		}

		if( SENCERR_NOTSUPPORT & err )/* <S1BE> */
		{	/* A.051 : ���i���T�|�[�g�A���[�� */
			ALMSetServoAlarm( AlmMngr, ALM_NOTSUPPORT );
		}

#if !defined( CSW_NETIF_SYNQNET_TYPE )
		if( SENCERR_MTLMT & err )
		{	/* A.CC0 : �}���`�^�[�����~�b�g�l�s��v */
			ALMSetServoAlarm( AlmMngr, ALM_MTLMT );
		}
#endif
		if( SENCERR_SCALE & err )
		{	/* A.080 : �X�P�[���s�b�`�ݒ�ُ�		*/
			ALMSetServoAlarm( AlmMngr, ALM_SCALE );
		}

		if( SENCERR_PRMDATA & err )
		{	/* A.CA0 : �G���R�[�_�p�����[�^�ُ� */
			ALMSetServoAlarm( AlmMngr, ALM_PGP );
		}

		if( SENCERR_BRCHNGFAIL & err )
		{	/* A.C93 : �G���R�[�_�ʐM�V�X�e���ُ� */
//			ALMSetServoAlarm( AlmMngr, ALM_ENCBR ); //@@CHECK ���󖢑Ή�
		}


		if(mask != FALSE)
		{
			/* �A���[���}�X�N���� */
			ALMSetMask( AlmMngr, ALM_PGCOM1    , TRUE );	/* A.C91 : �G���R�[�_�ʐM�ُ�(�����x)	*/
			ALMSetMask( AlmMngr, ALM_PGCOM2    , TRUE );	/* A.C92 : �G���R�[�_�ʐM�^�C�}�ُ�		*/
			ALMSetMask( AlmMngr, ALM_PRM       , TRUE );	/* A.040 : �p�����[�^�ݒ�ُ�			*/
			ALMSetMask( AlmMngr, ALM_DIVPRM    , TRUE );	/* A.041 : �����p���X�o�͐ݒ�ُ�		*/
			ALMSetMask( AlmMngr, ALM_UNMATCH   , TRUE );	/* A.050 : ���[�^/�T�[�{�e�ʃA���}�b�`	*/
			ALMSetMask( AlmMngr, ALM_OLF1      , TRUE );	/* A.710 : �ߕ��ׁi�u���ő啉�ׁj		*/
			ALMSetMask( AlmMngr, ALM_OLF2      , TRUE );	/* A.720 : �ߕ��ׁi�A���ő啉�ׁj		*/
			ALMSetMask( AlmMngr, ALM_PGP       , TRUE );	/* A.CA0 : �G���R�[�_�p�����[�^�ُ�		*/
			ALMSetMask( AlmMngr, ALM_PGECHO    , TRUE );	/* A.CB0 : �G���R�[�_�G�R�[�o�b�N�ُ�	*/
			ALMSetMask( AlmMngr, ALM_OSDIV     , TRUE );	/* A.511 : �����p���X�o�͉ߑ��x			*/
			ALMSetMask( AlmMngr, ALM_SCALE     , TRUE );	/* A.080 : �X�P�[���s�b�`�ݒ�ُ�		*/
			ALMSetMask( AlmMngr, ALM_PHASE     , TRUE );	/* A.C20 : �ʑ��댟�o					*/
			ALMSetMask( AlmMngr, ALM_POLESENSOR, TRUE );	/* A.C21 : �|�[���Z���T�ُ�				*/
			ALMSetMask( AlmMngr, ALM_VELPRM    , TRUE );	/* A.550 : �ō����x�ݒ�ُ�				*/
		}
	}
}


/****************************************************************************************************/
/*  Description : ���[�^&��ݺ��ޑI������(�S��)														*/
/*  Parameter   : UNI_PRM_STR UniPrms																*/
/*              : OPFBK_HNDL FbkOpt																	*/
/*              : INT AxisRscNum			�F����													*/
/*  Return      :																					*/
/****************************************************************************************************/ /* <S04D> */
static void SysInitSelectMotorEncoder( AXIS_HANDLE *AxisRscI, INT AxisRscNum )
{
	INT	 		ax_no;
	PRMDATA 	*Prm;
	BPRMDAT 	*Bprm;
	AXIS_HANDLE *AxisRsc;
	OPFBK_HNDL 	FbkOpt;
//	BOOL 		PrmError;/*<S0C7>*/
/*--------------------------------------------------------------------------------------------------*/
	for( ax_no = 0; ax_no < AxisRscNum; ax_no++ )
	{
		AxisRsc = &AxisRscI[ax_no];
		Prm = AxisRsc->UniPrms->Prm;
		Bprm = AxisRsc->UniPrms->Bprm;
		FbkOpt = AxisRsc->OptCardHdl->FbkOpt;

	/*----------------------------------------------------------------------------------------------*/
	/*		���[�^�I��(�����p)																		*/
	/*----------------------------------------------------------------------------------------------*/
		Bprm->MotSel = MOTSEL_YASKAWA;
	/*----------------------------------------------------------------------------------------------*/
	/*		���[�^�G���R�[�_�I��(�����p)															*/
	/*----------------------------------------------------------------------------------------------*/
		Bprm->EncSel = ENCSEL_SERIAL;

	/*----------------------------------------------------------------------------------------------*/
	/*			�t���N�^�C�v����																	*/
	/*----------------------------------------------------------------------------------------------*/
		if(Prm->syssw1 & 0x0001)		/* �t���N���[�Y�Ή��@��H			*/
		{/* �Ή��@��(SGD7S) */
			Bprm->FencType = TRUE;
		}
		else
		{/* ��Ή��@��(SGD7W)�FCh1��2���ڂ��g�p���邽�߁B */
			Bprm->FencType = FALSE;
		}
	/*----------------------------------------------------------------------------------------------*/
	/*			�t���N�G���R�[�_�g�p����															*/
	/*----------------------------------------------------------------------------------------------*/
		Bprm->FencUse = FALSE;
		if((Bprm->FencType == TRUE)	&&				/* �t���N�^�C�v�A���A							*/
		   ((Prm->b_prm2>>12) & 0x000F)	&&			/* �t���N�G���R�[�_�g�p�A���A					*/
		   (FbkOpt.conf.OpInfo.f.Connect == TRUE))	/* �t�B�[�h�o�b�N�I�v�V������Րڑ�����Ă���H	*/
		{
			Bprm->FencUse = TRUE;
		}
		
		Bprm->PsFulcSelect = FALSE;
		if((Bprm->FencUse == TRUE)&&( Prm->fcsel & 0x1000 ))
		{
			Bprm->PsFulcSelect = TRUE;				/* ���S�t���N����								*/
		}

	/*----------------------------------------------------------------------------------------------*/
	/*			�Z�~�N����:�t�B�[�h�o�b�N�I�v�V�������G���R�[�_�g�p����								*/
	/*----------------------------------------------------------------------------------------------*/
		if( (FbkOpt.conf.OpInfo.IfType == FBKOP_SCNV_MODULE) && (((Prm->b_prm2>>12) & 0x000F) == 0) &&
			(((Prm->b_prmB>>12) & 0x0F) == 1) )
		{
			Bprm->SencFbOptUse = TRUE;
		}
		else
		{
			Bprm->SencFbOptUse = FALSE;
		}
	
	/*----------------------------------------------------------------------------------------------*/
	/*			�t���N����:�t�B�[�h�o�b�N�I�v�V�������G���R�[�_�g�p���� 							*/
	/*----------------------------------------------------------------------------------------------*/
		if( (FbkOpt.conf.OpInfo.IfType == FBKOP_SCNV_MODULE) && (Bprm->FencUse == TRUE) )
		{
			FbkOpt.var.f.FullclosedUse = TRUE;
		}
		else
		{
			FbkOpt.var.f.FullclosedUse = FALSE;
		}
/*--------------------------------------------------------------------------------------------------*/
	}
	return;
}


//#define ENCODER_CONNECT_TIMEOUT		1500000		/* 1.5s */
#define ENCODER_CONNECT_TIMEOUT		1500		/* 1.5s *///�b��
//TODO:����ٽ�A��۸޴ݺ��ނ��l�����֐��߲�������遫
//TODO:Ӱ�ڽ���Ή�
/****************************************************************************************************/
/*  Description : �V���A���G���R�[�_�ڑ�����(�S��)													*/
/*  Parameter   : AXIS_HANDLE *AxisRscI																*/
/*              : INT AxisRscNum			�F����													*/
/*  Return      :																					*/
/****************************************************************************************************/
static void SysConnectEncoders( AXIS_HANDLE *AxisRscI, INT AxisRscNum )
{
	INT	 			ax_no;
#if 0/* <S04D>:�������ɍ폜 */
#if	(CSW_SVCF_FULC_MODE != CSW_FULC_NO_SUPPORT)
	INT				fslot, fch;
	MODULE_ID_INFO	*ModuleIDInfo;
	CMN_PRMDATA		*Cprm;
#endif
#endif
//	USHORT			d_clk1, d_clk2;
	ULONG			d_clk1;
	VULONG			d_clk, past_t;
	VLONG			all_err;
	USHORT			BaudRateSel; 		/* <S003> */
	BOOL			EncExchgSupported;	/* <S09C> */
	MENC_INIT_PRM	MencInitPrm;
	SENC_ERROR		err[MAX_AXIS_NUMBER];
//	SENC_ERROR		ferr[MAX_AXIS_NUMBER];	/* <S04D>:�������ɍ폜 */
	PRMDATA 		*Prm;
	BPRMDAT 		*Bprm;
	AXIS_HANDLE 	*AxisRsc;
	ASIC_HW_IF_STR	*encAsicHwReg;		/* <S04D> */
	ASIC_HW_IF_STR	*posCmpAsicHwReg;	/* <S04D> */
	OPFBK_HNDL 		*FbkOpt;			/* <S04D> */
	VFBTIMBASE		*Vfbtimbase;		/* <S067> */
	MOTLESSTST		*MotorLessTest;			/* <S1B7> */
	MOTOR_LESS		*MotorLessTestDatas;	/* <S1B7> */
	
	/* �G���R�[�_�֘A���W�X�^�A�N�Z�X�pUDL���W�X�^�A�h���X(0���ڂ����ʂŎg�p) *//* <S04D> */
	encAsicHwReg = AxisRscI->SvAsicRegs->AsicHwReg;

	/*----------------------------------------------------------------------------------------------*/
	/*		�V���A���G���R�[�_IF������																*/
	/*----------------------------------------------------------------------------------------------*/
	for( ax_no = 0; ax_no < AxisRscNum; ax_no++ )
	{
		AxisRsc = &AxisRscI[ax_no];
		Prm = AxisRsc->UniPrms->Prm;
		Bprm = AxisRsc->UniPrms->Bprm;
		FbkOpt = &AxisRsc->OptCardHdl->FbkOpt;			/* <S04D> */
		Vfbtimbase = &(AxisRsc->BaseLoops->Vfbtimbase);	/* <S065> */

		/*------------------------------------------------------------------------------------------*/
		/*		Initialize Motor Encoder Variables.													*/
		/*------------------------------------------------------------------------------------------*/
		MlibResetLongMemory( Vfbtimbase, sizeof( *Vfbtimbase )/4 );		/* ���x���Z�ϐ�				*//* <S067> */

		/*------------------------------------------------------------------------------------------*/
		/*		Initialize Motor Encoder channel.													*/
		/*------------------------------------------------------------------------------------------*/
#if CSW_BOARD_TYPE == SGD7S_TYPE /* <S0A2> */
		/* <S04D> �t�B�[�h�o�b�N�I�v�V�����Ή������ǉ� */
//		SencInitChannel(AxisRsc->MencV, 0, AxisRsc->SvAsicRegs->AsicHwReg);
		if(Bprm->SencFbOptUse != FALSE)
		{/* �t�B�[�h�o�b�N�I�v�V������p�����Z�~�N���[�Y�h����	 */
			/* Set Initialize Channel (̨����ޯ���߼�݂ł̾и�́ACh1���g�p����)	*/
//			posCmpAsicHwReg = (ASIC_HW_IF_STR*)((UCHAR*)AxisRsc->SvAsicRegs->AsicHwReg + ASIC_AXIS2_OFFSET);	/* <S056>	�萔��`���̕ύX */
			posCmpAsicHwReg = (ASIC_HW_IF_STR*)((UCHAR*)AxisRsc->SvAsicRegs->AsicHwReg + JL086_SVIP_OFS);		/* <S056>	�萔��`���̕ύX */

			SencInitChannel(AxisRsc->MencV, 1, encAsicHwReg, posCmpAsicHwReg);
			/* �G���R�[�_��񂨂�у��[�^�����R�s�[	*/
			AxisRsc->MencV->pActEncInfo = &FbkOpt->conf.EncInfo;
			AxisRsc->MencV->pActMotInfo = &FbkOpt->conf.MotInfo;

			/* �G���R�[�_�ʐM���x�ݒ�(Pn250.1�Őݒ肳��A�t�B�[�h�o�b�N�I�v�V�����ŒʐM�m�F����)	*/
			AxisRsc->MencV->PgBaudRate = FbkOpt->conf.PgBaudRate;
			BaudRateSel = FbkOpt->conf.PgBaudRate;

			/*------------------------------------------------------------------*/
			/*	�G���R�[�_�ʒu�␳�@�\�̏�����(�����x�`�F�b�N���x��(�b��l)�ݒ�)*/
			/*------------------------------------------------------------------*/
			SHal_InitEncPosCompAccLvl( posCmpAsicHwReg );
		}
		else
		{/* �ʏ�(�A���v�{�̂̃R�l�N�^)��p�����Z�~�N���[�Y�h���� */
			/* Set Initialize Channel (�ʏ�ł̾и�́A���ɍ��킹��Ch���g�p����)	*/
			posCmpAsicHwReg = encAsicHwReg;
			SencInitChannel(AxisRsc->MencV, ax_no, encAsicHwReg, posCmpAsicHwReg);
			/* �G���R�[�_��񂨂�у��[�^�����R�s�[	*/
			AxisRsc->MencV->pActEncInfo = &AxisRsc->MencV->EncInfo;
			AxisRsc->MencV->pActMotInfo = &AxisRsc->MencV->MotInfo;
			
			/* �G���R�[�_�ʐM���x�ݒ�(Pn250.0�Őݒ肳�ꂽ�l)	*/
			if( ((Prm->encbaudrate & 0x000F) == SENCBR_AUTO_SEL)
							|| ((Prm->encbaudrate & 0x000F) == SENCBR_4MBPS_SEL) )
			{/* Initialize Current PG Baud Rate (4MBps) */
				AxisRsc->MencV->PgBaudRate = SENC_COMRATE_4MBPS;
				BaudRateSel = SENC_COMRATE_4MBPS;
			}
			else /* Prm->encbaudrate == SENCBR_8MBPS_SEL */
			{/* Initialize Current PG Baud Rate (8MBps) */
				AxisRsc->MencV->PgBaudRate = SENC_COMRATE_8MBPS;
				BaudRateSel = SENC_COMRATE_8MBPS;
			}
			
			/*------------------------------------------------------------------*/
			/*	�G���R�[�_�ʒu�␳�@�\�̏�����(�����x�`�F�b�N���x��(�b��l)�ݒ�)*/
			/*------------------------------------------------------------------*//* <S004> */
			SHal_InitEncPosCompAccLvl( posCmpAsicHwReg );
		}
#else /* CSW_BOARD_TYPE == SGD7W_TYPE *//* <S0A2> */

		/* �ʏ�(�A���v�{�̂̃R�l�N�^)��p�����Z�~�N���[�Y�h���� *//* <S0E4> */
			/* Set Initialize Channel (�ʏ�ł̾и�́A���ɍ��킹��Ch���g�p����)	*/
//			posCmpAsicHwReg = encAsicHwReg;
			posCmpAsicHwReg = (ASIC_HW_IF_STR*)((UCHAR*)AxisRsc->SvAsicRegs->AsicHwReg);		/* <S056>	�萔��`���̕ύX */
			SencInitChannel(AxisRsc->MencV, ax_no, encAsicHwReg, posCmpAsicHwReg);
			/* �G���R�[�_��񂨂�у��[�^�����R�s�[	*/
			AxisRsc->MencV->pActEncInfo = &AxisRsc->MencV->EncInfo;
			AxisRsc->MencV->pActMotInfo = &AxisRsc->MencV->MotInfo;

			/* �G���R�[�_�ʐM���x�ݒ�(Pn250.0�Őݒ肳�ꂽ�l)	*/
			if( ((Prm->encbaudrate & 0x000F) == SENCBR_AUTO_SEL)
							|| ((Prm->encbaudrate & 0x000F) == SENCBR_4MBPS_SEL) )
			{/* Initialize Current PG Baud Rate (4MBps) */
				AxisRsc->MencV->PgBaudRate = SENC_COMRATE_4MBPS;
				BaudRateSel = SENC_COMRATE_4MBPS;
				}
			else /* Prm->encbaudrate == SENCBR_8MBPS_SEL */
			{/* Initialize Current PG Baud Rate (8MBps) */
				AxisRsc->MencV->PgBaudRate = SENC_COMRATE_8MBPS;
				BaudRateSel = SENC_COMRATE_8MBPS;
			}

			/*------------------------------------------------------------------*/
			/*	�G���R�[�_�ʒu�␳�@�\�̏�����(�����x�`�F�b�N���x��(�b��l)�ݒ�)*/
			/*------------------------------------------------------------------*//* <S004> */
			SHal_InitEncPosCompAccLvl( posCmpAsicHwReg );
//		}

#endif /* CSW_BOARD_TYPE == SGD7S_TYPE *//* <S0A2> */


#if 0 /*<S02D>*/
		/* @@�������K�v */
		/*------------------------------------------------------------------------------------------*/
		/*		�����p���X�o�͗L��Flag (�T���v���Ńn�[�h�F0���ڗL��)								*/
		/*------------------------------------------------------------------------------------------*/
		if( ax_no == 0 )
		{
#if 0 /* 2012.08.20 Y.Oka Make Mercury Base Software (SynqNet?) */
			AxisRsc->MencV->DivOutEnable = TRUE;
#else
			AxisRsc->MencV->DivOutEnable = FALSE;
#endif /* 2012.08.20 Y.Oka Make Mercury Base Software (SynqNet?) */
		}
#else/*<S02D>*/
		/*------------------------------------------------------------------------------------------*/
		/*	�����p���X�o�͗L��Flag�ݒ�																*/
		/*------------------------------------------------------------------------------------------*/
#if	(SVFSEL_DIVOUT_USE == TRUE)
		if( Bprm->FencUse != TRUE )
		{ /* �Z�~�N���[�Y����̏ꍇ */
			AxisRsc->MencV->DivInfo.DivOut = TRUE;
			AxisRsc->FencV->DivInfo.DivOut = FALSE;
		}
		else
		{ /* �t���N���[�Y����̏ꍇ */
			AxisRsc->FencV->DivInfo.DivOut = TRUE;
			AxisRsc->MencV->DivInfo.DivOut = FALSE;
		}
#else /* �����p���X�o�͋@�\�����̏ꍇ */
		AxisRsc->MencV->DivInfo.DivOut = FALSE;
		AxisRsc->FencV->DivInfo.DivOut = FALSE;
#endif	/*(SVFSEL_DIVOUT_USE == TRUE)*/
#endif	/*<S02D>*/

		err[ax_no] = SENCERR_PGCOM;
//		ferr[ax_no] = SENCERR_PGCOM;/* <S04D> */
	}


	/*----------------------------------------------------------------------*/
	/* �V���A���G���R�[�_�ڑ� */
	/*----------------------------------------------------------------------*/
//	d_clk = 0;/* <S0E4> */
	KlibRstLongTimer(&d_clk1);
	do
	{
//		d_clk1 = SHal_GetSysClock( AxisRscI[0].SvAsicRegs );

		all_err = SENCERR_SUCCESS;
		for( ax_no = 0; ax_no < AxisRscNum; ax_no++ )
		{
			AxisRsc = &AxisRscI[ax_no];
			Prm = AxisRsc->UniPrms->Prm;
			Bprm = AxisRsc->UniPrms->Bprm;

			if( err[ax_no] == SENCERR_PGCOM )
			{
/* <S1B7> �b�� �֐�������H >>>>> */
				/*----------------------------------------------------------------------*/
				/*	�A���[���}�X�N����													*/
				/*----------------------------------------------------------------------*/
				if( AxisRsc->MotorLessTest->MotorLessTestMode == TRUE )		/* ���[�^���X�e�X�g���̏���				*/
				{
					ALMSetMask(AxisRsc->AlmManager, ALM_PG1			, TRUE);	/* A.810:�G���R�[�_�o�b�N�A�b�v�A���[��	*/
					ALMSetMask(AxisRsc->AlmManager, ALM_PG3			, TRUE);	/* A.830:�G���R�[�_�o�b�e���A���[��		*/
					ALMSetMask(AxisRsc->AlmManager, ALM_PG4			, TRUE);	/* A.840:�G���R�[�_�f�[�^�A���[��		*/
					ALMSetMask(AxisRsc->AlmManager, ALM_PG5			, TRUE);	/* A.850:�G���R�[�_�I�[�o�X�s�[�h		*/
					ALMSetMask(AxisRsc->AlmManager, ALM_PG6			, TRUE);	/* A.860:�G���R�[�_�I�[�o�q�[�g			*/
					ALMSetMask(AxisRsc->AlmManager, ALM_SCL_ERR		, TRUE);	/* A.890:�G���R�[�_�X�P�[���G���[		*/
					ALMSetMask(AxisRsc->AlmManager, ALM_MOD_ERR		, TRUE);	/* A.891:�G���R�[�_���W���[���G���[		*/
					ALMSetMask(AxisRsc->AlmManager, ALM_PGCOM		, TRUE);	/* A.C90:�G���R�[�_�ʐM�ُ�				*/
					ALMSetMask(AxisRsc->AlmManager, ALM_PGCOM1		, TRUE);	/* A.C91:�G���R�[�_�ʐM�ُ�(�����x)		*/
					ALMSetMask(AxisRsc->AlmManager, ALM_PGCOM2		, TRUE);	/* A.C92:�G���R�[�_�ʐM�^�C�}�ُ�		*/
					ALMSetMask(AxisRsc->AlmManager, ALM_PGECHO		, TRUE);	/* A.CB0:�G���R�[�_�G�R�[�o�b�N�ُ�		*/
					ALMSetMask(AxisRsc->AlmManager, ALM_POLESENSOR	, TRUE);	/* A.C21:�|�[���Z���T�ُ�				*/
					ALMSetMask(AxisRsc->AlmManager, ALM_PHASEANGLE	, TRUE);	/* A.C22:�ʑ����s��v					*/
				}
/* <<<<< <S1B7> */

				/*------------------------------------------------------------------------------*/
				/*		Get Parameters related to Motor Encoder									*/
				/*------------------------------------------------------------------------------*/
				MencInitPrm.MencP = &(Prm->MencP);
				MencInitPrm.b_prm2 = Prm->b_prm2;
				MencInitPrm.b_prm80 = Prm->b_prm80;
				MencInitPrm.mtlmt = Prm->mtlmt;
				MencInitPrm.scalepitch2l = Prm->scalepitch2l;
				MencInitPrm.spgfail = Prm->spgfail;
				MencInitPrm.syssw1 = Prm->syssw1;

 /* <S003> */
				/*------------------------------------------------------------------------------*/
				/*		�J�n����(P0���[�h�ݒ�)													*/
				/*------------------------------------------------------------------------------*/
//				SencChangeP0Mode( AxisRsc->MencV );			/* <S0F8> */
				SencInitChangeP0Mode( AxisRsc->MencV );		/* <S0F8> */

				/*------------------------------------------------------------------------------*/
				/*		Initialize Motor Encoder												*/
				/* Pn250.0�̒l�Ɋ�ăG���R�[�_�ʐM���x��ݒ肷��B							*/
				/*------------------------------------------------------------------------------*/
#if 0/* <S04D> */
				if( (Prm->encbaudrate == SENCBR_AUTO_SEL) || (Prm->encbaudrate == SENCBR_4MBPS_SEL) )
				{
					/* Initialize Current PG Baud Rate (4MBps) */
					AxisRsc->MencV->PgBaudRate = SENC_COMRATE_4MBPS;
					BaudRateSel = SENC_COMRATE_4MBPS;
				}
				else /* Prm->encbaudrate == SENCBR_8MBPS_SEL */
				{
					/* Initialize Current PG Baud Rate (8MBps) */
					AxisRsc->MencV->PgBaudRate = SENC_COMRATE_8MBPS;
					BaudRateSel = SENC_COMRATE_8MBPS;
				}
#endif
				/* Motor Encoder Communication Initialize */
				err[ax_no] = SencInitMotorEncoder( AxisRsc->MencV,
												   &MencInitPrm,
												   AxisRsc->SvAsicRegs->AsicHwReg,
												   FbkOpt,/* <S04D>:FbkOpt�ǉ� */
												   BaudRateSel,
												   Bprm->SencFbOptUse );		/* <S04D>:SencFbOptUse�ǉ� */

/* <S1B7> >>>>> */
/* TODO: �����Ƀ��[�^���X�e�X�g�p�̏������W�߂Ă��邪�A���Ȃ����m�F����K�v���� */
				/* MencV->EncConnect�̍Đݒ� (�֐�������H) */
				MotorLessTest = AxisRsc->MotorLessTest;
				MotorLessTestDatas = &(MotorLessTest->MotorLessTestDatas);

				if( err[ax_no] == SENCERR_SUCCESS )						/* Check Return Code					*/
				{
					MotorLessTestDatas->SemiEncConnect = TRUE;			/* ���[�^���X�e�X�g�p�G���R�[�_�ڑ�Flag	*/

					/* ���[�^���X�e�X�g���G���R�[�_�ڑ����A�G���R�[�_�֘A�A���[�����}�X�N����		*/
					/* �A���AA.810��Fn008���s�ɂăN���A�AA.830�̓}�X�N���Ă��AA.810�������������	*/
					/* ����肷�邽�߂ɏo�����ق����ǂ��̂Ń}�X�N���Ȃ�								*/
					if( MotorLessTest->MotorLessTestMode == TRUE )		/* �G���R�[�_�ڑ�&&���[�^���X�̏���		*/
					{
						ALMSetMask(AxisRsc->AlmManager, ALM_PG2, TRUE);	/* A.820 : �G���R�[�_�T���`�F�b�N�A���[��	*/
						ALMSetMask(AxisRsc->AlmManager, ALM_PG4, TRUE);	/* A.840 : �G���R�[�_�f�[�^�A���[��		*/
						ALMSetMask(AxisRsc->AlmManager, ALM_PG5, TRUE);	/* A.850 : �G���R�[�_�I�[�o�X�s�[�h	 	*/
						ALMSetMask(AxisRsc->AlmManager, ALM_PG6, TRUE);	/* A.860 : �G���R�[�_�I�[�o�q�[�g		*/
					}
				}
				else if( MotorLessTest->MotorLessTestMode == FALSE )	/* �G���R�[�_���ڑ����̏���				*/
				{
					/* �����Ȃ� */
				}
				else													/* ���[�^���X�e�X�g���̏���				*/
				{
					AxisRsc->MencV->EncConnect = TRUE;					/* �G���R�[�_�ڑ�Flag					*/
					MotorLessTestDatas->SemiEncConnect = FALSE;			/* ���[�^���X�e�X�g�p�G���R�[�_�ڑ�Flag	*/
					ALMSetMask(AxisRsc->AlmManager, ALM_PGP, TRUE);		/* A.CA0:�G���R�[�_�p�����[�^�ُ�		*/
					ALMSetMask(AxisRsc->AlmManager, ALM_PG2, TRUE);		/* A.820:�G���R�[�_�T���`�F�b�N�A���[��	*/
				}

				/*------------------------------------------------------------------------------*/
				/*		���[�^���X�e�X�g���[�h�p�����[�^�ǂݍ��ݏ���							*/
				/*------------------------------------------------------------------------------*/
				if( (MotorLessTest->MotorLessTestMode == TRUE)				/* ���[�^���X�e�X�g���[�h			*/
						&& (MotorLessTestDatas->SemiEncConnect == FALSE))	/* �G���R�[�_���ڑ�					*/
				{
					ApiReadMotPrmMotorLessTestMode( AxisRsc->MencV, Prm );							/* PnFxx �p�����[�^��������				*/
					LpxSetSencInfoMotorLessTestMode( AxisRsc->MencV, &(Prm->MencP), Prm->b_prm2 );	/* �G���R�[�_���ݒ�					*/

					/*--------------------------------------------------------------------------*/
					/*		�G���R�[�_�x�[�X�p�����[�^�v�Z����									*/
					/*--------------------------------------------------------------------------*/
					MencInitPrm.mtlmt = Prm->mtlmt;
					SencCalcBaseParameters( AxisRsc->MencV, &MencInitPrm );
				}

				/* �G���R�[�_IF�֐��|�C���^�ݒ�͂����Ŏ��{����	*/
				if( MotorLessTest->MotorLessTestMode == FALSE )
				{
					/* �ʏ펞 */
					AxisRsc->BaseLoops->SencReadPosition = SencReadPosition;
				}
				else
				{
					if( AxisRsc->MencV->AxisMotType == MOTTYPE_ROTARY )
					{
						/* ���[�^���X�e�X�g�F��]�^ */
						AxisRsc->BaseLoops->SencReadPosition = RmxReadMotorLessPosition;
					}
					else
					{
						/* ���[�^���X�e�X�g�F���j�A�^ */
						AxisRsc->BaseLoops->SencReadPosition = LmxReadMotorLessPosition;
					}
				}
/* <<<<< <S1B7> */

				/*------------------------------------------------------------------------------*/
				/*		Exchange Encoder Baud Rate												*/
				/*------------------------------------------------------------------------------*/
				if( err[ax_no] == SENCERR_SUCCESS )
				{ /* Encorder is Connected */
//					if(Prm->encbaudrate == SENCBR_AUTO_SEL ) /* <S04D> */
					if( ((Bprm->SencFbOptUse == FALSE)&&(Prm->encbaudrate == SENCBR_AUTO_SEL ))
						||((Bprm->SencFbOptUse != FALSE)&&(FbkOpt->conf.PgBaudRateType == SENCBR_AUTO_SEL)) )
					{ /* 8MBps�����؂�ւ��v������ && �G���R�[�_�ڑ��G���[�Ȃ�					*/
						err[ax_no] |= SencExchngBaudrate( AxisRsc->MencV,
														  &MencInitPrm,
														  AxisRsc->SvAsicRegs->AsicHwReg,
														  SENC_COMRATE_8MBPS,
														  &EncExchgSupported);/* <S09C> */
//<S076>						if( err[ax_no] == SENCERR_PGCOM )
//						if( err[ax_no] == SENCERR_SUCCESS )				/* <S076> */
						if( EncExchgSupported == TRUE )					/* <S09C> */
						{ /* Exchange Baud Rate was Success */
							AxisRsc->MencV->PgBaudRate = SENC_COMRATE_8MBPS;
						}
						else
						{
							AxisRsc->MencV->PgBaudRate = SENC_COMRATE_4MBPS;
						}
					}
					else if( Prm->encbaudrate == SENCBR_8MBPS_SEL )
					{
						; /* �ʐM���x�؂�ւ��Ȃ� */
					}
				}
 /* <S003> */

				/*------------------------------------------------------------------------------*/
				/*		Detect and Set Alarm													*/
				/*------------------------------------------------------------------------------*/
				SysSetMotorEncoderError( err[ax_no], AxisRsc->AlmManager );
				all_err |= err[ax_no];							/* Set Error Info.				*/
			}
#if 0/* <S04D>:�{�֐��Ƃ͕ʂ̊֐�(�t���N�p�̏������֐�)�Ŏ��{�B�������ɂ͍폜 */
#if	(CSW_SVCF_FULC_MODE != CSW_FULC_NO_SUPPORT)
			/* Fully-closed Encoder */
			if((ferr[ax_no] == SENCERR_PGCOM) && (Bprm->FencUse != FALSE))
			{
#if 0 /*<S02D>*/
				ferr[ax_no] = FencInitEncoder(AxisRsc->FencV,
		                                      &(Prm->MencP),
		                                      Prm->b_prm2,
		                                      Prm->fcplsml);
#else /*<S02D>*/
				ferr[ax_no] = FencInitEncoder(AxisRsc->FencV,
		                                      &(Prm->MencP),
		                                      Prm->b_prm2,
		                                      Prm->fcplsml,
											  Bprm->RvsDir);
#endif /*<S02D>*/

				all_err |= ferr[ax_no];
			}
#endif
#endif
		}

		/*--------------------------------------------------------------------------------------*/
		/*		�o�ߎ��Ԍv�� & �^�C���A�E�g����													*/
		/*--------------------------------------------------------------------------------------*/
//		d_clk2 = SHal_GetSysClock( AxisRscI[0].SvAsicRegs );	/* <S0E4> */
//		d_clk += (ULONG)SHal_Get_dClock( d_clk1, d_clk2 );
//		past_t = (VULONG)SHal_ClockToTime( (USHORT)(d_clk/1000) );
		past_t = KlibGetLongTimerMs( d_clk1 );
	}while((ENCODER_CONNECT_TIMEOUT > past_t) && (all_err != SENCERR_SUCCESS) );

	for( ax_no = 0; ax_no < AxisRscNum; ax_no++ )/* <S04D>:��ѱ�Č�ړ� */
	{
		AxisRsc = &AxisRscI[ax_no];
		/*----------------------------------------------------------------------------------*/
		/*		P1���[�h�ݒ�(��L�����̐���/���s�Ɍ��炸P1���[�h(62.5us�ʐM����)�ɕύX		*/
		/*----------------------------------------------------------------------------------*/
		SencChangeP1Mode( AxisRsc->MencV );
	}
	
#if 0/* <S04D>:�{�֐��Ƃ͕ʂ̊֐�(�t���N�p�̏������֐�)�Ŏ��{�B �������ɂ͍폜 */
	/*----------------------------------------------------------------------*/
	/* �V���A���G���R�[�_�p�����[�^�����ݒ� */
	/*----------------------------------------------------------------------*/
	for( ax_no = 0; ax_no < AxisRscNum; ax_no++ )
	{
		AxisRsc = &AxisRscI[ax_no];
		Bprm = AxisRsc->UniPrms->Bprm;

		/* Fully-closed Encoder																	*/
		if( (Bprm->FencUse != FALSE) && (ferr[ax_no] != SENCERR_SUCCESS) )
		{	
			/* A.CF1 : �G���R�[�_�ʐM�ُ�														*/
			ALMSetServoAlarm( AxisRsc->AlmManager, ALM_FPGCOM );

		}
	}
#endif

}


#if	(CSW_SVCF_FULC_MODE != CSW_FULC_NO_SUPPORT)/* <S04D> */
/****************************************************************************************************/
/*  Description : �t���N���[�Y �V���A���G���R�[�_�ڑ�����											*/
/* 				: �{�֐��ł́A���ԍ���0����/1���ڂƂ���B											*/
/* 				------------------------------------------------------------------------------------*/
/* 				: �t���N���[�Y�h����́ACh1���g�p���邽�߁A�����A���v��1���ڂƏd������B			*/
/* 				: �����A���v�ł̓t���N���[�Y�h��������{���Ȃ��B									*/
/* 				: 	->	0���ڂ̂݁BSGD7W�́A���߲ٽ����ɂ���ٸ�𖳌��ɐݒ�ς�						*/
/* 				------------------------------------------------------------------------------------*/
/* 				: HOST CPU�̕ϐ��́A0���ڂ̗̈�ɐݒ肷��B											*/
/* 				: �V���A���G���R�[�_�̑���M�Ɋւ���UDL�̐ݒ�́A0���ڂɍs���B						*/
/* 				: �ʒu�␳���W���[���Ɋւ���UDL�̐ݒ�́A1���ڂɍs��								*/
/* 				------------------------------------------------------------------------------------*/
/*  Parameter   : AXIS_HANDLE *AxisRscI																*/
/*  Return      :																					*/
/****************************************************************************************************//* <S04D> */
static void SysConnectFcEncoders( AXIS_HANDLE *AxisRsc )
{
//	INT				fslot, fch;		/*<S0C7*/
//	MODULE_ID_INFO	*ModuleIDInfo;	/*<S0C7*/
//	CMN_PRMDATA		*Cprm;			/*<S0C7*/
	
	USHORT			d_clk1, d_clk2;
	VULONG			d_clk, past_t;
	BOOL			EncExchgSupported;/*<S09C>*/
	SENC_ERROR		ferr;
	USHORT			BaudRateSel;
	MENC_INIT_PRM	MencInitPrm;
	PRMDATA 		*Prm;
	BPRMDAT 		*Bprm;
//	OPFBK_HNDL 		*FbkOpt;		/*<S0C7*/
	ASIC_HW_IF_STR	*encAsicHwReg;
	ASIC_HW_IF_STR	*posCmpAsicHwReg;
	MOTLESSTST		*MotorLessTest;			/* ���[�^���X�e�X�g�p�f�[�^�ւ̃|�C���^	*//*<S1B7>*/
	MOTOR_LESS		*MotorLessTestDatas;	/* �G���R�[�_IF�p�f�[�^�ւ̃|�C���^		*//*<S1B7>*/

	Prm = AxisRsc->UniPrms->Prm;
	Bprm = AxisRsc->UniPrms->Bprm;
	ferr = SENCERR_PGCOM;
	MotorLessTest = AxisRsc->MotorLessTest;						/*<S1B7>*/
	MotorLessTestDatas = &(MotorLessTest->MotorLessTestDatas);	/*<S1B7>*/
/*--------------------------------------------------------------------------------------------------*/
/*		Initialize Motor Encoder channel.															*/
/* �t���N���[�Y�h�G���R�[�_�ł́ACh1���g�p����B													*/
/* ��{�ݒ�́A0���ڂ�UDL�̈�𗘗p���邪�A�ʒu�␳���W���[���֘A��1���ڂ̗̈�𗘗p����B			*/
/*--------------------------------------------------------------------------------------------------*/
	/* Set Initialize Channel */
	encAsicHwReg = AxisRsc->SvAsicRegs->AsicHwReg;
//	posCmpAsicHwReg = (ASIC_HW_IF_STR*)((UCHAR*)AxisRsc->SvAsicRegs->AsicHwReg + ASIC_AXIS2_OFFSET);	/* <S056>	�萔��`���̕ύX */
	posCmpAsicHwReg = (ASIC_HW_IF_STR*)((UCHAR*)AxisRsc->SvAsicRegs->AsicHwReg + JL086_SVIP_OFS);		/* <S056>	�萔��`���̕ύX */
	SencInitChannel(AxisRsc->FencV, 1, encAsicHwReg, posCmpAsicHwReg);

	/*----------------------------------------------------------------------------------------------*/
	/*	�G���R�[�_�ʒu�␳�@�\�̏�����(�����x�`�F�b�N���x��(�b��l)�ݒ�)							*/
	/*----------------------------------------------------------------------------------------------*/
	SHal_InitEncPosCompAccLvl( posCmpAsicHwReg );

#if 0
/*--------------------------------------------------------------------------------------------------*/
/*	�����p���X�o�͑I��Flag�̐ݒ�																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( (SVFSEL_DIVOUT_USE != 0) && (Kprm.f.DivoutUse == TRUE) )
	{
		FencV->DivOut = TRUE;								/* �t���N�G���R�[�_�����o�͑I��ON		*/
	}
	else
	{
		FencV->DivOut = FALSE;								/* �t���N�G���R�[�_�����o�͑I��OFF		*/
	}
#endif
//#if 0 <S1B7> ���[�^���X�e�X�g���[�h�Ή�
/*--------------------------------------------------------------------------------------------------*/
/*		�A���[���}�X�N����																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( MotorLessTest->MotorLessTestMode == TRUE )
	{/* ���[�^���X�e�X�g���̏���		*/
	/*----------------------------------------------------------------------------------------------*/
	/*	�A���[���}�X�N����																			*/
	/*----------------------------------------------------------------------------------------------*/
		ALMSetMask(AxisRsc->AlmManager, ALM_FPGCOM		, TRUE);	/* A.CF1 : �e�o�f�ʐM�ُ�				*/
		ALMSetMask(AxisRsc->AlmManager, ALM_FPGCOMTIMER	, TRUE);	/* A.CF2 : �e�o�f�ʐM�^�C�}�ُ�			*/
		ALMSetMask(AxisRsc->AlmManager, ALM_FPGMODULE	, TRUE);	/* A.8A1 : �e�o�f���W���[���ُ�			*/
		ALMSetMask(AxisRsc->AlmManager, ALM_FPGSEN		, TRUE);	/* A.8A2 : �e�o�f�Z���T�ُ�(�C���N��)	*/
		ALMSetMask(AxisRsc->AlmManager, ALM_FPGPOS		, TRUE);	/* A.8A3 : �e�o�f�|�W�V�����ُ�(�A�u�\)	*/
		ALMSetMask(AxisRsc->AlmManager, ALM_FPGOVSPD	, TRUE);	/* A.8A5 : �e�o�f���޽�߰�ވُ�			*/
		ALMSetMask(AxisRsc->AlmManager, ALM_FPGOVHEAT	, TRUE);	/* A.8A6 : �e�o�f����˰Ĉُ�			*/
		ALMSetMask(AxisRsc->AlmManager, ALM_FPGFSCALE	, TRUE);	/* A.8A0 : �e�o�f�X�P�[���ُ�			*/
	}
//#endif

	/*------------------------------------------------------------------------------*/
	/*		Get Parameters related to Motor Encoder									*/
	/*------------------------------------------------------------------------------*/
	MencInitPrm.MencP = &(Prm->MencP);
	MencInitPrm.b_prm2 = Prm->b_prm2;
	MencInitPrm.b_prm80 = Prm->b_prm80;
	MencInitPrm.mtlmt = Prm->mtlmt;
	MencInitPrm.scalepitch2l = Prm->scalepitch2l;
	MencInitPrm.fcplsml = Prm->fcplsml;
	MencInitPrm.spgfail = Prm->spgfail;
	/*----------------------------------------------------------------------------------------------*/
	/*		�J�n����																				*/
	/*----------------------------------------------------------------------------------------------*/
//	SencChangeP0Mode( AxisRsc->FencV );			/* <S0F8> */
	SencInitChangeP0Mode( AxisRsc->FencV );		/* <S0F8> */

	/*------------------------------------------------------------------------------*/
	/*		Initialize Motor Encoder												*/
	/* Pn250.1�̒l�Ɋ�ăG���R�[�_�ʐM���x��ݒ肷��B							*/
	/*------------------------------------------------------------------------------*/
	if( (((Prm->encbaudrate >> 4) & 0x000F) == SENCBR_AUTO_SEL)
							|| (((Prm->encbaudrate >> 4) & 0x000F) == SENCBR_4MBPS_SEL) )
	{
		/* Initialize Current PG Baud Rate (4MBps) */
		AxisRsc->FencV->PgBaudRate = SENC_COMRATE_4MBPS;
		BaudRateSel = SENC_COMRATE_4MBPS;
	}
	else /* Prm->encbaudrate == SENCBR_8MBPS_SEL */
	{
		/* Initialize Current PG Baud Rate (8MBps) */
		AxisRsc->FencV->PgBaudRate = SENC_COMRATE_8MBPS;
		BaudRateSel = SENC_COMRATE_8MBPS;
	}

	d_clk = 0;
	do
	{
		d_clk1 = SHal_GetSysClock( AxisRsc->SvAsicRegs );
		if( ferr == SENCERR_PGCOM )
		{
			ferr = FencInitEncoder( AxisRsc->FencV,
								 	  &MencInitPrm,
									   AxisRsc->SvAsicRegs->AsicHwReg,
									   &AxisRsc->OptCardHdl->FbkOpt,
									   BaudRateSel,
									   Bprm->SencFbOptUse,
									   Bprm->RvsDir );
		}
		/*------------------------------------------------------------------------------*/
		/*		Exchange Encoder Baud Rate												*/
		/*------------------------------------------------------------------------------*/
		if( ferr == SENCERR_SUCCESS )
		{ /* Encorder is Connected */
			if( Prm->encbaudrate == SENCBR_AUTO_SEL )
			{/* 8MBps�����؂�ւ��v������ && �G���R�[�_�ڑ��G���[�Ȃ�					*/
				ferr |= SencExchngBaudrate( AxisRsc->FencV,
												  &MencInitPrm,
												  AxisRsc->SvAsicRegs->AsicHwReg,
												  SENC_COMRATE_8MBPS,
												  &EncExchgSupported);/* <S09C> */
				if( ferr == SENCERR_PGCOM )
				{/* Exchange Baud Rate was Success */
					AxisRsc->FencV->PgBaudRate = SENC_COMRATE_8MBPS;
				}
			}
			else if( Prm->encbaudrate == SENCBR_8MBPS_SEL )
			{
				; /* �ʐM���x�؂�ւ��Ȃ� */
			}
		}
		/*--------------------------------------------------------------------------------------*/
		/*		�o�ߎ��Ԍv�� & �^�C���A�E�g����													*/
		/*--------------------------------------------------------------------------------------*/
		d_clk2 = SHal_GetSysClock( AxisRsc->SvAsicRegs );
		d_clk += (ULONG)SHal_Get_dClock( d_clk1, d_clk2 );
		past_t = (VULONG)SHal_ClockToTime( (USHORT)(d_clk/1000) );
		
	}while((ENCODER_CONNECT_TIMEOUT > past_t) && (ferr != SENCERR_SUCCESS) );

/* <S1B7> >>>>> */
	/*----------------------------------------------------------------------------------*/
	/*		���[�^���X�e�X�g���[�h�G���R�[�_���ݒ菈��								*/
	/*----------------------------------------------------------------------------------*/
	if( AxisRsc->FencV->EncConnect == TRUE )
	{
		MotorLessTestDatas->FullEncConnect = TRUE;
	}
	else if( MotorLessTest->MotorLessTestMode == TRUE )
	{
		MotorLessTestDatas->FullEncConnect = FALSE;
	}

	/*----------------------------------------------------------------------------------*/
	/*		���[�^���X�e�X�g���[�h�G���R�[�_���ݒ菈��								*/
	/*----------------------------------------------------------------------------------*/
	if( (MotorLessTest->MotorLessTestMode)						/* ���[�^���X�e�X�g���[�h		*/
			&& (MotorLessTestDatas->FullEncConnect == FALSE))	/* �G���R�[�_���ڑ�				*/
	{
		ApiReadFencPrmMotorLessTestMode( &(Prm->MencP) );					/* PnFxx �p�����[�^��������		*/
		LpxSetFencInfoMotorLessTestMode( AxisRsc->FencV, &(Prm->MencP) );	/* �G���R�[�_���ݒ�			*/
	}

	/*----------------------------------------------------------------------------------*/
	/*		�G���R�[�_�h�e�֐��|�C���^�ݒ�												*/
	/*----------------------------------------------------------------------------------*/
	if( MotorLessTest->MotorLessTestMode == FALSE )
	{
		AxisRsc->BaseLoops->FencReadPosition = FencReadPosition;
	}
	else
	{
		AxisRsc->BaseLoops->FencReadPosition = LpxReadMotorLessFencPosition;
	}
/* <<<<< <S1B7> */

	/*----------------------------------------------------------------------------------*/
	/*		P1���[�h�ݒ�(��L�����̐���/���s�Ɍ��炸P1���[�h(62.5us�ʐM����)�ɕύX		*/
	/*----------------------------------------------------------------------------------*/
	SencChangeP1Mode( AxisRsc->FencV );
	
	/* Fully-closed Encoder																	*/
	if(ferr != SENCERR_SUCCESS)
	{	
		/* A.CF1 : �G���R�[�_�ʐM�ُ�														*/
		ALMSetServoAlarm( AxisRsc->AlmManager, ALM_FPGCOM );
	}

}
#endif/* CSW_SVCF_FULC_MODE != CSW_FULC_NO_SUPPORT*/
/*****************************************************************************
*  Description :�����p�����[�^�v�Z(���P��)
*  Parameter   :
*  Return      :
*****************************************************************************/
static void SysInitAxisParameters( AXIS_HANDLE *AxisRsc, BOOL UnitCommonCalc )
{
	LONG 			rc;
	CMN_PRMDATA		*Cprm;
	PRMDATA 		*Prm;
	BPRMDAT 		*Bprm;
	REGCFG_PRM		RegPrm;				/* <S0F4> */
	MAICRO_CFGPRM	uCfgPrm;
	SVIP_INIT_PRM	SvipInitPrm;
	SVIP_INIT_PCOMP SvipInitPcomp;		/* <S004> */
//	MICRO_SA_IPRMS	MicroSAIprm;
	BPRM_INICFG		BprmCfg;
	LONG			alm;
#if 0/* �b�菈�� */
	DIVSET_ERROR	div_err;
#endif
	DIVSET_ERROR	div_err;			/*<S02D>*/
#ifndef CSW_NO_SERVO_ASICS
	SENC_ERROR		err;
	MENC_REF_PRM	MencRefPrm;
#endif
	CARRCHNG		*CarrerChange;		/* ���[�^�L�����A���g���؂�ւ��Ή�<S117> */

#if 0	/* �L�����A���g���̓���͍s�Ȃ�Ȃ����ߕs�v<S11E> */
#if ( defined(CSW_SGD7W_TYPE) || defined(CSW_COMMON_CARRIER) )		/* ���[�^�L�����A���g���؂�ւ��Ή�<S117> */
	AXIS_IF	*AxisIf;
	AxisIf = AxisRsc->AxisIf;
#endif	/* #if ( defined(CSW_SGD7W_TYPE) || defined(CSW_COMMON_CARRIER) ) */
#endif		/*#if 0	 �L�����A���g���̓���͍s�Ȃ�Ȃ����ߕs�v<S11E> */

	Cprm = AxisRsc->UniPrms->Cprm;
	Prm = AxisRsc->UniPrms->Prm;
	Bprm = AxisRsc->UniPrms->Bprm;

	CarrerChange = &AxisRsc->BaseLoops->CarrerChange;		/* ���[�^�L�����A���g���؂�ւ��Ή�<S117> */

	/*----------------------------------------------------------------------*/
	/* �e�ʕʃ��j�A�����p�����[�^�ݒ� */
	/*----------------------------------------------------------------------*/
	if( AxisRsc->MencV->AxisMotType == MOTTYPE_LINEAR )
	{
		Prm->dq_sw = Prm->dq_sw_L;
		Prm->TunLessGain = Prm->TunLessGain_L;
		Prm->TunLessStiff = Prm->TunLessStiff_L;
	}

	/*----------------------------------------------------------------------*/
	/* ��{�p�����[�^�v�Z */
	/*----------------------------------------------------------------------*/
	BprmCfg.ratb2l = Prm->ratb2l;			/* Pn20E : �d�q�M�A��(���q)				*/
	BprmCfg.rata2l = Prm->rata2l;			/* Pn210 : �d�q�M�A��(����)				*/
	BprmCfg.maxvel_l = Prm->maxvel_l;		/* Pn385 : ���[�^�ō����x(���j�A) 		*/
	BprmCfg.styp = Prm->styp;				/* PnE11 : �T�[�{�`��/���͓d���R�[�h	*/
	BprmCfg.ampw = Prm->ampw;				/* PnE12 : �T�[�{�e��					*/
	BprmCfg.imaxs = Prm->imaxs;				/* PnE15 : �T�[�{�ő�d��				*/
	BprmCfg.dq_sw2= Prm->dq_sw2;			/* PnE39 : �d������X�C�b�`2			*/
	BprmCfg.ratmt2_os = Prm->ratmt2_os;		/* PnE62 : �ߕ��׌��o���Ԏ���2/OS���x��	*/
	BprmCfg.vrefgn = Prm->vrefgn;			/* Pn300 : ���x�w�ߓ��̓Q�C��			*/
	BprmCfg.gadjv = Prm->gadjv;				/* PnE55 : ���x�w�߃Q�C����				*/

	alm = BprmCalcBaseParameters( Bprm,
								  &Prm->MencP,
								  &BprmCfg,
								  AxisRsc->MencV, AxisRsc->FencV );
	if(alm != BPRM_NOALM)
	{
		if(alm & BPRM_NOTSUPPORT)			/* A.051:���i���T�|�[�g�A���[��			*/
		{
			ALMSetServoAlarm(AxisRsc->AlmManager, ALM_NOTSUPPORT);
		}
		if(alm & BPRM_VELPRM)				/* A.550:�ō����x�ݒ�ُ�				*/
		{
			ALMSetServoAlarm(AxisRsc->AlmManager, ALM_VELPRM);
		}
	}

	/*----------------------------------------------------------------------*/
	/* ���[�^�e�ʃ`�F�b�N */
	/*----------------------------------------------------------------------*/
//	SysCheckAmpMotType(AxisRsc); /* <S09A> */

	/*----------------------------------------------------------------------*/
	/* �����p�����[�^�v�Z */
	/*----------------------------------------------------------------------*/
	if( UnitCommonCalc != FALSE )
	{
		/* <S0F4> */
		RegPrm.regres = AxisRsc->UniPrms->Cprm->regres;
		RegPrm.exreger = AxisRsc->UniPrms->Cprm->exreger;
		RegPrm.reger = AxisRsc->UniPrms->Cprm->reger;
		RegPrm.wre = AxisRsc->UniPrms->Cprm->regonlvl;
		RegPrm.regonlvl = AxisRsc->UniPrms->Cprm->regonlvl;
		RegPrm.ConvW = AxisRsc->UniPrms->Cprm->ConvW;

		/* �񐶓d�͉��Z�Q�C���v�Z */
		IprmcalRegenePowerGain( &(AxisRsc->PowerManager->RegeneCtrl),
								Bprm,
								&RegPrm );									  /* <S0F4> */
//								AxisRsc->UniPrms->Cprm->regonlvl,			  /* <S0F4> */
//								AxisRsc->UniPrms->Cprm->reger,				  /* <S0F4> */
//								AxisRsc->UniPrms->Cprm->ConvW);				  /* <S0F4> */
		IprmcalMainCommon( AxisRsc );
	}
	IprmcalMainAxis( AxisRsc );

	/*----------------------------------------------------------------------*/
	/* ���[�^�e�ʃ`�F�b�N */
	/*----------------------------------------------------------------------*/
	SysCheckAmpMotType(AxisRsc); /* <S09A> */

	/*----------------------------------------------------------------------*/
	/*  ASIC Micro-Program Parameter Calculation */
	/*----------------------------------------------------------------------*/
	uCfgPrm.imaxs = Prm->imaxs;					/* PnE15 : �T�[�{�p�b�N�ő�d��						*/
	uCfgPrm.ignd = Prm->ignd;					/* PnE20 : �d�����[�v�Q�C��(D)						*/
	uCfgPrm.ignq = Prm->ignq;					/* PnE21 : �d�����[�v�Q�C��(Q)						*/
	uCfgPrm.kid = Prm->kid;						/* PnE22 : �d�����[�v�ϕ����萔(D)					*/
	uCfgPrm.kiq = Prm->kiq;						/* PnE23 : �d�����[�v�ϕ����萔(Q)					*/
	uCfgPrm.limintd = Prm->limintd;				/* PnE24 : �d�����[�v�ϕ������l(D)					*/
	uCfgPrm.limintq = Prm->limintq;				/* PnE25 : �d�����[�v�ϕ������l(Q)					*/
	uCfgPrm.ifil = Prm->ifil;					/* PnE26 : �d�����[�v�t�B���^���萔					*/
	uCfgPrm.shant = Prm->shant;					/* PnE28 : �d�����o�Q�C��							*/
	uCfgPrm.pwmf = Prm->pwmf;					/* PnE2C : �o�v�l���g��								*/
	uCfgPrm.dq_sw = Prm->dq_sw;					/* PnE2F : �c�p�p�X�C�b�`							*/
	uCfgPrm.vrevgn = Prm->vrevgn;				/* PnE2A : �d���␳�Q�C��							*/
	uCfgPrm.lmdata = Prm->lmdata;				/* PnE2B : �d�������l								*/
	uCfgPrm.odt_c = Prm->odt_c;					/* PnE2D : �I���f�B���C����, �I���f�B���C�⏞�萔	*/
	uCfgPrm.odlv_slp = Prm->odlv_slp;			/* PnE2E : �I���f�B���C�⏞�ύX���x�� 				*/
	uCfgPrm.motresq = Prm->motresq;				/* PnE3B : ���[�^���U�}���p�m�b�`�t�B���^Q�l		*/
	uCfgPrm.fltrange = Prm->fltrange;			/* PnE3F : ���[�^�ߓd�����o�t�B���^��				*/	/* <S1F5> */
	uCfgPrm.zadjiu = Prm->zadjiu;				/* PnE50 : �d�����o�[����(U��)						*/
	uCfgPrm.zadjiv = Prm->zadjiv;				/* PnE51 : �d�����o�[����(V��)						*/
	uCfgPrm.gadjiu_v = Prm->gadjiu_v;			/* PnE52 : �d�����o�Q�C����(U,V��)					*/
	uCfgPrm.pnvoltgn = Cprm->pnvoltgn;			/* PnE70 : P-N�d�����o���x��						*/
	uCfgPrm.pwmf2 =  Prm->pwmf2;				/* PnEA5 : �É����[�hPWM���g��						*/
	uCfgPrm.ilimlvl =  Prm->ilimlvl;			/* PnEA6 : �L�����A���g���؂�ւ��d��臒l			*/
	uCfgPrm.pwmf3 = Prm->pwmf3;					/* PnEA7 : ��2PWM���g��								*/
	uCfgPrm.idlim = Prm->idlim;					/* PnEB2 : �d���t�B�[�h�o�b�N�ϕ����~�b�g			*/
	uCfgPrm.oslv_pol = Prm->MencP.oslv_pol;		/* PnF06 : �n�r���o���x��, �|�[����					*/
	uCfgPrm.irat =  Prm->MencP.irat;			/* PnF09 : ��i�d��									*/
	uCfgPrm.imax = Prm->MencP.imax;				/* PnF0A : �u���ő�d��								*/
	uCfgPrm.motresfrq = Prm->MencP.motresfrq;	/* PnF1C : ���[�^���U�}���p�m�b�`�t�B���^���g��		*/
	uCfgPrm.flg_wf = Prm->MencP.flg_wf;			/* PnF1E : �t���O									*/
/* ���[�^�L�����A���g���؂�ւ��Ή�<S117> */
	uCfgPrm.motfuncf = Prm->MencP.motfuncf;		/* PnF24 : ���[�^�@�\�t���O2						*/
	uCfgPrm.mot_pwmf = Prm->MencP.mot_pwmf;		/* PnF25 : ���[�^PWM���g��							*/
#if 0	/* �L�����A���g���̓���͍s�Ȃ�Ȃ����ߕs�v<S11E> */
	uCfgPrm.motCarrUse = 						/* ���[�^�L�����A(PnF25)�L�� 	[0:OFF,1:ON]		*/
			CarrerChange->conf.MotCarrUse;
#endif		/*#if 0	 �L�����A���g���̓���͍s�Ȃ�Ȃ����ߕs�v<S11E> */
	uCfgPrm.NormCarrCnt =				 		/* �ʏ�L�����A�J�E���g�l							*/
			CarrerChange->conf.NormCarrCnt;
	uCfgPrm.HighCarrCnt =				 		/* ���L�����A�J�E���g�l								*/
			CarrerChange->conf.HighCarrCnt;
/* ���[�^�L�����A���g���؂�ւ��Ή�<S117> */

	rc = KpxPcalMicroProgram( &uCfgPrm,
	                          Bprm,
	                          AxisRsc->SvAsicRegs,
							  &(AxisRsc->BaseControls->WeakenField));
//@@�T�[�{�I���ł��Ȃ��Ȃ�!! rc = KpxMotorOCLevel( &uCfgPrm, AxisRsc->SvAsicRegs, &(Prm->MencP) );	/*<S1F5>*/
	rc = KpxMotorOCLevel( &uCfgPrm, AxisRsc->SvAsicRegs, &(Prm->MencP) );	/* <S1F7> */

#if 0	/* �L�����A���g���̓���͍s�Ȃ�Ȃ����ߕs�v<S11E> */
 * * ���[�^�L�����A���g���؂�ւ��Ή�<S117> */
/*--------------------------------------------------------------------------------*/
/*  @@@@@@ SGD7W��p�L�����A���g���������� �ʃL�����A�ݒ���m�F�܂ł̎b�� @@@@@@@ */
/*--------------------------------------------------------------------------------*/
#if ( defined(CSW_SGD7W_TYPE) || defined(CSW_COMMON_CARRIER) )
	if( uCfgPrm.motCarrUse == TRUE )
	{/* ���[�^�L�����A���g�����g���ꍇ�ő����ł���΁A�L�����A���g�������낦�� */
		AxisIf->pwm_ax[AxisRsc->AxisNo] = CarrerChange->conf.NormCarrCnt;
		if( AxisRsc->AxisNo == 1 )
		{
			AXIS_HANDLE	*AxisRscI;
			MAICRO_CFGPRM	uCfgPrmI;
			BPRMDAT			*BprmI;
			LONG i;
			/* PWM���g�������ԂŒႢ���g���ɍ��킹�� */
			if( AxisIf->pwm_ax[0] >= AxisIf->pwm_ax[1] )
			{
				AxisIf->pwm_unit = AxisIf->pwm_ax[0];
			}
			else
			{
				AxisIf->pwm_unit = AxisIf->pwm_ax[1];
			}
			AxisRscI = AxisRsc;
			for( i=0; i<2; i++ )
			{
				BprmI = AxisRsc->UniPrms->Bprm;
				/*----------------------------------------------------------------------*/
				/*  ASIC Micro-Program Parameter Calculation */
				/*----------------------------------------------------------------------*/
				uCfgPrmI.ignq = Prm->ignq;					/* PnE21 : �d�����[�v�Q�C��(Q)						*/
				uCfgPrmI.dq_sw = Prm->dq_sw;					/* PnE2F : �c�p�p�X�C�b�`							*/
				uCfgPrmI.flg_wf = Prm->MencP.flg_wf;			/* PnF1E : �t���O									*/
				uCfgPrmI.NormCarrCnt =	AxisIf->pwm_unit; 		/* �ʏ�L�����A�J�E���g�l							*/

				rc |= KpxRecalcPWMparameter( &uCfgPrmI,
											 BprmI,
											 AxisRscI->SvAsicRegs);

				AxisRscI--;
			}
		}
	}
#endif	/* #if ( defined(CSW_SGD7W_TYPE) || defined(CSW_COMMON_CARRIER) ) */
#endif		/*#if 0	 �L�����A���g���̓���͍s�Ȃ�Ȃ����ߕs�v<S11E> */


	if(rc != 0)
	{/* Micro Parameter setting error!! : A.BF2 */
		ALMSetServoAlarm(AxisRsc->AlmManager, ALM_UPROG);
	}
	/* d���d���w�߃��~�b�g�l�v�Z�p�萔�̌v�Z */
	PcalIdrefLimTerm234(&(AxisRsc->BaseControls->WeakenField),
	                    AxisRsc->SvAsicRegs, Bprm, uCfgPrm.oslv_pol,
	                    AxisRsc->UniPrms->Cprm->pnvoltgn);

#ifndef _VC_DEBUG_
	/*----------------------------------------------------------------------*/
	/*  �ʐM�h�e�����p�����[�^�v�Z */
	/*----------------------------------------------------------------------*/
	OpiCalcInitParam( AxisRsc );//sq_kira
#endif /* _VC_DEBUG_ */

	/*----------------------------------------------------------------------*/
	/*  �S�p�����[�^�v�Z */
	/*----------------------------------------------------------------------*/
	if( UnitCommonCalc != FALSE)
	{
		/* Calculate Common Parameters for all axises */
		PrmRecalcAllParameters( AxisRsc->RegManager->hPnReg, TRUE );
	}
	PrmRecalcAllParameters( AxisRsc->RegManager->hPnReg, FALSE );

	/*----------------------------------------------------------------------*/
	/*  JL-076B ����������B (�p�����[�^�ˑ�) */
	/*----------------------------------------------------------------------*/
	SvipInitPrm.syssw1 = Prm->syssw1;
	SvipInitPrm.syssw2 = AxisRsc->UniPrms->Cprm->syssw2;
	SvipInitPrm.prstsw = Prm->prstsw;
	SvipInitPrm.comfil = Prm->comfil;
	SvipInitPrm.odt_c = Prm->odt_c;
	alm = SHal_InitAsicB( AxisRsc->SvAsicRegs->AsicHwReg, &SvipInitPrm );
	if( alm != SVIP_NOALM )
	{/* A.040 : �p�����[�^�G���[�̐ݒ� */
		ALMSetPramError( AxisRsc->AlmManager, pndef_prstsw.RegNumber );
	}
#if 0	/* �b�菈��	*/
	div_err = SencInitDivPulseIF( AxisRsc->SvAsicRegs->AsicHwReg,
	                              AxisRsc->MencV,
	                              AxisRsc->UniPrms->Bprm,
	                              (AxisRsc->UniPrms->Prm)->pgrat2l,
	                              (AxisRsc->UniPrms->Prm)->scalepgrat );

	if(div_err == DIVSET_DIVERR)
	{/* A.041 : �����p���X�o�͐ݒ�ُ�		*/
		ALMSetServoAlarm( AxisRsc->AlmManager, ALM_DIVPRM );
	}
	else if(div_err == DIVSET_PRMERR_R)
	{/* A.040 : �p�����[�^�G���[�̐ݒ�(��]�`)	*/
		ALMSetPramError( AxisRsc->AlmManager, pndef_pgrat2l.RegNumber );
	}
	else if(div_err == DIVSET_PRMERR_L)
	{/* A.040 : �p�����[�^�G���[�̐ݒ�(���j�A�`)	*/
		ALMSetPramError( AxisRsc->AlmManager, pndef_scalepgrat.RegNumber );
	}
	else
	{
		;
	}
#endif
#if	(SVFSEL_DIVOUT_USE == TRUE)  /*<S02D>*/
	/*----------------------------------------------------------------------*/
	/*  �����p���X�h�e������												*/
	/*----------------------------------------------------------------------*/
	if( AxisRsc->MencV->DivInfo.DivOut == TRUE )
	{
		div_err = SencInitDivPulseIF(AxisRsc->SvAsicRegs->AsicHwReg,
									 AxisRsc->MencV,
									 AxisRsc->UniPrms);
	}
#if	(CSW_SVCF_FULC_MODE != CSW_FULC_NO_SUPPORT)
	else if( AxisRsc->FencV->DivInfo.DivOut == TRUE )
	{
		div_err = FencSencInitDivPulseIF(AxisRsc->SvAsicRegs->AsicHwReg,
									     AxisRsc->FencV,
									     AxisRsc->UniPrms);
	}
#endif/*(CSW_SVCF_FULC_MODE != CSW_FULC_NO_SUPPORT)*/
	/*----------------------------------------------------------------------*/
	if(div_err == DIVSET_DIVERR)
	{/* A.041 : �����p���X�o�͐ݒ�ُ�		*/
		ALMSetServoAlarm( AxisRsc->AlmManager, ALM_DIVPRM );
	}
	/*----------------------------------------------------------------------*/
	else if(div_err == DIVSET_PRMERR_R)
	{/* A.040 : �p�����[�^�G���[�̐ݒ�(��]�`)	*/
		ALMSetPramError( AxisRsc->AlmManager, pndef_pgrat2l.RegNumber );
	}
	/*----------------------------------------------------------------------*/
	else if(div_err == DIVSET_PRMERR_L)
	{/* A.040 : �p�����[�^�G���[�̐ݒ�(���j�A�`)	*/
		ALMSetPramError( AxisRsc->AlmManager, pndef_scalepgrat.RegNumber );
	}
	/*----------------------------------------------------------------------*/
	else
	{
		;
	}
	/*----------------------------------------------------------------------*/
#endif	/*(SVFSEL_DIVOUT_USE == TRUE)*/
#ifndef CSW_NO_SERVO_ASICS
	/*----------------------------------------------------------------------*/
	/*  �V���A���G���R�[�_�ϐ�������(�p�����[�^�ˑ�) */
	/*----------------------------------------------------------------------*/
	MencRefPrm.syssw3 		= Prm->syssw3;
	MencRefPrm.scnvorgoffset= Prm->scnvorgoffset;
	MencRefPrm.phsofst 		= Prm->phsofst;
	MencRefPrm.MaxTrq		= Bprm->MaxTrq;
	MencRefPrm.Kfbpls		= Bprm->Kfbpls;
	MencRefPrm.Jmot			= Bprm->Jmot;

	err = SencInitMencVariables( AxisRsc->MencV,
	                             &(Prm->MencP),
	                             &MencRefPrm );
	if(SENCERR_POLESENSOR == err)		/* A.C21 : �|�[���Z���T�ُ�	*/
	{	ALMSetServoAlarm( AxisRsc->AlmManager, ALM_POLESENSOR );}
	else if(SENCERR_PHASEANGLE == err)	/* A.C22�F�ʑ����s��v */
	{	ALMSetServoAlarm( AxisRsc->AlmManager, ALM_PHASEANGLE );}
	else								/* ���� */
	{	;}

	/* ���[�^���X�e�X�g�@�\�ϐ������� *//* <S1B7> */
	if( AxisRsc->MotorLessTest->MotorLessTestMode == TRUE )
	{
		LpxSencMotLessInitVariables( AxisRsc->MencV, AxisRsc->SvAsicRegs->AsicHwReg );
	}
#endif

	/*----------------------------------------------------------------------*/
	/* �t���N �V���A���G���R�[�_�ϐ�������(�p�����[�^�ˑ�) */
	/*----------------------------------------------------------------------*/
	/* <S06E> */
	MlibResetByteMemory( &AxisRsc->BaseControls->MotSts, sizeof(BASE_MOT_STS) );
	
#if	(CSW_SVCF_FULC_MODE != CSW_FULC_NO_SUPPORT)
	if(AxisRsc->UniPrms->Bprm->FencUse != FALSE)
	{
		err = FencInitVariables(AxisRsc->FencV,
		                        AxisRsc->SvAsicRegs->AsicHwReg,
		                        &(Prm->MencP),
		                        Prm->scnvorgoffset,
		                        TRUE,
		                        Bprm->KConvLoadSpd,
		                        Prm->spgfail.b.h);
		if( err != SENCERR_SUCCESS )
		{	/* A.CF1 : �G���R�[�_�ʐM�ُ� */
			ALMSetServoAlarm( AxisRsc->AlmManager, ALM_FPGCOM );
		}
	}
/* <S1B7> >>>>> */
	/*----------------------------------------------------------------------*/
	/*		���[�^���X�e�X�g�@�\�ϐ�������									*/
	/*----------------------------------------------------------------------*/
	if( AxisRsc->MotorLessTest->MotorLessTestMode == TRUE )
	{
		/* �ϐ����������� */
		LpxFencMotLessInitVariables( AxisRsc->FencV,
									 AxisRsc->SvAsicRegs->AsicHwReg );
	}
/* <<<<< <S1B7> */
#endif/* (CSW_SVCF_FULC_MODE != CSW_FULC_NO_SUPPORT)	*/
	/*----------------------------------------------------------------------*/
	/* �ʒu�Ǘ��ϐ�������													*/
	/*----------------------------------------------------------------------*/
	if(AxisRsc->MencV->AxisMotType == MOTTYPE_ROTARY)
	{
		PosMngSenOnReCalcAbsoPos( AxisRsc->MencV,
		                          &(AxisRsc->BaseControls->PosManager.MencPos),
		                          &(AxisRsc->BaseControls->PosManager.conf.Egear),
								  Prm->MencP.bit_dp,
								  Prm->MencP.limmlt,
								  Bprm->RvsDir );
	}
	else
	{
		PosMngSenOnReCalcAbsoScalePos(AxisRsc->MencV,
									  &(AxisRsc->BaseControls->PosManager.MencPos),
									  &(AxisRsc->BaseControls->PosManager.conf.Egear),
									  Bprm->RvsDir);
	}

	if((AxisRsc->UniPrms->Bprm)->FencUse != FALSE)/* <S04D> */
	{
		PosMngSenOnReCalcAbsoScalePos(AxisRsc->FencV,
									  &(AxisRsc->BaseControls->PosManager.FencPos),
									  &(AxisRsc->BaseControls->PosManager.conf.Egear),
									  Bprm->RvsDir);
		AxisRsc->BaseControls->PosManager.PgPos = AxisRsc->BaseControls->PosManager.FencPos;
	}
	else
	{
		AxisRsc->BaseControls->PosManager.PgPos = AxisRsc->BaseControls->PosManager.MencPos;
	}


#if 0 /* 2012.08.31 Y.Oka ����ScanA��CpuScanA�� */
	/*----------------------------------------------------------------------*/
	/*  �}�C�N���v���O�����h�e�ϐ��̏����� */
	/*----------------------------------------------------------------------*/
	MicroSAIprm.RvsDir 			= Bprm->RvsDir;
	MicroSAIprm.CarrChange 		= ((Prm->b_prm9 & 0x0010) && (Prm->pwmf <= 4000) ) ? TRUE : FALSE;
	MicroSAIprm.flg_wf 			= Prm->MencP.flg_wf;
	MicroSAIprm.spgfail 		= Prm->spgfail;
	MicroSAIprm.SpdMaNumBit		= AxisRsc->MotSpdMafil->manumBit;
	MicroSAIprm.KmotspdA 		= Bprm->KmotspdA;
	MicroSAIprm.SvCycleNs		= Bprm->SvCycleNs;

	MicroSA_InitVariables( AxisRsc->BaseCtrlData,
	                       AxisRsc->MencV,  AxisRsc->SvAsicRegs, &MicroSAIprm);

	MicroSA_InitGainParameters( AxisRsc->SvAsicRegs->AsicMicroReg );
#endif
	/*----------------------------------------------------------------------*/
	/*  �G���R�[�_�ʒu�␳�@�\ ����������									*/
	/*----------------------------------------------------------------------*/
	/* �ʒu�␳UDL�́ACh0��0���ڂ̗̈�� Ch1��1���ڂ̗̈�𗘗p����B		*/
	/*----------------------------------------------------------------------*//* <S004> */
//	SHal_InitEncRdDly( AxisRsc->MencV->EncIf.PosCmpSetReg, AxisRsc->MencV->PgBaudRate );/*<S09C>*//* <S0E4> */
	SHal_InitEncRdDly( AxisRsc->MencV->EncIf.SencSetReg, AxisRsc->MencV->PgBaudRate, AxisRsc->MencV->EncIf.ch );/*<S09C>*//* <S0E4> */
	SvipInitPcomp.bit_dp 		= Prm->MencP.bit_dp;
	SvipInitPcomp.RevAsmMotor 	= AxisRsc->MencV->RevAsmMotor;
	SvipInitPcomp.AxisMotType 	= AxisRsc->MencV->AxisMotType;
	SvipInitPcomp.PulseNo 		= AxisRsc->MencV->PulseNo;
	SvipInitPcomp.AccErrLvl 	= AxisRsc->MencV->AccErrLvl;
	SvipInitPcomp.spgfail		= Prm->spgfail;
	SHal_InitEncPosComp( AxisRsc->MencV->EncIf.PosCmpSetReg, &SvipInitPcomp );		/* <S04D> �����ύX */

#if	(CSW_SVCF_FULC_MODE != CSW_FULC_NO_SUPPORT)	/* <S04D> */
	if(AxisRsc->UniPrms->Bprm->FencUse != FALSE)
	{
		SHal_InitEncRdDly_ch1( AxisRsc->FencV->EncIf.PosCmpSetReg, AxisRsc->FencV->PgBaudRate );/*<S09C>*/
		SvipInitPcomp.bit_dp 		= Prm->MencP.BitnoDpoint;
		SvipInitPcomp.RevAsmMotor 	= AxisRsc->FencV->RevAsmMotor;
		SvipInitPcomp.AxisMotType 	= AxisRsc->FencV->AxisMotType;
		SvipInitPcomp.PulseNo 		= AxisRsc->FencV->PulseNo;
		SvipInitPcomp.AccErrLvl 	= 0x007FFFFF;
		SvipInitPcomp.spgfail		= Prm->spgfail;
		SHal_InitEncPosComp( AxisRsc->FencV->EncIf.PosCmpSetReg, &SvipInitPcomp );	/* <S04D> */
	}
#endif/* (CSW_SVCF_FULC_MODE != CSW_FULC_NO_SUPPORT)	*/	

}


/*****************************************************************************
*  Description :�p�����[�^���Z�b�g����(���P��)
*  Parameter   :
*  Return      :
*****************************************************************************/
void SysResetAxisParameters( AXIS_HANDLE *AxisRsc )
{
	ALMMAN_HANDLE	*AlmManager;
	PRMDATA 		*Prm;							/* <S16A> */
	BOOL 			err;							/* <S16A> */
#if	(CSW_SVCF_FULC_MODE != CSW_FULC_NO_SUPPORT)
	BPRMDAT 		*Bprm;
	Bprm = AxisRsc->UniPrms->Bprm;					/* <S16A> */
#endif
//	SENC_ERROR		err;							/*<S0C7>*/
//	MENC_INIT_PRM	MencInitPrm;					/*<S0C7>*/
//	USHORT			BaudRateSel;  		/*<S003>*/	/*<S0C7>*/
//	BOOL			EncExchgSupported;	/*<S09C>*/	/*<S0C7>*/

	/* ���[�J���ϐ��̏����� */
	Prm = AxisRsc->UniPrms->Prm;		/* <S16A> */

	/*----------------------------------------------------------------------*/
	/*  �X�L�����`(�}�C�N��)�X�g�b�v */
	/*----------------------------------------------------------------------*/
	AxisRsc->MencV->EncDisable = TRUE;
	AxisRsc->SvAsicRegs->MicroRunningSts = FALSE;
	//	stop_micro_prg(AxisRsc->SvAsicRegs);
	KlibWaitus(500);

	/*----------------------------------------------------------------------*/
	/*  �A���[�������[�j���O�̃N���A */
	/*----------------------------------------------------------------------*/
	AlmManager	= AxisRsc->AlmManager;
	AlmManager->PrmErrInfo.ErrFlag  = FALSE;	/* Clear Error Flag			*/
	AlmManager->PrmErrInfo.ErrCount = 0;		/* Clear Error Counter		*/
	AlmManager->PrmErrInfo.ErrPrmNo = 0;		/* Clear Error PrmNo		*/
	ALMClearWarning( AxisRsc->AlmManager, WRN_BOOT );

	/* �G���R�[�_������ *//* <S04D> */
	if(SysPrstSelectMotorEncoder(AxisRsc) != FALSE)
	{/* �Z�~�t���N�p�����[�^�ݒ�ُ�iA.044)		*/
		ALMSetServoAlarm( AxisRsc->AlmManager, ALM_SEMIFULLPRM );
	}

#if 0 /* @@CHECK �p�����[�^�Čv�Z�ŃG���R�[�_�Đڑ��K�v�H */
 /* <S003> */
	/*------------------------------------------------------------------------------*/
	/*		Get Parameters related to Motor Encoder									*/
	/*------------------------------------------------------------------------------*/
	MencInitPrm.MencP = &(Prm->MencP);
	MencInitPrm.b_prm2 = Prm->b_prm2;
	MencInitPrm.b_prm80 = Prm->b_prm80;
	MencInitPrm.mtlmt = Prm->mtlmt;
	MencInitPrm.scalepitch2l = Prm->scalepitch2l;
	MencInitPrm.spgfail = Prm->spgfail;

	/*------------------------------------------------------------------------------*/
	/*		�J�n����(P0���[�h�ݒ�)													*/
	/*------------------------------------------------------------------------------*/
	SencChangeP0Mode( AxisRsc->MencV );

	/*------------------------------------------------------------------------------*/
	/* Initialize Motor Encoder */
	/*------------------------------------------------------------------------------*/
	if( (Prm->encbaudrate == SENCBR_AUTO_SEL) || (Prm->encbaudrate == SENCBR_4MBPS_SEL) )
	{
		/* Initialize Current PG Baud Rate (4MBps) */
		AxisRsc->MencV->PgBaudRate = SENC_COMRATE_4MBPS;
		BaudRateSel = SENC_COMRATE_4MBPS;
	}
	else /* Prm->encbaudrate == SENCBR_8MBPS_SEL */
	{
		/* Initialize Current PG Baud Rate (8MBps) */
		AxisRsc->MencV->PgBaudRate = SENC_COMRATE_8MBPS;
		BaudRateSel = SENC_COMRATE_8MBPS;
	}
	/* Motor Encoder Communication Initialize */
	err = SencInitMotorEncoder( AxisRsc->MencV,
							    &MencInitPrm,
							    AxisRsc->SvAsicRegs->AsicHwReg,
							   &AxisRsc->OptCardHdl->FbkOpt,/* <S04D>:FbkOpt�ǉ� */
								BaudRateSel,
								Bprm->SencFbOptUse );		/* <S04D>:SencFbOptUse�ǉ� */

	/*------------------------------------------------------------------------------*/
	/*		Exchange Encoder Baud Rate												*/
	/*------------------------------------------------------------------------------*/
	if( err == SENCERR_SUCCESS )
	{ /* Encorder is Connected */
		if( Prm->encbaudrate == SENCBR_AUTO_SEL )
		{ /* 8MBps�����؂�ւ��v������ && �G���R�[�_�ڑ��G���[�Ȃ�					*/
			err |= SencExchngBaudrate( AxisRsc->MencV,
									   &MencInitPrm,
									   AxisRsc->SvAsicRegs->AsicHwReg,
									   SENC_COMRATE_8MBPS,
									   &EncExchgSupported);/* <S09C> */
			if( err == SENCERR_PGCOM )
			{ /* Exchange Baud Rate was Success */
				AxisRsc->MencV->PgBaudRate = SENC_COMRATE_8MBPS;
			}
		}
		else if( Prm->encbaudrate == SENCBR_8MBPS_SEL )
		{
			; /* �ʐM���x�؂�ւ��Ȃ� */
		}
	}

	/*------------------------------------------------------------------------------*/
	/*		Detect and Set Alarm													*/
	/*------------------------------------------------------------------------------*/
	SysSetMotorEncoderError(err, AxisRsc->AlmManager);

	/*----------------------------------------------------------------------------------*/
	/* P1���[�h�ݒ�(��L�����̐���/���s�Ɍ��炸P1���[�h�i62.5us�ʐM�����j�ɕύX����		*/
	/*----------------------------------------------------------------------------------*/
	SencChangeP1Mode( AxisRsc->MencV );

#if	(CSW_SVCF_FULC_MODE != CSW_FULC_NO_SUPPORT)
	Bprm = AxisRsc->UniPrms->Bprm;
	/* Fully-closed Encoder */
	if(Bprm->FencUse != FALSE)
	{
		ferr = FencInitEncoder( AxisRsc->FencV,
							 	  &MencInitPrm,
								   AxisRsc->SvAsicRegs->AsicHwReg,
								   &AxisRsc->OptCardHdl->FbkOpt,
								   BaudRateSel,
								   Bprm->SencFbOptUse,
								   Bprm->RvsDir);
		if(err != SENCERR_SUCCESS)
		{	/* A.CF1 : �G���R�[�_�ʐM�ُ� */
			ALMSetServoAlarm( AxisRsc->AlmManager, ALM_FPGCOM );
		}
	}
#endif/* (CSW_SVCF_FULC_MODE != CSW_FULC_NO_SUPPORT)	*/

 /* <S003> */
 #endif /* @@CHECK �p�����[�^�Čv�Z�ŃG���R�[�_�Đڑ��K�v�H */
	err = SencSetEncoderType( AxisRsc->MencV, &(Prm->MencP), Prm->b_prm2, Bprm->SencFbOptUse );/* <S16A> */
	if(err == TRUE)
	{/* A.CA0 : �G���R�[�_�p�����[�^�ُ� */
		ALMSetServoAlarm( AxisRsc->AlmManager, ALM_PGP );
	}
	/*----------------------------------------------------------------------*/
	/*  �p�����[�^�Čv�Z */
	/*----------------------------------------------------------------------*/
	SysInitAxisParameters( AxisRsc, TRUE );

	/*----------------------------------------------------------------------*/
	/*  �X�L�����ϐ����Z�b�g */
	/*----------------------------------------------------------------------*/
//	PrstScanBData( AxisRsc );

	/*----------------------------------------------------------------------*/
	/*  �X�L�����`(�}�C�N��)���X�^�[�g */
	/*----------------------------------------------------------------------*/
//	start_micro_prg(AxisRsc->SvAsicRegs);
	AxisRsc->SvAsicRegs->MicroRunningSts = TRUE;
	AxisRsc->MencV->EncDisable = FALSE;	/* ScanA�X�^�[�g */
	KlibWaitus(500);
}

/****************************************************************************************************/
/*																									*/
/*		���[�^���G���R�[�_�I��(�p�����[�^���Z�b�g�p)												*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �g�p���郂�[�^���G���R�[�_���̐ݒ���s���B											*/
/*																									*/
/*			1) ���[�^�I�� : Yaskawa Motor, Non Yaskawa Motor										*/
/*																									*/
/*			2) ���[�^�G���R�[�_�I�� : Serial Encoder, AB Pulse Encoder, Encoder Less				*/
/*																									*/
/*			3) �t���N�G���R�[�_�I��(�g�p�^�s�g�p)													*/
/*																									*/
/*																									*/
/****************************************************************************************************/ /* <S04D> */
static	BOOL SysPrstSelectMotorEncoder( AXIS_HANDLE *AxisRsc )
{
//	INT	 		ax_no;/*<S0C7>*/
	PRMDATA 	*Prm;
	BPRMDAT 	*Bprm;
	OPFBK_HNDL 	FbkOpt;
	BOOL 		PrmError;

/*--------------------------------------------------------------------------------------------------*/
	Prm = AxisRsc->UniPrms->Prm;
	Bprm = AxisRsc->UniPrms->Bprm;
	FbkOpt = AxisRsc->OptCardHdl->FbkOpt;

	/*----------------------------------------------------------------------------------------------*/
	/*	�p�����[�^���Z�b�g�p�t�B�[�h�o�b�N�I�v�V�������G���R�[�_�g�p���� 							*/
	/*----------------------------------------------------------------------------------------------*/
#if CSW_OPT_CARD_FBOPTION != CSW_FBOPTION_NO_SUPPORT   /*  <S19A>  */
	PrmError = FbOptPrstSelectEncoder( &FbkOpt, Prm, Bprm );
#else
	PrmError = FALSE;
#endif    /*  <S19A>  */
	/*----------------------------------------------------------------------------------------------*/
	/*	�t���N�G���R�[�_�g�p����																	*/
	/*----------------------------------------------------------------------------------------------*/
	Bprm->FencUse = FALSE;

	if((Bprm->FencType == TRUE)	&&				/* �t���N�^�C�v�A���A							*/
	   ((Prm->b_prm2>>12) & 0x000F)	&&			/* �t���N�G���R�[�_�g�p�A���A					*/
	   (FbkOpt.conf.OpInfo.f.Connect == TRUE))		/* �t�B�[�h�o�b�N�I�v�V������Րڑ�����Ă���H	*/
	{
		Bprm->FencUse = TRUE;
	}
/*--------------------------------------------------------------------------------------------------*/
	return PrmError;

}

/*****************************************************************************
*  Description : EEPROM����̃p�����[�^���[�h(�S��)
*  Parameter   :
*  Return      :
*****************************************************************************/
static void SysLoadEepromParameters( AXIS_HANDLE *AxisRscI )
{
	INT					ax_no;
	LONG				eRslt, eRslt_u;
	EEP_HEADER_INFO_T	EepInfo;
	REGIF_CONFIG_T		*hPnReg, *hPnRegTop;
#if 0 /* 2012.08.22 Y.Oka Make Mercury Base Software (for INGRAM) */
	MODULE_ID_INFO		*mId;
#endif /* 2012.08.22 Y.Oka Make Mercury Base Software (for INGRAM) */
#ifdef _VC_DEBUG_ /* 2012.08.22 Y.Oka Make Mercury Base Software */
	USHORT				i;
#endif /* _VC_DEBUG_ */ /* 2012.08.22 Y.Oka Make Mercury Base Software */
	USHORT  nosumBlockValCMN[EEP_NOSUM_CMN_BLOCK_LENGTH]={0};	/* <S04E> */


#if defined(_VC_DEBUG_) /* 2012.08.22 Y.Oka Make Mercury Base Software */
	for( i=0; i<DEBUG_EEPROM_SIZE; i++)
	{
		AxisRscI->pDummyEeprom[i] = 0x00;
	}
	eRslt = DbgEepAllRead( AxisRscI->pDummyEeprom, DEBUG_EEPROM_SIZE );
	if( eRslt != 0 )
	{
		return;
	}
#endif /* #if defined(_VC_DEBUG_) */ /* 2012.08.22 Y.Oka Make Mercury Base Software */


	/* Load the identity information from IF's EEPROM  */
	hPnRegTop = AxisRscI->RegManager->hPnReg;

#if !defined(_VC_DEBUG_) /* 2012.08.22 Y.Oka Make Mercury Base Software */
	EepdevReadValues(PRMDEF_EEPROM_IF, 0, (USHORT*)&EepInfo, sizeof(EepInfo)/2);/* <S1D2> */
#else /* #if !defined(_VC_DEBUG_) */ /* 2012.08.22 Y.Oka Make Mercury Base Software */
	DbgEepReadValues(0, (UCHAR*)&EepInfo, sizeof(EepInfo)/2);
#endif /* #if !defined(_VC_DEBUG_) */ /* 2012.08.22 Y.Oka Make Mercury Base Software */

	*(AxisRscI->ModuleID->IfInfo) = EepInfo;

	/*--------------------------------------------------------------------------*/
	/* Store default parameters to EEPROM, if the parameter version is updated. */
	/*--------------------------------------------------------------------------*/
	if(EepInfo.FormatID != EEP_FORMAT_ID)
	{
//		ALMSetGlobalAlarm( ALM_EEPROM2 );

		for( ax_no = 0; ax_no < MAX_AXIS_NUMBER; ax_no++ )
		{
			USHORT	nosumBlockVal[EEP_NOSUM_BLOCK_LENGTH]={0};

			hPnReg = AxisRscI[ax_no].RegManager->hPnReg;
			/* Axis0�`7�̏��ɍēx�ݒ� */
//			hPnReg->EepromBlockAddress = (USHORT)(EEP_AXPRM_TOP_ADDRESS							/* <S0E4> */
//													+ (EEP_MAP_BLOCK_LENGTH * ax_no));
			hPnReg->EepromBlockAddress = (USHORT)(EEP_AXPRM_TOP_ADDRESS
													+ (EEP_MAP_AXIS_OFFSET * ax_no));			/* <S0E4> */
			// Store default parameters.
			PrmStoreSystemDefaultValues(hPnReg, FALSE, ACCLVL_SYSTEM);

			// Clear the no-checksum info.
			EepdevWriteNoSumValues(PRMDEF_EEPROM_IF, EEP_NOSUM_ADDRESS(ax_no,0),
								   nosumBlockVal, sizeof(EEP_NOCHKSUM_VAL_T)/2);
		}

		/* No Checksum Axis Parameters *//* <S04E> */		
		EepdevWriteNoSumValues(PRMDEF_EEPROM_IF,
							 EEP_NOSUMCMN_ADDRESS(0), nosumBlockValCMN, sizeof(EEP_NOCHKSUM_CMN_T)/2);
		OpCardEepDeviceInit( );	/* <S04D> */

		// Write the default parameters to EEPROM
		PrmStoreSystemDefaultValues(hPnRegTop, TRUE, ACCLVL_SYSTEM);

#if 0 /* EtherNet�p���� */ /* <S00D> */
		// Clear the no-checksum info.
//		EepdevWriteNoSumValues(hPnRegTop->hEepDev, PRMDEF_EEPROM_IF, EEP_NOSUMCMN_ADDRESS(EEP_IPADDR_OFS),
//		                       (USHORT*)&EepCmnDefaultValue.IPAddress, (sizeof(EEP_NOCHKSUM_CMN_T) - EEP_IPADDR_OFS)/2);
#endif

		if((EepInfo.ProductID == EepDefaultInfoValue.ProductID)
			&& (EepInfo.VendorID == EepDefaultInfoValue.VendorID))
		{
			EepInfo.FormatID = EEP_FORMAT_ID;
			EepdevWriteNoSumValues(PRMDEF_EEPROM_IF, 0,
						   (USHORT*)&EepInfo, sizeof(EEP_HEADER_INFO_T)/2);
		}
		else
		{	// Write the header information
			EepdevWriteNoSumValues(PRMDEF_EEPROM_IF, 0,
						   (USHORT*)&EepDefaultInfoValue, sizeof(EEP_HEADER_INFO_T)/2);
		}
	}

	/*----------------------------------------------------------------------*/
	/* Load Parameters from IF module */
	/*----------------------------------------------------------------------*/
#if !defined(_VC_DEBUG_) /* 2012.08.22 Y.Oka Make Mercury Base Software */
	eRslt_u = PrmLoadEepromValues(hPnRegTop, PRMDEF_EEPROM_IF, TRUE);
#else
	eRslt_u = DbgPrmLoadEepValues(hPnRegTop, AxisRscI->pDummyEeprom, TRUE);
#endif

	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
		hPnReg = (AxisRscI[ax_no].RegManager)->hPnReg;

#if !defined(_VC_DEBUG_) /* 2012.08.22 Y.Oka Make Mercury Base Software */
		eRslt = PrmLoadEepromValues(hPnReg, PRMDEF_EEPROM_IF, FALSE);
#else
		eRslt = DbgPrmLoadEepValues(hPnReg, AxisRscI->pDummyEeprom, FALSE);
#endif

		if((eRslt | eRslt_u) != 0)
		{/* A.020 : ���[�U�[�萔�`�F�b�N�T���ُ�	*/
			ALMSetServoAlarm( AxisRscI[ax_no].AlmManager, ALM_EEPROM1 );
//			PrmLoadSystemDefaultValues( hPnReg );	/* Parameter */
//			return;
		}
	}

#if 0 /* 2012.08.22 Y.Oka Make Mercury Base Software (for INGRAM) */
	/*----------------------------------------------------------------------*/
	/* Load Parameters from SDA module */
	/*----------------------------------------------------------------------*/
	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
		hPnReg  = (AxisRscI[ax_no].RegManager)->hPnReg;
		mId		= AxisRscI[ax_no].ModuleID;

		/* Load the identity information from SDA's EEPROM  */
		MlibResetByteMemory(&EepInfo, sizeof(EepInfo));
		eRslt = EepdevReadValues(hPnReg->hEepDev, PRMDEF_EEPROM_SDA,
								 hPnReg->EepromBlockAddress,
								 (USHORT*)&EepInfo, sizeof(EepInfo)/2);
		if(eRslt != 0)
		{// EEPROM�ǂ߂Ȃ���SDA�A���v�Ȃ�
			MlibResetByteMemory(&mId->SdaInfo, sizeof(EepInfo));
			ALMSetServoAlarm( AxisRscI[ax_no].AlmManager, ALM_EEPROM4 );	/* <S053> */
		}
		else
		{
//<S053>			if(EepInfo.SizeBlockA == EEP_MAP_HEADER_INFO_LENGTH)
			{
				mId->SdaInfo = EepInfo;
				if(EepInfo.FormatID != EEP_FORMAT_ID_SDA)
				{	/* A.021 : �p�����[�^�t�H�[�}�b�g�ُ�	*/
					ALMSetServoAlarm( AxisRscI[ax_no].AlmManager, ALM_EEPROM2 );
				}
				else
				{
					// Load parameters from SDA's EEPROM
					eRslt = PrmLoadEepromValues(hPnReg, PRMDEF_EEPROM_SDA, FALSE);
					if(eRslt != 0)
					{	/* A.022 : �V�X�e���`�F�b�N�T���ُ�	*/
						ALMSetServoAlarm( AxisRscI[ax_no].AlmManager, ALM_EEPROM3 );
					}
				}
			}
//<S053>	else
//			{/* SDA�A���v�Ȃ� */
//				MlibResetByteMemory(&mId->SdaInfo, sizeof(EepInfo));
//			}
		}
	}

	/*----------------------------------------------------------------------*/
	/* Load Parameters from COA module */
	/*----------------------------------------------------------------------*/
	/* Load the identity information from COA's EEPROM  */
	MlibResetByteMemory(&EepInfo, sizeof(EepInfo));
	eRslt = EepdevReadValues(hPnRegTop->hEepDev, PRMDEF_EEPROM_COA, EEP_CMNPRM_ADDRESS,
								(USHORT*)&EepInfo, sizeof(EepInfo)/2);
	if(eRslt != 0)
	{// EEPROM�ǂ߂Ȃ���COA���j�b�g�Ȃ�
		MlibResetByteMemory(AxisRscI->ModuleID->CoaInfo, sizeof(EepInfo));
		ALMSetGlobalAlarm( ALM_EEPROM4 );			/* <S053> */
	}
	else
	{
		*(AxisRscI->ModuleID->CoaInfo) = EepInfo;
		if(EepInfo.FormatID != EEP_FORMAT_ID_COA)
		{	/* A.021 : �p�����[�^�t�H�[�}�b�g�ُ�	*/
			ALMSetGlobalAlarm( ALM_EEPROM2 );
		}
		else
		{
			// Load parameters from COA's EEPROM
			eRslt = PrmLoadEepromValues(hPnRegTop, PRMDEF_EEPROM_COA, TRUE);
			if(eRslt != 0)
			{	/* A.022 : �V�X�e���`�F�b�N�T���ُ�	*/
				ALMSetGlobalAlarm( ALM_EEPROM3 );
			}
		}
	}

#if !defined(CSW_NETIF_SYNQNET_TYPE)
	/*----------------------------------------------------------------------*/
	/* Load Parameters from MBB module */
	/*----------------------------------------------------------------------*/
	/* Load the identity information from MBB's EEPROM  */
	MlibResetByteMemory(&EepInfo, sizeof(EepInfo));
	eRslt = EepdevReadValues(hPnRegTop->hEepDev, PRMDEF_EEPROM_MBB, 0,
								(USHORT*)&EepInfo, sizeof(EepInfo)/2);
	if(eRslt != 0)
	{// EEPROM�ǂ߂Ȃ���MBB���j�b�g�Ȃ�
		MlibResetByteMemory(AxisRscI->ModuleID->MbuInfo, sizeof(EepInfo));
	}
	else
	{
		*(AxisRscI->ModuleID->MbuInfo) = EepInfo;
		if(EepInfo.FormatID != EEP_FORMAT_ID_MBB)
		{	/* A.021 : �p�����[�^�t�H�[�}�b�g�ُ�	*/
			ALMSetGlobalAlarm( ALM_EEPROM2 );
		}

		// Load parameters from MBB's EEPROM
		eRslt = PrmLoadEepromValues(hPnRegTop, PRMDEF_EEPROM_MBB, TRUE);
		if(eRslt != 0)
		{	/* A.022 : �V�X�e���`�F�b�N�T���ُ�	*/
			ALMSetGlobalAlarm( ALM_EEPROM3 );
		}
	}
#endif
#endif /* 2012.08.22 Y.Oka Make Mercury Base Software (for INGRAM) */
}


#if 0 /* <S04D>�FThis function is no use (2013/10/3) */
/*****************************************************************************
*  Description : Initialize the detected Module informations
*  Parameter   :
*  Return      :
*****************************************************************************/
void SysInitModuleConfigInfo(AXIS_HANDLE *AxisRsc,
								EEP_NOCHKSUM_CMN_T *EepIdInfo, BOOL AutoConfig)
{
	INT					i, t_power;
	USHORT				ampw;
	MODULE_ID_INFO		*ModuleIDs;
	PRMDATA				*pprm_ptr;

#if (CSW_CSTR_AUTO_ADDR_CFG == 1)
	INT					j;
	CMN_PRMDATA			*Cprm;
	Cprm = AxisRsc->UniPrms->Cprm;
	j = 0;
#endif

	/* Loads the last connected SDA module info */
	EepdevReadValues(AxisRsc->AlmManager->EepDevice, 0,
					 EEP_NOSUMCMN_ADDRESS(EEP_SDAWAT_OFS),
					 (USHORT*)EepIdInfo->SdaWatt, MAX_AXIS_NUMBER*2);

	t_power = 0;
	for(i = 0; i < MAX_AXIS_NUMBER; i++)
	{	t_power += EepIdInfo->SdaWatt[i];}

	if((t_power != 0) && (AutoConfig == FALSE))
	{/* if last detected SDA info has been stored and MDINIT is OFF. */
		return;
	}

	for(i = 0; i < NumberOfAxes; i++)
	{
		ModuleIDs = AxisRsc[i].ModuleID;
		pprm_ptr = AxisRsc[i].UniPrms->Prm;

		/* Write SDA module info to EEPROM */
		ampw = (ModuleIDs->SdaInfo.BoardID != 0) ? pprm_ptr->ampw : 0;
		if(EepIdInfo->SdaWatt[i] != ampw)
		{
			EepdevWriteNoSumValues( AxisRsc->AlmManager->EepDevice, 0,
									EEP_NOSUMCMN_ADDRESS(EEP_SDAWAT_OFS)+(2*i),
									(USHORT*)&ampw, 1);
			EepIdInfo->SdaWatt[i] = ampw;
		}

#if (CSW_CSTR_AUTO_ADDR_CFG == 1)
		if(AutoConfig != FALSE)
		{
			/* Write Axis Address to EEPROM */
			Cprm->AxisAddress[i] = MAX_AXIS_NUMBER;
			if(ampw != 0)
			{
				Cprm->AxisAddress[i] = j;
				j++;
			}
			PrmStoreValueToEeprom(AxisRsc->RegManager->hPnReg,
											CprmAxisAddrDef[i], 0);
		}
#endif
	}
}
#endif


/*****************************************************************************
*  Description : Module Configuration
*  Parameter   :
*  Return      :
*****************************************************************************/
static void SysModuleConfiguration(AXIS_HANDLE *AxisRsc, MEMOBUS_ADDR_MAP *AddrMap)
{
	INT					i,/* j,*/ axnum;
//	ULONG				dsw;
	ULONG				err;
//	USHORT				ampw;
	USHORT				t_power;
	MODULE_ID_INFO		*ModuleIDs;
	PRMDATA				*pprm_ptr;
	CMN_PRMDATA			*cmnprm_ptr;
//	EEP_NOCHKSUM_CMN_T	EepIdInfo = {0};
//	UCHAR				addr[MAX_AXIS_NUMBER+1] = {0};
//	AXIS_HANDLE 		AxRscTmp[MAX_AXIS_NUMBER];			/* <S053> */

	cmnprm_ptr = AxisRsc->UniPrms->Cprm;
	ModuleIDs = AxisRsc->ModuleID;
	err = 0;												/* <S053> */
	axnum = 0;												/* <S0C4> */

	ModuleIDs->MbuInfo->BoardID = (USHORT)((ModuleIDs->MbuInfo->BoardID == 0) ?
	                              (0x2000 + NumberOfAxes) :
	                              (ModuleIDs->MbuInfo->BoardID));


	/* 2012.08.19 Y.Oka Make Mercury Base Software */

	/* Calculates total power */
	/* ��2����̗̂e�ʂ����ʃp�����[�^�Ƃ��邩�����̍��v�Ƃ��邩�v������ */
	t_power = 0;
	for(i = 0; i < NumberOfAxes; i++)
	{
		pprm_ptr = AxisRsc[i].UniPrms->Prm;
		t_power += pprm_ptr->ampw;
	}

#if 0 /* ��2012.08.19 Y.Oka Make Mercury Base Software (���W���[���R���t�B�O���Ή�)�� */
	/* Initialize the module detected info. */
	if(t_power != 0)
	{
		dsw = HALdrv_GetDipSwitchSetting();
		if(0 != (dsw & DSW_MDINIT_BIT))
		{/* MDINIT switch is ON */
			SysInitModuleConfigInfo(AxisRsc, &EepIdInfo, TRUE);
		}
		else
		{/* MDINIT switch is OFF */
			SysInitModuleConfigInfo(AxisRsc, &EepIdInfo, FALSE);
		}
	}
#endif

#if 0	/* <S145> */
	/* Compare with the last connected module info. */
	for(i = 0; i < NumberOfAxes; i++)
	{
		pprm_ptr = AxisRsc[i].UniPrms->Prm;
//		ampw = pprm_ptr->ampw;

		/* Check Axis Address */
#if 1   /* <S053> */
		j = cmnprm_ptr->AxisAddress[i];
		addr[j]++;
		if(addr[j] > 1)
		{	err++;}

		if(j < NumberOfAxes)
		{	axnum++;}
#else
		if(cmnprm_ptr->AxisAddress[i] > MAX_AXIS_NUMBER)
		{	/* A.E42 : axis address error */
			ALMSetServoAlarm(AxisRsc[i].AlmManager, ALM_AXIS_NO_ERR);
		}
		else
		{
			j = cmnprm_ptr->AxisAddress[i];
			addr[j]++;
		}
#endif
	}

#if 0		/* <S053> */
	/* Check the overlapped Addresses */
	axnum = 0;
	err = FALSE;
	for(i = 0; i < NumberOfAxes; i++)
	{
		if(addr[i] > 1)
		{
			err = TRUE;
		}

		if(cmnprm_ptr->AxisAddress[i] < NumberOfAxes)
		{
			axnum++;
		}

		/* Sets Axis address info. */
		AxisRsc[i].AxisNo = cmnprm_ptr->AxisAddress[i];
		AddrMap[i].AxisAddress = cmnprm_ptr->AxisAddress[i];
		AddrMap[i].hRegMngr = AxisRsc[i].RegManager;
		AddrMap[i].Axis = (void*)&AxisRsc[i];
	}
#endif
	
	/* <S053> Clear the rest of Address mapping variables */
	for(i = 0; i < MAX_AXIS_NUMBER; i++)
	{
		AddrMap[i].AxisAddress = AxisRsc[i].AxisNo;
		AddrMap[i].hRegMngr = AxisRsc[i].RegManager;
		AddrMap[i].Axis = (void*)&AxisRsc[i];
		AxRscTmp[i] = AxisRsc[i];
	}

	if((err != 0 /* FALSE */) || (axnum == 0))
	{	/* A.E42 : axis address error */
#ifndef	CSW_NETIF_ANLGPLS_TYPE		/* A.E42�΍�<S127> */
		ALMSetGlobalAlarm( ALM_AXIS_NO_ERR );
#endif	//@@zantei		/* A.E42�΍�<S127> */
	}
	else
	{
		axnum = 0;
		for(i = 0; i < NumberOfAxes; i++)
		{
#if 1		/* <S053> */
			j = cmnprm_ptr->AxisAddress[i];
			if(j < NumberOfAxes)
			{
				/* Sets Axis address info. */
				AddrMap[axnum].AxisAddress = j;
				AddrMap[axnum].hRegMngr = AxisRsc[i].RegManager;
				AddrMap[axnum].Axis = (void*)&AxisRsc[i];
				/* move up to the front of the Axis handle */
				AxisRsc[axnum] = AxRscTmp[j];
				AxisRsc[axnum].AxisNo = j;
				axnum++;
			}
#else
			if(cmnprm_ptr->AxisAddress[i] < NumberOfAxes)
			{
				/* move up to the front of the Axis handle */
				if(i != axnum)
				{
					AxisRsc[axnum] = AxisRsc[i];
				}
				axnum++;
			}
#endif
		}
		/* Sets number of Axes */
		NumberOfAxes = axnum;
	}

#endif /* <S145> */

	/* 
	 * <S167> 
	 * <S145>�̕ύX�ɂ��APn010>=MAX_AXIS_NUMBER����
	 * AddrMap������������ԂƂȂ��Ă����B
	 * Pn010,Pn011�ɂ�炸�A0���ڂ���AddrMap���Z�b�g����B 
	 * �_�����̓���ւ��͕ʓr�d�l����������B
	 * */
	for(i = 0; i < NumberOfAxes; i++)
	{
		AddrMap[i].AxisAddress = AxisRsc[i].AxisNo;
		AddrMap[i].hRegMngr = AxisRsc[i].RegManager;
		AddrMap[i].Axis = (void*)&AxisRsc[i];
	}
#if 0				/* <S167> */
/* <S145> start */
	/* Check Axis Address parameters */
	for(i = 0; i < MAX_AXIS_NUMBER; i++)
	{
		/* Check Axis Address */
		j = cmnprm_ptr->AxisAddress[i];
		if(j < NumberOfAxes)
		{
			addr[j]++;
			if(addr[j] > 1)
			{	err++;}
			axnum++;
			/* Clear the rest of Address mapping variables */
			AddrMap[i].AxisAddress = AxisRsc[i].AxisNo;
			AddrMap[i].hRegMngr = AxisRsc[i].RegManager;
			AddrMap[i].Axis = (void*)&AxisRsc[i];
			AxRscTmp[i] = AxisRsc[i];
		}
	}

	if((err != 0) || (axnum == 0) || (addr[0] == 0))
	{	/* A.E42 : axis address error */
#ifndef	CSW_NETIF_ANLGPLS_TYPE		/* A.E42�΍�<S127> */
		ALMSetGlobalAlarm( ALM_AXIS_NO_ERR );
#endif	//@@zantei		/* A.E42�΍�<S127> */
//		for(i = 0; i < MAX_AXIS_NUMBER; i++)
//		{
//			cmnprm_ptr->AxisAddress[i] = i;
//		}
	}
	else
	{
		axnum = 0;
		for(i = 0; i < NumberOfAxes; i++)
		{
			j = cmnprm_ptr->AxisAddress[i];
			if(j < NumberOfAxes)
			{
				/* move up to the front of the Axis handle */
				AxisRsc[axnum] = AxRscTmp[i];
				AxisRsc[axnum].AxisNo = j;

				/* Sets Axis address info. */
				AddrMap[axnum].AxisAddress = j;
				AddrMap[axnum].hRegMngr = AxisRsc[axnum].RegManager;
				AddrMap[axnum].Axis = (void*)&AxisRsc[axnum];
				axnum++;
			}
		}
		/* Sets number of Axes */
		NumberOfAxes = axnum;
	}
/* <S145> end */
#endif		/* <S167> */

	/* Get Axis Num info. from Parameter */
	/* �����ʃp�����[�^���玲�����擾����B���󖢑Ή��� */
	NumberOfAxes = MAX_AXIS_NUMBER;



#if 0 /* 2012.08.19 Y.Oka Make Mercury Base Software (for INGRAM) */
	/* Calculates total power of SDA */
	t_power = 0;
	for(i = 0; i < NumberOfAxes; i++)
	{
		ModuleIDs = AxisRsc[i].ModuleID;
		if(ModuleIDs->SdaInfo.BoardID != 0)/* SDA is connected. */
		{
			pprm_ptr = AxisRsc[i].UniPrms->Prm;
			t_power += pprm_ptr->ampw;
		}
	}

#if 0
	/* Checks the total SDA power */
	if((t_power > cmnprm_ptr->ConvW) || (t_power == 0))
	{	/* A.052 : no pwm amp, or no enough converter power */
		ALMSetGlobalAlarm( ALM_PWMAMP_SLOT );
	}
#endif

	/* Initialize the module detected info. */
	if(t_power != 0)
	{
		/* Here is only executing when pwm amps were connected. */

		dsw = HALdrv_GetDipSwitchSetting();
		if(0 != (dsw & DSW_MDINIT_BIT))
		{/* MDINIT switch is ON */
			SysInitModuleConfigInfo(AxisRsc, &EepIdInfo, TRUE);
		}
		else
		{/* MDINIT switch is OFF */
			SysInitModuleConfigInfo(AxisRsc, &EepIdInfo, FALSE);
		}
	}

	/* Compare with the last connected module info. */
	for(i = 0; i < NumberOfAxes; i++)
	{
		ModuleIDs = AxisRsc[i].ModuleID;
		pprm_ptr = AxisRsc[i].UniPrms->Prm;
		ampw = (ModuleIDs->SdaInfo.BoardID != 0) ? pprm_ptr->ampw : 0;

		if(ampw == 0)
		{	/* If SDA was not connected, mask the encoder alarm. */
			ALMSetMask(AxisRsc[i].AlmManager, ALM_PGCOM, TRUE);
		}

		/* Compare the power of connected SDA to last detected it. */
		if(EepIdInfo.SdaWatt[i] != ampw)
		{/* A.053 : pwm amp was changed. */
			ALMSetServoAlarm(AxisRsc[i].AlmManager, ALM_MODULE_CHANGE);
		}

		/* Check Axis Address */
		if(cmnprm_ptr->AxisAddress[i] > MAX_AXIS_NUMBER)
		{	/* A.E42 : axis address error */
			ALMSetServoAlarm(AxisRsc[i].AlmManager, ALM_AXIS_NO_ERR);
		}
		else
		{
			j = cmnprm_ptr->AxisAddress[i];
			addr[j]++;
		}
	}

	/* Check the overlapped Addresses */
	axnum = 0;
	err = FALSE;
	for(i = 0; i < NumberOfAxes; i++)
	{
		if(addr[i] > 1)
		{	err = TRUE;}

		if(cmnprm_ptr->AxisAddress[i] < NumberOfAxes)
		{	axnum++;}

		/* Sets Axis address info. */
		AxisRsc[i].AxisAddress = cmnprm_ptr->AxisAddress[i];
		AddrMap[i].AxisAddress = cmnprm_ptr->AxisAddress[i];
		AddrMap[i].hRegMngr = AxisRsc[i].RegManager;
		AddrMap[i].Axis = (void*)&AxisRsc[i];
	}

	if((err != FALSE) || (axnum == 0))
	{	/* A.E42 : axis address error */
		ALMSetGlobalAlarm( ALM_AXIS_NO_ERR );
	}
	else
	{
		axnum = 0;
		for(i = 0; i < NumberOfAxes; i++)
		{
			if(cmnprm_ptr->AxisAddress[i] < NumberOfAxes)
			{
				/* move up to the front of the Axis handle */
				if(i != axnum)
				{	AxisRsc[axnum] = AxisRsc[i];}
				axnum++;
			}
		}
		/* Sets number of Axises */
		NumberOfAxes = axnum;
	}
#endif /* 2012.08.19 Y.Oka Make Mercury Base Software (for INGRAM) */


}



#if 0 /* 2012.08.06 Y.Oka Make Mercury Base Software */
/*****************************************************************************
*  Description : Configuration of the DMA transfers
*  Parameter   :
*  Return      :
*****************************************************************************/
#if (CSW_MAICRO_DMA_TRANSFER != 0)
#include "DMAdrv.h"

static void SysInitMicroDMATransfers(AXIS_HANDLE *AxisRsc, LONG AxRscNum)
{
	INT					i;
	DMA_ADDR_MAP		DmaInputs;
	DMA_ADDR_MAP		DmaOutputs;
	DMA_SHARED_MEMORY	*DmaBuffer;

	/* Get the DMA Buffer address */
	DmaBuffer = (DMA_SHARED_MEMORY*)GetVirtualMemoryRegionAddress("dmaLocMemory");

	for(i = 0; i < AxRscNum; i++)
	{
		/* Sets the DMA Link descriptor for Outputs */
//		DmaOutputs.SrcAddr	= (void*)&(AxisRsc[i].SvAsicRegs->uDMAOutBuffer);
		AxisRsc[i].SvAsicRegs->uDMAOutBuffer = (MAICRO_CYC_OUTPUTS*)DmaBuffer->uDmaOutBuffer[i];
		DmaOutputs.SrcAddr	= (void*)(DmaBuffer->uDmaOutBuffer[i]);
		DmaOutputs.DstAddr	= (void*)&(AxisRsc[i].SvAsicRegs->AsicMicroReg)->MREG_BITCMDI;
		DmaOutputs.Length	= sizeof(MAICRO_CYC_OUTPUTS);
		DmaOutputs.NextLink	= DMA_SVDEV_OUT_LINK + i + 1;
		if(i == (AxRscNum-1))/* End of Link */
		{	DmaOutputs.NextLink	= 0x100;}

		DMAdrvSetLinkDescriptor(&DmaOutputs, 1, DMA_SVDEV_OUT_LINK+i);

		/* Sets the DMA Link descriptor for Inputs */
//		DmaInputs.DstAddr	= (void*)&(AxisRsc[i].SvAsicRegs->uDMAInBuffer);
		AxisRsc[i].SvAsicRegs->uDMAInBuffer = (MAICRO_CYC_INPUTS*)DmaBuffer->uDmaInBuffer[i];
		DmaInputs.DstAddr	= (void*)(DmaBuffer->uDmaInBuffer[i]);
		DmaInputs.SrcAddr	= (void*)&(AxisRsc[i].SvAsicRegs->AsicMicroReg)->MREG_BITDAT;
		DmaInputs.Length	= sizeof(MAICRO_CYC_INPUTS);
		DmaInputs.NextLink	= DMA_SVDEV_IN_LINK + i + 1;
		if(i == (AxRscNum-1))/* End of Link */
		{	DmaInputs.NextLink	= 0x100;}

		DMAdrvSetLinkDescriptor(&DmaInputs, 1, DMA_SVDEV_IN_LINK+i);
	}

	/* Initialize the DMA device driver */
	DMAdrvInitialize(DMA_SVDEV_CH);
}

#endif
#endif /* 2012.08.06 Y.Oka Make Mercury Base Software */



/* 2012.12.27 Y.Oka �b��Ή��B��`�ꏊ�����K�v */
/*****************************************************************************
*  Description : Initialize Function Control Common Servo Data
*  Parameter   :
*  Return      :
*****************************************************************************/
void FunCmnDataInitialize( FUN_CMN_CONTROL *FnCmnCtrl )
{
	FnCmnCtrl->FnSvControl = FALSE;					/* ROUND�^�]�� */
	FnCmnCtrl->FnCtrlMcmd = CTRLMODE_NOCMD;			/* ROUND�^�]�ł̃R���g���[�����[�h */
	FnCmnCtrl->FnSvonReq = FALSE;					/* ROUND�^�]�ł̃T�[�{�I���v�� */
	FnCmnCtrl->FnSensOnReq = FALSE;					/* ROUND�^�]�ł̃T�[�{�I���v�� */
	FnCmnCtrl->PdetReq = FALSE;						/* ���Ɍ��o�v��(�ʓd�v��) */
	FnCmnCtrl->JogSpeed = 0;						/* JOG���x�w�� +:���]�A-:�t�] */
	FnCmnCtrl->SafeStopTime = 0;					/* JOG���S��~���� */

	FnCmnCtrl->ForceOutSignals = FALSE;				/* �����M���o�͎w�� */
	FnCmnCtrl->KnlMode = KPI_KMD_NORMAL;			/* Tester mode */

	FnCmnCtrl->State.FnSvCtrlDisable = FALSE;		/* Fn�^�]�s��� */
	FnCmnCtrl->State.LockNormalOpeDrive = FALSE;	/* Fn�^�]��A�ʏ�^�]�֎~�t���O	*/
	FnCmnCtrl->State.TuningRun = FALSE;				/* �T�[�{������ */
	FnCmnCtrl->State.TuningPctrlRun = FALSE;		/* �T�[�{����P���䒆 */

	return;
}



/****************************************************************************
  Description: Initialize ScanB scheduling counters
-----------------------------------------------------------------------------
  Parameters	:
		AXIS_HANDLE *AxisRscB	: Pointer to Axis Handle
  Return		:
		none
*****************************************************************************/
static void	SysInitializeScanBcycle( AXIS_HANDLE *AxisRscB, ULONG SBcycle )
{
	AxisRscB->TaskProcessTime->SCtask_counter = 0;									/* Sigma7�ł͖��g�p	*/
	AxisRscB->TaskProcessTime->SBCount = 0;
	AxisRscB->TaskProcessTime->SCtask_wakeup_time = KPI_SCCYCLEUS / SBcycle;		/* Sigma7�ł͖��g�p	*/
	AxisRscB->TaskProcessTime->SBcountInMS = 1000 / SBcycle;
	AxisRscB->TaskProcessTime->SBCountInc = (SBcycle > (KPI_SBCYCLE_BASE_NS/1000)) ? 1 : 0;
}


/* <S14B> Start */
/****************************************************************************/
/*	���x���b�v���⏞���� �d�C�p(PnFA0-PnFB2) �Čv�Z����						*/
/****************************************************************************/
static void IniVelRippleCompPrmE( AXIS_HANDLE *Axis )
{
	PRMDATA	*Prm;
	BPRMDAT	*Bprm;
	RPLCMP	*RippleCmp;

	USHORT	RippleCompCycle_ele[6];
	SHORT	RippleCompAmplitude_ele[6];
	USHORT	RippleCompPhase_ele[6];

	LONG	i;

	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;
	RippleCmp = &(Axis->BaseLoops->RippleCmp);


	for( i = 0; i < RPLCMP_ORDER_ELE; i++ )
	{	/* �d�C�p�␳ */
		RippleCompCycle_ele[i] = Prm->RippleCompCycleE[i];
		RippleCompAmplitude_ele[i] = Prm->RippleCompAmplitudeE[i];
		RippleCompPhase_ele[i] = Prm->RippleCompPhaseE[i];
	}


/*--------------------------------------------------------------------------------------------------*/
/*		�d�C�p�␳�p�����[�^ 																		*/
/*--------------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------*/
	/*		�␳�Q�C��	[%] 																		*/
	/*----------------------------------------------------------------------------------------------*/
	RippleCmp->RplCmpPrm.RplCmpPrmEle.CompGain = MlibScalKxgain( Prm->RippleCompGainE, 1, 100, NULL, 24 );

	for(i=0; i<RPLCMP_ORDER_ELE; i++)
	{
		RippleCmp->RplCmpPrm.RplCmpPrmEle.CompFrq[i] = RippleCompCycle_ele[i];
		/*------------------------------------------------------------------------------------------*/
		/*		�␳�U���v�Z	[0.1%] --> [15000/MaxTrq]											*/
		/*------------------------------------------------------------------------------------------*/
		RippleCmp->RplCmpPrm.RplCmpPrmEle.CompAmpValue[i]  = BprmTorqueLevelCal( Bprm,
														(LONG)(RippleCompAmplitude_ele[i]), 1 );

		/*------------------------------------------------------------------------------------------*/
		/*		�ʑ��f�[�^�v�Z	[deg] --> [65536/360]												*/
		/*------------------------------------------------------------------------------------------*/
		RippleCmp->RplCmpPrm.RplCmpPrmEle.PhaseOfset[i]  = (USHORT)(SHORT)MlibScalKxgain
									((LONG)((SHORT)RippleCompPhase_ele[i]), 65536, 360, NULL,  -24 );
	}

	return;
}
/* <S14B> End */

/**************************** end of file **********************************/

