/*****************************************************************************
Description		: �f�[�^�g���[�X��` ���[�J���w�b�_�t�@�C��
-----------------------------------------------------------------------------
Author		: SHCORE
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
K.Liu		2013.05.20		created

*****************************************************************************/
#ifndef _PNLOP_H
#define _PNLOP_H

#include "Basedef.h"


/******************************************************************************/
/*		STRUCT DEFINITION													  */
/******************************************************************************/
typedef	struct _OPKEYFLG
{
	UINT	mode		:1;
	UINT	up			:1;
	UINT	down		:1;
	UINT	shup		:1;
	UINT	shdown		:1;
	UINT	data		:1;
	UINT	svon		:1;
	UINT	almrst		:1;
	UINT	scroll		:1;
	UINT	read		:1;
	UINT	write		:1;
	UINT	rst			:1;	/* up+down��������	*/
}	OPKEYFLG;

typedef	struct _BLINKFLG
{
	UINT	PrmEewrite	:1;
	UINT	read		:1;
	UINT	write		:1;
	UINT	no_op		:1;
	UINT	error		:1;
	UINT	done		:1;
	UINT	end			:1;
}	BLINKFLG;

/****************************************************************************************************/
/*																									*/
/*		���j�^��`�\����																			*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		�t�����j�^��`�\����																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct _MONDEF
{
	UINT	IsLong     : 1;						/* ���j�^�f�[�^��LONG�\��							*/
	UINT	Base       : 2;						/* �\�����̒�(DEC/HEX/BIT)							*/
	UINT	Sign       : 1;						/* �\�����̕����L��(W_SIGN:�L/NOSIGN:��)			*/
	UINT	Decimals   : 3;						/* �����_�ȉ�����(0-4)								*/
	UINT	ValClr     : 1;						/* ���j�^�ϐ��N���A(DISABLE/ENABLE)					*/
	UINT	GType      : 2;						/* GTYPE_VAL(�ϐ�)/GTYPE_CONST(�萔)				*/
	UINT	NoneDisp   : 1;						/* ���j�^������none�\��								*/
	UINT	SpareBits  : 5;						/* �\��												*/
/*--------------------------------------------------------------------------------------------------*/
	UCHAR	ChkSpUse;							/* ��p�g�p(COMMON/ROTARY/LINEAR/etc)				*/
	UCHAR	AccessLevel;						/* �A�N�Z�X���x��(USR1/USR2/etc)					*/
/*--------------------------------------------------------------------------------------------------*/
	ULONG	(*FuncPtr)( struct _MONDEF * );		/* ���j�^�f�[�^���Z�֐�								*/
/*--------------------------------------------------------------------------------------------------*/
	union	{									/* ���j�^�ϐ���`									*/
	ULONG	Ulong;								/* ��`�p(�ϐ��������́A�ŏ��̃����o���L��)			*/
	LONG	*Plong;								/* LONG�ϐ��ւ̃|�C���^								*/
	ULONG	*Pulong;							/* ULONG�ϐ��ւ̃|�C���^							*/
	SHORT	*Pshort;							/* SHORT�ϐ��ւ̃|�C���^							*/
	USHORT	*Pushort;							/* USHORT�ϐ��ւ̃|�C���^							*/
	} Var;										/* ���j�^�ϐ��ւ̃|�C���^							*/
/*--------------------------------------------------------------------------------------------------*/
	union	{									/* ���j�^���Z�Q�C����`								*/
	ULONG	Ulong;								/* ��`�p(�ϐ��������́A�ŏ��̃����o���L��)			*/
	LONG	*Plong;								/* LONG�Q�C���ւ̃|�C���^							*/
	ULONG	*Pulong;							/* ULONG�Q�C���ւ̃|�C���^							*/
	SHORT	Short[2];							/* SHORT�Q�C�� : Var*Short[0]/Short[1]				*/
	USHORT	Ushort[2];							/* USHORT�Q�C�� : Var*Ushort[0]/Ushort[1]			*/
	} Gain;										/* ���j�^�Q�C���ւ̃|�C���^�܂��͒萔				*/
} MONDEF;
typedef const MONDEF CMONDEF;
/*--------------------------------------------------------------------------------------------------*/
/*		�t�����j�^�Ǘ��e�[�u���\����																*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct _MONTBL
{
	USHORT	UnNo;								/* UnNo/RegNo										*/
	CMONDEF	*MonDefPtr;							/* �f�[�^��`�ւ̃|�C���^							*/
} MONTBL;
typedef const MONTBL CMONTBL;

/*--------------------------------------------------------------------------------------------------*/
/*		�����݋֎~ & ���[�U���x���\����																*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _ACCESS
{
	USHORT	inhibit;	/* �����݋֎~						*/
	USHORT	level;		/* ���[�U���x��							*/
	USHORT	org_inhi;	/* �d���������̏����݋֎~���			*/
	USHORT	OpeDspMode;	/* �I�y���[�^�̃��[�U�[�萔�\���I��		*/							/* <V041> */
} ACCESS;

/*--------------------------------------------------------------------------*/
/*		���W�X�^�h�e�p�f�[�^��`											*/
/*--------------------------------------------------------------------------*/
typedef	struct	_MSGIF {				/* ���b�Z�[�W�h�e�p�\���̒�`				*/
	UCHAR  *CmdBuf;						/* �w�߃��b�Z�[�W�o�b�t�@�|�C���^			*/
	UCHAR  *ResBuf;						/* �������b�Z�[�W�o�b�t�@�|�C���^			*/
	LONG  (*StxFunc)( UCHAR *, USHORT );/* �������b�Z�[�W���M�J�n�`�o�h�֐�			*/
	void  (*ResFunc)( struct _MSGIF * );/* �e�����s���ʉ������b�Z�[�W�����֐�		*/
	USHORT	RcvLen;						/* �w�߃��b�Z�[�W��							*/
	USHORT	RcvBufSize;					/* ��M�o�b�t�@�T�C�Y						*/
	USHORT	SndBufSize;					/* ���M�o�b�t�@�T�C�Y						*/
	USHORT	FunSts;						/* �⏕�@�\���s����							*/
//	USHORT	FunIdx;						/* �⏕�@�\�e�[�u���C���f�b�N�X				*///<V620>
	USHORT	FunExeIdx;					/* �⏕�@�\���s�e�[�u���w��C���f�b�N�X		*//* <V620> */
	USHORT	FnStsToOpi;					/* �⏕�@�\���s����(OPTION-SERVO I/F�p)		*//* <V620> */
	USHORT	FunResHold;					/* �⏕�@�\�����z�[���h						*/
	USHORT	ErrRno;						/* �ُ탌�W�X�^�ԍ�							*/
	USHORT	LstErrRno;					/* �O��R�}���h�ُ̈탌�W�X�^�ԍ�			*/
	USHORT	AccsLvl;					/* �A�N�Z�X���x��							*/
	USHORT	ComPortType;				/* �ʐM���̃`���l�����						*//* <V239> Add */
	USHORT	Spare00;					/* Spare for alignment						*//* <V620> */
	ULONG	ComTimeLong;				/* �ʐM�^�C���A�E�g�v���p					*/
#if 0
#if (SERVO_OPT_IF_VER >= 0x0210)
	USHORT	ComTimeShort;				/* �I�v�V�����ʐM�^�C���A�E�g�v���p			*/
	USHORT	RcvWait;					/* SC���b�Z�[�W�ʐM��M�҂��t���O			*/
#endif
#endif
} MSGIF;
typedef	MSGIF*	HMSGIF;					/* ����p�n���h����`					*/

typedef struct	_PNLOP_DISP_CTRL
{	/*Structure of Display Control */
	USHORT	CurDigit;	/* ���݃J�[�\���ʒu							*/
	USHORT	Decimals;	/* �����_�ȉ�����							*/
	SHORT	LLimit;		/* �����l									*/
	SHORT	ULimit;		/* ����l;									*/
	USHORT	Base;		/* DATA �\���̒�							*/
	USHORT	Sign;		/* �����L��									*/
	USHORT	DspPos;		/* ���ݕ\���ʒu								*/
	USHORT	SupFlg;		/* 0�\���}�~�t���O							*/
	USHORT	Digit;		/* ��������									*/
	USHORT	StartDgt;	/* �J�n��									*/
	USHORT	Length;		/* �\��������								*/
} PNLOP_DISP_CTRL;



/*-------------------------------------------End Of File  -----------------------------------------------*/
#endif	/* _PNLOP_H */

