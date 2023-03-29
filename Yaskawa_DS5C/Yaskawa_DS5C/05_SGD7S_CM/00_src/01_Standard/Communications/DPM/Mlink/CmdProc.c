/****************************************************************************/
/*                                                                          */
/*            MECHATROLINK �T�[�{�ʐM�h�e�A�v�� �ʐM�R���|�[�l���g          */
/*                                                                          */
/*            �e�R�}���h��͏���											*/
/*																			*/
/****************************************************************************/
/*	Histroy:																*/
/*		Rev2.01 2007.12.14	�����x�w�ߒǉ�									*/
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*******************  Yaskawa Electric Corporation **************************/

/* includes */
#include "Basedef.h"
#include "M2Handler.h"
#include "ComAlmMngr.h"
#include "ComManager.h"
#include "SysApi.h"
#include "MtApi.h"
#include "CmdAnalyzer.h"
#include "MLib.h"

/* ID_RD */
#define ID_RD_MAX_SIZE			8				/* �ő�ǂݏo���T�C�Y	*/

/* ALM_RD */
#define ALM_RD_MOD0				0				/* ���ݔ������ُ̈�E�x���̏�� */
#define ALM_RD_MOD1				1				/* �ُ픭���󋵗��� */
#define ALM_RD_MOD2				2				/* ���ݔ������ُ̈�E�x���̏ڍ׏�� */
#define ALM_RD_MOD3				3				/* �ُ픭���󋵗����̏ڍ׏�� */

/* CONFIG */
#define CONFIG_MOD0				0				/* �p�����[�^�Čv�Z */
#define CONFIG_MOD1				1				/* EEPROM�ւ̈ꊇ�������� */

/* ALM_CLR */
#define ALM_CLR_MOD0			0				/* ���ݔ������ُ̈�E�x���̏�ԃN���A */
#define ALM_CLR_MOD1			1				/* �ُ픭���󋵗����N���A */

/* MEM_RD */
#define MEM_RD_MAX_SIZE			20				/* �ő�ǂݏo���T�C�Y(���C���R�}���h) */
#define EX_MEM_RD_MAX_SIZE		4				/* �ő�ǂݏo���T�C�Y(�T�u�R�}���h)	 */

/* MEM_WR */
#define MEM_WR_MAX_SIZE			20				/* �ő发�����݃T�C�Y(���C���R�}���h) */
#define EX_MEM_WR_MAX_SIZE		4				/* �ő发�����݃T�C�Y(�T�u�R�}���h)	 */

/* PRM_RD/PRM_WR/PPRM_WR */
#define RAM_OFFSET 				0x1000			/* RAM�������ݗp�I�t�Z�b�g */
#define EEPROM_OFFSET			0				/* EEPROM�������ݗp�I�t�Z�b�g */

/* ���z������ */
#define	VMEM_VEN_MIN			0x80000000		/* �x���_�[�X�y�V�t�B�b�N�̈� �J�n�A�h���X	*/
#define	VMEM_VEN_MAX			0x8001FFFF		/* �x���_�[�X�y�V�t�B�b�N�̈� �I���A�h���X	*/

/* LTMOD_ON/OFF */
#define MODAL_LATCH			0		/* ���[�_�����b�`�v�� */
#define	MOTION_LATCH		1		/* ���[�V�������b�`�v�� */

/* MOTION CMD */
#define	CMD_MAXTRQ			0x40000	/* �R�}���h�̂s�q�p�w�߂̂l�`�w�l  2^14*/

/* SVCTRL */
#define	SVCTRL_HOLD			0		/* ���[�V�����I�� HOLD */
#define	SVCTRL_INTERPOLATE	1		/* ���[�V�����I�� INTERPOLATE */
#define	SVCTRL_FEED			2		/* ���[�V�����I�� FEED */
#define	SVCTRL_POSING		3		/* ���[�V�����I�� POSING */

#define SVCTRL_SON_SEL		1		/* �V�[�P���X�M�� SON */
#define SVCTRL_BRK_SEL		2		/* �V�[�P���X�M�� BRK */
#define SVCTRL_SEN_SEL		4		/* �V�[�P���X�M�� SEN */
#define SVCTRL_ACLR_SEL		8		/* �V�[�P���X�M�� ACLR */

/* forward declarations (local function) */
static INT32 cmdPrmWrProc(CMD_HNDL* cmdHdl, REG_HNDL* regHdl, USHORT ofst);
static INT32 cmdAlmRdProc(ALM_HNDL* alm_hdl, UCHAR alm_rd_mod, UCHAR idx, UCHAR* alm_data);
static INT32 exCmdPrmWrProc(CMD_HNDL* cmdHdl, REG_HNDL* regHdl, USHORT ofst);
static INT32 cmdChkPrmWr(CMD_LINK_DATA* cmd_buf,CMD_LINK_DATA* rslt_buf);
static INT32 cmdChkConnectCmd(MLNET_HANDLE *hNetData, LONG *reqphs, LONG *dtmod, LONG *com_tim);
static INT32 cmdChkExPrmWr(CMD_LINK_EXDATA* cmd_buf,CMD_LINK_EXDATA* rslt_buf);
static INT32 cmdSvCtrlMotionSelectProc( MLNET_HANDLE *hNetData, CHAR sel_mt);
static INT32 cmdADJ_normal_mode( CMD_HNDL *pCmdHdl,  REG_HNDL *pRegHdl );
static INT32 cmdADJ_sigma2_mode( CMD_HNDL *pCmdHdl,  REG_HNDL *pRegHdl );
static INT32 cmdADJ_tester_mode(CMD_HNDL* hdl);
static INT32 cmdADJ_opIoCmd_mode(CMD_HNDL* hdl);


/****************************************************************************/
/*								���C���R�}���h								*/
/****************************************************************************/

/****************************************************************************/
/* �T�v		�F����`�R�}���h��͏���										*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_WARNG6			�R�}���h�x���U						*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_NO_DEFINE( MLNET_HANDLE *hNetData )
{
	/* �ُ�I����Ԃ� */
	hNetData->cmd_hdl.cmdInfo.rsltBuf = hNetData->cmd_hdl.cmdInfo.cmdBuf;
	return CMD_WARNG6;
}

/****************************************************************************/
/* �T�v		:	���T�|�[�g�R�}���h��͏���									*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	=z CMD_WARNG2			�R�}���h�x���Q						*/
/****************************************************************************/
INT32 cmd_NO_SUPP( MLNET_HANDLE *hNetData )
{
	/* �ُ�I����Ԃ� */
	hNetData->cmd_hdl.cmdInfo.rsltBuf = hNetData->cmd_hdl.cmdInfo.cmdBuf;
	return CMD_WARNG2;
}

/****************************************************************************/
/* �T�v		�F�m�n�o�R�}���h��͏���										*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END		����I��						*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_NOP( MLNET_HANDLE *hNetData )
{
	hNetData->cmd_hdl.cmdInfo.rsltBuf.nop.cmd = hNetData->cmd_hdl.cmdInfo.cmdBuf.nop.cmd;
	return CMD_FUNC_NORMAL_END;
}

/****************************************************************************/
/* �T�v		�F�o�q�l�Q�q�c�R�}���h��͏���									*/
/****************************************************************************/
/* ����		;	CMD_HNDL* hdl		�R�}���h�����							*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END	����I��							*/
/*			:	= CMD_FUNC_CONTINUE		�p�����s��							*/
/*			:	= DATA_SET_WARNG1		�f�[�^�ݒ�x���P	�i�`�D�X�S�`�j	*/
/*			:	= DATA_SET_WARNG4		�f�[�^�ݒ�x���S	�i�`�D�X�S�c�j	*/
/*			:	= CMD_WARNG1			�R�}���h�x���P		�i�`�D�X�T�`�j	*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_PRM_RD( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	UCHAR  		cmd;								/* �R�}���h�R�[�h 		*/
	USHORT 		no;  								/* �p�����[�^�ԍ� 		*/
	UCHAR  		size;								/* �p�����[�^�T�C�Y 	*/
	INT32  		ret  = CMD_FUNC_NORMAL_END;
	
	/* ���[�J���ϐ��̏����� */
	pCmdHdl = &hNetData->cmd_hdl;
	
	cmd = pCmdHdl->cmdInfo.cmdBuf.prm.cmd;
	no  = pCmdHdl->cmdInfo.cmdBuf.prm.no;
	size = pCmdHdl->cmdInfo.cmdBuf.prm.size;

	/* ���s�����`�F�b�N */
	if(  (pCmdHdl->cmdInfo.cmdRdy != FALSE) 
	     && ((pCmdHdl->cmdInfo.rsltBuf.prm.cmd != cmd) 
	     || (pCmdHdl->cmdInfo.rsltBuf.prm.no != no) 
	     || (pCmdHdl->cmdInfo.rsltBuf.prm.size != size)) )
	{
		if(size == 0)/* <S16C> */
		{
			ret = DATA_SET_WARNG4;			/* A.94D �f�[�^�ݒ�x���S */
		}
		else
		{
			/* ���W�X�^�A�N�Z�X�p�f�[�^�ݒ� */
			pCmdHdl->cmdInfo.RegData.AccessSize = size;
			pCmdHdl->cmdInfo.RegData.Length = 1;
			pCmdHdl->cmdInfo.RegData.Number = (no<<1) + VMEM_VEN_MIN + ((RAM_OFFSET)<<1);
			pCmdHdl->cmdInfo.RegData.EepWrite = FALSE;
			pCmdHdl->cmdInfo.RegData.pBuf = (LONG*)pCmdHdl->cmdInfo.rsltBuf.prm.data;
			pCmdHdl->cmdInfo.RegData.BufferSize = sizeof(pCmdHdl->cmdInfo.rsltBuf.prm.data);
	
			/* ���W�X�^�R�}���h�����v�����sAPI	*/
			ret = RegRequestCommand(&hNetData->reg_hdl, REG_CMD_PRMRD, &pCmdHdl->cmdInfo.RegData);
		}

		/* ��͌��ʂ��o�b�t�@�փR�s�[ */
		pCmdHdl->cmdInfo.rsltBuf.prm.cmd  = cmd;
		pCmdHdl->cmdInfo.rsltBuf.prm.no   = no;
		pCmdHdl->cmdInfo.rsltBuf.prm.size = size;
	}
	else
	{
		if(pCmdHdl->cmdInfo.cmdRdy == FALSE)
		{
			ret = RegCheckCommandState(&hNetData->reg_hdl);
		}
	}
	return ret;
}

/****************************************************************************/
/* �T�v		�F�o�q�l�Q�v�q�R�}���h��͏���									*/
/****************************************************************************/
/* ����		;	CMD_HNDL* hdl			�R�}���h�����						*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END		����I��						*/
/*			:	= CMD_FUNC_CONTINUE			�p�����s��						*/
/*			:	= DATA_SET_WARNG1		�f�[�^�ݒ�x���P�i�`�D�X�S�`�j		*/
/*			:	= DATA_SET_WARNG2		�f�[�^�ݒ�x���Q�i�`�D�X�S�a�j		*/
/*			:	= DATA_SET_WARNG3		�f�[�^�ݒ�x���R�i�`�D�X�S�b�j		*/
/*			:	= DATA_SET_WARNG4		�f�[�^�ݒ�x���S�i�`�D�X�S�c�j		*/
/*			:	= CMD_WARNG1			�R�}���h�x���P	�i�`�D�X�T�`�j		*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_PRM_WR( MLNET_HANDLE *hNetData )
{
	USHORT ofst = RAM_OFFSET;
	INT32  ret;

	ret = cmdPrmWrProc(&hNetData->cmd_hdl, &hNetData->reg_hdl ,ofst);
	return ret;
}

/****************************************************************************/
/* �T�v		�F�p�����[�^�������ݎ��s����									*/
/****************************************************************************/
static  INT32 cmdPrmWrProc(CMD_HNDL* cmdHdl, REG_HNDL* regHdl, USHORT ofst)
{
	USHORT no = cmdHdl->cmdInfo.cmdBuf.prm.no;			/* �p�����[�^�ԍ� */
	UCHAR  size = cmdHdl->cmdInfo.cmdBuf.prm.size;		/* �p�����[�^�T�C�Y */
	INT32  ret  = CMD_FUNC_NORMAL_END;

	/* ���s�����`�F�b�N */
	if( (cmdHdl->cmdInfo.cmdRdy != FALSE)
		&& cmdChkPrmWr(&cmdHdl->cmdInfo.cmdBuf, &cmdHdl->cmdInfo.rsltBuf))
	{
		if(size == 0)/* <S16C> */
		{
			ret = DATA_SET_WARNG4;			/* A.94D �f�[�^�ݒ�x���S */
		}
		else
		{
			/* ���W�X�^�A�N�Z�X�p�f�[�^�ݒ� */
			cmdHdl->cmdInfo.RegData.AccessSize = size;
			cmdHdl->cmdInfo.RegData.Length = 1;
			cmdHdl->cmdInfo.RegData.Number = (no<<1) + VMEM_VEN_MIN + ( (ofst) <<1 );
			cmdHdl->cmdInfo.RegData.EepWrite = FALSE;
			cmdHdl->cmdInfo.RegData.pBuf = (LONG*)cmdHdl->cmdInfo.cmdBuf.prm.data;
			cmdHdl->cmdInfo.RegData.BufferSize = sizeof(cmdHdl->cmdInfo.cmdBuf.prm.data);
	
			/* ���W�X�^�R�}���h�����v�����sAPI	*/
			ret = RegRequestCommand(regHdl, REG_CMD_PRMWR, &cmdHdl->cmdInfo.RegData);
		}
		/* ��͌��ʂ��o�b�t�@�փR�s�[ */
		cmdHdl->cmdInfo.rsltBuf.prm.cmd  = cmdHdl->cmdInfo.cmdBuf.prm.cmd;
		cmdHdl->cmdInfo.rsltBuf.prm.no   = no;
		cmdHdl->cmdInfo.rsltBuf.prm.size = size;
		/* �Wbyte���R�s�[(�擪�A�h���X�́A��A�h���X�ł��邽�߁ADWORD�̏������݂͂ł��Ȃ�) */
		UtilSetLLData(cmdHdl->cmdInfo.rsltBuf.prm.data, cmdHdl->cmdInfo.cmdBuf.prm.data);
	}
	else
	{
		if(cmdHdl->cmdInfo.cmdRdy == FALSE)
		{
			ret = RegCheckCommandState(regHdl);
		}
	}
	
	/* �߂�l�ݒ� */
	return ret;
}

/****************************************************************************/
/* �T�v		�F�p�����[�^�������ݎ��s�����`�F�b�N����						*/
/****************************************************************************/
static INT32 cmdChkPrmWr(CMD_LINK_DATA* cmd_buf, CMD_LINK_DATA* rslt_buf)
{
	UCHAR i, sz;

	if(rslt_buf->prm.cmd != cmd_buf->prm.cmd)	return TRUE;
	if(rslt_buf->prm.no != cmd_buf->prm.no)		return TRUE;
	if(rslt_buf->prm.size != cmd_buf->prm.size)	return TRUE;

	sz = (cmd_buf->prm.size >= 8) ? 8: cmd_buf->prm.size;
	for(i = 0; i < sz; i++)
	{
		if(cmd_buf->prm.data[i] != rslt_buf->prm.data[i])
			return TRUE;
	}

	return FALSE;
}

/****************************************************************************/
/* �T�v		:	�h�c�Q�q�c�R�}���h��͏���									*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END		����I��						*/
/*			:	= DATA_SET_WARNG2		�f�[�^�ݒ�x���Q�i�`�D�X�S�a�j		*/
/*			:	= DATA_SET_WARNG4		�f�[�^�ݒ�x���S�i�`�D�X�S�c�j		*/
/****************************************************************************/
INT32 cmd_ID_RD( MLNET_HANDLE *hNetData )
{
	UCHAR		cmd, dev_cod, offset, size;/* <S155> *//* <S**1> */
	LONG		code;/* <S**1> */
	INT32		ret;
	CMD_HNDL	*pCmdHdl;

	/* ���[�J���ϐ��̏����� */
	ret	= CMD_FUNC_NORMAL_END;
	
	pCmdHdl = &hNetData->cmd_hdl;
	
	cmd		= pCmdHdl->cmdInfo.cmdBuf.id_rd.cmd;
	dev_cod = pCmdHdl->cmdInfo.cmdBuf.id_rd.dev_cod;
	offset   = pCmdHdl->cmdInfo.cmdBuf.id_rd.offset;
	size    = pCmdHdl->cmdInfo.cmdBuf.id_rd.size;
	
	/* ���s�����`�F�b�N */
	if( (pCmdHdl->cmdInfo.cmdRdy != FALSE) 
		&& ((pCmdHdl->cmdInfo.rsltBuf.id_rd.cmd != cmd) 
		 || (pCmdHdl->cmdInfo.rsltBuf.id_rd.dev_cod != dev_cod)
		  || (pCmdHdl->cmdInfo.rsltBuf.id_rd.offset  != offset)
		   || (pCmdHdl->cmdInfo.rsltBuf.id_rd.size    != size)) )
	{
		if( size <= ID_RD_MAX_SIZE )
		{
			/* ���W�X�^�A�N�Z�X�p�f�[�^�ݒ� */
			pCmdHdl->cmdInfo.RegData.AccessSize = 1;
			pCmdHdl->cmdInfo.RegData.Length = (LONG)size;
//			pCmdHdl->cmdInfo.RegData.Number = (dev_cod*4) + ofset;/* <S155> */
			code = ( (LONG)(dev_cod & 0xF0) << 1 ) + ( (LONG)(dev_cod & 0x0F) << 3);/* <S155> *//* <S**1> */
			pCmdHdl->cmdInfo.RegData.Number = (code << 1);	/* <S155> *//* <S**1> */
			pCmdHdl->cmdInfo.RegData.offset = (LONG)offset;		/* <S**1> */
			pCmdHdl->cmdInfo.RegData.EepWrite = FALSE;
			pCmdHdl->cmdInfo.RegData.pBuf = (LONG*)pCmdHdl->cmdInfo.rsltBuf.id_rd.id_info;
			pCmdHdl->cmdInfo.RegData.BufferSize = sizeof(pCmdHdl->cmdInfo.rsltBuf.id_rd.id_info);

			ret = RegRequestCommand(&hNetData->reg_hdl, REG_CMD_IDRD, &pCmdHdl->cmdInfo.RegData);
		}
		else
		{
			MlibResetByteMemory( pCmdHdl->cmdInfo.rsltBuf.id_rd.id_info, ID_RD_MAX_SIZE );
			ret = DATA_SET_WARNG4;
		}

		/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
		pCmdHdl->cmdInfo.rsltBuf.id_rd.cmd = cmd;
		pCmdHdl->cmdInfo.rsltBuf.id_rd.dev_cod = dev_cod;
		pCmdHdl->cmdInfo.rsltBuf.id_rd.offset  = offset;
		pCmdHdl->cmdInfo.rsltBuf.id_rd.size    = size;
	}
	else
	{
		if(pCmdHdl->cmdInfo.cmdRdy == FALSE)
		{
			ret = RegCheckCommandState(&hNetData->reg_hdl);
		}
	}
	
	return ret;
}

/****************************************************************************/
/* �T�v		�F�b�n�m�e�h�f�R�}���h��͏���									*/
/****************************************************************************/
/* ����		;	CMD_HNDL* hdl			�R�}���h�����						*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END		����I��						*/
/*			:	= CMD_FUNC_CONTINUE			�p�����s��						*/
/*			:	= CMD_WARNG1				�R�}���h�x���P�i�`�D�X�T�`�j	*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_CONFIG( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	LONG		cmd;
	INT32		result;
	LONG		cmd_switch_time;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl			= &hNetData->cmd_hdl;
	cmd 			= pCmdHdl->cmdInfo.cmdBuf.nop.cmd;
	result			= CMD_FUNC_NORMAL_END;
	cmd_switch_time = FALSE;

/* ���s�����`�F�b�N */
	if(pCmdHdl->cmdInfo.rsltBuf.nop.cmd != cmd)
	{
		pCmdHdl->cmd_seq = SV_OFF_SEQ;	/* �R�}���h�؂�ւ��ŏ����J�n */
		cmd_switch_time = TRUE;
	}
	
	if( (pCmdHdl->cmdInfo.cmdRdy == FALSE) || (cmd_switch_time == TRUE) )
	{
		result	= CMD_FUNC_CONTINUE;
		/* CONFIG�V�[�P���X*/
		if(pCmdHdl->cmd_seq == SV_OFF_SEQ)
		{
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
				pCmdHdl->cmdInfo.chkOpIoCmd = pCmdHdl->cmdInfo.cmdCdataBuf;		/* ADJ(SUBCODE=10H)��CDATA���擾 */
				pCmdHdl->cmd_seq = MT_ENABLE_SEQ;
			}
		}
		else
		{
//			if(0 >= RegCheckCommandState(&hNetData->reg_hdl)) /* <S0EC> */
			result = RegCheckCommandState(&hNetData->reg_hdl);
			if(result != REG_CONTINUE)
			{
				pCmdHdl->cmd_seq = IDOL_SEQ;
//				result = CMD_FUNC_NORMAL_END; /* <S0EC> */
			}					
		}
	
		/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
		pCmdHdl->cmdInfo.rsltBuf.nop.cmd = cmd;
	}
	
	return result;
}

/****************************************************************************/
/* �T�v		�F�`�k�l�Q�q�c�R�}���h��͏���									*/
/****************************************************************************/
/* ����		;	CMD_HNDL* hdl			�R�}���h�����						*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END		����I��						*/
/*			:	= CMD_FUNC_CONTINUE			�p�����s��						*/
/*			:	= DATA_SET_WARNG2			�f�[�^�ݒ�x���Q�i�`�D�X�S�a�j	*/
/*			:	= CMD_WARNG1				�R�}���h�x���P�i�`�D�X�T�`�j	*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_ALM_RD( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	UCHAR		cmd, alm_rd_mod, alm_idx;
	UCHAR		*alm_data;
	INT32 		ret;
	
	/* ���[�J���ϐ��̏����� */
	ret		= CMD_FUNC_NORMAL_END;
	pCmdHdl	= &hNetData->cmd_hdl;

	cmd			= pCmdHdl->cmdInfo.cmdBuf.alm.cmd;		/* �R�}���h�R�[�h 			*/
	alm_rd_mod	= pCmdHdl->cmdInfo.cmdBuf.alm.mode;     /* �A���[���ǂݏo�����[�h	*/
	alm_idx		= pCmdHdl->cmdInfo.cmdBuf.alm.data[0];  /* �A���[���C���f�b�N�X�ԍ� */
    alm_data	= &pCmdHdl->cmdInfo.rsltBuf.alm.data[0];/* �A���[���f�[�^			*/

	/* ���s�����`�F�b�N */
	if( (pCmdHdl->cmdInfo.cmdRdy == FALSE)
		|| (pCmdHdl->cmdInfo.rsltBuf.alm.cmd != cmd) 
		|| (pCmdHdl->cmdInfo.rsltBuf.alm.mode != alm_rd_mod) 
	    || ((alm_rd_mod == ALM_RD_MOD2) && (*alm_data != alm_idx))
	    || ((alm_rd_mod == ALM_RD_MOD3) && (*alm_data != alm_idx)) )
	{
		ret = cmdAlmRdProc(&hNetData->alm_hdl, alm_rd_mod, alm_idx, alm_data);

	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
		pCmdHdl->cmdInfo.rsltBuf.alm.cmd  = cmd;
		pCmdHdl->cmdInfo.rsltBuf.alm.mode = alm_rd_mod;
	}

	return ret;
}


/****************************************************************************/
/* �T�v		�F�A���[���ǂݏo���`�o�h���s����								*/
/****************************************************************************/
static INT32	cmdAlmRdProc(ALM_HNDL* alm_hdl, UCHAR alm_rd_mod, UCHAR idx, UCHAR* alm_data)
{
	INT32 ret;

	switch(alm_rd_mod)
	{
	case ALM_RD_MOD0:	/* ���ݔ����A���[�����[�h */
		ret = AlmGetCrtBufAll(alm_hdl, alm_data);
		break;
	case ALM_RD_MOD1:	/* �A���[���������[�h */
		ret = AlmGetHistoryAll(alm_hdl, alm_data);
		break;
	case ALM_RD_MOD2:	/* ���ݔ����A���[���o�b�t�@�ڍ׏�񃊁[�h */
		*alm_data = idx;
		*(alm_data+7) = 0;
		*(alm_data+8) = 0;
		*(alm_data+9) = 0;
		alm_data++;
		ret = AlmGetCrtBuf(alm_hdl, idx, alm_data);
		break;
	case ALM_RD_MOD3:	/* �ߋ������A���[�������ڍ׏�񃊁[�h */
		*alm_data = idx;
		*(alm_data+7) = 0;
		*(alm_data+8) = 0;
		*(alm_data+9) = 0;
		alm_data++;
		ret = AlmGetHistory(alm_hdl, idx, alm_data);
		break;
	default:
		ret = DATA_SET_WARNG2;
		break;
	}
	return ret;
}

/****************************************************************************/
/* �T�v		�F�`�k�l�Q�b�k�q�R�}���h��͏���								*/
/****************************************************************************/
/* ����		;	CMD_HNDL* hdl			�R�}���h�����						*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END		����I��						*/
/*			:	= CMD_FUNC_CONTINUE			�p�����s��						*/
/*			:	= DATA_SET_WARNG2			�f�[�^�ݒ�x���Q�i�`�D�X�S�a�j	*/
/*			:	= CMD_WARNG1				�R�}���h�x���P�i�`�D�X�T�`�j	*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_ALM_CLR( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	UCHAR		cmd;
	USHORT		alm_clr_mod;
	USHORT 		option;								/* �I�v�V���� 			*/
	INT32		ret;


	/* ���[�J���ϐ��̏����� */
	pCmdHdl	= &hNetData->cmd_hdl;
	
	cmd = pCmdHdl->cmdInfo.cmdBuf.alm.cmd;			/* �R�}���h�R�[�h		*/
	alm_clr_mod = pCmdHdl->cmdInfo.cmdBuf.alm.mode;	/* �A���[���N���A���[�h */
	ret = CMD_FUNC_NORMAL_END;

	/* ���s�����`�F�b�N */
	if( (pCmdHdl->cmdInfo.cmdRdy != FALSE)
	   &&((pCmdHdl->cmdInfo.rsltBuf.alm.cmd  != cmd)
	    || (pCmdHdl->cmdInfo.rsltBuf.alm.mode != alm_clr_mod)) )
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
		pCmdHdl->cmdInfo.rsltBuf.alm.cmd  = cmd;
		pCmdHdl->cmdInfo.rsltBuf.alm.mode = alm_clr_mod;
	}
	else
	{
		if(pCmdHdl->cmdInfo.cmdRdy == FALSE)
		{
			if( alm_clr_mod == ALM_CLR_MOD0 )
			{
				ret = AlmSetClearReq( &hNetData->alm_hdl, FALSE );
			}
			else /* if( alm_clr_mod == ALM_CLR_MOD1 )	*/
			{
				ret = AlmGetAlarmHistoryClearStatus( &hNetData->alm_hdl );
			}
		}
	}

	/*---------------------------------------------------------------*/
	/*Option�t�B�[���h�L��(YEA�˗��̑Ή�)                            */
	/*---------------------------------------------------------------*/
	if((pCmdHdl->cmdInfo.chkOpIoCmd & 0x0001) == 0x0001)
	{
		/* �I�v�V�����w�ߒ��o */
		option = pCmdHdl->cmdInfo.cmdBuf.mt.option;
		MciSetOptionCmd(&hNetData->mt_hdl, option);
	}
	/*---------------------------------------------------------------*/

	/* �߂�l�ݒ� */
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
INT32 cmd_SYNC_SET( MLNET_HANDLE *hNetData )
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

	cmd	= pCmdHdl->cmdInfo.cmdBuf.nop.cmd;
	/* ���s�����`�F�b�N */
	if( (pCmdHdl->cmdInfo.rsltBuf.nop.cmd != cmd) && (pComHdl->Phase == 2) )
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
				pComHdl->NetPrm.tType, pComHdl->NetPrm.aplCycle );
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
		}
		break;
	default:
		result = CMD_FUNC_NORMAL_END;
		break;
	}

	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->cmdInfo.rsltBuf.nop.cmd = cmd;

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
INT32 cmd_CONNECT( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	COM_HNDL	*pComHdl;
	MT_HNDL		*pMtHdl;
	CYC_CHG_PRM	*cycPrm;
	LONG		com_tim, dtmod, reqphs;
	INT32		ret, rc;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl		= &hNetData->cmd_hdl;
	pComHdl		= &hNetData->com_hdl;
	pMtHdl		= &hNetData->mt_hdl;
	com_tim		= pCmdHdl->cmdInfo.cmdBuf.connect.com_time;		/* �ʐM���� */
	rc			= CMD_FUNC_CONTINUE;

	/* ���s�����`�F�b�N */
	if( (pCmdHdl->cmdInfo.cmdRdy) && (pComHdl->Phase == 1) )
	{ /* IDOL��� && Phase=1 �ŏ����J�n */
		pCmdHdl->cmd_seq = REQ_PHSUP_SEQ;
	}

	switch(pCmdHdl->cmd_seq)
	{
	case REQ_PHSUP_SEQ:	/* �t�F�[�Y�A�b�v�v�� */
		ret = cmdChkConnectCmd( hNetData, &reqphs, &dtmod, &com_tim);
		if( ret == CMD_FUNC_NORMAL_END )
		{ /* �R�}���h���펞 */
			if( COM_FUNC_NORMAL_END == 
					ComReqPhaseUp(pComHdl, reqphs, dtmod, com_tim) )
			{ /* ����Ȃ玟�̃V�[�P���X�ցA���s���̓��g���C */
				pCmdHdl->cmd_seq = PHASE_CHK_SEQ;
			}
		}
		else if( ret != CMD_FUNC_CONTINUE )
		{ /* �ُ펞 */
			rc = ret;
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
		cycPrm->intCyc = pComHdl->NetPrm.tCycle;
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
		}
		break;
	case FAIL_END_SEQ:		/* �V�[�P���X���s */
		if( MciEnableState(pMtHdl, TRUE) == MCI_SUCCESS )
		{
			pCmdHdl->cmd_seq = IDOL_SEQ;
			rc = CMD_SYNC_FAULT;
		}
		break;
	default:
		rc = CMD_FUNC_NORMAL_END;
		break;
	}
	
	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->cmdInfo.rsltBuf.connect.cmd =
					pCmdHdl->cmdInfo.cmdBuf.connect.cmd;
	pCmdHdl->cmdInfo.rsltBuf.connect.ver =
					pCmdHdl->cmdInfo.cmdBuf.connect.ver;
	pCmdHdl->cmdInfo.rsltBuf.connect.com_mod =
					pCmdHdl->cmdInfo.cmdBuf.connect.com_mod;
	pCmdHdl->cmdInfo.rsltBuf.connect.com_time =
					pCmdHdl->cmdInfo.cmdBuf.connect.com_time;

	return rc;
}


/****************************************************************************/
/* �T�v		�F�b�n�m�m�d�b�s�R�}���h�f�[�^�`�F�b�N����						*/
/****************************************************************************/
static INT32 cmdChkConnectCmd(MLNET_HANDLE *hNetData, LONG* reqphs, LONG* dtmod, LONG* com_tim)
{
	CMD_HNDL	*pCmdHdl;
	COM_HNDL	*pComHdl;
	UCHAR		ver, com_mod;
	ULONG		t_spd, t_byte;/* <S052>:USHORT -> ULONG */
	BOOL		sv_on;
	INT32		ret;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl	= &hNetData->cmd_hdl;
	pComHdl	= &hNetData->com_hdl;
	ver     = pCmdHdl->cmdInfo.cmdBuf.connect.ver;		/* �o�[�W����		*/
	com_mod = pCmdHdl->cmdInfo.cmdBuf.connect.com_mod;	/* �ʐM���[�h		*/
	t_spd	= (ULONG)pComHdl->NetPrm.tSpeed;			/* �ʐM���x 4/10	*/
	t_byte	= (ULONG)pComHdl->NetPrm.dataSize;			/* �`���o�C�g��		*/
	sv_on	= hNetData->mt_hdl.svRsp->RspSeqBit & SERVO_ON_COMP_BITMSK;/* �T�[�{�I����� 0:OFF,1:ON */
	ret		= CMD_FUNC_NORMAL_END;

/* ���s�����`�F�b�N */
	if(sv_on != 0)
	{
		return  CMD_WARNG1;
	}

	if(ver == 0x10)
	{/* MECHATROLINK-I	*/
		if(t_spd != 4)
		{/* �ʐM���x�s����	*/
			return  DATA_SET_WARNG2;
		}
		if(*com_tim % 2)
		{/* �ʐM�����ُ�	*/
			return  DATA_SET_WARNG2;
		}
		if((*com_tim < 2) || (*com_tim > 32))
		{/* �ʐM�����ُ�	*/
			return  DATA_SET_WARNG2;
		}
		/* �ʐM������`�������P��[2ms�P��]�ɕϊ� */
		*com_tim = *com_tim >> 1;
		/* �T�u�R�}���h�g�p�^���g�p�ݒ� */
		pCmdHdl->useExCmd = FALSE;
		/* �v���t�F�[�Y�ݒ� */
		if((com_mod & 0x03) == 0x2)
		{
			*reqphs = 3;
		}
		else
		{
			*reqphs = 2;
		
		}
	}
	else if(ver == 0x21)
	{/* MECHATROLINK-II	*/
		if(t_spd != 10)
		{/* �ʐM���x�s����	*/
			return  DATA_SET_WARNG2;
		}
		if((com_mod & 0x80) && (t_byte == 17))
		{/* SubCmd,�`���޲Đ� �s����	*/
			return  DATA_SET_WARNG2;
		}
		if((*com_tim < 1) || (*com_tim > 32))
		{/* �ʐM�����ُ�	*/
			return  DATA_SET_WARNG2;
		}
		/* �T�u�R�}���h�g�p�^���g�p�ݒ� */
		pCmdHdl->useExCmd = (com_mod >> 7) & 0x1;
		/* �v���t�F�[�Y�ݒ� */
		if((com_mod & 0x02) == 0x2)
		{
			*reqphs = 3;
		}
		else
		{
			*reqphs = 2;
		}
	}
	else
	{
		return  DATA_SET_WARNG2;
	}

	/* �`�����[�h�ݒ� */
	com_mod = (com_mod >> 2) & 0x3;
	if(com_mod == 0x1)
	{/* 01b: �A�����[�h */
		if(*com_tim < 2)
		{
		 	return DATA_SET_WARNG2;	/* 2�{�����Ȃ�NG */
		}
		*dtmod = CONSECUTIVE_TRANSFER;
	}
	else if(com_mod == 0x2)
	{/* 10b:�������[�h */
		return  DATA_SET_WARNG2;
	}
	else
	{/* 00b,11b: �P�����[�h */
		*dtmod = SINGLE_TRANSFER;
	}

	return ret;
}

/****************************************************************************/
/* �T�v		�F�c�h�r�b�n�m�m�d�b�s�R�}���h��͏���							*/
/****************************************************************************/
/* ����		;	CMD_HNDL* hdl			�R�}���h�����						*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END		����I��						*/
/*			:	= CMD_FUNC_CONTINUE			�p�����s��						*/
/*			:	= CMD_WARNG1				�R�}���h�x���P�i�`�D�X�T�`�j	*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_DISCONNECT( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	COM_HNDL	*pComHdl;
	UCHAR		cmd;
	INT32		result;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl	= &hNetData->cmd_hdl;
	pComHdl	= &hNetData->com_hdl;
	cmd     = pCmdHdl->cmdInfo.cmdBuf.nop.cmd;		/* �R�}���h�R�[�h */
	result	= CMD_FUNC_CONTINUE;

	/* ���s�����`�F�b�N */
	if( (pCmdHdl->cmdInfo.rsltBuf.nop.cmd != cmd)
		&& ((pComHdl->Phase > 1) || (pComHdl->reqPhase > 1)) )
	{/* �R�}���h�؂�ւ�� && (����Phase2�ȏ� or PhaseUP��) */
		pCmdHdl->cmd_seq = REQ_PHSDOWN_SEQ;
	}
	
	/* DISCONNECT�������s */
	result = CmdRunDisConnect(hNetData);

	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->cmdInfo.rsltBuf.nop.cmd = pCmdHdl->cmdInfo.cmdBuf.nop.cmd;

	return result;
}

/****************************************************************************/
/* �T�v		�F�o�o�q�l�Q�v�q�R�}���h��͏���								*/
/****************************************************************************/
/* ����		;	CMD_HNDL* hdl			�R�}���h�����						*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END		����I��						*/
/*			:	= CMD_FUNC_CONTINUE			�p�����s��						*/
/*			:	= DATA_SET_WARNG1			�f�[�^�ݒ�x���P�i�`�D�X�S�`�j	*/
/*			:	= DATA_SET_WARNG2			�f�[�^�ݒ�x���Q�i�`�D�X�S�a�j	*/
/*			:	= DATA_SET_WARNG3			�f�[�^�ݒ�x���R�i�`�D�X�S�b�j	*/
/*			:	= DATA_SET_WARNG4			�f�[�^�ݒ�x���S�i�`�D�X�S�c�j	*/
/*			:	= CMD_WARNG1				�R�}���h�x���P	�i�`�D�X�T�`�j	*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_PPRM_WR( MLNET_HANDLE *hNetData )
{
	USHORT ofst = EEPROM_OFFSET;
	INT32  ret;

	ret = cmdPrmWrProc(&hNetData->cmd_hdl, &hNetData->reg_hdl, ofst);
	return ret;
}

/****************************************************************************/
/* �T�v		�F�o�n�r�Q�r�d�s�R�}���h��͏���								*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END		����I��						*/
/*			:	= DATA_SET_WARNG2			�f�[�^�ݒ�x���Q�i�`�D�X�S�a�j	*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_POS_SET( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	UCHAR 		ps_cmd;									/* ���W�ݒ胂�[�h */
	BOOL 		refe;									/* ���t�@�����X�_ */
	LONG		pos_data;								/* �ݒ�ʒu */
	INT32		ret;
	USHORT		option;									/* �I�v�V���� */

	/* ���[�J���ϐ��̏����� */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;

	ps_cmd = pCmdHdl->cmdInfo.cmdBuf.pos_set.ps_cmd;
	refe   = (BOOL)(ps_cmd & 0x80) >> 7;
	pos_data = UtilGetDWData(pCmdHdl->cmdInfo.cmdBuf.pos_set.pos);	/* Long�ް��ɂ���B	*/

	if((ps_cmd & 0xf) == 0x03)
	{/* ���W�n�ݒ�=APOS�̏ꍇ */
		ret = MciSetCordinatePos(pMtHdl, pos_data, refe);		/* ���W�n�ݒ� */
	}
	else
	{/* ���W�n�ݒ�!=APOS�̏ꍇ */
		ret = DATA_SET_WARNG2;
	}

	/*---------------------------------------------------------------*/
	/*Option�t�B�[���h�L��(YEA�˗��̑Ή�)                            */
	/*---------------------------------------------------------------*/
	if((pCmdHdl->cmdInfo.chkOpIoCmd & 0x0001) == 0x0001)
	{
		/* �I�v�V�����w�ߒ��o */
		option = pCmdHdl->cmdInfo.cmdBuf.mt.option;
		MciSetOptionCmd(pMtHdl, option);
	}
	/*---------------------------------------------------------------*/

	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->cmdInfo.rsltBuf.pos_set.cmd    = pCmdHdl->cmdInfo.cmdBuf.pos_set.cmd;
	pCmdHdl->cmdInfo.rsltBuf.pos_set.ps_cmd = ps_cmd;
	UtilSetDWData(pCmdHdl->cmdInfo.rsltBuf.pos_set.pos, pos_data);	/* ��A�h���X�ɃR�s�[	*/

	return ret;
}

/****************************************************************************/
/* �T�v		�F�a�q�j�Q�n�m�R�}���h��͏���									*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END		����I��						*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_BRK_ON( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	USHORT		option;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;
	
	/* ���s�����`�F�b�N */
	if(pCmdHdl->cmdInfo.rsltBuf.mt.cmd != pCmdHdl->cmdInfo.cmdBuf.mt.cmd) 
	{/* �a�j�w�߃I�� */
		pMtHdl->svCmd->CmdSeqBit &= ~(BRK_RELEASE_BIT);
	}
	/*---------------------------------------------------------------*/
	/*Option�t�B�[���h�L��(YEA�˗��̑Ή�)                            */
	/*---------------------------------------------------------------*/
	if((pCmdHdl->cmdInfo.chkOpIoCmd & 0x0001) == 0x0001)
	{
		/* �I�v�V�����w�ߒ��o */
		option = pCmdHdl->cmdInfo.cmdBuf.mt.option;
		MciSetOptionCmd(pMtHdl, option);
	}
	/*---------------------------------------------------------------*/
	
	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->cmdInfo.rsltBuf.mt.cmd       = pCmdHdl->cmdInfo.cmdBuf.mt.cmd;
	pCmdHdl->cmdInfo.rsltBuf.mt.sel_mon12 = pCmdHdl->cmdInfo.cmdBuf.mt.sel_mon12;

	return CMD_FUNC_NORMAL_END;
}

/****************************************************************************/
/* �T�v		�F�a�q�j�Q�n�e�e�R�}���h��͏���								*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END		����I��						*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_BRK_OFF( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	USHORT 		option;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;

	/* ���s�����`�F�b�N */
	if(pCmdHdl->cmdInfo.rsltBuf.mt.cmd != pCmdHdl->cmdInfo.cmdBuf.mt.cmd) 
	{/* �a�j�w�߃I�t */
		pMtHdl->svCmd->CmdSeqBit |= BRK_RELEASE_BIT;
	}
	/*---------------------------------------------------------------*/
	/*Option�t�B�[���h�L��(YEA�˗��̑Ή�)                            */
	/*---------------------------------------------------------------*/
	if((pCmdHdl->cmdInfo.chkOpIoCmd & 0x0001) == 0x0001)
	{
		/* �I�v�V�����w�ߒ��o */
		option = pCmdHdl->cmdInfo.cmdBuf.mt.option;
		MciSetOptionCmd(pMtHdl, option);
	}
	/*---------------------------------------------------------------*/
	
	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->cmdInfo.rsltBuf.mt.cmd       = pCmdHdl->cmdInfo.cmdBuf.mt.cmd;
	pCmdHdl->cmdInfo.rsltBuf.mt.sel_mon12 = pCmdHdl->cmdInfo.cmdBuf.mt.sel_mon12;
	
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
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_SENS_ON( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	INT32  		ret;
	USHORT 		option;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;


	/* �Z���T�I���w�߃Z�b�g */
	ret = MciSetSensOn(pMtHdl, TRUE);

	/*---------------------------------------------------------------*/
	/*Option�t�B�[���h�L��(YEA�˗��̑Ή�)                            */
	/*---------------------------------------------------------------*/
	if((pCmdHdl->cmdInfo.chkOpIoCmd & 0x0001) == 0x0001)
	{
		/* �I�v�V�����w�ߒ��o */
		option = pCmdHdl->cmdInfo.cmdBuf.mt.option;
		MciSetOptionCmd(pMtHdl, option);
	}
	/*---------------------------------------------------------------*/

	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->cmdInfo.rsltBuf.mt.cmd       = pCmdHdl->cmdInfo.cmdBuf.mt.cmd;
	pCmdHdl->cmdInfo.rsltBuf.mt.sel_mon12 = pCmdHdl->cmdInfo.cmdBuf.mt.sel_mon12;
	
	return ret;
}

/****************************************************************************/
/* �T�v		�F�r�d�m�r�Q�n�e�e�R�}���h��͏���								*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END		����I��						*/
/*			:	= CMD_FUNC_CONTINUE			�p�����s��						*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_SENS_OFF( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	USHORT	 	option;
	INT32		ret;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;
	ret			= CMD_FUNC_NORMAL_END;

	/* �Z���T�I�t�w�߃Z�b�g */
	if(pCmdHdl->cmdInfo.rsltBuf.mt.cmd != pCmdHdl->cmdInfo.cmdBuf.mt.cmd)
	{
		ret = MciSetSensOn(pMtHdl, FALSE);
	}
	
	/*---------------------------------------------------------------*/
	/*Option�t�B�[���h�L��(YEA�˗��̑Ή�)                            */
	/*---------------------------------------------------------------*/
	if((pCmdHdl->cmdInfo.chkOpIoCmd & 0x0001) == 0x0001)
	{
		/* �I�v�V�����w�ߒ��o */
		option = pCmdHdl->cmdInfo.cmdBuf.mt.option;
		MciSetOptionCmd(pMtHdl, option);
	}
	/*---------------------------------------------------------------*/

	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->cmdInfo.rsltBuf.mt.cmd       = pCmdHdl->cmdInfo.cmdBuf.mt.cmd;
	pCmdHdl->cmdInfo.rsltBuf.mt.sel_mon12 = pCmdHdl->cmdInfo.cmdBuf.mt.sel_mon12;
	
	return ret;
}

/****************************************************************************/
/* �T�v		�F�g�n�k�c�R�}���h��͏���								(25H)	*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	�F	����I��	:	CMD_FUNC_NORMAL_END							*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_HOLD( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MCI_CMD 	cmd;
	INT32		ret;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl		= &hNetData->cmd_hdl;
	ret			= CMD_FUNC_NORMAL_END;


	/* �e�f�[�^�𒊏o */
	cmd.HoldMod   = (UCHAR)pCmdHdl->cmdInfo.cmdBuf.mt.pos1;	/* Hold���[�h�Z�b�g */
	cmd.TrqLimitP = CMD_MAXTRQ;								/* TRQ LIMIT�Z�b�g  */
	cmd.TrqLimitN = CMD_MAXTRQ;								/* TRQ LIMIT�Z�b�g  */
	cmd.Option    = pCmdHdl->cmdInfo.cmdBuf.mt.option;		/* Option�Z�b�g     */

	/* �g�n�k�c�R�}���h�`�o�h�֐����s */
	MciSetHoldCmd(&hNetData->mt_hdl, &cmd);

	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->cmdInfo.rsltBuf.mt.cmd       = pCmdHdl->cmdInfo.cmdBuf.mt.cmd;
	pCmdHdl->cmdInfo.rsltBuf.mt.sel_mon12 = pCmdHdl->cmdInfo.cmdBuf.mt.sel_mon12;

	return CMD_FUNC_NORMAL_END;
}

/****************************************************************************/
/* �T�v		�F�k�s�l�n�c�Q�n�m�R�}���h��͏���								*/
/****************************************************************************/
/* ����		;	CMD_HNDL* hdl		�R�}���h�����							*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END	����I��							*/
/*			:	= LTMOD_ON_CMD_WARNG4	�R�}���h�x���S�i�`�D�X�T�c�j		*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_LTMOD_ON( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	LONG  lt_sgn;
	UCHAR lt_mod;
	INT32  ret;
	USHORT option;										/* �I�v�V����		 */

	/* ���[�J���ϐ��̏����� */
	ret		= CMD_FUNC_NORMAL_END;
	pCmdHdl	= &hNetData->cmd_hdl;
	pMtHdl	= &hNetData->mt_hdl;
	
	lt_sgn = (LONG)pCmdHdl->cmdInfo.cmdBuf.mt.lt_sgn & 0x3;
	lt_mod = (UCHAR)pCmdHdl->cmdInfo.cmdBuf.mt.pos1;
	
	/* ���b�`�v�� */
	ret = MciRequestMdlLatch(pMtHdl, lt_mod, lt_sgn);

	/*---------------------------------------------------------------*/
	/*Option�t�B�[���h�L��(YEA�˗��̑Ή�)                            */
	/*---------------------------------------------------------------*/
	if((pCmdHdl->cmdInfo.chkOpIoCmd & 0x0001) == 0x0001)
	{
		/* �I�v�V�����w�ߒ��o */
		option = pCmdHdl->cmdInfo.cmdBuf.mt.option;
		MciSetOptionCmd(pMtHdl, option);
	}
	/*---------------------------------------------------------------*/

	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->cmdInfo.rsltBuf.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.mt.cmd;
	pCmdHdl->cmdInfo.rsltBuf.mt.sel_mon12 = pCmdHdl->cmdInfo.cmdBuf.mt.sel_mon12;

	return ret;
}

/****************************************************************************/
/* �T�v		�F�k�s�l�n�c�Q�n�e�e�R�}���h��͏���							*/
/****************************************************************************/
/* ����		;	CMD_HNDL* hdl		�R�}���h�����							*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END	����I��							*/
/*			:	= LTMOD_ON_CMD_WARNG4	�R�}���h�x���S�i�`�D�X�T�c�j		*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_LTMOD_OFF( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	INT32  ret;
	USHORT option;											/* �I�v�V���� 	*/

	/* ���[�J���ϐ��̏����� */
	ret		= CMD_FUNC_NORMAL_END;
	pCmdHdl	= &hNetData->cmd_hdl;
	pMtHdl	= &hNetData->mt_hdl;
	
	/* ���b�`�L�����Z�� */
	ret = MciCancelLatch(pMtHdl, MODAL_LTMD);
	
	/*---------------------------------------------------------------*/
	/*Option�t�B�[���h�L��(YEA�˗��̑Ή�)                            */
	/*---------------------------------------------------------------*/
	if((pCmdHdl->cmdInfo.chkOpIoCmd & 0x0001) == 0x0001)
	{
		/* �I�v�V�����w�ߒ��o */
		option = pCmdHdl->cmdInfo.cmdBuf.mt.option;
		MciSetOptionCmd(pMtHdl, option);
	}
	/*---------------------------------------------------------------*/

	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->cmdInfo.rsltBuf.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.mt.cmd;
	pCmdHdl->cmdInfo.rsltBuf.mt.sel_mon12 = pCmdHdl->cmdInfo.cmdBuf.mt.sel_mon12;
	
	return ret;
}

/****************************************************************************/
/* �T�v		�F�r�l�n�m�R�}���h��͏���										*/
/****************************************************************************/
/* ����		;	CMD_HNDL* hdl		�R�}���h�����							*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END	����I��							*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_SMON( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	USHORT 		option;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;

	/*---------------------------------------------------------------*/
	/*Option�t�B�[���h�L��(YEA�˗��̑Ή�)                            */
	/*---------------------------------------------------------------*/
	if((pCmdHdl->cmdInfo.chkOpIoCmd & 0x0001) == 0x0001)
	{
		/* �I�v�V�����w�ߒ��o */
		option = pCmdHdl->cmdInfo.cmdBuf.mt.option;
		MciSetOptionCmd(pMtHdl, option);
	}
	/*---------------------------------------------------------------*/

	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->cmdInfo.rsltBuf.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.mt.cmd;
	pCmdHdl->cmdInfo.rsltBuf.mt.sel_mon12 = pCmdHdl->cmdInfo.cmdBuf.mt.sel_mon12;

	return CMD_FUNC_NORMAL_END;
}

/****************************************************************************/
/* �T�v		�F�r�u�Q�n�m�R�}���h��͏���									*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END		����I��						*/
/*			:	= CMD_FUNC_CONTINUE			�p�����s��						*/
/*			:	= CMD_WARNG1				�R�}���h�x���P�i�`�D�X�T�`�j	*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_SV_ON( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	USHORT 		option;
	BOOL		reset;
	INT32		ret;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl	= &hNetData->cmd_hdl;
	pMtHdl	= &hNetData->mt_hdl;
	ret		= CMD_FUNC_NORMAL_END;
	

	/* �T�[�{�I���w�߃Z�b�g */
	reset = (pCmdHdl->cmdInfo.rsltBuf.mt.cmd != pCmdHdl->cmdInfo.cmdBuf.mt.cmd);
	ret = MciSetServoOn(pMtHdl, TRUE, reset);
	
	if(ret != MCI_CNDTN_ERR)
	{	/* �ُ�I���łȂ���΁A�I�v�V�����w�߃Z�b�g */
		option = pCmdHdl->cmdInfo.cmdBuf.mt.option;
		MciSetOptionCmd(pMtHdl, option);
	}
	
	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->cmdInfo.rsltBuf.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.mt.cmd;
	pCmdHdl->cmdInfo.rsltBuf.mt.sel_mon12 = pCmdHdl->cmdInfo.cmdBuf.mt.sel_mon12;

	return ret;
}

/****************************************************************************/
/* �T�v		�F�r�u�Q�n�e�e�R�}���h��͏���									*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END	����I��							*/
/*			:	= CMD_FUNC_CONTINUE		�p�����s��							*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_SV_OFF( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	USHORT		option;
	INT32		ret;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;
	
	/* �T�[�{�I�t�w�߃Z�b�g */
	ret = MciSetServoOn(pMtHdl, FALSE, TRUE);

	/*---------------------------------------------------------------*/
	/*Option�t�B�[���h�L��(YEA�˗��̑Ή�)                            */
	/*---------------------------------------------------------------*/
	if((pCmdHdl->cmdInfo.chkOpIoCmd & 0x0001) == 0x0001)
	{/* �I�v�V�����w�ߒ��o */
		option = pCmdHdl->cmdInfo.cmdBuf.mt.option;
		MciSetOptionCmd(pMtHdl, option);
	}
	/*---------------------------------------------------------------*/

	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->cmdInfo.rsltBuf.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.mt.cmd;
	pCmdHdl->cmdInfo.rsltBuf.mt.sel_mon12 = pCmdHdl->cmdInfo.cmdBuf.mt.sel_mon12;

	return ret;
}


/****************************************************************************/
/* �T�v		�F�h�m�s�d�q�o�n�k�`�s�d�R�}���h��͏���				(34H)	*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	����I��	:	CMD_FUNC_NORMAL_END							*/
/*				�����ُ�	:	IPOLATE_CMD_WARNING1						*/
/*				�f�[�^�ُ�	:	IPOLATE_CMD_DATA_SET_WARNING2				*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_INTERPOLATE( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	MCI_CMD		cmd;
	INT32		result;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;

	/* �e�f�[�^�𒊏o */
	cmd.TPos      = pCmdHdl->cmdInfo.cmdBuf.mt.pos1;		/* TPOS�Z�b�g       */
	cmd.Velocity  = pCmdHdl->cmdInfo.cmdBuf.mt.pos2;		/* VFF/���x�w�� [ref/s]/[OS/2^24] */
	cmd.Trq       = (LONG)UtilGetWData(pCmdHdl->cmdInfo.cmdBuf.mt.iocmd);
	cmd.TrqLimitP = cmd.TrqLimitN = cmd.Trq;				/* TLIM�Z�b�g		*/
	cmd.Option    = pCmdHdl->cmdInfo.cmdBuf.mt.option;		/* Option�Z�b�g     */

	/* �ʐM���[�j���O���̕␳���� */
	if( (hNetData->com_hdl.recvErrCnt == COM_RCV_WRN_STS)
		&& (pMtHdl->crtMtype == MTYPE_INTERPOL) )
	{/* �ʐM���[�j���O && ���݃��[�V����=INTERPOLATE */
		cmd.TPos += pMtHdl->outData.DposIn;
	}

	/* �h�m�s�d�q�o�n�k�`�s�d�R�}���h�`�o�h�֐����s */
	result = MciSetInterpolateCmd(pMtHdl, &cmd);

	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->cmdInfo.rsltBuf.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.mt.cmd;
	pCmdHdl->cmdInfo.rsltBuf.mt.sel_mon12 = pCmdHdl->cmdInfo.cmdBuf.mt.sel_mon12;

	return result;
}

/****************************************************************************/
/* �T�v		�F�o�n�r�h�m�f�R�}���h��͏���							(35H)	*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	����I��	:	CMD_FUNC_NORMAL_END							*/
/*				�����ُ�	:	POSING_CMD_WARNING1							*/
/*				�f�[�^�ُ�	:	POSING_CMD_DATA_SET_WARNING2				*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_POSING( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	MCI_CMD		cmd;
	USHORT		buf;
	INT32		result;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;
	
	/* �e�f�[�^�𒊏o */
	cmd.TPos      = pCmdHdl->cmdInfo.cmdBuf.mt.pos1;	/* TPOS�Z�b�g       */
	cmd.TSpeed    = pCmdHdl->cmdInfo.cmdBuf.mt.pos2;	/* TSPD�Z�b�g       */
	buf           = (USHORT)UtilGetWData(pCmdHdl->cmdInfo.cmdBuf.mt.iocmd);
	cmd.TrqLimitP = cmd.TrqLimitN = (ULONG)buf;			/* TLIM�Z�b�g		*/
	cmd.Trq       = 0;									/* TFF�Z�b�g		*/
	cmd.Option    = pCmdHdl->cmdInfo.cmdBuf.mt.option;	/* Option�Z�b�g     */
	cmd.RstReq    = (pCmdHdl->cmdInfo.rsltBuf.mt.cmd != pCmdHdl->cmdInfo.cmdBuf.mt.cmd);
	cmd.AccRate   = (LONG)buf;							/* ACC_TIME�Z�b�g	*/
	cmd.EnableRate = TRUE;								/* ACC_TIME�L��		*/

	/* �o�n�r�h�m�f�R�}���h�`�o�h�֐����s */
	result = MciSetPosingCmd(pMtHdl, &cmd);

	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->cmdInfo.rsltBuf.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.mt.cmd;
	pCmdHdl->cmdInfo.rsltBuf.mt.sel_mon12 = pCmdHdl->cmdInfo.cmdBuf.mt.sel_mon12;

	return result;
}

/****************************************************************************/
/* �T�v		�F�e�d�d�c�R�}���h��͏���								(36H)	*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	����I��	:	CMD_FUNC_NORMAL_END							*/
/*				�����ُ�	:	FEED_CMD_WARNING1							*/
/*				�f�[�^�ُ�	:	FEED_CMD_DATA_SET_WARNING2					*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_FEED( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	MCI_CMD		cmd;
	USHORT		buf;
	INT32		result;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;
	
	/* �e�f�[�^�𒊏o */
	cmd.TSpeed    = pCmdHdl->cmdInfo.cmdBuf.mt.pos2;	/* TSPD�Z�b�g       */
	buf           = (USHORT)UtilGetWData(pCmdHdl->cmdInfo.cmdBuf.mt.iocmd);
	cmd.TrqLimitP = cmd.TrqLimitN = (ULONG)buf;			/* TLIM�Z�b�g		*/
	cmd.Trq       = 0;									/* TFF�Z�b�g		*/
	cmd.Option    = pCmdHdl->cmdInfo.cmdBuf.mt.option;	/* Option�Z�b�g     */
	cmd.RstReq    = (pCmdHdl->cmdInfo.rsltBuf.mt.cmd != pCmdHdl->cmdInfo.cmdBuf.mt.cmd);
	cmd.AccRate   = (LONG)buf;							/* ACC_TIME�Z�b�g */
	cmd.EnableRate = FALSE;								/* ACC_TIME�L�� */

	/* �e�d�d�c�R�}���h�`�o�h�֐����s */
	result = MciSetFeedCmd(pMtHdl, &cmd);

	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->cmdInfo.rsltBuf.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.mt.cmd;
	pCmdHdl->cmdInfo.rsltBuf.mt.sel_mon12 = pCmdHdl->cmdInfo.cmdBuf.mt.sel_mon12;

	return result;
}

/****************************************************************************/
/* �T�v		�F�k�`�s�b�g�R�}���h��͏���							(38H)	*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	�F	����I��	:	CMD_FUNC_NORMAL_END							*/
/*				�����ُ�	:	LATCH_CMD_WARNING1							*/
/*				�ް��ُ�	:	LATCH_CMD_DATA_SET_WARNING2					*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_LATCH( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	MCI_CMD		cmd;
	INT32		result;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;

	/* �e�f�[�^�𒊏o */
	cmd.LatchSgnl = pCmdHdl->cmdInfo.cmdBuf.mt.lt_sgn & 0x3;/* LT_SGN�Z�b�g	*/
	cmd.TPos      = pCmdHdl->cmdInfo.cmdBuf.mt.pos1;		/* TPOS�Z�b�g 	*/
	cmd.Velocity  = pCmdHdl->cmdInfo.cmdBuf.mt.pos2;		/* VFF/���x�w�� [ref/s]/[OS/2^24] */
	cmd.Trq       = (LONG)UtilGetWData(pCmdHdl->cmdInfo.cmdBuf.mt.iocmd);
	cmd.TrqLimitP = cmd.TrqLimitN = cmd.Trq;				/* TLIM�Z�b�g	*/
	cmd.Option    = pCmdHdl->cmdInfo.cmdBuf.mt.option;		/* Option�Z�b�g */

	/* �ʐM���[�j���O���̕␳���� */
	if( (hNetData->com_hdl.recvErrCnt == COM_RCV_WRN_STS)
		&& (pMtHdl->crtMtype == MTYPE_INTERPOL) )
	{/* �ʐM���[�j���O && ���݃��[�V����=INTERPOLATE */
		cmd.TPos += pMtHdl->outData.DposIn;
	}

	/* �k�`�s�b�g�R�}���h�`�o�h�֐����s */
	result = MciSetLatchCmd(pMtHdl, &cmd);

	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->cmdInfo.rsltBuf.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.mt.cmd;
	pCmdHdl->cmdInfo.rsltBuf.mt.sel_mon12 = pCmdHdl->cmdInfo.cmdBuf.mt.sel_mon12;

	return result;
}

/****************************************************************************/
/* �T�v		�F�d�w�Q�o�n�r�h�m�f�R�}���h��͏���					(39H)	*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	�F	����I��	:	CMD_FUNC_NORMAL_END							*/
/*				�����ُ�	:	EX_POSING_CMD_WARNING1						*/
/*				�ް��ُ�	:	EX_POSING_CMD_DATA_SET_WARNING2				*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_EX_POSING( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	MCI_CMD		cmd;
	USHORT		buf;
	INT32		result;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;

	/* �e�f�[�^�𒊏o */
	cmd.LatchSgnl = pCmdHdl->cmdInfo.cmdBuf.mt.lt_sgn & 0x3;/* LT_SGN�Z�b�g */
	cmd.TPos      = pCmdHdl->cmdInfo.cmdBuf.mt.pos1;	/* TPOS�Z�b�g       */
	cmd.TSpeed    = pCmdHdl->cmdInfo.cmdBuf.mt.pos2;	/* TSPD�Z�b�g       */
	buf           = (USHORT)UtilGetWData(pCmdHdl->cmdInfo.cmdBuf.mt.iocmd);
	cmd.TrqLimitP = cmd.TrqLimitN = (ULONG)buf;			/* TLIM�Z�b�g		*/
	cmd.Trq       = 0;									/* TFF�Z�b�g		*/
	cmd.Option    = pCmdHdl->cmdInfo.cmdBuf.mt.option;	/* Option�Z�b�g     */
	cmd.RstReq    = (pCmdHdl->cmdInfo.rsltBuf.mt.cmd != pCmdHdl->cmdInfo.cmdBuf.mt.cmd);
	cmd.AccRate   = (LONG)buf;							/* ACC_TIME�Z�b�g 	*/
	cmd.EnableRate = TRUE;								/* ACC_TIME�L�� 	*/

	/* �d�w�Q�o�n�r�h�m�f�R�}���h�`�o�h�֐����s */
	result = MciSetExPosingCmd(pMtHdl, &cmd);

	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->cmdInfo.rsltBuf.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.mt.cmd;
	pCmdHdl->cmdInfo.rsltBuf.mt.sel_mon12 = pCmdHdl->cmdInfo.cmdBuf.mt.sel_mon12;

	return result;
}

/****************************************************************************/
/* �T�v		�F�y�q�d�s�R�}���h��͏���								(3AH)	*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	�F	����I��	:	CMD_FUNC_NORMAL_END							*/
/*				�����ُ�	:	ZRET_CMD_WARNING1							*/
/*				�ް��ُ�	:	ZRET_CMD_DATA_SET_WARNING2					*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_ZRET( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	MCI_CMD		cmd;
	USHORT		buf;
	INT32		result;
	
	/* ���[�J���ϐ��̏����� */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;

	/* �e�f�[�^�𒊏o */
	cmd.LatchSgnl = pCmdHdl->cmdInfo.cmdBuf.mt.lt_sgn & 0x3;/* LT_SGN�Z�b�g     */
	cmd.TSpeed    = pCmdHdl->cmdInfo.cmdBuf.mt.pos2;		/* TSPD�Z�b�g       */
	buf           = (USHORT)UtilGetWData(pCmdHdl->cmdInfo.cmdBuf.mt.iocmd);
	cmd.TrqLimitP = cmd.TrqLimitN = (ULONG)buf;				/* TLIM�Z�b�g		*/
	cmd.Trq       = 0;										/* TFF�Z�b�g		*/
	cmd.Option    = pCmdHdl->cmdInfo.cmdBuf.mt.option;		/* Option�Z�b�g     */
	cmd.RstReq    = pCmdHdl->cmdInfo.rsltBuf.mt.cmd != pCmdHdl->cmdInfo.cmdBuf.mt.cmd;
	cmd.AccRate   = (LONG)buf;								/* ACC_TIME�Z�b�g */
	cmd.EnableRate = FALSE;									/* ACC_TIME�L�� */

	/* �y�q�d�s�R�}���h�`�o�h�֐����s */
	result = MciSetHomingCmd(pMtHdl, &cmd);

	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->cmdInfo.rsltBuf.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.mt.cmd;
	pCmdHdl->cmdInfo.rsltBuf.mt.sel_mon12 = pCmdHdl->cmdInfo.cmdBuf.mt.sel_mon12;

	return result;
}

/****************************************************************************/
/* �T�v		�F�u�d�k�b�s�q�k�R�}���h��͏���						(3CH)	*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	�F	����I��	:	CMD_FUNC_NORMAL_END							*/
/*				�����ُ�	:	VELCTRL_CMD_WARNING1						*/
/*				�ް��ُ�	:	VELCTRL_CMD_DATA_SET_WARNING2				*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_VELCTRL( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	MCI_CMD		cmd;
	INT32		result;
	
	/* ���[�J���ϐ��̏����� */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;

	/* �e�f�[�^�𒊏o */
	cmd.Velocity  = pCmdHdl->cmdInfo.cmdBuf.mt.pos2;	/* VFF/���x�w�� [ref/s]/[OS/2^30] */
	cmd.TrqLimitP = (ULONG)pCmdHdl->cmdInfo.cmdBuf.mt.pos1;			/* TRQ LIMIT�Z�b�g  */
	cmd.TrqLimitN = (ULONG)(pCmdHdl->cmdInfo.cmdBuf.mt.pos1 >> 16);	/* TRQ LIMIT�Z�b�g  */
	cmd.Option    = pCmdHdl->cmdInfo.cmdBuf.mt.option;	/* Option�Z�b�g     */
	cmd.Trq       = (LONG)cmd.TrqLimitP;				/* TFF�Z�b�g */

	/* �u�d�k�b�s�q�k�R�}���h�`�o�h�֐����s */
	result = MciSetVelCmd(pMtHdl, &cmd);

	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->cmdInfo.rsltBuf.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.mt.cmd;
	pCmdHdl->cmdInfo.rsltBuf.mt.sel_mon12 = pCmdHdl->cmdInfo.cmdBuf.mt.sel_mon12;

	return result;
}

/****************************************************************************/
/* �T�v		�F�s�q�p�b�s�q�k�R�}���h��͏���						(3DH)	*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	�F	����I��	:	CMD_FUNC_NORMAL_END							*/
/*				�����ُ�	:	TRQCTRL_CMD_WARNING1						*/
/*				�ް��ُ�	:	TRQCTRL_CMD_DATA_SET_WARNING2				*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_TRQCTRL( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	MCI_CMD		cmd;
	INT32		result;
	
	/* ���[�J���ϐ��̏����� */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;
	
	/* �e�f�[�^�𒊏o */
	cmd.Velocity  = pCmdHdl->cmdInfo.cmdBuf.mt.pos1;	/* VLIM�Z�b�g       */
	cmd.Trq       = pCmdHdl->cmdInfo.cmdBuf.mt.pos2;	/* TQREF�Z�b�g      */
	cmd.TrqLimitP = CMD_MAXTRQ;							/* TRQ LIMIT�Z�b�g  */
	cmd.TrqLimitN = CMD_MAXTRQ;							/* TRQ LIMIT�Z�b�g  */
	cmd.Option    = pCmdHdl->cmdInfo.cmdBuf.mt.option;	/* Option�Z�b�g     */

	/* �s�q�p�b�s�q�k�R�}���h�`�o�h�֐����s */
	result = MciSetTrqCmd(pMtHdl, &cmd);

	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->cmdInfo.rsltBuf.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.mt.cmd;
	pCmdHdl->cmdInfo.rsltBuf.mt.sel_mon12 = pCmdHdl->cmdInfo.cmdBuf.mt.sel_mon12;

	return result;
}

/****************************************************************************/
/* �T�v		�F�r�u�b�s�q�k�R�}���h��͏���							(3FH)	*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	�F	����I��	:	CMD_FUNC_NORMAL_END							*/
/*																			*/
/*	SUBCTRL : d0-d1 L_SGN	���b�`�M���I��	0:C��, 1:EXT1, 2:EXT2, 3:EXT3	*/
/*			  d2    SET_L	���b�`�w��										*/
/*			  d3    RESERVE													*/
/*			  d4-d6 MOTION	���[�V�����I��	0:HOLD, 1:INTERPORATE			*/
/*											2,FEED, 3:POSING				*/
/*			  d7    RESERVE													*/
/*																			*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_SVCTRL( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	LONG		subctrl;	/* �T�u�R���g���[�� */
	CHAR		selmotion;	/* ���[�V�����I��   */
	UCHAR		sq_cmd;		/* �V�[�P���X�M��   */
	UCHAR		last_sq_cmd;
	INT32		result;
	
	/* ���[�J���ϐ��̏����� */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;
	result 		= CMD_FUNC_NORMAL_END;
	
	/* �e�f�[�^�𒊏o */
	subctrl = (LONG)pCmdHdl->cmdInfo.cmdBuf.mt.lt_sgn;
	selmotion = (subctrl >> 4) & 0x7;
	sq_cmd = pCmdHdl->cmdInfo.cmdBuf.mt.iocmd[0];

/*--------------------------------------------------------------------------*/
/*	���[�V�����M������				               		          			*/
/*--------------------------------------------------------------------------*/
	result = cmdSvCtrlMotionSelectProc(hNetData, selmotion);

/*--------------------------------------------------------------------------*/
/*	���b�`�M������				                             				*/
/*--------------------------------------------------------------------------*/
	if(subctrl & 0x04)
	{/*	���b�`���N�G�X�g	*/
		MciRequestMtLatch(pMtHdl, MOTION_MDL_LTMD, subctrl & 0x3);
	}
	else
	{/* ���b�`�L�����Z��	*/
		MciCancelLatch(pMtHdl, MOTION_MDL_LTMD);
	}
	
/*--------------------------------------------------------------------------*/
/*	�V�[�P���X�M������			                             				*/
/*--------------------------------------------------------------------------*/
	if(pCmdHdl->cmdInfo.rsltBuf.mt.cmd != pCmdHdl->cmdInfo.cmdBuf.mt.cmd)
	{
		last_sq_cmd = 0;
	}
	else
	{/* 	*/
		last_sq_cmd = pCmdHdl->cmdInfo.rsltBuf.mt.iocmd[0];
	}
	
	/*----	�u���[�L�I���M��	----------------*/
	if( (pCmdHdl->cmdInfo.rsltBuf.mt.cmd != pCmdHdl->cmdInfo.cmdBuf.mt.cmd)
			||((last_sq_cmd & SVCTRL_BRK_SEL) != (sq_cmd & SVCTRL_BRK_SEL)))/* ���s�����`�F�b�N */
	{
		if(sq_cmd & SVCTRL_BRK_SEL)
		{/* �a�j�w�߃I�� */
			pMtHdl->svCmd->CmdSeqBit &= ~(BRK_RELEASE_BIT);
		}
		else
		{/* �a�j�w�߃I�t */
			pMtHdl->svCmd->CmdSeqBit |= BRK_RELEASE_BIT;
		}
	}
	
	/*----	�Z���T�I���M��	--------------------*/
	MciSetSensOn(pMtHdl, sq_cmd & SVCTRL_SEN_SEL);

	/*----	�A���[���N���A�M��	----------------*/
	if(!(last_sq_cmd & SVCTRL_ACLR_SEL))
	{
		AlmSetClearReq(&hNetData->alm_hdl, (sq_cmd & SVCTRL_ACLR_SEL));
	}

	/*----	�T�[�{�I���M��	--------------------*/
	if(!(sq_cmd & SVCTRL_SON_SEL))
	{/* �T�[�{OFF */	
		MciSetServoOn(pMtHdl, FALSE, TRUE);
	}
	else if(!(last_sq_cmd & SVCTRL_SON_SEL))
	{/* �����オ��G�b�W�ŃT�[�{ON */
		if(MciSetServoOn(pMtHdl, TRUE, TRUE) != MCI_EMGBB_ERR)
		{
			sq_cmd &= (~SVCTRL_SON_SEL);	/* Safety Stop�ȊO�́A��ɃG�b�W�ɂ��� */
		}
	}

	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->cmdInfo.rsltBuf.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.mt.cmd;
	pCmdHdl->cmdInfo.rsltBuf.mt.sel_mon12 = pCmdHdl->cmdInfo.cmdBuf.mt.sel_mon12;
	pCmdHdl->cmdInfo.rsltBuf.mt.iocmd[0] = sq_cmd;

	return result;
}

/****************************************************************************/
/*						���[�V�����M������									*/
/****************************************************************************/
static INT32 cmdSvCtrlMotionSelectProc( MLNET_HANDLE *hNetData, CHAR sel_mt)
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	MCI_CMD		cmd;
	INT32		rc;

	/* �e�������ʃf�[�^�ݒ� */
	cmd.Trq       = 0;									/* TFF�Z�b�g */
	cmd.TrqLimitP = CMD_MAXTRQ;							/* TRQ LIMIT�Z�b�g  */
	cmd.TrqLimitN = CMD_MAXTRQ;							/* TRQ LIMIT�Z�b�g  */
	cmd.Option    = pCmdHdl->cmdInfo.cmdBuf.mt.option;		/* Option�Z�b�g     */
	cmd.EnableRate = FALSE;								/* ACC_TIME���� */

	/* ���[�J���ϐ��̏����� */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;
	
	switch (sel_mt)
	{/* �T�[�{�I����Ԃ̃`�F�b�N�͊eAPI�Ŏ��{ */
	/*----	HOLD	--------------------------------------------------------*/
		case SVCTRL_HOLD :
			cmd.HoldMod = (UCHAR)pCmdHdl->cmdInfo.cmdBuf.mt.pos1;/* Hold���[�h       */
			rc = MciSetHoldCmd(pMtHdl, &cmd);
			break;
	
	/*----	INTERPOLATE	----------------------------------------------------*/
		case SVCTRL_INTERPOLATE :
			if(hNetData->com_hdl.Phase != 3)					/* �t�F�[�Y�`�F�b�N 				*/
			{
				return CMD_WARNG1;
			}
			cmd.TPos      = pCmdHdl->cmdInfo.cmdBuf.mt.pos1;	/* TPOS�Z�b�g  						*/
			cmd.Velocity  = pCmdHdl->cmdInfo.cmdBuf.mt.pos2;	/* VFF/���x�w�� [ref/s]/[OS/2^24]	*/
			if( (hNetData->com_hdl.recvErrCnt == COM_RCV_WRN_STS)/* �ʐM���[�j���O���̕␳���� 		*/
					&& (pMtHdl->crtMtype == MTYPE_INTERPOL) )
			{/* �ʐM���[�j���O && ���݃��[�V����=INTERPOLATE */
				cmd.TPos += pMtHdl->outData.DposIn;
			}
			rc = MciSetInterpolateCmd(pMtHdl, &cmd);
			break;
		
	/*----	FEED	--------------------------------------------------------*/
		case SVCTRL_FEED :
			cmd.TSpeed   = pCmdHdl->cmdInfo.cmdBuf.mt.pos2;		/* TSPD�Z�b�g       */
			rc = MciSetFeedCmd(pMtHdl, &cmd);
			break;
			
	/*----	POSING	--------------------------------------------------------*/
		case SVCTRL_POSING :
			cmd.TPos     = pCmdHdl->cmdInfo.cmdBuf.mt.pos1;		/* TPOS�Z�b�g       */
			cmd.TSpeed   = pCmdHdl->cmdInfo.cmdBuf.mt.pos2;		/* TSPD�Z�b�g       */
			rc = MciSetPosingCmd(pMtHdl, &cmd);
			break;
			
	/*----	default	--------------------------------------------------------*/
		default :
			rc = DATA_SET_WARNG2;
			break;
	}
	
	if(rc != MCI_CNDTN_ERR)
	{
		return rc;	/* �����ُ�̓}�X�N�A����ȊO�̓G���[ */
	}
	
	return CMD_FUNC_NORMAL_END;
}


/****************************************************************************/
/* �T�v		�F�`�c�i�R�}���h��͏���										*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END	����I��							*/
/*			:	= CMD_FUNC_CONTINUE		�p�����s��							*/
/*			:	= DATA_SET_WARNG1		�f�[�^�ݒ�x���P	�i�`�D�X�S�`�j	*/
/*			:	= DATA_SET_WARNG2		�f�[�^�ݒ�x���Q	�i�`�D�X�S�a�j	*/
/*			:	= DATA_SET_WARNG3		�f�[�^�ݒ�x���R	�i�`�D�X�S�b�j	*/
/*			:	= DATA_SET_WARNG4		�f�[�^�ݒ�x���S	�i�`�D�X�S�c�j	*/
/*			:	= CMD_WARNG1			�R�}���h�x���P		�i�`�D�X�T�`�j	*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_ADJ( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	REG_HNDL	*pRegHdl;
	INT32  		ret;
	UCHAR		sub_code;
	
	/* ���[�J���ϐ��̏����� */
	pCmdHdl		= &hNetData->cmd_hdl;
	pRegHdl		= &hNetData->reg_hdl;
	sub_code = pCmdHdl->cmdInfo.cmdBuf.adj.subcode;
	
	switch(sub_code)
	{
	case 0x00:
		ret = cmdADJ_sigma2_mode(pCmdHdl, pRegHdl);
		break;
	case 0x01:
		ret = cmdADJ_normal_mode(pCmdHdl, pRegHdl);
		break;
	case 0x0E:
		ret = cmdADJ_tester_mode(pCmdHdl);
		break;
	case 0x10:
		ret = cmdADJ_opIoCmd_mode(pCmdHdl); /* OPTION/IO_MON�t�B�[���h�g�p�\�R�}���h�g�����[�h */
		break;
	default:
		ret = DATA_SET_WARNG2;
		break;						   /*<V624>Add*/
	}

	return ret;
}

/****************************************************************************/
/* �T�v		�F�`�c�i�iOPTION/IO_MON�t�B�[���h�g�p�\�R�}���h�g�����[�h)    */
/****************************************************************************/
static INT32 cmdADJ_opIoCmd_mode( CMD_HNDL *pCmdHdl )
{
	UCHAR  cmd;
	UCHAR  sub_code;
	USHORT ccmd;
	USHORT cdata;
	INT32  ret;

	/* ���[�J���ϐ��̏����� */
	cmd      = pCmdHdl->cmdInfo.cmdBuf.adj.cmd;
	sub_code = pCmdHdl->cmdInfo.cmdBuf.adj.subcode;
	ccmd     = pCmdHdl->cmdInfo.cmdBuf.adj.ccmd;
	cdata    = pCmdHdl->cmdInfo.cmdBuf.adj.caddress;	/* CDATA�w�ߒ��o */
	ret  = CMD_FUNC_NORMAL_END;                    		/* ����I��		 */


	/* ���s�����`�F�b�N */
	if( (pCmdHdl->cmdInfo.cmdRdy != TRUE)                     /* CMDRDY = FALSE */
	    || (pCmdHdl->cmdInfo.rsltBuf.adj.cmd != cmd)          /* �O���MCMD�R�[�h != �����MCMD�R�[�h */
	    || (pCmdHdl->cmdInfo.rsltBuf.adj.subcode != sub_code) /* �O���MSUB_CODE  != �����MSUB_CODE 	*/
	    || (pCmdHdl->cmdInfo.rsltBuf.adj.ccmd != ccmd)        /* �O���MCCMD      != �����MCCMD		*/
	    || (pCmdHdl->cmdInfo.rsltBuf.adj.caddress != cdata) ) /* �O���MCDATA     != �����MCDATA 	*/
	{
	    /* ��͌��ʂ��o�b�t�@�փR�s�[ */
	    pCmdHdl->cmdInfo.rsltBuf.adj.cmd      = cmd;
	    pCmdHdl->cmdInfo.rsltBuf.adj.subcode  = sub_code;
	    pCmdHdl->cmdInfo.rsltBuf.adj.ccmd     = ccmd;

	    switch(ccmd)
	    {
	    case 0x0000:                                      /* �f�[�^�Q�� */
			pCmdHdl->cmdInfo.rsltBuf.adj.caddress = pCmdHdl->cmdInfo.chkOpIoCmd;
			return ret;
//			break;<S0C7>
	    case 0x0001:                                      /* �f�[�^�ݒ� */
			if(cdata & 0xFFFE)
			{
		    	ret = DATA_SET_WARNG2;
			}
			else
			{
		    	pCmdHdl->cmdInfo.cmdCdataBuf = cdata;         /* cdata�w�߂��o�b�t�@�փR�s�[ */
			}
			break;
	    default:
			ret = DATA_SET_WARNG2;
			break;
	    }
	    /* ��͌��ʂ��o�b�t�@�փR�s�[ */
	    pCmdHdl->cmdInfo.rsltBuf.adj.caddress = cdata;
	}
	/* �߂�l�ݒ� */
	return ret;
}

/****************************************************************************/
/* �T�v		�F�`�c�i�i���|�U�݊����[�h�j									*/
/****************************************************************************/
static INT32 cmdADJ_sigma2_mode( CMD_HNDL *pCmdHdl,  REG_HNDL *pRegHdl)
{
	REG_DATA reg_data;									/* ���W�X�^�A�N�Z�X��� */
	USHORT cdata[4]={0};
	UCHAR  cmd;
	UCHAR  sub_code;
	UCHAR  ccmd;
	USHORT cadrs;
	USHORT lcadrs;
	USHORT lcdata;
	INT32  ret;

	/* ���[�J���ϐ��̏����� */
	cdata[0] = UtilGetWData(pCmdHdl->cmdInfo.cmdBuf.adj2.cdata);
	cmd      = pCmdHdl->cmdInfo.cmdBuf.adj2.cmd;
	sub_code = pCmdHdl->cmdInfo.cmdBuf.adj2.subcode;
	ccmd     = pCmdHdl->cmdInfo.cmdBuf.adj2.ccmd;
	cadrs    = UtilGetWData(pCmdHdl->cmdInfo.cmdBuf.adj2.caddress);
	lcadrs   = UtilGetWData(pCmdHdl->cmdInfo.rsltBuf.adj2.caddress);
	lcdata   = UtilGetWData(pCmdHdl->cmdInfo.rsltBuf.adj2.cdata);
	ret  = CMD_FUNC_NORMAL_END;
	
	/* ���s�����`�F�b�N */
	if(  (pCmdHdl->cmdInfo.cmdRdy != FALSE) 
		&& ( (pCmdHdl->cmdInfo.rsltBuf.adj2.cmd != cmd) 
	    || (pCmdHdl->cmdInfo.rsltBuf.adj2.subcode != sub_code) 
		|| (pCmdHdl->cmdInfo.adjWrk != ccmd)
		|| (lcadrs != cadrs) 
		|| ((ccmd == 1) && (lcdata != cdata[0])) )	 )
	{
		/* ��͌��ʂ��o�b�t�@�փR�s�[ */
		pCmdHdl->cmdInfo.rsltBuf.adj2.cmd     = cmd;
		pCmdHdl->cmdInfo.rsltBuf.adj2.subcode = sub_code;
		pCmdHdl->cmdInfo.rsltBuf.adj2.ccmd    = ccmd;
		pCmdHdl->cmdInfo.adjWrk = ccmd;
		UtilSetWData(pCmdHdl->cmdInfo.rsltBuf.adj2.caddress, cadrs);

		if(ccmd == 0)
		{	/* �f�[�^�Q�� */
			pCmdHdl->cmdInfo.RegData.AccessSize = 2;
			pCmdHdl->cmdInfo.RegData.Length = 1;
			pCmdHdl->cmdInfo.RegData.Number = VMEM_VEN_MIN + (cadrs<<1);
			pCmdHdl->cmdInfo.RegData.EepWrite = FALSE;
			pCmdHdl->cmdInfo.RegData.pBuf = (LONG*)pCmdHdl->cmdInfo.rsltBuf.adj2.cdata;
			pCmdHdl->cmdInfo.RegData.BufferSize = sizeof(pCmdHdl->cmdInfo.rsltBuf.adj2.cdata);
	
			/* ���W�X�^�R�}���h�����v�����sAPI	*/
			ret = RegRequestCommand(pRegHdl, REG_CMD_ADJ_RD, &pCmdHdl->cmdInfo.RegData);
		}
		else if(ccmd == 1)
		{	/* �f�[�^�ݒ� */
			/* ���W�X�^�A�N�Z�X�p�f�[�^�ݒ� */
			pCmdHdl->cmdInfo.RegData.AccessSize = 2;
			pCmdHdl->cmdInfo.RegData.Length = 1;
			pCmdHdl->cmdInfo.RegData.Number = VMEM_VEN_MIN + (cadrs<<1);
			pCmdHdl->cmdInfo.RegData.EepWrite = FALSE;
//			pCmdHdl->cmdInfo.RegData.pBuf = (LONG*)&cdata[0];
			pCmdHdl->cmdInfo.RegData.pBuf = (LONG*)&pCmdHdl->cmdInfo.cmdBuf.adj2.cdata;/* <S0D4> */
			pCmdHdl->cmdInfo.RegData.BufferSize = sizeof(pCmdHdl->cmdInfo.cmdBuf.adj2.cdata);
	
			/* ���W�X�^�R�}���h�����v�����sAPI	*/
			ret = RegRequestCommand(pRegHdl, REG_CMD_PRMWR, &pCmdHdl->cmdInfo.RegData);

			/* ��͌��ʂ��o�b�t�@�փR�s�[ */
			UtilSetDWData(pCmdHdl->cmdInfo.rsltBuf.adj.cdata, cdata[0]);
		}
		else
		{
			return DATA_SET_WARNG2;
		}
#if 0	/* <S0D4> */	
		UtilSetWData(pCmdHdl->cmdInfo.rsltBuf.adj2.cdata, cdata[0]);
		if((ret != REG_CONTINUE) && (ret != REG_SUCCESS))
		{/* �G���[���̓G���[�����Z�b�g */
			pCmdHdl->cmdInfo.rsltBuf.adj2.ccmd = ccmd;
		}
#endif
	}
	else
	{
		if(pCmdHdl->cmdInfo.cmdRdy == FALSE)
		{
			ret = RegCheckCommandState(pRegHdl);	
			if((ret != REG_CONTINUE) && (ret != REG_SUCCESS))	/* <S0D4> */
			{/* �G���[���̓G���[�����Z�b�g */
				pCmdHdl->cmdInfo.rsltBuf.adj2.ccmd = pRegHdl->accErrCode;
			}
		}
	}

	/* �߂�l�ݒ� */
	return ret;
}

/****************************************************************************/
/* �T�v		�F�`�c�i�i�ʏ탂�[�h�j											*/
/****************************************************************************/
static INT32 cmdADJ_normal_mode( CMD_HNDL *pCmdHdl,  REG_HNDL *pRegHdl)
{
	REG_DATA reg_data;									/* ���W�X�^�A�N�Z�X��� */
	ULONG  cdata[2]={0};
	UCHAR  cmd;
	UCHAR  sub_code;
	USHORT ccmd;
	USHORT cadrs;
	USHORT csize;
	ULONG  lcdata;
	INT32  ret;
	
	/* ���[�J���ϐ��̏����� */
	cdata[0] = UtilGetDWData(pCmdHdl->cmdInfo.cmdBuf.adj.cdata);
	cmd      = pCmdHdl->cmdInfo.cmdBuf.adj.cmd;
	sub_code = pCmdHdl->cmdInfo.cmdBuf.adj.subcode;
	ccmd     = pCmdHdl->cmdInfo.cmdBuf.adj.ccmd;
	cadrs    = pCmdHdl->cmdInfo.cmdBuf.adj.caddress;
	csize    = pCmdHdl->cmdInfo.cmdBuf.adj.csize;
	lcdata   = UtilGetDWData(pCmdHdl->cmdInfo.rsltBuf.adj.cdata);
	ret  = CMD_FUNC_NORMAL_END;


	/* ���s�����`�F�b�N */
	if(  (pCmdHdl->cmdInfo.cmdRdy != FALSE)
		&& ( (pCmdHdl->cmdInfo.rsltBuf.adj.cmd != cmd) 
	    || (pCmdHdl->cmdInfo.rsltBuf.adj.subcode != sub_code) 
		|| (pCmdHdl->cmdInfo.rsltBuf.adj.ccmd != ccmd)
		|| (pCmdHdl->cmdInfo.rsltBuf.adj.caddress != cadrs)
		|| (pCmdHdl->cmdInfo.adjWrk != csize)
		|| ((ccmd == 4) && (lcdata != cdata[0])) )	 )
	{
		if(ccmd == 3)
		{/* �f�[�^�Q�� */
			/* ���W�X�^�A�N�Z�X�p�f�[�^�ݒ� */
			pCmdHdl->cmdInfo.RegData.AccessSize = 0;
			pCmdHdl->cmdInfo.RegData.Length = 1;
			pCmdHdl->cmdInfo.RegData.Number = VMEM_VEN_MIN + (cadrs<<1);
			pCmdHdl->cmdInfo.RegData.EepWrite = FALSE;
			pCmdHdl->cmdInfo.RegData.pBuf = (LONG*)pCmdHdl->cmdInfo.rsltBuf.adj.cdata;
			pCmdHdl->cmdInfo.RegData.BufferSize = sizeof(pCmdHdl->cmdInfo.cmdBuf.adj.cdata);
	
			/* ���W�X�^�R�}���h�����v�����sAPI	*/
			ret = RegRequestCommand(pRegHdl, REG_CMD_ADJ_RD, &pCmdHdl->cmdInfo.RegData);

		}
		else if(ccmd == 4)
		{/* �f�[�^�ݒ� */
			/* ���W�X�^�A�N�Z�X�p�f�[�^�ݒ� */
			pCmdHdl->cmdInfo.RegData.AccessSize = csize;
			pCmdHdl->cmdInfo.RegData.Length = 1;
			pCmdHdl->cmdInfo.RegData.Number = VMEM_VEN_MIN + (cadrs<<1);
			pCmdHdl->cmdInfo.RegData.EepWrite = FALSE;
//			pCmdHdl->cmdInfo.RegData.pBuf = &cdata[0];
			pCmdHdl->cmdInfo.RegData.pBuf = (LONG*)&pCmdHdl->cmdInfo.cmdBuf.adj.cdata;/* <S0D4> */
			pCmdHdl->cmdInfo.RegData.BufferSize = sizeof(pCmdHdl);
	
			/* ���W�X�^�R�}���h�����v�����sAPI	*/
			ret = RegRequestCommand(pRegHdl, REG_CMD_PRMWR, &pCmdHdl->cmdInfo.RegData);

			/* ��͌��ʂ��o�b�t�@�փR�s�[ */
			UtilSetDWData(pCmdHdl->cmdInfo.rsltBuf.adj.cdata, cdata[0]);
		}
		else
		{
			return DATA_SET_WARNG2;
		}

		/* ��͌��ʂ��o�b�t�@�փR�s�[ */
		pCmdHdl->cmdInfo.rsltBuf.adj.cmd      = cmd;
		pCmdHdl->cmdInfo.rsltBuf.adj.subcode  = sub_code;
		pCmdHdl->cmdInfo.rsltBuf.adj.ccmd     = ccmd;
		pCmdHdl->cmdInfo.rsltBuf.adj.caddress = cadrs;
		pCmdHdl->cmdInfo.rsltBuf.adj.csize 	  = csize;
		pCmdHdl->cmdInfo.adjWrk = csize;
	}
	else
	{
		if(pCmdHdl->cmdInfo.cmdRdy == FALSE)
		{
			ret = RegCheckCommandState(pRegHdl);
			if((ret != REG_CONTINUE) && (ret != REG_SUCCESS))	/* <S0D4> */
			{/* �G���[���̓G���[�����Z�b�g */
				pCmdHdl->cmdInfo.rsltBuf.adj.csize = pRegHdl->accErrCode;
			}
		}
	}
	/* �߂�l�ݒ� */
	return ret;
}

/****************************************************************************/
/* �T�v		�F�`�c�i�i�e�X�^�[���[�h�j										*/
/****************************************************************************/
//#include "HardSw.h"
static INT32 cmdADJ_tester_mode( CMD_HNDL *hdl )
{
#if 0
	UCHAR  cmd;     
    UCHAR  sub_code;
    UCHAR  LedCode;
    UCHAR  sw[3];
    
	cmd      = hdl->cmdInfo.cmdBuf.adj.cmd;
	sub_code = hdl->cmdInfo.cmdBuf.adj.subcode;
	LedCode  = hdl->cmdInfo.cmdBuf.byte[8];

	HwTestSWFunc(LedCode, &sw[0], &sw[2], &sw[1]);

	hdl->cmdInfo.rsltBuf.adj.cmd      = cmd;
	hdl->cmdInfo.rsltBuf.adj.subcode  = sub_code;
	hdl->cmdInfo.rsltBuf.byte[4] = sw[0];
	hdl->cmdInfo.rsltBuf.byte[5] = sw[1];
	hdl->cmdInfo.rsltBuf.byte[6] = sw[2];
#endif
	return CMD_FUNC_NORMAL_END;
}

/****************************************************************************/
/* �T�v		�F�l�d�l�Q�q�c�R�}���h��͏���									*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END	����I��							*/
/*			:	= CMD_FUNC_CONTINUE		�p�����s��							*/
/*			:	= DATA_SET_WARNG4		�f�[�^�ݒ�x���S	�i�`�D�X�S�c�j	*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_MEM_RD( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	LONG   adrs;
	UCHAR  size;
	UCHAR  *p, i;
	INT32  ret;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl = &hNetData->cmd_hdl;
	adrs  = UtilGetDWData(&pCmdHdl->cmdInfo.cmdBuf.byte[4]);
	size  = pCmdHdl->cmdInfo.cmdBuf.byte[8];
	ret  = CMD_FUNC_NORMAL_END;
  
	if(size > 6)	
	{	/* �f�[�^�T�C�Y�ُ� */
		ret = DATA_SET_WARNG4;
	}
	else
	{
		p = (UCHAR*)adrs;
		for(i = 0;i < size; i++)
		{
			pCmdHdl->cmdInfo.rsltBuf.byte[9+i] = *(p+i);
		}
		UtilMemSet(&pCmdHdl->cmdInfo.rsltBuf.byte[9+size], 0, 6-size);
	}

	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->cmdInfo.rsltBuf.nop.cmd = pCmdHdl->cmdInfo.cmdBuf.nop.cmd;
	UtilSetDWData(&pCmdHdl->cmdInfo.rsltBuf.byte[4], adrs);
	pCmdHdl->cmdInfo.rsltBuf.byte[8] = size;

	/* �߂�l�ݒ� */
	return ret;

}

/****************************************************************************/
/*								�T�u�R�}���h								*/
/****************************************************************************/

/****************************************************************************/
/* �T�v		�F����`�R�}���h��͏���										*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_WARNG1				�R�}���h�x���P					*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 exCmd_NO_DEFINE( MLNET_HANDLE *hNetData )
{	/* �ُ�I����Ԃ� */
	hNetData->cmd_hdl.exCmdInfo.rsltBuf = hNetData->cmd_hdl.exCmdInfo.cmdBuf;
	return CMD_WARNG6;
}
INT32 exCmd_NO_SUPP( MLNET_HANDLE *hNetData )
{	/* �ُ�I����Ԃ� */
	hNetData->cmd_hdl.exCmdInfo.rsltBuf = hNetData->cmd_hdl.exCmdInfo.cmdBuf;	  /*�R�[�f�B���O�~�X�C��*//*<V594>*/
	return CMD_WARNG2;
}

/****************************************************************************/
/* �T�v		�F�m�n�o�R�}���h��͏���										*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END		����I��						*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 exCmd_NOP( MLNET_HANDLE *hNetData )
{
	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	hNetData->cmd_hdl.exCmdInfo.rsltBuf.mon.cmd = hNetData->cmd_hdl.exCmdInfo.cmdBuf.mon.cmd;
	return CMD_FUNC_NORMAL_END;
}

/****************************************************************************/
/* �T�v		�F�o�q�l�Q�q�c�R�}���h��͏���									*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END	����I��							*/
/*			:	= CMD_FUNC_CONTINUE		�p�����s��							*/
/*			:	= DATA_SET_WARNG1		�f�[�^�ݒ�x���P	�i�`�D�X�S�`�j	*/
/*			:	= DATA_SET_WARNG4		�f�[�^�ݒ�x���S	�i�`�D�X�S�c�j	*/
/*			:	= CMD_WARNG1			�R�}���h�x���P		�i�`�D�X�T�`�j	*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 exCmd_PRM_RD( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	UCHAR  		cmd;								/* �R�}���h�R�[�h 		*/
	USHORT 		no;  								/* �p�����[�^�ԍ� 		*/
	UCHAR  		size;								/* �p�����[�^�T�C�Y 	*/
	USHORT 		data[4] = {0};						/* �p�����[�^�f�[�^ 	*/
	REG_DATA 	reg_data;							/* ���W�X�^�A�N�Z�X��� */
	INT32  		ret  = CMD_FUNC_NORMAL_END;
	
	/* ���[�J���ϐ��̏����� */
	pCmdHdl = &hNetData->cmd_hdl;
	
	cmd = pCmdHdl->exCmdInfo.cmdBuf.prm.cmd;
	no  = pCmdHdl->exCmdInfo.cmdBuf.prm.no;
	size = pCmdHdl->exCmdInfo.cmdBuf.prm.size;

	/* ���s�����`�F�b�N */
	if(  (pCmdHdl->exCmdInfo.cmdRdy != FALSE) 
	     && ((pCmdHdl->exCmdInfo.rsltBuf.prm.cmd != cmd) 
	     || (pCmdHdl->exCmdInfo.rsltBuf.prm.no != no) 
	     || (pCmdHdl->exCmdInfo.rsltBuf.prm.size != size)) )
	{
		if(size == 0)/* <S16C> */
		{
			ret = DATA_SET_WARNG4;			/* A.94D �f�[�^�ݒ�x���S */
		}
		else
		{
			/* ���W�X�^�A�N�Z�X�p�f�[�^�ݒ� */
			pCmdHdl->exCmdInfo.RegData.AccessSize = size;
			pCmdHdl->exCmdInfo.RegData.Length = 1;
			pCmdHdl->exCmdInfo.RegData.Number = (no<<1) + VMEM_VEN_MIN + ((RAM_OFFSET)<<1);
			pCmdHdl->exCmdInfo.RegData.EepWrite = FALSE;
			pCmdHdl->exCmdInfo.RegData.pBuf = (LONG*)pCmdHdl->exCmdInfo.rsltBuf.prm.data;
			pCmdHdl->exCmdInfo.RegData.BufferSize = sizeof(pCmdHdl->exCmdInfo.rsltBuf.prm.data);
	
			/* ���W�X�^�R�}���h�����v�����sAPI	*/
			ret = RegRequestCommand(&hNetData->reg_hdl, REG_CMD_PRMRD, &pCmdHdl->exCmdInfo.RegData);
		}
		/* ��͌��ʂ��o�b�t�@�փR�s�[ */
		pCmdHdl->exCmdInfo.rsltBuf.prm.cmd  = cmd;
		pCmdHdl->exCmdInfo.rsltBuf.prm.no   = no;
		pCmdHdl->exCmdInfo.rsltBuf.prm.size = size;
	}
	else
	{
		if(pCmdHdl->exCmdInfo.cmdRdy == FALSE)
		{
			ret = RegCheckCommandState(&hNetData->reg_hdl);
		}
	}

	/* �߂�l�ݒ� */
	return ret;
}

/****************************************************************************/
/* �T�v		�F�o�q�l�Q�v�q�R�}���h��͏���									*/
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
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 exCmd_PRM_WR( MLNET_HANDLE *hNetData )
{
	USHORT ofst = RAM_OFFSET;
	INT32  ret;

	ret = exCmdPrmWrProc(&hNetData->cmd_hdl, &hNetData->reg_hdl ,ofst);
	return ret;
}

/****************************************************************************/
/* �T�v		�F�p�����[�^�������ݎ��s����									*/
/****************************************************************************/
static  INT32 exCmdPrmWrProc(CMD_HNDL* cmdHdl, REG_HNDL* regHdl, USHORT ofst)
{
	USHORT no   = cmdHdl->exCmdInfo.cmdBuf.prm.no;		/* �p�����[�^�ԍ� */
	UCHAR  size = cmdHdl->exCmdInfo.cmdBuf.prm.size;	/* �p�����[�^�T�C�Y */
	USHORT data[4] = {0};								/* �p�����[�^�f�[�^ */
	INT32  ret  = CMD_FUNC_NORMAL_END;

	/* ���s�����`�F�b�N */
	if( (cmdHdl->exCmdInfo.cmdRdy != FALSE)
		&& cmdChkExPrmWr(&cmdHdl->exCmdInfo.cmdBuf, &cmdHdl->exCmdInfo.rsltBuf) )
	{
		if(size == 0)/* <S16C> */
		{
			ret = DATA_SET_WARNG4;			/* A.94D �f�[�^�ݒ�x���S */
		}
		else
		{
			/* ���W�X�^�A�N�Z�X�p�f�[�^�ݒ� */
			cmdHdl->exCmdInfo.RegData.AccessSize = size;
			cmdHdl->exCmdInfo.RegData.Length = 1;
			cmdHdl->exCmdInfo.RegData.Number = (no<<1) + VMEM_VEN_MIN + ( (ofst) <<1 );
			cmdHdl->exCmdInfo.RegData.EepWrite = FALSE;
			cmdHdl->exCmdInfo.RegData.pBuf = (LONG*)cmdHdl->exCmdInfo.cmdBuf.prm.data;
			cmdHdl->exCmdInfo.RegData.BufferSize = sizeof(cmdHdl->exCmdInfo.cmdBuf.prm.data);
	
			/* ���W�X�^�R�}���h�����v�����sAPI	*/
			ret = RegRequestCommand(regHdl, REG_CMD_PRMWR, &cmdHdl->exCmdInfo.RegData);
		}
		/* ��͌��ʂ��o�b�t�@�փR�s�[ */
		cmdHdl->exCmdInfo.rsltBuf.prm.cmd  = cmdHdl->exCmdInfo.cmdBuf.prm.cmd;
		cmdHdl->exCmdInfo.rsltBuf.prm.no   = no;
		cmdHdl->exCmdInfo.rsltBuf.prm.size = size;
		/* �Wbyte���R�s�[(�擪�A�h���X�́A��A�h���X�ł��邽�߁ADWORD�̏������݂͂ł��Ȃ�) */
		UtilSetLLData(cmdHdl->exCmdInfo.rsltBuf.prm.data, cmdHdl->exCmdInfo.cmdBuf.prm.data);
	}
	else
	{
		if(cmdHdl->exCmdInfo.cmdRdy == FALSE)
		{
			ret = RegCheckCommandState(regHdl);
		}
	}

	/* �߂�l�ݒ� */
	return ret;
}

/****************************************************************************/
/* �T�v		�F�p�����[�^�������ݎ��s�����`�F�b�N����						*/
/****************************************************************************/
static INT32 cmdChkExPrmWr(CMD_LINK_EXDATA* cmd_buf, CMD_LINK_EXDATA* rslt_buf)
{
	UCHAR i, sz;

	if(rslt_buf->prm.cmd != cmd_buf->prm.cmd)	return TRUE;
	if(rslt_buf->prm.no != cmd_buf->prm.no)		return TRUE;
	if(rslt_buf->prm.size != cmd_buf->prm.size)	return TRUE;

	sz = (cmd_buf->prm.size >= 8) ? 8: cmd_buf->prm.size;
	for(i = 0; i < sz; i++)
	{
		if(cmd_buf->prm.data[i] != rslt_buf->prm.data[i])
			return TRUE;
	}

	return FALSE;
}

/****************************************************************************/
/* �T�v		�F�`�k�l�Q�q�c�R�}���h��͏���									*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END		����I��						*/
/*			:	= CMD_FUNC_CONTINUE			�p�����s��						*/
/*			:	= DATA_SET_WARNG2			�f�[�^�ݒ�x���Q�i�`�D�X�S�a�j	*/
/*			:	= CMD_WARNG1				�R�}���h�x���P�i�`�D�X�T�`�j	*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 exCmd_ALM_RD( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	UCHAR		cmd, alm_rd_mod, alm_idx;
	UCHAR		*alm_data;
	INT32  		ret;
	
	/* ���[�J���ϐ��̏����� */
	ret		= CMD_FUNC_NORMAL_END;
	pCmdHdl	= &hNetData->cmd_hdl;
	
	cmd        = pCmdHdl->exCmdInfo.cmdBuf.alm.cmd;			/* �R�}���h�R�[�h			*/
	alm_rd_mod = pCmdHdl->exCmdInfo.cmdBuf.alm.mode;		/* �A���[���ǂݏo�����[�h	*/
	alm_idx    = pCmdHdl->exCmdInfo.cmdBuf.alm.data[0];		/* �A���[���C���f�b�N�X�ԍ�	*/
	alm_data  = &pCmdHdl->exCmdInfo.rsltBuf.alm.data[0];	/* �A���[���f�[�^			*/

	/* ���s�����`�F�b�N */
	if( (pCmdHdl->exCmdInfo.cmdRdy == FALSE) 
	    || (pCmdHdl->exCmdInfo.rsltBuf.alm.cmd != cmd)
	    || (pCmdHdl->exCmdInfo.rsltBuf.alm.mode != alm_rd_mod) 
	    || ((alm_rd_mod == ALM_RD_MOD2) && (pCmdHdl->exCmdInfo.rsltBuf.alm.data[0] != alm_idx)) 
	    || ((alm_rd_mod == ALM_RD_MOD3) && (pCmdHdl->exCmdInfo.rsltBuf.alm.data[0] != alm_idx)) )
	{
		ret = cmdAlmRdProc(&hNetData->alm_hdl, alm_rd_mod, alm_idx, alm_data);

		/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
		pCmdHdl->exCmdInfo.rsltBuf.alm.cmd  = cmd;
		pCmdHdl->exCmdInfo.rsltBuf.alm.mode = alm_rd_mod;
	}

	return ret;
}

/****************************************************************************/
/* �T�v		�F�o�o�q�l�Q�v�q�R�}���h��͏���								*/
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
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 exCmd_PPRM_WR( MLNET_HANDLE *hNetData )
{
	USHORT ofst = EEPROM_OFFSET;
	INT32  ret;

	ret = exCmdPrmWrProc(&hNetData->cmd_hdl, &hNetData->reg_hdl ,ofst);
	return ret;
}

/****************************************************************************/
/* �T�v		�F�k�s�l�n�c�Q�n�m�R�}���h��͏���								*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END	����I��							*/
/*			:	= LTMOD_ON_CMD_WARNG4	�R�}���h�x���S�i�`�D�X�T�c�j		*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 exCmd_LTMOD_ON( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	LONG  lt_sgn;
	UCHAR lt_mod;
	INT32  ret;

	/* ���[�J���ϐ��̏����� */
	ret		= CMD_FUNC_NORMAL_END;
	pCmdHdl	= &hNetData->cmd_hdl;
	pMtHdl	= &hNetData->mt_hdl;
	
	lt_sgn = (LONG)pCmdHdl->exCmdInfo.cmdBuf.mon.lt_sgn & 0x03;
	lt_mod = (UCHAR)pCmdHdl->exCmdInfo.cmdBuf.mon.mon3[0];

	/* ���b�`�v�� */
	ret = MciRequestMdlLatch(pMtHdl, lt_mod, lt_sgn);

	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->exCmdInfo.rsltBuf.mon.cmd = pCmdHdl->exCmdInfo.cmdBuf.mon.cmd;
	pCmdHdl->exCmdInfo.rsltBuf.mon.sel_mon34 = pCmdHdl->exCmdInfo.cmdBuf.mon.sel_mon34;
	
	return ret;
}

/****************************************************************************/
/* �T�v		�F�k�s�l�n�c�Q�n�e�e�R�}���h��͏���							*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END	����I��							*/
/*			:	= LTMOD_ON_CMD_WARNG4	�R�}���h�x���S�i�`�D�X�T�c�j		*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 exCmd_LTMOD_OFF( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	INT32  ret;

	/* ���[�J���ϐ��̏����� */
	ret		= CMD_FUNC_NORMAL_END;
	pCmdHdl	= &hNetData->cmd_hdl;
	pMtHdl	= &hNetData->mt_hdl;
	
	/* ���b�`�L�����Z�� */
	ret = MciCancelLatch(pMtHdl, MODAL_LTMD);
	
	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->exCmdInfo.rsltBuf.mon.cmd = pCmdHdl->exCmdInfo.cmdBuf.mon.cmd;
	pCmdHdl->exCmdInfo.rsltBuf.mon.sel_mon34 = pCmdHdl->exCmdInfo.cmdBuf.mon.sel_mon34;
	
	return ret;
}

/****************************************************************************/
/* �T�v		�F�r�l�n�m�R�}���h��͏���										*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	= CMD_FUNC_NORMAL_END	����I��							*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 exCmd_SMON( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;

	pCmdHdl		= &hNetData->cmd_hdl;
	/* �b�l�c�f�[�^����͌��ʃo�b�t�@�փR�s�[ */
	pCmdHdl->exCmdInfo.rsltBuf.mon.cmd = pCmdHdl->exCmdInfo.cmdBuf.mon.cmd;
	pCmdHdl->exCmdInfo.rsltBuf.mon.sel_mon34 = pCmdHdl->exCmdInfo.cmdBuf.mon.sel_mon34;

	return CMD_FUNC_NORMAL_END;
}
