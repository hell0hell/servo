/****************************************************************************/
/*																			*/
/*																			*/
/*		POSITION MANAGER 													*/
/*																			*/
/*																			*/
/****************************************************************************/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/************** Copyright Yaskawa Electric Corporation **********************/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/****************************************************************************/

/* includes */
#include "ScanN.h"
//#include "Basedef.h"
//#include "MlibSvc.h"
//#include "ServoIf.h"
//#include "Motion.h"
#include "MtPRM.h"
#include "Position.h"


/* forward declarations */
/* Public functions */
VOID PsnInit(PSNHNDL* hdl, PSN_PRM* prm);
VOID PsnUpdateFBData(PSNHNDL* hdl, LONG pgcnt);
BOOL PsnGetPsetSts(PSNHNDL* hdl);
BOOL PsnGetNearSts(PSNHNDL* hdl);
BOOL PsnGetZPointSts(PSNHNDL* hdl);
LONG PsnSetTPOS(PSNHNDL* hdl,LONG tpos);
LONG PsnSetTPOSDist(PSNHNDL* hdl,LONG dpos);
VOID PsnResetCmdPosAll(PSNHNDL* hdl, LONG err);
UCHAR PsnCheckSoftLimit(PSNHNDL* hdl, LONG* dpos);
UCHAR PsnCheckSoftLimitFB(PSNHNDL* hdl, BOOL* p_ot, BOOL* n_ot);
VOID PsnSetCordinate(PSNHNDL* hdl, UCHAR mode, LONG pos);
VOID PsnResetApos(PSNHNDL* hdl, BOOL abs);
BOOL PsnMakeLpos(PSNHNDL* hdl, LONG ltcnt);


/****************************************************************************
 ����������
****************************************************************************/
#pragma arm section code = "IXX_EXCODE"
VOID PsnInit(PSNHNDL* hdl, PSN_PRM* prm)
{
	hdl->Tpos = 0;			/* �ڕW�ʒu */
	hdl->Ipos = 0;			/* �w�߈ʒu�i�t�B���^�O�j */
	hdl->Pos = 0;			/* �w�߈ʒu�i�t�B���^��j */
	hdl->Mpos = 0;			/* �w�߈ʒu�i�t�B���^��j */
	hdl->Apos = 0;			/* FB�ʒu */
	hdl->Dpos = 0;
	hdl->Perr = 0;			/* �΍� */
	hdl->Fbspd = 0;			/* FB���x */
	hdl->Pgcnt = 0;			/* FB���x */
	hdl->SoftLimit = 0;		/* �\�t�g���~�b�g��� */
	hdl->RefEnable = FALSE;	/* ���t�@�����X�_���� */
	hdl->PosEnable = FALSE;	/* �ʒu�f�[�^���� */
	hdl->Prm = prm;
}
#pragma arm section code


/****************************************************************************
 FB�f�[�^�쐬����
 APOS,PERR, FBSPD�̍X�V
****************************************************************************/
VOID PsnUpdateFBData(PSNHNDL* hdl, LONG pgcnt)
{
	LONG data;

/* FB�ʒu�A���x�쐬 */
	data = pgcnt - hdl->Pgcnt;
	hdl->Fbspd = data;					/* FB���x */
	hdl->Apos += data;					/* FB�ʒu */
	hdl->Pgcnt = pgcnt;					/* PGCNT�ۑ� */
/* MPOS�X�V */
	if(hdl->Prm->TdlyIn <= 0)
	{
		data = (hdl->lstDpos * hdl->Prm->TdlyIn) / hdl->Prm->SegNum;
		hdl->Mpos = (hdl->Pos - hdl->Dpos) + data;
	}
	else
	{
		data = (hdl->Dpos * hdl->Prm->TdlyIn) / hdl->Prm->SegNum;
		hdl->Mpos = (hdl->Pos - hdl->Dpos) - data;
	}
/* PERR�X�V */
	hdl->Perr = hdl->Mpos - hdl->Apos;	/* PERR */

/* 32ms���� */
#if 0
	hdl->FbSpdSum += hdl->Fbspd;
	hdl->FbSpdSumCnt ++;
	if(hdl->FbSpdSumCnt >= KPI_SCANN_MS( 32 ) )
	{
		hdl->FbSpdx = (hdl->FbSpdSum >> 5);
		hdl->FbSpdSum= 0;
		hdl->FbSpdSumCnt = 0;
	}
#endif
}

/****************************************************************************
 PSET��Ԏ擾
****************************************************************************/
BOOL PsnGetPsetSts(PSNHNDL* hdl)
{
	LONG data = hdl->Tpos - hdl->Apos;
	if((hdl->Den == TRUE) && ((ULONG)MlibABS(data) <= hdl->Prm->psetWidth))
	{/* DEN=1 && (Tpos-Apos)<=�ʒu���ߊ����� */
		return TRUE;
	}
	return FALSE;
}

/****************************************************************************
 NEAR��Ԏ擾
****************************************************************************/
BOOL PsnGetNearSts(PSNHNDL* hdl)
{
	LONG data = hdl->Tpos - hdl->Apos;
	if((ULONG)MlibABS(data) <= hdl->Prm->nearWidth)
	{/* (Tpos-Apos)<=NEAR�� */
		return TRUE;
	}
	return FALSE;
}

/****************************************************************************
 ZPOINT��Ԏ擾
****************************************************************************/
BOOL PsnGetZPointSts(PSNHNDL* hdl)
{
	if((hdl->RefEnable == TRUE) 
		&& ((ULONG)MlibABS(hdl->Apos) <= hdl->Prm->zPointWidth))
	{/* Refernce�_�L�� && APOS <= ���_�ߖT�� */
		return TRUE;
	}
	return FALSE;
}


/****************************************************************************
 TPOS�ݒ菈��(��Βl)
 ****************************************************************************/
LONG PsnSetTPOS(PSNHNDL* hdl,LONG tpos)
{
	LONG dpos = tpos - hdl->Ipos;

	PsnCheckSoftLimit(hdl, &dpos);
	hdl->Tpos = dpos + hdl->Ipos;
	return dpos;
}

/****************************************************************************
 TPOS�ݒ菈��(���΋���)
 ****************************************************************************/
LONG PsnSetTPOSDist(PSNHNDL* hdl,LONG dpos)
{
	PsnCheckSoftLimit(hdl, &dpos);
	hdl->Tpos = dpos + hdl->Ipos;
	return dpos;
}

/****************************************************************************
 ���x�E�g���N���䎞�̈ʒu���킹����
 TPOS=IPOS=POS=APOS+err�ɂ���
****************************************************************************/
VOID PsnResetCmdPosAll(PSNHNDL* hdl, LONG err)
{
	LONG data = hdl->Apos + err;

	hdl->Ipos = data;
	hdl->Pos = data;
	if(hdl->PosEnable)
	{/* ���W�n�L���� */
		hdl->Tpos = data;
		hdl->Mpos = data;
		hdl->Perr = hdl->Mpos - hdl->Apos;
	}
	else
	{/* ���W�n������ */
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
UCHAR PsnCheckSoftLimit(PSNHNDL* hdl, LONG* dpos)
{
	LONG dist, sdist;

/* �w�߃��~�b�g�`�F�b�N */	
	if(hdl->Prm->tposLimitEnable && hdl->RefEnable)
	{
		dist = *dpos;
		if(dist >= 0)
		{/* ���� */
			sdist = hdl->Prm->softLimit_P - hdl->Ipos;
			if((dist >= sdist) && (hdl->Prm->sotEnableP == TRUE))
			{/* �\�t�g���~�b�g�L�� && ���~�b�g�O */
				/* ���~�b�g�l�ɒB���ĂȂ��Ȃ�A���~�b�g�܂ł̈ړ��ʂ��Z�b�g */
				/* ���~�b�g�l�ɒB���Ă���A�ړ���0���Z�b�g */
				*dpos = ((sdist > 0) ? sdist: 0);
			}
		}
		else
		{/* ���� */
			sdist = hdl->Prm->softLimit_N - hdl->Ipos;
			if((dist <= sdist) && (hdl->Prm->sotEnableN == TRUE))
			{/* �\�t�g���~�b�g�L�� && ���~�b�g�O */
				/* ���~�b�g�l�ɒB���ĂȂ��Ȃ�A���~�b�g�܂ł̈ړ��ʂ��Z�b�g */
				/* ���~�b�g�l�ɒB���Ă���A�ړ���0���Z�b�g */
				*dpos = ((sdist < 0) ? sdist: 0);
			}
		}
	}

	return 0;
}

/****************************************************************************
 �\�t�g���~�b�g��ԃ`�F�b�N����(APOS)
 1)���t�@�����X�_�������FSOT�X�e�[�^�X�N���A���A���^�[��
 2)APOS�̃\�t�g���~�b�g�`�F�b�N
   APOS���\�t�g���~�b�g�ȏ�Ȃ�SOT�X�e�[�^�X���X�V���ASOT��Ԃ����^�[��
****************************************************************************/
UCHAR PsnCheckSoftLimitFB(PSNHNDL* hdl, BOOL* p_ot, BOOL* n_ot)
{
	UCHAR	sts = 0;
	*p_ot = FALSE;
	*n_ot = FALSE;
/* ���t�@�����X�_�������A�X�e�[�^�X���N���A���A�������^�[�� */
	if(!hdl->RefEnable)
	{
		hdl->SoftLimit = 0;
		return 0;
	}
/* FB���~�b�g�`�F�b�N */	
	if((hdl->Apos >= hdl->Prm->softLimit_P) && (hdl->Prm->sotEnableP == TRUE))
	{/* �����\�t�g���~�b�g��� */
		sts |= SOFT_LIMIT_P;
		*p_ot = TRUE;
	}
	if((hdl->Apos <= hdl->Prm->softLimit_N) && (hdl->Prm->sotEnableN == TRUE))
	{/* �����\�t�g���~�b�g��� */
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
VOID PsnSetCordinate(PSNHNDL* hdl, UCHAR mode, LONG pos)
{
	LONG offset;

	/* POS_SET���[�h�FPOS��ňʒu�����Z�b�g */
	offset = pos - hdl->Pos;
	hdl->Pos = pos;
	hdl->Mpos = pos;
	hdl->Ipos += offset;
	hdl->Apos += offset;
	hdl->Tpos += offset;
}

/****************************************************************************
 ���W�n���Z�b�g
 ��őS�Ă̈ʒu��PGCNT��Ƀ��Z�b�g����
 �܂��Aabs=1&&ABS_PG���́AABSOffset�����Z���A���t�@�����X�_�L���ɂ���
 abs=0�̂Ƃ��́A���t�@�����X�_�����ƂȂ�
****************************************************************************/
VOID PsnResetApos(PSNHNDL* hdl, BOOL abs)
{
	LONG offset;
	LONG apos;

	if(abs == TRUE)
	{/* ABS�ʒu�쐬���[�h�F���_�ʒu�I�t�Z�b�g�����Z���AAPOS��ňʒu�����Z�b�g */
		apos = hdl->Pgcnt;
		if(hdl->Prm->absPG)
		{
			apos += hdl->Prm->absOfsset;
			hdl->RefEnable = TRUE;
		}
		offset = apos - hdl->Apos;
		hdl->Apos = apos;
		hdl->Ipos += offset;
		hdl->Pos += offset;
		hdl->Mpos += offset;
		hdl->Tpos += offset;
		hdl->PosEnable = TRUE;
	}
	else
	{/* APOS���Z�b�g���[�h�FAPOS��ňʒu�����Z�b�g */
		offset = hdl->Pgcnt - hdl->Apos;
		hdl->Apos = hdl->Pgcnt;
		hdl->Ipos += offset;
		hdl->Pos += offset;
		hdl->Mpos += offset;
		hdl->Tpos += offset;
		hdl->RefEnable = FALSE;
		hdl->PosEnable = FALSE;
	}
}


/****************************************************************************
 LPOS�擾
	���b�`�̈���Ȃ�΁ALpos���X�V���ATRUE��Ԃ��B
****************************************************************************/
BOOL PsnMakeLpos(PSNHNDL* hdl, LONG ltcnt)
{
	LONG lpos;
	BOOL ltcmp = FALSE;

	lpos = hdl->Apos + (ltcnt - hdl->Pgcnt);
	if(hdl->Prm->ltRangeP > hdl->Prm->ltRangeN) 
	{/* ���b�`����l > ���b�`�����l �̏ꍇ�A
		���b�`�̈�́A���b�`�����l <= lpos <= ���b�`����l */
		if((lpos >= hdl->Prm->ltRangeN) && (lpos <= hdl->Prm->ltRangeP))
		{
			ltcmp = TRUE;
			hdl->LstLpos = hdl->Lpos;
			hdl->Lpos = lpos;
		}
	}
	else
	{/* ���b�`����l <= ���b�`�����l �̏ꍇ
		���b�`�̈�́Alpos <= ���b�`�����l���A���́A ���b�`����l <= lpos */
		if((lpos <= hdl->Prm->ltRangeP) || (lpos >= hdl->Prm->ltRangeN))
		{
			ltcmp = TRUE;
			hdl->LstLpos = hdl->Lpos;
			hdl->Lpos = lpos;
		}
	}
	return ltcmp;
}


