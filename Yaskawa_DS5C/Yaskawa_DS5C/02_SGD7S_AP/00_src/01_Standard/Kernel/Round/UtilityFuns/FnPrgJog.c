/****************************************************************************
Description	: FunPrgJog.c : �v���O����JOG�֘A���W���[��
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
T.Kira 2011.02.18    ����
****************************************************************************/
#include "Basedef.h"
#include "MercuryGlobals.h"

/****************************************************************************
		Fn004 Program Jog mode Begin
****************************************************************************/
PRM_RSLT FnEnterPrgJog( FUNEXE *FunExe, AXIS_HANDLE *Axis )
{
	FUN_CMN_CONTROL *FnCmnCtrl;
	PJOGV *PJogV;

	FnCmnCtrl = Axis->FnCmnCtrl;
	PJogV = &Axis->BaseControls->PJogHdl.PJogV;

	/* �T�[�{�I�����̏ꍇ�͓��炸�ɏI��	*/
	if(FnCmnCtrl->State.FnSvCtrlDisable != FALSE)
	{
		return PRM_RSLT_CONDITION_ERR;
	}

	/* �p�����[�^�g�ݍ��킹�ُ픭����	*/
//<S0E5>	if( ALMGetAlarmInfo( Axis->AlmManager ) == ALM_PRMUNMATCH )
	if(FALSE != ALMCheckEachState( Axis->AlmManager, ALM_PRMUNMATCH ))	/* <S0E5> */
	{
		return PRM_RSLT_CONDITION_ERR;
	}

/* <S226> */
	PrgJogReset( &Axis->BaseControls->PJogHdl );
	PJogV->Var.State = PJOG_START;
	PcalPjogSw(       Axis );		/* Pn530 : �v���O����JOG�^�]�֌W�X�C�b�`		*/
	PcalPjogDistance( Axis );		/* Pn531,532 : �v���O����JOG�ړ�����[�w�ߒP��]	*/
	PcalPjogRotspd(   Axis );		/* Pn533 : �v���O����JOG�ړ����x[min-1]			*/
	PcalPjogAcctime(  Axis );		/* Pn534 : �v���O����JOG����������[ms]			*/
	PcalPjogWaitTime( Axis );		/* Pn535 : �v���O����JOG�҂�����[ms]			*/
	PcalPjogNum(      Axis );		/* Pn536 : �v���O����JOG�ړ���				*/
	IprmcalPrgJog( &Axis->BaseControls->PJogHdl,
					Axis->UniPrms->Bprm,
				   &Axis->BaseControls->PosManager );
/* <S226> */

	/* Fn�T�[�{�^�]���l�� */
	FnCmnCtrl->FnSvControl = TRUE;
	FnCmnCtrl->FnCtrlMcmd  = CTRLMODE_PJOG;	/* <S107> */
	FnCmnCtrl->FnSensOnReq = TRUE;
	FnCmnCtrl->JogSpeed = 0;

	if ( ( PJogV->Cnst.PrmUnMatch == TRUE ) || ( PJogV->Cnst.PrmUnMatch2 == TRUE ) )
	{
		/* A.042 : �p�����[�^�g���킹�ُ� */
		ALMSetServoAlarm( Axis->AlmManager, ALM_PRMUNMATCH );
	}

	FunExe->HoldFnMode = TRUE;/* Fn���[�h�����I���Ȃ� */

	return(PRM_RSLT_SUCCESS);
}

/****************************************************************************
		Fn004 Program Jog mode End
****************************************************************************/
void FnLeavePrgJog(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	FUN_CMN_CONTROL *FnCmnCtrl;
	//SEQ_CTRL_OUT	*SeqCtrlOut;
	PJOGV *PJogV;

	FnCmnCtrl = Axis->FnCmnCtrl;
	PJogV = &Axis->BaseControls->PJogHdl.PJogV;

	PJogV->Var.RoutCmd = PJOGCMD_INIT;

	/* ���E���h���䒆��SEN�M��ON�v����I/O�̏�ԂŐݒ肷�� */
	//FnCmnCtrl->FnSensOnReq  = ((Axis->SeqCtrlData->CmdSeqBit & SENSOR_ON_BIT)== TRUE)
	//																	? TRUE : FALSE;

	/* �T�[�{�I����Ԃ̏ꍇ�A�T�[�{�I�t���鏈��	*/
	//while(SeqCtrlOut->BaseEnableReq != FALSE);

	/* �I������	*/
	FnCmnCtrl->JogSpeed    = 0;
	FnCmnCtrl->FnSvonReq   = FALSE;
	FnCmnCtrl->FnCtrlMcmd  = CTRLMODE_NOCMD;
	FnCmnCtrl->FnSensOnReq = FALSE;
	FnCmnCtrl->FnSvControl = FALSE;
}

/****************************************************************************
		Fn004 Program Jog mode Execution
****************************************************************************/
LONG FnExecutePrgJog(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	PRM_RSLT rc;
	ULONG WaitTime;

	PJOGV *PJogV;
	FUN_CMN_CONTROL *FnCmnCtrl;
	SEQ_CTRL_OUT *SeqCtrlOut;

	PJogV = &Axis->BaseControls->PJogHdl.PJogV;
	FnCmnCtrl = Axis->FnCmnCtrl;
	SeqCtrlOut= Axis->SeqCtrlOut;

	rc = PRM_RSLT_SUCCESS;

	switch( FunExe->State )
	{
		case 0:	/* ����������	*/
			/* �v���O����JOG���[�h�̐ݒ菈�� */
			FnCmnCtrl->FnCtrlMcmd = CTRLMODE_PJOG;
			PJogV->Var.RoutCmd = PJOGCMD_INIT;
			PrgJogReset( &Axis->BaseControls->PJogHdl );
			FnCmnCtrl->FnSensOnReq = TRUE;
			FnCmnCtrl->FnSvonReq = FALSE;

			/* �����X�e�[�g�̈ڍs */
			FunExe->State = 1;
			break;

		case 1:	/* �T�[�{�I�t��	*/
			if( (PJogV->Var.RoutCmd == PJOGCMD_INIT && PJogV->Var.State == PJOG_INIT) ||
				 PJogV->Var.State == PJOG_ABORT || PJogV->Var.State == PJOG_ERROR )
			{
				PJogV->Var.RoutCmd = PJOGCMD_NONE;
			}

			switch( FunExe->CmdCode )
			{
				/* �T�[�{�I������ */
				case FCMD_SV:
//<S0E5>					if( ALMGetAlarmInfo( Axis->AlmManager ) != 0x00 || ( SeqCtrlOut->HwbbSts == TRUE) ||
					if( ((Axis->AlmManager)->Status.AlmFlag != FALSE) || ( SeqCtrlOut->HwbbSts == TRUE) ||	/* <S0E5> */ /* <S0FF> */
					  (SeqCtrlOut->MainPowerOn == FALSE) || (SeqCtrlOut->OverTrvlSts == TRUE) )
					{
						rc = PRM_RSLT_CONDITION_ERR;
					}
					else
					{
						FnCmnCtrl->FnSvonReq = ~FnCmnCtrl->FnSvonReq;
					}
					break;
				case FCMD_EXEC: case FCMD_CH1: case FCMD_CH2:
				case FCMD_EWRT: case FCMD_MTUP: case FCMD_MTDOWN:
					rc = PRM_RSLT_CONDITION_ERR;
					break;
				default:
					break;
			}
			/* �T�[�{�I���ŏ����X�e�[�g�̈ڍs���� */
			if( Axis->SeqCtrlOut->BaseEnableReq == TRUE )
			{
				PJogV->Var.RoutCmd = PJOGCMD_INIT;
				FunExe->State = 2;
			}
			break;

		case 2:	/* �T�[�{�I����	(��~���) */
			if( (PJogV->Var.RoutCmd == PJOGCMD_INIT) && (PJogV->Var.State == PJOG_INIT) )
			{
				PJogV->Var.RoutCmd = PJOGCMD_NONE;
			}
			switch( FunExe->CmdCode )
			{
				case FCMD_NONE:
				case FCMD_STOP:
					break;
				case FCMD_UP:		/* �v���O����JOG�^�]�J�n(���])	*/
					if( !PJogV->Cnst.RevDir && Axis->SeqCtrlOut->BaseEnableReq )
					{
						PJogV->Var.RoutCmd = PJOGCMD_START;
						KlibRstLongTimer( &FunExe->LongTimer );
						FunExe->State = 3;
					}
					break;
				case FCMD_DOWN:		/* �v���O����JOG�^�]�J�n(�t�])	*/
					if( PJogV->Cnst.RevDir && Axis->SeqCtrlOut->BaseEnableReq )
					{
						PJogV->Var.RoutCmd = PJOGCMD_START;
						KlibRstLongTimer( &FunExe->LongTimer );
						FunExe->State = 3;
					}
					break;
				case FCMD_SV:		/* �T�[�{�I�t */
					FnCmnCtrl->FnSvonReq = FALSE;
					break;
				case FCMD_EXEC: case FCMD_CH1:  case FCMD_CH2:
				case FCMD_EWRT: case FCMD_MTUP: case FCMD_MTDOWN:
					rc = PRM_RSLT_CONDITION_ERR;
					break;
				default:
					break;
			}
			/* �T�[�{�I�t�ŏ����X�e�[�g�̈ڍs���� */
			if( !Axis->SeqCtrlOut->BaseEnableReq )
			{
				FunExe->State = 1;
			}
			break;

		case 3:	/* �^�]�� */
			if( (PJogV->Var.RoutCmd == PJOGCMD_START) && (PJogV->Var.State == PJOG_START) )
			{
				PJogV->Var.RoutCmd = PJOGCMD_NONE;
			}

/* <S198> */
//			if(FnCmnCtrl->SafeStopTime == 65535)
//			{
//				WaitTime = 10 * 1000; /* 10s Wait	*/
//			}
//			else
//			{
//				WaitTime = FnCmnCtrl->SafeStopTime * 1000;	/* ���S��~���� Wait */
//			}
//			if( KlibGetLongTimerMs( FunExe->LongTimer ) > WaitTime)
//			{
//				PJogV->Var.RoutCmd = PJOGCMD_ABORT;
//			}

			switch( FunExe->CmdCode )
			{
//			case FCMD_NONE:		/* <S198> */
//				break;			/* <S198> */
			case FCMD_STOP:
				PJogV->Var.RoutCmd = PJOGCMD_ABORT;
				break;
			case FCMD_UP:		/* ���]	*/
				if( !PJogV->Cnst.RevDir && Axis->SeqCtrlOut->BaseEnableReq )
				{
					KlibRstLongTimer( &FunExe->LongTimer );
				}
				break;
			case FCMD_DOWN:		/* �t�]	*/
				if( PJogV->Cnst.RevDir && Axis->SeqCtrlOut->BaseEnableReq )
				{
					KlibRstLongTimer( &FunExe->LongTimer );
				}
				break;
			case FCMD_SV:		/* �T�[�{�I�t	*/
				FnCmnCtrl->FnSvonReq = FALSE;
				break;
			case FCMD_EXEC: case FCMD_CH1: case FCMD_CH2:
			case FCMD_EWRT: case FCMD_MTUP: case FCMD_MTDOWN:
				rc = PRM_RSLT_CONDITION_ERR;
				break;
			case FCMD_NONE:		/* <S193> */
			default:
/* <S198> */
				if(FnCmnCtrl->SafeStopTime == 65535)
				{
					WaitTime = 10 * 1000; /* 10s Wait	*/
				}
				else
				{
					WaitTime = FnCmnCtrl->SafeStopTime * 1000;	/* ���S��~���� Wait */
				}
				if( KlibGetLongTimerMs( FunExe->LongTimer ) > WaitTime)
				{
					PJogV->Var.RoutCmd = PJOGCMD_ABORT;
				}
				break;
			}

			/* �T�[�{�I�t�ŏ����X�e�[�g�̈ڍs����	*/
			if(!Axis->SeqCtrlOut->BaseEnableReq)
			{
				FunExe->State = 1;
			}

			/* ScanC���s�m�F */
			if( PJogV->Var.ScanCRunFlg == TRUE )
			{/* PJOG���f��ɍăX�^�[�g����ƁAScanC������Ȃ������� */	
			 /* �����ɂ��Ă��܂�(State=PJOG_ABORT�̂܂܂�)����		*/
			 /* ScanC���s���m�F����									*/

				if( (PJogV->Var.State == PJOG_ABORT) 
				 || (PJogV->Var.State == PJOG_END) 
				 ||	(PJogV->Var.State == PJOG_ERROR) )
				{
					PJogV->Var.RoutCmd = PJOGCMD_NONE;
					KlibRstLongTimer( &FunExe->LongTimer );
					FunExe->State = 4;
				}
			}
			break;

		case 4:	/* �^�]�I��(�㏈��)	*/
			if(FnCmnCtrl->SafeStopTime == 65535)
			{
				WaitTime = 10 * 1000; /* 10s Wait	*/
			}
			else
			{
				WaitTime = FnCmnCtrl->SafeStopTime * 1000;	/* ���S��~���� Wait */
			}
			if( KlibGetLongTimerMs( FunExe->LongTimer ) > WaitTime)
			{
				FunExe->State = 2;
			}
			break;

		default:
			break;
	}

	/* ScanC��ON,Round��OFF����t���O	*/
	PJogV->Var.ScanCRunFlg = FALSE;
	
	return(rc);
}

/****************************************************************************
		Pn530 : �v���O����JOG�^�]�֌W�X�C�b�`
****************************************************************************/
void PcalPjogSw( void *AxisHdl )
{
	AXIS_HANDLE *Axis;
	PRMDATA *Prm;
	PJOGV *PJogV;

	Axis = (AXIS_HANDLE*)AxisHdl;
	Prm = Axis->UniPrms->Prm;
	PJogV = &Axis->BaseControls->PJogHdl.PJogV;

	if( PJogV->Var.State == PJOG_START )
	{
		PJogV->Var.RoutCmd = PJOGCMD_ABORT;
	}
	PJogV->Cnst.Pattern = (CHAR) ( Prm->pjogsw & 0x000F );
	PJogV->Cnst.RevDir = ( PJogV->Cnst.Pattern & 0x01 )? TRUE : FALSE;
	PJogV->Cnst.MTimes = ( PJogV->Cnst.Pattern < 2 )? Prm->pjognum : ( Prm->pjognum << 1 );
}

/****************************************************************************
		Pn531,532 : �v���O����JOG�ړ�����[�w�ߒP��]
****************************************************************************/
void PcalPjogDistance( void *AxisHdl )
{
	AXIS_HANDLE *Axis;
	PRMDATA *Prm;
	PJOGV *PJogV;

	Axis = (AXIS_HANDLE*)AxisHdl;
	Prm = Axis->UniPrms->Prm;
	PJogV = &Axis->BaseControls->PJogHdl.PJogV;

	if( PJogV->Var.State == PJOG_START )
	{
		PJogV->Var.RoutCmd = PJOGCMD_ABORT;
	}
	PJogV->Cnst.Distance = /*(Prm->pjogdisth << 16)*/ + Prm->pjogdistl;
	/* Prm->pjogdisth�͖����p�����[�^�̂��߃R�����g�A�E�g_kira_110223 */
}

/****************************************************************************
		Pn533 : �v���O����JOG�ړ����x[min-1]
****************************************************************************/
void PcalPjogRotspd( void *AxisHdl )
{
	LONG	PjogSpdP1;
	LONG	PjogSpdP2;
	LONG	kx1, sx1;
	LONG	kx2, sx2;
	LONG	Egear_a,Egear_b;

	AXIS_HANDLE *Axis;
	PRMDATA *Prm;
	BPRMDAT *Bprm;
	PJOGV	*PJogV;
	PCMKPV  *PJogPcmk;

	Axis = (AXIS_HANDLE*)AxisHdl;
	Bprm = Axis->UniPrms->Bprm;
	Prm  = Axis->UniPrms->Prm;
	PJogV = &Axis->BaseControls->PJogHdl.PJogV;
	PJogPcmk = &Axis->BaseControls->PJogHdl.PJogPcmk;
	Egear_a = Axis->BaseControls->PosManager.conf.Egear.a;
	Egear_b = Axis->BaseControls->PosManager.conf.Egear.b;

	if( Axis->MencV->AxisMotType == MOTTYPE_ROTARY )
	{
		PjogSpdP1 = (Axis->MencV->LowSpdMotor)? Prm->pjogrotspd : (10 * Prm->pjogrotspd);
		if( PJogV->Var.State == PJOG_START )
		{
			PJogV->Var.RoutCmd = PJOGCMD_ABORT;
		}

		PJogV->Cnst.MaxSpd = BprmSpeedLevelCal( Bprm, PjogSpdP1, 0 );
		PJogV->Cnst.AccTime = ( Prm->pjogacctm == 0 )? 2000 : ( Prm->pjogacctm * 1000 );
		MlibPcalaPcmdMaker( PJogV->Cnst.MaxSpd, PJogV->Cnst.AccTime,
									PJogV->Cnst.AccTime, Bprm->SvCycleUs, &PJogPcmk->P );
		kx1 = MlibScalKxgain( Egear_a, 256, Egear_b, &sx1, -1 );
		kx2 = MlibScalKskxkx( Bprm->KMotSpdConv, PjogSpdP1, 1, &sx2, -1 );
		PjogSpdP2 = MlibScalKskskx( kx1, kx2, 256, NULL, 24 );
		PjogSpdP2 = MlibGAINRD( PjogSpdP2 );
		if( PjogSpdP2 < LMTLOWSPEED )
		{
			PJogV->Cnst.PrmUnMatch = TRUE;
		}
		else
		{
			PJogV->Cnst.PrmUnMatch = FALSE;
		}
	}
}

/****************************************************************************
		Pn534 : �v���O����JOG����������[ms]
****************************************************************************/
void PcalPjogAcctime( void *AxisHdl )
{
	AXIS_HANDLE *Axis;
	PRMDATA *Prm;
	BPRMDAT *Bprm;
	PJOGV *PJogV;
	PCMKPV *PJogPcmk;

	Axis = (AXIS_HANDLE*)AxisHdl;
	Bprm = Axis->UniPrms->Bprm;
	Prm  = Axis->UniPrms->Prm;
	PJogV = &Axis->BaseControls->PJogHdl.PJogV;
	PJogPcmk = &Axis->BaseControls->PJogHdl.PJogPcmk;

	if( PJogV->Var.State == PJOG_START )
	{
		PJogV->Var.RoutCmd = PJOGCMD_ABORT;
	}
	PJogV->Cnst.AccTime = ( Prm->pjogacctm == 0 )? 2000 : ( Prm->pjogacctm * 1000 );

	MlibPcalaPcmdMaker( PJogV->Cnst.MaxSpd, PJogV->Cnst.AccTime,
								PJogV->Cnst.AccTime, Bprm->SvCycleUs, &PJogPcmk->P );
}

/****************************************************************************
		Pn535 : �v���O����JOG�҂�����[ms]
****************************************************************************/
void PcalPjogWaitTime( void *AxisHdl )
{
	AXIS_HANDLE *Axis;
	PRMDATA *Prm;
	PJOGV	*PJogV;

	Axis = (AXIS_HANDLE*)AxisHdl;
	Prm = Axis->UniPrms->Prm;
	PJogV = &Axis->BaseControls->PJogHdl.PJogV;

	if( PJogV->Var.State == PJOG_START )
	{
		PJogV->Var.RoutCmd = PJOGCMD_ABORT;
	}
	if( Prm->pjogwaittm < 2 )
	{
		PJogV->Cnst.WaitTime = 0;
	}
	else
	{
		/* �V�[�P���X�J�ڕ����Z */
		PJogV->Cnst.WaitTime = Prm->pjogwaittm - 2;
	}
}

/****************************************************************************
		Pn536 : �v���O����JOG�ړ���
****************************************************************************/
void PcalPjogNum( void *AxisHdl )
{
	AXIS_HANDLE *Axis;
	PRMDATA *Prm;
	PJOGV	*PJogV;

	Axis = (AXIS_HANDLE*)AxisHdl;
	Prm   = Axis->UniPrms->Prm;
	PJogV = &Axis->BaseControls->PJogHdl.PJogV;

	if( PJogV->Var.State == PJOG_START )
	{
		PJogV->Var.RoutCmd = PJOGCMD_ABORT;
	}
	PJogV->Cnst.MTimes = ( PJogV->Cnst.Pattern < 2 )? Prm->pjognum : ( Prm->pjognum << 1 );
}

/****************************************************************************
		Pn585 : �v���O����JOG�ړ����x[mm/s] (���j�A��p)
****************************************************************************/
void PcalPjogLinspd( void *AxisHdl )
{
	LONG	PjogSpdP1;
	LONG	PjogSpdP2;
	LONG	kx1, sx1;
	LONG	kx2, sx2;
	LONG	Egear_a,Egear_b;

	AXIS_HANDLE *Axis;
	PRMDATA *Prm;
	BPRMDAT *Bprm;
	PJOGV *PJogV;
	PCMKPV *PJogPcmk;

	Axis = (AXIS_HANDLE*)AxisHdl;
	Bprm = Axis->UniPrms->Bprm;
	Prm  = Axis->UniPrms->Prm;
	PJogV = &Axis->BaseControls->PJogHdl.PJogV;
	PJogPcmk = &Axis->BaseControls->PJogHdl.PJogPcmk;
	Egear_a = Axis->BaseControls->PosManager.conf.Egear.a;
	Egear_b = Axis->BaseControls->PosManager.conf.Egear.b;

	if( Axis->MencV->AxisMotType == MOTTYPE_LINEAR )
	{
		PjogSpdP1 = (Axis->MencV->LowSpdMotor)? Prm->pjoglinspd : (10 * Prm->pjoglinspd);

		if( PJogV->Var.State == PJOG_START )
		{
			PJogV->Var.RoutCmd = PJOGCMD_ABORT;
		}

		PJogV->Cnst.MaxSpd  = BprmSpeedLevelCal(Bprm, PjogSpdP1, 0 );
		PJogV->Cnst.AccTime = ( Prm->pjogacctm == 0 )? 2000 : ( Prm->pjogacctm * 1000 );
		MlibPcalaPcmdMaker( PJogV->Cnst.MaxSpd, PJogV->Cnst.AccTime,
									PJogV->Cnst.AccTime, Bprm->SvCycleUs, &PJogPcmk->P );
		kx1 = MlibScalKxgain( Egear_a, 256, Egear_b, &sx1, -1 );
		kx2 = MlibScalKskxkx( Bprm->KMotSpdConv, PjogSpdP1, 1, &sx2, -1 );
		PjogSpdP2 = MlibScalKskskx( kx1, kx2, 256, NULL, 24 );
		PjogSpdP2 = MlibGAINRD( PjogSpdP2 );
		if( PjogSpdP2 < LMTLOWSPEED )
		{
			PJogV->Cnst.PrmUnMatch = TRUE;
		}
		else
		{
			PJogV->Cnst.PrmUnMatch = FALSE;
		}
    }
}

/****************************************************************************************************/

