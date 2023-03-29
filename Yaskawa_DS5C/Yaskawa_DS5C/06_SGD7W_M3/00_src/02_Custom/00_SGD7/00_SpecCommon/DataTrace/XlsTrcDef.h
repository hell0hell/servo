/****************************************************************************************************/
/*																									*/
/*																									*/
/*		XlsTrcDef.h : �g���[�X��`�w�b�_�t�@�C��													*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1. �g���[�X��`�\����																		*/
/*																									*/
/*		2. �g���[�X��`�e�[�u��Extern�錾															*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		2010.02.12 : Made by Excel Macro															*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"




/****************************************************************************************************/
/*																									*/
/*		�g���[�X��`�\����																			*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		���l�g���[�X��`�\����																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct  {
//		UCHAR	SelNo;								/* �I��No										*/	/* <S17A> */
		USHORT	SelNo;								/* �I��No										*/	/* <S17A> */
//		UCHAR	VxType;								/* �ϐ��^�C�v									*/
		UCHAR	VxSftL;								/* �g���[�X�ϐ����V�t�g							*/
		UCHAR	VxSftR;								/* �g���[�X�ϐ��E�V�t�g							*/
		BOOL	AvrgSel;							/* �g���[�X���ω��I��:TRUE�˕��ω�����			*/	
		LONG	(*GetVarData)( void* Axis );		/* �g���[�X�ϐ��A�h���X�擾�֐�					*/
		LONG	(*GetVarGain)( void* Axis );		/* �g���[�X�ϐ��̃Q�C���v�Z�֐�					*/
} TVXDEF;
/*--------------------------------------------------------------------------------------------------*/
#define XTRCDEF_TxSHORT			0x02				/* Type�FSHORT									*/
/*--------------------------------------------------------------------------------------------------*/
/*		�r�b�g�g���[�X��`�\����																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct  {
		UCHAR	SelNo;								/* �I��No										*/
//		UCHAR	BitNo;								/* �r�b�gNo.									*/
		BOOL	(*GetBitData)( void* Axis );		/* �g���[�X�ϐ��A�h���X�擾�֐�					*/
} TBXDEF;
/*--------------------------------------------------------------------------------------------------*/
/*		TVXDEF/TBXDEF Version																		*/
/*--------------------------------------------------------------------------------------------------*/
#define TVXDEF_VERSION			0x0001				/* TVXDEF Version								*/
#define TBXDEF_VERSION			0x0001				/* TBXDEF Version								*/
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		�g���[�X��`�e�[�u��Extern�錾																*/
/*																									*/
/****************************************************************************************************/
extern  const TVXDEF TrcVarTbl[];					/* ���l�g���[�X��`�e�[�u��						*/
extern  const TBXDEF TrcBitTbl[];					/* �r�b�g�g���[�X��`�e�[�u��					*/
extern  const USHORT TrcVarTblEntNum;				/* ���l�g���[�X��`�e�[�u���o�^��				*/
extern  const USHORT TrcBitTblEntNum;				/* �r�b�g�g���[�X��`�e�[�u���o�^��			*/
//extern  const TBXDEF TrcBitTblUser[];				/* �r�b�g�g���[�X��`�e�[�u��					*/
/*--------------------------------------------------------------------------------------------------*/
extern  const USHORT TrcVarTblUserEntNum;			/* ���l�g���[�X��`�e�[�u���o�^��				*/
extern  const USHORT TrcBitTblUserEntNum;			/* �r�b�g�g���[�X��`�e�[�u���o�^��			*/
/*--------------------------------------------------------------------------------------------------*/
/*		Define Entry Number																			*/
/*--------------------------------------------------------------------------------------------------*/
#if		OPENSW_TRCDEF == 0
#define TRCVARTBL_ENTNUM	TrcVarTblEntNum
#define TRCBITTBL_ENTNUM	TrcBitTblEntNum
#else
#define TRCVARTBL_ENTNUM	(TrcVarTblEntNum + TrcVarTblUserEntNum)
#define TRCBITTBL_ENTNUM	(TrcBitTblEntNum + TrcBitTblUserEntNum)
#endif
/*--------------------------------------------------------------------------------------------------*/







/***************************************** end of file **********************************************/