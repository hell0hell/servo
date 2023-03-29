/****************************************************************************/
/*																			*/
/*																			*/
/*		PRM Bank ヘッダファイル												*/
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
#ifndef		PRM_BANK_H_
#define		PRM_BANK_H_

/* defines */
/* error code */
#define MBANK_SUCCESS	0		/* 正常終了 */
#define MBANK_INIT_ERR	(-1)	/* 異常終了(初期計算異常) */

/* typedefs: struct */
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

/* forward declarations */
#ifdef PRM_BANK_ENABLE
extern LONG MBnkInit(MTPRM* prm, USHORT bnkNum, USHORT memNum, USHORT* pMember);
extern LONG MBnkSetData(MTPRM* prm, USHORT* pVal, USHORT dataNo);
extern LONG MBnkSetDefData(MTPRM* prm, LONG data, MTBNK_TBLIDX idx);
#define MBnkChange(prm)							{(prm)->Bnk.bankSel = (prm)->Bnk.cmdSel;}
#define MBnkSetSelector(prm, sel)				{if((sel) < (prm)->Bnk.bankNum) (prm)->Bnk.cmdSel = sel;}
#define MBnkIsEnable(prm)						(prm)->Bnk.bankNum

#else
#define MBnkInit(prm, bnkNum, memNum, pMember)	(MBANK_SUCCESS)
#define MBnkSetData(prm, pVal, dataNo)			(TRUE)
#define MBnkSetDefData(prm, data, idx)			{;}
#define MBnkChange(prm)							{;}
#define MBnkSetSelector(prm, sel)				{;}
#define MBnkIsEnable(prm)						(0)
#endif

/* 操作関数マクロ定義 */
#define MBnkGetMgPrm(prm)	&((prm)->Bnk.mgPrm[(prm)->Bnk.bankSel])
#define MBnkGetAcfPrm(prm)	&((prm)->Bnk.acfPrm[(prm)->Bnk.bankSel])


#endif /* PRM_BANK_H_ */

