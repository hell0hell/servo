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
#ifndef		PRM_BANKDEF_H_
#define		PRM_BANKDEF_H_


/* defines */
#define BANK_NUM_MAX	16	/* 最大バンク数 */
#define BANK_MEM_MAX	15	/* 最大メンバ数 */
#define BANK_DATA_MAX	64	/* 最大バンクデータ数 */

typedef struct tagMTBANK {
	UCHAR		bankNum;				/* バンク数 */
	UCHAR		memNum;					/* メンバ数 */
	UCHAR		bankSel;				/* バンクセレクタ現在値 */
	UCHAR		cmdSel;					/* バンクセレクタ指令値 */
	ULONG		enaIdxBits;				/* 割付table */
	struct {
		UCHAR	idx;					/* 割付メンバ定義テーブルへのIdx */
		UCHAR	highWd;					/* 4byteメンバ時のHigh Word指定 */
	}memAttr[BANK_MEM_MAX];				/* メンバ割付テーブル */
	MG_PRM		mgPrm[BANK_NUM_MAX];	/* MG用バンクデータバッファ */
	ACF_PRM		acfPrm[BANK_NUM_MAX];	/* AccFilter用バンクデータバッファ */
} MTBANK;


#endif /* PRM_BANK_H_ */

