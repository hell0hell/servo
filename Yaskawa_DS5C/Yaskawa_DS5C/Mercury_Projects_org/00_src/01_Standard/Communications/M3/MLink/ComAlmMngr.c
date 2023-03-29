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


#define HIST_NUM		10		/* �A���[���g���[�X�� */



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


/****************************************************************************
 �A���[���Ǘ�����
****************************************************************************/
VOID AlmHandleMain( ALM_HNDL *hAlm )
{

	/* CMD_ALM�����N���A�`�F�b�N */
//	CmdAlmClear( hAlm, pCmdHdl->cmdInfo.CmdAlm, pCmdHdl->exCmdInfo.CmdAlm );

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

	/* �ʒu�f�[�^�����A���[��,M3�ʐM�p�A���[�����X�V */
	hAlm->PosDataNG = ALMGetExNetStatus(hAlm->SvAlmMngr, &hAlm->M3StdAlm);

	/* COMM_ALM��ԂłȂ��ꍇ�ACOMM_ALM�R�[�h���N���A */
	if( hAlm->M3StdAlm.CommAlm == FALSE )
	{
		hAlm->CommAlmCode = 0x00;
	}

//#if 0 /* <S092> */
	/* CMD_ALM��ԂłȂ��ꍇ�ACMD_ALM�R�[�h���N���A */
	if( hAlm->M3StdAlm.CmdAlm == FALSE )
	{
		hAlm->CmdAlmCode   = 0x00;
		hAlm->ExCmdAlmCode = 0x00;
	}
//#endif /* <S092> */
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

	/* COMM_ALM�ݒ� */
	if(0 != (alm_no & ERR_COM_BIT))
	{
		/*------------------------------------------------------------------*/
		/* �OCOMM_ALM=0�̏ꍇ(COMM_ALM���������) �� COMM_ALM�X�V			*/
		/*------------------------------------------------------------------*/
		if(hAlm->CommAlmCode == 0)
		{
			hAlm->CommAlmCode = GET_ERR_RSP(alm_no);
		}
		/*------------------------------------------------------------------*/
		/* �OCOMM_ALM<8�̏ꍇ(�OCOMM_ALM��ܰ�ݸ�) �� �װю��̂�COMM_ALM�X�V	*/
		/*------------------------------------------------------------------*/
		else if(hAlm->CommAlmCode < 0x08)
		{
			if(0 == (alm_no & ERR_WRN_BIT))
			{
				hAlm->CommAlmCode = GET_ERR_RSP(alm_no);
			}
		}
		/*------------------------------------------------------------------*/
		/* �OCOMM_ALM��8�̏ꍇ(�OCOMM_ALM���װ�) �� COMM_ALM�X�V���Ȃ�		*/
		/*------------------------------------------------------------------*/
		else
		{
			;
		}
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

/****************************************************************************/
/*	CMD_ALM�R�[�h�Z�b�g														*/
/*																			*/
/*	�p�l�I�y�\���p���[�j���O�R�[�h�X�V�ɂ���								*/
/*		CMD_ALM�́A�l�b�g���[�N���W���[���pScanC�������O�ɃN���A			*/
/*		�����ꍇ�����邽�߁A�����ōX�V���Ă����B							*/
/****************************************************************************/
VOID AlmSetCmdAlmCode( ALM_HNDL *hAlm, INT32 alm_no, BOOL ExCmd )
{
	ALMID_T	errIdNo;
	errIdNo	 = (ALMID_T)(alm_no & ERR_SVID_MASK);

	/* �A���[�����o�^ */
	if(NULL == ALMSetServoAlarm(hAlm->SvAlmMngr, errIdNo))
	{/* �A���[���}�X�N�� */
		return;
	}

	/* CMD_ALM�ݒ� */
	if(0 != (alm_no & ERR_CMD_BIT) )
	{
		if( ExCmd != FALSE )
		{
			hAlm->ExCmdAlmCode = GET_ERR_RSP(alm_no);
		}
		else
		{
			hAlm->CmdAlmCode = GET_ERR_RSP(alm_no);
		}
	}
	
	/* COMM_ALM�ݒ� */
	if(0 != (alm_no & ERR_COM_BIT))
	{
		/*------------------------------------------------------------------*/
		/* �OCOMM_ALM=0�̏ꍇ(COMM_ALM���������) �� COMM_ALM�X�V			*/
		/*------------------------------------------------------------------*/
		if(hAlm->CommAlmCode == 0)
		{
			hAlm->CommAlmCode = GET_ERR_RSP(alm_no);
		}
		/*------------------------------------------------------------------*/
		/* �OCOMM_ALM<8�̏ꍇ(�OCOMM_ALM��ܰ�ݸ�) �� �װю��̂�COMM_ALM�X�V	*/
		/*------------------------------------------------------------------*/
		else if(hAlm->CommAlmCode < 0x08)
		{
			if(0 == (alm_no & ERR_WRN_BIT))
			{
				hAlm->CommAlmCode = GET_ERR_RSP(alm_no);
			}
		}
		/*------------------------------------------------------------------*/
		/* �OCOMM_ALM��8�̏ꍇ(�OCOMM_ALM���װ�) �� COMM_ALM�X�V���Ȃ�		*/
		/*------------------------------------------------------------------*/
		else
		{
			;
		}
	}
}


/****************************************************************************
 CMD_ALM�N���A
****************************************************************************/
VOID AlmClearCmdSate( ALM_HNDL *hAlm, BOOL CmdAlm, BOOL ExCmdAlm )
{
	/*----------------------------------------------------------------------*/
	/*	�����N���A�Ȃ�														*/
	/*----------------------------------------------------------------------*/
	if( hAlm->CmdAlmClrMode == FALSE )
	{
		return;
	}
	
	if( (hAlm->CmdAlmCode != 0) || (hAlm->ExCmdAlmCode != 0) )
	{
		if( CmdAlm == FALSE )		/* ���C���R�}���h��͐���I�� */
		{
			if( ExCmdAlm == FALSE )	/* ���C���A�T�u�Ƃ��ɐ���I��*/
			{
				hAlm->SvAlmMngr->NetCmdAlmRst = TRUE;
				hAlm->ExCmdAlmCode = 0x00;
			}
			hAlm->CmdAlmCode = 0x00;
		}
		else
		{
			if( ExCmdAlm == FALSE )	/* �T�u�R�}���h��͐���I�� */
			{
				hAlm->ExCmdAlmCode = 0x00;
			}
		}
	}
}


/****************************************************************************
 CMD_ALM�����N���A���[�h�ݒ菈��
****************************************************************************/
VOID AlmSetCmdAlmClearMode(ALM_HNDL *hAlm, LONG prof, ULONG comCtrl)
{
	if( (prof == M3STD_PROFILE) || (prof == EVTID_PROFILE) )
	{
		if( (comCtrl & 0x1000) != 0 )
		{
			hAlm->CmdAlmClrMode = TRUE;
		}
		else
		{
			hAlm->CmdAlmClrMode = FALSE;
		}
	}
	else
	{
		hAlm->CmdAlmClrMode = FALSE;
	}
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
 �A���[�����ݔ��������擾(�S�āA�Q�o�C�g)
****************************************************************************/
INT32 AlmGetCrtDetailBufAll( ALM_HNDL *hAlm, USHORT *pHist, LONG hist_num )
{
	ALMGetNetAlarmTableDetailCode( hAlm->SvAlmMngr, pHist, hist_num );
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
 �T�[�{�A���[�����������擾(�S�āA�Q�o�C�g)
****************************************************************************/
INT32 AlmGetHistoryDetailAll( ALM_HNDL *hAlm, USHORT *pHist, LONG hist_num )
{
	ALMGetNetAlarmTraceDetailCode( hAlm->SvAlmMngr, pHist, hist_num );
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



