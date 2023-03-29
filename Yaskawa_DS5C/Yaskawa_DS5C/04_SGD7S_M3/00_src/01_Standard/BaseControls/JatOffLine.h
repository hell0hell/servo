#ifndef __JAT_OFFLINE_H 
#define __JAT_OFFLINE_H
/****************************************************************************
Description	: Jrate OffLine Function
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
T.kira  2011.03.01    created

*****************************************************************************/
#include "Basedef.h"
#include "ResVibCtrl.h"
#include "CtrlLoopHandler.h"

/****************************************************************************************************/
/*																									*/
/*	定数定義										            									*/
/*																									*/
/****************************************************************************************************/
//#define	C10POW7			10000000					/* 10^7											*/
//#define	C10POW9			1000000000					/* 10^9											*/
//#define	C2PAIE7			62831853					/* (2*PAI) * 10^7								*/
#define	JSHBITNUM		7							/* 積算トルクシフト数							*/

/*--------------------------------------------------------------------------------------------------*/
/*	同定シーケンス定数定義																			*/
/*--------------------------------------------------------------------------------------------------*/
#define	JATINITIAL		0x00						/* イナーシャ同定未実行							*/
#define	JATCWSTART		0x01						/* 正転→逆転イナーシャ同定実行					*/
#define	JATCCWSTART		0x02						/* 逆転→正転イナーシャ同定実行					*/
#define	JATSUMEND		0x04						/* トルク積算実行完了							*/
#define	JATEXEEND		0x08						/* イナーシャ同定実行完了						*/

/*--------------------------------------------------------------------------------------------------*/
/*	同定ステータス定数定義																			*/
/*--------------------------------------------------------------------------------------------------*/
#define	JATINITIAL		0x00						/* イナーシャ同定未実行							*/
#define	JATCWCOMP		0x01						/* 正転→逆転イナーシャ同定実行完了				*/
#define	JATCCWCOMP		0x02						/* 逆転→正転イナーシャ同定実行完了				*/
#define	JATCOMP			(JATCWCOMP|JATCCWCOMP)		/* イナーシャ同定実行完了						*/
#define	JATJRATECOMP	0x04						/* イナーシャ同定値更新完了						*/
#define	JATEXEERR		0x40						/* 実行中エラー									*/
#define	JATERR			0x80						/* エラーステータス								*/

/****************************************************************************************************/
/*																									*/
/*		Variables Definition																		*/
/*																									*/
/****************************************************************************************************/
typedef struct _PRMBK {
	ULONG	kp;							/* Ｋｐ：Pn102												*/
	ULONG	jrate;						/* イナーシャ比：Pn103										*/
	ULONG	vffgn;						/* 速度FFゲイン：Pn109										*/
} PRMBK;

typedef struct _JATP {
	LONG	mkv;						/* モデルＫｖ												*/
	LONG	mkvi;						/* モデルＫｖi												*/
	LONG	mklpf;						/* モデルＴｆ												*/
	LONG	mkj;						/* モデルイナーシャゲイン									*/
	LONG	mjrate;						/* モデルイナーシャ											*/
	LONG	jfil;						/* 同定値更新ローパスフィルタゲイン							*/
	LONG	jverrfil;					/* 同定エラー検出用速度偏差ローパスフィルタゲイン			*/
} JATP;

typedef struct _JATV {
	UCHAR	enable;						/* イナーシャ同定実行中フラグ	0:未実行  1:実行中			*/
	UCHAR	status;						/* イナーシャ同定ステータス									*/
	UCHAR	jarte_start;				/* イナーシャ同定シーケンス									*/
	USHORT	jrate;						/* イナーシャ同定値					[%]						*/
	LONG	mnfb;						/* モデルフィードバック速度			[2^24/OvrSpd]			*/
	LONG	mnfbwrk[2];					/* 															*/
	LONG	mvref;						/* モデル速度指令					[2^24/OvrSpd]			*/
//	LONG	mner;						/* モデル速度偏差											*/
	LONG	mnerp;						/* モデル速度偏差(比例)										*/
	LONG	mneri;						/* モデル速度偏差(積分)										*/
	LONG	mtrf;						/* モデルトルク						[2^24/MaxTrq]			*/
	LONG	mneribf[2];					/*															*/
	LONG	jverr;						/* 同定エラー用速度偏差				[2^24/OvrSpd]			*/
	LONG	sum1;						/* 正転→逆転実トルク指令積算値								*/
	LONG	msum1;						/* 正転→逆転モデルトルク指令積算値							*/
	LONG	sum2;						/* 逆転→正転実トルク指令積算値								*/
	LONG	msum2;						/* 逆転→正転モデルトルク指令積算値							*/
	LONG	j0;							/* 正転→逆転総イナーシャ同定値								*/
	LONG	j1;							/* 総イナーシャ同定値										*/
	LONG	j2;							/* 逆転→正転総イナーシャ同定値								*/
	LONG	TargetDist;					/* トルク積算開始距離（同定移動距離×1/2）		[指令単位]	*/
	LONG	fbsum;						/* ｲﾅｰｼｬ同定中FB位置(ｲﾅｰｼｬ開始位置からの絶対値)	[指令単位]	*/
	LONG	fbsumrem;					/* 															*/
	BOOL	JatPrmSetReq;				/*															*/
} JATV;
/*--------------------------------------------------------------------------------------------------*/
typedef struct _JATHNDL{
	JATP	jatp;							/* イナーシャ同定関連パラメータ							*/
	JATP	jatp_wk;						/* イナーシャ同定関連パラメータ(パラメータ保持用)		*/
	JATV	jatv;							/* イナーシャ同定関連変数								*/
	PRMBK	prmbk;							/* 関連パラメータバックアップ値							*/
	USHORT	EstimatJrat;					/* 負荷イナーシャ比同定値			[%]					*/
}JATHNDL;

/****************************************************************************************************/
/*																									*/
/*		Function Prototype Definition																*/
/*																									*/
/****************************************************************************************************/
void BpxJatOfflineCalc( JATHNDL	*jathdl, EGEAR *Egear, CTRL_CMD_PRM *CtrlCmdPrm, 
														CTRL_CMD_MNG *CtrlCmdMngr, LONG	TrqRefo_a );
void CpxJatOfflineCalc( JATHNDL *JatHdl, LONG UnMotSpdx, DETVIB	*DetVib, 
														BOOL PConActFlag, BOOL PerrClrReq );

#endif //__JAT_OFFLINE_H 
/***************************************** end of file **********************************************/
