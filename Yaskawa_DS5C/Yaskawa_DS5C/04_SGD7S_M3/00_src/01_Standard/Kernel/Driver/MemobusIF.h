#ifndef	_MEMOBUS_IF_H /* �ēǂݍ��ݖh�~�p */
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		MemobusIF.h : Memobus�����֐���`															*/
/*																									*/
/*																									*/
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
/*																									*/
/*																									*/
/*	Note	:	����	2010.09.13	T.Kira	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"
#ifndef	_MERCURY_PLATFORM_			/*<S190>*/
#include "PrmAccessIf.h"
#endif	/*_MERCURY_PLATFORM_*/



/****************************************************************************************************/
/*		DEFINES																						*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		�`�����l����`																				*/
/*--------------------------------------------------------------------------------------------------*/
#define	COM2_OPT1		0x01				/* CH2 : C1 message										*/
#define	COM3_OPT2		0x02				/* CH3 : C2 message										*/

/*--------------------------------------------------------------------------------------------------*/
/*	MEMOBUS ���b�Z�[�W�ʐM�w�b�_����`																*/
/*--------------------------------------------------------------------------------------------------*/
#define	MSG_MAXBUFSZB			582				/* �o�b�t�@�T�C�Y(>=�ő�Memobus�T�C�Y)				*/
#define	MBUS_MINSIZE			8				/* MEMOBUS���b�Z�[�W�ŏ��o�C�g��					*/
#define	MBUS_MAXSIZE			480				/* MEMOBUS���b�Z�[�W�ő�o�C�g��(LONG�Ŋ���؂�鐔)*/
#define	MBUS_HDROFS				0x00			/* �w�b�_���̃T�C�Y(0Byte)							*/
#define	MBUS_MIN_REGNUM			1				/* �ŏ����W�X�^�Ǐo�������݌�						*/





#ifndef	_MERCURY_PLATFORM_			/*<S190>*/
/****************************************************************************************************/
/*		STRUCT DEFINITION																			*/
/****************************************************************************************************/
typedef	struct	_MEMOBUS_ADDR_MAP {
	LONG				AxisAddress;
	REG_MANAGER_HANDLE	*hRegMngr;
	void				*Axis;		/* �����̐擪�A�h���X */
} MEMOBUS_ADDR_MAP;

/*--------------------------------------------------------------------------------------------------*/
/*		���W�X�^�h�e�p�f�[�^��`																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	_MEMOBUS_IF {					/* ���b�Z�[�W�h�e�p�\���̒�`						*/
	UCHAR	RxBuf[ MSG_MAXBUFSZB ];				/* ��M�f�[�^�o�b�t�@								*//* <S01C> */
	UCHAR	TxBuf[ MSG_MAXBUFSZB ];				/* ���M�f�[�^�o�b�t�@								*//* <S01C> */
//	UCHAR	*RxBuf;								/* ��M�f�[�^�o�b�t�@								*//* <S01C> */
//	UCHAR	*TxBuf;								/* ���M�f�[�^�o�b�t�@								*//* <S01C> */
	UCHAR	*CmdBuf;							/* �w�߃��b�Z�[�W�o�b�t�@�|�C���^					*/
	UCHAR	*ResBuf;							/* �������b�Z�[�W�o�b�t�@�|�C���^					*/
	USHORT	RelayHdSize;						/* ���p�R�}���h�w�b�_�[�T�C�Y						*/
	USHORT	RcvbLength;							/* ��M�f�[�^��(�o�C�g�P��)							*/
	USHORT	RcvwLength;							/* ��M�f�[�^��(���[�h�P��)							*/
	USHORT	TrnbLength;							/* ���M�f�[�^��(�o�C�g�P��)							*/
	LONG	NodeAddress;						/* ���ǃA�h���X										*/
	ULONG	ComTimeLong;						/* �ʐM�^�C���A�E�g�v���p							*/

	MEMOBUS_ADDR_MAP	AxisMap[MAX_AXIS_NUMBER];/* ���A�h���X��`									*/
	
	ULONG	ErrRno;								/* ���W�X�^�ǂݏo��:MEMOBUS�G���[���				*/	/* <S056> */
	USHORT	OpeDspMode;							/* �I�y���[�^�̃��[�U�[�萔�\���I��					*/	/* <S056> */

} MEMOBUS_IF;



/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
/* MEMOBUS ���b�Z�[�W���������� */
void	MbusFuncInitialize(MEMOBUS_IF *MbusIfPtr, MEMOBUS_ADDR_MAP *AxisCfg, LONG node_address );

/* MEMOBUS ���b�Z�[�W��́����s����	*/
USHORT	MbusFuncMsgProcedure( MEMOBUS_IF *MbusIfPtr );

/* MEMOBUS ���b�Z�[�W�o�b�t�@ Swap����(BigEndian�p) */
void mbusFuncBufSwap( UCHAR *RxBuf );
#endif	/*_MERCURY_PLATFORM_*/




/***************************************** end of file **********************************************/
#define _MEMOBUS_IF_H /* �ēǂݍ��ݖh�~�p */
#endif /* _MEMOBUS_IF_H */
