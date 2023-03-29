#ifndef REG_TBL_H
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		RegTable.h : Register Manager�p�e�[�u����`													*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	����	2010.02.23	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
/* �f�o�C�X���e�[�u���T�C�Y */
#define DEV_TBL_SIZE		100		/* �b�� 2010.08.20 Y.Oka (>_<) */

/* �s����/�������A�h���X */
#define DEV_VERSION			0x0003
#define EX_ADDRESS			0x0005
#define TR_BYTE_SIZE		0x001C
#define MAIN_DEV_NAME		0x0080
#define SUB_DEV1_NAME		0x0090
#define SUB_DEV1_VER		0x0098
#define SUB_DEV2_NAME		0x00A0
#define SUB_DEV2_VER		0x00A8

/* ID_RD��p��` */
#define TR_BYTE_ERR_FOR_ID	0x00
#define TR_BYTE_32_FOR_ID	0x04
#define TR_BYTE_48_FOR_ID	0x08

/* �A���v��� */
#define AMP_LINEAR_BIT		10		/* ���j�A */
#define AMP_FULC_BIT		8		/* �t���N���[�Y�h */

/* �p�����[�^���[�h�^�C�v */
#define LINEAR_ONLY			0x01	/* ���j�A�̂� */
#define FULLC_ONLY			0x02	/* �t���N�̂� */
#define COMMON_TYPE			0x00	/* �S�^�C�v���� */
#define REG_1WDACC			0x10	/* ���W�X�^1Word�A�N�Z�X */

#define REG_SYSRST_NUM		3
#define REG_RECALC_NUM		3
#define REG_ALMHISTCLR_NUM	3
#define REG_INITPRM_NUM		3
#define REG_CANCELACC_NUM	1

#define	BINARY		0
#define	ASCII		1
#define	ARRAY		2
#define	RESRVE		3


/****************************************************************************************************/
/*		STRUCT DEFINITION																			*/
/****************************************************************************************************/
/* �f�o�C�X���e�[�u����` */
typedef struct DEV_INFO_TBL_ {
	USHORT	id_code;				/* ID_CODE						*/
	UCHAR	type;					/* BINARY=0, ASCII=1, ARRAY=2	*/
	USHORT	size;					/* �f�[�^�T�C�Y					*/
	void	*ptr;					/* �f�[�^�|�C���^				*/
} DEV_INFO_TBL;


/****************************************************************************************************/
/*		EXTERN(�b��)																				*/
/****************************************************************************************************/
extern	const DEV_INFO_TBL	devInfoTbl[];



#define REG_TBL_H
#endif /* REG_TBL_H */
/***************************************** end of file **********************************************/
