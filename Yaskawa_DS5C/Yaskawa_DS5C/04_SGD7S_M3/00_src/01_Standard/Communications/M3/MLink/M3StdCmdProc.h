/****************************************************************************
Description	: Definition for MECHATROLINK Command Analyzer
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : Mercury

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------

****************************************************************************/
#ifndef M3STD_CMD_PROC_H_

//#include "M3Handler.h"
//#include "ComAlmMngr.h"
//#include "ComManager.h"
//#include "SysApi.h"
//#include "MtApi.h"
//#include "CmdAnalyzer.h"
//#include "MLib.h"


/****************************************************************************
	Command functions
****************************************************************************/
/* メインコマンド解析関数定義 */
INT32 stdCmd_NO_DEFINE( void *hNetData );		/* 未定義コマンド解析処理					*/
INT32 stdCmd_NO_SUPP( void *hNetData );			/* 未サポートコマンド解析処理				*/
INT32 stdCmd_NOP( void *hNetData );				/* NOPコマンド解析処理						*/
INT32 stdCmd_ID_RD( void *hNetData );			/* ID_RDコマンド解析処理					*/
INT32 stdCmd_CONFIG( void *hNetData );			/* CONFIGコマンド解析処理					*/
INT32 stdCmd_ALM_RD( void *hNetData );			/* ALM_RDコマンド解析処理					*/
INT32 stdCmd_ALM_CLR( void *hNetData );			/* ALM_CLRコマンド解析処理					*/
INT32 stdCmd_SYNC_SET( void *hNetData );		/* SYNC_SETコマンド解析処理					*/
INT32 stdCmd_CONNECT( void *hNetData );			/* CONNECTコマンド解析処理					*/
INT32 stdCmd_DISCONNECT( void *hNetData );		/* DISCONNECTコマンド解析処理				*/
INT32 stdCmd_MEM_RD( void *hNetData );			/* MEM_RDコマンド解析処理					*/
INT32 stdCmd_MEM_WR( void *hNetData );			/* MEM_WRコマンド解析処理					*/
INT32 stdCmd_POS_SET( void *hNetData );			/* POS_SETコマンド解析処理					*/
INT32 stdCmd_BRK_ON( void *hNetData );			/* BRK_ONコマンド解析処理					*/
INT32 stdCmd_BRK_OFF( void *hNetData );			/* BRK_OFFコマンド解析処理					*/
INT32 stdCmd_SENS_ON( void *hNetData );			/* SENS_ONコマンド解析処理					*/
INT32 stdCmd_SENS_OFF( void *hNetData );		/* SENS_OFFコマンド解析処理					*/
INT32 stdCmd_SMON( void *hNetData );			/* SMONコマンド解析処理						*/
INT32 stdCmd_SV_ON( void *hNetData );			/* SV_ONコマンド解析処理					*/
INT32 stdCmd_SV_OFF( void *hNetData );			/* SV_OFFコマンド解析処理					*/
INT32 stdCmd_INTERPOLATE( void *hNetData );		/* INTERPOLATEコマンド解析処理				*/
INT32 stdCmd_POSING( void *hNetData );			/* POSINGコマンド解析処理					*/
INT32 stdCmd_FEED( void *hNetData );			/* FEEDコマンド解析処理						*/
INT32 stdCmd_EX_FEED( void *hNetData );			/* EX_FEEDコマンド解析処理					*/
INT32 stdCmd_EX_POSING( void *hNetData );		/* EX_POSINGコマンド解析処理				*/
INT32 stdCmd_ZRET( void *hNetData );			/* ZERTコマンド解析処理						*/
INT32 stdCmd_VELCTRL( void *hNetData );			/* VELCTRLコマンド解析処理					*/
INT32 stdCmd_TRQCTRL( void *hNetData );			/* TRQCTRLコマンド解析処理					*/
INT32 stdCmd_SVPRM_RD( void *hNetData );		/* SVPRM_RDコマンド解析処理					*/
INT32 stdCmd_SVPRM_WR( void *hNetData );		/* SVPRM_WRコマンド解析処理					*/
INT32 stdCmd_SPOSING( void *hNetData );			/* SPOSINGコマンド解析処理					*/

/* サブコマンド解析関数定義 */
INT32 stdExCmd_NO_DEFINE( void *hNetData );		/* 未定義コマンド解析処理					*/
INT32 stdExCmd_NO_SUPP( void *hNetData );		/* 未サポートコマンド解析処理				*/
INT32 stdExCmd_NOP( void *hNetData );			/* NOPコマンド解析処理						*/
INT32 stdExCmd_ALM_RD( void *hNetData );		/* ALM_RDコマンド解析処理					*/
INT32 stdExCmd_ALM_CLR( void *hNetData );		/* ALM_CLRコマンド解析処理					*/
INT32 stdExCmd_MEM_RD( void *hNetData );		/* MEM_RDコマンド解析処理					*/
INT32 stdExCmd_MEM_WR( void *hNetData );		/* MEM_WRコマンド解析処理					*/
INT32 stdExCmd_SMON( void *hNetData );			/* SMONコマンド解析処理						*/
INT32 stdExCmd_SVPRM_RD( void *hNetData );		/* SVPRM_RDコマンド解析処理					*/
INT32 stdExCmd_SVPRM_WR( void *hNetData );		/* SVPRM_WRコマンド解析処理					*/






#define M3STD_CMD_PROC_H_
#endif /* M3STD_CMD_PROC_H_ */



