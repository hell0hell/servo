/****************************************************************************/
/*																			*/
/*																			*/
/*		Motion Generator 													*/
/*																			*/
/*																			*/
/****************************************************************************/
/*																			*/
/* MtgInit()			:Motion Generator������								*/
/* MtgAttachPRM()		:Motion�p�����[�^�f�[�^�o�^							*/
/* MtgGetDctDistance()	:���������擾										*/
/* MtgSetCspd()			:���ݎw�ߑ��x�ݒ�(���x�E�g���N���䎞�p)				*/
/*																			*/
/* MtgNone()			:���[�V�����j���[�g����(NOP)����					*/
/* MtgFollowup()		:�t�H���[�A�b�v���[�V����							*/
/* MtgVelctrl()			:���x�E�g���N���䃂�[�V����							*/
/* MtgInterpolate()		:�⊮���胂�[�V����									*/
/* MtgPosing()			:�ʒu���߃��[�V����									*/
/* MtgHold()			:������~���[�V����									*/
/*																			*/
/*																			*/
/************** Copyright Yaskawa Electric Corporation **********************/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/****************************************************************************/

/* includes */
#include "Motion.h"
//#include "Basedef.h"
//#include "ServoIf.h"
//#include "Motion.h"
//#include "MtPRM.h"
#include "MtGenerator.h"
//#include "MlibSvc.h"
//#include "UtilityLib.h" /* <V715> */

/* defines */
#define VELREM_BIT_MSK	((1<<MG_VREMBIT_MAX)-1)
#define VELREM_CARRYUP	(1<<MG_VREMBIT_MAX)


typedef struct MG_POSWK_ {
	LONG	err_dist;	/* �c�����i�w�߈ʒu����ڕW�ʒu�܂ł̋����j ****.[ref] */
	ULONG	err_dist0;	/* �c�����i�������j�i�w�߈ʒu����ڕW�ʒu�܂ł̋����j .**[ref] */
	ULONG	sd_dist0;	/* ���������i�������j�i���ݎw�ߑ��x�����~����܂ł̌��������j .**[ref] */
	LONG	sd_dist;	/* ���������i�������j�i���ݎw�ߑ��x�����~����܂ł̌��������j ****[ref] */
	LONG	sd_rem;		/* �������[���i���������Ǝc�����̍��Ŕ������镥���o���[���j ***.*[ref/cyc] */
	LONG	spdrem1;	/* �P�i�ڌ������̑��x�[�������o�� ***.*[ref/cyc] */
	LONG	spdrem2;	/* �Q�i�ڌ������̑��x�[�������o�� ***.*[ref/cyc] */
	LONG	spd;		/* ���ݎw�ߑ��x ***.*[ref/cyc] */
	LONG	tspd;		/* �ڕW���x ***.*[ref/cyc] */
	LONG	lspd;		/* �O��w�ߑ��x ***.*[ref/cyc] */
	LONG	bspd;		/* �w�ߏo�͑��x(dpos) ***.*[ref/cyc] */
} MG_POSWK;


/* forward declarations */
/* Public functions */
VOID MtgInit(MGHNDL* hdl, MC_BASE_PRM* bPrm, MG_PRM* prm);
VOID MtgAttachPRM(MGHNDL* hdl, MG_PRM* prm);
BOOL MtgGenerate(MGHNDL* hdl, MG_INOUT* inOut);
LONG MtgGetDctDistance(MGHNDL* hdl, LONG spd, USHORT ctrl);
LONG MtgSetCspd(MGHNDL* hdl, LONG spd);
LONG MtgAccelerateSpd(LONG spd, LONG tspd, MGHNDL* hdl);

/* Public Motion functions */
BOOL MtgNone(MGHNDL* hdl, MG_INOUT* inOut);
BOOL MtgFollowup(MGHNDL* hdl, MG_INOUT* inOut);
BOOL MtgVelctrl(MGHNDL* hdl, MG_INOUT* inOut);
BOOL MtgInterpolate(MGHNDL* hdl, MG_INOUT* inOut);
BOOL MtgPosing(MGHNDL* hdl, MG_INOUT* inOut);
BOOL MtgHold(MGHNDL* hdl, MG_INOUT* inOut);

/* Private sub functions */
LONG mtgCheckDecelaration(MG_POSWK*	wk, MG_PRM* prm, MC_BASE_PRM* bPrm);
LONG mtgAccelerateDecRate(LONG lspd, LONG Tspd, MG_PRM* prm);
LONG mtgAccelerateMinRate(LONG lspd, LONG Tspd, MG_PRM* prm);
VOID mtgDecreaseSpd(MG_POSWK* wk, MG_PRM* prm, MC_BASE_PRM* bPrm);
LONG mtgAccelerateSpd(LONG spd, LONG tspd, MG_PRM* prm);
BOOL mtgCalcStpDistance(MG_POSWK* wk, MG_PRM* prm, MC_BASE_PRM* bPrm);


/* motion function table */
typedef BOOL (*MTGFUNC)(MGHNDL* hdl, MG_INOUT* inOut);
//const MTGFUNC _mtgOutputMotion[] = {	/* <V715> */
#if 0
const MTGFUNC mtgOutputMotion[] = {	/* <V715> */
	MtgNone,		/* NOP���[�V���� */
	MtgInterpolate,	/* INTERPOLATE */
	MtgPosing,		/* POSING */ 
	MtgHold,		/* HOLD */
	MtgFollowup,	/* FOLLOW UP */
	MtgVelctrl,		/* VELCTRL */
	MtgVelctrl,		/* TRQCTRL */
};
#endif

/* variable <V715> */
#ifdef	__ghs__
#pragma ghs startzda
#endif
//MTGFUNC mtgOutputMotion[sizeof(_mtgOutputMotion)/sizeof(MTGFUNC)];
#ifdef	__ghs__
#pragma ghs endzda
#endif

/****************************************************************************
 ����������
****************************************************************************/
#pragma arm section code = "IXX_EXCODE"
VOID MtgInit(MGHNDL* hdl, MC_BASE_PRM* bPrm, MG_PRM* prm)
{
	hdl->cMinusDir = 0;	/* ���ݕ����o������ */
	hdl->tMinusDir = 0;	/* ���݂̎w�߈ʒu����ڕW�ʒu�̕��� */
	hdl->dpos0 = 0;		/* DPOS�̏����� for POSING�̎w�ߑ��x�i�������j�̐ώZ */
	hdl->dposRem = 0;	/* �`�������ړ���(dpos)�]��         */
	hdl->Cspd = 0;		/* ���ݎw�ߑ��x */
	hdl->basePrm = bPrm;/* ��{�p�����[�^�f�[�^�Q�Ɛ�o�^ */
	hdl->prm = prm;		/* �p�����[�^�f�[�^�Q�Ɛ�o�^ */

	/* Const�e�[�u�����f�[�^RAM�֓W�J *//* <V715> */
	//UtilMemCpy((MTGFUNC *)mtgOutputMotion, (MTGFUNC *)_mtgOutputMotion, sizeof(mtgOutputMotion));
	//MlibCopyByteMemory ( (UCHAR *)_mtgOutputMotion, (UCHAR *)mtgOutputMotion, sizeof(mtgOutputMotion) );
}
#pragma arm section code

/****************************************************************************
 �p�����[�^�f�[�^��MG�̃p�����[�^�ɓo�^����
****************************************************************************/
VOID MtgAttachPRM(MGHNDL* hdl, MG_PRM* prm)
{
	hdl->prm = prm;		/* �p�����[�^�f�[�^�Q�Ɛ�o�^ */
}



/****************************************************************************
 ���[�V�����������C������
****************************************************************************/
BOOL MtgGenerate(MGHNDL* hdl, MG_INOUT* inOut)
{
	BOOL out_cmp;

	//out_cmp = mtgOutputMotion[inOut->Mtype](hdl, inOut);
	switch( inOut->Mtype )
	{
	case 0: out_cmp = MtgNone( hdl, inOut ); break;
	case 1: out_cmp = MtgInterpolate( hdl, inOut ); break;
	case 2: out_cmp = MtgPosing( hdl, inOut ); break;
	case 3: out_cmp = MtgHold( hdl, inOut ); break;
	case 4: out_cmp = MtgFollowup( hdl, inOut ); break;
	case 5: out_cmp = MtgVelctrl( hdl, inOut ); break;
	case 6: out_cmp = MtgVelctrl( hdl, inOut ); break;
	default:
		out_cmp = FALSE; break;
	}


	return out_cmp;
}


/****************************************************************************
 ���������擾
****************************************************************************/
LONG MtgGetDctDistance(MGHNDL* hdl, LONG spd, USHORT ctrl)
{
	MG_POSWK	wk;
	LONG		dist;
	MG_PRM*		prm = hdl->prm;

	if(ctrl == MG_HOLD_CTRL2)
		prm = &hdl->basePrm->StopPrm;

	wk.spd = MlibABS(spd);
	mtgCalcStpDistance(&wk, prm, hdl->basePrm);	/* ���������v�Z */

	dist = wk.sd_dist;                                  /* buf = sd_dist (������) */
	if(((wk.sd_dist | wk.sd_dist0) != 0) && ((wk.sd_dist0 | hdl->dpos0) != 0))
		dist = dist + 1;	/* �����������������J��グ�� */

	if(hdl->cMinusDir) return (-dist); 
	return dist;
}

/****************************************************************************
 �g���N/���x���䎞 ���ݎw�ߑ��x�ݒ�
****************************************************************************/
LONG MtgSetCspd(MGHNDL* hdl, LONG spd)
{
	LONG maxSpd = hdl->basePrm->vel_max >> 1;

	hdl->cMinusDir = (spd < 0) ? TRUE: FALSE;
	spd = MlibABS(spd);
	if(spd > maxSpd) spd = maxSpd;
	hdl->Cspd = spd << hdl->basePrm->vel_shift;

	return hdl->Cspd;
}


/****************************************************************************
 NOP���[�V��������
****************************************************************************/
BOOL MtgNone(MGHNDL* hdl, MG_INOUT* inOut)
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
BOOL MtgFollowup(MGHNDL* hdl, MG_INOUT* inOut)
{
	hdl->cMinusDir = 0;
	hdl->dpos0 = 0;
	hdl->dposRem = 0;
	hdl->Cspd = 0;
	inOut->DposOut = inOut->DposIn;
	inOut->VrefOut = 0;
	inOut->TrefOut = 0;

	if(MlibABS(inOut->DposIn) >= FOLLOWUP_MAX_REF)
		return FALSE;

	return TRUE;
}

/****************************************************************************
 ���x�E�g���N���� ���[�V��������
****************************************************************************/
BOOL MtgVelctrl(MGHNDL* hdl, MG_INOUT* inOut)
{
	hdl->dpos0 = 0;
	hdl->dposRem = 0;
	inOut->DposOut = 0;
	inOut->VrefOut = inOut->Vref /* TODO: MlibMulgain(inOut->Vref, hdl->basePrm->maxToOsKx) */;
	inOut->TrefOut = inOut->Tref /* >> 6 */;
	if(hdl->Cspd != 0) return FALSE;
	return TRUE;
}


/****************************************************************************
 INTERPOLATE���[�V��������
****************************************************************************/
BOOL MtgInterpolate(MGHNDL* hdl, MG_INOUT* inOut)
{
	LONG dpos, cSpd;
	BOOL comp = FALSE;

/* �����o�������`�F�b�N */
	if(inOut->DposIn == 0)
	{
		hdl->dposRem = 0;
		comp = TRUE;
	}
/* �����o���ʍ쐬 */
	dpos = inOut->DposIn + hdl->dposRem;
	inOut->DposOut = dpos / hdl->basePrm->seg_num;
	hdl->dposRem = dpos % hdl->basePrm->seg_num;
/* POSING���x�ݒ� */
	cSpd = MtgSetCspd(hdl, inOut->DposOut);
	hdl->dpos0 = 0;

	//inOut->Tspd = MlibMulgain32(cSpd, hdl->basePrm->PosingSpdInvKx);		// �b��
	inOut->Tspd = cSpd;														// �b��
	inOut->VrefOut = inOut->Vref /* @@ MlibMulgain(inOut->Vref, hdl->basePrm->refToOsKx) */;
	inOut->TrefOut = inOut->Tref /* @@ >> 6 */;

	return comp;
}

/****************************************************************************
 Posing���[�V��������
****************************************************************************/
BOOL MtgPosing(MGHNDL* hdl, MG_INOUT* inOut)
{
	MG_POSWK	wk;
	LONG		buf;
	BOOL		rc;
	BOOL		check = TRUE;

	hdl->dposRem = 0;
	inOut->VrefOut = 0;
	inOut->TrefOut = 0;
/* POSING�����ݒ� */
	//wk.tspd = MlibMulgain32(inOut->Tspd, hdl->basePrm->PosingSpdKx);		// �b��
	wk.tspd = inOut->Tspd;													// �b��
	if(wk.tspd >= 0x40000000) {wk.tspd = 0x3FFFFFFF;}
	wk.spd =  hdl->Cspd;				/* �w�ߑ��x���R�s�|***.*[ref/cyc] */
	wk.lspd = wk.spd;					/* �w�ߑ��x��ۊ� */
	wk.bspd = wk.spd;					/* �w�ߑ��x(dpos)��ۊ� */
	wk.err_dist = inOut->DposIn;		/* �c�����v�Z�i�����t���j */
	hdl->tMinusDir = FALSE;				/* +���� */
	if(wk.err_dist < 0)
	{/* -�ړ����� */
		hdl->tMinusDir = TRUE;			/* �|���� */
		wk.err_dist = -inOut->DposIn;	/* �c���� */
	}
/* POSING�J�n�E�I���`�F�b�N */
	if(wk.spd == 0)
	{/* �w�ߑ��x=0�Ȃ�, POSING������Ԃփ��Z�b�g */
		hdl->cMinusDir = hdl->tMinusDir;
		hdl->dpos0 = 0;
		inOut->DposOut = 0;
		if(wk.err_dist == 0)	return TRUE;	/* �����o������ */
	}
/* �O��]��␳ */
	if(hdl->dpos0 != 0) 
		wk.err_dist = wk.err_dist - 1;
	wk.err_dist0 = (0 - hdl->dpos0) & VELREM_BIT_MSK;

/* ���������v�Z */
	rc = mtgCalcStpDistance(&wk, hdl->prm, hdl->basePrm);	/* ���������v�Z */
	if((hdl->tMinusDir != hdl->cMinusDir) || (rc != 0))
	{/* �ڕW�ʒu���t���� or ���������I�[�o �� ���� */
		wk.sd_rem = 0;									/* �]�v�ȕ����o���͂��Ȃ� */
		mtgDecreaseSpd(&wk, hdl->prm, hdl->basePrm);	/* �������� */
		check = FALSE; 									/* POSING�I���ł��Ȃ���� */
	}
	else
	{
		buf = mtgCheckDecelaration(&wk, hdl->prm, hdl->basePrm);	/* buf = �c���� - �������� - �w�ߑ��x */
		if(buf < 0)
		{/* �c�������������������̎��C�������� */
			wk.sd_rem = buf + wk.spd;							/* �����[�����v�Z */
			mtgDecreaseSpd(&wk, hdl->prm, hdl->basePrm);		/* �������� */
			if(wk.sd_rem < 0)	check = FALSE;					/* �������ŖڕW�ʒu����O�������߂� */
		}
		else
		{/* �c���������������ȏ�̎� */
			if(wk.spd < wk.tspd)
			{/* ���ݑ��x < �ڕW���x�ɓ��B���Ă��Ȃ� */
				wk.spd = mtgAccelerateSpd(wk.spd, wk.tspd, hdl->prm);	/* �������� */

				rc = mtgCalcStpDistance(&wk, hdl->prm, hdl->basePrm);	/* ���������v�Z */
				buf = mtgCheckDecelaration(&wk, hdl->prm, hdl->basePrm);/* buf = �c���� - �������� - �w�ߑ��x */
				if((buf < 0) || (rc != 0))
				{/* �����萔�ŉ����ł��Ȃ� */
					wk.spd = mtgAccelerateDecRate(wk.lspd, wk.tspd, hdl->prm);	/* �����萔�ŉ��� */
					rc = mtgCalcStpDistance(&wk, hdl->prm, hdl->basePrm);		/* ���������v�Z */
					buf = mtgCheckDecelaration(&wk, hdl->prm, hdl->basePrm);	/* buf = �c���� - �������� - �w�ߑ��x */
					if((buf < 0) || (rc != 0))
					{/* �����ł��Ȃ� */
						if(wk.lspd != 0)
						{/* �O�񑬓x�͂O���H */
							wk.bspd = wk.spd = wk.lspd;	/* �O��̑��x�ɂ��� */
						}
						else
						{/* �O�񑬓x=0�Ȃ琔�p���X���� */
							wk.spd = mtgAccelerateMinRate(wk.lspd, wk.tspd, hdl->prm);	/* ���ђ萔�ŉ��� */
							buf = wk.err_dist << hdl->basePrm->vel_shift;
							if(wk.spd >= buf)	wk.bspd = wk.spd = buf;	/* �c�����𑬓x�ɂ��� */
							else				wk.bspd = wk.spd;		/* ���щ����x�ŉ����������x�ɂ���B */
						}
					}
					else
					{
						wk.bspd = wk.spd;
						if( wk.spd == wk.lspd )
						{/* ����,�����ł��ĂȂ� */
							wk.sd_rem = (buf << hdl->basePrm->vel_shift);	/* buf��****.[ref] */
							if(wk.sd_rem > wk.lspd)  wk.bspd = wk.sd_rem;	/* �����[���𕥂��o���B */
						}
					}
				}
				else
				{
					wk.bspd = wk.spd;
				}
			}
			else if(wk.spd > wk.tspd)
			{/* ���ݑ��x > �ڕW���x���z���Ă���i�ڕW���x�ύX���ꂽ�j*/
				wk.sd_rem = 0;
				mtgDecreaseSpd(&wk, hdl->prm, hdl->basePrm);	/* �������� */
				if(wk.spd < wk.tspd) wk.spd = wk.tspd;			/* �ڕW���x�ɃN�����v */
				wk.bspd = wk.spd;								/* for dpos */
			}
			else
			{/* ���ݑ��x = �ڕW���x�ɓ��B���Ă��� */
				wk.bspd = wk.spd;		/* for dpos */
			}
		}
	}
	hdl->Cspd = wk.spd;		/* �w�ߑ��x */
	buf = hdl->dpos0 + ((wk.bspd << hdl->basePrm->vel_shift_s) & VELREM_BIT_MSK);	/* ��������ώZ */
	hdl->dpos0 = buf & VELREM_BIT_MSK;
	if(buf > VELREM_BIT_MSK){ buf = (wk.bspd >> hdl->basePrm->vel_shift) + 1;}
	else					{ buf = (wk.bspd >> hdl->basePrm->vel_shift);}
	if(hdl->cMinusDir)	{ inOut->DposOut = (~buf) + 1;}	/* �����g�� */
	else				{ inOut->DposOut = buf;}

/* �I������ */
	if((inOut->DposOut == inOut->DposIn) && (check == TRUE))
	{
		hdl->Cspd = 0;		/* �w�ߑ��x */
		return TRUE;
	}

	return FALSE;
}

/****************************************************************************
 Hold���[�V��������
****************************************************************************/
BOOL MtgHold(MGHNDL* hdl, MG_INOUT* inOut)
{
	MG_PRM*	prm = hdl->prm;
	BOOL	rc;

	if(inOut->Ctrl == MG_HOLD_CTRL2)
		hdl->prm = &hdl->basePrm->StopPrm;

	rc = MtgPosing(hdl, inOut);

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
LONG mtgCheckDecelaration(MG_POSWK*	wk, MG_PRM* prm, MC_BASE_PRM* bPrm)
{
	LONG result;
	LONG buf0;

	result = wk->err_dist - wk->sd_dist - (wk->spd >> bPrm->vel_shift);
	if( result > -bPrm->vel_max/*-1048576*/)
	{/* ��������������(11�ޯļ�Ď��̍ŏ��f�[�^������) */
		if(result <= 1)
		{/* err_dist�����傾�ƌ�������K�v�Ȃ��A1���傫�����́A�������������Ă��}�C�i�X�ɂȂ�Ȃ��B */
			buf0 = (LONG)(wk->err_dist0 - wk->sd_dist0) 
					- ((wk->spd << bPrm->vel_shift_s) & VELREM_BIT_MSK);  /* ������ */
			result = (result << bPrm->vel_shift) + (buf0 >> bPrm->vel_shift_s); /* buf(***.*) = err_dist - sd_dist - spd */
			if(result > 0)
				result = result >> bPrm->vel_shift;  /* ������׽�Ȃ珬���_�ȉ��s�v */
		}
	}
	else
	{
		if(wk->err_dist == 0x80000000/*-2147483648*/)
			result = 0x7fffffff/*2147483647*/;    /* �����-214748368�̈ړ��ʂ��������邽�� */
		else
			result = 0x80000000/*-2147483648*/;
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
LONG mtgAccelerateDecRate(LONG lspd, LONG Tspd, MG_PRM* prm)
{
	LONG spd;
	LONG rate;
	
 /* �������錸����I�� */
 	rate = ((lspd >= prm->altspd_d) ? prm->decrate2: prm->decrate1);
	if(lspd >= prm->altspd_a )
	{/* �Q�i�ڂ̎� */
		if(rate >= prm->accrate2) rate = 0;        /* �����x���������x�Ȃ�O��̑��x�ɂ��� */
		spd = lspd + rate;
	}
	else
	{/* �P�i�ڂ̎� */
		if(rate >= prm->accrate1)	rate = 0;		/* �����x���������x�Ȃ�O��̑��x�ɂ��� */
		spd = lspd + rate;
		if(spd >= prm->altspd_a)	spd = lspd;	/* �����������ʉ����x�ύX���x�ȏ�Ȃ�O��̑��x�ɂ��� */
	}
	if(spd >= Tspd) spd = lspd;           /* �����������ʖڕW���x�ȏ�Ȃ�O��̑��x�ɂ��� */

	return spd;
}

/****************************************************************************
  �����x�ƌ����x�̔�r�I��
  �T�v:  �����x�ƌ����x���r��,���������ŉ�������B
         �ڕW���x�ȏ�Ȃ�ڕW���x
         �����x�ύX���x�Ȃ�����x�ύX���x
****************************************************************************/
LONG mtgAccelerateMinRate(LONG lspd, LONG Tspd, MG_PRM* prm)
{
	LONG spd;
	LONG rate;
	
 /* �������錸����I�� */
	rate = ((lspd >= prm->altspd_d) ? prm->decrate2: prm->decrate1);
	if(lspd >= prm->altspd_a )
	{/* �Q�i�ڂ̎� */
		if(rate >= prm->accrate2) rate = prm->accrate2;        /* �����x���������x�Ȃ�����x�ɂ��� */
		spd = lspd + rate;
	}
	else
	{/* �P�i�ڂ̎� */
		if(rate >= prm->accrate1) rate = prm->accrate1;   /* �����x���������x�Ȃ�����x�ɂ��� */
		spd = lspd + rate;
		if(spd >= prm->altspd_a)  spd = prm->altspd_a;      /* �����������ʉ����x�ύX���x�ȏ�Ȃ�����x�ύX���x�ɂ��� */
	}
	if(spd >= Tspd) spd = Tspd;           /* �����������ʖڕW���x�ȏ�Ȃ�ڕW���x�ɂ��� */

	return spd;
}

/****************************************************************************
  ��������
  ���݂̎w�ߑ��x���猸���������w�ߑ��x���v�Z����B
  �������[���𕥂��o������N���A����B            
****************************************************************************/
VOID mtgDecreaseSpd(MG_POSWK* wk, MG_PRM* prm, MC_BASE_PRM* bPrm)
{
	LONG buf;          /* ���[�N */
	LONG lspd;
	
	lspd = wk->spd;
	if( wk->spd > prm->altspd_d )
	{/* �Q�i�ڌ������� */
		buf = ((wk->spdrem2 != 0) ? wk->spdrem2: prm->decrate2);	/*���x�[������Ȃ瑬�x�[���Ō��� */
		wk->spd -= buf;

		if(wk->spd < prm->altspd_d)   wk->spd = prm->altspd_d;	/* �����x�ύX���x�����̂Ƃ� */
		
		if(wk->sd_rem > 0)
		{ 
			if(wk->spd <= wk->sd_rem)
			{/* �������[���𕥂��o���邩�H */
				wk->bspd = wk->sd_rem;	/* �������[���𕥂��o���B */
				wk->spd = lspd;
				wk->sd_rem = 0;
			}
			else
			{
				LONG mask = (1 << bPrm->vel_shift) - 1;
				wk->bspd = wk->spd + (wk->sd_rem & mask);	/* �������[���̏������������I�ɕ����o�� */
			}
		}
		else
		{/* �������[���𕥂��o���Ȃ��̂łQ�i�ڂ̑��x�[���𕥂��o�����ƂɂȂ� */
			wk->bspd = wk->spd;
			wk->spdrem2 = 0;	/* ���x�[���͌����J�n�̂͂��߂��������o���C�ȍ~�͂Ȃ� */
		}
	}
	else
	{/* �P�i�ڌ������� */
		buf = ((wk->spdrem1 != 0) ? wk->spdrem1: prm->decrate1);	/*���x�[������Ȃ瑬�x�[���Ō��� */
		wk->spd -= buf;

		if(wk->spd < 0)	wk->spd = 0;

		if(wk->sd_rem > 0)
		{ 
			if(wk->spd <= wk->sd_rem)
			{/* �������[���𕥂��o���邩�H */
				wk->bspd = wk->sd_rem;	/* �������[���𕥂��o���B */
				wk->spd = lspd;
				wk->sd_rem = 0;
			}
			else
			{
				LONG mask = (1 << bPrm->vel_shift) - 1;
				wk->bspd = wk->spd + (wk->sd_rem & mask);	/* �������[���̏������������I�ɕ����o�� */
			}
		}
		else
		{/* �������[���𕥂��o���Ȃ��̂łQ�i�ڂ̑��x�[���𕥂��o�����ƂɂȂ� */
			wk->bspd = wk->spd;
			wk->spdrem1 = 0;	/* ���x�[���͌����J�n�̂͂��߂��������o���C�ȍ~�͂Ȃ� */
		}
	}
}


/****************************************************************************
  ��������
****************************************************************************/
LONG mtgAccelerateSpd(LONG spd, LONG tspd, MG_PRM* prm)
{
	if( spd < prm->altspd_a )
	{   /* �P�i�ډ����������Ă݂� */
		spd += prm->accrate1;
		if(spd > prm->altspd_a)
			spd = prm->altspd_a;
	}
	else
	{/* �Q�i�ډ����������Ă݂� */
		spd += prm->accrate2;
	}

	if(spd > tspd) spd = tspd;

	return spd;
}


/****************************************************************************
  ���������v�Z
****************************************************************************/
BOOL mtgCalcStpDistance(MG_POSWK* wk, MG_PRM* prm, MC_BASE_PRM* bPrm)
{
	ULONG cnt;       /* ������ */
	LONG work1[2] = {0};            /* ���Z���ʊi�[�p�-���-� */
	//LONG work2[2] = {0};    /* ���Z���ʊi�[�p�-���-� */
	LONG work3 /*,spd */;               /* ���Z���ʊi�[�pܰ��ް� */
	
#ifndef POSING_1RATEONLY
	if(prm->altspd_d != 0)
	{/* 1,2�i�ڗL���̏ꍇ */
		spd = wk->spd;
		if( wk->spd > prm->altspd_d )
		{/* �w�ߑ��x�������x�ύX���x���傫���H */
		/* �Q�i�ڌ��������v�Z ------------------------------------ */
			cnt = ( wk->spd - prm->altspd_d ) / prm->decrate2;					/* �����񐔌v�Z */
			wk->spdrem2 = wk->spd - ( prm->decrate2 * cnt ) - prm->altspd_d;	/* �����x�Q�[�� */
			if(wk->spdrem2 != 0)   cnt = cnt + 1;
			MlibMul3232( prm->altspd_d*2, cnt , work1 );			/* work1 = altspd * 2 * cnt */
			work3  = prm->decrate2*(cnt-1);
			MlibMul3232( work3, cnt , work2 );						/* work2 = rate * ((cnt*(cnt-1))) */
			MlibAdd6464( work1, work2, work2);						/* work2 = work2 + work1 */
			spd = prm->altspd_d;							/* �P�i�ڕ����̌v�Z�p */
		}
		/* �P�i�ڌ��������v�Z ------------------------------------ */
		cnt = spd / prm->decrate1;						/* �����񐔌v�Z */
		wk->spdrem1 = spd - ( prm->decrate1 * cnt );	/* �����x�P�[�� */
		if(wk->spdrem1 != 0)   cnt = cnt + 1;
		work3 = prm->decrate1*(cnt-1);
		MlibMul3232( work3, cnt , work1 );		/* work1 = rate * ((cnt*(cnt+1))) */

		MlibAdd6464( work1, work2, work2);		/* work2 = work2 + work1 �Q�i�ځ{�P�i�ڌ������������� */
		wk->sd_dist  = ((work2[1] << (31-bPrm->vel_shift))		/* ��������(������)= (2�i��+1�i�ڌ�������������)/2 */
					| ((work2[0] >> (bPrm->vel_shift+1)) & (bPrm->vel_max-1)));
		wk->sd_dist0 = (work2[0] << (bPrm->vel_shift_s-1)) & VELREM_BIT_MSK;/* ��������(������) */

		work3 = work2[1] >> (bPrm->vel_shift-1);	/* sd_dist>0x40000000���`�F�b�N */
		if(work3 != 0)	return TRUE;		/* ���������I�[�o�t���[ */
	}
	else
#endif
	{/* 2�i�ڂ̂ݗL���̏ꍇ */
		cnt = wk->spd / prm->decrate2;						/* �����񐔌v�Z */
		wk->spdrem2 = wk->spd - ( prm->decrate2 * cnt );	/* �����x�P�[�� */
		if(wk->spdrem2 != 0)   cnt = cnt + 1;
		work3 = prm->decrate2*(cnt-1);
		MlibMul3232( work3, cnt , work1 );		/* work1 = rate * ((cnt*(cnt+1))) */

		wk->sd_dist  = ((work1[1] << (31-bPrm->vel_shift))  		/* ��������(������)= (1�i�ڌ�������������)/2 */
					| ((work1[0] >> (bPrm->vel_shift+1)) & (bPrm->vel_max-1)));
		wk->sd_dist0 = (work1[0] << (bPrm->vel_shift_s-1)) & VELREM_BIT_MSK;/* ��������(������)= (1�i�ڌ�������������)/2 */

		work3 = work1[1] >> (bPrm->vel_shift-1);	/* sd_dist>0x40000000���`�F�b�N */
		if(work3 != 0)	return TRUE;		/* ���������I�[�o�t���[ */
	}
	return FALSE;
}
/****************************************************************************
  ��������
****************************************************************************/
LONG MtgAccelerateSpd(LONG spd, LONG tspd, MGHNDL* hdl)
{
LONG speed;

	speed = mtgAccelerateSpd(spd, tspd, hdl->prm);			/* �������� */

	return speed;
}
