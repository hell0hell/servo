/****************************************************************************
Description	: MECHATROLINK Position Filter
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
#include "AccFilter.h"
#include "MLib.h"



/* �w���������t�B���^�T�u���� */
static	BOOL AcfExpFilter( ACF_HNDL *hdl, LONG *spd );
/* �ړ����σt�B���^�T�u�֐� */
static	BOOL AcfMaFilter( ACF_MAFIL *hdl, LONG *spd );
/* �ړ����σt�B���^���� */
static	BOOL AcfIMaFilter( ACF_HNDL *hdl, LONG *spd );



/****************************************************************************
 ����������
****************************************************************************/
VOID AcfInit( ACF_HNDL *hdl, MC_BASE_PRM *bPrm, ACF_PRM *prm )
{
	hdl->maFilTb = 0;
	hdl->maFil.maTb = 0;
	hdl->maFil.maBuf = hdl->maBuf;
	hdl->maIFil.iTb = 0;
	hdl->maIFil.iTbSft = 0;
	hdl->expBias = 0;
	hdl->expTb = 0;
	hdl->filSw = ACF_SW_NOFIL;
	hdl->Prm = prm;
	hdl->bPrm = bPrm;
	AcfClear( hdl );
}



/****************************************************************************
 �t�B���^��������
****************************************************************************/
VOID AcfGenerate( ACF_HNDL *hdl, LONG *spd, LONG sel, BOOL *den )
{
	BOOL cmp;

	cmp = TRUE;

	if( hdl->filComp )
	{ /* �t�B���^�o�͊�����Ԃ̂ݐ؂�ւ� */
		if( hdl->maFilTb != hdl->Prm->mafil.lTb )
		{
			hdl->filSw = ACF_SW_NOFIL;
			hdl->maFil.maTb = hdl->Prm->mafil.wTb[1];
			hdl->maIFil.iTb = 1 << hdl->Prm->mafil.wTb[0];
			hdl->maIFil.iTbSft = hdl->Prm->mafil.wTb[0] << 1;
			hdl->maFilTb = hdl->Prm->mafil.lTb;
		}
		if( hdl->expTb != hdl->Prm->exptb )
		{
			hdl->filSw = ACF_SW_NOFIL;
			hdl->expTb = hdl->Prm->exptb;
		}
		hdl->expBias = hdl->Prm->expbias;

		if( hdl->filSw != sel )
		{
			AcfClear( hdl );
		}
		hdl->filSw = (UCHAR)sel;
	}

	if( hdl->filSw == ACF_SW_MAFIL )
	{
		cmp = AcfIMaFilter( hdl, spd ); 
	}
	else if( hdl->filSw == ACF_SW_EXPFIL )
	{
		cmp = AcfExpFilter( hdl, spd );
	}
	else
	{
		AcfClear( hdl );
	}

	*den = ( *den & cmp );
	hdl->filComp = *den;
}



/****************************************************************************
 �t�B���^�N���A����
****************************************************************************/
VOID AcfClear( ACF_HNDL *hdl )
{
	hdl->expBuf = 0;
	hdl->expRem = 0;

	hdl->maFil.maSum = 0;
	hdl->maFil.maRem = 0;
	hdl->maFil.maBufPtr = 0;
	hdl->maFil.maBufCnt = 0;

	hdl->maIFil.inPosSum = 0;
	hdl->maIFil.lstPos = 0;
	hdl->maIFil.dPosSum = 0;
	hdl->maIFil.dPos = 0;
	hdl->maIFil.iRem = 0;
	hdl->maIFil.iCnt = 0;

	hdl->filComp = TRUE;
}



/****************************************************************************
 �w���������t�B���^����
****************************************************************************/
BOOL AcfExpFilter( ACF_HNDL *hdl, LONG *spd )
{
	LONG exp_tmp;
	LONG expbuf;
	BOOL rc;

	if( hdl->expTb != 0 )
	{
		expbuf = hdl->expBuf + *spd;

		exp_tmp = MlibLABS( expbuf );
		exp_tmp = exp_tmp - hdl->expBias;

		if( exp_tmp > 0 )
		{ /* expbuf > expbias �̏ꍇ */
			exp_tmp = ( exp_tmp * hdl->expTb );												/* �~���萔 */
			exp_tmp = exp_tmp + ( hdl->expBias << hdl->bPrm->vel_sft_exp ) + hdl->expRem;	/* �o�C�A�X�����Z */
			hdl->expRem = (USHORT)exp_tmp & ( (1 << hdl->bPrm->vel_sft_exp) - 1 );
			
			if( expbuf < 0 )
			{
				*spd = ( (~exp_tmp) >> hdl->bPrm->vel_sft_exp ) + 1;
			}
			else
			{
				*spd = exp_tmp >> hdl->bPrm->vel_sft_exp;
			}
		}
		else
		{ /* expbuf <= expbias �̏ꍇ */
			*spd = expbuf;
			hdl->expRem = 0;
		}
		hdl->expBuf = expbuf - *spd;

	}
	else
	{ /* �w�����������O�̎� �֘A�̕ϐ����N���A */
		hdl->expBuf = 0;
		hdl->expRem = 0;
	}

	if( (hdl->expBuf | hdl->expRem) == 0 )
	{
		rc = TRUE;
	}
	else
	{
		rc = FALSE;
	}
	return rc;
}



/****************************************************************************
 �ړ����σt�B���^ Sub Function
****************************************************************************/
static	BOOL AcfMaFilter( ACF_MAFIL *hdl, LONG *spd )
{
	LONG data;
	BOOL rc;
	
	if( hdl->maTb != 0 )
	{
		data = hdl->maBuf[hdl->maBufPtr];
		hdl->maBuf[hdl->maBufPtr] = *spd;				/* ����SPD�ۑ� */
		
		if( hdl->maBufCnt != hdl->maTb )
		{ /* ���萔�ύX���ꂽ */
			hdl->maBufCnt = 0;
			data = 0;									/* �t�B���^�o�� = 0 */
		}
		++hdl->maBufPtr;
		if( hdl->maBufPtr >= hdl->maTb )
		{
			hdl->maBufPtr = 0;
			hdl->maBufCnt = hdl->maTb;
		}

		hdl->maSum += ( *spd - data );
		data = hdl->maSum + hdl->maRem;
		
		*spd = data / hdl->maTb;
		hdl->maRem = data - ( *spd * hdl->maTb );		/* (data % hdl->maTb) */
	}
	else
	{ /* �ړ����ώ��萔���O�̎� �֘A�̕ϐ����N���A */
		hdl->maSum = 0;
		hdl->maRem = 0;
		hdl->maBufPtr = 0;
		hdl->maBufCnt = 0;
	}

	if( (hdl->maSum | hdl->maRem) == 0 )
	{
		rc = TRUE;
	}
	else
	{
		rc = FALSE;
	}
	return rc;
}



/****************************************************************************
 �ړ����σt�B���^����
****************************************************************************/
static	BOOL AcfIMaFilter( ACF_HNDL *hdl, LONG *spd )
{
	LONG sumx;
	BOOL icmp, cmp;

	if( hdl->maIFil.iTb != 0 )
	{
		hdl->maIFil.inPosSum += *spd;
		if( (++hdl->maIFil.iCnt) >= hdl->maIFil.iTb )
		{
			cmp = AcfMaFilter( &hdl->maFil, &hdl->maIFil.inPosSum );

			hdl->maIFil.dPos = hdl->maIFil.inPosSum - hdl->maIFil.lstPos;
			hdl->maIFil.lstPos = hdl->maIFil.inPosSum;
			hdl->maIFil.inPosSum = 0;
			hdl->maIFil.iCnt = 0;
		}
		else
		{
			if( (hdl->maFil.maSum | hdl->maFil.maRem) == 0 )
			{
				cmp = TRUE;
			}
			else
			{
				cmp = FALSE;
			}
		}

		hdl->maIFil.dPosSum += hdl->maIFil.dPos;
		sumx = hdl->maIFil.dPosSum + hdl->maIFil.iRem;
		*spd = sumx >> hdl->maIFil.iTbSft;
		hdl->maIFil.iRem = sumx - ( *spd << hdl->maIFil.iTbSft );

		if( (hdl->maIFil.dPosSum | hdl->maIFil.iRem) == 0 )
		{
			icmp = TRUE;
		}
		else
		{
			icmp = FALSE;
		}
		cmp &= icmp;
	}
	else
	{
		cmp = AcfMaFilter( &hdl->maFil, spd );

		hdl->maIFil.inPosSum = 0;
		hdl->maIFil.lstPos = 0;
		hdl->maIFil.dPos = 0;
		hdl->maIFil.dPosSum = 0;
		hdl->maIFil.iRem = 0;
		hdl->maIFil.iCnt = 0;
	}

	return cmp;
}



/***************************************** end of file **********************************************/
