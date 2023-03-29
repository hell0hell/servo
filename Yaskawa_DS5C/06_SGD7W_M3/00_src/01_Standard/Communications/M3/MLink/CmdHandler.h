#ifndef CMD_HANDLER_H_
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		CmdHandler.h : CMD_HNDL��`																	*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	����	2010.02.23	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "M3StdDataFormat.h"
#include "CmdTable.h"
#include "SysApi.h"

/****************************************************************************************************/
/*		STRUCT DEFINITION																			*/
/****************************************************************************************************/


/* ���C���R�}���h��͗p�\���� */
typedef struct CMD_INFO_ {
	CMD_LINK_DATA 	cmdBuf;					/* ��͗p���C��CMD�o�b�t�@ */
	CMD_LINK_DATA	rsltBuf;				/* �R�}���h��͌��� */
	ULONG			busyCnt;				/* �R�}���h�r�W�[����[��͎���] */
	LONG			crtCmd;					/* ���݃R�}���h�R�[�h */
	LONG			profileType;			/* �v���t�@�C���^�C�v */
	BOOL			busy;					/* �R�}���h���s���t���O(�R�}���h���`�F�b�N�p) */
	BOOL			enableCmdAna;			/* �R�}���h��͎��s�v���t���O */
	BOOL			cmdRdy;					/* �R�}���h�̃R�}���h���f�B�r�b�g */
	BOOL			warng;					/* ���[�j���O�r�b�g */
	/* M3Std�v���t�@�C���p */
	ULONG			lstSvCmdCtrl;			/* SVCMD_CTRL�O��l */
	INT32			ResultCmdAna;			/* �R�}���h��͎��s����	*/
	LONG			lstCmdCtrl;				/* CMD_CTRL�O��l */	
	LONG			homingType;				/* ���_���A�^�C�v */	
	LONG			ltSelR[2];				/* ���b�`�M���I����� */
	BOOL			cmdPauseEnable;			/* CMD_PAUSE �L���t���O */
	BOOL			cmdCancelEnable;		/* CMD_CANCEL �L���t���O */
	BOOL			almClrCmp;				/* �A���[���N���A������� 1:�A���[���N���A���� 0:���L�ȊO�̏�� */
	BOOL			ltSeqEnable;			/* �A�����b�`�L���t���O */
	BOOL			CmdAlm;					/* CMD_ALM��ԃt���O */
	BOOL			reLatchReq;				/* ���b�`�ėv���t���O */
	REG_DATA		RegData;				/* ���W�X�^�A�N�Z�X�p */
	LONG			RegWkBuf[10];			/* ���W�X�^�A�N�Z�X�p�o�b�t�@ */
} CMD_INFO;


/* �T�u�R�}���h��͗p�\���� */
typedef struct EX_CMD_INFO_ {
	CMD_LINK_EXDATA	cmdBuf;					/* ��͗p�T�uCMD�o�b�t�@ */
	CMD_LINK_EXDATA	rsltBuf;				/* �R�}���h��͌��� */
	LONG			busyCnt;				/* �R�}���h�r�W�[����[��͎���] */
	BOOL			busy;					/* �R�}���h���s���t���O(�R�}���h���`�F�b�N�p) */
	BOOL			enableCmdAna;			/*�R�}���h��͎��s�v���t���O */
	BOOL			cmdRdy;					/* �R�}���h���f�B�r�b�g */
	BOOL			warng;					/* ���[�j���O�r�b�g */
	INT32			ResultCmdAna;			/* �R�}���h��͌���	*/
	BOOL			CmdAlm;					/* CMD_ALM��ԃt���O */
	REG_DATA		RegData;				/* ���W�X�^�A�N�Z�X�p */
	LONG			RegWkBuf[10];			/* ���W�X�^�A�N�Z�X�p�o�b�t�@ */
} EX_CMD_INFO;


/* CONFIG/SYNC_SET/CONNECT/DISCONNECT */
typedef enum CMD_SEQ_TYPE {	
	IDOL_SEQ=0,								/* IDOL�����V�[�P���X */
	SV_OFF_SEQ,								/* �T�[�{�I�t�����V�[�P���X */
	BK_OFF_SEQ, 							/* �u���[�L�I�t�����V�[�P���X */
	SENS_OFF_SEQ, 							/* �Z���X�I�t�����V�[�P���X */
	MT_DISABLE_SEQ, 						/* ���[�V�������������V�[�P���X */
	PRM_RE_CALC_SEQ,						/* �p�����[�^�Čv�Z�����V�[�P���X */
	MT_ENABLE_SEQ, 							/* ���[�V�����L�������V�[�P���X */
	REQ_PHSUP_SEQ, 							/* �t�F�[�Y�A�b�v�����V�[�P���X */
	REQ_PHSDOWN_SEQ, 						/* �t�F�[�Y�_�E�������V�[�P���X */
	SOFT_RESET_SEQ, 						/* �\�t�g���Z�b�g�����V�[�P���X */
	WAIT_SOFT_RESET_SEQ, 					/* �\�t�g���Z�b�g���s�҂��V�[�P���X */
	PHASE_CHK_SEQ,							/* �t�F�[�Y�m�F�����V�[�P���X */
	FAIL_END_SEQ, 							/* �������s�V�[�P���X */
	PRM_RESET_SEQ,							/* �p�����[�^�������V�[�P���X */
	PRM_ACCESS_SEQ,							/* �p�����[�^�A�N�Z�X�V�[�P���X */
}CMD_SEQ_TYPE;


/* CMD Analyzer �n���h�� */
typedef struct CMD_HNDL_ {
	CMD_INFO		cmdInfo;				/* ���C���R�}���h��� */
	EX_CMD_INFO		exCmdInfo;				/* �T�u�R�}���h��� */
	CMD_SEQ_TYPE	cmd_seq;				/* �R�}���h��̓V�[�P���X�ԍ� */
	BOOL			useExCmd;				/* �T�u�R�}���h�g�p�t���O */
	CMD_DEF_TBL		*cmdTblBasePtr;			/* �R�}���h��`�e�[�u���̃|�C���^ */
	EX_CMD_DEF_TBL	*exCmdTblBasePtr;		/* �T�u�R�}���h��`�e�[�u���̃|�C���^*/
	UCHAR			*CmdIdxTbl;				/* �R�}���h�C���f�b�N�X�e�[�u�� */
} CMD_HNDL;



#define CMD_HANDLER_H_
#endif /* CMD_HANDLER_H_ */
/***************************************** end of file **********************************************/
