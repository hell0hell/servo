/****************************************************************************
Description	: MECHATROLINK System Api��`
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : Mercury

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------

****************************************************************************/
#ifndef SYS_API_H_

#include "Alarm.h"
#include "RegMngr.h"

/* �ʐM��������ύX�ɂ��p�����[�^�Čv�ZAPI�p�\���� */
typedef struct CYC_CHG_PRM_ {
	USHORT		scanCyc;			/* �ʐM�������[0.25us] */
	USHORT		intCyc;				/* �`������[0.25us] */
	USHORT		aplCyc;				/* �ʐM����[x�`������] */
	SHORT		dlyTime;			/* To��Ti�܂ł̎��� */
} CYC_CHG_PRM;


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
/* ���W�X�^����o�b�N�O�����h���� */
VOID RegRunBackgrndProc( REG_HNDL *hReg );

INT32 RegRequestCommand( REG_HNDL *hReg, REG_CMD_TYPE type, REG_DATA *pData );
INT32 RegCheckCommandState( REG_HNDL *hReg );

/* �ʐM���p�����[�^�X�VAPI */
VOID RegSetComPrmMon( REG_HNDL *hReg, USHORT adr, USHORT byte );
/* �ʐM�f�[�^���j�^�X�V */
VOID RegSetComMonData( REG_HNDL *hReg, USHORT *pCmd, USHORT *pRsp );/* <S0BD> */
/* �ʐMASIC�G���[���擾���� */
VOID RegSetComErrData( REG_HNDL *hReg, USHORT cnt, ULONG ests );




#define SYS_API_H_
#endif /* SYS_API_H_ */

