#ifndef CMD_HANDLER_H_
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		CmdHandler.h : CMD_HNDL定義																	*/
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
#include "M2StdDataFormat.h"
#include "CmdTable.h"
#include "SysApi.h"

/****************************************************************************************************/
/*		STRUCT DEFINITION																			*/
/****************************************************************************************************/


/* メインコマンド解析用構造体 */
typedef struct CMD_INFO_ {
	CMD_LINK_DATA 	cmdBuf;					/* 解析用メインCMDバッファ */
	CMD_LINK_DATA	rsltBuf;				/* コマンド解析結果 */
	ULONG			busyCnt;				/* コマンドビジー時間[解析周期] */
	BOOL			busy;					/* コマンド実行中フラグ(コマンド干渉チェック用) */
	BOOL			enableCmdAna;			/* コマンド解析実行要求フラグ */
	BOOL			cmdRdy;					/* コマンドのコマンドレディビット */
	/* M2プロファイル用 */
	LONG			adjWrk;					/* ADJ用ワーク */
	LONG			crtCmd;					/* 現在コマンドコード */
    USHORT		    cmdCdataBuf;			/* ADJ(SUBCODE=10H)メインコマンドのCDATA指令バッファ*//*<V624>*/
	USHORT		    chkOpIoCmd; 			/* OPTION/IO_MONフィールド追加コマンド使用チェック用*//*<V624>*/

	REG_DATA		RegData;				/* レジスタアクセス用 */
	LONG			RegWkBuf[10];			/* レジスタアクセス用バッファ */
} CMD_INFO;


/* サブコマンド解析用構造体 */
typedef struct EX_CMD_INFO_ {
	CMD_LINK_EXDATA	cmdBuf;					/* 解析用サブCMDバッファ */
	CMD_LINK_EXDATA	rsltBuf;				/* コマンド解析結果 */
	LONG			busyCnt;				/* コマンドビジー時間[解析周期] */	
	BOOL			busy;					/* コマンド実行中フラグ(コマンド干渉チェック用) */
	BOOL			enableCmdAna;			/*コマンド解析実行要求フラグ */
	BOOL			cmdRdy;					/* コマンドレディビット */
	BOOL			warng;					/* ワーニングビット */
	
	REG_DATA		RegData;				/* レジスタアクセス用 */
	LONG			RegWkBuf[10];			/* レジスタアクセス用バッファ */
} EX_CMD_INFO;


/* CONFIG/SYNC_SET/CONNECT/DISCONNECT */
typedef enum CMD_SEQ_TYPE {	
	IDOL_SEQ=0,								/* IDOL処理シーケンス */
	SV_OFF_SEQ,								/* サーボオフ処理シーケンス */
	BK_OFF_SEQ, 							/* ブレーキオフ処理シーケンス */
	SENS_OFF_SEQ, 							/* センスオフ処理シーケンス */
	MT_DISABLE_SEQ, 						/* モーション無効処理シーケンス */
	PRM_RE_CALC_SEQ,						/* パラメータ再計算処理シーケンス */
	MT_ENABLE_SEQ, 							/* モーション有効処理シーケンス */
	REQ_PHSUP_SEQ, 							/* フェーズアップ処理シーケンス */
	REQ_PHSDOWN_SEQ, 						/* フェーズダウン処理シーケンス */
	SOFT_RESET_SEQ, 						/* ソフトリセット処理シーケンス */
	WAIT_SOFT_RESET_SEQ, 					/* ソフトリセット実行待ちシーケンス */
	PHASE_CHK_SEQ,							/* フェーズ確認処理シーケンス */
	FAIL_END_SEQ, 							/* 処理失敗シーケンス */
	PRM_RESET_SEQ,							/* パラメータ初期化シーケンス */
	PRM_ACCESS_SEQ,							/* パラメータアクセスシーケンス */
}CMD_SEQ_TYPE;


/* CMD Analyzer ハンドラ */
typedef struct CMD_HNDL_ {
	CMD_INFO		cmdInfo;				/* メインコマンド情報 */
	EX_CMD_INFO		exCmdInfo;				/* サブコマンド情報 */
	CMD_SEQ_TYPE	cmd_seq;				/* コマンド解析シーケンス番号 */
	BOOL			useExCmd;				/* サブコマンド使用フラグ */
	CMD_DEF_TBL		*cmdTblBasePtr;			/* コマンド定義テーブルのポインタ */
	EX_CMD_DEF_TBL	*exCmdTblBasePtr;		/* サブコマンド定義テーブルのポインタ*/
	UCHAR			*CmdIdxTbl;				/* コマンドインデックステーブル */
} CMD_HNDL;



#define CMD_HANDLER_H_
#endif /* CMD_HANDLER_H_ */
/***************************************** end of file **********************************************/
