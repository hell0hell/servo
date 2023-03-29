/****************************************************************************************************/
/*																									*/
/*																									*/
/*		ScanI.c : Mercury�d������v���O����															*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
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
/*		Rev.0.00 : 2012.08.06  Y.Tanaka		�EJL-086�����d������C���ꉻ�p�Ƀo�[�W������蒼��		*/
/*		Rev.0.01 : 2012.08.17  Y.Tanaka		�E�\���́A���[�J���ϐ�������							*/
/*		Rev.0.02 : 2012.08.20  Y.Tanaka		�E�\���́A���[�J���ϐ�������							*/
/*																									*/
/****************************************************************************************************/
//#include "Basedef.h"
/*--------------------------------------------------------------------------------------------------*/
#include "IxInst.h"
#include "MprgStruct.h"
#include "MpConstTbl.h"						/* �萔�e�[�u���ǂݍ���								*/

#if defined(WIN32)
	#include "IlibSvc.h"					/* VC�ł݂̂Ŏg�p									*/
#include "MprgLmtChkVCMacro.h"				/* �����Z���~�b�g���o�p�}�N����`					*/
#endif

/****************************************************************************************************/
/*		Definitions																					*/
/****************************************************************************************************/
#define	MSW_VER			0x0001				/* �\�t�g�o�[�W�����ݒ�									*/
#define	TST_VER			0x0000				/* �e�X�g�o�[�W�����ݒ�									*/
#define	YSP_VER			0x0000				/* Y�d�l�o�[�W�����ݒ�									*/

#define	MAX_AXIS_NUM	2					/* �ő吧�䎲��											*/
/****************************************************************************************************/


/****************************************************************************************************/
/*		ProtoType																					*/
/****************************************************************************************************/
void	MpDataClear( MICRO_AXIS_HANDLE *AxisRsc );			/* �}�C�N���p�f�[�^�N���A				*/
void	MpIntHost( void );
USHORT	MpSQRT( INTADWK *IntAdwk, ULONG src );
void	MpOVMMODK( INTADP *IntAdP, INTADV *IntAdV, INTADWK *IntAdwk );

#if defined(WIN32)
SVIP_READ_REG	SvIpReadReg[MAX_AXIS_NUM];
SVIP_WRITE_REG	SvIpWriteReg[MAX_AXIS_NUM];
#endif

/* for debug */
long	micro_debug_ctr;
long	micro_debug_buf1[2000];
long	micro_debug_buf2[2000];
long	micro_debug_buf3[2000];
long	micro_debug_buf4[2000];
long	micro_debug_buf5[2000];
long	micro_debug_buf6[2000];
/* for debug */


/****************************************************************************************************/
/*																									*/
/*		����������																					*/
/*																									*/
/****************************************************************************************************/
#ifdef ASIP_CC
void	main( void )
#elif defined(WIN32)
void	MpStart( void )
#endif
{
	USHORT				ax_no;
	MICRO_AXIS_HANDLE	*AxisRsc;

/*--------------------------------------------------------------------------------------------------*/
/*		interupt set																				*/
/*--------------------------------------------------------------------------------------------------*/
/*		�o�[�W�����ݒ�																				*/
/*--------------------------------------------------------------------------------------------------*/
	VerInfo.MswVer = MSW_VER; 			/* �\�t�g�o�[�W�����ݒ�										*/
	VerInfo.TstVer = TST_VER;			/* �e�X�g�o�[�W�����ݒ�										*/
	VerInfo.YspVer = YSP_VER;			/* Y�d�l�o�[�W�����ݒ�										*/

/*--------------------------------------------------------------------------------------------------*/
/*		Get Axis Num from CPU																		*/
/*--------------------------------------------------------------------------------------------------*/
#if 0	/* ���ǉ��K�v�� */
	if( �擾���� <= MAX_AXIS_NUM )
	{
		AxisNum = �擾����;
	}
	else
	{
		AxisNum = MAX_AXIS_NUM;
	}
#elif defined(WIN32)
	/* MicroCallMpStart()�ɂĐݒ�ς� */
#else
	/* �b�菈�u */
	AxisInfo.AxisNum = 1;
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		Set H/W Register Address Pointer															*/
/*--------------------------------------------------------------------------------------------------*/
	for( ax_no = 0; ax_no < AxisInfo.AxisNum; ax_no++ )
	{
		AxisRsc = &AxisHdl[ax_no];
#if defined(WIN32)
		AxisRsc->SvIpRegR = &(SvIpReadReg[ax_no]);
		AxisRsc->SvIpRegW = &(SvIpWriteReg[ax_no]);
#else
		AxisRsc->SvIpRegR = (SVIP_READ_REG*)(SVIP_TOP_ADR + (SVIP_AXIS_SIZE * ax_no));
		AxisRsc->SvIpRegW = (SVIP_WRITE_REG*)(SVIP_TOP_ADR + (SVIP_AXIS_SIZE * ax_no));
#endif
	}

/*--------------------------------------------------------------------------------------------------*/
/*		Set Interrupt Level																			*/
/*--------------------------------------------------------------------------------------------------*/
	/* level(AD=3, INT1=4, HOST=0) */
	/* ��H/W�A�N�Z�X�����ʂ̂��̂��܂Ƃ߂����I�I0���ڂ��ď����̂��i�D������ */
	AxisHdl[0].SvIpRegW->INTLVWR = 0x0043;

/*--------------------------------------------------------------------------------------------------*/
/*		Initialize variables																		*/
/*--------------------------------------------------------------------------------------------------*/
	for( ax_no = 0; ax_no < AxisInfo.AxisNum; ax_no++ )
	{
		AxisRsc = &AxisHdl[ax_no];

		AxisRsc->StsFlg.BbSetW = 0x2004;						/* INT1=Encoder0, BB				*/
		AxisRsc->SvIpRegW->BBSET = AxisRsc->StsFlg.BbSetW;		/* INT1=Encoder0, BB				*/
//		IxSetFuncAdr( ISA0, MpIntAD );		/* INT0(ISA0)��MpIntAD��ݒ�(�e�[�u��or�֐��|�C���^�ݒ�)						*/
//		IxSetFuncAdr( ISA1, MpIntEnc );		/* INT1(ISA1)��MpIntEnc��ݒ�(�e�[�u��or�֐��|�C���^�ݒ�)						*/
/*--------------------------------------------------------------------------------------------------*/
		AxisRsc->SvIpRegW->PCVS0 = AxisRsc->EncIfV.DivPls.s[0];	/* �p���X�ϊ��ʒu      (bit15-0)			*/
/*--------------------------------------------------------------------------------------------------*/
		AxisRsc->IHostWk.PoSet1W = AxisRsc->DivPlsV.PoSet1In;	/* MpUPDATE_DIVPOS()�Ŕ�r���������邽�ߎc���Ă���		*/
		AxisRsc->IHostWk.PoSet2W = AxisRsc->DivPlsV.PoSet2In;	/* MpUPDATE_DIVPOS()�Ŕ�r���������邽�ߎc���Ă���		*/
		AxisRsc->SvIpRegW->PCVS1 = AxisRsc->IHostWk.PoSet1W;	/* �p���X�ϊ����_�␳1 (bit15-0)							*/
		AxisRsc->SvIpRegW->PCVS2 = AxisRsc->IHostWk.PoSet2W;	/* �p���X�ϊ����_�␳2 (bit15-0)							*/
/*--------------------------------------------------------------------------------------------------*/
		AxisRsc->IHostWk.DivSetW = AxisRsc->DivPlsV.DivSetIn;	/* MpUPDATE_DIVPOS()�Ŕ�r���������邽�ߎc���Ă���		*/
		AxisRsc->SvIpRegW->DIVSET = AxisRsc->IHostWk.DivSetW;	/* �����@�\�ݒ� 											*/

/*--------------------------------------------------------------------------------------------------*/
//110914tanaka21 0,1,-1�͒萔�}�N�������邽�ߏ����������s�v
///*--------------------------------------------------------------------------------------------------*/
///*		Power on reset Register(�萔���W�X�^������)																*/
///*--------------------------------------------------------------------------------------------------*/
//		ZEROR = 0x00000000;	/* ZeroR,ZERORH <-- 0								*/
//		ONER = 0x00000001;	/* OneR,ONERH   <--  1								<V720>	*/
//		NONER = 0xffffffff;	/* NOneR,NONERH <-- -1								<V720>	*/
/*--------------------------------------------------------------------------------------------------*/
		AxisRsc->SinTbl.SinT = 0x0000;	/* SinTbl.SinT=	sin(��)			sin(0)=		 0.000 �� 0000h		*/
		AxisRsc->SinTbl.CosT = 0x4000;	/* SinTbl.CosT=	cos(��)			cos(0)=		 1.000 �� 4000h		*/
		AxisRsc->SinTbl.SinT2 = 0x376D;	/* SinTbl.SinT2=sin(��+2��/3)	sin(2��/3)=	 0.866 �� 376Dh			*/
		AxisRsc->SinTbl.CosT2 = 0xE000;	/* SinTbl.CosT2=cos(��+2��/3)	cos(2��/3)=	-0.500 �� E000h			*/
		AxisRsc->SinTbl.SinT3 = 0xC893;	/* SinTbl.SinT3=sin(��-2��/3)	sin(-2��/3)=-0.866 �� C893h			*/
		AxisRsc->SinTbl.CosT3 = 0xE000;	/* SinTbl.CosT3=cos(��-2��/3)	cos(-2��/3)=-0.500 �� E000h			*/
/*--------------------------------------------------------------------------------------------------*/
/*		PWM set																						*/
/*--------------------------------------------------------------------------------------------------*/
		AxisRsc->SvIpRegW->PWMOS = 0x0A0;						/* 2level,triangle,servo(bit7: no-Saw mode for JL-056)		*/
		AxisRsc->IntAdV.CrFreqW = AxisRsc->IntAdP.CrFreq;		/* Carrier set(IntAdP.CrFreq must be set before starts)		*/
		AxisRsc->SvIpRegW->CRSET1 = 0x10;						/* CLA=Both(unavailable on JL-056)							*/
		AxisRsc->SvIpRegW->CRFRQ = AxisRsc->IntAdV.CrFreqW;		/* Carrier 6kHz												*/
		AxisRsc->IHostWk.uswk = (AxisRsc->IntAdV.CrFreqW >> 1);	/* TMP0 <-- IntAdV.CrFreqW /2(50p duty)						*/
		AxisRsc->SvIpRegW->PwmT2 = AxisRsc->IHostWk.uswk;		/* T2(W) = (duty:50p)										*/
		AxisRsc->SvIpRegW->PwmT1 = AxisRsc->IHostWk.uswk;		/* T1(V) = (duty:50p)										*/
		AxisRsc->SvIpRegW->PwmT0 = AxisRsc->IHostWk.uswk;		/* T0(U) = (duty:50p)										*/
/*--------------------------------------------------------------------------------------------------*/
/*		Clear Register																				*/
/*--------------------------------------------------------------------------------------------------*/
		MpDataClear( AxisRsc );
/*--------------------------------------------------------------------------------------------------*/
/*		input CPORT, DLIM = QLIM = 0, output CPORT													*/
/*--------------------------------------------------------------------------------------------------*/
		AxisRsc->StsFlg.CtrlStsRW = AxisRsc->SvIpRegR->CTSTR;	/* StsFlg.CtrlStsRW <- Control register								*/
		AxisRsc->StsFlg.CtrlStsRW = ( AxisRsc->StsFlg.CtrlStsRW & DLIMI );	/* StsFlg.CtrlStsRW <-- StsFlg.CtrlStsRW & DLIMI (imm_16)						*/
		AxisRsc->SvIpRegW->CTSTW = AxisRsc->StsFlg.CtrlStsRW;	/* Status Set												*/
/*--------------------------------------------------------------------------------------------------*/
/*		START : INTERRUPT, PWM																		*/
/*--------------------------------------------------------------------------------------------------*/
		AxisRsc->SvIpRegW->EIX = 0x0;							/* Interuput start											*/
		AxisRsc->SvIpRegW->CRST = 0x1;							/* Carrier(PWM) start										*/
		AxisRsc->StsFlg.BbSetW = ( AxisRsc->StsFlg.BbSetW & 0xFFFB );	/* Reset soft_BB											*/
		AxisRsc->SvIpRegW->BBSET = AxisRsc->StsFlg.BbSetW;		/* 															*/
	}


/****************************************************************************************************/
/*																									*/
/*		ROUND Procedure																				*/
/*																									*/
/****************************************************************************************************/
#if !defined(WIN32)
	while (1)
#endif
	{
		for( ax_no = 0; ax_no < AxisInfo.AxisNum; ax_no++ )
		{
			AxisRsc = &AxisHdl[ax_no];

/*--------------------------------------------------------------------------------------------------*/
/*		A/D error check and clear																	*/
/*--------------------------------------------------------------------------------------------------*/
			AxisRsc->StsFlg.FccStsMon = AxisRsc->SvIpRegR->FCCST;
			AxisRsc->StsFlg.FltStsW = AxisRsc->SvIpRegR->FLTSTAT;
			AxisRsc->StsFlg.FltStsW = ( AxisRsc->StsFlg.FltStsW & 0x7FFF );

			if ( AxisRsc->StsFlg.FltStsW != 0x0 )
			{
				IxNop( );															/* JL-076�s�No.6�΍� <V214>	*/
			//---------------------------
			// insert error sequence
			//---------------------------
			}
		}

/*--------------------------------------------------------------------------------------------------*/
/*		Host port check for host INT																*/
/*			���݁AWREG100�`WREG104�܂ł͖��g�p�̂��߁A�폜�B										*/
/*--------------------------------------------------------------------------------------------------*/
		/* ��H/W�A�N�Z�X�����ʂ̂��̂��܂Ƃ߂����I�I0���ڂ��ď����̂��i�D������ */
		if ( AxisHdl[0].SvIpRegR->HSUR0 != 0x0 )
		{
			IxNop( );						/* 															*/
			MpIntHost( );				/* 															*/
			IxNop( );															/* JL-076�s�No.6�΍� <V214>	*/
		}

/*--------------------------------------------------------------------------------------------------*/
/*		Host port check for host INT2																*/
/*--------------------------------------------------------------------------------------------------*/
		/* ��H/W�A�N�Z�X�����ʂ̂��̂��܂Ƃ߂����I�I0���ڂ��ď����̂��i�D������ */
		if ( AxisHdl[0].SvIpRegR->HSUR1 != 0x0 )
		{
			AxisHdl[0].SvIpRegW->DIX = 0x0;		/* disable interupt		<V112>								*/

			for( ax_no = 0; ax_no < AxisInfo.AxisNum; ax_no++ )
			{
				AxisRsc = &AxisHdl[ax_no];

				AxisRsc->PhaseV.PhaseH = AxisRsc->AdinV.PhaseHIn;		/* 															*/
				AxisRsc->PhaseV.PhaseIp = AxisRsc->PhaseV.PhaseIpIn;	/* �ʑ���ԗ�			<V112>								*/
				AxisRsc->PhaseV.PhaseIpF = AxisRsc->PhaseV.PhaseIpFIn;	/* �ʑ���ԃt���O		<V112>						*/
				AxisRsc->PhaseV.PhaseIpFIn = 1;	/* �ʑ���ԃt���O�Z�b�g	<V112>								*/
				AxisRsc->WeakFV.WfKpV.s[0] = AxisRsc->WeakFV.WfKpVLIn;	/* �d��FB���Q�C��(����16bit)	<V214>					*/
				AxisRsc->WeakFV.WfKpV.s[1] = AxisRsc->WeakFV.WfKpVHIn;	/* �d��FB���Q�C��(���16bit)	<V214>				*/
				AxisRsc->WeakFV.WfKiV.s[0] = AxisRsc->WeakFV.WfKiVLIn;	/* �d��FB�ϕ��Q�C��(����16bit)	<V214>					*/
				AxisRsc->WeakFV.WfKiV.s[1] = AxisRsc->WeakFV.WfKiVHIn;	/* �d��FB�ϕ��Q�C��(���16bit)	<V214>				*/
				AxisRsc->WeakFV.WfV1Max = AxisRsc->WeakFV.WfV1MaxIn;	/* �d���w�ߐ����l				<V214>					*/
				AxisRsc->WeakFV.WfIdRefLim = AxisRsc->WeakFV.WfIdRefLimIn;	/* d���d���w�߃��~�b�g			<V214>			*/
			}

			AxisHdl[0].SvIpRegW->EIX = 0x0;		/* enable interupt		<V112>								*/
		}
	}
}



/****************************************************************************************************/
/*																									*/
/*		HOST Interupt Procedure																		*/
/*																									*/
/****************************************************************************************************/
void	MpIntHost( void )
{
#ifdef	WIN32
	DWREG lmtBuf;			/* �������Z�p���~�b�g���f�p�o�b�t�@				*/
	UCHAR lmtBufsign[2];	/* ���~�b�g�o�b�t�@���͒l����		0:�O���A1:�㍀	*/
	UCHAR lmtBufSw;			/* ���~�b�g�o�b�t�@���͒l�X�C�b�`	0:�O���A1:�㍀	*/
#endif
	USHORT				ax_no;
	USHORT				ActiveAxis;
	MICRO_AXIS_HANDLE	*AxisRsc;

	/* ��H/W�A�N�Z�X�����ʂ̂��̂��܂Ƃ߂����I�I0���ڂ��ď����̂��i�D������ */
	AxisHdl[0].SvIpRegW->WDT1L = 0x1;		/* Watch dog set											*/
	AxisHdl[0].SvIpRegW->OUTPT = 0x1;		/* 1.13														*/

	for( ax_no = 0; ax_no < AxisInfo.AxisNum; ax_no++ )
	{
		AxisRsc = &AxisHdl[ax_no];

		AxisRsc->IntAdV.IqMon = AxisRsc->IntAdV.IqRef;	/* for CPU monitor											*/
/*--------------------------------------------------------------------------------------------------*/
/*		�L�����A���g���؂�ւ�����												<V057>	<V075>		*/
/*--------------------------------------------------------------------------------------------------*/
		if ( AxisRsc->IntAdP.CrFreq != AxisRsc->IntAdV.CrFreqW )
		{
			AxisRsc->IntAdV.CrFreqW = AxisRsc->IntAdP.CrFreq;		/* Carrier Buffer Change									*/
			AxisRsc->SvIpRegW->CRFRQ = AxisRsc->IntAdV.CrFreqW;		/* Carrier Freq. Change										*/
		}
	}

/*--------------------------------------------------------------------------------------------------*/
/*		input from host																				*/
/*--------------------------------------------------------------------------------------------------*/
//		swk0 = CTSTR;	/* HTMP5 <-- CTSTR										*/

	/* Check Current Ajust Request */
	ActiveAxis = 0;
	for( ax_no = 0; ax_no < AxisInfo.AxisNum; ax_no++ )
	{
		AxisRsc = &AxisHdl[ax_no];
		if ( ( AxisRsc->SvIpRegR->CTSTR & RLOCK ) == 0 )
		{
			ActiveAxis |= 0x01 << ax_no;	/* �r�b�g�o�^ */
		}
	}

	if( ActiveAxis != 0 )
	{ /* �d�����o�����v������ */
		IxNop( );						/* 															*/
		/* ��H/W�A�N�Z�X�����ʂ̂��̂��܂Ƃ߂����I�I0���ڂ��ď����̂��i�D������ */
		AxisHdl[0].SvIpRegW->DIX = 0x0;		/* disable interupt		<V112>								*/

		for( ax_no = 0; ax_no < AxisInfo.AxisNum; ax_no++ )
		{
			AxisRsc = &AxisHdl[ax_no];

			if( 0 != (ActiveAxis & (0x01 << ax_no)) )
			{
				AxisRsc->IntAdV.IuOffset = AxisRsc->AdinV.IuOffsetIn;	/* IntAdV.IuOffset <-- AdinV.IuOffsetIn											*/
				AxisRsc->IntAdV.IvOffset = AxisRsc->AdinV.IvOffsetIn;	/* IntAdV.IvOffset <-- AdinV.IvOffsetIn											*/
				AxisRsc->IntAdP.Kcu = AxisRsc->AdinV.KcuIn;				/* IntAdP.Kcu <-- AdinV.KcuIn											*/
				AxisRsc->IntAdP.Kcv = AxisRsc->AdinV.KcvIn;				/* IntAdP.Kcv <-- AdinV.KcvIn											*/
			}
		}

		/* ��H/W�A�N�Z�X�����ʂ̂��̂��܂Ƃ߂����I�I0���ڂ��ď����̂��i�D������ */
		AxisHdl[0].SvIpRegW->EIX = 0x0;		/* enable interupt		<V112>								*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		interupt  enable																			*/
/*--------------------------------------------------------------------------------------------------*/
	/* ��H/W�A�N�Z�X�����ʂ̂��̂��܂Ƃ߂����I�I0���ڂ��ď����̂��i�D������ */
	AxisHdl[0].SvIpRegW->OUTPT = 0x0;		/* <H>														*/

	/* ��H/W�A�N�Z�X�����ʂ̂��̂��܂Ƃ߂����I�I0���ڂ��ď����̂��i�D������ */
	AxisHdl[0].SvIpRegW->DIX = 0x0;		/* disable interupt		<V112>								*/

	for( ax_no = 0; ax_no < AxisInfo.AxisNum; ax_no++ )
	{
		AxisRsc = &AxisHdl[ax_no];
		AxisRsc->PhaseV.PhaseH = AxisRsc->AdinV.PhaseHIn;		/* 															*/
		AxisRsc->PhaseV.PhaseIp = AxisRsc->PhaseV.PhaseIpIn;	/* �ʑ���ԗ�			<V112>								*/
		AxisRsc->PhaseV.PhaseIpF = AxisRsc->PhaseV.PhaseIpFIn;	/* �ʑ���ԃt���O		<V112>						*/
		AxisRsc->PhaseV.PhaseIpFIn = 1;							/* �ʑ���ԃt���O�Z�b�g	<V112>								*/
		AxisRsc->WeakFV.Vel = AxisRsc->AdinV.VelIn;				/* 															*/
		AxisRsc->VcmpV.VdRef = AxisRsc->AdinV.VdRefIn;			/* 															*/
		AxisRsc->VcmpV.VqRef = AxisRsc->AdinV.VqRefIn;			/* 															*/
		AxisRsc->IntAdV.TLimP = AxisRsc->AdinV.TLimPIn;			/* 															*/
		AxisRsc->IntAdV.TLimM = AxisRsc->AdinV.TLimMIn;			/* 															*/
		AxisRsc->IntAdP.Kvv = AxisRsc->IntAdP.KvvIn;			/* for AVR													*/
		AxisRsc->IntAdV.IqDist = AxisRsc->IntAdV.IqDistIn;		/* <V224>												*/
		AxisRsc->WeakFV.WfKpV.s[0] = AxisRsc->WeakFV.WfKpVLIn;	/* �d��FB���Q�C��(����16bit)	<V214>					*/
		AxisRsc->WeakFV.WfKpV.s[1] = AxisRsc->WeakFV.WfKpVHIn;	/* �d��FB���Q�C��(���16bit)	<V214>				*/
		AxisRsc->WeakFV.WfKiV.s[0] = AxisRsc->WeakFV.WfKiVLIn;	/* �d��FB�ϕ��Q�C��(����16bit)	<V214>					*/
		AxisRsc->WeakFV.WfKiV.s[1] = AxisRsc->WeakFV.WfKiVHIn;	/* �d��FB�ϕ��Q�C��(���16bit)	<V214>				*/
		AxisRsc->WeakFV.WfV1Max = AxisRsc->WeakFV.WfV1MaxIn;	/* �d���w�ߐ����l				<V214>					*/
		AxisRsc->WeakFV.WfIdRefLim = AxisRsc->WeakFV.WfIdRefLimIn;	/* d���d���w�߃��~�b�g			<V214>			*/
	}

	/* ��H/W�A�N�Z�X�����ʂ̂��̂��܂Ƃ߂����I�I0���ڂ��ď����̂��i�D������ */
	AxisHdl[0].SvIpRegW->EIX = 0x0;		/* enable interupt		<V112>								*/

/*--------------------------------------------------------------------------------------------------*/
/*		Carrier Freq Change check : if( status & BB ) Carrier Freq. change							*/
/*--------------------------------------------------------------------------------------------------*/
	/* Check Current Ajust Request */
	ActiveAxis = 0;
	for( ax_no = 0; ax_no < AxisInfo.AxisNum; ax_no++ )
	{
		AxisRsc = &AxisHdl[ax_no];
		if ( AxisRsc->IntAdP.FccRst != 0)
		{
			ActiveAxis |= 0x01 << ax_no;	/* �r�b�g�o�^ */
		}
	}

	if( ActiveAxis != 0 )
	{ /* �d�����o�����v������ */
		for( ax_no = 0; ax_no < AxisInfo.AxisNum; ax_no++ )
		{
			AxisRsc = &AxisHdl[ax_no];

			if( 0 != (ActiveAxis & (0x01 << ax_no)) )
			{
				/* �s�No.15��076A�̕s��̂��ߑ΍�͏ȗ��\<002>(tanaka21)*/
				AxisRsc->SvIpRegW->SDMECLR = ( AxisRsc->SvIpRegR->FCCST | 8 );
				AxisRsc->AdStop.ADRst = AxisRsc->IntAdP.FccRst;
				AxisRsc->IntAdP.FccRst = 0;
			}
		}
		AxisRsc->SvIpRegW->ADSYNC = 1;
	}

/*--------------------------------------------------------------------------------------------------*/
//		swk0 = CTSTR;	/* HTMP5 <-- control register							*/
	/* Check BB Status */
	ActiveAxis = 0;
	for( ax_no = 0; ax_no < AxisInfo.AxisNum; ax_no++ )
	{
		AxisRsc = &AxisHdl[ax_no];
		if ( AxisRsc->SvIpRegR->CTSTR & BB )
		{
			ActiveAxis |= 0x01 << ax_no;	/* �r�b�g�o�^ */
		}
	}

	if( ActiveAxis != 0 )
	{ /* BB��Ԃ̎�������ꍇ */
		/* ��H/W�A�N�Z�X�����ʂ̂��̂��܂Ƃ߂����I�I0���ڂ��ď����̂��i�D������ */
		AxisHdl[0].SvIpRegW->DIX = 0x0;		/* disable interupt		<V112>								*/

		for( ax_no = 0; ax_no < AxisInfo.AxisNum; ax_no++ )
		{
			AxisRsc = &AxisHdl[ax_no];
/*--------------------------------------------------------------------------------------------------*/
/*		data clear while BB																			*/
/*--------------------------------------------------------------------------------------------------*/
			if( 0 != (ActiveAxis & (0x01 << ax_no)) )
			{ /* BB���̎��̏ꍇ */
				MpDataClear( AxisRsc );

				if( AxisRsc->IntAdP.CrFreq == AxisRsc->IntAdV.CrFreqW )
				{
					AxisRsc->IntAdV.CrFreqW = AxisRsc->IntAdP.CrFreq;		/* Carrier Buffer Change									*/
					AxisRsc->SvIpRegW->CRFRQ = AxisRsc->IntAdV.CrFreqW;		/* Carrier Freq. Change										*/
				}
			}
		}

		/* ��H/W�A�N�Z�X�����ʂ̂��̂��܂Ƃ߂����I�I0���ڂ��ď����̂��i�D������ */
		AxisHdl[0].SvIpRegW->EIX = 0x0;		/* enable interupt		<V112>								*/
	}

	for( ax_no = 0; ax_no < AxisInfo.AxisNum; ax_no++ )
	{
		if( 0 == (ActiveAxis & (0x01 << ax_no)) )
		{ /* BB���ł͂Ȃ����̏ꍇ */
			AxisRsc = &AxisHdl[ax_no];

/****************************************************************************************************/
/* 		notch filter 1st (before 2nd filter)														*/
/****************************************************************************************************/
/*		input		: AdinV.IqIn  (max:15000) 															*/
/*		output		: IntAdV.IqOut1L (max:15000,limit:32768)													*/
/*		parameter	: IntAdP.Kf11,IntAdP.Kf12,IntAdP.Kf13,IntAdP.Kf14 (KFx= Kfx * 8192)												*/
/*		buffer		: IntAdV.IqIn1PL,IntAdV.IqIn1PPL,IntAdV.IqOut1PL,IntAdV.IqOut1PPL														*/
/****************************************************************************************************/
			if( AxisRsc->IntAdP.CtrlSw & F1DSABL )			/* Notch filter1 Disable		*/
			{
				AxisRsc->IntAdV.IqOut1L.s[0] = AxisRsc->AdinV.IqIn;	/* �t�B���^�����Ȃ�				*/
			}
			else
			{
/*--------------------------------------------------------------------------------------------------*/
/*		IHostWk.lwk1 = IntAdP.Kf12 * AdinV.IqIn + IntAdP.Kf11 * IntAdV.IqIn1PL + IntAdP.Kf14 * IntAdV.IqIn1PPL										*/
/*--------------------------------------------------------------------------------------------------*/
#ifdef	WIN32		/* �������Z���~�b�g���ʗp����(VC�p)		*/
				IxADDSUBLMTCHKRDY( (LONG)AxisRsc->IntAdP.Kf12 * (LONG)AxisRsc->AdinV.IqIn, (LONG)AxisRsc->IntAdP.Kf11 * AxisRsc->IntAdV.IqIn1PL.l );
				AxisRsc->IHostWk.lwk1 = ((LONG)AxisRsc->IntAdP.Kf12 * (LONG)AxisRsc->AdinV.IqIn) + ((LONG)AxisRsc->IntAdP.Kf11 * AxisRsc->IntAdV.IqIn1PL.l);
				IxADDLMTCHK( AxisRsc->IHostWk.lwk1 )

				IxADDSUBLMTCHKRDY( AxisRsc->IHostWk.lwk1, (LONG)AxisRsc->IntAdP.Kf14 * AxisRsc->IntAdV.IqIn1PPL.l );
				AxisRsc->IHostWk.lwk1 = AxisRsc->IHostWk.lwk1 + ((LONG)AxisRsc->IntAdP.Kf14 * AxisRsc->IntAdV.IqIn1PPL.l);
				IxADDLMTCHK( AxisRsc->IHostWk.lwk1 )
#else
				AxisRsc->IHostWk.lwk1 = (((LONG)AxisRsc->IntAdP.Kf12 * (LONG)AxisRsc->AdinV.IqIn)
										+ ((LONG)AxisRsc->IntAdP.Kf11 * AxisRsc->IntAdV.IqIn1PL.l)
										+ ((LONG)AxisRsc->IntAdP.Kf14 * AxisRsc->IntAdV.IqIn1PPL.l));
#endif
				AxisRsc->IHostWk.lwk1 = IxLmtCBS32( AxisRsc->IHostWk.lwk1 );			/* �����t32bit��������							*/

/*--------------------------------------------------------------------------------------------------*/
/*		IHostWk.lwk1  = IHostWk.lwk1 - (IntAdP.Kf11 * IntAdV.IqOut1PL + IntAdP.Kf13 * IntAdV.IqOut1PPL)											*/
/*--------------------------------------------------------------------------------------------------*/
//#ifdef WIN32
//				AxisRsc->IHostWk.lwk2 = (LONG)(( (INT64)AxisRsc->IntAdP.Kf11 * (INT64)AxisRsc->IntAdV.IqOut1PL.l ) >> 13);	/* �Q�C����Z�㐮����	 				*/
//#elif defined(ASIP_CC)
//				AxisRsc->IHostWk.lwk2 = asr( AxisRsc->IntAdP.Kf11 * AxisRsc->IntAdV.IqOut1PL.l, 13);	/* �Q�C����Z�㐮����	 				*/
//#endif
				AxisRsc->IHostWk.lwk2 = (LONG)IlibASR64(( (INT64)AxisRsc->IntAdP.Kf11 * (INT64)AxisRsc->IntAdV.IqOut1PL.l ) , 13);	/* �Q�C����Z�㐮����	 				*/
				AxisRsc->IHostWk.lwk2 = IxLmtCBS32( AxisRsc->IHostWk.lwk2 );			/* <V502> �ǉ�									*/

//#ifdef WIN32
//				AxisRsc->IHostWk.lwk3 = (LONG)(( (INT64)AxisRsc->IntAdP.Kf13 * (INT64)AxisRsc->IntAdV.IqOut1PPL.l ) >> 13);	/* �Q�C����Z�㐮����	 				*/
//#elif defined(ASIP_CC)
//				AxisRsc->IHostWk.lwk3 = asr( AxisRsc->IntAdP.Kf13 * AxisRsc->IntAdV.IqOut1PPL.l, 13);	/* �Q�C����Z�㐮����	 				*/
//#endif
				AxisRsc->IHostWk.lwk3 = (LONG)IlibASR64(( (INT64)AxisRsc->IntAdP.Kf13 * (INT64)AxisRsc->IntAdV.IqOut1PPL.l ) , 13);	/* �Q�C����Z�㐮����	 				*/
				AxisRsc->IHostWk.lwk3 = IxLmtCBS32( AxisRsc->IHostWk.lwk3 );			/* <V502> �ǉ�											*/

				AxisRsc->IHostWk.lwk1 = AxisRsc->IHostWk.lwk1 - AxisRsc->IHostWk.lwk2 - AxisRsc->IHostWk.lwk3;

/*--------------------------------------------------------------------------------------------------*/
/*		IntAdV.IqIn1PPL = IntAdV.IqIn1PL, IntAdV.IqIn1PL = AdinV.IqIn, IntAdV.IqOut1PPL = IntAdV.IqOut1PL, IntAdV.IqOut1PL = IHostWk.lwk1					*/
/*--------------------------------------------------------------------------------------------------*/
				AxisRsc->IntAdV.IqIn1PPL.l = AxisRsc->IntAdV.IqIn1PL.l;	/* <V388> �ǉ�											*/
				AxisRsc->IntAdV.IqIn1PL.l = (LONG)AxisRsc->AdinV.IqIn;	/* <V388> �ǉ�												*/
				AxisRsc->IntAdV.IqOut1PPL.l = AxisRsc->IntAdV.IqOut1PL.l;	/* <V388> �ǉ�										*/
				AxisRsc->IntAdV.IqOut1PL.l = AxisRsc->IHostWk.lwk1;	/* <V388> �ǉ�											*/
				AxisRsc->IntAdV.IqOut1BufL.l = AxisRsc->IHostWk.lwk1;	/*				<V502> �ǉ�							*/

				AxisRsc->IntAdV.IqOut1L.l = AxisRsc->IntAdV.IqOut1BufL.l >> 13;
				AxisRsc->IntAdV.IqOut1L.s[0] = IxLmtCBS16( AxisRsc->IntAdV.IqOut1L.s[0] );	/*				<V502> �ǉ�				*/

			}
/****************************************************************************************************/
/*		notch filter																				*/
/****************************************************************************************************/
/*		input		: IntAdV.IqOut1L (max:15000) 															*/
/*		output		: IntAdV.IqOut3L (max:15000,limit:32768)													*/
/*		parameter	: IntAdP.Kf31,IntAdP.Kf32,IntAdP.Kf33,IntAdP.Kf34 (KF3x= Kf3x * 8192)										*/
/*		buffer		: IQI3P,IQI3PP,IQO3P,IQO3PP														*/
/****************************************************************************************************/
			if( AxisRsc->IntAdP.CtrlSw & F3DSABL )
			{
				AxisRsc->IntAdV.IqOut3L.s[0] = AxisRsc->IntAdV.IqOut1L.s[0];	/* �t�B���^�����Ȃ�			*/
			}
			else
			{
/*--------------------------------------------------------------------------------------------------*/
/*		HTMP0 = IntAdP.Kf32 * IntAdV.IqOut1L + IntAdP.Kf31 * IQI3P + IntAdP.Kf34 * IQI3PP											*/
/*--------------------------------------------------------------------------------------------------*/
#ifdef	WIN32
				IxADDSUBLMTCHKRDY( (LONG)AxisRsc->IntAdP.Kf32 * (LONG)AxisRsc->IntAdV.IqOut1L.s[0], (LONG)AxisRsc->IntAdP.Kf31 * AxisRsc->IntAdV.IqIn3PL.l );
				AxisRsc->IHostWk.lwk1 = (LONG)AxisRsc->IntAdP.Kf32 * (LONG)AxisRsc->IntAdV.IqOut1L.s[0]
								+ (LONG)AxisRsc->IntAdP.Kf31 * AxisRsc->IntAdV.IqIn3PL.l;
				IxADDLMTCHK( AxisRsc->IHostWk.lwk1 );
				
				IxADDSUBLMTCHKRDY( AxisRsc->IHostWk.lwk1, (LONG)AxisRsc->IntAdP.Kf34 + AxisRsc->IntAdV.IqIn3PPL.l );
				AxisRsc->IHostWk.lwk1 = AxisRsc->IHostWk.lwk1
								+ (LONG)AxisRsc->IntAdP.Kf34 + AxisRsc->IntAdV.IqIn3PPL.l;
				IxADDLMTCHK( AxisRsc->IHostWk.lwk1 );
#else
				AxisRsc->IHostWk.lwk1 = (((LONG)AxisRsc->IntAdP.Kf32 * (LONG)AxisRsc->IntAdV.IqOut1L.s[0])
										+ ((LONG)AxisRsc->IntAdP.Kf31 * AxisRsc->IntAdV.IqIn3PL.l)
										+ ((LONG)AxisRsc->IntAdP.Kf34 + AxisRsc->IntAdV.IqIn3PPL.l));
#endif
				AxisRsc->IHostWk.lwk1 = IxLmtCBS32( AxisRsc->IHostWk.lwk1 );				/* 32bit����										*/

/*--------------------------------------------------------------------------------------------------*/
/*		HTMP0  = HTMP0 - (IntAdP.Kf31 * IQO3P + IntAdP.Kf33 * IQO3PP)												*/
/*--------------------------------------------------------------------------------------------------*/
//#ifdef WIN32
//				AxisRsc->IHostWk.lwk2 = (LONG)(( (INT64)AxisRsc->IntAdP.Kf31 * (INT64)AxisRsc->IntAdV.IqOut3PL.l ) >> 13);	/* �Q�C����Z�㐮����					*/
//#elif defined(ASIP_CC)
//				AxisRsc->IHostWk.lwk2 = asr( AxisRsc->IntAdP.Kf31 * AxisRsc->IntAdV.IqOut3PL.l, 13);	/* �Q�C����Z�㐮����	 				*/
//#endif
				AxisRsc->IHostWk.lwk2 = (LONG)IlibASR64(( (INT64)AxisRsc->IntAdP.Kf31 * (INT64)AxisRsc->IntAdV.IqOut3PL.l ) , 13);	/* �Q�C����Z�㐮����					*/
				AxisRsc->IHostWk.lwk2 = IxLmtCBS32( AxisRsc->IHostWk.lwk2 );					/* �����ӂ�m�F							*/

//#ifdef WIN32
//				AxisRsc->IHostWk.lwk3 = (LONG)(( (INT64)AxisRsc->IntAdP.Kf33 * (INT64)AxisRsc->IntAdV.IqOut3PPL.l ) >> 13);	/* �Q�C����Z�㐮����					*/
//#elif defined(ASIP_CC)
//				AxisRsc->IHostWk.lwk3 = asr( AxisRsc->IntAdP.Kf33 * AxisRsc->IntAdV.IqOut3PPL.l, 13);	/* �Q�C����Z�㐮����	 				*/
//#endif
				AxisRsc->IHostWk.lwk3 = (LONG)IlibASR64(( (INT64)AxisRsc->IntAdP.Kf33 * (INT64)AxisRsc->IntAdV.IqOut3PPL.l ) , 13);	/* �Q�C����Z�㐮����					*/
				AxisRsc->IHostWk.lwk3 = IxLmtCBS32( AxisRsc->IHostWk.lwk3 );					/* �����ӂ�m�F							*/

				AxisRsc->IHostWk.lwk1 = AxisRsc->IHostWk.lwk1 - AxisRsc->IHostWk.lwk2 - AxisRsc->IHostWk.lwk3;

/*--------------------------------------------------------------------------------------------------*/
/*		IQI3PP = IQI3P, IQI3P = IQO1, IQO3PP = IQO3P, IQO3P = HTMP0									*/
/*--------------------------------------------------------------------------------------------------*/
				AxisRsc->IntAdV.IqIn3PPL.l = AxisRsc->IntAdV.IqIn3PL.l;		/* �O�X��l�ۑ�										*/
				AxisRsc->IntAdV.IqIn3PL.l = (LONG)AxisRsc->IntAdV.IqOut1L.s[0];	/* �O��l�ۑ�										*/
				AxisRsc->IntAdV.IqOut3PPL.l = AxisRsc->IntAdV.IqOut3PL.l;		/* �O�X��l�ۑ�										*/
				AxisRsc->IntAdV.IqOut3PL.l = AxisRsc->IHostWk.lwk1;				/* �O��l�ۑ�										*/
				AxisRsc->IntAdV.IqOut3BufL.l = AxisRsc->IHostWk.lwk1;			/* �������O�o�͍���l�ۑ�							*/

				AxisRsc->IntAdV.IqOut3L.l = AxisRsc->IHostWk.lwk1 >> 13;			/* �o�͒l�̐�����*/
				AxisRsc->IntAdV.IqOut3L.s[0] = IxLmtCBS16( AxisRsc->IntAdV.IqOut3L.s[0] );	/*				<V502> �ǉ�				*/

			}
/****************************************************************************************************/
/*		Low Pass Filter																				*/
/****************************************************************************************************/
/*		IntAdP.TLpf   : Time-constant																		*/
/*		IntAdV.IqOut1Lpf : Output(32 bit) .. IQO1F: High 16 bit												*/
/*		IntAdV.IqOut3   : INPUT																				*/
/****************************************************************************************************/
			if( AxisRsc->IntAdP.CtrlSw & LPFDSABL )
			{
				AxisRsc->IntAdV.IqOut1Lpf.s[1] = AxisRsc->IntAdV.IqOut3L.s[0];	/* �t�B���^�����Ȃ�		*/
			}
			else
			{
				AxisRsc->IntAdV.IqOut3 = AxisRsc->IntAdV.IqOut3L.s[0];	/* �t�B���^�����Ȃ�		*/

#ifdef	WIN32
				IxADDSUBLMTCHKRDY( AxisRsc->IntAdV.IqOut3, AxisRsc->IntAdV.IqOut1Lpf.s[1] );
#endif
				AxisRsc->IHostWk.swk1 = AxisRsc->IntAdV.IqOut3 - AxisRsc->IntAdV.IqOut1Lpf.s[1];	/* HTMP0 <-- IntAdV.IqOut3 - IQO1FH						*/
#ifdef	WIN32
				IxSUBLMTCHK( AxisRsc->IHostWk.swk1 );
#endif
				AxisRsc->IHostWk.swk1 = IxLmtCBS16( AxisRsc->IHostWk.swk1 );	/* HTMP0 <-- limit(HTMP0, 2^15 - 1)						*/

				AxisRsc->IHostWk.lwk2 = ((LONG)AxisRsc->IntAdP.TLpf * (LONG)AxisRsc->IHostWk.swk1) << 2;

#ifdef	WIN32
				IxADDSUBLMTCHKRDY( AxisRsc->IHostWk.lwk2, AxisRsc->IntAdV.IqOut1Lpf.l );
#endif
				AxisRsc->IHostWk.lwk2 = AxisRsc->IHostWk.lwk2 + AxisRsc->IntAdV.IqOut1Lpf.l;
#ifdef	WIN32
				IxADDLMTCHK( AxisRsc->IHostWk.lwk2 );
#endif
				AxisRsc->IntAdV.IqOut1Lpf.l = IxLmtCBS32( AxisRsc->IHostWk.lwk2 );	/* HTMP0 <-- limit(HTMP0, 2^15 - 1)						*/
			}
/****************************************************************************************************/
/* 		notch filter (before data input)															*/
/****************************************************************************************************/
/*		input		: IQO1F (max:15000) 															*/
/*		output		: IntAdV.IqOut2L (max:15000,limit:32768)													*/
/*		parameter	: IntAdP.Kf21,IntAdP.Kf22,IntAdP.Kf23,IntAdP.Kf24 (KF2x= Kf2x * 8192)										*/
/*		buffer		: IQI2P,IQI2PP,IQO2P,IQO2PP														*/
/****************************************************************************************************/
			if( AxisRsc->IntAdP.CtrlSw & F2DSABL )
			{
				AxisRsc->IntAdV.IqOut2L.s[0] = AxisRsc->IntAdV.IqOut1Lpf.s[1];	/* <V388> �ǉ�										*/
			}
			else
			{
/*--------------------------------------------------------------------------------------------------*/
/*		HTMP0 = IntAdP.Kf22 * IQO1F + IntAdP.Kf21 * IQI2P + IntAdP.Kf24 * IQI2PP											*/
/*--------------------------------------------------------------------------------------------------*/
#ifdef	WIN32
				IxADDSUBLMTCHKRDY( (LONG)AxisRsc->IntAdP.Kf22 * (LONG)AxisRsc->IntAdV.IqOut1Lpf.s[1], (LONG)AxisRsc->IntAdP.Kf21 * AxisRsc->IntAdV.IqOut2PL.l );
				AxisRsc->IHostWk.lwk1 = (LONG)AxisRsc->IntAdP.Kf22 * (LONG)AxisRsc->IntAdV.IqOut1Lpf.s[1]
								+ (LONG)AxisRsc->IntAdP.Kf21 * AxisRsc->IntAdV.IqOut2PL.l;
				IxADDLMTCHK( AxisRsc->IHostWk.lwk1 );
				
				IxADDSUBLMTCHKRDY( AxisRsc->IHostWk.lwk1, (LONG)AxisRsc->IntAdP.Kf34 + AxisRsc->IntAdV.IqIn2PPL.l );
				AxisRsc->IHostWk.lwk1 = AxisRsc->IHostWk.lwk1
								+ (LONG)AxisRsc->IntAdP.Kf34 + AxisRsc->IntAdV.IqIn2PPL.l;
				IxADDLMTCHK( AxisRsc->IHostWk.lwk1 );
#else
				AxisRsc->IHostWk.lwk1 = (((LONG)AxisRsc->IntAdP.Kf22 * (LONG)AxisRsc->IntAdV.IqOut1Lpf.s[1])
										+ ((LONG)AxisRsc->IntAdP.Kf21 * AxisRsc->IntAdV.IqOut2PL.l)
										+ ((LONG)AxisRsc->IntAdP.Kf34 + AxisRsc->IntAdV.IqIn2PPL.l));
#endif
				AxisRsc->IHostWk.lwk1 = IxLmtCBS32( AxisRsc->IHostWk.lwk1 );				/* 32bit����										*/

/*--------------------------------------------------------------------------------------------------*/
/*		HTMP0  = HTMP0 - (IntAdP.Kf21 * IQOP + IntAdP.Kf23 * IQOPH)												*/
/*--------------------------------------------------------------------------------------------------*/
//#ifdef WIN32
//				AxisRsc->IHostWk.lwk2 = (LONG)(( (INT64)AxisRsc->IntAdP.Kf21 * (INT64)AxisRsc->IntAdV.IqOut2PL.l ) >> 13);	/* �Q�C����Z�㐮����					*/
//#elif defined(ASIP_CC)
//				AxisRsc->IHostWk.lwk2 = asr( AxisRsc->IntAdP.Kf21 * AxisRsc->IntAdV.IqOut2PL.l, 13);	/* �Q�C����Z�㐮����	 				*/
//#endif
				AxisRsc->IHostWk.lwk2 = (LONG)IlibASR64(( (INT64)AxisRsc->IntAdP.Kf21 * (INT64)AxisRsc->IntAdV.IqOut2PL.l ) , 13);	/* �Q�C����Z�㐮����					*/
				AxisRsc->IHostWk.lwk2 = IxLmtCBS32( AxisRsc->IHostWk.lwk2 );					/* �����ӂ�m�F							*/

//#ifdef WIN32
//				AxisRsc->IHostWk.lwk3 = (LONG)(( (INT64)AxisRsc->IntAdP.Kf23 * (INT64)AxisRsc->IntAdV.IqOut2PPL.l ) >> 13);	/* �Q�C����Z�㐮����					*/
//#elif defined(ASIP_CC)
//				AxisRsc->IHostWk.lwk3 = asr( AxisRsc->IntAdP.Kf23 * AxisRsc->IntAdV.IqOut2PPL.l, 13);	/* �Q�C����Z�㐮����	 				*/
//#endif
				AxisRsc->IHostWk.lwk3 = (LONG)IlibASR64(( (INT64)AxisRsc->IntAdP.Kf23 * (INT64)AxisRsc->IntAdV.IqOut2PPL.l ) , 13);	/* �Q�C����Z�㐮����					*/
				AxisRsc->IHostWk.lwk3 = IxLmtCBS32( AxisRsc->IHostWk.lwk3 );					/* �����ӂ�m�F							*/

				AxisRsc->IHostWk.lwk1 = AxisRsc->IHostWk.lwk1 - AxisRsc->IHostWk.lwk2 - AxisRsc->IHostWk.lwk3;

/*--------------------------------------------------------------------------------------------------*/
/*		IQI2PP = IQI2P, IQI2P = IQO1F, IQO2PP = IQO2P, IQO2P = HTMP0								*/
/*--------------------------------------------------------------------------------------------------*/
				AxisRsc->IntAdV.IqIn2PPL.l = AxisRsc->IntAdV.IqIn2PL.l;		/* �O�X��l�ۑ�										*/
				AxisRsc->IntAdV.IqIn2PL.l = (LONG)AxisRsc->IntAdV.IqOut1Lpf.s[0];	/* �O��l�ۑ�										*/
				AxisRsc->IntAdV.IqOut2PPL.l = AxisRsc->IntAdV.IqOut2PL.l;		/* �O�X��l�ۑ�										*/
				AxisRsc->IntAdV.IqOut2PL.l = AxisRsc->IHostWk.lwk1;				/* �O��l�ۑ�										*/
				AxisRsc->IntAdV.IqOut2BufL.l = AxisRsc->IHostWk.lwk1;			/* �������O�o�͍���l�ۑ�							*/

				AxisRsc->IntAdV.IqOut2L.l = AxisRsc->IHostWk.lwk1 >> 13;			/* �o�͒l�̐�����*/
				AxisRsc->IntAdV.IqOut2L.s[0] = IxLmtCBS16( AxisRsc->IntAdV.IqOut2L.s[0] );	/*				<V502> �ǉ�				*/

			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		omega calculation																			*/
/*--------------------------------------------------------------------------------------------------*/
		AxisRsc->IHostWk.swk0 = (SHORT)((( (LONG)AxisRsc->IntAdP.Ld * (LONG)AxisRsc->WeakFV.Vel) >> 15) * AxisRsc->IntAdV.KEangle);
		AxisRsc->IHostWk.swk0 = IxLmtCBS16( AxisRsc->IHostWk.swk0 );

		AxisRsc->IHostWk.swk1 = (SHORT)((( (LONG)AxisRsc->IntAdP.Lq * (LONG)AxisRsc->WeakFV.Vel) >> 15) * AxisRsc->IntAdV.KEangle);
		AxisRsc->IHostWk.swk1 = IxLmtCBS16( AxisRsc->IHostWk.swk1 );
	}


/*--------------------------------------------------------------------------------------------------*/
/*		data transmit(2)																			*/
/*--------------------------------------------------------------------------------------------------*/
	/* ��H/W�A�N�Z�X�����ʂ̂��̂��܂Ƃ߂����I�I0���ڂ��ď����̂��i�D������ */
	AxisHdl[0].SvIpRegW->DIX = 0x0;		/* disable interupt		<V112>								*/

	for( ax_no = 0; ax_no < AxisInfo.AxisNum; ax_no++ )
	{
		AxisRsc = &AxisHdl[ax_no];

		AxisRsc->VcmpV.MagC = (SHORT)(( (LONG)AxisRsc->IntAdP.Mag * (LONG)AxisRsc->WeakFV.Vel) >> 15);		/* VcmpV.MagC <-- ACC >> 15										*/
		AxisRsc->VcmpV.LdC = AxisRsc->IHostWk.swk0;	/* VcmpV.LdC														*/
		AxisRsc->VcmpV.LqC = AxisRsc->IHostWk.swk1;	/* VcmpV.LqC														*/
/*--------------------------------------------------------------------------------------------------*/
		AxisRsc->WeakFV.IqOut = AxisRsc->IntAdV.IqOut2L.s[0];	/* <V388> �ǉ�												*/
//		swk0 = IntAdP.CtrlSw;	/* 															*/
		
		if( (AxisRsc->IntAdP.CtrlSw & V_FB) == 0 )
		{
			AxisRsc->WeakFV.IdOut = AxisRsc->AdinV.IdIn;		/* WeakFV.IdOut(reference)											*/
		}

/* �����p���X��H/W���\�� */
/*--------------------------------------------------------------------------------------------------*/
/*		�����p���X�X�V����																	<V720>	*/
/*--------------------------------------------------------------------------------------------------*/
//		swk1 = EncIfV.BitIprm;	/* DivWk0 <-- EncIfV.BitIprm									*/

//		if( AxisRsc->EncIfV.BitIprm & UPGDIVOUT )
//		{
//			MpUPDATE_DIVPOS( );			/*  --> �����p���X�X�V,etc									*/
//		}
/*--------------------------------------------------------------------------------------------------*/
		AxisRsc->StsFlg.CtrlStsRW = AxisRsc->SvIpRegR->CTSTR;	/* StsFlg.CtrlStsRW <- Control register								*/
		AxisRsc->StsFlg.CtrlStsRW = ( AxisRsc->StsFlg.CtrlStsRW & DLIMI );	/* StsFlg.CtrlStsRW <-- StsFlg.CtrlStsRW & DLIMI (imm_16)						*///110525tanaka21,���̃r�b�g���Z�͕K�v�Ȃ̂��H
		AxisRsc->StsFlg.CtrlStsRW = ( AxisRsc->StsFlg.CtrlStsRW & TLIMI );	/* StsFlg.CtrlStsRW <-- StsFlg.CtrlStsRW & TLIMI (imm_16)						*/
/*--------------------------------------------------------------------------------------------------*/
	}

	/* ��H/W�A�N�Z�X�����ʂ̂��̂��܂Ƃ߂����I�I0���ڂ��ď����̂��i�D������ */
	AxisHdl[0].SvIpRegW->EIX = 0x0;		/* enable interupt		<V112>								*/

	return;
}





/****************************************************************************************************/
/*																									*/
/*		AD Interupt Procedure																		*/
/*																									*/
/*		�}�C�N�������@�\�ɂăG���R�[�_����(@INT_ENC)�ǉ��̂��ߊ������x��(INTLVWR)�}�X�N�����ύX		*/
/****************************************************************************************************/
void	MpIntAD( void )
{
#ifdef	WIN32
	DWREG lmtBuf;			/* �������Z�p���~�b�g���f�p�o�b�t�@				*/
	UCHAR lmtBufsign[2];	/* ���~�b�g�o�b�t�@���͒l����		0:�O���A1:�㍀	*/
	UCHAR lmtBufSw;			/* ���~�b�g�o�b�t�@���͒l�X�C�b�`	0:�O���A1:�㍀	*/
#endif
	USHORT				ax_no;
	MICRO_AXIS_HANDLE	*AxisRsc;

	/* ��H/W�A�N�Z�X�����ʂ̂��̂��܂Ƃ߂����I�I0���ڂ��ď����̂��i�D������ */
	/* level(AD=0, INT1=0/4 HOST=0) */
	AxisHdl[0].SvIpRegW->INTLVWR &= 0x00F0;
	AxisHdl[0].SvIpRegW->OUTPT = 0x1;
	AxisHdl[0].SvIpRegW->WDT1L = 0x0;		/* Watch dog reset */

	/* Get Current Feedback Data from A/D */
	for( ax_no = 0; ax_no < AxisInfo.AxisNum; ax_no++ )
	{
		AxisRsc = &AxisHdl[ax_no];
/*--------------------------------------------------------------------------------------------------*/
/*		A/D convert data loading																	*/
/*--------------------------------------------------------------------------------------------------*/
/*		IntAdV.IuInData = IntAdP.Kcu * ( IUS + IntAdV.IuOffset ) / 2^8															*/
/*		IntAdV.IvInData = IntAdP.Kcv * ( IVS + IntAdV.IvOffset ) / 2^8															*/
/*--------------------------------------------------------------------------------------------------*/
//		IntAdV.IuInData = ( ( (IuAD >> 2) + IntAdV.IuOffset ) * IntAdP.Kcu ) >> 8;
/* 2012.10.05 Y.Oka �ϊ��O��%shr�Ȃ̂�IlibASR32�ł́H */
//		AxisRsc->IntAdwk.swk0 = IlibASR16(AxisRsc->SvIpRegR->IuAD, 2);
//		AxisRsc->IntAdV.IuInData = IlibASR16( (LONG)(AxisRsc->IntAdwk.swk0 + AxisRsc->IntAdV.IuOffset) * (LONG)AxisRsc->IntAdP.Kcu, 8 );
		AxisRsc->IntAdwk.swk0 = (SHORT)IlibASR32((SHORT)AxisRsc->SvIpRegR->IuAD, 2);
		AxisRsc->IntAdV.IuInData = (SHORT)IlibASR32( (LONG)(AxisRsc->IntAdwk.swk0 + AxisRsc->IntAdV.IuOffset) * (LONG)AxisRsc->IntAdP.Kcu, 8 );
/* 2012.10.05 Y.Oka �ϊ��O��%shr�Ȃ̂�IlibASR32�ł́H */
/*--------------------------------------------------------------------------------------------------*/
//		IntAdV.IvInData = ( ( (IvAD >> 2) + IntAdV.IvOffset ) * IntAdP.Kcv ) >> 8;
/* 2012.10.05 Y.Oka �ϊ��O��%shr�Ȃ̂�IlibASR32�ł́H */
//		AxisRsc->IntAdwk.swk0 = IlibASR16(AxisRsc->SvIpRegR->IvAD, 2);
//		AxisRsc->IntAdV.IvInData = IlibASR16( (LONG)(AxisRsc->IntAdwk.swk0 + AxisRsc->IntAdV.IvOffset) * (LONG)AxisRsc->IntAdP.Kcv, 8 );
		AxisRsc->IntAdwk.swk0 = (SHORT)IlibASR32((SHORT)AxisRsc->SvIpRegR->IvAD, 2);
		AxisRsc->IntAdV.IvInData = (SHORT)IlibASR32( (LONG)(AxisRsc->IntAdwk.swk0 + AxisRsc->IntAdV.IvOffset) * (LONG)AxisRsc->IntAdP.Kcv, 8 );
/* 2012.10.05 Y.Oka �ϊ��O��%shr�Ȃ̂�IlibASR32�ł́H */

/* for debug */
		if( micro_debug_ctr < 1000 )
		{
			micro_debug_buf1[micro_debug_ctr] = AxisRsc->SvIpRegR->IuAD;
			micro_debug_buf2[micro_debug_ctr] = AxisRsc->SvIpRegR->IvAD;
			micro_debug_buf3[micro_debug_ctr] = AxisRsc->IntAdV.IuInData;
			micro_debug_buf4[micro_debug_ctr] = AxisRsc->IntAdV.IvInData;
//			micro_debug_buf5[micro_debug_ctr] = AxisRsc->VcmpV.LqC;
//			micro_debug_buf6[micro_debug_ctr] = AxisHdl[0].SvIpRegW->PwmT0;
			micro_debug_ctr++;
		}
		else
		{
			micro_debug_ctr = 0;
		}
/* for debug */


	}


	/* Execute Current Loop Main Operation */
	for( ax_no = 0; ax_no < AxisInfo.AxisNum; ax_no++ )
	{
		AxisRsc = &AxisHdl[ax_no];

//===================================================================
// �ʑ���ԏ���			<V112>
//===================================================================
		if( AxisRsc->PhaseV.PhaseIpF != 1 )
		{
			/* �t���O���Z�b�g */
			AxisRsc->PhaseV.PhaseIpF = 1;
		}
		else
		{
			/* �ʑ��Ɉʑ���Ԓl�𑫂����� */
			AxisRsc->PhaseV.PhaseH = AxisRsc->PhaseV.PhaseH + AxisRsc->PhaseV.PhaseIp;
		}


//===================================================================
// PHASE_UPDATE����		<V112>
//===================================================================
/*--------------------------------------------------------------------------------------------------*/
/*		theta calculation																			*/
/*--------------------------------------------------------------------------------------------------*/
		AxisRsc->IntAdwk.swk0 = AxisRsc->PhaseV.PhaseH;
		AxisRsc->IntAdwk.swk0 = AxisRsc->IntAdwk.swk0 + 32;						/* TMP3 <-- PhaseV.PhaseH + 2^5 */
		AxisRsc->IntAdwk.swk1 = PI23;
		AxisRsc->IntAdwk.swk2 = AxisRsc->IntAdwk.swk1 + AxisRsc->IntAdwk.swk0;	/* TMP4 <-- PhaseV.PhaseH + 2PI/3 */
		AxisRsc->IntAdwk.swk3 = AxisRsc->IntAdwk.swk0 - AxisRsc->IntAdwk.swk1;	/* TMP5 <-- PhaseV.PhaseH - 2PI/3 */
/*--------------------------------------------------------------------------------------------------*/
/*		table read and get iu,iv by Id,Iq reference													*/
/*--------------------------------------------------------------------------------------------------*/
		AxisRsc->IntAdwk.swk1 = AxisRsc->IntAdwk.swk0 >> 6;	/* TMP1 <-- TMP3 >> 6 */
		IxTblSin16( &AxisRsc->SinTbl.SinT, AxisRsc->IntAdwk.swk1 );		/* SinTbl.SinT <-- stable[ TMP1 ] *//* tanaka21,�v�R�����g����	*/
		AxisRsc->IntAdwk.swk0 = AxisRsc->IntAdwk.swk0 + PI2;			/* TMP3 <-- TMP3 + PI/2 */
		AxisRsc->IntAdwk.swk1 = AxisRsc->IntAdwk.swk0 >> 6;	/* TMP1 <-- TMP3 >> 6 */
		IxTblSin16( &AxisRsc->SinTbl.CosT, AxisRsc->IntAdwk.swk1 );		/* SinTbl.CosT <-- stable[ TMP1 ] *//* tanaka21,�v�R�����g����	*/

		AxisRsc->IntAdwk.swk1 = AxisRsc->IntAdwk.swk3 >> 6;	/* TMP1 <-- TMP5 >> 6 */
		IxTblSin16( &AxisRsc->SinTbl.SinT3, AxisRsc->IntAdwk.swk1 );	/* SinTbl.SinT3 <-- stable[ TMP1 ] *//* tanaka21,�v�R�����g����	*/
		AxisRsc->IntAdwk.swk3 = AxisRsc->IntAdwk.swk3 + PI2;			/* TMP5 <-- TMP5 + PI/2 */
		AxisRsc->IntAdwk.swk1 = AxisRsc->IntAdwk.swk3 >> 6;	/* TMP1 <-- TMP5 >> 6 */
		IxTblSin16( &AxisRsc->SinTbl.CosT3, AxisRsc->IntAdwk.swk1 );	/* SinTbl.CosT3 <-- stable[ TMP1 ] *//* tanaka21,�v�R�����g����	*/

		AxisRsc->IntAdwk.swk1 = AxisRsc->IntAdwk.swk2 >> 6;	/* TMP1 <-- TMP4 >> 6 */
		IxTblSin16( &AxisRsc->SinTbl.SinT2, AxisRsc->IntAdwk.swk1 );	/* SinTbl.SinT2 <-- stable[ TMP1 ] *//* tanaka21,�v�R�����g����	*/
		AxisRsc->IntAdwk.swk2 = AxisRsc->IntAdwk.swk2 + PI2;			/* TMP4 <-- TMP4 + PI/2 */
		AxisRsc->IntAdwk.swk1 = AxisRsc->IntAdwk.swk2 >> 6;	/* TMP1 <-- TMP4 >> 6 */
		IxTblSin16( &AxisRsc->SinTbl.CosT2, AxisRsc->IntAdwk.swk1 );	/* SinTbl.CosT2 <-- stable[ TMP1 ] *//* tanaka21,�v�R�����g����	*/

/*--------------------------------------------------------------------------------------------------*/
/*		dq-trans(UVW to DQ)																			*/
/*--------------------------------------------------------------------------------------------------*/
/*		ID = IntAdP.Kc * ( (SinTbl.CosT-SinTbl.CosT2)*IntAdV.IuInData/2^14 + (SinTbl.CosT3-SinTbl.CosT2)*IntAdV.IvInData/2^14 ) /2^9							*/
/*		IQ = IntAdP.Kc * ( (SinTbl.SinT2-SinTbl.SinT)*IntAdV.IuInData/2^14 + (SinTbl.SinT2-SinTbl.SinT3)*IntAdV.IvInData/2^14 ) /2^9							*/
/*--------------------------------------------------------------------------------------------------*/
		/* TMP1 <-- cos(th) - cos(th-2pi/3) */
		AxisRsc->IntAdwk.swk1 = AxisRsc->SinTbl.CosT - AxisRsc->SinTbl.CosT2;
		/* ACC <-- TMP1 * iu */
		AxisRsc->IntAdwk.swk2 = (SHORT)IlibASR32(( (LONG)AxisRsc->IntAdwk.swk1 * (LONG)AxisRsc->IntAdV.IuInData ) , 14 );
		/* TMP1 <-- cos(th-2pi/3)-cos(th+2pi/3) */
		AxisRsc->IntAdwk.swk1 = AxisRsc->SinTbl.CosT3 - AxisRsc->SinTbl.CosT2;
		/* ACC <-- TMP1 * iv */
		AxisRsc->IntAdwk.swk1 = (SHORT)IlibASR32(( (LONG)AxisRsc->IntAdwk.swk1 * (LONG)AxisRsc->IntAdV.IvInData ) , 14 );
		/* TMP2 <-- TMP2 + TMP1 */
		AxisRsc->IntAdwk.swk2 = AxisRsc->IntAdwk.swk1 + AxisRsc->IntAdwk.swk2;
		/* ACC <-- IntAdP.Kc * TMP2 */
		AxisRsc->IntAdV.IdInData = (SHORT)IlibASR32(( (LONG)AxisRsc->IntAdP.Kc * (LONG)AxisRsc->IntAdwk.swk2 ) , 9 );
/*--------------------------------------------------------------------------------------------------*/
		AxisRsc->IntAdwk.swk1 = AxisRsc->SinTbl.SinT2 - AxisRsc->SinTbl.SinT;												/* TMP1 <-- sin(th+2pi/3) - sin(th)					*/
		AxisRsc->IntAdwk.swk2 = (SHORT)IlibASR32(( (LONG)AxisRsc->IntAdwk.swk1 * (LONG)AxisRsc->IntAdV.IuInData ) , 14 );	/* ACC <-- TMP1 * iu										*/
		AxisRsc->IntAdwk.swk1 = AxisRsc->SinTbl.SinT2 - AxisRsc->SinTbl.SinT3;												/* TMP1 <-- sin(th+2pi/3)-sin(th-2pi/3)				*/
		AxisRsc->IntAdwk.swk1 = (SHORT)IlibASR32(( (LONG)AxisRsc->IntAdwk.swk1 * (LONG)AxisRsc->IntAdV.IvInData ) , 14 );	/* ACC <-- TMP1 * iv										*/
		AxisRsc->IntAdwk.swk2 = AxisRsc->IntAdwk.swk1 + AxisRsc->IntAdwk.swk2;												/* TMP2 <-- TMP2 + TMP1							*/
		AxisRsc->IntAdV.IqInData = (SHORT)IlibASR32(( (LONG)AxisRsc->IntAdP.Kc * (LONG)AxisRsc->IntAdwk.swk2 ) , 9 );		/* ACC <-- IntAdP.Kc * TMP2										*/


/*--------------------------------------------------------------------------------------------------*/
/*		Current Observer	<V038>																	*/
/*--------------------------------------------------------------------------------------------------*/
//==============================================
//	�d���I�u�U�[�o�X�C�b�`
//==============================================
		if( AxisRsc->IntAdP.CtrlSw & OBSSEL )
		{
//==============================================
//	�_���s���O�Q�C���̐ݒ�	<V076>
//==============================================
			AxisRsc->DobsV.DmpGain = 2;
//==============================================
//	q���d���̖O�a�`�F�b�N	<V076>
//==============================================
			if( AxisRsc->IntAdV.IqInData >= 0 )
			{ /* 0�ȏ�̂Ƃ� */
				/* TMP3 = IntAdV.IqInData */
				AxisRsc->IntAdwk.swk2 = AxisRsc->IntAdV.IqInData;
			}
			else					/* ���̂Ƃ�				*/
			{
				AxisRsc->IntAdwk.swk2 = ~AxisRsc->IntAdV.IqInData;	/* TMP3 = ~IntAdV.IqInData;											*///110530tanaka21��ƃ����A-1�|����̂Ƃǂ����������H
				AxisRsc->IntAdwk.swk2 = AxisRsc->IntAdwk.swk2 + 1;	/* TMP3 = TMP3 + 1											*/
			}
			if( AxisRsc->IntAdwk.swk2 <= 14250 )
			{
				AxisRsc->IntAdwk.swk3 = 0;		/* TMP4 = 0 ( OverFlowCheck = OK )							*/
			}
			else
			{
				AxisRsc->IntAdwk.swk3 = 1;		/* TMP4 = 1 ( OverFlowCheck = NG )							*/
			}
//======================================
//	d���I�u�U�[�o��
//======================================
			AxisRsc->IntAdwk.swk0 = (SHORT)IlibASR32(( (LONG)AxisRsc->DobsP.TsPerL * (LONG)AxisRsc->VcmpV.VdOut ) , 15 );		/* TMP0	<-- ACC >> 15		( TMP0 = Ts/L * Vd_out >> 15 )	*/
			AxisRsc->IntAdwk.swk2 = AxisRsc->IntAdV.IdInData;	/* TMP3	<-- IntAdV.IdInData					<V076>						*/
			if( AxisRsc->IntAdwk.swk2 > 15000 )
			{
				AxisRsc->IntAdwk.swk2 = 15000;
			}
			else if( AxisRsc->IntAdwk.swk2 < (-15000) )
			{
				AxisRsc->IntAdwk.swk2 = -15000;
			}
			AxisRsc->IntAdwk.swk1 = AxisRsc->IntAdwk.swk2 - AxisRsc->DobsV.IdObsOut;	/*								<V076>			*/
			AxisRsc->IntAdwk.swk1 = (SHORT)IlibASR32(( (LONG)AxisRsc->DobsP.Gobs * (LONG)AxisRsc->IntAdwk.swk1 ) , 16 );	/* ACC	<-- TMP2*DobsP.Gobs		( TMP2 = g * ( Id - Id_obs ) )	*/
			AxisRsc->IntAdwk.swk0 = AxisRsc->IntAdwk.swk1 + AxisRsc->IntAdwk.swk0;	/* TMP0	<-- TMP0 + TMP2		( TMP0 = ( g*(Id-Id_obs)>>16 ) + (Ts/L*Vd_out>>15) )	*/
			AxisRsc->IntAdwk.swk1 = (SHORT)IlibASR32(( (LONG)AxisRsc->DobsP.RLTs * (LONG)AxisRsc->DobsV.IdObsOut ) , 12 );	/* TMP2	<-- DobsV.IqObsOut			( TMP2 = Id_obs )				*/
#ifdef	WIN32
			IxADDSUBLMTCHKRDY( AxisRsc->IntAdwk.swk1, AxisRsc->IntAdwk.swk0 );
#endif
			AxisRsc->DobsV.IdObsOut = AxisRsc->IntAdwk.swk1 + AxisRsc->IntAdwk.swk0;	/* DobsV.IdObsOut	<-- TMP0 + TMP2		( TMP2 = Id_obs[k+1] )	*/
#ifdef	WIN32
			IxADDLMTCHK( AxisRsc->DobsV.IdObsOut );
#endif
			AxisRsc->DobsV.IdObsOut = IxLmtCBS16( AxisRsc->DobsV.IdObsOut );	/* DobsV.IdObsOut	<-- limit( DobsV.IdObsOut, 2^15-1 )						*/
//======================================
//	d���t�B���^��
//======================================
//--------------------------------------
//	error obs
//--------------------------------------
			AxisRsc->IntAdwk.swk0 = AxisRsc->IntAdV.IdInData - AxisRsc->DobsV.IdObsOut;	/* 														*/
//--------------------------------------
//	low pass filter
//--------------------------------------
#ifdef	WIN32
			IxADDSUBLMTCHKRDY( AxisRsc->IntAdwk.swk0, AxisRsc->DobsV.LpfIld.s[1] );
#endif
			AxisRsc->IntAdwk.swk0 = AxisRsc->IntAdwk.swk0 - AxisRsc->DobsV.LpfIld.s[1];	/* 											*/
#ifdef	WIN32
			IxSUBLMTCHK( AxisRsc->IntAdwk.swk0 );
#endif
			AxisRsc->IntAdwk.swk0 = IxLmtCBS16( AxisRsc->IntAdwk.swk0 );	/* 														*/
			AxisRsc->IntAdwk.lwk2 = (LONG)(((INT64)AxisRsc->DobsP.FilObsGain * (INT64)AxisRsc->IntAdwk.swk0 ) << 2 );	/* 														*/
#ifdef	WIN32
			IxADDSUBLMTCHKRDY( AxisRsc->IntAdwk.lwk2, AxisRsc->DobsV.LpfIld.l );
#endif
			AxisRsc->IntAdwk.lwk2 = AxisRsc->IntAdwk.lwk2 + AxisRsc->DobsV.LpfIld.l;	/* 												*/
			AxisRsc->IntAdwk.lwk2 = (LONG)IlibASR64(( (INT64)AxisRsc->DobsP.FilObsGain * (INT64)AxisRsc->IntAdwk.swk0 ), 2 );	/* 														*/
#ifdef	WIN32
			IxADDLMTCHK( AxisRsc->IntAdwk.lwk2 );
#endif
			AxisRsc->DobsV.LpfIld.l = IxLmtCBS32( AxisRsc->IntAdwk.lwk2 );	/* 														*/
			IxNop( );
			IxNop( );
//--------------------------------------
//	high pass filter
//--------------------------------------
#ifdef	WIN32
			IxADDSUBLMTCHKRDY( AxisRsc->DobsV.LpfIld.s[1], AxisRsc->DobsV.HpfIld.s[1] );
#endif
			AxisRsc->IntAdwk.swk0 = AxisRsc->DobsV.LpfIld.s[1] - AxisRsc->DobsV.HpfIld.s[1];	/* 										*/
#ifdef	WIN32
			IxSUBLMTCHK( AxisRsc->IntAdwk.swk0 );
#endif
			AxisRsc->IntAdwk.swk0 = IxLmtCBS16( AxisRsc->IntAdwk.swk0 );	/* 														*/
			AxisRsc->IntAdwk.lwk2 = (LONG)(((INT64)AxisRsc->DobsP.FilObsGain * (INT64)AxisRsc->IntAdwk.swk0 ) << 2 );	/* 														*/
#ifdef	WIN32
			IxADDSUBLMTCHKRDY( AxisRsc->IntAdwk.lwk2, AxisRsc->DobsV.HpfIld.l );
#endif
			AxisRsc->IntAdwk.lwk2 = AxisRsc->IntAdwk.lwk2 + AxisRsc->DobsV.HpfIld.l;	/* 												*/
#ifdef	WIN32
			IxADDLMTCHK( AxisRsc->IntAdwk.lwk2 );
#endif
			AxisRsc->DobsV.HpfIld.l = IxLmtCBS32( AxisRsc->IntAdwk.lwk2 );	/* 														*/
			IxNop( );
			IxNop( );
			AxisRsc->DobsV.IdObsFreq = AxisRsc->DobsV.LpfIld.s[1] - AxisRsc->DobsV.HpfIld.s[1];	/* 										*/
//--------------------------------------
//	IntAdV.IdInData = IntAdV.IdInData - DobsV.IdObsFreq
//--------------------------------------
			AxisRsc->DobsV.IdObsFreq = AxisRsc->DobsV.IdObsFreq * AxisRsc->DobsV.DmpGain;		/* ACC	<-- DobsV.IdObsFreq * DobsV.DmpGain									*/
			if( AxisRsc->IntAdwk.swk3 != 0 )
			{
				AxisRsc->DobsV.IdObsFreq = 0;		/* DobsV.IdObsFreq��0�Ƃ���				*/
			}
			AxisRsc->IntAdV.IdInData = AxisRsc->IntAdV.IdInData - AxisRsc->DobsV.IdObsFreq;	/* 														*/
//=====================================
//	q���I�u�U�[�o��
//======================================
			AxisRsc->IntAdwk.swk0 = (SHORT)IlibASR32(( (LONG)AxisRsc->DobsP.TsPerL * (LONG)AxisRsc->VcmpV.VqOut ) , 15 );	/* ACC	<-- TMP0*Ts/L		( TMP0 = Ts/L * Vq_out)			*/
			AxisRsc->IntAdwk.swk2 = AxisRsc->IntAdV.IqInData;	/* TMP3	<-- IntAdV.IqInData				<V076>							*/
			AxisRsc->IntAdwk.swk2 = IxLIMITUL(AxisRsc->IntAdwk.swk2, 15000, -15000 );	/* TMP3	<-- Limit(15000)	<V076>						*/
			AxisRsc->IntAdwk.swk1 = AxisRsc->IntAdwk.swk2 - AxisRsc->DobsV.IqObsOut;	/*							<V076>				*/
			AxisRsc->IntAdwk.swk1 = (SHORT)IlibASR32( (LONG)AxisRsc->DobsP.Gobs * (LONG)AxisRsc->IntAdwk.swk1 , 16 );		/* TMP2	<-- ACC >> 16		( TMP2 = g * ( Iq - Iq_obs ) >> 16 )	*/
			AxisRsc->IntAdwk.swk0 = AxisRsc->IntAdwk.swk1 + AxisRsc->IntAdwk.swk0;	/* TMP0	<-- TMP0 + TMP2		( TMP0 = ( g*(Iq-Iq_obs)>>16 ) + (Ts/L*Vq_out>>15) )	*/
			AxisRsc->IntAdwk.swk1 = (SHORT)IlibASR32( (LONG)AxisRsc->DobsP.RLTs * (LONG)AxisRsc->DobsV.IqObsOut , 12 );		/* TMP2	<-- ACC >> 12		( TMP2 = (1-R*Ts/L)*Iq_obs >> 12 )	*/
#ifdef	WIN32
			IxADDSUBLMTCHKRDY( AxisRsc->IntAdwk.swk1, AxisRsc->IntAdwk.swk0 );
#endif
			AxisRsc->DobsV.IqObsOut = AxisRsc->IntAdwk.swk1 + AxisRsc->IntAdwk.swk0;	/* DobsV.IqObsOut	<-- TMP0 + TMP2		( TMP2 = Iq_obs[k+1] )	*/
#ifdef	WIN32
			IxADDLMTCHK( AxisRsc->DobsV.IqObsOut );
#endif
			AxisRsc->DobsV.IqObsOut = IxLmtCBS16( AxisRsc->DobsV.IqObsOut );	/* DobsV.IqObsOut	<-- limit( DobsV.IqObsOut, 2^15-1 )						*/
//======================================
//	q���t�B���^��
//======================================
//--------------------------------------
//	error obs
//--------------------------------------
			AxisRsc->IntAdwk.swk0 = AxisRsc->IntAdV.IqInData - AxisRsc->DobsV.IqObsOut;	/* 														*/
//--------------------------------------
//	low pass filter
//--------------------------------------
#ifdef	WIN32
			IxADDSUBLMTCHKRDY( AxisRsc->IntAdwk.swk0, AxisRsc->DobsV.LpfIlq.s[1] );
#endif
			AxisRsc->IntAdwk.swk0 = AxisRsc->IntAdwk.swk0 - AxisRsc->DobsV.LpfIlq.s[1];	/* 											*/
#ifdef	WIN32
			IxSUBLMTCHK( AxisRsc->IntAdwk.swk0 );
#endif
			AxisRsc->IntAdwk.swk0 = IxLmtCBS16( AxisRsc->IntAdwk.swk0 );	/* 														*/
			AxisRsc->IntAdwk.lwk2 = ( (LONG)AxisRsc->DobsP.FilObsGain * (LONG)AxisRsc->IntAdwk.swk0) << 2;	/* 														*/
#ifdef	WIN32
			IxADDSUBLMTCHKRDY( AxisRsc->IntAdwk.lwk2, AxisRsc->DobsV.LpfIlq.l );
#endif
			AxisRsc->IntAdwk.lwk2 = AxisRsc->IntAdwk.lwk2 + AxisRsc->DobsV.LpfIlq.l;	/* 												*/
#ifdef	WIN32
			IxADDLMTCHK( AxisRsc->IntAdwk.lwk2 );
#endif
			AxisRsc->DobsV.LpfIlq.l = IxLmtCBS32( AxisRsc->IntAdwk.lwk2 );	/* 														*/
			IxNop( );
			IxNop( );
//--------------------------------------
//	high pass filter
//--------------------------------------
#ifdef	WIN32
			IxADDSUBLMTCHKRDY( AxisRsc->DobsV.LpfIlq.s[1], AxisRsc->DobsV.HpfIlq.s[1] );
#endif
			AxisRsc->IntAdwk.swk0 = AxisRsc->DobsV.LpfIlq.s[1] - AxisRsc->DobsV.HpfIlq.s[1];	/* 										*/
#ifdef	WIN32
			IxSUBLMTCHK( AxisRsc->IntAdwk.swk0 );
#endif
			AxisRsc->IntAdwk.swk0 = IxLmtCBS16( AxisRsc->IntAdwk.swk0 );	/* 														*/
			AxisRsc->IntAdwk.lwk2 = ( (LONG)AxisRsc->DobsP.FilObsGain * (LONG)AxisRsc->IntAdwk.swk0) << 2;	/* 														*/
#ifdef	WIN32
			IxADDSUBLMTCHKRDY( AxisRsc->IntAdwk.lwk2, AxisRsc->DobsV.HpfIlq.l );
#endif
			AxisRsc->IntAdwk.lwk2 = AxisRsc->IntAdwk.lwk2 + AxisRsc->DobsV.HpfIlq.l;	/* 												*/
#ifdef	WIN32
			IxADDLMTCHK( AxisRsc->IntAdwk.lwk2 );
#endif
			AxisRsc->DobsV.HpfIlq.l = IxLmtCBS32( AxisRsc->IntAdwk.lwk2 );	/* 														*/
			IxNop( );
			IxNop( );
			AxisRsc->DobsV.IqObsFreq = AxisRsc->DobsV.LpfIlq.s[1] - AxisRsc->DobsV.HpfIlq.s[1];	/* 										*/
//--------------------------------------
//	IntAdV.IqInData = IntAdV.IqInData - DobsV.IqObsFreq
//--------------------------------------
			AxisRsc->DobsV.IqObsFreq = AxisRsc->DobsV.IqObsFreq * AxisRsc->DobsV.DmpGain;		/* ACC	<-- DobsV.IqObsFreq * DobsV.DmpGain									*/
			if( AxisRsc->IntAdwk.swk3 != 0 )
			{
				AxisRsc->DobsV.IqObsFreq = 0;		/* DobsV.IdObsFreq��0�Ƃ���				*/
			}
			AxisRsc->IntAdV.IqInData = AxisRsc->IntAdV.IqInData - AxisRsc->DobsV.IqObsFreq;	/* 														*/
		}
/*--------------------------------------------------------------------------------------------------*///110526tanaka21,BB�`�F�b�N�����A�����������낢��ύX�B
/*		Base Block Check																			*///if-else if-else�̌`�ŏ��������B���������삷�邩�v�m�F�B
/*--------------------------------------------------------------------------------------------------*/
		if( AxisRsc->AdStop.ADRst != 0 )
		{
			AxisRsc->AdStop.ADRst = 0;
			AxisRsc->IntAdwk.swk6 = AxisRsc->IntAdV.CrFreqW >> 1;
			AxisRsc->IntAdwk.swk5 = AxisRsc->IntAdwk.swk6;
			AxisRsc->IntAdwk.swk4 = AxisRsc->IntAdwk.swk6;
/*--------------------------------------------------------------------------------------------------*/
		}
		else if( AxisRsc->StsFlg.CtrlStsRW == BB )
		{
/*--------------------------------------------------------------------------------------------------*/
			AxisRsc->IntAdwk.swk6 = AxisRsc->IntAdV.CrFreqW >> 1;
			AxisRsc->IntAdwk.swk5 = AxisRsc->IntAdwk.swk6;
			AxisRsc->IntAdwk.swk4 = AxisRsc->IntAdwk.swk6;
/*--------------------------------------------------------------------------------------------------*/
		}
		else
		{
/****************************************************************************************************/
/*																									*/
/*		��ߊE���pId�w�ߌv�Z����		<V214>														*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		��ߊE�������I��																			*/
/*--------------------------------------------------------------------------------------------------*/
			if( AxisRsc->IntAdP.CtrlSw & V_FB )
			{
/*--------------------------------------------------------------------------------------------------*/
/* 		�����d���쐬																				*/
/*			Vq*�Ɗ�d��(��(IntAdP.Vmax^2-Vd^2))���r���A�����d�������B								*/
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
// 		Vqmax = ��( VmaxX^2 - Vd^2 )																*
/*--------------------------------------------------------------------------------------------------*/
				AxisRsc->IntAdwk.lwk2 = AxisRsc->WeakFV.WfV1Max * AxisRsc->WeakFV.WfV1Max;	/* IntAdP.Vmax^2													*/
				AxisRsc->IntAdwk.lwk4 = AxisRsc->WeakFV.WfVdRef * AxisRsc->WeakFV.WfVdRef;	/* Vd^2								; �폜 <V309>	����<V531>	*/
#ifdef	WIN32
				IxADDSUBLMTCHKRDY( AxisRsc->IntAdwk.lwk2, AxisRsc->IntAdwk.lwk4 );
#endif
				AxisRsc->IntAdwk.lwk2 = AxisRsc->IntAdwk.lwk2 - AxisRsc->IntAdwk.lwk4;	/* IntAdP.Vmax^2 - Vd^2								*/
#ifdef	WIN32
				IxSUBLMTCHK( AxisRsc->IntAdwk.lwk2 );
#endif
				AxisRsc->IntAdwk.lwk2 = IxLmtCBS32( AxisRsc->IntAdwk.lwk2 );	/* 														*/
				AxisRsc->IntAdwk.lwk2 = IxLIMITUL( AxisRsc->IntAdwk.lwk2, LPX_REG32_MAX, LPX_REG32_MIN );	/* if (IntAdP.Vmax^2 - Vd^2)< 0, then (IntAdP.Vmax^2 - Vd^2) = 0	*/
				AxisRsc->IntAdwk.swk0 = MpSQRT( &AxisRsc->IntAdwk, AxisRsc->IntAdwk.lwk2 );					/* ��( IntAdP.Vmax^2 - Vd^2 )										*/
				if( AxisRsc->IntAdwk.swk0 > 0x7FFF )
				{
					AxisRsc->IntAdwk.swk0 = 0x7FFF;	/* 															*/
				}
				AxisRsc->WeakFV.WfVqMax = AxisRsc->IntAdwk.swk0;	/* Vqmax = ��( IntAdP.Vmax^2 - Vd^2 )							*/
/*--------------------------------------------------------------------------------------------------*/
// 		TMP0 = Vqmax - Vq																			*
/*--------------------------------------------------------------------------------------------------*/
				AxisRsc->IntAdwk.swk1 = AxisRsc->WeakFV.WfVqRef;
				if( AxisRsc->IntAdwk.swk1 < 0 )
				{
					AxisRsc->IntAdwk.swk1 = (SHORT)ZEROR - AxisRsc->IntAdwk.swk1;	/* TMP1 = |Vq|								*/
					IxNop( );						/* 															*/
				}
				AxisRsc->IntAdwk.swk0 = AxisRsc->WeakFV.WfVqMax - AxisRsc->IntAdwk.swk1;	/* TMP0 = Vqmax - Vq = ��Vq						*/
				AxisRsc->IntAdwk.swk0 = IxLmtCBS16( AxisRsc->IntAdwk.swk0 );	/* 														*/
/*--------------------------------------------------------------------------------------------------*/
/*		��ፀ�v�Z																					*/
/*--------------------------------------------------------------------------------------------------*/
				AxisRsc->IntAdwk.lwk1 = (LONG)AxisRsc->IntAdwk.swk0;	/* TMP1,0 = �����g��(TMP0)									*/
//#ifdef WIN32
				AxisRsc->IntAdwk.swk2 = (SHORT)IlibASR64(( (INT64)AxisRsc->IntAdwk.lwk1 * (INT64)AxisRsc->WeakFV.WfKpV.l ) , 32 );
//#elif defined(ASIP_CC)
//				AxisRsc->IntAdwk.swk2 = asr( AxisRsc->IntAdwk.lwk1 * AxisRsc->WeakFV.WfKpV.l, 32 );
//#endif
				if( AxisRsc->IntAdwk.swk2 > 0 )
				{
					AxisRsc->IntAdwk.swk2 = LPX_REG16_MAX;	/* ���̍ő�l												*/
				}
				else if( AxisRsc->IntAdwk.swk2 < (SHORT)0xFF80 )
				{
					AxisRsc->IntAdwk.swk2 = LPX_REG16_MIN;	/* ���̍ő�l												*/
				}
				else
				{
//#ifdef WIN32
					AxisRsc->IntAdwk.lwk2 = (LONG)IlibASR64(( (INT64)AxisRsc->IntAdwk.lwk1 * (INT64)AxisRsc->WeakFV.WfKpV.l ) , 16 );
					AxisRsc->IntAdwk.swk2 = (SHORT)IlibASR64(( (INT64)AxisRsc->IntAdwk.lwk2 * (INT64)256 ) , 16 );
//#elif defined(ASIP_CC)
//					AxisRsc->IntAdwk.lwk2 = asr( AxisRsc->IntAdwk.lwk1 * AxisRsc->WeakFV.WfKpV.l, 16 );
//					AxisRsc->IntAdwk.swk2 = asr( AxisRsc->IntAdwk.lwk2 * 256, 16 );
//#endif
				}
/*--------------------------------------------------------------------------------------------------*/
/*		�ϕ����v�Z																					*/
/*--------------------------------------------------------------------------------------------------*/
				AxisRsc->IntAdwk.lwk4 = AxisRsc->IntAdwk.lwk1 * AxisRsc->WeakFV.WfKiV.l;	/* ��Vq * Kiv											*/
//#ifdef WIN32
				AxisRsc->IntAdwk.lwk6 = (LONG)IlibASR64(( (INT64)AxisRsc->IntAdwk.lwk1 * (INT64)AxisRsc->WeakFV.WfKiV.l ) , 32 );	/* ��Vq * Kiv											*/
//#elif defined(ASIP_CC)
//				AxisRsc->IntAdwk.lwk6 = asr( AxisRsc->IntAdwk.lwk1 * AxisRsc->WeakFV.WfKiV.l, 32 );	/* ��Vq * Kiv											*/
//#endif
				if( (SHORT)AxisRsc->IntAdwk.lwk6 > 0x08 )
				{
					AxisRsc->IntAdwk.lwk4 = LPX_REG32_MAX;	/* ���̍ő�l											*/
				}
				else if( (USHORT)AxisRsc->IntAdwk.lwk6 > 0xFFF8 )
				{
					AxisRsc->IntAdwk.lwk4 = LPX_REG32_MIN;	/* ���̍ő�l											*/
				}
				else
				{
					AxisRsc->IntAdwk.lwk4 = AxisRsc->IntAdwk.lwk4 >> 4;	/* 														*/
					AxisRsc->IntAdwk.lwk4 = AxisRsc->IntAdwk.lwk4 & 0x0fffffff;	/* 														*/
					AxisRsc->IntAdwk.lwk6 = AxisRsc->IntAdwk.lwk6 << 28;	/* 													*/
					AxisRsc->IntAdwk.lwk4 = AxisRsc->IntAdwk.lwk6 | AxisRsc->IntAdwk.lwk4;	/* TMP5,4 = ��Vq * Kiv (* 2^16)					*/
				}
#ifdef	WIN32
				IxADDSUBLMTCHKRDY( AxisRsc->IntAdwk.lwk4, AxisRsc->WeakFV.WfIntgl.l );
#endif
				AxisRsc->WeakFV.WfIntgl.l = AxisRsc->IntAdwk.lwk4 + AxisRsc->WeakFV.WfIntgl.l;	/* 										*/
#ifdef	WIN32
				IxADDLMTCHK( AxisRsc->WeakFV.WfIntgl.l );
#endif
				AxisRsc->WeakFV.WfIntgl.l = IxLmtCBS32( AxisRsc->WeakFV.WfIntgl.l );	/* 												*/
				AxisRsc->IntAdwk.lwk8 = (LONG)AxisRsc->WeakFV.WfIntegLim << 16;	/* TMP9,8 = WeakFV.WfIntegLim * 2^16						*/
				AxisRsc->WeakFV.WfIntgl.l = IxLIMITUL( AxisRsc->WeakFV.WfIntgl.l, AxisRsc->IntAdwk.lwk8, -AxisRsc->IntAdwk.lwk8 );	/* WFINTEGH = ��Vq * Kiv (* 2^16 / 2^16)	*/
/*--------------------------------------------------------------------------------------------------*/
/*		��ፀ + �ϕ���																				*/
/*--------------------------------------------------------------------------------------------------*/
				IxNop( );															/* JL-076�s�No.3�΍� <V214>	*/
				IxNop( );															/* JL-076�s�No.3�΍� <V214>	*/
#ifdef	WIN32
				IxADDSUBLMTCHKRDY( AxisRsc->WeakFV.WfIntgl.s[1], AxisRsc->IntAdwk.swk2 );
#endif
				AxisRsc->IntAdwk.swk4 = AxisRsc->WeakFV.WfIntgl.s[1] + AxisRsc->IntAdwk.swk2;	/* Idref = TMP4 = ��ፀ + �ϕ���			*/
#ifdef	WIN32
				IxADDLMTCHK( AxisRsc->IntAdwk.swk4 );
#endif
				AxisRsc->IntAdwk.swk4 = IxLmtCBS16( AxisRsc->IntAdwk.swk4 );	/* 														*/
				AxisRsc->IntAdwk.swk4 = IxLIMITUL( AxisRsc->IntAdwk.swk4, AxisRsc->WeakFV.WfIdRefLim, -AxisRsc->WeakFV.WfIdRefLim );	/* IdrefLim�Ń��~�b�g						*/
/*--------------------------------------------------------------------------------------------------*/
/*		Idref > 0 �Ȃ�΁AIdref = 0,�ϕ� = 0														*/
/*			Idref(d���d���w��)�����ɂȂ邱�Ƃ͖����B���ɂȂ����ꍇ�͂O�ɂ���B						*/
/*--------------------------------------------------------------------------------------------------*/
				AxisRsc->WeakFV.IdOut = AxisRsc->IntAdwk.swk4;
				if( AxisRsc->WeakFV.IdOut > 0 )
				{
					AxisRsc->WeakFV.IdOut = 0;			/* Idref > 0 �̏ꍇ�AIdref = 0								*/
					AxisRsc->WeakFV.WfIntgl.l = ZEROR;	/* Idref > 0 �̏ꍇ�A�ϕ� = 0						*/
				}
			}

/****************************************************************************************************/
/*																									*/
/*		ACRd(d���d������)																			*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		TMP1 = limit( WeakFV.IdOut - IntAdV.IdInData , 2^15 - 1)															*/
/*--------------------------------------------------------------------------------------------------*/
#ifdef	WIN32
			IxADDSUBLMTCHKRDY( AxisRsc->WeakFV.IdOut, AxisRsc->IntAdV.IdInData );
#endif
			AxisRsc->IntAdwk.swk1 = AxisRsc->WeakFV.IdOut - AxisRsc->IntAdV.IdInData;	/* TMP1 <-- WeakFV.IdOut - IntAdV.IdInData									*/
#ifdef	WIN32
			IxSUBLMTCHK( AxisRsc->IntAdwk.swk1 );
#endif
			AxisRsc->IntAdwk.swk1 = IxLmtCBS16( AxisRsc->IntAdwk.swk1 );	/* TMP1 <-- limit( TMP1 , 2^15 - 1 )				*/
/*--------------------------------------------------------------------------------------------------*/
/*		TMP2 = limit( IntAdP.KdP * TMP1 / 2^9 , 2^15 - 1 )													*/
/*--------------------------------------------------------------------------------------------------*/
			AxisRsc->IntAdwk.swk2 = (SHORT)IlibASR32(( (LONG)AxisRsc->IntAdP.KdP * (LONG)AxisRsc->IntAdwk.swk1 ) , 9);	/* ACC <-- IntAdP.KdP * TMP1										*/
			AxisRsc->IntAdwk.swk2 = IxLmtCBS16( AxisRsc->IntAdwk.swk2 );	/* TMP2 <-- limit( TMP2 , 2^15 - 1 )					*/
/*--------------------------------------------------------------------------------------------------*/
/*		IdIntgl(32) = (IntAdP.KdI * TMP1)<<3 + IdIntgl(32)														*/
/*		IDIH = limit( IDIH , IntAdP.VdLim )																*/
/*--------------------------------------------------------------------------------------------------*/
			AxisRsc->IntAdwk.lwk4 = ((LONG)AxisRsc->IntAdP.VdLim) << 16;	/* 													*/
			AxisRsc->IntAdwk.lwk6 = ( (LONG)AxisRsc->IntAdP.KdI * (LONG)AxisRsc->IntAdwk.swk1 ) << 3;	/* 														*/
#ifdef	WIN32
			IxADDSUBLMTCHKRDY( AxisRsc->IntAdwk.lwk6, AxisRsc->AcrV.IdIntgl.l );
#endif
			AxisRsc->AcrV.IdIntgl.l = AxisRsc->IntAdwk.lwk6 + AxisRsc->AcrV.IdIntgl.l;	/* 												*/
#ifdef	WIN32
			IxADDLMTCHK( AxisRsc->AcrV.IdIntgl.l );
#endif
			AxisRsc->AcrV.IdIntgl.l = IxLmtCBS32( AxisRsc->AcrV.IdIntgl.l );	/* AcrV.IdIntgl <-- limit( AcrV.IdIntgl , 2^31 - 1 )					*/
			if( LPX_ABS(AxisRsc->AcrV.IdIntgl.l) > LPX_ABS(AxisRsc->IntAdwk.lwk4) )
			{
				AxisRsc->StsFlg.CtrlStsRW = AxisRsc->StsFlg.CtrlStsRW | DLIM;		/* 															*/
				AxisRsc->IntAdwk.swk0 = AxisRsc->IntAdP.CtrlSw;
				if( AxisRsc->IntAdwk.swk0 != AxisRsc->IntAdP.CtrlSw )
				{
					AxisRsc->AcrV.IdIntgl.l = ZEROR;	/* else integral clear									*/
					IxNop( );															/* JL-076�s�No.3�΍� <V214>	*/
					IxNop( );															/* JL-076�s�No.3�΍� <V214>	*/
				}
			}
/*--------------------------------------------------------------------------------------------------*/
/*		VcmpV.VdOut = limit( TMP2 + IDIH +TMP3, 2^15 - 1 )													*/
/*--------------------------------------------------------------------------------------------------*/
#ifdef	WIN32
			IxADDSUBLMTCHKRDY( AxisRsc->AcrV.IdIntgl.s[1], AxisRsc->IntAdwk.swk2 );
#endif
			AxisRsc->IntAdwk.swk1 = AxisRsc->AcrV.IdIntgl.s[1] + AxisRsc->IntAdwk.swk2;	/* TMP1 <-- TMP2 + IDIH						*/
#ifdef	WIN32
			IxADDLMTCHK( AxisRsc->IntAdwk.swk1 );
#endif
			AxisRsc->IntAdwk.swk1 = IxLmtCBS16( AxisRsc->IntAdwk.swk1 );	/* TMP1 <-- limit( TMP1 , 2^15 - 1 )				*/
/*--------------------------------------------------------------------------------------------------*/
/*		filter : AcrV.VdFil =  ( ( ( TMP1 - VDFH ) * IntAdP.Tfil ) << 2 ) + AcrV.VdFil									*/
/*--------------------------------------------------------------------------------------------------*/
#ifdef	WIN32
			IxADDSUBLMTCHKRDY( AxisRsc->IntAdwk.swk1, AxisRsc->AcrV.VdFil.s[1] );
#endif
			AxisRsc->IntAdwk.swk1 = AxisRsc->IntAdwk.swk1 - AxisRsc->AcrV.VdFil.s[1];	/* TMP1 <-- TMP1 - VDFH						*/
#ifdef	WIN32
			IxSUBLMTCHK( AxisRsc->IntAdwk.swk1 );
#endif
			AxisRsc->IntAdwk.swk1 = IxLmtCBS16( AxisRsc->IntAdwk.swk1 );	/* TMP1 <-- limit( TMP1 , 2^15 - 1 )				*/
			AxisRsc->IntAdwk.lwk0 = ((LONG)AxisRsc->IntAdP.Tfil * (LONG)AxisRsc->IntAdwk.swk1) << 2;	/* 														*/
#ifdef	WIN32
			IxADDSUBLMTCHKRDY( AxisRsc->AcrV.VdFil.l, AxisRsc->IntAdwk.lwk0 );
#endif
			AxisRsc->IntAdwk.lwk2 = AxisRsc->AcrV.VdFil.l + AxisRsc->IntAdwk.lwk0;	/* AcrV.VdFil <-- AcrV.VdFil + TMP0							*/
#ifdef	WIN32
			IxADDLMTCHK( AxisRsc->IntAdwk.lwk2 );
#endif
			AxisRsc->AcrV.VdFil.l = IxLmtCBS32( AxisRsc->IntAdwk.lwk2 );	/* 														*/


/****************************************************************************************************/
/*																									*/
/*		ACRq(q���d������)																			*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Low Pass Filter																				*/
/*--------------------------------------------------------------------------------------------------*/
/*		IntAdP.TLpf2 : Time-constant																		*/
/*		IntAdV.IqOut2Lpf : Output(32 bit) .. IQOF: High 16 bit													*/
/*		WeakFV.IqOut   : Input																				*/
/*--------------------------------------------------------------------------------------------------*/
/*		IQOF(32) =  ( ( ( WeakFV.IqOut - IQOF(16) ) * IntAdP.TLpf2 ) << 2 ) + IntAdV.IqOut2Lpf(32)								*/
/*--------------------------------------------------------------------------------------------------*/
			if( (AxisRsc->IntAdP.CtrlSw & LPFCDSABL) != 0 )
			{
				AxisRsc->IntAdV.IqOut2Lpf.s[1] = AxisRsc->WeakFV.IqOut;	/* disable LPF												*/
			}
/*--------------------------------------------------------------------------------------------------*/
			else
			{
#ifdef	WIN32
				IxADDSUBLMTCHKRDY( AxisRsc->WeakFV.IqOut, AxisRsc->IntAdV.IqOut2Lpf.s[1] );
#endif
				AxisRsc->IntAdwk.swk0 = AxisRsc->WeakFV.IqOut - AxisRsc->IntAdV.IqOut2Lpf.s[1];	/* TMP0 <-- WeakFV.IqOut - IQOF							*/
#ifdef	WIN32
				IxSUBLMTCHK( AxisRsc->IntAdwk.swk0 );
#endif
				AxisRsc->IntAdwk.swk0 = IxLmtCBS16( AxisRsc->IntAdwk.swk0 );	/* TMP0 <-- limit( TMP0, 2^15 - 1 )						*/
				AxisRsc->IntAdwk.lwk2 = ( (LONG)AxisRsc->IntAdP.TLpf2 * (LONG)AxisRsc->IntAdwk.swk0 ) << 2;
#ifdef	WIN32
				IxADDSUBLMTCHKRDY( AxisRsc->IntAdV.IqOut2Lpf.l, AxisRsc->IntAdwk.lwk2 );
#endif
				AxisRsc->IntAdwk.lwk2 = AxisRsc->IntAdV.IqOut2Lpf.l + AxisRsc->IntAdwk.lwk2;	/* IntAdV.IqOut2Lpf <-- IntAdV.IqOut2Lpf + TMP2						*/
#ifdef	WIN32
				IxADDLMTCHK( AxisRsc->IntAdwk.lwk2 );
#endif
				AxisRsc->IntAdV.IqOut2Lpf.l = IxLmtCBS32( AxisRsc->IntAdwk.lwk2 );
			}
/*--------------------------------------------------------------------------------------------------*/
			AxisRsc->IntAdV.IqMonFil = AxisRsc->IntAdV.IqOut2Lpf.s[1];	/* IntAdV.IqMonFil:�t�B���^���q���d��(���j�^�p)	<V224>	*/
#ifdef	WIN32
			IxADDSUBLMTCHKRDY( AxisRsc->IntAdV.IqOut2Lpf.s[1], AxisRsc->IntAdV.IqDist );
#endif
			AxisRsc->IntAdV.IqOfRef = AxisRsc->IntAdV.IqOut2Lpf.s[1] + AxisRsc->IntAdV.IqDist;	/* IntAdV.IqOfRef = IQOF + IntAdV.IqDist (�O���g���N���Z)	<V224>	*/
#ifdef	WIN32
			IxADDLMTCHK( AxisRsc->IntAdV.IqOfRef );
#endif
			AxisRsc->IntAdV.IqOfRef = IxLmtCBS16( AxisRsc->IntAdV.IqOfRef );	/* IntAdV.IqOfRef <-- limit( IntAdV.IqOfRef , 2^15 - 1 )	<V224>		*/
/*--------------------------------------------------------------------------------------------------*/
/*		Torque Limit:			<V214>																*/
/*			�d���t�B�[�h�o�b�N��ߊE�������d���d���w�߂������̂ŁAq���d���w�߂͈ȉ��̎���		*/
/*			���߂��l�ƃg���N���~�b�g�ݒ�l�̂����ꂩ���������Ń��~�b�g����B						*/
/*				Iq*���~�b�g�l = ��(Imax^2-Id*^2)													*/
/*--------------------------------------------------------------------------------------------------*/
/*		Id*�ɂ��Torque Limit�l			;															*/
/*--------------------------------------------------------------------------------------------------*/
			AxisRsc->IntAdwk.lwk2 = 0x0d693a40;	/* 15000^2									*/
			AxisRsc->IntAdwk.swk0 = AxisRsc->IntAdP.CtrlSw;
			AxisRsc->IntAdwk.swk1 = V_FB | V_FB2;
			AxisRsc->IntAdwk.swk0 = AxisRsc->IntAdwk.swk0 & AxisRsc->IntAdwk.swk1;	/* TMP0��bit11,bit13�ȊO���}�X�N����			*/
			if( AxisRsc->IntAdwk.swk0 != V_FB )
			{
				AxisRsc->IntAdwk.lwk4 = (LONG)AxisRsc->WeakFV.IdOut * (LONG)AxisRsc->WeakFV.IdOut;			/* Idref^2							; �폜<V309>����<V531>	*/
			}
			else
			{
				AxisRsc->IntAdwk.lwk4 = (LONG)AxisRsc->WeakFV.WfIdRefLim * (LONG)AxisRsc->WeakFV.WfIdRefLim;	/* IdrefLim^2						; <V309>		*/
			}
			AxisRsc->IntAdwk.lwk2 = AxisRsc->IntAdwk.lwk2 - AxisRsc->IntAdwk.lwk4;	/* Imax^2 - Id^2								*/
			AxisRsc->IntAdwk.swk0 = MpSQRT( &AxisRsc->IntAdwk, AxisRsc->IntAdwk.lwk2 );					/* 															*/
			AxisRsc->IntAdwk.swk1 = AxisRsc->IntAdwk.swk0;	/* TMP0 = ��( Imax^2 - Id^2 )							*/
/*--------------------------------------------------------------------------------------------------*/
/*		Torque Limit																				*/
/*--------------------------------------------------------------------------------------------------*/
			if( AxisRsc->IntAdV.IqOfRef >= 0 )
			{
				AxisRsc->IntAdwk.swk1 = IxLIMITUL( AxisRsc->IntAdwk.swk1, AxisRsc->IntAdV.TLimP, -AxisRsc->IntAdV.TLimP );	/* �����g���N���~�b�g							*/
				AxisRsc->IntAdV.IqRef = IxLIMITUL( AxisRsc->IntAdV.IqOfRef, AxisRsc->IntAdwk.swk1, -AxisRsc->IntAdwk.swk1 );	/* <V224> �O���g���N���Z���q���d���w��			*/
				if( AxisRsc->IntAdV.IqRef == AxisRsc->IntAdwk.swk1 )
				{
					AxisRsc->StsFlg.CtrlStsRW = AxisRsc->StsFlg.CtrlStsRW | TLIM;		/* TLIM flag set											*/
				}
			}
			else
			{
				AxisRsc->IntAdwk.swk1 = IxLIMITUL( AxisRsc->IntAdwk.swk1, AxisRsc->IntAdV.TLimM, -AxisRsc->IntAdV.TLimM );	/* �����g���N���~�b�g							*/
				AxisRsc->IntAdV.IqRef = IxLIMITUL( AxisRsc->IntAdV.IqOfRef, AxisRsc->IntAdwk.swk1, -AxisRsc->IntAdwk.swk1 );	/* <V224> �O���g���N���Z���q���d���w��			*/
				if( (AxisRsc->IntAdV.IqRef + AxisRsc->IntAdwk.swk1) == 0 )
				{
					AxisRsc->StsFlg.CtrlStsRW = AxisRsc->StsFlg.CtrlStsRW | TLIM;		/* TLIM flag set											*/
				}
			}
/*--------------------------------------------------------------------------------------------------*/
/*		TMP1 = limit( IntAdV.IqRef - IntAdV.IqInData , 2^15 - 1 )														*/
/*--------------------------------------------------------------------------------------------------*/
#ifdef	WIN32
			IxADDSUBLMTCHKRDY( AxisRsc->IntAdV.IqRef, AxisRsc->IntAdV.IqInData );
#endif
			AxisRsc->IntAdwk.swk1 = AxisRsc->IntAdV.IqRef - AxisRsc->IntAdV.IqInData;	/* TMP1 <-- IQFEF - IntAdV.IqInData								*/
#ifdef	WIN32
			IxSUBLMTCHK( AxisRsc->IntAdwk.swk1 );
#endif
			AxisRsc->IntAdwk.swk1 = IxLmtCBS16( AxisRsc->IntAdwk.swk1 );	/* TMP1 <-- limit( TMP1 , 2^15 - 1 )				*/
/*--------------------------------------------------------------------------------------------------*/
/*		TMP2 = limit( IntAdP.KqP * TMP1 / 2^9 , 2^15 - 1 )													*/
/*--------------------------------------------------------------------------------------------------*/
			AxisRsc->IntAdwk.swk2 = (SHORT)IlibASR32( (LONG)AxisRsc->IntAdP.KqP * (LONG)AxisRsc->IntAdwk.swk1  , 9);		/* TMP2 <-- ACC >> 9										*/
			AxisRsc->IntAdwk.swk2 = IxLmtCBS16( AxisRsc->IntAdwk.swk2 );	/* TMP2 <-- limit( TMP2 , 2^15 - 1 )					*/
/*--------------------------------------------------------------------------------------------------*/
/*		AcrV.IqIntgl(32) = (IntAdP.KqI * TMP1)<<3 + AcrV.IqIntgl(32)														*/
/*		IQIH = limit( IQIH , IntAdP.VqLim )																*/
/*--------------------------------------------------------------------------------------------------*/
			if( ( (AxisRsc->IntAdP.CtrlSw & INT_ST) == 0) || ( (AxisRsc->StsFlg.IntglFlg & 1) == 0 ) )
			{
				AxisRsc->IntAdwk.lwk6 = (LONG)AxisRsc->IntAdP.KqI * (LONG)AxisRsc->IntAdwk.swk1;	/* ACC <-- IntAdP.KqI * TMP1										*/
				AxisRsc->IntAdwk.lwk4 = (LONG)AxisRsc->IntAdP.VqLim;	/* 															*/
				AxisRsc->IntAdwk.lwk4 = AxisRsc->IntAdwk.lwk4 << 16;	/* 													*/
				AxisRsc->IntAdwk.lwk6 = AxisRsc->IntAdwk.lwk6 << 3;	/* 														*/
#ifdef	WIN32
				IxADDSUBLMTCHKRDY( AxisRsc->IntAdwk.lwk6, AxisRsc->AcrV.IqIntgl.l );
#endif
				AxisRsc->AcrV.IqIntgl.l = AxisRsc->IntAdwk.lwk6 + AxisRsc->AcrV.IqIntgl.l;	/* AcrV.IqIntgl <-- AcrV.IqIntgl + (IntAdP.KqI*TMP1)					*/
#ifdef	WIN32
				IxADDLMTCHK( AxisRsc->AcrV.IqIntgl.l );
#endif
				AxisRsc->AcrV.IqIntgl.l = IxLmtCBS32( AxisRsc->AcrV.IqIntgl.l );	/* AcrV.IqIntgl <-- limit( AcrV.IqIntgl , 2^32 - 1 )					*/
				if( LPX_ABS(AxisRsc->AcrV.IqIntgl.l) > LPX_ABS(AxisRsc->IntAdwk.lwk4) )
				{
					AxisRsc->StsFlg.CtrlStsRW = AxisRsc->StsFlg.CtrlStsRW | QLIM;		/* IMM3 <-- STAT | QLIM (imm_16)							*/
					if( (AxisRsc->IntAdP.CtrlSw & ICLR) != 0 )
					{
						AxisRsc->AcrV.IqIntgl.l = ZEROR;	/* else integral clear									*/
						IxNop( );															/* JL-076�s�No.3�΍� <V214>	*/
						IxNop( );															/* JL-076�s�No.3�΍� <V214>	*/
					}
				}
			}
/*--------------------------------------------------------------------------------------------------*/
/*		VcmpV.VqOut = limit( TMP2 + IQIH +TMP3 , 2^15 - 1 )													*/
/*--------------------------------------------------------------------------------------------------*/
#ifdef	WIN32
			IxADDSUBLMTCHKRDY( AxisRsc->AcrV.IqIntgl.s[1], AxisRsc->IntAdwk.swk2 );
#endif
			AxisRsc->IntAdwk.swk1 = AxisRsc->AcrV.IqIntgl.s[1] + AxisRsc->IntAdwk.swk2;	/* TMP1 <-- TMP2 + IQIH						*/
#ifdef	WIN32
			IxADDLMTCHK( AxisRsc->IntAdwk.swk1 );
#endif
			AxisRsc->IntAdwk.swk1 = IxLmtCBS16( AxisRsc->IntAdwk.swk1 );	/* TMP1 <-- limit( TMP1 , 2^15 - 1 )				*/
/*--------------------------------------------------------------------------------------------------*/
/*		filter : AcrV.VqFil = ( ( ( TMP1 - VQFH ) * IntAdP.Tfil ) << 2 ) + AcrV.VqFil									*/
/*--------------------------------------------------------------------------------------------------*/
			AxisRsc->IntAdwk.swk1 = AxisRsc->IntAdwk.swk1 - AxisRsc->AcrV.VqFil.s[1];	/* TMP1 <-- TMP1 - VQFH						*/
			AxisRsc->IntAdwk.swk1 = IxLmtCBS16( AxisRsc->IntAdwk.swk1 );	/* TMP1 <-- limit( TMP1 , 2^15 - 1 )				*/
			AxisRsc->IntAdwk.lwk0 = ( (LONG)AxisRsc->IntAdP.Tfil * (LONG)AxisRsc->IntAdwk.swk1 ) << 2;	/* 														*/
#ifdef	WIN32
			IxADDSUBLMTCHKRDY( AxisRsc->AcrV.VqFil.l, AxisRsc->IntAdwk.lwk0 );
#endif
			AxisRsc->IntAdwk.lwk2 = AxisRsc->AcrV.VqFil.l + AxisRsc->IntAdwk.lwk0;	/* AcrV.VdFil <-- AcrV.VdFil + TMP0							*/
#ifdef	WIN32
			IxADDLMTCHK( AxisRsc->IntAdwk.lwk2 );
#endif
			AxisRsc->AcrV.VqFil.l = IxLmtCBS32( AxisRsc->IntAdwk.lwk2 );



/****************************************************************************************************/
/*																									*/
/*		Voltage Compensation(�d���⏞)																*/
/*																									*/
/****************************************************************************************************/
			if( (AxisRsc->IntAdP.CtrlSw & ISEL) != 0 )
			{
				AxisRsc->IntAdwk.swk1 = AxisRsc->WeakFV.IdOut;	/* TMP1 <-- reference ID									*/
				AxisRsc->IntAdwk.swk2 = AxisRsc->IntAdV.IqRef;	/* 															*/
			}
			else
			{
				AxisRsc->IntAdwk.swk1 = AxisRsc->IntAdV.IdInData;	/* TMP1 <-- feedback ID										*/
				AxisRsc->IntAdwk.swk2 = AxisRsc->IntAdV.IqInData;	/* TMP2 <-- feedback IQ										*/
			}
/*--------------------------------------------------------------------------------------------------*/
/*		TMP4(VcmpV.VdComp) = IntAdP.MotResist*TMP1/2^15 - VcmpV.LqC * TMP2 / 2^15												*/
/*--------------------------------------------------------------------------------------------------*/
			AxisRsc->IntAdwk.swk4 = (SHORT)IlibASR32( ( (LONG)AxisRsc->VcmpV.LqC * (LONG)AxisRsc->IntAdwk.swk2 ) , 15 );		/* VcmpV.VdComp <-- ACC >> 15										*/
			AxisRsc->IntAdwk.swk0 = (SHORT)IlibASR32( ( (LONG)AxisRsc->IntAdP.MotResist * (LONG)AxisRsc->IntAdwk.swk1 ) , 15 );
			AxisRsc->IntAdwk.swk4 = AxisRsc->IntAdwk.swk0 - AxisRsc->IntAdwk.swk4;
/*--------------------------------------------------------------------------------------------------*/
/*		TMP5(VcmpV.VqComp) = VcmpV.LdC * TMP1 / 2^15 + VcmpV.MagC + IntAdP.MotResist*TMP2/2^15											*/
/*--------------------------------------------------------------------------------------------------*/
			AxisRsc->IntAdwk.swk3 = (SHORT)IlibASR32( ( (LONG)AxisRsc->VcmpV.LdC * (LONG)AxisRsc->IntAdwk.swk1 ) , 15 );	/* TMP3 <-- ACC >> 15										*/
			AxisRsc->IntAdwk.swk0 = (SHORT)IlibASR32( ( (LONG)AxisRsc->IntAdP.MotResist * (LONG)AxisRsc->IntAdwk.swk2 ) , 15 );
			AxisRsc->IntAdwk.swk3 = AxisRsc->IntAdwk.swk3 + AxisRsc->VcmpV.MagC;
			AxisRsc->IntAdwk.swk5 = AxisRsc->IntAdwk.swk3 + AxisRsc->IntAdwk.swk0;	/* VcmpV.VqComp <-- VcmpV.MagC + TMP3 + TMP0				*/
/*--------------------------------------------------------------------------------------------------*/
/*		if(IntAdP.CtrlSw & DIDTSET) VcmpV.VdComp = TMP4 + KDD * (IntAdV.IdDataP - IntAdV.IdInData), IntAdV.IdDataP=IntAdV.IdInData									*/
/*						  VcmpV.VqComp = TMP5 + KQD * (IntAdV.IqDataP - IntAdV.IqRef), IntAdV.IqDataP=IntAdV.IqRef								*/
/*--------------------------------------------------------------------------------------------------*/
			if( (AxisRsc->IntAdP.CtrlSw & DIDTSEL) == 0 )
			{
				AxisRsc->VcmpV.VdComp = AxisRsc->IntAdwk.swk4;	/* 															*/
				AxisRsc->VcmpV.VqComp = AxisRsc->IntAdwk.swk5;	/* 															*/
			}
/*--------------------------------------------------------------------------------------------------*/
/*		filter : I*FL =  ( ( ( TMP1 - I*FH ) * IntAdP.Tfil ) << 2 ) + I*FL									*/
/*--------------------------------------------------------------------------------------------------*/
			else
			{
				AxisRsc->IntAdwk.swk1 = AxisRsc->WeakFV.IdOut;	/* 															*/
#ifdef	WIN32
				IxADDSUBLMTCHKRDY( AxisRsc->IntAdwk.swk1, AxisRsc->IntAdV.IdLfil.s[1] );
#endif
				AxisRsc->IntAdwk.swk1 = AxisRsc->IntAdwk.swk1 - AxisRsc->IntAdV.IdLfil.s[1];	/* 											*/
#ifdef	WIN32
				IxSUBLMTCHK( AxisRsc->IntAdwk.swk1 );
#endif
				AxisRsc->IntAdwk.swk1 = IxLmtCBS16( AxisRsc->IntAdwk.swk1 );	/* 													*/
				AxisRsc->IntAdwk.lwk0 = ( (LONG)AxisRsc->IntAdP.Tfil * (LONG)AxisRsc->IntAdwk.swk1 ) << 2;	/* 														*/
#ifdef	WIN32
				IxADDSUBLMTCHKRDY( AxisRsc->IntAdV.IdLfil.l, AxisRsc->IntAdwk.lwk0 );
#endif
				AxisRsc->IntAdwk.lwk2 = AxisRsc->IntAdV.IdLfil.l + AxisRsc->IntAdwk.lwk0;	/* 												*/
#ifdef	WIN32
				IxADDLMTCHK( AxisRsc->IntAdwk.lwk2 );
#endif
				AxisRsc->IntAdV.IdLfil.l = IxLmtCBS32( AxisRsc->IntAdwk.lwk2 );	/* 														*/
/*--------------------------------------------------------------------------------------------------*/
				AxisRsc->IntAdwk.swk1 = AxisRsc->IntAdV.IqRef;	/* 														*/
#ifdef	WIN32
				IxADDSUBLMTCHKRDY( AxisRsc->IntAdwk.swk1, AxisRsc->IntAdV.IqLfil.s[1] );
#endif
				AxisRsc->IntAdwk.swk1 = AxisRsc->IntAdwk.swk1 - AxisRsc->IntAdV.IqLfil.s[1];	/* 											*/
#ifdef	WIN32
				IxSUBLMTCHK( AxisRsc->IntAdwk.swk1 );
#endif
				AxisRsc->IntAdwk.swk1 = IxLmtCBS16( AxisRsc->IntAdwk.swk1 );	/* 													*/
				IxNop( );						/* 															*/
				AxisRsc->IntAdwk.lwk0 = ( (LONG)AxisRsc->IntAdP.Tfil * (LONG)AxisRsc->IntAdwk.swk1 ) << 2;	/* 														*/
#ifdef	WIN32
				IxADDSUBLMTCHKRDY( AxisRsc->IntAdV.IqLfil.l, AxisRsc->IntAdwk.lwk0 );
#endif
				AxisRsc->IntAdwk.lwk2 = AxisRsc->IntAdV.IqLfil.l + AxisRsc->IntAdwk.lwk0;	/* 												*/
#ifdef	WIN32
				IxADDLMTCHK( AxisRsc->IntAdwk.lwk2 );
#endif
				AxisRsc->IntAdV.IqLfil.l = IxLmtCBS32( AxisRsc->IntAdwk.lwk2 );	/* 														*/
/* ------------------------------------------------------------------------------------------------ */
				AxisRsc->IntAdwk.swk2 = AxisRsc->IntAdV.IdLfil.s[1] - AxisRsc->IntAdV.IdDataP;	/* 												*/
				AxisRsc->IntAdV.IdDataP = AxisRsc->IntAdV.IdLfil.s[1];	/* 															*/
				AxisRsc->IntAdwk.swk2 = (SHORT)IlibASR32(( (LONG)AxisRsc->IntAdP.L_dIdt * (LONG)AxisRsc->IntAdwk.swk2 ) , 9 );	/* 															*/
				AxisRsc->IntAdwk.swk2 = IxLmtCBS16( AxisRsc->IntAdwk.swk2 );	/* limit( VDL , 2^15 - 1 )							*/
#ifdef	WIN32
				IxADDSUBLMTCHKRDY( AxisRsc->IntAdwk.swk2, AxisRsc->IntAdwk.swk4 );
#endif
				AxisRsc->IntAdwk.swk0 = AxisRsc->IntAdwk.swk2 + AxisRsc->IntAdwk.swk4;	/* VcmpV.VdComp <-- TMP4 + TMP3							*/
#ifdef	WIN32
				IxADDLMTCHK( AxisRsc->IntAdwk.swk0 );
#endif
				AxisRsc->VcmpV.VdComp = IxLmtCBS16( AxisRsc->IntAdwk.swk0 );	/* VcmpV.VdComp <-- limit( VcmpV.VdOut , 2^15 - 1 )							*/
/*--------------------------------------------------------------------------------------------------*/
				AxisRsc->IntAdwk.swk2 = AxisRsc->IntAdV.IqLfil.s[1] - AxisRsc->IntAdV.IqDataP;	/* 												*/
				AxisRsc->IntAdV.IqDataP = AxisRsc->IntAdV.IqLfil.s[1];
				AxisRsc->IntAdwk.swk2 = (SHORT)IlibASR32( ( (LONG)AxisRsc->IntAdP.L_dIdt * (LONG)AxisRsc->IntAdwk.swk2 ) , 9 );	/* 															*/
				AxisRsc->IntAdwk.swk2 = IxLmtCBS16( AxisRsc->IntAdwk.swk2 );	/* limit( VQL , 2^15 - 1 )							*/
#ifdef	WIN32
				IxADDSUBLMTCHKRDY( AxisRsc->IntAdwk.swk2, AxisRsc->IntAdwk.swk5 );
#endif
				AxisRsc->IntAdwk.swk0 = AxisRsc->IntAdwk.swk2 + AxisRsc->IntAdwk.swk5;	/* VcmpV.VqComp <-- TMP5 + TMP3						*/
#ifdef	WIN32
				IxADDLMTCHK( AxisRsc->IntAdwk.swk0 );
#endif
				AxisRsc->VcmpV.VqComp = IxLmtCBS16( AxisRsc->IntAdwk.swk0 );	/* VcmpV.VqComp <-- limit( VcmpV.VqOut , 2^15 - 1 )							*/
			}
/*--------------------------------------------------------------------------------------------------*/
/*		TMP1 = limit( VDFH + VcmpV.VdComp , 2^15 - 1 )														*/
/*		TMP2 = limit( VQFH + VcmpV.VqComp , 2^15 - 1 )														*/
/*--------------------------------------------------------------------------------------------------*/
#ifdef	WIN32
			IxADDSUBLMTCHKRDY( AxisRsc->AcrV.VdFil.s[1], AxisRsc->VcmpV.VdComp );
#endif
			AxisRsc->IntAdwk.swk0 = AxisRsc->AcrV.VdFil.s[1] + AxisRsc->VcmpV.VdComp;	/* VcmpV.VdOut <-- VDFH + VcmpV.VdComp								*/
#ifdef	WIN32
			IxADDLMTCHK( AxisRsc->IntAdwk.swk0 );
#endif
			AxisRsc->IntAdwk.swk1 = IxLmtCBS16( AxisRsc->IntAdwk.swk0 );	/* VcmpV.VdOut <-- limit( VcmpV.VdOut , 2^15 - 1 )						*/
#ifdef	WIN32
			IxADDSUBLMTCHKRDY( AxisRsc->AcrV.VqFil.s[1], AxisRsc->VcmpV.VqComp );
#endif
			AxisRsc->IntAdwk.swk0 = AxisRsc->AcrV.VqFil.s[1] + AxisRsc->VcmpV.VqComp;	/* VcmpV.VqOut <-- VQFH + VcmpV.VqComp								*/
#ifdef	WIN32
			IxADDLMTCHK( AxisRsc->IntAdwk.swk0 );
#endif
			AxisRsc->IntAdwk.swk2 = IxLmtCBS16( AxisRsc->IntAdwk.swk0 );	/* VcmpV.VqOut <-- limit( VcmpV.VqOut , 2^15 - 1 )						*/
/*--------------------------------------------------------------------------------------------------*/
/*		TMP1 = limit( VcmpV.VdRef + TMP1 , 2^15 - 1 )														*/
/*		TMP2 = limit( VcmpV.VqRef + TMP2 , 2^15 - 1 )														*/
/*--------------------------------------------------------------------------------------------------*/
#ifdef	WIN32
			IxADDSUBLMTCHKRDY( AxisRsc->VcmpV.VdRef, AxisRsc->IntAdwk.swk1 );
#endif
			AxisRsc->IntAdwk.swk0 = AxisRsc->VcmpV.VdRef + AxisRsc->IntAdwk.swk1;	/* VcmpV.VdOut <-- VcmpV.VdRef + TMP1							*/
#ifdef	WIN32
			IxADDLMTCHK( AxisRsc->IntAdwk.swk0 );
#endif
			AxisRsc->IntAdwk.swk1 = IxLmtCBS16( AxisRsc->IntAdwk.swk0 );	/* VcmpV.VdOut <-- limit( VcmpV.VdOut , 2^15 - 1 )						*/
#ifdef	WIN32
			IxADDSUBLMTCHKRDY( AxisRsc->VcmpV.VqRef, AxisRsc->IntAdwk.swk2 );
#endif
			AxisRsc->IntAdwk.swk0 = AxisRsc->VcmpV.VqRef + AxisRsc->IntAdwk.swk2;	/* VcmpV.VqOut <-- VcmpV.VqRef + TMP2								*/
#ifdef	WIN32
			IxADDLMTCHK( AxisRsc->IntAdwk.swk0 );
#endif
			AxisRsc->IntAdwk.swk2 = IxLmtCBS16( AxisRsc->IntAdwk.swk0 );	/* VcmpV.VqOut <-- limit( VcmpV.VqOut , 2^15 - 1 )						*/
/*--------------------------------------------------------------------------------------------------*/
/*		VcmpV.VdOut = limit( IntAdP.Kvv * TMP1 / 2^13 , 2^15 - 1 )													*/
/*		VcmpV.VqOut = limit( IntAdP.Kvv * TMP2 / 2^13 , 2^15 - 1 )													*/
/*--------------------------------------------------------------------------------------------------*/
			AxisRsc->IntAdwk.swk1 = (SHORT)IlibASR32( ( (LONG)AxisRsc->IntAdP.Kvv * (LONG)AxisRsc->IntAdwk.swk1 ) , 13 );	/* TMP1 <-- ACC >> 13										*/
			AxisRsc->VcmpV.VdOut = IxLmtCBS16( AxisRsc->IntAdwk.swk1 );	/* VcmpV.VdOut   <-- limit( TMP1 , 2^15 - 1 )						*/
			AxisRsc->IntAdwk.swk2 = (SHORT)IlibASR32( ( (LONG)AxisRsc->IntAdP.Kvv * (LONG)AxisRsc->IntAdwk.swk2 ) , 13 );		/* TMP2 <-- ACC >> 13										*/
			AxisRsc->VcmpV.VqOut = IxLmtCBS16( AxisRsc->IntAdwk.swk2 );		/* VcmpV.VqOut   <-- limit( TMP2 , 2^15 - 1 )						*/
			AxisRsc->WeakFV.WfVdRef = AxisRsc->VcmpV.VdOut;		/* d���d���w�ߕۑ�			<V531>							*/
			AxisRsc->WeakFV.WfVqRef = AxisRsc->VcmpV.VqOut;		/* q���d���w�ߕۑ�			<V531>							*/

/****************************************************************************************************/
/*		�d���x�N�g���␳�l�v�Z			<V537> �V��ߊE������ȊO�͂��̏������W�����v����			*/
/****************************************************************************************************/
			if( (AxisRsc->IntAdP.CtrlSw & V_FB2) != 0 )
			{
/****************************************************************************************************/
/*		Get modulation							<V531> �ϒ����v�Z���ړ�								*/
/****************************************************************************************************/
				AxisRsc->IntAdwk.lwk2 = (LONG)AxisRsc->VcmpV.VdOut * (LONG)AxisRsc->VcmpV.VdOut;
				AxisRsc->IntAdwk.lwk4 = (LONG)AxisRsc->VcmpV.VqOut * (LONG)AxisRsc->VcmpV.VqOut;
				AxisRsc->IntAdwk.lwk2 = AxisRsc->IntAdwk.lwk2 + AxisRsc->IntAdwk.lwk4;	/* TMP2 = VcmpV.VdOut^2 + VcmpV.VqOut^2							*/
				AxisRsc->IntAdwk.swk0 = MpSQRT( &AxisRsc->IntAdwk, AxisRsc->IntAdwk.lwk2 );					/* TMP0 = ��(VcmpV.VdOut^2 + VcmpV.VqOut^2)									*/
				AxisRsc->IntAdV.V1 = AxisRsc->IntAdwk.swk0;		/* IntAdV.V1   = TMP0												*/
/****************************************************************************************************/
/*		�O�a���f							<V531> IntAdV.V1 > 8192*127%(10403.8) -> �O�a���				*/
/****************************************************************************************************/
				AxisRsc->VcmpV.Vmax2 = 10403;	/* VcmpV.Vmax2 = 8192 * 1.27										*/
				AxisRsc->VcmpV.V12 = AxisRsc->IntAdV.V1;			/* VcmpV.V12 = ��(VcmpV.VdOut^2 + VcmpV.VqOut^2)									*/
				if( AxisRsc->IntAdV.V1 < 0 )
				{
					AxisRsc->VcmpV.Vmax2 = AxisRsc->VcmpV.Vmax2 >> 1;	/* VcmpV.Vmax2 = 8192 * 1.27 / 2									*/
					AxisRsc->VcmpV.V12 = AxisRsc->IntAdV.V1 >> 1;		/* VcmpV.V12 = ��(VcmpV.VdOut^2 + VcmpV.VqOut^2) / 2								*/
				}
				if( AxisRsc->VcmpV.Vmax2 < AxisRsc->VcmpV.V12 )
				{
					AxisRsc->IntAdV.V1 = 10403;		/* IntAdV.V1 = IntAdP.Vmax( 8192 * 1.27 )									*/
					AxisRsc->StsFlg.IntglFlg = AxisRsc->StsFlg.IntglFlg | 1;			/* �ϕ���~�t���O�Z�b�g										*/
//;****************************************************************************************************
//;*		�d���x�N�g���␳�l�v�Z				<V531> VcmpV.VdOut',VcmpV.VqOut' = IntAdP.Vmax / IntAdV.V1 * VcmpV.VdOut, VcmpV.VqOut	<V537> �폜			*
//;****************************************************************************************************
/*--------------------------------------------------------------------------------------------------*/
/*		�d�������e�[�u���A�h���X�擾																*/
/*--------------------------------------------------------------------------------------------------*/
					AxisRsc->IntAdwk.lwk2 = (LONG)AxisRsc->VcmpV.V12 * (LONG)AxisRsc->VcmpV.V12;	/* TMP3,2 = VcmpV.V12^2										*/
					AxisRsc->IntAdwk.lwk2 = AxisRsc->IntAdwk.lwk2 -  0x00400000;	/* TMP3,2 = IntAdV.V1^2 - 2^22									*/
					AxisRsc->IntAdwk.lwk2 = AxisRsc->IntAdwk.lwk2 >> 4;	/* TMP3,2 = (VcmpV.V12^2 - 2^22) / 2^4						*/
					IxNop( );
					IxNop( );
					AxisRsc->IntAdwk.swk0 = (USHORT)( AxisRsc->IntAdwk.lwk2 >> 16 );	/* TMP0 = (VcmpV.V12^2 - 2^22) / 2^4 / 2^16 = addr			*/
					AxisRsc->IntAdwk.lwk2 = AxisRsc->IntAdwk.lwk2 & 0x0000ffff;	/* TMP2 = { (VcmpV.V12^2 - 2^22) / 2^4 } & 0x0000ffff				*/
/*--------------------------------------------------------------------------------------------------*/
/*		�d�������x�N�g��������ԗp�f�[�^�擾														*/
/*--------------------------------------------------------------------------------------------------*/
					AxisRsc->IntAdwk.lwk4 = 65536;	/* TMP5,TMP4 = 65536										*/
					AxisRsc->IntAdwk.lwk6 = AxisRsc->IntAdwk.lwk4 - AxisRsc->IntAdwk.lwk2;	/* TMP7,6 = 10000h - Table Index (Lo) -> (addr*2^16-low)	*/
					IxTblVlmt16( &AxisRsc->IntAdwk.swk8, AxisRsc->IntAdwk.swk0 );	/* TMP8 : �e�[�u���f�[�^�ǂݏo��(�ǂݏo���A�h���Xaddr)	*//* tanaka21,�R���p�C���Ή��҂�		*/
					AxisRsc->IntAdwk.lwk6 = (LONG)AxisRsc->IntAdwk.swk8 * AxisRsc->IntAdwk.lwk6;	/* TMP6 = tblrv(addr)*(2^16-low)						*/
					AxisRsc->IntAdwk.swk0 = AxisRsc->IntAdwk.swk0 + 1;		/* TMP0 = addr+1											*/
					IxTblVlmt16( &AxisRsc->IntAdwk.swk8, AxisRsc->IntAdwk.swk0 );	/* TMP8 : �e�[�u���f�[�^�ǂݏo��(�ǂݏo���A�h���Xaddr+1)	*//* tanaka21,�R���p�C���Ή��҂�		*/
					AxisRsc->IntAdwk.lwk4 = (LONG)AxisRsc->IntAdwk.swk8 * AxisRsc->IntAdwk.lwk2;	/* TMP4 = tblrv(addr+1)*low								*/
					AxisRsc->IntAdwk.lwk0 = AxisRsc->IntAdwk.lwk6 + AxisRsc->IntAdwk.lwk4;	/* TMP0 = tblrv(addr)*(2^16-low) + tblrv(addr+1)*low	*/
/*--------------------------------------------------------------------------------------------------*/
/*		�d���d���x�N�g���␳�l�v�Z																	*/
/*--------------------------------------------------------------------------------------------------*/
					AxisRsc->IntAdwk.swk8 = AxisRsc->VcmpV.Vmax2;	/* TMP8 = VcmpV.Vmax2												*/
//#ifdef WIN32
					AxisRsc->IntAdwk.lwk2 = (LONG)IlibASR64( ( (INT64)AxisRsc->IntAdwk.swk8 * (INT64)AxisRsc->IntAdwk.lwk0 ) , 28 );		/* TMP2 = MAC / 2^28										*/
					AxisRsc->VcmpV.VdOut = (SHORT)IlibASR32( ( (LONG)AxisRsc->IntAdwk.swk2 * (LONG)AxisRsc->VcmpV.VdOut ) , 14 );			/* VcmpV.VdOut = IntAdP.Vmax / VcmpV.V12 * VcmpV.VdOut * 2^(13+13+16) / 2^(28+14)			*/
					AxisRsc->VcmpV.VqOut = (SHORT)IlibASR32( ( (LONG)AxisRsc->IntAdwk.swk2 * (LONG)AxisRsc->VcmpV.VqOut ) , 14 );			/* VcmpV.VqOut = IntAdP.Vmax / VcmpV.V12 * VcmpV.VqOut * 2^(13+13+16) / 2^(28+14)			*/
//#elif defined(ASIP_CC)
//					AxisRsc->IntAdwk.lwk2 = asr( AxisRsc->IntAdwk.swk8 * AxisRsc->IntAdwk.lwk0, 28 );		/* TMP2 = MAC / 2^28										*/
//					AxisRsc->VcmpV.VdOut = asr( AxisRsc->IntAdwk.swk2 * AxisRsc->VcmpV.VdOut, 14 );		/* VcmpV.VdOut = IntAdP.Vmax / VcmpV.V12 * VcmpV.VdOut * 2^(13+13+16) / 2^(28+14)			*/
//					AxisRsc->VcmpV.VqOut = asr( AxisRsc->IntAdwk.swk2 * AxisRsc->VcmpV.VqOut, 14 );		/* VcmpV.VqOut = IntAdP.Vmax / VcmpV.V12 * VcmpV.VqOut * 2^(13+13+16) / 2^(28+14)			*/
//#endif
				}
				else
				{
					AxisRsc->StsFlg.IntglFlg = AxisRsc->StsFlg.IntglFlg & 0xFFFE;		/* �ϕ���~�t���O�N���A										*/
				}
			}

/****************************************************************************************************/
/*																									*/
/*		UVW transform : dq( 2phase ) to UVW( 3phase ) Transform										*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		VcmpV.VuOut = limit( SinTbl.CosT * VcmpV.VdOut / 2^14 - SinTbl.SinT * VcmpV.VqOut / 2^14 , 2^15 - 1 )								*/
/*--------------------------------------------------------------------------------------------------*/
			AxisRsc->IntAdwk.swk4 = AxisRsc->IntAdP.Vmax;	/* 															*/
			AxisRsc->IntAdwk.swk1 = (SHORT)IlibASR32( ( (LONG)AxisRsc->SinTbl.CosT * (LONG)AxisRsc->VcmpV.VdOut ) , 14 );	/* TMP1 <-- ACC >> 14										*/
			AxisRsc->IntAdwk.swk2 = (SHORT)IlibASR32( ( (LONG)AxisRsc->SinTbl.SinT * (LONG)AxisRsc->VcmpV.VqOut ) , 14 );		/* TMP2 <-- ACC >> 14										*/
			IxNop( );						/* 															*/
#ifdef	WIN32
			IxADDSUBLMTCHKRDY( AxisRsc->IntAdwk.swk1, AxisRsc->IntAdwk.swk2 );
#endif
			AxisRsc->VcmpV.VuOut = AxisRsc->IntAdwk.swk1 - AxisRsc->IntAdwk.swk2;	/* VcmpV.VuOut <-- TMP1 - TMP2								*/
#ifdef	WIN32
			IxSUBLMTCHK( AxisRsc->VcmpV.VuOut );
#endif
			AxisRsc->VcmpV.VuOut = IxLmtCBS16( AxisRsc->VcmpV.VuOut );			/* VcmpV.VuOut <-- limit( VcmpV.VuOut , 2^15 - 1 )							*/
			AxisRsc->VcmpV.VuOut = IxLIMIT( AxisRsc->VcmpV.VuOut, AxisRsc->IntAdwk.swk4 );	/* 															*/
/*--------------------------------------------------------------------------------------------------*/
/*		VcmpV.VvOut = limit( SinTbl.CosT3 * VcmpV.VdOut / 2^14 - SinTbl.SinT3 * VcmpV.VqOut / 2^14 , 2^15 - 1 )								*/
/*--------------------------------------------------------------------------------------------------*/
			AxisRsc->IntAdwk.swk1 = (SHORT)IlibASR32( ( (LONG)AxisRsc->SinTbl.CosT3 * (LONG)AxisRsc->VcmpV.VdOut ) , 14 );	/* TMP1 <-- ACC >> 14										*/
			AxisRsc->IntAdwk.swk2 = (SHORT)IlibASR32( ( (LONG)AxisRsc->SinTbl.SinT3 * (LONG)AxisRsc->VcmpV.VqOut ) , 14 );		/* TMP2 <-- ACC >> 14										*/
			IxNop( );						/* 															*/
#ifdef	WIN32
			IxADDSUBLMTCHKRDY( AxisRsc->IntAdwk.swk1, AxisRsc->IntAdwk.swk2 );
#endif
			AxisRsc->VcmpV.VvOut = AxisRsc->IntAdwk.swk1 - AxisRsc->IntAdwk.swk2;	/* VcmpV.VvOut <-- TMP1 - TMP2								*/
#ifdef	WIN32
			IxSUBLMTCHK( AxisRsc->VcmpV.VvOut );
#endif
			AxisRsc->VcmpV.VvOut = IxLmtCBS16(AxisRsc-> VcmpV.VvOut );			/* VcmpV.VvOut <-- limit( VcmpV.VvOut , 2^15 - 1 )							*/
			AxisRsc->VcmpV.VvOut = IxLIMIT( AxisRsc->VcmpV.VvOut, AxisRsc->IntAdwk.swk4 );	/* 															*/
/*--------------------------------------------------------------------------------------------------*/
/*		VcmpV.VwOut = limit( - VcmpV.VuOut - VcmpV.VvOut , 2^15 - 1 )															*/
/*--------------------------------------------------------------------------------------------------*/
			AxisRsc->IntAdwk.swk1 = (SHORT)ZEROR - AxisRsc->VcmpV.VuOut;	/* VcmpV.VwOut <-- - VcmpV.VuOut - VcmpV.VvOut									*/
#ifdef	WIN32
			IxADDSUBLMTCHKRDY( AxisRsc->IntAdwk.swk1, AxisRsc->VcmpV.VvOut );
#endif
			AxisRsc->VcmpV.VwOut = AxisRsc->IntAdwk.swk1 - AxisRsc->VcmpV.VvOut;
#ifdef	WIN32
			IxSUBLMTCHK( AxisRsc->VcmpV.VwOut );
#endif
			AxisRsc->VcmpV.VwOut = IxLmtCBS16( AxisRsc->VcmpV.VwOut );			/* VcmpV.VwOut <-- limit( VcmpV.VwOut , 2^15 - 1 )							*/
			AxisRsc->VcmpV.VwOut = IxLIMIT( AxisRsc->VcmpV.VwOut, AxisRsc->IntAdwk.swk4 );	/* 															*/



#if 0 /* for ROMSIM debug */


/****************************************************************************************************/
/*		�V��ߊE�����䔻�f����		<V537> �V��ߊE���̏ꍇ�ϒ����v�Z�C�O�a���f�������W�����v����	*/
/****************************************************************************************************/
			if( (AxisRsc->IntAdP.CtrlSw & V_FB2) == 0 )
			{
/****************************************************************************************************/
/*		Get modulation					<V531> �ϒ����v�Z��2��3���ϊ��O�ɂ��� <V537> ����			*/
/****************************************************************************************************/
				AxisRsc->IntAdwk.lwk2 = (LONG)AxisRsc->VcmpV.VdOut * (LONG)AxisRsc->VcmpV.VdOut;
				AxisRsc->IntAdwk.lwk4 = (LONG)AxisRsc->VcmpV.VqOut * (LONG)AxisRsc->VcmpV.VqOut;
				AxisRsc->IntAdwk.lwk2 = AxisRsc->IntAdwk.lwk2 + AxisRsc->IntAdwk.lwk4;
				AxisRsc->IntAdwk.swk0 = MpSQRT( &AxisRsc->IntAdwk, AxisRsc->IntAdwk.lwk2 );
				if( (USHORT)AxisRsc->IntAdwk.swk0 > 0x7FFF )
				{
					AxisRsc->IntAdwk.swk0 = 0x7FFF;	/* ��̌v�Z��32767�𒴂�����A32767�ɂ���B					; <V350>	*/
				}
				AxisRsc->IntAdV.V1 = AxisRsc->IntAdwk.swk0;
/*--------------------------------------------------------------------------------------------------*/
/*		�O�a���f						<V531> <V537> ����											*/
/*--------------------------------------------------------------------------------------------------*/
				if( AxisRsc->IntAdV.V1 >= 9421 )
				{
					AxisRsc->StsFlg.IntglFlg = AxisRsc->StsFlg.IntglFlg | 1;			/* 															*/
				}
				else
				{
					AxisRsc->StsFlg.IntglFlg = AxisRsc->StsFlg.IntglFlg & 0xFFFE;		/* 															*/
				}
			}
/****************************************************************************************************/
/*		Over modulation type select																	*/
/****************************************************************************************************/
			if( AxisRsc->IntAdP.Vmax >= 0x2000 )
			{
				if( (AxisRsc->IntAdP.CtrlSw & OVMSEL2) == 0 )
				{
					if( ( AxisRsc->IntAdV.V1 >= 0x2000 )||( (AxisRsc->IntAdP.CtrlSw & OVMSEL1) != 0 ) )
					{
/****************************************************************************************************/
/*		Over modulation1																			*/
/****************************************************************************************************/
//						IxSetCtblAdr( IntAdwk.pCtbl, &OVMODTBLG[0][0] );	/* gain type										*/
						IxSetCtblAdr( AxisRsc->IntAdwk.pCtbl, &(OVMODTBLG[0][0]) );	/* gain type										*/
						MpOVMMODK( &AxisRsc->IntAdP, &AxisRsc->IntAdV, &AxisRsc->IntAdwk );
						AxisRsc->VcmpV.VuOut = (SHORT)IlibASR32( ( (LONG)AxisRsc->VcmpV.VuOut * (LONG)AxisRsc->IntAdP.Kmod ) , 13 );
						AxisRsc->VcmpV.VuOut = IxLmtCBS16( AxisRsc->VcmpV.VuOut );
						AxisRsc->VcmpV.VvOut = (SHORT)IlibASR32( ( (LONG)AxisRsc->VcmpV.VvOut * (LONG)AxisRsc->IntAdP.Kmod ) , 13 );
						AxisRsc->VcmpV.VvOut = IxLmtCBS16( AxisRsc->VcmpV.VvOut );
						AxisRsc->VcmpV.VwOut = (SHORT)IlibASR32( ( (LONG)AxisRsc->VcmpV.VwOut * (LONG)AxisRsc->IntAdP.Kmod ) , 13 );
						AxisRsc->VcmpV.VwOut = IxLmtCBS16( AxisRsc->VcmpV.VwOut );
/*--------------------------------------------------------------------------------------------------*/
/*		TMP1 = |VcmpV.VuOut|,     TMP2 = |VcmpV.VvOut|,     TMP3 = |VcmpV.VwOut|					*/
/*		TMP4 = sign(VcmpV.VuOut), TMP5 = sign(VcmpV.VvOut), TMP6 = sign(VcmpV.VwOut)				*/
/*--------------------------------------------------------------------------------------------------*/
						AxisRsc->IntAdwk.swk0 = 1;
						AxisRsc->IntAdwk.swk4 = IxLIMIT( AxisRsc->VcmpV.VuOut, AxisRsc->IntAdwk.swk0 );
						AxisRsc->IntAdwk.swk1 = (SHORT)( (LONG)AxisRsc->IntAdwk.swk4 * (LONG)AxisRsc->VcmpV.VuOut );
						AxisRsc->IntAdwk.swk5 = IxLIMIT( AxisRsc->VcmpV.VvOut, AxisRsc->IntAdwk.swk0 );
						AxisRsc->IntAdwk.swk2 = (SHORT)( (LONG)AxisRsc->IntAdwk.swk5 * (LONG)AxisRsc->VcmpV.VvOut );
						AxisRsc->IntAdwk.swk6 = IxLIMIT( AxisRsc->VcmpV.VwOut, AxisRsc->IntAdwk.swk0 );
						AxisRsc->IntAdwk.swk3 = (SHORT)( (LONG)AxisRsc->IntAdwk.swk6 * (LONG)AxisRsc->VcmpV.VwOut );
						if( AxisRsc->IntAdwk.swk1 >= AxisRsc->IntAdwk.swk2 )
						{
							if( AxisRsc->IntAdwk.swk1 >= AxisRsc->IntAdwk.swk3 )
							{
#ifdef	WIN32
								IxADDSUBLMTCHKRDY( AxisRsc->IntAdwk.swk1, 0x2000 );
#endif
								AxisRsc->IntAdwk.swk1 = AxisRsc->IntAdwk.swk1 - 0x2000;	/* TMP1 <-- |VcmpV.VuOut|-2000h									*/
#ifdef	WIN32
								IxSUBLMTCHK( AxisRsc->IntAdwk.swk1 );
#endif
								IxLmtzImm16( AxisRsc->IntAdwk.swk1, 0x7fff );	/* zero limit											*/
								AxisRsc->IntAdwk.swk0 = (SHORT)( (LONG)AxisRsc->IntAdwk.swk4 * (LONG)AxisRsc->IntAdwk.swk1 );
							}
							else
							{
#ifdef	WIN32
								IxADDSUBLMTCHKRDY( AxisRsc->IntAdwk.swk3, 0x2000 );
#endif
								AxisRsc->IntAdwk.swk3 = AxisRsc->IntAdwk.swk3 - 0x2000;	/* TMP0 <-- |VcmpV.VwOut|-2000h									*/
#ifdef	WIN32
								IxSUBLMTCHK( AxisRsc->IntAdwk.swk3 );
#endif
								IxLmtzImm16( AxisRsc->IntAdwk.swk3, 0x7fff );	/* zero limit											*/
								AxisRsc->IntAdwk.swk0 = (SHORT)( (LONG)AxisRsc->IntAdwk.swk6 * (LONG)AxisRsc->IntAdwk.swk3 );
							}
						}
						else
						{
							if( AxisRsc->IntAdwk.swk2 >= AxisRsc->IntAdwk.swk3 )
							{
#ifdef	WIN32
								IxADDSUBLMTCHKRDY( AxisRsc->IntAdwk.swk2, 0x2000 );
#endif
								AxisRsc->IntAdwk.swk2 = AxisRsc->IntAdwk.swk2 - 0x2000;	/* TMP0 <-- |VcmpV.VvOut|-2000h										*/
#ifdef	WIN32
								IxSUBLMTCHK( AxisRsc->IntAdwk.swk2 );
#endif
								IxLmtzImm16( AxisRsc->IntAdwk.swk2, 0x7fff );	/* zero limit											*/
								AxisRsc->IntAdwk.swk0 = (SHORT)( (LONG)AxisRsc->IntAdwk.swk5 * (LONG)AxisRsc->IntAdwk.swk2 );
							}
							else
							{
#ifdef	WIN32
								IxADDSUBLMTCHKRDY( AxisRsc->IntAdwk.swk3, 0x2000 );
#endif
								AxisRsc->IntAdwk.swk3 = AxisRsc->IntAdwk.swk3 - 0x2000;	/* TMP0 <-- |VcmpV.VwOut|-2000h									*/
#ifdef	WIN32
								IxSUBLMTCHK( AxisRsc->IntAdwk.swk3 );
#endif
								IxLmtzImm16( AxisRsc->IntAdwk.swk3, 0x7fff );	/* zero limit											*/
								AxisRsc->IntAdwk.swk0 = (SHORT)( (LONG)AxisRsc->IntAdwk.swk6 * (LONG)AxisRsc->IntAdwk.swk3 );
							}
						}
#ifdef	WIN32
						IxADDSUBLMTCHKRDY( AxisRsc->VcmpV.VuOut, AxisRsc->IntAdwk.swk0 );
#endif
						AxisRsc->VcmpV.VuOut = AxisRsc->VcmpV.VuOut - AxisRsc->IntAdwk.swk0;
#ifdef	WIN32
						IxSUBLMTCHK( AxisRsc->VcmpV.VuOut );
#endif
						AxisRsc->VcmpV.VuOut = IxLmtCBS16( AxisRsc->VcmpV.VuOut );			/* 															*/
#ifdef	WIN32
						IxADDSUBLMTCHKRDY( AxisRsc->VcmpV.VvOut, AxisRsc->IntAdwk.swk0 );
#endif
						AxisRsc->VcmpV.VvOut = AxisRsc->VcmpV.VvOut - AxisRsc->IntAdwk.swk0;
#ifdef	WIN32
						IxSUBLMTCHK( AxisRsc->VcmpV.VvOut );
#endif
						AxisRsc->VcmpV.VvOut = IxLmtCBS16( AxisRsc->VcmpV.VvOut );			/* 															*/
#ifdef	WIN32
						IxADDSUBLMTCHKRDY( AxisRsc->VcmpV.VwOut, AxisRsc->IntAdwk.swk0 );
#endif
						AxisRsc->VcmpV.VwOut = AxisRsc->VcmpV.VwOut - AxisRsc->IntAdwk.swk0;
#ifdef	WIN32
						IxSUBLMTCHK( AxisRsc->VcmpV.VwOut );
#endif
						AxisRsc->VcmpV.VwOut = IxLmtCBS16( AxisRsc->VcmpV.VwOut );			/* 															*/
						AxisRsc->IntAdV.Vcent = AxisRsc->IntAdwk.swk0;
					}
				}
/****************************************************************************************************/
/*		Over modulation2																			*/
/****************************************************************************************************/
				else
				{
//					IxSetCtblAdr( AxisRsc->IntAdwk.pCtbl, &(OVMODTBLO) );	/* ofset type										*/
					IxSetCtblAdr( AxisRsc->IntAdwk.pCtbl, &(OVMODTBLO[0][0]) );	/* ofset type										*/
					MpOVMMODK( &AxisRsc->IntAdP, &AxisRsc->IntAdV, &AxisRsc->IntAdwk );
/*--------------------------------------------------------------------------------------------------*/
/*		MAX = TMP1, MIN = TMP2																		*/
/*		OFS = (TMP1+TMP2)/2																			*/
/*--------------------------------------------------------------------------------------------------*/
					if( AxisRsc->VcmpV.VuOut >= AxisRsc->VcmpV.VvOut )
					{
						AxisRsc->IntAdwk.swk1 = AxisRsc->VcmpV.VuOut;
						AxisRsc->IntAdwk.swk2 = AxisRsc->VcmpV.VvOut;
					}
					else
					{
						AxisRsc->IntAdwk.swk1 = AxisRsc->VcmpV.VvOut;
						AxisRsc->IntAdwk.swk2 = AxisRsc->VcmpV.VuOut;
					}
					if( AxisRsc->IntAdwk.swk1 < AxisRsc->VcmpV.VwOut )
					{
						AxisRsc->IntAdwk.swk1 = AxisRsc->VcmpV.VwOut;
					}
					else
					{
						if( AxisRsc->VcmpV.VwOut < AxisRsc->IntAdwk.swk2 )
						{
							AxisRsc->IntAdwk.swk2 = AxisRsc->VcmpV.VwOut;
						}
					}
#ifdef	WIN32
					IxADDSUBLMTCHKRDY( AxisRsc->IntAdwk.swk2, AxisRsc->IntAdwk.swk1 );
#endif
					AxisRsc->IntAdwk.swk0 = AxisRsc->IntAdwk.swk2 + AxisRsc->IntAdwk.swk1;
#ifdef	WIN32
					IxADDLMTCHK( AxisRsc->IntAdwk.swk0 );
#endif
					AxisRsc->IntAdwk.swk0 = IxLmtCBS16( AxisRsc->IntAdwk.swk0 );	/* 														*/
/* 2012.10.05 Y.Oka �ϊ��O��%shr�Ȃ̂�IlibASR32�ł́H */
//					AxisRsc->IntAdwk.swk0 = IlibASR16(AxisRsc->IntAdwk.swk0 , 1);
					AxisRsc->IntAdwk.swk0 = (SHORT)IlibASR32((LONG)AxisRsc->IntAdwk.swk0 , 1);
/* 2012.10.05 Y.Oka �ϊ��O��%shr�Ȃ̂�IlibASR32�ł́H */
					IxNop( );						/* 															*/
/*--------------------------------------------------------------------------------------------------*/
#ifdef	WIN32
					IxADDSUBLMTCHKRDY( AxisRsc->VcmpV.VuOut, AxisRsc->IntAdwk.swk0 );
#endif
					AxisRsc->VcmpV.VuOut = AxisRsc->VcmpV.VuOut - AxisRsc->IntAdwk.swk0;
#ifdef	WIN32
					IxSUBLMTCHK( AxisRsc->VcmpV.VuOut );
#endif
					AxisRsc->VcmpV.VuOut = IxLmtCBS16( AxisRsc->VcmpV.VuOut );			/* 															*/
#ifdef	WIN32
					IxADDSUBLMTCHKRDY( AxisRsc->VcmpV.VvOut, AxisRsc->IntAdwk.swk0 );
#endif
					AxisRsc->VcmpV.VvOut = AxisRsc->VcmpV.VvOut - AxisRsc->IntAdwk.swk0;
#ifdef	WIN32
					IxSUBLMTCHK( AxisRsc->VcmpV.VvOut );
#endif
					AxisRsc->VcmpV.VvOut = IxLmtCBS16( AxisRsc->VcmpV.VvOut );			/* 															*/
#ifdef	WIN32
					IxADDSUBLMTCHKRDY( AxisRsc->VcmpV.VwOut, AxisRsc->IntAdwk.swk0 );
#endif
					AxisRsc->VcmpV.VwOut = AxisRsc->VcmpV.VwOut - AxisRsc->IntAdwk.swk0;
#ifdef	WIN32
					IxSUBLMTCHK( AxisRsc->VcmpV.VwOut );
#endif
					AxisRsc->VcmpV.VwOut = IxLmtCBS16( AxisRsc->VcmpV.VwOut );			/* 															*/
					AxisRsc->IntAdV.Vcent = AxisRsc->IntAdwk.swk0;
/*--------------------------------------------------------------------------------------------------*/
					AxisRsc->IntAdwk.swk0 = 1;
/*--------------------------------------------------------------------------------------------------*/
					AxisRsc->IntAdwk.swk0 = IxLIMIT( AxisRsc->VcmpV.VuOut, AxisRsc->IntAdwk.swk0 );	/* TMP1= -1/0/+1									*/
					AxisRsc->IntAdwk.swk1 = AxisRsc->IntAdwk.swk1 | 1;		/* TMP1 = -1/+1	-----sign(VcmpV.VuOut)								*/
					AxisRsc->VcmpV.VuOut = (SHORT)( (LONG)AxisRsc->IntAdwk.swk1 * (LONG)AxisRsc->IntAdP.Kmod ) + AxisRsc->VcmpV.VuOut;
					AxisRsc->VcmpV.VuOut = IxLmtCBS16( AxisRsc->VcmpV.VuOut );			/* 															*/
/*--------------------------------------------------------------------------------------------------*/
					AxisRsc->IntAdwk.swk1 = IxLIMIT( AxisRsc->VcmpV.VvOut, AxisRsc->IntAdwk.swk0 );
					AxisRsc->IntAdwk.swk1 = AxisRsc->IntAdwk.swk1 | 1;		/* sign(VcmpV.VvOut)													*/
					AxisRsc->VcmpV.VvOut = (SHORT)( (LONG)AxisRsc->IntAdwk.swk1 * (LONG)AxisRsc->IntAdP.Kmod ) + AxisRsc->VcmpV.VvOut;
					AxisRsc->VcmpV.VvOut = IxLmtCBS16( AxisRsc->VcmpV.VvOut );			/* 															*/
/*--------------------------------------------------------------------------------------------------*/
					AxisRsc->IntAdwk.swk1 = IxLIMIT( AxisRsc->VcmpV.VwOut, AxisRsc->IntAdwk.swk0 );
					AxisRsc->IntAdwk.swk1 = AxisRsc->IntAdwk.swk1 | 1;		/* sign(VcmpV.VwOut)													*/
					AxisRsc->VcmpV.VwOut = (SHORT)( (LONG)AxisRsc->IntAdwk.swk1 * (LONG)AxisRsc->IntAdP.Kmod ) + AxisRsc->VcmpV.VwOut;
					AxisRsc->VcmpV.VwOut = IxLmtCBS16( AxisRsc->VcmpV.VwOut );			/* 															*/
				}
			}


#endif /* for ROMSIM debug */


/****************************************************************************************************/
/*		On-Delay																					*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*	IU, IV reference calc																			*/
/*--------------------------------------------------------------------------------------------------*/
			AxisRsc->IntAdwk.swk1 = (SHORT)IlibASR32( ( (LONG)AxisRsc->WeakFV.IdOut * (LONG)AxisRsc->SinTbl.CosT ) , 14 );	/* TMP1 <--	ACC >> 14										*/
			AxisRsc->IntAdwk.swk2 = (SHORT)IlibASR32( ( (LONG)AxisRsc->IntAdV.IqRef * (LONG)AxisRsc->SinTbl.SinT ) , 14 );		/* TMP2 <--	ACC >> 14										*/
			IxNop( );						/* 															*/
			AxisRsc->IntAdV.IuOut = AxisRsc->IntAdwk.swk1 - AxisRsc->IntAdwk.swk2;	/* IntAdV.IuOut  <--	TMP1 - TMP2								*/

			AxisRsc->IntAdwk.swk3 = (SHORT)IlibASR32( ( (LONG)AxisRsc->WeakFV.IdOut * (LONG)AxisRsc->SinTbl.CosT3 ) , 14 );	/* TMP3 <--	ACC >> 14										*/
			AxisRsc->IntAdwk.swk4 = (SHORT)IlibASR32( ( (LONG)AxisRsc->IntAdV.IqRef * (LONG)AxisRsc->SinTbl.SinT3 ) , 14 );		/* TMP4 <--	ACC >> 14										*/
			IxNop( );						/* 															*/
/* C����ւ̕ϊ��~�X 2013.01.23 Y.Oka */
//			AxisRsc->IntAdV.IvOut = AxisRsc->IntAdwk.swk3 ^ AxisRsc->IntAdwk.swk4;	/* IntAdV.IvOut  <--	TMP3 - TMP4								*/
			AxisRsc->IntAdV.IvOut = AxisRsc->IntAdwk.swk3 - AxisRsc->IntAdwk.swk4;	/* IntAdV.IvOut  <--	TMP3 - TMP4								*/
/* C����ւ̕ϊ��~�X 2013.01.23 Y.Oka */
/****************************************************************************************************/
//     if ( |IntAdV.IuInData| < IntAdP.OnDelayLvl ) TMP1 = IntAdV.IuOut	/* Reference */
//     else                 TMP1 = IntAdV.IuInData
//     if ( |IntAdV.IvInData| < IntAdP.OnDelayLvl ) TMP2 = IntAdV.IvOut	/* Reference */
//     else                 TMP2 = IntAdV.IvInData
//     if ( |IWD| < IntAdP.OnDelayLvl ) TMP2 = IWO	/* Reference */
//     else                 TMP2 = IWD
/****************************************************************************************************/
			AxisRsc->IntAdwk.swk5 = AxisRsc->IntAdP.OnDelayLvl;
			if(LPX_ABS(AxisRsc->IntAdV.IuInData) > LPX_ABS(AxisRsc->IntAdwk.swk5))	//110530tanaka21��ƃ��� IntAdwk.swk2���ȍ~�g��Ȃ����ߑ���͍s�Ȃ�Ȃ�
			{
				AxisRsc->IntAdwk.swk1 = AxisRsc->IntAdV.IuInData;	/* TMP1 <-- IntAdV.IuInData												*/
			}
			else
			{
				AxisRsc->IntAdwk.swk1 = AxisRsc->IntAdV.IuOut;	/* TMP1 <-- IntAdV.IuOut												*/
			}
			if( LPX_ABS(AxisRsc->IntAdV.IvInData) > LPX_ABS(AxisRsc->IntAdwk.swk5 ) )	//110530tanaka21��ƃ��� IntAdwk.swk2���ȍ~�g��Ȃ����ߑ���͍s�Ȃ�Ȃ�
			{
				AxisRsc->IntAdwk.swk2 = AxisRsc->IntAdV.IvInData;	/* TMP2 <-- IntAdV.IvInData												*/
			}
			else
			{
				AxisRsc->IntAdwk.swk2 = AxisRsc->IntAdV.IvOut;	/* TMP2 <-- IntAdV.IvOut												*/
			}
			AxisRsc->IntAdwk.swk3 = -AxisRsc->IntAdV.IuInData - AxisRsc->IntAdV.IvInData;	/* TMP3(IWD) <-- - TMP1 - TMP2					*/
			if( LPX_ABS(AxisRsc->IntAdwk.swk3) <= LPX_ABS(AxisRsc->IntAdwk.swk5) )	//110530tanaka21��ƃ��� IntAdwk.swk4���ȍ~�g��Ȃ����ߑ���͍s�Ȃ�Ȃ�
			{
/* C����ւ̕ϊ��~�X 2013.01.23 Y.Oka */
//				AxisRsc->IntAdwk.swk3 = AxisRsc->IntAdV.IuOut - AxisRsc->IntAdV.IvOut;	/* TMP3 										*/
				AxisRsc->IntAdwk.swk3 = -AxisRsc->IntAdV.IuOut - AxisRsc->IntAdV.IvOut;	/* TMP3 										*/
/* C����ւ̕ϊ��~�X 2013.01.23 Y.Oka */
			}
			AxisRsc->IntAdwk.swk7 = 0x2000;	/* TMP7 <-- 2000h										*/
			AxisRsc->IntAdwk.swk5 = 1;	/* TMP5 <-- 1												*/

/*--------------------------------------------------------------------------------------------------*/
/*		if(IntAdP.OnDelaySlope != 0) trapezoid type else rectangle type											*/
/*--------------------------------------------------------------------------------------------------*/
			if( AxisRsc->IntAdP.OnDelaySlope == 0 )
			{
/*--------------------------------------------------------------------------------------------------*/
/*		TMP1(ONDVU) = sign(IU)*IntAdP.OnDelayComp																	*/
/*--------------------------------------------------------------------------------------------------*/
				AxisRsc->IntAdwk.swk6 = IxLIMIT( AxisRsc->IntAdwk.swk1, AxisRsc->IntAdwk.swk5 );	/* TMP6 = -1/0/+1							*/
				AxisRsc->IntAdwk.swk1 = (SHORT)( (LONG)AxisRsc->IntAdP.OnDelayComp * (LONG)AxisRsc->IntAdwk.swk6 );
/*--------------------------------------------------------------------------------------------------*/
/*		TMP2(ONDVU) = sign(IV)*IntAdP.OnDelayComp																	*/
/*--------------------------------------------------------------------------------------------------*/
				AxisRsc->IntAdwk.swk6 = IxLIMIT( AxisRsc->IntAdwk.swk2, AxisRsc->IntAdwk.swk5 );
				AxisRsc->IntAdwk.swk2 = (SHORT)( (LONG)AxisRsc->IntAdP.OnDelayComp * (LONG)AxisRsc->IntAdwk.swk6 );
/*--------------------------------------------------------------------------------------------------*/
/*		TMP3(ONDVU) = sign(IW)*IntAdP.OnDelayComp																	*/
/*--------------------------------------------------------------------------------------------------*/
				AxisRsc->IntAdwk.swk6 = IxLIMIT( AxisRsc->IntAdwk.swk3, AxisRsc->IntAdwk.swk5 );
				AxisRsc->IntAdwk.swk3 = (SHORT)( (LONG)AxisRsc->IntAdP.OnDelayComp * (LONG)AxisRsc->IntAdwk.swk6 );
			}
/*--------------------------------------------------------------------------------------------------*/
/*		trapezoid type																				*/
/*--------------------------------------------------------------------------------------------------*/
			else
			{
				AxisRsc->IntAdwk.swk0 = (SHORT)IlibASR32( ( (LONG)AxisRsc->IntAdP.OnDelaySlope * (LONG)AxisRsc->IntAdwk.swk1 ) , 8 );		/* TMP0 <-- IU*IntAdP.OnDelaySlope>>8										*/
				AxisRsc->IntAdwk.swk0 = IxLmtCBS16( AxisRsc->IntAdwk.swk0 );	/* TMP0 = limit(TMP0,2^15-1)							*/
				AxisRsc->IntAdwk.swk0 = IxLIMIT( AxisRsc->IntAdwk.swk0, 8192 );	/* TMP0 = limit(TMP0,8192)									*/
				AxisRsc->IntAdwk.swk1 = (SHORT)IlibASR32( ( (LONG)AxisRsc->IntAdP.OnDelayComp * (LONG)AxisRsc->IntAdwk.swk0 ) , 13 );	/* TMP1(ONDVU) = (IntAdP.OnDelayComp*TMP0)>>13							*/
/*--------------------------------------------------------------------------------------------------*/
				AxisRsc->IntAdwk.swk0 = (SHORT)IlibASR32( ( (LONG)AxisRsc->IntAdP.OnDelaySlope * (LONG)AxisRsc->IntAdwk.swk2 ) , 8 );		/* TMP0 <-- IV*IntAdP.OnDelaySlope>>8										*/
				AxisRsc->IntAdwk.swk0 = IxLmtCBS16( AxisRsc->IntAdwk.swk0 );	/* TMP0 = limit(TMP0,2^15-1)							*/
				AxisRsc->IntAdwk.swk0 = IxLIMIT( AxisRsc->IntAdwk.swk0, 8192 );	/* TMP0 = limit(TMP0,8192)									*/
				AxisRsc->IntAdwk.swk2 = (SHORT)IlibASR32( ( (LONG)AxisRsc->IntAdP.OnDelayComp * (LONG)AxisRsc->IntAdwk.swk0 ) , 13 );	/* TMP1(ONDVU) = (IntAdP.OnDelayComp*TMP0)>>13							*/
/*--------------------------------------------------------------------------------------------------*/
				AxisRsc->IntAdwk.swk0 = (SHORT)IlibASR32( ( (LONG)AxisRsc->IntAdP.OnDelaySlope * (LONG)AxisRsc->IntAdwk.swk3 ) , 8 );		/* TMP0 <-- IV*IntAdP.OnDelaySlope>>8										*/
				AxisRsc->IntAdwk.swk0 = IxLmtCBS16( AxisRsc->IntAdwk.swk0 );	/* TMP0 = limit(TMP0,2^15-1)							*/
				AxisRsc->IntAdwk.swk0 = IxLIMIT( AxisRsc->IntAdwk.swk0, 8192 );	/* TMP0 = limit(TMP0,8192)									*/
				AxisRsc->IntAdwk.swk3 = (SHORT)IlibASR32( ( (LONG)AxisRsc->IntAdP.OnDelayComp * (LONG)AxisRsc->IntAdwk.swk0 ) , 13 );	/* TMP1(ONDVU) = (IntAdP.OnDelayComp*TMP0)>>13							*/
			}
/*--------------------------------------------------------------------------------------------------*/


/****************************************************************************************************/
/*		Voltage conversion to Carrier count range													*/
/****************************************************************************************************/
/*		-2000h..2000h  ---> 0h..4000h  ---> 0h..CRFRQ												*/
/****************************************************************************************************/
			AxisRsc->VcmpV.VuOut = IxLIMIT( AxisRsc->VcmpV.VuOut, AxisRsc->IntAdwk.swk7 );	/* limit +-2000h										*/
			AxisRsc->VcmpV.VvOut = IxLIMIT( AxisRsc->VcmpV.VvOut, AxisRsc->IntAdwk.swk7 );
			AxisRsc->VcmpV.VwOut = IxLIMIT( AxisRsc->VcmpV.VwOut, AxisRsc->IntAdwk.swk7 );
			AxisRsc->IntAdwk.swk4 = AxisRsc->IntAdwk.swk7 - AxisRsc->VcmpV.VuOut;
			AxisRsc->IntAdwk.swk4 = (SHORT)IlibASR32( ( (LONG)AxisRsc->IntAdwk.swk4 * (LONG)AxisRsc->IntAdV.CrFreqW ) , 14 );
			AxisRsc->IntAdwk.swk5 = AxisRsc->IntAdwk.swk7 - AxisRsc->VcmpV.VvOut;
			AxisRsc->IntAdwk.swk5 = (SHORT)IlibASR32(( (LONG)AxisRsc->IntAdwk.swk5 * (LONG)AxisRsc->IntAdV.CrFreqW ) , 14 );
			AxisRsc->IntAdwk.swk6 = AxisRsc->IntAdwk.swk7 - AxisRsc->VcmpV.VwOut;
			AxisRsc->IntAdwk.swk6 = (SHORT)IlibASR32( ( (LONG)AxisRsc->IntAdwk.swk6 * (LONG)AxisRsc->IntAdV.CrFreqW ) , 14 );

/*--------------------------------------------------------------------------------------------------*/
/*		Deat-time compensation (timer) : if(Vx == 0 || Vx == IntAdV.CrFreqW) No compensation					*/
/*--------------------------------------------------------------------------------------------------*/
/* C����ւ̕ϊ��~�X 2013.01.23 Y.Oka */
			if( ( AxisRsc->IntAdwk.swk4 != ZEROR ) || (AxisRsc->IntAdwk.swk4 != AxisRsc->IntAdV.CrFreqW ) )
//			if( ( AxisRsc->IntAdwk.swk4 != ZEROR ) && (AxisRsc->IntAdwk.swk4 != AxisRsc->IntAdV.CrFreqW ) )
/* C����ւ̕ϊ��~�X 2013.01.23 Y.Oka */
			{
#ifdef	WIN32
				IxADDSUBLMTCHKRDY( AxisRsc->IntAdwk.swk4, AxisRsc->IntAdwk.swk1 );
#endif
				AxisRsc->IntAdwk.swk4 = AxisRsc->IntAdwk.swk4 - AxisRsc->IntAdwk.swk1;	/* VcmpV.VuOut <-- VcmpV.VuOut+ONDVU								*/
#ifdef	WIN32
				IxSUBLMTCHK( AxisRsc->IntAdwk.swk4 );
#endif
				IxLmtzReg16( AxisRsc->IntAdwk.swk4, AxisRsc->IntAdwk.swk4, AxisRsc->IntAdV.CrFreqW );	/* VcmpV.VuOut <-- limitz( VcmpV.VuOut , IntAdV.CrFreqW )					*/
			}
/* C����ւ̕ϊ��~�X 2013.01.23 Y.Oka */
			if( ( AxisRsc->IntAdwk.swk5 != ZEROR ) || (AxisRsc->IntAdwk.swk5 != AxisRsc->IntAdV.CrFreqW ) )
//			if( ( AxisRsc->IntAdwk.swk5 != ZEROR ) && (AxisRsc->IntAdwk.swk5 != AxisRsc->IntAdV.CrFreqW ) )
/* C����ւ̕ϊ��~�X 2013.01.23 Y.Oka */
			{
#ifdef	WIN32
				IxADDSUBLMTCHKRDY( AxisRsc->IntAdwk.swk5, AxisRsc->IntAdwk.swk2 );
#endif
				AxisRsc->IntAdwk.swk5 = AxisRsc->IntAdwk.swk5 - AxisRsc->IntAdwk.swk2;	/* VcmpV.VvOut <-- VcmpV.VvOut+ONDVV							*/
#ifdef	WIN32
				IxSUBLMTCHK( AxisRsc->IntAdwk.swk5 );
#endif
				IxLmtzReg16( AxisRsc->IntAdwk.swk5, AxisRsc->IntAdwk.swk5, AxisRsc->IntAdV.CrFreqW );	/* VcmpV.VvOut <-- limitz( VcmpV.VvOut , IntAdV.CrFreqW )				*/
			}
/* C����ւ̕ϊ��~�X 2013.01.23 Y.Oka */
			if( ( AxisRsc->IntAdwk.swk6 != ZEROR ) || (AxisRsc->IntAdwk.swk6 != AxisRsc->IntAdV.CrFreqW ) )
//			if( ( AxisRsc->IntAdwk.swk6 != ZEROR ) && (AxisRsc->IntAdwk.swk6 != AxisRsc->IntAdV.CrFreqW ) )
/* C����ւ̕ϊ��~�X 2013.01.23 Y.Oka */
			{
#ifdef	WIN32
				IxADDSUBLMTCHKRDY( AxisRsc->IntAdwk.swk6, AxisRsc->IntAdwk.swk3 );
#endif
				AxisRsc->IntAdwk.swk6 = AxisRsc->IntAdwk.swk6 - AxisRsc->IntAdwk.swk3;	/* VcmpV.VwOut <-- VcmpV.VwOut+ONDVW								*/
#ifdef	WIN32
				IxSUBLMTCHK( AxisRsc->IntAdwk.swk6 );
#endif
				IxLmtzReg16( AxisRsc->IntAdwk.swk6, AxisRsc->IntAdwk.swk6, AxisRsc->IntAdV.CrFreqW );	/* VcmpV.VwOut <-- limitz( VcmpV.VwOut , IntAdV.CrFreqW )					*/
			}

/*--------------------------------------------------------------------------------------------------*/
/*		Output Voltage & status																		*/
/*--------------------------------------------------------------------------------------------------*/
		}
		AxisRsc->SvIpRegW->CTSTW = AxisRsc->StsFlg.CtrlStsRW;	/* Status Set												*/
	}

	/* Output PWM Data */
	for( ax_no = 0; ax_no < AxisInfo.AxisNum; ax_no++ )
	{
		AxisRsc = &AxisHdl[ax_no];
/****************************************************************************************************/
/*		PWM data set(for test)																		*/
/****************************************************************************************************/
		AxisRsc->SvIpRegW->PwmT2 = AxisRsc->IntAdwk.swk6;
		AxisRsc->SvIpRegW->PwmT1 = AxisRsc->IntAdwk.swk5;
		AxisRsc->SvIpRegW->PwmT0 = AxisRsc->IntAdwk.swk4;
	}

/*--------------------------------------------------------------------------------------------------*/
	/* ��H/W�A�N�Z�X�����ʂ̂��̂��܂Ƃ߂����I�I0���ڂ��ď����̂��i�D������ */
	/* level(AD=3, INT1=0/4 HOST=0) */
	AxisHdl[0].SvIpRegW->INTLVWR |= 0x0003;
	AxisHdl[0].SvIpRegW->OUTPT = 0x0;

	return;
}


#if 0
/****************************************************************************************************/
/*																									*/
/*		Encoder(SPG0) Interrupt Procedure		; �ʏ�(�����C���N���p���X�o�͊�����):11clk	<V720>	*/
/*																									*/
/*		[����]�D�揇�ʂ��ō��ʂ̊��������Ȃ̂ŁA�ł��邾���Z�������ɂ��邱�ƁB						*/
/****************************************************************************************************/
void	MpIntEnc( void )
{
/*--------------------------------------------------------------------------------------------------*/
		if( EncIfV.IncPlsReq == 1 )
		{
			PCVS0 = EncIfV.DivPls.s[0];		/* �p���X�ϊ��ʒu�Z�b�g								*/
		}
		else if( EncIfV.PAOSeqCmd != PAOPLSOUT )
		{
			PCVS0 = (SHORT)IHostWk.IncInitPls;		/* �p���X�ϊ��ʒu�Z�b�g								*/
		}
/*--------------------------------------------------------------------------------------------------*/
		IEncWk.RxFlg0 = FCCST;			/* SDM status bit8 : IEncWk.RxFlg0(Serial-Enc0 receive flag)	*/
/*--------------------------------------------------------------------------------------------------*/
/*		�������ԒZ�k�̂��߁A�g�p���Ȃ��f�[�^�̓Ǎ��݂͂��Ȃ��B										*/
/*--------------------------------------------------------------------------------------------------*/
		IEncWk.RxPos.s[0] = SRPG0RD5;		/* ����l�Ǎ��݁FPosition Low						*/
		IEncWk.RxPos.s[1] = SRPG0RD6;	/* ����l�Ǎ��݁FPosition High						*/
/*--------------------------------------------------------------------------------------------------*/
		IEncWk.EncWk0 = INT1SET;			/* INT1 Acknowledge									*/
/*--------------------------------------------------------------------------------------------------*/
		return;						/* return											*/
}



/****************************************************************************************************/
/*																									*/
/*		�����p���X�X�V����						;	�ő�:???clk, �ʏ�:???clk				<V720>	*/
/*																									*/
/****************************************************************************************************/
void	MpUPDATE_DIVPOS( void )
{
/*--------------------------------------------------------------------------------------------------*/
		IHostWk.Divuswk = INT1SET;		/* INT1 Acknowledge							<V741>	*/
/*--------------------------------------------------------------------------------------------------*/
		IHostWk.LastRcvPosX = EncIfV.RcvPosX0.l;	/* �O��ʒu�f�[�^�X�V								*	*/
/*--------------------------------------------------------------------------------------------------*/
/*		�V���A���G���R�[�_��M�`�F�b�N			; IEncWk.RxFlg0�̒l��@INT_ENC�����ɂčX�V					*/
/*--------------------------------------------------------------------------------------------------*/
//		Divuswk = IEncWk.RxFlg0;		/* SDMSTS bit8 : SPG0 Recieve Completed Check		*/
		if( (IEncWk.RxFlg0 & 0x100 ) == 0 )
		{
			if( EncIfV.SPGFail >= IHostWk.EncMstErrCnt )
			{
				EncIfV.RcvPosX2.l = EncIfV.RcvPosX1.l;	/* �O�X��ʒu�f�[�^								*/
				EncIfV.RcvPosX1.l = EncIfV.RcvPosX0.l;	/* �O��ʒu�f�[�^								*/
				EncIfV.RcvPosX0.l = EncIfV.RcvPosX0.l + EncIfV.RcvPosX1.l;	/* ��ԉ��Z							*/
				EncIfV.RcvPosX0.l = EncIfV.RcvPosX0.l - EncIfV.RcvPosX2.l;	/* EncIfV.RcvPosX0 += (EncIfV.RcvPosX1 - EncIfV.RcvPosX2)	*/
				IHostWk.EncMstErrCnt++;			/* IHostWk.EncMstErrCnt++										*/
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		else
		{
			IHostWk.RxPos0 = IEncWk.RxPos.l;	/* ����l�X�V : IEncWk.RxPos�̒l��@INT_ENC�����ɂčX�V	*/
/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu���Z																					*/
/*		IHostWk.RcvPosX = MencP.MposSign * ((MencV.RxPosL[0].sl>>MencP.MposSftX)<<MencP.MposSftR);			*/
/*																									*/
/*		32bit��ʋl�߃f�[�^�̂��߁A�_���V�t�g�ɂČv�Z(�����r�b�g�̉e���Ȃ�)							*/
/*--------------------------------------------------------------------------------------------------*/
			IHostWk.RcvPosX = ( IHostWk.RxPos0 >> EncIfV.MotPosSftX ) << EncIfV.MotPosSftR;	/* IHostWk.RcvPosX = (ULONG)DivWk0  << EncIfV.MotPosSftR		*/
/*--------------------------------------------------------------------------------------------------*/
/*		IHostWk.RcvPosX = IHostWk.RcvPosX * EncIfV.MotPosSign																*/
/*--------------------------------------------------------------------------------------------------*/
			if( EncIfV.MotPosSign != 1 )
			{
				IHostWk.RcvPosX = ~IHostWk.RcvPosX;
				IHostWk.RcvPosX = IHostWk.RcvPosX + ONER;	/* IHostWk.RcvPosX = -IHostWk.RcvPosX						*/
			}
/*--------------------------------------------------------------------------------------------------*/
/*		�����x���Z�`�F�b�N																			*/
/*--------------------------------------------------------------------------------------------------*/
			if( DivPlsV.AccCntClrReq != 0 )
			{
				IHostWk.Divuswk = ~EncIfV.BitData;		/* DivWk0=~EncIfV.BitData									*/
				IHostWk.Divuswk = IHostWk.Divuswk | ACCCHKENA;		/* DivWk0.ACCCHKENA = TRUE							*/
				EncIfV.BitData = ~IHostWk.Divuswk;		/* EncIfV.BitData=~DivWk0									*/
				IHostWk.AccChkCnt = 0;			/* IHostWk.AccChkCnt = 0									*/
				DivPlsV.AccCntClrReq = 0;			/* �����x�`�F�b�N�J�n�J�E���g�N���A�v�����Z�b�g		*/
			}
//			Divuswk = EncIfV.BitData;
			if( ( EncIfV.BitData & ACCCHKENA ) == 0 )
			{
				IHostWk.MotAcc = ZEROR;		/* IHostWk.MotAcc = 0										*/
				IHostWk.AccChkCnt++;			/* IHostWk.AccChkCnt++										*/
				if( IHostWk.AccChkCnt >= 4 )
				{
					EncIfV.BitData = EncIfV.BitData | ACCCHKENA;		/* EncIfV.BitData.ACCCHKENA = TRUE							*/
				}
				EncIfV.RcvPosX0.l = IHostWk.RcvPosX;	/* EncIfV.RcvPosX0 = IHostWk.RcvPosX								*/
				EncIfV.RcvPosX1.l = IHostWk.RcvPosX;	/* EncIfV.RcvPosX1 = IHostWk.RcvPosX								*/
				EncIfV.RcvPosX2.l = IHostWk.RcvPosX;	/* EncIfV.RcvPosX2 = IHostWk.RcvPosX								*/
			}
			else
			{
				IHostWk.DivWk0 = IHostWk.RcvPosX - EncIfV.RcvPosX0.l;	/* DivWk0   = IHostWk.RcvPosX  - EncIfV.RcvPosX0		*/
				IHostWk.DivWk1 = EncIfV.RcvPosX0.l - EncIfV.RcvPosX1.l;	/* DivWk1   = EncIfV.RcvPosX0 - EncIfV.RcvPosX1		*/
				IHostWk.MotAcc = IHostWk.DivWk0 - IHostWk.DivWk1;	/* IHostWk.MotAcc   = DivWk0 - DivWk1				*/
				if( EncIfV.AccErrLv.l >= IHostWk.MotAcc )
				{
					if( ( EncIfV.AccErrLv.l + IHostWk.MotAcc ) < 0 )
					{
/*--------------------------------------------------------------------------------------------------*/
/*		DivWk0 = (IHostWk.RcvPosX - EncIfV.RcvPosX1) >> 1															*/
/*--------------------------------------------------------------------------------------------------*/
						IHostWk.DivWk0 = IHostWk.RcvPosX - EncIfV.RcvPosX1.l;	/* DivWk0 =  IHostWk.RcvPosX  - EncIfV.RcvPosX1		*/
						IHostWk.DivWk0 = IHostWk.DivWk0 & 0xfffffffe;	/* �Z�p�E�V�t�g�̎l�̌ܓ��������̑΍�				*/
						IHostWk.DivWk0 = IlibASR32(IHostWk.DivWk0 , 1);			/* DivWk0  = (IHostWk.RcvPosX  - EncIfV.RcvPosX1) >> 1				*/
						IHostWk.DivWk1 = EncIfV.RcvPosX1.l - EncIfV.RcvPosX2.l;	/* DivWk1  =  EncIfV.RcvPosX1 - EncIfV.RcvPosX2		*/
						IHostWk.MotAcc = IHostWk.DivWk0 - IHostWk.DivWk1;	/* IHostWk.MotAcc  =  DivWk0 - DivWk1				*/
					}
				}
				else
				{
/*--------------------------------------------------------------------------------------------------*/
/*		DivWk0 = (IHostWk.RcvPosX - EncIfV.RcvPosX1) >> 1															*/
/*--------------------------------------------------------------------------------------------------*/
					IHostWk.DivWk0 = IHostWk.RcvPosX - EncIfV.RcvPosX1.l;	/* DivWk0 =  IHostWk.RcvPosX  - EncIfV.RcvPosX1		*/
					IHostWk.DivWk0 = IHostWk.DivWk0 & 0xfffffffe;	/* �Z�p�E�V�t�g�̎l�̌ܓ��������̑΍�				*/
					IHostWk.DivWk0 = IlibASR32(IHostWk.DivWk0 , 1);			/* DivWk0  = (IHostWk.RcvPosX  - EncIfV.RcvPosX1) >> 1				*/
					IHostWk.DivWk1 = EncIfV.RcvPosX1.l - EncIfV.RcvPosX2.l;	/* DivWk1  =  EncIfV.RcvPosX1 - EncIfV.RcvPosX2		*/
					IHostWk.MotAcc = IHostWk.DivWk0 - IHostWk.DivWk1;	/* IHostWk.MotAcc  =  DivWk0 - DivWk1				*/
				}
			}
			if( EncIfV.AccErrLv.l >= IHostWk.MotAcc )
			{
/*--------------------------------------------------------------------------------------------------*/
/*		�����x�ُ펞																				*/
/*--------------------------------------------------------------------------------------------------*/
				if( EncIfV.SPGFail < IHostWk.EncMstErrCnt )
				{
					EncIfV.RcvPosX2.l = EncIfV.RcvPosX1.l;	/* �O�X��ʒu�f�[�^								*/
					EncIfV.RcvPosX1.l = EncIfV.RcvPosX0.l;	/* �O��ʒu�f�[�^								*/
					EncIfV.RcvPosX0.l = IHostWk.RcvPosX;	/* �����x�ُ펞�͕�Ԃ��Ȃ�							*/
					IHostWk.EncMstErrCnt++;			/* IHostWk.EncMstErrCnt++										*/
				}
			}
			else if( ( EncIfV.AccErrLv.l + IHostWk.MotAcc ) < 0 )
			{
/*--------------------------------------------------------------------------------------------------*/
/*		�����x���펞																				*/
/*--------------------------------------------------------------------------------------------------*/
				IHostWk.EncMstErrCnt = 0;			/* IHostWk.EncMstErrCnt=0										*/
				EncIfV.RcvPosX2.l = EncIfV.RcvPosX1.l;	/* �O�X��ʒu�f�[�^								*/
				EncIfV.RcvPosX1.l = EncIfV.RcvPosX0.l;	/* �O��ʒu�f�[�^								*/
				EncIfV.RcvPosX0.l = IHostWk.RcvPosX;	/* ����ʒu�f�[�^									*/
			}
/*--------------------------------------------------------------------------------------------------*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		dMotPos = RMX_dPosOfXpos( MencV.MotPosX[0], LastMotPosX );									*/
/*--------------------------------------------------------------------------------------------------*/
/*		�Z�p�E�V�t�g�ɂĐ؂�̂Ă��鉺�ʃr�b�g��0�̂��߁A�l�̌ܓ��̉e���Ȃ��B						*/
/*--------------------------------------------------------------------------------------------------*/
		IHostWk.DMotPos = EncIfV.RcvPosX0.l - IHostWk.LastRcvPosX;	/* IHostWk.DMotPos = EncIfV.RcvPosX0 - IHostWk.LastRcvPosX	*/
		IHostWk.DMotPos = IlibASR32(IHostWk.DMotPos , EncIfV.MotPosSftR);
/*--------------------------------------------------------------------------------------------------*/
		if( EncIfV.IncPlsReq == 1 )
		{
			EncIfV.PlsOSetCmd = DivPlsV.PlsOSetCmdIn;	/* �p���X�o�͉�H�������v���X�V from HostCPU		*/
			if( EncIfV.PlsOSetCmd == POSETCMD00 )
			{
				PCVS0 = 0x0000;			/* 													*/
				DivPlsV.PlsOSetCmdIn = POSETNOCMD;	/* �������v���N���A									*/
			}
			else if( EncIfV.PlsOSetCmd == POSETCMDFF )
			{
				PCVS0 = 0xFFFF;			/* 													*/
				DivPlsV.PlsOSetCmdIn = POSETNOCMD;	/* �������v���N���A									*/
			}
			else
			{
				IHostWk.IncInitPls = DivPlsV.IncInitPlsIn.l;	/* 													*/
				EncIfV.DivPls.l = DivPlsV.IncInitPlsIn.l;	/* 													*/
				EncIfV.DivPos.l = DivPlsV.IncInitPlsIn.l;	/* for Linear										*/
				EncIfV.DivPlsRem.l = DivPlsV.IncInitRemIn.l;	/* for Linear										*/
			}
		}
		else
		{
			if( IHostWk.PoSet1W != DivPlsV.PoSet1In )
			{
				IHostWk.PoSet1W = DivPlsV.PoSet1In;		/* 													*/
				IHostWk.PoSet2W = DivPlsV.PoSet2In;		/* 													*/
				PCVS1 = IHostWk.PoSet1W;			/* �p���X�ϊ����_�␳1�Z�b�g (HostCPU�Ɠ�����Ԃɐݒ�)	*/
				PCVS2 = IHostWk.PoSet2W;			/* �p���X�ϊ����_�␳2�Z�b�g						*/
			}
		}
		if( IHostWk.DivSetW != DivPlsV.DivSetIn )
		{
			IHostWk.DivSetW = DivPlsV.DivSetIn;		/* 													*/
			DivSet = IHostWk.DivSetW;			/* �����@�\�Z�b�g (HostCPU�Ɠ�����Ԃɐݒ�)			*/
		}
		if( EncIfV.IncPlsReq != 1 )
		{
			if( EncIfV.AmpType != LINEAR )
			{
/*--------------------------------------------------------------------------------------------------*/
//		�����p���X = (MencV.MotPosX[0] >> MencP.EncIfV.DivOutSft);											*
/*--------------------------------------------------------------------------------------------------*/
//		�Z�p�E�V�t�g�ɂĐ؂�̂Ă��鉺�ʃr�b�g��0�ɂ���(�l�̌ܓ��������΍�)						*
/*--------------------------------------------------------------------------------------------------*/
				IHostWk.DivWk1 = NONER << EncIfV.DivOutSft;	/* DivWk1=(FFFFFFFFh<<EncIfV.DivOutSft)			*/
				IHostWk.DivWk0 = EncIfV.RcvPosX0.l & IHostWk.DivWk1;	/* DivWk0=((EncIfV.RcvPosX0&(FFFFFFFFh<<EncIfV.DivOutSft))	*/
				EncIfV.DivPls.l = IlibASR32(IHostWk.DivWk0 , EncIfV.DivOutSft);	/* EncIfV.DivPls=((EncIfV.RcvPosX0&(FFFFFFFFh<<EncIfV.DivOutSft))>>EncIfV.DivOutSft	*/
			}
			else
			{
				DivPlsV.Argu0.l = IHostWk.DMotPos;		/* DivPlsV.Argu0 <-- IHostWk.DMotPos									*/
				DivPlsV.Argu1.l = EncIfV.DivOutGain.l;	/* DivPlsV.Argu1 <-- EncIfV.DivOutGain								*/
				DivPlsV.Iu0.l = EncIfV.DivPlsRem.l;		/* DivPlsV.Iu0 <-- EncIfV.DivPlsRem									*/
				MpMlibPfbkxremNolim( );				/* DivPlsV.Ret0 = MLIBPFBKXREMNOLIM()							*/
				EncIfV.DivPos.l = EncIfV.DivPos.l + DivPlsV.Ret0.l;	/* EncIfV.DivPos = EncIfV.DivPos + DivPlsV.Ret0							*/
				EncIfV.DivPlsRem.l = DivPlsV.Iu0.l;		/* EncIfV.DivPlsRem <-- DivPlsV.Iu0									*/
				EncIfV.DivPls.l = EncIfV.DivPos.l;	/* EncIfV.DivPls = EncIfV.DivPos									*/
			}
		}
		EncIfV.IncPlsReq = DivPlsV.IncPlsReqIn;	/* �����C���N���p���X�o�͗v���X�V from HostCPU		*/
		EncIfV.PAOSeqCmd = DivPlsV.PAOSeqCmdIn;	/* 													*/

		return;						/* return											*/
}
#endif


/****************************************************************************************************/
/*																									*/
/*		DATA clear subroutin																		*/
/*																									*/
/****************************************************************************************************/
void	MpDataClear( MICRO_AXIS_HANDLE *AxisRsc )
{
/*--------------------------------------------------------------------------------------------------*/
/*		HOST int clear<1.02>																		*/
/*--------------------------------------------------------------------------------------------------*/
	AxisRsc->IntAdV.IqOut1L.l = ZEROR;	/*									; <V388> �ǉ�		*/
	AxisRsc->IntAdV.IqOut1PL.l = ZEROR;	/*									; <V388> �ǉ�		*/
	AxisRsc->IntAdV.IqOut1PPL.l = ZEROR;	/*									; <V388> �ǉ�	*/
	AxisRsc->IntAdV.IqIn1PL.l = ZEROR;	/*									; <V388> �ǉ�		*/
	AxisRsc->IntAdV.IqIn1PPL.l = ZEROR;	/*									; <V388> �ǉ�		*/
	AxisRsc->IntAdV.IqOut2L.l = ZEROR;	/*									; <V388> �ǉ�		*/
	AxisRsc->IntAdV.IqOut2PL.l = ZEROR;	/*									; <V388> �ǉ�		*/
	AxisRsc->IntAdV.IqOut2PPL.l = ZEROR;	/*									; <V388> �ǉ�	*/
	AxisRsc->IntAdV.IqIn2PL.l = ZEROR;	/*									; <V388> �ǉ�		*/
	AxisRsc->IntAdV.IqIn2PPL.l = ZEROR;	/*									; <V388> �ǉ�	*/
	AxisRsc->IntAdV.IqOut3L.l = ZEROR;	/*									; <V388> �ǉ�		*/
	AxisRsc->IntAdV.IqOut3PL.l = ZEROR;	/*									; <V388> �ǉ�		*/
	AxisRsc->IntAdV.IqOut3PPL.l = ZEROR;	/*									; <V388> �ǉ�	*/
	AxisRsc->IntAdV.IqIn3PL.l = ZEROR;	/*									; <V388> �ǉ�		*/
	AxisRsc->IntAdV.IqIn3PPL.l = ZEROR;	/*									; <V388> �ǉ�	*/
/*--------------------------------------------------------------------------------------------------*/
	AxisRsc->AcrV.IdIntgl.l = ZEROR;	/* integral(32bit) <-- 0								*/
	AxisRsc->AcrV.IqIntgl.l = ZEROR;	/* integral(32bit) <-- 0								*/
	AxisRsc->AcrV.VdFil.l = ZEROR;	/* vd filter out(32bit) <-- 0							*/
	AxisRsc->AcrV.VqFil.l = ZEROR;	/* vq filter out(32bit) <-- 0							*/
	AxisRsc->IntAdV.IqOut2Lpf.l = ZEROR;	/* iq filter out(32bit) <-- 0							*/
	AxisRsc->IntAdV.IqRef = 0x0;		/* iq(after limit) <-- 0									*/
	AxisRsc->VcmpV.VdOut = 0x0;			/* vd <-- 0													*/
	AxisRsc->VcmpV.VqOut = 0x0;			/* vq <-- 0													*/
	AxisRsc->VcmpV.VuOut = 0x0;			/* vu <-- 0													*/
	AxisRsc->VcmpV.VvOut = 0x0;			/* vv <-- 0													*/
	AxisRsc->VcmpV.VwOut = 0x0;			/* vw <-- 0													*/
	AxisRsc->VcmpV.LdC = 0x0;
	AxisRsc->VcmpV.LqC = 0x0;
	AxisRsc->VcmpV.MagC = 0x0;
	AxisRsc->IntAdV.IuOut = 0x0;
	AxisRsc->IntAdV.IvOut = 0x0;
	AxisRsc->IntAdV.IdDataP = AxisRsc->IntAdV.IdInData;		/* 															*/
	AxisRsc->IntAdV.IqDataP = AxisRsc->IntAdV.IqRef;		/* 															*/
/*--------------------------------------------------------------------------------------------------*/
	AxisRsc->WeakFV.IdOut = 0;			/* 															*/
	AxisRsc->VcmpV.VdOut = 0;			/* 															*/
	AxisRsc->VcmpV.VqOut = 0;			/* 															*/
	AxisRsc->IntAdV.IdLfil.l = ZEROR;	/* 														*/
	AxisRsc->IntAdV.IqLfil.l = ZEROR;	/* 														*/

	AxisRsc->WeakFV.WfIntgl.l = ZEROR;	/* <V214>											*/
	AxisRsc->WeakFV.WfVdRef = 0;		/* <V214>							; �폜<V309>	����<V531>	*/
	AxisRsc->WeakFV.WfVqRef = 0;		/* <V214>							; �폜<V309>	����<V531>	*/

/*--------------------------------------------------------------------------------------------------*/
	return;
}



/****************************************************************************************************/
/*																									*/
/*		 SQRT(TMP2(32)) Sub-routin (MAX 1.21us)														*/
/*																									*/
/****************************************************************************************************/
/*		Input		TMP2 : Low  data																*/
/*					TMP3 : High data																*/
/*		Output		TMP0 : SQRT(dat)																*/
/*		Stack No.	0																				*/
/*		Work		TMP0,TMP1,TMP2,TMP3,TMP4,TMP5,TMP8												*/
/*					MACCL,MACCH,SACCL,SACCH															*/
/****************************************************************************************************/
USHORT	MpSQRT( INTADWK *IntAdwk, ULONG src )
{
		IntAdwk->sqrtwk.Low = (USHORT)src;
		IntAdwk->sqrtwk.High = (USHORT)( src >> 16 );

/*--------------------------------------------------------------------------------------------------*/
/*		TMP0(16) = sqrt(TMP2(32))																	*/
/*--------------------------------------------------------------------------------------------------*/
/*		TMP3(High), TMP2(Low)		---> TMP0(result)												*/
/*		table search from high 8bits																*/
/*		and closely resemble using low 15 bits														*/
/*				|----|----|----|----|----|----|--------|											*/
/*				31   27   23   19   15   11   7        0											*/
/*		TMP8	   0    2    4    6    8    10    12												*/
/*--------------------------------------------------------------------------------------------------*/
		IntAdwk->sqrtwk.uswk6 = 0;
		if( IntAdwk->sqrtwk.High & 0xF000 )
/*--------------------------------------------------------------------------------------------------*/
/*		TMP8 0																						*/
/*		|xxxx|yyyy|aaaa|aaaa|aaaa|aaa-|--------|													*/
/*--------------------------------------------------------------------------------------------------*/
		{
			IntAdwk->sqrtwk.tmp0.ul = ( src >> 9 );	/* TMP4 for approxmate(15bit)							*/
			IntAdwk->sqrtwk.tmp0.us[0] = ( IntAdwk->sqrtwk.tmp0.us[0] & 0x7FFF );	/* mask 15bit												*/
			IntAdwk->sqrtwk.uswk5 = ( IntAdwk->sqrtwk.High >> 8 );	/* TMP5 for table search(8bit)						*/
		}
		else if( IntAdwk->sqrtwk.High & 0x0F00 )
/*--------------------------------------------------------------------------------------------------*/
/*		TMP8 2																						*/
/*		|0000|xxxx|yyyy|aaaa|aaaa|aaaa|aaa-----|													*/
/*--------------------------------------------------------------------------------------------------*/
		{
			IntAdwk->sqrtwk.uswk6 = 2;
			IntAdwk->sqrtwk.tmp0.ul = ( src >> 5 );	/* TMP4 for approximate(15bit)							*/
			IntAdwk->sqrtwk.tmp0.us[0] = ( IntAdwk->sqrtwk.tmp0.us[0] & 0x7FFF );	/* mask 15bit												*/
			IntAdwk->sqrtwk.uswk5 = ( IntAdwk->sqrtwk.High >> 4 );	/* TMP5 for table search(8bit)						*/
		}
		else if( IntAdwk->sqrtwk.High & 0x00F0 )
/*--------------------------------------------------------------------------------------------------*/
/*		TMP8 4																						*/
/*		|0000|0000|xxxx|yyyy|aaaa|aaaa|aaaaaaa-|													*/
/*--------------------------------------------------------------------------------------------------*/
		{
			IntAdwk->sqrtwk.uswk6 = 4;
			IntAdwk->sqrtwk.uswk5 = IntAdwk->sqrtwk.High;	/* TMP5 for table search(8bit)						*/
			IntAdwk->sqrtwk.tmp0.us[0] = ( IntAdwk->sqrtwk.Low >> 1 );	/* TMP4 for approximate(15bit)							*/
		}
		else if( IntAdwk->sqrtwk.High & 0x000F )
/*--------------------------------------------------------------------------------------------------*/
/*		TMP8 6																						*/
/*		|0000|0000|0000|xxxx|yyyy|aaaa|aaaaaaaa|(000)												*/
/*--------------------------------------------------------------------------------------------------*/
		{
			IntAdwk->sqrtwk.uswk6 = 6;
			IntAdwk->sqrtwk.uswk5 = (USHORT)(( src & 0x0FFFF000 ) >> 12);	/* TMP5 for table search(8bit)						*/
			IntAdwk->sqrtwk.tmp0.ul = ( src << 4 );	/* TMP5 for table search(8bit)							*/
			IntAdwk->sqrtwk.tmp0.us[0] = ( IntAdwk->sqrtwk.tmp0.us[0] >> 1 );	/* TMP4 for approximate(15bit)							*/
			IntAdwk->sqrtwk.tmp0.us[0] = ( IntAdwk->sqrtwk.tmp0.us[0] & 0x7FFF );	/* mask 15bit												*/
		}
		else if( IntAdwk->sqrtwk.Low & 0xF000 )
/*--------------------------------------------------------------------------------------------------*/
/*		TMP8 8																						*/
/*		|0000|0000|0000|0000|xxxx|yyyy|aaaaaaaa|(0000000)											*/
/*--------------------------------------------------------------------------------------------------*/
		{
			IntAdwk->sqrtwk.uswk6 = 8;
			IntAdwk->sqrtwk.uswk5 = ( IntAdwk->sqrtwk.Low >> 8 );	/* TMP5 for table search (8bit)						*/
			IntAdwk->sqrtwk.uswk4 = ( IntAdwk->sqrtwk.Low & 0x0FF );
			IntAdwk->sqrtwk.tmp0.us[0] = ( IntAdwk->sqrtwk.uswk4 << 7 );	/* TMP4 for approximate (15bit)							*/
		}
		else if( IntAdwk->sqrtwk.Low & 0x0F00 )
/*--------------------------------------------------------------------------------------------------*/
/*		TMP8 10																						*/
/*		|0000|0000|0000|0000|0000|xxxx|yyyyaaaa|(00000000000)										*/
/*--------------------------------------------------------------------------------------------------*/
		{
			IntAdwk->sqrtwk.uswk6 = 10;
			IntAdwk->sqrtwk.uswk5 = ( IntAdwk->sqrtwk.Low >> 4 );	/* TMP5 table search (8bit)							*/
			IntAdwk->sqrtwk.uswk4 = ( IntAdwk->sqrtwk.Low & 0x00F );
			IntAdwk->sqrtwk.tmp0.us[0] = ( IntAdwk->sqrtwk.uswk4 << 11 );	/* TMP4 approximate (15bit)							*/
		}
// |0000|0000|0000|0000|0000|0000|xxxxyyyy|(000000000000000)
		else
		{
			IntAdwk->sqrtwk.uswk6 = 12;
			IxTblSqrt16( &(IntAdwk->sqrtwk.uswk0), IntAdwk->sqrtwk.Low );	/* TMP0 = table data									*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		table read and approximate																	*/
/*		TMP5(High), TMP4(Low)																		*/
/*--------------------------------------------------------------------------------------------------*/
		if( IntAdwk->sqrtwk.uswk6 < 12 )
		{
			IxTblSqrt16( &(IntAdwk->sqrtwk.uswk3), IntAdwk->sqrtwk.uswk5 );	/* TMP3 <-- tbl[tmp]								*/
			if( IntAdwk->sqrtwk.uswk5 == 0x00FF )
			{
				IntAdwk->sqrtwk.uswk0 = 0xFFFF;	/* TMP0 <-- (tbl[tmp+1])									*/
			}
			else
			{
				IntAdwk->sqrtwk.uswk5 = IntAdwk->sqrtwk.uswk5 + 1;
				IxTblSqrt16( &(IntAdwk->sqrtwk.uswk0), IntAdwk->sqrtwk.uswk5 );	/* TMP0 <-- tbl[tmp+1]									*/
			}
/*--------------------------------------------------------------------------------------------------*/
/*		(tbl[tmp+1] - tbl[tmp])*low/32768 + tbl[tmp]												*/
/*--------------------------------------- -----------------------------------------------------------*/
			IntAdwk->sqrtwk.uswk4 = IntAdwk->sqrtwk.uswk0 - IntAdwk->sqrtwk.uswk3;
			IntAdwk->sqrtwk.uswk1 = (USHORT)IlibASR32(( (LONG)IntAdwk->sqrtwk.uswk4 * (LONG)IntAdwk->sqrtwk.tmp0.us[0] ) , 15);
			IntAdwk->sqrtwk.uswk0 = IntAdwk->sqrtwk.uswk1 + IntAdwk->sqrtwk.uswk3;	/* TMP0 = read data							*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Scaling																						*/
/*--------------------------------------------------------------------------------------------------*/
		IntAdwk->sqrtwk.ulwk2 = (ULONG)(IntAdwk->sqrtwk.uswk0);
		IntAdwk->sqrtwk.ulwk0 = IlibASR32(IntAdwk->sqrtwk.ulwk2 , IntAdwk->sqrtwk.uswk6);
		return( (USHORT)IntAdwk->sqrtwk.ulwk0 );
}


/****************************************************************************************************/
/*																									*/
/*		Over modulation compasation calculation														*/
/*																									*/
/****************************************************************************************************/
/*		INPUT:   TMP4: table address, IntAdV.V1:modulation											*/
/*		OUTPUT:  Kmod:   compensation gain/offset													*/
/*		work:    TMP0,TMP1,TMP2,TMP3																*/
/****************************************************************************************************/
void	MpOVMMODK( INTADP *IntAdP, INTADV *IntAdV, INTADWK *IntAdwk )
{
	if( IntAdV->V1 < 9459 )
	{
		IxNop( );						/* 															*/
		IxLoadMpmem16( IntAdP->Kmod, IntAdwk->pCtbl, 0 );	/* IntAdP->Kmod = G[0];											*/
	}
	else if( IntAdP->CtrlSw & OVMMOD )
	{
		IntAdwk->pCtbl = IntAdwk->pCtbl + 15;
		IxLoadMpmem16( IntAdP->Kmod, IntAdwk->pCtbl, 1 );
	}
	else if( IntAdV->V1 < 10431 )
	{
		IntAdwk->swk0 = IntAdV->V1;
		IntAdwk->swk0 = IntAdwk->swk0 - 9443;	/* -9439-5(margin)											*/
		IntAdwk->swk1 = IntAdwk->swk0;
		IntAdwk->swk0 = IntAdwk->swk0 >> 5;	/* high													*/
		IntAdwk->swk1 = IntAdwk->swk1 & 0x1F;	/* low														*/
		if( IntAdwk->swk0 >= 32 )
		{
			IntAdwk->pCtbl = IntAdwk->pCtbl + 15;
			IxLoadMpmem16( IntAdP->Kmod, IntAdwk->pCtbl, 1 );
		}
		else
		{
			IntAdwk->swk2 = IntAdwk->swk0;
			IntAdwk->swk0 = IntAdwk->swk0 >> 1;
			if( ( IntAdwk->swk2 & 1 ) == 0 )
			{
				IntAdwk->pCtbl = IntAdwk->pCtbl + IntAdwk->swk0;
				IxLoadMpmem16( IntAdwk->swk2, IntAdwk->pCtbl, 0 );
				IxLoadMpmem16( IntAdwk->swk3, IntAdwk->pCtbl, 1 );
			}
			else
			{
				IntAdwk->pCtbl = IntAdwk->pCtbl + IntAdwk->swk0;
				IxLoadMpmem16( IntAdwk->swk2, IntAdwk->pCtbl, 1 );
				IntAdwk->pCtbl = IntAdwk->pCtbl + 1;
				IxLoadMpmem16( IntAdwk->swk3, IntAdwk->pCtbl, 0 );
			}
			IntAdwk->swk0 = IntAdwk->swk3 - IntAdwk->swk2;
/* 2012.10.05 Y.Oka �ϊ��O��%shr�Ȃ̂�IlibASR32�ł́H */
//			IntAdwk->swk0 = IlibASR16( IntAdwk->swk0 * IntAdwk->swk1, 5);
			IntAdwk->swk0 = (SHORT)IlibASR32( (LONG)IntAdwk->swk0 * (LONG)IntAdwk->swk1, 5);
/* 2012.10.05 Y.Oka �ϊ��O��%shr�Ȃ̂�IlibASR32�ł́H */
			IntAdP->Kmod = IntAdwk->swk0 + IntAdwk->swk2;
		}
	}
	return;
}



#if 0
/****************************************************************************************************/
/*																									*/
/*		���䉉�Z���C�u����																			*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		�]��t���ʒu�e�a�v�Z : rv = (kx*u+pfbrem)>>sx 	; ??clk								<V720>	*/
/*																									*/
/****************************************************************************************************/
//LONG	MpMlibPfbkxremNolim(
/*		LONG u,									/* DivPlsV.Argu0    : ����							*/
/*		LONG k,									/* DivPlsV.Argu1    : �Q�C��						*/
/*		LONG *pfbrem )							/* DivPlsV.Iu0    : �]��ւ̃|�C���^				*/
/*--------------------------------------------------------------------------------------------------*/
/*												/* DivPlsV.Ret0     : �߂�l						*/
/*--------------------------------------------------------------------------------------------------*/
/*		LONG	kx								/* DivPlsV.Kx     : kx								*/
/*		LONG	sx								/* DivPlsV.Sx     : sx								*/
/*		LONG	rv								/* lswk10  : ���Z����								*/
/*		LONG	pfbrem							/* lswk11  : �]��									*/
/*		LONG	wk1								/* lswk1	  : ��Ɨp								*/
/*		LONG	wk2								/* lswk2   : ��Ɨp									*/
/*												/* lswk3   : ��Z���ʕێ��p(����32bit)				*/
/*												/* lswk4   : ��Z���ʕێ��p(���32bit)				*/
/*--------------------------------------------------------------------------------------------------*/
void	MpMlibPfbkxremNolim( void )
{
/*--------------------------------------------------------------------------------------------------*/
		DivPlsV.Kx.l = DivPlsV.Argu1.l << 8;		/* DivPlsV.Kx = k<<8										*/
		DivPlsV.Sx.l = DivPlsV.Argu1.l >> 24;		/* DivPlsV.Sx = k>>24										*/
/*--------------------------------------------------------------------------------------------------*/
		IPfbwk.lswk1 = 24;				/* lswk1 = 24										*/
		if( IPfbwk.lswk1 >= DivPlsV.Sx.l )
		{
/*--------------------------------------------------------------------------------------------------*/
//			IPfbwk.dlwk.dl = DivPlsV.Argu0.l * DivPlsV.Kx.l;
			IPfbwk.dlwk.l[0] = DivPlsV.Argu0.l * DivPlsV.Kx.l;  //provision
			IPfbwk.lswk1 = IPfbwk.lswk1 - DivPlsV.Sx.l;	/* lswk1 = 24 - sx									*/
/*--------------------------------------------------------------------------------------------------*/
			IPfbwk.lswk2 = IPfbwk.dlwk.l[0] >> DivPlsV.Sx.s[0];	/* lswk2 = (xl>>sx)									*/
			IPfbwk.lswk2 = IPfbwk.lswk2 >> 8;		/* lswk2 =((xl>>sx)>>8)								*/
			IPfbwk.lswk10 = IPfbwk.dlwk.l[1] << IPfbwk.lswk1;	/* lswk10 = (xh<<(24-sx))						*/
			IPfbwk.lswk10 = IPfbwk.lswk10 + IPfbwk.lswk2;	/* lswk10 =((xh<<(24-sx)) + ((xl>>sx)>>8))		*/
/*--------------------------------------------------------------------------------------------------*/
			IPfbwk.lswk11 = IPfbwk.dlwk.l[0] << IPfbwk.lswk1;	/* lswk11 = (xl<<(24-sx))						*/
			IPfbwk.lswk11 = IPfbwk.lswk11 >> 8;	/* lswk11 =((xl<<(24-sx))>>8)						*/
			IPfbwk.lswk11 = IPfbwk.lswk11 + DivPlsV.Iu0.l;
		}
		else
		{
//			IPfbwk.dlwk.dl = DivPlsV.Argu0.l * DivPlsV.Kx.l;
			IPfbwk.dlwk.l[0] = DivPlsV.Argu0.l * DivPlsV.Kx.l;  //provision
			IPfbwk.lswk3 = IPfbwk.dlwk.l[0];		/* lswk3 = xl										*/
			IPfbwk.lswk4 = IPfbwk.dlwk.l[1];		/* lswk4 = xh										*/
			IPfbwk.lswk1 = DivPlsV.Sx.l - IPfbwk.lswk1;	/* lswk1 = sx - 24									*/
/*--------------------------------------------------------------------------------------------------*/
		// �Z�p�E�V�t�g�ɂĐ؂�̂Ă��鉺�ʃr�b�g��0�ɂ���(�l�̌ܓ��������΍�)			*
/*--------------------------------------------------------------------------------------------------*/
			IPfbwk.lswk2 = NONER << IPfbwk.lswk1;	/* lswk2 =(FFFFFFFFh<<(sx-24))					*/
			IPfbwk.lswk2 = IPfbwk.lswk4 & IPfbwk.lswk2;	/* lswk2 =(xh & (FFFFFFFFh<<(sx-24)))			*/
//#ifdef WIN32
			IPfbwk.lswk10 = (LONG)((INT64)IPfbwk.lswk2 >> IPfbwk.lswk1);	/* lswk10 = (xh>>(sx-24))							*/
//#elif defined(ASIP_CC)
//			IPfbwk.lswk10 = asr( IPfbwk.lswk2, IPfbwk.lswk1);	/* lswk10 = (xh>>(sx-24))							*/
//#endif
/*--------------------------------------------------------------------------------------------------*/
			IPfbwk.lswk11 = IPfbwk.lswk3 >> IPfbwk.lswk1;	/* lswk11 =   (xl>>(sx-24))						*/
			IPfbwk.lswk11 = IPfbwk.lswk11 >> 7;	/* lswk11 =  ((xl>>(sx-24))>>7)						*/
			IPfbwk.lswk11 = IPfbwk.lswk11 + ONER;	/* lswk11 = (((xl>>(sx-24))>>7)+1)				*/
			IPfbwk.lswk11 = IPfbwk.lswk11 >> 1;	/* lswk11 =((((xl>>(sx-24))>>7)+1)>>1)				*/
			IPfbwk.lswk11 = IPfbwk.lswk11 + DivPlsV.Iu0.l;	/* lswk11 = pfbrem + ((((xl>>(sx-24))>>7)+1)>>1)	*/
/*--------------------------------------------------------------------------------------------------*/
			IPfbwk.lswk1 = 56;				/* lswk1 = 56										*/
			IPfbwk.lswk1 = IPfbwk.lswk1 - DivPlsV.Sx.l;	/* lswk1 = 56 - sx									*/
			IPfbwk.lswk2 = IPfbwk.lswk4 << IPfbwk.lswk1;	/* lswk2 = (xh<<(56-sx))							*/
			IPfbwk.lswk2 = IPfbwk.lswk2 >> 8;		/* lswk2 =((xh<<(56-sx))>>8)							*/
			IPfbwk.lswk11 = IPfbwk.lswk11 + IPfbwk.lswk2;	/* lswk11= lswk11 + ((xh<<(56-sx))>>8)			*/
		}
		IPfbwk.lswk2 = 0x00800000;		/* lswk2 = 0x00800000								*/
#if 0
		if( IPfbwk.lswk11 >= IPfbwk.lswk2 )
		{
			IPfbwk.lswk11 = IPfbwk.lswk11 - ( IPfbwk.lswk2 << 1 );	/* lswk11 = pfbrem - 0x00800000 * 2					*/
			IPfbwk.lswk10 = IPfbwk.lswk10 + ONER;	/* lswk10 = lswk10 + 1							*/
		}
#endif
		DivPlsV.Iu0.l = IPfbwk.lswk11;		/* lswk11 --> pfbrem									*/
		DivPlsV.Ret0.l = IPfbwk.lswk10;			/* lswk10 --> DivPlsV.Ret0										*/
/*--------------------------------------------------------------------------------------------------*/
		return;
}
#endif



/***************************************** end of file **********************************************/

