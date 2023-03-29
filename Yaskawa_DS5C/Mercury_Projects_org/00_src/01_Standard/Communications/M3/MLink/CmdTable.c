/****************************************************************************
Description	: MECHATROLINK Command Table
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : Mercury

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------

****************************************************************************/
#include "Basedef.h"
#include "CmdTable.h"
#include "M3StdCmdProc.h"
#include "M3StdRspProc.h"


/****************************************************************************/
/* 					コマンド管理テーブル									*/
/****************************************************************************/
/* 概要																		*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
/* 標準サーボプロファイル */
const CMD_DEF_TBL StdCmdTbl[] =
{
	{TIMEOUT_OF_NO_DEFINE,	2,	Std_Idx_ExNO_DEFINE,	FALSE,	stdCmd_NO_DEFINE,	stdRsp_SYS_CMD},
	{TIMEOUT_OF_NOP,		1,	Std_Idx_ExNOP,			FALSE,	stdCmd_NOP,			stdRsp_NOP    },
	{TIMEOUT_OF_NO_DEFINE,	2,	Std_Idx_ExPRM_RD,		FALSE,	stdCmd_NO_SUPP,		stdRsp_SYS_CMD},/* PRM_RD */
	{TIMEOUT_OF_NO_DEFINE,	2,	Std_Idx_ExPRM_WR,		FALSE,	stdCmd_NO_SUPP,		stdRsp_SYS_CMD},/* PRM_WR */
	{TIMEOUT_OF_ID_RD,		2,	Std_Idx_ExNO_DEFINE,	TRUE,	stdCmd_ID_RD,		stdRsp_ID_RD  },
	{TIMEOUT_OF_CONFIG,		2,	Std_Idx_ExNO_DEFINE,	TRUE,	stdCmd_CONFIG,		stdRsp_CONFIG },
	{TIMEOUT_OF_ALM_RD,		2,	Std_Idx_ExALM_RD,		TRUE,	stdCmd_ALM_RD,		stdRsp_ALM_RD},
	{TIMEOUT_OF_ALM_CLR,	2,	Std_Idx_ExALM_CLR,		TRUE,	stdCmd_ALM_CLR,		stdRsp_ALM_CLR},
	{TIMEOUT_OF_SYNC_SET,	2,	Std_Idx_ExNO_DEFINE,	TRUE,	stdCmd_SYNC_SET,	stdRsp_NOP    },
	{TIMEOUT_OF_CONNECT,	1,	Std_Idx_ExNO_DEFINE,	TRUE,	stdCmd_CONNECT,		stdRsp_CONNECT},
	{TIMEOUT_OF_DISCONNECT,	1,	Std_Idx_ExNO_DEFINE,	FALSE,	stdCmd_DISCONNECT,	stdRsp_NOP    },
	{TIMEOUT_OF_NO_DEFINE,	2,	Std_Idx_ExPPRM_RD,		FALSE,	stdCmd_NO_SUPP,		stdRsp_SYS_CMD},/* PPRM_RD */
	{TIMEOUT_OF_NO_DEFINE,	2,	Std_Idx_ExPPRM_WR,		FALSE,	stdCmd_NO_SUPP,		stdRsp_SYS_CMD},/* PPRM_WR */
	{TIMEOUT_OF_MEM_RD,		2,	Std_Idx_ExMEM_RD,		TRUE,	stdCmd_MEM_RD,		stdRsp_MEM_RD },
	{TIMEOUT_OF_MEM_WR,		2,	Std_Idx_ExMEM_WR,		TRUE,	stdCmd_MEM_WR,		stdRsp_SYS_CMD},
	{TIMEOUT_OF_POS_SET,	2,	Std_Idx_ExNO_DEFINE,	TRUE,	stdCmd_POS_SET,		stdRsp_POS_SET},
	{TIMEOUT_OF_BRK_ON,		2,	Std_Idx_ExNO_DEFINE,	TRUE,	stdCmd_BRK_ON,		stdRsp_MON_CMD},
	{TIMEOUT_OF_BRK_OFF,	2,	Std_Idx_ExNO_DEFINE,	TRUE,	stdCmd_BRK_OFF,		stdRsp_MON_CMD},
	{TIMEOUT_OF_SENS_ON,	2,	Std_Idx_ExNO_DEFINE,	TRUE,	stdCmd_SENS_ON,		stdRsp_MON_CMD},
	{TIMEOUT_OF_SENS_OFF,	2,	Std_Idx_ExNO_DEFINE,	TRUE,	stdCmd_SENS_OFF,	stdRsp_MON_CMD},
	{TIMEOUT_OF_SMON,		2,	Std_Idx_ExSMON,			FALSE,	stdCmd_SMON,		stdRsp_MON_CMD},
	{TIMEOUT_OF_SV_ON,		2,	Std_Idx_ExNO_DEFINE,	FALSE,	stdCmd_SV_ON,		stdRsp_MON_CMD},
	{TIMEOUT_OF_SV_OFF,		2,	Std_Idx_ExNO_DEFINE,	FALSE,	stdCmd_SV_OFF,		stdRsp_MON_CMD},
	{TIMEOUT_OF_INTERPOLATE,3,	Std_Idx_ExNO_DEFINE,	FALSE,	stdCmd_INTERPOLATE,	stdRsp_MON_CMD},
	{TIMEOUT_OF_POSING,		2,	Std_Idx_ExNO_DEFINE,	FALSE,	stdCmd_POSING,		stdRsp_MON_CMD},
	{TIMEOUT_OF_FEED,		2,	Std_Idx_ExNO_DEFINE,	FALSE,	stdCmd_FEED,		stdRsp_MON_CMD},
	{TIMEOUT_OF_EX_FEED,	2,	Std_Idx_ExNO_DEFINE,	FALSE,	stdCmd_EX_FEED,		stdRsp_MON_CMD},
	{TIMEOUT_OF_EX_POSING,	2,	Std_Idx_ExNO_DEFINE,	FALSE,	stdCmd_EX_POSING,	stdRsp_MON_CMD},
	{TIMEOUT_OF_ZRET,		2,	Std_Idx_ExNO_DEFINE,	FALSE,	stdCmd_ZRET,		stdRsp_MON_CMD},
	{TIMEOUT_OF_VELCTRL,	2,	Std_Idx_ExNO_DEFINE,	FALSE,	stdCmd_VELCTRL,		stdRsp_MON_CMD},
	{TIMEOUT_OF_TRQCTRL,	2,	Std_Idx_ExNO_DEFINE,	FALSE,	stdCmd_TRQCTRL,		stdRsp_MON_CMD},
	{TIMEOUT_OF_SVPRM_RD,	2,	Std_Idx_ExSVPRM_RD,		TRUE,	stdCmd_SVPRM_RD,	stdRsp_SVPRM_RD},
	{TIMEOUT_OF_SVPRM_WR,	2,	Std_Idx_ExSVPRM_WR,		TRUE,	stdCmd_SVPRM_WR,	stdRsp_SVPRM  },
	{TIMEOUT_OF_SPOSING,	2,	Std_Idx_ExNO_DEFINE,	FALSE,	stdCmd_SPOSING,		stdRsp_MON_CMD},
};


/* サブコマンド */
/* 標準サーボプロファイル */
const EX_CMD_DEF_TBL StdExCmdTbl[] =
{
	{TIMEOUT_OF_NO_DEFINE,	2,	FALSE,	stdExCmd_NO_DEFINE,		stdExRsp_SYS_CMD},
	{TIMEOUT_OF_NOP,		1,	FALSE,	stdExCmd_NOP,			stdExRsp_NOP    },
	{TIMEOUT_OF_NO_DEFINE,	2,	TRUE,	stdExCmd_NO_SUPP,		stdExRsp_SYS_CMD},/* PRM_RD */
	{TIMEOUT_OF_NO_DEFINE,	2,	TRUE,	stdExCmd_NO_SUPP,		stdExRsp_SYS_CMD},/* PRM_WR */
	{TIMEOUT_OF_ALM_RD,		2,	TRUE,	stdExCmd_ALM_RD,		stdExRsp_ALM_RD},
	{TIMEOUT_OF_ALM_CLR,	2,	TRUE,	stdExCmd_ALM_CLR,		stdExRsp_ALM_CLR},
	{TIMEOUT_OF_NO_DEFINE,	2,	TRUE,	stdExCmd_NO_SUPP,		stdExRsp_SYS_CMD},/* PPRM_RD */
	{TIMEOUT_OF_NO_DEFINE,	2,	TRUE,	stdExCmd_NO_SUPP,		stdExRsp_SYS_CMD},/* PPRM_WR */
	{TIMEOUT_OF_MEM_RD,		2,	TRUE,	stdExCmd_MEM_RD,		stdExRsp_MEM_RD },
	{TIMEOUT_OF_MEM_WR,		2,	TRUE,	stdExCmd_MEM_WR,		stdExRsp_SYS_CMD},
	{TIMEOUT_OF_SMON,		2,	FALSE,	stdExCmd_SMON,			stdExRsp_MON_CMD},
	{TIMEOUT_OF_SVPRM_RD,	2,	TRUE,	stdExCmd_SVPRM_RD,		stdExRsp_SVPRM_RD},
	{TIMEOUT_OF_SVPRM_WR,	2,	TRUE,	stdExCmd_SVPRM_WR,		stdExRsp_SYS_CMD},
};


/* イベントドリブン通信ID情報取得用プロファイル */
#if 0 /* 現状未対応 */
/* イベントドリブン通信ID情報取得用プロファイル */
const CMD_DEF_TBL _EvtCmdTbl[] =
{
	{TIMEOUT_OF_NO_DEFINE,	2,	Evt_Idx_ExNO_DEFINE, FALSE,	evtCmd_NO_DEFINE,	evtRsp_SYS_CMD},
	{TIMEOUT_OF_NOP,		1,	Evt_Idx_ExNO_DEFINE, FALSE,	evtCmd_NOP,			evtRsp_NOP},
	{TIMEOUT_OF_ID_RD,		2,	Evt_Idx_ExNO_DEFINE, FALSE,	evtCmd_ID_RD,		evtRsp_SYS_CMD},
	{TIMEOUT_OF_CONNECT,	1,	Evt_Idx_ExNO_DEFINE, FALSE,	evtCmd_CONNECT,		evtRsp_CONNECT},
	{TIMEOUT_OF_DISCONNECT,	1,	Evt_Idx_ExNO_DEFINE, FALSE,	evtCmd_DISCONNECT,	evtRsp_NOP},
	{TIMEOUT_OF_MEM_RD,		2,	Evt_Idx_ExNO_DEFINE, FALSE,	evtCmd_MEM_RD,		evtRsp_SYS_CMD},
};

/* イベントドリブン通信ID情報取得用プロファイル */
const EX_CMD_DEF_TBL _EvtExCmdTbl[] =
{
	{TIMEOUT_OF_NO_DEFINE,	2,	FALSE,	evtExCmd_NO_DEFINE,		evtExRsp_NOP},
};
#endif /* #if 0 */



/****************************************************************************/
/* 				コマンド定義indexテーブル									*/
/****************************************************************************/
/* 概要		受信コマンドコードから該当するコマンド定義テーブル(CmdTbl[])	*/
/* 			へのマッピングに利用します。									*/
/*			コマンドの追加／削除がある場合はこのテーブルの変更が必要と		*/
/*			なります。														*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
const UCHAR M3StdCmdIdx[CMD_NUMBER] = 
{	
	/* 標準サーボプロファイル */
	/* 0x00 ～ 0x0F */
	Std_Idx_NOP,		Std_Idx_PRM_RD,		Std_Idx_PRM_WR,		Std_Idx_ID_RD,
	Std_Idx_CONFIG,		Std_Idx_ALM_RD,		Std_Idx_ALM_CLR,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_SYNC_SET,	Std_Idx_CONNECT,	Std_Idx_DISCONNECT,
	/* 0x10 ～ 0x1F */
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_PPRM_RD,
	Std_Idx_PPRM_WR,	Std_Idx_MEM_RD,		Std_Idx_MEM_WR,		Std_Idx_NO_DEFINE,
	/* 0x20 ～ 0x2F */
	Std_Idx_POS_SET,	Std_Idx_BRK_ON,		Std_Idx_BRK_OFF,	Std_Idx_SENS_ON,
	Std_Idx_SENS_OFF,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	/* 0x30 ～ 0x3F */
	Std_Idx_SMON,		Std_Idx_SV_ON,		Std_Idx_SV_OFF,		Std_Idx_NO_DEFINE,
	Std_Idx_INTERPOLATE,Std_Idx_POSING,		Std_Idx_FEED,		Std_Idx_EX_FEED,
	Std_Idx_NO_DEFINE,	Std_Idx_EX_POSING,	Std_Idx_ZRET	,	Std_Idx_NO_DEFINE,
	Std_Idx_VELCTRL,	Std_Idx_TRQCTRL,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	/* 0x40 ～ 0x4F */
	Std_Idx_SVPRM_RD,	Std_Idx_SVPRM_WR,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	
	/* 0x50 ～ 0x5F */
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	/* 0x60 ～ 0x6F */
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	/* 0x70 ～ 0x7F */
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	/* 0x80 ～ 0x8F */
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	/* 0x90 ～ 0x9F */
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	/* 0xA0 ～ 0xAF */
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	/* 0xB0 ～ 0xBF */
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	/* 0xC0 ～ 0xCF */
	Std_Idx_SPOSING,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	/* 0xD0 ～ 0xDF */
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	/* 0xE0 ～ 0xEF */
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	/* 0xF0 ～ 0xFF */
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE
};

const UCHAR M3EvtCmdIdx[CMD_NUMBER] =
{/* <S14D> */
	/* 標準サーボプロファイル */
	/* 0x00 ～ 0x0F */
	Std_Idx_NOP,		Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_ID_RD,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_CONNECT,	Std_Idx_DISCONNECT,
	/* 0x10 ～ 0x1F */
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_MEM_RD,		Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	/* 0x20 ～ 0x2F */
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	/* 0x30 ～ 0x3F */
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	/* 0x40 ～ 0x4F */
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	/* 0x50 ～ 0x5F */
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	/* 0x60 ～ 0x6F */
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	/* 0x70 ～ 0x7F */
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	/* 0x80 ～ 0x8F */
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	/* 0x90 ～ 0x9F */
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	/* 0xA0 ～ 0xAF */
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	/* 0xB0 ～ 0xBF */
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	/* 0xC0 ～ 0xCF */
	Std_Idx_SPOSING,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	/* 0xD0 ～ 0xDF */
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	/* 0xE0 ～ 0xEF */
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	/* 0xF0 ～ 0xFF */
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,
	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE,	Std_Idx_NO_DEFINE
};

/****************************************************************************/
/* 		コマンドテーブルポインタ取得										*/
/****************************************************************************/
void CmdSetCMDTablePointer(UCHAR ProfileType, void **IdxTblPtr, 
								void **CmdTblPtr, void **ExCmdTblPtr)
{
	switch(ProfileType)
	{
	case 0x01:	/* Event driven Profile *//* <S14D> */
		*IdxTblPtr = (void*)M3EvtCmdIdx;
		*CmdTblPtr = (void*)StdCmdTbl;
		*ExCmdTblPtr = (void*)StdExCmdTbl;
		break;

	case 0x00:	/* M2 Compatible Profile */
	case 0x10:	/* M3 Standard Servo Profile */
	default:
		*IdxTblPtr = (void*)M3StdCmdIdx;
		*CmdTblPtr = (void*)StdCmdTbl;
		*ExCmdTblPtr = (void*)StdExCmdTbl;
		break;
	}
}


