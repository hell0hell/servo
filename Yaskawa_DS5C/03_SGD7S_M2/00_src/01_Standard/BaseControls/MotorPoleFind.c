/****************************************************************************************************/
/*																									*/
/*																									*/
/*		MotorPoleFind.c : ���Ɍ��o����p	���W���[����`											*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	����	2011.01.25	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "MotorPoleFind.h"
#include "MLib.h"



/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
/*	LONG	MpoleFindSpeedControl( MPFIND *MpFind, MPFEXEINFO *MpfExeInfo, BOOL BeComplete );		*/
/*	ALMID_T	MpoleFindSequence( MPFIND *MpFind, MPFEXEINFO *MpfExeInfo, BOOL PdetReq );				*/
/*	ALMID_T	MpoleFindDetectAlarm( MPFIND *MpFind, MPFEXEINFO *MpfExeInfo, BOOL PdetReq );			*/
/*	ALMID_T	MpoleFindCheckMovingRange( MPFIND *MpFind, BASE_CTRL_HNDL *pBaseCtrl, KSGAIN Kmovpos );	*/
/****************************************************************************************************/
static ALMID_T	MpoleFindDetectAlarm( MPFIND *MpFind, MPFEXEINFO *MpfExeInfo );
static ALMID_T	MpoleFindCheckMovingRange( MPFIND *MpFind, LONG dMotPos, KSGAIN Kmovpos );
static	void	MpoleFindDrive( MPFIND *MpFind, LONG SpdFbki, LONG TrqRefo );
static	void	CheckMpFindFail( MPFIND *MpFind, LONG SpdFbki );
static	void	MpFindLauSpdRef( MPFIND *MpFind, LONG SpdFbki );
static	void	MpFindCalPosAbout( MPFIND *MpFind );
static	void	MpFindCalPos( LONG *DegCalc, LONG *Counter, LONG *Degree, BOOL *NoGood_flag );
static	void	MpFindGetMaxTrqSpd( MPFIND *MpFind, LONG SpdFbki, LONG TrqRefo );
static	void	JudgeMpFindErrSign( MPFIND *MpFind );
static	void	UpdatePhaseCalculation( MPFIND *MpFind );
static	void	StopMpoleFind( MPFIND *MpFind, BOOL MotStopSts );
static	BOOL	ConfirmMpoleFind( MPFIND *MpFind, KSGAIN Kphasepos, LONG dMotPos,
																			LONG SpdFbki );
static	LONG	MpFindLauTrqRef( MPFIND *MpFind, BOOL *WaitFinish );
/****************************************************************************************************/



/****************************************************************************************************/
/*																									*/
/*		���Ɍ��o�p���x���䉉�Z																		*/
/*																									*/
/****************************************************************************************************/
LONG	MpoleFindSpeedControl( MPFIND *MpFind, MPFEXEINFO *MpfExeInfo, BOOL BeComplete )
{
	LONG			PacOut;
	LONG			IacOut;
	LONG			TrqRefx;
	LONG			work_integless;
	LONG			work;

/*--------------------------------------------------------------------------------------------------*/
/*		�ʑ��␳�l�m�F�p�g���N����v��������														*/
/*--------------------------------------------------------------------------------------------------*/
	/* Check Control Mode Change Flag */
	if( MpFind->var.PhaseConfFlg == TRUE )
	{
		/* Direct Torque Reference Input */
		MpFind->var.Ivar64[1] = MpFind->var.Ivar64[0] = 0;
		return( MpFind->var.TrqRefPhaConf );
	}

/*--------------------------------------------------------------------------------------------------*/
/*		���xFB�t�B���^����																			*/
/*--------------------------------------------------------------------------------------------------*/
	MpFind->var.SpdFbFilo = MlibLpfilter1( MpfExeInfo->SpdFbMonitor,
										   MpfExeInfo->KVfbFil,
										   MpFind->var.SpdFbFilo );

/*--------------------------------------------------------------------------------------------------*/
/*		���x�΍����Z																				*/
/*--------------------------------------------------------------------------------------------------*/
	MpFind->var.SpdErr = MpFind->MpFindSpdRef - MpFind->var.SpdFbFilo;

/*--------------------------------------------------------------------------------------------------*/
/*		�x�[�X�u���b�N���̏���																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( (BeComplete == FALSE) || (MpFind->var.ZeroTorque == TRUE) )
	{
		MpFind->TrqFilOut = 0;
		MpFind->var.Ivar64[1] = MpFind->var.Ivar64[0] = 0;
//		pBaseCtrl->CtrlCmdMngr.SpdRefo = 0;
		return( 0 );
	}

/*--------------------------------------------------------------------------------------------------*/
/*		���x���䏈��																				*/
/*--------------------------------------------------------------------------------------------------*/
	/* ��ፀ�v�Z */
	PacOut = MpFind->var.SpdErr;

	/* �ϕ����v�Z */
	if( MpFind->var.ClrIntegral )
	{
		IacOut = 0;
		work_integless = 0;
		MpFind->var.Ivar64[0] = 0;
		MpFind->var.Ivar64[1] = 0;
		MpFind->var.ClrIntegral = FALSE;
	}
	else
	{
		if( !MpFind->var.InteglessON )
		{
			/* �ʏ펞 */
			work_integless = 0;
		}
		else
		{
			/* �ϕ����X�� */
#if (FLOAT_USE==TRUE)
			work_integless = (LONG)( (float)(MpFind->var.Ivar64[1] >> 1) * MpFind->conf.InteglessGain );
#else
			work_integless = MlibMulgainNolim( (MpFind->var.Ivar64[1] >> 1), MpFind->conf.InteglessGain );
#endif /* FLOAT_USE */
		}
	}
	work = MpFind->var.SpdErr - work_integless;
	IacOut =
		MlibIntegral( work, MpFind->conf.Ki, MpFind->var.Ivar64 );

	work = PacOut + IacOut;

	/* �Q�C���̎������� */
	switch( MpFind->var.KvGain )
	{
		case 0 :		/* �Q�C��1�{ */
			break;

		case 1 :		/* �Q�C��1.5�{ */
			work = work + (work >> 1);
			break;

		case 2 :		/* �Q�C��2�{ */
			work = (work << 1);
			break;

		case -1 :		/* �Q�C��0.75�{ */
			work = (work >> 1) + (work >> 2);
			break;

		case -2 :		/* �Q�C��0.5�{ */
			work = (work >> 1);
			break;

		default :
			break;
	}
#if (FLOAT_USE==TRUE)
	TrqRefx = ( (float)work * MpFind->conf.Kv );
#else
	TrqRefx = MlibMulgain( work, MpFind->conf.Kv );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		�P�����[�p�X�t�B���^���g���N��������														*/
/*--------------------------------------------------------------------------------------------------*/
	MpFind->TrqFilOut = MlibLpfilter1( TrqRefx, MpFind->conf.Klpf, MpFind->TrqFilOut );
	return( MlibLimitul(MpFind->TrqFilOut, 0x01000000, -0x01000000) );

}



/****************************************************************************************************/
/*																									*/
/*		���Ɍ댟�o�A���[�����o																		*/
/*																									*/
/****************************************************************************************************/
static ALMID_T	MpoleFindDetectAlarm( MPFIND *MpFind, MPFEXEINFO *MpfExeInfo )
{
	ALMID_T		AlarmCode;

	if(MpfExeInfo->HaltCondition != FALSE)
	{
		MpFind->MPJudgeMonitor[0] = 0;	/* ���Ɍ��o���胂�j�^���Z�b�g		*/
		MpFind->MPJudgeMonitor[1] = 0;	/* ���Ɍ��o���胂�j�^���Z�b�g		*/
		MpFind->MPJudgeMonitor[2] = 0;	/* ���Ɍ��o���胂�j�^���Z�b�g		*/
		AlarmCode = ALM_PFINDSTOP;	/* A.C51 : ���Ɍ��o��~ */
	}
	else if(MpFind->var.FailTimes >= TRYCOUNT)
	{
//		MpFind->var.Step = MPF_INIT;
		AlarmCode = ALM_POLE;		/* A.C50 : ���Ɍ댟�o */
	}
	else
	{
		AlarmCode = MpoleFindCheckMovingRange(MpFind,
		                                      MpfExeInfo->dMotPosition,
		                                      MpfExeInfo->Kmovpos);
	}

	return	AlarmCode;
}



/****************************************************************************************************/
/*																									*/
/*		���Ɍ��o���͈͊m�F����																	*/
/*																									*/
/****************************************************************************************************/
static ALMID_T	MpoleFindCheckMovingRange( MPFIND *MpFind, LONG dMotPos, KSGAIN Kmovpos )
{
	LONG		lwk;
	ALMID_T		AlarmCode;
	AlarmCode = NO_ALARM;

	MpFind->UnMpFindingTime++;

	MpFind->var.MovPos += dMotPos;

	if( MpFind->var.MovPos > MpFind->UnMpfMovePosP )
	{
		MpFind->UnMpfMovePosP = MpFind->var.MovPos;
	}
	else if( MpFind->var.MovPos < MpFind->UnMpfMovePosN )
	{
		MpFind->UnMpfMovePosN = MpFind->var.MovPos;
	}

	if( MpFind->UnMpfMovePosP >= -MpFind->UnMpfMovePosN )
	{
		lwk = MpFind->UnMpfMovePosP;
	}
	else
	{
		lwk = -MpFind->UnMpfMovePosN;
	}

	if( MpFind->conf.DetectRange <  MlibMulgain( lwk, Kmovpos ) )
	{
		/* A.C53 : ���Ɍ��o���͈̓I�[�o�[ */
		AlarmCode = ALM_PDET_MOVEOVER;
	}

	return	AlarmCode;
}



/****************************************************************************************************/
/*																									*/
/*		���Ɍ��o�����V�[�P���X																		*/
/*																									*/
/****************************************************************************************************/
ALMID_T	MpoleFindSequence( MPFIND *MpFind, MPFEXEINFO *MpfExeInfo, BOOL PdetReq, BOOL BaseEnable )
{
	ALMID_T		AlarmCode;
	AlarmCode = NO_ALARM;

	if( PdetReq == FALSE )
	{
		MpFind->var.Step = MPF_INIT;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		���Ɍ��o�����V�[�P���X																		*/
/*--------------------------------------------------------------------------------------------------*/
	switch( MpFind->var.Step )
	{
	/*----------------------------------------------------------------------------------------------*/
	case MPF_INIT:			/* Initial Step															*/
	/*----------------------------------------------------------------------------------------------*/
		if( (PdetReq & BaseEnable) == TRUE )
		{ /* ���Ɍ��o�V�[�P���X�J�n�v�� */

			/* ���Ɍ��o�V�[�P���X�����p�ϐ������� */
			MlibResetLongMemory( &MpFind->var, sizeof( MpFind->var ) >> 2 );
			/* ���j�^�p�ϐ������� */
			MpFind->UnMpFindingTime = 0;
			MpFind->UnMpfMovePosP = 0;
			MpFind->UnMpfMovePosN = 0;

			MpFind->var.Step = MPF_INIT2;
		}
		break;
	/*----------------------------------------------------------------------------------------------*/
		case MPF_INIT2:		/* Initial Step2														*/
	/*----------------------------------------------------------------------------------------------*/
			MpFind->var.Ivar64[0] = 0;
			MpFind->var.Ivar64[1] = 0;
			MpFind->var.OSFail = FALSE;
			MpFind->var.dAxisMaxTrqPlus = 0;
			MpFind->var.dAxisMaxTrqMinus = 0;
			MpFind->var.qAxisMaxTrqPlus = 0;
			MpFind->var.qAxisMaxTrqMinus = 0;

			MpFind->var.dAxisMaxSpdPlus = 0;
			MpFind->var.dAxisMaxSpdMinus = 0;
			MpFind->var.qAxisMaxSpdPlus = 0;
			MpFind->var.qAxisMaxSpdMinus = 0;

			MpFind->var.DegRev = 0;
			MpFind->var.DegFinal = 0;
			MpFind->var.AreaCheckStep = 0;
			MpFind->var.ReverseCtr = 0;
			MpFind->var.NoGood = 0;

			/* �I�t�Z�b�g���x(���x�w�ߊJ�n���̑��x) */
			MpFind->var.SpdOffset = MpfExeInfo->SpdFbMonitor;

			MpFind->var.Step = MPF_DRIVE;
			break;
	/*----------------------------------------------------------------------------------------------*/
	case MPF_DRIVE:			/* �^�]����																*/
	/*----------------------------------------------------------------------------------------------*/
		MpoleFindDrive( MpFind, MpfExeInfo->SpdFbMonitor, MpfExeInfo->TrefMonitor );
		AlarmCode = MpoleFindDetectAlarm(MpFind, MpfExeInfo);
		break;
	/*----------------------------------------------------------------------------------------------*/
	case MPF_CAL:			/* �ʑ��v�Z																*/
	/*----------------------------------------------------------------------------------------------*/
		AlarmCode = MpoleFindDetectAlarm(MpFind, MpfExeInfo);
		UpdatePhaseCalculation( MpFind );
		break;
	/*----------------------------------------------------------------------------------------------*/
	case MPF_STOP:			/* ��~�m�F																*/
	/*----------------------------------------------------------------------------------------------*/
		AlarmCode = MpoleFindDetectAlarm(MpFind, MpfExeInfo);
		StopMpoleFind( MpFind, MpfExeInfo->MotStopSts );
		break;
	/*----------------------------------------------------------------------------------------------*/
	case MPF_CONFIRM:		/* ����m�F																*/
	/*----------------------------------------------------------------------------------------------*/
		AlarmCode = MpoleFindDetectAlarm(MpFind, MpfExeInfo);
		MpFind->var.PhaseConfFlg = TRUE;		/* �ʑ��␳�l�m�F�̂��߃g���N����ɂ���t���O		*/
		if( FALSE != ConfirmMpoleFind( MpFind,
									   MpfExeInfo->Kphasepos,
									   MpfExeInfo->dMotPosition,
									   MpfExeInfo->SpdFbMonitor ) )
		{
			/* A.C54 : ���Ɍ��o�m�F�s�� */
			AlarmCode = ALM_PDET_NG;
		}
		break;
	/*----------------------------------------------------------------------------------------------*/
	case MPF_END:			/* �I������																*/
	/*----------------------------------------------------------------------------------------------*/
		MpFind->var.PhaseConfFlg = FALSE;
		MpFind->var.Ivar64[0] = 0;
		MpFind->var.Ivar64[1] = 0;
//		MpFind->var.Step = MPF_INIT;
		break;
	/*----------------------------------------------------------------------------------------------*/
	default:				/* �{�P�[�X�̓V�[�P���X�\�t�g�ُ�										*/
	/*----------------------------------------------------------------------------------------------*/
		MpFind->var.Step = MPF_INIT;

		/* A.C50 : ���Ɍ댟�o */
		AlarmCode = ALM_POLE;
		break;
	}

	return	AlarmCode;
}



/****************************************************************************************************/
/*																									*/
/*		���Ɍ��o�^�]����																			*/
/*																									*/
/****************************************************************************************************/
static	void	MpoleFindDrive( MPFIND *MpFind, LONG SpdFbki, LONG TrqRefo )
{
/*--------------------------------------------------------------------------------------------------*/
/*		�^�]�V�[�P���X��Ԋm�F																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( !MpFind->var.Fail )
	{
		MpFind->var.PastTime++;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�t�������x�I�[�o�[�`�F�b�N����																*/
/*--------------------------------------------------------------------------------------------------*/
	CheckMpFindFail( MpFind, SpdFbki );

/*--------------------------------------------------------------------------------------------------*/
/*		���x�w�ߍ쐬����																			*/
/*--------------------------------------------------------------------------------------------------*/
	MpFindLauSpdRef( MpFind, SpdFbki );

	if( !MpFind->var.RangeFix )
	{
/*--------------------------------------------------------------------------------------------------*/
/*		8���������S�̈�`�F�b�N																	*/
/*--------------------------------------------------------------------------------------------------*/
		switch( MpFind->var.RangeFixStep )
		{
		case PAREACHECK_000:
			if ( MpFind->var.PhaseChange )
			{
				/* �t�����莞�̏��� */
				MpFind->MpfPhaseOffset += PHASE_DEG180;
				MpFind->var.PhaseChange = FALSE;
				MpFind->var.RangeFixStep = PAREACHECK_001;
			}
			else if ( MpFind->var.PastTime == 0 )
			{
				if ( MpFind->var.AreaCheckStep == 7 )
				{
					/* ���x�ő储��ыt���ʒu�����܂��Ȉʒu���v�Z */
					MpFindCalPosAbout( MpFind );

					/* ��蒼���`�F�b�N */
					switch ( MpFind->var.NoGood )
					{
					case 0:
						/* �X�e�b�v1�I��� */
						MpFind->MpfPhaseOffset += MpFind->var.DegFinal + PHASE_DEG090;
						MpFind->var.RangeFix = TRUE;
						MpFind->var.Direction = FALSE;
						MpFind->var.AxisChange = FALSE;
						break;

					case 1:
						/* �Q�C�����グ�� */
						MpFind->var.KvGain++;
						MpFind->MpfPhaseOffset = 0;
						MpFind->var.RangeFixStep = PAREACHECK_000;
						MpFind->var.FailTimes++;						/* ���s�񐔂�1���₷ */
						MpFind->var.Step = MPF_INIT2;					/* ���������������蒼�� */
						break;
					}
				}
				else
				{
					MpFind->MpfPhaseOffset += PHASE_DEG045;
					MpFind->var.RangeFixStep = PAREACHECK_000;
					MpFind->var.AreaCheckStep++;
				}
			}
			break;

		case PAREACHECK_001:
			if ( MpFind->var.PastTime == 0 )
			{
				/* �t���ʒu�擾 */
				if ( MpFind->var.ReverseCtr <= 3 )
				{
					MpFind->var.DegReverse[MpFind->var.ReverseCtr] = PHASE_DEG045 * MpFind->var.AreaCheckStep;
				}
				MpFind->var.ReverseCtr++;

				if ( MpFind->var.AreaCheckStep == 7 )
				{
					/* ���x�ő储��ыt���ʒu�����܂��Ȉʒu���v�Z */
					MpFindCalPosAbout( MpFind );

					/* ��蒼���`�F�b�N */
					switch ( MpFind->var.NoGood )
					{
					case 0:
						/* �X�e�b�v1�I��� */
						MpFind->MpfPhaseOffset += MpFind->var.DegFinal - PHASE_DEG090;
						MpFind->var.RangeFix = TRUE;
						MpFind->var.Direction = FALSE;
						MpFind->var.AxisChange = FALSE;
						break;

					case 1:
						/* �Q�C�����グ�� */
						MpFind->var.KvGain++;
						MpFind->MpfPhaseOffset = 0;
						MpFind->var.RangeFixStep = PAREACHECK_000;
						MpFind->var.FailTimes++;						/* ���s�񐔂�1���₷ */
						MpFind->var.Step = MPF_INIT2;					/* ���������������蒼�� */
						break;
					}
				}
				else
				{
					MpFind->MpfPhaseOffset -= PHASE_DEG135;
					MpFind->var.RangeFixStep = PAREACHECK_000;
					MpFind->var.AreaCheckStep++;
				}
			}
			break;

		case PAREACHECK_002:
			if ( MpFind->var.PastTime == 0 )
			{
				MpFind->MpfPhaseOffset = 0;
				MpFind->var.KvGain--;									/* �Q�C���������� */
				MpFind->var.RangeFixStep = PAREACHECK_000;
				MpFind->MPJudgeMonitor[MpFind->var.FailTimes] = 5;		/* ���胂�j�^�Z�b�g */
				MpFind->var.FailTimes++;								/* ���s�񐔂�1���₷ */
				MpFind->var.Step = MPF_INIT2;							/* ���������������蒼�� */
			}
			break;

		case PAREACHECK_003:
			MpFind->MpfPhaseOffset = 0;
			MpFind->var.KvGain--;										/* �Q�C���������� */
			MpFind->var.RangeFixStep = PAREACHECK_000;
			MpFind->MPJudgeMonitor[MpFind->var.FailTimes] = 4;			/* ���胂�j�^�Z�b�g */
			MpFind->var.FailTimes++;									/* ���s�񐔂�1���₷ */
			MpFind->var.Step = MPF_INIT2;								/* ���������������蒼�� */
			break;

		default :
			MpFind->var.RangeFixStep = PAREACHECK_000;
			MpFind->MpfPhaseOffset = 0;
			break;
		}
	}
	else
	{
/*--------------------------------------------------------------------------------------------------*/
/*		���|�����������ɐ���p�x�i����															*/
/*--------------------------------------------------------------------------------------------------*/
		if ( MpFind->var.PhaseChange )
		{
			MpFind->MpfPhaseOffset += PHASE_DEG180;
			MpFind->var.Times = 0;
			MpFind->var.RangeFixStep = PAREACHECK_002;
			MpFind->var.RangeFix = FALSE;
			MpFind->var.PhaseChange = FALSE;
		}
		else
		{
			MpFindGetMaxTrqSpd( MpFind, SpdFbki, TrqRefo );

			if ( MpFind->var.PastTime == 0 )
			{
				if ( MpFind->var.AxisChange )
				{
					MpFind->MpfPhaseOffset -= PHASE_DEG090;
				}
				else
				{
					MpFind->var.Times++;
					JudgeMpFindErrSign( MpFind );

					if ( MpFind->var.Step != MPF_CAL )
					{
						switch ( MpFind->var.AxisErrSign )
						{
							case AXISERR_EQUAL :
							case AXISERR_MINUS :
								MpFind->MpfPhaseOffset += PHASE_DEG090 + (PHASE_DEG030 >> MpFind->var.Times);
								break;

							case AXISERR_PLUS :
								MpFind->MpfPhaseOffset += PHASE_DEG090 - (PHASE_DEG030 >> MpFind->var.Times);
								break;

							default :
								break;
						}
					}
				}
			}
		}
	}
	return;
}



/****************************************************************************************************/
/*																									*/
/*		�G���[���o : �t������уI�[�o�[�X�s�[�h������s���B											*/
/*																									*/
/****************************************************************************************************/
static	void	CheckMpFindFail( MPFIND *MpFind, LONG SpdFbki )
{
	LONG	work;
	LONG	OverSpeed;

	/* �t�����背�x���̐ݒ� */
	if ( MpFind->var.ReverseLevel )
	{
		/* �҂����Ԓ� */
		work = MpFind->conf.ReverseSpd2;
	}
	else
	{
		/* ���x�w�ߒ� */
		work = MpFind->conf.ReverseSpd;
	}

	/* �\�����背�x���̐ݒ� */
	if ( MpFind->var.OSFail )
	{
		/* �I�[�o�[�X�s�[�h�ɂ���������̃��x�� */
		OverSpeed = MpFind->conf.MaxSpd;
	}
	else
	{
		/* �ʏ펞�̃��x�� */
		OverSpeed = MpFind->conf.OverSpd;
	}

	/* �펞�I�[�o�[�X�s�[�h���� */
	if ( MlibLABS( SpdFbki ) >= OverSpeed )
	{
		MpFind->var.PastTime = MpFind->conf.WaitTime;
		MpFind->var.Fail = TRUE;
		MpFind->var.OSFail = TRUE;
		MpFind->var.ZeroTorque = TRUE;
	}

	/* �t���������łȂ����X�e�b�v1�̎��A�t�����o���s���B */
	if ( (!MpFind->var.Fail) && (!MpFind->var.RangeFix) )
	{
		if ( !MpFind->var.Direction )
		{
			/* ���������x�w�ߎ� */
			if ( (SpdFbki - MpFind->var.SpdOffset) < ( -work ) )
			{
				MpFind->var.Fail = TRUE;
				MpFind->var.PhaseChange = TRUE;
				MpFind->var.PastTime = MpFind->conf.WaitTime;
				MpFind->var.ClrIntegral = TRUE;
			}
		}
		else
		{
			/* ���������x�w�ߎ� */
			if ( (SpdFbki - MpFind->var.SpdOffset) > work )
			{
				MpFind->var.Fail = TRUE;
				MpFind->var.PhaseChange = TRUE;
				MpFind->var.PastTime = MpFind->conf.WaitTime;
				MpFind->var.ClrIntegral = TRUE;
			}
		}
	}
	return;
}



/****************************************************************************************************/
/*																									*/
/*		���x�w�ߍ쐬 : ���Ɍ��o���̉^�]�w�߂��쐬����B												*/
/*																									*/
/****************************************************************************************************/
static	void	MpFindLauSpdRef( MPFIND *MpFind, LONG SpdFbki )
{
	LONG	AbsSpdRef;

/*--------------------------------------------------------------------------------------------------*/
/*		�G���[���o���̏���																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( MpFind->var.Fail )
	{
		MpFind->MpFindSpdRef = 0;
		MpFind->var.InteglessON = FALSE;

		if( --MpFind->var.PastTime <= 0 )
		{
			MpFind->var.RefOutTime = 0;
			MpFind->var.Fail = FALSE;
			MpFind->var.RefOutStep = MPFLAU_ACC;
			MpFind->var.Direction = FALSE;
			MpFind->var.AxisChange = FALSE;
			MpFind->var.ZeroTorque = FALSE;
			MpFind->var.ReverseLevel = FALSE;
			MpFind->var.SpdOffset = SpdFbki;

			if ( MpFind->var.OSFail )
			{
				MpFind->var.RangeFixStep = PAREACHECK_003;
			}
		}
		return;
	}

	AbsSpdRef = MlibLABS( MpFind->MpFindSpdRef );

/*--------------------------------------------------------------------------------------------------*/
/*		���x�w�ߍ쐬�V�[�P���X																		*/
/*--------------------------------------------------------------------------------------------------*/
	switch( MpFind->var.RefOutStep )
	{
	/*----------------------------------------------------------------------------------------------*/
	case MPFLAU_INIT:		/* �҂����Ԍo��															*/
	/*----------------------------------------------------------------------------------------------*/
		MpFind->var.RefOutStep = MPFLAU_ACC;
		MpFind->var.RefOutTime = 0;
		MpFind->var.SpdOffset = SpdFbki;
		break;

	/*----------------------------------------------------------------------------------------------*/
	case MPFLAU_ACC:		/* �����w�ߍ쐬															*/
	/*----------------------------------------------------------------------------------------------*/
		AbsSpdRef = MlibLaufilter( MpFind->conf.MaxSpd, AbsSpdRef, MpFind->conf.dSpd );
		MpFind->var.RefOutTime++;
		if( MpFind->var.RefOutTime >= MpFind->conf.AccTime )
		{
			MpFind->var.RefOutStep = MPFLAU_CONST;
			MpFind->var.RefOutTime = 0;
		}
		break;

	/*----------------------------------------------------------------------------------------------*/
	case MPFLAU_CONST:		/* ��푬�x�w�ߍ쐬														*/
	/*----------------------------------------------------------------------------------------------*/
		AbsSpdRef = MpFind->conf.MaxSpd;
		MpFind->var.RefOutTime++;
		if( MpFind->var.RefOutTime >= MpFind->conf.ClmpTime )
		{
			MpFind->var.RefOutStep = MPFLAU_DEC;
			MpFind->var.RefOutTime = 0;
		}
		break;

	/*----------------------------------------------------------------------------------------------*/
	case MPFLAU_DEC:		/* �����w�ߍ쐬															*/
	/*----------------------------------------------------------------------------------------------*/
		AbsSpdRef = MlibLaufilter( 0, AbsSpdRef, MpFind->conf.dSpd );
		MpFind->var.RefOutTime++;
		if( MpFind->var.RefOutTime >= MpFind->conf.AccTime )
		{
			MpFind->var.RefOutStep = MPFLAU_WAIT;
			MpFind->var.RefOutTime = 0;
		}
		break;

	/*----------------------------------------------------------------------------------------------*/
	case MPFLAU_WAIT:		/* �҂����Ԍo��															*/
	/*----------------------------------------------------------------------------------------------*/
		AbsSpdRef = 0;
		MpFind->var.RefOutTime++;
		MpFind->var.ReverseLevel = TRUE;
		if ( MpFind->var.RefOutTime >= MpFind->conf.InteglessTime )
		{
			MpFind->var.InteglessON = TRUE;
		}

		if( MpFind->var.RefOutTime >= MpFind->conf.WaitTime )
		{
			if( MpFind->var.Direction )
			{
				MpFind->var.PastTime = 0;
				if( MpFind->var.AxisChange )
				{
					MpFind->var.AxisChange = FALSE;
				}
				else
				{
					MpFind->var.AxisChange = TRUE;
				}
			}
			if( MpFind->var.Direction )
			{
				MpFind->var.Direction = FALSE;
			}
			else
			{
				MpFind->var.Direction = TRUE;
			}
			MpFind->var.RefOutStep = MPFLAU_ACC;
			MpFind->var.RefOutTime = 0;
			MpFind->var.InteglessON = FALSE;
			MpFind->var.ReverseLevel = FALSE;
			MpFind->var.SpdOffset = SpdFbki;
		}
		break;

	/*----------------------------------------------------------------------------------------------*/
	default:
	/*----------------------------------------------------------------------------------------------*/
		AbsSpdRef = 0;
		break;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		���x�w�ߏo��																				*/
/*--------------------------------------------------------------------------------------------------*/
	if( MpFind->var.Direction )
	{
		MpFind->MpFindSpdRef = -AbsSpdRef;
	}
	else
	{
		MpFind->MpFindSpdRef = AbsSpdRef;
	}

	return;
}



/****************************************************************************************************/
/*																									*/
/*		��܂��Ȉʒu�v�Z�֐��F�t���ʒu�����܂��Ȉʒu���v�Z										*/
/*			�@�t���񐔃`�F�b�N																		*/
/*			�A�t���ʒu��萄��ʒu���v�Z															*/
/*																									*/
/****************************************************************************************************/
static	void	MpFindCalPosAbout( MPFIND *MpFind )
{
	BOOL	NG_flag;

	/* �t���񐔃`�F�b�N */
	if( MpFind->var.ReverseCtr >= 5 )
	{
		/* �Q�C�����グ�āA������x��蒼�� */
		MpFind->var.NoGood = TRUE;
		MpFind->MPJudgeMonitor[MpFind->var.FailTimes] = 1;			/* ���胂�j�^�Z�b�g */
		return;
	}

	/* �t���񐔃`�F�b�N */
	if( MpFind->var.ReverseCtr <= 1 )
	{
		/* �Q�C�����グ�āA������x��蒼�� */
		MpFind->var.NoGood = TRUE;
		MpFind->MPJudgeMonitor[MpFind->var.FailTimes] = 2;			/* ���胂�j�^�Z�b�g */
		return;
	}

	/* �t���ʒu���v�Z */
	NG_flag = FALSE;
	MpFindCalPos( &MpFind->var.DegRev, &MpFind->var.ReverseCtr,
													&MpFind->var.DegReverse[0], &NG_flag );
	/* ��蒼������ */
	if( NG_flag )
	{
		/* �Q�C�����グ�āA������x��蒼�� */
		MpFind->var.NoGood = TRUE;
		MpFind->MPJudgeMonitor[MpFind->var.FailTimes] = 3;			/* ���胂�j�^�Z�b�g */
		return;
	}
	MpFind->var.DegRev += PHASE_DEG180;

	MpFind->var.DegFinal = MpFind->var.DegRev;

	return;
}



/****************************************************************************************************/
/*																									*/
/*		�ʒu�v�Z�֐��F�ʒu���v�Z(MpFindCalPosAbout()�֐��Ŏg�p)										*/
/*																									*/
/****************************************************************************************************/
static	void	MpFindCalPos( LONG *DegCalc, LONG *Counter, LONG *Degree, BOOL *NoGood_flag )
{
	ULONG	lwork1, lwork2;
	USHORT	work[3];
	USHORT	i;

	*NoGood_flag = FALSE;
	lwork1 = lwork2 = 0;

	if( *Counter == 1 )
	{
		lwork1 = *Degree;
	}
	else if( *Counter == 2 )
	{
		lwork1 = (ULONG)*Degree + (ULONG)*(Degree + 1);
		work[0] = (USHORT)( *(Degree + 1) - *Degree );

		if( work[0] > (USHORT)PHASE_DEG180 )
		{
			lwork1 = (lwork1 - (ULONG)PHASE_DEG360) >> 1;
		}
		else if( work[0] < (USHORT)PHASE_DEG180 )
		{
			lwork1 = lwork1 >> 1;
		}
		else
		{
			/* �t�������ʑ���180deg����Ă����̂ŁANoGood_flag���Z�b�g */
			*NoGood_flag = TRUE;
		}
	}
	else if( *Counter == 3 )
	{
		lwork1 = (ULONG)*Degree + (ULONG)*(Degree + 1) + (ULONG)*(Degree + 2);
		work[0] = (USHORT)( *(Degree + 2) - *Degree );
		work[1] = (USHORT)( *(Degree + 1) - *Degree );
		work[2] = (USHORT)( *(Degree + 2) - *(Degree + 1) );

		if( work[0] <= (USHORT)PHASE_DEG135 )
		{
			lwork1 = lwork1 / 3;
		}
		else if( work[1] >= (USHORT)PHASE_DEG225 )
		{
			lwork1 = (lwork1 + (ULONG)PHASE_DEG360) / 3;
		}
		else if( work[2] >= (USHORT)PHASE_DEG225 )
		{
			lwork1 = (lwork1 + (ULONG)PHASE_DEG360 + (ULONG)PHASE_DEG360) / 3;
		}
		else
		{
			/* �t�������ʑ���135deg�͈̔͊O�ɑ��݂����̂ŁANoGood_flag���Z�b�g */
			*NoGood_flag = TRUE;
		}
	}
	else if( *Counter == 4 )
	{
		lwork1 = (ULONG)*Degree + (ULONG)*(Degree + 1) + (ULONG)*(Degree + 2) + (ULONG)*(Degree + 3);
		work[0] = (USHORT)( *(Degree + 1) - *Degree );
		work[1] = (USHORT)( *(Degree + 2) - *(Degree + 1) );
		work[2] = (USHORT)( *(Degree + 3) - *(Degree + 2) );

		lwork2 = 0;
		for( i = 0; i <= 2; i++ )
		{
			if( (work[i] != (USHORT)PHASE_DEG045) && (work[i] != (USHORT)PHASE_DEG225) )
			{
				/* �t�������ʑ���135deg�͈̔͊O�ɑ��݂����̂ŁANoGood_flag���Z�b�g */
				*NoGood_flag = TRUE;
				break;
			}
			else if( work[i] == (USHORT)PHASE_DEG225 )
			{
				lwork2 = ((ULONG)PHASE_DEG360 * (i+1));
			}
		}

		if( *NoGood_flag == FALSE )
		{
			lwork1 = (lwork1 + lwork2) >> 2;
		}
	}
	*DegCalc = lwork1;
	return;
}



/****************************************************************************************************/
/*																									*/
/*		�ő�g���N���ő呬�x�擾 : �^�]����d-q�����ꂼ��̍ő�g���N�ƍő呬�x���擾����B			*/
/*																									*/
/****************************************************************************************************/
static	void	MpFindGetMaxTrqSpd( MPFIND *MpFind, LONG SpdFbki, LONG TrqRefo )
{
	if( !MpFind->var.AxisChange )
	{
		/* q�� */
		if( !MpFind->var.Direction )
		{
			/* ���������x�w�ߎ� */
			if( MpFind->var.qAxisMaxTrqPlus < TrqRefo )
			{
				MpFind->var.qAxisMaxTrqPlus = TrqRefo;
			}
			if( MpFind->var.qAxisMaxSpdPlus < SpdFbki )
			{
				MpFind->var.qAxisMaxSpdPlus = SpdFbki;
			}
		}
		else
		{
			/* ���������x�w�ߎ� */
			if( MpFind->var.qAxisMaxTrqMinus > TrqRefo )
			{
				MpFind->var.qAxisMaxTrqMinus = TrqRefo;
			}
			if( MpFind->var.qAxisMaxSpdMinus > SpdFbki )
			{
				MpFind->var.qAxisMaxSpdMinus = SpdFbki;
			}
		}
	}
	else
	{
		/* d�� */
		if( !MpFind->var.Direction )
		{
			/* ���������x�w�ߎ� */
			if( MpFind->var.dAxisMaxTrqPlus < TrqRefo )
			{
				MpFind->var.dAxisMaxTrqPlus = TrqRefo;
			}
			if( MpFind->var.dAxisMaxSpdPlus < SpdFbki )
			{
				MpFind->var.dAxisMaxSpdPlus = SpdFbki;
			}
		}
		else
		{
			/* ���������x�w�ߎ� */
			if( MpFind->var.dAxisMaxTrqMinus > TrqRefo )
			{
				MpFind->var.dAxisMaxTrqMinus = TrqRefo;
			}
			if( MpFind->var.dAxisMaxSpdMinus > SpdFbki )
			{
				MpFind->var.dAxisMaxSpdMinus = SpdFbki;
			}
		}
	}
	return;
}



/****************************************************************************************************/
/*																									*/
/*		d-q������p�Z�o : d-q�����ꂼ��̍ő�g���N�̊֌W���炸��p�Z�o����B						*/
/*																									*/
/****************************************************************************************************/
static	void	JudgeMpFindErrSign( MPFIND *MpFind )
{
	LONG	lwork1, lwork2;
	LONG	lwork3, lwork4;
	LONG	kx;

	lwork1 = MlibLABS( MpFind->var.dAxisMaxTrqPlus - MpFind->var.qAxisMaxTrqPlus );
	lwork2 = MlibLABS( MpFind->var.dAxisMaxTrqMinus - MpFind->var.qAxisMaxTrqMinus );
	lwork3 = MlibLABS( MpFind->var.dAxisMaxSpdPlus - MpFind->var.qAxisMaxSpdPlus );
	lwork4 = MlibLABS( MpFind->var.dAxisMaxSpdMinus - MpFind->var.qAxisMaxSpdMinus );

	if( lwork1 == lwork2 )
	{
		if( lwork3 < lwork4 )
		{
			/* �������̗p */
			lwork1 = MlibLABS( MpFind->var.dAxisMaxTrqMinus );
			lwork2 = MlibLABS( MpFind->var.qAxisMaxTrqMinus );
			lwork3 = MlibLABS( MpFind->var.dAxisMaxSpdMinus );
			lwork4 = MlibLABS( MpFind->var.qAxisMaxSpdMinus );
		}
		else
		{
			/* �������̗p */
			lwork1 = MpFind->var.dAxisMaxTrqPlus;
			lwork2 = MpFind->var.qAxisMaxTrqPlus;
			lwork3 = MpFind->var.dAxisMaxSpdPlus;
			lwork4 = MpFind->var.qAxisMaxSpdPlus;
		}
	}
	else if( lwork1 < lwork2 )
	{
		/* �������̗p */
		lwork1 = MlibLABS( MpFind->var.dAxisMaxTrqMinus );
		lwork2 = MlibLABS( MpFind->var.qAxisMaxTrqMinus );
		lwork3 = MlibLABS( MpFind->var.dAxisMaxSpdMinus );
		lwork4 = MlibLABS( MpFind->var.qAxisMaxSpdMinus );
	}
	else
	{
		/* �������̗p */
		lwork1 = MpFind->var.dAxisMaxTrqPlus;
		lwork2 = MpFind->var.qAxisMaxTrqPlus;
		lwork3 = MpFind->var.dAxisMaxSpdPlus;
		lwork4 = MpFind->var.qAxisMaxSpdPlus;
	}

	if( lwork1 == lwork2 )
	{
		lwork2 = 16384;
		lwork1 = AXISERR_EQUAL;
	}
	else if( lwork1 > lwork2 )
	{
		kx = MlibScalKxgain( lwork2, 1, lwork1, NULL, 24 );
#if (FLOAT_USE==TRUE)
		lwork2 = 16384 * kx;
#else		
		lwork2 = MlibMulgain( 16384, kx );
#endif /* FLOAT_USE */
		lwork1 = AXISERR_MINUS;
	}
	else
	{
		kx = MlibScalKxgain( lwork1, 1, lwork2, NULL, 24 );
#if (FLOAT_USE==TRUE)
		lwork2 = 16384 * kx;
#else	
		lwork2 = MlibMulgain( 16384, kx );
#endif /* FLOAT_USE */

		lwork1 = AXISERR_PLUS;
	}

	if( lwork3 == lwork4 )
	{
		lwork4 = 16384;
		lwork3 = AXISERR_EQUAL;
	}
	else if( lwork3 > lwork4 )
	{
		kx = MlibScalKxgain( lwork4, 1, lwork3, NULL, 24 );
#if (FLOAT_USE==TRUE)
		lwork4 = 16384 * kx;
#else	
		lwork4 = MlibMulgain( 16384, kx );
#endif /* FLOAT_USE */
		lwork3 = AXISERR_PLUS;
	}
	else
	{
		kx = MlibScalKxgain( lwork3, 1, lwork4, NULL, 24 );
#if (FLOAT_USE==TRUE)
		lwork4 = 16384 * kx;
#else	
		lwork4 = MlibMulgain( 16384, kx );
#endif /* FLOAT_USE */
		lwork3 = AXISERR_MINUS;
	}

	if( MpFind->var.Times >= MpFind->conf.RepeatNum )
	{
		MpFind->MpFindSpdRef = 0;
		MpFind->var.AdjDegree = SINPHASE_045;
		MpFind->var.AdjSwpDeg = SINPHASE_045;
		MpFind->var.Step = MPF_CAL;
	}
	else
	{
		MpFind->var.dAxisMaxTrqPlus = 0;
		MpFind->var.qAxisMaxTrqPlus = 0;
		MpFind->var.dAxisMaxTrqMinus = 0;
		MpFind->var.qAxisMaxTrqMinus = 0;
		MpFind->var.dAxisMaxSpdPlus = 0;
		MpFind->var.qAxisMaxSpdPlus = 0;
		MpFind->var.dAxisMaxSpdMinus = 0;
		MpFind->var.qAxisMaxSpdMinus = 0;
	}

	if( lwork1 == lwork3 )
	{
		MpFind->var.AxisErrSign = (UCHAR)lwork1;
		MpFind->var.AxisErr = lwork2;
	}
	else if( lwork2 <= lwork4 )
	{
		MpFind->var.AxisErrSign = (UCHAR)lwork1;
		MpFind->var.AxisErr = lwork2;
	}
	else
	{
		MpFind->var.AxisErrSign = (UCHAR)lwork3;
		MpFind->var.AxisErr = lwork4;
	}
	return;
}



/****************************************************************************************************/
/*																									*/
/*		�ʑ��v�Z : �^�]�����œ����f�[�^����ʑ��Z�o���A�ʑ��X�V���s���B								*/
/*																									*/
/****************************************************************************************************/
static	void	UpdatePhaseCalculation( MPFIND *MpFind )
{
	LONG	lwk;

	MpFind->var.ClrIntegral = TRUE;
	MpFind->var.AdjSwpDeg >>= 1;

	if( MpFind->var.AxisErrSign != AXISERR_EQUAL )
	{
		lwk = ( MlibSins16( MpFind->var.AdjDegree, 0x11 ) << 14 ) /
				MlibSins16( MpFind->var.AdjDegree + SINPHASE_090, 0x11 );

		if( MpFind->var.AxisErr > MlibLABS( lwk ) )
		{
			MpFind->var.AdjDegree += MpFind->var.AdjSwpDeg;
		}
		else
		{
			MpFind->var.AdjDegree -= MpFind->var.AdjSwpDeg;
		}
	}

	if( (MpFind->var.AdjSwpDeg == 1) || (MpFind->var.AxisErrSign == AXISERR_EQUAL) )
	{
		lwk = MpFind->var.AdjDegree << 2;

		switch( MpFind->var.AxisErrSign )
		{
		case AXISERR_EQUAL :
			MpFind->MpfPhaseOffset += PHASE_DEG045;
			break;

		case AXISERR_MINUS :
			MpFind->MpfPhaseOffset += PHASE_DEG090 - (SHORT)lwk;
			break;

		case AXISERR_PLUS :
			MpFind->MpfPhaseOffset += (SHORT)lwk;
			break;

		default :
			break;
		}
		MpFind->var.Step = MPF_STOP;
	}
	return;
}



/****************************************************************************************************/
/*																									*/
/*		��~�m�F���� : ���Ɍ��o�̒�~�m�F�������s���B												*/
/*																									*/
/****************************************************************************************************/
static	void	StopMpoleFind( MPFIND *MpFind, BOOL MotStopSts )
{
	if( MotStopSts == TRUE )
	{
		/* ���Ɍ��o�ϐ������� */
		MlibResetLongMemory( &MpFind->var, sizeof( MpFind->var ) >> 2 );

		MpFind->var.RangeFix = TRUE;
		MpFind->var.Step = MPF_CONFIRM;
	}
	return;
}



/****************************************************************************************************/
/*																									*/
/*		����m�F����(�d�����b�N����) : ���Ɍ��o�̓���m�F�������s���B								*/
/*																									*/
/****************************************************************************************************/
static BOOL ConfirmMpoleFind( MPFIND *MpFind, KSGAIN Kphasepos, LONG dMotPos, LONG SpdFbki )
{
	BOOL pdet_err;
	BOOL WaitFinish;
	pdet_err = FALSE;
	WaitFinish = FALSE;

	/* ���͎w�ߍ쐬 */
	MpFind->var.TrqRefPhaConf = MpFindLauTrqRef( MpFind, &WaitFinish );

	switch( MpFind->var.ConfirmStep )
	{
		case MPFCSTEP_WAIT:
			/* ConfirmStep��PHSCHANGE, ADJUST�ȊO�̏ꍇ�A�I�[�o�[�X�s�[�h���`�F�b�N����B */
			if( MlibLABS( SpdFbki ) >= MpFind->conf.OverSpd )
			{
				pdet_err = TRUE;
			}
			/* ���͎w�ߍ쐬�����J�n�m�F */
			if( MpFind->var.RefOutStep == MPFTRQ_ACC )
			{
				MpFind->var.ConfirmStep = MPFCSTEP_PHSCHANGE;
			}
			break;


		case MPFCSTEP_PHSCHANGE:
			/* ConfirmStep��PHSCHANGE, ADJUST�̏ꍇ�A�ړ��������`�F�b�N����B */
			/* ����m�F�p�ʒu�쐬 */
			MpFind->var.MovPosPhaConf += dMotPos;
			if( MlibLABS(MpFind->var.MovPosPhaConf) >= MpFind->conf.ErrOKLevel )
			{
				pdet_err = TRUE;
			}
			/* ���o���ʈꎞ�ޔ� */
			MpFind->var.PhsOffsetCalRslt = MpFind->MpfPhaseOffset;
			/* �ʑ��ύX(d���ɓd���𗬂�) */
			MpFind->MpfPhaseOffset -= PHASE_DEG090;
			MpFind->var.ConfirmStep = MPFCSTEP_ADJUST;
			break;


		case MPFCSTEP_ADJUST:
			/* ConfirmStep��PHSCHANGE, ADJUST�̏ꍇ�A�ړ��������`�F�b�N����B */
			/* ����m�F�p�ʒu�쐬 */
			MpFind->var.MovPosPhaConf += dMotPos;
			if( MlibLABS(MpFind->var.MovPosPhaConf) >= MpFind->conf.ErrOKLevel )
			{
				pdet_err = TRUE;
			}
			/* �ʑ����Œ肷�邽�߁A�������������I�t�Z�b�g�𒲐�����B */
#if (FLOAT_USE==TRUE)
			MpFind->MpfPhaseOffset -= ( (float)dMotPos * Kphasepos );
#else
			MpFind->MpfPhaseOffset -= MlibMulgainNolim( dMotPos, Kphasepos );
#endif /* FLOAT_USE */

			/* WAIT�I���^�C�~���O�m�F */
			if( WaitFinish == TRUE )
			{
				if( MpFind->var.RefOutStep == MPFTRQ_DEC )
				{ /* �葬����I�� */
					MpFind->var.ConfirmStep = MPFCSTEP_PHSSTORE;
				}
				else if( MpFind->var.RefOutStep == MPFTRQ_END )
				{ /* ��������I�� */
					MpFind->var.ConfirmStep = MPFCSTEP_PHSREVERT;
				}
				else
				{ /* ��������I�� */
					; /* ��������I�����͏����Ȃ� */
				}
			}
			break;


		case MPFCSTEP_PHSSTORE:
			/* ConfirmStep��PHSCHANGE, ADJUST�ȊO�̏ꍇ�A�I�[�o�[�X�s�[�h���`�F�b�N����B */
			if( MlibLABS( SpdFbki ) >= MpFind->conf.OverSpd )
			{
				pdet_err = TRUE;
			}
			/* �ŏI�ʑ���ۑ�����B */
			MpFind->var.PhsOffsetComp = MpFind->MpfPhaseOffset;
			/* �ʑ����Œ肷�邽�߁A�������������I�t�Z�b�g�𒲐�����B */
#if (FLOAT_USE==TRUE)
			MpFind->MpfPhaseOffset -= ( (float)dMotPos * Kphasepos );
#else
			MpFind->MpfPhaseOffset -= MlibMulgainNolim( dMotPos, Kphasepos );
#endif /* FLOAT_USE */
			MpFind->var.ConfirmStep = MPFCSTEP_ADJUST;
			break;


		case MPFCSTEP_PHSREVERT:
			/* ConfirmStep��PHSCHANGE, ADJUST�ȊO�̏ꍇ�A�I�[�o�[�X�s�[�h���`�F�b�N����B */
			if( MlibLABS( SpdFbki ) >= MpFind->conf.OverSpd )
			{
				pdet_err = TRUE;
			}
			/* �ʑ������ɖ߂�(q���ɓd���𗬂�) */
			MpFind->MpfPhaseOffset = MpFind->var.PhsOffsetComp + PHASE_DEG090;
			MpFind->var.ConfirmStep = MPFCSTEP_WAIT;
			MpFind->var.Step = MPF_END;
			break;

		default:
			break;
	}

	/* ���o���ʂ̕␳�ʌv�Z(for Monitor) */
	MpFind->var.PhaseOffsetCalComp = MpFind->MpfPhaseOffset - MpFind->var.PhsOffsetCalRslt;

	return pdet_err;
}



/****************************************************************************************************/
/*																									*/
/*		���͎w�ߍ쐬 : ���Ɍ��o�m�F���̐��͎w�߂��쐬����B											*/
/*																									*/
/****************************************************************************************************/
static	LONG	MpFindLauTrqRef( MPFIND *MpFind, BOOL *WaitFinish )
{
	LONG	TrqRef;

	/* ���ϐ��� */
	TrqRef = MpFind->var.TrqRefPhaConf;
/*--------------------------------------------------------------------------------------------------*/
/*		���͎w�ߍ쐬�V�[�P���X																		*/
/*--------------------------------------------------------------------------------------------------*/
	switch( MpFind->var.RefOutStep )
	{
	/*----------------------------------------------------------------------------------------------*/
	case MPFTRQ_INIT:	/* �҂����Ԍo��																*/
	/*----------------------------------------------------------------------------------------------*/
		TrqRef = 0;
		MpFind->var.ClrIntegral = TRUE;
		MpFind->var.RefOutStep = MPFTRQ_ACC;
		MpFind->var.RefOutTime = 0;
		break;

	/*----------------------------------------------------------------------------------------------*/
	case MPFTRQ_ACC:	/* ���͎w�ߑ���																*/
	/*----------------------------------------------------------------------------------------------*/
		TrqRef = MlibLaufilter( MpFind->conf.MaxTrq, TrqRef, MpFind->conf.dTrq );
		MpFind->var.RefOutTime++;
		if( MpFind->var.RefOutTime >= MpFind->conf.IncTime )
		{
			MpFind->var.RefOutStep = MPFTRQ_CONST;
			MpFind->var.RefOutTime = 0;
			*WaitFinish = TRUE;
		}
		break;

	/*----------------------------------------------------------------------------------------------*/
	case MPFTRQ_CONST:	/* 	���͎w�߈��															*/
	/*----------------------------------------------------------------------------------------------*/
		TrqRef = MpFind->conf.MaxTrq;
		MpFind->var.RefOutTime++;
		if( MpFind->var.RefOutTime >= MpFind->conf.ConstTime )
		{
			MpFind->var.RefOutStep = MPFTRQ_DEC;
			MpFind->var.RefOutTime = 0;
			*WaitFinish = TRUE;
		}
		break;

	/*----------------------------------------------------------------------------------------------*/
	case MPFTRQ_DEC:	/* ���͎w�ߌ���																*/
	/*----------------------------------------------------------------------------------------------*/
		TrqRef = MlibLaufilter( 0, TrqRef, MpFind->conf.dTrq );
		MpFind->var.RefOutTime++;
		if( MpFind->var.RefOutTime >= MpFind->conf.IncTime )
		{
			MpFind->var.RefOutStep = MPFTRQ_END;
			MpFind->var.RefOutTime = 0;
			*WaitFinish = TRUE;
		}
		break;

	/*----------------------------------------------------------------------------------------------*/
	case MPFTRQ_END:	/* �҂����Ԍo��																*/
	/*----------------------------------------------------------------------------------------------*/
		MpFind->var.ClrIntegral = TRUE;
		TrqRef = 0;
		MpFind->var.RefOutStep = MPFTRQ_ACC;
		MpFind->var.RefOutTime = 0;
		break;

	/*----------------------------------------------------------------------------------------------*/
	default :
	/*----------------------------------------------------------------------------------------------*/
		break;
	}

	return( MlibLimitul(TrqRef, MpFind->conf.MaxTrq, -MpFind->conf.MaxTrq) );
}





/****************************************************************************************************/
/*																									*/
/*		���Ɍ��o�p�w�ߑ��x�p�����[�^�v�Z															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		��]�^�̃p�����[�^																			*/
/*			Prm.pdetmaxspd_r	: Pn493	���Ɍ��o�w�ߑ��x			[min-1]							*/
/*																									*/
/*		���j�A�̃p�����[�^																			*/
/*			Prm.pdetmaxspd		: Pn485	���Ɍ��o�w�ߑ��x 			[mm/s]							*/
/*																									*/
/*		���ʂ̃p�����[�^																			*/
/*			Prm.pdetmaxt		: Pn486 ���Ɍ��o�w�߉���������		[ms]							*/
/*			Prm.pdetclmpt		: Pn487 ���Ɍ��o�w�߈�葬����		[ms]							*/
/*			Prm.pdetwait		: Pn488	���Ɍ��o�w�ߑ҂�����		[ms]							*/
/*			Prm.pdetgett		: Pn489	���Ɍ��o�f�[�^���n�ߎ���	[ms]							*/
/*			Prm.pdetinteglesst	: Pn492 ���Ɍ��o�ϕ����X�J�n����	[%]								*/
/*			Prm.pdetrevlvspdref	: Pn499 ���Ɍ��o���x�w�ߎ��t�����x��[%]								*/
/*			Prm.pdetrevlvwait	: Pn49A ���Ɍ��o�҂����Ԏ��t�����x��[%]								*/
/*			Prm.pdetoslevel		: Pn49B ���Ɍ��o�ߑ��x���x��		[%]								*/
/*																									*/
/****************************************************************************************************/
void	MpoleFind_CalcSpeedCommandPrm( MPFIND *MpFind, MENCV *MencV,
    	                               BPRMDAT *Bprm, MPFIND_UPRM *PdetPrm )
{
	LONG		work;

	if( MencV->AxisMotType == MOTTYPE_ROTARY )
	{ /* ��]�^���[�^ */
		if( MencV->LowSpdMotor )
		{
			work = PdetPrm->pdetmaxspd_r;
		}
		else
		{
			work = 10 * PdetPrm->pdetmaxspd_r;
		}
		MpFind->conf.MaxSpd = BprmSpeedLevelCal( Bprm, work, 0 );
	}
	else if( MencV->AxisMotType == MOTTYPE_LINEAR )
	{ /* ���j�A���[�^ */
		MpFind->conf.MaxSpd = BprmSpeedLevelCal( Bprm, (PdetPrm->pdetmaxspd * 10), 0 );
	}

	MpFind->conf.ReverseSpd		= MpFind->conf.MaxSpd * PdetPrm->pdetrevlvspdref / 100;
	MpFind->conf.ReverseSpd2	= MpFind->conf.MaxSpd * PdetPrm->pdetrevlvwait / 100;
	MpFind->conf.OverSpd		= MpFind->conf.MaxSpd * PdetPrm->pdetoslevel / 100;

	MpFind->conf.dSpd			= MpFind->conf.MaxSpd * Bprm->MPFCycleUs_MR / 1000 / PdetPrm->pdetmaxt;
	MpFind->conf.AccTime		= PdetPrm->pdetmaxt * 1000 / Bprm->MPFCycleUs_MR;
	MpFind->conf.ClmpTime		= PdetPrm->pdetclmpt * 1000 / Bprm->MPFCycleUs_MR;
	MpFind->conf.WaitTime		= PdetPrm->pdetwait * 1000 / Bprm->MPFCycleUs_MR;
	MpFind->conf.InteglessTime	= MpFind->conf.WaitTime * PdetPrm->pdetinteglesst / 100;

	MpFind->conf.MaxTrq		= BprmTorqueLevelCal( Bprm, (PdetPrm->pdetmaxtrq * 100), 0 );
	MpFind->conf.dTrq		= MpFind->conf.MaxTrq * Bprm->MPFCycleUs_MR / 1000 / PdetPrm->pdetinctime;
	MpFind->conf.IncTime	= PdetPrm->pdetinctime * 1000 / Bprm->MPFCycleUs_MR;
	MpFind->conf.ConstTime	= PdetPrm->pdetconsttime * 1000 / Bprm->MPFCycleUs_MR;

}



/****************************************************************************************************/
/*																									*/
/*		���Ɍ��o�p���x����p�����[�^�v�Z															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*			Prm.pdetloophz		: Pn481 ���Ɍ��o�����x���[�v�Q�C��		[0.1Hz]						*/
/*			Prm.pdetpitime		: Pn482 ���Ɍ��o�����x���[�v�ϕ�����	[0.01ms]					*/
/*			Prm.pdetjrate		: Pn490	���Ɍ��o���׃��x��				[%]							*/
/*			Prm.pdetintegless	: Pn491 ���Ɍ��o�ϕ����X�Q�C��			[0.01%]						*/
/*			���Ɍ��o���́A���Ɍ��o�p���׏d�ʔ���g�p����											*/
/*			���Ɍ��o���́Ai-P����g�p�s��															*/
/*																									*/
/****************************************************************************************************/
void	MpoleFind_CalcSpeedControlPrm( MPFIND *MpFind,
    	                               USHORT trqfil11, BPRMDAT *Bprm, MPFIND_UPRM *PdetPrm  )
{
#if (FLOAT_USE==TRUE)

	float	fwk;

	fwk = Bprm->Kvx * (float)((100 + PdetPrm->pdetjrate) * PdetPrm->pdetloophz) / 1000.0F;
	MpFind->conf.Kv = fwk;

	fwk = 1.0F * (float)Bprm->MPFCycleUs_SC / (float)(10 * PdetPrm->pdetpitime);
	MpFind->conf.Ki = fwk;

	fwk = (float)PdetPrm->pdetintegless * 10.0f * (float)PdetPrm->pdetpitime / 10000.0f;
	MpFind->conf.InteglessGain = fwk / (float)Bprm->MPFCycleUs_SC;

#else
	LONG		s;
	LONG		wk;

	MpFind->conf.Kv = MlibScalKskxkx( Bprm->Kvx, (100 + PdetPrm->pdetjrate) * PdetPrm->pdetloophz, 1000, NULL, 24 );
	MpFind->conf.Ki = MlibScalKxgain( 1, Bprm->MPFCycleUs_SC, 10*PdetPrm->pdetpitime, NULL, 24 );

	wk = MlibScalKxgain( PdetPrm->pdetintegless, 10*PdetPrm->pdetpitime, 10000, &s, 0 );
	MpFind->conf.InteglessGain = MlibPcalKxgain( wk, 1, Bprm->MPFCycleUs_SC, &s, 24 );
#endif /* FLOAT_USE */

	MpFind->conf.Klpf = MlibPcalKf1gain( 10 * trqfil11, Bprm->MPFCycleUs_SC, 0 );

}



/****************************************************************************************************/
/*																									*/
/*		���Ɍ��o�G���[���x���v�Z																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		��]�^�̏ꍇ																				*/
/*			Prm.pdetdistok_r: Pn494 ���Ɍ��o���͈�		[0.001rev]								*/
/*			Prm.pdetdegreeok: Pn498 ���Ɍ��o�m�F���i�͈�	[deg(�d�C�p)]							*/
/*						  pdetdegreeokpole * (2^BitNo)												*/
/*			ErrOKLevel = ------------------------------		[pulse]									*/
/*									pole / 2 														*/
/*																									*/
/*		���j�A�̏ꍇ																				*/
/*			Prm.pdetdistok	: Pn48E ���Ɍ��o���͈�								[mm]			*/
/*			Prm.pdetdegreeok: Pn498 ���Ɍ��o�m�F���i�͈�							[deg(�d�C�p)]	*/
/*						  pdetdegreeokpole * (ppitch/10*10^-3 * 2) * (2^BitNo)						*/
/*			ErrOKLevel = ------------------------------------------------------		[pulse]			*/
/*									360 * ScalePitch/100*10^-6										*/
/*																									*/
/****************************************************************************************************/
void	MpoleFind_CalcErrorLevel( MPFIND *MpFind, MENCV *MencV,
    	                          MENCPRM *MencP, BPRMDAT *Bprm, MPFIND_UPRM *PdetPrm  )
{
	LONG		kx, sx;
	LONG		work;

	if( MencV->AxisMotType == MOTTYPE_LINEAR )
	{ /* ���j�A���[�^ */
		MpFind->conf.DetectRange = 100 * PdetPrm->pdetdistok;
		/* �d�C�p���p���X���ɕϊ�����B */
		work = MencV->NormScalePitch;
		kx = MlibScalKxgain( PdetPrm->pdetdegreeok, MencP->ppitch, 360, &sx, 0 );
		kx = MlibPcalKxgain( kx, (1 << MencP->bit_dp.b.l), work, &sx, 0 );
		kx = MlibPcalKxgain( kx, MencV->NormScaleExp, 1, &sx, 0 );
		kx = MlibPcalKxgain( kx, 200000, 1, &sx, -30 );
		MpFind->conf.ErrOKLevel = kx;
	}
	else
	{ /* ��]�^���[�^ */
		if( MencV->LowSpdMotor )
		{
			MpFind->conf.DetectRange = PdetPrm->pdetdistok_r / 10;
		}
		else
		{
			MpFind->conf.DetectRange = PdetPrm->pdetdistok_r;
		}
		/* �d�C�p���p���X���ɕϊ�����B */
		MpFind->conf.ErrOKLevel =
			MlibScalKxgain( PdetPrm->pdetdegreeok, Bprm->MotPulse, (180 * MencP->oslv_pol.b.h), NULL, -30 );
	}
}




/***************************************** end of file **********************************************/
