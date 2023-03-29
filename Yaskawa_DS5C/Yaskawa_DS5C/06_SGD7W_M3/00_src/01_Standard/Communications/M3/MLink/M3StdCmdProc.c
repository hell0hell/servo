/****************************************************************************
Description	: MECHATROLINK Command Analyzation (Standard Servo Profile)
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : Mercury

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------

****************************************************************************/
#include "Basedef.h"
#include "M3Handler.h"
#include "ComAlmMngr.h"
#include "ComManager.h"
#include "SysApi.h"
#include "MtApi.h"
#include "CmdAnalyzer.h"
#include "MLib.h"
//#include "M3StdCmdProc.h"

/****************************************************************************
	Defines for Command function
****************************************************************************/
/* ID_RD */
#define ID_RD_MAX_SIZE			24				/* �ő�ǂݏo���T�C�Y	*/
#define	EVT_ID_RD_MAX_SIZE		8				/* �ő�ǂݏo���T�C�Y(�C�x���g�h���u���ʐM) */

/* ALM_RD */
#define ALM_RD_MOD0				0				/* ���ݔ������ُ̈�E�x���̏�� */
#define ALM_RD_MOD1				1				/* �ُ픭���󋵗��� */
#define ALM_RD_MOD2				2				/* ���ݔ������ُ̈�E�x���̏ڍ׏�� */
#define ALM_RD_MOD3				3				/* �ُ픭���󋵗����̏ڍ׏�� */
#define ALM_RD_NUM				12				/* �A���[���ǂݏo����(���C���R�}���h) */
#define EX_ALM_RD_NUM			4				/* �A���[���ǂݏo����(�T�u�R�}���h) */

/* CONFIG */
#define CONFIG_MOD0				0				/* �p�����[�^�Čv�Z */
#define CONFIG_MOD1				1				/* EEPROM�ւ̈ꊇ�������� */
#define CONFIG_MOD2				2				/* �p�����[�^������ */

/* ALM_CLR */
#define ALM_CLR_MOD0			0				/* ���ݔ������ُ̈�E�x���̏�ԃN���A */
#define ALM_CLR_MOD1			1				/* �ُ픭���󋵗����N���A */

/* MEM_RD */
#define MEM_RD_MAX_SIZE			20				/* �ő�ǂݏo���T�C�Y(���C���R�}���h) */
#define EX_MEM_RD_MAX_SIZE		4				/* �ő�ǂݏo���T�C�Y(�T�u�R�}���h)	 */
#define EVT_MEM_RD_MAX_SIZE		4				/* �ő�ǂݏo���T�C�Y(�C�x���g�h���u���ʐM)	*/

/* MEM_WR */
#define MEM_WR_MAX_SIZE			20				/* �ő发�����݃T�C�Y(���C���R�}���h) */
#define EX_MEM_WR_MAX_SIZE		4				/* �ő发�����݃T�C�Y(�T�u�R�}���h)	 */

/* PRM_RD/PRM_WR/PPRM_WR */
#define RAM_OFFSET 				0x1000			/* RAM�������ݗp�I�t�Z�b�g */
#define EEPROM_OFFSET			0				/* EEPROM�������ݗp�I�t�Z�b�g */
#define COMM_PRM_OFFSET			0xA00			/* ���ʃp�����[�^�p�I�t�Z�b�g */

/* INTERPOLATE�`TRQCTRL */
#define CMD_MAX_TLIM			MT_MAXTRQ_30	/* TLIM�ő�l */
#define CMD_MAX_VLIM			MT_OSVEL_30		/* VLIM�ő�l */
#define CMD_MAX_ACCR			MT_MAX_ACCR		/* ACCR,DECR[ref/cyc^2] �ő�l */
#define CMD_MAX_ACCR_VEL		MT_MAX_ACCR_VEL	/* ACCR,DECR[ref/cyc^2] �ő�l(VELCTRL�R�}���h��) */

/* ���z������ */
#define VMEM_ID_MAX				0x0000FFFF		/* ID�̈�				�I���A�h���X	*/
#define VMEM_COM_MIN			0x00010000		/* ���ʃp�����[�^		�J�n�A�h���X	*/
#define VMEM_COM_MAX			0x0001FFFF		/* ���ʃp�����[�^		�I���A�h���X	*/
#define	VMEM_VEN_MIN			0x80000000		/* �x���_�[�X�y�V�t�B�b�N�̈� �J�n�A�h���X	*/
#define	VMEM_VEN_MAX			0x8001FFFF		/* �x���_�[�X�y�V�t�B�b�N�̈� �I���A�h���X	*/

#define	ONLY_C_PHASE_LATCH		0x00			/* LT_SGNL C�����b�`�̂� */

/* SPOSING S���������䗦��` */
#define S_RATIO1	50			/* S�����������Ԕ䗦1(%) */
#define S_RATIO2	25			/* S�����������Ԕ䗦2(%) */

/* POSING���[�h�I�� */
#define POSING_SEL  0x0000
#define SPOSING_SEL 0x0001

/************ �r�b�g�w�� ************/
/* CMD_CTRL */
#define CMD_CTRL_ALM_CLR_MAP	3
#define CMD_CTRL_CMD_ID_MAP		6

#define CMD_CTRL_ALM_CLR_MASK	0x1
#define CMD_CTRL_CMD_ID_MASK	0x3

/* SVCMD_CTRL */
#define SVCMD_CTRL_CMD_PAUSE_MAP	0
#define SVCMD_CTRL_CMD_CANCEL_MAP	1
#define SVCMD_CTRL_STOP_MODE_MAP	2
#define SVCMD_CTRL_ACCFIL_MAP		4
#define SVCMD_CTRL_LT_REQ1_MAP		8
#define SVCMD_CTRL_LT_REQ2_MAP		9
#define SVCMD_CTRL_LT_SEL1_MAP		10
#define SVCMD_CTRL_LT_SEL2_MAP		12
#define SVCMD_CTRL_SEL_MON1_MAP		16
#define SVCMD_CTRL_SEL_MON2_MAP		20
#define SVCMD_CTRL_SEL_MON3_MAP		24

#define SVCMD_CTRL_CMD_PAUSE_MASK	0x1
#define SVCMD_CTRL_CMD_CANCEL_MASK	0x1
#define SVCMD_CTRL_STOP_MODE_MASK	0x3
#define SVCMD_CTRL_ACCFIL_MASK		0x3
#define SVCMD_CTRL_LT_REQ1_MASK		0x1
#define SVCMD_CTRL_LT_REQ2_MASK		0x1
#define SVCMD_CTRL_LT_SEL1_MASK		0x3
#define SVCMD_CTRL_LT_SEL2_MASK		0x3
#define SVCMD_CTRL_SEL_MON1_MASK	0xF
#define SVCMD_CTRL_SEL_MON2_MASK	0xF
#define SVCMD_CTRL_SEL_MON3_MASK	0xF

#define SVCMD_CTRL_ACK_MASK			0x0FFF0033


/* SVCMD_IO */
#define SVCMD_IO_V_CLMP_MAP			3
#define SVCMD_IO_V_PPI_MAP			4
#define SVCMD_IO_P_PPI_MAP			5
#define SVCMD_IO_P_CL_MAP			6
#define SVCMD_IO_N_CL_MAP			7
#define SVCMD_IO_G_SEL_MAP			8
//#define SVCMD_IO_BANK_SEL_MAP		16	/* <S1C0> :Delete */
//#define SVCMD_IO_NET_TRG_MAP		25/* <S012> */	/* <S0CB> Delete */
#define SVCMD_IO_NET_TRG_MAP		31/* <S0CB> */
//#define SVCMD_IO_VENDER_IO2_MAP	20	/* <S1C0> :Delete */
//#define SVCMD_IO_VENDER_IO3_MAP	24	/* <S1C0> :Delete */
//#define SVCMD_IO_TFF_FIL_INV_MAP	26/* <S1D0> */
//#define SVCMD_IO_VFF_FIL_INV_MAP	27/* <S1D0> */
//#define SVCMD_IO_VENDER_IO4_MAP	28	/* <S1C0> :Delete */

#define SVCMD_IO_V_CLMP_MASK		0x1
#define SVCMD_IO_V_PPI_MASK			0x1
#define SVCMD_IO_P_PPI_MASK			0x1
#define SVCMD_IO_P_CL_MASK			0x1
#define SVCMD_IO_N_CL_MASK			0x1
#define SVCMD_IO_G_SEL_MASK			0xF
#define SVCMD_IO_NET_TRG_MASK		0x1/* <S012> */

#define SVCMD_IO_BANK_SEL_MASK		0xF			/* BANK_SEL */
#define SVCMD_IO_SO1_MASK			0x1			/* IO�o�͐ݒ�@�\ */
#define SVCMD_IO_SO2_MASK			0x2			/* IO�o�͐ݒ�@�\ */
#define SVCMD_IO_SO3_MASK			0x4			/* IO�o�͐ݒ�@�\ */
#define SVCMD_IO_VENDER_IO3_MASK	0xF
#define SVCMD_IO_VENDER_IO4_MASK	0xF


/* SUB_CTRL */
//#define SUB_CTRL_SUB_CMD_ID_MAP	6
#define SUB_CTRL_SELMON4_MAP		12
#define SUB_CTRL_SELMON5_MAP		16
#define SUB_CTRL_SELMON6_MAP		20

//#define SUB_CTRL_SUB_CMD_ID_MASK	0x3
#define SUB_CTRL_SELMON4_MASK		0xF
#define SUB_CTRL_SELMON5_MASK		0xF
#define SUB_CTRL_SELMON6_MASK		0xF

/*************************************/

/* SVCMD_CTRL�����̖߂�l */
#define CMD_FUNC_PAUSE_REQ			1
#define CMD_FUNC_CANCEL_REQ			2

/* ���[�V�������b�`�R�}���h�R�[�h */
#define CMD_CODE_EX_FEED			0x37
#define CMD_CODE_EX_POSING			0x39
#define CMD_CODE_ZRET				0x3A

/* ZRET MODE */
#define ZRET_MODE_TYPE_MAP			0
#define ZRET_MODE_TYPE_MASK			0xF
#define ZRET_MODE_HOME_DIR_MAP		7
#define ZRET_MODE_HOME_DIR_MASK		1

#define ZRET_MODE_TYPE_LATCH_MODE			0
#define ZRET_MODE_TYPE_LATCH_AND_DEC_MODE	1


/* Local functions */
static VOID stdCmdCmdCtrlProc( CMD_HNDL *pCmdHdl, ALM_HNDL *pAlmHdl, MT_HNDL *pMtHdl );
static VOID stdCmdSvCmdCtrlProc( CMD_HNDL *pCmdHdl, MT_HNDL *pMtHdl, BOOL enable );
static VOID stdCmdSvCmdIoProc( CMD_HNDL *pCmdHdl, MT_HNDL *pMtHdl );
static VOID stdCmdSubCtrlProc( CMD_HNDL *hdl );
static INT32 stdCmdAlmRdProc( ALM_HNDL *alm_hdl, USHORT alm_rd_mod, USHORT idx, USHORT *alm_data, LONG read_num );
static INT32 stdCmdChkConnectCmd( MLNET_HANDLE *hNetData, LONG *reqphs, 
								  LONG *dtmod, LONG *com_tim, BOOL AsyncMode );
//static INT32 cmdMulgain32WithClampData( ULONG inData, LONG maxData, KSGAIN kx, LONG* outData );
//static INT32 cmdMulgain32WithClampAccTime( ULONG inData, LONG maxData, KSGAIN kx, LONG* outData )




/****************************************************************************/
/*								�r�b�g�w�ߏ���								*/
/****************************************************************************/
/****************************************************************************/
/* �T�v		:	�R�}���h����(CMD_CTRL)����									*/
/****************************************************************************/
/* ����		:	CMD_HNDL *hdl												*/
/*			:	ALM_HNDL *pAlmHdl											*/
/* �߂�l	:	�Ȃ�														*/
/****************************************************************************/
static	VOID stdCmdCmdCtrlProc( CMD_HNDL *pCmdHdl, ALM_HNDL *pAlmHdl, MT_HNDL *pMtHdl )
{
	LONG	cmd_ctrl;
	LONG	almClr, lstAlmClr;
	INT32	ret;

	cmd_ctrl = pCmdHdl->cmdInfo.cmdBuf.stdData.nop.cmd_ctrl;

	/* ALM_CLR *//* �G�b�W�ŃA���[���N���A */
	almClr = ( cmd_ctrl >> CMD_CTRL_ALM_CLR_MAP ) & CMD_CTRL_ALM_CLR_MASK;
	if( almClr )
	{
		lstAlmClr = ( pCmdHdl->cmdInfo.lstCmdCtrl >> CMD_CTRL_ALM_CLR_MAP ) & CMD_CTRL_ALM_CLR_MASK;
		ret = AlmSetClearReq( pAlmHdl, (BOOL)(almClr != lstAlmClr) );
		
		/* ALM_CLR�X�e�[�^�X�쐬 */
		pCmdHdl->cmdInfo.almClrCmp = (BOOL)( (ret==ALM_SUCCESS) && almClr );
	}
	else
	{
		/* ALM_CLR�X�e�[�^�X�쐬 */
		pCmdHdl->cmdInfo.almClrCmp = FALSE;
	}
	/* CMD_CTRL�̕ۑ� */
	pCmdHdl->cmdInfo.lstCmdCtrl = cmd_ctrl;
	
	/* CMD_ID ����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->cmdInfo.rsltBuf.stdData.mt.cmd_ctrl
		 = (USHORT)( cmd_ctrl & (CMD_CTRL_CMD_ID_MASK << CMD_CTRL_CMD_ID_MAP) );
}


/****************************************************************************/
/* �T�v		:	�T�[�{�R�}���h����t�B�[���h(SVCMD_CTRL)����				*/
/****************************************************************************/
/* ����		:	CMD_HNDL *hdl												*/
/*			:	MT_HNDL *pMtHdl												*/
/* 			:	BOOL enable		TRUE/FALSE									*/
/*								CMD_CANCEL,CMD_PAUSE �����s����  = TRUE		*/
/*								CMD_CANCEL,CMD_PAUSE �����s���Ȃ�= FALSE	*/
/* �߂�l	:	�Ȃ�														*/
/****************************************************************************/
static VOID stdCmdSvCmdCtrlProc( CMD_HNDL *pCmdHdl, MT_HNDL *pMtHdl, BOOL enable )
{
	MCI_CMD		cmd;
	ULONG		svcmd_ctrl;
	ULONG		lstsvcmd_ctrl;										/* svcmd_ctrl �O��l */
	ULONG		cmdPause, cmdCancel;								/* CMD_PAUSE, CMD_CANCEL�w�� */
	LONG		ltSel1, ltSel2;										/* ���b�`�I�� */
	LONG		ltReq1, ltReq2;										/* ���b�`�v�� */
	UCHAR		cmdCode;											/* �R�}���h�R�[�h */

	/* ���[�J���ϐ��̏����� */
	svcmd_ctrl		= pCmdHdl->cmdInfo.cmdBuf.stdData.mt.svcmd_ctrl;
	lstsvcmd_ctrl	= pCmdHdl->cmdInfo.lstSvCmdCtrl;
	cmdCode			= pCmdHdl->cmdInfo.cmdBuf.stdData.nop.cmd;


	/*-----------------------------------------------------------------*/
	/* 					CMD_CANCEL, CMD_PAUSE			 			   */
	/*-----------------------------------------------------------------*/
	/* POSING,FEED,EX_FEED,EX_POSING,ZRET,VELCTRL �̂Ƃ��̂ݏ������s�� */
	/*-----------------------------------------------------------------*/
	if(enable)
	{ /* CMD_CANCEL,CMD_PAUSE ���L���ȃR�}���h�̏ꍇ�̏��� */
		/* CMD_PAUSE, CMD_CANCEL �̃r�b�g�w�ߎ擾 */
		cmdPause  = ( svcmd_ctrl & (1<<SVCMD_CTRL_CMD_PAUSE_MAP) );
		cmdCancel = ( svcmd_ctrl & (1<<SVCMD_CTRL_CMD_CANCEL_MAP) );

		/*--------------------------------------*/
		/*				CMD_CANCEL				*/
		/*--------------------------------------*/
		if( cmdCancel )
		{ /* CMD_CANCEL �I�� */
			/* ���[�V�����w�߃f�[�^��ݒ� */
			cmd.HoldMod = (svcmd_ctrl >> SVCMD_CTRL_STOP_MODE_MAP) & SVCMD_CTRL_STOP_MODE_MASK;

			/* CMD_CANCEL API�֐����s */
			MciSetCmdCancel( pMtHdl, &cmd );
			pCmdHdl->cmdInfo.cmdCancelEnable = TRUE;
		}
		else
		{
			/* CMD_CANCEL = FALSE �̏ꍇ �t���O���N���A���� */
			pCmdHdl->cmdInfo.cmdCancelEnable = FALSE;

			/*--------------------------------------*/
			/*				CMD_PAUSE				*/
			/*--------------------------------------*/
			if( cmdPause )
			{ /* CMD_PAUSE �I�� */
				/* ���[�V�����w�߃f�[�^��ݒ� */
				cmd.HoldMod = (svcmd_ctrl >> SVCMD_CTRL_STOP_MODE_MAP) & SVCMD_CTRL_STOP_MODE_MASK;

				/* ���_���A�A�v���[�`���x���[���ɐݒ� */
				MciClearZretSpd(pMtHdl);

				/* CMD_PAUSE API�֐����s */
				MciSetCmdPause(pMtHdl, &cmd);
				pCmdHdl->cmdInfo.cmdPauseEnable = TRUE;
			}
			else
			{
				/* ���_���A�A�v���[�`���x�𕜋A */
				MciReturnZretSpd(pMtHdl);

				/* CMD_PAUSE = FALSE �̏ꍇ �t���O���N���A���� */
				pCmdHdl->cmdInfo.cmdPauseEnable = FALSE;
			}
		}
	}
	else
	{/* CMD_PAUSE, CMD_CANCEL ���g�p�̏ꍇ�͗L���t���O���N���A���� */
		pCmdHdl->cmdInfo.cmdPauseEnable = FALSE;
		pCmdHdl->cmdInfo.cmdCancelEnable = FALSE;
	}

	/*-------------------*/
	/* 		ACCFIL		 */
	/*-------------------*/
	pMtHdl->cmdData.AccFilSel = (svcmd_ctrl >> SVCMD_CTRL_ACCFIL_MAP) & SVCMD_CTRL_ACCFIL_MASK;

	/*--------------------------------------------*/
	/* LT_REQ1,LT_SEL1 */  /* �G�b�W�Ń��b�`1�v�� */
	/*--------------------------------------------*/
	ltSel1 = (svcmd_ctrl >> SVCMD_CTRL_LT_SEL1_MAP) & SVCMD_CTRL_LT_SEL1_MASK;
	ltReq1 = (svcmd_ctrl & (1 << SVCMD_CTRL_LT_REQ1_MAP)) ;

	if( (cmdCode != CMD_CODE_EX_FEED) && (cmdCode != CMD_CODE_EX_POSING) && (cmdCode != CMD_CODE_ZRET) )
	{/* ���b�`�R�}���h�ȊO�̃R�}���h�̏ꍇ�Ƀ��[�_�����b�`�v�����s�� */
	 /* ���b�`�R�}���h�̓��[�V�����`�o�h�Ń��b�`�v�����s�� */
		if( 0 != ltReq1 )
		{
			if( (0 == (lstsvcmd_ctrl & (1 << SVCMD_CTRL_LT_REQ1_MAP)) )
				|| (pCmdHdl->cmdInfo.reLatchReq != FALSE) )
			{
				MciRequestMdlLatch( pMtHdl, 0, ltSel1, 0);
				pCmdHdl->cmdInfo.reLatchReq = FALSE;
			}
		}
	}
	else
	{/* ���b�`�R�}���h�̏ꍇ�́A���[�V����API�Ń��b�`�v�����s���̂�"FALSE"�Ƃ��� */
		pCmdHdl->cmdInfo.reLatchReq = FALSE;
	}
	if( 0 == ltReq1 )
	{/* ���b�`�v���Ȃ��̏ꍇ�́A���b�`�v���N���A */
		MciCancelLatch( pMtHdl, MOTION_LTMD, 0 );
		MciCancelLatch( pMtHdl, MODAL_LTMD, 0 );
		pCmdHdl->cmdInfo.reLatchReq = FALSE;
	}

	/*--------------------------------------------*/
	/* LT_REQ2,LT_SEL2  */ /* �G�b�W�Ń��b�`2�v�� */
	/*--------------------------------------------*/
	ltSel2 = (svcmd_ctrl >> SVCMD_CTRL_LT_SEL2_MAP) & SVCMD_CTRL_LT_SEL2_MASK;
	ltReq2 = (svcmd_ctrl & (1 << SVCMD_CTRL_LT_REQ2_MAP)) ;

	if( 0 != ltReq2 )
	{/* ���[�_�����b�`�v�����s�� �i���b�`�R�}���h��LT_REQ2�𗘗p���Ȃ��j */
		if( 0 == (lstsvcmd_ctrl & (1 << SVCMD_CTRL_LT_REQ2_MAP)))
		{
			if( pMtHdl->mtPrm.LtPrm.seqNum == 0 )
			{/* �ʏ탉�b�` */	
				MciRequestMdlLatch( pMtHdl, 0, ltSel2, 1 );
			}
			else
			{/* �A�����b�`�L�� */
				MciRequestMdlLatch( pMtHdl, 1, ltSel2, 1 );
			}
		}
	}
	else 
	{/* ���b�`�v���Ȃ��̏ꍇ�́A���b�`�v���N���A */
		MciCancelLatch( pMtHdl, MOTION_LTMD, 1 );
		MciCancelLatch( pMtHdl, MODAL_LTMD, 1 );
	}

	/* ���b�`�M���I�����̕ۑ��i���ʃ��j�^�p�j */
	pCmdHdl->cmdInfo.ltSelR[0] = ltSel1;
	pCmdHdl->cmdInfo.ltSelR[1] = ltSel2;

	/* SVCMD_CTRL���̕ۑ� */
	pCmdHdl->cmdInfo.lstSvCmdCtrl = svcmd_ctrl;

	/* ACCFIL, SEL_MON1�`3 ����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->cmdInfo.rsltBuf.stdData.mt.svcmd_ctrl = svcmd_ctrl & SVCMD_CTRL_ACK_MASK;
}


/****************************************************************************/
/* �T�v		:	�T�[�{�R�}���h���o�͐M��(SVCMD_IO)����						*/
/****************************************************************************/
/* ����		:	CMD_HNDL *hdl												*/
/*			:	MT_HNDL *pMtHdl												*/
/*			:	REG_HNDL *pRegHdl											*/
/* �߂�l	:	�Ȃ�														*/
/****************************************************************************/
static VOID stdCmdSvCmdIoProc( CMD_HNDL *pCmdHdl, MT_HNDL *pMtHdl )
{
	ULONG	svcmd_io, CmdSeq;
	ULONG	bitVal;	
	USHORT	CmdCtrl;

	svcmd_io = pCmdHdl->cmdInfo.cmdBuf.stdData.mt.svcmd_io;

	///* V_CLMP */	/* ���T�|�[�g */
	//if( pMtHdl->cmdData.CtrlMode == MT_TRQ_CTRLMD )
	//{
	//	bitVal = (UCHAR)( (svcmd_io >> SVCMD_IO_V_CLMP_MAP) & SVCMD_IO_V_CLMP_MASK );
	//	MciSetEnbVelClmpSignal( pMtHdl, bitVal );
	//}

	/* Reset CmdCtrlBit Data *//* <S021> */
	CmdCtrl = pMtHdl->svCmd->CmdCtrl.CmdCtrlBit
			  & (~(ENBPCTRL_BIT | CLRPOSINTG_BIT | TRACE_TRG_BIT));


	/* V_PPI */
//	CmdCtrl = pMtHdl->svCmd->CmdCtrl.CmdCtrlBit & (~(ENBPCTRL_BIT | CLRPOSINTG_BIT));	/* <S013> */
	bitVal = ( svcmd_io >> SVCMD_IO_V_PPI_MAP ) & 1;
	CmdCtrl |= (USHORT)(bitVal << ENBPCTRL_BITNO);

	/* P_PPI */
	bitVal = ( svcmd_io >> SVCMD_IO_P_PPI_MAP ) & 1;
	CmdCtrl |= (USHORT)(bitVal << CLRPOSINTG_BITNO);

#if CSW_NET_TRG_USE == TRUE		/* <S012> */
	/* NET_TRG *//* <S012> */
	bitVal = ( svcmd_io >> SVCMD_IO_NET_TRG_MAP ) & SVCMD_IO_NET_TRG_MASK;
	CmdCtrl |= (USHORT)(bitVal << TRACE_TRIGGER_BITNO);
#endif		/* <S012> */

	pMtHdl->svCmd->CmdCtrl.CmdCtrlBit = CmdCtrl;

	/* P_CL/N_CL */
	CmdSeq = pMtHdl->svCmd->CmdSeqBit & (~(ENB_EXTPTRQLMT_BIT | ENB_EXTNTRQLMT_BIT));
	bitVal = ( svcmd_io >> SVCMD_IO_P_CL_MAP ) & 3;
	CmdSeq |= (ULONG)(bitVal << ENB_EXTPTRQLMT_BITNO);

	pMtHdl->svCmd->CmdSeqBit = CmdSeq;

	/* G_SEL */
	bitVal = ( svcmd_io >> SVCMD_IO_G_SEL_MAP ) & SVCMD_IO_G_SEL_MASK;
	pMtHdl->svCmd->CmdCtrl.SelBank = (UCHAR)bitVal;

#if 0/* VendorSpecificsI/O�C�ӊ���t���Ή�	*//* <S1C0>:Start */
	/* BANK_SEL 16bit��*//* <S080> */
	bitVal = ( svcmd_io >> SVCMD_IO_BANK_SEL_MAP ) & SVCMD_IO_BANK_SEL_MASK;
	if( bitVal < pMtHdl->mtPrm.Bnk.bankNum )
	{
		pMtHdl->mtPrm.Bnk.cmdSel = bitVal;
	}

	///* SO1, SO2, SO3 *//* <S14D>:�ȉ��A�R�����g�A�E�g������� */
	if( pMtHdl->mtPrm.ProfPrm.OutSigEnable != FALSE)
	{ /* Pn50E,50F,510=0�̂Ƃ��̂ݎ��s */
		bitVal  = ( svcmd_io >> SVCMD_IO_VENDER_IO2_MAP ) & SVCMD_IO_SO1_MASK;
		bitVal |= ( svcmd_io >> SVCMD_IO_VENDER_IO2_MAP ) & SVCMD_IO_SO2_MASK;
		bitVal |= ( svcmd_io >> SVCMD_IO_VENDER_IO2_MAP ) & SVCMD_IO_SO3_MASK;
		pMtHdl->svCmd->DOSignal = bitVal;
	}
#else
	/* BANK_SEL 16bit��*//* <S080> */
	bitVal = ( svcmd_io >> pMtHdl->mtPrm.SvCmdIoOutMap[VENDOR_IOBIT_BANK].bitNo )
									 & pMtHdl->mtPrm.SvCmdIoOutMap[VENDOR_IOBIT_BANK].bitsMsk;
	if( bitVal < pMtHdl->mtPrm.Bnk.bankNum )
	{
		pMtHdl->mtPrm.Bnk.cmdSel = bitVal;
	}

	///* SO1, SO2, SO3 *//* <S14D>:�ȉ��A�R�����g�A�E�g������� */
	if( pMtHdl->mtPrm.ProfPrm.OutSigEnable != FALSE)
	{ /* Pn50E,50F,510=0�̂Ƃ��̂ݎ��s */
		bitVal  = ( svcmd_io >> pMtHdl->mtPrm.SvCmdIoOutMap[VENDOR_IOBIT_SO1].bitNo )
								 & pMtHdl->mtPrm.SvCmdIoOutMap[VENDOR_IOBIT_SO1].bitsMsk;
		bitVal |= ((( svcmd_io >> pMtHdl->mtPrm.SvCmdIoOutMap[VENDOR_IOBIT_SO2].bitNo )
								 & pMtHdl->mtPrm.SvCmdIoOutMap[VENDOR_IOBIT_SO2].bitsMsk) << 1);
		bitVal |= ((( svcmd_io >> pMtHdl->mtPrm.SvCmdIoOutMap[VENDOR_IOBIT_SO3].bitNo )
								 & pMtHdl->mtPrm.SvCmdIoOutMap[VENDOR_IOBIT_SO3].bitsMsk) << 2);
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W�i2���Ή��j�̂�	*/
		bitVal |= ((( svcmd_io >> pMtHdl->mtPrm.SvCmdIoOutMap[VENDOR_IOBIT_SO4].bitNo )
								 & pMtHdl->mtPrm.SvCmdIoOutMap[VENDOR_IOBIT_SO4].bitsMsk) << 3);
		bitVal |= ((( svcmd_io >> pMtHdl->mtPrm.SvCmdIoOutMap[VENDOR_IOBIT_SO5].bitNo )
								 & pMtHdl->mtPrm.SvCmdIoOutMap[VENDOR_IOBIT_SO5].bitsMsk) << 4);
#endif/*	SGD7W�i2���Ή��j�̂�	*/

		pMtHdl->svCmd->DOSignal = bitVal;
	}
#endif/* VendorSpecificsI/O�C�ӊ���t���Ή�	*//* <S1C0>:End */
/* <S1D0> Start */
/* TFF_FIL_INV */
//	bitVal = (BOOL)(svcmd_io >> SVCMD_IO_TFF_FIL_INV_MAP) & SVCMD_IO_TFF_FIL_INV_MASK;		/* <VFF_TFF_MAFIL> */
//	pMtHdl->svCmd->CmdCtrl.tffFilSts = bitVal;

/* VFF_FIL_INV */
//	bitVal = (BOOL)(svcmd_io >> SVCMD_IO_VFF_FIL_INV_MAP) & SVCMD_IO_VFF_FIL_INV_MASK;		/* <VFF_TFF_MAFIL> */
//	pMtHdl->svCmd->CmdCtrl.vffFilSts = bitVal;
/* <S1D0> End */
}


/****************************************************************************/
/* �T�v		:	�T�u�R�}���h����(SUB_CTRL)����								*/
/****************************************************************************/
/* ����		:	CMD_HNDL *hdl												*/
/* �߂�l	:	�Ȃ�														*/
/****************************************************************************/
static VOID stdCmdSubCtrlProc( CMD_HNDL *hdl )
{
	/*----------------------------------------*/
	/*  SELMON4�`6 ����͌��ʃo�b�t�@�փR�s�[ */
	/*----------------------------------------*/
	/* SELMON4 */
	hdl->exCmdInfo.rsltBuf.stdData.mon.sub_ctrl1 = 
			(UCHAR)( hdl->exCmdInfo.cmdBuf.stdData.mon.sub_ctrl1 & (SUB_CTRL_SELMON4_MASK << 4));

	/* SELMON5, SELMON6 */
	hdl->exCmdInfo.rsltBuf.stdData.mon.sub_ctrl2 = 
				hdl->exCmdInfo.cmdBuf.stdData.mon.sub_ctrl2;
}


/****************************************************************************/
/*								���C���R�}���h								*/
/****************************************************************************/
/****************************************************************************/
/* �T�v		:	����`�R�}���h��͏���										*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_WARNG6			�R�}���h�x���U						*/
/****************************************************************************/
INT32 stdCmd_NO_DEFINE( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;

	pCmdHdl = &hNetData->cmd_hdl;

	/* �ُ�I����Ԃ� */
	pCmdHdl->cmdInfo.rsltBuf.stdData = pCmdHdl->cmdInfo.cmdBuf.stdData;
	return CMD_WARNG6;
}


/****************************************************************************/
/* �T�v		:	���T�|�[�g�R�}���h��͏���									*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_WARNG2			�R�}���h�x���Q						*/
/****************************************************************************/
INT32 stdCmd_NO_SUPP( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;

	pCmdHdl = &hNetData->cmd_hdl;

	/* �ُ�I����Ԃ� */
	pCmdHdl->cmdInfo.rsltBuf.stdData = pCmdHdl->cmdInfo.cmdBuf.stdData;
	return CMD_WARNG2;
}


/****************************************************************************/
/* �T�v		:	�m�n�o�R�}���h��͏���										*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END		����I��						*/
/****************************************************************************/
INT32 stdCmd_NOP( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;

	pCmdHdl = &hNetData->cmd_hdl;

	/* �R�}���h����(CMD_CTRL)��͏��� */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, &hNetData->mt_hdl );
	/* ��͌��ʂ��o�b�t�@�փR�s�[ */
	pCmdHdl->cmdInfo.rsltBuf.stdData.nop.cmd = pCmdHdl->cmdInfo.cmdBuf.stdData.nop.cmd;
	/* �R�}���h��͎��s���ʐݒ� */
	pCmdHdl->cmdInfo.ResultCmdAna = CMD_FUNC_NORMAL_END;
	return CMD_FUNC_NORMAL_END;
}

/****************************************************************************/
/* �T�v		:	�h�c�Q�q�c�R�}���h��͏���									*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END		����I��						*/
/*			:	= DATA_SET_WARNG2		�f�[�^�ݒ�x���Q�i�`�D�X�S�a�j		*/
/*			:	= DATA_SET_WARNG4		�f�[�^�ݒ�x���S�i�`�D�X�S�c�j		*/
/****************************************************************************/
INT32 stdCmd_ID_RD( MLNET_HANDLE *hNetData )
{
	UCHAR		cmd, id_code, offset;
	USHORT		size;
	INT32		ret;
	CMD_HNDL	*pCmdHdl;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl = &hNetData->cmd_hdl;

	/* �R�}���h����(CMD_CTRL)��͏��� */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, &hNetData->mt_hdl );

	ret	= CMD_FUNC_NORMAL_END;
	cmd	= pCmdHdl->cmdInfo.cmdBuf.stdData.id_rd.cmd;

	/* ���s�����`�F�b�N */
	if( (pCmdHdl->cmdInfo.cmdRdy != FALSE) 
		&& ((pCmdHdl->cmdInfo.rsltBuf.stdData.id_rd.cmd != cmd) 
		||  (pCmdHdl->cmdInfo.rsltBuf.dword[1] != pCmdHdl->cmdInfo.cmdBuf.dword[1])) ) 
	{
		id_code = pCmdHdl->cmdInfo.cmdBuf.stdData.id_rd.id_code;
		offset   = pCmdHdl->cmdInfo.cmdBuf.stdData.id_rd.offset;
		size    = pCmdHdl->cmdInfo.cmdBuf.stdData.id_rd.size;

		if( size <= ID_RD_MAX_SIZE )
		{
			/* �f�o�C�X���ǂݏo���p�f�[�^�ݒ� */
			pCmdHdl->cmdInfo.RegData.AccessSize = 1;
			pCmdHdl->cmdInfo.RegData.Length = (LONG)size;
			pCmdHdl->cmdInfo.RegData.Number = (LONG)(id_code*4);	/* <S**1> */
			pCmdHdl->cmdInfo.RegData.offset = (LONG)offset;			/* <S**1> */
			pCmdHdl->cmdInfo.RegData.EepWrite = FALSE;
			pCmdHdl->cmdInfo.RegData.pBuf = (LONG*)pCmdHdl->cmdInfo.rsltBuf.stdData.id_rd.id_info;
			pCmdHdl->cmdInfo.RegData.BufferSize = sizeof(pCmdHdl->cmdInfo.rsltBuf.stdData.id_rd.id_info);
			
			/* ���W�X�^�R�}���h�����v�����sAPI	*/
			ret = RegRequestCommand(&hNetData->reg_hdl, REG_CMD_IDRD, &pCmdHdl->cmdInfo.RegData);
		}
		else
		{
			MlibResetByteMemory( pCmdHdl->cmdInfo.rsltBuf.stdData.id_rd.id_info, ID_RD_MAX_SIZE );
			ret = DATA_SET_WARNG4;
		}

		/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
		pCmdHdl->cmdInfo.rsltBuf.stdData.id_rd.cmd = cmd;
		pCmdHdl->cmdInfo.rsltBuf.dword[1] = pCmdHdl->cmdInfo.cmdBuf.dword[1];

		/* �R�}���h��͎��s���ʐݒ� */
		pCmdHdl->cmdInfo.ResultCmdAna = ret;
	}
	else
	{
		if(pCmdHdl->cmdInfo.cmdRdy == FALSE)
		{
			ret = RegCheckCommandState(&hNetData->reg_hdl);
			pCmdHdl->cmdInfo.ResultCmdAna = ret;
		}
	}

	return ret;
}


/****************************************************************************/
/* �T�v		:	�b�n�m�e�h�f�R�}���h��͏���								*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END		����I��						*/
/*			:	= CMD_FUNC_CONTINUE			�p�����s��						*/
/*			:	= CMD_WARNG1				�R�}���h�x���P�i�`�D�X�T�`�j	*/
/*			:	= DATA_SET_WARNG2			�f�[�^�ݒ�x���Q�i�`�D�X�S�a�j	*/
/****************************************************************************/
INT32 stdCmd_CONFIG( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
//	COM_HNDL	*pComHdl;
	LONG		cmd, config_mod;
	LONG		cmd_switch_time;
	BOOL		sv_on;/* <S06D> */
	INT32		result;

#if (CSW_DEBUG_PRINT_ENABLE == TRUE)
	printf( "called CONFIG command\n" );
#endif

	/* ���[�J���ϐ��̏����� */
	pCmdHdl			= &hNetData->cmd_hdl;
//	pComHdl			= &hNetData->com_hdl;
	cmd				= pCmdHdl->cmdInfo.cmdBuf.stdData.config.cmd;
	config_mod		= pCmdHdl->cmdInfo.cmdBuf.stdData.config.config_mod;
	result			= CMD_FUNC_NORMAL_END;
	cmd_switch_time = FALSE;

	sv_on			= hNetData->mt_hdl.svRsp->RspSeqBit & SERVO_ON_COMP_BITMSK;/* �T�[�{�I����� *//* <S06D> */

	/* �R�}���h����(CMD_CTRL)��͏��� */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, &hNetData->mt_hdl );

	/* ���s�����`�F�b�N */
	if( (pCmdHdl->cmdInfo.rsltBuf.stdData.config.cmd != pCmdHdl->cmdInfo.cmdBuf.stdData.config.cmd) 
		|| (pCmdHdl->cmdInfo.rsltBuf.stdData.config.config_mod != config_mod) )
	{
		pCmdHdl->cmd_seq = SV_OFF_SEQ;	/* �R�}���h�؂�ւ��ŏ����J�n */
		cmd_switch_time = TRUE;
	}

	if( (pCmdHdl->cmdInfo.cmdRdy == FALSE) || (cmd_switch_time != FALSE) )
	{ /* CMDRDY=FALSE or �R�}���h�̐؂�ւ��Ŏ��s */
		result = CMD_FUNC_CONTINUE;
		if( sv_on == FALSE )/* <S06D>:sv_on��ǉ� *//* <S0B3> */
		{
			if( config_mod == CONFIG_MOD0 )
			{ /* �p�����[�^�Čv�Z */
				if(pCmdHdl->cmd_seq == SV_OFF_SEQ)
				{
					pCmdHdl->cmdInfo.lstSvCmdCtrl = 0;	/* LT_REQ�̑O��l�N���A */
					if(MCI_SUCCESS == MciSetServoOn(&hNetData->mt_hdl, FALSE, TRUE))
					{
						/* BK�I�� */
						(hNetData->mt_hdl.svCmd)->CmdSeqBit &= ~(BRK_RELEASE_BIT);
						pCmdHdl->cmd_seq = PRM_RE_CALC_SEQ;
					}
				}
				else if(pCmdHdl->cmd_seq == PRM_RE_CALC_SEQ)
				{
					/* ���W�X�^�R�}���h�����v�����sAPI	*/
					if( REG_CONTINUE == RegRequestCommand(&hNetData->reg_hdl, REG_CMD_RECALC, NULL) ) 
					{ /* ���Z�b�g�����I�� */
						pCmdHdl->cmd_seq = MT_ENABLE_SEQ;
					}
				}
				else
				{
					result = RegCheckCommandState(&hNetData->reg_hdl);
					if(result != REG_CONTINUE)
					{
						pCmdHdl->cmd_seq = IDOL_SEQ;
//						result = CMD_FUNC_NORMAL_END; /* <S0EC> */
					}					
				}
			}
			else if( config_mod == CONFIG_MOD2 )
			{ /* �p�����[�^������ */
				if(pCmdHdl->cmd_seq == SV_OFF_SEQ)
				{
					pCmdHdl->cmdInfo.lstSvCmdCtrl = 0;	/* LT_REQ�̑O��l�N���A */
					if(MCI_SUCCESS == MciSetServoOn(&hNetData->mt_hdl, FALSE, TRUE))
					{
						/* BK�I�� */
						(hNetData->mt_hdl.svCmd)->CmdSeqBit &= ~(BRK_RELEASE_BIT);
						pCmdHdl->cmd_seq = PRM_RE_CALC_SEQ;
					}
				}
				else if(pCmdHdl->cmd_seq == PRM_RE_CALC_SEQ)
				{
					/* ���W�X�^�R�}���h�����v�����sAPI	*/
					if( REG_CONTINUE == RegRequestCommand(&hNetData->reg_hdl, REG_CMD_INITPRM, NULL) ) 
					{ /* ���Z�b�g�����I�� */
						pCmdHdl->cmd_seq = MT_ENABLE_SEQ;
					}
				}
				else
				{
					result = RegCheckCommandState(&hNetData->reg_hdl);
					if(result != REG_CONTINUE)
					{
						pCmdHdl->cmd_seq = IDOL_SEQ;
//						result = CMD_FUNC_NORMAL_END; /* <S0EC> */
					}					
				}
			}
			else
				{/* mode�͈͊O	*/
				result = DATA_SET_WARNG2;
			}
		}
		else/* <S0B3> */
		{/* �T�[�{�I����	*/
			result = CMD_WARNG1;
		}
		/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
		pCmdHdl->cmdInfo.rsltBuf.stdData.config.cmd = (UCHAR)cmd;
		pCmdHdl->cmdInfo.rsltBuf.stdData.config.config_mod = (UCHAR)config_mod;

		/* �R�}���h��͎��s���ʐݒ� */
		pCmdHdl->cmdInfo.ResultCmdAna = result;
	}

#if (CSW_DEBUG_PRINT_ENABLE == TRUE)
	if( result == CMD_FUNC_NORMAL_END )
	{
		printf( "CONFIG command normal end\n\n" );
	}
#endif

	return result;
}


/****************************************************************************/
/* �T�v		:	�`�k�l�Q�q�c�R�}���h��͏���								*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END		����I��						*/
/*			:	= CMD_FUNC_CONTINUE			�p�����s��						*/
/*			:	= DATA_SET_WARNG2			�f�[�^�ݒ�x���Q�i�`�D�X�S�a�j	*/
/****************************************************************************/
INT32 stdCmd_ALM_RD( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	UCHAR		cmd;
	USHORT		alm_rd_mod, alm_idx;
	USHORT		*alm_data;
	INT32		ret;
	
	/* ���[�J���ϐ��̏����� */
	ret		= CMD_FUNC_NORMAL_END;
	pCmdHdl	= &hNetData->cmd_hdl;

	/* �R�}���h����(CMD_CTRL)��͏��� */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, &hNetData->mt_hdl );

	cmd			= pCmdHdl->cmdInfo.cmdBuf.stdData.alm.cmd;
	alm_rd_mod	= pCmdHdl->cmdInfo.cmdBuf.stdData.alm.mode;
	alm_idx		= pCmdHdl->cmdInfo.cmdBuf.stdData.alm.index;

	/* ���s�����`�F�b�N */
	if( (pCmdHdl->cmdInfo.cmdRdy == FALSE)
		|| (pCmdHdl->cmdInfo.rsltBuf.stdData.alm.cmd != cmd) 
		|| (pCmdHdl->cmdInfo.rsltBuf.stdData.alm.mode != alm_rd_mod) 
		|| (pCmdHdl->cmdInfo.rsltBuf.stdData.alm.index != alm_idx) )
	{
		/* AlmTblMax = 10 */
		alm_data = &pCmdHdl->cmdInfo.rsltBuf.stdData.alm.data[0];
		ret = stdCmdAlmRdProc( &hNetData->alm_hdl, alm_rd_mod, alm_idx, alm_data, ALM_RD_NUM );

		/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
		pCmdHdl->cmdInfo.rsltBuf.stdData.alm.cmd  = cmd;
		pCmdHdl->cmdInfo.rsltBuf.stdData.alm.mode = alm_rd_mod;
		pCmdHdl->cmdInfo.rsltBuf.stdData.alm.index = alm_idx;
		
		/* �R�}���h��͎��s���ʐݒ� *//* <S14D> */
		pCmdHdl->cmdInfo.ResultCmdAna = ret;
	}

	return ret;
}


/****************************************************************************/
/* �T�v		:	�A���[���ǂݏo���`�o�h���s����								*/
/****************************************************************************/
static	INT32 stdCmdAlmRdProc( ALM_HNDL *alm_hdl, USHORT alm_rd_mod, USHORT idx, 
											USHORT *alm_data, LONG read_num )
{
	INT32 ret;

	if( alm_rd_mod == ALM_RD_MOD0 )
	{ /* ���ݔ����A���[�����[�h */
		ret = AlmGetCrtDetailBufAll( alm_hdl, alm_data, read_num );
	}
	else if( alm_rd_mod == ALM_RD_MOD1 )
	{ /* �A���[���������[�h */
		ret = AlmGetHistoryDetailAll( alm_hdl, alm_data, read_num );
	}
	else
	{
		ret = DATA_SET_WARNG2;
	}
	return ret;
}


/****************************************************************************/
/* �T�v		:	�`�k�l�Q�b�k�q�R�}���h��͏���								*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END		����I��						*/
/*			:	= CMD_FUNC_CONTINUE			�p�����s��						*/
/*			:	= DATA_SET_WARNG2			�f�[�^�ݒ�x���Q�i�`�D�X�S�a�j	*/
/*			:	= CMD_WARNG1				�R�}���h�x���P�i�`�D�X�T�`�j	*/
/****************************************************************************/
INT32 stdCmd_ALM_CLR( MLNET_HANDLE *hNetData )
{
	CMD_HNDL				*pCmdHdl;
	UCHAR					cmd;			
	USHORT					alm_clr_mod;	
	INT32					ret;

	/* ���[�J���ϐ��̏����� */
	ret		= CMD_FUNC_NORMAL_END;
	pCmdHdl	= &hNetData->cmd_hdl;

	/* �R�}���h����(CMD_CTRL)��͏��� */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, &hNetData->mt_hdl );

	cmd			= pCmdHdl->cmdInfo.cmdBuf.stdData.alm.cmd;
	alm_clr_mod	= pCmdHdl->cmdInfo.cmdBuf.stdData.alm.mode;

	/* ���s�����`�F�b�N */
	if( (pCmdHdl->cmdInfo.cmdRdy != FALSE)
	    && ((pCmdHdl->cmdInfo.rsltBuf.stdData.alm.cmd  != cmd) 
	    || (pCmdHdl->cmdInfo.rsltBuf.stdData.alm.mode != alm_clr_mod)) )
	{
		if( alm_clr_mod == ALM_CLR_MOD0 )
		{
			ret = AlmSetClearReq( &hNetData->alm_hdl, TRUE );
		}
		else if( alm_clr_mod == ALM_CLR_MOD1 )
		{
			AlmRequestAlarmHistoryClear( &hNetData->alm_hdl );
			ret = CMD_FUNC_CONTINUE;
		}
		else
		{
			ret = DATA_SET_WARNG2;
		}

		/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
		pCmdHdl->cmdInfo.rsltBuf.stdData.alm.cmd  = cmd;
		pCmdHdl->cmdInfo.rsltBuf.stdData.alm.mode = alm_clr_mod;

		/* �R�}���h��͎��s���ʐݒ� */
		pCmdHdl->cmdInfo.ResultCmdAna = ret;
	}
	else
	{
		if(pCmdHdl->cmdInfo.cmdRdy == FALSE)
		{
			if( alm_clr_mod == ALM_CLR_MOD0 )
			{
				ret = AlmSetClearReq( &hNetData->alm_hdl, FALSE );
			}
			else //if( alm_clr_mod == ALM_CLR_MOD1 )
			{
				ret = AlmGetAlarmHistoryClearStatus( &hNetData->alm_hdl );
			}
			pCmdHdl->cmdInfo.ResultCmdAna = ret;
		}
	}

	return ret;
}


/****************************************************************************/
/* �T�v		:	�r�x�m�b�Q�r�d�s�R�}���h��͏���							*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END		����I��						*/
/*			:	= CMD_FUNC_CONTINUE			�p�����s��						*/
/*			:	= CMD_SYNC_FAULT			�l�|�U�������s�i�`�D�d�T�P�j	*/
/*			:	= CMD_COM_ERR				�l�|�U�ʐM�ُ�i�`�D�d�U�O�j	*/
/*			:   = CMD_WARNG1				�R�}���h�x���P�i�`�D�X�T�`�j	*/
/****************************************************************************/
INT32 stdCmd_SYNC_SET( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	COM_HNDL	*pComHdl;
	MT_HNDL		*pMtHdl;
	UCHAR		cmd;
	INT32		ret, result;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl		= &hNetData->cmd_hdl;
	pComHdl		= &hNetData->com_hdl;
	pMtHdl		= &hNetData->mt_hdl;
	result		= CMD_FUNC_CONTINUE;

	/* �R�}���h����(CMD_CTRL)��͏��� */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, pMtHdl );

	cmd	= pCmdHdl->cmdInfo.cmdBuf.stdData.nop.cmd;
	/* ���s�����`�F�b�N */
	if( (pCmdHdl->cmdInfo.rsltBuf.stdData.nop.cmd != cmd) && (pComHdl->Phase == 2) )
	{
		pCmdHdl->cmd_seq = SV_OFF_SEQ;	/* �R�}���h�ؑւ� && Ph=2�ŏ����J�n */
	}

	switch(pCmdHdl->cmd_seq)
	{
	case SV_OFF_SEQ :		/* �T�[�{�I�t�w�� */
		if( MciSetServoOn( pMtHdl, FALSE, TRUE) == MCI_SUCCESS )
		{
			pCmdHdl->cmd_seq = MT_DISABLE_SEQ;
		}
		break;
	case MT_DISABLE_SEQ :	/* �u���[�L�I���E���[�V�������� */
		if( MciEnableState( pMtHdl, FALSE ) == MCI_SUCCESS )
		{
			pCmdHdl->cmd_seq = REQ_PHSUP_SEQ;
		}
		break;
	case REQ_PHSUP_SEQ :	/* �t�F�[�Y�A�b�v�v�� */
		ret = ComReqPhaseUp( pComHdl, 3, 
				pComHdl->NetPrm.tType, pComHdl->NetPrm.aplCycle, NdGetComMode() );
		pCmdHdl->cmd_seq = (ret == COM_FUNC_NORMAL_END) ? 
							PHASE_CHK_SEQ : FAIL_END_SEQ;
		break;
	case PHASE_CHK_SEQ :	/* �t�F�[�Y�A�b�v�`�F�b�N */
		ret = (pComHdl->reqPhase != 0) ? COM_FUNC_CONTINUE : pComHdl->PhUpSts;
		if( ret == CMD_FUNC_NORMAL_END )
		{ /* �t�F�[�Y�ڍs����������I�������� */
			if( MciEnableState(pMtHdl, TRUE) == MCI_SUCCESS )
			{ /* ���[�V�����L�� */
				pCmdHdl->cmd_seq = IDOL_SEQ;
				result = CMD_FUNC_NORMAL_END;
				pCmdHdl->cmdInfo.ResultCmdAna = result;/* <S14D> */
			}
		}
		else if(ret != CMD_FUNC_CONTINUE)
		{ /* �t�F�[�Y�ڍs�������ُ�I�������� */
			pCmdHdl->cmd_seq = FAIL_END_SEQ;
		}
		break;
	case FAIL_END_SEQ :	/* �V�[�P���X���s */
		if( MciEnableState(pMtHdl, TRUE) == MCI_SUCCESS )
		{
			pCmdHdl->cmd_seq = IDOL_SEQ;
			result = CMD_SYNC_FAULT;
			pCmdHdl->cmdInfo.ResultCmdAna = result;/* <S14D> */
		}
		break;
	default:
		result = CMD_FUNC_NORMAL_END;
		break;
	}

	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->cmdInfo.rsltBuf.stdData.nop.cmd = cmd;
//	/* �R�}���h��͎��s���ʐݒ� */
//	pCmdHdl->cmdInfo.ResultCmdAna = result;/* <S14D> */

	return result;
}


/****************************************************************************/
/* �T�v		:	�b�n�m�m�d�b�s�R�}���h��͏���								*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END		����I��						*/
/*			:	= CMD_FUNC_CONTINUE			�p�����s��						*/
/*			:	= DATA_SET_WARNG2			�f�[�^�ݒ�x���Q�i�`�D�X�S�a�j	*/
/*			:	= CMD_WARNG1				�R�}���h�x���P�i�`�D�X�T�`�j	*/
/*			:   = CMD_TCYC_ERROR			�`�������ݒ�ُ�i�`�D�d�S�O�j	*/
/*			:	= CMD_SYNC_FAULT			�l�|�U�������s�i�`�D�d�T�P�j	*/
/****************************************************************************/
INT32 stdCmd_CONNECT( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	COM_HNDL	*pComHdl;
	MT_HNDL		*pMtHdl;
	CYC_CHG_PRM	*cycPrm;
	LONG		com_tim, dtmod, reqphs;
	INT32		ret, rc;
	BOOL		AsyncMode;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl		= &hNetData->cmd_hdl;
	pComHdl		= &hNetData->com_hdl;
	pMtHdl		= &hNetData->mt_hdl;
	com_tim		= pCmdHdl->cmdInfo.cmdBuf.stdData.connect.com_time;		/* �ʐM���� */
	rc			= CMD_FUNC_CONTINUE;

	/* �R�}���h����(CMD_CTRL)��͏��� */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, pMtHdl );
	
	/* ���s�����`�F�b�N */
	if( (pCmdHdl->cmdInfo.cmdRdy) && (pComHdl->Phase == 1) )
	{ /* IDOL��� && Phase=1 �ŏ����J�n */
		pCmdHdl->cmd_seq = REQ_PHSUP_SEQ;
	}

	switch(pCmdHdl->cmd_seq)
	{
	case REQ_PHSUP_SEQ:	/* �t�F�[�Y�A�b�v�v�� */
		AsyncMode = (BOOL)NdGetComMode();
		ret = stdCmdChkConnectCmd( hNetData, &reqphs, &dtmod, &com_tim, AsyncMode);
		if( ret == CMD_FUNC_NORMAL_END )
		{ /* �R�}���h���펞 */

			/* �v���t�@�C���^�C�v�ݒ菈�� */
			CmdSetProfileType( hNetData, 0);

			if( COM_FUNC_NORMAL_END == 
					ComReqPhaseUp(pComHdl, reqphs, dtmod, com_tim, AsyncMode) )
			{ /* ����Ȃ玟�̃V�[�P���X�ցA���s���̓��g���C */
				pCmdHdl->cmd_seq = PHASE_CHK_SEQ;
			}
		}
		else if( ret != CMD_FUNC_CONTINUE )
		{ /* �ُ펞 */
			rc = ret;
			pCmdHdl->cmdInfo.ResultCmdAna = rc;/* <S14D> */
			pCmdHdl->cmd_seq = IDOL_SEQ;
		}
		break;
	case PHASE_CHK_SEQ:	/* �t�F�[�Y�A�b�v�`�F�b�N */
		ret = (pComHdl->reqPhase != 0) ? COM_FUNC_CONTINUE : pComHdl->PhUpSts;
		if( ret == COM_FUNC_NORMAL_END )
		{
			pCmdHdl->cmd_seq = PRM_RE_CALC_SEQ;
		}
		else if( ret != COM_FUNC_CONTINUE )
		{ /* �ُ�I�� */
			if( pComHdl->Phase <= 1 )
			{ /* �t�F�[�Y�P�Ȃ�G���[�R�[�h���Z�b�g */
				rc = ret;
				pCmdHdl->cmdInfo.ResultCmdAna = rc;/* <S14D> */
				pCmdHdl->cmd_seq = IDOL_SEQ;
			}
			else
			{
				pCmdHdl->cmd_seq = FAIL_END_SEQ;
			}
		}
		break;
	case PRM_RE_CALC_SEQ:	/* �p�����[�^�Čv�Z���s */
		cycPrm = (CYC_CHG_PRM*)pCmdHdl->cmdInfo.RegWkBuf;
		cycPrm->dlyTime = 0;
		cycPrm->scanCyc = (USHORT)CFG_TIO_BASE_TIME;
		cycPrm->intCyc = NdGetTransmissionCycle();
		cycPrm->aplCyc = (USHORT)pComHdl->NetPrm.aplCycle;
		pCmdHdl->cmdInfo.RegData.pBuf = pCmdHdl->cmdInfo.RegWkBuf;
		
			/* ���W�X�^�R�}���h�����v�����sAPI	*/
		RegRequestCommand(&hNetData->reg_hdl, REG_CMD_CHGCYC, &pCmdHdl->cmdInfo.RegData);
		pCmdHdl->cmd_seq = PRM_ACCESS_SEQ;
		break;
	case PRM_ACCESS_SEQ:	/* �p�����[�^�Čv�Z���s�����܂� */
		if(0 >= RegCheckCommandState(&hNetData->reg_hdl))
		{	pCmdHdl->cmd_seq = MT_ENABLE_SEQ;}
		break;
	case MT_ENABLE_SEQ:		/* ���[�V�����L�� */
		if( MciEnableState( pMtHdl, TRUE ) == MCI_SUCCESS )
		{
			pCmdHdl->cmd_seq = IDOL_SEQ;
			rc = CMD_FUNC_NORMAL_END;
			pCmdHdl->cmdInfo.ResultCmdAna = rc;/* <S14D> */
		}
		break;
	case FAIL_END_SEQ:		/* �V�[�P���X���s */
		if( MciEnableState(pMtHdl, TRUE) == MCI_SUCCESS )
		{
			pCmdHdl->cmd_seq = IDOL_SEQ;
			rc = CMD_SYNC_FAULT;
			pCmdHdl->cmdInfo.ResultCmdAna = rc;/* <S14D> */
		}
		break;
	default:
		rc = CMD_FUNC_NORMAL_END;
		break;
	}
	
	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->cmdInfo.rsltBuf.stdData.connect.cmd =
					pCmdHdl->cmdInfo.cmdBuf.stdData.connect.cmd;
	pCmdHdl->cmdInfo.rsltBuf.stdData.connect.ver =
					pCmdHdl->cmdInfo.cmdBuf.stdData.connect.ver;
	pCmdHdl->cmdInfo.rsltBuf.stdData.connect.com_mod =
					pCmdHdl->cmdInfo.cmdBuf.stdData.connect.com_mod;
	pCmdHdl->cmdInfo.rsltBuf.stdData.connect.com_time =
					pCmdHdl->cmdInfo.cmdBuf.stdData.connect.com_time;
	pCmdHdl->cmdInfo.rsltBuf.stdData.connect.profile_type =
					pCmdHdl->cmdInfo.cmdBuf.stdData.connect.profile_type;

	/* �R�}���h��͎��s���ʐݒ� */
//	pCmdHdl->cmdInfo.ResultCmdAna = rc;/* <S14D> */

	return rc;
}


/****************************************************************************/
/* �T�v		:	�b�n�m�m�d�b�s�R�}���h�f�[�^�`�F�b�N����					*/
/****************************************************************************/
static INT32 stdCmdChkConnectCmd( MLNET_HANDLE *hNetData, LONG *reqphs, 
								  LONG *dtmod, LONG *com_tim, BOOL AsyncMode )
{
	CMD_HNDL	*pCmdHdl;
	LONG		ver, com_mod, profile;
	LONG		t_byte;		/* <S07A> */
	BOOL		sv_on;		/* <S07A> */
	INT32		ret;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl		= &hNetData->cmd_hdl;
	ver			= pCmdHdl->cmdInfo.cmdBuf.stdData.connect.ver;				/* �o�[�W���� */
	com_mod		= pCmdHdl->cmdInfo.cmdBuf.stdData.connect.com_mod;			/* �ʐM���[�h */
	profile		= pCmdHdl->cmdInfo.cmdBuf.stdData.connect.profile_type;		/* �v���t�@�C���^�C�v */
	t_byte		= hNetData->com_hdl.NetPrm.dataSize;						/* �`���o�C�g�� */		/* <S07A> */
	sv_on		= hNetData->mt_hdl.svRsp->RspSeqBit & SERVO_ON_COMP_BITMSK; /* �T�[�{�I����� */	/* <S07A> */
	ret			= CMD_FUNC_NORMAL_END;

	/* ���s�����`�F�b�N *//*<S07A1> */
	if( sv_on != FALSE )
	{
		return  CMD_WARNG1;
	}

	if( ver != 0x30 )
	{/*	���T�|�[�gVer	*/
		return  DATA_SET_WARNG2;
	}

	/* Profile�ݒ� */
	switch(profile)
	{
	case M3STD_PROFILE:
		if(AsyncMode != FALSE)
		{
			return  DATA_SET_WARNG2;
		}
		if( (com_mod & 0x80)&&(t_byte != 48) )/* <S07A> */
		{
			return  DATA_SET_WARNG2;
		}
		break;

	case EVTID_PROFILE:
		if(AsyncMode == FALSE)
		{
			ret = DATA_SET_WARNG2;
		}

		pCmdHdl->useExCmd = FALSE;
		*reqphs = 2;
		return ret;

	case M2_PROFILE:
	default:
		return  DATA_SET_WARNG2;
	}

	/* �T�u�R�}���h�g�p�^���g�p�ݒ� */
	pCmdHdl->useExCmd = (com_mod >> 7) & 0x1;

	/* �v���t�F�[�Y�ݒ� */
	*reqphs = ((com_mod & 0x02) == 0x2) ? 3 : 2;

	/* �`�����[�h�ݒ� */
	com_mod = ( com_mod >> 2 ) & 0x3;
	if( com_mod == 0x0 )
	{ /* 00b: �P�����[�h */
		if( (*com_tim < 1) /*|| (com_tim > 32)*/ )
		{
			return DATA_SET_WARNG2;
		}
		*dtmod = SINGLE_TRANSFER;
	}
	else if( com_mod == 0x1 )
	{ /* 01b: �A�����[�h */
		if( *com_tim < 2 )
		{/* 2�{�����Ȃ�NG */
			return DATA_SET_WARNG2;
		}
		*dtmod = CONSECUTIVE_TRANSFER;
	}
	else
	{ /* 10b,11b :�\�� */
		return DATA_SET_WARNG2;
	}
	return ret;
}


/****************************************************************************/
/* �T�v		:	�c�h�r�b�n�m�m�d�b�s�R�}���h��͏���						*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END		����I��						*/
/*			:	= CMD_FUNC_CONTINUE			�p�����s��						*/
/*			:	= CMD_WARNG1				�R�}���h�x���P�i�`�D�X�T�`�j	*/
/****************************************************************************/
INT32 stdCmd_DISCONNECT( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	COM_HNDL	*pComHdl;
	UCHAR		cmd;
	INT32		result;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl	= &hNetData->cmd_hdl;
	pComHdl	= &hNetData->com_hdl;
	cmd		= pCmdHdl->cmdInfo.cmdBuf.stdData.nop.cmd;
//	result	= CMD_FUNC_CONTINUE;

	/* ���s�����`�F�b�N */
	if( (pCmdHdl->cmdInfo.rsltBuf.stdData.nop.cmd != cmd)
		&& ((pComHdl->Phase > 1) || (pComHdl->reqPhase > 1)) )
	{
		/* �R�}���h�؂�ւ�� && (����Phase2�ȏ� or PhaseUP��) */
		pCmdHdl->cmd_seq = REQ_PHSDOWN_SEQ;
	}

	/* DISCONNECT�������s */
	result = CmdRunDisConnect( hNetData );

	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->cmdInfo.rsltBuf.stdData.nop.cmd = cmd;

	/* �R�}���h��͎��s���ʐݒ� */
	pCmdHdl->cmdInfo.ResultCmdAna = result;

	return result;
}


/****************************************************************************/
/* �T�v		:	�l�d�l�Q�q�c�R�}���h��͏���								*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END	����I��							*/
/*			:	= CMD_FUNC_CONTINUE		�p�����s��							*/
/*			:	= DATA_SET_WARNG4		�f�[�^�ݒ�x���S	�i�`�D�X�S�c�j	*/
/*			:	= REG_SIZE_ERR			�f�[�^�T�C�Y�x��	�i�`�D�X�S�c�j	*/
/*			:	= REG_NO_ERR			�A�h���X�ُ�		�i�`�D�X�S�`�j	*/
/****************************************************************************/
INT32 stdCmd_MEM_RD( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	UCHAR		cmd;
	LONG		mode, type;
	LONG		adrs;
	LONG		size, accSize, buf;/* <S14D> */
	INT32		ret;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl		= &hNetData->cmd_hdl;

	/* �R�}���h����(CMD_CTRL)��͏��� */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, &hNetData->mt_hdl );

	cmd	 = pCmdHdl->cmdInfo.cmdBuf.stdData.mem.cmd;	
	adrs = pCmdHdl->cmdInfo.cmdBuf.stdData.mem.address;
	ret	 = CMD_FUNC_NORMAL_END;

	/* ���s�����`�F�b�N */
	if( (pCmdHdl->cmdInfo.cmdRdy != FALSE)
		&& ((pCmdHdl->cmdInfo.rsltBuf.stdData.mem.cmd != cmd)
		||  (pCmdHdl->cmdInfo.rsltBuf.stdData.mem.address != adrs)
		||  (pCmdHdl->cmdInfo.rsltBuf.dword[1] != pCmdHdl->cmdInfo.cmdBuf.dword[1])) )
	{
		size = pCmdHdl->cmdInfo.cmdBuf.stdData.mem.size;
		mode = pCmdHdl->cmdInfo.cmdBuf.stdData.mem.mode >> 4;
		type = pCmdHdl->cmdInfo.cmdBuf.stdData.mem.mode & 0x0F;
		/* �A�N�Z�X����f�[�^�T�C�Y�擾 */
		if ( (mode != 1) || (type < 1) || (type >= 4) )
		{
			ret = DATA_SET_WARNG2;		/* A.94B �f�[�^�ݒ�ُ�			*/
		}
		else if(size == 0)/* <S14D> */
		{
			ret = DATA_SET_WARNG4;			/* A.94D �f�[�^�ݒ�x���S */
		}
		else
		{
			/* �f�o�C�X���ǂݏo���p�f�[�^�ݒ� */
/* <S14D> */
//			pCmdHdl->cmdInfo.RegData.AccessSize = 1 << (type-1);
//			pCmdHdl->cmdInfo.RegData.Length = size;
			accSize = 1 << (type-1);
			if((adrs >= VMEM_COM_MIN) && (adrs <= VMEM_COM_MAX))
			{/* ���ʃp�����[�^�̈�	*/
				buf = accSize * size;
				pCmdHdl->cmdInfo.RegData.AccessSize = 4;
				pCmdHdl->cmdInfo.RegData.Length = buf >> 2;	/* buf�́A4�̐����{("accSize < 4"�łȂ����)	*/
				if( accSize < 4 )
				{/* ���ʃp�����[�^�̈�́A�A�N�Z�X�T�C�Y��4�̂�	*/
					ret = DATA_SET_WARNG4;			/* A.94D �f�[�^�ݒ�x���S */
				}
			}
			else if(adrs >= VMEM_VEN_MIN)
			{/* �x���_�[�̈�	*/
				buf = accSize * size;
				pCmdHdl->cmdInfo.RegData.AccessSize = 2;
				pCmdHdl->cmdInfo.RegData.Length = buf >> 1;	/* buf�́A2�̐����{("accSize < 2"�łȂ����)	*/
				if( accSize < 2 )
 				{/* �x���_�́A�A�N�Z�X�T�C�Y��2,4�̂�	*/
					ret = DATA_SET_WARNG4;			/* A.94D �f�[�^�ݒ�x���S */
				}
			}
			else if(adrs <= VMEM_ID_MAX)
			{
				buf = accSize * size;
				pCmdHdl->cmdInfo.RegData.AccessSize = 1;
				pCmdHdl->cmdInfo.RegData.Length = buf;
			}
			else
			{
				pCmdHdl->cmdInfo.RegData.AccessSize = accSize;
				pCmdHdl->cmdInfo.RegData.Length = size;
			}
/* <S14D> */
			if(ret == CMD_FUNC_NORMAL_END)
			{
				pCmdHdl->cmdInfo.RegData.Number = adrs;
				pCmdHdl->cmdInfo.RegData.EepWrite = FALSE;
				pCmdHdl->cmdInfo.RegData.pBuf = pCmdHdl->cmdInfo.rsltBuf.stdData.mem.ldata;
				pCmdHdl->cmdInfo.RegData.BufferSize = sizeof(pCmdHdl->cmdInfo.rsltBuf.stdData.mem.ldata);
			
				/* ���W�X�^�R�}���h�����v�����sAPI	*/
				ret = RegRequestCommand(&hNetData->reg_hdl, REG_CMD_PRMRD, &pCmdHdl->cmdInfo.RegData);
			}
		}
		/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
		pCmdHdl->cmdInfo.rsltBuf.stdData.mem.cmd = cmd;
		pCmdHdl->cmdInfo.rsltBuf.stdData.mem.address = adrs;
		pCmdHdl->cmdInfo.rsltBuf.dword[1] = pCmdHdl->cmdInfo.cmdBuf.dword[1];
		/* �R�}���h��͎��s���ʐݒ� */
		pCmdHdl->cmdInfo.ResultCmdAna = ret;
	}
	else
	{
		if(pCmdHdl->cmdInfo.cmdRdy == FALSE)
		{
			ret = RegCheckCommandState(&hNetData->reg_hdl);
			pCmdHdl->cmdInfo.ResultCmdAna = ret;
		}
	}
	return ret;
}


/****************************************************************************/
/* �T�v		:	�l�d�l�Q�v�q�R�}���h��͏���								*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END	����I��							*/
/*			:	= CMD_FUNC_CONTINUE		�p�����s��							*/
/*			:	= DATA_SET_WARNG4		�f�[�^�ݒ�x���S	�i�`�D�X�S�c�j	*/
/*			:	= REG_SIZE_ERR			�f�[�^�T�C�Y�x��	�i�`�D�X�S�c�j	*/
/*			:	= REG_NO_ERR			�A�h���X�ُ�		�i�`�D�X�S�`�j	*/
/*			:	= ERR_CMD_CNDTN			�R�}���h�x��(�����O)�i�`�D�X�T�`�j	*/
/****************************************************************************/
INT32 stdCmd_MEM_WR( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	UCHAR		cmd;
	ULONG		adrs;
	LONG		size;
	LONG		mode, type;
	INT32		ret;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl			= &hNetData->cmd_hdl;

	/* �R�}���h����(CMD_CTRL)��͏��� */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, &hNetData->mt_hdl );

	cmd	 = pCmdHdl->cmdInfo.cmdBuf.stdData.mem.cmd;	
	adrs = pCmdHdl->cmdInfo.cmdBuf.stdData.mem.address;
	ret  = CMD_FUNC_NORMAL_END;

	/* ���s�����`�F�b�N */
	if( (pCmdHdl->cmdInfo.cmdRdy != FALSE)
		&& ((pCmdHdl->cmdInfo.rsltBuf.stdData.mem.cmd != cmd)
		||	(pCmdHdl->cmdInfo.rsltBuf.stdData.mem.address != adrs) 
		||	(pCmdHdl->cmdInfo.rsltBuf.dword[1] != pCmdHdl->cmdInfo.cmdBuf.dword[1])
		||	(pCmdHdl->cmdInfo.rsltBuf.stdData.mem.ldata[0] != pCmdHdl->cmdInfo.cmdBuf.stdData.mem.ldata[0])
		||	(pCmdHdl->cmdInfo.rsltBuf.stdData.mem.ldata[1] != pCmdHdl->cmdInfo.cmdBuf.stdData.mem.ldata[1])
		||	(pCmdHdl->cmdInfo.rsltBuf.stdData.mem.ldata[2] != pCmdHdl->cmdInfo.cmdBuf.stdData.mem.ldata[2])
		||	(pCmdHdl->cmdInfo.rsltBuf.stdData.mem.ldata[3] != pCmdHdl->cmdInfo.cmdBuf.stdData.mem.ldata[3])
		||	(pCmdHdl->cmdInfo.rsltBuf.stdData.mem.ldata[4] != pCmdHdl->cmdInfo.cmdBuf.stdData.mem.ldata[4])) )
	{
		/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
		pCmdHdl->cmdInfo.rsltBuf.stdData.mem.cmd = cmd;
		pCmdHdl->cmdInfo.rsltBuf.stdData.mem.address = adrs;
		pCmdHdl->cmdInfo.rsltBuf.dword[1] = pCmdHdl->cmdInfo.cmdBuf.dword[1];
		pCmdHdl->cmdInfo.rsltBuf.stdData.mem.ldata[0] = pCmdHdl->cmdInfo.cmdBuf.stdData.mem.ldata[0];
		pCmdHdl->cmdInfo.rsltBuf.stdData.mem.ldata[1] = pCmdHdl->cmdInfo.cmdBuf.stdData.mem.ldata[1];
		pCmdHdl->cmdInfo.rsltBuf.stdData.mem.ldata[2] = pCmdHdl->cmdInfo.cmdBuf.stdData.mem.ldata[2];
		pCmdHdl->cmdInfo.rsltBuf.stdData.mem.ldata[3] = pCmdHdl->cmdInfo.cmdBuf.stdData.mem.ldata[3];
		pCmdHdl->cmdInfo.rsltBuf.stdData.mem.ldata[4] = pCmdHdl->cmdInfo.cmdBuf.stdData.mem.ldata[4];

		size = pCmdHdl->cmdInfo.cmdBuf.stdData.mem.size;
		mode = pCmdHdl->cmdInfo.cmdBuf.stdData.mem.mode >> 4;
		type = pCmdHdl->cmdInfo.cmdBuf.stdData.mem.mode & 0x0F;
		/* �A�N�Z�X����f�[�^�T�C�Y�擾 */
		if( (mode < 1) || (mode > 2) || (type < 1) || (type >= 4) )
		{
			ret = DATA_SET_WARNG2;			/* A.94B �f�[�^�ݒ�ُ� */
		}
		else if(size == 0)/* <S14D> */
		{
			ret = DATA_SET_WARNG4;			/* A.94D �f�[�^�ݒ�x���S */
		}
		else
		{
			/* �f�o�C�X���ǂݏo���p�f�[�^�ݒ� */
			pCmdHdl->cmdInfo.RegData.AccessSize = 1 << (type-1);
			pCmdHdl->cmdInfo.RegData.Length = size;
			pCmdHdl->cmdInfo.RegData.Number = adrs;
			pCmdHdl->cmdInfo.RegData.EepWrite = (mode-1);
			pCmdHdl->cmdInfo.RegData.pBuf = pCmdHdl->cmdInfo.rsltBuf.stdData.mem.ldata;
			pCmdHdl->cmdInfo.RegData.BufferSize = sizeof(pCmdHdl->cmdInfo.rsltBuf.stdData.mem.ldata);
			
			/* ���W�X�^�R�}���h�����v�����sAPI	*/
			ret = RegRequestCommand(&hNetData->reg_hdl, REG_CMD_PRMWR, &pCmdHdl->cmdInfo.RegData);
		}
		/* �R�}���h��͎��s���ʐݒ� */
		pCmdHdl->cmdInfo.ResultCmdAna = ret;
	}
	else
	{
		if(pCmdHdl->cmdInfo.cmdRdy == FALSE)
		{
			ret = RegCheckCommandState(&hNetData->reg_hdl);
			pCmdHdl->cmdInfo.ResultCmdAna = ret;
		}
	}
	return ret;
}


/****************************************************************************/
/* �T�v		:	�o�n�r�Q�r�d�s�R�}���h��͏���								*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END		����I��						*/
/*			:	= DATA_SET_WARNG2			�f�[�^�ݒ�x���Q�i�`�D�X�S�a�j	*/
/****************************************************************************/
INT32 stdCmd_POS_SET( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	ULONG		pos_set_mod;								/* ���W�ݒ胂�[�h */
	UCHAR		refe;										/* ���t�@�����X�_ */
	LONG		pos_data;									/* �ݒ�ʒu */
	INT32		ret;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;
	pos_set_mod	= pCmdHdl->cmdInfo.cmdBuf.stdData.pos_set.pos_set_mod;
	refe		= (UCHAR)( (pos_set_mod & 0x80) >> 7 );
	pos_data	= pCmdHdl->cmdInfo.cmdBuf.stdData.pos_set.pos_data;

	/* �r�b�g�w�߉�͏��� */
	/* �R�}���h����(CMD_CTRL)���� */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, pMtHdl );
	/* �T�[�{�R�}���h����t�B�[���h(SVCMD_CTRL)���� */
	stdCmdSvCmdCtrlProc( pCmdHdl, pMtHdl, FALSE );
	/* �T�[�{�R�}���h���o�͐M��(SVCMD_IO)���� */
	stdCmdSvCmdIoProc( pCmdHdl, pMtHdl );

	if( (pos_set_mod & 0xf) == 0x00 )
	{	/* ���W�n�ݒ�=APOS�̏ꍇ */
		ret = MciSetCordinatePos( pMtHdl, pos_data, refe, TRUE );
	}
	else
	{	/* ���W�n�ݒ�!=APOS�̏ꍇ */
		ret = DATA_SET_WARNG2;
	}

	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->cmdInfo.rsltBuf.stdData.pos_set.cmd = pCmdHdl->cmdInfo.cmdBuf.stdData.pos_set.cmd;
	pCmdHdl->cmdInfo.rsltBuf.stdData.pos_set.pos_set_mod = pos_set_mod;
	pCmdHdl->cmdInfo.rsltBuf.stdData.pos_set.pos_data = pos_data;

	/* �R�}���h��͎��s���ʐݒ� */
	pCmdHdl->cmdInfo.ResultCmdAna = ret;

	return ret;
}


/****************************************************************************/
/* �T�v		:	�a�q�j�Q�n�m�R�}���h��͏���								*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END		����I��						*/
/****************************************************************************/
INT32 stdCmd_BRK_ON( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;

	/* �r�b�g�w�߉�͏��� */
	/* �R�}���h����(CMD_CTRL)���� */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, pMtHdl );
	/* �T�[�{�R�}���h����t�B�[���h(SVCMD_CTRL)���� */
	stdCmdSvCmdCtrlProc( pCmdHdl, pMtHdl, FALSE );
	/* �T�[�{�R�}���h���o�͐M��(SVCMD_IO)���� */
	stdCmdSvCmdIoProc( pCmdHdl, pMtHdl );

	/* �a�j�w�߃I�� */
	pMtHdl->svCmd->CmdSeqBit &= ~(BRK_RELEASE_BIT);
	
	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->cmdInfo.rsltBuf.stdData.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.stdData.mt.cmd;

	/* �R�}���h��͎��s���ʐݒ� */
	pCmdHdl->cmdInfo.ResultCmdAna = CMD_FUNC_NORMAL_END;

	return CMD_FUNC_NORMAL_END;
}


/****************************************************************************/
/* �T�v		:	�a�q�j�Q�n�e�e�R�}���h��͏���								*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END		����I��						*/
/****************************************************************************/
INT32 stdCmd_BRK_OFF( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;

	/* �r�b�g�w�߉�͏��� */
	/* �R�}���h����(CMD_CTRL)���� */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, pMtHdl );
	/* �T�[�{�R�}���h����t�B�[���h(SVCMD_CTRL)���� */
	stdCmdSvCmdCtrlProc( pCmdHdl, pMtHdl, FALSE );
	/* �T�[�{�R�}���h���o�͐M��(SVCMD_IO)���� */
	stdCmdSvCmdIoProc( pCmdHdl, pMtHdl );

	/* �a�j�w�߃I�t */
	pMtHdl->svCmd->CmdSeqBit |= BRK_RELEASE_BIT;
	
	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->cmdInfo.rsltBuf.stdData.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.stdData.mt.cmd;

	/* �R�}���h��͎��s���ʐݒ� */
	pCmdHdl->cmdInfo.ResultCmdAna = CMD_FUNC_NORMAL_END;

	return CMD_FUNC_NORMAL_END;
}


/****************************************************************************/
/* �T�v		:	�r�d�m�r�Q�n�m�R�}���h��͏���								*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END	����I��							*/
/*			:	= CMD_FUNC_CONTINUE		�p�����s��							*/
/*			:	= CMD_WARNG1			�R�}���h�x���P						*/
/****************************************************************************/
INT32 stdCmd_SENS_ON( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	INT32		ret;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;
	ret			= CMD_FUNC_NORMAL_END;/* <S14D> */

	/* �r�b�g�w�߉�͏��� */
	/* �R�}���h����(CMD_CTRL)���� */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, pMtHdl );
	/* �T�[�{�R�}���h����t�B�[���h(SVCMD_CTRL)���� */
	stdCmdSvCmdCtrlProc( pCmdHdl, pMtHdl, FALSE );
	/* �T�[�{�R�}���h���o�͐M��(SVCMD_IO)���� */
	stdCmdSvCmdIoProc( pCmdHdl, pMtHdl );

	/* ���s�����`�F�b�N */
	if( (pCmdHdl->cmdInfo.cmdRdy == FALSE)
		|| (pCmdHdl->cmdInfo.rsltBuf.stdData.alm.cmd != pCmdHdl->cmdInfo.cmdBuf.stdData.mem.cmd) )/* <S14D> */
	{
	/* �Z���T�I���w�߃Z�b�g */
		ret = MciSetSensOn( pMtHdl, TRUE );
		/* �R�}���h��͎��s���ʐݒ� */
		pCmdHdl->cmdInfo.ResultCmdAna = ret;/* <S14D> */
	}

	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->cmdInfo.rsltBuf.stdData.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.stdData.mt.cmd;

	return ret;
}


/****************************************************************************/
/* �T�v		:	�r�d�m�r�Q�n�e�e�R�}���h��͏���							*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END		����I��						*/
/*			:	= CMD_FUNC_CONTINUE			�p�����s��						*/
/****************************************************************************/
INT32 stdCmd_SENS_OFF( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	INT32		ret;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;
	ret			= CMD_FUNC_NORMAL_END;

	/* �r�b�g�w�߉�͏��� */
	/* �R�}���h����(CMD_CTRL)���� */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, pMtHdl );
	/* �T�[�{�R�}���h����t�B�[���h(SVCMD_CTRL)���� */
	stdCmdSvCmdCtrlProc( pCmdHdl, pMtHdl, FALSE );
	/* �T�[�{�R�}���h���o�͐M��(SVCMD_IO)���� */
	stdCmdSvCmdIoProc( pCmdHdl, pMtHdl );

	/* �Z���T�I�t�w�߃Z�b�g */
	if(pCmdHdl->cmdInfo.rsltBuf.stdData.mt.cmd != pCmdHdl->cmdInfo.cmdBuf.stdData.mt.cmd)
	{
		ret = MciSetSensOn( pMtHdl, FALSE );
		
		/* �R�}���h��͎��s���ʐݒ� */
		pCmdHdl->cmdInfo.ResultCmdAna = ret;
	}
	
	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->cmdInfo.rsltBuf.stdData.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.stdData.mt.cmd;

	return ret;
}


/****************************************************************************/
/* �T�v		:	�r�l�n�m�R�}���h��͏���									*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END	����I��							*/
/****************************************************************************/
INT32 stdCmd_SMON( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;

	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;

	/* �r�b�g�w�߉�͏��� */
	/* �R�}���h����(CMD_CTRL)���� */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, pMtHdl );
	/* �T�[�{�R�}���h����t�B�[���h(SVCMD_CTRL)���� */
	stdCmdSvCmdCtrlProc( pCmdHdl, pMtHdl, FALSE );
	/* �T�[�{�R�}���h���o�͐M��(SVCMD_IO)���� */
	stdCmdSvCmdIoProc( pCmdHdl, pMtHdl );

	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->cmdInfo.rsltBuf.stdData.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.stdData.mt.cmd;

	/* �R�}���h��͎��s���ʐݒ� */
	pCmdHdl->cmdInfo.ResultCmdAna = CMD_FUNC_NORMAL_END;

	return CMD_FUNC_NORMAL_END;
}


/****************************************************************************/
/* �T�v		:	�r�u�Q�n�m�R�}���h��͏���									*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END		����I��						*/
/*			:	= CMD_FUNC_CONTINUE			�p�����s��						*/
/*			:	= CMD_WARNG1				�R�}���h�x���P�i�`�D�X�T�`�j	*/
/****************************************************************************/
INT32 stdCmd_SV_ON( MLNET_HANDLE *hNetData )
{
	CMD_HNDL				*pCmdHdl;
	MT_HNDL					*pMtHdl;
	BOOL					reset;
	INT32					ret;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl			= &hNetData->cmd_hdl;
	pMtHdl			= &hNetData->mt_hdl;
//	ret				= CMD_FUNC_NORMAL_END;

	/* �r�b�g�w�߉�͏��� */
	/* �R�}���h����(CMD_CTRL)���� */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, pMtHdl );
	/* �T�[�{�R�}���h����t�B�[���h(SVCMD_CTRL)���� */
	stdCmdSvCmdCtrlProc( pCmdHdl, pMtHdl, FALSE );
	/* �T�[�{�R�}���h���o�͐M��(SVCMD_IO)���� */
	stdCmdSvCmdIoProc( pCmdHdl, pMtHdl );

	reset = (pCmdHdl->cmdInfo.rsltBuf.stdData.mt.cmd != pCmdHdl->cmdInfo.cmdBuf.stdData.mt.cmd);
	ret = MciSetServoOn( pMtHdl, TRUE, reset );

	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->cmdInfo.rsltBuf.stdData.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.stdData.mt.cmd;

	/* �R�}���h��͎��s���ʐݒ� */
	pCmdHdl->cmdInfo.ResultCmdAna = ret;

	return ret;
}


/****************************************************************************/
/* �T�v		:	�r�u�Q�n�e�e�R�}���h��͏���								*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END	����I��							*/
/*			:	= CMD_FUNC_CONTINUE		�p�����s��							*/
/****************************************************************************/
INT32 stdCmd_SV_OFF( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	INT32		ret;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;

	/* �r�b�g�w�߉�͏��� */
	/* �R�}���h����(CMD_CTRL)���� */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, pMtHdl );
	/* �T�[�{�R�}���h����t�B�[���h(SVCMD_CTRL)���� */
	stdCmdSvCmdCtrlProc( pCmdHdl, pMtHdl, FALSE );
	/* �T�[�{�R�}���h���o�͐M��(SVCMD_IO)���� */
	stdCmdSvCmdIoProc( pCmdHdl, pMtHdl );

	/* �T�[�{�I�t�w�߃Z�b�g */
	ret = MciSetServoOn( pMtHdl, FALSE, TRUE );

	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->cmdInfo.rsltBuf.stdData.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.stdData.mt.cmd;

	/* �R�}���h��͎��s���ʐݒ� */
	pCmdHdl->cmdInfo.ResultCmdAna = ret;

	return ret;
}


/****************************************************************************/
/* �T�v		:	�h�m�s�d�q�o�n�k�`�s�d�R�}���h��͏���				(34H)	*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	����I��	:	CMD_FUNC_NORMAL_END							*/
/*				�����ُ�	:	IPOLATE_CMD_WARNING1						*/
/*				�f�[�^�ُ�	:	IPOLATE_CMD_DATA_SET_WARNING2				*/
/****************************************************************************/
INT32 stdCmd_INTERPOLATE( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	MCI_CMD		cmd;
	INT32		result;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;

	/* �e�f�[�^�𒊏o */
	cmd.TPos		= pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data1;
	cmd.Velocity	= pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data2;
	cmd.Trq			= pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data3;
	cmd.TrqLimitP	= (ULONG)pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data5;

	/* �ʐM���[�j���O���̕␳���� */
	if( (hNetData->com_hdl.recvErrCnt == COM_RCV_WRN_STS)
		&& (pMtHdl->crtMtype == MTYPE_INTERPOL) )
	{/* �ʐM���[�j���O && ���݃��[�V����=INTERPOLATE */
		cmd.TPos += pMtHdl->outData.DposIn;
	}

	/* �h�m�s�d�q�o�n�k�`�s�d�R�}���h�`�o�h�֐����s */
	result = MciSetInterpolateCmd( pMtHdl, &cmd );

	/* �r�b�g�w�߉�͏��� */
	/* stdCmdSvCmdCtrlProc()�� Mtype, CtrlMode �𗘗p���邽�߁A�����ŏ��� */
	/* �R�}���h����(CMD_CTRL)���� */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, pMtHdl );
	/* �T�[�{�R�}���h����t�B�[���h(SVCMD_CTRL)���� */
	stdCmdSvCmdCtrlProc( pCmdHdl, pMtHdl, FALSE );
	/* �T�[�{�R�}���h���o�͐M��(SVCMD_IO)���� */
	stdCmdSvCmdIoProc( pCmdHdl, pMtHdl );

	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->cmdInfo.rsltBuf.stdData.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.stdData.mt.cmd;

	/* �R�}���h��͎��s���ʐݒ� */
	pCmdHdl->cmdInfo.ResultCmdAna = result;

	return result;
}


/****************************************************************************/
/* �T�v		:	�r�o�n�r�h�m�f�R�}���h��͏���						(C0H)	*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	����I��	:	CMD_FUNC_NORMAL_END							*/
/*				�����ُ�	:	POSING_CMD_WARNING1							*/
/*				�f�[�^�ُ�	:	POSING_CMD_DATA_SET_WARNING2				*/
/****************************************************************************/
INT32 stdCmd_SPOSING( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	MCI_CMD		cmd;
	INT32		result;
	BOOL		SratioBit;
	ULONG		svcmd_io;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;

	/* �e�f�[�^�𒊏o */
	cmd.TPos	= pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data1;
	cmd.TSpeed	= pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data2;
	cmd.AccRate	= (ULONG)pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data3;
	cmd.DecRate	= (ULONG)pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data4;
	cmd.TrqLimitP= (ULONG)pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data5;
	svcmd_io = pCmdHdl->cmdInfo.cmdBuf.stdData.mt.svcmd_io;	

	/* ���s�����`�F�b�N *//* <S06D> */
	if( ((ULONG)cmd.TSpeed <= 0x7FFFFFFF)
	    && ((cmd.AccRate <= 0x7FFFFFFF)||(cmd.AccRate == 0xFFFFFFFF))
	    && ((cmd.DecRate <= 0x7FFFFFFF)||(cmd.DecRate == 0xFFFFFFFF)) )
	{
		/* S����Ԕ䗦(svcmd_io�Fbit24��1:50%�A0:25%)	*/
		SratioBit	= ((svcmd_io >> pMtHdl->mtPrm.SvCmdIoOutMap[VENDOR_IOBIT_SRATIO].bitNo )
									 & pMtHdl->mtPrm.SvCmdIoOutMap[VENDOR_IOBIT_SRATIO].bitsMsk);
									 
		/* S�����Ԕ䗦������ */
		if( hNetData->reg_hdl.Prm->posing_mode == 0 )
		{/* Pn8FF=0�̂Ƃ� */
			if( SratioBit == 1 )
			{
				cmd.Sratio = S_RATIO1;/* S�����������Ԕ䗦1(%) */
			}
			else
			{
				cmd.Sratio = S_RATIO2;/* S�����������Ԕ䗦2(%) */
			}
		}
		else
		{/* Pn8FF!=0�̂Ƃ� */
			cmd.Sratio = hNetData->reg_hdl.Prm->posing_mode;
		}
		/*------------------*/
		/* �������^�C�v�ݒ� *//* <S004> */
		/*------------------*/
		if( (cmd.AccRate == 0) && (cmd.DecRate == 0) )
		{/* �������x���Ƃ��Ƀ[���̏ꍇ�͋@��p�����[�^�̉������x���g�� */
			cmd.RateType  = RATE_PRM;					/* �������x�w�߃^�C�v�Z�b�g */
		}
		else
		{/* ��L�ȊO�͒ʐM�Ŏw�߂��ꂽ�������x���g�� */
			cmd.RateType  = RATE_CMD;					/* �������x�w�߃^�C�v�Z�b�g */
		}
	
		/* SPOSING�R�}���hAPI�֐����s */
		result = MciSetSposingCmd( pMtHdl, &cmd );

	}
	else
	{
		result = DATA_SET_WARNG2;
	}
	
	/* �r�b�g�w�߉�͏��� */
	/* stdCmdSvCmdCtrlProc()�� Mtype, CtrlMode �𗘗p���邽�߁A�����ŏ��� */
	/* �R�}���h����(CMD_CTRL)���� */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, pMtHdl );
	/* �T�[�{�R�}���h����t�B�[���h(SVCMD_CTRL)���� */
	stdCmdSvCmdCtrlProc( pCmdHdl, pMtHdl, TRUE );
	/* �T�[�{�R�}���h���o�͐M��(SVCMD_IO)���� */
	stdCmdSvCmdIoProc( pCmdHdl, pMtHdl );

	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->cmdInfo.rsltBuf.stdData.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.stdData.mt.cmd;

	/* �R�}���h��͎��s���ʐݒ� */
	pCmdHdl->cmdInfo.ResultCmdAna = result;

	return result;
}


/****************************************************************************/
/* �T�v		:	�o�n�r�h�m�f�R�}���h��͏���						(35H)	*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	����I��	:	CMD_FUNC_NORMAL_END							*/
/*				�����ُ�	:	POSING_CMD_WARNING1							*/
/*				�f�[�^�ُ�	:	POSING_CMD_DATA_SET_WARNING2				*/
/****************************************************************************/
INT32 stdCmd_POSING( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	MCI_CMD		cmd;
	INT32		result;

	/* SPOSING���[�h(Pn8FF�Ő؂芷��:0��POSING�A0��SPOSING�F) */
//	if( hNetData->reg_hdl.Prm->posing_mode == SPOSING_SEL )
	if( hNetData->reg_hdl.Prm->posing_mode > 0 )
	{
		return stdCmd_SPOSING( hNetData );
	}

	/* ���[�J���ϐ��̏����� */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;

	/* �e�f�[�^�𒊏o */
	cmd.TPos	= pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data1;
	cmd.TSpeed	= pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data2;
	cmd.AccRate	= (ULONG)pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data3;
	cmd.DecRate	= (ULONG)pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data4;
	cmd.TrqLimitP= (ULONG)pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data5;

	/* ���s�����`�F�b�N *//* <S06D> */
	if( ((ULONG)cmd.TSpeed <= 0x7FFFFFFF)
	    && ((cmd.AccRate <= 0x7FFFFFFF)||(cmd.AccRate == 0xFFFFFFFF))
	    && ((cmd.DecRate <= 0x7FFFFFFF)||(cmd.DecRate == 0xFFFFFFFF)) )
	{
		/*------------------*/
		/* �������^�C�v�ݒ� */
		/*------------------*/
		if( (cmd.AccRate == 0) && (cmd.DecRate == 0) )
		{/* �������x���Ƃ��Ƀ[���̏ꍇ�͋@��p�����[�^�̉������x���g�� */
			cmd.RateType  = RATE_PRM;					/* �������x�w�߃^�C�v�Z�b�g */
		}
		else
		{/* ��L�ȊO�͒ʐM�Ŏw�߂��ꂽ�������x���g�� */
			cmd.RateType  = RATE_CMD;					/* �������x�w�߃^�C�v�Z�b�g */
		}
	
		/* �o�n�r�h�m�f�R�}���h�`�o�h�֐����s */
		result = MciSetPosingCmd( pMtHdl, &cmd );
	}
	else
	{
		result = DATA_SET_WARNG2;
	}

	/* �r�b�g�w�߉�͏��� */
	/* stdCmdSvCmdCtrlProc()�� Mtype, CtrlMode �𗘗p���邽�߁A�����ŏ��� */
	/* �R�}���h����(CMD_CTRL)���� */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, pMtHdl );
	/* �T�[�{�R�}���h����t�B�[���h(SVCMD_CTRL)���� */
	stdCmdSvCmdCtrlProc( pCmdHdl, pMtHdl, TRUE );
	/* �T�[�{�R�}���h���o�͐M��(SVCMD_IO)���� */
	stdCmdSvCmdIoProc( pCmdHdl, pMtHdl );

	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->cmdInfo.rsltBuf.stdData.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.stdData.mt.cmd;

	/* �R�}���h��͎��s���ʐݒ� */
	pCmdHdl->cmdInfo.ResultCmdAna = result;

	return result;
}


/****************************************************************************/
/* �T�v		:	�e�d�d�c�R�}���h��͏���							(36H)	*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	����I��	:	CMD_FUNC_NORMAL_END							*/
/*				�����ُ�	:	FEED_CMD_WARNING1							*/
/*				�f�[�^�ُ�	:	FEED_CMD_DATA_SET_WARNING2					*/
/****************************************************************************/
INT32 stdCmd_FEED( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	MCI_CMD		cmd;
	INT32		result;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;

	cmd.TSpeed	= pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data2;
	cmd.AccRate	= (ULONG)pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data3;
	cmd.DecRate	= (ULONG)pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data4;
	cmd.TrqLimitP= (ULONG)pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data5;

	/* ���s�����`�F�b�N *//* <S06D> *//* <S07B>:TSPD�̃`�F�b�N�����{���Ȃ��悤�ɕύX */
	if( ((cmd.AccRate <= 0x7FFFFFFF)||(cmd.AccRate == 0xFFFFFFFF))
	    && ((cmd.DecRate <= 0x7FFFFFFF)||(cmd.DecRate == 0xFFFFFFFF)) )
	{
		/*------------------*/
		/* �������^�C�v�ݒ� *//* <S004> */
		/*------------------*/
		if((cmd.AccRate == 0) && (cmd.DecRate == 0))
		{/* �������x���Ƃ��Ƀ[���̏ꍇ�͋@��p�����[�^�̉������x���g�� */
			cmd.RateType  = RATE_PRM;					/* �������x�w�߃^�C�v�Z�b�g */
		}
		else
		{/* ��L�ȊO�͒ʐM�Ŏw�߂��ꂽ�������x���g�� */
			cmd.RateType  = RATE_CMD;					/* �������x�w�߃^�C�v�Z�b�g */
		}
		
		/* �e�d�d�c�R�}���h�`�o�h�֐����s */
		result = MciSetFeedCmd( pMtHdl, &cmd );

	}
	else
	{
		result = DATA_SET_WARNG2;
	}
	/* �r�b�g�w�߉�͏��� */
	/* stdCmdSvCmdCtrlProc()�� Mtype, CtrlMode �𗘗p���邽�߁A�����ŏ��� */
	/* �R�}���h����(CMD_CTRL)���� */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, pMtHdl );
	/* �T�[�{�R�}���h����t�B�[���h(SVCMD_CTRL)���� */
	stdCmdSvCmdCtrlProc( pCmdHdl, pMtHdl, TRUE );
	/* �T�[�{�R�}���h���o�͐M��(SVCMD_IO)���� */
	stdCmdSvCmdIoProc( pCmdHdl, pMtHdl );

	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->cmdInfo.rsltBuf.stdData.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.stdData.mt.cmd;

	/* �R�}���h��͎��s���ʐݒ� */
	pCmdHdl->cmdInfo.ResultCmdAna = result;

	return result;
}


/****************************************************************************/
/* �T�v		:	�d�w�Q�e�d�d�c�R�}���h��͏���						(37H)	*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	����I��	:	CMD_FUNC_NORMAL_END							*/
/*				�����ُ�	:	EX_FEED_CMD_WARNING1						*/
/*				�f�[�^�ُ�	:	EX_FEED_CMD_DATA_SET_WARNING2				*/
/*				ׯ������ُ�	:	LATCH_DATA_SET_WARNG5						*/
/****************************************************************************/
INT32 stdCmd_EX_FEED( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	MCI_CMD		cmd;
	INT32		result, wk1, wk2;
	LONG		cmd_ctrl;
	ULONG		svcmd_ctrl;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;
	svcmd_ctrl	= pCmdHdl->cmdInfo.cmdBuf.stdData.mt.svcmd_ctrl;
	cmd_ctrl	= pCmdHdl->cmdInfo.cmdBuf.stdData.nop.cmd_ctrl;

		/* �e�f�[�^�𒊏o */
	cmd.TSpeed	= pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data2;
	cmd.AccRate	= (ULONG)pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data3;
	cmd.DecRate	= (ULONG)pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data4;
	cmd.TrqLimitP= (ULONG)pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data5;

	/* ���s�����`�F�b�N *//* <S06D> *//* <S07B>:TSPD�̃`�F�b�N�����{���Ȃ��悤�ɕύX */
	if( ((cmd.AccRate <= 0x7FFFFFFF)||(cmd.AccRate == 0xFFFFFFFF))
	    && ((cmd.DecRate <= 0x7FFFFFFF)||(cmd.DecRate == 0xFFFFFFFF)) )
	{
		cmd.MtLatchReq  = (svcmd_ctrl >> SVCMD_CTRL_LT_REQ1_MAP) & 1;	/* ���b�`�v���Z�b�g */
		cmd.LatchSgnl   = (svcmd_ctrl >> SVCMD_CTRL_LT_SEL1_MAP) & SVCMD_CTRL_LT_SEL1_MASK;	/* LT_SGN�Z�b�g */
	
		wk1 = ( (pCmdHdl->cmdInfo.lstCmdCtrl & (CMD_CTRL_CMD_ID_MASK << CMD_CTRL_CMD_ID_MAP) ) 
								!= (cmd_ctrl & (CMD_CTRL_CMD_ID_MASK << CMD_CTRL_CMD_ID_MAP)) );
		wk2 = ( pCmdHdl->cmdInfo.rsltBuf.stdData.mt.cmd != pCmdHdl->cmdInfo.cmdBuf.stdData.mt.cmd );
		cmd.RstReq    = ( wk1 || wk2 );							/* ���[�V�������Z�b�g�v���Z�b�g */

		/*------------------*/
		/* �������^�C�v�ݒ� *//* <S004> */
		/*------------------*/
		if((cmd.AccRate == 0) && (cmd.DecRate == 0))
		{/* �������x���Ƃ��Ƀ[���̏ꍇ�͋@��p�����[�^�̉������x���g�� */
			cmd.RateType  = RATE_PRM;					/* �������x�w�߃^�C�v�Z�b�g */
		}
		else
		{/* ��L�ȊO�͒ʐM�Ŏw�߂��ꂽ�������x���g�� */
			cmd.RateType  = RATE_CMD;					/* �������x�w�߃^�C�v�Z�b�g */
		}
		
		/* �d�w�Q�e�d�d�c�R�}���h�`�o�h�֐����s */
		result = MciSetExFeedCmd( pMtHdl, &cmd );
	}
	else
	{
		result = DATA_SET_WARNG2;
	}

	/* �r�b�g�w�߉�͏��� */
	/* lstCmdCtrl���w�߃f�[�^�쐬�����ŗ��p���邽�߁AstdCmdCmdCtrlProc()�������ŏ��� */
	/* stdCmdSvCmdCtrlProc()�� Mtype, CtrlMode �𗘗p���邽�߁A�����ŏ��� */
	/* �R�}���h����(CMD_CTRL)���� */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, pMtHdl );
	/* �T�[�{�R�}���h����t�B�[���h(SVCMD_CTRL)���� */
	stdCmdSvCmdCtrlProc( pCmdHdl, pMtHdl, TRUE );
	/* �T�[�{�R�}���h���o�͐M��(SVCMD_IO)���� */
	stdCmdSvCmdIoProc( pCmdHdl, pMtHdl );

	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->cmdInfo.rsltBuf.stdData.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.stdData.mt.cmd;

	/* �R�}���h��͎��s���ʐݒ� */
	pCmdHdl->cmdInfo.ResultCmdAna = result;

	return result;
}


/****************************************************************************/
/* �T�v		:	�d�w�Q�o�n�r�h�m�f�R�}���h��͏���					(39H)	*/
/****************************************************************************/
/* ����		:	CMD_HNDL *hdl												*/
/* �߂�l	:	����I��	:	CMD_FUNC_NORMAL_END							*/
/*				�����ُ�	:	EX_POSING_CMD_WARNING1						*/
/*				�f�[�^�ُ�	:	EX_POSING_CMD_DATA_SET_WARNING2				*/
/*				ׯ������ُ�	:	LATCH_DATA_SET_WARNG5						*/
/****************************************************************************/
INT32 stdCmd_EX_POSING( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	MCI_CMD		cmd;
	ULONG		svcmd_ctrl;
	ULONG		cmd_ctrl;
	INT32		result, wk1, wk2;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;

	/* �e�f�[�^�𒊏o */
	svcmd_ctrl	= pCmdHdl->cmdInfo.cmdBuf.stdData.mt.svcmd_ctrl;
	cmd_ctrl	= pCmdHdl->cmdInfo.cmdBuf.stdData.nop.cmd_ctrl;
	cmd.TPos	= pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data1;
	cmd.TSpeed	= pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data2;
	cmd.AccRate	= (ULONG)pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data3;
	cmd.DecRate	= (ULONG)pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data4;
	cmd.TrqLimitP= (ULONG)pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data5;

	/* ���s�����`�F�b�N *//* <S06D> */
	if( ((ULONG)cmd.TSpeed <= 0x7FFFFFFF)
	    && ((cmd.AccRate <= 0x7FFFFFFF)||(cmd.AccRate == 0xFFFFFFFF))
	    && ((cmd.DecRate <= 0x7FFFFFFF)||(cmd.DecRate == 0xFFFFFFFF)) )
	{
		cmd.MtLatchReq  = (svcmd_ctrl >> SVCMD_CTRL_LT_REQ1_MAP) & 1;	/* ���b�`�v���Z�b�g */
		cmd.LatchSgnl   = (svcmd_ctrl >> SVCMD_CTRL_LT_SEL1_MAP) & SVCMD_CTRL_LT_SEL1_MASK;	/* LT_SGN�Z�b�g */
	
		wk1 = ( (pCmdHdl->cmdInfo.lstCmdCtrl & (CMD_CTRL_CMD_ID_MASK << CMD_CTRL_CMD_ID_MAP) ) 
								!= (cmd_ctrl & (CMD_CTRL_CMD_ID_MASK << CMD_CTRL_CMD_ID_MAP)) );
		wk2 = ( pCmdHdl->cmdInfo.rsltBuf.stdData.mt.cmd != pCmdHdl->cmdInfo.cmdBuf.stdData.mt.cmd );
		cmd.RstReq = ( wk1 || wk2 );							/* ���[�V�������Z�b�g�v���Z�b�g */
	
		/*------------------*/
		/* �������^�C�v�ݒ� *//* <S004> */
		/*------------------*/
		if((cmd.AccRate == 0) && (cmd.DecRate == 0))
		{/* �������x���Ƃ��Ƀ[���̏ꍇ�͋@��p�����[�^�̉������x���g�� */
			cmd.RateType  = RATE_PRM;					/* �������x�w�߃^�C�v�Z�b�g */
		}
		else
		{/* ��L�ȊO�͒ʐM�Ŏw�߂��ꂽ�������x���g�� */
			cmd.RateType  = RATE_CMD;					/* �������x�w�߃^�C�v�Z�b�g */
		}
			
		/* �d�w�Q�o�n�r�h�m�f�R�}���h�`�o�h�֐����s */
		result = MciSetExPosingCmd( pMtHdl, &cmd );
	}
	else
	{
		result = DATA_SET_WARNG2;
	}

	/* �r�b�g�w�߉�͏��� */
	/* lstCmdCtrl���w�߃f�[�^�쐬�����ŗ��p���邽�߁AstdCmdCmdCtrlProc()�������ŏ��� */
	/* stdCmdSvCmdCtrlProc()�� Mtype, CtrlMode �𗘗p���邽�߁A�����ŏ��� */
	/* �R�}���h����(CMD_CTRL)���� */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, pMtHdl );
	/* �T�[�{�R�}���h����t�B�[���h(SVCMD_CTRL)���� */
	stdCmdSvCmdCtrlProc( pCmdHdl, pMtHdl, TRUE );
	/* �T�[�{�R�}���h���o�͐M��(SVCMD_IO)���� */
	stdCmdSvCmdIoProc( pCmdHdl, pMtHdl );

	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->cmdInfo.rsltBuf.stdData.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.stdData.mt.cmd;

	/* �R�}���h��͎��s���ʐݒ� */
	pCmdHdl->cmdInfo.ResultCmdAna = result;

	return result;
}


/****************************************************************************/
/* �T�v		:	�y�q�d�s�R�}���h��͏���							(3AH)	*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	����I��	:	CMD_FUNC_NORMAL_END							*/
/*				�����ُ�	:	ZRET_CMD_WARNING1							*/
/*				�ް��ُ�	:	ZRET_CMD_DATA_SET_WARNING2					*/
/*				ׯ������ُ�	:	LATCH_DATA_SET_WARNG5						*/
/****************************************************************************/
INT32 stdCmd_ZRET( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	MCI_CMD		cmd;
	INT32		result, wk1, wk2;
	LONG		cmd_ctrl;
	UCHAR		type;
	ULONG		svcmd_ctrl, mode;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;

	/* �e�f�[�^�𒊏o */
	svcmd_ctrl	= pCmdHdl->cmdInfo.cmdBuf.stdData.mt.svcmd_ctrl;
	cmd_ctrl	= pCmdHdl->cmdInfo.cmdBuf.stdData.nop.cmd_ctrl;
	mode		= pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data1;
	type		= (UCHAR)( (mode >> ZRET_MODE_TYPE_MAP) & ZRET_MODE_TYPE_MASK );/* ���_���A�^�C�v */

	cmd.MtLatchReq  = (svcmd_ctrl >> SVCMD_CTRL_LT_REQ1_MAP) & 1;	/* ���b�`�v���Z�b�g */
	cmd.LatchSgnl   = (svcmd_ctrl >> SVCMD_CTRL_LT_SEL1_MAP) & SVCMD_CTRL_LT_SEL1_MASK;	/* LT_SGN�Z�b�g */
	cmd.TSpeed	= pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data2;
	cmd.AccRate	= (ULONG)pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data3;
	cmd.DecRate	= (ULONG)pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data4;
	cmd.TrqLimitP= (ULONG)pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data5;

	/* ���s�����`�F�b�N *//* <S06D> */
	if( ((ULONG)cmd.TSpeed <= 0x7FFFFFFF)
	    && ((cmd.AccRate <= 0x7FFFFFFF)||(cmd.AccRate == 0xFFFFFFFF))
	    && ((cmd.DecRate <= 0x7FFFFFFF)||(cmd.DecRate == 0xFFFFFFFF)) )
	{
		wk1 = ( (pCmdHdl->cmdInfo.lstCmdCtrl & (CMD_CTRL_CMD_ID_MASK << CMD_CTRL_CMD_ID_MAP)) 
								!= (cmd_ctrl & (CMD_CTRL_CMD_ID_MASK << CMD_CTRL_CMD_ID_MAP)) );
		wk2 = ( pCmdHdl->cmdInfo.rsltBuf.stdData.mt.cmd != pCmdHdl->cmdInfo.cmdBuf.stdData.mt.cmd );
		cmd.RstReq    = ( wk1 || wk2 );							/* ���[�V�������Z�b�g�v���Z�b�g */
		cmd.HomeDir   = (BOOL)( (mode >> ZRET_MODE_HOME_DIR_MAP) & ZRET_MODE_HOME_DIR_MASK );			/* HOME DIR �Z�b�g */
	
		/*------------------*/
		/* �������^�C�v�ݒ� *//* <S004> */
		/*------------------*/
		if((cmd.AccRate == 0) && (cmd.DecRate == 0))
		{/* �������x���Ƃ��Ƀ[���̏ꍇ�͋@��p�����[�^�̉������x���g�� */
			cmd.RateType  = RATE_PRM;					/* �������x�w�߃^�C�v�Z�b�g */
		}
		else
		{/* ��L�ȊO�͒ʐM�Ŏw�߂��ꂽ�������x���g�� */
			cmd.RateType  = RATE_CMD;					/* �������x�w�߃^�C�v�Z�b�g */
		}
		/*--------------------*/
		/* ���_���A�^�C�v�ݒ� */
		/*--------------------*/
		if( cmd.RstReq )
		{ /* �R�}���h�؂�ւ�莞�Ɍ��_���A�^�C�v���擾 */
		  /* ZRET���s���͌��_���A���[�h��؂�ւ��Ȃ� */
			pCmdHdl->cmdInfo.homingType = type;	/* ���_���A�^�C�v���擾 */
		}
	
		/*--------------------*/
		/* ���_���AAPI���s    */
		/*--------------------*/
		if( pCmdHdl->cmdInfo.homingType == ZRET_MODE_TYPE_LATCH_MODE )
		{
			/* �y�q�d�s�R�}���h�`�o�h�֐��iDEC�Ȃ����[�h�j���s */
			result = MciSetHomingCmd( pMtHdl, &cmd );
		}
		else
		{
			/* �y�q�d�s�R�}���h�`�o�h�֐��iDEC���胂�[�h�j���s */
			result = MciSetHomingCmdWithDec( pMtHdl, &cmd );
		}
	}
	else
	{
		result = DATA_SET_WARNG2;
	}

	/* �r�b�g�w�߉�͏��� */
	/* lstCmdCtrl���w�߃f�[�^�쐬�����ŗ��p���邽�߁AstdCmdCmdCtrlProc()�������ŏ��� */
	/* stdCmdSvCmdCtrlProc()�� Mtype, CtrlMode �𗘗p���邽�߁A�����ŏ��� */
	/* �R�}���h����(CMD_CTRL)���� */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, pMtHdl );
	/* �T�[�{�R�}���h����t�B�[���h(SVCMD_CTRL)���� */
	stdCmdSvCmdCtrlProc( pCmdHdl, pMtHdl, TRUE );
	/* �T�[�{�R�}���h���o�͐M��(SVCMD_IO)���� */
	stdCmdSvCmdIoProc( pCmdHdl, pMtHdl );

	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->cmdInfo.rsltBuf.stdData.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.stdData.mt.cmd;

	/* �R�}���h��͎��s���ʐݒ� */
	pCmdHdl->cmdInfo.ResultCmdAna = result;

	return result;
}


/****************************************************************************/
/* �T�v		:	�u�d�k�b�s�q�k�R�}���h��͏���						(3CH)	*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	����I��	:	CMD_FUNC_NORMAL_END							*/
/*				�����ُ�	:	VELCTRL_CMD_WARNING1						*/
/*				�f�[�^�ُ�	:	VELCTRL_CMD_DATA_SET_WARNING2				*/
/****************************************************************************/
INT32 stdCmd_VELCTRL( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	MCI_CMD		cmd;
	INT32		result;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;

	/* �e�f�[�^�𒊏o */
	cmd.Trq		= pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data1;
	cmd.Velocity= pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data2;
	cmd.AccRate	= (ULONG)pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data3;
	cmd.DecRate	= (ULONG)pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data4;
	cmd.TrqLimitP= (ULONG)pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data5;
	cmd.RateType  = RATE_CMD;										/* �������x�w�߃^�C�v�Z�b�g *//* <S004> */

	/* ���s�����`�F�b�N *//* <S06D> */
	if( ((cmd.AccRate <= 0x7FFFFFFF)||(cmd.AccRate == 0xFFFFFFFF))
	    && ((cmd.DecRate <= 0x7FFFFFFF)||(cmd.DecRate == 0xFFFFFFFF)) )
	{
		/* �u�d�k�b�s�q�k�R�}���h�`�o�h�֐����s */
		result = MciSetVelCmdWithAccr( pMtHdl, &cmd );
	}
	else
	{
		result = DATA_SET_WARNG2;
	}

	/* �r�b�g�w�߉�͏��� */
	/* stdCmdSvCmdCtrlProc()�� Mtype, CtrlMode �𗘗p���邽�߁A�����ŏ��� */
	/* �R�}���h����(CMD_CTRL)���� */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, pMtHdl );
	/* �T�[�{�R�}���h����t�B�[���h(SVCMD_CTRL)���� */
	stdCmdSvCmdCtrlProc( pCmdHdl, pMtHdl, TRUE );
	/* �T�[�{�R�}���h���o�͐M��(SVCMD_IO)���� */
	stdCmdSvCmdIoProc( pCmdHdl, pMtHdl );	

	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->cmdInfo.rsltBuf.stdData.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.stdData.mt.cmd;

	/* �R�}���h��͎��s���ʐݒ� */
	pCmdHdl->cmdInfo.ResultCmdAna = result;

	return result;
}


/****************************************************************************/
/* �T�v		:	�s�q�p�b�s�q�k�R�}���h��͏���						(3DH)	*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	����I��	:	CMD_FUNC_NORMAL_END							*/
/*				�����ُ�	:	TRQCTRL_CMD_WARNING1						*/
/*				�ް��ُ�	:	TRQCTRL_CMD_DATA_SET_WARNING2				*/
/****************************************************************************/
INT32 stdCmd_TRQCTRL( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	MCI_CMD		cmd;
	INT32		result;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;

	/* �e�f�[�^�𒊏o */
	cmd.Velocity= pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data1;/* TQREF�Z�b�g		*/
	cmd.Trq		= pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data2;/* VLIM�Z�b�g 		*/

	/* �s�q�p�b�s�q�k�R�}���h�`�o�h�֐����s */
	result = MciSetTrqCmd( pMtHdl, &cmd );

	/* �r�b�g�w�߉�͏��� */
	/* stdCmdSvCmdCtrlProc()�� Mtype, CtrlMode �𗘗p���邽�߁A�����ŏ��� */
	/* �R�}���h����(CMD_CTRL)���� */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, pMtHdl );
	/* �T�[�{�R�}���h����t�B�[���h(SVCMD_CTRL)���� */
	stdCmdSvCmdCtrlProc( pCmdHdl, pMtHdl, FALSE );
	/* �T�[�{�R�}���h���o�͐M��(SVCMD_IO)���� */
	stdCmdSvCmdIoProc( pCmdHdl, pMtHdl );

	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->cmdInfo.rsltBuf.stdData.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.stdData.mt.cmd;

	/* �R�}���h��͎��s���ʐݒ� */
	pCmdHdl->cmdInfo.ResultCmdAna = result;

	return result;
}


/****************************************************************************/
/* �T�v		:	�r�u�o�q�l�Q�q�c�R�}���h��͏���							*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END	����I��							*/
/*			:	= CMD_FUNC_CONTINUE		�p�����s��							*/
/*			:	= DATA_SET_WARNG1		�f�[�^�ݒ�x���P	�i�`�D�X�S�`�j	*/
/*			:	= DATA_SET_WARNG2		�f�[�^�ݒ�x���Q	�i�`�D�X�S�a�j	*/
/*			:	= DATA_SET_WARNG4		�f�[�^�ݒ�x���S	�i�`�D�X�S�c�j	*/
/*			:	= CMD_WARNG1			�R�}���h�x���P		�i�`�D�X�T�`�j	*/
/****************************************************************************/
INT32 stdCmd_SVPRM_RD( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	UCHAR		cmd;
	ULONG		no,chekNo;/* <S06D>�FchekNo�ǉ� */
	INT32		ret;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;

	/* �r�b�g�w�߉�͏��� */
	/* �R�}���h����(CMD_CTRL)���� */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, pMtHdl );
	/* �T�[�{�R�}���h����t�B�[���h(SVCMD_CTRL)���� */
	stdCmdSvCmdCtrlProc( pCmdHdl, pMtHdl, FALSE );
	/* �T�[�{�R�}���h���o�͐M��(SVCMD_IO)���� */
	stdCmdSvCmdIoProc( pCmdHdl, pMtHdl );

	ret	 = CMD_FUNC_NORMAL_END;
	cmd	 = pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.cmd;	/* �R�}���h�R�[�h */

	/* ���s�����`�F�b�N */
	if( (pCmdHdl->cmdInfo.cmdRdy != FALSE) 
		&& ((pCmdHdl->cmdInfo.rsltBuf.stdData.svprm.cmd != cmd) 
		||  (pCmdHdl->cmdInfo.rsltBuf.dword[3] != pCmdHdl->cmdInfo.cmdBuf.dword[3])) ) 
	{

		switch(pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.mode)
		{
		case 0x00:/* ���ʃp�����[�^(RAM) */
//			no = (pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.no<<2);							/* <S06D>�FDell */
//			no = no + VMEM_VEN_MIN + ((COMM_PRM_OFFSET+RAM_OFFSET)<<1); 				/* <S06D>�FDell */
			chekNo = (pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.no<<1) + COMM_PRM_OFFSET;	/* <S06D>�FAdd */
			no = VMEM_VEN_MIN + ((chekNo + RAM_OFFSET)<<1); 							/* <S06D>�FAdd */
			break;
		case 0x10:/* �@��p�����[�^(RAM) */
//			no = (pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.no<<1);							/* <S06D>�FDell */
//			no = no + VMEM_VEN_MIN + ((RAM_OFFSET)<<1);        							/* <S06D>�FDell */
			chekNo = pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.no;							/* <S06D>�FAdd */
			no = VMEM_VEN_MIN + ((chekNo + RAM_OFFSET)<<1); 							/* <S06D>�FAdd */
			break;
		default:
			chekNo = 0;/* <S14D> */
			ret = DATA_SET_WARNG2;
			break;
		}
		if(pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.size == 0)/* <S16C> */
		{
			ret = DATA_SET_WARNG4;			/* A.94D �f�[�^�ݒ�x���S */
		}

		/* �p�����[�^�ԍ��A�T�C�Y�`�F�b�N *//* <S06D>�FAdd */
		if(chekNo >= 0x1000)
		{
			ret = DATA_SET_WARNG1;
		}

		if( ret == CMD_FUNC_NORMAL_END )
		{
			/* ���W�X�^�A�N�Z�X�p�f�[�^�ݒ� */
			pCmdHdl->cmdInfo.RegData.AccessSize = pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.size;
			pCmdHdl->cmdInfo.RegData.Length = 1;
			pCmdHdl->cmdInfo.RegData.Number = no;
			pCmdHdl->cmdInfo.RegData.EepWrite = FALSE;
			pCmdHdl->cmdInfo.RegData.pBuf = pCmdHdl->cmdInfo.rsltBuf.stdData.svprm.ldata;
			pCmdHdl->cmdInfo.RegData.BufferSize = sizeof(pCmdHdl->cmdInfo.rsltBuf.stdData.svprm.ldata);

			/* ���W�X�^�R�}���h�����v�����sAPI	*/
			ret = RegRequestCommand(&hNetData->reg_hdl, REG_CMD_PRMRD, &pCmdHdl->cmdInfo.RegData);
		}
		/* ��͌��ʂ��o�b�t�@�փR�s�[ */
		pCmdHdl->cmdInfo.rsltBuf.stdData.svprm.cmd  = cmd;
		pCmdHdl->cmdInfo.rsltBuf.dword[3] = pCmdHdl->cmdInfo.cmdBuf.dword[3];

		/* �R�}���h��͎��s���ʐݒ� */
		pCmdHdl->cmdInfo.ResultCmdAna = ret;
	}
	else
	{
		if(pCmdHdl->cmdInfo.cmdRdy == FALSE)
		{
			ret = RegCheckCommandState(&hNetData->reg_hdl);
			pCmdHdl->cmdInfo.ResultCmdAna = ret;
		}
	}

	return ret;
}


/****************************************************************************/
/* �T�v		:	�r�u�o�q�l�Q�v�q�R�}���h��͏���							*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END		����I��						*/
/*			:	= CMD_FUNC_CONTINUE			�p�����s��						*/
/*			:	= DATA_SET_WARNG1		�f�[�^�ݒ�x���P�i�`�D�X�S�`�j		*/
/*			:	= DATA_SET_WARNG2		�f�[�^�ݒ�x���Q�i�`�D�X�S�a�j		*/
/*			:	= DATA_SET_WARNG3		�f�[�^�ݒ�x���R�i�`�D�X�S�b�j		*/
/*			:	= DATA_SET_WARNG4		�f�[�^�ݒ�x���S�i�`�D�X�S�c�j		*/
/*			:	= CMD_WARNG1			�R�}���h�x���P	�i�`�D�X�T�`�j		*/
/****************************************************************************/
INT32 stdCmd_SVPRM_WR( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	ULONG		no,chekNo;/* <S06D>�FchekNo�ǉ� */
	INT32		ret;
	UCHAR		cmd;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;

	/* �r�b�g�w�߉�͏��� */
	/* �R�}���h����(CMD_CTRL)���� */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, pMtHdl );
	/* �T�[�{�R�}���h����t�B�[���h(SVCMD_CTRL)���� */
	stdCmdSvCmdCtrlProc( pCmdHdl, pMtHdl, FALSE );
	/* �T�[�{�R�}���h���o�͐M��(SVCMD_IO)���� */
	stdCmdSvCmdIoProc( pCmdHdl, pMtHdl );

	ret	 = CMD_FUNC_NORMAL_END;
	cmd	= pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.cmd;

	/* ���s�����`�F�b�N */
	if( (pCmdHdl->cmdInfo.cmdRdy != FALSE)
		&& ((pCmdHdl->cmdInfo.rsltBuf.stdData.svprm.cmd != cmd) 
		||  (pCmdHdl->cmdInfo.rsltBuf.dword[3] != pCmdHdl->cmdInfo.cmdBuf.dword[3]) 
		||  (pCmdHdl->cmdInfo.rsltBuf.stdData.svprm.ldata[0] != pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.ldata[0])
		||  (pCmdHdl->cmdInfo.rsltBuf.stdData.svprm.ldata[1] != pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.ldata[1])
		||  (pCmdHdl->cmdInfo.rsltBuf.stdData.svprm.ldata[2] != pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.ldata[2])
		||  (pCmdHdl->cmdInfo.rsltBuf.stdData.svprm.ldata[3] != pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.ldata[3])) )
	{
		switch(pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.mode)
		{
		case 0x00:/* ���ʃp�����[�^(RAM) */
//			no = (pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.no<<2);							/* <S06D>�FDell */
//			no = no + VMEM_VEN_MIN + ((COMM_PRM_OFFSET+RAM_OFFSET)<<1); 				/* <S06D>�FDell */
			chekNo = (pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.no<<1) + COMM_PRM_OFFSET;	/* <S06D>�FAdd */
			no = VMEM_VEN_MIN + ((chekNo + RAM_OFFSET)<<1); 							/* <S06D>�FAdd */
			break;
		case 0x01:/* ���ʃp�����[�^(EEPROM) */
//			no = (pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.no<<2);							/* <S06D>�FDell */
//			no = no + VMEM_VEN_MIN + ((COMM_PRM_OFFSET)<<1);   							/* <S06D>�FDell */
			chekNo = (pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.no<<1) + COMM_PRM_OFFSET;	/* <S06D>�FAdd */
			no = VMEM_VEN_MIN + ((chekNo)<<1);  										/* <S06D>�FAdd */
			break;
		case 0x10:/* �@��p�����[�^(RAM) */
//			no = (pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.no<<1);							/* <S06D>�FDell */
//			no = no + VMEM_VEN_MIN + ((RAM_OFFSET)<<1);        							/* <S06D>�FDell */
			chekNo = pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.no;							/* <S06D>�FAdd */
			no = VMEM_VEN_MIN + ((chekNo + RAM_OFFSET)<<1); 							/* <S06D>�FAdd */
			break;
		case 0x11:/* �@��p�����[�^(EEPROM) */
//			no = (pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.no<<1);							/* <S06D>�FDell */
//			no = no + VMEM_VEN_MIN;                            							/* <S06D>�FDell */
			chekNo = pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.no;							/* <S06D>�FAdd */
			no = VMEM_VEN_MIN + ((chekNo)<<1); 											/* <S06D>�FAdd */
			break;
		default:
			chekNo = 0;/* <S14D> */
			ret = DATA_SET_WARNG2;
			break;
		}
		if(pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.size == 0)/* <S16C> */
		{
			ret = DATA_SET_WARNG4;			/* A.94D �f�[�^�ݒ�x���S */
		}
		
		/* �p�����[�^�ԍ��A�T�C�Y�`�F�b�N *//* <S06D>�FAdd */
		if(chekNo >= 0x1000)
		{
			ret = DATA_SET_WARNG1;
		}

		if( ret == CMD_FUNC_NORMAL_END )
		{
			/* �f�o�C�X���ǂݏo���p�f�[�^�ݒ� */
			pCmdHdl->cmdInfo.RegData.AccessSize = pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.size;
			pCmdHdl->cmdInfo.RegData.Length = 1;
			pCmdHdl->cmdInfo.RegData.Number = no;
			pCmdHdl->cmdInfo.RegData.EepWrite = FALSE;
			pCmdHdl->cmdInfo.RegData.pBuf = pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.ldata;
			pCmdHdl->cmdInfo.RegData.BufferSize = sizeof(pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.ldata);

			/* ���W�X�^�R�}���h�����v�����sAPI	*/
			ret = RegRequestCommand(&hNetData->reg_hdl, REG_CMD_PRMWR, &pCmdHdl->cmdInfo.RegData);
		}

		/* ��͌��ʂ��o�b�t�@�փR�s�[ */
		pCmdHdl->cmdInfo.rsltBuf.stdData.svprm.cmd  = cmd;
		pCmdHdl->cmdInfo.rsltBuf.dword[3] = pCmdHdl->cmdInfo.cmdBuf.dword[3];

		pCmdHdl->cmdInfo.rsltBuf.stdData.svprm.ldata[0] = pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.ldata[0];
		pCmdHdl->cmdInfo.rsltBuf.stdData.svprm.ldata[1] = pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.ldata[1];
		pCmdHdl->cmdInfo.rsltBuf.stdData.svprm.ldata[2] = pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.ldata[2];
		pCmdHdl->cmdInfo.rsltBuf.stdData.svprm.ldata[3] = pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.ldata[3];

		/* �R�}���h��͎��s���ʐݒ� */
		pCmdHdl->cmdInfo.ResultCmdAna = ret;
	}
	else
	{
		if(pCmdHdl->cmdInfo.cmdRdy == FALSE)
		{
			ret = RegCheckCommandState(&hNetData->reg_hdl);
			pCmdHdl->cmdInfo.ResultCmdAna = ret;
		}
	}

	return ret;
}


/****************************************************************************/
/*								�T�u�R�}���h								*/
/****************************************************************************/
/****************************************************************************/
/* �T�v		:	����`�R�}���h��͏���										*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_WARNG6				�R�}���h�x���U					*/
/****************************************************************************/
INT32 stdExCmd_NO_DEFINE( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;

	pCmdHdl = &hNetData->cmd_hdl;

	/* �ُ�I����Ԃ� */
	pCmdHdl->exCmdInfo.rsltBuf.stdData = pCmdHdl->exCmdInfo.cmdBuf.stdData;
	return CMD_WARNG6;
}


/****************************************************************************/
/* �T�v		:	���T�|�[�g�R�}���h��͏���									*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_WARNG2				�R�}���h�x���Q					*/
/****************************************************************************/
INT32 stdExCmd_NO_SUPP( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;

	pCmdHdl = &hNetData->cmd_hdl;

	/* �ُ�I����Ԃ� */
	pCmdHdl->exCmdInfo.rsltBuf.stdData = pCmdHdl->exCmdInfo.cmdBuf.stdData;
	return CMD_WARNG2;
}


/****************************************************************************/
/* �T�v		:	�m�n�o�R�}���h��͏���										*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END		����I��						*/
/****************************************************************************/
INT32 stdExCmd_NOP( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;

	pCmdHdl = &hNetData->cmd_hdl;

	/* �T�u�R�}���h����(SUB_CTRL)���� */
	stdCmdSubCtrlProc( pCmdHdl );
	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->exCmdInfo.rsltBuf.stdData.mon.cmd = pCmdHdl->exCmdInfo.cmdBuf.stdData.mon.cmd;

	/* �R�}���h��͎��s���ʐݒ� */
	pCmdHdl->exCmdInfo.ResultCmdAna = CMD_FUNC_NORMAL_END;

	return CMD_FUNC_NORMAL_END;
}


/****************************************************************************/
/* �T�v		:	�`�k�l�Q�q�c�R�}���h��͏���								*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END		����I��						*/
/*			:	= CMD_FUNC_CONTINUE			�p�����s��						*/
/*			:	= DATA_SET_WARNG2			�f�[�^�ݒ�x���Q�i�`�D�X�S�a�j	*/
/*			:	= CMD_WARNG1				�R�}���h�x���P�i�`�D�X�T�`�j	*/
/****************************************************************************/
INT32 stdExCmd_ALM_RD( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	UCHAR		cmd;
	USHORT		alm_rd_mod, alm_idx;
	USHORT		*alm_data;
	INT32		ret;

	/* ���[�J���ϐ��̏����� */
	ret			= CMD_FUNC_NORMAL_END;
	pCmdHdl		= &hNetData->cmd_hdl;

	/* �T�u�R�}���h����(SUB_CTRL)���� */
	stdCmdSubCtrlProc( pCmdHdl );

	cmd			= pCmdHdl->exCmdInfo.cmdBuf.stdData.alm.cmd;		/* �R�}���h�R�[�h */
	alm_rd_mod	= pCmdHdl->exCmdInfo.cmdBuf.stdData.alm.mode;		/* �A���[���ǂݏo�����[�h */
	alm_idx		= pCmdHdl->exCmdInfo.cmdBuf.stdData.alm.index;		/* �A���[���C���f�b�N�X�ԍ� */

	/* ���s�����`�F�b�N */
	if( (pCmdHdl->exCmdInfo.cmdRdy == FALSE) 
	    || (pCmdHdl->exCmdInfo.rsltBuf.stdData.alm.cmd   != cmd)
	    || (pCmdHdl->exCmdInfo.rsltBuf.stdData.alm.mode  != alm_rd_mod)
	    || (pCmdHdl->exCmdInfo.rsltBuf.stdData.alm.index != alm_idx) )
	{
		alm_data = &pCmdHdl->exCmdInfo.rsltBuf.stdData.alm.data[0];	
		ret = stdCmdAlmRdProc( &hNetData->alm_hdl, alm_rd_mod, alm_idx, alm_data, EX_ALM_RD_NUM );

		/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
		pCmdHdl->exCmdInfo.rsltBuf.stdData.alm.cmd  = cmd;
		pCmdHdl->exCmdInfo.rsltBuf.stdData.alm.mode = alm_rd_mod;
		pCmdHdl->exCmdInfo.rsltBuf.stdData.alm.index = alm_idx;

		/* �R�}���h��͎��s���ʐݒ� */
		pCmdHdl->exCmdInfo.ResultCmdAna = ret;
	}

	return ret;
}


/****************************************************************************/
/* �T�v		:	�`�k�l�Q�b�k�q�R�}���h��͏���								*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END		����I��						*/
/*			:	= CMD_FUNC_CONTINUE			�p�����s��						*/
/*			:	= DATA_SET_WARNG2			�f�[�^�ݒ�x���Q�i�`�D�X�S�a�j	*/
/*			:	= CMD_WARNG1				�R�}���h�x���P�i�`�D�X�T�`�j	*/
/****************************************************************************/
INT32 stdExCmd_ALM_CLR( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	UCHAR		cmd;
	USHORT		alm_clr_mod;
	INT32		ret;

	/* ���[�J���ϐ��̏����� */
	ret			= CMD_FUNC_NORMAL_END;
	pCmdHdl		= &hNetData->cmd_hdl;

	/* �T�u�R�}���h����(SUB_CTRL)���� */
	stdCmdSubCtrlProc( pCmdHdl );

	cmd			= pCmdHdl->exCmdInfo.cmdBuf.stdData.alm.cmd;
	alm_clr_mod	= pCmdHdl->exCmdInfo.cmdBuf.stdData.alm.mode;

	/* ���s�����`�F�b�N */
	if( (pCmdHdl->exCmdInfo.cmdRdy != FALSE)
	    && ((pCmdHdl->exCmdInfo.rsltBuf.stdData.alm.cmd  != cmd) 
	    || (pCmdHdl->exCmdInfo.rsltBuf.stdData.alm.mode != alm_clr_mod)) )
	{
		if( alm_clr_mod == ALM_CLR_MOD0 )
		{
			ret = AlmSetClearReq( &hNetData->alm_hdl, TRUE );
		}
		else if( alm_clr_mod == ALM_CLR_MOD1 )
		{
			AlmRequestAlarmHistoryClear( &hNetData->alm_hdl );
			ret = CMD_FUNC_CONTINUE;
		}
		else
		{
			ret = DATA_SET_WARNG2;
		}
		/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
		pCmdHdl->exCmdInfo.rsltBuf.stdData.alm.cmd  = cmd;
		pCmdHdl->exCmdInfo.rsltBuf.stdData.alm.mode = alm_clr_mod;

		/* �R�}���h��͎��s���ʐݒ� */
		pCmdHdl->exCmdInfo.ResultCmdAna = ret;
	}
	else
	{
		if(pCmdHdl->exCmdInfo.cmdRdy == FALSE)
		{
			if( alm_clr_mod == ALM_CLR_MOD0 )
			{
				ret = AlmSetClearReq( &hNetData->alm_hdl, FALSE );
			}
			else //if( alm_clr_mod == ALM_CLR_MOD1 )
			{
				ret = AlmGetAlarmHistoryClearStatus( &hNetData->alm_hdl );
			}
			pCmdHdl->exCmdInfo.ResultCmdAna = ret;
		}
	}

	return ret;
}


/****************************************************************************/
/* �T�v		:	�l�d�l�Q�q�c�R�}���h��͏���								*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END	����I��							*/
/*			:	= CMD_FUNC_CONTINUE		�p�����s��							*/
/*			:	= DATA_SET_WARNG4		�f�[�^�ݒ�x���S	�i�`�D�X�S�c�j	*/
/*			:	= REG_SIZE_ERR			�f�[�^�T�C�Y�x��	�i�`�D�X�S�c�j	*/
/*			:	= REG_NO_ERR			�A�h���X�ُ�		�i�`�D�X�S�`�j	*/
/****************************************************************************/
INT32 stdExCmd_MEM_RD( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	LONG		mode, type;
	LONG		adrs;
	LONG		size, accSize, buf;/* <S14D> */
	UCHAR		cmd;
	INT32		ret;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl		= &hNetData->cmd_hdl;

	/* �T�u�R�}���h����(SUB_CTRL)���� */
	stdCmdSubCtrlProc( pCmdHdl );

	cmd	 = pCmdHdl->exCmdInfo.cmdBuf.stdData.mem.cmd;	
	adrs = pCmdHdl->exCmdInfo.cmdBuf.stdData.mem.address;
	ret	 = CMD_FUNC_NORMAL_END;

	/* ���s�����`�F�b�N */
	if( (pCmdHdl->exCmdInfo.cmdRdy != FALSE)
		&& ((pCmdHdl->exCmdInfo.rsltBuf.stdData.mem.cmd != cmd)
		||  (pCmdHdl->exCmdInfo.rsltBuf.stdData.mem.address != adrs)
		||  (pCmdHdl->exCmdInfo.rsltBuf.dword[1] != pCmdHdl->exCmdInfo.cmdBuf.dword[1])) )
	{
		size = pCmdHdl->exCmdInfo.cmdBuf.stdData.mem.size;
		mode = pCmdHdl->exCmdInfo.cmdBuf.stdData.mem.mode >> 4;
		type = pCmdHdl->exCmdInfo.cmdBuf.stdData.mem.mode & 0x0F;
		/* �A�N�Z�X����f�[�^�T�C�Y�擾 */
		if ( (mode != 1) || (type < 1) || (type >= 4) )
		{
			ret = DATA_SET_WARNG2;		/* A.94B �f�[�^�ݒ�ُ�			*/
		}
		else if(size == 0)/* <S14D> */
		{
			ret = DATA_SET_WARNG4;			/* A.94D �f�[�^�ݒ�x���S */
		}
		else
		{			/* �f�o�C�X���ǂݏo���p�f�[�^�ݒ� */
/* <S14D> */
//			pCmdHdl->exCmdInfo.RegData.AccessSize = 1 << (type-1);
//			pCmdHdl->exCmdInfo.RegData.Length = size;
			accSize = 1 << (type-1);
			if((adrs >= VMEM_COM_MIN) && (adrs <= VMEM_COM_MAX))
			{/* ���ʃp�����[�^�̈�	*/
				buf = accSize * size;
				pCmdHdl->exCmdInfo.RegData.AccessSize = 4;
				pCmdHdl->exCmdInfo.RegData.Length = buf >> 2;	/* buf�́A4�̐����{("accSize < 4"�łȂ����)	*/
				if( accSize < 4 )
				{/* ���ʃp�����[�^�̈�́A�A�N�Z�X�T�C�Y��4�̂�	*/
					ret = DATA_SET_WARNG4;			/* A.94D �f�[�^�ݒ�x���S */
				}
			}
			else if(adrs >= VMEM_VEN_MIN)
			{/* �x���_�[�̈�	*/
				buf = accSize * size;
				pCmdHdl->exCmdInfo.RegData.AccessSize = 2;
				pCmdHdl->exCmdInfo.RegData.Length = buf >> 1;	/* buf�́A2�̐����{("accSize < 2"�łȂ����)	*/
				if( accSize < 2 )
 				{/* �x���_�́A�A�N�Z�X�T�C�Y��2,4�̂�	*/
					ret = DATA_SET_WARNG4;			/* A.94D �f�[�^�ݒ�x���S */
				}
			}
			else if(adrs <= VMEM_ID_MAX)
			{
				buf = accSize * size;
				pCmdHdl->exCmdInfo.RegData.AccessSize = 1;
				pCmdHdl->exCmdInfo.RegData.Length = buf;
			}
			else
			{
				pCmdHdl->exCmdInfo.RegData.AccessSize = accSize;
				pCmdHdl->exCmdInfo.RegData.Length = size;
			}
/* <S14D> */
			if(ret == CMD_FUNC_NORMAL_END)
			{
				pCmdHdl->exCmdInfo.RegData.Number = adrs;
				pCmdHdl->exCmdInfo.RegData.EepWrite = FALSE;
				pCmdHdl->exCmdInfo.RegData.pBuf = &pCmdHdl->exCmdInfo.rsltBuf.stdData.mem.ldata;
				pCmdHdl->exCmdInfo.RegData.BufferSize = sizeof(pCmdHdl->exCmdInfo.rsltBuf.stdData.mem.ldata);
				
				/* ���W�X�^�R�}���h�����v�����sAPI	*/
				ret = RegRequestCommand(&hNetData->reg_hdl, REG_CMD_PRMRD, &pCmdHdl->exCmdInfo.RegData);
			}
		}
		/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
		pCmdHdl->exCmdInfo.rsltBuf.stdData.mem.cmd = cmd;
		pCmdHdl->exCmdInfo.rsltBuf.stdData.mem.address = adrs;
		pCmdHdl->exCmdInfo.rsltBuf.dword[1] = pCmdHdl->exCmdInfo.cmdBuf.dword[1];
		/* �R�}���h��͎��s���ʐݒ� */
		pCmdHdl->exCmdInfo.ResultCmdAna = ret;
	}
	else
	{
		if(pCmdHdl->exCmdInfo.cmdRdy == FALSE)
		{
			ret = RegCheckCommandState(&hNetData->reg_hdl);
			pCmdHdl->exCmdInfo.ResultCmdAna = ret;
		}
	}

	return ret;
}


/****************************************************************************/
/* �T�v		:	�l�d�l�Q�v�q�R�}���h��͏���								*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END	����I��							*/
/*			:	= CMD_FUNC_CONTINUE		�p�����s��							*/
/*			:	= DATA_SET_WARNG4		�f�[�^�ݒ�x���S	�i�`�D�X�S�c�j	*/
/*			:	= REG_SIZE_ERR			�f�[�^�T�C�Y�x��	�i�`�D�X�S�c�j	*/
/*			:	= REG_NO_ERR			�A�h���X�ُ�		�i�`�D�X�S�`�j	*/
/****************************************************************************/
INT32 stdExCmd_MEM_WR( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	UCHAR		cmd;
	ULONG		adrs;
	LONG		size;
	LONG		mode, type;
	INT32		ret;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl			= &hNetData->cmd_hdl;

	/* �T�u�R�}���h����(SUB_CTRL)���� */
	stdCmdSubCtrlProc( pCmdHdl );

	cmd	 = pCmdHdl->exCmdInfo.cmdBuf.stdData.mem.cmd;	
	adrs = pCmdHdl->exCmdInfo.cmdBuf.stdData.mem.address;
	ret  = CMD_FUNC_NORMAL_END;

	/* ���s�����`�F�b�N */
	if( (pCmdHdl->exCmdInfo.cmdRdy != FALSE)/* <S1F8> */
		&& ((pCmdHdl->exCmdInfo.rsltBuf.stdData.mem.cmd != cmd)
		||	(pCmdHdl->exCmdInfo.rsltBuf.stdData.mem.address != adrs) 
		||	(pCmdHdl->exCmdInfo.rsltBuf.dword[1] != pCmdHdl->exCmdInfo.cmdBuf.dword[1])
		||	(pCmdHdl->exCmdInfo.rsltBuf.stdData.mem.ldata != pCmdHdl->exCmdInfo.cmdBuf.stdData.mem.ldata)) )
	{
		/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
		pCmdHdl->exCmdInfo.rsltBuf.stdData.mem.cmd = cmd;
		pCmdHdl->exCmdInfo.rsltBuf.stdData.mem.address = adrs;
		pCmdHdl->exCmdInfo.rsltBuf.dword[1] = pCmdHdl->exCmdInfo.cmdBuf.dword[1];
		pCmdHdl->exCmdInfo.rsltBuf.stdData.mem.ldata = pCmdHdl->exCmdInfo.cmdBuf.stdData.mem.ldata;

		size = pCmdHdl->exCmdInfo.cmdBuf.stdData.mem.size;
		mode = pCmdHdl->exCmdInfo.cmdBuf.stdData.mem.mode >> 4;
		type = pCmdHdl->exCmdInfo.cmdBuf.stdData.mem.mode & 0x0F;
		/* �A�N�Z�X����f�[�^�T�C�Y�擾 */
		if( (mode < 1) || (mode > 2) || (type < 1) || (type >= 4) )
		{
			ret = DATA_SET_WARNG2;			/* A.94B �f�[�^�ݒ�ُ� */
		}
		else if(size == 0)/* <S14D> */
		{
			ret = DATA_SET_WARNG4;			/* A.94D �f�[�^�ݒ�x���S */
		}
		else
		{
			/* �f�o�C�X���ǂݏo���p�f�[�^�ݒ� */
			pCmdHdl->exCmdInfo.RegData.AccessSize = 1 << (type-1);
			pCmdHdl->exCmdInfo.RegData.Length = size;
			pCmdHdl->exCmdInfo.RegData.Number = adrs;
			pCmdHdl->exCmdInfo.RegData.EepWrite = (mode-1);
			pCmdHdl->exCmdInfo.RegData.pBuf = &pCmdHdl->exCmdInfo.rsltBuf.stdData.mem.ldata;
			pCmdHdl->exCmdInfo.RegData.BufferSize = sizeof(pCmdHdl->exCmdInfo.rsltBuf.stdData.mem.ldata);
			
			/* ���W�X�^�R�}���h�����v�����sAPI	*/
			ret = RegRequestCommand(&hNetData->reg_hdl, REG_CMD_PRMWR, &pCmdHdl->exCmdInfo.RegData);
		}
		/* �R�}���h��͎��s���ʐݒ� */
		pCmdHdl->exCmdInfo.ResultCmdAna = ret;
	}
	else
	{
		if(pCmdHdl->exCmdInfo.cmdRdy == FALSE)
		{
			ret = RegCheckCommandState(&hNetData->reg_hdl);
			pCmdHdl->exCmdInfo.ResultCmdAna = ret;
		}
	}
	return ret;
}


/****************************************************************************/
/* �T�v		:	�r�l�n�m�R�}���h��͏���									*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END	����I��							*/
/****************************************************************************/
INT32 stdExCmd_SMON( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl	= &hNetData->cmd_hdl;
	/* �T�u�R�}���h����(SUB_CTRL)���� */
	stdCmdSubCtrlProc( pCmdHdl );
	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->exCmdInfo.rsltBuf.stdData.mon.cmd = pCmdHdl->exCmdInfo.cmdBuf.stdData.mon.cmd;

	/* �R�}���h��͎��s���ʐݒ� */
	pCmdHdl->exCmdInfo.ResultCmdAna = CMD_FUNC_NORMAL_END;

	return CMD_FUNC_NORMAL_END;
}


/****************************************************************************/
/* �T�v		:	�r�u�o�q�l�Q�q�c�R�}���h��͏���							*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END	����I��							*/
/*			:	= CMD_FUNC_CONTINUE		�p�����s��							*/
/*			:	= DATA_SET_WARNG1		�f�[�^�ݒ�x���P	�i�`�D�X�S�`�j	*/
/*			:	= DATA_SET_WARNG2		�f�[�^�ݒ�x���Q	�i�`�D�X�S�a�j	*/
/*			:	= DATA_SET_WARNG4		�f�[�^�ݒ�x���S	�i�`�D�X�S�c�j	*/
/*			:	= CMD_WARNG1			�R�}���h�x���P		�i�`�D�X�T�`�j	*/
/****************************************************************************/
INT32 stdExCmd_SVPRM_RD( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	UCHAR		cmd;
	ULONG		no,chekNo;/* <S06D>�FchekNo�ǉ� *//* <S1C1> */
	INT32		ret;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl		= &hNetData->cmd_hdl;

	/* �T�u�R�}���h����(SUB_CTRL)���� */
	stdCmdSubCtrlProc( pCmdHdl );

	ret	= CMD_FUNC_NORMAL_END;
	cmd	= pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.cmd;

	/* ���s�����`�F�b�N */
	if( (pCmdHdl->exCmdInfo.cmdRdy != FALSE) 
		&& ((pCmdHdl->exCmdInfo.rsltBuf.stdData.svprm.cmd != cmd) 
		||  (pCmdHdl->exCmdInfo.rsltBuf.dword[1] != pCmdHdl->exCmdInfo.cmdBuf.dword[1])) ) 
	{

		switch(pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.mode)
		{
		case 0x00:/* ���ʃp�����[�^(RAM) */
//			no = (pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.no<<2);
//			no = no + VMEM_VEN_MIN + ((COMM_PRM_OFFSET+RAM_OFFSET)<<1); 
			chekNo = (pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.no<<1) + COMM_PRM_OFFSET;	/* <S06D>�FAdd *//* <S1C1> */
			no = VMEM_VEN_MIN + ((chekNo + RAM_OFFSET)<<1); 							/* <S06D>�FAdd *//* <S1C1> */
			break;
		case 0x10:/* �@��p�����[�^(RAM) */
//			no = (pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.no<<1);
//			no = no + VMEM_VEN_MIN + ((RAM_OFFSET)<<1); 
			chekNo = pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.no;							/* <S06D>�FAdd *//* <S1C1> */
			no = VMEM_VEN_MIN + ((chekNo + RAM_OFFSET)<<1); 							/* <S06D>�FAdd *//* <S1C1> */
			break;
		default:
			ret = DATA_SET_WARNG2;
			break;
		}
		if(pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.size == 0)/* <S16C> *//* <S1C0> */
		{
			ret = DATA_SET_WARNG4;			/* A.94D �f�[�^�ݒ�x���S */
		}
		/* �p�����[�^�ԍ��A�T�C�Y�`�F�b�N *//* <S06D>�FAdd *//* <S1C1> */
		if(chekNo >= 0x1000)
		{
			ret = DATA_SET_WARNG1;
		}

		if( ret == CMD_FUNC_NORMAL_END )
		{
			/* �f�o�C�X���ǂݏo���p�f�[�^�ݒ� */
			pCmdHdl->exCmdInfo.RegData.AccessSize = pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.size;
			pCmdHdl->exCmdInfo.RegData.Length = 1;
			pCmdHdl->exCmdInfo.RegData.Number = no;
			pCmdHdl->exCmdInfo.RegData.EepWrite = FALSE;
			pCmdHdl->exCmdInfo.RegData.pBuf = pCmdHdl->exCmdInfo.rsltBuf.stdData.svprm.ldata;
			pCmdHdl->exCmdInfo.RegData.BufferSize = sizeof(pCmdHdl->exCmdInfo.rsltBuf.stdData.svprm.ldata);
			
			/* ���W�X�^�R�}���h�����v�����sAPI	*/
			ret = RegRequestCommand(&hNetData->reg_hdl, REG_CMD_PRMRD, &pCmdHdl->exCmdInfo.RegData);
		}

		/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
		pCmdHdl->exCmdInfo.rsltBuf.stdData.svprm.cmd  = cmd;
		pCmdHdl->exCmdInfo.rsltBuf.dword[1] = pCmdHdl->exCmdInfo.cmdBuf.dword[1];

		/* �R�}���h��͎��s���ʐݒ� */
		pCmdHdl->exCmdInfo.ResultCmdAna = ret;
	}
	else
	{
		if(pCmdHdl->exCmdInfo.cmdRdy == FALSE)
		{
			ret = RegCheckCommandState(&hNetData->reg_hdl);
			pCmdHdl->exCmdInfo.ResultCmdAna = ret;
		}
	}

	return ret;
}


/****************************************************************************/
/* �T�v		:	�r�u�o�q�l�Q�v�q�R�}���h��͏���							*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END		����I��						*/
/*			:	= CMD_FUNC_CONTINUE			�p�����s��						*/
/*			:	= DATA_SET_WARNG1		�f�[�^�ݒ�x���P�i�`�D�X�S�`�j		*/
/*			:	= DATA_SET_WARNG2		�f�[�^�ݒ�x���Q�i�`�D�X�S�a�j		*/
/*			:	= DATA_SET_WARNG3		�f�[�^�ݒ�x���R�i�`�D�X�S�b�j		*/
/*			:	= DATA_SET_WARNG4		�f�[�^�ݒ�x���S�i�`�D�X�S�c�j		*/
/*			:	= CMD_WARNG1			�R�}���h�x���P	�i�`�D�X�T�`�j		*/
/****************************************************************************/
INT32 stdExCmd_SVPRM_WR( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	ULONG		no,chekNo;/* <S06D>�FchekNo�ǉ� *//* <S1C1> */
	INT32		ret;
	UCHAR		cmd;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl		= &hNetData->cmd_hdl;

	/* �T�u�R�}���h����(SUB_CTRL)���� */
	stdCmdSubCtrlProc( pCmdHdl );

	ret	= CMD_FUNC_NORMAL_END;
	cmd	= pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.cmd;

	/* ���s�����`�F�b�N */
	if( (pCmdHdl->exCmdInfo.cmdRdy != FALSE)
		&& ((pCmdHdl->exCmdInfo.rsltBuf.stdData.svprm.cmd != cmd) 
		||  (pCmdHdl->exCmdInfo.rsltBuf.dword[1] != pCmdHdl->exCmdInfo.cmdBuf.dword[1])
		||  (pCmdHdl->exCmdInfo.rsltBuf.stdData.svprm.ldata[0] != pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.ldata[0])
		||  (pCmdHdl->exCmdInfo.rsltBuf.stdData.svprm.ldata[1] != pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.ldata[1])) )
	{
		switch(pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.mode)
		{
		case 0x00:/* ���ʃp�����[�^(RAM) */
//			no = (pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.no<<2);
//			no = no + VMEM_VEN_MIN + ((COMM_PRM_OFFSET+RAM_OFFSET)<<1); 
			chekNo = (pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.no<<1) + COMM_PRM_OFFSET;		/* <S06D>�FAdd *//* <S1C1> */
			no = VMEM_VEN_MIN + ((chekNo + RAM_OFFSET)<<1); 							/* <S06D>�FAdd *//* <S1C1> */		
			break;
		case 0x01:/* ���ʃp�����[�^(EEPROM) */
//			no = (pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.no<<2);
//			no = no + VMEM_VEN_MIN + ((COMM_PRM_OFFSET)<<1); 
			chekNo = (pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.no<<1) + COMM_PRM_OFFSET;		/* <S06D>�FAdd *//* <S1C1> */
			no = VMEM_VEN_MIN + ((chekNo)<<1); 						/* <S06D>�FAdd *//* <S1C1> */
			break;
		case 0x10:/* �@��p�����[�^(RAM) */
//			no = (pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.no<<1);
//			no = no + VMEM_VEN_MIN + ((RAM_OFFSET)<<1); 
			chekNo = pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.no;							/* <S06D>�FAdd *//* <S1C1> */
			no = VMEM_VEN_MIN + ((chekNo + RAM_OFFSET)<<1); 							/* <S06D>�FAdd *//* <S1C1> */
			break;
		case 0x11:/* �@��p�����[�^(EEPROM) */
//			no = (pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.no<<1);
//			no = no + VMEM_VEN_MIN;
			chekNo = pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.no;							/* <S06D>�FAdd *//* <S1C1> */
			no = VMEM_VEN_MIN + ((chekNo)<<1); 											/* <S06D>�FAdd *//* <S1C1> */
			break;
		default:
			ret = DATA_SET_WARNG2;
			break;
		}
		if(pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.size == 0)/* <S16C> *//* <S1C1> */
		{
			ret = DATA_SET_WARNG4;			/* A.94D �f�[�^�ݒ�x���S */
		}
		/* �p�����[�^�ԍ��A�T�C�Y�`�F�b�N *//* <S06D>�FAdd *//* <S1C1> */
		if(chekNo >= 0x1000)
		{
			ret = DATA_SET_WARNG1;
		}

		if( ret == CMD_FUNC_NORMAL_END )
		{
			/* �f�o�C�X���ǂݏo���p�f�[�^�ݒ� */
			pCmdHdl->exCmdInfo.RegData.AccessSize = pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.size;
			pCmdHdl->exCmdInfo.RegData.Length = 1;
			pCmdHdl->exCmdInfo.RegData.Number = no;
			pCmdHdl->exCmdInfo.RegData.EepWrite = FALSE;
			pCmdHdl->exCmdInfo.RegData.pBuf = pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.ldata;
			pCmdHdl->exCmdInfo.RegData.BufferSize = sizeof(pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.ldata);

			/* ���W�X�^�R�}���h�����v�����sAPI	*/
			ret = RegRequestCommand(&hNetData->reg_hdl, REG_CMD_PRMWR, &pCmdHdl->exCmdInfo.RegData);
		}

		/* ��͌��ʂ��o�b�t�@�փR�s�[ */
		pCmdHdl->exCmdInfo.rsltBuf.stdData.svprm.cmd  = cmd;
		pCmdHdl->exCmdInfo.rsltBuf.dword[1] = pCmdHdl->exCmdInfo.cmdBuf.dword[1];

		pCmdHdl->exCmdInfo.rsltBuf.stdData.svprm.ldata[0] = pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.ldata[0];
		pCmdHdl->exCmdInfo.rsltBuf.stdData.svprm.ldata[1] = pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.ldata[1];

		/* �R�}���h��͎��s���ʐݒ� */
		pCmdHdl->exCmdInfo.ResultCmdAna = ret;
	}
	else
	{
		if(pCmdHdl->exCmdInfo.cmdRdy == FALSE)
		{
			ret = RegCheckCommandState(&hNetData->reg_hdl);
			pCmdHdl->exCmdInfo.ResultCmdAna = ret;
		}
	}

	return ret;
}

#if 0
/****************************************************************************/
/* �T�v		�F�w�߃f�[�^�̃N�����v�@�\�t���Q�C����Z						*/
/****************************************************************************/
/* ULONG inData			���͒l�i�w�߃f�[�^�j								*/
/* LONG  maxData		�ő�l												*/
/* LONG  kx				�m�[�}���C�Y�萔									*/
/* LONG* outData		�o�͒l�i�v�Z���ʁj									*/
/****************************************************************************/
/* ret = CMD_FUNC_NORMAL_END  ����I��										*/
/* ret = DATA_SET_WARNG6      �f�[�^�ݒ�x��(�f�[�^�N�����v����)			*/
/****************************************************************************/
static INT32 cmdMulgain32WithClampData( ULONG inData, LONG maxData, KSGAIN kx, LONG* outData )
{
	INT32 ret;

	ret = CMD_FUNC_NORMAL_END;

	if( inData <= 0x7FFFFFFF )
	{/* inData <= 0x7FFFFFFF �̂Ƃ��͒ʐM�f�[�^���w�� */
		*outData = MlibMulgain32( (LONG)inData, kx );
	}
	else if( inData == 0xFFFFFFFF )
	{/* inData = 0xFFFFFFFF �̂Ƃ��͍ő�l���w�� */
		*outData = maxData;
	}
	else
	{/* inData > 0x7FFFFFFF �̂Ƃ��� inData = 0x7FFFFFFF ���w�� */
		*outData = MlibMulgain32( 0x7FFFFFFF, kx );
		ret = DATA_SET_WARNG6;
	}
	return ret;
}
#endif

#if 0 /* ���󖢑Ή� */
/****************************************************************************/
/* �T�v		:	�������Ԃ̃N�����v�@�\�t���Q�C����Z ����������				*/
/****************************************************************************/
/* ULONG inData			���͒l�i�w�߃f�[�^�j								*/
/* LONG  maxData		�ő�l												*/
/* LONG  kx				�m�[�}���C�Y�萔									*/
/* LONG* outData		�o�͒l�i�v�Z���ʁj									*/
/****************************************************************************/
/* ret = CMD_FUNC_NORMAL_END  ����I��										*/
/* ret = DATA_SET_WARNG6      �f�[�^�ݒ�x��(�f�[�^�N�����v����)			*/
/****************************************************************************/
static INT32 cmdMulgain32WithClampAccTime_f( ULONG inData, LONG maxData, KSGAIN kx, LONG *outData )
{
	INT32 ret;

	ret = CMD_FUNC_NORMAL_END;

	if( inData <= 0x7FFFFFFF )
	{/* inData <= 0x7FFFFFFF �̂Ƃ��͒ʐM�f�[�^���w�� */
		*outData = MlibDiv6432_f( (double)kx, (double)1000, (double)inData );			/* ACCR�Z�b�g */
	}
	else if( inData == 0x0 )
	{/* inData = 0x0 �̂Ƃ��͍ő�l���w�� */
		*outData = maxData;
	}
	else
	{/* inData > 0x7FFFFFFF �̂Ƃ��� inData = 0x7FFFFFFF ���w�� */
		*outData = MlibDiv6432_f( (double)kx, (double)1000, (double)0x7FFFFFFF );		/* ACCR�Z�b�g */
		ret = DATA_SET_WARNG6;
	}
	return ret;
}

/****************************************************************************/
/* �T�v		�F�������Ԃ̃N�����v�@�\�t���Q�C����Z							*/
/****************************************************************************/
/* ULONG inData			���͒l�i�w�߃f�[�^�j								*/
/* LONG  maxData		�ő�l												*/
/* LONG  kx				�m�[�}���C�Y�萔									*/
/* LONG* outData		�o�͒l�i�v�Z���ʁj									*/
/****************************************************************************/
/* ret = CMD_FUNC_NORMAL_END  ����I��										*/
/* ret = DATA_SET_WARNG6      �f�[�^�ݒ�x��(�f�[�^�N�����v����)			*/
/****************************************************************************/
static INT32 cmdMulgain32WithClampAccTime( ULONG inData, LONG maxData, KSGAIN kx, LONG* outData )
{
	INT32 ret;
	
	ret = CMD_FUNC_NORMAL_END;

	if( inData <= 0x7FFFFFFF )
	{/* inData <= 0x7FFFFFFF �̂Ƃ��͒ʐM�f�[�^���w�� */
		*outData = MlibDiv6432( kx, 1000, inData );	/* ACCR�Z�b�g */
	}
	else if( inData == 0x0 )
	{/* inData = 0x0 �̂Ƃ��͍ő�l���w�� */
		*outData = maxData;
	}
	else
	{/* inData > 0x7FFFFFFF �̂Ƃ��� inData = 0x7FFFFFFF ���w�� */
		*outData = MlibDiv6432( kx, 1000, 0x7FFFFFFF );	/* ACCR�Z�b�g */
		ret = DATA_SET_WARNG6;
	}
	return ret;
}

#endif /* #if 0 */




