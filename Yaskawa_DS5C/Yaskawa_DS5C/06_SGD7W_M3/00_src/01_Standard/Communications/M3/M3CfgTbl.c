/****************************************************************************
Description	: �ʐM�ݒ��`
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : Mercury

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
I.M.   2010.10.18     created

****************************************************************************/
#include "Basedef.h"
#include "RegTable.h"
#include "ComCfg.h"


/****************************************************************************
 �Ή��`����������`
****************************************************************************/
#define TIM_TBL_NUM			12
const CFG_TIM_TBL CfgTimTbl[TIM_TBL_NUM] = {
	{500,1,1},		/* 125us,250us,250us */
	{1000,1,1},		/* 250us,250us,250us */
	{2000,1,1},		/* 500us,500us,250us */
	{3000,1,1},		/* 750us,750us,250us */
	{4000,1,1},		/* 1000us,1000us,250us */
	{6000,1,1},		/* 1500us,1500us,250us */
	{8000,1,1},		/* 2000us,2000us,250us */
	{10000,1,1},	/* 2500us,2500us,250us */
	{12000,1,1},	/* 3000us,3000us,250us */
	{14000,1,1},	/* 3500us,3500us,250us */
	{16000,1,1},	/* 4000us,4000us,250us */
	{0xFFFF,1,1},	/* Table End */
};

/****************************************************************************
 �ʐMIF�^����`
****************************************************************************//* <S155> */
UCHAR COM_MODEL_STR[24] =
{"MECHATROLINK3-IF"};


#if 0
#include "alarm.h"
#include "ComAlmMngr.h"
/****************************************************************************
 MECHATROLINK�A���[��/���[�j���O��`�e�[�u��	
****************************************************************************/
const NET_ALM_DEF net_alm_tbl[] = {
	{0x04A, ALM_OPTPRM_ERR,		TYP_ALM | RST_DISABLE, 0x00 | M3STD_DEV_ALM , 0x00 },/* �p�����[�^�ݒ�ُ� */
	{0x51A, ALM_TRACE_SPD_OF,	TYP_ALM | RST_ENABLE,  0x00 | M3STD_DEV_ALM , 0x00 },/* �Ǐ]���x�ߑ�A���[�� */
	{0xBFB, ALM_SCANN_STOP,		TYP_ALM | RST_DISABLE, 0x00 | M3STD_DEV_ALM , 0x00 },/* �������ԃI�[�o(ScanN2) */
	{0xB6A, ALM_COMLSI_ERR1,	TYP_ALM | RST_DISABLE, 0x00 | M3STD_DEV_ALM , 0x00 },/* JL-080���������s */
	{0xB6B, ALM_COMLSI_ERR2,	TYP_ALM | RST_DISABLE, 0x00 | M3STD_DEV_ALM , 0x00 },/* JL-080�����V�X�e���G���[ */
	{0xE40, ALM_TCYC_ERR,		TYP_ALM | RST_ENABLE,  0x00 | M3STD_DEV_ALM , 0x00 },/* MECHATROLINK�`�������ݒ�ُ� */
	{0xE41, ALM_CD_LEN_ERR,		TYP_ALM | RST_DISABLE, 0x00 | M3STD_DEV_ALM , 0x00 },/* MECHATROLINK�ʐM�f�[�^�T�C�Y�ݒ�ُ� */
	{0xE42, ALM_AXIS_NO_ERR,	TYP_ALM | RST_DISABLE, 0x00 | M3STD_DEV_ALM , 0x00 },/* MECHATROLINK�ǃA�h���X�ݒ�ُ� */
	{0xE50, ALM_COMSYNC,		TYP_ALM | RST_ENABLE,  0xC0 | M3STD_COMM_ALM, 0x00 },/* MECHATROLINK WDT�ُ� */
	{0xE51, ALM_COMSYNC_TOUT,	TYP_ALM | RST_ENABLE,  0xC0 | M3STD_COMM_ALM, 0x00 },/* MECHATROLINK WDT�ُ�(��������) */
	{0xE60, ALM_COM,			TYP_ALM | RST_ENABLE,  0x90 | M3STD_COMM_ALM, 0x00 },/* MECHATROLINK�ʐM�ُ� */
	{0xE61, ALM_COM_INT_ERR,	TYP_ALM | RST_ENABLE,  0xB0 | M3STD_COMM_ALM, 0x00 },/* �l�b�g���[�N�����Ԋu�ُ� */
	{0xE62, ALM_COM_FCS, 		TYP_ALM | RST_ENABLE,  0x80 | M3STD_COMM_ALM, 0x00 },/* �l�b�g���[�NFCS�G���[ */
	{0xE63, ALM_COM_GBL, 		TYP_ALM | RST_ENABLE,  0xA0 | M3STD_COMM_ALM, 0x00 },/* �O���[�o���f�[�^��M�G���[ */
	{0xE64, ALM_MON_SLV_COM,	TYP_ALM | RST_ENABLE,  0x00 | M3STD_DEV_ALM , 0x00 },/* ���ǃ��j�^�f�[�^��M�G���[ */
	{0xE6A, ALM_COM_RCV_ASYNC,	TYP_ALM | RST_ENABLE,  0x90 | M3STD_COMM_ALM , 0x00 },/* �l�b�g���[�N��M�G���[�i�C�x���g�h���u���ʐM�j */
	{0xE6B, ALM_COM_TOUT_ASYNC, TYP_ALM | RST_ENABLE,  0x00 | M3STD_COMM_ALM , 0x00 },/* �l�b�g���[�N��M�^�C���A�E�g�i�C�x���g�h���u���ʐM�j */
	{0xE6C, ALM_CON_SEND_ASYNC, TYP_ALM | RST_ENABLE,  0x00 | M3STD_COMM_ALM , 0x00 },/* �l�b�g���[�N���M�ُ�i�C�x���g�h���u���ʐM�j */
	{0xEA0, ALM_NOANS,			TYP_ALM | RST_DISABLE, 0x00 | M3STD_DEV_ALM , 0x00 },/* �T�[�{�p�b�N�̏� */
	{0xEA1, ALM_SVINIT_ERR,		TYP_ALM | RST_DISABLE, 0x00 | M3STD_DEV_ALM , 0x00 },/* �T�[�{�����A�N�Z�X�ُ� */
	{0xEA2, ALM_SVWDC_ERR,		TYP_ALM | RST_DISABLE, 0x00 | M3STD_DEV_ALM , 0x00 },/* �T�[�{WDC�ُ� */
	{0xED0, ALM_SVANS_TOUT,		TYP_ALM | RST_ENABLE,  0x00 | M3STD_DEV_ALM , 0x00 },/* �T�[�{������ */
	{0xED1, ALM_CMDEXEC,		TYP_ALM | RST_ENABLE,  0x00 | M3STD_DEV_ALM , 0x00 },/* �R�}���h���s���� */

	{0x960, WRN_COM,			TYP_WRN | RST_ENABLE,  0x20 | M3STD_COMM_ALM, 0x00 },/* MECHATROLINK�ʐM�x�� */
	{0x962, WRN_COM_FCS,		TYP_WRN | RST_ENABLE,  0x10 | M3STD_COMM_ALM, 0x00 },/* FCS�ُ�x�� */
	{0x963, WRN_COM_GBL,		TYP_WRN | RST_ENABLE,  0x30 | M3STD_COMM_ALM, 0x00 },/* �O���[�o���f�[�^�ʐM�x�� */
	{0x964, WRN_SLV_COM,		TYP_WRN | RST_ENABLE,  0x00 | M3STD_COMM_ALM, 0x00 },/* ���ǃ��j�^�f�[�^�ʐM�x�� */
	{0x96A, WRN_COM_RCV_ASYNC,	TYP_WRN | RST_ENABLE,  0x20 | M3STD_COMM_ALM, 0x00 },/* �l�b�g���[�N��M�x��(�C�x���g�h���u���ʐM) */
	{0x94A, WRN_PRM_NO,			TYP_WRN | RST_ENABLE,  0x90 | M3STD_CMD_ALM , 0x00 },/* �f�[�^�ݒ�x��(���[�U�萔�ԍ�) */
	{0x94B, WRN_DATA_AREA,		TYP_WRN | RST_ENABLE,  0x90 | M3STD_CMD_ALM , 0x00 },/* �f�[�^�ݒ�x��(�f�[�^�͈͊O) */
	{0x94C, WRN_DATA_CALC,		TYP_WRN | RST_ENABLE,  0x90 | M3STD_CMD_ALM , 0x00 },/* �f�[�^�ݒ�x��(���Z�ُ�) */
	{0x94D, WRN_DATA_SIZE,		TYP_WRN | RST_ENABLE,  0x90 | M3STD_CMD_ALM , 0x00 },/* �f�[�^�ݒ�x��(�f�[�^�T�C�Y�ُ�) */
	{0x94E, WRN_DATA_LTMOD,		TYP_WRN | RST_ENABLE,  0xA0 | M3STD_CMD_ALM , 0x00 },/* �f�[�^�ݒ�x��(���b�`���[�h�ُ�) */
	{0x95A, WRN_CMD_CNDTN,		TYP_WRN | RST_ENABLE,  0xA0 | M3STD_CMD_ALM , 0x00 },/* �R�}���h�x��(�����O) */
	{0x95B, WRN_CMD_NOSPT,		TYP_WRN | RST_ENABLE,  0x80 | M3STD_CMD_ALM , 0x00 },/* �R�}���h�x��(���T�|�[�g) */
	{0x95C, WRN_CMD_PRM,		TYP_WRN | RST_ENABLE,  0xA0 | M3STD_CMD_ALM , 0x00 },/* �R�}���h�x��(���[�U�萔�ɂ������O) */
	{0x95D, WRN_CMD_CNFLCT,		TYP_WRN | RST_ENABLE,  0xA0 | M3STD_CMD_ALM , 0x00 },/* �R�}���h�x��(�R�}���h�̊���) */
	{0x95E, WRN_SUBCMD,			TYP_WRN | RST_ENABLE,  0xB0 | M3STD_CMD_ALM , 0x00 },/* �R�}���h�x��(�T�u�R�}���h�s��) */
	{0x95F, WRN_FUNC_CNFLCT,	TYP_WRN | RST_ENABLE,  0x80 | M3STD_CMD_ALM , 0x00 },/* �R�}���h�x��(�|�[�g�̊���) */
	{0x97A, WRN_COM_PHASE,		TYP_WRN | RST_ENABLE,  0xC0 | M3STD_CMD_ALM , 0x00 },/* �t�F�[�Y�ُ� */
	{0x97B, WRN_DATA_CLAMP,		TYP_WRN | RST_ENABLE,  0x10 | M3STD_CMD_ALM , 0x00 },/* �f�[�^�ݒ�x��(�f�[�^�N�����v����) */
	{0x97C, WRN_ACT_COP_STP,	TYP_WRN | RST_ENABLE,  0x00 | M3STD_CMD_ALM , 0x00 },/* ����x��(������~����) */
	{0xFFFF,ALM_NOANS,			TYP_WRN | RST_ENABLE,  0x00 | M3STD_DEV_ALM , 0x00 } /* end of table */
};
#endif

#if 0
/****************************************************************************
 �f�o�C�X����`
****************************************************************************/
const ULONG	null_data[32] 
		= {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000};
const ULONG	VenderIDCode 	= 0x00000000;			/* 01H �x���_�[ID�R�[�h		(��)����d�@			*/
const ULONG	DeviceIDCode 	= 0x02240001;			/* 02H �f�o�C�X�R�[�h		��-�X-MD�V���[�Y		*/
const ULONG	MdiFileVer	 	= 0x00001000;			/* 04H �@���`(MDI)�t�@�C���o�[�W����				*/
const ULONG	ExAddress 		= 0x00000008;			/* 05H �g���A�h���X�ݒ�p(�L������)	(>_<)���ݒ�		*/
const ULONG ProfileType01 	= 0x00000010;			/* 10H �v���t�@�C���^�C�v�P							*/
const ULONG	stdProfileVer	= 0x00000030;			/* 11H �v���t�@�C���o�[�W�����P						*/
const ULONG ProfileType02 	= 0x000000FF;			/* 12H �v���t�@�C���^�C�v�Q							*/
const ULONG	m2ProfileVer	= 0x00000000;			/* 13H �v���t�@�C���o�[�W�����Q						*/
const ULONG	ukProfileType	= 0x000000FF;			/* 14H �v���t�@�C���^�C�v�R							*/
const ULONG	ukProfileVer	= 0x00000000;			/* 14H �v���t�@�C���o�[�W�����R						*/
const ULONG MinTcyc 		= 25000;				/* 16H �`�������ŏ��l								*/
const ULONG	MaxTcyc			= 400000;				/* 17H �`�������ő�l								*/
const ULONG TcycGranularity = 0x00000003;			/* 18H �`����������									*/
const ULONG MinAplcyc 		= 25000;				/* 19H �ʐM�����ŏ��l								*/
const ULONG	MaxAplcyc		= 3200000;				/* 1AH �ʐM�����ő�l								*/
const ULONG TrByteList 		= 0x0000000E;			/* 1BH �`���o�C�g���i���X�g�j						*/
const ULONG	SupComMode		= 0x00000002;			/* 20H �ʐM���[�h�Ή� �T�C�N���b�N �� ���b�Z�[�W	*/
const ULONG	SupMainCmd[8]							/* 30H ���C���R�}���h�Ή����X�g						*/
		= {0x6000E079, 0x36F7001F, 0x00000003, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000};
const ULONG	SupSubCmd[8]							/* 38H �T�u�R�}���h�Ή����X�g						*/
		= {0x60000061, 0x00010000, 0x00000003, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000};
const ULONG	SupComPrm[8]							/* 40H ���ʃp�����[�^�Ή����X�g						*/
		= {0x00001FFE, 0x000003FE, 0x000003FE, 0x000000FE, 0x000FFFFE, 0x00000000, 0x00000000, 0x00000000};



/* �����f�o�C�X���e�[�u�� */
/* (>_<)Y.Oka */
const DEV_INFO_TBL devInfoTbl[] = {
	{0x0000,	RESRVE,	4,		(void *)&null_data				},	/* Resrve														0000 - 0003 */
	{0x0001,	BINARY,	4,		(void *)&VenderIDCode			},	/* �x���_�[ID�R�[�h												0004 - 0007 */
	{0x0002,	BINARY,	4,		(void *)&DeviceIDCode			},	/* �f�o�C�X�R�[�h												0008 - 000B */
	{0x0003,	BINARY,	4,		(void *)&null_data				},	/* �f�o�C�X�o�[�W����											000C - 000F */
	{0x0004,	BINARY,	4,		(void *)&null_data				},	/* �@���`(MDI)�t�@�C���o�[�W����								0010 - 0013 */
	{0x0005,	BINARY,	4,		(void *)&ExAddress				},	/* �g���A�h���X�ݒ�(�L������)									0014 - 0017 */
	{0x0006,	ASCII,	32,		(void *)&null_data				},	/* �V���A���ԍ�													0018 - 0037 */
	{0x000E,	RESRVE,	8,		(void *)&null_data				},	/* Resrve														0038 - 003F */
	{0x0010,	BINARY,	4,		(void *)&ProfileType01			},	/* �v���t�@�C���^�C�v�P�i�v���C�}���j							0040 - 0043 */
	{0x0011,	BINARY,	4,		(void *)&stdProfileVer			},	/* �v���t�@�C���o�[�W�����P�i�v���C�}���j						0044 - 0047 */
	{0x0012,	BINARY,	4,		(void *)&ProfileType02			},	/* �v���t�@�C���^�C�v�Q											0048 - 004B */
	{0x0013,	BINARY,	4,		(void *)&m2ProfileVer			},	/* �v���t�@�C���o�[�W�����Q										004C - 004F */
	{0x0014,	BINARY,	4,		(void *)&ukProfileType			},	/* �v���t�@�C���^�C�v�R											0050 - 0053 */
	{0x0015,	BINARY,	4,		(void *)&ukProfileVer			},	/* �v���t�@�C���o�[�W�����R										0054 - 0057 */
	{0x0016,	BINARY,	4,		(void *)&MinTcyc				},	/* �`�������ŏ��l												0058 - 005B */
	{0x0017,	BINARY,	4,		(void *)&MaxTcyc				},	/* �`�������ő�l												005C - 005F */
	{0x0018,	BINARY,	4,		(void *)&TcycGranularity		},	/* �`���������݁iGRANULARITY�j									0060 - 0063 */
	{0x0019,	BINARY,	4,		(void *)&MinAplcyc				},	/* �ʐM�����ŏ��l												0064 - 0067 */
	{0x001A,	BINARY,	4,		(void *)&MaxAplcyc				},	/* �ʐM�����ő�l												0068 - 006B */
	{0x001B,	BINARY,	4,		(void *)&TrByteList				},	/* �`���o�C�g��													006C - 006F */
	{0x001C,	BINARY,	4,		(void *)&null_data				},	/* �`���o�C�g��(���ݐݒ�l)										0070 - 0073 */
	{0x001D,	BINARY,	4,		(void *)&ProfileType01			},	/* �v���t�@�C���^�C�v�i���ݑI��l�j								0074 - 0077 */
	{0x001E,	RESRVE,	8,		(void *)&null_data				},	/* Resrve														0078 - 007F */
	{0x0020,	BINARY,	4,		(void *)&SupComMode				},	/* �ʐM���[�h�Ή�												0080 - 0083 */
	{0x0021,	RESRVE,	60,		(void *)&null_data				},	/* Resrve														0084 - 00BF */
	{0x0030,	ARRAY,	32,		(void *)&SupMainCmd				},	/* ���C���R�}���h�Ή����X�g										00C0 - 00DF */
	{0x0038,	ARRAY,	32,		(void *)&SupSubCmd				},	/* �T�u�R�}���h�Ή����X�g										00E0 - 00FF */
	{0x0040,	ARRAY,	32,		(void *)&SupComPrm				},	/* ���ʃp�����[�^�Ή����X�g										0100 - 011F */
	{0x0048,	RESRVE,	224,	(void *)&null_data				},	/* Resrve														0120 - 01FF */
	{0x0080,	ASCII,	32,		(void *)&null_data				},	/* ��f�o�C�X���́i���i�`���j									0200 - 021F */
	{0x0081,	RESRVE,	32,		(void *)&null_data				},	/* Resrve														0220 - 023F */
	{0x0090,	ASCII,	32,		(void *)&null_data				},	/* �T�u�f�o�C�X�P���́i���[�^�`���j								0240 - 025F */
	{0x0098,	BINARY,	4,		(void *)&null_data				},	/* �T�u�f�o�C�X�P�o�[�W����										0260 - 0263 */
	{0x0099,	RESRVE,	28,		(void *)&null_data				},	/* Resrve														0264 - 027F */
	{0x00A0,	ASCII,	32,		(void *)&null_data				},	/* �T�u�f�o�C�X�Q���́i�O���G���R�[�_�`���j						0280 - 029F */
	{0x00A8,	BINARY,	4,		(void *)&null_data				},	/* �T�u�f�o�C�X�Q�o�[�W�����i�O���G���R�[�_�\�t�g�o�[�W�����j	02A0 - 02A3 */
	{0x00A9,	RESRVE,	28,		(void *)&null_data				},	/* Resrve														02A4 - 02BF */
	{0x00B0,	ASCII,	32,		(void *)&null_data				},	/* �T�u�f�o�C�X�R����											02C0 - 02DF */
	{0x00B8,	BINARY,	4,		(void *)&null_data				},	/* �T�u�f�o�C�X�R�o�[�W����										02E0 - 02E3 */
	{0x00B9,	RESRVE,	28,		(void *)&null_data				},	/* Resrve														02E4 - 02FF */
	{0x00C0,	ASCII,	32,		(void *)&null_data				},	/* �T�u�f�o�C�X�S���́iSafty OptionUnit�̌`���j					0300 - 031F */
	{0x00C8,	BINARY,	4,		(void *)&null_data				},	/* �T�u�f�o�C�X�S�o�[�W�����iSafty OptionUnit�̿���ް�ޮ݁j		0320 - 0323 */
	{0x00C9,	RESRVE,	28,		(void *)&null_data				},	/* Resrve														0324 - 033F */
	{0x00D0,	ASCII,	32,		(void *)&null_data				},	/* �T�u�f�o�C�X�T���́iFeedback OptinUnit�̌`���j				0340 - 035F */
	{0x00D8,	BINARY,	4,		(void *)&null_data				},	/* �T�u�f�o�C�X�T�o�[�W�����iFeedback OptinUnit�̿���ް�ޮ݁j	0360 - 0363 */
	{0x00D9,	RESRVE,	28,		(void *)&null_data				},	/* Resrve														0364 - 037F */
	{0x00E0,	ASCII,	32,		(void *)&null_data				},	/* �T�u�f�o�C�X�U����											0380 - 039F */
	{0x00E8,	BINARY,	4,		(void *)&null_data				},	/* �T�u�f�o�C�X�U�o�[�W����										03A0 - 03A3 */
	{0x00E9,	RESRVE,	28,		(void *)&null_data				},	/* Resrve														03A4 - 03BF */
	{0x00F0,	RESRVE,	64,		(void *)&null_data				},	/* Resrve														03C0 - 03FF */
	{0x0100,	RESRVE,	0xFC00,	(void *)&null_data				},	/* Resrve														0400 - FFFF */
	{0xFFFF,	0,		0,		(void *)&null_data				}	/* end of table																*/
};
#endif

/***************************************** end of file **********************************************/
