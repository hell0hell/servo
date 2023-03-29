/*****************************************************************************
Description	: Alarm Manager
-----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

-----------------------------------------------------------------------------
Changes		:
Name   Date          Description
-----------------------------------------------------------------------------
I.M.   2010.11.2     created

*****************************************************************************/
#ifndef ALARM_H
#define ALARM_H

/*--------------------------------------------------------------------------*/
/*		�A���[��������`													*/
/*--------------------------------------------------------------------------*/
#define ALMDEF_ATTR_WARNING		0x01		/* ���[�j���O(1),�A���[��(0)	*/
#define ALMDEF_ATTR_NOEEPROM	0x02		/* EEPROM��i�[(1),EEPROM�i�[(0)*/
#define ALMDEF_ATTR_COMALARM	0x04		/* �ʐMAlarm(1),�A���vAlarm(0)	*/
#define ALMDEF_ATTR_NOTRESET	0x08		/* ���Z�b�g�s��(1),���Z�b�g��(0)*/
#define ALMDEF_ATTR_MENCDANG	0x10		/* ���[�^�G���R�[�_�f�[�^����(1)*/
#define ALMDEF_ATTR_POSNG		0x20		/* �ʒu�f�[�^����(1)			*/
/*--------------------------------------------------------------------------*/
/*		�n�k�A���[������`												*/
/*--------------------------------------------------------------------------*/
#define ALMDEF_OLINFO_OLF		0x01		/* �ߕ���						*/
#define ALMDEF_OLINFO_RGOL		0x02		/* �񐶉ߕ���					*/
#define ALMDEF_OLINFO_DBOL		0x04		/* �c�a�ߕ���					*/
/*--------------------------------------------------------------------------*/
/*		�A���[���g��������`												*/
/*--------------------------------------------------------------------------*/
#define ALMDEF_M3ATTR_CMDALM	0x01		/* M3�R�}���h�A���[��(1)		*/
#define ALMDEF_M3ATTR_COMALM	0x02		/* M3�ʐM�A���[��(1)			*/
#define ALMDEF_EXATTR_INIALM	0x04		/* �N�������o�A���[��(1)		*/
#define ALMDEF_M3ATTR_CODEMASK	0xF0		/* M3�A���[���R�[�h�}�X�N		*/


/*****************************************************************************
*	�A���[��ID��`
*****************************************************************************/
typedef enum ALMID_T {
	NO_ALARM		= 0,		/* No Alarm */

	/* �T�[�{�A���[���h�c��` */
	ALM_EEPROM1,				/* A.020 : ���[�U�[�萔�`�F�b�N�T���ُ�							*/
	ALM_EEPROM2,				/* A.021 : �p�����[�^�t�H�[�}�b�g�ُ�							*/
	ALM_EEPROM3,				/* A.022 : �V�X�e���`�F�b�N�T���ُ�								*/
	ALM_EEPROM4,				/* A.023 : �p�����[�^�p�X���[�h�ُ�								*/
	ALM_BOOT1,					/* A.024 : Flash����N�����ُ�(�H�ꎎ���@����p�A���[��)		*//* �Í����t�H�[�}�b�g�Ή� <S146> */
	ALM_BOOT2,					/* A.025 : �����^�C���t�@�[�����ُ�(�~���[�N�����A���[��)		*//* �Í����t�H�[�}�b�g�Ή� <S146> */
	ALM_VDC,					/* A.030 : ���H���o���ُ�										*/
	ALM_PRM,					/* A.040 : �p�����[�^�ݒ�ُ�									*/
	ALM_DIVPRM,					/* A.041 : �����p���X�o�͐ݒ�ُ�								*/
	ALM_PRMUNMATCH,				/* A.042 : �p�����[�^�g���킹�ُ�								*/
	ALM_SEMIFULLPRM,			/* A.044 : �Z�~�N�t���N�p�����[�^�ݒ�ُ�						*/
	ALM_UNMATCH,				/* A.050 : ���[�^/�T�[�{�e�ʃA���}�b�`							*/
	ALM_NOTSUPPORT,				/* A.051 : ���i���T�|�[�g�A���[��								*/
	ALM_PWMAMP_SLOT,			/* A.052 : PWM�A���v�ڑ��A���[��								*/
	ALM_MODULE_CHANGE,			/* A.053 : ���W���[���\���ύX�A���[��							*/
	ALM_SCALE,					/* A.080 : �X�P�[���s�b�`�ݒ�ُ�								*/
	ALM_SVONCMD,				/* A.0B0 : �T�[�{�I���w�ߖ����A���[��							*/
	ALM_OC,						/* A.100 : �ߓd��(�n�b)											*/
	ALM_OVERCURRENT,			/* A.101 : ���[�^�ߓd�����o										*//* <S1F5> */
	ALM_RLYON_ACK,				/* A.230 : RLYON�A���T�[�ُ�									*/
	ALM_RG,						/* A.300 : �񐶈ُ�												*/
	ALM_RGOL,					/* A.320 : �񐶉ߕ���											*/
	ALM_WIR,					/* A.330 : ���H�z���G���[										*/
	/* <S21D> Start */
	ALM_CONVIO,					/* A.331 : �d���֘A���͐M���ُ�									*/
	ALM_CAPALINK,				/* A.332 : �R���f���T�o���N�ڑ��A���T�[�ُ�						*/
	ALM_CAPAMC,					/* A.333 : �R���f���T�o���NMC�A���T�[�ُ�						*/
	/* <S21D> End */
	ALM_OV,						/* A.400 : �ߓd��(�n�u)											*/
	ALM_PUV,					/* A.410 : �s���d��												*/
	ALM_CONVFLT,				/* A.42A : �R���o�[�^�ُ� for IDM								*/
	ALM_PCOV,					/* A.450 : ���H�R���f���T�ߓd��								*/
	ALM_OSF1,					/* A.510 : �ߑ��x(�n�r)											*/
	ALM_OSDIV,					/* A.511 : �����p���X�o�͉ߑ��x									*/
	ALM_VIB,					/* A.520 : �U���A���[��(���U�A���[��)							*/
	ALM_AT,						/* A.521 : �I�[�g�`���[�j���O�A���[��							*/
	ALM_VELPRM,					/* A.550 : �ō����x�ݒ�ُ�										*/
	ALM_OLF1,					/* A.710 : �ߕ���(�u���ő啉��)									*/
	ALM_OLF2,					/* A.720 : �ߕ���(�A���ő啉��)									*/
	ALM_DBOL,					/* A.730 : �c�a�ߕ���											*/
	ALM_DBOL_LVL,				/* A.731 : �c�a�ߕ���											*/
	ALM_RSOL,					/* A.740 : �˓���R�ߕ���										*/
	ALM_OH,						/* A.7A0 : �q�[�g�V���N�ߔM										*/
	ALM_CTRLBRD,				/* A.7A1 : �������x�ُ�1�i�������x�ُ�j					*//* <S0DE> */
	ALM_PWRBRD,					/* A.7A2 : �������x�ُ�2�i�p���[����x�ُ�j					*//* <S0DE> */
	ALM_INTTEMP,				/* A.7A3 : �������x���o���ُ�									*//* <S0DE> */
	ALM_CTRLBDOH,				/* A.7AA : �������x�ُ� for IDM								*/
	ALM_FANLOCK,				/* A.7AB : �T�[�{�p�b�N�����t�@���̒�~							*/
	ALM_PG1,					/* A.810 : �G���R�[�_�o�b�N�A�b�v�A���[��						*/
	ALM_PG2,					/* A.820 : �G���R�[�_�T���`�F�b�N�A���[��						*/
	ALM_PG3,					/* A.830 : �G���R�[�_�o�b�e���A���[��							*/
	ALM_PG4,					/* A.840 : �G���R�[�_�f�[�^�A���[��								*/
	ALM_PG5,					/* A.850 : �G���R�[�_�I�[�o�X�s�[�h								*/
	ALM_PG6,					/* A.860 : �G���R�[�_�I�[�o�q�[�g								*/
	ALM_SCL_ERR,				/* A.890 : �G���R�[�_�X�P�[���G���[								*/
	ALM_MOD_ERR,				/* A.891 : �G���R�[�_���W���[���G���[							*/
	ALM_FPGFSCALE,				/* A.8A0 : �t���N���[�Y�V���A���G���R�[�_�X�P�[���ُ�			*/
	ALM_FPGMODULE,				/* A.8A1 : �t���N���[�Y�V���A���G���R�[�_���W���[���ُ�			*/
	ALM_FPGSEN,					/* A.8A2 : �t���N���[�Y�V���A���G���R�[�_�Z���T�ُ�(�C���N�������^��)	*/
	ALM_FPGPOS,					/* A.8A3 : �e�o�f�|�W�V�����ُ�(�A�u�\)							*/
	ALM_FPGOVSPD,				/* A.8A5 : �t���N���[�Y�V���A���G���R�[�_�I�[�o�X�s�[�h�ُ�		*/
	ALM_FPGOVHEAT,				/* A.8A6 : �t���N���[�Y�V���A���G���R�[�_�I�[�o�[�q�[�g�ُ�		*/
	ALM_VREFADERR,				/* A.B10 : ���x�w�߂`�^�c�ُ�									*/
	ALM_VREFADCHK,				/* A.B11 : ���x�w�߂`�^�c�f�[�^�ُ�								*/
	ALM_TREFADERR,				/* A.B20 : �g���N�w�߂`�^�c�ُ�									*/
	ALM_CURAD_U,				/* A.B31 : �d�����o�ُ�P(�t��)									*/
	ALM_CURAD_V,				/* A.B32 : �d�����o�ُ�Q(�u��)									*/
	ALM_JL056_GUP1,				/* A.B33 : �d�����o�ُ�R(�d�����o��)							*/
	ALM_HWBB_UNSUPPORT,			/* A.B40 : �Q�[�g�h���C�u�ُ�									*/
	ALM_SAMP_OVER,				/* A.BF0 : �V�X�e���A���[���O(�����v���O���������ُ�)			*/
	ALM_STACK_OVF,				/* A.BF1 : �V�X�e���A���[���P(�����v���O��������ُ�)			*/
	ALM_UPROG,					/* A.BF2 : �V�X�e���A���[���Q(�d�����䏈�����v���O���������ُ�)	*/
	ALM_SCANA_STOP,				/* A.BF3 : �V�X�e���A���[���R(�G���R�[�_I/F�������ُ�)			*/
	ALM_JL056_GUP,				/* A.BF4 : �V�X�e���A���[���S(CPU WDT�ُ�)						*/
	ALM_RWF,					/* A.C10 : �\���h�~���o											*/
	ALM_PHASE,					/* A.C20 : �ʑ��댟�o											*/
	ALM_POLESENSOR,				/* A.C21 : �|�[���Z���T�ُ�										*/
	ALM_PHASEANGLE,				/* A.C22 : �ʑ����s��v										*/
	ALM_POLE,					/* A.C50 : ���Ɍ��o���s											*/
	ALM_PFINDSTOP,				/* A.C51 : ���Ɍ��o���I�[�o�[�g���x�����o						*/
	ALM_PDET,					/* A.C52 : ���Ɍ��o����											*/
	ALM_PDET_MOVEOVER,			/* A.C53 : ���Ɍ��o���͈͒���									*/
	ALM_PDET_NG,				/* A.C54 : ���Ɍ��o���s2										*/
	ALM_PGCLR,					/* A.C80 : �G���R�[�_�N���A�ُ�(�}���`�^�[�����~�b�g�ݒ�ُ�)	*/
	ALM_PGCOM,					/* A.C90 : �G���R�[�_�ʐM�ُ�									*/
	ALM_PGCOM1,					/* A.C91 : �G���R�[�_�ʐM�ʒu�f�[�^�����x�ُ�					*/
	ALM_PGCOM2,					/* A.C92 : �G���R�[�_�ʐM�^�C�}�ُ�								*/
	ALM_PGP,					/* A.CA0 : �G���R�[�_�p�����[�^�ُ�								*/
	ALM_PGECHO,					/* A.CB0 : �G���R�[�_�G�R�[�o�b�N�ُ�							*/
	ALM_MTLMT,					/* A.CC0 : �}���`�^�[�����~�b�g�l�s��v							*/
	ALM_FPGCOM,					/* A.CF1 : �t�B�[�h�o�b�N�I�v�V�����J�[�h�ʐM�ُ�(��M���s)		*/
	ALM_FPGCOMTIMER,			/* A.CF2 : �t�B�[�h�o�b�N�I�v�V�����J�[�h�ʐM�ُ�(�^�C�}��~)	*/
	ALM_OF,						/* A.D00 : �ʒu�΍��ߑ�											*/
	ALM_BEOF,					/* A.D01 : �T�[�{�I�����ʒu�΍��ߑ�								*/
	ALM_BEVLMTOF,				/* A.D02 : �T�[�{�I�������x�����ʒu�΍��ߑ�						*/
	ALM_FSOF,					/* A.D10 : ���[�^�|���׈ʒu�ԕ΍��ߑ�							*/
	ALM_POSOF,					/* A.D30 : �ʒu�f�[�^�I�[�o�[									*/
	ALM_OPSET,					/* A.E00 : �I�v�V���������A�N�Z�X�v����M�^�C���A�E�g			*/
	ALM_TIMEOUT,				/* A.E01 : �I�v�V�����^�]�J�n�v����M�^�C���A�E�g				*/
	ALM_WDC_ERR,				/* A.E02 : WDC�{SyncFlag�����ُ�								*/
	ALM_OPDETECT,				/* A.E07 : �I�v�V��������o�G���[								*/
	ALM_DGOP_TOUT,				/* A.E08 : �c�o�q�`�l�f�W�I�y�ʐM�^�C���A�E�g DigOpCom			*/
	ALM_SC_TOUT,				/* A.E09 : DPRAM S->C �ʐM��ѱ��								*/
	ALM_CMDOP_DET,				/* A.E70 : �w�ߵ�߼�ݶ��ތ��o���s								*/
	ALM_SFTOP_DET,				/* A.E71 : ���è��߼�ݶ��ތ��o���s								*/
	ALM_FBKOP_DET,				/* A.E72 : ̨����ޯ���߼�ݶ��ތ��o���s							*/
	ALM_CMDOP_NSUP,				/* A.E73 : �w�ߵ�߼�ݶ��ޖ���߰�								*/
	ALM_SFTOP_NSUP,				/* A.E74 : ���è��߼�ݶ��ޖ���߰�								*/
	ALM_FBKOP_NSUP,				/* A.E75 : ̨����ޯ���߼�ݶ��ޖ���߰�							*/
	ALM_CMDOP_UNMATCH,			/* A.E80 : �w�ߵ�߼�ݶ��ތ��o�s��v								*/
	ALM_SMC_ERR,				/* A.EB0 : ���è�@�\Drv�Ď���H�ُ�								*/
	ALM_BBON_ERR0,				/* A.EB1 : ���è�@�\�p���͐M���ُ�								*/
	ALM_BBON_ERR1,				/* A.EB2 : ���è�@�\Drv�����M���ُ�								*/
	ALM_SV_COMERR1,				/* A.EB3 : ���è�@�\Drv�ʐM�ُ�P								*/
	ALM_SV_COMERR2,				/* A.EB4 : ���è�@�\Drv�ʐM�ُ�Q								*/
	ALM_SV_COMERR3,				/* A.EB5 : ���è�@�\Drv�ʐM�ُ�R								*/
	ALM_SV_COMDATA,				/* A.EB6 : ���è�@�\Drv�ʐM�f�[�^�ُ�							*/
	ALM_OP_SYSERR1,				/* A.EC0 : ���è��߼�ݶ��޼��шُ�P							*/
	ALM_OP_SYSERR2,				/* A.EC1 : ���è��߼�ݶ��޼��шُ�Q							*/
	ALM_OP_PRMERR,				/* A.EC2 : ���è��߼�ݶ������Ұ��ݒ�ُ�						*/
	ALM_OP_MOTTYP,				/* A.EC3 : ���è��߼�ݶ���Ӱ����ߕs��v							*/
	ALM_OP_COMERR1,				/* A.EC4 : ���è��߼�ݶ��ޒʐM�ُ�P							*/
	ALM_OP_COMERR2,				/* A.EC5 : ���è��߼�ݶ��ޒʐM�ُ�Q							*/
	ALM_OP_COMERR3,				/* A.EC6 : ���è��߼�ݶ��ޒʐM�ُ�R							*/
	ALM_OP_OVERRUN,				/* A.EC7 : ���è��߼�ݶ��ޒ�~�w�߈ُ�							*/
	ALM_DPIN1_ERR,				/* A.EC8 : ���è�@�\Drv�����M���ُ�P					<S0F1>	*/
	ALM_DPIN2_ERR,				/* A.EC9 : ���è�@�\Drv�����M���ُ�Q					<S0F1>	*/
	ALM_PWPHASE,				/* A.F10 : �d������												*/

//	ALM_MBOH,					/* A.7A1 : ���j�b�g�������x�ߔM									*/
//	ALM_CONVOH,					/* A.7A2 : �R���o�[�^���x�ߔM									*/
//	ALM_IFOH,					/* A.7A3 : �������x�ߔM										*/

	ALM_WDC_BIT_ERR,			/* A.E0A : WDC�����ُ�											*/
	ALM_PRM_TRANSLATE,			/* A.BF5 : �V�X�e���A���[���T(�p�����[�^�ꊇ�]���ُ�)			*//* <S066> */
	ALM_I2C_DRVERR,				/* A.BF6 : �V�X�e���A���[���U(EEPROM�h���C�o�ُ�)				*//* <S098> */

	/*	MECHATROLINK�A���[���h�c��`	*/
	ALM_OPTPRM_ERR,				/* A.04A : �I�v�V�����p�����[�^�ݒ�ُ�							*/
	ALM_TRACE_SPD_OF,			/* A.51A : �Ǐ]���x�ߑ�A���[��									*/
	ALM_COMLSI_ERR1,			/* A.B6A : �ʐMASIC�ݒ�ُ�										*/
	ALM_COMLSI_ERR2,			/* A.B6B : �ʐMASIC�V�X�e���ُ�									*/
	ALM_SCANN_STOP,				/* A.BFB : ScanN���s��~										*/
	ALM_TCYC_ERR,				/* A.E40 : MECHATROLINK �`�������ݒ�ُ�						*/
	ALM_CD_LEN_ERR,				/* A.E41 : MECHATROLINK�ʐM�f�[�^�T�C�Y�ݒ�ُ�					*/
	ALM_AXIS_NO_ERR,			/* A.E42 : MECHATROLINK�ǃA�h���X�ݒ�ُ�						*/
	ALM_COMSYNC,				/* A.E50 : �l�b�g���[�N�����G���[								*/
	ALM_COMSYNC_TOUT,			/* A.E51 : �l�b�g���[�N�����m�����s								*/
	ALM_COM,					/* A.E60 : �l�b�g���[�N��M�G���[								*/
	ALM_COM_INT_ERR,			/* A.E61 : �l�b�g���[�N�����Ԋu�ُ�								*/
	ALM_COM_FCS,				/* A.E62 : �l�b�g���[�NFCS�G���[								*/
	ALM_COM_GBL,				/* A.E63 : �O���[�o���f�[�^��M�G���[							*/
	ALM_MON_SLV_COM,			/* A.E64 : ���ǃ��j�^�f�[�^��M�G���[							*/
	ALM_COM_RCV_ASYNC,			/* A.E6A : �l�b�g���[�N��M�G���[�i�C�x���g�h���u���ʐM�j		*/
	ALM_COM_TOUT_ASYNC,			/* A.E6B : �l�b�g���[�N��M�^�C���A�E�g�i�C�x���g�h���u���ʐM�j	*/
	ALM_CON_SEND_ASYNC,			/* A.E6C : �l�b�g���[�N���M�ُ�i�C�x���g�h���u���ʐM�j			*/
	ALM_NOANS,					/* A.EA0 : �T�[�{�p�b�N�̏�										*/
	ALM_SVINIT_ERR,				/* A.EA1 : �T�[�{�p�b�N�����A�N�Z�X�ُ�							*/
	ALM_SVWDC_ERR,				/* A.EA2 : �T�[�{�p�b�NWDC�ُ�									*/
	ALM_SVANS_TOUT,				/* A.ED0 : �T�[�{�p�b�N�����^�C���A�E�g							*/
	ALM_CMDEXEC,				/* A.ED1 : �R�}���h���s����										*/

	/* �T�[�{���[�j���O�h�c��` */
	WRN_OF,						/* A.900 : �ʒu�΍��ߑ�											*/
	WRN_BEOF,					/* A.901 : �T�[�{�I�����ʒu�΍��ߑ�								*/
	WRN_OLF,					/* A.910 : �ߕ���												*/
	WRN_VIB,					/* A.911 : �U��(���U)											*/
	WRN_CTRLBRD,				/* A.912 : �������x�x��1�i�������x�x���j					*//* <S0DE> */
	WRN_PWRBRD,					/* A.913 : �������x�x��2�i�p���[����x�x���j					*//* <S0DE> */
	WRN_RGOLF,					/* A.920 : �񐶉ߕ���											*/
	WRN_DBOL_LVL,				/* A.921 : �c�a�ߕ���											*/
	WRN_FANLOCK,				/* A.923 : �T�[�{�p�b�N�����t�@����~							*//* <S0A3> */
	WRN_EXREG,					/* A.924 : �񐶌x��											*//* <S21D> */
	WRN_PGBAT,					/* A.930 : ��Βl�G���R�[�_�o�b�e���ُ�							*/
	WRN_FPGBAT,					/* A.931 : �e�o�f�o�b�e���ُ�									*/
	WRN_DATA,					/* A.940 : �f�[�^�ُ�											*/
	WRN_BOOT,					/* A.941 : �d���ē����v���[�U�[�萔�ύX							*/
	WRN_RIPPLE,					/* A.942 : ���b�v���␳���s��v								*/
	WRN_CMD,					/* A.950 : �R�}���h�ُ�											*/
	WRN_UV,						/* A.971 : �s���d��												*/
	WRN_OPT,					/* A.9F0 : �I�v�V�����ُ�										*/

	WRN_MBOH,					/* A.981 : ���j�b�g�������x�ߔM									*/
	WRN_CONVOH,					/* A.982 : �R���o�[�^���x�ߔM									*/
	WRN_IFOH,					/* A.983 : �������x�ߔM										*/
	WRN_OT,						/* A.9A0 : OT�M�����o���[�j���O									*/

	/*	MECHATROLINK���[�j���O�h�c��` */
	WRN_PRM_NO,					/* A.94A : �f�[�^�ݒ�x��(���[�U�萔�ԍ�)						*/
	WRN_DATA_AREA,				/* A.94B : �f�[�^�ݒ�x��(�f�[�^�͈͊O)							*/
	WRN_DATA_CALC,				/* A.94C : �f�[�^�ݒ�x��(���Z�ُ�)								*/
	WRN_DATA_SIZE,				/* A.94D : �f�[�^�ݒ�x��(�f�[�^�T�C�Y�ُ�)						*/
	WRN_DATA_LTMOD,				/* A.94E : �f�[�^�ݒ�x��(���b�`���[�h�ُ�)						*/
	WRN_CMD_CNDTN,				/* A.95A : �R�}���h�x��(�����O)									*/
	WRN_CMD_NOSPT,				/* A.95B : �R�}���h�x��(���T�|�[�g)								*/
	WRN_CMD_PRM,				/* A.95C : �R�}���h�x��(���[�U�萔�ɂ������O)					*/
	WRN_CMD_CNFLCT,				/* A.95D : �R�}���h�x��(�R�}���h�̊���) 						*/
	WRN_SUBCMD,					/* A.95E : �R�}���h�x��(�T�u�R�}���h�s��) 						*/
	WRN_FUNC_CNFLCT,			/* A.95F : �R�}���h�x��(�|�[�g�̊���) 							*/
	WRN_COM,					/* A.960 : MECHATROLINK�ʐM�x��									*/
	WRN_COM_FCS,				/* A.962 : FCS�ُ�x��											*/
	WRN_COM_GBL,				/* A.963 : �O���[�o���f�[�^�ʐM�x��								*/
	WRN_SLV_COM,				/* A.964 : ���ǃ��j�^�f�[�^�ʐM�x��								*/
	WRN_COM_RCV_ASYNC,			/* A.96A : �l�b�g���[�N��M�x��(�C�x���g�h���u���ʐM)			*/
	WRN_COM_PHASE,				/* A.97A : �t�F�[�Y�ُ�											*/
	WRN_DATA_CLAMP,				/* A.97B : �f�[�^�ݒ�x��(�f�[�^�N�����v����)					*/
	WRN_ACT_COP_STP,			/* A.97C : ����x���i������~����j								*/
	WRN_PREVENTMAINT,			/* A.9B0:�\�h�ۑS���[�j���O										*/ /* <S154> */

} ALMID_T;


/* �A���[����`�\���� */
typedef struct ALM_TBL_DEF {
	USHORT	AlmCode;				/* �A���[���R�[�h						*/
	UCHAR	AlmAttr;				/* �A���[������							*/
	UCHAR	StopMode;				/* �A���[���������̒�~���@,�n�k����	*/
	UCHAR	AlmCodeOut;				/* �A���[���R�[�h�o�͐M��				*/
	UCHAR	AlmLedOut;				/* �A���[���k�d�c�o�͐M��				*/
	UCHAR	AlmAttrEx;				/* �A���[���g������						*/
	UCHAR	Spare;					/* Spare								*/
} ALM_TBL_DEF;
typedef const ALM_TBL_DEF*	CPALM_TBL_DEF;


/* Glogals */
extern const ALM_TBL_DEF	servo_alm_tbl[];	/* �A���[���e�[�u��			*/
extern const LONG			AlmDefTblEntNum;	/* �A���[���e�[�u���v�f��	*/


#endif /* ALARM_H */
