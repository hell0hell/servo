/****************************************************************************
Description	: MECHATROLINK Position Manager
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
#include "Position.h"
#include "MLib.h"



/* �\�t�g���~�b�g��ԃ`�F�b�N���� */
static	VOID PsnCheckSoftLimit( PSN_HNDL *hdl, LONG *dpos );


/****************************************************************************
 ����������
****************************************************************************/
VOID PsnInit( PSN_HNDL *hdl, PSN_PRM *prm )
{
	hdl->Tpos = 0;			/* �ڕW�ʒu */
	hdl->Ipos = 0;			/* �w�߈ʒu�i�t�B���^�O�j */
	hdl->Pos = 0;			/* �w�߈ʒu�i�t�B���^��j */
	hdl->Mpos = 0;			/* FB�ʒu���̓^�C�~���O�ł̎w�߈ʒu */
	hdl->Apos = 0;			/* FB�ʒu */
	hdl->Dpos = 0;			/* ����o�͈ړ����� */
	hdl->Perr = 0;			/* �΍� */
	hdl->Fbspd = 0;			/* FB���x */
	hdl->Pgcnt = 0;			/* �ڕW�ʒu */
	hdl->SoftLimit = 0;		/* �\�t�g���~�b�g��� */
	hdl->PosOfs = 0;		/* ���ʃ��j�^ POS_OFST */
	hdl->InitPgPos64[0] = 0;/* �����G���R�[�_�ʒu�i64bit�j*/
	hdl->InitPgPos64[1] = 0;/* �����G���R�[�_�ʒu�i64bit�j*/
	hdl->RefEnable = FALSE;	/* ���t�@�����X�_���� */
	hdl->PosEnable = FALSE;	/* �ʒu�f�[�^���� */
	hdl->Prm = prm;
}


/****************************************************************************
 FB�f�[�^�쐬����
 Apos, Perr, Fbspd, Pset, Near�̍X�V
****************************************************************************/
VOID PsnUpdateFBData( PSN_HNDL *hdl, LONG pgcnt, LONG PosErr )
{
	LONG data;
	ULONG pos_err;

	/* FB�ʒu�A���x�쐬 */
	data = pgcnt - hdl->Pgcnt;
	hdl->Fbspd = data;					/* FB���x */
	hdl->Apos += data;					/* FB�ʒu */
	hdl->Pgcnt = pgcnt;					/* PGCNT�ۑ� */
#if 0
	/* MPOS�X�V */
	if( hdl->Prm->TdlyIn <= 0 )
	{
		data = ( hdl->lstDpos * hdl->Prm->TdlyIn ) / hdl->Prm->SegNum;
		hdl->Mpos = ( hdl->Pos - hdl->Dpos ) + data;
	}
	else
	{
		data = ( hdl->Dpos * hdl->Prm->TdlyIn ) / hdl->Prm->SegNum;
		hdl->Mpos = ( hdl->Pos - hdl->Dpos ) - data;
	}
#endif
	/* PERR�X�V */// <1209 I.M.�b��>
//	hdl->Perr = hdl->Mpos - hdl->Apos;
//	hdl->Mpos = hdl->Pos + (hdl->Dpos>>CFG_POSERR_DELAY_SHIFT);
	/* 2011.01.06 Y.Oka Perr�쐬������ScanB�ֈړ� */
	hdl->Perr = PosErr;
	hdl->Mpos = hdl->Apos + hdl->Perr;

#if 0/* <S0AE> */
	/* Pset�X�V */
	pos_err = MlibLABS( hdl->Tpos - hdl->Apos );
	hdl->Pset = FALSE;
	if( hdl->Den )
	{
		if( pos_err <= hdl->Prm->psetWidth )
		{ /* DEN=1 && (Mpos-Apos)<=�ʒu���ߊ����� */
			hdl->Pset = TRUE;
		}
	}
	/* Near�X�V */
	hdl->Near = FALSE;
	if( pos_err <= hdl->Prm->nearWidth )
	{ /* (Mpos-Apos)<=Near�� */
		hdl->Near = TRUE;
	}
#endif/* <S0AE> */
}

/****************************************************************************
 PSET_NEAR�X�V����
****************************************************************************//* <S0AE> */
VOID PsnSetPsetState( PSN_HNDL *hdl )
{
	ULONG pos_err;

	/* Pset�X�V */
	pos_err = MlibLABS( hdl->Tpos - hdl->Apos );
	hdl->Pset = FALSE;
	if( hdl->Den )
	{
		if( pos_err <= hdl->Prm->psetWidth )
		{ /* DEN=1 && (Mpos-Apos)<=�ʒu���ߊ����� */
			hdl->Pset = TRUE;
		}
	}
	/* Near�X�V */
	hdl->Near = FALSE;
	if( pos_err <= hdl->Prm->nearWidth )
	{ /* (Mpos-Apos)<=Near�� */
		hdl->Near = TRUE;
	}
}


/****************************************************************************
 TPOS�ݒ菈��(��Βl)
****************************************************************************/
LONG PsnSetTPOS( PSN_HNDL *hdl, LONG tpos )
{
	LONG dpos;

	dpos = tpos - hdl->Ipos;

	PsnCheckSoftLimit( hdl, &dpos );
	hdl->Tpos = dpos + hdl->Ipos;
	return dpos;
}


/****************************************************************************
 TPOS�ݒ菈��(���΋���)
****************************************************************************/
LONG PsnSetTPOSDist( PSN_HNDL *hdl, LONG dpos )
{
	PsnCheckSoftLimit( hdl, &dpos );
	hdl->Tpos = dpos + hdl->Ipos;
	return dpos;
}


/****************************************************************************
 ���x�E�g���N���䎞�̈ʒu���킹����
 TPOS=IPOS=POS=APOS+err�ɂ���
****************************************************************************/
VOID PsnResetCmdPosAll( PSN_HNDL *hdl, LONG err )
{
	LONG data;

	data = hdl->Apos + err;

	hdl->Ipos = data;
	hdl->Pos = data;
	if( hdl->PosEnable )
	{ /* ���W�n�L���� */
		hdl->Tpos = data;
		hdl->Mpos = data;
		hdl->Perr = hdl->Mpos - hdl->Apos;
	}
	else
	{ /* ���W�n������ */
		hdl->Tpos = hdl->Apos;
		hdl->Mpos = hdl->Apos;
		hdl->Perr = 0;
	}
}


/****************************************************************************
 �\�t�g���~�b�g��ԃ`�F�b�N����
 1)���t�@�����X�_�������FSOT�X�e�[�^�X�N���A�ATPOS=dpos+IPOS�ɂ��A���^�[��
 2)TPOS�̃\�t�g���~�b�g�`�F�b�N
   TPOS���\�t�g���~�b�g�ȏ�Ȃ�\�t�g���~�b�g��TPOS���N�����v���A
   dpos �Ƀ\�t�g���~�b�g�܂ł̋���(TPOS-IPOS)��Ԃ�
****************************************************************************/
static	VOID PsnCheckSoftLimit( PSN_HNDL *hdl, LONG *dpos )
{
	LONG dist, sdist;

	/* �w�߃��~�b�g�`�F�b�N */	
	if( hdl->Prm->tposLimitEnable && hdl->RefEnable )
	{
		dist = *dpos;
		if( dist >= 0 )
		{ /* ���� */
			sdist = hdl->Prm->softLimit_P - hdl->Ipos;
			if( (dist >= sdist) && (hdl->Prm->sotEnableP == TRUE) )
			{ /* �\�t�g���~�b�g�L�� && ���~�b�g�O */
				/* ���~�b�g�l�ɒB���ĂȂ��Ȃ�A���~�b�g�܂ł̈ړ��ʂ��Z�b�g */
				/* ���~�b�g�l�ɒB���Ă���A�ړ���0���Z�b�g */
				if( sdist > 0 )
				{
					*dpos = sdist;
				}
				else
				{
					*dpos = 0;
				}
			}
		}
		else
		{ /* ���� */
			sdist = hdl->Prm->softLimit_N - hdl->Ipos;
			if( (dist <= sdist) && (hdl->Prm->sotEnableN == TRUE) )
			{ /* �\�t�g���~�b�g�L�� && ���~�b�g�O */
				/* ���~�b�g�l�ɒB���ĂȂ��Ȃ�A���~�b�g�܂ł̈ړ��ʂ��Z�b�g */
				/* ���~�b�g�l�ɒB���Ă���A�ړ���0���Z�b�g */
				if( sdist < 0 )
				{
					*dpos = sdist;
				}
				else
				{
					*dpos = 0;
				}
			}
		}
	}
}


/****************************************************************************
 �\�t�g���~�b�g��ԃ`�F�b�N����(APOS)
 1)���t�@�����X�_�������FSOT�X�e�[�^�X�N���A���A���^�[��
 2)APOS�̃\�t�g���~�b�g�`�F�b�N
   APOS���\�t�g���~�b�g�ȏ�Ȃ�SOT�X�e�[�^�X���X�V���ASOT��Ԃ����^�[��
****************************************************************************/
LONG PsnCheckSoftLimitFB( PSN_HNDL *hdl, BOOL *p_ot, BOOL *n_ot )
{
	LONG	sts;

	sts = 0;
	*p_ot = FALSE;
	*n_ot = FALSE;

	/* ���t�@�����X�_�������A�X�e�[�^�X���N���A���A�������^�[�� */
	if( !hdl->RefEnable )
	{
		hdl->SoftLimit = 0;
		return 0;
	}
	/* FB���~�b�g�`�F�b�N */
	if( (hdl->Apos >= hdl->Prm->softLimit_P) && (hdl->Prm->sotEnableP == TRUE) )
	{ /* �����\�t�g���~�b�g��� */
		sts |= SOFT_LIMIT_P;
		*p_ot = TRUE;
	}
	if( (hdl->Apos <= hdl->Prm->softLimit_N) && (hdl->Prm->sotEnableN == TRUE) )
	{ /* �����\�t�g���~�b�g��� */
		sts |= SOFT_LIMIT_N;
		*n_ot = TRUE;
	}

	hdl->SoftLimit = sts;			/* SoftLimit��ԍX�V */
	return sts;
}


/****************************************************************************
 ���W�n�ݒ�
 POS��őS�Ă̈ʒu��pos�ɂ���
****************************************************************************/
VOID PsnSetCordinate( PSN_HNDL *hdl, LONG mode, LONG pos )
{
	LONG offset;

	/* POS_SET���[�h�FPOS��ňʒu�����Z�b�g */
	offset = pos - hdl->Pos;
	hdl->Pos = pos;
	hdl->Mpos = pos;
	hdl->Ipos += offset;
	hdl->Apos += offset;
	hdl->Tpos += offset;
	hdl->PosOfs += offset;		/* ���ʃ��j�^ POS_OFST */
}


/****************************************************************************
 ���W�n���Z�b�g
 ��őS�Ă̈ʒu��PGCNT��Ƀ��Z�b�g����
 �܂��Aabs=1&&ABS_PG���́AABSOffset�����Z���A���t�@�����X�_�L���ɂ���
 abs=0�̂Ƃ��́A���t�@�����X�_�����ƂȂ�
****************************************************************************/
VOID PsnResetApos( PSN_HNDL *hdl, BOOL abs )
{
	LONG offset;
	LONG apos;

	if( abs == TRUE )
	{ /* ABS�ʒu�쐬���[�h�F���_�ʒu�I�t�Z�b�g�����Z���AAPOS��ňʒu�����Z�b�g */
		apos = hdl->Pgcnt;
		if( hdl->Prm->absPG )
		{
			apos += hdl->Prm->absOfsset;
			hdl->RefEnable = TRUE;
		}
		else
		{ /* �C���N�����[�h�Ŏg���ꍇ */
			;
		}
		offset = apos - hdl->Apos;
		hdl->Apos = apos;
		hdl->Ipos += offset;
		hdl->Pos += offset;
		hdl->Mpos += offset;
		hdl->Tpos += offset;
		hdl->PosEnable = TRUE;
		hdl->PosOfs = 0;
	}
	else
	{ /* APOS���Z�b�g���[�h�FAPOS��ňʒu�����Z�b�g */
		offset = hdl->Pgcnt - hdl->Apos;
		hdl->Apos = hdl->Pgcnt;
		hdl->Ipos += offset;
		hdl->Pos += offset;
		hdl->Mpos += offset;
		hdl->Tpos += offset;
		hdl->RefEnable = FALSE;
		hdl->PosEnable = FALSE;
		hdl->PosOfs = 0;
	}
}


/****************************************************************************
 LPOS�擾
	���b�`�̈���Ȃ�΁ALpos���X�V���ATRUE��Ԃ��B
 i�F���b�`�ԍ� �ii<= LT_NUM �ł��邱��)
****************************************************************************/
BOOL PsnMakeLpos( PSN_HNDL *hdl, LONG ltcnt, LONG i )
{
	LONG lpos;
	BOOL ltcmp;

	ltcmp = FALSE;

	lpos = hdl->Apos + ( ltcnt - hdl->Pgcnt );
	if( hdl->Prm->ltRangeP > hdl->Prm->ltRangeN )
	{ /* ���b�`����l > ���b�`�����l �̏ꍇ�A
		 ���b�`�̈�́A���b�`�����l <= lpos <= ���b�`����l */
		if( (lpos >= hdl->Prm->ltRangeN) && (lpos <= hdl->Prm->ltRangeP) )
		{
			ltcmp = TRUE;
			hdl->LstLpos[i] = hdl->Lpos[i];
			hdl->Lpos[i] = lpos;
		}
	}
	else
	{ /* ���b�`����l <= ���b�`�����l �̏ꍇ
		 ���b�`�̈�́Alpos <= ���b�`�����l���A���́A ���b�`����l <= lpos */
		if( (lpos <= hdl->Prm->ltRangeP) || (lpos >= hdl->Prm->ltRangeN) )
		{
			ltcmp = TRUE;
			hdl->LstLpos[i] = hdl->Lpos[i];
			hdl->Lpos[i] = lpos;
		}
	}
	return ltcmp;
}



