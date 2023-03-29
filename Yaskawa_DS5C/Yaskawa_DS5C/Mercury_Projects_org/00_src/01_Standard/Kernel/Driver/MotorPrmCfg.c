/****************************************************************************************************/
/*																									*/
/*																									*/
/*		MotorPrmCfg.c : ���[�^�G���R�[�_�֘A�������W���[��											*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : ���[�^�G���R�[�_�֘A�����֐����`�B													*/
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
#include "SerialEncIf.h"
#include "JL086ServoIpHw.h"
#include "MLib.h"
#include "KLib.h"
#include "PnPrmListTbl.h"

#define	SVCDEF_DIVOUT_MAXFREQ		1802240				/* �����o�̓p���X������g����`				*/
#define	SVFSEL_INCPOUT				1					/* �C���N���p���X�o��						*/
#define	SVFSEL_PAOPOUT				1					/* �o�`�n���p���X�o��						*//*<S02D>*/

#define	UNIT_FEMT					-15
#define	UNIT_PICO					-12
#define	UNIT_NANO					-9

#define	EvenAsciiCode_A				( 'A'  + 0x00 )		/* 0 100 0001	 <S14F> */
#define	EvenAsciiCode_L				( 'L'  + 0x80 )		/* 1 100 1100	 <S14F> */
#define	EvenAsciiCode_M				( 'M'  + 0x00 )		/* 0 100 1101	 <S14F> */
#define	EvenAsciiCode_P				( 'P'  + 0x00 )		/* 0 101 0000	 <S14F> */
#define	EvenAsciiCode_Puls			( '+'  + 0x00 )		/* 0 010 1011	 <S14F> */
#define	EvenAsciiCode_Conm			( ','  + 0x80 )		/* 1 010 1100	 <S14F> */
#define	EvenAsciiCode_Mins			( '-'  + 0x00 )		/* 0 010 1101	 <S14F> */
#define	EvenAsciiCode_Prio			( '.'  + 0x00 )		/* 0 010 1110	 <S14F> */
#define	EvenAsciiCode_Cr			( 0x0D + 0x80 )		/* 1 000 1101	 <S14F> */

/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
static	LONG	RmxSencCalMencDivOverSpeed( LONG pgrate, KSGAIN OvrSpd );
static DIVSET_ERROR RmxSencSetMencDividedPulse( ASIC_HW_IF_STR *AsicHwReg, MENCV *MencV, UNI_PRM_STR *UniPrms );
static DIVSET_ERROR LmxSencSetMencDividedPulse( ASIC_HW_IF_STR *AsicHwReg, MENCV *MencV, UNI_PRM_STR *UniPrms );
static DIVSET_ERROR LmxSencCalMencDivOverSpeed( MENCV *MencV, UNI_PRM_STR *UniPrms );

static LONG	SencCheckMturnLimit( LONG MtnCur, MENCV *MencV );							  			  /*<S02D>*/
static void	LmxSencGetLinearAbsPos( MENCV  *MencV, BOOL RvsDir ); 									  /*<S02D>*/
static void	SencOutputPaoMtdata( MENCV *MencV, ASIC_HW_IF_STR *pAsicHwReg, ULONG digit );  			  /*<S02D>*/
static DIVSET_ERROR RmxSencSetMencDividedPulseHw( ASIC_HW_IF_STR *AsicHwReg, MENCV *MencV, UNI_PRM_STR *UniPrms );/*<S02D>*/
static DIVSET_ERROR LmxSencSetMencDividedPulseHw( ASIC_HW_IF_STR *AsicHwReg, MENCV *MencV, UNI_PRM_STR *UniPrms );/*<S079>*/
static BOOL SencSetSensorType( MENCV *MencV, MENCPRM *MencP, USHORT b_prm2 );//<S0C7>
void SencOutputPsoSerial    (MENCV*,ASIC_HW_IF_STR*,SHORT,BOOL);/*<S189>*//*<S0C2>*/
void FencSencOutputPsoSerial(MENCV*,ASIC_HW_IF_STR*,SHORT,BOOL);/*<189>*//*<S0C2>*/
static void	OutputPsoMturnIncre( MENCV*,ULONG, ULONG,ASIC_HW_IF_STR*);/*<S189>*//*<S0C2>*/
static void	OutputPsoLinearIncre(LONG *PsoIncPulseOut, ULONG digit, ASIC_HW_IF_STR *pAsicHwReg);	/* <S20A>:���������� */


/****************************************************************************************************/
/*																									*/
/*		���j�A���[�^�F�X�P�[���s�b�`�Z�o����														*/
/*																									*/
/****************************************************************************************************/
static BOOL	SencScalePitchCalc( MENCV *MencV )
{
	LONG			i;
	LONG			Index;
	ULONG			ulwk64[2];
	ULONG			ScalePitch;
	ULONG			ScaleExp;

/*--------------------------------------------------------------------------------------------------*/
/*		�X�P�[���s�b�`�v�Z	�P�ʁF[fm]																*/
/*--------------------------------------------------------------------------------------------------*/
	ScalePitch = (MencV->ScalePitch[2]<<16) | (MencV->ScalePitch[1]<<8) | MencV->ScalePitch[0];
	ScaleExp = 1;
	for( i=0; i < MencV->ScalePitchExp; i++ )
	{
		ScaleExp = 10 * ScaleExp;
	}
	MlibMulu32u32( ScalePitch, ScaleExp, ulwk64 );

/*--------------------------------------------------------------------------------------------------*/
/*		�X�P�[���s�b�`�㉺���`�F�b�N																*/
/*			�����l�F0x0000F4240(=1000000)[fm]														*/
/*			����l�F0x5AF3FFFFFFFF(=100004018520063)[fm]											*/
/*--------------------------------------------------------------------------------------------------*/
	if( ulwk64[1]==0 )
	{
		if( ulwk64[0] < 0x000F4240 )
		{ /* �����l��菬���� */
			return( FALSE );
		}
	}
	else
	{
		if( ulwk64[1] > 0x00005AF3 )
		{ /* ����l���傫�� */
			return( FALSE );
		}
	}

/*--------------------------------------------------------------------------------------------------*/
/*		���j�^�p�X�P�[���s�b�`�A�X�P�[���s�b�`�w���ݒ�	�P�ʁF[pm]									*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->MonScalePitch = ScalePitch;
	MencV->MonScaleExp = UNIT_FEMT - UNIT_PICO + MencV->ScalePitchExp;/* [pm]�P�ʂ̎w���ɕϊ�	*/

/*--------------------------------------------------------------------------------------------------*/
/*		�X�P�[���s�b�`�A�X�P�[���s�b�`�w���ݒ�	�P�ʁF[nm]											*/
/*--------------------------------------------------------------------------------------------------*/
	Index = UNIT_FEMT - UNIT_NANO + MencV->ScalePitchExp;				/* [nm]�P�ʂ̎w���ɕϊ�	*/

	if( Index >= 0 )
	{
		ScaleExp = 1;
		for( i=0; i < Index; i++ )
		{
			ScaleExp = 10 * ScaleExp;
		}
		MencV->NormScalePitch = MlibScalKxgain( ScalePitch, ScaleExp, 1, NULL, -30 );
		MencV->NormScaleExp   = 1;
	}
	else
	{
		ScaleExp = 1;
		Index = -Index;
		for( i=0; i < Index; i++ )
		{
			ScaleExp = 10 * ScaleExp;
		}

		if( ScaleExp >= 10000)
		{
			MencV->NormScalePitch = MlibScalKxgain( ScalePitch, 10000, ScaleExp, NULL, -30 );
			MencV->NormScaleExp   = 10000;
		}
		else
		{
			MencV->NormScalePitch = ScalePitch;
			MencV->NormScaleExp   = ScaleExp;
		}
	}
	return( TRUE );
}


/****************************************************************************************************/
/*																									*/
/*		��]�^�F�G���R�[�_��{�p�����[�^�v�Z														*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �T�[�{��{�p�����[�^�v�Z(BprmCalc.c)�ɕK�v�ȃG���R�[�_�p�����[�^�̌v�Z���s���B			*/
/*			��{�p�����[�^�v�Z����(Bprm.Xxxx/Hprm.Xxxx)����я����p�����[�^�v�Z����(Iprm.Xxxx)		*/
/*			���g�p���Ȃ��p�����[�^�v�Z�́A�����ōs���Ă��ǂ��B										*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static SENC_ERROR	SencBaseParamCalcForRotary( MENCV *MencV, MENC_INIT_PRM *MencInitPrm )
{
	/*----------------------------------------------------------------------------------------------*/
	/*		�ʐM�ُ팟�o���x���ݒ�																	*/
	/*----------------------------------------------------------------------------------------------*/
	MencV->ComErrLimit = MencInitPrm->spgfail.b.l;

	/*----------------------------------------------------------------------------------------------*/
	/*		�G���R�[�_�p���X���̐ݒ�																*/
	/*----------------------------------------------------------------------------------------------*/
	/*																								*/
	/*		PulseNo = 2^BitNo	[pulse/rev]															*/
	/*																								*/
	/*----------------------------------------------------------------------------------------------*/
	MencV->PulseNo = (1 << MencInitPrm->MencP->bit_dp.b.l);			/* PulseNo = 2^BitNo			*/
	MencV->HalfPulseNo = (MencV->PulseNo >> 1);						/* HalfPulseNo = PulseNo / 2	*/

	MencV->EncBitNo = MencInitPrm->MencP->bit_dp.b.l;				/* �G���R�[�_�r�b�g��			*/	/* <S14B> */

#if 0 /*<S02D>*/
	if( MencInitPrm->MencP->bit_dp.b.l <= 13 )						/* �p���X�ϊ�13bit�w��			*/
	{
		MencV->PlsTrn13bit = TRUE;
	}
	else
	{
		MencV->PlsTrn13bit = FALSE;
	}
#else/*<S02D>*/
	if( MencInitPrm->MencP->bit_dp.b.l <= 13 )						/* �p���X�ϊ�13bit�w��			*/
	{
		MencV->DivInfo.PlsTrn13bit = TRUE;
	}
	else
	{
		MencV->DivInfo.PlsTrn13bit = FALSE;
	}

#endif/*<S02D>*/
/*--------------------------------------------------------------------------------------------------*/
/*		�p���[�o�̓��[�^�����ݒ�																	*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->PhaseOrderUWV = FALSE;								/* ���݁A�s�g�p					*/
/*--------------------------------------------------------------------------------------------------*/
/*		�|�[���Z���T�L���ݒ�												<V250>					*/
/*			��]�^���Ѓ��[�^�Ή����l�����Ȃ��`�ł���B												*/
/*			���샂�[�^�̓|�[���Z���T����̂��߃z�[���Z���T�L�����Œ�ɂ���							*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->withPoleSensor = TRUE;								/* ���샂�[�^�̂ݑΉ�			*/
/*--------------------------------------------------------------------------------------------------*/
/*		�}���`�^�[�����~�b�g�`�F�b�N																*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->DivInfo.Limmlt = MencInitPrm->MencP->limmlt;   /*<S02D>*/
	MencV->DivInfo.Bit_dp = MencInitPrm->MencP->bit_dp;   /*<S02D>*/

#if !defined(CSW_NETIF_SYNQNET_TYPE)
	if( (MencV->AbsoEncType == MENC_MABSO_TYPE)
	/*  ||((MencV->AbsoEncType == MENC_SABSO_TYPE)&&(Kprm.NetId == NETID_DPRAM)) */ )	/*<S03F>*/
	{
		if(/* ( MencV->DivOut == TRUE ) &&*/ ( MencInitPrm->mtlmt != MencInitPrm->MencP->limmlt ) )
		{
			return SENCERR_MTLMT;/* A.CC0 : �}���`�^�[�����~�b�g�l�s��v */
		}
	}
#endif
	return SENCERR_SUCCESS;
}


/****************************************************************************************************/
/*																									*/
/*		���j�A���[�^�F�G���R�[�_��{�p�����[�^�v�Z													*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �T�[�{��{�p�����[�^�v�Z(BprmCalc.c)�ɕK�v�ȃG���R�[�_�p�����[�^�̌v�Z���s���B			*/
/*			��{�p�����[�^�v�Z����(Bprm.Xxxx/Hprm.Xxxx)����я����p�����[�^�v�Z����(Iprm.Xxxx)		*/
/*			���g�p���Ȃ��p�����[�^�v�Z�́A�����ōs���Ă��ǂ��B										*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static SENC_ERROR	SencBaseParamCalcForLinear( MENCV *MencV, MENC_INIT_PRM *MencInitPrm )
{
	/*----------------------------------------------------------------------------------------------*/
	/*		�ʐM�ُ팟�o���x���ݒ�																	*/
	/*----------------------------------------------------------------------------------------------*/
	MencV->ComErrLimit = MencInitPrm->spgfail.b.l;

	/*----------------------------------------------------------------------------------------------*/
	/*		�X�P�[���s�b�`�̐ݒ�																	*/
	/*----------------------------------------------------------------------------------------------*/
	if(MencV->ScalePitchExp == 0xFF)
	{ /* �V���A���R���o�[�^�̃X�P�[���s�b�`��񖳌� */
		/*------------------------------------------------------------------------------------------*/
		/*	Pn282���X�P�[���s�b�`�ݒ�																*/
		/*------------------------------------------------------------------------------------------*/
		MencV->MonScalePitch = MencInitPrm->scalepitch2l;
		MencV->MonScaleExp = SCALE_EXP_PM; /* [pm]�P�ʂ̎w��			*/
		MencV->NormScalePitch = MencInitPrm->scalepitch2l * 10;
		MencV->NormScaleExp = SCALE_EXP_UM; /* [nm]�P�ʂ̎w��			*/
	}
	else
	{ /* �V���A���R���o�[�^�̃X�P�[���s�b�`���L�� */
		/* �V���A���R���o�[�^�̏����X�P�[���s�b�`�ݒ� */
		if(FALSE == SencScalePitchCalc(MencV))
		{
			/* �v�Z���ʂ��m�f�̏ꍇ�N���A */
			MencV->MonScalePitch = 0;
			MencV->MonScaleExp = 0;
			MencV->NormScalePitch = 0;
			MencV->NormScaleExp = 0;
		}
	}

	/*----------------------------------------------------------------------------------------------*/
	/*		�G���R�[�_�p���X���̐ݒ�																*/
	/*----------------------------------------------------------------------------------------------*/
	/*																								*/
	/*					(2*PolePitch) * (100000) * (2^BitNo)		PulseNo    : [pulse/360deg]		*/
	/*		PulseNo = -----------------------------------------		PolePitch  : [0.1mm/180deg]		*/
	/*								ScalePitch						ScalePitch : [nm]				*/
	/*																								*/
	/*		���ӁFPulseNo�ɗ]�肪����ꍇ�A�ʏ�̉��Z�ł́A���Ɉʒu�̉��Z�덷���ݐς���B			*/
	/*			  1) PulseNo = 460800 [pulse/360deg], PolePitch=18.0mm, ScalePitch=20.00um			*/
	/*			  2) PoleErr = 360 / 460800  [deg/(2*PolePitch)]									*/
	/*			  3) PoleErr = (360/460800) * 1000/(2*18.0) = 0.02 [deg/m]							*/
	/*			  4) �ݐό덷��������(0.02deg/m)�̂ŁA���ʂ͖�������B								*/
	/*																								*/
	/*----------------------------------------------------------------------------------------------*/
	if(MencV->NormScalePitch != 0)
	{
		MencV->PulseNo = MlibDiv6432( MencInitPrm->MencP->ppitch * MencV->NormScaleExp,
									  (200000 << MencInitPrm->MencP->bit_dp.b.l),
									  MencV->NormScalePitch );
		/*------------------------------------------------------------------------------------------*/
#if 0
		if( 0 ) /* PulseNo�]��`�F�b�N�F�ݐό덷���������̂ŁA���ʂ͖�������(SGDV�d�l)�B	*/
		{
			MlibMulu32u32( MencV->PulseNo, MencV->NormScalePitch, xx );
			MlibMulu32u32((prm_ptr->ppitch*MencV->NormScaleExp),(200000 << prm_ptr->bit_dp.b.l), yy );
			if( (xx[0] != yy[0]) || (xx[1] != yy[1]) ) /* �]��`�F�b�N							*/
			{
				KpiSetParamError( &pndef_ppitch ); /* A.040 : �p�����[�^�G���[�̐ݒ�		*/
			}
		}
#endif
	}
	/*----------------------------------------------------------------------------------------------*/
	else
	{
		return SENCERR_SCALE;								/* A.080 : �X�P�[���s�b�`�ݒ�ُ�		*/
	}
	/*----------------------------------------------------------------------------------------------*/
	MencV->HalfPulseNo = (MencV->PulseNo >> 1);				/* HalfPulseNo = PulseNo / 2			*/
//	MencV->PlsTrn13bit = FALSE;								/* �p���X�ϊ�13bit�w��Flag				*/
	MencV->DivInfo.PlsTrn13bit = FALSE;						/* �p���X�ϊ�13bit�w��Flag				*//*<S02D>*/

	/*----------------------------------------------------------------------------------------------*/
	/*		���_�`�F�b�N�p�p���X���̐ݒ�															*/
	/*----------------------------------------------------------------------------------------------*/
	MencV->PulseNo30deg = MencV->PulseNo / 12;				/* PulseNo30deg [Pulse/30deg]			*/

	/*----------------------------------------------------------------------------------------------*/
	/*		Pn080.0 : �|�[���Z���T�L���ݒ�															*/
	/*----------------------------------------------------------------------------------------------*/
	MencV->withPoleSensor = (0 == (MencInitPrm->b_prm80 & 0x0F)) ? TRUE : FALSE;
	/*----------------------------------------------------------------------------------------------*/
	/*		Pn080.1 : ���[�^�����ݒ�																*/
	/*----------------------------------------------------------------------------------------------*/
	MencV->PhaseOrderUWV = (0 == ((MencInitPrm->b_prm80 >> 4) & 0x0F)) ? FALSE : TRUE;
	/*----------------------------------------------------------------------------------------------*/
	/*		Pn080.2 : �t�����b�`�Ď��I��ݒ�														*/
	/*----------------------------------------------------------------------------------------------*/
	MencV->MotOrgChkAlways = (0 == ((MencInitPrm->b_prm80 >> 8) & 0x0F)) ? TRUE : FALSE;
	/*----------------------------------------------------------------------------------------------*/
	/*		Pn080.3 : �ݒ�\�ȍő呬�x�E������v�Z�I��ݒ�										*/
	/*----------------------------------------------------------------------------------------------*/
//	MencV->PhaseOrderUWV = (0 == ((b_prm80 >> 12) & 0x0F)) ? FALSE : TRUE;

	return SENCERR_SUCCESS;
}

/****************************************************************************************************/
/*																									*/
/*		�V���A���G���R�[�_�̕ϐ�����������															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �C�j�V���������ɂ����āA�V���A���G���R�[�_�̏���ʒu�f�[�^��ǂݍ��݁A					*/
/*			�ʒu�f�[�^�A�O��ʒu�̐ݒ���s���B														*/
/*			�܂��A�G���R�[�_�^�C�v�ɂ���Ăb���o�́A�����p���X���o���Ȃ��ݒ������B				*/
/*																									*/
/****************************************************************************************************/
SENC_ERROR	SencCalcBaseParameters(MENCV *MencV, MENC_INIT_PRM *MencInitPrm )
{
	SENC_ERROR	err;

	/*----------------------------------------------------------------------------------------------*/
	/*		�����p���X�o�͑I��Flag�̐ݒ�															*/
	/*----------------------------------------------------------------------------------------------*/
	/* 2011.01.11 Y.Oka �����p���X�o�͖��Ή� */
	//	MencV->DivOut = TRUE;
#if 0 /*<S02D>*/
    	MencV->DivOut = FALSE;
	/*----------------------------------------------------------------------------------------------*/
	/*		�T���v���Ńn�[�h�Ή�																	*/
	/*----------------------------------------------------------------------------------------------*/
	if( MencV->DivOutEnable == TRUE )
	{
		MencV->DivOut = TRUE;
	}
#endif /*<S02D>*/
	/*----------------------------------------------------------------------------------------------*/
	/*		�G���R�[�_���ڑ����̏���																*/
	/*----------------------------------------------------------------------------------------------*/
	if( MencV->EncConnect == FALSE )
	{
		MencV->AbsoEncType = MENC_INCRE_TYPE;
		MencV->AxisMotType = MOTTYPE_ROTARY;
	}

	/*----------------------------------------------------------------------------------------------*/
	/*		�V���A���G���R�[�_��{�p�����[�^�v�Z													*/
	/*----------------------------------------------------------------------------------------------*/
	if( MencV->AxisMotType == MOTTYPE_LINEAR )
	{ /* ���j�A�^ */
		err = SencBaseParamCalcForLinear( MencV, MencInitPrm );
	}
	else
	{ /* ��]�^ */
		err = SencBaseParamCalcForRotary( MencV, MencInitPrm );
	}

	return err;
}


/****************************************************************************************************/
/*																									*/
/*		���[�^�d���R�[�h�ϊ�																		*/
/*																									*/
/****************************************************************************************************/
static UCHAR	SencGetMotorVoltCode( USHORT volt )
{
	switch( volt )
	{
	case 100 : return( 0x00 );								/* AC 100V								*/
	case 200 : return( 0x01 );								/* AC 200V								*/
	case 400 : return( 0x02 );								/* AC 400V								*/
	case  24 : return( 0x03 );								/* DC  24V								*/
	case  48 : return( 0x04 );								/* DC  48V								*/
	default  : return( 0x00 );								/* Default : AC 100V					*/
	}
}


#pragma arm section code = "CALL_SCANA" /* <S1D2> */
/****************************************************************************************************/
/*																									*/
/*		�}���`�^�[���f�[�^�擾����																	*/
/*																									*/
/****************************************************************************************************/
static	USHORT	SencGetMturnData(MENCV *MencV, USHORT limmlt, DBYTEX bit_dp, USHORT PosH, ULONG PosL )
{
	USHORT	Mturn;

/*--------------------------------------------------------------------------------------------------*/
/*		����]�A�u�\�G���R�[�_																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->AbsoEncType == MENC_MABSO_TYPE )
	{
		Mturn = (USHORT)(((PosH<<16) | ((ULONG)PosL>>16)) >> (bit_dp.b.h - 64));
		if( MencV->RevAsmMotor )
		{
			if( limmlt == 65535 )
			{
				Mturn = -Mturn;
			}
			else if( Mturn != 0 )
			{
				Mturn = limmlt - Mturn;
			}
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�P��]�A�u�\�G���R�[�_																		*/
/*--------------------------------------------------------------------------------------------------*/
	else
	{
		Mturn = 0;
	}
/*--------------------------------------------------------------------------------------------------*/
	return( Mturn );

}
#pragma arm section code


/****************************************************************************************************/
/*																									*/
/*		�A�u�\�G���R�[�_�^�C�v�̐ݒ�																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �G���R�[�_�`�����As-abs(***A*-*****)�̂Ƃ��Ăяo���� �APn002.2���						*/
/*			�A�u�\�R�[�_�^�C�v(MencP.AbsoEncType)��ݒ肷��B										*/
/*																									*/
/*			Pn002.2 = 0 : �G���R�[�_�̎d�l�ʂ�Ɏg�p												*/
/*			Pn002.2 = 1 : �A�u�\���C���N���Ƃ��Ďg�p												*/
/*			Pn002.2 = 2 : ���]�A�u�\�G���R�[�_�Ƃ��Ďg�p				 <S16A> 					*/
/****************************************************************************************************/
static void	SencSetAbsoEncType( MENCV *MencV, USHORT b_prm2, USHORT typm_e, USHORT *limmlt )
{
	if( 1 == ((b_prm2>>8) & 0x0F) )
	{	/* �A�u�\���C���N���Ƃ��Ďg�p	*/
		MencV->AbsoEncType = MENC_INCRE_TYPE;
	}
	else if(( 2 == ((b_prm2>>8) & 0x0F) ) && (MencV->SerialConvType == SCONVTYPE_NONE))
	{	/* �V���A�����]�A�u�\�G���R�[�_�Ƃ��Ďg�p	*/
		*limmlt = 0;
		MencV->AbsoEncType = MENC_SABSO_TYPE;
	}
	else
	{	/* �G���R�[�_�̎d�l�ʂ�Ɏg�p */
		if(typm_e & 0x1000)
		{
			MencV->AbsoEncType = MENC_MABSO_TYPE;
		}
		else if(typm_e & 0x2000)
		{
			MencV->AbsoEncType = MENC_SABSO_TYPE;
		}
		else
		{
			MencV->AbsoEncType = MENC_INCRE_TYPE;
		}
	}
}

/****************************************************************************************************/
/*																									*/
/*		�p�����[�^�w�������Z																		*/
/*																									*/
/****************************************************************************************************/
static BOOL	SencExpConvert( UCHAR prm, UCHAR *buf )
{
	BOOL	rc;
	SHORT	swrk;

	rc = TRUE;

/*--------------------------------------------------------------------------------------------------*/
/*		�w�����̉��																				*/
/*--------------------------------------------------------------------------------------------------*/
	if ( prm == 0xFF )										/* �w�����F�[��(10^0)					*/
	{
		swrk = 0x0000;
	}
	else if ( prm & 0x80 )									/* �w�����F��(10^-n)					*/
	{
		swrk = -(prm & 0x7F);
	}
	else													/* �w�����F��(10^+n)					*/
	{
		swrk = (SHORT)prm;
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�w�����̃`�F�b�N																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( (swrk > 8) || (swrk < -8) )							/* �w�����͈̓`�F�b�N					*/
	{
		rc = FALSE;
	}
/*--------------------------------------------------------------------------------------------------*/
	*buf = (UCHAR)swrk;
	return( rc );
}

/****************************************************************************************************/
/*																									*/
/*		�G���R�[�_�^�C�v�R�[�h�ݒ�																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �G���R�[�_�h�c�����A�A�u�\�^�C���N���^�R���o�[�^�̔��f�A�y�у�-3					*/
/*			�G���R�[�_�̔��f������ݒ���s���B														*/
/*																									*/
/****************************************************************************************************/
BOOL	SencSetEncoderType( MENCV *MencV, MENCPRM *MencP, USHORT b_prm2, BOOL SencFbOptUse ) /* <S16A> */
{
	UCHAR 	*Model;
	BOOL	PrmError;

	Model = &(MencV->EncInfo.Model[0]);
	PrmError = FALSE;

	/*----------------------------------------------------------------------------------------------*/
	/*		�A�u�\�^�C���N���^�R���o�[�^�̔��f														*/
	/*----------------------------------------------------------------------------------------------*/
	MencV->SerialConvType = SCONVTYPE_NONE; /* �V���A���R���o�[�^�^�C�v������		*/
	MencV->OrgClrByWrAdrCmd = FALSE; /* WRADR Command�ɂ�錴�_�N���A����	*/

	if( SencFbOptUse == FALSE )
	{/*	�t�B�[�h�o�b�N�I�v�V�����s�g�p���̏ꍇ�i�]���̏����j										*/
	switch(Model[3])
	{
	case 'A': /* �V���A������]�A�u�\�G���R�[�_												*/
		MencV->AxisMotType = MOTTYPE_ROTARY;
		MencV->AbsoEncoder = TRUE;
		MencP->typm_e.b.h  |= 0x10; /* <S16A> */
		SencSetAbsoEncType(MencV, b_prm2, MencP->typm_e.w, &(MencP->limmlt));	/* <S179> */
		break;
			/*--------------------------------------------------------------------------------------*/
	case 'B': /* �V���A�����]�A�u�\�G���R�[�_(�A�u�\�d�l)									*/
		MencV->AxisMotType = MOTTYPE_ROTARY;
		MencV->AbsoEncoder = TRUE;
		MencP->limmlt = 0;
		MencP->typm_e.b.h  |= 0x20; /* <S16A> */
		SencSetAbsoEncType(MencV, b_prm2, MencP->typm_e.w, &(MencP->limmlt));	/* <S16A> */
		break;
			/*--------------------------------------------------------------------------------------*/
	case 'S': /* �V���A�����]�A�u�\�G���R�[�_(�C���N���d�l)									*/
		MencV->AxisMotType = MOTTYPE_ROTARY;
		MencV->AbsoEncoder = TRUE;
		MencP->limmlt = 0;
		if(((b_prm2>>8) & 0x0F)== 2)
		{
			MencP->typm_e.b.h |= 0x20;
		}
		else
		{
			MencP->typm_e.b.h &= 0x0F;
		}
		SencSetAbsoEncType(MencV, b_prm2, MencP->typm_e.w, &(MencP->limmlt));	/* <S16A> */
		break;
			/*--------------------------------------------------------------------------------------*/
	case 'I': /* �V���A���C���N���G���R�[�_													*/
		MencV->AxisMotType = MOTTYPE_ROTARY;
		MencV->AbsoEncoder = FALSE;
		MencV->AbsoEncType = MENC_INCRE_TYPE;
		MencP->typm_e.b.h &= 0x0F;
		break;
			/*--------------------------------------------------------------------------------------*/
	case 'P': /* �V���A���R���o�[�^ for ���j�A���[�^											*/
			PrmError = SencSetSensorType( MencV, MencP, b_prm2 );									/* <S04D> �֐��� */
			break;
			/*--------------------------------------------------------------------------------------*/
		default: // �f�t�H���g�F�p�����[�^�ُ�
			MencV->AxisMotType = MOTTYPE_ROTARY;
			/* A.CA0 : �G���R�[�_�p�����[�^�ُ� */
			PrmError = TRUE;
			break;
			/*--------------------------------------------------------------------------------------*/
		}
	
		/*------------------------------------------------------------------------------------------*/
		/*		��-3�G���R�[�_����																	*/
		/*	<S04D>�F�V���A���ϊ����j�b�g�܂��͎ЊO��]�`�G���R�[�_(�`���FJZDP)�̏ꍇ�̏�����ǉ�	*/
		/*------------------------------------------------------------------------------------------*/
		if( Model[3] == 'P')
		{/* �V���A���ϊ����j�b�g�܂��͎ЊO��]�`�G���R�[�_(�`���FJZDP)�̏ꍇ						*/
			if( MencV->SenSpecEnable == TRUE )
			{/* Encoder Prm 0x43 ���L���̏ꍇ�́A0x43��bit4,5�ɂ���ăt�H�[�}�b�g��ݒ�				*/
				switch( (MencV->SenSpec >> 4) & 0x03 )
				{
			/*----------------------------------------------------------------------------------*/
				case 0x00 :	/* ��-�U�t�H�[�}�b�g	*/
					MencV->Sigma3Encoder = FALSE;
			break;
			/*----------------------------------------------------------------------------------*/
				case 0x01 :	/* ��-�V�t�H�[�}�b�g	*/
					MencV->Sigma3Encoder = TRUE;
			break;
			/*----------------------------------------------------------------------------------*/
				case 0x02 :	/* ��-�X�t�H�[�}�b�g	*/
					MencV->Sigma3Encoder = TRUE;
			PrmError = TRUE;
			break;
				/*----------------------------------------------------------------------------------*/
				default	  :	/* �f�t�H���g			*/
					MencV->Sigma3Encoder = FALSE;
		PrmError = TRUE;
		break;
	}
			}
			else
			{/* Encoder Prm 0x43 �������̏ꍇ�́A���|�U�t�H�[�}�b�g�Ƃ��� */
				MencV->Sigma3Encoder = FALSE;
			}
		}
		else
		{/* ����W����]�`�G���R�[�_�i�`���FUT**)�̏ꍇ												*/
	switch(Model[2])
	{
	case 'D':
	case 'S':
		MencV->Sigma3Encoder = FALSE;
		break;
	case 'T':
		MencV->Sigma3Encoder = TRUE;
		break;
	default:
		PrmError = TRUE;
		MencV->Sigma3Encoder = FALSE;
		break;
	}
		}

		if( Model[3] == 'P')
		{/* JZDP�̏ꍇ */
			if( MencV->SenSpecEnable == TRUE )
			{
				/* ���Ұ�43H���L���ȏꍇ��43H�̃r�b�g3�Ŕ��f */
				MencV->PoleWriteEnable = ((MencV->SenSpec & 0x08) == 0x08)? TRUE : FALSE;
			}
			else
			{
				/* ���Ұ�43H�������ȏꍇ�̓^�C�v�Ŕ��f */
				if(( MencV->SerialConvType == SCONVTYPE_MITUTOYO_ABS )||( MencV->SerialConvType == SCONVTYPE_ENDAT_ABS ))
		        {
		            MencV->PoleWriteEnable = TRUE;
				}
				else
				{
					MencV->PoleWriteEnable = FALSE;
				}
			}
		}
		else
		{/* ����W����]�`�G���R�[�_�̏ꍇ�͖��� */
			MencV->PoleWriteEnable = FALSE;
		}
	}
	else/* <S04D> */
	{/*	�t�B�[�h�o�b�N�I�v�V�����g�p���̏ꍇ														*/
		/*------------------------------------------------------------------------------------------*/
		/*	�G���R�[�_�ʒu�Z���T�^�C�v�ݒ�															*/
		/*------------------------------------------------------------------------------------------*/
		PrmError = SencSetSensorType( MencV, MencP, b_prm2 );
		/*------------------------------------------------------------------------------------------*/
		/*	���|�U�^���|�V�G���R�[�_����															*/
		/*------------------------------------------------------------------------------------------*/
		/*	�t�B�[�h�o�b�N�I�v�V�����g�p���̏ꍇ�A���|�U�^�C�v�݂̂̂��ߔ��ʕs�v�BFALSE�ݒ�Ƃ���B	*/
		/*------------------------------------------------------------------------------------------*/
		MencV->Sigma3Encoder = FALSE;
		/*------------------------------------------------------------------------------------------*/
		/*		�d���ʑ��p�����[�^�X�P�[���������݉ېݒ�											*/
		/*------------------------------------------------------------------------------------------*/
		/*	MencP.SenSpec(Addr:0x43) �� bit7=1,bit6=0 �̂Ƃ��̂݃p�����[�^��񂪗L��				*/
		/*------------------------------------------------------------------------------------------*/
		if( MencV->SenSpecEnable == TRUE )
		{
			MencV->PoleWriteEnable = ((MencV->SenSpec & 0x08) == 0x08)? TRUE : FALSE;
		}
		else
		{/* ���Ұ�43H�������ȏꍇ�̓^�C�v�Ŕ��f */
			if(( MencV->SerialConvType == SCONVTYPE_MITUTOYO_ABS )||( MencV->SerialConvType == SCONVTYPE_ENDAT_ABS ))
	        {
	            MencV->PoleWriteEnable = TRUE;
			}
			else
			{
				MencV->PoleWriteEnable = FALSE;
			}
		}
	}
#if 0
/* <S04D>:�t�B�[�h�o�b�N�I�v�V�����ɂ��Z�~�N���ɑΉ����邽�߁A�֐�������B				*/
/* SencSetMotorAndEncoderID()�֐��Ńt�B�[�h�o�b�N�I�v�V�������W���[�����ݒ��Ɏ��{����B	*/
		/*------------------------------------------------------------------------------------------*/
	/*		���[�^�萔�X�P�[�������݉۔���														*/
		/*------------------------------------------------------------------------------------------*/
	if(MencV->AxisMotType == MOTTYPE_ROTARY)
	{
		MencV->ScaleWriteEnable = FALSE;
	}
	else if(MencV->AxisMotType == MOTTYPE_LINEAR)
	{
		if(((MencV->TypeCode & 0x0080) == FALSE) ||
			((MencV->EncConnect == TRUE) && (Model[1] == '\0') && (Model[0] == '\0')))
		{
			MencV->ScaleWriteEnable = TRUE;
		}
		else
		{
			MencV->ScaleWriteEnable = FALSE;
		}
	}
	else
	{
		MencV->ScaleWriteEnable = FALSE;
	}
#endif
	return PrmError;
}

/****************************************************************************************************/
/*																									*/
/*		�G���R�[�_�ʒu�Z���T�^�C�v�ݒ�																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �G���R�[�_�p�����[�^���A�A�u�\�^�C���N���^�R���o�[�^�Ȃǔ��f���A�����ϐ���ݒ肷��B	*/
/*																									*/
/****************************************************************************************************//* <S04D> */
static BOOL SencSetSensorType( MENCV *MencV, MENCPRM *MencP, USHORT b_prm2 )
{
	BOOL	PrmError;
	
	MencV->AxisMotType = MOTTYPE_LINEAR;
	PrmError = FALSE;
	
	switch(MencV->TypeCode)
	{
	/*----------------------------------------------------------------------------------*/
	case 'A': /* �~�c�g���A�u�\�^�C�v													*/
		MencV->SerialConvType = SCONVTYPE_MITUTOYO_ABS;
		MencV->AbsoEncoder = TRUE;
		MencP->typm_e.b.h |= 0x10; /* <S16A> */
		SencSetAbsoEncType(MencV, b_prm2, MencP->typm_e.w, &(MencP->limmlt));	/* <S16A> */
		MencV->AxisMotType = MOTTYPE_LINEAR;
		break;
	/*----------------------------------------------------------------------------------*/
	case 'B': /* ��]�`�C���N���G���R�[�_�AINIT�R�}���h�Ή�								*/
		MencV->SerialConvType = SCONVTYPE_SONY_INC;
		MencV->AbsoEncoder = FALSE;
		MencP->typm_e.b.h &= 0x0F;
		MencV->AbsoEncType = MENC_INCRE_TYPE;
		MencV->AxisMotType = MOTTYPE_ROTARY;
		break;
	/*----------------------------------------------------------------------------------*/
	case 'C': /* ��]�`��Βl�G���R�[�_�A	���_�ݒ�\								*/
		MencV->SerialConvType = SCONVTYPE_MITUTOYO_ABS;
		MencV->AbsoEncoder = TRUE;
		if( (MencV->SenSpecEnable == TRUE)&&((MencV->SenSpec & 0x02) == 0x02) )
		{
			MencP->typm_e.b.h |= 0x20;					/* �}���`�^�[���Ȃ�(1��]��Βl)*/
			MencP->limmlt = 0;
		}
		else
		{
			MencP->typm_e.b.h |= 0x10;					/* �}���`�^�[������(����]��Βl)*/
		}
		SencSetAbsoEncType(MencV, b_prm2, MencP->typm_e.w, &(MencP->limmlt));	/* <S16A> */
		MencV->AxisMotType = MOTTYPE_ROTARY;
		break;
	/*----------------------------------------------------------------------------------*/
	case 'D': /* �r�l�r�C���N���^�C�v													*/
		MencV->SerialConvType = SCONVTYPE_SONY_INC;
		MencV->AbsoEncoder = FALSE;
		MencP->typm_e.b.h &= 0x0F;
		MencV->AbsoEncType = MENC_INCRE_TYPE;
		MencV->AxisMotType = MOTTYPE_LINEAR;
		break;
	/*----------------------------------------------------------------------------------*/
	case 'E': /* �d���c�����^�C�v														*/
		MencV->SerialConvType = SCONVTYPE_ENDAT_ABS;
		MencV->AbsoEncoder = TRUE;
		MencP->typm_e.b.h |= 0x10;  /* <S16A> */
		SencSetAbsoEncType(MencV, b_prm2, MencP->typm_e.w, &(MencP->limmlt));	/* <S16A> */
		MencV->AxisMotType = MOTTYPE_LINEAR;
		break;
	/*----------------------------------------------------------------------------------*/
	case 'G': /* ��]�`��Βl�G���R�[�_�A���_�ݒ�s��									*/
		MencV->SerialConvType = SCONVTYPE_ENDAT_ABS;
		MencV->AbsoEncoder = TRUE;
		if( (MencV->SenSpecEnable == TRUE)&&((MencV->SenSpec & 0x02) == 0x02) )
		{
			MencP->typm_e.b.h |= 0x20;					/* �}���`�^�[���Ȃ�(1��]��Βl)*/
			MencP->limmlt = 0;
		}
		else
		{
			MencP->typm_e.b.h |= 0x10;					/* �}���`�^�[������(����]��Βl)*/
		}
		SencSetAbsoEncType(MencV, b_prm2, MencP->typm_e.w, &(MencP->limmlt));	/* <S16A> */
		MencV->AxisMotType = MOTTYPE_ROTARY;
		break;
/*--------------------------------------------------------------------------------------*/
	case 'J': /* ��]�C���N���G���R�[�_�AWRITE_ADR�R�}���h�Ή�							*/
		MencV->SerialConvType = SCONVTYPE_STANDARD_INC;
		MencV->AbsoEncoder = FALSE;
		MencP->typm_e.b.h &= 0x0F;
		MencV->AbsoEncType = MENC_INCRE_TYPE;
		MencV->OrgClrByWrAdrCmd = TRUE;		/* WRITE_ADR�R�}���h�ɂ�錴�_�N���A�L��*/
		MencV->AxisMotType = MOTTYPE_ROTARY;
		break;
	/*----------------------------------------------------------------------------------*/
	case 'L': /* �W���C���N���^�C�v														*/
		MencV->SerialConvType = SCONVTYPE_STANDARD_INC;
		MencV->AbsoEncoder = FALSE;
		MencV->AbsoEncType = MENC_INCRE_TYPE;
		MencP->typm_e.b.h &= 0x0F;
		MencV->OrgClrByWrAdrCmd = TRUE; 		/* WRADR Command�ɂ�錴�_�N���A�L��	*/
		MencV->AxisMotType = MOTTYPE_LINEAR;
		break;
	/*----------------------------------------------------------------------------------*/
	default:
		/* A.CA0 : �G���R�[�_�p�����[�^�ُ� */
		PrmError = TRUE;
		break;
	}
	return PrmError;
}

/****************************************************************************************************/
/*																									*/
/*		���[�^�^�C�v�R�[�h�ݒ�																		*/
/*																									*/
/****************************************************************************************************/
static void	SencSetMotorType( MENCV *MencV, MENCPRM *MencP )
 {
	UCHAR *Model;
	UCHAR MotType; /* <S03A> */
	UCHAR Seriese; /* <S03A> */

	Model = &(MencV->MotInfo.Model[0]);
	MencV->TuningLessLowGainMotor = FALSE; /* �������X�@�픻�ʂ��Ȃ��ɏ�����		*/
	MencV->WeakenFieldEnable = FALSE; /* ��ߊE���������L�����ʂ��Ȃ��ɏ�����	*/

	/*----------------------------------------------------------------------------------------------*/
	/*		��]�`���[�^�V���[�Y����												<S03A>			*/
	/*----------------------------------------------------------------------------------------------*/
	if( Model[3] == '7' )
	{ /* ��-7���[�^ */
		Seriese = 3;
		MotType = 4;
	}
	else
	{
		Seriese = 4;
		MotType = 3;
	}

	/*----------------------------------------------------------------------------------------------*/
	/*		���ꃂ�[�^																				*/
	/*----------------------------------------------------------------------------------------------*/
	if((Model[0] != 'S') || (Model[2] != 'M'))
	{
		MencP->typm_e.b.l = 0xFF;
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		�W�����[�^																				*/
	/*----------------------------------------------------------------------------------------------*/
	else
	{
//		switch(Model[3]) /* <S03A> */
		switch(Model[MotType])
		{
		/*------------------------------------------------------------------------------------------*/
		case 'A':
//			switch(Model[4]) /* <S03A> */
			switch(Model[Seriese])
			{
			case 'J':
				MencP->typm_e.b.l = 0x0D;
				break;
			default:
				MencP->typm_e.b.l = 0x00;
				break;
			}
			break;
			/*--------------------------------------------------------------------------------------*/
		case 'P':
			MencP->typm_e.b.l = 0x01;
			break;
			/*--------------------------------------------------------------------------------------*/
		case 'S':
			MencP->typm_e.b.l = 0x02;
			/* SGMS*���[�^�͎�ߊE���������L���Ƃ��� */
			MencV->WeakenFieldEnable = TRUE;
			break;
			/*--------------------------------------------------------------------------------------*/
		case 'G':
			switch(Model[10])
			{
			case 'A':
			case 'C':
				MencP->typm_e.b.l = 0x03;
				/* SGMG*���[�^�͒������X�ᑛ���ݒ�Ƃ��� */
				MencV->TuningLessLowGainMotor = TRUE;
				break;
			case 'B':
			case 'D':
				MencP->typm_e.b.l = 0x04;
				/* SGMG*���[�^�͒������X�ᑛ���ݒ�Ƃ��� */
				MencV->TuningLessLowGainMotor = TRUE;
				break;
			default:
				break;
			}
			break;
			/*--------------------------------------------------------------------------------------*/
		case 'D':
			MencP->typm_e.b.l = 0x05;
			/* SGMD*���[�^�͒������X�ᑛ���ݒ�Ƃ��� */
			MencV->TuningLessLowGainMotor = TRUE;
			break;
			/*--------------------------------------------------------------------------------------*/
		case 'U':
			MencP->typm_e.b.l = 0x06;
			/* SGMU*���[�^�͒������X�ᑛ���ݒ�Ƃ��� */
			MencV->TuningLessLowGainMotor = TRUE;
			/* SGMU*���[�^�͎�ߊE���������L���Ƃ��� */
			MencV->WeakenFieldEnable = TRUE;
			break;
			/*--------------------------------------------------------------------------------------*/
		case 'M':
//			switch(Model[4]) /* <S03A> */
			switch(Model[Seriese])
			{
			case 'J':
				MencP->typm_e.b.l = 0x11;
				break;
			default:
				MencP->typm_e.b.l = 0x07;
				break;
			}
			break;
			/*--------------------------------------------------------------------------------------*/
		case 'B':
			switch(Model[10])
			{
			case 'A':
				MencP->typm_e.b.l = 0x08;
				break;
			case 'B':
				MencP->typm_e.b.l = 0x09;
				break;
			case 'C':
				MencP->typm_e.b.l = 0x0B;
				break;
			case 'D':
				MencP->typm_e.b.l = 0x0C;
				break;
			default:
				break;
			}
			break;
			/*--------------------------------------------------------------------------------------*/
		case 'T':
			MencP->typm_e.b.l = 0x0A;
			break;
			/*--------------------------------------------------------------------------------------*/
		case 'J':
			MencP->typm_e.b.l = 0x0D;
			break;
			/*--------------------------------------------------------------------------------------*/
		case 'K':
			MencP->typm_e.b.l = 0x11;
			break;
			/*--------------------------------------------------------------------------------------*/
		case 'C':
			switch(Model[8])
			{
			case 'C':
				MencP->typm_e.b.l = 0x12;
				break;
			case 'D':
				MencP->typm_e.b.l = 0x13;
				break;
			case 'B':
				MencP->typm_e.b.l = 0x14;
				break;
			case 'E':
				MencP->typm_e.b.l = 0x15;
				break;
			case 'L':
				MencP->typm_e.b.l = 0x16;
				break;
			case 'M':
				MencP->typm_e.b.l = 0x17;
				break;
			case 'N':
				MencP->typm_e.b.l = 0x18;
				break;
			case 'R':
				MencP->typm_e.b.l = 0x19;
				break;
			default:
				MencP->typm_e.b.l = 0x1F;
				break;
			}
			break;
			/*--------------------------------------------------------------------------------------*/
		case 'E':
			MencP->typm_e.b.l = 0x0E;
			break;
			/*--------------------------------------------------------------------------------------*/
		case 'Z':
			MencP->typm_e.b.l = 0x0F;
			break;
			/*--------------------------------------------------------------------------------------*/
		default:
			MencP->typm_e.b.l = 0xFF;
			break;
		}
		/*------------------------------------------------------------------------------------------*/
//		switch(Model[4]) /* <S03A> */
		switch(Model[Seriese])
		{
		case 'S':
			MencP->typm_e.b.l |= 0x20;
			/* �C���N���G���R�[�_���x���o���� */
			MencV->IncDetOhFlg = FALSE; /* <S03A> */
			break;
		case 'V':
			MencP->typm_e.b.l |= 0x60;
			/* V���[�^�͒������X�@�픻�ʂ��Ȃ� */
			MencV->TuningLessLowGainMotor = FALSE;
			/* V���[�^�͎�ߊE���������L�����ʂ��Ȃ� */
			MencV->WeakenFieldEnable = FALSE;
			/* �C���N���G���R�[�_���x���o���� */
			MencV->IncDetOhFlg = FALSE; /* <S03A> */
			break;
		case '7': /* <S03A> */
			MencP->typm_e.b.l |= 0xA0;
			/* 7���[�^�͒������X�@�픻�ʂ��Ȃ� */
			MencV->TuningLessLowGainMotor = FALSE;
			/* 7���[�^�͎�ߊE���������L�����ʂ��Ȃ� */
			MencV->WeakenFieldEnable = FALSE;
			/* 7���[�^�̓C���N���G���R�[�_���x���o�L�� */
			MencV->IncDetOhFlg = TRUE;
			break;
		default:
			break;
		}
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		���j�A���[�^																			*/
	/*----------------------------------------------------------------------------------------------*/
	if(MencV->AxisMotType == MOTTYPE_LINEAR)
	{
		if(MencV->TypeCode == 'E')
		{
			MencP->typm_e.b.l = 0x41;
		}
		else
		{
			MencP->typm_e.b.l = 0x40;
		}
		/* ���j�A���[�^�͒������X�@�픻�ʂ��Ȃ��ɏ����� */
		MencV->TuningLessLowGainMotor = FALSE;
		/* ���j�A���[�^�͎�ߊE���������L�����ʂ��Ȃ� */
		MencV->WeakenFieldEnable = FALSE;
		/*------------------------------------------------------------------------------------------*/
		/*	�R�A���X���j�A���[�^�`������	<V108>													*/
		/*------------------------------------------------------------------------------------------*/
		switch(Model[3])
		{
		case 'G':
			MencV->CoreLessLinearMotor = TRUE; /* �R�A���X���j�A				*/
			break;
		default:
			MencV->CoreLessLinearMotor = FALSE;
			break;
		}
		/*------------------------------------------------------------------------------------------*/
		/*	��-Trac�`������		<V335>																*/
		/*------------------------------------------------------------------------------------------*/
		switch(Model[2])
		{
		case 'T':
			switch(Model[3])
			{
			case 'M':
			case 'F':
				MencV->SigmaTracMotor = TRUE; /* ��-Trac ��, MAG				*/
				break;
			default:
				MencV->SigmaTracMotor = FALSE;
				break;
			}
			break;
		default:
			MencV->SigmaTracMotor = FALSE;
			break;
		}
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		�ᑬ���[�^�t���O�ݒ�																	*/
	/*----------------------------------------------------------------------------------------------*/
	if(MencV->AxisMotType == MOTTYPE_ROTARY)
	{
//		kx = MlibExpCal(MencP->exp_motw_spd.b.h);
//		kx = MlibScalKskxkx(kx, (100 * MencP->vrat_max.b.h), 1, NULL, -30);
		MencV->LowSpdMotor = (Model[3] == 'C') ? TRUE : FALSE;
	}
	else
	{
		MencV->LowSpdMotor = FALSE;
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		���]�ڑ��t���O�ݒ�																		*/
	/*----------------------------------------------------------------------------------------------*/
	if(Model[20] == 'R')
	{
		MencV->RevAsmMotor = TRUE;
	}
	else
	{
		MencV->RevAsmMotor = FALSE;
	}

	/*----------------------------------------------------------------------------------------------*/
	/*	���j�A���F���]�ڑ��t���O�����ݒ�															*/
	/*----------------------------------------------------------------------------------------------*/
	if(MencV->AxisMotType == MOTTYPE_LINEAR)
	{
		MencV->RevAsmMotor = FALSE;
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		��ߊE�����䋭���L���ݒ�																*/
	/*----------------------------------------------------------------------------------------------*/
	if(MencV->WeakenFieldEnable == TRUE)
	{ /* ��ߊE���L���t���OON */
		MencP->flg_wf |= 0x0001; /* �d��FB������ߊE������L��			*/
	}
}


/****************************************************************************************************/
/*																									*/
/*		�V���A���G���R�[�_ : �G���R�[�_�����[�^ID�擾�E�ݒ菈��										*/
/*																									*/
/****************************************************************************************************/
static BOOL	SencSetMotorAndEncoderID( MENCV *MencV, MENCPRM *MencP, OPFBK_HNDL *FbkOpt,			/* <S04D>:FbkOpt�ǉ� */
										 USHORT b_prm2, UCHAR *EncPrmBuf, BOOL SencFbOptUse )	/* <S04D>:SencFbOptUse�ǉ� */
{
	BOOL err;

	/*----------------------------------------------------------------------------------------------*/
	/*		���j�A�^�C�v�R�[�h�ݒ�																	*/
	/*----------------------------------------------------------------------------------------------*/
	MencV->TypeCode = EncPrmBuf[0x40];

	/*----------------------------------------------------------------------------------------------*/
	/*		�G���R�[�_�^�C�v�ݒ�																	*/
	/* <S04D> mod( EncInfo -> pActEncInfo )														*/
	/*----------------------------------------------------------------------------------------------*/
	SencSetIDINFO( 0x00, &EncPrmBuf[0x00], MencV->pActEncInfo );
	SencSetIDINFO( 0x08, &EncPrmBuf[0x08], MencV->pActEncInfo );
	SencSetIDINFO( 0x10, &EncPrmBuf[0x10], MencV->pActEncInfo );
	SencSetIDINFO( 0x18, &EncPrmBuf[0x18], MencV->pActEncInfo );

	SencFillNullIDINFO( MencV->pActEncInfo );
	err = SencSetEncoderType( MencV, MencP, b_prm2, SencFbOptUse );/* <S04D>:SencFbOptUse�ǉ� */

	/*----------------------------------------------------------------------------------------------*/
	/*		���[�^�^�C�v�ݒ�																		*/
	/* <S04D> mod( MotInfo -> pActMotInfo )														*/
	/*----------------------------------------------------------------------------------------------*/
	SencSetIDINFO( 0x20, &EncPrmBuf[0x20], MencV->pActMotInfo );
	SencSetIDINFO( 0x28, &EncPrmBuf[0x28], MencV->pActMotInfo );
	SencSetIDINFO( 0x30, &EncPrmBuf[0x30], MencV->pActMotInfo );
	SencSetIDINFO( 0x38, &EncPrmBuf[0x38], MencV->pActMotInfo );
	SencSetIDINFO( 0x98, &EncPrmBuf[0x98], MencV->pActMotInfo );

	SencSetMotorType( MencV, MencP);
	SencFillNullIDINFO( MencV->pActMotInfo );
	
	/*--------------------------------------------------------------------------------------------------*/
	/*		�t�B�[�h�o�b�N�I�v�V�������W���[�����ݒ�													*/
	/*--------------------------------------------------------------------------------------------------*/
	/*		����̃V���A���ϊ��^�t�B�[�h�o�b�N�I�v�V�������W���[����ID���̓G���R�[�_ID���̈��		*/
	/*		�����Ă��邽�߁A�G���R�[�_ID��񂩂�쐬����B										<S04D>	*/
	/*--------------------------------------------------------------------------------------------------*/
	if( SencFbOptUse == TRUE )
	{
		FbOptSetIDInfo(FbkOpt, MencV, SencFbOptUse );
	}
	

	return err;
}

/****************************************************************************************************/
/*																									*/
/*		�V���A���G���R�[�_ : ���j�A���[�^��Ή����菈��												*/
/*																									*/
/****************************************************************************************************//*<S1BE>*/
static SENC_ERROR SencCheckLinearMotor( MENCV *MencV, USHORT syssw1 )
{
	SENC_ERROR	err;
	err = SENCERR_SUCCESS;
	/*----------------------------------------------------------------------------------------------*/
	/*  ���j�A���[�^��Ή����菈��																	*/
	/*		PnE0B.1 =	0	: ��]�^�A���j�A�Ή�													*/
	/*				=	1	: ��]�^�Ή��A���j�A��Ή�(A.051����)									*/
	/*----------------------------------------------------------------------------------------------*/
	if(syssw1 & 0x0002)
	{
		if(MencV->AxisMotType == MOTTYPE_LINEAR)
		{/* A.051 : ���i���T�|�[�g�A���[��	*/
			err = SENCERR_NOTSUPPORT;
		}
	}
	return err;
}

/****************************************************************************************************/
/*																									*/
/*		�V���A���G���R�[�_ : �S�p�����[�^�����ݒ菈��												*/
/*																									*/
/****************************************************************************************************/
static SENC_ERROR	SencInitEncParameters( MENCV *MencV, MENCPRM *MencP, UCHAR  *EncPrmBuf)
{
	USHORT		addr;
	UCHAR		*PrmBuf;
	SENC_ERROR	err;
	LONG 		kx;

	err = SENCERR_SUCCESS;
	/*----------------------------------------------------------------------------------------------*/
	/* �G���R�[�_�p�����[�^�ݒ菈�� */
	/*----------------------------------------------------------------------------------------------*/
	/* Bank0����l���擾�����l��ݒ肷��B															*/
	for( addr = 0x0048; addr < 0x00A0; addr += 8 )
	{
		PrmBuf = &EncPrmBuf[addr];
		switch(addr)
		{
		case 0x48: /* �G���R�[�_�r�b�g���A�����_�ʒu�A�}���`�^�[�����~�b�g					*/
			MencV->DivInfo.IncreBit_dp = (UCHAR)PrmBuf[0]; /* �Ή��@��F21-24�r�b�g�C���N�� *//*<S161>*/
			MencP->bit_dp.b.l = ( PrmBuf[2] == (UCHAR)0xFF )? 0 : PrmBuf[2];
			MencP->bit_dp.b.h = PrmBuf[3];
			MencP->limmlt = ((USHORT) PrmBuf[7] << 8) | (USHORT) PrmBuf[6];

			/* �X�P�[���s�b�`																*/
			if(MencV->AxisMotType == MOTTYPE_LINEAR)
			{
				MencV->ScalePitch[0] = PrmBuf[4]; /* �X�P�[���s�b�`����				*/
				MencV->ScalePitch[1] = PrmBuf[5]; /* �X�P�[���s�b�`����				*/
				MencV->ScalePitch[2] = PrmBuf[6]; /* �X�P�[���s�b�`���				*/
				MencV->ScalePitchExp = PrmBuf[7]; /* �X�P�[���s�b�`�w��				*/
			}

			/* �G���R�[�_�ʒu�␳�p�T���v�����O�����ݒ� */
			MencV->EncLchNum = 0;
			if(MencV->AxisMotType == MOTTYPE_ROTARY)
			{
				if(MencP->bit_dp.b.l > 13)
				{
					if(MencP->typm_e.b.l == 0x11)
					{/* Sigma3 motor : SGMMJ */
						MencV->EncLchNum = 28; /* 16bit,17bit : 28/6M =4.67us(for SGMMJ)	*/
					}
					else if(MencP->typm_e.b.l < 0x60)
					{/* Sigma2, Sigma3 motor */
						MencV->EncLchNum = 16; /* 16bit,17bit : 16/6M =2.67us				*/
					}
					else
					{/* Sigma5 motor or later */
						;
					}
				}
			}

			break;
			/*--------------------------------------------------------------------------------------*/
		case 0x50: /* �ʑ��p�␳�l															*/
			if(MencV->AxisMotType == MOTTYPE_LINEAR)
			{
				MencP->phscmpM2 = ((USHORT) PrmBuf[1] << 8) | (USHORT) PrmBuf[0];
			}
			else
			{
				MencP->phscmpM2 = 0;
			}
			break;
			/*--------------------------------------------------------------------------------------*/
		case 0x58: /* �ʐM���x�ݒ�A�p�����[�^�g���̈�I���A�G���R�[�_�\�t�g�E�F�A�o�[�W����	*/
			MencP->exmotprm = (USHORT) ~PrmBuf[4];				/* 0�ŗL���̂��߃r�b�g���]		*/
			MencP->chng_baudrate = (USHORT) ~PrmBuf[5];			/* 0�ŗL���̂��߃r�b�g���]		*/
			MencP->encver = (USHORT) PrmBuf[7];
			break;
			/*--------------------------------------------------------------------------------------*/
		case 0x60: /* ��i�o�́A���[�^�|�[�����A��i�g���N									*/
			MencP->motw = ((USHORT) PrmBuf[1] << 8) | (USHORT) PrmBuf[0];
			MencP->oslv_pol.b.h = PrmBuf[2];
			MencP->rattrq = ((USHORT) PrmBuf[5] << 8) | (USHORT) PrmBuf[4];
			if(TRUE != SencExpConvert(PrmBuf[6], &(MencP->exp_trat_jmot.b.l)))
			{
				err = SENCERR_PRMDATA;
			}
			break;
			/*--------------------------------------------------------------------------------------*/
		case 0x68: /* �ő�g���N�A��i�d���A�ő�d���A�d�l�e�⏞							*/
			MencP->maxtrq = (((USHORT) PrmBuf[2] << 8) | (USHORT) PrmBuf[1]);
			MencP->irat = (((USHORT) PrmBuf[4] << 8) | (USHORT) PrmBuf[3]);
			MencP->imax = (((USHORT) PrmBuf[6] << 8) | (USHORT) PrmBuf[5]);
			MencP->emfcmp = (MencP->emfcmp & 0xFF00U) | (USHORT) PrmBuf[7];
			break;
			/*--------------------------------------------------------------------------------------*/
		case 0x70: /* �d�l�e�⏞,��i���x,�ő呬�x,�n�r���x��,�C�i�[�V��,������R			*/
			MencP->emfcmp = (MencP->emfcmp & 0x00FFU) | ((USHORT) PrmBuf[0] << 8);
			MencP->vrat_max.b.l = PrmBuf[1];
			MencP->vrat_max.b.h = PrmBuf[2];
			MencP->oslv_pol.b.l = PrmBuf[3];
			MencP->jmot = ((USHORT) PrmBuf[5] << 8) | (USHORT) PrmBuf[4];
			if(TRUE != SencExpConvert(PrmBuf[6], &(MencP->exp_trat_jmot.b.h)))
			{
				err = SENCERR_PRMDATA;
			}
			MencP->motr = (MencP->motr & 0xFF00U) | (USHORT) PrmBuf[7];
			break;
			/*--------------------------------------------------------------------------------------*/
		case 0x78: /* ������R�A�C���_�N�^���X�A�ׁ[�X�g���N�A���ԃg���N					*/
			MencP->motr = (MencP->motr & 0x00FFU) | ((USHORT) PrmBuf[0] << 8);
			MencP->motl = ((USHORT) PrmBuf[2] << 8) | (USHORT) PrmBuf[1];
			MencP->bastrq = (USHORT) PrmBuf[5];
			MencP->midtrq = ((USHORT) PrmBuf[7] << 8) | (USHORT) PrmBuf[6];
			break;
			/*--------------------------------------------------------------------------------------*/
		case 0x80: /* ���Ԏ��ԁA���Ԏ��ԂQ�A���ԃg���N�Q									*/
			MencP->midolt = (USHORT) PrmBuf[0];
			MencP->mtolt2 = (USHORT) PrmBuf[1];
			MencP->mtrq2 = ((USHORT) PrmBuf[3] << 8) | (USHORT) PrmBuf[2];
			MencP->flg_wf = (USHORT) PrmBuf[4];
			break;
			/*--------------------------------------------------------------------------------------*/
		case 0x88: /* �ʑ��⏞�P�A�ʑ��⏞�Q(2004.06.29)�A���[�^�d���A�|�[���s�b�`			*/
			MencP->phscmpM1.b.l = PrmBuf[2];
			MencP->poleoffset = ((USHORT) PrmBuf[3] >= 180) ? 0 : ((USHORT) PrmBuf[3] << 1);
			MencP->typm_e.b.h
					|= SencGetMotorVoltCode(((USHORT) PrmBuf[5] << 8) | (USHORT) PrmBuf[4]);
			MencP->ppitch = ((USHORT) PrmBuf[7] << 8) | (USHORT) PrmBuf[6];
			break;
			/*--------------------------------------------------------------------------------------*/
		case 0x90: /* �����C���_�N�^���X(Lq0,Lq1)�A�w���A�ʑ��⏞�J�n���x(2004.06.29)		*/
			MencP->motlq0 = ((USHORT) PrmBuf[1] << 8) | (USHORT) PrmBuf[0];
			MencP->motlq1 = ((USHORT) PrmBuf[3] << 8) | (USHORT) PrmBuf[2];

			if(TRUE != SencExpConvert(PrmBuf[4], &(MencP->exp_motw_spd.b.l)))
			{
				err = SENCERR_PRMDATA;
			}

			if(TRUE != SencExpConvert(PrmBuf[5], &(MencP->exp_motw_spd.b.h)))
			{
				err = SENCERR_PRMDATA;
			}

			MencP->phscmpM1.b.h = 0;
			if((ULONG) PrmBuf[6] < 50)
			{
				MencP->motresfrq = (USHORT) (PrmBuf[6] * 100);
			}
			else if((ULONG) PrmBuf[6] > 250)
			{
				MencP->motresfrq = 0;
			}
			else
			{
				MencP->motresfrq = (USHORT) (PrmBuf[6] * 10);
			}
			break;
		default:
			break;
		}
	}

///	if( MencP->exmotprm == TRUE )		/* Bank6�L���m�F<S16E> */
	if( (SENCBR_EXAREA1 & MencP->exmotprm) == TRUE )		/* Bank6�L���m�F<S16E> *//* <S1A6> */
	{
		/* Bank6����l���擾�����l��ݒ肷��B															*//* <S0D7> */
		for( addr = 0x00A0; addr < 0x00E0; addr += 8 )
		{
			PrmBuf = &EncPrmBuf[addr];
			switch(addr - 0x00A0)/* 0x600�A�h���X����擾�����l�́A0x00A0�ȍ~�Ɋi�[����Ă��邽��		*/
			{
			case 0x00:	/* �@�\�t���O�Q�APWM���g���A�ᑬ�ߕ��׃x�[�X�d���E���ԓd���P/�Q�E���Ԏ���		*/
				MencP->motfuncf.ul	=  (USHORT) PrmBuf[0];										/* PnF24	*//*���[�^�L�����A���g���؂�ւ��Ή�<S117>*/
				MencP->mot_pwmf	= ((USHORT) PrmBuf[2] << 8) | (USHORT) PrmBuf[1];			/* PnF25	*/
				MencP->mot_ibs	 = (USHORT) PrmBuf[3];										/* PnF26	*/
				MencP->mot_imd	= ((USHORT) PrmBuf[5] << 8) | (USHORT) PrmBuf[4];			/* PnF27	*/
				MencP->mot_imdt	=  (USHORT) PrmBuf[6];										/* PnF28	*/
				MencP->mot_imd2	=  (USHORT) PrmBuf[7];										/* PnF29	*/
				break;
				/*--------------------------------------------------------------------------------------*/
			case 0x08:	/* �ᑬ�ߕ��ג��Ԏ��ԂQ�A��]���x臒l�A���[�^OC���o�d���A�������X���[�h�I��		*/
						/* �d���w�ߐ������x��(�d���w�ߐ������x���Ad���d���w�߃��~�b�g�p�d���������x��)	*/
				MencP->mot_imdt2		= ((USHORT) PrmBuf[1] << 8) | (USHORT) PrmBuf[0];	/* PnF2A	*/
				MencP->mot_ichgspd		=  (USHORT) PrmBuf[2];								/* PnF2B	*/
				MencP->mot_oc			= ((USHORT) PrmBuf[4] << 8) | (USHORT) PrmBuf[3];	/* PnF2C	*/
				MencP->mot_TunLess		=  (USHORT) PrmBuf[5];								/* PnF2D	*/
				MencP->RefAdjust_ForWF.b.l	=  PrmBuf[6];									/* PnF2E.l	*/
				MencP->RefAdjust_ForWF.b.h	=  PrmBuf[7];									/* PnF2E.h	*/
				break;
				/*--------------------------------------------------------------------------------------*/
			case 0x10:	/* ��ߊE��L�␳�W���A�񐶎��ʑ��i�ݕ⏞(�J�n���x�A�ő��]���x)				*/
				MencP->LCompForWF = (USHORT) PrmBuf[0];										/* PnF2F	*/
				MencP->RegPhscmp.b.l = PrmBuf[1];											/* PnF30.l	*/
				MencP->RegPhscmp.b.h = PrmBuf[2];											/* PnF30.h	*/
				break;
				/*--------------------------------------------------------------------------------------*/
			case 0x18:	/* �\���̈�																		*/

				break;
				/*--------------------------------------------------------------------------------------*/
			case 0x20:	/* IPM����p�����[�^�̈�\��													*/
			case 0x28:
			case 0x30:
			case 0x38:

				break;
				/*--------------------------------------------------------------------------------------*/
			default:
				break;
			}
		}
	}
	else
	{/* bank6�������͏����l0xFFFF�Ƃ��� */
		MencP->motfuncf.ul	= 0xFFFF;													/* PnF24	*//*���[�^�L�����A���g���؂�ւ��Ή�<S117>*/
		MencP->mot_pwmf	= 0xFFFF;														/* PnF25	*/
		MencP->mot_ibs	 = 0xFFFF;														/* PnF26	*/
		MencP->mot_imd	= 0xFFFF;														/* PnF27	*/
		MencP->mot_imdt	= 0xFFFF;														/* PnF28	*/
		MencP->mot_imd2	= 0xFFFF;														/* PnF29	*/
		MencP->mot_imdt2		= 0xFFFF;												/* PnF2A	*/
		MencP->mot_ichgspd		= 0xFFFF;												/* PnF2B	*/
		MencP->mot_oc			= 0xFFFF;												/* PnF2C	*/
		MencP->mot_TunLess		= 0xFFFF;												/* PnF2D	*/
		MencP->RefAdjust_ForWF.b.l	= 0xFFFF;											/* PnF2E.l	*/
		MencP->RefAdjust_ForWF.b.h	= 0xFFFF;											/* PnF2E.h	*/
		MencP->LCompForWF = 0xFFFF;														/* PnF2F	*/
		MencP->RegPhscmp.b.l = 0xFFFF;													/* PnF30.l	*/
		MencP->RegPhscmp.b.h = 0xFFFF;													/* PnF30.h	*/
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		�ᑬ���[�^�t���O�ݒ�																	*/
	/*----------------------------------------------------------------------------------------------*/
	if(MencV->AxisMotType == MOTTYPE_ROTARY)
	{
		kx = MlibExpCal(MencP->exp_motw_spd.b.h);
		kx = MlibScalKskxkx(kx, (100 * MencP->vrat_max.b.h), 1, NULL, -30);
		MencV->LowSpdMotor |= ((kx <= 500) ? TRUE : FALSE);
	}

	return err;
}

/****************************************************************************************************/
/*																									*/
/*		�V���A���G���R�[�_ : �ʐM�ڑ�����															*/
/*																									*/
/****************************************************************************************************/
SENC_ERROR	SencInitMotorEncoder( MENCV *MencV, MENC_INIT_PRM *MencInitPrm,	ASIC_HW_IF_STR *AsicHwReg,
										OPFBK_HNDL *FbkOpt, USHORT EncBaudrate, BOOL SencFbOptUse )/* <S04D>:FbkOpt,SencFbOptUse�ǉ� */
{
//	UCHAR 		EncPrmBuf[160];	/* 0xA0	*//* <S0D7> */
	UCHAR 		EncPrmBuf[224];	/* 0xE0	*//* <S0D7> */
	SENC_ERROR	err;
	err = SENCERR_SUCCESS;

	/*----------------------------------------------------------------------------------------------*/
	/*		CONNECT																					*/
	/*----------------------------------------------------------------------------------------------*/
	err |= SencConnect( MencV, AsicHwReg, EncBaudrate, FbkOpt, SencFbOptUse );

	if( SENCERR_SUCCESS == err )	/* Execute in case success encoder connect */
	{
		/*------------------------------------------------------------------------------------------*/
		/*		Read Encoder parameters																*/
		/*------------------------------------------------------------------------------------------*/
		err |= SencReadParamAll( MencV, EncPrmBuf );
		if( SENCERR_SUCCESS != err )
		{	/* Try again... */
			err |= SencReadParamAll( MencV, EncPrmBuf );
		}
		if( SENCERR_SUCCESS == err )	/* Execute in case success read encoder parameter */
		{
			/*--------------------------------------------------------------------------------------*/
			/*		�G���R�[�_/���[�^ID�ݒ菈��														*/
			/*--------------------------------------------------------------------------------------*/
			if( FALSE != SencSetMotorAndEncoderID(MencV, MencInitPrm->MencP, FbkOpt,			/* <S04D>:FbkOpt�ǉ� */
												MencInitPrm->b_prm2, EncPrmBuf, SencFbOptUse) )	/* <S04D>:SencFbOptUse�ǉ� */
			{
				err |= SENCERR_PRMDATA;
			}
			/*--------------------------------------------------------------------------------------*/
			/*		���j�A���[�^��Ή����菈��														*/
			/*--------------------------------------------------------------------------------------*//* <S1BE> */
			if( FALSE != SencCheckLinearMotor(MencV, MencInitPrm->syssw1) )
			{
				err |= SENCERR_NOTSUPPORT;
			}
			/*--------------------------------------------------------------------------------------*/
			/*		���_���Z�b�g�R�}���h�̑��M														*/
			/*--------------------------------------------------------------------------------------*/
			/* <S04D> �������Ɂu�t�B�[�h�o�b�N�I�v�V�����ɂ��Z�~�N�v�Ɓu��Βl�ݺ��ށv��ǉ�		*/
			if( (( MencV->AxisMotType == MOTTYPE_LINEAR ) || ( SencFbOptUse == TRUE ))
																&&( MencV->AbsoEncoder == TRUE ) )
			{
				err |= SencSendRstOrgCmd( MencV );
			}
			/*--------------------------------------------------------------------------------------*/
			/*		�G���R�[�_�p�����[�^����������													*/
			/*--------------------------------------------------------------------------------------*/
			err |= SencInitEncParameters( MencV, MencInitPrm->MencP, EncPrmBuf );

			/*--------------------------------------------------------------------------------------*/
			/*		�G���R�[�_�x�[�X�p�����[�^�v�Z����												*/
			/*--------------------------------------------------------------------------------------*/
			err |= SencCalcBaseParameters( MencV, MencInitPrm );
		}
	}
	else/* <S04D> */
	{
		/*--------------------------------------------------------------------------------------*/
		/*		�G���R�[�_�x�[�X�p�����[�^�v�Z����												*/
		/*--------------------------------------------------------------------------------------*/
		err |= SencCalcBaseParameters( MencV, MencInitPrm );
	}

	/* 2013.08.29 Y.Oka P1���[�h�ȍ~��8MBps�؂�ւ���ɕύX */
	/*----------------------------------------------------------------------------------------------*/
	/* P1���[�h�ݒ�(��L�����̐���/���s�Ɍ��炸P1���[�h�i62.5us�ʐM�����j�ɕύX����->Scan������ς��Ȃ�����) */
	/*----------------------------------------------------------------------------------------------*/
//	SencChangeP1Mode( MencV );

	return err;
}



#if 0
/****************************************************************************************************/
/*																									*/
/*		�p���X�h�e������																			*/
/*																									*/
/*		�� ������5�Ƃ������ASV_OFF���̏����̂��ߏ������Ԃɂ͗]�T����BY.Oka						*/
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
static void	InitAsicPulseIF( ALMMAN_HANDLE *AlmMngr, UNI_PRM_STR *UniPrms, ASIC_HW_IF_STR *AsicHwReg, MENCV *MencV )
{
	PRMDATA		*prm_ptr;
	LONG		i;
	USHORT		uswk;
	BOOL		err;
	prm_ptr = UniPrms->Prm;

/*--------------------------------------------------------------------------------------------------*/
/*		�w�߃J�E���^�ݒ�																			*/
/*--------------------------------------------------------------------------------------------------*/
	i = prm_ptr->prstsw & 0x0F;							/* �w�߃p���X�`�� (Pn200)					*/
	if( i > 6 )											/* �p�����[�^�ݒ�͈̓`�F�b�N				*/
	{
		uswk = 0;
		/* �p�����[�^�ݒ�G���[ */
		ALMSetPramError( AlmMngr, &pndef_prstsw );
	}
	else
	{
		uswk = AsicPocsetTable[i];						/* �J�E���g���[�h,etc						*/
	}
	/*----------------------------------------------------------------------------------------------*/
	if( ((prm_ptr->prstsw >> 4) & 0x0F) < 2 )
	{
		uswk = ( uswk | 0x0800 );							/* CC�ɐ��ݒ�							*/
	}

/*--------------------------------------------------------------------------------------------------*/
/*		CA,CB �t�B���^�ݒ�																			*/
/*--------------------------------------------------------------------------------------------------*/
	switch( (prm_ptr->prstsw >> 12) & 0x0F )
	{
	case 0x00:											/* ���C���h���C�o�M���p�w�ߓ��̓t�B���^�P	*/
		/* (PnE0B.4-5)�ɂĐݒ� */
		 uswk = (USHORT)(uswk | ((prm_ptr->syssw1 >> 4) & 0x3) );
		 break;
	case 0x01:											/* �I�[�v���R���N�^�M���p�w�ߓ��̓t�B���^	*/
		/* (PnE0E.0-1)�ɂĐݒ� */
		 uswk = (USHORT)(uswk | ((prm_ptr->comfil)      & 0x3) );
		 break;
	case 0x02:											/* ���C���h���C�o�M���p�w�ߓ��̓t�B���^�Q	*/
		/* (PnE0E.8-9)�ɂĐݒ� */
		 uswk = (USHORT)(uswk | ((prm_ptr->comfil >> 8) & 0x3) );
		 break;
	default:
		/* (PnE0B.4-5)�ɂĐݒ� */
		 uswk = (USHORT)(uswk | ((prm_ptr->syssw1 >> 4) & 0x3) );
		 break;
	}
/*--------------------------------------------------------------------------------------------------*/
	AsicHwReg->AREG_POCSET	= uswk;						/* �w�߃J�E���^�ݒ�							*/
	uswk = ( uswk | 0x0100 );							/* CLM���b�`�C�l�[�u���ݒ�					*/
	AsicHwReg->AREG_POCSET	= uswk ;					/* �w�߃J�E���^�ݒ�							*/
	AsicHwReg->AREG_LODSET	= AREG_LODSET_SETV;			/* �J�E���^���[�h�ݒ�						*/

/*--------------------------------------------------------------------------------------------------*/
/*		�w�߃t�B���^�ݒ�																			*/
/*--------------------------------------------------------------------------------------------------*/
	switch( (prm_ptr->prstsw >> 12) & 0x0F )
	{
	case 0x00:											/* ���C���h���C�o�M���p�w�ߓ��̓t�B���^�P	*/
		/* (PnE0B.8-11)�ɂĐݒ� */
		 AsicHwReg->AREG_FCCSET	= (USHORT)((prm_ptr->syssw1 & 0x0F00) );
		 break;
	case 0x01:											/* �I�[�v���R���N�^�M���p�w�ߓ��̓t�B���^	*/
		/* (PnE0E.4-7)�ɂĐݒ� */
		 AsicHwReg->AREG_FCCSET	= (USHORT)((prm_ptr->comfil << 4) & 0x0F00);
		 break;
	case 0x02:											/* ���C���h���C�o�M���p�w�ߓ��̓t�B���^�Q	*/
		/* (PnE0E.12-15)�ɂĐݒ� */
		 AsicHwReg->AREG_FCCSET	= (USHORT)((prm_ptr->comfil >> 4) & 0x0F00);
		 break;
	default:
		/* (PnE0B.8-11)�ɂĐݒ� */
		 AsicHwReg->AREG_FCCSET	= (USHORT)(prm_ptr->syssw1 & 0x0F00);
		 break;
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�V���A���G���R�[�_�p���X�ϊ��ݒ�(Bit��,�ϊ�����) ��JL-056�����̐ݒ聗						*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->PlsTrn13bit == TRUE )					/* �p���X�ϊ�13bit							*/
	{
		AsicHwReg->AREG_PLSET	= 0x3100;					/* �ϊ�Bit��:13bit, �ϊ�����:64us			*/
	}
	else
	{
		AsicHwReg->AREG_PLSET	= 0x3400;					/* �ϊ�Bit��:16bit, �ϊ�����:64us			*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�V���A���G���R�[�_�p���X�ϊ��ݒ�(�ʒu,���_,etc)												*/
/*--------------------------------------------------------------------------------------------------*/
	// �p���X�ϊ��b���}�X�N(�O�̂���)
	AsicHwReg->AREG_WDT1SET	|=	BIT8;
	MencV->DivCMaskStatus = DIV_CMASK_ON;

	AsicHwReg->AREG_DIVSET	= 0x0102;					/* �p���X�ϊ������ݑ��ݒ�:HostCPU			*/
	AsicHwReg->AREG_POSET0	= 0xFFFF;					/* �p���X�ϊ��ʒu�ݒ�						*/
	AsicHwReg->AREG_POSET0H	= 0x0000;				/* �p���X�ϊ��ʒu�ݒ�bit16-23				*/
	AsicHwReg->AREG_POTEST	= 0xFFFF;					/* �p���X�ϊ��J�E���^�ݒ�					*/
	AsicHwReg->AREG_POSET1	= 0x0000;					/* �p���X�ϊ����_�␳�P						*/
	AsicHwReg->AREG_POSET2	= 0xFFFF;					/* �p���X�ϊ����_�␳�Q						*/

/*--------------------------------------------------------------------------------------------------*/
/*		�������̓t�B���^�ݒ�																		*/
/*--------------------------------------------------------------------------------------------------*/
	AsicHwReg->AREG_FBCSET	= 0x0005;					/* �������̓t�B���^�ݒ�						*/

/*--------------------------------------------------------------------------------------------------*/
/*		���[�^��ʏ����F�����p���X�v�Z���`�r�h�b�ݒ�												*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->AxisMotType == MOTTYPE_LINEAR )
	{ /* ���j�A�^ */
		err = LmxSencSetMencDividedPulse( AsicHwReg, MencV, UniPrms );
	}
	else
	{ /* ��]�^ */
		err = RmxSencSetMencDividedPulse( AsicHwReg, MencV, UniPrms );
	}

	if(err != FALSE)
	{
		ALMSetServoAlarm(AlmMngr, ALM_DIVPRM );		/* A.041 : �����p���X�o�͐ݒ�ُ�		*/
	}
}
#endif

//#if 0 /* MENCV�֘A�̍\���̂�ύX����K�v����H *//*<S02D>�폜*/
/****************************************************************************************************/
/*																									*/
/*		�����p���X�h�e������																		*/
/*																									*/
/****************************************************************************************************/
DIVSET_ERROR SencInitDivPulseIF( ASIC_HW_IF_STR* AsicHwReg, MENCV *MencV, UNI_PRM_STR *UniPrms )
{
	DIVSET_ERROR	err;
	LONG	BitNo;  
	LONG	pgrate;        /*<S079>*/

	BitNo  = UniPrms->Prm->MencP.bit_dp.b.l;								/* PnF03				*//*<S02D>*/

	if( MencV->AxisMotType != MOTTYPE_LINEAR )								/*<S079>*/
	{ /* ��]�^ */
		pgrate = (1 << BitNo) / (UniPrms->Prm->pgrat2l << 2);				/* Pn212(Pn213)			*/
		if( pgrate > 65536 )
		{ /* ������͈�:1/65536 �ȊO�̏ꍇ�́A�\�t�g�����Ƃ���  */
			MencV->DivInfo.DivOutASIC = FALSE;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�V���A���G���R�[�_�p���X�ϊ��ݒ�(Bit��,�ϊ�����) ��JL-056�����̐ݒ聗						*/
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/*	PLSET:�p���X�ϊ��ݒ�																			*/
/*--------------------------------------------------------------------------------------------------*/
/*	VULONG	AREG_PLSET;		 0x0168 �p���X�ϊ��ݒ�													*/
								/*------------------------------------------------------------------*/
								/*  bit.7-0  :�p���X�ϊ��q�X�e���V�X�ݒ�							*/
								/*------------------------------------------------------------------*/
								/*	bit.11-8 :�p���X�ϊ��r�b�g���ݒ�								*/
								/*             0000�F12bit											*/
								/*             0001�F13bit											*/
								/*                |													*/
								/*             1011�F23bit											*/
								/*       1100-1111�F24bit											*/
								/*------------------------------------------------------------------*/
								/*  bit.14-12 :�p���X�ϊ������I��									*/
								/*					  ����             ����N���b�N DDA�r�b�g��		*/
								/*				000: 16us              32MHz          9				*/
								/*				001: 32us              32MHz         10				*/
								/*				010: 64us              32MHz         11				*/
								/*				011: 64us              16MHz         10�i�����l�j	*/
								/*				100: 128us              8MHz         10				*/
								/*				101-111: 1.024ms   1MHz							*/
								/*------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->DivInfo.DivOutASIC != FALSE )
	{
        AsicHwReg->AREG_PLSET	= 0x00003000 + ((BitNo - 12) << 8); /* �ϊ�Bit��:BitNo-12, �ϊ�����:64us*//*<S079>*/
	}
	else
	{
		if( MencV->DivInfo.PlsTrn13bit == TRUE )			/* �p���X�ϊ�13bit							*/
		{
			AsicHwReg->AREG_PLSET	= 0x00003100;			/* �ϊ�Bit��:13bit, �ϊ�����:64us			*/
		}
		else
		{
			AsicHwReg->AREG_PLSET	= 0x00003400;			/* �ϊ�Bit��:16bit, �ϊ�����:64us			*/
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�V���A���G���R�[�_�p���X�ϊ��ݒ�(�ʒu,���_,etc)												*/
/*--------------------------------------------------------------------------------------------------*/
	// �p���X�ϊ��b���}�X�N(�O�̂���)
	AsicHwReg->AREG_WDT1SET	|=	BIT8;
	MencV->DivInfo.DivCMaskStatus = DIV_CMASK_ON;

/*--------------------------------------------------------------------------------------------------*/
/*	DIVSET:�����@�\�ݒ�																				*/
/*--------------------------------------------------------------------------------------------------*/
/*	VULONG	AREG_DIVSET;		 0x0164 �����@�\�ݒ�												*/
								/*------------------------------------------------------------------*/
								/*  bit.1-0 :�ް���ٽ�ϊ��ʒu�ް������ݑI��						*/
								/*            0�F�ر�PGIF����قO���� (�����l)						*/
								/*  		  1�F�ر�PGIF����قP����								*/
								/*  		  2�Fν�CPU����											*/
								/*  		  3�F�������Z������										*/
								/*------------------------------------------------------------------*/
								/*  bit.2 : �����ۯ��I��											*/
								/*            0�F26.7MHz (�����l)									*/
								/*  		  1�F32MHz												*/
								/*------------------------------------------------------------------*/
								/*  bit.4-3 :�p���X�ϊ����_�␳�l�ݒ茳�I��						*/
								/*            0�F�ر�PGIF����قO���� (�����l)						*/
								/*  		  1�F�ر�PGIF����قP����								*/
								/*  		  2�Fν�CPU����											*/
								/*  		  3�F�������Z������										*/
								/*------------------------------------------------------------------*/
								/*  bit.8 : DIVSEL0													*/
								/*            0�FPA,PB,PC				 							*/
								/*  		  1�F�ް���ٽ�ϊ��o�́i�����l�j							*/
								/*------------------------------------------------------------------*/
								/*  bit.11-10 :DCSEL												*/
								/*            0�F�ʏ�C�� (�����l)									*/
								/*  		  1�F����C��											*/
								/*  		  2�F����C��2											*/
								/*  		  3�F����C��2											*/
								/*------------------------------------------------------------------*/
								/*  bit.13 : DIVASEL												*/
								/*            0�FDA�o�� (�����l)									*/
								/*  		  1�F�ر��ް��ٰ�o��									*/
								/*------------------------------------------------------------------*/
								/*  bit.15 : DIVCBIT												*/
								/*            0�FDC�o�� (�����l)									*/
								/*  		  1�FDCʲ���ً����o��									*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->DivInfo.DivOutASIC != FALSE )
	{
		if(MencV->AbsoEncType == MENC_INCRE_TYPE)		/*<S079>*/
		{
			if( MencV->AbsoEncoder == FALSE ) /*<S0D3>*/
			{
				AsicHwReg->AREG_DIVSET	= 0x00000900;  		/* ��ٽ�ϊ������ݑ��ݒ�:�ر�PGIF����قO����	*/
			}
			else
			{
				AsicHwReg->AREG_DIVSET	= 0x00000910;  		/* ��ٽ�ϊ������ݑ��ݒ�:�ر�PGIF����قO����	*/
			}
		}
		else
		{ /* ��߿�ݺ��ނ̏ꍇ�́A�����ݸڏo�͊�����ɼر�PGIF����قO���� */
			AsicHwReg->AREG_DIVSET	= 0x00000912;  		/* ��ٽ�ϊ������ݑ��ݒ�:ν�CPU				*/
		}
	}
	else
	{
		AsicHwReg->AREG_DIVSET	= 0x00000112;  			/* ��ٽ�ϊ������ݑ��ݒ�:ν�CPU,				*/
	}
/*--------------------------------------------------------------------------------------------------*/
	AsicHwReg->AREG_POSET0	= 0xFFFFFFFF;				/* �p���X�ϊ��ʒu�ݒ�						*/
	AsicHwReg->AREG_POSET1	= 0x00000000;				/* �p���X�ϊ����_�␳�P						*/
	AsicHwReg->AREG_POSET2	= 0xFFFFFFFF;				/* �p���X�ϊ����_�␳�Q						*/
	AsicHwReg->AREG_POTEST	= 0xFFFFFFFF;				/* �p���X�ϊ��J�E���^�ݒ�					*/
/*--------------------------------------------------------------------------------------------------*/
/*		�������̓t�B���^�ݒ�																		*/
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/*	FBCSET:̨����ޯ���ٽ����																		*/
/*--------------------------------------------------------------------------------------------------*/
/*	VULONG	AREG_FBCSET;	     0x0040 �������̓t�B���^�ݒ�										*/
								/*------------------------------------------------------------------*/
								/*  bit.1-0 :PA,PB̨���I��											*/
								/*            0�F26.7MHz���� (�����l)								*/
								/*  		  1�F37.5ns												*/
								/*  		  2�F75ns												*/
								/*  		  3�F�f�ʂ�												*/
								/*------------------------------------------------------------------*/
								/*  bit.3-2 : PC̨���I��											*/
								/*            0�F26.7MHz���� (�����l)								*/
								/*  		  1�F37.5ns												*/
								/*  		  2�F75ns												*/
								/*  		  3�F�f�ʂ�												*/
								/*------------------------------------------------------------------*/
								/*  bit.5-15 : �ȗ�													*/
/*--------------------------------------------------------------------------------------------------*/
	AsicHwReg->AREG_FBCSET	= 0x00000005;				/* �������̓t�B���^�ݒ�						*/

/*--------------------------------------------------------------------------------------------------*/
/*		�t��]���[�h(Pn000.0 : ��]�����I��(�t��]���[�h))�ݒ�										*//* <S21B> */
/*--------------------------------------------------------------------------------------------------*/
	MencV->DivInfo.RvsDir = UniPrms->Bprm->RvsDir;
/*--------------------------------------------------------------------------------------------------*/
/*		���[�^��ʏ����F�����p���X�v�Z���`�r�h�b�ݒ�												*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->DivInfo.DivOutASIC != FALSE )
	{
		if( MencV->AxisMotType == MOTTYPE_LINEAR )
		{ /* ���j�A�^ */
			err = LmxSencSetMencDividedPulseHw( AsicHwReg, MencV, UniPrms ); /*<S079>*/
		}
		else
		{ /* ��]�^ */
			err = RmxSencSetMencDividedPulseHw( AsicHwReg, MencV, UniPrms );
		}
	}
	else
	{
		if( MencV->AxisMotType == MOTTYPE_LINEAR )
		{ /* ���j�A�^ */
			err = LmxSencSetMencDividedPulse( AsicHwReg, MencV, UniPrms );
		}
		else
		{ /* ��]�^ */
			err = RmxSencSetMencDividedPulse( AsicHwReg, MencV, UniPrms );
		}
	}

	return( err );
}

/****************************************************************************************************/
/*																									*/
/*		��]�^�F�V���A���G���R�[�_�����o�̓p���X�v�Z �� �`�r�h�b�ݒ�								*//*<S02D>*/
/*				(�����o�͂̈ʒu�ް���JL-086�Œ�����ٽ�ϊ�)											*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �����p���X�o�͂̕�����C�������[�h�̉��Z���s���B										*/
/*																									*/
/*																									*/
/*			BitNo				32-BitNo 															*/
/*		|<------------------>|<---------->|		PulseNo = 2^BitNo									*/
/*		+----------------+----------------+		 													*/
/*		|	PulseData		 |000000000000|		RcvPosX : [2^32/rev]								*/
/*		+----------------+----------------+															*/
/*		|<---------------->|<------------>| 	PulseOut = RcvPosX >> DivOutSft						*/
/*			PulseOut		   DivOutSft															*/
/*																									*/
/*							   <---- i ---|															*/
/*		|0000000001XXXXXXXXXXXX10000000000|		pgrate												*/
/*		|00000000000000000001XXXXXXXXXXXX1|		ratex = pgrate >> i									*/
/*						    <---- j ------|															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*				   DivRate        2^32																*/
/*		DivOut  = --------- * ------------- = (4 * pgrate)		[4���{pulse/rev]					*/
/*				    65536      2^DivOutSft															*/
/*																									*/
/*				   4 * pgrate * 65536																*/
/*		DivRate = -------------------- = pgrate >> (14 - DivOutSft) = pgrate >> RateSft				*/
/*				   2^(32 - DivOutSft)																*/
/*																									*/
/*		RateSft = 14 - DivOutSft,	DivOutSft = 14 - RateSft										*/
/*																									*/
/*		Condition : DivRate < 65536 , (DivRate % 4) = 0												*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static DIVSET_ERROR	RmxSencSetMencDividedPulseHw( ASIC_HW_IF_STR *AsicHwReg, MENCV *MencV, UNI_PRM_STR *UniPrms )
{
	LONG	i,j,x;
	LONG	BitNo;
	LONG	pulsno;
	LONG	pgrate;
	LONG	IncreSftR;   /*<S079>*/
	LONG	IncreSftX;   /*<S079>*/
	LONG	IncreLeng;   /*<S079>*/
	UCHAR 	*Model;  	 /*<S0D3>*/


	BitNo  = UniPrms->Prm->MencP.bit_dp.b.l;								/* PnF03				*/
	pgrate = UniPrms->Prm->pgrat2l;											/* Pn212(Pn213)			*/
/*--------------------------------------------------------------------------------------------------*/
/*		�����o�̓p���X����l�ݒ�																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->EncConnect )
	{
		pulsno = (MencV->PulseNo >> 2);
	}
	else
	{
		pulsno = pndef_pgrat2l.UpperLimit;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�����o�̓p���X���g��������x�̌v�Z															*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->DivInfo.DivOvrSpd = RmxSencCalMencDivOverSpeed( (pgrate -1), UniPrms->Bprm->OvrSpd );
	if( MencV->DivInfo.DivOvrSpd == 0 )
	{
		return ( DIVSET_PRMERR_R );                        /* �߂�l�F�p�����[�^�ݒ�ُ�(��]):A.040*/
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�p���X���`�F�b�N																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( MlibCheckPowerOf2( pulsno ) < 0 )					/* pulsno : �Q�̗ݏ�`�F�b�N			*/
	{
		return ( DIVSET_DIVERR );							/* �߂�l�F�����p���X�ݒ�ُ�:A.041		*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�o�f������ݒ�͈̓`�F�b�N																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( (pgrate > pulsno) || (pgrate < 16) )				/* pgrate : �͈̓`�F�b�N				*/
	{
		return ( DIVSET_DIVERR );							/* �߂�l�F�����p���X�ݒ�ُ�:A.041		*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�o�f������ݒ�����̃`�F�b�N																*/
/*--------------------------------------------------------------------------------------------------*/
	i = MlibSrhbiton( pgrate, SRH_FROM_LSB );				/*						i<---- Search	*/
	x = ((ULONG)pgrate >> i);								/* pgrate : xxxxxxxxxxxx100000			*/
	if( (i < 0) || (x > 16383) )							/* �ݒ�͈� : 1 <= x <= 16383			*/
	{														/*										*/
		return ( DIVSET_DIVERR );							/*										*/
	}														/*										*/
/*--------------------------------------------------------------------------------------------------*/
/*		�����p���X�o�͊����̉��Z : 11bit (for �����������[�^)										*/
/*--------------------------------------------------------------------------------------------------*/
	if( BitNo == 11 )
	{
		MencV->DivInfo.DivMode = FALSE;						/* Mode : �s�g�p						*/
		MencV->DivInfo.DivPass = FALSE;						/* Pass : �������[�h					*/
		if( pgrate < pulsno )								/* �������[�h : pgrate < pulsno			*/
		{
			MencV->DivInfo.DivRate = (USHORT)(pgrate << 5);	/* Rate = pgrate << (16-(BitNo-2)-2)	*/
		}
		else
		{
			MencV->DivInfo.DivRate = (USHORT)(pulsno << 5);	/* Rate = pulsno << (16-(BitNo-2)-2)	*/
		}
		MencV->DivInfo.DivOutSft = 19;						/* OutSft = 32 - BitNo - 2				*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�����p���X�o�͊����̉��Z : 16bit���� (for 13bit Encoder)									*/
/*--------------------------------------------------------------------------------------------------*/
	else if( BitNo < 16 )
	{
		if( pgrate < pulsno )								/* �������[�h : pgrate < pulsno			*/
		{
			MencV->DivInfo.DivMode = FALSE;					/* Mode : �s�g�p						*/
			MencV->DivInfo.DivPass = FALSE;					/* Pass : �������[�h					*/
			j = 18 - BitNo;									/* j = -(14 - (32-BitNo)) = 18 - BitNo	*/
			MencV->DivInfo.DivRate = (USHORT)(pgrate << j);	/* Rate = pgrate << (16-(BitNo-2))		*/
			MencV->DivInfo.DivOutSft = (UCHAR)(32 - BitNo);	/* OutSft = 32 - BitNo					*/
		}
	/*----------------------------------------------------------------------------------------------*/
		else												/* �X���[���[�h : pgrate == pulsno		*/
		{
			MencV->DivInfo.DivMode = FALSE;					/* Mode : �s�g�p						*/
			MencV->DivInfo.DivPass = TRUE;					/* Pass : �X���[���[�h					*/
			MencV->DivInfo.DivRate = 0;						/* Rate = 65536 = pgrate << (18-BitNo)	*/
			MencV->DivInfo.DivOutSft = (UCHAR)(32 - BitNo);	/* OutSft = 32 - BitNo					*/
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�����p���X�o�͊����̉��Z : 16bit�ȏ�														*/
/*--------------------------------------------------------------------------------------------------*/
	else if( x != 1 )										/* �������[�h�P : pgrate != 2^i			*/
	{														/* Search ------------------>j			*/
		j = MlibSrhbiton( x, SRH_FROM_MSB );				/*    x : 000000...00000000001xxxx1		*/
		if( (i + j) < 13 ){ j = 13 - i;}					/* Limit j by (OutSft <= 16)			*/
		MencV->DivInfo.DivMode = FALSE;						/* Mode : �s�g�p						*/
		MencV->DivInfo.DivPass = FALSE;						/* Pass : �������[�h					*/
#if 0 /*<S0C8>*/
		MencV->DivInfo.DivRate = (USHORT)(x << (15-j));		/* Rate = (pgrate>>i)<<(15-j)			*/
#else/*<S0C8>*/
		MencV->DivInfo.DivRate = (USHORT)((x << (15-j)) >> (BitNo - 16));
		j = BitNo - 16;										/* j = BitNo -16						*/
#endif/*<S0C8>*/
		MencV->DivInfo.DivOutSft = (UCHAR)(29 - i - j);		/* OutSft = 14 - i + 15 - j				*/
	}
	/*----------------------------------------------------------------------------------------------*/
	else if( i < 14 )										/* �������[�h�Q : pgrate == 2^i			*/
	{
		MencV->DivInfo.DivMode = FALSE;						/* Mode : �s�g�p						*/
		MencV->DivInfo.DivPass = FALSE;						/* Pass : �������[�h					*/
#if 0 /*<S079>*/
		MencV->DivInfo.DivRate = (USHORT)(pgrate << 2);		/* Rate = pgrate << 2 					*/
#else/*<S079>*/
		MencV->DivInfo.DivOutSft = 16;						/* OutSft = 16							*/
		j = BitNo - 16;										/* j = BitNo -16						*/
		MencV->DivInfo.DivRate = (USHORT)((pgrate << 2)>>j);/* Rate = pgrate << (16-(BitNo-2))		*/
		if( MencV->DivInfo.DivRate == 0 )
		{
			MencV->DivInfo.DivRate = 1;
		}
#endif/*<S079>*/
	}
	/*----------------------------------------------------------------------------------------------*/
	else													/* �X���[���[�h : pgrate == 2^i			*/
	{
		j = BitNo - 16;										/* j = BitNo -16						*//*<S0C8>*/
		MencV->DivInfo.DivMode = FALSE;						/* Mode : �s�g�p						*/
		MencV->DivInfo.DivPass = TRUE;						/* Pass : �X���[���[�h					*/
		MencV->DivInfo.DivRate = 0;							/* Rate = 65536 = ((pgrate>>i)<<16)		*/
		MencV->DivInfo.DivOutSft = (UCHAR)(30 - i);			/* OutSft = 14 - i + 16 = 30 - i		*/
	}
#if 0/*<S079>*/
/*--------------------------------------------------------------------------------------------------*/
/*		�����b���o�̓}�X�N����Flag�̐ݒ�															*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->DivInfo.DivPcmaskCtrl = (MencV->DivInfo.DivOutSft < 16)? TRUE : FALSE;
/*--------------------------------------------------------------------------------------------------*/
/*		�����b���o�̓}�X�N�͈͂̐ݒ�																*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->DivInfo.DivPcmaskCtrl )
	{
		MencV->DivInfo.DivOutCmskChk = (ULONG)0x80000000 >> (BitNo - 16);
	}
#endif/*<S079>*/
/*--------------------------------------------------------------------------------------------------*/
/*		�`�r�h�b�����@�\�ݒ�																		*/
/*--------------------------------------------------------------------------------------------------*/
/*	DINCSET�i0x0048�j:�����@�\																		*/
/*--------------------------------------------------------------------------------------------------*/
/*	VULONG	AREG_DINCSET;	     0x0048 �����@�\�ݒ�												*/
								/*------------------------------------------------------------------*/
								/*  bit.0  : DA,DB�X���[���[�h�ݒ�									*/
								/*            0�F�����o�́i�����l�j									*/
								/*  		  1�FPA PB�X���[�o��									*/
								/*------------------------------------------------------------------*/
								/*  bit.1  : DC�X���[���[�h�ݒ�										*/
								/*            0�FPC�X���[�i�����l�j									*/
								/*  		  1�FPA��PB��PC											*/
								/*------------------------------------------------------------------*/
								/*  bit.2  : DB�t�]���[�h�ݒ�										*/
								/*            0�FDB�X���[�o�́i�����l�j								*/
								/*  		  1�FDB���]�o��											*/
								/*------------------------------------------------------------------*/
								/*  bit.3  : �������[�h�I��											*/
								/*            0�F16bitDDA���[�h(�����l)������͈�:1/65536,65535/65536*/
								/*  		  1�F65005												*/
								/*------------------------------------------------------------------*/
								/*  bit.4  : �V�C���N���f�R�[�_C���ʉ� 1:�G�b�W�ʉ�					*/
								/*            0�F�V�C���N���f�R�[�_C���ʉ߁i�����l�j				*/
								/*  		  1�F�G�b�W�ʉ�											*/
								/*------------------------------------------------------------------*/
								/*  bit.5  : CLSET�r�b�g											*/
								/*            0�F���_�ē����s�i�����l�j							*/
								/*  		  1�F���_�ē�����										*/
								/*------------------------------------------------------------------*/
								/*  bit.10  : �����펞�N���A�ݒ�									*/
								/*            0�F�펞�N���A�f�B�Z�[�u���i�����l�j					*/
								/*  		  1�F�펞�N���A�C�l�[�u��								*/
								/*------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
	x = (MencV->DivInfo.DivMode << 4) | MencV->DivInfo.DivPass | 0x0400;
	AsicHwReg->AREG_DINCSET	= (USHORT)x;							/* �����@�\�ݒ�					*/
	AsicHwReg->AREG_DIVCSEV	= (USHORT)(MencV->DivInfo.DivRate);		/* ������ݒ�					*/

/*--------------------------------------------------------------------------------------------------*/
/*		�p���X�ϊ����_�␳�ݒ�																		*/
/*--------------------------------------------------------------------------------------------------*/
/*	VULONG	DCTRIM;	    0x0190 �����o�͌��_�␳�ݒ�(Bit0-23)										*/
/*--------------------------------------------------------------------------------------------------*/
								/*  bit.1-0  : �h0 �h												*/
								/*------------------------------------------------------------------*/
								/*  bit.23-2 : DVCP23-2												*/
								/*  		  �����o�͌��_�␳�ݒ�r�b�g23-2						*/
/*--------------------------------------------------------------------------------------------------*/
	if( BitNo == 13 )
	{
		AsicHwReg->AREG_DCTRIM	= (ULONG)((MencV->DivInfo.DivRate>>3)-1);
	}
	else
	{
		AsicHwReg->AREG_DCTRIM	= (ULONG)( (MencV->DivInfo.DivRate<<j) - 1 );
	}

/*--------------------------------------------------------------------------------------------------*/
/*  DINCSET bit.2 DB�t�]���[�h	(0�FDB�X���[�o��;    �P�FDB���]�o��)								*/
/*--------------------------------------------------------------------------------------------------*/
	if( UniPrms->Bprm->RvsDir )
	{
		AsicHwReg->AREG_DINCSET |= BIT2;					/* DB�t�]���[�h�ݒ�						*/
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�p���X�ϊ����_�␳�ݒ�																		*/
/*--------------------------------------------------------------------------------------------------*/
/*	VULONG	AREG_ZDR;	     0x0150 �p���X�ϊ����_�␳�ݒ�											*/
/*--------------------------------------------------------------------------------------------------*/
								/*  bit.3-0  : �C���N���␳�f�[�^ �E�V�t�g��						*/
								/*     ZDRSFT  �V���A����M�f�[�^D10-D11��16bit����LSB������		*/
								/*------------------------------------------------------------------*/
								/*  bit.7-4  : �C���N���␳�f�[�^ ���V�t�g��						*/
								/*     ZDLSFT �C���N���␳�f�[�^�̉��ʃf�[�^�ɐݒ�l����0�𖄂߂�j*/
								/*------------------------------------------------------------------*/
								/*  bit.11-8 : �C���N���␳�f�[�^��									*/
								/*     ZDLNG   �f�[�^�����i�r�b�g��-1�j�Őݒ�						*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->AbsoEncoder == FALSE )/*<S0D3>*/
	{
		if( BitNo > 20 )				/*<S079>*/
		{
			IncreSftX = 7;
//			IncreLeng = 8;
		 	IncreLeng = 95 - MencV->DivInfo.IncreBit_dp; /*<S161>*/
		}
		else if( BitNo == 13 )
		{
			IncreSftX = 0;
			IncreLeng = 10;
		}
		else if( BitNo == 17)   /*<S0D3>*/
		{
			IncreSftX = 0;
			Model = &(MencV->EncInfo.Model[0]);
			if( Model[3] == 'J' )
			{/* UTSJ��-B17 �̏ꍇ */
				IncreLeng = 9;
			}
			else if((Model[9] == 'C') && (Model[10] == 'C'))
			{/* UTSIH-B17CC�i���U�j*/
				IncreLeng = 10;
			}
			else
			{
				IncreLeng = 8;
			}
		}
		else
		{
			IncreSftX = 0;
			IncreLeng = 8;
		}
		if( MencV->Sigma3Encoder )
		{
			IncreSftX =  IncreSftX + 4;
		}
		IncreSftR = BitNo-(IncreLeng+1); /*<S0D3>*/
	
		AsicHwReg->AREG_ZDR   =	  ( IncreSftX << 0 )			/* �C���N���␳�f�[�^ �E�V�t�g��		*/
								+ ( IncreSftR << 4 )			/* �C���N���␳�f�[�^ ���V�t�g��		*/
								+ ( IncreLeng << 8 );			/* �C���N���␳�f�[�^��				*/
	/*--------------------------------------------------------------------------------------------------*/
	/*	VULONG	AREG_ZENBR;	     0x0154 �p���X�ϊ����_�␳�ݒ�											*/
	/*--------------------------------------------------------------------------------------------------*/
									/*------------------------------------------------------------------*/
									/*  bit.5-0  : ���_�ʉ߃r�b�g�ʒu�ݒ�								*/
									/*   ��M�f�[�^D7-D11��40bit���̌��_�ʉ߂������r�b�g�ʒu������j */
									/*------------------------------------------------------------------*/
									/*  bit.6  : �C���N���␳�f�[�^�펞�X�V���[�h�ݒ�					*/
									/*            0�F�ŏ��̌��_�ʉߎ��̂ݐݒ�							*/
									/*  		  1�F���_�ʉߎ��ɖ���X�V								*/
									/*------------------------------------------------------------------*/
									/*  bit.7  : �C���N���␳�f�[�^�Z�b�g�����r�b�g�N���A				*/
									/*            0�F�Z�b�g����											*/
									/*  		  1�F�Z�b�g����											*/
									/*------------------------------------------------------------------*/
									/*  bit.8  : �C���N���␳�f�[�^�Z�b�g�C�l�[�u��1					*/
									/*      �i���_���ʉ߃t���O=0�ŃC���N���␳�f�[�^���Z�b�g�j          */
									/*            0�F����												*/
									/*  		  1�F�L��												*/
									/*------------------------------------------------------------------*/
									/*  bit.9  : �C���N���␳�f�[�^�Z�b�g�C�l�[�u��2					*/
									/*      �i�C���N���␳�ް��̑O��l����̕ω��ŃC���N���␳�ް����)*/
									/*            0�F����												*/
									/*  		  1�F�L��												*/
									/*------------------------------------------------------------------*/
									/*  bit.10  : �C���N���␳�f�[�^�Z�b�g�C�l�[�u��3					*/
									/*      �i���_�ʉ߃r�b�gZ=1�ŃC���N���␳�f�[�^���Z�b�g)			*/
									/*            0�F����												*/
									/*  		  1�F�L��												*/
	/*--------------------------------------------------------------------------------------------------*/
	//	AsicHwReg->AREG_ZENBR = 0x0400;
		AsicHwReg->AREG_ZENBR = 0x0200;/*<S0D3>*/
		AsicHwReg->AREG_WDT1SET &= ~BIT8;				/* �p���X�ϊ�C���}�X�NOFF�ݒ�			*/
	}
/*--------------------------------------------------------------------------------------------------*/
	return ( DIVSET_SUCCESS );
}


/****************************************************************************************************/
/*																									*/
/*		��]�^�F�V���A���G���R�[�_�����o�̓p���X�v�Z �� �`�r�h�b�ݒ�								*//*<S02D>*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �����p���X�o�͂̕�����C�������[�h�̉��Z���s���B										*/
/*																									*/
/*																									*/
/*			BitNo				32-BitNo 															*/
/*		|<------------------>|<---------->|		PulseNo = 2^BitNo									*/
/*		+----------------+----------------+		 													*/
/*		|	PulseData		 |000000000000|		RcvPosX : [2^32/rev]								*/
/*		+----------------+----------------+															*/
/*		|<---------------->|<------------>| 	PulseOut = RcvPosX >> DivOutSft						*/
/*			PulseOut		   DivOutSft															*/
/*																									*/
/*							   <---- i ---|															*/
/*		|0000000001XXXXXXXXXXXX10000000000|		pgrate												*/
/*		|00000000000000000001XXXXXXXXXXXX1|		ratex = pgrate >> i									*/
/*						    <---- j ------|															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*				   DivRate        2^32																*/
/*		DivOut  = --------- * ------------- = (4 * pgrate)		[4���{pulse/rev]					*/
/*				    65536      2^DivOutSft															*/
/*																									*/
/*				   4 * pgrate * 65536																*/
/*		DivRate = -------------------- = pgrate >> (14 - DivOutSft) = pgrate >> RateSft				*/
/*				   2^(32 - DivOutSft)																*/
/*																									*/
/*		RateSft = 14 - DivOutSft,	DivOutSft = 14 - RateSft										*/
/*																									*/
/*		Condition : DivRate < 65536 , (DivRate % 4) = 0												*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static DIVSET_ERROR	RmxSencSetMencDividedPulse( ASIC_HW_IF_STR *AsicHwReg, MENCV *MencV, UNI_PRM_STR *UniPrms )
{
	LONG	i,j,x;
	LONG	BitNo;
	LONG	pulsno;
	LONG	pgrate;

	BitNo  = UniPrms->Prm->MencP.bit_dp.b.l;								/* PnF03				*/
	pgrate = UniPrms->Prm->pgrat2l;											/* Pn212(Pn213)			*/
/*--------------------------------------------------------------------------------------------------*/
/*		�����o�̓p���X����l�ݒ�																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->EncConnect )
	{
		pulsno = (MencV->PulseNo >> 2);
	}
	else
	{
		pulsno = pndef_pgrat2l.UpperLimit;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�����o�̓p���X���g��������x�̌v�Z															*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->DivInfo.DivOvrSpd = RmxSencCalMencDivOverSpeed( (pgrate -1), UniPrms->Bprm->OvrSpd );
	if( MencV->DivInfo.DivOvrSpd == 0 )
	{
		return ( DIVSET_PRMERR_R );                        /* �߂�l�F�p�����[�^�ݒ�ُ�(��]):A.040*/
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�p���X���`�F�b�N																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( MlibCheckPowerOf2( pulsno ) < 0 )					/* pulsno : �Q�̗ݏ�`�F�b�N			*/
	{
		return ( DIVSET_DIVERR );							/* �߂�l�F�����p���X�ݒ�ُ�:A.041		*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�o�f������ݒ�͈̓`�F�b�N																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( (pgrate > pulsno) || (pgrate < 16) )				/* pgrate : �͈̓`�F�b�N				*/
	{
		return ( DIVSET_DIVERR );							/* �߂�l�F�����p���X�ݒ�ُ�:A.041		*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�o�f������ݒ�����̃`�F�b�N																*/
/*--------------------------------------------------------------------------------------------------*/
	i = MlibSrhbiton( pgrate, SRH_FROM_LSB );				/*						i<---- Search	*/
	x = ((ULONG)pgrate >> i);								/* pgrate : xxxxxxxxxxxx100000			*/
	if( (i < 0) || (x > 16383) )							/* �ݒ�͈� : 1 <= x <= 16383			*/
	{														/*										*/
		return ( DIVSET_DIVERR );							/*										*/
	}														/*										*/
/*--------------------------------------------------------------------------------------------------*/
/*		�����p���X�o�͊����̉��Z : 11bit (for �����������[�^)										*/
/*--------------------------------------------------------------------------------------------------*/
	if( BitNo == 11 )
	{
		MencV->DivInfo.DivMode = FALSE;						/* Mode : �s�g�p						*/
		MencV->DivInfo.DivPass = FALSE;						/* Pass : �������[�h					*/
		if( pgrate < pulsno )								/* �������[�h : pgrate < pulsno			*/
		{
			MencV->DivInfo.DivRate = (USHORT)(pgrate << 5);	/* Rate = pgrate << (16-(BitNo-2)-2)	*/
		}
		else
		{
			MencV->DivInfo.DivRate = (USHORT)(pulsno << 5);	/* Rate = pulsno << (16-(BitNo-2)-2)	*/
		}
		MencV->DivInfo.DivOutSft = 19;						/* OutSft = 32 - BitNo - 2				*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�����p���X�o�͊����̉��Z : 16bit���� (for 13bit Encoder)									*/
/*--------------------------------------------------------------------------------------------------*/
	else if( BitNo < 16 )
	{
		if( pgrate < pulsno )								/* �������[�h : pgrate < pulsno			*/
		{
			MencV->DivInfo.DivMode = FALSE;					/* Mode : �s�g�p						*/
			MencV->DivInfo.DivPass = FALSE;					/* Pass : �������[�h					*/
			j = 18 - BitNo;									/* j = -(14 - (32-BitNo)) = 18 - BitNo	*/
			MencV->DivInfo.DivRate = (USHORT)(pgrate << j);	/* Rate = pgrate << (16-(BitNo-2))		*/
			MencV->DivInfo.DivOutSft = (UCHAR)(32 - BitNo);	/* OutSft = 32 - BitNo					*/
		}
	/*----------------------------------------------------------------------------------------------*/
		else												/* �X���[���[�h : pgrate == pulsno		*/
		{
			MencV->DivInfo.DivMode = FALSE;					/* Mode : �s�g�p						*/
			MencV->DivInfo.DivPass = TRUE;					/* Pass : �X���[���[�h					*/
			MencV->DivInfo.DivRate = 0;						/* Rate = 65536 = pgrate << (18-BitNo)	*/
			MencV->DivInfo.DivOutSft = (UCHAR)(32 - BitNo);	/* OutSft = 32 - BitNo					*/
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�����p���X�o�͊����̉��Z : 16bit�ȏ�														*/
/*--------------------------------------------------------------------------------------------------*/
	else if( x != 1 )										/* �������[�h�P : pgrate != 2^i			*/
	{														/* Search ------------------>j			*/
		j = MlibSrhbiton( x, SRH_FROM_MSB );				/*    x : 000000...00000000001xxxx1		*/
		if( (i + j) < 13 ){ j = 13 - i;}					/* Limit j by (OutSft <= 16)			*/
		MencV->DivInfo.DivMode = FALSE;						/* Mode : �s�g�p						*/
		MencV->DivInfo.DivPass = FALSE;						/* Pass : �������[�h					*/
		MencV->DivInfo.DivRate = (USHORT)(x << (15-j));		/* Rate = (pgrate>>i)<<(15-j)			*/
		MencV->DivInfo.DivOutSft = (UCHAR)(29 - i - j);		/* OutSft = 14 - i + 15 - j				*/
	}
	/*----------------------------------------------------------------------------------------------*/
	else if( i < 14 )										/* �������[�h�Q : pgrate == 2^i			*/
	{
		MencV->DivInfo.DivMode = FALSE;						/* Mode : �s�g�p						*/
		MencV->DivInfo.DivPass = FALSE;						/* Pass : �������[�h					*/
		MencV->DivInfo.DivRate = (USHORT)(pgrate << 2);		/* Rate = pgrate << 2 					*/
		MencV->DivInfo.DivOutSft = 16;						/* OutSft = 16							*/
	}
	/*----------------------------------------------------------------------------------------------*/
	else													/* �X���[���[�h : pgrate == 2^i			*/
	{
		MencV->DivInfo.DivMode = FALSE;						/* Mode : �s�g�p						*/
		MencV->DivInfo.DivPass = TRUE;						/* Pass : �X���[���[�h					*/
		MencV->DivInfo.DivRate = 0;							/* Rate = 65536 = ((pgrate>>i)<<16)		*/
		MencV->DivInfo.DivOutSft = (UCHAR)(30 - i);			/* OutSft = 14 - i + 16 = 30 - i		*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�����b���o�̓}�X�N����Flag�̐ݒ�															*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->DivInfo.DivPcmaskCtrl = (MencV->DivInfo.DivOutSft < 16)? TRUE : FALSE;
/*--------------------------------------------------------------------------------------------------*/
/*		�����b���o�̓}�X�N�͈͂̐ݒ�																*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->DivInfo.DivPcmaskCtrl )
	{
		MencV->DivInfo.DivOutCmskChk = (ULONG)0x80000000 >> (BitNo - 16);
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�`�r�h�b�����@�\�ݒ�																		*/
/*--------------------------------------------------------------------------------------------------*/
	x = (MencV->DivInfo.DivMode << 4) | MencV->DivInfo.DivPass | 0x0400;
	AsicHwReg->AREG_DINCSET	= (USHORT)x;						/* �����@�\�ݒ�						*/
	AsicHwReg->AREG_DIVCSEV	= (USHORT)(MencV->DivInfo.DivRate); /* ������ݒ�						*/
	/*----------------------------------------------------------------------------------------------*/
	/* �����o�͌��_�␳ */
	if( BitNo <= 13 )
	{
		AsicHwReg->AREG_DCTRIM	= (ULONG)((MencV->DivInfo.DivRate>>3)-1);
	}
	else
	{
		AsicHwReg->AREG_DCTRIM	= (ULONG)( MencV->DivInfo.DivRate - 1 );
	}
	/*----------------------------------------------------------------------------------------------*/
	if( UniPrms->Bprm->RvsDir )
	{
		AsicHwReg->AREG_DINCSET |= BIT2;					/* DB�t�]���[�h�ݒ�						*/
	}
	return ( DIVSET_SUCCESS );
}

/****************************************************************************************************/
/*																									*/
/*		��]�^�F�V���A���G���R�[�_�����o�̓p���X�ݒ�l�̒���										*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �����o�̓p���X�ݒ�ɂ����āA�ݒ�l���ݒ�\�Ȓl�ƂȂ�悤�ɒ�������B					*/
/*																									*/
/****************************************************************************************************/
//<S149>#define	BITNOCHK	14							/* 2^14 = 16384									*/
//<S149>#define	BITNOCHK		18							/* 2^18 = 262144								*//*<S02D>*/
/*--------------------------------------------------------------------------------------------------*/
#if 0			/* <S149> */
PRM_RSLT	RmxSencAdjustMencDivPulse( PRMDATA *Prm, MENCV *MencV, PRM_ACCCMD Cmd, LONG Data )
{
	ULONG	MaskX;
	ULONG	DataX;
	ULONG	BitNoX;
	ULONG	PulseNoX;
	ULONG	DataMinX = pndef_pgrat2l.LowerLimit;

/*--------------------------------------------------------------------------------------------------*/
/*		�����o�̓p���X����l�ݒ�																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->EncConnect )
	{
		PulseNoX = (MencV->PulseNo >> 2);
	}
	else
	{
		PulseNoX = pndef_pgrat2l.UpperLimit;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�p�����[�^�v�Z����(�p�����[�^�`�F�b�N)														*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->AxisMotType == MOTTYPE_LINEAR )
	{
		return( PRM_RSLT_SUCCESS );
	}
	else
	{
		DataX = Prm->pgrat2l;
		BitNoX = MlibSrhbiton( DataX, SRH_FROM_MSB );
		if( DataX > PulseNoX  ){ return( PRM_RSLT_LIMIT_ERR );}
		if( DataX < DataMinX  ){ return( PRM_RSLT_LIMIT_ERR );}
		if( BitNoX < BITNOCHK ){ return( PRM_RSLT_SUCCESS  );}
		MaskX = ( 0xFFFFFFFF << (BitNoX + 1 - BITNOCHK) );
		if( (DataX & MaskX) == DataX ){ return( PRM_RSLT_SUCCESS );}
		return( PRM_RSLT_CALC_ERR );
	}
}
#endif
/****************************************************************************************************/
/*																									*/
/*		��]�^�F�V���A���G���R�[�_�����o�̓p���X�ݒ�l�̒��� (���݁A�s�g�p) <S149>					*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �����o�̓p���X�ݒ�ɂ����āA�ݒ�l���ݒ�\�Ȓl�ƂȂ�悤�ɒ�������B					*/
/*																									*/
/*	�p �r : �I�y���[�^�ł̕����p���X�ݒ�l���̐ݒ�l�����p											*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#define	BITNOCHK	14							/* 2^14 = 16384										*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT	RmxSencAdjustMencDivPulse( PRMDATA *Prm, MENCV *MencV, PRM_ACCCMD Cmd, LONG Data )
{
ULONG	MaskX;
ULONG	DataX;
ULONG	BitNoX;
ULONG	PulseNoX;
ULONG	DataMinX = pndef_pgrat2l.LowerLimit;

/*--------------------------------------------------------------------------------------------------*/
/*		�����o�̓p���X����l�ݒ�																	*/
/*--------------------------------------------------------------------------------------------------*/
		if( MencV->EncConnect )
		{
			PulseNoX = (MencV->PulseNo >> 2);
		}
		else
		{
			PulseNoX = pndef_pgrat2l.UpperLimit;
		}

/*--------------------------------------------------------------------------------------------------*/
/*		�p�����[�^�v�Z����(�p�����[�^�`�F�b�N)														*/
/*--------------------------------------------------------------------------------------------------*/
		if( ( Cmd == PRM_ACCCMD_WRITE) || ( Cmd == PRM_ACCCMD_EEPWRITE) 
			|| ( Cmd == PRM_ACCCMD_RECALC ) )
		{
			if( MencV->AxisMotType == MOTTYPE_LINEAR )
			{
				return( TRUE );
			}
			else
			{
				DataX = Prm->pgrat2l;
				BitNoX = MlibSrhbiton( DataX, SRH_FROM_MSB );
				if( DataX > PulseNoX  ){ return( PRM_RSLT_LIMIT_ERR );}
				if( DataX < DataMinX  ){ return( PRM_RSLT_LIMIT_ERR );}
				if( BitNoX < BITNOCHK ){ return( PRM_RSLT_SUCCESS );}
				MaskX = ( 0xFFFFFFFF << (BitNoX + 1 - BITNOCHK) );
				if( (DataX & MaskX) == DataX ){ return( PRM_RSLT_SUCCESS );}
				return( PRM_RSLT_CALC_ERR );
			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Inc/Dec�f�[�^��������																		*/
/*--------------------------------------------------------------------------------------------------*/
		DataX = Data;
		BitNoX = MlibSrhbiton( DataX, SRH_FROM_MSB );
/*--------------------------------------------------------------------------------------------------*/
/*		�ݒ�͈̓`�F�b�N																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( DataX >= PulseNoX )
		{
			DataX = PulseNoX;									/* �p���X���ŏ�����~�b�g			*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Data >= 2^BITNOCHK �̏ꍇ�́A�ݒ�l�𒲐�����												*/
/*--------------------------------------------------------------------------------------------------*/
		else if( BitNoX >= BITNOCHK )
		{
			MaskX = ( 0xFFFFFFFF << (BitNoX + 1 - BITNOCHK) );
			if( Cmd == PRM_ACCCMD_ADJINCDATA )					/* Adjust Inc. Data					*/
			{
				DataX = (DataX + ~MaskX) & MaskX;				/* Round Up							*/
			}
			else if( Cmd == PRM_ACCCMD_ADJDECDATA )				/* Adjust Dec. Data					*/
			{
				DataX = DataX & MaskX;							/* Round Down						*/
			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Data < 2^BITNOCHK �̏ꍇ�́A���ׂĐݒ�\�Ȃ̂ŁA�������Ȃ�								*/
/*--------------------------------------------------------------------------------------------------*/
		else
		{
			;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Upper/Lower Limit																			*/
/*--------------------------------------------------------------------------------------------------*/
		DataX = MlibLIMITUL( DataX, PulseNoX, DataMinX );
		return( DataX );
}



/****************************************************************************************************/
/*																									*/
/*		���j�A���[�^ : �����o�̓p���X�v�Z �� �`�r�h�b�ݒ�								<S079>		*/
/*				(�����o�͂̈ʒu�ް���JL-086�Œ�����ٽ�ϊ�)											*/
/****************************************************************************************************/
static DIVSET_ERROR LmxSencSetMencDividedPulseHw( ASIC_HW_IF_STR *AsicHwReg, MENCV *MencV, UNI_PRM_STR *UniPrms )
{
	LONG			x;
	LONG			BitNo;
	DIVSET_ERROR	err;

	BitNo  = UniPrms->Prm->MencP.bit_dp.b.l;

/*--------------------------------------------------------------------------------------------------*/
/*		�����o�̓p���X���g��������x�̌v�Z															*/
/*--------------------------------------------------------------------------------------------------*/
	err = LmxSencCalMencDivOverSpeed( MencV, UniPrms );

/*--------------------------------------------------------------------------------------------------*/
/*		�����p���X�o�̓Q�C���̉��Z																	*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->DivInfo.DivMode = FALSE;
	MencV->DivInfo.DivPass = TRUE;
	MencV->DivInfo.DivRate = 0;
	MencV->DivInfo.DivOutGain = MlibScalKxgain( UniPrms->Prm->scalepgrat, 1, (1 << BitNo), NULL, 24 );
	MencV->DivInfo.DivOutCmskChk = MlibScalKxkxks( 0x1000, 1, MencV->DivInfo.DivOutGain, NULL, -30 );
/*--------------------------------------------------------------------------------------------------*/
/*		�`�r�h�b�����@�\�ݒ�																		*/
/*--------------------------------------------------------------------------------------------------*/
	x = (MencV->DivInfo.DivMode << 4) | MencV->DivInfo.DivPass | 0x0400;
	AsicHwReg->AREG_DINCSET = (USHORT)x;						/* �����@�\�ݒ�						*/
	AsicHwReg->AREG_DIVCSEV = (USHORT)MencV->DivInfo.DivRate;	/* ������ݒ�						*/
	AsicHwReg->AREG_DCTRIMH = 0x00;
	AsicHwReg->AREG_DCTRIM  = (USHORT)(MencV->DivInfo.DivRate - 1);
	if( UniPrms->Bprm->RvsDir )
	{
		AsicHwReg->AREG_DINCSET |= BIT2;						/* DB�t�]���[�h�ݒ�					*/
	}
	return (err);
}

/****************************************************************************************************/
/*																									*/
/*		���j�A���[�^ : �����o�̓p���X�v�Z �� �`�r�h�b�ݒ�											*/
/*																									*/
/****************************************************************************************************/
static DIVSET_ERROR LmxSencSetMencDividedPulse( ASIC_HW_IF_STR *AsicHwReg, MENCV *MencV, UNI_PRM_STR *UniPrms )
{
	LONG			x;
	LONG			BitNo;
	DIVSET_ERROR	err;

	BitNo  = UniPrms->Prm->MencP.bit_dp.b.l;

/*--------------------------------------------------------------------------------------------------*/
/*		�����o�̓p���X���g��������x�̌v�Z															*/
/*--------------------------------------------------------------------------------------------------*/
	err = LmxSencCalMencDivOverSpeed( MencV, UniPrms );

/*--------------------------------------------------------------------------------------------------*/
/*		�����p���X�o�̓Q�C���̉��Z																	*/
/*--------------------------------------------------------------------------------------------------*/
#if 0 /*<S02D>*/
	MencV->DivMode = FALSE;
	MencV->DivPass = TRUE;
	MencV->DivRate = 0;
	MencV->DivOutGain = MlibScalKxgain( UniPrms->Prm->scalepgrat, 1, (1 << BitNo), NULL, 24 );
	MencV->DivOutCmskChk = MlibScalKxkxks( 0x1000, 1, MencV->DivOutGain, NULL, -30 );
/*--------------------------------------------------------------------------------------------------*/
/*		�`�r�h�b�����@�\�ݒ�																		*/
/*--------------------------------------------------------------------------------------------------*/
	x = (MencV->DivMode << 4) | MencV->DivPass | 0x0400;
	AsicHwReg->AREG_DINCSET = (USHORT)x;							/* �����@�\�ݒ�						*/
	AsicHwReg->AREG_DIVCSEV = (USHORT)MencV->DivRate;			/* ������ݒ�						*/
	AsicHwReg->AREG_DCTRIMH = 0x00;
	AsicHwReg->AREG_DCTRIM  = (USHORT)(MencV->DivRate - 1);
	if( UniPrms->Bprm->RvsDir )
	{
		AsicHwReg->AREG_DINCSET |= BIT2;							/* DB�t�]���[�h�ݒ�					*/
	}
#else/*<S02D>*/
	MencV->DivInfo.DivMode = FALSE;
	MencV->DivInfo.DivPass = TRUE;
	MencV->DivInfo.DivRate = 0;
	MencV->DivInfo.DivOutGain = MlibScalKxgain( UniPrms->Prm->scalepgrat, 1, (1 << BitNo), NULL, 24 );
	MencV->DivInfo.DivOutCmskChk = MlibScalKxkxks( 0x1000, 1, MencV->DivInfo.DivOutGain, NULL, -30 );
/*--------------------------------------------------------------------------------------------------*/
/*		�`�r�h�b�����@�\�ݒ�																		*/
/*--------------------------------------------------------------------------------------------------*/
	x = (MencV->DivInfo.DivMode << 4) | MencV->DivInfo.DivPass | 0x0400;
	AsicHwReg->AREG_DINCSET = (USHORT)x;						/* �����@�\�ݒ�						*/
	AsicHwReg->AREG_DIVCSEV = (USHORT)MencV->DivInfo.DivRate;	/* ������ݒ�						*/
	AsicHwReg->AREG_DCTRIMH = 0x00;
	AsicHwReg->AREG_DCTRIM  = (USHORT)(MencV->DivInfo.DivRate - 1);
	if( UniPrms->Bprm->RvsDir )
	{
		AsicHwReg->AREG_DINCSET |= BIT2;						/* DB�t�]���[�h�ݒ�					*/
	}

#endif/*<S02D>*/
	return (err);
}
//#endif /* MENCV�֘A�̍\���̂�ύX����K�v����H *//*<S02D>�폜*/
/****************************************************************************************************/
/*																									*/
/*		��]�^�F�����o�̓p���X���g��������x�̌v�Z													*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �����o�̓p���X���ő���g���ƂȂ��]���x���Z�o����B									*/
/*																									*/
/*			�p���X���g�����(SVCDEF_DIVOUT_MAXFREQ) :												*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*			MaxFreq [pps] = pgrate [pulse/rev] * DivOvrSpd [rev/s]									*/
/*																									*/
/*									       MaxFreq * 2^24											*/
/*			DivOvrSpd[2^24/OvrSpd] = ----------------------------	 								*/
/*									  pgrate  * (OvrSpd / 2*PAI) 									*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	LONG	RmxSencCalMencDivOverSpeed( LONG pgrate, KSGAIN OvrSpd )
{
#if (FLOAT_USE==TRUE)
	float	fw;
#else
	LONG	kx, sx;
#endif /* FLOAT_USE */
	LONG	MaxFreq;											/* [pulse/sec]						*/
	LONG	i;
	LONG	pulsno;
	LONG	rc;

	MaxFreq = SVCDEF_DIVOUT_MAXFREQ;
	pulsno = 1;
/*--------------------------------------------------------------------------------------------------*/
/*		����������x��l�Z�o																		*/
/*--------------------------------------------------------------------------------------------------*/
	for ( i = 0; pgrate > 0; i++ )
	{
		pgrate = pgrate / 2;
		pulsno = pulsno * 2;
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�����o�̓p���X���g��������x�̌v�Z															*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	fw = (float)MaxFreq * (float)0x01000000 / (float)pulsno;
	fw = fw / OvrSpd;
	fw = fw * 62832.0F / 10000.0F;
	if( fw > (float)0x01000000 )
	{
		rc = 0x01000000;
	}
	else
	{
		rc = (LONG)fw;
	}
#else
	kx = MlibScalKxgain( MaxFreq, 0x01000000, pulsno, &sx,   0 );
	kx = MlibPcalKxkxks( kx,      1,		  OvrSpd, &sx,   0 );
	rc = MlibPcalKxgain( kx,      62832,      10000,  &sx, -24 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
	return( rc );
}

/****************************************************************************************************/
/*																									*/
/*		���j�A���[�^ : �����o�̓p���X���g��������x�̌v�Z											*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �����o�̓p���X���ő���g���ƂȂ��]���x���Z�o����B									*/
/*																									*/
/*			�p���X���g�����(SVCDEF_DIVOUT_MAXFREQ) : 												*/
/*																									*/
/*			�� SGDS Linear Source �ł� DIVPLSMAXFRQ = 1802240 [pulse/sec]							*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*			MaxFreq [pps] = pgrate [pulse/m] * DivOvrSpd [m/s]										*/
/*																									*/
/*									       MaxFreq * 2^24											*/
/*			DivOvrSpd[2^24/OvrSpd] = ----------------------------	 								*/
/*									      pgrate  * OvrSpd 											*/
/*																									*/
/*								 scalepgrat * 100000000												*/
/*			pgrate[pulse/m] = ---------------------------- 											*/
/*								     ScalePitch * 4													*/
/*																									*/
/*								 MaxFreq * 1000														*/
/*			MaxDivSpd[mm/s] = --------------------	 												*/
/*								     pgrate 														*/
/*																									*/
/*										 MaxFreq * 2^BitNo											*/
/*			MaxDivRat[pulse/spitch] = -----------------------										*/
/*										    MaxPspd / 4												*/
/*																									*/
/*			OvrSpd     : �ő呬�x [rad/s], [m/s]													*/
/*			ScalePitch : �X�P�[���s�b�` [0.01um]													*/
/*			scalepgrat : �G���R�[�_�o�͕���\ [pulse/spitch]										*/
/*			MaxPspd    : ���[�^�ō����x [pulse/sec]													*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static DIVSET_ERROR LmxSencCalMencDivOverSpeed( MENCV *MencV, UNI_PRM_STR *UniPrms )
{

#if (FLOAT_USE==TRUE)
	float	fw;
#else	
	LONG	kx, sx;
#endif /* FLOAT_USE */

	KSGAIN			pgrate;
	KSGAIN			MaxFreq; /* [pulse/sec] */
	LONG			MaxSpd;  /* [r/min], [mm/s] */
	LONG			BitNo;
	DIVSET_ERROR	err;

	err = DIVSET_SUCCESS;

	MaxFreq = SVCDEF_DIVOUT_MAXFREQ;
	MaxSpd  = UniPrms->Bprm->MaxVel * 100;
	BitNo   = UniPrms->Prm->MencP.bit_dp.b.l;

/*--------------------------------------------------------------------------------------------------*/
/*		�o�f������ [pulse/m]																		*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	fw = (float)UniPrms->Prm->scalepgrat * 250000000.0f / (float)MencV->NormScalePitch;
	pgrate = fw * (float)MencV->NormScaleExp;
#else
	kx     = MlibScalKxgain( UniPrms->Prm->scalepgrat, 250000000, MencV->NormScalePitch, &sx, 0 );
	pgrate = MlibPcalKxgain( kx, MencV->NormScaleExp, 1, &sx, -1 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		�����o�̓p���X���g��������x [2^24/OvrSpd] �̌v�Z											*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	fw = (float)MaxFreq * (float)0x01000000 / pgrate;
	fw = fw / (float)UniPrms->Bprm->OvrSpd;
	if( fw > (float)0x01000000 )
	{
		MencV->DivInfo.DivOvrSpd = 0x01000000;
	}
	else
	{
		MencV->DivInfo.DivOvrSpd = (LONG)fw;
	}
#else
	kx = MlibScalKxkxks( MaxFreq, 0x01000000, pgrate, &sx, 0 );
	kx = MlibPcalKxkxks( kx, 1, UniPrms->Bprm->OvrSpd, &sx, -24 );
	if( kx > 0x01000000 )
	{
		MencV->DivInfo.DivOvrSpd = 0x01000000;
	}
	else
	{
		MencV->DivInfo.DivOvrSpd = kx;
	}
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		�����o�̓p���X���g��������̍ō����x [mm/s] �̌v�Z											*/
/*--------------------------------------------------------------------------------------------------*/
#if 0 /* 2010.04.13 Y.Oka MlibGAINRD���g�p�������Ȃ����߁Afloat�ɂĉ��Z���� */

	fw = (float)MaxFreq * 1000.0f / pgrate;
	if( fw > (float)MaxSpd )
	{
		MencV->MaxDivSpd = MaxSpd;
	}
	else
	{
		MencV->MaxDivSpd = (LONG)fw;
	}
#endif
#if (FLOAT_USE==TRUE)
	fw = (float)MaxFreq * 1000.0f / pgrate;
	if( fw > (float)MaxSpd )
	{
		MencV->DivInfo.MaxDivSpd = MaxSpd;
	}
	else
	{
		MencV->DivInfo.MaxDivSpd = (LONG)fw;
	}
#else
	kx = MlibScalKxkxks( MaxFreq, 1000, pgrate, NULL, 24 );
	kx = MlibGAINRD( kx );
	if( kx > MaxSpd )
	{
		MencV->DivInfo.MaxDivSpd = MaxSpd;
	}
	else
	{
		MencV->DivInfo.MaxDivSpd = kx;
	}
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		�ō����x���̕�����ݒ����l [pulse/spitch] �̌v�Z											*/
/*--------------------------------------------------------------------------------------------------*/
#if 0 /* 2010.04.13 Y.Oka MlibGAINRD���g�p�������Ȃ����߁Afloat�ɂĉ��Z���� */
	fw = (float)MaxFreq * (float)(4 << BitNo) / Bprm->MaxPspd;
	if( fw > (float)(1<<BitNo) )
	{
		MencV->MaxDivRat = 1 << BitNo;
	}
	else
	{
		MencV->MaxDivRat = (LONG)fw;
	}
#endif
#if (FLOAT_USE==TRUE)
	fw = (float)MaxFreq * (float)(4 << BitNo) / UniPrms->Bprm->MaxPspd;
	if( fw > (float)(1<<BitNo) )
	{
		MencV->DivInfo.MaxDivRat = 1 << BitNo;
	}
	else
	{
		MencV->DivInfo.MaxDivRat = (LONG)fw;
	}
#else
	kx = MlibScalKxkxks( MaxFreq, (4 << BitNo), UniPrms->Bprm->MaxPspd, NULL, 24 );
	kx = MlibGAINRD( kx );
	if( kx > (1<<BitNo) )
	{
		MencV->DivInfo.MaxDivRat = 1 << BitNo;
	}
	else
	{
		MencV->DivInfo.MaxDivRat = kx;
	}
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		�����p���X�o�͐ݒ�ُ�																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( (MencV->DivInfo.MaxDivRat < (LONG)UniPrms->Prm->scalepgrat) || (MencV->DivInfo.MaxDivSpd < MaxSpd) )
	{
		err = DIVSET_DIVERR;							/* A.041 : �����p���X�o�͐ݒ�ُ�	*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�p�����[�^�ݒ�ُ�																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->DivInfo.DivOvrSpd == 0 )
	{
		err = DIVSET_PRMERR_L;							/* A.040 : �p�����[�^�G���[�̐ݒ�	*/
	}
	return (err);
}






/****************************************************************************************************/
/*																									*/
/*		��]�^�F�G���R�[�_�����p�����[�^�v�Z														*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �T�[�{��{�p�����[�^�v�Z(BprmCalc.c)��A�G���R�[�_�h�e�����ɕK�v�ȃp�����[�^��			*/
/*			�v�Z���s���B																			*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static void	SencInitParamCalcForRotary( MENCV *MencV, MENCPRM *MencP, MENC_REF_PRM *MencIPrm )
{
#if (FLOAT_USE==TRUE)
	float	fw;
#else
	LONG	sx;
#endif
	LONG	kx;
	UCHAR	BitNo;
	UCHAR	Dpoint;

	BitNo  = MencP->bit_dp.b.l;
	Dpoint = MencP->bit_dp.b.h;

/*--------------------------------------------------------------------------------------------------*/
/*		�ʑ����_���Z�p�p�����[�^�̌v�Z																*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->MorgSft      = 100 - Dpoint;
	MencV->MorgPassSft  =  96 - Dpoint;
	if( MencV->Sigma3Encoder )
	{
		MencV->IncreCompSft = 4;
	}
	else
	{
		MencV->IncreCompSft = 0;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu���Z�p�p�����[�^�̌v�Z																	*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->MposSftR = 32 - BitNo;
	MencV->MposSftX = MencV->MposSftR - (80 - Dpoint);
	MencV->MposSign = (MencV->RevAsmMotor == TRUE)?  -1 : 1;
	MencV->MultiturnSft = MencP->bit_dp.b.h - 64;

/*--------------------------------------------------------------------------------------------------*/
/*		�b�����X�t���O�ݒ�																			*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->CphaseLess = FALSE;

/*--------------------------------------------------------------------------------------------------*/
/*		���[�^���Ɉʑ����Z�W���̌v�Z																*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		MotPhase [65536/360] = (PoleNo/2) * (MotPosX [2^32/Rev] >> 16)								*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->Kmotphase = (MencP->oslv_pol.b.h >> 1);

/*--------------------------------------------------------------------------------------------------*/
/*		�����x�ُ팟�o���x���v�Z																	*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*					  MaxTrq * 2^32	*  Ts  *  Ts			MaxTrq * Ts * Ts						*/
/*		AccErrLvl = ------------------------------ * Cx	 = ------------------- * Cx			 		*/
/*					   Jmot * 2*PAI	* 10^9 * 10^9			Jmot * 1462918079						*/
/*																									*/
/*		MaxTrq [Nm],[N]					: �ő�g���N												*/
/*		Jmot   [kgm^2],[kg]				: ���[�^�C�i�[�V��											*/
/*		Kfbpls [rad/pulse],[m/pulse]	: �e�a�p���X�ϊ��W��										*/
/*		Ts     [ns]	   					: ���Z����													*/
/*		Cx     [-]	   					: ���S�W�� (=400.000) 										*/
/*																									*/
/*		�����S�W���͉�]�^SGDS�̖�400�{�ɍ��킹���B													*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	fw = (float)KPI_SACYCLENS * (float)KPI_SACYCLENS / 1462918.0F;
	fw = fw * MencIPrm->MaxTrq / MencIPrm->Jmot;
	fw = fw * 400000.0F / 1000000.0F;
#else	
	kx = MlibScalKxgain( KPI_SACYCLENS, KPI_SACYCLENS, 1462918, &sx,  0 );
	kx = MlibPcalKxksks( kx, MencIPrm->MaxTrq, MencIPrm->Jmot, &sx,   0 );
	kx = MlibPcalKxgain( kx, 400000,        1000000,   &sx, -30 );
#endif /* FLOAT_USE */

	if( MencV->PosCompMode == POSCOMP_SW )/* <S004> *//*<S05C>rename*/
	{/* SW�ɂăG���R�[�_�ʒu�␳(��-�X�݊�):32bit���K�� */
		MencV->AccErrLvl = kx;
	}
	else
	{/* JL-086�ɂăG���R�[�_�ʒu�␳:24bit���K��		*/
		MencV->AccErrLvl = kx >> ( 32 - 24);
		if( MencV->AccErrLvl > 0x007FFFFF )
		{
			MencV->AccErrLvl = 0x007FFFFF;
		}
	}

/*--------------------------------------------------------------------------------------------------*/
/*		���x���Z�Q�C���v�Z	[pulse/ScanA]��[2^24/]													*/
/*--------------------------------------------------------------------------------------------------*/
//	MencV->MotspdA = MlibScalKskxkx( bprm_ptr->Kmotspd, 1000000000, KPI_SACYCLENS, NULL, 24 );

/*--------------------------------------------------------------------------------------------------*/
/*		�ړ��ʕϊ��Q�C���̌v�Z																		*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*				     1000																			*/
/*		Kmovpos = -----------		[0.001rev/pulse]												*/
/*				   (2^BitNo) 																		*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	fw = 1000.0f / (1 << BitNo);
#else
	kx = MlibScalKxgain( 1000, 1, (1 << BitNo), NULL, 24 );
#endif /* FLOAT_USE */

#if (FLOAT_USE==TRUE)
	MencV->Kmovpos =  fw ;
#else
	MencV->Kmovpos = kx;
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		�ړ��ʕϊ��Q�C���̌v�Z																		*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*					 pole * 65536																	*/
/*		Kphasepos = --------------		[65536/(360deg)/pulse]										*/
/*					(2^BitNo) * 2 																	*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	fw = MencP->oslv_pol.b.h * 32768.0f / (1 << BitNo);
#else
	kx = MlibScalKxgain( MencP->oslv_pol.b.h, 32768, (1 << BitNo), NULL, 24 );
#endif /* FLOAT_USE */

#if (FLOAT_USE==TRUE)
	MencV->Kphasepos = fw;
#else
	MencV->Kphasepos = kx;
#endif

}



/****************************************************************************************************/
/*																									*/
/*		���j�A���[�^�F�G���R�[�_�����p�����[�^�v�Z													*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �T�[�{��{�p�����[�^�v�Z(BprmCalc.c)��A�G���R�[�_�h�e�����ɕK�v�ȃp�����[�^��			*/
/*			�v�Z���s���B																			*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static void	SencInitParamCalcForLinear( MENCV *MencV, MENCPRM *MencP, MENC_REF_PRM *MencIPrm )
{
#if (FLOAT_USE==TRUE)
	float	fw;
#endif
	LONG	kx, sx;
	UCHAR	BitNo;
	UCHAR	Dpoint;

	BitNo  = MencP->bit_dp.b.l;
	Dpoint = MencP->bit_dp.b.h;

/*--------------------------------------------------------------------------------------------------*/
/*		�ʑ����_���Z�p�p�����[�^�̌v�Z																*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->MorgSft      = 100 - Dpoint;
	MencV->MorgPassSft  =  96 - Dpoint;
	if( MencV->Sigma3Encoder )
	{
		MencV->IncreCompSft = 4;
	}
	else
	{
		MencV->IncreCompSft = 0;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu���Z�p�p�����[�^�̌v�Z																	*/
/*--------------------------------------------------------------------------------------------------*/
//	MencV->MposSftR = 32 - BitNo;											/* <S004> */
//	MencV->MposSftX = MencV->MposSftR - (80 - Dpoint);			/* <S004> */
	MencV->MposSftR = LMX_MPOS_SFT;									/* <S004> */
	MencV->MposSftX = LMX_MPOS_SFT;									/* <S004> */

	if( MencV->RevAsmMotor == TRUE )
	{
		MencV->MposSign = -1;
	}
	else
	{
		MencV->MposSign = 1;
	}
	MencV->MultiturnSft = MencP->bit_dp.b.h - 64;

/*--------------------------------------------------------------------------------------------------*/
/*		�b�����X�t���O�ݒ�																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->AbsoEncoder )
	{
		MencV->CphaseLess = TRUE;
	}
	else
	{
		MencV->CphaseLess = FALSE;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		���[�^���Ɉʑ����Z�W���̌v�Z																*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*						 	   65536							PulseNo  : [pulse/360deg]			*/
/*			Kmotphase	= ---------------						MotPhase : [65536/360deg]			*/
/*						      PulseNo																*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	MencV->Kmotphase = 65536.0f / (float)MencV->PulseNo;
	MencV->Kinvphase = (float)MencV->PulseNo / 65536.0f;
#else
	MencV->Kmotphase = MlibScalKxgain( 65536, 1, MencV->PulseNo, NULL, 24 );
	MencV->Kinvphase = MlibScalKxgain( MencV->PulseNo, 1, 65536, NULL, 24 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		�����x�ُ팟�o���x���v�Z																	*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*					  MaxTrq * 2^12	*  Ts  *  Ts			       MaxTrq * Ts * Ts					*/
/*		AccErrLvl = ------------------------------ * Cx	 = --------------------------------- * Cx	*/
/*					  Jmot * Kfbpls * 10^9 * 10^9			Jmot * Kfbpls * 244140625000000			*/
/*																									*/
/*		MaxTrq [Nm],[N]					: �ő�g���N												*/
/*		Jmot   [kgm^2],[kg]				: ���[�^�C�i�[�V��											*/
/*		Kfbpls [rad/pulse],[m/pulse]	: �e�a�p���X�ϊ��W��										*/
/*		Ts     [ns]	   					: ���Z����													*/
/*		Cx     [-]	   					: ���S�W�� (=400.000) 										*/
/*																									*/
/*		�����S�W���͉�]�^SGDS�̖�400�{�ɍ��킹���B													*/
/*		���������ASGDS���j�A�͈ʒu�f�[�^���p���X�P�ʂł���̂ɉ�]�^�Ɠ����v�Z�l���g���Ă���̂ŁA	*/
/*		  ���j�A�̏ꍇ�ASGDS��SVOS�ƂňقȂ�B														*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	fw = (float)KPI_SACYCLENS * (float)KPI_SACYCLENS / 244140625.0F;
	fw = fw * MencIPrm->MaxTrq / MencIPrm->Jmot;
	fw = fw / MencIPrm->Kfbpls;
	fw = fw * 400.0F / 1000000.0F;
#else
	kx = MlibScalKxgain( KPI_SACYCLENS, KPI_SACYCLENS, 244140625, &sx, 0 );
	kx = MlibPcalKxksks( kx, MencIPrm->MaxTrq, MencIPrm->Jmot, &sx, 0 );
	kx = MlibPcalKxkxks( kx, 1, MencIPrm->Kfbpls, &sx, 0 );
	kx = MlibPcalKxgain( kx, 400, 1000000, &sx, -30 );
#endif /*FLOAT_USE==TRUE*/

	if( MencV->PosCompMode == POSCOMP_SW )/* <S004> *//*<S0C5>rename*/
	{/* SW�ɂăG���R�[�_�ʒu�␳(��-�X�݊�):32bit���K�� */
		MencV->AccErrLvl = kx;
	}
	else
	{/* JL-086�ɂăG���R�[�_�ʒu�␳:24bit���K��		*/
		MencV->AccErrLvl = (kx >> (32 - 24));
		if( MencV->AccErrLvl > 0x007FFFFF )
		{
			MencV->AccErrLvl = 0x007FFFFF;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�ړ��ʕϊ��Q�C���̌v�Z																		*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*					      ScalePitch					ScalePitch : [nm]							*/
/*		Kmovpos  = ------------------------				Kmovpos    : [0.01mm/pulse]					*/
/*					   (2^BitNo) * 10000															*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
	kx = MlibScalKxgain( MencV->NormScalePitch, 1, (10000<< BitNo), &sx, 0 );
	kx = MlibPcalKxgain( kx, 1, MencV->NormScaleExp, &sx, 24 );

	MencV->Kmovpos = kx;

/*--------------------------------------------------------------------------------------------------*/
/*		�ړ��ʕϊ��Q�C���̌v�Z																		*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*					   (ScalePitch/10^9) * 65536		ScalePitch : [nm]							*/
/*		Kphasepos = ---------------------------------	[65536/(360deg)/pulse]						*/
/*					(2^BitNo) * (PolePitch/10^4) * 2 	PolePitch  : [0.1mm/180deg]					*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
	kx = MlibScalKxgain( MencV->NormScalePitch, 32768, 100000, &sx, 0 );
	kx = MlibPcalKxgain( kx, 1, (1 << BitNo), &sx, 0 );
	kx = MlibPcalKxgain( kx, 1, MencP->ppitch, &sx, 0 );
	kx = MlibPcalKxgain( kx, 1, MencV->NormScaleExp, &sx, 24 );

	MencV->Kphasepos = kx;

/*--------------------------------------------------------------------------------------------------*/
/*		���x���Z�Q�C���v�Z	[pulse/ScanA]��[2^24/]													*/
/*--------------------------------------------------------------------------------------------------*/
//	MencV->MotspdA = MlibScalKskxkx( bprm_ptr->Kmotspd, 1000000000, KPI_SACYCLENS, NULL, 24 );

/*--------------------------------------------------------------------------------------------------*/
/*		PnE8C   : �V���A���ϊ����j�b�g���_�I�t�Z�b�g												*/
/*																									*/
/*		MencP.OrgDetDelayComp = PnE8C [0.01deg]�~2^BITNO / 100 / 360[deg]							*/
/*--------------------------------------------------------------------------------------------------*/
/*		��C���ʒu�v�Z�Ŏg�p���邽�߁ALMX_MPOS_SFT�����V�t�g���Ă���									*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->OrgDetDelayComp = ( ((MencIPrm->scnvorgoffset << BitNo) / 100) << LMX_MPOS_SFT ) / 360;

}



/****************************************************************************************************/
/*																									*/
/*		SEN ON���̐�Έʒu�Čv�Z																	*/
/*																									*/
/****************************************************************************************************/
void	SencMakeAbsPosition(MENCV *MencV, USHORT limmlt, DBYTEX bit_dp, BOOL RvsDir)
{
	LONG	IniPos[2];
	LONG	MaxPos[2];
	UCHAR	BitNo;
	UCHAR	Sftx;

	/* ���[�J���ϐ��̏����� */
	BitNo	= bit_dp.b.l;
	Sftx	= bit_dp.b.h - BitNo - 48;

	IniPos[1] = (LONG)((USHORT)MencV->RxPosH[0] >> Sftx);			/* <S209> */
	IniPos[0] = (LONG)( ((ULONG)MencV->RxPosH[0] << (32-Sftx))
						 + ((ULONG)MencV->RxPosL[0].dw >> Sftx) );	/* <S209> */

	if( MencV->AbsoEncType == MENC_SABSO_TYPE )		/* 1��]�A�u�\								*/
	{
		IniPos[1] = 0;
		IniPos[0] = ((ULONG)(IniPos[0]<<(32-BitNo)) >> (32-BitNo));
	}
	else												/* ����]�A�u�\								*/
	{
		if( limmlt == 0xFFFF )							/* �}���`�^�[�����~�b�g�ŕ����g�����f		*/
		{												/* ����]�ʕ����t��							*/
			if( BitNo > 16 )							/* �����r�b�g�ŏ�ʃr�b�g��ݒ�				*/
			{
				IniPos[1] = (LONG)(IniPos[1] << (48-BitNo)) >> (48-BitNo);
			}
			else										/* ���ʃ��[�h�̕����ŏ�ʃ��[�h��ݒ�		*/
			{
				IniPos[1] = IniPos[0] >> 31;			/* ��ʃ��[�h�ݒ�(���ʃ��[�h����)			*/
			}
		}
	}

	/*----------------------------------------------------------------------------------------------*/
	if( MencV->RevAsmMotor ^ RvsDir )					/* ���]�ڑ� XOR �t��]���[�h				*/
	{
		IniPos[1] = (~IniPos[1]);						/* �������]����								*/
		IniPos[0] = (~IniPos[0]) + 1;					/* �������]����								*/
		if( IniPos[0] == 0 )
		{	/* Carry�̏ꍇ�́A��ʂ�+1 */
			++IniPos[1];
		}
	/*----------------------------------------------------------------------------------------------*/
		if( (limmlt != 0xFFFF) && ((IniPos[0] | IniPos[1]) != 0) )
		{
			MaxPos[1] = ((LONG)limmlt + 1) >> (32-BitNo);
			MaxPos[0] = ((LONG)limmlt + 1) <<  BitNo;
			MlibAdd6464( MaxPos, IniPos, IniPos );
		}
	}


	/*	����}���`�^�[���f�[�^�̐ݒ� */
	if( (MencV->AbsoEncoder == FALSE)						/* �C���N���G���R�[�_					*/
		|| (MencV->AbsoEncType == MENC_INCRE_TYPE) )		/* �A�u�\���C���N���Ƃ��Ďg�p			*/
	{
		MencV->MotAbsPos = 0;
		MencV->SenOnScalePosL = 0;
		MencV->SenOnScalePosH = 0;
		MencV->SenOnMtnData = 0;
	}
	else													/* �A�u�\�G���R�[�_						*/
	{
		MencV->MotAbsPos = IniPos[0];
		MencV->SenOnScalePosL = IniPos[0];
		MencV->SenOnScalePosH = IniPos[1];
		MencV->SenOnMtnData = SencGetMturnData(MencV,
		                                        limmlt, bit_dp,
		                                        (USHORT)MencV->RxPosH[0],
		                                        MencV->RxPosL[0].dw );
	}
	MencV->SenOnIncPulse = (ULONG)(MencV->RxPosL[0].dw >> MencV->MposSftX);
}





/****************************************************************************************************/
/*																									*/
/*		��]�^�F�V���A���G���R�[�_�ϐ�����������													*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Phase Table for Initial Phase																*/
/*--------------------------------------------------------------------------------------------------*/
static	const	USHORT	PhaseTableUVW[8] = {
/*	UVW   deg	Cal.					65536/360deg												*/
/*--------------------------------------------------------------------------------------------------*/
/*	000 :  NG : 0				*/		    0,
/*	001 : 330 : 65536*330/360	*/		60075,
/*	010 : 210 : 65536*210/360	*/		38229,
/*	011 : 270 : 65536*270/360	*/		49152,
/*	100 :  90 : 65536* 90/360	*/		16384,
/*	101 :  30 : 65536* 30/360	*/		 5461,
/*	110 : 150 : 65536*150/360	*/		27307,
/*	111 :  NG : 0				*/		    0
};
/*--------------------------------------------------------------------------------------------------*/
static SENC_ERROR	SencInitVariablesForRotary( MENCV *MencV, MENCPRM *MencP, ASIC_HW_IF_STR *AsicHwReg )
{
	LONG	i;
	LONG	lwk;
	LONG	RcvPosX;

/*--------------------------------------------------------------------------------------------------*/
/*		����ʒu�f�[�^(����,�O��,�O�X��)�̐ݒ�														*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->MotPos = 0;									/* Motor Position [32bitRingPulse]		*/
	/*----------------------------------------------------------------------------------------------*/
	RcvPosX = MencV->MposSign * (LONG)((MencV->RxPosL[0].dw>>MencV->MposSftX)<<MencV->MposSftR);
	MencV->RcvPosX[0] = MencV->RcvPosX[1] = MencV->RcvPosX[2] = RcvPosX;
	MencV->MotPosX[0] = MencV->MotPosX[1] = RcvPosX;				/* �␳��ʒu������ 		*/

/*--------------------------------------------------------------------------------------------------*/
/*		����}���`�^�[���f�[�^�̐ݒ�																*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->AbsoEncoder == FALSE )					/* �C���N���G���R�[�_					*/
	{
		MencV->IniMturn = 0;
	}
	else if( MencV->AbsoEncType == MENC_INCRE_TYPE )		/* �A�u�\���C���N���Ƃ��Ďg�p			*/
	{
		MencV->IniMturn = 0;
	}
	else													/* �A�u�\�G���R�[�_						*/
	{
		MencV->IniMturn = SencGetMturnData(MencV,
	                                        MencP->limmlt, MencP->bit_dp,
		                                    (USHORT)MencV->RxPosH[0],
		                                    MencV->RxPosL[0].dw);
	}
	MencV->IniIncre = ((ULONG)RcvPosX >> MencV->MposSftR);

/*--------------------------------------------------------------------------------------------------*/
/*		�b���t���O�֘A,�ʑ����_�̏����ݒ�F�C���N���G���R�[�_										*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->AbsoEncoder != TRUE )						/* �C���N���G���R�[�_					*/
	{
		MencV->Cset     = FALSE;							/* �b���ݒ�Flag							*/
		MencV->Cpass    = FALSE;							/* �b���ʉ�Flag							*/
//		MencV->DivCpass = FALSE;							/* �����b���ʉ�Flag						*/
		MencV->DivInfo.DivCpass = FALSE;					/* �����b���ʉ�Flag						*//*<S02D>*/
		MencV->OrgOk    = FALSE;							/* ���_�ݒ芮��Flag						*/
		MencV->CposOk   = FALSE;							/* �b���ʒu�ݒ芮��Flag					*/
	/*----------------------------------------------------------------------------------------------*/
	/*	�������[�^�ʑ����_���Z�F�t�u�v�M����艉�Z													*/
	/*----------------------------------------------------------------------------------------------*/
		if( MencV->Sigma3Encoder )
		{
			i = MencV->RxPosH[0] >> 1;
		}
		else
		{
			i = MencV->RxPosL[0].w.l >> 9;
		}
		lwk = (PhaseTableUVW[(i & 0x07)] * (65536/MencV->Kmotphase));
	/*------------------------------------------------------------------------------------------*/
		MencV->MotOrgX  = RcvPosX - lwk;					/* ���[�^���_�ʒu						*/
		MencV->MotCposX = 0;								/* ���[�^�b���ʒu						*/
		MencV->MotAngle = 0;								/* ���[�^��]�p(�p���X)					*/
	/*----------------------------------------------------------------------------------------------*/
	/*	�����o�͏����ݒ�																			*/
	/*----------------------------------------------------------------------------------------------*/
//		if( MencV->DivOut == TRUE )						/* ���[�^�G���R�[�_�����o�͑I����		*/
		if( MencV->DivInfo.DivOut == TRUE )					/* ���[�^�G���R�[�_�����o�͑I����		*//*<S02D>*/
		{
			/* �ʏ�b���o�͑I��(INGRAM�����o�͖��Ή�) */
//			KPI_DIVSET_DCNORMAL( );
#if 0 /*<S02D>*/

			MencV->DivModeStatus = DIV_MODE_NORMAL ;

			AsicHwReg->AREG_WDT1SET |= BIT8;				/* �p���X�ϊ��b���}�X�N					*/
			MencV->DivCMaskStatus = DIV_CMASK_ON;
			AsicHwReg->AREG_POSET0	= (USHORT)(RcvPosX >> MencV->DivOutSft);
			AsicHwReg->AREG_POTEST	= (USHORT)(RcvPosX >> MencV->DivOutSft);
#else/*<S02D>*/
			/* �ʏ�b���o�͑I��(INGRAM�����o�͖��Ή�) */
			MencV->DivInfo.DivModeStatus = DIV_MODE_NORMAL ;

			AsicHwReg->AREG_WDT1SET |= BIT8;				/* �p���X�ϊ��b���}�X�N					*/
			MencV->DivInfo.DivCMaskStatus = DIV_CMASK_ON;
			AsicHwReg->AREG_POSET0	= (ULONG)(RcvPosX >> MencV->DivInfo.DivOutSft);
			AsicHwReg->AREG_POTEST	= (ULONG)(RcvPosX >> MencV->DivInfo.DivOutSft);
			if(( MencV->DivInfo.DivPass != FALSE )&&(MencV->DivInfo.DivOutASIC != FALSE))          /*<S0D3>*/
			{/* �����X���[���[�h�AH/W�����̏ꍇ�@*/
				/* �ʏ�b���o�͑I�� */
				AsicHwReg->AREG_DIVSET &= 0xfffff3ff;
				MencV->DivInfo.DivModeStatus = DIV_MODE_NORMAL ;
			}
#endif/*<S02D>*/
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�b���t���O�֘A,�ʑ����_�̏����ݒ�F�A�u�\���C���N���Ƃ��Ďg�p								*/
/*--------------------------------------------------------------------------------------------------*/
	else if( MencV->AbsoEncType == MENC_INCRE_TYPE )		/* �A�u�\���C���N���Ƃ��Ďg�p			*/
	{
		MencV->Cset     = TRUE;							/* �b���ݒ�Flag							*/
		MencV->Cpass    = FALSE;							/* �b���ʉ�Flag							*/
//		MencV->DivCpass = FALSE;							/* �����b���ʉ�Flag						*/
		MencV->DivInfo.DivCpass = FALSE;					/* �����b���ʉ�Flag						*//*<S02D>*/
		MencV->OrgOk    = FALSE;							/* ���_�ݒ芮��Flag						*/
		MencV->CposOk   = FALSE;							/* �b���ʒu�ݒ芮��Flag					*/
	/*----------------------------------------------------------------------------------------------*/
		MencV->MotOrgX  = 0;								/* ���[�^���_�ʒu						*/
		MencV->MotCposX = 0;								/* ���[�^�b���ʒu						*/
		MencV->MotAngle = ((ULONG)(RcvPosX) >> MencV->MposSftR); /* ���[�^��]�p(�p���X)		*/
	/*----------------------------------------------------------------------------------------------*/
	/*	�����o�͏����ݒ�																			*/
	/*----------------------------------------------------------------------------------------------*/
#if 0 /*<S02D>*/
		if( MencV->DivOut == TRUE )						/* ���[�^�G���R�[�_�����o�͑I����		*/
		{
			AsicHwReg->AREG_POSET0	= (USHORT)(RcvPosX >> MencV->DivOutSft);
			AsicHwReg->AREG_POTEST	= (USHORT)(RcvPosX >> MencV->DivOutSft);
			/* �ʏ�b���o�͑I�� */
//			KPI_DIVSET_DCNORMAL( );
			MencV->DivModeStatus = DIV_MODE_NORMAL ;

			/* �b���ē����ݒ� */
			AsicHwReg->AREG_DINCSET	|= BIT5;

			/* �����펞�N���A�ݒ�OFF(�K�v���H) */
			AsicHwReg->AREG_DINCSET	&= ~(BIT10);

			/* �b���ē������� */
			AsicHwReg->AREG_DINCSET	&= ~(BIT5);

			/* �p���X�ϊ��b���}�X�N�n�m */
			AsicHwReg->AREG_WDT1SET |= BIT8;
			MencV->DivCMaskStatus = DIV_CMASK_ON;

			/* �p���X�ϊ��b���}�X�N�n�e�e(INGRAM�����o�͖��Ή�) */
//			KPI_PCMASK_OFF( );
			MencV->DivCMaskStatus  = DIV_CMASK_OFF ;
		}
#else/*<S02D>*/
		if( MencV->DivInfo.DivOut == TRUE )					/* ���[�^�G���R�[�_�����o�͑I����		*/
		{
			AsicHwReg->AREG_POSET0	= (ULONG)(RcvPosX >> MencV->DivInfo.DivOutSft);
			AsicHwReg->AREG_POTEST	= (ULONG)(RcvPosX >> MencV->DivInfo.DivOutSft);
			/* �ʏ�b���o�͑I��				*/
			AsicHwReg->AREG_DIVSET &= 0xfffff3ff;
			MencV->DivInfo.DivModeStatus = DIV_MODE_NORMAL ;

			/* �b���ē����ݒ� */
			AsicHwReg->AREG_DINCSET	|= BIT5;

			/* �����펞�N���A�ݒ�OFF(�K�v���H) */
			AsicHwReg->AREG_DINCSET	&= ~(BIT10);

			/* �b���ē������� */
			AsicHwReg->AREG_DINCSET	&= ~(BIT5);

			/* �p���X�ϊ��b���}�X�N�n�m */
			AsicHwReg->AREG_WDT1SET |= BIT8;
			MencV->DivInfo.DivCMaskStatus = DIV_CMASK_ON;

			/* �p���X�ϊ��b���}�X�N�n�e�e */
			AsicHwReg->AREG_WDT1SET &= ~BIT8;
			MencV->DivInfo.DivCMaskStatus  = DIV_CMASK_OFF ;
		}
#endif/*<S02D>*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�b���t���O�֘A,�ʑ����_�̏����ݒ�F�A�u�\���A�u�\�Ƃ��Ďg�p									*/
/*--------------------------------------------------------------------------------------------------*/
	else													/* �A�u�\���A�u�\�Ƃ��Ďg�p				*/
	{
#if 0/*<S02D>*/
		MencV->Cset     = TRUE;							/* �b���ݒ�Flag							*/
		MencV->Cpass    = TRUE;							/* �b���ʉ�Flag							*/
		MencV->DivCpass = TRUE;							/* �����b���ʉ�Flag						*/
		MencV->OrgOk    = TRUE;							/* ���_�ݒ芮��Flag						*/
		MencV->CposOk   = FALSE;							/* �b���ʒu�ݒ芮��Flag					*/
	/*----------------------------------------------------------------------------------------------*/
		MencV->MotOrgX  = 0;								/* ���[�^���_�ʒu						*/
		MencV->MotCposX = 0;								/* ���[�^�b���ʒu						*/
		MencV->MotAngle = ((ULONG)(RcvPosX) >> MencV->MposSftR); /* ���[�^��]�p(�p���X)		*/
	/*----------------------------------------------------------------------------------------------*/
	/*	�����o�͏����ݒ�																			*/
	/*----------------------------------------------------------------------------------------------*/
		if( MencV->DivOut == TRUE )						/* ���[�^�G���R�[�_�����o�͑I����		*/
		{
			if( MencV->DivPass == FALSE )					/* �����X���[���[�h�H					*/
			{
				/* �����b���o�͑I���Q(INGRAM�����o�͖��Ή�) */
//				KPI_DIVSET_DCC2( );
				MencV->DivModeStatus = DIV_MODE_DCC2 ;
			}
			else
			{
				/* �ʏ�b���o�͑I��(INGRAM�����o�͖��Ή�) */
//				KPI_DIVSET_DCNORMAL( );
				MencV->DivModeStatus = DIV_MODE_NORMAL ;
			}
		}
#else/*<S02D>*/
		MencV->Cset     = TRUE;								/* �b���ݒ�Flag							*/
		MencV->Cpass    = TRUE;								/* �b���ʉ�Flag							*/
		MencV->DivInfo.DivCpass = TRUE;						/* �����b���ʉ�Flag						*/
		MencV->OrgOk    = TRUE;								/* ���_�ݒ芮��Flag						*/
		MencV->CposOk   = FALSE;							/* �b���ʒu�ݒ芮��Flag					*/
	/*----------------------------------------------------------------------------------------------*/
		MencV->MotOrgX  = 0;								/* ���[�^���_�ʒu						*/
		MencV->MotCposX = 0;								/* ���[�^�b���ʒu						*/
		MencV->MotAngle = ((ULONG)(RcvPosX) >> MencV->MposSftR);		/* ���[�^��]�p(�p���X)		*/
	/*----------------------------------------------------------------------------------------------*/
	/*	�����o�͏����ݒ�																			*/
	/*----------------------------------------------------------------------------------------------*/
		if( MencV->DivInfo.DivOut == TRUE )					/* ���[�^�G���R�[�_�����o�͑I����		*/
		{
			if( MencV->DivInfo.DivPass == FALSE )			/* �����X���[���[�h�H					*/
			{
				/* �����b���o�͑I���Q */
				AsicHwReg->AREG_DIVSET |= (BIT11 | BIT8);
				MencV->DivInfo.DivModeStatus = DIV_MODE_DCC2 ;
			}
			else
			{
				/* �ʏ�b���o�͑I�� */
				AsicHwReg->AREG_DIVSET &= 0xfffff3ff;
				MencV->DivInfo.DivModeStatus = DIV_MODE_NORMAL ;
			}
		}
#endif/*<S02D>*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�G���R�[�_���f�B�ݒ� : �G���R�[�_�������E�p�����[�^�������̌�Őݒ�							*/
/*--------------------------------------------------------------------------------------------------*/
#if 0/*<S02D>*/
	if( MencV->EncConnect == FALSE )						/* �G���R�[�_���ڑ���					*/
	{
		MencV->SenReady = FALSE;							/* SEN�M����������Flag					*/
		MencV->IncPulseReq = FALSE;						/* �����C���N���p���X�o�͗v��			*/
	}
	/*----------------------------------------------------------------------------------------------*/
	else if( MencV->DivOut == FALSE )					/* ���[�^�G���R�[�_�����o�́F��I����	*/
	{
		MencV->SenReady = TRUE;							/* SEN�M����������Flag					*/
		MencV->IncPulseReq = FALSE;						/* �����C���N���p���X�o�͗v��			*/
	}
	/*----------------------------------------------------------------------------------------------*/
	else if( MencV->AbsoEncType )						/* �A�u�\�G���R�[�_��					*/
	{
		/* �V���A���o�͂��s��Ȃ����߁A����������TRUE�Ƃ���B										*/
		MencV->SenReady = TRUE;							/* SEN�M����������Flag					*/
		MencV->IncPulseReq = TRUE;						/* �����C���N���p���X�o�͗v��			*/
	}
	/*----------------------------------------------------------------------------------------------*/
	else													/* �C���N���G���R�[�_��					*/
	{
		MencV->SenReady = TRUE;							/* SEN�M����������Flag					*/
		MencV->IncPulseReq = FALSE;						/* �����C���N���p���X�o�͗v��			*/
	}
#else/*<S02D>*/
	if( MencV->EncConnect == FALSE )						/* �G���R�[�_���ڑ���					*/
	{
		MencV->SenReady = FALSE;							/* SEN�M����������Flag					*/
		MencV->DivInfo.IncPulseReq = FALSE;					/* �����C���N���p���X�o�͗v��			*/
	}
	/*----------------------------------------------------------------------------------------------*/
	else if( MencV->DivInfo.DivOut == FALSE )				/* ���[�^�G���R�[�_�����o�́F��I����	*/
	{
		MencV->SenReady = TRUE;								/* SEN�M����������Flag					*/
		MencV->DivInfo.IncPulseReq = FALSE;					/* �����C���N���p���X�o�͗v��			*/
	}
	/*----------------------------------------------------------------------------------------------*/
	else if( MencV->AbsoEncType )							/* �A�u�\�G���R�[�_��					*/
	{
		/* �V���A���o�͂��s��Ȃ����߁A����������TRUE�Ƃ���B										*/
		MencV->SenReady = TRUE;								/* SEN�M����������Flag					*/
		MencV->DivInfo.IncPulseReq = TRUE;					/* �����C���N���p���X�o�͗v��			*/
	}
	/*----------------------------------------------------------------------------------------------*/
	else													/* �C���N���G���R�[�_��					*/
	{
		MencV->SenReady = TRUE;								/* SEN�M����������Flag					*/
		MencV->DivInfo.IncPulseReq = FALSE;					/* �����C���N���p���X�o�͗v��			*/
	}

#endif/*<S02D>*/
/*--------------------------------------------------------------------------------------------------*/
/*		�G���R�[�_�t���O�֘A�F���ʐݒ�																*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->MotCphPass		= FALSE;					/* �b���ʉ�Flag							*/
	MencV->IncreCposLatch	= FALSE;					/* �b���ʒu���b�`�M��(ScanA-->ScanB)	*/
	/*----------------------------------------------------------------------------------------------*/
	MencV->PhaseReady		= TRUE;						/* �ʑ����o����Flag						*/
//	MencV->EncDisable		= FALSE;					/* �G���R�[�_�f�B�X�G�[�u��Flag			*/
	MencV->AccChkWaitCnt		= 0;						/* �����x���o�J�n�J�E���g�N���A			*/
	MencV->AccChkEnable		= FALSE;					/* �����x�`�F�b�NFlag					*/
	MencV->SpdCmpEnable		= FALSE;					/* ���x�␳�����L��Flag					*/
//	MencV->DivRatFix			= FALSE;					/* �ő呬�x�A������֌W					*/
	MencV->DivInfo.DivRatFix = FALSE;						/* �ő呬�x�A������֌W					*//*<S02D>*/
	MencV->MpfPhaseOffset	= 0;						/* ���ɏ��̏����l�ݒ�		<V251>		*/
	/*----------------------------------------------------------------------------------------------*/
	MencV->PaoseqNormal = FALSE;							/* PAO�o��Seq ���񏈗��ݒ�				*/
/*--------------------------------------------------------------------------------------------------*/
/*		�@�\�L���I���F�C���N���p���X�o�͗v�����Z�b�g												*/
/*--------------------------------------------------------------------------------------------------*/
#if 0/*<S02D>*/
	if( SVFSEL_INCPOUT == 0 )
	{
		MencV->IncPulseReq = FALSE;						/* �����C���N���p���X�o�͗v��			*/
	}
#else/*<S02D>*/
	if( SVFSEL_INCPOUT == 0 )
	{
		MencV->DivInfo.IncPulseReq = FALSE;						/* �����C���N���p���X�o�͗v��			*/
	}
	else
	{
		if( MencV->DivInfo.DivOut == TRUE) /* && (Kprm.f.MotorLessTestMode==FALSE) )*/
		{
			MencV->DivInfo.IncPulseReq = TRUE;					/* �����C���N���p���X�o�͗v��			*/
		}
	}
#endif/*<S02D>*/
/*--------------------------------------------------------------------------------------------------*/
/*		��]���x�f�[�^�̏����l�ݒ�																	*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->MotSpd = 0;									/* Motor Speed [2^24/OvrSpd]			*/

	return SENCERR_SUCCESS;
}



/****************************************************************************************************/
/*																									*/
/*		���j�A���[�^�F�V���A���G���R�[�_�ϐ�����������												*/
/*																									*/
/****************************************************************************************************/
static SENC_ERROR	SencInitVariablesForLinear( MENCV *MencV, MENCPRM *MencP,
												MENC_REF_PRM *MencIPrm, ASIC_HW_IF_STR *AsicHwReg )
{
	LONG		idx;
	LONG		lwk;
	LONG		RcvPosX;
	SENC_ERROR	err;

	err = SENCERR_SUCCESS;
/*--------------------------------------------------------------------------------------------------*/
/*		����ʒu�f�[�^(����,�O��,�O�X��)�̐ݒ�														*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->MotPos  = 0;									/* Motor Position	[32bitRing]			*/
	MencV->MotCposX = 0;									/* Motor CphasePos.	[32bitRing]			*/
	/*----------------------------------------------------------------------------------------------*/
	MencV->RxPosH[1]  = MencV->RxPosH[0];				/* �O��C���N���␳�l�����ݒ�			*/
	/* LposToXpos */
	RcvPosX = MencV->MposSign * ( (MencV->RxPosL[0].Long >> LMX_MPOS_SFT) << LMX_MPOS_SFT );
	MencV->RcvPosX[0] = MencV->RcvPosX[1] = MencV->RcvPosX[2] = RcvPosX;
	MencV->MotPosX[0] = MencV->MotPosX[1] = RcvPosX;

/*--------------------------------------------------------------------------------------------------*/
/*		�����Έʒu�f�[�^(��Έʒu36Bit�f�[�^�̉���32Bit�f�[�^�����o��)							*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->AbsoEncoder == TRUE )
	{
		MencV->MotAbsPos = (MencV->RxPosH[0]<<(32-LMX_MPOS_SFT)) | ((ULONG)MencV->RxPosL[0].dw>>LMX_MPOS_SFT);
		MencV->MotAbsPos =  MencV->MposSign * MencV->MotAbsPos;
	}
	else
	{
		MencV->MotAbsPos = 0;
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�ʑ��p�����l		0 �� MencV.MotPhasX < MencP.PulseNo										*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->MotPhaX = MencV->MotAbsPos % MencV->PulseNo;	/* Motor PhasePos [PuleseNo/360deg]	*/
	if( MencV->MotPhaX < 0 )
	{
		MencV->MotPhaX += MencV->PulseNo;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		����}���`�^�[���f�[�^�̐ݒ�																*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->IniMturn = 0;
	MencV->IniIncre = RcvPosX >> LMX_MPOS_SFT;			/* XposToUpos							*/

/*--------------------------------------------------------------------------------------------------*/
/*		�ʑ����_�̏����ݒ�F�|�[���Z���T�L��														*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->withPoleSensor )							/* �|�[���Z���T�L��						*/
	{
	/*----------------------------------------------------------------------------------------------*/
	/*	�������[�^�ʑ����_���Z�F�t�u�v�M����艉�Z													*/
	/*----------------------------------------------------------------------------------------------*/
		if( MencV->Sigma3Encoder )
		{
			lwk = MencV->RxPosH[0] >> 1;
		}
		else
		{
			lwk = MencV->RxPosL[0].w.l >> 9;
		}
		idx = MencV->MotPoleInfo[0] = MencV->MotPoleInfo[1] = (UCHAR)(lwk & 0x07);
#if (FLOAT_USE==TRUE)
		lwk = (LONG)((float)PhaseTableUVW[idx] * MencV->Kinvphase );
#else
		lwk = MlibMulgainNolim( PhaseTableUVW[idx], MencV->Kinvphase );
#endif /* FLOAT_USE */

	/*----------------------------------------------------------------------------------------------*/
		if( MencV->PhaseOrderUWV == FALSE )				/* �t�u�v�ʑ���							*/
		{
			/* MotOrgX [PulseNo/360deg] */
			MencV->MotOrgX = MencV->MotPhaX - lwk;
			/* �z�[���Z���T�I�t�Z�b�g�� */
			MencV->PhaseOffset = (MencP->poleoffset<<16) / 360;
		}
		else												/* �t�v�u�ʑ���							*/
		{
			/* MotOrgX [PulseNo/360deg] */
			MencV->MotOrgX = MencV->MotPhaX + lwk;
			/* �z�[���Z���T�I�t�Z�b�g��*/
			MencV->PhaseOffset = ((360-MencP->poleoffset)<<16) / 360;
		}
		MencV->PhaseReady = TRUE;						/* �ʑ����o����Flag						*/
	/*----------------------------------------------------------------------------------------------*/
	/*	�|�[���Z���T�ُ�`�F�b�N�F�I�[���k���܂��̓I�[���g���ُ͈�									*/
	/*----------------------------------------------------------------------------------------------*/
		if( (MencV->MotPoleInfo[1]==0) || (MencV->MotPoleInfo[1]==7) )
		{
			err = SENCERR_POLESENSOR;
		}
		MencV->MpfPhaseOffset = 0;							/* ���ɏ��̏����l�ݒ�				*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�ʑ����_�̏����ݒ�F�|�[���Z���T����														*/
/*--------------------------------------------------------------------------------------------------*/
	else													/* �|�[���Z���T����						*/
	{
		MencV->MotOrgX = 0;									/* ���[�^���_						*/
		MencV->PhaseOffset = 0;								/* �z�[���Z���T�I�t�Z�b�g�� 		*/
		MencV->MotPoleInfo[0] = MencV->MotPoleInfo[1] = 0;/* �z�[���Z���T���					*/
		if( MencV->AbsoEncType == MENC_INCRE_TYPE )			/* �C���N���g�p��					*/
		{
			MencV->PhaseReady = FALSE;						/* �ʑ����o����Flag					*/
			MencV->MpfPhaseOffset = 0;						/* ���ɏ��̏����l�ݒ�				*/
		}
		else													/* �A�u�\�g�p��						*/
		{
			/* �ʑ����o����Flag */
			MencV->PhaseReady = (MencIPrm->syssw3 & 0x0020) ? TRUE : FALSE;
			MencV->MpfPhaseOffset = MencIPrm->phsofst;		/* ���ɏ��̏����l(PnE8D)�ݒ�		*/
			/*--------------------------------------------------------------------------------------*/
			/* �X�P�[����EEPROM�ƃA���v��EEPROM�̈ʑ����s��v���o									*/
			/*--------------------------------------------------------------------------------------*/
			if( MencIPrm->phsofst != MencP->phscmpM2 )			/* PnF1D��PnE8D�̔�r				*/
			{
				err = SENCERR_PHASEANGLE;
				MencV->PhaseReady = FALSE;
			}
		}
	}

/*--------------------------------------------------------------------------------------------------*/
/*		���[�^���_�`�F�b�N�p�ϐ��̏�����															*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->MotOrgChkEnd = FALSE;
	MencV->MotOrgChkCnt   = 0;
	MencV->MotOrgXchk[0]  = MencV->MotOrgXchk[1] = MencV->MotOrgX;

/* 2009.09.24 Y.Oka �����p���X�o�͋@�\��Ή��Ƃ������B */
/*--------------------------------------------------------------------------------------------------*/
/*		�����p���X�����ݒ�																			*/
/*--------------------------------------------------------------------------------------------------*/
//	MencV->DivOutRem = 0;
//	MencV->DivOutPos = MlibPfbkxremNolim( MencV->IniIncre, MencV->DivOutGain, &MencV->DivOutRem );
//	MencV->InitDivPosOffset = MencV->DivOutPos ;
#if 0 /*<S02D>*/
/*--------------------------------------------------------------------------------------------------*/
/*		�b���t���O�֘A�F�C���N���G���R�[�_															*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->AbsoEncoder == FALSE )
	{
		MencV->Cset = FALSE;								/* �b���ݒ�Flag							*/
		MencV->CposOk = FALSE;							/* �b���ʒu�ݒ芮��Flag					*/
	/*----------------------------------------------------------------------------------------------*/
	/*	�����o�͏����ݒ�																			*/
	/*----------------------------------------------------------------------------------------------*/
		if( MencV->DivOut == TRUE )
		{
			/* �p���X�ϊ��b���}�X�N�n�m				*/
//			KPI_PCMASK_ON( );
			AsicHwReg->AREG_WDT1SET |= BIT8;
			MencV->DivCMaskStatus = DIV_CMASK_ON;

			/* �ʏ�b���o�͑I��(INGRAM�����o�͖��Ή�) */
//			KPI_DIVSET_DCNORMAL( );
			MencV->DivModeStatus = DIV_MODE_NORMAL;

			AsicHwReg->AREG_POSET0 = (USHORT)MencV->DivOutPos;
			AsicHwReg->AREG_POTEST = (USHORT)MencV->DivOutPos;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�b���t���O�֘A�F��Βl�G���R�[�_ (�A�u�\���C���N���Ƃ��Ďg�p)								*/
/*--------------------------------------------------------------------------------------------------*/
	else if( MencV->AbsoEncType == MENC_INCRE_TYPE )
	{
		MencV->Cset = TRUE;								/* �b���ݒ�Flag							*/
		MencV->CposOk = FALSE;							/* �b���ʒu�ݒ芮��Flag					*/
	/*----------------------------------------------------------------------------------------------*/
	/*	�����o�͏����ݒ�																			*/
	/*----------------------------------------------------------------------------------------------*/
		if( MencV->DivOut == TRUE )
		{
			/* �p���X�ϊ��b���}�X�N�n�m				*/
//			KPI_PCMASK_ON( );
			AsicHwReg->AREG_WDT1SET |= BIT8;
			MencV->DivCMaskStatus = DIV_CMASK_ON;

			/* �ʏ�b���o�͑I��						*/
//			KPI_DIVSET_DCNORMAL( );
			MencV->DivModeStatus = DIV_MODE_NORMAL;

			AsicHwReg->AREG_POSET0 = (USHORT)MencV->DivOutPos;
			AsicHwReg->AREG_POTEST = (USHORT)MencV->DivOutPos;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�b���t���O�֘A�F��Βl�G���R�[�_ (�A�u�\���A�u�\�Ƃ��Ďg�p)									*/
/*--------------------------------------------------------------------------------------------------*/
	else
	{
		MencV->Cset = TRUE;								/* �b���ݒ�Flag							*/
		MencV->CposOk = FALSE;							/* �b���ʒu�ݒ芮��Flag					*/
	/*----------------------------------------------------------------------------------------------*/
	/*	�����o�͏����ݒ�																			*/
	/*----------------------------------------------------------------------------------------------*/
		if( MencV->DivOut == TRUE )
		{
			/* �p���X�ϊ��b���}�X�N�n�m				*/
//			KPI_PCMASK_ON( );
			AsicHwReg->AREG_WDT1SET |= BIT8;
			MencV->DivCMaskStatus = DIV_CMASK_ON;

			/* �ʏ�b���o�͑I��(INGRAM�����o�͖��Ή�) */
//			KPI_DIVSET_DCNORMAL( );
			MencV->DivModeStatus = DIV_MODE_NORMAL;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�G���R�[�_���f�B�ݒ� : �G���R�[�_�������E�p�����[�^�������̌�Őݒ�							*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->EncConnect == FALSE )						/* �G���R�[�_���ڑ���					*/
	{
		MencV->SenReady = FALSE;							/* SEN�M����������Flag					*/
		MencV->IncPulseReq = FALSE;						/* �����C���N���p���X�o�͗v��			*/
	}
	/*----------------------------------------------------------------------------------------------*/
	else if( MencV->DivOut == FALSE )					/* ���[�^�G���R�[�_�����o�́F��I����	*/
	{
		MencV->SenReady = TRUE;							/* SEN�M����������Flag					*/
		MencV->IncPulseReq = FALSE;						/* �����C���N���p���X�o�͗v��			*/
	}
	/*----------------------------------------------------------------------------------------------*/
	else if( MencV->AbsoEncType )						/* �A�u�\�G���R�[�_��					*/
	{
//		MencV->SenReady = FALSE;							/* SEN�M����������Flag					*/
		MencV->SenReady = TRUE;								/* SEN�M����������Flag					*/
		MencV->IncPulseReq = TRUE;						/* �����C���N���p���X�o�͗v��			*/
	}
	/*----------------------------------------------------------------------------------------------*/
	else													/* �C���N���G���R�[�_��					*/
	{
		MencV->SenReady = TRUE;							/* SEN�M����������Flag					*/
		MencV->IncPulseReq = FALSE;						/* �����C���N���p���X�o�͗v��			*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�G���R�[�_�t���O�ݒ� : ���ʏ���																*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->MotCphPass		= FALSE;					/* �b���ʉ�Flag							*/
	MencV->IncreCposLatch	= FALSE;					/* �b���ʒu���b�`�M��(ScanA-->ScanB)	*/
//	MencV->EncDisable		= FALSE;					/* �G���R�[�_�f�B�X�G�[�u��Flag			*/
	MencV->AccChkEnable		= FALSE;					/* �����x�`�F�b�NFlag					*/
	MencV->SpdCmpEnable		= FALSE;					/* ���x�␳�����L��Flag					*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->PaoseqNormal		= FALSE;					/* PAO�o��Seq ���񏈗��ݒ�				*/

/* 2009.09.24 Y.Oka �����C���N���p���X�o�͂Ȃ� */
/*--------------------------------------------------------------------------------------------------*/
/*		�@�\�L���I���F�C���N���p���X�o�͗v�����Z�b�g												*/
/*--------------------------------------------------------------------------------------------------*/
//	if( SVFSEL_INCPOUT == 0 )
//	{
//		MencV->IncPulseReq = FALSE;						/* �����C���N���p���X�o�͗v��			*/
//	}
#else/*<S02D>*/
/* <S21B>:Start */
	MencV->DivInfo.DivOutRem = 0;
	MencV->DivInfo.DivOutPos = MlibPfbkxremNolim( MencV->IniIncre, MencV->DivInfo.DivOutGain, &MencV->DivInfo.DivOutRem );
	MencV->DivInfo.InitDivPosOffset = MencV->DivInfo.DivOutPos ;
/* <S21B>:end */
/*--------------------------------------------------------------------------------------------------*/
/*		�b���t���O�֘A�F�C���N���G���R�[�_															*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->AbsoEncoder == FALSE )
	{
		MencV->Cset = FALSE;								/* �b���ݒ�Flag							*/
		MencV->CposOk = FALSE;								/* �b���ʒu�ݒ芮��Flag					*/
	/*----------------------------------------------------------------------------------------------*/
	/*	�����o�͏����ݒ�																			*/
	/*----------------------------------------------------------------------------------------------*/
		if( MencV->DivInfo.DivOut == TRUE )
		{
			/* �p���X�ϊ��b���}�X�N�n�m				*/
			AsicHwReg->AREG_WDT1SET |= BIT8;
			MencV->DivInfo.DivCMaskStatus = DIV_CMASK_ON;

			/* �ʏ�b���o�͑I��(INGRAM�����o�͖��Ή�) */
			MencV->DivInfo.DivModeStatus = DIV_MODE_NORMAL;
			AsicHwReg->AREG_POSET0 = (ULONG)MencV->DivInfo.DivOutPos;
			AsicHwReg->AREG_POTEST = (ULONG)MencV->DivInfo.DivOutPos;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�b���t���O�֘A�F��Βl�G���R�[�_ (�A�u�\���C���N���Ƃ��Ďg�p)								*/
/*--------------------------------------------------------------------------------------------------*/
	else if( MencV->AbsoEncType == MENC_INCRE_TYPE )
	{
		MencV->Cset = TRUE;								/* �b���ݒ�Flag							*/
		MencV->CposOk = FALSE;							/* �b���ʒu�ݒ芮��Flag					*/
	/*----------------------------------------------------------------------------------------------*/
	/*	�����o�͏����ݒ�																			*/
	/*----------------------------------------------------------------------------------------------*/
		if( MencV->DivInfo.DivOut == TRUE )
		{
			/* �p���X�ϊ��b���}�X�N�n�m				*/
			AsicHwReg->AREG_WDT1SET |= BIT8;
			MencV->DivInfo.DivCMaskStatus = DIV_CMASK_ON;

			/* �ʏ�b���o�͑I��						*/
			MencV->DivInfo.DivModeStatus = DIV_MODE_NORMAL;
			AsicHwReg->AREG_POSET0 = (ULONG)MencV->DivInfo.DivOutPos;
			AsicHwReg->AREG_POTEST = (ULONG)MencV->DivInfo.DivOutPos;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�b���t���O�֘A�F��Βl�G���R�[�_ (�A�u�\���A�u�\�Ƃ��Ďg�p)									*/
/*--------------------------------------------------------------------------------------------------*/
	else
	{
		MencV->Cset = TRUE;								/* �b���ݒ�Flag							*/
		MencV->CposOk = FALSE;							/* �b���ʒu�ݒ芮��Flag					*/
	/*----------------------------------------------------------------------------------------------*/
	/*	�����o�͏����ݒ�																			*/
	/*----------------------------------------------------------------------------------------------*/
		if( MencV->DivInfo.DivOut == TRUE )
		{
			/* �p���X�ϊ��b���}�X�N�n�m				*/
			AsicHwReg->AREG_WDT1SET |= BIT8;
			MencV->DivInfo.DivCMaskStatus = DIV_CMASK_ON;

			/* �ʏ�b���o�͑I��(INGRAM�����o�͖��Ή�) */
			MencV->DivInfo.DivModeStatus = DIV_MODE_NORMAL;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�G���R�[�_���f�B�ݒ� : �G���R�[�_�������E�p�����[�^�������̌�Őݒ�							*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->EncConnect == FALSE )						/* �G���R�[�_���ڑ���					*/
	{
		MencV->SenReady = FALSE;							/* SEN�M����������Flag					*/
		MencV->DivInfo.IncPulseReq = FALSE;					/* �����C���N���p���X�o�͗v��			*/
	}
	/*----------------------------------------------------------------------------------------------*/
	else if( MencV->DivInfo.DivOut == FALSE )				/* ���[�^�G���R�[�_�����o�́F��I����	*/
	{
		MencV->SenReady = TRUE;								/* SEN�M����������Flag					*/
		MencV->DivInfo.IncPulseReq = FALSE;					/* �����C���N���p���X�o�͗v��			*/
	}
	/*----------------------------------------------------------------------------------------------*/
	else if( MencV->AbsoEncType )							/* �A�u�\�G���R�[�_��					*/
	{
		MencV->SenReady = TRUE;								/* SEN�M����������Flag					*/
		MencV->DivInfo.IncPulseReq = TRUE;					/* �����C���N���p���X�o�͗v��			*/
	}
	/*----------------------------------------------------------------------------------------------*/
	else													/* �C���N���G���R�[�_��					*/
	{
		MencV->SenReady = TRUE;								/* SEN�M����������Flag					*/
		MencV->DivInfo.IncPulseReq = FALSE;					/* �����C���N���p���X�o�͗v��			*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�G���R�[�_�t���O�ݒ� : ���ʏ���																*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->MotCphPass		= FALSE;						/* �b���ʉ�Flag							*/
	MencV->IncreCposLatch	= FALSE;						/* �b���ʒu���b�`�M��(ScanA-->ScanB)	*/
	MencV->AccChkEnable		= FALSE;						/* �����x�`�F�b�NFlag					*/
	MencV->SpdCmpEnable		= FALSE;						/* ���x�␳�����L��Flag					*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->PaoseqNormal		= FALSE;						/* PAO�o��Seq ���񏈗��ݒ�				*/

	if( SVFSEL_INCPOUT == 0 )
	{
		MencV->DivInfo.IncPulseReq = FALSE;					/* �����C���N���p���X�o�͗v��			*/
	}
	else
	{
		MencV->DivInfo.IncPulseReq = TRUE;					/* �����C���N���p���X�o�͗v��			*/
	}
#endif/*<S02D>*/

/*--------------------------------------------------------------------------------------------------*/
/*		��]���x�f�[�^�̏����l�ݒ�																	*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->MotSpd = 0;									/* Motor Speed [2^24/OvrSpd]			*/

	return err;
}



/****************************************************************************************************/
/*																									*/
/*		�V���A���G���R�[�_�̕ϐ�����������															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �C�j�V���������ɂ����āA�V���A���G���R�[�_�̏���ʒu�f�[�^��ǂݍ��݁A					*/
/*			�ʒu�f�[�^�A�O��ʒu�̐ݒ���s���B														*/
/*			�܂��A�G���R�[�_�^�C�v�ɂ���Ăb���o�́A�����p���X���o���Ȃ��ݒ������B				*/
/*																									*/
/****************************************************************************************************/
SENC_ERROR	SencInitMencVariables(MENCV *MencV, MENCPRM *MencP, MENC_REF_PRM *MencInPrm )
{
	SENC_ERROR		err, rc_err;

	/*----------------------------------------------------------------------------------------------*/
	/*		�V���A���G���R�[�_����̃f�[�^�ǂݍ���													*/
	/*----------------------------------------------------------------------------------------------*/
	rc_err = SencReadInitialP1Data(MencV);

	/*----------------------------------------------------------------------------------------------*/
	/*		�V���A���G���R�[�_�ϐ�����������, ������Έʒu([pulse]��[�w�ߒP��])�v�Z					*/
	/*----------------------------------------------------------------------------------------------*/
	if( MencV->AxisMotType == MOTTYPE_LINEAR )
	{ /* ���j�A�^ */

		/* �����p�����[�^�v�Z */
		SencInitParamCalcForLinear( MencV, MencP, MencInPrm );

		/* �ϐ����������� */
		err = SencInitVariablesForLinear( MencV, MencP, MencInPrm, MencV->EncIf.SencSetReg );/* <S04D> �����ύX */
		if(rc_err == SENCERR_SUCCESS)
		{	rc_err = err;}
	}
	else
	{ /* ��]�^ */

		/* �����p�����[�^�v�Z */
		SencInitParamCalcForRotary( MencV, MencP, MencInPrm );

		/* �ϐ����������� */
		err = SencInitVariablesForRotary( MencV, MencP, MencV->EncIf.SencSetReg );/* <S04D> �����ύX */
		if(rc_err == SENCERR_SUCCESS)
		{	rc_err = err;}
	}

	return rc_err;
}


#pragma arm section code = "CALL_SCANA" /* <S1D2> */
//#if	(SVFSEL_DIVOUT_USE == TRUE)  /* <S02D> *//* <S09F> */
/****************************************************************************************************/
/*																									*/
/*		�����C���N���p���X�o�͏���																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �V���A���G���R�[�_�o�`�n���V���A���o�̓V�[�P���X(SencOutputPaoSerial())					*/
/*			����̎w�߂ɏ]���A�A�u�\�G���R�[�_�ڑ��ɁA�r�d�m�M�������オ�莞�ɏo�͂���				*/
/*			�}���`�^�[���f�[�^�擾�Ə����C���N���p���X�o�͏������s���B								*/
/*																									*/
/*																									*/
/*	�� �� : �T�[�{�I�t���̏����ł���̂ŁA�������ԓI�ɂ͑����]�T������B							*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#define MAXPULSEOUT		170
/*--------------------------------------------------------------------------------------------------*/
void	 SencOutIniIncPulse( MENCV  *MencV, ASIC_HW_IF_STR  *pAsicHwReg )
{
LONG	lwk;
LONG	MtnCur;
LONG	MtnSub;
LONG	MtnSepos;
#if	(SVFSEL_DIVOUT_USE == TRUE)       /* <S09F> */
/*--------------------------------------------------------------------------------------------------*/
/*		�ʐM�ُ�`�F�b�N																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( MencV->SeRxErr == TRUE )
		{
			return;											/* �ʐM�ُ펞�́A�������Ȃ�				*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�}���`�^�[���f�[�^�擾����																	*/
/*--------------------------------------------------------------------------------------------------*/
		if( MencV->DivInfo.PaoSeqMcmd == KPX_MTLATCH_REQUEST )	/* ���b�`�v���R�}���h�`�F�b�N		*/
		{
			MencV->DivInfo.MturnLatch = SencGetMturnData( MencV,
			                                      MencV->DivInfo.Limmlt, 
			                                      MencV->DivInfo.Bit_dp,
												  (USHORT)MencV->RxPosH[0],
												  MencV->RxPosL[0].dw );
			MencV->DivInfo.IncPulseOut = 0;				/* �C���N���p���X�o�̓��Z�b�g			*/
			pAsicHwReg->AREG_WDT1SET &= ~BIT8;				/* �p���X�ϊ�C���}�X�NOFF�ݒ�			*/
			MencV->DivInfo.DivCMaskStatus  = DIV_CMASK_OFF ;
			MencV->DivInfo.PaoSeqMcmd = KPX_MTLATCH_FINISH;/* ���� ==> SencOutputPaoSerial()		*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�����C���N���p���X�o�͏���																	*/
/*--------------------------------------------------------------------------------------------------*/
		else if( MencV->DivInfo.PaoSeqMcmd == KPX_IPLSOUT_REQUEST )/* �p���X�o�̓R�}���h�`�F�b�N	*/
		{
		/*------------------------------------------------------------------------------------------*/
		/*		�����C���N���p���X�o�̓f�[�^���Z													*/
		/*------------------------------------------------------------------------------------------*/
			MtnCur = SencGetMturnData( MencV,
                                       MencV->DivInfo.Limmlt, 
                                       MencV->DivInfo.Bit_dp,
									   (USHORT)MencV->RxPosH[0],
									   MencV->RxPosL[0].dw );

			MtnSub = SencCheckMturnLimit( MtnCur, MencV );
			MtnSepos = ((ULONG)MencV->RcvPosX[0]>>MencV->DivInfo.DivOutSft) + (MtnSub<<(32-MencV->DivInfo.DivOutSft));
		/*------------------------------------------------------------------------------------------*/
		/*		�����C���N���p���X�o�̓f�[�^�`�F�b�N���o�̓f�[�^�ݒ�								*/
		/*------------------------------------------------------------------------------------------*/
			lwk = MtnSepos - MencV->DivInfo.IncPulseOut;
			if( lwk > MAXPULSEOUT )							/* �p���X�o�͏��						*/
			{
				MencV->DivInfo.IncPulseOut += MAXPULSEOUT;	/* �p���X�o�̓f�[�^�ݒ�					*/
			}
			else if( lwk < -MAXPULSEOUT )					/* �p���X�o�͉���						*/
			{
				MencV->DivInfo.IncPulseOut -= MAXPULSEOUT;	/* �p���X�o�̓f�[�^�ݒ�					*/
			}
			else											/* �ŏI�p���X�o��						*/
			{
				MencV->DivInfo.IncPulseOut = MtnSepos;		/* �p���X�o�̓f�[�^�ݒ�					*/
				MencV->DivInfo.PaoSeqMcmd = KPX_IPLSOUT_FINISH;/* ���� ==> SencOutputPaoSerial		*/
				MencV->DivInfo.IncPulseReq = FALSE;			/* �����C���N���p���X�o�͊���			*/
			}
		/*------------------------------------------------------------------------------------------*/
			pAsicHwReg->AREG_POSET0	= MencV->DivInfo.IncPulseOut;	/* �p���X�o�͉�H������			*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�R�}���h����																				*/
/*--------------------------------------------------------------------------------------------------*/
		else
		{
			;												/* �R�}���h�҂�							*/
		}
/*--------------------------------------------------------------------------------------------------*/
#endif                           /* <S09F> */
		return;

}
#pragma arm section code

#pragma arm section code = "CALL_SCANA"
/****************************************************************************************************/
/*																									*/
/*		���j�A���[�^(�t�B�[�h�o�b�N�I�v�V����)�F�����C���N���p���X�o�͏���							*/
/*																									*/
/****************************************************************************************************//* <S209> */
void	LmxOutSencIniIncPulse( MENCV  *MencV, ASIC_HW_IF_STR  *pAsicHwReg, UCHAR Sftx )
{
LONG	lwk;
LONG	MtnCur;
LONG	MtnSub;
LONG	MtnRem;
LONG	MtnSepos;
SHORT	MturnLatchCur;

#if	(SVFSEL_DIVOUT_USE == TRUE)
/*--------------------------------------------------------------------------------------------------*/
/*		�ʐM�ُ�`�F�b�N																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( MencV->SeRxErr == TRUE )
		{
			return;											/* �ʐM�ُ펞�́A�������Ȃ�				*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�}���`�^�[���f�[�^�擾����																	*/
/*--------------------------------------------------------------------------------------------------*/
		if( MencV->DivInfo.PaoSeqMcmd == KPX_MTLATCH_REQUEST )	/* ���b�`�v���R�}���h�`�F�b�N		*/
		{
		/*------------------------------------------------------------------------------------------*/
		/*		��Έʒu36Bit�f�[�^�̏��32Bit�f�[�^�����o���A�}���`�^�[���̕����l���v�Z����		*/
		/*------------------------------------------------------------------------------------------*/
			MtnCur    = ((USHORT)MencV->RxPosH[0] << 16 ) | ((USHORT)MencV->RxPosL[0].w.h );
			MtnRem    = 0;
			MtnCur    = MlibPfbkxremNolim( MtnCur, MencV->DivInfo.DivOutGain, &MtnRem );
			MtnSepos = (MencV->RxPosH[0]<<(32-Sftx)) | ((ULONG)MencV->RxPosL[0].dw>>Sftx);
			MencV->DivInfo.DivOutRem = 0;														
			MtnSepos =  MlibPfbkxremNolim( MtnSepos, MencV->DivInfo.DivOutGain, &MencV->DivInfo.DivOutRem );
			if( ( (MtnCur>>15) & 0x00000001 ) ^ ( (MtnSepos>>19) & 0x00000001 ) )
			{
				if( (MtnCur>>15) & 0x00000001 )
				{
					MtnCur = MtnCur + 1;
				}
				else if( (MtnSepos>>19) & 0x00000001 )
				{
					MtnCur = MtnCur - 1;
				}
			}
			MencV->DivInfo.MturnLatch = (SHORT)( MtnCur >> 16);
		/*------------------------------------------------------------------------------------------*/
		/*		�����p���X���Z�b�g����																*/
		/*------------------------------------------------------------------------------------------*/
			MencV->DivInfo.DivOutPos   = 0;					/* �����o�̓p���X���Z�b�g				*/
			MencV->DivInfo.IncPulseOut = 0;					/* �C���N���p���X�o�̓��Z�b�g			*/
			MencV->DivInfo.PaoSeqMcmd  = KPX_MTLATCH_FINISH;/* ���� ==> LpxOutputSencPaoSerial()	*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�����C���N���p���X�o�͏���																	*/
/*--------------------------------------------------------------------------------------------------*/
		else if( MencV->DivInfo.PaoSeqMcmd == KPX_IPLSOUT_REQUEST )	/* �p���X�o�̓R�}���h�`�F�b�N	*/
		{
		/*------------------------------------------------------------------------------------------*/
		/*		�}���`�^�[���f�[�^�̕ω������Z														*/
		/*		��Έʒu36Bit�f�[�^�̉���32Bit�f�[�^�����o���A�����C���N���̕����l���v�Z����		*/
		/*------------------------------------------------------------------------------------------*/
			MtnCur        = ((USHORT)MencV->RxPosH[0] << 16 ) | ((USHORT)MencV->RxPosL[0].w.h );
			MtnRem        = 0;
			MtnCur        = MlibPfbkxremNolim( MtnCur, MencV->DivInfo.DivOutGain, &MtnRem );
			MtnSepos = (MencV->RxPosH[0]<<(32-Sftx)) | ((ULONG)MencV->RxPosL[0].dw>>Sftx);
			MencV->DivInfo.DivOutRem   = 0;						/* �����o�̓p���X�]�胊�Z�b�g		*/
			MtnSepos =  MlibPfbkxremNolim( MtnSepos, MencV->DivInfo.DivOutGain, &MencV->DivInfo.DivOutRem );
			if( ( (MtnCur>>15) & 0x00000001 ) ^ ( (MtnSepos>>19) & 0x00000001 ) )
			{
				if( (MtnCur>>15) & 0x00000001 )
				{
					MtnCur = MtnCur + 1;
				}
				else if( (MtnSepos>>19) & 0x00000001 )
				{
					MtnCur = MtnCur - 1;
				}
			}
			MturnLatchCur = (SHORT)( MtnCur >> 16);
			MtnSub = (USHORT)MturnLatchCur - (USHORT)MencV->DivInfo.MturnLatch;
			MtnSepos = (MtnSepos & 0x000FFFFF );
			if( MencV->RevAsmMotor ^ MencV->DivInfo.RvsDir )/* ���]�ڑ� XOR �t��]���[�h			*/
			{
				MtnSepos = MtnSepos | 0xFFF00000;			/* �������]����							*/
				MtnSepos = (ULONG)MtnSepos - ( MtnSub << (32-Sftx) );
			}
			else
			{
				MtnSepos = (ULONG)MtnSepos + ( MtnSub << (32-Sftx) );
			}
		/*------------------------------------------------------------------------------------------*/
		/*		�����C���N���p���X�o�̓f�[�^�`�F�b�N���o�̓f�[�^�ݒ�								*/
		/*------------------------------------------------------------------------------------------*/
			lwk = MtnSepos - MencV->DivInfo.IncPulseOut;
			if( lwk > MAXPULSEOUT )							/* �p���X�o�͏��						*/
			{
				MencV->DivInfo.IncPulseOut += MAXPULSEOUT;	/* �p���X�o�̓f�[�^�ݒ�					*/
			}
			else if( lwk < -MAXPULSEOUT )					/* �p���X�o�͉���						*/
			{
				MencV->DivInfo.IncPulseOut -= MAXPULSEOUT;	/* �p���X�o�̓f�[�^�ݒ�					*/
			}
			else											/* �ŏI�p���X�o��						*/
			{
				MencV->DivInfo.IncPulseOut = MtnSepos;		/* �p���X�o�̓f�[�^�ݒ�					*/
				MencV->DivInfo.PaoSeqMcmd = KPX_IPLSOUT_FINISH;/* ���� ==> SencOutputPaoSerial		*/
				MencV->DivInfo.IncPulseReq = FALSE;			/* �����C���N���p���X�o�͊���			*/
			}
			MencV->DivInfo.DivOutPos = MencV->DivInfo.IncPulseOut;	/* �����o�̓p���X�X�V			*/
		/*------------------------------------------------------------------------------------------*/
			pAsicHwReg->AREG_POSET0	= MencV->DivInfo.IncPulseOut;	/* �C���N���p���X�o�͐ݒ�		*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�R�}���h����																				*/
/*--------------------------------------------------------------------------------------------------*/
		else
		{
			;												/* �R�}���h�҂�							*/
		}
/*--------------------------------------------------------------------------------------------------*/
#endif 
		return;
}
#pragma arm section code

#pragma arm section code = "CALL_SCANA" /* <S1D2> */
#if	(SVFSEL_DIVOUT_USE == TRUE)       /* <S09F> */
/****************************************************************************************************/
/*																									*/
/*		�}���`�^�[���f�[�^�̕ω������Z		<S02D>													*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �p���X�o�͎��̈ʒu�f�[�^�Ƒ���]���b�`���̈ʒu�f�[�^����ω��������߂�B				*/
/*			�p���X�o�̓f�[�^�ɂ��ẮA�}���`�^�[�����~�b�g�l�𒴂��Ă��邩�ǂ�����				*/
/*			�`�F�b�N���s���A�p���X�o�̓f�[�^�ɔ��f����B											*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static LONG	 SencCheckMturnLimit( LONG MtnCur, MENCV *MencV )
{
LONG	MtnSub;
LONG	limmlt = MencV->DivInfo.Limmlt;

/*--------------------------------------------------------------------------------------------------*/
/*		�}���`�^�[���ω��ʉ��Z																		*/
/*--------------------------------------------------------------------------------------------------*/
		MtnSub = (USHORT)MtnCur - (USHORT)MencV->DivInfo.MturnLatch;
		if( MtnSub < 0 )									/* ���̏ꍇ								*/
		{
			if( MtnSub < -((limmlt>>1) + 1) )				/* �}���`�^�[�����~�b�g�𒴂����ꍇ,	*/
			{												/* �}���`�^�[�����~�b�g�𒴂������� 	*/
				MtnSub += (limmlt + 1);						/* ����]�ʂ����Z						*/
			}
		}
		else												/* ���̏ꍇ								*/
		{
			if ( MtnSub > (limmlt>>1) )						/* �}���`�^�[�����~�b�g�𒴂����ꍇ, 	*/
			{												/* �}���`�^�[�����~�b�g�𒴂�������		*/
				MtnSub -= (limmlt + 1);						/* ����]�ʂ����Z						*/
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		return ( MtnSub );
}
#pragma arm section code


/****************************************************************************************************/
/*																									*/
/*		�V���A���G���R�[�_�o�`�n���V���A���o��			<S02D>										*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �A�u�\�G���R�[�_�g�p���A�r�d�m�M���ɂ��}���`�^�[���f�[�^�y�уA���[����				*/
/*			�o�`�n����̃V���A���o�͏������s���B													*/
/*																									*/
/*			1) �o�̓t�H�[�}�b�g : �Vbit �����p���e�B												*/
/*				 �`�r�h�b�̂s�w�c�́A�p���e�B�̕t���y�тV�r�b�g�f�[�^�����M�̋@�\��					*/
/*				 �Ȃ��̂ŁA�p���e�B���\�t�g�I�ɕt�������W�r�b�g�f�[�^�Ƃ��đ��M����					*/
/*																									*/
/*			2) �}���`�^�[�� : �o�}����������[CR]	( �}@@@@@ : -32768 - +32768 )					*/
/*																									*/
/*			3) �A���[��		: �`�k�l�����D[CR]		( @@ : A.@@ )									*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
void	SencOutputPaoSerial( MENCV *MencV, BPRMDAT *Bprm, ASIC_HW_IF_STR *pAsicHwReg,
							     BOOL DbBeReq, BOOL SenReq, USHORT AlmCode, BOOL AlmFlag )
{

	USHORT	uswk;
	ASIC_HW_IF_STR *pAsicHwReg2;/*<S0C2>*/
	pAsicHwReg2 = (ASIC_HW_IF_STR*)((UCHAR*)pAsicHwReg + 0x1000);/*2-axis pointing address <S0C2>*/
/*--------------------------------------------------------------------------------------------------*/
/*		�@�\�L���`�F�b�N																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( SVFSEL_PAOPOUT == 0 )		{ return;}				/* Mask Function					*/
/*--------------------------------------------------------------------------------------------------*/
/*		�T�[�{�I�����V�[�P���X������																*/
/*--------------------------------------------------------------------------------------------------*/
		if( DbBeReq == TRUE )
		{
			MencV->DivInfo.PaoSeqStep = 0;
			return;
		}

/*--------------------------------------------------------------------------------------------------*/
/*		�o�`�n�V���A���o�̓V�[�P���X																*/
/*--------------------------------------------------------------------------------------------------*/
		switch( MencV->DivInfo.PaoSeqStep )
		{
	/*----------------------------------------------------------------------------------------------*/
		case  0: /* Initial Step																	*/
	/*----------------------------------------------------------------------------------------------*/
				MencV->DivInfo.PaoSeqWait = 0;
				MencV->DivInfo.PaoSeqMcmd = KPX_PAOSEQ_NOCMD;
				if( MencV->PaoseqNormal == TRUE )				/* �d�������㏉�񓮍�-				*/
				{
					MencV->DivInfo.PaoSeqStep = (SenReq == FALSE)? 1 : 20;
				}
				else
				{
					MencV->DivInfo.PaoSeqStep = 1;				/* �d�������㏉���SEN Lo->Hi��҂�	*/
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case  1: /* Wait SEN Lo --> Hi (Output Multi-turn Data)										*/
	/*----------------------------------------------------------------------------------------------*/
				MencV->PaoseqNormal = TRUE;							/* ���񓮍샂�[�h����			*/
				MencV->SenReady = FALSE;
				MencV->DivInfo.PaoSeqMcmd = KPX_PAOSEQ_NOCMD;
				MencV->DivInfo.IncPulseReq = TRUE;
				if( SenReq == TRUE )
				{
					/*------------------------------------------------------------------------------*/
					/*		�c�`�o�̓��[�h�F�s�w�c�o��												*/
					/*------------------------------------------------------------------------------*/
					/*		Bit13 :	DASEL(�����o��DA�[�q�I��)	:  0 : DA,  1: TXD_1				*/
					/*------------------------------------------------------------------------------*/
					pAsicHwReg->AREG_PFNCSET  |= BIT13;			/* �c�`�o�̓��[�h�F�s�w�c�o��		*/
					pAsicHwReg2->AREG_PFNCSET &= ~BIT13;		/*PSO NO way <S0C2>	    */
					/*------------------------------------------------------------------------------*/
					/*	��]�^�̏ꍇ																*/
					/*------------------------------------------------------------------------------*/
					if( MencV->AxisMotType == MOTTYPE_ROTARY )
					{
						pAsicHwReg->AREG_WDT1SET &= ~BIT8;		/* �p���X�ϊ�C���}�X�NOFF�ݒ�		*/
						MencV->DivInfo.DivCMaskStatus  = DIV_CMASK_OFF ;

						pAsicHwReg->AREG_POSET0	= 0x00000000;	/* �p���X�ϊ�������0000h������۰��	*/
						pAsicHwReg->AREG_POTEST	= 0x00000000;	/* �p���X�ϊ��ʒu��0000h�ɐݒ�		*/
					}
					/*------------------------------------------------------------------------------*/
					/*	���j�A�^�̏ꍇ�FJL-076�����@�\�s�g�p(�X���[���[�h)�̏ꍇ					*/
					/*------------------------------------------------------------------------------*/
					else if( MencV->AxisMotType == MOTTYPE_LINEAR )
					{
						pAsicHwReg->AREG_POSET0	= 0xFFFFFFFF;	/* �p���X�o�͉�H������				*/
						pAsicHwReg->AREG_POTEST	= 0xFFFFFFFF;	/* �p���X�o�͉�H������				*/
						pAsicHwReg->AREG_DINCSET  |= BIT10;		/* �����펞�N���A�r�b�g���Ă�		*/
						pAsicHwReg->AREG_WDT1SET |= BIT8;		/* �p���X�ϊ��b���}�X�N�n�m			*/
						MencV->DivInfo.DivCMaskStatus = DIV_CMASK_ON;	/* �p���X�ϊ��b���}�X�N�n�m	*/
					}
					MencV->DivInfo.PaoSeqStep = 2;
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case  2: /* Wait 120ms																		*/
	/*----------------------------------------------------------------------------------------------*/
				/*----------------------------------------------------------------------------------*/
				/*	��]�^�̏ꍇ�F																	*/
				/*----------------------------------------------------------------------------------*/
				if( MencV->AxisMotType == MOTTYPE_ROTARY )
				{
					/*------------------------------------------------------------------------------*/
					/*	POSET0��0000h�Z�b�g����2ms���C���ē����Z�b�g								*/
					/*------------------------------------------------------------------------------*/
					if( MencV->DivInfo.PaoSeqWait == 0 )
					{
						pAsicHwReg->AREG_DINCSET	|= BIT5;	/* �b���ē�����						*/
						pAsicHwReg->AREG_POSET0	= 0xFFFFFFFF;	/* �p���X�o�͉�H������				*/
					}
					if( ++MencV->DivInfo.PaoSeqWait > KPI_SCANC_MS( 120 ) )
					{
						pAsicHwReg->AREG_DINCSET	&= ~(BIT10);/* �����펞�N���A���Ƃ�				*/
						pAsicHwReg->AREG_DINCSET	&= ~(BIT5); /* �b���ē�������					*/
						MencV->DivInfo.PaoSeqWait = 0;
						MencV->DivInfo.PaoSeqStep = 3;
					}
				}
				/*----------------------------------------------------------------------------------*/
				/*	���j�A�^�̏ꍇ�F�����@�\�s�g�p(�X���[���[�h)�̏ꍇ								*/
				/*----------------------------------------------------------------------------------*/
				else if( MencV->AxisMotType == MOTTYPE_LINEAR )
				{
					if( ++MencV->DivInfo.PaoSeqWait > KPI_SCANC_MS( 120 ) )
					{
						pAsicHwReg->AREG_DINCSET	|= BIT5;	/* �b���ē�����						*/
						pAsicHwReg->AREG_DINCSET	&= ~(BIT10);/* �����펞�N���A���Ƃ�				*/
						pAsicHwReg->AREG_DINCSET	&= ~(BIT5); /* �b���ē�������					*/
						MencV->DivInfo.PaoSeqWait = 0;
						MencV->DivInfo.PaoSeqStep = 3;
					}
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case  3: /* Multiturn Latch Request															*/
	/*----------------------------------------------------------------------------------------------*/
				MencV->DivInfo.PaoSeqMcmd = KPX_MTLATCH_REQUEST;	/* MultiturnLatch�w�� ==> ScanA	*/
				MencV->DivInfo.PaoSeqStep = 4;
				break;
	/*----------------------------------------------------------------------------------------------*/
		case  4: /* Wait Multiturn Latch															*/
	/*----------------------------------------------------------------------------------------------*/
				if( MencV->DivInfo.PaoSeqMcmd == KPX_MTLATCH_FINISH )	/* MultiturnLatch�����H		*/
				{
					MencV->DivInfo.PaoSeqData = MencV->DivInfo.MturnLatch;
					MencV->DivInfo.PaoSeqStep = 5;
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case  5: /* Output 'P'																		*/
	/*----------------------------------------------------------------------------------------------*/
				/*----------------------------------------------------------------------------------*/
				/*	UARTXD:�ėp�񓯊��|�[�g���M�f�[�^�F�ėp�񓯊��|�[�g���M�f�[�^�r�b�g�V-0   	    */
				/*----------------------------------------------------------------------------------*/
				//pAsicHwReg->AREG_UARTXD  = 'P';
				pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_P;		/* <S14F> */
				/*----------------------------------------------------------------------------------*/
				MencV->DivInfo.PaoSeqStep = 6;
				break;
	/*----------------------------------------------------------------------------------------------*/
		case  6: /* Output '+' or '-'																*/
	/*----------------------------------------------------------------------------------------------*/
				/*----------------------------------------------------------------------------------*/
				/*	��]�^�̏ꍇ																	*/
				/*----------------------------------------------------------------------------------*/
				if( MencV->AxisMotType == MOTTYPE_ROTARY )
				{
					if( (MencV->DivInfo.Limmlt != 65535) || ((SHORT)MencV->DivInfo.PaoSeqData >= 0) )
					{
						//pAsicHwReg->AREG_UARTXD  = '+';
						pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_Puls;		/* <S14F> */
					}
					else
					{
						//pAsicHwReg->AREG_UARTXD  = '-';
						pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_Mins;		/* <S14F> */
						MencV->DivInfo.PaoSeqData = -MencV->DivInfo.PaoSeqData;
					}
				}
				/*----------------------------------------------------------------------------------*/
				/*	���j�A�^�̏ꍇ																	*/
				/*----------------------------------------------------------------------------------*/
				else if( MencV->AxisMotType == MOTTYPE_LINEAR )
				{
					/*------------------------------------------------------------------------------*/
					/*	�t��]���[�h																*/
					/*------------------------------------------------------------------------------*/
					if( MencV->RevAsmMotor ^ Bprm->RvsDir )			/* ���]�ڑ� XOR �t��]���[�h	*/
					{
						if( (SHORT)MencV->DivInfo.PaoSeqData >= 0 )
						{
							//pAsicHwReg->AREG_UARTXD  = '-';
							pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_Mins;		/* <S14F> */
							MencV->DivInfo.PaoSeqData = MencV->DivInfo.PaoSeqData + 1;
						}
						else
						{
							//pAsicHwReg->AREG_UARTXD  = '+';
							pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_Puls;		/* <S14F> */
							MencV->DivInfo.PaoSeqData = ~MencV->DivInfo.PaoSeqData;
						}
					}
					/*------------------------------------------------------------------------------*/
					/*	����]���[�h																*/
					/*------------------------------------------------------------------------------*/
					else
					{
						if( (SHORT)MencV->DivInfo.PaoSeqData >= 0 )
						{
							//pAsicHwReg->AREG_UARTXD  = '+';
							pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_Puls;		/* <S14F> */
						}
						else
						{
							//pAsicHwReg->AREG_UARTXD  = '-';
							pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_Mins;		/* <S14F> */
							MencV->DivInfo.PaoSeqData = ~MencV->DivInfo.PaoSeqData + 1;
						}
					}
				}
				MencV->DivInfo.PaoSeqStep = 7;
				break;
	/*----------------------------------------------------------------------------------------------*/
	/*			�P�O�i�f�[�^�o�� : �T����,�S����,�R����,�Q����,�P����								*/
	/*----------------------------------------------------------------------------------------------*/
		case  7: SencOutputPaoMtdata( MencV, pAsicHwReg, 5 ); MencV->DivInfo.PaoSeqStep =  8; break;
		case  8: SencOutputPaoMtdata( MencV, pAsicHwReg, 4 ); MencV->DivInfo.PaoSeqStep =  9; break;
		case  9: SencOutputPaoMtdata( MencV, pAsicHwReg, 3 ); MencV->DivInfo.PaoSeqStep = 10; break;
		case 10: SencOutputPaoMtdata( MencV, pAsicHwReg, 2 ); MencV->DivInfo.PaoSeqStep = 11; break;
		case 11: SencOutputPaoMtdata( MencV, pAsicHwReg, 1 ); MencV->DivInfo.PaoSeqStep = 12; break;
	/*----------------------------------------------------------------------------------------------*/
		case 12: /* Output '0x0D'																	*/
	/*----------------------------------------------------------------------------------------------*/
				//pAsicHwReg->AREG_UARTXD  = (0x0d + 0x80);
				pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_Cr;		/* <S14F> */
				MencV->DivInfo.PaoSeqStep = 13;
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 13: /* Wait 2ms																		*/
	/*----------------------------------------------------------------------------------------------*/
				MencV->DivInfo.PaoSeqStep = 14;
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 14: /* IncPulse Output Request															*/
	/*----------------------------------------------------------------------------------------------*/
				/*----------------------------------------------------------------------------------*/
				/*		�c�`�o�̓��[�h�F�p���X�o��										   		    */
				/*----------------------------------------------------------------------------------*/
				/*		Bit13 :	DASEL(�����o��DA�[�q�I��)	:  0 : DA,  1: TXD_1				    */
				/*----------------------------------------------------------------------------------*/
				pAsicHwReg->AREG_PFNCSET  &= ~BIT13;			/* �c�`�o�̓��[�h�F�p���X�o��		*/
				/*----------------------------------------------------------------------------------*/
				MencV->DivInfo.PaoSeqMcmd = KPX_IPLSOUT_REQUEST;/* �����C���N���o�͎w�� ==> ScanA	*/
				MencV->DivInfo.PaoSeqStep = 15;
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 15: /* Check IncPulse Output Complete													*/
	/*----------------------------------------------------------------------------------------------*/
				if( MencV->DivInfo.PaoSeqMcmd == KPX_IPLSOUT_FINISH )	/* �����C���N���o�͊����H	*/
				{
					/*------------------------------------------------------------------------------*/
					/*	��]�^�̏ꍇ																*/
					/*------------------------------------------------------------------------------*/
					if( MencV->AxisMotType == MOTTYPE_ROTARY )
					{
						if( MencV->EncDisable == FALSE )
						{
							MencV->SenReady = TRUE;
							MencV->ReCalcAbsPosReq = TRUE;		/* ��Έʒu�Čv�Z�v���Z�b�g			*/
							SencMakeAbsPosition( MencV, 		/* ����]�ʋy��1��]���p���X���̎擾*/
												 MencV->DivInfo.Limmlt,
												 MencV->DivInfo.Bit_dp,
												 Bprm->RvsDir );
						}
					}
					/*------------------------------------------------------------------------------*/
					/*	���j�A�^�̏ꍇ																*/
					/*------------------------------------------------------------------------------*/
					else if( MencV->AxisMotType == MOTTYPE_LINEAR )
					{
						if( MencV->EncDisable == FALSE )
						{
							MencV->SenReady = TRUE;
							LmxSencGetLinearAbsPos( MencV, Bprm->RvsDir );/* ��Έʒu�f�[�^�̎擾	*/
						}
						pAsicHwReg->AREG_WDT1SET &= ~BIT8;		/* ��ٽ�ϊ��b��Ͻ��n�e�e  			*/
						MencV->DivInfo.DivCMaskStatus  = DIV_CMASK_OFF ;
					}
					/*------------------------------------------------------------------------------*/
					MencV->DivInfo.PaoSeqStep = 0;
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 20: /* Wait SEN Hi --> Lo (Output Alarm)												*/
	/*----------------------------------------------------------------------------------------------*/
				if( SenReq == FALSE )
				{
					MencV->SenReady = FALSE;
					MencV->DivInfo.PaoSeqMcmd = KPX_PAOSEQ_NOCMD;
					MencV->DivInfo.IncPulseReq = TRUE;
					if( AlmFlag )								/* �A���[��������					*/
					{
						pAsicHwReg->AREG_PFNCSET  |= BIT13;		/* �c�`�o�̓��[�h�F�s�w�c�o��		*/
						pAsicHwReg->AREG_POSET0	= 0xFFFFFFFF;	/* �p���X�o�͉�H������				*/
						pAsicHwReg->AREG_POTEST	= 0xFFFFFFFF;	/* �p���X�o�͉�H������				*/

						MencV->DivInfo.PaoSeqData = AlmCode;	/* �A���[���R�[�h�擾				*/
						MencV->DivInfo.PaoSeqStep = 21;			/* �A���[���o�̓X�e�b�v�ɑJ��		*/
					}
					else										/* �A���[�������Ȃ�					*/
					{
						MencV->DivInfo.PaoSeqStep =  1;			/* Lo-->Hi�Ď��X�e�b�v�ɑJ��		*/
					}
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 21: /* Wait 120ms																		*/
	/*----------------------------------------------------------------------------------------------*/
				if( SenReq == FALSE )
				{
					if( ++MencV->DivInfo.PaoSeqWait > KPI_SCANC_MS( 120 ) )
					{
						MencV->DivInfo.PaoSeqWait = 0;
						MencV->DivInfo.PaoSeqStep = 22;
					}
				}
				else
				{
					MencV->DivInfo.PaoSeqWait = 0;
					MencV->DivInfo.PaoSeqStep = 1;
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 22: /* Output 'A'																		*/
	/*----------------------------------------------------------------------------------------------*/
				if( SenReq == FALSE )
				{
					//pAsicHwReg->AREG_UARTXD  = 'A';
					pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_A;		/* <S14F> */
					MencV->DivInfo.PaoSeqStep = 23;
				}
				else
				{
					MencV->DivInfo.PaoSeqStep = 1;
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 23: /* Output 'L'																		*/
	/*----------------------------------------------------------------------------------------------*/
				if( SenReq == FALSE )
				{
					//pAsicHwReg->AREG_UARTXD  = ('L' + 0x80);
					pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_L;		/* <S14F> */
					MencV->DivInfo.PaoSeqStep = 24;
				}
				else
				{
					MencV->DivInfo.PaoSeqStep = 1;
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 24: /* Output 'M'																		*/
	/*----------------------------------------------------------------------------------------------*/
				if( SenReq == FALSE )
				{
					//pAsicHwReg->AREG_UARTXD  = 'M';
					pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_M;		/* <S14F> */
					MencV->DivInfo.PaoSeqStep = 25;
				}
				else
				{
					MencV->DivInfo.PaoSeqStep = 1;
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 25: /* Output Alarm Code																*/
	/*----------------------------------------------------------------------------------------------*/
				if( SenReq == FALSE )
				{
					uswk = MlibEvenAsciiTbl[((MencV->DivInfo.PaoSeqData >> 8) & 0x0F)];
					//pAsicHwReg->AREG_UARTXD  = uswk;
					pAsicHwReg->AREG_UARTXD  = MlibEvenAsciiTbl[ uswk ];		/* <S14F> */
					MencV->DivInfo.PaoSeqStep = 26;
				}
				else
				{
					MencV->DivInfo.PaoSeqStep = 1;
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 26: /* Output Alarm Code																*/
	/*----------------------------------------------------------------------------------------------*/
				if( SenReq == FALSE )
				{
					uswk = MlibEvenAsciiTbl[((MencV->DivInfo.PaoSeqData>> 4) & 0x0F)];
					//pAsicHwReg->AREG_UARTXD  = uswk;
					pAsicHwReg->AREG_UARTXD  = MlibEvenAsciiTbl[ uswk ];		/* <S14F> */
					MencV->DivInfo.PaoSeqStep = 27;
				}
				else
				{
					MencV->DivInfo.PaoSeqStep = 1;
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 27: /* Output '.'																		*/
	/*----------------------------------------------------------------------------------------------*/
				if( SenReq == FALSE )
				{
					//pAsicHwReg->AREG_UARTXD  = '.';
					pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_Prio;		/* <S14F> */
					MencV->DivInfo.PaoSeqStep = 28;
				}
				else
				{
					MencV->DivInfo.PaoSeqStep = 1;
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 28: /* Output 0x0D																		*/
	/*----------------------------------------------------------------------------------------------*/
				if( SenReq == FALSE )
				{
					//pAsicHwReg->AREG_UARTXD  = (0x0d + 0x80);
					pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_Cr;		/* <S14F> */
					MencV->DivInfo.PaoSeqStep = 29;
				}
				else
				{
					MencV->DivInfo.PaoSeqStep = 1;
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 29: /* End of Output Alarm																*/
	/*----------------------------------------------------------------------------------------------*/
				pAsicHwReg->AREG_PFNCSET  &= ~BIT13;			/* �c�`�o�̓��[�h�F�p���X�o��		*/
				MencV->DivInfo.PaoSeqStep = 1;
				break;
	/*----------------------------------------------------------------------------------------------*/
		default: /* Default																			*/
	/*----------------------------------------------------------------------------------------------*/
				MencV->DivInfo.PaoSeqStep = 0;
				break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;

}

/****************************************************************************************************/
/*																									*/
/*		�o�`�n���}���`�^�[���f�[�^�o��						<S02D>									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �}���`�^�[���f�[�^���w�茅�Ŋ����������A�X�L�[�����ɕϊ����Ďw��|�[�g�֏o�͂��A		*/
/*			���̌����������c���ۑ����܂��B�w�茅���f�[�^�̍ŏ�ʂł��鎖���K�v�ł��B				*/
/*																									*/
/*			��j*data = 32767 �� �T����(digit=5)���o�͂���ꍇ										*/
/*																									*/
/*				�o�̓f�[�^ = '3'																	*/
/*				�ۑ��f�[�^ = 2767																	*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static void SencOutputPaoMtdata( MENCV *MencV, ASIC_HW_IF_STR *pAsicHwReg, ULONG digit )
{
ULONG	x;
ULONG	outx;

/*--------------------------------------------------------------------------------------------------*/
/*		���f�[�^�ϊ�																				*/
/*--------------------------------------------------------------------------------------------------*/
		switch( digit )
		{
		case  1: digit = 1;     break;
		case  2: digit = 10;    break;
		case  3: digit = 100;   break;
		case  4: digit = 1000;  break;
		case  5: digit = 10000; break;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�}���`�^�[���f�[�^�o��																		*/
/*--------------------------------------------------------------------------------------------------*/
		x = MencV->DivInfo.PaoSeqData / digit;
		outx = MlibEvenAsciiTbl[ x ];
		pAsicHwReg->AREG_UARTXD  = (USHORT)outx;
		MencV->DivInfo.PaoSeqData -= (USHORT)(x * digit);
/*--------------------------------------------------------------------------------------------------*/
		return;

}

/****************************************************************************************************/
/*																									*/
/*		���j�A���[�^�F��Έʒu�f�[�^(64bit)�擾														*/
/*																									*/
/****************************************************************************************************/
static void LmxSencGetLinearAbsPos( MENCV  *MencV, BOOL RvsDir )
{
ULONG	PosL;
USHORT	PosH;
LONG	AbsPos[2];
UCHAR	Sftx = LMX_MPOS_SFT;

/*--------------------------------------------------------------------------------------------------*/
/*		�G���R�[�_�f�[�^���b�`																		*/
/*--------------------------------------------------------------------------------------------------*/
		PosH = MencV->RxPosH[0];						/* �X�L�����`�œǍ��񂾃f�[�^���g�p���āA	*/
		PosL = MencV->RxPosL[0].dw;						/* ��Έʒu(36Bit)���v�Z����B				*/

		AbsPos[1] = (LONG)((SHORT)PosH) >> Sftx;		
		AbsPos[0] = (PosH<<(32-Sftx))+((ULONG)PosL>>Sftx);

		/*------------------------------------------------------------------------------------------*/
		if( MencV->RevAsmMotor ^ RvsDir )				/* ���]�ڑ� XOR �t��]���[�h				*/
		{
			AbsPos[1] = (~AbsPos[1]);					/* �������]����								*/
			AbsPos[0] = (~AbsPos[0]) + 1;				/* �������]����								*/
			if( AbsPos[0] == 0 ){ ++AbsPos[1];}			/* Carry�̏ꍇ�́A��ʂ�+1					*/
		}

		MencV->SenOnScalePosH = AbsPos[1];
		MencV->SenOnScalePosL = AbsPos[0];
/*--------------------------------------------------------------------------------------------------*/
		return;
}

#if	(CSW_SVCF_FULC_MODE != CSW_FULC_NO_SUPPORT)
//�ȉ��̃t���N�֘A�����͎b��ł���A�t���N��p�t�@�C���ֈړ�����K�v������@<S02D>
/****************************************************************************************************/
/*																									*/
/*		�t���N�G���R�[�_ �F�����o�̓p���X���g��������x�̌v�Z							<S02D>		*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �����o�̓p���X���ő���g���ƂȂ��]���x���Z�o����B									*/
/*																									*/
/*			�p���X���g�����(SVCDEF_DIVOUT_MAXFREQ) :												*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*			MaxFreq [pps] = pgrate [pulse/rev] * DivOvrSpd [rev/s]									*/
/*																									*/
/*									       MaxFreq * 2^24											*/
/*			DivOvrSpd[2^24/OvrSpd] = ----------------------------	 								*/
/*									  pgrate  * (OvrSpd / 2*PAI) 									*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	DIVSET_ERROR FencSencCalMencDivOverSpeed( MENCV *FencV, UNI_PRM_STR *UniPrms )
{
#if (FLOAT_USE==TRUE)
	float	fw;
#else
	LONG	kx, sx;
#endif /* FLOAT_USE */
LONG	pgrate;													/* [pulse/rev]						*/
LONG	i;
LONG	BitNo = UniPrms->Prm->MencP.BitnoDpoint.b.l;
LONG	MaxFreq = SVCDEF_DIVOUT_MAXFREQ;						/* [pulse/sec]						*/
LONG	pulsno = 1;
DIVSET_ERROR	err;

/*--------------------------------------------------------------------------------------------------*/
/*		�����o�̓p���X���[�g�̌v�Z [pulse/rev]														*/
/*--------------------------------------------------------------------------------------------------*/
		pgrate = (((FencV->PulseNo >> BitNo) * UniPrms->Prm->scalepgrat) >> 2) - 1;
/*--------------------------------------------------------------------------------------------------*/
/*		����������x��l�Z�o																		*/
/*--------------------------------------------------------------------------------------------------*/
		for ( i = 0; pgrate > 0; i++ )
		{
			pgrate = pgrate / 2;
			pulsno = pulsno * 2;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�����o�̓p���X���g��������x�̌v�Z															*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
		fw = (float)MaxFreq * (float)0x01000000 / (float)pulsno;
		fw = fw / OvrSpd;
		fw = fw * 62832.0F / 10000.0F;
		if( fw > (float)0x01000000 )
		{
			FencV->DivInfo.DivOvrSpd = 0x01000000;
		}
		else
		{
			FencV->DivInfo.DivOvrSpd = (LONG)fw;
		}
#else
		kx = MlibScalKxgain( MaxFreq, 0x01000000, pulsno, &sx,   0 );
		kx = MlibPcalKxkxks( kx,      1,     UniPrms->Bprm->OvrSpd, &sx,   0 );
		kx = MlibPcalKxgain( kx,      62832,      10000,  &sx, -24 );
		FencV->DivInfo.DivOvrSpd = kx;
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		�p�����[�^�ݒ�ُ�																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( FencV->DivInfo.DivOvrSpd == 0 )
		{
			err = DIVSET_PRMERR_L;								/* A.040 : �p�����[�^�G���[�̐ݒ�	*/
		}
/*--------------------------------------------------------------------------------------------------*/
	return( err );
}

/****************************************************************************************************/
/*																									*/
/*		�t���N�G���R�[�_ : �����o�̓p���X�v�Z �� �`�r�h�b�ݒ�					<S02D>				*/
/*																									*/
/****************************************************************************************************/
static DIVSET_ERROR	FencSencSetMencDividedPulse( ASIC_HW_IF_STR *AsicHwReg, MENCV *FencV, UNI_PRM_STR *UniPrms )
{
LONG	x;
LONG	BitNo = UniPrms->Prm->MencP.BitnoDpoint.b.l;
LONG	pulsno;
LONG	pgrate = UniPrms->Prm->scalepgrat;
DIVSET_ERROR	err;

/*--------------------------------------------------------------------------------------------------*/
/*		�����o�̓p���X����l�ݒ�																	*/
/*--------------------------------------------------------------------------------------------------*/
		if( BitNo > 0 )
		{
			pulsno = (1<<BitNo);
		}
		else
		{
			pulsno = pndef_scalepgrat.UpperLimit;
		}

/*--------------------------------------------------------------------------------------------------*/
/*		�����o�̓p���X���g��������x�̌v�Z															*/
/*--------------------------------------------------------------------------------------------------*/
		err = FencSencCalMencDivOverSpeed( FencV, UniPrms );

/*--------------------------------------------------------------------------------------------------*/
/*		�o�f������ݒ�͈̓`�F�b�N																	*/
/*--------------------------------------------------------------------------------------------------*/
		if( (pgrate > pulsno) || (pgrate < 1) )				/* pgrate : �͈̓`�F�b�N				*/
		{
			return ( DIVSET_DIVERR );						/* A.041 : �����p���X�o�͐ݒ�ُ�		*/
		}

/*--------------------------------------------------------------------------------------------------*/
/*		�����p���X�o�̓Q�C���̉��Z																	*/
/*--------------------------------------------------------------------------------------------------*/
		FencV->DivInfo.DivMode = FALSE;
		FencV->DivInfo.DivPass = TRUE;
		FencV->DivInfo.DivRate = 0;
		FencV->DivInfo.DivOutGain = MlibScalKxgain( pgrate, 1, (1 << BitNo), NULL, 24 );
		FencV->DivInfo.DivOutCmskChk = MlibScalKxkxks( 0x1000, 1, FencV->DivInfo.DivOutGain, NULL, -30 );
/*--------------------------------------------------------------------------------------------------*/
/*		�`�r�h�b�����@�\�ݒ�																		*/
/*--------------------------------------------------------------------------------------------------*/
		x = (FencV->DivInfo.DivMode << 4) | FencV->DivInfo.DivPass | 0x0400;
		AsicHwReg->AREG_DINCSET	= (ULONG)x;						     /* �����@�\�ݒ�				*/
		AsicHwReg->AREG_DIVCSEV	= (ULONG)(FencV->DivInfo.DivRate);  /* ������ݒ�					*/
		AsicHwReg->AREG_DCTRIM  = (ULONG)FencV->DivInfo.DivRate - 1;/* �����o�͌��_�␳			*/
		if( UniPrms->Bprm->RvsDir )
		{
			AsicHwReg->AREG_DINCSET |= BIT2;						/* DB�t�]���[�h�ݒ�				*/
		}
/*--------------------------------------------------------------------------------------------------*/
		return (err);

}

/****************************************************************************************************/
/*																									*/
/*		�t���N�G���R�[�_ : �����p���X�h�e������														*/
/*																									*/
/****************************************************************************************************/
DIVSET_ERROR FencSencInitDivPulseIF( ASIC_HW_IF_STR* AsicHwReg, MENCV *FencV, UNI_PRM_STR *UniPrms )
{
	DIVSET_ERROR	err;

/*--------------------------------------------------------------------------------------------------*/
/*		�V���A���G���R�[�_�p���X�ϊ��ݒ�(Bit��,�ϊ�����) ��JL-056�����̐ݒ聗						*/
/*--------------------------------------------------------------------------------------------------*/
	AsicHwReg->AREG_PLSET	= 0x00003400;				/* �ϊ�Bit��:16bit, �ϊ�����:64us			*/
/*--------------------------------------------------------------------------------------------------*/
/*		�V���A���G���R�[�_�p���X�ϊ��ݒ�(�ʒu,���_,etc)												*/
/*--------------------------------------------------------------------------------------------------*/
	AsicHwReg->AREG_WDT1SET	|=	BIT8;
	FencV->DivInfo.DivCMaskStatus = DIV_CMASK_ON;
/*--------------------------------------------------------------------------------------------------*/
/*	DIVSET:�����@�\�ݒ�																				*/
/*--------------------------------------------------------------------------------------------------*/
/*	VULONG	AREG_DIVSET;		 0x0164 �����@�\�ݒ�												*/
								/*------------------------------------------------------------------*/
								/*  bit.1-0 :�ް���ٽ�ϊ��ʒu�ް������ݑI��						*/
								/*            0�F�ر�PGIF����قO���� (�����l)						*/
								/*  		  1�F�ر�PGIF����قP����								*/
								/*  		  2�Fν�CPU����											*/
								/*  		  3�F�������Z������										*/
								/*------------------------------------------------------------------*/
								/*  bit.2 : �����ۯ��I��											*/
								/*            0�F26.7MHz (�����l)									*/
								/*  		  1�F32MHz												*/
								/*------------------------------------------------------------------*/
								/*  bit.4-3 :�p���X�ϊ����_�␳�l�ݒ茳�I��						*/
								/*            0�F�ر�PGIF����قO���� (�����l)						*/
								/*  		  1�F�ر�PGIF����قP����								*/
								/*  		  2�Fν�CPU����											*/
								/*  		  3�F�������Z������										*/
								/*------------------------------------------------------------------*/
								/*  bit.8 : DIVSEL0													*/
								/*            0�FPA,PB,PC				 							*/
								/*  		  1�F�ް���ٽ�ϊ��o�́i�����l�j							*/
								/*------------------------------------------------------------------*/
								/*  bit.11-10 :DCSEL												*/
								/*            0�F�ʏ�C�� (�����l)									*/
								/*  		  1�F����C��											*/
								/*  		  2�F����C��2											*/
								/*  		  3�F����C��2											*/
								/*------------------------------------------------------------------*/
								/*  bit.13 : DIVASEL												*/
								/*            0�FDA�o�� (�����l)									*/
								/*  		  1�F�ر��ް��ٰ�o��									*/
								/*------------------------------------------------------------------*/
								/*  bit.15 : DIVCBIT												*/
								/*            0�FDC�o�� (�����l)									*/
								/*  		  1�FDCʲ���ً����o��									*/
/*--------------------------------------------------------------------------------------------------*/
	AsicHwReg->AREG_DIVSET	= 0x00000112;  				/* ��ٽ�ϊ������ݑ��ݒ�:ν�CPU,				*/
/*--------------------------------------------------------------------------------------------------*/
	AsicHwReg->AREG_POSET0	= 0xFFFFFFFF;				/* �p���X�ϊ��ʒu�ݒ�						*/
	AsicHwReg->AREG_POSET1	= 0x00000000;				/* �p���X�ϊ����_�␳�P						*/
	AsicHwReg->AREG_POSET2	= 0xFFFFFFFF;				/* �p���X�ϊ����_�␳�Q						*/
	AsicHwReg->AREG_POTEST	= 0xFFFFFFFF;				/* �p���X�ϊ��J�E���^�ݒ�					*/
/*--------------------------------------------------------------------------------------------------*/
/*		�������̓t�B���^�ݒ�																		*/
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/*	FBCSET:̨����ޯ���ٽ����																		*/
/*--------------------------------------------------------------------------------------------------*/
/*	VULONG	AREG_FBCSET;	     0x0040 �������̓t�B���^�ݒ�										*/
								/*------------------------------------------------------------------*/
								/*  bit.1-0 :PA,PB̨���I��											*/
								/*            0�F26.7MHz���� (�����l)								*/
								/*  		  1�F37.5ns												*/
								/*  		  2�F75ns												*/
								/*  		  3�F�f�ʂ�												*/
								/*------------------------------------------------------------------*/
								/*  bit.3-2 : PC̨���I��											*/
								/*            0�F26.7MHz���� (�����l)								*/
								/*  		  1�F37.5ns												*/
								/*  		  2�F75ns												*/
								/*  		  3�F�f�ʂ�												*/
								/*------------------------------------------------------------------*/
								/*  bit.5-15 : �ȗ�													*/
/*--------------------------------------------------------------------------------------------------*/
	AsicHwReg->AREG_FBCSET	= 0x00000005;				/* �������̓t�B���^�ݒ�						*/
/*--------------------------------------------------------------------------------------------------*/
/*		�����p���X�v�Z���`�r�h�b�ݒ�																*/
/*--------------------------------------------------------------------------------------------------*/
	err = FencSencSetMencDividedPulse( AsicHwReg, FencV, UniPrms );
/*--------------------------------------------------------------------------------------------------*/

	return( err );
}
#endif	/*(CSW_SVCF_FULC_MODE != CSW_FULC_NO_SUPPORT)*/
#endif	/*(SVFSEL_DIVOUT_USE == TRUE)*/




/*<S189>_Start*//*<S0C2>*/
#define   PS0_Use   /*PS0 Use*/
#ifdef    PS0_Use
/*********************************************************************************/
/*Explanation: This is a function for PSO data output , used by Full Closed loop*/
/*********************************************************************************/
void FencSencOutputPsoSerial(
								MENCV*           FencV,      /*Incremental */
                                ASIC_HW_IF_STR*  pAsicHwReg, /*Register AREG_UARTXD  Selection*/
                                SHORT limmlt,    /*Pn205 Set*/
                                BOOL  RvsDir     /*�t��]���[�h���f*/
                             )
{
		ASIC_HW_IF_STR *pAsicHwReg2;
		pAsicHwReg2 = (ASIC_HW_IF_STR*)((UCHAR*)pAsicHwReg + 0x1000);/*2-axis pointing address*/
		if(FencV->SenReady == FALSE)  /*waiting for the SenReady time*/
		{
			FencV->DivInfo.PsoSeqStep = 0;
			return;
		}
		if(FencV->EncReady==FALSE)/* if Encoder Ready==FALSE Retrun*/
		{
			FencV->DivInfo.PsoSeqStep = 0;
			return;
		}
	  switch( FencV->DivInfo.PsoSeqStep )/*Step One by One*/
	  {
		case  0: /* Initial Step*/
				FencV->DivInfo.PsoSeqWait = 0;
				FencV->DivInfo.PsoSeqStep = 1;
		break;
		case  1: /* Get  Multiturn  and Incremental Data*/
				 FencV->DivInfo.PsoLinearData_H=  (USHORT)FencV->RxPosH[0];
				 FencV->DivInfo.PsoLinearData_L = ((ULONG)FencV->RxPosL[0].dw)>>FencV->MposSftR;/*Linear Value*/
				 FencV->DivInfo.PsoSeqStep = 2;
		break;
		case  2: /*Output 'P'*/
				pAsicHwReg->AREG_PFNCSET &= ~BIT13; /*Output Data Set DA2*/
				pAsicHwReg2->AREG_PFNCSET|= BIT13;  /*Output Data Set PSO  Register Selection*/
				pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_P;		/* <S14F> P*/
				FencV->DivInfo.PsoSeqStep = 3;
		break;
		case 3 : /* Output '+' or '-'*/
				/*----------------------------------------------------------------------------------*/
				/*	�t��]���[�h																	    */
				/*----------------------------------------------------------------------------------*/
				if( FencV->DivInfo.RvsDir )
				{
					if( (SHORT)FencV->DivInfo.PsoMturnLatch >= 0 )
					{
						pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_Mins;		/* <S14F>  -*/
					}
					else
					{
						pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_Puls;		/* <S14F>  +*/
					}
				}
				/*----------------------------------------------------------------------------------*/
				/*	����]���[�h																	*/
				/*----------------------------------------------------------------------------------*/
				else
				{
					if( (SHORT)FencV->DivInfo.PsoMturnLatch >= 0 )
					{
						pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_Puls;		/* <S14F>  +*/
					}
					else
					{
						pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_Mins;		/* <S14F>  -*/
					}
				}
				FencV->DivInfo.PsoSeqStep = 4;
		break;
/* <S20A>�F�����������F[MencV->DivInfo.PsoLinearData_*]���|�C���^�� */
		case 4:  OutputPsoLinearIncre((LONG*)&FencV->DivInfo.PsoLinearData_H, 5, pAsicHwReg); FencV->DivInfo.PsoSeqStep=  5; break;
		case 5:  OutputPsoLinearIncre((LONG*)&FencV->DivInfo.PsoLinearData_H, 4, pAsicHwReg); FencV->DivInfo.PsoSeqStep=  6; break;
		case 6:  OutputPsoLinearIncre((LONG*)&FencV->DivInfo.PsoLinearData_H, 3, pAsicHwReg); FencV->DivInfo.PsoSeqStep=  7; break;
		case 7:  OutputPsoLinearIncre((LONG*)&FencV->DivInfo.PsoLinearData_H, 2, pAsicHwReg); FencV->DivInfo.PsoSeqStep=  8; break;
		case 8:  OutputPsoLinearIncre((LONG*)&FencV->DivInfo.PsoLinearData_H, 1, pAsicHwReg); FencV->DivInfo.PsoSeqStep=  9; break;
		case 9:  OutputPsoLinearIncre((LONG*)&FencV->DivInfo.PsoLinearData_L, 6, pAsicHwReg); FencV->DivInfo.PsoSeqStep=  10; break;
		case 10: OutputPsoLinearIncre((LONG*)&FencV->DivInfo.PsoLinearData_L, 5, pAsicHwReg); FencV->DivInfo.PsoSeqStep=  11; break;
		case 11: OutputPsoLinearIncre((LONG*)&FencV->DivInfo.PsoLinearData_L, 4, pAsicHwReg); FencV->DivInfo.PsoSeqStep=  12; break;
		case 12: OutputPsoLinearIncre((LONG*)&FencV->DivInfo.PsoLinearData_L, 3, pAsicHwReg); FencV->DivInfo.PsoSeqStep=  13; break;
		case 13: OutputPsoLinearIncre((LONG*)&FencV->DivInfo.PsoLinearData_L, 2, pAsicHwReg); FencV->DivInfo.PsoSeqStep=  14; break;
		case 14: OutputPsoLinearIncre((LONG*)&FencV->DivInfo.PsoLinearData_L, 1, pAsicHwReg); FencV->DivInfo.PsoSeqStep=  15; break;
/* <S20A>�F�����܂� */
		case 15: /* Output 0x0D   CR*/
				 pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_Cr;		/* <S14F> 0x0d*/
				 FencV->DivInfo.PsoSeqStep = 16;break;
		case 16: /* Wait 2ms*/
				 FencV->DivInfo.PsoSeqStep = 17;break;
		case 17: /* Wait 2ms*/
				 FencV->DivInfo.PsoSeqStep = 18;break;
		case 18: /* Wait 2ms*/
				FencV->DivInfo.PsoSeqStep = 19;break;
		case 19: /* Wait 2ms*/
				FencV->DivInfo.PsoSeqStep = 20;break;
		case 20: /* End Step*/
				FencV->DivInfo.PsoSeqStep = 1;break;
		default: /* Default Step*/
				FencV->DivInfo.PsoSeqStep = 0;break;
 }/* End if MencV->AxisMotType == MOTTYPE_LINEAR*/
}
/*********************************************************************************/
/*Explanation: This is a function for PSO data output , used by Motor Closed loop*/
/*********************************************************************************/
void SencOutputPsoSerial(MENCV* MencV,               /*Multiturn  and Incremental step*/
		                 ASIC_HW_IF_STR* pAsicHwReg, /*Register AREG_UARTXD  Selection*/
		                 SHORT limmlt,                /*Pn205 Set  �}���`�^�[�����~�b�g*/
		                 BOOL  RvsDir)                /*�t��]���[�h���f���܂�*/
{
	ASIC_HW_IF_STR *pAsicHwReg2;
	pAsicHwReg2 = (ASIC_HW_IF_STR*)((UCHAR*)pAsicHwReg + 0x1000);/*2-axis pointing address*/
	if(MencV->SenReady==FALSE)  /*waiting for the SenReady time*/
	{
		MencV->DivInfo.PsoSeqStep = 0;
		return;
	}
	if(MencV->EncReady==FALSE)/* if Encoder Ready==FALSE Retrun*/
	{
		MencV->DivInfo.PsoSeqStep = 0;
		return;
	}
	/* MOTTYPE_ROTARY */
   if(MencV->AxisMotType == MOTTYPE_ROTARY)
   {

	   switch( MencV->DivInfo.PsoSeqStep )/*Step One by One*/
		{
			case  0: /* Initial Step*/
					MencV->DivInfo.PsoSeqWait = 0;
					MencV->DivInfo.PsoSeqStep = 1;
			break;
			case  1: /* Get  Multiturn  and Incremental Data	*/
					MencV->DivInfo.PsoMturnLatch= SencGetMturnData(MencV,MencV->DivInfo.Limmlt, MencV->DivInfo.Bit_dp,(USHORT)MencV->RxPosH[0],MencV->RxPosL[0].dw );
					MencV->DivInfo.PsoIncPulseOut = ((ULONG)MencV->RcvPosX[0])>>MencV->MposSftR;/*Determine the decimal point position*/
					MencV->DivInfo.PsoSeqStep = 2;
			break;
			case  2: /*Output 'P'*/
				    pAsicHwReg->AREG_PFNCSET &= ~BIT13; /*Output Data Set DA2*/
				    pAsicHwReg2->AREG_PFNCSET|= BIT13;  /*Output Data Set PSO Register Selection*/
				    /*SGDV�����ڏ����A��7�ǉ����W�X�^����*/
					pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_P;		/* <S14F> P*/
					MencV->DivInfo.PsoSeqStep = 3;
			break;
			case 3 : /* Output '+' or '-'*/
					if( (limmlt != 65535)/*Pn205 Set*/
						|| ((SHORT)MencV->DivInfo.PsoMturnLatch >= 0) )
					{
						pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_Puls;/* <S14F> + */
					}
					else
					{
						pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_Mins;/* <S14F>  - */
						MencV->DivInfo.PsoMturnLatch = -MencV->DivInfo.PsoMturnLatch;
					}
					MencV->DivInfo.PsoSeqStep = 4;
			break;
		    /*����]�f�[�^*/
			case  4: OutputPsoMturnIncre(MencV, 5, 0,pAsicHwReg); MencV->DivInfo.PsoSeqStep =  5; break;
			case  5: OutputPsoMturnIncre(MencV, 4, 0,pAsicHwReg); MencV->DivInfo.PsoSeqStep =  6; break;
			case  6: OutputPsoMturnIncre(MencV, 3, 0,pAsicHwReg); MencV->DivInfo.PsoSeqStep =  7; break;
			case  7: OutputPsoMturnIncre(MencV, 2, 0,pAsicHwReg); MencV->DivInfo.PsoSeqStep =  8; break;
			case  8: OutputPsoMturnIncre(MencV, 1, 0,pAsicHwReg); MencV->DivInfo.PsoSeqStep =  9; break;
			case  9: pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_Conm; MencV->DivInfo.PsoSeqStep =  10; break;	/* <S147> , */
		     /*�P��]���ʒu�f�[�^*/
			case 10: OutputPsoMturnIncre(MencV, 8, 1, pAsicHwReg); MencV->DivInfo.PsoSeqStep= 11; break;
			case 11: OutputPsoMturnIncre(MencV, 7, 1, pAsicHwReg); MencV->DivInfo.PsoSeqStep= 12; break;
			case 12: OutputPsoMturnIncre(MencV, 6, 1, pAsicHwReg); MencV->DivInfo.PsoSeqStep= 13; break;
			case 13: OutputPsoMturnIncre(MencV, 5, 1, pAsicHwReg); MencV->DivInfo.PsoSeqStep= 14; break;
			case 14: OutputPsoMturnIncre(MencV, 4, 1, pAsicHwReg); MencV->DivInfo.PsoSeqStep= 15; break;
			case 15: OutputPsoMturnIncre(MencV, 3, 1, pAsicHwReg); MencV->DivInfo.PsoSeqStep= 16; break;
			case 16: OutputPsoMturnIncre(MencV, 2, 1, pAsicHwReg); MencV->DivInfo.PsoSeqStep= 17; break;
			case 17: OutputPsoMturnIncre(MencV, 1, 1, pAsicHwReg); MencV->DivInfo.PsoSeqStep= 18; break;
			case 18: /* Output 0x0D*/
					 pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_Cr;/* <S147> 0x0D */
					 MencV->DivInfo.PsoSeqStep = 19;break;
			case 19: /* Wait 2ms*/
				     MencV->DivInfo.PsoSeqStep = 20;break;
			case 20: /* End Step*/
				     MencV->DivInfo.PsoSeqStep = 1;break;
			default: /* Default Step*/
				     MencV->DivInfo.PsoSeqStep = 0;break;
	 }/*End MencV.PsoSeqStep*/
  }/*End (MencV->AxisMotType == MOTTYPE_ROTARY)*/
/*-----------------------------------------------------------------------------------------------------------------------*/
 /* MOTTYPE_LINEAR */
	 if(MencV->AxisMotType == MOTTYPE_LINEAR)
	 {
		  switch( MencV->DivInfo.PsoSeqStep )/*Step One by One*/
		  {
			case  0: /* Initial Step*/
					MencV->DivInfo.PsoSeqWait = 0;
					MencV->DivInfo.PsoSeqStep = 1;
			break;
			case  1: /* Get  Multiturn  and Incremental Data	*/
				    MencV->DivInfo.PsoMturnLatch= SencGetMturnData(MencV,MencV->DivInfo.Limmlt, MencV->DivInfo.Bit_dp,(USHORT)MencV->RxPosH[0],MencV->RxPosL[0].dw );
				    MencV->DivInfo.PsoLinearData_H=  (USHORT)MencV->RxPosH[0];/* DATA_High*/
				    MencV->DivInfo.PsoLinearData_L = ((ULONG)MencV->RxPosL[0].dw)>>MencV->MposSftR;/*DATA_LOW*/
					MencV->DivInfo.PsoSeqStep = 2;
			break;
			case  2: /*Output 'P'*/
				     pAsicHwReg->AREG_PFNCSET &= ~BIT13; /*Output Data Set DA2*/
					 pAsicHwReg2->AREG_PFNCSET|= BIT13;  /*Output Data Set PSO Register Selection*/
					 /*SGDV�����ڏ����A��7�ǉ����W�X�^����*/
					 pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_P;/* <S147> P*/
					 MencV->DivInfo.PsoSeqStep = 3;
			break;
			case 3 : /* Output '+' or '-'*/
				    /*------------------------------------------------------------------------------*/
					/*	�t��]���[�h																*/
					/*------------------------------------------------------------------------------*/
					if( MencV->RevAsmMotor ^ RvsDir)	/* ���]�ڑ� XOR �t��]���[�h	*/
					{
						if( (SHORT)MencV->DivInfo.PsoMturnLatch >= 0 )
						{
							pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_Mins;/*-*/
						}
						else
						{
							pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_Puls;/*+*/
						}
					}
					/*------------------------------------------------------------------------------*/
					/*	����]���[�h																    */
					/*------------------------------------------------------------------------------*/
					else
					{
						if( (SHORT)MencV->DivInfo.PsoMturnLatch >= 0 )
						{
							pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_Puls;/*+*/
						}
						else
						{
							pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_Mins;/*-*/
						}
					}
					MencV->DivInfo.PsoSeqStep = 4;
			break;
/* <S20A>�F�����������F[MencV->DivInfo.PsoLinearData_*]���|�C���^�� */
			case 4:  OutputPsoLinearIncre((LONG*)&MencV->DivInfo.PsoLinearData_H, 5, pAsicHwReg); MencV->DivInfo.PsoSeqStep= 5; break;
			case 5:  OutputPsoLinearIncre((LONG*)&MencV->DivInfo.PsoLinearData_H, 4, pAsicHwReg); MencV->DivInfo.PsoSeqStep= 6; break;
			case 6:  OutputPsoLinearIncre((LONG*)&MencV->DivInfo.PsoLinearData_H, 3, pAsicHwReg); MencV->DivInfo.PsoSeqStep=  7; break;
			case 7:  OutputPsoLinearIncre((LONG*)&MencV->DivInfo.PsoLinearData_H, 2, pAsicHwReg); MencV->DivInfo.PsoSeqStep=  8; break;
			case 8:  OutputPsoLinearIncre((LONG*)&MencV->DivInfo.PsoLinearData_H, 1, pAsicHwReg); MencV->DivInfo.PsoSeqStep=  9; break;
			case 9:  OutputPsoLinearIncre((LONG*)&MencV->DivInfo.PsoLinearData_L, 6, pAsicHwReg); MencV->DivInfo.PsoSeqStep=  10; break;
			case 10: OutputPsoLinearIncre((LONG*)&MencV->DivInfo.PsoLinearData_L, 5, pAsicHwReg); MencV->DivInfo.PsoSeqStep=  11; break;
			case 11: OutputPsoLinearIncre((LONG*)&MencV->DivInfo.PsoLinearData_L, 4, pAsicHwReg); MencV->DivInfo.PsoSeqStep=  12; break;
			case 12: OutputPsoLinearIncre((LONG*)&MencV->DivInfo.PsoLinearData_L, 3, pAsicHwReg); MencV->DivInfo.PsoSeqStep=  13; break;
		    case 13: OutputPsoLinearIncre((LONG*)&MencV->DivInfo.PsoLinearData_L, 2, pAsicHwReg); MencV->DivInfo.PsoSeqStep=  14; break;
			case 14: OutputPsoLinearIncre((LONG*)&MencV->DivInfo.PsoLinearData_L, 1, pAsicHwReg); MencV->DivInfo.PsoSeqStep=  15; break;
/* <S20A>:�����܂� */
			case 15: /* Output 0x0D   CR*/
					 pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_Cr; /* <S147>  0x0D */
					 MencV->DivInfo.PsoSeqStep = 16;break;
			case 16: /* Wait 2ms*/
				     MencV->DivInfo.PsoSeqStep = 17;break;
			case 17: /* Wait 2ms*/
				     MencV->DivInfo.PsoSeqStep = 18;break;
			case 18: /* Wait 2ms*/
				     MencV->DivInfo.PsoSeqStep = 19;break;
			case 19: /* Wait 2ms*/
				     MencV->DivInfo.PsoSeqStep = 20;break;
			case 20: /* End Step*/
				     MencV->DivInfo.PsoSeqStep = 1;break;
			default: /* Default Step*/
				     MencV->DivInfo.PsoSeqStep = 0;break;
		  }/*End MencV.PsoSeqStep*/
	 }/* End if(MencV->AxisMotType == MOTTYPE_LINEAR)*/
}

/************************************************************************************************/
/* Linear Get Incremental Data                                                                	*/
/************************************************************************************************/
static void	OutputPsoLinearIncre(LONG *PsoIncPulseOut, ULONG digit, ASIC_HW_IF_STR *pAsicHwReg)	/* <S20A>:���������� */
{
	 long	x;
	 long	outx;
/*--------------------------------------------------------------------------------------------------*/
/*		Convert digit data																			*/
/*--------------------------------------------------------------------------------------------------*/
		switch( digit )
		{
			case  1: digit =  1;        break;
			case  2: digit =  10;       break;
			case  3: digit =  100;      break;
			case  4: digit =  1000;     break;
			case  5: digit =  10000;    break;
			case  6: digit =  100000;   break;
			default: break;
		}
/*		Output Incremental data		*/
/*--------------------------------------------------------------------------------------------------*/
	x = *PsoIncPulseOut/digit;	/* <S20A> */
	outx = MlibEvenAsciiTbl[ x ];		/* <S14F> */
	pAsicHwReg->AREG_UARTXD = (UCHAR)outx; /*Send the Data*/
	*PsoIncPulseOut -= (x * digit);	/* <S20A> */
}


/************************************************************************************************/
/*Get Multiturn and Incremental Data*/
/****************************************************************************************************/
static void	OutputPsoMturnIncre( MENCV *MencV,ULONG digit, ULONG outsel,ASIC_HW_IF_STR* pAsicHwReg)
{
	ULONG	x;
	ULONG	outx;
/*--------------------------------------------------------------------------------------------------*/
/*		Convert digit data	*/
/*--------------------------------------------------------------------------------------------------*/
		switch( digit )
		{
			case  1: digit = 1;        break;
			case  2: digit = 10;       break;
			case  3: digit = 100;      break;
			case  4: digit = 1000;     break;
			case  5: digit = 10000;    break;
			case  6: digit = 100000;   break;
			case  7: digit = 1000000;  break;
			case  8: digit = 10000000; break;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Output Multiturn data	 */
/*--------------------------------------------------------------------------------------------------*/
		if( outsel == 0 )
		{
			x = MencV->DivInfo.PsoMturnLatch/digit;
			outx =MlibEvenAsciiTbl[x];		/* <S14F> */
			pAsicHwReg->AREG_UARTXD = (UCHAR)outx;
			/*Send the Data SGDV�����ڏ����A��7�ǉ����W�X�^����*/
			MencV->DivInfo.PsoMturnLatch -= (USHORT)(x * digit);
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Output Incremental data		*/
/*--------------------------------------------------------------------------------------------------*/
		else
		{
			x = MencV->DivInfo.PsoIncPulseOut/digit;
			outx =MlibEvenAsciiTbl[ x ];		/* <S14F> */
			pAsicHwReg->AREG_UARTXD = (UCHAR)outx; /*Send the Data*/
			/*Send the Data SGDV�����ڏ����A��7�ǉ����W�X�^����*/
			MencV->DivInfo.PsoIncPulseOut -= (x * digit);
		}
}
#endif
/*End <S189>*/
/***************************************** end of file **********************************************/
