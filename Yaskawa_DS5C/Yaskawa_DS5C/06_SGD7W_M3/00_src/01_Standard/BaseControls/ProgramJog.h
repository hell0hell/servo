#ifndef _PRGRAM_JOG_H
#define _PRGRAM_JOG_H
/****************************************************************************
Description	: Program Jog
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
T.Kira  2011.03.08    created

*****************************************************************************/
#include "Basedef.h"
#include "CtrlLoopHandler.h"
#include "BaseCmnStruct.h"
#include "AlarmManager.h"
#include "KLib.h"
#include "MLib.h"

/************************************************************************************************/
/*																								*/
/*		プログラムJOG運転用データ定義															*/
/*																								*/
/************************************************************************************************/
/*----------------------------------------------------------------------------------------------*/
/*		プログラムJOG運転データ構造体															*/
/*----------------------------------------------------------------------------------------------*/
typedef	struct PJOGV{
	struct{							/* プログラムJOG使用定数データ								*/
		LONG	Distance;			/* プログラムJOG運転目標位置		[指令単位]				*/
		LONG	MaxSpd;				/* プログラムJOG運転最高速度		[2^24/OvrSpd]			*/
		LONG	AccTime;			/* プログラムJOG運転加減速時間		[us]					*/
		USHORT	WaitTime;			/* 待ち時間							[ms]					*/
		USHORT	MTimes;				/* 移動回数													*/
		CHAR	Pattern;			/* プログラムJOG運転指令パターン							*/
		CHAR	RevDir;				/* プログラムJOG運転モード開始方向							*/
		CHAR	PrmUnMatch;			/* パラメータ組合わせ異常状態								*/
		CHAR	PrmUnMatch2;		/* パラメータ組合わせ異常状態2								*/
	} Cnst;
/*----------------------------------------------------------------------------------------------*/
	struct{							/* プログラムJOG使用変数データ								*/
		LONG	AbsPosCmd;			/* プログラムJOG位置指令			[指令単位	]			*/
		ULONG	TimeWork;			/* 待ち時間計測用データ										*/
		USHORT	DispTime;			/* パネオペ表示用データ	<V068>								*/
		USHORT	MCntr;				/* 繰り返し回数												*/
		USHORT	SeqPtr;				/* プログラムJOG運転指令作成シーケンスポインタ				*/
		UCHAR	RoutCmd;			/* プログラムJOG運転指令	(Round  -> Scan C)				*/
		UCHAR	CoutCmd;			/* プログラムJOG運転指令	(Scan C -> Scan B)				*/
		UCHAR	State;				/* プログラムJOG運転状態									*/
		BOOL	ScanCRunFlg;		/* ScanCでON,RoundでOFFするフラグ(割り込み代用)				*/
	} Var;
} PJOGV;

/*----------------------------------------------------------------------------------------------*/
/*		プログラムJOG構造体																		*/
/*----------------------------------------------------------------------------------------------*/
typedef	struct PJOGHNDL{
	PJOGV   PJogV;
	PCMKPV	PJogPcmk;
	SHORT	PJogState;
}PJOGHNDL;

/*----------------------------------------------------------------------------------------------*/
/*		プログラムJOG運転ステータス定義															*/
/*----------------------------------------------------------------------------------------------*/
#define	PJOG_INIT	0x00				/* イニシャル状態										*/
#define	PJOG_ABORT	0x10				/* 中断状態												*/
#define	PJOG_START	0x01				/* 運転状態												*/
#define	PJOG_AFTER	0x11				/* 運転後処理状態										*/
#define	PJOG_END	0x02				/* 終了状態												*/
#define	PJOG_ERROR	0x03				/* 異常終了												*/
/*----------------------------------------------------------------------------------------------*/
/*		プログラムJOG運転コマンド定義															*/
/*----------------------------------------------------------------------------------------------*/
#define	PJOGCMD_NONE	0x00			/* コマンドなし											*/
#define	PJOGCMD_INIT	0x01			/* イニシャライズ										*/
#define	PJOGCMD_ABORT	0x02			/* 中断													*/
#define	PJOGCMD_START	0x03			/* 運転													*/

#define		LMTLOWSPEED	10
/*----------------------------------------------------------------------------------------------*/
/*		プログラムJOG運転関連関数定義															*/
/*----------------------------------------------------------------------------------------------*/
void PrgJogReset( PJOGHNDL *PJogHdl );
void IprmcalPrgJog( PJOGHNDL *PJogHdl, BPRMDAT *Bprm, POS_MNG_HNDL *PosManager );
void CpxPrgJogPosSequence( PJOGHNDL *PJogHdl, SEQ_CTRL_OUT *SeqCtrlOut, 
												ALMMAN_HANDLE *AlmManager, LONG FnCtrlMcmd ); 
BOOL PrgJogMakePositionReference( PJOGHNDL *PJogHdl, LONG *dPosRefo );

#endif //_PRGRAM_JOG_H 
/***************************************** end of file ******************************************/
