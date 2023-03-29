/****************************************************************************
Description	: MECHATROLINK Communication Manager
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : Mercury

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------

****************************************************************************/
#ifndef		COM_MANAGER_H_
#define		COM_MANAGER_H_

/* includes */
#include "ComCfg.h"
#include "NetDriver.h"

/***************************************************************************/
#define COM_FUNC_CONTINUE   ERR_WAIT_COMP	/* �p�����s�� */
#define COM_FUNC_NORMAL_END ERR_NONE		/* ����I�� */
#define COM_WDC_ERR			ERR_COM_WDC		/* WDT�ُ� */
#define COM_PHASE_ERROR     ERR_SYN_REQ		/* �J�ڐ�ُ� */
#define COM_RECV_ERR		ERR_COM_RCVSTP	/* �ʐM�ُ� */
#define COM_RECV_WARNG		ERR_COM_RCV		/* �ʐM���[�j���O */
#define COM_APLCYC_ERR		ERR_DATA_SET	/* �ʐM�����ُ� */
#define COM_TCYC_ERR		ERR_COM_TCYC	/* �`�������ُ� */
#define COM_SYNC_ERR		ERR_GBL_SYNC	/* Phase�ڍs�������s */
#define COM_WDTOUT_ERR		ERR_SYNC_TOUT	/* WDC�ُ� */

/* Phase�ڍs�����V�[�P���X��� */
typedef enum COM_PHUP_STEP_ {
	PH_IDOL = 0,										/* ����� */
	PH_RESET,											/* Phase���������  */
	PH_CHK_COM_CYCLE,									/* �ʐM�����`�F�b�N��� */
	PH_SYNC_COMCYC,										/* �ʐM����������� */
} COM_PHUP_STEP;

/* �ʐM�����ڍs�V�[�P���X��� */
typedef enum COM_SYNC_STEP_ {
	DISABLE_SYNC = 0,									/* �ʐM���� �񓯊� */
	START_COMCYC,										/* �}�X�^�v�c�b�G�b�W���o�J�n */
	READY_COMCYC,										/* �}�X�^�v�c�b�G�b�W���o */
	START_WDCCHK,										/* �v�c�b�`�F�b�N�J�n */
	ENA_WDCCHK,											/* �v�c�b�`�F�b�N�L�� */
} COM_SYNC_STEP;

/* �f�[�^�]������ */
#define SINGLE_TRANSFER 	  (0)			/* �P�� */
#define CONSECUTIVE_TRANSFER  (1)			/* �A�� */

/* �f�t�H���g�ݒ� */
#define COM_DEF_M2TCYC 		(16000)			/* �f�t�H���g�`������[0.25us] */
#define COM_DEF_M1TCYC 		(8000)			/* �f�t�H���g�`������[0.25us] */
#define MAX_COM_APL_CYC 	(128000)		/* �ő�ʐM����[0.25us] */
#define LINK_DATA_SIZE		(32)			/* �����N�f�[�^�̃T�C�Y�i�o�C�g�j */

#define COM_RECV_ERR_LIMIT 	(2)				/* �ʐM�ُ팟�o�l */
#define COM_BUF_A			(0)				/* �g�O���o�b�t�@A�� */
#define COM_BUF_B			(1)				/* �g�O���o�b�t�@B�� */
#define COM_ALT_BUF			(2)				/* �g�O���o�b�t�@�� */

#define PH_UP_TIMEOUT		(8000)			/* Phase UP�^�C���A�E�g[ms] */

/* RSP�쐬�����쐬�e�[�u�� */
#define DTMOD_NUM   3						/* �]�����[�h�� */
#define MAX_COM_CYC 32						/* �ő�ʐM���� */

#define COM_APLCMD_TIME		0x01			/* �ʐM����CMD��̓^�C�~���O */
#define COM_APLRSP_TIME		0x02			/* �ʐM����RSP�쐬�^�C�~���O */

/* recvErrCnt�̒l��` */
#define COM_RCV_ERR_STS		COM_RECV_ERR_LIMIT
#define COM_RCV_WRN_STS		1
#define COM_RCV_NOERR_STS	0

/* WDT�I�t�Z�b�g�ʒu */
#define M2_WDT_OFS				(15)					/* WDT�I�t�Z�b�g�ʒu�i�o�C�g�j */

/* �T�u�R�}���h�I�t�Z�b�g�ʒu */
#define M2_EX_CMD_OFS			(4)						/* �T�u�R�}���h�J�n�ʒu(�_�u�����[�h) */

/* comChkWdcEdge�֘A */
#define COM_WDC_BUF_NUM		(3)				/* WDC�G�b�W���o�pWDC�l�ۑ��o�b�t�@�� 	*//*<V592>*/
#define WDC_EDGE_PATTERN_A	(1)				/* WDC�G�b�W�p�^�[��A 					*//*<V592>*/
#define WDC_EDGE_PATTERN_B	(2)				/* WDC�G�b�W�p�^�[��B 					*//*<V592>*/
#define WDC_EDGE_PATTERN_C	(3)				/* WDC�G�b�W�p�^�[��C 					*//*<V592>*/

#define CRT_CMD_DISCONNECT	0x0f			/* ��M�f�[�^�̃R�}���h��DISCONNECT 	*//*<V593>*/


/* �ʐM�p�p�����[�^�\���� */
typedef struct COM_NET_PRM_ {
	LONG	tCycle;			/* �`������[0.25us] */
	LONG	tSlot;			/* �^�C���X���b�g[0.25us] */
	LONG	nodeAddress;	/* ���ǃA�h���X */
	LONG	dataSize;		/* �`���o�C�g�� */
	LONG	tSpeed;			/* �`�����x 4Mbps, 10Mbps */
	LONG	aplCycle;		/* �ʐM����[x �`������] */
	LONG	tType;			/* �`������ �P��(0)/�A��(1) */

	LONG	recvErrLimt;	/* �ʐM�ُ�J�E���g */
	BOOL	wdtErrMask;		/* �ʐM�ُ�(A.E60)���� */
	BOOL	recvErrMask;	/* �v�c�s�ُ�(A.E50)���� */
	BOOL	recvWrnMask;	/* �ʐM�x��(A.96*)���� */
	BOOL	enaSoftRst;		/* DISC���\�t�g���Z�b�gSW */
} COM_NET_PRM;

/* COM Manager �n���h�� */
typedef struct COM_HNDL_ {
	COM_NET_PRM	NetPrm;			/* �ʐM�ݒ�p�����[�^ */

	LONG		comCycCnt;		/* �ʐM�����J�E���g�l */
	LONG		recvErrCntNum;	/* ��M�G���[�J�E���g�ώZ�l */
	UINT32		*CmdBufPtr;		/* ����CMD�o�b�t�@ */
	UINT32		*RspBufPtr;		/* ����RSP�o�b�t�@ */
	UINT32		CmdBuffer[16];	/* ����CMD�o�b�t�@ */
	UINT32		RspBuffer[16];	/* ����RSP�o�b�t�@ */
	BOOL		wdcErrChk;		/* WDC�G���[�`�F�b�N[0:����/1:�L��] */
	BOOL		recvErrChk;		/* �ʐM�G���[�`�F�b�N[0:����/1:�L��] */
	LONG		RecvErr;		/* �ʐM�ُ��� [0:�����M/1:��M�ُ�]*/
	LONG		LrecvErr;		/* �O��`�������̒ʐM�ُ��� */
	LONG		recvErrCnt;		/* ��M�G���[�J�E���^(��M����Ń��Z�b�g) */
	UCHAR		mWdc;			/* �}�X�^WDC */
	UCHAR		mWdcForSync;	/* �����J�n���}�X�^WDC */
	UCHAR		sWdc;			/* �X���[�uWDC */
	LONG		transMode;		/* �`������ */

	BOOL		gblReadReq;		/* GBL�f�[�^�擾�v�� */
	VINT32		Phase;			/* ���ݒʐMPhase */
	VINT32		reqPhase;		/* Phase�ڍs�v�� */
	UCHAR		stateOfPhUp;	/* Phase�ڍs�����V�[�P���X��� */
	UCHAR		stateOfSync;	/* WDC�G�b�W���o�V�[�P���X��� */
	UCHAR		wdChkCnt;		/* WDC�G�b�W���o����� */
	VINT32		PhUpSts;		/* �t�F�[�Y�ڍs���s���� */
	LONG		startTime;		/* �v���J�n����(ms) */
	
	LONG		wdtOfs;			/* WDT�I�t�Z�b�g�ʒu(�o�C�g) */
	LONG		exCmdOfs;		/* �T�u�R�}���h�I�t�Z�b�g�ʒu(���[�h) */
	LONG		my_ax_number;	/* ���ǎ��ԍ� */
} COM_HNDL;

/* COM Manager ���J�f�[�^�擾�p�`�o�h */
#define ComGetNetPrmaplCycle(hdl)		(hdl->NetPrm.aplCycle)
#define ComGetNetPrmtType(hdl)			(hdl->NetPrm.tType)

/****************************************************************************
 API's
****************************************************************************/
/* ���W���[��������			*/
VOID ComInit(COM_HNDL* comHdl, LONG ax_no);
/* �ʐM���[�h�p�����[�^�ݒ�	*/
VOID ComNetPrmSet( COM_HNDL *hdl, ND_PARAM	nd_prm );
/* PhaseUP�v��				*/
INT32 ComReqPhaseUp( COM_HNDL *hdl, LONG phs, LONG mode, LONG aplCyc );
/* PhaseDOWN�v��			*/
INT32 ComReqPhaseDown(COM_HNDL* hdl, LONG phs);
/* Phase�ڍs��Ԋm�F		*/
INT32 ComGetPhaseChangeState(COM_HNDL* hdl);
/* Phase�ڍs������			*/
VOID ComRunPhaseChange(COM_HNDL* hdl);
/* CMD��M					*/
INT32 ComReceiveCmd(COM_HNDL* hdl);
/* RSP���M					*/
VOID ComSendRsp(COM_HNDL* hdl);
/* ���M�o�b�t�@�ݒ�			*/
VOID ComSetRspBuffer(COM_HNDL* hdl);
/* �ʐM�T�C�N���̐���		*/
VOID ComGenerateAplCycle(COM_HNDL* hdl);
/* �ʐM�ُ팟�o				*/
INT32 ComCheckNetError(COM_HNDL* hdl);
/* ���X�|���X�f�[�^�쐬�^�C�~���O�擾 */
BOOL ComCheckRspTiming( COM_HNDL *pComHdl );
/* �ʐM�`�r�h�b�Đݒ菈��	*/
INT32 ComResetAsicProc(COM_NET_PRM* netPrm);
#endif /* COM_MANAGER_H_ */
