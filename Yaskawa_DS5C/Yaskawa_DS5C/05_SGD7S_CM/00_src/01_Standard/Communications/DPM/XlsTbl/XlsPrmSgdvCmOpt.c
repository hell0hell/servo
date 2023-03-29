/****************************************************************************************************/
/*																									*/
/*																									*/
/*		XlsPrmSgdvCmOpt.c : �p�����[�^��`�\�[�X�t�@�C��											*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1. �p�����[�^��`																			*/
/*																									*/
/*		2. �p�����[�^�Ǘ��e�[�u��																	*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		2013.01.11 : Made by Excel Macro															*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "XlsPrmCmOpt.h"





/****************************************************************************************************/
/*																									*/
/*		�p�����[�^�ϐ���`																			*/
/*																									*/
/****************************************************************************************************/
#pragma ghs startzda
OPPRMDAT	OpPrm;
#pragma ghs endzda





/****************************************************************************************************/
/*																									*/
/*		�p�����[�^��`																				*/
/*																									*/
/****************************************************************************************************/
/****************************************************************************************************/
/*		Pn800 : �ʐM����@�\�I��																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comCtrl = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASENBL,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0290,
/* �����֐��|�C���^				*/	pcal_comCtrl,
/* RAM�A�h���X					*/	&(OpPrm.comCtrl),
/* �����l						*/	(USHORT)(	0x0000),
/* ����l						*/	(USHORT)(	0x0F73),
};



/****************************************************************************************************/
/*		Pn801 : �\�t�g���~�b�g�@�\�I��																*/
/****************************************************************************************************/
const	PRMDEF	pndef_sLimitSw = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASENBL,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0292,
/* �����֐��|�C���^				*/	pcal_sLimitSw,
/* RAM�A�h���X					*/	&(OpPrm.sLimitSw),
/* �����l						*/	(USHORT)(	0x0000),
/* ����l						*/	(USHORT)(	0x0103),
};



/****************************************************************************************************/
/*		Pn802 : �R�}���h����ݒ�																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_cmdCtrlSw = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASENBL,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0294,
/* �����֐��|�C���^				*/	pcal_cmdCtrlSw,
/* RAM�A�h���X					*/	&(OpPrm.cmdCtrlSw),
/* �����l						*/	(USHORT)(	0x0000),
/* ����l						*/	(USHORT)(	0x1111),
};



/****************************************************************************************************/
/*		Pn803 : ���_�ʒu�͈� [[ref]]																*/
/****************************************************************************************************/
const	PRMDEF	pndef_zpWidth = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0296,
/* �����֐��|�C���^				*/	pcal_zpWidth,
/* RAM�A�h���X					*/	&(OpPrm.zpWidth),
/* �����l						*/	(USHORT)(	0),
/* ����l						*/	(USHORT)(	250),
};



/****************************************************************************************************/
/*		Pn804 : �����\�t�g���~�b�g�l L [[ref]]														*/
/****************************************************************************************************/
const	PRMDEF	pndef_p_sLimitL = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_W_SIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0298,
/* �����֐��|�C���^				*/	pcal_p_sLimitL,
/* RAM�A�h���X					*/	&(OpPrm.p_sLimitL),
/* �����l						*/	(USHORT)(	0x0001),
/* ����l						*/	(USHORT)(	0xFFFF),
};



/****************************************************************************************************/
/*		Pn805 : �����\�t�g���~�b�g�l H [[ref]]														*/
/****************************************************************************************************/
const	PRMDEF	pndef_p_sLimitH = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_W_SIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x029A,
/* �����֐��|�C���^				*/	pcal_p_sLimitL,
/* RAM�A�h���X					*/	&(OpPrm.p_sLimitH),
/* �����l						*/	(USHORT)(	0xC000),
/* ����l						*/	(USHORT)(	0x3FFF),
};



/****************************************************************************************************/
/*		Pn806 : �����\�t�g���~�b�g�l L [[ref]]														*/
/****************************************************************************************************/
const	PRMDEF	pndef_n_sLimitL = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_W_SIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x029C,
/* �����֐��|�C���^				*/	pcal_n_sLimitL,
/* RAM�A�h���X					*/	&(OpPrm.n_sLimitL),
/* �����l						*/	(USHORT)(	0x0001),
/* ����l						*/	(USHORT)(	0xFFFF),
};



/****************************************************************************************************/
/*		Pn807 : �����\�t�g���~�b�g�l H [[ref]]														*/
/****************************************************************************************************/
const	PRMDEF	pndef_n_sLimitH = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_W_SIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x029E,
/* �����֐��|�C���^				*/	pcal_n_sLimitL,
/* RAM�A�h���X					*/	&(OpPrm.n_sLimitH),
/* �����l						*/	(USHORT)(	0xC000),
/* ����l						*/	(USHORT)(	0x3FFF),
};



/****************************************************************************************************/
/*		Pn808 : �A�u�\�o�f���_�ʒu�I�t�Z�b�g L [[ref]]												*/
/****************************************************************************************************/
const	PRMDEF	pndef_absOffsetL = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_W_SIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x02A0,
/* �����֐��|�C���^				*/	pcal_absOffsetL,
/* RAM�A�h���X					*/	&(OpPrm.absOffsetL),
/* �����l						*/	(USHORT)(	0x0001),
/* ����l						*/	(USHORT)(	0xFFFF),
};



/****************************************************************************************************/
/*		Pn809 : �A�u�\�o�f���_�ʒu�I�t�Z�b�g H [[ref]]												*/
/****************************************************************************************************/
const	PRMDEF	pndef_absOffsetH = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_W_SIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x02A2,
/* �����֐��|�C���^				*/	pcal_absOffsetL,
/* RAM�A�h���X					*/	&(OpPrm.absOffsetH),
/* �����l						*/	(USHORT)(	0xC000),
/* ����l						*/	(USHORT)(	0x3FFF),
};



/****************************************************************************************************/
/*		Pn80A : �P�i�ڒ��������萔 [[1000ref/s^s]]													*/
/****************************************************************************************************/
const	PRMDEF	pndef_accRate1 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x02A4,
/* �����֐��|�C���^				*/	pcal_accRate1,
/* RAM�A�h���X					*/	&(OpPrm.accRate1),
/* �����l						*/	(USHORT)(	1),
/* ����l						*/	(USHORT)(	65535),
};



/****************************************************************************************************/
/*		Pn80B : �Q�i�ڒ��������萔 [[1000ref/s^s]]													*/
/****************************************************************************************************/
const	PRMDEF	pndef_accRate2 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x02A6,
/* �����֐��|�C���^				*/	pcal_accRate2,
/* RAM�A�h���X					*/	&(OpPrm.accRate2),
/* �����l						*/	(USHORT)(	1),
/* ����l						*/	(USHORT)(	65535),
};



/****************************************************************************************************/
/*		Pn80C : �����萔�ؑւ����x [[100ref/s]]														*/
/****************************************************************************************************/
const	PRMDEF	pndef_accChgSpd = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x02A8,
/* �����֐��|�C���^				*/	pcal_accChgSpd,
/* RAM�A�h���X					*/	&(OpPrm.accChgSpd),
/* �����l						*/	(USHORT)(	0),
/* ����l						*/	(USHORT)(	65535),
};



/****************************************************************************************************/
/*		Pn80D : �P�i�ڒ��������萔 [[1000ref/s^s]]													*/
/****************************************************************************************************/
const	PRMDEF	pndef_decRate1 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x02AA,
/* �����֐��|�C���^				*/	pcal_decRate1,
/* RAM�A�h���X					*/	&(OpPrm.decRate1),
/* �����l						*/	(USHORT)(	1),
/* ����l						*/	(USHORT)(	65535),
};



/****************************************************************************************************/
/*		Pn80E : �Q�i�ڒ��������萔 [[1000ref/s^s]]													*/
/****************************************************************************************************/
const	PRMDEF	pndef_decRate2 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x02AC,
/* �����֐��|�C���^				*/	pcal_decRate2,
/* RAM�A�h���X					*/	&(OpPrm.decRate2),
/* �����l						*/	(USHORT)(	1),
/* ����l						*/	(USHORT)(	65535),
};



/****************************************************************************************************/
/*		Pn80F : �����萔�ؑւ����x [[100ref/s]]														*/
/****************************************************************************************************/
const	PRMDEF	pndef_decChgSpd = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x02AE,
/* �����֐��|�C���^				*/	pcal_decChgSpd,
/* RAM�A�h���X					*/	&(OpPrm.decChgSpd),
/* �����l						*/	(USHORT)(	0),
/* ����l						*/	(USHORT)(	65535),
};



/****************************************************************************************************/
/*		Pn810 : �w���֐��������o�C�A�X [[100ref/s]]													*/
/****************************************************************************************************/
const	PRMDEF	pndef_filExpBias = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x02B0,
/* �����֐��|�C���^				*/	pcal_filExpBias,
/* RAM�A�h���X					*/	&(OpPrm.filExpBias),
/* �����l						*/	(USHORT)(	0),
/* ����l						*/	(USHORT)(	65535),
};



/****************************************************************************************************/
/*		Pn811 : �w���֐����������萔 [[0.1ms]]														*/
/****************************************************************************************************/
const	PRMDEF	pndef_filExpTb = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	1,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x02B2,
/* �����֐��|�C���^				*/	pcal_filExpTb,
/* RAM�A�h���X					*/	&(OpPrm.filExpTb),
/* �����l						*/	(USHORT)(0),
/* ����l						*/	(USHORT)(5100),
};



/****************************************************************************************************/
/*		Pn812 : �ړ����ώ��� [[0.1ms]]																*/
/****************************************************************************************************/
const	PRMDEF	pndef_filMaTb = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	1,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x02B4,
/* �����֐��|�C���^				*/	pcal_filMaTb,
/* RAM�A�h���X					*/	&(OpPrm.filMaTb),
/* �����l						*/	(USHORT)(0),
/* ����l						*/	(USHORT)(5100),
};



/****************************************************************************************************/
/*		Pn813 : �\��																				*/
/****************************************************************************************************/
const	PRMDEF	pndef_resrv2 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x02B6,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.resrv2),
/* �����l						*/	(USHORT)(	0),
/* ����l						*/	(USHORT)(	65535),
};



/****************************************************************************************************/
/*		Pn814 : �O���ʒu���ߍŏI���s���� L [[ref]]													*/
/****************************************************************************************************/
const	PRMDEF	pndef_exDistL = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_W_SIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x02B8,
/* �����֐��|�C���^				*/	pcal_exDistL,
/* RAM�A�h���X					*/	&(OpPrm.exDistL),
/* �����l						*/	(USHORT)(	0x0001),
/* ����l						*/	(USHORT)(	0xFFFF),
};



/****************************************************************************************************/
/*		Pn815 : �O���ʒu���ߍŏI���s���� H [[ref]]													*/
/****************************************************************************************************/
const	PRMDEF	pndef_exDistH = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_W_SIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x02BA,
/* �����֐��|�C���^				*/	pcal_exDistL,
/* RAM�A�h���X					*/	&(OpPrm.exDistH),
/* �����l						*/	(USHORT)(	0xC000),
/* ����l						*/	(USHORT)(	0x3FFF),
};



/****************************************************************************************************/
/*		Pn816 : ���_���A���[�h�ݒ�																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_zretDir = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASENBL,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x02BC,
/* �����֐��|�C���^				*/	pcal_zretDir,
/* RAM�A�h���X					*/	&(OpPrm.zretDir),
/* �����l						*/	(USHORT)(	0x0),
/* ����l						*/	(USHORT)(	0x1),
};



/****************************************************************************************************/
/*		Pn817 : ���_���A�A�v���[�`���x�P [[100ref/s]]												*/
/****************************************************************************************************/
const	PRMDEF	pndef_zretSpd1 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x02BE,
/* �����֐��|�C���^				*/	pcal_zretSpd1,
/* RAM�A�h���X					*/	&(OpPrm.zretSpd1),
/* �����l						*/	(USHORT)(	0),
/* ����l						*/	(USHORT)(	65535),
};



/****************************************************************************************************/
/*		Pn818 : ���_���A�A�v���[�`���x�Q [[100ref/s]]												*/
/****************************************************************************************************/
const	PRMDEF	pndef_zretSpd2 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x02C0,
/* �����֐��|�C���^				*/	pcal_zretSpd2,
/* RAM�A�h���X					*/	&(OpPrm.zretSpd2),
/* �����l						*/	(USHORT)(	0),
/* ����l						*/	(USHORT)(	65535),
};



/****************************************************************************************************/
/*		Pn819 : ���_���A�ŏI���s���� L [[ref]]														*/
/****************************************************************************************************/
const	PRMDEF	pndef_zretDistL = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_W_SIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x02C2,
/* �����֐��|�C���^				*/	pcal_zretDistL,
/* RAM�A�h���X					*/	&(OpPrm.zretDistL),
/* �����l						*/	(USHORT)(	0x0001),
/* ����l						*/	(USHORT)(	0xFFFF),
};



/****************************************************************************************************/
/*		Pn81A : ���_���A�ŏI���s���� H [[ref]]														*/
/****************************************************************************************************/
const	PRMDEF	pndef_zretDistH = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_W_SIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x02C4,
/* �����֐��|�C���^				*/	pcal_zretDistL,
/* RAM�A�h���X					*/	&(OpPrm.zretDistH),
/* �����l						*/	(USHORT)(	0xC000),
/* ����l						*/	(USHORT)(	0x3FFF),
};



/****************************************************************************************************/
/*		Pn81B : �\��																				*/
/****************************************************************************************************/
const	PRMDEF	pndef_resrv3 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x02C6,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.resrv3),
/* �����l						*/	(USHORT)(	0),
/* ����l						*/	(USHORT)(	65535),
};



/****************************************************************************************************/
/*		Pn81C : �\��																				*/
/****************************************************************************************************/
const	PRMDEF	pndef_resrv4 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x02C8,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.resrv4),
/* �����l						*/	(USHORT)(	0),
/* ����l						*/	(USHORT)(	65535),
};



/****************************************************************************************************/
/*		Pn81D : �\��																				*/
/****************************************************************************************************/
const	PRMDEF	pndef_resrv5 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x02CA,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.resrv5),
/* �����l						*/	(USHORT)(	0),
/* ����l						*/	(USHORT)(	65535),
};



/****************************************************************************************************/
/*		Pn81E : ���͐M�����j�^�I��																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_ioMonSel = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASENBL,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x02CC,
/* �����֐��|�C���^				*/	pcal_ioMonSel,
/* RAM�A�h���X					*/	&(OpPrm.ioMonSel),
/* �����l						*/	(USHORT)(	0),
/* ����l						*/	(USHORT)(	0x7777),
};



/****************************************************************************************************/
/*		Pn81F : �R�}���h�f�[�^���t�I��																*/
/****************************************************************************************************/
const	PRMDEF	pndef_optBitSw = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASENBL,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x02CE,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.optBitSw),
/* �����l						*/	(USHORT)(	0x0),
/* ����l						*/	(USHORT)(	0x1111),
};



/****************************************************************************************************/
/*		Pn820 : �������b�`�\�̈� L [[ref]]														*/
/****************************************************************************************************/
const	PRMDEF	pndef_p_ltLimitL = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_W_SIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x02D0,
/* �����֐��|�C���^				*/	pcal_p_ltLimitL,
/* RAM�A�h���X					*/	&(OpPrm.p_ltLimitL),
/* �����l						*/	(USHORT)(	0x0000),
/* ����l						*/	(USHORT)(	0xFFFF),
};



/****************************************************************************************************/
/*		Pn821 : �������b�`�\�̈� H [[ref]]														*/
/****************************************************************************************************/
const	PRMDEF	pndef_p_ltLimitH = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_W_SIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x02D2,
/* �����֐��|�C���^				*/	pcal_p_ltLimitL,
/* RAM�A�h���X					*/	&(OpPrm.p_ltLimitH),
/* �����l						*/	(USHORT)(	0x8000),
/* ����l						*/	(USHORT)(	0x7FFF),
};



/****************************************************************************************************/
/*		Pn822 : �t�����b�`�\�̈� L [[ref]]														*/
/****************************************************************************************************/
const	PRMDEF	pndef_n_ltLimitL = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_W_SIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x02D4,
/* �����֐��|�C���^				*/	pcal_n_ltLimitL,
/* RAM�A�h���X					*/	&(OpPrm.n_ltLimitL),
/* �����l						*/	(USHORT)(	0x0000),
/* ����l						*/	(USHORT)(	0xFFFF),
};



/****************************************************************************************************/
/*		Pn823 : �t�����b�`�\�̈� H [[ref]]														*/
/****************************************************************************************************/
const	PRMDEF	pndef_n_ltLimitH = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_W_SIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x02D6,
/* �����֐��|�C���^				*/	pcal_n_ltLimitL,
/* RAM�A�h���X					*/	&(OpPrm.n_ltLimitH),
/* �����l						*/	(USHORT)(	0x8000),
/* ����l						*/	(USHORT)(	0x7FFF),
};



/****************************************************************************************************/
/*		Pn824 : �I�v�V�������j�^�P�I��																*/
/****************************************************************************************************/
const	PRMDEF	pndef_opMon1 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x02D8,
/* �����֐��|�C���^				*/	pcal_opMon1,
/* RAM�A�h���X					*/	&(OpPrm.opMon1),
/* �����l						*/	(USHORT)(	0),
/* ����l						*/	(USHORT)(	0xFFFF),
};



/****************************************************************************************************/
/*		Pn825 : �I�v�V�������j�^�Q�I��																*/
/****************************************************************************************************/
const	PRMDEF	pndef_opMon2 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x02DA,
/* �����֐��|�C���^				*/	pcal_opMon2,
/* RAM�A�h���X					*/	&(OpPrm.opMon2),
/* �����l						*/	(USHORT)(	0),
/* ����l						*/	(USHORT)(	0xFFFF),
};



/****************************************************************************************************/
/*		Pn826 : ����~�p�P�i�ڒ��������萔 [[1000ref/s^s]]										*/
/****************************************************************************************************/
const	PRMDEF	pndef_decRate1Emg = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER2,
/* EEPROM�A�h���X				*/	0x02DC,
/* �����֐��|�C���^				*/	pcal_decRate1Emg,
/* RAM�A�h���X					*/	&(OpPrm.decRate1Emg),
/* �����l						*/	(USHORT)(	1),
/* ����l						*/	(USHORT)(	65535),
};



/****************************************************************************************************/
/*		Pn827 : ����~�p�Q�i�ڒ��������萔 [[1000ref/s^s]]										*/
/****************************************************************************************************/
const	PRMDEF	pndef_decRate2Emg = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x02DE,
/* �����֐��|�C���^				*/	pcal_decRate2Emg,
/* RAM�A�h���X					*/	&(OpPrm.decRate2Emg),
/* �����l						*/	(USHORT)(	1),
/* ����l						*/	(USHORT)(	65535),
};



/****************************************************************************************************/
/*		Pn828 : ����~�p�����萔�ؑւ����x [[100ref/s]]											*/
/****************************************************************************************************/
const	PRMDEF	pndef_decChgSpdEmg = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER2,
/* EEPROM�A�h���X				*/	0x02E0,
/* �����֐��|�C���^				*/	pcal_decChgSpdEmg,
/* RAM�A�h���X					*/	&(OpPrm.decChgSpdEmg),
/* �����l						*/	(USHORT)(	0),
/* ����l						*/	(USHORT)(	65535),
};



/****************************************************************************************************/
/*		Pn829 : SVOFF�҂�����(������~SVOFF��)														*/
/****************************************************************************************************/
const	PRMDEF	pndef_svOffTime = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x02E2,
/* �����֐��|�C���^				*/	pcal_svOffTime,
/* RAM�A�h���X					*/	&(OpPrm.svOffTime),
/* �����l						*/	(USHORT)(	0),
/* ����l						*/	(USHORT)(	65535),
};



/****************************************************************************************************/
/*		Pn82A : OPTION�@�\�r�b�g���t�P																*/
/****************************************************************************************************/
const	PRMDEF	pndef_optbit1 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASENBL,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x02E4,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.optbit1),
/* �����l						*/	(USHORT)(	0x0000),
/* ����l						*/	(USHORT)(	0x1E1E),
};



/****************************************************************************************************/
/*		Pn82B : OPTION�@�\�r�b�g���t�Q																*/
/****************************************************************************************************/
const	PRMDEF	pndef_optbit2 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASENBL,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x02E6,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.optbit2),
/* �����l						*/	(USHORT)(	0x0000),
/* ����l						*/	(USHORT)(	0x1F1F),
};



/****************************************************************************************************/
/*		Pn82C : OPTION�@�\�r�b�g���t�R																*/
/****************************************************************************************************/
const	PRMDEF	pndef_optbit3 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASENBL,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x02E8,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.optbit3),
/* �����l						*/	(USHORT)(	0x0000),
/* ����l						*/	(USHORT)(	0x1F1F),
};



/****************************************************************************************************/
/*		Pn82D : OPTION�@�\�r�b�g���t�S																*/
/****************************************************************************************************/
const	PRMDEF	pndef_optbit4 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASENBL,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x02EA,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.optbit4),
/* �����l						*/	(USHORT)(	0x0000),
/* ����l						*/	(USHORT)(	0x1F1C),
};



/****************************************************************************************************/
/*		Pn82E : OPTION�@�\�r�b�g���t�T																*/
/****************************************************************************************************/
const	PRMDEF	pndef_optbit5 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASENBL,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x02EC,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.optbit5),
/* �����l						*/	(USHORT)(	0x0000),
/* ����l						*/	(USHORT)(	0x1D1F),
};



/****************************************************************************************************/
/*		Pn82F : �\��																				*/
/****************************************************************************************************/
const	PRMDEF	pndef_resrv6 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASENBL,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER2,
/* EEPROM�A�h���X				*/	0x02EE,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.resrv6),
/* �����l						*/	(USHORT)(	0x0000),
/* ����l						*/	(USHORT)(	0x1D1F),
};



/****************************************************************************************************/
/*		Pn830 : �@�B���]������̎w�ߒP�ʗ�L [[ref]]												*/
/****************************************************************************************************/
const	PRMDEF	pndef_rotrefL = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER2,
/* EEPROM�A�h���X				*/	0x02F0,
/* �����֐��|�C���^				*/	pcal_rotrefL,
/* RAM�A�h���X					*/	&(OpPrm.rotrefL),
/* �����l						*/	(USHORT)(	0x0001),
/* ����l						*/	(USHORT)(	0xFFFF),
};



/****************************************************************************************************/
/*		Pn831 : �@�B���]������̎w�ߒP�ʗ�H [[ref]]												*/
/****************************************************************************************************/
const	PRMDEF	pndef_rotrefH = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER2,
/* EEPROM�A�h���X				*/	0x02F2,
/* �����֐��|�C���^				*/	pcal_rotrefL,
/* RAM�A�h���X					*/	&(OpPrm.rotrefH),
/* �����l						*/	(USHORT)(	0x0000),
/* ����l						*/	(USHORT)(	0x7FFF),
};



/****************************************************************************************************/
/*		Pn832 : �@�B���]������̃X�e�[�V������													*/
/****************************************************************************************************/
const	PRMDEF	pndef_st_num = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER2,
/* EEPROM�A�h���X				*/	0x02F4,
/* �����֐��|�C���^				*/	pcal_st_num,
/* RAM�A�h���X					*/	&(OpPrm.st_num),
/* �����l						*/	(USHORT)(0),
/* ����l						*/	(USHORT)(32767),
};



/****************************************************************************************************/
/*		Pn833 : POSING�萔�I��																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_rateChgSw = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASENBL,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x02F6,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.rateChgSw),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0x0001),
};



/****************************************************************************************************/
/*		Pn834 : �P�i�ڒ��������萔 2nd L [[1000ref/s^s]]											*/
/****************************************************************************************************/
const	PRMDEF	pndef_accRate12L = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x02F8,
/* �����֐��|�C���^				*/	pcal_accRate12L,
/* RAM�A�h���X					*/	&(OpPrm.accRate12L),
/* �����l						*/	(USHORT)(0x0001),
/* ����l						*/	(USHORT)(0x0000),
};



/****************************************************************************************************/
/*		Pn835 : �P�i�ڒ��������萔 2nd H [[1000ref/s^s]]											*/
/****************************************************************************************************/
const	PRMDEF	pndef_accRate12H = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x02FA,
/* �����֐��|�C���^				*/	pcal_accRate12L,
/* RAM�A�h���X					*/	&(OpPrm.accRate12H),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0x0140),
};



/****************************************************************************************************/
/*		Pn836 : �Q�i�ڒ��������萔 2nd L [[1000ref/s^s]]											*/
/****************************************************************************************************/
const	PRMDEF	pndef_accRate22L = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x02FC,
/* �����֐��|�C���^				*/	pcal_accRate22L,
/* RAM�A�h���X					*/	&(OpPrm.accRate22L),
/* �����l						*/	(USHORT)(0x0001),
/* ����l						*/	(USHORT)(0x0000),
};



/****************************************************************************************************/
/*		Pn837 : �Q�i�ڒ��������萔 2nd H [[1000ref/s^s]]											*/
/****************************************************************************************************/
const	PRMDEF	pndef_accRate22H = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x02FE,
/* �����֐��|�C���^				*/	pcal_accRate22L,
/* RAM�A�h���X					*/	&(OpPrm.accRate22H),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0x0140),
};



/****************************************************************************************************/
/*		Pn838 : �����萔�ؑւ����x 2nd L [[ref/s]]													*/
/****************************************************************************************************/
const	PRMDEF	pndef_accChgSpd2L = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0300,
/* �����֐��|�C���^				*/	pcal_accChgSpd2L,
/* RAM�A�h���X					*/	&(OpPrm.accChgSpd2L),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0x0000),
};



/****************************************************************************************************/
/*		Pn839 : �����萔�ؑւ����x 2nd H [[ref/s]]													*/
/****************************************************************************************************/
const	PRMDEF	pndef_accChgSpd2H = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0302,
/* �����֐��|�C���^				*/	pcal_accChgSpd2L,
/* RAM�A�h���X					*/	&(OpPrm.accChgSpd2H),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0x7D00),
};



/****************************************************************************************************/
/*		Pn83A : �P�i�ڒ��������萔 2nd L [[1000ref/s^s]]											*/
/****************************************************************************************************/
const	PRMDEF	pndef_decRate12L = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0304,
/* �����֐��|�C���^				*/	pcal_decRate12L,
/* RAM�A�h���X					*/	&(OpPrm.decRate12L),
/* �����l						*/	(USHORT)(0x0001),
/* ����l						*/	(USHORT)(0x0000),
};



/****************************************************************************************************/
/*		Pn83B : �P�i�ڒ��������萔 2nd H [[1000ref/s^s]]											*/
/****************************************************************************************************/
const	PRMDEF	pndef_decRate12H = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0306,
/* �����֐��|�C���^				*/	pcal_decRate12L,
/* RAM�A�h���X					*/	&(OpPrm.decRate12H),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0x0140),
};



/****************************************************************************************************/
/*		Pn83C : �Q�i�ڒ��������萔 2nd L [[1000ref/s^s]]											*/
/****************************************************************************************************/
const	PRMDEF	pndef_decRate22L = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0308,
/* �����֐��|�C���^				*/	pcal_decRate22L,
/* RAM�A�h���X					*/	&(OpPrm.decRate22L),
/* �����l						*/	(USHORT)(0x0001),
/* ����l						*/	(USHORT)(0x0000),
};



/****************************************************************************************************/
/*		Pn83D : �Q�i�ڒ��������萔 2nd H [[1000ref/s^s]]											*/
/****************************************************************************************************/
const	PRMDEF	pndef_decRate22H = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x030A,
/* �����֐��|�C���^				*/	pcal_decRate22L,
/* RAM�A�h���X					*/	&(OpPrm.decRate22H),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0x0140),
};



/****************************************************************************************************/
/*		Pn83E : �����萔�ؑւ����x 2nd L [[ref/s]]													*/
/****************************************************************************************************/
const	PRMDEF	pndef_decChgSpd2L = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x030C,
/* �����֐��|�C���^				*/	pcal_decChgSpd2L,
/* RAM�A�h���X					*/	&(OpPrm.decChgSpd2L),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0x0000),
};



/****************************************************************************************************/
/*		Pn83F : �����萔�ؑւ����x 2nd H [[ref/s]]													*/
/****************************************************************************************************/
const	PRMDEF	pndef_decChgSpd2H = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x030E,
/* �����֐��|�C���^				*/	pcal_decChgSpd2L,
/* RAM�A�h���X					*/	&(OpPrm.decChgSpd2H),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0x7D00),
};



/****************************************************************************************************/
/*		Pn840 : ����~�p�Q�i�ڒ��������萔�@2nd L [[1000ref/s^s]]									*/
/****************************************************************************************************/
const	PRMDEF	pndef_decRateEmg2L = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0310,
/* �����֐��|�C���^				*/	pcal_decRateEmg2L,
/* RAM�A�h���X					*/	&(OpPrm.decRateEmg2L),
/* �����l						*/	(USHORT)(0x0001),
/* ����l						*/	(USHORT)(0x0000),
};



/****************************************************************************************************/
/*		Pn841 : ����~�p�Q�i�ڒ��������萔�@2nd H [[1000ref/s^s]]									*/
/****************************************************************************************************/
const	PRMDEF	pndef_decRateEmg2H = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0312,
/* �����֐��|�C���^				*/	pcal_decRateEmg2L,
/* RAM�A�h���X					*/	&(OpPrm.decRateEmg2H),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0x0140),
};



/****************************************************************************************************/
/*		Pn842 : ���_���A�A�v���[�`���x�P 2nd L [[100ref/s]]											*/
/****************************************************************************************************/
const	PRMDEF	pndef_zretSpd12L = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x06,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0345,
/* �����֐��|�C���^				*/	pcal_zretSpd12,
/* RAM�A�h���X					*/	&(OpPrm.zretSpd12L),
/* �����l						*/	(USHORT)(	0),
/* ����l						*/	(USHORT)(0x0000),
};



/****************************************************************************************************/
/*		Pn843 : ���_���A�A�v���[�`���x�P 2nd H [[100ref/s]]											*/
/****************************************************************************************************/
const	PRMDEF	pndef_zretSpd12H = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x06,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0347,
/* �����֐��|�C���^				*/	pcal_zretSpd12,
/* RAM�A�h���X					*/	&(OpPrm.zretSpd12H),
/* �����l						*/	(USHORT)(	0),
/* ����l						*/	(USHORT)(0x0140),
};



/****************************************************************************************************/
/*		Pn844 : ���_���A�A�v���[�`���x�Q 2nd L [[100ref/s]]											*/
/****************************************************************************************************/
const	PRMDEF	pndef_zretSpd22L = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x06,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0349,
/* �����֐��|�C���^				*/	pcal_zretSpd22,
/* RAM�A�h���X					*/	&(OpPrm.zretSpd22L),
/* �����l						*/	(USHORT)(	0),
/* ����l						*/	(USHORT)(0x0000),
};



/****************************************************************************************************/
/*		Pn845 : ���_���A�A�v���[�`���x�Q 2nd H [[100ref/s]]											*/
/****************************************************************************************************/
const	PRMDEF	pndef_zretSpd22H = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x06,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x034B,
/* �����֐��|�C���^				*/	pcal_zretSpd22,
/* RAM�A�h���X					*/	&(OpPrm.zretSpd22H),
/* �����l						*/	(USHORT)(	0),
/* ����l						*/	(USHORT)(0x0140),
};



/****************************************************************************************************/
/*		Pn850 : �A�����b�`�V�[�P���X��																*/
/****************************************************************************************************/
const	PRMDEF	pndef_ltSeqNum = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0314,
/* �����֐��|�C���^				*/	pcal_ltSeqNum,
/* RAM�A�h���X					*/	&(OpPrm.ltSeqNum),
/* �����l						*/	(USHORT)(0),
/* ����l						*/	(USHORT)(8),
};



/****************************************************************************************************/
/*		Pn851 : �A�����b�`��																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_ltLoopNum = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0316,
/* �����֐��|�C���^				*/	pcal_ltLoopNum,
/* RAM�A�h���X					*/	&(OpPrm.ltLoopNum),
/* �����l						*/	(USHORT)(0),
/* ����l						*/	(USHORT)(255),
};



/****************************************************************************************************/
/*		Pn852 : �A�����b�`�V�[�P���X�ݒ�1-4															*/
/****************************************************************************************************/
const	PRMDEF	pndef_ltSeqSet1 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASENBL,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0318,
/* �����֐��|�C���^				*/	pcal_ltSeqSet1,
/* RAM�A�h���X					*/	&(OpPrm.ltSeqSet1),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0x3333),
};



/****************************************************************************************************/
/*		Pn853 : �A�����b�`�V�[�P���X�ݒ�4-8															*/
/****************************************************************************************************/
const	PRMDEF	pndef_ltSeqSet2 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASENBL,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x031A,
/* �����֐��|�C���^				*/	pcal_ltSeqSet2,
/* RAM�A�h���X					*/	&(OpPrm.ltSeqSet2),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0x3333),
};



/****************************************************************************************************/
/*		Pn880 : �ǃA�h���X�ݒ�																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_nodeAdr = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.nodeAdr),
/* �����l						*/	(USHORT)(0),
/* ����l						*/	(USHORT)(65535),
};



/****************************************************************************************************/
/*		Pn881 : �`���o�C�g��																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_trByte = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.trByte),
/* �����l						*/	(USHORT)(0),
/* ����l						*/	(USHORT)(65535),
};



/****************************************************************************************************/
/*		Pn882 : �`������																			*/
/****************************************************************************************************/
const	PRMDEF	pndef_comTCycle = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.comTCycle),
/* �����l						*/	(USHORT)(0),
/* ����l						*/	(USHORT)(65535),
};



/****************************************************************************************************/
/*		Pn883 : �ʐM����																			*/
/****************************************************************************************************/
const	PRMDEF	pndef_comAplCycle = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.comAplCycle),
/* �����l						*/	(USHORT)(0),
/* ����l						*/	(USHORT)(65535),
};



/****************************************************************************************************/
/*		Pn88A : ��M�G���[�J�E���^																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comErrCnt = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.comErrCnt),
/* �����l						*/	(USHORT)(0),
/* ����l						*/	(USHORT)(0),
};



/****************************************************************************************************/
/*		Pn88B : (�\��)																				*/
/****************************************************************************************************/
const	PRMDEF	pndef_comErrSts1 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER2,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.comErrSts1),
/* �����l						*/	(USHORT)(0),
/* ����l						*/	(USHORT)(0),
};



/****************************************************************************************************/
/*		Pn88C : �ʐM�G���[�������́A�G���[�X�e�[�^�X(�ݐ�)											*/
/****************************************************************************************************/
const	PRMDEF	pndef_comErrSts2 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER2,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.comErrSts2),
/* �����l						*/	(USHORT)(0),
/* ����l						*/	(USHORT)(0),
};



/****************************************************************************************************/
/*		Pn88F : �ʐMCMD/RSP�f�[�^����																*/
/****************************************************************************************************/
const	PRMDEF	pndef_comDataCtrl = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASENBL,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER2,
/* EEPROM�A�h���X				*/	0x031C,
/* �����֐��|�C���^				*/	pcal_comDataCtrl,
/* RAM�A�h���X					*/	&(OpPrm.comDataCtrl),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn890 : �A���[�����A�ʐMCMD�f�[�^															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almCmd0 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.almCmd[0]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn891 : �A���[�����A�ʐMCMD�f�[�^															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almCmd1 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.almCmd[1]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn892 : �A���[�����A�ʐMCMD�f�[�^															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almCmd2 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.almCmd[2]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn893 : �A���[�����A�ʐMCMD�f�[�^															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almCmd3 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.almCmd[3]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn894 : �A���[�����A�ʐMCMD�f�[�^															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almCmd4 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.almCmd[4]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn895 : �A���[�����A�ʐMCMD�f�[�^															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almCmd5 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.almCmd[5]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn896 : �A���[�����A�ʐMCMD�f�[�^															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almCmd6 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.almCmd[6]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn897 : �A���[�����A�ʐMCMD�f�[�^															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almCmd7 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.almCmd[7]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn898 : �A���[�����A�ʐMCMD�f�[�^															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almCmd8 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.almCmd[8]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn899 : �A���[�����A�ʐMCMD�f�[�^															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almCmd9 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.almCmd[9]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn89A : �A���[�����A�ʐMCMD�f�[�^															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almCmd10 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.almCmd[10]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn89B : �A���[�����A�ʐMCMD�f�[�^															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almCmd11 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.almCmd[11]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn89C : �A���[�����A�ʐMCMD�f�[�^															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almCmd12 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.almCmd[12]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn89D : �A���[�����A�ʐMCMD�f�[�^															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almCmd13 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.almCmd[13]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn89E : �A���[�����A�ʐMCMD�f�[�^															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almCmd14 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.almCmd[14]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn89F : �A���[�����A�ʐMCMD�f�[�^															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almCmd15 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.almCmd[15]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8A0 : �A���[�����A�ʐMRSP�f�[�^															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almRsp0 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.almRsp[0]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8A1 : �A���[�����A�ʐMRSP�f�[�^															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almRsp1 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.almRsp[1]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8A2 : �A���[�����A�ʐMRSP�f�[�^															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almRsp2 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.almRsp[2]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8A3 : �A���[�����A�ʐMRSP�f�[�^															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almRsp3 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.almRsp[3]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8A4 : �A���[�����A�ʐMRSP�f�[�^															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almRsp4 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.almRsp[4]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8A5 : �A���[�����A�ʐMRSP�f�[�^															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almRsp5 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.almRsp[5]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8A6 : �A���[�����A�ʐMRSP�f�[�^															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almRsp6 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.almRsp[6]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8A7 : �A���[�����A�ʐMRSP�f�[�^															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almRsp7 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.almRsp[7]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8A8 : �A���[�����A�ʐMRSP�f�[�^															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almRsp8 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.almRsp[8]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8A9 : �A���[�����A�ʐMRSP�f�[�^															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almRsp9 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.almRsp[9]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8AA : �A���[�����A�ʐMRSP�f�[�^															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almRsp10 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.almRsp[10]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8AB : �A���[�����A�ʐMRSP�f�[�^															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almRsp11 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.almRsp[11]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8AC : �A���[�����A�ʐMRSP�f�[�^															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almRsp12 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.almRsp[12]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8AD : �A���[�����A�ʐMRSP�f�[�^															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almRsp13 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.almRsp[13]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8AE : �A���[�����A�ʐMRSP�f�[�^															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almRsp14 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.almRsp[14]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8AF : �A���[�����A�ʐMRSP�f�[�^															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almRsp15 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.almRsp[15]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8B0 : Online�ʐMCMD�f�[�^																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comCmd0 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER2,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.comCmd[0]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8B1 : Online�ʐMCMD�f�[�^																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comCmd1 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER2,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.comCmd[1]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8B2 : Online�ʐMCMD�f�[�^																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comCmd2 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER2,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.comCmd[2]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8B3 : Online�ʐMCMD�f�[�^																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comCmd3 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER2,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.comCmd[3]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8B4 : Online�ʐMCMD�f�[�^																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comCmd4 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER2,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.comCmd[4]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8B5 : Online�ʐMCMD�f�[�^																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comCmd5 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER2,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.comCmd[5]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8B6 : Online�ʐMCMD�f�[�^																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comCmd6 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER2,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.comCmd[6]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8B7 : Online�ʐMCMD�f�[�^																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comCmd7 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER2,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.comCmd[7]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8B8 : Online�ʐMCMD�f�[�^																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comCmd8 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER2,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.comCmd[8]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8B9 : Online�ʐMCMD�f�[�^																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comCmd9 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER2,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.comCmd[9]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8BA : Online�ʐMCMD�f�[�^																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comCmd10 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER2,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.comCmd[10]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8BB : Online�ʐMCMD�f�[�^																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comCmd11 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER2,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.comCmd[11]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8BC : Online�ʐMCMD�f�[�^																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comCmd12 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER2,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.comCmd[12]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8BD : Online�ʐMCMD�f�[�^																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comCmd13 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER2,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.comCmd[13]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8BE : Online�ʐMCMD�f�[�^																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comCmd14 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER2,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.comCmd[14]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8BF : Online�ʐMCMD�f�[�^																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comCmd15 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER2,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.comCmd[15]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8C0 : Online�ʐMRSP�f�[�^																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comRsp0 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER2,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.comRsp[0]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8C1 : Online�ʐMRSP�f�[�^																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comRsp1 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER2,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.comRsp[1]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8C2 : Online�ʐMRSP�f�[�^																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comRsp2 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER2,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.comRsp[2]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8C3 : Online�ʐMRSP�f�[�^																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comRsp3 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER2,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.comRsp[3]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8C4 : Online�ʐMRSP�f�[�^																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comRsp4 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER2,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.comRsp[4]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8C5 : Online�ʐMRSP�f�[�^																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comRsp5 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER2,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.comRsp[5]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8C6 : Online�ʐMRSP�f�[�^																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comRsp6 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER2,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.comRsp[6]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8C7 : Online�ʐMRSP�f�[�^																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comRsp7 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER2,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.comRsp[7]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8C8 : Online�ʐMRSP�f�[�^																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comRsp8 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER2,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.comRsp[8]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8C9 : Online�ʐMRSP�f�[�^																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comRsp9 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER2,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.comRsp[9]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8CA : Online�ʐMRSP�f�[�^																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comRsp10 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER2,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.comRsp[10]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8CB : Online�ʐMRSP�f�[�^																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comRsp11 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER2,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.comRsp[11]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8CC : Online�ʐMRSP�f�[�^																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comRsp12 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER2,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.comRsp[12]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8CD : Online�ʐMRSP�f�[�^																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comRsp13 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER2,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.comRsp[13]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8CE : Online�ʐMRSP�f�[�^																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comRsp14 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER2,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.comRsp[14]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8CF : Online�ʐMRSP�f�[�^																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comRsp15 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER2,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.comRsp[15]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8E0 : �w��Option�p Option Card ID															*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardID = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.OpCardID),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8E1 : �w��Option�p Option ����ݒ�														*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardCtrlSw = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASENBL,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.OpCardCtrlSw),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0x0001),
};



/****************************************************************************************************/
/*		Pn8E2 : �w��Option�p Option S/W Ver.														*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardSwVerL = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.OpCardSwVerL),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8E3 : �w��Option�p Option S/W Ver.														*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardSwVerH = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.OpCardSwVerH),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8E4 : �w��Option�p Option Y Spec. No.														*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardYNo = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.OpCardYNo),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8E5 : �w��Option�p ManufactureDate.														*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardDate = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.OpCardDate),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8E6 : �w��Option�p �T�[�{SW�ݒ�															*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardSvSw = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.OpCardSvSw),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8E8 : �w��Option�p �A���[���ݒ�															*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardAlmCode = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_SetOpCardAlarm,
/* RAM�A�h���X					*/	&(OpPrm.OpCardAlmCode),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8E9 : �w��Option�p �A���[���ݒ�															*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardAlmAttr = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	TRUE,
/* :1: ���[�h��					*/	1,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_SetOpCardAlarm,
/* RAM�A�h���X					*/	&(OpPrm.OpCardAlmAttr),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8F0 : �w��Option�p Option Model															*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardModel0 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.OpCardModel[0]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8F1 : �w��Option�p Option Model															*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardModel1 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.OpCardModel[1]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8F2 : �w��Option�p Option Model															*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardModel2 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.OpCardModel[2]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8F3 : �w��Option�p Option Model															*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardModel3 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.OpCardModel[3]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8F4 : �w��Option�p Option Model															*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardModel4 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.OpCardModel[4]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8F5 : �w��Option�p Option Model															*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardModel5 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.OpCardModel[5]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8F6 : �w��Option�p Option Model															*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardModel6 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.OpCardModel[6]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8F7 : �w��Option�p Option Model															*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardModel7 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.OpCardModel[7]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8F8 : �w��Option�p Serial No																*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardSNo0 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.OpCardSNo[0]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8F9 : �w��Option�p Serial No																*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardSNo1 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.OpCardSNo[1]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8FA : �w��Option�p Serial No																*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardSNo2 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.OpCardSNo[2]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8FB : �w��Option�p Serial No																*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardSNo3 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.OpCardSNo[3]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8FC : �w��Option�p Serial No																*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardSNo4 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.OpCardSNo[4]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8FD : �w��Option�p Serial No																*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardSNo5 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.OpCardSNo[5]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8FE : �w��Option�p Serial No																*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardSNo6 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.OpCardSNo[6]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8FF : �w��Option�p Serial No																*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardSNo7 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.OpCardSNo[7]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn900 : �o���N��																			*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkNum = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x031E,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.bnkNum),
/* �����l						*/	(USHORT)(0),
/* ����l						*/	(USHORT)(16),
};



/****************************************************************************************************/
/*		Pn901 : �o���N�����o��																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkMemNum = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEDEC,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0320,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.bnkMemNum),
/* �����l						*/	(USHORT)(0),
/* ����l						*/	(USHORT)(15),
};



/****************************************************************************************************/
/*		Pn902 : �o���N�����o																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkMember0 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0322,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.bnkMember[0x0]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0x08FF),
};



/****************************************************************************************************/
/*		Pn903 : �o���N�����o																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkMember1 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0324,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.bnkMember[0x1]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0x08FF),
};



/****************************************************************************************************/
/*		Pn904 : �o���N�����o																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkMember2 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0326,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.bnkMember[0x2]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0x08FF),
};



/****************************************************************************************************/
/*		Pn905 : �o���N�����o																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkMember3 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0328,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.bnkMember[0x3]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0x08FF),
};



/****************************************************************************************************/
/*		Pn906 : �o���N�����o																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkMember4 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x032A,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.bnkMember[0x4]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0x08FF),
};



/****************************************************************************************************/
/*		Pn907 : �o���N�����o																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkMember5 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x032C,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.bnkMember[0x5]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0x08FF),
};



/****************************************************************************************************/
/*		Pn908 : �o���N�����o																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkMember6 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x032E,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.bnkMember[0x6]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0x08FF),
};



/****************************************************************************************************/
/*		Pn909 : �o���N�����o																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkMember7 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0330,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.bnkMember[0x7]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0x08FF),
};



/****************************************************************************************************/
/*		Pn90A : �o���N�����o																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkMember8 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0332,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.bnkMember[0x8]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0x08FF),
};



/****************************************************************************************************/
/*		Pn90B : �o���N�����o																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkMember9 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0334,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.bnkMember[0x9]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0x08FF),
};



/****************************************************************************************************/
/*		Pn90C : �o���N�����o																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkMemberA = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0336,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.bnkMember[0xA]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0x08FF),
};



/****************************************************************************************************/
/*		Pn90D : �o���N�����o																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkMemberB = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0338,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.bnkMember[0xB]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0x08FF),
};



/****************************************************************************************************/
/*		Pn90E : �o���N�����o																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkMemberC = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x033A,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.bnkMember[0xC]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0x08FF),
};



/****************************************************************************************************/
/*		Pn90F : �o���N�����o																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkMemberD = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x033C,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.bnkMember[0xD]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0x08FF),
};



/****************************************************************************************************/
/*		Pn910 : �o���N�����o																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkMemberE = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x033E,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.bnkMember[0xE]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0x08FF),
};



/****************************************************************************************************/
/*		Pn920 : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData00 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData00,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x00]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn921 : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData01 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData01,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x01]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn922 : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData02 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData02,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x02]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn923 : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData03 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData03,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x03]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn924 : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData04 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData04,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x04]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn925 : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData05 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData05,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x05]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn926 : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData06 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData06,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x06]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn927 : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData07 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData07,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x07]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn928 : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData08 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData08,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x08]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn929 : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData09 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData09,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x09]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn92A : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData0A = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData0A,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x0A]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn92B : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData0B = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData0B,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x0B]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn92C : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData0C = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData0C,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x0C]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn92D : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData0D = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData0D,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x0D]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn92E : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData0E = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData0E,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x0E]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn92F : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData0F = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData0F,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x0F]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn930 : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData10 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData10,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x10]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn931 : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData11 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData11,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x11]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn932 : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData12 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData12,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x12]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn933 : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData13 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData13,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x13]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn934 : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData14 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData14,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x14]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn935 : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData15 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData15,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x15]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn936 : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData16 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData16,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x16]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn937 : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData17 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData17,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x17]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn938 : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData18 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData18,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x18]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn939 : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData19 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData19,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x19]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn93A : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData1A = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData1A,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x1A]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn93B : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData1B = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData1B,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x1B]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn93C : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData1C = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData1C,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x1C]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn93D : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData1D = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData1D,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x1D]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn93E : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData1E = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData1E,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x1E]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn93F : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData1F = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData1F,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x1F]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn940 : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData20 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData20,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x20]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn941 : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData21 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData21,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x21]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn942 : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData22 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData22,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x22]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn943 : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData23 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData23,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x23]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn944 : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData24 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData24,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x24]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn945 : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData25 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData25,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x25]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn946 : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData26 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData26,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x26]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn947 : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData27 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData27,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x27]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn948 : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData28 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData28,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x28]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn949 : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData29 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData29,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x29]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn94A : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData2A = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData2A,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x2A]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn94B : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData2B = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData2B,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x2B]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn94C : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData2C = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData2C,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x2C]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn94D : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData2D = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData2D,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x2D]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn94E : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData2E = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData2E,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x2E]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn94F : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData2F = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData2F,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x2F]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn950 : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData30 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData30,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x30]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn951 : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData31 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData31,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x31]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn952 : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData32 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData32,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x32]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn953 : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData33 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData33,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x33]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn954 : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData34 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData34,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x34]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn955 : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData35 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData35,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x35]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn956 : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData36 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData36,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x36]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn957 : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData37 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData37,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x37]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn958 : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData38 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData38,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x38]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn959 : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData39 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData39,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x39]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn95A : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData3A = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData3A,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x3A]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn95B : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData3B = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData3B,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x3B]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn95C : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData3C = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData3C,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x3C]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn95D : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData3D = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData3D,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x3D]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn95E : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData3E = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData3E,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x3E]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn95F : �o���N�f�[�^																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData3F = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pcal_bnkData3F,
/* RAM�A�h���X					*/	&(OpPrm.bnkData[0x3F]),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn9E0 : �p�����[�^Ver.																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_prmVer = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_SYSTEM,
/* EEPROM�A�h���X				*/	0x0340 | PRMDEF_NODEFTPRM,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.prmVer),
/* �����l						*/	(USHORT)(0),
/* ����l						*/	(USHORT)(65535),
};



/****************************************************************************************************/
/*		Pn9E1 : ���x����\�ݒ�																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_spdDivision = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NEEDBOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_SYSTEM,
/* EEPROM�A�h���X				*/	0x0341 | PRMDEF_NODEFTPRM,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.spdDivision),
/* �����l						*/	(USHORT)(0),
/* ����l						*/	(USHORT)(23),
};



/****************************************************************************************************/
/*		Pn9E2 : Command-Option�@�\SW																*/
/****************************************************************************************************/
const	PRMDEF	pndef_CmdOpSw = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_SYSTEM,
/* EEPROM�A�h���X				*/	0x0342 | PRMDEF_NODEFTPRM,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.CmdOpSw),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0x1111),
};



/****************************************************************************************************/
/*		Pn9E3 : 																					*/
/****************************************************************************************************/
const	PRMDEF	pndef_sys_rsrv2 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_SYSTEM,
/* EEPROM�A�h���X				*/	0x0343 | PRMDEF_NODEFTPRM,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.sys_rsrv2),
/* �����l						*/	(USHORT)(0),
/* ����l						*/	(USHORT)(65535),
};



/****************************************************************************************************/
/*		Pn9E4 : 																					*/
/****************************************************************************************************/
const	PRMDEF	pndef_sys_rsrv3 = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_RAM_EEPROM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_WRITABLE,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_SYSTEM,
/* EEPROM�A�h���X				*/	0x0344 | PRMDEF_NODEFTPRM,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.sys_rsrv3),
/* �����l						*/	(USHORT)(0),
/* ����l						*/	(USHORT)(65535),
};



/****************************************************************************************************/
/*		PnB00 : ���z�w�߃I�v�V�����p�����[�^														*/
/****************************************************************************************************/
const	PRMDEF	pndef_cmdopv = {
/* �����w��(Bitfield)			*/	{
/* :1: �����O�p�����[�^			*/	FALSE,
/* :1: ���[�h��					*/	0,
/* :1: �����p�����[�^���H		*/	PRMDEF_NORMAL,
/* :1: �d���ė��グ�v/�s�v		*/	PRMDEF_NON_BOOT,
/* :1: �i�[�ꏊ					*/	PRMDEF_ONLY_RAM,
/* :2: �\�����̒�A				*/	DSPDEF_BASEHEX,
/* :1: �\�����̕����L��			*/	DSPDEF_NOSIGN,
/* :3: �����_�ȉ�����			*/	0,
/* :1: �����_�ʒu�ύX(LSM)		*/	FALSE,
/* :1: ���[�h�I�����[			*/	PRMDEF_READONLY,
/* :1: �I�y���[�^���͒���		*/	FALSE,
/* :X: �\��						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: �P��						*/	UNIT_NONE },
/* ��p�g�pCHECK�bOPE�\���I��	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* �A�N�Z�X���x��				*/	ACCLVL_USER1,
/* EEPROM�A�h���X				*/	0x0000,
/* �����֐��|�C���^				*/	pncal_dummy,
/* RAM�A�h���X					*/	&(OpPrm.cmdopv),
/* �����l						*/	(USHORT)(0x0000),
/* ����l						*/	(USHORT)(0xFFFF),
};





/****************************************************************************************************/
/*																									*/
/*		�p�����[�^�Ǘ��e�[�u��																		*/
/*																									*/
/****************************************************************************************************/
#pragma ghs section rodata="PnPrmTblUser"
CPRMTBL	PnPrmTblUser[] = {
/*  Idx			  PrmNo   PrmDef																	*/
/*--------------------------------------------------------------------------------------------------*/
/*    0 */		{ 0x800, &pndef_comCtrl							},
/*    1 */		{ 0x801, &pndef_sLimitSw						},
/*    2 */		{ 0x802, &pndef_cmdCtrlSw						},
/*    3 */		{ 0x803, &pndef_zpWidth							},
/*    4 */		{ 0x804, &pndef_p_sLimitL						},
/*    5 */		{ 0x805, &pndef_p_sLimitH						},
/*    6 */		{ 0x806, &pndef_n_sLimitL						},
/*    7 */		{ 0x807, &pndef_n_sLimitH						},
/*    8 */		{ 0x808, &pndef_absOffsetL						},
/*    9 */		{ 0x809, &pndef_absOffsetH						},
/*   10 */		{ 0x80A, &pndef_accRate1						},
/*   11 */		{ 0x80B, &pndef_accRate2						},
/*   12 */		{ 0x80C, &pndef_accChgSpd						},
/*   13 */		{ 0x80D, &pndef_decRate1						},
/*   14 */		{ 0x80E, &pndef_decRate2						},
/*   15 */		{ 0x80F, &pndef_decChgSpd						},
/*   16 */		{ 0x810, &pndef_filExpBias						},
/*   17 */		{ 0x811, &pndef_filExpTb						},
/*   18 */		{ 0x812, &pndef_filMaTb							},
/*   19 */		{ 0x813, &pndef_resrv2							},
/*   20 */		{ 0x814, &pndef_exDistL							},
/*   21 */		{ 0x815, &pndef_exDistH							},
/*   22 */		{ 0x816, &pndef_zretDir							},
/*   23 */		{ 0x817, &pndef_zretSpd1						},
/*   24 */		{ 0x818, &pndef_zretSpd2						},
/*   25 */		{ 0x819, &pndef_zretDistL						},
/*   26 */		{ 0x81A, &pndef_zretDistH						},
/*   27 */		{ 0x81B, &pndef_resrv3							},
/*   28 */		{ 0x81C, &pndef_resrv4							},
/*   29 */		{ 0x81D, &pndef_resrv5							},
/*   30 */		{ 0x81E, &pndef_ioMonSel						},
/*   31 */		{ 0x81F, &pndef_optBitSw						},
/*   32 */		{ 0x820, &pndef_p_ltLimitL						},
/*   33 */		{ 0x821, &pndef_p_ltLimitH						},
/*   34 */		{ 0x822, &pndef_n_ltLimitL						},
/*   35 */		{ 0x823, &pndef_n_ltLimitH						},
/*   36 */		{ 0x824, &pndef_opMon1							},
/*   37 */		{ 0x825, &pndef_opMon2							},
/*   38 */		{ 0x826, &pndef_decRate1Emg						},
/*   39 */		{ 0x827, &pndef_decRate2Emg						},
/*   40 */		{ 0x828, &pndef_decChgSpdEmg					},
/*   41 */		{ 0x829, &pndef_svOffTime						},
/*   42 */		{ 0x82A, &pndef_optbit1							},
/*   43 */		{ 0x82B, &pndef_optbit2							},
/*   44 */		{ 0x82C, &pndef_optbit3							},
/*   45 */		{ 0x82D, &pndef_optbit4							},
/*   46 */		{ 0x82E, &pndef_optbit5							},
/*   47 */		{ 0x82F, &pndef_resrv6							},
/*   48 */		{ 0x830, &pndef_rotrefL							},
/*   49 */		{ 0x831, &pndef_rotrefH							},
/*   50 */		{ 0x832, &pndef_st_num							},
/*   51 */		{ 0x833, &pndef_rateChgSw						},
/*   52 */		{ 0x834, &pndef_accRate12L						},
/*   53 */		{ 0x835, &pndef_accRate12H						},
/*   54 */		{ 0x836, &pndef_accRate22L						},
/*   55 */		{ 0x837, &pndef_accRate22H						},
/*   56 */		{ 0x838, &pndef_accChgSpd2L						},
/*   57 */		{ 0x839, &pndef_accChgSpd2H						},
/*   58 */		{ 0x83A, &pndef_decRate12L						},
/*   59 */		{ 0x83B, &pndef_decRate12H						},
/*   60 */		{ 0x83C, &pndef_decRate22L						},
/*   61 */		{ 0x83D, &pndef_decRate22H						},
/*   62 */		{ 0x83E, &pndef_decChgSpd2L						},
/*   63 */		{ 0x83F, &pndef_decChgSpd2H						},
/*   64 */		{ 0x840, &pndef_decRateEmg2L					},
/*   65 */		{ 0x841, &pndef_decRateEmg2H					},
/*   66 */		{ 0x842, &pndef_zretSpd12L						},
/*   67 */		{ 0x843, &pndef_zretSpd12H						},
/*   68 */		{ 0x844, &pndef_zretSpd22L						},
/*   69 */		{ 0x845, &pndef_zretSpd22H						},
/*   70 */		{ 0x850, &pndef_ltSeqNum						},
/*   71 */		{ 0x851, &pndef_ltLoopNum						},
/*   72 */		{ 0x852, &pndef_ltSeqSet1						},
/*   73 */		{ 0x853, &pndef_ltSeqSet2						},
/*   74 */		{ 0x880, &pndef_nodeAdr							},
/*   75 */		{ 0x881, &pndef_trByte							},
/*   76 */		{ 0x882, &pndef_comTCycle						},
/*   77 */		{ 0x883, &pndef_comAplCycle						},
/*   78 */		{ 0x88A, &pndef_comErrCnt						},
/*   79 */		{ 0x88B, &pndef_comErrSts1						},
/*   80 */		{ 0x88C, &pndef_comErrSts2						},
/*   81 */		{ 0x88F, &pndef_comDataCtrl						},
/*   82 */		{ 0x890, &pndef_almCmd0							},
/*   83 */		{ 0x891, &pndef_almCmd1							},
/*   84 */		{ 0x892, &pndef_almCmd2							},
/*   85 */		{ 0x893, &pndef_almCmd3							},
/*   86 */		{ 0x894, &pndef_almCmd4							},
/*   87 */		{ 0x895, &pndef_almCmd5							},
/*   88 */		{ 0x896, &pndef_almCmd6							},
/*   89 */		{ 0x897, &pndef_almCmd7							},
/*   90 */		{ 0x898, &pndef_almCmd8							},
/*   91 */		{ 0x899, &pndef_almCmd9							},
/*   92 */		{ 0x89A, &pndef_almCmd10						},
/*   93 */		{ 0x89B, &pndef_almCmd11						},
/*   94 */		{ 0x89C, &pndef_almCmd12						},
/*   95 */		{ 0x89D, &pndef_almCmd13						},
/*   96 */		{ 0x89E, &pndef_almCmd14						},
/*   97 */		{ 0x89F, &pndef_almCmd15						},
/*   98 */		{ 0x8A0, &pndef_almRsp0							},
/*   99 */		{ 0x8A1, &pndef_almRsp1							},
/*  100 */		{ 0x8A2, &pndef_almRsp2							},
/*  101 */		{ 0x8A3, &pndef_almRsp3							},
/*  102 */		{ 0x8A4, &pndef_almRsp4							},
/*  103 */		{ 0x8A5, &pndef_almRsp5							},
/*  104 */		{ 0x8A6, &pndef_almRsp6							},
/*  105 */		{ 0x8A7, &pndef_almRsp7							},
/*  106 */		{ 0x8A8, &pndef_almRsp8							},
/*  107 */		{ 0x8A9, &pndef_almRsp9							},
/*  108 */		{ 0x8AA, &pndef_almRsp10						},
/*  109 */		{ 0x8AB, &pndef_almRsp11						},
/*  110 */		{ 0x8AC, &pndef_almRsp12						},
/*  111 */		{ 0x8AD, &pndef_almRsp13						},
/*  112 */		{ 0x8AE, &pndef_almRsp14						},
/*  113 */		{ 0x8AF, &pndef_almRsp15						},
/*  114 */		{ 0x8B0, &pndef_comCmd0							},
/*  115 */		{ 0x8B1, &pndef_comCmd1							},
/*  116 */		{ 0x8B2, &pndef_comCmd2							},
/*  117 */		{ 0x8B3, &pndef_comCmd3							},
/*  118 */		{ 0x8B4, &pndef_comCmd4							},
/*  119 */		{ 0x8B5, &pndef_comCmd5							},
/*  120 */		{ 0x8B6, &pndef_comCmd6							},
/*  121 */		{ 0x8B7, &pndef_comCmd7							},
/*  122 */		{ 0x8B8, &pndef_comCmd8							},
/*  123 */		{ 0x8B9, &pndef_comCmd9							},
/*  124 */		{ 0x8BA, &pndef_comCmd10						},
/*  125 */		{ 0x8BB, &pndef_comCmd11						},
/*  126 */		{ 0x8BC, &pndef_comCmd12						},
/*  127 */		{ 0x8BD, &pndef_comCmd13						},
/*  128 */		{ 0x8BE, &pndef_comCmd14						},
/*  129 */		{ 0x8BF, &pndef_comCmd15						},
/*  130 */		{ 0x8C0, &pndef_comRsp0							},
/*  131 */		{ 0x8C1, &pndef_comRsp1							},
/*  132 */		{ 0x8C2, &pndef_comRsp2							},
/*  133 */		{ 0x8C3, &pndef_comRsp3							},
/*  134 */		{ 0x8C4, &pndef_comRsp4							},
/*  135 */		{ 0x8C5, &pndef_comRsp5							},
/*  136 */		{ 0x8C6, &pndef_comRsp6							},
/*  137 */		{ 0x8C7, &pndef_comRsp7							},
/*  138 */		{ 0x8C8, &pndef_comRsp8							},
/*  139 */		{ 0x8C9, &pndef_comRsp9							},
/*  140 */		{ 0x8CA, &pndef_comRsp10						},
/*  141 */		{ 0x8CB, &pndef_comRsp11						},
/*  142 */		{ 0x8CC, &pndef_comRsp12						},
/*  143 */		{ 0x8CD, &pndef_comRsp13						},
/*  144 */		{ 0x8CE, &pndef_comRsp14						},
/*  145 */		{ 0x8CF, &pndef_comRsp15						},
/*  146 */		{ 0x8E0, &pndef_OpCardID						},
/*  147 */		{ 0x8E1, &pndef_OpCardCtrlSw					},
/*  148 */		{ 0x8E2, &pndef_OpCardSwVerL					},
/*  149 */		{ 0x8E3, &pndef_OpCardSwVerH					},
/*  150 */		{ 0x8E4, &pndef_OpCardYNo						},
/*  151 */		{ 0x8E5, &pndef_OpCardDate						},
/*  152 */		{ 0x8E6, &pndef_OpCardSvSw						},
/*  153 */		{ 0x8E8, &pndef_OpCardAlmCode					},
/*  154 */		{ 0x8E9, &pndef_OpCardAlmAttr					},
/*  155 */		{ 0x8F0, &pndef_OpCardModel0					},
/*  156 */		{ 0x8F1, &pndef_OpCardModel1					},
/*  157 */		{ 0x8F2, &pndef_OpCardModel2					},
/*  158 */		{ 0x8F3, &pndef_OpCardModel3					},
/*  159 */		{ 0x8F4, &pndef_OpCardModel4					},
/*  160 */		{ 0x8F5, &pndef_OpCardModel5					},
/*  161 */		{ 0x8F6, &pndef_OpCardModel6					},
/*  162 */		{ 0x8F7, &pndef_OpCardModel7					},
/*  163 */		{ 0x8F8, &pndef_OpCardSNo0						},
/*  164 */		{ 0x8F9, &pndef_OpCardSNo1						},
/*  165 */		{ 0x8FA, &pndef_OpCardSNo2						},
/*  166 */		{ 0x8FB, &pndef_OpCardSNo3						},
/*  167 */		{ 0x8FC, &pndef_OpCardSNo4						},
/*  168 */		{ 0x8FD, &pndef_OpCardSNo5						},
/*  169 */		{ 0x8FE, &pndef_OpCardSNo6						},
/*  170 */		{ 0x8FF, &pndef_OpCardSNo7						},
/*  171 */		{ 0x900, &pndef_bnkNum							},
/*  172 */		{ 0x901, &pndef_bnkMemNum						},
/*  173 */		{ 0x902, &pndef_bnkMember0						},
/*  174 */		{ 0x903, &pndef_bnkMember1						},
/*  175 */		{ 0x904, &pndef_bnkMember2						},
/*  176 */		{ 0x905, &pndef_bnkMember3						},
/*  177 */		{ 0x906, &pndef_bnkMember4						},
/*  178 */		{ 0x907, &pndef_bnkMember5						},
/*  179 */		{ 0x908, &pndef_bnkMember6						},
/*  180 */		{ 0x909, &pndef_bnkMember7						},
/*  181 */		{ 0x90A, &pndef_bnkMember8						},
/*  182 */		{ 0x90B, &pndef_bnkMember9						},
/*  183 */		{ 0x90C, &pndef_bnkMemberA						},
/*  184 */		{ 0x90D, &pndef_bnkMemberB						},
/*  185 */		{ 0x90E, &pndef_bnkMemberC						},
/*  186 */		{ 0x90F, &pndef_bnkMemberD						},
/*  187 */		{ 0x910, &pndef_bnkMemberE						},
/*  188 */		{ 0x920, &pndef_bnkData00						},
/*  189 */		{ 0x921, &pndef_bnkData01						},
/*  190 */		{ 0x922, &pndef_bnkData02						},
/*  191 */		{ 0x923, &pndef_bnkData03						},
/*  192 */		{ 0x924, &pndef_bnkData04						},
/*  193 */		{ 0x925, &pndef_bnkData05						},
/*  194 */		{ 0x926, &pndef_bnkData06						},
/*  195 */		{ 0x927, &pndef_bnkData07						},
/*  196 */		{ 0x928, &pndef_bnkData08						},
/*  197 */		{ 0x929, &pndef_bnkData09						},
/*  198 */		{ 0x92A, &pndef_bnkData0A						},
/*  199 */		{ 0x92B, &pndef_bnkData0B						},
/*  200 */		{ 0x92C, &pndef_bnkData0C						},
/*  201 */		{ 0x92D, &pndef_bnkData0D						},
/*  202 */		{ 0x92E, &pndef_bnkData0E						},
/*  203 */		{ 0x92F, &pndef_bnkData0F						},
/*  204 */		{ 0x930, &pndef_bnkData10						},
/*  205 */		{ 0x931, &pndef_bnkData11						},
/*  206 */		{ 0x932, &pndef_bnkData12						},
/*  207 */		{ 0x933, &pndef_bnkData13						},
/*  208 */		{ 0x934, &pndef_bnkData14						},
/*  209 */		{ 0x935, &pndef_bnkData15						},
/*  210 */		{ 0x936, &pndef_bnkData16						},
/*  211 */		{ 0x937, &pndef_bnkData17						},
/*  212 */		{ 0x938, &pndef_bnkData18						},
/*  213 */		{ 0x939, &pndef_bnkData19						},
/*  214 */		{ 0x93A, &pndef_bnkData1A						},
/*  215 */		{ 0x93B, &pndef_bnkData1B						},
/*  216 */		{ 0x93C, &pndef_bnkData1C						},
/*  217 */		{ 0x93D, &pndef_bnkData1D						},
/*  218 */		{ 0x93E, &pndef_bnkData1E						},
/*  219 */		{ 0x93F, &pndef_bnkData1F						},
/*  220 */		{ 0x940, &pndef_bnkData20						},
/*  221 */		{ 0x941, &pndef_bnkData21						},
/*  222 */		{ 0x942, &pndef_bnkData22						},
/*  223 */		{ 0x943, &pndef_bnkData23						},
/*  224 */		{ 0x944, &pndef_bnkData24						},
/*  225 */		{ 0x945, &pndef_bnkData25						},
/*  226 */		{ 0x946, &pndef_bnkData26						},
/*  227 */		{ 0x947, &pndef_bnkData27						},
/*  228 */		{ 0x948, &pndef_bnkData28						},
/*  229 */		{ 0x949, &pndef_bnkData29						},
/*  230 */		{ 0x94A, &pndef_bnkData2A						},
/*  231 */		{ 0x94B, &pndef_bnkData2B						},
/*  232 */		{ 0x94C, &pndef_bnkData2C						},
/*  233 */		{ 0x94D, &pndef_bnkData2D						},
/*  234 */		{ 0x94E, &pndef_bnkData2E						},
/*  235 */		{ 0x94F, &pndef_bnkData2F						},
/*  236 */		{ 0x950, &pndef_bnkData30						},
/*  237 */		{ 0x951, &pndef_bnkData31						},
/*  238 */		{ 0x952, &pndef_bnkData32						},
/*  239 */		{ 0x953, &pndef_bnkData33						},
/*  240 */		{ 0x954, &pndef_bnkData34						},
/*  241 */		{ 0x955, &pndef_bnkData35						},
/*  242 */		{ 0x956, &pndef_bnkData36						},
/*  243 */		{ 0x957, &pndef_bnkData37						},
/*  244 */		{ 0x958, &pndef_bnkData38						},
/*  245 */		{ 0x959, &pndef_bnkData39						},
/*  246 */		{ 0x95A, &pndef_bnkData3A						},
/*  247 */		{ 0x95B, &pndef_bnkData3B						},
/*  248 */		{ 0x95C, &pndef_bnkData3C						},
/*  249 */		{ 0x95D, &pndef_bnkData3D						},
/*  250 */		{ 0x95E, &pndef_bnkData3E						},
/*  251 */		{ 0x95F, &pndef_bnkData3F						},
/*  252 */		{ 0x9E0, &pndef_prmVer							},
/*  253 */		{ 0x9E1, &pndef_spdDivision						},
/*  254 */		{ 0x9E2, &pndef_CmdOpSw							},
/*  255 */		{ 0x9E3, &pndef_sys_rsrv2						},
/*  256 */		{ 0x9E4, &pndef_sys_rsrv3						},
/*  257 */		{ 0xB00, &pndef_cmdopv							},
};
#pragma ghs section rodata=default
/*--------------------------------------------------------------------------------------------------*/
const	USHORT	PnPrmTblUserEntNum = sizeof(PnPrmTblUser)/sizeof(PnPrmTblUser[0]);







/***************************************** end of file **********************************************/
