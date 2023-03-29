/****************************************************************************************************/
/*																									*/
/*																									*/
/*		CheckMotSts.h : モータ状態監視処理定義														*/
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
#ifndef CHECK_MOT_STS_H
#define CHECK_MOT_STS_H

#include "Bprm.h"
#include "BaseCmnStruct.h"


/****************************************************************************************************/
/*		DEFINES																						*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		モータ回転検出用定義																		*/
/*--------------------------------------------------------------------------------------------------*/
#define MOTSPD_TGMGN	12710		/* 12710 / 16777216 x 6600 ≒ 5rpm		LpxCheckMotorRun( )参照	*/
#define MOTSPD_DBLVL	60000		/* 16777216 x 0.7/175 = 67108 -> 60000	LpxCheckMotorStop( )参照*/

/*--------------------------------------------------------------------------------------------------*/
/*		ScanCモータステータス構造体定義																*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct SEQ_MOT_STS {
	struct {								/* モータステータス定数定義								*/
		LONG			VcmpSpdLevel;			/* 速度一致検出幅				[2^24/OvrSpd]		*/
		LONG			TgonSpdLevel;			/* モータ回転検出レベル			[2^24/OvrSpd]		*/
	} conf;
	struct {								/* モータステータス変数定義								*/
//		BOOL			TgOn;					/* TGON Status										*/
//		BOOL			Vcmp;					/* VCMP Status										*/
//		BOOL			MotStop;				/* Motor Stop Status								*/
		LONG			MotSpd;					/* Motor Speed										*/
		LONG			AbsMotSpd;				/* Absolute Motor Speed								*/
		LONG			LstMotSpd;				/* Last Motor Speed for ModeSwAcc Cal.				*/
		LONG			MotPos;					/* Motor Position									*/
		LONG			dMotPos;				/* Delta Motor Position								*/
		LONG			PcmdCntr;				/* Pulse Command Counter	[pulse]					*/
		LONG			dPcmdCnt;				/* Delta Pulse Command		[pulse]					*/

		LONG			UnMotSpd;				/* Un000  : モータ速度				[pulse/64ms]	*/
		LONG			UnPfbk64ms;				/* Un000  : モータ速度				[pulse/64ms]	*/
		LONG			UnPcmd64ms;				/* Un007  : 指令パルス速度			[pulse/64ms]	*/
		LONG			Pfbk64SumCnt;			/* モニタ用モータ速度演算用							*/
		LONG			Pcmd64Sum;				/* モニタ用指令パルス速度演算用						*/
		LONG			Pfbk64Sum;				/* モニタ用モータ速度演算用							*/
		LONG			MotSpd64Sum;			/* モニタ用モータ速度演算用							*/

		LONG			IsqrSum;				/* モニタ用電流負荷演算用							*/
		LONG			IsqrSumCnt;				/* モニタ用電流負荷演算用							*/
		LONG			UnIsqrSum;				/* Un009  : 累積負荷率		[(15000/MaxTrq)/10sec]	*/
		ULONG			UnCrntRef;				/* Un142 : ２乗電流指令値	[(15000/MAXTRQ)^2]		*/
		ULONG			UnIsqrSumMax;			/* Un145 : 累積負荷率最大値	[(15000/MaxTrq)/10sec]	*/

		LONG			TrqRefMon;				/* トルク指令モニタ					[2^24/MaxTrq]	*/

		LONG			UnPcmd64Sum;			/* 指令パルス速度 <S1B6>			[64ms]			*/
	} var;
} SEQ_MOT_STS;



/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
void SMotSts_InputMotorStatus( SEQ_MOT_STS *MotSts, BASE_CTRL_OUT *BaseCtrlOut,
                               SEQ_CTRL_OUT *SeqCtrlOut, BPRMDAT *Bprm );
BOOL	LpxCheckMotorStop( LONG AbsMotSpd );
BOOL	LpxCheckMotorVcmp( SEQ_MOT_STS *MotSts, LONG SpdRef, BOOL SpdCtrlMode );
BOOL	LpxCheckMotorRun( SEQ_MOT_STS *MotSts, LONG LstTgonSts );
BOOL    LpxCheckMotorSuspendLevel( SEQ_MOT_STS *MotSts,LONG NorSuspSpd);/*<S201>*/


#endif /* CHECK_MOT_STS_H */
/***************************************** end of file **********************************************/
