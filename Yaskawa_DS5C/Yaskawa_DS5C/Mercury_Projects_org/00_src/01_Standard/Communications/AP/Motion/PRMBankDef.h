/****************************************************************************/
/*																			*/
/*																			*/
/*		PRM Bank �w�b�_�t�@�C��												*/
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
#define BANK_NUM_MAX	16	/* �ő�o���N�� */
#define BANK_MEM_MAX	15	/* �ő僁���o�� */
#define BANK_DATA_MAX	64	/* �ő�o���N�f�[�^�� */

typedef struct tagMTBANK {
	UCHAR		bankNum;				/* �o���N�� */
	UCHAR		memNum;					/* �����o�� */
	UCHAR		bankSel;				/* �o���N�Z���N�^���ݒl */
	UCHAR		cmdSel;					/* �o���N�Z���N�^�w�ߒl */
	ULONG		enaIdxBits;				/* ���ttable */
	struct {
		UCHAR	idx;					/* ���t�����o��`�e�[�u���ւ�Idx */
		UCHAR	highWd;					/* 4byte�����o����High Word�w�� */
	}memAttr[BANK_MEM_MAX];				/* �����o���t�e�[�u�� */
	MG_PRM		mgPrm[BANK_NUM_MAX];	/* MG�p�o���N�f�[�^�o�b�t�@ */
	ACF_PRM		acfPrm[BANK_NUM_MAX];	/* AccFilter�p�o���N�f�[�^�o�b�t�@ */
} MTBANK;


#endif /* PRM_BANK_H_ */

