/****************************************************************************
Description	: MECHATROLINK Communication Manager
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------

****************************************************************************/
#ifndef COM_MANAGER_H_
#define COM_MANAGER_H_

#include "ComCfg.h"
#include "NetDriver.h"

/***************************************************************************/
#define MCOM_FUNC_NORMAL_END	0x0						/* ����I�� */
#define COM_FUNC_CONTINUE   	0x1						/* �p�����s�� */
#define COM_FUNC_NORMAL_END		0x0						/* ����I�� */
#define COM_FUNC_ERROR_END		-1						/* �ُ�I�� */
#define COM_WDC_ERR				ERR_COM_WDC				/* WDT�ُ� */
#define COM_PHASE_ERROR     	ERR_SYN_REQ				/* �J�ڐ�ُ� */
#define COM_RECV_ERR			ERR_COM_RCVSTP			/* ��M�ُ� */
#define COM_FCS_ERR				ERR_COM_FCS_A			/* FCS�ُ� */
#define COM_RECV_GBL_ERR		ERR_GBL_COM_A			/* �O���[�o���f�[�^��M�ُ� */
#define COM_RECV_SYNC_ERR		ERR_GBL_SYNC			/* �����ʐM�ُ� */
#define COM_RECV_WARNG			ERR_COM_RCV				/* ��M�ُ탏�[�j���O */
#define COM_FCS_WARNG			ERR_COM_FCS_W			/* FCS���[�j���O */
#define COM_RECV_GBL_WARNG		ERR_GBL_COM_W			/* �O���[�o���f�[�^��M���[�j���O */
#define COM_APLCYC_ERR			ERR_DATA_SET			/* �ʐM�����ُ� */
#define COM_TCYC_ERR			ERR_COM_TCYC			/* �`�������ُ� */
#define COM_SYNC_ERR			ERR_GBL_SYNC			/* Phase�ڍs�������s */
#define COM_WDTOUT_ERR			ERR_SYNC_TOUT			/* WDC�ُ� */
#define COM_CHGE_BUFF_ERROR		ERR_COM_LSISYS			/* ��փo�b�t�@�؂�ւ����s */
#define	COM_RECV_ASYNC_ERR		ERR_COM_RCV_ASYNC_A		/* ��M�ُ�(�C�x���g�h���u���ʐM) */
#define	COM_RECV_ASYNC_WARNG	ERR_COM_RCV_ASYNC_W		/* ��M�x��(�C�x���g�h���u���ʐM) */

#define COM_WDT_OFS_ERR			(-1)					/* WDT�I�t�Z�b�g�l�ُ� */
#define COM_EXCMD_OFS_ERR		(-2)					/* �T�u�R�}���h�I�t�Z�b�g�l�ُ� */

#define COM_DATA_SIZE_ERR_BIT	ND_DATA_SIZE_ERR_BIT	/* �ʐM�f�[�^�T�C�Y�ݒ�ُ�r�b�g */
#define COM_AXIS_NO_ERR_BIT		ND_AXIS_NO_ERR_BIT		/* �ǃA�h���X�ݒ�ُ�r�b�g */
#define COM_ASIC_STATUS_ERR_BIT	ND_ASIC_STATUS_ERR_BIT	/* JL-100�X�e�[�^�X�ُ�r�b�g */


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
#define SINGLE_TRANSFER 		(0)						/* �P�� */
#define CONSECUTIVE_TRANSFER	(1)						/* �A�� */


/* �f�t�H���g�ݒ� */
#define MAX_COM_APL_CYC 		(128000)				/* �ő�ʐM����[0.25us] */
#define T_CYCLE_125US			(500)					/* �`������125us[0.25us] */

#define COM_RECV_ERR_LIMIT 		(2)						/* �ʐM�ُ팟�o�l */
#define COM_BUF_A				(0)						/* �g�O���o�b�t�@A�� */
#define COM_BUF_B				(1)						/* �g�O���o�b�t�@B�� */
#define COM_ALT_BUF				(2)						/* �g�O���o�b�t�@�� */

#define PH_UP_TIMEOUT			(8000)					/* Phase UP�^�C���A�E�g[ms] */


/* RSP�쐬�����쐬�e�[�u�� */
#define DTMOD_NUM				(3)						/* �]�����[�h�� */
#define MAX_COM_CYC				(32)					/* �ő�ʐM���� */

#define COM_APLCMD_TIME			(0x01)					/* �ʐM����CMD��̓^�C�~���O */
#define COM_APLRSP_TIME			(0x02)					/* �ʐM����RSP�쐬�^�C�~���O */


/* recvErrCnt�̒l��` */
#define COM_RCV_ERR_STS			COM_RECV_ERR_LIMIT
#define COM_RCV_WRN_STS			(1)
#define COM_RCV_NOERR_STS		(0)

#define CRT_CMD_DISCONNECT		(0x0f)					/* ��M�f�[�^�̃R�}���h��DISCONNECT */


#ifdef __BIG_ENDIAN__
/* WDT�I�t�Z�b�g�ʒu */
#define M3STD_WDT_OFS			(2)						/* WDT�I�t�Z�b�g�ʒu�i�o�C�g�j */
#define M2_WDT_OFS				(12)					/* WDT�I�t�Z�b�g�ʒu�i�o�C�g�j */
#define EVTDRV_WDT_OFS			(2)						/* WDT�I�t�Z�b�g�ʒu�i�o�C�g�j */

/* �T�u�R�}���h�I�t�Z�b�g�ʒu */
#define M3STD_EX_CMD_OFS		(8)						/* �T�u�R�}���h�J�n�ʒu(�_�u�����[�h) */ 
#define M2_EX_CMD_OFS			(4)						/* �T�u�R�}���h�J�n�ʒu(�_�u�����[�h) */
#define EVTDRV_EX_CMD_OFS		(8)						/* �T�u�R�}���h�J�n�ʒu(�_�u�����[�h) */
#else
/* WDT�I�t�Z�b�g�ʒu */
#define M3STD_WDT_OFS			(1)						/* WDT�I�t�Z�b�g�ʒu�i�o�C�g�j */
#define M2_WDT_OFS				(15)					/* WDT�I�t�Z�b�g�ʒu�i�o�C�g�j */
#define EVTDRV_WDT_OFS			(1)						/* WDT�I�t�Z�b�g�ʒu�i�o�C�g�j */

/* �T�u�R�}���h�I�t�Z�b�g�ʒu */
#define M3STD_EX_CMD_OFS		(8)						/* �T�u�R�}���h�J�n�ʒu(�_�u�����[�h) */ 
#define M2_EX_CMD_OFS			(4)						/* �T�u�R�}���h�J�n�ʒu(�_�u�����[�h) */
#define EVTDRV_EX_CMD_OFS		(8)						/* �T�u�R�}���h�J�n�ʒu(�_�u�����[�h) *//*�����u*/
#endif


/* �ʐM�ُ��� */
#define RECV_NORMAL				(0)						/* �����M */
#define RECV_CMD_ERR			(1)						/* ��M�ُ� */
#define RECV_FCS_ERR			(2)						/* FCS�ُ�  */
#define RECV_GBL_ERR			(3)						/* �O���[�o���f�[�^��M�ُ�  */
#define	RECV_ASYNC_CMD_ERR		(4)						/* ��M�ُ�(�񓯊��ʐM) */


/* �񓯊��ʐM */
#define COM_ASYNC_RCV_SUCCESS	ND_ASYNC_RCV_SUCCESS	/* �񓯊��ʐM ����I���i��M) */
#define COM_ASYNC_SEND_SUCCESS	ND_ASYNC_SEND_SUCCESS	/* �񓯊��ʐM ����I���i���M) */
#define COM_SENDING_FRAME		ND_SENDING_FRAME		/* �񓯊��ʐM �f�[�^���M�� */
#define COM_SEND_FRAME_ERR		ND_SEND_FRAME_ERR		/* �񓯊��ʐM ���M�ُ� */
#define COM_ASYNC_RCV_ERR		ND_ASYNC_RCV_ERR		/* �񓯊��ʐM ��M�ُ� */
#define COM_ASYNC_TOUT			ND_ASYNC_TOUT			/* �񓯊��ʐM ��M�����^�C���A�E�g */


/* �ʐM�p�p�����[�^�\���� */
typedef struct COM_NET_PRM_ {
	LONG	nodeAddress;		/* ���ǃA�h���X *//* <S07A> */
	LONG	dataSize;			/* �`���o�C�g�� *//* <S07A> */
	
	LONG	aplCycle;			/* �ʐM����[x �`������] */
	LONG	tType;				/* �`������ �P��(0)/�A��(1) */
	LONG	recvErrLimt;		/* �ʐM�ُ�J�E���g */

	BOOL	wdtErrMask;			/* �ʐM�ُ�(A.E60)���� */
	BOOL	recvErrMask;		/* �v�c�s�ُ�(A.E50)���� */
	BOOL	recvWrnMask;		/* �ʐM�x��(A.96*)���� */
} COM_NET_PRM;



/* COM Manager �n���h�� */
typedef struct COM_HNDL_ {
	COM_NET_PRM		NetPrm;				/* �ʐM�ݒ�p�����[�^ */

	LONG			comCycCnt;			/* �ʐM�����J�E���g�l */
	USHORT			recvErrCntNum;		/* ��M�G���[�J�E���g�ώZ�l */
	UINT32			*CmdBufPtr;			/* ����CMD�o�b�t�@ */
	UINT32			*RspBufPtr;			/* ����RSP�o�b�t�@ */
	UINT32			CmdBuffer[16];		/* ����CMD�o�b�t�@ */
	UINT32			RspBuffer[16];		/* ����RSP�o�b�t�@ */
	BOOL			wdcErrChk;			/* WDC�G���[�`�F�b�N[0:����/1:�L��] */
	BOOL			recvErrChk;			/* �ʐM�G���[�`�F�b�N[0:����/1:�L��] */
	LONG			RecvErr;			/* �ʐM�ُ��� [0:�����M/1:��M�ُ�/2:FCS�ُ�]*/
	LONG			LrecvErr;			/* �O��`�������̒ʐM�ُ��� */
	LONG			recvErrCnt;			/* ��M�G���[�J�E���^(��M����Ń��Z�b�g) */
	UCHAR			mWdc;				/* �}�X�^WDC */
	UCHAR			mWdcForSync;		/* �����J�n���}�X�^WDC */
	UCHAR			sWdc;				/* �X���[�uWDC */
	LONG			transMode;			/* �`������ */

	VINT32			Phase;				/* ���ݒʐMPhase */
	VINT32			reqPhase;			/* Phase�ڍs�v�� */
	COM_PHUP_STEP	stateOfPhUp;		/* Phase�ڍs�����V�[�P���X��� */
	COM_SYNC_STEP	stateOfSync;		/* WDC�G�b�W���o�V�[�P���X��� */
	LONG			wdChkCnt;			/* WDC�G�b�W���o����� */
	VINT32			PhUpSts;			/* �t�F�[�Y�ڍs���s���� */
	ULONG			startTime;			/* �v���J�n����(ms) */
	
	LONG			wdtOfs;				/* WDT�I�t�Z�b�g�ʒu(�o�C�g) */
	LONG			exCmdOfs;			/* �T�u�R�}���h�I�t�Z�b�g�ʒu(���[�h) */
	LONG			my_ax_number;		/* ���ǎ��ԍ� */
} COM_HNDL;



/****************************************************************************
 API's
****************************************************************************/

/* COM Manager���������� */
VOID ComInit( COM_HNDL *hdl, LONG ax_no );
/* �ʐM���[�h�p�����[�^�ݒ�	*//* <S07A> */
VOID ComNetPrmSet( COM_HNDL *hdl, ND_PARAM	nd_prm );
/* �ʐMPhase�J��(Phase�A�b�v)�v������ */
INT32 ComReqPhaseUp( COM_HNDL *hdl, LONG phs, LONG mode, LONG aplCyc, BOOL AsyncMode );
/* �ʐMPhase�J�ڗv��(Phase�_�E��)���� */
INT32 ComReqPhaseDown( COM_HNDL *hdl, LONG phs );
/* �ʐMPhase�J�ڎ��s���� */
VOID ComRunPhaseChange( COM_HNDL *hdl );

/* ���ǎ�M�X�e�[�^�X�擾���� */
//INT32 ComReceiveCmd( COM_HNDL *hComMngr );
/* ���ǃf�[�^�̑��M(�`�r�h�b�ւ̃��C�g)���� */
//VOID ComSendRsp( COM_HNDL *hComMngr );
/* �X���[�uWDC�J�E���g�A�b�v�A���M�pRSP�o�b�t�@�|�C���^�ؑւ����� */

VOID ComSetRspBuffer( COM_HNDL *hComMngr );
/* �ʐM�����T�C�N���̐������� */
//VOID ComGenerateAplCycle( COM_HNDL *pComHdl, INT32 RecvErr );
VOID ComGenerateAplCycle( COM_HNDL *pComHdl );
/* �ʐM�G���[(��M�G���[�AWDC�G���[)�̌��o���� */
INT32 ComCheckNetError( COM_HNDL *hdl );
/* WDT�I�t�Z�b�g�ʒu�ݒ菈�� */
INT32 ComSetWdtOfs( COM_HNDL *pComHdl, LONG tdata_size, LONG wdtOfs );
/* �T�u�R�}���h�I�t�Z�b�g�ʒu�ݒ菈�� */
INT32 ComSetExCmdOfs( COM_HNDL *pComHdl, LONG tdata_size, LONG exCmdOfs );
/* ���X�|���X�f�[�^�쐬�^�C�~���O�擾 */
BOOL ComCheckRspTiming( COM_HNDL *pComHdl );



#endif /* COM_MANAGER_H_ */


