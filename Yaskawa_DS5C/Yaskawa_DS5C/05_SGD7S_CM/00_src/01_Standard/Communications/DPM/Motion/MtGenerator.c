/****************************************************************************
Description	: MECHATROLINK Motion Generator
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
#include "MtGenerator.h"
#include "MtPRM.h"
#include "MtApi.h"
#include "MLib.h"

/****************************************************************************
 Private functions
****************************************************************************/
/* NOP���[�V�������� */
static	BOOL MtgNone( MG_HNDL *hdl, MG_INOUT *inOut );
/* Follow up ���[�V�������� */
static	BOOL MtgFollowup( MG_HNDL *hdl, MG_INOUT *inOut );
/* ���x�E�g���N���� ���[�V�������� */
static	BOOL MtgVelctrl( MG_HNDL *hdl, MG_INOUT *inOut );
/* INTERPOLATE���[�V�������� */
static	BOOL MtgInterpolate( MG_HNDL *hdl, MG_INOUT *inOut );
/* Posing���[�V�������� */
static	BOOL MtgPosing( MG_HNDL *hdl, MG_INOUT *inOut );
/* Hold���[�V�������� */
static	BOOL MtgHold( MG_HNDL *hdl, MG_INOUT *inOut );
/* �������ʗp�f�[�^���Z���� */
static	LONG mtgCheckDecelaration( MG_POSWK *wk, MG_PRM *prm, MC_BASE_PRM *bPrm );
/* �����x�ŉ��������f */
static	LONG mtgAccelerateDecRate( LONG lspd, LONG Tspd, MG_PRM *prm );
/* �����x�ƌ����x�̔�r�I�� */
static	LONG mtgAccelerateMinRate( LONG lspd, LONG Tspd, MG_PRM *prm );
/* �������� */
static	VOID mtgDecreaseSpd( MG_POSWK *wk, MG_PRM *prm, MC_BASE_PRM *bPrm );
/* �������� */
static	LONG mtgAccelerateSpd( LONG spd, LONG tspd, MG_PRM *prm );
/* ���������v�Z */
static	BOOL mtgCalcStpDistance( MG_POSWK *wk, MG_PRM *prm, MC_BASE_PRM *bPrm );


/* motion function table */
typedef BOOL (*MTGFUNC)(MG_HNDL *hdl, MG_INOUT *inOut);
const MTGFUNC mtgOutputMotion[] = {
	MtgNone,					/* NOP���[�V���� */
	MtgInterpolate,				/* INTERPOLATE */
	MtgPosing,					/* POSING */ 
	MtgHold,					/* HOLD */
	MtgFollowup,				/* FOLLOW UP */
	MtgVelctrl,					/* VELCTRL */
	MtgVelctrl,					/* TRQCTRL */
};



/****************************************************************************
 ����������
****************************************************************************/
VOID MtgInit( MG_HNDL *hdl, MC_BASE_PRM *bPrm, MG_PRM *prm )
{
	hdl->cMinusDir = 0;		/* ���ݕ����o������ */
	hdl->tMinusDir = 0;		/* ���݂̎w�߈ʒu����ڕW�ʒu�̕��� */
	hdl->dpos0 = 0;			/* DPOS�̏����� for POSING�̎w�ߑ��x�i�������j�̐ώZ */
	hdl->dposRem = 0;		/* �`�������ړ���(dpos)�]��         */
	hdl->Cspd = 0;			/* ���ݎw�ߑ��x */
	hdl->basePrm = bPrm;	/* ��{�p�����[�^�f�[�^�Q�Ɛ�o�^ */
	hdl->prm = prm;			/* �p�����[�^�f�[�^�Q�Ɛ�o�^ */
}


/****************************************************************************
 ���[�V�����������C������
****************************************************************************/
BOOL MtgGenerate(MG_HNDL* hdl, MG_INOUT* inOut)
{
	BOOL out_cmp;

	out_cmp = mtgOutputMotion[inOut->Mtype](hdl, inOut);

	return out_cmp;
}


/****************************************************************************
 ���������擾
****************************************************************************/
LONG MtgGetDctDistance( MG_HNDL *hdl, LONG spd, LONG ctrl )
{
	MG_POSWK	wk;
	LONG		dist;
	MG_PRM		*prm;

	prm = hdl->prm;

	if( ctrl == MG_HOLD_CTRL2 )
	{
		prm = &hdl->basePrm->StopPrm;
	}

	wk.spd = MlibLABS( spd );
	mtgCalcStpDistance( &wk, prm, hdl->basePrm );		/* ���������v�Z */

	dist = wk.sd_dist;                                  /* buf = sd_dist (������) */
	if( ((wk.sd_dist | wk.sd_dist0) != 0) && ((wk.sd_dist0 | hdl->dpos0) != 0) )
	{
		dist = dist + 1;								/* �����������������J��グ�� */
	}

	if( hdl->cMinusDir )
	{
		return (-dist); 
	}
	return dist;
}


/****************************************************************************
 �g���N/���x���䎞 ���ݎw�ߑ��x�ݒ�
****************************************************************************/
LONG MtgSetCspd( MG_HNDL *hdl, LONG spd )
{
	LONG maxSpd;

	maxSpd = hdl->basePrm->vel_max >> 1;

	if( spd < 0 )
	{
		hdl->cMinusDir = TRUE;
	}
	else
	{
		hdl->cMinusDir = FALSE;
	}

	spd = MlibLABS( spd );

	if( spd > maxSpd )
	{
		spd = maxSpd;
	}
	hdl->Cspd = spd << hdl->basePrm->vel_shift;

	return hdl->Cspd;
}


/****************************************************************************
 NOP���[�V��������
****************************************************************************/
static	BOOL MtgNone( MG_HNDL *hdl, MG_INOUT *inOut )
{
	hdl->cMinusDir = 0;
	hdl->dpos0 = 0;
	hdl->dposRem = 0;
	hdl->Cspd = 0;
	inOut->DposOut = 0;
	inOut->VrefOut = 0;
	inOut->TrefOut = 0;

	return TRUE;
}


/****************************************************************************
 Follow up ���[�V��������
****************************************************************************/
static	BOOL MtgFollowup( MG_HNDL *hdl, MG_INOUT *inOut )
{
	hdl->cMinusDir = 0;
	hdl->dpos0 = 0;
	hdl->dposRem = 0;
	hdl->Cspd = 0;
	inOut->DposOut = inOut->DposIn;
	inOut->VrefOut = 0;
	inOut->TrefOut = 0;

	if( MlibLABS(inOut->DposIn) >= FOLLOWUP_MAX_REF )
	{
		return FALSE;
	}

	return TRUE;
}


/****************************************************************************
 ���x�E�g���N���� ���[�V��������
****************************************************************************/
static	BOOL MtgVelctrl( MG_HNDL *hdl, MG_INOUT *inOut )
{
	/*----------------------------------------------*/
	/* 				�ʒu�w�߂̃N���A				*/
	/*----------------------------------------------*/
	hdl->dpos0     = 0;
	hdl->dposRem   = 0;
	inOut->DposOut = 0;

	/*----------------------------------------------*/
	/* 				���x�w�ߍ쐬					*/
	/*----------------------------------------------*/
	inOut->VrefOut = MlibMulgain( inOut->Vref, hdl->basePrm->maxToOsKx );

	/*----------------------------------------------*/
	/* 				�g���N�w�ߍ쐬					*/
	/*----------------------------------------------*/
	inOut->TrefOut = inOut->Tref >> 6;

	if( hdl->Cspd != 0 )
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

/****************************************************************************
 INTERPOLATE���[�V��������
****************************************************************************/
static	BOOL MtgInterpolate( MG_HNDL *hdl, MG_INOUT *inOut )
{
	LONG dpos, cSpd;
	BOOL comp;

	comp = FALSE;

	/* �����o�������`�F�b�N */
	if( inOut->DposIn == 0 )
	{
		hdl->dposRem = 0;
		comp = TRUE;
	}

	/* �����o���ʍ쐬 */
	dpos = inOut->DposIn + hdl->dposRem;
	inOut->DposOut = dpos / hdl->basePrm->seg_num;
	hdl->dposRem = dpos % hdl->basePrm->seg_num;

	/* POSING���x�ݒ� */
	cSpd = MtgSetCspd( hdl, inOut->DposOut );
	hdl->dpos0 = 0;

	inOut->Tspd = MlibMulgain32( cSpd, hdl->basePrm->PosingSpdInvKx );
	inOut->VrefOut = MlibMulgain( inOut->Vref, hdl->basePrm->refToOsKx );
	inOut->TrefOut = inOut->Tref >> 6;

	return comp;
}


/****************************************************************************
 Posing���[�V��������
****************************************************************************/
static	BOOL MtgPosing( MG_HNDL *hdl, MG_INOUT *inOut )
{
	MG_POSWK	wk;
	LONG		buf;
	LONG		tmp;
	BOOL		rc, check;

	check = TRUE;

	hdl->dposRem = 0;
	inOut->VrefOut = 0;
	inOut->TrefOut = 0;

	tmp = MlibMulgain32( inOut->Tspd, hdl->basePrm->PosingSpdKx );
	if( tmp >= 0x40000000L )
	{
		wk.tspd = 0x3FFFFFFF;
	}
	else
	{
		wk.tspd = (LONG)tmp;
	}

	wk.spd =  hdl->Cspd;				/* �w�ߑ��x���R�s�|***.*[ref/cyc] */
	wk.lspd = wk.spd;					/* �w�ߑ��x��ۊ� */
	wk.bspd = wk.spd;					/* �w�ߑ��x(dpos)��ۊ� */
	wk.err_dist = inOut->DposIn;		/* �c�����v�Z�i�����t���j */
	hdl->tMinusDir = FALSE;				/* +���� */
	if( wk.err_dist < 0 )
	{ /* -�ړ����� */
		hdl->tMinusDir = TRUE;			/* �|���� */
		wk.err_dist = -inOut->DposIn;	/* �c���� */
	}

	/* POSING�J�n�E�I���`�F�b�N */
	if( wk.spd == 0 )
	{ /* �w�ߑ��x=0�Ȃ�, POSING������Ԃփ��Z�b�g */
		hdl->cMinusDir = hdl->tMinusDir;
		hdl->dpos0 = 0;
		inOut->DposOut = 0;
		if( wk.err_dist == 0 )
		{
			return TRUE;				/* �����o������ */
		}
	}

	/* �O��]��␳ */
	if( hdl->dpos0 != 0 )
	{
		wk.err_dist = wk.err_dist - 1;
	}
	wk.err_dist0 = ( 0 - hdl->dpos0 ) & VELREM_BIT_MSK;

	/* ���������v�Z */
	rc = mtgCalcStpDistance( &wk, hdl->prm, hdl->basePrm );
	if( (hdl->tMinusDir != hdl->cMinusDir) || (rc != 0) )
	{ /* �ڕW�ʒu���t���� or ���������I�[�o �� ���� */
		wk.sd_rem = 0;											/* �]�v�ȕ����o���͂��Ȃ� */
		mtgDecreaseSpd( &wk, hdl->prm, hdl->basePrm );			/* �������� */
		check = FALSE; 											/* POSING�I���ł��Ȃ���� */
	}
	else
	{
		/* buf = �c���� - �������� - �w�ߑ��x */
		buf = mtgCheckDecelaration( &wk, hdl->prm, hdl->basePrm );
		if( buf < 0 )
		{ /* �c�������������������̎��C�������� */
			wk.sd_rem = buf + wk.spd;							/* �����[�����v�Z */
			mtgDecreaseSpd( &wk, hdl->prm, hdl->basePrm );		/* �������� */
			if( wk.sd_rem < 0 )
			{
				/* �������ŖڕW�ʒu����O�������߂� */
				check = FALSE;
			}
		}
		else
		{ /* �c���������������ȏ�̎� */
			if( wk.spd < wk.tspd )
			{ /* ���ݑ��x < �ڕW���x�ɓ��B���Ă��Ȃ� */
				/* �������� */
				wk.spd = mtgAccelerateSpd( wk.spd, wk.tspd, hdl->prm );

				/* ���������v�Z */
				rc = mtgCalcStpDistance( &wk, hdl->prm, hdl->basePrm );
				/* buf = �c���� - �������� - �w�ߑ��x */
				buf = mtgCheckDecelaration( &wk, hdl->prm, hdl->basePrm );
				if( (buf < 0) || (rc != 0) )
				{ /* �����萔�ŉ����ł��Ȃ� */
					/* �����萔�ŉ��� */
					wk.spd = mtgAccelerateDecRate( wk.lspd, wk.tspd, hdl->prm );
					/* ���������v�Z */
					rc = mtgCalcStpDistance( &wk, hdl->prm, hdl->basePrm );
					/* buf = �c���� - �������� - �w�ߑ��x */
					buf = mtgCheckDecelaration( &wk, hdl->prm, hdl->basePrm );
					if( (buf < 0) || (rc != 0) )
					{/* �����ł��Ȃ� */
						if( wk.lspd != 0 )
						{ /* �O�񑬓x�͂O���H */
							/* �O��̑��x�ɂ��� */
							wk.bspd = wk.spd = wk.lspd;
						}
						else
						{ /* �O�񑬓x=0�Ȃ琔�p���X���� */
							/* �~�j�}���萔�ŉ��� */
							wk.spd = mtgAccelerateMinRate( wk.lspd, wk.tspd, hdl->prm );
							buf = wk.err_dist << hdl->basePrm->vel_shift;
							if( wk.spd >= buf )
							{
								/* �c�����𑬓x�ɂ��� */
								wk.bspd = wk.spd = buf;
							}
							else
							{
								/* �~�j�}�������x�ŉ����������x�ɂ���B */
								wk.bspd = wk.spd;
							}
						}
					}
					else
					{
						wk.bspd = wk.spd;
						if( wk.spd == wk.lspd )
						{ /* ����,�����ł��ĂȂ� */
							/* buf��****.[ref] */
							wk.sd_rem = ( buf << hdl->basePrm->vel_shift );
							/* �����[���𕥂��o���B */
							if( wk.sd_rem > wk.lspd )
							{
								wk.bspd = wk.sd_rem;
							}
						}
					}
				}
				else
				{
					wk.bspd = wk.spd;
				}
			}
			else if( wk.spd > wk.tspd )
			{ /* ���ݑ��x > �ڕW���x���z���Ă���i�ڕW���x�ύX���ꂽ�j*/
				wk.sd_rem = 0;
				/* �������� */
				mtgDecreaseSpd( &wk, hdl->prm, hdl->basePrm );
				if( wk.spd < wk.tspd )
				{
					/* �ڕW���x�ɃN�����v */
					wk.spd = wk.tspd;
				}
				wk.bspd = wk.spd;								/* for dpos */
			}
			else
			{/* ���ݑ��x = �ڕW���x�ɓ��B���Ă��� */
				wk.bspd = wk.spd;								/* for dpos */
			}
		}
	}
	hdl->Cspd = wk.spd;											/* �w�ߑ��x */
	/* ��������ώZ */
	buf = hdl->dpos0 + ( (wk.bspd << hdl->basePrm->vel_shift_s) & VELREM_BIT_MSK );
	hdl->dpos0 = buf & VELREM_BIT_MSK;
	if( buf > VELREM_BIT_MSK )
	{
		buf = ( wk.bspd >> hdl->basePrm->vel_shift ) + 1;
	}
	else
	{
		buf = ( wk.bspd >> hdl->basePrm->vel_shift );
	}

	if( hdl->cMinusDir )
	{
		inOut->DposOut = (~buf) + 1;							/* �����g�� */
	}
	else
	{
		inOut->DposOut = buf;
	}

	/* �I������ */
	if( (inOut->DposOut == inOut->DposIn) && (check == TRUE) )
	{
		hdl->Cspd = 0;											/* �w�ߑ��x */
		return TRUE;
	}

	return FALSE;
}


/****************************************************************************
 Hold���[�V��������
****************************************************************************/
static	BOOL MtgHold( MG_HNDL *hdl, MG_INOUT *inOut )
{
	MG_PRM *prm;
	BOOL	rc;

	prm = hdl->prm;

	if( inOut->Ctrl == MG_HOLD_CTRL2 )
	{
		hdl->prm = &hdl->basePrm->StopPrm;
	}

	rc = MtgPosing( hdl, inOut );

	hdl->prm = prm;

	return rc;
}


/****************************************************************************
 �������ʗp�f�[�^���Z����
  �T�v:  �c�����|���������|�w�ߑ��x�����߂�B
         ϲŽ�ƂȂ鎞�������܂ł��v�Z����B
         ��׽�ƂȂ�Ƃ��͂��̂܂܁i���������ł͏����_�ȉ��s�v�j
         ��������������̎������ђl(-2147483648)�Ƃ���B
         -2147483648�̈ړ����������������,�c������-2147483648�̎���
         �����I��214748367�ɂ���B
****************************************************************************/
static	LONG mtgCheckDecelaration( MG_POSWK *wk, MG_PRM *prm, MC_BASE_PRM *bPrm )
{
	LONG result;
	LONG buf0;

	result = wk->err_dist - wk->sd_dist - ( wk->spd >> bPrm->vel_shift );
	if( result > -bPrm->vel_max/*-1048576*/ )
	{ /* ��������������(11�r�b�g�V�t�g���̍ŏ��f�[�^�Ń`�F�b�N) */
		if( result <= 1 )
		{ /* err_dist�����傾�ƌ�������K�v�Ȃ��A1���傫�����́A�������������Ă��}�C�i�X�ɂȂ�Ȃ��B */
			/* ������ */
			buf0 = (LONG)( wk->err_dist0 - wk->sd_dist0 ) 
					- ( (wk->spd << bPrm->vel_shift_s) & VELREM_BIT_MSK );
			/* buf(***.*) = err_dist - sd_dist - spd */
			result = ( result << bPrm->vel_shift) + (buf0 >> bPrm->vel_shift_s );
			if( result > 0 )
			{
				/* ���ʃv���X�Ȃ珬���_�ȉ��s�v */
				result = result >> bPrm->vel_shift;
			}
		}
	}
	else
	{
		if( wk->err_dist == 0x80000000/*-2147483648*/ )
		{
		    /* �����-214748368�̈ړ��ʂ��������邽�� */
			result = 0x7fffffff /*2147483647*/;
		}
		else
		{
			result = 0x80000000 /*-2147483648*/;
		}
	}
	return result;
}



/****************************************************************************
  �����x�ŉ��������f
  �T�v:  ���݂̑��x���猸���萔�ŉ����ł��邩���f���A���x�w�߂��o�͂���B
         �������ł��������
         �����萔�������萔�Ȃ�����ł������B
         �ڕW���x�����Ȃ�n�j�i�ڕW���x�͉����x�Ń`�F�b�N�ς݁j
         �����x�ύX���x�����Ȃ�n�j�i�����x�ύX���x�͉����x�Ń`�F�b�N�ς݁j
****************************************************************************/
static	LONG mtgAccelerateDecRate( LONG lspd, LONG Tspd, MG_PRM *prm )
{
	LONG spd;
	LONG rate;
	
	 /* �������錸����I�� */
	if( lspd >= prm->altspd_d )
	{
 		rate = prm->decrate2;
	}
	else
	{
		rate = prm->decrate1;
	}
	if( lspd >= prm->altspd_a )
	{ /* �Q�i�ڂ̎� */
		if( rate >= prm->accrate2 )
		{
			rate = 0;					/* �����x���������x�Ȃ�O��̑��x�ɂ��� */
		}
		spd = lspd + rate;
	}
	else
	{ /* �P�i�ڂ̎� */
		if( rate >= prm->accrate1 )
		{
			rate = 0;					/* �����x���������x�Ȃ�O��̑��x�ɂ��� */
		}
		spd = lspd + rate;
		if( spd >= prm->altspd_a )
		{
			spd = lspd;					/* �����������ʉ����x�ύX���x�ȏ�Ȃ�O��̑��x�ɂ��� */
		}
	}
	if( spd >= Tspd )
	{
		spd = lspd;						/* �����������ʖڕW���x�ȏ�Ȃ�O��̑��x�ɂ��� */
	}

	return spd;
}



/****************************************************************************
  �����x�ƌ����x�̔�r�I��
  �T�v:  �����x�ƌ����x���r��,���������ŉ�������B
         �ڕW���x�ȏ�Ȃ�ڕW���x
         �����x�ύX���x�Ȃ�����x�ύX���x
****************************************************************************/
static	LONG mtgAccelerateMinRate( LONG lspd, LONG Tspd, MG_PRM *prm )
{
	LONG spd;
	LONG rate;

	/* �������錸����I�� */
	if( lspd >= prm->altspd_d )
	{
		rate = prm->decrate2;
	}
	else
	{
		rate = prm->decrate1;
	}
	if( lspd >= prm->altspd_a )
	{ /* �Q�i�ڂ̎� */
		if( rate >= prm->accrate2 )
		{
			rate = prm->accrate2;		/* �����x���������x�Ȃ�����x�ɂ��� */
		}
		spd = lspd + rate;
	}
	else
	{ /* �P�i�ڂ̎� */
		if( rate >= prm->accrate1 )
		{
			rate = prm->accrate1;		/* �����x���������x�Ȃ�����x�ɂ��� */
		}
		spd = lspd + rate;
		if( spd >= prm->altspd_a )
		{
			spd = prm->altspd_a;		/* �����������ʉ����x�ύX���x�ȏ�Ȃ�����x�ύX���x�ɂ��� */
		}
	}
	if( spd >= Tspd )
	{
		spd = Tspd;						/* �����������ʖڕW���x�ȏ�Ȃ�ڕW���x�ɂ��� */
	}

	return spd;
}


/****************************************************************************
  ��������
  ���݂̎w�ߑ��x���猸���������w�ߑ��x���v�Z����B
  �������[���𕥂��o������N���A����B            
****************************************************************************/
static	VOID mtgDecreaseSpd( MG_POSWK *wk, MG_PRM *prm, MC_BASE_PRM *bPrm )
{
	LONG buf;          /* ���[�N */
	LONG lspd;
	LONG mask;
	
	lspd = wk->spd;
	if( wk->spd > prm->altspd_d )
	{ /* �Q�i�ڌ������� */
		/*���x�[������Ȃ瑬�x�[���Ō��� */
		if( wk->spdrem2 != 0 )
		{
			buf = wk->spdrem2;
		}
		else
		{
			buf = prm->decrate2;
		}
		wk->spd -= buf;

		if( wk->spd < prm->altspd_d )
		{
			wk->spd = prm->altspd_d;	/* �����x�ύX���x�����̂Ƃ� */
		}

		if( wk->sd_rem > 0 )
		{ 
			if( wk->spd <= wk->sd_rem )
			{ /* �������[���𕥂��o���邩�H */
				wk->bspd = wk->sd_rem;	/* �������[���𕥂��o���B */
				wk->spd = lspd;
				wk->sd_rem = 0;
			}
			else
			{
				mask = ( 1 << bPrm->vel_shift ) - 1;
				/* �������[���̏������������I�ɕ����o�� */
				wk->bspd = wk->spd + (wk->sd_rem & mask);
			}
		}
		else
		{ /* �������[���𕥂��o���Ȃ��̂łQ�i�ڂ̑��x�[���𕥂��o�����ƂɂȂ� */
			wk->bspd = wk->spd;
			wk->spdrem2 = 0;			/* ���x�[���͌����J�n�̂͂��߂��������o���C�ȍ~�͂Ȃ� */
		}
	}
	else
	{ /* �P�i�ڌ������� */
		/*���x�[������Ȃ瑬�x�[���Ō��� */
		if( wk->spdrem1 != 0 )
		{
			buf = wk->spdrem1;
		}
		else
		{
			buf = prm->decrate1;
		}
		wk->spd -= buf;

		if( wk->spd < 0 )
		{
			wk->spd = 0;
		}

		if( wk->sd_rem > 0 )
		{ 
			if( wk->spd <= wk->sd_rem )
			{ /* �������[���𕥂��o���邩�H */
				wk->bspd = wk->sd_rem;	/* �������[���𕥂��o���B */
				wk->spd = lspd;
				wk->sd_rem = 0;
			}
			else
			{
				mask = ( 1 << bPrm->vel_shift ) - 1;
				/* �������[���̏������������I�ɕ����o�� */
				wk->bspd = wk->spd + ( wk->sd_rem & mask );
			}
		}
		else
		{ /* �������[���𕥂��o���Ȃ��̂łP�i�ڂ̑��x�[���𕥂��o�����ƂɂȂ� */
			wk->bspd = wk->spd;
			wk->spdrem1 = 0;			/* ���x�[���͌����J�n�̂͂��߂��������o���C�ȍ~�͂Ȃ� */
		}
	}
}



/****************************************************************************
  ��������
****************************************************************************/
static	LONG mtgAccelerateSpd( LONG spd, LONG tspd, MG_PRM *prm )
{
	if( spd < prm->altspd_a )
	{ /* �P�i�ډ����������Ă݂� */
		spd += prm->accrate1;
		if( spd > prm->altspd_a )
		{
			spd = prm->altspd_a;
		}
	}
	else
	{ /* �Q�i�ډ����������Ă݂� */
		spd += prm->accrate2;
	}

	if( spd > tspd )
	{
		spd = tspd;
	}

	return spd;
}



/****************************************************************************
  ���������v�Z
****************************************************************************/
static	BOOL mtgCalcStpDistance( MG_POSWK *wk, MG_PRM *prm, MC_BASE_PRM *bPrm )
{
	ULONG cnt;					/* ������ */
	LONG work3,spd;				/* ���Z���ʊi�[�pܰ��ް� */
	LONG work1[2] = {0};		/* ���Z���ʊi�[�p�-���-� */
	LONG work2[2] = {0};		/* ���Z���ʊi�[�p�-���-� */
	
	if( prm->altspd_d != 0 )
	{ /* 1, 2�i�ڗL���̏ꍇ */
		spd = wk->spd;
		if( wk->spd > prm->altspd_d )
		{ /* �w�ߑ��x�������x�ύX���x���傫���H */
			/* �Q�i�ڌ��������v�Z ------------------------------------ */
			/* �����񐔌v�Z */
			cnt = ( wk->spd - prm->altspd_d ) / prm->decrate2;
			/* �����x�Q�[�� */
			wk->spdrem2 = wk->spd - ( prm->decrate2 * cnt ) - prm->altspd_d;
			if(wk->spdrem2 != 0)
			{
				cnt = cnt + 1;
			}
			/* work1 = altspd * 2 * cnt */
			MlibMul3232( prm->altspd_d*2, cnt , work1 );
			work3 = prm->decrate2 * (cnt - 1);
			/* work2 = rate * ((cnt*(cnt-1))) */
			MlibMul3232( work3, cnt , work2 );
			/* work2 = work2 + work1 */
			MlibAdd6464( work1, work2, work2);
			/* �P�i�ڕ����̌v�Z�p */
			spd = prm->altspd_d;
		}

		/* �P�i�ڌ��������v�Z ------------------------------------ */
		/* �����񐔌v�Z */
		cnt = spd / prm->decrate1;
		/* �����x�P�[�� */
		wk->spdrem1 = spd - ( prm->decrate1 * cnt );
		if( wk->spdrem1 != 0 )
		{
			cnt = cnt + 1;
		}
		work3 = prm->decrate1 * ( cnt-1 );

		/* work1 = rate * ((cnt*(cnt-1))) */
		MlibMul3232( work3, cnt , work1 );

		/* work2 = work2 + work1 �Q�i�ځ{�P�i�ڌ������������� */
		MlibAdd6464( work1, work2, work2 );
		/* ��������(������)= (2�i��+1�i�ڌ�������������)/2 */
		wk->sd_dist  = ( (work2[1] << (31-bPrm->vel_shift)) | 
						((work2[0] >> (bPrm->vel_shift+1)) & (bPrm->vel_max-1)) );
		/* ��������(������) */
		wk->sd_dist0 = ( work2[0] << (bPrm->vel_shift_s-1) ) & VELREM_BIT_MSK;

		/* sd_dist>0x40000000���`�F�b�N */
		work3 = work2[1] >> ( bPrm->vel_shift - 1 );
		if( work3 != 0 )
		{
			return TRUE;		/* ���������I�[�o�t���[ */
		}
	}
	else
	{ /* 2�i�ڂ̂ݗL���̏ꍇ */
		cnt = wk->spd / prm->decrate2;						/* �����񐔌v�Z */
		wk->spdrem2 = wk->spd - ( prm->decrate2 * cnt );	/* �����x�P�[�� */
		if(wk->spdrem2 != 0)
		{
			cnt = cnt + 1;
		}
		work3 = prm->decrate2*(cnt-1);

		/* work1 = rate * ((cnt*(cnt+1))) */
		MlibMul3232( work3, cnt , work1 );

		/* ��������(������)= (1�i�ڌ�������������)/2 */
		wk->sd_dist = ( (work1[1] << (31-bPrm->vel_shift)) | 
						((work1[0] >> (bPrm->vel_shift+1)) & (bPrm->vel_max-1)) );
		/* ��������(������)= (1�i�ڌ�������������)/2 */
		wk->sd_dist0 = (work1[0] << (bPrm->vel_shift_s-1)) & VELREM_BIT_MSK;

		/* sd_dist>0x40000000���`�F�b�N */
		work3 = work1[1] >> ( bPrm->vel_shift - 1 );
		if( work3 != 0 )
		{
			return TRUE;		/* ���������I�[�o�t���[ */
		}
	}
	return FALSE;
}
