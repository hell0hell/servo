/****************************************************************************
  Description   : Set Tuning Less Level (Fn200)
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corporation. 
                  Copyright (c) 2010 All Rights Reserved

  Project       : INGRAM

----------------------------------------------------------------------------
  Changes		:
	Name   Date          Description
----------------------------------------------------------------------------
	Y.Oka   2011.01.31   created

****************************************************************************/
#include "FnSetTuneLessLvl.h"
#include "MercuryGlobals.h"
#include "KLib.h"
#include "PnPrmListTbl.h"


/****************************************************************************************************/
/* Set Tuning Less Level (Fn200)																	*/
/****************************************************************************************************/
static PRM_RSLT	setTLLvlChkCondition( TUNELESS_CTRL *TuneLessCtrl );
static PRM_RSLT	setTLLvlUpdateInEeprom( REGIF_CONFIG_T *RegCfg );
static void		setTLLvlUndoRamGain( REGIF_CONFIG_T *RegCfg );
static PRM_RSLT	setTLLvlSetLevel( REG_ACC_T *AccessPrm, USHORT GainLevel, USHORT JratLevel, USHORT DetLevel );



/****************************************************************************************************/
/*																									*/
/*		Fn200 : Set Tuning Less Level																*/
/*																									*/
/****************************************************************************************************/
/* -------- Fn200 Set Tuning Less Level Begin	-------- */
PRM_RSLT	FnEnterSetTuneLessLevel( FUNEXE *FunExe, AXIS_HANDLE *Axis )
{
	PRM_RSLT		rc;
	TUNELESS_CTRL	*TuneLessCtrl;
	PRMDATA			*Prm;

	TuneLessCtrl = &(Axis->BaseControls->TuneLessCtrl);
	Prm = Axis->UniPrms->Prm;
	rc = PRM_RSLT_SUCCESS;

	/* Initialize Register Data */
	RpiRegSetTuneLessLevel( Prm->DatLevel, TuneLessCtrl );

	/* Check Execution Condition */
	rc = setTLLvlChkCondition( TuneLessCtrl );

	/* ���[�^���X�e�X�g���[�h�̏ꍇ�͓��炸�ɏI������ *//* <S1B7> */
	if( Axis->MotorLessTest->MotorLessTestMode == TRUE )
	{
		return PRM_RSLT_CONDITION_ERR;
	}

	/* Fn���[�h�����I���Ȃ� */
	FunExe->HoldFnMode = TRUE;

	return(rc);
}

/* -------- Fn200 Set Tuning Less Level End	-------- */
void	FnLeaveSetTuneLessLevel( FUNEXE *FunExe, AXIS_HANDLE *Axis )
{
	REGIF_CONFIG_T	*RegCfg;
	RegCfg = Axis->RegManager->hPnReg;

	/* EEPROM�f�[�^�ŕύX�p�����[�^�����ɖ߂� */
	setTLLvlUndoRamGain( RegCfg );

	/* Fn200���s���t���OOFF */
	Axis->BaseControls->TuneLessCtrl.var.TuningFuncRun = FALSE;

	/* Fn���[�h�����I���ݒ� */
	FunExe->HoldFnMode = FALSE;
}

/* -------- Fn200 Set Tuning Less Level Execution -------- */
PRM_RSLT	FnExecuteSetTuneLessLevel( FUNEXE *FunExe, AXIS_HANDLE *Axis )
{
	PRM_RSLT		rc;
	ULONG			ulwork;
	REG_ACC_T		AccessPrm;
	TUNELESS_CTRL	*TuneLessCtrl;
	PRMDATA			*Prm;
	REGIF_CONFIG_T	*RegCfg;

	TuneLessCtrl = &(Axis->BaseControls->TuneLessCtrl);
	Prm = Axis->UniPrms->Prm;
	RegCfg = Axis->RegManager->hPnReg;
	rc = PRM_RSLT_SUCCESS;

	/* RAM�������ݗp���W�X�^���ݒ� */
	AccessPrm.MemAddr = 0;
	AccessPrm.hRegCfg = RegCfg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

	/* Set Auto Notch Status in Register */
	RpiSetRegSpatANotchState( Axis->ANotchSeq );

	switch( FunExe->State )
	{
	case DAT_SEQ_INIT:
		/* Fn200���s���t���OON */
		TuneLessCtrl->var.TuningFuncRun = TRUE;
		FunExe->State = DAT_SEQ_TUNING;
		break;

	case DAT_SEQ_TUNING:	/* �Q�C�����x���ύX����	*/
		switch( FunExe->CmdCode )
		{
		case FCMD_UP:		/* �`���[�j���O���x���lUP		*/
			if( TuneLessCtrl->OpeReg.DatGnLevel < DAT_GAINLEVEL_MAX )
			{
				TuneLessCtrl->OpeReg.DatGnLevel++;
			}
			else
			{
				TuneLessCtrl->OpeReg.DatGnLevel = DAT_GAINLEVEL_MAX;
			}
			/*------------------------------------------------------------------------------------------*/
			/*	�`���[�j���O���x���ݒ�																	*/
			/*------------------------------------------------------------------------------------------*/
			setTLLvlSetLevel( &(AccessPrm),
							  TuneLessCtrl->OpeReg.DatGnLevel,
							  TuneLessCtrl->OpeReg.DatJratLevel,
							  Prm->DatLevel );

			/*------------------------------------------------------------------------------------------*/
			/*	�m�b�`�ݒ�Ǎ���																		*/
			/*------------------------------------------------------------------------------------------*/
			ulwork = (ULONG)Prm->tfuncsw;

			/*------------------------------------------------------------------------------------------*/
			/*	�����ݒ�m�b�`��������																	*/
			/*------------------------------------------------------------------------------------------*/
			if( Prm->anotchsw & 0x0F00 )						/* �����ݒ�m�b�`�𒊏o					*/
			{
				ulwork &= 0x0000F0FF;							/* ��2�m�b�`OFF							*/
			}
			RpiResetStatusANotch1( Axis->ANotchSeq );			/* �֘A�X�e�[�^�X���Z�b�g				*/
			RpiResetStatusANotch2( Axis->ANotchSeq );			/* �֘A�X�e�[�^�X���Z�b�g				*/
//			rc = RpiPrmWriteRamForFn( &pndef_tfuncsw, (USHORT)ulwork, pfexe->App.Hmsg );
			TunePrmWriteRam( &AccessPrm, &pndef_tfuncsw, (LONG)ulwork, 2 );

			/*------------------------------------------------------------------------------------------*/
			/*	�g���N�t�B���^�v�Z																		*/
			/*------------------------------------------------------------------------------------------*/
			RpiANotchTrqFilCalc( Axis->ANotchSeq, Axis->ANotch, RegCfg, Prm );

			/*------------------------------------------------------------------------------------------*/
			/*	�U�����x���N���A																		*/
			/*------------------------------------------------------------------------------------------*/
			DetVibResetMaxMinSpeedError( Axis->DetVib );
			break;

		case FCMD_DOWN:		/* �Q�C�����x���lDOWN		*/
			if( TuneLessCtrl->OpeReg.DatGnLevel > DAT_GAINLEVEL_MIN )
			{
				TuneLessCtrl->OpeReg.DatGnLevel--;
			}
			else
			{
				TuneLessCtrl->OpeReg.DatGnLevel = DAT_GAINLEVEL_MIN;
			}
			/*------------------------------------------------------------------------------------------*/
			/*	�`���[�j���O���x���ݒ�																	*/
			/*------------------------------------------------------------------------------------------*/
			setTLLvlSetLevel( &(AccessPrm),
							  TuneLessCtrl->OpeReg.DatGnLevel,
							  TuneLessCtrl->OpeReg.DatJratLevel,
							  Prm->DatLevel );

			/*------------------------------------------------------------------------------------------*/
			/*	�m�b�`�ݒ�Ǎ���																		*/
			/*------------------------------------------------------------------------------------------*/
			ulwork = (ULONG)Prm->tfuncsw;

			/*------------------------------------------------------------------------------------------*/
			/*	�����ݒ�m�b�`��������																	*/
			/*------------------------------------------------------------------------------------------*/
			if( Prm->anotchsw & 0x0F00 )						/* �����ݒ�m�b�`�𒊏o					*/
			{
				ulwork &= 0x0000F0FF;							/* ��2�m�b�`OFF							*/
			}
			RpiResetStatusANotch1( Axis->ANotchSeq );			/* �֘A�X�e�[�^�X���Z�b�g				*/
			RpiResetStatusANotch2( Axis->ANotchSeq );			/* �֘A�X�e�[�^�X���Z�b�g				*/
//			rc = RpiPrmWriteRamForFn( &pndef_tfuncsw, (USHORT)ulwork, pfexe->App.Hmsg );
			TunePrmWriteRam( &AccessPrm, &pndef_tfuncsw, (LONG)ulwork, 2 );

			/*------------------------------------------------------------------------------------------*/
			/*	�g���N�t�B���^�v�Z																		*/
			/*------------------------------------------------------------------------------------------*/
			RpiANotchTrqFilCalc( Axis->ANotchSeq, Axis->ANotch, RegCfg, Prm );

			/*------------------------------------------------------------------------------------------*/
			/*	�U�����x���N���A																		*/
			/*------------------------------------------------------------------------------------------*/
			DetVibResetMaxMinSpeedError( Axis->DetVib );
			break;

		case FCMD_STOP:		/* ��~	�� �m�b�`����OFF	*/
			/*------------------------------------------------------------------------------------------*/
			/*	�m�b�`�ݒ�Ǎ���																		*/
			/*------------------------------------------------------------------------------------------*/
			ulwork = (ULONG)Prm->tfuncsw;

			/*------------------------------------------------------------------------------------------*/
			/*	�����ݒ�m�b�`��������																	*/
			/*------------------------------------------------------------------------------------------*/
			switch( RpiGetANotchPreStat(Axis->ANotchSeq) )
			{ /* �m�b�`�����X�e�[�^�X�擾 */
				case OPTUNE_PSTS_NT1_COMP:							/* �O��1�i�ڃm�b�`ON				*/
					ulwork &= 0x0000FFF0;							/* ��1�m�b�`OFF						*/
					RpiResetStatusANotch1( Axis->ANotchSeq );		/* �֘A�X�e�[�^�X���Z�b�g			*/
					break;

				case OPTUNE_PSTS_NT2_COMP:							/* �O��2�i�ڃm�b�`ON				*/
					ulwork &= 0x0000F0FF;							/* ��2�m�b�`OFF						*/
					RpiResetStatusANotch2( Axis->ANotchSeq );		/* �֘A�X�e�[�^�X���Z�b�g			*/
					break;

				default :
					break;
			}
//			rc = RpiPrmWriteRamForFn( &pndef_tfuncsw, (USHORT)ulwork, pfexe->App.Hmsg );
			TunePrmWriteRam( &AccessPrm, &pndef_tfuncsw, (LONG)ulwork, 2 );

			/*------------------------------------------------------------------------------------------*/
			/*	�g���N�t�B���^�v�Z																		*/
			/*------------------------------------------------------------------------------------------*/
			RpiANotchTrqFilCalc( Axis->ANotchSeq, Axis->ANotch, RegCfg, Prm );

			/*------------------------------------------------------------------------------------------*/
			/*	�U�����x���N���A																		*/
			/*------------------------------------------------------------------------------------------*/
			DetVibResetMaxMinSpeedError( Axis->DetVib );

			rc = PRM_RSLT_CONTINUE;
			break;

		case FCMD_EXEC:		/* ���s�@���@���x���ύX		*/
			/*------------------------------------------------------------------------------------------*/
			/*	�`���[�j���O���x���ݒ�																	*/
			/*------------------------------------------------------------------------------------------*/
			if( PRM_RSLT_SUCCESS != setTLLvlSetLevel(&(AccessPrm),
													 TuneLessCtrl->OpeReg.DatGnLevel,
													 TuneLessCtrl->OpeReg.DatJratLevel,
													 Prm->DatLevel) )
			{
				rc = PRM_RSLT_CALC_ERR;
			}

			/*------------------------------------------------------------------------------------------*/
			/*	�m�b�`�ݒ�Ǎ���																		*/
			/*------------------------------------------------------------------------------------------*/
			ulwork = (ULONG)Prm->tfuncsw;

			/*------------------------------------------------------------------------------------------*/
			/*	�����ݒ�m�b�`��������																	*/
			/*------------------------------------------------------------------------------------------*/
			if( Prm->anotchsw & 0x0F00 )						/* �����ݒ�m�b�`�𒊏o					*/
			{
				ulwork &= 0x0000F0FF;							/* ��2�m�b�`OFF							*/
			}
			RpiResetStatusANotch1( Axis->ANotchSeq );			/* �֘A�X�e�[�^�X���Z�b�g				*/
			RpiResetStatusANotch2( Axis->ANotchSeq );			/* �֘A�X�e�[�^�X���Z�b�g				*/
//			rc = RpiPrmWriteRamForFn( &pndef_tfuncsw, (USHORT)ulwork, pfexe->App.Hmsg );
			rc = TunePrmWriteRam( &AccessPrm, &pndef_tfuncsw, (LONG)ulwork, 2 );
			if( rc != PRM_RSLT_SUCCESS )
			{
				rc = PRM_RSLT_CALC_ERR;
			}

			/*------------------------------------------------------------------------------------------*/
			/*	�g���N�t�B���^�v�Z																		*/
			/*------------------------------------------------------------------------------------------*/
			RpiANotchTrqFilCalc( Axis->ANotchSeq, Axis->ANotch, RegCfg, Prm );

			/*------------------------------------------------------------------------------------------*/
			/*	�U�����x���N���A																		*/
			/*------------------------------------------------------------------------------------------*/
			DetVibResetMaxMinSpeedError( Axis->DetVib );
			break;

		case FCMD_EXEC2 :	/* ���s2				*/
		case FCMD_SV :		/* �T�[�{�I���L�[		*/
			/* �������g����͏����v���Z�b�g */
			RpiANotchFftReqSet( Axis->ANotchSeq, Axis->FftAna );
			rc = PRM_RSLT_CONTINUE;
			break;

		case FCMD_EWRT:		/* EEPROM������			*/
			if( setTLLvlUpdateInEeprom(RegCfg) != PRM_RSLT_SUCCESS )
			{ /* EEPROM�����ݎ��s�� */
				rc = PRM_RSLT_CALC_ERR;
			}
			break;

		case FCMD_NONE :
			break;

		default:
			rc = PRM_RSLT_CMD_ERR;
			break;
		}
		break;

	default:
		break;
	}
	return(rc);
}


/****************************************************************************************************/
/*		�Q�C�����x��(Pn170.2,Pn170.3�̒l)���� 														*/
/****************************************************************************************************/
void	RpiRegSetTuneLessLevel( ULONG Gain, TUNELESS_CTRL *TuneLessCtrl )
{
	LONG	GainLevel;
	LONG	JratLevel;

	/*------------------------------------------------------------------------------------------*/
	/*	���x���ݒ�擾																			*/
	/*------------------------------------------------------------------------------------------*/
	GainLevel = (Gain >> 8) & 0x0F;
	JratLevel = (Gain >> 12) & 0x0F;

	/*------------------------------------------------------------------------------------------*/
	/*	�Q�C�����x���㉺���`�F�b�N																*/
	/*------------------------------------------------------------------------------------------*/
	if( GainLevel > DAT_GAINLEVEL_MAX )
	{
		GainLevel = DAT_GAINLEVEL_MAX;
	}
	else if( GainLevel < DAT_GAINLEVEL_MIN )
	{
		GainLevel = DAT_GAINLEVEL_MIN;
	}

	/*------------------------------------------------------------------------------------------*/
	/*	���׃��x���㉺���`�F�b�N																*/
	/*------------------------------------------------------------------------------------------*/
	if( JratLevel > DAT_JRATLEVEL_MAX )
	{
		JratLevel = DAT_JRATLEVEL_MAX;
	}
	else if( JratLevel < DAT_JRATLEVEL_MIN )
	{
		JratLevel = DAT_JRATLEVEL_MIN;
	}

	/*------------------------------------------------------------------------------------------*/
	/*	���W�X�^�ݒ�																			*/
	/*------------------------------------------------------------------------------------------*/
	TuneLessCtrl->OpeReg.DatGnLevel = (USHORT)GainLevel;
	TuneLessCtrl->OpeReg.DatJratLevel = (USHORT)JratLevel;

	return;
}


/* -------- Fn200 Check the Condition for Setting Tuning Less Level	-------- */
static PRM_RSLT	setTLLvlChkCondition( TUNELESS_CTRL *TuneLessCtrl )
{
	PRM_RSLT	rc;
	rc = PRM_RSLT_SUCCESS;

	/* �������X�@�\�������́A�f�[�^�ُ�Ƃ��� */
	if( TuneLessCtrl->conf.TuningLessUse != TRUE )
	{
		rc = PRM_RSLT_CONDITION_ERR;
	}
	return rc;
}



/* -------- Fn200 Update Parmeter In EEPROM -------- */
static PRM_RSLT	setTLLvlUpdateInEeprom( REGIF_CONFIG_T *RegCfg )
{
	PRM_RSLT	rc;
	rc = PRM_RSLT_SUCCESS;

	/* Pn170 �_�C�i�~�b�N�I�[�g�`���[�j���O�֘A�X�C�b�` */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_DatLevel, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return(rc);
	}
	/* Pn401 �P�i�ڑ�P�g���N�w�߃t�B���^���萔 */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_trqfil11, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return(rc);
	}
	/* Pn408 �g���N�֌W�@�\�X�C�b�` */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_tfuncsw, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return(rc);
	}
	/* Pn409 �P�i�ڃm�b�`�t�B���^���g�� */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_ntchfil1, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return(rc);
	}
	/* Pn40A �P�i�ڃm�b�`�t�B���^�p�l */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_ntchq1, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return(rc);
	}
	/* Pn40C �Q�i�ڃm�b�`�t�B���^���g�� */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_ntchfil2, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return(rc);
	}
	/* Pn40D �Q�i�ڃm�b�`�t�B���^�p�l */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_ntchq2, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return(rc);
	}

	return(rc);
}





/* -------- Fn200 Write Prm To Ram With EEPROM Data	-------- */
static void	setTLLvlUndoRamGain( REGIF_CONFIG_T *RegCfg )
{
	/* RAM�̒l��EEPROM�̒l�Ɠ����ɂ��� */
	/* Pn170 �_�C�i�~�b�N�I�[�g�`���[�j���O�֘A�X�C�b�` */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_DatLevel, 0 );
	/* Pn401 �P�i�ڑ�P�g���N�w�߃t�B���^���萔 */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_trqfil11, 0 );
	/* Pn408 �g���N�֌W�@�\�X�C�b�` */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_tfuncsw, 0 );
	/* Pn409 �P�i�ڃm�b�`�t�B���^���g�� */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_ntchfil1, 0 );
	/* Pn40A �P�i�ڃm�b�`�t�B���^�p�l */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_ntchq1, 0 );
	/* Pn40C �Q�i�ڃm�b�`�t�B���^���g�� */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_ntchfil2, 0 );
	/* Pn40D �Q�i�ڃm�b�`�t�B���^�p�l */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_ntchq2, 0 );

	return;
}


/****************************************************************************************************/
/*		�������x���̃p�����[�^�ݒ菈�� 																*/
/****************************************************************************************************/
PRM_RSLT	setTLLvlSetLevel( REG_ACC_T *AccessPrm, USHORT GainLevel, USHORT JratLevel, USHORT DetLevel )
{
	PRM_RSLT	rc;
	ULONG		uswk, uswk2;
	ULONG		ulwk;

	rc = PRM_RSLT_SUCCESS;

	uswk = (GainLevel << 8);
	uswk2 = (JratLevel << 12);
	ulwk = (ULONG)DetLevel & 0xFFFF00FF;
	ulwk |= (ULONG)uswk + (ULONG)uswk2;
//	rc = RpiPrmWriteRamForFn( &pndef_DatLevel, ulwk, Hmsg );
	rc = TunePrmWriteRam( AccessPrm, &pndef_DatLevel, ulwk, 2 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return(rc);
	}

	return(rc);
}



/****************************************************************************************************/
/*		�Q�C�����x���ݒ菈�� (���W�X�^IF)															*/
/****************************************************************************************************/
PRM_RSLT	RpiFunSetDATGainLevel( TUNELESS_CTRL *TuneLessCtrl, USHORT Level )
{
	PRM_RSLT	rc;
	rc = PRM_RSLT_SUCCESS;

	/*------------------------------------------------------------------------------------------*/
	/*	�Q�C�����x���㉺���`�F�b�N																*/
	/*------------------------------------------------------------------------------------------*/
	if( ((LONG)Level >= DAT_GAINLEVEL_MIN) && ((LONG)Level <= DAT_GAINLEVEL_MAX) )
	{
		TuneLessCtrl->OpeReg.DatGnLevel = Level;
	}
	else
	{
		TuneLessCtrl->OpeReg.DatGnLevel = DAT_GAINLEVEL_MIN;
		rc = PRM_RSLT_REGNO_ERR;
	}
	return( rc );
}


/****************************************************************************************************/
/*		���׃��x���ݒ菈�� (���W�X�^IF)																*/
/****************************************************************************************************/
PRM_RSLT	RpiFunSetDATJratLevel( TUNELESS_CTRL *TuneLessCtrl, USHORT Level )
{
	PRM_RSLT	rc;
	rc = PRM_RSLT_SUCCESS;

	/*------------------------------------------------------------------------------------------*/
	/*	���׃��x���㉺���`�F�b�N																*/
	/*------------------------------------------------------------------------------------------*/
	if( ((LONG)Level >= DAT_JRATLEVEL_MIN) && ((LONG)Level <= DAT_JRATLEVEL_MAX) )
	{
		TuneLessCtrl->OpeReg.DatJratLevel = Level;
	}
	else
	{
		TuneLessCtrl->OpeReg.DatJratLevel = DAT_JRATLEVEL_MIN;
		rc = PRM_RSLT_REGNO_ERR;
	}
	return( rc );
}



/****************************************************************************************************/
