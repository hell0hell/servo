/****************************************************************************
  Description   : Global Data Definition for Mercury
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corporation.
                  Copyright (c) 2010 All Rights Reserved

  Project       : Mercury

----------------------------------------------------------------------------
  Changes		:
	Name	Date		Description
  ------------------------------------------------------------------------
	Y.Okqa	2011.02.01	created

****************************************************************************/
#include "Basedef.h"
#include "MercuryGlobals.h"
#include "JL086ServoIpReg.h"
#include "JL086ServoIpHw.h"
#include "BaseCmnStruct.h"
#include "AlarmManager.h"
#include "PrmAccessIf.h"
#include "FunManager.h"
#include "PnPrmListTbl.h"
#include "UnMonFunc.h"
#include "OpeRegFunc.h"
#include "InfRegFunc.h"
#include "FunManager.h"
#include "NetworkIf.h"
#include "EepromIF.h"
#include "SystemSetting.h"
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)
#include "ScanN.h"
#endif

#if 0 /* 2012.08.18 Y.Oka Make Mercury Base Software */
#include "SynqNetIF.h"
#endif /* 2012.08.18 Y.Oka Make Mercury Base Software */


/* 2012.08.06 Y.Oka Make Mercury Base Software */
/* ★暫定★ */
#define		GetBoardAxNum( )	MAX_AXIS_NUMBER	/* Get Axis Number by board ID */
/* 2012.08.06 Y.Oka Make Mercury Base Software */

#ifdef _VC_DEBUG_
extern	LONG	GetSvipTopAddress( );
#endif

/****************************************************************************************************/
/*																									*/
/*	Variables for Real-Time processing.																*/
/*	(The following data are locked to be always assigned to L2 Cache Area.)							*/
/*																									*/
/****************************************************************************************************/
#ifdef	CSW_L2_DCACHE_SECTION
#pragma ghs section bss=CSW_L2_DCACHE_SECTION
#endif

#pragma arm section rwdata = "Globals", zidata = "Globals"
#ifdef	_ASIP_DEBUG_BUF_USE_
USHORT				DebugBuf1[100];		/* デバッグ用											 *//* <TempSensorIC> */
USHORT				DebugBuf2[100];		/* デバッグ用											 *//* <TempSensorIC> */
USHORT				DebugBuf3[100];		/* デバッグ用											 *//* <TempSensorIC> */
USHORT				DebugBuf4[100];		/* デバッグ用											 *//* <TempSensorIC> */
USHORT				DebugBuf5[100];		/* デバッグ用											 *//* <TempSensorIC> */
USHORT				DebugBuf6[100];		/* デバッグ用											 *//* <TempSensorIC> */
USHORT				DebugBuf7[100];		/* デバッグ用											 *//* <TempSensorIC> */
USHORT				DebugBuf8[100];		/* デバッグ用											 *//* <TempSensorIC> */
USHORT				DebugBuf9[100];		/* デバッグ用											 *//* <TempSensorIC> */
USHORT				DebugBuf10[100];		/* デバッグ用											 *//* <TempSensorIC> */
USHORT				DebugBuf11[100];		/* デバッグ用											 *//* <TempSensorIC> */
USHORT				DebugBuf12[100];		/* デバッグ用											 *//* <TempSensorIC> */
USHORT				DebugBuf13[100];		/* デバッグ用											 *//* <TempSensorIC> */
USHORT				DebugBuf14[100];		/* デバッグ用											 *//* <TempSensorIC> */
INT					DebugCount;				/* デバッグ用											 *//* <TempSensorIC> */
INT					Debug_i;				/* wait用												 *//* <TempSensorIC> */
INT					Debug_j;				/* 														 *//* <TempSensorIC> */
#endif	//#ifdef	_ASIP_DEBUG_BUF_USE_

/* for debug */
LONG				DebugVariable[10];
/* for debug */

/* System Data */
LONG				NumberOfAxes;						/* 軸数設定									*/
static TASK_MEASURE_TIME	TaskProcessTime;					/* ScanB/ScanC処理時間計測用				*/
#if defined(_TEST_BOARD_) && !defined(_VC_DEBUG_)
LONG				ScanAStartCtr;						/* ScanA処理カウンタ							*/
LONG				ScanBStartCtr;						/* ScanB処理カウンタ							*/
LONG				ScanCStartCtr;						/* ScanC処理カウンタ							*/
#endif
static AXIS_HANDLE			SysAxisHandle[MAX_AXIS_NUMBER];		/* 軸制御演算用変数(論理軸アドレス用)		*/
static LIFETIME_HANDLE		LifeTimePrediction;					/* 寿命予測機能構造体 *//* <S154>  */
static DBLIFETIME_HADNLE	DBLifeTimeHdl[MAX_AXIS_NUMBER];		/* DB寿命予測機能				*//* <S180> */
#if 0	/* キャリア周波数の統一は行なわないため不要<S11E> */
#if ( defined(CSW_SGD7W_TYPE) || defined(CSW_COMMON_CARRIER) )		/* モータキャリア周波数切り替え対応<S117> */
static	AXIS_IF				AxisIf;								/* ユニット内軸間インタフェース構造体		*/
#endif	/* #if ( defined(CSW_SGD7W_TYPE) || defined(CSW_COMMON_CARRIER) ) */
#endif		/*#if 0	 キャリア周波数の統一は行なわないため不要<S11E> */

/* H/W関連 */
static ASICS				jl076_adr[MAX_AXIS_NUMBER];			/* JL-076アクセス用構造体					*/

static MENCV				EncData[MAX_AXIS_NUMBER];			/* エンコーダ読み出し情報					*/
static MENCV				FencData[MAX_AXIS_NUMBER];			/* フルクエンコーダ読み出し情報				*/
static SHAL_LATCH_STS_ADR	LtStsAdr[MAX_AXIS_NUMBER][3];		/* EXTラッチ信号用							*/
//static OH_INFO			Ohinfo[OHNUM];						/* 温度監視用データ							*//* <S03A> */
static PLSIFV 				LPlsIfV[MAX_AXIS_NUMBER];			/* <S00C> Command Pulse Interface			*/
static CADC					LAdc[MAX_AXIS_NUMBER];				/* <S00C> A/D Converter Interface Variable	*/
static RWALLMD 				EepPrmRwAllMng[MAX_AXIS_NUMBER];	/* <S00C> Parameter Download				*/
static RWPDLMD				EepPrmRwLcdMng[MAX_AXIS_NUMBER];	/* <S031> Lcd Parameter Copy				*/

/* ベースループ制御 */
static BASE_LOOPCTRLS		BaseLoops[MAX_AXIS_NUMBER];			/* 制御ループ演算用データ					*/
static CTRL_LOOP_OUT		CtrlLoopOut[MAX_AXIS_NUMBER];		/* ScanA出力データ							*/

/* ベースサーボ制御 */
static BASE_CTRL_HNDL		BaseCtrlData[MAX_AXIS_NUMBER];		/* 基本制御用共有データ						*/
static SERVO_CONTROL_IF		SvControlIf[MAX_AXIS_NUMBER];		/* オプションモジュール(M3,M2,Ana..) I/F	*/
static BASE_CTRL_OUT		BaseCtrlOut[MAX_AXIS_NUMBER];		/* ScanB出力データ							*/
static DETVIB				DetVib[MAX_AXIS_NUMBER];			/* 振動検出用変数							*/

static MOTSPDMAFIL			MotSpdMafil[MAX_AXIS_NUMBER];		/* モータ速度検出用移動平均演算用変数		*/

static PDET_ABSCONTROL		PdetAbsSeq[MAX_AXIS_NUMBER];		/* ABSスケール磁極検出用変数				*/
static MPFIND				MpFind[MAX_AXIS_NUMBER];			/* 磁極検出用変数							*/

/* ベースシーケンス制御 */
static SEQ_IO_HNDL			SeqIoSignal[MAX_AXIS_NUMBER];		/* シーケンスI/O */
static SEQ_CTRL_OUT			SeqCtrlOut[MAX_AXIS_NUMBER];		/* ScanC出力データ							*/
static BE_SEQ_HNDL			BeSeqData[MAX_AXIS_NUMBER];			/* Base Enable Sequence Handle				*/
static CHECK_ALARM			CheckAlarm[MAX_AXIS_NUMBER];		/* アラーム検出処理用構造体					*/
static SEQ_MOT_STS			SeqMotSts[MAX_AXIS_NUMBER];			/* ScanCにて取得・作成するモータ状態*/
static REMVIBFREQ			RemVibFreq[MAX_AXIS_NUMBER];		/* 残留振動周波数モニタ用データ				*/
static CALC_WATTAGE			CalcWattage[MAX_AXIS_NUMBER];		/* <S020> 消費電力演算用データ(軸毎)		*/
static ENVMON				EnvMonitor[MAX_AXIS_NUMBER];		/* 設置環境モニタデータ(軸毎)				*//* <S0ED> */
static POWER_MONITOR		PowerMonitor;						/* <S020> 消費電力モニタ用変数(ユニット共通)*/
static POW_MNG_HNDL			hPowerManager;						/* 電源管理用データ(ユニット共通)			*/
static ALMLATCH_HNDL		AlmLatchHndl[MAX_AXIS_NUMBER];		/* アラームラッチ用データ					*//* <S154> */
static CHECK_OH				OverHeat;							/* オーバーヒート監視用変数(ユニット共通)	*//* <S0ED> */
#if (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE)
static PNLOP_DATA_HANDLE	PnlOpLedDataHndl[MAX_AXIS_NUMBER];	/* ネットワーク用パネオペ構造体				*//* <S0A1> */
#endif

/* アラーム/レジスタ/Fn管理 */
static FUN_CMN_CONTROL		FunCmnControls[MAX_AXIS_NUMBER];	/* Fn機能共通制御データ						*/
static ALMMAN_HANDLE		AlarmManager[MAX_AXIS_NUMBER];		/* アラーム制御用構造体						*/
static TRCHNDL				TrcHndl;							/* データトレース用ハンドル					*/
/*--------------------------------------------------------------------------------------------------*/
static OPACCSV 				AccessOP[MAX_AXIS_NUMBER];			/* Operation Register Access Right <S166>	*/

/* アナログモニタ *//* <S016> */
static AMONOUT				AmonOut[MAX_AXIS_NUMBER][2];		/* アナログモニタ出力用構造体				*/
static UCHAR				ScanB250us;							/* ScanB 250us フラグ						*/

/* モータレステストモード関連	*//* <S1B7> */
static MOTLESSTST			MotorLessTest[MAX_AXIS_NUMBER];		/* モータレステストIF定義					*/

/* パラメータ/レジスタデータ格納バッファ */
static UNI_PRM_STR			prm_hndl[MAX_AXIS_NUMBER];			/* パラメータ用ハンドル						*/
static BPRMDAT				bprm_data[MAX_AXIS_NUMBER];			/* 共通基本パラメータ						*/
static ANLG_UN_MON_DATA		AnlgUnMonData[MAX_AXIS_NUMBER];		/* Amon/Unモニタ用データ					*/
static TEMP_MANAGER			TempManager;						/* 温度検出用データ							*//* <S03A> */

/* Network */
#if (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE)/* AnalogIFも下記構造体を利用するように変更する？	*/
static NETIF_AXIS_HANDLE	NetAxisIf[MAX_AXIS_NUMBER];			/* Network Interface Handle for each axis */
static NETIF_SYS_HANDLE		NetNodeIf;							/* Network Interface Handle for node device */
#endif
static LONG					NetIfMonDummy;

/* オプションモジュール	*//* <S04D> */
static OPTCARD_HNDL			OptCardHdl[MAX_AXIS_NUMBER];		/* オプションモジュール構造体へのポインタ	*/

#if 0 /* 2012.08.06 Y.Oka Make Mercury Base Software */
static TRIGIO_INFO			TrigInfo[TRIG_IO_MAX_TBL_NUMBER];	/* TrigIO制御用データ 						*/
#endif /* 2012.08.06 Y.Oka Make Mercury Base Software */

#ifdef	CSW_L2_DCACHE_SECTION
#pragma ghs section bss=default
#endif



/****************************************************************************************************/
/*																									*/
/*	Variables for Non-Real-Time processing.															*/
/*																									*/
/****************************************************************************************************/

//static AXIS_HANDLE			AxGlobals[MAX_AXIS_NUMBER];			/* 軸制御演算用変数(物理軸アドレス用)		*/

/* H/W関連 */
//static EEP_DEVICE_HANDLE	EepromDeviceHandle[MAX_AXIS_NUMBER];	/* EEPROM device handle	<S053>			*//* <S1D2> */
//<S053> static HW_AXIS_MAP			AsicMapInfo;						/* 軸番号とJL077ASIC番頭とのマップ情報		*/
static EEP_HEADER_INFO_T	UnitEepInfo[3];						/* IF/COA/MBU ID情報						*/
static MODULE_ID_INFO		ModuleIDInfo[MAX_AXIS_NUMBER];		/* Module ID情報							*/

/* Network */
//<S150>static NETMSG_IF			NetMsgServer;						/* Network Messages IF						*/

//#pragma arm section rwdata, zidata							/* TODO: Zantei */
static PRMDATA				usr_prm[MAX_AXIS_NUMBER];			/* ユーザパラメータ							*/
#pragma arm section rwdata, zidata						/* TODO: Zantei */
/* アラーム/レジスタ/Fn管理 */
static REGIF_CONFIG_T		PnRegConfig[MAX_AXIS_NUMBER];		/* Paramter Register						*/
static REGIF_CONFIG_T		FnRegConfig[MAX_AXIS_NUMBER];		/* Operation Register						*/
static REGIF_CONFIG_T		TrcRegConfig;						/* Trace buffer Register					*/
static REGIF_CONFIG_T		UnRegConfig[MAX_AXIS_NUMBER];		/* Monitor Register							*/
static REGIF_CONFIG_T		InfRegConfig[MAX_AXIS_NUMBER];		/* Information Register						*/
#if (CSW_NETIF_TYPE == NETIF_M3_TYPE) ||(CSW_NETIF_TYPE == NETIF_M2_TYPE)
static REGIF_CONFIG_T		NetworkIdRegConfig[MAX_AXIS_NUMBER];/* Network ID Register						*/
#endif
static REG_MANAGER_HANDLE	RegManagerHandle[MAX_AXIS_NUMBER];	/* レジスタアクセスIFハンドル				*/
static FUN_MANAGER			FunManager[MAX_AXIS_NUMBER];		/* Fn Managerハンドル						*/
static FUN_AXCOMMON			FunManagerAxisCommons;				/* Fn Manager用ユニット共通ハンドル			*/

/* パラメータ/レジスタデータ格納バッファ */
//static PRMDATA				usr_prm[MAX_AXIS_NUMBER];			/* ユーザパラメータ							*/
static CMN_PRMDATA			AxisCommonParams;					/* ユニット共通パラメータ					*/

#ifdef	__ghs__
#pragma ghs section bss=".fixed_buffer"
#endif
static USHORT				TraceBuffer[TRC_BUF_SIZE];		/* データトレースバッファ					*//* <S04C> */
static USHORT				TableRefBuffer[MAX_AXIS_NUMBER][AX_TBL_BUF_SIZE]; /* テーブル運転指令バッファ	*/
#ifdef	__ghs__
#pragma ghs section bss=default
#endif

/* Utility Functions */
static ANOTCHSEQ			ANotchSeq[MAX_AXIS_NUMBER];			/* 自動ノッチシーケンス用構造体				*/
static AUTONOTCH			ANotch[MAX_AXIS_NUMBER];			/* 自動ノッチ設定用構造体					*/
static FFTANALYZE			FftAna[MAX_AXIS_NUMBER];			/* 振動周波数解析(Online FFT)用構造体		*/
static MFCTUNE				MfcTune[MAX_AXIS_NUMBER];			/* モデルゲイン調整用構造体					*/
static VIBMON				VibMon[MAX_AXIS_NUMBER];			/* オンライン振動モニタ用構造体				*/
static INIVIBDET			IniVibDet[MAX_AXIS_NUMBER];			/* 振動検出レベル初期化用構造体				*/
static ONEPRMTUNE			OnePrmTune[MAX_AXIS_NUMBER];		/* ワンパラメータチューニング用構造体		*/
static ADATHNDL				AdatHndl[MAX_AXIS_NUMBER];			/* Advanced Auto Tuning Hanlde				*/
static AVIBTUNE				AvibTune[MAX_AXIS_NUMBER];			/* A型制振制御調整用構造体					*/
static EASYFFT				EasyFFT[MAX_AXIS_NUMBER];			/* Easy FFT用構造体							*/
static OPTCLR				OptClr[MAX_AXIS_NUMBER];			/* オプションカード検出クリア用構造体		*//* <S0BE> */


#if defined(_VC_DEBUG_)
UCHAR				DummySvip[0x8000];					/* JL-086H/Wダミーバッファ					*/
UCHAR				DummyEeprom[DEBUG_EEPROM_SIZE];		/* EEPROMダミーバッファ						*/
USHORT				DummyMicroSram[15000];				/* Micro download buffer					*/
#endif

/* <S074> */
DWORDX				ReadDummy;							/* Dummy Variable for Read 					*/

/*--------------------------------------------------------------------------------------------------*/



#pragma arm section code = "CALL_SCANA" /* <S1D2> */
/*******************************************************************************
*  Function    : GetAxisHandle
*  Parameter   :
*		axis_number	- Axis number
*  Return      :
*		Pointer to the AXIS_HANDLE
*******************************************************************************/
void* GetAxisHandle(UINT axis_number)
{
	return (&SysAxisHandle[axis_number]);
}
#pragma arm section code /* <S1D2> */



/****************************************************************************************************/
/*																									*/
/*		ASICポインタ & 軸数設定処理																	*/
/*																									*/
/****************************************************************************************************/
#if 1			/* <S053> */
static void InitializeSvASICPointer( void )
{
	UINT			ax;
	ASIC_HW_IF_STR	*AsicHwReg;

	/* Check the Number of Axes by reading JL-077 ID */
	for( ax = 0; ax < MAX_AXIS_NUMBER; ax++ )
	{
		AsicHwReg = (ASIC_HW_IF_STR*)(HW_SVASIC_ADDRESS + (JL086_SVIP_OFS * ax));
		jl076_adr[ax].AsicHwReg	= AsicHwReg;
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W（2軸対応）のみ	*//* <S0E4> */
		jl076_adr[ax].AsicMicroReg = (ASIC_U_IF_STR*)((UCHAR*)HW_SVASIC_ADDRESS + JL086_MICRO_OFS + JL086_MICRO_AXIS_OFS*ax);
#elif CSW_BOARD_TYPE == SGD7S_TYPE	/*	SGD7S（単軸時）のみ	*/
		jl076_adr[ax].AsicMicroReg = (ASIC_U_IF_STR*)((UCHAR*)AsicHwReg + JL086_MICRO_OFS);
#endif
//Mercury未使用？		jl076_adr[ax].AsicComHwReg = (ASIC_HW_IF_STR*)(HW_SVASIC_ADDRESS + HW_SVASIC_COMADR_OFS);
//Mercury未使用？		jl076_adr[ax].AsicComMicroReg = (ASIC_U_IF_STR* )(HW_SVASIC_ADDRESS
//													+ HW_SVASIC_COMADR_OFS + JL086_MICRO_OFS);
	}
}
#else
static LONG	InitializeSvASICPointer( void *SvAsicAddress, HW_AXIS_MAP *AxisMap )
{
	VLONG		ax;
	VLONG		ax_cnt;
	USHORT		no;

	/* The axis no is ordered by physical slot number. */
	for( ax = 0; ax < MAX_SLOT_NUMBER; ax++ )
	{
		AxisMap->asicNo[ax] = (USHORT)ax;
	}

	/* Set the Asic Pointer */
	for( ax = 0; ax < MAX_SLOT_NUMBER; ax++ )
	{
		no = AxisMap->asicNo[ax];
		jl076_adr[ax].AsicHwReg	= (ASIC_HW_IF_STR*)((UCHAR*)SvAsicAddress
													+ (JL086_SVIP_OFS * no));
		jl076_adr[ax].AsicMicroReg = (ASIC_U_IF_STR*)((UCHAR*)SvAsicAddress
													+ JL086_MICRO_OFS + (JL086_MICRO_AXIS_OFS * no));
		AxisMap->asicAddr[ax] = jl076_adr[ax].AsicHwReg;

#if defined(_VC_DEBUG_) /* 2012.08.24 Y.Oka Make Mercury Base Software */
		jl076_adr[ax].AsicHwReg	= (ASIC_HW_IF_STR*)((UCHAR*)DummySvip
													+ (JL086_SVIP_OFS * no));
		AxisMap->asicAddr[ax] = jl076_adr[ax].AsicHwReg;
		jl076_adr[ax].pDummyMicroSram	= DummyMicroSram;
#endif /* _VC_DEBUG_ */ /* 2012.08.24 Y.Oka Make Mercury Base Software */

	}

	/* 2012.08.06 Y.Oka Make Mercury Base Software */
	/* Check the Number of Axises by Board ID */
	/* ★現状は仕様未決定のため、ダミーマクロとする★ */
	/* ★ここでは機種単位での軸数設定とし、実際の制御軸数はパラメータ設定も加味する★ */
	ax_cnt = GetBoardAxNum( );

	/* Set asicNum Information */
	AxisMap->asicNum = ax_cnt;

	if((ax_cnt == 0) || (ax_cnt > MAX_AXIS_NUMBER))
	{
		ax_cnt = MAX_AXIS_NUMBER;
	}

	return ax_cnt;
}
#endif


/****************************************************************************************************/
/*																									*/
/*		レジスタアクセス：コンフィギュレーションデータ設定											*/
/*																									*/
/****************************************************************************************************/
//static void InitializeRegisterHandle(HW_AXIS_MAP *AxisMap)
static void InitializeRegisterHandle( void )		/* <S053> */
{
	LONG ax /* , no <S053> */;
	AXIS_HANDLE *pAxis;

	// Set parameter ram pointer
	for( ax = 0; ax < MAX_AXIS_NUMBER; ax++ )
	{
		prm_hndl[ax].Prm	 = &usr_prm[ax];
		prm_hndl[ax].Bprm	 = &bprm_data[ax];
		prm_hndl[ax].Cprm	 = &AxisCommonParams;
	}

	// Configure to Trace register interface
//	TrcRegConfig.hEepDev = NULL;/* <S1D2> */
	TrcRegConfig.PrmListTable = TrcRegTable;
	TrcRegConfig.NumOfTableEntry = TrcRegTableEntNum;
	TrcRegConfig.PrmCalParam = &SysAxisHandle[0];
	TrcRegConfig.GetRamPtrParam = TraceBuffer;
	TrcRegConfig.EepromBlockAddress = 0;
	TrcRegConfig.SysLevel = &FunManagerAxisCommons.AccessLevel;
	TrcRegConfig.AlmManager = &AlarmManager[0];				/* <S0AA> 暫定 */

	for( ax = 0, pAxis = &SysAxisHandle[0]; ax < MAX_AXIS_NUMBER; ax++, pAxis++ )
	{
//<S053>		no = AxisMap->asicNo[ax];
		// Configure to Pn register interface
//		PnRegConfig[ax].hEepDev = &EepromDeviceHandle[ax];		/* <S053> *//* <S1D2> */
		PnRegConfig[ax].PrmListTable = PnPrmTbl;
		PnRegConfig[ax].NumOfTableEntry = PnPrmTblEntNum;
		PnRegConfig[ax].PrmCalParam = pAxis;
		PnRegConfig[ax].GetRamPtrParam = pAxis;
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W（2軸対応）のみ						*//* <S0E4> */
		PnRegConfig[ax].EepromBlockAddress = (USHORT)(EEP_AXPRM_TOP_ADDRESS
													+ (EEP_MAP_AXIS_OFFSET * ax));
#elif CSW_BOARD_TYPE == SGD7S_TYPE	/*	SGD7S（単軸時）のみ	*/
		PnRegConfig[ax].EepromBlockAddress = (USHORT)(EEP_AXPRM_TOP_ADDRESS
													+ (EEP_MAP_BLOCK_LENGTH * ax /* <S053> no */));
#endif
		PnRegConfig[ax].SysLevel = &FunManagerAxisCommons.AccessLevel;
		PnRegConfig[ax].AlmManager = &AlarmManager[ax];					/* <S0AA> */
		// Configure to Fn register interface
//		FnRegConfig[ax].hEepDev = NULL;/* <S1D2> */
		FnRegConfig[ax].PrmListTable = OpeRegTable;
		FnRegConfig[ax].NumOfTableEntry = OpeRegTableEntNum;
		FnRegConfig[ax].PrmCalParam = pAxis;
		FnRegConfig[ax].GetRamPtrParam = pAxis;
		FnRegConfig[ax].EepromBlockAddress = 0;
		FnRegConfig[ax].SysLevel = &FunManagerAxisCommons.AccessLevel;
		FnRegConfig[ax].AlmManager = &AlarmManager[ax];					/* <S0AA> */
		// Configure to Un register interface
//		UnRegConfig[ax].hEepDev = NULL;/* <S1D2> */
		UnRegConfig[ax].PrmListTable = UnRegTable;
		UnRegConfig[ax].NumOfTableEntry = UnRegTableEntNum;
		UnRegConfig[ax].PrmCalParam = pAxis;
		UnRegConfig[ax].GetRamPtrParam = pAxis;
		UnRegConfig[ax].EepromBlockAddress = 0;
		UnRegConfig[ax].SysLevel = &FunManagerAxisCommons.AccessLevel;
		UnRegConfig[ax].AlmManager = &AlarmManager[ax];					/* <S0AA> */
		// Configure to Inf register interface
//		InfRegConfig[ax].hEepDev = NULL;/* <S1D2> */
		InfRegConfig[ax].PrmListTable = InfoRegTable;
		InfRegConfig[ax].NumOfTableEntry = InfoRegTableEntNum;
		InfRegConfig[ax].PrmCalParam = pAxis;
		InfRegConfig[ax].GetRamPtrParam = pAxis;
		InfRegConfig[ax].EepromBlockAddress = 0;
		InfRegConfig[ax].SysLevel = &FunManagerAxisCommons.AccessLevel;
		InfRegConfig[ax].AlmManager = &AlarmManager[ax];					/* <S0AA> */


#if (CSW_NETIF_TYPE == NETIF_M3_TYPE)|| (CSW_NETIF_TYPE == NETIF_M2_TYPE)
		// Configure to Network register interface
//		NetworkIdRegConfig[ax].hEepDev = NULL;/* <S1D2> */
		NetworkIdRegConfig[ax].PrmListTable = NetworkIdRegTable;
		NetworkIdRegConfig[ax].NumOfTableEntry = NetworkIdRegTableEntNum;
		NetworkIdRegConfig[ax].PrmCalParam = &SysAxisHandle[ax];
		NetworkIdRegConfig[ax].GetRamPtrParam = &SysAxisHandle[ax];
		NetworkIdRegConfig[ax].EepromBlockAddress = 0;
		NetworkIdRegConfig[ax].SysLevel = &FunManagerAxisCommons.AccessLevel;
		NetworkIdRegConfig[ax].AlmManager = &AlarmManager[ax];					/* <S0AA> */
#endif

		// Configure to Register Manager Handle
		RegManagerHandle[ax].hPnReg = &PnRegConfig[ax];
		RegManagerHandle[ax].hFnReg = &FnRegConfig[ax];
//		RegManagerHandle[ax].hEngReg = NULL;
		RegManagerHandle[ax].hTrcReg = &TrcRegConfig;
		RegManagerHandle[ax].hUnReg = &UnRegConfig[ax];
		RegManagerHandle[ax].hInfReg = &InfRegConfig[ax];
#if (CSW_NETIF_TYPE == NETIF_M3_TYPE)|| (CSW_NETIF_TYPE == NETIF_M2_TYPE)
		RegManagerHandle[ax].hNetworkIdReg = &NetworkIdRegConfig[ax];
#endif
	}

//	FunManagerAxisCommons.AccessLevel = 1;				/* <S20B> */
	FunManagerAxisCommons.AccessLevel = ACCLVL_USER2;	/* <S20B> */
}

/****************************************************************************************************/
/*																									*/
/*		モニタ選択管理テーブル作成																	*/
/*																									*/
/****************************************************************************************************/
static void	InitializeNetIfMonitorPointer( void )
{
	LONG				ax_no, i;
	ANLG_UN_MON_DATA	*pMonData;
	BASE_CTRL_HNDL		*pBaseCtrl;
	SERVO_CONTROL_IF	*SvControlIf;
	MENCV				*MencV;
	REGENE_CTRL 		*pRegCtrl;/* <S123> */
	LIFETIME_HANDLE		*hLifeTime;		/* 寿命予測機能		  <S1D7> */
	DBLIFETIME_HADNLE	*hDBLifeTime;	/* DB寿命予測機能	   <S1D7> */
	POWER_MONITOR		*PowerMonitor;	/* 消費電力モニタ変数  <S1D7> */
	AXIS_HANDLE			*pAxis;

	NetIfMonDummy = 0;
	for( ax_no = 0, pAxis = &SysAxisHandle[0]; ax_no < MAX_AXIS_NUMBER; ax_no++, pAxis++ )
	{
		/* ローカル変数の初期化 */
		pMonData	= pAxis->UnMonitor;
		pBaseCtrl	= pAxis->BaseControls;
		SvControlIf	= pAxis->SvControlIf;
		MencV		= pAxis->MencV;
		pRegCtrl	= &pAxis->PowerManager->RegeneCtrl;/*<S123>*/
		hLifeTime = pAxis->hLifeTime;		/* <S1D7> */
		hDBLifeTime = pAxis->hDBLifeTime;	/* <S1D7> */
		PowerMonitor = pAxis->PowerMonitor;	/* <S1D7> */

		for( i = 0; i < sizeof(SvControlIf->MonRamTable)/4; i++ )
		{
			SvControlIf->MonRamTable[i] = &NetIfMonDummy;
		}
		/*---- モニタデータ0x0* ------------------------------------------------------------------------------	*/
		SvControlIf->MonRamTable[0x00] = (LONG *)&( pBaseCtrl->CtrlCmdMngr.SpdFbki );
		SvControlIf->MonRamTable[0x01] = (LONG *)&( pBaseCtrl->CtrlCmdMngr.SpdRefo );
		SvControlIf->MonRamTable[0x02] = (LONG *)&( pAxis->CtrlLoopOut->TrqRefMon );
		/* μプログラムからもらう偏差は32ビットのため、現状high側は無効とする */
		SvControlIf->MonRamTable[0x03] = (LONG *)&( pBaseCtrl->PosManager.var.PosErrA.per32s );/* <S107> *//* <S14C> */
		SvControlIf->MonRamTable[0x0a] = (LONG *)&( pBaseCtrl->PosManager.MencPos.apos[0] );
		SvControlIf->MonRamTable[0x0b] = (LONG *)&( pBaseCtrl->PosManager.MencPos.apos[1] );
		SvControlIf->MonRamTable[0x0e] = (LONG *)&( pBaseCtrl->PosManager.PgPos.apos[0] );
		SvControlIf->MonRamTable[0x0f] = (LONG *)&( pBaseCtrl->PosManager.PgPos.apos[1] );

		/*---- モニタデータ0x1* ------------------------------------------------------------------------------	*/
		SvControlIf->MonRamTable[0x10] = (LONG *)&( pMonData->UnMotSpd );		/* Un000 */
		SvControlIf->MonRamTable[0x11] = (LONG *)&( pMonData->UnSpdRef );		/* Un001 */
		SvControlIf->MonRamTable[0x12] = (LONG *)&( pMonData->UnTrqRef );		/* Un002 */
		SvControlIf->MonRamTable[0x13] = (LONG *)&( pMonData->UnMotAngle );		/* Un003 */
		SvControlIf->MonRamTable[0x14] = (LONG *)&( pMonData->UnMotPhase );		/* Un004 */
		SvControlIf->MonRamTable[0x17] = (LONG *)&( pMonData->UnPcmd64ms );		/* Un007 */
		SvControlIf->MonRamTable[0x18] = (LONG *)&( pBaseCtrl->PosManager.var.PosErrA.per32s );	/* Un008 *//* <S107> *//* <S14C> */
		SvControlIf->MonRamTable[0x19] = (LONG *)&( pMonData->UnIsqrSum );		/* Un009 */
//		SvControlIf->MonRamTable[0x1a] = (LONG *)&( pMonData->UnMeanRgPower );	/* Un00A */
		SvControlIf->MonRamTable[0x1a] = (LONG *)&( pRegCtrl->UnMeanRgPower );	/* Un00A *//* <S123> */
		SvControlIf->MonRamTable[0x1b] = (LONG *)&( pMonData->UnMeanDbPower );	/* Un00B */
		SvControlIf->MonRamTable[0x1d] = (LONG *)&( pMonData->UnPfbkCntr );		/* Un00D */

		SvControlIf->MonRamTable[0x23] = (LONG *)&( MencV->SenOnMtnData );
		SvControlIf->MonRamTable[0x24] = (LONG *)&( MencV->SenOnIncPulse );
		SvControlIf->MonRamTable[0x25] = (LONG *)&( MencV->SenOnScalePosL );
		SvControlIf->MonRamTable[0x26] = (LONG *)&( MencV->SenOnScalePosH );
		SvControlIf->MonRamTable[0x28] = (LONG *)&( MencV->Temperature );/* <S1D7> *//* <S1DE> */

		/*---- モニタデータ0x3* ------------------------------------------------------------------------------	*/
		SvControlIf->MonRamTable[0x30] = (LONG *)&( SvControlIf->MonCphaLpos[0] );
		SvControlIf->MonRamTable[0x31] = (LONG *)&( SvControlIf->MonCphaLpos[1] );
		SvControlIf->MonRamTable[0x32] = (LONG *)&( SvControlIf->MonExt1Lpos[0] );
		SvControlIf->MonRamTable[0x33] = (LONG *)&( SvControlIf->MonExt1Lpos[1] );
		SvControlIf->MonRamTable[0x34] = (LONG *)&( SvControlIf->MonExt2Lpos[0] );
		SvControlIf->MonRamTable[0x35] = (LONG *)&( SvControlIf->MonExt2Lpos[1] );
		SvControlIf->MonRamTable[0x36] = (LONG *)&( SvControlIf->MonExt3Lpos[0] );
		SvControlIf->MonRamTable[0x37] = (LONG *)&( SvControlIf->MonExt3Lpos[1] );
		SvControlIf->MonRamTable[0x38] = (LONG *)&( pBaseCtrl->PosManager.VirtualPositionError );// 仮想位置偏差 <S14C>
#if (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE)
//		SvControlIf->MonRamTable[0x39] = (LONG *)&( pBaseCtrl->PosManager.PosErrAmp.per32s );// 仮想位置アンプ偏差?
		/*--------------------------------------------------------------------------------------------------*/
		/*		SGDVでは、SI0~SI7以外に以下を設定する。														*/
		/* 		PgAphase/PgBphase/PgCphase																	*/
		/* 			：SGD7では、未実装であり、NetIfMonInSignalに設定するかRspSigBitに設定するか検討が必要	*/
		/* 		BBon1/BBon2：SGD7では、RspSigBitに設定されている。											*/
		/*--------------------------------------------------------------------------------------------------*/
//		SvControlIf->MonRamTable[0x3a] = (LONG *)&( (SysAxisHandle[ax_no].IoSignals)->HwSin );/* <S0AC> *//* <S1C0>:DEL */
		SvControlIf->MonRamTable[0x3a] = (LONG *)&( SysAxisHandle[ax_no].SvControlIf->NetIfMonInSignal1 );/* <S1C0> */
		SvControlIf->MonRamTable[0x3b] = NULL;								 /* 3B: Alarm, Warning Code *//* <S1C0> */
		SvControlIf->MonRamTable[0x3c] = (LONG *)&( SysAxisHandle[ax_no].SvControlIf->NetIfMonInSignal2 );/* <S1C0> */
#endif
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)
		/* 
		 * @@ Monitors for Analog Interface are temporary defined as follows
		 * 	0x3A: Hardware Input Signal
		 * 	0x3B: Not Used
		 * 	0x3C: AD2(Torque Reference)
		 * 	0x3D: AD1(Speed Reference)
		 * 	0x3E: Clear Signal Latch Position
		 * 	0x3F: Pulse Counter
		 * */
		SvControlIf->MonRamTable[0x3a] = (LONG *)&( pAxis->IoSignals->HwSin ); 			
		SvControlIf->MonRamTable[0x3b] = NULL /* */;
		SvControlIf->MonRamTable[0x3c] = &(LAdc[ax_no].Var[ADC1_TREFAD]);
		SvControlIf->MonRamTable[0x3d] = &(LAdc[ax_no].Var[ADC0_VREFAD]);
		SvControlIf->MonRamTable[0x3e] = &(pBaseCtrl->MonPerrLpos[0]);
		SvControlIf->MonRamTable[0x3f] = &(LPlsIfV[ax_no].PcmdRingCntr); 
#endif


		/*---- モニタデータ0x4* ------------------------------------------------------------------------------	*//* <S1D7> */
		SvControlIf->MonRamTable[0x40] = (LONG *)&( pMonData->UnInstEnvMonAmp );						/* Un025 */
		SvControlIf->MonRamTable[0x41] = (LONG *)&( pMonData->UnInstEnvMonMot );						/* Un026 */
		SvControlIf->MonRamTable[0x42] = (LONG *)&( hLifeTime->FanLifeTimeHdl.var.FanSurvivor );		/* Un027 */
		SvControlIf->MonRamTable[0x43] = (LONG *)&( hLifeTime->CapaLifeTimeHdl.var.CapaSurvivor );		/* Un028 */
		SvControlIf->MonRamTable[0x44] = (LONG *)&( hLifeTime->RushLifeTimeHdl.var.RushSurvivor );		/* Un029 */
		SvControlIf->MonRamTable[0x45] = (LONG *)&( hDBLifeTime->var.DBSurvivor );						/* Un02A */
		SvControlIf->MonRamTable[0x46] = (LONG *)&( PowerMonitor->var.UnConsumedPower );				/* Un032 */
		SvControlIf->MonRamTable[0x47] = (LONG *)&( PowerMonitor->var.UnEnergyConsumption );			/* Un033 */
		SvControlIf->MonRamTable[0x48] = (LONG *)&( PowerMonitor->var.UnCumulativeEnergyI );			/* Un034 */

	}
}


/****************************************************************************************************/
/*																									*/
/*		Initialize the Axis global data pointer														*/
/*																									*/
/****************************************************************************************************/
AXIS_HANDLE *InitializeAxisGlobals( void )
{
	LONG		ax_no;
//<S053>	void		*SvAsicPtr;				/* JL-076/077 MemoryRegionへのポインタ			*/
	AXIS_HANDLE	*ax_globals;

	NumberOfAxes = MAX_AXIS_NUMBER;				/* <S053> */
	InitializeSvASICPointer( );					/* <S053> */
#if 0											/* <S053> */
	/* ASICポインタ設定 */
	/* JL-076B Host IF Address取得 */
#ifdef _VC_DEBUG_
	SvAsicPtr = (void*)GetSvipTopAddress( );
#else
//	SvAsicPtr = GetVirtualMemoryRegionAddress("mbboard");
	SvAsicPtr = (void*)HW_SVASIC_ADDRESS;
#endif
	NumberOfAxes = InitializeSvASICPointer( SvAsicPtr, &AsicMapInfo );
#endif

	/* AXISデータポインタ初期化 */
	for( ax_no = 0; ax_no < MAX_AXIS_NUMBER; ax_no++ )
	{
		/* ローカル変数の初期化 */
		ax_globals = &SysAxisHandle[ax_no];

		/* 軸番号 */
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W（2軸対応）のみ	*//* <S0E4> */
		ax_globals->AxisNo = ax_no;		/*	０：１軸目、１：２軸目	*/
		ax_globals->SlotNo = 0;			/*	スロット番号は０固定	*/
#elif CSW_BOARD_TYPE == SGD7S_TYPE	/*	SGD7S（単軸時）のみ	*/
		ax_globals->AxisNo = ax_no;
		ax_globals->SlotNo = ax_no;
#endif

		/* ベースサーボ制御用データ(ScanA) */
		ax_globals->BaseLoops 					= &BaseLoops[ax_no];
		ax_globals->CtrlLoopOut					= &CtrlLoopOut[ax_no];

		/* ベースサーボ制御用データ(ScanB) */
		ax_globals->BaseControls				= &BaseCtrlData[ax_no];
		ax_globals->SvControlIf					= &SvControlIf[ax_no];
		ax_globals->BaseCtrlOut					= &BaseCtrlOut[ax_no];
		ax_globals->DetVib						= &DetVib[ax_no];
		ax_globals->TableRefBuffer				= &(TableRefBuffer[ax_no][0]);

		ax_globals->MencV						= &EncData[ax_no];
		ax_globals->FencV						= &FencData[ax_no];
		ax_globals->SvAsicRegs					= &jl076_adr[ax_no];
		ax_globals->LtStsAdr					= &(LtStsAdr[ax_no][0]);
//		ax_globals->OhInfo						= Ohinfo;					/* <S03A> */
		ax_globals->pPlsIfV						= &LPlsIfV[ax_no];			/* <S00C> */
		ax_globals->pAdc						= &LAdc[ax_no];				/* <S00C> */
		ax_globals->pRwAllMng					= &EepPrmRwAllMng[ax_no];	/* <S00C> */
		ax_globals->pRwPdlMng					= &EepPrmRwLcdMng[ax_no];	/* <S031> */

		ax_globals->MotSpdMafil					= &MotSpdMafil[ax_no];

		ax_globals->PdetAbsSeq					= &PdetAbsSeq[ax_no];
		ax_globals->MpFind						= &MpFind[ax_no];

		/* サーボシーケンス制御用データ(ScanC) */
		ax_globals->IoSignals					= &SeqIoSignal[ax_no];
		ax_globals->SeqCtrlOut					= &SeqCtrlOut[ax_no];
		ax_globals->BeSequence					= &BeSeqData[ax_no];
		ax_globals->CheckAlarm					= &CheckAlarm[ax_no];
		ax_globals->SeqMotSts					= &SeqMotSts[ax_no];
		ax_globals->RemVibFreq					= &RemVibFreq[ax_no];
		ax_globals->CalcWattage					= &CalcWattage[ax_no];		/* <S02C> */
		ax_globals->EnvMonitor					= &EnvMonitor[ax_no];		/* <S0ED> */
		ax_globals->PowerMonitor				= &PowerMonitor;			/* <S02C> */
		ax_globals->PowerManager				= &hPowerManager;
		ax_globals->OverHeat					= &OverHeat;				/* <S0ED> */
		ax_globals->hLifeTime					= &LifeTimePrediction;		/* <S154> */
		ax_globals->hDBLifeTime					= &DBLifeTimeHdl[ax_no];	/* <S180> */
		ax_globals->hAlmLatchHndl				= &AlmLatchHndl[ax_no];		/* <S154> */
#if (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE)/* AnalogIFも下記構造体を利用するように変更する？	*/
		ax_globals->PnlOpLedDataHndl			= &PnlOpLedDataHndl[ax_no];	/* <S0A1> */
#endif
		/* Network */
#if (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE)/* AnalogIFも下記構造体を利用するように変更する？	*/
		ax_globals->NetAxisIf					= &NetAxisIf[ax_no];
		ax_globals->NetNodeIf					= &NetNodeIf;
#endif
#if (CSW_NETIF_TYPE == NETIF_SYNQNET_TYPE)
		SqnetHandle[ax_no].Sqnet_NodeCommons	= &Sqnet_NodeCommons;
		ax_globals->SqnetHandle					= &SqnetHandle[ax_no];
#endif

		/* オプションモジュール	*//* <S04D> */
		ax_globals->OptCardHdl					= &OptCardHdl[ax_no];

		/* Servo Resource Managers */
		ax_globals->RegManager					= &RegManagerHandle[ax_no];
		ax_globals->UniPrms						= &prm_hndl[ax_no];
		ax_globals->AlmManager					= &AlarmManager[ax_no];
		ax_globals->FnCmnCtrl					= &FunCmnControls[ax_no];
		ax_globals->FnManager					= &FunManager[ax_no];
		ax_globals->FnAxCommons					= &FunManagerAxisCommons;

		/* Utility Functions */
		ax_globals->ANotchSeq					= &ANotchSeq[ax_no];
		ax_globals->ANotch						= &ANotch[ax_no];
		ax_globals->FftAna						= &FftAna[ax_no];
		ax_globals->MfcTune						= &MfcTune[ax_no];
		ax_globals->VibMon						= &VibMon[ax_no];
		ax_globals->IniVibDet					= &IniVibDet[ax_no];
		ax_globals->OnePrmTune					= &OnePrmTune[ax_no];
		ax_globals->AdatHndl					= &AdatHndl[ax_no];
		ax_globals->AvibTune					= &AvibTune[ax_no];
		ax_globals->EasyFFT						= &EasyFFT[ax_no];
		ax_globals->OptClr						= &OptClr[ax_no];/* <S0BE> */

		ax_globals->TempManager					= &TempManager;				/* <S03A> */
		ax_globals->UnMonitor					= &AnlgUnMonData[ax_no];
		ax_globals->TaskProcessTime				= &TaskProcessTime;

#if 0	/* キャリア周波数の統一は行なわないため不要<S11E> */
#if ( defined(CSW_SGD7W_TYPE) || defined(CSW_COMMON_CARRIER) )		/* モータキャリア周波数切り替え対応<S117> */
		ax_globals->AxisIf						= &AxisIf;
#endif	/* #if ( defined(CSW_SGD7W_TYPE) || defined(CSW_COMMON_CARRIER) ) */
#endif		/*#if 0	 キャリア周波数の統一は行なわないため不要<S11E> */

		/* MEMOBUS Message */
//<S150>		ax_globals->NetMsgIf					= &NetMsgServer;

		/* Data Trace Handle */
		ax_globals->TrcHndl 					= &TrcHndl;

		/* Data Trace Buffer */
		ax_globals->TraceBuffer  				= TraceBuffer;

		/* Analog Monitor *//* <S016> */
//		ax_globals->AmonOut						= &(AmonOut[ax_no][0]);	/* <S0E4> */
		ax_globals->AmonOut						= &(AmonOut[0][0]);		/* <S0E4> */
		ax_globals->ScanB250us					= &ScanB250us;

		/* モータレステストIF定義 *//* <S1B7> */
		ax_globals->MotorLessTest				= &MotorLessTest[ax_no];

		/* Module ID */
//<S053>		ModuleIDInfo[ax_no].AsicNoMap 			= &AsicMapInfo;
		ModuleIDInfo[ax_no].CoaInfo				= &UnitEepInfo[0];
		ModuleIDInfo[ax_no].MbuInfo				= &UnitEepInfo[1];
		ModuleIDInfo[ax_no].IfInfo				= &UnitEepInfo[2];
		ax_globals->ModuleID					= &ModuleIDInfo[ax_no];

#if 0 /* 2012.08.06 Y.Oka Make Mercury Base Software */
		/* Application */
		ax_globals->TrigInfo					= &TrigInfo[0];
#endif /* 2012.08.06 Y.Oka Make Mercury Base Software */

#if defined(_VC_DEBUG_) /* 2012.08.23 Y.Oka Make Mercury Base Software */
		ax_globals->pDummyEeprom				= DummyEeprom;
#endif /* _VC_DEBUG_ */ /* 2012.08.23 Y.Oka Make Mercury Base Software */

		/* <S166> Operator Access Control */
		ax_globals->pAccessOp					= &AccessOP[ax_no];
	}

	/* モニタ選択管理テーブル作成 */
	InitializeNetIfMonitorPointer();

	/* レジスタハンドラ初期化 */
//<S053>	InitializeRegisterHandle( &AsicMapInfo );
	InitializeRegisterHandle( );		/* <S053> */

	/* 論理軸情報初期化 */
#if 0	/* RAM確保 */
	for( ax_no = 0; ax_no < MAX_AXIS_NUMBER; ax_no++ )
	{
		SysAxisHandle[ax_no] = AxGlobals[ax_no];
	}
#endif

#if defined(_TEST_BOARD_) && !defined(_VC_DEBUG_)
	ScanAStartCtr = 0;						/* ScanA処理カウンタ							*/
	ScanBStartCtr = 0;						/* ScanB処理カウンタ							*/
	ScanCStartCtr = 0;						/* ScanC処理カウンタ							*/
#endif

	return (&SysAxisHandle[0]);
}



/***************************************** end of file **********************************************/
