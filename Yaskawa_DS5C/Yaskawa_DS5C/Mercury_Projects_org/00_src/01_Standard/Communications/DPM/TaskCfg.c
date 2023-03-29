/****************************************************************************/
/*																			*/
/*		Task Configration ファイル											*/
/*																			*/
/****************************************************************************/
/*																			*/
/*	Target model: SigmaV Command-Option	ServoDrive							*/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/****************************************************************************/
/*	History:																*/
/*	2007.9.12	created														*/
/*																			*/
/*																			*/
/************** Copyright(c) Yaskawa Electric Corporation *******************/


/* includes */
#include "SysData.h"
#include "UtilityLib.h"
#include "HardSw.h"
#include "OpTasks.h"
#include "NetMsgDrv.h"

/* globals */
#if defined(__ghs__)
#pragma ghs startzda
#elif !defined(_WIN32)
#pragma section sidata begin
#endif

void	(*OpiCalcInitParam)(void);		/* Initial Parameter Calculation	*/
void	(*OpiStartDevice)(void);		/* Start Option Devices 			*/
void	(*OpiIntSync)(void);			/* Option Interrupt Procedure 		*/
void	(*OpiPreCtrlProcedure)(void);	/* Management Before Control Process*/
void	(*OpiPostCtrlProcedure)(void);	/* Management After Control Process	*/
void	(*OpiScanNProcedure)(void);		/* Option ScanN Procedure			*/
void	(*OpiScanCProcedure)(void);		/* Option ScanC Procedure			*/
void	(*OpiRoundProcedure)(void);		/* Option Round Procedure			*/

LONG	(*NdiReqRecvMsg)(UCHAR);					/* Request Op-MSG Recv 	*/
LONG	(*NdiGetRecvMsgSts)(UCHAR, USHORT *, USHORT*);	/* Get Op-MSG Recv Sts	*/
void	(*NdiReadMsgData)(UCHAR, USHORT *, USHORT);	/* Get Op-MSG Recv Data	*/
void	(*NdiWriteMsgData)(UCHAR, USHORT *, USHORT);/* Set Op-MSG Send Data	*/
LONG	(*NdiReqSendMsg)(UCHAR);					/* Request Op-MSG Send 	*/
LONG	(*NdiGetSendMsgSts)(UCHAR);					/* Get Op-MSG Send Sts	*/
LONG	(*NdiSendMsgAbort)(UCHAR);					/* Abort message */

LONG (*OpiGetDegOpState)(DEGOP_PRMRNG *);					/* Get DegOp. state */
LONG (*OpiReqDegOpPrmRead)(USHORT, USHORT);					/* Request DegOp. read */
LONG (*OpiReqDegOpPrmWrite)(USHORT, USHORT, LONG );			/* Request DegOp. write */
LONG (*OpiGetDegOpPrmValue)(USHORT *, DEGOP_ATTR *, LONG *);/* Get DegOp. answer */
LONG (*OpiGetSCMsgState)(void);								/* Get SC Message state */
LONG (*OpiReqSCMsgTrans)(USHORT *, LONG);					/* Request SC Message Trans */
LONG (*OpiGetSCMsgRspData)(USHORT *, LONG *);				/* Get SC Message answer */
void (*OpiCancelSCMsgReq)(void);							/* Cancel SC Message request */
/*------------- <V685> Add Begin -------------------------------------------*/
USHORT (*OpiGetDegOpNumOfFn)(void);					/* Get Number of Option Function Mode	*/
LONG (*OpiReqDegOpFn)(USHORT, USHORT, USHORT);			/* Request DegOp. Fn List */
LONG (*OpiGetDegOpFn)(DGOP_RSP *, DGOPEX_RSP *);		/* Get DegOp. Fn List */
/*------------- <V685> Add End ---------------------------------------------*/

static ULONG	mLOpCardIniTimer;					/* A.E00検出用タイマ */
static LONG		mLIniTimerStart;
#if defined(__ghs__)
#pragma ghs endzda
#elif !defined(_WIN32)
#pragma section sidata end
#endif


/* forward declarations */
//void OpiInitialize(void);
//void OpiInitDevice(void);
static void MLOpiCalcInitParam_CmOp(void);
static void MLOpiStartDevice_CmOp(void);
static void MLOpiScanCProcedure_CmOp(void);
static LONG MLOpiGetDegOpState(DEGOP_PRMRNG *pPrmRange);
static LONG MLOpiReqDegOpPrmRead(USHORT cmd, USHORT no);
static LONG MLOpiReqDegOpPrmWrite(USHORT no, USHORT size, LONG data);
static LONG MLOpiGetDegOpPrmValue(USHORT *no, DEGOP_ATTR *Attr, LONG *data);
static void OpiDummyProc(void);
/*------------- <V685> Add Begin -------------------------------------------*/
static USHORT MLOpiGetDegOpNumOfFn(void);
static LONG MLOpiReqDegOpFn(USHORT cmd, USHORT idx, USHORT data);
static LONG MLOpiGetDegOpFn(DGOP_RSP *pDgopRsp, DGOPEX_RSP *pDgopExRsp);
/*------------- <V685> Add End ---------------------------------------------*/







/****************************************************************************
****************************************************************************/
/* M2 Command-Option type Special Functions  */
/****************************************************************************
****************************************************************************/

/* M-2 IF ScanC Procedure */
static void MLOpiScanCProcedure_CmOp(void)
{
	USHORT sts_wk = 0;
	USHORT wrk;

/* UART driver Procedure */
	OpxSCSifMsgManager();
/* Initial Handshake Timeout Check */
	if(mLIniTimerStart != FALSE)
	{
		if( KpiGetLongTimer(&mLOpCardIniTimer) > 10000 )
		{/* 初回CONFIG未完時 */
			KPI_ALARMSET(ALM_OPCARD_TOUT);	/* A.E00 */
			/* オプション検出情報(E51A)に通信異常(7)をセット*/
			KpiSetOptDetectInfo( CARDTYPE_CMPOP, STS_DET_ERR );
			KpiRstLongTimer(&mLOpCardIniTimer);
		}
	}

/* LED Display */
	sts_wk = MciGetInputSignals(GetMtHndl(0));
	sts_wk |= MciGetSoftLimitStatus(GetMtHndl(0));
	sts_wk &= ((1 << LED_POT_BNO) | (1 << LED_NOT_BNO) | (1 << LED_HBB_BNO));

	wrk = (USHORT)( 
					( (ComGetPhase(GetComHndl(0)) > 1)	&&
					  (mLIniTimerStart == FALSE)		&&
					  (ComChkRecvSts(GetComHndl(0)) == COM_RCV_NOERR_STS)
					) ? 0x1: 0x0
				  );
	sts_wk |= (wrk << LED_CNCT_BNO);

	sts_wk |= ((USHORT)MciGetServoOnStatus(GetMtHndl(0)) << LED_SVON_BNO);
	sts_wk |= ((USHORT)MciGetDenStatus(GetMtHndl(0)) << LED_DEN_BNO);
	sts_wk |= (((1+(USHORT)MciGetZspdStatus(GetMtHndl(0)))&1) << LED_TGON_BNO);

	HwDisplayLedData(sts_wk, AlmGetCode(GetAlmHndl(0)));
}


#ifdef	__ghs__
#pragma ghs section text=".textEX"
#endif

/* M-2 IF Start-up Device Procedure */
static void MLOpiStartDevice_CmOp(void)
{
	MLOpiStartDevice();
	OpxInitSCSerialMsg();

	OpiCalcInitParam = &MLOpiCalcInitParam_CmOp;
	KpiRstLongTimer(&mLOpCardIniTimer);
	mLIniTimerStart = TRUE;

}

/* M-2 IF Initial Parameter Calculation */
static void MLOpiCalcInitParam_CmOp(void)
{
	OPINFO_DATA opInfo = {0};
	OPPRMDAT *pPRM = GetPRMPtr(0);

	opInfo.OpType = pPRM->OpCardID;				/* Option I/f Type */
	opInfo.OpYNumber = pPRM->OpCardYNo;			/* Option Y Number */
	opInfo.ManufactureDate = pPRM->OpCardDate;	/* Manufacture Date */
	opInfo.OpVer = ((ULONG)pPRM->OpCardSwVerH<<16) | pPRM->OpCardSwVerL;	/* Option Module Version */
	UtilMemCpy(opInfo.OpModelStr, pPRM->OpCardModel, 16);	/* Option Module Name */
	UtilMemCpy(opInfo.SerialNoStr, pPRM->OpCardSNo, 16);		/* Option Serial Number */
/* Set Option Card Info. to Register data */	
	KpxSetCommandOptID(&opInfo);
	mLIniTimerStart = FALSE;

	MLOpiCalcInitParam();

	OpxEnableSCSif(pPRM->OpCardCtrlSw);
}

/* M-2 DegOp. operation API */
static LONG MLOpiGetDegOpState(DEGOP_PRMRNG *pPrmRange){ return DEGOP_REQ_ERR;}
static LONG MLOpiReqDegOpPrmRead(USHORT cmd, USHORT no){ return DEGOP_REQ_ERR;}
static LONG MLOpiReqDegOpPrmWrite(USHORT no, USHORT size, LONG data){ return DEGOP_REQ_ERR;}
static LONG MLOpiGetDegOpPrmValue(USHORT *no, DEGOP_ATTR *Attr, LONG *data){ return DEGOP_REQ_ERR;}
/*------------- <V685> Add Begin -------------------------------------------*/
static USHORT MLOpiGetDegOpNumOfFn(void){ return (0);}
static LONG MLOpiReqDegOpFn(USHORT cmd, USHORT idx, USHORT data){ return DEGOP_REQ_ERR;}
static LONG MLOpiGetDegOpFn(DGOP_RSP *pDgopRsp, DGOPEX_RSP *pDgopExRsp){ return DEGOP_REQ_ERR;}
/*------------- <V685> Add End ---------------------------------------------*/
static LONG	MLOpiSendMsgAbort(UCHAR ch){ return 0;}					/* Abort message */// <DPM_V103>

#ifdef	__ghs__
#pragma ghs section text=default
#endif




/****************************************************************************
****************************************************************************/
/* M2/DPM Common Functions  */
/****************************************************************************
****************************************************************************/
static void OpiDummyProc(void){ ;}

#ifdef	__ghs__
#pragma ghs section text=".textEX"
#endif

/****************************************************************************
 OPTIONモジュール初期化処理
****************************************************************************/
void OpiInitialize(void)
{
/* Initialize M2 Device */
	MLOpiInitialize();
}


/****************************************************************************
 OPTIONモジュールデバイス初期化(Task 切り替え)処理
****************************************************************************/
void OpiInitDevice(void)
{
	USHORT rdcnt;
	USHORT swset[2]={0};
	OPPRMDAT *pPRM = GetPRMPtr(0);
	USHORT cardType = KPI_GET_CMDOP_TYPE();

	switch(cardType)
	{
	case CMDOP_M2_IF:	/* M-2 IF */
	/* Set Interrupt Vector for Option Procedure */
		OpiCalcInitParam = &MLOpiCalcInitParam;			/* Initial Parameter Calculation*/
		OpiStartDevice = &MLOpiStartDevice_CmOp;			/* Start Option Devices 		*/
		OpiIntSync = &MLOpiIntSync;						/* Option Interrupt Procedure 	*/
		OpiPreCtrlProcedure = &MLOpiPreCtrlProcedure;	/* Option ScanB Before Process */
		OpiPostCtrlProcedure = &MLOpiPostCtrlProcedure;	/* Option ScanB After Process	*/
		OpiScanNProcedure = &MLOpiScanNProcedure;		/* Option ScanN Procedure	*/
		OpiScanCProcedure = &MLOpiScanCProcedure_CmOp;	/* Option ScanC Procedure	*/
		OpiRoundProcedure = &MLOpiRoundProcedure;		/* Option Round Procedure	*/
	/* Set Message Driver Funciton for Option Procedure */
		NdiReqRecvMsg = &NdReqRecvMsg;					/* Request Op-MSG Recv 	*/
		NdiGetRecvMsgSts = &NdGetRecvMsgSts;			/* Get Op-MSG Recv Sts	*/
		NdiReadMsgData = &NdReadMsgData;				/* Get Op-MSG Recv Data	*/
		NdiWriteMsgData = &NdWriteMsgData;				/* Set Op-MSG Send Data	*/
		NdiReqSendMsg = &NdReqSendMsg;					/* Request Op-MSG Send 	*/
		NdiGetSendMsgSts = &NdGetSendMsgSts;			/* Get Op-MSG Send Sts	*/
		NdiSendMsgAbort = &MLOpiSendMsgAbort;			/* Abort message */// <DPM_V103>
	/* DegOp. operation API */
		OpiGetDegOpState = &MLOpiGetDegOpState;		/* Get DegOp. state */
		OpiReqDegOpPrmRead = &MLOpiReqDegOpPrmRead;	/* Request DegOp. read */
		OpiReqDegOpPrmWrite = &MLOpiReqDegOpPrmWrite;/* Request DegOp. write */
		OpiGetDegOpPrmValue = &MLOpiGetDegOpPrmValue;/* Get DegOp. answer */
/*------------- <V685> Add Begin -------------------------------------------*/
		OpiGetDegOpNumOfFn = &MLOpiGetDegOpNumOfFn;	/* Get Number of Option Function Mode */
		OpiReqDegOpFn = &MLOpiReqDegOpFn;	/* Request DegOp. Function Mode */
		OpiGetDegOpFn = &MLOpiGetDegOpFn;	/* Get DegOp. Function Mode */
/*------------- <V685> Add End ---------------------------------------------*/
	/* SC Message. operation API */
		OpiGetSCMsgState = &MLOpiGetSCMsgState;		/* Get SC Message state */
		OpiReqSCMsgTrans = &MLOpiReqSCMsgTrans;		/* Request SC Message Trans */
		OpiGetSCMsgRspData = &MLOpiGetSCMsgRspData;	/* Get SC Message answer */
		OpiCancelSCMsgReq = &MLOpiCancelSCMsgReq;

		for(rdcnt = 0; rdcnt < 4; rdcnt++)
		{
			if(KpiGetInitM2SwInfo(&swset[0],&swset[1]) != FALSE)
			{	break;}
		}
		pPRM->OpCardSvSw = (swset[1] << 4) | (swset[0]&0xF);
		MLOpiInitDevice();
		KPX_INIT_SYNC_TIMER(KnlV.SyncChkTime, CREG_M2CAP);
		break;

	case CMDOP_DPM_NETXIF:
	case CMDOP_DPM_CMNIF:	/* DPM IF */
	/* Set Interrupt Vector for Option Procedure */
		OpiCalcInitParam = &MLOpiCalcInitParam;		/* Initial Parameter Calculation*/
		OpiStartDevice = &DPiStartDevice;			/* Start Option Devices 		*/
		OpiIntSync = &DPiIntSync;					/* Option Interrupt Procedure 	*/
		OpiPreCtrlProcedure = &DPiPreCtrlProcedure;	/* Option ScanB Before Process */
		OpiPostCtrlProcedure = &DPiPostCtrlProcedure;/* Option ScanB After Process	*/
		OpiScanNProcedure = &OpiDummyProc;			/* Option ScanN Procedure	*/
		OpiScanCProcedure = &DPiScanCProcedure;		/* Option ScanC Procedure	*/
		OpiRoundProcedure = &DPiRoundProcedure;		/* Option Round Procedure	*/
	/* Set Message Driver Funciton for Option Procedure */
		NdiReqRecvMsg = &DPiReqRecvCSMsg;			/* Request Op-MSG Recv 	*/
		NdiGetRecvMsgSts = &DPiGetRecvCSMsgSts;		/* Get Op-MSG Recv Sts	*/
		NdiReadMsgData = &DPiReadCSMsgData;			/* Get Op-MSG Recv Data	*/
		NdiWriteMsgData = &DPiWriteCSMsgData;		/* Set Op-MSG Send Data	*/
		NdiReqSendMsg = &DPiReqSendCSMsg;			/* Request Op-MSG Send 	*/
		NdiGetSendMsgSts = &DpiGetSendCSMsgSts;		/* Get Op-MSG Send Sts	*/
		NdiSendMsgAbort = &DpiSendMsgAbort;			/* Abort message */ // <DPM_V103>
	/* DegOp. operation API */
		OpiGetDegOpState = &DPiGetDegOpState;		/* Get DegOp. state */
		OpiReqDegOpPrmRead = &DPiReqDegOpPrmRead;	/* Request DegOp. read */
		OpiReqDegOpPrmWrite = &DPiReqDegOpPrmWrite;	/* Request DegOp. write */
		OpiGetDegOpPrmValue = &DPiGetDegOpPrmValue;	/* Get DegOp. answer */
/*------------- <V685> Add Begin -------------------------------------------*/
		OpiGetDegOpNumOfFn = &DpiGetDegOpNumOfFn;	/* Get Number of Option Function Mode */
		OpiReqDegOpFn = &DpiReqDegOpFn;		/* Request DegOp. Function Mode */
		OpiGetDegOpFn = &DpiGetDegOpFn;		/* Request DegOp. Function Mode */
/*------------- <V685> Add End ---------------------------------------------*/
	/* SC Message. operation API */
		OpiGetSCMsgState = &DPiGetSCMsgState;		/* Get SC Message state */
		OpiReqSCMsgTrans = &DPiReqSCMsgTrans;		/* Request SC Message Trans */
		OpiGetSCMsgRspData = &DPiGetSCMsgRspData;	/* Get SC Message answer */
		OpiCancelSCMsgReq = &DPiCancelSCMsgReq;

	/* Initialize DPM Device */
		DPiInitialize();
		KPX_INIT_SYNC_TIMER(KnlV.SyncChkTime, CREG_DPCAP);
		break;

	default:	/* No-Supported Card */
	/* Set Interrupt Vector for Option Procedure */
		OpiCalcInitParam = &MLOpiCalcInitParam;		/* Initial Parameter Calculation*/
		OpiStartDevice = &OpiDummyProc;				/* Start Option Devices 		*/
		OpiIntSync = &OpiDummyProc;					/* Option Interrupt Procedure 	*/
		OpiPreCtrlProcedure = &DPiPreCtrlProcedure;	/* Option ScanB Before Process */
		OpiPostCtrlProcedure = &DPiPostCtrlProcedure;/* Option ScanB After Process	*/
		OpiScanNProcedure = &OpiDummyProc;			/* Option ScanN Procedure	*/
		OpiScanCProcedure = &DPiScanCProcedure;		/* Option ScanC Procedure	*/
		OpiRoundProcedure = &OpiDummyProc;			/* Option Round Procedure	*/
	/* Set Message Driver Funciton for Option Procedure */
		NdiReqRecvMsg = &DPiReqRecvCSMsg;			/* Request Op-MSG Recv 	*/
		NdiGetRecvMsgSts = &DPiGetRecvCSMsgSts;		/* Get Op-MSG Recv Sts	*/
		NdiReadMsgData = &DPiReadCSMsgData;			/* Get Op-MSG Recv Data	*/
		NdiWriteMsgData = &DPiWriteCSMsgData;		/* Set Op-MSG Send Data	*/
		NdiReqSendMsg = &DPiReqSendCSMsg;			/* Request Op-MSG Send 	*/
		NdiGetSendMsgSts = &DpiGetSendCSMsgSts;		/* Get Op-MSG Send Sts	*/
		NdiSendMsgAbort = &DpiSendMsgAbort;			/* Abort message */// <DPM_V103>
	/* DegOp. operation API */
		OpiGetDegOpState = &DPiGetDegOpState;		/* Get DegOp. state */
		OpiReqDegOpPrmRead = &DPiReqDegOpPrmRead;	/* Request DegOp. read */
		OpiReqDegOpPrmWrite = &DPiReqDegOpPrmWrite;	/* Request DegOp. write */
		OpiGetDegOpPrmValue = &DPiGetDegOpPrmValue;	/* Get DegOp. answer */
/*------------- <V685> Add Begin -------------------------------------------*/
		OpiGetDegOpNumOfFn = &DpiGetDegOpNumOfFn;	/* Get Number of Option Function Mode */
		OpiReqDegOpFn = &DpiReqDegOpFn;		/* Request DegOp. Function Mode */
		OpiGetDegOpFn = &DpiGetDegOpFn;		/* Request DegOp. Function Mode */
/*------------- <V685> Add End ---------------------------------------------*/
	/* SC Message. operation API */
		OpiGetSCMsgState = &DPiGetSCMsgState;		/* Get SC Message state */
		OpiReqSCMsgTrans = &DPiReqSCMsgTrans;	/* Request SC Message Trans */
		OpiGetSCMsgRspData = &DPiGetSCMsgRspData;	/* Get SC Message answer */
		OpiCancelSCMsgReq = &DPiCancelSCMsgReq;

	/* Initialize DPM Device */
		DPiInitialize();
		break;
	}
}
#ifdef	__ghs__
#pragma ghs section text=default
#endif







