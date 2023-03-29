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
/* ���C���R�}���h��͊֐���` */
INT32 stdCmd_NO_DEFINE( void *hNetData );		/* ����`�R�}���h��͏���					*/
INT32 stdCmd_NO_SUPP( void *hNetData );			/* ���T�|�[�g�R�}���h��͏���				*/
INT32 stdCmd_NOP( void *hNetData );				/* NOP�R�}���h��͏���						*/
INT32 stdCmd_ID_RD( void *hNetData );			/* ID_RD�R�}���h��͏���					*/
INT32 stdCmd_CONFIG( void *hNetData );			/* CONFIG�R�}���h��͏���					*/
INT32 stdCmd_ALM_RD( void *hNetData );			/* ALM_RD�R�}���h��͏���					*/
INT32 stdCmd_ALM_CLR( void *hNetData );			/* ALM_CLR�R�}���h��͏���					*/
INT32 stdCmd_SYNC_SET( void *hNetData );		/* SYNC_SET�R�}���h��͏���					*/
INT32 stdCmd_CONNECT( void *hNetData );			/* CONNECT�R�}���h��͏���					*/
INT32 stdCmd_DISCONNECT( void *hNetData );		/* DISCONNECT�R�}���h��͏���				*/
INT32 stdCmd_MEM_RD( void *hNetData );			/* MEM_RD�R�}���h��͏���					*/
INT32 stdCmd_MEM_WR( void *hNetData );			/* MEM_WR�R�}���h��͏���					*/
INT32 stdCmd_POS_SET( void *hNetData );			/* POS_SET�R�}���h��͏���					*/
INT32 stdCmd_BRK_ON( void *hNetData );			/* BRK_ON�R�}���h��͏���					*/
INT32 stdCmd_BRK_OFF( void *hNetData );			/* BRK_OFF�R�}���h��͏���					*/
INT32 stdCmd_SENS_ON( void *hNetData );			/* SENS_ON�R�}���h��͏���					*/
INT32 stdCmd_SENS_OFF( void *hNetData );		/* SENS_OFF�R�}���h��͏���					*/
INT32 stdCmd_SMON( void *hNetData );			/* SMON�R�}���h��͏���						*/
INT32 stdCmd_SV_ON( void *hNetData );			/* SV_ON�R�}���h��͏���					*/
INT32 stdCmd_SV_OFF( void *hNetData );			/* SV_OFF�R�}���h��͏���					*/
INT32 stdCmd_INTERPOLATE( void *hNetData );		/* INTERPOLATE�R�}���h��͏���				*/
INT32 stdCmd_POSING( void *hNetData );			/* POSING�R�}���h��͏���					*/
INT32 stdCmd_FEED( void *hNetData );			/* FEED�R�}���h��͏���						*/
INT32 stdCmd_EX_FEED( void *hNetData );			/* EX_FEED�R�}���h��͏���					*/
INT32 stdCmd_EX_POSING( void *hNetData );		/* EX_POSING�R�}���h��͏���				*/
INT32 stdCmd_ZRET( void *hNetData );			/* ZERT�R�}���h��͏���						*/
INT32 stdCmd_VELCTRL( void *hNetData );			/* VELCTRL�R�}���h��͏���					*/
INT32 stdCmd_TRQCTRL( void *hNetData );			/* TRQCTRL�R�}���h��͏���					*/
INT32 stdCmd_SVPRM_RD( void *hNetData );		/* SVPRM_RD�R�}���h��͏���					*/
INT32 stdCmd_SVPRM_WR( void *hNetData );		/* SVPRM_WR�R�}���h��͏���					*/
INT32 stdCmd_SPOSING( void *hNetData );			/* SPOSING�R�}���h��͏���					*/

/* �T�u�R�}���h��͊֐���` */
INT32 stdExCmd_NO_DEFINE( void *hNetData );		/* ����`�R�}���h��͏���					*/
INT32 stdExCmd_NO_SUPP( void *hNetData );		/* ���T�|�[�g�R�}���h��͏���				*/
INT32 stdExCmd_NOP( void *hNetData );			/* NOP�R�}���h��͏���						*/
INT32 stdExCmd_ALM_RD( void *hNetData );		/* ALM_RD�R�}���h��͏���					*/
INT32 stdExCmd_ALM_CLR( void *hNetData );		/* ALM_CLR�R�}���h��͏���					*/
INT32 stdExCmd_MEM_RD( void *hNetData );		/* MEM_RD�R�}���h��͏���					*/
INT32 stdExCmd_MEM_WR( void *hNetData );		/* MEM_WR�R�}���h��͏���					*/
INT32 stdExCmd_SMON( void *hNetData );			/* SMON�R�}���h��͏���						*/
INT32 stdExCmd_SVPRM_RD( void *hNetData );		/* SVPRM_RD�R�}���h��͏���					*/
INT32 stdExCmd_SVPRM_WR( void *hNetData );		/* SVPRM_WR�R�}���h��͏���					*/






#define M3STD_CMD_PROC_H_
#endif /* M3STD_CMD_PROC_H_ */



