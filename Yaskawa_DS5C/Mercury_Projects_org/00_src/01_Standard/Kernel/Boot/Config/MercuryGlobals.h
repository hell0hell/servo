#ifndef GROBAL_DATA_H
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		MercuryGlobals.h : �O���[�o���f�[�^��`														*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	����	2010.05.11	Y.Oka															*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#if 0 /* 2012.08.186 Y.Oka Make Mercury Base Software */
#include "BaseControls.h"
#include "SequenceControls.h"
#include "BaseEnableSequence.h"
#include "PrmAccessIf.h"
#include "FunManager.h"
#include "MpfHandler.h"
#include "PdetABSScale.h"
#include "AutoNotchSet.h"
#include "TuneLib.h"
#include "FnMfcTuning.h"
#include "FnVibMonitor.h"
#include "FnInitVibDetLvl.h"
#include "FnOnePrmTuning.h"
#include "FnAtypeVibTune.h"
#include "FnEasyFFT.h"
#include "DataTraceManager.h"
#include "AdvancedAutoTuning.h"
#include "NetworkIf.h"
#else /* 2012.08.18 Y.Oka Make Mercury Base Software */
#include "NetworkIf.h"
#include "HwDeviceIf.h"
#include "BaseControls.h"
#include "SequenceIo.h"
#include "BaseEnableSequence.h"
#include "FunManager.h"
#include "FnMfcTuning.h"
#include "FnVibMonitor.h"
#include "FnInitVibDetLvl.h"
#include "FnOnePrmTuning.h"
#include "FnAtypeVibTune.h"
#include "FnEasyFFT.h"
#include "FnSysConfig.h" /* <S0BE> */
#include "AutoNotchSet.h"
#include "AdvancedAutoTuning.h"
#include "PdetABSScale.h"
#include "JL086ServoIpHw.h"
#include "EepromIF.h"
#include "PrmAccessIf.h"
#include "UnitPrmStruct.h"
#include "AlarmManager.h"
#include "OverheatManager.h"
#include "Pls.h"									/* <S00C> Command Pulse Counter Driver			*/
#include "Adc.h"									/* <S00C> A/D Converter Driver					*/
#include "I2cDriver.h"									/* <S00C> EEPROM Driver							*/
#include "XlsAmonDef.h"		/* <S016>���[�u */
#include "SvidDef.h"		/* <S031> ��ŏ��� */
#include "DetectTemperature.h"	/* <S03A> */
#include "SvOptCardIf.h"							/* <S04D> Servo Option Card If					*/
#include "PnlOp.h"
#include "LifeTimePredictionApi.h"	/* <S154> */
#include "AlmLatch.h"				/* <S154> */
#include "MotorLessTestStr.h"						/* <S1B7> Motor Less Test Mode					*/
#endif /* 2012.08.18 Y.Oka Make Mercury Base Software */


/****************************************************************************************************/
/*		STRUCT DEFINITION(���j�b�g�P��)																*/
/****************************************************************************************************/
/* �������Ԍv���p */
typedef struct _TASK_MEASURE_TIME {
	ULONG	SA_maxTime;		/* ScanA�ő又������ */
	ULONG	SA_crtTime;		/* ScanA���ݏ������� */
	ULONG	SA_sumTime;		/* ScanA2ms�ݐϏ�������	<S043> */
	ULONG	SAinB_exeTime;		/* ScanB���s����ScanA���s�J�E���g	<S043> */
	ULONG	SAinC_exeTime;		/* ScanC���s����ScanA���s�J�E���g	<S043> */
	ULONG	SB_maxTime;		/* ScanB�ő又������ */
	ULONG	SB_crtTime;		/* ScanB���ݏ������� */
	ULONG	SB_sumTime;		/* ScanB2ms�ݐϏ�������	<S043> */
	ULONG	SB_exeflg;		/* ScanB���s���t���O	<S043> */
	ULONG	SBinC_exeTime;		/* ScanC���s����ScanB���s�J�E���g	<S043> */
	ULONG	SC_maxTime;		/* ScanC�ő又������ */
	ULONG	SC_crtTime;		/* ScanC���ݏ������� */
	ULONG	SC_exeflg;		/* ScanC���s���t���O	<S043> */
	ULONG	crtTimePer2ms;	/* 2ms�������ScanA,ScanB,ScanC��L�������� */
	ULONG	maxTimePer2ms;	/* 2ms�������ScanA,ScanB,ScanC��L�ő又������ */
	ULONG	ScanSumTime;	/* work */

	ULONG	SCtask_counter;
	ULONG	SCtask_wakeup_time;
	ULONG	SBcountInMS;
	ULONG	SBCount;
	ULONG	SBCountInc;
	ULONG	SACount;		/* ScanA���s��	<S043> */
} TASK_MEASURE_TIME;

#define	SCANA_EXE_CNT_IN_2MS	(2000000/KPI_SACYCLENS)		/* 2ms����ScanA���s��	<S043> */
#define	SCANB_EXE_CNT_IN_2MS	(2000000/KPI_SBCYCLENS)		/* 2ms����ScanB���s��	<S043> */
#define	SCANC_EXE_CNT_IN_2MS	(2/KPI_SCCYCLEMS)			/* 2ms����ScanC���s��	<S043> */

/* Unit ID Information */
typedef struct MODULE_ID_INFO {
	EEP_HEADER_INFO_T	*IfInfo;		/* IF ID��� */
	EEP_HEADER_INFO_T	*CoaInfo;		/* COA ID��� */
	EEP_HEADER_INFO_T	*MbuInfo;		/* MBU ID��� */
	EEP_HEADER_INFO_T	SdaInfo;		/* SDA ID��� */
//<S053>	HW_AXIS_MAP			*AsicNoMap;		/* ���ԍ���JL077ASIC�ԓ��Ƃ̃}�b�v���  */
} MODULE_ID_INFO;


/****************************************************************************************************/
/*		STRUCT DEFINITION(���j�b�g�����ԃC���^�t�F�[�X)		���[�^�L�����A���g���؂�ւ��Ή�<S117>								*/
/****************************************************************************************************/
/*  */
#if 0	/* �L�����A���g���̓���͍s�Ȃ�Ȃ����ߕs�v<S11E> */
#if ( defined(CSW_SGD7W_TYPE) || defined(CSW_COMMON_CARRIER) )
typedef struct AXIS_IF {
	SHORT	pwm_ax[MAX_AXIS_NUMBER];		/* �e���̐ݒ�PWM�L�����A���g��							*/
	SHORT	pwm_unit;						/* �S�̂̐ݒ�PWM�L�����A���g��							*/
}AXIS_IF;
#endif	/* #if ( defined(CSW_SGD7W_TYPE) || defined(CSW_COMMON_CARRIER) ) */
#endif		/*#if 0	 �L�����A���g���̓���͍s�Ȃ�Ȃ����ߕs�v<S11E> */



/****************************************************************************************************/
/*		STRUCT DEFINITION(���P��)																	*/
/****************************************************************************************************/
typedef	struct AXIS_HANDLE {
	LONG				SlotNo;				/* ���ԍ�(=MB��X���b�g�ԍ�)							*/
	LONG				AxisNo;				/* ���A�h���X(�_�����ԍ�)								*/

	/* Hardware abstraction layer */
	MODULE_ID_INFO		*ModuleID;			/* Module ID information */
	MENCV				*MencV;				/* ���[�^�G���R�[�_�f�[�^�ւ̃|�C���^					*/
	MENCV				*FencV;				/* �t���N�G���R�[�_�f�[�^								*/
	ASICS				*SvAsicRegs;		/* ASIC�A�h���X�e�[�u���ւ̃|�C���^						*/
	SHAL_LATCH_STS_ADR	*LtStsAdr;			/* EXT���b�`�M���p 										*/
//	OH_INFO				*OhInfo;			/* ����x���o�p 										*//* <S03A> */
	PLSIFV 				*pPlsIfV;			/* <S00C> Command Pulse Interface						*/
	CADC				*pAdc;				/* <S00C> A/D Converter Interface						*/
	RWALLMD 			*pRwAllMng;			/* <S00C> Parameter Download							*/
	RWPDLMD 			*pRwPdlMng;			/* <S031> Lcd Parameter Copy							*/

	/* �x�[�X�T�[�{����p�f�[�^(ScanA) */
	BASE_LOOPCTRLS		*BaseLoops;			/* ���䃋�[�v���Z�p�f�[�^								*/
	CTRL_LOOP_OUT		*CtrlLoopOut;		/* ScanA�o�̓f�[�^										*/

	/* �x�[�X�T�[�{����p�f�[�^(ScanB) */
	BASE_CTRL_HNDL		*BaseControls;		/* ��{����p���L�f�[�^									*/
	SERVO_CONTROL_IF	*SvControlIf;		/* M3 I/F�f�[�^											*/
	BASE_CTRL_OUT		*BaseCtrlOut;		/* ScanB�o�̓f�[�^										*/
	DETVIB				*DetVib;			/* �U�����o�p�ϐ�										*/
	USHORT				*TableRefBuffer;	/* �e�[�u���^�]�w�߃o�b�t�@�ւ̃|�C���^					*/
	
	MOTSPDMAFIL			*MotSpdMafil;		/* ���[�^���x���o�p�ړ����ω��Z�p�ϐ�					*/

	PDET_ABSCONTROL		*PdetAbsSeq;		/* ABS�X�P�[�����Ɍ��o�p�ϐ�							*/
	MPFIND				*MpFind;			/* ���Ɍ��o����p�ϐ�									*/
	
	/* �T�[�{�V�[�P���X����p�f�[�^(ScanC) */
	SEQ_IO_HNDL			*IoSignals;			/* �V�[�P���XI/O */
	SEQ_CTRL_OUT		*SeqCtrlOut;		/* ScanC�o�̓f�[�^										*/
	BE_SEQ_HNDL			*BeSequence;		/* Base Enable Sequence Handle							*/
	CHECK_ALARM			*CheckAlarm;		/* �A���[�����o�����p�\����								*/
	SEQ_MOT_STS			*SeqMotSts;			/* ScanC�ɂĎ擾�E�쐬���郂�[�^���					*/
	REMVIBFREQ			*RemVibFreq;		/* �c���U�����g�����j�^�p�f�[�^							*/
	CALC_WATTAGE		*CalcWattage;		/* <S020> ����d�͉��Z�p�f�[�^(����)					*/
	ENVMON				*EnvMonitor;		/* �ݒu�����j�^�f�[�^(����)							*//* <S0ED> */
	POWER_MONITOR		*PowerMonitor;		/* <S020> ����d�̓��j�^�ϐ�(���j�b�g����)				*/
	POW_MNG_HNDL		*PowerManager;		/* �d���Ǘ��p�f�[�^(���j�b�g����)						*/
	CHECK_OH			*OverHeat;			/* �I�[�o�[�q�[�g�`�F�b�N�p�ϐ�(���j�b�g����)			*//* <S0ED> */
	LIFETIME_HANDLE		*hLifeTime;			/* �����\���@�\											*//* <S154> */
	DBLIFETIME_HADNLE	*hDBLifeTime;		/* DB�����\���@�\										*//* <S180> */
	ALMLATCH_HNDL		*hAlmLatchHndl;		/* �A���[�����b�`�p�f�[�^								*//* <S154> */
#if (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE)
	PNLOP_DATA_HANDLE	*PnlOpLedDataHndl;	/* �l�b�g���[�N�p�p�l�I�y�\����							*//* <S0A1> */
#endif

	/* Network */
#if (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE)/* AnalogIF�����L�\���̂𗘗p����悤�ɕύX����H			*/
	NETIF_AXIS_HANDLE	*NetAxisIf;			/* Network Interface Handle for each axis 				*/
	NETIF_SYS_HANDLE	*NetNodeIf;			/* Network Interface Handle for node device 			*/
#endif
	NETMSG_IF			*NetMsgIf;			/* Network Messages IF									*/

	/* �I�v�V�������W���[��	*//* <S04D> */
	OPTCARD_HNDL		*OptCardHdl;		/* �I�v�V�������W���[���\���̂ւ̃|�C���^				*/
	
	/* �p�����[�^�f�[�^ */
	REG_MANAGER_HANDLE	*RegManager;		/* Register Manager */
	UNI_PRM_STR			*UniPrms;			/* �p�����[�^�n���h���ւ̃|�C���^						*/
	ALMMAN_HANDLE		*AlmManager;		/* �A���[������p�\����									*/

	/* Fn�@�\�f�[�^ */
	FUN_CMN_CONTROL		*FnCmnCtrl;			/* Fn�@�\�����p�f�[�^�ւ̃|�C���^						*/
	FUN_MANAGER			*FnManager;			/* Fn Manager�n���h��									*/
	FUN_AXCOMMON		*FnAxCommons;		/* Fn Manager�p���j�b�g���ʃn���h��						*/

	/* Utility Functions */
	ANOTCHSEQ			*ANotchSeq;			/* �����m�b�`�V�[�P���X�p�\����							*/
	AUTONOTCH			*ANotch;			/* �����m�b�`�ݒ�p�\����								*/
	FFTANALYZE			*FftAna;			/* �U�����g�����(Online FFT)�p�\����					*/
	MFCTUNE				*MfcTune;			/* ���f���Q�C�������p�\����								*/
	VIBMON				*VibMon;			/* �I�����C���U�����j�^�p�\����							*/
	INIVIBDET			*IniVibDet;			/* �U�����o���x���������p�\����							*/
	ADATHNDL			*AdatHndl;			/* Advanced Auto Tuning	Handle							*/
	ONEPRMTUNE			*OnePrmTune;		/* �����p�����[�^�`���[�j���O�p�\����					*/
	AVIBTUNE			*AvibTune;			/* A�^���U���䒲���p�\����								*/
	EASYFFT				*EasyFFT;			/* Easy FFT�p�\����										*/
	OPTCLR				*OptClr;			/* �I�v�V�����J�[�h���o�N���A�p�\����					*//* <S0BE> */

	TEMP_MANAGER		*TempManager;		/* ���x���o�f�[�^�ւ̃|�C���^							*//* <S03A> */
	ANLG_UN_MON_DATA	*UnMonitor;			/* Amon/Un���j�^�f�[�^�ւ̃|�C���^						*/
	TASK_MEASURE_TIME	*TaskProcessTime;	/* �������ԃ��j�^�p */

#if 0	/* �L�����A���g���̓���͍s�Ȃ�Ȃ����ߕs�v<S11E> */
#if ( defined(CSW_SGD7W_TYPE) || defined(CSW_COMMON_CARRIER) )			/* ���[�^�L�����A���g���؂�ւ��Ή�<S117> */
	AXIS_IF				*AxisIf;			/* ���j�b�g�����ԃC���^�t�F�[�X�\����			*/
#endif	/* #if ( defined(CSW_SGD7W_TYPE) || defined(CSW_COMMON_CARRIER) ) */
#endif		/*#if 0	 �L�����A���g���̓���͍s�Ȃ�Ȃ����ߕs�v<S11E> */

	/* �f�[�^�g���[�X�֘A *///kira_101027
	TRCHNDL				*TrcHndl;			/* �f�[�^�g���[�X�n���h�� */
	USHORT				*TraceBuffer;		/* �f�[�^�g���[�X�o�b�t�@�ւ̃|�C���^ */

	/* �A�i���O���j�^�֘A *//* <S016> ���[�u */
	AMONOUT				*AmonOut;			/* �A�i���O���j�^�o�͏��ւ̃|�C���^	*/
	UCHAR				*ScanB250us;		/* ScanB 250us �t���O				*/

	/* ���[�^���X�e�X�g���[�h�֘A	*//* <S1B7> */
	MOTLESSTST			*MotorLessTest;		/* ���[�^���X�e�X�gIF��`�ւ̃|�C���^					*/

#if 0 /* 2012.08.06 Y.Oka Make Mercury Base Software */
	/* �A�v���P�[�V���� */
	TRIGIO_INFO			*TrigInfo;			/* TrigIO����p�f�[�^ 									*/
#endif /* 2012.08.06 Y.Oka Make Mercury Base Software */

#if defined(_VC_DEBUG_) /* 2012.08.23 Y.Oka Make Mercury Base Software */
	UCHAR				*pDummyEeprom;
#endif /* #if defined(_VC_DEBUG_) */ /* 2012.08.23 Y.Oka Make Mercury Base Software */
	
	OPACCSV 			*pAccessOp;			/* <S166> Operator Access Control */

} AXIS_HANDLE;


void* GetAxisHandle(UINT axis_number);
AXIS_HANDLE *InitializeAxisGlobals( void );


extern	LONG	NumberOfAxes;				/* �����ݒ�						*/
#if defined(_TEST_BOARD_) && !defined(_VC_DEBUG_)
extern	LONG	ScanAStartCtr;						/* ScanA�����J�E���^							*/
extern	LONG	ScanBStartCtr;						/* ScanB�����J�E���^							*/
extern	LONG	ScanCStartCtr;						/* ScanC�����J�E���^							*/
#endif
extern DWORDX	ReadDummy;					/* <S074> Dummy Variable for Read */

#define GROBAL_DATA_H
#endif /* GROBAL_DATA_H */

//#define _INFO_REG_CHECK_	/* �@���񃌃W�X�^�p�R���p�C���X�C�b�` */	/* <S061> */


/***************************************** end of file **********************************************/
