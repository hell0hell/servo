/****************************************************************************
Description	: �ʐM�ݒ��`
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : Mercury

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------

****************************************************************************/
#ifndef COM_CFG_H_

#include "alarm.h"

/* MECHATROLINK�ʐM�R���|�[�l���gVer */
#define COM_VER_MAIN		0x02
#define COM_VER_MINOR		0x10	/* BCD �\�L */
#define COM_VER_TEST		0		/* �ǔ� */
#define COM_IF_TYPE			0x0012
#define COM_Y_NUMBER		0
#define COM_Y_VER			0x0
#define	COM_COMPONENT_VER	((COM_VER_MAIN<<8) | COM_VER_MINOR)
/* <S155>:start */
//#define	COM_MODEL_STR		"MECHATROLINK2-IF"
//#define	OPTIF_MODEL_STR		"MECHATROLINK-IF"
extern UCHAR COM_MODEL_STR[24];
#define COM_MODULE_VER		COM_COMPONENT_VER | (((COM_Y_NUMBER<<4) | COM_Y_VER) << 16)
/* <S155>:end */
typedef struct CFG_TIM_TBL_ {
	USHORT	SuptCycle;		/* �T�|�[�g�`������[0.25us] */
	UCHAR	reserve;		/* �\�� */
	UCHAR	SvIfDataCyc;	/* �T�[�{�h�e�̃f�[�^��������(���[�V��������)[Tio_base] */
} CFG_TIM_TBL;



/****************************************************************************
* MECHATROLINK�G���[�R�[�h��`
*
* Bit 00 - 07: Alam ID 
* Bit 16 - 23: CMD_ALM/COM_ALM Code 
* Bit 24 - 27: Device alarm=0, CMD alarm=1, COM alarm=2
* Bit 27 - 29: reserved
* Bit 30     : warning=1, alarm = 0
* Bit 31     : alwasy 1 
****************************************************************************/
#define ERR_DEF_BIT		0x80000000
#define ERR_WRN_BIT		0x40000000
#define ERR_COM_BIT		0x02000000
#define ERR_CMD_BIT		0x01000000
#define ERR_SVID_MASK	0x000000FF

#define ERR_ALM_COM		(ERR_DEF_BIT | ERR_COM_BIT)
#define ERR_ALM_CMD		(ERR_DEF_BIT | ERR_CMD_BIT)
#define ERR_ALM_DEV		(ERR_DEF_BIT)
#define ERR_WRN_COM		(ERR_DEF_BIT | ERR_WRN_BIT | ERR_COM_BIT)
#define ERR_WRN_CMD		(ERR_DEF_BIT | ERR_WRN_BIT | ERR_CMD_BIT)
#define ERR_WRN_DEV		(ERR_DEF_BIT | ERR_WRN_BIT)
#define ERR_RSP(a)		(a<<16)
#define GET_ERR_RSP(a)	(((a)>>16)&0xF)


enum ERROR_NO_ {
	ERR_NONE			= 0,	/* No Error */
	ERR_WAIT_COMP		= 1,	/* Busy status */

	ERR_COM_PRM			= (INT)(ERR_ALM_DEV | ERR_RSP(0) | ALM_OPTPRM_ERR),	/* A.04A : �p�����[�^�ݒ�ُ� */
	ERR_COM_SCAN		= (INT)(ERR_ALM_DEV | ERR_RSP(0) | ALM_SCANN_STOP),	/* A.BFB : �������ԃI�[�o(ScanN2) */
	ERR_COM_LSISET		= (INT)(ERR_ALM_DEV | ERR_RSP(0) | ALM_COMLSI_ERR1), 	/* A.B6A : JL-100���������s */
	ERR_COM_LSISYS		= (INT)(ERR_ALM_DEV | ERR_RSP(0) | ALM_COMLSI_ERR2), 	/* A.B6B : JL-100�����V�X�e���G���[ */
	ERR_COM_TCYC		= (INT)(ERR_ALM_DEV | ERR_RSP(0) | ALM_TCYC_ERR), 		/* A.E40 : MECHATROLINK�`�������ݒ�ُ� */
	ERR_COM_WDC			= (INT)(ERR_ALM_COM | ERR_RSP(12)| ALM_COMSYNC), 		/* A.E50 : MECHATROLINK WDT�ُ� */
	ERR_SYNC_TOUT		= (INT)(ERR_ALM_COM | ERR_RSP(12)| ALM_COMSYNC_TOUT), 	/* A.E51 : MECHATROLINK WDT�ُ�(��������) */
	ERR_COM_RCVSTP		= (INT)(ERR_ALM_COM | ERR_RSP(9) | ALM_COM), 			/* A.E60 : MECHATROLINK�ʐM�ُ� */
	ERR_GBL_SYNC		= (INT)(ERR_ALM_COM | ERR_RSP(11)| ALM_COM_INT_ERR), 	/* A.E61 : MECHATROLINK�`���������� */
	ERR_SYN_REQ			= (INT)(ERR_ALM_DEV | ERR_RSP(0) | ALM_NOANS), 			/* A.EA0 : �T�[�{�p�b�N�̏� */
	ERR_SYN_FAIL		= (INT)(ERR_ALM_DEV | ERR_RSP(0) | ALM_SVINIT_ERR), 	/* A.EA1 : �T�[�{�����A�N�Z�X�ُ� */
	ERR_SV_WDC			= (INT)(ERR_ALM_DEV | ERR_RSP(0) | ALM_SVWDC_ERR), 		/* A.EA2 : �T�[�{WDC�ُ� */
	ERR_SV_NOANS		= (INT)(ERR_ALM_DEV | ERR_RSP(0) | ALM_SVANS_TOUT), 	/* A.ED0 : �T�[�{������ */
	ERR_CMD_FAIL		= (INT)(ERR_ALM_DEV | ERR_RSP(0) | ALM_CMDEXEC), 		/* A.ED1 : �R�}���h���s���� */

	ERR_COM_RCV			= (INT)(ERR_WRN_COM | ERR_RSP(2) | WRN_COM), 			/* A.960 : MECHATROLINK�ʐM�x��	*/
	ERR_PRM_NO			= (INT)(ERR_WRN_CMD | ERR_RSP(9) | WRN_PRM_NO), 		/* A.94A : �f�[�^�ݒ�x��(���[�U�萔�ԍ�) */
	ERR_DATA_SET		= (INT)(ERR_WRN_CMD | ERR_RSP(9) | WRN_DATA_AREA), 		/* A.94B : �f�[�^�ݒ�x��(�f�[�^�͈͊O)	*/
	ERR_DATA_CALC		= (INT)(ERR_WRN_CMD | ERR_RSP(9) | WRN_DATA_CALC), 		/* A.94C : �f�[�^�ݒ�x��(���Z�ُ�) */
	ERR_DATA_SZ			= (INT)(ERR_WRN_CMD | ERR_RSP(9) | WRN_DATA_SIZE), 		/* A.94D : �f�[�^�ݒ�x��(�f�[�^�T�C�Y�ُ�) */
	ERR_DATA_LTMOD		= (INT)(ERR_WRN_CMD | ERR_RSP(10)| WRN_DATA_LTMOD), 	/* A.94E : �f�[�^�ݒ�x��(���b�`���[�h�ُ�) */
	ERR_CMD_CNDTN		= (INT)(ERR_WRN_CMD | ERR_RSP(10)| WRN_CMD_CNDTN), 		/* A.95A : �R�}���h�x��(�����O) */
	ERR_CMD_NOSUP		= (INT)(ERR_WRN_CMD | ERR_RSP(8) | WRN_CMD_NOSPT), 		/* A.95B : �R�}���h�x��(���T�|�[�g) */
	ERR_CMD_PRM			= (INT)(ERR_WRN_CMD | ERR_RSP(10)| WRN_CMD_PRM), 		/* A.95C : �R�}���h�x��(���[�U�萔�ɂ������O) */
	ERR_CMD_CNFLCT		= (INT)(ERR_WRN_CMD | ERR_RSP(10)| WRN_CMD_CNFLCT), 	/* A.95D : �R�}���h�x��(���b�`�R�}���h�̊���) */
	ERR_SUBCMD			= (INT)(ERR_WRN_CMD | ERR_RSP(11)| WRN_SUBCMD), 		/* A.95E : �R�}���h�x��(���C��-�T�u�R�}���h����) */
	ERR_FUNC_CNFLCT		= (INT)(ERR_WRN_CMD | ERR_RSP(8) | WRN_FUNC_CNFLCT), 	/* A.95F : �R�}���h�x��(�|�[�g�̊���) */
};

/****************************************************************************
	MECHATROLINK�G���[�X�e�[�^�X
****************************************************************************/
/* ���[�j���O��` */
#define DATA_SET_WARNG1		ERR_PRM_NO		/* A.94A �f�[�^�ݒ�x���P */
#define DATA_SET_WARNG2		ERR_DATA_SET	/* A.94B �f�[�^�ݒ�x���Q */
#define DATA_SET_WARNG3		ERR_DATA_CALC	/* A.94C �f�[�^�ݒ�x���R */
#define DATA_SET_WARNG4		ERR_DATA_SZ		/* A.94D �f�[�^�ݒ�x���S */
#define DATA_SET_WARNG5		ERR_SW_CNG		/* A.94D �f�[�^�ݒ�x���T */

#define CMD_WARNG1			ERR_CMD_CNDTN	/* A.95A �R�}���h�x���P */
#define CMD_WARNG2			ERR_CMD_NOSUP	/* A.95B �R�}���h�x���Q */
#define CMD_WARNG3			ERR_CMD_PRM		/* A.95C �R�}���h�x���R */
#define CMD_WARNG4			ERR_CMD_CNFLCT	/* A.95D �R�}���h�x���S */
#define CMD_WARNG5			ERR_SUBCMD		/* A.95E �R�}���h�x���T */
#define CMD_WARNG6			ERR_FUNC_CNFLCT	/* A.95F �R�}���h�x���U */

/* �A���[����` */
#define CMD_SYNC_FAULT		ERR_SYNC_TOUT	/* A.E51 Phase�ڍs��Ԉُ� */



extern const CFG_TIM_TBL CfgTimTbl[];



#define COM_CFG_H_
#endif /* COM_CFG_H_ */
/***************************************** end of file **********************************************/
