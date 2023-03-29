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
	LONG			errCode;				/* �A���[���E���[�j���O�\���R�[�h */
	BOOL			CommAlmBlkCancel;		/* COMM_ALM������BLK�M������OFF(�u���[�L�쓮)�t���O */
	ALMMAN_HANDLE	*SvAlmMngr;				/* �T�[�{�A���[����� */
} ALM_HNDL;


/* ALM Manager�̏��������� */
VOID AlmInit( ALM_HNDL *hAlm, ALMMAN_HANDLE *SvAlmMngr );

/* �A���[���Ǘ����� */
VOID AlmHandleMain( ALM_HNDL *hAlm );

/* �A���[���o�^ */
VOID AlmSetCode( ALM_HNDL *hAlm, LONG ax_no, INT32 alm_no );
/* �A���[���}�X�N�Z�b�g */
void AlmSetMask( ALM_HNDL *hAlm, INT32 idx, UCHAR enable );

/* �A���[���N���A */
INT32 AlmSetClearReq( ALM_HNDL *hAlm, BOOL req );

/* �A���[�����ݔ��������擾(INDEX�w��) */
INT32 AlmGetCrtBuf( ALM_HNDL *hAlm, UCHAR idx, UCHAR *pHist );

/* �A���[�����ݔ��������擾(�S��) */
INT32 AlmGetCrtBufAll( ALM_HNDL *hAlm, UCHAR *pHist );

/* �T�[�{�A���[�����������擾(INDEX�w��) */
INT32 AlmGetHistory( ALM_HNDL *hAlm, UCHAR idx, UCHAR *pHist );

/* �T�[�{�A���[�����������擾(�S��) */
INT32 AlmGetHistoryAll( ALM_HNDL *hAlm, UCHAR *pHist );

void AlmRequestAlarmHistoryClear( ALM_HNDL *hAlm );
INT32 AlmGetAlarmHistoryClearStatus( ALM_HNDL *hAlm );



#define COM_ALM_MNGR_H
#endif /* COM_ALM_MNGR_H */



