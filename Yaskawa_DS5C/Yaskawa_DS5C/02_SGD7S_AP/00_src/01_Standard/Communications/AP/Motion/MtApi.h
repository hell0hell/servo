/****************************************************************************************************/
/*																									*/
/*																									*/
/*		MtApi.h : Motion API header file															*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	Histroy:																						*/
/*		Rev2.01 2007.12.14	加速度指令追加															*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.1.00 : 2012.08.28  K.Ozaki																*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#ifndef		MOTION_API_H_
#define		MOTION_API_H_

#include "Basedef.h"
#include "error_no.h"

/****************************************************************************************************/
/*																									*/
/*		Global Definition																			*/
/*																									*/
/****************************************************************************************************/
#define	MCI_WAIT_COMP		ERR_WAIT_COMP
#define	MCI_SUCCESS			ERR_NONE
#define	MCI_CNDTN_ERR		(0x100 | ERR_CMD_CNDTN)
#define	MCI_EMGBB_ERR		(0x200 | ERR_CMD_CNDTN)
#define	MCI_SVRDY_ERR		(0x300 | ERR_CMD_CNDTN)
#define	MCI_LTCNFLCT_ERR	(0x400 | ERR_CMD_CNFLCT)
#define	MCI_SPD_ERR			(0x500 | ERR_DATA_SET)
#define	MCI_LTMOD_ERR		(0x600 | ERR_DATA_LTMOD)

/*--------------------------------------------------------------------------------------------------*/
/* defines : ラッチモード(API設定用) */
/*--------------------------------------------------------------------------------------------------*/
#if 0
#define MOTION_LTMD			1	/* モーションラッチモード */
#define MOTION_MDL_LTMD		2	/* モーションラッチモード(SVCTRL) */
#define MODAL_LTMD			3	/* モーダルラッチモード(Normal) */
#endif
/*--------------------------------------------------------------------------------------------------*/
/* defines : ラッチモード(内部処理用) */
/*--------------------------------------------------------------------------------------------------*/
#if 0
#define INTERPOL_LTMD		4	/* モーションラッチラッチモード(INTERPOLATE) */
#define EXPOSING_LTMD		5	/* モーションラッチラッチモード(EXPOSING) */
#define ZRET_LTMD			6	/* モーションラッチラッチモード(ZRET) */
//#define ZRET_LOCK_LTMD		7	/* モーションラッチラッチモード(ZRET中) */
#define NONE_LTMD			0	/* ラッチモードなし */
#endif

/*--------------------------------------------------------------------------------------------------*/
/* defines : 制御モード */
/*--------------------------------------------------------------------------------------------------*/
#if 0
#define MT_NO_CTRLMD	0		/* 制御無効モード */
#define MT_POS_CTRLMD	1		/* 位置制御モード */
#define MT_VEL_CTRLMD	2		/* 速度制御モード */
#define MT_TRQ_CTRLMD	3		/* トルク制御モード */
#endif

/*--------------------------------------------------------------------------------------------------*/
/* enums : モーションタイプ */
/*--------------------------------------------------------------------------------------------------*/
#if 0
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
#endif

/*--------------------------------------------------------------------------------------------------*/
/* enums : モーション状態 */
/*--------------------------------------------------------------------------------------------------*/
enum MT_STATE {
	MSTAT_DISABLE=0,	/* Disable状態 */
	MSTAT_WAITPG_RDY,	/* PG RDY待ち状態 */
	MSTAT_READY,		/* Ready状態 */
	MSTAT_RUN,			/* Run状態 */
	MSTAT_WAIT_SVOFF,	/* サーボOFF待ち */
	MSTAT_RAMP_STOP,	/* サーボOFF減速停止状態 */
};

/*--------------------------------------------------------------------------------------------------*/
/* enums : モニタ選択番号 */
/*--------------------------------------------------------------------------------------------------*/
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

/*--------------------------------------------------------------------------------------------------*/
/* enums : I/Oビット割付位置 */
/*--------------------------------------------------------------------------------------------------*/
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

/*--------------------------------------------------------------------------------------------------*/
/* typedefs: struct */
/* モーション指令構造体定義 */
/*--------------------------------------------------------------------------------------------------*/
#if 0
typedef struct MCI_CMD_ {
	LONG		TPos;		/* 目標位置[ref] */
	LONG		TSpeed;		/* 目標速度[ref/s] */
	LONG		Velocity;	/* VFF/速度指令 [ref/s],[2^30/MAX] */
	LONG		Trq;		/* トルク指令[MAX_TRQ/2^30] */
	ULONG		TrqLimitP;	/* 正側トルク制限[MAX_TRQ/2^30] */
	ULONG		TrqLimitN;	/* 負側トルク制限[MAX_TRQ/2^30] */
	LONG		AccRate;	/* 加速定数<Rev2.01> */
	LONG		DecRate;	/* 減速定数<Rev2.01> */
	ULONG		Option;		/* オプション指令 */
	UCHAR		HoldMod;	/* 停止方法選択 0: 減速停止1,1:急速停止,2減速停止2 */
	UCHAR		LatchSgnl;	/* ラッチ信号選択 0:C相,n:EXTn */
	BOOL		EnableRate;	/* 加速度指令有効<Rev2.01> */
	BOOL		RstReq;		/* モーションリセット要求 */
} MCI_CMD;
#endif

#ifndef		MOTION_CTRL_H_

#if 0
#ifndef __MTHNDL
#define __MTHNDL
typedef struct MTHNDL_	MTHNDL;
#endif
#ifndef __MTPRM
#define __MTPRM
typedef struct MTPRM_	MTPRM;
#endif
#ifndef __SV_OPTION_IF
#define __SV_OPTION_IF
typedef struct SV_OPTION_IF_	SV_OPTION_IF;
#endif
#endif

#endif	/* MOTION_CTRL_H_ */

/****************************************************************************************************/
/*																									*/
/*		Motion Control APIs																			*/
/*																									*/
/****************************************************************************************************/
#if 0
#ifdef __cplusplus
extern	"C"	{
#endif
/*--------------------------------------------------------------------------------------------------*/
/* Motion操作API */
/*--------------------------------------------------------------------------------------------------*/
extern INT32 MciSetInterpolateCmd(MTHNDL* hdl, MCI_CMD* cmd);
extern INT32 MciSetLatchCmd(MTHNDL* hdl, MCI_CMD* cmd);
extern INT32 MciSetPosingCmd(MTHNDL* hdl, MCI_CMD* cmd);
extern INT32 MciSetExPosingCmd(MTHNDL* hdl, MCI_CMD* cmd);
extern INT32 MciSetHomingCmd(MTHNDL* hdl, MCI_CMD* cmd);
extern INT32 MciSetFeedCmd(MTHNDL* hdl, MCI_CMD* cmd);
extern INT32 MciSetHoldCmd(MTHNDL* hdl, MCI_CMD* cmd);
extern INT32 MciSetVelCmd(MTHNDL* hdl, MCI_CMD* cmd);
extern INT32 MciSetTrqCmd(MTHNDL* hdl, MCI_CMD* cmd);
extern VOID MciSetOptionCmd(MTHNDL* hdl, ULONG option);

extern INT32 MciEnableState(MTHNDL* hdl, BOOL enable);
extern INT32 MciResetMotion(MTHNDL* hdl);
extern INT32 MciSetServoOn(MTHNDL* hdl, BOOL enable, BOOL reset);
extern INT32 MciSetSensOn(MTHNDL* hdl, BOOL enable);

extern INT32 MciRequestMtLatch(MTHNDL* hdl, UCHAR mode, UCHAR signl);
extern INT32 MciRequestMdlLatch(MTHNDL* hdl, UCHAR mode, UCHAR signl);
extern INT32 MciCancelLatch(MTHNDL* hdl, UCHAR mode);
extern INT32 MciSetCordinatePos(MTHNDL* hdl, LONG data, BOOL ref);

extern VOID MciOutputSignals(MTHNDL* hdl, USHORT signl);
extern VOID MciOutputBkSignal(MTHNDL* hdl, BOOL enable);

/*--------------------------------------------------------------------------------------------------*/
/* Motion状態取得API */
/*--------------------------------------------------------------------------------------------------*/
extern UCHAR MciGetMType(MTHNDL* hdl);
extern BOOL MciGetServoOnStatus(MTHNDL* hdl);
extern BOOL MciGetSensOnStatus(MTHNDL* hdl);
extern UCHAR MciGetCtrlMode(MTHNDL* hdl);
extern LONG MciGetMonitorData(MTHNDL* hdl, USHORT sel, UCHAR unit);
extern BOOL MciGetDenStatus(MTHNDL* hdl);
extern BOOL MciGetLatchStatus(MTHNDL* hdl ,UCHAR *ltEdge);
extern USHORT MciGetLatchSeqStatus(MTHNDL* hdl);

extern BOOL MciGetInPosStatus(MTHNDL* hdl);
extern BOOL MciGetNearStatus(MTHNDL* hdl);
extern BOOL MciGetZpointStatus(MTHNDL* hdl);
extern UCHAR MciGetSoftLimitStatus(MTHNDL* hdl);
extern LONG MciGetOutputCmdDist(MTHNDL* hdl);

extern BOOL MciGetZspdStatus(MTHNDL* hdl);
extern BOOL MciGetMainPowerStatus(MTHNDL* hdl);
extern BOOL MciGetVlimitStatus(MTHNDL* hdl);
extern BOOL MciGetTrqLimitStatus(MTHNDL* hdl);
extern BOOL MciGetVcmpStatus(MTHNDL* hdl);

extern USHORT MciGetInputSignals(MTHNDL* hdl);
extern USHORT MciGetInPortSignals(MTHNDL* hdl);

#if ( CCSW_SFTID == SFTID_EXIST )						/*<V717>Add:Start*/
extern USHORT MciGetSomStatus(void);
#endif													/*<V717>Add:End  */

#ifdef __cplusplus
}
#endif
#endif


#endif /* MOTION_API_H_ */
