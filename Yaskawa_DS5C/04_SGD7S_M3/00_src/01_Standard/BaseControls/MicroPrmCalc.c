/****************************************************************************************************/
/*																									*/
/*																									*/
/*		ini_sub.c : SVN�������֘A�T�u�֐����W���[��													*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : SVN�������֘A�T�u�֐����`�B															*/
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
#include "JL086ServoIpHw.h"
//#include "PowerManager.h"
#include "MLib.h"
#include "KLib.h"
#include "MicroPrmCalc.h"

/* defines */
#define	D_AXIS			0
#define	Q_AXIS			1

#define	REQ_LMT_CHK		1
#define	NO_LMT_CHK		0



/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
/*	void KpxPcalMicroProgram( UNI_PRM_STR *uni_prm_ptr, ASICS *SvAsicRegs,							*/
/*													USHORT *mreg_csw_ptr, LONG caller_axis_no );	*/
/*	void KpiPcalMicroNotchFilter2( ASICS *SvAsicRegs, LONG hz, LONG qx, LONG dx, SHORT *MpReg );	*/
/*	LONG KpiChangePwmFrequency( ULONG PwmFx, PRMDATA *prm_ptr );									*/
/*	LONG LpxSetPrmToASIC( SHORT *adr, LONG prm, BOOL limchk );										*/
/*	LONG MicroTranslatePrmReq( ASICS *SvAsicRegs, void *Data, VUSHORT *Address, LONG Size );		*/
/****************************************************************************************************/
static LONG	PcalDqAcrKc( LONG Iadmax, USHORT imax, USHORT imaxs );
static LONG	PcalLDidt( KSGAIN Le, KSGAIN max_cur, KSGAIN vdc );
static LONG	PcalDqLxComp( KSGAIN Lx, KSGAIN over_spd, KSGAIN max_cur, KSGAIN vdc );
static LONG	PcalDqRxComp( KSGAIN Rx, KSGAIN max_cur, KSGAIN vdc );
static LONG	PcalDqEmfComp( KSGAIN Emf, KSGAIN over_spd, KSGAIN vdc );
static LONG	PcalOndelayLevel( LONG Level );
static LONG	PcalOndelaySlope( LONG Slope );
static LONG	PcalPwmFrequency( LONG PwmFx );
static	LONG	PcalPwmCounter( LONG CarrCnt );		/* ���[�^�L�����A���g���؂�ւ�<S117> */
static LONG	PcalVFBKiLim( LONG TvLim );
static void	KpiPcalCurrentObs( ASICS *SvAsicRegs, USHORT ignq, USHORT pwmf, BPRMDAT *Bprm, SHORT *MpReg);
static void	KpiPcalMicroLpassFilter1( LONG tf, LONG ts, SHORT *MpReg );



/****************************************************************************************************/
/*																									*/
/*		�ʃv���O�����p�p�����[�^�v�Z���C���֐�														*/
/*																									*/
/****************************************************************************************************/
LONG	KpxPcalMicroProgram( MAICRO_CFGPRM *uCfgPrm, BPRMDAT *Bprm,
							 ASICS *SvAsicRegs, WEAKENFIELD* WeakenField )
{
	ASIC_U_IF_STR	*pAsicMicroReg;
	LONG			kp, kx;
	LONG			rc;

#if (FLOAT_USE==FALSE)
	LONG			sx;
#endif /* FLOAT_USE */


	pAsicMicroReg = SvAsicRegs->AsicMicroReg;
	rc  = 0;

/*--------------------------------------------------------------------------------------------------*/
/*		���䎲���̐ݒ�																<S0E4>			*/
/*--------------------------------------------------------------------------------------------------*/
	SvAsicRegs->AsicMicroReg->AxisInfo.AxisNum = MAX_AXIS_NUMBER;

/*--------------------------------------------------------------------------------------------------*/
/*		����t���O(CSW)�̐ݒ�																		*/
/*--------------------------------------------------------------------------------------------------*/
	SvAsicRegs->MicroCsw.data = (USHORT)( uCfgPrm->dq_sw | 0x00F0 );			/* ����t���O		*/
//	if( uCfgPrm->pwmf > 4000 )
	kx = PcalPwmCounter( uCfgPrm->NormCarrCnt );		/* �ݒ�L�����A�J�E���^�l������g�����t�Z	*//* ���[�^�L�����A���g���؂�ւ�<S117> */
	if( kx > 4000 )
	{
		/* �L�����A���g����4kHz�ȏ�̃A���v�ł͓d���I�u�U�[�o���g�p */
		SvAsicRegs->MicroCsw.data &= 0xFFF7;
	}

	if(( uCfgPrm->flg_wf & 0x0001 ) == 0 )
	{ /* ���[�^���̎�ߊE���L�������`�F�b�N */
		/* ���[�^���������̏ꍇ�A�����I�ɖ��� */
		SvAsicRegs->MicroCsw.data &= 0xF7FF;
	}

	if( ( SvAsicRegs->MicroCsw.data & 0x0800 ) == 0 )
	{
		/* ��ߊE��=OFF�̏ꍇ�C��ߊE��2=OFF�ɂ��� */
		SvAsicRegs->MicroCsw.data &= 0xDFFF;
	}

	if( ( SvAsicRegs->MicroCsw.data & 0x2000 ) != 0 )
	{
		/* ��ߊE��2=ON�̏ꍇ�C�ϕ���~�@�\=ON�ɂ��� */
		SvAsicRegs->MicroCsw.data |= 0x4000;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�d�����Z�Q�C�����d���␳�Q�C��																*/
/*--------------------------------------------------------------------------------------------------*/
	/* �d�����Z�Q�C�� */
	kx = PcalDqAcrKc( uCfgPrm->shant, uCfgPrm->imax, uCfgPrm->imaxs );
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->IntAdP.Kc), kx, REQ_LMT_CHK );
	/*----------------------------------------------------------------------------------------------*/
	/* �d���␳�Q�C�� */
	kx = uCfgPrm->vrevgn;
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->IntAdP.KvvIn), kx, NO_LMT_CHK );

/*--------------------------------------------------------------------------------------------------*/
/*		d���d�����[�v�Q�C��(��ၕ�ϕ�������)														*/
/*--------------------------------------------------------------------------------------------------*/
	/* ���Q�C�� */
	kp = PcalDqAcrKp( uCfgPrm->ignd, 100, Bprm, D_AXIS );
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->IntAdP.KdP), kp, REQ_LMT_CHK );
	/*----------------------------------------------------------------------------------------------*/
	/* �ϕ��Q�C�� */
	kx = PcalDqAcrKi( uCfgPrm->kid, kp, Bprm->MotLd, Bprm );
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->IntAdP.KdI), kx, REQ_LMT_CHK );
	/*----------------------------------------------------------------------------------------------*/
	/* �ϕ������l */
	kx = uCfgPrm->limintd;
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->IntAdP.VdLim), kx, NO_LMT_CHK );
	/*----------------------------------------------------------------------------------------------*/
	/* L(di/dt)��L */
	kx = PcalLDidt( Bprm->MotLd, Bprm->MaxCur, Bprm->Vdc );
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->IntAdP.L_dIdt), kx, REQ_LMT_CHK );

/*--------------------------------------------------------------------------------------------------*/
/*		q���d�����[�v�Q�C��(��ၕ�ϕ�������)														*/
/*--------------------------------------------------------------------------------------------------*/
	/* ���Q�C�� */
	kp = PcalDqAcrKp( uCfgPrm->ignq, 100, Bprm, Q_AXIS );
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->IntAdP.KqP), kp, REQ_LMT_CHK );
	/*----------------------------------------------------------------------------------------------*/
	/* �ϕ��Q�C�� */
	kx = PcalDqAcrKi( uCfgPrm->kiq, kp, Bprm->MotLq, Bprm );
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->IntAdP.KqI), kx, REQ_LMT_CHK );
	/*----------------------------------------------------------------------------------------------*/
	/* �ϕ������l */
	kx = uCfgPrm->limintq;
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->IntAdP.VqLim), kx, NO_LMT_CHK );

/*--------------------------------------------------------------------------------------------------*/
/*		�d�����[�v�o�̓t�B���^���萔																*/
/*--------------------------------------------------------------------------------------------------*/
	/* �o�̓t�B���^�Q�C�� */
	kx = PcalDqAcrFilter( uCfgPrm->ifil );
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->IntAdP.Tfil), kx, REQ_LMT_CHK );

/*--------------------------------------------------------------------------------------------------*/
/*		�C���_�N�^���X�⏞�萔(Ld,Lq)																*/
/*--------------------------------------------------------------------------------------------------*/
	/* d���C���_�N�^���X */
	kx = PcalDqLxComp( Bprm->MotLd, Bprm->OvrSpd, Bprm->MaxCur,  Bprm->Vdc );
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->IntAdP.Ld), kx, REQ_LMT_CHK );
	/*----------------------------------------------------------------------------------------------*/
	/* q���C���_�N�^���X */
	kx = PcalDqLxComp( Bprm->MotLq, Bprm->OvrSpd, Bprm->MaxCur,  Bprm->Vdc );
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->IntAdP.Lq), kx, REQ_LMT_CHK );

/*--------------------------------------------------------------------------------------------------*/
/*		�@�B�p �� �d�C�p�ϊ��W��																	*/
/*--------------------------------------------------------------------------------------------------*/
	/* �@�B�p �� �d�C�p�ϊ��W�� */
#if (FLOAT_USE==TRUE) 
	kx = (LONG)Bprm->Keangle;
#else
	kx = MlibScalKskxkx( Bprm->Keangle, 1, 1, &sx, -24 );
#endif /* FLOAT_USE */
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->IntAdV.KEangle), kx, NO_LMT_CHK );

/*--------------------------------------------------------------------------------------------------*/
/*		�d�@�q��R(R1)�⏞�萔																		*/
/*--------------------------------------------------------------------------------------------------*/
	/* R1�⏞�萔 */
	kx = PcalDqRxComp( Bprm->MotR, Bprm->MaxCur, Bprm->Vdc );
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->IntAdP.MotResist), kx, REQ_LMT_CHK );

/*--------------------------------------------------------------------------------------------------*/
/*		EMF�⏞�萔(��)																				*/
/*--------------------------------------------------------------------------------------------------*/
	/* EMF�⏞�萔 */
	kx = PcalDqEmfComp( Bprm->MotEmf, Bprm->OvrSpd, Bprm->Vdc );
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->IntAdP.Mag), kx, REQ_LMT_CHK );

/*--------------------------------------------------------------------------------------------------*/
/*		�I���f�B���C�⏞																			*/
/*--------------------------------------------------------------------------------------------------*/
	/* �I���f�B���C�⏞���� */
	kx = uCfgPrm->odt_c.b.h * (ASIC_CLKMHZ/2) / 10;
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->IntAdP.OnDelayComp),  kx, NO_LMT_CHK );
	/*----------------------------------------------------------------------------------------------*/
	/* �I���f�B���C�⏞�ؑփ��x�� */
	kx = PcalOndelayLevel( uCfgPrm->odlv_slp.b.l );
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->IntAdP.OnDelayLvl), kx, NO_LMT_CHK );
	/*----------------------------------------------------------------------------------------------*/
	/* �I���f�B���C�⏞�X�� */
	kx = PcalOndelaySlope( uCfgPrm->odlv_slp.b.h );
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->IntAdP.OnDelaySlope), kx, NO_LMT_CHK );

/*--------------------------------------------------------------------------------------------------*/
/*		�d�����o�[����																				*/
/*--------------------------------------------------------------------------------------------------*/
	/* U���d���[���� */
	kx = (SHORT)(uCfgPrm->zadjiu);
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->AdinV.IuOffsetIn), kx, NO_LMT_CHK );
	/*----------------------------------------------------------------------------------------------*/
	/* V���d���[���� */
	kx = (SHORT)(uCfgPrm->zadjiv);
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->AdinV.IvOffsetIn), kx, NO_LMT_CHK );

/*--------------------------------------------------------------------------------------------------*/
/*		�d�����o�Q�C����																			*/
/*--------------------------------------------------------------------------------------------------*/
	/* U���d���Q�C���� */
	kx = (CHAR)uCfgPrm->gadjiu_v.b.l + 0x100;
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->AdinV.KcuIn), kx, NO_LMT_CHK );
	/*----------------------------------------------------------------------------------------------*/
	/* V���d���Q�C���� */
	kx = (CHAR)uCfgPrm->gadjiu_v.b.h + 0x100;
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->AdinV.KcvIn), kx, NO_LMT_CHK );

/*--------------------------------------------------------------------------------------------------*/
/*		PWM�L�����A���g���J�E���g�l																	*/
/*--------------------------------------------------------------------------------------------------*/
	/* PWM�L�����A���g�� */
//	kx =  KpiChangePwmFrequency( 0, uCfgPrm->pwmf, uCfgPrm->pwmf3, uCfgPrm->flg_wf );
////	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->IntAdP.CrFreq), kx, NO_LMT_CHK );
	rc |= LpxSetPrmToASIC( (SHORT*)&(pAsicMicroReg->IntAdP.CrFreq), uCfgPrm->NormCarrCnt, NO_LMT_CHK );/*<S0C7>*//* ���[�^�L�����A���g���؂�ւ�<S117> */

/*--------------------------------------------------------------------------------------------------*/
/*		�d�����[�v�d�������l																		*/
/*--------------------------------------------------------------------------------------------------*/
	/* �d�������l */
	kx = uCfgPrm->lmdata;
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->IntAdP.Vmax), kx, NO_LMT_CHK );

/*--------------------------------------------------------------------------------------------------*/
/*		�g���N�t�B���^ : �ꎟ�x��t�B���^�P(�s�g�p,�O�̂���)										*/
/*--------------------------------------------------------------------------------------------------*/
//	KpiManageMicroTorqueFilter( CSW_LPFDSABL, FALSE, mreg_csw_ptr, pAsicMicroReg );
	SvAsicRegs->MicroCsw.data |= LPFDSABL;
	KpiPcalMicroLpassFilter1( 0, KPI_MACYCLENS, &(pAsicMicroReg->IntAdP.TLpf) );

#if 0 /* 2012.08.31 Y.Oka �@�\�����̂��߁A�R�����g�A�E�g */
/*--------------------------------------------------------------------------------------------------*/
/*		�g���N�t�B���^ : �ꎟ�x��t�B���^�Q(Pn411,SGDX�ł͕s�g�p,�O�̂���)							*/
/*--------------------------------------------------------------------------------------------------*/
//	KpiManageMicroTorqueFilter( CSW_LPFCDSABL, FALSE, mreg_csw_ptr, pAsicMicroReg  );
	SvAsicRegs->MicroCsw.data |= LPFCDSABL;
	KpiPcalMicroLpassFilter1( 0, KPI_MACYCLENS, &(pAsicMicroReg->MREG_TLPFC) );
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		���[�^���U�}���p�m�b�`�t�B���^																*/
/*--------------------------------------------------------------------------------------------------*/
	if ( (uCfgPrm->motresfrq != 0) && (uCfgPrm->motresq != 0) )
	{ /* �ݒ���g����0�̂Ƃ��A�m�b�`�t�B���^�L�� */
	/*----------------------------------------------------------------------------------------------*/
		KpiPcalMicroNotchFilter2(						/* �Q���m�b�`�t�B���^�̃p�����[�^�v�Z		*/
			SvAsicRegs,									/* JL-076�A�N�Z�X�p�\����					*/
			uCfgPrm->motresfrq,							/* �t�B���^���g��		[Hz]				*/
			uCfgPrm->motresq,							/* �t�B���^�p�萔		[0.01]				*/
			0,											/* �t�B���^�[��			[0.001]				*/
			&(pAsicMicroReg->IntAdP.Kf31) );			/* �t�B���^�p�����[�^�ݒ�擪�A�h���X		*/
	/*----------------------------------------------------------------------------------------------*/
		/* �m�b�`�t�B���^�R�L�� */
//		KpiManageMicroTorqueFilter( CSW_F3DSABL, TRUE, mreg_csw_ptr, pAsicMicroReg );
		SvAsicRegs->MicroCsw.data &= ~F3DSABL;
	}
	else
	{
		/* �m�b�`�t�B���^�R���� */
//		KpiManageMicroTorqueFilter( CSW_F3DSABL, FALSE, mreg_csw_ptr, pAsicMicroReg );
		SvAsicRegs->MicroCsw.data |= F3DSABL;
	}

	/* Set to ASIC */
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->IntAdP.CtrlSw), SvAsicRegs->MicroCsw.data, NO_LMT_CHK );

/*--------------------------------------------------------------------------------------------------*/
/*		�d�����[�v�ϕ����~�b�g																		*/
/*--------------------------------------------------------------------------------------------------*/
	kx = PcalVFBKiLim( uCfgPrm->idlim );
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->WeakFV.WfIntegLim), kx, NO_LMT_CHK );

/*--------------------------------------------------------------------------------------------------*/
/*		d���d���w�߃��~�b�g�l�v�Z�p�萔�̌v�Z														*/
/*--------------------------------------------------------------------------------------------------*/
	PcalIdrefLimTerm234( WeakenField, SvAsicRegs, Bprm, uCfgPrm->oslv_pol, uCfgPrm->pnvoltgn );

/*--------------------------------------------------------------------------------------------------*/
/*		�d���I�u�U�[�o�p�����[�^�v�Z																*/
/*--------------------------------------------------------------------------------------------------*/
	/* �p�����[�^�ݒ� */
//	KpiPcalCurrentObs( SvAsicRegs, uCfgPrm->ignq, uCfgPrm->pwmf, Bprm, &(pAsicMicroReg->DobsP.TsPerL) );		/* ���[�^�L�����A���g���؂�ւ�<S117> */
	kx = PcalPwmCounter( uCfgPrm->NormCarrCnt );		/* �ݒ�L�����A�J�E���^�l������g�����t�Z	*/
	KpiPcalCurrentObs( SvAsicRegs, uCfgPrm->ignq, kx, Bprm, &(pAsicMicroReg->DobsP.TsPerL) );

	return rc;
}

#if 0		/* ���j�b�g�L�����A�Œ菈���s�v�̂��ߍ폜<S11E> */
#if ( defined(CSW_SGD7W_TYPE) || defined(CSW_COMMON_CARRIER) )		/* ���[�^�L�����A���g���؂�ւ�<S117> */
/*----------------------------------------------------------------------------------*/
/*  @@@@@@ SGD7W��p�L�����A���g���������� �ʃL�����A�ݒ���m�F�܂ł̒��b�� @@@@@@@ */
/*----------------------------------------------------------------------------------*/
LONG	KpxRecalcPWMparameter( MAICRO_CFGPRM *uCfgPrm, BPRMDAT *Bprm, ASICS *SvAsicRegs )
{
	ASIC_U_IF_STR	*pAsicMicroReg;
	LONG			kx;
	LONG			rc;

	pAsicMicroReg = SvAsicRegs->AsicMicroReg;
	rc  = 0;

/*--------------------------------------------------------------------------------------------------*/
/*		����t���O(CSW)�̐ݒ�																		*/
/*--------------------------------------------------------------------------------------------------*/
	SvAsicRegs->MicroCsw.data = (USHORT)( uCfgPrm->dq_sw | 0x00F0 );			/* ����t���O		*/
	kx = PcalPwmCounter( uCfgPrm->NormCarrCnt );		/* �ݒ�L�����A�J�E���^�l������g�����t�Z	*/
	if( kx > 4000 )
	{
		/* �L�����A���g����4kHz�ȏ�̃A���v�ł͓d���I�u�U�[�o���g�p */
		SvAsicRegs->MicroCsw.data &= 0xFFF7;
	}

	if(( uCfgPrm->flg_wf & 0x0001 ) == 0 )
	{ /* ���[�^���̎�ߊE���L�������`�F�b�N */
		/* ���[�^���������̏ꍇ�A�����I�ɖ��� */
		SvAsicRegs->MicroCsw.data &= 0xF7FF;
	}

	if( ( SvAsicRegs->MicroCsw.data & 0x0800 ) == 0 )
	{
		/* ��ߊE��=OFF�̏ꍇ�C��ߊE��2=OFF�ɂ��� */
		SvAsicRegs->MicroCsw.data &= 0xDFFF;
	}

	if( ( SvAsicRegs->MicroCsw.data & 0x2000 ) != 0 )
	{
		/* ��ߊE��2=ON�̏ꍇ�C�ϕ���~�@�\=ON�ɂ��� */
		SvAsicRegs->MicroCsw.data |= 0x4000;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		PWM�L�����A���g���J�E���g�l																	*/
/*--------------------------------------------------------------------------------------------------*/
	/* PWM�L�����A���g�� */
	rc |= LpxSetPrmToASIC( (SHORT*)&(pAsicMicroReg->IntAdP.CrFreq), uCfgPrm->NormCarrCnt, NO_LMT_CHK );/*<S0C7>*/

/*--------------------------------------------------------------------------------------------------*/
/*		�d���I�u�U�[�o�p�����[�^�v�Z																*/
/*--------------------------------------------------------------------------------------------------*/
	/* �p�����[�^�ݒ� */
	kx = PcalPwmCounter( uCfgPrm->NormCarrCnt );		/* �ݒ�L�����A�J�E���^�l������g�����t�Z	*/
	KpiPcalCurrentObs( SvAsicRegs, uCfgPrm->ignq, kx, Bprm, &(pAsicMicroReg->DobsP.TsPerL) );

	return rc;
}
#endif	/* #if ( defined(CSW_SGD7W_TYPE) || defined(CSW_COMMON_CARRIER) ) *//* ���[�^�L�����A���g���؂�ւ�<S117> */
#endif	/* #if 0		���j�b�g�L�����A�Œ菈���s�v�̂��ߍ폜<S11E> */


/*<S1F5> START*/
/*****************************************************************************
  Description: OC���x��
------------------------------------------------------------------------------
  Parameters:
	uCfgPrm             �A���[���Ǘ��h�e�f�[�^
	SvAsicRegs			ASIC�A�h���X�e�[�u���ւ̃|�C���^
	MencP				�G���R�[�_�p�����[�^��`
  Return:

------------------------------------------------------------------------------
  Note:

*****************************************************************************/
#if 0	/* <S1F7> */
LONG	KpxMotorOCLevel( MAICRO_CFGPRM *uCfgPrm, ASICS *SvAsicRegs, MENCPRM *MencP )
{
	LONG	kx;
	LONG	rc;
	LONG	OcLevel;

	rc  = 0;

/*--------------------------------------------------------------------------------------------------*/
/*		�}�X�N���																						*/
/*--------------------------------------------------------------------------------------------------*/
	kx = MencP->motfuncf.f.OCset;
	if( kx == 1 )
	{
		rc |= LpxSetPrmToASIC( (SHORT*)&(SvAsicRegs->AsicHwReg->AREG_SETMSK), kx, REQ_LMT_CHK );
	}
/*--------------------------------------------------------------------------------------------------*/
/*		OC���o���x��																					*/
/*--------------------------------------------------------------------------------------------------*/
	OcLevel = MlibDiv6432( (MencP->irat * MencP->mot_oc), (uCfgPrm->shant * 4), (uCfgPrm->imaxs * 100) );
	kx = OcLevel;
	rc |= LpxSetPrmToASIC( (SHORT*)&(SvAsicRegs->AsicHwReg->AREG_OCLVUP), kx, REQ_LMT_CHK );
	rc |= LpxSetPrmToASIC( (SHORT*)&(SvAsicRegs->AsicHwReg->AREG_OCLVUM), kx, REQ_LMT_CHK );
	rc |= LpxSetPrmToASIC( (SHORT*)&(SvAsicRegs->AsicHwReg->AREG_OCLVVP), kx, REQ_LMT_CHK );
	rc |= LpxSetPrmToASIC( (SHORT*)&(SvAsicRegs->AsicHwReg->AREG_OCLVVM), kx, REQ_LMT_CHK );
	rc |= LpxSetPrmToASIC( (SHORT*)&(SvAsicRegs->AsicHwReg->AREG_OCLVWP), kx, REQ_LMT_CHK );
	rc |= LpxSetPrmToASIC( (SHORT*)&(SvAsicRegs->AsicHwReg->AREG_OCLVWM), kx, REQ_LMT_CHK );
/*--------------------------------------------------------------------------------------------------*/
/*		�f�V���[�V�����t�B���^���샂�[�h																		*/
/*--------------------------------------------------------------------------------------------------*/
	kx = 0;
	rc |= LpxSetPrmToASIC( (SHORT*)&(SvAsicRegs->AsicHwReg->AREG_OCDCONF1), kx, REQ_LMT_CHK );
/*--------------------------------------------------------------------------------------------------*/
/*		���[�^�ߓd�����o�t�B���^��																		*/
/*--------------------------------------------------------------------------------------------------*/
	kx = uCfgPrm->fltrange;
	rc |= LpxSetPrmToASIC( (SHORT*)&(SvAsicRegs->AsicHwReg->AREG_FLTRANGE), kx, REQ_LMT_CHK );
	return( rc );
}
/*<S1F5> END*/
#else	/* <S1F7> */
LONG	KpxMotorOCLevel( MAICRO_CFGPRM *uCfgPrm, ASICS *SvAsicRegs, MENCPRM *MencP )
{
	LONG			rc;
	LONG			OcMaxU, OcMaxV, OcMaxW;			/* OC���o臒l(����)								*/
	LONG			OcMinU, OcMinV, OcMinW;			/* OC���o臒l(����)								*/
	LONG			OcWorkU, OcWorkV, OcWorkW;		/* OC���o臒l���Z�p���[�N						*/
	LONG			OcLevel;						/* OC���o���x��(�Q�C�����A�I�t�Z�b�g���O)		*/
	ASIC_HW_IF_STR	*pAsicHwReg = SvAsicRegs->AsicHwReg;

	rc  = 0;
/*--------------------------------------------------------------------------------------------------*/
/*		�}�X�N���																					*/
/*--------------------------------------------------------------------------------------------------*/
	if( (LONG)MencP->motfuncf.f.OCset == 1 )
	{
		pAsicHwReg->AREG_SETMSK		|= 0x000F;			/* OC���o����								*/
/*--------------------------------------------------------------------------------------------------*/
/*		OC���o�L��																					*/
/*--------------------------------------------------------------------------------------------------*/
		pAsicHwReg->AREG_SETREG1	= 0x00000005;		/* ASIC Write Enable						*/
		pAsicHwReg->AREG_CSTP		|= ( 1 << 26 );		/* OC���o����								*/
		pAsicHwReg->AREG_SETREG1	= 0x00000001;		/* ASIC Write Disable						*/
/*--------------------------------------------------------------------------------------------------*/
		return( rc );									/* �����̏ꍇ�́A���̐ݒ�͍s��Ȃ��B		*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�f�V���[�V�����t�B���^���샂�[�h															*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_OCDCONF1	&= 0xFFF0;
	pAsicHwReg->AREG_OCSDMCONF3	= 0x00001B38;	/*
								bit0 - 7:OC���o�pADIF�T�C�N����                0x38(�d��AD�Ɠ���)
								bit8 -11:AD��{�N���b�N�r�b�g                  0x0B(�d��AD�Ɠ���)
								bit12   :MSB�I��                               0x01(�d��AD�Ɠ���)	*/
	pAsicHwReg->AREG_OCSDMCONF4	= 0x00003839;	/*
								bit0 - 7:OC���o�pAD�R���g���[��0�����オ��ݒ� 0x39(�d��AD�Ɠ���)
								bit8 -11:OC���o�pAD�R���g���[��0�~���ݒ�       0x38(�d��AD�Ɠ���)	*/
/*--------------------------------------------------------------------------------------------------*/
/*		OC���o���x��																				*/
/*		0x0000(-320mV) �` 0xFFFF(+320mV)	shant max 8192											*/
/*			���� �� 0x8000 +�i PnF2C �~ PnE28 �~ 4 �j�� PnE15										*/
/*			���� �� 0x8000 -�i PnF2C �~ PnE28 �~ 4 �j�� PnE15										*/
/*		PnF2C = MencP->mot_oc  : ���[�^OC���o�d��[0.1A]												*/
/*		PnE28 = uCfgPrm->shant : �d�����o�Q�C��														*/
/*		PnE15 = uCfgPrm->imaxs : �T�[�{�p�b�N�ő�d��[0.1A]											*/
/*																									*/
/*		�Q�C���A�I�t�Z�b�g�␳�v�Z																	*/
/*			�␳�� �� �␳�O �� ( �d�����o�Q�C���� + 1.00 ) - �d�����o�[���� �~ 4					*/
/*		PnE50   = uCfgPrm->zadjiu : �d�����o�[����(U��)												*/
/*		PnE51   = uCfgPrm->zadjiv : �d�����o�[����(V��)												*/
/*		PnE52.L = uCfgPrm->gadjiu_v.b.l : �d�����o�Q�C����(U��)										*/
/*		PnE52.H = uCfgPrm->gadjiu_v.b.h : �d�����o�Q�C����(V��)										*/
/*--------------------------------------------------------------------------------------------------*/
	/* OC���o���x�� */
	OcLevel = MlibDiv6432(( MencP->mot_oc ), ( uCfgPrm->shant * 4 ), ( uCfgPrm->imaxs ));
	/* U���␳ */
	OcWorkU  = OcLevel * 0x200 / ((CHAR)uCfgPrm->gadjiu_v.b.l + 0x100 );
//	OcWorkU  = (( OcWorkU + 1 ) >> 1 ) - (SHORT)uCfgPrm->zadjiu * 4;								  /* <S1FC> */
	OcWorkU  = (( OcWorkU + 1 ) >> 1 );
	OcMaxU = 0x00008000 + OcWorkU - ((SHORT)uCfgPrm->zadjiu * 4);									  /* <S1FC> */
	if( OcMaxU > 0x0000FFFF ) OcMaxU = 0x0000FFFF;
	OcMinU = 0x00008000 - OcWorkU - ((SHORT)uCfgPrm->zadjiu * 4);									  /* <S1FC> */
	if( OcMinU < 0x00000000 ) OcMinU = 0x00000000;
	pAsicHwReg->AREG_OCLVUP = OcMaxU;
	pAsicHwReg->AREG_OCLVUM = OcMinU;
	/* V���␳ */
	OcWorkV  = OcLevel * 0x200 / ((CHAR)uCfgPrm->gadjiu_v.b.h + 0x100 );
//	OcWorkV  = (( OcWorkV + 1 ) >> 1 ) - (SHORT)uCfgPrm->zadjiv * 4;								  /* <S1FC> */
	OcWorkV  = (( OcWorkV + 1 ) >> 1 );
	OcMaxV = 0x00008000 + OcWorkV - ((SHORT)uCfgPrm->zadjiv * 4);									  /* <S1FC> */
	if( OcMaxV > 0x0000FFFF ) OcMaxV = 0x0000FFFF;
	OcMinV = 0x00008000 - OcWorkV - ((SHORT)uCfgPrm->zadjiv * 4);									  /* <S1FC> */
	if( OcMinV < 0x00000000 ) OcMinV = 0x00000000;
	pAsicHwReg->AREG_OCLVVP = OcMaxV;
	pAsicHwReg->AREG_OCLVVM = OcMinV;
	/* W���␳ */ /* <S1FC> */
	OcWorkW = ( OcWorkU > OcWorkV )? OcWorkU : OcWorkV;
	OcMaxW = 0x00008000 + OcWorkW + ((SHORT)uCfgPrm->zadjiu * 4) + ((SHORT)uCfgPrm->zadjiv * 4);
	if( OcMaxW > 0x0000FFFF ) OcMaxW = 0x0000FFFF;
	OcMinW = 0x00008000 - OcWorkW + ((SHORT)uCfgPrm->zadjiu * 4) + ((SHORT)uCfgPrm->zadjiv * 4);
	if( OcMinW < 0x00000000 ) OcMinW = 0x00000000;
	pAsicHwReg->AREG_OCLVWP = OcMaxW;
	pAsicHwReg->AREG_OCLVWM = OcMinW;
//	pAsicHwReg->AREG_OCLVWP = ( OcMaxU > OcMaxV )? OcMaxU : OcMaxV ;								  /* <S1FC> */
//	pAsicHwReg->AREG_OCLVWM = ( OcMinU < OcMinV )? OcMinU : OcMinV ;								  /* <S1FC> */
/*--------------------------------------------------------------------------------------------------*/
/*		���[�^�ߓd�����o�t�B���^��																*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_FLTRANGE = uCfgPrm->fltrange;
/*--------------------------------------------------------------------------------------------------*/
/*		OC���o�L��																					*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SETMSK		&= 0xFFF0;			/* OC���o�L��									*/
	pAsicHwReg->AREG_SETREG1	= 0x00000005;		/* ASIC Write Enable							*/
	pAsicHwReg->AREG_CSTP		&= ~( 1 << 26 );	/* OC���o�L��									*/
	pAsicHwReg->AREG_SETREG1	= 0x00000001;		/* ASIC Write Disable							*/
/*--------------------------------------------------------------------------------------------------*/
/*		OC���o�pAD�T�C�N������																		*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_OCVADSYNC = 0x0000;
/*--------------------------------------------------------------------------------------------------*/
	return( rc );
}
#endif	/* <S1F7> */
/****************************************************************************************************/
/*																									*/
/*		�d�����Z�Q�C���̌v�Z																		*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*			  10000 * 2^9																			*/
/*		Kc = -------------																			*/
/*			    Iadmax																				*/
/*																									*/
/*		Iadmax [-] : �ő�d�����̂`�c�b�l															*/
/*																									*/
/****************************************************************************************************/
static	LONG	PcalDqAcrKc( LONG Iadmax, USHORT imax, USHORT imaxs )
{
#if (FLOAT_USE==TRUE)
	float fw;
#else
		LONG	kx,sx;
#endif /* FLOAT_USE */

	if( imax >= imaxs )
	{
#if (FLOAT_USE==TRUE)
		fw = (10000.0F * 512.0F) / (float)Iadmax;
#else
		kx = (10000 * 512) / Iadmax;
#endif /* FLOAT_USE */

	}
	else
	{
#if (FLOAT_USE==TRUE)
		fw = (512.0F * 10000.0F) / (float)Iadmax;
		fw = fw * (float)imaxs / (float)imax;
#else
		kx = MlibScalKxgain( 512, 10000,     Iadmax,   &sx,   0 );
		kx = MlibPcalKxgain( kx,  imaxs, imax, &sx, -24 );
#endif /* FLOAT_USE */
	}

#if (FLOAT_USE==TRUE)
	return( fw );
#else
	return( kx );
#endif /* FLOAT_USE */

}



/****************************************************************************************************/
/*																									*/
/*		�d�����[�v���Q�C���̌v�Z																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*			  2*PAI * Gain * Derate * MotLd       2^13     Imax * 512 								*/
/*		Kp = ----------------------------- * ------- * -------------								*/
/*				              100    	      Vdc/2	      15000  									*/
/*																									*/
/*		Gain  [Hz] : �d�����[�v�Q�C��																*/
/*		Derate [%] : �d�����[�v�Q�C���f�B���[�e�B���O												*/
/*		Lx     [H] : ���[�^�C���_�N�^���X															*/
/*																									*/
/****************************************************************************************************/
LONG	PcalDqAcrKp( LONG Gain, LONG Derate, BPRMDAT *Bprm, LONG axis_type )
{
#if (FLOAT_USE==TRUE)
	float	fw;
#else
	LONG	kx, sx;
#endif /* FLOAT_USE */
	LONG	rc;

#if (FLOAT_USE==TRUE)
	fw = (float)Gain * (float)Derate;
#else
	kx = MlibScalKxgain( Gain, Derate, 1, &sx, 0 );
#endif /* FLOAT_USE */

	if(axis_type == D_AXIS)
	{ /* D�� */
#if (FLOAT_USE==TRUE)
		fw = fw * Bprm->MotLd / Bprm->Vdc;
#else
		kx = MlibPcalKxksks( kx,   Bprm->MotLd,          Bprm->Vdc, &sx,   0 );
#endif /* FLOAT_USE */

	}
	else //if(axis_type == Q_AXIS)
	{ /* Q�� */
#if (FLOAT_USE==TRUE)
				fw = fw * Bprm->MotLq / Bprm->Vdc;
#else
		kx = MlibPcalKxksks( kx,   Bprm->MotLq,          Bprm->Vdc, &sx,   0 );
#endif /* FLOAT_USE */
	}
#if (FLOAT_USE==TRUE)
	fw = fw * Bprm->MaxCur;
	fw = fw * 26353589.0F / 750000.0F;
	if( fw > (float)0x01000000 )
	{
		rc = 0x01000000;
	}
	else
	{
		rc = (LONG)fw;
	}
#else
	kx = MlibPcalKxkskx( kx,   Bprm->MaxCur, 1,        &sx,   0 );
	rc = MlibPcalKxgain( kx,   26353589,    750000,   &sx, -24 );
#endif /* FLOAT_USE */

	return( rc );
}



/****************************************************************************************************/
/*																									*/
/*		�d�����[�v�ϕ��Q�C���̌v�Z																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*			  Kp * Ts * 2^4																			*/
/*		Ki = ---------------																		*/
/*				1000 * Ti																			*/
/*																									*/
/*		Ts [ns] : �d�����[�v���Z����																*/
/*		Ti [us] : �d�����[�v�ϕ����萔																*/
/*																									*/
/****************************************************************************************************/
LONG	PcalDqAcrKi( LONG Ti, LONG Kp, KSGAIN MotL, BPRMDAT *Bprm )
{
	LONG	kx,sx;
	LONG	Tx;
	LONG	MotW;
#if (FLOAT_USE==TRUE)
	float fw;
#endif
	
/*--------------------------------------------------------------------------------------------------*/
/*		�d�����[�v�ϕ����萔[us]�̌v�Z(���[�^�̓d�C�I���萔)										*/
/*																									*/
/*				         Ld[H]		   																*/
/*		Tx[us] =  --------------- �~ 1000000														*/
/*				     Rx[0.001ohm]																	*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	fw = MotL * 1000000.0;
	fw = fw / Bprm->MotR;
#else
	kx = MlibScalKskxkx( MotL, 1000000, 1, &sx, 0 );
	kx = MlibPcalKxkxks( kx, 1,	Bprm->MotR, &sx,   -24 );
#endif /* FLOAT_USE */

	/* ���~�b�g����	*/
#if (FLOAT_USE==TRUE)
	Tx = MlibLimitul( (long)fw, 0xFFFF, 0 );
#else
	Tx = MlibLimitul( kx, 0xFFFF, 0 );					/* 0���ŏ��l�Ƃ��Ďg�p						*/
#endif /* FLOAT_USE */

	if( Bprm->CurrentLoopTiSw == TRUE )
	{	/* ���[�^�@��Ɋ֌W�Ȃ��A���[�^�d�C�I�ϕ����萔��ݒ� */
		Ti = Tx;
	}
	else
	{	/* �w��@��̂݁A���[�^�d�C�I�ϕ����萔��ݒ� */
		if( Bprm->MotTypeCode == 0x67 )					/* SGMMV���[�^�R�[�h							*/
		{
#if (FLOAT_USE==TRUE)
			MotW = (LONG)Bprm->MotW;
#else
			MotW = MlibScalKskxkx( Bprm->MotW, 1, 1, NULL, -30 );
#endif /* FLOAT_USE */
			switch( MotW )
			{
				case 0x0003:	/* SGMMV-B3E */
				case 0x0005:	/* SGMMV-B5E */
				case 0x000B:	/* SGMMV-B9E */
					Ti = Tx;
				break;
			
				default:
					break;
			}
		}
	}

/*--------------------------------------------------------------------------------------------------*/

	if( Ti == 0 )
	{
#if (FLOAT_USE==TRUE)
		fw = 0;
#else
		kx = 0;
#endif /* FLOAT_USE */
	}
	else
	{
#if (FLOAT_USE==TRUE)
		fw = (float)Kp * 16.0 * (float)KPI_MACYCLENS / (1000.0 * (float)Ti);
#else
		kx = MlibScalKxgain( Kp, 16*KPI_MACYCLENS, 1000*Ti, NULL, -24 );
#endif /* FLOAT_USE */
	}
#if (FLOAT_USE==TRUE)
	return( fw );
#else
	return( kx );
#endif /* FLOAT_USE */
}



/****************************************************************************************************/
/*																									*/
/*		L(di/dt)�̃C���_�N�^���X�p�����[�^�v�Z														*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*						   Imax[A]		8192			10^9										*/
/*		L_DIDT = Le[H] �~ --------- �~ -------- �~ -------------- �~ 2^9							*/
/*							15000		Vdc/2		 scantime[ns]									*/
/*																									*/
/****************************************************************************************************/
static	LONG	PcalLDidt( KSGAIN Le, KSGAIN max_cur, KSGAIN vdc )
{
#if (FLOAT_USE==TRUE)
	float	fw;
#else
	LONG	kx, sx;
#endif /* FLOAT_USE */
	LONG	rc;

#if (FLOAT_USE==TRUE)
	fw = Le * max_cur / 15000.0F;
	fw = fw * 16384.0F * 512.0F / vdc;
	fw = fw * 1000000000.0F / (float)KPI_MBCYCLENS;
	if( fw > (float)0x01000000 )
	{
		rc = 0x01000000;
	}
	else
	{
		rc = (LONG)fw;
	}
#else
	kx = MlibScalKskskx( Le,   max_cur,      15000,          &sx,   0 );
	kx = MlibPcalKxkxks( kx,   16384*512,    vdc,            &sx,   0 );
	rc = MlibPcalKxgain( kx,   1000000000,   KPI_MBCYCLENS,  &sx, -24 );
#endif /* FLOAT_USE */

	return( rc );
}



/****************************************************************************************************/
/*																									*/
/*		�d�����[�v�t�B���^�Q�C���̌v�Z																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*			    Ts * 2^14																			*/
/*		Kf = -----------------																		*/
/*			  (1000*Tf) + Ts																		*/
/*																									*/
/*		Ts [ns] : �d�����[�v���Z����																*/
/*		Tf [us] : �d�����[�v�t�B���^���萔															*/
/*																									*/
/****************************************************************************************************/
LONG	PcalDqAcrFilter( LONG Tf )
{
#if (FLOAT_USE==TRUE) 
	float fw;

	fw = (float)KPI_MACYCLENS * 16384.0 / ((1000.0*(float)Tf) + (float)KPI_MACYCLENS);

	return( fw );
#else
	LONG	kx;

	kx = MlibScalKxgain( KPI_MACYCLENS, 16384, ((1000*Tf) + KPI_MACYCLENS), NULL, -24 );

	return( kx );
#endif /* FLOAT_USE */
}



/****************************************************************************************************/
/*																									*/
/*		�C���_�N�^���X�⏞�萔(Ld,Lq)�̌v�Z															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*			  2^15 * 2*PAI*Nos     2^15 * Imax      Lx * 2^13										*/
/*		kx = ------------------ * ------------- * --------------		--- SGDS					*/
/*			     60 * 15000        15000 * 10      10^5 * Vdc/2										*/
/*																									*/
/*																									*/
/*			  2^15 * Nmax     2^15 * Imax     Lx * 2^13     PoleNo									*/
/*		kx = ------------- * ------------- * ----------- * --------		--- SVOS					*/
/*				 15000           15000          Vdc/2         2										*/
/*																									*/
/*																									*/
/*		Nmax [rad/s] : Max. Speed																	*/
/*		Imax [A]     : Max. Current																	*/
/*		Lx   [H]     : Inductance																	*/
/*		Vdc  [V]     : DC Voltage																	*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	LONG	PcalDqLxComp( KSGAIN Lx, KSGAIN over_spd, KSGAIN max_cur, KSGAIN vdc )
{
#if (FLOAT_USE==TRUE)
	float	fw;
#else
	LONG	kx, sx;
#endif /* FLOAT_USE */
	LONG	rc;

/*--------------------------------------------------------------------------------------------------*/
/*		�C���_�N�^���X�⏞�萔(Ld,Lq)�̌v�Z															*/
/*--------------------------------------------------------------------------------------------------*/
/*		��]�^�FBprm.Keangle = PoleNo / 2		: Motor[rad] ---> Eangle[rad]						*/
/*		���j�A�FBprm.Keangle = PAI / PolePitch	: Motor[ m ] ---> Eangle[rad]						*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	fw = over_spd * max_cur / vdc;
	fw = fw * Lx;
	fw = fw * 78187.0F;
	if( fw > (float)0x01000000 )
	{
		rc = 0x01000000;
	}
	else
	{
		rc = (LONG)fw;
	}
#else
	kx = MlibScalKsksks( over_spd,	  max_cur,  	vdc,   &sx,   0 );
	kx = MlibPcalKxkskx( kx,          Lx,           1,     &sx,   0 );
	rc = MlibPcalKxgain( kx,          78187,        1,     &sx, -24 );
#endif /* FLOAT_USE */
/*--------------------------------------------------------------------------------------------------*/
	return( rc );
}



/****************************************************************************************************/
/*																									*/
/*		�d�@�q��R(R1)�⏞�萔�̌v�Z																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*			   2^15 * Rx      2^13     Imax															*/
/*		kx = ------------ * ------- * -------														*/
/*				   1         Vdc/2     15000														*/
/*																									*/
/*																									*/
/*		Rx   [ohm] : Resistance																		*/
/*		Imax [A]   : Max. Current																	*/
/*		Vdc  [V]   : DC Voltage																		*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	LONG	PcalDqRxComp( KSGAIN Rx, KSGAIN max_cur, KSGAIN vdc )
{
#if (FLOAT_USE==TRUE)
	float	fw;
#else
	LONG	kx, sx;
#endif /* FLOAT_USE */
	LONG	rc;
#if (FLOAT_USE==TRUE)
	fw = Rx * 35791.0F;
	fw = fw * max_cur / vdc;
	if( fw > (float)0x01000000 )
	{
		rc = 0x01000000;
	}
	else
	{
		rc = (LONG)fw;
	}
#else
	kx = MlibScalKskxkx( Rx, 35791,       1,        &sx,   0 );
	rc = MlibPcalKxksks( kx, max_cur,	  vdc,		&sx, -24 );
#endif /* FLOAT_USE */

	return( rc );
}



/****************************************************************************************************/
/*																									*/
/*		�d�l�e�⏞�萔(��)�̌v�Z																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*			   Nmax * 1.4142 * Emf * 2^15      2^13													*/
/*		kx = ------------------------------ * -------												*/
/*			               15000               Vdc/2												*/
/*																									*/
/*		Nmax [rad/s]      : Max. Speed																*/
/*		Emf  [Vrms/rad/s] : EMF Constant															*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	LONG	PcalDqEmfComp( KSGAIN Emf, KSGAIN over_spd, KSGAIN vdc )
{
#if (FLOAT_USE==TRUE)
	float	fw;
#else
	LONG	kx, sx;
#endif /* FLOAT_USE */
	LONG	rc;

#if (FLOAT_USE==TRUE)
	fw = Emf * over_spd / vdc;
	fw = fw * 50617.0F;
	if( fw > (float)0x01000000 )
	{
		rc = 0x01000000;
	}
	else
	{
		rc = (LONG)fw;
	}
#else
	kx = MlibScalKsksks( Emf, over_spd,    vdc,		&sx,   0 );
	rc = MlibPcalKxgain( kx,  50617,       1,       &sx, -24 );
#endif /* FLOAT_USE */

	return( rc );
}




/****************************************************************************************************/
/*																									*/
/*		�`�r�h�b�}�C�N���p�d���Q�C���p�����[�^�v�Z													*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/****************************************************************************************************/
void	PcalMicroDqAcrGain(	ASIC_U_IF_STR	*pAsicMicroReg,	/* ����ϐ��|�C���^						*/
							DQACR_GAIN_PRM	*Prm,			/* �p�����[�^�f�[�^�|�C���^				*/
							BPRMDAT			*Bprm,			/* �x�[�X�p�����[�^�|�C���^ 			*/
							ULONG			Loophz,			/* ���x���[�v�Q�C��			[0.1Hz]		*/
							ULONG			ratio)			/* �Q�C���o�����X			[%]			*/
{

	LONG	kp, ki;
	LONG	kx, sx;
	LONG	Kxd, Kxq, Txd, Txq;
	LONG	kfil, Tfx;
	LONG	MinKpGain;
#if (FLOAT_USE==TRUE)
	float Fwd, Fwq, fw;
#endif /* FLOAT_USE */

	MinKpGain = 900;							/* �d�����[�v�Q�C���ŏ��l	[Hz]					*/

/*--------------------------------------------------------------------------------------------------*/
/*	�d���Q�C���A�����s����																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( ratio >= 2000 )
	{	/* �䗦��2000%�ȏ�̏ꍇ�́A�d���Q�C���A������	*/
		/* �܂��́A�g���N����ݒ�͖���					*/
		Kxd = Prm->ignd;
		Kxq = Prm->ignq;
		Txd = Prm->kid;
		Txq = Prm->kiq;
		Tfx = Prm->ifil;
	}
	else
	{
/*--------------------------------------------------------------------------------------------------*/
/*		�d���Q�C��[Hz]�̌v�Z																			*/
/*																									*/
/*				    Kv[0.1Hz]           ratio[%]													*/
/*		Kxd[Hz] =  ----------- �~ 4 �~ ---------- 													*/
/*				       10                 100														*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
		Fwd = (float)Loophz * 4 * (float)ratio /1000;
		Fwq = Fwd;

		/*	���~�b�g���� (�p�����[�^���ő�l�Ƃ��Ďg�p����)	*/
		if( Fwd < (float)MinKpGain )
		{
			Fwd = (float)MinKpGain;
		}
		if( Fwd > (float)Prm->ignd )
		{ /* �ő�l���ŏ��l�̏ꍇ�͍ő�l�ɃN�����v */
			Fwd = (float)Prm->ignd;
		}
		if( Fwq < (float)MinKpGain )
		{
			Fwq = (float)MinKpGain;
		}
		if( Fwq > (float)Prm->ignq )
		{
			Fwq =(float) Prm->ignq;
		}

#else
		Kxd = MlibScalKxgain( (LONG)Loophz,   (4 * (LONG)ratio),   1000,  NULL, -24 );
		Kxq = Kxd;


		/*	���~�b�g���� (�p�����[�^���ő�l�Ƃ��Ďg�p����)	*/
		if( Kxd < MinKpGain )
		{
			Kxd = MinKpGain;
		}
		if( Kxd > (LONG)Prm->ignd )
		{ /* �ő�l���ŏ��l�̏ꍇ�͍ő�l�ɃN�����v */
			Kxd = Prm->ignd;
		}
		if( Kxq < MinKpGain )
		{
			Kxq = MinKpGain;
		}
		if( Kxq > (LONG)Prm->ignq )
		{
			Kxq = Prm->ignq;
		}
#endif /* FLOAT_USE */
		/* �d�����[�v�ϕ����萔�A���Ȃ�	*//*<V242>*/
		Txd = Prm->kid;
		Txq = Prm->kiq;

/*--------------------------------------------------------------------------------------------------*/
/*		�d���t�B���^���萔[us]�̌v�Z																*/
/*																									*/
/*			�d���Q�C���ŏ��l��PnEA4�ɂȂ�A�d���Q�C���ő�l(PnE21)�Ńt�B���^���萔=0�ƂȂ�X��		*/
/*																									*/
/*																									*/
/*				            Prm.ifil2[us]		   													*/
/*		Tfx[us] =  --------------------------------- �~ (Kxq[Hz] - MinKpGain[Hz]) + Prm.ifil2[us]	*/
/*				     (MinKpGain[Hz] - Prm.ignq)[Hz])												*/
/*--------------------------------------------------------------------------------------------------*/
		if( MinKpGain != Prm->ignq )
		{
#if (FLOAT_USE==TRUE)
			fw = (float)Prm->ifil2 * ((float)Kxq - (float)MinKpGain) / ((float)MinKpGain - (float)Prm->ignq);
			(float)Tfx = fw + (float)Prm->ifil2;
#else
			kx = MlibScalKxgain( Prm->ifil2, (Kxq - MinKpGain), (MinKpGain - Prm->ignq), &sx, -24 );
			Tfx = kx + Prm->ifil2;
#endif /* FLOAT_USE */
		}
		else
		{
			Tfx = Prm->ifil2;
		}
		if( Tfx < 0 )
		{
			Tfx = 0;
		}
		if( Tfx > Prm->ifil2 )
		{
			Tfx = Prm->ifil2;
		}
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�p�����[�^�v�Z																				*/
/*--------------------------------------------------------------------------------------------------*/
#ifndef _JL076_USE_
	/*----------------------------------------------------------------------------------------------*/
	/*		d���d���Q�C��																			*/
	/*----------------------------------------------------------------------------------------------*/
	/* ���Q�C�� */
	kp = PcalDqAcrKp( Kxd, 100, Bprm, D_AXIS );
	LpxSetPrmToASIC( &pAsicMicroReg->IntAdP.KdP, kp, 1 );

	/*----------------------------------------------------------------------------------------------*/
	/* �ϕ��Q�C�� */
	ki = PcalDqAcrKi( Txd, kp, Bprm->MotLd, Bprm );
	LpxSetPrmToASIC( &pAsicMicroReg->IntAdP.KdI, ki, 1 );

	/*----------------------------------------------------------------------------------------------*/
	/*		q���d���Q�C��																			*/
	/*----------------------------------------------------------------------------------------------*/
	/* ���Q�C�� */
	kp = PcalDqAcrKp( Kxq, 100, Bprm, Q_AXIS );
	LpxSetPrmToASIC( &pAsicMicroReg->IntAdP.KqP, kp, 1 );

	/*----------------------------------------------------------------------------------------------*/
	/* �ϕ��Q�C�� */
	ki = PcalDqAcrKi( Txq, kp, Bprm->MotLq, Bprm );
	LpxSetPrmToASIC( &pAsicMicroReg->IntAdP.KqI, ki, 1 );

	/*----------------------------------------------------------------------------------------------*/
	/*		�d�����[�v�o�̓t�B���^���萔															*/
	/*----------------------------------------------------------------------------------------------*/
	/* �o�̓t�B���^�Q�C�� */
	kfil = PcalDqAcrFilter( Tfx );
	LpxSetPrmToASIC( &pAsicMicroReg->IntAdP.Tfil, kfil, 1 );

#else /* _JL076_USE_ */

	/*----------------------------------------------------------------------------------------------*/
	/*		d���d���Q�C��																			*/
	/*----------------------------------------------------------------------------------------------*/
	/* ���Q�C�� */
	kp = PcalDqAcrKp( Kxd, 100, Bprm, D_AXIS );
	LpxSetPrmToASIC( (SHORT*)&pAsicMicroReg->MREG_KDP, kp, 1 );

	/*----------------------------------------------------------------------------------------------*/
	/* �ϕ��Q�C�� */
	ki = PcalDqAcrKi( Txd, kp, Bprm->MotLd, Bprm );
	LpxSetPrmToASIC( (SHORT*)&pAsicMicroReg->MREG_KDI, ki, 1 );

	/*----------------------------------------------------------------------------------------------*/
	/*		q���d���Q�C��																			*/
	/*----------------------------------------------------------------------------------------------*/
	/* ���Q�C�� */
	kp = PcalDqAcrKp( Kxq, 100, Bprm, Q_AXIS );
	LpxSetPrmToASIC( (SHORT*)&pAsicMicroReg->MREG_KQP, kp, 1 );

	/*----------------------------------------------------------------------------------------------*/
	/* �ϕ��Q�C�� */
	ki = PcalDqAcrKi( Txq, kp, Bprm->MotLq, Bprm );
	LpxSetPrmToASIC( (SHORT*)&pAsicMicroReg->MREG_KQI, ki, 1 );

	/*----------------------------------------------------------------------------------------------*/
	/*		�d�����[�v�o�̓t�B���^���萔															*/
	/*----------------------------------------------------------------------------------------------*/
	/* �o�̓t�B���^�Q�C�� */
	kfil = PcalDqAcrFilter( Tfx );
	LpxSetPrmToASIC( (SHORT*)&pAsicMicroReg->MREG_TFIL, kfil, 1 );

#endif /* _JL076_USE_ */
 
}



/****************************************************************************************************/
/*																									*/
/*		�I���f�B���C���x���̌v�Z																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*			   Level * 15000																		*/
/*		kx = -----------------		��		= limit( Level *150 ) :0 �`15000�Ń��~�b�g				*/
/*			       Imax       	 																	*/
/*																									*/
/*		Level [%] : �I���f�B���C�؂�ւ����x��														*/
/*		Imax  [%] : �ő�d��[%] (=�ő�g���N[%])													*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	LONG	PcalOndelayLevel( LONG Level )
{
	LONG	kx;

	kx = MlibLimitul( (Level * 150), 15000, 0 );

	return( kx );
}



/****************************************************************************************************/
/*																									*/
/*		�I���f�B���C�⏞��`�X���̌v�Z																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*			     2^8 * 2^13 	     2^8 * 2^13 * 1000	   139810   								*/
/*		kx = -------------------- = ------------------- = -------- 									*/
/*			  15000 * Slope/1000       15000 * Slope        Slope  									*/
/*																									*/
/*		Odslp [0.1%] : �⏞��100%�ƂȂ�d��															*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	LONG	PcalOndelaySlope( LONG slope )
{
#if (FLOAT_USE==TRUE)
	float fw;

	if( slope != 0 )
	{
		fw = 139810.0F / (float)slope;
	}
	else
	{
		fw = 139810.0F;
	}

	return( fw );

#else
	LONG	kx;

	if( slope != 0 )
	{
		kx = 139810 / slope;
	}
	else
	{
		kx = 139810;
	}

	return( kx );

#endif /* FLOAT_USE */

}



/****************************************************************************************************/
/*																									*/
/*		�o�v�l�L�����A���g���ύX�T�[�r�X		<V356>												*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : PWM���g�����I�����C���ŕύX����															*/
/*			������PnE2C�ݒ艺���l�������w�肳�ꂽ�ꍇ�́A�uPnF1E bit2�F�L�����A���g���I���v			*/
/*			�ɏ]���Đݒ肷��																		*/
/*			PnE2C�ݒ艺���l�ȏ�̏ꍇ�́A�L�����A���g����ݒ�ɕύX����								*/
/*			���ύX�����L�����A���g�������ɖ߂����͈����Ɂh�O�h��ݒ肷��							*/
/*																									*/
/****************************************************************************************************/
#define		PWM_FREQ_LOWER_LIMIT		2000					/* �L�����A���g�������l				*/
/****************************************************************************************************/
LONG	KpiChangePwmFrequency( ULONG PwmFx, USHORT pwmf, USHORT pwmf3, USHORT flg_wf )
{
	LONG	Tc;

	if( PwmFx < PWM_FREQ_LOWER_LIMIT )
	{ /* �p�����[�^�������� */
		if( (flg_wf & 0x0002 )							/* ��2�L�����A���g���I���t���O		*/
			&& (pwmf3 >= PWM_FREQ_LOWER_LIMIT)	)		/* ��2�L�����A���g���ݒ肠��		*/
		{
			Tc = PcalPwmFrequency( pwmf3 );				/* ��2�L�����A���g���ݒ�			*/
		}
		else
		{
			Tc = PcalPwmFrequency( pwmf );				/* ��1�L�����A���g���ݒ�			*/
		}
	}
	else
	{ /* �w����g������ */
		Tc = PcalPwmFrequency( PwmFx );					/* �w��L�����A���g���ݒ�			*/
	}

	return( Tc );

}



/****************************************************************************************************/
/*																									*/
/*		�o�v�l�L�����A���g���J�E���g�l�̌v�Z														*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*			   40000000																				*/
/*		Tc = -----------																			*/
/*				  Fc																				*/
/*																									*/
/*		Fc [Hz] : �o�v�l���g��																		*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	LONG	PcalPwmFrequency( LONG PwmFx )
{
	LONG	Tc;

	switch ( PwmFx )
	{
#if 0 /* �b��I��80MHz�ݒ�(�オ���K) */
		case 10667 : Tc = 15000; break;						/* PwmFc = 10.666666... [kHz]			*/
		case  8000 : Tc = 20000; break;						/* PwmFc =  8.0 [kHz]					*/
		case  4000 : Tc = 40000; break;						/* PwmFc =  4.0 [kHz]					*/
#else
		case 10667 : Tc = 3750; break;						/* PwmFc = 10.666666... [kHz]			*//*<V324>*/
		case  8000 : Tc = 5000; break;						/* PwmFc =  8.0 [kHz]					*/
		case  5333 : Tc = 7500; break;						/* PwmFc =  5.333333... [kHz]			*//*<V324>*/
		case  4000 : Tc = 10000; break;					/* PwmFc =  4.0 [kHz]					*/
		case  3556 : Tc = 11250; break;					/* PwmFc =  3.555555... [kHz]			*//*<V324>*/
#endif
		case	 0 : Tc = (ASIC_CLKMHZ/2) * 1000000; break;	/* for escape 0div						*/
		default    : Tc = (ASIC_CLKMHZ/2) * 1000000 / PwmFx; break;	/* PwmFc = Other				*/
	}

	return( Tc );

}



/****************************************************************************************************/
/*																									*/
/*		�o�v�l�L�����A���g���̌v�Z						���[�^�L�����A���g���؂�ւ�<S117>						*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*			   40000000																				*/
/*		Fc = -----------																			*/
/*				  Tc																				*/
/*																									*/
/*		Tc [-] : �L�����A�J�E���g�l																	*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	LONG	PcalPwmCounter( LONG CarrCnt )
{
	LONG	Fc;

	switch ( CarrCnt )
	{
#if 0 /* �b��I��80MHz�ݒ�(�オ���K) */
		case 15000 : Tc = 10667; break;						/* PwmFc = 10.666666... [kHz]			*/
		case 20000 : Tc =  8000; break;						/* PwmFc =  8.0 [kHz]					*/
		case 40000 : Tc =  4000; break;						/* PwmFc =  4.0 [kHz]					*/
#else
		case  3750 : Fc = 10667; break;						/* PwmFc = 10.666666... [kHz]			*//*<V324>*/
		case  5000 : Fc = 8000; break;							/* PwmFc =  8.0 [kHz]					*/
		case  7500 : Fc = 5333; break;							/* PwmFc =  5.333333... [kHz]			*//*<V324>*/
		case 10000 : Fc = 4000; break;							/* PwmFc =  4.0 [kHz]					*/
		case 11250 : Fc = 3556; break;							/* PwmFc =  3.555555... [kHz]			*//*<V324>*/
#endif
		case	 0 : Fc = (ASIC_CLKMHZ/2) * 1000000; break;	/* for escape 0div						*/
		default    : Fc = (ASIC_CLKMHZ/2) * 1000000 / CarrCnt; break;	/* PwmFc = Other				*/
	}

	return( Fc );

}



/****************************************************************************************************/
/*																									*/
/*		�d���t�B�[�h�o�b�N���[�v�ϕ����~�b�g�̌v�Z													*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*							 15000																	*/
/*		TvLim = Prm.TvLim * -------																	*/
/*							  100																	*/
/*																									*/
/*		Prm.TvLim [%] : �d���t�B�[�h�o�b�N���[�v�ϕ����萔�p�����[�^ (PnEB2)						*/
/*																									*/
/****************************************************************************************************/
static	LONG	PcalVFBKiLim( LONG TvLim )
{
	LONG	kx;

	kx = TvLim * 150;

	return( kx );
}



/* 2012.10.26 Y.Oka ���d���I�u�U�[�o���󖢑Ή��˃p�����[�^�ꊇ�������ݑΉ��v�� */
/****************************************************************************************************/
/*																									*/
/*		�d���I�u�U�[�o�p�����[�^�v�Z																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		�I�u�U�[�o�Q�C��1	: Ts/L																	*/
/*				   scantime[ns]		     1	          Bprm.Vdc[Vop]/2          15000     			*/
/*		wk_TSPL = -------------- * --------------- * ----------------- * ------------------ * 2^7	*/
/*					  10^9			Bprm.MotR[��]         2^13            Bprm.MaxCur[Aop]			*/
/*																									*/
/*		�I�u�U�[�o�Q�C��2	: 1 - R*L/Ts - exp( -2*pi*Kpi )											*/
/*                       Bprm.MotR[��]     scantime[ns]												*/
/*		wk_GOBS = 2^8 - --------------- * -------------- * 2^8										*/
/*                       Bprm.MotLq[H]        10^9													*/
/*																									*/
/*                                                  scantime[ns]									*/
/*					- 2^8 * exp( -2*pi * Kpi[Hz] * -------------- )									*/
/*                                                      10^9										*/
/*																									*/
/*		�I�u�U�[�o�Q�C��3	: 1 - R*L/Ts															*/
/*                       Bprm.MotR[��]     scantime[ns]												*/
/*		wk_GOBS = 2^4 - --------------- * -------------- * 2^4										*/
/*                       Bprm.MotLq[H]        10^9													*/
/*																									*/
/*		�t�B���^�Q�C��		: Ts / ( Ts + Tfil )													*/
/*		                       scantime[ns]															*/
/*		wk_FILOBS = ----------------------------------- * 16384										*/
/*			                               10^9														*/
/*					 scantime[ns] + -----------------												*/
/*					                 2*pi * Pwmf[Hz]												*/
/*																									*/
/*		scantime( KPI_MACYCLENS )	: �d���������[ns]												*/
/*		Bprm.MotLq					: q�����[�^�C���_�N�^���X[H]									*/
/*		Bprm.MotR					: q�����[�^��R�l[��]											*/
/*		Bprm.MaxCur					: �ő�d��[Aop]													*/
/*		Bprm.Vdc					: �ő�d��[Vop]													*/
/*		Kpi							: q���d�����[�v�Q�C��[Hz]	( PnE20 : Prm.ignq )				*/
/*		Pwmf						: �L�����A���g��[Hz]		( PnE2C : Prm.pwmf )				*/
/****************************************************************************************************/
static	void	KpiPcalCurrentObs( ASICS *SvAsicRegs, USHORT ignq, USHORT pwmf, BPRMDAT *Bprm, SHORT *MpReg)
{
	LONG	n0,n1,n2;							/* bit�V�t�g��										*/
	LONG	kx, sx;								/* work�p�ϐ�										*/
	LONG	kx2, sx2;							/* work�p�ϐ�2										*/
	LONG	kx3, sx3;							/* work�p�ϐ�3										*/
	LONG	kx4, sx4;							/* work�p�ϐ�4										*/
	LONG	MaxCur;								/* �ő�d��[Aop]									*/
	LONG	Vdc;								/* �ő�d��[Vop]									*/
	LONG	MotLq;								/* q�����[�^�C���_�N�^���X[H]						*/
	LONG	MotR;								/* q�����[�^��R�l[��]								*/
	USHORT	wk_TSPL;							/* TSPL work�p�ϐ�									*/
	USHORT	wk_GOBS;							/* GOBS work�p�ϐ�									*/
	USHORT	wk_RLTS;							/* RLTS work�p�ϐ�									*/
	USHORT	wk_FILOBS;							/* FILOBS work�p�ϐ�								*/

	n0 = 1 << (7+8);		/* 2^(7+2) */
	n1 = 1 << (8+8);		/* 2^(8+2) */
	n2 = 1 << (4+8);		/* 2^(4+2) */
	MaxCur	= Bprm->MaxCur;


	Vdc		= Bprm->Vdc;
	MotLq	= Bprm->MotLq;
	MotR	= Bprm->MotR;

	/* TSPL�̌v�Z	: Ts/L */
	kx		= MlibScalKxgain( KPI_MACYCLENS,	15,			2,				&sx,	0	);
	kx		= MlibPcalKxgain( kx,				n0,			8192,			&sx,	0	);
	kx		= MlibPcalKxkskx( kx,				Vdc,		1000000,		&sx,	0	);
	kx		= MlibPcalKxkxks( kx,				1,			MotLq,			&sx,	0	);
	wk_TSPL = (USHORT)MlibPcalKxkxks( kx,		1,			MaxCur,			&sx,	-24	);

	/* GOBS�̌v�Z	: 1 - R*Ts/L - g = 1 - R*Ts/L - exp( -2*pi*Ts*w ) */
	/* ( g = 1 - x + x^2/2! - x^3/3! + x^4/4! ) */
	/* x : 2*pi*Ts*w */
	/* R*Ts/L */
	kx		= MlibScalKxksks( n1,		MotR,				MotLq,			&sx,	0	);
	kx		= MlibPcalKxgain( kx,		KPI_MACYCLENS,		1000000000,		&sx,	0	);

	/* 1 */
	kx2		= MlibScalKxgain( n1,		1,					1,				&sx2,	0	);

	/* 1 - R*Ts/L */
	kx2		= MlibPcalKxsubx( kx2,		sx2,	kx,		sx,		&sx2 );

	/*  exp( -2*pi*Ts*w ) : 4���̃e�[���[�W�J�ŋߎ� */
	/* x : 2*pi*Ts*w �̌v�Z */
	kx		= MlibScalKxgain( 62831853,		ignq,	10000000,		&sx,	0	);
	kx		= MlibPcalKxgain( kx,			KPI_MACYCLENS,	1000000000, 	&sx,	0	);
	kx3		= kx;
	sx3		= sx;
	/* exp() ��1�� : 1 */
	kx4		= MlibScalKxgain( 1,		1,		1,		&sx4,	0 );
	/* exp() ( ��1�� - ��2�� ) : 1 - x */
	kx4		= MlibPcalKxsubx( kx4,		sx4,	kx, 	sx,	&sx4	);
	/* exp() ��3�� : x^2/2! */
	sx		+= sx ;
	kx		= MlibPcalKxgain( kx,	kx,	2,		&sx,	0	);
	/* exp() ( ��1�� - ��2�� + ��3�� ) : 1 - x + x^2/2! */
	kx4		= MlibPcalKxaddx( kx4,		sx4,	kx,		sx,  &sx4	);
	/* exp() ��4�� : x^3/3! */
	sx		+= sx3;
	kx		= MlibPcalKxgain( kx,	kx3,	3,		&sx,	0	);
	/* exp() ( ��1�� - ��2�� + ��3�� - ��4�� ) : 1 - x + x^2/2! - x^3/3! */
	kx4		= MlibPcalKxsubx( kx4,		sx4,	kx,		sx,  &sx4	);
	/* exp() ��5�� : x^4/4! */
	sx		+= sx3;
	kx		= MlibPcalKxgain( kx,	kx3,	4,		&sx,	0	);
	/* exp() ( ��1�� - ��2�� + ��3�� - ��4�� + ��5�� ) : 1 - x + x^2/2! - x^3/3! + x^4/4! */
	kx4		= MlibPcalKxaddx( kx4,		sx4,	kx,		sx,  &sx4	);
	/*  exp( -2*pi*Ts*w )*2^8  */
	kx4		= MlibPcalKxgain( kx4,	n1,	1,		&sx4,	0	);
	/* 1 - R*Ts/L - exp( -2*pi*Ts*w ) */
	kx2		= MlibPcalKxsubx( kx2,		sx2,	kx4,	sx4,	&sx2 );
	wk_GOBS	= (USHORT)MlibPcalKxgain( kx2,		1,		1,		&sx2,	-24	 );

	/* RLTS�̌v�Z	: 1 - R*Ts/L */
	/* R*Ts/L */
	kx		= MlibScalKxksks( n2,		MotR,				MotLq,			&sx,	0	);
	kx		= MlibPcalKxgain( kx,		KPI_MACYCLENS,		1000000000,		&sx,	0	);
	/* 1 */
	kx2		= MlibScalKxgain( n2,		1,					1,				&sx2,	0	);
	/* 1 - R*Ts/L */
	kx2		= MlibPcalKxsubx( kx2,		sx2,	kx,		sx,		&sx2 );
	wk_RLTS	= (USHORT)MlibPcalKxgain( kx2,		1,		1,		&sx2,	-24	 );

	/* FILOBS�̌v�Z	: Ts/(Tf+Ts) */
	/* Tf : 10^6 / ( PWMF*2*pi ) */
	kx		= MlibScalKxgain( 1000000,	1,			pwmf,		&sx,	0	);
	kx		= MlibPcalKxgain( kx,		10000000,	62831853,	&sx,	0	);
	kx		= MlibPcalKxgain( kx,		1000,		1,			&sx,	-24	);
	/* Ts/(Ts+Tf) */
	wk_FILOBS	= (USHORT)MlibPcalKxgain( KPI_MACYCLENS,	16384,	KPI_MACYCLENS+kx,	&sx,	-24	);


#if 0 /* 2012.08.31 Y.Oka ����ScanA��CpuScanA�� */
	if( ( wk_GOBS == 0 ) || ( wk_TSPL == 0 ) || ( wk_RLTS == 0 ) )
	{
		tbuf[0] = 0;											/* TSPL 			*/
		tbuf[1] = 0;											/* GOBS 			*/
		tbuf[2] = 0;											/* RLTS 			*/
		tbuf[3] = wk_FILOBS;									/* FILOBS 			*/
		MicroTranslatePrmReq( SvAsicRegs, tbuf, MpReg, 4 );
	}
	else
	{
		tbuf[0] = wk_TSPL;										/* TSPL 			*/
		tbuf[1] = wk_GOBS;										/* GOBS 			*/
		tbuf[2] = wk_RLTS;										/* RLTS 			*/
		tbuf[3] = wk_FILOBS;									/* FILOBS 			*/
		MicroTranslatePrmReq( SvAsicRegs, tbuf, MpReg, 4 );
	}
#else
	if( ( wk_GOBS == 0 ) || ( wk_TSPL == 0 ) || ( wk_RLTS == 0 ) )
	{
		MpReg[0] = 0;											/* TSPL 			*/
		MpReg[1] = 0;											/* GOBS 			*/
		MpReg[2] = 0;											/* RLTS 			*/
		MpReg[3] = wk_FILOBS;									/* FILOBS 			*/
	}
	else
	{
		MpReg[0] = wk_TSPL;										/* TSPL 			*/
		MpReg[1] = wk_GOBS;										/* GOBS 			*/
		MpReg[2] = wk_RLTS;										/* RLTS 			*/
		MpReg[3] = wk_FILOBS;									/* FILOBS 			*/
	}
#endif

}



/****************************************************************************************************/
/*																									*/
/*		�`�r�h�b�}�C�N���F�g���N�t�B���^�Ǘ�														*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ :	�`�r�h�b�}�C�N���v���O�����̃g���N�t�B���^�̂n�m�^�n�e�e�Ǘ����s���B					*/
/*																									*/
/*	�� �� :	�ʏ�A�p�����[�^�v�Z���[�`������b�����������B										*/
/*			�g���N�t�B���^�̔r�����䂪�K�v�ȏꍇ�́A�{���W���[���őΏ����邱�ƁB					*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
void KpiManageMicroTorqueFilter( VUSHORT *mreg_csw_ptr, USHORT *CswData, USHORT FilSel, LONG FilOn )
{
	/* Get Current Status */
	*CswData = *mreg_csw_ptr;

/*--------------------------------------------------------------------------------------------------*/
/*		Set/Clear Filter Disable Flag																*/
/*--------------------------------------------------------------------------------------------------*/
	if( FilOn == TRUE )
	{
		*CswData &= ~FilSel;			/* Clear Filter Disable Flag					*/
	}
	else
	{
		*CswData |= FilSel;				/* Set   Filter Disable Flag					*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		Set to ASIC Micro Register																	*/
/*--------------------------------------------------------------------------------------------------*/
	*mreg_csw_ptr = *CswData;

}



/****************************************************************************************************/
/*																									*/
/*		�Q���m�b�`�t�B���^�̃p�����[�^�v�Z															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*			     2*(wx^2 - hx^2)				  hx^2 + dx*wx*hx/qx + wx^2  						*/
/*		K1 = ------------------------   	K2 = ---------------------------						*/
/*			  hx^2 + wx*hx/qx + wx^2               hx^2 + wx*hx/qx + wx^2							*/
/*																									*/
/*																									*/
/*			  hx^2 - wx*hx/qx + wx^2 			  hx^2 - dx*wx*hx/qx + wx^2							*/
/*		K3 = ------------------------   	K4 = ---------------------------						*/
/*			  hx^2 + wx*hx/qx + wx^2               hx^2 + wx*hx/qx + wx^2							*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
void	KpiPcalMicroNotchFilter2(				/* �Q���m�b�`�t�B���^�̃p�����[�^�v�Z				*/
		ASICS	*SvAsicRegs,					/* JL-076�A�N�Z�X�p�\����							*/
		LONG	hz,								/* �t�B���^���g��		[Hz]						*/
		LONG	qx,								/* �t�B���^�p�萔		[0.01]						*/
		LONG	dx,								/* �t�B���^�[��			[0.001]						*/
		SHORT	*MpReg	)						/* �t�B���^�p�����[�^�ݒ�擪�A�h���X				*/
{
	LONG	ts;
	LONG	hx;
	LONG	wx;
	LONG	cx[3];
	LONG	freq;
	USHORT	kf[4];

/*--------------------------------------------------------------------------------------------------*/
/*		�p�����[�^�v�Z																				*/
/*--------------------------------------------------------------------------------------------------*/
	if( qx < 50 )
	{ /* Lower Limit for qx */
		qx = 50;
	}
	if( dx > 1000 )
	{ /* Upper Limit for dx */
		dx = 1000;
	}
	/*----------------------------------------------------------------------------------------------*/
	/* �t�B���^���Z�����ݒ� */
	/*----------------------------------------------------------------------------------------------*/
	ts = KPI_MBCYCLENS/100;
	/*----------------------------------------------------------------------------------------------*/
	/* ���g���␳																					*/
	/*----------------------------------------------------------------------------------------------*/
	if ( hz > 2000 )
	{
		freq = (LONG)( (SHORT)hz * (SHORT)(3 * hz - 1894) + 11788000) / 10000;
	}
	else
	{
		freq = hz;
	}
	/*----------------------------------------------------------------------------------------------*/
	hx = 10000000 / ts;											/* hx = (2/ts)*(1/2)				*/
	wx = (freq * 12868) >> 12;									/* wx = (hz*2*PAI)*(1/2)			*/
	/*----------------------------------------------------------------------------------------------*/
	while( hx > 23170 )
	{ /* Scaling : hx^2 < 2^29 */
		hx = hx>>1; wx = wx>>1;
	}
	while( wx > 23170 )
	{ /* Scaling : wx^2 < 2^29 */
		hx = hx>>1; wx = wx>>1;
	}
	/*----------------------------------------------------------------------------------------------*/
	cx[0] = MlibScalKxgain( 100*wx, hx, qx,   NULL, -30 );		/* cx[0] = wx*hx/qx					*/
	cx[1] = MlibScalKxgain( cx[0],  dx, 1000, NULL, -30 );		/* cx[1] = dx*wx*hx/qx				*/
	cx[2] = hx*hx + cx[0] + wx*wx;								/* cx[2] = hx*hx + wx*hx/qx + wx*wx	*/
	/*----------------------------------------------------------------------------------------------*/
	kf[0] = (USHORT)MlibScalKxgain( 2*(wx*wx - hx*hx),       (1<<13), cx[2], NULL, -24 );	/* K1	*/
	kf[1] = (USHORT)MlibScalKxgain( (hx*hx + cx[1] + wx*wx), (1<<13), cx[2], NULL, -24 );	/* K2	*/
	kf[2] = (USHORT)MlibScalKxgain( (hx*hx - cx[0] + wx*wx), (1<<13), cx[2], NULL, -24 );	/* K3	*/
	kf[3] = (USHORT)MlibScalKxgain( (hx*hx - cx[1] + wx*wx), (1<<13), cx[2], NULL, -24 );	/* K4	*/
/*--------------------------------------------------------------------------------------------------*/
/*		�}�C�N���h�e���W�X�^�ւ̏�������															*/
/*--------------------------------------------------------------------------------------------------*/
#if 0 /* 2012.09.04 Y.Oka ���ʃv���O�����ւ̃p�����[�^�ꊇ�������݋@�\�ǉ��K�v�� */
	MicroTranslatePrmReq( SvAsicRegs, kf, MpReg, 4 );
#else /* �b��Ή� */
	MpReg[0]	= (USHORT)kf[0];						/* Write k[0] to ASIC Micro Register		*/
	MpReg[1]	= (USHORT)kf[1];						/* Write k[1] to ASIC Micro Register		*/
	MpReg[2]	= (USHORT)kf[2];						/* Write k[2] to ASIC Micro Register		*/
	MpReg[3]	= (USHORT)kf[3];						/* Write k[3] to ASIC Micro Register		*/
#endif
/*--------------------------------------------------------------------------------------------------*/

}



/****************************************************************************************************/
/*																									*/
/*		�P�����[�p�X�t�B���^�̃p�����[�^�v�Z														*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*				Ts  																				*/
/*		kf = --------- * 2^14																		*/
/*			  Tf + Ts   																			*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	void	KpiPcalMicroLpassFilter1(		/* �P�����[�p�X�t�B���^�̃p�����[�^�v�Z				*/
		LONG	tf,								/* �t�B���^���萔		[xs]						*/
		LONG	ts,								/* �t�B���^���Z����		[xs]						*/
		SHORT	*MpReg )						/* �t�B���^�p�����[�^�ݒ�擪�A�h���X				*/
{
	LONG	kf;

	kf = MlibScalKxgain( ts, (1<<14), (ts + tf), NULL, -24 );
	*MpReg = (USHORT)kf;						/* Write kf to ASIC Micro Register					*/

}



/****************************************************************************************************/
/*																									*/
/*		�`�r�h�b�ւ̃p�����[�^�ݒ�(������)															*/
/*																									*/
/****************************************************************************************************/
LONG	LpxSetPrmToASIC( SHORT *adr, LONG prm, BOOL limchk )
{
	USHORT	rdata;
	VUSHORT	dummy;
	LONG	prmx;
	LONG	rc;

	prmx = prm;
	rc = 0;

/*--------------------------------------------------------------------------------------------------*/
/*		Check Parameter Limit																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( limchk == REQ_LMT_CHK )
	{
		if( (prm > 32767) || (prm < 0) )				/* Limit Check								*/
		{
			prmx = MlibLimitul( prm, 32767, 0 );		/* Limit : 0 <= prm <= 32767				*/
			/* 2009.06.04 Y.Oka (>_<) �p�����[�^���~�b�g�K�v */
//			KpiSetPrmLmtInfo( pdef );					/* �p�����[�^���~�b�g���̐ݒ�				*/
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		Write to ASIC and Verify																	*/
/*--------------------------------------------------------------------------------------------------*/
	*adr	= (USHORT)prmx;								/* Write to ASIC Register					*/
	dummy = *adr;										/*	KPX_ASIC_DUMMY_READ( dummy );			*/
	rdata = *adr;										/* Read from ASIC Register					*/
	if( rdata != (USHORT)prmx )							/* Verify Write Data						*/
	{
		*adr	= (USHORT)prmx;							/* Write to ASIC Register					*/
		dummy = *adr;									/*	KPX_ASIC_DUMMY_READ( dummy );			*/
		rdata = *adr;									/* Read from ASIC Register					*/
		if( rdata != (USHORT)prmx )						/* Verify Write Data						*/
		{
			rc = -1;									/* A.BF2 : �V�X�e���A���[���Q(Micro�ُ�)	*/
		}
	}
/*--------------------------------------------------------------------------------------------------*/
	return(rc);

}


#if 0 /* 2012.08.31 Y.Oka ����ScanA��CpuScanA�� */
/****************************************************************************************************/
/*																									*/
/*		�}�C�N���p�����[�^�ꊇ�]���v������															*/
/*																									*/
/*			��Size�̍ő�l��32�܂�																	*/
/*																									*/
/****************************************************************************************************/
LONG	MicroTranslatePrmReq( ASICS *SvAsicRegs, void *Data, VUSHORT *Address, LONG Size )
{
	LONG			i;
	USHORT			*TbufAddress;
	ASIC_U_IF_STR	*pAsicMicroReg;
	ULONG			Timout;

	pAsicMicroReg = SvAsicRegs->AsicMicroReg;

	/* Check Micro Program Running Status */
	if( SvAsicRegs->MicroRunningSts == TRUE )
	{
		/* Check Request Data Size */
		if( Size > MICRO_TR_DATA_MAX )
		{
			return	MICRO_TR_DATA_ERROR;
		}

		/* Check Src Data Address and Dst Data Address */
		if( (Data == NULL) || (Address == NULL) )
		{
			return	MICRO_TR_DATA_ERROR;
		}

		/* Check and Wait Finish Last Micro Translation */
		Timout = 0;
		while( pAsicMicroReg->MREG_TR_END != MICRO_TR_READY )
		{
			/* Wait for Ready */
			KlibWaitus( KPI_SACYCLEUS );
			if((++Timout) > (KPI_SCCYCLEMS*KPI_SACOUNTMS))
			{
				return	MICRO_TR_DATA_ERROR;
			}
		}

		/* Make Target Address (Micro Address) */
		pAsicMicroReg->MREG_TADDR = (USHORT)( ((LONG)Address & MICRO_ADDRESS_CHANGE) >> 1 );

		/* Set Parameter Data */
		TbufAddress = (USHORT*)&(pAsicMicroReg->MREG_TBUF_0);
		for( i=0; i<Size; i++ )
		{
			TbufAddress[i] = ( (USHORT*)Data )[i];
		}

		/* Set Prameter Number (2byte unit) */
		pAsicMicroReg->MREG_TNUM = (USHORT)Size;

		/* Set Parameter Renewal Request (2byte unit) */
		pAsicMicroReg->MREG_TR_END = MICRO_TR_SIZE_SHORT;

		/* �T�[�{ASIC�s�No.3 �Ή�(�_�~�[���C�g) *//* 2008.04.02 */
		pAsicMicroReg->MREG_DUMMY = 0x0000;									/* �_�~�[���C�g				*/
	}
	else
	{ /* Micro Program Stopped State */
		for( i=0; i<Size; i++ )
		{
			( (USHORT*)Address )[i] = ( (USHORT*)Data )[i];
		}
	}
	return	MICRO_TR_SUCCESS;
}
#endif



/***************************************** end of file **********************************************/
