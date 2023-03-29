/****************************************************************************
Description	: Function list of the Data Trace
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
T.Kira   2010.11.01     created

****************************************************************************/
#ifndef __DATA_TRACE_CALC_H
#define __DATA_TRACE_CALC_H

#include "Basedef.h"
#include "PrmAccessIf.h"

/* ���l�g���[�X�f�[�^�擾�֐�	*/
LONG	GetVarData_AmonTrqRef( void *Axis );				/* 0x01,0x81: �g���N/���̓��j�^					*/
LONG	GetVarData_MotSpdSmp( void *Axis );					/* 0x02,0x82: ���[�^���x						*/
LONG	GetVarData_AmonSpdRef( void *Axis );				/* 0x03,0x83: ���x�w��							*/
LONG	GetVarData_dPosCmdSmp( void *Axis );				/* 0x04,0x84: �ʒu�w�ߑ��x						*/
LONG	GetVarData_AmonPosErra( void *Axis );				/* 0x05,0x85: �ʒu�΍�							*/
LONG	GetVarData_AmonDisTrq( void *Axis );				/* 0x13	    : ���C�⏞�g���N�^����				*/
LONG	GetVarData_MonResVib( void *Axis );					/* 0x14		: ���U�����x						*/
LONG	GetVarData_TrqGravity( void *Axis );				/*  0x15	: TrqGravity						*/	/*<S0CD>*/
LONG	GetVarData_AmonPosErrx( void *Axis );				/* 0x16,0x96: �ʒu�A���v�΍�					*/
LONG	GetVarData_sfperra_per32s( void *Axis );			/* 0x17,0x97: ���[�^���׊Ԉʒu�΍�				*/
LONG	GetVarData_SpdFFCx( void *Axis );					/* 0x18,0x98: ���x�t�B�[�h�t�H���[�h			*/
LONG	GetVarData_TrqFFCx( void *Axis );					/* 0x19,0x99: �g���N�^���̓t�B�[�h�t�H���[�h	*/
LONG	GetVarData_TrefBeforeDtrq( void *Axis );			/* 0x1E		: �g���N�w��(�O���O)				*/
LONG	GetVarData_TrcCtrlTrqRef( void *Axis );				/* 0x1F		: �g���N�w�߁|���C�⏞�g���N		*/
LONG	GetVarData_TrqRefi( void *Axis );					/* 0x20		: �g���N�w��(�O���O�E�t�B���^�O)	*/
LONG	GetVarData_AmonActGain( void *Axis );				/* 0x21     : �L���Q�C��						*/
LONG	GetVarData_dPosRefi( void *Axis );					/* 0x22,0xA2: �ʒu���[�v�O�ʒu�w�ߑ��x			*/	/* <S187> */
LONG	GetVarData_DcVoltx( void *Axis );					/* 0x23		: ���H����d��					*/
LONG	GetVarData_UnCrntDet( void *Axis );					/* 0x24     : �d�����o�l						*/
LONG	GetVarData_UnCrntRef( void *Axis );					/* 0x26	    : �ݐϕ��ח�						*/
LONG	GetVarData_TrcRgPower( void *Axis );				/* 0x27		: �񐶕��ח�						*/
LONG	GetVarData_TrcDbPower( void *Axis );				/* 0x28		: DB��R����d��					*/
LONG	GetVarData_FencSpdSmp( void *Axis );				/* 0x29		: �O���G���R�[�_���x				*/
LONG	GetVarData_DeltaPos( void *Axis );					/* 0x40		: �ʒu����							*/
LONG	GetVarData_dPcmdEgear( void *Axis );				/* 0x41     : �w�ߍ���							*/
LONG	GetVarData_DeltaPos( void *Axis );					/* 0x42		: ���[�^�ʒu����					*/
LONG	GetVarData_dFencPos( void *Axis );					/* 0x43		: ���׈ʒu����						*/
LONG	GetVarData_MotPhase( void *Axis );					/* 0x44		: �d���ʑ�							*/
LONG	GetVarData_InterSignalX_L( void *Axis );			/* 0x60		: �����M�����j�^(����)				*/
LONG	GetVarData_InterSignalX_H( void *Axis );			/* 0x61		: �����M�����j�^(���)				*/
LONG	GetVarData_InternalInputSignals_L( void *Axis );	/* 0x62		: �������͐M�����j�^(����)			*/
LONG	GetVarData_InternalInputSignals_H( void *Axis );	/* 0x63		: �������͐M�����j�^(���)			*/
LONG	GetVarData_InternalOutputSignals_L( void *Axis );	/* 0x64		: �����o�͐M�����j�^(����)			*/
LONG	GetVarData_InternalOutputSignals_H( void *Axis );	/* 0x65		: �����o�͐M�����j�^(���)			*/
LONG	GetVarData_VelRippleTrqRefBeforeComp( void *Axis );	/* 0xAA		: ���x���b�v�� �⏞�O�g���N/���͎w��*/	/* <S14B> */
LONG	GetVarData_CurrentPhase( void *Axis );				/* 0xC4		: �d���ʑ�							*/	/* <S14B> */
LONG	GetVarData_TrcDcVoltFili( void *Axis );				/* 0xC5		:���H����d��(�t�B���^�O)			*/	/* <S199> */
LONG	GetVarData_M3_Apos( void *Axis );					/* 0x100	: M-�V�t�B�[�h�o�b�N�ʒu(APOS)		*/			/* <S164> : �ݒ�l�ύX 0xD0��0x100 */
LONG	GetVarData_M3_Cpos( void *Axis );					/* 0x101	: M-�V���ݎw�߈ʒu(CPOS)			*/			/* <S164> : �ݒ�l�ύX 0xD1��0x101 */
LONG	GetVarData_M3_Tpos( void *Axis );					/* 0x102	: M-�V�ڕW�ʒu(TPOS)				*/			/* <S164> : �ݒ�l�ύX 0xD2��0x102 */
LONG	GetVarData_M3_Lpos1( void *Axis );					/* 0x103	: M-�V���b�`�ʒu1(LPOS1)			*/			/* <S164> : �ݒ�l�ύX 0xD3��0x103 */
LONG	GetVarData_M3_Lpos2( void *Axis );					/* 0x104	: M-�V���b�`�ʒu2(LPOS2)			*/			/* <S164> : �ݒ�l�ύX 0xD4��0x104 */
LONG	GetVarData_M3_Fspd( void *Axis );					/* 0x105	: M-�V�t�B�[�h�o�b�N���x(FSPD)		*/			/* <S164> : �ݒ�l�ύX 0xD5��0x105 */
LONG	GetVarData_M3_Cspd( void *Axis );					/* 0x106	: M-�V���݈ʒu�w�ߑ��x(CSPD)		*/			/* <S164> : �ݒ�l�ύX 0xD6��0x106 */
LONG	GetVarData_M3_Trq( void *Axis );					/* 0x107	: M-�V�g���N/���͎w��(TRQ)			*/			/* <S164> : �ݒ�l�ύX 0xD7��0x107 */
LONG	GetVarData_M3_TrqLim( void *Axis );					/* 0x108	: M-�V�g���N�����w��(TRQ_LIM)		*/			/* <S164> : �ݒ�l�ύX 0xD8��0x108 */
LONG	GetVarData_M3_VLim( void *Axis );					/* 0x109	: M-�V���x�����w��(V_LIM)			*/			/* <S164> : �ݒ�l�ύX 0xD9��0x109 */
LONG	GetVarData_M3_M3_Alam( void *Axis );				/* 0x10A	: M-�V���ݔ����A���[���R�[�h(ALARM)	*/			/* <S164> : �ݒ�l�ύX 0xDA��0x10A */
LONG	GetVarData_M3_RCmd( void *Axis );					/* 0x10B	: M-�V���ݎ��s�R�}���h�R�[�h(RCMD)	*/			/* <S164> : �ݒ�l�ύX 0xDB��0x10B */
LONG	GetVarData_M3_SubRCmd( void *Axis );				/* 0x10C	: M-�V���ݎ��s�T�u�R�}���h�R�[�h(SUB_RCMD)*/	/* <S164> : �ݒ�l�ύX 0xDC��0x10C */
LONG	GetVarData_M3_Cmd_0( void *Axis );					/* 0x110	: M-�V�R�}���h�f�[�^( 0�` 3byte)	*/			/* <S164> : �ݒ�l�ύX 0xE0��0x110 */
LONG	GetVarData_M3_Cmd_1( void *Axis );					/* 0x111	: M-�V�R�}���h�f�[�^( 4�` 7byte)	*/			/* <S164> : �ݒ�l�ύX 0xE1��0x111 */
LONG	GetVarData_M3_Cmd_2( void *Axis );					/* 0x112	: M-�V�R�}���h�f�[�^( 8�`11byte)	*/			/* <S164> : �ݒ�l�ύX 0xE2��0x112 */
LONG	GetVarData_M3_Cmd_3( void *Axis );					/* 0x113	: M-�V�R�}���h�f�[�^(12�`15byte)	*/			/* <S164> : �ݒ�l�ύX 0xE3��0x113 */
LONG	GetVarData_M3_Cmd_4( void *Axis );					/* 0x114	: M-�V�R�}���h�f�[�^(16�`19byte)	*/			/* <S164> : �ݒ�l�ύX 0xE4��0x114 */
LONG	GetVarData_M3_Cmd_5( void *Axis );					/* 0x115	: M-�V�R�}���h�f�[�^(20�`23byte)	*/			/* <S164> : �ݒ�l�ύX 0xE5��0x115 */
LONG	GetVarData_M3_Cmd_6( void *Axis );					/* 0x116	: M-�V�R�}���h�f�[�^(24�`27byte)	*/			/* <S164> : �ݒ�l�ύX 0xE6��0x116 */
LONG	GetVarData_M3_Cmd_7( void *Axis );					/* 0x117	: M-�V�R�}���h�f�[�^(28�`31byte)	*/			/* <S164> : �ݒ�l�ύX 0xE7��0x117 */
LONG	GetVarData_M3_Cmd_8( void *Axis );					/* 0x118	: M-�V�R�}���h�f�[�^(32�`35byte)	*/			/* <S164> : �ݒ�l�ύX 0xE8��0x118 */
LONG	GetVarData_M3_Cmd_9( void *Axis );					/* 0x119	: M-�V�R�}���h�f�[�^(36�`39byte)	*/			/* <S164> : �ݒ�l�ύX 0xE9��0x119 */
LONG	GetVarData_M3_Cmd_a( void *Axis );					/* 0x11A	: M-�V�R�}���h�f�[�^(40�`43byte)	*/			/* <S164> : �ݒ�l�ύX 0xEA��0x11A */
LONG	GetVarData_M3_Cmd_b( void *Axis );					/* 0x11B	: M-�V�R�}���h�f�[�^(44�`47byte)	*/			/* <S164> : �ݒ�l�ύX 0xEB��0x11B */
LONG	GetVarData_M3_Rsp_0( void *Axis );					/* 0x120	: M-�V���X�|���X�f�[�^( 0�` 3byte)	*/			/* <S164> : �ݒ�l�ύX 0xF0��0x120 */
LONG	GetVarData_M3_Rsp_1( void *Axis );					/* 0x121	: M-�V���X�|���X�f�[�^( 4�` 7byte)	*/			/* <S164> : �ݒ�l�ύX 0xF1��0x121 */
LONG	GetVarData_M3_Rsp_2( void *Axis );					/* 0x122	: M-�V���X�|���X�f�[�^( 8�`11byte)	*/			/* <S164> : �ݒ�l�ύX 0xF2��0x122 */
LONG	GetVarData_M3_Rsp_3( void *Axis );					/* 0x123	: M-�V���X�|���X�f�[�^(12�`15byte)	*/			/* <S164> : �ݒ�l�ύX 0xF3��0x123 */
LONG	GetVarData_M3_Rsp_4( void *Axis );					/* 0x124	: M-�V���X�|���X�f�[�^(16�`19byte)	*/			/* <S164> : �ݒ�l�ύX 0xF4��0x124 */
LONG	GetVarData_M3_Rsp_5( void *Axis );					/* 0x125	: M-�V���X�|���X�f�[�^(20�`23byte)	*/			/* <S164> : �ݒ�l�ύX 0xF5��0x125 */
LONG	GetVarData_M3_Rsp_6( void *Axis );					/* 0x126	: M-�V���X�|���X�f�[�^(24�`27byte)	*/			/* <S164> : �ݒ�l�ύX 0xF6��0x126 */
LONG	GetVarData_M3_Rsp_7( void *Axis );					/* 0x127	: M-�V���X�|���X�f�[�^(28�`31byte)	*/			/* <S164> : �ݒ�l�ύX 0xF7��0x127 */
LONG	GetVarData_M3_Rsp_8( void *Axis );					/* 0x128	: M-�V���X�|���X�f�[�^(32�`35byte)	*/			/* <S164> : �ݒ�l�ύX 0xF8��0x128 */
LONG	GetVarData_M3_Rsp_9( void *Axis );					/* 0x129	: M-�V���X�|���X�f�[�^(36�`39byte)	*/			/* <S164> : �ݒ�l�ύX 0xF9��0x129 */
LONG	GetVarData_M3_Rsp_a( void *Axis );					/* 0x12A	: M-�V���X�|���X�f�[�^(40�`43byte)	*/			/* <S164> : �ݒ�l�ύX 0xFA��0x12A */
LONG	GetVarData_M3_Rsp_b( void *Axis );					/* 0x12B	: M-�V���X�|���X�f�[�^(44�`47byte)	*/			/* <S164> : �ݒ�l�ύX 0xFB��0x12B */
LONG	GetVarData_AdrShortData( void *Axis );				/* 0x7F,0xFF: ���A�h���X�w��(SHORT)				*/
LONG	GetVarData_AdrLongData( void *Axis );				/* 0x7F,0xFF: ���A�h���X�w��(LONG)				*/
LONG	GetVarData_Dummy( void *Axis );						/* ****		: �_�~�[�ϐ�						*/
LONG	GetVarData_SqnetAdrData( void *Axis );				/* ****		: ���A�h���X�w��(SynqNet)			*/
LONG	GetVarData_SqnetAdrData2( void *Axis );			/* ****		: ���A�h���X�w��(SynqNet)			*/

LONG GetVarData_AmonAdrOutCH0H( void *Axis );				/* ****<S074>: ���A�h���X�w��(AMON_CH:0 SHORT)  */
LONG GetVarData_AmonAdrOutCH0W( void *Axis );				/* ****<S074>: ���A�h���X�w��(AMON_CH:0 LONG)   */
LONG GetVarData_AmonAdrOutCH1H( void *Axis );				/* ****<S074>: ���A�h���X�w��(AMON_CH:1 SHORT)  */
LONG GetVarData_AmonAdrOutCH1W( void *Axis );				/* ****<S074>: ���A�h���X�w��(AMON_CH:1 LONG)   */


/* ���l�g���[�X�f�[�^�̃Q�C���v�Z�֐�	*/
LONG	LpxPcalMotSpeedCalGain( void* Axis );				/* 0x02,0x82: ���[�^���x						*/
LONG	LpxPcalMotSpeedCalGain2( void* Axis );				/* 0x04,0x84: �ʒu�w�ߑ��x						*/
LONG	LpxPcalPcmdSpeedCalGain( void* Axis );				/* 0x22		: �ʒu���[�v�O�w�ߑ��x				*/

/* �r�b�g�g���[�X�f�[�^�擾�֐�	*/
BOOL	GetBitData_SvonReq( void *Axis );					/* 0x01	: �T�[�{�I���w��(/S-ON)					*/
BOOL	GetBitData_Pcon( void *Axis );						/* 0x02	: P����w��(/P-CON)						*/
BOOL	GetBitData_Pot( void *Axis );						/* 0x03	: ���]�֎~(P-OT)						*/
BOOL	GetBitData_Not( void *Axis );						/* 0x04	: �t�]�֎~(N-OT)						*/
BOOL	GetBitData_AlmRst( void *Axis );					/* 0x05	: �A���[�����Z�b�g(/ALMRST)				*/
BOOL	GetBitData_Pcl( void *Axis );						/* 0x06	: �����g���N����(/P-CL)					*/
BOOL	GetBitData_Ncl( void *Axis );						/* 0x07	: �����g���N����(/N-CL)					*/
/* <S164> Start */
BOOL	GetBitData_SpdD( void *Axis );						/* 0x08	: �r�o�c�|�c�M��(/SPD-D)				*/
BOOL	GetBitData_SpdA( void *Axis );						/* 0x09	: �r�o�c�|�`�M��(/SPD-A)				*/
BOOL	GetBitData_SpdB( void *Axis );						/* 0x0A	: �r�o�c�|�a�M��(/SPD-B)				*/
BOOL	GetBitData_Csel( void *Axis );						/* 0x0B	: �b�|�r�d�k�M��(/C-SEL)				*/
BOOL	GetBitData_Zclamp( void *Axis );					/* 0x0C	: �[���N�����v�M��(/ZCLAMP)				*/
BOOL	GetBitData_Inhibit( void *Axis );					/* 0x0D	: �p���X�w�ߖ����M��(/INHIBIT)			*/
/* <S164> End */
BOOL	GetBitData_Gsel( void *Axis );						/* 0x0E	: G-SEL1�M��(/G-SEL1)					*/
BOOL	GetBitData_Pdet( void *Axis );						/* 0x0F : ���Ɍ��o�w��(/P-DET)					*/
BOOL	GetBitData_Estp( void *Axis );						/* 0x11	: ����~(/E-STP)						*/	/* <S164> */
BOOL	GetBitData_Dec( void *Axis );						/* 0x12 : �����M��(/DEC)						*/
BOOL	GetBitData_Ext1( void *Axis );						/* 0x13 : EXT1�����ݗv��(/EXT1)					*/
BOOL	GetBitData_Ext2( void *Axis );						/* 0x14 : EXT2�����ݗv��(/EXT2)					*/
BOOL	GetBitData_Ext3( void *Axis );						/* 0x15 : EXT3�����ݗv��(/EXT3)					*/
BOOL	GetBitData_Fstp( void *Axis);						/* 0x16 : FSTP�����ݗv��(/FSTP)  <S1F5>					*/
BOOL	GetBitData_Sen( void *Axis );						/* 0x19 : SEN�M��(SEN)							*/
/* <S164> Start */
BOOL	GetBitData_Puls( void *Axis );						/* 0x1A	: �p���X�w�ߓ��͗L��(PULS)				*/
BOOL	GetBitData_Sign( void *Axis );						/* 0x1B	: �����w�ߓ��͗L��(SIGN)				*/
BOOL	GetBitData_Clr( void *Axis );						/* 0x1C	: �΍��N���A�M���L��(CLR)				*/
BOOL	GetBitData_Psel( void *Axis );						/* 0x1D	: �w�߃p���X���͔{���ؑ֐M��(/PSEL)		*/
/* <S164> End */
BOOL	GetBitData_HWBB1( void *Axis );						/* 0x20	: HWBB1�M��(/HWBB1)						*/
BOOL	GetBitData_HWBB2( void *Axis );						/* 0x20	: HWBB2�M��(/HWBB2)						*//* <S064> */
BOOL	GetBitData_AlmFlag( void *Axis );					/* 0x80	: �A���[������(ALM)						*/
BOOL	GetBitData_Coin( void *Axis );						/* 0x81	: �ʒu���ߊ���(/COIN)					*/
BOOL	GetBitData_Vcmp( void *Axis );						/* 0x82	: ���x��v(/V-CMP)						*/
BOOL	GetBitData_Tgon( void *Axis );						/* 0x83	: ���[�^��]���o(/TGON)					*/
BOOL	GetBitData_SvRdy( void *Axis );						/* 0x84	: �T�[�{���f�B(/S-RDY)					*/
BOOL	GetBitData_Clt( void *Axis );						/* 0x85 : �d��������(CLT)						*/
BOOL	GetBitData_Vlt( void *Axis );						/* 0x86 : ���x������(/VLT)						*/
BOOL	GetBitData_Bk( void *Axis );						/* 0x87	: �u���[�L�ێ�(/BK)						*/
BOOL	GetBitData_WrnFlag( void *Axis );					/* 0x88	: ���[�j���O����(/WARN)					*/
BOOL	GetBitData_Near( void *Axis );						/* 0x89	: �ʒu���ߋߖT(/NEAR)					*/
BOOL	GetBitData_Cplue( void *Axis );						/* 0x8A : C���o��(/C-PLUS)						*/
/* <S164> Start */
BOOL	GetBitData_Alo1( void *Axis );						/* 0x8E	: �A���[���R�[�h�o�͂P(AL01)			*/
BOOL	GetBitData_Alo2( void *Axis );						/* 0x8F	: �A���[���R�[�h�o�͂Q(AL02)			*/
BOOL	GetBitData_Alo3( void *Axis );						/* 0x90	: �A���[���R�[�h�o�͂R(AL03)			*/
BOOL	GetBitData_Pao( void *Axis );						/* 0x91	: �����p���X�o�͂`��(PAO)				*/
BOOL	GetBitData_Pbo( void *Axis );						/* 0x92	: �����p���X�o�͂a��(PBO)				*/
BOOL	GetBitData_Pco( void *Axis );						/* 0x93	: �����p���X�o�͂b��(PCO)				*/
BOOL	GetBitData_Psela( void *Axis );						/* 0x94	: �w�߃p���X���͔{���ؑ։���(/PSELA)	*/
/* <S164> End */
BOOL	GetBitData_Acon( void *Axis );						/* 0xC0	: ���H�d���I��(ACON)					*/
BOOL	GetBitData_Refz( void *Axis );						/* 0xC1	: �w�ߕ����o���[��(REFZ)				*/
BOOL	GetBitData_Pdetcmp( void *Axis );					/* 0xC2 : ���Ɍ��o����(PDETCMP)					*/
BOOL	GetBitData_Den( void *Axis );						/* 0xC8	: �w�ߕ����o������(DEN)					*/
BOOL	GetBitData_Pset( void *Axis );						/* 0xC9	: �ʒu���ߊ���(PSET)					*/
BOOL	GetBitData_M3CmdRdy( void *Axis );					/* 0xCA : M-�V�R�}���h���f�B(CMDRDY)			*/
//BOOL	GetBitData_Dummy( void *Axis );						/* ****	: �_�~�[�ϐ�							*/

#endif //__DATA_TRACE_CALC_H

