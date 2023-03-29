/****************************************************************************
Description	: MECHATROLINK Command Table
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------

****************************************************************************/
#ifndef CMD_TABLE_H_

//#include "M3Handler.h"

/* タイムアウト時間定義 */
#define TIMEOUT_OF_NOP			0
#define TIMEOUT_OF_PRM_RD		1000
#define TIMEOUT_OF_PRM_WR		1000
#define TIMEOUT_OF_ID_RD		1000
#define TIMEOUT_OF_CONFIG		10000
#define TIMEOUT_OF_ALM_RD		1000
#define TIMEOUT_OF_ALM_CLR		1000
#define TIMEOUT_OF_SYNC_SET		10000
#define TIMEOUT_OF_CONNECT		10000
#define TIMEOUT_OF_DISCONNECT	10000
#define TIMEOUT_OF_MEM_RD		1000
#define TIMEOUT_OF_MEM_WR		10000	/* ADJ相当機能なのでADJに合わせる */
#define TIMEOUT_OF_PPRM_WR		1000
#define TIMEOUT_OF_POS_SET		0
#define TIMEOUT_OF_BRK_ON		0
#define TIMEOUT_OF_BRK_OFF		0
#define TIMEOUT_OF_SENS_ON		2000
#define TIMEOUT_OF_SENS_OFF		2000
#define TIMEOUT_OF_HOLD			0
#define TIMEOUT_OF_LTMOD_ON		0
#define TIMEOUT_OF_LTMOD_OFF	0
#define TIMEOUT_OF_SMON			0
#define TIMEOUT_OF_SV_ON		15000
#define TIMEOUT_OF_SV_OFF		0
#define TIMEOUT_OF_INTERPOLATE	0
#define TIMEOUT_OF_POSING		0
#define TIMEOUT_OF_SPOSING		0
#define TIMEOUT_OF_FEED			0
#define TIMEOUT_OF_LATCH		0
#define TIMEOUT_OF_EX_FEED		0
#define TIMEOUT_OF_EX_POSING	0
#define TIMEOUT_OF_ZRET			0
#define TIMEOUT_OF_VELCTRL		0
#define TIMEOUT_OF_TRQCTRL		0
#define TIMEOUT_OF_ADJ			10000
#define TIMEOUT_OF_SVCTRL		1000
#define TIMEOUT_OF_SVPRM_RD		1000
#define TIMEOUT_OF_SVPRM_WR		1000
#define TIMEOUT_OF_MON_START	0
#define TIMEOUT_OF_MON_STOP		0
#define TIMEOUT_OF_NO_DEFINE	0

#define	CMD_NUMBER				0x100	/* コマンド定義Indexテーブルに登録されているコマンド数 */		
#define	PROFILE_NUMBER			0x3		/* コマンド定義Indexテーブルに登録されているプロファイル数 */		


/* メインコマンド  コマンドコードインデックス */
enum {	Std_Idx_NO_DEFINE,	Std_Idx_NOP,		Std_Idx_PRM_RD,		Std_Idx_PRM_WR,		Std_Idx_ID_RD,	
		Std_Idx_CONFIG,		Std_Idx_ALM_RD,		Std_Idx_ALM_CLR,	Std_Idx_SYNC_SET,	Std_Idx_CONNECT,
		Std_Idx_DISCONNECT,	Std_Idx_PPRM_RD,	Std_Idx_PPRM_WR,	Std_Idx_MEM_RD,		Std_Idx_MEM_WR,
		Std_Idx_POS_SET,	Std_Idx_BRK_ON,		Std_Idx_BRK_OFF,	Std_Idx_SENS_ON,	Std_Idx_SENS_OFF,
		Std_Idx_SMON,		Std_Idx_SV_ON,		Std_Idx_SV_OFF,		Std_Idx_INTERPOLATE,Std_Idx_POSING,
		Std_Idx_FEED,		Std_Idx_EX_FEED,	Std_Idx_EX_POSING,	Std_Idx_ZRET,		Std_Idx_VELCTRL,
		Std_Idx_TRQCTRL,	Std_Idx_SVPRM_RD,	Std_Idx_SVPRM_WR,	Std_Idx_SPOSING,
	 };


enum {	M2_Idx_NO_DEFINE,	M2_Idx_NOP,			M2_Idx_PRM_RD,		M2_Idx_PRM_WR,		M2_Idx_ID_RD,
		M2_Idx_CONFIG,		M2_Idx_ALM_RD,		M2_Idx_ALM_CLR,		M2_Idx_SYNC_SET,	M2_Idx_CONNECT,
		M2_Idx_DISCONNECT,	M2_Idx_PPRM_RD,		M2_Idx_PPRM_WR,		M2_Idx_MEM_RD,		M2_Idx_MEM_WR,	
		M2_Idx_POS_SET,		M2_Idx_BRK_ON,		M2_Idx_BRK_OFF,		M2_Idx_SENS_ON,		M2_Idx_SENS_OFF,
		M2_Idx_HOLD,		M2_Idx_MLOCK_ON,	M2_Idx_MLOCK_OFF,	M2_Idx_LTMOD_ON,	M2_Idx_LTMOD_OFF,
		M2_Idx_SMON,		M2_Idx_SV_ON,		M2_Idx_SV_OFF,		M2_Idx_INTERPOLATE,	M2_Idx_POSING,	
		M2_Idx_FEED,		M2_Idx_LATCH,		M2_Idx_EX_POSING,	M2_Idx_ZRET,		M2_Idx_VELCTRL,
		M2_Idx_TRQCTRL,		M2_Idx_ADJ,			M2_Idx_SVCTRL,		M2_Idx_MON_START,	M2_Idx_MON_STOP,
	 };


enum {	Evt_Idx_NO_DEFINE,	Evt_Idx_NOP,		Evt_Idx_ID_RD,		Evt_Idx_CONNECT,	Evt_Idx_DISCONNECT,
		Evt_Idx_MEM_RD,
	 };


/* サブコマンド  コマンドコードインデックス */
enum {	Std_Idx_ExNO_DEFINE,Std_Idx_ExNOP,		Std_Idx_ExPRM_RD,	Std_Idx_ExPRM_WR,	Std_Idx_ExALM_RD,
		Std_Idx_ExALM_CLR,	Std_Idx_ExPPRM_RD,	Std_Idx_ExPPRM_WR,	Std_Idx_ExMEM_RD,	Std_Idx_ExMEM_WR,
		Std_Idx_ExSMON,		Std_Idx_ExSVPRM_RD,	Std_Idx_ExSVPRM_WR,
	 };


enum {	M2_Idx_ExNO_DEFINE,	M2_Idx_ExNOP,		M2_Idx_ExPRM_RD,	M2_Idx_ExPRM_WR,	M2_Idx_ExALM_RD,
		M2_Idx_ExPPRM_RD,	M2_Idx_ExPPRM_WR,	M2_Idx_ExLTMOD_ON,	M2_Idx_ExLTMOD_OFF,	M2_Idx_ExSMON,
	 };


enum {	Evt_Idx_ExNO_DEFINE,
	 };


/* プロファイルコードインデックス */
enum {	Idx_M3STDPROFILE, Idx_M2PROFILE, Idx_EVTPROFILE, Idx_NO_DEFINE };



/****************************************************************************
 Structure for the Command table 
****************************************************************************/
typedef struct CMD_DEF_TBL_ {
	ULONG timeOut;					/* コマンドタイムアウト時間[ms] */
	LONG availablePhase;			/* 使用可能フェーズ */
	LONG exCmdTblIdx;				/* サブコマンド管理テーブルへのIndex */
	BOOL  chkCmdLock;				/* サブ干渉チェック(サブ不可)  1 : 有効(不可)  0 : なし(可) */
	INT32 (*cmdFuncPtr)(void *hNetData);					/* コマンド解析関数処理ポインタ */
	VOID (*rspFuncPtr)(void *hNetData, void* rsp_buf);		/* レスポンス作成関数処理ポインタ */
} CMD_DEF_TBL;


typedef struct EX_CMD_DEF_TBL_ {
	ULONG timeOut;					/* コマンドタイムアウト時間[ms] */
	LONG availablePhase;			/* 使用可能フェーズ */
	BOOL  chkCmdLock;				/* メインコマンド干渉チェック 1 : 有効  0 : なし */
	INT32 (*exCmdFuncPtr)(void *hNetData);					/* コマンド解析関数処理ポインタ */
	VOID (*exRspFuncPtr)(void *hNetData, void* rsp_buf); 	/* レスポンス作成関数処理ポインタ */
} EX_CMD_DEF_TBL;



/* externs */
void CmdSetCMDTablePointer(UCHAR ProfileType, void **IdxTblPtr, 
								void **CmdTblPtr, void **ExCmdTblPtr);



#define CMD_TABLE_H_
#endif /* CMD_TABLE_H_ */


