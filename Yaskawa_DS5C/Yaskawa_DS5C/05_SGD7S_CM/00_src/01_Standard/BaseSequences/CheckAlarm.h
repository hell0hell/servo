/****************************************************************************************************/
/*																									*/
/*																									*/
/*		CheckAlarm.h : �A���[���Ď�������`															*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	����	2010.02.23	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#ifndef CHECK_ALARM_H
#define CHECK_ALARM_H


#include "MLib.h"
#include "EncData.h"
#include "AlarmManager.h"
#include "SftDiagFunc.h"


/****************************************************************************************************/
/*		DEFINES																						*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		�G���R�[�_�A���[���p��`																	*/
/*--------------------------------------------------------------------------------------------------*/
#define PG_OH_LEVEL				90							/* �o�f�I�[�o�q�[�g���x [��]			*/

/*--------------------------------------------------------------------------------------------------*/
/*		���j�A�p��`																				*/
/*--------------------------------------------------------------------------------------------------*/
#define	POSDATALMT				0x70000000					/* �ʒu�f�[�^�I�[�o臒l[�w�ߒP��]		*/

/*--------------------------------------------------------------------------------------------------*/
/*		���[�^�\�����o�p��`																		*/
/*--------------------------------------------------------------------------------------------------*/
#define MOTSPD_OS_10	1677721		/* OS���x��1/10													*/


/*--------------------------------------------------------------------------------------------------*/
/*		�\���Ď��p�\���̒�`																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct CHECK_OVRRUN {
	LONG	OvrChkCnt;						/* �\���Ď� : �Ď����ԃJ�E���g							*/
	LONG	OvrAlmCnt;						/* �\���Ď� : �A���[���J�E���g							*/
	LONG	OvrTrqRef;						/* �\���Ď� : �O��g���N�w��							*/
	LONG	OvrAbsMotSpd;					/* �\���Ď� : �O�񃂁[�^���x��Βl						*/
	LONG	OvrPeakMotSpd;					/* �\���Ď� : ���[�^���x�ő�l							*/
	LONG	OvrTrqLevel;					/* �\���Ď� : �g���N���x��								*/
	LONG	OvrSpdLevel;					/* �\���Ď� : ���x���x��								*/
} CHECK_OVRRUN;


/*--------------------------------------------------------------------------------------------------*/
/*		�A���v�����[�^�ߕ��׃`�F�b�N�p�f�[�^��`													*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct OLPRM {					/* �ߕ��׃`�F�b�N��{�p�����[�^								*/
	LONG	Tmid;							/* ���ԓd��OL����(�f�B���[�e�B���O�܂�)	[0.1s]			*/
	LONG	Tmax;							/* �ő�d��OL����(�f�B���[�e�B���O�܂�)	[0.1s]			*/
	LONG	Ibse;							/* �x�[�X�d��(�f�B���[�e�B���O�܂�)		[15000/Imax]	*/
	LONG	Imid;							/* ����OL�d��(�f�B���[�e�B���O�܂�)		[15000/Imax]	*/
	LONG	Imax;							/* �ő�OL�d��(�f�B���[�e�B���O�܂�)		[15000/Imax]	*/
} OLPRM;


/*--------------------------------------------------------------------------------------------------*/
/*		�ߕ��׃`�F�b�N�p�\���̒�`																	*/
/*--------------------------------------------------------------------------------------------------*/
#if 1	/* <S154> */
typedef	struct _CHECK_OLP {					/* �ߕ��׃`�F�b�N�p�p�����[�^��`						*/
		LONG	Ibsqrmid;					/* �A���ߕ��׃x�[�X�d���̂Q��							*/
		LONG	Ibsqrmax;					/* �u���ߕ��׃x�[�X�d���̂Q��							*/
		LONG	AlmLvlmid;					/* �A���ߕ��׃A���[�����x��								*/
		LONG	WrnLvlmid;					/* �A���ߕ��׃��[�j���O���x��							*/
		LONG	AlmLvlmax;					/* �u���ߕ��׃A���[�����x��								*/
		LONG	WrnLvlmax;					/* �u���ߕ��׃��[�j���O���x��							*/
		KSGAIN	OlLvlmidGain;				/* �A���ߕ��׌��o���x���Q�C��[�ᑬOL�Ή�]				*/
		KSGAIN	OlLvlmaxGain;				/* �A���ߕ��׌��o���x���Q�C��[�ᑬOL�Ή�]				*/
		KSGAIN	CoolRateGain;				/* �������W��(���j�A�p)								*/
} CHECK_OLP;/* <S154> */

/* <S154> Start */
/*--------------------------------------------------------------------------------------------------*/
/*		�ߕ��׃`�F�b�N�p�\���̒�`																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct CHECK_OL {
	struct	{							/* �ߕ��׃`�F�b�N�p�p�����[�^��`						*/
		CHECK_OLP	NormalArea;			/* �ʏ��ԉߕ��׃`�F�b�N�p�p�����[�^					*/
		CHECK_OLP	LowSpdArea;			/* �ᑬ��ԉߕ��׃`�F�b�N�p�p�����[�^					*/
		CHECK_OLP	LockArea;			/* ���b�N��ԉߕ��׃`�F�b�N�p�p�����[�^					*/
	} conf;
/*--------------------------------------------------------------------------------------------------*/
	struct	{							/* �ߕ��׃`�F�b�N�p�ϐ���`									*/
		LONG	WrnInfo;					/* ���[�j���O���										*/
		LONG	SumImid;					/* �A���ߕ��חp�Q��d���ώZ�l							*/
		LONG	SumImax;					/* �u���ߕ��חp�Q��d���ώZ�l							*/
	} var;
} CHECK_OL;
#else	/* <S154> */
typedef	struct CHECK_OL {
	struct	{							/* �ߕ��׃`�F�b�N�p�p�����[�^��`							*/
		LONG	Ibsqrmid;					/* �A���ߕ��׃x�[�X�d���̂Q��							*/
		LONG	Ibsqrmax;					/* �u���ߕ��׃x�[�X�d���̂Q��							*/
		LONG	AlmLvlmid;					/* �A���ߕ��׃A���[�����x��								*/
		LONG	WrnLvlmid;					/* �A���ߕ��׃��[�j���O���x��							*/
		LONG	AlmLvlmax;					/* �u���ߕ��׃A���[�����x��								*/
		LONG	WrnLvlmax;					/* �u���ߕ��׃��[�j���O���x��							*/
		KSGAIN	OlLvlmidGain;				/* �A���ߕ��׌��o���x���Q�C��[�ᑬOL�Ή�]				*/
		KSGAIN	OlLvlmaxGain;				/* �A���ߕ��׌��o���x���Q�C��[�ᑬOL�Ή�]				*/
		KSGAIN	CoolRateGain;				/* �������W��(���j�A�p)								*/
	} conf;
/*--------------------------------------------------------------------------------------------------*/
	struct	{							/* �ߕ��׃`�F�b�N�p�ϐ���`									*/
		LONG	WrnInfo;					/* ���[�j���O���										*/
		LONG	SumImid;					/* �A���ߕ��חp�Q��d���ώZ�l							*/
		LONG	SumImax;					/* �u���ߕ��חp�Q��d���ώZ�l							*/
	} var;
} CHECK_OL;
#endif	/* <S154> */


/*--------------------------------------------------------------------------------------------------*/
/*		DB�ߕ��׃`�F�b�N�p�\���̒�`																*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct CHECK_DBOL {
	struct	{							/* DB�ߕ��׃`�F�b�N�p�萔��`								*/
		BOOL	DbOHeatCheck;				/* ���x�㏸�ɂ��c�a�ߕ��׌��o�����̗L���I��			*/
		LONG	Kdbpower;					/* �c�a�ߕ��� : �d�͉��Z�Q�C��							*/
		LONG	DbolPowMax; 				/* �c�a�ߕ��� : �d�͍ő�l								*/
		LONG	DbolPowBase; 				/* �c�a�ߕ��� : �d�̓x�[�X								*/
		LONG	DbolSpdLevel;				/* �c�a�ߕ��� : ���x���x��								*/
		LONG	DbolAlmLevel;				/* �c�a�ߕ��� : �A���[�����x��							*/
		LONG	Kundbpower;					/* �c�a��R����d�͉��Z�Q�C��(for Un00B)				*/
		LONG	KundbpowerCycle;			/* �c�a��R����d�͉��Z�Q�C��(for Un144)				*/
		LONG	DbOHeatAlmLvl;				/* �c�a�ߕ���(���x�㏸)�F�A���[�����x��					*/
		LONG	DbOHeatWrnLvl;				/* �c�a�ߕ���(���x�㏸)�F���[�j���O���x��				*/
	} conf;
/*--------------------------------------------------------------------------------------------------*/
	struct	{							/* DB�ߕ��׃`�F�b�N�p�ϐ���`								*/
		BOOL	DbOHeatWarn;				/* �c�a�ߕ���(���x�㏸)���[�j���O���o���t���O			*/
		LONG	DbolPowSum;					/* �c�a�ߕ��� : �d�͐ώZ�l								*/
		LONG	DbPowerSum;					/* �c�a���ϒl : �d�͐ώZ�l								*/
		LONG	DbMeanCycle;				/* �c�a���ϒl : �d�͐ώZ��							*/
		LONG	DbHeat;						/* �c�a�ߕ���(���x�㏸)���o�p�ώZ�l						*/
		LONG	UnDbPower;					/* Un144  : �c�a����d�͒l 			[%]					*/
		LONG	UnMeanDbPower;				/* Un00B  : �c�a����d�͕��ϒl 		[%]					*/
	} var;
} CHECK_DBOL;

/* <S154> Start */
/************************************************************************************************/
/*																								*/
/*		�A���v�ᑬ�ߕ��׃`�F�b�N�p�ϐ���`																*/
/*																								*/
/************************************************************************************************/
/************************************************************************************************/
/*		�A���v�ᑬ�ߕ��ׁF�r�f�c�r����																	*/
/************************************************************************************************/
typedef	struct _LOWOLPRM{							/* �ᑬ�ߕ��׃`�F�b�N��{�p�����[�^					*/
		OLPRM	AmpLock;						/* ���b�N��ԗp��{�p�����[�^							*/
		OLPRM	AmpLow;							/* �ᑬ��ԗp��{�p�����[�^							*/
} LOWOLPRM;
/*----------------------------------------------------------------------------------------------*/
typedef	struct _LOWOLCHKP{							/* �ᑬ�ߕ��׃`�F�b�N�p�p�����[�^					*/
		struct {
		LONG	LowSpdOLchg;					/* �ᑬ�n�k�ϋȓ_���x[�ᑬ���]	[2^24/OvrSpd]	*/
		LONG	LockSpdOLchg;					/* �ᑬ�n�k�ϋȓ_���x[���b�N���]	[2^24/OvrSpd]	*/
		LONG	LowSpdOffsetImid;				/* �ᑬ�n�k�ϋȓ_�n�k�d��[�ᑬ���] [15000/Imax]		*/
		LONG	LowSpdOffsetTmax;				/* �ᑬ�n�k�ϋȓ_�n�k����[�ᑬ���] [0.1s]			*/
		LONG	TransOLImid;					/* �ᑬ�n�k���Ԃn�k�d���ϊ��W�� 						*/
		LONG	TransOLTmax;					/* �ᑬ�n�k�ő�n�k���ԕϊ��W�� 						*/
		UCHAR	LowSpdOLChkEnable;				/* �ᑬ�ߕ��׃`�F�b�N���s�t���O						*/
		CHAR	Dummy1;							/* for alignment								*/
		SHORT	Dummy2;							/* for alignment 								*/
		} conf;
} LOWOLCHKP;

typedef	struct _CHECK_LOWOL{							/* �ᑬ�ߕ��׃`�F�b�N�p�\���̒�`			*/
	LOWOLPRM	LowOLPrm;								/* �ᑬ�ߕ��׃`�F�b�N��{�p�����[�^				*/
	LOWOLCHKP	LowOLChkP;								/* �ᑬ�ߕ��׃`�F�b�N�p�p�����[�^				*/
	/* <S1F5> Start */
	struct {
		BOOL	BaseCurrentDerateEnable;				/* �ߕ��׌��o�x�[�X�d���f�B���[�e�B���O�@�\�L�� 	*/
	}conf;
	/* <S1F5> End */
} CHECK_LOWOL;
/* <S154> End */

#if 0	/*<S1F6>*/
/*--------------------------------------------------------------------------------------------------*/
/*		�Z�[�t�e�B���͐M���^�C�~���O�ُ�`�F�b�N�p�\���̒�`										*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct CHECK_HWBB_TIMING {
	struct	{							/* �Z�[�t�e�B���͐M���^�C�~���O�ُ�`�F�b�N�p�萔��`		*/
		LONG	MaxBbonTimeDiff;			/* ���͐M�����ԍ��Ď��F�ő勖�e����						*/
	} conf;
/*--------------------------------------------------------------------------------------------------*/
	struct	{							/* �Z�[�t�e�B���͐M���^�C�~���O�ُ�`�F�b�N�p�ϐ���`		*/
		LONG	BbonTimeDiffCnt;			/* �Z�[�t�e�B���͐M���^�C�~���O�ُ펞�ԃJ�E���g			*/
	} var;
} CHECK_HWBB_TIMING;
#endif	//#if 0	/*<S1F6>*/


/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu�΍��ߑ�`�F�b�N�p�\���̒�`															*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct CHECK_OVRPOSERR {
	LONG		PerAlmLevel;				/* �ʒu�΍��ߑ�A���[�����x��				[�w�ߒP��]	*/
	LONG		PerWrnLevel;				/* �ʒu�΍��ߑ像�[�j���O���x��				[�w�ߒP��]	*/
	LONG		SvonPerAlmLevel;			/* �T�[�{�I�����ʒu�΍��ߑ�A���[�����x��	[�w�ߒP��]	*/
	LONG		SvonPerWrnLevel;			/* �T�[�{�I�����ʒu�΍��ߑ像�[�j���O���x��	[�w�ߒP��]	*/
} CHECK_OVRPOSERR;

typedef	struct CHECK_FENCPOSERR {
	LONG	FencPosLast;					/* �Z�~�t���΍��ߑ匟�o�F�t���N�O��ʒu					*/
	LONG	MencPosLast;					/* �Z�~�t���΍��ߑ匟�o�F�Z�~�N�O��ʒu					*/
	LONG	ExtPGRem;						/* �Z�~�t���΍��ߑ匟�o�F								*/
	LONG	MtPGRem;						/* �Z�~�t���΍��ߑ匟�o�F								*/
	LONG	FencPosOneTurn;					/* �Z�~�t���΍��ߑ匟�o�F�t���N�P��]�O�ʒu				*/
	ULONG	FSOFAlmCnt;						/* �Z�~�t���΍��ߑ匟�o�F���o�t�B���^					*/
	PERRA	sfperra;						/* �ʒu�΍��`�\���� (��-��)�΍��v�Z�p					*/

	EGEAR	Egear;							/* �ЃM���\����											*/
	EGEAR	sfgear;							/* ��/�ٕϊ��M���\����									*/
	ULONG	fpgoverLevel;					/* ���[�^-���׈ʒu�ԕ΍��ߑ匟�o���x��		[�w�ߒP��]	*/
	LONG	fpgcutratelv;					/* �t���N���[�Y1��]������̏�Z�l				   [%]	*/
	LONG	fcpgpls;						/* �O�t���X�P�[���s�b�`��					  [P/Rev]	*/
	USHORT	fpgcutrate;						/* Pn52A : �t���N���[�Y1��]������̏�Z�l				*/
} CHECK_FENCPOSERR;

/*--------------------------------------------------------------------------------------------------*/
/*		�A���[�����o�����p�\���̒�`																*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct CHECK_ALARM {
//	ENVMON				EnvMon;				/* �ݒu�����j�^�p�ϐ�									*//* <S03A> *//* <S0ED> */
//	CHECK_OH			OverHeat;			/* �I�[�o�[�q�[�g�`�F�b�N�p�ϐ�							*//* <S03A> *//* <S0ED> */
	CHECK_OL			MotOL;				/* ���[�^�ߕ��׃`�F�b�N�p�ϐ�							*/
	CHECK_OL			AmpOL;				/* �A���v�ߕ��׃`�F�b�N�p�ϐ�							*/
	CHECK_OVRRUN		OvrRun;				/* �\�����o�p�ϐ�										*/
	CHECK_DBOL			DbOL;				/* DB�ߕ��׃`�F�b�N�p�ϐ�								*/
	CHECK_HWBB_TIMING	HwbbTiming;			/* HWBB���͐M���^�C�~���O�ُ�`�F�b�N�p�ϐ�				*/
	CHECK_OVRPOSERR		OvrPosErr;			/* �ʒu�΍��ߑ�`�F�b�N�p�ϐ�							*/
	CHECK_FENCPOSERR	OvrFPosErr;			/* �Z�~�t���ʒu�΍��ߑ�`�F�b�N�p�ϐ�					*/
	CHECK_LOWOL			AmpLowOL;			/* �A���v�ᑬ�ߕ��חp�ϐ���`							*/ /* <S154>  */
//	CHECK_RUSH			*pRush;				/* �˓��h�~��R�ߕ��׌��o�p�ϐ�							*/
//	CHECK_REGOL			*pRegOL;			/* �񐶉ߕ��׃`�F�b�N�p�ϐ��ւ̃|�C���^(���j�b�g����)	*/
//	CHECK_REGENE		*pRegChk;			/* �񐶈ُ�`�F�b�N�p�ϐ��ւ̃|�C���^(���j�b�g����)		*/
	ULONG				OSCount;			/* OS��ԘA���J�E���^									*/
	LONG				Gup1Timer;			/* A.b33���o�p�^�C�}									*/
	USHORT 				faultState;			/* Fault status of Servo ASIC 							*/

} CHECK_ALARM;




/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
void	AdetInitCheckAlarm( CHECK_ALARM *pChkAlm, ULONG AlmOLInfo );
USHORT	AdetCheckAsicHwAlarm( CHECK_ALARM *pChkAlm, ASICS *SvAsic,
										BOOL EncDisable, ALMMAN_HANDLE *AlmMngr );
void	AdetCheckOCAlarmDetections( ALMMAN_HANDLE *AlmMngr, ASICS *SvAsic, LONG axisNo, BOOL BeSts );	/* <S126> *//* <S137> */
void	AdetCheckMotorOverCurrent( ALMMAN_HANDLE *AlmMngr, ASICS *SvAsic, BOOL BeSts, BOOL BaseEnableSts );/*<S1F5>*/
void	AdetCheckSencAlarm( ALMMAN_HANDLE *AlmMngr, MENCV *MencV, USHORT RxAlmCode );					/* <S03A> */
void	AdetCheckSencBaseTimer(MENCV *MencV, ALMMAN_HANDLE *AlmMngr );
void	AdetCheckSencOverHeat( MENCV *MencV, ALMMAN_HANDLE *AlmMngr );
void	AdetCheckOverSpeed( ALMMAN_HANDLE *AlmMngr, CHECK_ALARM *pChkAlm, LONG MotSpd, BPRMDAT *Bprm );
/*--------------------------------------------------------------------------------------------------*/
void	AdetCheckDivOverSpeed( ALMMAN_HANDLE *AlmMngr, LONG MotSpd, LONG DivOvrSpd );				/*<S02D>*/
/*--------------------------------------------------------------------------------------------------*/
//void	AdetCheckOverHeatAlm( ALMMAN_HANDLE *AlmMngr, CHECK_OH *CheckOh,							  /* <S03A> *//* <S0ED> */
//														SHORT CtrlBoadTemp, SHORT PowerBoadTemp );		/* <S0ED> */
//void	AdetCheckTempDeviceErr( ALMMAN_HANDLE *AlmMngr, USHORT CtrlBoadErrCnt, USHORT PowerBoadErrCnt );/* <S0DE> *//* <S0ED> */
//void	AdetInstEnvMonitor( ENVMON *EnvMon, SHORT AmpTemp, SHORT MotTemp );							  /* <S03A> *//* <S0ED> */

//void	AdetCheckOverLoadAmpMot( ALMMAN_HANDLE *AlmMngr, CHECK_ALARM *CheckAlarm,
//								LONG IdRefMon, LONG IqRefMon, LONG AbsMotSpd, BOOL EncConnect );				/* <S154> */
BOOL	AdetCheckOverLoadAmpMot( ALMMAN_HANDLE *AlmMngr, CHECK_ALARM *CheckAlarm,
								LONG IdRefMon, LONG IqRefMon, LONG AbsMotSpd, BOOL EncConnect );				/* <S154> */

void	AdetCheckOLAmpMot( LONG AbsMotSpd, LONG Ixsqr, CHECK_OL *ol_ptr, ALMMAN_HANDLE *AlmMngr , UCHAR Mode);	/* <S154> */

void	AdetCheckDbrakeOverLoad( ALMMAN_HANDLE *AlmMngr, CHECK_DBOL *DbOL, BOOL DbOn, LONG AbsMotSpd );
void	AdetCheckRippleWrn( ALMMAN_HANDLE *AlmMngr, BOOL RippleWrnState );									/* <S1FD> */
BOOL	AdetCheckBbonSignal( CHECK_HWBB_TIMING *pHwbbTmg, BOOL LackOneBbon );
//BOOL	AdetCheckGateDriveDefect( DET_HWBB_STS *pDetHwbbSts );
void	AdetCheckMotorOverrun( ALMMAN_HANDLE *AlmMngr, CHECK_OVRRUN *OvrRun,
								 LONG MotSpd, LONG TrqRefMon, LONG BaseEnableSts, BOOL MotTypeLinear );
void	AdetCheckSvonPerrOverFlow( ALMMAN_HANDLE *AlmMngr, CHECK_OVRPOSERR *OvrPosErr, LONG PositionError );
void	AdetCheckPerrOverFlow( ALMMAN_HANDLE *AlmMngr, CHECK_OVRPOSERR *OvrPosErr,
										BOOL PosCtrlMode, LONG PositionError, BOOL SvonSpdLmt );
void AdetCheckFullcEncoderAlarm(CHECK_FENCPOSERR *ChkFposErr, MENCV *FencV,
                            	BOOL PosCtrlMode, LONG MencPos, LONG FencPos, ALMMAN_HANDLE *AlmMngr );
//<S1F5>BOOL	AdetTestPulseDiagnostic( BOOL DpInState, BOOL DpInLastState, BOOL CheckEnable, BOOL Bbon );					/* <S0F1> *//*<S1DB>*/





#endif /* CHECK_ALARM_H */
/***************************************** end of file **********************************************/
