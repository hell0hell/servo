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
/* FLASH上のアドレス情報など */
#define FLS_BOOT_BTM_ADDRESS		0xFFFFFFFF		/* ブート領域最終アドレス						*/
#define FLS_BOOT_TOP_ADDRESS		0xFFFF0000		/* ブート領域先頭アドレス						*/
#define FLS_FACTORY_PASSWD_ADDRESS	0xFFFEFFF8		/* 初回起動時パスワード格納アドレス				*/
#define FLS_CODE_EFW_ADDRESS		0xFFF30000		/* 暗号化ファイル格納アドレス(通常)				*/
#define FLS_MIRROR_EFW_ADDRESS		0xFFE80000		/* 暗号化ファイル格納アドレス(ミラー)			*/
#define FLS_MICRO_CODE_ADDRESS		0xFFE70000		/* マイクロコード格納アドレス						*/
#define FLS_CODE_AREA_SIZE			0x000B0000		/* コード領域サイズ(704KB)						*/
#define FLS_BOOT_AREA_SIZE			0x00010000		/* ブート領域サイズ(64KB)						*/
#define FLS_BOOT_SUM_SIZE			0x00000002		/* ブート領域チェックサム値サイズ(2)			*/
#define FLS_BOOT_SUM_OFFSET			(FLS_BOOT_AREA_SIZE - FLS_BOOT_SUM_SIZE)
#define FLS_MICRO_AREA_SIZE			0x00010000		/* マイクロコード領域サイズ(64KB)					*/
#define FLS_MICRO_SUM_SIZE			0x00000002		/* マイクロコード領域チェックサム値サイズ(2)			*/
#define FLS_MICRO_SUM_OFFSET		(FLS_MICRO_AREA_SIZE - FLS_MICRO_SUM_SIZE)

/* ZIPローカルヘッダのシグネチャコード(little endian 32ビット値)*/
#define ZIP_LOCAL_HEADER_SIGNATURE	0x04034B50		/* 50 4B 03 04 のUINT32チェック用				*/

/* typedefs */
/* ファームヘッダ */
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
