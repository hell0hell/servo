/****************************************************************************
Description	: MECHATROLINK Alarm Manager
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
#include "CmdAnalyzer.h"
#include "ComAlmMngr.h"
#include "MtApi.h"
#include "SysApi.h"
#include "AlarmManager.h"
#include "MLib.h"

#define HIST_NUM	10		/* �A���[���g���[�X�� */

VOID  AlmSetCommAlmBlkCancel(USHORT data);/* <V923> */

#ifdef	__ghs__
#pragma ghs section text=".textEX"
#endif
/****************************************************************************
 ����������
****************************************************************************/
VOID AlmInit( ALM_HNDL *hAlm, ALMMAN_HANDLE *SvAlmMngr )
{
	/* �f�[�^�N���A */
	MlibResetByteMemory( hAlm, sizeof(ALM_HNDL) );

	/* �A���[���Ǘ��f�[�^�ւ̃|�C���^�ݒ� */
	hAlm->SvAlmMngr = SvAlmMngr;
}

#ifdef	__ghs__
#pragma ghs section text=default
#endif
/****************************************************************************
 �A���[���Ǘ�����
****************************************************************************/
VOID AlmHandleMain( ALM_HNDL *hAlm )
{
	/* �A���[���N���A */
	if(FALSE != hAlm->clrReq)
	{
		if(FALSE != ALMGetAlarmResetStatus(hAlm->SvAlmMngr))
		{/* �A���[�����Z�b�g���̓��^�[�� */
			return;
		}
		hAlm->clrReq = FALSE;
	}

	/* �A���[���X�e�[�^�X�X�V */
	hAlm->errCode = ALMGetStatus(hAlm->SvAlmMngr, &hAlm->almSts, &hAlm->wrnSts);

}


/****************************************************************************
 �A���[���o�^
****************************************************************************/
VOID AlmSetCode( ALM_HNDL *hAlm, LONG ax_no, INT32 alm_no )
{
	ALMID_T	errIdNo;
	errIdNo	 = (ALMID_T)(alm_no & ERR_SVID_MASK);

	/* �A���[�����o�^ */
	if(NULL == ALMSetServoAlarm(hAlm->SvAlmMngr, errIdNo))
	{/* �A���[���}�X�N�� */
		return;
	}

}
/****************************************************************************
 �A���[�����o�}�X�N�ݒ菈��
****************************************************************************/
void AlmSetMask( ALM_HNDL *hAlm, INT32 idx, UCHAR enable )
{
	ALMID_T	errIdNo;
	errIdNo	 = (ALMID_T)(idx & ERR_SVID_MASK);

	ALMSetMask(hAlm->SvAlmMngr, errIdNo, (BOOL)enable);	
}

/****************************************************************************
 �A���[���N���A
****************************************************************************/
INT32 AlmSetClearReq( ALM_HNDL *hAlm, BOOL req )
{
	/* �N���A���̏ꍇ�́A�p�����Ń��^�[�� */
	if(FALSE != hAlm->clrReq)
	{
		return ALM_CONTINUE;
	}

	/* �N���A�v�����s */
	if(req != FALSE)
	{ /* �v������ && �G���[���� -> ���Z�b�g�J�n */
		ALMSetAlarmResetRequest(hAlm->SvAlmMngr);
		hAlm->clrReq = TRUE;						/* �N���A�v��ON */
		return ALM_CONTINUE;
	}
	return ALM_SUCCESS;
}


/****************************************************************************
 �A���[�����ݔ��������擾(INDEX�w��)
****************************************************************************/
INT32 AlmGetCrtBuf(  ALM_HNDL *hAlm, UCHAR idx, UCHAR *pHist )
{
	ALMGetNetAlarmTableInfo( hAlm->SvAlmMngr, (USHORT*)pHist, idx );
	return 0;
}


/****************************************************************************
 �A���[�����ݔ��������擾(�S��)
****************************************************************************/
INT32 AlmGetCrtBufAll(  ALM_HNDL *hAlm, UCHAR *pHist )
{
	ALMGetNetAlarmTableCode( hAlm->SvAlmMngr, pHist, HIST_NUM );
	return 0;
}


/****************************************************************************
 �T�[�{�A���[�����������擾(INDEX�w��)
****************************************************************************/
INT32 AlmGetHistory(  ALM_HNDL *hAlm, UCHAR idx, UCHAR *pHist )
{
	ALMGetNetAlarmTraceInfo( hAlm->SvAlmMngr, (USHORT*)pHist, idx );
	return 0;
}


/****************************************************************************
 �T�[�{�A���[�����������擾(�S��)
****************************************************************************/
INT32 AlmGetHistoryAll(  ALM_HNDL *hAlm, UCHAR *pHist )
{
	ALMGetNetAlarmTraceCode( hAlm->SvAlmMngr, pHist, HIST_NUM );
	return 0;
}


/****************************************************************************
 �T�[�{�A���[�����������N���A
****************************************************************************/
void AlmRequestAlarmHistoryClear( ALM_HNDL *hAlm )
{
	ALMSetAlarmHistResetRequest( hAlm->SvAlmMngr );
}
INT32 AlmGetAlarmHistoryClearStatus( ALM_HNDL *hAlm )
{
	return ALMGetAlarmHistResetStatus( hAlm->SvAlmMngr );
}


