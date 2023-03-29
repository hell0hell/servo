/************************************************************************************/
/*																					*/
/*																					*/
/*		�A���[��ID��` �w�b�_�t�@�C��												*/
/*																					*/
/*																					*/
/************************************************************************************/
/*																					*/
/*																					*/
/*																					*/
/*																					*/
/*																					*/
/*																					*/
/************** Copyright Yaskawa Electric Corporation ******************************/
/*																					*/
/*																					*/
/*																					*/
/*																					*/
/************************************************************************************/
#ifndef		ERR_NO_H_
#define		ERR_NO_H_

//#include "XlsAlmDef.h"

/* defines */
#define ALM_ID_MASK	0xFF		/* �A���[��ID�}�X�N 							*/

#define ERR_NONE		0x0
#define ERR_WAIT_COMP	0x1

enum ERROR_NO_ {
	ERR_COM_PRM = (LONG)0x80000000,	/* A.04A : �p�����[�^�ݒ�ُ�					*//*<S0C7>*/
	ERR_COM_SCAN,					/* A.BFB : �������ԃI�[�o(ScanN2)				*/
	ERR_COM_LSISET,					/* A.B6A : JL-080���������s						*/
	ERR_COM_LSISYS,					/* A.B6B : JL-080�����V�X�e���G���[				*/
	ERR_COM_TCYC,					/* A.E40 : MECHATROLINK�`�������ݒ�ُ�			*/
	ERR_COM_WDC,					/* A.E50 : MECHATROLINK WDT�ُ�					*/
	ERR_SYNC_TOUT,					/* A.E51 : MECHATROLINK WDT�ُ�(��������)		*/
	ERR_COM_RCVSTP,					/* A.E60 : MECHATROLINK�ʐM�ُ�					*/
	ERR_GBL_SYNC,					/* A.E61 : MECHATROLINK�`����������				*/
	ERR_SYN_REQ,					/* A.EA0 : �T�[�{�p�b�N�̏�						*/
	ERR_SYN_FAIL,					/* A.EA1 : �T�[�{�����A�N�Z�X�ُ�				*/
	ERR_SV_WDC,						/* A.EA2 : �T�[�{WDC�ُ�						*/
	ERR_SV_NOANS,					/* A.ED0 : �T�[�{������							*/
	ERR_CMD_FAIL,					/* A.ED1 : �R�}���h���s����						*/

	ERR_COM_RCV,					/* A.960 : MECHATROLINK�ʐM�x��					*/
	ERR_PRM_NO,						/* A.94A : �f�[�^�ݒ�x��(���[�U�萔�ԍ�)		*/
	ERR_DATA_SET,					/* A.94B : �f�[�^�ݒ�x��(�f�[�^�͈͊O)			*/
	ERR_DATA_CALC,					/* A.94C : �f�[�^�ݒ�x��(���Z�ُ�)				*/
	ERR_DATA_SZ,					/* A.94D : �f�[�^�ݒ�x��(�f�[�^�T�C�Y�ُ�)		*/
	ERR_DATA_LTMOD,					/* A.94E : �f�[�^�ݒ�x��(���b�`���[�h�ُ�)		*/
	ERR_CMD_CNDTN,					/* A.95A : �R�}���h�x��(�����O)					*/
	ERR_CMD_NOSUP,					/* A.95B : �R�}���h�x��(���T�|�[�g)				*/
	ERR_CMD_PRM,					/* A.95C : �R�}���h�x��(���[�U�萔�ɂ������O)	*/
	ERR_CMD_CNFLCT,					/* A.95D : �R�}���h�x��(���b�`�R�}���h�̊���)	*/
	ERR_SUBCMD,						/* A.95E : �R�}���h�x��(���C��-�T�u�R�}���h����)*/
	ERR_FUNC_CNFLCT,				/* A.95F : �R�}���h�x��(�|�[�g�̊���)			*/
};

#endif		/*ERR_NO_H_*/

