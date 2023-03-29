/****************************************************************************
Description	: USB function driver
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2013 All Rights Reserved

Project       : Mercury

Functions	  : none

----------------------------------------------------------------------------
Changes		:
Name		Date			Description
------------------------------------------------------------------------
K.Sakamoto	2013.05.12		created

****************************************************************************/
#ifndef USB_F_H__
#define USB_F_H__

/*===========================================================================*/
/* I N C L U D E                                                             */
/*===========================================================================*/
#include "Basedef.h"
//<S150>#include "errcodes.h"	/* �Í����t�H�[�}�b�g�Ή� <S146> */
#include "u2f_desc.h"
//<S146>#include "errcodes.h"

/*===========================================================================*/
/* D E F I N E                                                               */
/*===========================================================================*/
#define U2F_PLUG_OUT    0
#define U2F_PLUG_IN     1

/* <S150> */
/* define of error code */
#define ER_OK      ((ER_RET)0)				/**< Error code for Normal end (no error) */
#define ER_NG      ((ER_RET)-1)				/**< Error code for Abnormal end (error) */
#define ER_SYS     ((ER_RET)(2 * ER_NG))	/**< Error code for Undefined error */
#define ER_PARAM   ((ER_RET)(3 * ER_NG))	/**< Error code for Invalid parameter */
#define ER_NOTYET  ((ER_RET)(4 * ER_NG))	/**< Error code for Incomplete processing */
#define ER_NOMEM   ((ER_RET)(5 * ER_NG))	/**< Error code for Out of memory */
#define ER_BUSY    ((ER_RET)(6 * ER_NG))	/**< Error code for Busy */
#define ER_INVAL   ((ER_RET)(7 * ER_NG))	/**< Error code for Invalid state */
#define ER_TIMEOUT ((ER_RET)(8 * ER_NG))	/**< Error code for Timeout occurs */

/*==========================================================================*/
/* E X T E R N   V A R I A B L E                                            */
/*==========================================================================*/

/*===========================================================================*/
/* P R O T O T Y P E                                                         */
/*===========================================================================*/
/* UsbFuncDriver.c */
/* --- ������ --- */
void	UsbFuncInitializeDriver( ULONG MyAddress );	/* USB Function �h���C�o������					*/
BOOL	UsbFuncChkAvailable( void );				/* USB�g�p�\���菈��							*//*<S138>*/
/* --- ���荞�� --- */
void    UsbFuncIntIntu2f ( void );					/* INTU2F ���荞�ݏ���							*/
void    UsbFuncIntIntu2fepc ( void );				/* INTU2FEPC ���荞�ݏ���						*/
/* --- �R���g���[���]�� --- */
void	UsbFuncTaskControl( void );					/* �R���g���[���]���^�X�N����					*//*<S1A2>*/
/* --- �o���N�]���^�X�N --- */
void	UsbFuncPollBulkOutIn( void );				/* �o���N�]���������C��(ScanB������s��)		*/

ULONG	UsbFuncStartBkout( UCHAR *Buffer ,USHORT );	/* �o���N�]��(OUT)�J�n�v���֐�					*/
ULONG	UsbFuncStartBkin( UCHAR *Buffer ,USHORT );	/* �o���N�]��(IN) �J�n�v���֐�					*/
USHORT	UsbFuncGetRcvSize( void );					/* ��M�������̎�M�o�C�g���擾					*/
ULONG	UsbFuncChkRcvComp( void );					/* �o���N�]��(OUT)��ԃ`�F�b�N					*/
ULONG	UsbFuncChkTrnsComp( void );					/* �o���N�]��(IN)��ԃ`�F�b�N					*/
void	UsbFuncResetBuffer( void );					/* �o���N�]���p�o�b�t�@�̃N���A����				*/

ULONG	UsbFuncPortConfiguration( void );			/* <S150> */

/*--------------------------------------------------------------------------------------------------*/
/*	�G���h�|�C���g1or2��Ԓ�`																		*/
/*--------------------------------------------------------------------------------------------------*/
#define	BLKTRNS_DISABLE			0x00		/* �o���N�]���֎~										*/
#define	BLKTRNS_ENABLE			0x01		/* �o���N�]������										*/
#define	BLKTRNS_ACTIVE			0x02		/* �o���N�]�����쒆										*/
#define	BLKTRNS_END				0x03		/* �o���N�]������										*/
#define	BLKTRNS_STALL			0x10		/* �o���N�]���ɂĈُ픭����STALL����					*/

/****************************************************************************************************/
/*																									*/
/*		INTUSB0B�ɂ��R���g���[���]������															*/
/*																									*/
/****************************************************************************************************/
#define	REQ_ANA					0		/* CPUDEC�����݂ɂ��f�[�^���N�G�X�g��͏���				*/
#define	EP0_INTAKEN				1		/* EP0IN�����݂ɂ��h�m�g�[�N������						*/
#define	EP0_INTAKEN_CMP			2		/* EP0INDT�����݂ɂ��h�m�g�[�N����������					*/

#endif // USB_F_H__
/*-------end of file----------------------------------------------------------------------------------------*/
