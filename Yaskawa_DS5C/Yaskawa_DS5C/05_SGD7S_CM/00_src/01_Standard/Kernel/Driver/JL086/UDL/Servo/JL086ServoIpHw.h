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
#ifndef	_JL086_SERVOIP_HW_H
#define	_JL086_SERVOIP_HW_H

#ifndef _JL076_USE_

#include "SystemSetting.h"
#include "JL086ServoIpReg.h"
#include "MLib.h"
#include "KLib.h"


/****************************************************************************************************/
/*		STRUCTURES																					*/
/****************************************************************************************************/
typedef struct _SVIP_INIT_PRM
{
	USHORT	prstsw;		/* Pn200: �ʒu����w�ߌ`�ԑI���X�C�b�` */
	USHORT	syssw1;		/* PnE0B: �V�X�e���X�C�b�`1 */
	USHORT	syssw2;		/* PnE0C: �V�X�e���X�C�b�`2 */
	USHORT	comfil;		/* PnE0E: �w�ߓ��̓t�B���^�I�����̎w�߃t�B���^�ݒ� */
	DBYTEX	odt_c;		/* PnE2D: ����:�I���f�B���C����, ���:�I���f�B���C�⏞�萔 */
} SVIP_INIT_PRM;

typedef struct _SVIP_INIT_PCOMP
{
	DBYTEX	bit_dp;				/* PnF03 : �G���R�[�_�r�b�g��, �ʒu�f�[�^�����_�ʒu				*/
	BOOL	RevAsmMotor;		/* ���[�^���]�ڑ�Flag										*/
	LONG	AxisMotType;		/* Motor Type Rotary or Linear							*/
	LONG	PulseNo;			/* Pulse Number	[pulse/rev] or [pulse/360deg]			*/
	LONG	AccErrLvl;			/* Motor Encoder Acc. Error Check						*/
	DBYTEX	spgfail;			/* PnE1E : ����:S-PG��M���s���e��, ���:FS-PG��M���s���e��*/
}SVIP_INIT_PCOMP;/* <S004> */

typedef enum _SVIP_INIT_ERR {
	SVIP_NOALM		= 0,
	SVIP_PRMERR		= 0x00000001,		/* A.040 : �p�����[�^�G���[�̐ݒ�	*/
} SVIP_INIT_ERR;


/****************************************************************************************************/
/*		DEFINES																						*/
/****************************************************************************************************/
/* �s�v */
//#define JL076B_ASICID_CODE	0x3400							/* JL-077 rev.B ID					*/
//#define JL076B_COMADR_OFS		0x80000							/* �S������ASIC���W�X�^�I�t�Z�b�g��	*/

#define JL086_SVIP_OFS			0x1000							/* 1������ASIC���W�X�^�I�t�Z�b�g��	*/
#define JL086_MICRO_OFS			0x7000							/* �ʃ��W�X�^�I�t�Z�b�g��			*/
//#define ASIC_MREGSIZE			0x0500							/* ASIC MP Reg Byte Size			*//* <S068> */
//#define ASIC_MREGSIZE			0x0C00							/* ASIC MP Reg Byte Size			*//* <S068> */
#define JL086_MICRO_AXIS_OFS	0x0500							/* 1�����̃ʃ��W�X�^�I�t�Z�b�g��	*/
#if CSW_BOARD_TYPE == SGD7S_TYPE /* <S0E4> */
#define ASIC_MREGSIZE			0x0500							/* ASIC MP Reg Byte Size (1axis)	*/
#else /* CSW_BOARD_TYPE == SGD7W_TYPE */
#define ASIC_MREGSIZE			0x0C00							/* ASIC MP Reg Byte Size (2axis)	*/
#endif /* CSW_BOARD_TYPE == SGD7S_TYPE */

/* �n�[�h�E�F�A�o�͐M�������l��` */
//#define ASIC_CLKMHZ				320								/* ASIC Clock [MHz]					*/
#define ASIC_CLKMHZ				80								/* ASIC Clock [MHz]					*//* @@�b�� */

/*--------------------------------------------------------------------------------------------------*/
/*		�������[�h��ԋy�ѕ����b���}�X�N��Ԑݒ�l�p��}�N��										*/
/*--------------------------------------------------------------------------------------------------*/
#define DIV_MODE_NORMAL		0x00				/* �����X���[���[�h���̐ݒ�l						*/
#define DIV_MODE_DCC		0x01				/* �������[�h1���̐ݒ�l							*/
#define DIV_MODE_DCC2		0x02				/* �������[�h2���̐ݒ�l							*/
#define DIV_CMASK_ON		0x01				/* ����C���}�X�NON���̐ݒ�l						*/
#define DIV_CMASK_OFF		0x00				/* ����C���}�X�NOFF���̐ݒ�l						*/

/* �G���R�[�_�ʐM�p��` */
#define SENC_COMRATE_4MBPS	0x0000				/* �G���R�[�_�ʐM���x4M BPS �ݒ�					*/
#define SENC_COMRATE_8MBPS	0x0001				/* �G���R�[�_�ʐM���x8M BPS �ݒ�					*/
#define ENC_BUF_SIZE_L		4					/* Encoder communication Buffer Size (LONG)			*/
#define ENC_BUF_SIZE_S		8					/* Encoder communication Buffer Size (SHORT)		*/

/*--------------------------------------------------------------------------------------------------*/
#define	COMSYNCSEL_MASK		0x000000E1			/* �ʐM�����}�X�N									*/

/****************************************************************************************************/
/*		Low level ASIC Access API's																	*/
/****************************************************************************************************/
//#define SHAL_FAULT_OH		0x0010		/* Over Current */
#define SHAL_FAULT_OV		0x0010		/* Over Voltage <S058> */
#define SHAL_FAULT_OC1		0x0020		/* Over Current *//* <S126>:rename */
#define SHAL_FAULT_WDOUT	0x0200		/* host cpu watch dog time over. */
#define SHAL_FAULT_BB		0x0400		/* Base Block */
#define SHAL_FAULT_GUP1		0x0800		/* Watch dog time over in the Micro.*/
#define SHAL_FAULTS			0xffff		/* All bit */
//#define SHAL_FAULT_OC2		0x0080		/* Over Current *//* <S126> */
#define SHAL_FAULT_OC2		0x0800		/* Over Current *//* <S133> */
#define SHAL_FAULT_OCALMULT	0x1000		/* U��OC���o�A���[�����b�`�l *//* <S1F5> */
#define SHAL_FAULT_OCALMVLT	0x2000		/* V��OC���o�A���[�����b�`�l *//* <S1F5> */
#define SHAL_FAULT_OCALMWLT	0x4000		/* W��OC���o�A���[�����b�`�l *//* <S1F5> */

/* <S004> Start */
/****************************************************************************************************/
/*	�G���R�[�_�ʒu�␳�@�\��`																		*/
/****************************************************************************************************/
typedef enum _POSCOMP_MODE {
	POSCOMP_NOP		 = 0,								/* �ʒu�␳���s��Ȃ�						*/
	POSCOMP_ASIC	 = 1,								/* �ʒu�␳��JL-086�ɂď���					*/
	POSCOMP_SW		 = 2,								/* �ʒu�␳��SW�ɂď���						*/
} POSCOMP_MODE;
/*--------------------------------------------------------------------------------------------------*/
#define VFBTINMINPLS	2								/* �ω���ϕ������s�ŏ������ʒu��ٽ [pluse]	*/
#define TIMBASECLK		6								/* �ݺ��޸ۯ����g��[MHz]: 6 MHz				*/
#define ENCLCHCLKNUM	16								/* �ݺ���ׯ�����(2.667[us])�̸ۯ���			*/
#define SAENCCLKNUM	(KPI_SACYCLENS*TIMBASECLK/1000)	/* ���Z�����̴ݺ��޸ۯ���:	62.5us=375		*/
#define FSX_HposToXpos(PosH)	(LONG)((PosH) << (32 - FSX_FPOS_SFT))
/*--------------------------------------------------------------------------------------------------*/
/*	�G���R�[�_�␳�ݒ肨��уA���[���FEncFlag(0390h)												*/
/*--------------------------------------------------------------------------------------------------*/
typedef enum _POSCOMP_DEF_ENCFLAG {					/* �G���R�[�_�␳�ݒ肨��уA���[��			*/
	COM_ALARM	= 0x00,
	ACC_ALARM	= 0x01,
	TRGSEL		= 0x02,
	MOT_AMPTYPE	= 0x03,
	SE_FBOPTUSE	= 0x04,
	ACC_CHKEN	= 0x05,
	SE_RXERR 	= 0x06,
	SE_RXCOMP 	= 0x07,
	ENCDATA_NG	= 0x08,
	POSDAT_SEL	= 0x09,
	OPRSEL_0	= 0x0A,
	OPRSEL_1	= 0x0B,
} POSCOMP_DEF_ENCFLAG;
/*--------------------------------------------------------------------------------------------------*/
typedef enum _POSCOMP_SET_TRGSEL {						/* �N�����@�ݒ�(bit2)						*/
	ENCINT		= 0x00,
	REGWRITE	= 0x01,
} POSCOMP_SET_TRGSEL;
/*--------------------------------------------------------------------------------------------------*/
typedef enum _POSCOMP_SET_POSDATSEL {					/* �ʒu�␳�C�l�[�u���ݒ�(bit9)				*/
	POSDATSEL_00	= 0x00,
	POSDATSEL_01	= 0x01,
	POSDATSEL_10	= 0x02,
	POSDATSEL_11	= 0x03,
} POSCOMP_SET_POSDATSEL;
/*--------------------------------------------------------------------------------------------------*/
/*	�V�t�g�ʐݒ�FPGDCAL(0394h)																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef enum _POSCOMP_DEF_PGDCAL {						/* �V�t�g�ʐݒ�								*/
	POS_SFTX	= 0x00,
	POS_SFTR	= 0x05,
	POS_LENG	= 0x0A,
	POS_SIGN	= 0x0F,
} POSCOMP_DEF_PGDCAL;
/*--------------------------------------------------------------------------------------------------*/
/*	���Z�b�g�ݒ�FRSTSET(03CCh)																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef enum _POSCOMP_DEF_RESET {						/* ���Z�b�g�ݒ�								*/
	ALMCLR0	= 0x00,
	ALMCLR1	= 0x01,
	ACENCLR = 0x05,
	SRECLR  = 0x06,
	SRCCLR  = 0x07,
} POSCOMP_DEF_RESET;

/*--------------------------------------------------------------------------------------------------*/
/*		�G���R�[�_�ʒu�␳�@�\ 	�ʐM�ُ�A���[����Ԏ擾											*/
/*--------------------------------------------------------------------------------------------------*/
__inline BOOL SHal_GetComAlarm( ASIC_HW_IF_STR *pAsicHwReg )
{
	return (BOOL)( (pAsicHwReg->AREG_ENCFLAG >> COM_ALARM) & 0x01 );
}
/*--------------------------------------------------------------------------------------------------*/
/*		�G���R�[�_�ʒu�␳�@�\ 	�����x�ُ�A���[����Ԏ擾											*/
/*--------------------------------------------------------------------------------------------------*/
__inline BOOL SHal_GetAccAlarm( ASIC_HW_IF_STR *pAsicHwReg )
{
	return (BOOL)( (pAsicHwReg->AREG_ENCFLAG >> ACC_ALARM) & 0x01 );
}
/*--------------------------------------------------------------------------------------------------*/
/*		�G���R�[�_�ʒu�␳�@�\ 	��M�G���[�t���O�擾												*/
/*--------------------------------------------------------------------------------------------------*/
__inline BOOL SHal_GetSeRxErr( ASIC_HW_IF_STR *pAsicHwReg )
{
	return (BOOL)( (pAsicHwReg->AREG_ENCFLAG >> SE_RXERR) & 0x01 );
}
/*--------------------------------------------------------------------------------------------------*/
/*		�G���R�[�_�ʒu�␳�@�\ 	�G���R�[�_�f�[�^�ُ�t���O�擾										*/
/*--------------------------------------------------------------------------------------------------*/
__inline BOOL SHal_GetEncDataNG( ASIC_HW_IF_STR *pAsicHwReg )
{
	return (BOOL)( (pAsicHwReg->AREG_ENCFLAG >> SE_RXERR) & 0x01 );
}
/*--------------------------------------------------------------------------------------------------*/
/*		�G���R�[�_�ʒu�␳�@�\ ��M�����t���O�N���A													*/
/*--------------------------------------------------------------------------------------------------*/
__inline void SHal_SrcClr( ASIC_HW_IF_STR *pAsicHwReg )
{
	pAsicHwReg->AREG_RSTSET |= 1 << SRCCLR;
}
/*--------------------------------------------------------------------------------------------------*/
/*		�G���R�[�_�ʒu�␳�@�\ �␳��ʒu�f�[�^(����)�̎擾(03E0h)									*/
/*--------------------------------------------------------------------------------------------------*/
__inline VULONG SHal_GetCurMotPos( ASIC_HW_IF_STR *pAsicHwReg )
{
	return pAsicHwReg->AREG_CURMOTPOS;
}
/*--------------------------------------------------------------------------------------------------*/
/*		�G���R�[�_�ʒu�␳�@�\ �␳��ʒu�f�[�^(�O��)�̎擾(03E4h)									*/
/*--------------------------------------------------------------------------------------------------*/
__inline VULONG SHal_GetLstMotPos( ASIC_HW_IF_STR *pAsicHwReg )
{
	return pAsicHwReg->AREG_LSTMOTPOS;
}
/*--------------------------------------------------------------------------------------------------*/
/*		�G���R�[�_�ʒu�␳�@�\ �␳�����ʒu�f�[�^(����)�̎擾(03E8h)								*/
/*--------------------------------------------------------------------------------------------------*/
__inline VULONG SHal_GetCurRcvPos( ASIC_HW_IF_STR *pAsicHwReg )
{
	return pAsicHwReg->AREG_CURRCVPOS;
}
/*--------------------------------------------------------------------------------------------------*/
/*		�G���R�[�_�ʒu�␳�@�\ �␳�����ʒu�f�[�^(�O��)�̎擾(03ECh)								*/
/*--------------------------------------------------------------------------------------------------*/
__inline VULONG SHal_GetLstRcvPos( ASIC_HW_IF_STR *pAsicHwReg )
{
	return pAsicHwReg->AREG_LSTRCVPOS;
}
/*--------------------------------------------------------------------------------------------------*/
/*		�G���R�[�_�ʒu�␳�@�\ ��M�f�[�^L(����)�̎擾(03F0h)										*/
/*--------------------------------------------------------------------------------------------------*/
__inline VULONG SHal_GetCurRxPosL( ASIC_HW_IF_STR *pAsicHwReg )
{
	return pAsicHwReg->AREG_CURRXPOS0_L;
}
/*--------------------------------------------------------------------------------------------------*/
/*		�G���R�[�_�ʒu�␳�@�\ ��M�f�[�^H(����)�̎擾(03F4h)										*/
/*--------------------------------------------------------------------------------------------------*/
__inline VULONG SHal_GetCurRxPosH( ASIC_HW_IF_STR *pAsicHwReg )
{
	return pAsicHwReg->AREG_CURRXPOS0_H;
}
/*--------------------------------------------------------------------------------------------------*/
/*		�G���R�[�_�ʒu�␳�@�\ ��M�f�[�^L(�O��)�̎擾(03F8h)										*/
/*--------------------------------------------------------------------------------------------------*/
__inline VULONG SHal_GetLstRxPosL( ASIC_HW_IF_STR *pAsicHwReg )
{
	return pAsicHwReg->AREG_LSTRXPOS0_L;
}
/*--------------------------------------------------------------------------------------------------*/
/*		�G���R�[�_�ʒu�␳�@�\ ��M�f�[�^H(�O��)�̎擾(03FCh)										*/
/*--------------------------------------------------------------------------------------------------*/
__inline VULONG SHal_GetLstRxPosH( ASIC_HW_IF_STR *pAsicHwReg )
{
	return pAsicHwReg->AREG_LSTRXPOS0_H;
}
/* <S004> End */
/*--------------------------------------------------------------------------------------------------*/
/*		�G���R�[�_�ʒu�␳�@�\ �x�[�X�^�C�}�l�̎擾(03B4h)											*//* <S067> */
/*--------------------------------------------------------------------------------------------------*/
__inline VULONG SHal_GetBaseTim( ASIC_HW_IF_STR *pAsicHwReg )
{
	return pAsicHwReg->AREG_BASTIM;
}
/*--------------------------------------------------------------------------------------------------*/
/*		�G���R�[�_�ʒu�␳�@�\ �x�[�X�^�C�}�l�̎擾(03B4h)											*//* <S067> */
/*--------------------------------------------------------------------------------------------------*/
__inline VULONG SHal_GetHenkaItiTim( ASIC_HW_IF_STR *pAsicHwReg )
{
	return pAsicHwReg->AREG_HENKAITITIM;
}



/*--------------------------------------------------------------------------------------------------*/
/*		�G���R�[�_���x�f�[�^�̎擾												<S03A>				*/
/*--------------------------------------------------------------------------------------------------*/
__inline USHORT SHal_GetEncAlmCode( ASIC_HW_IF_STR *pAsicHwReg, UINT ch )
{
	USHORT ret;
	if( ch == 0 )
	{
		ret = (USHORT)(pAsicHwReg->AREG_SE0_RXD23);
	}
	else
	{
		ret = (USHORT)(pAsicHwReg->AREG_SE1_RXD23);
	}
	return (ret);
}



/*--------------------------------------------------------------------------------------------------*/
/*		�J�E���^���b�`(CMD,FB,FC)																	*/
/*--------------------------------------------------------------------------------------------------*/
#define LODSET_SET_LDCMD	0x02C0


/* JL-077 �̏���͏�Ԏ擾 */
__inline USHORT SHalDrv_GetFaultStatus(ASIC_HW_IF_STR *AsicHwReg, USHORT mask)
{
	return (AsicHwReg->AREG_FLTSTAT & mask);
}
__inline USHORT SHalDrv_GetSetreg1Status(ASIC_HW_IF_STR *AsicHwReg)/* <S126> */
{
	return (AsicHwReg->AREG_SETREG1);
}
/* JL-077 �}�C�N�����Z��WDT��Ԏ擾 */
__inline USHORT SHalDrv_GetMicroWdtStatus(ASIC_HW_IF_STR *AsicHwReg)
{
	return (AsicHwReg->AREG_WDT1SET & 0x0040);
}

/* JL-077 �A���[�����b�`���Z�b�g */
__inline void SHalDrv_ResetAlarmLtStatus(ASIC_HW_IF_STR *AsicHwReg)
{
//	AsicHwReg->AREG_ALMCLR = 0x802F;
	AsicHwReg->AREG_ALMCLR = 0x813F;
}

/* AD�ϊ��f�W���[�V�����t�B���^�̃A���[���N���A */
__inline void SHalDrv_ResetCrtAdAlarmStatus(ASIC_U_IF_STR *AsicMicroReg)
{
	AsicMicroReg->IntAdP.FccRst = 0x01;
}

/* JL-077 BBON�M�����b�`���Z�b�g */
__inline void SHalDrv_ResetBBONLtStatus(ASIC_HW_IF_STR *AsicHwReg)
{
	AsicHwReg->AREG_ALMCLR |= (BIT2 | BIT3);
}

/* JL-077 BBON�M����Ԏ擾 */
__inline void SHalDrv_GetBBONStatus(ASIC_HW_IF_STR *AsicHwReg, BOOL *Bbon1, BOOL *Bbon2)
{
	ULONG reg = AsicHwReg->AREG_ALMSET;
	*Bbon1 = (reg >> 11) & 0x1;
	*Bbon2 = (reg >> 12) & 0x1;
}

/* JL-077 BBON�M�����b�`��Ԏ擾 */
__inline void SHalDrv_GetBBONLtStatus(ASIC_HW_IF_STR *AsicHwReg, BOOL *Bbon1, BOOL *Bbon2)
{
	ULONG reg = AsicHwReg->AREG_SETREG1;
	*Bbon1 = (reg >> 8) & 0x1;
	*Bbon2 = (reg >> 9) & 0x1;
}

/* JL-077 DBON�M���o�͗L�� */
__inline void SHalDrv_EnableDynamicBrake(ASIC_HW_IF_STR *AsicHwReg)
{
	AsicHwReg->AREG_TIM1SET = 0x009C;
}

/* JL-077 DBON�M��ON */
__inline void SHalDrv_DynamicBrakeON(ASIC_HW_IF_STR *AsicHwReg)
{
#ifndef CSW_CSTR_DB_REVERSE
	AsicHwReg->AREG_ALMSET &= ~(BIT5);
#else
	AsicHwReg->AREG_ALMSET |= BIT5;
#endif
}

/* JL-077 DBON�M��OFF */
__inline void SHalDrv_DynamicBrakeOFF(ASIC_HW_IF_STR *AsicHwReg)
{
#ifndef CSW_CSTR_DB_REVERSE
	AsicHwReg->AREG_ALMSET |= BIT5;
#else
	AsicHwReg->AREG_ALMSET &= ~(BIT5);
#endif
}

/* JL-077 Charge Pump ON */
__inline void SHalDrv_ChargePumpON(ASIC_HW_IF_STR *AsicHwReg)
{
	AsicHwReg->AREG_PUMPON = 0x02FF;
}

/* JL-077 Charge Pump OFF */
__inline void SHalDrv_ChargePumpOFF(ASIC_HW_IF_STR *AsicHwReg)
{
	AsicHwReg->AREG_PUMPON = 0x00FF;
}

/* <S0CB>	STart */
/* JL-077 Charge Pump(P��) ON */
__inline void SHalDrv_ChargePumpON_P(ASIC_HW_IF_STR *AsicHwReg)
{
	AsicHwReg->AREG_PUMPON = 0x04FF;
}
/* <S0CB>	End */

/* JL-077 Relay Control (use DB I/F) *//* <S0A2S0A3> */
__inline void SHalDrv_RelayControl(ASIC_HW_IF_STR *AsicHwReg, BOOL sts)
{
	if( sts == TRUE )
	{
		AsicHwReg->AREG_ALMSET |= (BIT5);
	}
	else
	{
		AsicHwReg->AREG_ALMSET &= ~(BIT5);
	}
}

/* JL-077 Base Block ON */
__inline void SHalDrv_BaseBlock(ASIC_HW_IF_STR *AsicHwReg, ASIC_U_IF_STR *AsicMicroReg) /* <S0E4> */
{
	AsicHwReg->AREG_SETREG1 = 0x0011;/* BaseBlock */
	AsicHwReg->MREG_CTSTR = MPCMD_SVOFF;/* Micro ServoOff */
	AsicMicroReg->CtrlStsIn = MPCMD_SVOFF;/* Micro ServoOff *//* <S0E4> */
	AsicHwReg->AREG_WDT1SET |= BIT6;/*  Stop Micro WDT */
}

/* JL-077 Base Enable ON */
__inline void SHalDrv_BaseEnable(ASIC_HW_IF_STR *AsicHwReg, ASIC_U_IF_STR *AsicMicroReg)
{
	AsicMicroReg->IntAdP.FccRst = 0x01;	/* Reset Current A/D */
	AsicHwReg->MREG_CTSTR = MPCMD_SVON;/* Micro ServoOn */
	AsicMicroReg->CtrlStsIn = MPCMD_SVON;/* Micro ServoOn *//* <S0E4> */
	AsicHwReg->AREG_WDT1SET &= ~(BIT6);/* start Micro WDT for JL076FPGA */
	AsicHwReg->AREG_SETREG1 = 0x0010;/* Base Enable */
}

/* JL-077 WDT Kick */
__inline void SHalDrv_KickWDT( ASIC_HW_IF_STR *AsicHwReg, BOOL TestMode4 )
{
	if( TestMode4 == FALSE)
	{
		AsicHwReg->AREG_HINT1 = 0x0000;
	}
}

/* JL-077 Latch ASIC Pulse Counter */
__inline void SHalDrv_LatchPulseCntr( ASIC_HW_IF_STR *AsicHwReg )
{
	AsicHwReg->AREG_LODSET = LODSET_SET_LDCMD;
}

/* JL-077 Read Reference A/D */
__inline LONG SHalDrv_ReadRefAd( ASIC_HW_IF_STR *AsicHwReg )
{
	SHORT	RefAd;
	AsicHwReg->AREG_ADSET1 |= BIT14;
	RefAd = (SHORT)AsicHwReg->AREG_ACUL;
	return ((LONG)RefAd);
}

/* Set Encoder Transmit Data to JL-086 */
__inline void SHalDrv_SencSetTxData( void *TxBufAdr, void *TxData )
{
	UCHAR	i;
	for( i=0; i<ENC_BUF_SIZE_L; i++ )
	{
		((ULONG*)TxBufAdr)[i] = ((ULONG*)TxData)[i];
	}
}

/* Get Encode Received Data from JL-086 */
__inline void SHalDrv_SencGetRxData( void *RxBufAdr, void *RxData )
{
	UCHAR	IndexR;
	for( IndexR=0; IndexR<ENC_BUF_SIZE_L; IndexR++ )
	{
		((ULONG*)RxData)[IndexR] = ((ULONG*)RxBufAdr)[IndexR];
	}
}

/* Set Encode Transmission Start Request to JL-086 */
__inline void SHalDrv_SencStartTx( void *TxStartAdr, void *TxCmd )
{
	*((ULONG*)TxStartAdr) = *((ULONG*)TxCmd);
}

/* Get Encode Receive Status from JL-086 */
__inline ULONG SHalDrv_SencGetRxStatus( void *RxFlgAdr )
{
	return	*((ULONG*)RxFlgAdr);
}

/* Enable the Encoder Interrupt */
__inline void SHalDrv_EnableEncoderInterrupt( ASIC_HW_IF_STR *AsicHwReg )
{
	AsicHwReg->AREG_INT1SET |= BIT5;
}

/* Sync the Encoder Communication with CPU Operation */
__inline void SHalDrv_SyncEncoderCommunication( ASIC_HW_IF_STR *AsicHwReg, USHORT* RxFlgAdr )
{
	*RxFlgAdr |= BIT8;				/* TXKCS    = 1	*/
	AsicHwReg->AREG_ESYNC = 0x0000;	/* ESYNC0,1 = 0	*/
	KlibWaitns( 200 );				/* Wait 200ns + ReadModifyWrite AREG	*/
	*RxFlgAdr &= ~BIT8;				/* TXKCS    = 0	*/
}

/* Enable the Encoder Interrupt */
__inline USHORT SHalDrv_AckEncoderInterrupt( ASIC_HW_IF_STR *AsicHwReg )
{
	USHORT dummy;
	dummy = (USHORT)AsicHwReg->AREG_INT1SET;
	return dummy;
}


#if 0  /* <S003> */
/* Change Encoder Communication Mode to P0 */
__inline void SHalDrv_ChangeToP0Mode( ASIC_HW_IF_STR *AsicHwReg, UINT ch )
{
	if( ch == 0 )
	{
		AsicHwReg->AREG_SEPGSET00 &= (~BIT0);			/* �O���������� */
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)
		AsicHwReg->AREG_SEPGSET00 |= BIT12;				/* ������������ */
		AsicHwReg->AREG_SEPGSET01 &= 0x0000F1FF;		/* P0�}�X�^16�o�C�g */
		AsicHwReg->AREG_SEPGSET01 |= 0x00000100;		/* ���W�X�^�������ݓ��� */
#else
														/* �������������͎��{���Ȃ� */
		AsicHwReg->AREG_SEPGSET01 &= 0x0000F1FF;		/* P0�}�X�^16�o�C�g */
														/* ���W�X�^�������ݓ����͎��{���Ȃ� */
#endif
		AsicHwReg->AREG_SE0_TXD01 = 0x00000000;			/* ���M�o�b�t�@�N���A */
		AsicHwReg->AREG_SRCNT0 = 0x000003E7;			/* 125us */
	}
	else
	{
		AsicHwReg->AREG_SEPGSET10 &= (~BIT0);			/* �O���������� */
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)
		AsicHwReg->AREG_SEPGSET10 |= BIT12;				/* ������������ */
		AsicHwReg->AREG_SEPGSET11 &= 0x0000F4FF;		/* P0�}�X�^16�o�C�g */
		AsicHwReg->AREG_SEPGSET11 |= 0x00000400;		/* ���W�X�^�������ݓ��� */
#else
														/* �������������͎��{���Ȃ� */
		AsicHwReg->AREG_SEPGSET11 &= 0x0000F1FF;		/* P0�}�X�^16�o�C�g */
														/* ���W�X�^�������ݓ����͎��{���Ȃ� */
#endif
		AsicHwReg->AREG_SE1_TXD01 = 0x00000000;			/* ���M�o�b�t�@�N���A */
		AsicHwReg->AREG_SRCNT1 = 0x000003E7;			/* 125us */
	}
}

/* Change Encoder communication Mode to P1 */
__inline void SHalDrv_ChangeToP1Mode( ASIC_HW_IF_STR *AsicHwReg, UINT ch )
{
	if( ch == 0 )
	{
		AsicHwReg->AREG_SEPGSET00 &= 0xFFFFDFFF;	/* �`�������^�C�}���� */
		AsicHwReg->AREG_SEPGSET01 &= 0x0000F4FF;	/* �`�������^�C�}�g�p */
		AsicHwReg->AREG_SEPGSET01 |= 0x00000400;	/* P1���[�h�ݒ� */
		AsicHwReg->AREG_SRCNT0 = 0x000001F3;		/* 62.5us */
	}
	else
	{
		AsicHwReg->AREG_SEPGSET10 &= 0xFFFFDFFF;	/* �`�������^�C�}���� */
		AsicHwReg->AREG_SEPGSET11 &= 0x0000F4FF;
		AsicHwReg->AREG_SEPGSET11 |= 0x00000400;	/*P1���[�h�ݒ� */
		AsicHwReg->AREG_SRCNT1 = 0x000001F3;		/* 62.5us */
	}
}
#else


/* Change Encoder Communication Mode to P0 for Initialize */ /* <S0F8> */
__inline void SHalDrv_InitChangeToP0Mode( ASIC_HW_IF_STR *AsicHwReg, UINT ch )
{
	if( ch == 0 )
	{
		AsicHwReg->AREG_SEPGSET00 &= (~BIT0);			/* �O���������� */
		AsicHwReg->AREG_SEPGSET00 |= (BIT12 | BIT13);	/* ������������ & �`�������^�C�}��~ *//* @@CHECK ���荞�݃��W���[���Œx�������ɂ���K�v����H�H */
		AsicHwReg->AREG_SEPGSET01 &= 0x0000F1FF;		/* P0�}�X�^16�o�C�g */
		AsicHwReg->AREG_SEPGSET01 |= 0x00000100;		/* ���W�X�^�������ݓ��� */
		AsicHwReg->AREG_SE0_TXD01 = 0x00000000;			/* ���M�o�b�t�@�N���A */
		AsicHwReg->AREG_SRCNT0 = 0x000003E7;			/* 125us */
	}
	else
	{
		AsicHwReg->AREG_SEPGSET10 &= (~BIT0);			/* �O���������� */
		AsicHwReg->AREG_SEPGSET10 |= (BIT12 | BIT13);	/* ������������ */
		AsicHwReg->AREG_SEPGSET11 &= 0x0000F1FF;		/* P0�}�X�^16�o�C�g */
		AsicHwReg->AREG_SEPGSET11 |= 0x00000100;		/* ���W�X�^�������ݓ��� */
		AsicHwReg->AREG_SE1_TXD01 = 0x00000000;			/* ���M�o�b�t�@�N���A */
		AsicHwReg->AREG_SRCNT1 = 0x000003E7;			/* 125us */
	}
}

/* Change Encoder Communication Mode to P0 */
/* ����������������P0���[�h�ؑւɂ͎g�p���Ȃ����� */
__inline void SHalDrv_ChangeToP0Mode( ASIC_HW_IF_STR *AsicHwReg, UINT ch )
{
	if( ch == 0 )
	{
		AsicHwReg->AREG_SEPGSET00 &= (~BIT0);			/* �O���������� */
//<S0F2>AsicHwReg->AREG_SEPGSET00 |= (BIT12 | BIT13);	/* ������������ & �`�������^�C�}��~ *//* @@CHECK ���荞�݃��W���[���Œx�������ɂ���K�v����H�H */
		AsicHwReg->AREG_SEPGSET01 &= 0x0000F1FF;		/* P0�}�X�^16�o�C�g */
//<S0F2>AsicHwReg->AREG_SEPGSET01 |= 0x00000100;		/* ���W�X�^�������ݓ��� */
		AsicHwReg->AREG_SE0_TXD01 = 0x00000000;			/* ���M�o�b�t�@�N���A */
//		AsicHwReg->AREG_SRCNT0 = 0x000003E7;			/* 125us *//* <S1D3> */
	}
	else
	{
		AsicHwReg->AREG_SEPGSET10 &= (~BIT0);			/* �O���������� */
//<S134>AsicHwReg->AREG_SEPGSET10 |= (BIT12 | BIT13);	/* ������������ */
		AsicHwReg->AREG_SEPGSET11 &= 0x0000F1FF;		/* P0�}�X�^16�o�C�g */
//<S134>AsicHwReg->AREG_SEPGSET11 |= 0x00000100;		/* ���W�X�^�������ݓ��� */
		AsicHwReg->AREG_SE1_TXD01 = 0x00000000;			/* ���M�o�b�t�@�N���A */
		AsicHwReg->AREG_SRCNT1 = 0x000003E7;			/* 125us */
	}
}

/* Change Encoder communication Mode to P1 */
__inline void SHalDrv_ChangeToP1Mode( ASIC_HW_IF_STR *AsicHwReg, UINT ch )
{
	if( ch == 0 )
	{
		AsicHwReg->AREG_SEPGSET00 &= 0xFFFFDFFF;	/* �`�������^�C�}���� *//* @@CHECK ���荞�݃��W���[���Œx�������ɂ���K�v����H�H */
#if (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE)/* <S004> */
		AsicHwReg->AREG_SEPGSET00 &= 0xFFFFEFFF;	/* ������������ */
#endif
		AsicHwReg->AREG_SEPGSET01 &= 0x0000F4FF;	/* �`�������^�C�}�g�p */
		AsicHwReg->AREG_SEPGSET01 |= 0x00000400;	/* P1���[�h�ݒ� */
		AsicHwReg->AREG_SRCNT0 = 0x000001F3;		/* 62.5us */
	}
	else
	{
		AsicHwReg->AREG_SEPGSET10 &= 0xFFFFDFFF;	/* �`�������^�C�}���� *//* @@CHECK ���荞�݃��W���[���Œx�������ɂ���K�v����H�H */
#if (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE)/* <S004> */
		AsicHwReg->AREG_SEPGSET10 &= 0xFFFFEFFF;	/* ������������ */
#endif
		AsicHwReg->AREG_SEPGSET11 &= 0x0000F4FF;	/* �`�������^�C�}�g�p */
		AsicHwReg->AREG_SEPGSET11 |= 0x00000400;	/* P1���[�h�ݒ� */
		AsicHwReg->AREG_SRCNT1 = 0x000001F3;		/* 62.5us */
	}
}

#endif
/* Encoder alm reset */
__inline VULONG SHalDrv_EncAlmReset( ASIC_HW_IF_STR *AsicHwReg )/* <S217> */
{
	AsicHwReg->AREG_RSTSET = (( 1 << ALMCLR0 )					/* ComAlarm�N���A					*/
							+ ( 1 << ALMCLR1 )					/* AccAlarm�N���A					*/
							+ ( 1 << ACENCLR )					/* �����x�`�F�b�N�C�l�[�u���N���A	*/
							+ ( 1 << SRECLR )					/* ��M�G���[�t���O�N���A			*/
							+ ( 1 << SRCCLR ));					/* ��M�����t���O�N���A				*/
							
	return AsicHwReg->AREG_COMERRCNT;
}

 /* <S003> */

/* Set Encoder communication Rate */
#if 0/* <S04D> */
__inline void SHalDrv_SetSencComRate( ASIC_HW_IF_STR *AsicHwReg, UCHAR ComRate )
{
	if( ComRate == SENC_COMRATE_4MBPS )
	{
		AsicHwReg->AREG_SETREG1 = 0x0005;				/* ASIC Write Enable		*/
		AsicHwReg->AREG_CLKO &= 0xF0FF;					/* Set Ch0 and ch1 to 4Mbps	*/
		AsicHwReg->AREG_SETREG1 = 0x0001;				/* ASIC Write Disable		*/
	}
	else if( ComRate == SENC_COMRATE_8MBPS )
	{
		AsicHwReg->AREG_SETREG1 = 0x0005;				/* ASIC Write Enable		*/
		AsicHwReg->AREG_CLKO &= 0xF0FF;					/* Reset Ch0 and ch1		*/
		AsicHwReg->AREG_CLKO |= 0x0500;					/* Set Ch0 and ch1 to 8Mbps	*/
		AsicHwReg->AREG_SETREG1 = 0x0001;				/* ASIC Write Disable		*/
	}
}
#else
__inline void SHalDrv_SetSencComRate( ASIC_HW_IF_STR *AsicHwReg, UCHAR ComRate, UCHAR ch )/* <S04D> */
{
	if( ComRate == SENC_COMRATE_4MBPS )
	{
		AsicHwReg->AREG_SETREG1 = 0x0005;				/* ASIC Write Enable		*/
		if(ch == 0)
		{/* Set Ch0 to 4Mbps			*/
			AsicHwReg->AREG_CLKO &= 0xFCFF;				/* Bit8,9 = 0				*/
		}
		else
		{/* Set ch1 to 4Mbps			*/
			AsicHwReg->AREG_CLKO &= 0xF3FF;				/* Bit10,11 = 0				*/
		}
		AsicHwReg->AREG_SETREG1 = 0x0001;				/* ASIC Write Disable		*/
	}
	else if( ComRate == SENC_COMRATE_8MBPS )
	{
		AsicHwReg->AREG_SETREG1 = 0x0005;				/* ASIC Write Enable		*/
		if(ch == 0)
		{/* Set Ch0 to 8Mbps			*/
			AsicHwReg->AREG_CLKO &= 0xFCFF;				/* Bit8,9 = 0				*/
			AsicHwReg->AREG_CLKO |= 0x0100;				/* Bit8 = 1,Bit9 = 0		*/
		}
		else
		{/* Set ch1 to 8Mbps			*/
			AsicHwReg->AREG_CLKO &= 0xF3FF;				/* Bit10,11 = 0				*/
			AsicHwReg->AREG_CLKO |= 0x0400;				/* Bit8 = 1,Bit9 = 0		*/
		}
		AsicHwReg->AREG_SETREG1 = 0x0001;				/* ASIC Write Disable		*/
	}
}
#endif

/* Set Current Position for Divide Out */
__inline void SHalDrv_SetDivOutPosition( ASIC_HW_IF_STR *AsicHwReg, LONG Pos )
{
	AsicHwReg->AREG_POSET0 = (ULONG)Pos;
}

/* Set Origin Correction 1 for Divide Out */
__inline void SHalDrv_SetOriginCorrect1( ASIC_HW_IF_STR *AsicHwReg, LONG CorrectVal )
{
	AsicHwReg->AREG_POSET1 = (ULONG)CorrectVal;
}

/* Set Origin Correction 2 for Divide Out */
__inline void SHalDrv_SetOriginCorrect2( ASIC_HW_IF_STR *AsicHwReg, LONG CorrectVal )
{
	AsicHwReg->AREG_POSET2 = (ULONG)CorrectVal;
}

/* Get Fault Status from MicroProgram */
__inline USHORT SHalDrv_GetMicroFaultSts( ASIC_U_IF_STR *AsicMicroReg )
{
	return	(USHORT)AsicMicroReg->StsFlg.FccStsMon;
}

/* Set Carrer Frequency for MicroProgram */
__inline void SHalDrv_SetCarrerFreq( ASIC_U_IF_STR *AsicMicroReg, SHORT Carrer )
{
	AsicMicroReg->IntAdP.CrFreq = Carrer;
}

/* Set Carrer Frequency for MicroProgram */
__inline void SHalDrv_SetMotSpdToMicro( ASIC_U_IF_STR *AsicMicroReg, LONG MotSpd )
{
	SHORT	MotSpdMp;
	MotSpdMp = (SHORT)MlibMulhigh32( MotSpd, 3840000 );	/* Motor Speed for Micro 		*/
	AsicMicroReg->AdinV.VelIn = MotSpdMp;				/* �d�l�e�⏞�v�Z�p���x			*/
}

/* Set EnableComsyncsel for MicroProgram *//* <S04D> Ch1�����ǉ� */
__inline void SHalDrv_SetEnableComsyncsel( ASIC_HW_IF_STR *AsicHwReg )
{
	VULONG	SetValue0;
	VULONG	SetValue1;

	SetValue0 = AsicHwReg->AREG_SEPGSET00 & ~COMSYNCSEL_MASK;
	SetValue1 = AsicHwReg->AREG_SEPGSET10 & ~COMSYNCSEL_MASK;
	
#if (CSW_NETIF_TYPE == NETIF_M2_TYPE)
	SetValue0 |= 0x00000020;	/*  Encoder CH0 Sync with M-II Network				*/
	SetValue1 |= 0x00000020;	/*  Encoder CH1 Sync with M-II Network				*/
#elif (CSW_NETIF_TYPE == NETIF_M3_TYPE)
	SetValue0 |= 0x00000080;	/*  Encoder CH0 Sync with M-III Network				*/
	SetValue1 |= 0x00000080;	/*  Encoder CH1 Sync with M-III Network				*/
#else /* (CSW_NETIF_TYPE == NETIF_CM_TYPE) */
	SetValue0 |= 0x00000001;	/*  Encoder CH0 Sync with SYNCL Signal				*/
	SetValue1 |= 0x00000001;	/*  Encoder CH1 Sync with SYNCL Signal				*/
#endif
	AsicHwReg->AREG_SEPGSET00 = SetValue0;
	AsicHwReg->AREG_SEPGSET10 = SetValue1;
}

/* Set DisableComsyncsel  for MicroProgram *//* <S04D> Ch1�����ǉ� */
__inline void SHalDrv_SetDisableComsyncsel( ASIC_HW_IF_STR *AsicHwReg )
{
	VULONG	SetValue0;
	VULONG	SetValue1;
	
	SetValue0 = AsicHwReg->AREG_SEPGSET00 & ~COMSYNCSEL_MASK;
	SetValue1 = AsicHwReg->AREG_SEPGSET10 & ~COMSYNCSEL_MASK;
	
	AsicHwReg->AREG_SEPGSET00 = SetValue0;
	AsicHwReg->AREG_SEPGSET10 = SetValue1;
}

/* Reset Intcycle  for MicroProgram */
__inline void SHalDrv_Reset_Intcycle( ASIC_HW_IF_STR *AsicHwReg )
{
	VULONG	SetValue;

	SetValue = AsicHwReg->AREG_INTCYCLE;
	AsicHwReg->AREG_INTCYCLE = SetValue;
}

/* Enable the MEHCATROLINK-2 Interrupt */
__inline void SHalDrv_Enable_MEHCATROLINK2Interrupt( ASIC_HW_IF_STR *AsicHwReg )
{
	AsicHwReg->AREG_INT1SET |= BIT18;
}

/* Enable the Encoder Interrupt */
__inline BOOL SHalDrv_Ack_MEHCATROLINK2Interrupt( ASIC_HW_IF_STR *AsicHwReg )
{
	BOOL ret;
	
	ret = (BOOL)((AsicHwReg->AREG_INT1SET >> 26 ) & 0x01);
	return ret;
}
/****************************************************************************************************/
/*		Latch Function 																				*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _SHAL_LATCH_STS_ADR {
	VULONG*		STimLtAdr;
	VULONG*		SFbLtAdr;
	UCHAR		STimShift;
} SHAL_LATCH_STS_ADR;/* <S004> */


typedef struct _SHAL_LATCH_STS {
	VBOOL			Complete;
	VULONG			LatchTime;
	VULONG			LatchPosition;
} SHAL_LATCH_STS;
/*--------------------------------------------------------------------------------------------------*/
#define HW_SI_NO_DEFINE	0	/* <S004> */
#define HW_SI4			1		/* <S004> */
#define HW_SI5			2		/* <S004> */
#define HW_SI6			3		/* <S004> */

/*--------------------------------------------------------------------------------------------------*/
/* JL-077 Read Latch timer(Position) status */
__inline void SHalDrv_ReadLatchPosition(ASIC_HW_IF_STR *AsicHwReg, SHAL_LATCH_STS_ADR* LtStsAdr, SHAL_LATCH_STS* LtSts, BOOL* ExtLtEnable)
{
	LONG	i;
	VULONG	latchState;
	VULONG	dummy;

	latchState = AsicHwReg->AREG_STIMSTAT;
	for(i=0; i < 3; i++ )
	{
		if(ExtLtEnable[i] == TRUE)
		{
			LtSts[i].Complete = (latchState >> LtStsAdr[i].STimShift) & 1;	/* ׯ�����				*/
			LtSts[i].LatchTime = *(LtStsAdr[i].STimLtAdr);									/* ���ׯ��l				*/
			LtSts[i].LatchPosition	= *(LtStsAdr[i].SFbLtAdr);								/* ׯ���̨����ޯ��ʒu	*/
		}
	}

}


/****************************************************************************************************/
/*		Initializing Servo ASIC functions 															*/
/****************************************************************************************************/
void SHal_AsicSwReset( ASIC_HW_IF_STR *pAsicHwReg ); /* <S0E4> */
void SHal_InitAsicA( ASIC_HW_IF_STR* pAsicHwReg );
BOOL SHal_InitAsicB( ASIC_HW_IF_STR* pAsicHwReg, SVIP_INIT_PRM *pSvipInitPrm );
BOOL SHal_PrstAsicB( ASIC_HW_IF_STR* pAsicHwReg, SVIP_INIT_PRM *pSvipInitPrm );

BOOL SHal_InitMicroProgram( ASICS *SvAsicRegs );
BOOL SHal_LoadAsicMicroProgram( ASICS *SvAsicRegs, CULONG *src_adr, LONG u_size);

//void SHal_InitEncRdDly( ASIC_HW_IF_STR *pAsicHwReg, USHORT PgBaudRate );		/*<S09C>*//* <S0E4> */
void SHal_InitEncRdDly( ASIC_HW_IF_STR *pAsicHwReg, USHORT PgBaudRate, UINT ch );		/*<S09C>*//* <S0E4> */
void SHal_InitEncRdDly_ch0( ASIC_HW_IF_STR *pAsicHwReg, USHORT PgBaudRate );	/*<S09C>*/
void SHal_InitEncRdDly_ch1( ASIC_HW_IF_STR *pAsicHwReg, USHORT PgBaudRate );	/*<S09C>*/
void SHal_InitEncPosComp( ASIC_HW_IF_STR *pAsicHwReg, SVIP_INIT_PCOMP *SvipInitPrm );/* <S004> */
void SHal_InitEncPosCompAccLvl( ASIC_HW_IF_STR *pAsicHwReg );/* <S004> */
void SHal_SrcClr( ASIC_HW_IF_STR *pAsicHwReg );/* <S004> */

BOOL SHal_ResetAsicMicroRegister( ASICS *SvAsicRegs );
void SHal_StartMicroProgram( ASICS *SvAsicRegs );
void SHal_StopMicroProgram( ASICS *SvAsicRegs );
ULONG SHal_ClockToTime( USHORT clock_diff );
USHORT SHal_GetSysClock( ASICS *SvAsicRegs );
USHORT SHal_Get_dClock( USHORT last_clk , USHORT clk );

BOOL SHalDrv_InitLatchSignal(ASIC_HW_IF_STR *AsicHwReg, SHAL_LATCH_STS_ADR* LtStsAdr,
												USHORT PrmInselA, BOOL* ExtLtEnable);/* <S004> */

#ifdef CSW_MICRO_DEBUG_MODE
void SHal_SetMicroDebugMode( ASIC_HW_IF_STR *pAsicHwReg );
#endif



/****************************************************************************************************/
/*		Micro Program code																			*/
/****************************************************************************************************/
extern	const ULONG	YecAsicPrg_SGD7[ ];
extern	const ULONG	JL086UprgSz_SGD7;
//extern	const USHORT	YecAsicPrg_INGRAM[ ];
//extern	const USHORT	JL076UprgSz_INGRAM;
//extern	const USHORT	YecAsicPrg_sgdv[ ];
//extern	const USHORT	JL076UprgSz_sgdv;

#endif /* _JL076_USE_ */

#endif	/* _JL086_SERVOIP_HW_H */

