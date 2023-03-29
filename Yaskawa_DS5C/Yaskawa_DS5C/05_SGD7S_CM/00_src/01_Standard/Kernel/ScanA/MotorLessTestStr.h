/****************************************************************************
Description	: Motor Less Test Mode
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2013 All Rights Reserved

Project       : Mercury

Functions	  : none

----------------------------------------------------------------------------
Changes		:
Name		Date			Description
------------------------------------------------------------------------
K.Sakamoto	2013.11.11		created <S00X>

****************************************************************************/
#ifndef MOTORLESSTESTSTR_H__
#define MOTORLESSTESTSTR_H__

/*==========================================================================*/
/* I N C L U D E															*/
/*==========================================================================*/
#include "Basedef.h"
#include "PnPrmStruct.h"
#include "EncData.h"

/*==========================================================================*/
/* D E F I N E																*/
/*==========================================================================*/

/*==========================================================================*/
/* S T R U C T																*/
/*==========================================================================*/
/*****************************************************************************
*	モータレステストモード用変数定義
*****************************************************************************/
/* 内部メカモデル用パラメータ定義	*/
typedef	struct MOTLESSMDLP {
	LONG	MdlKj;					/* モデルイナーシャゲイン									*/
	LONG	MdlKvp;					/* 速度[2^24/OvrSpd]→位置差分[pulse/Scan](制御)変換係数	*/
	LONG	MdlKvpM;				/* 速度[2^24/OvrSpd]→位置差分[pulse/Scan](モータ)変換係数	*/
} MOTLESSMDLP;

/* 内部メカモデル用パラメータ＆変数定義	*/
typedef	struct MOTLESSMDL {
	MOTLESSMDLP	P[2];				/* モータレステストモード用パラメータ				*/
	MOTLESSMDLP	*PrmPtr;			/* 有効なパラメータへのポインタ						*/

	struct {						/* モータレステストモード用変数データ定義	*/
		LONG	MotMdlSpdFbk;			/* モデル速度ＦＢ				[2^24/OvrSpd]	*/
		LONG	MotMdldPosFbk;			/* モータモデル位置ＦＢ差分		[pluse/scan]	*/
		LONG	MotMdlPosFbk;			/* モータモデル位置ＦＢ			[pluse]			*/
		LONG	MechaMdldPosFbk;		/* モデル位置ＦＢ差分			[pluse/scan]	*/
		LONG	MechaMdlPosFbk;			/* モデル位置ＦＢ				[pluse]			*/
		LONG	Mvar;					/* モデル速度積分演算用							*/
		LONG	MotMdlSfbrem;			/* モデル速度ＦＢあまり							*/
		LONG	MotMdlpfbrem;			/* モデル位置ＦＢあまり							*/
		LONG	MotMdlpfbremM;			/* モデルモータ位置ＦＢあまり					*/
	} V;
} MOTLESSMDL;

/*****************************************************************************
*	モータレステストモードデータ定義
*****************************************************************************/
/* エンコーダデータ	*/
typedef	union ENCODER_DATA {
	LONG		Long[2];			/* 位置データ(32bit×2=64bit)定義		*/
	struct {						/* 位置データエンコーダフォーマット定義	*/
		DWORDX	RxPosL;					/* エンコーダフォーマットデータ(Low)  32bit	*/
		SHORT	RxPosH;					/* エンコーダフォーマットデータ(High) 16bit	*/
		SHORT	Dummy;					/* ダミーデータ						  16bit	*/
	} EncFormat;
} ENCODER_DATA;

/* エンコーダIF */
typedef	struct MOTOR_LESS {
	BOOL			SemiEncConnect;	/* Semi-Closed Encoder Connected		*/
	BOOL			FullEncConnect;	/* Full-Closed Encoder Connected		*/
	BOOL			FencType;		/* Full-Closed Encoder Type				*/
	LONG			dSemiModePos;	/* セミ位置差分データ					*/
	LONG			dFullModePos;	/* フル位置差分データ					*/
	ENCODER_DATA	SencData;		/* セミエンコーダデータ					*/
	ENCODER_DATA	FencData;		/* フルエンコーダデータ					*/
} MOTOR_LESS;

/*****************************************************************************
*	モータレステストモードIF定義
*****************************************************************************/
typedef struct MOTLESSTST {
	BOOL		MotorLessTestMode;	/* モータレステストモードフラグ			*/

	/* APIの引数で指定が困難なデータへのポインタ(初期化時に設定する)	*/
	MENCV		*MencV;				/* モータエンコーダデータへのポインタ	*/
	MENCV		*FencV;				/* フルクエンコーダデータへのポインタ	*/
	PRMDATA		*Prm;				/* パラメータデータへのポインタ			*/

	/* モータレステストモード用ワーク	*/
	MOTLESSMDL	MotorLessMdl;		/* 内部メカモデル用データ				*/
	MOTOR_LESS	MotorLessTestDatas;	/* エンコーダIF用データ					*/
} MOTLESSTST;

/*==========================================================================*/
/* E X T E R N   V A R I A B L E											*/
/*==========================================================================*/

/*==========================================================================*/
/* P R O T O T Y P E														*/
/*==========================================================================*/

#endif // MOTORLESSTESTSTR_H__
/*-------end of file----------------------------------------------------------------------------------------*/
