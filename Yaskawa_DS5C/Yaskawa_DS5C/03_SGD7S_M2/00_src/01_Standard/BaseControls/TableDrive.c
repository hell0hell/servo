/****************************************************************************************************/
/*																									*/
/*																									*/
/*		TableDrive.c : �e�[�u���^�]�p	���W���[����`												*/
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
/*	Note	:	����	2011.02.18	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "TableDrive.h"
#include "BaseCmnStruct.h"
#include "PosManager.h"
#include "ModelControl.h"
#include "DataTraceManager.h"
#include "MLib.h"


/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
/* void	TblDriveInitialize( TBLDRIVE *TblDrive );													*/
/* void	TblDriveGetTraceRegData( TBLDRIVE *TblDrive, USHORT DataSize, USHORT SampIntv,				*/
/*															ULONG DataSampIntv, ULONG DataNum );	*/
/* void	TblDriveExecute( TBLDRIVE *TblDrive, TBLCHK_SV_STS *TblChkSts, TRCHNDL *TrcHndl,			*/
/*													POS_MNG_HNDL *PosManager, MFCTRL *MFControl );	*/
/* PRM_RSLT	TblDriveCalculateParameter( TBLDRIVE *TblDrive, LONG XfbPulse1 );						*/
/* PRM_RSLT	TblDrivePrepareProc( TBLDRIVE *TblDrive );												*/
/* PRM_RSLT	TblDriveStartRequest( TBLDRIVE *TblDrive, LONG TblDir );								*/
/* PRM_RSLT	TblDriveSetStopRequest( TBLDRIVE *TblDrive );											*/
/****************************************************************************************************/
static	void tblDriveOutputRef( TBLDRIVE *TblDrive );
static	void tblDriveIPolatePosRef( TBLDRIVE *TblDrive );
static	void tblDriveSwitchPcmdFil( TBLDRIVE *TblDrive, POS_MNG_HNDL *PosManager,
																MFCTRL *MFControl, BOOL Switch );
static	void tblDriveSwitchEgear( TBLDRIVE *TblDrive, POS_MNG_HNDL *PosManager, BOOL Switch );
static	void tblDriveSetTrqFilter( TBLDRIVE *TblDrive, LONG TrqInputPos );


/****************************************************************************************************/
/*																									*/
/*		�e�[�u���^�]����������																		*/
/*																									*/
/****************************************************************************************************/
void	TblDriveInitialize( TBLDRIVE *TblDrive, USHORT *BufferAddress , TRCHNDL *TrcHndl )
{
	/* Reset Table Drive Variables */
	MlibResetLongMemory( &(TblDrive->var), sizeof(TblDrive->var)/4 );
	/* Set Table Refference Buffer Pointer */
	TblDrive->conf.TblBuffer = BufferAddress;
	TblDrive->TrcHndl = TrcHndl;
}


/****************************************************************************************************/
/*																									*/
/*		�e�[�u���^�]���s�p�����[�^�v�Z����															*/
/*																									*/
/****************************************************************************************************/
void	TblDriveGetTraceRegData( TBLDRIVE *TblDrive, USHORT DataSize, USHORT SampIntv,
																ULONG DataSampIntv, ULONG DataNum )
{
	TblDrive->OpeReg.TblDriveDataSize = DataSize;
	TblDrive->OpeReg.TblDriveSampIntv = SampIntv;
	TblDrive->conf.DataSampIntv = DataSampIntv;
	TblDrive->conf.TblDataNum = DataNum;
}


/****************************************************************************************************/
/*																									*/
/*		�e�[�u���^�]���s�p�����[�^�v�Z����															*/
/*																									*/
/****************************************************************************************************/
PRM_RSLT	TblDriveCalculateParameter( TBLDRIVE *TblDrive, LONG XfbPulse1 )
{
	LONG	x;

	/* �e�[�u���^�]�p�����[�^�v�Z�����t���O�N���A */
	TblDrive->conf.TblPrmOk = FALSE;

/*--------------------------------------------------------------------------------------------------*/
/*		�e�[�u���^�]���[�h�ݒ�																		*/
/*--------------------------------------------------------------------------------------------------*/
	switch( TblDrive->OpeReg.TblMode )
	{
	case 0x01:
		TblDrive->conf.CtrlMode = CTRLMODE_TRQ;					/* �g���N����						*/
		TblDrive->conf.RefOutPx = &TblDrive->var.TrqRef;		/* �w�ߏo�̓|�C���^					*/
		break;
	case 0x02:
		TblDrive->conf.CtrlMode = CTRLMODE_SPD;					/* ���x����							*/
		TblDrive->conf.RefOutPx = &TblDrive->var.SpdRef;		/* �w�ߏo�̓|�C���^					*/
		break;
	case 0x03:
		TblDrive->conf.CtrlMode = CTRLMODE_POS;					/* �ʒu����FYEC����				*/
		TblDrive->conf.RefOutPx = &TblDrive->var.dPosRef;		/* �w�ߏo�̓|�C���^					*/
		break;
	case 0x04:
		TblDrive->conf.CtrlMode = CTRLMODE_POS;					/* �ʒu����FNCT����				*/
		TblDrive->conf.RefOutPx = &TblDrive->var.dPosRef;		/* �w�ߏo�̓|�C���^					*/
		return( PRM_RSLT_CONDITION_ERR );						/* �����u(NCT���Ή�)				*/
	case 0x05:
		TblDrive->conf.CtrlMode = CTRLMODE_POS;					/* �ʒu����F�g���N�O���w��			*/
		TblDrive->conf.RefOutPx = &TblDrive->var.TrqRef;		/* �w�ߏo�̓|�C���^					*/
		break;
	default:
		TblDrive->conf.CtrlMode = CTRLMODE_POS;					/* �f�t�H���g						*/
		TblDrive->conf.RefOutPx = &TblDrive->var.dPosRef;		/* �w�ߏo�̓|�C���^					*/
		return( PRM_RSLT_CONDITION_ERR );						/* �ُ탊�^�[��						*/
	}
	TblDrive->conf.TblMode = (UCHAR)TblDrive->OpeReg.TblMode;

/*--------------------------------------------------------------------------------------------------*/
/*		�e�[�u���^�]�F�w�߃^�C�v, �w�ߕ�ԉ�, �w�ߕ�ԃV�t�g���̐ݒ�								*/
/*--------------------------------------------------------------------------------------------------*/
	/* �w�ߕ�ԉ� */
	TblDrive->conf.RefIpNum = TblDrive->OpeReg.TblDriveSampIntv;
	if( (TblDrive->conf.TblMode == 0x03) || (TblDrive->conf.TblMode == 0x04) )
	{
		/* 2�̗ݏ�`�F�b�N */
		x = MlibSrhbiton( TblDrive->conf.RefIpNum, SRH_FROM_LSB );
		if( (x < 0) || (TblDrive->conf.RefIpNum != (ULONG)(1 << x)) )
		{
			x = -1;
		}

		/* ��ԉ񐔐ݒ� */
		if( TblDrive->conf.RefIpNum <= 1 )
		{
			/* �ʒu�w�߁F��Ԗ��� */
			TblDrive->conf.RefType = TBLREFTYPE_POSIP1;
		}
		else if( x >= 0 )
		{
			/* �ʒu�w�߁F��ԉ� == 2^n */
			TblDrive->conf.RefType = TBLREFTYPE_POSIP2;
			/* �w�ߕ�ԃV�t�g�� */
			TblDrive->conf.RefIpSft = (UCHAR)x;
		}
		else
		{
			/* �ʒu�w�߁F��ԉ� != 2^n */
			TblDrive->conf.RefType = TBLREFTYPE_POSIP3;
		}
	}
	else
	{
		/* ���x�w�߁^�g���N�w�� */
		TblDrive->conf.RefType = TBLREFTYPE_NOTPOS;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�e�[�u���^�]�F���[�^�p���X��,�}���`�^�[�����~�b�g�̐ݒ�										*/
/*--------------------------------------------------------------------------------------------------*/
	/* ���[�^�p���X�� [p/rev,p/mm] */
	TblDrive->conf.MotPulse = XfbPulse1;
	/* �}���`�^�[�����~�b�g [rev,mm] */
	TblDrive->conf.MtnLimit = TblDrive->OpeReg.TblLimit;

/*--------------------------------------------------------------------------------------------------*/
/*		�e�[�u���^�]�F�@�\�I��																		*/
/*--------------------------------------------------------------------------------------------------*/
	/* �g���N���͈ʒu */
	TblDrive->conf.TrqInputPos = (TblDrive->OpeReg.TblMode == 0x05) ? 
										(UCHAR)TblDrive->OpeReg.TblFuncSel : 0;
	/* �������X���䖳�� */
	TblDrive->conf.TuneLessDisable =
		(TblDrive->OpeReg.TblFuncSel & TBLFUNCBIT_TUNELESS)? TRUE : FALSE;
	/* �d�q�M�A�䖳�� */
	TblDrive->conf.EGearDisable =
		(TblDrive->OpeReg.TblFuncSel & TBLFUNCBIT_EGEAR)? TRUE : FALSE;
	/* �ʒu�w�߃t�B���^���� */
	TblDrive->conf.PcmdFilDisable =
		(TblDrive->OpeReg.TblFuncSel & TBLFUNCBIT_PCMDFIL)? TRUE : FALSE;

/*--------------------------------------------------------------------------------------------------*/
/*		�e�[�u���^�]�p�����[�^�v�Z����																*/
/*--------------------------------------------------------------------------------------------------*/
	/* �e�[�u���^�]�p�����[�^OK�Z�b�g */
	TblDrive->conf.TblPrmOk = TRUE;

	/* �p�����[�^�v�Z�������^�[�� */
	return( PRM_RSLT_SUCCESS );
}


/****************************************************************************************************/
/*																									*/
/*		�e�[�u���^�]���s����																		*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		�e�[�u���^�]�w�ߏo�̓}�N��																	*/
/*--------------------------------------------------------------------------------------------------*/
static	void tblDriveOutputRef( TBLDRIVE *TblDrive )
{
	USHORT	Idx;
	LONG	TblRef;

	Idx = TblDrive->var.TblIndx;

	if( (TblDrive->OpeReg.TblDriveDataSize == TBLREFLENGTH_31BIT_IO)
		|| (TblDrive->OpeReg.TblDriveDataSize == TBLREFLENGTH_32BIT) )
	{ /* LONG Data */
		TblRef = ((LONG *)TblDrive->conf.TblBuffer)[Idx] * TblDrive->conf.KoutDir;
	}
	else if( (TblDrive->OpeReg.TblDriveDataSize == TBLREFLENGTH_15BIT_IO)
			|| (TblDrive->OpeReg.TblDriveDataSize == TBLREFLENGTH_16BIT) )
	{ /* SHORT Data */
		TblRef = ((SHORT*)TblDrive->conf.TblBuffer)[Idx] * TblDrive->conf.KoutDir;
	}
	else /* TBLREFLENGTH_NOTDEF and Others */
	{ /* Data Setting Error (SHORT) */
		TblRef = ((SHORT*)TblDrive->conf.TblBuffer)[Idx] * TblDrive->conf.KoutDir;
	}

	if( TblDrive->conf.RefType == TBLREFTYPE_NOTPOS )
	{ /* �w�߃^�C�v�F���x/�g���N�w�� */
		TblRef = MlibLIMITUL( TblRef, 0x01000000, -0x01000000 );
		/* ���x/�g���N�w�ߏo�� */
		*TblDrive->conf.RefOutPx = TblRef;
	}
	else
	{ /* �w�߃^�C�v�F�ʒu�w�� */
		/* �ʒu�w�ߕ�ԉ��Z���� */
		TblDrive->var.InPosRef = TblRef;
		/* �e�[�u���^�]�ʒu�w�ߕ�ԏ��� */
		tblDriveIPolatePosRef( TblDrive );
	}
}


/****************************************************************************************************/
/*																									*/
/*		�e�[�u���^�]���s����֐�																	*/
/*																									*/
/****************************************************************************************************/
void	TblDriveExecute( TBLDRIVE *TblDrive, TBLCHK_SV_STS *TblChkSts, 
													POS_MNG_HNDL *PosManager, MFCTRL *MFControl )
{

/*--------------------------------------------------------------------------------------------------*/
/*		�e�[�u���^�]���s�w�߃`�F�b�N																*/
/*--------------------------------------------------------------------------------------------------*/
	if( TblDrive->var.TblCmd == TBLCMD_NOCMD )
	{
		TblDrive->var.TblStep = 0;
		TblDrive->var.TrqRef  = 0;
		TblDrive->var.SpdRef  = 0;
		TblDrive->var.dPosRef = 0;
		TblDrive->var.TblZmd.dw = 0x0000;
		return;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�e�[�u���^�]���s�V�[�P���X����																*/
/*--------------------------------------------------------------------------------------------------*/
	switch( TblDrive->var.TblStep )
	{
	case 0: /* Initial Step */
		TblDrive->var.TrqRef  = 0;
		TblDrive->var.SpdRef  = 0;
		TblDrive->var.dPosRef = 0;
		TblDrive->var.MotMtn  = 0;
		TblDrive->var.MotPos  = 0;
		TblDrive->var.TblIndx = 0;
		TblDrive->var.TblStep = 1;
		TblDrive->var.TblZmd.dw = 0x0000;

		/* ZeroClamp Control */
		TblDrive->var.TblZmd.b.zm = CTRLMODE_ZCLMP;
		break;

	case 1: /* Wait Base Enable */
	case 2: /* ZeroClamp and Wait Start Table Drive */
		if( TblDrive->var.TblCmd == TBLCMD_TABLEDRIVE )
		{
			TblDrive->var.TblStep = 3;
			TblDrive->var.TblIndx = 0;
			TblDrive->var.TblSamp = 0;

			if( TblDrive->var.WithTrcReq )
			{ /* �g���[�X�������s�v������ */
				DtrcSetTrgPointToZero(  TblDrive->TrcHndl );
			}

			/* �g���N�w�߃t�B���^���� */
			tblDriveSetTrqFilter( TblDrive, TblDrive->conf.TrqInputPos );
			/* �d�q�M������ */
			tblDriveSwitchEgear( TblDrive, PosManager, !(TblDrive->conf.EGearDisable) );
			/* �ʒu�w�߃t�B���^���� */
			tblDriveSwitchPcmdFil( TblDrive, PosManager, MFControl, !(TblDrive->conf.PcmdFilDisable) );

			TblDrive->var.ErrState = TBLCMD_STS_INIT;
		}
		break;

	case 3: /* Table Drive */
		/*------------------------------------------------------------------------------------------*/
		/*	�A���[�������`�F�b�N����																*/
		/*------------------------------------------------------------------------------------------*/
		if ( TblChkSts->AlmFlag )
		{
			TblDrive->var.ErrState = TBLCMD_STS_ALARM;
		}
		/*------------------------------------------------------------------------------------------*/
		/*	HWBB��ԃ`�F�b�N����																	*/
		/*------------------------------------------------------------------------------------------*/
		else if ( TblChkSts->HwbbSts )
		{
			TblDrive->var.ErrState = TBLCMD_STS_HWBB;
		}
		/*------------------------------------------------------------------------------------------*/
		/*	���H�d���I�t�`�F�b�N����																*/
		/*------------------------------------------------------------------------------------------*/
		else if ( !(TblChkSts->MainPowerOn) )
		{
			TblDrive->var.ErrState = TBLCMD_STS_POWEROFF;
		}
		/*------------------------------------------------------------------------------------------*/
		/*	�I�[�o�g���x���`�F�b�N����																*/
		/*------------------------------------------------------------------------------------------*/
		else if ( TblChkSts->OverTrvlSts )
		{
			TblDrive->var.ErrState = TBLCMD_STS_OT;
		}
		/*------------------------------------------------------------------------------------------*/
		/*	���[�^�ʓd�`�F�b�N����																	*/
		/*------------------------------------------------------------------------------------------*/
		else if ( !(TblChkSts->BeComplete) )
		{
			TblDrive->var.ErrState = TBLCMD_STS_BB;
		}
		/*------------------------------------------------------------------------------------------*/
		/*	�g���N�����`�F�b�N����																	*/
		/*------------------------------------------------------------------------------------------*/
		else if ( TblChkSts->TrqClamp )
		{
			TblDrive->var.ErrState = TBLCMD_STS_TRQLMT;
		}
		/*------------------------------------------------------------------------------------------*/
		/*	�}���`�^�[�����~�b�g�`�F�b�N����														*/
		/*------------------------------------------------------------------------------------------*/
		if( TblDrive->conf.MtnLimit > 0 )
		{
			/* �}���`�^�[�����Z */
			TblDrive->var.MotPos += TblChkSts->dMotPos;
			if( TblDrive->var.MotPos >= TblDrive->conf.MotPulse )
			{
				TblDrive->var.MotMtn += 1;
				TblDrive->var.MotPos -= TblDrive->conf.MotPulse;
			}
			else if( TblDrive->var.MotPos <= -TblDrive->conf.MotPulse )
			{
				TblDrive->var.MotMtn -= 1;
				TblDrive->var.MotPos += TblDrive->conf.MotPulse;
			}
			if( (TblDrive->var.MotMtn >= TblDrive->conf.MtnLimit)
				|| (TblDrive->var.MotMtn <= -TblDrive->conf.MtnLimit) )
			{
				/* Table Drive Error Step */
				TblDrive->var.TblStep = 5;
				/* Clear Reference Output */
				*TblDrive->conf.RefOutPx = 0;
				/* TableDriveState = MturnErr */
				TblDrive->OpeReg.TblState = TBLCMD_STS_POSLMT;
				/* ZeroClamp Control */
				TblDrive->var.TblZmd.b.zm = CTRLMODE_ZCLMP;
				break;
			}
		}
		/*------------------------------------------------------------------------------------------*/
		/*	�e�[�u���^�]�w�ߍ쐬����																*/
		/*------------------------------------------------------------------------------------------*/
		if( TblDrive->var.WithTrcReq )
		{
			if( (TblDrive->var.TblSamp == 0)
				|| (TblDrive->var.TblSamp > (TblDrive->OpeReg.TblDriveSampIntv - TblDrive->conf.DataSampIntv)) )
			{
				/* Set Trace Variable Function */
				DtrcSetTrcVar( TblDrive->TrcHndl );
			}
		}
		/*------------------------------------------------------------------------------------------*/
		if( TblDrive->var.TblSamp == 0 )
		{
			/* �e�[�u���^�]�I���`�F�b�N */
			if( TblDrive->var.TblIndx >= TblDrive->conf.TblDataNum )
			{
				TblDrive->var.TblStep = 4;					/* Table Drive End Step					*/
				*TblDrive->conf.RefOutPx = 0;				/* Clear Reference Output				*/
				TblDrive->var.TblZmd.b.zm = CTRLMODE_ZCLMP;	/* ZeroClamp Control					*/
				break;
			}
			/* �e�[�u���^�]�w�ߏo�͏��� */
			tblDriveOutputRef( TblDrive );

			/* �f�[�^�g���[�X���s���� */
			if( TblDrive->var.WithTrcReq )
			{
				/* Data Trace Procedure */
				DtrcProcedure( TblDrive->TrcHndl, TblDrive->var.TblIndx );

				/* Reset Average Variable */
				DtrcResetAverageVar( TblDrive->TrcHndl );
			}

			/* �e�[�u���^�]�C���f�N�X�X�V */
			TblDrive->var.TblIndx++;
		}
		else if( TblDrive->conf.RefType != TBLREFTYPE_NOTPOS )
		{ /* �w�߃^�C�v�`�F�b�N */
			/* �e�[�u���^�]�ʒu�w�ߕ�ԏ��� */
			tblDriveIPolatePosRef( TblDrive );
		}
		/*------------------------------------------------------------------------------------------*/
		TblDrive->var.TblSamp++;
		if( TblDrive->var.TblSamp >= TblDrive->OpeReg.TblDriveSampIntv )
		{
			TblDrive->var.TblSamp = 0;
		}
		/*------------------------------------------------------------------------------------------*/
		TblDrive->OpeReg.TblState =TBLCMD_STS_RUN;			/* TableDriveState = Execute			*/
		TblDrive->var.TblZmd.b.zm = CTRLMODE_ZNONE;			/* NoneZero Control						*/
		break;

	case 4: /* Table Drive End Step */
		/* Clear Reference Output */
		*TblDrive->conf.RefOutPx = 0;
		/* �g���N�w�߃t�B���^���� */
		tblDriveSetTrqFilter( TblDrive, FALSE );
		/* �d�q�M���L�� */
		tblDriveSwitchEgear( TblDrive, PosManager, TRUE );
		/* �ʒu�w�߃t�B���^�L�� */
		tblDriveSwitchPcmdFil( TblDrive, PosManager, MFControl, TRUE );
		if ( TblDrive->var.ErrState != 0 )
		{
			/* TableDriveState = End(ErrState) */
			TblDrive->OpeReg.TblState = TblDrive->var.ErrState;
		}
		else
		{
			/* TableDriveState = End */
			TblDrive->OpeReg.TblState = TBLCMD_STS_FINISH;
		}
		TblDrive->var.TblCmd  = TBLCMD_TBLDRV_END;			/* �e�[�u���^�]����I��					*/
		TblDrive->var.TblZmd.b.zm = CTRLMODE_ZCLMP;			/* ZeroClamp Control					*/
		TblDrive->var.TblStep = 2;
		break;

	case 5: /* Table Drive Error Step */
		/* Clear Reference Output */
		*TblDrive->conf.RefOutPx = 0;
		/* �g���N�w�߃t�B���^���� */
		tblDriveSetTrqFilter( TblDrive, FALSE );
		/* �d�q�M���L�� */
		tblDriveSwitchEgear( TblDrive, PosManager, TRUE );
		/* �ʒu�w�߃t�B���^�L�� */
		tblDriveSwitchPcmdFil( TblDrive, PosManager, MFControl, TRUE );

		TblDrive->var.TblCmd  = TBLCMD_TBLDRV_ERR;			/* �e�[�u���^�]�ُ�I��					*/
		TblDrive->var.TblStep = 6;
		break;

	case 6: /* Table Drive Error Step */
		if( (TblChkSts->AlmFlag == TRUE) || (TblChkSts->BeComplete == FALSE) )
		{ /* �A���[������ || BaseBlock */
			/* NoneZero Control */
			TblDrive->var.TblZmd.b.zm = CTRLMODE_ZNONE;
		}
		else
		{
			/* ZeroClamp Control */
			TblDrive->var.TblZmd.b.zm = CTRLMODE_ZCLMP;
		}
		break;

	default: /* Default */
		break;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�[�����䃂�[�h�w�ߏ���																		*/
/*--------------------------------------------------------------------------------------------------*/
	TblDrive->var.TblZmd.b.zmchg  = TblDrive->var.TblZmd.b.zm ^ TblDrive->var.TblZmd.b.zmlast;
	TblDrive->var.TblZmd.b.zmlast = TblDrive->var.TblZmd.b.zm;

	return;
}


/****************************************************************************************************/
/*																									*/
/*		�e�[�u���^�]�ʒu�w�ߕ�ԏ���																*/
/*																									*/
/****************************************************************************************************/
static	void tblDriveIPolatePosRef( TBLDRIVE *TblDrive )
{
/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu�w�ߕ�� : ����																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( TblDrive->conf.RefType == TBLREFTYPE_POSIP1 )
	{
		TblDrive->var.dPosRef = TblDrive->var.InPosRef;
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu�w�ߕ�� : ��ԉ� == 2^n																*/
/*--------------------------------------------------------------------------------------------------*/
	else if( TblDrive->conf.RefType == TBLREFTYPE_POSIP2 )
	{
		if( TblDrive->var.TblSamp == 0 )
		{
			TblDrive->var.IxPosRef = (TblDrive->var.InPosRef >> TblDrive->conf.RefIpSft);
			TblDrive->var.IxPosRem =
				TblDrive->var.InPosRef - (TblDrive->var.IxPosRef << TblDrive->conf.RefIpSft);
			if( TblDrive->var.InPosRef < 0 )
			{
				TblDrive->var.IxPosRef += 1;
				TblDrive->var.IxPosRem -= TblDrive->conf.RefIpNum;
			}
			TblDrive->var.WkPosRem = 0;
		}
	/*------------------------------------------------------------------------------------------*/
		if( TblDrive->var.InPosRef >= 0 )
		{ /* ���̏ꍇ */
			TblDrive->var.WkPosRem += TblDrive->var.IxPosRem;
			if( TblDrive->var.WkPosRem > 0 )
			{
				TblDrive->var.dPosRef = TblDrive->var.IxPosRef + 1;
				TblDrive->var.WkPosRem -= TblDrive->conf.RefIpNum;
			}
			else
			{
				TblDrive->var.dPosRef = TblDrive->var.IxPosRef;
			}
		}
	/*------------------------------------------------------------------------------------------*/
		else
		{ /* ���̏ꍇ */
			TblDrive->var.WkPosRem += TblDrive->var.IxPosRem;
			if( TblDrive->var.WkPosRem < 0 )
			{
				TblDrive->var.dPosRef = TblDrive->var.IxPosRef - 1;
				TblDrive->var.WkPosRem += TblDrive->conf.RefIpNum;
			}
			else
			{
				TblDrive->var.dPosRef = TblDrive->var.IxPosRef;
			}
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu�w�ߕ�� : ��ԉ� != 2^n																*/
/*--------------------------------------------------------------------------------------------------*/
	else
	{
		if( TblDrive->var.TblSamp == 0 )
		{
			TblDrive->var.IxPosRef = TblDrive->var.InPosRef / TblDrive->conf.RefIpNum;
			TblDrive->var.IxPosRem =
				TblDrive->var.InPosRef - (TblDrive->var.IxPosRef * TblDrive->conf.RefIpNum);
			TblDrive->var.WkPosRem = 0;
		}
	/*------------------------------------------------------------------------------------------*/
		if( TblDrive->var.InPosRef >= 0 )
		{ /* ���̏ꍇ */
			TblDrive->var.WkPosRem += TblDrive->var.IxPosRem;
			if( TblDrive->var.WkPosRem > 0 )
			{
				TblDrive->var.dPosRef = TblDrive->var.IxPosRef + 1;
				TblDrive->var.WkPosRem -= TblDrive->conf.RefIpNum;
			}
			else
			{
				TblDrive->var.dPosRef = TblDrive->var.IxPosRef;
			}
		}
	/*------------------------------------------------------------------------------------------*/
		else
		{ /* ���̏ꍇ */
			TblDrive->var.WkPosRem += TblDrive->var.IxPosRem;
			if( TblDrive->var.WkPosRem < 0 )
			{
				TblDrive->var.dPosRef = TblDrive->var.IxPosRef - 1;
				TblDrive->var.WkPosRem += TblDrive->conf.RefIpNum;
			}
			else
			{
				TblDrive->var.dPosRef = TblDrive->var.IxPosRef;
			}
		}
	}
	return;
}


/****************************************************************************************************/
/*																									*/
/*		�e�[�u���^�]�g���N�w�߃t�B���^����(�L���^����)												*/
/*																									*/
/****************************************************************************************************/
static	void tblDriveSetTrqFilter( TBLDRIVE *TblDrive, LONG TrqInputPos )
{
	if( TblDrive->conf.CtrlMode == CTRLMODE_TRQ )
	{
		switch( TrqInputPos )
		{
		case 0x00:								/* �Ȃ�												*/
		case 0x01:								/* �g���N�w�߃t�B���^�O								*/
		default :
			TblDrive->var.TrqFilDisable = FALSE;	/* �g���N�w�߃t�B���^�L��						*/
			TblDrive->var.NotchLpassDisable = FALSE;/* �m�b�`�����[�p�X�t�B���^�L��					*/
//			BpiEnableCPUTorqueFilter( );		
//			KpiEnableMicroTorqueFilter( );			/* �}�C�N���g���N�w�߃t�B���^�L��				*/
			break;
		case 0x02:								/* ���[�p�X�t�B���^��A�m�b�`�t�B���^�O				*/
			TblDrive->var.TrqFilDisable = TRUE;		/* �g���N�w�߃t�B���^����						*/
			TblDrive->var.NotchLpassDisable = FALSE;/* �m�b�`�����[�p�X�t�B���^�L��					*/
//			BpiDisableCPUTorqueFilter( );			/* CPU�g���N�w�߃t�B���^����					*/
//			KpiEnableMicroTorqueFilter( );			/* �}�C�N���g���N�w�߃t�B���^�L��				*/
			break;
		case 0x03:								/* �g���N�w�߃t�B���^��								*/
			TblDrive->var.TrqFilDisable = TRUE;		/* �g���N�w�߃t�B���^����						*/
			TblDrive->var.NotchLpassDisable = TRUE;	/* �m�b�`�����[�p�X�t�B���^����					*/
//			BpiDisableCPUTorqueFilter( );			/* CPU�g���N�w�߃t�B���^����					*/
//			KpiDisableMicroTorqueFilter( );			/* �}�C�N���g���N�w�߃t�B���^����				*/
			break;
		}
	}
	else if( TblDrive->conf.CtrlMode == CTRLMODE_POS )
	{
		/* �g���N�O���w�ߓ��͈ʒu�I�� */
		TblDrive->var.DisTrqInTiming = TrqInputPos;
	}
	return;
}


/****************************************************************************************************/
/*																									*/
/*		�e�[�u���^�]�d�q�M������(�L���^����)														*/
/*																									*/
/****************************************************************************************************/
static	void tblDriveSwitchEgear( TBLDRIVE *TblDrive, POS_MNG_HNDL *PosManager, BOOL Switch )
{
	if( TblDrive->conf.CtrlMode == CTRLMODE_POS )
	{
		BpiEgearFuncSwitch( PosManager, Switch );
	}
}


/****************************************************************************************************/
/*																									*/
/*		�e�[�u���^�]�ʒu�w�߃t�B���^����(�L���^����)												*/
/*																									*/
/****************************************************************************************************/
static	void tblDriveSwitchPcmdFil( TBLDRIVE *TblDrive, POS_MNG_HNDL *PosManager,
																MFCTRL *MFControl, BOOL Switch )
{
	if( TblDrive->conf.CtrlMode == CTRLMODE_POS )
	{
		BpiPcmdFilSwitch( PosManager, Switch );
		MfcSetMfcCtrlSwitch( MFControl, Switch );
	}
}


/****************************************************************************************************/
/*																									*/
/*		�e�[�u���^�]�X�^�[�g�pAPI																	*/
/*																									*/
/****************************************************************************************************/
PRM_RSLT	TblDrivePrepareProc( TBLDRIVE *TblDrive )
{
/*--------------------------------------------------------------------------------------------------*/
/*		�e�[�u���^�]�����J�n�`�F�b�N																*/
/*--------------------------------------------------------------------------------------------------*/
	if( TblDrive->var.TblCmd != TBLCMD_NOCMD )
	{
		return( PRM_RSLT_CONDITION_ERR );
	}
#if 0 /* �g���[�X�ƕ������邽�ߎg�p���Ȃ��悤�݌v���� */
	else if( TrcExeV.TrcCmd != TRCCMD_NOCMD )
	{
		return( PRM_RSLT_NOACCRIGHT_ERR );
	}
	else if( TblDrive->conf.TblPrmOk != TRUE )
	{
		return( PRM_RSLT_NOACCRIGHT_ERR );
	}
	else if( TrcExeP.TrcPrmOk != TRUE )
	{
		return( PRM_RSLT_NOACCRIGHT_ERR );
	}
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		�e�[�u���^�]�����J�n�w��																	*/
/*--------------------------------------------------------------------------------------------------*/
	/* �f�[�^�g���[�X��p�ϐ����Z�b�g���� */
//	LpxResetDataTraceVarX( );

	/* �e�[�u���^�]�����J�n�w�� */
	TblDrive->var.TblCmd = TBLCMD_TABLESTART;

	/* 2011.02.28 Y.Oka �e�[�u���^�]���g���[�X�������A���Ƃ��� */
	TblDrive->var.WithTrcReq = TRUE;

	return( PRM_RSLT_SUCCESS );
}


/****************************************************************************************************/
/*																									*/
/*		�e�[�u���^�]���s�J�n�v��API																	*/
/*																									*/
/****************************************************************************************************/
PRM_RSLT	TblDriveStartRequest( TBLDRIVE *TblDrive, LONG TblDir )
{
	LONG	KoutDir;

/*--------------------------------------------------------------------------------------------------*/
/*		�e�[�u���^�]���s�J�n�`�F�b�N																*/
/*--------------------------------------------------------------------------------------------------*/
	if( TblDrive->conf.TblPrmOk != TRUE )
	{
		return( PRM_RSLT_CONDITION_ERR );
	}
#if 0 /* �g���[�X�ƕ������邽�ߎg�p���Ȃ��悤�݌v���� */
	else if( TrcExeP.TrcPrmOk != TRUE )
	{
		return( REGIF_DAMTHERR2 );
	}
	else if( (TblDrive->var.TblCmd != TBLCMD_TABLESTART) && (TblDrive->var.TblCmd != TBLCMD_TBLDRV_END) )
	{
		return( REGIF_DAMTHERR2 );
	}
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		�e�[�u���^�]�w�ߏo�̓Q�C���̌v�Z(���K���Q�C���{�w�ߕ���)									*/
/*--------------------------------------------------------------------------------------------------*/
	KoutDir = (TblDir >= 0)? 1 : -1;					/* �e�[�u���^�]�w�ߕ���						*/
	if( TblDrive->conf.RefType == TBLREFTYPE_NOTPOS )	/* �w�߃^�C�v�F���x or �g���N				*/
	{
		if( (TblDrive->OpeReg.TblDriveDataSize != TBLREFLENGTH_31BIT_IO)
			&& (TblDrive->OpeReg.TblDriveDataSize != TBLREFLENGTH_32BIT) )
		{ /* SHORT Data */
			KoutDir = KoutDir << 10;					/* 16384 --> 2^24 : 2^10					*/
		}
	}
	TblDrive->conf.KoutDir = KoutDir;					/* �e�[�u���^�]�w�ߏo�̓Q�C��				*/
/*--------------------------------------------------------------------------------------------------*/
/*		�e�[�u���^�]���s�J�n�w�ߏ���																*/
/*--------------------------------------------------------------------------------------------------*/
#if 0 /* �g���[�X�ƕ������邽�ߎg�p���Ȃ��悤�݌v���� */
	if( TrcReg.TrcMode.b.h == 0x00 )					/* �w�߃e�[�u���ی샂�[�h					*/
	{
		TrcExeP.TrcObj[0].TrcExe |= 0x10;				/* �g���[�XCH0���s�֎~						*/
	}
	else												/* �w�߃e�[�u���㏑�����[�h					*/
	{
		TrcExeP.TrcObj[0].TrcExe &= 0x0F;				/* �g���[�XCH0���s����						*/
	}
#endif
	TblDrive->var.TblCmd = TBLCMD_TABLEDRIVE;			/* �e�[�u���^�]���s�J�n�w��					*/
/*--------------------------------------------------------------------------------------------------*/
	return( PRM_RSLT_SUCCESS );
}


/****************************************************************************************************/
/*																									*/
/*		�e�[�u���^�]�X�g�b�v�pAPI																	*/
/*																									*/
/****************************************************************************************************/
PRM_RSLT	TblDriveSetStopRequest( TBLDRIVE *TblDrive )
{
	TblDrive->var.TblCmd = TBLCMD_NOCMD;

	/* 2011.02.28 Y.Oka �e�[�u���^�]���g���[�X�������A���Ƃ��� */
	TblDrive->var.WithTrcReq = FALSE;

	return( PRM_RSLT_SUCCESS );
}


/****************************************************************************************************/
/*																									*/
/*		�e�[�u���^�]���䃂�[�h�擾�pAPI																*/
/*																									*/
/****************************************************************************************************/
USHORT	TblDriveGetControlMode( TBLDRIVE *TblDrive )
{
	return( TblDrive->conf.CtrlMode );
}


/****************************************************************************************************/
/*																									*/
/*		�e�[�u���^�]�����g���[�X���s�v���ݒ�pAPI													*/
/*																									*/
/****************************************************************************************************/
void	TblDriveSetWithTrcReq( TBLDRIVE *TblDrive, BOOL Switch )
{
	if( Switch == TRUE )
	{ /* �����g���[�X���s�v������ */
		TblDrive->var.WithTrcReq = TRUE;
	}
	else
	{ /* �����g���[�X���s�v���Ȃ� */
		TblDrive->var.WithTrcReq = FALSE;
	}
}


/***************************************** end of file **********************************************/
