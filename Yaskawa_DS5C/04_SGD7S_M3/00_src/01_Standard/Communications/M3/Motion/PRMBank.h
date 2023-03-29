/****************************************************************************************************/
/*																									*/
/*																									*/
/*		PRMBank.h : Parameter Bank定義																*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : Parameter Bank定義																		*/
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
/*	Note	:	初版	2010.02.23	Y.Oka	For Mercury												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#ifndef PRM_BANK_H_
#include "PrmAccessIf.h"

/****************************************************************************************************/
/*		DEFINES																						*/
/****************************************************************************************************/
#define PRM_BANK_ENABLE		/* バンク切り替え有効SW */

#define MBANK_SUCCESS	0		/* 正常終了 */
#define MBANK_INIT_ERR	(-1)	/* 異常終了(初期計算異常) */

#define BANK_DATA_MAX	64		/* 最大バンクデータ数 */

/* 割付可能メンバ定義テーブル(mBnkDefTblのIndex No.) */
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
/* 割付可能メンバ定義 */
typedef struct tagBANK_DEF_TBL {
	USHORT			pnNo;			/* Pn番号 */
	const PRM_ATTR_T	*attrDef;	/* パラメータ属性定義へのポインタ */
	BOOL			longPrm;		/* ロングパラメータ？	*//* <S14D> */
	LONG			MtPrmCalc;		/* 計算関数ポインタ */
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
/*		EXTERN(暫定)																				*/
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

