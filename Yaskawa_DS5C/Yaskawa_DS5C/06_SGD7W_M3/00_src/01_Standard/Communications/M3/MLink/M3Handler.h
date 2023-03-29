/****************************************************************************
Description	: M3通信処理用HNDL定義
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------

****************************************************************************/
#ifndef M3_HANDLER_H_


#include "CmdHandler.h"
#include "RspHandler.h"
#include "ComManager.h"
#include "RegMngr.h"
#include "ComAlmMngr.h"
#include "MtCtrl.h"


/* Define's */
#define	SYNC_WAIT_TIME 32	/* <S052> */

/* Structure of the network node common State */
typedef enum ML_HWSYNC_STATE {
	ML_HWSYNC_DISABLE = 0, 
	ML_HWSYNC_ENABLE,
	ML_HWSYNC_START, 
	ML_HWSYNC_WAIT_TIME,
	ML_HWSYNC_RESET,
	ML_HWSYNC_ENCODER,
	ML_HWSYNC_INTERRUPT,
	ML_HWSYNC_WAIT_COMP,
}ML_HWSYNC_STATE;

typedef enum ML_COMRST_STATE {
	ML_COMRST_NONE = 0,
	ML_COMRST_START,
	ML_COMRST_DONE,
}ML_COMRST_STATE;

typedef struct ML_NODE_COMMONS {
	UINT			Tcyc_RspTiming;	
	UINT			Tcyc_Counter;
	UINT			TcycEnd_Timing;
	UINT			Tcyc_ScanA_Counter;
//	USHORT			SyncWaitTime;			/* <S052> */

	ULONG			M3IntCounter;
	ULONG			GblFrameErr;
	
	ML_HWSYNC_STATE SyncState;
	ML_COMRST_STATE	ComResetSeq;
	
	BOOL			InterruptReq;
	BOOL			SyncExe;				/* <S1DB> */
} ML_NODE_COMMONS;


/* Structure of the M-3 control handle for each axis */
typedef struct MLNET_HANDLE {
	CMD_HNDL				cmd_hdl;		/* CMD Managerハンドル */
	COM_HNDL				com_hdl;		/* COM Managerハンドル */
	RSP_HNDL				rsp_hdl;		/* RSP Managerハンドル */
	REG_HNDL				reg_hdl;		/* REG Managerハンドル */
	ALM_HNDL				alm_hdl;		/* ALM Managerハンドル */
	MT_HNDL					mt_hdl;			/* Motionハンドル	   */
	
	ML_NODE_COMMONS			*NodeCommons;	/* node共通情報 */
} MLNET_HANDLE;


#define M3_HANDLER_H_
#endif



