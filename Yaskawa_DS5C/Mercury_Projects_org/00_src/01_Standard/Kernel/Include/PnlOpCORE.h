/*****************************************************************************
Description		: データトレース定義 ローカルヘッダファイル
-----------------------------------------------------------------------------
Author		: SHCORE
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
K.Liu		2013.05.20		created

*****************************************************************************/
#ifndef _PNLOP_H
#define _PNLOP_H

#include "Basedef.h"


/******************************************************************************/
/*		STRUCT DEFINITION													  */
/******************************************************************************/
typedef	struct _OPKEYFLG
{
	UINT	mode		:1;
	UINT	up			:1;
	UINT	down		:1;
	UINT	shup		:1;
	UINT	shdown		:1;
	UINT	data		:1;
	UINT	svon		:1;
	UINT	almrst		:1;
	UINT	scroll		:1;
	UINT	read		:1;
	UINT	write		:1;
	UINT	rst			:1;	/* up+down同時押下	*/
}	OPKEYFLG;

typedef	struct _BLINKFLG
{
	UINT	PrmEewrite	:1;
	UINT	read		:1;
	UINT	write		:1;
	UINT	no_op		:1;
	UINT	error		:1;
	UINT	done		:1;
	UINT	end			:1;
}	BLINKFLG;

/****************************************************************************************************/
/*																									*/
/*		モニタ定義構造体																			*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Ｕｎモニタ定義構造体																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct _MONDEF
{
	UINT	IsLong     : 1;						/* モニタデータのLONG表示							*/
	UINT	Base       : 2;						/* 表示時の底(DEC/HEX/BIT)							*/
	UINT	Sign       : 1;						/* 表示時の符号有無(W_SIGN:有/NOSIGN:無)			*/
	UINT	Decimals   : 3;						/* 小数点以下桁数(0-4)								*/
	UINT	ValClr     : 1;						/* モニタ変数クリア(DISABLE/ENABLE)					*/
	UINT	GType      : 2;						/* GTYPE_VAL(変数)/GTYPE_CONST(定数)				*/
	UINT	NoneDisp   : 1;						/* モニタ無効時none表示								*/
	UINT	SpareBits  : 5;						/* 予備												*/
/*--------------------------------------------------------------------------------------------------*/
	UCHAR	ChkSpUse;							/* 専用使用(COMMON/ROTARY/LINEAR/etc)				*/
	UCHAR	AccessLevel;						/* アクセスレベル(USR1/USR2/etc)					*/
/*--------------------------------------------------------------------------------------------------*/
	ULONG	(*FuncPtr)( struct _MONDEF * );		/* モニタデータ演算関数								*/
/*--------------------------------------------------------------------------------------------------*/
	union	{									/* モニタ変数定義									*/
	ULONG	Ulong;								/* 定義用(変数初期化は、最初のメンバが有効)			*/
	LONG	*Plong;								/* LONG変数へのポインタ								*/
	ULONG	*Pulong;							/* ULONG変数へのポインタ							*/
	SHORT	*Pshort;							/* SHORT変数へのポインタ							*/
	USHORT	*Pushort;							/* USHORT変数へのポインタ							*/
	} Var;										/* モニタ変数へのポインタ							*/
/*--------------------------------------------------------------------------------------------------*/
	union	{									/* モニタ演算ゲイン定義								*/
	ULONG	Ulong;								/* 定義用(変数初期化は、最初のメンバが有効)			*/
	LONG	*Plong;								/* LONGゲインへのポインタ							*/
	ULONG	*Pulong;							/* ULONGゲインへのポインタ							*/
	SHORT	Short[2];							/* SHORTゲイン : Var*Short[0]/Short[1]				*/
	USHORT	Ushort[2];							/* USHORTゲイン : Var*Ushort[0]/Ushort[1]			*/
	} Gain;										/* モニタゲインへのポインタまたは定数				*/
} MONDEF;
typedef const MONDEF CMONDEF;
/*--------------------------------------------------------------------------------------------------*/
/*		Ｕｎモニタ管理テーブル構造体																*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct _MONTBL
{
	USHORT	UnNo;								/* UnNo/RegNo										*/
	CMONDEF	*MonDefPtr;							/* データ定義へのポインタ							*/
} MONTBL;
typedef const MONTBL CMONTBL;

/*--------------------------------------------------------------------------------------------------*/
/*		書込み禁止 & ユーザレベル構造体																*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _ACCESS
{
	USHORT	inhibit;	/* 書込み禁止						*/
	USHORT	level;		/* ユーザレベル							*/
	USHORT	org_inhi;	/* 電源投入時の書込み禁止状態			*/
	USHORT	OpeDspMode;	/* オペレータのユーザー定数表示選択		*/							/* <V041> */
} ACCESS;

/*--------------------------------------------------------------------------*/
/*		レジスタＩＦ用データ定義											*/
/*--------------------------------------------------------------------------*/
typedef	struct	_MSGIF {				/* メッセージＩＦ用構造体定義				*/
	UCHAR  *CmdBuf;						/* 指令メッセージバッファポインタ			*/
	UCHAR  *ResBuf;						/* 応答メッセージバッファポインタ			*/
	LONG  (*StxFunc)( UCHAR *, USHORT );/* 応答メッセージ送信開始ＡＰＩ関数			*/
	void  (*ResFunc)( struct _MSGIF * );/* Ｆｎ実行結果応答メッセージ処理関数		*/
	USHORT	RcvLen;						/* 指令メッセージ長							*/
	USHORT	RcvBufSize;					/* 受信バッファサイズ						*/
	USHORT	SndBufSize;					/* 送信バッファサイズ						*/
	USHORT	FunSts;						/* 補助機能実行結果							*/
//	USHORT	FunIdx;						/* 補助機能テーブルインデックス				*///<V620>
	USHORT	FunExeIdx;					/* 補助機能実行テーブル指定インデックス		*//* <V620> */
	USHORT	FnStsToOpi;					/* 補助機能実行結果(OPTION-SERVO I/F用)		*//* <V620> */
	USHORT	FunResHold;					/* 補助機能応答ホールド						*/
	USHORT	ErrRno;						/* 異常レジスタ番号							*/
	USHORT	LstErrRno;					/* 前回コマンドの異常レジスタ番号			*/
	USHORT	AccsLvl;					/* アクセスレベル							*/
	USHORT	ComPortType;				/* 通信中のチャネル情報						*//* <V239> Add */
	USHORT	Spare00;					/* Spare for alignment						*//* <V620> */
	ULONG	ComTimeLong;				/* 通信タイムアウト計測用					*/
#if 0
#if (SERVO_OPT_IF_VER >= 0x0210)
	USHORT	ComTimeShort;				/* オプション通信タイムアウト計測用			*/
	USHORT	RcvWait;					/* SCメッセージ通信受信待ちフラグ			*/
#endif
#endif
} MSGIF;
typedef	MSGIF*	HMSGIF;					/* 同上用ハンドル定義					*/

typedef struct	_PNLOP_DISP_CTRL
{	/*Structure of Display Control */
	USHORT	CurDigit;	/* 現在カーソル位置							*/
	USHORT	Decimals;	/* 小数点以下桁数							*/
	SHORT	LLimit;		/* 下限値									*/
	SHORT	ULimit;		/* 上限値;									*/
	USHORT	Base;		/* DATA 表示の底							*/
	USHORT	Sign;		/* 符号有無									*/
	USHORT	DspPos;		/* 現在表示位置								*/
	USHORT	SupFlg;		/* 0表示抑止フラグ							*/
	USHORT	Digit;		/* 処理桁数									*/
	USHORT	StartDgt;	/* 開始桁									*/
	USHORT	Length;		/* 表示文字数								*/
} PNLOP_DISP_CTRL;



/*-------------------------------------------End Of File  -----------------------------------------------*/
#endif	/* _PNLOP_H */

