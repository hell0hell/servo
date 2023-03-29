/****************************************************************************
Description	: Utility Function Manager
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
I.M.   2010.11.12     created

*****************************************************************************/
#ifndef __FUN_MANAGER_H
#define __FUN_MANAGER_H

#include "PrmAccessIf.h"


/****************************************************************************
 Definition of the application data
*****************************************************************************/
typedef enum FCMD_CODE {
	FCMD_STOP	= 0x00,					/* 停止					*/
	FCMD_EXEC	= 0x01,					/* 実行					*/
	FCMD_UP		= 0x02,					/* UP,正転,正方向		*/
	FCMD_DOWN	= 0x03,					/* DOWN,逆転,逆方向		*/
	FCMD_SV		= 0x04,					/* サーボオン/オフ		*/
	FCMD_CH1	= 0x05,					/* CH1,U相選択			*/
	FCMD_CH2	= 0x06,					/* CH2,V相選択			*/
	FCMD_EWRT	= 0x07,					/* EEPROM書込み処理		*/
	FCMD_EXEC2	= 0x08,					/* 補助実行				*/
	FCMD_MTEXEC	= 0x11,					/* 操作モード維持(実行)	*/
	FCMD_MTUP	= 0x12,					/* 操作モード維持(正転)	*/
	FCMD_MTDOWN	= 0x13,					/* 操作モード維持(逆転)	*/
	FCMD_NONE	= 0xFF,					/* 命令なし				*/
} FCMD_CODE;

/*----------------------------------------------------------------------------
 Definition of the number of Function mode
----------------------------------------------------------------------------*/
#define FUN_MODE_NUM	2
typedef enum FUN_EXECH {
	FUN_CH1 = 0,	/* the first mode */
	FUN_CH2 = 1,	/* the second mode */
} FUN_EXECH;

typedef	struct FUNEXE { /* 補助機能アプリ側で参照/操作できるメンバ */
	FCMD_CODE	CmdCode;				/* 命令コード */
	UINT		State;					/* Fnアプリで管理するシーケンス状態 */
										/* (Fnモード開始で0リセット) */
	ULONG		LongTimer;				/* Fnアプリ用のタイマ */
	BOOL		HoldFnMode;				/* Fnモード継続フラグ */
	PRM_RSLT	ErrorResult;			/* Fnモード実行結果 */
} FUNEXE;

/*----------------------------------------------------------------------------
 補助機能実行時のパラメータ書き込み規制テーブル定義
----------------------------------------------------------------------------*/
#define FUNWRTDISFN_MAX 20				/* Fn番号最大登録数					*/
#define FUNWRTDISPN_MAX 40				/* Pn番号最大登録数					*/

typedef struct FUNWRTDISFNPNTbl {
	USHORT FnNo ;						/* 書込み禁止対象Fn番号				*/
	USHORT FnMode ;						/* 書込み禁止対象レジスタ番号		*/
	USHORT PnNo[FUNWRTDISPN_MAX] ;		/* 書込み禁止対象Pn番号				*/
} FUNWRTDISFNPNTbl ;



/****************************************************************************
 Structure of the Function Table Attribute
*****************************************************************************/
typedef PRM_RSLT (*FUN_ENTER_CALLBACK)(FUNEXE*, void*);
typedef PRM_RSLT (*FUN_EXE_CALLBACK)(FUNEXE*, void*);
typedef void (*FUN_LEAVE_CALLBACK)(FUNEXE*, void*);
//typedef LONG (*FUN_INIT_CALLBACK)(void*);

typedef struct	FUNDEF_ {
	UINT8				RunLevel;		/* No.1/2 OpeReg ParaRun(1(No.1)/2(No.2)/0(NG))		*/
	UINT8				ParaTrace;		/* Executable mode channel bits	*/
	UINT8				AxCommon;		/* Access lock to all axes 	*/
	UINT8				ExePrmProhibit;	/* Access lock in Parameter write prohibited mode */
	UINT8				AccessLevel;	/* AccessLevel(USR1/USR2/etc)	*/
	CHAR				SName[12];		/* FnXXX Short Name for Display	*/
	FUN_ENTER_CALLBACK	EnterFunc;		/* Begin Function for FnFunc */
	FUN_EXE_CALLBACK	ExecuteFunc;	/* Execute Function for FnFunc */
	FUN_LEAVE_CALLBACK	LeaveFunc;		/* End Function for FnFunc	*/
/*--------------------------------------------------------------------------------------------------*/
	LONG				(*OpeBeginFunc)(void *Fmsg);		/* Operator Begin Function				*/
	LONG				(*OpeMainFunc)(void *Fmsg);		/* Operator Main Function				*/
	void				(*OpeEndFunc) (void *Fmsg);		/* Operator End Function				*/
/*--------------------------------------------------------------------------------------------------*/
//	FUN_INIT_CALLBACK	InitFunc;		/* Initialize Function for FnFunc */
} FUNDEF;
typedef const FUNDEF	CFUNDEF;		/* const FUNDEF Type Definition	*/
typedef CFUNDEF*		CPFUNDEF;		/* const FUNDEF Type Definition	*/

typedef struct FUNTBL_ {
	USHORT	FnNo;					/* FnNo.						*/
	USHORT	FnMode;					/* FnMode for Register I/F		*/
	CFUNDEF	*FunDefAttr;			/* Pointer to FUNDEF(FnFuncDef)	*/
} FUNTBL;
typedef const FUNTBL	CFUNTBL;	/* const FUNTBL Type Definition	*/
typedef CFUNTBL*		CPFUNTBL;	/* const FUNTBL Type Definition	*/




/****************************************************************************
 Structure of the Utility Function Manager Handle
*****************************************************************************/
typedef struct FUN_AXCOMMON {
	ULONG		TraceMode;				/* トレースモード */
	BOOL		modeLock;				/* 操作モードロック中 */
	UINT		AccessLevel;			/* アクセスレベル */
} FUN_AXCOMMON;

typedef struct FUN_MANAGER {
	CPFUNTBL	FunTable;			/* Function Table */
	UINT		NumOfFunTable;		/* Number of the Function Table list */

	void		*CallbackParam;		/* コールバック関数用パラメータ */
	FUN_AXCOMMON *AxCommon;			/* 全軸共通フラグ情報 */
	void		*RegManager;		/* レジスタマネージャハンドルポインタ */

	CPFUNTBL	cFunMode[FUN_MODE_NUM];		/* 現在実行中のFunctionTableへのポインタ */
	FUNEXE		FunExeState[FUN_MODE_NUM];	/* 実行状態 */
} FUN_MANAGER;



/****************************************************************************
 API functions
*****************************************************************************/
void FunInitialize(FUN_MANAGER *FunMngr, void *RegMngr,
				   FUN_AXCOMMON *FunAxCommon, CPFUNTBL FunTable, UINT TableNum);
PRM_RSLT FunSetOpeModeRegister(FUN_MANAGER *FunMngr, FUN_EXECH ch, ULONG mode, void* ClbkParam);
PRM_RSLT FunSetTraceModeRegister(FUN_MANAGER *FunMngr, ULONG mode);
PRM_RSLT FunSetOpeCommandRegister(FUN_MANAGER *FunMngr, FUN_EXECH ch, ULONG cmd);
void FunRunTimeService(FUN_MANAGER *FunMngr, FUN_EXECH ch);
USHORT FunGetOpeModeRegister(FUN_MANAGER *FunMngr, FUN_EXECH ch);



#endif //__FUN_MANAGER_H

