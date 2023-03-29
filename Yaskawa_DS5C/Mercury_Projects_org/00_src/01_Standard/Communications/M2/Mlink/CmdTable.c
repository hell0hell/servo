/****************************************************************************/
/*                                                                          */
/*            MECHATROLINK サーボ通信ＩＦアプリ 通信コンポーネント          */
/*                                                                          */
/*						コマンド管理テーブル								*/
/*																			*/
/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/************** Copyright Yaskawa Electric Corporation **********************/

/* includes */
#include "Basedef.h"
#include "CmdAnalyzer.h"
#include "RspManager.h"
#include "CmdProc.h"
#include "RspProc.h"
#include "CmdTable.h"
#include "UtilityLib.h" /* <V715> */


/****************************************************************************/
/* 					コマンド管理テーブル									*/
/****************************************************************************/
/* 概要																		*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/

/* メインコマンド */
const CMD_DEF_TBL CmdTbl[] = /* <V715> *//*<V730>Mod:Start*/
{
	{TIMEOUT_OF_NO_DEFINE,	2,	0,	Idx_ExNO_DEFINE,	FALSE,	cmd_NO_DEFINE,	rsp_SYS_CMD},
	{TIMEOUT_OF_NOP,		1,	0,	Idx_ExNOP,			FALSE,	cmd_NOP,		rsp_NOP},
	{TIMEOUT_OF_PRM_RD,		2,	0,	Idx_ExPRM_RD,		TRUE,	cmd_PRM_RD,		rsp_PRM_RD},
	{TIMEOUT_OF_PRM_WR,		2,	0,	Idx_ExPRM_WR,		TRUE,	cmd_PRM_WR,		rsp_SYS_CMD},
	{TIMEOUT_OF_ID_RD,		2,	0,	Idx_ExNO_DEFINE,	FALSE,	cmd_ID_RD,		rsp_ID_RD},
	{TIMEOUT_OF_CONFIG,		2,	0,	Idx_ExNO_DEFINE,	TRUE,	cmd_CONFIG,		rsp_NOP},
	{TIMEOUT_OF_ALM_RD,		2,	0,	Idx_ExALM_RD,		TRUE,	cmd_ALM_RD,		rsp_ALM_RD},
	{TIMEOUT_OF_ALM_CLR,	2,	0,	Idx_ExNO_DEFINE,	TRUE,	cmd_ALM_CLR,	rsp_ALM_CLR},
	{TIMEOUT_OF_SYNC_SET,	2,	0,	Idx_ExNO_DEFINE,	TRUE,	cmd_SYNC_SET,	rsp_NOP},
	{TIMEOUT_OF_CONNECT,	1,	0,	Idx_ExNO_DEFINE,	TRUE,	cmd_CONNECT,	rsp_CONNECT},
	{TIMEOUT_OF_DISCONNECT,	1,	0,	Idx_ExNO_DEFINE,	FALSE,	cmd_DISCONNECT,	rsp_NOP},
	{TIMEOUT_OF_NO_DEFINE,	2,	0,	Idx_ExPPRM_RD,		FALSE,	cmd_NO_SUPP,	rsp_SYS_CMD},/* PPRM_RD */
	{TIMEOUT_OF_PPRM_WR,	2,	0,	Idx_ExPPRM_WR,		TRUE,	cmd_PPRM_WR,	rsp_SYS_CMD},
	{TIMEOUT_OF_NO_DEFINE,	2,	0,	Idx_ExNO_DEFINE,	FALSE,	cmd_MEM_RD,		rsp_SYS_CMD},/* MEM_RD */
	{TIMEOUT_OF_NO_DEFINE,	2,	0,	Idx_ExNO_DEFINE,	FALSE,	cmd_NO_DEFINE,	rsp_SYS_CMD},/* MEM_WR */
	{TIMEOUT_OF_POS_SET,	2,	0,	Idx_ExNO_DEFINE,	TRUE,	cmd_POS_SET,	rsp_POS_SET},
	{TIMEOUT_OF_BRK_ON,		2,	0,	Idx_ExNO_DEFINE,	TRUE,	cmd_BRK_ON,		rsp_MON_CMD},
	{TIMEOUT_OF_BRK_OFF,	2,	0,	Idx_ExNO_DEFINE,	TRUE,	cmd_BRK_OFF,	rsp_MON_CMD},
	{TIMEOUT_OF_SENS_ON,	2,	0,	Idx_ExNO_DEFINE,	TRUE,	cmd_SENS_ON,	rsp_MON_CMD},
	{TIMEOUT_OF_SENS_OFF,	2,	0,	Idx_ExNO_DEFINE,	TRUE,	cmd_SENS_OFF,	rsp_MON_CMD},
	{TIMEOUT_OF_HOLD,		2,	0,	Idx_ExNO_DEFINE,	FALSE,	cmd_HOLD,		rsp_MON_CMD},
	{TIMEOUT_OF_NO_DEFINE,	2,	0,	Idx_ExNO_DEFINE,	FALSE,	cmd_NO_SUPP,	rsp_SYS_CMD},/* MLOCK_ON */
	{TIMEOUT_OF_NO_DEFINE,	2,	0,	Idx_ExNO_DEFINE,	FALSE,	cmd_NO_SUPP,	rsp_SYS_CMD},/* MLOCK_OFF */
	{TIMEOUT_OF_LTMOD_ON,	2,	0,	Idx_ExLTMOD_ON,		TRUE,	cmd_LTMOD_ON,	rsp_MON_CMD},
	{TIMEOUT_OF_LTMOD_OFF,	2,	0,	Idx_ExLTMOD_OFF,	TRUE,	cmd_LTMOD_OFF,	rsp_MON_CMD},
	{TIMEOUT_OF_SMON,		2,	0,	Idx_ExSMON,			FALSE,	cmd_SMON,		rsp_MON_CMD},
	{TIMEOUT_OF_SV_ON,		2,	0,	Idx_ExNO_DEFINE,	FALSE,	cmd_SV_ON,		rsp_MON_CMD},
	{TIMEOUT_OF_SV_OFF,		2,	0,	Idx_ExNO_DEFINE,	FALSE,	cmd_SV_OFF,		rsp_MON_CMD},
	{TIMEOUT_OF_INTERPOLATE,3,	1,	Idx_ExNO_DEFINE,	FALSE,	cmd_INTERPOLATE,rsp_MON_CMD},
	{TIMEOUT_OF_POSING,		2,	1,	Idx_ExNO_DEFINE,	FALSE,	cmd_POSING,		rsp_MON_CMD},
	{TIMEOUT_OF_FEED,		2,	1,	Idx_ExNO_DEFINE,	FALSE,	cmd_FEED,		rsp_MON_CMD},
	{TIMEOUT_OF_LATCH,		3,	1,	Idx_ExNO_DEFINE,	FALSE,	cmd_LATCH,		rsp_MON_CMD},
	{TIMEOUT_OF_EX_POSING,	2,	1,	Idx_ExNO_DEFINE,	FALSE,	cmd_EX_POSING,	rsp_MON_CMD},
	{TIMEOUT_OF_ZRET,		2,	1,	Idx_ExNO_DEFINE,	FALSE,	cmd_ZRET,		rsp_MON_CMD},
	{TIMEOUT_OF_VELCTRL,	2,	1,	Idx_ExNO_DEFINE,	FALSE,	cmd_VELCTRL,	rsp_MON_CMD},
	{TIMEOUT_OF_TRQCTRL,	2,	1,	Idx_ExNO_DEFINE,	FALSE,	cmd_TRQCTRL,	rsp_MON_CMD},
	{TIMEOUT_OF_ADJ,		2,	0,	Idx_ExNO_DEFINE,	TRUE,	cmd_ADJ,		rsp_ADJ},
	{TIMEOUT_OF_SVCTRL,		2,	1,	Idx_ExNO_DEFINE,	FALSE,	cmd_SVCTRL,		rsp_MON_CMD},
};										  /*<V730>Mod:End  */

/* サブコマンド */
const EX_CMD_DEF_TBL ExCmdTbl[] = /* <V715> */
{
	{TIMEOUT_OF_NO_DEFINE,	2,	FALSE,	exCmd_NO_DEFINE,	exRsp_SYS_CMD},
	{TIMEOUT_OF_NOP,		1,	FALSE,	exCmd_NOP,			exRsp_NOP},
	{TIMEOUT_OF_PRM_RD,		2,	TRUE,	exCmd_PRM_RD,		exRsp_PRM_RD},
	{TIMEOUT_OF_PRM_WR,		2,	TRUE,	exCmd_PRM_WR,		exRsp_SYS_CMD},
	{TIMEOUT_OF_ALM_RD,		2,	TRUE,	exCmd_ALM_RD,		exRsp_ALM_RD},
	{TIMEOUT_OF_NO_DEFINE,	2,	TRUE,	exCmd_NO_SUPP,		exRsp_SYS_CMD},
	{TIMEOUT_OF_PPRM_WR,	2,	TRUE,	exCmd_PPRM_WR,		exRsp_SYS_CMD},
	{TIMEOUT_OF_LTMOD_ON,	2,	TRUE,	exCmd_LTMOD_ON,		exRsp_MON_CMD},
	{TIMEOUT_OF_LTMOD_OFF,	2,	TRUE,	exCmd_LTMOD_OFF,	exRsp_MON_CMD},
	{TIMEOUT_OF_SMON,		2,	FALSE,	exCmd_SMON,			exRsp_MON_CMD},
};

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
const UCHAR CmdIdx[] = /* <V715> */
{	/* 0x00 ～ 0x0F */
	Idx_NOP,		Idx_PRM_RD,		Idx_PRM_WR,		Idx_ID_RD,
	Idx_CONFIG,		Idx_ALM_RD,		Idx_ALM_CLR,	Idx_NO_DEFINE,
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	Idx_NO_DEFINE,	Idx_SYNC_SET,	Idx_CONNECT,	Idx_DISCONNECT,
	/* 0x10 ～ 0x1F */
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_PPRM_RD,
	Idx_PPRM_WR,	Idx_MEM_RD,		Idx_MEM_WR,		Idx_NO_DEFINE,
	/* 0x20 ～ 0x2F */
	Idx_POS_SET,	Idx_BRK_ON,		Idx_BRK_OFF,	Idx_SENS_ON,
	Idx_SENS_OFF,	Idx_HOLD,		Idx_MLOCK_ON,	Idx_MLOCK_OFF,
	Idx_LTMOD_ON,	Idx_LTMOD_OFF,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	/* 0x30 ～ 0x3F */
	Idx_SMON,		Idx_SV_ON,		Idx_SV_OFF,		Idx_NO_DEFINE,
	Idx_INTERPOLATE,Idx_POSING,		Idx_FEED,		Idx_NO_DEFINE,
	Idx_LATCH,		Idx_EX_POSING,	Idx_ZRET,		Idx_NO_DEFINE,
	Idx_VELCTRL,	Idx_TRQCTRL,	Idx_ADJ,		Idx_SVCTRL,
	/* 0x40 ～ 0x4F */
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	
	/* 0x50 ～ 0x5F */
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	/* 0x60 ～ 0x6F */
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	/* 0x70 ～ 0x7F */
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	/* 0x80 ～ 0x8F */
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	/* 0x90 ～ 0x9F */
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	/* 0xA0 ～ 0xAF */
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	/* 0xB0 ～ 0xBF */
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	/* 0xC0 ～ 0xCF */
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	/* 0xD0 ～ 0xDF */
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	/* 0xE0 ～ 0xEF */
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	/* 0xF0 ～ 0xFF */
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,
	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE,	Idx_NO_DEFINE
};

/****************************************************************************/
/* 		コマンドテーブルポインタ取得										*/
/* 			M2にﾌﾟﾛﾌｧｲﾙﾀｲﾌﾟは関係無い。										*/
/****************************************************************************/
void CmdSetCMDTablePointer(void **IdxTblPtr, void **CmdTblPtr, void **ExCmdTblPtr)
{
	*IdxTblPtr = (void*)CmdIdx;
	*CmdTblPtr = (void*)CmdTbl;
	*ExCmdTblPtr = (void*)ExCmdTbl;
}



