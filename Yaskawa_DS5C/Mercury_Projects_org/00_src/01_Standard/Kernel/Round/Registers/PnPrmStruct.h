/****************************************************************************
Description	: Definition of the Parameter(Pn) Structure
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
I.M.   2010.5.22     created

****************************************************************************/
#ifndef __PN_PRM_STRUCT_H
#define __PN_PRM_STRUCT_H

#include "Basedef.h"
#include "EncData.h"
#include "MotorPoleFind.h"



#define VENDOR_IOPRM_NUM	4						/* SVCMD_IO(�o��)VendorSpecificsI/O����t���p�����[�^��		*//* <S1C0> */

/****************************************************************************
	���j�b�g���ʃp�����[�^�\���̒�`
****************************************************************************/
typedef struct CMN_PRMDATA {
	USHORT	b_prm6;					/* Pn006 : �@�\�I�����p�X�C�b�`�U									*//* 2���ڃA�i���O���j�^�Ή�<S103> */
	USHORT	b_prm7;					/* Pn007 : �@�\�I�����p�X�C�b�`�V									*//* 2���ڃA�i���O���j�^�Ή�<S103> */
//	USHORT	b_prm8;					/* Pn008 : �@�\�I�����p�X�C�b�`�W									*/
	USHORT	b_prmE;					/* Pn00E : �@�\�I�����p�X�C�b�`�d									*//* <S0A6> */
	USHORT	b_prmF;					/* Pn00F : �@�\�I�����p�X�C�b�`�e									*//* <S0A6> */
	USHORT	AxisAddress[2];			/* Pn010 : �X���b�g�ʎ��A�h���X										*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	UniPrmSel;				/* Pn020 : �������j�b�g���ʃp�����[�^�ݒ�							*//* <Oka06> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	CycleSwitch;			/* Pn041 : �X�L���������؂�ւ��X�C�b�`								*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	acoftm;					/* Pn509 : �u��ێ�����												*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	monoffset1;				/* Pn550 : �A�i���O���j�^�P�I�t�Z�b�g�d��							*//* <S0DF> */
	USHORT	monoffset2;				/* Pn551 : �A�i���O���j�^�Q�I�t�Z�b�g�d��							*//* <S0DF> */
	USHORT	mongain1;				/* Pn552 : �A�i���O���j�^1�{��										*//* <S0DF> */
	USHORT	mongain2;				/* Pn553 : �A�i���O���j�^2�{��										*//* <S0DF> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	powmon_unitime ;		/* Pn55A : ����d�̓��j�^�P�ʎ���									*//* <S0A6> *//* <S0DF> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	regres;					/* Pn600 : �񐶒�R�e��												*/
	USHORT	exreger;				/* Pn603 : �񐶒�R�l												*/

//	LONG	TrgioPosition[8];		/* Pn6xx : Trigger I/O */
//	USHORT	TrgioCfg[8];			/* Pn6xx : Trigger I/O */
//	USHORT	TrgioOnTime[8];			/* Pn6xx : Trigger I/O */
//	USHORT	TrgioAdjTime[8];		/* Pn6xx : Trigger I/O */
//	LONG	TrgIoDelayTime[8];		/* Pn6xx : Trigger I/O */

	USHORT	syssw2;					/* PnE0C : �V�X�e���X�C�b�`�Q										*/
	DBYTEX	InstEnvTmp_sw1;			/* PnE0F : ����:�ݒu�����j�^����x, ���:�@�\�����X�C�b�`�P		*//* <S03A> */
	USHORT	reger;					/* PnE1A : �񐶒�R�l(�f�t�H���g)									*//* <S0F4> */
	USHORT	wre;					/* PnE1C : �񐶒�R�e��(�f�t�H���g)									*/
	USHORT	kvvfil;					/* PnE3E : ���H�d���␳�p���[�p�X�t�B���^���萔					*/
	USHORT	mnrng0;					/* PnE42 : ���j�^�P�Q�C��											*//* <S016> *//* <S0DF> */
	USHORT	mnrng1;					/* PnE43 : ���j�^�Q�Q�C��											*//* <S016> *//* <S0DF> */
	ULONG 	mnptr0l;				/* PnE48 : ���j�^�P�A�h���X											*//* <S0DF> */
	ULONG 	mnptr1l;				/* PnE4A : ���j�^�Q�A�h���X											*//* <S0DF> */
	DBYTEX	mnaz0_1;				/* PnE58 : ���j�^�P,�Q�[����										*//* <S016> */
	DBYTEX	mnag0_1;				/* PnE59 : ���j�^�P,�Q�Q�C����										*//* <S016> */
	DBYTEX	pgadj;					/* PnE5A : ����:PG�d���d����, ���:��								*/
	DBYTEX	pnadj;					/* PnE5B : ����:P-N�d�����o���x������, ���:P-N�d�����o���x���[����	*/

	USHORT	NonRegOvlvl;			/* PnE71 : �񐶒�R������@��ŉ񐶕s�g�p���̂n�u���o���x��			*//* <S0F7> */
	USHORT	pnvoltgn;				/* PnE70 : �o−�m�d�����o���x��									*/
	USHORT	ovlvl;					/* PnE71 : �n�u���o���x��											*/
	USHORT	regonlvl;				/* PnE72 : �񐶃I�����x��											*/
	USHORT	uvlvl;					/* PnE73 : �t�u���o���x��											*/
	USHORT	uvfil;					/* PnE74 : �t�u���o�t�B���^���萔									*/
	USHORT	regoffhys;				/* PnE78 : �񐶃I�t�q�X�e���V�X��									*/
	USHORT	vdetadfil;				/* PnE79 : ���H���o�`�c�p���[�p�X�t�B���^���萔					*/
	USHORT	vdefilsw;				/* PnE7A : ���H���o�t�B���^�I���X�C�b�`							*/
	USHORT	UvWrnLvl;				/* PnE7B : �t�u���[�j���O���o���x��									*/

	USHORT	VdetADMax;				/* PnE7C : PnE70[V]���͎���A/D�l */
	USHORT	ConvW;					/* PnE7F : �R���o�[�^�e��(�V�X�e��)									*/
#if 0 /* ��-7�s�v�̂��ߍ폜 *//* <S0F3> */
	USHORT	CnvCrtADNormMax;		/* PnE80 : �R���o�[�^�d�����oAD=32767���̓d���l */
	USHORT	CnvCrtRatAD;			/* PnE81 : �R���o�[�^�A���ߕ��׌��oAD�l(��) */
	USHORT	CnvCrtMaxAD;			/* PnE82 : �R���o�[�^�u���ߕ��׌��oAD�l(��) */
	USHORT	CnvCrtRatTime;			/* PnE83 : �R���o�[�^�A���ߕ��׌��o����(��)*/
	USHORT	CnvCrtMaxTime;			/* PnE84 : �R���o�[�^�u���ߕ��׌��o����(��) */
#endif /* ��-7�s�v�̂��ߍ폜 */
	/* <S154> Start */
	USHORT FanTemperatureOffset;    /* PnE80 : 	����FAN���������v�Z�p���͉��x�I�t�Z�b�g	*/
	USHORT FanTimeOffset;			/* PnE81 :  FAN�����I�t�Z�b�g����					*/
	USHORT RushLifeTime;			/* PnE82 :  �˖h��H����							*/
	/*  Start */
	ULONG RushSwichCount;			/* PnE84 :  ���݂̓˖h�����[�����				*/
//	USHORT RushSwichCount;			/* PnE83 :  ���݂̓˖h�����[����񐔁i����16bit�j		*/
//	USHORT DBLifeTime;				/* PnE84 :  DB��H����							*/
//	USHORT DBSwichCount;			/* PnE85 : 	���݂̌��݂�DB����񐔁i����16bit�j		*/
//	USHORT DRSwithingCount;			/* PnE86 :  ���݂̓˖h/DB�����[����񐔁i��ʁj 0,1���ځF
//												���݂̓˖h����񐔁i���8bit�j 2,3���ځF���݂�DB����񐔁i���8bit�j  */
	/* <S192> End */
	/* <S154> End */
	USHORT	ChgWaitTime;			/* PnE88 : �c�b�[�d�����҂�����	<S053>								*/
	USHORT	PrdyWaitTime;			/* PnE89 : �p���[���f�B�҂�����	<S053>								*/
	USHORT	DischCompVlt;			/* PnE8A : �}�����d�p��臒l�d��	<S1D1>								*/
	/* <S21D> Start */
	USHORT	regsynonlvl;			/* PnE9A : �񐶓������̉�ON���x�������l							*/
	USHORT	regsynontime;			/* PnE9B : �񐶓������̉�ON�҂�����								*/
	/* <S21D> End */
	USHORT	ctrlboardalmtmpr;		/* PnE9C : �����ُ퉷�x/�x�����x								*//* <S0E2> */
	USHORT	pwrboardalmtmpr;		/* PnE9D : �p���[��ُ퉷�x/�x�����x								*//* <S0E2> */
	USHORT	ctrlboardalmclrtmpr;	/* PnE9E : �����ُ�N���A�\���x/�ُ퉷�x���o����				*//* <S0E2> */
	USHORT	pwrboardalmclrtmpr;		/* PnE9F : �p���[��ُ�N���A�\���x/�ُ퉷�x���o����			*//* <S0E2> */

	USHORT	FanLockTime;			/* PnEE5 : �T�[�{�p�b�N���ԓ����t�@����~����						*//* <S0A3> */
	USHORT	SvonFanLock_Time;		/* PnEE6 : �T�[�{�I���������t�@����~�A���[�����o����				*/	/* <S219> */

#if 0 /* ��-7�s�v�̂��ߍ폜 *//* <S0F3> */
	USHORT	ohfil;					/* PnEF0 : ���x�ߔM���o�t�B���^���萔								*/
	SHORT	mbohlvl;				/* PnEF1 : ���j�b�g�������x�ߔM�A���[�����x��						*/
	SHORT	convohlvl;				/* PnEF2 : �R���o�[�^���x�ߔM�A���[�����x��							*/
	SHORT	ifohlvl;				/* PnEF3 : �������x�ߔM�A���[�����x��							*/
	USHORT	ohwrnlvl;				/* PnEF4 : ���x�ߔM���[�j���O���x��									*/
#endif /* ��-7�s�v�̂��ߍ폜 */
	USHORT	DcBaseLvl;				/* PnEF5 : ���H�d�����o�␳�p ��d��							*/

	ULONG	SeqInputInf;			/* PnEF6 */
	ULONG	SeqOutputInf;			/* PnEF8 */
} CMN_PRMDATA;



/****************************************************************************
	�p�����[�^�f�[�^�\���̒�`
****************************************************************************/
typedef struct PRMDATA
{
/*------------------------------------------------------------------------------------------------------*/
	USHORT	b_prm0;					/* Pn000 : �@�\�I����{�X�C�b�`�O									*/
	USHORT	b_prm1;					/* Pn001 : �@�\�I�����p�X�C�b�`�P									*/
	USHORT	b_prm2;					/* Pn002 : �@�\�I�����p�X�C�b�`�Q									*/
//	USHORT	b_prm6;					/* Pn006 : �@�\�I�����p�X�C�b�`�U									*//* <S103> */
//	USHORT	b_prm7;					/* Pn007 : �@�\�I�����p�X�C�b�`�V									*//* <S103> */
	USHORT	b_prm8;					/* Pn008 : �@�\�I�����p�X�C�b�`�W									*/
	USHORT	b_prm9;					/* Pn009 : �@�\�I�����p�X�C�b�`�X									*/
	USHORT	b_prmA;					/* Pn00A : �@�\�I�����p�X�C�b�`�`									*//* <S0A6> */
	USHORT	b_prmB;					/* Pn00B : �@�\�I�����p�X�C�b�`�a									*/
	USHORT	b_prmC;					/* Pn00C : �@�\�I�����p�X�C�b�`�b									*//* <S0A6> */
	USHORT	b_prmD;					/* Pn00D : �@�\�I�����p�X�C�b�`�c									*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	FullcSlot;				/* Pn040 : �t���N���[�Y�G���R�[�_�ڑ��X���b�g�ԍ�					*/
	USHORT	b_prm80;				/* Pn080 : �@�\�I�����p�X�C�b�`�W�O									*/
	USHORT	b_prm81;				/* Pn081 : �@�\�I�����p�X�C�b�`�W�P									*//* <S0A6> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	loophz;					/* Pn100 : ���x���[�v�Q�C��											*/
	USHORT	pitime;					/* Pn101 : ���x���[�v�ϕ����萔										*/
	USHORT	posgn;					/* Pn102 : �ʒu���[�v�Q�C��											*/
	USHORT	jrate;					/* Pn103 : �������[�����g��											*/
	USHORT	loophz2;				/* Pn104 : ��Q���x���[�v�Q�C��										*/
	USHORT	pitime2;				/* Pn105 : ��Q���x���[�v�ϕ����萔									*/
	USHORT	posgn2;					/* Pn106 : ��Q�ʒu���[�v�Q�C��										*/
	USHORT	ffgn;					/* Pn109 : �t�B�[�h�t�H���[�h										*/
	USHORT	fffil;					/* Pn10A : �t�B�[�h�t�H���[�h�t�B���^���萔							*/
	USHORT	gnmode;					/* Pn10B : �Q�C���֌W���p�X�C�b�`									*/
	USHORT	mdswlv;					/* Pn10C : ���[�h�X�C�b�`(�g���N�w��)								*/
	USHORT	mswrf;					/* Pn10D : ���[�h�X�C�b�`(���x�w��)									*/
	USHORT	mswacc;					/* Pn10E : ���[�h�X�C�b�`(�����x)									*/
	USHORT	msplvl;					/* Pn10F : ���[�h�X�C�b�`(�ʒu�΍�)									*/
	USHORT	pitfil;					/* Pn11F : �ʒu�ϕ����萔											*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	dobgn;					/* Pn121 : ���C�⏞�Q�C��											*/
	USHORT	dobgn2;					/* Pn122 : ��Q���C�⏞�Q�C��										*/
	USHORT	dtrqgn;					/* Pn123 : ���C�⏞�W��												*/
	SHORT	dlpfil;					/* Pn124 : ���C�⏞���g���␳										*/
	USHORT	dobjgn;					/* Pn125 : ���C�⏞�Q�C���␳										*/
	USHORT	evobgn;					/* Pn127 : ���x�I�u�U�[�o�Q�C��										*/
	USHORT	evobjgn;				/* Pn128 : ���x�I�u�U�[�o�ʒu�⏞�Q�C��								*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	gnswfil1;				/* Pn131 : �Q�C���؂�ւ����ԂP										*/
	USHORT	gnswfil2;				/* Pn132 : �Q�C���؂�ւ����ԂQ										*/
	USHORT	gnswwait1;				/* Pn135 : �Q�C���؂�ւ��҂����ԂP									*/
	USHORT	gnswwait2;				/* Pn136 : �Q�C���؂�ւ��҂����ԂQ									*/
	USHORT	gnsw1;					/* Pn139 : �����Q�C���؂�ւ��֌W�X�C�b�`�P							*/
	USHORT	Pn13B;					/* Pn13B : ���x�I�u�U�[�o���[�p�X�t�B���^���萔						*/
	USHORT	KcGainRatio;			/* Pn13D : �d���Q�C�����x��											*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	mdlsw;					/* Pn140 : ���f���Ǐ]����֘A�X�C�b�`								*/
	USHORT	mdlgn;					/* Pn141 : ���f���Ǐ]����Q�C�� [0.1/s]								*/
	USHORT	mdlzeta;				/* Pn142 : ���f���Ǐ]����Q�C���␳									*/
	USHORT	mdlptff;				/* Pn143 : ���f���Ǐ]����o�C�A�X(���]����)							*/
	USHORT	mdlntff;				/* Pn144 : ���f���Ǐ]����o�C�A�X(�t�]����)							*/
	USHORT	mdlantfrq;				/* Pn145 : �U���}���P���g���`										*/
	USHORT	mdlresfrq;				/* Pn146 : �U���}���P���g���a										*/
	USHORT	mdlvff;					/* Pn147 : ���f���Ǐ]���䑬�x�t�B�[�h�t�H���[�h�⏞					*/
	USHORT	mdlgn2;					/* Pn148 : ��Q���f���Ǐ]����Q�C�� [0.1/s]							*/
	USHORT	mdlzeta2;				/* Pn149 : ��Q���f���Ǐ]����Q�C���␳								*/
	USHORT	ff_frq;					/* Pn14A : �U���}���Q���g��											*/
	USHORT	ff_fil;					/* Pn14B : �U���}���Q�␳											*/
	USHORT	mfctype;				/* Pn14F : ����֘A�X�C�b�`											*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	avibsw;					/* Pn160 : ���U����֘A�X�C�b�`										*/
	USHORT	avibfrq;				/* Pn161 : �`�^���U���g��											*/
	USHORT	avibgn;					/* Pn162 : �`�^���U�Q�C���␳										*/
	USHORT	avibdamp;				/* Pn163 : �`�^���U�_���s���O�Q�C��									*/
	SHORT	avibfil1;				/* Pn164 : �`�^���U�t�B���^���萔�P�␳								*/
	SHORT	avibfil2;				/* Pn165 : �`�^���U�t�B���^���萔�Q�␳								*/
	USHORT	avibdamp2;				/* Pn166 : 3�������U�_���s���O�Q�C��				<S03E>		*/
	SHORT	avibfrq2;				/* Pn167 : 3�������U���g��						<S03E>		*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	DatLevel;				/* Pn170 : �_�C�i�~�b�N�I�[�g�`���[�j���O�֘A�X�C�b�`				*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	jrateSw;				/* Pn17A : �\��p�����[�^�i�C�i�[�V����؂�ւ��֌W�X�C�b�`�j		*//* <S0A6> */
	USHORT	jrate2;					/* Pn17B : ��2�C�i�[�V����											*//* <S0A6> */
	USHORT	jrateChgTime1;			/* Pn17C : �C�i�[�V����؂�ւ����ԂP								*//* <S0A6> */
	USHORT	jrateChgTime2;			/* Pn17D : �C�i�[�V����؂�ւ����ԂQ								*//* <S0A6> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	mswrf_l;				/* Pn181 : ���[�h�X�C�b�`(���x�w��)									*/
	USHORT	mswacc_l;				/* Pn182 : ���[�h�X�C�b�`(�����x)									*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	divlessSw;				/* Pn190 : �\��p�����[�^�i�΍����X�E��֘A�X�C�b�`�j					*//* <S0A6> */
	USHORT	velffgain;				/* Pn191 : ���x�t�B�[�h�t�H���[�h�Q�C��								*//* <S0A6> */
	USHORT	trqffgain;				/* Pn192 : �g���N�t�B�[�h�t�H���[�h�Q�C��							*//* <S0A6> */
	USHORT	trqfftime;				/* Pn193 : �g���N�t�B�[�h�t�H���[�h�t�B���^���萔					*//* <S0A6> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	prstsw;					/* Pn200 : �ʒu����w�ߌ`�ԑI���X�C�b�`								*/
	USHORT	mtlmt;					/* Pn205 : �}���`�^�[�����~�b�g�ݒ�									*/
	USHORT	cmfilsw;				/* Pn207 : �ʒu�w�ߋ@�\�X�C�b�`										*/
	USHORT	expbias1;				/* Pn209 : �ʒu�w�߉������o�C�A�X									*/
	ULONG	fcplsml;				/* Pn20A : �O�t���X�P�[���s�b�`��									*/
	ULONG	ratb2l;					/* Pn20E : �d�q�M�A��(���q)											*/
/*------------------------------------------------------------------------------------------------------*/
	ULONG	rata2l;					/* Pn210 : �d�q�M�A��(����)											*/
	ULONG	pgrat2l;				/* Pn212 : �o�f������(�p���X�P��)									*/
	USHORT	expfil;					/* Pn216 : �ʒu�w�߉��������萔										*/
	USHORT	mafil;					/* Pn217 : �ʒu�w�߈ړ����ώ���										*/
	USHORT	MulFrq;					/* Pn218 : �w�߃p���X���͔{��										*/
	USHORT	highrad;				/* Pn21B : HIGH filter average rad                                 *//*<S0C3>*/
	/*------------------------------------------------------------------------------------------------------*/
	USHORT	prefHfilacctime;		/* Pn21B : �ʒu�w�ߍ����t�B���^����������							*//* <S0A6> */
	USHORT	prefHfilsmooth;			/* Pn21C : �ʒu�w�ߍ����t�B���^�X���[�W���O����						*//* <S0A6> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT  fcsel;					/* Pn22A : �t���N���[�Y����I���X�C�b�`								*//* <S04D> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	cmctrlexsw;				/* Pn230 : �ʒu����g���@�\�X�C�b�`									*/
	LONG	blcm2l;					/* Pn231 : �o�b�N���b�V�␳��										*/
	USHORT	blfiltime;				/* Pn233 : �o�b�N���b�V�␳���萔									*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	perrclrsigtime;			/* Pn240 : �ʒu�΍��N���A�M�����͍ŏ����ԊԊu						*//* <S0A6> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	encbaudrate;			/* Pn250 : �G���R�[�_�ʐM���x�I��									*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	scalepgrat;				/* Pn281 : �G���R�[�_�o�͕���\										*/
	ULONG	scalepitch2l;			/* Pn282 : ���j�A�X�P�[���s�b�`										*/
/*------------------------------------------------------------------------------------------------------*/
	ULONG	psencernum;				/* Pn2D0 : �\��p�����[�^�i�ʒu�Z���T�o�͐��j						*//* <S0A6> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	vrefgn;					/* Pn300 : ���x�w�ߓ��̓Q�C��										*/
	USHORT	speed1;					/* Pn301 : �����ݒ葬�x1(��])										*/
	USHORT	speed2;					/* Pn302 : �����ݒ葬�x2(��])										*/
	USHORT	speed3;					/* Pn303 : �����ݒ葬�x3(��])										*/
	USHORT	jogspd;					/* Pn304 : �i�n�f���x(��])											*/
	USHORT	sfsacc;					/* Pn305 : �\�t�g�X�^�[�g��������									*/
	USHORT	sfsdec;					/* Pn306 : �\�t�g�X�^�[�g��������									*/
	USHORT	srfil;					/* Pn307 : ���x�w�߃t�B���^���萔									*/
	USHORT	fbfil;					/* Pn308 : ���x�e/�a�t�B���^���萔									*/
	USHORT	svoffdectime;			/* 0x30A : �T�[�{�I�t�y�ы�����~���̌�������						*//* <S0A6> */
	USHORT	vrefmafil;				/* 0x30B : ���x�w�߈ړ����ώ���										*//* <S0A6> */
	USHORT	vffmafil;				/* 0x30C : ���x�t�B�[�h�t�H���[�h�ړ����ώ���						*//* <S0DF> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	vibsel;					/* Pn310 : �U�����o�X�C�b�`											*/
	USHORT	vibsens;				/* Pn311 : �U�����o���x												*/
	USHORT	vibnorm;				/* Pn312 : �U�����o���x��(��])										*/
	USHORT	maxvel;					/* 0x316 : ���[�^�ō����x(��])										*//* <S0DF> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	Pn324;					/* Pn324 : �������[�����g����J�n���x��								*/
	USHORT	Pn325;					/* Pn325 : �������[�����g���蒆�̐U�����o���x��(��])				*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	speed1_l;				/* Pn380 : �����ݒ葬�x1(���j�A)									*/
	USHORT	speed2_l;				/* Pn381 : �����ݒ葬�x2(���j�A)									*/
	USHORT	speed3_l;				/* Pn382 : �����ݒ葬�x3(���j�A)									*/
	USHORT	jogspd_l;				/* Pn383 : �i�n�f���x(���j�A)										*/
	USHORT	vibnorm_l;				/* Pn384 : �U�����o���x��(���j�A)									*/
	USHORT	maxvel_l;				/* Pn385 : ���[�^�ō����x(���j�A)									*/
	USHORT	Pn387;					/* Pn386 : �������[�����g���蒆�̐U�����o���x��(���j�A)				*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	trefgn;					/* Pn400 : �g���N�w�ߓ��̓Q�C��										*/
	USHORT	trqfil11;				/* Pn401 : �P�i�ڑ�P�g���N�w�߃t�B���^���萔						*/
	USHORT	tlmtf;					/* Pn402 : ���]�g���N����											*/
	USHORT	tlmtr;					/* Pn403 : �t�]�g���N����											*/
	USHORT	outlmf1;				/* Pn404 : ���]���O���g���N����										*/
	USHORT	outlmr1;				/* Pn405 : �t�]���O���g���N����										*/
	USHORT	emgtrq;					/* Pn406 : ����~�g���N											*/
	USHORT	tcrlmt;					/* Pn407 : �g���N���䎞�̑��x����									*/
	USHORT	tfuncsw;				/* Pn408 : �g���N�֌W�@�\�X�C�b�`									*/
	USHORT	ntchfil1;				/* Pn409 : �P�i�ڃm�b�`�t�B���^���g��								*/
	USHORT	ntchq1;					/* Pn40A : �P�i�ڃm�b�`�t�B���^�p�l									*/
	USHORT	ntchdamp1;				/* Pn40B : �P�i�ڃm�b�`�t�B���^�[��									*/
	USHORT	ntchfil2;				/* Pn40C : �Q�i�ڃm�b�`�t�B���^���g��								*/
	USHORT	ntchq2;					/* Pn40D : �Q�i�ڃm�b�`�t�B���^�p�l									*/
	USHORT	ntchdamp2;				/* Pn40E : �Q�i�ڃm�b�`�t�B���^�[��									*/
	USHORT	secfil;					/* Pn40F : �Q�i�ڂQ���g���N�w�߃t�B���^���g��						*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	secq;					/* Pn410 : �Q�i�ڂQ���g���N�w�߃t�B���^�p�l							*/
	USHORT	trqfil12;				/* Pn412 : �P�i�ڑ�Q�g���N�w�߃t�B���^���萔						*/
	USHORT	treffil;				/* Pn415 : T-REF�t�B���^���萔										*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	ntchfil3;				/* Pn416 : 3�i�ڃm�b�`�t�B���^���g��								*//* <S0A6> */
	USHORT	ntchq3;					/* Pn417 : 3�i�ڃm�b�`�t�B���^Q�l									*//* <S0A6> */
	USHORT	ntchdamp3;				/* Pn418 : 3�i�ڃm�b�`�t�B���^�[��									*//* <S0A6> */
	USHORT	ntchfil4;				/* Pn419 : 4�i�ڃm�b�`�t�B���^���g��								*//* <S0A6> */
	USHORT	ntchq4;					/* Pn41A : 4�i�ڃm�b�`�t�B���^Q�l									*//* <S0A6> */
	USHORT	ntchdamp4;				/* Pn41B : 4�i�ڃm�b�`�t�B���^�[��									*//* <S0A6> */
	USHORT	ntchfil5;				/* Pn41C : 5�i�ڃm�b�`�t�B���^���g��								*//* <S0A6> */
	USHORT	ntchq5;					/* Pn41D : 5�i�ڃm�b�`�t�B���^Q�l									*//* <S0A6> */
	USHORT	ntchdamp5;				/* Pn41E : 5�i�ڃm�b�`�t�B���^�[��									*//* <S0A6> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	tfuncsw2;				/* Pn41F : �g���N(����)�֌W�@�\�X�C�b�`�Q							*//* <S0A6> */
	USHORT	RippleCompSw;			/* Pn423 : �g���N�⏞�֌W�@�\�X�C�b�`								*//* <S0A6> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	UvTrqLmt;				/* Pn424 : ���H�d���~�����g���N���� [%]							*/
	USHORT	UvTrqLmtOffFil;			/* Pn425 : ���H�d���~�����g���N������������						*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	tffmafil;				/* 0x426 : �g���N�t�B�[�h�t�H���[�h�ړ����ώ���						*//* <S0DF> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	VelRippleCompSpd;		/* Pn427 : ���x���b�v���⏞�L�����x									*/	/* <S14B> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	swpminfrq;				/* Pn453 : �|���J�n���g��											*/
	USHORT	swpmaxfrq;				/* Pn454 : �|���I�����g��											*/
	USHORT	detfrqlmt;				/* Pn455 : ���o���U���g�������l										*/
	USHORT	ezffttrq;				/* Pn456 : �|���g���N�w�ߐU��										*/
	USHORT	ezfftsw;				/* Pn457 : �e�e�s�֌W�@�\�X�C�b�`									*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	anotchsw;				/* Pn460 : �m�b�`�t�B���^�����X�C�b�`								*/
	USHORT	anotchsw2;				/* Pn466 : �m�b�`�t�B���^�����X�C�b�`2								*//* <S0A6> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	gcomptrq;				/* Pn470 : �d�͕⏞�g���N											*//* <S0A6> */
	USHORT	pqfriqcomptrq;			/* Pn471 : �������N�[�������C�⏞�g���N								*//* <S0A6> */
	USHORT	nqfriqcomptrq;			/* Pn472 : �������N�[�������C�⏞�g���N								*//* <S0A6> */
	USHORT	vfrigcomptrq;			/* Pn473 : �S�����C�⏞�g���N										*//* <S0A6> */
	USHORT	spdhys;					/* Pn474 : �����ݒ葬�xhysteresis									*//* <S0CD> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	tcrlmt_l;				/* Pn480 : ���͐��䎞�̑��x����										*/
	USHORT	tlmtf_l;				/* Pn483 : ���]���͐���												*/
	USHORT	tlmtr_l;				/* Pn484 : �t�]���͐���												*/

	MPFIND_UPRM PdetP;				/* Pn481 - Pn49B : ���Ɍ��o�p�����[�^ 								*/

	/*------------------------------------------------------------------------------------------------------*/
	USHORT	clamp;					/* Pn501 : �[���N�����v���x��										*/
	USHORT	tgonlv;					/* Pn502 : ��]���o���x��											*/
	USHORT	vcmplv;					/* Pn503 : ���x��v�M���o�͕�										*/
	USHORT	brktim;					/* Pn506 : �u���[�L�w��−�T�[�{�I�t�x�ꎞ��							*/
	USHORT	brkspd;					/* Pn507 : �u���[�L�w�ߏo�͑��x���x��								*/
	USHORT	brkwai;					/* Pn508 : �T�[�{�I�t−�u���[�L�w�ߑ҂�����							*/
	USHORT	insel1;					/* Pn50A : ���͐M���I��1											*/
	USHORT	insel2;					/* Pn50B : ���͐M���I��2											*/
	USHORT	insel3;					/* Pn50C : ���͐M���I��3											*/
	USHORT	insel4;					/* Pn50D : ���͐M���I��4											*/
	USHORT	outsel1;				/* Pn50E : �o�͐M���I��1											*/
	USHORT	outsel2;				/* Pn50F : �o�͐M���I��2											*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	outsel3;				/* Pn510 : �o�͐M���I��3											*/
	USHORT	inselA;					/* Pn511 : ���͐M���I��5											*/
//	USHORT	sorev;					/* Pn512 : �o�͐M�����]�ݒ�											*//* <S04B> */
	USHORT	sorev1;					/* Pn512 : �o�͐M�����]�ݒ�1										*//* <S04B> */
//	USHORT	outselA;				/* Pn513 : �o�͐M���I��4											*//* <S04B> */
	USHORT	sorev2;					/* Pn513 : �o�͐M�����]�ݒ�2										*//* <S04B> */
	USHORT	outsel4;				/* 0x514 : �o�͐M���I��5											*//* <S0A6> */
	USHORT	insel5;					/* Pn515 : ���͐M���I��6											*/
	USHORT	insel6;					/* 0x516 : ���͐M���I��7											*//* <S0A6> */

	USHORT	outsel6;				/* Pn517 : �o�͐M���I��6											*/
	USHORT	outsel7;				/* Pn518 : �o�͐M���I��7											*/
	USHORT	insel7;					/* Pn519 : ���͐M���I��8											*//* <S0A6> */
	ULONG	fpgoverlv2l;			/* Pn51B : ���[�^-���׈ʒu�ԕ΍��ߑ匟�o���x��						*/
	USHORT	overwrnlv;				/* Pn51E : �ʒu�΍��ߑ像�[�j���O���x��								*/
/*------------------------------------------------------------------------------------------------------*/
	ULONG	overlv2l;				/* Pn520 : �ʒu�΍��ߑ�A���[�����x��								*/
	ULONG	coinlv2l;				/* Pn522 : �ʒu���ߊ�����											*/
	ULONG	nearlv2l;				/* Pn524 : �m�d�`�q�M����											*/
	ULONG	svonoverlv2l;			/* Pn526 : �T�[�{�I�����ʒu�΍��ߑ�A���[�����x��					*/
	USHORT	svonoverwrnlv;			/* Pn528 : �T�[�{�I�����ʒu�΍��ߑ像�[�j���O���x��					*/
	USHORT	svonspdlmtlv;			/* Pn529 : �T�[�{�I�������x�������x��								*/
	USHORT	fpgcutrate;				/* Pn52A : �t���N���[�Y1��]������̏�Z�l							*/
	USHORT	olwarlvl;				/* Pn52B : �ߕ��׃��[�j���O���x��									*/
	USHORT	mtbastrq_drt;			/* Pn52C : ���[�^�ߕ��׌��o�x�[�X�d���f�B���[�e�B���O				*/
	USHORT	svbastrq_drt;			/* Pn52D : �P���d�����͑I�����T�[�{�p�b�N�ߕ��׌��o�x�[�X�d���f�B���[�e�B���O	*//* <S0A6> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	powon_mon	;			/* 0x52F : �d���������̃��j�^�\��									*//* <S0A6> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	pjogsw;					/* Pn530 : �v���O�����i�n�f�^�]�֌W�X�C�b�`							*/
	ULONG	pjogdistl;				/* Pn531 : �v���O�����i�n�f�ړ�����									*/
	USHORT	pjogrotspd;				/* Pn533 : �v���O�����i�n�f�ړ����x									*/
	USHORT	pjogacctm;				/* Pn534 : �v���O�����i�n�f����������								*/
	USHORT	pjogwaittm;				/* Pn535 : �v���O�����i�n�f�҂�����									*/
	USHORT	pjognum;				/* Pn536 : �v���O�����i�n�f�ړ���									*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	outalm_no1 ;			/* Pn53A : �M���o�̓A���[���E���[�j���O�ԍ�1						*//* <S0A6> */
	USHORT	outalm_no2 ;			/* Pn53B : �M���o�̓A���[���E���[�j���O�ԍ�2						*//* <S0A6> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	adatgmax;				/* Pn540 : �Q�C�����~�b�g											*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	trcalm_no;				/* Pn548 : �g���[�X�w��A���[���ԍ�									*//* <S0A6> */
/*------------------------------------------------------------------------------------------------------*/
//	USHORT	monoffset1;				/* Pn550 : �A�i���O���j�^�P�I�t�Z�b�g�d��							*//* <S0DF> */
//	USHORT	monoffset2;				/* Pn551 : �A�i���O���j�^�Q�I�t�Z�b�g�d��							*//* <S0DF> */
//	USHORT	mongain1;				/* Pn552 : �A�i���O���j�^1�{��										*//* <S0DF> */
//	USHORT	mongain2;				/* Pn553 : �A�i���O���j�^2�{��										*//* <S0DF> */
/*------------------------------------------------------------------------------------------------------*/
//	USHORT	powmon_unitime ;		/* Pn55A : ����d�̓��j�^�P�ʎ���									*//* <S0A6> *//* <S0DF> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	remdetw;				/* Pn560 : �c���U�����o��											*/
	USHORT	ovserrdetlvl;			/* Pn561 : �I�[�o�[�V���[�g���o���x��								*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	clamp_l;				/* Pn580 : �[���N�����v���x��										*/
	USHORT	tgonlv_l;				/* Pn581 : ��]���o���x��											*//* <S003> */
	USHORT	vcmplv_l;				/* Pn582 : ���x��v�M���o�͕�										*//* <S003> */
	USHORT	brkspd_l;				/* Pn583 : �u���[�L�w�ߏo�͑��x���x��								*/
	USHORT	svonspdlmtlv_l;			/* Pn584 : �T�[�{�I�������x�������x��								*/
	USHORT	pjoglinspd;				/* Pn585 : �v���O�����i�n�f�ō����x(���j�A)							*/
	USHORT	msrcra;					/* Pn586 : ���[�^������p��											*/
	USHORT	pldetsw;				/* Pn587 : ��Βl�X�P�[���p���Ɍ��o���s�I��							*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	pot_selsw;				/* Pn590 : POT���͐M���ݒ�											*/
	USHORT	not_selsw;				/* Pn591 : NOT���͐M���ݒ�											*/
	USHORT	dec_selsw;				/* Pn592 : DEC���͐M���ݒ�											*/
	USHORT	ext_selsw[3];			/* Pn593 : EXT���͐M���ݒ�											*/
//	USHORT	bk_selsw;				/* Pn596 : BK�o�͐M���ݒ�											*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	fstp_selsw;				/* Pn597 : <S1B5> FSTP���͐M���ݒ�									*/
	USHORT	pcl_selsw;				/* Pn598 : <S1B5> P-CL���͐M���ݒ�									*/
	USHORT	ncl_selsw;				/* Pn599 : <S1B5> N-CL���͐M���ݒ�									*/
	USHORT	clinkans_selsw;			/* Pn59A : <S1B5> CLINKANS���͐M���ݒ�								*/
	USHORT	cmcans_selsw;			/* Pn59B : <S1B5> CMCANS���͐M���ݒ�								*/
	USHORT	convready_selsw;		/* Pn59C : <S1B5> CONV_READY���͐M���ݒ�							*/
	USHORT	convmcon_selsw;			/* Pn59D : <S1B5> CONV_MCON���͐M���ݒ�								*/
	USHORT	coin_selsw;				/* Pn5B0 : <S1B5> COIN�o�͐M���ݒ�									*/
	USHORT	vcmp_selsw;				/* Pn5B1 : <S1B5> VCMP�o�͐M���ݒ�									*/
	USHORT	tgon_selsw;				/* Pn5B2 : <S1B5> TGON�o�͐M���ݒ�									*/
	USHORT	srdy_selsw;				/* Pn5B3 : <S1B5> S-RDY�o�͐M���ݒ�									*/
	USHORT	clt_selsw;				/* Pn5B4 : <S1B5> CLT�o�͐M���ݒ�									*/
	USHORT	vlt_selsw;				/* Pn5B5 : <S1B5> VLT�o�͐M���ݒ�									*/
	USHORT	bk_selsw;				/* Pn5B6 : <S1B5> BK�o�͐M���ݒ�									*/
	USHORT	warn_selsw;				/* Pn5B7 : <S1B5> WARN�o�͐M���ݒ�									*/
	USHORT	near_selsw;				/* Pn5B8 : <S1B5> NEAR�o�͐M���ݒ�									*/
	USHORT	cpulse_selsw;			/* Pn5B9 : <S1B5> C-PULSE�o�͐M���ݒ�								*/
	USHORT	clink_selsw;			/* Pn5BA : <S1B5> CLINK�o�͐M���ݒ�									*/
	USHORT	cmcon_selsw;			/* Pn5BB : <S1B5> CMCON�o�͐M���ݒ�									*/
	USHORT	pm_selsw;				/* Pn5BC : <S1B5> PM�o�͐M���ݒ�									*/
	USHORT	dao_selsw[2];			/* Pn5BD : <S1B5> DAO�o�͐M���ݒ�									*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	dbres;					/* Pn601 : �c�a��R�e��												*/
//	USHORT	dber;					/* Pn604 : �c�a��R�l												*//* <S0A6> *//* <S0F5> */
	USHORT	exdbr;					/* Pn604 : �c�a��R�l												*//* <S0F5> */
	USHORT	bbondiff0;				/* Pn620 : HWBB�M���^�C�~���O����									*/
	USHORT	saftyfuncSw;			/* Pn621 : ���S�@�\���p�I���X�C�b�`									*//* <S0A6> */
	USHORT	safeindecA;				/* Pn622 : �A�N�e�B�u���[�h���������萔A							*//* <S0A6> */
	USHORT	safeindecB;				/* Pn623 : �A�N�e�B�u���[�h���������萔B							*//* <S0A6> */
	USHORT	safeindecVel;			/* Pn624 : �A�N�e�B�u���[�h����������~���x							*//* <S0A6> */
	USHORT	safeStopTime;			/* Pn625 : �A�N�e�B�u���[�h��~�p������								*//* <S0A6> */
	ULONG	safePerrlvl;			/* Pn626 : �A�N�e�B�u���[�h�����ʒu�΍����x��						*//* <S0A6> */
	USHORT	safeVreflvl;			/* Pn628 : �A�N�e�B�u���[�h�������x�w�߃��x��						*//* <S0A6> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	sq_b_prm0;				/* Pn800 : SynqNet�@�\�I���X�C�b�`�O								*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	comCtrl;				/* Pn800 : �ʐM����@�\�I��											*/
	USHORT	sLimitSw;				/* Pn801 : �\�t�g���~�b�g�@�\�I��									*/
	USHORT	cmdCtrlSw;				/* Pn802 : �R�}���h����ݒ�											*/
	USHORT	zpWidth;				/* Pn803 : ���_�ʒu�͈�												*/
#if 0/* <S07F>:�^�ύX */
	USHORT	p_sLimitL;				/* Pn804 : �����\�t�g���~�b�g�l L									*/
	USHORT	p_sLimitH;				/* Pn805 : �����\�t�g���~�b�g�l H									*/
	USHORT	n_sLimitL;				/* Pn806 : �����\�t�g���~�b�g�l L									*/
	USHORT	n_sLimitH;				/* Pn807 : �����\�t�g���~�b�g�l H									*/
	USHORT	absOffsetL;				/* Pn808 : �A�u�\�o�f���_�ʒu�I�t�Z�b�g L							*/
	USHORT	absOffsetH;				/* Pn809 : �A�u�\�o�f���_�ʒu�I�t�Z�b�g H							*/
#else
	LONG	p_sLimitL;				/* Pn804 : �����\�t�g���~�b�g�l L									*/
	LONG	n_sLimitL;				/* Pn806 : �����\�t�g���~�b�g�l L									*/
	LONG	absOffsetL;				/* Pn808 : �A�u�\�o�f���_�ʒu�I�t�Z�b�g L							*/
#endif/* <S07F>:�^�ύX */
	USHORT	accRate1;				/* Pn80A : �P�i�ڒ��������萔										*/
	USHORT	accRate2;				/* Pn80B : �Q�i�ڒ��������萔										*/
	USHORT	accChgSpd;				/* Pn80C : �����萔�ؑւ����x										*/
	USHORT	decRate1;				/* Pn80D : �P�i�ڒ��������萔										*/
	USHORT	decRate2;				/* Pn80E : �Q�i�ڒ��������萔										*/
	USHORT	decChgSpd;				/* Pn80F : �����萔�ؑւ����x										*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	filExpBias;				/* Pn810 : �w���֐��������o�C�A�X									*/
	USHORT	filExpTb;				/* Pn811 : �w���֐����������萔										*/
	USHORT	filMaTb;				/* Pn812 : �ړ����ώ���												*/
	USHORT	resrv2;					/* Pn813 : �\��														*/
#if 0/* <S07F>:�^�ύX */
	USHORT	exDistL;				/* Pn814 : �O���ʒu���ߍŏI���s���� L								*/
	USHORT	exDistH;				/* Pn815 : �O���ʒu���ߍŏI���s���� H								*/
#else
	LONG	exDistL;				/* Pn814 : �O���ʒu���ߍŏI���s���� L								*/
#endif/* <S07F>:�^�ύX */
	USHORT	zretDir;				/* Pn816 : ���_���A���[�h�ݒ�										*/
	USHORT	zretSpd1;				/* Pn817 : ���_���A�A�v���[�`���x�P									*/
	USHORT	zretSpd2;				/* Pn818 : ���_���A�A�v���[�`���x�Q									*/
#if 0/* <S07F>:�^�ύX */
	USHORT	zretDistL;				/* Pn819 : ���_���A�ŏI���s���� L									*/
	USHORT	zretDistH;				/* Pn81A : ���_���A�ŏI���s���� H									*/
#else
	LONG	zretDistL;				/* Pn819 : ���_���A�ŏI���s���� L									*/
#endif/* <S07F>:�^�ύX */
	USHORT	resrv3;					/* Pn81B : �\��														*/
	USHORT	resrv4;					/* Pn81C : �\��														*/
	USHORT	resrv5;					/* Pn81D : �\��														*/
	USHORT	ioMonSel;				/* Pn81E : ���͐M�����j�^�I��										*/
	USHORT	optBitSw;				/* Pn81F : �R�}���h�f�[�^���t�I��									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	p_ltLimitL;				/* Pn820 : �������b�`�\�̈� L										*/
	LONG	n_ltLimitL;				/* Pn822 : �t�����b�`�\�̈� L										*/
//	USHORT	n_ltLimitH;				/* Pn823 : �t�����b�`�\�̈� H										*/
	USHORT	opMon1;					/* Pn824 : �I�v�V�������j�^�P�I��									*/
	USHORT	opMon2;					/* Pn825 : �I�v�V�������j�^�Q�I��									*/
	USHORT	decRate1Emg;			/* Pn826 : ����~�p�P�i�ڒ��������萔								*/
	USHORT	decRate2Emg;			/* Pn827 : ����~�p�Q�i�ڒ��������萔								*/
	USHORT	decChgSpdEmg;			/* Pn828 : ����~�p�����萔�ؑւ����x								*/
	USHORT	svOffTime;				/* Pn829 : SVOFF�҂�����(������~SVOFF��)							*/
	USHORT	optbit1;				/* Pn82A : OPTION�@�\�r�b�g���t�P									*/
	USHORT	optbit2;				/* Pn82B : OPTION�@�\�r�b�g���t�Q									*/
	USHORT	optbit3;				/* Pn82C : OPTION�@�\�r�b�g���t�R									*/
	USHORT	optbit4;				/* Pn82D : OPTION�@�\�r�b�g���t�S									*/
	USHORT	optbit5;				/* Pn82E : OPTION�@�\�r�b�g���t�T									*/
	USHORT	resrv6;					/* Pn82F : �\��														*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	rotrefL;				/* Pn830 : �@�B���]������̎w�ߒP�ʗ�L							*/
	USHORT	rotrefH;				/* Pn831 : �@�B���]������̎w�ߒP�ʗ�H							*/
	USHORT	st_num;					/* Pn832 : �@�B���]������̃X�e�[�V������							*/
	USHORT	rateChgSw;				/* Pn833 : POSING�萔�I��											*/
	ULONG	accRate12L;				/* Pn834 : �P�i�ڒ��������萔 2nd L									*/
	ULONG	accRate22L;				/* Pn836 : �Q�i�ڒ��������萔 2nd L									*/
	ULONG	accChgSpd2L;			/* Pn838 : �����萔�ؑւ����x 2nd L									*/
	ULONG	decRate12L;				/* Pn83A : �P�i�ڒ��������萔 2nd L									*/
	ULONG	decRate22L;				/* Pn83C : �Q�i�ڒ��������萔 2nd L									*/
	ULONG	decChgSpd2L;			/* Pn83E : �����萔�ؑւ����x 2nd L									*/
/*--------------------------------------------------------------------------------------------------*/
	ULONG	decRateEmg2L;			/* Pn840 : ����~�p�Q�i�ڒ��������萔�@2nd L						*/
	LONG	zretSpd12L;				/* Pn842 : ���_���A�A�v���[�`���x�P 2nd L							*/
	LONG	zretSpd22L;				/* Pn844 : ���_���A�A�v���[�`���x�Q 2nd L							*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	ltSeqNum;				/* Pn850 : �A�����b�`�V�[�P���X��									*/
	USHORT	ltLoopNum;				/* Pn851 : �A�����b�`��											*/
	USHORT	ltSeqSet1;				/* Pn852 : �A�����b�`�V�[�P���X�ݒ�1-4								*/
	USHORT	ltSeqSet2;				/* Pn853 : �A�����b�`�V�[�P���X�ݒ�4-8								*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	svcmdIoOut[VENDOR_IOPRM_NUM];	/* Pn855~: SVCMD_IO�o��Vendor Specific I/O�̈抄�t�@�\X				*//* <S1C0> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	cmdIo1;					/* Pn860 : SVCMD_IO(���͐M�����j�^)���t�@�\1						*/
	USHORT	cmdIo2;					/* Pn861 : SVCMD_IO(���͐M�����j�^)���t�@�\2						*/
	USHORT	cmdIo3;					/* Pn862 : SVCMD_IO(���͐M�����j�^)���t�@�\3						*/
	USHORT	cmdIo4;					/* Pn863 : SVCMD_IO(���͐M�����j�^)���t�@�\4						*/
	USHORT	cmdIo5;					/* Pn864 : SVCMD_IO(���͐M�����j�^)���t�@�\5						*/
	USHORT	cmdIo6;					/* Pn865 : SVCMD_IO(���͐M�����j�^)���t�@�\6						*/
	USHORT	cmdIo7;					/* Pn866 : SVCMD_IO(���͐M�����j�^)���t�@�\7						*/
	USHORT	cmdIoOut1;				/* Pn868 : SVCMD_IO(�o�͐M�����j�^)���t�@�\1						*/
	USHORT	cmdIoOut2;				/* Pn869 : SVCMD_IO(�o�͐M�����j�^)���t�@�\2						*/
	USHORT	cmdIoOut3;				/* Pn86A : SVCMD_IO(�o�͐M�����j�^)���t�@�\2						*//* <S1C0> */
	USHORT	p_zone1L;				/* Pn870 : ����ZONE1�M���o�͋��E�ʒu								*/
	USHORT	n_zone1L;				/* Pn872 : ����ZONE1�M���o�͋��E�ʒu								*/
	USHORT	p_zone2L;				/* Pn874 : ����ZONE2�M���o�͋��E�ʒu								*/
	USHORT	n_zone2L;				/* Pn876 : ����ZONE2�M���o�͋��E�ʒu								*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	nodeAdr;				/* Pn880 : �ǃA�h���X�ݒ�											*/
	USHORT	trByte;					/* Pn881 : �`���o�C�g��												*/
	USHORT	comTCycle;				/* Pn882 : �`������													*/
	USHORT	comAplCycle;			/* Pn883 : �ʐM����												*/
	USHORT	commErrBehavior;
//	USHORT	comErrSts1;
//	USHORT	comErrSts2;
	USHORT	comDataCtrl;
	USHORT	comErrCnt;				/* Pn88A : ��M�G���[�J�E���^										*/

/*------------------------------------------------------------------------------------------------------*/
 #if (CSW_NETIF_TYPE == NETIF_M2_TYPE)												/* MECATROLINK- M2	*/
	USHORT	comErrSts1;
	USHORT	comErrSts2;
/* <S17F>�^�Ɨv�f���ύX */
	USHORT	almCmd[16];				/* Pn890 : �A���[�����A�ʐMCMD�f�[�^								*//* <S0BD> */
	USHORT	almRsp[16];				/* Pn8A0 : �A���[�����A�ʐMRSP�f�[�^								*//* <S0BD> */
	USHORT	comCmd[16];				/* Pn8B0 : �I�����C���A�ʐMCMD�f�[�^								*//* <S0BD> */
	USHORT	comRsp[16];				/* Pn8C0 : �I�����C���A�ʐMRSP�f�[�^								*//* <S0BD> */
/*------------------------------------------------------------------------------------------------------*/
 #elif (CSW_NETIF_TYPE == NETIF_M3_TYPE)					/*  MECATROLINK-M3	*/
	ULONG	comErrSts1L;
	ULONG	comErrSts2L;
/* <S0CA> Start *//* <S17F>�^�Ɨv�f���ύX */
	USHORT	almCmd[24];				/* Pn890 : �A���[�����A�ʐMCMD�f�[�^								*//* <S0BD> */
	USHORT	almRsp[24];				/* Pn8A8 : �A���[�����A�ʐMRSP�f�[�^								*//* <S0BD> */
	USHORT	comCmd[24];				/* Pn8C0 : �I�����C���A�ʐMCMD�f�[�^								*//* <S0BD> */
	USHORT	comRsp[24];				/* Pn8D8 : �I�����C���A�ʐMRSP�f�[�^								*//* <S0BD> */
/* <S0CA> End */
 #endif
	/*--------------------------------------------------------------------------------------------------*/
	USHORT	bnkNum;					/* Pn900 : �o���N��													*/
	USHORT	bnkMemNum;				/* Pn901 : �o���N�����o��											*/
	/*--------------------------------------------------------------------------------------------------*/
	USHORT	bnkMember[15];			/* Pn902 : �o���N�����o												*//* <S080> */
	USHORT	bnkData[0x3F];			/* Pn920~Pn95F :�e�o���N�����o�l									*//* <S080> */
	/*--------------------------------------------------------------------------------------------------*/
	USHORT	prmVer;					/* Pn9E0 : �p�����[�^Ver.											*/
	USHORT	spdDivision;			/* Pn9E1 : ���x����\�ݒ�											*/
	USHORT	CmdOpSw;				/* Pn9E2 : Command-Option�@�\SW										*/
	USHORT	sys_rsrv2;				/* Pn9E3 : 															*/
	USHORT	sys_rsrv3;				/* Pn9E4 : 															*//*------------------------------------------------------------------------------------------------------*/
	USHORT	posing_mode;			/* Pn8FF : 	POSING���[�h�I��										*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	monAdr;					/* Pn980 : 	���ǃ��j�^�A�h���X 										*/
	USHORT	monDataSize;			/* Pn981 : 	���ǒʐM�f�[�^�T�C�Y									*/
	USHORT	monFuncSel;				/* Pn982 : 	���ǃ��j�^�@�\�I���X�C�b�`								*/
	USHORT	monDataSel;				/* Pn983 : 	��~����̓��͎w�߁@���j�^�ݒ�							*/
	USHORT	trceStpSpd;				/* Pn984 : 	�Ǐ]��~���x											*/
	USHORT	monAdj;					/* Pn985 : 	������~�����p�����[�^									*/
/*------------------------------------------------------------------------------------------------------*/
	ULONG	m3_encTypeL;			/* PnA02 : �G���R�[�_�^�C�v�I�� L									*/
	ULONG	m3_motorTypeL;			/* PnA04 : ���[�^�^�C�v�I�� L										*/
	ULONG	m3_fulcTypeL;			/* PnA06 : �Z�~�N���[�Y�^�t���N���[�Y�I�� L							*/
	ULONG	m3_ratSpdL;				/* PnA08 : ��i��]���x L											*/
	ULONG	m3_permitSpdL;			/* PnA0A : �ő�o�͉\���x L										*/
	ULONG	m3_spdMultiplierL;		/* PnA0C : ���x�搔 L												*/
	ULONG	m3_ratTrqL;				/* PnA0E : ��i�g���N L												*/
/*------------------------------------------------------------------------------------------------------*/
	ULONG	m3_permitTrqL;			/* PnA10 : �ő�o�͉\�g���N L										*/
	ULONG	m3_trqMultiplierL;		/* PnA12 : �g���N�搔 L												*/
	ULONG	m3_reslutionL;			/* PnA14 : ����\(���[�^��) L										*/
	ULONG	m3_scalepitchL;			/* PnA16 : ���j�A�X�P�[���s�b�` L									*/
	ULONG	m3_scalepitchH;			/* PnA17 : ���j�A�X�P�[���s�b�` H									*/
	ULONG	m3_pulsePerPitchL;		/* PnA18 : �X�P�[���s�b�`����̃p���X���i���j�A�j L					*/
/*------------------------------------------------------------------------------------------------------*/
	LONG	m3_ratbL;				/* PnA42 : �d�q�M�A��i���q�j										*/
	LONG	m3_rataL;				/* PnA44 : �d�q�M�A��i����j										*/
	LONG	m3_absOffsetL;			/* PnA46 : �A�u�\PG���_�ʒu�I�t�Z�b�g L								*/
	ULONG	m3_mtLimitL;			/* PnA48 : �}���`�^�[�����~�b�g�ݒ� L								*/
	ULONG	m3_limitSwL;			/* PnA4A : ���~�b�g�ݒ� L											*/
	LONG	m3_psLimitL;			/* PnA4C : �����\�t�g���~�b�g�l L									*/
	LONG	m3_reserve1L;			/* �\��	*/
/*------------------------------------------------------------------------------------------------------*/
	LONG	m3_nsLimitL;			/* PnA50 : �����\�t�g���~�b�g�l L									*/
	LONG	m3_reserve2L;			/* �\��	*/
/*------------------------------------------------------------------------------------------------------*/
	ULONG	m3_velUnitL;			/* PnA82 : ���x�P�ʑI�� L											*/
	ULONG	m3_velBase4L;			/* PnA84 : ���x��{�P�ʑI�� L										*/
	ULONG	m3_posUnitL;			/* PnA86 : �ʒu�P�ʑI�� L											*/
	ULONG	m3_posBaseL;			/* PnA88 : �ʒu��{�P�ʑI�� L										*/
	ULONG	m3_accUnitL;			/* PnA8A : �����x�P�ʑI�� L											*/
	ULONG	m3_accBase4L;			/* PnA8C : �����x��{�P�ʑI�� L										*/
	ULONG	m3_trqUnitL;			/* PnA8E : �g���N�P�ʑI�� L											*/
/*------------------------------------------------------------------------------------------------------*/
	ULONG	m3_trqBase4L;			/* PnA90 : �g���N��{�P�ʑI�� L										*/
	ULONG	m3_supportUnitL;		/* PnA92 : �Ή��P�ʌn L												*/
/*------------------------------------------------------------------------------------------------------*/
	ULONG	m3_KvL;					/* PnAC2 : ���x���[�v�Q�C�� L										*/
	ULONG	m3_TviL;				/* PnAC4 : ���x���[�v�ϕ����萔 L									*/
	ULONG	m3_KpL;					/* PnAC6 : �ʒu���[�v�Q�C�� L										*/
	ULONG	m3_FFGainL;				/* PnAC8 : �t�B�[�h�t�H���[�h�⏞ L									*/
	ULONG	m3_TpiL;				/* PnACA : �ʒu���[�v�ϕ����萔 L									*/
	ULONG	m3_psetWidthL;			/* PnACC : �ʒu���ߊ����� L											*/
	ULONG	m3_nearWidthL;			/* PnACE : �ʒu���ߋߖT��  L										*/
/*------------------------------------------------------------------------------------------------------*/
	ULONG	m3_filExpTbL;			/* PnB02 : �w���֐����������萔 L									*/
	ULONG	m3_filMaTbL;			/* PnB04 : �ړ����ώ��� L											*/
	LONG	m3_exDistL;				/* PnB06 : �O���M���ʒu���ߍŏI���s���� L							*/
	ULONG	m3_zretSpd1L;			/* PnB08 : ���_���A�A�v���[�`���x L									*/
	ULONG	m3_zretSpd2L;			/* PnB0A : ���_���A�N���[�v���x L									*/
	LONG	m3_zretDistL;			/* PnB0C : ���_���A�ŏI���s���� L									*/
	ULONG	m3_cPrmMon1L;			/* PnB0E : �Œ胂�j�^�I���P L										*/
/*------------------------------------------------------------------------------------------------------*/
	ULONG	m3_cPrmMon2L;			/* PnB10 : �Œ胂�j�^�I���Q L										*/
	ULONG	m3_selMon1L;			/* PnB12 : SEL_MON�̃��j�^�I���P L									*/
	ULONG	m3_selMon2L;			/* PnB14 : SEL_MON�̃��j�^�I���Q L									*/
	ULONG	m3_zpWidthL;			/* PnB16 : ���_���o�� L												*/
	ULONG	m3_pTrqLmtL;			/* PnB18 : ���]�g���N�����l L										*/
	ULONG	m3_nTrqLmtL;			/* PnB1A : �t�]�g���N�����l L										*/
	ULONG	m3_zSpdLvL;				/* PnB1C : �[�����x���o�� L											*/
	ULONG	m3_vCmpLvL;				/* PnB1E : ���x��v�M�����o�� L										*/
/*------------------------------------------------------------------------------------------------------*/
	ULONG	m3_enaCmdFeildL;		/* PnB20 : �T�[�{�R�}���h����t�B�[���h���̗L���^�����I�� L			*/
	ULONG	m3_enabStatFeildL;		/* PnB22 : �T�[�{�X�e�[�^�X����t�B�[���h���̗L���^�����I�� L		*/
	ULONG	m3_enaOutbitL;			/* PnB24 : I/O�r�b�g��`�̗L���^�����I���i�o�͑��j L				*/
	ULONG	m3_enaInbitL;			/* PnB26 : I/O�r�b�g��`�̗L���^�����I���i���͑��j L				*/
	ULONG	m3_Pn50A;				/* PnB28 : �����I�[�o�[�g���x���M���l���t�l L						*/
	ULONG	m3_Pn50B;				/* PnB29 : �����I�[�o�[�g���x���M���l���t�l L						*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	almmsk1;				/* PnE05 : �A���[���}�X�N�P											*/
	USHORT	almmsk2;				/* PnE06 : �A���[���}�X�N�Q											*/
	USHORT 	opdetsw;				/* PnE09 : �I�v�V�����J�[�h�^�Z�[�t�e�B�@�\�Ή������X�C�b�`			*//* <S04D> */
	USHORT	syssw1;					/* PnE0B : �V�X�e���X�C�b�`�P										*/
	USHORT	syssw3;					/* PnE0D : �V�X�e���X�C�b�`�R										*/
	USHORT	comfil;					/* PnE0E : �w�ߓ��̓t�B���^�I�����̎w�߃t�B���^�ݒ�					*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	paswrd;					/* PnE10 : �p�����[�^�p�X���[�h										*/
	DBYTEX	styp;					/* PnE11 : ����:�T�[�{�p�b�N�`���R�[�h, ���:���͓d���R�[�h			*/
	USHORT	ampw;					/* PnE12 : �T�[�{�p�b�N�e��											*/
	USHORT	yopt;					/* PnE13 : �x�d�l�ԍ�												*/
	USHORT	irats;					/* PnE14 : �T�[�{�p�b�N��i�d��										*/
	USHORT	imaxs;					/* PnE15 : �T�[�{�p�b�N�ő�d��										*/
	DBYTEX	ibs_md;					/* PnE17 : ����:�ߕ��׌��o�x�[�X�d��, ���:�ߕ��׌��o���ԓd��		*/
	DBYTEX	imdt_mxt;				/* PnE18 : ����:�ߕ��׌��o���ԓd������, ���:�ߕ��׌��o�ő�d������	*/
	USHORT	olwrat;					/* PnE19 : �n�k�x����												*/
	USHORT	dbr;					/* PnE1B : �c�a��R�l												*/
	USHORT	wdb;					/* PnE1D : �c�a��R�e��(�f�t�H���g)									*/
	DBYTEX	spgfail;				/* PnE1E : ����:S-PG��M���s���e��, ���:FS-PG��M���s���e��	*/
	DBYTEX	PnE1F;					/* PnE1F : ����:�˓���R�ߕ��חp�d���������e��, ���:(�\��)		*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	ignd;					/* PnE20 : �d�����[�v�Q�C��(D)										*/
	USHORT	ignq;					/* PnE21 : �d�����[�v�Q�C��(Q)										*/
	USHORT	kid;					/* PnE22 : �d�����[�v�ϕ����萔(D)									*/
	USHORT	kiq;					/* PnE23 : �d�����[�v�ϕ����萔(Q)									*/
	USHORT	limintd;				/* PnE24 : �d�����[�v�ϕ������l(D)									*/
	USHORT	limintq;				/* PnE25 : �d�����[�v�ϕ������l(Q)									*/
	USHORT	ifil;					/* PnE26 : �d�����[�v�t�B���^���萔									*/
	USHORT	shant;					/* PnE28 : �d�����o�Q�C��											*/
	USHORT	vrevgn;					/* PnE2A : �d���␳�Q�C��											*/
	USHORT	lmdata;					/* PnE2B : �d�������l												*/
	USHORT	pwmf;					/* PnE2C : �o�v�l���g��												*/
	DBYTEX	odt_c;					/* PnE2D : ����:�I���f�B���C����, ���:�I���f�B���C�⏞�萔			*/
	DBYTEX	odlv_slp;				/* PnE2E : ����:�I���f�B���C�⏞�ύX���x��, ���:�⏞�X��			*/
	USHORT	dq_sw;					/* PnE2F : �c�p�p�X�C�b�`											*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	phscmp;					/* PnE34 : �ʑ��⏞													*/
	USHORT	kdd;					/* PnE35 : �d�����[�v��������(D)									*/
	USHORT	kdq;					/* PnE36 : �d�����[�v��������(Q)									*/
	USHORT	dfild;					/* PnE37 : �d�����[�v�����t�B���^���萔(D)							*/
	USHORT	dfilq;					/* PnE38 : �d�����[�v�����t�B���^���萔(Q)							*/
	USHORT	dq_sw2;					/* PnE39 : �d������X�C�b�`�Q										*/
	USHORT	motresq;				/* PnE3B : ���[�^���U�}���p�m�b�`�t�B���^Q�l						*/
	USHORT	PnE3C;					/* PnE3C : N���X�C�b�`ON�ŏ�����									*/
	USHORT	PnE3D;					/* PnE3D : �T���v�����z�[���h����									*/
	USHORT	fltrange;				/* PnE3F : ���[�^�ߓd�����o�t�B���^��								*/ /* <S1F5> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	tplsfil;				/* PnE40 : DRV�����M���ُ팟�o�t�B���^��							*//* <S1F6> */
//	USHORT	mnrng0;					/* PnE42 : ���j�^�P�Q�C��											*//* <S016> *//* <S0DF> */
//	USHORT	mnrng1;					/* PnE43 : ���j�^�Q�Q�C��											*//* <S016> *//* <S0DF> */
	DBYTEX	ipg_rwt;				/* PnE44 : ����:i-P���䊄��, ���:�\�����o�g���N					*/
	USHORT	PnE45;					/* PnE45 : �ᑬ�n�k�F�x�[�X�d��[%]�C�n�k�����ϋȓ_[�d�C�p10r/min]	*/
	USHORT	PnE46;					/* PnE46 : �ᑬ�n�k�F���ԓd��[10%]�C�ő�d��[10%]					*/
	USHORT	PnE47;					/* PnE47 : �ᑬ�n�k�F���ԓd������[0.1s]�C�ő�d������[0.1s]			*/
//	ULONG /*USHORT*/	mnptr0l;	/* PnE48 : <S074> ���j�^�P�A�h���X									*//* <S0DF> */
//	ULONG /*USHORT*/	mnptr1l;	/* PnE4A : <S074> ���j�^�Q�A�h���X									*//* <S0DF> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	zadjiu;					/* PnE50 : �d�����o�[����(U��)										*/
	USHORT	zadjiv;					/* PnE51 : �d�����o�[����(V��)										*/
	DBYTEX	gadjiu_v;				/* PnE52 : �d�����o�Q�C����(U,V��)									*/
//	USHORT	zadjvl;					/* PnE53 : ���x�w�߃[����											*/	/* <S0A9> �f�[�^�^�ύX */
	LONG	zadjvl;					/* PnE53 : ���x�w�߃[����											*/	/* <S0A9> �f�[�^�^�ύX */
	USHORT	zadjvh;					/* PnE54 : �\��萔(����Ȃ��ł�������)								*/
	USHORT	gadjv;					/* PnE55 : ���x�w�߃Q�C����											*/
	DBYTEX	zadjtp_n;				/* PnE56 : �g���N�w�߃[����(������/������)							*/
	DBYTEX	gadjtp_n;				/* PnE57 : �g���N�w�߃Q�C����(������/������)						*/
/*------------------------------------------------------------------------------------------------------*/
	DBYTEX	ratbt_mt;				/* PnE60 : ����:�ߕ��׌��o�x�[�X�d��, ���:�ߕ��׌��o���ԓd��		*/
	DBYTEX	ratmtt_2;				/* PnE61 : ����:�ߕ��׌��o���Ԏ���, ���:�ߕ��׌��o���ԓd���Q		*/
	DBYTEX	ratmt2_os;				/* PnE62 : ����:�ߕ��׌��o���Ԏ��ԂQ, ���:�n�r���o���x��			*/
	USHORT	PnE63;					/* PnE63 : �A���[���g���[�X�@�\�I���X�C�b�`							*/
	USHORT	PnE64;					/* PnE64 : �A���[���g���[�X�@�\�T���v�����O�Ԋu						*/
	USHORT	PnE65;					/* PnE65 : �A���[���g���[�X�@�\�g���[�X�P�ΏۑI��					*/
	USHORT	PnE66;					/* PnE66 : �A���[���g���[�X�@�\�g���[�X�Q�ΏۑI��					*/
	USHORT	PnE67;					/* PnE67 : �A���[���g���[�X�@�\�g���[�X�R�ΏۑI��					*/
	USHORT	PnE68;					/* PnE68 : �A���[���g���[�X�@�\�g���[�X�P���A�h���X�w��				*/
	USHORT	PnE6A;					/* PnE6A : �A���[���g���[�X�@�\�g���[�X�Q���A�h���X�w��				*/
	USHORT	PnE6C;					/* PnE6C : �A���[���g���[�X�@�\�g���[�X�R���A�h���X�w��				*/
	/* <S192> Start */
/*------------------------------------------------------------------------------------------------------*/
	USHORT DBLifeTime;				/* PnE83 :  DB��H����							*/
	ULONG DBSwichCount;				/* PnE86 : 	���݂�DB�����				*//* <S192> */
	/* <S192> End */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	scnvorgoffset;			/* PnE8C : �V���A���ϊ����j�b�g���_�I�t�Z�b�g						*/
	USHORT	phsofst;				/* PnE8D : �d����ʑ��ʒu											*/
/*------------------------------------------------------------------------------------------------------*/
	DBYTEX	olspdlvl;				/* PnE90 : �ᑬ�ߕ��׌��o���x���x��									*/
	DBYTEX	ollspdprm;				/* PnE91 : �ᑬ�ߕ��׌��o�p�����[�^									*/
	USHORT	dbol_warn;				/* PnE93 : �c�a�ߕ��׌��o�d�͂P										*/
	USHORT	dbol_alm;				/* PnE94 : �c�a�ߕ��׌��o�d�͂Q										*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	ifil2;					/* PnEA4 : �É����[�h�d�����[�v�t�B���^���萔						*/
	USHORT	pwmf2;					/* PnEA5 : �É����[�hPWM���g��										*/
	USHORT	ilimlvl;				/* PnEA6 : �L�����A���g���؂�ւ��d��臒l							*/
	USHORT	pwmf3;					/* PnEA7 : ��2PWM���g��												*/
	USHORT	fbfil2;					/* PnEA8 : ��Q���x�e/�a�t�B���^���萔								*/
	USHORT	kv;						/* PnEB0 : �d���t�B�[�h�o�b�N���[�v���Q�C��						*/
	USHORT	tv;						/* PnEB1 : �d���t�B�[�h�o�b�N�ϕ����萔								*/
	USHORT	idlim;					/* PnEB2 : �d���t�B�[�h�o�b�N�ϕ����~�b�g							*/
	USHORT	v1max;					/* PnEB3 : �d���w�ߐ������x��										*/
	USHORT	vmaxid;					/* PnEB4 : d���d���w�߃��~�b�g�p�d���������x��						*/
	USHORT	vpnavg;					/* PnEB5 : ���H�d�����ώ���										*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	PnED0;					/* PnED0 : �����m�b�`�������										*/
	ULONG	PnED1;					/* PnED1 : �����m�b�`�ő�U���l										*/
	USHORT	PnED3;					/* PnED3 : �m�b�`���g���P��ڕ␳�W���P								*/
	USHORT	PnED4;					/* PnED4 : �m�b�`���g���P��ڕ␳�W���Q								*/
	USHORT	PnED5;					/* PnED5 : �m�b�`���g���P��ڕ␳�͈�臒l							*/
	USHORT	PnED6;					/* PnED6 : �m�b�`���g���Q��ڕ␳�W���P								*/
	USHORT	PnED7;					/* PnED7 : �m�b�`���g���Q��ڕ␳�W���Q								*/
	USHORT	PnED8;					/* PnED8 : �m�b�`���g���Q��ڕ␳�͈�臒l							*/
	USHORT	PnED9;					/* PnED9 : �����m�b�`�U���`�F�b�N���̒�~���ʑ҂�����				*/
	USHORT	TunLessGain;			/* PnEDA : ����ڽ�O����޻ް�޹޲�									*/
	USHORT	TunLessStiff;			/* PnEDB : ����ڽ�Ű���␳�޲�										*/
	USHORT	TunLessTrqLpf;			/* PnEDC : ����ڽ�O���ٸ۰�̨߽�����萔								*/
	USHORT	TunLessVfbLpf;			/* PnEDD : ����ڽ���xFB۰�̨߽�����萔								*/
/*------------------------------------------------------------------------------------------------------*/
	DBYTEX	MLess_vrat_max;			/* PnEE0 : ���[�^���X�e�X�g�@�\���z���[�^���x						*//* <S1B7> */
	DBYTEX	MLess_oslv_pol;			/* PnEE1 : ���[�^���X�e�X�g�@�\���z���[�^�n�r���o���x��, �|�[����	*//* <S1B7> */
	USHORT	Mless_jmot;				/* PnEE2 : ���[�^���X�e�X�g�@�\���z���[�^���[�^�C�i�[�V��			*//* <S1B7> */
	DBYTEX	Mless_exp_spd_jmot;		/* PnEE3 : ���[�^���X�e�X�g�@�\���z���[�^�w���p�����[�^				*//* <S1B7> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	dq_sw_L;				/* PnEE8 : �c�p�p�X�C�b�`(���j�A)									*/
	USHORT	v1max_L;				/* PnEE9 : �d���w�ߐ������x��(���j�A)								*/
	USHORT	TunLessGain_L;			/* PnEEA : ����ڽ�O����޻ް�޹޲�(���j�A)							*/
	USHORT	TunLessStiff_L;			/* PnEEB : ����ڽ�Ű���␳�޲�(���j�A)								*/
/*------------------------------------------------------------------------------------------------------*/
	MENCPRM	MencP;					/* PnF00 - PnF1E : �G���R�[�_�p�����[�^ */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	RippleCompGainE;		/* PnFA0 : ���x���b�v���⏞�Q�C��(�d�C�p)							*/	/* <S14B> */
	USHORT	RippleCompCycleE[6];	/* PnFA1, PnFA4, �c, PnFB0 : ���x���b�v���⏞��������(�d�C�p)		*/	/* <S14B> */
	SHORT	RippleCompAmplitudeE[6];/* PnFA2, PnFA5, �c, PnFB1 : ���x���b�v���⏞�����U��(�d�C�p)		*/	/* <S14B> */
	USHORT	RippleCompPhaseE[6];	/* PnFA3, PnFA6, �c, PnFB2 : ���x���b�v���⏞�����ʑ�(�d�C�p)		*/	/* <S14B> */
/*------------------------------------------------------------------------------------------------------*/

} PRMDATA;



#endif //__PN_PRM_STRUCT_H

