/****************************************************************************/
/*                                                                          */
/*            MECHATROLINK サーボ通信ＩＦアプリ 通信コンポーネント          */
/*                                                                          */
/*            コマンド管理テーブル	ヘッダーファイル						*/
/*																			*/
/****************************************************************************/
/* ＩＦ定義関数 :                                                           */
/*																			*/
/*                                                                          */
/****************************************************************************/
/*	【改版履歴】															*/
/*		Ver1.00																*/
/*                                                                          */
/*******************  2003 Yaskawa Electric Corporation *********************/
#ifndef		CMD_TABLE_H_
#define		CMD_TABLE_H_


/* タイムアウト時間定義 */
#define TIMEOUT_OF_NOP			0
#define TIMEOUT_OF_PRM_RD		1000
#define TIMEOUT_OF_PRM_WR		1000
#define TIMEOUT_OF_ID_RD		1000
#define TIMEOUT_OF_CONFIG		5000
#define TIMEOUT_OF_ALM_RD		1000
#define TIMEOUT_OF_ALM_CLR		1000
#define TIMEOUT_OF_SYNC_SET		10000
#define TIMEOUT_OF_CONNECT		10000
#define TIMEOUT_OF_DISCONNECT	10000
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
#define TIMEOUT_OF_FEED			0
#define TIMEOUT_OF_LATCH		0
#define TIMEOUT_OF_EX_POSING	0
#define TIMEOUT_OF_ZRET			0
#define TIMEOUT_OF_VELCTRL		0
#define TIMEOUT_OF_TRQCTRL		0
#define TIMEOUT_OF_ADJ			10000
#define TIMEOUT_OF_SVCTRL		1000
#define TIMEOUT_OF_NO_DEFINE	0

/* 定数定義 */
#define	CMD_NUMBER		0x100	/* コマンド定義Indexテーブルに登録されているコマンド数 *//* <V715> */

/* メインコマンド  コマンドコードインデックス */
enum {	Idx_NO_DEFINE,	Idx_NOP,		Idx_PRM_RD,		Idx_PRM_WR,		Idx_ID_RD,
		Idx_CONFIG,		Idx_ALM_RD,		Idx_ALM_CLR,	Idx_SYNC_SET,	Idx_CONNECT,
		Idx_DISCONNECT,	Idx_PPRM_RD,	Idx_PPRM_WR,	Idx_MEM_RD,		Idx_MEM_WR,	
		Idx_POS_SET,	Idx_BRK_ON,		Idx_BRK_OFF,	Idx_SENS_ON,	Idx_SENS_OFF,
		Idx_HOLD,		Idx_MLOCK_ON,	Idx_MLOCK_OFF,	Idx_LTMOD_ON,	Idx_LTMOD_OFF,
		Idx_SMON,		Idx_SV_ON,		Idx_SV_OFF,		Idx_INTERPOLATE,Idx_POSING,	
		Idx_FEED,		Idx_LATCH,		Idx_EX_POSING,	Idx_ZRET,		Idx_VELCTRL,
		Idx_TRQCTRL,	Idx_ADJ,		Idx_SVCTRL,		
	 };

/* サブコマンド  コマンドコードインデックス */
enum {	Idx_ExNO_DEFINE,Idx_ExNOP,		Idx_ExPRM_RD,	Idx_ExPRM_WR,	Idx_ExALM_RD,
		Idx_ExPPRM_RD,	Idx_ExPPRM_WR,	Idx_ExLTMOD_ON,	Idx_ExLTMOD_OFF,Idx_ExSMON,		
	 };


/* typedefs: struct */
typedef struct CMD_DEF_TBL_ {
	ULONG timeOut;				/* コマンドタイムアウト時間[ms] */
	UCHAR availablePhase;		/* 使用可能フェーズ */
	UCHAR activeModeFlg;		/* アクティブモード解除待ちフラグ *//*<V730>Add*/
	UCHAR exCmdTblIdx;			/* サブコマンド管理テーブルへのIndex */
	BOOL  chkCmdLock;			/* サブ干渉チェック(サブ不可)  1 : 有効(不可)  0 : なし(可) */
	INT32 (*cmdFuncPtr)(void *hNetData);		/* コマンド解析関数処理ポインタ */
	VOID (*rspFuncPtr)(void* hNetData, void *rsp_buf);		/* レスポンス作成関数処理ポインタ */
} CMD_DEF_TBL;

typedef struct EX_CMD_DEF_TBL_ {
	ULONG timeOut;				/* コマンドタイムアウト時間[ms] */
	UCHAR availablePhase;		/* 使用可能フェーズ */
	BOOL  chkCmdLock;			/* メインコマンド干渉チェック 1 : 有効  0 : なし */
	INT32 (*exCmdFuncPtr)(void *hNetData);	/* コマンド解析関数処理ポインタ */
	VOID (*exRspFuncPtr)(void *hNetData, void *rsp_buf); 	/* レスポンス作成関数処理ポインタ */
} EX_CMD_DEF_TBL;

/* externs */
void CmdSetCMDTablePointer(void **IdxTblPtr, void **CmdTblPtr, void **ExCmdTblPtr);
								

#if 0/* 暫定処理：tableの割付未定	*/
/* variables *//* <V715> */
#ifdef	__ghs__
#pragma ghs startzda
#endif
extern CMD_DEF_TBL CmdTbl[];		/* メインコマンド  コマンド管理テーブル */
extern EX_CMD_DEF_TBL ExCmdTbl[];	/* サブコマンド  コマンド管理テーブル */
extern UCHAR CmdIdx[];				/* コマンド定義indexテーブル */
#ifdef	__ghs__
#pragma ghs endzda
#endif

/* function */
extern VOID CmdTableCopyforRam(VOID);	    /* <V715> */
#endif /* 暫定処理：tableの割付未定	*/

#endif /* CMD_TABLE_H_ */
