/****************************************************************************************************/
/*																									*/
/*																									*/
/*		ServoSequence.h : �T�[�{�V�[�P���X�Ǘ�������`												*/
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
#ifndef __BASE_ENABLE_SEQUENCE_H_
#define __BASE_ENABLE_SEQUENCE_H_


//#include "BaseCmnStruct.h"
//#include "alarm.h"
//#include "EncData.h"
#include "HwDeviceIf.h"
#include "PowerManager.h"/*<S201>*/  
                         /*#include "JL086ServoIpReg.h"*/
                         /*#include "BaseCmnStruct.h"*/
                         /*#include "Bprm.h" -------->#include "alarm.h"*/
                         /*                  --------->#include "EncData.h"*/
#include "AlarmManager.h"/*<S201>*/
/*#include "SequenceIo.h" -------------->#include "BaseEnableSequence.h" */

/****************************************************************************************************/
/*		DEFINES																						*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		�X�g�b�v���[�h��`																			*/
/*--------------------------------------------------------------------------------------------------*/
/*		���ӁF�X�g�b�v���[�h���`����ꍇ�́A0x0E�ȉ��̒l�Œ�`���邱��(�A���[����`�Ƃ̊֌W)�B	*/
/*			  �X�g�b�v���[�h�͒�`�l�̑傫�������D��x�͍����B										*/
/*--------------------------------------------------------------------------------------------------*/
#define	STOPMODE_NOTSTOP		0x00		/* ���~(�������Ȃ�)									*/
#define	STOPMODE_ZEROSTOP		0x01		/* �[������~											*/
#define	STOPMODE_LDECSTOP		0x02		/* ���������x�ɂ�錸����~								*/   /* <S1F5> */
/*--------------------------------------------------------------------------------------------------*/
// #define	STOPMODE_DBSTOPDB		0x04		/* �c�a�Ń��[�^��~���c�a�p��(Pn001.0==0)				*/  /* <S1F5> */
// #define	STOPMODE_DBSTOPFR		0x05		/* �c�a�Ń��[�^��~���c�a����(Pn001.0==1)				*/    /* <S1F5> */
// #define	STOPMODE_FREERUN		0x06		/* �c�a���g�킸�A�t���[������~(Pn001.0==2)				*/    /* <S1F5> */
#define	STOPMODE_DBSTOPDB		0x08		/* �c�a�Ń��[�^��~���c�a�p��(Pn001.0==0)				*/   /* <S1F5> */
#define	STOPMODE_DBSTOPFR		0x09		/* �c�a�Ń��[�^��~���c�a����(Pn001.0==1)				*/   /* <S1F5> */
#define	STOPMODE_FREERUN		0x10		/* �c�a���g�킸�A�t���[������~(Pn001.0==2)				*/   /* <S1F5> */
/*--------------------------------------------------------------------------------------------------*/
/* <S1F5> Start */
/*		Gr.2�A���[���X�g�b�v���[�h��`																*/
/*--------------------------------------------------------------------------------------------------*/
#define	GR2STOPMODE_DEFAULT		0x03		/* Pn001.0�Ɠ���(Pn00A.0==0)							*/
#define	GR2STOPMODE_ZSTOPDB		0x04		/* ������~��DB��~���(Pn00A.0==1)						*/
#define	GR2STOPMODE_ZSTOPFR		0x05		/* ������~���t���[�������(Pn00A.0==2)					*/
#define	GR2STOPMODE_LDSTOPDB	0x06		/* ����������~��DB��~���(Pn00A.0==3)					*/
#define	GR2STOPMODE_LDSTOPFR	0x07		/* ����������~���t���[�������(Pn00A.0==4)				*/
/*--------------------------------------------------------------------------------------------------*/
/*		Gr2/DB��~���[�h��`																		*/
/*--------------------------------------------------------------------------------------------------*/
#define	GR2DBOP_NONE			0x00		/* �c�a�����v���Ȃ�										*/
#define	GR2DBOP_FREE			0x02		/* �t���[������~�v������								*/
/*--------------------------------------------------------------------------------------------------*/
/*		FSTP�X�g�b�v���[�h��`																		*/
/*--------------------------------------------------------------------------------------------------*/
#define	FSTOPMODE_DEFAULT		0x03		/* Pn001.0�Ɠ���(Pn00A.1==0)							*/
#define	FSTOPMODE_ZSTOPDB		0x04		/* ������~��DB��~���(Pn00A.1==1)						*/
#define	FSTOPMODE_ZSTOPFR		0x05		/* ������~���t���[�������(Pn00A.1==2)					*/
#define	FSTOPMODE_LDSTOPDB		0x06		/* ����������~��DB��~���(Pn00A.1==3)					*/
#define	FSTOPMODE_LDSTOPFR		0x07		/* ����������~���t���[�������(Pn00A.1==4)				*/
/*--------------------------------------------------------------------------------------------------*/
/*		FSTP/DB��~���[�h��`																		*/
/*--------------------------------------------------------------------------------------------------*/
#define	FSTPDBOP_NONE			0x00		/* �c�a�����v���Ȃ�										*/
#define	FSTPDBOP_FREE			0x02		/* �t���[������~�v������								*/
/*--------------------------------------------------------------------------------------------------*/
/* <S1F5> End */
/*		OT���X�g�b�v���[�h��`																		*/
/*--------------------------------------------------------------------------------------------------*/
#define	OTSTOPMODE_DEFAULT		0x00		/* Pn001.0�Ɠ���										*/
#define	OTSTOPMODE_ZSTOPZC		0x01		/* ������~���T�[�{���b�N								*/
#define	OTSTOPMODE_ZSTOPFR		0x02		/* ������~���t���[����									*/
#define	OTSTOPMODE_LDSTOPZC		0x03		/* ����������~���T�[�{���b�N							*/      /* <S1F5> */
#define	OTSTOPMODE_LDSTOPFR		0x04		/* ����������~���t���[����								*/     /* <S1F5> */
/*--------------------------------------------------------------------------------------------------*/
/*		OT/DB��~���[�h��`																			*/
/*--------------------------------------------------------------------------------------------------*/
#define	OTDBOP_NONE				0x00		/* �c�a�����v���Ȃ�										*/
#define	OTDBOP_DEF				0x01		/* �n�s�W����~											*/
#define	OTDBOP_FREE				0x02		/* �t���[������~�v������								*/
/*--------------------------------------------------------------------------------------------------*/
/*		OT�����DB����v�����																		*/
/*--------------------------------------------------------------------------------------------------*/
//#define	OTDBOP_NONE				0x00		/* �c�a�����v���Ȃ�										*/
//#define	OTDBOP_DEF				0x01		/* �n�s�W����~											*/
//#define	OTDBOP_FREE				0x02		/* �t���[������~�v������								*/


/****************************************************************************************************/
/*		STRUCT DEFINITION																			*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		�A���[����~����p�\���̒�`																*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct ALM_STOP {
	struct {
		BOOL		AlmZeroStop;			/* Alarm Zero Stop Request								*/
		BOOL		AlmZstopFinish;			/* Alarm Zero Stop Finish								*/
		LONG		AlmStopMode;			/* Alarm Stop Mode for Sequence							*/
		BOOL		Gr2TrqLimit;			/* Alarm Stop Torque Limit Request						*/    /* <S1F5> */
		BOOL		Gr2ZeroStop;			/* Alarm Stop Zero Stop  Request						*/   /* <S1F5> */
		BOOL		Gr2LinDecStop;			/* Alarm Stop Linearity Deceleration Stop  Request		*/   /* <S1F5> */
		LONG		Gr2DbopReq;				/* Alarm Stop DB Operate Request						*/   /* <S1F5> */
		LONG		Gr2StopStep;			/* Alarm Stop Step										*/   /* <S1F5> */
	} var;
	struct {
//		BOOL		G2AlmStopMode;			/* G2�A���[���̒�~���@									*/   /* <S1F5> */
		LONG		G2AlmStopMode;			/* G2�A���[���̒�~���@									*/   /* <S1F5> */
		LONG		StopMode;				/* �T�[�{�I�t�y�уA���[���������̒�~���[�h				*/
	} conf;
} ALM_STOP;

/* <S1F5> Start */
/*--------------------------------------------------------------------------------------------------*/
/*		������~����p�\���̒�`																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct FORCE_STOP {
	struct {
		LONG		FstpMode;				/* Force Stop Mode for Sequence							*/
		BOOL		FstpTrqLimit;			/* Force Stop Torque Limit Request						*/
		BOOL		FstpZeroStop;			/* Force Stop Zero Stop  Request						*/
		BOOL		FstpLinDecStop;			/* Force Stop Linearity Deceleration Stop  Request		*/
		LONG		FstpDbopReq;			/* Force Stop DB Operate Request						*/
		LONG		FstpStopStep;			/* Force Stop Step										*/
	} var;
	struct {
		LONG		ForceStopMode;			/* ������~�̒�~���@									*/
	} conf;
} FORCE_STOP;
/* <S1F5> End */

/*--------------------------------------------------------------------------------------------------*/
/*		OT����p�\���̒�`																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct OT_SEQ_DATA {
	struct {								/* OT�V�[�P���X�p�萔��`								*/
		BOOL			OtWarningSw;			/* OT���[�j���O���o�I��								*/
		LONG			OtStopMode;				/* �I�[�o�g���x��(OT)���̒�~���@					*/
	} conf;
	struct {								/* OT�V�[�P���X�p�ϐ���`								*/
		BOOL			OverTrvlSts;			/* OT���(OT�V�[�P���X�����o��)						*/
		BOOL			OtBeReq;				/* OtSequence BaseEnable Request					*/
		BOOL			OtTrqLimit;				/* OT Sequence Torque Limit Request					*/
		BOOL			OtZeroStop;				/* OT Sequence Zero Stop  Request					*/
		BOOL			OtLinDecStop;			/* OT Sequence Linearity Deceleration Stop  Request	*/    /* <S1F5> */
		BOOL			OtZeroClamp;			/* OT Sequence Zero Clamp Request					*/
		LONG			OtStep;					/* OT Sequence Step									*/
		LONG			OtWait;					/* OT Sequence Wait Counter							*/
		LONG			OtOnCnt;				/* OT Sequence OT-ON Wait Counter					*/
		LONG			OtOffCnt;				/* OT Sequence OT-OFF Wait Counter					*/
		LONG			OtDbopReq;				/* OT Sequence DB Operate Request					*/
		BOOL			OtWarningDet;			/* OT Warning Detect								*/
		LONG			OtWarngWaitCnt;			/* OT Sequence Warning Clear Wait Count				*/
		LONG			CtrlMode2;				/* OT Sequence OT CtrlMode for M3 I/F				*/
	} var;
} OT_SEQ_DATA;


/*--------------------------------------------------------------------------------------------------*/
/*		�u���[�L����p�\���̒�`																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct BK_SEQ_DATA {
	struct {								/* �u���[�L����p�萔��`								*/
		LONG			BkonSpdLevel;			/* �u���[�L�w�ߏo�͑��x���x��(Pn507)	[2^24/OS]	*/
		LONG			BkonWaitTime;			/* �T�[�{�I�t�u���[�L�w�ߑ҂�����(Pn508)	 [ms]	*/
		LONG			BkonDelayTime;			/* �u���[�L�w�߃T�[�{�I�t�x�ꎞ��(Pn506)	 [ms]	*/
	} conf;
	struct {								/* �u���[�L����p�ϐ���`								*/
		BOOL			BkBeReq;				/* BkSequence BaseEnable Request						*/
		BOOL			Brake;					/* BK Sequence Brake Request						*/
		BOOL			BkZeroStop;				/* BK Sequence Zero Stop  Request					*/
		BOOL			BkLinDecStop;			/* BK Sequence Linearity Deceleration Stop Request	*/	/* <S1F5> */
		LONG			BkStep;					/* BK Sequence Step									*/
		LONG			BkWait;					/* BK Sequence Wait Counter							*/
	} var;
} BK_SEQ_DATA;


/*--------------------------------------------------------------------------------------------------*/
/*		DB����p�\���̒�`																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct DB_SEQ_DATA {
	LONG			DbStep;					/* DB Sequence Step										*/
	LONG			DbWait;					/* DB Sequence Wait Counter								*/
	BOOL			DbBeReq;				/* DbSequence BaseEnable Request						*/
	USHORT	        NegPumpState;			/* ���HOFF����IPM N���A�[��������	<S201>				   */
} DB_SEQ_DATA;


/*--------------------------------------------------------------------------------------------------*/
/*		�[���N�����v����p�\���̒�`																*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _ZC_CTRL_PRM {				/* Zero Clamp Parameter	<S00C>							*/
	LONG			ZcSpdLevel;				/* ZC Speed Level [2^24/OS]								*/
} ZC_CTRL_PRM;
typedef	struct ZC_CTRL_DATA {
	ZC_CTRL_PRM		P;						/* Zero Clamp Parameter									*/
/*--------------------------------------------------------------------------------------------------*/
	struct {								/* Zero Clamp Flags										*/
		UCHAR Reserve0;						/*														*/
		UCHAR Reserve1;						/*														*/
		UCHAR Reserve2;						/*														*/
		UCHAR Reserve3;						/*														*/
	} f;
	BOOL			ZcReqSignal;			/* ZC Sequence Req. Input Signal						*/
	BOOL			ZcZeroStop;				/* ZC Sequence Zero Stop Request						*/
	BOOL			ZcZeroClamp;			/* ZC Sequence Zero Clamp Request						*/
	LONG			ZcStep;					/* ZC Sequence Step										*/
	LONG			ZcWait;					/* ZC Sequence Wait Counter								*/
} ZC_CTRL_DATA;

/*--------------------------------------------------------------------------------------------------*/
/*		PUMP ON����p�\���̒�`																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct PUMP_CTR_STR {
	BOOL			PumpOnReq;				/* Charge Pump Control Request							*/
	BOOL			AdjPumponReq;			/* Adjust Mode PUMPON Request							*/
	BOOL			PumpOnComp;				/* Charge Pump Complete									*/
	SHORT			SvonPumpCnt;			/* Charge pump for before Base-Enable					*/
//	SHORT			SvonRelayCnt;			/* �˖h�����[ for �`���[�W�|���v�O�J�E���^				*/
	ULONG			TimerCnt;				/* Charge pump Timer Counter */
	UINT			PumpOnSeq;				/* Charge pump Sequence Number */
	USHORT          	NegPumpCnt;             /* Charge pump for before Power-On <S201>				*/
} PUMP_CTR_STR;

/*--------------------------------------------------------------------------------------------------*/
/*		�T�[�{�^�]�V�[�P���X�ϐ��f�[�^�|�C���^��`(�����ݒ�p)										*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct BE_SEQ_HNDL {
	ALM_STOP		AlmStop;				/* �A���[����~�p�f�[�^									*/
	FORCE_STOP		ForceStop;				/* ������~�p�f�[�^										*/   /* <S1F5> */
	BK_SEQ_DATA		BkSeqData;				/* BK�V�[�P���X�p�f�[�^									*/
	OT_SEQ_DATA		OtSeqData;				/* OT�V�[�P���X�p�f�[�^									*/
	DB_SEQ_DATA		DbSeqData;				/* DB�V�[�P���X�p�f�[�^									*/
	ZC_CTRL_DATA	ZcCtrlData;				/* �[���N�����v����p�f�[�^								*/
	PUMP_CTR_STR	ChargePump;				/* PUMP ON�p�f�[�^�ւ̃|�C���^							*/

	BOOL			SvonReq;				/* Servo ON Request(M-III/LCD OP����̗v��)				*/
	BOOL			SenStatus;				/* SEN Signal											*/
	BOOL			BeReqSeqOut;			/* BaseEnable Request(�V�[�P���X�����o��)				*/
	LONG			StopMode;				/* Stop Mode											*/
	BOOL			DbOn;					/* DB ON Flag											*/
	BOOL			SvonEnable;				/* ServoOn Enable										*/
	BOOL			SvReadyPhaseIgnore;		/* Servo Ready (Ignore Phase)							*/
	BOOL			ServoReady;				/* �T�[�{���f�B											*/
//	BOOL			EncReady;				/* Encoder Ready										*/
//	BOOL			ScanCEncDataNG;			/* �G���R�[�_�f�[�^�����t���O(0:�L��/1:����)			*/
	BOOL			SoftResetReq;			/* �\�t�g���Z�b�g�v��				<S076>			*/
	BOOL			LdstpTimeZero;			/* ����������~�̌�������(Pn30A)=0���̃t���O			*/     /* <S1F5> */
} BE_SEQ_HNDL;

/*--------------------------------------------------------------------------------------------------*/
/*		HWBB���o�p�ϐ���`  <S201> ��`�ꏊ��ύX							     		*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct DET_HWBB_STS {
	BOOL			HwbbState;				/* Hard-wired Baseblock Status							*//* <HWBB> */
	BOOL			Bbon1;					/* BBON1 Signal											*//* <HWBB> */
	BOOL			Bbon2;					/* BBON2 Signal											*//* <HWBB> */
	BOOL			Bbon1Edge;				/* BBON1 Signal Rising Edge								*//* <HWBB> */
	BOOL			Bbon2Edge;				/* BBON2 Signal Rising Edge								*//* <HWBB> */
	BOOL			GateDrvDefect;			/* Gate Drive Defect Status Flag						*//* <HWBB> */
	BOOL			LackOneBbon;			/* One BBON Signal Input Only							*//* <HWBB> */
	BOOL			hwbbsupport;			/* HWBB Support flag(PnE0D.bit9 = 1�ŗL��)				*//* <HWBB> */
} DET_HWBB_STS;

/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
void BeSeqMakeServoReadySignal( BE_SEQ_HNDL *BeSeq, BOOL ready_cond, LONG PhaseReady );
ALMID_T	BeSeqMakeServoOnRequest( BE_SEQ_HNDL *BeSeq, LONG CmdSeqBit,
       	                          CTRLMD CtrlMode, FUN_CMN_CONTROL *FnCmnCtrl, MENCV *MencV );
void BeSeqDBControlProcess( BOOL DbOn, LONG RlyDbWay, USHORT faultState, ASIC_HW_IF_STR *pAsicHwReg,
							BOOL MotorLessTestMode );	/* <S1B7> */
ULONG BeSeqControlModeManager( BE_SEQ_HNDL *BeSeq, FUN_CMN_CONTROL *FnCmnCtrl );

// void BeSeqInputAlmStopMode( ALM_STOP *AlmStop, LONG AlmStopMode,     /* <S1F5> */
//                            CTRLMD CtrlMode, BOOL PhaseReady, BOOL HwbbSts, BOOL MotStop );  /* <S1F5> */
void BeSeqInputAlmStopMode( BE_SEQ_HNDL *BeSeq, LONG AlmStopMode,     /* <S1F5> */
                            CTRLMD CtrlMode, BOOL PhaseReady, SEQ_CTRL_OUT *Cout );    /* <S1F5> */
void BeSeqForceStopMode( BE_SEQ_HNDL *BeSeq, CTRLMD CtrlMode, BOOL MotStop, BOOL FstpSts );    /* <S1F5> */
BOOL BeSeqMechaBrakeSequence( BE_SEQ_HNDL *BeSeq, CTRLMD CtrlMode,
    	                       LONG AbsMotSpd, BOOL HwbbStatus, USHORT faultState,
							   BOOL BeComplete, BOOL MotorLessTestMode );	/* <S1B7> */
// BOOL BeSeqOverTravelSequence( OT_SEQ_DATA *OverTravelSeq, LONG CtrlMode, BOOL MotStop, BOOL BeComplete );   /* <S1F5> */
BOOL BeSeqOverTravelSequence( OT_SEQ_DATA *OverTravelSeq, LONG CtrlMode, BOOL MotStop,      /* <S1F5> */
                                                          BOOL BeComplete, BOOL FstpDecTimeZero );     /* <S1F5> */
BOOL BeSeqDetectOTStatus( OT_SEQ_DATA *OverTravelSeq, BASE_CTRL_OUT *BaseCtrlOut, BOOL PotSig, BOOL NotSig );
LONG BeSeqStopModeManager( BE_SEQ_HNDL *BeSeq );
BOOL BeSeqDynamicBrakeSequence( BE_SEQ_HNDL *BeSeq, ASICS *SvAsicRegs, LONG MotStop, BOOL EncDataNG,
								BOOL MotorLessTestMode );	/* <S1B7> */
void BeSeqChargePumpControl( ASICS *SvAsicRegs, PUMP_CTR_STR *workptr, BOOL OCState,
								BOOL MotorLessTestMode,BOOL NegPumpReq );	/* <S1B7> *//*<S201>*/
void BeNegativePumpSequence( POWER_STATUS   *CoutK, ALMMAN_HANDLE *Alm,
						     DET_HWBB_STS   *DetHwbb,CHECK_MAIN_POWER *PowerChecker,
						     DB_SEQ_DATA   *SvSeqV,  SEQ_CTRL_OUT *CoutV1, ASIC_HW_IF_STR* AsicHwReg);/* <S201>*/



#endif /* __BASE_ENABLE_SEQUENCE_H_ */
/***************************************** end of file **********************************************/
