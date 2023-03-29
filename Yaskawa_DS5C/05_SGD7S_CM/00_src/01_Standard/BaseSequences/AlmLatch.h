/*****************************************************************************
Description		: アラームラッチ機能ヘッドフィール
-----------------------------------------------------------------------------
Author		: SHCORE
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
K.Liu		2013.10.11		created

*****************************************************************************/

#ifndef ALARM_LATCH_API_H
#define ALARM_LATCH_API_H

#include "BaseControls.h"
#include "SequenceIo.h"
#include "CheckMotSts.h"
#include "PowerManager.h"
#include "Pls.h"
#include "SvOptCardIf.h"
#include "UnitPrmStruct.h"					/* <S212> */

#define EEPSIZE_ALMMON		64

/*--------------------------------------------------------------------------------------------------*/
/*	アラームラッチモニタ																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct _ALMLTMON
{
	/*----------------------------------------------------------------------------------------------*/
		SHORT	MotSpd;					/* モータ位置差分(2ms)				[min-1/mm/s]			*/
		SHORT	SpdRef;					/* 速度指令							[min-1/mm/s]			*/
		SHORT	TrqRef;					/* トルク指令						[%]						*/
		SHORT	PcmdSpd;				/* 位置指令差分(2ms)				[min-1/mm/s]			*/
		LONG	PosErr;					/* 位置偏差出力(四捨五入)									*/
	/*----------------------------------------------------------------------------------------------*/
		ULONG	InterSignal;			/* 内部信号(モニタ用)										*/
		ULONG	InSignal;				/* 入力信号(モニタ用)										*/
		ULONG	OutSignal;				/* 出力信号(モニタ用)										*/
	/*----------------------------------------------------------------------------------------------*/
		SHORT	DcVoltage;				/* 主回路母線電圧					[V]						*/
		SHORT	IFBMon;					/* 電流ＦＢモニタ					[15000/Imax]			*/
		USHORT	MotLoad;				/* 累積負荷率 						[%]						*/
		USHORT	RgPower;				/* 回生消費電力						[%]						*/
		USHORT	DbPower;				/* ＤＢ消費電力 					[%]						*/
		USHORT	MaxMotLoad10s;			/* 最大累積負荷率 					[%（周期10s）]			*/
		USHORT	JLRatio;				/* 慣性モーメント比/重量比			[%]						*/
	/*----------------------------------------------------------------------------------------------*/
		USHORT	EncComErrCnt;			/* シリアルエンコーダ通信異常回数							*/
		USHORT	M2ComErrCnt;			/* MECHATROLINK通信異常回数									*/
	/*----------------------------------------------------------------------------------------------*/
		USHORT	SafetyIOSignal;			/* セーフティ入出力信号(モニタ用) <V066>					*/
	/*----------------------------------------------------------------------------------------------*/
		USHORT	AlmCode;				/* アラームコード											*/
		USHORT	spare1;					/* 予備														*/
		ULONG	TmStamp;				/* アラームタイムスタンプ									*/
	/*----------------------------------------------------------------------------------------------*/
		USHORT	spare2[6];				/* 予備														*/
	/*----------------------------------------------------------------------------------------------*/
} ALMLTMON;

/*--------------------------------------------------------------------------------------------------*/
/*	アラームラッチ用データ定義																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct _ALMLATCH_HNDL
{
	struct {
		BOOL	LastAlmStatus;					/* 前回アラームステータス							*/
	} var;
	/*----------------------------------------------------------------------------------------------*/
	ALMLTMON			AlmLtMon;			/* アラームラッチモニタ(更新)						*/
	ALMLTMON			LatchMon;			/* アラームラッチモニタ(アラーム発生時更新)			*/
	ANLG_UN_MON_DATA	*UnMonitor;			/* Amon/Unモニタデータへのポインタ						*/
	SEQ_MOT_STS			*SeqMotSts;			/* ScanCにて取得・作成するモータ状態					*/
	BPRMDAT				*Bprm;				/* 基本パラメータ定義									*/
	BASE_LOOPCTRLS		*BaseLoops;			/* 制御ループ演算用データ								*/
	POW_MNG_HNDL		*PowerManager;		/* 電源管理用データ(ユニット共通)						*/
	SEQ_CTRL_OUT		*SeqCtrlOut;		/* ScanC出力データ										*/
	MENCV				*FencV;				/* フルクエンコーダデータ								*/
	MENCV				*MencV;				/* モータエンコーダデータへのポインタ					*/
	BE_SEQ_HNDL			*BeSequence;		/* Base Enable Sequence Handle							*/
	BASE_CTRL_HNDL		*BaseControls;		/* 基本制御用共有データ									*/
	BASE_CTRL_OUT		*BaseCtrlOut;		/* ScanB出力データ										*/
	ALMMAN_HANDLE		*AlmManager;		/* アラーム制御用構造体									*/
	SEQ_IO_HNDL			*IoSignals;			/* シーケンスI/O 										*/
	SERVO_CONTROL_IF	*SvControlIf;		/* M3 I/Fデータ											*/
	PLSIFV 				*pPlsIfV;			/* <S00C> Command Pulse Interface						*/
	OPSFT_HNDL			*SftOpt;			/* セーフティーモジュールデータへのポインタ		*/
	UNI_PRM_STR			*UniPrm;			/* パラメータハンドルへのポインタ				 <S212> */

} ALMLATCH_HNDL;





/****************************************************************************************************/
/*																									*/
/*		関数一覧																					*/
/*																									*/
/****************************************************************************************************/
extern void AlmLatchMonitorInitial( ALMLATCH_HNDL *hAlmLatchHndl );
extern void AlmLatchMonitorSequence( ALMLATCH_HNDL *hAlmLatchHndl );




#endif	/* ALARM_LATCH_API_H */

/***************************************** end of file **********************************************/

