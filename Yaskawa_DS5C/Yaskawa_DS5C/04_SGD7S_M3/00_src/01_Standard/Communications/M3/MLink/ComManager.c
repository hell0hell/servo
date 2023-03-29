/****************************************************************************
Description	: MECHATROLINK Communication Manager
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : Mercury

Functions     :
 ComInit()				:COM Manager����������		
 ComInitNetwork()		:�ʐM�f�o�C�X�̏���������		
 ComReqPhaseUp()		:�ʐMPhase�J��(Phase�A�b�v)�v������
 ComReqPhaseDown()		:�ʐMPhase�J�ڗv��(Phase�_�E��)����		
 ComRunPhaseChange()	:�ʐMPhase�J�ڎ��s����				
 ComReceiveCmd()		:���ǎ�M�X�e�[�^�X�擾����		
 ComSendRsp()			:���ǃf�[�^�̑��M(�`�r�h�b�ւ̃��C�g)����		
 ComSetRspBuffer()		:�X���[�uWDC�J�E���g�A�b�v�A���M�pRSP�o�b�t�@�|�C���^�ؑւ�����	
 ComGenerateAplCycle()	:�ʐM�����T�C�N���̐�������				
 ComCheckNetError()		:�ʐM�G���[(��M�G���[�AWDC�G���[)�̌��o����	
 ComSetWdtOfs()			:WDT�I�t�Z�b�g�ʒu�ݒ菈��				
 ComSetExCmdOfs()		:�T�u�R�}���h�I�t�Z�b�g�ʒu�ݒ菈��	
 ComSetFcsErrSts()		:FCS�G���[��ԃZ�b�g����					

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
I.M.   2010.10.18     created 

****************************************************************************/
#include "Basedef.h"
#include "KLib.h"
#include "MLib.h"
#include "ComManager.h"
#include "ComCfg.h"

/* Local functions */
/* mWDC�̃G�b�W���o����	*/			
//static VOID comDetectEdegeOfmWdc( COM_HNDL *pComHdl, INT32 RecvErr );
static VOID comDetectEdegeOfmWdc( COM_HNDL *pComHdl );
/* �ʐMPhase�P�ւ̑J�ڎ��s���� */
static	INT32	comRunPhaseDown( COM_HNDL *hdl );
/* �ʐMPhase�Q�C�R�ւ̑J�ڎ��s���� */
static	INT32	comRunPhaseUp( COM_HNDL *hdl );
/* �`������,�ʐM�����͈̔̓`�F�b�N���� */
static	INT32	comChkCycle( LONG t_cyc, LONG com_tim );
/* �`���������ʐM��`�e�[�u���ɓo�^����Ă��邩�̃`�F�b�N���� */
//static	INT32	comGetCfgTbl( USHORT t_cyc, CFG_TIM_TBL *cfg_tbl_ptr );
/* WDC�G���[�̌��o���� */
static	LONG comChkWdtErr( UCHAR mWdc, UCHAR *lmWdc, BOOL mask );
/* ��M�G���[�̌��o���� */
static	LONG comChkRecvErr( LONG errCtr, LONG dtmod, LONG rcvErr, LONG lrcvErr );


/****************************************************************************/
/* �T�v		:	COM Manager���������s��										*/
/****************************************************************************/
/* ����		:	COM_HNDL* hdl					�ʐM�}�l�[�W���̃n���h��	*/
/*			:	LONG	ax_no					��No.						*/
/* �߂�l	:	�Ȃ�														*/
/****************************************************************************/
VOID ComInit( COM_HNDL *hdl, LONG ax_no )
{
	/* �O���[�o���f�[�^���[���N���A */
	MlibResetByteMemory( hdl, sizeof(COM_HNDL) );

	/* ���ǎ��ԍ��Z�b�g */
	hdl->my_ax_number = ax_no+1;

	/* �o�b�t�@�|�C���^�Z�b�g */
	hdl->CmdBufPtr = hdl->CmdBuffer;
	hdl->RspBufPtr = hdl->RspBuffer;

	/* WDT�I�t�Z�b�g�ʒu */
	hdl->wdtOfs = M3STD_WDT_OFS;
	/* �T�u�R�}���h�I�t�Z�b�g�ʒu */
	hdl->exCmdOfs = M3STD_EX_CMD_OFS;

	/* �ʐM���[�h�����ݒ� */
	hdl->NetPrm.aplCycle = 1;
	hdl->NetPrm.tType = SINGLE_TRANSFER;
	hdl->transMode = SINGLE_TRANSFER;

	/* Phase�����ݒ� */
	hdl->Phase = 0;
	
}

/****************************************************************************/
/* �T�v		�F�ʐM���[�h�p�����[�^�̐ݒ���s��								*/
/****************************************************************************/
/* ����		�F	COM_HNDL* hdl	�ʐM�}�l�[�W���̃n���h��					*/
/* �߂�l	�F�Ȃ�															*/
/****************************************************************************//* <S07A> */
VOID ComNetPrmSet( COM_HNDL *hdl, ND_PARAM	nd_prm )
{
	/* �ʐM���[�h�����ݒ� */
	hdl->NetPrm.dataSize = nd_prm.DataSize;
	hdl->NetPrm.nodeAddress = nd_prm.NodeAddress;
}

/****************************************************************************/
/* �T�v		:	�ʐMPhase�J�ڗv���iPhase�A�b�v�j���s���B					*/
/****************************************************************************/
/* ����		:	COM_HNDL* hdl	�ʐM�}�l�[�W���̃n���h��					*/
/*				UCHAR phs �F�J�ڐ�Phase(1,2,3)								*/
/*		    	UCHAR mode�F��1 �P���A=1 �A���iphs=1�̏ꍇ�j				*/
/*				USHORT apl_cyc 			�F �ʐM���� [unit: �`�������̔{��]	*/
/* �߂�l 	: = COM_PHS_STATE_ERROR  	: �t�F�[�Y�ڍs��Ԉُ�				*/
/*		  	   = COM_FUNC_NORMAL_END	: ����I��							*/
/****************************************************************************/
INT32 ComReqPhaseUp( COM_HNDL *hdl, LONG phs, LONG mode, LONG aplCyc, BOOL AsyncMode )
{
	/* Phase�ڍs���́A�G���[ */
	if( hdl->reqPhase != 0 )
	{
		return COM_PHASE_ERROR;
	}
	/* Phase�����ɂ������Ă���ꍇ�́A����I�� */
	if( phs <= hdl->Phase )
	{
		return COM_FUNC_NORMAL_END;
	}

	if( AsyncMode == FALSE )
	{ /* �����ʐM���[�h */
		/* Phase Up�v�� */
		hdl->NetPrm.tType    = mode;
		hdl->NetPrm.aplCycle = aplCyc;
		hdl->stateOfPhUp = PH_CHK_COM_CYCLE;	/* �ʐM�����`�F�b�N */

		KlibRstLongTimer(&hdl->startTime);
		hdl->PhUpSts = 0;
		hdl->reqPhase = phs;
	}
	else
	{ /* �񓯊��ʐM���[�h(�񓯊��ʐM���� Phase2 �� Phase3 �ւ̈ڍs�Ȃ�) */
		hdl->NetPrm.tType    = SINGLE_TRANSFER;		/* �P���ݒ������(���g�p) */
		hdl->NetPrm.aplCycle = 1;
		hdl->Phase = 2;								/* �񓯊�����Phase2�ֈڍs */
	}

	return COM_FUNC_NORMAL_END;
}



/****************************************************************************/
/* �T�v		:	�ʐMPhase�J�ڗv���iPhase�_�E���j���s���B					*/
/****************************************************************************/
/* ����		:	COM_HNDL* hdl	�ʐM�}�l�[�W���̃n���h��					*/
/* 			:	LONG phs		�J�ڐ�Phase(1,2,3)							*/
/* �߂�l	:	= COM_PHASE_ERROR  	: �J�ڐ�ُ�							*/
/*				= COM_FUNC_NORMAL_END 	: ����I��							*/
/****************************************************************************/
INT32 ComReqPhaseDown( COM_HNDL *hdl, LONG phs )
{
	/* �ڍs��Phase�̐ݒ�(���ႢPhase��) */
	if( (hdl->reqPhase != 0) && (hdl->reqPhase < phs) )
	{
		phs = hdl->reqPhase;
	}

	/* �J�ڐ�Phase = ����Phase���́A���̂܂܏I�� */
	if( (hdl->reqPhase == 0) && (phs == hdl->Phase) )
	{
		return COM_FUNC_NORMAL_END;
	}

	/* �J�ڐ�Phase > ����Phase�̏ꍇ�́A�G���[ */
	if( phs > hdl->Phase )
	{
		return COM_PHASE_ERROR;
	}

	/* Phase Down�v�� */
	if( phs == 1 )
	{
		hdl->NetPrm.aplCycle = 1;
		hdl->NetPrm.tType = SINGLE_TRANSFER;
		hdl->recvErrChk = FALSE;			/* �ʐM�G���[���o���� */
		hdl->Phase = phs;					/* Phase 1�Z�b�g */
	}
	else if( phs == 2 )
	{
		hdl->Phase = phs;					/* Phase 2�Z�b�g */
	}

	hdl->stateOfSync = DISABLE_SYNC;		/* �ʐM��������������Ԃ� */
	hdl->transMode = SINGLE_TRANSFER;		/* SINGLE���[�h */
	hdl->wdcErrChk = FALSE;					/* WDC�G���[���o���� */

	return COM_FUNC_NORMAL_END;
}



/****************************************************************************/
/* �T�v�F�ʐMPhase�J�ڎ��s�������s���B										*/
/****************************************************************************/
/* ����		�F	COM_HNDL* hdl	�ʐM�}�l�[�W���̃n���h��					*/
/* �߂�l	�F	�Ȃ�														*/
/****************************************************************************/
VOID ComRunPhaseChange( COM_HNDL *hdl )
{
	INT32 rc;

	rc = 0;

	if( hdl->reqPhase != 0 )
	{ /* �t�F�[�Y�ڍs�v������̏ꍇ */
		switch( hdl->reqPhase )
		{
		case 1:	/* Phase1�ւ̈ڍs */
			rc = comRunPhaseDown( hdl );
			break;
		case 2:	/* Phase2�ւ̈ڍs */
		case 3:	/* Phase3�ւ̈ڍs */
			rc = comRunPhaseUp( hdl );
			break;
		default:
			hdl->stateOfPhUp = PH_IDOL;
			hdl->reqPhase = 0;
			hdl->PhUpSts = COM_PHASE_ERROR;
			break;
		}

		/*�����m�F */
		if( rc != COM_FUNC_CONTINUE )
		{
			hdl->reqPhase = 0;
			hdl->stateOfPhUp = PH_IDOL;
		}
	}
}



/****************************************************************************/
/* �T�v�F�ʐMPhase�P�ւ̑J�ڎ��s�������s���B								*/
/****************************************************************************/
/* ����		�F	COM_HNDL* hdl	�ʐM�}�l�[�W���̃n���h��					*/
/* �߂�l	�F	�t�F�[�Y�ڍs���											*/
/*				= COM_FUNC_NORMAL_END	: ����I��							*/
/*				= COM_FUNC_CONTINUE		: �p�����s��						*/
/****************************************************************************/
static	INT32 comRunPhaseDown( COM_HNDL *hdl )
{
	hdl->NetPrm.aplCycle = 1;
	hdl->NetPrm.tType = SINGLE_TRANSFER;
	hdl->transMode = SINGLE_TRANSFER;
	hdl->stateOfSync = DISABLE_SYNC;
	hdl->PhUpSts = 0;
	hdl->Phase = 1;

	return COM_FUNC_NORMAL_END;
}



/****************************************************************************/
/* �T�v�F�ʐMPhase�Q�C�R�ւ̑J�ڎ��s�������s���B							*/
/****************************************************************************/
/* ����		�F	COM_HNDL* hdl	�ʐM�}�l�[�W���̃n���h��					*/
/* �߂�l	�F	�t�F�[�Y�ڍs���											*/
/*				= COM_FUNC_NORMAL_END	: �I��								*/
/*				= COM_FUNC_CONTINUE		: �p�����s��						*/
/****************************************************************************/
static	INT32 comRunPhaseUp( COM_HNDL *hdl )
{
	INT32 sts, rc;
	USHORT t_cyc;

	switch( hdl->stateOfPhUp )
	{
	case PH_CHK_COM_CYCLE:		/* �`������, �ʐM�����`�F�b�N */
		t_cyc = NdGetTransmissionCycle();
		sts = comChkCycle( t_cyc, hdl->NetPrm.aplCycle );
		if( sts != COM_FUNC_NORMAL_END )
		{ /* �`������, �ʐM�����ݒ� �ُ� */
			hdl->PhUpSts = sts;									/* �G���[�R�[�h�Z�b�g */
			hdl->Phase = 1;										/* Phase 1�Z�b�g */
			hdl->NetPrm.aplCycle = 1;							/* �ʐM����=�`�������Z�b�g */
			rc = sts;
		}
		else
		{
			if( hdl->reqPhase == 3 )
			{ /* Phase3�ւ̈ڍs�� */
				hdl->stateOfSync = START_COMCYC; 				/* WDC�����J�n */
				hdl->stateOfPhUp = PH_SYNC_COMCYC;				/* �ʐM�����^�C�~���O���o��Ԃ� */
				rc = COM_FUNC_CONTINUE;
			}
			else
			{ /* Phase2�ւ̈ڍs�� */
				hdl->stateOfSync = DISABLE_SYNC;				/* �ʐM������������ */
				hdl->transMode = SINGLE_TRANSFER;				/* Single�]�����[�h */
				hdl->recvErrChk = TRUE;							/* �ʐM�G���[���o�L�� */
				hdl->Phase = hdl->reqPhase;						/* Phase�Z�b�g */
				rc = COM_FUNC_NORMAL_END;
			}
		}
		break;

	case PH_SYNC_COMCYC:	/* �ʐM�����^�C�~���O���o */
		if( hdl->stateOfSync == ENA_WDCCHK )
		{ /* �ʐM�����^�C�~���O���o���� */
			hdl->recvErrChk = TRUE;								/* �ʐM�G���[���o�L�� */
			hdl->Phase = hdl->reqPhase;							/* Phase�Z�b�g */
			rc = COM_FUNC_NORMAL_END;
		}
		else
		{
			rc = COM_FUNC_CONTINUE;
			if( KlibGetLongTimerMs(hdl->startTime) > PH_UP_TIMEOUT ) /* unit:[ms] */
			{ /* �^�C���A�E�g�� */
				hdl->stateOfSync = DISABLE_SYNC;				/* �ʐM������������ */
				hdl->transMode = SINGLE_TRANSFER;				/* Single�]�����[�h */
				hdl->recvErrChk = TRUE;							/* �ʐM�G���[���o�L�� */

				hdl->PhUpSts = COM_WDTOUT_ERR;
				hdl->Phase = 2;
				rc = COM_WDTOUT_ERR;
			}
		}
		break;

	default:
		rc = COM_FUNC_NORMAL_END;
		break;
	}

	return rc;
}



/****************************************************************************/
/* �T�v�F�`������,�ʐM�����͈̔̓`�F�b�N���s���B							*/
/****************************************************************************/
/* ����		�Ft_cyc	�F�`������[0.25us]										*/
/* ����		�Fcom_cyc	�F�ʐM����[�`������]								*/
/* �߂�l	�F= COM_FUNC_NORMAL_END	�F�`����������							*/
/*			�F= COM_TCYC_ERR		�F�`�������ُ�							*/
/****************************************************************************/
static	INT32 comChkCycle( LONG t_cyc, LONG com_tim )
{
	ULONG com_cyc;
	const CFG_TIM_TBL *pTbl;

	pTbl = &CfgTimTbl[0];

	/* �`��������ʐM��`�e�[�u���Ń`�F�b�N */
	while( pTbl->SuptCycle != 0xFFFF )
	{
		if( pTbl->SuptCycle == t_cyc )
		{ /* �T�|�[�g�`������ */
			com_cyc = t_cyc * com_tim;
			if( com_cyc > MAX_COM_APL_CYC )
			{ /* �ő�l�I�[�o */
				return COM_APLCYC_ERR;
			}
			if( com_cyc % (pTbl->SvIfDataCyc*CFG_TIO_BASE_TIME) )
			{ /* ScanN�̐����{�� */
				return COM_APLCYC_ERR;
			}

			return COM_FUNC_NORMAL_END;
		}
		pTbl++;
	}
	return COM_TCYC_ERR;
}


#if 0
/****************************************************************************/
/* �T�v�F�`���������ʐM��`�e�[�u���ɓo�^����Ă��邩���`�F�b�N����B		*/
/****************************************************************************/
/* ����		�Ft_cyc	 �F�O���[�o���f�[�^����擾�����`������[0.25us]			*/
/*			�FpCfgtbl�F�ʐM��`�e�[�u��(�f�[�^�Q�Ɨp)						*/
/* �߂�l	�F= COM_FUNC_NORMAL_END	�F�`����������							*/
/*			�F= COM_TCYC_ERR		�F�`�������ُ�							*/
/****************************************************************************/
static	INT32 comGetCfgTbl( USHORT t_cyc, CFG_TIM_TBL *cfg_tbl_ptr )
{
	INT32 ret;
	const CFG_TIM_TBL *tbl_ptr;

	/* ���[�J���ϐ��̏����� */
	ret = COM_TCYC_ERR;
	tbl_ptr = &CfgTimTbl[0];

	/* �`��������ʐM��`�e�[�u���Ń`�F�b�N */
	while( tbl_ptr->SuptCycle != 0xFFFF )
	{
		if( tbl_ptr->SuptCycle == t_cyc )
		{
			*cfg_tbl_ptr = *tbl_ptr;
			ret = COM_FUNC_NORMAL_END;
			break;
		}
		tbl_ptr++;
	}
	return ret;
}
#endif


#if 0
/****************************************************************************************/
/* �T�v�F���ǎ�M�X�e�[�^�X���擾���A�����M���Ă���Ύ��ǃf�[�^�̎�M���s���B			*/
/****************************************************************************************/
/* ����		�F	MLNET_HANDLE* hNetData					�����p�n���h�������\����		*/
/*				JL100_CPU_EXCHNG_DATA* exchng_data_ptr	�����p�m�[�h���ʕϐ��\����		*/
/* �߂�l	�F  = COM_FUNC_NORMAL_END		:			�����M						*/
/*				= COM_FUNC_ERROR_END		:			��M�ُ�						*/
/* �G���[�X�e�[�^�X																		*/
/*				= COM_RECV_ERR				:			�w�߃f�[�^�̎�M�ُ�			*/
/*				= COM_RECV_GBL_ERR			:			�O���[�o���f�[�^�̎�M�ُ�		*/
/*				= COM_RECV_SYNC_ERR			:			�����ʐM�ُ�					*/
/****************************************************************************************/
INT32 ComReceiveCmd( COM_HNDL *hComMngr )
{
	INT32	rc;

	/* �O�񎩋ǎ�M��Ԃ̕ۑ� */
	hComMngr->LrecvErr = hComMngr->RecvErr;

	/* �w�߃f�[�^�̎�M����(�߂�l�͎�M�ُ펲��) */
	if(NdReceiveCMD( hComMngr->my_ax_number, hComMngr->CmdBufPtr ) > 0)
	{/* �����ް������M	*/
		hComMngr->RecvErr = FALSE;
		rc = COM_FUNC_NORMAL_END;
	}
	else
	{
		hComMngr->RecvErr = TRUE;
		rc = COM_RECV_ERR;
	}
	
	return rc;
}

/****************************************************************************************/
/* �T�v�F���ǃf�[�^�̑��M�i�`�r�h�b�ւ̃��C�g�j���s���B									*/
/****************************************************************************************/
/* ����		:	COM_HNDL* hdl							�ʐM�}�l�[�W���̃n���h��		*/
/* �߂�l   :	�Ȃ�																	*/
/****************************************************************************************/
VOID ComSendRsp( COM_HNDL *hComMngr )
{
	NdSendRSP( hComMngr->my_ax_number, hComMngr->RspBufPtr );
}
#endif

/****************************************************************************/
/* �T�v�F�X���[�u�v�c�b���J�E���g�A�b�v���A���M�p�q�r�o�o�b�t�@�|�C���^��	*/
/*		 �ؑւ�B															*/
/****************************************************************************/
/* ����		:	COM_HNDL* hdl	�ʐM�}�l�[�W���̃n���h��					*/
/* �߂�l   :	�Ȃ�														*/
/****************************************************************************/
VOID ComSetRspBuffer( COM_HNDL *pComHdl )
{
	UCHAR	*sWdt;

	sWdt = ( (UCHAR*)pComHdl->RspBufPtr + pComHdl->wdtOfs );

	/* �X���[�u�v�c�b���J�E���g�A�b�v */
	pComHdl->sWdc = (UCHAR)(pComHdl->sWdc + 1);
	*sWdt = (UCHAR)( (pComHdl->sWdc << 4) | (pComHdl->mWdc & 0xF) );

}

/****************************************************************************/
/* �T�v�F���X�|���X�f�[�^�쐬�^�C�~���O�擾									*/
/****************************************************************************/
/* ����		:	COM_HNDL* hdl					�ʐM�}�l�[�W���̃n���h��	*/
/* �߂�l   :	FALSE/TRUE													*/
/****************************************************************************/
BOOL ComCheckRspTiming( COM_HNDL *pComHdl )
{
	BOOL		rsp_timing;
	INT			apl_cycle;
	INT			rsp_time;
	const INT	rspTimTbl[DTMOD_NUM][5] =
	{  /* 1  2  3  4  5   */
		{ 0, 1, 0, 1, 2 },  /* single mode */
	    { 0, 1, 1, 3, 0 },  /* dual mode */
	    { 0, 0, 2, 0, 1 }   /* dual&single mode */
	};

	apl_cycle = pComHdl->NetPrm.aplCycle-1;
	rsp_time = ((apl_cycle < 5) ?
				rspTimTbl[pComHdl->transMode][apl_cycle] :
				rspTimTbl[pComHdl->transMode][4] + (apl_cycle - 4));

	rsp_timing = (pComHdl->comCycCnt == rsp_time) ? TRUE: FALSE;
	
	return rsp_timing;
}

/****************************************************************************/
/* �T�v�F�ʐM�����T�C�N���̐������s���B										*/
/****************************************************************************/
/* ����		:	COM_HNDL* hdl					�ʐM�}�l�[�W���̃n���h��	*/
/* �߂�l   :	�Ȃ�														*/
/****************************************************************************/
//VOID ComGenerateAplCycle( COM_HNDL *pComHdl, INT32 RecvErr )
VOID ComGenerateAplCycle( COM_HNDL *pComHdl )	/* ����ނ̎�M��IntSync�ŏ��������{���Ă��邽�߁ARecvErr�͍\���̂Ɋi�[�ς�	*/
{
	/* �ʐM�����J�E���^���J�E���g�A�b�v */
	pComHdl->comCycCnt++;

	if( pComHdl->comCycCnt >= pComHdl->NetPrm.aplCycle )
	{
		/* �ʐM�����J�E���^�͒ʐM�����ݒ� - 1 �Ń��Z�b�g */
		pComHdl->comCycCnt = 0;
	}

	/* �}�X�^�v�c�b�̃G�b�W���o���� */
//	comDetectEdegeOfmWdc( pComHdl, RecvErr );
	comDetectEdegeOfmWdc( pComHdl );/* ����ނ̎�M��IntSync�ŏ��������{���Ă��邽�߁ARecvErr�͍\���̂Ɋi�[�ς�	*/
}


/****************************************************************************/
/* �T�v�FmWDC�̃G�b�W���o�������s���B										*/
/****************************************************************************/
/* ����		:	COM_HNDL* hdl	�ʐM�}�l�[�W���̃n���h��					*/
/* �߂�l   :	�Ȃ�														*/
/****************************************************************************/
static VOID comDetectEdegeOfmWdc( COM_HNDL *pComHdl )
{
	LONG wdchk;

	switch( pComHdl->stateOfSync )
	{
	case ENA_WDCCHK:	/* WDC�G���[���o�L��(�ʏ���) */
		break;

	case START_COMCYC:	/* �ʐM�����J�n */
		wdchk = comChkWdtErr( *((UCHAR*)pComHdl->CmdBufPtr + pComHdl->wdtOfs),
//										&pComHdl->mWdc, pComHdl->NetPrm.wdtErrMask );			/* <S0D5> */
										&pComHdl->mWdcForSync, pComHdl->NetPrm.wdtErrMask );	/* <S0D5> */;
		if( !pComHdl->LrecvErr && !pComHdl->RecvErr && (wdchk == 0) )
		{ /* �O��`�������A����`�������Ő����M���v�c�b�̃G�b�W�����o���ꂽ�ꍇ */
			if( pComHdl->NetPrm.tType == SINGLE_TRANSFER )
			{
				/* �P�� */
				pComHdl->comCycCnt = 0;
			}
			else
			{
				/* �A�� */
				pComHdl->comCycCnt = pComHdl->NetPrm.aplCycle - 1;
			}

			pComHdl->stateOfSync = READY_COMCYC;
			pComHdl->wdChkCnt = 0;
		}
		break;

	case READY_COMCYC:	/* �ʐM�����m�F */
		wdchk = comChkWdtErr( *((UCHAR*)pComHdl->CmdBufPtr + pComHdl->wdtOfs),
								&pComHdl->mWdcForSync, pComHdl->NetPrm.wdtErrMask );
		if( !pComHdl->LrecvErr && !pComHdl->RecvErr && (wdchk == 0) )
		{ /* �O��`�������A����`�������Ő����M���v�c�b�̃G�b�W�����o���ꂽ�ꍇ */
			if( (pComHdl->NetPrm.tType == SINGLE_TRANSFER) && (pComHdl->comCycCnt == 0) )
			{ /* �P�� */
				if( (++pComHdl->wdChkCnt) >= 4 )
				{ /* 4��A������ */
					pComHdl->stateOfSync = START_WDCCHK;
					pComHdl->transMode = SINGLE_TRANSFER;
					pComHdl->wdChkCnt = 0;
				}
			}
			else if( (pComHdl->NetPrm.tType == CONSECUTIVE_TRANSFER) 
				&&   (pComHdl->comCycCnt == (pComHdl->NetPrm.aplCycle - 1)) )
			{ /* �A�� */
				if( (++pComHdl->wdChkCnt) >= 4 )
				{ /* 4��A������ */
					pComHdl->stateOfSync = START_WDCCHK;
					pComHdl->transMode = CONSECUTIVE_TRANSFER;
					pComHdl->wdChkCnt = 0;
				}
			}
			else
			{ /* �^�C�~���O��������ꍇ�͈�O�̏�Ԃɖ߂� */
				if( !pComHdl->NetPrm.wdtErrMask )
				{ /* WDC�}�X�N�łȂ��ꍇ�̂݁A���g���C */
					pComHdl->stateOfSync = START_COMCYC;
				}
			}
		}
		break;

	case START_WDCCHK:	/* WDC�G���[���o�J�n */
		if( pComHdl->comCycCnt == 0 )
		{
			/* mWDC�ۑ� */
//			pComHdl->mWdc = *((UCHAR*)pComHdl->CmdBufPtr + pComHdl->wdtOfs);
	
			pComHdl->wdChkCnt++;
			if( pComHdl->wdChkCnt >= 2 )
			{ /* �^�X�N���������肷��̂��Q�����҂� */
				pComHdl->stateOfSync = ENA_WDCCHK;
				pComHdl->wdcErrChk = TRUE;
			}
		}
		break;

	case DISABLE_SYNC:	/* �ʐM�������� */
		pComHdl->wdcErrChk = FALSE;
		break;

	default : break;
	}

}


/****************************************************************************/
/* �T�v		:	�ʐM�G���[�i��M�G���[�A�v�c�b�G���[�j�̌��o���s���B		*/
/****************************************************************************/
/* ����		:	COM_HNDL* hdl	�ʐM�}�l�[�W���̃n���h��					*/
/* �߂�l   : = COM_FUNC_NORMAL_END	�����M								*/
/*			: = COM_RECV_ERR		��M�G���[								*/
/*			: = COM_FCS_ERR			FCS�G���[								*/
/*          : = COM_RECV_WARNG		��M���[�j���O							*/
/*          : = COM_FCS_WARNG		FCS���[�j���O							*/
/*          : = COM_WDC_ERR			WDC�G���[								*/
/****************************************************************************/
INT32 ComCheckNetError( COM_HNDL *pComHdl )/* ����ނ̎�M��IntSync�ŏ��������{���Ă��邽�߁ARecvErr�͍\���̂Ɋi�[�ς�	*/
{
	UCHAR	crtCmd;				/* ��M�f�[�^�̃��C���R�}���h */
	INT32	ret;
	LONG	wdchk;

	/* ���[�J���ϐ��̏����� */
	ret = COM_FUNC_NORMAL_END;

	/* ��M�f�[�^�̃��C���R�}���h�擾 */
	crtCmd = (UCHAR)( pComHdl->CmdBufPtr[0] & 0xff );
	if( crtCmd == CRT_CMD_DISCONNECT )
	{ /* ��M�f�[�^�̃��C���R�}���h�擾 == DISCONNECT */
		return ret;
	}

	/* ��M�G���[�`�F�b�N */
	pComHdl->recvErrCnt = comChkRecvErr( pComHdl->recvErrCnt, pComHdl->transMode, pComHdl->RecvErr, pComHdl->LrecvErr );
	if( pComHdl->recvErrCnt == 0 )
	{ /* �����M */
		wdchk = comChkWdtErr( *((UCHAR*)pComHdl->CmdBufPtr+pComHdl->wdtOfs), &pComHdl->mWdc, pComHdl->NetPrm.wdtErrMask );
		if( (wdchk != 0) && pComHdl->wdcErrChk )
		{ /* WDT�G���[ && WDC�G���[���o�L��*/
			ComReqPhaseDown( pComHdl, 2 );
			ret = COM_WDC_ERR;
		}
	}
	else if( pComHdl->recvErrCnt == COM_RECV_ERR_LIMIT )
	{ /* �A����M�ُ�A���[�� */
		pComHdl->mWdc = (UCHAR)(pComHdl->mWdc + 1);			/* �ʐM�ُ�}�X�N�p�� mWdt+1 �␳ */
		pComHdl->recvErrCntNum = (USHORT)(pComHdl->recvErrCntNum + 1);	/* �G���[�J�E���g�A�b�v */

		if( !pComHdl->NetPrm.recvErrMask && pComHdl->recvErrChk )
		{
			ComReqPhaseDown( pComHdl, 2 );
//			if( pComHdl->RecvErr != 0 )
			{
				ret = COM_RECV_ERR;
			}
		}
	}
	else
	{ /* ��M�ُ탏�[�j���O */
		pComHdl->mWdc = (UCHAR)(pComHdl->mWdc + 1);			/* �ʐM�ُ�}�X�N�p�� mWdt+1 �␳ */
		pComHdl->recvErrCntNum = (USHORT)(pComHdl->recvErrCntNum + 1);	/* �G���[�J�E���g�A�b�v */

		if( !pComHdl->NetPrm.recvWrnMask && pComHdl->recvErrChk )
		{
//			if( pComHdl->RecvErr != 0 )
			{
				ret = COM_RECV_WARNG;
			}
		}
	}
	return ret;
}



/****************************************************************************/
/* �T�v		:	��M�G���[�̌��o���s���B									*/
/****************************************************************************/
/* ����		:	LONG errCnt					��M�G���[�J�E���g�l			*/
/*			:	LONG dtmod					�]������						*/
/*			:	BOOL rcvErr					�ʐM�ُ���					*/
/*			:	BOOL lrcvErr				�O��ʐM�ُ���				*/
/* �߂�l   :	= �G���[�J�E���g(errCnt)									*/
/****************************************************************************/
static	LONG comChkRecvErr( LONG errCtr, LONG dtmod, LONG rcvErr, LONG lrcvErr )
{
	/* ��M���� */
	if( rcvErr == 0 )
	{
		return 0;
	}

	/* ��M�ُ� */
	if( dtmod == CONSECUTIVE_TRANSFER )
	{ /* �A�� */
		if( lrcvErr != 0 )
		{
			errCtr = COM_RECV_ERR_LIMIT;
		}
		else
		{
			errCtr = 0;
		}
	}
	else
	{ /* �P�� */
		errCtr++;
		if( errCtr >= COM_RECV_ERR_LIMIT )
		{
			errCtr = COM_RECV_ERR_LIMIT;		/* �ʐM�ُ� */
		}
	}
	return errCtr;
}



/****************************************************************************/
/* �T�v		:	�v�c�b�G���[�̌��o���s���B									*/
/****************************************************************************/
/* ����		:	LONG  mWdc		�}�X�^WDC									*/
/*			:	LONG* lmWdc		�O��}�X�^WDC�|�C���^						*/
/*			:	BOOL  mask		WDC�}�X�N�ݒ�								*/
/* �߂�l	:	= 0				����					    				*/
/*			:	!= 0			�v�c�b�G���[								*/
/****************************************************************************/
static	LONG comChkWdtErr( UCHAR mWdc, UCHAR *lmWdc, BOOL mask )
{
	LONG ret;

	/* ���[�J���ϐ��̏����� */
	ret = 0;

	if( !mask )
	{
		ret = ( mWdc - (*lmWdc + 1) ) & 0xF;
	}

	*lmWdc = mWdc;
	return ret;
}


/****************************************************************************/
/* �T�v�FWDT�I�t�Z�b�g�ʒu��ݒ肷��										*/
/****************************************************************************/
/* ����		:	COM_HNDL* hdl	�ʐM�}�l�[�W���̃n���h��					*/
/*				LONG tdata_size	�`���o�C�g��								*/
/*				UCHAR wdtOfs	WDT�I�t�Z�b�g�ʒu�i�o�C�g�j					*/
/* �߂�l   : = COM_FUNC_NORMAL_END	�����M								*/
/* 			  = COM_WDT_OFS_ERR		�ݒ菈�����s							*/
/****************************************************************************/
INT32 ComSetWdtOfs( COM_HNDL *pComHdl, LONG tdata_size, LONG wdtOfs )
{
	INT32 ret;

	if( wdtOfs <= tdata_size )
	{
		pComHdl->wdtOfs = wdtOfs;
		ret = COM_FUNC_NORMAL_END;
	}
	else
	{
		ret = COM_WDT_OFS_ERR;
	}
	return ret;
}



/****************************************************************************/
/* �T�v�F�T�u�R�}���h�I�t�Z�b�g�ʒu��ݒ肷��								*/
/****************************************************************************/
/* ����		:	COM_HNDL* hdl	�ʐM�}�l�[�W���̃n���h��					*/
/* ����		:	UCHAR exCmdOfs	�T�u�R�}���h�I�t�Z�b�g�ʒu�i�o�C�g�j		*/
/* �߂�l   : = COM_FUNC_NORMAL_END	�����M								*/
/* 			  = COM_EXCMD_OFS_ERR	�ݒ菈�����s							*/
/****************************************************************************/
INT32 ComSetExCmdOfs( COM_HNDL *pComHdl, LONG tdata_size, LONG exCmdOfs )
{
	INT32 ret;

	if( exCmdOfs <= tdata_size )
	{
		pComHdl->exCmdOfs = exCmdOfs;
		ret = COM_FUNC_NORMAL_END;
	}
	else
	{
		ret = COM_EXCMD_OFS_ERR;
	}
	return ret;
}


#if 0
/****************************************************************************/
/* �T�v�FFCS�G���[��ԃZ�b�g												*/
/****************************************************************************/
/* ����		�F	COM_HNDL* hdl	�ʐM�}�l�[�W���̃n���h��					*/
/* ����		�F	�Ȃ�														*/
/* �߂�l   :   �Ȃ�														*/
/****************************************************************************/
VOID ComSetFcsErrSts( COM_HNDL *hdl )
{
	/* 2010.02.08 Y.Oka SGDV�s��C�����f */
//	hdl->LrecvErr = hdl->RecvErr;	/* �O��l��ۑ� */
	hdl->RecvErr = RECV_FCS_ERR;	/* FCS�ُ��Ԃ�ݒ� */
}
#endif



