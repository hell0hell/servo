/****************************************************************************************************/
/*																									*/
/*																									*/
/*		PRMBank.h : Parameter Bank��`																*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : Parameter Bank��`																		*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	����	2010.02.23	Y.Oka	For Mercury												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#ifndef PRM_BANK_H_
#include "PrmAccessIf.h"

/****************************************************************************************************/
/*		DEFINES																						*/
/****************************************************************************************************/
#define PRM_BANK_ENABLE		/* �o���N�؂�ւ��L��SW */

#define MBANK_SUCCESS	0		/* ����I�� */
#define MBANK_INIT_ERR	(-1)	/* �ُ�I��(�����v�Z�ُ�) */

#define BANK_DATA_MAX	64		/* �ő�o���N�f�[�^�� */

/* ���t�\�����o��`�e�[�u��(mBnkDefTbl��Index No.) */
typedef enum MTBNK_TBLIDX_ {
	MTBNK_PN80A = 0,
	MTBNK_PN80B, 
	MTBNK_PN80C, 
	MTBNK_PN80D, 
	MTBNK_PN80E, 
	MTBNK_PN80F, 
	MTBNK_PN810, 
	MTBNK_PN811, 
	MTBNK_PN812, 
	MTBNK_PN834, 
	MTBNK_PN836, 
	MTBNK_PN838, 
	MTBNK_PN83A, 
	MTBNK_PN83C, 
	MTBNK_PN83E, 
	MTBNK_PN_NONE, 
} MTBNK_TBLIDX;


/****************************************************************************************************/
/*		STRUCT DEFINITION																			*/
/****************************************************************************************************/
/* ���t�\�����o��` */
typedef struct tagBANK_DEF_TBL {
	USHORT			pnNo;			/* Pn�ԍ� */
	const PRM_ATTR_T	*attrDef;	/* �p�����[�^������`�ւ̃|�C���^ */
	BOOL			longPrm;		/* �����O�p�����[�^�H	*//* <S14D> */
	LONG			MtPrmCalc;		/* �v�Z�֐��|�C���^ */
} BANK_DEF_TBL;

typedef	LONG	(*MTPRMCALC)( MT_PRM *prm, LONG bnkNo, LONG data );

#ifndef	DWDATA_UNION
typedef	union DWDATA_UNION_{
	UCHAR	ucVal[4];
	USHORT	usVal[2];
	ULONG	ulVal;
	CHAR	cVal[4];
	SHORT	sVal[2];
	LONG	lVal;
}DWDATA_UNION;
#endif

/****************************************************************************************************/
/*		EXTERN(�b��)																				*/
/****************************************************************************************************/
extern	const BANK_DEF_TBL	mBnkDefTbl[];

/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
#ifdef PRM_BANK_ENABLE
LONG MBnkInit(MT_PRM *prm, USHORT bnkNum, USHORT memNum, USHORT *pMember);
LONG MBnkSetData(MT_PRM* prm, USHORT* pVal, USHORT dataNo);
LONG MBnkSetDefData(MT_PRM *prm, LONG data, MTBNK_TBLIDX idx);

#else
#define MBnkInit(prm, bnkNum, memNum, pMember)	(MBANK_SUCCESS)
#define MBnkSetData(prm, pVal, dataNo)			(TRUE)
#define MBnkSetDefData(prm, data, idx)			{;}
#endif

#endif /* PRM_BANK_H_ */
/***************************************** end of file **********************************************/

