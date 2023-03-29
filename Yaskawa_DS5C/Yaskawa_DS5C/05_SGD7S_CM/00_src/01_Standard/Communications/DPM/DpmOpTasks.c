/****************************************************************************/
/*																			*/
/*		DPM Option Task Control �\�[�X�t�@�C��								*/
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

#define	KPI_IOREG


/* includes */
#include "KnlStd.h"
#include "XlsTblM2Opt.h"
#include "Dpm.h"
#include "HardSw.h"
#include "NetMsgDrv.h"
#include "OpTasks.h"

#ifdef __cplusplus
extern	"C"	{
#endif

/* globals : LOCAL */
#if defined(__ghs__)
#pragma ghs startzda
#elif !defined(_WIN32)
#pragma section sidata begin
#endif

static DPMCTRLIF	DpmCtrl;
static LONG			bScanAStop;

#if defined(__ghs__)
#pragma ghs endzda
#elif !defined(_WIN32)
#pragma section sidata end
#endif
/* forward declarations */
/* Public functions of Option-module main tasks */
//void DPiInitialize(void);
//void DPiStartDevice(void);
//void DPiIntSync(void);
//void DPiPreCtrlProcedure(void);
//void DPiPostCtrlProcedure(void);
//void DPiScanCProcedure(void);
//void DPiRoundProcedure(void);
/* Public functions API of Option-module message driver */
//LONG DPiReqRecvCSMsg(UCHAR ch);
//LONG DPiGetRecvCSMsgSts(UCHAR ch, USHORT *rcvWSz, USHORT *rcvBSz);
//void DPiReadCSMsgData(UCHAR ch, USHORT *rcvData, USHORT rdSz);
//void DPiWriteCSMsgData(UCHAR ch, USHORT *sndData, USHORT wrSz);
//LONG DPiReqSendCSMsg(UCHAR ch);
//LONG DpiGetSendCSMsgSts(UCHAR ch);
/* DPM DegOp. operation API */
//LONG DPiGetDegOpState(void);
//LONG DPiReqDegOpPrmRead(USHORT cmd, USHORT no);
//LONG DPiReqDegOpPrmWrite(USHORT no, USHORT size, LONG data);
//LONG DPiGetDegOpPrmValue(USHORT *no, DEGOP_ATTR *Attr, LONG *data);
/* DPM SC Message. operation API */
//LONG DPiGetSCMsgState(void);
//LONG DPiReqSCMsgTrans(USHORT *pMsgBuf, LONG bSz);
//LONG DPiGetSCMsgRspData(USHORT *pMsgBuf, LONG *bSz);

#ifdef __cplusplus
}
#endif


/****************************************************************************
****************************************************************************/
/* Public functions of Option-module main tasks */
/****************************************************************************
****************************************************************************/

#ifdef	__ghs__
#pragma ghs section text=".textEX"
#endif
/****************************************************************************
 DPM Option���W���[������������
****************************************************************************/
void DPiInitialize(void)
{
/* �T�[�{IF�f�[�^������ */
	MlibResetLongMemory(&SvOptIf, sizeof(SV_OPTION_IF)/4 );

/* ���W�X�^IF������ */
	SvOptIf.Regif = &NetRegif;
	SvOptIf.Msgif = &NetMsgif;
	OpiInitRegAcc();

/* DPM���W���[�������� */
	DpmInitialize(&DpmCtrl, &SvOptIf);

/* LED �\�����W���[�������� */
	HwInitialize();

	bScanAStop = FALSE;
}


/****************************************************************************
 DPM Option���W���[���f�o�C�X�X�^�[�g�A�b�v����
****************************************************************************/
void DPiStartDevice(void)
{
/* ���荞�ݗL�� */
#ifdef KNLDEF_SYNC_PIN_OLD
	KPX_CLR_INTMSK( CPU_ICR_DPSYNC );	/*IntSync�����݃}�X�N����	*/
#endif
	KPX_CLR_INTMSK( CPU_ICR_DPSYNCR );	/*IntSync�����݃}�X�N����	*/

}


/****************************************************************************
 DPM Option���W���[��Round����
****************************************************************************/
void DPiRoundProcedure(void)
{
/* DPM�C�j�V�����V�[�P���X���� */
	if(DpmHandshake(&DpmCtrl) != DPM_FUNC_SUCCESS)
	{/* Handshake�ُ� A.E00 */
		KPI_ALARMSET(ALM_OPCARD_TOUT);
		/* �I�v�V�������o���(E51A)�ɒʐM�ُ�(7)���Z�b�g*/
		KpiSetOptDetectInfo( CARDTYPE_CMPOP, STS_DET_ERR );
	}

/* DPM�����N�f�[�^�������� */
	DpmLinkRegAcc(&DpmCtrl);
}
#ifdef	__ghs__
#pragma ghs section text=default
#endif


/****************************************************************************
 DPM Option���W���[��IntSync����
****************************************************************************/
void DPiIntSync(void)
{
	LONG rc;
/* Handshake�t���O�X�V */
	DpmUpdateHndshkFlag(&DpmCtrl);

/* �T�C�N���b�N�f�[�^�擾 */
	rc = DpmReadCycCMD(&DpmCtrl);
	if (rc != DPM_FUNC_SUCCESS)
	{
		if(rc == DPM_CHKSUM_ERR)
		{	KPI_ALARMSET(ALM_DPM_CHKSUM);/* SUM�ُ� A.E03 */}
		else
		{	KPI_ALARMSET(ALM_WDC_ERR);	/* �����ُ� A.E02 */}
	}

/* ScanA��~������ */
	if(DpmCtrl.pSvOptIf->SvInfo.IOCycle < DPM_TIO_BASE_TIME)
	{
		DpmWriteNonCycRSP(&DpmCtrl);
		if((!KnlV.f.ScanAEnable) || bScanAStop)
		{/* ScanA��~ */
			bScanAStop = (KnlV.f.ScanAEnable != FALSE) ? FALSE: TRUE;
			if((!DpmCtrl.pSvOptIf->CtrlReq.SyncChg) && bScanAStop)
			{
				DpmCtrl.pSvOptIf->CtrlReq.SyncChg = TRUE;	/* WDC�̂ݍX�V */
				DpmCtrl.pSvOptIf->CtrlReq.RspUpdate = TRUE;

				ApxAinputNetIf( );	/* �l�b�g���[�N�h�e���͏���(62.5us���Ή�)	*/

				DpmCtrl.pSvOptIf->CtrlReq.CmdUpdate = FALSE;
//				DpmCtrl.pSvOptIf->CtrlReq.RspUpdate = FALSE;
				DpmCtrl.pSvOptIf->CtrlReq.SyncChg = FALSE;
			}
			DpmResetTioCnt(&DpmCtrl);	/* �X�P�W���[���J�E���^���Z�b�g */

			return;	/* �������^�[�� */
		}
	}

/* �X�P�W���[�����Z�b�g�������J�� */
	if(DpmResetScheduler(&DpmCtrl) != DPM_FUNC_SUCCESS)
	{/* Error */
		KPI_ALARMSET(ALM_WDC_ERR);	/* A.E02 */
	}
}


/****************************************************************************
 DPM Option���W���[��ScanB�擪����
****************************************************************************/
void DPiPreCtrlProcedure(void)
{
/* �f�[�^�X�V�v���쐬 */
	if( DpmMakeCycDataUpdateReq(&DpmCtrl) != DPM_FUNC_SUCCESS)
	{/* Error */
		KPI_ALARMSET(ALM_WDC_ERR);	/* A.E02 */
	}
}

/****************************************************************************
 DPM Option���W���[��ScanB�㏈��
****************************************************************************/
void DPiPostCtrlProcedure(void)
{
/* �T�C�N���b�N�f�[�^���C�g */
	DpmWriteCycRSP(&DpmCtrl);

/* Tio�X�P�W���[���J�E���^�X�V */
	DpmUpdateTioCnt(&DpmCtrl);
}

/****************************************************************************
 DPM Option���W���[��ScanC����
****************************************************************************/
void DPiScanCProcedure(void)
{
	USHORT sts = 0;
	USHORT almCode = 0;
	USHORT wrk;

/* LED�\�� */
#if defined(MCCSW_SGDVCMOP)	//<V684>
	if(BoutV.CtrlMode.b.cm != CTRLMODE_JOG && BoutV.CtrlMode.b.cm != CTRLMODE_ZSRCH)
	{
		sts |= ((USHORT)CoutV.f.Sv.Pot << LED_POT_BNO);
		sts |= ((USHORT)CoutV.f.Sv.Not << LED_NOT_BNO);
		sts |= ((USHORT)CoutV.f.Sv.Pls << LED_PLS_BNO);
		sts |= ((USHORT)CoutV.f.Sv.Nls << LED_NLS_BNO);
	}
#else
	sts |= ((USHORT)CoutV.f.Sv.Pot << LED_POT_BNO);
	sts |= ((USHORT)CoutV.f.Sv.Not << LED_NOT_BNO);
#endif

#if ( CCSW_SFTID == SFTID_EXIST )																	  /*<V717>Add:Start*/
	sts |= (DpmCtrl.pSvOptIf->SomInfo & 0x01) ? 1 << LED_SFON_BNO : 0x0000;
	sts |= (DpmCtrl.pSvOptIf->SomInfo & 0x02) ? 1 << LED_SAFE_BNO : 0x0000;
	sts |= (DpmCtrl.pSvOptIf->SomInfo & 0x10) ? 1 << LED_ACT_BNO  : 0x0000;							  /*<V729>Add*/
#endif																								  /*<V717>Add:End  */
	sts |= ((USHORT)DpmCtrl.pSvOptIf->CycRsp.InSig.EMG_Sig << LED_HBB_BNO);

	wrk = (USHORT)(((DpmCtrl.State == DPHNDSHK_SYNC_SEQ) && (DpmCtrl.wdErrCnt <= 0)) ? 0x1: 0x0);
	sts |= (wrk << LED_CNCT_BNO);

	sts |= ((USHORT)DpmCtrl.pSvOptIf->CycRsp.Seq.ServoOnComp << LED_SVON_BNO);
	sts |= ((USHORT)DpmCtrl.pSvOptIf->CycRsp.Ctrl.MotorMoving << LED_TGON_BNO);

	wrk = ((USHORT)CoutV.f.Sv.Prefon | (USHORT)CoutV.f.Sv.Vrefon | (USHORT)CoutV.f.Sv.Trefon);
	wrk = (USHORT)(~wrk) & 0x1;
	sts |= (wrk << LED_DEN_BNO);

	sts |= (((CMDOP_NOT_CONNECTED == KPI_GET_CMDOP_TYPE()) ? 1: 0) << LED_OP_DISABLE_BNO); //<DPM_V103>

	if(!KpiGetNetFirstAlarmInfo(&almCode))
	{	KpiGetNetFirstWarningInfo(&almCode);}

	HwDisplayLedData(sts, almCode);
}


/****************************************************************************
****************************************************************************/
/* Public function APIs of Option-module CS message driver */
/****************************************************************************
****************************************************************************/
#ifdef	__ghs__
#pragma ghs section text=".textEX"
#endif

/*****************************************************************************
	CS���b�Z�[�W��M�J�n�v���Z�b�g
*****************************************************************************/
LONG DPiReqRecvCSMsg(UCHAR ch)
{
	LONG rc = ND_SUCCESS;
	if(ch == 1)
	{
		rc = DpmReqRecvCSMsg(&DpmCtrl);
	}
	return rc;
}

/*****************************************************************************
	CS���b�Z�[�W��M�����`�F�b�N
*****************************************************************************/
LONG DPiGetRecvCSMsgSts(UCHAR ch, USHORT *rcvWSz, USHORT *rcvBSz)
{
	LONG rc = ND_RECV_BUSY;
	if(ch == 1)
	{
		rc = DpmGetRecvCSMsgSts(&DpmCtrl, rcvWSz, rcvBSz);
	}
	return rc;
}

/*****************************************************************************
	CS���b�Z�[�W�f�[�^���[�h
	�����FrcvData	�E�E�E��M�f�[�^�i�[��, rdSz	�E�E�E���[�h���[�h��
*****************************************************************************/
void DPiReadCSMsgData(UCHAR ch, USHORT *rcvData, USHORT rdSz)
{
	if(ch == 1)
	{
		DpmReadCSMsgData(&DpmCtrl, rcvData, rdSz);
	}
}

/*****************************************************************************
	CS���b�Z�[�W�f�[�^���C�g
	�����F	sndData	�E�E�E���M�f�[�^�i�[��, wrSz	�E�E�E���C�gByte��
*****************************************************************************/
void DPiWriteCSMsgData(UCHAR ch, USHORT *sndData, USHORT wrSz)
{
	if(ch == 1)
	{
		DpmWriteCSMsgData(&DpmCtrl, sndData, wrSz);
	}
}

/*****************************************************************************
	CS���b�Z�[�W���M�J�n�v���Z�b�g
*****************************************************************************/
LONG DPiReqSendCSMsg(UCHAR ch)
{
	LONG rc = ND_RECV_BUSY;
	if(ch == 1)
	{
		rc = DpmReqSendCSMsg(&DpmCtrl);
	}
	return rc;
}

/*****************************************************************************
	CS���b�Z�[�W���M�����`�F�b�N
*****************************************************************************/
LONG DpiGetSendCSMsgSts(UCHAR ch)
{
	return ND_SUCCESS;
}

/*****************************************************************************
	CS���b�Z�[�W���M�L�����Z��<DPM_V103>
*****************************************************************************/
LONG DpiSendMsgAbort(UCHAR ch)
{
	LONG rc = ND_SUCCESS;
	if(ch == 1)
	{
		rc = DpmSendMsgAbort(&DpmCtrl);
	}
	return rc;
}

#ifdef	__ghs__
#pragma ghs section text=default
#endif


/****************************************************************************
****************************************************************************/
/* Public function APIs of Option-module DegOp. driver */
/****************************************************************************
****************************************************************************/
#ifdef	__ghs__
#pragma ghs section text=".textEX"
#endif
LONG DPiGetDegOpState(DEGOP_PRMRNG *pPrmRange)
{
	return DpmGetDegOpState(&DpmCtrl, pPrmRange);
}

LONG DPiReqDegOpPrmRead(USHORT cmd, USHORT no)
{
	return DpmReqDegOpPrmRead(&DpmCtrl, cmd, no);
}

LONG DPiReqDegOpPrmWrite(USHORT no, USHORT size, LONG data)
{
	return DpmReqDegOpPrmWrite(&DpmCtrl, no, size, data);
}

LONG DPiGetDegOpPrmValue(USHORT *no, DEGOP_ATTR *Attr, LONG *data)
{
	return DpmGetDegOpPrmValue(&DpmCtrl, no, Attr, data);
}
/*------------ <V685> Add Begin -------------------------------------------*/
USHORT DpiGetDegOpNumOfFn(void)
{
	return (DpmCtrl.pSvOptIf->OptSetting.NumOfFnMode);
}

LONG DpiReqDegOpFn(USHORT cmd, USHORT idx, USHORT data)
{
	return DpmReqDegOpFn(&DpmCtrl, cmd, idx, data);
}

LONG DpiGetDegOpFn(DGOP_RSP *pDgopRsp, DGOPEX_RSP *pDgopExRsp)
{
	return DpmGetDegOpFn(&DpmCtrl, pDgopRsp, pDgopExRsp);
}
/*------------ <V685> Add End ---------------------------------------------*/
#ifdef	__ghs__
#pragma ghs section text=default
#endif

/****************************************************************************
****************************************************************************/
/* Public function APIs of Option-module SC message driver */
/****************************************************************************
****************************************************************************/
#ifdef	__ghs__
#pragma ghs section text=".textEX"
#endif
LONG DPiGetSCMsgState(void)
{
	return DpmGetSCMsgState(&DpmCtrl);
}
LONG DPiReqSCMsgTrans(USHORT *pMsgBuf, LONG bSz)
{
	return DpmReqSCMsgTrans(&DpmCtrl, pMsgBuf, bSz);
}
LONG DPiGetSCMsgRspData(USHORT *pMsgBuf, LONG *bSz)
{
	return DpmGetSCMsgRspData(&DpmCtrl, pMsgBuf, bSz);
}
void DPiCancelSCMsgReq(void)
{
//	DpmCancelSCMsgReq(&DpmCtrl);
}

#ifdef	__ghs__
#pragma ghs section text=default
#endif
