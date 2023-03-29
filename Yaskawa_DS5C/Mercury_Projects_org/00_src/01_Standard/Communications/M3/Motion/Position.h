/****************************************************************************
Description	: MECHATROLINK Position Manager
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : Mercury

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------

****************************************************************************/
#ifndef POSITION_H_

#include "MtPRM.h"


/****************************************************************************
 Defines
****************************************************************************/
#define SOFT_LIMIT_P		0x1
#define SOFT_LIMIT_N		0x2

#define SOFT_LIMIT_PBIT		0x0
#define SOFT_LIMIT_NBIT		0x1

//#define LT_NUM			2		/* ���b�`�{�� */
#define LT_NUM				3		/* ���b�`�{�� */	/* <S056>:���b�`�{����ύX */


/****************************************************************************
 Structure of the Position Handle
****************************************************************************/
typedef struct PSN_HNDL_ {
	LONG		Tpos;				/* �ڕW�ʒu */
	LONG		Ipos;				/* �w�߈ʒu�i�t�B���^�O�j */
	LONG		Pos;				/* �w�߈ʒu�i�t�B���^��j */
	LONG		Mpos;				/* FB�ʒu���̓^�C�~���O�ł̎w�߈ʒu */
	LONG		Apos;				/* FB�ʒu */
	LONG		Perr;				/* �΍� */
	LONG		Lpos[LT_NUM];		/* ���b�`�ʒu */
	LONG		LstLpos[LT_NUM];	/* �O�񃉃b�`�ʒu */
	LONG		Fbspd;				/* FB���x */
	LONG		Pgcnt;				/* �ڕW�ʒu */
	LONG		Dpos;				/* ����o�͈ړ����� */
	LONG		lstDpos;			/* �O��o�͈ړ����� */
	LONG		PosOfs;				/* ���ʃ��j�^ POS_OFST */
	LONG		InitPgPos64[2];		/* �����G���R�[�_�ʒu�i64bit�j*/
	BOOL		Den;				/* �w�ߏo�͊��� */
	BOOL		Pset;				/* �ʒu���ߊ����M�� */
	BOOL		Near;				/* Near�M�� */
	BOOL		RefEnable;			/* ���t�@�����X�_�L�� */
	BOOL		PosEnable;			/* ���W�ʒu�L�� */
	LONG		SoftLimit;			/* �\�t�g���~�b�g��� */
	PSN_PRM		*Prm;				/* Potion�p�����[�^ */
} PSN_HNDL;



/****************************************************************************
 API's
****************************************************************************/
/* PSN_HNDL�̏��������� */
VOID PsnInit( PSN_HNDL *hdl, PSN_PRM *prm );
/* FB�f�[�^�쐬���� */
VOID PsnUpdateFBData( PSN_HNDL *hdl, LONG pgcnt, LONG PosErr );
/* PSET�����NEAR�M���쐬����	*//* <S0AE> */
VOID PsnSetPsetState( PSN_HNDL *hdl );
/* TPOS�ݒ菈��(��Βl) */
LONG PsnSetTPOS( PSN_HNDL *hdl, LONG tpos );
/* TPOS�ݒ菈��(���΋���) */
LONG PsnSetTPOSDist( PSN_HNDL *hdl, LONG dpos );
/* ���x�E�g���N���䎞�̈ʒu���킹���� */
VOID PsnResetCmdPosAll( PSN_HNDL *hdl, LONG err );
/* �\�t�g���~�b�g��ԃ`�F�b�N����(APOS) */
LONG PsnCheckSoftLimitFB( PSN_HNDL *hdl, BOOL *p_ot, BOOL *n_ot );
/* ���W�n�ݒ� */
VOID PsnSetCordinate( PSN_HNDL *hdl, LONG mode, LONG pos );
/* ���W�n���Z�b�g */
VOID PsnResetApos( PSN_HNDL *hdl, BOOL abs );
/* LPOS�擾 */
BOOL PsnMakeLpos( PSN_HNDL *hdl, LONG ltcnt, LONG i );



#define POSITION_H_
#endif /* POSITION_H_ */



