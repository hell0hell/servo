/****************************************************************************************************/
/*																			*/
/*																			*/
/*		PRMBank.c : Parameter Bank��`																*/
/*																			*/
/*																			*/
/*																			*/
/****************************************************************************************************/
/*																			*/
/*	�@ �\ : Parameter Bank��`																		*/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	����	2010.02.23	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"
#include "MtPRM.h"
#include "PRMBank.h"
#include "PnPrmListTbl.h"/* <S004> */
//#include "HtiPrmStr.h"
//#include "DataType.h"
//#include "XlsTblM3Opt.h"

#ifdef PRM_BANK_ENABLE			/* �o���N�؂�ւ��L��SW */
/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
/* �ʒu�P�ʑI�� �ϊ��萔�쐬����																	*/
/*	LONG MBnkInit(MT_PRM *prm, USHORT bnkNum, USHORT memNum, ULONG *pMember);						*/
/****************************************************************************************************/
static LONG	mBnkCalcDummy(MT_PRM *prm, LONG bnkNo, LONG data);
static LONG	mBnkCalcAccRate1(MT_PRM *prm, LONG bnkNo, LONG data);
static LONG	mBnkCalcAccRate2(MT_PRM *prm, LONG bnkNo, LONG data);
static LONG	mBnkCalcAccAltSpdWd(MT_PRM *prm, LONG bnkNo, LONG data);
static LONG	mBnkCalcAccAltSpd(MT_PRM *prm, LONG bnkNo, LONG data);
static LONG	mBnkCalcDecRate1(MT_PRM *prm, LONG bnkNo, LONG data);
static LONG	mBnkCalcDecRate2(MT_PRM *prm, LONG bnkNo, LONG data);
static LONG	mBnkCalcDecAltSpdWd(MT_PRM *prm, LONG bnkNo, LONG data);
static LONG	mBnkCalcDecAltSpd(MT_PRM *prm, LONG bnkNo, LONG data);
static LONG	mBnkCalcExpFilBias(MT_PRM *prm, LONG bnkNo, LONG data);
static LONG	mBnkCalcExpFilTb(MT_PRM *prm, LONG bnkNo, LONG data);
static LONG	mBnkCalcMaFilTb(MT_PRM *prm, LONG bnkNo, LONG data);


/****************************************************************************************************/
/*		TABLE DEFINITION																			*/
/****************************************************************************************************/
/****************************************************************************************************/
/*																									*/
/*		�o���N�p�����[�^����t���\�����o��`														*/
/*																									*/
/****************************************************************************************************/
const BANK_DEF_TBL mBnkDefTbl[] = {
//  Pn�ԍ�  �p�����[�^������`(Low)     �v�Z�֐��|�C���^
	{0x80A, &pndef_accRate1,	FALSE,		(LONG)mBnkCalcAccRate1		},
	{0x80B, &pndef_accRate2,	FALSE,		(LONG)mBnkCalcAccRate2		},
	{0x80C, &pndef_accChgSpd,	FALSE,		(LONG)mBnkCalcAccAltSpdWd	},
	{0x80D, &pndef_decRate1,	FALSE,		(LONG)mBnkCalcDecRate1		},
	{0x80E, &pndef_decRate2,	FALSE,		(LONG)mBnkCalcDecRate2		},
	{0x80F, &pndef_decChgSpd,	FALSE,		(LONG)mBnkCalcDecAltSpdWd	},
	{0x810, &pndef_filExpBias,	FALSE,		(LONG)mBnkCalcExpFilBias	},
	{0x811, &pndef_filExpTb,	FALSE,		(LONG)mBnkCalcExpFilTb		},
	{0x812, &pndef_filMaTb,		FALSE,		(LONG)mBnkCalcMaFilTb		},
	{0x834, &pndef_accRate12L,	TRUE,		(LONG)mBnkCalcAccRate1		},
	{0x836, &pndef_accRate22L,	TRUE,		(LONG)mBnkCalcAccRate2		},
	{0x838, &pndef_accChgSpd2L,	TRUE,		(LONG)mBnkCalcAccAltSpd		},
	{0x83A, &pndef_decRate12L,	TRUE,		(LONG)mBnkCalcDecRate1		},
	{0x83C, &pndef_decRate22L,	TRUE,		(LONG)mBnkCalcDecRate2		},
	{0x83E, &pndef_decChgSpd2L,	TRUE,		(LONG)mBnkCalcDecAltSpd		},
	{0x000, NULL,				FALSE,		(LONG)mBnkCalcDummy			}
	/* end of table */
};/* <S004> *//* <S14D> */


const ULONG mBnkEnaBit[] = {
	(1<<MTBNK_PN80A) | (1<<MTBNK_PN834),
	(1<<MTBNK_PN80B) | (1<<MTBNK_PN836), 
	(1<<MTBNK_PN80C) | (1<<MTBNK_PN838), 
	(1<<MTBNK_PN80D) | (1<<MTBNK_PN83A), 
	(1<<MTBNK_PN80E) | (1<<MTBNK_PN83C), 
	(1<<MTBNK_PN80F) | (1<<MTBNK_PN83E), 
	(1<<MTBNK_PN810), 
	(1<<MTBNK_PN811), 
	(1<<MTBNK_PN812), 
	(1<<MTBNK_PN834) | (1<<MTBNK_PN80A), 
	(1<<MTBNK_PN836) | (1<<MTBNK_PN80B), 
	(1<<MTBNK_PN838) | (1<<MTBNK_PN80C), 
	(1<<MTBNK_PN83A) | (1<<MTBNK_PN80D), 
	(1<<MTBNK_PN83C) | (1<<MTBNK_PN80E), 
	(1<<MTBNK_PN83E) | (1<<MTBNK_PN80F), 
	0,
};



/****************************************************************************
****************************************************************************/
/* �o���N�p�����[�^�v�Z����:Public Functions */
/****************************************************************************
****************************************************************************/
/****************************************************************************
 ���[�V�����p�����[�^�����v�Z����
****************************************************************************/
LONG MBnkInit( MT_PRM *prm, USHORT bnkNum, USHORT memNum, USHORT *pMember )
{
	LONG wk, cnt;
	UCHAR idx;

/* Bank������ */
	prm->Bnk.bankNum = 0;
	prm->Bnk.bankSel = prm->Bnk.cmdSel = 0;	
	prm->Bnk.memNum = 0;
	prm->Bnk.enaIdxBits = 0;
	for(cnt = BANK_MEM_MAX-1; cnt >= 0; cnt--)
	{	prm->Bnk.memAttr[cnt].idx = MTBNK_PN_NONE;}

/* �o���N���`�F�b�N */
	wk = bnkNum*memNum;
	if(wk > BANK_DATA_MAX)
	{
		return MBANK_INIT_ERR;
	}
	if(wk == 0)
	{
		return MBANK_SUCCESS;
	}
	if(bnkNum > BANK_NUM_MAX)
	{
		return MBANK_INIT_ERR;
	}

/* �����o���t */
	for(cnt = memNum-1; cnt >= 0; cnt--)
	{
		for(idx = 0; mBnkDefTbl[idx].pnNo != 0; idx++)
		{
			if(mBnkDefTbl[idx].pnNo == pMember[cnt]) 
			{/* No��v */
				if( !((prm->Bnk.enaIdxBits >> idx) & 0x01) )
				{/* ���Ɋ��t���ĂȂ��ꍇ�́A�����o���t*/
					prm->Bnk.memAttr[cnt].idx = idx;
					prm->Bnk.memAttr[cnt].highWd = FALSE;
					prm->Bnk.enaIdxBits |= (mBnkEnaBit[idx]);
				}
				
				if(mBnkDefTbl[idx].longPrm != FALSE)/* <S14D> */
				{/* 4byte�f�[�^*/
					prm->Bnk.memAttr[cnt].highWd = TRUE;
					/* 4byte�f�[�^�̏ꍇ�́A���̃����o���ݒ� */
					--cnt;
					if((cnt < 0) || (mBnkDefTbl[idx].pnNo != pMember[cnt]))
					{
						return MBANK_INIT_ERR;
					}
					prm->Bnk.memAttr[cnt].idx = idx;
					prm->Bnk.memAttr[cnt].highWd = FALSE;
				}
				break;
			}
		}
	}

	prm->Bnk.memNum = (UCHAR)memNum;
	prm->Bnk.bankNum = (UCHAR)bnkNum;
	return MBANK_SUCCESS;
}

/****************************************************************************
 �o���N�f�[�^�萔�v�Z�����F�o���N�p�����[�^�v�Z�p
****************************************************************************/
LONG MBnkSetData(MT_PRM* prm, USHORT* pVal, USHORT dataNo)
{
	UCHAR			bnkNo, memNo;
	UCHAR			idx;
	DWDATA_UNION	val, minVal,maxVal;

/* �o���N�L���A�����o�L���`�F�b�N */
	if(prm->Bnk.bankNum == 0) return TRUE;

	bnkNo = dataNo / prm->Bnk.memNum;
	memNo = dataNo % prm->Bnk.memNum;
	idx = prm->Bnk.memAttr[memNo].idx;
	if(idx == MTBNK_PN_NONE)  return TRUE;
	if(bnkNo >= prm->Bnk.bankNum ) return TRUE;

/* �f�[�^���o */
	if(mBnkDefTbl[idx].longPrm != FALSE)/* <S14D> */
	{/* 4byte�f�[�^*/
		if(prm->Bnk.memAttr[memNo].highWd != FALSE)
		{
			val.usVal[0] = *(pVal+dataNo-1);
			val.usVal[1] = *(pVal+dataNo);
		}
		else
		{
			val.usVal[0] = *(pVal+dataNo);
			val.usVal[1] = *(pVal+dataNo+1);
		}
	}
	else
	{
		val.lVal = *(pVal+dataNo);
	}
	minVal.lVal = mBnkDefTbl[idx].attrDef->LowerLimit;
	maxVal.lVal = mBnkDefTbl[idx].attrDef->UpperLimit;

/* �f�[�^�N�����v */
	if(val.lVal < minVal.lVal) val.lVal = minVal.lVal;
	if(val.lVal > maxVal.lVal) val.lVal = maxVal.lVal;

/* �o���N�f�[�^�v�Z */
	(void)(*(MTPRMCALC)mBnkDefTbl[idx].MtPrmCalc)(prm, bnkNo, val.lVal);

	return TRUE;
}

/****************************************************************************
 �o���N�f�[�^�萔�v�Z�����F�f�t�H���g�p�����[�^�p
****************************************************************************/
LONG	MBnkSetDefData(MT_PRM *prm, LONG data, MTBNK_TBLIDX idx)
{
	int i;

/* �����o���t����̏ꍇ�͉������Ȃ� */
	if( (prm->Bnk.enaIdxBits >> idx) & 0x01 )
	{
		return TRUE;
	}

/* �S�o���N�f�[�^�v�Z */
	for(i = 0; i < prm->Bnk.bankNum; i++)
	{
		(void)(*(MTPRMCALC)mBnkDefTbl[idx].MtPrmCalc)(prm, i, data);
	}

	return TRUE;
}




/****************************************************************************
****************************************************************************/
/* �o���N�p�����[�^�v�Z����:Private Functions */
/****************************************************************************/
LONG mBnkCalcDummy(MT_PRM *prm, LONG bnkNo, LONG data){return 0;}

/****************************************************************************
 �����x1�ݒ�(MG�p)
****************************************************************************/
LONG mBnkCalcAccRate1(MT_PRM *prm, LONG bnkNo, LONG data)
{
	MPrmSetAccRate1(prm, &prm->Bnk.mgPrm[bnkNo], data);
	return 0;
}

/****************************************************************************
 �����x2�ݒ�(MG�p)
****************************************************************************/
LONG mBnkCalcAccRate2(MT_PRM *prm, LONG bnkNo, LONG data)
{
	MPrmSetAccRate2(prm, &prm->Bnk.mgPrm[bnkNo], data);
	return 0;
}

/****************************************************************************
 �����x�؂�ւ����x�ݒ�(MG�p)
****************************************************************************/
LONG mBnkCalcAccAltSpdWd(MT_PRM *prm, LONG bnkNo, LONG data)
{
	MPrmSetAccAltSpd(prm, &prm->Bnk.mgPrm[bnkNo], data*100);
	return 0;
}
LONG mBnkCalcAccAltSpd(MT_PRM *prm, LONG bnkNo, LONG data)
{
	MPrmSetAccAltSpd(prm, &prm->Bnk.mgPrm[bnkNo], data);
	return 0;
}

/****************************************************************************
 �����x1�ݒ�(MG�p)
****************************************************************************/
LONG mBnkCalcDecRate1(MT_PRM *prm, LONG bnkNo, LONG data)
{
	MPrmSetDecRate1(prm, &prm->Bnk.mgPrm[bnkNo], data);
	return 0;
}

/****************************************************************************
 �����x2�ݒ�(MG�p)
****************************************************************************/
LONG mBnkCalcDecRate2(MT_PRM *prm, LONG bnkNo, LONG data)
{
	MPrmSetDecRate2(prm, &prm->Bnk.mgPrm[bnkNo], data);
	return 0;
}

/****************************************************************************
 �����x��ւ����x�ݒ�(MG�p)
****************************************************************************/
LONG mBnkCalcDecAltSpdWd(MT_PRM *prm, LONG bnkNo, LONG data)
{
	MPrmSetDecAltSpd(prm, &prm->Bnk.mgPrm[bnkNo], data*100);
	return 0;
}
LONG mBnkCalcDecAltSpd(MT_PRM *prm, LONG bnkNo, LONG data)
{
	MPrmSetDecAltSpd(prm, &prm->Bnk.mgPrm[bnkNo], data);
	return 0;
}

/****************************************************************************
 �w���֐��������o�C�A�X�ݒ�(AccFilter)
****************************************************************************/
LONG mBnkCalcExpFilBias(MT_PRM *prm, LONG bnkNo, LONG data)
{
	MPrmSetExpFilBias(prm, &prm->Bnk.acfPrm[bnkNo], data*100);
	return 0;
}

/****************************************************************************
 �w���֐����������萔�ݒ�(AccFilter)
****************************************************************************/
LONG mBnkCalcExpFilTb(MT_PRM *prm, LONG bnkNo, LONG data)
{
	MPrmSetExpFilTb(prm, &prm->Bnk.acfPrm[bnkNo], (USHORT)data);
	return 0;
}

/****************************************************************************
 �ړ����ώ��萔�ݒ�(AccFilter)
****************************************************************************/
LONG mBnkCalcMaFilTb(MT_PRM *prm, LONG bnkNo, LONG data)
{
	MPrmSetMaFilTb(prm, &prm->Bnk.acfPrm[bnkNo], (USHORT)data);
	return 0;
}
#endif

/***************************************** end of file **********************************************/
