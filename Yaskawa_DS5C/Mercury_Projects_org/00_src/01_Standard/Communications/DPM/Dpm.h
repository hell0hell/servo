/****************************************************************************/
/*																			*/
/*		Dual-Port Function Interface 										*/
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
#ifndef		DPM_OPT_H_
#define		DPM_OPT_H_

/****************************************************************************/
/*	DPM Interface Version													*/
/****************************************************************************/
#define DPM_IF_VER	0x0104	//<DPM_V104>

/****************************************************************************/
/* Dual-Port Memory Map														*/
/****************************************************************************/
#define DPM_TOP_ADDR			0x10000000	/* DPM Top Address */
#define DPM_OPINFO_OFS_ADR		0x0020		/* Option Card Infomation */
#define DPM_SVINFO_OFS_ADR		0x0090		/* Servo Infomation */
#define DPM_CYCCMDA_OFS_ADR		0x0100		/* Cyclic Command Data A */
#define DPM_CYCRSPA_OFS_ADR		0x0140		/* Cyclic Responce Data A */
#define DPM_CYCCMDB_OFS_ADR		0x0180		/* Cyclic Command Data B */
#define DPM_CYCRSPB_OFS_ADR		0x01C0		/* Cyclic Responce Data B */
#define DPM_LINKCMD_OFS_ADR		0x0200		/* Link Command Data */
#define DPM_LINKRSP_OFS_ADR		0x0210		/* Link Responce Data */
#define DPM_DOCMD_OFS_ADR		0x0220		/* DegOp. Command Data */
#define DPM_DORSP_OFS_ADR		0x0230		/* DegOp. Responce Data */
#define DPM_CSMSGCMD_OFS_ADR	0x0240		/* CS Message Command Data */
#define DPM_CSMSGRSP_OFS_ADR	0x0340		/* CS Message Responce Data */
#define DPM_SCMSGCMD_OFS_ADR	0x0440		/* SC Message Command Data */
#define DPM_SCMSGRSP_OFS_ADR	0x0540		/* SC Message Responce Data */
#define DPM_DORSPEX_OFS_ADR		0x0640		/* DegOp. Extension Data <V685> */

#define DPM_OPINFO_BSZ			0x0070		/* Option Card Infomation */
#define DPM_SVINFO_BSZ			0x0070		/* Servo Infomation */
#define DPM_CYCDATA_BSZ			0x0040		/* Cyclic Data */
#define DPM_LINKDATA_BSZ		0x0010		/* Link Data */
#define DPM_DODATA_BSZ			0x0010		/* DegOp. Data */
#define DPM_MSGDATA_BSZ			0x0100		/* Message Data */

/****************************************************************************/
/* Cycle Time of Cyclic DataExchage											*/
/****************************************************************************/
#define DPM_TIO_BASE_TIME		500			/* Normal Update Cycle [125us] */
#define DPM_SUPP_CYCLE			{250, 500, 1000, 2000, 4000, 8000, 16000}	/* 62.5,125,250,500,1000,2000,4000 */
#define DPM_DEFAULT_CYCLE		4000		/* default cycle 1000us */
#define DPM_UNDEFINED_CYCLE		0			/* undefined cycle */
#define DPM_HANDSHAKE_TIMEOUT	10000		/* default cycle 10s */
#define DPM_WDCCHK_CNT			16			/* WDCチェック回数 */

/****************************************************************************/
/* Handshake Data															*/
/****************************************************************************/
/* Defines of Handshake Sequence number */
#define DPHNDSHK_IDOL_SEQ		0x0000U		/* IDOL State */
#define DPHNDSHK_INTRDY_SEQ		0x1111U		/* CMSINT check State <DPM_V103>*/
#define DPHNDSHK_START_SEQ		0x2222U		/* Handshake Start State */
#define DPHNDSHK_MEMCHK_SEQ		0x5555U		/* Memory Check State */
#define DPHNDSHK_MEMCLR_SEQ		0x9999U		/* Memory Clear State */
#define DPHNDSHK_ASYNC_SEQ		0xAAAAU		/* Non-Cyclic Exchange State */
#define DPHNDSHK_SYNC_SEQ		0xCCCCU		/* Cyclic Exchange  State */
#define DPHNDSHK_ERR_SEQ		0xEEEEU		/* Error  State */
#define DPHNDSHK_CMDERR			0xEE00U		/* Sequence Error Code */
#define DPHNDSHK_CMDERR_MSK		0x00FFU		/* Sequence Error Code Mask */
/* Defines of Cyclic Control Data */
#define DPCYC_BUFCHG_R			0x0001U		/* Req. of Alt-Cyclic buffer */
#define DPCYC_BUFCHG_ACK		0x0100U		/* Ack. of Alt-Cyclic buffer */
#define DPCYC_BUFCHG_ACK_BIT	8
#define DPCYC_SYNC_REQ			0x2000U		/* task sycn. request(internal only)*/
#define DPCYC_SYNC_ENABLED		0x4000U		/* task sycn. completed status(internal only)*/
#define DPCYC_WDC_ENABLED		0x8000U		/* wdc check enable status(internal only)*/
#define DPCYC_CTRL_MASK			0xFF00U		/* mask value of internal status flags */
/* Defines of Non-Cyclic Control Data */
#define DPNOCYC_RES_REQ			0x0001U		/* Request of Non-Cyclce Exchange */
#define DPNOCYC_RES_COMP		0x0100U		/* Compeleted status of Non-Cyclce Exchange */
#define DPNOCYC_RES_BSY			0x0200U		/* Busy status of Non-Cyclce Exchange */
#define DPNOCYC_ENABLED			0x8000U		/* Enable status of Non-Cyclce Exchange */


/****************************************************************************/
/* Handshake Data															*/
/****************************************************************************/
typedef	struct	tagHNDSHK_DATA {
	USHORT		C_Setup;	/* Request Code of Handshake sequence */
	USHORT		S_Setup;	/* Ack Code of Handshake sequence */
	USHORT		ServoEnd;	/* Handshake data Access Completed counter */
	USHORT		CycCtrl;	/* Control flag for Cyclic data */
	USHORT		LinkCtrl;	/* Control flag for Link data */
	USHORT		DOCtrl;		/* Control flag for DegOp. data */
	USHORT		CSCtrl;		/* Control flag for CS Message data */
	USHORT		SCCtrl;		/* Control flag for SC Message data */
} HNDSHK_DATA;

/****************************************************************************/
/* Option Card Infomation Data												*/
/****************************************************************************/
//typedef OPSET_INFO		OPINFO_DATA;

/****************************************************************************/
/* Servo Infomation Data													*/
/****************************************************************************/
typedef struct tagSVINFO_DATA {
	USHORT	SvIfSuppVer;
	USHORT	dummy1[2];
	USHORT	SvAxisAdr;
	ULONG	SvFirmVer;
	CHAR	dummy2[16];
	USHORT	SvCycTime;
	USHORT	HwSwSet;
}SVINFO_DATA;

/****************************************************************************/
/* Cyclic Infomation Data													*/
/****************************************************************************/
typedef struct tagCYC_CMD {
	USHORT 		C_Chksum;
	USHORT 		dummy;
	CYC_CMD		Cmd;
} CYCCMD_DATA;
typedef struct tagCYC_RSP {
	USHORT 		S_Chksum;
	USHORT 		dummy;
	CYC_RSP		Rsp;
} CYCRSP_DATA;

/****************************************************************************/
/* DegOp. Link Data															*/
/****************************************************************************/
#define DOLNK_MKSUM_WSZ		5
#define DOLNK_CHKSUM_WSZ	6
typedef struct tagDOLNK_STRUCT {
	USHORT	Chksum;
	USHORT	Cmd;
	USHORT	Address;
	USHORT	Len;
	LONG	Data;
	USHORT	dummy_c;
	USHORT	dummy_e;
}DOLNK_STRUCT;

typedef struct tagLINK_DATA {
	DOLNK_STRUCT Cmd;
	DOLNK_STRUCT Rsp;
}DOLINK_DATA;

/****************************************************************************/
/* Message Data																*/
/****************************************************************************/
#define MSG_FMTID_MBUS		0x2		/* Format ID (2:MEMBOBUS) 				*/
#define MSG_FMTID_ERR		0xF		/* Format ID (F:ERROR) 					*/
#define MSG_MINDATA_BSZ		5		/* Minimum Message Data Size [byte] 	*/
typedef struct tagDPMSG_DATA {
	USHORT Cmd[DPM_MSGDATA_BSZ/2];
	USHORT Rsp[DPM_MSGDATA_BSZ/2];
}DPMSG_DATA;


/****************************************************************************/
/* Handshake																*/
/****************************************************************************/
typedef enum ENUM_NONCYC_STATE { 
	Enum_DisableState = 0,	/* DISABLE状態 */
	Enum_IdolState,			/* IDOL(Req待ち)状態 */
	Enum_RecvReqState,		/* Req受信完了状態 */
	Enum_WaitCompState,		/* Busy(処理中)状態 */
	Enum_ReqCompState,		/* 処理完了状態 */
} NONCYC_STATE;

typedef	struct	tagHNDSHK_CTRL {
	USHORT			C_Setup;	/* Request Code of Handshake sequence */
	USHORT			S_Setup;	/* Ack Code of Handshake sequence */
	USHORT			ServoEnd;	/* Handshake data Access Completed counter */
	USHORT			CycCtrl;	/* Control flag for Cyclic data */

	NONCYC_STATE	LnkCtrl;	/* Control state for Link data */
	NONCYC_STATE	DOCtrl;		/* Control state for DegOp. data */
	NONCYC_STATE	CSCtrl;		/* Control state for CS Message data */
	NONCYC_STATE	SCCtrl;		/* Control state for SC Message data */
} HNDSHK_CTRL;



/****************************************************************************/
/* Structure of DPM Control Data											*/
/****************************************************************************/
typedef	struct	tagDPMCTRLIF {
	HNDSHK_DATA		*pHndshk;		/* DPM Handshakeデータポインタ */
	OPINFO_DATA		*pOpInfo;		/* DPM Option Infomationデータポインタ */
	SVINFO_DATA		*pSvInfo;		/* DPM Servo Infomationデータポインタ */
	CYCCMD_DATA		*pCycCmd[2];	/* DPM Cyclic CMDデータポインタ */
	CYCRSP_DATA		*pCycRsp[2];	/* DPM Cyclic RSPデータポインタ */
	DOLINK_DATA		*pLink;			/* DPM Linkデータポインタ */
	DOLINK_DATA		*pDeg;			/* DPM DegOpデータポインタ */
	DPMSG_DATA		*pCSMsg;		/* DPM CS Messageデータポインタ */
	DPMSG_DATA		*pSCMsg;		/* DPM SC Messageデータポインタ */
	DGOPEX_RSP		*pDegEx;		/* DPM DegOp拡張データポインタ <V685> */

	HNDSHK_CTRL		hndshk;			/* Handshakeフラグ */
	ULONG			timerCnt;		/* Handshakeシーケンス用タイマ */
	USHORT			State;			/* Handshakeシーケンス状態 */
	USHORT			tioCycCnt;		/* TIO(ScanB)スケジュールカウンタ */
	USHORT			lstChkSum;		/* 前回チェックサム値 */
	SHORT			wdErrCnt;		/* WDCエラーカウンタ */
	DOLNK_STRUCT	lnkCmd;			/* Linkデータ交換用ワーク */
	SV_OPTION_IF	*pSvOptIf;		/* 内部Option I/F */
} DPMCTRLIF;
typedef	const DPMCTRLIF CDPMCTRLIF;

/****************************************************************************/
/*	Option API Functions Prototype Definition (Called from Servo Control)	*/
/****************************************************************************/
#define DPM_FUNC_SUCCESS		(0)
#define DPM_TIMEOUT_ERR			(-1)
#define DPM_SYNC_ERR			(-2)
#define DPM_CHKSUM_ERR			(-3)

#ifdef __cplusplus
extern	"C"	{
#endif

/* DPM 状態取得＆操作 API */
void DpmInitialize(DPMCTRLIF *pDpmCtl, SV_OPTION_IF *pSvIf);
LONG DpmResetScheduler(DPMCTRLIF *pDpmCtl);
#define DpmResetTioCnt(pDpmCtl)		((pDpmCtl)->tioCycCnt = 0)
#define DpmUpdateTioCnt(pDpmCtl)	((pDpmCtl)->tioCycCnt++)

/* Handshake Function API */
LONG DpmHandshake(DPMCTRLIF *pDpmCtl);
LONG DpmUpdateHndshkFlag(DPMCTRLIF *pDpmCtl);

/* Cyclic Data Exhange Function API */
LONG DpmMakeCycDataUpdateReq(DPMCTRLIF *pDpmCtl);
void DpmWriteCycRSP(DPMCTRLIF *pDpmCtl);
LONG DpmReadCycCMD(DPMCTRLIF *pDpmCtl);
void DpmWriteNonCycRSP(DPMCTRLIF *pDpmCtl);

/* Link Data Exhange Function API */
void DpmLinkRegAcc(DPMCTRLIF *pDpmCtl);

/* CS Message Exhange Function API */
LONG DpmReqRecvCSMsg(DPMCTRLIF *pDpmCtl);
LONG DpmGetRecvCSMsgSts(DPMCTRLIF *pDpmCtl, USHORT *rcvWSz, USHORT *rcvBSz);
void DpmReadCSMsgData(DPMCTRLIF *pDpmCtl, USHORT *rcvData, USHORT rdSz);
void DpmWriteCSMsgData(DPMCTRLIF *pDpmCtl, USHORT *sndData, USHORT wrSz);
LONG DpmReqSendCSMsg(DPMCTRLIF *pDpmCtl);
LONG DpmSendMsgAbort(DPMCTRLIF *pDpmCtl);//<DPM_V103>

/* DegOp. operation API */
LONG DpmGetDegOpState(DPMCTRLIF *pDpmCtl, DEGOP_PRMRNG *pPrmRange);
LONG DpmReqDegOpPrmRead(DPMCTRLIF *pDpmCtl, USHORT cmd, USHORT no);
LONG DpmReqDegOpPrmWrite(DPMCTRLIF *pDpmCtl, USHORT no, USHORT size, LONG data);
LONG DpmGetDegOpPrmValue(DPMCTRLIF *pDpmCtl, USHORT *no, DEGOP_ATTR *Attr, LONG *data);
LONG DpmReqDegOpFn(DPMCTRLIF *pDpmCtl, USHORT cmd, USHORT idx, USHORT data);			/*<V685>*/
LONG DpmGetDegOpFn(DPMCTRLIF *pDpmCtl, DGOP_RSP *pDgopRsp, DGOPEX_RSP *pDgopExRsp);	/*<V685>*/

/* SC Message. operation API */
LONG DpmGetSCMsgState(DPMCTRLIF *pDpmCtl);
LONG DpmReqSCMsgTrans(DPMCTRLIF *pDpmCtl, USHORT *pMsgBuf, LONG bSz);
LONG DpmGetSCMsgRspData(DPMCTRLIF *pDpmCtl, USHORT *pMsgBuf, LONG *bSz);

/* レジスタ操作IF */
void OpiInitRegAcc(void);
LONG OpiReadRegister(HMSGIF Hmsg, LONG RegNo, LONG *Number, USHORT *RdBuf);
LONG OpiWriteRegister(HMSGIF Hmsg, LONG RegNo, LONG Number, USHORT *RdBuf);
LONG OpiReadRegisterBlk(HMSGIF Hmsg, LONG RegNo, LONG *Number, USHORT *RdBuf);
LONG OpiWriteRegisterBlk(HMSGIF Hmsg, LONG RegNo, LONG Number, USHORT *RdBuf);
LONG OpiGetRegWrBusySts(HMSGIF Hmsg);



#ifdef __cplusplus
}
#endif


#endif		/* DPM_OPT_H_ */
