/****************************************************************************
Description	: MECHATROLINK Communication Driver
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes		:
 Name   Date            Description
----------------------------------------------------------------------------
 Y.Oka	2010.02.23		created.
 I.M.   2011.01.18		reviewed. 

****************************************************************************/
#ifndef NET_DRIVER_H_
#define NET_DRIVER_H_


#ifdef __BIG_ENDIAN__
#include "big_jl100_def.h" 
#else
#include "little_jl100_def.h"
#endif


/****************************************************************************/
/*		DEFINES																*/
/****************************************************************************/

/* PHY��CH��` */
#define ND_PORT_CH1					(0)			/* PHY��CH1 */
#define ND_PORT_CH2					(1)			/* PHY��CH2 */

/* Error code definitions */
#define ND_SUCCESS					(0)			/* ����I�� */
#define ND_CONTINUE					(1)			/* �������s�� */
#define ND_COMM_STAT_RESET			(2)			/* �R���g���[�����ʐM��Ԃ����Z�b�g���� */
#define ND_RCVED_TMC_FRAME			(3)			/* TMC�t���[����M */
#define ND_NOT_RCV_TMC_FRAME		(4)			/* TMC�t���[������M */

#define ND_JL100_READY_TIMEOUT		(-1)		/* JL-100�N�������^�C���A�E�g */
#define ND_JL100_PHY_INIT_ERR		(-2)		/* JL-100 PHY���W�X�^���������s */
#define ND_SRAM_CHECK_ERROR			(-3)		/* JL-100����RAM�̃��[�h�E���C�g�`�F�b�N���s */
#define ND_JL100_INIT_ERROR			(-4)		/* JL-100���������s */

#define ND_MEASURE_TRANSDLY_ERROR	(-5)		/* �x���v���v�����s */
#define ND_DETECT_SLV_CMP_ERROR		(-6)		/* ���ݐڑ��ǌ��o���s */
#define ND_GET_CMODE_ERROR			(-7)		/* �ʐM���[�h�擾���s */
#define ND_START_SYNC_ERROR			(-8)		/* �����ʐM�X�^�[�g���s */
#define ND_START_ASYNC_ERROR		(-9)		/* �񓯊��ʐM�X�^�[�g���s */
#define ND_PORT_SEL_ERROR			(-10)		/* ���T�|�[�g�ʐM���[�h�I�� */

#define ND_CHANGE_CBUFF_ERROR		(-11)		/* ��փo�b�t�@�؂�ւ����s */
#define ND_NOT_SYNC_ERROR			(-12)		/* �����ʐM�ُ� */
#define ND_GBL_RCV_ERROR			(-13)		/* �����t���[����M�ُ� */
#define ND_READ_CDATA_ERROR			(-14)		/* �w�߃f�[�^�ǂݏo�����s */
#define ND_WRITE_RDATA_ERROR		(-15)		/* �����f�[�^�������ݎ��s */
#define ND_CHANGE_RBUFF_ERROR		(-16)		/* ��փo�b�t�@�؂�ւ����s */

#define ND_INIT_SEQ_ERROR			(-17)		/* JL-100�����������V�[�P���X�ُ� */
#define ND_GET_MYSTAT_ERROR			(-18)		/* ���ǂ̎w�߃f�[�^�̎�M�ُ� */

#define ND_DATA_READ_ERROR			(-19)		/* �T�C�N���b�N�f�[�^�ǂݏo�����s */
#define ND_DATA_RECV_ERROR			(-20)		/* �T�C�N���b�N�f�[�^��M���s */

/* JL-100 �G���[��� */
#define ND_DATA_SIZE_ERR_BIT		(1)			/* �ʐM�f�[�^�T�C�Y�ݒ�ُ�r�b�g */
#define ND_AXIS_NO_ERR_BIT			(2)			/* �ǃA�h���X�ݒ�ُ�r�b�g */
#define ND_FCS_ERR_BIT				(4)			/* FCS�ُ�r�b�g */
#define ND_ASIC_STATUS_ERR_BIT		(8)			/* JL-100�X�e�[�^�X�ُ�r�b�g */

/* Message Communication SCF�b�� */
#define ND_RECV_BUSY				(2)			/* MSG��M�ҋ@��� */
#define ND_SEND_BUSY				(1)			/* MSG���M����� */
#define ND_SUCCESS					(0)			/* ����I�� */
#define ND_RECVMSG_ERR				(-1)		/* MSG��M�G���[ */
#define ND_RECVADR_ERR				(-2)		/* MSG��M�A�h���X�G���[ */
#define ND_SENDMSG_ERR				(-3)		/* MSG���M�G���[ */

/* �C�x���g�h���u���ʐM */
#define ND_ASYNC_RCV_SUCCESS		(0)			/* �񓯊��ʐM ����I���i��M) */
#define ND_ASYNC_SEND_SUCCESS		(1)			/* �񓯊��ʐM ����I���i���M) */
#define ND_SENDING_FRAME			(2)			/* �񓯊��ʐM �f�[�^���M�� */

#define ND_SEND_FRAME_ERR			(-1)		/* �񓯊��ʐM ���M�ُ� */
#define ND_ASYNC_RCV_ERR			(-2)		/* �񓯊��ʐM ��M�ُ� */
#define ND_ASYNC_TOUT				(-3)		/* �񓯊��ʐM ��M�����^�C���A�E�g */

#define ND_ASYNC_REQ_RCV_SEQ		(0)			/* �t���[����M�v���V�[�P���X	*//* <S0EC> */
#define ND_ASYNC_RCV_SEQ			(1)			/* �w�߃f�[�^��M�V�[�P���X		*/
#define ND_ASYNC_RCVBUF_SEQ			(2)			/* �w�߃f�[�^��M�V�[�P���X		*/
#define ND_ASYNC_SND_SEQ			(3)			/* �����f�[�^���M�V�[�P���X		*/
#define ND_ASYNC_WAIT_SND_SEQ		(4)			/* �����f�[�^���M���V�[�P���X	*/

#define MASK_INT1					0x0001

#define DEF_WDT						(10000) 	/* WDT setting 4000us�~20=80000us */
#define WDT_NOTCHK					(0)			/* ASIC�Ńz�X�gCPU��WDT���`�F�b�N���Ȃ��ݒ� */
#define DEF_MA_MAX					(MAX_AXIS_NUMBER)	/* �ڑ��X���[�u�� */
#define JL100_READY_TIMEOUT			(10000)		/* JL100�N�������^�C���A�E�g���� = 10s(10000ms) */

/* Async. communication parameter */
#define DEF_ASYNC_PEER_ADR			(0x01)		/* Destination address of Async. communication */
#define DEF_ASYNC_FTYPE				(0x02)		/* Frame type of async communication(0x02 or 0x0C) */
#define DEF_ASYNC_DATA_SIZE			(64)		/* Async. communication data size (unit : byte) */

/* C1/C2 message communication parameter */
#define MSG_PKT_SIZE				(64)		/* �ЂƂ̃��b�Z�[�W�t���[���ő��M����f�[�^���i�o�C�g�j */

/* ���j�^�X���[�u�@�\�p�����[�^ */
#define MSLV_RSP_DATA				(0)			/* ���j�^�X���[�u�@�\ �����f�[�^��M */
#define MSLV_CMD_DATA				(1)			/* ���j�^�X���[�u�@�\ �w�߃f�[�^��M */

#define ND_MAX_DATA_SIZE			(48)		/* �T�C�N���b�N�f�[�^�̍ő��M�T�C�Y */
#define RECV_STAT_REG_BIT_SIZE		(32)		/* ��M�X�e�[�^�X���W�X�^�̃r�b�g�� */

#define PHYPORTADR_CH1				(2)			/* PHY�̃|�[�g�A�h���X ch1 */
#define PHYPORTADR_CH2				(3)			/* PHY�̃|�[�g�A�h���X ch2 */

/* Message buffer size */
#define ML_MSGBUF_SZ				512			/* 480+32 */

/* Error code */
#define ML_FUNC_SUCCESS				(0)
#define ML_RAMCHECK_ERR				(-1)
#define ML_RECV_ERR					(-2)
#define ML_ABORT_ERR				(-3)
#define ML_HNDSHK_ERR				(-4)
#define ML_RXBUF_ERR				(-5)
#define ML_TXBUF_ERR				(-6)



/****************************************************************************/
/*		STRUCT DEFINITION													*/
/****************************************************************************/
/* �ʐM�p�����[�^�\���� */
typedef struct ND_PARAM_ {
	USHORT	TCycle;			/* �`������[0.25us] */
	UCHAR	NodeAddress;	/* ���ǃA�h���X */
	UCHAR	DataSize;		/* �`���o�C�g�� */
	UCHAR	MslvEnable;		/* ���j�^�X���[�u���[�h�L��[1:�L�� 0:����] */
} ND_PARAM;

/* Async. communication parameter structure */
typedef struct ASYNC_ND_PARAM_ {
	USHORT	rcvSrcAdr;		/* Received source address of async com. buffer */
	USHORT	rcvDstAdr;		/* Received destination address of async com. buffer */
	USHORT	rcvStat;		/* Receive frame status of async com. buffer */
	USHORT	rcvFtype;		/* Receive frame type of async com. buffer */
	USHORT	rcvSize;		/* Receive data size of async com. buffer */
	USHORT	sw;				/* Async. communication sequence flag */
} ASYNC_ND_PARAM;

/* typedefs of struct type */
typedef union _MLWD_DATA {
	USHORT wd;
	UCHAR  byte[2];
} MLWD_DATA;




/****************************************************************************/
/*		API's																*/
/****************************************************************************/
/* JL-100������ */
INT32 NdInit( ND_PARAM *nd_prmp, void* DeviceAddress );
/* �ʏ�ʐM���� */
INT32 NdNormalCommunicationIsReady( USHORT *t_mcycp, ULONG *t_int_ofs );
/* TMC�t���[���̎�M�m�F */
//SHORT NdReceiveTmcFrame( VOID );
/* CMD�f�[�^�擾 */
INT32 NdReceiveCMD( UINT st_no, ULONG *CmdBuffer );
/* RSP�f�[�^���M */
void NdSendRSP( UINT st_no, ULONG *RspBuffer );
/* �ʐM��Ԏ擾 */
INT32 NdBeginCommunication( ULONG *ests );
/* �`�������擾���� */
USHORT NdGetTransmissionCycle( VOID );
/* �ʐM���[�h�擾 */
USHORT NdGetComMode( VOID );
/* �񓯊��ʐM�V�[�P���X�t���O�擾 */
USHORT NdGetAsyncSw( VOID );
/* �C�x���g�h���u���ʐM���̒ʐM�f�[�^��M�i�ʐM�h���C�o�j */
//ULONG NdBeginAsyncCyclicCmdRead( void );
/* �C�x���g�h���u���ʐM���̒ʐM�f�[�^���M�i�ʐM�h���C�o�j */
//void NdSendAsyncRSP(ULONG st_no, ULONG *async_rbuff );
/* ���b�Z�[�W�p�ʐM�h���C�o */
LONG NdReqRecvMsg( UCHAR ch );
/* ���b�Z�[�W��M�����`�F�b�N */
LONG NdGetRecvMsgSts( UCHAR ch, USHORT *rcvWSz, USHORT *rcvBSz );
/* ���b�Z�[�W�f�[�^���[�h */
void NdReadMsgData( UCHAR ch, USHORT *rcvData, USHORT rdSz );
/* ���b�Z�[�W�f�[�^���C�g */
void NdWriteMsgData( UCHAR ch, USHORT *sndData, USHORT wrSz );
/* ���b�Z�[�W���M�J�n�v���Z�b�g */
LONG NdReqSendMsg( UCHAR ch );
/* ���b�Z�[�W���M�����`�F�b�N */
LONG NdGetSendMsgSts( UCHAR ch );
/* �A�{�[�g���b�Z�[�W���M */
void NdSendMsgAbort(UCHAR ch);
/* �ʐM�p�����[�^�擾 */
ND_PARAM* NdGetNdParamPtr( VOID );
/* �ʐMASIC���Z�b�g */
void NdResetAsic( void *DeviceAddress ); /* <S14E> */

/* for DMA transfer */
INT32 NdChangeTglBufAddress(ULONG *RcvAltBufOfst, ULONG *SndAltBufOfst, UINT alt_no);
void *NdGetRecvCycDevBufAddress(UINT ch);
void *NdGetSndCycDevBufAddress(UINT ch);
void *NdGetRecvCycBufferPointer(UINT ch);
void *NdGetSndCycBufferPointer(UINT ch);
ULONG NdGetSndAltBufferAddress(void);
ULONG NdGetRecvAltBufferAddress(void);

VOID NdToggleChange( VOID );

/****************************************************************************/
#endif /* NET_DRIVER_H_ */
/* End of File */

