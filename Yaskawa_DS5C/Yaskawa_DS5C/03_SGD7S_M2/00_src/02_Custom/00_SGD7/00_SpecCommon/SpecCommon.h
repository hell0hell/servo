/****************************************************************************
Description	: Definitions for each product functionality
----------------------------------------------------------------------------
File name		: SpecCommon.h		<S008>

Author			: Yaskawa Electric Corp.,
					Copyright (c) 2012 All Rights Reserved

Project			: Mercury

----------------------------------------------------------------------------
Changes :
	Name	Date		Description
  ------------------------------------------------------------------------
	Y.Oka	2013.08.17	created

******************************************************************************/
#ifndef _SPEC_COMMON_H
#define _SPEC_COMMON_H



/****************************************************************************
	Define the FirmWare Version
****************************************************************************/
#if (FLOAT_USE==TRUE)	/* Float�Ή��ł͎b��I�Ƀ}�C�i�[ver�̐擪�ɂ�F��t����		<S00A>	*/
#define SERVO_FIRM_VER		0xF009F016	/* �\�t�g�E�F�AVer. */
										/* byte3,4:���W���[Ver. */
										/* byte1,2:�}�C�i�[Ver. */
#else
#define SERVO_FIRM_VER		0xF0090016	/* �\�t�g�E�F�AVer. */
										/* byte3,4:���W���[Ver. */
										/* byte1,2:�}�C�i�[Ver. */
#endif

/* <S056>	Start */
/********************************************************************/
/*																	*/
/*	�uSERVO_YSPEC_VER�v�ɂ�Y�d�AFT�d�l�AEX�d�l��Ver���Ǘ����邽�߁A	*/
/*	��`���̂��uSERVO_SPECIAL_SPEC_VER�v�֕ύX���܂��B				*/
/*																	*/
/********************************************************************/
//#define SERVO_YSPEC_VER		0x0000	/* B4-B15:Y�d�lVer. */
										/* B0-B3:Y�d�lVer.���Ŕԍ� */

#define SERVO_SPECIAL_SPEC_VER	0x0000	/* B4-B15:����Ver. */
										/* B0-B3:����Ver.���Ŕԍ� */


#define REG_SPEC_VER	0x0E000101	/* ���W�X�^�X�y�b�N�o�[�W���� */
									/* B0-B7:�f�o�C�XID */
									/* B8-B15:�T�v���C���[ID */
									/* B16-B23:�}�C�i�[�o�[�W���� */
									/* B24-B31:���W���[�o�[�W���� */

#define SERVO_BOOT_VER	0x0002		/* �\�t�g�E�F�A�u�[�gVer. */

/************************************************************************************************************/
/*																											*/
/*�p�����[�^Ver.�Ǘ����@�͈ȉ��Ƃ���																		*/
/*	�E�o�׎��ݒ��ύX�����ꍇ�̓f�[�^���Ŕԍ����{�P����B(�f�[�^���Ŕԍ�:PRM_DATA_VERSION)					*/
/*	�E�p�����[�^�̃t�H�[�}�b�g�ύX���i�p�����[�^���̑����A�z�u�ύX�j�̓t�H�[�}�b�g���Ŕԍ����{�P���A		*/
/*	�@���f�[�^���Ŕԍ����{�P����B(�t�H�[�}�b�g���Ŕԍ�:PRM_FORMAT_VERSION)								*/
/*	�E���Ŕԍ��͂P�U�i���i�o�C�i���j�Œ�`����B															*/
/*																											*/
/************************************************************************************************************/
/* �����W�X�^�Ή��ŁA���W�X�^�d�l��(900-133-284)�̊Ǘ����@�ɍ��킹��ׁA�uPRM_VERSION�v�ł̃p�����[�^Ver.�Ǘ������߁A�ȉ���2��Ver.�Ǘ�������悤�ɕύX���܂����B */
//#define PRM_VERSION			0x0001		/* �p�����[�^Ver. */
#define PRM_DATA_VERSION		0x0001		/* �p�����[�^Ver.(�f�[�^���Ŕԍ�) */
#define PRM_FORMAT_VERSION		0x0001		/* �p�����[�^Ver.(�t�H�[�}�b�g���Ŕԍ�) */
/* <S056>	End */



/****************************************************************************
	Define the CSW_NETIF_TYPE
****************************************************************************/
#define	NETIF_ANLGPLS_TYPE   1			/* Analog/Pulse type				*/
#define	NETIF_M2_TYPE        2			/* MECHATROLINK-II type				*/
#define	NETIF_M3_TYPE        3			/* MECHATROLINK-III type			*/
#define	NETIF_CM_TYPE   	 4			/* ECAT��̂�CMD�͓��������ɂ���(�\�Ȍ���) <S008>	*/
#define	NETIF_SYNQNET_TYPE   5			/* Synqnet??						*/
#define	NETIF_ECAT_TYPE   	 6			/* EtherCAT������SoC�ōs���ꍇ		*/



/****************************************************************************
	Define the CSW_BOARD_TYPE	<S0A2>
****************************************************************************/
#define	SGD7S_TYPE			1			/* Power Board is SGD7S Type		*/
#define	SGD7W_TYPE			2			/* Power Board is SGD7W Type		*/

#endif /* _SPEC_COMMON_H */
/***************************************** end of file **********************************************/
