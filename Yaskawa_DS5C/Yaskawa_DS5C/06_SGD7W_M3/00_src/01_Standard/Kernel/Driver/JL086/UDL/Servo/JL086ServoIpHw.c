/****************************************************************************
Description	: JL086 ServoIP Hardware Definition
----------------------------------------------------------------------------
Author			: Yaskawa Electric Corp.,
					Copyright (c) 2012 All Rights Reserved

Project			: Mercury

Functions		:

----------------------------------------------------------------------------
Changes			:
	Name	Date		Description
  ------------------------------------------------------------------------
	Y.Oka	2012.08.06	created

****************************************************************************/
#ifndef _JL076_USE_

#include "Basedef.h"
#include "HwDeviceIf.h"
#include "JL086ServoIpHw.h"
#include "KLib.h"
#include "EncData.h"/* <S004> */

#include "ARMPF.h"
#include "ARMPF_SCU.h"

//#include "MpConstTbl.h"

/* <S068> */
#define TABLESTART	0xC0007A00
#define TABLESIZE 	64

//<S146>extern CSHORT OVMODTBLG[16][2];
//extern CSHORT OVMODTBLO[16][2];//<S0C7>
/* <S068> */

/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
/* Low Level functions */
void	SHal_InitAsicSystem( ASIC_HW_IF_STR* );
void	SHal_InitAsicAlarm( ASIC_HW_IF_STR* );
void	SHal_InitAsicInterrupt( ASIC_HW_IF_STR* );
void	SHal_InitAsicSencIF( ASIC_HW_IF_STR* );
void	SHal_InitAsicSerialIF( ASIC_HW_IF_STR* );
LONG	SHal_InitAsicPulseIF( ASIC_HW_IF_STR *pAsicHwReg, USHORT prstsw, USHORT syssw1, USHORT comfil );
void	SHal_InitAsicAdInterface( ASIC_HW_IF_STR* pAsicHwReg );
void	SHal_InitAsicTimer( USHORT syssw2, ASIC_HW_IF_STR *pAsicHwReg );
void	SHal_InitAsicRegenerate( ASIC_HW_IF_STR *pAsicHwReg );
void	SHal_InitAsicPwmControl( ASIC_HW_IF_STR *pAsicHwReg, LONG odt_c );



/****************************************************************************************************/
/*																									*/
/*		ASIC�\�t�g�E�F�A���Z�b�g����(�����ʏ���)									<S0E4>			*/
/*																									*/
/****************************************************************************************************/
void	SHal_AsicSwReset( ASIC_HW_IF_STR *pAsicHwReg )
{
	/*----------------------------------------------------------------------------------------------*/
	/*		�N���b�N�@�\�̏�����(JL-076�Ń\�t�g���Z�b�g�O�ɃN���b�N�L�����K�v)						*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SETREG1	= 0x00000005;	/* ASIC Write Enable								*/
	pAsicHwReg->AREG_CSTP		= 0x00000000;	/* ASIC Clock Stop Selection						*/
	pAsicHwReg->AREG_SETREG1	= 0x00000001;	/* ASIC Write Disable								*/

	/*----------------------------------------------------------------------------------------------*/
	/*		�\�t�g���Z�b�g																			*/
	/*----------------------------------------------------------------------------------------------*/
#if 0	/* JL-086A UDL�N���b�N�����s��΍�		*/
	pAsicHwReg->AREG_SETREG1	= 0x00000009;	/* ASIC S/W Reset Enable							*/
	KlibWaitus( 1 );							/* Wait												*/
	pAsicHwReg->AREG_SRESET		= 0x00000000;	/* ASIC Soft Reset									*/
	KlibWaitus( 1 );							/* Wait												*/
	pAsicHwReg->AREG_SETREG1	= 0x00000001;	/* ASIC S/W Reset Disable							*/
#else	/* UDL�N���b�N��~���ĊJ��UDL���Z�b�g�Ɠ��l�̌��ʂ�����		*/
	SCU_CLK_STOP(UDL_SERVO_CLK);
	KlibWaitus(1);
	SCU_CLK_START(UDL_SERVO_CLK);
	KlibWaitus(1);
#endif
}


/****************************************************************************************************/
/*																									*/
/*		ASIC����������A(�p�����[�^��ˑ�)															*/
/*																									*/
/****************************************************************************************************/
void	SHal_InitAsicA( ASIC_HW_IF_STR *AsicHwReg )
{
	SHal_InitAsicSystem( AsicHwReg );				/* �V�X�e������@�\(Clock,WDT,etc)�̏�����		*/
	SHal_InitAsicAlarm( AsicHwReg );				/* �A���[���@�\�̏�����							*/
	SHal_InitAsicInterrupt( AsicHwReg );			/* �����݋@�\�̏�����							*/
	SHal_InitAsicSencIF( AsicHwReg );				/* �V���A���G���R�[�_�h�e������					*/
	SHal_InitAsicSerialIF( AsicHwReg );				/* �V���A���h�e������							*/
	SHal_InitAsicAdInterface( AsicHwReg );			/* �`�c�C���^�[�t�F�C�X������					*/

	return;
}



/****************************************************************************************************/
/*																									*/
/*		ASIC����������B(�p�����[�^�ˑ���)															*/
/*																									*/
/****************************************************************************************************/
BOOL	SHal_InitAsicB( ASIC_HW_IF_STR* pAsicHwReg, SVIP_INIT_PRM *pSvipInitPrm )
{
	BOOL	ret;
	ret = SVIP_NOALM;

	/*------------------------------------------------------------------------------------------*/
	/*		�V���A���G���R�[�_�h�e���W�X�^������												*/
	/*------------------------------------------------------------------------------------------*/
	if( 0 != (pSvipInitPrm->syssw1 & 0x2000) )
	{ /* PnE0B.D == 1 �N���X�g�[�N���� */
		pAsicHwReg->AREG_SEPGSET00 &= (0xffffffff & ~BIT1);
		pAsicHwReg->AREG_SEPGSET10 &= (0xffffffff & ~BIT1);
	}

	/* �p���XIF������ */
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)
	ret = SHal_InitAsicPulseIF( pAsicHwReg,
								pSvipInitPrm->prstsw,
								pSvipInitPrm->syssw1,
								pSvipInitPrm->comfil );
#endif

	/* �^�C�}�@�\�̏����� */
#ifndef CSW_CSTR_DB_REVERSE
	/* �^�C�}�@�\�̏����� <2010.12.2 �폜>
		�_���t�]�ɂ��Ă��邽�߁A��������(WDT����������܂�)�́ADB��L���ɂ��Ȃ� */
	SHal_InitAsicTimer( pSvipInitPrm->syssw2, pAsicHwReg );
#endif

	/* �񐶋@�\�̏�����(�����������̏ꍇ) */
	SHal_InitAsicRegenerate( pAsicHwReg );

	/* PWM����@�\�̏����� */
	SHal_InitAsicPwmControl( pAsicHwReg, pSvipInitPrm->odt_c.b.l );

#ifdef CSW_MICRO_DEBUG_MODE
	/* �e�X�g���[�h����INT2�M���o�͐ݒ� */
	SHal_SetMicroDebugMode( pAsicHwReg );
#endif

	return ret;
}



/****************************************************************************************************/
/*																									*/
/*		ASIC����������B(�p�����[�^�ˑ���)	(�p�����[�^���Z�b�g�p)									*/
/*																									*/
/****************************************************************************************************/
BOOL	SHal_PrstAsicB( ASIC_HW_IF_STR* pAsicHwReg, SVIP_INIT_PRM *pSvipInitPrm )
{
	BOOL	ret;
	ret = SVIP_NOALM;

	/* �p���XIF������ */
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)
	ret = SHal_InitAsicPulseIF( pAsicHwReg,
								pSvipInitPrm->prstsw,
								pSvipInitPrm->syssw1,
								pSvipInitPrm->comfil );
#endif

#ifndef CSW_CSTR_DB_REVERSE
	/* �^�C�}�@�\�̏����� */
	SHal_InitAsicTimer( pSvipInitPrm->syssw2, pAsicHwReg );
#endif

	/* PWM����@�\�̏����� */
	SHal_InitAsicPwmControl( pAsicHwReg, pSvipInitPrm->odt_c.b.l);

	return ret;
}



/****************************************************************************************************/
/*																									*/
/*		�V�X�e������@�\(Clock,WDT,etc)�̏�����														*/
/*																									*/
/****************************************************************************************************/
void	SHal_InitAsicSystem( ASIC_HW_IF_STR *pAsicHwReg )
{

//LONG work;//<S0C7>

#if	(CSW_SVCF_FULC_MODE == CSW_FULC_BASIC_SUPPORT)
	/*----------------------------------------------------------------------------------------------*/
	/*		̨����ޯ���߼�݂ɂ��t���N���[�Y�h����уZ�~�N���[�Y�h�̈ʒu�␳���W���[���ݒ�́A		*/
	/*		2���ڂɐݒ肷��K�v������B																*/
	/*----------------------------------------------------------------------------------------------*//* <S04D> */
ASIC_HW_IF_STR *pAsicHwReg2;

	pAsicHwReg2 = (ASIC_HW_IF_STR*)((UCHAR*)pAsicHwReg + 0x1000);
#endif/* (CSW_SVCF_FULC_MODE == CSW_FULC_BASIC_SUPPORT)	*/

//-	SHal_AsicSwReset()�ł��łɍs���Ă���̂ŕs�v /* <S0E4> */
//-	/*----------------------------------------------------------------------------------------------*/
//-	/*		�N���b�N�@�\�̏�����(JL-076�Ń\�t�g���Z�b�g�O�ɃN���b�N�L�����K�v)						*/
//-	/*----------------------------------------------------------------------------------------------*/
//-	pAsicHwReg->AREG_SETREG1	= 0x00000005;	/* ASIC Write Enable								*/
//-	pAsicHwReg->AREG_CSTP		= 0x00000000;	/* ASIC Clock Stop Selection						*/
//-	pAsicHwReg->AREG_SETREG1	= 0x00000001;	/* ASIC Write Disable								*/

#if	(CSW_SVCF_FULC_MODE == CSW_FULC_BASIC_SUPPORT)
	/*----------------------------------------------------------------------------------------------*/
	/*		̨����ޯ���߼�݂ɂ��t���N���[�Y�h����уZ�~�N���[�Y�h�̈ʒu�␳���W���[���ݒ�́A		*/
	/*		2���ڂɐݒ肷��K�v������B																*/
	/*----------------------------------------------------------------------------------------------*//* <S04D> */
	pAsicHwReg2->AREG_SETREG1	= 0x00000005;	/* ASIC Write Enable								*/
	pAsicHwReg2->AREG_CSTP		= 0x00000000;	/* ASIC Clock Stop Selection						*/
	pAsicHwReg2->AREG_SETREG1	= 0x00000001;	/* ASIC Write Disable								*/
#endif/* (CSW_SVCF_FULC_MODE == CSW_FULC_BASIC_SUPPORT)	*/

//-	SHal_AsicSwReset()�ł��łɍs���Ă���̂ŕs�v /* <S0E4> */
//	/*----------------------------------------------------------------------------------------------*/
//	/*		�\�t�g���Z�b�g																			*/
//	/*----------------------------------------------------------------------------------------------*/
//#if 0	/* JL-086A UDL�N���b�N�����s��΍�		*/
//	pAsicHwReg->AREG_SETREG1	= 0x00000009;	/* ASIC S/W Reset Enable							*/
//	KlibWaitus( 1 );							/* Wait												*/
//	pAsicHwReg->AREG_SRESET		= 0x00000000;	/* ASIC Soft Reset									*/
//	KlibWaitus( 1 );							/* Wait												*/
//	pAsicHwReg->AREG_SETREG1	= 0x00000001;	/* ASIC S/W Reset Disable							*/
//#else	/* UDL�N���b�N��~���ĊJ��UDL���Z�b�g�Ɠ��l�̌��ʂ�����		*/
//	SCU_CLK_STOP(UDL_SERVO_CLK);
//	KlibWaitus(1);
//	SCU_CLK_START(UDL_SERVO_CLK);
//	KlibWaitus(1);
//#endif

	/*----------------------------------------------------------------------------------------------*/
	/*		�N���b�N�@�\�̏�����																	*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SETREG1	= 0x00000005;	/* ASIC Write Enable								*/
	/*----------------------------------------------------------------------------------------------*/
#if 0 /* <S0004> ���l�̏����������ɂ����邽�ߺ��ı�� */
	pAsicHwReg->AREG_CSTP		= 0x04028410;	/* ASIC Clock Stop Selection						*/
												/*	Encoder ADIF, NCT, Full Closed Cntr,			*/
												/*	New Incre Decoder, OC Detect Clock Stop			*/
	
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)
	pAsicHwReg->AREG_CSTP		|= 0x00080000;	/*  M-II Slave I/F Clock Stop						*/
#elif (CSW_NETIF_TYPE == NETIF_M2_TYPE)
//	pAsicHwReg->AREG_CSTP		|= 0x00004000;	/*  Pulse Counter Clock Stop						*/
#else /* (CSW_NETIF_TYPE == NETIF_M3_TYPE || NETIF_CM_TYPE) */
//	pAsicHwReg->AREG_CSTP		|= 0x00084000;	/*  M-II Slave I/F, Pulse Counter Clock Stop		*/
	pAsicHwReg->AREG_CSTP		|= 0x00080000;	/*  M-II Slave I/F									*/
#endif
#endif /* <S0004> ���l�̏����������ɂ����邽�ߺ��ı�� */
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_CSTP		= (0x04028410	/* ASIC Clock Stop Selection						*/
												/*	Encoder ADIF, NCT, Full Closed Cntr,			*/
												/*	New Incre Decoder, OC Detect Clock Stop			*/
								 | (1<<2)		/* bit2: Reference A/D clock Stop 					*/
								 | (1<<14)  	/* bit14: Command Pulse Counter clock Stop			*/
								 | (1<<19)   ); /* bit19: M-II  Slave I/F Clock Stop				*/
								 
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)
	pAsicHwReg->AREG_CSTP		&= ~((1<<2) 	/*  Ref A/D clock enable							*/
									| (1<<14));	/*  Pulse Counter clock enable						*/
#elif (CSW_NETIF_TYPE == NETIF_M2_TYPE)
	pAsicHwReg->AREG_CSTP		&= ~((1<<19)	/*  M-II Slave I/F Clock Enable						*/
									| (1<<14));	/*  Pulse Counter clock enable	(for Latch)			*//* <S0004> */
#else /* (CSW_NETIF_TYPE == NETIF_M3_TYPE || NETIF_CM_TYPE) */
//	pAsicHwReg->AREG_CSTP		&= 0xFFFFFFFF;	/*  												*//* <S0004>�폜 */
	pAsicHwReg->AREG_CSTP		&= ~(1<<14);	/*  Pulse Counter clock enable	(for Latch)			*//* <S0004> */
#endif
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SETREG1	= 0x00000001;	/* ASIC Write Disable								*/

#if	(CSW_SVCF_FULC_MODE == CSW_FULC_BASIC_SUPPORT)
	/*----------------------------------------------------------------------------------------------*/
	/*		̨����ޯ���߼�݂ɂ��t���N���[�Y�h����уZ�~�N���[�Y�h�̈ʒu�␳���W���[���ݒ�́A		*/
	/*		2���ڂɐݒ肷��K�v������B																*/
	/*----------------------------------------------------------------------------------------------*//* <S04D> */
	pAsicHwReg2->AREG_SETREG1	= 0x00000005;	/* ASIC Write Enable								*/
	pAsicHwReg2->AREG_CSTP	&= ~(1<<14);		/* clock enable	(for Position compensate )			*/
	pAsicHwReg2->AREG_CSTP	&= ~(1<<25);		/* clock enable	(for Position compensate )			*/
	pAsicHwReg2->AREG_SETREG1	= 0x00000001;	/* ASIC Write Disable								*/
#endif/* (CSW_SVCF_FULC_MODE == CSW_FULC_BASIC_SUPPORT)	*/

	/*----------------------------------------------------------------------------------------------*/
	/*		���Z��(MicroProgram)�E�H�b�`�h�b�O�@�\���p���X�ϊ��b���}�X�N�r�b�g�̏�����				*/
	/*----------------------------------------------------------------------------------------------*/
//	pAsicHwReg->AREG_WDT1SET	= 0x00000150;	/* Set C-Phase Mask(bit8), WDT Stop(bit6),			*/
	pAsicHwReg->AREG_WDT1SET	= 0x00000168;	/* Set C-Phase Mask(bit8), WDT Stop(bit6),			@@TEST*/
												/*		and Micro WDT Time Over(0x10*16us=256us)	*/
	/*----------------------------------------------------------------------------------------------*/
	/*		�z�X�g(CPU)�E�H�b�`�h�b�O�@�\�̏�����													*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_WDT2SET	= 0x00000020;	/* Set CPU WDT Time Over(64us*0x20=2048us)			*/

	/*----------------------------------------------------------------------------------------------*/
	/*		�A�i���O���j�^�@�\�̏�����																*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_PWMU		= 0x00000000;	/* Set Zero for Initial Out							*/
	pAsicHwReg->AREG_PWMV		= 0x00000000;	/* Set Zero for Initial Out							*/


#if 0
/********************************************************************************************************************************
		DEBUG
********************************************************************************************************************************/ 
	/*----------------------------------------------------------------------------------------------*/
	/*		�N���b�N�@�\�̏�����(JL-076�Ń\�t�g���Z�b�g�O�ɃN���b�N�L�����K�v)						*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg2->AREG_SETREG1	= 0x00000005;	/* ASIC Write Enable								*/
	pAsicHwReg2->AREG_CSTP		= 0x00000000;	/* ASIC Clock Stop Selection						*/
	pAsicHwReg2->AREG_SETREG1	= 0x00000001;	/* ASIC Write Disable								*/

	/*----------------------------------------------------------------------------------------------*/
	/*		�\�t�g���Z�b�g																			*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg2->AREG_SETREG1	= 0x00000009;	/* ASIC S/W Reset Enable							*/
	KlibWaitus( 1 );							/* Wait												*/
	pAsicHwReg2->AREG_SRESET		= 0x00000000;	/* ASIC Soft Reset									*/
	KlibWaitus( 1 );							/* Wait												*/
	pAsicHwReg2->AREG_SETREG1	= 0x00000001;	/* ASIC S/W Reset Disable							*/

	/*----------------------------------------------------------------------------------------------*/
	/*		�N���b�N�@�\�̏�����																	*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg2->AREG_SETREG1	= 0x00000005;	/* ASIC Write Enable								*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg2->AREG_CSTP		= 0x04028410;	/* ASIC Clock Stop Selection						*/
												/*	Encoder ADIF, NCT, Full Closed Cntr,			*/
												/*	New Incre Decoder, OC Detect Clock Stop			*/
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)
	pAsicHwReg2->AREG_CSTP		|= 0x00080000;	/*  M-II Slave I/F Clock Stop						*/
#elif (CSW_NETIF_TYPE == NETIF_M2_TYPE)
	pAsicHwReg2->AREG_CSTP		|= 0x00004000;	/*  Pulse Counter Clock Stop						*/
#else /* (CSW_NETIF_TYPE == NETIF_M3_TYPE || NETIF_CM_TYPE) */
	pAsicHwReg2->AREG_CSTP		|= 0x00084000;	/*  M-II Slave I/F, Pulse Counter Clock Stop		*/
#endif
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg2->AREG_SETREG1	= 0x00000001;	/* ASIC Write Disable								*/
/********************************************************************************************************************************
		DEBUG
********************************************************************************************************************************/
#endif

	return;
}



/****************************************************************************************************/
/*																									*/
/*		�A���[���@�\�̏�����																		*/
/*																									*/
/****************************************************************************************************/
void	SHal_InitAsicAlarm( ASIC_HW_IF_STR *pAsicHwReg )
{
	/*----------------------------------------------------------------------------------------------*/
	/*		�A���[���ݒ�																			*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_ALMSET		= 0x00000000;	/* HWGBON:Invalid, DBONL:DBON						*/

	/*----------------------------------------------------------------------------------------------*/
	/*		�ɐ����t�B���^�ݒ� : OV, OC, VCAP, BBON1, BBON2											*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SETREG1	= 0x00000005;	/* ASIC Write Enable								*/
//	pAsicHwReg->AREG_SETREG2	= 0x000038F0;	/* OV, OC : ActiveLow, Filter valid					*/
												/*	BBON1-2 : ActiveHigh, Filter invalid			*/
												/*	Ondelay Counter Clock : 320MHz					*/
												/*	INT1L Recovery Time : 640ns						*/
//	pAsicHwReg->AREG_SETREG2	= 0x000021F0;	/* Ondelay Counter Clock : 26.7MHz					*//* @@�b�� */
//	pAsicHwReg->AREG_SETREG2	= 0x000029F0;	/* Ondelay Counter Clock : 80MHz					*//* @@�b��,�ݒ�l�ԈႢ�C��<S017> */
	pAsicHwReg->AREG_SETREG2	= 0x00002950;	/* Ondelay Counter Clock : 80MHz					*//* @@�b��,�m�C�Y�Ή���BBON�Ƀt�B���^�ǉ� *//*<S141>*/
	pAsicHwReg->AREG_SETREG2	|= BIT9;		/* Carrer Counter Sync				<S019>				*/
	pAsicHwReg->AREG_SETREG1	= 0x00000001;	/* ASIC Write Disable								*/

	/*----------------------------------------------------------------------------------------------*/
	/*		�A���[���}�X�N�ݒ� : OV����, OC����, OC2����, OC���o									*/
	/*----------------------------------------------------------------------------------------------*/
	//	pAsicHwReg->AREG_SETMSK	= 0x0000000F;	/* All Alarm Mask									*/
#if CSW_BOARD_TYPE == SGD7S_TYPE /* <S0A2> */
	pAsicHwReg->AREG_SETMSK		= 0x0000000D;	/* OV, OC2, OC_UVW Mask								*//* <S01B> */
#else /* CSW_BOARD_TYPE == SGD7W_TYPE *//* <S0A2> */
	pAsicHwReg->AREG_SETMSK		= 0x00000009;	/* OV, OC_UVW Mask									*//* <S0A2> */
#endif /* CSW_BOARD_TYPE == SGD7S_TYPE *//* <S0A2> */
	/*----------------------------------------------------------------------------------------------*/
	/*		�A���[���N���A																			*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_ALMCLR		= 0x0000FFFF;	/* Clear All Alarm									*/

	return;
}



/****************************************************************************************************/
/*																									*/
/*		�����݋@�\�̏�����																			*/
/*																									*/
/****************************************************************************************************/
void	SHal_InitAsicInterrupt( ASIC_HW_IF_STR *pAsicHwReg )
{
	/*----------------------------------------------------------------------------------------------*/
	/*		�����C�l�[�u�����W�X�^�ݒ�																*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_INT1SET	= 0x00000000;	/* Disable All Interrupt							*/

	/*----------------------------------------------------------------------------------------------*/
	/*		�����p�^�C�}���W�X�^�ݒ�																*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_INT0SET	= 0x00000000;	/* INT0 Frequency									*/

	/*----------------------------------------------------------------------------------------------*/
	/*		�e���荞�ݒx�����Ԑݒ�																	*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_ENCTXDLY0	= 0x00000000;	/* CH0 Tx Delay : 0ns								*/
	pAsicHwReg->AREG_ENCTXDLY1	= 0x00000000;	/* CH1 Tx Delay : 0ns								*/
	/*----------------------------------------------------------------------------------------------*/
	/*		�G���R�[�_���[�h�����ݒx�����Ԑݒ�														*/
	/*		�����l�ɂ�0���i�[���Ă����A�G���R�[�_�̒ʐM����(8Mbps/4Mbps)�ɏ]���čĐݒ肷��			*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_ENCRDDLY0	= 0x00000000;	/* CH0 Read Interrupt Delay : 0ns <S009><S09C>		*/
	pAsicHwReg->AREG_ENCRDDLY1	= 0x00000000;	/* CH1 Read Interrupt Delay : 0ns					*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_INTASET	= 0x00000078;	/* ScanA Interrupt Delay : 1500ns					*/
												/* (jitter & Position Compensation Delay)			*/
	pAsicHwReg->AREG_INTBSET	= 0x000000C8;	/* ScanB Interrupt Delay : 2500ns (ScanA + 1.0us)	*/
	pAsicHwReg->AREG_INTCSET	= 0x00000118;	/* ScanC Interrupt Delay : 3500ns (ScanB + 1.0us)	*/
	pAsicHwReg->AREG_INTNSET	= 0x00000168;	/* ScanN Interrupt Delay : 4500ns (ScanC + 1.0us)	*/
												/* ScanN is non-use									*/
	/*----------------------------------------------------------------------------------------------*/
	/*		�e���荞�ݎ����ݒ�																		*/
	/*----------------------------------------------------------------------------------------------*/
//	pAsicHwReg->AREG_INTCYCLE	= 0x00011f10;	/* ScanA = Encoder Communication Cycle		62.5us	*/
												/* ScanB = Encoder Communication Cycle * 2	125us	*/
												/* ScanC = Encoder Communication Cycle * 32	2ms		*/
												/* ScanN = Encoder Communication Cycle * 2	125us	*/
	pAsicHwReg->AREG_INTCYCLE	= 0x00000000;	/* ���荞�݋N����ɍĐݒ�@					<S1D3>	*/

	/*----------------------------------------------------------------------------------------------*/
	/*		�ʃv���O�����G���R�[�_�������荞�ݐݒ�													*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_INTMPESET	= 0x00000000;	/* ScanA������ɏ������������ߎg�p���Ȃ�			*/

	/*----------------------------------------------------------------------------------------------*/
	/*		�O���ւ̊��荞�ݐM���o�͑I��															*/
	/*----------------------------------------------------------------------------------------------*/
#if (CSW_NETIF_TYPE == NETIF_M3_TYPE)
	pAsicHwReg->AREG_INTSET		= 0x00000005;	/* MECHATROLINK-3									*/
#else
	pAsicHwReg->AREG_INTSET		= 0x00000000;	/* MECHATROLINK-2 or Non-use						*/
#endif
	return;
}



/****************************************************************************************************/
/*																									*/
/*		�V���A���G���R�[�_�h�e������																*/
/*																									*/
/****************************************************************************************************/
void	SHal_InitAsicSencIF( ASIC_HW_IF_STR *pAsicHwReg )
{
	/*----------------------------------------------------------------------------------------------*/
	/*		�V���A���G���R�[�_�I��																	*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_PFNCSET	&= 0xFFFFFF80;	/* �[�q�@�\�ݒ� : SESEL0:S-PG0, SESEL1:S-PG1		*/

	/*----------------------------------------------------------------------------------------------*/
	/*		�V���A���G���R�[�_�h�e���W�X�^������													*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SEPGSET00	= 0x00000006;	/* CH0,1���� CrossTalk Avoidance ON, Delay Sync		*/
	pAsicHwReg->AREG_SEPGSET10	= 0x00000006;	/* CH0,1���� ManchesterCode Squelch ON				*/
#if 0/* �ʐM�J�n���ɓ����v�����s���B�������ł́A�f�B�Z�[�u���Ƃ���B	*/
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)
	/* Nothing Additional Establishment */
#elif (CSW_NETIF_TYPE == NETIF_M2_TYPE)
	pAsicHwReg->AREG_SEPGSET00	|= 0x00000020;	/*  Encoder CH0 Sync with M-II Network				*/
	pAsicHwReg->AREG_SEPGSET10	|= 0x00000020;	/*  Encoder CH1 Sync with M-II Network				*/
#elif (CSW_NETIF_TYPE == NETIF_M3_TYPE)
	pAsicHwReg->AREG_SEPGSET00	|= 0x00000080;	/*  Encoder CH0 Sync with M-III Network				*/
	pAsicHwReg->AREG_SEPGSET10	|= 0x00000080;	/*  Encoder CH1 Sync with M-III Network				*/
#else /* (CSW_NETIF_TYPE == NETIF_CM_TYPE) */
	pAsicHwReg->AREG_SEPGSET00	|= 0x00000001;	/*  Encoder CH0 Sync with SYNCL Signal				*/
	pAsicHwReg->AREG_SEPGSET10	|= 0x00000001;	/*  Encoder CH1 Sync with SYNCL Signal				*/
#endif
#endif
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SEPGSET01	= 0x00000001;	/* CH0,1���� Slave Address : 01h					*/
	pAsicHwReg->AREG_SEPGSET11	= 0x00000001;	/* CH0,1���� 16Byte P0 Master Mode					*/

	/*----------------------------------------------------------------------------------------------*/
	/*		�x���������Ԑݒ�																		*/
	/*----------------------------------------------------------------------------------------------*/
//	pAsicHwReg->AREG_SEDLLD		= 0x00000000;	/* Delay Sync Time : (n + 1) * 125 = 125[ns]
	pAsicHwReg->AREG_SEDLLD		= 0x0000001F;	/* Delay Sync Time : (0x1F + 1) * 125 = 4000[ns]	*/

	/*----------------------------------------------------------------------------------------------*/
	/*		�V���A���G���R�[�_���M�f�[�^���W�X�^������												*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SE0_TXD01	= 0x00000000;	/* CH0 Transmit Register							*/
	pAsicHwReg->AREG_SE0_TXD23	= 0x00000000;	/* CH0 Transmit Register							*/
	pAsicHwReg->AREG_SE0_TXD45	= 0x00000000;	/* CH0 Transmit Register							*/
	pAsicHwReg->AREG_SE0_TXD67	= 0x00000000;	/* CH0 Transmit Register							*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SE1_TXD01	= 0x00000000;	/* CH1 Transmit Register							*/
	pAsicHwReg->AREG_SE1_TXD23	= 0x00000000;	/* CH1 Transmit Register							*/
	pAsicHwReg->AREG_SE1_TXD45	= 0x00000000;	/* CH1 Transmit Register							*/
	pAsicHwReg->AREG_SE1_TXD67	= 0x00000000;	/* CH1 Transmit Register							*/

	return;
}



/****************************************************************************************************/
/*																									*/
/*		�V���A���h�e������																			*/
/*																									*/
/****************************************************************************************************/
void	SHal_InitAsicSerialIF( ASIC_HW_IF_STR *pAsicHwReg )
{
	/*----------------------------------------------------------------------------------------------*/
	/*		�ėp�����V���A���h�e�̏�����															*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_IOSTAT	= KPX_AREG_IOSTAT_INIOUT;	/* Set Initial Output Data to High Word	*/

	/*----------------------------------------------------------------------------------------------*/
	/*		�񓯊��V���A���h�e�̏�����																*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_UARTSTS	= 0x00000000;	/* UART Status Register								*/

	return;
}



/****************************************************************************************************/
/*																									*/
/*		�p���X�h�e������																			*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		ASIC �w�߃J�E���^�ݒ�l�e�[�u��																*/
/*--------------------------------------------------------------------------------------------------*/
const	USHORT	AsicPocsetTable[7] =
{
	0x0650,		/* 0 : ~A,~B, 101=Sign+Pulse		*/
	0x0640,		/* 1 : ~A,~B, 100=CW+CCW			*/
	0x0630,		/* 2 : ~A,~B, 011=(A+B)*1			*/
	0x0610,		/* 3 : ~A,~B, 001=(A+B)*2			*/
	0x0600,		/* 4 : ~A,~B, 000=(A+B)*4			*/
	0x0050,		/* 5 :  A, B, 101=Sign+Pulse		*/
	0x0040,		/* 6 :  A, B, 100=CW+CCW			*/
};
/*--------------------------------------------------------------------------------------------------*/
LONG	SHal_InitAsicPulseIF( ASIC_HW_IF_STR *pAsicHwReg, USHORT prstsw, USHORT syssw1, USHORT comfil )
{
	LONG	i;
	USHORT	uswk;
	LONG	ret;
	ret = FALSE;

/*--------------------------------------------------------------------------------------------------*/
/*		�w�߃J�E���^�ݒ�																			*/
/*--------------------------------------------------------------------------------------------------*/
	i = prstsw & 0x0F;									/* �w�߃p���X�`��							*/
	if( i > 6 )											/* �p�����[�^�ݒ�͈̓`�F�b�N				*/
	{
		uswk = 0;
		ret = TRUE;										/* �p�����[�^�ݒ�G���[						*/
	}
	else
	{
		uswk = AsicPocsetTable[i];						/* �J�E���g���[�h,etc						*/
	}
	/*----------------------------------------------------------------------------------------------*/
	if( ((prstsw >> 4) & 0x0F) < 2 )
	{
		uswk = ( uswk | 0x0800 );						/* CC�ɐ��ݒ�								*/
	}

/*--------------------------------------------------------------------------------------------------*/
/*		CA,CB �t�B���^�ݒ�																			*/
/*--------------------------------------------------------------------------------------------------*/
	switch( (prstsw >> 12) & 0x0F )
	{
	case 0x00:											/* ���C���h���C�o�M���p�w�ߓ��̓t�B���^1	*/
		uswk = (uswk | ((syssw1 >> 4) & 0x3) );			/* (PnE0B.4-5)�ɂĐݒ�						*/
		break;
	case 0x01:											/* �I�[�v���R���N�^�M���p�w�ߓ��̓t�B���^	*/
		uswk = (uswk | ((comfil) & 0x3) );				/* (PnE0E.0-1)�ɂĐݒ�						*/
		break;
	case 0x02:											/* ���C���h���C�o�M���p�w�ߓ��̓t�B���^2	*/
		uswk = (uswk | ((comfil >> 8) & 0x3) );			/* (PnE0E.8-9)�ɂĐݒ�						*/
		break;
	default:
		uswk = (uswk | ((syssw1 >> 4) & 0x3) );			/* (PnE0B.4-5)�ɂĐݒ�						*/
		break;
	}
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_POCSET = uswk;						/* �w�߃J�E���^�ݒ�							*/
	uswk = ( uswk | 0x0100 );							/* CLM���b�`�C�l�[�u���ݒ�					*/
	pAsicHwReg->AREG_POCSET = uswk;						/* �w�߃J�E���^�ݒ�							*/
	pAsicHwReg->AREG_LODSET = LODSET_SET_LDCMD;			/* �J�E���^���[�h�ݒ�						*/

/*--------------------------------------------------------------------------------------------------*/
/*		�w�߃t�B���^�ݒ�																			*/
/*--------------------------------------------------------------------------------------------------*/
	switch( (prstsw >> 12) & 0x0F )
	{
	case 0x00:											/* ���C���h���C�o�M���p�w�ߓ��̓t�B���^1	*/
		pAsicHwReg->AREG_FCCSET = syssw1 & 0x0F00;		/* (PnE0B.8-11)�ɂĐݒ�						*/
		break;
	case 0x01:											/* �I�[�v���R���N�^�M���p�w�ߓ��̓t�B���^	*/
		pAsicHwReg->AREG_FCCSET = (comfil<<4) & 0x0F00;	/* (PnE0E.4-7)�ɂĐݒ�						*/
		break;
	case 0x02:											/* ���C���h���C�o�M���p�w�ߓ��̓t�B���^2	*/
		pAsicHwReg->AREG_FCCSET = (comfil>>4) & 0x0F00;	/* (PnE0E.12-15)�ɂĐݒ�					*/
		break;
	default:
		pAsicHwReg->AREG_FCCSET = syssw1 & 0x0F00;		/* (PnE0B.8-11)�ɂĐݒ�						*/
		break;
	}

#if 0 /* 2012.11.01 Y.Oka (>_<) �����o�͌��󖢑Ή� */
�������ݒ�͕ʊ֐��ɕ����邱��
/*--------------------------------------------------------------------------------------------------*/
/*		�V���A���G���R�[�_�p���X�ϊ��ݒ�(Bit��,�ϊ�����)											*/
/*--------------------------------------------------------------------------------------------------*/
		if( Kprm.f.FencUse == TRUE )					/* �t���N�G���R�[�_�����o��					*/
		{
			ASIC_WR( AREG_PLSET, 0x3400 );				/* �ϊ�Bit��:16bit, �ϊ�����:64us			*//* <V501> */
		}
		else if( MencP.f.PlsTrn13bit == TRUE )			/* �p���X�ϊ�13bit							*/
		{
			ASIC_WR( AREG_PLSET, 0x3100 );				/* �ϊ�Bit��:13bit, �ϊ�����:64us			*/
		}
		else
		{
			ASIC_WR( AREG_PLSET, 0x3400 );				/* �ϊ�Bit��:16bit, �ϊ�����:64us			*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�V���A���G���R�[�_�p���X�ϊ��ݒ�(�ʒu,���_,etc)												*/
/*--------------------------------------------------------------------------------------------------*/
		KPI_PCMASK_ON( );								/* �p���X�ϊ��b���}�X�N(�O�̂���)			*/
		ASIC_WR( AREG_DIVSET, 0x0102 );					/* �p���X�ϊ������ݑ��ݒ�:HostCPU			*//* <JL076_TEST> */
		ASIC_WR( AREG_POSET0, 0xFFFF );					/* �p���X�ϊ��ʒu�ݒ�						*/
		ASIC_WR( AREG_POSET0H, 0x0000 );				/* �p���X�ϊ��ʒu�ݒ�bit16-23				*/
		ASIC_WR( AREG_POTEST, 0xFFFF );					/* �p���X�ϊ��J�E���^�ݒ�					*/
		ASIC_WR( AREG_POSET1, 0x0000 );					/* �p���X�ϊ����_�␳�P						*/
		ASIC_WR( AREG_POSET2, 0xFFFF );					/* �p���X�ϊ����_�␳�Q						*/

/*--------------------------------------------------------------------------------------------------*/
/*		�������̓t�B���^�ݒ�																		*/
/*--------------------------------------------------------------------------------------------------*/
		ASIC_WR( AREG_FBCSET, 0x0005 );					/* �������̓t�B���^�ݒ�						*/

/*--------------------------------------------------------------------------------------------------*/
/*		�����p���X�v�Z���`�r�h�b�ݒ�																*/
/*--------------------------------------------------------------------------------------------------*/
		if( MencP.f.DivOut == TRUE )
		{
			KpxSetMencDividedPulse( );					/* ���[�^�G���R�[�_�����p���X�v�Z���ݒ�		*/
		}
		else if( FencP.f.DivOut == TRUE )
		{
			KpxSetFencDividedPulse( );					/* �t���N�G���R�[�_�����p���X�v�Z���ݒ�		*/
		}
#endif
/*--------------------------------------------------------------------------------------------------*/
	return ret;
}



/****************************************************************************************************/
/*																									*/
/*		A/D�C���^�[�t�F�C�X������																	*/
/*																									*/
/****************************************************************************************************/
void	SHal_InitAsicAdInterface( ASIC_HW_IF_STR *pAsicHwReg )
{
/*--------------------------------------------------------------------------------------------------*/
/*		����A/D�ݒ�(�w��A/D & �d�����oA/D)															*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_ADSET1		= 0x00001043;	/* Command AD 16Bit AD I/F							*/
												/* Current AD 10Bit Decimation Filter				*/
												/* Current AD Sync Invalid							*/

/*--------------------------------------------------------------------------------------------------*/
/*		�w��A/D�C���^�[�t�F�C�X������																*/
/*--------------------------------------------------------------------------------------------------*/
//	pAsicHwReg->AREG_ADSET2		= 0x0000C0CE;	/* AD Clock, AD Frequency, Base Frequency			*/
	pAsicHwReg->AREG_ADSET2		= 0x0000D4CE;	/* AD Clock, AD Frequency, Base Frequency			*/
//	pAsicHwReg->AREG_ADSET3		= 0x00008280;	/* AD Start Timing, AD I/F Cycle					*/
	pAsicHwReg->AREG_ADSET3		= 0x0000A7A0;	/* AD Start Timing, AD I/F Cycle					*/
//	pAsicHwReg->AREG_ADSET4		= 0x0000DADA;	/* AD Latch Timing, AD Latch Timing					*/
	pAsicHwReg->AREG_ADSET4		= 0x0000FEFE;	/* AD Latch Timing, AD Latch Timing					*/
//	pAsicHwReg->AREG_ADSET5		= 0x000080DB;	/* AD Control H to L Edge, L to H Edge				*/
	pAsicHwReg->AREG_ADSET5		= 0x0000A5A1;	/* AD Control H to L Edge, L to H Edge				*/
//	pAsicHwReg->AREG_ADSET6		= 0x000080DB;	/* AD Control H to L Edge, L to H Edge				*/
	pAsicHwReg->AREG_ADSET6		= 0x0000A5A1;	/* AD Control H to L Edge, L to H Edge				*/

/*--------------------------------------------------------------------------------------------------*/
/*		�d�����oA/D�C���^�[�t�F�C�X������			   												*/
/*--------------------------------------------------------------------------------------------------*/
	/* Mode2 */									/* �d��������� = 15.625us(64kHz)					*/
	pAsicHwReg->AREG_ADFCNT1	= 0x000000FB;	/* �d��ADIF��{�N���b�N���g�� = 3.2MHz				*/
	pAsicHwReg->AREG_ADFCNT2	= 0x00000038;	/* ADIF Cycle, (256 - 38h) / 3.2MHz = 62.5us		*/
	pAsicHwReg->AREG_ADFCNT3	= 0x00000000;	/* AD Latch Timing, AD Latch Timing					*/
	pAsicHwReg->AREG_ADFCNT4	= 0x00003839;	/* AD Control H to L Edge, L to H Edge				*/
	pAsicHwReg->AREG_ADFCNT5	= 0x00000000;	/* AD Control H to L Edge, L to H Edge				*/

/*--------------------------------------------------------------------------------------------------*/
/*		A/D�f�V���[�V�����t�B���^������(�d�����oA/D & �d�����oA/D)									*/
/*--------------------------------------------------------------------------------------------------*/
//	pAsicHwReg->AREG_SDMCONF1	= 0x00001864;	/* CH1,2,3 Mode2, �d�����o����\ = 12.8bit			*/
												/* CH1�Œ��A/D�ϊ��������荞��				*/
if( CSW_DECIMATION == 0x0000 )	/* <S202> */
{
	pAsicHwReg->AREG_SDMCONF1	= 0x0000187B;	/* CH1,2 Mode3, �d�����o����\ = 10.9bit				*//* <S1F2> */
}												/* CH3 Mode1, �d�����o����\= 13.5bit				*//* <S1F2> */
else
{
	pAsicHwReg->AREG_SDMCONF1	= 0x0000185B;	/* CH1,2,3 Mode3, �d�����o����\ = 10.9bit			*//* @@ */
}												/* CH1�Œ��A/D�ϊ��������荞��						*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SDMCONF2	= 0x00003A47;	/* �v���g���K�v�Z:1/8��, �����:4					*/
												/* CH1,2:20MHz Clock Out, CH3 External Clock In		*/
												/* Over range Detect:Mask							*/
/*--------------------------------------------------------------------------------------------------*/
if( CSW_DECIMATION == 0x0000 )	/* <S202> */
{
	pAsicHwReg->AREG_SDMCONF3	= 0x00001F06;	/* �d�����o���� = 250us(4kHz)							*//* <S1F2> */
}												/* �d��ADIF��{�N���b�N���g�� = 1.6MHz					*/
												/* ADIF Cycle, (256-06h)*(16-Fh)/16 = 250us			*//* <S1F2> */
else
{
	pAsicHwReg->AREG_SDMCONF3	= 0x00001638;	/* �d�����o���� = 125us(8kHz)							*/
}
												/* MSB���]����										*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SDMCONF4	= 0x00003839;	/* AD Control H to L Edge, L to H Edge				*/

/*--------------------------------------------------------------------------------------------------*/
/*		OC���o�f�V���[�V�����t�B���^������	OC���o ���e������ 2013.07.12 Y.Oka (>_<)				*/
/*--------------------------------------------------------------------------------------------------*/
//	pAsicHwReg->AREG_OCDCONF1	= 0x0000000F;	/* CH1,2 Mode3 �d�����o����\ = 11.9bit				*/	/* <S1F7> */
//	pAsicHwReg->AREG_OCSDMCONF3	= 0x00000000;	/* @@												*/	/* <S1F7> */
//	pAsicHwReg->AREG_OCSDMCONF4	= 0x00000000;	/* @@												*/	/* <S1F7> */

/*--------------------------------------------------------------------------------------------------*/
/*		OC���o���x���ݒ�																			*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_OCLVUP		= 0x0000FFFF;	/* @@U�����OC���o���x��							*/
	pAsicHwReg->AREG_OCLVUM		= 0x0000FFFF;	/* @@U�����OC���o���x��							*/
	pAsicHwReg->AREG_OCLVVP		= 0x0000FFFF;	/* @@U�����OC���o���x��							*/
	pAsicHwReg->AREG_OCLVVM		= 0x0000FFFF;	/* @@U�����OC���o���x��							*/
	pAsicHwReg->AREG_OCLVWP		= 0x0000FFFF;	/* @@U�����OC���o���x��							*/
	pAsicHwReg->AREG_OCLVWM		= 0x0000FFFF;	/* @@U�����OC���o���x��							*/


	return;
}



/****************************************************************************************************/
/*																									*/
/*		�^�C�}�@�\�̏�����																			*/
/*																									*/
/****************************************************************************************************/
void	SHal_InitAsicTimer( USHORT syssw2, ASIC_HW_IF_STR *pAsicHwReg )
{
/*--------------------------------------------------------------------------------------------------*/
/*		DB�p���X�ݒ�FDB���� = �T�C���X�^															*/
/*--------------------------------------------------------------------------------------------------*/
	if( syssw2 & 0x0010 )						/* PnE0C.5:4 = x:1 : �T�C���X�^						*/
	{											/* LowByte:Frequency,  HighByte:HighPulseWidth		*/
		pAsicHwReg->AREG_TIM1SET = 0x0000EB9C;	/* TB=(256 - 156)[us], TW=(255 - 235)[us]			*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		DB�p���X�ݒ�FDB���� = �����[																*/
/*--------------------------------------------------------------------------------------------------*/
	else										/* PnE0C.5:4 = x:0 : �����[							*/
	{											/* LowByte:Frequency,  HighByte:HighPulseWidth		*/
		pAsicHwReg->AREG_TIM1SET = 0x0000009C;	/* TB=(256 - 156)[us], TW=(255 - 0)[us]				*/
	}

	return;
}



/****************************************************************************************************/
/*																									*/
/*		�񐶋@�\�̏�����																			*/
/*																									*/
/****************************************************************************************************/
void	SHal_InitAsicRegenerate( ASIC_HW_IF_STR *pAsicHwReg )
{
/*--------------------------------------------------------------------------------------------------*/
/*		�񐶐ݒ�																					*/
/*--------------------------------------------------------------------------------------------------*/
/*		Bit13 :	REGOFF	: 0:�񐶐�������, 1:�񐶐������Ȃ�											*/
/*		Bit14 :	DISCH	: 0:�ʏ퓮��,     1:�}�����d�J�n											*/
/*		Bit15 : GFTON   : 0:�ʏ퓮��,     1:�n���e�X�g(BB�������L��)								*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SETREG3	= 0x00000000;	/* REGOFF=0, DISCH=0, GFTON=0						*/

	return;
}



/****************************************************************************************************/
/*																									*/
/*		PWM����@�\�̏�����																			*/
/*																									*/
/****************************************************************************************************/
void	SHal_InitAsicPwmControl( ASIC_HW_IF_STR *pAsicHwReg, LONG odt_c )
{
	ULONG	OnDelay;

/*--------------------------------------------------------------------------------------------------*/
/*		�I���f�B���C���Ԑݒ�																		*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SETREG1	= 0x00000005;	/* ASIC Write Enable								*/
//	pAsicHwReg->AREG_SETREG2	&= (~BIT12);	/* Clock Select : 80MHz								*/
//	pAsicHwReg->AREG_SETREG2	|= (BIT11);		/* Clock Select : 80MHz								*/
/* for debug (INGRAM�ł͕K�v�ł�����) */
//	pAsicHwReg->AREG_SETREG2		|= BIT9;	/* Carrer Counter Sync								*/
/* for debug (INGRAM�ł͕K�v�ł�����) */
	/*----------------------------------------------------------------------------------------------*/
	OnDelay = (ULONG)((ASIC_CLKMHZ * odt_c) / 10);	/* 320[MHz] * Td[0.1us] / 10					*/
	pAsicHwReg->AREG_DYSETUP	= OnDelay;		/* UP1 : U-Phase,Positive-Side						*/
	pAsicHwReg->AREG_DYSETUN	= OnDelay;		/* UN1 : U-Phase,Negative-Side						*/
	pAsicHwReg->AREG_DYSETVP	= OnDelay;		/* VP1 : V-Phase,Positive-Side						*/
	pAsicHwReg->AREG_DYSETVN	= OnDelay;		/* VN1 : V-Phase,Negative-Side						*/
	pAsicHwReg->AREG_DYSETWP	= OnDelay;		/* WP1 : W-Phase,Positive-Side						*/
	pAsicHwReg->AREG_DYSETWN	= OnDelay;		/* WN1 : W-Phase,Negative-Side						*/
	pAsicHwReg->AREG_SETREG1	= 0x00000001;	/* ASIC Write Disable								*/

	return;
}

/****************************************************************************************************/
/*																									*/
/*		�G���R�[�_���[�h������ch0�x�����Ԃ̏�����													*/
/*																									*/
/****************************************************************************************************//*<S09C>*/
void SHal_InitEncRdDly_ch0( ASIC_HW_IF_STR *pAsicHwReg, USHORT PgBaudRate )
{
	LONG delay;

	/* �G���R�[�_���[�h������ch0�x�����Ԑݒ� */
	if( PgBaudRate == SENC_COMRATE_8MBPS )
	{
		delay = ENCRDDLY_8MBPS;
	}
	else
	{
		delay = ENCRDDLY_4MBPS;
	}
	/* �G���R�[�_���[�h������ch0�x�����Ԑݒ� */
	pAsicHwReg->AREG_ENCRDDLY0	= delay;	/* CH0 Read Interrupt Delay */

}

/****************************************************************************************************/
/*																									*/
/*		�G���R�[�_���[�h������ch1�x�����Ԃ̏�����													*/
/*																									*/
/****************************************************************************************************//*<S09C>*/
void SHal_InitEncRdDly_ch1( ASIC_HW_IF_STR *pAsicHwReg, USHORT PgBaudRate )
{
	LONG delay;

	/* �G���R�[�_���[�h������ch1�x�����Ԑݒ� */
	if( PgBaudRate == SENC_COMRATE_8MBPS )
	{
		delay	= ENCRDDLY_8MBPS;
	}
	else
	{
		delay	= ENCRDDLY_4MBPS;
	}
	pAsicHwReg->AREG_ENCRDDLY1	= delay;	/* CH1 Read Interrupt Delay */
}

/****************************************************************************************************/
/*																									*/
/*		�G���R�[�_���[�h�����ݒx�����Ԃ̏�����														*/
/*																									*/
/****************************************************************************************************//*<S09C>*/
//void SHal_InitEncRdDly( ASIC_HW_IF_STR *pAsicHwReg, USHORT PgBaudRate ) /* <S0E4> */
void SHal_InitEncRdDly( ASIC_HW_IF_STR *pAsicHwReg, USHORT PgBaudRate, UINT ch ) /* <S0E4> */
{
//	if(pAsicHwReg->AREG_ENCRDDLY0 == 0)	/* SGD7S�̂Ƃ��A�����SGD7W 0���ڂ̂Ƃ�						*//* <S0E4> */
	if(ch == 0)	/* SGD7S�̂Ƃ��A�����SGD7W 0���ڂ̂Ƃ� *//* <S0E4> */
	{
		/* �G���R�[�_���[�h������ch0�x�����Ԑݒ� */
		SHal_InitEncRdDly_ch0( pAsicHwReg, PgBaudRate );
	}
	else								/* SGD7W 1���ڂ̂Ƃ�										*/
	{
		/* �G���R�[�_���[�h������ch1�x�����Ԑݒ� */
		SHal_InitEncRdDly_ch1( pAsicHwReg, PgBaudRate );
	}
}
/****************************************************************************************************/
/*																									*/
/*		�G���R�[�_�ʒu�␳�@�\�̏�����																*/
/*																									*/
/****************************************************************************************************//* <S004> */
void SHal_InitEncPosComp( ASIC_HW_IF_STR *pAsicHwReg, SVIP_INIT_PCOMP *SvipInitPcomp )/*<S09C>�ϐ����ύX*/
{
	UCHAR BitNo;
	UCHAR Dpoint;
	LONG PosSftR;
	LONG PosSftX;
	LONG PosSign;
	LONG PosLeng;

	BitNo  = SvipInitPcomp->bit_dp.b.l;
	Dpoint = SvipInitPcomp->bit_dp.b.h;

	if( SvipInitPcomp->AxisMotType  == MOTTYPE_ROTARY)
	{	/* ��]�`�̏ꍇ */
		PosLeng = 0;
		PosSftR = 24 - BitNo;
		PosSftX = 32 - BitNo - (80 - Dpoint);	/* JL-086�̎d�l���ƈقȂ�̂Œ���(�d�l���̃~�X)	*//* <S210>:���ʂ����]�`�݂̂� */
	}
	else
	{	/* ���j�A�̏ꍇ */
		PosLeng = HPOS_TO_XPOS;
		PosSftR = 24 - PosLeng;
		PosSftX = LMX_MPOS_SFT;					/* <S210> */
	}
	PosSign = (SvipInitPcomp->RevAsmMotor == TRUE) ? 1 : 0;

	/* �G���R�[�_�␳�ݒ肨��уA���[���FEncFlag(0390h)	*/
	pAsicHwReg->AREG_ENCFLAG = ( ENCINT << TRGSEL )						/* �N�����@�ݒ� 			*/
							 + ( SvipInitPcomp->AxisMotType<<MOT_AMPTYPE )/* ��]�^/���j�A�^		*/
							 + ( POSDATSEL_11 << POSDAT_SEL );			/* �ʒu�␳�C�l�[�u���ݒ�	*/

	/* �V�t�g�ʐݒ�FPGDCAL(0394h)					*/
	pAsicHwReg->AREG_PGDCAL = ( PosSftX << POS_SFTX )					/*�@�ʒu�f�[�^�V�t�g��1		*/
							+ ( PosSftR << POS_SFTR )					/*�@�ʒu�f�[�^�V�t�g��2		*/
							+ ( PosLeng << POS_LENG )					/*�@�ʒu�f�[�^��			*/
							+ ( PosSign << POS_SIGN );					/*�@�ʒu�f�[�^����			*/

	/* 1��]�G���R�[�_�p���X���ݒ�:PulseNo(0398h)	*/
	pAsicHwReg->AREG_PULSENO = SvipInitPcomp->PulseNo;

	/* �����x�G���[���x��:AccErrLvl(03A0h)			*/
	pAsicHwReg->AREG_ACCERRLVL = SvipInitPcomp->AccErrLvl;

	/* �݌v�G���[���x��:SPGFAIL(03A8h)	 			*/
	pAsicHwReg->AREG_SPGFAIL = SvipInitPcomp->spgfail.b.l;

	/* ���b�`�������N���b�N��:LchClkNum(03ACh) 		*/
	pAsicHwReg->AREG_LCHCLKNUM = ENCLCHCLKNUM;

	/* ���Z�������N���b�N��:SaclkNum(03B0h) 		*/
	pAsicHwReg->AREG_SACLKNUM = SAENCCLKNUM;

	/* ���Z�b�g�ݒ�:RSTSET(03CCh)	 				*/
	pAsicHwReg->AREG_RSTSET = ( 1 << ALMCLR0 )					/* ComAlarm�N���A					*/
							+ ( 1 << ALMCLR1 )					/* AccAlarm�N���A					*/
							+ ( 1 << ACENCLR )					/* �����x�`�F�b�N�C�l�[�u���N���A	*/
							+ ( 1 << SRECLR )					/* ��M�G���[�t���O�N���A			*/
							+ ( 1 << SRCCLR );					/* ��M�����t���O�N���A				*/
}

/****************************************************************************************************/
/*																									*/
/*		�G���R�[�_�ʒu�␳�@�\(�����x�`�F�b�N���x��)�̏�����										*/
/*																									*/
/* �t�B�[�h�o�b�N�I�v�V�����֘A�́ACh1���g�p����B													*/
/* Ch1�ňʒu�␳���W���[�����g�p����ꍇ�́A1����(+0x1000)�̗̈�ɐݒ肷��K�v������B				*/
/****************************************************************************************************//* <S004> */
void SHal_InitEncPosCompAccLvl( ASIC_HW_IF_STR *pAsicHwReg )
{
	/* �����l0�̂��߃G���R�[�_�ɐڑ�����O�Ɏb��l��ݒ肵�Ă����B	*/
	/* �����x�G���[�����o���Ă��܂�����							*/

	/* �����x�G���[���x��:AccErrLvl(03A0h)			*/
	pAsicHwReg->AREG_ACCERRLVL = 0x007FFFFF;

}
/****************************************************************************************************/
/*																									*/
/*		�^�C�}���b�`�@�\�̏�����																	*/
/*																									*/
/****************************************************************************************************//* <S004> */
BOOL SHalDrv_InitLatchSignal(ASIC_HW_IF_STR *AsicHwReg, SHAL_LATCH_STS_ADR* LtStsAdr,
												USHORT PrmInselA, BOOL* ExtLtEnable)
{
LONG	i;
BOOL	rc;
UCHAR	prmSet[3];
UCHAR	prmChek[3];

	/* ���[�J���ϐ��ݒ�	*/
	prmSet[0] = (PrmInselA >>  4) & 0x0F;
	prmSet[1] = (PrmInselA >>  8) & 0x0F;
	prmSet[2] = (PrmInselA >> 12) & 0x0F;
	rc = TRUE;

	/* �O�����b�`���͐M���I��	*/
	for( i=0; i < 3; i++ )
	{
		switch( prmSet[i] )
		{
		case KPI_EXT1_LOWLEVEL:		/* <S1D6> SI4, LowLevel			*/
					LtStsAdr[i].STimShift = 13;
					LtStsAdr[i].STimLtAdr = &AsicHwReg->AREG_STIMLT5;
					LtStsAdr[i].SFbLtAdr = &AsicHwReg->AREG_SFBLT1;
					ExtLtEnable[i] = TRUE;
					prmChek[i]=HW_SI4;
					break;
		case KPI_EXT2_LOWLEVEL:		/* <S1D6> SI5, LowLevel			*/
					LtStsAdr[i].STimShift = 14;
					LtStsAdr[i].STimLtAdr = &AsicHwReg->AREG_STIMLT6;
					LtStsAdr[i].SFbLtAdr = &AsicHwReg->AREG_SFBLT3;
					ExtLtEnable[i] = TRUE;
					prmChek[i]=HW_SI5;
					break;
		case KPI_EXT3_LOWLEVEL:		/* <S1D6> SI6, LowLevel			*/
					LtStsAdr[i].STimShift = 15;
					LtStsAdr[i].STimLtAdr = &AsicHwReg->AREG_STIMLT7;
					LtStsAdr[i].SFbLtAdr = &AsicHwReg->AREG_SFBLT5;
					ExtLtEnable[i] = TRUE;
					prmChek[i]=HW_SI6;
					break;
		case KPI_EXT1_HIGHLEVEL:	/* <S1D6> SI4, HighLevel		*/
					LtStsAdr[i].STimShift = 9;
					LtStsAdr[i].STimLtAdr = &AsicHwReg->AREG_STIMLT1;
					LtStsAdr[i].SFbLtAdr = &AsicHwReg->AREG_SFBLT0;
					ExtLtEnable[i] = TRUE;
					prmChek[i]=HW_SI4;
					break;
		case KPI_EXT2_HIGHLEVEL:	/* <S1D6> SI5, HighLevel		*/
					LtStsAdr[i].STimShift = 10;
					LtStsAdr[i].STimLtAdr = &AsicHwReg->AREG_STIMLT2;
					LtStsAdr[i].SFbLtAdr = &AsicHwReg->AREG_SFBLT2;
					ExtLtEnable[i] = TRUE;
					prmChek[i]=HW_SI5;
					break;
		case KPI_EXT3_HIGHLEVEL:	/* <S1D6> SI6, HighLevel		*/
					LtStsAdr[i].STimShift = 11;
					LtStsAdr[i].STimLtAdr = &AsicHwReg->AREG_STIMLT3;
					LtStsAdr[i].SFbLtAdr = &AsicHwReg->AREG_SFBLT4;
					ExtLtEnable[i] = TRUE;
					prmChek[i]=HW_SI6;
					break;
		default:	/* Not Select				*/
					LtStsAdr[i].STimShift = 0;
					LtStsAdr[i].STimLtAdr = 0;
					LtStsAdr[i].SFbLtAdr = 0;
					ExtLtEnable[i] = FALSE;
					prmChek[i]=HW_SI_NO_DEFINE;
					break;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		���b�`�I�𐮍����`�F�b�N																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( prmChek[0] == prmChek[1] )
	{
		if( prmChek[0] != 0 ){	rc = FALSE;}
	}
	else if( prmChek[0] == prmChek[2] )
	{
		if( prmChek[0] != 0 ){	rc = FALSE;}
	}
	else if( prmChek[1] == prmChek[2] )
	{
//		if( prmChek[0] != 0 ){	rc = FALSE;}
		if( prmChek[1] != 0 ){	rc = FALSE;}	/* <S226> */
	}
/*--------------------------------------------------------------------------------------------------*/
	return rc;

}

/****************************************************************************************************/
/*																									*/
/*		�}�C�N���v���O�����̃��[�h����																*/
/*																									*/
/****************************************************************************************************/
BOOL	SHal_LoadAsicMicroProgram( ASICS *SvAsicRegs, CULONG *src_adr, LONG u_size)
{
	LONG				i;
#ifndef _VC_DEBUG_ /* 2012.08.24 Y.Oka Make Mercury Base Software */
	VULONG				dummy;
#endif /* _VC_DEBUG_ */ /* 2012.08.24 Y.Oka Make Mercury Base Software */
	CULONG				*PrgSrc;							/* MicroProgram Src  Address			*/
	LONG				PrgSize;							/* MicroProgram Word Size				*/
	ASIC_HW_IF_STR		*pAsicHwReg;

	pAsicHwReg = SvAsicRegs->AsicHwReg;
/*--------------------------------------------------------------------------------------------------*/
/*		�ʃv���O�����̑I��																			*/
/*--------------------------------------------------------------------------------------------------*/
	PrgSrc  = src_adr;										/* Set Src  Address						*/
	PrgSize = u_size;										/* Set Word Size						*/
	if( (PrgSize % 2) != 0 )								/* �T�C�Y�`�F�b�N						*/
	{
		return TRUE;	/* A.BF2 : �V�X�e���A���[���Q(Micro�ُ�) */
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�ʃv���O�����̏����݋���																	*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SETREG1 = 0x00000005;					/* ASIC Write Enable					*/
	pAsicHwReg->MREG_CTSTR = MPCMD_PRGSTOP;					/* �ʃv���O�������s��~					*/
	SvAsicRegs->MicroRunningSts = FALSE;					/* �ʃv���O�������s��~(CPU�Ǘ�)		*/
/*--------------------------------------------------------------------------------------------------*/
/*		�ʃv���O�����̓]������																		*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->MREG_UPLDA	= 0;
	for( i=0; i<PrgSize; i++ )
	{

#ifndef _VC_DEBUG_ /* 2012.08.24 Y.Oka Make Mercury Base Software */

		/* 2010.05.21 Y.Oka Wait�p */
		dummy = pAsicHwReg->AREG_TSTT1CNT;

		/* ���[�h�|�C���^�`�F�b�N */
		if( ( pAsicHwReg->MREG_UPLDA ) != ((i%2)<<14)+(i/2) )
		{
			return TRUE;	/* A.BF2 : �V�X�e���A���[���Q(Micro�ُ�) */
		}

		pAsicHwReg->MREG_UPLD	= PrgSrc[i];		/* �ʃv���O������������					*/

#else /* _VC_DEBUG_ */ /* 2012.08.24 Y.Oka Make Mercury Base Software */

		/* �ʃv���O������������ */
		*(ULONG*)(&SvAsicRegs->pDummyMicroSram[i*2]) = *(ULONG*)(&PrgSrc[i]);

#endif /* _VC_DEBUG_ */ /* 2012.08.24 Y.Oka Make Mercury Base Software */

	}
/*--------------------------------------------------------------------------------------------------*/
/*		�ʃv���O�����̃x���t�@�C����																*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->MREG_UPLDA	= 0;
	for( i=0; i<PrgSize; i++ )
	{

#ifndef _VC_DEBUG_ /* 2012.08.24 Y.Oka Make Mercury Base Software */

		/* 2010.05.21 Y.Oka Wait�p */
		dummy = pAsicHwReg->AREG_TSTT1CNT;

		/* ���[�h�|�C���^�`�F�b�N */
		if( ( pAsicHwReg->MREG_UPLDA ) != ((i%2)<<14)+(i/2) )
		{
			return TRUE;	/* A.BF2 : �V�X�e���A���[���Q(Micro�ُ�) */
		}

		/* �ʃv���O�����ǂݏo�����f�[�^�`�F�b�N */
		if( ( pAsicHwReg->MREG_UPLD ) != PrgSrc[i] )
		{
			return TRUE;	/* A.BF2 : �V�X�e���A���[���Q(Micro�ُ�) */
		}

#else /* _VC_DEBUG_ */ /* 2012.08.24 Y.Oka Make Mercury Base Software */

		/* �ʃv���O�����ǂݏo�����f�[�^�`�F�b�N */
		if( SvAsicRegs->pDummyMicroSram[i] != PrgSrc[i] )
		{
			return TRUE;	/* A.BF2 : �V�X�e���A���[���Q(Micro�ُ�) */
		}

#endif /* _VC_DEBUG_ */ /* 2012.08.24 Y.Oka Make Mercury Base Software */
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�`�r�h�b�����݋֎~																			*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SETREG1	= 0x00000001;				/* ASIC Write Disable					*/

	return FALSE;
}



/****************************************************************************************************/
/*																									*/
/*		�}�C�N���v���O�������W�X�^�̃��Z�b�g														*/
/*																									*/
/****************************************************************************************************/
BOOL	SHal_ResetAsicMicroRegister( ASICS *SvAsicRegs )
{
	LONG				i;									/* Loop Index							*/
	VULONG				vulwk;								/* ULONG Work							*/
	VULONG				Dummy;								/* Dummy for Read						*/
	VULONG				*WkReg;								/* MicroProgram Work Register Address	*/
	ASIC_HW_IF_STR		*pAsicHwReg;

	WkReg = (VULONG*)(SvAsicRegs->AsicMicroReg);			/* MicroProgram Work Register Address	*/
	pAsicHwReg = SvAsicRegs->AsicHwReg;

/*--------------------------------------------------------------------------------------------------*/
/*		�}�C�N���v���O�������s��~(�O�̂���)														*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SETREG1	= 0x00000005;				/* ASIC Write Enable					*/
	pAsicHwReg->MREG_CTSTR		= MPCMD_PRGSTOP;			/* �ʃv���O�������s��~					*/
	pAsicHwReg->AREG_SETREG1	= 0x00000001;				/* ASIC Write Disable					*/
	SvAsicRegs->MicroRunningSts = FALSE;					/* �ʃv���O�������s��~(CPU�Ǘ�)		*/

/*--------------------------------------------------------------------------------------------------*/
/*		���[�N���W�X�^�̃`�F�b�N���N���A															*/
/*--------------------------------------------------------------------------------------------------*/
	for( i=0; i<(ASIC_MREGSIZE/4); i++ )					/* Write to WkReg[i]					*/
	{
		WkReg[i]	= (ULONG)(i);							/* Write 0,1,2,3,4,5,...				*/
	}
	/*----------------------------------------------------------------------------------------------*/
	for( i=0; i<(ASIC_MREGSIZE/4); i++ )					/* Check WkReg[i]						*/
	{
		vulwk = WkReg[i];									/* Read WkReg[i]						*/
		if( vulwk != (i) )									/* Check Read Data						*/
		{
			return TRUE;	/* A.BF2 : �V�X�e���A���[���Q(Micro�ُ�) */
		}
	}
/*--------------------------------------------------------------------------------------------------*/
	for( i=0; i<(ASIC_MREGSIZE/4); i++ )					/* Check and Clear WkReg[i]				*/
	{
		WkReg[i]	= 0x5A5A5A5A;							/* Write 0x5A5A5A5A to WkReg[i]			*/
		Dummy = pAsicHwReg->AREG_PWMU;						/* Dummy Read							*/
		vulwk = WkReg[i];									/* Read and Check						*/
		if( vulwk != 0x5A5A5A5A )							/* Check Read Data						*/
		{
			return TRUE;	/* A.BF2 : �V�X�e���A���[���Q(Micro�ُ�) */
		}
	/*----------------------------------------------------------------------------------------------*/
		WkReg[i]	= 0xA5A5A5A5;							/* Write 0xA5A5A5A5 to WkReg[i]			*/
		Dummy = pAsicHwReg->AREG_PWMU;						/* Dummy Read							*/
		vulwk = WkReg[i];									/* Read and Check						*/
		if( vulwk != 0xA5A5A5A5 )							/* Check Read Data						*/
		{
			return TRUE;	/* A.BF2 : �V�X�e���A���[���Q(Micro�ُ�) */
		}
	/*----------------------------------------------------------------------------------------------*/
		WkReg[i]	= 0x00000000;							/* Clear WkReg[i]						*/
		Dummy = pAsicHwReg->AREG_PWMU;						/* Dummy Read							*/
		vulwk = WkReg[i];									/* Read and Check						*/
		if( vulwk != 0x00000000 )							/* Check Read Data						*/
		{
			return TRUE;	/* A.BF2 : �V�X�e���A���[���Q(Micro�ُ�) */
		}
	}
	return FALSE;
}

/****************************************************************************************************/
/*																									*/
/*		�}�C�N���v���O�����e�[�u���f�[�^�̃��[�h����					<NeCO>_130919				*/
/*																									*/
/****************************************************************************************************/
BOOL	SHal_LoadAsicMicroTable( USHORT *asic_adr, USHORT *src_adr, ULONG array_size)
{
	LONG				i;									/* Loop Index							*/
	
	for( i=0; i<array_size; i++ )							/* Write to AsicReg[i]					*/
	{
		asic_adr[i] = src_adr[i];
	}
	return FALSE;
}

// <S146>
#define JL086_MCAREA_S_TOP		(0xFFE70000)
#define	JL086UprgSz_SGD7_S		*(CULONG*)(JL086_MCAREA_S_TOP)
#define	YecAsicPrg_SGD7_S_Addr	(JL086_MCAREA_S_TOP+4)
#if (CSW_BOARD_TYPE == SGD7S_TYPE)
#define	JL086UprgSz_SGD7		JL086UprgSz_SGD7_S
#define	YecAsicPrg_SGD7_Addr	YecAsicPrg_SGD7_S_Addr
#elif	(CSW_BOARD_TYPE == SGD7W_TYPE)
#define JL086_MCAREA_W_TOP		(0xFFE75000)
#define	JL086UprgSz_SGD7		*(CULONG*)(JL086_MCAREA_W_TOP)
#define	YecAsicPrg_SGD7_Addr	(JL086_MCAREA_W_TOP+4)
#endif
#define	YecAsicPrg_SGD7			(CULONG*)(YecAsicPrg_SGD7_Addr)
#define	OVMODTBLG				(CUSHORT*)((CULONG*)YecAsicPrg_SGD7_S_Addr + JL086UprgSz_SGD7_S)
/****************************************************************************************************/
/*																									*/
/*		�`�r�h�b�}�C�N���v���O�����̏�����															*/
/*																									*/
/****************************************************************************************************/
BOOL	SHal_InitMicroProgram( ASICS *SvAsicRegs )
{
	LONG rst_err, load_err, table_err;

	/* �}�C�N���v���O�����̃��[�h���� */
	load_err = SHal_LoadAsicMicroProgram( SvAsicRegs, YecAsicPrg_SGD7, (JL086UprgSz_SGD7) );

	/* �}�C�N���v���O�������W�X�^�̃N���A */
	rst_err = SHal_ResetAsicMicroRegister( SvAsicRegs );

	/* �}�C�N���v���O�����e�[�u���f�[�^�̃��[�h���� */ /* <NeCO>_130919 */
	table_err = SHal_LoadAsicMicroTable( (USHORT *)TABLESTART, (USHORT *)OVMODTBLG, TABLESIZE );

	
	return (load_err | rst_err | table_err);
}


/****************************************************************************************************/
/*																									*/
/*		ASIC �}�C�N���v���O�����X�^�[�g����															*/
/*																									*/
/****************************************************************************************************/
void	SHal_StartMicroProgram( ASICS *SvAsicRegs )
{

/*--------------------------------------------------------------------------------------------------*/
/*		Start ASIC Current-A/D, Reference-A/D, Micro-Program										*/
/*--------------------------------------------------------------------------------------------------*/
	/* Current-A/D Cycle Synchronize */
	SvAsicRegs->AsicHwReg->AREG_ADFSYNC = 0x0000;
	/* Reference-A/D Cycle Synchronize */
	SvAsicRegs->AsicHwReg->AREG_ADSYNC = 0x0000;
/*----------------------------------------------------------------------------------------------*/
	/* Clear Alarm */
	SvAsicRegs->AsicHwReg->AREG_ALMCLR = 0x802F;
	/* ASIC Write Enable */
	SvAsicRegs->AsicHwReg->AREG_SETREG1 = 0x0005L;
	/* Start Micro-Program */
	// ��(MregCtstr = MPCMD_PRGSTART) MregCtstr�X�V��Y�ꂸ��!
	SvAsicRegs->AsicHwReg->MREG_CTSTR = MPCMD_PRGSTART;
	SvAsicRegs->MicroRunningSts = TRUE;
	/* Set ASIC Start Register */
	SvAsicRegs->AsicHwReg->AREG_SETREG1 = 0x0001;
}



/****************************************************************************************************/
/*																									*/
/*		ASIC �}�C�N���v���O�����X�g�b�v����															*/
/*																									*/
/****************************************************************************************************/
void	SHal_StopMicroProgram( ASICS *SvAsicRegs )
{
/*--------------------------------------------------------------------------------------------------*/
/*		Stop ASIC Micro-Program																		*/
/*--------------------------------------------------------------------------------------------------*/
	/* ASIC Write Enable */
	SvAsicRegs->AsicHwReg->AREG_SETREG1 = 0x0005L;
	/* Start Micro-Program */
	// ��(MregCtstr = MPCMD_PRGSTART) MregCtstr�X�V��Y�ꂸ��!
	SvAsicRegs->AsicHwReg->MREG_CTSTR = MPCMD_PRGSTOP;
	SvAsicRegs->MicroRunningSts = FALSE;
}



#ifdef CSW_MICRO_DEBUG_MODE
/****************************************************************************************************/
/*																									*/
/*	�V�X�e���N���b�N�i�t�B�[�h�o�b�N�^�C�}�j�����擾�֐� �i 1Clock : 37.5[ns] �j											*/
/*																									*/
/****************************************************************************************************/
void SHal_SetMicroDebugMode( ASIC_HW_IF_STR *pAsicHwReg )
{
	/* Output INT2 Interrupt Signal from TP5 */
	pAsicHwReg->MREG_DBOS = 0x00000900;
}
#endif



/****************************************************************************************************/
/*																									*/
/*	�V�X�e���N���b�N��[ns]�ϊ� �ʃv���O�����������ԑ���p											*/
/*																									*/
/****************************************************************************************************/
ULONG SHal_ClockToTime( USHORT clock_diff )
{
	float time_in_ns;
	time_in_ns = (float)clock_diff * (float)(37.5);
	return (ULONG)time_in_ns; /* [ns] */
}


/****************************************************************************************************/
/*																									*/
/*	�V�X�e���N���b�N�i�t�B�[�h�o�b�N�^�C�}�j�擾�֐� 					�j											*/
/*																									*/
/****************************************************************************************************/
USHORT SHal_GetSysClock( ASICS *SvAsicRegs )
{
	/* ASIC�̃t�B�[�h�o�b�N�J�E���^(16bit : 1Clock : 37.5[ns])��Ԃ� */
	return SvAsicRegs->AsicHwReg->AREG_TCNT;
}

/****************************************************************************************************/
/*																									*/
/*	�V�X�e���N���b�N�i�t�B�[�h�o�b�N�^�C�}�j�����擾�֐� �i 1Clock : 37.5[ns] �j											*/
/*																									*/
/****************************************************************************************************/
USHORT SHal_Get_dClock( USHORT last_clk , USHORT clk )
{
	USHORT d_clk;

	if( clk > last_clk )
	{
		d_clk = clk - last_clk;
	}
	else
	{/* �I�[�o�[�t���[ �΍�*/
		d_clk = ( 0xFFFF - last_clk ) + clk;
	}
	/* ����N���b�N(clk)�ƑO��N���b�N(last_clk)�̍�����Ԃ� */
	return d_clk;
}



#endif /* _JL076_USE_ */
/***************************************** end of file **********************************************/
