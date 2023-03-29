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
/* TODO: AP用パネオペ */
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


/* 2012.12.27 Y.Oka 暫定対応。定義場所検討必要 */
void FunCmnDataInitialize( FUN_CMN_CONTROL *FnCmnCtrl );


/* 外部関数 */
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

#if 0 /* Σ-7不使用 *//* <S0DF> */
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
extern USHORT		DebugBuf1[100];		/* デバッグ用											 *//* <TempSensorIC> */
extern USHORT		DebugBuf2[100];		/* デバッグ用											 *//* <TempSensorIC> */
extern USHORT		DebugBuf3[100];		/* デバッグ用											 *//* <TempSensorIC> */
extern USHORT		DebugBuf4[100];		/* デバッグ用											 *//* <TempSensorIC> */
extern USHORT		DebugBuf5[100];		/* デバッグ用											 *//* <TempSensorIC> */
extern USHORT		DebugBuf6[100];		/* デバッグ用											 *//* <TempSensorIC> */
extern USHORT		DebugBuf7[100];		/* デバッグ用											 *//* <TempSensorIC> */
extern USHORT		DebugBuf8[100];		/* デバッグ用											 *//* <TempSensorIC> */
extern USHORT		DebugBuf9[100];		/* デバッグ用											 *//* <TempSensorIC> */
extern USHORT		DebugBuf10[100];		/* デバッグ用											 *//* <TempSensorIC> */
extern USHORT		DebugBuf11[100];		/* デバッグ用											 *//* <TempSensorIC> */
extern USHORT		DebugBuf12[100];		/* デバッグ用											 *//* <TempSensorIC> */
extern USHORT		DebugBuf13[100];		/* デバッグ用											 *//* <TempSensorIC> */
extern USHORT		DebugBuf14[100];		/* デバッグ用											 *//* <TempSensorIC> */
extern INT			DebugCount;				/* デバッグ用											 *//* <TempSensorIC> */
extern INT 			Debug_i;				/* wait用												 *//* <TempSensorIC> */
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
*  Description :Mercury初期化処理（main）(全軸)
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
//	HALdrv_EnableServoAsicSync(KPI_SBCYCLE_BASE_NS, 0); /* 不使用 <S1D2> */

	/* オプションIFアプリ初期化 */
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

#if 0 /* 2012.08.19 Y.Oka Make Mercury Base Software ファイルダウンロード用？ */
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
	/* !!Timer処理を使う場合はこの処理より後で行なうこと!!		*/
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
	{	/* JL-086 初期化処理A (パラメータ非依存) */
			SHal_InitAsicA( (ASIC_HW_IF_STR*)(HW_SVASIC_ADDRESS + (JL086_SVIP_OFS * ax_no))
						/*	<S053> (ASIC_HW_IF_STR*)((AxisRscI->ModuleID->AsicNoMap)->asicAddr[ax_no])  */
							);
	}
	/*	JL-076 ASICマイクロプログラムのダウンロード＆メモリ初期化 */
	if( FALSE != SHal_InitMicroProgram( AxisRscI->SvAsicRegs ) )
	{	/* A.BF2 : システムアラーム２(Micro異常) */
		ALMSetGlobalAlarm( ALM_UPROG );
	}

	/* I/O Device Initialize */
	HALdrv_InitIODevices();
	/* LED Output Initialize */
//	HALdrv_OutputLED(LED_DEFAULT_OUT); /* 不使用 <Oka02> */
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
/*		I2Cバス I/Fの初期化																			*/
/*--------------------------------------------------------------------------------------------------*/
	/* 軸数分ループ */	/* <S19B> */
	for( ax_no = 0; ax_no < MAX_AXIS_NUMBER; ax_no++ )
	{
		AxisRsc = &AxisRscI[ax_no];
		if( I2C_DRV_SUCCESS != I2CdrvInitCpuI2CBusIF( ) )												  /* <S097> */
		{ /* A.BF6 : システムアラーム６(I2Cドライバ異常) */
			ALMSetServoAlarm( AxisRsc->AlmManager, ALM_I2C_DRVERR );
		}
//		EepdevInitialize(NULL, NULL, NULL, AxisRscI->pRwAllMng);	/* <S00C> EEPROM 初期化				*/
//		EepdevInitialize(NULL, NULL, NULL, AxisRscI->pRwAllMng, AxisRscI->pRwPdlMng);	/* <S031> 		*/
		EepdevInitialize(NULL, NULL, AxisRsc->pRwAllMng, AxisRsc->pRwPdlMng);	/* <S19B> *//* <S1D2> */
	}
	/* 0軸へ戻す */
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
	const UCHAR CProductName[] = { 	'G','S',  'V','D',  'M','-',  'A','D',   /* @@ テスト用暫定型式 */
									'2','0',  '8','E',  '3','M',  ' ','A',  };
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
	MlibCopyByteMemory( AxisRscI->ModuleID->IfInfo->ProductName, CProductName, sizeof(CProductName) );
/*--------------------------------------------------------------------------------------------------*/
	}
#endif

	time = 0;
	/* 軸数分ループ */	/* <S19B> */
	for( ax_no = 0; ax_no < MAX_AXIS_NUMBER; ax_no++ )
	{
		AxisRsc = &AxisRscI[ax_no];
//		EepdevReadValues(AxisRscI->AlmManager->EepDevice, 0,
		EepdevReadValues( 0,	/* <S19B> *//* <S1D2> */
						  //EEP_NOSUM_ADDRESS(0, EEP_TIMESTAMP_OFS),							/* <S04E>:DELL */
						  EEP_NOSUMCMN_ADDRESS(EEP_TIMESTAMP_OFS),							/* <S04E>:ADD */
						  (USHORT*)&time, 2 );
	}
	/* 0軸へ戻す */
	AxisRsc = &AxisRscI[0];	/* <S19B> */

	/* Set last time stamp value */
	KlibInitializeTimeStamp( time );
	/* <S154> Start */
#if ( CSW_SVCF_LIFEPRED_USE == TRUE )
	/* 平均内部温度の値をEEPROMから読む(EEPROMより使用平均温度取得)  */
	EepdevReadValues(AxisRscI->AlmManager->EepDevice, 0,
					 EEP_NOSUMCMN_ADDRESS(EEP_AVRINTERTMPR_OFS),
					 (USHORT*)&InternalTemperature, 1 );
	/* 平均内部温度算出初期化処理 */
	AvgInternalTemperatureCalInit(&(AxisRscI->hLifeTime->AvgTmprHdl), time, InternalTemperature );

	/* 寿命動作回数初期化 */
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

	/* 軸数分ループ */	/* <S19B> */
	for( ax_no = 0; ax_no < MAX_AXIS_NUMBER; ax_no++ )
	{
		AxisRsc = &AxisRscI[ax_no];
		/* Read EEPROM Option ID */
//		OpCardSetLastOptionId( AxisRscI->OptCardHdl, (AxisRscI->RegManager->hPnReg)->hEepDev );	/* <S04D> */
		OpCardSetLastOptionId( AxisRsc->OptCardHdl ); /* <S19B> *//* <S1D2> */
	}
	/* 0軸へ戻す */
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
		注意：この処理で、NumberOfAxesとAxisPointerの中身の入れ替えを実施)
		これ以降の処理では、物理的な軸位置と配列番号が異なるので注意すること
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
		/* オプションカード検出処理	*//* <S04D> */
		OpCardDetectOptions( AxisRsc->UniPrms->Prm, AxisRsc->OptCardHdl
									, AxisRsc->AlmManager, AxisRsc->SvAsicRegs->AsicHwReg);
		/* Write EEPROM Option ID	*//* <S04D> */
		OpCardWriteEepromOptionId( AxisRsc->OptCardHdl, ax_no );/* <S1D2> */

		/* 診断処理部初期化<S1F6> */
		SysInitSafetyDevice( AxisRsc->UniPrms->Prm->opdetsw, &AxisRsc->IoSignals->DetHwbbSts,
								AxisRsc->UniPrms->Prm->tplsfil, AxisRsc->IoSignals->TestPulseDiag );
	}

	/* モータレステストモード設定 *//* <S1B7> */
	MltSetMotorLessTestMode( AxisRscI, NumberOfAxes );

	/* エンコーダ初期化 */
	SysInitSelectMotorEncoder(AxisRscI, NumberOfAxes);	/* モータ＆エンコーダ選択			*//* <S04D> */
	SysConnectEncoders(AxisRscI, NumberOfAxes);			/* シリアルエンコーダ接続処理(全軸) */
#if	(CSW_SVCF_FULC_MODE != CSW_FULC_NO_SUPPORT)			/* <S04D> */
	if(AxisRscI->UniPrms->Bprm->FencUse != FALSE)
	{/*ﾌｨｰﾄﾞﾊﾞｯｸｵﾌﾟｼｮﾝ検出済みで、外部ｴﾝｺｰﾀﾞの使用方法が設定されている。 					*/
		SysConnectFcEncoders(AxisRscI);					/* フルクローズド接続処理			*/
	}
#endif/* CSW_SVCF_FULC_MODE != CSW_FULC_NO_SUPPORT*/

	/* オプションIFデバイス初期化 */
	OpiInitDevice( AxisRscI, (VOID*)HW_NETLSI_BASE_ADDRESS );


	/****************************************************************************/
	/*		Parameter Configurations											*/
	/****************************************************************************/
	UnitCmnFlag = TRUE;
	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
		AxisRsc = &AxisRscI[ax_no];
		/* 制御周期設定 */
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
		/* ベースループ初期化 */
		/* ★ポインタ設定のみ。引数化を検討する★ */
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

		/* モータレステストIF定義領域初期化 *//* <S1B7> */
		ApxInitMotorLessTestMode( AxisRsc );

		/* ベースコントロールデータ初期化(制御周期依存、パラメータ非依存) */
		InitScanBData( AxisRsc );

		/* 初期パラメータ計算 */
		SysInitAxisParameters( AxisRsc, UnitCmnFlag );
		UnitCmnFlag = FALSE;
	}
	SysInitializeScanBcycle(AxisRscI, AxisRsc->UniPrms->Bprm->SvCycleUs);

	/****************************************************************************/
	/*		Final Initializations												*/
	/* 			(各スキャンパラメータ,エンコーダ依存データ初期化)				*/
	/****************************************************************************/
//	HALdrv_EnableServoAsicSync(KPI_SBCYCLENS, 6/KPI_SBCYCLE_TIME);
	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
		AxisRsc = &AxisRscI[ax_no];

		/* アラーム管理スタート処理(AlmOLInfoを使う人はこの後でないとNG) */
		AlmOLInfo = ALMStartupProcedure( AxisRsc->AlmManager,
							 (UCHAR)(AxisRsc->BeSequence->AlmStop.conf.StopMode),
							  AxisRsc->MencV->AxisMotType, AxisRsc->MencV->AbsoEncType );
		/* サーボゲイン切替用変数初期化 */
		GselInitServoGainChange( &(AxisRsc->BaseControls->GainChange) );
		/* Reset Servo Sequence Variables */
		SyscInitServoSequence( AxisRsc );
		/* <S154> Start */
		/* アラームラッチ初期化処理 */
		/* アラームラッチモニタの値をEEPROMへ読む */
		EepdevReadValues( 0, /* <Oka02> */
						  EEP_NOSUM_ADDRESS(ax_no, EEP_ALMMON_OFS),
						  (USHORT*)&AxisRsc->hAlmLatchHndl->LatchMon , (EEPSIZE_ALMMON >> 1) );
		AlmLatchMonitorInitial( AxisRsc->hAlmLatchHndl );
		/* <S154> End */
		/* アラーム監視用変数初期化 */
		AdetInitCheckAlarm( AxisRsc->CheckAlarm, AlmOLInfo );
		/* サーボ制御状態監視変数初期化処理 */
		DetVibInitialize(AxisRsc->DetVib );
		/* JL076B 分周出力開始 */
//		(AxisRsc->SvAsicRegs->AsicHwReg)->AREG_IOSTAT1 |= BIT3;
		/* 自動ノッチ設定シーケンス用変数初期化 */
		AutoNotchInitialize( AxisRsc->ANotchSeq );
		/* AAT Initialize */
//<S053>		AdatSetStrPtr( (void*)AxisRsc );

		/* 制御用変数一括変換サービス変換モード変更 */
		TranlateCtrlSetIntMode( &(AxisRsc->BaseControls->TransduceCtrl) );

		/* Initialize the Operation register */
		PrmRecalcAllParameters( AxisRsc->RegManager->hFnReg, FALSE );

		/* 設置環境モニタ変数初期化 *//* <S0ED> */
		EnvMonInitialize( AxisRsc->EnvMonitor );


#if (CSW_POWER_MONITOR_USE == TRUE) /* <S020> */
		/* 消費電力演算用変数初期化 */
		PrmCalcPowerMonitor( AxisRsc->CalcWattage,
							 AxisRsc->UniPrms->Prm->imaxs,
							 AxisRsc->UniPrms->Prm->MencP.imax );
#endif

#if (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE)/* 将来的には、AnalogIFでも下記を実行する *//* <S0A1> */
		/* ネットワーク形パネオペ初期化 */
		PnlOpInitialize( AxisRsc->PnlOpLedDataHndl );
#endif/* (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE)	*/
		/* <S180> Start */
		/* 寿命予測機能 DB動作回数初期化 */
		LifeTimeDBSwithingCountInit(AxisRsc->hDBLifeTime);
		/* <S180> End */
	}

	/* Initialize the Operation register */
	PrmRecalcAllParameters( AxisRsc->RegManager->hFnReg, TRUE );

#if (CSW_POWER_MONITOR_USE == TRUE) /* <S020> */
	/* 消費電力演算用変数初期化 */
	InitializePowerMonitor( AxisRsc->PowerMonitor );
#endif

	/* 電源管理用変数初期化 */
	PcmInitPowerManager( AxisRscI->PowerManager,
						 AxisRscI->AlmManager->Status.AlmOLInfo );	/* <S0FF> */

	/* オーバーヒート監視用変数初期化 *//* <S0ED> */
	OhmInitialize( AxisRscI->OverHeat );

/* <S14B> Start */
	/* 速度リップル補償成分 電気角(PnFA0-PnFB2)の再計算 */
	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
		AxisRsc = &AxisRscI[ax_no];
		IniVelRippleCompPrmE( AxisRsc );		/* 速度リップル補償成分 電気角(PnFA0-PnFB2)	*/
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
	/* JL-100 通信ドライバ実行開始処理 */
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W（2軸対応）のみ	*//* <S0E4> */
	/* M-III MicroProgram Download Setting (PROGSEL Control) */
	HALdrv_SetM3PrgSel( CSW_M3_MICRO_DL );
#endif

#if (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE)/* 将来的には、AnalogIFでも下記を実行する。	*/
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
	{ /* 軸アドレス有効範囲 0x01 ～ 0x7F */
		/* 範囲外の場合は"0x01"を設定 */
		nodeAddr = 0x01;
	}
//	MsgIfInitialize(AxisRscI->NetMsgIf,
//					AddrMap, MAX_AXIS_NUMBER, nodeAddr,		/* <S10E> */
//					(AxisRscI->UniPrms->Cprm)->IP_Port);
// 実質使用しなくなったため削除 <S150>
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
//	u2f_init();							/* 不要な処理<S026> */
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
	/*  TODO: 軸アドレス対応	 																	*/
	/*  TODO: 多軸対応																				*/
	/*----------------------------------------------------------------------------------------------*/
	KpxInitCpuDMA( );  											/* Initialize CPU DMA <XXX> */
	KpxInitCpuSerialIF( COM0_UART, 19200, 8, 2, 1);				/* Initialize CPU Uart if <XXX> */
//	KpxInitCpuSerialIF( COM0_UART, 38400, 8, 2, 1);				/* Initialize CPU Uart if <XXX> */
//	KpxInitSifMsgManager( COM0_UART, SIFCOM0, 1, (void*)&(AxisRscI->NetMsgIf->UartMsg) ); /* initialize UART Ch.0 *//* TODO Zantei */			/* <S061> : 軸No.= 1を廃止 */
//<S150>		KpxInitSifMsgManager( COM0_UART, SIFCOM0, nodeAddr, (void*)&(AxisRscI->NetMsgIf->UartMsg) ); /* initialize UART Ch.0 *//* TODO Zantei */	/* <S061> : ロータリSWで設定した軸No.を使用する */
	KpxInitSifMsgManager( COM0_UART, SIFCOM0, nodeAddr ); /* initialize UART Ch.0 *//* <S150> NetMsgIfとの連携廃止 */
	KpxInitVirSifManager( VCOM0_PANEL, 1 );						/* Initialize Panel Operator <XXX>  */
//<S150>	KpxInitUsbMsgIf( 0, 0, 1, &(AxisRscI->NetMsgIf->UsbMbusServer) );/* TODO: Zantei */
	KpxInitUsbMsgIf( 0, nodeAddr, 1 );							/* Initialize USB */
#ifdef CSW_NETIF_M3_TYPE	/* <S150> */
	KpxInitMlinkMsgIf( 0, 0, nodeAddr, 0 );						/* Initialize M3 Message */
	KpxInitMlinkMsgIf( 0, 0, nodeAddr, 1 );						/* Initialize M3 Message */
#endif	/* <SAKA_TEST> */
//	KpxInitMsgInterface( &AxisRsc[0] );                         /* Initialize Message if <XXX> *//* <S0E4> */
//<S150>	KpxInitMsgInterface( AxisRscI/* <S0E4> 暫定で1軸目 &AxisRsc[0]*/ );                         /* Initialize Message if <XXX> *//* <S0E4> */
	KpxInitMsgInterface( AxisRscI/* <S0E4> 暫定で1軸目 &AxisRsc[0]*/, AddrMap );                         /* Initialize Message if <XXX> *//* <S0E4> */
	/*----------------------------------------------------------------------------------------------*/
//	RpxLcdOpInitDispInfo( ); 									/* Initialize LCD Operator Display Info<XXX> */
	RpxLcdOpInitDispInfo( AxisRscI );			/* Initialize LCD Operator Display Info<XXX> 	*/ /* <S214> */
	/*----------------------------------------------------------------------------------------------*/
#if (SVFSEL_ADC_USE == 1)||(SVFSEL_ONLY_OH_ADC_USE == 1)		/* <S09F> *//* <S0E1>:SVFSEL_ONLY_OH_ADC_USE追加 */
	KpxInitAdc( AxisRscI );										/* TODO: どっかに移動 */
#endif            /* <S09F> */
	KpxInitPls( AxisRscI );										/* TODO: どっかに移動 */
	/*----------------------------------------------------------------------------------------------*/
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)
	NpiScanNInitMain( &(AxisRscI->SvControlIf->CmdOptionIf) );	/* Message I/F 初期化後に呼ぶこと */
#endif

	/****************************************************************************/
	/*		Start Data Trace Manager Initialize									*/
	/****************************************************************************/
	DtrcInitDtrcManager( AxisRscI->TrcHndl, AxisRscI->TraceBuffer, AddrMap );

	/****************************************************************************/
	/*		Start ScanA Interrupt												*/
	/****************************************************************************/
#if defined(_TEST_BOARD_) && !defined(_VC_DEBUG_)
	/* 暫定処置 */
	InitializeInterruptCpu( );
	InitializeInterruptTiming( );
#else /* 本基板用 */

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

		/* micro program start処理	*/
		AxisRsc->MencV->EncDisable = FALSE;	/* ScanAスタート */
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
		DebugBuf1[ i ] = 0;		/* 配列を初期化									*//* <TempSensorIC> */
		DebugBuf2[ i ] = 0;		/* 配列を初期化									*//* <TempSensorIC> */
		DebugBuf3[ i ] = 0;		/* 配列を初期化									*//* <TempSensorIC> */
		DebugBuf4[ i ] = 0;		/* 配列を初期化									*//* <TempSensorIC> */
		DebugBuf5[ i ] = 0;		/* 配列を初期化									*//* <TempSensorIC> */
		DebugBuf6[ i ] = 0;		/* 配列を初期化									*//* <TempSensorIC> */
		DebugBuf7[ i ] = 0;		/* 配列を初期化									*//* <TempSensorIC> */
		DebugBuf8[ i ] = 0;		/* 配列を初期化									*//* <TempSensorIC> */
		DebugBuf9[ i ] = 0;		/* 配列を初期化									*//* <TempSensorIC> */
		DebugBuf10[ i ] = 0;	/* 配列を初期化									*//* <TempSensorIC> */
		DebugBuf11[ i ] = 0;	/* 配列を初期化									*//* <TempSensorIC> */
		DebugBuf12[ i ] = 0;	/* 配列を初期化									*//* <TempSensorIC> */
		DebugBuf13[ i ] = 0;	/* 配列を初期化									*//* <TempSensorIC> */
		DebugBuf14[ i ] = 0;	/* 配列を初期化									*//* <TempSensorIC> */
	}
	DebugCount = 0;				/* 初期化										*//* <TempSensorIC> */
	Debug_i = 0;				/* 初期化										*//* <TempSensorIC> */
	Debug_j = 0;				/* 初期化										*//* <TempSensorIC> */
//	AxisRsc->BaseCtrlOut->CmdSeqBit = 0;	/* 初期化									*//* <TempSensorIC> */
//	AxisRsc->SeqCtrlOut->BeComplete = 0;	/* 初期化									*//* <TempSensorIC> */
#endif	//#ifdef	_ASIP_DEBUG_BUF_USE_

//#ifdef SVFIF_CMDOPT		/* for debug EtherCAT一体	*/		/*<S008>*/
//	HALdrv_OutputDebugSP( CoE_SRST );		/* for netX Soft Reset debug */
//#endif		//#ifdef SVFIF_CMDOPT		/* for debug EtherCAT一体	*/		/*<S008>*/
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
*  Description :サーボアンプ＆サーボモータのタイプチェック(軸単位)
*
*	アンマッチ条件
*	(1) モータとアンプの入力電圧の不一致
*	(2) モータとアンプとの容量差が4倍以上
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

	/*	アラームマスクチェック	*/
	if(0 == (Prm->almmsk1 & 0x0001))
	{
		/*	電圧コードチェック	*/
		if( Bprm->AmpOutVcode != Bprm->MotVcode )
		{	/* A.050 : モータ/サーボ容量アンマッチ */
			ALMSetServoAlarm(AxisRsc->AlmManager, ALM_UNMATCH );
		}
		/*	容量比チェック */
		if( ( Bprm->AmpMotWatRatio <= 0x4000 ) ||
			((Bprm->AmpMotWatRatio >= 0x40000) && (Bprm->AmpMotImaxRatio > 0x40000)) )
		{	/* A.050 : モータ/サーボ容量アンマッチ */
			ALMSetServoAlarm(AxisRsc->AlmManager, ALM_UNMATCH );
		}
	}
}


/*****************************************************************************
*  Description :シリアルエンコーダの初期化(軸単位)
*  Parameter   :
*  Return      :
*****************************************************************************/
static void	SysSetMotorEncoderError(SENC_ERROR err, ALMMAN_HANDLE *AlmMngr )
{
	BOOL mask;

	mask = FALSE;
	/*----------------------------------------------------------------------*/
	/* シリアルエンコーダ接続チェック */
	/*----------------------------------------------------------------------*/
	if(err != SENCERR_SUCCESS)
	{
		if( SENCERR_PGECHO & err )
		{	/* A.CB0 : エンコーダエコーバック異常 */
			ALMSetServoAlarm( AlmMngr, ALM_PGECHO );
			mask = TRUE;
		}

		if( SENCERR_PGCOM & err )
		{	/* A.C90 : エンコーダ通信異常 */
			ALMSetServoAlarm( AlmMngr, ALM_PGCOM );
			mask = TRUE;
		}

		if( SENCERR_NOTSUPPORT & err )/* <S1BE> */
		{	/* A.051 : 製品未サポートアラーム */
			ALMSetServoAlarm( AlmMngr, ALM_NOTSUPPORT );
		}

#if !defined( CSW_NETIF_SYNQNET_TYPE )
		if( SENCERR_MTLMT & err )
		{	/* A.CC0 : マルチターンリミット値不一致 */
			ALMSetServoAlarm( AlmMngr, ALM_MTLMT );
		}
#endif
		if( SENCERR_SCALE & err )
		{	/* A.080 : スケールピッチ設定異常		*/
			ALMSetServoAlarm( AlmMngr, ALM_SCALE );
		}

		if( SENCERR_PRMDATA & err )
		{	/* A.CA0 : エンコーダパラメータ異常 */
			ALMSetServoAlarm( AlmMngr, ALM_PGP );
		}

		if( SENCERR_BRCHNGFAIL & err )
		{	/* A.C93 : エンコーダ通信システム異常 */
//			ALMSetServoAlarm( AlmMngr, ALM_ENCBR ); //@@CHECK 現状未対応
		}


		if(mask != FALSE)
		{
			/* アラームマスク処理 */
			ALMSetMask( AlmMngr, ALM_PGCOM1    , TRUE );	/* A.C91 : エンコーダ通信異常(加速度)	*/
			ALMSetMask( AlmMngr, ALM_PGCOM2    , TRUE );	/* A.C92 : エンコーダ通信タイマ異常		*/
			ALMSetMask( AlmMngr, ALM_PRM       , TRUE );	/* A.040 : パラメータ設定異常			*/
			ALMSetMask( AlmMngr, ALM_DIVPRM    , TRUE );	/* A.041 : 分周パルス出力設定異常		*/
			ALMSetMask( AlmMngr, ALM_UNMATCH   , TRUE );	/* A.050 : モータ/サーボ容量アンマッチ	*/
			ALMSetMask( AlmMngr, ALM_OLF1      , TRUE );	/* A.710 : 過負荷（瞬時最大負荷）		*/
			ALMSetMask( AlmMngr, ALM_OLF2      , TRUE );	/* A.720 : 過負荷（連続最大負荷）		*/
			ALMSetMask( AlmMngr, ALM_PGP       , TRUE );	/* A.CA0 : エンコーダパラメータ異常		*/
			ALMSetMask( AlmMngr, ALM_PGECHO    , TRUE );	/* A.CB0 : エンコーダエコーバック異常	*/
			ALMSetMask( AlmMngr, ALM_OSDIV     , TRUE );	/* A.511 : 分周パルス出力過速度			*/
			ALMSetMask( AlmMngr, ALM_SCALE     , TRUE );	/* A.080 : スケールピッチ設定異常		*/
			ALMSetMask( AlmMngr, ALM_PHASE     , TRUE );	/* A.C20 : 位相誤検出					*/
			ALMSetMask( AlmMngr, ALM_POLESENSOR, TRUE );	/* A.C21 : ポールセンサ異常				*/
			ALMSetMask( AlmMngr, ALM_VELPRM    , TRUE );	/* A.550 : 最高速度設定異常				*/
		}
	}
}


/****************************************************************************************************/
/*  Description : モータ&ｴﾝﾝｺｰﾀﾞ選択処理(全軸)														*/
/*  Parameter   : UNI_PRM_STR UniPrms																*/
/*              : OPFBK_HNDL FbkOpt																	*/
/*              : INT AxisRscNum			：軸数													*/
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
	/*		モータ選択(将来用)																		*/
	/*----------------------------------------------------------------------------------------------*/
		Bprm->MotSel = MOTSEL_YASKAWA;
	/*----------------------------------------------------------------------------------------------*/
	/*		モータエンコーダ選択(将来用)															*/
	/*----------------------------------------------------------------------------------------------*/
		Bprm->EncSel = ENCSEL_SERIAL;

	/*----------------------------------------------------------------------------------------------*/
	/*			フルクタイプ判定																	*/
	/*----------------------------------------------------------------------------------------------*/
		if(Prm->syssw1 & 0x0001)		/* フルクローズ対応機種？			*/
		{/* 対応機種(SGD7S) */
			Bprm->FencType = TRUE;
		}
		else
		{/* 非対応機種(SGD7W)：Ch1を2軸目が使用するため。 */
			Bprm->FencType = FALSE;
		}
	/*----------------------------------------------------------------------------------------------*/
	/*			フルクエンコーダ使用判定															*/
	/*----------------------------------------------------------------------------------------------*/
		Bprm->FencUse = FALSE;
		if((Bprm->FencType == TRUE)	&&				/* フルクタイプ、かつ、							*/
		   ((Prm->b_prm2>>12) & 0x000F)	&&			/* フルクエンコーダ使用、かつ、					*/
		   (FbkOpt.conf.OpInfo.f.Connect == TRUE))	/* フィードバックオプション基盤接続されている？	*/
		{
			Bprm->FencUse = TRUE;
		}
		
		Bprm->PsFulcSelect = FALSE;
		if((Bprm->FencUse == TRUE)&&( Prm->fcsel & 0x1000 ))
		{
			Bprm->PsFulcSelect = TRUE;				/* 完全フルク制御								*/
		}

	/*----------------------------------------------------------------------------------------------*/
	/*			セミク制御:フィードバックオプション側エンコーダ使用判定								*/
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
	/*			フルク制御:フィードバックオプション側エンコーダ使用判定 							*/
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
#define ENCODER_CONNECT_TIMEOUT		1500		/* 1.5s *///暫定
//TODO:↓ﾊﾟﾙｽ、ｱﾅﾛｸﾞｴﾝｺｰﾀﾞを考慮し関数ﾎﾟｲﾝﾀ化する↓
//TODO:ﾓｰﾀﾚｽ未対応
/****************************************************************************************************/
/*  Description : シリアルエンコーダ接続処理(全軸)													*/
/*  Parameter   : AXIS_HANDLE *AxisRscI																*/
/*              : INT AxisRscNum			：軸数													*/
/*  Return      :																					*/
/****************************************************************************************************/
static void SysConnectEncoders( AXIS_HANDLE *AxisRscI, INT AxisRscNum )
{
	INT	 			ax_no;
#if 0/* <S04D>:整理時に削除 */
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
//	SENC_ERROR		ferr[MAX_AXIS_NUMBER];	/* <S04D>:整理時に削除 */
	PRMDATA 		*Prm;
	BPRMDAT 		*Bprm;
	AXIS_HANDLE 	*AxisRsc;
	ASIC_HW_IF_STR	*encAsicHwReg;		/* <S04D> */
	ASIC_HW_IF_STR	*posCmpAsicHwReg;	/* <S04D> */
	OPFBK_HNDL 		*FbkOpt;			/* <S04D> */
	VFBTIMBASE		*Vfbtimbase;		/* <S067> */
	MOTLESSTST		*MotorLessTest;			/* <S1B7> */
	MOTOR_LESS		*MotorLessTestDatas;	/* <S1B7> */
	
	/* エンコーダ関連レジスタアクセス用UDLレジスタアドレス(0軸目を共通で使用) *//* <S04D> */
	encAsicHwReg = AxisRscI->SvAsicRegs->AsicHwReg;

	/*----------------------------------------------------------------------------------------------*/
	/*		シリアルエンコーダIF初期化																*/
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
		MlibResetLongMemory( Vfbtimbase, sizeof( *Vfbtimbase )/4 );		/* 速度演算変数				*//* <S067> */

		/*------------------------------------------------------------------------------------------*/
		/*		Initialize Motor Encoder channel.													*/
		/*------------------------------------------------------------------------------------------*/
#if CSW_BOARD_TYPE == SGD7S_TYPE /* <S0A2> */
		/* <S04D> フィードバックオプション対応処理追加 */
//		SencInitChannel(AxisRsc->MencV, 0, AxisRsc->SvAsicRegs->AsicHwReg);
		if(Bprm->SencFbOptUse != FALSE)
		{/* フィードバックオプションを用いたセミクローズド制御	 */
			/* Set Initialize Channel (ﾌｨｰﾄﾞﾊﾞｯｸｵﾌﾟｼｮﾝでのｾﾐｸは、Ch1を使用する)	*/
//			posCmpAsicHwReg = (ASIC_HW_IF_STR*)((UCHAR*)AxisRsc->SvAsicRegs->AsicHwReg + ASIC_AXIS2_OFFSET);	/* <S056>	定数定義名称変更 */
			posCmpAsicHwReg = (ASIC_HW_IF_STR*)((UCHAR*)AxisRsc->SvAsicRegs->AsicHwReg + JL086_SVIP_OFS);		/* <S056>	定数定義名称変更 */

			SencInitChannel(AxisRsc->MencV, 1, encAsicHwReg, posCmpAsicHwReg);
			/* エンコーダ情報およびモータ情報をコピー	*/
			AxisRsc->MencV->pActEncInfo = &FbkOpt->conf.EncInfo;
			AxisRsc->MencV->pActMotInfo = &FbkOpt->conf.MotInfo;

			/* エンコーダ通信速度設定(Pn250.1で設定され、フィードバックオプションで通信確認ずみ)	*/
			AxisRsc->MencV->PgBaudRate = FbkOpt->conf.PgBaudRate;
			BaudRateSel = FbkOpt->conf.PgBaudRate;

			/*------------------------------------------------------------------*/
			/*	エンコーダ位置補正機能の初期化(加速度チェックレベル(暫定値)設定)*/
			/*------------------------------------------------------------------*/
			SHal_InitEncPosCompAccLvl( posCmpAsicHwReg );
		}
		else
		{/* 通常(アンプ本体のコネクタ)を用いたセミクローズド制御 */
			/* Set Initialize Channel (通常でのｾﾐｸは、軸に合わせたChを使用する)	*/
			posCmpAsicHwReg = encAsicHwReg;
			SencInitChannel(AxisRsc->MencV, ax_no, encAsicHwReg, posCmpAsicHwReg);
			/* エンコーダ情報およびモータ情報をコピー	*/
			AxisRsc->MencV->pActEncInfo = &AxisRsc->MencV->EncInfo;
			AxisRsc->MencV->pActMotInfo = &AxisRsc->MencV->MotInfo;
			
			/* エンコーダ通信速度設定(Pn250.0で設定された値)	*/
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
			/*	エンコーダ位置補正機能の初期化(加速度チェックレベル(暫定値)設定)*/
			/*------------------------------------------------------------------*//* <S004> */
			SHal_InitEncPosCompAccLvl( posCmpAsicHwReg );
		}
#else /* CSW_BOARD_TYPE == SGD7W_TYPE *//* <S0A2> */

		/* 通常(アンプ本体のコネクタ)を用いたセミクローズド制御 *//* <S0E4> */
			/* Set Initialize Channel (通常でのｾﾐｸは、軸に合わせたChを使用する)	*/
//			posCmpAsicHwReg = encAsicHwReg;
			posCmpAsicHwReg = (ASIC_HW_IF_STR*)((UCHAR*)AxisRsc->SvAsicRegs->AsicHwReg);		/* <S056>	定数定義名称変更 */
			SencInitChannel(AxisRsc->MencV, ax_no, encAsicHwReg, posCmpAsicHwReg);
			/* エンコーダ情報およびモータ情報をコピー	*/
			AxisRsc->MencV->pActEncInfo = &AxisRsc->MencV->EncInfo;
			AxisRsc->MencV->pActMotInfo = &AxisRsc->MencV->MotInfo;

			/* エンコーダ通信速度設定(Pn250.0で設定された値)	*/
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
			/*	エンコーダ位置補正機能の初期化(加速度チェックレベル(暫定値)設定)*/
			/*------------------------------------------------------------------*//* <S004> */
			SHal_InitEncPosCompAccLvl( posCmpAsicHwReg );
//		}

#endif /* CSW_BOARD_TYPE == SGD7S_TYPE *//* <S0A2> */


#if 0 /*<S02D>*/
		/* @@見直し必要 */
		/*------------------------------------------------------------------------------------------*/
		/*		分周パルス出力有効Flag (サンプル版ハード：0軸目有効)								*/
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
		/*	分周パルス出力有効Flag設定																*/
		/*------------------------------------------------------------------------------------------*/
#if	(SVFSEL_DIVOUT_USE == TRUE)
		if( Bprm->FencUse != TRUE )
		{ /* セミクローズ制御の場合 */
			AxisRsc->MencV->DivInfo.DivOut = TRUE;
			AxisRsc->FencV->DivInfo.DivOut = FALSE;
		}
		else
		{ /* フルクローズ制御の場合 */
			AxisRsc->FencV->DivInfo.DivOut = TRUE;
			AxisRsc->MencV->DivInfo.DivOut = FALSE;
		}
#else /* 分周パルス出力機能無効の場合 */
		AxisRsc->MencV->DivInfo.DivOut = FALSE;
		AxisRsc->FencV->DivInfo.DivOut = FALSE;
#endif	/*(SVFSEL_DIVOUT_USE == TRUE)*/
#endif	/*<S02D>*/

		err[ax_no] = SENCERR_PGCOM;
//		ferr[ax_no] = SENCERR_PGCOM;/* <S04D> */
	}


	/*----------------------------------------------------------------------*/
	/* シリアルエンコーダ接続 */
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
/* <S1B7> 暫定 関数化する？ >>>>> */
				/*----------------------------------------------------------------------*/
				/*	アラームマスク処理													*/
				/*----------------------------------------------------------------------*/
				if( AxisRsc->MotorLessTest->MotorLessTestMode == TRUE )		/* モータレステスト時の処理				*/
				{
					ALMSetMask(AxisRsc->AlmManager, ALM_PG1			, TRUE);	/* A.810:エンコーダバックアップアラーム	*/
					ALMSetMask(AxisRsc->AlmManager, ALM_PG3			, TRUE);	/* A.830:エンコーダバッテリアラーム		*/
					ALMSetMask(AxisRsc->AlmManager, ALM_PG4			, TRUE);	/* A.840:エンコーダデータアラーム		*/
					ALMSetMask(AxisRsc->AlmManager, ALM_PG5			, TRUE);	/* A.850:エンコーダオーバスピード		*/
					ALMSetMask(AxisRsc->AlmManager, ALM_PG6			, TRUE);	/* A.860:エンコーダオーバヒート			*/
					ALMSetMask(AxisRsc->AlmManager, ALM_SCL_ERR		, TRUE);	/* A.890:エンコーダスケールエラー		*/
					ALMSetMask(AxisRsc->AlmManager, ALM_MOD_ERR		, TRUE);	/* A.891:エンコーダモジュールエラー		*/
					ALMSetMask(AxisRsc->AlmManager, ALM_PGCOM		, TRUE);	/* A.C90:エンコーダ通信異常				*/
					ALMSetMask(AxisRsc->AlmManager, ALM_PGCOM1		, TRUE);	/* A.C91:エンコーダ通信異常(加速度)		*/
					ALMSetMask(AxisRsc->AlmManager, ALM_PGCOM2		, TRUE);	/* A.C92:エンコーダ通信タイマ異常		*/
					ALMSetMask(AxisRsc->AlmManager, ALM_PGECHO		, TRUE);	/* A.CB0:エンコーダエコーバック異常		*/
					ALMSetMask(AxisRsc->AlmManager, ALM_POLESENSOR	, TRUE);	/* A.C21:ポールセンサ異常				*/
					ALMSetMask(AxisRsc->AlmManager, ALM_PHASEANGLE	, TRUE);	/* A.C22:位相情報不一致					*/
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
				/*		開始処理(P0モード設定)													*/
				/*------------------------------------------------------------------------------*/
//				SencChangeP0Mode( AxisRsc->MencV );			/* <S0F8> */
				SencInitChangeP0Mode( AxisRsc->MencV );		/* <S0F8> */

				/*------------------------------------------------------------------------------*/
				/*		Initialize Motor Encoder												*/
				/* Pn250.0の値に基いてエンコーダ通信速度を設定する。							*/
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
												   FbkOpt,/* <S04D>:FbkOpt追加 */
												   BaudRateSel,
												   Bprm->SencFbOptUse );		/* <S04D>:SencFbOptUse追加 */

/* <S1B7> >>>>> */
/* TODO: ここにモータレステスト用の処理を集めているが、問題ないか確認する必要あり */
				/* MencV->EncConnectの再設定 (関数化する？) */
				MotorLessTest = AxisRsc->MotorLessTest;
				MotorLessTestDatas = &(MotorLessTest->MotorLessTestDatas);

				if( err[ax_no] == SENCERR_SUCCESS )						/* Check Return Code					*/
				{
					MotorLessTestDatas->SemiEncConnect = TRUE;			/* モータレステスト用エンコーダ接続Flag	*/

					/* モータレステストかつエンコーダ接続時、エンコーダ関連アラームをマスクする		*/
					/* 但し、A.810はFn008実行にてクリア、A.830はマスクしても、A.810が発生する条件	*/
					/* を特定するために出したほうが良いのでマスクしない								*/
					if( MotorLessTest->MotorLessTestMode == TRUE )		/* エンコーダ接続&&モータレスの処理		*/
					{
						ALMSetMask(AxisRsc->AlmManager, ALM_PG2, TRUE);	/* A.820 : エンコーダサムチェックアラーム	*/
						ALMSetMask(AxisRsc->AlmManager, ALM_PG4, TRUE);	/* A.840 : エンコーダデータアラーム		*/
						ALMSetMask(AxisRsc->AlmManager, ALM_PG5, TRUE);	/* A.850 : エンコーダオーバスピード	 	*/
						ALMSetMask(AxisRsc->AlmManager, ALM_PG6, TRUE);	/* A.860 : エンコーダオーバヒート		*/
					}
				}
				else if( MotorLessTest->MotorLessTestMode == FALSE )	/* エンコーダ未接続時の処理				*/
				{
					/* 処理なし */
				}
				else													/* モータレステスト時の処理				*/
				{
					AxisRsc->MencV->EncConnect = TRUE;					/* エンコーダ接続Flag					*/
					MotorLessTestDatas->SemiEncConnect = FALSE;			/* モータレステスト用エンコーダ接続Flag	*/
					ALMSetMask(AxisRsc->AlmManager, ALM_PGP, TRUE);		/* A.CA0:エンコーダパラメータ異常		*/
					ALMSetMask(AxisRsc->AlmManager, ALM_PG2, TRUE);		/* A.820:エンコーダサムチェックアラーム	*/
				}

				/*------------------------------------------------------------------------------*/
				/*		モータレステストモードパラメータ読み込み処理							*/
				/*------------------------------------------------------------------------------*/
				if( (MotorLessTest->MotorLessTestMode == TRUE)				/* モータレステストモード			*/
						&& (MotorLessTestDatas->SemiEncConnect == FALSE))	/* エンコーダ未接続					*/
				{
					ApiReadMotPrmMotorLessTestMode( AxisRsc->MencV, Prm );							/* PnFxx パラメータ書き込み				*/
					LpxSetSencInfoMotorLessTestMode( AxisRsc->MencV, &(Prm->MencP), Prm->b_prm2 );	/* エンコーダ情報設定					*/

					/*--------------------------------------------------------------------------*/
					/*		エンコーダベースパラメータ計算処理									*/
					/*--------------------------------------------------------------------------*/
					MencInitPrm.mtlmt = Prm->mtlmt;
					SencCalcBaseParameters( AxisRsc->MencV, &MencInitPrm );
				}

				/* エンコーダIF関数ポインタ設定はここで実施する	*/
				if( MotorLessTest->MotorLessTestMode == FALSE )
				{
					/* 通常時 */
					AxisRsc->BaseLoops->SencReadPosition = SencReadPosition;
				}
				else
				{
					if( AxisRsc->MencV->AxisMotType == MOTTYPE_ROTARY )
					{
						/* モータレステスト：回転型 */
						AxisRsc->BaseLoops->SencReadPosition = RmxReadMotorLessPosition;
					}
					else
					{
						/* モータレステスト：リニア型 */
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
					{ /* 8MBps自動切り替え要求あり && エンコーダ接続エラーなし					*/
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
						; /* 通信速度切り替えなし */
					}
				}
 /* <S003> */

				/*------------------------------------------------------------------------------*/
				/*		Detect and Set Alarm													*/
				/*------------------------------------------------------------------------------*/
				SysSetMotorEncoderError( err[ax_no], AxisRsc->AlmManager );
				all_err |= err[ax_no];							/* Set Error Info.				*/
			}
#if 0/* <S04D>:本関数とは別の関数(フルク用の初期化関数)で実施。整理時には削除 */
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
		/*		経過時間計測 & タイムアウト判定													*/
		/*--------------------------------------------------------------------------------------*/
//		d_clk2 = SHal_GetSysClock( AxisRscI[0].SvAsicRegs );	/* <S0E4> */
//		d_clk += (ULONG)SHal_Get_dClock( d_clk1, d_clk2 );
//		past_t = (VULONG)SHal_ClockToTime( (USHORT)(d_clk/1000) );
		past_t = KlibGetLongTimerMs( d_clk1 );
	}while((ENCODER_CONNECT_TIMEOUT > past_t) && (all_err != SENCERR_SUCCESS) );

	for( ax_no = 0; ax_no < AxisRscNum; ax_no++ )/* <S04D>:ﾀｲﾑｱｳﾄ後移動 */
	{
		AxisRsc = &AxisRscI[ax_no];
		/*----------------------------------------------------------------------------------*/
		/*		P1モード設定(上記処理の成功/失敗に限らずP1モード(62.5us通信周期)に変更		*/
		/*----------------------------------------------------------------------------------*/
		SencChangeP1Mode( AxisRsc->MencV );
	}
	
#if 0/* <S04D>:本関数とは別の関数(フルク用の初期化関数)で実施。 整理時には削除 */
	/*----------------------------------------------------------------------*/
	/* シリアルエンコーダパラメータ初期設定 */
	/*----------------------------------------------------------------------*/
	for( ax_no = 0; ax_no < AxisRscNum; ax_no++ )
	{
		AxisRsc = &AxisRscI[ax_no];
		Bprm = AxisRsc->UniPrms->Bprm;

		/* Fully-closed Encoder																	*/
		if( (Bprm->FencUse != FALSE) && (ferr[ax_no] != SENCERR_SUCCESS) )
		{	
			/* A.CF1 : エンコーダ通信異常														*/
			ALMSetServoAlarm( AxisRsc->AlmManager, ALM_FPGCOM );

		}
	}
#endif

}


#if	(CSW_SVCF_FULC_MODE != CSW_FULC_NO_SUPPORT)/* <S04D> */
/****************************************************************************************************/
/*  Description : フルクローズ シリアルエンコーダ接続処理											*/
/* 				: 本関数では、軸番号は0軸目/1軸目とする。											*/
/* 				------------------------------------------------------------------------------------*/
/* 				: フルクローズド制御は、Ch1を使用するため、多軸アンプの1軸目と重複する。			*/
/* 				: 多軸アンプではフルクローズド制御を実施しない。									*/
/* 				: 	->	0軸目のみ。SGD7Wは、ｺﾝﾊﾟｲﾙｽｲｯﾁによりﾌﾙｸを無効に設定済み						*/
/* 				------------------------------------------------------------------------------------*/
/* 				: HOST CPUの変数は、0軸目の領域に設定する。											*/
/* 				: シリアルエンコーダの送受信に関するUDLの設定は、0軸目に行う。						*/
/* 				: 位置補正モジュールに関するUDLの設定は、1軸目に行う								*/
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
	MOTLESSTST		*MotorLessTest;			/* モータレステスト用データへのポインタ	*//*<S1B7>*/
	MOTOR_LESS		*MotorLessTestDatas;	/* エンコーダIF用データへのポインタ		*//*<S1B7>*/

	Prm = AxisRsc->UniPrms->Prm;
	Bprm = AxisRsc->UniPrms->Bprm;
	ferr = SENCERR_PGCOM;
	MotorLessTest = AxisRsc->MotorLessTest;						/*<S1B7>*/
	MotorLessTestDatas = &(MotorLessTest->MotorLessTestDatas);	/*<S1B7>*/
/*--------------------------------------------------------------------------------------------------*/
/*		Initialize Motor Encoder channel.															*/
/* フルクローズドエンコーダでは、Ch1を使用する。													*/
/* 基本設定は、0軸目のUDL領域を利用するが、位置補正モジュール関連は1軸目の領域を利用する。			*/
/*--------------------------------------------------------------------------------------------------*/
	/* Set Initialize Channel */
	encAsicHwReg = AxisRsc->SvAsicRegs->AsicHwReg;
//	posCmpAsicHwReg = (ASIC_HW_IF_STR*)((UCHAR*)AxisRsc->SvAsicRegs->AsicHwReg + ASIC_AXIS2_OFFSET);	/* <S056>	定数定義名称変更 */
	posCmpAsicHwReg = (ASIC_HW_IF_STR*)((UCHAR*)AxisRsc->SvAsicRegs->AsicHwReg + JL086_SVIP_OFS);		/* <S056>	定数定義名称変更 */
	SencInitChannel(AxisRsc->FencV, 1, encAsicHwReg, posCmpAsicHwReg);

	/*----------------------------------------------------------------------------------------------*/
	/*	エンコーダ位置補正機能の初期化(加速度チェックレベル(暫定値)設定)							*/
	/*----------------------------------------------------------------------------------------------*/
	SHal_InitEncPosCompAccLvl( posCmpAsicHwReg );

#if 0
/*--------------------------------------------------------------------------------------------------*/
/*	分周パルス出力選択Flagの設定																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( (SVFSEL_DIVOUT_USE != 0) && (Kprm.f.DivoutUse == TRUE) )
	{
		FencV->DivOut = TRUE;								/* フルクエンコーダ分周出力選択ON		*/
	}
	else
	{
		FencV->DivOut = FALSE;								/* フルクエンコーダ分周出力選択OFF		*/
	}
#endif
//#if 0 <S1B7> モータレステストモード対応
/*--------------------------------------------------------------------------------------------------*/
/*		アラームマスク処理																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( MotorLessTest->MotorLessTestMode == TRUE )
	{/* モータレステスト時の処理		*/
	/*----------------------------------------------------------------------------------------------*/
	/*	アラームマスク処理																			*/
	/*----------------------------------------------------------------------------------------------*/
		ALMSetMask(AxisRsc->AlmManager, ALM_FPGCOM		, TRUE);	/* A.CF1 : ＦＰＧ通信異常				*/
		ALMSetMask(AxisRsc->AlmManager, ALM_FPGCOMTIMER	, TRUE);	/* A.CF2 : ＦＰＧ通信タイマ異常			*/
		ALMSetMask(AxisRsc->AlmManager, ALM_FPGMODULE	, TRUE);	/* A.8A1 : ＦＰＧモジュール異常			*/
		ALMSetMask(AxisRsc->AlmManager, ALM_FPGSEN		, TRUE);	/* A.8A2 : ＦＰＧセンサ異常(インクレ)	*/
		ALMSetMask(AxisRsc->AlmManager, ALM_FPGPOS		, TRUE);	/* A.8A3 : ＦＰＧポジション異常(アブソ)	*/
		ALMSetMask(AxisRsc->AlmManager, ALM_FPGOVSPD	, TRUE);	/* A.8A5 : ＦＰＧｵｰﾊﾞｽﾋﾟｰﾄﾞ異常			*/
		ALMSetMask(AxisRsc->AlmManager, ALM_FPGOVHEAT	, TRUE);	/* A.8A6 : ＦＰＧｵｰﾊﾞﾋｰﾄ異常			*/
		ALMSetMask(AxisRsc->AlmManager, ALM_FPGFSCALE	, TRUE);	/* A.8A0 : ＦＰＧスケール異常			*/
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
	/*		開始処理																				*/
	/*----------------------------------------------------------------------------------------------*/
//	SencChangeP0Mode( AxisRsc->FencV );			/* <S0F8> */
	SencInitChangeP0Mode( AxisRsc->FencV );		/* <S0F8> */

	/*------------------------------------------------------------------------------*/
	/*		Initialize Motor Encoder												*/
	/* Pn250.1の値に基いてエンコーダ通信速度を設定する。							*/
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
			{/* 8MBps自動切り替え要求あり && エンコーダ接続エラーなし					*/
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
				; /* 通信速度切り替えなし */
			}
		}
		/*--------------------------------------------------------------------------------------*/
		/*		経過時間計測 & タイムアウト判定													*/
		/*--------------------------------------------------------------------------------------*/
		d_clk2 = SHal_GetSysClock( AxisRsc->SvAsicRegs );
		d_clk += (ULONG)SHal_Get_dClock( d_clk1, d_clk2 );
		past_t = (VULONG)SHal_ClockToTime( (USHORT)(d_clk/1000) );
		
	}while((ENCODER_CONNECT_TIMEOUT > past_t) && (ferr != SENCERR_SUCCESS) );

/* <S1B7> >>>>> */
	/*----------------------------------------------------------------------------------*/
	/*		モータレステストモードエンコーダ情報設定処理								*/
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
	/*		モータレステストモードエンコーダ情報設定処理								*/
	/*----------------------------------------------------------------------------------*/
	if( (MotorLessTest->MotorLessTestMode)						/* モータレステストモード		*/
			&& (MotorLessTestDatas->FullEncConnect == FALSE))	/* エンコーダ未接続				*/
	{
		ApiReadFencPrmMotorLessTestMode( &(Prm->MencP) );					/* PnFxx パラメータ書き込み		*/
		LpxSetFencInfoMotorLessTestMode( AxisRsc->FencV, &(Prm->MencP) );	/* エンコーダ情報設定			*/
	}

	/*----------------------------------------------------------------------------------*/
	/*		エンコーダＩＦ関数ポインタ設定												*/
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
	/*		P1モード設定(上記処理の成功/失敗に限らずP1モード(62.5us通信周期)に変更		*/
	/*----------------------------------------------------------------------------------*/
	SencChangeP1Mode( AxisRsc->FencV );
	
	/* Fully-closed Encoder																	*/
	if(ferr != SENCERR_SUCCESS)
	{	
		/* A.CF1 : エンコーダ通信異常														*/
		ALMSetServoAlarm( AxisRsc->AlmManager, ALM_FPGCOM );
	}

}
#endif/* CSW_SVCF_FULC_MODE != CSW_FULC_NO_SUPPORT*/
/*****************************************************************************
*  Description :初期パラメータ計算(軸単位)
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
#if 0/* 暫定処理 */
	DIVSET_ERROR	div_err;
#endif
	DIVSET_ERROR	div_err;			/*<S02D>*/
#ifndef CSW_NO_SERVO_ASICS
	SENC_ERROR		err;
	MENC_REF_PRM	MencRefPrm;
#endif
	CARRCHNG		*CarrerChange;		/* モータキャリア周波数切り替え対応<S117> */

#if 0	/* キャリア周波数の統一は行なわないため不要<S11E> */
#if ( defined(CSW_SGD7W_TYPE) || defined(CSW_COMMON_CARRIER) )		/* モータキャリア周波数切り替え対応<S117> */
	AXIS_IF	*AxisIf;
	AxisIf = AxisRsc->AxisIf;
#endif	/* #if ( defined(CSW_SGD7W_TYPE) || defined(CSW_COMMON_CARRIER) ) */
#endif		/*#if 0	 キャリア周波数の統一は行なわないため不要<S11E> */

	Cprm = AxisRsc->UniPrms->Cprm;
	Prm = AxisRsc->UniPrms->Prm;
	Bprm = AxisRsc->UniPrms->Bprm;

	CarrerChange = &AxisRsc->BaseLoops->CarrerChange;		/* モータキャリア周波数切り替え対応<S117> */

	/*----------------------------------------------------------------------*/
	/* 容量別リニア初期パラメータ設定 */
	/*----------------------------------------------------------------------*/
	if( AxisRsc->MencV->AxisMotType == MOTTYPE_LINEAR )
	{
		Prm->dq_sw = Prm->dq_sw_L;
		Prm->TunLessGain = Prm->TunLessGain_L;
		Prm->TunLessStiff = Prm->TunLessStiff_L;
	}

	/*----------------------------------------------------------------------*/
	/* 基本パラメータ計算 */
	/*----------------------------------------------------------------------*/
	BprmCfg.ratb2l = Prm->ratb2l;			/* Pn20E : 電子ギア比(分子)				*/
	BprmCfg.rata2l = Prm->rata2l;			/* Pn210 : 電子ギア比(分母)				*/
	BprmCfg.maxvel_l = Prm->maxvel_l;		/* Pn385 : モータ最高速度(リニア) 		*/
	BprmCfg.styp = Prm->styp;				/* PnE11 : サーボ形式/入力電圧コード	*/
	BprmCfg.ampw = Prm->ampw;				/* PnE12 : サーボ容量					*/
	BprmCfg.imaxs = Prm->imaxs;				/* PnE15 : サーボ最大電流				*/
	BprmCfg.dq_sw2= Prm->dq_sw2;			/* PnE39 : 電流制御スイッチ2			*/
	BprmCfg.ratmt2_os = Prm->ratmt2_os;		/* PnE62 : 過負荷検出中間時間2/OSレベル	*/
	BprmCfg.vrefgn = Prm->vrefgn;			/* Pn300 : 速度指令入力ゲイン			*/
	BprmCfg.gadjv = Prm->gadjv;				/* PnE55 : 速度指令ゲイン調				*/

	alm = BprmCalcBaseParameters( Bprm,
								  &Prm->MencP,
								  &BprmCfg,
								  AxisRsc->MencV, AxisRsc->FencV );
	if(alm != BPRM_NOALM)
	{
		if(alm & BPRM_NOTSUPPORT)			/* A.051:製品未サポートアラーム			*/
		{
			ALMSetServoAlarm(AxisRsc->AlmManager, ALM_NOTSUPPORT);
		}
		if(alm & BPRM_VELPRM)				/* A.550:最高速度設定異常				*/
		{
			ALMSetServoAlarm(AxisRsc->AlmManager, ALM_VELPRM);
		}
	}

	/*----------------------------------------------------------------------*/
	/* モータ容量チェック */
	/*----------------------------------------------------------------------*/
//	SysCheckAmpMotType(AxisRsc); /* <S09A> */

	/*----------------------------------------------------------------------*/
	/* 初期パラメータ計算 */
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

		/* 回生電力演算ゲイン計算 */
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
	/* モータ容量チェック */
	/*----------------------------------------------------------------------*/
	SysCheckAmpMotType(AxisRsc); /* <S09A> */

	/*----------------------------------------------------------------------*/
	/*  ASIC Micro-Program Parameter Calculation */
	/*----------------------------------------------------------------------*/
	uCfgPrm.imaxs = Prm->imaxs;					/* PnE15 : サーボパック最大電流						*/
	uCfgPrm.ignd = Prm->ignd;					/* PnE20 : 電流ループゲイン(D)						*/
	uCfgPrm.ignq = Prm->ignq;					/* PnE21 : 電流ループゲイン(Q)						*/
	uCfgPrm.kid = Prm->kid;						/* PnE22 : 電流ループ積分時定数(D)					*/
	uCfgPrm.kiq = Prm->kiq;						/* PnE23 : 電流ループ積分時定数(Q)					*/
	uCfgPrm.limintd = Prm->limintd;				/* PnE24 : 電流ループ積分制限値(D)					*/
	uCfgPrm.limintq = Prm->limintq;				/* PnE25 : 電流ループ積分制限値(Q)					*/
	uCfgPrm.ifil = Prm->ifil;					/* PnE26 : 電流ループフィルタ時定数					*/
	uCfgPrm.shant = Prm->shant;					/* PnE28 : 電流検出ゲイン							*/
	uCfgPrm.pwmf = Prm->pwmf;					/* PnE2C : ＰＷＭ周波数								*/
	uCfgPrm.dq_sw = Prm->dq_sw;					/* PnE2F : ＤＱ用スイッチ							*/
	uCfgPrm.vrevgn = Prm->vrevgn;				/* PnE2A : 電圧補正ゲイン							*/
	uCfgPrm.lmdata = Prm->lmdata;				/* PnE2B : 電圧制限値								*/
	uCfgPrm.odt_c = Prm->odt_c;					/* PnE2D : オンディレイ時間, オンディレイ補償定数	*/
	uCfgPrm.odlv_slp = Prm->odlv_slp;			/* PnE2E : オンディレイ補償変更レベル 				*/
	uCfgPrm.motresq = Prm->motresq;				/* PnE3B : モータ共振抑制用ノッチフィルタQ値		*/
	uCfgPrm.fltrange = Prm->fltrange;			/* PnE3F : モータ過電流検出フィルタ回数				*/	/* <S1F5> */
	uCfgPrm.zadjiu = Prm->zadjiu;				/* PnE50 : 電流検出ゼロ調(U相)						*/
	uCfgPrm.zadjiv = Prm->zadjiv;				/* PnE51 : 電流検出ゼロ調(V相)						*/
	uCfgPrm.gadjiu_v = Prm->gadjiu_v;			/* PnE52 : 電流検出ゲイン調(U,V相)					*/
	uCfgPrm.pnvoltgn = Cprm->pnvoltgn;			/* PnE70 : P-N電圧検出レベル						*/
	uCfgPrm.pwmf2 =  Prm->pwmf2;				/* PnEA5 : 静音モードPWM周波数						*/
	uCfgPrm.ilimlvl =  Prm->ilimlvl;			/* PnEA6 : キャリア周波数切り替え電流閾値			*/
	uCfgPrm.pwmf3 = Prm->pwmf3;					/* PnEA7 : 第2PWM周波数								*/
	uCfgPrm.idlim = Prm->idlim;					/* PnEB2 : 電圧フィードバック積分リミット			*/
	uCfgPrm.oslv_pol = Prm->MencP.oslv_pol;		/* PnF06 : ＯＳ検出レベル, ポール数					*/
	uCfgPrm.irat =  Prm->MencP.irat;			/* PnF09 : 定格電流									*/
	uCfgPrm.imax = Prm->MencP.imax;				/* PnF0A : 瞬時最大電流								*/
	uCfgPrm.motresfrq = Prm->MencP.motresfrq;	/* PnF1C : モータ共振抑制用ノッチフィルタ周波数		*/
	uCfgPrm.flg_wf = Prm->MencP.flg_wf;			/* PnF1E : フラグ									*/
/* モータキャリア周波数切り替え対応<S117> */
	uCfgPrm.motfuncf = Prm->MencP.motfuncf;		/* PnF24 : モータ機能フラグ2						*/
	uCfgPrm.mot_pwmf = Prm->MencP.mot_pwmf;		/* PnF25 : モータPWM周波数							*/
#if 0	/* キャリア周波数の統一は行なわないため不要<S11E> */
	uCfgPrm.motCarrUse = 						/* モータキャリア(PnF25)有効 	[0:OFF,1:ON]		*/
			CarrerChange->conf.MotCarrUse;
#endif		/*#if 0	 キャリア周波数の統一は行なわないため不要<S11E> */
	uCfgPrm.NormCarrCnt =				 		/* 通常キャリアカウント値							*/
			CarrerChange->conf.NormCarrCnt;
	uCfgPrm.HighCarrCnt =				 		/* 高キャリアカウント値								*/
			CarrerChange->conf.HighCarrCnt;
/* モータキャリア周波数切り替え対応<S117> */

	rc = KpxPcalMicroProgram( &uCfgPrm,
	                          Bprm,
	                          AxisRsc->SvAsicRegs,
							  &(AxisRsc->BaseControls->WeakenField));
//@@サーボオンできなくなる!! rc = KpxMotorOCLevel( &uCfgPrm, AxisRsc->SvAsicRegs, &(Prm->MencP) );	/*<S1F5>*/
	rc = KpxMotorOCLevel( &uCfgPrm, AxisRsc->SvAsicRegs, &(Prm->MencP) );	/* <S1F7> */

#if 0	/* キャリア周波数の統一は行なわないため不要<S11E> */
 * * モータキャリア周波数切り替え対応<S117> */
/*--------------------------------------------------------------------------------*/
/*  @@@@@@ SGD7W専用キャリア周波数調整処理 別キャリア設定を確認までの暫定 @@@@@@@ */
/*--------------------------------------------------------------------------------*/
#if ( defined(CSW_SGD7W_TYPE) || defined(CSW_COMMON_CARRIER) )
	if( uCfgPrm.motCarrUse == TRUE )
	{/* モータキャリア周波数を使う場合で多軸であれば、キャリア周波数をそろえる */
		AxisIf->pwm_ax[AxisRsc->AxisNo] = CarrerChange->conf.NormCarrCnt;
		if( AxisRsc->AxisNo == 1 )
		{
			AXIS_HANDLE	*AxisRscI;
			MAICRO_CFGPRM	uCfgPrmI;
			BPRMDAT			*BprmI;
			LONG i;
			/* PWM周波数を軸間で低い周波数に合わせる */
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
				uCfgPrmI.ignq = Prm->ignq;					/* PnE21 : 電流ループゲイン(Q)						*/
				uCfgPrmI.dq_sw = Prm->dq_sw;					/* PnE2F : ＤＱ用スイッチ							*/
				uCfgPrmI.flg_wf = Prm->MencP.flg_wf;			/* PnF1E : フラグ									*/
				uCfgPrmI.NormCarrCnt =	AxisIf->pwm_unit; 		/* 通常キャリアカウント値							*/

				rc |= KpxRecalcPWMparameter( &uCfgPrmI,
											 BprmI,
											 AxisRscI->SvAsicRegs);

				AxisRscI--;
			}
		}
	}
#endif	/* #if ( defined(CSW_SGD7W_TYPE) || defined(CSW_COMMON_CARRIER) ) */
#endif		/*#if 0	 キャリア周波数の統一は行なわないため不要<S11E> */


	if(rc != 0)
	{/* Micro Parameter setting error!! : A.BF2 */
		ALMSetServoAlarm(AxisRsc->AlmManager, ALM_UPROG);
	}
	/* d軸電流指令リミット値計算用定数の計算 */
	PcalIdrefLimTerm234(&(AxisRsc->BaseControls->WeakenField),
	                    AxisRsc->SvAsicRegs, Bprm, uCfgPrm.oslv_pol,
	                    AxisRsc->UniPrms->Cprm->pnvoltgn);

#ifndef _VC_DEBUG_
	/*----------------------------------------------------------------------*/
	/*  通信ＩＦ初期パラメータ計算 */
	/*----------------------------------------------------------------------*/
	OpiCalcInitParam( AxisRsc );//sq_kira
#endif /* _VC_DEBUG_ */

	/*----------------------------------------------------------------------*/
	/*  全パラメータ計算 */
	/*----------------------------------------------------------------------*/
	if( UnitCommonCalc != FALSE)
	{
		/* Calculate Common Parameters for all axises */
		PrmRecalcAllParameters( AxisRsc->RegManager->hPnReg, TRUE );
	}
	PrmRecalcAllParameters( AxisRsc->RegManager->hPnReg, FALSE );

	/*----------------------------------------------------------------------*/
	/*  JL-076B 初期化処理B (パラメータ依存) */
	/*----------------------------------------------------------------------*/
	SvipInitPrm.syssw1 = Prm->syssw1;
	SvipInitPrm.syssw2 = AxisRsc->UniPrms->Cprm->syssw2;
	SvipInitPrm.prstsw = Prm->prstsw;
	SvipInitPrm.comfil = Prm->comfil;
	SvipInitPrm.odt_c = Prm->odt_c;
	alm = SHal_InitAsicB( AxisRsc->SvAsicRegs->AsicHwReg, &SvipInitPrm );
	if( alm != SVIP_NOALM )
	{/* A.040 : パラメータエラーの設定 */
		ALMSetPramError( AxisRsc->AlmManager, pndef_prstsw.RegNumber );
	}
#if 0	/* 暫定処理	*/
	div_err = SencInitDivPulseIF( AxisRsc->SvAsicRegs->AsicHwReg,
	                              AxisRsc->MencV,
	                              AxisRsc->UniPrms->Bprm,
	                              (AxisRsc->UniPrms->Prm)->pgrat2l,
	                              (AxisRsc->UniPrms->Prm)->scalepgrat );

	if(div_err == DIVSET_DIVERR)
	{/* A.041 : 分周パルス出力設定異常		*/
		ALMSetServoAlarm( AxisRsc->AlmManager, ALM_DIVPRM );
	}
	else if(div_err == DIVSET_PRMERR_R)
	{/* A.040 : パラメータエラーの設定(回転形)	*/
		ALMSetPramError( AxisRsc->AlmManager, pndef_pgrat2l.RegNumber );
	}
	else if(div_err == DIVSET_PRMERR_L)
	{/* A.040 : パラメータエラーの設定(リニア形)	*/
		ALMSetPramError( AxisRsc->AlmManager, pndef_scalepgrat.RegNumber );
	}
	else
	{
		;
	}
#endif
#if	(SVFSEL_DIVOUT_USE == TRUE)  /*<S02D>*/
	/*----------------------------------------------------------------------*/
	/*  分周パルスＩＦ初期化												*/
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
	{/* A.041 : 分周パルス出力設定異常		*/
		ALMSetServoAlarm( AxisRsc->AlmManager, ALM_DIVPRM );
	}
	/*----------------------------------------------------------------------*/
	else if(div_err == DIVSET_PRMERR_R)
	{/* A.040 : パラメータエラーの設定(回転形)	*/
		ALMSetPramError( AxisRsc->AlmManager, pndef_pgrat2l.RegNumber );
	}
	/*----------------------------------------------------------------------*/
	else if(div_err == DIVSET_PRMERR_L)
	{/* A.040 : パラメータエラーの設定(リニア形)	*/
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
	/*  シリアルエンコーダ変数初期化(パラメータ依存) */
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
	if(SENCERR_POLESENSOR == err)		/* A.C21 : ポールセンサ異常	*/
	{	ALMSetServoAlarm( AxisRsc->AlmManager, ALM_POLESENSOR );}
	else if(SENCERR_PHASEANGLE == err)	/* A.C22：位相情報不一致 */
	{	ALMSetServoAlarm( AxisRsc->AlmManager, ALM_PHASEANGLE );}
	else								/* 正常 */
	{	;}

	/* モータレステスト機能変数初期化 *//* <S1B7> */
	if( AxisRsc->MotorLessTest->MotorLessTestMode == TRUE )
	{
		LpxSencMotLessInitVariables( AxisRsc->MencV, AxisRsc->SvAsicRegs->AsicHwReg );
	}
#endif

	/*----------------------------------------------------------------------*/
	/* フルク シリアルエンコーダ変数初期化(パラメータ依存) */
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
		{	/* A.CF1 : エンコーダ通信異常 */
			ALMSetServoAlarm( AxisRsc->AlmManager, ALM_FPGCOM );
		}
	}
/* <S1B7> >>>>> */
	/*----------------------------------------------------------------------*/
	/*		モータレステスト機能変数初期化									*/
	/*----------------------------------------------------------------------*/
	if( AxisRsc->MotorLessTest->MotorLessTestMode == TRUE )
	{
		/* 変数初期化処理 */
		LpxFencMotLessInitVariables( AxisRsc->FencV,
									 AxisRsc->SvAsicRegs->AsicHwReg );
	}
/* <<<<< <S1B7> */
#endif/* (CSW_SVCF_FULC_MODE != CSW_FULC_NO_SUPPORT)	*/
	/*----------------------------------------------------------------------*/
	/* 位置管理変数初期化													*/
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


#if 0 /* 2012.08.31 Y.Oka ★μScanA→CpuScanA★ */
	/*----------------------------------------------------------------------*/
	/*  マイクロプログラムＩＦ変数の初期化 */
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
	/*  エンコーダ位置補正機能 初期化処理									*/
	/*----------------------------------------------------------------------*/
	/* 位置補正UDLは、Ch0で0軸目の領域に Ch1は1軸目の領域を利用する。		*/
	/*----------------------------------------------------------------------*//* <S004> */
//	SHal_InitEncRdDly( AxisRsc->MencV->EncIf.PosCmpSetReg, AxisRsc->MencV->PgBaudRate );/*<S09C>*//* <S0E4> */
	SHal_InitEncRdDly( AxisRsc->MencV->EncIf.SencSetReg, AxisRsc->MencV->PgBaudRate, AxisRsc->MencV->EncIf.ch );/*<S09C>*//* <S0E4> */
	SvipInitPcomp.bit_dp 		= Prm->MencP.bit_dp;
	SvipInitPcomp.RevAsmMotor 	= AxisRsc->MencV->RevAsmMotor;
	SvipInitPcomp.AxisMotType 	= AxisRsc->MencV->AxisMotType;
	SvipInitPcomp.PulseNo 		= AxisRsc->MencV->PulseNo;
	SvipInitPcomp.AccErrLvl 	= AxisRsc->MencV->AccErrLvl;
	SvipInitPcomp.spgfail		= Prm->spgfail;
	SHal_InitEncPosComp( AxisRsc->MencV->EncIf.PosCmpSetReg, &SvipInitPcomp );		/* <S04D> 引数変更 */

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
*  Description :パラメータリセット処理(軸単位)
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

	/* ローカル変数の初期化 */
	Prm = AxisRsc->UniPrms->Prm;		/* <S16A> */

	/*----------------------------------------------------------------------*/
	/*  スキャンＡ(マイクロ)ストップ */
	/*----------------------------------------------------------------------*/
	AxisRsc->MencV->EncDisable = TRUE;
	AxisRsc->SvAsicRegs->MicroRunningSts = FALSE;
	//	stop_micro_prg(AxisRsc->SvAsicRegs);
	KlibWaitus(500);

	/*----------------------------------------------------------------------*/
	/*  アラーム＆ワーニングのクリア */
	/*----------------------------------------------------------------------*/
	AlmManager	= AxisRsc->AlmManager;
	AlmManager->PrmErrInfo.ErrFlag  = FALSE;	/* Clear Error Flag			*/
	AlmManager->PrmErrInfo.ErrCount = 0;		/* Clear Error Counter		*/
	AlmManager->PrmErrInfo.ErrPrmNo = 0;		/* Clear Error PrmNo		*/
	ALMClearWarning( AxisRsc->AlmManager, WRN_BOOT );

	/* エンコーダ初期化 *//* <S04D> */
	if(SysPrstSelectMotorEncoder(AxisRsc) != FALSE)
	{/* セミフルクパラメータ設定異常（A.044)		*/
		ALMSetServoAlarm( AxisRsc->AlmManager, ALM_SEMIFULLPRM );
	}

#if 0 /* @@CHECK パラメータ再計算でエンコーダ再接続必要？ */
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
	/*		開始処理(P0モード設定)													*/
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
							   &AxisRsc->OptCardHdl->FbkOpt,/* <S04D>:FbkOpt追加 */
								BaudRateSel,
								Bprm->SencFbOptUse );		/* <S04D>:SencFbOptUse追加 */

	/*------------------------------------------------------------------------------*/
	/*		Exchange Encoder Baud Rate												*/
	/*------------------------------------------------------------------------------*/
	if( err == SENCERR_SUCCESS )
	{ /* Encorder is Connected */
		if( Prm->encbaudrate == SENCBR_AUTO_SEL )
		{ /* 8MBps自動切り替え要求あり && エンコーダ接続エラーなし					*/
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
			; /* 通信速度切り替えなし */
		}
	}

	/*------------------------------------------------------------------------------*/
	/*		Detect and Set Alarm													*/
	/*------------------------------------------------------------------------------*/
	SysSetMotorEncoderError(err, AxisRsc->AlmManager);

	/*----------------------------------------------------------------------------------*/
	/* P1モード設定(上記処理の成功/失敗に限らずP1モード（62.5us通信周期）に変更する		*/
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
		{	/* A.CF1 : エンコーダ通信異常 */
			ALMSetServoAlarm( AxisRsc->AlmManager, ALM_FPGCOM );
		}
	}
#endif/* (CSW_SVCF_FULC_MODE != CSW_FULC_NO_SUPPORT)	*/

 /* <S003> */
 #endif /* @@CHECK パラメータ再計算でエンコーダ再接続必要？ */
	err = SencSetEncoderType( AxisRsc->MencV, &(Prm->MencP), Prm->b_prm2, Bprm->SencFbOptUse );/* <S16A> */
	if(err == TRUE)
	{/* A.CA0 : エンコーダパラメータ異常 */
		ALMSetServoAlarm( AxisRsc->AlmManager, ALM_PGP );
	}
	/*----------------------------------------------------------------------*/
	/*  パラメータ再計算 */
	/*----------------------------------------------------------------------*/
	SysInitAxisParameters( AxisRsc, TRUE );

	/*----------------------------------------------------------------------*/
	/*  スキャン変数リセット */
	/*----------------------------------------------------------------------*/
//	PrstScanBData( AxisRsc );

	/*----------------------------------------------------------------------*/
	/*  スキャンＡ(マイクロ)リスタート */
	/*----------------------------------------------------------------------*/
//	start_micro_prg(AxisRsc->SvAsicRegs);
	AxisRsc->SvAsicRegs->MicroRunningSts = TRUE;
	AxisRsc->MencV->EncDisable = FALSE;	/* ScanAスタート */
	KlibWaitus(500);
}

/****************************************************************************************************/
/*																									*/
/*		モータ＆エンコーダ選択(パラメータリセット用)												*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 使用するモータ＆エンコーダ情報の設定を行う。											*/
/*																									*/
/*			1) モータ選択 : Yaskawa Motor, Non Yaskawa Motor										*/
/*																									*/
/*			2) モータエンコーダ選択 : Serial Encoder, AB Pulse Encoder, Encoder Less				*/
/*																									*/
/*			3) フルクエンコーダ選択(使用／不使用)													*/
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
	/*	パラメータリセット用フィードバックオプション側エンコーダ使用判定 							*/
	/*----------------------------------------------------------------------------------------------*/
#if CSW_OPT_CARD_FBOPTION != CSW_FBOPTION_NO_SUPPORT   /*  <S19A>  */
	PrmError = FbOptPrstSelectEncoder( &FbkOpt, Prm, Bprm );
#else
	PrmError = FALSE;
#endif    /*  <S19A>  */
	/*----------------------------------------------------------------------------------------------*/
	/*	フルクエンコーダ使用判定																	*/
	/*----------------------------------------------------------------------------------------------*/
	Bprm->FencUse = FALSE;

	if((Bprm->FencType == TRUE)	&&				/* フルクタイプ、かつ、							*/
	   ((Prm->b_prm2>>12) & 0x000F)	&&			/* フルクエンコーダ使用、かつ、					*/
	   (FbkOpt.conf.OpInfo.f.Connect == TRUE))		/* フィードバックオプション基盤接続されている？	*/
	{
		Bprm->FencUse = TRUE;
	}
/*--------------------------------------------------------------------------------------------------*/
	return PrmError;

}

/*****************************************************************************
*  Description : EEPROMからのパラメータロード(全軸)
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
			/* Axis0～7の順に再度設定 */
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

#if 0 /* EtherNet用処理 */ /* <S00D> */
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
		{/* A.020 : ユーザー定数チェックサム異常	*/
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
		{// EEPROM読めない→SDAアンプなし
			MlibResetByteMemory(&mId->SdaInfo, sizeof(EepInfo));
			ALMSetServoAlarm( AxisRscI[ax_no].AlmManager, ALM_EEPROM4 );	/* <S053> */
		}
		else
		{
//<S053>			if(EepInfo.SizeBlockA == EEP_MAP_HEADER_INFO_LENGTH)
			{
				mId->SdaInfo = EepInfo;
				if(EepInfo.FormatID != EEP_FORMAT_ID_SDA)
				{	/* A.021 : パラメータフォーマット異常	*/
					ALMSetServoAlarm( AxisRscI[ax_no].AlmManager, ALM_EEPROM2 );
				}
				else
				{
					// Load parameters from SDA's EEPROM
					eRslt = PrmLoadEepromValues(hPnReg, PRMDEF_EEPROM_SDA, FALSE);
					if(eRslt != 0)
					{	/* A.022 : システムチェックサム異常	*/
						ALMSetServoAlarm( AxisRscI[ax_no].AlmManager, ALM_EEPROM3 );
					}
				}
			}
//<S053>	else
//			{/* SDAアンプなし */
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
	{// EEPROM読めない→COAユニットなし
		MlibResetByteMemory(AxisRscI->ModuleID->CoaInfo, sizeof(EepInfo));
		ALMSetGlobalAlarm( ALM_EEPROM4 );			/* <S053> */
	}
	else
	{
		*(AxisRscI->ModuleID->CoaInfo) = EepInfo;
		if(EepInfo.FormatID != EEP_FORMAT_ID_COA)
		{	/* A.021 : パラメータフォーマット異常	*/
			ALMSetGlobalAlarm( ALM_EEPROM2 );
		}
		else
		{
			// Load parameters from COA's EEPROM
			eRslt = PrmLoadEepromValues(hPnRegTop, PRMDEF_EEPROM_COA, TRUE);
			if(eRslt != 0)
			{	/* A.022 : システムチェックサム異常	*/
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
	{// EEPROM読めない→MBBユニットなし
		MlibResetByteMemory(AxisRscI->ModuleID->MbuInfo, sizeof(EepInfo));
	}
	else
	{
		*(AxisRscI->ModuleID->MbuInfo) = EepInfo;
		if(EepInfo.FormatID != EEP_FORMAT_ID_MBB)
		{	/* A.021 : パラメータフォーマット異常	*/
			ALMSetGlobalAlarm( ALM_EEPROM2 );
		}

		// Load parameters from MBB's EEPROM
		eRslt = PrmLoadEepromValues(hPnRegTop, PRMDEF_EEPROM_MBB, TRUE);
		if(eRslt != 0)
		{	/* A.022 : システムチェックサム異常	*/
			ALMSetGlobalAlarm( ALM_EEPROM3 );
		}
	}
#endif
#endif /* 2012.08.22 Y.Oka Make Mercury Base Software (for INGRAM) */
}


#if 0 /* <S04D>：This function is no use (2013/10/3) */
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
	/* ★2軸一体の容量を共通パラメータとするか軸毎の合計とするか要検討★ */
	t_power = 0;
	for(i = 0; i < NumberOfAxes; i++)
	{
		pprm_ptr = AxisRsc[i].UniPrms->Prm;
		t_power += pprm_ptr->ampw;
	}

#if 0 /* ★2012.08.19 Y.Oka Make Mercury Base Software (モジュールコンフィグ未対応)★ */
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
#ifndef	CSW_NETIF_ANLGPLS_TYPE		/* A.E42対策<S127> */
		ALMSetGlobalAlarm( ALM_AXIS_NO_ERR );
#endif	//@@zantei		/* A.E42対策<S127> */
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
	 * <S145>の変更により、Pn010>=MAX_AXIS_NUMBERだと
	 * AddrMapが未初期化状態となっていた。
	 * Pn010,Pn011によらず、0軸目からAddrMapをセットする。 
	 * 論理軸の入れ替えは別途仕様を検討する。
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
#ifndef	CSW_NETIF_ANLGPLS_TYPE		/* A.E42対策<S127> */
		ALMSetGlobalAlarm( ALM_AXIS_NO_ERR );
#endif	//@@zantei		/* A.E42対策<S127> */
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
	/* ★共通パラメータから軸数を取得する。現状未対応★ */
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



/* 2012.12.27 Y.Oka 暫定対応。定義場所検討必要 */
/*****************************************************************************
*  Description : Initialize Function Control Common Servo Data
*  Parameter   :
*  Return      :
*****************************************************************************/
void FunCmnDataInitialize( FUN_CMN_CONTROL *FnCmnCtrl )
{
	FnCmnCtrl->FnSvControl = FALSE;					/* ROUND運転権 */
	FnCmnCtrl->FnCtrlMcmd = CTRLMODE_NOCMD;			/* ROUND運転でのコントロールモード */
	FnCmnCtrl->FnSvonReq = FALSE;					/* ROUND運転でのサーボオン要求 */
	FnCmnCtrl->FnSensOnReq = FALSE;					/* ROUND運転でのサーボオン要求 */
	FnCmnCtrl->PdetReq = FALSE;						/* 磁極検出要求(通電要求) */
	FnCmnCtrl->JogSpeed = 0;						/* JOG速度指令 +:正転、-:逆転 */
	FnCmnCtrl->SafeStopTime = 0;					/* JOG安全停止時間 */

	FnCmnCtrl->ForceOutSignals = FALSE;				/* 強制信号出力指令 */
	FnCmnCtrl->KnlMode = KPI_KMD_NORMAL;			/* Tester mode */

	FnCmnCtrl->State.FnSvCtrlDisable = FALSE;		/* Fn運転不可状態 */
	FnCmnCtrl->State.LockNormalOpeDrive = FALSE;	/* Fn運転後、通常運転禁止フラグ	*/
	FnCmnCtrl->State.TuningRun = FALSE;				/* サーボ調整中 */
	FnCmnCtrl->State.TuningPctrlRun = FALSE;		/* サーボ調整P制御中 */

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
	AxisRscB->TaskProcessTime->SCtask_counter = 0;									/* Sigma7では未使用	*/
	AxisRscB->TaskProcessTime->SBCount = 0;
	AxisRscB->TaskProcessTime->SCtask_wakeup_time = KPI_SCCYCLEUS / SBcycle;		/* Sigma7では未使用	*/
	AxisRscB->TaskProcessTime->SBcountInMS = 1000 / SBcycle;
	AxisRscB->TaskProcessTime->SBCountInc = (SBcycle > (KPI_SBCYCLE_BASE_NS/1000)) ? 1 : 0;
}


/* <S14B> Start */
/****************************************************************************/
/*	速度リップル補償成分 電気角(PnFA0-PnFB2) 再計算処理						*/
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
	{	/* 電気角補正 */
		RippleCompCycle_ele[i] = Prm->RippleCompCycleE[i];
		RippleCompAmplitude_ele[i] = Prm->RippleCompAmplitudeE[i];
		RippleCompPhase_ele[i] = Prm->RippleCompPhaseE[i];
	}


/*--------------------------------------------------------------------------------------------------*/
/*		電気角補正パラメータ 																		*/
/*--------------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------*/
	/*		補正ゲイン	[%] 																		*/
	/*----------------------------------------------------------------------------------------------*/
	RippleCmp->RplCmpPrm.RplCmpPrmEle.CompGain = MlibScalKxgain( Prm->RippleCompGainE, 1, 100, NULL, 24 );

	for(i=0; i<RPLCMP_ORDER_ELE; i++)
	{
		RippleCmp->RplCmpPrm.RplCmpPrmEle.CompFrq[i] = RippleCompCycle_ele[i];
		/*------------------------------------------------------------------------------------------*/
		/*		補正振幅計算	[0.1%] --> [15000/MaxTrq]											*/
		/*------------------------------------------------------------------------------------------*/
		RippleCmp->RplCmpPrm.RplCmpPrmEle.CompAmpValue[i]  = BprmTorqueLevelCal( Bprm,
														(LONG)(RippleCompAmplitude_ele[i]), 1 );

		/*------------------------------------------------------------------------------------------*/
		/*		位相データ計算	[deg] --> [65536/360]												*/
		/*------------------------------------------------------------------------------------------*/
		RippleCmp->RplCmpPrm.RplCmpPrmEle.PhaseOfset[i]  = (USHORT)(SHORT)MlibScalKxgain
									((LONG)((SHORT)RippleCompPhase_ele[i]), 65536, 360, NULL,  -24 );
	}

	return;
}
/* <S14B> End */

/**************************** end of file **********************************/

