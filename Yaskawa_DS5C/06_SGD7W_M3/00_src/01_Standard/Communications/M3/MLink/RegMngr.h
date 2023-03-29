/****************************************************************************
Description	: MECHATROLINK Register and Parameter Access Interface
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : Mercury

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------

****************************************************************************/
#ifndef _ML_REG_MANAGER_H_

#include "ComManager.h"
#include "UnitPrmStruct.h"
#include "MtPrm.h"


#define DEV_MAX_STR			32			/* �^���ő�o�C�g�T�C�Y		��	ID_RD�̃T�C�Y(32)�ɍ��킹�� */


/* ���W�X�^�A�N�Z�X�R�}���h�R�[�h */
typedef enum REG_CMD_TYPE {
	REG_CMD_NONE = 0,					/* 0:No Operation						 */
	REG_CMD_PRMRD,						/* 1:���W�X�^���[�h						 */
	REG_CMD_PRMWR,						/* 2:���W�X�^���C�g						 */
	REG_CMD_RECALC,						/* 3:�p�����[�^�Čv�Z					 */
	REG_CMD_ALMHISTCLR,					/* 4:�T�[�{�A���[�������N���A			 */
	REG_CMD_SYSRST,						/* 5:�V�X�e�����Z�b�g					 */
	REG_CMD_CHGCYC,						/* 6:Task�����ύX�ɂ��p�����[�^�Čv�Z	 */
	REG_CMD_INITPRM,					/* 7:�p�����[�^������					 */
	REG_CMD_IDRD,						/* 8:�p�����[�^������					 */
}REG_CMD_TYPE;


/* ���W�X�^�A�N�Z�X�\���� */
typedef struct REG_DATA_ {
	ULONG		Number;					/* ���W�X�^�ԍ�							 */
	LONG		Length;					/* ���W�X�^��							 */
	LONG		AccessSize;				/* �A�N�Z�X�T�C�Y(1,2,4byte)			 */
	LONG		offset;					/* �I�t�Z�b�g							 *//* <S**1> */
	BOOL		EepWrite;				/* EEPROM���C�g�L��						 */
	LONG		*pBuf;					/* ���W�X�^�f�[�^�i�[��					 */
	LONG		BufferSize;				/* �o�b�t�@�T�C�Y						 */
} REG_DATA;


/* REG Manager�\���� */
typedef struct REG_HNDL_ {
	volatile REG_CMD_TYPE	accCode;	/* �R�}���h�R�[�h						 */
	VLONG			accSts;				/* �X�e�[�^�X							 */
	ULONG			accSeq;				/* ���W�X�^�A�N�Z�X���s�V�[�P���X���	 */
	BOOL			accWaitBusy;		/* ���W�X�^�A�N�Z�XBusy���				 */
	REG_DATA		*accData;			/* ���W�X�^�A�N�Z�X�f�[�^				 */
	LONG			accBuffer[32];		/* ���W�X�^�A�N�Z�X�p�o�b�t�@			 */

	struct {
		MTPRM_INI	mt;					/* ���[�V���������p�����[�^�f�[�^		 */
		UCHAR		rateChgSw;			/* Posing�萔�I�� 						 *//* <S004> */
		UCHAR		monStop;			/* ���j�^��~							 *//* <S0BD> */
	} iniPrm;							/* �����p�����[�^���[�h�f�[�^			 */
//	DEV_INFO		devInfo;			/* �f�o�C�X���							 */
//	DEV_INFO_TBL	devInfoTable[DEV_TBL_SIZE];	/* �f�o�C�X���					 */

	MT_PRM			*mtPrm;				/* ���[�V�����p�����[�^RAM�A�h���X		 */
	COM_NET_PRM		*netPrm;			/* �ʐM�p�����[�^						 */
	PRMDATA			*Prm;				/* �p�����[�^RAM�A�h���X				 */
	void			*hSvRegMngr;		/* ���W�X�^�A�N�X�Z�X�n���h��			 */
} REG_HNDL;



#define _ML_REG_MANAGER_H_
#endif /* _ML_REG_MANAGER_H_ */


