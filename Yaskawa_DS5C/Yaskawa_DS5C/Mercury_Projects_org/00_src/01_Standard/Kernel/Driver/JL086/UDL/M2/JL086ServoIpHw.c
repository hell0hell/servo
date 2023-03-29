/****************************************************************************************************/
/*																									*/
/*																									*/
/*		InitJL076_sub.c : JL-076�������֘A�T�u�֐����W���[��										*/
/*			���t�@�C������JL086�����A���g��JL076��													*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : JL-076�������֘A�T�u�֐����`�B														*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	����	2009.06.04	Y.Oka															*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"
#include "HwDeviceIf.h"
#include "JL086ServoIpHw.h"
#include "KLib.h"


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
	TempICI2Cdrv_Initialize( AsicHwReg);			/* ���x�Z���T������								*//* <TempSensorIC> */

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
		pAsicHwReg->AREG_SEPGSET00 &= (0xffff & ~BIT1);
		pAsicHwReg->AREG_SEPGSET10 &= (0xffff & ~BIT1);
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
	/*----------------------------------------------------------------------------------------------*/
	/*		�N���b�N�@�\�̏�����(JL-076�Ń\�t�g���Z�b�g�O�ɃN���b�N�L�����K�v)						*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SETREG1	= 0x0005;		/* ASIC Write Enable								*/
	pAsicHwReg->AREG_CSTP		= 0x0000;		/* ASIC Clock Stop Selection						*/
	pAsicHwReg->AREG_SETREG1	= 0x0001;		/* ASIC Write Disable								*/

	/*----------------------------------------------------------------------------------------------*/
	/*		�\�t�g���Z�b�g																			*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SETREG1	= 0x0009;		/* ASIC S/W Reset Enable							*/
	KlibWaitus( 1 );							/* Wait												*/
	pAsicHwReg->AREG_SRESET		= 0x0000;		/* ASIC Soft Reset									*/
	KlibWaitus( 1 );							/* Wait												*/
	pAsicHwReg->AREG_SETREG1	= 0x0001;		/* ASIC S/W Reset Disable							*/

	/*----------------------------------------------------------------------------------------------*/
	/*		�N���b�N�@�\�̏�����																	*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SETREG1	= 0x0005;		/* ASIC Write Enable								*/
	pAsicHwReg->AREG_CSTP		= 0x0000;		/* ASIC Clock Stop Selection						*/
												/*	Encoder ADIF, FB Pulse Cntr,					*/
												/*	New Incre Decoder,Dedband Comp. Clock Stop		*/
	pAsicHwReg->AREG_SETREG1	= 0x0001;		/* ASIC Write Disable								*/

	/*----------------------------------------------------------------------------------------------*/
	/*		���Z��(MicroProgram)�E�H�b�`�h�b�O�@�\���p���X�ϊ��b���}�X�N�r�b�g�̏�����				*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_WDT1SET	= 0x0150;		/* Set C-Phase Mask(bit8), WDT Stop(bit6),			*/
												/*		and Micro WDT Time Over(0x10*16us=256us)	*/
	/*----------------------------------------------------------------------------------------------*/
	/*		�z�X�g(CPU)�E�H�b�`�h�b�O�@�\�̏�����													*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_WDT2SET	= 0x0020;		/* Set CPU WDT Time Over(64us*0x20=2048us)			*/

	/*----------------------------------------------------------------------------------------------*/
	/*		�A�i���O���j�^�@�\�̏�����																*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_PWMU		= 0x0000;		/* Set Zero for Initial Out							*/
	pAsicHwReg->AREG_PWMV		= 0x0000;		/* Set Zero for Initial Out							*/

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
	pAsicHwReg->AREG_ALMSET		= 0x0000;		/* HWGBON:Invalid, DBONL:DBON						*/

	/*----------------------------------------------------------------------------------------------*/
	/*		�ɐ����t�B���^�ݒ� : OV, OC, VCAP, BBON1, BBON2											*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SETREG1	= 0x0005;		/* ASIC Write Enable								*/
	pAsicHwReg->AREG_SETREG2	= 0x30F0;		/* OV, OC : ActiveLow, Filter valid					*/
	pAsicHwReg->AREG_SETREG1	= 0x0001;		/* ASIC Write Disable								*/

	/*----------------------------------------------------------------------------------------------*/
	/*		�A���[���N���A																			*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_ALMCLR		= 0xFFFF;		/* Clear All Alarm									*/

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
	pAsicHwReg->AREG_INT1SET	= 0x0000;		/* Disable All Interrupt							*/

	/*----------------------------------------------------------------------------------------------*/
	/*		�����p�^�C�}���W�X�^�ݒ�																*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_INT0SET	= 0x0000;		/* INT0 Frequency									*/

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
	pAsicHwReg->AREG_PFNCSET	&= 0xFF80;		/* �[�q�@�\�ݒ� : SESEL0:S-PG0, SESEL1:S-PG1		*/

	/*----------------------------------------------------------------------------------------------*/
	/*		�V���A���G���R�[�_�h�e���W�X�^������													*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SEPGSET00	= 0x1006;		/* CH0,1���� CrossTalk Avoidance ON,				*/
	pAsicHwReg->AREG_SEPGSET10	= 0x1006;		/* CH0,1���� ManchesterCode Squelch ON,				*/
												/* CH0,1���� NetworkSync OFF						*/
	pAsicHwReg->AREG_SEPGSET01	= 0x0001;		/* CH0,1���� Slave Address : 01h					*/
	pAsicHwReg->AREG_SEPGSET11	= 0x0001;		/* CH0,1���� 16Byte P0 Master Mode					*/

	/*----------------------------------------------------------------------------------------------*/
	/*		�V���A���G���R�[�_���M�f�[�^���W�X�^������												*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SE0_TXD0	= 0x0000;		/* CH0 Transmit Register							*/
	pAsicHwReg->AREG_SE0_TXD1	= 0x0000;		/* CH0 Transmit Register							*/
	pAsicHwReg->AREG_SE0_TXD2	= 0x0000;		/* CH0 Transmit Register							*/
	pAsicHwReg->AREG_SE0_TXD3	= 0x0000;		/* CH0 Transmit Register							*/
	pAsicHwReg->AREG_SE0_TXD4	= 0x0000;		/* CH0 Transmit Register							*/
	pAsicHwReg->AREG_SE0_TXD5	= 0x0000;		/* CH0 Transmit Register							*/
	pAsicHwReg->AREG_SE0_TXD6	= 0x0000;		/* CH0 Transmit Register							*/
	pAsicHwReg->AREG_SE0_TXD7	= 0x0000;		/* CH0 Transmit Register							*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SE1_TXD0	= 0x0000;		/* CH1 Transmit Register							*/
	pAsicHwReg->AREG_SE1_TXD1	= 0x0000;		/* CH1 Transmit Register							*/
	pAsicHwReg->AREG_SE1_TXD2	= 0x0000;		/* CH1 Transmit Register							*/
	pAsicHwReg->AREG_SE1_TXD3	= 0x0000;		/* CH1 Transmit Register							*/
	pAsicHwReg->AREG_SE1_TXD4	= 0x0000;		/* CH1 Transmit Register							*/
	pAsicHwReg->AREG_SE1_TXD5	= 0x0000;		/* CH1 Transmit Register							*/
	pAsicHwReg->AREG_SE1_TXD6	= 0x0000;		/* CH1 Transmit Register							*/
	pAsicHwReg->AREG_SE1_TXD7	= 0x0000;		/* CH1 Transmit Register							*/

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
	pAsicHwReg->AREG_IOSTAT2	= KPX_AREG_IOSTAT2_INIOUT;	/* Set Initial Output Data to High Word	*/
	pAsicHwReg->AREG_IOSTAT1	= KPX_AREG_IOSTAT1_INIOUT;	/* Set Initial Output Data to Low  Word	*/

	/*----------------------------------------------------------------------------------------------*/
	/*		�񓯊��V���A���h�e�̏�����																*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_UARTSTS	= 0x0000;		/* UART Status Register								*/

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
//	uswk = ( uswk | 0x0100 );							/* CLM���b�`�C�l�[�u���ݒ�					*/
		;												/* @@ CLM���b�`�f�B�Z�[�u��					*/
	pAsicHwReg->AREG_POCSET = uswk;						/* �w�߃J�E���^�ݒ�							*/
	MlibWaitus(1);										/* @@ �O�̂���								*/
	pAsicHwReg->AREG_POCSET = ( uswk | 0x0100 );		/* @@ CLM���b�`�C�l�[�u���ݒ�				*/
	
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
/*		�`�c�C���^�[�t�F�C�X������																	*/
/*																									*/
/****************************************************************************************************/
void	SHal_InitAsicAdInterface( ASIC_HW_IF_STR *pAsicHwReg )
{
/*--------------------------------------------------------------------------------------------------*/
/*		���ʂ`�c�ݒ�(�w�߂`�c���d�����o�`�c)														*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_ADSET1		= 0x1043;		/* Command AD 16Bit AD I/F							*/
												/* Current AD 10Bit Decimation Filter				*/
												/* Current AD Sync Invalid							*/

/*--------------------------------------------------------------------------------------------------*/
/*		�w�߂`�c�C���^�[�t�F�C�X������			Comment : MSB --> LSB								*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_ADSET2		= 0xC0CE;		/* AD Clock, AD Frequency, Base Frequency			*/
	pAsicHwReg->AREG_ADSET3		= 0x8280;		/* AD Start Timing, AD I/F Cycle					*/
	pAsicHwReg->AREG_ADSET4		= 0xDADA;		/* AD Latch Timing, AD Latch Timing					*/
	pAsicHwReg->AREG_ADSET5		= 0x80DB;		/* AD Control H to L Edge, L to H Edge				*/
	pAsicHwReg->AREG_ADSET6		= 0x80DB;		/* AD Control H to L Edge, L to H Edge				*/

/*--------------------------------------------------------------------------------------------------*/
/*		�d�����o�`�c�C���^�[�t�F�C�X������			   Comment : MSB --> LSB						*/
/*--------------------------------------------------------------------------------------------------*/
	/* Mode3 */									/* �d��������� = 15.625us(64kHz)					*/
	pAsicHwReg->AREG_ADFCNT1	= 0x00FB;		/* �d��ADIF��{�N���b�N���g�� = 3.2MHz				*/
	pAsicHwReg->AREG_ADFCNT2	= 0x0038;		/* ADIF Cycle, (256 - 38h) / 3.2MHz = 62.5us		*/
	pAsicHwReg->AREG_ADFCNT3	= 0x0000;		/* AD Latch Timing, AD Latch Timing					*/
	pAsicHwReg->AREG_ADFCNT4	= 0x3839;		/* AD Control H to L Edge, L to H Edge				*/
	pAsicHwReg->AREG_ADFCNT5	= 0x0000;		/* AD Control H to L Edge, L to H Edge				*/

/*--------------------------------------------------------------------------------------------------*/
/*		�d�����o�`�c�f�V���[�V�����t�B���^������													*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SDMCONF1	= 0x181B;		/* CH1,2,3 Mode3, �d�����o����\ = 11.9bit			*/
	pAsicHwReg->AREG_SDMCONF2	= 0x2A47;		/* �v���g���K�v�Z:1/8��, �����:4					*/
												/* Overrange Detect:Mask, Mclock:10MHz Clock Out	*/
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
		pAsicHwReg->AREG_TIM1SET = 0xEB9C;		/* TB=(256 - 156)[us], TW=(255 - 235)[us]			*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		DB�p���X�ݒ�FDB���� = �����[																*/
/*--------------------------------------------------------------------------------------------------*/
	else										/* PnE0C.5:4 = x:0 : �����[							*/
	{											/* LowByte:Frequency,  HighByte:HighPulseWidth		*/
		pAsicHwReg->AREG_TIM1SET = 0x009C;		/* TB=(256 - 156)[us], TW=(255 - 0)[us]				*/
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
	pAsicHwReg->AREG_SETREG3	= 0x0000;		/* REGOFF=0, DISCH=0, GFTON=0						*/

	return;
}



/****************************************************************************************************/
/*																									*/
/*		PWM����@�\�̏�����																			*/
/*																									*/
/****************************************************************************************************/
void	SHal_InitAsicPwmControl( ASIC_HW_IF_STR *pAsicHwReg, LONG odt_c )
{
	USHORT	OnDelay;

/*--------------------------------------------------------------------------------------------------*/
/*		�I���f�B���C���Ԑݒ�																		*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SETREG1	= 0x0005;		/* ASIC Write Enable								*/
	pAsicHwReg->AREG_SETREG2	&= (~BIT12);	/* Clock Select : 80MHz								*/
	pAsicHwReg->AREG_SETREG2	|= (BIT11);		/* Clock Select : 80MHz								*/
/* for debug (INGRAM�ł͕K�v�ł�����) */
//	pAsicHwReg->AREG_SETREG2		|= BIT9;	/* Carrer Counter Sync								*/
/* for debug (INGRAM�ł͕K�v�ł�����) */
	/*----------------------------------------------------------------------------------------------*/
	OnDelay = (USHORT)((ASIC_CLKMHZ * odt_c) / 10);	/* 80[MHz] * Td[0.1us] / 10						*/
	pAsicHwReg->AREG_DYSETUP1	= OnDelay;		/* UP1 : U-Phase,Positive-Side						*/
	pAsicHwReg->AREG_DYSETUN1	= OnDelay;		/* UN1 : U-Phase,Negative-Side						*/
	pAsicHwReg->AREG_DYSETVP1	= OnDelay;		/* VP1 : V-Phase,Positive-Side						*/
	pAsicHwReg->AREG_DYSETVN1	= OnDelay;		/* VN1 : V-Phase,Negative-Side						*/
	pAsicHwReg->AREG_DYSETWP1	= OnDelay;		/* WP1 : W-Phase,Positive-Side						*/
	pAsicHwReg->AREG_DYSETWN1	= OnDelay;		/* WN1 : W-Phase,Negative-Side						*/
	pAsicHwReg->AREG_SETREG1	= 0x0001;		/* ASIC Write Disable								*/

	return;
}



/****************************************************************************************************/
/*																									*/
/*		�}�C�N���v���O�����̃��[�h����																*/
/*																									*/
/****************************************************************************************************/
BOOL	SHal_LoadAsicMicroProgram( ASICS *SvAsicRegs, CUSHORT *src_adr, LONG u_size)
{
	LONG				i;
#ifndef _VC_DEBUG_ /* 2012.08.24 Y.Oka Make Mercury Base Software */
	VULONG				dummy;
#endif /* _VC_DEBUG_ */ /* 2012.08.24 Y.Oka Make Mercury Base Software */
	CUSHORT				*PrgSrc;							/* MicroProgram Src  Address			*/
	LONG				PrgSize;							/* MicroProgram Word Size				*/
	ASIC_HW_IF_STR		*pAsicHwReg;

	pAsicHwReg = SvAsicRegs->AsicHwReg;
/*--------------------------------------------------------------------------------------------------*/
/*		�ʃv���O�����̑I��																			*/
/*--------------------------------------------------------------------------------------------------*/
	PrgSrc  = src_adr;										/* Set Src  Address						*/
	PrgSize = u_size;										/* Set Word Size						*/
	if( (PrgSize % 3) != 0 )								/* �T�C�Y�`�F�b�N						*/
	{
		return TRUE;	/* A.BF2 : �V�X�e���A���[���Q(Micro�ُ�) */
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�ʃv���O�����̏����݋���																	*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SETREG1 = 0x0005 ;						/* ASIC Write Enable					*/
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
//		dummy = pAsicHwReg->AREG_TSTT1CNT;

		/* ���[�h�|�C���^�`�F�b�N */
		if( ( pAsicHwReg->MREG_UPLDA ) != ((i%3)<<14)+(i/3) )
		{
			return TRUE;	/* A.BF2 : �V�X�e���A���[���Q(Micro�ُ�) */
		}

		pAsicHwReg->MREG_UPLD	= PrgSrc[i];				/* �ʃv���O������������					*/

#else /* _VC_DEBUG_ */ /* 2012.08.24 Y.Oka Make Mercury Base Software */

		/* �ʃv���O������������ */
		SvAsicRegs->pDummyMicroSram[i] = PrgSrc[i];

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
		if( ( pAsicHwReg->MREG_UPLDA ) != ((i%3)<<14)+(i/3) )
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
	pAsicHwReg->AREG_SETREG1	= 0x0001;							/* ASIC Write Disable					*/

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
	VUSHORT				vuswk;								/* USHORT Work							*/
	VULONG				Dummy;								/* Dummy for Read						*/
	VUSHORT				*WkReg;								/* MicroProgram Work Register Address	*/
	ASIC_HW_IF_STR		*pAsicHwReg;

	WkReg = (VUSHORT*)(SvAsicRegs->AsicMicroReg);			/* MicroProgram Work Register Address	*/
	pAsicHwReg = SvAsicRegs->AsicHwReg;

/*--------------------------------------------------------------------------------------------------*/
/*		�}�C�N���v���O�������s��~(�O�̂���)														*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SETREG1	= 0x0005;					/* ASIC Write Enable					*/
	pAsicHwReg->MREG_CTSTR		= MPCMD_PRGSTOP;			/* �ʃv���O�������s��~					*/
	pAsicHwReg->AREG_SETREG1	= 0x0001;					/* ASIC Write Disable					*/
	SvAsicRegs->MicroRunningSts = FALSE;					/* �ʃv���O�������s��~(CPU�Ǘ�)		*/

/*--------------------------------------------------------------------------------------------------*/
/*		���[�N���W�X�^�̃`�F�b�N���N���A															*/
/*--------------------------------------------------------------------------------------------------*/
	for( i=0; i<(ASIC_MREGSIZE/2); i++ )					/* Write to WkReg[i]					*/
	{
		WkReg[i]	= (USHORT)(i);							/* Write 0,1,2,3,4,5,...				*/
	}
	/*----------------------------------------------------------------------------------------------*/
	for( i=0; i<(ASIC_MREGSIZE/2); i++ )					/* Check WkReg[i]						*/
	{
		vuswk = WkReg[i];									/* Read WkReg[i]						*/
		if( vuswk != (i) )									/* Check Read Data						*/
		{
			return TRUE;	/* A.BF2 : �V�X�e���A���[���Q(Micro�ُ�) */
		}
	}
/*--------------------------------------------------------------------------------------------------*/
	for( i=0; i<(ASIC_MREGSIZE/2); i++ )					/* Check and Clear WkReg[i]				*/
	{
		WkReg[i]	= 0x5A5A;								/* Write 0x5A5A to WkReg[i]				*/
		Dummy = pAsicHwReg->AREG_PWMU;						/* Dummy Read							*/
		vuswk = WkReg[i];									/* Read and Check						*/
		if( vuswk != 0x5A5A )								/* Check Read Data						*/
		{
			return TRUE;	/* A.BF2 : �V�X�e���A���[���Q(Micro�ُ�) */
		}
	/*----------------------------------------------------------------------------------------------*/
		WkReg[i]	= 0xA5A5;								/* Write 0xA5A5 to WkReg[i]				*/
		Dummy = pAsicHwReg->AREG_PWMU;						/* Dummy Read							*/
		vuswk = WkReg[i];									/* Read and Check						*/
		if( vuswk != 0xA5A5 )								/* Check Read Data						*/
		{
			return TRUE;	/* A.BF2 : �V�X�e���A���[���Q(Micro�ُ�) */
		}
	/*----------------------------------------------------------------------------------------------*/
		WkReg[i]	= 0x0000;								/* Clear WkReg[i]						*/
		Dummy = pAsicHwReg->AREG_PWMU;						/* Dummy Read							*/
		vuswk = WkReg[i];									/* Read and Check						*/
		if( vuswk != 0x0000 )								/* Check Read Data						*/
		{
			return TRUE;	/* A.BF2 : �V�X�e���A���[���Q(Micro�ُ�) */
		}
	}

	return FALSE;
}



/****************************************************************************************************/
/*																									*/
/*		�`�r�h�b�}�C�N���v���O�����̏�����															*/
/*																									*/
/****************************************************************************************************/
BOOL	SHal_InitMicroProgram( ASICS *SvAsicRegs )
{
	LONG rst_err, load_err;

	/* �}�C�N���v���O�����̃��[�h���� */
//	load_err = SHal_LoadAsicMicroProgram( SvAsicRegs, YecAsicPrg_INGRAM, (JL076UprgSz_INGRAM) );
	load_err = SHal_LoadAsicMicroProgram( SvAsicRegs, YecAsicPrg_SGDV, (JL076UprgSz_SGDV) );

	/* �}�C�N���v���O�������W�X�^�̃N���A */
	rst_err = SHal_ResetAsicMicroRegister( SvAsicRegs );

	return (load_err | rst_err);
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
	SvAsicRegs->AsicHwReg->AREG_ADSYNC = 0x0000;	// �� �w��AD�͎g�p���Ȃ����ߕs�v�ł́H	nov29
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



/***************************************** end of file **********************************************/
