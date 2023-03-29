/****************************************************************************************************/
/*																									*/
/*																									*/
/*		XlsTrcDef.c : �g���[�X��`�\�[�X�t�@�C��													*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1. ���l�g���[�X��`�e�[�u��																	*/
/*																									*/
/*		2. �r�b�g�g���[�X��`�e�[�u��																*/
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
/****************************************************************************************************/
#include "XlsTrcDef.h"
#include "MercuryGlobals.h"
#include "DataTraceCalc.h"

/*--------------------------------------------------------------------------------------------------*/
/*  ���ӁF �G�N�Z���}�N����Ή��B																	*/
/*  �ȉ���ݒ肷��K�v����B																		*/
/*  Sel					�F���l�g���[�X�I��ԍ�														*/
/*  SftL				�F���V�t�g��																*/
/*  SftR				�F�E�V�t�g��																*/
/*	AvrgSel				�F���ω�����:TRUE�A���ω����Ȃ�:FALSE										*/
/*	GetVerData Function	�F�֐��|�C���^(�g���[�X����ϐ��̒l��Ԃ��֐�)								*/
/*	GainCal Function	�F�֐��|�C���^(SftL,SftR�ȊO�ɃQ�C�����Z���K�v�ȂƂ��ɃQ�C���W����Ԃ��֐�)	*/
/*--------------------------------------------------------------------------------------------------*/
/****************************************************************************************************/
/*																									*/
/*		���l�g���[�X��`�e�[�u��																	*/
/*																									*/
/****************************************************************************************************/
const	TVXDEF	TrcVarTbl[] = {
/*  Sel  SftL SftR AvrgSel	Get TraceVarData Func       Gain Calcurate Func							*/
/*--------------------------------------------------------------------------------------------------*/
  { 0x00, 0,   0,   FALSE,	NULL,						NULL                      },/* �g���[�X�w��Ȃ�					*/
  { 0x01, 0,  10,   FALSE,	&GetVarData_AmonTrqRef,		NULL                      },/* �g���N�w�߁^���͎w��				*/
  { 0x02, 0,  10,	TRUE,	&GetVarData_MotSpdSmp,		&LpxPcalMotSpeedCalGain2  },/* ���[�^���x						*/
  { 0x03, 0,  10,	FALSE,	&GetVarData_AmonSpdRef,		NULL	                  },/* ���x�w��							*/
  { 0x04, 0,  10, 	TRUE,	&GetVarData_dPosCmdSmp,		&LpxPcalPcmdSpeedCalGain  },/* �ʒu�w�ߑ��x						*/
  { 0x05, 0,   0, 	FALSE,	&GetVarData_AmonPosErra,	NULL                      },/* �ʒu�΍�							*/
//{ 0x10, 0,  10,   FALSE,	&GetVarData_Dummy,			NULL					  },/* �I�u�U�[�o���x					*//* ��Ή� */
//{ 0x11, 0,  10,   FALSE,	&GetVarData_Dummy,			NULL					  },/* ���f���g���N�w�߁^���͎w��		*//* ��Ή� */
//{ 0x12, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* ���f���ʒu�΍�					*//* ��Ή� */
  { 0x13, 0,  10,   FALSE,	&GetVarData_AmonDisTrq,		NULL					  },/* ���C�⏞�g���N�^����				*//* ����J */
  { 0x14, 0,  10,   FALSE,	&GetVarData_MonResVib,		NULL					  },/* ���U�����x						*//* ����J */
  { 0x15, 0,  10,   FALSE,	&GetVarData_TrqGravity,		NULL					  },/* �d�͕⏞�ς݃g���N�w�߁^���͎w��	*/	/*<S0CD>*/	/* ��Ή��ˑΉ� */
  { 0x16, 0,   0,   FALSE,	&GetVarData_AmonPosErrx,	NULL	                  },/* �ʒu�A���v�΍�					*/
  { 0x17, 0,   0,   FALSE,	&GetVarData_sfperra_per32s, NULL				      },/* ���[�^���׊Ԉʒu�΍�				*/
  { 0x18, 0,  10,   FALSE,	&GetVarData_SpdFFCx,		NULL					  },/* ���x�t�B�[�h�t�H���[�h			*/
  { 0x19, 0,  10,   FALSE,	&GetVarData_TrqFFCx,		NULL					  },/* �g���N�^���̓t�B�[�h�t�H���[�h	*/
//{ 0x1A, 0,  10,   FALSE,	&GetVarData_Dummy,			NULL					  },/* MFC��̈ʒu�w��					*//* ��Ή� */
//{ 0x1B, 0,  10,   FALSE,	&GetVarData_Dummy,			NULL					  },/* ���Z�g���N�w�߁^���͎w��			*//* ��Ή� */
//{ 0x1C, 0,  10,   FALSE,	&GetVarData_Dummy,			NULL				      },/* ���Z�O���g���N�w�߁^���͎w��		*//* ��Ή� */
//{ 0x1D, 0,  10,   FALSE,	&GetVarData_Dummy,			NULL					  },/* ���葬�x							*//* ��Ή� */
  { 0x1E, 0,  10,   FALSE,	&GetVarData_TrefBeforeDtrq, NULL					  },/* �g���N�w��(�O���O)				*//* ����J */
  { 0x1F, 0,  10,   FALSE,	&GetVarData_TrcCtrlTrqRef,	NULL					  },/* �g���N�w�߁|���C�⏞�g���N		*//* ����J */
  { 0x20, 0,  10,   FALSE,	&GetVarData_TrqRefi,		NULL					  },/* �g���N�w��(�O���O�E�t�B���^�O)	*//* ����J */
  { 0x21, 0,  10,   FALSE,	&GetVarData_AmonActGain,	NULL					  },/* �L���Q�C��						*/
  { 0x22, 0,  10,   TRUE,	&GetVarData_dPosRefi,		&LpxPcalMotSpeedCalGain   },/* �ʒu���[�v�O�w�ߑ��x				*/
  { 0x23, 0,   0,   FALSE,	&GetVarData_DcVoltx,		NULL					  },/* ���H����d��					*/
  { 0x24, 0,   0,   FALSE,	&GetVarData_UnCrntDet,		NULL					  },/* �d�����o�l						*/
//{ 0x25, 0,  10,   FALSE,	&GetVarData_Dummy,			NULL					  },/* �������ח�						*/
  { 0x26, 0,   0,   FALSE,	&GetVarData_UnCrntRef,		NULL					  },/* �ݐϕ��ח�						*/
  { 0x27, 0,   0,   FALSE,	&GetVarData_TrcRgPower,		NULL					  },/* �񐶕��ח�						*/
  { 0x28, 0,   0,   FALSE,	&GetVarData_TrcDbPower,		NULL					  },/* DB��R����d��					*/
  { 0x29, 0,  10,   TRUE,	&GetVarData_FencSpdSmp,		NULL					  },/* �O���G���R�[�_���x				*/
	//0x40��0x42�͓������̂��o�́I0x40���t���N�ɕύX�Ikira_110309
  { 0x40, 1,   0,   FALSE,	&GetVarData_DeltaPos,		NULL                      },/* �ʒu����							*/
  { 0x41, 1,   0,   FALSE,	&GetVarData_dPcmdEgear,		NULL					  },/* �w�ߍ���							*/
  { 0x42, 1,   0,   FALSE,	&GetVarData_DeltaPos,		NULL                      },/* ���[�^�ʒu����					*/
  { 0x43, 1,   0,   FALSE,	&GetVarData_dFencPos,		NULL					  },/* ���׈ʒu����						*/
  { 0x44, 0,   2,   FALSE,	&GetVarData_MotPhase,		NULL					  },/* �d���ʑ�							*/
//{ 0x50, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* SOM���[�^���x					*//* ��Ή� */
//{ 0x51, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* SOM���[�^�ړ���					*//* ��Ή� */
//{ 0x52, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* SOM�Ď����xA						*//* ��Ή� */
//{ 0x53, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* SOM�Ď����xB						*//* ��Ή� */
//{ 0x54, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* �A�N�e�B�u���[�h�����w�ߑ��x		*//* ��Ή� */
//{ 0x55, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* SOM�����f�[�^1					*//* ��Ή� */
//{ 0x56, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* SOM�����f�[�^2					*//* ��Ή� */
  { 0x60, 0,   0,   FALSE,	&GetVarData_InterSignalX_L,	NULL					  },/* �����M�����j�^(����)forSynqNet	*/
  { 0x61, 0,   0,   FALSE,	&GetVarData_InterSignalX_H,	NULL					  },/* �����M�����j�^(���)forSynqNet	*/
  { 0x62, 0,   0,   FALSE,	&GetVarData_InternalInputSignals_L,		NULL		  },/* �������͐M�����j�^(����)forSynqNet*/
  { 0x63, 0,   0,   FALSE,	&GetVarData_InternalInputSignals_H,		NULL		  },/* �������͐M�����j�^(���)forSynqNet*/
  { 0x64, 0,   0,   FALSE,	&GetVarData_InternalOutputSignals_L,	NULL		  },/* �����o�͐M�����j�^(����)forSynqNet*/
  { 0x65, 0,   0,   FALSE,	&GetVarData_InternalOutputSignals_H,	NULL		  },/* �����o�͐M�����j�^(���)forSynqNet*/
  { 0x81, 0,   0,   FALSE,	&GetVarData_AmonTrqRef,		NULL                      },/* �g���N�w�߁^���͎w��				*/
  { 0x82, 0,   0,	TRUE,	&GetVarData_MotSpdSmp,		&LpxPcalMotSpeedCalGain2  },/* ���[�^���x						*/
  { 0x83, 0,   0,   FALSE,	&GetVarData_AmonSpdRef,		NULL                      },/* ���x�w��							*/
  { 0x84, 0,   0,   TRUE,	&GetVarData_dPosCmdSmp,		&LpxPcalPcmdSpeedCalGain  },/* �ʒu�w�ߑ��x						*/
  { 0x85, 0,   0,   FALSE,	&GetVarData_AmonPosErra,	NULL                      },/* �ʒu�΍�							*/
//{ 0x90, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* �I�u�U�[�o���x					*//* ��Ή� */
//{ 0x91, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* ���f���g���N�w�߁^���͎w��		*//* ��Ή� */
//{ 0x92, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* ���f���ʒu�΍�					*//* ��Ή� */
//{ 0x93, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* ���C�⏞�g���N�^����				*//* ��Ή� */
  { 0x94, 0,   0,   FALSE,	&GetVarData_MonResVib,		NULL					  },/* ���U�����x						*/
  { 0x95, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL				  	  },/* �d�͕⏞�ς݃g���N�w�߁^���͎w��	*//* <S0CD> */	/* ��Ή��ˑΉ� */
  { 0x96, 0,   0,   FALSE,	&GetVarData_AmonPosErrx,	NULL	                  },/* �ʒu�A���v�΍�					*/
  { 0x97, 0,   0,   FALSE,	&GetVarData_sfperra_per32s, NULL					  },/* ���[�^���׊Ԉʒu�΍�				*/
  { 0x98, 0,   0,   FALSE,	&GetVarData_SpdFFCx,		NULL					  },/* ���x�t�B�[�h�t�H���[�h			*/
  { 0x99, 0,   0,   FALSE,	&GetVarData_TrqFFCx,		NULL					  },/* �g���N�^���̓t�B�[�h�t�H���[�h	*/
//{ 0x9A, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* MFC��̈ʒu�w��					*//* ��Ή� */
//{ 0x9B, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* ���Z�g���N�w�߁^���͎w��			*//* ��Ή� */
//{ 0x9C, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* ���Z�O���g���N�w�߁^���͎w��		*//* ��Ή� */
//{ 0x9D, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* ���葬�x							*//* ��Ή� */
//{ 0x9E, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* �g���N�w��(�O���O)				*//* ��Ή� */
//{ 0x9F, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* �g���N�w�߁|���C�⏞�g���N		*//* ��Ή� */
//{ 0xA0, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* �g���N�w��(�O���O�E�t�B���^�O)	*//* ��Ή� */
//{ 0xA1, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* �L���Q�C��						*//* ��Ή� */
  { 0xA2, 0,   0,   TRUE,	&GetVarData_dPosRefi,		&LpxPcalMotSpeedCalGain   },/* �ʒu���[�v�O�w�ߑ��x				*/
  { 0xA9, 0,   0,   TRUE,	&GetVarData_FencSpdSmp,		NULL					  },/* �O���G���R�[�_���x				*/
  { 0xAA, 0,   0,   FALSE,	&GetVarData_VelRippleTrqRefBeforeComp,	NULL		  },/* �g���N�w�߁i���b�v���␳�O�j		*/	/* <S14B><S18C> */
  { 0xC4, 0,   0,   FALSE,	&GetVarData_CurrentPhase,	NULL					  },/* �d���ʑ� 						*/	/* <S14B><S18C> */
  { 0xC5, 0,   0,   FALSE,	&GetVarData_TrcDcVoltFili,	NULL					  },/* ���H����d���i�t�B���^�O�j		*/	/* <S199> *//* ��Ή��ˑΉ� */
//{ 0xD0, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* SOM���[�^���x					*//* ��Ή� */
//{ 0xD1, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* SOM���[�^�ړ���					*//* ��Ή� */
//{ 0xD2, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* SOM�Ď����xA						*//* ��Ή� */
//{ 0xD3, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* SOM�Ď����xB						*//* ��Ή� */
//{ 0xD4, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* �A�N�e�B�u���[�h�����w�ߑ��x		*//* ��Ή� */
//{ 0xD5, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* SOM�����f�[�^1					*//* ��Ή� */
//{ 0xD6, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* SOM�����f�[�^2					*//* ��Ή� */
#if (CSW_NETIF_TYPE == NETIF_M3_TYPE)
  { 0x100, 0,   0,   FALSE,	&GetVarData_M3_Apos,		NULL					  },/* �t�B�[�h�o�b�N�ʒu(APOS)			*/				/* <S164> : �ݒ�l�ύX 0xD0��0x100 */
  { 0x101, 0,   0,   FALSE,	&GetVarData_M3_Cpos,		NULL					  },/* ���ݎw�߈ʒu(CPOS)				*/				/* <S164> : �ݒ�l�ύX 0xD1��0x101 */
  { 0x102, 0,   0,   FALSE,	&GetVarData_M3_Tpos,		NULL					  },/* �ڕW�ʒu(TPOS)					*/				/* <S164> : �ݒ�l�ύX 0xD2��0x102 */
  { 0x103, 0,   0,   FALSE,	&GetVarData_M3_Lpos1,		NULL					  },/* ���b�`�ʒu1(LPOS1)				*/				/* <S164> : �ݒ�l�ύX 0xD3��0x103 */
  { 0x104, 0,   0,   FALSE,	&GetVarData_M3_Lpos2,		NULL					  },/* ���b�`�ʒu2(LPOS2)				*/				/* <S164> : �ݒ�l�ύX 0xD4��0x104 */
//{ 0x105, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* ���b�`�ʒu3(LPOS3)				*//* ��Ή� */	/* <S164> */
  { 0x106, 0,   0,   FALSE,	&GetVarData_M3_Fspd,		NULL					  },/* �t�B�[�h�o�b�N���x(FSPD)			*/			/* <S164> : �ݒ�l�ύX 0xD5��0x106 */
  { 0x107, 0,   0,   FALSE,	&GetVarData_M3_Cspd,		NULL					  },/* ���݈ʒu�w�ߑ��x(CSPD)			*/			/* <S164> : �ݒ�l�ύX 0xD6��0x107 */
  { 0x108, 0,   0,   FALSE,	&GetVarData_M3_Trq,			NULL					  },/* �g���N/���͎w��(TRQ)				*/			/* <S164> : �ݒ�l�ύX 0xD7��0x108 */
  { 0x109, 0,   0,   FALSE,	&GetVarData_M3_TrqLim,		NULL					  },/* �g���N�����w��(TRQ_LIM)			*/			/* <S164> : �ݒ�l�ύX 0xD8��0x109 */
  { 0x10A, 0,   0,   FALSE,	&GetVarData_M3_VLim,		NULL					  },/* ���x�����w��(V_LIM)				*/			/* <S164> : �ݒ�l�ύX 0xD9��0x10A */
  { 0x10B, 0,   0,   FALSE,	&GetVarData_M3_M3_Alam,		NULL					  },/* ���ݔ����A���[���R�[�h(ALARM)	*/			/* <S164> : �ݒ�l�ύX 0xDA��0x10B */
  { 0x10C, 0,   0,   FALSE,	&GetVarData_M3_RCmd,		NULL					  },/* M-�V���ݎ��s�R�}���h�R�[�h(RCMD)	*/			/* <S164> : �ݒ�l�ύX 0xDB��0x10C */
  { 0x10D, 0,   0,   FALSE,	&GetVarData_M3_SubRCmd,		NULL					  },/* M-�V���ݎ��s�T�u�R�}���h�R�[�h(SUB_RCMD)	*/	/* <S164> : �ݒ�l�ύX 0xDC��0x10D */
  { 0x110, 0,   0,   FALSE,	&GetVarData_M3_Cmd_0,		NULL					  },/* M-�V�R�}���h�f�[�^( 0�` 3byte)	*/			/* <S164> : �ݒ�l�ύX 0xE0��0x110 */
  { 0x111, 0,   0,   FALSE,	&GetVarData_M3_Cmd_1,		NULL					  },/* M-�V�R�}���h�f�[�^( 4�` 7byte)	*/			/* <S164> : �ݒ�l�ύX 0xE1��0x111 */
  { 0x112, 0,   0,   FALSE,	&GetVarData_M3_Cmd_2,		NULL					  },/* M-�V�R�}���h�f�[�^( 8�`11byte)	*/			/* <S164> : �ݒ�l�ύX 0xE2��0x112 */
  { 0x113, 0,   0,   FALSE,	&GetVarData_M3_Cmd_3,		NULL					  },/* M-�V�R�}���h�f�[�^(12�`15byte)	*/			/* <S164> : �ݒ�l�ύX 0xE3��0x113 */
  { 0x114, 0,   0,   FALSE,	&GetVarData_M3_Cmd_4,		NULL					  },/* M-�V�R�}���h�f�[�^(16�`19byte)	*/			/* <S164> : �ݒ�l�ύX 0xE4��0x114 */
  { 0x115, 0,   0,   FALSE,	&GetVarData_M3_Cmd_5,		NULL					  },/* M-�V�R�}���h�f�[�^(20�`23byte)	*/			/* <S164> : �ݒ�l�ύX 0xE5��0x115 */
  { 0x116, 0,   0,   FALSE,	&GetVarData_M3_Cmd_6,		NULL					  },/* M-�V�R�}���h�f�[�^(24�`27byte)	*/			/* <S164> : �ݒ�l�ύX 0xE6��0x116 */
  { 0x117, 0,   0,   FALSE,	&GetVarData_M3_Cmd_7,		NULL					  },/* M-�V�R�}���h�f�[�^(28�`31byte)	*/			/* <S164> : �ݒ�l�ύX 0xE7��0x117 */
  { 0x118, 0,   0,   FALSE,	&GetVarData_M3_Cmd_8,		NULL					  },/* M-�V�R�}���h�f�[�^(32�`35byte)	*/			/* <S164> : �ݒ�l�ύX 0xE8��0x118 */
  { 0x119, 0,   0,   FALSE,	&GetVarData_M3_Cmd_9,		NULL					  },/* M-�V�R�}���h�f�[�^(36�`39byte)	*/			/* <S164> : �ݒ�l�ύX 0xE9��0x119 */
  { 0x11A, 0,   0,   FALSE,	&GetVarData_M3_Cmd_a,		NULL					  },/* M-�V�R�}���h�f�[�^(40�`43byte)	*/			/* <S164> : �ݒ�l�ύX 0xEA��0x11A */
  { 0x11B, 0,   0,   FALSE,	&GetVarData_M3_Cmd_b,		NULL					  },/* M-�V�R�}���h�f�[�^(44�`47byte)	*/			/* <S164> : �ݒ�l�ύX 0xEB��0x11B */
  { 0x120, 0,   0,   FALSE,	&GetVarData_M3_Rsp_0,		NULL					  },/* M-�V���X�|���X�f�[�^( 0�` 3byte)	*/			/* <S164> : �ݒ�l�ύX 0xF0��0x120 */
  { 0x121, 0,   0,   FALSE,	&GetVarData_M3_Rsp_1,		NULL					  },/* M-�V���X�|���X�f�[�^( 4�` 7byte)	*/			/* <S164> : �ݒ�l�ύX 0xF1��0x121 */
  { 0x122, 0,   0,   FALSE,	&GetVarData_M3_Rsp_2,		NULL					  },/* M-�V���X�|���X�f�[�^( 8�`11byte)	*/			/* <S164> : �ݒ�l�ύX 0xF2��0x122 */
  { 0x123, 0,   0,   FALSE,	&GetVarData_M3_Rsp_3,		NULL					  },/* M-�V���X�|���X�f�[�^(12�`15byte)	*/			/* <S164> : �ݒ�l�ύX 0xF3��0x123 */
  { 0x124, 0,   0,   FALSE,	&GetVarData_M3_Rsp_4,		NULL					  },/* M-�V���X�|���X�f�[�^(16�`19byte)	*/			/* <S164> : �ݒ�l�ύX 0xF4��0x124 */
  { 0x125, 0,   0,   FALSE,	&GetVarData_M3_Rsp_5,		NULL					  },/* M-�V���X�|���X�f�[�^(20�`23byte)	*/			/* <S164> : �ݒ�l�ύX 0xF5��0x125 */
  { 0x126, 0,   0,   FALSE,	&GetVarData_M3_Rsp_6,		NULL					  },/* M-�V���X�|���X�f�[�^(24�`27byte)	*/			/* <S164> : �ݒ�l�ύX 0xF6��0x126 */
  { 0x127, 0,   0,   FALSE,	&GetVarData_M3_Rsp_7,		NULL					  },/* M-�V���X�|���X�f�[�^(28�`31byte)	*/			/* <S164> : �ݒ�l�ύX 0xF7��0x127 */
  { 0x128, 0,   0,   FALSE,	&GetVarData_M3_Rsp_8,		NULL					  },/* M-�V���X�|���X�f�[�^(32�`35byte)	*/			/* <S164> : �ݒ�l�ύX 0xF8��0x128 */
  { 0x129, 0,   0,   FALSE,	&GetVarData_M3_Rsp_9,		NULL					  },/* M-�V���X�|���X�f�[�^(36�`39byte)	*/			/* <S164> : �ݒ�l�ύX 0xF9��0x129 */
  { 0x12A, 0,   0,   FALSE,	&GetVarData_M3_Rsp_a,		NULL					  },/* M-�V���X�|���X�f�[�^(40�`43byte)	*/			/* <S164> : �ݒ�l�ύX 0xFA��0x12A */
  { 0x12B, 0,   0,   FALSE,	&GetVarData_M3_Rsp_b,		NULL					  },/* M-�V���X�|���X�f�[�^(44�`47byte)	*/			/* <S164> : �ݒ�l�ύX 0xFB��0x12B */
#endif //CSW_NETIF_TYPE == NETIF_M3_TYPE
};
/*--------------------------------------------------------------------------------------------------*/
const	USHORT	TrcVarTblEntNum = sizeof(TrcVarTbl)/sizeof(TrcVarTbl[0]);
/*--------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------*/
/*  ���ӁF �G�N�Z���}�N����Ή��B																	*/
/*  �ȉ���ݒ肷��K�v����B																		*/
/*  Sel					�F���l�g���[�X�I��ԍ�														*/
/*	GetBitData Function	�F�֐��|�C���^(�g���[�X����r�b�g�̒l��Ԃ��֐�)							*/
/*--------------------------------------------------------------------------------------------------*/
/****************************************************************************************************/
/*																									*/
/*		�r�b�g�g���[�X��`�e�[�u��																	*/
/*																									*/
/****************************************************************************************************/
const	TBXDEF	TrcBitTbl[] = {
/*  Sel�@  Get BitData Function																		*/
/*--------------------------------------------------------------------------------------------------*/
  { 0x00,  NULL						},	/* �g���[�X�w��Ȃ�						*/
  { 0x01,  &GetBitData_SvonReq		},	/* �T�[�{�I���w��(/S-ON)				*/
  { 0x02,  &GetBitData_Pcon			},	/* P����w��(/P-CON)					*/
  { 0x03,  &GetBitData_Pot			},	/* ���]�֎~(P-OT)						*/
  { 0x04,  &GetBitData_Not			},	/* �t�]�֎~(N-OT)						*/
  { 0x05,  &GetBitData_AlmRst		},	/* �A���[�����Z�b�g(/ALMRST)			*/
  { 0x06,  &GetBitData_Pcl			},	/* �����g���N����(/P-CL)				*/
  { 0x07,  &GetBitData_Ncl			},	/* �����g���N����(/N-CL)				*/
  { 0x08,  &GetBitData_SpdD			},	/* SPD-D�M��(/SPD-D)					*//* �Ή� */	/* <S164> *//* ��Ή��ˑΉ� */
  { 0x09,  &GetBitData_SpdA			},	/* SPD-A�M��(/SPD-A)					*//* �Ή� */	/* <S164> *//* ��Ή��ˑΉ� */
  { 0x0A,  &GetBitData_SpdB			},	/* SPD-B�M��(/SPD-B)					*//* �Ή� */	/* <S164> *//* ��Ή��ˑΉ� */
  { 0x0B,  &GetBitData_Csel			},	/* C-SEL�M��(/C-SEL)					*//* �Ή� */	/* <S164> *//* ��Ή��ˑΉ� */
  { 0x0C,  &GetBitData_Zclamp		},	/* �[���N�����v�M��(/ZCLAMP)			*//* �Ή� */	/* <S164> *//* ��Ή��ˑΉ� */
  { 0x0D,  &GetBitData_Inhibit		},	/* �p���X�w�ߖ����M��(/INHIBIT)			*//* �Ή� */	/* <S164> *//* ��Ή��ˑΉ� */
  { 0x0E,  &GetBitData_Gsel			},	/* G-SEL1�M��(/G-SEL1)					*/
  { 0x0F,  &GetBitData_Pdet			},	/* ���Ɍ��o�w��(/P-DET)					*/
//{ 0x10,  &BinV.f.Sv				},	/* G-SEL2�M��(/GSEL-2)					*//* ��Ή� */
  { 0x11,  &GetBitData_Estp			},	/* ����~(/EMG-STP)					*//* ��Ή� */	/* <S199> *//* ��Ή��ˑΉ� */
  { 0x12,  &GetBitData_Dec			},	/* �����M��(/DEC)						*/
  { 0x13,  &GetBitData_Ext1			},	/* EXT1�����ݗv��(/EXT1)				*/
  { 0x14,  &GetBitData_Ext2			},	/* EXT2�����ݗv��(/EXT2)				*/
  { 0x15,  &GetBitData_Ext3			},	/* EXT3�����ݗv��(/EXT3)				*/
  { 0x16,  &GetBitData_Fstp			},	/* ������~/FSTP)						*/	/* <S1F5> */
//{ 0x16,  &ReadDummy.Ulong			},	/* ���͐M��1(/INPUT1)					*//* ��Ή� */
//{ 0x17,  &ReadDummy.Ulong			},	/* ���͐M��2(/INPUT2)					*//* ��Ή� */
//{ 0x18,  &ReadDummy.Ulong			},	/* ���͐M��3(/INPUT3)					*//* ��Ή� */
  { 0x19,  &GetBitData_Sen			},	/* SEN�M��(SEN)							*/
  { 0x1A,  &GetBitData_Puls			},	/* �p���X�w�ߓ��͗L��(PULS)				*//* �Ή� */	/* <S164> *//* ��Ή��ˑΉ� */	/* <S199> */
  { 0x1B,  &GetBitData_Sign			},	/* �����w�ߓ��͗L��(SIGN)				*//* �Ή� */	/* <S164> *//* ��Ή��ˑΉ� */	/* <S199> */
  { 0x1C,  &GetBitData_Clr			},	/* �΍��N���A�M���L��(CLR)				*//* �Ή� */	/* <S164> *//* ��Ή��ˑΉ� */	/* <S199> */
  { 0x1D,  &GetBitData_Psel			},	/* �w�߃p���X���͔{���ؑ֐M��(/PSEL)	*//* �Ή� */	/* <S164> *//* ��Ή��ˑΉ� */	/* <S199> */
  { 0x20,  &GetBitData_HWBB1		},	/* HWBB1�M��(/HWBB1)					*/
  { 0x21,  &GetBitData_HWBB2		},	/* HWBB2�M��(/HWBB2)					*//* <S064> */
//{ 0x22,  &CoutSom.UnSomIOSignal	},	/* ���S�v�����͐M��A1					*//* ��Ή� */
//{ 0x23,  &CoutSom.UnSomIOSignal	},	/* ���S�v�����͐M��A2					*//* ��Ή� */
//{ 0x24,  &CoutSom.UnSomIOSignal	},	/* ���S�v�����͐M��B1					*//* ��Ή� */
//{ 0x25,  &CoutSom.UnSomIOSignal	},	/* ���S�v�����͐M��B2					*//* ��Ή� */
//{ 0x40,  &CoutSom.UnSomIOSignal	},	/* ���S�v�����͏��A					*//* ��Ή� */
//{ 0x41,  &CoutSom.UnSomIOSignal	},	/* ���S�v�����͏��B					*//* ��Ή� */
  { 0x80,  &GetBitData_AlmFlag		},	/* �A���[������(ALM)					*/
  { 0x81,  &GetBitData_Coin			},	/* �ʒu���ߊ���(/COIN)					*/
  { 0x82,  &GetBitData_Vcmp			},	/* ���x��v(/V-CMP)						*/
  { 0x83,  &GetBitData_Tgon			},	/* ���[�^��]���o(/TGON)				*/
  { 0x84,  &GetBitData_SvRdy		},	/* �T�[�{���f�B(/S-RDY)					*/
  { 0x85,  &GetBitData_Clt			},	/* �d��������(CLT)						*/
  { 0x86,  &GetBitData_Vlt			},	/* ���x������(/VLT)						*/
  { 0x87,  &GetBitData_Bk			},	/* �u���[�L�ێ�(/BK)					*/
  { 0x88,  &GetBitData_WrnFlag		},	/* ���[�j���O����(/WARN)				*/
  { 0x89,  &GetBitData_Near			},	/* �ʒu���ߋߖT(/NEAR)					*/
  { 0x8A,  &GetBitData_Cplue		},	/* C���o��(/C-PLUS)						*/
//{ 0x8B,  &ReadDummy.Ulong			},	/* �o�͐M��1(/OUTPUT1)					*//* ��Ή� */
//{ 0x8C,  &ReadDummy.Ulong			},	/* �o�͐M��2(/OUTPUT2)					*//* ��Ή� */
//{ 0x8D,  &ReadDummy.Ulong			},	/* �o�͐M��3(/OUTPUT3)					*//* ��Ή� */
  { 0x8E,  &GetBitData_Alo1			},	/* �A���[���R�[�h�o��1(AL01)			*//* �Ή� */	/* <S164> *//* ��Ή��ˑΉ� */
  { 0x8F,  &GetBitData_Alo2			},	/* �A���[���R�[�h�o��2(AL02)			*//* �Ή� */	/* <S164> *//* ��Ή��ˑΉ� */
  { 0x90,  &GetBitData_Alo3			},	/* �A���[���R�[�h�o��3(AL03)			*//* �Ή� */	/* <S164> *//* ��Ή��ˑΉ� */
  { 0x91,  &GetBitData_Pao			},	/* �����p���X�o��A��(PAO)				*//* �Ή� */	/* <S164> *//* ��Ή��ˑΉ� */
  { 0x92,  &GetBitData_Pbo			},	/* �����p���X�o��B��(PBO)				*//* �Ή� */	/* <S164> *//* ��Ή��ˑΉ� */
  { 0x93,  &GetBitData_Pco			},	/* �����p���X�o��C��(PCO)				*//* �Ή� */	/* <S164> *//* ��Ή��ˑΉ� */
  { 0x94,  &GetBitData_Psela		},	/* �w�߃p���X���͔{���ؑ։���(/PSELA)	*//* �Ή� */	/* <S164> *//* ��Ή��ˑΉ� */
//{ 0xA0,  &CoutSom.UnSomIOSignal	},	/* �O���@�탂�j�^�M��A					*//* ��Ή� */
//{ 0xA1,  &CoutSom.UnSomIOSignal	},	/* �O���@�탂�j�^�M��B					*//* ��Ή� */
  { 0xC0,  &GetBitData_Acon			},	/* ���H�d���I��(ACON)					*/
  { 0xC1,  &GetBitData_Refz			},	/* �w�ߕ����o���[��(REFZ)				*/
  { 0xC2,  &GetBitData_Pdetcmp		},	/* ���Ɍ��o����(PDETCMP)				*/
//{ 0xC3,  &CoutSom.UnSomState		},	/* �A�N�e�B�u���[�h������				*//* ��Ή� */
//{ 0xC4,  &CoutSom.UnSomState		},	/* ���S�@�\������-�Ď�				*//* ��Ή� */
//{ 0xC5,  &CoutSom.UnSomState		},	/* ���S�@�\������-���S				*//* ��Ή� */
//{ 0xC6,  &CoutSom.UnSomState		},	/* ���S�@�\������-HWBB				*//* ��Ή� */
//{ 0xC7,  &CoutSom.UnSomState		},	/* ���S�@�\������-�\��				*//* ��Ή� */
  { 0xC8,  &GetBitData_Den			},	/* �w�ߕ����o������(DEN)				*/
  { 0xC9,  &GetBitData_Pset			},	/* �ʒu���ߊ���(PSET)					*/
#if (CSW_NETIF_TYPE == NETIF_M3_TYPE)
  { 0xCA,  &GetBitData_M3CmdRdy		},	/* M-�V�R�}���h���f�B(CMDRDY)			*/
#endif //CSW_NETIF_TYPE == NETIF_M3_TYPE
};
/*--------------------------------------------------------------------------------------------------*/
const	USHORT	TrcBitTblEntNum = sizeof(TrcBitTbl)/sizeof(TrcBitTbl[0]);
/*--------------------------------------------------------------------------------------------------*/

/***************************************** end of file **********************************************/
