/****************************************************************************
Description	: Elf loader for Power PC, JL-086	<S146>
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2012 All Rights Reserved

Project       : SGDZ-BS63/80, SGDV-MD, SGD7

----------------------------------------------------------------------------
Changes       :
  Name   Date          Description
  ------------------------------------------------------------------------
  I.M.   2012.10.24     created

******************************************************************************/
#ifndef _ELF_LOADER_H
#define _ELF_LOADER_H

/* defines*/
/* FLASH��̃A�h���X���Ȃ� */
#define FLS_BOOT_BTM_ADDRESS		0xFFFFFFFF		/* �u�[�g�̈�ŏI�A�h���X						*/
#define FLS_BOOT_TOP_ADDRESS		0xFFFF0000		/* �u�[�g�̈�擪�A�h���X						*/
#define FLS_FACTORY_PASSWD_ADDRESS	0xFFFEFFF8		/* ����N�����p�X���[�h�i�[�A�h���X				*/
#define FLS_CODE_EFW_ADDRESS		0xFFF30000		/* �Í����t�@�C���i�[�A�h���X(�ʏ�)				*/
#define FLS_MIRROR_EFW_ADDRESS		0xFFE80000		/* �Í����t�@�C���i�[�A�h���X(�~���[)			*/
#define FLS_MICRO_CODE_ADDRESS		0xFFE70000		/* �}�C�N���R�[�h�i�[�A�h���X						*/
#define FLS_CODE_AREA_SIZE			0x000B0000		/* �R�[�h�̈�T�C�Y(704KB)						*/
#define FLS_BOOT_AREA_SIZE			0x00010000		/* �u�[�g�̈�T�C�Y(64KB)						*/
#define FLS_BOOT_SUM_SIZE			0x00000002		/* �u�[�g�̈�`�F�b�N�T���l�T�C�Y(2)			*/
#define FLS_BOOT_SUM_OFFSET			(FLS_BOOT_AREA_SIZE - FLS_BOOT_SUM_SIZE)
#define FLS_MICRO_AREA_SIZE			0x00010000		/* �}�C�N���R�[�h�̈�T�C�Y(64KB)					*/
#define FLS_MICRO_SUM_SIZE			0x00000002		/* �}�C�N���R�[�h�̈�`�F�b�N�T���l�T�C�Y(2)			*/
#define FLS_MICRO_SUM_OFFSET		(FLS_MICRO_AREA_SIZE - FLS_MICRO_SUM_SIZE)

/* ZIP���[�J���w�b�_�̃V�O�l�`���R�[�h(little endian 32�r�b�g�l)*/
#define ZIP_LOCAL_HEADER_SIGNATURE	0x04034B50		/* 50 4B 03 04 ��UINT32�`�F�b�N�p				*/

/* typedefs */
/* �t�@�[���w�b�_ */
typedef struct FIRM_HEADER_T_ {
	UINT32	zip_file_size;
	UINT16	check_sum;
	UINT16	all_check_sum;
} FIRM_HEADER_T;

typedef enum EFW_ERR {
	EFW_SUCCESS = 0,

	EFW_ERR_UNKNOWN_TYPE,
	EFW_ERR_OPEN_IMAGE,
	EFW_ERR_HEADER,
	EFW_ERR_PRG_HEADER,
	EFW_ERR_PRG_SEGMENT,
	EFW_ERR_OTHER_SECTION,
	EFW_ERR_CHECK_SUM,
} EFW_ERR;


EFW_ERR EFW_Loader(void);


#endif //_ELF_LOADER_H
