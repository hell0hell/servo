/****************************************************************************/
/*																			*/
/*																			*/
/*		Motion API ヘッダファイル											*/
/*																			*/
/*																			*/
/****************************************************************************/
/*	Histroy:																*/
/*		Rev2.01 2007.12.14	加速度指令追加									*/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/************** Copyright Yaskawa Electric Corporation **********************/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/****************************************************************************/
#ifndef		MOTION_API_H_

#include "MtCtrl.h"
#include "ComCfg.h"

/****************************************************************************
 Structure of the Motion Handle
****************************************************************************/
/* モーション指令構造体定義 */
typedef struct MCI_CMD_ {
	LONG		TPos;		/* 目標位置[ref] */
	LONG		TSpeed;		/* 目標速度[ref/s] */
	LONG		Velocity;	/* VFF/速度指令 [ref/s],[2^30/MAX] */
	LONG		Trq;		/* トルク指令[MAX_TRQ/2^30] */
	ULONG		TrqLimitP;	/* 正側トルク制限[MAX_TRQ/2^30] */
	ULONG		TrqLimitN;	/* 負側トルク制限[MAX_TRQ/2^30] */
	ULONG		AccRate;	/* 加速定数<Rev2.01> */
	ULONG		DecRate;	/* 減速定数<Rev2.01> */
	ULONG		Option;		/* オプション指令 */
	LONG		HoldMod;	/* 停止方法選択 0: 減速停止1,1:急速停止,2減速停止2 */
	LONG		LatchSgnl;	/* ラッチ信号選択 0:C相,n:EXTn */
	BOOL		EnableRate;	/* 加速度指令有効<Rev2.01> */
	BOOL		RstReq;		/* モーションリセット要求 */
} MCI_CMD;

/****************************************************************************
 API's
****************************************************************************/
/* INTERPOLATE指令設定 */
INT32 MciSetInterpolateCmd(MT_HNDL* hdl, MCI_CMD* cmd);
/* LATCH指令設定 */
INT32 MciSetLatchCmd(MT_HNDL* hdl, MCI_CMD* cmd);
/* POSING指令設定 */
INT32 MciSetPosingCmd(MT_HNDL* hdl, MCI_CMD* cmd);
/* EX_POSING指令設定 */
INT32 MciSetExPosingCmd(MT_HNDL* hdl, MCI_CMD* cmd);
/* HOMING指令設定 */
INT32 MciSetHomingCmd(MT_HNDL* hdl, MCI_CMD* cmd);
/* FEED指令設定 */
INT32 MciSetFeedCmd(MT_HNDL* hdl, MCI_CMD* cmd);
/* HOLD指令設定 */
INT32 MciSetHoldCmd(MT_HNDL* hdl, MCI_CMD* cmd);
/* VELCTRL指令設定 */
INT32 MciSetVelCmd(MT_HNDL* hdl, MCI_CMD* cmd);
/* TRQCTRL指令設定 */
INT32 MciSetTrqCmd(MT_HNDL* hdl, MCI_CMD* cmd);
/* OPTION指令設定 */
VOID MciSetOptionCmd(MT_HNDL* hdl, ULONG option);

/* モーション状態切り替え */
INT32 MciEnableState(MT_HNDL* hdl, BOOL enable);
/* モーション状態リセット */
INT32 MciResetMotion(MT_HNDL* hdl);
/* サーボオン指令発行 */
INT32 MciSetServoOn(MT_HNDL* hdl, BOOL enable, BOOL reset);
/* センサオン指令発行 */
INT32 MciSetSensOn(MT_HNDL* hdl, BOOL enable);

/* ラッチ要求発行(Motion用) */
INT32 MciRequestMtLatch(MT_HNDL* hdl, LONG mode, LONG signl);
/* Modalラッチ要求発行 */
INT32 MciRequestMdlLatch(MT_HNDL* hdl, UCHAR mode, LONG signl);
/* ラッチキャンセル処理 */
INT32 MciCancelLatch(MT_HNDL* hdl, LONG mode);
/* 座標系設定 */
INT32 MciSetCordinatePos(MT_HNDL* hdl, LONG data, BOOL ref);
/* リモートI/O出力	*/
VOID MciOutputSignals(MT_HNDL* hdl, USHORT signl);

/* Motion状態取得API */
/* モニタデータ取得	*/
LONG MciGetMonitorData(MT_HNDL* hdl, USHORT sel, UCHAR unit);
/* LATCHステータス取得	*/
BOOL MciGetLatchStatus(MT_HNDL* hdl ,LONG *ltEdge);
/* I/Oモニタデータ取得	*/
USHORT MciGetInputSignals(MT_HNDL* hdl);
/* モニタデータ(ポート入力)取得	*/
USHORT MciGetInPortSignals(MT_HNDL *hdl);
#if 0
#if ( CCSW_SFTID == SFTID_EXIST )
/* セーフティモジュールモニタデータ取得	*/
USHORT MciGetSomStatus(void);
#endif
#endif
/****************************************************************************
 Defines
****************************************************************************/
/* API戻り値*/
#define	MCI_WAIT_COMP		ERR_WAIT_COMP
#define	MCI_SUCCESS			ERR_NONE
#define	MCI_CNDTN_ERR		ERR_CMD_CNDTN
#define	MCI_EMGBB_ERR		ERR_CMD_CNDTN
#define	MCI_SVRDY_ERR		ERR_CMD_CNDTN
#define	MCI_LTCNFLCT_ERR	ERR_CMD_CNFLCT
#define	MCI_SPD_ERR			ERR_DATA_SET
#define	MCI_LTMOD_ERR		ERR_DATA_LTMOD

/* ラッチモード(API設定用) */
#define MOTION_LTMD			1	/* モーションラッチモード */
#define MOTION_MDL_LTMD		2	/* モーションラッチモード(SVCTRL) */
#define MODAL_LTMD			3	/* モーダルラッチモード(Normal) */
/* ラッチモード(内部処理用) */
#define INTERPOL_LTMD		4	/* モーションラッチラッチモード(INTERPOLATE) */
#define EXPOSING_LTMD		5	/* モーションラッチラッチモード(EXPOSING) */
#define ZRET_LTMD			6	/* モーションラッチラッチモード(ZRET) */
//#define ZRET_LOCK_LTMD		7	/* モーションラッチラッチモード(ZRET中) */
#define NONE_LTMD			0	/* ラッチモードなし */

/* 制御モード */
#define MT_NO_CTRLMD	0		/* 制御無効モード */
#define MT_POS_CTRLMD	1		/* 位置制御モード */
#define MT_VEL_CTRLMD	2		/* 速度制御モード */
#define MT_TRQ_CTRLMD	3		/* トルク制御モード */

/* モーションタイプ */
enum MOTIONTYPE {
	MTYPE_NONE=0,	/* 0:モーション停止中 */ 
	MTYPE_POSING,	/* 1:位置決め */ 
	MTYPE_EXPOSING,	/* 2:外部位置決め */ 
	MTYPE_HOMING,	/* 3:原点復帰 */
	MTYPE_FEED,		/* 4:定速送り */
	MTYPE_HOLD,		/* 5:停止 */
	MTYPE_INTERPOL,	/* 6:補間送り */ 
	MTYPE_VELCTRL,	/* 7:速度制御 */
	MTYPE_TRQCTRL,	/* 8:トルク制御 */
	MTYPE_FOLLOW,	/* 9:フォローアップ */ 
};

/* モーション状態 */
enum MT_STATE {
	MSTAT_DISABLE=0,	/* Disable状態 */
	MSTAT_WAITPG_RDY,	/* PG RDY待ち状態 */
	MSTAT_READY,		/* Ready状態 */
	MSTAT_RUN,			/* Run状態 */
	MSTAT_WAIT_SVOFF,	/* サーボOFF待ち */
	MSTAT_RAMP_STOP,	/* サーボOFF減速停止状態 */
};

/* モニタ選択番号 */
enum MCI_MONITOR {
	POS_MON = 0,	/* 0: POS */
	MPOS_MON,		/* 1: MPOS */
	PERR_MON,		/* 2: PERR */
	APOS_MON,		/* 3: APOS */
	LPOS_MON,		/* 4: LPOS */
	IPOS_MON,		/* 5: IPOS */
	TPOS_MON,		/* 6: TPOS */
	DUMMY1_MON,		/* 7: ---- */
	FSPD_MON,		/* 8: FSPD */
	CSPD_MON,		/* 9: CSPD */
	TSPD_MON,		/* a: TSPD */
	TRQ_MON,		/* b: TRQ */
	DUMMY2_MON,		/* c: ---- */
	DUMMY3_MON,		/* d: ---- */
	OMN1_MON,		/* e: OMN1 */
	OMN2_MON,		/* f: OMN2 */
};

/* I/Oビット割付位置 */
enum MCI_SGNLBIT {
	P_OT_SGNL=0,
	N_OT_SGNL,
	DEC_SGNL,
	PA_SGNL,
	PB_SGNL,
	PC_SGNL,
	EXT1_SGNL,
	EXT2_SGNL,
	EXT3_SGNL,
	BK_SGNL,
	EMG_SGNL,
};


/* defines */
#define TREFMODE_NON		0
#define TREFMODE_TLIM1		1
#define TREFMODE_TFF		2
#define TREFMODE_TLIM2		3
#define TREFMODE_TFF_TLIM	4

#define		MOTION_API_H_
#endif /* MOTION_API_H_ */
