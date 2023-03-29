/****************************************************************************
Description	: MECHATROLINK Communication Driver
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

Functions     :
	NdInit()						:JL-100������
	NdNormalCommunicationIsReady()	:�ʏ�ʐM���f�B�`�F�b�N
	NdReceiveTmcFrame()				:TMC�t���[���̎�M�m�F
	NdReceiveCMD()					:CMD�f�[�^�擾
	NdSendRSP()						:RSP�f�[�^���M
	NdGetDevStatus()				:ASIC��Ԏ擾
	NdGetComMode()					:�ʐM���[�h�擾
	NdGetAsyncSw()					:�񓯊��ʐM�V�[�P���X�t���O�擾
	NdExchangeAsyncData()			:�C�x���g�h���u���ʐM���̒ʐM�f�[�^����M����
	NdAsyncRcvRequest()				:�C�x���g�h���u���ʐM���̃t���[����M�v���Z�b�g
	NdReceiveAsyncCMD()				:�C�x���g�h���u���ʐM���̒ʐM�f�[�^��M
	NdSendAsyncRSP()				:�C�x���g�h���u���ʐM���̒ʐM�f�[�^���M

----------------------------------------------------------------------------
Changes		:
 Name   Date            Description
----------------------------------------------------------------------------
 Y.Oka	2010.02.23		created.
 I.M.   2011.01.18		reviewed. 

****************************************************************************/
#include "Basedef.h"
#include "NetDriver.h"
#include "jl100s_gbl.h"
#include "KLib.h"
#include "MLib.h"



/* JL-100 status definitions */
typedef enum ML_INITIAL_STATUS_ {
	ND_WAIT_INIT_CMP = 0,		/* ���������������҂���� */
	ND_INIT_PROC,				/* JL-100������������� */
	ND_MEASURE_TRANSDLY,		/* �`���x���v����� */
	ND_CHK_CONNECT_SLVCMP,		/* �ڑ��ǌ��o������� */
	ND_GET_CMODE_CMP,			/* �ʐM�����ʒm�t���[����M��� */
	ND_RESET_ASIC_PROC,			/* �ʐMASIC���Z�b�g�҂���� */
	ND_COM_READY_CMP,			/* �ʏ�ʐM�������� */
}ML_INITIAL_STATUS;


/* definition of function pointer type */
typedef	INT32 (*ND_BEGIN_COM_FPTR)(ULONG *);
typedef	INT32 (*ND_READ_FPTR)(UINT, ULONG *);
typedef	void (*ND_WRITE_FPTRE)(UINT, ULONG *);

typedef struct ML_NDFUNC_DEF_ {
	ND_BEGIN_COM_FPTR 	NdBeginComm;
	ND_READ_FPTR		NdReadCycData;
	ND_WRITE_FPTRE		NdWriteCycData;
} ML_NDFUNC_DEF;



typedef struct MLDRV_CHINFO {
	CHANNEL_INFO		chbuff;						/* Channel buffer								*/
	JL100_USER_PAR		usr_par;					/* Communication parameters						*/
	JL100_USER_IOMAP	usr_io_map[DEF_MA_MAX+1];	/* IO MAP Parameters(Slave 2node)				*/
	ND_PARAM			nd_prm;						/* �ʐM�p�����[�^								*/
	ASYNC_ND_PARAM 		async_nd_parm;				/* �C�x���g�h���u���ʐM�p�����[�^				*/

	ML_INITIAL_STATUS	InitialStep;				/* Step number of the M3 Initial sequence */
	ULONG				startTime;					/* ASIC���Z�b�g�J�n����					*/
	SHORT				comRdyErrInfo;				/* JL100�ʏ�ʐM�J�n���ُ̈���|�C���^*/

	/* for Cyclic Communications */
	ULONG				crcvstat[2];				/* ��M�X�e�[�^�X			*/
	ULONG				RcvAltBufAddr;
	ULONG				SndAltBufAddr;
	ULONG				RcvBufAddrOfs[DEF_MA_MAX+1];
	ULONG				SndBufAddrOfs[DEF_MA_MAX+1];
	ULONG				DmaRcvBuffer[DEF_MA_MAX+1][DEF_ASYNC_DATA_SIZE/4];
	ULONG				DmaSndBuffer[DEF_MA_MAX+1][DEF_ASYNC_DATA_SIZE/4];

	ULONG				RcvAltBufOfsAddr[2];
	ULONG				SndAltBufOfsAddr[2];

	/* for Message Communications */
	USHORT				crcv_msgsz1;				/* C1 Receive message data size buffer	*/
	USHORT				crcv_msgsz2;				/* C2 Receive message data size buffer	*/
	USHORT				csnd_msgsz;					/* Send message data size buffer		*/
	USHORT				c1msg_peer_multi;			/* for multi slave message				*/
	USHORT				c2msg_peer_multi;			/* for multi slave message				*/

	const ML_NDFUNC_DEF	*NdFunc;
} MLDRV_CHINFO;

/* Local functions */
static INT32 nd_start_normal_comunication( CHANNEL_INFO *chbuffp, USHORT *t_mcycp );
static VOID nd_set_usr_prm(JL100_USER_PAR	*usr_parp, 
				JL100_USER_IOMAP *usr_iomapp, ND_PARAM *nd_prmp );
static SHORT nd_ram_check( ULONG *pJL, USHORT size, ULONG chkdata );
static void nd_set_cycbuf_addr(MLDRV_CHINFO	*MLDevice, CHANNEL_INFO *chbuffp);

static INT32 NdBeginCyclicCommunication( ULONG *ests );
static INT32 NdReceiveCyclicCMD( UINT st_no, ULONG *CmdBuffer );
static void NdSendCyclicRSP( UINT st_no, ULONG *RspBuffer );

static INT32 NdBeginAsyncCommunication( ULONG *ests );
static INT32 NdReceiveAsyncCMD( UINT st_no, ULONG *CmdBuffer );
static void NdSendAsyncRSP(UINT st_no, ULONG *async_rbuff );


/* Local variables */
static MLDRV_CHINFO		MLDevice;		/* ML Device driver information */

const ML_NDFUNC_DEF ML_CyclicFunc = {
                                     &NdBeginCyclicCommunication,
                                     &NdReceiveCyclicCMD,
                                     &NdSendCyclicRSP
};
const ML_NDFUNC_DEF ML_AsyncFunc = {
                                     &NdBeginAsyncCommunication,
                                     &NdReceiveAsyncCMD,
                                     &NdSendAsyncRSP
};


/********************************************************************************/
/*																				*/
/*	�֐��� :	NdInit()														*/
/*																				*/
/*	�@�\�T�v�F																	*/
/*		MECHATROLINK-3�ʐM�̏���ASIC�`�F�b�N����								*/
/*																				*/
/*==============================================================================*/
/*																				*/
/*	���� :																		*/
/*		ND_PARAM* nd_prmp		�ʐM�p�����[�^�i�A�v���ݒ�j�̃|�C���^			*/
/*		ULONG devOpt			ASIC�̎�ʂ��w��i���g�p�j						*/
/*	�߂�l :																	*/
/*		= ND_SUCCESS			����I��										*/
/*		= ND_SRAM_CHECK_ERROR	JL-100����RAM�`�F�b�N�T���ُ�					*/
/*		= ND_JL100_READY_TIMEOUTJL-100����RAM�`�F�b�N�T���ُ�					*/
/*																				*/
/*==============================================================================*/
/*																				*/
/*	Note	:	�ʐM�����������͂��̊֐�������I�����A							*/
/*				NdIsNormalCommunicationReady()=ND_SUCCESS�ƂȂ邱�ƂŊ����ƂȂ�	*/
/*	Log		:																	*/
/*																				*/
/********************************************************************************/
INT32 NdInit( ND_PARAM *nd_prmp, void *DeviceAddress )
{
	ML_INITIAL_STATUS	*init_stsp;		/* JL100���������						*/
	USHORT				*async_swp;		/* �񓯊��ʐM�V�[�P���X�ԍ�				*/
	ULONG				*pJL;			/* JL-100 register top address.			*/
	ND_PARAM			*ndprmp;		/* �ʐM�p�����[�^�i�ۑ��p�j�̃|�C���^	*/
	CHANNEL_INFO		*chbuffp;		/* JL-100�A�N�Z�X��`�|�C���^			*/
	JL100_USER_PAR		*usr_parp;		/* JL100_USER_PAR�|�C���^				*/
	JL100_USER_IOMAP	*usr_io_mapp;	/* JL100_USER_IOMAP�|�C���^				*/
	VULONG				Timer;			

	MlibResetByteMemory(&MLDevice, sizeof(MLDevice));
	/* Get the pointer of Buffer */
	init_stsp	= &MLDevice.InitialStep;
	async_swp   = &MLDevice.async_nd_parm.sw;
	ndprmp		= &MLDevice.nd_prm;
	chbuffp		= &MLDevice.chbuff;
	usr_parp	= &MLDevice.usr_par;
	usr_io_mapp = MLDevice.usr_io_map;
	MLDevice.NdFunc = &ML_AsyncFunc;

	/* Set JL-100 register top address */
	pJL = (ULONG*)DeviceAddress;
	chbuffp->hif_reg_top = (JL100_HOST_IF_REGS *)DeviceAddress;

#ifdef _VC_DEBUG_
	return ND_SUCCESS;
#endif

	/* jl100s_init_sts Initialization */
	*init_stsp = ND_WAIT_INIT_CMP;
	/* �񓯊��V�[�P���X�ԍ������� */
	*async_swp = ND_ASYNC_REQ_RCV_SEQ;
	/* �ʐM�p�����[�^�̒l��ۑ� */
	*ndprmp = *nd_prmp;

	/* M-III Core Software Reset */
	jl100s_reset_asic( chbuffp );		/* <S01A> */

	/* JL-100 Set up */
	Timer = 0;
	while( WAIT_SETUP == jl100s_setup_asic( pJL ))
	{
		KlibWaitms(1);
		Timer++;
		if( Timer > JL100_READY_TIMEOUT )
		{/* �^�C���A�E�g 10[s] */
			return ND_JL100_READY_TIMEOUT;
		}
	}

	/* JL-100�N�������҂� */
	Timer = 0;
	while( LIB_OK != jl100s_chk_asic_ready(pJL) )
	{
		KlibWaitms(1);
		Timer++;
		if( Timer > JL100_READY_TIMEOUT )
		{/* �^�C���A�E�g 10[s] */
			return ND_JL100_READY_TIMEOUT;
		}
	}

	/* Check & Clear JL-100 RAM */
	if( nd_ram_check( pJL, DEF_SRAM_SIZE, 0x5a5a5a5a) != ND_SUCCESS )
	{
		return ND_SRAM_CHECK_ERROR;
	}
	if( nd_ram_check( pJL, DEF_SRAM_SIZE, 0xa5a5a5a5) != ND_SUCCESS )
	{
		return ND_SRAM_CHECK_ERROR;
	}
	if( nd_ram_check( pJL, DEF_SRAM_SIZE, 0x00000000) != ND_SUCCESS )
	{
		return ND_SRAM_CHECK_ERROR;
	}

	/* ���[�U�Őݒ肪�K�v�ȒʐM�p�����[�^�̐ݒ� */
	nd_set_usr_prm( usr_parp, usr_io_mapp, nd_prmp );
	/* JL-100 Initialization */
	if( jl100s_initialize(chbuffp, pJL, usr_parp, usr_io_mapp) == LIB_OK )
	{
		nd_set_cycbuf_addr(&MLDevice, chbuffp);
		*init_stsp = ND_MEASURE_TRANSDLY;
	}
	else
	{
		*init_stsp = ND_INIT_PROC;
	}
	
	return ND_SUCCESS;
}

/********************************************************************************/
/*																				*/
/*	�֐��� :	NdNormalCommunicationReady()									*/
/*																				*/
/*	�@�\�T�v�F																	*/
/*		MECHATROLINK-3�ʐM�̒ʏ�ʐM��������									*/
/*																				*/
/*==============================================================================*/
/*																				*/
/*	���� :																		*/
/*		USHORT* t_mcycp			�`������[0.25us]�̃|�C���^						*/
/*		ULONG*  t_int_ofs		Interrupt Delay Time							*/
/*	�߂�l :																	*/
/*		= ND_SUCCESS			����I��										*/
/*		= ND_CONTINUE			�����������s��									*/
/*		= ND_COMM_STAT_RESET	�ʐM��ԍĐݒ�v��								*/
/*		= ��L�ȊO				�����������s									*/
/*																				*/
/*==============================================================================*/
/*																				*/
/*	Note	:	�{�֐���NdInit()�֐��̐���I������s���邱��					*/
/*				�{�֐��͒჌�x���̃^�X�N�ŏ펞���s���邱��						*/
/*	Log		:	�֐��̑�������SGDV-MD�ɂ��킹�� <S0EC>	 					*/
/*																				*/
/********************************************************************************/
INT32 NdNormalCommunicationIsReady( USHORT *t_mcycp, ULONG *t_int_ofs )
{
	INT32				ret;			/* return code							*/
	ULONG				*pJL;			/* JL-100 register top address			*/
	CHANNEL_INFO		*chbuffp;		/* JL-100�A�N�Z�X��`�|�C���^			*/
	JL100_USER_PAR		*usr_parp;		/* JL100_USER_PAR�|�C���^				*/
	JL100_USER_IOMAP	*usr_io_mapp;	/* JL100_USER_IOMAP�|�C���^				*/
	ND_PARAM			*nd_prmp;		/* �ʐM�p�����[�^						*/
	ULONG				t_mcyc_10ns;	/* �`������ 10ns						*/

	/* Get the pointer of Buffer */
	chbuffp			= &MLDevice.chbuff;
	usr_parp		= &MLDevice.usr_par;
	usr_io_mapp 	= MLDevice.usr_io_map;
	nd_prmp			= &MLDevice.nd_prm;

	/* Set JL-100 register top address */
	pJL = (ULONG*)chbuffp->hif_reg_top;

	switch( MLDevice.InitialStep )
	{
	case ND_INIT_PROC:
		/* ���[�U�Őݒ肪�K�v�ȒʐM�p�����[�^�̐ݒ� */
//		nd_set_usr_prm( usr_parp, usr_io_mapp, nd_prmp );
		/* JL-100 Initialization */
		if( jl100s_initialize(chbuffp, pJL, usr_parp, usr_io_mapp) == LIB_OK )
		{
//			nd_set_cycbuf_addr(&MLDevice, chbuffp);
			MLDevice.InitialStep = ND_MEASURE_TRANSDLY;
			ret = ND_CONTINUE;
		}
		else
		{
			ret = ND_JL100_INIT_ERROR;
		}
		break;

	case ND_MEASURE_TRANSDLY:
		/* Check complete detect connecting slave */
		ret = jl100s_chk_detect_slv_cmp( chbuffp );
		if( ret == LIB_OK )
		{
			/* Excute measure transmission delay time */
			ret = jl100s_req_measure_transdly( chbuffp );
			if(ret == LIB_OK)
			{
				MLDevice.InitialStep = ND_CHK_CONNECT_SLVCMP;
				ret = ND_CONTINUE;
			}
			else
			{
				ret =  ND_MEASURE_TRANSDLY_ERROR;
			}
		}
		else if( ret == WAIT_CMP_DETECT )
		{
			// ��ʃR���g���[�������I�����C���ɂȂ邩�킩��Ȃ��̂�
			// �������ԃI�[�o�̃`�F�b�N�͍s��Ȃ��I�I
			ret = ND_CONTINUE;
		}
		else
		{
//			MLDevice.InitialStep = ND_INIT_PROC;
			ret = ND_DETECT_SLV_CMP_ERROR;
		}

		break;

	case ND_CHK_CONNECT_SLVCMP:
		/* Get communication mode */
		ret = jl100s_get_cmode(chbuffp);
		if( ret == LIB_OK )
		{
			MLDevice.InitialStep = ND_GET_CMODE_CMP;
			ret = ND_CONTINUE;
		}
		else if( (ret == WAIT_TRANSCYC_FRM) || (ret == MEASURING_TRANSDLY) )
		{
			// ��ʃR���g���[�������I�����C���ɂȂ邩�킩��Ȃ��̂�
			// �������ԃI�[�o�̃`�F�b�N�͍s��Ȃ��I�I
			ret = ND_CONTINUE;
		}
		else if(ret == ERROR_RESET_ASIC)
		{
			jl100s_reset_asic( chbuffp );
			KlibRstLongTimer(&MLDevice.startTime);
			MLDevice.InitialStep = ND_RESET_ASIC_PROC;
			ret = ND_CONTINUE;
		}
		else
		{
			ret = ND_GET_CMODE_ERROR;
		}
		break;

	case ND_GET_CMODE_CMP:
		ret = nd_start_normal_comunication( chbuffp, &nd_prmp->TCycle);
		if(ret == ND_SUCCESS)
		{
			MLDevice.InitialStep = ND_COM_READY_CMP;
			*t_int_ofs = jl100_get_idly(chbuffp->hif_reg_top); /* <S0EC> */
		}
		*t_mcycp = nd_prmp->TCycle;
		break;

	case ND_RESET_ASIC_PROC:
		/* Check ASIC ready (ASIC ready��ԂɂȂ�܂ő҂�) */
		if(( jl100s_chk_asic_ready(pJL) ) == LIB_OK)
		{
			MLDevice.InitialStep = ND_INIT_PROC;
			ret = ND_CONTINUE;
		}
		else
		{
			ret = ND_CONTINUE;
			if( KlibGetLongTimerMs(MLDevice.startTime) > JL100_READY_TIMEOUT )
			{/* �^�C���A�E�g */
				ret = ND_JL100_READY_TIMEOUT;
			}
		}
		break;

	case ND_COM_READY_CMP:
		/* TMC�t���[����M��Ԋm�F */
		ret = ND_CONTINUE;
		if( RCVED_TMC_FRAME == jl100s_chk_rcv_tmcframe(chbuffp, &t_mcyc_10ns) )
		{ /* TMC�t���[������M������ */
			MLDevice.nd_prm.TCycle = (USHORT)( t_mcyc_10ns / 25 );	/* �P�ʕϊ� 10ns �� 250ns */
			MLDevice.InitialStep = ND_GET_CMODE_CMP;
			ret = ND_RCVED_TMC_FRAME;
		}
		else
		{
			/* Asic ready check */
			if(( jl100s_chk_asic_ready(pJL) ) != LIB_OK)
			{
				ret = ND_JL100_READY_TIMEOUT;
			}
		}
		break;

	default:
		MLDevice.InitialStep = ND_INIT_PROC;
		ret = ND_INIT_SEQ_ERROR;
		break;
	}
	
	/* �ُ���̕ۑ� */
	if( ret < 0 )
	{
		MLDevice.comRdyErrInfo = (SHORT)ret;
	}

	return ret;
}


/****************************************************************************/
/* �T�v�F�ʐM���[�h�̃`�F�b�N���s���A�����ʐM�J�n�����Ⴕ����				*/
/*		 �񓯊��ʐM�J�n�������J�n����										*/
/****************************************************************************/
/* ����		�F	CHANNEL_INFO* chbuffp	JL-100�A�N�Z�X��`�|�C���^			*/
/*				USHORT* t_mcycp			�`������[0.25us]					*/
/*				USHORT* init_stsp		JL100���������						*/
/*				ULONG* start_time		JL100���Z�b�g�J�n����				*/
/* �߂�l   �F	= ND_SUCCESS			�ʐM��������������					*/
/*				= ND_CONTINUE			�����p����							*/
/*				= ��L�ȊO				�ُ�I��							*/
/****************************************************************************/
static INT32 nd_start_normal_comunication( CHANNEL_INFO *chbuffp, USHORT *t_mcycp )
{
	INT32	ret,i;
	ULONG	t_mcyc_10ns;						/* �`������ 10ns			*/

	/* Get transmission cycle */
	jl100s_get_trans_cycle( chbuffp, &t_mcyc_10ns );
	*t_mcycp = (USHORT)(t_mcyc_10ns/25);		/* �P�ʕϊ� 10ns �� 250ns	*/
	/* 2009.09.01 Y.Oka ���󖢑Ή� */

	MLDevice.crcvstat[0] = 0;
	/* Start cyclic communication */
	if(chbuffp->prot_sel == COM_MODE_SYNC)
	{
		if(LIB_OK != jl100s_start_sync(chbuffp))
		{
			ret = ND_START_SYNC_ERROR;
		}
		else
		{
			MLDevice.NdFunc = &ML_CyclicFunc;
			ret = ND_SUCCESS;
		}
	}
	else if(chbuffp->prot_sel == COM_MODE_ASYNC)
	{
		MLDevice.async_nd_parm.sw = ND_ASYNC_REQ_RCV_SEQ;
		for(i = 0; i < (DEF_ASYNC_DATA_SIZE/4); i++)
		{	MLDevice.DmaRcvBuffer[0][i] = 0;}

//		ret = ND_SUCCESS;
		if(LIB_OK != jl100s_start_async(chbuffp))
		{
			ret = ND_START_ASYNC_ERROR;
		}
		else
		{
			MLDevice.NdFunc = &ML_AsyncFunc;
			ret = ND_SUCCESS;
		}
	}
	else
	{	/* ���T�|�[�g�ʐM���[�h */
		ret = ND_PORT_SEL_ERROR;
	}
	return ret;
}

#if 0
/********************************************************************************/
/*																				*/
/*	�֐��� :	NdReceiveTmcFrame()												*/
/*																				*/
/*	�@�\�T�v�F																	*/
/*		�ʏ�^�]��Ԃ�TMC�t���[���̎�M�m�F���s���B								*/
/*	TMC�t���[���̎�M�m�F�ɂ���ʃR���g���[���̒ʐM���Z�b�g�����o����			*/
/*																				*/
/*==============================================================================*/
/*																				*/
/*	���� :																		*/
/*	�߂�l :																	*/
/*		= ND_RCVED_TMC_FRAME	TMC�t���[����M									*/
/*		= ND_NOT_RCV_TMC_FRAME	TMC�t���[������M								*/
/*																				*/
/*==============================================================================*/
/*																				*/
/*	Note	:	�{�֐���M3�ʐM��ԂŎ��s���邱��								*/
/*				�{�֐��͒჌�x���̃^�X�N�ŏ펞���s���邱��						*/
/*	Log		:																	*/
/*																				*/
/********************************************************************************/
SHORT NdReceiveTmcFrame( VOID )
{
	SHORT 			ret;					/* return code						*/
	ULONG			t_mcyc_10ns;			/* return code						*/
	CHANNEL_INFO	*chbuffp;				/* JL-100�A�N�Z�X��`�|�C���^		*/

	/* ���[�J���ϐ��̏����� */
	chbuffp		= &MLDevice.chbuff;

	/* TMC�t���[����M��Ԋm�F */
	ret = jl100s_chk_rcv_tmcframe( chbuffp, &t_mcyc_10ns );

	if( ret == RCVED_TMC_FRAME )
	{ /* TMC�t���[������M������ */
		MLDevice.nd_prm.TCycle = (USHORT)( t_mcyc_10ns / 25 );	/* �P�ʕϊ� 10ns �� 250ns */
		MLDevice.InitialStep = ND_GET_CMODE_CMP;
		MLDevice.async_nd_parm.sw = ND_ASYNC_REQ_RCV_SEQ;
		MLDevice.crcvstat[0] = 0;
		ret = ND_RCVED_TMC_FRAME;
	}
	return ret;
}
#endif


/********************************************************************************/
/*																				*/
/*	�֐��� :	�T�C�N���b�N/�C�x���g�h���u���ʐM����							*/
/*																				*/
/********************************************************************************/
INT32 NdBeginCommunication( ULONG *ests )
{
	return (MLDevice.NdFunc)->NdBeginComm(ests);
}

INT32 NdReceiveCMD( UINT st_no, ULONG *Buffer )
{
	return (MLDevice.NdFunc)->NdReadCycData(st_no, Buffer);
}

void NdSendRSP( UINT st_no, ULONG *Buffer )
{
	(MLDevice.NdFunc)->NdWriteCycData(st_no, Buffer);
}



/********************************************************************************/
/*																				*/
/*	�֐��� :	NdReceiveCyclicCMD()											*/
/*																				*/
/*	�@�\�T�v:																	*/
/*		�w�߃f�[�^�̎�M�����i�����ʐM�j										*/
/*																				*/
/*==============================================================================*/
/*																				*/
/*	���� :																		*/
/*		ULONG* lnk_cbuff		�w�߃f�[�^�̎�M�o�b�t�@						*/
/*	�߂�l :																	*/
/*		INT32 					��M�ُ펲��									*/
/*																				*/
/********************************************************************************/
static INT32 NdReceiveCyclicCMD( UINT st_no, ULONG *CmdBuffer )
{
	INT32 			ret;					/* return code						*/
	CHANNEL_INFO	*chbuffp;				/* JL-100�A�N�Z�X��`�|�C���^		*/
	ULONG			*rdp;					/* for RAM access */
	USHORT			size_buf;				/* Responce data length from IOMAP */

	/* Receive [JL-100 -> buffer] */
	if(0 != (MLDevice.crcvstat[0] & ( 1 << st_no )))
	{/* CMD�f�[�^�����M */
		/* Get response data length */
		chbuffp = &MLDevice.chbuff;
		size_buf = chbuffp->usr_iomap[st_no].cd_len;

		/* Read receive data */
#if (CSW_COMASIC_DMA_TRANSFER == FALSE)
		rdp = (ULONG *)(MLDevice.RcvAltBufAddr + MLDevice.RcvBufAddrOfs[st_no]);
#else
		rdp = MLDevice.DmaRcvBuffer[st_no];
#endif
		while(size_buf > 0)
		{
			*CmdBuffer = jl100_read_dwdata(rdp);
			CmdBuffer++;
			size_buf -= DWORD_SIZE;
		}
		ret = ND_SUCCESS;
	}
	else
	{
		ret = ND_DATA_RECV_ERROR;
	}

	return ret;
}


/********************************************************************************/
/*																				*/
/*	�֐��� :	NdSendCyclicRSP()												*/
/*																				*/
/*	�@�\�T�v:																	*/
/*		�����f�[�^���M�����i�����ʐM�j											*/
/*																				*/
/*==============================================================================*/
/*																				*/
/*	���� :																		*/
/*		INT ch					�`�����l���ԍ�									*/
/*		ULONG* lnk_rbuff		���M�f�[�^�̑��M�o�b�t�@						*/
/*	�߂�l :																	*/
/*		= ND_SUCCESS			����I��										*/
/*		= ��L�ȊO				�����f�[�^�������ݎ��s							*/
/*																				*/
/********************************************************************************/
void NdSendCyclicRSP( UINT st_no, ULONG *RspBuffer )
{
	CHANNEL_INFO	*chbuffp;					/* JL-100�A�N�Z�X��`�|�C���^	*/
	ULONG	*wtp;						/* for RAM access */
	USHORT	size_buf;					/* Command data length from IOMAP */

	chbuffp = &MLDevice.chbuff;
//	jl100s_write_ldata( chbuffp, (USHORT)st_no, (ULONG*)RspBuffer );

	size_buf = chbuffp->usr_iomap[st_no].rd_len;
	/* Write command data */
#if (CSW_COMASIC_DMA_TRANSFER == FALSE)
	wtp = (ULONG *)(MLDevice.SndAltBufAddr + MLDevice.SndBufAddrOfs[st_no]);
#else
	wtp = MLDevice.DmaSndBuffer[st_no];
#endif
	while(size_buf > 0)
	{
		jl100_write_dwdata(wtp, RspBuffer);
		size_buf -= DWORD_SIZE;
	}
}

/********************************************************************************/
/*																				*/
/*	�֐��� :	NdGetDevStatus()												*/
/*																				*/
/*	�@�\�T�v:																	*/
/*		�T�C�N���b�N�ʐM�J�n & �ʐM��Ԏ擾�����iASIC��ԁj						*/
/*																				*/
/*==============================================================================*/
/*																				*/
/*	���� :																		*/
/*		ULONG *ests				�G���[�X�e�[�^�X								*/
/*	�߂�l :																	*/
/*		= ND_SUCCESS				����I��									*/
/*																				*/
/********************************************************************************/
static INT32 NdBeginCyclicCommunication( ULONG *ests )
{
	ULONG			i;
	VULONG			asic_stat;
	CHANNEL_INFO	*chbuffp;		/* JL-100�A�N�Z�X��`�|�C���^				*/
	JL100_HOST_IF_REGS *hif_reg_top;

	chbuffp = &MLDevice.chbuff;
	hif_reg_top = chbuffp->hif_reg_top;

	/* Request change receive buffer */
	jl100_set_cmd(hif_reg_top, (JL100_CMD_CBX_R|JL100_CMD_RBX_R|JL100_CMD_WD_R));
	/* Clear interrupt factor */
	jl100_clear_ints(hif_reg_top, JL100_INTS_TIM1);

	/* Toggle response buffer */
//	jl100s_change_cbuff( chbuffp );
//	jl100s_change_rbuff( chbuffp );
//	jl100s_begin_cyclic_com( chbuffp );

	/* Get ESTS register */
	asic_stat = jl100s_read_comstat(chbuffp);
	*ests = asic_stat & (JL100_ESTS_WDTOVR | JL100_ESTS_RXLEN_ERR | JL100_ESTS_FCS_ERR);

	/* Get command receive status */
	MLDevice.crcvstat[0] = jl100_get_crcv1(hif_reg_top);
//	jl100s_get_crcvstat(chbuffp, MLDevice.crcvstat);

	for(i = 0; i < JL100_CMD_POLLING_CNT; i++)
	{
		asic_stat = jl100_get_cmd(hif_reg_top);
		if((asic_stat & (JL100_CMD_CBX_R|JL100_CMD_RBX_R)) == 0)
		{
			break;
		}
	}
	MLDevice.RcvAltBufAddr = (USHORT)jl100_get_cbadr(hif_reg_top);
	MLDevice.SndAltBufAddr = (USHORT)jl100_get_rbadr(hif_reg_top);

	return (MLDevice.crcvstat[0] & 1);
}

/****************************************************************************/
/* �T�v�F�`�������擾����													*/
/****************************************************************************/
/* ����		�F	�Ȃ�														*/
/* �߂�l   �F	�`������[0.25us]											*/
/****************************************************************************/
USHORT NdGetTransmissionCycle( VOID )
{
	return MLDevice.nd_prm.TCycle;
}

/****************************************************************************/
/* �T�v�F�ʐM���[�h�擾����													*/
/****************************************************************************/
/* ����		�F	�Ȃ�														*/
/* �߂�l   �F	COM_MODE_SYNC	�T�C�N���b�N�ʐM���[�h						*/
/*			�F	COM_MODE_ASYNC	�C�x���g�h���u���ʐM���[�h					*/
/*			�F	��L�ȊO		�ʐM���[�h�ُ�ݒ�							*/
/****************************************************************************/
USHORT NdGetComMode( VOID )
{
//	CHANNEL_INFO	*chbuffp;		/* JL-100�A�N�Z�X��`�|�C���^ */
//	chbuffp = &MLDevice.chbuff;				/* �|�C���^�ɃA�h���X���Z�b�g */

//	return chbuffp->prot_sel;
	return ((MLDevice.NdFunc == &ML_AsyncFunc) ? COM_MODE_ASYNC : COM_MODE_SYNC);
}

#if 0
/****************************************************************************/
/* �T�v�F�񓯊��ʐM�V�[�P���X�t���O�擾����									*/
/****************************************************************************/
/* ����		�F	�Ȃ�														*/
/* �߂�l   �F	ASYNC_RCV_SEQ	�w�߃f�[�^��M�V�[�P���X					*/
/*			�F	ASYNC_SND_SEQ	�����f�[�^���M�V�[�P���X					*/
/****************************************************************************/
USHORT NdGetAsyncSw( VOID )
{
	ASYNC_ND_PARAM* asy_prmp;	/* �C�x���g�h���u���ʐM�p�����[�^ */
	asy_prmp = &MLDevice.async_nd_parm;	/* �|�C���^�ɃA�h���X���Z�b�g */

	return asy_prmp->sw;
}

/****************************************************************************/
/* �T�v�F�񓯊��ʐM�V�[�P���X�t���O�ݒ菈��									*/
/****************************************************************************/
/* ����		�F	�񓯊��ʐM�V�[�P���X�t���O									*/
/* �߂�l   �F	�Ȃ�														*/
/****************************************************************************/
VOID NdSetAsyncSw( USHORT sw )
{
	ASYNC_ND_PARAM* asy_prmp;	/* �C�x���g�h���u���ʐM�p�����[�^ */
	asy_prmp = &MLDevice.async_nd_parm;	/* �|�C���^�ɃA�h���X���Z�b�g */

	asy_prmp->sw = sw;
}
#endif

/****************************************************************************/
/* �T�v�F�X���[�u�p�̒ʐM�p�����[�^��ݒ肷��								*/
/****************************************************************************/
/* ����		�F	JL100_USER_PAR	*usr_parp	JL-100���[�U�p�����[�^�̃|�C���^*/
/*				JL100_USER_IOMAP* usr_iomapp JL-100���[�UI/O�}�b�v�̃|�C���^*/
/*				ND_PARAM* nd_prmp	�ʐM�p�����[�^�i�A�v���ݒ�j�̃|�C���^	*/
/* �߂�l   �F	�Ȃ�														*/
/****************************************************************************/
static VOID nd_set_usr_prm(JL100_USER_PAR *usr_parp, 
							JL100_USER_IOMAP *usr_iomapp, ND_PARAM *nd_prmp )
{
	LONG ax_no;

	/* set default user parameter setting */
#if CSW_BOARD_TYPE == SGD7S_TYPE /* <S0A2> */
	usr_parp->mod = JL100_MOD_TYPE_SLV | JL100_MOD_INT_FR | JL100_MOD_INTLV_PLS;
#else /* CSW_BOARD_TYPE == SGD7W_TYPE */
	usr_parp->mod = JL100_MOD_TYPE_MULTISLV | JL100_MOD_INT_FR | JL100_MOD_INTLV_PLS;
#endif /* CSW_BOARD_TYPE == SGD7S_TYPE */
	usr_parp->ma0 = nd_prmp->NodeAddress;			/* My Address from switch */
	usr_parp->ma_max = MAX_AXIS_NUMBER;				/* MAX slave node  */
//	usr_parp->wdt = DEF_WDT;						/* Watch dog timer [8usec]; if wdt=0, WDT function disabled */
	usr_parp->wdt = WDT_NOTCHK;						/* <2008.10.22 Y.Oka (>_<) ASIC�Ńz�X�gCPU��WDT���`�F�b�N���Ȃ��ݒ�> */
	usr_parp->pkt_sz = MSG_PKT_SIZE;				/* Message communication packet size */
	usr_parp->max_rtry = 0;							/* Reserve (default:0) */
	usr_parp->c2_dly   = 0;							/* Reserve (default:0) */
	usr_parp->dly_cnt  = 0;							/* Reserve (default:0) */

	/* Set IOMAP parameters(C1 master) */
	usr_iomapp->axis_adr = 0x0001;					/* Station address */
	usr_iomapp->t_rsp  = 0;							/* Reserve(default:0) */
	usr_iomapp->cd_len = 8;							/* Command data length */
	usr_iomapp->rd_len = 8;							/* Response data length */

	/* Set IOMAP prameters(slave) */
	for(ax_no = 1; ax_no <= MAX_AXIS_NUMBER; ax_no++)
	{
		(usr_iomapp+ax_no)->axis_adr = 
			(USHORT)(((ax_no-1) << 8) | nd_prmp->NodeAddress);	/* Station address */
		(usr_iomapp+ax_no)->t_rsp = 0;							/* reserve : default 0 */
		(usr_iomapp+ax_no)->cd_len = nd_prmp->DataSize;			/* Commando data length */
		(usr_iomapp+ax_no)->rd_len = nd_prmp->DataSize;			/* Response data length */
	}
}

static void nd_set_cycbuf_addr(MLDRV_CHINFO	*MLDevice, CHANNEL_INFO *chbuffp)
{
	LONG			ax_no;

	chbuffp->usr_iomap[0].cmdbuff_offset = 0;	/* <S0EC> */
	chbuffp->usr_iomap[0].rspbuff_offset = 0;

	/* Set IOMAP prameters(slave) */
	for(ax_no = 0; ax_no <= MAX_AXIS_NUMBER; ax_no++)
	{
		MLDevice->RcvBufAddrOfs[ax_no] = ((ULONG)chbuffp->hif_reg_top + DEF_SRAM_TOP_ADR);
		MLDevice->RcvBufAddrOfs[ax_no] += chbuffp->usr_iomap[ax_no].cmdbuff_offset;
		MLDevice->RcvBufAddrOfs[ax_no] += HEADER_SIZE;

		MLDevice->SndBufAddrOfs[ax_no] = ((ULONG)chbuffp->hif_reg_top + DEF_SRAM_TOP_ADR);
		MLDevice->SndBufAddrOfs[ax_no] += chbuffp->usr_iomap[ax_no].rspbuff_offset;
		MLDevice->SndBufAddrOfs[ax_no] += HEADER_SIZE;
	}
}

/****************************************************************************/
/* �T�v�FJL-100 �����������p�g�O���A�h���X�X�V����							*/
/****************************************************************************/
/* ����		�F	�Ȃ�														*/
/* �߂�l   �F	0�ȊO�F�G���[												*/
/****************************************************************************/
INT32 NdChangeTglBufAddress(ULONG *RcvAltBufOfst, ULONG *SndAltBufOfst, UINT alt_no)
{
	INT32			ret1, ret2;
	CHANNEL_INFO	*chbuffp;

	chbuffp	= &MLDevice.chbuff;

	/* Toggle buffer */
	ret1 = jl100s_change_rbuff( chbuffp );
	ret2 = jl100s_change_cbuff( chbuffp );
	MLDevice.RcvAltBufAddr = (USHORT)jl100_get_cbadr(chbuffp->hif_reg_top);
	MLDevice.SndAltBufAddr = (USHORT)jl100_get_rbadr(chbuffp->hif_reg_top);
	MLDevice.RcvAltBufOfsAddr[alt_no] = MLDevice.RcvAltBufAddr;
	MLDevice.SndAltBufOfsAddr[alt_no] = MLDevice.SndAltBufAddr;

	if(SndAltBufOfst != NULL)
	{	
		*SndAltBufOfst = MLDevice.SndAltBufAddr;
	}
	if(RcvAltBufOfst != NULL)
	{	
		*RcvAltBufOfst = MLDevice.RcvAltBufAddr;
	}

	return (ret1|ret2);
}

/****************************************************************************/
/* �T�v�FJL-100 �T�C�N���b�N�o�b�t�@�A�h���X�擾����						*/
/****************************************************************************/
/* ����		�F	UINT ch					�`�����l���ԍ�						*/
/*				ULONG *AltBufOfst		�g�O���o�b�t�@�I�t�Z�b�g�A�h���X	*/
/* �߂�l   �F	�o�b�t�@�A�h���X											*/
/****************************************************************************/
void *NdGetRecvCycDevBufAddress(UINT ch)
{
	return (void*)(MLDevice.RcvAltBufAddr + MLDevice.RcvBufAddrOfs[ch]);
}

void *NdGetSndCycDevBufAddress(UINT ch)
{
	return (void*)(MLDevice.SndAltBufAddr + MLDevice.SndBufAddrOfs[ch]);
}

void *NdGetRecvCycBufferPointer(UINT ch)
{
	return (void*)MLDevice.DmaRcvBuffer[ch];
}

void *NdGetSndCycBufferPointer(UINT ch)
{
	return (void*)MLDevice.DmaSndBuffer[ch];
}

ULONG NdGetRecvAltBufferAddress(void)
{
	return ((MLDevice.RcvAltBufOfsAddr[0] == MLDevice.RcvAltBufAddr) ? 0 : 1);
//	return (MLDevice.RcvAltBufOfsAddr[0] != MLDevice.RcvAltBufAddr);
}
ULONG NdGetSndAltBufferAddress(void)
{
//	return (MLDevice.SndAltBufOfsAddr[0] != MLDevice.SndAltBufAddr);
	return ((MLDevice.SndAltBufOfsAddr[0] == MLDevice.SndAltBufAddr) ? 0 : 1);
}
/****************************************************************************/
/* �T�v�FJL-100 ����RAM�`�F�b�N����											*/
/****************************************************************************/
/* ����		�F	ULONG *pJL		JL100 register top address					*/
/*				USHORT size		SRAM �f�[�^�T�C�Y							*/
/*				ULONG  chkdata	�`�F�b�N�f�[�^								*/
/* �߂�l   �F	�Ȃ�														*/
/****************************************************************************/
static SHORT nd_ram_check( ULONG *pJL, USHORT size, ULONG chkdata )
{
	VULONG	*pVal;			/* Host I/F Top address							*/
	ULONG	rdata;			/* Read data									*/
	USHORT	ret;			/* return code									*/
	int		i;				/* loop counter									*/

	/* Set JL-100 register top address */
	pVal = (VULONG*)pJL;

	/* Write data */
	ret = jl100s_write_ram( pJL, 0, size, chkdata );
	if(ret != LIB_OK)	return ret;

	/* Read and verify writed data */
	for( i = 0; i < size; i+=4 )
	{
		ret = jl100s_read_ram( (ULONG*)pVal, i, 4, &rdata );
		if(ret != LIB_OK)	return ret;

		if(rdata != chkdata)
			return ND_SRAM_CHECK_ERROR;
	}
	return ND_SUCCESS;
}

//#ifdef M3FSEL_EVENT_DRIVEN_ENABLE
/********************************************************************************/
/*																				*/
/*	�֐��� :	NdGetDevStatus()												*/
/*																				*/
/*	�@�\�T�v:																	*/
/*		�T�C�N���b�N�ʐM�J�n & �ʐM��Ԏ擾�����iASIC��ԁj						*/
/*																				*/
/*==============================================================================*/
/*																				*/
/*	���� :																		*/
/*		ULONG *ests				�G���[�X�e�[�^�X								*/
/*	�߂�l :																	*/
/*		= ND_SUCCESS				����I��									*/
/*																				*/
/********************************************************************************/
static INT32 NdBeginAsyncCommunication( ULONG *ests )
{
//	ULONG			asic_stat;
//	CHANNEL_INFO	*chbuffp;		/* JL-100�A�N�Z�X��`�|�C���^				*/

//	chbuffp = &MLDevice.chbuff;

//	jl100s_begin_cyclic_com( chbuffp );

	/* Get ESTS register */
//	asic_stat = jl100s_read_comstat(chbuffp);

	MLDevice.crcvstat[0] = 0;
	*ests = 0;

//	JL-102��WDT�@�\�͖��g�p�Ȃ̂ŏ����s�v /* <S0EC> */
	return 0;//(asic_stat & JL100_ESTS_WDTOVR);
}


/********************************************************************************/
/*																				*/
/*	�֐��� :	NdReceiveAsyncCMD()												*/
/*																				*/
/*	�@�\�T�v:																	*/
/*		�C�x���g�h���u���ʐM���̒ʐM�f�[�^��M�i�ʐM�h���C�o�j					*/
/*																				*/
/*==============================================================================*/
/*																				*/
/*	���� :																		*/
/*		ULONG* async_cbuff		�w�߃f�[�^�̃o�b�t�@							*/
/*	�߂�l :																	*/
/*		= ND_ASYNC_RCV_SUCCESS	����I���i��M�j								*/
/*		= ND_ASYNC_RCV_ERR		��M���G���[���o								*/
/*		= ND_ASYNC_TOUT			��M�����^�C���A�E�g							*/
/*																				*/
/********************************************************************************/
static INT32 NdReceiveAsyncCMD( UINT st_no, ULONG *CmdBuffer )
{
	INT32			ret,i;
	ULONG			no;
	CHANNEL_INFO*	chbuffp;
	ASYNC_ND_PARAM*	asy_prmp;
	JL100_USER_IOMAP *usr_iomapp;

	/* Set pointer of Channel Buffer and Async. communication parameter */
	chbuffp = &MLDevice.chbuff;
	asy_prmp = &MLDevice.async_nd_parm;
	usr_iomapp = MLDevice.usr_io_map;

	no = (asy_prmp->rcvDstAdr>>8)+1;
	if(asy_prmp->sw == ND_ASYNC_REQ_RCV_SEQ)
	{
		/* Request receive async frame */
		ret = jl100s_req_rcv_frame( chbuffp, 
				&asy_prmp->rcvDstAdr, &asy_prmp->rcvSrcAdr, 
				&asy_prmp->rcvStat, &asy_prmp->rcvFtype, 
				MLDevice.DmaRcvBuffer[0], &asy_prmp->rcvSize );

		if( (ret == RECEIVED_FRAME) && (asy_prmp->rcvStat == ASYNC_RCV_CMP) )
		{/* ��M�����튮�������� */
			no = (asy_prmp->rcvDstAdr>>8)+1;
			if(no <= DEF_MA_MAX)
			{
				if(asy_prmp->rcvDstAdr == usr_iomapp[no].axis_adr)
				{
					MLDevice.crcvstat[0] = 1<< no;
					asy_prmp->sw = ND_ASYNC_RCVBUF_SEQ;
				}
			}
		}
	}

	if((no == st_no) && (asy_prmp->sw == ND_ASYNC_RCVBUF_SEQ)) /* <S0EC> */
	{
		for(i = 0; i < (asy_prmp->rcvSize>>2); i++)
		{	CmdBuffer[i] = MLDevice.DmaRcvBuffer[0][i];}
		asy_prmp->sw = ND_ASYNC_SND_SEQ;
	}

	return ND_SUCCESS;
}


/********************************************************************************/
/*																				*/
/*	�֐��� :	NdSendAsyncRSP()												*/
/*																				*/
/*	�@�\�T�v:																	*/
/*		�C�x���g�h���u���ʐM���̒ʐM�f�[�^���M�i�ʐM�h���C�o�j					*/
/*																				*/
/*==============================================================================*/
/*																				*/
/*	���� :																		*/
/*		ULONG* async_rbuff		�����f�[�^�̃o�b�t�@							*/
/*	�߂�l :																	*/
/*		= ND_ASYNC_SEND_SUCCESS	����I���i���M�j								*/
/*		= ND_SENDING_FRAME		�f�[�^���M��									*/
/*		= ND_SEND_FRAME_ERR		���M�������s									*/
/*																				*/
/********************************************************************************/
static void NdSendAsyncRSP(UINT st_no, ULONG *async_rbuff )
{
	CHANNEL_INFO*	chbuffp;	/* Channel Buffer */
	ASYNC_ND_PARAM* asy_prmp;	/* �C�x���g�h���u���ʐM�p�����[�^ */
	INT32			ret;		/* return code */

	chbuffp = &MLDevice.chbuff;
	asy_prmp = &MLDevice.async_nd_parm;

	if( (asy_prmp->sw == ND_ASYNC_SND_SEQ) 
		&& ((st_no-1) == (asy_prmp->rcvDstAdr>>8)) )
	{	/* Request send async frame */
		ret = jl100s_send_frame(chbuffp, 
								asy_prmp->rcvSrcAdr, asy_prmp->rcvDstAdr, 
								DEF_ASYNC_FTYPE, 
								async_rbuff, DEF_ASYNC_DATA_SIZE );
		if( ret != SNDING_FRAME )
		{
			asy_prmp->sw = ND_ASYNC_REQ_RCV_SEQ;
//			printf("Send Async RSP(%d): 0x%04x\n",st_no, async_rbuff[0]);
		}
	}

}


/****************************************************************************/
/* 						���b�Z�[�W�p�ʐM�h���C�o							*/
/* 						�iSCF�p�̋󏈗�)									*/
/*	�T�v�F				�Ȃɂ������ɖ���ɏ������I������					*/
/****************************************************************************/

/*****************************************************************************
	���b�Z�[�W��M�J�n�v���Z�b�g
	�����F
		ch = 1	�E�E�EC1���b�Z�[�WCH		ch != 1	�E�E�EC2���b�Z�[�WCH
	�߂�l�F
		ND_RECV_BUSY	�E�E�E��M��(�v�����s���s)
		ND_SUCCESS		�E�E�E����I��
*****************************************************************************/
LONG NdReqRecvMsg( UCHAR ch )
{
	CHANNEL_INFO*	chbuffp;			/* �`�����l���o�b�t�@�̈�			*/
	USHORT			cmsg_rcv_stat;		/* C1 or C2 message receive status	*/
	USHORT			crcv_msgsz;
	USHORT			local_adr;
	LONG			lRet;

	chbuffp = &MLDevice.chbuff;
	lRet    = ND_RECV_BUSY;

	if (chbuffp->hif_reg_top == NULL)
	{
		return lRet;
	}

	if (ch == 1)
	{
		MLDevice.crcv_msgsz1 = 0;
		cmsg_rcv_stat = jl100s_req_rcv_msgdata(chbuffp, DEF_C1_MST, &local_adr, C1MST_ADR, &crcv_msgsz);
	}
	else
	{
		MLDevice.crcv_msgsz2 = 0;
		cmsg_rcv_stat = jl100s_req_rcv_msgdata(chbuffp, DEF_C2_MST, &local_adr, C2MST_ADR, &crcv_msgsz);
	}

//	cmsg_rcv_stat = jl100s_req_rcv_msgdata(chbuffp, def_mst, &local_adr, st_adrs, &crcv_msgsz);

	if ( cmsg_rcv_stat == LIB_OK )
	{
		if (crcv_msgsz > 0)
		{
			if (ch == 1)
			{
				MLDevice.crcv_msgsz1 = crcv_msgsz;
				MLDevice.c1msg_peer_multi = local_adr;
			}
			else
			{
				MLDevice.crcv_msgsz2 = crcv_msgsz;
				MLDevice.c2msg_peer_multi = local_adr;
			}
			lRet = ND_SUCCESS;
		}
	} 
	else
	{
		lRet = ND_RECVMSG_ERR;
	}

	return lRet;

}

/*****************************************************************************
	���b�Z�[�W��M�����`�F�b�N
	�����F
		ch = 1	�E�E�EC1���b�Z�[�WCH		ch != 1	�E�E�EC2���b�Z�[�WCH
		rcvWSz	�E�E�E��M���b�Z�[�W���[�h�T�C�Y
		rcvBSz	�E�E�E��M���b�Z�[�W�o�C�g�T�C�Y�i���0�j
	�߂�l�F
		ND_RECV_BUSY	�E�E�E��M��, or ��M�҂����
		ND_SUCCESS		�E�E�E��M����
		����ȊO		�E�E�E��M�G���[(�n���h�V�F�C�N�G���[��)
*****************************************************************************/
LONG NdGetRecvMsgSts( UCHAR ch, USHORT *rcvWSz, USHORT *rcvBSz )
{
	LONG		lRet;

	if (ch == 1)
	{
		if (MLDevice.crcv_msgsz1 > 0)
		{
			*rcvWSz = MLDevice.crcv_msgsz1 >> 1;	/* ���[�h�T�C�Y	*/
			*rcvBSz = MLDevice.crcv_msgsz1;		/* �o�C�g�T�C�Y	*/
			lRet = ND_SUCCESS;
		}
		else
		{
			*rcvWSz = 0;
			*rcvBSz = 0;
			lRet = ND_RECVMSG_ERR;
		}
	}
	else
	{
		if (MLDevice.crcv_msgsz2 > 0)
		{
			*rcvWSz = MLDevice.crcv_msgsz2 >> 1;	/* ���[�h�T�C�Y	*/
			*rcvBSz = MLDevice.crcv_msgsz2;		/* �o�C�g�T�C�Y	*/
			lRet = ND_SUCCESS;
		}
		else
		{
			*rcvWSz = 0;
			*rcvBSz = 0;
			lRet = ND_RECVMSG_ERR;
		}
	}
	return lRet;
}

/*****************************************************************************
	���b�Z�[�W�f�[�^���[�h
	�����F
		ch = 1	�E�E�EC1���b�Z�[�WCH		ch != 1	�E�E�EC2���b�Z�[�WCH
		rcvData	�E�E�E��M�f�[�^�i�[��
		rdSz	�E�E�E���[�h���[�h��
*****************************************************************************/
void NdReadMsgData( UCHAR ch, USHORT *rcvData, USHORT rdSz )
{
	CHANNEL_INFO*	chbuffp;
	USHORT			cmsg_rcv_stat;		/* C1 or C2 message receive status	*/
	USHORT			offset;				/*									*/
	USHORT			read_size, size;
	USHORT			def_mst;
	USHORT			nokori;
	UCHAR			*crcvData;

	chbuffp = &MLDevice.chbuff;
	offset = 0;
	crcvData = (UCHAR*)rcvData;
	def_mst = (ch == 1) ? DEF_C1_MST : DEF_C2_MST;

	read_size = 32;
	size = rdSz * 2;

	while (offset < size)
	{
		nokori = size - offset;
		if (read_size > nokori) read_size = nokori;
		cmsg_rcv_stat = jl100s_read_msgdata(chbuffp, def_mst, offset, read_size,
					 (ULONG*)(&crcvData[offset]));
		if(cmsg_rcv_stat != LIB_OK)
		{
			return;
		}
		offset += read_size;
	}
}

/*****************************************************************************
	���b�Z�[�W�f�[�^���C�g
	�����F
		ch = 1	�E�E�EC1���b�Z�[�WCH		ch != 1	�E�E�EC2���b�Z�[�WCH
		sndData	�E�E�E���M�f�[�^�i�[��
		wrSz	�E�E�E���C�g���[�h��
		wrSz	�E�E�E���C�g�o�C�g��
*****************************************************************************/
void NdWriteMsgData( UCHAR ch, USHORT *sndData, USHORT wrSz )
{
	CHANNEL_INFO*	chbuffp;			/* �`�����l���o�b�t�@�̈�			*/
	USHORT			cmsg_snd_stat;		/* C1 or C2 message receive status	*/
	USHORT			offset;				/*									*/
	USHORT			write_size;
	USHORT			def_mst;
	USHORT			nokori;
	UCHAR			*csndData;
	USHORT			rem;
	USHORT			size;
	USHORT			msg_size;
	
	chbuffp = &MLDevice.chbuff;
	csndData = (UCHAR*)sndData;
	offset = 0;
	def_mst = (ch == 1) ? DEF_C1_MST : DEF_C2_MST;

//	size = wrSz * 2;
	size = wrSz;
	rem = size % 4;
	if (rem != 0)	msg_size = size + (4-rem);
	else			msg_size = size;

	write_size = 32;

	while (offset < msg_size)
	{
		nokori = msg_size - offset;
		if (write_size > nokori) write_size = nokori;
		cmsg_snd_stat = jl100s_write_msgdata(chbuffp, def_mst, offset, write_size,
				 (ULONG*)(csndData+offset));
		if(cmsg_snd_stat != LIB_OK)
		{
			return;
		}
		offset += write_size;
	}
	MLDevice.csnd_msgsz = msg_size;
	//�O���[�o���ϐ��ɂ��̂܂ܒl���i�[���悤�Ƃ����
	//0�ƂȂ��Ă��܂����߁Astatiac�ϐ����o�R���Ċi�[����B
	//�R���p�C���̍œK���̉e���H�H
}

/*****************************************************************************
	���b�Z�[�W���M�J�n�v���Z�b�g
	�����F
		ch = 1	�E�E�EC1���b�Z�[�WCH		ch != 1	�E�E�EC2���b�Z�[�WCH
	�߂�l�F
		ND_SEND_BUSY	�E�E�E���M��(�v�����s���s)
		ND_SUCCESS		�E�E�E����I��
		����ȊO		�E�E�E���M�G���[
*****************************************************************************/
LONG NdReqSendMsg( UCHAR ch )
{
	CHANNEL_INFO*	chbuffp;			/* �`�����l���o�b�t�@�̈�			*/
	USHORT			cmsg_snd_stat;		/* C1 or C2 message send status		*/
	USHORT			st_adrs;			/* Satation address					*/
	USHORT			*st_adr_da;			/* Local node station address		*/
	LONG			lRet;
	USHORT			def_mst;

	chbuffp = &MLDevice.chbuff;

	if (ch == 1)
	{
		def_mst   = DEF_C1_MST;
		st_adrs   = C1MST_ADR;
		st_adr_da = &MLDevice.c1msg_peer_multi;

	}
	else
	{
		def_mst   = DEF_C2_MST;
		st_adrs   = C2MST_ADR;
		st_adr_da = &MLDevice.c2msg_peer_multi;

	}

	cmsg_snd_stat = jl100s_req_snd_msgdata(chbuffp, def_mst, 
								st_adrs, *st_adr_da, MLDevice.csnd_msgsz);
	if ( cmsg_snd_stat == LIB_OK )
	{
		lRet = ND_SUCCESS;
	}
	else if (cmsg_snd_stat == SNDING_MSG)
	{
		lRet = ND_SUCCESS;
	}
	else
	{
		lRet = ND_SENDMSG_ERR;
	}
	return lRet;
}

/*****************************************************************************
	���b�Z�[�W���M�����`�F�b�N
	�����F
		ch = 1	�E�E�EC1���b�Z�[�WCH		ch != 1	�E�E�EC2���b�Z�[�WCH
	�߂�l�F
		ND_SEND_BUSY	�E�E�E���M��
		ND_SUCCESS		�E�E�E���M����
		����ȊO		�E�E�E���M�G���[(�n���h�V�F�C�N�G���[��)
*****************************************************************************/
LONG NdGetSendMsgSts( UCHAR ch )
{
	CHANNEL_INFO*	chbuffp;			/* �`�����l���o�b�t�@�̈�			*/
	USHORT			cmsg_snd_stat;		/* C1 or C2 message send status		*/
	USHORT			st_adrs;			/* Satation address					*/
	USHORT			*st_adr_da;			/* Local node station address		*/
	LONG			lRet;
	USHORT			def_mst;

	chbuffp = &MLDevice.chbuff;

	if (ch == 1)
	{
		def_mst   = DEF_C1_MST;
		st_adrs   = C1MST_ADR;
		st_adr_da = &MLDevice.c1msg_peer_multi;
	}
	else
	{
		def_mst   = DEF_C2_MST;
		st_adrs   = C2MST_ADR;
		st_adr_da = &MLDevice.c2msg_peer_multi;
	}

	cmsg_snd_stat = jl100s_req_snd_msgdata(chbuffp, def_mst, 
							st_adrs, *st_adr_da, MLDevice.csnd_msgsz);
	if ( cmsg_snd_stat == LIB_OK )
	{
		lRet = ND_SUCCESS;
	}
	else if (cmsg_snd_stat == SNDING_MSG)
	{
		lRet = ND_SEND_BUSY;
	}
	else
	{
		lRet = ND_SENDMSG_ERR;
	}
	return lRet;
}

/*****************************************************************************
	�A�{�[�g���b�Z�[�W���M
	�����F
		ch = 1	�E�E�EC1���b�Z�[�WCH		ch != 1	�E�E�EC2���b�Z�[�WCH
	�߂�l�F
*****************************************************************************/
void NdSendMsgAbort(UCHAR ch)
{
	CHANNEL_INFO*	chbuffp;
	USHORT			def_mst;

	chbuffp = &MLDevice.chbuff;

	if (ch == 1)
	{
		def_mst = DEF_C1_MST;
	}
	else
	{
		def_mst = DEF_C2_MST;
	}

	(void)jl100s_abort_msgdata(chbuffp, def_mst);

	return;
}

/*****************************************************************************
	�ʐM�p�����[�^�擾
	�����F
	�߂�l�F*ND_PRAM	//kira_for_MEMOBUS_Message
*****************************************************************************/
ND_PARAM* NdGetNdParamPtr( VOID )
{
	return (&MLDevice.nd_prm);

}

/*****************************************************************************
	�ʐMASIC���Z�b�g <S14E>
	�����FDeviceAddress �ʐMASIC�擪�A�h���X
	�߂�l�F
*****************************************************************************/
void NdResetAsic( void *DeviceAddress )
{
	CHANNEL_INFO *chbuffp;		/* JL-100�A�N�Z�X��`�|�C���^ */
	chbuffp		= &MLDevice.chbuff;
	chbuffp->hif_reg_top = (JL100_HOST_IF_REGS *)DeviceAddress;
	jl100s_reset_asic( chbuffp );

	return;
}


/************** end of file **********************/

