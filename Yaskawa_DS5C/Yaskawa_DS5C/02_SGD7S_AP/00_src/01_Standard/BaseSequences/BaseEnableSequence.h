/****************************************************************************************************/
/*																									*/
/*																									*/
/*		ServoSequence.h : サーボシーケンス管理処理定義												*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	初版	2010.02.23	Y.Oka	For INGRAM												*/
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
/*		ストップモード定義																			*/
/*--------------------------------------------------------------------------------------------------*/
/*		注意：ストップモードを定義する場合は、0x0E以下の値で定義すること(アラーム定義との関係)。	*/
/*			  ストップモードは定義値の大きい方が優先度は高い。										*/
/*--------------------------------------------------------------------------------------------------*/
#define	STOPMODE_NOTSTOP		0x00		/* 非停止(何もしない)									*/
#define	STOPMODE_ZEROSTOP		0x01		/* ゼロ速停止											*/
#define	STOPMODE_LDECSTOP		0x02		/* 直線減速度による減速停止								*/   /* <S1F5> */
/*--------------------------------------------------------------------------------------------------*/
// #define	STOPMODE_DBSTOPDB		0x04		/* ＤＢでモータ停止＆ＤＢ継続(Pn001.0==0)				*/  /* <S1F5> */
// #define	STOPMODE_DBSTOPFR		0x05		/* ＤＢでモータ停止＆ＤＢ解除(Pn001.0==1)				*/    /* <S1F5> */
// #define	STOPMODE_FREERUN		0x06		/* ＤＢを使わず、フリーラン停止(Pn001.0==2)				*/    /* <S1F5> */
#define	STOPMODE_DBSTOPDB		0x08		/* ＤＢでモータ停止＆ＤＢ継続(Pn001.0==0)				*/   /* <S1F5> */
#define	STOPMODE_DBSTOPFR		0x09		/* ＤＢでモータ停止＆ＤＢ解除(Pn001.0==1)				*/   /* <S1F5> */
#define	STOPMODE_FREERUN		0x10		/* ＤＢを使わず、フリーラン停止(Pn001.0==2)				*/   /* <S1F5> */
/*--------------------------------------------------------------------------------------------------*/
/* <S1F5> Start */
/*		Gr.2アラームストップモード定義																*/
/*--------------------------------------------------------------------------------------------------*/
#define	GR2STOPMODE_DEFAULT		0x03		/* Pn001.0と同じ(Pn00A.0==0)							*/
#define	GR2STOPMODE_ZSTOPDB		0x04		/* 減速停止＆DB停止状態(Pn00A.0==1)						*/
#define	GR2STOPMODE_ZSTOPFR		0x05		/* 減速停止＆フリーラン状態(Pn00A.0==2)					*/
#define	GR2STOPMODE_LDSTOPDB	0x06		/* 直線減速停止＆DB停止状態(Pn00A.0==3)					*/
#define	GR2STOPMODE_LDSTOPFR	0x07		/* 直線減速停止＆フリーラン状態(Pn00A.0==4)				*/
/*--------------------------------------------------------------------------------------------------*/
/*		Gr2/DB停止モード定義																		*/
/*--------------------------------------------------------------------------------------------------*/
#define	GR2DBOP_NONE			0x00		/* ＤＢ制限要求なし										*/
#define	GR2DBOP_FREE			0x02		/* フリーラン停止要求あり								*/
/*--------------------------------------------------------------------------------------------------*/
/*		FSTPストップモード定義																		*/
/*--------------------------------------------------------------------------------------------------*/
#define	FSTOPMODE_DEFAULT		0x03		/* Pn001.0と同じ(Pn00A.1==0)							*/
#define	FSTOPMODE_ZSTOPDB		0x04		/* 減速停止＆DB停止状態(Pn00A.1==1)						*/
#define	FSTOPMODE_ZSTOPFR		0x05		/* 減速停止＆フリーラン状態(Pn00A.1==2)					*/
#define	FSTOPMODE_LDSTOPDB		0x06		/* 直線減速停止＆DB停止状態(Pn00A.1==3)					*/
#define	FSTOPMODE_LDSTOPFR		0x07		/* 直線減速停止＆フリーラン状態(Pn00A.1==4)				*/
/*--------------------------------------------------------------------------------------------------*/
/*		FSTP/DB停止モード定義																		*/
/*--------------------------------------------------------------------------------------------------*/
#define	FSTPDBOP_NONE			0x00		/* ＤＢ制限要求なし										*/
#define	FSTPDBOP_FREE			0x02		/* フリーラン停止要求あり								*/
/*--------------------------------------------------------------------------------------------------*/
/* <S1F5> End */
/*		OT時ストップモード定義																		*/
/*--------------------------------------------------------------------------------------------------*/
#define	OTSTOPMODE_DEFAULT		0x00		/* Pn001.0と同じ										*/
#define	OTSTOPMODE_ZSTOPZC		0x01		/* 減速停止＆サーボロック								*/
#define	OTSTOPMODE_ZSTOPFR		0x02		/* 減速停止＆フリーラン									*/
#define	OTSTOPMODE_LDSTOPZC		0x03		/* 直線減速停止＆サーボロック							*/      /* <S1F5> */
#define	OTSTOPMODE_LDSTOPFR		0x04		/* 直線減速停止＆フリーラン								*/     /* <S1F5> */
/*--------------------------------------------------------------------------------------------------*/
/*		OT/DB停止モード定義																			*/
/*--------------------------------------------------------------------------------------------------*/
#define	OTDBOP_NONE				0x00		/* ＤＢ制限要求なし										*/
#define	OTDBOP_DEF				0x01		/* ＯＴ標準停止											*/
#define	OTDBOP_FREE				0x02		/* フリーラン停止要求あり								*/
/*--------------------------------------------------------------------------------------------------*/
/*		OTからのDB制御要求種別																		*/
/*--------------------------------------------------------------------------------------------------*/
//#define	OTDBOP_NONE				0x00		/* ＤＢ制限要求なし										*/
//#define	OTDBOP_DEF				0x01		/* ＯＴ標準停止											*/
//#define	OTDBOP_FREE				0x02		/* フリーラン停止要求あり								*/


/****************************************************************************************************/
/*		STRUCT DEFINITION																			*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		アラーム停止制御用構造体定義																*/
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
//		BOOL		G2AlmStopMode;			/* G2アラームの停止方法									*/   /* <S1F5> */
		LONG		G2AlmStopMode;			/* G2アラームの停止方法									*/   /* <S1F5> */
		LONG		StopMode;				/* サーボオフ及びアラーム発生時の停止モード				*/
	} conf;
} ALM_STOP;

/* <S1F5> Start */
/*--------------------------------------------------------------------------------------------------*/
/*		強制停止制御用構造体定義																	*/
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
		LONG		ForceStopMode;			/* 強制停止の停止方法									*/
	} conf;
} FORCE_STOP;
/* <S1F5> End */

/*--------------------------------------------------------------------------------------------------*/
/*		OT制御用構造体定義																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct OT_SEQ_DATA {
	struct {								/* OTシーケンス用定数定義								*/
		BOOL			OtWarningSw;			/* OTワーニング検出選択								*/
		LONG			OtStopMode;				/* オーバトラベル(OT)時の停止方法					*/
	} conf;
	struct {								/* OTシーケンス用変数定義								*/
		BOOL			OverTrvlSts;			/* OT状態(OTシーケンス処理出力)						*/
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
/*		ブレーキ制御用構造体定義																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct BK_SEQ_DATA {
	struct {								/* ブレーキ制御用定数定義								*/
		LONG			BkonSpdLevel;			/* ブレーキ指令出力速度レベル(Pn507)	[2^24/OS]	*/
		LONG			BkonWaitTime;			/* サーボオフブレーキ指令待ち時間(Pn508)	 [ms]	*/
		LONG			BkonDelayTime;			/* ブレーキ指令サーボオフ遅れ時間(Pn506)	 [ms]	*/
	} conf;
	struct {								/* ブレーキ制御用変数定義								*/
		BOOL			BkBeReq;				/* BkSequence BaseEnable Request						*/
		BOOL			Brake;					/* BK Sequence Brake Request						*/
		BOOL			BkZeroStop;				/* BK Sequence Zero Stop  Request					*/
		BOOL			BkLinDecStop;			/* BK Sequence Linearity Deceleration Stop Request	*/	/* <S1F5> */
		LONG			BkStep;					/* BK Sequence Step									*/
		LONG			BkWait;					/* BK Sequence Wait Counter							*/
	} var;
} BK_SEQ_DATA;


/*--------------------------------------------------------------------------------------------------*/
/*		DB制御用構造体定義																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct DB_SEQ_DATA {
	LONG			DbStep;					/* DB Sequence Step										*/
	LONG			DbWait;					/* DB Sequence Wait Counter								*/
	BOOL			DbBeReq;				/* DbSequence BaseEnable Request						*/
	USHORT	        NegPumpState;			/* 主回路OFF時のIPM N側アーム制御状態	<S201>				   */
} DB_SEQ_DATA;


/*--------------------------------------------------------------------------------------------------*/
/*		ゼロクランプ制御用構造体定義																*/
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
/*		PUMP ON制御用構造体定義																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct PUMP_CTR_STR {
	BOOL			PumpOnReq;				/* Charge Pump Control Request							*/
	BOOL			AdjPumponReq;			/* Adjust Mode PUMPON Request							*/
	BOOL			PumpOnComp;				/* Charge Pump Complete									*/
	SHORT			SvonPumpCnt;			/* Charge pump for before Base-Enable					*/
//	SHORT			SvonRelayCnt;			/* 突防リレー for チャージポンプ前カウンタ				*/
	ULONG			TimerCnt;				/* Charge pump Timer Counter */
	UINT			PumpOnSeq;				/* Charge pump Sequence Number */
	USHORT          	NegPumpCnt;             /* Charge pump for before Power-On <S201>				*/
} PUMP_CTR_STR;

/*--------------------------------------------------------------------------------------------------*/
/*		サーボ運転シーケンス変数データポインタ定義(引数設定用)										*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct BE_SEQ_HNDL {
	ALM_STOP		AlmStop;				/* アラーム停止用データ									*/
	FORCE_STOP		ForceStop;				/* 強制停止用データ										*/   /* <S1F5> */
	BK_SEQ_DATA		BkSeqData;				/* BKシーケンス用データ									*/
	OT_SEQ_DATA		OtSeqData;				/* OTシーケンス用データ									*/
	DB_SEQ_DATA		DbSeqData;				/* DBシーケンス用データ									*/
	ZC_CTRL_DATA	ZcCtrlData;				/* ゼロクランプ制御用データ								*/
	PUMP_CTR_STR	ChargePump;				/* PUMP ON用データへのポインタ							*/

	BOOL			SvonReq;				/* Servo ON Request(M-III/LCD OPからの要求)				*/
	BOOL			SenStatus;				/* SEN Signal											*/
	BOOL			BeReqSeqOut;			/* BaseEnable Request(シーケンス処理出力)				*/
	LONG			StopMode;				/* Stop Mode											*/
	BOOL			DbOn;					/* DB ON Flag											*/
	BOOL			SvonEnable;				/* ServoOn Enable										*/
	BOOL			SvReadyPhaseIgnore;		/* Servo Ready (Ignore Phase)							*/
	BOOL			ServoReady;				/* サーボレディ											*/
//	BOOL			EncReady;				/* Encoder Ready										*/
//	BOOL			ScanCEncDataNG;			/* エンコーダデータ無効フラグ(0:有効/1:無効)			*/
	BOOL			SoftResetReq;			/* ソフトリセット要求				<S076>			*/
	BOOL			LdstpTimeZero;			/* 直線減速停止の減速時間(Pn30A)=0時のフラグ			*/     /* <S1F5> */
} BE_SEQ_HNDL;

/*--------------------------------------------------------------------------------------------------*/
/*		HWBB検出用変数定義  <S201> 定義場所を変更							     		*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct DET_HWBB_STS {
	BOOL			HwbbState;				/* Hard-wired Baseblock Status							*//* <HWBB> */
	BOOL			Bbon1;					/* BBON1 Signal											*//* <HWBB> */
	BOOL			Bbon2;					/* BBON2 Signal											*//* <HWBB> */
	BOOL			Bbon1Edge;				/* BBON1 Signal Rising Edge								*//* <HWBB> */
	BOOL			Bbon2Edge;				/* BBON2 Signal Rising Edge								*//* <HWBB> */
	BOOL			GateDrvDefect;			/* Gate Drive Defect Status Flag						*//* <HWBB> */
	BOOL			LackOneBbon;			/* One BBON Signal Input Only							*//* <HWBB> */
	BOOL			hwbbsupport;			/* HWBB Support flag(PnE0D.bit9 = 1で有効)				*//* <HWBB> */
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
