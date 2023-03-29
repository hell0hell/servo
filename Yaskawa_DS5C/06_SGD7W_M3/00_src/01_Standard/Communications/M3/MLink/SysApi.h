/****************************************************************************
Description	: MECHATROLINK System Api��`
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------

****************************************************************************/
#ifndef SYS_API_H_

#include "Alarm.h"
#include "RegMngr.h"


/* �f�o�C�X���A�N�Z�X�\���� */
typedef struct DEV_DATA_ {
	ULONG		type;				/* �f�o�C�X�^�C�v */
	ULONG		ofst;				/* �ǂݏo���I�t�Z�b�g[byte] */
	ULONG		size;				/* �ǂݏo���T�C�Y[byte] */
	DWORDX		*pBuf;				/* �f�o�C�X���i�[�� */
} DEV_DATA;


/* �ʐM��������ύX�ɂ��p�����[�^�Čv�ZAPI�p�\���� */
typedef struct CYC_CHG_PRM_ {
	USHORT		scanCyc;			/* �ʐM�������[0.25us] */
	USHORT		intCyc;				/* �`������[0.25us] */
	USHORT		aplCyc;				/* �ʐM����[x�`������] */
	SHORT		dlyTime;			/* To��Ti�܂ł̎��� */
} CYC_CHG_PRM;


/* �f�o�C�X���擾API �f�o�C�X�R�[�h */
#define	DEV_SV_MODEL			0x00			/* �T�[�{�p�b�N�^�� */
#define	DEV_SV_VER				0x02			/* �T�[�{�p�b�NVer */
#define	DEV_ENC_MODEL			0x10			/* �G���R�[�_�^�� */
#define	DEV_ENC_VER				0x12			/* �G���R�[�_Ver */
#define	DEV_MTR_MODEL			0x20			/* ���[�^�^�� */
#define	DEV_COM_MODEL			0x50			/* �ʐMIF�^�� */
#define	DEV_COM_VER				0x52			/* �ʐMIFVer */
#define	DEV_FBPG_MODEL			0x30			/* �O�����[�^�^�� */
#define	DEV_FBPG_VER			0x32			/* �O���G���R�[�_Ver */
#define	DEV_SAFETY_MODEL		0x60			/* Safety Option�^�� */
#define	DEV_SAFETY_VER			0x62			/* Safety OptionVer */
#define	DEV_FBOP_MODEL			0x70			/* Feedback Option�^�� */
#define	DEV_FBOP_VER			0x72			/* Feedback OptionVer */

// ��  M3Proto �b��d�l
#define DEV_VENDER_ID			0x01			/* �x���_�[ID�R�[�h */
#define DEV_CODE				0x02			/* �f�o�C�X�R�[�h */
#define DEV_VER					0x03			/* �f�o�C�X�o�[�W���� */
#define DEV_PROFILETYPE01		0x10			/* �v���t�@�C���^�C�v�P */
#define DEV_PROFILETYPE02		0x12			/* �v���t�@�C���^�C�v�Q */
#define DEV_MIN_TCYC			0x16			/* �`�������ŏ��l */
#define DEV_TCYC_GRANULARITY	0x18			/* �`���������� */
#define DEV_MIN_APLCYC			0x19			/* �ʐM�����ŏ��l */
#define DEV_TRBYTE_LIST			0x1B			/* �`���o�C�g���i���X�g�j */
#define DEV_TRBYTE				0x1C			/* �`���o�C�g��(���ݒl) */

/* ���W�X�^�A�N�Z�X�^�C�v */
#define REG_NORMAL				0x0				/* �ʏ탌�W�X�^�A�N�Z�X */
#define REG_CANCEL				0x1				/* ���W�X�^�A�N�Z�XCancel */

/* �A���[������API���^�[���R�[�h */
#define ALM_SUCCESS				ERR_NONE		/* ����I�� */
#define ALM_CONTINUE			ERR_WAIT_COMP	/* �p���� */
#define ALM_CNDTN_ERR			ERR_CMD_CNDTN	/* ���s�����ُ� */

/* ���W�X�^�A�N�Z�XAPI���^�[���R�[�h */
#define REG_SUCCESS				ERR_NONE		/* ����I�� */
#define REG_CONTINUE			ERR_WAIT_COMP	/* �p���� */
#define REG_NO_ERR				ERR_PRM_NO		/* ���W�X�^�ԍ��ُ� */
#define REG_SIZE_ERR			ERR_DATA_SZ		/* ���W�X�^�T�C�Y�ُ� */
#define REG_DATA_ERR			ERR_DATA_SET	/* ���W�X�^�f�[�^�ُ͈͈� */
#define REG_CNDTN_ERR			ERR_CMD_CNDTN	/* ���s�����ُ� */
#define REG_TYPE_ERR			ERR_DATA_SET	/* �^�C�v�ُ� */


/* REG Manager�̏��������� */
VOID RegInit( REG_HNDL *hReg, MT_PRM *mtPrm, 
				COM_NET_PRM *netPrm, PRMDATA *pprm_ptr,  void *hRegMngr);
/* �����p�����[�^�̈ꊇ���[�h */
INT32 RegLoadDefaultVal( REG_HNDL *hReg, UNI_PRM_STR *uni_prm_ptr );
/* ����(�I�t���C��)�p�����[�^�̈ꊇ�v�Z */
INT32 RegCalcDefaultVal( REG_HNDL *hReg );
/*  �����p�����[�^���狤�ʃp�����[�^�̌v�Z�����{�֐�	*//* <S027> */
INT32 RegSetComDefaultVal( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm );

/* ���W�X�^����o�b�N�O�����h���� */
VOID RegRunBackgrndProc( REG_HNDL *hReg );

INT32 RegRequestCommand( REG_HNDL *hReg, REG_CMD_TYPE type, REG_DATA *pData );
INT32 RegCheckCommandState( REG_HNDL *hReg );

/* �ʐM���p�����[�^�X�VAPI */
VOID RegSetComPrmMon( REG_HNDL *hReg, USHORT adr, USHORT byte );
/* �ʐM�f�[�^���j�^�X�V *//* <S0BD> */
VOID RegSetComMonData(REG_HNDL* hdl, USHORT* pCmd, USHORT* pRsp, UCHAR prfType);
/* �ʐMASIC�G���[���擾���� *//* <S0BD> */
VOID RegSetComErrData(REG_HNDL* hdl, USHORT cnt, ULONG ests);




#define SYS_API_H_
#endif /* SYS_API_H_ */

