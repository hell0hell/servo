#ifndef __ADV_AUTO_TUNING_H
#define __ADV_AUTO_TUNING_H
/****************************************************************************
Description	: Advanced Auto Tuning
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
----------------------------------------------------------------------------
*****************************************************************************/
#include "Basedef.h"
#include "SystemSetting.h"
#include "FnPrgJog.h"
#include "PowerManager.h"
#include "CheckMotSts.h"
#include "CheckAlarm.h"
#include "RemVibFreq.h"
#include "AlarmManager.h"

/****************************************************************************************************/
/*																									*/
/*		Constant Definition																			*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*	�A�h�o���X�h�I�[�g�`���[�j���O�Q�C���ύX�ݒ�萔��`											*/
/*--------------------------------------------------------------------------------------------------*/
#define		GAIN_DW					0		/* �Q�C���_�E��											*/
#define		GAIN_UP					1		/* �Q�C���A�b�v											*/
#define		GAIN_BAL				2		/* �Q�C���o�����X�P										*/
#define		GAIN_BAL2				3		/* �Q�C���o�����X�Q										*/
#define		GAIN_CAL				4		/* �Q�C���v�Z											*/
#define		GAIN_BAL3				5		/* �Q�C���o�����X�R										*/
#define		GAIN_BAL4				6		/* �Q�C���o�����X�S										*/
/*--------------------------------------------------------------------------------------------------*/
/*	�A�h�o���X�h�I�[�g�`���[�j���O�p�����[�^�萔��`												*/
/*--------------------------------------------------------------------------------------------------*/
#define		RESGMAX					200		/* �w�߉������U���o臒l [%]								*/
#define		STPVIBDET				300		/* ��~���U�����o臒l [%]								*/
#define		STPVIBLMT				5		/* ��~���U�����o���~�b�g[%]							*/
#define		STPTRQFIL				1000	/* ��~���g���N�n�C�p�X�t�B���^���g�� [0.1Hz]			*/
/*--------------------------------------------------------------------------------------------------*/
/*	�A�h�o���X�h�I�[�g�`���[�j���O�G���[�X�e�[�^�X�萔��`											*/
/*--------------------------------------------------------------------------------------------------*/
#define		ADAT_STS_INIT			0x00	/* �C�j�V����/���f										*/
#define		ADAT_STS_DONE			0x01	/* �`���[�j���O����										*/
#define		ADAT_STS_ERR			0x11	/* �`���[�j���O���s										*/
#define		ADAT_STS_NG				0x21	/* ����s��												*/
#define		ADAT_STS_ABORT			0x22	/* �ُ�ɂ�钆�f										*/
#define		ADAT_STS_JERR			0x31	/* �C�i�[�V������s��									*/
#define		ADAT_STS_DRVWAIT		0x41	/* �����ݒ芮�� ���͑҂�								*/
#define		ADAT_STS_JRUN			0x50	/* �C�i�[�V��������s��									*/
#define		ADAT_STS_JEND			0x51	/* �C�i�[�V�����芮��									*/
#define		ADAT_STS_VIBDET			0x60	/* �U�����x��������s��									*/
#define		ADAT_STS_MAXGNADJ		0x70	/* ���E�Q�C���T�����s��									*/
#define		ADAT_STS_OPTGNADJ		0x80	/* ����]���Q�C���T�����s��								*/
//#define		ADAT_STS_NOTCHSET		0x90	/* �m�b�`�t�B���^�����ݒ���s��						*/
#define		ADAT_STS_VIBDISSET		0x90	/* �U���}���ݒ���s��									*/
/*--------------------------------------------------------------------------------------------------*/
#define		ADAT_STS_COINNOWAIT		0x0000	/* COIN�҂��Ȃ��X�e�[�^�X								*/
#define		ADAT_STS_COINERR		0x0100	/* COIN�҂��G���[�X�e�[�^�X								*/
#define		ADAT_STS_COINWAIT		0x0200	/* COIN�҂����X�e�[�^�X									*/
#define		ADAT_STS_COINWARN		0x0300	/* COIN�҂��������X�e�[�^�X								*/
/*--------------------------------------------------------------------------------------------------*/
#define		ADAT_END				99		/* �����I��												*/
#define		ADAT_OK					0		/* ����I��												*/
#define		ADAT_CONTINUE			1		/* ���s��												*/
#define		ADAT_BUSY				2		/* �����p����											*/
#define		ADAT_NG					-1		/* �ُ�I��												*/
#define		ADAT_NOTRDYERR			-2		/* �^�]���������G���[									*/
#define		ADAT_MOSESETERR			-3		/* ���[�h�ݒ�G���[										*/
#define		ADAT_JST_ERR			-4		/* �C�i�[�V������J�n�G���[								*/
#define		ADAT_J_ERR				-5		/* �C�i�[�V������G���[									*/
#define		ADAT_PROGERR			-6		/* �^�]�w�߃G���[										*/
#define		ADAT_QFILERR			-7		/* ���g����̓G���[										*/
#define		ADAT_NTCHNG				-8		/* �m�b�`�ݒ�G���[										*/
#define		ADAT_COINERR			-9		/* COIN���o�̓G���[										*/
#define		ADAT_COMERR				-50		/* ���쒆�̒ʐM�G���[									*/
/*--------------------------------------------------------------------------------------------------*/
/*	�A�h�o���X�h�I�[�g�`���[�j���O�����v����`														*/
/*--------------------------------------------------------------------------------------------------*/
#define		ADAT_SEQ_INIT			0		/* �����ݒ���											*/
#define		ADAT_SEQ_MODESET		1		/* �`���[�j���O���[�h�ݒ���							*/
#define		ADAT_SEQ_JRATSET		2		/* �C�i�[�V���ݒ���									*/
#define		ADAT_SEQ_NORMVIB		3		/* �ʏ�U�����x���T�����								*/
#define		ADAT_SEQ_SEARCHOPTKV	4		/* �œKKv�ATi�Q�C���T�����								*/
#define		ADAT_SEQ_NOTCHSET		5		/* �m�b�`�t�B���^�ݒ���								*/
#define		ADAT_SEQ_SEARCHOPTKP	6		/* �œKKp�Q�C���T�����									*/
#define		ADAT_SEQ_ADJMFC			7		/* ���f���Ǐ]���䒲�����								*/
#define		ADAT_SEQ_ENDDISP		10		/* �I���\�����											*/
#define		ADAT_SEQ_END			8		/* �ŏI�ݒ���											*/
#define		ADAT_SEQ_WAIT			9		/* �I���҂����											*/
#define		ADAT_SEQ_RETURN			11		/* ���[�h�I�����										*/
#define		ADAT_SEQ_NOOPWAIT		12		/* No Operation �\���E�F�C�g							*/
#define		ADAT_SEQ_START			13		/* �������������										*/
#define		ADAT_SEQ_2MASSSET		14		/* MFC(����+�@��U��)�ݒ���							*/
#define		ADAT_SEQ_KPSET			15		/* Kp�ݒ���											*/
/*--------------------------------------------------------------------------------------------------*/
#define		ADAT_NO_OPE				100		/* no operation 										*/
#define		ADAT_CHKKV				101		/* �w�߉����ɂ����S�m�F								*/
#define		ADAT_VIBCOIN			102		/* ���f���Ǐ]����COIN����m�F							*/
/*--------------------------------------------------------------------------------------------------*/
/*	�A�h�o���X�h�I�[�g�`���[�j���O�œK�Q�C���T���V�[�P���X��`										*/
/*--------------------------------------------------------------------------------------------------*/
#define		ADAT_OPTGN_INI			0		/* �œK�Q�C���T�������ݒ���							*/
#define		ADAT_OPTGN_REQ			1		/* �œK�Q�C���T���v�����								*/
#define		ADAT_OPTGN_SEARCH		2		/* �œK�Q�C���T�����									*/
#define		ADAT_OPTGN_WAIT			3		/* �U�������҂����										*/
#define		ADAT_OPTGN_CHKKV_REQ	4		/* �w�߉������S�m�F�v�����								*/
#define		ADAT_OPTGN_CHKKV		5		/* �œK�Q�C���T�����2									*/
#define		ADAT_OPTGN_INIWAIT		6		/* �œK�Q�C���T�������U�������҂�						*/
/*--------------------------------------------------------------------------------------------------*/
/*	�A�h�o���X�h�I�[�g�`���[�j���O�œK�j���Q�C���T���V�[�P���X��`									*/
/*--------------------------------------------------------------------------------------------------*/
#define		ADAT_OPTKP_INI			0		/* �œKkp�T�������ݒ���								*/
#define		ADAT_OPTKP_CHK_REQ		1		/* COIN����m�F�v�����									*/
#define		ADAT_OPTKP_CHK			2		/* COIN����m�F���										*/
#define		ADAT_OPTKP_WAIT			3		/* �U�������҂����										*/
/*--------------------------------------------------------------------------------------------------*/
/*	�A�h�o���X�h�I�[�g�`���[�j���O���f���Ǐ]�����V�[�P���X��`										*/
/*--------------------------------------------------------------------------------------------------*/
#define		ADAT_MFC_INI			0		/* ���f���Ǐ]���������ݒ���							*/
#define		ADAT_MFC_MAXREQ			1		/* Kpm�����v�����										*/
#define		ADAT_MFC_MAXSEARCH		2		/* Kpm�������											*/
#define		ADAT_MFC_WAIT			3		/* �U�������҂����										*/
#define		ADAT_MFC_CHK_REQ		4		/* COIN����m�F�v�����									*/
#define		ADAT_MFC_CHK			5		/* COIN����m�F���										*/
#define		ADAT_MFC_INIWAIT		6		/* ���f���Ǐ]���������U�������҂�						*/
/*--------------------------------------------------------------------------------------------------*/
/*	�A�h�o���X�h�I�[�g�`���[�j���O �X�L�����b�X�e�[�^�X��`											*/
/*--------------------------------------------------------------------------------------------------*/
#define		ADAT_SEAR_NORMALEND		5		/* �T����������I��										*/
#define		ADAT_SEAR_VIBEND		1		/* �U�����o�I��											*/
#define		ADAT_SEAR_STOPVIBEND	2		/* ��~���U�����o�I��									*/
#define		ADAT_SEAR_COINBREAKEND	3		/* COIN���ꌟ�o�I��										*/
#define		ADAT_SEAR_CONTINUE		4		/* �T��������											*/
#define		ADAT_SEAR_ERR			-1		/* �T�������ُ�											*/
#define		ADAT_SEAR_NOVIBDETECT	6		/* �U�������o�I��										*/
#define		ADAT_SEAR_COINOK		7		/*														*/
/*--------------------------------------------------------------------------------------------------*/
/*	�A�h�o���X�h�I�[�g�`���[�j���O �v���O�����i�n�f�X�e�[�^�X��`									*/
/*--------------------------------------------------------------------------------------------------*/
#define		ADAT_PJOG_NOP			0		/* �^�]��~��											*/
#define		ADAT_PJOG_EXEC			1		/* �^�]��												*/
#define		ADAT_PJOG_FIN			2		/* �^�]����												*/
#define		ADAT_PJOG_PLUSEND		3		/* ���]�����w�ߊ���										*/
#define		ADAT_PJOG_MINUSEND		4		/* �t�]�����w�ߊ���										*/
#define		ADAT_PJOG_WAIT			5		/* �^�]�J�n�ҋ@											*/
/*--------------------------------------------------------------------------------------------------*/
/*	�U�����o�����l																					*/
/*--------------------------------------------------------------------------------------------------*/
#define		VIBMFC_DET_FRQ_MIN		10		/* Fn023���̎c���U�����o���g������ 1.0Hz				*/
#define		VIBMFC_DET_FRQ_MAX		1000	/* Fn023���̎c���U�����o���g����� 100.0Hz				*/
/*--------------------------------------------------------------------------------------------------*/
/*	MFC(����+�@��)�p�����[�^�ݒ�																	*/
/*--------------------------------------------------------------------------------------------------*/
#define		VIBMFC_PRM_ACTIVE		0x0011	/* ���̌n�{�@�䃂�f���ݒ�(Pn140)						*/
#define		RIGMFC_PRM_ACTIVE		0xff0f	/* ���̌n���f���ݒ�(Pn140)								*/
/*--------------------------------------------------------------------------------------------------*/
/*	�A�h�o���X�h�I�[�g�`���[�j���O �v���O�����i�n�f�^�]�p�^�[����`									*/
/*																									*/
/*		�p�^�[���ݒ��	�O���O�O�e�����e����														*/
/*							  �b�b�b�b�b�b�b														*/
/*							  �b�b�b�b�b�b�P��ڂ̉^�]�I��											*/
/*							  �b�b�b�b�b�Q��ڂ̉^�]�I��											*/
/*							  �b�b�b�b�P��ځA�Q��ڂ��J��Ԃ�										*/
/*							  �b�b�b�R��ڂ̉^�]�I��												*/
/*							  �b�b�S��ڂ̉^�]�I��													*/
/*							  �b�R��ځA�S��ڂ��J��Ԃ�											*/
/*							  �^�]��~																*/
/*		�^�]�I��	0�F��~																			*/
/*					1�F�����萳�]																	*/
/*					2�F������t�]																	*/
/*					3�F�ʏ푗�萳�]																	*/
/*					4�F�ʏ푗��t�]																	*/
/*					6�F�Ű�����蒆�����萳�]														*/
/*					7�F�Ű�����蒆������t�]														*/
/*					F�F�J��Ԃ��i�J��Ԃ��񐔂́A���قɏ]���j										*/
/*																									*/
/*	���j�^�]�I�����́A�J�n���Ɠ����ʒu�ɂȂ�悤������݂��쐬���邱��								*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
#define		ADAT_PJOG_STOP			0x00000000	/* �^�]�p�^�[���O �^�]��~							*/
#define		ADAT_PJOG_PATTERN1		0x00002121	/* �^�]�p�^�[���P ������A��������(�Q����)			*/
#define		ADAT_PJOG_PATTERN2		0x0000F4F3	/* �^�]�p�^�[���Q �ʏ푗��A��������				*/
#define		ADAT_PJOG_PATTERN3		0x0021F4F3	/* �^�]�p�^�[���R �ʏ푗�� �� ������				*/
#define		ADAT_PJOG_PATTERN5		0x00000006	/* �^�]�p�^�[���T ���蒆������A��������			*/
/*--------------------------------------------------------------------------------------------------*/
#define		VIBOBS_KS				800		/* �U�����o�I�u�U�[�oKs�؂�ւ�Kv�Q�C�� [0.1Hz]			*/
#define		DETVIBLP				3183	/* �����p���U�����o�p���[�p�X�t�B���^(3183us = 50Hz)	*/
#define		DETVIBHP2				3183	/* AAT�U�����o�p�n�C�p�X�t�B���^(3183us = 50Hz)			*/
#define		DETVIBHP3				1591	/* AAT�U�����o�p�n�C�p�X�t�B���^(1591us = 100Hz)		*/
#define		DETVIBHP				320		/* ���U���o�p�n�C�p�X�t�B���^�F0.32[ms]=500[Hz]			*/
/*--------------------------------------------------------------------------------------------------*/
#define		TUNEACCTRQ2				1000	/* �`���[�j���O�����g���N(�V�d�l)	[0.1%]				*/
#define		JRATACCTRQ				100		/* �C�i�[�V�����莞�����g���N(�J�n)	[0.1%]				*/
#define		JRATACCTRQ2				500		/* �C�i�[�V�����莞�����g���N(���蒆)[0.1%]				*/
#define		AATCONSVELTM			150		/* ���葬����	[ms]								*/
#define		JRATE_VFFGN				100		/* �C�i�[�V�����蒆���x�e�e�Q�C��[%]					*/
#define		JRATPJOGACC				15		/* �C�i�[�V�����莞�����x�����l	[ms]					*/
/*--------------------------------------------------------------------------------------------------*/
#define		AATPJOGWAITTM			300		/* �v���O����JOG�^�]�҂�����		[ms]				*/
/*--------------------------------------------------------------------------------------------------*/
#define		ADATVIBWAITTM1			500		/* �`���[�j���O���U�������҂�����	500ms				*/
#define		ADATVIBWAITTM2			300		/* �`���[�j���O���U�������҂�����	300ms				*/
#define		ADATVIBWAITTM3			100		/* �`���[�j���O���U�������҂�����	100ms				*/
#define		ADATSTOPWAITTM			3000	/* �`���[�j���O�����[�^��~�҂�����	3000ms				*/
/*--------------------------------------------------------------------------------------------------*/
#define		ADATCOINWAIT1			8000	/* COIN�G���[���o�҂�����								*/
#define		ADATCOINWAIT2			3000	/* COIN�x�����o�҂�����									*/
#define		ADATDRVWAIT				300		/* ���[�^�^�]�҂�����									*/
/*--------------------------------------------------------------------------------------------------*/
#define		COINOFFLMT				1000	/* �ʒu���ߎ��s���e���Ԃ�1ms�Ƃ���[1us]					*/
#define		STPDETTM				100		/* ��~���ʎ���	100ms			[2ms]					*/
#define		ADAT_LFVIBDET_LVL		300		/* �c���U�����o���x��(COIN����)	300%	[%]				*/
/*--------------------------------------------------------------------------------------------------*/
#define		GNUPTM2					400		/* �Q�C���T���Ԋu(�V�d�l)	400ms	[2ms]				*/
/*--------------------------------------------------------------------------------------------------*/
#define		KVUP_UNIT				20		/* Kv UP��					[0.1Hz]						*/
#define		KV_UNLMT				100		/* Kv �����l				[0.1Hz]						*/
#define		KPUP_UNIT				30		/* Kp UP��					[0.1s-1]					*/
#define		KPDOWN_UNIT				30		/* Kp DOWN��				[0.1s-1]					*/
#define		KP_UNLMT				100		/* Kp �����l				[0.1s-1]					*/
#define		KPM_UNLMT				10		/* Kpm �����l				[0.1s-1]					*/
/*--------------------------------------------------------------------------------------------------*/
#define		ADAT_DTUN_MAX			5		/* �ړ�����������										*/
#define		ADAT_DIST_MAX			99990000	/* �ړ������ő�l									*/
/*--------------------------------------------------------------------------------------------------*/
#define		MINIMUM_LINEAR_MM		5		/* ���j�A���[�^�ŏ��ړ�����[mm]							*/
#define		DEFAULT_STROKE_ROTARY	30		/* ��]�^���[�^�����ړ�����[0.1rev]						*/
#define		DEFAULT_STROKE_LINEAR	900		/* ���j�A���[�^�����ړ�����[0.1mm]						*/
/*--------------------------------------------------------------------------------------------------*/
#define		ADAT_DRV_PLUS			0		/* �^�]�����F��											*/
#define		ADAT_DRV_MINUS			1		/* �^�]�����F�t											*/
/*--------------------------------------------------------------------------------------------------*/
#define		ADAT_JDRV_MAXCNT		9		/* �������[�����g����^�]�ő��						*/
#define		ADAT_JRAT_CHGCNT		5		/* �������[�����g���菉������l�␳��					*/
#define		ADAT_JRAT_CHG_RATE		200		/* �������[�����g���菉������l�␳�P�񓖂�̕␳�l		*/
/*--------------------------------------------------------------------------------------------------*/
#define		ADAT_ERR_ULMT( data )	(((data)*33)>>5)	/* +3.125%									*/
#define		ADAT_ERR_LLMT( data )	(((data)*31)>>5)	/* -3.125%									*/
#define		ADAT_ERR2_ULMT( data )	(((data)*21)>>4)	/* +31.25%									*/
#define		ADAT_ERR2_LLMT( data )	(((data)*11)>>4)	/* -31.25%									*/
/*--------------------------------------------------------------------------------------------------*/
#define		ADAT_GRAT_80( data )	(((data)*205)>>8)	/* 80%										*/
#define		ADAT_GRAT_70( data )	(((data)*180)>>8)	/* 70%										*/
#define		ADAT_GRAT_50( data )	((data)>>1)			/* 50%										*/
#define		ADAT_GRAT_62( data )	(((data)*5)>>3)		/* 62.5%									*/
#define		ADAT_GRAT_150( data )	(((data)*3)>>1)		/* 150%										*/
#define		ADAT_GRAT_94( data )	(((data)*15)>>4)	/* 93.75%									*/
#define		ADAT_GRAT_106( data )	(((data)*17)>>4)	/* 106.25%									*/
#define		ADAT_GRAT_113( data )	(((data)*9)>>3)		/* 112.5%									*/
/*--------------------------------------------------------------------------------------------------*/
#define		ADAT_STS_NF_SETOK		0		/* �m�b�`�t�B���^�ݒ芮��								*/
#define		ADAT_STS_NF_SETERR		-1		/* �m�b�`�t�B���^�ݒ�ُ�								*/
#define		ADAT_STS_NF_ANAERR		-2		/* ���g����ُ͈�										*/
/*--------------------------------------------------------------------------------------------------*/
#define		ADAT_LVL_50				0x0A000200	/* �Q�C���␳�F 50%									*/
#define		ADAT_LVL_70				0x0A0002CD	/* �Q�C���␳�F 70%									*/
#define		ADAT_LVL_80				0x0A000333	/* �Q�C���␳�F 80%									*/
#define		ADAT_LVL_100			0x0A000400	/* �Q�C���␳�F100%									*/
/*--------------------------------------------------------------------------------------------------*/
#define		ADATMFC_ONE_MASS		0x01	/* ���̌n���f��											*/
#define		ADATMFC_TWO_MASS1		0x02	/* �Q�����n�n���f���P									*/
#define		ADATMFC_TWO_MASS2		0x03	/* �Q�����n�n���f���Q									*/
#define		ADATMFC_ONE_BASE		0x04	/* ���̌n�{�@�䃂�f��									*/
/*--------------------------------------------------------------------------------------------------*/
#define		ADAT_MODE_MIN			0		/* ���[�h����											*/
#define		ADAT_MODE_MAX			1		/* ���[�h���											*/
/*--------------------------------------------------------------------------------------------------*/
#define		ADAT_LEVEL_MIN			1		/* ���x������											*/
#define		ADAT_LEVEL_MAX			3		/* ���x�����											*/
#define		ADAT_LEVEL_NOR			1		/* �������x�����W��										*/
#define		ADAT_LEVEL_POS			2		/* �������x�����ʒu���ߐ�p								*/
#define		ADAT_LEVEL_POS_ST		3		/* �������x�����ʒu���ߐ�p(�I�[�o�[�V���[�g�d��)		*/
/*--------------------------------------------------------------------------------------------------*/
#define		ADAT_SCROLL_MIN			0		/* �����w��AAT�ݒ��ʃX�N���[������					*/
#define		ADAT_SCROLL_MAX			3		/* �����w��AAT�ݒ��ʃX�N���[�����					*/
/*--------------------------------------------------------------------------------------------------*/
#define		ADAT_REFIN_SCROLL_MIN	1		/* �w�ߓ��͌^AAT�ݒ��ʃX�N���[������					*/
#define		ADAT_REFIN_SCROLL_MAX	2		/* �w�ߓ��͌^AAT�ݒ��ʃX�N���[�����					*/
/*--------------------------------------------------------------------------------------------------*/
#define		ADAT_JRAT_OK			0		/* �������[�����g����F���퓮�쒆						*/
#define		ADAT_JRAT_ERR1			1		/* �������[�����g����F����J�n���s						*/
#define		ADAT_JRAT_ERR2			2		/* �������[�����g����F����s��							*/
#define		ADAT_JRAT_ERR3			3		/* �������[�����g����F����g���U�����o					*/
#define		ADAT_JRAT_ERR4			4		/* �������[�����g����F�g���N�����G���[					*/
#define		ADAT_JRAT_ERR5			5		/* �������[�����g����F�o����A�΍��N���A				*/
/*--------------------------------------------------------------------------------------------------*/

/****************************************************************************************************/
/*																									*/
/*		Struct Definition																			*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		�A�h�o���X�h�I�[�g�`���[�j���O���샌�W�X�^�\����											*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct ADATREG{			/* ���샌�W�X�^														*/
	USHORT	AdatMode;						/* 0x2090 : �A�h�o���X�gAT���[�h						*/
	USHORT	AdatLevel;						/* 0x2091 : �A�h�o���X�gAT���x��						*/
	DWORDX	AdatMovep;						/* 0x2092 : �A�h�o���X�gAT�ړ�����						*/
	USHORT	AdatState;						/* 0x2094 : �A�h�o���X�gAT�����m�F						*/
	USHORT	AdatFilType;					/* 0x2095 : �A�h�o���X�gAT�t�B���^�^�C�v				*/
}ADATREG;

/*--------------------------------------------------------------------------------------------------*/
/*		�A�h�o���X�h�I�[�g�`���[�j���O�f�[�^�\����													*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct ADATV {
/*--------------------------------------------------------------------------------------------------*/
	struct {					/* �r�b�g�M��														*/
		UINT	RefInType		:1;			/* �w�ߓ��͌^											*/
		UINT	UsedAllNtFil	:1;			/* �S�m�b�`�t�B���^�g�p									*/
		UINT	UsedAResCtrl	:1;			/* �`�^���U�g�p											*/
		UINT	JratCalRun		:1;			/* �������[�����g���蒆									*/
		UINT	GainTuneRun		:1;			/* �Q�C��������											*/
		UINT	FftReq			:1;			/* ���g����͗v��										*/
		UINT	VibDetect		:1;			/* �U�����o												*/
		UINT	CoinBreak		:1;			/* COIN����												*/
		UINT	VibDetectatStop	:1;			/* ��~���U�����o										*/
		UINT	VibDetectinCycle:1;			/* ����^�]�ł̐U�����o									*/
		UINT	CoinLatch		:1;			/* COIN���b�`�t���O										*/
		UINT	LastCoinLatch	:1;			/* �O��COIN���b�`�t���O									*/
		UINT	CoinIgnore		:1;			/* COIN����												*/
		UINT	OrgIPSpdControl	:1;			/* I-P����ł̓���										*/
		UINT	DrvPatternSet	:1;			/* �^�]�p�^�[���ݒ�										*/
		UINT	DrvErrDetect	:1;			/* �^�]�G���[���o										*/
		UINT	CoinWaitStart	:1;			/* COIN�҂������J�n�t���O								*/
		UINT	Used2MassMfc	:1;			/* MFC(����+�@�䃂�f��)�ݒ�ς݃t���O					*/
		UINT	Mfc2MassOK		:1;			/* MFC(����+�@�䃂�f��)�ŏI�ݒ�ς��׸�					*/
		UINT	Used2MassOld	:1;			/* AAT�O��MFC�ݒ�ς݃t���O								*/
		UINT	MfcLastSet		:1;			/* MFC�ŏI�����ςݍς݃t���O							*/
		UINT	OvsErr			:1;			/* �I�[�o�[�V���[�g�G���[�t���O							*/
		UINT	StopDetectMode2	:1;			/* �w�ߓ���AAT��~���胂�[�h�Q�t���O					*/
											/* FALSE�FCOIN�Œ�~����@TURE�F�w��=0�Œ�~����		*/
		UINT	KvTuneWait 		:1;			/* Kv�Q�C���`���[�j���O��~�t���O						*/
	} f;
/*--------------------------------------------------------------------------------------------------*/
//	USHORT	TunStartMFCModel;				/* AAT�����J�n����MFC���J���f��							*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	ErrState;						/* �G���[�X�e�[�^�X										*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	ModeSetPtr;						/* ���[�h�ݒ�F�V�[�P���X�|�C���^						*/
	UCHAR	ModeMin;						/* ���[�h�ݒ�F���[�h�I������							*/
	UCHAR	ModeMax;						/* ���[�h�ݒ�F���[�h�I�����							*/
	UCHAR	ModeSetMin;						/* ���[�h�ݒ�F���[�h�ݒ艺��							*/
	UCHAR	ModeSetMax;						/* ���[�h�ݒ�F���[�h�ݒ���							*/
	USHORT	TunModeMax;						/* ���[�h�ݒ�F�������[�h�ő�l							*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	JratSetPtr;						/* �������[�����g����F�V�[�P���X�|�C���^				*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	VibDetPtr;						/* �U�����o�F�V�[�P���X�|�C���^							*/
	SHORT	VibDetStat;						/* �U�����o�F�X�e�[�^�X									*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	KvTunePtr;						/* Kv�����F�V�[�P���X�|�C���^							*/
	USHORT	MaxKv;							/* Kv�����F�ő�Kv�l										*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	NextSeqPtr;						/* ���̃V�[�P���X�|�C���^								*/
	USHORT	NextSubSeqPtr;					/* ���̃T�u�V�[�P���X�|�C���^							*/
	USHORT	Set2MassMfcSequence;			/* �Q����MFC��ݒ肵���V�[�P���X�|�C���^				*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	KpSetPtr;						/* Kp�ݒ�F�V�[�P���X�|�C���^							*/
	USHORT	MaxKp;							/* Kp�ݒ�F�ő�Kp�l										*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	KpTunePtr;						/* Kp�����F�V�[�P���X�|�C���^							*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	MaxKpm;							/* Kpm�ݒ�F�ő�Kpm�l									*/
	USHORT	MfcTunePtr;						/* ���f���Ǐ]���䒲���F�V�[�P���X�|�C���^				*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	NotchSetPtr;					/* �m�b�`�t�B���^�ݒ�F�V�[�P���X�|�C���^				*/
	SHORT	NotchSetStat;					/* �m�b�`�t�B���^�ݒ�F�m�b�`�ݒ�X�e�[�^�X				*/
	USHORT	VibMaxKv;						/* �m�b�`�t�B���^�ݒ�F�m�b�`���ݒ莞�̌��E�Q�C��		*/
	USHORT	VibMaxKvAresOFF;				/* �`�^���U�ݒ�F�`�^���U���ݒ莞�̌��E�Q�C��			*/
	USHORT	MaxKvbk;						/* �m�b�`�t�B���^�ݒ�F�U��Kv�l							*/
	SHORT	FftStat;						/* �m�b�`�t�B���^�ݒ�F���g����̓X�e�[�^�X				*/
	UCHAR	ANotchSetStat;					/* �m�b�`�t�B���^�ݒ�F�����m�b�`�ݒ�X�e�[�^�X			*/
	UCHAR	ANotchSetStatBk;				/* �m�b�`�t�B���^�ݒ�F�����m�b�`�ݒ�ۑ��X�e�[�^�X		*/
	UCHAR	AResSetStat;					/* �m�b�`�t�B���^�ݒ�F�����`�^�ݒ�X�e�[�^�X			*/
	UCHAR	AResSetStatBk;					/* �m�b�`�t�B���^�ݒ�F�����`�^�ݒ�ۑ��X�e�[�^�X		*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	ScanCRequest;					/* �X�L����C�ւ̗v���f�[�^								*/
	SHORT	ScanCStat;						/* �X�L����C����̉����X�e�[�^�X						*/
/*--------------------------------------------------------------------------------------------------*/
	ULONG	PrgJogPatternReq;				/* �^�]�w�߁F�v���p�^�[��								*/
	ULONG	PrgJogPattern;					/* �^�]�w�߁F��̃p�^�[��								*/
	ULONG	PrgJogActPattern;				/* �^�]�w�߁F���݂̃p�^�[��								*/
	USHORT	PrgJogPatternCnt;				/* �^�]�w�߁F�p�^�[���C���f�b�N�X						*/
	SHORT	PrgJogStat;						/* �^�]�w�߁F�^�]�X�e�[�^�X								*/
	USHORT	PrgJogPtr;						/* �^�]�w�߁F�V�[�P���X�|�C���^							*/
	USHORT	PrgJogTimes;					/* �^�]�w�߁F�^�]�J��Ԃ���							*/
	USHORT	PrgJogDrvCnt;					/* �^�]�w�߁F�^�]��									*/
	ULONG	PrgJogDrvCntr;					/* �^�]�w�߁F�^�]�������Ԍ��o�p�J�E���^					*/
	ULONG	PrgJogWaitCntr;					/* �^�]�w�߁F�^�]��~���Ԑ����p�J�E���^					*/
	ULONG	PrgJogMovStartCntr;				/* �^�]�w�߁F�ړ��J�n�҂��J�E���^						*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	TrqRefatStop;					/* ��~���ʏ�g���N�w�ߒl								*/
	LONG	MaxTrqRefatStop;				/* ��~���ʏ�g���N�ő�l								*/
	LONG	MinTrqRefatStop;				/* ��~���ʏ�g���N�ŏ��l								*/
	LONG	VibSpeed;						/* �ʏ�^�]�U�����x��									*/
/*--------------------------------------------------------------------------------------------------*/
	ULONG	CoinWaitStat;					/* COIN���o�F�X�e�[�^�X									*/
	ULONG	CoinWaitCntr;					/* COIN���o�FCOIN���ꎞ�Ԍ��o�p�J�E���^					*/
	ULONG	CoinOnCntr;						/* COIN���o�FCOIN�I�����Ԍ��o�p�J�E���^					*/
/*--------------------------------------------------------------------------------------------------*/
	ULONG	GainUpCntr;						/* �Q�C�������F�Q�C���A�b�v��̐U���L�����o�p�J�E���^	*/
	ULONG	KpGainUpCntr;					/* �Q�C�������F�Q�C���A�b�v��̐U���L�����o�p�J�E���^	*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	KTrqHpFil;						/* �g���N�n�C�p�X�t�B���^���萔���Z�l					*/
	LONG	KTrqHpftmp;						/* �g���N�n�C�p�X�t�B���^�o�b�t�@						*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	MinDistance;					/* �ŏ��ړ�����											*/
	LONG	DefaultDistance;				/* �����ړ�����											*/
	LONG	MoveDistance;					/* �ړ�����		[�w�ߒP��]								*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	PositioningTimes;				/* �ʒu���߉�											*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	JratDrvCnt;						/* �������[�����g����F�^�]��							*/
	SHORT	LastEstimatJrat;				/* �������[�����g����F�����				[%]			*/
	USHORT	Jrate;							/* �������[�����g����F�Q�C�������g�p�����	[%]			*/
	USHORT	JratChangeTimes;				/* �������[�����g����F��������l�␳��				*/
	ULONG	JratVibWaitCntr;				/* �������[�����g����F�����U���������Ԍ��o�p�J�E���^	*/
	ULONG	JratVibWaitCntr2;				/* �������[�����g����F�U���������Ԍ��o�p�J�E���^		*/
	USHORT	JratErr;						/* �������[�����g����F�G���[���						*/
/*--------------------------------------------------------------------------------------------------*/
	ULONG	KvTuneCntr;						/* Kv�����F�V�[�P���X�����Ԍv���J�E���^					*/
	ULONG	KvTuneWaitCntr;					/* Kv�����F�V�[�P���X�����Ԍv���J�E���^					*/
	ULONG	NotchSetCntr;					/* �m�b�`�t�B���^�ݒ�F�V�[�P���X�����Ԍv���J�E���^		*/
	ULONG	KpTuneCntr;						/* Kp�����F�V�[�P���X�����Ԍv���J�E���^					*/
	ULONG	MfcTuneCntr;					/* ���f���Ǐ]���䒲���F�V�[�P���X�����Ԍv���J�E���^		*/
	ULONG	KpSetCntr;						/* Kp�ݒ�F�V�[�P���X�����Ԍv���J�E���^					*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	frq_buf;						/* ���U�E�����U���g���o�b�t�@							*/
	USHORT	Kpm_buf;						/* Kpm�o�b�t�@											*/
	USHORT	Kv_buf;							/* Kv�o�b�t�@											*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	settling_max;					/* ���莞�Ԃ̍ő�l										*/
	LONG	settling_buf;					/* ���莞�Ԃ̍ő�l�o�b�t�@								*/
/*--------------------------------------------------------------------------------------------------*/
	BOOL	ScanCReqFlg;
	USHORT	ScanCReqCmd;
/*--------------------------------------------------------------------------------------------------*/
} ADATV;

/*--------------------------------------------------------------------------------------------------*/
/*		�A�h�o���X�h�I�[�g�`���[�j���O �f�[�^�|�C���^												*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct ADATP{
	FUN_CMN_CONTROL 	*FnCmnCtrl;
	UNI_PRM_STR 		*UniPrms;
	MENCV 				*MencV;
	BASE_CTRL_HNDL 		*BaseCtrlData;
	REG_MANAGER_HANDLE 	*RegManager;
//	SEQ_CTRL_HNDL 		*SeqCtrlData;
	SEQ_MOT_STS 		*SeqMotSts;
	SEQ_CTRL_OUT 		*SeqCtrlOut;
	ALMMAN_HANDLE 		*AlmManager;
	SETTLINGTIME 		*SettlingTime;
	AUTONOTCH   		*ANotch;
//	DET_HWBB_STS 		*DetHwbbSts;
	REMVIBFREQ   		*RemVibFreq;
	FFTANALYZE			*FftAna;
	ASICS 				*SvAsicRegs;
	MOTSPDMAFIL  	 	*MotSpdMafil;
	DETVIB 				*DetVib;
	SERVO_CONTROL_IF 	*SvControlIf;
} ADATP;

/*--------------------------------------------------------------------------------------------------*/
/*		�A�h�o���X�h�I�[�g�`���[�j���O�\����														*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct ADATHNDL {
	ADATREG AdatReg;
	ADATV	AdatV;
	ADATP	AdatP;
	PJOGV	OrgPJogV;						/* �v���O����JOG�p�ϐ�									*/
}ADATHNDL;

/****************************************************************************************************/
/*																									*/
/*		Function Prototype Definition																*/
/*																									*/
/****************************************************************************************************/
void	CpxAdvancedAutoTuning( ADATHNDL	*AdatHndl, FUN_CMN_CONTROL *FnCmnCtrl );
void 	AdatSetStrPtr( void *Axis );
LONG 	AdatPcalDefautDistance( ADATP *AdatP, LONG AxisMotType, BOOL LowSpdMotor ); /* <S145> */
#endif //__ADV_AUTO_TUNING_H
/***************************************** end of file **********************************************/
