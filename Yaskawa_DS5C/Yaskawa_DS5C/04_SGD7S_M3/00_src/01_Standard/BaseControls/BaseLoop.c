/****************************************************************************************************/
/*																									*/
/*																									*/
/*		ScanAMain.c : ScanA���C��������`															*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : ScanA���C������																			*/
/*																									*/
/*		ScanB���擾�����ʒu�A���x�A�g���N�w�߂���ASIC(�ʃv���O����)�֓n���d���w�߂��Z�o����B		*/
/*		���Ɍ��o�������ɂ͎��Ɍ��o���C�����������s����B											*/
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
#include "BaseControls.h"
#include "SerialEncIf.h"
#include "PosManager.h"
#include "ResVibCtrl.h"
#include "GainChange.h"
#include "PowerManager.h"
#include "MotorPoleFind.h"
#include "MLib.h"
#include "HwDeviceIf.h"
#include "JL086ServoIpHw.h"
#include "AlarmManager.h"
#include "ExControls.h"
#include "ExCtrlPrmCalc.h"
#include "RippleComp.h"		/* <S1FD> */


static	void	LpxSvPositionManager( BASE_LOOPCTRLS *BaseLoops, POS_MNG_HNDL *PosManager );
static	void	LpxPosSpdFeedbackCal( BASE_LOOPCTRLS *BaseLoops );
static void	ModeSwitch( MODE_SW *ModeSwData, LONG trq_ref, LONG spd_ref, LONG pos_err );
extern void ApxMotorLessTestModeMechaModel( LONG TrqRef, BOOL BeComplete, MOTLESSTST *MotorLessTest );


#define NRFMAX	16014615
/****************************************************************************************************/
/*																									*/
/*		�ʒu���䉉�Z																				*//*<S00A>*/
/*																									*/
/****************************************************************************************************/
static KSGAIN	BasePositionControlA(
			BASE_LOOPCTRLS *BaseLoops,
			KSGAIN	PerClrCmd,			/*<S00A>*/
			KSGAIN	IcvClrCmd,			/*<S00A>*/
			KSGAIN	Kp,					/*<S00A>*/
			LONG	PerClrEdge /* @@ */ )
{
	KSGAIN		SpdRefx;		/* ���x�w��(�ꎞ�ۊǗp) *//*<S00A>*/
	POSCTRL		*PosCtrl;		/* �ʒu����p�ϐ� */
	POS_MNG_HNDL *PosMngV;

	PosCtrl = &BaseLoops->PosCtrl;
	PosMngV = &BaseLoops->BaseCtrls->PosManager;
/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu�΍��N���A����																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( PerClrCmd )
	{
		PosCtrl->V.PerIvar64[0] = 0;
		PosCtrl->V.PerIvar64[1] = 0;
		PosCtrl->V.LastPacOut   = 0;
		return( 0 );
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu����ϕ��N���A����																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( IcvClrCmd )
	{
		PosCtrl->V.PerIvar64[0] = 0;
		PosCtrl->V.PerIvar64[1] = 0;
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu����F�ʒu�΍��v�Z																<S14C>	*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	/*<V739>SafetyModule���މ��P*/
#if	0 //( CCSW_SFTID == SFTID_EXIST && CCSW_HWID == HWID_SGDV )									  /*<V717>Mod:Start*/
	if (!((BoutV.ZctrlMode.b.zm == CTRLMODE_DSTOP) && (CoutSom.St.PosErrLt == 1)))
	{ PosMngV.PosErr = FlibPerrcalx( dPosRef, dPosFbk, PosMngV.Per64 ); }						  /*<V730>Mod*/
#else
	PosMngV->var.PosErr = FlibPerrcalx( BaseLoops->dPosRefi, BaseLoops->dPosFbki, PosMngV->var.Per64 ); 
#endif	

#else

/*<V739>SafetyModule���މ��P*/
#if	0 //( CCSW_SFTID == SFTID_EXIST && CCSW_HWID == HWID_SGDV )									  /*<V717>Mod:Start*/
	if (!((BoutV.ZctrlMode.b.zm == CTRLMODE_DSTOP) && (CoutSom.St.PosErrLt == 1)))
	{ PosMngV.PosErr = MlibPerrcalx( dPosRef, dPosFbk, PosMngV.Per64 ); }						  /*<V730>Mod*/
#else
	PosMngV->var.PosErr = MlibPerrcalx( BaseLoops->dPosRefi, BaseLoops->dPosFbki, PosMngV->var.Per64 );
#endif																							  /*<V717>Mod:End  */
#endif /* FLOAT_USE */
/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu����F��ᐧ�䉉�Z																		*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	PosCtrl->V.PacOut = PosMngV->var.PosErr * Kp;
#else
	PosCtrl->V.PacOut = MlibMulgain( PosMngV->var.PosErr, Kp );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu����F�ϕ����䉉�Z																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( (BaseLoops->BaseCtrls)->BeComplete == FALSE )
	{ /* �x�[�X�u���b�N */
		PosCtrl->V.PerIvar64[0] = 0;					/* SGDS�ł́A�x�[�X�u���b�N���A				*/
		PosCtrl->V.PerIvar64[1] = 0;					/* �ʒu�ϕ������N���A���Ă���				*/
		PosCtrl->V.IacOut       = 0;
	}
	else if( (BaseLoops->BaseCtrls)->CtrlCmdPrm.Kpi == 0 )
	{ /* �ϕ��Q�C���[����(�������ԑ΍�) */
#if (FLOAT_USE==TRUE)
		PosCtrl->V.IacOut = ((PosCtrl->V.PerIvar64[1] + 1) / 2.0f);
#else
		PosCtrl->V.IacOut = ((PosCtrl->V.PerIvar64[1] + 1) >> 1);
#endif /* FLOAT_USE */
	}
	else
	{ /* �x�[�X�C�l�[�u����(�ϕ��Q�C���[���ȊO) */
#if (FLOAT_USE==TRUE)
		PosCtrl->V.IacOut = FlibIntegral( PosCtrl->V.PacOut,
										  (BaseLoops->BaseCtrls)->CtrlCmdPrm.Kpi,
										  PosCtrl->V.PerIvar64 ); 
#else
		PosCtrl->V.IacOut = MlibIntegral( PosCtrl->V.PacOut,
										  (BaseLoops->BaseCtrls)->CtrlCmdPrm.Kpi,
										  PosCtrl->V.PerIvar64 );
#endif /* FLOAT_USE */
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu����F�������䉉�Z																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( PosCtrl->P.Kpd == 0 )							/* �����Q�C���[����(�������ԑ΍�)			*/
	{
		PosCtrl->V.DacOut = 0;							/* ��������o�� = �[��						*/
	}
	else
	{
#if (FLOAT_USE==TRUE)
		PosCtrl->V.DacOut = (PosCtrl->V.PacOut - PosCtrl->V.LastPacOut) * PosCtrl->P.Kpd;
#else
		PosCtrl->V.DacOut = MlibMulgain( PosCtrl->V.PacOut - PosCtrl->V.LastPacOut, PosCtrl->P.Kpd );
#endif /* FLOAT_USE */
	}
	PosCtrl->V.LastPacOut = PosCtrl->V.PacOut;
/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu����F�o�h�c����o��																	*/
/*--------------------------------------------------------------------------------------------------*/
	SpdRefx = PosCtrl->V.PacOut + PosCtrl->V.IacOut + PosCtrl->V.DacOut;

/*--------------------------------------------------------------------------------------------------*/
/*		���x�o�C�A�X���Z���Z																		*/
/*--------------------------------------------------------------------------------------------------*/
	/* ��SGDV���Ή��� */
//	SpdRefx = VelocityBias( SpdRefx, PosCtrl->V.PosErr );	/* ���x�o�C�A�X���Z���Z					*/
	if( SpdRefx == 0 )
	{
		if( PosMngV->var.PosErr > 0 )		{ SpdRefx =  1;}
		else if( PosMngV->var.PosErr < 0 )	{ SpdRefx = -1;}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu����o�̓��~�b�g																		*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	PosCtrl->V.SpdRef = FlibLimitul( SpdRefx, NRFMAX, -NRFMAX );
#else
	PosCtrl->V.SpdRef = MlibLimitul( SpdRefx, NRFMAX, -NRFMAX );
#endif /* FLOAT_USE */

	return( PosCtrl->V.SpdRef );

}

/****************************************************************************************************/
/*																									*/
/*		�T�[�{�I�����ʒu�΍�����̑��x���~�b�g����													*/
/*																									*/
/****************************************************************************************************/
static KSGAIN	SvonSpeedLimit( BOOL BaseEnable, BASE_LOOPCTRLS *BaseLoops, KSGAIN SpdRefi )/*<S00A>*/
{
#if (FLOAT_USE==TRUE)
	float	AbsSpdRefi;
	float	SpdRefx;
#else
	LONG	AbsSpdRefi;
	LONG	SpdRefx;
#endif /* FLOAT_USE */
	SPDCTRL *hSpdCtrl;

	hSpdCtrl = &BaseLoops->SpdCtrl;
	SpdRefx = SpdRefi;

	if(((BaseEnable == TRUE) && (hSpdCtrl->V.LastSvonFlag == FALSE))
			|| hSpdCtrl->V.SvonSpdLmtFlag )
	{
		AbsSpdRefi = SpdRefi;
		if( AbsSpdRefi < 0 )
		{
			AbsSpdRefi = -AbsSpdRefi;
		}

		if( AbsSpdRefi > (BaseLoops->BaseCtrls)->CtrlCmdPrm.SvonSpdLmtLevel )
		{ /* ����T�[�{�I�������x�w�߂�Pn529����̏ꍇ */
			/* �T�[�{ON�����x�����J�n */
			hSpdCtrl->V.SvonSpdLmtFlag = TRUE;
			if( SpdRefi >= 0 )
			{
				SpdRefx = (BaseLoops->BaseCtrls)->CtrlCmdPrm.SvonSpdLmtLevel;
			}
			else
			{
				SpdRefx = -(BaseLoops->BaseCtrls)->CtrlCmdPrm.SvonSpdLmtLevel;
			}
		}
		else if( AbsSpdRefi < (BaseLoops->BaseCtrls)->CtrlCmdPrm.SvonSpdLmtValue )
		{ /* ���x�w�߂�Pn529/10��菬�̏ꍇ */
			/* �T�[�{ON�����x�������� */
			hSpdCtrl->V.SvonSpdLmtFlag = FALSE;
		}
	}
	hSpdCtrl->V.LastSvonFlag = BaseEnable;
	return( SpdRefx );
}


/****************************************************************************************************/
/*																									*/
/*		�[���N�����v���䉉�Z����																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �X�L�����b����̃[���N�����v���䃂�[�h�w�߂ɂ��s���鏈���B							*/
/*			�ʒu�w�߁��O�ɂ��ʒu���[�v��g��Ń��[�^���N�����v���鏈���B							*/
/*			�t���N���[�Y�h����̏ꍇ���Z�~�N���[�Y�h���Z�ő��x�w�߂��v�Z����B						*/
/*																									*/
/*	�� �� : ���[�^�ʒu����ł���A�t���N���[�Y�h����̓T�|�[�g���Ă��Ȃ��̂Œ��ӂ��邱�ƁB			*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static KSGAIN	ZeroPositionControl( BASE_LOOPCTRLS *BaseLoops, KSGAIN dPosFbk )/*<S00A>*/
{
#if (FLOAT_USE==TRUE)
float	SpdRef;
#else
LONG	SpdRef;
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		�[���N�����v����																			*/
/*--------------------------------------------------------------------------------------------------*/
	/* �ʒu�΍����Z */
	BaseLoops->ZxCtrl.V.PosError -= dPosFbk;
#if (FLOAT_USE==TRUE)
	SpdRef = ( (float)BaseLoops->ZxCtrl.V.PosError * BaseLoops->GselGains->ZcKp );/* �ʒu���䉉�Z(P����)		*/
#else
	SpdRef = MlibMulgain( BaseLoops->ZxCtrl.V.PosError, BaseLoops->GselGains->ZcKp );	/* �ʒu���䉉�Z(P����)		*/
#endif /* FLOAT_USE */
	/*----------------------------------------------------------------------------------------------*/
	if( SpdRef == 0 )
	{ /* ���x�w�߃[�����̕␳ */
		if( BaseLoops->ZxCtrl.V.PosError > 0 )
		{
			SpdRef =  1;
		}
		else if( BaseLoops->ZxCtrl.V.PosError < 0 )
		{
			SpdRef = -1;
		}
	}
	/*----------------------------------------------------------------------------------------------*/
	else if( SpdRef > BaseLoops->ZxCtrl.P.SpdLimit )
	{ /* ���x�w�ߏ�����~�b�g */
		SpdRef =  BaseLoops->ZxCtrl.P.SpdLimit;
	}
	/*----------------------------------------------------------------------------------------------*/
	else if( SpdRef < -BaseLoops->ZxCtrl.P.SpdLimit )
	{ /* ���x�w�߉������~�b�g */
		SpdRef = -BaseLoops->ZxCtrl.P.SpdLimit;
	}
/*--------------------------------------------------------------------------------------------------*/
	return( SpdRef );

}

/****************************************************************************************************/
/*																									*/
/*		�������X����L�����̑��x�w�߈ʑ��i�ݕ⏞���Z												*/
/*																									*/
/****************************************************************************************************/
static KSGAIN	LpxSpdRefPhaseComp(SPDCTRL *SpdCtrl, KSGAIN SpdRefx, KSGAIN SpdRefPhFil, KSGAIN SpdRefPhFilGn)/*<S00A>*/
{
#if (FLOAT_USE==TRUE)
	float	SpdRefComp;
#else
	LONG	SpdRefComp;
#endif /* FLOAT_USE */

	/* ����ؑւ����̏���������		*//*<V392>*/
	if(SpdCtrl->V.SpdRefFilClrCmd == TRUE)
	{
		/* ���x�w�߈ʑ��i�ݕ⏞	*/
		SpdCtrl->V.SpdRefPhLpfo = SpdRefx;
		SpdCtrl->V.SpdRefPhHpfTmp = SpdRefx;

		SpdCtrl->V.SpdRefFilClrCmd = FALSE;
	}
	/* ���x�w��	*/
#if (FLOAT_USE==TRUE)
	SpdCtrl->V.SpdRefPhLpfo = FlibLpfilter1(SpdRefx, SpdRefPhFil, SpdCtrl->V.SpdRefPhLpfo);
	SpdCtrl->V.SpdRefPhHpfo = FlibHpfilter1(SpdRefx, SpdRefPhFil, &SpdCtrl->V.SpdRefPhHpfTmp);
#else
	SpdCtrl->V.SpdRefPhLpfo = MlibLpfilter1(SpdRefx, SpdRefPhFil, SpdCtrl->V.SpdRefPhLpfo);
	SpdCtrl->V.SpdRefPhHpfo = MlibHpfilter1(SpdRefx, SpdRefPhFil, &SpdCtrl->V.SpdRefPhHpfTmp);
#endif /* FLOAT_USE */

#if (FLOAT_USE==TRUE)
	SpdRefComp = SpdCtrl->V.SpdRefPhLpfo + ((LONG)SpdCtrl->V.SpdRefPhHpfo * SpdRefPhFilGn);
#else
	SpdRefComp = SpdCtrl->V.SpdRefPhLpfo + MlibMulgain(SpdCtrl->V.SpdRefPhHpfo, SpdRefPhFilGn);
#endif /* FLOAT_USE */

	return(SpdRefComp);
}

/****************************************************************************************************/
/*																									*/
/*		�������X����L�����̑��x�e�e�ʑ��i�ݕ⏞���Z												*/
/*																									*/
/****************************************************************************************************/
static KSGAIN	LpxSpdFFPhaseComp(SPDCTRL *SpdCtrl, KSGAIN SpdFFC, KSGAIN P_SpdFFC, KSGAIN SpdFFPhFil, KSGAIN SpdFFPhFilGn)/*<S00A>*/
{
#if (FLOAT_USE==TRUE)
	float	SpdFFCComp;
#else
	LONG	SpdFFCComp;
#endif /* FLOAT_USE */

	/* ����ؑւ����̏���������		*//*<V392>*/
	if(SpdCtrl->V.SpdFFFilClrCmd == TRUE)
	{
		SpdCtrl->V.SpdFFFilClrCmd = FALSE;

		/* ���x�e�e�ʑ��i�ݕ⏞	*/
		SpdCtrl->V.SpdFFPhLpfo = P_SpdFFC;
		SpdCtrl->V.SpdFFPhHpfTmp = P_SpdFFC;
	}
	/* ���x�e�e	*/
#if (FLOAT_USE==TRUE)
	SpdCtrl->V.SpdFFPhLpfo = FlibLpfilter1(SpdFFC, SpdFFPhFil, SpdCtrl->V.SpdFFPhLpfo);
	SpdCtrl->V.SpdFFPhHpfo = FlibHpfilter1(SpdFFC, SpdFFPhFil, &SpdCtrl->V.SpdFFPhHpfTmp);
#else	
	SpdCtrl->V.SpdFFPhLpfo = MlibLpfilter1(SpdFFC, SpdFFPhFil, SpdCtrl->V.SpdFFPhLpfo);
	SpdCtrl->V.SpdFFPhHpfo = MlibHpfilter1(SpdFFC, SpdFFPhFil, &SpdCtrl->V.SpdFFPhHpfTmp);
#endif /* FLOAT_USE */

#if (FLOAT_USE==TRUE)
	SpdFFCComp = SpdCtrl->V.SpdFFPhLpfo + (SpdCtrl->V.SpdFFPhHpfo * SpdFFPhFilGn);
#else
	SpdFFCComp = SpdCtrl->V.SpdFFPhLpfo + MlibMulgain(SpdCtrl->V.SpdFFPhHpfo, SpdFFPhFilGn);
#endif /* FLOAT_USE */

	return(SpdFFCComp);
}


/****************************************************************************************************/
/*																									*/
/*		���x���䉉�Z																				*/
/*																									*/
/****************************************************************************************************/
static KSGAIN	BaseSpeedControl( BASE_LOOPCTRLS *BaseLoops,
							  KSGAIN SpdFFC,			/* ���xFF�⏞�l									*/
							  KSGAIN SpdFBC,			/* ���xFB�⏞�l									*/
							  BOOL BaseEnable )			/* �ʒuFB�����l�i�[���N�����v�����p�j			*/
{
#if (FLOAT_USE==TRUE)
	float	SpdRefx;
	float	d_SpdRefx, d_SpdFFC;
	float	SpdErrP;
	float	SpdErrI;
	float	TrqRef0;		/*<V263>*/
#else
	LONG	SpdRefx;
	LONG	d_SpdRefx, d_SpdFFC;
	LONG	SpdErrP;
	LONG	SpdErrI;
	LONG	TrqRef0;		/*<V263>*/
#endif /* FLOAT_USE */

	BOOL	neri_calc_on;
	POS_MNG_HNDL *PosMngV;

	PosMngV = &BaseLoops->BaseCtrls->PosManager;
/*--------------------------------------------------------------------------------------------------*/
/*		���x�w�߉��Z																				*/
/*--------------------------------------------------------------------------------------------------*/
	/* �T�[�{����w�߃N���A�v���`�F�b�N */
	/* ���ʒu�΍��N���A�Ή��K�v�� */
	if( BaseLoops->BaseCtrls->BBRefClrReq )
	{ /* (BaseBlock && OT�ł͂Ȃ�) */
		/* ���x�w�߃N���A */
		BaseLoops->SpdRefo = 0;
	}
	else if( (BaseLoops->CtrlMode == CTRLMODE_SPD) || (BaseLoops->CtrlMode == CTRLMODE_JOG) )
	{
		/* �\�t�g�X�^�[�g���Z *//* ScanB����\�t�g�X�^�[�g���ړ�����K�v���� @@CHECK */
//		BaseLoops->SpdRefo = BaseSoftStartSpdRef( SpdRef, BaseLoops->SpdRefo );
		BaseLoops->SpdRefo = BaseLoops->BaseCtrls->CtrlCmdMngr.SpdRefo;
	}
	else if( BaseLoops->CtrlMode == CTRLMODE_ZSRCH ) /* <S050> */
	{
		BaseLoops->SpdRefo = BaseLoops->BaseCtrls->CtrlCmdMngr.SpdRefo;
	}
	else
	{
		/* �ʒu���䎞�̓\�t�g�X�^�[�g�Ȃ� */
		BaseLoops->SpdRefo = BaseLoops->SpdRefi;
	}

	BaseLoops->SpdRefSum = MlibSatAdd32(BaseLoops->SpdRefo, BaseLoops->SpdFFCx);

/*--------------------------------------------------------------------------------------------------*/
/*		�x�[�X�u���b�N���̏���																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( BaseEnable == FALSE )
	{
		MlibResetLongMemory( &(BaseLoops->SpdCtrl.V), sizeof(BaseLoops->SpdCtrl.V)/4 );
		BaseLoops->ZxCtrl.V.PosError = 0;
		return( 0 );
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�[������~, �[���N�����v����																*/
/*--------------------------------------------------------------------------------------------------*/
	if( BaseLoops->ZCtrlMode == CTRLMODE_ZSTOP )		/* �[������~								*/
	{
		SpdRefx = 0;
		SpdFFC = 0;
		BaseLoops->TrqFFCx = 0;
	}
/* <S1F5> Start */
	/*----------------------------------------------------------------------------------------------*/
	else if( BaseLoops->ZCtrlMode == CTRLMODE_LDSTOP )	/* ���������x�ɂ�錸����~					*/
	{
		SpdRefx = BaseLoops->BaseCtrls->CtrlCmdMngr.SpdLdstpRefo;
	}
/* <S1F5> End */
	/*----------------------------------------------------------------------------------------------*/
	else if( BaseLoops->ZCtrlMode == CTRLMODE_ZCLMP )	/* �[���N�����v								*/
	{
		/* �[���N�����v���䉉�Z */
		SpdRefx = ZeroPositionControl( BaseLoops, BaseLoops->dMotPos );
		SpdFFC = 0;
		BaseLoops->TrqFFCx = 0;
	}
	/*----------------------------------------------------------------------------------------------*/
	else												/* �ʏ퐧��									*/
	{
		SpdRefx = BaseLoops->SpdRefo;
		/* 2012.10.05 Y.Oka SGDV�ɂ͂Ȃ��B�K�v�H */
//		BaseLoops->ZxCtrl.V.PosError = 0;
	}
	/*----------------------------------------------------------------------------------------------*/
	if( BaseLoops->SpdCtrl.V.SvonSpdLmtFlag )			/* �T�[�{ON�����x���~�b�g					*/
	{
		SpdFFC = 0;
	}

	/*----------------------------------------------------------------------------------------------*/
	/*	�ʑ��i�݃t�B���^����	<V108>																*/
	/*----------------------------------------------------------------------------------------------*/
	d_SpdRefx = LpxSpdRefPhaseComp( &BaseLoops->SpdCtrl, SpdRefx,
									BaseLoops->BaseCtrls->CtrlCmdPrm.TLPrm.SpdRefPhFil,
									BaseLoops->BaseCtrls->CtrlCmdPrm.TLPrm.SpdRefPhFilGn );
	if( BaseLoops->CtrlMode == CTRLMODE_SPD )
	{
		d_SpdFFC = LpxSpdFFPhaseComp( &BaseLoops->SpdCtrl, SpdFFC, 0,
									  BaseLoops->BaseCtrls->CtrlCmdPrm.TLPrm.SpdRefPhFil,
									  BaseLoops->BaseCtrls->CtrlCmdPrm.TLPrm.SpdRefPhFilGn );
	}
	else
	{
		d_SpdFFC = LpxSpdFFPhaseComp( &BaseLoops->SpdCtrl, SpdFFC, SpdFFC,
									  BaseLoops->BaseCtrls->CtrlCmdPrm.TLPrm.SpdRefPhFil,
									  BaseLoops->BaseCtrls->CtrlCmdPrm.TLPrm.SpdRefPhFilGn );
	}
//	if( ((BaseLoops->BaseCtrls)->TuneLessCtrl.var.TuneLessAct) )									  /* <S1C7> */
	if( (TuneLessGetTuneLessSts(&((BaseLoops->BaseCtrls)->TuneLessCtrl)))			/* TuneLessAct	*/
	/*	&& !(TuneLessGetTuneLessModeEx(&((BaseLoops->BaseCtrls)->TuneLessCtrl))) */ )	/* TuningLessEx	*/
	{	/* �������X�L�� && �݊����[�h�������X */
		SpdRefx = d_SpdRefx;
		SpdFFC = d_SpdFFC;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		���[�h�X�C�b�`���Z	<S0BA>																	*/
/*--------------------------------------------------------------------------------------------------*/
	ModeSwitch(	&BaseLoops->BaseCtrls->ModeSwData,
				BaseLoops->TrqRefo,
				(SpdRefx + SpdFFC),
				PosMngV->var.PosErr );

/*--------------------------------------------------------------------------------------------------*/
/*		���x���䉉�Z																				*/
/*--------------------------------------------------------------------------------------------------*/
	if( (BaseLoops->ZCtrlMode != CTRLMODE_ZSTOP) && (BaseLoops->ZCtrlMode != CTRLMODE_ZCLMP) )
	{
		BaseLoops->SpdRefSum = MlibSatAdd32(SpdRefx, SpdFFC);
	}

	if( ( (SpdRefx - BaseLoops->SpdObsFbki >= 0) && (BaseLoops->SpdCtrl.V.TrqRef < 0) )
			|| ( (SpdRefx - BaseLoops->SpdObsFbki < 0) && (BaseLoops->SpdCtrl.V.TrqRef > 0) ) )
	{ /* ���x�΍��ƃg���N�w�߂̕������قȂ� */
		neri_calc_on = TRUE;
	}
	else
	{ /* ���x�΍��ƃg���N�w�߂̕���������(�g���N�w�� = 0�܂�) */
		neri_calc_on = FALSE;
	}

	if( (BaseLoops->CmdCtrlBit & ENBPCTRL_BIT)
			|| ((BaseLoops->BaseCtrls)->ModeSwData.var.ModeSWFlag
				&& (!BaseLoops->BaseCtrls->CtrlCmdPrm.IPSpdControl)) )
	{ /* ��ᐧ�䎞 */
//		BaseLoops->uCycInputs.MREG_BITDAT |= BITDAT_PCONACTFLG;
		BaseLoops->PConActFlg = TRUE; /* <S064> */
#if (FLOAT_USE==TRUE)
		BaseLoops->SpdCtrl.V.SpdFbFilo = FlibLpfilter1(	BaseLoops->SpdObsFbki + SpdFBC, 
															BaseLoops->BaseCtrls->CtrlCmdPrm.KVfbFil, 
															BaseLoops->SpdCtrl.V.SpdFbFilo );
		SpdErrP = SpdRefx - BaseLoops->SpdCtrl.V.SpdFbFilo + SpdFFC;
		BaseLoops->SpdCtrl.V.PacOut = SpdErrP * BaseLoops->GselGains->Kv ;
#else
		BaseLoops->SpdCtrl.V.SpdFbFilo = MlibLpfilter1( BaseLoops->SpdObsFbki + SpdFBC,
														BaseLoops->BaseCtrls->CtrlCmdPrm.KVfbFil,
														BaseLoops->SpdCtrl.V.SpdFbFilo );
		SpdErrP = SpdRefx - BaseLoops->SpdCtrl.V.SpdFbFilo + SpdFFC;
		BaseLoops->SpdCtrl.V.PacOut = MlibMulgain( SpdErrP, BaseLoops->GselGains->Kv );
#endif /* FLOAT_USE */
	/*------------------------------------------------------------------------------------------*/
		if( !BaseLoops->BaseCtrls->CtrlCmdPrm.SpdIctrlKeep )
		{ /* ���x����ϕ��ێ��ł͂Ȃ��ꍇ */
			/* ���萔�������Đϕ��N���A */
#if (FLOAT_USE==TRUE)
			BaseLoops->SpdCtrl.V.IacOut = FlibLpfilter1( 0.0f,
														 BaseLoops->SpdCtrl.P.KVintegFil,
														 BaseLoops->SpdCtrl.V.IacOut );
			BaseLoops->SpdCtrl.V.Ivar64[1] = BaseLoops->SpdCtrl.V.IacOut *2.0f;
#else
			BaseLoops->SpdCtrl.V.IacOut = MlibLpfilter1( 0,
														 BaseLoops->SpdCtrl.P.KVintegFil,
														 BaseLoops->SpdCtrl.V.IacOut );
			BaseLoops->SpdCtrl.V.Ivar64[1] = BaseLoops->SpdCtrl.V.IacOut << 1;
#endif /* FLOAT_USE */
			BaseLoops->SpdCtrl.V.Ivar64[0] = 0;
		}
	}
	else
	{ /* ���ϕ����䎞 */
//		BaseLoops->uCycInputs.MREG_BITDAT &= BITDAT_PCONACTFLG;
		BaseLoops->PConActFlg = FALSE; /* <S064> */
#if (FLOAT_USE==TRUE)
		BaseLoops->SpdCtrl.V.SpdFbFilo = FlibLpfilter1(	BaseLoops->SpdObsFbki + SpdFBC, 
															BaseLoops->BaseCtrls->CtrlCmdPrm.KVfbFil, 
															BaseLoops->SpdCtrl.V.SpdFbFilo );
		SpdErrP = ( SpdRefx * BaseLoops->BaseCtrls->CtrlCmdPrm.PI_rate ) - 
													BaseLoops->SpdCtrl.V.SpdFbFilo + SpdFFC;
		BaseLoops->SpdCtrl.V.PacOut = ( SpdErrP * BaseLoops->GselGains->Kv );
#else
		BaseLoops->SpdCtrl.V.SpdFbFilo = MlibLpfilter1( BaseLoops->SpdObsFbki + SpdFBC,
														BaseLoops->BaseCtrls->CtrlCmdPrm.KVfbFil,
														BaseLoops->SpdCtrl.V.SpdFbFilo );
		SpdErrP = MlibMulgainNolim( SpdRefx, BaseLoops->BaseCtrls->CtrlCmdPrm.PI_rate);
		SpdErrP = SpdErrP - BaseLoops->SpdCtrl.V.SpdFbFilo + SpdFFC;
		BaseLoops->SpdCtrl.V.PacOut = MlibMulgain( SpdErrP, BaseLoops->GselGains->Kv );
#endif /* FLOAT_USE */
	/*------------------------------------------------------------------------------------------*/
		SpdErrI = SpdRefx - BaseLoops->SpdCtrl.V.SpdFbFilo + SpdFFC;
	/*------------------------------------------------------------------------------------------*/
		/* �g���N�������ł͂Ȃ��A�܂��͑��x�΍��ƃg���N�w�߂̕������قȂ�ꍇ�ɐϕ����� */
		if ( (FALSE == BaseLoops->TrqClamp) || (neri_calc_on == TRUE) )
		{
//#if (FLOAT_USE==TRUE)
//			BaseLoops->SpdCtrl.V.IacOut = FlibIntegral( SpdErrI, BaseLoops->GselGains->Kvi, &BaseLoops->SpdCtrl.V.Ivar64 );
//#else
			BaseLoops->SpdCtrl.V.IacOut = MlibIntegral( SpdErrI, BaseLoops->GselGains->Kvi, BaseLoops->SpdCtrl.V.Ivar64 );
//#endif /* FLOAT_USE */
		}
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�G���R�[�_���X����										@@@ENCLESS@@@						*/
/*--------------------------------------------------------------------------------------------------*/
//	KpiMencLessSpdCtrl( &BaseLoops->SpdCtrl );				/* MencLess����F���[�^���x����␳		*/

/*--------------------------------------------------------------------------------------------------*/
/*		���x����o��																				*/
/*--------------------------------------------------------------------------------------------------*/
	TrqRef0 = BaseLoops->SpdCtrl.V.PacOut + BaseLoops->SpdCtrl.V.IacOut;
#if (FLOAT_USE==TRUE)
	BaseLoops->SpdCtrl.V.TrqRef = ( TrqRef0 * BaseLoops->GselGains->Kv2 );
#else
	BaseLoops->SpdCtrl.V.TrqRef = MlibMulgain29( TrqRef0, BaseLoops->GselGains->Kv2 );
#endif
	return( BaseLoops->SpdCtrl.V.TrqRef );
}


/****************************************************************************************************/
/*																									*/
/*		���[�h�X�C�b�`���Z																			*/
/*																									*/
/****************************************************************************************************/
static void ModeSwitch( MODE_SW *ModeSwData, LONG trq_ref, LONG spd_ref, LONG pos_err )
{
	LONG	ModeSWFlag;
	LONG	wk;

	/* ���[�J���ϐ��̏����� */
	ModeSWFlag = FALSE;
	wk = 0;

	switch( ModeSwData->conf.ModeSWSel )
	{
	case MODESW_TRQREF: /* ���[�h�X�C�b�` �g���N�w�� */
		wk = trq_ref;
		if( wk < 0 )
		{
			wk = -wk;
		}
		if( wk >= ModeSwData->conf.MSWTrqLevel )
		{
			ModeSWFlag = TRUE;
		}
		break;

	case MODESW_SPDREF: /* ���[�h�X�C�b�` ���x�w�� */
		wk = spd_ref;
		if( wk < 0 )
		{
			wk = -wk;
		}
		if( wk >= ModeSwData->conf.MSWSpdLevel )
		{
			ModeSWFlag = TRUE;
		}
		break;

	case MODESW_MOTACC: /* ���[�h�X�C�b�` �����x */
		wk = ModeSwData->var.Acceleration;
		if( wk < 0 )
		{
			wk = -wk;
		}
		if( wk >= ModeSwData->conf.MSWAccLevel )
		{
			ModeSWFlag = TRUE;
		}
		break;

	case MODESW_POSERR: /* ���[�h�X�C�b�` �΍��p���X */
		wk = pos_err;
		if( wk < 0 )
		{
			wk = -wk;
		}
		if( wk >= ModeSwData->conf.MSWErrLevel )
		{
			ModeSWFlag = TRUE;
		}
		break;

	case MODESW_NOTUSE: /* ���[�h�X�C�b�` �s�g�p */
	default:
		break;
	}
	ModeSwData->var.ModeSWFlag = ModeSWFlag;
}



/****************************************************************************************************/
/*																									*/
/*		�g���N�t�B���^ for �ʒu���[�v & ���x���[�v���Z												*/
/*																									*/
/****************************************************************************************************/
static KSGAIN	BaseTorqueFilterForPSCtrl( BASE_LOOPCTRLS *BaseLoops, KSGAIN TrqRefi,
											KSGAIN TrqFBC, KSGAIN TrqFFC, KSGAIN BaseEnable )/*<S00A>*//* <S0B8> */
{
	TRQFIL *TrqFil;
#if (FLOAT_USE==TRUE)
	float	TrqRefx;
	float	DisTrqRef1 = 0;			/* �g���N�O���w�ߓ���1 */
	float	DisTrqRef2 = 0;			/* �g���N�O���w�ߓ���2 */
#else
	LONG	TrqRefx;
	LONG	DisTrqRef1 = 0;			/* �g���N�O���w�ߓ���1 */
	LONG	DisTrqRef2 = 0;			/* �g���N�O���w�ߓ���2 */
#endif /* FLOAT_USE */

	TrqFil = &BaseLoops->TrqFil;
/*--------------------------------------------------------------------------------------------------*/
/*		�e�[�u���^�]���̃g���N�O���w�ߓ��͏���														*/
/*--------------------------------------------------------------------------------------------------*/
	BaseLoops->DisTrqRef3 = 0;						/* �g���N�O���w�ߓ���3							*/
#if (FLOAT_USE==TRUE)
//	switch( TrqFil->V.TrqInputPos ) /* <S0B7> */
	switch( BaseLoops->BaseCtrls->TblDrive.var.DisTrqInTiming )										  /* <S0B7> */
	{
	case 0x01:										/* �g���N�w�߃t�B���^�O							*/
		DisTrqRef1 = (BaseLoops->BaseCtrls->TblDrive.var.TrqRef);
		break;
	case 0x02:										/* ���[�p�X�t�B���^��A�m�b�`�t�B���^�O			*/
		DisTrqRef2 = (BaseLoops->BaseCtrls->TblDrive.var.TrqRef);
		break;
	case 0x03:										/* �g���N�w�߃t�B���^��							*/
		BaseLoops->DisTrqRef3 = (BaseLoops->BaseCtrls->TblDrive.var.TrqRef);
		break;
	case 0x00:										/* �Ȃ�											*/
	default :
		break;
	}
#else
	switch( BaseLoops->BaseCtrls->TblDrive.var.DisTrqInTiming )										  /* <S0B7> */
	{
	case 0x01:										/* �g���N�w�߃t�B���^�O							*/
		DisTrqRef1 = BaseLoops->BaseCtrls->TblDrive.var.TrqRef;
		break;
	case 0x02:										/* ���[�p�X�t�B���^��A�m�b�`�t�B���^�O			*/
		DisTrqRef2 = BaseLoops->BaseCtrls->TblDrive.var.TrqRef;
		break;
	case 0x03:										/* �g���N�w�߃t�B���^��							*/
		BaseLoops->DisTrqRef3 = BaseLoops->BaseCtrls->TblDrive.var.TrqRef;
		break;
	case 0x00:										/* �Ȃ�											*/
	default :
		break;
	}
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		�x�[�X�u���b�N���̏���																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( BaseEnable == FALSE )
	{
		TrqFil->V.TrqFilClrCmd = FALSE;				/* �ϕ��������R�}���h�N���A						*/
		TrqFil->V.FilOut = 0;
		TrqFil->V.FilOut3 = 0;
		TrqFil->V.LpFil2[0] = 0;
		TrqFil->V.LpFil2[1] = 0;
		TrqFil->V.LpFil2[2] = 0;
		TrqFil->V.LpFil2[3] = 0;
		return( 0 );
	}
#if (FLOAT_USE==TRUE)
	TrqRefx = FlibLimitul( (TrqRefi + DisTrqRef1), (float)0x01000000, (float)-0x01000000 );
#else
	TrqRefx = MlibLimitul( (TrqRefi + DisTrqRef1), 0x01000000, -0x01000000 );
#endif /* FLOAT_USE */
/*--------------------------------------------------------------------------------------------------*/
/*		1�����[�p�X�t�B���^�ϕ�����������(�g������ON/OFF���Ɏg�p����)								*/
/*--------------------------------------------------------------------------------------------------*/
	if(TrqFil->V.TrqFilClrCmd == TRUE)			/* �ϕ��������R�}���h����							*/
	{
		TrqFil->V.FilOut = TrqRefx;				/* �g���N�t�B���^�ϕ��l�������͒l�ŏ�����			*/
		TrqFil->V.TrqFilClrCmd = FALSE;			/* �ϕ��������R�}���h�N���A							*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		1�����[�p�X�t�B���^����																		*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	TrqFil->V.FilOut = FlibLpfilter1( TrqRefx, BaseLoops->GselGains->Klpf, TrqFil->V.FilOut );
#else
	TrqFil->V.FilOut = MlibLpfilter1( TrqRefx, BaseLoops->GselGains->Klpf, TrqFil->V.FilOut );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		�g���N�e�a�⏞����																			*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	TrqRefx = FlibLimitul( (TrqFil->V.FilOut + TrqFFC - TrqFBC + DisTrqRef2), 0x01000000, -0x01000000 );
#else
	TrqRefx = MlibLimitul( (TrqFil->V.FilOut + TrqFFC - TrqFBC + DisTrqRef2), 0x01000000, -0x01000000 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		1�����[�p�X�t�B���^����(�g���N�⏞��)														*/
/*			���ӁF�g���N�⏞��̃t�B���^�͒������X�̃��o�X�g��������������̂ŁA�t�B���^��			*/
/* 				  �ŏ����ɂ��邱�ƁB																*/
/*--------------------------------------------------------------------------------------------------*/
		if(BaseLoops->BaseCtrls->CtrlCmdPrm.LpassFil3 == TRUE)
		{ /* �g���N�⏞��g���N�t�B���^�L�� */
			if(BaseLoops->BaseCtrls->TuneLessCtrl.conf.TuningLessUse )
			{ /* �������X�L�� */
#if (FLOAT_USE==TRUE)
				TrqFil->V.FilOut3 = FlibLpfilter1( TrqRefx,
												   (BaseLoops->BaseCtrls)->CtrlCmdPrm.TLPrm.Klpf3,
												   TrqFil->V.FilOut3 );
#else
				TrqFil->V.FilOut3 = MlibLpfilter1( TrqRefx,
												   (BaseLoops->BaseCtrls)->CtrlCmdPrm.TLPrm.Klpf3,
												   TrqFil->V.FilOut3 );
#endif /* FLOAT_USE */
				TrqRefx = TrqFil->V.FilOut3;
			}
			else
			{
				TrqFil->V.FilOut3 = 0;
			}
		}
		else
		{
			TrqFil->V.FilOut3 = 0;
		}

/*--------------------------------------------------------------------------------------------------*/
/*		�Q�����[�p�X�t�B���^����																	*/
/*--------------------------------------------------------------------------------------------------*/
		if(FALSE == BaseLoops->BaseCtrls->CtrlCmdPrm.LpassFil2)
		{
			TrqFil->V.LpFil2[0] = 0;
			TrqFil->V.LpFil2[1] = 0;
			TrqFil->V.LpFil2[2] = 0;
			TrqFil->V.LpFil2[3] = 0;
		}
		else
		{
#if (FLOAT_USE==TRUE)
			TrqRefx = FlibLowPassfilter2( TrqRefx,
										  (BaseLoops->BaseCtrls)->CtrlCmdPrm.Klpf2,
//										  (BaseLoops->BaseCtrls)->CtrlCmdPrm.Klpf2, 	/* S036 */
										  TrqFil->P.Klpf2, 						/* S036 */	/* TODO:�v�Z���ʖ��m�F */
										  TrqFil->V.LpFil2 );
#else
			TrqRefx = MlibLowPassfilter2( TrqRefx,
//										  (BaseLoops->BaseCtrls)->CtrlCmdPrm.Klpf2, 	/* S036 */
										  TrqFil->P.Klpf2, 						/* S036 */
										  TrqFil->V.LpFil2 );
#endif /* FLOAT_USE */
		}
/*--------------------------------------------------------------------------------------------------*/
//		BoutV.TrcCompTrqRef = TrqRefx;		/* �O�����͑O�g���N�w��									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
		return( FlibLimitul( TrqRefx, (float)0x01000000, (float)-0x01000000 ) );
#else
		return( MlibLimitul( TrqRefx, 0x01000000, -0x01000000 ) );
#endif /* FLOAT_USE */
}



/****************************************************************************************************/
/*																									*/
/*		�g���N����w�߉��Z����		<S0B8>															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �g���N���䃂�[�h���̃g���N�w�ߓ��͏����ƃg���N�w�߃t�B���^�������s���B					*/
/*																									*/
/*																									*/
/*	�⑫ 1) �g���N�t�B���^���Z�p�ϐ��́A��{�T�[�{����g���N�t�B���^�����Ƌ��p���A					*/
/*			���䃂�[�h�ؑւ����̃g���N�w�߂̘A�������m�ۂ���B										*/
/*		 2) �n�s�������́A�n�s�V�[�P���X�p(ScanC)�ɁA�ꎟ���[�p�X�t�B���^���Z���s���B				*/
/*		 3) �[������~����у[���N�����v����́A��{�T�[�{���䏈���ɔC����B						*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static LONG	BaseTorqueControl( BASE_LOOPCTRLS *BaseLoops, BOOL BeComplete )
{
	LONG	TrqRefx;
//	LONG	TRefAd;//<S0C7>

/****************************************************************************************************/
/*		�e�[�u���^�]����																			*/
/****************************************************************************************************/
	if( BaseLoops->BaseCtrls->TblDrive.var.TblCmd != TBLCMD_NOCMD )
	{
	/*----------------------------------------------------------------------------------------------*/
		if( BaseLoops->ZCtrlMode != CTRLMODE_ZNONE )		/* �[��������^�[���N�����v���䎞		*/
		{
			/* ���x���䉉�Z */
			BaseLoops->TrqRefi = BaseSpeedControl( BaseLoops,
												   BaseLoops->SpdFFCx,
												   BaseLoops->SpdFBCx,
												   BeComplete );
			TrqRefx = BaseLoops->TrqRefi;				/* �g���N�w�߃t�B���^���������s����			*/
		}
	/*----------------------------------------------------------------------------------------------*/
		else if( BeComplete == FALSE )
		{ /* �x�[�X�u���b�N�� */
			BaseLoops->TrqRefo = 0;						/* Output = Input (No Filter)				*/
			BaseLoops->TrqFil.V.FilOut = 0;
			BaseLoops->TrqFil.V.LpFil2[0] = 0;
			BaseLoops->TrqFil.V.LpFil2[1] = 0;
			BaseLoops->TrqFil.V.LpFil2[2] = 0;
			BaseLoops->TrqFil.V.LpFil2[3] = 0;
			return	BaseLoops->TrqRefo;					/* �g���N�w�߃t�B���^���������s���Ȃ�		*/
		}
	/*----------------------------------------------------------------------------------------------*/
		else
		{
			/* �g���N�w�ߓ��� = �e�[�u���^�]�g���N�w�� */
			BaseLoops->TrqRefi = BaseLoops->BaseCtrls->TblDrive.var.TrqRef;
			TrqRefx = BaseLoops->TrqRefi;				/* �g���N�w�߃t�B���^���������s����			*/
		}
	}

/****************************************************************************************************/
/*		�e�[�u���^�]�����ȊO																		*/
/****************************************************************************************************/
	else
	{
/*--------------------------------------------------------------------------------------------------*/
/*		�g���N�w�ߓ��͏���																			*/
/*--------------------------------------------------------------------------------------------------*/
#if	SVFSEL_ADC_USE == 1
		/* �A�i���O�w�� */
//		BaseLoops->TrqRefi = MlibLpfilter1( BaseLoops->TRefAd,
//											BaseLoops->TrqFil.A.Ktrflpf,
//											BaseLoops->TrqRefi );
		if( BaseLoops->MencV->PhaseReady == FALSE )						/* <S21C> */
		{/* ���Ɍ��o�� */
			BaseLoops->TrqRefi = (BaseLoops->BaseCtrls)->CtrlCmdMngr.TrqRefo;
		}
		else
		{/* �ʑ����o���� */
			BaseLoops->TrqRefi = MlibLpfilter1( BaseLoops->TRefAd,
												BaseLoops->TrqFil.A.Ktrflpf,
												BaseLoops->TrqRefi );
		}																/* <S21C> */
		
#else
		/* �ʐM�w�� */
		BaseLoops->TrqRefi = (BaseLoops->BaseCtrls)->CtrlCmdMngr.TrqRefo;
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		���x�����g���N���Z����																		*/
/*--------------------------------------------------------------------------------------------------*/
		TrqRefx = BaseLoops->TrqRefi - (BaseLoops->BaseCtrls)->CtrlCmdMngr.OverTrq;

/*--------------------------------------------------------------------------------------------------*/
/*		�e�[�u���^�]���ȊO�̃[������~���[���N�����v���䎞�̏���									*/
/*--------------------------------------------------------------------------------------------------*/
/*		�R�����g�ǉ��F���L�ύX�ɂ��g���N���䒆�̃[������~�͂Ȃ��̂ŉ��L�����͕s�v				*/
/*			BK���ݽ�ɂ�Pn506��0�ł��ٸ���䒆�;�ۑ���~�����A����p���ɕύX���Ă���					*/
/*			�ٸ���䒆�ɾ�ۑ��װє������Ă��ۑ���~�����APn001.0�̒�~���@�ɏ]��					*/
/*--------------------------------------------------------------------------------------------------*/
		if( BaseLoops->ZCtrlMode != CTRLMODE_ZNONE )
		{
			/* �O��g���N�w�ߒl�����̂܂ܕԂ� */
			return	BaseLoops->TrqRefo;
		}

/*--------------------------------------------------------------------------------------------------*/
/*		�x�[�X�u���b�N���̏���																		*/
/*--------------------------------------------------------------------------------------------------*/
		else if( BeComplete == FALSE )
		{
			BaseLoops->TrqRefo = 0;
			if ( BaseLoops->BaseCtrls->BBRefClrReq )
			{
				BaseLoops->TrqRefi = 0;
			}
			BaseLoops->TrqFil.V.FilOut = 0;
			BaseLoops->TrqFil.V.LpFil2[0] = 0;
			BaseLoops->TrqFil.V.LpFil2[1] = 0;
			BaseLoops->TrqFil.V.LpFil2[2] = 0;
			BaseLoops->TrqFil.V.LpFil2[3] = 0;
			return	BaseLoops->TrqRefo;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�t�B���^�Ȃ�����		<V181>																*/
/*--------------------------------------------------------------------------------------------------*/
	if( BaseLoops->TrqFil.V.TrqFilDisable )
	{
		BaseLoops->TrqRefo   = MlibLimitul( TrqRefx, 0x01000000, -0x01000000 );
		BaseLoops->TrqFil.V.FilOut = TrqRefx;
		BaseLoops->TrqFil.V.LpFil2[0] = 0;
		BaseLoops->TrqFil.V.LpFil2[1] = 0;
		BaseLoops->TrqFil.V.LpFil2[2] = 0;
		BaseLoops->TrqFil.V.LpFil2[3] = 0;
		return	BaseLoops->TrqRefo;;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�P�����[�p�X�t�B���^�ϕ�����������(�g������ON/OFF���Ɏg�p����)								*/
/*--------------------------------------------------------------------------------------------------*/
/*			�g���N���䎞�� LpxInitTrqCtrlModeVar( )�ŏ������������s�����߁A							*/
/*			�����ł͐ϕ��������R�}���h�̃N���A�̂ݍs���B											*/
/*--------------------------------------------------------------------------------------------------*/
	if( BaseLoops->TrqFil.V.TrqFilClrCmd == TRUE )
	{ /* �ϕ��������R�}���h���� */
		/* �ϕ��������R�}���h�N���A */
		BaseLoops->TrqFil.V.TrqFilClrCmd = FALSE;
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�P�����[�p�X�t�B���^����																	*/
/*--------------------------------------------------------------------------------------------------*/
	BaseLoops->TrqFil.V.FilOut = MlibLpfilter1( TrqRefx,
												BaseLoops->TrqFil.A.Klpf,
												BaseLoops->TrqFil.V.FilOut );

	TrqRefx = MlibLimitul( BaseLoops->TrqFil.V.FilOut, 0x01000000, -0x01000000 );

/*--------------------------------------------------------------------------------------------------*/
/*		�Q�����[�p�X�t�B���^����																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( BaseLoops->BaseCtrls->CtrlCmdPrm.LpassFil2 == FALSE )
	{
		BaseLoops->TrqFil.V.LpFil2[0] = 0;
		BaseLoops->TrqFil.V.LpFil2[1] = 0;
		BaseLoops->TrqFil.V.LpFil2[2] = 0;
		BaseLoops->TrqFil.V.LpFil2[3] = 0;
	}
	else
	{
		TrqRefx = MlibLowPassfilter2( TrqRefx,
									  BaseLoops->TrqFil.P.Klpf2,
									  BaseLoops->TrqFil.V.LpFil2 );
	}

	BaseLoops->TrqRefo = MlibLimitul( TrqRefx, 0x01000000, -0x01000000 );

	return	BaseLoops->TrqRefo;

}



/****************************************************************************************************/
/*																									*/
/*		�g���N���䃂�[�h�ؑ֏���ϐ�������		<S0B8>												*/
/*																									*/
/****************************************************************************************************/
void	BaseInitTrqCtrlVariables( BASE_LOOPCTRLS *BaseLoops, SHORT IqRefMon, LONG *SpdRefo )
{
	if( BaseLoops->LastCtrlMode != CTRLMODE_TRQ )
	{ /* �O��̐��䃂�[�h���g���N���䃂�[�h�ȊO */
		/* ����g���N�w�� = �g���N�w�߃t�B���^�o�� = �O��q���d���w�� */
		BaseLoops->TrqFil.V.FilOut = MlibMulhigh32( (IqRefMon << 16), 73300775 );
		BaseLoops->TrqRefi = BaseLoops->TrqFil.V.FilOut;

		/* �g���N���䎞�͊O������ = 0 */
		BaseLoops->DisTrqRef3 = 0;

		/* �g���N���䎞�͑��x�w�� = 0 */
		*SpdRefo = 0;
	}
	return;
}



/****************************************************************************************************/
/*																									*/
/*		�g���N�w�ߏo�� : Torque Reference --> ScanA AinK.IqRef/Idref								*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�ʑ��⏞����A : �]������																		*/
/*																									*/
/*		IdRef = 0;																					*/
/*						  15000		TrqRef * 15000 * 2^8											*/
/*		IqRef = TrqRef * ------- = ----------------------											*/
/*						   2^24				2^32													*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�ʑ��⏞����B : IdRef/IqRef����																	*/
/*																									*/
/*						  15000		MlibFASTSINS16( Pcmps + 0   )	  TrqRef * 1875 * SinX			*/
/*		IdRef = TrqRef * ------- * ------------------------------- = -----------------------		*/
/*						   2^24				16384							2^32 * 2^3				*/
/*																									*/
/*						  15000		MlibFASTSINS16( Pcmps + 256 )	  TrqRef * 1875 * CosX			*/
/*		IqRef = TrqRef * ------- * ------------------------------- = -----------------------		*/
/*						   2^24				16384							2^32 * 2^3				*/
/*																									*/
/****************************************************************************************************/
static void	KaiOutputTrqRef(BASE_LOOPCTRLS *BaseLoops, KSGAIN TrqRef, KSGAIN IdRef, KSGAIN DisTrq )/*<S00A>*/
{
#if (FLOAT_USE==TRUE)
	float IqRef;
#else
	LONG	IqRef;
#endif /* FLOAT_USE */
/*--------------------------------------------------------------------------------------------------*/
/*		�d���w�߉��Z : 2^24 --> 15000																*/
/*--------------------------------------------------------------------------------------------------*/
#if( SVFSEL_PCMP_IDREF != 1 )						/* Pcmps by MotPhase							*/
	IdRef = MlibMulhigh32( IdRef,  3840000 );		/* IdRef(2^24)  --> IdRef(15000)				*/
	IqRef = MlibMulhigh32( TrqRef, 3840000 );		/* TrqRef(2^24) --> IqRef(15000)				*/
/*--------------------------------------------------------------------------------------------------*/
#else /* ( SVFSEL_PCMP_IDREF == 1 ) */				/* Pcmps by IdRef/IqRef							*/
	LONG	x,y;
	LONG	Pcmps;
	LONG	Pcmpx;
	LONG	Pcmpy;

	Pcmps = MlibABS( AinK.Pcmps );					/* Pcmps [65536/360deg]							*/
	Pcmpx = Pcmps >> 6;								/* Pcmpx [ 1024/360deg]							*/
	Pcmpy = Pcmps & 0x003F;							/* Pcmpy [Rem : 6bit]							*/
	/*----------------------------------------------------------------------------------------------*/
	x = MlibFASTSINS16( Pcmpx );					/* x = Sin( Pcmpx )								*/
	y = MlibFASTSINS16( Pcmpx + 1 );				/* y = Sin( Pcmpx + 1 )							*/
	y = (x + (((y - x) * Pcmpy)>>6)) * 1875;		/* y = (Sin( Pcmpx ) + dx) * 1875				*/
	IdRef = (MlibMulhigh32( TrqRef, y )+4) >> 3;	/* IdRef = TrqRef * Sin( x )					*/
	if( IdRef > 0 )
	{ /* IdRef is Negative Compensation */
		IdRef = -IdRef;
	}
	/*----------------------------------------------------------------------------------------------*/
	x = MlibFASTSINS16( Pcmpx + 256 );				/* x = Cos( Pcmpx )								*/
	y = MlibFASTSINS16( Pcmpx + 256 + 1 );			/* y = Cos( Pcmpx + 1 )							*/
	y = (x + (((y - x) * Pcmpy)>>6)) * 1875;		/* y = (Cos( Pcmpx ) + dx) * 1875				*/
	IqRef = (MlibMulhigh32( TrqRef, y )+4) >> 3;	/* IqRef = TrqRef * Cos( x )					*/
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		�d���w��(d����q��)�o��																		*/
/*--------------------------------------------------------------------------------------------------*/
	BaseLoops->IdRef   = (SHORT)IdRef;				/* IdRef  --> AinK.IdRef						*/
	BaseLoops->IqRef   = (SHORT)IqRef;				/* IqRef  --> AinK.IqRef	 					*/
/*--------------------------------------------------------------------------------------------------*/
/*		�O���g���N�w��																				*/
/*--------------------------------------------------------------------------------------------------*/
	/* DisTrq(2^24) --> AinK.IqDist(15000) */
	BaseLoops->IqDist = (SHORT)MlibMulhigh32( DisTrq, 3840000 );
/*--------------------------------------------------------------------------------------------------*/

}


/****************************************************************************************************/
/*																									*/
/*		����T�[�{���䏈�����C��																	*/
/*																									*/
/****************************************************************************************************/
void ApxYecServoControl( BASE_LOOPCTRLS *BaseLoops, CTRL_LOOP_OUT *CtrlLoopOut ) /* <S0B8> */
{
	LONG			lwk;
	BOOL			BeComplete;
	TUNELESS_CTRL	*TuneLessCtrl;
	DOBS_CTRL		*DobsCtrl;
	RPLCMP			*RippleCmp;		/* <S14B> */
//	BOOL			RippleCmpExe;	/* <S14B> *//* <S1FD> */


	/* ���[�J���ϐ��̏����� */
	BeComplete		= (BaseLoops->BaseCtrls)->BeComplete;
	TuneLessCtrl	= &(BaseLoops->BaseCtrls->TuneLessCtrl);
	DobsCtrl		= &(BaseLoops->BaseCtrls->DobsCtrl);
	RippleCmp		= &(BaseLoops->RippleCmp);			/* <S14B> */
//	RippleCmpExe	= FALSE;							/* <S14B> *//* <S1FD> */

/*--------------------------------------------------------------------------------------------------*/
/*		����p�p�����[�^�]������																	*//* <S066> */
/*--------------------------------------------------------------------------------------------------*/
	/* Control Parameter Translate */
	if( TR_REQ_SCANA == TranlateCtrlGetReq(&(BaseLoops->BaseCtrls->TransduceCtrl)) )
	{
		TranlateCtrlPrm( &(BaseLoops->BaseCtrls->TransduceCtrl) );
	}

/*--------------------------------------------------------------------------------------------------*/
/*		���x�e�a���Z�A�ʒu�w�ߍ쐬																	*/
/*--------------------------------------------------------------------------------------------------*/
	LpxPosSpdFeedbackCal( BaseLoops );				/* ���[�^�ʒu�����x�e�a���Z						*/

/*--------------------------------------------------------------------------------------------------*/
/*		�T�[�{�g������																				*/
/*--------------------------------------------------------------------------------------------------*/
	if( TRUE == TuneLessGetTuneLessUse(TuneLessCtrl) )
	{ /* �������X�L���� */
		/*------------------------------------------------------------------------------------------*/
		/*	�������X���� --> AoutV.SpdFbki, AoutV.TrqFBCx											*/
		/*------------------------------------------------------------------------------------------*/
		TuningLessCtrl( TuneLessCtrl,
						BaseLoops->SpdFbki,
						BaseLoops->TrqRefi,
						BaseLoops->TrqRefoLimit );

		/*------------------------------------------------------------------------------------------*/
		/*	�������X���䉉�Z���ʎ擾																*/
		/*------------------------------------------------------------------------------------------*/
		if( TRUE == TuneLessGetTuneLessSts(TuneLessCtrl) )
		{ /* �������X�L���� */
			/* ���x�t�B�[�h�o�b�N[2^24/OvrSpd] */
			BaseLoops->SpdObsFbki = TuneLessGetSpdObsFbk( TuneLessCtrl );
			/* �g���NFB�⏞[2^24/MaxTrq] */
			BaseLoops->TrqFBCx += TuneLessGetDisTrqCmp( TuneLessCtrl );

#if 0 /* 2013.08.16 Y.Oka �A�i���O���j�^���Ή� @@Check */
			/* ���葬�x[2^24/OvrSpd] */
			BoutV.AmonSpdObs = BaseLoops->SpdObsFbki;
			/* �O���g���N[2^24/MaxTrq] */
			BoutV.AmonDisTrq  = -TuneLessGetDisTrq( TuneLessCtrl );
			/* �⏞�g���N[2^24/MaxTrq] */
			BoutV.AmonDisTrqCmp = -TuneLessGetDisTrqCmp( TuneLessCtrl );
#endif
		}
		else
		{ /* �������X������ */
			/* ���x�t�B�[�h�o�b�N[2^24/OvrSpd]�X�V�Ȃ� */
			/* �g���NFB�⏞[2^24/MaxTrq]�X�V�Ȃ� */
			BaseLoops->TrqFil.V.TrqFilClrCmd = TRUE;

#if 0 /* 2013.08.16 Y.Oka �A�i���O���j�^���Ή� @@Check */
			/* ���葬�x[2^24/OvrSpd] */
			BoutV.AmonSpdObs = 0;
			/* �O���g���N[2^24/MaxTrq] */
			BoutV.AmonDisTrq  = -TuneLessGetDisTrq( TuneLessCtrl );
			/* �⏞�g���N[2^24/MaxTrq] */
			BoutV.AmonDisTrqCmp = 0;
#endif
		}

		/* �g���N�w�߃t�B���^�ϕ��������v�� */
		if( TRUE == TuneLessGetTFilClrReq(TuneLessCtrl) )
		{
			BaseLoops->TrqFil.V.TrqFilClrCmd = TRUE;
		}
	}
	else
	{ /* �������X������ */
		/*------------------------------------------------------------------------------------------*/
		/*	�O���I�u�U�[�o���Z --> AoutV.TrqFBCx													*/
		/*------------------------------------------------------------------------------------------*/
		DisturbObserver( DobsCtrl,
						 BaseLoops->SpdObsFbki,
						 BaseLoops->TrqRefoLimit );

		if( TRUE == DobsGetObserverSts(DobsCtrl) )
		{ /* �O���I�u�U�[�o�L���� */
			BaseLoops->TrqFBCx += DobsGetDisTrqCmp( DobsCtrl );
		}

		switch ( DobsGetExchangeSts(DobsCtrl) )
		{

		case DOBSCHNG_INV2ACT:	/* �����˗L�� */
			/* ���x�ϕ��g���N = ���x�ϕ��g���N - �g���N�⏞ (�g���N�⏞�͕��̂��߉��Z) */
#if (FLOAT_USE==TRUE)
			lwk = ((float)DobsCtrl->var.DisTrqCmp * BaseLoops->GselGains->Kv2Inv);
#else
			lwk = MlibMulgain(DobsCtrl->var.DisTrqCmp, BaseLoops->GselGains->Kv2Inv);
#endif /* FLOAT_USE */

			BaseLoops->SpdCtrl.V.Ivar64[1] += (lwk << 1);
			/* �g���N�w�߃t�B���^�ϕ��������v�� */
			BaseLoops->TrqFil.V.TrqFilClrCmd = TRUE;
			break;

		case DOBSCHNG_ACT2INV:	/* �L���˖��� */
			/* ���x�ϕ��g���N = ���x�ϕ��g���N + �g���N�⏞ (�g���N�⏞�͕��̂��ߌ��Z) */
#if (FLOAT_USE==TRUE)
			lwk = ((float)DobsCtrl->var.DisTrqCmp * BaseLoops->GselGains->Kv2Inv);
#else
			lwk = MlibMulgain(DobsCtrl->var.DisTrqCmp, BaseLoops->GselGains->Kv2Inv);
#endif /* FLOAT_USE */
			BaseLoops->SpdCtrl.V.Ivar64[1] -= (lwk << 1);
			/* �g���N�w�߃t�B���^�ϕ��������v�� */
			BaseLoops->TrqFil.V.TrqFilClrCmd = TRUE;
			break;

		case DOBS_NOCHANGE:		/* �؂�ւ��Ȃ� */
		default:
			break;
		}

#if 0 /* 2013.08.16 Y.Oka �A�i���O���j�^���Ή� @@Check */
		/*--------------------------------------------------------------------------------------------------*/
		/*		���j�^�p�ϐ��o��				����)�F�h�⏞�h�g���N���o�͂��邽�ߊO���g���N�ƕ����t�]		*/
		/*--------------------------------------------------------------------------------------------------*/
		/* �O���I�u�U�[�o���葬�x[2^24/OvrSpd] */
		BoutV.AmonDobSpd  = DobsCtrl->var.SpdObs;
		/* �O���g���N[2^24/MaxTrq] */
		BoutV.AmonDisTrq  = -DobsCtrl->var.DTrqLpfo;
		/* ���C�⏞�g���N[2^24/MaxTrq] */
		BoutV.AmonDisTrqCmp = DobsCtrl->var.DisTrqCmp;
#endif


	}

	/************************************************************************************************/
	/* ���䃋�[�v���Z																				*/
	/************************************************************************************************/
	switch(BaseLoops->CtrlMode)
	{
	/*----------------------------------------------------------------------------------------------*/
	/* �ʒu���䃂�[�h & PJOG���[�h																	*/
	/*----------------------------------------------------------------------------------------------*/
	case CTRLMODE_POS:
		/* �ʒu�w�ߍX�V �� �ʒu�΍����Z <S14C> */
		LpxSvPositionManager( BaseLoops, &(BaseLoops->BaseCtrls->PosManager) );

		/* �ʒu���䉉�Z���� */
		BaseLoops->SpdRefp = BasePositionControlA( BaseLoops,
												   (BaseLoops->BaseCtrls)->CtrlCmdMngr.PerrClrReq,
												   (BaseLoops->CmdCtrlBit & CLRPOSINTG_BIT),
												   BaseLoops->GselGains->Kp,
												   BaseLoops->f.PerClrEdge /* @@ AP */ );

		/* �T�[�{�I�������x���~�b�g���� */
		BaseLoops->SpdRefi = SvonSpeedLimit( BeComplete, BaseLoops, BaseLoops->SpdRefp );

		/* ���x���䉉�Z */
		BaseLoops->TrqRefi = BaseSpeedControl( BaseLoops,
											   BaseLoops->SpdFFCx,
											   BaseLoops->SpdFBCx,
											   BeComplete );
		/* �g���N�t�B���^ */
		BaseLoops->TrqRefo = BaseTorqueFilterForPSCtrl( BaseLoops,									  /* <S0B8> */
														BaseLoops->TrqRefi,
														BaseLoops->TrqFBCx,
														BaseLoops->TrqFFCx,
														BeComplete );

//		RippleCmpExe = FALSE;	/* ���x���b�v���⏞�@�\�̖��� */	/* <S14B> *//*<500>*/

		break;

	/*----------------------------------------------------------------------------------------------*/
	/* ���x���䃂�[�h & JOG���[�h & ���_�T�[�`���[�h												*/
	/*----------------------------------------------------------------------------------------------*/
	case CTRLMODE_SPD:	/* ���x���䃂�[�h */
	case CTRLMODE_ZSRCH:	/* @@ */
	case CTRLMODE_JOG:	/* 
						 * @@check JOG MODE
						 * switch�̒���JOG�ƌ��_���A���Ȃ����߁A�⏕�@�\���s������ٸ���o�Ȃ�
						 */
	/*----------------------------------------------------------------------------------------------*/
		/* ���x���䉉�Z */
		BaseLoops->TrqRefi = BaseSpeedControl( BaseLoops,
											   0,
											   BaseLoops->SpdFBCx,
											   BeComplete );
		/* �g���N�t�B���^ */
		BaseLoops->TrqRefo = BaseTorqueFilterForPSCtrl( BaseLoops,									  /* <S0B8> */
														BaseLoops->TrqRefi,
														BaseLoops->TrqFBCx,
														BaseLoops->TrqFFCx,
														BeComplete );
/*<S1FD>*/
#if 0
/* <S14B> Start */
	if( RippleCmp->VelRipplePhaseReady == TRUE)	/* ���Ɍ��o����(�ʑ����o����) */
	{
		if(RippleCmp->RippleCmpThresholdSel == CMD_SPD)
		{	/* ���x���b�v���⏞�@�\�̗L�����������l���� = �u�w�ߑ��x�v */
			if( RippleCmp->VelRippleCompSpd < BaseLoops->SpdRefo )
			{
				RippleCmpExe = TRUE;
			}
			else
			{
				RippleCmpExe = FALSE;
			}
		}
		else
		{/* ���x���b�v���⏞�@�\�̗L�����������l���� = �u�t�B�[�h�o�b�N���x�v */
			if( RippleCmp->VelRippleCompSpd < BaseLoops->SpdFbki )
			{
				RippleCmpExe = TRUE;
			}
			else
			{
				RippleCmpExe = FALSE;
			}
		}
	}
	else	/* ���Ɍ��o������(�ʑ����o������) */
	{
		RippleCmpExe = FALSE;
	}
/* <S14B> End */
#endif
		break;

	/*----------------------------------------------------------------------------------------------*/
	/* �g���N���䃂�[�h																				*/
	/*----------------------------------------------------------------------------------------------*/
	case CTRLMODE_TRQ:	/* �g���N���䃂�[�h *//* <S0B8> */
		/* �g���N���䃂�[�h�؂�ւ��V���b�N�y������ */
		BaseInitTrqCtrlVariables( BaseLoops,
								  CtrlLoopOut->IqRefMon,
								  &(BaseLoops->BaseCtrls->CtrlCmdMngr.SpdRefo) );
		BaseLoops->TrqRefo = BaseTorqueControl( BaseLoops, BeComplete );
#if 0 /* <S0B8> */
#if	SVFSEL_ADC_USE == 1
		/* <S051> */
		BaseLoops->TrqRefi = MlibLpfilter1( BaseLoops->TRefAd, BaseLoops->TrqFil.A.Ktrflpf /* <S083> Klpf */, BaseLoops->TrqRefi );
		BaseLoops->TrqRefo = BaseLoops->TrqRefi - (BaseLoops->BaseCtrls)->CtrlCmdMngr.OverTrq;
#else
		BaseLoops->TrqRefo = /* BaseLoops->TrqRefi */
				(BaseLoops->BaseCtrls)->CtrlCmdMngr.TrqRefo -		/* @@ */
				(BaseLoops->BaseCtrls)->CtrlCmdMngr.OverTrq;

#endif
#endif /* <S0B8> */

/*<S1FD>*/
#if 0
/* <S14B> Start */
	/* ���x���b�v���⏞�@�\�̗L�����������l�����m�F */
	if( RippleCmp->VelRipplePhaseReady == TRUE)	/* ���Ɍ��o����(�ʑ����o����) */
	{
		if( RippleCmp->VelRippleCompSpd < BaseLoops->SpdFbki )
		{
			RippleCmpExe = TRUE;
		}
		else
		{
			RippleCmpExe = FALSE;
		}
	}
	else	/* ���Ɍ��o������(�ʑ����o������) */
	{
		RippleCmpExe = FALSE;
	}
/* <S14B> End */
#endif
		break;

	default:
		break;
	}

	/* ���[�^���X�e�X�g���[�h�������J���f�����Z *//* <S1B7> */
	if( BaseLoops->MotorLessTest->MotorLessTestMode == TRUE )
	{
		ApxMotorLessTestModeMechaModel( BaseLoops->TrqRefoLimit, BeComplete, BaseLoops->MotorLessTest );
	}

/* <S14B> Start */
	/*--------------------------------------------------------------------------------------------------*/
	/*		���x���b�v���⏞	--> BaseLoops->TrqRefo														*/
	/*--------------------------------------------------------------------------------------------------*/
	/* ���b�v���␳�O�g���N�w��(�g���[�X�E�A�i���O���j�^�p�f�[�^)�擾 */
	RippleCmp->TrqRefBeforeComp = BaseLoops->TrqRefo;

	if(RippleCmp->RplCmpPrm.RplCmpUse == TRUE)	/* ���x���b�v���⏞�@�\ �L�� *//*<S1FD>*/
	{
//		if(  RippleCmpExe == TRUE )	/* ���x���b�v���⏞�@�\�̗L�����������l���� ���� *//*<S1FD>*/
		{
#if 0	/* ���x���b�v���⏞ �@�B�p�␳����(�����p�Ƃ��ăR�����g�A�E�g) */
			/* �@�B�p�Z�o �i�G���R�[�_�r�b�g��/360���j */
			lwk = (LONG)((LONG)((BaseLoops->MencV->MotPosX[0]) - (BaseLoops->MencV->MotCposX)) >> BaseLoops->MencV->MposSftR);
			BaseLoops->MencV->MotAngle = ( lwk >= 0 ) ? lwk : lwk + BaseLoops->MencV->PulseNo;

			/* [�G���R�[�_�r�b�g��/360��] ���@[65536/360��]*/
			lwk = (BaseLoops->MencV->EncBitNo > 16) ?  (lwk >> ( BaseLoops->MencV->EncBitNo - 16 ))
													  :(lwk << ( 16 - BaseLoops->MencV->EncBitNo ));
#endif
			/* ���x���b�v���⏞�@�\�̗L�������`�F�b�N */
			RippleCompChkThresholdVal( 	&(BaseLoops->RippleCmp),
										BaseLoops->CtrlMode,
										BaseLoops->SpdRefo,
										BaseLoops->SpdFbki,
										TuneLessGetTuneLessSts(&((BaseLoops->BaseCtrls)->TuneLessCtrl)));
			/* ���x���b�v���⏞ */
			BaseLoops->TrqRefoComp = RippleCompensation( &(BaseLoops->RippleCmp),
														 0,//lwk,/*<S1FD>*/
														 BaseLoops->MencV->MotPhaseC,
														 BeComplete);

			//RippleCmp->AmonTrqComp1 = BaseLoops->TrqRefoComp;/*<S1FD>*/
			BaseLoops->TrqRefoComp  = BaseLoops->TrqRefoComp + BaseLoops->TrqRefo;
		}
//		else	/* ���x���b�v���⏞�@�\�̗L�����������l���� �s���� */
//		{
//			BaseLoops->TrqRefoComp = RippleCmp->AmonTrqComp1;
//			BaseLoops->TrqRefoComp =RippleCmp->TrqRefBeforeComp;	/* ���x���b�v���⏞�@�\ ���� */
//		}
	}
	else
	{
		BaseLoops->TrqRefoComp = BaseLoops->TrqRefo;	/* ���x���b�v���⏞�@�\ ���� */
	}
/* <S14B> End */

	/*--------------------------------------------------------------------------------------------------*/
	/*		<S0CD> ���C���ٕ⏞												*/
	/*--------------------------------------------------------------------------------------------------*/
//#if( FRICTRQ_COMP != 0 )
   BaseLoops->BaseCtrls->TrqMotFricComp = LpxMotorFricComp( BaseLoops->BaseCtrls->FrictionM.MotFricEnable, BaseLoops );
   BaseLoops->TrqRefoComp += BaseLoops->BaseCtrls->TrqMotFricComp;
//#endif
	/*--------------------------------------------------------------------------------------------------*/
	/*		�g���N�w��(CPU)	--> q���d���w��(�ʃv���O����)												*/
	/*--------------------------------------------------------------------------------------------------*/
	/* TrqRefo --> AinK.IqRef */
	KaiOutputTrqRef( BaseLoops, BaseLoops->TrqRefoComp, 0, BaseLoops->DisTrqRef3 );

	/*--------------------------------------------------------------------------------------------------*/
	/*		ScanB���X�V����																			*/
	/*--------------------------------------------------------------------------------------------------*/
	/* ���σg���N�w�ߍX�V(For ScanB) */
#if (FLOAT_USE==TRUE)
	BaseLoops->TrqRefo_a = ( BaseLoops->TrqRefo + BaseLoops->TrqRefo_l ) /2.0f;
#else
	BaseLoops->TrqRefo_a = ( BaseLoops->TrqRefo + BaseLoops->TrqRefo_l ) >> 1;
#endif /* FLOAT_USE */
	BaseLoops->TrqRefo_l = BaseLoops->TrqRefo;

}



/****************************************************************************************************/
/*																									*/
/*		���[�^�ʒu�����x�e�a���Z																	*/
/*																									*/
/****************************************************************************************************/
static	void	LpxPosSpdFeedbackCal( BASE_LOOPCTRLS *BaseLoops )
{
	EHVOBS_CTRL		*EhVobsCtrl;

	EhVobsCtrl = &(BaseLoops->BaseCtrls->EhVobsCtrl);

/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu�e�a�X�V																				*/
/*--------------------------------------------------------------------------------------------------*/
	/* �t���N���Ή� 2013.08.20 Y.Oka @@CHECK */
//	AoutV.dPosFbki = (Kprm.f.FencUse)? AinK.dFencPos : AinK.dMotPos;
//	BaseLoops->dPosFbki = BaseLoops->dMotPos;/* <S04D> */
	BaseLoops->dPosFbki = (BaseLoops->Bprm->FencUse)? BaseLoops->dFencPos : BaseLoops->dMotPos;/* <S04D> */

/*--------------------------------------------------------------------------------------------------*/
/*		���x�e�a�X�V																				*/
/*--------------------------------------------------------------------------------------------------*/
	/* �ʑ��⏞���x�I�u�U�[�o�O���x���� */
	/* �t���N���Ή� 2013.08.20 Y.Oka @@CHECK */
//	AoutV.SpdFbki = ( Iprm.f.PsFulcSelect ) ? AinK.FencSpd : AinK.MotSpd;
//	BaseLoops->SpdFbki = BaseLoops->MotSpd;/* <S04D> */
	BaseLoops->SpdFbki = ( BaseLoops->Bprm->PsFulcSelect ) ? BaseLoops->FencSpd : BaseLoops->MotSpd;

	/* �ʑ��⏞���x�I�u�U�[�o���Z */
	if( TRUE == EhVobsGetObserverUse(EhVobsCtrl) )
	{
#if( CSW_SVCF_EHVOBS_USE != 0 )
		/* FB���x */
		BaseLoops->SpdObsFbki = EhSpeedObserver( EhVobsCtrl,
												 BaseLoops->SpdFbki,
												 BaseLoops->TrqRefi );
#else
		/* FB���x */
		BaseLoops->SpdObsFbki = BaseLoops->SpdFbki;
#endif
	}
	else
	{
		/* FB���x */
		BaseLoops->SpdObsFbki = BaseLoops->SpdFbki;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		���ϑ��x�X�V(For ScanB)																		*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	BaseLoops->MotSpd_a = ( BaseLoops->MotSpd + BaseLoops->MotSpd_l ) / 2.0f;
#else
	BaseLoops->MotSpd_a = ( BaseLoops->MotSpd + BaseLoops->MotSpd_l ) >> 1;
#endif /* FLOAT_USE */
	BaseLoops->MotSpd_l = BaseLoops->MotSpd;

/*	Full Closed Encoder Position																	*//* <S04D> */
#if (FLOAT_USE==TRUE)
	BaseLoops->FencSpd_a = ( BaseLoops->FencSpd + BaseLoops->FencSpd_l ) / 2.0f
#else
	BaseLoops->FencSpd_a = ( BaseLoops->FencSpd + BaseLoops->FencSpd_l ) >> 1;
#endif /* FLOAT_USE */
	BaseLoops->FencSpd_l = BaseLoops->FencSpd;

	
#if (FLOAT_USE==TRUE)
	BaseLoops->SpdObsFbki_a = ( BaseLoops->SpdObsFbki + BaseLoops->SpdObsFbki_l ) /2.0f;
#else
	BaseLoops->SpdObsFbki_a = ( BaseLoops->SpdObsFbki + BaseLoops->SpdObsFbki_l ) >> 1;
#endif /* FLOAT_USE */

	BaseLoops->SpdObsFbki_l = BaseLoops->SpdObsFbki;

	return;
}



/****************************************************************************************************/
/*																									*/
/*		���[�^���x���o�p�ړ����σt�B���^����	<V259>	<S02F>										*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : ���x��������̍������ɂ�鑬�x���o����\�̒ቺ�̑΍�Ƃ��Č��o���x���ړ����σt�B���^	*/
/*			�ɒʂ��B�������A�ڑ����ꂽ�G���R�[�_����\�ɂ���Ĉړ����ω񐔂�ύX����B				*/
/*			�ړ����ω񐔂̃I�����C���ł̕ύX�́ABpiChangeSpdDetMaFil( )�ɂčs�����ƁB				*/
/*																									*/
/****************************************************************************************************/
static KSGAIN	LpxSpdDetMaFilter( MOTSPDMAFIL *SpdMafil, KSGAIN MotSpd )/*<S00A>*/
{
#if (FLOAT_USE==TRUE)
	float	lwk0;
	float	output;
	float	idx_max;
#else
	LONG	lwk0;
	LONG	output;
	LONG	idx_max;
#endif /* FLOAT_USE */

	idx_max = 1 << SpdMafil->manumBit;

	/* �o�b�t�@���f�[�^�擾 */
	lwk0 = SpdMafil->ksub * SpdMafil->mabuf[SpdMafil->idx];
	/* ����f�[�^�i�[ */
	SpdMafil->mabuf[SpdMafil->idx] = MotSpd;

	SpdMafil->idx++;
	if( SpdMafil->idx >= idx_max )
	{
		/* �C���f�b�N�X�����Z�b�g */
		SpdMafil->idx = 0;
		SpdMafil->ksub = 1;
	}
	/* �o�b�t�@�ώZ�l�X�V */
	SpdMafil->mabufSumx = SpdMafil->mabufSumx - lwk0 + MotSpd;
	/* ����o�� */
	output = ( SpdMafil->mabufSumx >> SpdMafil->manumBit );

	return( output );
}



/****************************************************************************************************/
/*																									*/
/*		Kernel Input Service for ScanA																*/
/*																									*/
/****************************************************************************************************/
void	KpxInputScanA( BASE_LOOPCTRLS *BaseLoops )/*<S0C5>*/
{
#if (FLOAT_USE==TRUE)
	float			lwk0;									/* long Work							*/
	float			MotSpd;									/* Motor Speed							*/
	float			dMotPos;
	float			PosTrqLmt;
	float			NegTrqLmt;
#else	
	LONG			lwk0;									/* long Work							*/
	LONG			MotSpd;									/* Motor Speed							*/
	LONG			dMotPos;
	LONG			PosTrqLmt;
	LONG			NegTrqLmt;
	LONG			TuneLessMode;
	BOOL			TuningLessVComp;
	BOOL			VcompSts;
#endif /* FLOAT_USE */
	MENCV			*MencV;
#if	(CSW_SVCF_FULC_MODE == CSW_FULC_BASIC_SUPPORT)/* <S04D> */
	MENCV			*FencV;
#endif /* (CSW_SVCF_FULC_MODE == CSW_FULC_BASIC_SUPPORT) */
	BPRMDAT			*Bprm;
	BASE_CTRL_HNDL	*BaseCtrls;		/* <S004> */
	ASIC_HW_IF_STR	*pAsicHwReg;	/* <S004> */
	ALMMAN_HANDLE	*AlmManager;	/* <S018> */
	VFBTIMBASE		*Vfbtimbase;	/* <S067> */

	MencV	= BaseLoops->MencV;
	FencV	= BaseLoops->FencV;
	Bprm	= BaseLoops->Bprm;
	BaseCtrls = BaseLoops->BaseCtrls;		/* <S004> */
	pAsicHwReg = BaseLoops->pAsicHwReg;		/* <S004> */
	AlmManager = BaseLoops->AlmManager;		/* <S018> */
	Vfbtimbase = &(BaseLoops->Vfbtimbase);	/* <S067> */

/*--------------------------------------------------------------------------------------------------*/
/*		�G���R�[�_�ʒu�f�[�^�Ǎ���, �����p���X�o�͏���												*/
/*--------------------------------------------------------------------------------------------------*/
	/* �ݒ肳��Ă���G���R�[�_�ʒu�f�[�^�Ǎ��݊֐������s *//* <S1B7> */
//	SencReadPosition( MencV, pAsicHwReg, AlmManager, BaseCtrls->BeComplete );						  /* <S018> *//* <S0C5> */
	BaseLoops->SencReadPosition( MencV, pAsicHwReg, AlmManager, BaseCtrls->BeComplete, (void*)BaseLoops );
	dMotPos = ( MencV->RcvPosX[0] - MencV->RcvPosX[1] ) >> MencV->MposSftR;

/*--------------------------------------------------------------------------------------------------*/
/*		�ω������^�C�}�ɂ�鑬�x�␳																*/
/*--------------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------*/
	/*	�����x�����ɐ��K��[pulse/ScanA]��[2^24/OvrSpd]												*/
	/*----------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	MotSpd = dMotPos * (Bprm->KmotspdA);
#else
	MotSpd = MlibMulgain( dMotPos, Bprm->KmotspdA );					/* Motor Speed[2^24/OvrSpd]	*/
#endif /* FLOAT_USE */

	/*----------------------------------------------------------------------------------------------*/
	/*		���x�␳�p������Ԏ擾	<S067>															*/
	/*----------------------------------------------------------------------------------------------*/
	TuneLessMode    = BaseLoops->BaseCtrls->TuneLessCtrl.conf.TuningLessMode;
	TuningLessVComp = BaseLoops->BaseCtrls->TuneLessCtrl.conf.TuningLessVComp;
	VcompSts = ( (TuneLessMode & TLMODE_SPDDETCHNG) && (TuningLessVComp == FALSE) ) ? FALSE : TRUE;
	/*----------------------------------------------------------------------------------------------*/
	/*		���x�␳�L������		<S067>															*/
	/*----------------------------------------------------------------------------------------------*/
	if( MencV->SpdCmpEnable == FALSE )
	{
		MencV->MotSpd = TimeBasesMotSpdCalc( Vfbtimbase, MencV, dMotPos, MotSpd, VcompSts, FALSE );
		MencV->SpdCmpEnable = TRUE;
	}
	else
	{
		MencV->MotSpd = TimeBasesMotSpdCalc( Vfbtimbase, MencV, dMotPos, MotSpd, VcompSts, TRUE );
	}

	/*----------------------------------------------------------------------------------------------*/
	/*	���x���o�ړ����σt�B���^����		<V110>													*/
	/*----------------------------------------------------------------------------------------------*/
	if( BaseLoops->MotSpdMafil->manumBit == 0 )
	{ /* �G���R�[�_17bit�ȏ� */
		MotSpd = MencV->MotSpd;
	}
	else
	{ /* �G���R�[�_16bit�ȉ� */
		MotSpd = LpxSpdDetMaFilter( BaseLoops->MotSpdMafil, MencV->MotSpd );
	}

/*--------------------------------------------------------------------------------------------------*/
/*		Calculate Motor Position and Speed															*/
/*--------------------------------------------------------------------------------------------------*/
	/* Cal. Delta Position */
	lwk0 = MencV->MotPos - BaseLoops->LastMotPos;
	/* Update Motor Position */
	BaseLoops->LastMotPos = MencV->MotPos;

//<S00E>���ʉ�#if CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE				/* M2,M3,etc */
//	lwk0 = Bprm->DirSign * lwk0;
//	BaseLoops->MotPos += lwk0;							/* Update Motor Position		*/
//	BaseLoops->dMotPos = lwk0;							/* Update Motor Delta Position	*/
//	BaseLoops->MotSpd  = MotSpd;						/* Motor Speed for ScanB		*/
//<S00E>���ʉ�#else													/* Analog & Pulse */
	{ 
	CTRL_CMD_MNG *pcmd = &(BaseLoops->BaseCtrls->CtrlCmdMngr);	
	UINT PerClrEdge = ( pcmd->PerClrEdge == TRUE) && ( BaseLoops->f.PastInputClrEdge == FALSE );
	
	if( Bprm->RvsDir )									/* �t��]���[�h							*/
	{
		BaseLoops->MotPos -=  lwk0;						/* Update Motor Position				*/
		if(	PerClrEdge && (Bprm->FencUse == FALSE) )
		{
			BaseLoops->dMotPos = BaseLoops->MotPos - pcmd->ClrFbPos;		//<V584>
		}
		else
		{
			BaseLoops->dMotPos = -lwk0;
		}
		BaseLoops->MotSpd  = -MotSpd;					/* Motor Speed for ScanB [2^24/OvrSpd]	*/
	}
	else												/* ����]���[�h							*/
	{
		BaseLoops->MotPos += lwk0;						/* Update Motor Position				*/
		if(	PerClrEdge && (Bprm->FencUse == FALSE) )
		{
			BaseLoops->dMotPos = BaseLoops->MotPos - pcmd->ClrFbPos;
		}
		else
		{
			BaseLoops->dMotPos = lwk0;
		}

		BaseLoops->MotSpd  = MotSpd;					/* Motor Speed for ScanB [2^24/OvrSpd]	*/
	}
	/*----------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		Input Full Closed Encoder Position															*/
/*--------------------------------------------------------------------------------------------------*//* <S04D> */
#if	(CSW_SVCF_FULC_MODE == CSW_FULC_BASIC_SUPPORT)
	if( Bprm->FencUse )										/* �t���N�G���R�[�_�g�p��				*/
	{
		if(Bprm->RvsDir )									/* �t��]���[�h							*/
		{
//			lwk0 = -FencReadPosition( FencV, AlmManager, BaseCtrls->BeComplete );	/* <S0E0>:AlmManager�ǉ� */
			lwk0 = -BaseLoops->FencReadPosition( FencV, AlmManager, BaseCtrls->BeComplete, (void*)BaseLoops );	/*<S1B7>*/
															/* �t���N�G���R�[�_�ʒu�f�[�^�Ǎ���		*/
			BaseLoops->FencSpd = -FencV->MotSpd;			/* Load Speed [2^24/OvrSpd]				*/
		}
		else												/* ����]���[�h							*/
		{
//			lwk0 = FencReadPosition( FencV, AlmManager, BaseCtrls->BeComplete );	/* <S0E0>:AlmManager�ǉ� */
			lwk0 = BaseLoops->FencReadPosition( FencV, AlmManager, BaseCtrls->BeComplete, (void*)BaseLoops );	/*<S1B7>*/
															/* �t���N�G���R�[�_�ʒu�f�[�^�Ǎ���		*/
			BaseLoops->FencSpd = FencV->MotSpd;				/* Load Speed [2^24/OvrSpd]				*/
		}
		/*------------------------------------------------------------------------------------------*/
		BaseLoops->FencPos += lwk0;							/* Update Fencoder Position				*/
		if(	PerClrEdge )
		{
			BaseLoops->dFencPos = BaseLoops->FencPos - pcmd->ClrFbPos;
		}
		else
		{
			BaseLoops->dFencPos = lwk0;
		}
		/*------------------------------------------------------------------------------------------*/
	}
#endif/* 	(CSW_SVCF_FULC_MODE == CSW_FULC_BASIC_SUPPORT)	*/
	BaseLoops->f.PastInputClrEdge = pcmd->PerClrEdge;		/* �΍��G�b�W�N���A���͑O��l�ێ�		*/
	BaseLoops->f.PerClrEdge = PerClrEdge;					/* Perr Clear Edge Command Input */
	/* 
	 * @@ AP
	 */
	}

//#endif	<S00E>���ʉ�

/*--------------------------------------------------------------------------------------------------*/
/*		Torque Reference after Torque Limit for Observer											*/
/*--------------------------------------------------------------------------------------------------*/
	PosTrqLmt = (BaseLoops->BaseCtrls)->TrqLimitData.var.PosTrqLmtOut;
	NegTrqLmt = (BaseLoops->BaseCtrls)->TrqLimitData.var.NegTrqLmtOut;
	if( BaseLoops->TrqRefo > PosTrqLmt )
	{
		BaseLoops->TrqRefoLimit = PosTrqLmt;
	}
	else if( BaseLoops->TrqRefo < NegTrqLmt )
	{
		BaseLoops->TrqRefoLimit = NegTrqLmt;
	}
	else
	{
		BaseLoops->TrqRefoLimit = BaseLoops->TrqRefo;
	}

	return;/*<S0C5>*/
}



/****************************************************************************
  Carrer frequency change Module
*****************************************************************************/
static void	BaseLoopChangeCarrerFreq( BASE_LOOPCTRLS *BaseLoops,
									  CTRL_LOOP_OUT *CtrlLoopOut,
									  ASICS *SvAsicRegs )
{
	CARRCHNG	*CarrerChange;
#if (FLOAT_USE==TRUE)
	float		AbsMotSpd;		/* FB���x��Βl */
	float		MotIqref2;		/* q���d���w��^2 */
	float		MotIqfb;		/* q���d�����o�l */
	float		MotIqfb2;		/* q���d�����o�l^2 */
	float		MotIdfb;		/* d���d�����o�l */
	float		MotIdfb2;		/* d���d�����o�l^2 */
	float		MotIfb2;		/* (q���d�����o�l^2) + (d���d�����o�l^2) */
	float		MotIq;			/* Max(q���d���w��^2, |q���d��FB�x�N�g��|^2) */
#else
	LONG		AbsMotSpd;		/* FB���x��Βl */
	LONG		MotIqref2;		/* q���d���w��^2 */
	LONG		MotIqfb;		/* q���d�����o�l */
	LONG		MotIqfb2;		/* q���d�����o�l^2 */
	LONG		MotIdfb;		/* d���d�����o�l */
	LONG		MotIdfb2;		/* d���d�����o�l^2 */
	LONG		MotIfb2;		/* (q���d�����o�l^2) + (d���d�����o�l^2) */
	LONG		MotIq;			/* Max(q���d���w��^2, |q���d��FB�x�N�g��|^2) */
#endif /* FLOAT_USE */

	CarrerChange = &(BaseLoops->CarrerChange);

/*--------------------------------------------------------------------------------------------------*/
/*		�L�����A���g���؂�ւ�����																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( CarrerChange->conf.CarrChange )					/* �L�����A�؂�ւ��t���OON							*/
	{
		/*---------- ��Βl�v�Z���� ----------------------------------------------------------------*/
		/* �d���w�ߐ�Βl�v�Z	*/
		MotIqref2 = (LONG)BaseLoops->IqRef * (LONG)BaseLoops->IqRef;

		/* �d��FB��Βl�v�Z		*/
//		MotIqfb = KPX_INPUT_IQMON( );
		MotIqfb = CtrlLoopOut->IqRefMon;
		MotIqfb2 = (LONG)MotIqfb * (LONG)MotIqfb;
//		MotIdfb = KPX_INPUT_IDMON( );
		MotIdfb = CtrlLoopOut->IdRefMon;
		MotIdfb2 = (LONG)MotIdfb * (LONG)MotIdfb;
		MotIfb2 = MotIqfb2 + MotIdfb2;
		/* �w�߂�FB�ő傫���ق��̓d��臒l���f���� */
		if( MotIqref2 > MotIfb2 )
		{
			MotIq = MotIqref2;
		}
		else
		{
			MotIq = MotIfb2;
		}

		/* ���[�^���x��Βl�v�Z */
		if( BaseLoops->MotSpd < 0 )
		{
			AbsMotSpd = -BaseLoops->MotSpd;
		}
		else
		{
			AbsMotSpd = BaseLoops->MotSpd;
		}
		/*--------- �L�����A���g���؂�ւ����f & �ʃv���O�����������ݏ��� --------------------------*/
		if( CarrerChange->var.CarrState )
		{ /* ���ݍ��L�����A�ݒ� */
			/*--------- 臒lB�����傫���Ȃ�����L�����A���g����������(Or) ------------------------*/
			if( ( MotIq > CarrerChange->conf.IqLimitB ) || ( AbsMotSpd > CarrerChange->conf.VelLimitB ) )
			{
				CarrerChange->var.CarrState = FALSE;
				/* ����IF�쐬��A�h���X�ύX�� */
				/* ����IF�ւ̃A�E�g�v�b�g�͐�p�����Ƃ���B�� */
				SHalDrv_SetCarrerFreq( SvAsicRegs->AsicMicroReg, CarrerChange->conf.NormCarrCnt );
			}
		}
		else
		{ /* ���ݒʏ�L�����A�ݒ� */
			/*--------- 臒lA�����������Ȃ�����L�����A���g�����グ��(And) ------------------------*/
			if( ( MotIq < CarrerChange->conf.IqLimitA ) && ( AbsMotSpd < CarrerChange->conf.VelLimitA ) )
			{
				CarrerChange->var.CarrState = TRUE;
				/* ����IF�쐬��A�h���X�ύX�� */
				/* ����IF�ւ̃A�E�g�v�b�g�͐�p�����Ƃ���B�� */
				SHalDrv_SetCarrerFreq( SvAsicRegs->AsicMicroReg, CarrerChange->conf.HighCarrCnt );
			}
		}
	}
	return;
}


/****************************************************************************
  Phase compensation value calculation function
*****************************************************************************/
#define SVFSEL_PCMP_IDREF			0
/*--------------------------------------------------------------------------------------------------*/
/*		�ʑ��⏞ : Pcmp = Pcmpd(�x��⏞) �{ Pcmps(���x�⏞=Kpcmps*MotSpd)							*/
/*--------------------------------------------------------------------------------------------------*/
/*	����1�F	�]������(SVFSEL_PCMP_IDREF != 1)�ł́A�񐶎��̑��x�⏞���E�����߂ɂȂ�A				*/
/*			OV�����������₷���Ȃ�B																*/
/*	����2�F	�g���N�w�߂̕���(�d���^��)�ŁA���x�⏞(Pcmps)�̕�����؂�ւ�������ł́A				*/
/*			�����׎�(�g���N�w�߂��[���ߖT)�ɁA���x�⏞�������ɐU��邽�ߖ�肪����B				*/
/*�@����3�F	���j�A���[�^�̏ꍇ�́ALpxUpdateLMPhase( )�ňʑ��X�V�B									*/
/*--------------------------------------------------------------------------------------------------*/
static void	BaseLoopMakePhaseCompValue( BASE_LOOPCTRLS *BaseLoops,
										CTRL_LOOP_OUT *CtrlLoopOut,
										MENCV *MencV,
										BOOL WfActive )
{
	BPRMDAT		*Bprm;
	PHASECMP	*PhaseCmp;
	KSGAIN		MotSpd;			/*<S00A>*/

	Bprm = BaseLoops->Bprm;
	PhaseCmp = &(BaseLoops->PhaseComp);
	MotSpd = BaseLoops->MotSpd * Bprm->DirSign;				/* Motor Speed [2^24/OvrSpd]			*/

/*--------------------------------------------------------------------------------------------------*/
/*		Calculate Pcmpd[65536/360deg]																*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	/* �ʑ��⏞(�x��1) */
	PhaseCmp->var.Pcmpd = (MotSpd * PhaseCmp->conf.Kpcmpd);
	/* �ʑ��⏞(�x��2) */
	PhaseCmp->var.Pcmpd2 = ( MotSpd * PhaseCmp->conf.Kpcmpd2 );
	/* �ʑ���ԗʂ̌v�Z */
	PhaseCmp->var.PcmpdItplt = ( MotSpd * PhaseCmp->conf.KpcmpdItplt );
#else
	/* �ʑ��⏞(�x��1) */
	PhaseCmp->var.Pcmpd = (SHORT)MlibMulgainNolim( MotSpd, PhaseCmp->conf.Kpcmpd );
	/* �ʑ��⏞(�x��2) */
	PhaseCmp->var.Pcmpd2 = (SHORT)MlibMulgainNolim( MotSpd, PhaseCmp->conf.Kpcmpd2 );
	/* �ʑ���ԗʂ̌v�Z */
	PhaseCmp->var.PcmpdItplt = (SHORT)MlibMulgainNolim( MotSpd, PhaseCmp->conf.KpcmpdItplt );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		Calculate Pcmps[65536/360deg]																*/
/*--------------------------------------------------------------------------------------------------*/
	/* �ʑ��⏞1(���x) */
#if (FLOAT_USE==TRUE)
	if( MotSpd > PhaseCmp->conf.PcmpSpd1 )
	{
		PhaseCmp->var.Pcmps1 = ( (MotSpd - PhaseCmp->conf.PcmpSpd1) *PhaseCmp->conf.Kpcmpsp1 );
	}
	else if( MotSpd < -PhaseCmp->conf.PcmpSpd1 )
	{
		PhaseCmp->var.Pcmps1 = ( (MotSpd + PhaseCmp->conf.PcmpSpd1) * PhaseCmp->conf.Kpcmpsp1 );
	}
#else
	if( MotSpd > PhaseCmp->conf.PcmpSpd1 )
	{
		PhaseCmp->var.Pcmps1 = 
			(SHORT)MlibMulgainNolim( (MotSpd - PhaseCmp->conf.PcmpSpd1), PhaseCmp->conf.Kpcmpsp1 );
	}
	else if( MotSpd < -PhaseCmp->conf.PcmpSpd1 )
	{
		PhaseCmp->var.Pcmps1 = 
			(SHORT)MlibMulgainNolim( (MotSpd + PhaseCmp->conf.PcmpSpd1), PhaseCmp->conf.Kpcmpsp1 );
	}
#endif /* FLOAT_USE */
	else
	{
		PhaseCmp->var.Pcmps1 = 0;
	}
	/*----------------------------------------------------------------------------------------------*/
#if 0 /* SGDV�@�\���� */
	/* �ʑ��⏞2(���x) */
#if (FLOAT_USE==TRUE)
	if( MotSpd > PhaseCmp->conf.PcmpSpd2 )
	{
		PhaseCmp->var.Pcmps2 = 
			(SHORT)( (float)(MotSpd - PhaseCmp->conf.PcmpSpd2) * PhaseCmp->conf.Kpcmpsp2 );
	}
	else if( MotSpd < -PhaseCmp->conf.PcmpSpd2 )
	{
		PhaseCmp->var.Pcmps2 = 	(SHORT)( (float)(MotSpd + PhaseCmp->conf.PcmpSpd2)  * PhaseCmp->conf.Kpcmpsp2);
			
#else
	if( MotSpd > PhaseCmp->conf.PcmpSpd2 )
	{
		PhaseCmp->var.Pcmps2 = 
			(SHORT)MlibMulgainNolim( (MotSpd - PhaseCmp->conf.PcmpSpd2), PhaseCmp->conf.Kpcmpsp2 );
	}
	else if( MotSpd < -PhaseCmp->conf.PcmpSpd2 )
	{
		PhaseCmp->var.Pcmps2 = 
			(SHORT)MlibMulgainNolim( (MotSpd + PhaseCmp->conf.PcmpSpd2), PhaseCmp->conf.Kpcmpsp2 );
	}
#endif /* FLOAT_USE */
	else
	{
		PhaseCmp->var.Pcmps2 = 0;
	}
#endif
	/*----------------------------------------------------------------------------------------------*/
	/* �ʑ��⏞���v(���x) [65536/360deg] */
	PhaseCmp->var.Pcmps = PhaseCmp->var.Pcmps1 + PhaseCmp->var.Pcmps2;


	if( SVFSEL_PCMP_IDREF != 1 )
	{ /* Pcmps by MotPhase */
//		if( ((Prm->dq_sw & 0x0800) == 0 )
//			|| (( Prm.flg_wf & 0x0001 ) == 0) )
		if( WfActive == TRUE )
		{ /* �d���t�B�[�h�o�b�N������ߊE���g�p���A���x�⏞�͂��Ȃ� */
//			MencV->MotPhaseX = MencV->MotPhase + PhaseCmp->var.Pcmpd;
			MencV->MotPhaseX = MencV->MotPhase;
		}
		else
		{
			MencV->MotPhaseX = MencV->MotPhase + PhaseCmp->var.Pcmpd + PhaseCmp->var.Pcmps;
		}
	}
	else if( SVFSEL_PCMP_IDREF == 1 )
	{ /* Pcmps by IdRef/IqRef */
		MencV->MotPhaseX = MencV->MotPhase + PhaseCmp->var.Pcmpd;
	}

	MencV->MotPhaseC = MencV->MotPhase + PhaseCmp->var.Pcmpd2;

	return;
}



/****************************************************************************************************/
/*																									*/
/*		Kernel Output Service for ScanA																*/
/*																									*/
/****************************************************************************************************/
ALMID_T	KpxOutputScanA( BASE_LOOPCTRLS *BaseLoops, CTRL_LOOP_OUT *CtrlLoopOut, ASICS *SvAsicRegs, PRMDATA *Prm )
{
	MENCV			*MencV;
	USHORT			WfActive;
	ALMID_T			errorID;

	MencV = BaseLoops->MencV;
	errorID = NO_ALARM;

	/* Change Carrer Frequency */
	BaseLoopChangeCarrerFreq( BaseLoops, CtrlLoopOut, SvAsicRegs );

	/* Make Phase Compensetion Value */
	if( ((Prm->dq_sw & 0x0800) == 0 ) || (( Prm->MencP.flg_wf & 0x0001 ) == 0) )
	{
		WfActive = FALSE;
	}
	else
	{
		WfActive = TRUE;
	}
	BaseLoopMakePhaseCompValue( BaseLoops, CtrlLoopOut, MencV, WfActive );

	return errorID;
}




/****************************************************************************************************/
/*																									*/
/*		Initialize function for ScanA																*/
/*																									*/
/****************************************************************************************************/
void SysScanAInit(	BASE_LOOPCTRLS *BaseLoops,
					MENCV			*MencV,
					MENCV			*FencV,	/* <S04D> */
					BPRMDAT			*Bprm,
					MOTSPDMAFIL		*MotSpdMafil,		/* ���[�^���x���o�p�ړ����ω��Z�p�ϐ�		*/
					BASE_CTRL_HNDL 	*BaseCtrls,
					GSELGAINS		*GselGains,
					ASIC_HW_IF_STR	*pAsicHwReg,
					ALMMAN_HANDLE 	*AlmManager,														/* <S018> */
					MOTLESSTST 		*MotorLessTest)														/* <S1B7> */

{
	BaseLoops->MencV = MencV;
	BaseLoops->FencV = FencV;	/* <S04D> */
	BaseLoops->Bprm = Bprm;
	BaseLoops->MotSpdMafil = MotSpdMafil;
	BaseLoops->BaseCtrls = BaseCtrls;
	BaseLoops->GselGains = GselGains;
	BaseLoops->pAsicHwReg = pAsicHwReg;																	/* <S004> */
	BaseLoops->AlmManager = AlmManager;																	/* <S018> */
	BaseLoops->MotorLessTest = MotorLessTest;															/* <S1B7> */

	/* �L�����A���g���؂�ւ��p�ϐ������� */
	MlibResetLongMemory( &(BaseLoops->CarrerChange.var), sizeof(BaseLoops->CarrerChange.var)/4 );
	/* �ʑ��⏞�l���Z�p�ϐ������� */
	MlibResetLongMemory( &(BaseLoops->PhaseComp.var), sizeof(BaseLoops->PhaseComp.var)/4 );
}



/****************************************************************************************************/
/*																									*/
/*		ScanA Cyclic data input function															*/
/*																									*/
/****************************************************************************************************/
//void SysInputScanAProcessData( BASE_LOOPCTRLS *BaseLoops, BASE_CTRL_HNDL *pBaseCtrl, BASE_CTRL_OUT *BaseCtrlOut )
void SysInputScanAProcessData( BASE_LOOPCTRLS *BaseLoops, BASE_CTRL_HNDL *pBaseCtrl,
												BASE_CTRL_OUT *BaseCtrlOut, CMD_CTRLBITS *CmdCtrl )	  /* <S050> */
{
	BaseLoops->LastCtrlMode = BaseLoops->CtrlMode;							/* <S007> */
	BaseLoops->CtrlMode = (USHORT)pBaseCtrl->CtrlCmdMngr.CtrlMode;
	BaseLoops->ZCtrlMode = (USHORT)pBaseCtrl->CtrlModeSet.ZctrlMode.zm;

	BaseLoops->CmdCtrlBit = CmdCtrl->CmdCtrlBit; /* <S050> */

//	BaseLoops->dPcmda = pBaseCtrl->CtrlCmdMngr.dPosRefo;
//	pBaseCtrl->CtrlCmdMngr.dPosRefo = 0;
//	LpxSvPositionManager( BaseLoops, &(pBaseCtrl->PosManager) );	/* <S14C> */
//	BaseLoops->SpdRefi = pBaseCtrl->CtrlCmdMngr.SpdRefo;
//	BaseLoops->TrqRefi = pBaseCtrl->CtrlCmdMngr.TrqRefo;

	BaseLoops->SpdFFCx = pBaseCtrl->CtrlCmdMngr.SpdFFCx;
	BaseLoops->TrqFFCx = pBaseCtrl->CtrlCmdMngr.TrqFFCx;
	BaseLoops->SpdFBCx = pBaseCtrl->CtrlCmdMngr.SpdFBCx;
	BaseLoops->TrqFBCx = pBaseCtrl->CtrlCmdMngr.TrqFBCx;

//	BaseLoops->TrqRefs;
//	BaseLoops->TrqClamp = FALSE; /* <S04B> */
	BaseLoops->TrqClamp = pBaseCtrl->CtrlCmdMngr.TrqClamp; /* <S04B> */

	if ( BaseLoops->LastCtrlMode == CTRLMODE_TRQ )       /* <S075> */
	{
		BaseLoops->SpdCtrl.V.SpdRefFilClrCmd = TRUE;     /* ���x�w�߈ʑ��i�ݕ⏞�t�B���^������ <S075> */
		BaseLoops->SpdCtrl.V.SpdFFFilClrCmd = TRUE;      /* ���x�e�e�ʑ��i�ݕ⏞�t�B���^������ <S075> */
	}
}



/* 2012.12.27 Y.Oka �b��Ή��B��ł����ƍ�邱�ƁB */
void SysOutputScanAProcessData( BASE_LOOPCTRLS *BaseLoops, CTRL_LOOP_OUT *CtrlLoopOut )
{
//	POSCTRL		*PosCtrl;		/* �ʒu����p�ϐ� */
	POS_MNG_HNDL *PosMngV;
//	PosCtrl = &BaseLoops->PosCtrl;
	PosMngV = &BaseLoops->BaseCtrls->PosManager;

	CtrlLoopOut->MotPosErr = PosMngV->var.PosErr;	/* �ʒu�A���v�΍�					[Pulse]				*/
	CtrlLoopOut->SpdRefo = BaseLoops->SpdRefo;		/* ���x�w�ߏo��						[2^24/OvrSpd]		*/
//	CtrlLoopOut->SpdFFC;							/* ���xFF�⏞						[2^24/OvrSpd]		*/
	CtrlLoopOut->MotSpd_a = BaseLoops->MotSpd_a;
	CtrlLoopOut->TrqRefo = BaseLoops->TrqRefo;		/* �g���N�w�ߏo��					[2^24/MaxTrq]		*/
	CtrlLoopOut->TrqRefo_a = BaseLoops->TrqRefo;	/* �g���N�w�ߏo��(���ϒl)			[2^24/MaxTrq]		*/
//	CtrlLoopOut->TrqRefMon = BaseLoops->TrqRefo;	/* �g���N�w�߃��j�^					[2^24/MaxTrq]		*/
//	CtrlLoopOut->TrqRefMon_a = BaseLoops->TrqRefo;	/* �g���N�w�߃��j�^					[2^24/MaxTrq]		*/
	CtrlLoopOut->TrqRefMon = 						/* @@Add �g���N�w�߃��j�^ [2^24/MaxTrq] */
	MlibMulhigh32( (CtrlLoopOut->IqRefMon << 16), NORM_IQ_TO_TRQ );
	CtrlLoopOut->TrqRefMon_a = 						/* @@Add  �g���N�w�߃��j�^(���ϒl) [2^24/MaxTrq] */
	MlibMulhigh32( (CtrlLoopOut->IqRefMon_a << 16), NORM_IQ_TO_TRQ );

//	CtrlLoopOut->IdRefMon;							/* d���d���w�߃��j�^				[15000/Imax]		*/
//	CtrlLoopOut->IqRefMon;							/* q���d���w�߃��j�^				[15000/Imax]		*/
//	CtrlLoopOut->IqRefMon_a;						/* q���d���w�߃��j�^(���ϒl)		[15000/Imax]		*/
//	CtrlLoopOut->IqRefMon_l;						/* q���d���w�߃��j�^(�O��l)		[15000/Imax]		*/
//	CtrlLoopOut->IqFbMon;							/* q���d��FB���j�^										*/
//	CtrlLoopOut->IdFbMon;							/* d���d��FB���j�^										*/
//	CtrlLoopOut->IqRefFilMon;						/* q���d���w�߃��j�^(�t�B���^��)						*/
//	CtrlLoopOut->PConActFlg = FALSE;				/* P�����Ԋm�F�t���O									*//* <S064> */
//	CtrlLoopOut->SvonSpdLmtFlag = FALSE;			/* �T�[�{ON�����x�������t���O							*//* <S04B> */
	CtrlLoopOut->SvonSpdLmtFlag = BaseLoops->SpdCtrl.V.SvonSpdLmtFlag;	/* �T�[�{ON�����x�������t���O		*//* <S04B> */
//	CtrlLoopOut->SpdClamp = FALSE;					/* �g���N���䎞���x�������t���O							*//* <S04B> */
//	CtrlLoopOut->TrqClamp = FALSE;					/* �g���N�������t���O									*//* <S04B> */
}


/****************************************************************************************************/
/*																									*/
/*		�T�[�{�ʒu�Ǘ����C���֐�																	*/
/*																									*/
/****************************************************************************************************/
static void	LpxSvPositionManager( BASE_LOOPCTRLS *BaseLoops, POS_MNG_HNDL *PosManager )
{
/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu�w�ߍX�V�i125us��1��ʒu�w�߂��X�V�����B�����62.5us�ňʒu���䂷��ׂ̏��u�j			*/
/*--------------------------------------------------------------------------------------------------*/
	if( PosManager->RefRenewCntA != PosManager->RefRenewCntB )
	{ /* ScanB�ňʒu�w�߂��X�V���ꂽ���1���	*/
		/* 1���ScanA�̈ʒu�w�߂��쐬 */
		BaseLoops->dPosRefi = PosManager->CompdPosRefi >> 1;
		BaseLoops->dPcmda   = PosManager->CompdPcmda   >> 1;

		if( PosManager->RefRenewalChkCnt == 0 )
		{ /* 2���ScanA������Ȃ������ꍇ�̏��u */
			/* �O���2���ScanA�̈ʒu�w�߂����Z */
			BaseLoops->dPosRefi += BaseLoops->NextdPosRefi;
			BaseLoops->dPcmda   += BaseLoops->NextdPcmda;
		}

		/* 2���ScanA��	�ʒu�w�߂��쐬 */
		BaseLoops->NextdPosRefi = PosManager->CompdPosRefi - (PosManager->CompdPosRefi >> 1);
		BaseLoops->NextdPcmda   = PosManager->CompdPcmda   - (PosManager->CompdPcmda   >> 1);

		PosManager->RefRenewalChkCnt = 0;
	}
	else
	{ /* ScanB�ňʒu�w�߂��X�V���ꂽ���2��� */
		if( PosManager->RefRenewalChkCnt == 0 )
		{
			/* 2���ScanA�̈ʒu�w�߂��쐬 */
			BaseLoops->dPosRefi = BaseLoops->NextdPosRefi;
			BaseLoops->dPcmda   = BaseLoops->NextdPcmda;
		}
		else
		{ /* ScanA��3��ȏ㑖�����ꍇ�̏��u */
			/* 3��ڈȍ~�͎w�߂̍쐬���s��Ȃ� */
			BaseLoops->dPosRefi = 0;
			BaseLoops->dPcmda   = 0;
		}
		PosManager->RefRenewalChkCnt++;
	}
	PosManager->RefRenewCntA = PosManager->RefRenewCntB;

/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu�΍����Z(�w�ߒP��)																<S14C>	*/
/*--------------------------------------------------------------------------------------------------*/
/* 2012.09.07 Y.Oka ���ʒu�΍��N���A�@�\���󖢑Ή��� */
//	if( !BoutV.f.PerClrCmd )
//	if((BaseLoops->LastCtrlMode == CTRLMODE_SPD)||(BaseLoops->LastCtrlMode == CTRLMODE_TRQ))/* <S007> */
//	{/* ���䃂�[�h�ؑ֎�	*/
//		BaseLoops->PosCtrl.V.Per64[0] = PosManager->Per64[0];
//		BaseLoops->PosCtrl.V.Per64[1] = PosManager->Per64[1];
//	}
	if( !(BaseLoops->BaseCtrls->CtrlCmdMngr.PerrClrReq) )
	{
#if (FLOAT_USE==TRUE)
		if( PosManager->var.EgearDisable )
		{
			FlibPerrcala( BaseLoops->dPcmda, BaseLoops->dPosFbki,
							&PosManager->conf.InvalidEgear, &PosManager->var.PosErrA );
		}
		else
		{
			FlibPerrcala( BaseLoops->dPcmda, BaseLoops->dPosFbki,
							&PosManager->conf.Egear, &PosManager->var.PosErrA );
		}
	}
#else
		if( PosManager->var.EgearDisable )
		{
			MlibPerrcala( BaseLoops->dPcmda, BaseLoops->dPosFbki,
							&PosManager->conf.InvalidEgear, &PosManager->var.PosErrA );
		}
		else
		{
			MlibPerrcala( BaseLoops->dPcmda, BaseLoops->dPosFbki,
							&PosManager->conf.Egear, &PosManager->var.PosErrA );
		}
	}
#endif
	return;
}



/***************************************** end of file **********************************************/
