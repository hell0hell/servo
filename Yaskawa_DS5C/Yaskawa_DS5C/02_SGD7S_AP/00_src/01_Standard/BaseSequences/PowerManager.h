#ifndef	_POWER_MANAGER_H_ /* �ēǂݍ��ݖh�~�p */
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		PowerManager.h : �T�[�{�d���Ǘ�������`														*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	����	2010.04.19	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "Bprm.h"
#include "BaseCmnStruct.h"
#include "JL086ServoIpReg.h"


/****************************************************************************************************/
/*		STRUCT DEFINITION																			*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		���H�Ď��p�\���̒�`																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct CHECK_MAIN_POWER {
	struct	{							/* ���H�d���Ď��p�p�����[�^��`							*/
		ULONG	AcoffDetctTime;				/* AC Power Off Detect Time								*/
		BOOL	Dcbus;						/* DC�d�����͑I��(Pn001.2)								*/
		BOOL	Ac3Phase;					/* �d������3��(PnE0C.1,2)								*/
		BOOL	DetectAcOn2;				/* �d���������o����(PnE0C.3)							*/
		LONG	RlyDbWay;					/* �˓�/DB������@�I��ݒ�(PnE0C.4,5)					*/
		ULONG	UvFilter;					/* UV���o�t�B���^���萔(PnE74)							*/
		LONG	OvdetAlmFilter;				/* �ߓd���A���[���t�B���^(PnE7A.2,3)	(1=250us)		*/
		ULONG	PowerChargeWaitTime;		/* DC�[�d�����҂�����	<S053>							*/
		ULONG	PowerReadyWaitTime;			/* �p���[���f�B�҂�����	<S053>							*/
		ULONG	DischCheckVlt;				/* �}�����d�m�F臒l�d��									*//* <S1C6> *//* <S1D1> */
		ULONG	DischCompVlt;				/* �}�����d�p��臒l�d��									*//* <S1C6> *//* <S1D1> */
		ULONG	MainDischActTime;			/* ���C���}�����d����(�[�d���ԂƓ���)					*//* <S1C6> */
		USHORT	UvLevel;					/* �s���d��(UV)���o���x��								*/
		USHORT	UvWrnLevel;					/* �s���d��(UV)���[�j���O���o���x��						*/
		USHORT	OvLevel;					/* �񐶎g�p���̉ߓd��(OV)���o���x��						*/
		USHORT	NonRegOvLevel;				/* �񐶕s�g�p���̉ߓd��(OV)���o���x��					*//* <S0F7> */
		USHORT	TestDcLevel;				/* ���������@���[�h�F���H�d�����o�␳��������d��	*/
		BOOL	TestMode1;					/* ���������@���[�h�F�d���Ǘ�							*/
		BOOL	TestMode2;					/* ���������@���[�h�F�񐶈ُ팟�o�Ȃ�					*/
		BOOL	TestMode3;					/* ���������@���[�h�F�c�a��~�������s�����n�m���Ȃ�(INGRAM�ł͕s�g�p)	*/
		BOOL	TestMode4;					/* ���������@���[�h�FASIC WDT��kick���Ȃ�				*/
		BOOL	TestMode5;					/* ���������@���[�h�F���H�Ւf���A�˖h�����[������ON/OFF	*/
		BOOL	TestMode6;					/* ���������@���[�h�F�񐶐��������ON/OFF				*//* <S0CB> */
		BOOL	TestMode7;					/* ���������@���[�h�FDB���������ON/OFF					*//* <S0CB> */
		BOOL	TestMode8;					/* ���������@���[�h�F�Q�[�g�h���C�u�M��������ON/OFF		*//* <S0CB> */
		BOOL	TestMode9;					/* ���������@���[�h�F�Q�[�g�h���C�u�M����ScanA�ɂ�ON/OFF*//* <S19F> */
		BOOL	GateDriveSigTestFlag;		/* ���������@���[�h�F�Q�[�g�h���C�u�M����ԃt���O(ScanA)*//* <S19F> */
		BOOL	PreWarningSw;				/* �\�h�ۑS���[�j���O�I��								*//* <S154> */
		BOOL	DcDischarge;				/* DC�d�����͎��̋}�����d�I��							*//* <S21D> */
		BOOL	CapaLinkFlag;				/* �R���f���T�o���N�ڑ��t���O								*//* <S21D> */
	} conf;
/*--------------------------------------------------------------------------------------------------*/
	struct	{							/* ���H�d���Ď��p�p�����[�^��`							*/
		ULONG	AconWaitTimer;				/* Initial Acon Wait Timer								*/
		ULONG	AcoffChkTimer;				/* AC Power Off Check Timer								*/
		ULONG	DconChkTimer;				/* DC Power On  Check Timer								*/
		ULONG	DischChkTimer;				/* Discharge Check Timer								*/
		ULONG	DischSeqNo;					/* Discharge Sequence Number							*//* <S1C6> */
		ULONG	DischActTimer;				/* Discharge Active Time Counter						*//* <S1C6> */
		ULONG	DischStartVolt;				/* Discharge Start Timing Voltage						*//* <S1C6> */
		ULONG	OpenChkTimer[2];			/* Open Phase Check Timer								*/
		ULONG	UvChkTimer;					/* Under Voltage Check Timer							*/
		ULONG	WireAlmDetWait;				/* Remain DC Power Check Timer							*/
		BOOL	DetOpenAcOn1;				/* ACON1 signal			(Used by Detect Open-Phase)		*/
		BOOL	DetOpenAcOn2;				/* ACON2 signal			(Used by Detect Open-Phase)		*/
		BOOL	Alm330Lt;					/* A.330 in DC mode Detect Flag							*/
		LONG	AcOn1OffCount;				/* ACON1 Off Count Timer(Used by Detect Open-Phase)		*/
		LONG	AcOn2OffCount;				/* ACON2 Off Conut Timer(Used by Detect Open-Phase)		*/
		LONG	AconDetectCount;			/* Acon Active Detect Count in DC mode					*/
		BOOL	PreSigOut;					/* �\�h�ۑS�o�͐M��										*//* <S154> */
		/* <S21D> Start */
		BOOL	ClinkOut;					/* CLINK Output Signal									*/
		BOOL	CmconOut;					/* CMCON Output Signal									*/
		ULONG	ConvInSigDischErrChkTimer;	/* Converter I/O Signal Discharge Error Check Timer		*/
		LONG	DcVoltOld;					/* DC Volt Before Discharge								*/
		ULONG	ClinkAnsErrChkTimer;		/* Capacitor Bank Link Answer Back Error Check Timer	*/
		ULONG	CmcAnsErrChkTimer;			/* Capacitor Bank MC Answer Back Error Check Timer		*/
		ULONG	ClinkAnsTimer;				/* Clink Answer Check Timer								*/
		BOOL	WrnExRegInhibit;			/* WRN_EXREG Inhibit Flag								*/
		BOOL	CapaChargeOk;				/* Capacitor Bank Charge OK Flag						*/
		/* <S21D> End */
	} var;
/*--------------------------------------------------------------------------------------------------*/
} CHECK_MAIN_POWER;


/*--------------------------------------------------------------------------------------------------*/
/*		DC�d���d�����o�p�ϐ���`																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct DET_DC_VOLT {
	struct	{							/* DC�d���d�����o�p�萔��`									*/
		UCHAR	VdetMode;					/* ���H����d�����o����(PnE0C.8,9)					*/
		USHORT	VdetMaxVolt;				/* Amp Volt detect at max input		[data/MaxV]			*/
		LONG	VdetAdNorm;					/* ���H���oAD�ϊ����ʐ��K���萔(PnE70)				*/
		LONG	VdetAdFil;					/* ���H���oAD�t�B���^�Q�C��							*/
		LONG	VdcdetAlmFilter;			/* ���H���o�ُ�A���[���t�B���^(PnE7A.0,1) (1=250us)	*/
		BOOL	VdetPrmOk;					/* ���H���o�֘A�p�����[�^�ݒ�(1:OK, 0:NG)				*/
		LONG	VdetAdadj;					/* PnE5B : ����:P-N�d�����o���x������					*/
		LONG	Kdcvolt;					/* ���H���o�l���Z�Q�C���@[Admax] �� [V]				*/
	} conf;
	LONG	DcAdFili;						/* DC Volt Ad read filter output						*/
	LONG	DcAdFilo[3];					/* DC Volt Ad read filter output						*/
	LONG	DcVoltx;						/* DC Volt												*/
	ULONG	DcvErrCnt;						/* DC Volt Error Check Counter							*/
	ULONG	DcvWaitCnt;						/* DC Volt Error Check Wait Counter						*/
	LONG	VdcdetAlmCnt;					/* ���H���o�A���[�����o�t�B���^	(1=250us)			*/
	LONG	OvdetAlmCnt;					/* �ߓd���A���[�����o�t�B���^		(1=250us)			*/
} DET_DC_VOLT;


/*--------------------------------------------------------------------------------------------------*/
/*		�d���X�e�[�^�X�\���̒�`																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct POWER_STATUS {
	BOOL			Acon;					/* AC�d�����o���										*/
	BOOL			Acon1Sig;				/* �R���o�[�^�o�͐M�����1								*/
	BOOL			Acon1Sig_l;				/* �R���o�[�^�o�͐M�����1(�O��l)						*/
	BOOL			Acon2Sig;				/* �R���o�[�^�o�͐M�����2(���󖢎g�p)					*/
	BOOL			ChargeOk;				/* DC�[�d�������										*/
	BOOL			Discharge;				/* DC�}�����d�v��										*/
	BOOL			PowerOn;				/* ��d��ON���(�p���[���f�B�҂����ԍl��)				*/
	BOOL			PowerOnEep;				/* ��d��ON���(EEPROM�Ǘ��p)							*/
	LONG			DcVolt;					/* P-N�d�����o�l										*/
	BOOL			NegPumpReq;		        /* Negative Pump On Control Request to Kernel<S201>*/
	BOOL			AconDetectReady;	    /* AC ON Signal Detect Ready Flag(FALSE:Not Ready, TRUE:Ready)<S201>*/
	/* <S21D> Start */
	BOOL			ClinkAnsSig;			/* �R���f���T�o���N�ڑ��Ď��M��							*/
	BOOL			CmcAnsSig;				/* �˖h�����[��ԊĎ��M��								*/
	BOOL			ConvRdySig;				/* �R���o�[�^���f�B�M��									*/
	BOOL			ConvMconSig;			/* �˓��d���h�~�p�����[�M��								*/
	/* <S21D> End */
} POWER_STATUS;

/*--------------------------------------------------------------------------------------------------*/
/*		���ԓd�ʈُ팟�o�\���̒�` <S053>															*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct PCOV_ERR_DETECT {
	struct {							/* ���ԓd�ʈُ팟�o�p�p�����[�^��`							*/
		BOOL		PcovEna;				/* ���ԓd�ʈُ팟�o�L��									*/
		ULONG		PcovErrCntLmt;			/* ���ԓd�ʈُ팟�o���萔(�J�E���g�l)					*/
	} conf;
	struct {							/* ���ԓd�ʈُ팟�o�p�ϐ���`								*/
		BOOL		Pcov;					/* ���ԓd�ʈُ�M��(���_��)								*/
		ULONG		PcovErrCnt;				/* ���ԓd�ʈُ�J�E���g									*/
	} var;
} PCOV_ERR_DETECT;


/*--------------------------------------------------------------------------------------------------*/
/*		�˓��h�~��R�ߕ��׃`�F�b�N�p�\���̒�`														*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct CHECK_RUSH {
	LONG	PowerOnChk;						/* �˓���R�ߕ��� : Last PowerOn						*/
	LONG	PowerOnCnt[2];					/* �˓���R�ߕ��� : Off-->On Counter					*/
	ULONG	PowerOnChkCycle;				/* �˓���R�ߕ��� : Check Cycle Counter					*/
} CHECK_RUSH;


/*--------------------------------------------------------------------------------------------------*/
/*		�񐶉ߕ��׃`�F�b�N�p�\���̒�`																*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct CHECK_REGOL {
	struct	{								/* �񐶉ߕ��׃`�F�b�N�p�萔��`							*/
		LONG	Krgpower;						/* �񐶉ߕ��� : �d�͉��Z�Q�C��						*/
		LONG	RgolPowBase;					/* �񐶉ߕ��� : �d�̓x�[�X							*/
		LONG	RgolAlmLevel;					/* �񐶉ߕ��� : �A���[�����x��						*/
		LONG	Kunrgpower;						/* �񐶏���d�͉��Z�Q�C��(for Un00A)				*/
		LONG	KunrgpowerCycle;				/* �񐶏���d�͉��Z�Q�C��(for Un143)				*/
		LONG	RgSumSel;						/* �񐶉ߕ��׌��o�p�����l�����I��(PnE02.6,7)		*/
	} conf;
/*--------------------------------------------------------------------------------------------------*/
	struct	{								/* �񐶉ߕ��׃`�F�b�N�p�ϐ���`							*/
		LONG	RgolPowSum;						/* �񐶉ߕ��� : �d�͐ώZ�l							*/
		BOOL	LastRgolDetect;					/* �񐶉ߕ��� : �O��N�����̉񐶉ߕ��׌��o�t���O	*/
		BOOL	RgPowUp;						/* �񐶉ߕ��� : �񐶓d�͑����t���O					*/
		LONG	RgPowerSum;						/* �񐶕��ϒl : �d�͐ώZ�l							*/
		LONG	RgMeanCycle;					/* �񐶕��ϒl : �d�͐ώZ��						*/
	} var;
} CHECK_REGOL;


/*--------------------------------------------------------------------------------------------------*/
/*		�񐶈ُ�`�F�b�N�p�\���̒�`																*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct CHECK_REGENE {
	LONG	RegeneErrCnt;					/* �񐶈ُ�`�F�b�N�J�E���g								*/
	BOOL	RegeAlmSignal;					/* �񐶈ُ�A���[���t���O								*/
} CHECK_REGENE;


/*--------------------------------------------------------------------------------------------------*/
/*		�񐶐���p�\���̒�`																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct REGENE_CTRL {
	struct	{								/* �񐶐���p�萔��`									*/
		ULONG			ResistorCapa;			/* �񐶒�R�e��(Pn600)								*/
		BOOL			RegRxIn;				/* �񐶒�R����(PnE0C.0)							*/
		LONG			RegStartFilter;			/* �񐶓���J�n�t�B���^(PnE7A.4,5)	(1=250us)		*/
		LONG			RegStopFilter;			/* �񐶓���J�n�t�B���^(PnE7A.6,7)	(1=250us)		*/
		USHORT			RegOnLevel;				/* ��Tr ON���x��									*/
		USHORT			RegOffLevel;			/* ��Tr OFF���x��									*/
		USHORT			RegSyn;					/* �񐶓����@�\�̑I��								*//* <S21D> */
		USHORT			RegSynWaitTime;			/* �񐶓������̉�ON�҂�����(PnE9B)				*//* <S21D> */
	} conf;
/*--------------------------------------------------------------------------------------------------*/
	LONG			RegStartCnt;			/* �񐶓���J�n�҂��J�E���^								*/
	LONG			RegStopCnt;				/* �񐶓���I���҂��J�E���^								*/
	LONG			RegOnCount[2];			/* �񐶃g�����W�X�^ON���� 0:ScanB�X�V, 1:ScanC�X�V		*/
	LONG			dRegOnCount;			/* �����񐶃g�����W�X�^ON����			[Count/ScanC]	*/
	BOOL			RegOn;					/* �񐶃g�����W�X�^ON�v��								*/
	BOOL			RegAlmSig;				/* �񐶈ُ팟�o�M��										*/
	BOOL			RegAlmMotStop;			/* �񐶉ߕ��׃A���[�������ヂ�[�^��~���o�t���O			*/
	CHECK_REGOL		RegOL;					/* �񐶉ߕ��׃`�F�b�N�p�ϐ��ւ̃|�C���^(���j�b�g����)	*/
	CHECK_REGENE	RegChk;					/* �񐶈ُ�`�F�b�N�p�ϐ�(���j�b�g����)					*/
	LONG			UnMeanRgPower;			/* �񐶏���d�͕��ϒl	(���j�b�g���ʁF���j�^�p)	[%]			*//* <S123> */
} REGENE_CTRL;


/*--------------------------------------------------------------------------------------------------*/
/*		�񐶐���ݒ�p�p�����[�^�\���̒�`															*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct REGCFG_PRM {
	USHORT			regres;					/* Pn600 : �񐶒�R�e��									*/
	USHORT			exreger;				/* Pn603 : �񐶒�R�l(�f�t�H���g)						*/
	USHORT			reger;					/* PnE1A : �񐶒�R�l									*/
	USHORT			wre;					/* PnE1C : �񐶒�R�e��(�f�t�H���g)						*/
	USHORT			regonlvl;				/* PnE72 : �񐶃I�����x��								*/
	USHORT			ConvW;					/* PnE7F : �R���o�[�^�e��(�V�X�e��)						*/
} REGCFG_PRM;


/*--------------------------------------------------------------------------------------------------*/
/*		�T�[�{�p�b�N�����t�@����~���o�p�\���̒�`	<S0A3>											*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct CHECK_FANLOCK {
	struct	{							/* �����t�@����~���o�p�萔��`								*/
		USHORT	FanLockDetectTime;		/* �t�@����~���o����(�A��)									*/
		ULONG	FanLockAlmDetectTime;	/* �T�[�{�I���������t�@����~�A���[�����o����(�ݒ�l)		*/	/* <S219> */
	} conf;
	struct	{							/* �����t�@����~���o�p�ϐ���`								*/
		USHORT	FanLockStartCnt;		/* �t�@����~���o�J�n���ԃJ�E���g							*/
		USHORT	FanLockTimeCnt;			/* �t�@����~���o���ԃJ�E���g								*/
		BOOL	FanLockWrnFlag;			/* �t�@����~���[�j���O�����t���O							*/	/* <S219> */
		ULONG	WrnStartTime;			/* �ݐωғ�����[100ms](�t�@����~���[�j���O������)			*/	/* <S219> */
		ULONG	FanLockWrnElapsedTime;	/* �T�[�{�I���������t�@����~�A���[�����o�c�莞��			*/	/* <S219> */
	} var;
} CHECK_FANLOCK;


/*--------------------------------------------------------------------------------------------------*/
/*		�T�[�{�d���Ǘ��p�\���̒�`																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct POW_MNG_HNDL {
	CHECK_MAIN_POWER	MainPowerChecker;			/* ���H�Ď��p�ϐ�(���j�b�g����)				*/
	DET_DC_VOLT			DetectDcVolt;				/* DC�d���d�����o�p�f�[�^(���j�b�g����)			*/
	POWER_STATUS		PowerSts;					/* �d�����(���j�b�g����)						*/
	REGENE_CTRL			RegeneCtrl;					/* �񐶃g�����W�X�^����p�f�[�^(���j�b�g����)	*/
	CHECK_RUSH			Rush;						/* �˓��h�~��R�ߕ��׌��o�p�ϐ�(���j�b�g����)	*/
	PCOV_ERR_DETECT		PcovErrDetect;				/* ���ԓd�ʈُ팟�o�p�ϐ�(���j�b�g����) <S053>	*/
	CHECK_FANLOCK		FanLock;					/* �T�[�{�p�b�N�����t�@����~���o�p�ϐ�			*//* <S0A3> */
} POW_MNG_HNDL;



/*--------------------------------------------------------------------------------------------------*/
/*		����d�͉��Z�p�\���̒�`	<S020>															*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct CALC_WATTAGE {
	struct	{								/* ����d�͉��Z�p�萔��`								*/
		float			CurrentNorm1;			/* ���K���l1(���Ƃő����������O���������邱��)		*/
		float			CurrentNorm2;			/* ���K���l2(���Ƃő����������O���������邱��)		*/
		float			CurrentNorm3;			/* ���K���l3(���Ƃő����������O���������邱��)		*/
		USHORT			Imax;					/* �ő�d���l(�A���v�ƃ��[�^�ŏ�������)				*/
	} conf;
	/*----------------------------------------------------------------------------------------------*/
	struct	{								/* ����d�͉��Z�p�萔��`								*/
		float			PowerConsumption;		/* �u������d��						[W]				*/
		float			PowerConsumption_a;		/* �u������d��						[W]				*//* <S039> */
	} var;

} CALC_WATTAGE;
/*--------------------------------------------------------------------------------------------------*/
typedef	struct POWER_MONITOR {
	struct	{								/* ����d�͉��Z�p�萔��`								*/
		ULONG			TimerCnt1s;				/* 1s�v���p�J�E���^									*/
		ULONG			TimerCnt10s;			/* 10s�v���p�J�E���^								*/
		float			CumulativeEnergy;		/* 1�b���̗ݐϏ���d�͗�			[Wh]			*/
		float			PonCumulativeEnergy;	/* �d����������̗ݐϏ���d�͗�		[Wh]			*/
		LONG			UnConsumedPower;		/* Un032 : ����d��					[W]				*/
		LONG			UnEnergyConsumption;	/* Un033 : ����d�͗� 				[Wh]			*/
		LONG			UnCumulativeEnergyI;	/* Un034 : �ݐϏ���d�͗�(������)	[Wh]			*/
		LONG			UnCumulativeEnergyD;	/* Un035 : �ݐϏ���d�͗�(������)	[0.001Wh]		*//* <S0A5> */
	} var;
} POWER_MONITOR;



/****************************************************************************************************/
/*		DEFINES																						*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		AC�d���������o�����p																		*/
/*--------------------------------------------------------------------------------------------------*/
#define	ACOPEN_DETECT_WAIT			KPI_SCANC_MS( 1000 )		/* �������o����[ms]					*/

/*--------------------------------------------------------------------------------------------------*/
/*		DC�d���Ď��p																				*/
/*		<S053>�ȉ��p�����[�^���Q�Ƃ��鏈���ɕύX�B													*/
/*				PnE88: DC�[�d�����҂�����[ms]														*/
/*				PnE89: �p���[���f�B�҂�����[ms]														*/
/*--------------------------------------------------------------------------------------------------*/
//#define	POWCHARG_WAIT				KPI_SCANC_MS( 180 )		/* DC�[�d�����҂����� : 180ms		*/
//#define	POWREADY_WAIT				KPI_SCANC_MS( 240 )		/* �p���[���f�B�҂����� : 240ms		*/
/* <S21D> Start */
#define CONVINSIGDISCHERRCHK_TIME	KPI_SCANC_MS( 1000 )	/* A.331�i�d���Ď����͐M���ُ�j�}�����d�ُ팟�o���ԁF1s	*/
#define CLINKANSERRCHK_TIME			KPI_SCANC_MS( 500 )		/* A.332�i�R���f���T�o���N�ڑ��A���T�[�ُ�j���o���ԁF500ms	*/
#define CMCANSERRCHK_TIME			KPI_SCANC_MS( 500 )		/* A.333�i�R���f���T�o���NMC�A���T�[�ُ�j���o���ԁF500ms	*/
#define CMCON_WAIT					KPI_SCANC_MS( 10000 )	/* /CMCON �M���o�͑҂����ԁF10000ms		*/
#define DCOFF_FILTER				100U					/* DC�u��ێ����ԃt�B���^�F100ms		*/
/* <S21D> End */
/*--------------------------------------------------------------------------------------------------*/
/*		DC�d���}�����d�����p									<S1C6>								*/
/*--------------------------------------------------------------------------------------------------*/
#define DCVOLT60V					30							/* DcVolt �� 60V��					*/
#define DCVOLT10V					5							/* A.331���o�p�d���l�F10V			*//* <S21D> */
#define DISCH_CHK_TIME				20							/* �}�����d�m�F����[ms](x3��)		*/
#define DISCH_CHK_VOLT				10							/* �}�����d�m�F臒l�d��[V](x3��)	*/
enum {
	DISCH_INIT = 0,		/* �}�����d�V�[�P���X������� 												*/
	DISCH_CHK1ST,		/* �}�����d�m�F��1�� 														*/
	DISCH_CHK2ND,		/* �}�����d�m�F��2�� 														*/
	DISCH_CHK3RD,		/* �}�����d�m�F��3�� 														*/
	DISCH_ACTIVE,		/* �}�����d���s�� 															*/
	DISCH_FINISH,		/* �}�����d����		 														*/
};

/*--------------------------------------------------------------------------------------------------*/
/*		DC�d���d�����o�p��`																		*/
/*--------------------------------------------------------------------------------------------------*/
#define	DETECT_DC_NO_OPERATION		0x00

/*--------------------------------------------------------------------------------------------------*/
/*		�񐶉ߕ��׌��o�p��`																		*/
/*--------------------------------------------------------------------------------------------------*/
#define	CHK_REGOL_NO_OPERATION		0x0000						/* �A���[���Ȃ�(�����s�v)			*/
#define	CHK_REGOL_SET_ALARM			0x0001						/* �S���A���[���o��					*/
#define	CHK_REGOL_SET_WARNING		0x0010						/* �S�����[�j���O�o��				*/
#define	CHK_REGOL_CLR_WARNING		0x0100						/* �S�����[�j���O�N���A				*/
/* <S21D> Start */
#define REGSYNOFF					0U				/* �񐶓����@�\���g�p���܂���						*/
#define PNREGSYN					1U				/* PN������ʃV�X�e���ɂ����ĉ񐶓����@�\���g�p���܂�	*/
#define PNEXREG						2U				/* PN������ʃV�X�e���ɂĊO���񐶋@�\�𗘗p���܂�		*/
/* <S21D> End */

/*--------------------------------------------------------------------------------------------------*/
/*		�˓��h�~��R�ߕ��׌��o�p��`																*/
/*--------------------------------------------------------------------------------------------------*/
#define	CHK_RUSH_NO_OPERATION		0x0000						/* �A���[���Ȃ�(�����s�v)			*/
#define	CHK_RUSH_SET_ALARM			0x0001						/* �S���A���[���o��					*/

/*--------------------------------------------------------------------------------------------------*/
/*		�񐶈ُ팟�o�p��`																			*/
/*--------------------------------------------------------------------------------------------------*/
#define	CHK_REGENE_NO_OPERATION		0x0000						/* �A���[���Ȃ�(�����s�v)			*/
#define	CHK_REGENE_SET_ALARM		0x0001						/* �S���A���[���o��					*/

/*--------------------------------------------------------------------------------------------------*/
/*		�T�[�{�p�b�N�����t�@����~���o�p��`	<S0A3>												*/
/*--------------------------------------------------------------------------------------------------*/
#define	CHK_FANLOCK_NO_OPERATION	0x0000						/* �A���[���Ȃ�(�����s�v)			*/
#define	CHK_FANLOCK_SET_ALARM		0x0001						/* �S���A���[���o��					*/
#define	CHK_FANLOCK_SET_WARNING		0x0010						/* �S�����[�j���O�o��				*/

/*--------------------------------------------------------------------------------------------------*/
/*		�񐶉ߕ��׌��o�p�����l�����I���`															*/
/*--------------------------------------------------------------------------------------------------*/
#define REGPOWSUM_RATE95PER			0x00						/* �񐶉ߕ��׃A���[�����x����95��	*/
#define REGPOWSUM_RATE50PER			0x01						/* �񐶉ߕ��׃A���[�����x����50��	*/

/*--------------------------------------------------------------------------------------------------*/
/*		���H����d�����o����																		*/
/*--------------------------------------------------------------------------------------------------*/
#define	VDET_NONE					0x00		/* �\�t�g�E�F�A�ɂ�錟�o�Ȃ�(SGDM/SGDH�^�C�v)		*/
#define	VDET_PWM_FIX_FOLLOW			0x01		/* PWM-�R���p���[�^���� : �Œ�l(1)�ɂ��Ǐ]		*/
#define	VDET_PWM_VAR_FOLLOW			0x02		/* PWM-�R���p���[�^���� : �ϒl(1-3)�ɂ��Ǐ]		*/
#define	VDET_AD						0x03		/* A/D�ϊ�����										*/
/*--------------------------------------------------------------------------------------------------*/
#define	KPX_VDET_PWM_MAX			255							/* 8bit PWM	(2^n - 1)				*/
#define	KPX_DCVOLT_ADMAX			1023

/*--------------------------------------------------------------------------------------------------*/
/*		�˓�/DB������@��`																			*/
/*--------------------------------------------------------------------------------------------------*/
#define DBCTRL_RLY_COMMON			0x00		/* �˓��{�c�a�F�����[����							*/
#define DBCTRL_TRIACK_SEPARATE		0x01		/* �˓��F�����[�G�c�a�F�T�C���X�^					*/
#define DBCTRL_RLY_SEPARATE			0x02		/* �˓��F�����[�G�c�a�F�����[						*/
#define DBCTRL_TRIACK_COMMON		0x03		/* �˓��{�c�a:�����[�{�T�C���X�^					*/
#define WAIT_RLY_DB					20			/* RLYON�o�͌�DBON�o�͂܂ł̃E�F�C�g����[us] 		*//* <S20E> */

/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
//void	PcmPowerManager( POW_MNG_HNDL *hPowerManager, BOOL MotStopSts );							  /* <S0F6> */
void	PcmPowerManager( POW_MNG_HNDL *hPowerManager, BOOL MotStopSts, BOOL BeSts );				  /* <S0F6> */
void	PcmRegeneControl( REGENE_CTRL *pRegCtrl, POWER_STATUS *PowSts, BOOL MotStop );
LONG	PcmDetectDcVolt( DET_DC_VOLT *pDetectDc, POWER_STATUS *PowSts );
//LONG	PcmCheckOverVoltage( CHECK_MAIN_POWER *PowerChecker, DET_DC_VOLT *pDetectDc, POWER_STATUS *PowSts ); /* <S0F7> */
LONG	PcmCheckOverVoltage( CHECK_MAIN_POWER *PowerChecker, DET_DC_VOLT *pDetectDc,
														POWER_STATUS *PowSts, REGENE_CTRL *RegCtrl ); /* <S0F7> */
LONG	PcmCheckRegeneOverLoad( REGENE_CTRL *pRegCtrl, ULONG *AlmOLInfo, ANLG_UN_MON_DATA *MonData );
void	SetRegeneOverLoadSts( LONG RegOlSts );
BOOL	PcmCheckRushRxOverLoad( CHECK_RUSH *pRush, POWER_STATUS *pPowerSts );
BOOL	PcmCheckRegeneAlarm( REGENE_CTRL *pRegCtrl, BOOL PowerOn );
LONG	PcmCheckFanLock( CHECK_FANLOCK *FanLock, BOOL FanStop, BOOL BbSts );						  /* <S0A3> */
void	PcmInitPowerManager( POW_MNG_HNDL *hPowerManager, ULONG AlmOLInfo );
void	PcmCountRegeneOnTime( POW_MNG_HNDL *hPowerManager );
void	PcmInputPowerStatus( POW_MNG_HNDL *hPowerManager, BOOL Acon1, BOOL RegeneAlm );

//void	IprmcalRegenePowerGain( REGENE_CTRL *RegeneCtrl, BPRMDAT *Bprm, USHORT regonlvl, USHORT	reger, USHORT ConvW);
void	IprmcalRegenePowerGain( REGENE_CTRL *RegeneCtrl, BPRMDAT *Bprm, REGCFG_PRM *RegPrm );
BOOL	PcalRegeneOverLoadLevel( REGENE_CTRL *RegeneCtrl,USHORT regres, USHORT wre, USHORT ConvW );
void	PcmCalculateVdetAdFil( DET_DC_VOLT *pDetectDc, USHORT vdetadfil, LONG SvCycleUs );

void	PcmRlyDbControlProcess( ASIC_HW_IF_STR *pAsicHwReg, POW_MNG_HNDL *PowerManager,
									BOOL BbSts, BOOL DbOn, BOOL KnlMode, BOOL MotorLessTestMode );	  /* <S20E> */
LONG	PcmGetDcVoltage( POWER_STATUS *PowSts );													  /* <S020> */

void	PrmCalcPowerMonitor( CALC_WATTAGE *CalcWattage, USHORT AmpImax, USHORT MotImax );			  /* <S020> */
void	InitializePowerMonitor( POWER_MONITOR *PowMon );											  /* <S020> */
void	CalcPowerMoniter( CALC_WATTAGE *CalcWattage, SHORT PowMonVq, SHORT PowMonIqd,				  /* <S020> *//* <S039> */
							SHORT PowMonVd, SHORT PowMonIdd, LONG DcVolt, BOOL BaseEnable );
void	SetPowerMoniter( POWER_MONITOR *PowMon, float PowerConsumption );							  /* <S020> */
void	ResetPowerConsumption( CALC_WATTAGE *CalcWattage );											  /* <S020> */
float	GetPowerConsumption( CALC_WATTAGE *CalcWattage );											  /* <S020> */

#define	_POWER_MANAGER_H_
#endif	/* _POWER_MANAGER_H_ */
/***************************************** end of file **********************************************/
