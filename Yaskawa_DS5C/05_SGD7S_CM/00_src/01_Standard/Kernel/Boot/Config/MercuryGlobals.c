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
/* ���b�聚 */
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
USHORT				DebugBuf1[100];		/* �f�o�b�O�p											 *//* <TempSensorIC> */
USHORT				DebugBuf2[100];		/* �f�o�b�O�p											 *//* <TempSensorIC> */
USHORT				DebugBuf3[100];		/* �f�o�b�O�p											 *//* <TempSensorIC> */
USHORT				DebugBuf4[100];		/* �f�o�b�O�p											 *//* <TempSensorIC> */
USHORT				DebugBuf5[100];		/* �f�o�b�O�p											 *//* <TempSensorIC> */
USHORT				DebugBuf6[100];		/* �f�o�b�O�p											 *//* <TempSensorIC> */
USHORT				DebugBuf7[100];		/* �f�o�b�O�p											 *//* <TempSensorIC> */
USHORT				DebugBuf8[100];		/* �f�o�b�O�p											 *//* <TempSensorIC> */
USHORT				DebugBuf9[100];		/* �f�o�b�O�p											 *//* <TempSensorIC> */
USHORT				DebugBuf10[100];		/* �f�o�b�O�p											 *//* <TempSensorIC> */
USHORT				DebugBuf11[100];		/* �f�o�b�O�p											 *//* <TempSensorIC> */
USHORT				DebugBuf12[100];		/* �f�o�b�O�p											 *//* <TempSensorIC> */
USHORT				DebugBuf13[100];		/* �f�o�b�O�p											 *//* <TempSensorIC> */
USHORT				DebugBuf14[100];		/* �f�o�b�O�p											 *//* <TempSensorIC> */
INT					DebugCount;				/* �f�o�b�O�p											 *//* <TempSensorIC> */
INT					Debug_i;				/* wait�p												 *//* <TempSensorIC> */
INT					Debug_j;				/* 														 *//* <TempSensorIC> */
#endif	//#ifdef	_ASIP_DEBUG_BUF_USE_

/* for debug */
LONG				DebugVariable[10];
/* for debug */

/* System Data */
LONG				NumberOfAxes;						/* �����ݒ�									*/
static TASK_MEASURE_TIME	TaskProcessTime;					/* ScanB/ScanC�������Ԍv���p				*/
#if defined(_TEST_BOARD_) && !defined(_VC_DEBUG_)
LONG				ScanAStartCtr;						/* ScanA�����J�E���^							*/
LONG				ScanBStartCtr;						/* ScanB�����J�E���^							*/
LONG				ScanCStartCtr;						/* ScanC�����J�E���^							*/
#endif
static AXIS_HANDLE			SysAxisHandle[MAX_AXIS_NUMBER];		/* �����䉉�Z�p�ϐ�(�_�����A�h���X�p)		*/
static LIFETIME_HANDLE		LifeTimePrediction;					/* �����\���@�\�\���� *//* <S154>  */
static DBLIFETIME_HADNLE	DBLifeTimeHdl[MAX_AXIS_NUMBER];		/* DB�����\���@�\				*//* <S180> */
#if 0	/* �L�����A���g���̓���͍s�Ȃ�Ȃ����ߕs�v<S11E> */
#if ( defined(CSW_SGD7W_TYPE) || defined(CSW_COMMON_CARRIER) )		/* ���[�^�L�����A���g���؂�ւ��Ή�<S117> */
static	AXIS_IF				AxisIf;								/* ���j�b�g�����ԃC���^�t�F�[�X�\����		*/
#endif	/* #if ( defined(CSW_SGD7W_TYPE) || defined(CSW_COMMON_CARRIER) ) */
#endif		/*#if 0	 �L�����A���g���̓���͍s�Ȃ�Ȃ����ߕs�v<S11E> */

/* H/W�֘A */
static ASICS				jl076_adr[MAX_AXIS_NUMBER];			/* JL-076�A�N�Z�X�p�\����					*/

static MENCV				EncData[MAX_AXIS_NUMBER];			/* �G���R�[�_�ǂݏo�����					*/
static MENCV				FencData[MAX_AXIS_NUMBER];			/* �t���N�G���R�[�_�ǂݏo�����				*/
static SHAL_LATCH_STS_ADR	LtStsAdr[MAX_AXIS_NUMBER][3];		/* EXT���b�`�M���p							*/
//static OH_INFO			Ohinfo[OHNUM];						/* ���x�Ď��p�f�[�^							*//* <S03A> */
static PLSIFV 				LPlsIfV[MAX_AXIS_NUMBER];			/* <S00C> Command Pulse Interface			*/
static CADC					LAdc[MAX_AXIS_NUMBER];				/* <S00C> A/D Converter Interface Variable	*/
static RWALLMD 				EepPrmRwAllMng[MAX_AXIS_NUMBER];	/* <S00C> Parameter Download				*/
static RWPDLMD				EepPrmRwLcdMng[MAX_AXIS_NUMBER];	/* <S031> Lcd Parameter Copy				*/

/* �x�[�X���[�v���� */
static BASE_LOOPCTRLS		BaseLoops[MAX_AXIS_NUMBER];			/* ���䃋�[�v���Z�p�f�[�^					*/
static CTRL_LOOP_OUT		CtrlLoopOut[MAX_AXIS_NUMBER];		/* ScanA�o�̓f�[�^							*/

/* �x�[�X�T�[�{���� */
static BASE_CTRL_HNDL		BaseCtrlData[MAX_AXIS_NUMBER];		/* ��{����p���L�f�[�^						*/
static SERVO_CONTROL_IF		SvControlIf[MAX_AXIS_NUMBER];		/* �I�v�V�������W���[��(M3,M2,Ana..) I/F	*/
static BASE_CTRL_OUT		BaseCtrlOut[MAX_AXIS_NUMBER];		/* ScanB�o�̓f�[�^							*/
static DETVIB				DetVib[MAX_AXIS_NUMBER];			/* �U�����o�p�ϐ�							*/

static MOTSPDMAFIL			MotSpdMafil[MAX_AXIS_NUMBER];		/* ���[�^���x���o�p�ړ����ω��Z�p�ϐ�		*/

static PDET_ABSCONTROL		PdetAbsSeq[MAX_AXIS_NUMBER];		/* ABS�X�P�[�����Ɍ��o�p�ϐ�				*/
static MPFIND				MpFind[MAX_AXIS_NUMBER];			/* ���Ɍ��o�p�ϐ�							*/

/* �x�[�X�V�[�P���X���� */
static SEQ_IO_HNDL			SeqIoSignal[MAX_AXIS_NUMBER];		/* �V�[�P���XI/O */
static SEQ_CTRL_OUT			SeqCtrlOut[MAX_AXIS_NUMBER];		/* ScanC�o�̓f�[�^							*/
static BE_SEQ_HNDL			BeSeqData[MAX_AXIS_NUMBER];			/* Base Enable Sequence Handle				*/
static CHECK_ALARM			CheckAlarm[MAX_AXIS_NUMBER];		/* �A���[�����o�����p�\����					*/
static SEQ_MOT_STS			SeqMotSts[MAX_AXIS_NUMBER];			/* ScanC�ɂĎ擾�E�쐬���郂�[�^���*/
static REMVIBFREQ			RemVibFreq[MAX_AXIS_NUMBER];		/* �c���U�����g�����j�^�p�f�[�^				*/
static CALC_WATTAGE			CalcWattage[MAX_AXIS_NUMBER];		/* <S020> ����d�͉��Z�p�f�[�^(����)		*/
static ENVMON				EnvMonitor[MAX_AXIS_NUMBER];		/* �ݒu�����j�^�f�[�^(����)				*//* <S0ED> */
static POWER_MONITOR		PowerMonitor;						/* <S020> ����d�̓��j�^�p�ϐ�(���j�b�g����)*/
static POW_MNG_HNDL			hPowerManager;						/* �d���Ǘ��p�f�[�^(���j�b�g����)			*/
static ALMLATCH_HNDL		AlmLatchHndl[MAX_AXIS_NUMBER];		/* �A���[�����b�`�p�f�[�^					*//* <S154> */
static CHECK_OH				OverHeat;							/* �I�[�o�[�q�[�g�Ď��p�ϐ�(���j�b�g����)	*//* <S0ED> */
#if (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE)
static PNLOP_DATA_HANDLE	PnlOpLedDataHndl[MAX_AXIS_NUMBER];	/* �l�b�g���[�N�p�p�l�I�y�\����				*//* <S0A1> */
#endif

/* �A���[��/���W�X�^/Fn�Ǘ� */
static FUN_CMN_CONTROL		FunCmnControls[MAX_AXIS_NUMBER];	/* Fn�@�\���ʐ���f�[�^						*/
static ALMMAN_HANDLE		AlarmManager[MAX_AXIS_NUMBER];		/* �A���[������p�\����						*/
static TRCHNDL				TrcHndl;							/* �f�[�^�g���[�X�p�n���h��					*/
/*--------------------------------------------------------------------------------------------------*/
static OPACCSV 				AccessOP[MAX_AXIS_NUMBER];			/* Operation Register Access Right <S166>	*/

/* �A�i���O���j�^ *//* <S016> */
static AMONOUT				AmonOut[MAX_AXIS_NUMBER][2];		/* �A�i���O���j�^�o�͗p�\����				*/
static UCHAR				ScanB250us;							/* ScanB 250us �t���O						*/

/* ���[�^���X�e�X�g���[�h�֘A	*//* <S1B7> */
static MOTLESSTST			MotorLessTest[MAX_AXIS_NUMBER];		/* ���[�^���X�e�X�gIF��`					*/

/* �p�����[�^/���W�X�^�f�[�^�i�[�o�b�t�@ */
static UNI_PRM_STR			prm_hndl[MAX_AXIS_NUMBER];			/* �p�����[�^�p�n���h��						*/
static BPRMDAT				bprm_data[MAX_AXIS_NUMBER];			/* ���ʊ�{�p�����[�^						*/
static ANLG_UN_MON_DATA		AnlgUnMonData[MAX_AXIS_NUMBER];		/* Amon/Un���j�^�p�f�[�^					*/
static TEMP_MANAGER			TempManager;						/* ���x���o�p�f�[�^							*//* <S03A> */

/* Network */
#if (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE)/* AnalogIF�����L�\���̂𗘗p����悤�ɕύX����H	*/
static NETIF_AXIS_HANDLE	NetAxisIf[MAX_AXIS_NUMBER];			/* Network Interface Handle for each axis */
static NETIF_SYS_HANDLE		NetNodeIf;							/* Network Interface Handle for node device */
#endif
static LONG					NetIfMonDummy;

/* �I�v�V�������W���[��	*//* <S04D> */
static OPTCARD_HNDL			OptCardHdl[MAX_AXIS_NUMBER];		/* �I�v�V�������W���[���\���̂ւ̃|�C���^	*/

#if 0 /* 2012.08.06 Y.Oka Make Mercury Base Software */
static TRIGIO_INFO			TrigInfo[TRIG_IO_MAX_TBL_NUMBER];	/* TrigIO����p�f�[�^ 						*/
#endif /* 2012.08.06 Y.Oka Make Mercury Base Software */

#ifdef	CSW_L2_DCACHE_SECTION
#pragma ghs section bss=default
#endif



/****************************************************************************************************/
/*																									*/
/*	Variables for Non-Real-Time processing.															*/
/*																									*/
/****************************************************************************************************/

//static AXIS_HANDLE			AxGlobals[MAX_AXIS_NUMBER];			/* �����䉉�Z�p�ϐ�(�������A�h���X�p)		*/

/* H/W�֘A */
//static EEP_DEVICE_HANDLE	EepromDeviceHandle[MAX_AXIS_NUMBER];	/* EEPROM device handle	<S053>			*//* <S1D2> */
//<S053> static HW_AXIS_MAP			AsicMapInfo;						/* ���ԍ���JL077ASIC�ԓ��Ƃ̃}�b�v���		*/
static EEP_HEADER_INFO_T	UnitEepInfo[3];						/* IF/COA/MBU ID���						*/
static MODULE_ID_INFO		ModuleIDInfo[MAX_AXIS_NUMBER];		/* Module ID���							*/

/* Network */
//<S150>static NETMSG_IF			NetMsgServer;						/* Network Messages IF						*/

//#pragma arm section rwdata, zidata							/* TODO: Zantei */
static PRMDATA				usr_prm[MAX_AXIS_NUMBER];			/* ���[�U�p�����[�^							*/
#pragma arm section rwdata, zidata						/* TODO: Zantei */
/* �A���[��/���W�X�^/Fn�Ǘ� */
static REGIF_CONFIG_T		PnRegConfig[MAX_AXIS_NUMBER];		/* Paramter Register						*/
static REGIF_CONFIG_T		FnRegConfig[MAX_AXIS_NUMBER];		/* Operation Register						*/
static REGIF_CONFIG_T		TrcRegConfig;						/* Trace buffer Register					*/
static REGIF_CONFIG_T		UnRegConfig[MAX_AXIS_NUMBER];		/* Monitor Register							*/
static REGIF_CONFIG_T		InfRegConfig[MAX_AXIS_NUMBER];		/* Information Register						*/
#if (CSW_NETIF_TYPE == NETIF_M3_TYPE) ||(CSW_NETIF_TYPE == NETIF_M2_TYPE)
static REGIF_CONFIG_T		NetworkIdRegConfig[MAX_AXIS_NUMBER];/* Network ID Register						*/
#endif
static REG_MANAGER_HANDLE	RegManagerHandle[MAX_AXIS_NUMBER];	/* ���W�X�^�A�N�Z�XIF�n���h��				*/
static FUN_MANAGER			FunManager[MAX_AXIS_NUMBER];		/* Fn Manager�n���h��						*/
static FUN_AXCOMMON			FunManagerAxisCommons;				/* Fn Manager�p���j�b�g���ʃn���h��			*/

/* �p�����[�^/���W�X�^�f�[�^�i�[�o�b�t�@ */
//static PRMDATA				usr_prm[MAX_AXIS_NUMBER];			/* ���[�U�p�����[�^							*/
static CMN_PRMDATA			AxisCommonParams;					/* ���j�b�g���ʃp�����[�^					*/

#ifdef	__ghs__
#pragma ghs section bss=".fixed_buffer"
#endif
static USHORT				TraceBuffer[TRC_BUF_SIZE];		/* �f�[�^�g���[�X�o�b�t�@					*//* <S04C> */
static USHORT				TableRefBuffer[MAX_AXIS_NUMBER][AX_TBL_BUF_SIZE]; /* �e�[�u���^�]�w�߃o�b�t�@	*/
#ifdef	__ghs__
#pragma ghs section bss=default
#endif

/* Utility Functions */
static ANOTCHSEQ			ANotchSeq[MAX_AXIS_NUMBER];			/* �����m�b�`�V�[�P���X�p�\����				*/
static AUTONOTCH			ANotch[MAX_AXIS_NUMBER];			/* �����m�b�`�ݒ�p�\����					*/
static FFTANALYZE			FftAna[MAX_AXIS_NUMBER];			/* �U�����g�����(Online FFT)�p�\����		*/
static MFCTUNE				MfcTune[MAX_AXIS_NUMBER];			/* ���f���Q�C�������p�\����					*/
static VIBMON				VibMon[MAX_AXIS_NUMBER];			/* �I�����C���U�����j�^�p�\����				*/
static INIVIBDET			IniVibDet[MAX_AXIS_NUMBER];			/* �U�����o���x���������p�\����				*/
static ONEPRMTUNE			OnePrmTune[MAX_AXIS_NUMBER];		/* �����p�����[�^�`���[�j���O�p�\����		*/
static ADATHNDL				AdatHndl[MAX_AXIS_NUMBER];			/* Advanced Auto Tuning Hanlde				*/
static AVIBTUNE				AvibTune[MAX_AXIS_NUMBER];			/* A�^���U���䒲���p�\����					*/
static EASYFFT				EasyFFT[MAX_AXIS_NUMBER];			/* Easy FFT�p�\����							*/
static OPTCLR				OptClr[MAX_AXIS_NUMBER];			/* �I�v�V�����J�[�h���o�N���A�p�\����		*//* <S0BE> */


#if defined(_VC_DEBUG_)
UCHAR				DummySvip[0x8000];					/* JL-086H/W�_�~�[�o�b�t�@					*/
UCHAR				DummyEeprom[DEBUG_EEPROM_SIZE];		/* EEPROM�_�~�[�o�b�t�@						*/
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
/*		ASIC�|�C���^ & �����ݒ菈��																	*/
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
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W�i2���Ή��j�̂�	*//* <S0E4> */
		jl076_adr[ax].AsicMicroReg = (ASIC_U_IF_STR*)((UCHAR*)HW_SVASIC_ADDRESS + JL086_MICRO_OFS + JL086_MICRO_AXIS_OFS*ax);
#elif CSW_BOARD_TYPE == SGD7S_TYPE	/*	SGD7S�i�P�����j�̂�	*/
		jl076_adr[ax].AsicMicroReg = (ASIC_U_IF_STR*)((UCHAR*)AsicHwReg + JL086_MICRO_OFS);
#endif
//Mercury���g�p�H		jl076_adr[ax].AsicComHwReg = (ASIC_HW_IF_STR*)(HW_SVASIC_ADDRESS + HW_SVASIC_COMADR_OFS);
//Mercury���g�p�H		jl076_adr[ax].AsicComMicroReg = (ASIC_U_IF_STR* )(HW_SVASIC_ADDRESS
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
	/* ������͎d�l������̂��߁A�_�~�[�}�N���Ƃ��遚 */
	/* �������ł͋@��P�ʂł̎����ݒ�Ƃ��A���ۂ̐��䎲���̓p�����[�^�ݒ���������遚 */
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
/*		���W�X�^�A�N�Z�X�F�R���t�B�M�����[�V�����f�[�^�ݒ�											*/
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
	TrcRegConfig.AlmManager = &AlarmManager[0];				/* <S0AA> �b�� */

	for( ax = 0, pAxis = &SysAxisHandle[0]; ax < MAX_AXIS_NUMBER; ax++, pAxis++ )
	{
//<S053>		no = AxisMap->asicNo[ax];
		// Configure to Pn register interface
//		PnRegConfig[ax].hEepDev = &EepromDeviceHandle[ax];		/* <S053> *//* <S1D2> */
		PnRegConfig[ax].PrmListTable = PnPrmTbl;
		PnRegConfig[ax].NumOfTableEntry = PnPrmTblEntNum;
		PnRegConfig[ax].PrmCalParam = pAxis;
		PnRegConfig[ax].GetRamPtrParam = pAxis;
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W�i2���Ή��j�̂�						*//* <S0E4> */
		PnRegConfig[ax].EepromBlockAddress = (USHORT)(EEP_AXPRM_TOP_ADDRESS
													+ (EEP_MAP_AXIS_OFFSET * ax));
#elif CSW_BOARD_TYPE == SGD7S_TYPE	/*	SGD7S�i�P�����j�̂�	*/
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
/*		���j�^�I���Ǘ��e�[�u���쐬																	*/
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
	LIFETIME_HANDLE		*hLifeTime;		/* �����\���@�\		  <S1D7> */
	DBLIFETIME_HADNLE	*hDBLifeTime;	/* DB�����\���@�\	   <S1D7> */
	POWER_MONITOR		*PowerMonitor;	/* ����d�̓��j�^�ϐ�  <S1D7> */
	AXIS_HANDLE			*pAxis;

	NetIfMonDummy = 0;
	for( ax_no = 0, pAxis = &SysAxisHandle[0]; ax_no < MAX_AXIS_NUMBER; ax_no++, pAxis++ )
	{
		/* ���[�J���ϐ��̏����� */
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
		/*---- ���j�^�f�[�^0x0* ------------------------------------------------------------------------------	*/
		SvControlIf->MonRamTable[0x00] = (LONG *)&( pBaseCtrl->CtrlCmdMngr.SpdFbki );
		SvControlIf->MonRamTable[0x01] = (LONG *)&( pBaseCtrl->CtrlCmdMngr.SpdRefo );
		SvControlIf->MonRamTable[0x02] = (LONG *)&( pAxis->CtrlLoopOut->TrqRefMon );
		/* �ʃv���O����������炤�΍���32�r�b�g�̂��߁A����high���͖����Ƃ��� */
		SvControlIf->MonRamTable[0x03] = (LONG *)&( pBaseCtrl->PosManager.var.PosErrA.per32s );/* <S107> *//* <S14C> */
		SvControlIf->MonRamTable[0x0a] = (LONG *)&( pBaseCtrl->PosManager.MencPos.apos[0] );
		SvControlIf->MonRamTable[0x0b] = (LONG *)&( pBaseCtrl->PosManager.MencPos.apos[1] );
		SvControlIf->MonRamTable[0x0e] = (LONG *)&( pBaseCtrl->PosManager.PgPos.apos[0] );
		SvControlIf->MonRamTable[0x0f] = (LONG *)&( pBaseCtrl->PosManager.PgPos.apos[1] );

		/*---- ���j�^�f�[�^0x1* ------------------------------------------------------------------------------	*/
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

		/*---- ���j�^�f�[�^0x3* ------------------------------------------------------------------------------	*/
		SvControlIf->MonRamTable[0x30] = (LONG *)&( SvControlIf->MonCphaLpos[0] );
		SvControlIf->MonRamTable[0x31] = (LONG *)&( SvControlIf->MonCphaLpos[1] );
		SvControlIf->MonRamTable[0x32] = (LONG *)&( SvControlIf->MonExt1Lpos[0] );
		SvControlIf->MonRamTable[0x33] = (LONG *)&( SvControlIf->MonExt1Lpos[1] );
		SvControlIf->MonRamTable[0x34] = (LONG *)&( SvControlIf->MonExt2Lpos[0] );
		SvControlIf->MonRamTable[0x35] = (LONG *)&( SvControlIf->MonExt2Lpos[1] );
		SvControlIf->MonRamTable[0x36] = (LONG *)&( SvControlIf->MonExt3Lpos[0] );
		SvControlIf->MonRamTable[0x37] = (LONG *)&( SvControlIf->MonExt3Lpos[1] );
		SvControlIf->MonRamTable[0x38] = (LONG *)&( pBaseCtrl->PosManager.VirtualPositionError );// ���z�ʒu�΍� <S14C>
#if (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE)
//		SvControlIf->MonRamTable[0x39] = (LONG *)&( pBaseCtrl->PosManager.PosErrAmp.per32s );// ���z�ʒu�A���v�΍�?
		/*--------------------------------------------------------------------------------------------------*/
		/*		SGDV�ł́ASI0~SI7�ȊO�Ɉȉ���ݒ肷��B														*/
		/* 		PgAphase/PgBphase/PgCphase																	*/
		/* 			�FSGD7�ł́A�������ł���ANetIfMonInSignal�ɐݒ肷�邩RspSigBit�ɐݒ肷�邩�������K�v	*/
		/* 		BBon1/BBon2�FSGD7�ł́ARspSigBit�ɐݒ肳��Ă���B											*/
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


		/*---- ���j�^�f�[�^0x4* ------------------------------------------------------------------------------	*//* <S1D7> */
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
//<S053>	void		*SvAsicPtr;				/* JL-076/077 MemoryRegion�ւ̃|�C���^			*/
	AXIS_HANDLE	*ax_globals;

	NumberOfAxes = MAX_AXIS_NUMBER;				/* <S053> */
	InitializeSvASICPointer( );					/* <S053> */
#if 0											/* <S053> */
	/* ASIC�|�C���^�ݒ� */
	/* JL-076B Host IF Address�擾 */
#ifdef _VC_DEBUG_
	SvAsicPtr = (void*)GetSvipTopAddress( );
#else
//	SvAsicPtr = GetVirtualMemoryRegionAddress("mbboard");
	SvAsicPtr = (void*)HW_SVASIC_ADDRESS;
#endif
	NumberOfAxes = InitializeSvASICPointer( SvAsicPtr, &AsicMapInfo );
#endif

	/* AXIS�f�[�^�|�C���^������ */
	for( ax_no = 0; ax_no < MAX_AXIS_NUMBER; ax_no++ )
	{
		/* ���[�J���ϐ��̏����� */
		ax_globals = &SysAxisHandle[ax_no];

		/* ���ԍ� */
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W�i2���Ή��j�̂�	*//* <S0E4> */
		ax_globals->AxisNo = ax_no;		/*	�O�F�P���ځA�P�F�Q����	*/
		ax_globals->SlotNo = 0;			/*	�X���b�g�ԍ��͂O�Œ�	*/
#elif CSW_BOARD_TYPE == SGD7S_TYPE	/*	SGD7S�i�P�����j�̂�	*/
		ax_globals->AxisNo = ax_no;
		ax_globals->SlotNo = ax_no;
#endif

		/* �x�[�X�T�[�{����p�f�[�^(ScanA) */
		ax_globals->BaseLoops 					= &BaseLoops[ax_no];
		ax_globals->CtrlLoopOut					= &CtrlLoopOut[ax_no];

		/* �x�[�X�T�[�{����p�f�[�^(ScanB) */
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

		/* �T�[�{�V�[�P���X����p�f�[�^(ScanC) */
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
#if (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE)/* AnalogIF�����L�\���̂𗘗p����悤�ɕύX����H	*/
		ax_globals->PnlOpLedDataHndl			= &PnlOpLedDataHndl[ax_no];	/* <S0A1> */
#endif
		/* Network */
#if (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE)/* AnalogIF�����L�\���̂𗘗p����悤�ɕύX����H	*/
		ax_globals->NetAxisIf					= &NetAxisIf[ax_no];
		ax_globals->NetNodeIf					= &NetNodeIf;
#endif
#if (CSW_NETIF_TYPE == NETIF_SYNQNET_TYPE)
		SqnetHandle[ax_no].Sqnet_NodeCommons	= &Sqnet_NodeCommons;
		ax_globals->SqnetHandle					= &SqnetHandle[ax_no];
#endif

		/* �I�v�V�������W���[��	*//* <S04D> */
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

#if 0	/* �L�����A���g���̓���͍s�Ȃ�Ȃ����ߕs�v<S11E> */
#if ( defined(CSW_SGD7W_TYPE) || defined(CSW_COMMON_CARRIER) )		/* ���[�^�L�����A���g���؂�ւ��Ή�<S117> */
		ax_globals->AxisIf						= &AxisIf;
#endif	/* #if ( defined(CSW_SGD7W_TYPE) || defined(CSW_COMMON_CARRIER) ) */
#endif		/*#if 0	 �L�����A���g���̓���͍s�Ȃ�Ȃ����ߕs�v<S11E> */

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

		/* ���[�^���X�e�X�gIF��` *//* <S1B7> */
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

	/* ���j�^�I���Ǘ��e�[�u���쐬 */
	InitializeNetIfMonitorPointer();

	/* ���W�X�^�n���h�������� */
//<S053>	InitializeRegisterHandle( &AsicMapInfo );
	InitializeRegisterHandle( );		/* <S053> */

	/* �_������񏉊��� */
#if 0	/* RAM�m�� */
	for( ax_no = 0; ax_no < MAX_AXIS_NUMBER; ax_no++ )
	{
		SysAxisHandle[ax_no] = AxGlobals[ax_no];
	}
#endif

#if defined(_TEST_BOARD_) && !defined(_VC_DEBUG_)
	ScanAStartCtr = 0;						/* ScanA�����J�E���^							*/
	ScanBStartCtr = 0;						/* ScanB�����J�E���^							*/
	ScanCStartCtr = 0;						/* ScanC�����J�E���^							*/
#endif

	return (&SysAxisHandle[0]);
}



/***************************************** end of file **********************************************/
