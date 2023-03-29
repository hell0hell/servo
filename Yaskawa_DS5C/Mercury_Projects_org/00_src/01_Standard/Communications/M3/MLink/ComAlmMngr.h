/****************************************************************************
Description	: MECHATROLINK Alarm Manager Interface
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------

****************************************************************************/
#ifndef COM_ALM_MNGR_H

#include "ComCfg.h"
#include "AlarmManager.h"


/* ALM Manager�\���� */
typedef struct ALM_HNDL_ {
	BOOL			almSts;					/* �A���[���X�e�[�^�X */
	BOOL			wrnSts;					/* ���[�j���O�X�e�[�^�X */
	BOOL			clrReq;					/* �A���[���N���A�v�� */
	BOOL			CommAlmBlkCancel;		/* COMM_ALM������BLK�M������OFF(�u���[�L�쓮)�t���O */
	LONG			errCode;				/* �A���[���E���[�j���O�\���R�[�h */
	ALMMAN_HANDLE	*SvAlmMngr;				/* �T�[�{�A���[����� */
	ALM_EX_INFO		M3StdAlm;				/* M3�W���v���t�@�C���p�A���[����� */
//	LONG			OpWrnCode;				/* �p�l�I�y�\���p���[�j���O�R�[�h */
//	LONG			OpWrnTimer;				/* �p�l�I�y�\���p���[�j���O�R�[�h�X�V�^�C�} */
	BOOL			CmdAlmClrMode;			/* CMD_ALM�����N���A���[�h�t���O */
	LONG			CmdAlmCode;				/* CMD_ALM�R�[�h */
	LONG			ExCmdAlmCode;			/* SUBCMD_ALM�R�[�h */
	LONG			CommAlmCode;			/* COMM_ALM�R�[�h */
	BOOL			PosDataNG;				/* �ʒu�f�[�^�����A���[����� */
} ALM_HNDL;


/* ALM Manager�̏��������� */
VOID AlmInit( ALM_HNDL *hAlm, ALMMAN_HANDLE *SvAlmMngr );

/* �A���[���Ǘ����� */
VOID AlmHandleMain( ALM_HNDL *hAlm );

/* �A���[���o�^ */
VOID AlmSetCode( ALM_HNDL *hAlm, LONG ax_no, INT32 alm_no );

/* CMD_ALM�R�[�h�Z�b�g���� */
VOID AlmSetCmdAlmCode( ALM_HNDL *hAlm, INT32 alm_no, BOOL ExCmd );
void AlmSetMask( ALM_HNDL *hAlm, INT32 idx, UCHAR enable );

/* CMD_ALM�����N���A���[�h�ݒ菈�� */
VOID AlmSetCmdAlmClearMode(ALM_HNDL *hAlm, LONG prof, ULONG comCtrl);
/* CMD_ALM�N���A */
VOID AlmClearCmdSate( ALM_HNDL *hAlm, BOOL CmdAlm, BOOL ExCmdAlm );

/* �A���[���N���A */
INT32 AlmSetClearReq( ALM_HNDL *hAlm, BOOL req );

/* �A���[�����ݔ��������擾(INDEX�w��) */
//INT32 AlmGetCrtBuf( ALM_HNDL *hAlm, UCHAR idx, UCHAR *pHist );

/* �A���[�����ݔ��������擾(�S��) */
//INT32 AlmGetCrtBufAll( ALM_HNDL *hAlm, UCHAR *pHist );

/* �A���[�����ݔ��������擾(�S�āA�Q�o�C�g) */
INT32 AlmGetCrtDetailBufAll( ALM_HNDL *hAlm, USHORT *pHist, LONG hist_num );

/* �T�[�{�A���[�����������擾(INDEX�w��) */
//INT32 AlmGetHistory( ALM_HNDL *hAlm, UCHAR idx, UCHAR *pHist );

/* �T�[�{�A���[�����������擾(�S��) */
//INT32 AlmGetHistoryAll( ALM_HNDL *hAlm, UCHAR *pHist );

/* �T�[�{�A���[�����������擾(�S�āA�Q�o�C�g) */
INT32 AlmGetHistoryDetailAll( ALM_HNDL *hAlm, USHORT *pHist, LONG hist_nu );

void AlmRequestAlarmHistoryClear( ALM_HNDL *hAlm );
INT32 AlmGetAlarmHistoryClearStatus( ALM_HNDL *hAlm );



#define COM_ALM_MNGR_H
#endif /* COM_ALM_MNGR_H */



