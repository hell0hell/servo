/****************************************************************************************************/
/*																									*/
/*																									*/
/*		IniPrmCalc.c : �����p�����[�^�v�Z������`													*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �d�������������p�����[�^�v�Z����														*/
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
/*	Note	:	����	2010.02.23	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"
#include "MercuryGlobals.h"
#include "AlarmManager.h"
#include "MLib.h"
#include "MicroPrmCalc.h"
#include "SysConfig.h"
#include "BaseControlPrmCalc.h"
#include "ExCtrlPrmCalc.h"
#include "PnPrmListTbl.h"
#include "TuneLib.h"
#include "FnPrgJog.h"
#include "FnEasyFFT.h"
#include "ModelControl.h"
#include "JL086ServoIpHw.h"/* <S004> */
#include "RippleComp.h"	/* <S1FD> */

/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
/*	void		pIprmcalMain( AXIS_HANDLE *Axis, LONG caller_id );									*/
/*	void		LpxPcalKioManager( LONG ax_no );													*/
/****************************************************************************************************/
static	void	LpxPcalSpdDetMaFilter( AXIS_HANDLE *Axis );		/* ���[�^���x���o�p�ړ����Ϗ�����	*/
static	void	IprmcalElectronicGear( AXIS_HANDLE *Axis );		/* �ʒu�w�ߓd�q�M�����Z				*/
static	void	IprmcalAlarmMask( AXIS_HANDLE *Axis );			/* �A���[���}�X�N�ݒ�(PnE00)		*/
static	void	IprmcalSystemSwitch( AXIS_HANDLE *Axis );		/* �V�X�e���X�C�b�`�ݒ�(PnE01)		*/
static	void	IprmcalVdetfilSwitch( AXIS_HANDLE *Axis );		/* �d�����o�t�B���^�I��SW�ݒ�(PnE7A)*/
static	void	IprmcalACInput( AXIS_HANDLE *Axis );			/* AC�d�����͑I��(PnE0C,Pn00B)		*/
static	void	IprmcalBitParam000( AXIS_HANDLE *Axis );		/* �@�\�I����{SW(Pn000)			*/
static	void	IprmcalBitParam001( AXIS_HANDLE *Axis );		/* �@�\�I�����pSW(Pn001)			*/
static	void	IprmcalBitParam002( AXIS_HANDLE *Axis );		/* �@�\�I�����pSW(Pn002)			*/
static	void	IprmcalBitParam008( AXIS_HANDLE *Axis );		/* �@�\�I�����pSW(Pn008)			*/
static	void	IprmcalBitParam009( AXIS_HANDLE *Axis );		/* �@�\�I�����p�X�C�b�`(Pn009)		*/
static	void	IprmcalBitParam00A( AXIS_HANDLE *Axis );		/* �@�\�I�����p�X�C�b�`(Pn00A)		*/    /* <S1F5> */
static	void	IprmcalBitParam00B( AXIS_HANDLE *Axis );		/* �@�\�I�����p�X�C�b�`(Pn00B)		*/
static	void	IprmcalBitParam00D( AXIS_HANDLE *Axis );		/* �@�\�I�����p�X�C�b�`(Pn00D)		*/
static	void	IprmcalBitParam00E( AXIS_HANDLE *Axis );		/* �@�\�I�����p�X�C�b�`(Pn00E)		*//* <S21D> */
static	void	IprmcalBitParam00F( AXIS_HANDLE *Axis );		/* �@�\�I�����p�X�C�b�`(Pn00F)		*/	/* < S154 > */
static	void	IprmcalBitParam10B( AXIS_HANDLE *Axis );		/* �Q�C���֌W���pSW(Pn10B)			*/
static	void	IprmcalBitParam200( AXIS_HANDLE *Axis );		/* �ʒu����w�ߌ`�ԑI��SW(Pn200)	*/
static	void	IprmcalBitParam207( AXIS_HANDLE *Axis );		/* �ʒu����@�\SW(Pn207)			*/
#if (CSW_SVCF_BACKLASHCOMP == TRUE)
static	void	IprmcalBitParam230( AXIS_HANDLE *Axis );		/* �ʒu����g���@�\�X�C�b�`(Pn230)	*/
#endif
//static	void	IprmcalBitParam408( AXIS_HANDLE *Axis );		/* �g���N�֌W�@�\SW(Pn408)		*/
static	void	IprmcalBitVelRippleCompSW( AXIS_HANDLE *Axis );	/* ���x���b�v���⏞�@�\�I��SW(Pn423)*/	/* <S14B> */
static	void	IprmcalVelRippleCompSpd( AXIS_HANDLE *Axis );	/* ���x���b�v���⏞�@�\�I��SW(Pn423)*/	/* <S14B> */

static	void	IprmcalBitParam514( AXIS_HANDLE *Axis );		/* �o�͐M���I��4(Pn514)				*//* <S21D> */
static	void	IprmcalVoltageCheckLevel( AXIS_HANDLE *Axis );	/* �c�b�d���d���Ď����x���v�Z		*/
static	void	IprmcalInstEnvMon( AXIS_HANDLE *Axis );			/* �ݒu�����j�^�p�p�����[�^�v�Z	*//* <S03A> */
static	void	IprmcalOverLoadLevelAmpMot( AXIS_HANDLE *Axis );/* �A���v�����[�^�ߕ��׃��x���̌v�Z	*/
static	void	IprmcalMotorOverrun( AXIS_HANDLE *Axis );		/* ���[�^�\���Ď��p�����[�^�v�Z		*/
static	void	IprmcalMaxBbonTimeDiff( AXIS_HANDLE *Axis );	/* HWBB�^�C�~���O�ُ�p�����[�^�v�Z */
static	void	IprmcalSeqInputMap( AXIS_HANDLE *Axis );		/* <S045> �V�[�P���X���̓}�b�v�v�Z	*/
/* SGD7W IO���o�͑Ή�<S118> */
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W�i2���Ή��j�̂�	*/
static	void	LpxPcalSetInputMap2( AXIS_HANDLE *Axis, SEQ_IOSIGNAL *IoSignal, 	/* <S045> �V�[�P���X���̓}�b�v�v�Z�i7W�p�j	*/
						ULONG prm, ULONG LogicH );
#elif CSW_BOARD_TYPE == SGD7S_TYPE	/*	SGD7S�i1���Ή��j�̂�	*/
static	void	LpxPcalSetInputMap( SEQ_IOSIGNAL *IoSignal, 	/* <S045> �V�[�P���X���̓}�b�v�v�Z 	*/
						ULONG prm, ULONG LogicH ); 
#endif
static	void	IprmcalSeqOutputMap( AXIS_HANDLE *Axis );		/* <S045> �V�[�P���X�o�̓}�b�v�v�Z	*/
/* SGD7W IO���o�͑Ή�<S118> */
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W�i2���Ή��j�̂�	*/
static  void	LpxPcalSetSoutMap2( AXIS_HANDLE *Axis, SEQ_IOSIGNAL *IoSignal, 		/* �V�[�P���X�o�̓}�b�v�v�Z			*//* <S04B> */
						ULONG prm, ULONG OppositeReq );
#elif CSW_BOARD_TYPE == SGD7S_TYPE	/*	SGD7S�i1���Ή��j�̂�	*/
//static  void	LpxPcalSetSoutMap( SEQ_IOSIGNAL *IoSignal, 		/* <S045> �V�[�P���X�o�̓}�b�v�v�Z	*//* <S04B> */
//						ULONG prm ); 
static  void	LpxPcalSetSoutMap( SEQ_IOSIGNAL *IoSignal, 		/* �V�[�P���X�o�̓}�b�v�v�Z			*//* <S04B> */
						ULONG prm, ULONG OppositeReq );
#endif

/*--------------------------------------------------------------------------------------------------*/
/* SGD7W IO���o�͑Ή�2<S1B5> 																		*/
/*--------------------------------------------------------------------------------------------------*/
static	void	LpxSetInputMapCmn( AXIS_HANDLE *Axis, LONG Mode );						/* <S1B5> */
static	void	LpxSetInputMapW( AXIS_HANDLE *Axis );									/* <S1B5> */
static  void	LpxPcalSetInputMap3( SEQ_IOSIGNAL *IoSignal, ULONG prm, ULONG LogicH );	/* <S1B5> */
static	void	LpxSetOutputMapCmn( AXIS_HANDLE *Axis );								/* <S1B5> */
static	void	LpxSetOutputMapW( AXIS_HANDLE *Axis );									/* <S1B5> */
static  void	LpxPcalSetSoutMap3( SEQ_IOSIGNAL *IoSignal, ULONG prm, ULONG LogicH );	/* <S1B5> */
static  LONG	LpxGetInSel( LONG Axis, ULONG prm );									/* <S1B5> */
static  void	LpxInitLatchSignal( AXIS_HANDLE *Axis );								/* <S1B5> */
/*--------------------------------------------------------------------------------------------------*/
static	void	IprmcalPcmdOvrSpd( AXIS_HANDLE *Axis );			/* �ʒu�w�߉ߑ��x(�n�r)�̌v�Z		*/
static	void	IprmcalZeroStopControl( AXIS_HANDLE *Axis );	/* �[������~����p�����[�^�v�Z		*/
static	void	IprmcalCarrerChange( AXIS_HANDLE *Axis );		/* �L�����A�؂�ւ��p�����[�^�v�Z	*/
static	void	IprmcalTorqueLevel( AXIS_HANDLE *Axis );		/* �g���N���x���̌v�Z				*/
static	void	IprmcalOtMask( AXIS_HANDLE *Axis );				/* �I�[�o�g���x���}�X�N�b��I�I�I<S032>*/
static	void	IprmcalPowerCircuitSetting( AXIS_HANDLE *Axis );/* �p���[��֘A�ݒ�				*/
static	void	IprmcalCopyParamToIprm( AXIS_HANDLE *Axis );	/* �R�s�[�p�����[�^(Prm --> Iprm)	*/
static	void	LpxPcalPcmpCalGain( AXIS_HANDLE	*Axis );		/* �ʑ��⏞�萔�̌v�Z				*/
static	void	IprmcalVoltageCheckTime( AXIS_HANDLE *Axis );	/* DC�d���d���Ď����Ԍv�Z <S053>	*/
static	void	IprmcalFanLockDetectTime( AXIS_HANDLE *Axis );	/* �����t�@����~���o���Ԍv�Z		*//* <S0A3> */
static	void	IprmcalSpdFbFil( AXIS_HANDLE *Axis );			/* ���[�^�É��p���x�e�aLPF�����ݒ� <S0DA>	*/
static  void	AdetCheckMotorOverSpeed(PRMDATA *Prm, BPRMDAT *Bprm, LONG AxisMotType );	/* <S1D0> */
static void	LpxMakeCarrCnt( PRMDATA *Prm, CARRCHNG	*CarrerChange );	/* �L�����A���g���J�E���g�쐬����	*/
static	void	IprmcalFanLockAlmDetectTime( AXIS_HANDLE *Axis );	/* �T�[�{�I���������t�@����~�A���[�����o���Ԑݒ�	*//* <S219> */


/* �O���֐� */
extern void InitInterruptStart( void );
extern LONG KpiPcalPerClrMode( PLSIFV *pPls, LONG data );

/****************************************************************************************************/
/*																									*/
/*		�������ʃp�����[�^�v�Z���C��(���j�b�g����)													*/
/*																									*/
/****************************************************************************************************/
void	IprmcalMainCommon( AXIS_HANDLE *Axis )
{
	IprmcalVdetfilSwitch( Axis );				/* �d�����o�t�B���^�I���X�C�b�`�ݒ�(PnE7A)			*/
	IprmcalACInput( Axis );						/* AC�d�����͑I��(PnE0C,Pn00B)						*/

	IprmcalVoltageCheckLevel( Axis );			/* DC�d���d���Ď����x���v�Z							*/
	IprmcalPowerCircuitSetting( Axis );			/* �p���[��֘A�ݒ�								*/

	IprmcalVoltageCheckTime( Axis );			/* DC�d���d���Ď����Ԍv�Z <S053>					*/

	IprmcalFanLockDetectTime( Axis );			/* �����t�@����~���o���Ԍv�Z						*//* <S0A3> */
}

/****************************************************************************************************/
/*																									*/
/*		�������ʃp�����[�^�v�Z���C���i���ˑ��j														*/
/*																									*/
/****************************************************************************************************/
void	IprmcalMainAxis( AXIS_HANDLE *Axis )
{
	BOOL			enable;
	LONG			PrmSetting;
	TUNELESS_CFGPRM	TuneLessCfgPrm;
#if ( CSW_SVCF_MFC_USE == TRUE )
	MFC_CFG_PRM 	MfcCfgPrm;
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		�����{,�P�ʕϊ��p�����[�^																	*/
/*--------------------------------------------------------------------------------------------------*/
	IprmcalElectronicGear( Axis );					/* �ʒu�w�ߓd�q�M�����Z							*/

/*--------------------------------------------------------------------------------------------------*/
/*		�@�\�I���r�b�g�p�����[�^																	*/
/*--------------------------------------------------------------------------------------------------*/
	IprmcalAlarmMask( Axis );						/* �A���[���}�X�N�ݒ�(PnE00)					*/
	IprmcalSystemSwitch( Axis );					/* �V�X�e���X�C�b�`�ݒ�(PnE01)					*/
//	IprmcalVdetfilSwitch( Axis );					/* �d�����o�t�B���^�I���X�C�b�`�ݒ�(PnE7A)		*/
//	IprmcalACInput( Axis );							/* AC�d�����͑I��(PnE0C,Pn00B)					*/
	IprmcalBitParam000( Axis );						/* �@�\�I����{�X�C�b�`(Pn000)					*/
	IprmcalBitParam001( Axis );						/* �@�\�I�����p�X�C�b�`(Pn001)					*/
	IprmcalBitParam002( Axis );						/* �@�\�I�����p�X�C�b�`(Pn002)					*/
	IprmcalBitParam008( Axis );						/* �@�\�I�����p�X�C�b�`(Pn008)					*/
	IprmcalBitParam009( Axis );						/* �@�\�I�����p�X�C�b�`(Pn009)					*/
	IprmcalBitParam00A( Axis );						/* �@�\�I�����p�X�C�b�`(Pn00A)					*/   /* <S1F5> */
	IprmcalBitParam00B( Axis );						/* �@�\�I�����p�X�C�b�`(Pn00B)					*/
	IprmcalBitParam00D( Axis );						/* �@�\�I�����p�X�C�b�`(Pn00D)					*/
	IprmcalBitParam00E( Axis );						/* �@�\�I�����p�X�C�b�`(Pn00E)					*//* <S21D> */
	IprmcalBitParam10B( Axis );						/* �Q�C���֌W���p�X�C�b�`(Pn10B)				*/
	IprmcalBitParam00F( Axis );						/* �@�\�I�����p�X�C�b�`(Pn00F)					*/	/* < S154 > */
	IprmcalBitParam200( Axis );						/* �ʒu����w�ߌ`�ԑI���X�C�b�`(Pn200)			*/
	IprmcalBitParam207( Axis );						/* �ʒu����@�\�X�C�b�`(Pn207)					*/
#if (CSW_SVCF_BACKLASHCOMP == TRUE)
	IprmcalBitParam230( Axis );						/* �ʒu����g���@�\�X�C�b�`(Pn230)				*/
#endif
//#if( FRICTRQ_COMP != 0 )	/*<S0CD>*/
	 Axis->BaseControls->FrictionM.InitEnable = TRUE;					/* <S0CD>���C�g���N�␳�@�\�֘A�p�����[�^�v�Z			*//*<S007>*/
//#endif /*<S0CD>*/
	//	IprmcalBitParam408( Axis );						/* �g���N�֌W�@�\�X�C�b�`(Pn408)			*/

	IprmcalBitVelRippleCompSW( Axis );				/* ���x���b�v���⏞�@�\�I��SW(Pn423)			*/	/* <S14B> */
	IprmcalVelRippleCompSpd( Axis );				/* ���x���b�v���⏞�L�����x(Pn427)				*/	/* <S14B> */
	IprmcalBitParam514( Axis );						/* �o�͐M���I��4(Pn514)						*//* <S21D> */

/*--------------------------------------------------------------------------------------------------*/
/*		�U�����o���x�I�u�U�[�o�p�p�����[�^�v�Z														*/
/*--------------------------------------------------------------------------------------------------*/
	DetVibObsCalculateInitPrm( &(Axis->DetVib->DetVibObs),
							   Axis->UniPrms->Bprm->Kvx,
							   Axis->UniPrms->Prm->jrate,
							   Axis->UniPrms->Bprm->SvCycleNs );

/*--------------------------------------------------------------------------------------------------*/
/*		�������X�@�\�I��																			*/
/*--------------------------------------------------------------------------------------------------*/
	/* �������X�@�\�I���X�C�b�`(Pn170/Pn14F) */
	TuneLessCfgPrm.gnmode	= (Axis->UniPrms->Prm)->gnmode;
	TuneLessCfgPrm.mfctype	= (Axis->UniPrms->Prm)->mfctype;
	TuneLessCfgPrm.avibsw	= (Axis->UniPrms->Prm)->avibsw;
	TuneLessCfgPrm.DatLevel	= (Axis->UniPrms->Prm)->DatLevel;
	TuneLessCfgPrm.tfuncsw 	= (Axis->UniPrms->Prm)->tfuncsw;
	TuneLessCfgPrm.flg_wf 	= (Axis->UniPrms->Prm)->MencP.flg_wf;
	PrmSetting = TuneLessCalculateInitPrm( Axis->BaseControls,
										   Axis->UniPrms->Bprm,
										   Axis->MencV,
										   Axis->SvAsicRegs,
										   &TuneLessCfgPrm );
	if( PrmSetting & PRMERR_TUNELESS_LVL )
	{
		ALMSetPramError( Axis->AlmManager, pndef_DatLevel.RegNumber );
	}
	if( PrmSetting & PRMERR_TUNELESS_TYPE )
	{
		ALMSetPramError( Axis->AlmManager, pndef_mfctype.RegNumber );
	}
	/* �������X�p�g���N�⏞��g���N�t�B���^�I�� */
	Axis->BaseControls->CtrlCmdPrm.LpassFil3 = TuneLessSetTrqFil3(
						Axis->BaseControls->TuneLessCtrl.conf.TuningLessUse,
						Axis->BaseControls->TuneLessCtrl.conf.TuningLessEx,
						(Axis->UniPrms->Prm)->MencP.flg_wf,
						Axis->MencV->TuningLessLowGainMotor );

#if ( CSW_SVCF_MFC_USE == TRUE )
/*--------------------------------------------------------------------------------------------------*/
/*		���f���Ǐ]����@�\�I��																		*/
/*--------------------------------------------------------------------------------------------------*/
	PcalCopyMFCCfgParam(&MfcCfgPrm, Axis->UniPrms->Prm);
	if( FALSE != MfcCalculateInitPrm( &(Axis->BaseControls->MFControl),
									  &MfcCfgPrm,
									  Axis->UniPrms->Prm->jrate,
									  Axis->UniPrms->Bprm,
									  Axis->UniPrms->Prm->mfctype) )
	{
		ALMSetPramError( Axis->AlmManager, pndef_mfctype.RegNumber );
	}
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		�A���[���Ď��֘A�p�����[�^																	*/
/*--------------------------------------------------------------------------------------------------*/
	/* �c�b�d���d���Ď����x���v�Z */
//	IprmcalVoltageCheckLevel( Axis );
	/* �A���v�����[�^�ߕ��׃��x���̌v�Z */
	IprmcalOverLoadLevelAmpMot( Axis );
	/* ���[�^�\���Ď��p�����[�^�v�Z */
	IprmcalMotorOverrun( Axis );
	/* ���͐M���^�C�~���O�ُ�p�����[�^�v�Z */
	IprmcalMaxBbonTimeDiff( Axis );
	/* �ݒu�����j�^�p�p�����[�^�v�Z *//* <S03A> */
	IprmcalInstEnvMon( Axis );

/*--------------------------------------------------------------------------------------------------*/
/*		�V�[�P���X���o�̓}�b�v																		*/
/*--------------------------------------------------------------------------------------------------*/
	IprmcalSeqInputMap( Axis );					/* <S045> �V�[�P���X���̓}�b�v�v�Z					*/
	IprmcalSeqOutputMap( Axis );				/* <S045> �V�[�P���X�o�̓}�b�v�v�Z					*/

/*--------------------------------------------------------------------------------------------------*/
/*		����֘A�p�����[�^																			*/
/*--------------------------------------------------------------------------------------------------*/
	IprmcalSpdFbFil( Axis );					/* <S0DA> ���[�^�É��p���x�e�aLPF�����ݒ�		*/
	IprmcalPcmdOvrSpd( Axis );					/* �ʒu�w�߉ߑ��x(�n�r)�̌v�Z					*/
	IprmcalZeroStopControl( Axis );				/* �[������~����p�����[�^�v�Z					*/

	/* �U�����g����͏����ݒ� */
	FftCalculateInitPrm( Axis->FftAna, (Axis->UniPrms->Bprm)->SvCycleUs );

	/* �L�����A���g���؂�ւ��p�����[�^�v�Z */
	IprmcalCarrerChange( Axis );

	/* �����m�b�`�p�����ݒ� */
	AutoNotchCalculateInitPrm( Axis->ANotchSeq,
							   Axis->ANotch,
							   Axis->MencV->AxisMotType,
							   Axis->UniPrms );

#if (CSW_SVCF_BACKLASHCOMP == TRUE)
	/* �o�b�N���b�V�␳�ʐ����l�v�Z */
	BlshCalcMaxBlshCompPulse( &(Axis->BaseControls->PosManager.BlshCmp),
							  &(Axis->BaseControls->PosManager.conf.Egear),
							  Axis->UniPrms->Bprm );
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		�v���O����JOG�����p�����[�^�v�Z																*/
/*--------------------------------------------------------------------------------------------------*/
	IprmcalPrgJog( &Axis->BaseControls->PJogHdl,
					Axis->UniPrms->Bprm,
				   &Axis->BaseControls->PosManager );

/*--------------------------------------------------------------------------------------------------*/
/*		���̑�																						*/
/*--------------------------------------------------------------------------------------------------*/
	IprmcalTorqueLevel( Axis );					/* �g���N���x���̌v�Z							*/
//	IprmcalVdetPrmChk( Axis );					/* ���H���o�p�����[�^�`�F�b�N					*/
	IprmcalOtMask( Axis );						/* �I�[�o�g���x���}�X�N�b��I�I�I�Ikira<S032>		*/
	IprmcalCopyParamToIprm( Axis );				/* �R�s�[�p�����[�^(Prm --> Iprm)				*/

	/* EasyFFT�����p�����[�^�v�Z */
	EasyFftInitialize( Axis->EasyFFT,
					   Axis->TableRefBuffer,
					   Axis->TraceBuffer,
					   Axis->UniPrms );

/*--------------------------------------------------------------------------------------------------*/
/*		Reset and Init motor speed ma filetr detas													*/
/*--------------------------------------------------------------------------------------------------*/
	LpxPcalSpdDetMaFilter( Axis );

/*----------------------------------------------------------------------------------------------*/
/*		PnE39 Bit2 : ���H�d���␳�@�\�I��															*/
/*----------------------------------------------------------------------------------------------*/
	enable = FALSE;
	if( ((Axis->UniPrms->Cprm->syssw2 >> 8) & 0x03) == VDET_AD )
	{
		enable = (0 != (Axis->UniPrms->Prm->dq_sw2 & 0x0004));
	}
	PcmKvvIniPrmCalc(&(Axis->BaseControls->MpKvvComp),
	                 Axis->UniPrms->Bprm,
	                 Axis->UniPrms->Cprm->kvvfil,
	                 Axis->UniPrms->Cprm->pnvoltgn,
	                 Axis->UniPrms->Prm->vrevgn,
	                 enable );

	/* �ʑ��⏞�萔�̌v�Z */
	LpxPcalPcmpCalGain( Axis );

/*<S1D0> START*/
/*----------------------------------------------------------------------------------------------*/
/*		���[�^�ō����x����																				*/
/*----------------------------------------------------------------------------------------------*/
	AdetCheckMotorOverSpeed( Axis->UniPrms->Prm, Axis->UniPrms->Bprm, Axis->MencV->AxisMotType);
/*<S1D0> START*/

/* <S219> Start */
/*----------------------------------------------------------------------------------------------*/
/*		 PnEE6 : �T�[�{�I���������t�@����~�A���[�����o���� 									*/
/*----------------------------------------------------------------------------------------------*/
	IprmcalFanLockAlmDetectTime( Axis );
/* <S219> End */


}


/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		�d�����o�t�B���^�I���X�C�b�`�ݒ�(PnE7A)														*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		PnE7A.Bit0,1 : ���H�d�����o�ُ팟�o�t�B���^	( 00-11 / 0us-750us , 250us���� )			*/
/*		PnE7A.Bit2,3 : �ߓd���A���[�����o�t�B���^		( 00-11 / 0us-750us , 250us���� )			*/
/*		PnE7A.Bit4,5 : �񐶓���J�n�t�B���^				( 00-11 / 0us-750us , 250us���� )			*/
/*		PnE7A.Bit6,7 : �񐶓���I���t�B���^				( 00-11 / 0us-750us , 250us���� )			*/
/*		PnE7A.Bit8-14: ��																			*/
/*		PnE7A.Bit15	 : ���H�`�c���o�i�t�B���^�O�j�g���[�X�I�� (=1:�g���[�X�I��0x05�Ńg���[�X�\)	*/
/*						���f�o�b�O�p																*/
/*--------------------------------------------------------------------------------------------------*/
void	IprmcalVdetfilSwitch( AXIS_HANDLE *Axis )
{
	LONG				swbuf;
	REGENE_CTRL			*RegCtrl;
	DET_DC_VOLT			*DetectDc;
	CHECK_MAIN_POWER	*PowerChecker;
	UNI_PRM_STR			*UniPrm;
	PCOV_ERR_DETECT		*PcovErrDetect;									/* <S053> */

	UniPrm = Axis->UniPrms;
	swbuf = UniPrm->Cprm->vdefilsw;
	RegCtrl = &(Axis->PowerManager->RegeneCtrl);
	DetectDc = &(Axis->PowerManager->DetectDcVolt);
	PowerChecker = &(Axis->PowerManager->MainPowerChecker);
	PcovErrDetect= &(Axis->PowerManager->PcovErrDetect);				/* <S053> */

/*--------------------------------------------------------------------------------------------------*/
/*		�e��t�B���^�ݒ�																			*/
/*--------------------------------------------------------------------------------------------------*/
	DetectDc->conf.VdcdetAlmFilter		= swbuf & 0x0003;				/* ���H�ُ팟�o�t�B���^	*/
	PowerChecker->conf.OvdetAlmFilter	= ( swbuf >> 2 ) & 0x0003;		/* �ߓd�����o�t�B���^		*/
	RegCtrl->conf.RegStartFilter		= ( swbuf >> 4 ) & 0x0003;		/* �񐶓���J�n�t�B���^		*/
	RegCtrl->conf.RegStopFilter			= ( swbuf >> 6 ) & 0x0003;		/* �񐶓���I���t�B���^		*/
	

#if 0 /* 2010.06.25 Y.Oka INGRAM���ԓd�ʈُ팟�o�@�\���󖢑Ή� */
/*--------------------------------------------------------------------------------------------------*/
/*		���ԓd�ʈُ팟�o�@�\�L��			<V576>													*/
/*							���o���萔		<V582>													*/
/*--------------------------------------------------------------------------------------------------*/
	wk = ( swbuf >> 8 ) & 0x0f;

	if( wk == 0 )
	{
		Iprm->PcovEna = FALSE;								/* ���ԓd�ʈُ팟�o����					*/
	}
	else
	{
		Iprm.f.PcovEna = TRUE;								/* ���ԓd�ʈُ팟�o�L��					*/
	}
	Iprm.PcovErrCntLmt = wk * (100/KPI_SCCYCLEMS);			/* ���ԓd�ʈُ팟�o���萔				*/
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		<S053> ���ԓd�ʈُ팟�o���萔�C�@�\�L��/����												*/
/*--------------------------------------------------------------------------------------------------*/
	PcovErrDetect->conf.PcovErrCntLmt = (( swbuf >> 8 ) & 0x0f) * (100/KPI_SCCYCLEMS);

	if( PcovErrDetect->conf.PcovErrCntLmt == 0 )
	{
		PcovErrDetect->conf.PcovEna = FALSE;				/* ���ԓd�ʈُ팟�o����					*/
	}
	else
	{
		PcovErrDetect->conf.PcovEna = TRUE;					/* ���ԓd�ʈُ팟�o�L��					*/
	}
	
}


/****************************************************************************************************/
/*																									*/
/*		AC�d�����͑I��(PnE0C,Pn00B)																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		PnE0C.Bit1-2 : �T�[�{�p�b�N�̓d�����͎d�l													*/
/*		PnE0C.Bit3   : �d���������o����																*/
/*--------------------------------------------------------------------------------------------------*/
/*		Pn00B.2      : �O�����͎d�l�T�[�{�p�b�N�̓d�����͑I��	 									*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalACInput( AXIS_HANDLE *Axis )
{
	LONG				lwk;
	CMN_PRMDATA			*Cprm;
	PRMDATA				*Prm;
	CHECK_MAIN_POWER	*PowerChecker;
	CHECK_LOWOL			*pLowOL;		/* <S1F5> */

	Prm = Axis->UniPrms->Prm;
	Cprm = Axis->UniPrms->Cprm;		// �p�����[�^�ݒ�f�[�^
	PowerChecker = &(Axis->PowerManager->MainPowerChecker);
	pLowOL = &(Axis->CheckAlarm->AmpLowOL);					/* <S1F5> */

/*--------------------------------------------------------------------------------------------------*/
/*	Bit 1,2: AC�d�����͑I��																			*/
/*--------------------------------------------------------------------------------------------------*/
	switch( (Cprm->syssw2 >> 1) & 0x03 )
	{
		case 0x00:											/* �P���d�����͂̂݉\					*/
			PowerChecker->conf.Ac3Phase = FALSE;
			pLowOL->conf.BaseCurrentDerateEnable = FALSE;			/* <S1F5> */
			break;
		case 0x01:											/* �O���d�����͂̂݉\					*/
			PowerChecker->conf.Ac3Phase = TRUE;
			pLowOL->conf.BaseCurrentDerateEnable = FALSE;			/* <S1F5> */
			break;
			/* <S1F5> Start */
		case 0x02:
			lwk = (Prm->b_prmB>>8) & 0x0F;
			if( lwk <= 1 )
			{
				if(lwk == 0)
				{
					PowerChecker->conf.Ac3Phase = TRUE;		/* �O���d�����͂Ŏg�p����				*/
					pLowOL->conf.BaseCurrentDerateEnable = FALSE;
				}
				else
				{
					PowerChecker->conf.Ac3Phase = FALSE;	/* �P���d�͓��͂Ŏg�p����				*/
					/* �P���d�����͑I�����̃x�[�X�d���f�B���[�e�B���O���� */
					pLowOL->conf.BaseCurrentDerateEnable = TRUE;
				}
			}
			else
			{
				PowerChecker->conf.Ac3Phase = TRUE;
				pLowOL->conf.BaseCurrentDerateEnable = FALSE;
				ALMSetPramError( Axis->AlmManager, pndef_b_prmB.RegNumber );
			}
			break;
			/* <S1F5> End */
		case 0x03:											/* �O�����͎d�l�ŒP���d�����͂��\		*/
			lwk = (Prm->b_prmB>>8) & 0x0F;
			if( lwk <= 1 )
			{
				if(lwk == 0)
				{
					PowerChecker->conf.Ac3Phase = TRUE;		/* �O���d�����͂Ŏg�p����				*/
					pLowOL->conf.BaseCurrentDerateEnable = FALSE;			/* <S1F5> */
				}
				else
				{
					PowerChecker->conf.Ac3Phase = FALSE;	/* �P���d�͓��͂Ŏg�p����				*/
					pLowOL->conf.BaseCurrentDerateEnable = FALSE;			/* <S1F5> */
				}
			}
			else
			{
				pLowOL->conf.BaseCurrentDerateEnable = FALSE;			/* <S1F5> */
				PowerChecker->conf.Ac3Phase = TRUE;
				ALMSetPramError( Axis->AlmManager, pndef_b_prmB.RegNumber );
			}
			break;
		default:											/* �\��									*/
			pLowOL->conf.BaseCurrentDerateEnable = FALSE;			/* <S1F5> */
			PowerChecker->conf.Ac3Phase = FALSE;
			ALMSetPramError( Axis->AlmManager, pndef_syssw2.RegNumber );
			break;
	}

/*--------------------------------------------------------------------------------------------------*/
/*	Bit 3: �d���������o����																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( Cprm->syssw2 & 0x0008 )
	{ /* PnE0C.bit3�őI�� */
		PowerChecker->conf.DetectAcOn2	= TRUE;
	}
	else
	{
		PowerChecker->conf.DetectAcOn2	= FALSE;
	}
	/*----------------------------------------------------------------------------------------------*/
	/*	�d���������o����:ACON1/ACON2�̏ꍇ�A3�����͎d�l�T�[�{�p�b�N�łȂ��Ȃ�p�����[�^�ݒ�ُ�		*/
	/*----------------------------------------------------------------------------------------------*/
	if( PowerChecker->conf.DetectAcOn2 )
	{
		if( ((Cprm->syssw2 >> 1) & 0x01) == 0 )							/* PnE0C.bit1���ڎQ�ƂɕύX	*/
		{
			ALMSetPramError( Axis->AlmManager, pndef_syssw2.RegNumber );
		}
	}
}


/****************************************************************************************************/
/*																									*/
/*		�@�\�I����{�X�C�b�`(Pn000)																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		Pn000.0 : ��]�����I��																		*/
/*		Pn000.1 : ��������I��																		*/
/*		Pn000.2 : ���A�h���X																		*/
/*		Pn000.3 : ��]�^�A���j�A�^�I��	2009.09.17 Y.Oka											*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalBitParam000( AXIS_HANDLE *Axis )
{
	PRMDATA		*Prm;
	BPRMDAT		*Bprm;

	Prm  = Axis->UniPrms->Prm;		// �p�����[�^�ݒ�f�[�^
	Bprm = Axis->UniPrms->Bprm;		// �����p�����[�^��`(�d���ē����ŕύX�L��)

/*--------------------------------------------------------------------------------------------------*/
/*		Pn000.0 : ��]�����I��(�t��]���[�h)														*/
/*--------------------------------------------------------------------------------------------------*/
	switch( Prm->b_prm0 & 0x0F )
	{
	case 0 :
		Bprm->RvsDir = FALSE;
		Bprm->DirSign = +1;
		break;
	case 1 :
		Bprm->RvsDir = TRUE;
		Bprm->DirSign = -1;
		break;
	default:
		ALMSetPramError( Axis->AlmManager, pndef_b_prm0.RegNumber );
		break;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		Pn000.1 : �������(���䃂�[�h)�I��															*/
/*--------------------------------------------------------------------------------------------------*/
// M3���ݒ肳���̂ō폜

/*--------------------------------------------------------------------------------------------------*/
/*		Pn000.3 : ��]�^�A���j�A�^�I��	2009.09.17 Y.Oka											*/
/*--------------------------------------------------------------------------------------------------*/
// �C�j�V���������ɂĐݒ�

}


/****************************************************************************************************/
/*																									*/
/*		�@�\�I�����p�X�C�b�`(Pn001)																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		Pn001.0 : �T�[�{�I�t�y�уA���[���������̒�~���@											*/
/*		Pn001.1 : �I�[�o�ƃ��x��(�n�s)���̒�~���@													*/
/*		Pn001.2 : �`�b�^�c�b�d�����͂̑I��															*/
/*		Pn001.3 : ���[�j���O�R�[�h�o�͂̑I��														*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalBitParam001( AXIS_HANDLE *Axis )
{
	PRMDATA				*Prm;
	BPRMDAT				*Bprm;
	CHECK_MAIN_POWER	*PowerChecker;
	ALM_STOP			*AlmStop;
	OT_SEQ_DATA			*OtSeqData;

	Prm  = Axis->UniPrms->Prm;		// �p�����[�^�ݒ�f�[�^
	Bprm = Axis->UniPrms->Bprm;		// �����p�����[�^��`(�d���ē����ŕύX�L��)
	PowerChecker = &(Axis->PowerManager->MainPowerChecker);
	AlmStop = &(Axis->BeSequence->AlmStop);
	OtSeqData = &(Axis->BeSequence->OtSeqData);

/*--------------------------------------------------------------------------------------------------*/
/*		Pn001.0 : �T�[�{�I�t�y�уA���[���������̒�~���@											*/
/*--------------------------------------------------------------------------------------------------*/
	switch( Prm->b_prm1 & 0x0F )
	{
		case 0x00: AlmStop->conf.StopMode = STOPMODE_DBSTOPDB; break;
		case 0x01: AlmStop->conf.StopMode = STOPMODE_DBSTOPFR; break;
		case 0x02: AlmStop->conf.StopMode = STOPMODE_FREERUN;  break;
		default:   ALMSetPramError( Axis->AlmManager, pndef_b_prm1.RegNumber ); break;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		Pn001.1 : �I�[�o�g���x��(OT)���̒�~���@													*/
/*--------------------------------------------------------------------------------------------------*/
	switch( (Prm->b_prm1>>4) & 0x0F )
	{
		case 0x00: OtSeqData->conf.OtStopMode = OTSTOPMODE_DEFAULT; break;
		case 0x01: OtSeqData->conf.OtStopMode = OTSTOPMODE_ZSTOPZC; break;
		case 0x02: OtSeqData->conf.OtStopMode = OTSTOPMODE_ZSTOPFR; break;
		case 0x03: OtSeqData->conf.OtStopMode = OTSTOPMODE_LDSTOPZC; break;    /* <S1F5> */
		case 0x04: OtSeqData->conf.OtStopMode = OTSTOPMODE_LDSTOPFR; break;    /* <S1F5> */
		default:   ALMSetPramError( Axis->AlmManager, pndef_b_prm1.RegNumber ); break;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		Pn001.2 : AC/DC�d�����͂̑I��																*/
/*--------------------------------------------------------------------------------------------------*/
/* 2013.01.07 Y.Oka �ȉ��R���p�C��SW�s�v */
//#if( SVFSEL_DCBUS == TRUE )	/* �����I��DC�d�����̓��[�h�Ƃ���ꍇ */
	switch( (Prm->b_prm1>>8) & 0x0F )
	{
	case 0x00: /* AC�d�����͑I�� */
		PowerChecker->conf.Dcbus = FALSE;								/* AC�d������				*/
		break;

	case 0x01: /* �c�b�d�����͑I��	*/
		if( Bprm->AmpInVcode == 0x00 )
		{ /* �{�d���d�l�A100�u���͎d�l�Ȃ��DC�d�����͑I��s�� */
			PowerChecker->conf.Dcbus = FALSE;
		}
		else
		{
			PowerChecker->conf.Dcbus = TRUE;							/* DC�d������				*/
		}
		break;
	default:
		PowerChecker->conf.Dcbus = FALSE;
		break;
	}
//#endif
/* 2013.01.07 Y.Oka �ȉ��R���p�C��SW�s�v */

//#if 0	/* <S1A8> : �uPn001.3 : ���[�j���O�R�[�h�o�͂̑I���v���A */
/*--------------------------------------------------------------------------------------------------*/
/*		Pn001.3 : ���[�j���O�R�[�h�o�͂̑I��														*/
/*--------------------------------------------------------------------------------------------------*/
	switch( (Prm->b_prm1>>12) & 0x0F )
	{
//		case 0x00: Iprm->WrnOutSel = FALSE; break;			/* <S1A8> */
//		case 0x01: Iprm->WrnOutSel = TRUE;  break;			/* <S1A8> */
		case 0x00: Axis->AlmManager->WrnOutSel = FALSE; break;	/* <S1A8> */
		case 0x01: Axis->AlmManager->WrnOutSel = TRUE;  break;	/* <S1A8> */
		default:   ALMSetPramError( Axis->AlmManager, pndef_b_prm1.RegNumber ); break;
	}
//#endif	/* <S1A8> : �uPn001.3 : ���[�j���O�R�[�h�o�͂̑I���v���A */
}


/****************************************************************************************************/
/*																									*/
/*		�@�\�I�����p�X�C�b�`(Pn002)																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		Pn002.0 : ���x����I�v�V����(T-REF���蓖��) 		 										*/
/*		Pn002.1 : �g���N����I�v�V����(V-REF���蓖��)												*/
/*		Pn002.2 : ��Βl�G���R�[�_�̎g�p���@														*/
/*		Pn002.3 : �t���N���[�Y�h�G���R�[�_�̎g�p���@												*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalBitParam002( AXIS_HANDLE *Axis )
{
	TRQ_LMT_DATA	*TrqLimits;
	PRMDATA			*Prm;

	TrqLimits = &(Axis->BaseControls->TrqLimitData);
	Prm  = Axis->UniPrms->Prm;		// �p�����[�^�ݒ�f�[�^

/*--------------------------------------------------------------------------------------------------*/
/*		Pn002.0 : ���x����I�v�V����(T-REF���蓖��) 		 										*/
/*--------------------------------------------------------------------------------------------------*/
	switch( Prm->b_prm2 & 0x0F )
	{
	case 0x00:	/* �Ȃ�							*/
//		TrqLimits->conf.TrefTrqFF    = FALSE;
		TrqLimits->conf.TrefTrqLmt   = FALSE;
		TrqLimits->conf.TrefTrqLmtCL = FALSE;
		break;
	case 0x01:	/* �O���g���N�������͂Ƃ��Ďg�p */
//		TrqLimits->conf.TrefTrqFF    = FALSE;
		TrqLimits->conf.TrefTrqLmt   = TRUE;
		TrqLimits->conf.TrefTrqLmtCL = FALSE;
		break;
	case 0x02:	/* �g���N�t�B�[�h�t�H���[�h���͂Ƃ��Ďg�p */
//		TrqLimits->conf.TrefTrqFF    = TRUE;
		TrqLimits->conf.TrefTrqLmt   = FALSE;
		TrqLimits->conf.TrefTrqLmtCL = FALSE;
		break;
	case 0x03:	/* �O���g���N��������(�����M��(P-CL,N-CL)�L����)�Ƃ��Ďg�p */
//		TrqLimits->conf.TrefTrqFF    = FALSE;
		TrqLimits->conf.TrefTrqLmt   = FALSE;
		TrqLimits->conf.TrefTrqLmtCL = TRUE;
		break;
	default:
		ALMSetPramError( Axis->AlmManager, pndef_b_prm2.RegNumber );
		break;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		Pn002.1 : �g���N����I�v�V����(V-REF���蓖��)												*/
/*--------------------------------------------------------------------------------------------------*/
	switch( (Prm->b_prm2>>4) & 0x0F )
	{
	case 0x00:	/* �Ȃ�							*/
		Axis->BaseControls->CtrlCmdPrm.VrefSpdLmt = FALSE;
		break;
	case 0x01:	/* �O�����x�������͂Ƃ��Ďg�p */
		Axis->BaseControls->CtrlCmdPrm.VrefSpdLmt = TRUE;
		break;
	default:
		ALMSetPramError( Axis->AlmManager, pndef_b_prm2.RegNumber );
		break;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		Pn002.2 : ��Βl�G���R�[�_�̎g�p���@			--> Sencif.c								*/
/*		Pn002.3 : �t���N���[�Y�h�G���R�[�_�̎g�p���@	--> Fencif.c								*/
/*--------------------------------------------------------------------------------------------------*/

}


/****************************************************************************************************/
/*																									*/
/*		�@�\�I�����p�X�C�b�`(Pn008)																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		Pn008.0 : �o�b�e���d���ቺ�̃A���[���^���[�j���O�I��										*/
/*		Pn008.1 : 									 		 										*/
/*		Pn008.2 : ���[�j���O���o�I��				 		 										*/
/*		Pn008.3 : 																					*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalBitParam008( AXIS_HANDLE *Axis )
{
	UV_TLIM_DATA	*UvTrqLimits;
//	IPRMDAT			*Iprm;
//	CMN_PRMDATA		*Cprm;
	PRMDATA			*Prm;
	ULONG			uswk;			/* <S0AA> */

	UvTrqLimits = &(Axis->BaseControls->TrqLimitData.UvTrqLimits);
//	Cprm = Axis->UniPrms->Cprm;		// �p�����[�^�ݒ�f�[�^
	Prm  = Axis->UniPrms->Prm;		// �p�����[�^�ݒ�f�[�^
//	Iprm = Axis->prm_hndl_ptr->iprm_ptr;		// �����p�����[�^��`(�d���ē����ŕύX�L��)

	/*--------------------------------------------------------------------------------------------------*/
	/*		�o�b�e���d���ቺ�̃A���[���^���[�j���O�I��													*/
	/*--------------------------------------------------------------------------------------------------*/
//	switch( Cprm->b_prm8 & 0x0F )
	switch( Prm->b_prm8 & 0x0F )
	{
	case 0x00:	/* �o�b�e���d���ቺ���A���[���Ƃ���		*/
		Axis->MencV->BatWrnMode = FALSE;
		break;
	case 0x01:	/* �o�b�e���d���ቺ�����[�j���O�Ƃ���	*/
		Axis->MencV->BatWrnMode = TRUE;
		break;
	default:	/* �p�����[�^�ݒ�ُ�					*/
		ALMSetPramError( Axis->AlmManager, pndef_b_prm8.RegNumber );
		break;
	}

	/*--------------------------------------------------------------------------------------------------*/
	/*		���H�d���~�����̋@�\�I��																	*//* <V200> */
	/*--------------------------------------------------------------------------------------------------*/
//	switch(( Cprm->b_prm8 >> 4 ) & 0x0F )
	switch(( Prm->b_prm8 >> 4 ) & 0x0F )
	{
	case 0x00:	/* ���H�d���~�����[�j���O�����o���Ȃ� */
		UvTrqLimits->conf.UvTrqLmtCtrl = FALSE;
		ALMSetMask( Axis->AlmManager, WRN_UV, TRUE );	/* A.971 : �s���d�����[�j���O		*/
		break;
	case 0x01:	/* ���H�d���~�����[�j���O�����o���� */
		UvTrqLimits->conf.UvTrqLmtCtrl = FALSE;
		ALMSetMask( Axis->AlmManager, WRN_UV, FALSE );	/* A.971 : �s���d�����[�j���O		*/
		break;
	case 0x02:	/* ���H�d���~�����[�j���O�����o���APn424,Pn425�ɂ��g���N�����������Ȃ� */
		UvTrqLimits->conf.UvTrqLmtCtrl = TRUE;
		ALMSetMask( Axis->AlmManager, WRN_UV, FALSE );	/* A.971 : �s���d�����[�j���O		*/
		break;
	default:	/* �p�����[�^�ݒ�ُ�					*/
		ALMSetPramError( Axis->AlmManager, pndef_b_prm8.RegNumber );
		break;
	}

	/*--------------------------------------------------------------------------------------------------*/
	/*		���[�j���O���o�I��																			*/
	/*--------------------------------------------------------------------------------------------------*/
//	switch( (Cprm->b_prm8>>8) & 0x0F )
	switch( (Prm->b_prm8>>8) & 0x0F )
	{
	case 0x00:	/* ���[�j���O�����o����		*/
		ALMSetMask( Axis->AlmManager, WRN_OLF,      FALSE );/* A.910 : �ߕ��׃��[�j���O					*/
		ALMSetMask( Axis->AlmManager, WRN_VIB,      FALSE );/* A.911 : ���U���[�j���O						*/
		ALMSetMask( Axis->AlmManager, WRN_CTRLBRD,  FALSE );/* A.912 : �������x�x��1�i�������x�x���j*//* <S0DE> */
		ALMSetMask( Axis->AlmManager, WRN_PWRBRD,   FALSE );/* A.913 : �������x�x��2�i�p���[����x�x�j*//* <S0DE> */
		ALMSetMask( Axis->AlmManager, WRN_RGOLF,    FALSE );/* A.920 : �񐶉ߕ��׃��[�j���O				*/
		ALMSetMask( Axis->AlmManager, WRN_DATA,     FALSE );/* A.940 : �f�[�^�ُ탏�[�j���O				*/
		ALMSetMask( Axis->AlmManager, WRN_CMD,      FALSE );/* A.950 : �R�}���h�ُ탏�[�j���O				*/
		ALMSetMask( Axis->AlmManager, WRN_OPT,      FALSE );/* A.9F0 : �I�v�V�����ُ탏�[�j���O			*/
		ALMSetMask( Axis->AlmManager, WRN_PGBAT,    FALSE );/* A.930 : ��Βl�G���R�[�_�o�b�e���ُ�		*/
		ALMSetMask( Axis->AlmManager, WRN_FPGBAT,   FALSE );/* A.930 : �e�o�f�o�b�e���ُ탏�[�j���O		*/
		ALMSetMask( Axis->AlmManager, WRN_OF,       FALSE );/* A.900 : �ʒu�΍��ߑ像�[�j���O				*/
		ALMSetMask( Axis->AlmManager, WRN_BEOF,     FALSE );/* A.901 : �T�[�{�I�����ʒu�΍��ߑ�			*/
		ALMSetMask( Axis->AlmManager, WRN_BOOT,     FALSE );/* A.941 : �d���f�v���[�U�萔�ύX���[�j���O	*/
		ALMSetMask( Axis->AlmManager, WRN_DBOL_LVL, FALSE );/* A.921 : �c�a�ߕ��׃��[�j���O				*/
//		ALMSetMask( Axis->AlmManager, WRN_RIPPLE,   FALSE );/* A.942 : ���b�v���␳���s��v���[�j���O	*//* <S1FD>�폜 */
		/* ---futami---> */
		ALMSetMask( Axis->AlmManager, WRN_MBOH,     FALSE );/* A.981 : ���j�b�g�������x�ߔM���[�j���O	*/
		ALMSetMask( Axis->AlmManager, WRN_CONVOH,   FALSE );/* A.982 : �R���o�[�^���x�ߔM���[�j���O		*/
		ALMSetMask( Axis->AlmManager, WRN_IFOH,     FALSE );/* A.983 : �������x�ߔM					*/
		/* <---futami--- */
		ALMSetMask( Axis->AlmManager, WRN_PREVENTMAINT, FALSE );/* A.9B0:�\�h�ۑS���[�j���O				*/ /* < S154 > */
		break;
	case 0x01:	/* ���[�j���O�����o���Ȃ�	*/
		ALMSetMask( Axis->AlmManager, WRN_OLF,      TRUE  );/* A.910 : �ߕ��׃��[�j���O					*/
		ALMSetMask( Axis->AlmManager, WRN_VIB,      TRUE  );/* A.911 : ���U���[�j���O						*/
		ALMSetMask( Axis->AlmManager, WRN_CTRLBRD,  TRUE  );/* A.912 : �������x�x��1�i�������x�x���j*//* <S0DE> */
		ALMSetMask( Axis->AlmManager, WRN_PWRBRD,   TRUE  );/* A.913 : �������x�x��2�i�p���[����x�x�j*//* <S0DE> */
		ALMSetMask( Axis->AlmManager, WRN_RGOLF,    TRUE  );/* A.920 : �񐶉ߕ��׃��[�j���O				*/
		ALMSetMask( Axis->AlmManager, WRN_DATA,     TRUE  );/* A.940 : �f�[�^�ُ탏�[�j���O				*/
		ALMSetMask( Axis->AlmManager, WRN_CMD,      TRUE  );/* A.950 : �R�}���h�ُ탏�[�j���O				*/
		ALMSetMask( Axis->AlmManager, WRN_OPT,      TRUE  );/* A.9F0 : �I�v�V�����ُ탏�[�j���O			*/
		ALMSetMask( Axis->AlmManager, WRN_PGBAT,    TRUE  );/* A.930 : ��Βl�G���R�[�_�o�b�e���ُ�		*/
		ALMSetMask( Axis->AlmManager, WRN_FPGBAT,   TRUE  );/* A.930 : �e�o�f�o�b�e���ُ탏�[�j���O		*/
		ALMSetMask( Axis->AlmManager, WRN_OF,       TRUE  );/* A.900 : �ʒu�΍��ߑ像�[�j���O				*/
		ALMSetMask( Axis->AlmManager, WRN_BEOF,     TRUE  );/* A.901 : �T�[�{�I�����ʒu�΍��ߑ�			*/
		ALMSetMask( Axis->AlmManager, WRN_BOOT,     TRUE  );/* A.941 : �d���f�v���[�U�萔�ύX���[�j���O	*/
		ALMSetMask( Axis->AlmManager, WRN_DBOL_LVL, TRUE  );/* A.921 : �c�a�ߕ��׃��[�j���O				*/
//		ALMSetMask( Axis->AlmManager, WRN_RIPPLE,   TRUE  );/* A.942 : ���b�v���␳���s��v���[�j���O	*//* <S1FD>�폜 */
		/* ---futami---> */
		ALMSetMask( Axis->AlmManager, WRN_MBOH,     TRUE  );/* A.981 : ���j�b�g�������x�ߔM���[�j���O	*/
		ALMSetMask( Axis->AlmManager, WRN_CONVOH,   TRUE  );/* A.982 : �R���o�[�^���x�ߔM���[�j���O		*/
		ALMSetMask( Axis->AlmManager, WRN_IFOH,     TRUE  );/* A.983 : �������x�ߔM					*/
		/* <---futami--- */
		ALMSetMask( Axis->AlmManager, WRN_PREVENTMAINT,   TRUE  );/* A.9B0:�\�h�ۑS���[�j���O			*/ /* < S154 > */
		break;
	default:	/* �p�����[�^�ݒ�ُ�		*/
		ALMSetPramError( Axis->AlmManager, pndef_b_prm8.RegNumber );
		break;
	}

	/*--------------------------------------------------------------------------------------------------*/
	/*		���[�j���O���o�}�X�N	<S0AA> SGDV�Ɠ��l�̏����ɕύX�B				 						*/
	/*--------------------------------------------------------------------------------------------------*/
//<S0AA>	ALMSetMask( Axis->AlmManager, WRN_BOOT, TRUE );			/* A.941 : �d���f�v���[�U�萔�ύX���[�j���O	*/
	uswk = (Prm->b_prm8>>12) & 0x0F;
	if( uswk <= 7 )
	{
		if( uswk & 0x01 )
		{
			ALMSetMask( Axis->AlmManager, WRN_BEOF, TRUE  ); 	/* A.901 : �T�[�{�I�����ʒu�΍��ߑ�		*/
		}
		if( uswk & 0x02 )
		{
			ALMSetMask( Axis->AlmManager, WRN_VIB, TRUE  ); 	/* A.911 : ���U���[�j���O				*/
		}
		if( uswk & 0x04 )
		{
			ALMSetMask( Axis->AlmManager, WRN_BOOT, TRUE );		/* A.941 : �d���f�v���[�U�萔�ύX���[�j���O		*/
		}
	}
	else
	{
		ALMSetPramError( Axis->AlmManager, pndef_b_prm8.RegNumber );
	}
	/*--------------------------------------------------------------------------------------------------*/

	/*--------------------------------------------------------------------------------------------------*/
	/*		���[�^���X�e�X�g���[�h�A���[���}�X�N	<S1B7>						 						*/
	/*--------------------------------------------------------------------------------------------------*/
	if( Axis->MotorLessTest->MotorLessTestMode == TRUE )	/* ���[�^���X�e�X�g���̏���					*/
	{
		ALMSetMask(Axis->AlmManager, WRN_DBOL_LVL, TRUE);	/* A.921 : �c�a�ߕ��׃��[�j���O				*/
	}
	/* <S21D> Start */
	/*--------------------------------------------------------------------------------------------------*/
	/*		���[�j���O���o�}�X�N	A.924�}�X�N���Ȃ�													*/
	/*--------------------------------------------------------------------------------------------------*/
	ALMSetMask( Axis->AlmManager, WRN_EXREG, FALSE );		/* A.924 : �񐶌x��							*/
	/* <S21D> End */
#if defined(CSW_NETIF_SYNQNET_TYPE)
	/*--------------------------------------------------------------------------------------------------*/
	/*		�ߑ��x���o�����ݒ�		SGDZ-BS61�̎d�l�ɍ��킹��											*/
	/*--------------------------------------------------------------------------------------------------*/
//	uswk = ( Cprm->b_prm8 >> 12 ) & 0x0F;
	uswk = ( Prm->b_prm8 >> 12 ) & 0x0F;
	if( uswk == 0 )
	{
		Axis->UniPrms->Bprm->OSCountMax = 32;				/* 2.000ms���� = 62.5us�~32					*/
	}
	else
	{
		Axis->UniPrms->Bprm->OSCountMax = uswk * 4 + 2;		/* 3.125ms���� = 62.5us�~(12�~4 + 2)		*/
	}
#else
	Axis->UniPrms->Bprm->OSCountMax = 0;					/* 2.000ms���� <S0E5> 						*/
#endif

}


/****************************************************************************************************/
/*																									*/
/*		�@�\�I�����p�X�C�b�`(Pn009)		<V259>														*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		Pn009.0 : �\��萔�i�É����[�h�I���j	�����u�ł͋@�\�Ȃ�									*/
/*		Pn009.1 : �d�����䃂�[�h�I��			--> IprmcalCarrerChange()							*/
/*		Pn009.2 : ���x���o�ړ����σt�B���^�I��	 													*/
/*		Pn009.3 : �\��萔																			*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalBitParam009( AXIS_HANDLE *Axis )
{
	UNI_PRM_STR		*UniPrm;
	UniPrm = Axis->UniPrms;
/*--------------------------------------------------------------------------------------------------*/
/*		���x���o�ړ����σt�B���^�I��																*/
/*--------------------------------------------------------------------------------------------------*/
	switch(( UniPrm->Prm->b_prm9 >> 8 ) & 0x0F )
	{
		case 0x00:	/* ���x���o�ړ����σt�B���^����(�G���R�[�_����\�ɂ���Ĉړ����ω񐔂����܂�)	*/
					UniPrm->Bprm->SpdDetMaFil = FALSE;
					break;

		case 0x01:	/* ���x���o�ړ����σt�B���^�L��(�G���R�[�_����\�ɂ�炸�ړ����ω�16��)		*/
					UniPrm->Bprm->SpdDetMaFil = TRUE;
					break;

		default:	/* �p�����[�^�ݒ�ُ�		*/
					ALMSetPramError( Axis->AlmManager, pndef_b_prm9.RegNumber );
					break;
	}

}

/* <S1F5> Start */
/****************************************************************************************************/
/*																									*/
/*		�@�\�I�����p�X�C�b�`(Pn00A)																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		Pn00A.0 : Gr.2�A���[���������̒�~���@�I��													*/
/*		Pn00A.1 : ������~���̒�~���@�I��				 		 									*/
/*		Pn00A.2 : �\��p�����[�^							 										*/
/*		Pn00A.3 : �\��p�����[�^																	*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalBitParam00A( AXIS_HANDLE *Axis )
{
	PRMDATA		*Prm;
	ALM_STOP	*AlmStop;
	FORCE_STOP	*ForceStop;

	Prm = Axis->UniPrms->Prm;		// �p�����[�^�ݒ�f�[�^
	AlmStop = &(Axis->BeSequence->AlmStop);
	ForceStop = &(Axis->BeSequence->ForceStop);

/*--------------------------------------------------------------------------------------------------*/
/*		�f�Q�A���[���̒�~���@�I��																	*/
/*--------------------------------------------------------------------------------------------------*/
	switch(( Prm->b_prmB >> 4 ) & 0x0F )
	{
		case 0x00:	/* Pn406�̐ݒ�g���N���ő�l�Ƃ��ă��[�^��������~								*/
					switch( Prm->b_prm1 & 0x0F )
					{
						case 0x00:	/* Pn406�̐ݒ�g���N���ő�l�Ƃ��ă��[�^��������~��DB��~���	*/
									AlmStop->conf.G2AlmStopMode = GR2STOPMODE_ZSTOPDB ;
									break;
						case 0x01:
						case 0x02:	/* Pn406�̐ݒ�g���N���ő�l�Ƃ��ă��[�^��������~���t���[�������	*/
									AlmStop->conf.G2AlmStopMode = GR2STOPMODE_ZSTOPFR ;
									break;
						default:	/* �p�����[�^�ݒ�ُ�		*/
									ALMSetPramError( Axis->AlmManager, pndef_b_prm1.RegNumber );
									break;
					}
					break;

		case 0x01:	/* �c�a��~�܂��̓t���[������~����(Pn001.0�Ɠ�����~���@)						*/
					AlmStop->conf.G2AlmStopMode = GR2STOPMODE_DEFAULT ;
					break;

		case 0x02:
				switch( Prm->b_prmA & 0x0F )
				{
					case 0x00:	/* �c�a��~�܂��̓t���[������~����(Pn001.0�Ɠ�����~���@)						*/
								AlmStop->conf.G2AlmStopMode = GR2STOPMODE_DEFAULT ;
								break;

					case 0x01:	/* Pn406�̐ݒ�g���N���ő�l�Ƃ��ă��[�^��������~								*/
								switch( Prm->b_prm1 & 0x0F )
								{
									case 0x00:	/* Pn406�̐ݒ�g���N���ő�l�Ƃ��ă��[�^��������~��DB��~���	*/
												AlmStop->conf.G2AlmStopMode = GR2STOPMODE_ZSTOPDB ;
												break;
									case 0x01:
									case 0x02:	/* Pn406�̐ݒ�g���N���ő�l�Ƃ��ă��[�^��������~���t���[�������	*/
												AlmStop->conf.G2AlmStopMode = GR2STOPMODE_ZSTOPFR ;
												break;
									default:	/* �p�����[�^�ݒ�ُ�		*/
												ALMSetPramError( Axis->AlmManager, pndef_b_prm1.RegNumber );
												break;
								}
								break;

					case 0x02:	/* Pn406�̐ݒ�g���N���ő�l�Ƃ��ă��[�^��������~���t���[�������				*/
								AlmStop->conf.G2AlmStopMode = GR2STOPMODE_ZSTOPFR ;
								break;

					case 0x03:	/* Pn30A�ɏ]���ă��[�^��������~												*/
								switch( Prm->b_prm1 & 0x0F )
								{
									case 0x00:	/* Pn30A�ɏ]���ă��[�^��������~��DB��~���					*/
												AlmStop->conf.G2AlmStopMode = GR2STOPMODE_LDSTOPDB ;
												break;
									case 0x01:
									case 0x02:	/* Pn30A�ɏ]���ă��[�^��������~���t���[�������				*/
												AlmStop->conf.G2AlmStopMode = GR2STOPMODE_LDSTOPFR ;
												break;
									default:	/* �p�����[�^�ݒ�ُ�		*/
												ALMSetPramError( Axis->AlmManager, pndef_b_prm1.RegNumber );
												break;
								}
								break;

					case 0x04:	/* Pn30A�ɏ]���ă��[�^��������~���t���[�������								*/
								AlmStop->conf.G2AlmStopMode = GR2STOPMODE_LDSTOPFR ;
								break;

					default:	/* �p�����[�^�ݒ�ُ�		*/
								ALMSetPramError( Axis->AlmManager, pndef_b_prmA.RegNumber );
								break;
				}
				break;
		default:	/* �p�����[�^�ݒ�ُ�		*/
					ALMSetPramError( Axis->AlmManager, pndef_b_prmB.RegNumber );
					break;

	}

/*--------------------------------------------------------------------------------------------------*/
/*		������~���̒�~���@�I��																	*/
/*--------------------------------------------------------------------------------------------------*/
	switch(( Prm->b_prmA >> 4 ) & 0x0F )
	{
		case 0x00:	/* �c�a��~�܂��̓t���[������~����(Pn001.0�Ɠ�����~���@)						*/
					ForceStop->conf.ForceStopMode = FSTOPMODE_DEFAULT ;
					break;

		case 0x01:	/* Pn406�̐ݒ�g���N���ő�l�Ƃ��ă��[�^��������~								*/
					switch( Prm->b_prm1 & 0x0F )
					{
						case 0x00:	/* Pn406�̐ݒ�g���N���ő�l�Ƃ��ă��[�^��������~��DB��~���	*/
									ForceStop->conf.ForceStopMode = FSTOPMODE_ZSTOPDB ;
									break;
						case 0x01:
						case 0x02:	/* Pn406�̐ݒ�g���N���ő�l�Ƃ��ă��[�^��������~���t���[�������	*/
									ForceStop->conf.ForceStopMode = FSTOPMODE_ZSTOPFR ;
									break;
						default:	/* �p�����[�^�ݒ�ُ�		*/
									ALMSetPramError( Axis->AlmManager, pndef_b_prm1.RegNumber );
									break;
					}
					break;

		case 0x02:	/* Pn406�̐ݒ�g���N���ő�l�Ƃ��ă��[�^��������~���t���[�������				*/
					ForceStop->conf.ForceStopMode = FSTOPMODE_ZSTOPFR ;
					break;

		case 0x03:	/* Pn30A�ɏ]���ă��[�^��������~												*/
					switch( Prm->b_prm1 & 0x0F )
					{
						case 0x00:	/* Pn30A�ɏ]���ă��[�^��������~��DB��~���					*/
									ForceStop->conf.ForceStopMode = FSTOPMODE_LDSTOPDB ;
									break;
						case 0x01:
						case 0x02:	/* Pn30A�ɏ]���ă��[�^��������~���t���[�������				*/
									ForceStop->conf.ForceStopMode = FSTOPMODE_LDSTOPFR ;
									break;
						default:	/* �p�����[�^�ݒ�ُ�		*/
									ALMSetPramError( Axis->AlmManager, pndef_b_prm1.RegNumber );
									break;
					}
					break;

		case 0x04:	/* Pn30A�ɏ]���ă��[�^��������~���t���[�������								*/
					ForceStop->conf.ForceStopMode = FSTOPMODE_LDSTOPFR ;
					break;

		default:	/* �p�����[�^�ݒ�ُ�		*/
					ALMSetPramError( Axis->AlmManager, pndef_b_prmA.RegNumber );
					break;
	}
}
/* <S1F5> End */

/****************************************************************************************************/
/*																									*/
/*		�@�\�I�����p�X�C�b�`(Pn00B)																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		Pn00B.0 : �I�y���[�^�̃��[�U�[�萔�\���I��													*/
/*		Pn00B.1 : �f�Q�A���[���̒�~���@�I��			 		 									*/
/*		Pn00B.2 : �O�����͎d�l�T�[�{�p�b�N�̓d�����͑I��	 										*/
/*		Pn00B.3 : �\��萔�i�Z�~�N���[�Y�G���R�[�_�g�p���@�j										*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalBitParam00B( AXIS_HANDLE *Axis )
{
	PRMDATA		*Prm;
//	ALM_STOP	*AlmStop;

	Prm = Axis->UniPrms->Prm;		// �p�����[�^�ݒ�f�[�^
//	AlmStop = &(Axis->BeSequence->AlmStop);

/* <S113> Start*/
/*--------------------------------------------------------------------------------------------------*/
/*	Bit 0: �I�y���[�^�̃��[�U�[�萔�\���I��															*/
/*--------------------------------------------------------------------------------------------------*/
		if( (Prm->b_prmB & 0x000F) > 2 )
		{
			/* �p�����[�^�ݒ�ُ� */
			ALMSetPramError( Axis->AlmManager, pndef_b_prmB.RegNumber );
			
		}
/* <S113> End */
#if 0	/* <S1F5> Start	*/
/*--------------------------------------------------------------------------------------------------*/
/*		�f�Q�A���[���̒�~���@�I��																	*/
/*--------------------------------------------------------------------------------------------------*/
	switch(( Prm->b_prmB >> 4 ) & 0x0F )
	{
		case 0x00:	/* �[������~����										*/
					AlmStop->conf.G2AlmStopMode = FALSE ;
					break;

		case 0x01:	/* �c�a��~�܂��̓t���[������~����(Pn001.0�Ɠ�����~���@)	*/
					AlmStop->conf.G2AlmStopMode = TRUE ;
					break;

		default:	/* �p�����[�^�ݒ�ُ�		*/
					ALMSetPramError( Axis->AlmManager, pndef_b_prmB.RegNumber );
					break;
	}
#endif /* <S1F5> End	*/
}


/****************************************************************************************************/
/*																									*/
/*		�@�\�I�����p�X�C�b�`(Pn00D)																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		Pn00B.3 : �I�[�o�[�g���x�����[�j���O���o�I��												*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalBitParam00D( AXIS_HANDLE *Axis )
{
	PRMDATA			*Prm;
	OT_SEQ_DATA		*OverTravelSeq;

	Prm = Axis->UniPrms->Prm;
	OverTravelSeq = &(Axis->BeSequence->OtSeqData);

/*--------------------------------------------------------------------------------------------------*/
/*		�I�[�o�[�g���x�����[�j���O���o�I��															*/
/*--------------------------------------------------------------------------------------------------*/
	switch(( Prm->b_prmD >> 12 ) & 0x0F )
	{
		case 0x00:	/* OT���[�j���O�����o���Ȃ� */
			OverTravelSeq->conf.OtWarningSw = FALSE;
			break;

		case 0x01:	/* OT���[�j���O�����o���� */
			OverTravelSeq->conf.OtWarningSw = TRUE;
			break;

		default:	/* �p�����[�^�ݒ�ُ� */
			ALMSetPramError( Axis->AlmManager, pndef_b_prmD.RegNumber );
			break;
	}
}
/* <S21D> Start */
/*****************************************************************************
  Description: �@�\�I�����p�X�C�b�`(Pn00E)
------------------------------------------------------------------------------
  Parameters:
	Axis;		Pointer to Axis Handle
  Return:
	none
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static	void	IprmcalBitParam00E( AXIS_HANDLE *Axis )
{
	CMN_PRMDATA			*Cprm;
	BPRMDAT				*Bprm;
	CHECK_MAIN_POWER	*PowerChecker;
	REGENE_CTRL			*RegCtrl;

	Cprm  = Axis->UniPrms->Cprm;	/* ���j�b�g���ʃp�����[�^�ݒ�f�[�^ */
	Bprm = Axis->UniPrms->Bprm;		/* �����p�����[�^��`(�d���ē����ŕύX�L��) */
	RegCtrl = &(Axis->PowerManager->RegeneCtrl);
	PowerChecker = &(Axis->PowerManager->MainPowerChecker);

/*--------------------------------------------------------------------------------------------------*/
/*		Pn00E.1 : DC�d�����͎��̋}�����d�I��														*/
/*--------------------------------------------------------------------------------------------------*/
	switch( (Cprm->b_prmE >> 4) & 0x0F )
	{
	case 0x00: /* DC�d�����͎��͋}�����d���s���܂��� */
		PowerChecker->conf.DcDischarge = FALSE;
		break;

	case 0x01: /* DC�d�����͎��A�R���o�[�^���f�B�M���̏�Ԃɉ����ċ}�����d���s���܂� */
		if( PowerChecker->conf.Dcbus == TRUE )
		{
			PowerChecker->conf.DcDischarge = TRUE;
		}
		else
		{
			PowerChecker->conf.DcDischarge = FALSE;
		}
		break;

	default:
		ALMSetPramError( Axis->AlmManager, pndef_b_prmE.RegNumber );
		break;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		Pn00E.3 : �񐶓����@�\�̎g�p���@															*/
/*--------------------------------------------------------------------------------------------------*/
	switch( (Cprm->b_prmE >> 12) & 0x0F )
	{
	case 0x00: /* �񐶓����@�\���g�p���܂��� */
		RegCtrl->conf.RegSyn = REGSYNOFF;
		break;

	case 0x01: /* PN������ʃV�X�e���ɂ����ĉ񐶓����@�\���g�p���܂� */
		RegCtrl->conf.RegSyn = PNREGSYN;
		break;

	case 0x02: /* PN������ʃV�X�e���ɂĊO���񐶋@�\�𗘗p���܂� */
		RegCtrl->conf.RegSyn = PNEXREG;
		break;

	default:
		ALMSetPramError( Axis->AlmManager, pndef_b_prmE.RegNumber );
		break;
	}
}
/* <S21D> End */

/* < S154 > Start */
/*****************************************************************************
  Description: �@�\�I�����p�X�C�b�`(Pn00F)
------------------------------------------------------------------------------
  Parameters:
	Axis;		Pointer to Axis Handle
  Return:
	none
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static	void	IprmcalBitParam00F( AXIS_HANDLE *Axis )
{
	CMN_PRMDATA		*Cprm;
	CHECK_MAIN_POWER	*PowerChecker;

	Cprm  = Axis->UniPrms->Cprm;		/* �p�����[�^�ݒ�f�[�^ */
	PowerChecker = &(Axis->PowerManager->MainPowerChecker);

/*--------------------------------------------------------------------------------------------------*/
/*		Pn00F.0 : �\�h�ۑS���[�j���O�I��															*/
/*--------------------------------------------------------------------------------------------------*/
	switch( Cprm->b_prmF & 0x0FU )
	{
	case 0x00U: /* �\�h�ۑS���[�j���O�����o���Ȃ� */
		PowerChecker->conf.PreWarningSw = FALSE;
		break;
	case 0x01U: /* �\�h�ۑS���[�j���O�����o���� */
		PowerChecker->conf.PreWarningSw = TRUE;
		break;
	default:
		ALMSetPramError( Axis->AlmManager, pndef_b_prmF.RegNumber );
		break;
	}
}
/* < S154 > End  */

/****************************************************************************************************/
/*																									*/
/*		�Q�C���֌W���p�X�C�b�`(Pn10B)																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		Pn10B.0 : ���[�h�X�C�b�`�I���X�C�b�` --> PcalBitParamMSW()									*/
/*		Pn10B.1 : ���x���[�v����I���X�C�b�`														*/
/*		Pn10B.2 : �ʒu���[�v����I���X�C�b�`														*/
/*		Pn10B.3 : ���U����I���X�C�b�`																*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalBitParam10B( AXIS_HANDLE *Axis )
{
	PRMDATA		*Prm;
	BOOL		PrmSetting; /* <S066> */

	Prm = Axis->UniPrms->Prm;		// �p�����[�^�ݒ�f�[�^

/*--------------------------------------------------------------------------------------------------*/
/*		Pn10B.1 : ���x���[�v����I��ݒ�															*/
/*--------------------------------------------------------------------------------------------------*/
	switch(( Prm->gnmode >> 4 ) & 0x0F )
	{
		case 0x00:
			Axis->BaseControls->CtrlCmdPrm.IPSpdControl = FALSE;
			break;
		case 0x01:
			Axis->BaseControls->CtrlCmdPrm.IPSpdControl = TRUE;
			break;
		default :
			ALMSetPramError( Axis->AlmManager, pndef_gnmode.RegNumber );
			break;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		Pn10B.3 : �ʑ��⏞���x�I�u�U�[�o�ݒ�														*/
/*--------------------------------------------------------------------------------------------------*/
	PrmSetting = EhVobsCalculatePrmSW( &(Axis->BaseControls->EhVobsCtrl.conf.EhVobsUse),
									   Axis->BaseControls->TuneLessCtrl.conf.TuningLessUse,
									   (Prm->gnmode >> 12) );

	if( PrmSetting == PRM_SETTING_NG )
	{
		ALMSetPramError( Axis->AlmManager, pndef_gnmode.RegNumber );
	}
}


/****************************************************************************************************/
/*																									*/
/*		�ʒu����w�ߌ`�ԑI���X�C�b�`(Pn200)															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		Pn200.0 : �w�߃p���X�`�ԑI���X�C�b�` --> JL056HW.C											*/
/*		Pn200.1 : �N���A�M���`�ԑI���X�C�b�` --> V850E.C											*/
/*		Pn200.2 : �N���A����I���X�C�b�`	 														*/
/*		Pn200.3 : �t�B���^�I���X�C�b�`																*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalBitParam200( AXIS_HANDLE *Axis )
{
#if SVFSEL_CMDPLS_USE == 1		/* <S182>�R�[�h���� */
	if(OK != KpiPcalPerClrMode( Axis->pPlsIfV, Axis->UniPrms->Prm->prstsw ) ) /* Pn200: Perr Clear Mode */
	{
		ALMSetPramError( Axis->AlmManager, pndef_prstsw.RegNumber );
	}
#endif
}


/****************************************************************************************************/
/*																									*/
/*		�ʒu����@�\�X�C�b�`(Pn207)																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		Pn207.0 : �ʒu�w�߃t�B���^�I���X�C�b�`														*/
/*		Pn207.1 : �ʒu����I�v�V�����I���X�C�b�`													*/
/*		Pn207.2 : �o�b�N���b�V�␳�I���X�C�b�`														*/
/*		Pn207.3 : /COIN�o�̓^�C�~���O�I���X�C�b�`													*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalBitParam207( AXIS_HANDLE *Axis )
{
	ULONG		ucwk;
	PRMDATA		*Prm;
//	IPRMDAT		*Iprm;

	Prm  = Axis->UniPrms->Prm;		// �p�����[�^�ݒ�f�[�^
//	Iprm = Axis->prm_hndl_ptr->iprm_ptr;		// �����p�����[�^��`(�d���ē����ŕύX�L��)

/*--------------------------------------------------------------------------------------------------*/
/*		Pn207.0 : �ʒu�w�߃t�B���^�I��ݒ�															*/
/*--------------------------------------------------------------------------------------------------*/
#if 0
	ucwk = Prm->cmfilsw & 0x0F;
	switch( ucwk )
	{
		case 0x00:					/* �������t�B���^												*/
		case 0x01:					/* �ړ����σt�B���^												*/
			Iprm->PcmdFilter = (UCHAR)ucwk;
			break;
		default :
			ALMSetPramError( Axis->AlmManager, &pndef_cmfilsw );
			break;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		Pn207.1 : �ʒu����I�v�V�����I��ݒ�														*/
/*--------------------------------------------------------------------------------------------------*/
	switch(( Prm->cmfilsw >> 4 ) & 0x0F )
	{
		case 0x00:
			Iprm->VrefSpdFF = FALSE;
			break;
		case 0x01:
			Iprm->VrefSpdFF = TRUE;
			break;
		default :
			ALMSetPramError( Axis->AlmManager, &pndef_cmfilsw );
			break;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		Pn207.2 : �o�b�N���b�V�␳�I��ݒ�															*/
/*--------------------------------------------------------------------------------------------------*/
	ucwk = ( Prm->cmfilsw >> 8 ) & 0x0F;
	switch( ucwk )
	{
		case 0x00:					/* �o�b�N���b�V�␳����											*/
		case 0x01:					/* ���]�����Ƀo�b�N���b�V�␳									*/
		case 0x02:					/* �t�]�����Ƀo�b�N���b�V�␳									*/
			Iprm->BacklashDir = (UCHAR)ucwk;
			break;
		default :
			ALMSetPramError( Axis->AlmManager, &pndef_cmfilsw );
			break;
	}
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		Pn207.3 : /COIN�o�̓^�C�~���O�I��ݒ�														*/
/*--------------------------------------------------------------------------------------------------*/
	ucwk = (Prm->cmfilsw >> 12) & 0x0F;
	switch( ucwk )
	{
		case 0x00:
		case 0x01:
		case 0x02:
			Axis->BaseControls->PosManager.conf.CoinOutSel = (UCHAR)ucwk;
			break;
		default :
			ALMSetPramError( Axis->AlmManager, pndef_cmfilsw.RegNumber );
			break;
	}
}

#if (CSW_SVCF_BACKLASHCOMP == TRUE)
/****************************************************************************************************/
/*																									*/
/*		�ʒu����g���@�\�X�C�b�`(Pn230)																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		Pn230.0 : �o�b�N���b�V�␳����																*/
/*		Pn230.1 : �\��萔																			*/
/*		Pn230.2 : �\��萔																			*/
/*		Pn230.3 : �\��萔																			*/
/*																									*/
/****************************************************************************************************/
void	IprmcalBitParam230( AXIS_HANDLE *Axis )
{
	UCHAR		ucwk;
	PRMDATA		*Prm;
	BLSHCMP		*BlshCmp;

	Prm = Axis->UniPrms->Prm;
	BlshCmp = &(Axis->BaseControls->PosManager.BlshCmp);

/*--------------------------------------------------------------------------------------------------*/
/*		Pn230.0 : �o�b�N���b�V�␳����																*/
/*--------------------------------------------------------------------------------------------------*/
	ucwk = (Prm->cmctrlexsw) & 0x0F;
	switch( ucwk )
	{
	case 0x00: /* ���]�����̎w�߂ŕ␳���� */
	case 0x01: /* �t�]�����̎w�߂ŕ␳���� */
		BlshCmp->conf.BacklashDir = ucwk;
		break;
	default :
		ALMSetPramError( Axis->AlmManager, pndef_cmctrlexsw.RegNumber );
		break;
	}
}
#endif

#if 0	/* Pn408.1:�I�����C���ɕύX */
/****************************************************************************************************/
/*																									*/
/*		�g���N�֌W�@�\�X�C�b�`(Pn408)																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		Pn408.0 : �m�b�`�t�B���^�I���P�X�C�b�`�i�I�����C���j--> pncal_tfuncsw()						*/
/*		Pn408.1 : ���x�����I���X�C�b�`�i�I�t���C���j												*/
/*		Pn408.2 : �m�b�`�t�B���^�I���Q�X�C�b�`�i�I�����C���j--> pncal_tfuncsw()						*/
/*		Pn408.3 : �O���I�u�U�[�o�I���X�C�b�`�i�I�t���C���j�i�s�g�p�j								*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalBitParam408( AXIS_HANDLE *Axis )
{
	PRMDATA		*Prm;
	IPRMDAT		*Iprm;

	Prm  = Axis->UniPrms->Prm;		// �p�����[�^�ݒ�f�[�^
	Iprm = Axis->UniPrms->iprm_ptr;		// �����p�����[�^��`(�d���ē����ŕύX�L��)

/*--------------------------------------------------------------------------------------------------*/
/*		Pn408.1 : ���x�����I��ݒ�																	*/
/*--------------------------------------------------------------------------------------------------*/
	switch(( Prm->tfuncsw >> 4 ) & 0x0F )
	{
		case 0x00:	Iprm->OvrSpdLimit = FALSE; break;
		case 0x01:	Iprm->OvrSpdLimit = TRUE;  break;
		default :	ALMSetPramError( Axis->AlmManager, pndef_tfuncsw.RegNumber ); break;
	}

}
#endif

/* <S14B> Start */
/****************************************************************************************************/
/*		Pn423 : ���x���b�v���⏞�@�\�I���X�C�b�` 		[�|]										*/
/****************************************************************************************************/
/*																									*/
/*		Pn423.0 : ���x���b�v���⏞�@�\�̎g�p�^�s�g�p�̑I��				�i�I�����C���j				*/
/*		Pn423.1 : ���x���b�v���⏞���s��v���[�j���O(A.942)���o�̑I��	�i�I�t���C���j				*/
/*		Pn423.2 : ���x���b�v���⏞�@�\�̗L�����������l�����̑I��		�i�I�t���C���j				*/
/*		Pn423.3 : ���x���b�v���⏞�@�\�ʑ��I���i����J�j				�i�I�t���C���j				*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalBitVelRippleCompSW( AXIS_HANDLE *Axis )/* <S1FD> */
{
//	ALMMAN_HANDLE	*AlmManager;	/* <S1FD> */
	PRMDATA			*Prm;
	RPLCMP			*RippleCmp;
	ULONG			MotorSnReadRslt;

//	AlmManager	= Axis->AlmManager;	/* <S1FD> */
	Prm			= Axis->UniPrms->Prm;
	RippleCmp	= &(Axis->BaseLoops->RippleCmp);

/*--------------------------------------------------------------------------------------------------*/
/*	Pn423.1 : ���x���b�v���⏞���s��v���[�j���O(A.942)���o�̑I��									*/
/*--------------------------------------------------------------------------------------------------*/
	if( (( Prm->RippleCompSw >> 4 ) & 0x0F ) == 0x00 )
	{
		/* ���[�j���O�����o����		*/
		ALMSetMask( Axis->AlmManager, WRN_RIPPLE, FALSE );/* A.942 : ���b�v���␳���s��v���[�j���O	*/
		RippleCmp->RplCmpPrm.RippleWrnMode = FALSE;	/*<S1FD>*/
	}
	else
	{
		/* ���[�j���O�����o���Ȃ�		*/
		ALMSetMask( Axis->AlmManager, WRN_RIPPLE, TRUE  );/* A.942 : ���b�v���␳���s��v���[�j���O	*/
		RippleCmp->RplCmpPrm.RippleWrnMode = TRUE;	/*<S1FD>*/
	}

/*--------------------------------------------------------------------------------------------------*/
/*	Pn423.2 : ���x���b�v���⏞�@�\�̗L�����������l�����̑I��										*/
/*--------------------------------------------------------------------------------------------------*/
	if( (( Prm->RippleCompSw >> 8 ) & 0x0F ) == 0x00 )
	{
		/* ���x���b�v���⏞�@�\�̗L�����������l���� = �u�w�ߑ��x�v */
		RippleCmp->RippleCmpThresholdSel = CMD_SPD;
	}
	else
	{
		/* ���x���b�v���⏞�@�\�̗L�����������l���� = �u�t�B�[�h�o�b�N���x�v */
		RippleCmp->RippleCmpThresholdSel = FB_SPD;
	}

/*--------------------------------------------------------------------------------------------------*/
/*	Pn423.3 : ���x���b�v���⏞�@�\�ʑ��I���i����J�j	<TODO>										*/
/*--------------------------------------------------------------------------------------------------*/
#if 0
	if( (( Prm->RippleCompSw >> 12 ) & 0x0F ) == 0x00 )
	{
		/* ���[�^�̓d�C�p�i0�`360���j���g�p */

	}
	else
	{
		/* ���[�^�̋@�B�p�i0�`360���j���g�p (���j�A���[�^�̏ꍇ�͖���) */

	}
#endif

/*--------------------------------------------------------------------------------------------------*/
/*	Pn423.0 : ���x���b�v���⏞�@�\�̎g�p�^�s�g�p�̑I��												*/
/*--------------------------------------------------------------------------------------------------*/
	if( (Prm->RippleCompSw & 0x0F) == 0x00 )
	{
		/* ���x���b�v���⏞����� : ���� */
		RippleCmp->var.RippleWrnState = TRUE;

		/* Pn423.0 = 0 : ���x���b�v���⏞�@�\���g�p���Ȃ� */
		RippleCmp->RplCmpPrm.RplCmpUse = FALSE;
	}
	else
	{
		/* ����擾���[�^�V���A��No.�A�O��擾���[�^�V���A��No. */
		MotorSnReadRslt = ChkMotorSerialNo( Axis->AxisNo, Axis->MencV );
//		if( MotorSnReadRslt != 0 )/*<S1FD>*/
		if( (MotorSnReadRslt != 0) && (RippleCmp->RplCmpPrm.RippleWrnMode == FALSE) )
		{	/* ���[�^�V���A��No.���قȂ� && Pn423.1=0 �̂Ƃ� */
//			ALMSetServoAlarm( AlmManager, WRN_RIPPLE );
			/* ���x���b�v���⏞����� : A.942���������� */
			RippleCmp->var.RippleWrnState = FALSE;

			/* Pn423.0 = 0 : ���x���b�v���⏞�@�\���g�p���Ȃ� */
			RippleCmp->RplCmpPrm.RplCmpUse = FALSE;
		}
		else
		{
			/* ���x���b�v���⏞����� : ���� */
			RippleCmp->var.RippleWrnState = TRUE;

			/* Pn423.0 = 1 : ���x���b�v���⏞�@�\���g�p����*/
			RippleCmp->RplCmpPrm.RplCmpUse = TRUE;
		}
	}
//	return;/* <S1FD> */
}

/****************************************************************************************************/
/*		Pn427 : ���x���b�v���⏞�L�����x 		[min-1]												*/
/****************************************************************************************************/
static	void	IprmcalVelRippleCompSpd( AXIS_HANDLE *Axis )
{
	PRMDATA		*Prm;
	BPRMDAT		*Bprm;
	RPLCMP		*RippleCmp;

	Prm			= Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;
	RippleCmp	= &(Axis->BaseLoops->RippleCmp);


	/* Speed [0.1r/min, 0.1mm/s] --> NormalizedSpeed [2^24/OvrSpd] */
	RippleCmp->VelRippleCompSpd = BprmSpeedLevelCal( Bprm, 10 * Prm->VelRippleCompSpd, 0 );
}
/* <S14B> End */
/* <S21D> Start */
/*****************************************************************************
  Description: �o�͐M���I��4(Pn514)
------------------------------------------------------------------------------
  Parameters:
	Axis;		Pointer to Axis Handle
  Return:
	none
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static	void	IprmcalBitParam514( AXIS_HANDLE *Axis )
{
	PRMDATA				*Prm;
	CHECK_MAIN_POWER	*PowerChecker;

	Prm  = Axis->UniPrms->Prm;		/* �p�����[�^�ݒ�f�[�^ */
	PowerChecker = &(Axis->PowerManager->MainPowerChecker);

	switch( (Prm->outsel4 >> 4) & 0x0F )
	{
		case 0x00:
			PowerChecker->conf.CapaLinkFlag = FALSE;
			break;
		case 0x01:
		case 0x02:
		case 0x03:
		case 0x04:
		case 0x05:
		case 0x06:
			PowerChecker->conf.CapaLinkFlag = TRUE;
			break;
		default:
		ALMSetPramError( Axis->AlmManager, pndef_outsel4.RegNumber );
		break;
	}
}
/* <S21D> End*/

/****************************************************************************************************/
/*																									*/
/*		�c�b�d���d���Ď����x���v�Z																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �c�b�d���d���Ď����x���̌v�Z���s���B													*/
/*																									*/
/*			1. �d�����x��																			*/
/*				1) �n�u���o���x��																	*/
/*				2) �t�u���o���x��																	*/
/*				3) �t�u���o�t�B���^���萔															*/
/*				4) �񐶃I�����x��/�I�t���x��														*/
/*																									*/
/*			2. �p�����[�^																			*/
/*				1) PnE70 : P-N�d�����o���x��														*/
/*		 		2) PnE71 : OV���o���x��   [V]														*/
/*				3) PnE72 : �񐶃I�����x�� [V]														*/
/*		 		4) PnE73 : UV���o���x��   [V]														*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalVoltageCheckLevel( AXIS_HANDLE *Axis )
{
	LONG				s;
	CMN_PRMDATA			*Cprm;
	BPRMDAT				*Bprm;
	REGENE_CTRL			*RegCtrl;
	DET_DC_VOLT			*DetectDc;
	CHECK_MAIN_POWER	*PowerChecker;

	s = 0;
	Cprm  = Axis->UniPrms->Cprm;		// �p�����[�^�ݒ�f�[�^
	Bprm = Axis->UniPrms->Bprm;		// �����p�����[�^��`(�d���ē����ŕύX�L��)
	RegCtrl = &(Axis->PowerManager->RegeneCtrl);
	DetectDc = &(Axis->PowerManager->DetectDcVolt);
	PowerChecker = &(Axis->PowerManager->MainPowerChecker);

	/*------------------------------------------------------------------------------------------*/
	/*		OV���x���̌v�Z																		*/
	/*------------------------------------------------------------------------------------------*/
	/* �񐶂��g�p����ꍇ */
	PowerChecker->conf.OvLevel = (USHORT)(Cprm->ovlvl * Bprm->VdetMax / Cprm->pnvoltgn);
	/* �񐶕s�g�p�̏ꍇ */
	PowerChecker->conf.NonRegOvLevel = (USHORT)(Cprm->NonRegOvlvl * Bprm->VdetMax / Cprm->pnvoltgn); /* <S0F7> */

	/*------------------------------------------------------------------------------------------*/
	/*		UV���x���̌v�Z																		*/
	/*------------------------------------------------------------------------------------------*/
	PowerChecker->conf.UvLevel = (USHORT)(Cprm->uvlvl * Bprm->VdetMax / Cprm->pnvoltgn);
	PowerChecker->conf.UvWrnLevel = (USHORT)(Cprm->UvWrnLvl * Bprm->VdetMax / Cprm->pnvoltgn);
	PowerChecker->conf.UvFilter = Cprm->uvfil;

	/*------------------------------------------------------------------------------------------*/
	/*		���H�d�����o�␳ ��d�����x���̌v�Z																		*/
	/*------------------------------------------------------------------------------------------*/
	PowerChecker->conf.TestDcLevel = (USHORT)(Cprm->DcBaseLvl * Bprm->VdetMax / Cprm->pnvoltgn);

	/*------------------------------------------------------------------------------------------*/
	/*		�񐶃��x���̌v�Z																	*/
	/*------------------------------------------------------------------------------------------*/
	/* <S21D> Start */
#if 0
	RegCtrl->conf.RegOnLevel  = (USHORT)( Cprm->regonlvl * Bprm->VdetMax / Cprm->pnvoltgn );
#else
	if( ((Cprm->b_prmE >> 12) & 0x0F) == PNREGSYN )
	{	/* �񐶓����L���̏ꍇ */
		RegCtrl->conf.RegOnLevel = (USHORT)( (Cprm->regonlvl + Cprm->regsynonlvl) * (Bprm->VdetMax / Cprm->pnvoltgn) );
	}
	else
	{
//		RegCtrl->conf.RegOnLevel = (USHORT)( Cprm->regonlvl * (Bprm->VdetMax / Cprm->pnvoltgn) );/* <S21E> */
		RegCtrl->conf.RegOnLevel = (USHORT)( (Cprm->regonlvl * Bprm->VdetMax) / Cprm->pnvoltgn );/* <S21E> */
	}
#endif
	/* <S21D> End */
	RegCtrl->conf.RegOffLevel = (USHORT)( ((Cprm->regonlvl - Cprm->regoffhys) * Bprm->VdetMax) / Cprm->pnvoltgn );

	/*------------------------------------------------------------------------------------------*/
	/*		AD�ϊ������œǏo�����l�̐��K���萔													*/
	/*------------------------------------------------------------------------------------------*/
	/* 2010.06.16 Y.Oka (>_<) for INGRAM PnE70[V]���͎���A/D�l���p�����[�^�����邱�ƁB���ł�VdetMax���I�H */
//	Iprm->VdetAdNorm = MlibPcalKxgain( Bprm->VdetMax, 1, Bprm->VdetAdMax, &s, 24 );
//	DetectDc->conf.VdetAdNorm = MlibPcalKxgain( Bprm->VdetMax, 1, PRM_VDETADMAX, &s, 24 );
	DetectDc->conf.VdetAdNorm = MlibPcalKxgain( Bprm->VdetMax, 1, Cprm->VdetADMax, &s, 24 );

	/*------------------------------------------------------------------------------------------*/
	/*		�d�����o�ő�l�̎擾																*/
	/*------------------------------------------------------------------------------------------*/
	DetectDc->conf.VdetMaxVolt = Bprm->VdetMax;

	/* ���H���oAD�p�t�B���^���萔�v�Z */
	/* 2011.03.29 Y.Oka ScanB�����ύX�ɑΉ� */
//	DetectDc->conf.VdetAdFil = MlibPcalKf1gain( Cprm->vdetadfil, (2 * Bprm->SvCycleUs), 0 );
	PcmCalculateVdetAdFil( DetectDc, Cprm->vdetadfil, Bprm->SvCycleUs );

	/*------------------------------------------------------------------------------------------*/
	/*		�}�����d�������x�� �}�����d���Ԃ̌v�Z								<S1C6>			*/
	/*------------------------------------------------------------------------------------------*/
	PowerChecker->conf.DischCheckVlt = (USHORT)(DISCH_CHK_VOLT * Bprm->VdetMax / Cprm->pnvoltgn);	  /* <S1D1> */
	PowerChecker->conf.DischCompVlt = (USHORT)(Cprm->DischCompVlt * Bprm->VdetMax / Cprm->pnvoltgn);  /* <S1D1> */
	PowerChecker->conf.MainDischActTime = KPI_SCANC_MS( Cprm->ChgWaitTime );

/*--------------------------------------------------------------------------------------------------*/
/*							P-N�d�����o���x��[V]   													*/
/*		KioP.Kdcvolt = -------------------------- 													*/
/*						  		VdetMax	   			VdetMax�F���K���萔								*/
/*--------------------------------------------------------------------------------------------------*/
	DetectDc->conf.Kdcvolt = MlibScalKxgain( Cprm->pnvoltgn, 1, Bprm->VdetMax, NULL, 24 );

}

/****************************************************************************************************/
/*																									*/
/*		�c�b�d���d���Ď����Ԍv�Z <S053>																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �c�b�d���d���Ď����Ԃ̌v�Z���s���B														*/
/*																									*/
/*			1. �d���Ď�����																			*/
/*				1) �c�b�[�d�����҂�����																*/
/*				2) �p���[���f�B�҂�����																*/
/*				3) �����O�n���҂�����																*/
/*																									*/
/*			2. �p�����[�^																			*/
/*				1) PnE88 : �c�b�[�d�����҂����� [ms]												*/
/*		 		2) PnE89 : �p���[���f�B�҂����� [ms]												*/
/*				3) PnE8A : �����O�n���҂�����   [ms]												*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalVoltageCheckTime( AXIS_HANDLE *Axis )
{
	CMN_PRMDATA			*Cprm;
	CHECK_MAIN_POWER	*PowerChecker;

	Cprm  = Axis->UniPrms->Cprm;
	PowerChecker = &(Axis->PowerManager->MainPowerChecker);

	/*------------------------------------------------------------------------------------------*/
	/*		�c�b�[�d�����҂����Ԃ̌v�Z															*/
	/*------------------------------------------------------------------------------------------*/
	PowerChecker->conf.PowerChargeWaitTime = KPI_SCANC_MS(Cprm->ChgWaitTime);

	/*------------------------------------------------------------------------------------------*/
	/*		�p���[���f�B�҂����Ԃ̌v�Z															*/
	/*------------------------------------------------------------------------------------------*/
	PowerChecker->conf.PowerReadyWaitTime = KPI_SCANC_MS(Cprm->PrdyWaitTime);

	/*------------------------------------------------------------------------------------------*/
	/*		�����O�n���҂����Ԃ̌v�Z															*/
	/*------------------------------------------------------------------------------------------*/
//	PowerChecker->conf.GndFaultWaitTime = KPI_SCANC_MS(Cprm->GFWaitTime);

	/*------------------------------------------------------------------------------------------*/
	/*		@@ �p�����[�^�������`�F�b�N															*/
	/*		PowerReadyWaitTime <= PowerChargeWaitTime �̏ꍇ�A�V�[�P���X��s�������N����		*/
	/*		(ChargeOk��TRUE�ɂȂ�Ȃ��Ȃ�)���߁A�p�����[�^�v�Z�G���[�ɂ��Ă����B				*/
	/*------------------------------------------------------------------------------------------*/
	if ( ( PowerChecker->conf.PowerChargeWaitTime == 0 ) || 
		( PowerChecker->conf.PowerReadyWaitTime <= PowerChecker->conf.PowerChargeWaitTime ) )
	{
		/* A.040 : �p�����[�^�ݒ�ُ� */
		ALMSetPramError( Axis->AlmManager, pndef_ChgWaitTime.RegNumber );
	}
}


/****************************************************************************************************/
/*																									*/
/*		�T�[�{�p�b�N�����t�@����~���o���� <S0A3>													*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �t�@����~���o���Ԃ̌v�Z���s���B														*/
/*																									*/
/*			1. �p�����[�^																			*/
/*				1) PnEE5 : �t�@����~�A���[���J�ڎ��� 0.1[s]										*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalFanLockDetectTime( AXIS_HANDLE *Axis )
{
	CMN_PRMDATA			*Cprm;
	Cprm  = Axis->UniPrms->Cprm;

	/* ���Z�p�ϐ������� */
	MlibResetLongMemory( &(Axis->PowerManager->FanLock.var), sizeof( Axis->PowerManager->FanLock.var )/4 );

	Axis->PowerManager->FanLock.conf.FanLockDetectTime = KPI_SCANC_MS( Cprm->FanLockTime * 100 );
	return;
}


/****************************************************************************************************/
/*																									*/
/*		�A���v�����[�^�ߕ��׃��x���̌v�Z															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		LpxPcalOverLoadLevelAmpMot(				�A���v�����[�^�ߕ������ٌv�Z�֐�(�ᑬOL:SGDS����)	*/
/*			OLPRM	*OlPrm,						�ߕ��׃p�����[�^									*/
/*			CHKOLP	*ChkOLP1,					�ߕ��׃`�F�b�N�p�p�����[�^(���Z����)				*/
/*			CHKOLP	*ChkOLP2,					�ߕ��׃`�F�b�N�p�p�����[�^(�ُ펞�ݒ�p)			*/
/*			CHKOLP	*ChkOLP3					�ߕ��׃`�F�b�N�p�p�����[�^(���o���ٹ޲݌v�Z�p)		*/
/*			PRMDATA *prm_ptr																		*/
/*			IPRMDAT *iprm_ptr	);																	*/
/*																									*/
/*		�⑫�jChkOLP1 == ChkOLP2�̏ꍇ�ɂ́A��O�����Ƃ���	ChkOLP2��ChkOLP1�̃R�s�[������			*/
/*			  �s��Ȃ��ŃG���[�Ƃ���B																*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
//static	LONG	LpxPcalOverLoadLevelAmpMot( OLPRM *OlPrm, CHECK_OL *ChkOLP1, CHECK_OL *ChkOLP2,
//											CHECK_OL *ChkOLP3, PRMDATA *prm_ptr, KSGAIN	LinearCoolRate100PerGain )		/* <S154> */
static	LONG	LpxPcalOverLoadLevelAmpMot( OLPRM *OlPrm, CHECK_OLP *ChkOLP1, CHECK_OLP *ChkOLP2,
											CHECK_OLP *ChkOLP3, PRMDATA *prm_ptr, KSGAIN	LinearCoolRate100PerGain )	/* <S154> */
{
	LONG	Ix;
	LONG	Ibsqr;
	LONG	Ixsqr;
	LONG	Iysqr;
	LONG	Izsqr;
	LONG	rc;
	LONG	wk;
	LONG	cflg;

	rc = TRUE;

	/********************************************************************************************/
	/*		�N�������̐ݒ�																		*/
	/*		  FLASE �FChkOLP2����̐ݒ�s��														*/
	/*		  TRUE  �FChkOLP2����̐ݒ��														*/
	/********************************************************************************************/
	if( ChkOLP1 != ChkOLP2 )
	{
		cflg = TRUE;
	}
	else
	{
		cflg = FALSE;
	}

	/********************************************************************************************/
	/*		�A���ߕ��׃��x���̌v�Z																*/
	/********************************************************************************************/
	/*																							*/
	/*		Ibsqr = Ibse * Ibse																	*/
	/*																							*/
	/*				  (Imid^2 - Ibse^2) * Tmid[0.1s]											*/
	/*		AlmLvl = --------------------------------											*/
	/*						  Cycle[ms]															*/
	/*																							*/
	/********************************************************************************************/
	/*------------------------------------------------------------------------------------------*/
	/* 	�x�[�X�d���̃`�F�b�N�F	OlPrm->Ibse														*/
	/*------------------------------------------------------------------------------------------*/
	if( OlPrm->Ibse > 15000 )						/* ���L�l��Б��̐ݒ�l�ɍ��킹��			*/
	{
		if( cflg == TRUE )
		{
#if 1	/* <S154> */
			/* �A���n�k�x�[�X�d��  */
			ChkOLP1->Ibsqrmid = ChkOLP2->Ibsqrmid;
			/* �A���n�k���o���x�� */
			ChkOLP1->AlmLvlmid = ChkOLP2->AlmLvlmid;
			/* �A���n�k���[�j���O���x��  */
			ChkOLP1->WrnLvlmid = ChkOLP2->WrnLvlmid;
			/* �A���n�k���o���x���Q�C�� */
			ChkOLP1->OlLvlmidGain = ChkOLP2->OlLvlmidGain;
			/* �u���n�k�x�[�X�d��  */
			ChkOLP1->Ibsqrmax = ChkOLP2->Ibsqrmax;
			/* �u���n�k���o���x�� */
			ChkOLP1->AlmLvlmax = ChkOLP2->AlmLvlmax;
			/* �u���n�k���[�j���O���x�� */
			ChkOLP1->WrnLvlmax = ChkOLP2->WrnLvlmax;
			/* �u���n�k���o���x���Q�C�� */
			ChkOLP1->OlLvlmaxGain = ChkOLP2->OlLvlmaxGain;
#else	/* <S154> */
			/* �A���n�k�x�[�X�d��  */
			ChkOLP1->conf.Ibsqrmid = ChkOLP2->conf.Ibsqrmid;
			/* �A���n�k���o���x�� */
			ChkOLP1->conf.AlmLvlmid = ChkOLP2->conf.AlmLvlmid;
			/* �A���n�k���[�j���O���x��  */
			ChkOLP1->conf.WrnLvlmid = ChkOLP2->conf.WrnLvlmid;
			/* �A���n�k���o���x���Q�C�� */
			ChkOLP1->conf.OlLvlmidGain = ChkOLP2->conf.OlLvlmidGain;
			/* �u���n�k�x�[�X�d��  */
			ChkOLP1->conf.Ibsqrmax = ChkOLP2->conf.Ibsqrmax;
			/* �u���n�k���o���x�� */
			ChkOLP1->conf.AlmLvlmax = ChkOLP2->conf.AlmLvlmax;
			/* �u���n�k���[�j���O���x�� */
			ChkOLP1->conf.WrnLvlmax = ChkOLP2->conf.WrnLvlmax;
			/* �u���n�k���o���x���Q�C�� */
			ChkOLP1->conf.OlLvlmaxGain = ChkOLP2->conf.OlLvlmaxGain;
#endif	/* <S154> */

			/*----------------------------------------------------------------------------------*/
			/*	���[�^�������W���ݒ�															*/
			/*		�A���v�ߕ��׌W�������[�^�ߕ��׌W���ɍ��킹��ꍇ�͎������100%				*/
			/*		�ɐݒ肵�ăA���v�ߕ��ׂ����[�^�ߕ��ׂ�背�x����������						*/
			/*		ChkOLP1�����[�^�̏ꍇ�́APn586�v�Z�ŏ㏑������邽�߁A						*/
			/*		�����ł́AChkOLP1��100%��ݒ肷��											*/
			/*----------------------------------------------------------------------------------*/
//			ChkOLP1->conf.CoolRateGain = LinearCoolRate100PerGain;	/* <S154> */
			ChkOLP1->CoolRateGain = LinearCoolRate100PerGain;		/* <S154> */

			return( TRUE );								/* �p�����[�^�ݒ�ُ�ɂ��Ȃ�			*/
		}
		else
		{
			return( FALSE );							/* �p�����[�^�ݒ�ُ�ɂ���				*/
		}
	}
	else
	{
		Ix = OlPrm->Ibse;
	}
	Ibsqr = (Ix * Ix) >> 16;							/* Ibsqr�F�x�[�X�d��^2					*/

	/*------------------------------------------------------------------------------------------*/
	/* 	���Ԃn�k�d���̃`�F�b�N�F	OlPrm->Imid													*/
	/*------------------------------------------------------------------------------------------*/
	if( OlPrm->Imid > 0x7FFF )
	{
		Ix = 0x7FFF;
	}
	else
	{
		Ix = OlPrm->Imid;
	}
	Ixsqr = (Ix * Ix) >> 16;							/* Ixsqr�F���Ԃn�k�d��^2				*/

	/*------------------------------------------------------------------------------------------*/
	/* 	�A���n�k�x�[�X�d��^2�A�A���[�����o���x���A���[�j���O���x���A���x���Q�C���̌v�Z			*/
	/*------------------------------------------------------------------------------------------*/
	/*	Izsqr		�F�A���n�k���o�A���[�����x���̕��q�� ( = ���Ԃn�k�d��^2 - �x�[�X�d��^2)		*/
	/*	Ibsqrmid	�F�A���n�k�x�[�X�d��^2														*/
	/*	AlmLvlmid	�F�A���n�k�A���[�����o���x��												*/
	/*	WrnLvlmid	�F�A���n�k���[�j���O���x��													*/
	/*	OlLvlmidGain�F�A���n�k���o���x���Q�C��													*/
	/*------------------------------------------------------------------------------------------*/
	Izsqr = Ixsqr - Ibsqr;
#if 1	/* <S154> */
	ChkOLP1->Ibsqrmid  = Ibsqr;
#if (FLOAT_USE==TRUE)
	ChkOLP1->AlmLvlmid = ( (float)Izsqr * (float)(100*OlPrm->Tmid) / (float)KPI_SCCYCLEMS );
	ChkOLP1->WrnLvlmid = ( (float)ChkOLP1->AlmLvlmid * (float)prm_ptr->olwarlvl / (float)100 );
	ChkOLP1->OlLvlmidGain = (float)ChkOLP3->AlmLvlmid / (float)ChkOLP1->AlmLvlmid;
#else
	ChkOLP1->AlmLvlmid = MlibScalKxgain( Izsqr, (100*OlPrm->Tmid), KPI_SCCYCLEMS, NULL, -30 );
	ChkOLP1->WrnLvlmid = MlibScalKxgain( ChkOLP1->AlmLvlmid, prm_ptr->olwarlvl, 100, NULL, -30 );
	ChkOLP1->OlLvlmidGain = MlibScalKxgain( ChkOLP3->AlmLvlmid, 1, ChkOLP1->AlmLvlmid, NULL, 24 );
#endif /* FLOAT_USE */
#else	/* <S154> */
	ChkOLP1->conf.Ibsqrmid  = Ibsqr;
#if (FLOAT_USE==TRUE)
	ChkOLP1->conf.AlmLvlmid = ( (float)Izsqr * (float)(100*OlPrm->Tmid) / (float)KPI_SCCYCLEMS );
	ChkOLP1->conf.WrnLvlmid = ( (float)ChkOLP1->conf.AlmLvlmid * (float)prm_ptr->olwarlvl / (float)100 );
	ChkOLP1->conf.OlLvlmidGain = (float)ChkOLP3->conf.AlmLvlmid / (float)ChkOLP1->conf.AlmLvlmid;
#else
	ChkOLP1->conf.AlmLvlmid = MlibScalKxgain( Izsqr, (100*OlPrm->Tmid), KPI_SCCYCLEMS, NULL, -30 );
	ChkOLP1->conf.WrnLvlmid = MlibScalKxgain( ChkOLP1->conf.AlmLvlmid, prm_ptr->olwarlvl, 100, NULL, -30 );
	ChkOLP1->conf.OlLvlmidGain = MlibScalKxgain( ChkOLP3->conf.AlmLvlmid, 1, ChkOLP1->conf.AlmLvlmid, NULL, 24 );
#endif /* FLOAT_USE */
#endif	/* <S154> */

//	if( (ChkOLP1->conf.AlmLvlmid >= 0x40000000) || (ChkOLP1->conf.AlmLvlmid == 0) )		/* <S154> */
	if( (ChkOLP1->AlmLvlmid >= 0x40000000) || (ChkOLP1->AlmLvlmid == 0) )		/* <S154> */
	{
		rc = FALSE;
	}

	/********************************************************************************************/
	/*		�u���ߕ��׃��x���̌v�Z																*/
	/********************************************************************************************/
	/*																							*/
	/*				 (Imid^2 * Tmid) - (Imax^2 * Tmax)											*/
	/*		Ibsqr = -----------------------------------											*/
	/*						 (Tmid - Tmax)														*/
	/*																							*/
	/*				  (Imax^2 - Imsqr) * Tmax													*/
	/*		AlmLvl = ----------------------------												*/
	/*						   Cycle															*/
	/*																							*/
	/********************************************************************************************/
	/*------------------------------------------------------------------------------------------*/
	/* 	�ő�n�k�d���̃`�F�b�N�F	 OlPrm->Imax												*/
	/*------------------------------------------------------------------------------------------*/
	if( OlPrm->Imax > 0x7FFF)						/* ���L�l��Б��̐ݒ�l�ɍ��킹��			*/
	{
		if( cflg == TRUE )
		{
#if 1	/* <S154> */
			/* �u���n�k�x�[�X�d�� */
			ChkOLP1->Ibsqrmax = ChkOLP2->Ibsqrmax;
			/* �u���n�k���o���x�� */
			ChkOLP1->AlmLvlmax = ChkOLP2->AlmLvlmax;
			/* �u���n�k���[�j���O���x�� */
			ChkOLP1->WrnLvlmax = ChkOLP2->WrnLvlmax;
			/* �u���n�k���o���x���Q�C�� */
			ChkOLP1->OlLvlmaxGain = ChkOLP2->OlLvlmaxGain;
#else	/* <S154> */
			/* �u���n�k�x�[�X�d�� */
			ChkOLP1->conf.Ibsqrmax = ChkOLP2->conf.Ibsqrmax;
			/* �u���n�k���o���x�� */
			ChkOLP1->conf.AlmLvlmax = ChkOLP2->conf.AlmLvlmax;
			/* �u���n�k���[�j���O���x�� */
			ChkOLP1->conf.WrnLvlmax = ChkOLP2->conf.WrnLvlmax;
			/* �u���n�k���o���x���Q�C�� */
			ChkOLP1->conf.OlLvlmaxGain = ChkOLP2->conf.OlLvlmaxGain;
#endif	/* <S154> */

			/*----------------------------------------------------------------------------------*/
			/*	���[�^�������W���ݒ�															*/
			/*		�A���v�ߕ��׌W�������[�^�ߕ��׌W���ɍ��킹��ꍇ�͎������100%				*/
			/*		�ɐݒ肵�ăA���v�ߕ��ׂ����[�^�ߕ��ׂ�背�x����������						*/
			/*		ChkOLP1�����[�^�̏ꍇ�́APn586�v�Z�ŏ㏑������邽�߁A						*/
			/*		�����ł́AChkOLP1��100%��ݒ肷��											*/
			/*----------------------------------------------------------------------------------*/
//			ChkOLP1->conf.CoolRateGain = LinearCoolRate100PerGain;	/* <S154> */
			ChkOLP1->CoolRateGain = LinearCoolRate100PerGain;		/* <S154> */

			return( TRUE );								/* �p�����[�^�ݒ�ُ�ɂ��Ȃ�			*/
		}
		else
		{
			return( FALSE );							/* �p�����[�^�ݒ�ُ�ɂ���				*/
		}
	}
	else
	{
		Ix = OlPrm->Imax;
	}
	Iysqr = (Ix * Ix) >> 16;						/* Iysqr�F�ő�n�k�d��^2					*/

	/*------------------------------------------------------------------------------------------*/
	/* 	�u���n�k�x�[�X�d��^2�̌v�Z�F	 Ibsqr													*/
	/*------------------------------------------------------------------------------------------*/
	wk = ( Ixsqr * OlPrm->Tmid ) - ( Iysqr * OlPrm->Tmax );		/* �u���n�k�x�[�X�d��^2�̕��q�� */
//	Ibsqr = MlibScalKxgain( wk, 1, (OlPrm->Tmid - OlPrm->Tmax), NULL, -30 );
	Ibsqr = (LONG)( (float)wk / (float)(OlPrm->Tmid - OlPrm->Tmax) );

	/*------------------------------------------------------------------------------------------*/
	/* 	�u���n�k�x�[�X�d��^2�̕��q���`�F�b�N 													*/
	/*------------------------------------------------------------------------------------------*/
	if( wk < 0 )
	{
#if 1	/* <S154> */
		ChkOLP1->Ibsqrmax		= 0;			/* �u���n�k�x�[�X�d��^2	= 0					*/
		ChkOLP1->AlmLvlmax		= 0;			/* �u���n�k���x�� = 0						*/
		ChkOLP1->WrnLvlmax		= 0;			/* �u���n�k���[�j���O���x�� = 0				*/
		ChkOLP1->OlLvlmaxGain	= 0;			/* �u���n�k���[�j���O���x�� = 0				*/
#else	/* <S154> */
		ChkOLP1->conf.Ibsqrmax		= 0;			/* �u���n�k�x�[�X�d��^2	= 0					*/
		ChkOLP1->conf.AlmLvlmax		= 0;			/* �u���n�k���x�� = 0						*/
		ChkOLP1->conf.WrnLvlmax		= 0;			/* �u���n�k���[�j���O���x�� = 0				*/
		ChkOLP1->conf.OlLvlmaxGain	= 0;			/* �u���n�k���[�j���O���x�� = 0				*/
#endif	/* <S154> */
		return( FALSE );							/* �p�����[�^�ݒ�ُ�ɂ���					*/
	}
	else
	{
		/*--------------------------------------------------------------------------------------*/
		/* 	�u���n�k�x�[�X�d��^2�̃`�F�b�N														*/
		/*--------------------------------------------------------------------------------------*/
		if( Ibsqr > 0x7fff)							/* �{�����͔������Ȃ����Ƃ肠��������Ă���	*/
		{
			if(cflg == TRUE)
			{
//				Ibsqr = ChkOLP2->conf.Ibsqrmax;		/* �u��OL�x�[�X�d����Б��̐ݒ�l�ɍ��킹��	*/	/* <S154> */
				Ibsqr = ChkOLP2->Ibsqrmax;		/* �u��OL�x�[�X�d����Б��̐ݒ�l�ɍ��킹��	*/		/* <S154> */
			}
			else
			{
				return( FALSE );					/* �p�����[�^�ݒ�ُ�ɂ���					*/
			}
		}

		/*--------------------------------------------------------------------------------------*/
		/* �u���n�k�x�[�X�d��^2�ƘA���n�k�x�[�X�d��^2�̑召�֌W�`�F�b�N 						*/
		/*--------------------------------------------------------------------------------------*/
//		if( Ibsqr < ChkOLP1->conf.Ibsqrmid )		/* ���L�l��A���n�k�̐ݒ�l�ɍ��킹��	*/	/* <S154> */
		if( Ibsqr < ChkOLP1->Ibsqrmid )				/* ���L�l��A���n�k�̐ݒ�l�ɍ��킹��	*/	/* <S154> */
		{
#if 1 /* <S154> */
			/* �u���n�k�x�[�X�d��^2 */
			ChkOLP1->Ibsqrmax  = ChkOLP1->Ibsqrmid;
			/* �u���n�k���x�� */
			ChkOLP1->AlmLvlmax = ChkOLP1->AlmLvlmid;
			/* �u���n�k���[�j���O���x�� */
			ChkOLP1->WrnLvlmax = ChkOLP1->WrnLvlmid;
			 /* �u���n�k���x���Q�C�� */
#if (FLOAT_USE==TRUE)
			ChkOLP1->OlLvlmaxGain =
					(float)ChkOLP3->AlmLvlmax / (float)ChkOLP1->AlmLvlmax;
#else
			ChkOLP1->OlLvlmaxGain =
					MlibScalKxgain( ChkOLP3->AlmLvlmax, 1, ChkOLP1->AlmLvlmax, NULL, 24 );
#endif /* FLOAT_USE */
#else /* <S154> */
			/* �u���n�k�x�[�X�d��^2 */
			ChkOLP1->conf.Ibsqrmax  = ChkOLP1->conf.Ibsqrmid;
			/* �u���n�k���x�� */
			ChkOLP1->conf.AlmLvlmax = ChkOLP1->conf.AlmLvlmid;
			/* �u���n�k���[�j���O���x�� */
			ChkOLP1->conf.WrnLvlmax = ChkOLP1->conf.WrnLvlmid;
			 /* �u���n�k���x���Q�C�� */
#if (FLOAT_USE==TRUE)
			ChkOLP1->conf.OlLvlmaxGain =
					(float)ChkOLP3->conf.AlmLvlmax / (float)ChkOLP1->conf.AlmLvlmax;
#else
			ChkOLP1->conf.OlLvlmaxGain =
					MlibScalKxgain( ChkOLP3->conf.AlmLvlmax, 1, ChkOLP1->conf.AlmLvlmax, NULL, 24 );
#endif /* FLOAT_USE */
#endif /* <S154> */
		}
		/*--------------------------------------------------------------------------------------*/
		/* 	�u���n�k�x�[�X�d��^2�̐ݒ�A�u���n�k���x���A���[�j���O���x���A���x���Q�C���̌v�Z	*/
		/*--------------------------------------------------------------------------------------*/
		/*	Izsqr		�F�u���n�k���o�A���[�����x���̕��q��(�ő�n�k�d��^2 - �u���x�[�X�d��^2)	*/
		/*	Ibsqrmax	�F�u���n�k�x�[�X�d��^2													*/
		/*	AlmLvlmax	�F�u���n�k�A���[�����o���x��											*/
		/*	WrnLvlmax	�F�u���n�k���[�j���O���x��												*/
		/*	OlLvlmaxGain�F�u���n�k���o���x���Q�C��												*/
		/*--------------------------------------------------------------------------------------*/
		else
		{
			Izsqr = Iysqr - Ibsqr;
#if 1 /* <S154> */
			ChkOLP1->Ibsqrmax  = Ibsqr;
#if (FLOAT_USE==TRUE)
			ChkOLP1->AlmLvlmax = ( (float)Izsqr * (float)(100*OlPrm->Tmax) / (float)KPI_SCCYCLEMS );
			ChkOLP1->WrnLvlmax = ( (float)ChkOLP1->AlmLvlmax * (float)prm_ptr->olwarlvl / (float)100 );
			ChkOLP1->OlLvlmaxGain = (float)ChkOLP3->AlmLvlmax / (float)ChkOLP1->AlmLvlmax;
#else
			ChkOLP1->AlmLvlmax = MlibScalKxgain( Izsqr, (100*OlPrm->Tmax), KPI_SCCYCLEMS, NULL, -30 );
			ChkOLP1->WrnLvlmax = MlibScalKxgain( ChkOLP1->AlmLvlmax, prm_ptr->olwarlvl, 100, NULL, -30 );
			ChkOLP1->OlLvlmaxGain = MlibScalKxgain( ChkOLP3->AlmLvlmax, 1, ChkOLP1->AlmLvlmax, NULL, 24 );
#endif /* FLOAT_USE */
#else /* <S154> */
			ChkOLP1->conf.Ibsqrmax  = Ibsqr;
#if (FLOAT_USE==TRUE)
			ChkOLP1->conf.AlmLvlmax = ( (float)Izsqr * (float)(100*OlPrm->Tmax) / (float)KPI_SCCYCLEMS );
			ChkOLP1->conf.WrnLvlmax = ( (float)ChkOLP1->conf.AlmLvlmax * (float)prm_ptr->olwarlvl / (float)100 );
			ChkOLP1->conf.OlLvlmaxGain = (float)ChkOLP3->conf.AlmLvlmax / (float)ChkOLP1->conf.AlmLvlmax;
#else
			ChkOLP1->conf.AlmLvlmax = MlibScalKxgain( Izsqr, (100*OlPrm->Tmax), KPI_SCCYCLEMS, NULL, -30 );
			ChkOLP1->conf.WrnLvlmax = MlibScalKxgain( ChkOLP1->conf.AlmLvlmax, prm_ptr->olwarlvl, 100, NULL, -30 );
			ChkOLP1->conf.OlLvlmaxGain = MlibScalKxgain( ChkOLP3->conf.AlmLvlmax, 1, ChkOLP1->conf.AlmLvlmax, NULL, 24 );
#endif /* FLOAT_USE */
#endif /* <S154> */

//			if( ChkOLP1->conf.AlmLvlmax >= 0x40000000 )	/* <S154> */
			if( ChkOLP1->AlmLvlmax >= 0x40000000 )		/* <S154> */
			{
				rc = FALSE;
			}
		}
	}
	return( rc );
}


/****************************************************************************************************/
/*																									*/
/*		�ݒu�����j�^�p�p�����[�^�v�Z												<S03A>			*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalInstEnvMon( AXIS_HANDLE *Axis )
{
	CMN_PRMDATA			*Cprm;
	Cprm	= Axis->UniPrms->Cprm;

	/*----------------------------------------------------------------------------------------------*/
	/*	PnE0F(����Byte)�F�ݒu�����j�^����x (�ݒ�����j�^100%���̓��C���x)					*/
	/*----------------------------------------------------------------------------------------------*/
	Axis->EnvMonitor->conf.AmpEnvMonOffset = 100 - (CHAR)(Cprm->InstEnvTmp_sw1.b.l);
	Axis->EnvMonitor->conf.MotEnvMonOffset = 100 - (CHAR)(Cprm->InstEnvTmp_sw1.b.l); /* TODO */
	return;

}



/****************************************************************************************************/
/*																									*/
/*		�A���v�����[�^�ߕ��׃��x���̌v�Z															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		PnF09 : USHORT	irat;			���[�^��i�d��						[0.1Ao-p]				*/
/*		PnF0A : USHORT	imax;			���[�^�u���ő�d��					[0.1Ao-p]				*/
/*		PnF0F : USHORT	bastrq;			���[�^�ߕ��׌��o�x�[�X�d��			[%]						*/
/*		PnF10 : USHORT	midtrq;			���[�^�ߕ��׌��o���ԓd��			[%]						*/
/*		PnF11 : USHORT	midolt;			���[�^�ߕ��׌��o���Ԏ���			[10s]					*/
/*		PnF12 : USHORT	mtrq2;			���[�^�ߕ��׌��o���ԓd���Q			[%]						*/
/*		PnF13 : USHORT	mtolt2;			���[�^�ߕ��׌��o���Ԏ��ԂQ			[s]						*/
/*																									*/
/*		PnE14 :	USHORT	irats;			�A���v��i�d��						[0.1Ao-p]				*/
/*		PnE15 : USHORT	imaxs;			�A���v�u���ő�d��					[0.1Ao-p]				*/
/*		PnE17 : DBYTE	ibs_md.b.l;		�A���v�ߕ��׌��o�x�[�X�d��			[%]						*/
/*						ibs_md.b.h;		�A���v�ߕ��׌��o���ԓd��			[10%]					*/
/*		PnE18 : DBYTE	imdt_mxt.b.l;	�A���v�ߕ��׌��o���Ԏ���			[10s]					*/
/*						imdt_mxt.b.h;	�A���v�ߕ��׌��o�ő厞��			[s]						*/
/*																									*/
/*		Pn52C : USHORT	mtbastrq_drt;	���[�^�ߕ��׌��o�x�[�X�d���f�B���[�e�B���O					*/
/*																									*/
/*		PnE60 : DBYTE	ratbt_mt.b.l;	�n�k�����f�B���[�e�B���O(Base�d��)	[%]						*/
/*						ratbt_mt.b.h;	�n�k�����f�B���[�e�B���O(���ԓd��)	[%]						*/
/*		PnE61 : DBYTE	ratmtt_2.b.l;	�n�k�����f�B���[�e�B���O(���Ԏ���)	[%]						*/
/*						ratmtt_2.b.h;	�n�k�����f�B���[�e�B���O(�ő�d��)	[%]						*/
/*		PnE62 : DBYTE	ratmt2_os.b.l;	�n�k�����f�B���[�e�B���O(�ő厞��)	[%]						*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		PnE90 : DBYTE	olspdlvl.b.l;	�A���v�ᑬ�n�k�ᑬ��ԕϋȓ_�̓d�C�p���x	[0.1Hz]			*/
/*						olspdlvl.b.h;	�A���v�ᑬ�n�k���b�N��ԕϋȓ_�̓d�C�p���x	[0.1Hz]			*/
/*		PnE91 : DBYTE	ollspdprm.b.l;	�A���v�ᑬ�n�k���o���ԓd��					[%]				*/
/*						ollspdprm.b.h;	�A���v�ᑬ�n�k���o�ő厞��					[0.1s]			*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalOverLoadLevelAmpMot( AXIS_HANDLE *Axis )
{
	OLPRM		Mot, Amp;
	BOOL		prmchk;
	PRMDATA		*Prm;
	BPRMDAT		*Bprm;
	CHECK_OL	*pMotOL;
	CHECK_OL	*pAmpOL;
	KSGAIN		ks, ks2;
#if (FLOAT_USE==FALSE)
	LONG		kx, sx;
#endif /* FLOAT_USE */

	KSGAIN		LinearCoolRate100PerGain;

/* <S154> Start */
#if CSW_SVCF_CHKAMPLOL == 1
	CHECK_LOWOL *pLowOL;
	LONG	wk;
	LONG	s;
	LONG	calcwk;
	OLPRM	AmpLock,AmpLow;
	CHECK_OLP	*AmpLockOLP;
	LOWOLPRM	*LowOLPrm;
	LOWOLCHKP	*LowOLChkP;
	pLowOL    = &(Axis->CheckAlarm->AmpLowOL);
	AmpLockOLP = &(Axis->CheckAlarm->AmpOL.conf.LockArea);
	LowOLPrm = &(pLowOL->LowOLPrm);
	LowOLChkP = &(pLowOL->LowOLChkP);
#endif
/* <S154> End */

	Prm       = Axis->UniPrms->Prm;		// �p�����[�^�ݒ�f�[�^
	Bprm      = Axis->UniPrms->Bprm;		// ��{�p�����[�^��`
	pMotOL    = &(Axis->CheckAlarm->MotOL);
	pAmpOL    = &(Axis->CheckAlarm->AmpOL);


	/********************************************************************************************/
	/*		�ߕ��׊�{�p�����[�^�v�Z ==> [0.1s], [15000/MaxTrq(Imax)]							*/
	/********************************************************************************************/
	/*	���[�^���n�k�֘A��{�p�����[�^�v�Z	*/
	Mot.Tmid = (Prm->MencP.midolt * Prm->ratmtt_2.b.l);
	Mot.Tmax = (Prm->MencP.mtolt2 * Prm->ratmt2_os.b.l) / 10;
#if (FLOAT_USE==TRUE)
	ks = (float)100 * (float)Bprm->PerMaxTrq / (float)15000;
	ks2 = (float)10000 * (float)Bprm->PerMaxTrq / (float)(15000 * Prm->mtbastrq_drt);
	Mot.Ibse = ( (float)Prm->MencP.bastrq * (float)Prm->ratbt_mt.b.l / ks2 );
	Mot.Imid = ( (float)Prm->MencP.midtrq * (float)Prm->ratbt_mt.b.h / ks );
	Mot.Imax = ( (float)Prm->MencP.mtrq2 * (float)Prm->ratmtt_2.b.h / ks );
#else
	ks = MlibScalKxgain( 100, Bprm->PerMaxTrq, 15000, NULL, -1 );
	ks2 = MlibScalKxgain( 10000, Bprm->PerMaxTrq, 15000 * Prm->mtbastrq_drt, NULL, -1 );
	Mot.Ibse = MlibScalKxkxks( Prm->MencP.bastrq, Prm->ratbt_mt.b.l, ks2, NULL, -30 );
	Mot.Imid = MlibScalKxkxks( Prm->MencP.midtrq, Prm->ratbt_mt.b.h, ks, NULL, -30 );
	Mot.Imax = MlibScalKxkxks( Prm->MencP.mtrq2,  Prm->ratmtt_2.b.h, ks, NULL, -30 );
#endif /* FLOAT_USE */
	/*-------------------------------------------------------------------------------------------*/
	/*	�A���v���n�k�֘A��{�p�����[�^�v�Z	*/
	Amp.Tmid = (Prm->imdt_mxt.b.l * Prm->ratmtt_2.b.l);
	Amp.Tmax = (Prm->imdt_mxt.b.h * Prm->ratmt2_os.b.l) / 10;
#if (FLOAT_USE==TRUE)
	ks = (float)100 * (float)(Prm->MencP.irat*Bprm->PerMaxTrq) / (float)15000;
	/* <S1F5> Start */
	if ( pLowOL->conf.BaseCurrentDerateEnable == TRUE )
	{	/* �P���d�����͑I�����̃x�[�X�d���f�B���[�e�B���O���� */
		ks2 = (float)10000 * (float)(Prm->MencP.irat*Bprm->PerMaxTrq) /
					(float)( 15000 * Prm->svbastrq_drt );
	}
	else
	{	/* �P���d�����͑I�����̃x�[�X�d���f�B���[�e�B���O�Ȃ� */
		ks2 = ks ;
	}
//	Amp.Ibse = ( (float)(Prm->ibs_md.b.l * Prm->irats) * Prm->ratbt_mt.b.l / ks );
	Amp.Ibse = ( (float)(Prm->ibs_md.b.l * Prm->irats) * Prm->ratbt_mt.b.l / ks2 );
	/* <S1F5> End */
	Amp.Imid = ( (float)(10*Prm->ibs_md.b.h * Prm->irats) * Prm->ratbt_mt.b.h / ks );
	Amp.Imax = ( (float)(Prm->ratmtt_2.b.h * Prm->imaxs) * Prm->ratmtt_2.b.h / ks );
#else
	ks = MlibScalKxgain( 100, (Prm->MencP.irat*Bprm->PerMaxTrq), 15000, NULL, -1 );
	/* <S1F5> Start */
	if ( pLowOL->conf.BaseCurrentDerateEnable == TRUE )
	{	/* �P���d�����͑I�����̃x�[�X�d���f�B���[�e�B���O���� */
		ks2 = MlibScalKxgain( 10000, (Prm->MencP.irat*Bprm->PerMaxTrq), (15000 * Prm->svbastrq_drt), NULL, -1 );
	}
	else
	{	/* �P���d�����͑I�����̃x�[�X�d���f�B���[�e�B���O�Ȃ� */
		ks2 = ks;
	}
//	Amp.Ibse = MlibScalKxkxks( (Prm->ibs_md.b.l * Prm->irats), Prm->ratbt_mt.b.l, ks, NULL, -30 );
	Amp.Ibse = MlibScalKxkxks( (Prm->ibs_md.b.l * Prm->irats), Prm->ratbt_mt.b.l, ks2, NULL, -30 );
	/* <S1F5> End */
	Amp.Imid = MlibScalKxkxks( (10*Prm->ibs_md.b.h * Prm->irats), Prm->ratbt_mt.b.h, ks, NULL, -30 );
	Amp.Imax = MlibScalKxkxks( (Prm->ratmtt_2.b.h  * Prm->imaxs), Prm->ratmtt_2.b.h, ks, NULL, -30 );
#endif /* FLOAT_USE */

	/********************************************************************************************/
	/*		�ߕ��׃p�����[�^�������`�F�b�N														*/
	/********************************************************************************************/
	prmchk = TRUE;								/* prmchk�F�p�����[�^�`�F�b�N�m�F�t���O			*/
	/*------------------------------------------------------------------------------------------*/
	/*	���[�^�p�����[�^�������`�F�b�N															*/
	/*------------------------------------------------------------------------------------------*/
	if( Mot.Tmid <= Mot.Tmax )					/* ���ԓd���n�k���� <= �ő�d���n�k����			*/
	{
		ALMSetPramError( Axis->AlmManager, pndef_midolt.RegNumber );
		prmchk = FALSE;
	}
	if( Mot.Imid >= Mot.Imax )					/* ���Ԃn�k�d�� > �ő�n�k�d��					*/
	{
		ALMSetPramError( Axis->AlmManager, pndef_midtrq.RegNumber );
		prmchk = FALSE;
	}
	if( Mot.Imid <= Mot.Ibse )					/* ���Ԃn�k�d�� < �x�[�X�n�k�d��				*/
	{
		ALMSetPramError( Axis->AlmManager, pndef_bastrq.RegNumber );
		prmchk = FALSE;
	}
	/*------------------------------------------------------------------------------------------*/
	/*	�A���v�p�����[�^�������`�F�b�N															*/
	/*------------------------------------------------------------------------------------------*/
	if( Amp.Tmid <= Amp.Tmax )					/* ���ԓd���n�k���� < �ő�d���n�k����			*/
	{
		ALMSetPramError( Axis->AlmManager, pndef_imdt_mxt.RegNumber );
		prmchk = FALSE;
	}
	if( Amp.Imid >= Amp.Imax )					/* ���Ԃn�k�d�� > �ő�n�k�d��					*/
	{
		ALMSetPramError( Axis->AlmManager, pndef_ibs_md.RegNumber );
		prmchk = FALSE;
	}
	if( Amp.Imid <= Amp.Ibse )					/* ���Ԃn�k�d�� < �x�[�X�n�k�d��				*/
	{
		ALMSetPramError( Axis->AlmManager, pndef_ibs_md.RegNumber );
		prmchk = FALSE;
	}

	/********************************************************************************************/
	/*	���[�^������p���p�����[�^�v�Z															*/
	/*		�����v�Z�ŕK�v�Ȃ��ߏ����ݒ�l�Ōv�Z												*/
	/********************************************************************************************/
//	IprmcalCoolRateGainMot( );
	/*------------------------------------------------------------------------------------------*/
	/*	�A���v������p���p�����[�^�v�Z															*/
	/*------------------------------------------------------------------------------------------*/
//	pAmpOL->conf.CoolRateGain = 0;											/* 0�Œ�			*/	/* <S154> */
	pAmpOL->conf.NormalArea.CoolRateGain = 0;								/* 0�Œ�			*/	/* <S154> */

#if 0 /* 2009.09.25 Y.Oka �ᑬOL�Ȃ� */
	if(LowOlChkP.LowSpdOLChkEnable == TRUE)		/* �ᑬ�n�k���o���s�t���O�F���o�H		*/
	{
	    LowOlChkP.AmpLockOLP.CoolRateGain = ChkAlmP.AmpOLP.CoolRateGain;
	}
#endif

	/*----------------------------------------------------------------------------------*/
	/*	���[�^������p�� 100%�v�Z														*/
	/*----------------------------------------------------------------------------------*/
	if( Axis->MencV->AxisMotType == MOTTYPE_LINEAR )
	{
#if (FLOAT_USE==TRUE)
		LinearCoolRate100PerGain = 16384.0f / (float)Bprm->NorMaxSpd;
#else
		kx = MlibScalKxgain( 16384, 100, 100, &sx, 0 );
		LinearCoolRate100PerGain = MlibPcalKxgain( kx, 1, Bprm->NorMaxSpd, &sx, 24 );
#endif /* FLOAT_USE */
	}
	else
	{
		LinearCoolRate100PerGain = 0;
	}

	/********************************************************************************************/
	/*		�ߕ��׃`�F�b�N�p�p�����[�^�v�Z														*/
	/********************************************************************************************/
	if( prmchk == TRUE )						/* �p�����[�^�ݒ�ُ�̗L������					*/
	{
		/*--------------------------------------------------------------------------------------*/
		/*	�e�ߕ��׃`�F�b�N�p�p�����[�^�v�Z													*/
		/*--------------------------------------------------------------------------------------*/
		if( Amp.Imax > Mot.Imax )
		{
			/*----------------------------------------------------------------------------------*/
			/*	���[�^�ߕ��׃`�F�b�N�p�p�����[�^�v�Z											*/
			/*----------------------------------------------------------------------------------*/
//			if( LpxPcalOverLoadLevelAmpMot( &Mot, pMotOL, pMotOL, pMotOL,
//			                                Prm, LinearCoolRate100PerGain ) != TRUE )		/* <S154> */
			if( LpxPcalOverLoadLevelAmpMot( &Mot, &(pMotOL->conf.NormalArea), &(pMotOL->conf.NormalArea),&(pMotOL->conf.NormalArea),
			                                Prm, LinearCoolRate100PerGain ) != TRUE )		/* <S154> */
			{
				/* A.040 : �p�����[�^�ݒ�ُ� */
				ALMSetPramError( Axis->AlmManager, pndef_midtrq.RegNumber );
			}
			/*----------------------------------------------------------------------------------*/
			/*	�A���v�ߕ��׃`�F�b�N�p�p�����[�^�v�Z											*/
			/*----------------------------------------------------------------------------------*/
//			if( LpxPcalOverLoadLevelAmpMot( &Amp, pAmpOL, pMotOL, pAmpOL,
//			                                Prm, LinearCoolRate100PerGain ) != TRUE )		/* <S154> */
			if( LpxPcalOverLoadLevelAmpMot( &Amp, &(pAmpOL->conf.NormalArea), &(pMotOL->conf.NormalArea), &(pAmpOL->conf.NormalArea),
			                                Prm, LinearCoolRate100PerGain ) != TRUE )		/* <S154> */

			{
				/* A.040 : �p�����[�^�ݒ�ُ� */
				ALMSetPramError( Axis->AlmManager, pndef_ibs_md.RegNumber );
			}
		}
		else
		{
			/*----------------------------------------------------------------------------------*/
			/*	�A���v�ߕ��׃`�F�b�N�p�p�����[�^�v�Z											*/
			/*----------------------------------------------------------------------------------*/
//			if( LpxPcalOverLoadLevelAmpMot( &Amp, pAmpOL, pAmpOL, pAmpOL,
//			                                Prm, LinearCoolRate100PerGain ) != TRUE )		/* <S154> */
			if( LpxPcalOverLoadLevelAmpMot( &Amp, &(pAmpOL->conf.NormalArea), &(pAmpOL->conf.NormalArea), &(pAmpOL->conf.NormalArea),
			                                Prm, LinearCoolRate100PerGain ) != TRUE )		/* <S154> */
			{
				/* A.040 : �p�����[�^�ݒ�ُ� */
				ALMSetPramError( Axis->AlmManager, pndef_ibs_md.RegNumber );
			}
			/*----------------------------------------------------------------------------------*/
			/*	���[�^�ߕ��׃`�F�b�N�p�p�����[�^�v�Z											*/
			/*----------------------------------------------------------------------------------*/
//			if( LpxPcalOverLoadLevelAmpMot( &Mot, pMotOL, pAmpOL, pMotOL,
//			                                Prm, LinearCoolRate100PerGain ) != TRUE )		/* <S154> */
			if( LpxPcalOverLoadLevelAmpMot( &Mot, &(pMotOL->conf.NormalArea), &(pAmpOL->conf.NormalArea), &(pMotOL->conf.NormalArea),
			                                Prm, LinearCoolRate100PerGain ) != TRUE )		/* <S154> */
			{
				/* A.040 : �p�����[�^�ݒ�ُ� */
				ALMSetPramError( Axis->AlmManager, pndef_midtrq.RegNumber );
			}
		}
/* <S154> Start */
#if CSW_SVCF_CHKAMPLOL == 1 			/* CSW_SVCF_CHKAMPLOL�F�@���`�i�@��ɂ��ᑬ�n�k�@�\�̗L���j	*/
		/****************************************************************************************/
		/*		�A���v�ᑬ�ߕ��׎��s��������													*/
		/****************************************************************************************/
		/*		1.( (PnE90.L==0)&&(PnE90.H==0) )�FPnE90�������Ƃ�0								*/
		/*			���e��Ԃ̕ϋȓ_���x���Ȃ����߁A�ᑬ�n�k���s�m�f							*/
		/*		2.( PnE90.L <= PnE90.H )�F���b�N��ԕϋȓ_���x���ᑬ��ԕϋȓ_���x				*/
		/*			���ϋȓ_���x�̑召�֌W���t�̂��߁A�ᑬ�n�k���s�m�f							*/
		/*		3.( Amp.Ibse >= 0x4000 )�F�A���n�k�x�[�X�d�����ő�d��(15000��16384)�ȏ�		*/
		/*			���n�k�J�[�u�����K���d���ȏ�͈̔͂ƂȂ邽�߁A�ᑬ�n�k���s�m�f				*/
		/*			�i�v�Z���Ă��Ӗ��̂Ȃ��n�k�J�[�u�ɂȂ�j									*/
		/*	�˒ᑬ�n�k���s�m�f�̏ꍇ�͒ʏ�̉ߕ��׌��o���s���B�ᑬ�n�k�p�����[�^�v�Z���Ȃ��B	*/
		/*--------------------------------------------------------------------------------------*/
		if(	((Prm->olspdlvl.b.l!= (UCHAR)0)||(Prm->olspdlvl.b.h!= (UCHAR)0)) && ( Prm->olspdlvl.b.l > Prm->olspdlvl.b.h )
			&& ( Amp.Ibse < 0x4000 ) )
		{
			/************************************************************************************/
			/*		�A���v�ᑬ�ߕ���  ���o���s�t���O����										*/
			/************************************************************************************/
			/*	�ᑬ�n�k���o���s�t���O��TRUE�ɂ���F���o��										*/
			/*		LowSpdOLChkEnable�Œᑬ�n�k�̌��o�ۂ��R���g���[������B					*/
			/*----------------------------------------------------------------------------------*/
			LowOLChkP->conf.LowSpdOLChkEnable = (UCHAR)TRUE;			/* �ᑬ�n�k���o���s�t���O�F���o��		*/

			/************************************************************************************/
			/*		�A���v�ᑬ�ߕ���  ���b�N��ԃp�����[�^�v�Z									*/
			/************************************************************************************/
			/*----------------------------------------------------------------------------------*/
			/*	�A���v�ᑬ�n�k�F���b�N��ԑ���{�p�����[�^�v�Z									*/
			/*----------------------------------------------------------------------------------*/
			/*	Tmid�F���b�N��Ԓ��Ԃn�k���ԁc�ʏ��ԂƓ���									*/
			/*	Imid�F���b�N��Ԓ��Ԃn�k�d���c���L���ɂĎZ�o									*/
			/*			���F(PnE91.L*PnE14*PnE60.H*15000)/(PnF09*100*Bprm.PerMaxTrq)			*/
			/*	Tmax�F���b�N��ԍő�n�k���ԁc���L���ɂĎZ�o									*/
			/*			���F(PnE91.H*PnE62.L)/100												*/
			/*	Imax�F���b�N��ԍő�n�k�d���c�ʏ��ԂƓ���									*/
			/*	Ibse�F���b�N��ԘA���n�k�x�[�X�d���c�ʏ��ԂƓ���								*/
			/*----------------------------------------------------------------------------------*/
			AmpLock.Tmid = Amp.Tmid;
			ks = MlibScalKxgain( 100, (LONG)(Prm->MencP.irat*Bprm->PerMaxTrq), 15000, NULL, -1 );
			AmpLock.Imid = MlibScalKxkxks( (LONG)(Prm->ollspdprm.b.l * Prm->irats), (LONG)Prm->ratbt_mt.b.h, ks, NULL, -30 );
			AmpLock.Tmax = MlibScalKxgain( (LONG)Prm->ollspdprm.b.h, (LONG)Prm->ratmt2_os.b.l, 100, NULL, -30 );
			AmpLock.Imax = Amp.Imax;
			AmpLock.Ibse = Amp.Ibse;
			LowOLPrm->AmpLock = AmpLock;

			/*----------------------------------------------------------------------------------*/
			/*	�A���v�ᑬ�n�k�F���b�N��ԃ`�F�b�N�p�p�����[�^�v�Z								*/
			/*----------------------------------------------------------------------------------*/
			if( LpxPcalOverLoadLevelAmpMot( &AmpLock, AmpLockOLP , &(pMotOL->conf.NormalArea),
				&(pAmpOL->conf.NormalArea) , Prm , LinearCoolRate100PerGain) != TRUE )
			{
				ALMSetPramError( Axis->AlmManager, pndef_midtrq.RegNumber );		/* A.040 : �p�����[�^�ݒ�ُ�			*/
			}

			/*----------------------------------------------------------------------------------*/
			/*	�A���v�ᑬ�n�k�F���b�N��ԏ����i�ʏ��Ԃƃ��b�N��Ԃ̂n�k���o���x���̓���j	*/
			/*----------------------------------------------------------------------------------*/
			AmpLockOLP->AlmLvlmid = pAmpOL->conf.NormalArea.AlmLvlmid;/* �A���n�k����			*/
			AmpLockOLP->AlmLvlmax = pAmpOL->conf.NormalArea.AlmLvlmax;/* �u���n�k����			*/
			AmpLockOLP->WrnLvlmid = pAmpOL->conf.NormalArea.WrnLvlmid;/* �A���n�kܰ�ݸ�����		*/
			AmpLockOLP->WrnLvlmax = pAmpOL->conf.NormalArea.WrnLvlmax;/* �u���n�kܰ�ݸ�����		*/

			/************************************************************************************/
			/*		�A���v�ᑬ�ߕ���  �ᑬ��ԃp�����[�^�v�Z									*/
			/************************************************************************************/
			/*----------------------------------------------------------------------------------*/
			/*	�A���v�ᑬ�n�k�F�ᑬ��ԑ���{�p�����[�^�v�Z									*/
			/*----------------------------------------------------------------------------------*/
			/*	Tmid�F�ᑬ��Ԓ��Ԃn�k���ԁc�ʏ��ԂƓ���										*/
			/*	Imid�F�ᑬ��Ԓ��Ԃn�k�d���c��ײ݂ŎZ�o											*/
			/*	Tmax�F�ᑬ��ԍő�n�k���ԁc��ײ݂ŎZ�o											*/
			/*	Imax�F�ᑬ��ԍő�n�k�d���c�ʏ��ԂƓ���										*/
			/*	Ibse�F�ᑬ��ԘA���n�k�x�[�X�d���c�ʏ��ԂƓ���								*/
			/*----------------------------------------------------------------------------------*/
			AmpLow.Tmid = Amp.Tmid;
			AmpLow.Imax = Amp.Imax;
			AmpLow.Ibse = Amp.Ibse;
			LowOLPrm->AmpLow = AmpLow;

			/*----------------------------------------------------------------------------------*/
			/*	�A���v�ᑬ�n�k�F�ᑬ��ԕ�ԉ��Z�p�W���v�Z�P									*/
			/*----------------------------------------------------------------------------------*/
			/*	calcwk			�F�ϋȓ_���x���Z�Q�C��											*/
			/* 	LowSpdOLchg		�F�ᑬ��ԕϋȓ_���x(PnE90.L)	[0.1Hz]��[2^24/OvrSpd]			*/
			/*	LockSpdOLchg	�F���b�N��ԕϋȓ_���x(PnE90.H) [0.1Hz]��[2^24/OvrSpd]			*/
			/*	LowSpdOffsetImid�F���b�N��ԕϋȓ_���Ԃn�k�d��	[0.1sec]						*/
			/*	LowSpdOffsetTmax�F���b�N��ԕϋȓ_�ő�n�k����	[15000/MaxTrq]					*/
			/*----------------------------------------------------------------------------------*/
			calcwk = MlibScalKxgain( 1, 1, 10, &s, 0 );
			calcwk = MlibPcalKxkxks( calcwk, 1, Bprm->Keangle, &s, 0 );
			calcwk = MlibPcalKxgain( calcwk, C2PAIE7, 10000000, &s, 0 );
			calcwk = MlibPcalKxkxks( calcwk, 0x1000000, Bprm->OvrSpd, &s, 24 );
			LowOLChkP->conf.LowSpdOLchg = MlibMulgain((LONG)Prm->olspdlvl.b.l, calcwk);
			LowOLChkP->conf.LockSpdOLchg = MlibMulgain((LONG)Prm->olspdlvl.b.h, calcwk);
			LowOLChkP->conf.LowSpdOffsetImid = AmpLock.Imid;
			LowOLChkP->conf.LowSpdOffsetTmax = AmpLock.Tmax;

			/*----------------------------------------------------------------------------------*/
			/*	�A���v�ᑬ�n�k�F�ᑬ��ԕ�ԉ��Z�p�W���v�Z�Q									*/
			/*----------------------------------------------------------------------------------*/
			/*	TransOLImid�F[�d�C�p���x]��[���Ԃn�k�d��]��ԌW��								*/
			/*					(�ʏ��Ԓ���OL�d���\���b�N��Ԓ���OL�d��)						*/
			/*			��	----------------------------------------------------				*/
			/*					(�ᑬ��ԕϋȓ_���x�\���b�N��ԕϋȓ_���x)						*/
			/*																					*/
			/*	TransOLTmax�F[�d�C�p���x]��[�n�k���o����]��ԌW��								*/
			/*						(�u������OL���ԁ\�ᑬ��ԍő�OL����)						*/
			/*			��	---------------------------------------------------					*/
			/*					��(�ᑬ��ԕϋȓ_���x�\���b�N��ԕϋȓ_���x)					*/
			/*		�E�u������OL����(PnE18.H[sec])			:	imdt_mxt.b.h					*/
			/*		�E�ᑬ��ԍő�OL����(PnE91.H[0.1sec])	:	ollspdprm.b.h					*/
			/*----------------------------------------------------------------------------------*/
			wk = LowOLChkP->conf.LowSpdOLchg - LowOLChkP->conf.LockSpdOLchg;
			LowOLChkP->conf.TransOLImid = MlibScalKxgain( (Amp.Imid - AmpLock.Imid), 1, wk, NULL, 24 );
			wk = (LONG)MlibSqrtu32( (ULONG)LowOLChkP->conf.LowSpdOLchg - (ULONG)LowOLChkP->conf.LockSpdOLchg );
			LowOLChkP->conf.TransOLTmax = MlibScalKxgain( (((LONG)Prm->imdt_mxt.b.h*10) - (LONG)Prm->ollspdprm.b.h), 1, wk, NULL, 24 );
		}
		else
		{
			/************************************************************************************/
			/*		�A���v�ᑬ�ߕ���  ���o���s�t���O����										*/
			/************************************************************************************/
			/*	�ᑬ�n�k���o���s�t���O��FALSE�ɂ���F���o�s��									*/
			/*		LowSpdOLChkEnable�Œᑬ�n�k�̌��o�ۂ��R���g���[������B					*/
			/*----------------------------------------------------------------------------------*/
			LowOLChkP->conf.LowSpdOLChkEnable = (UCHAR)FALSE;		/* �ᑬ�n�k���o���s�t���O�F���o�s��		*/
		}
#endif
/* <S154> End */
	}
}


/****************************************************************************************************/
/*																									*/
/*		���[�^�\���Ď��p�����[�^�v�Z(��]�`���[�^)													*//* <V051> */
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �\���Ď��g���N���x���y�ё��x���x���̌v�Z���s��											*/
/*																									*/
/*			�\���Ď��g���N		�FPnE44.h															*/
/*			�\���Ď����x���x��	�F�I�[�o�[�X�s�[�h��20%												*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalMotorOverrun( AXIS_HANDLE *Axis )
{
	ULONG			work1;
	PRMDATA			*Prm;
	BPRMDAT			*Bprm;
	CHECK_OVRRUN	*pOvrRun;

	Prm = Axis->UniPrms->Prm;		// �p�����[�^�ݒ�f�[�^
	Bprm = Axis->UniPrms->Bprm;		// ��{�p�����[�^��`
	pOvrRun = &(Axis->CheckAlarm->OvrRun);
	work1 = Prm->ipg_rwt.b.h;

	/* �\���Ď��g���N�w�߃��x�� */
	if ( work1 > (ULONG)Bprm->PerMaxTrq )
	{
		pOvrRun->OvrTrqLevel = 0x01000000;
	}
	else
	{
#if (FLOAT_USE==TRUE)
		pOvrRun->OvrTrqLevel = work1 * 0x01000000 / Bprm->PerMaxTrq;
#else
		pOvrRun->OvrTrqLevel = MlibScalKxgain( work1, 0x01000000, Bprm->PerMaxTrq, NULL, -24);
#endif /* FLOAT_USE */

	}

	/* �\���Ď����[�^���x���x�� */
	pOvrRun->OvrSpdLevel = 0x01000000 / 5;

}


/****************************************************************************************************/
/*																									*/
/*		���͐M���^�C�~���O�ő勖�e���Ԃ̌v�Z														*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		Pn620 : USHORT	bbondiff0;			STOP�M���^�C�~���O�ő勖�e���ԁiPn620�j[10ms]			*/
/*		PnE4C : USHORT	bbondiff1;			�����M���^�C�~���O�ő勖�e���ԁiPnE4C�j[10ms]			*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalMaxBbonTimeDiff( AXIS_HANDLE *Axis )
{
	CHECK_HWBB_TIMING *HwbbTiming;
	PRMDATA		*Prm;

	Prm = Axis->UniPrms->Prm;		// �p�����[�^�ݒ�f�[�^
	HwbbTiming = &(Axis->CheckAlarm->HwbbTiming);
	HwbbTiming->conf.MaxBbonTimeDiff = 10 * Prm->bbondiff0;
}


/****************************************************************************************************/
/*																									*/
/*		�ʒu�w�߉ߑ��x(�n�r)�̌v�Z																	*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalPcmdOvrSpd( AXIS_HANDLE *Axis )
{
#if 0	// ���󖢑Ή�
	PRMDATA		*Prm;
	BPRMDAT		*Bprm;
	IPRMDAT		*Iprm;
#if (FLOAT_USE==TRUE)
	float		fw;
#else
	LONG		kx, sx;
#endif /* FLOAT_USE */

	Prm = Axis->UniPrms->Prm;		// �p�����[�^�ݒ�f�[�^
	Bprm = Axis->UniPrms->Bprm;		// ��{�p�����[�^��`
	Iprm = Axis->UniPrms->iprm_ptr;		// ��{�p�����[�^��`

#if (FLOAT_USE==TRUE)
	fw = Bprm->OvrSpd / Bprm->Kfbpls;
	fw = fw * (float)Prm->rata2l / (float)Prm->ratb2l;
	Iprm->PcmdOvrSpd = fw * (float)Iprm->SvCycleNs / 1000000000.0F;
#else

	kx = MlibScalKxksks( 1,  Bprm->OvrSpd,    Bprm->Kfbpls,  &sx,  0 );
	kx = MlibPcalKxgain( kx, Prm->rata2l,     Prm->ratb2l,   &sx,  0 );
	kx = MlibPcalKxgain( kx, Iprm->SvCycleNs, 1000000000,    &sx, 24 );
	Iprm->PcmdOvrSpd = kx;
#endif /* FLOAT_USE */
#endif
}


/****************************************************************************************************/
/*																									*/
/*		�g���N���x���̌v�Z	<S00C>																	*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalTorqueLevel( AXIS_HANDLE *Axis )
{
	BPRMDAT		*Bprm;

	Bprm = Axis->UniPrms->Bprm;		// ��{�p�����[�^��`

	Bprm->TrefonLevel = MlibScalKxgain( 0x01000000, 10, Bprm->PerMaxTrq, NULL, -24 );
}

/****************************************************************************************************/
/*																									*/
/*		�I�[�o�g���x���}�X�N 	�b��I�I�I<S032>																*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalOtMask( AXIS_HANDLE *Axis )
{
	PRMDATA		*Prm;
	Prm = Axis->UniPrms->Prm;

	Axis->IoSignals->pot_mask = (( Prm->insel1 & 0xF000 ) == 0x8000)? 0 : 1;
	Axis->IoSignals->not_mask = (( Prm->insel2 & 0x000F ) == 0x0008)? 0 : 1;

}

/****************************************************************************************************/
/*																									*/
/*		�p���[��֘A�ݒ�																			*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		PnE0C.Bit0 : �񐶒�R����																	*/
/*		PnE0C.Bit4 : �˓�/�c�a�������																*/
/*		PnE0C.Bit5 : �˓�/�c�a�������																*/
/*		PnE0C.Bit6 : �񐶉ߕ��׌��o�p�����l�����I��													*/
/*		PnE0C.Bit7 : �񐶉ߕ��׌��o�p�����l�����I��													*/
/*		PnE0C.Bit8 : ���H����d�����o����															*/
/*		PnE0C.Bit9 : ���H����d�����o����															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		���H���o�p�����[�^�������m�F																*/
/*			�ȉ��̏����������A�ُ�Ƃ���B�ُ펞�A�񐶓���֎~�t���O���I������B					*/
/*				�o�v�l�����̏ꍇ�F���H���o�ő�l > PWM�o�͕�(KPX_VDET_PWM_MAX)					*/
/*				�`�c�����̏ꍇ�@�F���H���o�ő�l > AD�ϊ��@�ő�Ǐo���l(KPX_DCVOLT_ADMAX)			*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalPowerCircuitSetting( AXIS_HANDLE *Axis )
{
//	PRMDATA				*Prm;
	BPRMDAT				*Bprm;
	CMN_PRMDATA			*Cprm;
	REGENE_CTRL			*RegCtrl;
	DET_DC_VOLT			*DetectDc;
	CHECK_MAIN_POWER	*PowerChecker;

	Bprm 	= Axis->UniPrms->Bprm;		// ��{�p�����[�^��`
	Cprm	= Axis->UniPrms->Cprm;
	RegCtrl = &(Axis->PowerManager->RegeneCtrl);
	DetectDc = &(Axis->PowerManager->DetectDcVolt);
	PowerChecker = &(Axis->PowerManager->MainPowerChecker);

	/* ������ */
	DetectDc->conf.VdetPrmOk = TRUE;

/*--------------------------------------------------------------------------------------------------*/
/*		PnE0C : �V�X�e���X�C�b�`																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( Cprm->syssw2 & 0x0001 )
	{
		RegCtrl->conf.RegRxIn = TRUE;
	}
	else
	{
		RegCtrl->conf.RegRxIn = FALSE;
	}

	/*----------------------------------------------------------------------------------------------*/
	/*		bit4,5 : �˓�/DB������@																*/
	/*			00	�˓�+DB�F�����[(�~1)															*/
	/*			01 	�˓��F�����[�GDB:�T�C���X�^														*/
	/*			10	�˓��F�����[�GDB�F�����[														*/
	/*			11	�˓�+DB�F�����[+�T�C���X�^														*/
	/*----------------------------------------------------------------------------------------------*/
	switch( (Cprm->syssw2 >> 4) & 0x03 )
	{
	case 0x00:
		PowerChecker->conf.RlyDbWay = DBCTRL_RLY_COMMON;
		break;
	case 0x01:
		PowerChecker->conf.RlyDbWay = DBCTRL_TRIACK_SEPARATE;
		break;
	case 0x02:
		PowerChecker->conf.RlyDbWay = DBCTRL_RLY_SEPARATE;
		break;
	case 0x03:
		PowerChecker->conf.RlyDbWay = DBCTRL_TRIACK_COMMON;
		break;
	default:
		ALMSetPramError( Axis->AlmManager, pndef_syssw2.RegNumber );
		break;
	}

	/*----------------------------------------------------------------------------------------------*/
	/*		bit6,7 : �񐶉ߕ��׌��o�p�����l�����I��													*/
	/*			00�FRgolAlmLevel * 0.95																*/
	/*			01�FRgolAlmLevel * 0.50																*/
	/*			10�F�\�� (01�Ɠ���)																	*/
	/*			11�F�\�� (01�Ɠ���)																	*/
	/*----------------------------------------------------------------------------------------------*/
	switch( (Cprm->syssw2 >> 6) & 0x03 )
	{
	case 0x00:
		RegCtrl->RegOL.conf.RgSumSel = REGPOWSUM_RATE95PER;
		break;
	case 0x01:
	case 0x02:
	case 0x03:
		RegCtrl->RegOL.conf.RgSumSel = REGPOWSUM_RATE50PER;
		break;
	default:
		ALMSetPramError( Axis->AlmManager, pndef_syssw2.RegNumber );
		break;
	}

	/*----------------------------------------------------------------------------------------------*/
	/*		Bit8,9 : ���H����d�����o����															*/
	/*----------------------------------------------------------------------------------------------*/
	switch((UCHAR)( Cprm->syssw2 >> 8 ) & 0x03 )
	{
		case 0x01:									/* PWM-�R���p���[�^���� : �Œ�l(1)�ɂ��Ǐ]	*/
			DetectDc->conf.VdetMode = VDET_PWM_FIX_FOLLOW;
			break;
		case 0x02:									/* PWM-�R���p���[�^���� : �ϒl(1-3)�ɂ��Ǐ]	*/
			DetectDc->conf.VdetMode = VDET_PWM_VAR_FOLLOW;
//			Iprm->VdetFollowWidthMax = 0x03;		/* �Ǐ]���ő���Z��								*/
			break;
		case 0x03:									/* A/D�ϊ�����									*/
			DetectDc->conf.VdetMode = VDET_AD;
			break;
		default:									/* �\�t�g�E�F�A�ɂ�錟�o�Ȃ�					*/
			DetectDc->conf.VdetMode = VDET_NONE;
//			Iprm->VdetFollowWidthMax = 0;

			/* ���o�Ȃ����A�p�����[�^�G���[ */
			ALMSetPramError( Axis->AlmManager, pndef_syssw2.RegNumber );
			break;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�g�v�ɂ�����l�ƃ\�t�g���o����l�̊֌W�`�F�b�N											*/
/*--------------------------------------------------------------------------------------------------*/
	switch( DetectDc->conf.VdetMode )
	{
	/*------------------------------------------------------------------------------------------*/
	/*		�o�v�l�����̏ꍇ																	*/
	/*------------------------------------------------------------------------------------------*/
		case VDET_PWM_FIX_FOLLOW:
		case VDET_PWM_VAR_FOLLOW:
			if( Bprm->VdetMax > KPX_VDET_PWM_MAX )
			{
				DetectDc->conf.VdetPrmOk = FALSE;
			}
			break;
	/*------------------------------------------------------------------------------------------*/
	/*		�`�c�����̏ꍇ																		*/
	/*------------------------------------------------------------------------------------------*/
		case VDET_AD:
			if( Bprm->VdetMax > KPX_DCVOLT_ADMAX )
			{
				DetectDc->conf.VdetPrmOk = FALSE;
			}
			break;
	/*------------------------------------------------------------------------------------------*/
	/*		�\�t�g�ɂ��d�����o�Ȃ��̏ꍇ														*/
	/*------------------------------------------------------------------------------------------*/
		default:
			DetectDc->conf.VdetPrmOk = FALSE;
			break;
	}

	if( DetectDc->conf.VdetPrmOk == FALSE )
	{
		ALMSetPramError( Axis->AlmManager, pndef_syssw2.RegNumber );
	}

}


/****************************************************************************************************/
/*																									*/
/*		�R�s�[�p�����[�^(Prm --> Iprm)																*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalCopyParamToIprm( AXIS_HANDLE *Axis )
{
#if 0
	PRMDATA		*Prm;
	IPRMDAT		*Iprm;

	Prm = Axis->UniPrms->Prm;		// ��{�p�����[�^��`
	Iprm = Axis->UniPrms->iprm_ptr;	// ��{�p�����[�^��`

	Iprm->mtlmt = Prm->mtlmt;		/* �}���`�^�[�����~�b�g�ݒ�						*/
#endif
}



/****************************************************************************************************/
/*																									*/
/*		�ʒu�w�ߓd�q�M�����Z																		*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalElectronicGear( AXIS_HANDLE *Axis )
{
	EGEAR		*Egear;
	EGEAR		*InvalidEgear;
	PRMDATA		*Prm;
	BPRMDAT		*Bprm;
	LONG		Ratb_a, Rata_b;
	LONG		i, sht;

	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;
	Egear = &(Axis->BaseControls->PosManager.conf.Egear);
	InvalidEgear = &(Axis->BaseControls->PosManager.conf.InvalidEgear);

/*--------------------------------------------------------------------------------------------------*/
/*		�d�q�M�����Z																				*/
/*--------------------------------------------------------------------------------------------------*/
	MlibPcalKegear( Prm->rata2l, Prm->ratb2l, Egear );

/*--------------------------------------------------------------------------------------------------*/
/*		���Z�G���[�`�F�b�N																			*/
/*--------------------------------------------------------------------------------------------------*/
	Ratb_a = (Egear->k2 == 0) ? Egear->k1 : Egear->k1 + 1;
	Rata_b = (Egear->g2 == 0) ? Egear->g1 : Egear->g1 + 1;

	if( ( Prm->rata2l > 0x40000000 ) || ( Prm->ratb2l > 0x40000000 ) ||
		( Egear->a == 0 ) || ( Egear->b == 0 ) ||
//		( Ratb_a > 4000 ) || ( Rata_b > 1000 ) )	/* <S17C> */
		( Ratb_a > 64000 ) || ( Rata_b > 1000 ) )	/* <S17C> ����l�ύX(4000��64000) */
	{
		MlibPcalKegear( 1, 1, Egear );
		ALMSetPramError( Axis->AlmManager, pndef_rata2l.RegNumber );
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu�΍����j�^�p���ԕ␳���Z																*/
/*--------------------------------------------------------------------------------------------------*/
	sht = 0;
	for(i = 2; i <= (Bprm->SvCycleNs/CFG_POSERR_DELAY); i*=2)
	{	sht++;}
	Axis->BaseControls->PosManager.conf.PosErrDelayShift = sht;
/*--------------------------------------------------------------------------------------------------*/
/*		�d�q�M�A�@�\�����p�M�A�䉉�Z																*/
/*--------------------------------------------------------------------------------------------------*/
	MlibPcalKegear( 1, 1, InvalidEgear );
}


/****************************************************************************************************/
/*																									*/
/*		�A���[���}�X�N�ݒ�(PnE05/E06)																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �A���[���}�X�N�p�����[�^(PnE05/PnE06)�ɏ]���āA�A���[���}�X�N�t���O��ݒ肷��B			*/
/*																									*/
/*																									*/
/*	�� �� : �A���[���}�X�N�����@�\�͕s�v�Ǝv���邪�A�Ƃ肠�����A�r�f�c�r�ɍ��킹�Ă����B			*/
/*																									*/
/*			1) servo_alarm_mask( ) : �A���[���}�X�N�̐ݒ�^����(�����ɂ��ݒ�^�������w��)			*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalAlarmMask( AXIS_HANDLE *Axis )
{
	ULONG		Mskx;
	PRMDATA		*Prm;

	Prm		= Axis->UniPrms->Prm;

/*--------------------------------------------------------------------------------------------------*/
/*		�A���[���}�X�N : PnE05																		*/
/*--------------------------------------------------------------------------------------------------*/
	Mskx = Prm->almmsk1;
	/* �G���R�[�_�ڑ��ُ�ł��łɃ��[�^/�T�[�{�e�ʃA���}�b�`(A.050)���}�X�N����Ă��邩�̃`�F�b�N *//* <S19C> */
	if( !ALMCheckSetMask( Axis->AlmManager, ALM_UNMATCH ) )
	{	/* �}�X�N����Ă��Ȃ��ꍇ�̂�PnE05�̃}�X�N�r�b�g�̔��f */
		ALMSetMask(Axis->AlmManager, ALM_UNMATCH,	((Mskx & 0x0001)!=0));/* A.050 : �^�C�v�A���}�b�`			*/
	}	/* <S19C> */
	ALMSetMask(Axis->AlmManager, ALM_RGOL,		((Mskx & 0x0002)!=0));/* A.320 : �񐶉ߕ��� 				*/
	ALMSetMask(Axis->AlmManager, ALM_PUV,		((Mskx & 0x0004)!=0));/* A.410 : �s���d�� 					*/
	ALMSetMask(Axis->AlmManager, ALM_OSF1,		((Mskx & 0x0008)!=0));/* A.510 : �ߑ��x(�n�r)				*/
	if((Mskx & 0x0010)!=0)
	{	ALMSetMask(Axis->AlmManager, ALM_OLF1,	TRUE);}				  /* A.710 : �ߕ���(�u���ő啉��)		*/
	if((Mskx & 0x0020)!=0)
	{	ALMSetMask(Axis->AlmManager, ALM_OLF2,	TRUE);}				  /* A.720 : �ߕ���(�A���ő啉��)		*/
	ALMSetMask(Axis->AlmManager, ALM_DBOL,		((Mskx & 0x0040)!=0));/* A.730 : �c�a�ߕ��� 				*/
	ALMSetMask(Axis->AlmManager, ALM_RSOL,		((Mskx & 0x0080)!=0));/* A.740 : �˓���R�ߕ��� 			*/
	ALMSetMask(Axis->AlmManager, ALM_RWF,		((Mskx & 0x0100)!=0));/* A.C10 : �\���h�~���o				*/
	ALMSetMask(Axis->AlmManager, ALM_OF,		((Mskx & 0x0200)!=0));/* A.D00 : �ʒu�΍��ߑ� 				*/
	ALMSetMask(Axis->AlmManager, ALM_PWPHASE,	((Mskx & 0x0400)!=0));/* A.F10 : �d������			 		*/
	ALMSetMask(Axis->AlmManager, ALM_FSOF,		((Mskx & 0x0800)!=0));/* A.D10 : ���[�^-���׈ʒu�ԕ΍��ߑ�	*/
	ALMSetMask(Axis->AlmManager, ALM_OPSET,		((Mskx & 0x1000)!=0));/* A.E00 : ����Acc�v����MTimeOut		*/
	ALMSetMask(Axis->AlmManager, ALM_TIMEOUT,	((Mskx & 0x1000)!=0));/* A.E01 : �^�]�J�n�v����MTimeOut	*/
	ALMSetMask(Axis->AlmManager, ALM_WDC_ERR,	((Mskx & 0x2000)!=0));/* A.E02 : WDC�{SyncFalg�����ُ�		*/
	ALMSetMask(Axis->AlmManager, ALM_WIR,		((Mskx & 0x4000)!=0));/* A.330 : ���H�z���G���[			*/
	ALMSetMask(Axis->AlmManager, ALM_VREFADCHK,	((Mskx & 0x8000)!=0));/* A.B11 : ���x�w�߂`�c�f�[�^�ُ�		*/

/*--------------------------------------------------------------------------------------------------*/
/*		�A���[���}�X�N : PnE06																		*/
/*--------------------------------------------------------------------------------------------------*/
	Mskx = Prm->almmsk2;
	ALMSetMask(Axis->AlmManager, ALM_NOTSUPPORT,((Mskx & 0x0001)!=0));/* A.051 : ���i���T�|�[�g				*/
	ALMSetMask(Axis->AlmManager, ALM_VREFADERR, ((Mskx & 0x0002)!=0));/* A.B10 : ���x�w�ߗp�`�c�ُ�			*/
	ALMSetMask(Axis->AlmManager, ALM_POSOF,     ((Mskx & 0x0004)!=0));/* A.D30 : �ʒu�f�[�^�I�[�o�[�t���[	*/
	ALMSetMask(Axis->AlmManager, ALM_FANLOCK,	((Mskx & 0x0008)!=0));/* A.7AB : �T�[�{�p�b�N�����t�@����~	*/
	ALMSetMask(Axis->AlmManager, ALM_CTRLBRD,	((Mskx & 0x0010)!=0));/* A.7A1 : �������x�ُ�1(������)	*//* <S0DE> */
	ALMSetMask(Axis->AlmManager, ALM_PWRBRD,	((Mskx & 0x0010)!=0));/* A.7A2 : �������x�ُ�2(�p���[���)	*//* <S0DE> */

/*--------------------------------------------------------------------------------------------------*/
/*		���[�^���X�e�X�g���[�h�A���[���}�X�N����	<S1B7>											*/
/*--------------------------------------------------------------------------------------------------*/
	if( Axis->MotorLessTest->MotorLessTestMode == TRUE )				/* ���[�^���X�e�X�g���̏���			*/
	{
		ALMSetMask(Axis->AlmManager, ALM_DBOL, TRUE);					/* A.730 : �c�a�ߕ��� 				*/
		ALMSetMask(Axis->AlmManager, ALM_DBOL_LVL, TRUE);				/* A.731 : �c�a�ߕ��� 				*/
	}

}



/****************************************************************************************************/
/*																									*/
/*		�V�X�e���X�C�b�`�ݒ�(PnE0B/E0C/E0D)															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		PnE0B.Bit0 : �Z�~�N���[�Y/�t���N���[�Y�`���I��												*/
/*		PnE0B.Bit1 : ��]�^/���j�A�^�N���I��														*/
/*		PnE0B.Bit4 : CA,CB�t�B���^�ݒ�		--> JL056HW.c											*/
/*		PnE0B.Bit5 : CA,CB�t�B���^�ݒ�		--> JL056HW.c											*/
/*		PnE0B.Bit8 : �w�߃p���X�t�B���^�ݒ� --> JL056HW.c											*/
/*		PnE0B.Bit9 : �w�߃p���X�t�B���^�ݒ� --> JL056HW.c											*/
/*		PnE0B.BitA : �w�߃p���X�t�B���^�ݒ� --> JL056HW.c											*/
/*		PnE0B.BitB : �w�߃p���X�t�B���^�ݒ� --> JL056HW.c											*/
/*--------------------------------------------------------------------------------------------------*/
/*		PnE0B.BitC : ���x����ϕ��ێ�																*/
/*		PnE0B.BitD : �رٴݺ��޸۸İ�		--> JL056HW.c											*/
/*		PnE0B.BitE : �G���R�[�_�ʒu�␳�I��1														*/
/*		PnE0B.BitF : �G���R�[�_�ʒu�␳�I��2														*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		PnE0C.Bit1 : �d�����͂R��																	*/
/*		PnE0C.Bit2 : �d���������o����																*/
/*		PnE0C.BitB : �q�[�g�V���N�ߔM���o����														*/
/*		PnE0C.BitC : ���x�㏸�c�a�ߕ��׌��o�L��														*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		PnE0D.Bit0 : ���[�U�萔�����݋֎~															*/
/*		PnE0D.Bit1 : ���[�U�A�N�Z�X���x���I��														*/
/*		PnE0D.Bit4 : ���_�ʒu�ݒ���s																*/
/*		PnE0D.Bit5 : �d����ʑ��ʒu�ۑ����s															*/
/*		PnE0D.Bit8 : �����o�͑I��																	*//*<S02D>*/
/*		PnE39.Bit2 : ���H�d���␳�@�\�I��		<V079>												*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalSystemSwitch( AXIS_HANDLE *Axis )
{
	PRMDATA				*Prm;
	CMN_PRMDATA			*Cprm;
	FUN_AXCOMMON	*AxCommon;				/* <S056> */

	Prm		= Axis->UniPrms->Prm;
	Cprm	= Axis->UniPrms->Cprm;
	AxCommon = Axis->FnManager->AxCommon;	/* <S056> */


/*--------------------------------------------------------------------------------------------------*/
/*		PnE0B : �V�X�e���X�C�b�`																	*/
/*--------------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------*/
	/*		BitC : ���x����ϕ��ێ�																	*/
	/*----------------------------------------------------------------------------------------------*/
	if( Prm->syssw1 & 0x1000 )
	{
		Axis->BaseControls->CtrlCmdPrm.SpdIctrlKeep = TRUE;
	}
	else
	{
		Axis->BaseControls->CtrlCmdPrm.SpdIctrlKeep = FALSE;
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		BitE : �G���R�[�_�ʒu�␳�I��(0:�␳����,1:�␳���Ȃ�)									*/
	/*		BitF : �G���R�[�_�ʒu�␳�I��(0:JL-086�ɂď���(�ʏ폈��),1:SW�ɂď���(��-�X�݊�))		*/
	/*----------------------------------------------------------------------------------------------*//*<S0C5>rename*/
	if( Prm->syssw1 & 0x4000 )
	{	/* �ʒu�␳���Ȃ� */
		(Axis->MencV)->PosCompMode = POSCOMP_NOP;
	}
	else
	{	/* �ʒu�␳���� */
		if( Prm->syssw1 & 0x8000 )
		{	/* SW�ɂď���		*/
			(Axis->MencV)->PosCompMode = POSCOMP_SW;
		}
		else
		{	/* JL-086�ɂď��� 	*/
			(Axis->MencV)->PosCompMode = POSCOMP_ASIC;
		}
	}
	(Axis->FencV)->PosCompMode = (Axis->MencV)->PosCompMode;		/* <S04D> */
/*--------------------------------------------------------------------------------------------------*/
/*		PnE0C : �V�X�e���X�C�b�`																	*/
/*--------------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------*/
	/*		Bit12 : ���x�㏸�ɂ��c�a�ߕ��׌��o�L��												*/
	/*----------------------------------------------------------------------------------------------*/
	if( Cprm->syssw2 & 0x1000 )
	{
		(Axis->CheckAlarm)->DbOL.conf.DbOHeatCheck = TRUE;
	}
	else
	{
		(Axis->CheckAlarm)->DbOL.conf.DbOHeatCheck = FALSE;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		PnE0D : �V�X�e���X�C�b�`																	*/
/*--------------------------------------------------------------------------------------------------*/

/* <S056>	Start */
	/*----------------------------------------------------------------------------------------------*/
	/*		Bit1 : ���[�U�A�N�Z�X���x���I��															*/
	/*----------------------------------------------------------------------------------------------*/
	if( Prm->syssw3  & 0x0002 )	/* PnE0D.1 = 1 ���[�U���x��3 */
	{
//		AxCommon->AccessLevel = 2;	/* ACCLVL_USER2 */	/* <S1B0> */
		AxCommon->AccessLevel = 3;	/* ACCLVL_USER3 */	/* <S1B0> */

	}
	else	/* PnE0D.1 = 0 ���[�U���x��1 */
	{
//		AxCommon->AccessLevel = 1;	/* ACCLVL_USER1 */
		AxCommon->AccessLevel = 2;	/* ACCLVL_USER2 *//* <S20B> */
	}
/* <S056>	End */

	/*----------------------------------------------------------------------------------------------*/
	/*		Bit8 : �����o�͑I��	(1:JL-086�ɂď���,0:SW�ɂď���(��-�X�݊�))							*//*<S02D>*/
	/*----------------------------------------------------------------------------------------------*/
	if( Prm->syssw3 & 0x0100 )
	{
		(Axis->MencV)->DivInfo.DivOutASIC = TRUE; 
	}
	else
	{
		(Axis->MencV)->DivInfo.DivOutASIC = FALSE;
	}

}


/****************************************************************************************************/
/*																									*/
/*		�[������~����p�����[�^�v�Z																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �[������~����p�̏����p�����[�^�v�Z���s��												*/
/*																									*/
/*			1) ���_�T�[�`���x																		*/
/*			   ���_�T�[�`�Ŏg�p����i�n�f���x�̌v�Z���s���B											*/
/*																									*/
/*			2) ���x�w�߃��~�b�g																		*/
/*			   �n�r�����̔����̊����ŁA���x�w�߃��~�b�g��0x1000000�^�n�r���x�ɐ��K������B			*/
/*			   �{���x�w�߃��~�b�g�́A�[���N�����v�y�ь��_�T�[�`�ɂĎg�p����B						*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalZeroStopControl( AXIS_HANDLE *Axis )
{
	LONG	SpeedX;
	BPRMDAT	*Bprm;

	Bprm	= Axis->UniPrms->Bprm;

/*--------------------------------------------------------------------------------------------------*/
/*		���_�T�[�`���x�̌v�Z : 60.0r/min, 15.0mm/s --> NorSpeed [2^24/OvrSpd]						*/
/*--------------------------------------------------------------------------------------------------*/
	if( Axis->MencV->AxisMotType == MOTTYPE_LINEAR )
	{ /* ���j�A�^ */
		SpeedX = 15;											/* ���j�A�^���[�^ : 15.0 r/min		*/
	}
	else
	{ /* ��]�^ */
		SpeedX = 60;											/* ��]�^���[�^ : 60.0 r/min		*/
	}
/*--------------------------------------------------------------------------------------------------*/
	if( Axis->MencV->LowSpdMotor )
	{ /* �ᑬ���[�^(�c�c���[�^) */
		;
	}
	else
	{ /* �ᑬ���[�^(�c�c���[�^)�ȊO */
		SpeedX = 10 * SpeedX;
	}
/*--------------------------------------------------------------------------------------------------*/
	/* ���K�����x�v�Z [2^24/OvrSpd] */
	Axis->BaseControls->ZeroClampCtrl.conf.OrgSrchSpeed = BprmSpeedLevelCal( Bprm, SpeedX, 0 );

/*--------------------------------------------------------------------------------------------------*/
/*		���x�w�߃��~�b�g�̌v�Z																		*/
/*--------------------------------------------------------------------------------------------------*/
	Axis->BaseControls->ZeroClampCtrl.conf.SpdLimit = 0x01000000 - (Bprm->PerOvrSpd * 838);
	Axis->BaseLoops->ZxCtrl.P.SpdLimit = Axis->BaseControls->ZeroClampCtrl.conf.SpdLimit;			/* <S�PC2> */
}



/****************************************************************************************************/
/*																									*/
/*		�L�����A���g���؂�ւ��p�����[�^�v�Z									<V057>				*/
/*																									*/
/****************************************************************************************************/
void	IprmcalCarrerChange( AXIS_HANDLE *Axis )
{
	PRMDATA		*Prm;
//	CMN_PRMDATA	*Cprm;
	CARRCHNG	*CarrerChange;
	LONG		MotStopDef;		/* ���[�^��~���o���x */
	LONG		kx, sx, sx2;
	USHORT		Imax;
	SHORT		tmp_ilim;

	Prm = Axis->UniPrms->Prm;
//	Cprm = Axis->UniPrms->Cprm;
	CarrerChange = &(Axis->BaseLoops->CarrerChange);
	MotStopDef = 60000;

	/* �L�����A���g���؂�ւ��@�\�X�C�b�`�t���O */
	if( (Prm->b_prm9 & 0x0010) && (Prm->pwmf <= 4000) )
	{ /* (Pn009.1 == 1) && (PnE2C <= 4000) */
		CarrerChange->conf.CarrChange = TRUE;
	}
	else
	{
		CarrerChange->conf.CarrChange = FALSE;
	}

	/* �ő�d��Imax���` */
	if( Prm->MencP.imax <= Prm->imaxs )
	{
		Imax = Prm->MencP.imax;
	}
	else
	{
		Imax = Prm->imaxs;
	}

#if (FLOAT_USE==TRUE)
	/* �g���N臒lA�̌v�Z�FPnEA6 */
	kx = 15000 * Prm->MencP.irat / Imax;
	tmp_ilim = (SHORT)(kx * Prm->ilimlvl / 100.0f);
	CarrerChange->conf.IqLimitA = (LONG)tmp_ilim * (LONG)tmp_ilim;
		
	/* �g���N臒lA�̌v�Z�FPnEA6+10% */
	tmp_ilim = (SHORT)( kx * (Prm->ilimlvl+10) / 100.0f);
	CarrerChange->conf.IqLimitB = (LONG)tmp_ilim * (LONG)tmp_ilim;
#else
	/* �g���N臒lA�̌v�Z�FPnEA6 */
	kx = MlibScalKxgain( 15000, Prm->MencP.irat, Imax, &sx, 0 );
	sx2 = sx;
	tmp_ilim = (SHORT)MlibPcalKxgain( kx, Prm->ilimlvl, 100, &sx, -24 );
	CarrerChange->conf.IqLimitA = (LONG)tmp_ilim * (LONG)tmp_ilim;
		
	/* �g���N臒lA�̌v�Z�FPnEA6+10% */
	tmp_ilim = (SHORT)MlibPcalKxgain( kx, Prm->ilimlvl+10, 100, &sx2, -24 );
	CarrerChange->conf.IqLimitB = (LONG)tmp_ilim * (LONG)tmp_ilim;
#endif /* FLOAT_USE */
		
	/* ���x臒lA�̌v�Z�FMOTSPD_DBLVL/2 */
	CarrerChange->conf.VelLimitA = MotStopDef >> 1;
		
	/* ���x臒lB�̌v�Z�FMOTSPD_DBLVL */
	CarrerChange->conf.VelLimitB = MotStopDef;

	/* �ʏ�L�����A�J�E���g�I��		SGD7W IO���o�͑Ή�<S118> */
#if 0	/* ���[�^�L�����A���g���������Ă���ꍇ�͏�Ɋm�F���邽�߃V�X�e���X�C�b�`�s�v<S11E> */
	if( (Cprm->syssw2 & 0x2000) != 0 )
	{/* @@zantei �G���R�[�_�p�����[�^�ɂ��L�����A�؂�ւ��@�\�L��(PnE0C.bit13) */
		if( (0 == Prm->MencP.motfuncf.f.chgPwm) && (0 != Prm->MencP.mot_pwmf) )
		{/* PnF24.bit0=0�Ń��[�^PWM���g��(PnF25)��0�łȂ���΃��[�^���g���ƃA���v���g���̒Ⴂ�����g�p���� */
			if( Prm->MencP.mot_pwmf >= Prm->pwmf )
			{/* ���[�^PWM>=�A���vPWM�̏ꍇ�A�A���vPWM���g�p */
				CarrerChange->conf.MotCarrUse = FALSE;
			}
			else
			{/* ���[�^PWM<�A���vPWM�̏ꍇ�A���[�^PWM���g�p */
				CarrerChange->conf.MotCarrUse = TRUE;
			}
		}
		else
		{
			CarrerChange->conf.MotCarrUse = FALSE;
		}
	}
	else
	{
		CarrerChange->conf.MotCarrUse = FALSE;
	}
#else	/* #if 0	 ���[�^�L�����A���g���������Ă���ꍇ�͏�Ɋm�F���邽�߃V�X�e���X�C�b�`�s�v<S11E> */
	if( 0 == Prm->MencP.motfuncf.f.chgPwm )
	{/* PnF24.bit0=0�̏ꍇ�A���[�^�L�����A���g���̒l���m�F���āA�ǂ���̃L�����A���g�����g�����I������ */
		if( (0 < Prm->MencP.mot_pwmf) || (16000 >= Prm->MencP.mot_pwmf) )		/* ���16KH���Ƃ���(�R�[�h����)  <S191> */
		{/* ���[�^�L�����APWM���g�����͈͓��̏ꍇ�A���[�^���g�����g�p */
			CarrerChange->conf.MotCarrUse = TRUE;
		}
		else
		{/* ���[�^�L�����APWM���g�����͈͊O�̏ꍇ�A�A���v���g�����g�p */
			CarrerChange->conf.MotCarrUse = FALSE;
		}
	}
	else
	{/* PnF24.bit0=1�ł���΃A���v���g�����g�p���� */
		CarrerChange->conf.MotCarrUse = FALSE;
	}
#endif	/* #if 0	 ���[�^�L�����A���g���������Ă���ꍇ�͏�Ɋm�F���邽�߃V�X�e���X�C�b�`�s�v<S11E> */
	LpxMakeCarrCnt( Prm, CarrerChange );			/* �L�����A���g���J�E���g�쐬 */

	return;
}



/****************************************************************************************************/
/*																									*/
/*		�L�����A���g���J�E���g�쐬����							SGD7W IO���o�͑Ή�<S118>					*/
/*																									*/
/****************************************************************************************************/
static void	LpxMakeCarrCnt( PRMDATA *Prm, CARRCHNG *CarrerChange )
{
	if( TRUE == CarrerChange->conf.MotCarrUse )
	{/* ���[�^PWM���g�p */
		/* PnF25�Őݒ肷��ʏ�L�����A�J�E���g�l�̌v�Z */
		CarrerChange->conf.NormCarrCnt =
			(SHORT)KpiChangePwmFrequency( 0, Prm->MencP.mot_pwmf, Prm->pwmf3, Prm->MencP.flg_wf );

		/* PnE25�Őݒ肷�鍂�L�����A�J�E���g�l�̌v�Z */
		CarrerChange->conf.HighCarrCnt =
			(SHORT)KpiChangePwmFrequency( Prm->pwmf2, Prm->MencP.mot_pwmf, Prm->pwmf3, Prm->MencP.flg_wf );
	}
	else
	{/* �A���vPWM���g�p */
		/* PnE2C�Őݒ肷��ʏ�L�����A�J�E���g�l�̌v�Z */
		CarrerChange->conf.NormCarrCnt =
			(SHORT)KpiChangePwmFrequency( 0, Prm->pwmf, Prm->pwmf3, Prm->MencP.flg_wf );

		/* PnE25�Őݒ肷�鍂�L�����A�J�E���g�l�̌v�Z */
		CarrerChange->conf.HighCarrCnt =
			(SHORT)KpiChangePwmFrequency( Prm->pwmf2, Prm->pwmf, Prm->pwmf3, Prm->MencP.flg_wf );
	}

	CarrerChange->conf.CarrCnt = CarrerChange->conf.NormCarrCnt;		/* �L���L�����A�J�E���g�Z�b�g 	*/

	return;
}


/****************************************************************************************************/
/*																									*/
/*		�V�[�P���X���̓}�b�v�v�Z																	*/
/*																									*/
/****************************************************************************************************/
#if 0	/* <S045> */
static	void	IprmcalSeqInputMap( AXIS_HANDLE *Axis )
{
	PRMDATA				*Prm;
#if 0/* ���ݽIO�͖��Ή� */
	BPRMDAT				*Bprm;
	SERVO_CONTROL_IF	*SvControlIf;
	SHAL_LTSIGNALS		*LatchSignals;
	ULONG				i;
#endif
	SEQ_IOSIGNAL		*IoSignal;

	Prm	= Axis->UniPrms->Prm;
#if 0/* ���ݽIO�͖��Ή� */
	Bprm= Axis->UniPrms->Bprm;
	LatchSignals = Axis->LatchSignals;
	SvControlIf = Axis->SvControlIf;
#endif

	/* P-OT mapping */
	IoSignal = &(Axis->IoSignals->PotSignal);
	switch(Prm->pot_selsw >> 12)
	{
	case 1:	/* Switch OFF => active(Normal) */
		IoSignal->Mask		= 1;
		IoSignal->XorMask   = 1;
		IoSignal->PinNumber = (ULONG)(Prm->pot_selsw & 0xFF);
		break;
	case 2:	/* Switch OFF => inactive(Reverse) */
		IoSignal->Mask		= 1;
		IoSignal->XorMask   = 0;
		IoSignal->PinNumber = (ULONG)(Prm->pot_selsw & 0xFF);
		break;
	default:/* Always inactive */
		IoSignal->Mask		= 0;
		IoSignal->XorMask   = 0;
		IoSignal->PinNumber = 0;
		break;
	}

	/* N-OT mapping */
	IoSignal = &(Axis->IoSignals->NotSignal);
	switch(Prm->not_selsw >> 12)
	{
	case 1:	/* Switch OFF => active(Normal) */
		IoSignal->Mask		= 1;
		IoSignal->XorMask   = 1;
		IoSignal->PinNumber = (ULONG)(Prm->not_selsw & 0xFF);
		break;
	case 2:	/* Switch OFF => inactive(Reverse) */
		IoSignal->Mask		= 1;
		IoSignal->XorMask   = 0;
		IoSignal->PinNumber = (ULONG)(Prm->not_selsw & 0xFF);
		break;
	default:/* Always inactive */
		IoSignal->Mask		= 0;
		IoSignal->XorMask   = 0;
		IoSignal->PinNumber = 0;
		break;
	}

	/* DEC mapping */
	IoSignal = &(Axis->IoSignals->DecSignal);
	switch(Prm->dec_selsw >> 12)
	{
	case 1:	/* Switch ON => active(Normal) */
		IoSignal->Mask		= 1;
		IoSignal->XorMask   = 0;
		IoSignal->PinNumber = (ULONG)(Prm->dec_selsw & 0xFF);
		break;
	case 2:	/* Switch ON => inactive(Reverse) */
		IoSignal->Mask		= 1;
		IoSignal->XorMask   = 1;
		IoSignal->PinNumber = (ULONG)(Prm->dec_selsw & 0xFF);
		break;
	default:/* Always inactive */
		IoSignal->Mask		= 0;
		IoSignal->XorMask   = 0;
		IoSignal->PinNumber = 0;
		break;
	}

#if	(CSW_SVCF_FBLATCH == TRUE)
	/* <S04D>:�t���N���[�Y�h�G���R�[�_�Ή��̂��ߑ������ύX */
	if(Axis->UniPrms->Bprm->FencUse != FALSE)
	{
		axisHwReg = Axis->FencV->EncIf.PosCmpSetReg;
	}
	else
	{
		axisHwReg = Axis->MencV->EncIf.PosCmpSetReg;
	}
	if(TRUE != SHalDrv_InitLatchSignal(axisHwReg, Axis->LtStsAdr, Prm->inselA, Axis->SvControlIf->ExtLtEnable))
	{
		/* A.040 : �p�����[�^�ݒ�ُ� */
//		ALMSetPramError( Axis->AlmManager, pndef_midtrq.RegNumber );
		ALMSetPramError( Axis->AlmManager, pndef_inselA.RegNumber );	/* <S226> */
	}
#endif

#ifdef CSW_BSP_SGDVMD_B01	/* <S053> */
	/* E-STOP mapping */
	IoSignal = &(Axis->IoSignals->EstpSignal);
	switch(Prm->emg_selsw >> 12)
	{
	case 1:	/* Switch ON => active(Normal) */
		IoSignal->Mask		= 1;
		IoSignal->XorMask   = 0;
		IoSignal->PinNumber = (ULONG)(Prm->emg_selsw & 0xFF);
		break;
	case 2:	/* Switch ON => inactive(Reverse) */
		IoSignal->Mask		= 1;
		IoSignal->XorMask   = 1;
		IoSignal->PinNumber = (ULONG)(Prm->emg_selsw & 0xFF);
		break;
	default:/* Always inactive */
		IoSignal->Mask		= 0;
		IoSignal->XorMask   = 0;
		IoSignal->PinNumber = 0;
		break;
	}
#endif

#ifdef CSW_BSP_SGDVMD_B01 /* <S053> */
	/* BK mapping */
	IoSignal = &(Axis->IoSignals->BkSignal);
	switch(Prm->bk_selsw >> 12)
	{
	case 1:	/* Active low(Normal) */
		IoSignal->Mask		= 1;
		IoSignal->XorMask   = 0;
		IoSignal->PinNumber = KPX_SO1BITNO;
		break;
	case 2:	/* Active high(Reverse) */ // Active High�͊댯�Ȃ̂łȂ�
		IoSignal->Mask		= 1;
		IoSignal->XorMask   = 1;
		IoSignal->PinNumber = KPX_SO1BITNO;
		break;
	default:/* Always inactive */
		IoSignal->Mask		= 0;
		IoSignal->XorMask   = 0;
		IoSignal->PinNumber = 31;	/* Selects a reserved pin */
		break;
	}
#else
	/* BK mapping */
	IoSignal = &(Axis->IoSignals->BkSignal);
	switch(Prm->bk_selsw >> 12)
	{
	case 1:	/* Active low(Normal) */
		IoSignal->Mask		= 1;
		IoSignal->XorMask   = 0;
		IoSignal->PinNumber = (ULONG)(Prm->bk_selsw & 0xFF);
		break;
	case 2:	/* Active high(Reverse) */ // Active High�͊댯�Ȃ̂łȂ�
		IoSignal->Mask		= 1;
		IoSignal->XorMask   = 1;
		IoSignal->PinNumber = (ULONG)(Prm->bk_selsw & 0xFF);
		break;
	default:/* Always inactive */
		IoSignal->Mask		= 0;
		IoSignal->XorMask   = 0;
		IoSignal->PinNumber = 31;	/* Selects a reserved pin */
		break;
	}
#endif

}
#endif

/****************************************************************************************************/
/*																									*/
/*		�V�[�P���X���̓}�b�v�v�Z(M2,M3,CMD��p) <S045>												*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalSeqInputMap( AXIS_HANDLE *Axis )
{
#define Lpx_GetDefInSig( prm, pin ) (((((prm) & 0x0F) < 7) || (((prm) & 0x0F) > 8)) ? (pin) : ((prm) & 0x0F))
#if CSW_BOARD_TYPE == SGD7W_TYPE	/* <S200-0> */
#define Lpx_GetExtInSig( prm )      ((((prm) & 0x0F) < 3) || ((((prm) & 0x0F) > 8) && (((prm) & 0x0F) < 0x0C))) \
									? 8 : ((prm) & 0x0F)
#else
#define Lpx_GetExtInSig( prm )      ((((prm) & 0x0F) < 4) || ((((prm) & 0x0F) > 8) && (((prm) & 0x0F) < 0x0D))) \
									? 8 : ((prm) & 0x0F)
#endif
	LONG				Mode;
	PRMDATA				*Prm;

	Prm	= Axis->UniPrms->Prm;
/*--------------------------------------------------------------------------------------------------*/
/*		���͐M�����t���[�h																			*/
/*--------------------------------------------------------------------------------------------------*/
	Mode = Prm->insel1 & 0x0F;

	if ( Mode == 2 )
	{
		LpxSetInputMapW( Axis );						/* Only SGD7W */
	}
	else
	{
		LpxSetInputMapCmn( Axis, Mode );				/* Common */
	}

/*--------------------------------------------------------------------------------------------------*/
/* <S04A> Estp?�M������t���B�O�̂���															 	*/
/*--------------------------------------------------------------------------------------------------*/
#ifdef CSW_BSP_SGDVMD_IFECAT						
	/* E-STOP mapping */
	IoSignal = &(Axis->IoSignals->EstpSignal);
	switch(Prm->emg_selsw >> 12)
	{
	case 1:	/* Switch ON => active(Normal) */
		IoSignal->Mask		= 1;
		IoSignal->XorMask   = 0;
		IoSignal->PinNumber = (ULONG)(Prm->emg_selsw & 0xFF);
		break;
	case 2:	/* Switch ON => inactive(Reverse) */
		IoSignal->Mask		= 1;
		IoSignal->XorMask   = 1;
		IoSignal->PinNumber = (ULONG)(Prm->emg_selsw & 0xFF);
		break;
	default:/* Always inactive */
		IoSignal->Mask		= 0;
		IoSignal->XorMask   = 0;
		IoSignal->PinNumber = 0;
		break;
	}
#endif

}

/****************************************************************************************************/
/*																									*/
/*		�V�[�P���X���̓}�b�v�v�Z(����) <S045>														*/
/*																									*/
/****************************************************************************************************/
void	LpxSetInputMapCmn( AXIS_HANDLE *Axis, LONG Mode )
{
LONG				lwk;
PRMDATA				*Prm;
SEQ_IO_HNDL	 		*IoSignals;

	Prm	= Axis->UniPrms->Prm;
	IoSignals = Axis->IoSignals;
/*--------------------------------------------------------------------------------------------------*/
/*		���͐M���f�t�H���g���t																		*/
/*--------------------------------------------------------------------------------------------------*/
	if ( Mode == 0 )
	{
	/*----------------------------------------------------------------------------------------------*/
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W�i2���Ή��j�̂�	 SGD7W IO���o�͑Ή�<S118> */
		lwk = Lpx_GetDefInSig( (Prm->insel1 >> 12), 1 );				/* P-OT (M2,M3,Cmd��SI1) */
		LpxPcalSetInputMap2( Axis, &(IoSignals->PotSignal), lwk, 0x01 );	/* P-OT (M2,M3,Cmd��SI1) */
		lwk = Lpx_GetDefInSig( (Prm->insel2 >> 0), 2 );					/* N-OT (M2,M3,Cmd��SI2) */
		LpxPcalSetInputMap2( Axis, &(IoSignals->NotSignal), lwk, 0x01 );	/* N-OT (M2,M3,Cmd��SI2) */
		LpxPcalSetInputMap2( Axis, &(IoSignals->PclSignal), 8, 0x00 );	/* P-CL (M2,M3,Cmd��NA) */
		LpxPcalSetInputMap2( Axis, &(IoSignals->NclSignal), 8, 0x00 );	/* N-CL (M2,M3,Cmd��NA) */
#elif CSW_BOARD_TYPE == SGD7S_TYPE	/*	SGD7S�i1���Ή��j�̂�	*/
		lwk = Lpx_GetDefInSig( (Prm->insel1 >> 12), 1 );			/* P-OT (M2,M3,Cmd��SI1) */
		LpxPcalSetInputMap( &(IoSignals->PotSignal), lwk, 0x01 );	/* P-OT (M2,M3,Cmd��SI1) */
		lwk = Lpx_GetDefInSig( (Prm->insel2 >> 0), 2 );				/* N-OT (M2,M3,Cmd��SI2) */
		LpxPcalSetInputMap( &(IoSignals->NotSignal), lwk, 0x01 );	/* N-OT (M2,M3,Cmd��SI2) */
		LpxPcalSetInputMap( &(IoSignals->PclSignal), 8, 0x00 );		/* P-CL (M2,M3,Cmd��NA) */
		LpxPcalSetInputMap( &(IoSignals->NclSignal), 8, 0x00 );		/* N-CL (M2,M3,Cmd��NA) */
#endif
	/*----------------------------------------------------------------------------------------------*/
	}
	else
	{
	/*----------------------------------------------------------------------------------------------*/
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W�i2���Ή��j�̂�  SGD7W IO���o�͑Ή�<S118> */
		LpxPcalSetInputMap2( Axis, &(IoSignals->PotSignal), (Prm->insel1 >> 12), 0x01 );	/* P-OT */
		LpxPcalSetInputMap2( Axis, &(IoSignals->NotSignal), (Prm->insel2 >> 0),  0x01 );	/* N-OT */
		LpxPcalSetInputMap2( Axis, &(IoSignals->PclSignal), (Prm->insel2 >> 8),  0x00 );	/* P-CL */
		LpxPcalSetInputMap2( Axis, &(IoSignals->NclSignal), (Prm->insel2 >> 12), 0x00 );	/* N-CL */
#elif CSW_BOARD_TYPE == SGD7S_TYPE	/*	SGD7S�i1���Ή��j�̂�	*/
		LpxPcalSetInputMap( &(IoSignals->PotSignal), (Prm->insel1 >> 12), 0x01 );	/* P-OT */
		LpxPcalSetInputMap( &(IoSignals->NotSignal), (Prm->insel2 >> 0),  0x01 );	/* N-OT */
		LpxPcalSetInputMap( &(IoSignals->PclSignal), (Prm->insel2 >> 8),  0x00 );	/* P-CL */
		LpxPcalSetInputMap( &(IoSignals->NclSignal), (Prm->insel2 >> 12), 0x00 );	/* N-CL */
#endif
	/*----------------------------------------------------------------------------------------------*/
	}
/*--------------------------------------------------------------------------------------------------*/
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W�i2���Ή��j�̂�  SGD7W IO���o�͑Ή�<S118> */
	LpxPcalSetInputMap2( Axis, &(IoSignals->DecSignal), (Prm->inselA >> 0), 0x00 );	/* DEC */
	lwk = Lpx_GetExtInSig( (Prm->inselA >> 4) );
	LpxPcalSetInputMap2( Axis, &(IoSignals->ExtSignal[0]), lwk , 0x00 );				/* EXT1 */
	lwk = Lpx_GetExtInSig( (Prm->inselA >> 8) );
	LpxPcalSetInputMap2( Axis, &(IoSignals->ExtSignal[1]), lwk , 0x00 );				/* EXT2 */
	lwk = Lpx_GetExtInSig( (Prm->inselA >> 12) );
	LpxPcalSetInputMap2( Axis, &(IoSignals->ExtSignal[2]), lwk , 0x00 );				/* EXT3 */
	lwk = /*Lpx_GetExtInSig( (*/Prm->insel6/* >> 0) )*/;		/* <S1F5> */
	LpxPcalSetInputMap2( Axis, &(IoSignals->FstpSignal), lwk , 0x01/* @@ 0x00 */); 					/* FSTP */   /* <S1F5> */
	/* <S21D> Start */
	LpxPcalSetInputMap2( Axis, &(IoSignals->ClinkansSignal), Prm->insel7 >> 0 , 0x01 ); 	/* /CLINKANS */
	LpxPcalSetInputMap2( Axis, &(IoSignals->CmcansSignal), Prm->insel7 >> 4 , 0x01 ); 		/* /CMCANS */
	LpxPcalSetInputMap2( Axis, &(IoSignals->ConvreadySignal), Prm->insel7 >> 8 , 0x01 ); 	/* /CONV_READY */
	LpxPcalSetInputMap2( Axis, &(IoSignals->ConvmconSignal), Prm->insel7 >> 12 , 0x01 ); 	/* /CONV_MCON */
	/* <S21D> End */
#elif CSW_BOARD_TYPE == SGD7S_TYPE	/*	SGD7S�i1���Ή��j�̂�	*/
	LpxPcalSetInputMap( &(IoSignals->DecSignal), (Prm->inselA >> 0), 0x00 );	/* DEC */
	lwk = Lpx_GetExtInSig( (Prm->inselA >> 4) );
	LpxPcalSetInputMap( &(IoSignals->ExtSignal[0]), lwk , 0x00 );				/* EXT1 */
	lwk = Lpx_GetExtInSig( (Prm->inselA >> 8) );
	LpxPcalSetInputMap( &(IoSignals->ExtSignal[1]), lwk , 0x00 );				/* EXT2 */
	lwk = Lpx_GetExtInSig( (Prm->inselA >> 12) );
	LpxPcalSetInputMap( &(IoSignals->ExtSignal[2]), lwk , 0x00 );				/* EXT3 */
	lwk = /*Lpx_GetExtInSig( (*/Prm->insel6 /*>> 0) )*/;	/* <S1F5> */
	LpxPcalSetInputMap( &(IoSignals->FstpSignal), lwk , 0x01/* @@ 0x00 */ ); 				/* FSTP */ /* <S1F5> */
	/* <S21D> Start */
	LpxPcalSetInputMap( &(IoSignals->ClinkansSignal), Prm->insel7 >> 0 , 0x00 ); 	/* /CLINKANS */
	LpxPcalSetInputMap( &(IoSignals->CmcansSignal), Prm->insel7 >> 4 , 0x00 ); 		/* /CMCANS */
	LpxPcalSetInputMap( &(IoSignals->ConvreadySignal), Prm->insel7 >> 8 , 0x00 ); 	/* /CONV_READY */
	LpxPcalSetInputMap( &(IoSignals->ConvmconSignal), Prm->insel7 >> 12 , 0x00 ); 	/* /CONV_MCON */
	/* <S21D> End */
#endif
/*--------------------------------------------------------------------------------------------------*/
/*	<S04A> Initialize External Signal Latch Assign													*/
/*--------------------------------------------------------------------------------------------------*/
#if	(CSW_SVCF_FBLATCH == TRUE)
	if(TRUE != SHalDrv_InitLatchSignal(Axis->SvAsicRegs->AsicHwReg, 
										Axis->LtStsAdr, Prm->inselA, Axis->SvControlIf->ExtLtEnable))
	{
		/* A.040 : �p�����[�^�ݒ�ُ� */
//		ALMSetPramError( Axis->AlmManager, pndef_midtrq.RegNumber );
		ALMSetPramError( Axis->AlmManager, pndef_inselA.RegNumber );	/* <S226> */
	}
#endif
/*--------------------------------------------------------------------------------------------------*/
	return;
}


/****************************************************************************************************/
/*																									*/
/*		�V�[�P���X���̓}�b�v�v�Z(SGD7W��p) <S1B5>													*/
/*																									*/
/****************************************************************************************************/
void	LpxSetInputMapW( AXIS_HANDLE *Axis )
{
PRMDATA				*Prm;
SEQ_IO_HNDL	 		*IoSignals;

	Prm	= Axis->UniPrms->Prm;
	IoSignals = Axis->IoSignals;
/*--------------------------------------------------------------------------------------------------*/
/*	Set Input Signal Map for SGD7W																	*/
/*--------------------------------------------------------------------------------------------------*/
	LpxPcalSetInputMap3( &(IoSignals->PotSignal), Prm->pot_selsw, 1 );
	LpxPcalSetInputMap3( &(IoSignals->NotSignal), Prm->not_selsw, 1 );
	LpxPcalSetInputMap3( &(IoSignals->DecSignal), Prm->dec_selsw, 0 );
	LpxPcalSetInputMap3( &(IoSignals->ExtSignal[0]), Prm->ext_selsw[0], 0 );
	LpxPcalSetInputMap3( &(IoSignals->ExtSignal[1]), Prm->ext_selsw[1], 0 );
	LpxPcalSetInputMap3( &(IoSignals->ExtSignal[2]), Prm->ext_selsw[2], 0 );
	/*----------------------------------------------------------------------------------------------*/
	LpxPcalSetInputMap3( &(IoSignals->FstpSignal), Prm->fstp_selsw, 1 );
	LpxPcalSetInputMap3( &(IoSignals->PclSignal), Prm->pcl_selsw, 0 );
	LpxPcalSetInputMap3( &(IoSignals->NclSignal), Prm->ncl_selsw, 0 );
	LpxPcalSetInputMap3( &(IoSignals->ClinkansSignal), Prm->clinkans_selsw, 0 );
	LpxPcalSetInputMap3( &(IoSignals->CmcansSignal), Prm->cmcans_selsw, 0 );
	LpxPcalSetInputMap3( &(IoSignals->ConvreadySignal), Prm->convready_selsw, 0 );
	LpxPcalSetInputMap3( &(IoSignals->ConvmconSignal), Prm->convmcon_selsw, 0 );
/*--------------------------------------------------------------------------------------------------*/
/*	Initialize External Signal Latch Assign															*/
/*--------------------------------------------------------------------------------------------------*/
	LpxInitLatchSignal( Axis );
	return;
}


/****************************************************************************************************/
/*																									*/
/*		�V�[�P���X���̓}�b�v�v�Z3(SGD7W��p) <S1B5>													*/
/*																									*/
/****************************************************************************************************/
void	LpxPcalSetInputMap3( SEQ_IOSIGNAL *IoSignal, ULONG prm, ULONG LogicH ) 
{
LONG	Pin;
ULONG	LogicX;
ULONG	LogicL = (LogicH) ? 0x00 : 0x01;
/*--------------------------------------------------------------------------------------------------*/
#define Lpx_GetBcd( prm )			((((prm) & 0x0F) * 1) 			/* <S1BF> */					\
									+ ((((prm)>>4) & 0x0F) * 10))
/*--------------------------------------------------------------------------------------------------*/
#define INPUT_MAP3_LGC_NONACTIVE 	(0)
#define INPUT_MAP3_LGC_NORMAL 		(1)
#define INPUT_MAP3_LGC_OPPOSITE 	(2)
#define INPUT_MAP3_LGC_ACTIVE 		(3)
/*--------------------------------------------------------------------------------------------------*/
	Pin = HALdrv_GetPsBitNumber( Lpx_GetBcd( prm ) );				/* Get Pin <S1BF> */
	LogicX = ( prm >> 12 ) & 0x0F;									/* Get Logic */
//	LogicX = (( Pin < 0 ) ? ((LogicH) ? INPUT_MAP3_LGC_ACTIVE 		/* Disable Pin <S1BF> */
//					: INPUT_MAP3_LGC_NONACTIVE) : LogicX);
	LogicX = (( Pin < 0 ) ? INPUT_MAP3_LGC_NONACTIVE : LogicX);		/* Disable Pin <S1F5> */
/*--------------------------------------------------------------------------------------------------*/
/*	Set Input Signal Map																			*/
/*--------------------------------------------------------------------------------------------------*/
	switch ( LogicX )
	{
	/*----------------------------------------------------------------------------------------------*/
	case INPUT_MAP3_LGC_NONACTIVE:									/* 0: Non Active				*/
	/*----------------------------------------------------------------------------------------------*/
			IoSignal->PinNumber = KPI_SINLOBITNO;
			IoSignal->Mask		= 0;
			IoSignal->XorMask	= 0;
			break;
	/*----------------------------------------------------------------------------------------------*/
	case INPUT_MAP3_LGC_NORMAL:										/* 1: Normal					*/
	/*----------------------------------------------------------------------------------------------*/
			IoSignal->PinNumber = (ULONG)Pin;
			IoSignal->Mask		= 1;
			IoSignal->XorMask	= LogicL;
			break;
	/*----------------------------------------------------------------------------------------------*/
	case INPUT_MAP3_LGC_OPPOSITE:									/* 2: Opposite					*/
	/*----------------------------------------------------------------------------------------------*/
			IoSignal->PinNumber = (ULONG)Pin;
			IoSignal->Mask		= 1;
			IoSignal->XorMask	= LogicH;
			break;
	/*----------------------------------------------------------------------------------------------*/
	case INPUT_MAP3_LGC_ACTIVE:										/* 3: Always active				*/
	/*----------------------------------------------------------------------------------------------*/
			IoSignal->PinNumber = KPI_SINHIBITNO;
			IoSignal->Mask		= 1;
			IoSignal->XorMask	= 0;
			break;
	/*----------------------------------------------------------------------------------------------*/
	default:
	/*----------------------------------------------------------------------------------------------*/
			IoSignal->PinNumber = KPI_SINLOBITNO;
			IoSignal->Mask		= 0;
			IoSignal->XorMask	= 0;
			break;
	}
/*--------------------------------------------------------------------------------------------------*/
	return;
}

/****************************************************************************************************/
/*																									*/
/*		<S1B5> Get insel Parameter Number															*/
/*																									*/
/****************************************************************************************************/
LONG	LpxGetInSel( LONG Axis, ULONG prm )
{
LONG	wkx;
LONG	Pin;
ULONG	LogicX;
/*--------------------------------------------------------------------------------------------------*/
	Pin = HALdrv_GetPsBitNumber( Lpx_GetBcd( prm ) );				/* Get Pin <S1BF> */
	LogicX = ( prm >> 12 ) & 0x0F;									/* Get Logic */
	LogicX = (( Pin < 0 ) ? INPUT_MAP3_LGC_NONACTIVE : LogicX);		/* Disable Pin */
/*--------------------------------------------------------------------------------------------------*/
	if ( LogicX == INPUT_MAP3_LGC_NONACTIVE )
	{
		return (8);													/* Not Available */
	}
	else if ( ( LogicX == INPUT_MAP3_LGC_ACTIVE ) )
	{
		return (7);
	}
	else if ( LogicX == INPUT_MAP3_LGC_NORMAL ) 
	{
		wkx = 0;
	}
	else if ( LogicX == INPUT_MAP3_LGC_OPPOSITE )
	{
		wkx = 9;
	}
	else 
	{
		return (8);													/* Not Available */
	}
/*--------------------------------------------------------------------------------------------------*/
	if ( Axis == 0 ) 												/* Axis 0 */
	{
		switch ( Pin )
		{
		case KPI_SI5BITNO: 	wkx++;
		case KPI_SI4BITNO: 	wkx++;
		case KPI_SI3BITNO: 	wkx++;
		case KPI_SI2BITNO: 	wkx++;
		case KPI_SI1BITNO: 	wkx++;
		case KPI_SI0BITNO: 	break;
		default: return (8);										/* Not available */
		}
	}
	else															/* Axis 1 */
	{
		switch ( Pin )
		{
		case KPI_SI11BITNO:	wkx++;
		case KPI_SI10BITNO:	wkx++;
		case KPI_SI9BITNO: 	wkx++;
		case KPI_SI8BITNO: 	wkx++;
		case KPI_SI7BITNO: 	wkx++;
		case KPI_SI6BITNO: 	break;
		default: return (8);										/* Not available */
		}
	}
	/*----------------------------------------------------------------------------------------------*/
	return (wkx);
}

/****************************************************************************************************/
/*																									*/
/*		���b�`�M�����t(SGD7W��p) <S1B5>															*/
/*																									*/
/****************************************************************************************************/
void	LpxInitLatchSignal( AXIS_HANDLE *Axis )
{
LONG				inselA;
PRMDATA				*Prm;

	Prm	= Axis->UniPrms->Prm;
/*--------------------------------------------------------------------------------------------------*/
/*	Make inselA																						*/
/*--------------------------------------------------------------------------------------------------*/
	inselA = 0;
	inselA |= (LpxGetInSel( Axis->AxisNo, Prm->ext_selsw[0] ) << 4 );
	inselA |= (LpxGetInSel( Axis->AxisNo, Prm->ext_selsw[1] ) << 8 );
	inselA |= (LpxGetInSel( Axis->AxisNo, Prm->ext_selsw[2] ) << 12);
/*--------------------------------------------------------------------------------------------------*/
/*	 Initialize External Signal Latch Assign														*/
/*--------------------------------------------------------------------------------------------------*/
#if	(CSW_SVCF_FBLATCH == TRUE)
	if(TRUE != SHalDrv_InitLatchSignal(Axis->SvAsicRegs->AsicHwReg, 
										Axis->LtStsAdr, inselA, Axis->SvControlIf->ExtLtEnable))
	{
		/* A.040 : �p�����[�^�ݒ�ُ� */
//		ALMSetPramError( Axis->AlmManager, pndef_midtrq.RegNumber );
		ALMSetPramError( Axis->AlmManager, pndef_inselA.RegNumber );	/* <S226> */
	}
#endif
}

/****************************************************************************************************/
/*																									*/
/*		�V�[�P���X���̓}�b�v�v�Z(M2,M3,CMD��p) <S045>												*/
/*																									*/
/****************************************************************************************************/
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W�i2���Ή��j�̂� SGD7W IO���o�͑Ή�<S118>	*/
static	void	LpxPcalSetInputMap2( AXIS_HANDLE *Axis, SEQ_IOSIGNAL *IoSignal, ULONG prm, ULONG LogicH )
#elif CSW_BOARD_TYPE == SGD7S_TYPE	/*	SGD7S�i1���Ή��j�̂�	*/
static	void	LpxPcalSetInputMap( SEQ_IOSIGNAL *IoSignal, ULONG prm, ULONG LogicH ) 
#endif
{
ULONG	LogicL = (LogicH) ? 0x00 : 0x01;
/*--------------------------------------------------------------------------------------------------*/
	IoSignal->Mask		= 1;
/*--------------------------------------------------------------------------------------------------*/
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W�i2���Ή��j�̂�	SGD7W IO���o�͑Ή�<S118>	*/
	if(Axis->AxisNo == 0 )	/*	1����	*/
	{	/*	1����	*/
		switch ( prm & 0x0F )
		{
			case 0x00: IoSignal->PinNumber = KPI_SI0BITNO; IoSignal->XorMask = LogicL; break;
			case 0x01: IoSignal->PinNumber = KPI_SI1BITNO; IoSignal->XorMask = LogicL; break;
			case 0x02: IoSignal->PinNumber = KPI_SI2BITNO; IoSignal->XorMask = LogicL; break;
			case 0x03: IoSignal->PinNumber = KPI_SI3BITNO; IoSignal->XorMask = LogicL; break;
			case 0x04: IoSignal->PinNumber = KPI_SI4BITNO; IoSignal->XorMask = LogicL; break;
			case 0x05: IoSignal->PinNumber = KPI_SI5BITNO; IoSignal->XorMask = LogicL; break;
//-default��	case 0x06: IoSignal->PinNumber = KPI_SI6BITNO; IoSignal->XorMask = LogicL; break;
			case 0x07:
			default  : IoSignal->PinNumber = KPI_SINHIBITNO; IoSignal->XorMask = 0x00; break;
			case 0x08: IoSignal->PinNumber = KPI_SINLOBITNO; IoSignal->XorMask = 0x00; break;
			case 0x09: IoSignal->PinNumber = KPI_SI0BITNO; IoSignal->XorMask = LogicH; break;
			case 0x0A: IoSignal->PinNumber = KPI_SI1BITNO; IoSignal->XorMask = LogicH; break;
			case 0x0B: IoSignal->PinNumber = KPI_SI2BITNO; IoSignal->XorMask = LogicH; break;
			case 0x0C: IoSignal->PinNumber = KPI_SI3BITNO; IoSignal->XorMask = LogicH; break;
			case 0x0D: IoSignal->PinNumber = KPI_SI4BITNO; IoSignal->XorMask = LogicH; break;
			case 0x0E: IoSignal->PinNumber = KPI_SI5BITNO; IoSignal->XorMask = LogicH; break;
//--default��case 0x0F: IoSignal->PinNumber = KPI_SI6BITNO; IoSignal->XorMask = LogicH; break;
		}
	}else
	{	/*	2����	*/
		switch ( prm & 0x0F )
		{
			case 0x00: IoSignal->PinNumber = KPI_SI0BITNO - 6; IoSignal->XorMask = LogicL; break;
			case 0x01: IoSignal->PinNumber = KPI_SI1BITNO - 6; IoSignal->XorMask = LogicL; break;
			case 0x02: IoSignal->PinNumber = KPI_SI2BITNO - 6; IoSignal->XorMask = LogicL; break;
			case 0x03: IoSignal->PinNumber = KPI_SI3BITNO - 6; IoSignal->XorMask = LogicL; break;
			case 0x04: IoSignal->PinNumber = KPI_SI4BITNO - 6; IoSignal->XorMask = LogicL; break;
			case 0x05: IoSignal->PinNumber = KPI_SI5BITNO - 6; IoSignal->XorMask = LogicL; break;
//-default��	case 0x06: IoSignal->PinNumber = KPI_SI6BITNO; IoSignal->XorMask = LogicL; break;
			case 0x07:
			default  : IoSignal->PinNumber = KPI_SINHIBITNO; IoSignal->XorMask = 0x00; break;
			case 0x08: IoSignal->PinNumber = KPI_SINLOBITNO; IoSignal->XorMask = 0x00; break;
			case 0x09: IoSignal->PinNumber = KPI_SI0BITNO - 6; IoSignal->XorMask = LogicH; break;
			case 0x0A: IoSignal->PinNumber = KPI_SI1BITNO - 6; IoSignal->XorMask = LogicH; break;
			case 0x0B: IoSignal->PinNumber = KPI_SI2BITNO - 6; IoSignal->XorMask = LogicH; break;
			case 0x0C: IoSignal->PinNumber = KPI_SI3BITNO - 6; IoSignal->XorMask = LogicH; break;
			case 0x0D: IoSignal->PinNumber = KPI_SI4BITNO - 6; IoSignal->XorMask = LogicH; break;
			case 0x0E: IoSignal->PinNumber = KPI_SI5BITNO - 6; IoSignal->XorMask = LogicH; break;
//-default��	case 0x0F: IoSignal->PinNumber = KPI_SI6BITNO; IoSignal->XorMask = LogicH; break;
		}
	}
#elif CSW_BOARD_TYPE == SGD7S_TYPE	/*	SGD7S�i1���Ή��j�̂�	*/
	switch ( prm & 0x0F )
	{
		case 0x00: IoSignal->PinNumber = KPI_SI0BITNO; IoSignal->XorMask = LogicL; break;
		case 0x01: IoSignal->PinNumber = KPI_SI1BITNO; IoSignal->XorMask = LogicL; break;
		case 0x02: IoSignal->PinNumber = KPI_SI2BITNO; IoSignal->XorMask = LogicL; break;
		case 0x03: IoSignal->PinNumber = KPI_SI3BITNO; IoSignal->XorMask = LogicL; break;
		case 0x04: IoSignal->PinNumber = KPI_SI4BITNO; IoSignal->XorMask = LogicL; break;
		case 0x05: IoSignal->PinNumber = KPI_SI5BITNO; IoSignal->XorMask = LogicL; break;
		case 0x06: IoSignal->PinNumber = KPI_SI6BITNO; IoSignal->XorMask = LogicL; break;
		case 0x07:
		default  : IoSignal->PinNumber = KPI_SINHIBITNO; IoSignal->XorMask = 0x00; break;
		case 0x08: IoSignal->PinNumber = KPI_SINLOBITNO; IoSignal->XorMask = 0x00; break;
		case 0x09: IoSignal->PinNumber = KPI_SI0BITNO; IoSignal->XorMask = LogicH; break;
		case 0x0A: IoSignal->PinNumber = KPI_SI1BITNO; IoSignal->XorMask = LogicH; break;
		case 0x0B: IoSignal->PinNumber = KPI_SI2BITNO; IoSignal->XorMask = LogicH; break;
		case 0x0C: IoSignal->PinNumber = KPI_SI3BITNO; IoSignal->XorMask = LogicH; break;
		case 0x0D: IoSignal->PinNumber = KPI_SI4BITNO; IoSignal->XorMask = LogicH; break;
		case 0x0E: IoSignal->PinNumber = KPI_SI5BITNO; IoSignal->XorMask = LogicH; break;
		case 0x0F: IoSignal->PinNumber = KPI_SI6BITNO; IoSignal->XorMask = LogicH; break;
	}
#endif
/*--------------------------------------------------------------------------------------------------*/
}

/****************************************************************************************************/
/*																									*/
/*		�V�[�P���X�o�̓}�b�v�v�Z(M2,M3,CMD��p) <S045>												*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalSeqOutputMap( AXIS_HANDLE *Axis )
{
/*--------------------------------------------------------------------------------------------------*/
#define LPX_SETSOUTPTR( signal, ptr ) 	\
		SeqIo = &(IoSignals->##signal); \
		SeqIo->SignalStatus.p = (ptr);
/*--------------------------------------------------------------------------------------------------*/
	PRMDATA				*Prm;
	SEQ_IOSIGNAL		*SeqIo;
	SEQ_IO_HNDL	 		*IoSignals;
	BASE_CTRL_HNDL		*BaseControls;
	SEQ_CTRL_OUT		*SeqCtrlOut;
	BE_SEQ_HNDL			*BeSequence;
	ALMMAN_HANDLE		*AlmManager;
	CHECK_MAIN_POWER	*PowerChecker;/* <S154> */

	Prm	= Axis->UniPrms->Prm;
	IoSignals = Axis->IoSignals;
	BaseControls = Axis->BaseControls;
	SeqCtrlOut = Axis->SeqCtrlOut;
	BeSequence = Axis->BeSequence;
	AlmManager = Axis->AlmManager;
	PowerChecker = &(Axis->PowerManager->MainPowerChecker);/* <S154> */

/*--------------------------------------------------------------------------------------------------*/
/*		���͐M�����t���[�h																			*/
/*--------------------------------------------------------------------------------------------------*/
	if ( ( Prm->insel1 & 0x0F ) == 2 )
	{
		LpxSetOutputMapW( Axis );						/* Only SGD7W */
	}
	else
	{
		LpxSetOutputMapCmn( Axis );						/* Common */
	}

//<S081>  #if CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE /* <S04B> */
/*--------------------------------------------------------------------------------------------------*/
/* Set Output Signal Pointers (M2/M3/CMD Only) 														*/
/*--------------------------------------------------------------------------------------------------*/
	LPX_SETSOUTPTR( CoinSignal, &(BaseControls->PosCtrlSts.CoinSignal) );	/* /COIN */
	LPX_SETSOUTPTR( VcmpSignal, &(SeqCtrlOut->VcmpSts) );					/* /VCMP */
	LPX_SETSOUTPTR( TgonSignal, &(SeqCtrlOut->TgOnSts) );					/* /TGON */
	LPX_SETSOUTPTR( SrdySignal, &(BeSequence->ServoReady) );				/* /S-RDY */
/*--------------------------------------------------------------------------------------------------*/
	LPX_SETSOUTPTR( CltSignal,  &(BaseControls->CtrlCmdMngr.TrqClamp) );	/* /CLT */
	LPX_SETSOUTPTR( VltSignal,  &(BaseControls->CtrlCmdMngr.SpdClamp) );	/* /VLT */
	LPX_SETSOUTPTR( BkSignal,   &(SeqCtrlOut->BrkSigSts) );					/* /BK */
	LPX_SETSOUTPTR( WarnSignal, &(AlmManager->WrnFlagX) );					/* /WARN <S0FF> */
/*--------------------------------------------------------------------------------------------------*/
	LPX_SETSOUTPTR( NearSignal, &(BaseControls->PosCtrlSts.NearSignal) );	/* /NEAR */
/*--------------------------------------------------------------------------------------------------*/
	LPX_SETSOUTPTR( PreWrnSignal,  &(PowerChecker->var.PreSigOut) );		/* /PreWrn */ /* <S154> */
	LPX_SETSOUTPTR( ClinkSignal,  &(PowerChecker->var.ClinkOut) );		/* /CLINK *//* <S21D> */
	LPX_SETSOUTPTR( CmconSignal,  &(PowerChecker->var.CmconOut) );		/* /CMCON *//* <S21D> */
//<S081>  #endif /* CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE */

#if CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE /* <S04B> */
/*--------------------------------------------------------------------------------------------------*/
/* Set Signal Maps (A/P Only)																		*/
/*--------------------------------------------------------------------------------------------------*/
	LPX_SETSOUTPTR( Alo1Signal,  &(AlmManager->AlmCodeOut.Alo1) );			/* ALO1 */
	LPX_SETSOUTPTR( Alo2Signal,  &(AlmManager->AlmCodeOut.Alo2) );			/* ALO2 */
	LPX_SETSOUTPTR( Alo3Signal,  &(AlmManager->AlmCodeOut.Alo3) );			/* ALO3 */
#endif /* CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE */

}

/****************************************************************************************************/
/*																									*/
/*		<S1B5> �V�[�P���X�o�̓}�b�v�v�Z(����) 														*/
/*																									*/
/****************************************************************************************************/
void	LpxSetOutputMapW( AXIS_HANDLE *Axis )
{
	PRMDATA				*Prm;
	SEQ_IO_HNDL	 		*IoSignals;

	Prm	= Axis->UniPrms->Prm;
	IoSignals = Axis->IoSignals;
/*--------------------------------------------------------------------------------------------------*/
/* Set Signal Map 																					*/
/*--------------------------------------------------------------------------------------------------*/
	LpxPcalSetSoutMap3( &(IoSignals->CoinSignal), Prm->coin_selsw, 0 );
	LpxPcalSetSoutMap3( &(IoSignals->VcmpSignal), Prm->vcmp_selsw, 0 );
	LpxPcalSetSoutMap3( &(IoSignals->TgonSignal), Prm->tgon_selsw, 0 );
	LpxPcalSetSoutMap3( &(IoSignals->SrdySignal), Prm->srdy_selsw, 0 );
	LpxPcalSetSoutMap3( &(IoSignals->CltSignal), Prm->clt_selsw, 0 );
	LpxPcalSetSoutMap3( &(IoSignals->VltSignal), Prm->vlt_selsw, 0 );
	LpxPcalSetSoutMap3( &(IoSignals->BkSignal), Prm->bk_selsw, 0 );
	LpxPcalSetSoutMap3( &(IoSignals->WarnSignal), Prm->warn_selsw, 0 );
	LpxPcalSetSoutMap3( &(IoSignals->NearSignal), Prm->near_selsw, 0 );
	LpxPcalSetSoutMap3( &(IoSignals->CpulseSignal), Prm->cpulse_selsw, 0 );
	LpxPcalSetSoutMap3( &(IoSignals->ClinkSignal), Prm->clink_selsw, 0 );
	LpxPcalSetSoutMap3( &(IoSignals->CmconSignal), Prm->cmcon_selsw, 0 );
	//	LpxPcalSetSoutMap3( &(IoSignals->PmSignal), Prm->pm_selsw, 0 );			/* <S21D> */
	LpxPcalSetSoutMap3( &(IoSignals->PreWrnSignal), Prm->pm_selsw, 0 );			/* <S21D> */
	LpxPcalSetSoutMap3( &(IoSignals->DaoSignal[0]), Prm->dao_selsw[0], 0 );
	LpxPcalSetSoutMap3( &(IoSignals->DaoSignal[1]), Prm->dao_selsw[1], 0 );
/*--------------------------------------------------------------------------------------------------*/
	return; 
}

/****************************************************************************************************/
/*																									*/
/*		<S1B5> �V�[�P���X�o�̓}�b�v�v�Z(����) 														*/
/*																									*/
/****************************************************************************************************/
void	LpxSetOutputMapCmn( AXIS_HANDLE *Axis )
{
	PRMDATA				*Prm;
	SEQ_IO_HNDL	 		*IoSignals;
	ULONG				OppositeMode; /* <S04B> */

	Prm	= Axis->UniPrms->Prm;
	IoSignals = Axis->IoSignals;
	OppositeMode = Prm->sorev1 + (Prm->sorev2 << 16); /* <S04B> */

/*--------------------------------------------------------------------------------------------------*/
/* Set Signal Maps (M2/M3/CMD Only) 																*/
/*--------------------------------------------------------------------------------------------------*/
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W�i2���Ή��j�̂�	 SGD7W IO���o�͑Ή�<S118>	*/
	LpxPcalSetSoutMap2( Axis, &(IoSignals->CoinSignal), (Prm->outsel1 >> 0),  OppositeMode );	/* /COIN */
	LpxPcalSetSoutMap2( Axis, &(IoSignals->VcmpSignal), (Prm->outsel1 >> 4),  OppositeMode );	/* /VCMP */
	LpxPcalSetSoutMap2( Axis, &(IoSignals->TgonSignal), (Prm->outsel1 >> 8),  OppositeMode );	/* /TGON */
	LpxPcalSetSoutMap2( Axis, &(IoSignals->SrdySignal), (Prm->outsel1 >> 12), OppositeMode );	/* /S-RDY */
/*--------------------------------------------------------------------------------------------------*/
	LpxPcalSetSoutMap2( Axis, &(IoSignals->CltSignal),  (Prm->outsel2 >> 0),  OppositeMode );	/* /CLT */
	LpxPcalSetSoutMap2( Axis, &(IoSignals->VltSignal),  (Prm->outsel2 >> 4),  OppositeMode );	/* /VLT */
	LpxPcalSetSoutMap2( Axis, &(IoSignals->BkSignal),   (Prm->outsel2 >> 8),  OppositeMode );	/* /BK */
	LpxPcalSetSoutMap2( Axis, &(IoSignals->WarnSignal), (Prm->outsel2 >> 12), OppositeMode );	/* /WARN */
/*--------------------------------------------------------------------------------------------------*/
	LpxPcalSetSoutMap2( Axis, &(IoSignals->NearSignal), (Prm->outsel3 >> 0), OppositeMode );	/* /NEAR */

	LpxPcalSetSoutMap2( Axis, &(IoSignals->PreWrnSignal), (Prm->outsel4 >> 8), OppositeMode );	/* /PreWrn */ /*<S154>*/
	LpxPcalSetSoutMap2( Axis, &(IoSignals->ClinkSignal), (Prm->outsel4 >> 0), OppositeMode );	/* /CLINK *//* <S21D> */
	LpxPcalSetSoutMap2( Axis, &(IoSignals->CmconSignal), (Prm->outsel4 >> 4), OppositeMode );	/* /CMCON *//* <S21D> */
#elif CSW_BOARD_TYPE == SGD7S_TYPE	/*	SGD7S�i1���Ή��j�̂�	*/
	LpxPcalSetSoutMap( &(IoSignals->CoinSignal), (Prm->outsel1 >> 0),  OppositeMode );	/* /COIN */
	LpxPcalSetSoutMap( &(IoSignals->VcmpSignal), (Prm->outsel1 >> 4),  OppositeMode );	/* /VCMP */
	LpxPcalSetSoutMap( &(IoSignals->TgonSignal), (Prm->outsel1 >> 8),  OppositeMode );	/* /TGON */
	LpxPcalSetSoutMap( &(IoSignals->SrdySignal), (Prm->outsel1 >> 12), OppositeMode );	/* /S-RDY */
/*--------------------------------------------------------------------------------------------------*/
	LpxPcalSetSoutMap( &(IoSignals->CltSignal),  (Prm->outsel2 >> 0),  OppositeMode );	/* /CLT */
	LpxPcalSetSoutMap( &(IoSignals->VltSignal),  (Prm->outsel2 >> 4),  OppositeMode );	/* /VLT */
	LpxPcalSetSoutMap( &(IoSignals->BkSignal),   (Prm->outsel2 >> 8),  OppositeMode );	/* /BK */
	LpxPcalSetSoutMap( &(IoSignals->WarnSignal), (Prm->outsel2 >> 12), OppositeMode );	/* /WARN */
/*--------------------------------------------------------------------------------------------------*/
	LpxPcalSetSoutMap( &(IoSignals->NearSignal), (Prm->outsel3 >> 0), OppositeMode );	/* /NEAR */

	LpxPcalSetSoutMap( &(IoSignals->PreWrnSignal), (Prm->outsel4 >> 8), OppositeMode );	/* /PreWrn */ /*<S154>*/
	LpxPcalSetSoutMap( &(IoSignals->ClinkSignal), (Prm->outsel4 >> 0), OppositeMode );	/* /CLINK *//* <S21D> */
	LpxPcalSetSoutMap( &(IoSignals->CmconSignal), (Prm->outsel4 >> 4), OppositeMode );	/* /CMCON *//* <S21D> */
#endif
/*--------------------------------------------------------------------------------------------------*/

#if CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE /* <S04B> */
/*--------------------------------------------------------------------------------------------------*/
/* Set Signal Maps (A/P Only)																		*/
/*--------------------------------------------------------------------------------------------------*/
	LpxPcalSetSoutMap( &(IoSignals->Alo1Signal), (Prm->outsel6 >> 0), OppositeMode );	/* ALO1 *//* <S04B> */
	LpxPcalSetSoutMap( &(IoSignals->Alo2Signal), (Prm->outsel6 >> 4), OppositeMode );	/* ALO2 *//* <S04B> */
	LpxPcalSetSoutMap( &(IoSignals->Alo3Signal), (Prm->outsel6 >> 8), OppositeMode );	/* ALO3 *//* <S04B> */
#endif /* CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE */
				
	return;
}

/****************************************************************************************************/
/*																									*/
/*		�V�[�P���X�o�̓}�b�v�v�Z <S1B5>																*/
/*																									*/
/****************************************************************************************************/
void	LpxPcalSetSoutMap3( SEQ_IOSIGNAL *IoSignal, ULONG prm, ULONG LogicH )
{
LONG	Pin;
ULONG	LogicX;
//ULONG	LogicL = (LogicH) ? 0x00 : 0x01;
#define OUTPUT_MAP3_LGC_NONACTIVE 	(0)
#define OUTPUT_MAP3_LGC_NORMAL 		(1)
#define OUTPUT_MAP3_LGC_OPPOSITE 	(2)
/*--------------------------------------------------------------------------------------------------*/
	Pin = HALdrv_GetSpBitNumber( Lpx_GetBcd( prm ) );				/* Get Pin <S1BF> */
	LogicX = ( prm >> 12 ) & 0x0F;									/* Get Logic */
	LogicX = (( Pin < 0 ) ? OUTPUT_MAP3_LGC_NONACTIVE : LogicX);	/* Disable Pin */
/*--------------------------------------------------------------------------------------------------*/
/*	Set Output Signal Map																			*/
/*--------------------------------------------------------------------------------------------------*/
	switch ( LogicX )
	{
	/*----------------------------------------------------------------------------------------------*/
	case OUTPUT_MAP3_LGC_NONACTIVE:									/* 0: Non Active				*/
	/*----------------------------------------------------------------------------------------------*/
			IoSignal->PinNumber = 31;								/* �b�� */
			IoSignal->Mask		= 0;
			IoSignal->XorMask	= 0;
			break;
	/*----------------------------------------------------------------------------------------------*/
	case OUTPUT_MAP3_LGC_NORMAL:									/* 1: Normal					*/
	/*----------------------------------------------------------------------------------------------*/
			IoSignal->PinNumber = (ULONG)Pin;
			IoSignal->Mask		= 1;
			IoSignal->XorMask	= 0/*LogicL*/;
			break;
	/*----------------------------------------------------------------------------------------------*/
	case OUTPUT_MAP3_LGC_OPPOSITE:									/* 2: Opposite					*/
	/*----------------------------------------------------------------------------------------------*/
			IoSignal->PinNumber = (ULONG)Pin;
			IoSignal->Mask		= 1;
			IoSignal->XorMask	= 1/*LogicH*/;
			break;
	/*----------------------------------------------------------------------------------------------*/
	default:
	/*----------------------------------------------------------------------------------------------*/
			IoSignal->PinNumber = 31;								/* �b�� */
			IoSignal->Mask		= 0;
			IoSignal->XorMask	= 0;
			break;
	}
/*--------------------------------------------------------------------------------------------------*/
	return;
}

/****************************************************************************************************/
/*																									*/
/*		�V�[�P���X�o�̓}�b�v�v�Z <S045>																*/
/*																									*/
/****************************************************************************************************/
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W�i2���Ή��j�̂�	SGD7W IO���o�͑Ή�<S118>	*/
void	LpxPcalSetSoutMap2( AXIS_HANDLE *Axis, SEQ_IOSIGNAL *IoSignal, ULONG prm, ULONG OppositeReq )
#elif CSW_BOARD_TYPE == SGD7S_TYPE	/*	SGD7S�i1���Ή��j�̂�	*/
void	LpxPcalSetSoutMap( SEQ_IOSIGNAL *IoSignal, ULONG prm, ULONG OppositeReq )
#endif
{
/*--------------------------------------------------------------------------------------------------*/
	IoSignal->Mask		= 1;
//	IoSignal->XorMask   = 0; /* <S04B> */
/*--------------------------------------------------------------------------------------------------*/
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W�i2���Ή��j�̂�	SGD7W IO���o�͑Ή�<S118>	*/
		switch( prm & 0x0F )
		{
		/*--------------------------------------------------------------------------------------------------*/
			case 1:													/* Output to SO1 */
		/*--------------------------------------------------------------------------------------------------*/
				if(Axis->AxisNo == 0 )	/*	1����	*/
				{	/*	1����	*/
					IoSignal->PinNumber = KPI_SO1BITNO;		/*	CN1-23,24	*/
					//IoSignal->XorMask = ((OppositeReq >> 4) & 0x01) ? 1 : 0;	/* <S1DA> */
					IoSignal->XorMask = ((OppositeReq >> 0) & 0x01) ? 1 : 0;	/* <S1DA> */
				}else
				{	/*	2����	*/
					IoSignal->PinNumber = KPI_SO2BITNO;		/*	CN1-25,26	*/
					//IoSignal->XorMask = ((OppositeReq >> 8) & 0x01) ? 1 : 0;	/* <S1DA> */
					IoSignal->XorMask = ((OppositeReq >> 0) & 0x01) ? 1 : 0;	/* <S1DA> */
				}
				break;
			case 2:													/* Output to SO2 */
		/*--------------------------------------------------------------------------------------------------*/
				if(Axis->AxisNo == 0 )	/*	1����	*/
				{	/*	1����	*/
					IoSignal->PinNumber = KPI_SO3BITNO;		/*	CN1-27,28	*/
					//IoSignal->XorMask = ((OppositeReq >> 12) & 0x01) ? 1 : 0;	/* <S1DA> */
					IoSignal->XorMask = ((OppositeReq >> 4) & 0x01) ? 1 : 0;	/* <S1DA> */
				}else
				{	/*	2����	*/
					IoSignal->PinNumber = KPI_SO4BITNO;		/*	CN1-29,30	*/
					//IoSignal->XorMask = ((OppositeReq >> 16) & 0x01) ? 1 : 0;	/* <S1DA> */
					IoSignal->XorMask = ((OppositeReq >> 4) & 0x01) ? 1 : 0;	/* <S1DA> */
				}
				break;
		/*--------------------------------------------------------------------------------------------------*/
			default:												/* Always inactive */
		/*--------------------------------------------------------------------------------------------------*/
				IoSignal->Mask		= 0;
				IoSignal->XorMask   = 0;
				IoSignal->PinNumber = 31;			/* Selects a reserved pin */
				break;
		}
#elif CSW_BOARD_TYPE == SGD7S_TYPE	/*	SGD7S�i1���Ή��j�̂�	*/
	switch( prm & 0x0F )
	{
/*--------------------------------------------------------------------------------------------------*/
	case 1:													/* Output to SO1 */
/*--------------------------------------------------------------------------------------------------*/
		IoSignal->PinNumber = KPI_SO1BITNO;
		IoSignal->XorMask = ((OppositeReq >> 0) & 0x01) ? 1 : 0; /* <S04B> */
		break;
	case 2:													/* Output to SO2 */
/*--------------------------------------------------------------------------------------------------*/
		IoSignal->PinNumber = KPI_SO2BITNO;
		IoSignal->XorMask = ((OppositeReq >> 4) & 0x01) ? 1 : 0; /* <S04B> */
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 3:													/* Output to SO3 */
/*--------------------------------------------------------------------------------------------------*/
		IoSignal->PinNumber = KPI_SO3BITNO;
		IoSignal->XorMask = ((OppositeReq >> 8) & 0x01) ? 1 : 0; /* <S04B> */
		break;

#if CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE /* <S04B> */
/* <S1AA> : SO4�`SO6�̏o�͐M�����]��p�~ */
/*--------------------------------------------------------------------------------------------------*/
	case 4:													/* Output to SO4 *//* <S04B> */
/*--------------------------------------------------------------------------------------------------*/
		IoSignal->PinNumber = KPI_SO4BITNO;
		//IoSignal->XorMask = ((OppositeReq >> 12) & 0x01) ? 0 : 1; /* <S04B> */	/* <S1AA> */
		IoSignal->XorMask = ((OppositeReq >> 12) & 0x01) ? 1 : 0;					/* <S1AA> */

		break;
/*--------------------------------------------------------------------------------------------------*/
	case 5:													/* Output to SO5 *//* <S04B> */
/*--------------------------------------------------------------------------------------------------*/
		IoSignal->PinNumber = KPI_SO5BITNO;
		//IoSignal->XorMask = ((OppositeReq >> 16) & 0x01) ? 0 : 1; /* <S04B> */	/* <S1AA> */
		IoSignal->XorMask = ((OppositeReq >> 16) & 0x01) ? 1 : 0;					/* <S1AA> */

		break;
/*--------------------------------------------------------------------------------------------------*/
	case 6:													/* Output to SO6 *//* <S04B> */
/*--------------------------------------------------------------------------------------------------*/
		IoSignal->PinNumber = KPI_SO6BITNO;
		//IoSignal->XorMask = ((OppositeReq >> 20) & 0x01) ? 0 : 1; /* <S04B> */	/* <S1AA> */
		IoSignal->XorMask = ((OppositeReq >> 20) & 0x01) ? 1 : 0;					/* <S1AA> */
		break;
#endif /* CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE */

/*--------------------------------------------------------------------------------------------------*/
	default:												/* Always inactive */
/*--------------------------------------------------------------------------------------------------*/
		IoSignal->Mask		= 0;
		IoSignal->XorMask   = 0;
		IoSignal->PinNumber = 31;			/* Selects a reserved pin */
		break;
	}
#endif
/*--------------------------------------------------------------------------------------------------*/
}

/****************************************************************************************************/
/*																									*/
/*		���[�^���x���o�p�ړ����Ϗ���������	<V259>													*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �p�����[�^�ݒ�(Pn009.2)����ѐڑ����ꂽ�G���R�[�_����\�ɂ���Ĉړ����ω񐔂̏����l��	*/
/*			�ݒ肷��B�������A�������X���L���ȏꍇ�́A�ړ����ω񐔂�0��ɕύX���Ă����B				*/
/*			�ړ����ω񐔂̃I�����C���ł̕ύX�́ABpiChangeSpdDetMaFil( )�ɂčs�����ƁB				*/
/*																									*/
/*			<��]�`>							�ړ����ω�										*/
/*			�G���R�[�_����\	���x����\		�ʏ�(Pn009.2=0)	(Pn009.2=1)	���Ɍ��o	�������X	*/
/*			  13bit�ȉ�			117.12min-1�ȏ�		4��				16��	  16��		  0��		*/
/*			  16bit				 14.64min-1			2��				16��	  16��		  0��		*/
/*			  17bit�ȏ�			  7.32min-1�ȉ�		0��				16��	  16��		  0��		*/
/*																									*/
/*			<���j�A>							�ړ����ω�										*/
/*			�X�P�[���s�b�`		���x����\		�ʏ�(Pn009.2=0)	(Pn009.2=1)	���Ɍ��o	�������X	*/
/*			  200um�ȏ�			 12.50mm/s�ȏ�		4��				16��	  16��		  0��		*/
/*			  100um				  6.25mm/s			2��				16��	  16��		  0��		*/
/*			  100um����			  6.25mm/s����		0��				16��	  16��		  0��		*/
/*																									*/
/*			�����x����\(1�p���X����̑��x)�̒l�́A�T���v�����O����62.5us�̏ꍇ						*/
/*			�����j�A���[�^�̏ꍇ�́A256�Ă��{�Ōv�Z													*/
/*																									*/
/****************************************************************************************************/
#define	SPDDET13BIT		KPI_SACOUNTMS*600000/8192			/* 13bit�G���R�[�_���x���o����\		*/
#define	SPDDET16BIT		KPI_SACOUNTMS*600000/65536			/* 16bit�G���R�[�_���x���o����\		*/
#define	SPDDET100UM		KPI_SACOUNTMS*10000/256*100/1000	/* �X�P�[���s�b�`100um�̑��x���o����\	*/
#define	SPDDET200UM		KPI_SACOUNTMS*10000/256*200/1000	/* �X�P�[���s�b�`200um�̑��x���o����\	*/
/*--------------------------------------------------------------------------------------------------*/
static void	LpxPcalSpdDetMaFilter( AXIS_HANDLE *Axis )
{
	BPRMDAT			*Bprm;

	Bprm	= Axis->UniPrms->Bprm;
	MlibResetLongMemory( Axis->MotSpdMafil, sizeof( *Axis->MotSpdMafil )/4 );
/*--------------------------------------------------------------------------------------------------*/
/*		�ړ����ω񐔂̏����l��ݒ�																	*/
/*--------------------------------------------------------------------------------------------------*/
	if ( Bprm->SpdDetMaFil == TRUE )
	{ /* Pn009.2 = 1�̏ꍇ16�� */
		Axis->MotSpdMafil->defaultNumBit = 4;
	}
	else if( Axis->MencV->AxisMotType == MOTTYPE_LINEAR )
	{ /* ���j�A�̏ꍇ */
		if( Bprm->SpdDetectUnit >= SPDDET200UM )
		{ /* 200um�ȏ��4�� */
			Axis->MotSpdMafil->defaultNumBit = 2;
		}
		else if( Bprm->SpdDetectUnit >= SPDDET100UM )
		{ /* 100um�`200um��2�� */
			Axis->MotSpdMafil->defaultNumBit = 1;
		}
		else
		{ /* 100um������0�� */
			Axis->MotSpdMafil->defaultNumBit = 0;
		}
	}
	else
	{ /* ��]�^�̏ꍇ */
		if( Bprm->SpdDetectUnit >= SPDDET13BIT )
		{ /* 13bit�ȉ���4�� */
			Axis->MotSpdMafil->defaultNumBit = 2;
		}
		else if( Bprm->SpdDetectUnit >= SPDDET16BIT )
		{ /* 14�`16bit��2�� */
			Axis->MotSpdMafil->defaultNumBit = 1;
		}
		else
		{ /* 17bit�ȏ��0�� */
			Axis->MotSpdMafil->defaultNumBit = 0;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�ړ����ω񐔂�ݒ�																			*/
/*--------------------------------------------------------------------------------------------------*/
	Axis->MotSpdMafil->manumBit = Axis->MotSpdMafil->defaultNumBit;
	if ( Axis->BaseControls->TuneLessCtrl.conf.TuningLessUse == TRUE )
	{ /* �������X����0�Œ� */
		Axis->MotSpdMafil->manumBit = 0;
	}
	Axis->MotSpdMafil->idx = 0;
	Axis->MotSpdMafil->ksub = 0;
	Axis->MotSpdMafil->mabufSumx = 0;
/*--------------------------------------------------------------------------------------------------*/

}



/****************************************************************************************************/
#define SVCDEF_PCMP_DLYUS			( 80 )		/* �ʑ��x��⏞�x�ꎞ��(�G���R�[�_:50us+ScanA:30us)	*/
#define SVCDEF_PCMP_DLYCX			( 16 )				/* �ʑ��x��⏞�␳�W�� [-]					*/
/****************************************************************************************************/
/*																									*/
/*		�ʑ��⏞���Z�W���̌v�Z																		*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �x�ꎞ��(�G���R�[�_�Ȃ�)����A�ʑ��x��⏞���Z�W�����Z�o����B							*/
/*			�p�����[�^�ɂ��ʑ��⏞����A�ʑ����x�⏞���Z�W�����Z�o����B							*/
/*																									*/
/*	�� �� : �ʑ��⏞�v�Z���T�[�{�p�b�N�p�����[�^���O�̏ꍇ�A���[�^�p�����[�^���g���B				*/
/*			(������A���[�^���p�����[�^���e�l�߂̏ꍇ�̓T�[�{�p�b�N�p�����[�^���g��)				*/
/*			�T�[�{�p�b�N�p�����[�^���O�ȊO�̏ꍇ�A�T�[�{�p�b�N�p�����[�^���g���B					*/
/*																									*/
/*	�� �� : �T�[�{�p�b�N�p�����[�^�͂O�ȊO�łȂ��Ǝg�������ł��Ȃ��B								*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	void	LpxPcalPcmpCalGain( AXIS_HANDLE	*Axis )
{
#if (FLOAT_USE==TRUE)
	float			fx;
#else
	LONG			kx;
	LONG			sx;
#endif /* FLOAT_USE */
	LONG			PhsCmpDegx;
	BPRMDAT			*Bprm;
	PRMDATA			*Prm;

	Bprm	= Axis->UniPrms->Bprm;
	Prm		= Axis->UniPrms->Prm;

/*--------------------------------------------------------------------------------------------------*/
/*		�ʑ��⏞�p�����[�^�v�Z�F�x��⏞															*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*				         80 * nos * Pole * 65536       PhsCmp * 65536 * (PerOvrSpd+10000)			*/
/*		SGDS  Leang = -----------------------------	+ ------------------------------------			*/
/*		Rotary	       10^6 * 60 * 2 * 15000 * Cx             360 * 10000 * 15000	       			*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*					      Dx * OvrSpd * Pole * 65536		  Dx * OvrSpd * Pole					*/
/*		SVOS  Kpcmpd = --------------------------------	= ---------------------------				*/
/*		Rotary		    10^6 * 2 * PAI * 2 * 2^24 * Cx		10^6 * 3216.990877 * Cx					*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		PerOvrSpd : �n�r���o���x�� [0.01%]															*/
/*		Pole      : ���[�^�|�[����																	*/
/*		nos       : �n�r���x [r/min],[mm/s]															*/
/*		OvrSpd    : �n�r���x [rad/s],[m/s]															*/
/*		Dx        : �x�ꎞ�� [us] --> �J�[�l���\����`(*.cfg)�Ŏw��									*/
/*		Cx        : �o�O�␳ [--] --> �J�[�l���\����`(*.cfg)�Ŏw��									*/
/*		����(Cx=16) : ���[�^���{�����ɍ��킹�Ă���ꍇ�́A�o�O�C���s�A���j�A�̓o�O�Ȃ�			*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
	if( Axis->MencV->AxisMotType == MOTTYPE_ROTARY )
	{
		if( ((Prm->dq_sw & 0x0800) == 0 ) || ((Prm->MencP.flg_wf & 0x0001 ) == 0) )
		{
#if (FLOAT_USE==TRUE)
			fx = Bprm->OvrSpd * (LONG)SVCDEF_PCMP_DLYUS / 321699088.0f;
			Axis->BaseControls->PcmpCalcData.Kpcmpd = fx * (float)Prm->MencP.oslv_pol.b.h / (10.0f * (LONG)SVCDEF_PCMP_DLYCX);
#else
			kx = MlibScalKskxkx( Bprm->OvrSpd, (LONG)SVCDEF_PCMP_DLYUS, 321699088, &sx, 0 );
			Axis->BaseControls->PcmpCalcData.Kpcmpd =
					MlibPcalKxgain( kx, (LONG)Prm->MencP.oslv_pol.b.h, (10 * (LONG)SVCDEF_PCMP_DLYCX), &sx, 24 );
#endif /* FLOAT_USE */

		}
		else
		{
#if (FLOAT_USE==TRUE)
			fx = Bprm->OvrSpd * (float)SVCDEF_PCMP_DLYUS / 321699088.0f;
			Axis->BaseControls->PcmpCalcData.Kpcmpd = fx * (float)Prm->MencP.oslv_pol.b.h / 10.0f;
#else
			kx = MlibScalKskxkx( Bprm->OvrSpd, (LONG)SVCDEF_PCMP_DLYUS, 321699088, &sx, 0 );
			Axis->BaseControls->PcmpCalcData.Kpcmpd =
											MlibPcalKxgain( kx, (LONG)Prm->MencP.oslv_pol.b.h, 10, &sx, 24 );
#endif /* FLOAT_USE */
		}
	}
	else if( Axis->MencV->AxisMotType == MOTTYPE_LINEAR )
	{
#if (FLOAT_USE==TRUE)
		fx = Bprm->OvrSpd * (LONG)SVCDEF_PCMP_DLYUS / 51200.0f;
		Axis->BaseControls->PcmpCalcData.Kpcmpd = fx / Prm->MencP.ppitch;
		Axis->BaseControls->PcmpCalcData.Kpcmpd2 = Axis->BaseControls->PcmpCalcData.Kpcmpd;
#else
		kx = MlibScalKskxkx( Bprm->OvrSpd, (LONG)SVCDEF_PCMP_DLYUS, 51200, &sx, 0 );
		Axis->BaseControls->PcmpCalcData.Kpcmpd = MlibPcalKxgain( kx, 1, Prm->MencP.ppitch, &sx, 24 );
		Axis->BaseControls->PcmpCalcData.Kpcmpd2 = Axis->BaseControls->PcmpCalcData.Kpcmpd;
#endif /* FLOAT_USE */
	}

	/* �ʑ���ԗʉ��Z�W���v�Z */
#if (FLOAT_USE==TRUE) /* SVN_IRIS_MODE || INGRAM_MODE */
		fx = Bprm->OvrSpd * KPI_MACYCLENS / 160849544.0f;
		Axis->BaseControls->PcmpCalcData.KpcmpdItplt = fx * Bprm->Keangle / 10000.0f;
#else
	kx = MlibScalKskxkx( Bprm->OvrSpd, KPI_MACYCLENS, 160849544, &sx, 0 );
	Axis->BaseControls->PcmpCalcData.KpcmpdItplt =
							MlibPcalKxkskx( kx, Bprm->Keangle, 10000, &sx, 24 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		�ʑ��⏞�p�����[�^�v�Z�F�ʑ��⏞�P															*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		PcmpSpd1 = NorMaxSpd * PhsCmpSpd1 / MotMaxSpd		�ʑ��⏞�J�n���x						*/
/*																									*/
/*						  PhsCmpDeg1 * 65536														*/
/*		Kpcmpsp1 = --------------------------------			�ʑ��⏞���Z�Q�C��						*/
/*					 360 * (NorMaxSpd - PcmpSpd1)													*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		PhsCmpDeg1 : �ʑ��⏞�l�P			[deg/MaxSpd]											*/
/*		PhsCmpSpd1 : �ʑ��⏞�J�n���x�P		[100r/min]												*/
/*		MotMaxSpd  : ���[�^�ő呬�x			[100r/min]												*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
	if( Prm->phscmp != 0 )							/* �T�[�{�p�b�N�p�����[�^ != 0				*/
	{
		PhsCmpDegx = (LONG)Prm->phscmp;				/* �T�[�{�p�b�N�p�����[�^�g�p				*/
	}
	else if( Prm->MencP.phscmpM1.b.l == 0xFF )			/* ���[�^�p�����[�^���A�e�l�߂̏ꍇ			*/
	{
		PhsCmpDegx = (LONG)Prm->phscmp;				/* �T�[�{�p�b�N�p�����[�^�g�p				*/
	}
	else											/* ���[�^�p�����[�^���A�e�l�߂łȂ��ꍇ		*/
	{
		PhsCmpDegx = (LONG)Prm->MencP.phscmpM1.b.l;		/* ���[�^�p�����[�^�g�p						*/
	}
	/*----------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	fx = (KSGAIN)Bprm->NorMaxSpd * (KSGAIN)Prm->MencP.phscmpM1.b.h / (KSGAIN)Bprm->MaxVel;
	Axis->BaseControls->PcmpCalcData.PcmpSpd1 = fx;
#else
	kx = MlibScalKxgain( Bprm->NorMaxSpd, Prm->MencP.phscmpM1.b.h, Bprm->MaxVel, NULL, -30 );
	Axis->BaseControls->PcmpCalcData.PcmpSpd1 = kx;
#endif /* FLOAT_USE */

	/*----------------------------------------------------------------------------------------------*/
	if( Prm->MencP.phscmpM1.b.h >= Bprm->MaxVel )		/* �J�n���x�`�F�b�N						*/
	{
		Axis->BaseControls->PcmpCalcData.Kpcmpsp1 = 0;
		/* A.040 : �p�����[�^�ݒ�ُ� */
		ALMSetPramError( Axis->AlmManager, pndef_phscmpM1.RegNumber );
	}
	else
	{
#if (FLOAT_USE==TRUE)
		fx = (KSGAIN)(Bprm->NorMaxSpd - Axis->BaseControls->PcmpCalcData.PcmpSpd1) * 360.0f;
		Axis->BaseControls->PcmpCalcData.Kpcmpsp1 = (KSGAIN)PhsCmpDegx * 65536.0f / fx;
#else
		kx = MlibScalKxgain( PhsCmpDegx, 65536, 360, &sx,  0 );
		kx = MlibPcalKxgain( kx, 1, (Bprm->NorMaxSpd - Axis->BaseControls->PcmpCalcData.PcmpSpd1), &sx, 24 );
		Axis->BaseControls->PcmpCalcData.Kpcmpsp1 = kx;
#endif /* FLOAT_USE */
	}

#ifdef CSW_SCANA_ENABLE
	Axis->BaseLoops->PhaseComp.conf.Kpcmpd = Axis->BaseControls->PcmpCalcData.Kpcmpd;
	Axis->BaseLoops->PhaseComp.conf.Kpcmpd2 = Axis->BaseControls->PcmpCalcData.Kpcmpd2;
	Axis->BaseLoops->PhaseComp.conf.Kpcmpsp1 = Axis->BaseControls->PcmpCalcData.Kpcmpsp1;
	Axis->BaseLoops->PhaseComp.conf.PcmpSpd1 = Axis->BaseControls->PcmpCalcData.PcmpSpd1;
	Axis->BaseLoops->PhaseComp.conf.KpcmpdItplt = Axis->BaseControls->PcmpCalcData.KpcmpdItplt;
#endif /* CSW_SCANA_ENABLE */

/*--------------------------------------------------------------------------------------------------*/
/*	�ʑ��⏞�Q�͋@�\���g�p���C�j�V�����������ԒZ�k�̂��ߍ폜����B								*/
/*--------------------------------------------------------------------------------------------------*/

}


/****************************************************************************************************/
/*																									*/
/*		 ���[�^�É��p���x�e�aLPF�����ݒ� <S0DA>														*/
/*																									*/
/*		�T�v�F�G���R�[�_�p�����[�^�ɂ�葬�x�e�a�Ƀ��[�p�X�t�B���^��ǉ�����						*/
/*				���ۂ̏�����Pn308�F���xFB�t�B���^���萔�Ƀ��[�^�É��p�t�B���^���萔�����Z����		*/
/*																									*/
/****************************************************************************************************/
static void IprmcalSpdFbFil( AXIS_HANDLE *Axis )
{
	PRMDATA		*Prm;
	BPRMDAT		*Bprm;
	MENCV		*MencV;

	Prm   = Axis->UniPrms->Prm;
	Bprm  = Axis->UniPrms->Bprm;
	MencV = Axis->MencV;

	if( ( Prm->MencP.flg_wf & 0x0004 )								/* ���[�^�É��ݒ�I���t���O		*/
			|| (MencV->TuningLessLowGainMotor == TRUE))				/* ���[�^�@�픻��				*/
	{
		Bprm->SpdFbLpassFil = Prm->fbfil2;							/* ���xFB̨���ǉ�				*/
	}
	else
	{
		Bprm->SpdFbLpassFil = 0;									/* ���xFB̨��0.1[ms]�ǉ�		*/
	}

	return;

}

/*<S1D0> START*/
/*****************************************************************************
  Description: ���[�^�ō����x����
------------------------------------------------------------------------------
  Parameters:
	CfgPrm              cfg�p�����[�^��`
	Bprm				��{�p�����[�^��`
  Return:

------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void	AdetCheckMotorOverSpeed(PRMDATA *Prm, BPRMDAT *Bprm, LONG AxisMotType )
{
	if( AxisMotType == MOTTYPE_LINEAR )
	{/* ���j�A�^ */
		Bprm->OsAlmLvl = Bprm->NorOvrSpd;
	}
	else
	{/* ��]�^ */
		Bprm->OsAlmLvl = MlibDiv6432( Prm->maxvel, Bprm->NorOvrSpd, ( Bprm->MaxVel * 110 ) );
	}
}
/*<S1D0> END*/

/* <S219> Start */
/****************************************************************************************************/
/*																									*/
/*		�T�[�{�I���������t�@����~�A���[�����o���Ԃ̐ݒ�											*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �t�@����~���[�j���O������̎���(�A���[���ւ̈ڍs����)�̐ݒ���s���B					*/
/*																									*/
/*			1. �p�����[�^																			*/
/*				1) PnEE6 : �T�[�{�I���������t�@����~�A���[�����o���� [min]							*/
/*																									*/
/****************************************************************************************************/
static	void	IprmcalFanLockAlmDetectTime( AXIS_HANDLE *Axis )
{
	CMN_PRMDATA		*Cprm;
	POW_MNG_HNDL	*PowerManager;

	Cprm = Axis->UniPrms->Cprm;
	PowerManager = Axis->PowerManager;

	/* �T�[�{�I���������t�@����~�A���[�����o���Ԏ擾(�P��:[min]) */
	PowerManager->FanLock.conf.FanLockAlmDetectTime = (ULONG)(Cprm->SvonFanLock_Time * 60 * 10);
	return;
}
/* <S219> End */

/***************************************** end of file **********************************************/
