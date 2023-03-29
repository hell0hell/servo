/****************************************************************************
Description	: MECHATROLINK Motion API
----------------------------------------------------------------------------
File name		: MtApi.h			<S008>

Author			: Yaskawa Electric Corp.,
				Copyright (c) 2010 All Rights Reserved

Project			: Mercury

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------

****************************************************************************/
#ifndef MOTION_API_H_


#include "MtCtrl.h"
#include "ComCfg.h"


/* モーション指令構造体定義 */
typedef struct MCI_CMD_ {
	LONG		TPos;				/* 目標位置[ref] */
	LONG		TSpeed;				/* 目標速度[ref/s] */
	LONG		Velocity;			/* VFF/速度指令 [ref/s],[2^30/MAX] */
	LONG		Trq;				/* トルク指令[MAX_TRQ/2^30] */
	ULONG		TrqLimitP;			/* 正側トルク制限[MAX_TRQ/2^30] */
	ULONG		TrqLimitN;			/* 負側トルク制限[MAX_TRQ/2^30] */
	ULONG		AccRate;			/* 加速定数 */
	ULONG		DecRate;			/* 減速定数 */
	LONG		HoldMod;			/* 停止方法選択 0: 減速停止1,1:急速停止,2減速停止2 */
	LONG		LatchSgnl;			/* ラッチ信号選択 0:C相,n:EXTn */
	LONG		RateType;			/* 加減速度指令タイプ [RATE_CMD/ACC_TIME/RATE_PRM] */
	BOOL		RstReq;				/* モーションリセット要求 */
	BOOL		MtLatchReq;			/* モーションラッチ要求 */
	BOOL		HomeDir;			/* 原点復帰方向 [0：正方向 1:負方向] */
	LONG		Sratio;				/* SPOSING用S字区間比率 */
} MCI_CMD;



/****************************************************************************
 API's
****************************************************************************/
/* Motion Api */
/* INTERPOLATE指令設定 */
INT32 MciSetInterpolateCmd( MT_HNDL *hdl, MCI_CMD *cmd );
/* LATCH指令設定 */
INT32 MciSetLatchCmd( MT_HNDL *hdl, MCI_CMD *cmd );
/* POSING指令設定 */
INT32 MciSetPosingCmd( MT_HNDL *hdl, MCI_CMD *cmd );
/* SPOSING指令設定	*/
INT32 MciSetSposingCmd( MT_HNDL *hdl, MCI_CMD *cmd );
/* EX_POSING指令設定 */
INT32 MciSetExPosingCmd( MT_HNDL *hdl, MCI_CMD *cmd );
/* HOMING指令設定 */
INT32 MciSetHomingCmd( MT_HNDL *hdl, MCI_CMD *cmd );
/* HOMING指令設定(DEC信号あり) */
INT32 MciSetHomingCmdWithDec( MT_HNDL *hdl, MCI_CMD *cmd );
/* FEED指令設定 */
INT32 MciSetFeedCmd( MT_HNDL *hdl, MCI_CMD *cmd );
/* EX_FEED指令設定 */
INT32 MciSetExFeedCmd( MT_HNDL *hdl, MCI_CMD *cmd );
/* HOLD指令設定 */
INT32 MciSetHoldCmd( MT_HNDL *hdl, MCI_CMD *cmd );
/* VELCTRL指令設定 */
INT32	MciSetVelCmd( MT_HNDL *hdl, MCI_CMD *cmd );
/* VELCTRL指令設定(加減速度指令あり) */
INT32 MciSetVelCmdWithAccr( MT_HNDL *hdl, MCI_CMD *cmd );
/* TRQCTRL指令設定 */
INT32 MciSetTrqCmd( MT_HNDL *hdl, MCI_CMD *cmd );
/* CMD_PAUSE 指令設定 */
INT32 MciSetCmdPause( MT_HNDL *hdl, MCI_CMD *cmd );
/* CMD_CANCEL 指令設定 */
INT32 MciSetCmdCancel( MT_HNDL *hdl, MCI_CMD *cmd );
/* 移動コマンド一時停止指令時のアプローチ速度ゼロクリア設定 */
VOID MciClearZretSpd( MT_HNDL *hdl );
/* 移動コマンド一時停止指令時のアプローチ速度復帰設定 */
VOID MciReturnZretSpd( MT_HNDL *hdl );
/* OPTION指令設定 */
VOID MciSetOptionCmd( MT_HNDL *hdl, ULONG option );
/* モーション状態切り替え */
INT32 MciEnableState( MT_HNDL *hdl, BOOL enable );
/* サーボオン指令発行 */
INT32 MciSetServoOn( MT_HNDL *hdl, BOOL enable, BOOL reset );
/* センサオン指令発行 */
INT32 MciSetSensOn( MT_HNDL *hdl, BOOL enable );
/* ラッチ要求発行(Motion用) */
INT32 MciRequestMtLatch( MT_HNDL *hdl, LONG mode, LONG sgnl, UCHAR i );
/* Modalラッチ要求発行 */
INT32 MciRequestMdlLatch( MT_HNDL *hdl, UCHAR mode, LONG sgnl, LONG i );
/* ラッチキャンセル処理 */
INT32 MciCancelLatch( MT_HNDL *hdl, LONG mode, LONG i );
/* 座標系設定 */
INT32 MciSetCordinatePos( MT_HNDL *hdl, LONG data, BOOL ref, BOOL ref_clr );
/* モーション状態リセット */
INT32 MciResetMotion( MT_HNDL *hdl );

/* Motion状態取得API */
/* モニタデータ取得(M3標準サーボプロファイル) */
LONG MciGetM3StdMonitorData( MT_HNDL *hdl, LONG sel );
/* モニタデータ取得(M2互換プロファイル) */
LONG MciGetM2MonitorData( MT_HNDL *hdl, USHORT sel, UCHAR unit );
/* モニタデータ(ポート入力)取得(M2互換プロファイル) */
USHORT MciGetInPortSignals( MT_HNDL *hdl );
/* LATCHステータス取得 */
BOOL MciGetLatchStatus( MT_HNDL *hdl, LONG *ltEdge, LONG i );
/* I/Oモニタデータ取得 */
USHORT	MciGetInputSignals( MT_HNDL *hdl );
/* LtRdy(LT_EXE1,2)のモニタ信号作成 */
LONG MciGetLtRdy( MT_HNDL *hdl, LONG i );
/* 指令速度作成処理(速度制御で加減速度設定がありの場合) */
LONG MciMakeCrtVref( MG_HNDL *hdl, MG_INOUT *inOut );



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
#define MCI_ACCR_ERR		ERR_DATA_SET
#define MCI_DATA_CLAMP		ERR_DATA_CLAMP

/* ラッチモード(API設定用) */
#define MOTION_LTMD			1			/* モーションラッチモード */
#define MOTION_MDL_LTMD		2			/* モーションラッチモード(SVCTRL) */
#define MODAL_LTMD			3			/* モーダルラッチモード(Normal) */

/* defines : ラッチモード(内部処理用) */
#define INTERPOL_LTMD		4			/* モーションラッチラッチモード(INTERPOLATE) */
#define EXPOSING_LTMD		5			/* モーションラッチラッチモード(EXPOSING) */
#define ZRET_LTMD			6			/* モーションラッチラッチモード(ZRET) */
//#define ZRET_LOCK_LTMD	7			/* モーションラッチラッチモード(ZRET中) */
#define EXFEED_LTMD			8			/* モーションラッチラッチモード(EX_FEED) */
#define NONE_LTMD			0			/* ラッチモードなし */

/* 制御モード */
#define MT_NO_CTRLMD		0			/* 制御無効モード */
#define MT_POS_CTRLMD		1			/* 位置制御モード */
#define MT_VEL_CTRLMD		2			/* 速度制御モード */
#define MT_TRQ_CTRLMD		3			/* トルク制御モード */

/* プロファイルタイプ */
#define M2_PROFILE			0x00
#define M3STD_PROFILE		0x10
#define EVTID_PROFILE		0x01

/* 加減速指令タイプ */
#define RATE_PRM			0			/* パラメータによる加減速指令 */
#define RATE_ACC_TIME		1			/* 加速時間による加減速度指令 */
#define RATE_CMD			2			/* 通信データによる加減速度指令 */

/* ラッチ番号 */
#define MTLATCH_NUM			0			/* モーションラッチ用ラッチ番号 */
#define SEQLATCH_NUM		1			/* 連続ラッチ用ラッチ番号 */

/* VFF,TFF,TLIM, ACCR の最大値 */
#define	MT_MAXTRQ_30		0x40000000	/* TRQ指令のMAX値   2^30 */
#define	MT_OSVEL_30			0x40000000	/* 速度指令のMAX値  2^30 */
#define MT_MAX_ACCR 		0x1FFFFFFF  /* ACCR,DECR[ref/cyc^2] 最大値 2^30-1 */
#define MT_MAX_ACCR_VEL		0x7FFFFFFF  /* ACCR,DECR[ref/cyc^2] 最大値 2^31-1 (速度制御時) */
#define MT_LONG_MAX			0x7FFFFFFF 

/* TREF MODE */
#define TREFMODE_NON		0
#define TREFMODE_TLIM1		1
#define TREFMODE_TFF		2
#define TREFMODE_TLIM2		3
#define TREFMODE_TFF_TLIM	4

/* VLIM */
#define VLIM_MODE_DISABLE	0
#define VLIM_MODE_ENABLE	1

/* HOLD MODE */
#define HOLD_MODE_RSLOWDOWN_STOP	0
#define HOLD_MODE_RAPID_STOP		1

/* define : モニタ選択コード  */
#define SELMON_FBPOS64L		0xE
#define SELMON_FBPOS64H		0xF


/* enums : モーションタイプ */
enum MOTIONTYPE {
	MTYPE_NONE = 0,				/* 0:モーション停止中 */ 
	MTYPE_POSING,				/* 1:位置決め */ 
	MTYPE_EXPOSING,				/* 2:外部位置決め */ 
	MTYPE_HOMING,				/* 3:原点復帰 */
	MTYPE_HOMING_WITH_DEC,		/* 4:原点復帰(DECあり) */
	MTYPE_FEED,					/* 5:定速送り */
	MTYPE_EXFEED,				/* 6:外部入力位置決め定速送り */
	MTYPE_HOLD,					/* 7:停止 */
	MTYPE_INTERPOL,				/* 8:補間送り */ 
	MTYPE_VELCTRL,				/* 9:速度制御 */
	MTYPE_VELCTRL_WITH_ACCR,	/* 10:速度制御(加減速指定あり) */
	MTYPE_TRQCTRL,				/* 11:トルク制御 */
	MTYPE_FOLLOW,				/* 12:フォローアップ */
	MTYPE_SPOSING,				/* 13:S字位置決め */
	MTYPE_SHOLD,				/* 14:S字停止 */
};

/* enums : モーション状態 */
enum MT_STATE {
	MSTAT_DISABLE = 0,			/* 0:Disable状態 */
	MSTAT_WAITPG_RDY,			/* 1:PG RDY待ち状態 */
	MSTAT_READY,				/* 2:Ready状態 */
	MSTAT_RUN,					/* 3:Run状態 */
	MSTAT_WAIT_SVOFF,			/* 4:サーボOFF待ち */
	MSTAT_RAMP_STOP,			/* 5:サーボOFF減速停止状態 */
};

/* enums : モニタ選択番号 */
enum MCI_M3STD_MONITOR {
	M3STD_APOS_MON = 0,			/* 0: APOS */
	M3STD_CPOS_MON,				/* 1: CPOS */
	M3STD_PERR_MON,				/* 2: PERR */
	M3STD_LPOS1_MON,			/* 3: LPOS1 */
	M3STD_LPOS2_MON,			/* 4: LPOS2 */
	M3STD_FSPD_MON,				/* 5: FSPD */
	M3STD_CSPD_MON,				/* 6: CSPD */
	M3STD_TRQ_MON,				/* 7: TRQ */
	M3STD_ALM_DTL_MON,			/* 8: ALM_DTL */
	M3STD_MPOS_MON,				/* 9: MPOS */
	M3STD_DUMMY2_MON,			/* A: ---- */
	M3STD_DUMMY3_MON,			/* B: ---- */
	M3STD_CMN1_MON,				/* C: CMN1 */
	M3STD_CMN2_MON,				/* D: CMN2 */
	M3STD_OMN1_MON,				/* E: OMN1 */
	M3STD_OMN2_MON,				/* F: OMN2 */
};

enum MCI_M2_MONITOR {
	M2_POS_MON = 0,				/* 0: POS */
	M2_MPOS_MON,				/* 1: MPOS */
	M2_PERR_MON,				/* 2: PERR */
	M2_APOS_MON,				/* 3: APOS */
	M2_LPOS_MON,				/* 4: LPOS */
	M2_IPOS_MON,				/* 5: IPOS */
	M2_TPOS_MON,				/* 6: TPOS */
	M2_DUMMY1_MON,				/* 7: ---- */
	M2_FSPD_MON,				/* 8: FSPD */
	M2_CSPD_MON,				/* 9: CSPD */
	M2_TSPD_MON,				/* a: TSPD */
	M2_TRQ_MON,					/* b: TRQ */
	M2_DUMMY2_MON,				/* c: ---- */
	M2_DUMMY3_MON,				/* d: ---- */
	M2_OMN1_MON,				/* e: OMN1 */
	M2_OMN2_MON,				/* f: OMN2 */
};

/* enums : 共通モニタ選択番号 */
enum MCI_COMMON_MONITOR {
	TPOS_MON = 0,				/* 0: TPOS */
	IPOS_MON,					/* 1: IPOS */
	POS_OFST_MON,				/* 2: POS_OFSET */
	TSPD_MON,					/* 3: TSPD */
	SPD_LIM_MON,				/* 4: SPD_LIM */
	TRQ_LIM_MON,				/* 5: TRQ_LIM */
	SV_STAT_MON,				/* 6: SV_STAT */
	RESERVE_MON,				/* 7: Reserve */
	INIT_PGPOS_L_MON,			/* 8: INIT_PGPOS(Low) */
	INIT_PGPOS_H_MON,			/* 9: INIT_PGPOS(High)*/
};

/* enums : 共通モニタ選択番号 */
enum MCI_OPTION_MONITOR {
	LST_LPOS1_MON = 0x80,		/* 0x80: ラッチ位置LPOS1前回値 								*/
	LST_LPOS2_MON,				/* 0x81: ラッチ位置LPOS2前回値 								*/
	SEQ_LATCH_STS = 0x84,		/* 0x84: 連続ラッチステータス 								*/
/* <S1D7>:Start */
	HIGH_ORDER32_POS 		,	/* 0x85: 指令座標系の指令位置(フィルタ後)	上位32Bitデータ	*/
	HIGH_ORDER32_MPOS,			/* 0x86: 指令位置							上位32Bitデータ	*/
	HIGH_ORDER32_PERR,			/* 0x87: 位置偏差							上位32Bitデータ	*/
	HIGH_ORDER32_APOS,			/* 0x88: 機械座標系フィードバック位置		上位32Bitデータ	*/
	HIGH_ORDER32_LPOS1,			/* 0x89: 機械座標系フィードバックラッチ位置	上位32Bitデータ	*/
	HIGH_ORDER32_LPOS2,			/* 0x8A: 機械座標系フィードバックラッチ位置	上位32Bitデータ	*/
	HIGH_ORDER32_IPOS,			/* 0x8B: 指令座標系指令位置(フィルタ前)		上位32Bitデータ	*/
	HIGH_ORDER32_TPOS,			/* 0x8C: 指令座標系目標位置					上位32Bitデータ	*/
/* <S1D7>:End */
};

/* enums : I/Oビット割付位置 */
enum MCI_SGNLBIT {
	P_OT_SGNL_BNO=0,
	N_OT_SGNL_BNO,
	DEC_SGNL_BNO,
	PA_SGNL_BNO,
	PB_SGNL_BNO,
	PC_SGNL_BNO,
	EXT1_SGNL_BNO,
	EXT2_SGNL_BNO,
	EXT3_SGNL_BNO,
	BK_SGNL_BNO,
	EMG_SGNL_BNO,
};
#define	P_OT_SGNL_MSK	0x0001
#define	N_OT_SGNL_MSK	0x0002
#define	DEC_SGNL_MSK	0x0004
#define	PA_SGNL_MSK		0x0008
#define	PB_SGNL_MSK		0x0010
#define	PC_SGNL_MSK		0x0020
#define	EXT1_SGNL_MSK	0x0040
#define	EXT2_SGNL_MSK	0x0080
#define	EXT3_SGNL_MSK	0x0100
#define	BK_SGNL_MSK		0x0200
#define	EMG_SGNL_MSK	0x0400
#define	PA_SGNL_SFT		3



#define MOTION_API_H_
#endif /* MOTION_API_H_ */




