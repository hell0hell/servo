/****************************************************************************
  Description   : Callback functions for Operation Register
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corp., Copyright (c) 2010
                  All Rights Reserved

  Project       : INGRAM

  Create Date   : 2010-10-20

****************************************************************************/
#include "Basedef.h"
#include "MercuryGlobals.h"
#include "PrmAccessIf.h"
#include "FunManager.h"
#include "BaseControlPrmCalc.h"
#include "FnMfcTuning.h"
#include "FnSetTuneLessLvl.h"
#include "FnOnePrmTuning.h"
#include "FnAtypeVibTune.h"
#include "DataTraceManager.h"
#include "PdetABSScale.h"
#include "TableDrive.h"
#include "PosManager.h"
#include "PcmdFilter.h" 		/* <S065> */
#include "PnPrmListTbl.h"
#include "PrmAccessIf.h"		/* <S0C7> */
#include "LcdOp.h"				/* <S0C9> */
#include "KnlApi.h"			/* <S0C9> */
#include "Adc.h"				/* <S0CB> */
#include "AnalogMonitorCalc.h"	/* <S0E7> */
#include "RippleComp.h"		/* <S1FD> */
/* Extern Serial Message I/F for SigmaMsg/LcdOp/Memobus */	/* <S0C9> */
extern MSGIF	SrlMsg[SIF_N_CH];							/* <S0C9> */

/****************************************************************************
 2000H:���샂�[�h
****************************************************************************/
PRM_RSLT fnCalcOprationMode1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRM_RSLT ErrRes;
	if(Cmd == PRM_ACCCMD_READ)
	{
		*pValue = FunGetOpeModeRegister(((AXIS_HANDLE*)Axis)->FnManager,FUN_CH1);
		ErrRes = PRM_RSLT_SUCCESS;
	}
	else
	{
		ErrRes = FunSetOpeModeRegister(((AXIS_HANDLE*)Axis)->FnManager,
									FUN_CH1, (ULONG)*pValue, Axis);
	}

	return ErrRes;
}

/****************************************************************************
 2001H:����ݒ�
****************************************************************************/
PRM_RSLT fnCalcOprationCmd1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRM_RSLT ErrRes;
	ErrRes = PRM_RSLT_SUCCESS;

	if(Cmd == PRM_ACCCMD_READ)
	{
		*pValue = (USHORT)(((AXIS_HANDLE*)Axis)->FnManager)->FunExeState[0].ErrorResult;
	}
	else
	{
		ErrRes = FunSetOpeCommandRegister(((AXIS_HANDLE*)Axis)->FnManager,
										FUN_CH1, (ULONG)*pValue);
	}

	return ErrRes;
}

/****************************************************************************
 2002H:����I��
****************************************************************************/
PRM_RSLT fnCalcOperationFinish(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRM_RSLT ErrRes;
	if(Cmd == PRM_ACCCMD_READ)
	{
		ErrRes = PRM_RSLT_REGNO_ERR;
	}
	else
	{
		FunSetOpeModeRegister(((AXIS_HANDLE*)Axis)->FnManager, FUN_CH1, 0, Axis);
		FunSetOpeModeRegister(((AXIS_HANDLE*)Axis)->FnManager, FUN_CH2, 0, Axis);
		DtrcWriteOpeModeTrcRegister( ((AXIS_HANDLE*)Axis)->TrcHndl, 0 );
		ErrRes = PRM_RSLT_SUCCESS;
	}
	return ErrRes;
}

/****************************************************************************
 0x2020	���[�U�萔�����݋֎~
****************************************************************************/
PRM_RSLT fnCalcPrmAccessLock(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRM_RSLT 	ErrRes;
	REG_ACC_T	AccessPrm;
	LONG		syssw3;

	ErrRes = PRM_RSLT_SUCCESS;

	AccessPrm.hRegCfg = Axis->RegManager->hPnReg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.IfHandle = NULL;
	AccessPrm.EepWrite = TRUE;
	AccessPrm.cPrmDef = &pndef_syssw3;

	switch(Cmd)
	{
	case PRM_ACCCMD_WRITE:
	case PRM_ACCCMD_EEPWRITE:
		if(*pValue == 0)
		{
			/* Write the complete flag to PnE0D.0=0 */
			PrmReadValue(&AccessPrm, &syssw3, 2);
			syssw3 = syssw3 & 0xFFFE;
			PrmWriteValue(&AccessPrm, &syssw3, 2);
			RegMngrFreeWriteParameters(Axis->RegManager);
		}
		else if(*pValue == 1)
		{
			/* Write the complete flag to PnE0D.0=1 */
			PrmReadValue(&AccessPrm, &syssw3, 2);
			syssw3 = syssw3 | 0x0001;
			PrmWriteValue(&AccessPrm, &syssw3, 2);
			RegMngrLockWriteParameters(Axis->RegManager);
		}
		else
		{
			ErrRes = PRM_RSLT_LIMIT_ERR;
		}
		break;

	case PRM_ACCCMD_RECALC:
		PrmReadValue(&AccessPrm, &syssw3, 2);
		if(0 != (syssw3 & 1))
		{/* Lock to write parameter access */
			RegMngrLockWriteParameters(Axis->RegManager);
		}
		else
		{/* Unlock to write parameter access */
			RegMngrFreeWriteParameters(Axis->RegManager);
		}
		break;

	case PRM_ACCCMD_READ:
		PrmReadValue(&AccessPrm, &syssw3, 2);
		*pValue = syssw3 & 1;
		break;

	default:
		break;
	}

	return ErrRes;
}

/****************************************************************************
 2003H:�A�N�Z�X���x��
****************************************************************************/
PRM_RSLT fnCalcAccessLevel(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG			level; /* <S21F> */
	USHORT			passwd;

/* <S056>	Start */
	FUN_AXCOMMON	*AxCommon;
	PRMDATA			*Prm;
	REGIF_CONFIG_T	*RegCfg;

	AxCommon = ((AXIS_HANDLE*)Axis)->FnManager->AxCommon;
	Prm = Axis->UniPrms->Prm;
	RegCfg = Axis->RegManager->hPnReg;


//	if(Cmd == PRM_ACCCMD_READ)
//	{
//		*pValue = (LONG)(AxCommon->AccessLevel - 1);
//	}
//	else
//	{
//		passwd = (USHORT)*pValue;
//		if(passwd == 0x1002)
//		{	AxCommon->AccessLevel = 1/*ACCLVL_USER1*/;}
//		else if(passwd == 0x2001)
//		{	AxCommon->AccessLevel = 2/*ACCLVL_USER2*/;}
//		else if(passwd == 0x2236)
//		{	AxCommon->AccessLevel = 3/*ACCLVL_SYSTEM*/;}
//		else
//		{	;}


	if(Cmd == PRM_ACCCMD_READ)
	{
//		*pValue = (LONG)(AxCommon->AccessLevel - 1);	/* <S21F> */
		level = (LONG)(AxCommon->AccessLevel - 2);		/* <S21F> */
		*pValue = (level >= 0) ? level : 0;				/* <S21F> */
	}
	else
	{
		passwd = (USHORT)*pValue;

		switch(passwd)
		{
		case WRITE_OK_SET_VAL:	/* ��������(�A�N�Z�X���x���ω��Ȃ�) */
				fnCalcPrmAccessLock(Cmd, ArrayIdx, Axis, pValue);
			break;

		case WRITE_NG_SET_VAL:	/* �����֎~(�A�N�Z�X���x���ω��Ȃ�) */
				fnCalcPrmAccessLock(Cmd, ArrayIdx, Axis, pValue);
			break;

		case ACCLVL_USER1_SET_VAL:	/* ���[�U���x��1�ݒ� */
				AxCommon->AccessLevel = ACCLVL_USER1;

				/* EEPROM�ɃA�N�Z�����x����ݒ�(  PnE0D.1 = 0 : ���[�U���x��1 ) */
				Prm->syssw3 = Prm->syssw3 & (~0x0002);
				PrmStoreValueToEeprom( RegCfg, &pndef_syssw3, 0 );
			break;

		case ACCLVL_USER2_SET_VAL:	/* ���[�U���x��2�ݒ� */
				AxCommon->AccessLevel = ACCLVL_USER2;

				//Prm->syssw3 = Prm->syssw3 | 0x0002;					/* <S1B0> */
				//PrmStoreValueToEeprom( RegCfg, &pndef_syssw3, 0 );	/* <S1B0> */
			break;

		case ACCLVL_USER3_SET_VAL:	/* ���[�U���x��3�ݒ� */
				AxCommon->AccessLevel = ACCLVL_USER3;

				/* EEPROM�ɃA�N�Z�����x����ݒ�(  PnE0D.1 = 1 : ���[�U���x��3 ) */	/* <S1B0> */
				Prm->syssw3 = Prm->syssw3 | 0x0002;									/* <S1B0> */
				PrmStoreValueToEeprom( RegCfg, &pndef_syssw3, 0 );					/* <S1B0> */
			break;

		case ACCLVL_SYSTEM_SET_VAL:	/* �V�X�e�����x���ݒ� */
				AxCommon->AccessLevel = ACCLVL_SYSTEM;
			break;

		default:
			/* Do Nothing */
			break;
		}
		
	}
/* <S056>	End */

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
 2004H:��2���샂�[�h
****************************************************************************/
PRM_RSLT fnCalcOprationMode2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRM_RSLT ErrRes;
	if(Cmd == PRM_ACCCMD_READ)
	{
		*pValue = (LONG)FunGetOpeModeRegister(((AXIS_HANDLE*)Axis)->FnManager,FUN_CH2);
		ErrRes = PRM_RSLT_SUCCESS;
	}
	else
	{
		ErrRes = FunSetOpeModeRegister(((AXIS_HANDLE*)Axis)->FnManager,
									FUN_CH2, (ULONG)*pValue, Axis);
	}

	return ErrRes;
}

/****************************************************************************
 2005H:��2����ݒ�
****************************************************************************/
PRM_RSLT fnCalcOprationCmd2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRM_RSLT ErrRes;
	ErrRes = PRM_RSLT_SUCCESS;

	if(Cmd == PRM_ACCCMD_READ)
	{
		*pValue = (USHORT)(((AXIS_HANDLE*)Axis)->FnManager)->FunExeState[1].ErrorResult;
	}
	else
	{
		ErrRes = FunSetOpeCommandRegister(((AXIS_HANDLE*)Axis)->FnManager,
										FUN_CH2, (ULONG)*pValue);
	}
	return ErrRes;
}


/****************************************************************************
 2032H:���Ɍ��o����
****************************************************************************/
/*	0x00:MPDET_NORMAL		�ʏ�										  */
/*	0x01:MPDET_EXE			���s��										  */
/*	0x02:MPDET_COMP			����										  */
/*	0x03:MPDET_ERROR		�ُ�I��									  */
/**************************************************************************/
PRM_RSLT fnGetPdetStatus(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	switch(((AXIS_HANDLE*)Axis)->PdetAbsSeq->SeqStep)
	{
	case PDETSEQ_IDOL:
	case PDETSEQ_PHASE_WR_READY:
		if(Axis->MencV->PhaseReady == FALSE)
		{
			*pValue = Axis->FnCmnCtrl->PdetReq | Axis->BeSequence->BeReqSeqOut;
		}
		else
		{/* already completed. */
			*pValue = 2;
		}
		break;
	case PDETSEQ_FINISHED:
		*pValue = 2;
		break;
	case PDETSEQ_ERROR:
		*pValue = 3;
		break;
	default:
		*pValue = 1;
		break;
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
 2033H:���_�ʒu�ݒ芮��
****************************************************************************/
PRM_RSLT fnGetScaleZsetStatus(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA		*Prm;

	Prm = ((AXIS_HANDLE*)Axis)->UniPrms->Prm;
	*pValue = (0 == (Prm->syssw3 & 0x0010)) ? 0 : 1;

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
 2050H:1�p���`���[�j���O���[�h
****************************************************************************/
PRM_RSLT	FnOnePrmTuneSpatMode( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue )
{
	PRM_RSLT		rc;
	ONEPRMTUNE		*OnePrmTune;
	BASE_CTRL_HNDL	*pBaseCtrl;
	PRMDATA			*Prm;

	OnePrmTune = ((AXIS_HANDLE*)Axis)->OnePrmTune;
	pBaseCtrl = ((AXIS_HANDLE*)Axis)->BaseControls;
	Prm = (((AXIS_HANDLE*)Axis)->UniPrms)->Prm;

	rc = RpiFunSetOnePrmTuningMode( OnePrmTune, (USHORT)*pValue );
	if( (rc != PRM_RSLT_SUCCESS) && (rc != PRM_RSLT_CONTINUE) )
	{
		return( rc );
	}
	/* �`���[�j���O���x�������ݒ� */
	OnePrmReSetTuningLevel( OnePrmTune,
							Prm->loophz,
							Prm->posgn,
							pBaseCtrl->CtrlCmdPrm.IPSpdControl );
	return( PRM_RSLT_SUCCESS );
}

/****************************************************************************
 2051H:1�p���`���[�j���O���x��
****************************************************************************/
PRM_RSLT	FnOnePrmTuneSpatLevel( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue )
{
	PRM_RSLT	rc;
	ONEPRMTUNE	*OnePrmTune;
	OnePrmTune = ((AXIS_HANDLE*)Axis)->OnePrmTune;

	rc = RpiFunSetOnePrmTuningLevel( OnePrmTune, (USHORT)*pValue );
	if( (rc != PRM_RSLT_SUCCESS) && (rc != PRM_RSLT_CONTINUE) )
	{
		return( rc );
	}
	return( PRM_RSLT_SUCCESS );
}

/****************************************************************************
 2052H:1�p���`���[�j���OFF���x��
****************************************************************************/
PRM_RSLT	FnOnePrmTuneSpatFFLevel( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue )
{
	PRM_RSLT	rc;
	ONEPRMTUNE	*OnePrmTune;
	OnePrmTune = ((AXIS_HANDLE*)Axis)->OnePrmTune;

	/* <S0DA> */
	switch(Cmd)
	{
	case PRM_ACCCMD_WRITE:
	case PRM_ACCCMD_EEPWRITE:
		rc = RpiFunSetOnePrmTuningFfLevel( OnePrmTune, (USHORT)*pValue );
		break;
	case PRM_ACCCMD_READ:
		*pValue = (LONG)OnePrmTune->OpeReg.SpatFFLevel;
		rc = PRM_RSLT_SUCCESS;
		break;
	default:
		rc = PRM_RSLT_SUCCESS;
		break;
	}

	return rc;

#if 0	/* <S0DA> */
	rc = RpiFunSetOnePrmTuningFfLevel( OnePrmTune, (USHORT)*pValue );
	if( (rc != PRM_RSLT_SUCCESS) && (rc != PRM_RSLT_CONTINUE) )
	{
		return( rc );
	}
	return( PRM_RSLT_SUCCESS );
#endif

}

/****************************************************************************
 2062H:EasyFFT�g���N�U��
****************************************************************************/
PRM_RSLT	FnEasyFftTrqAmplitude( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue )
{
	PRM_RSLT	rc;
	EASYFFT		*EasyFFT;
	EasyFFT = ((AXIS_HANDLE*)Axis)->EasyFFT;

	rc = RpiFunSetEasyFftTrqLevel( EasyFFT, (USHORT)*pValue );
	if( (rc != PRM_RSLT_SUCCESS) && (rc != PRM_RSLT_CONTINUE) )
	{ /* �f�[�^�ُ펞 */
		return( rc );
	}
	return( PRM_RSLT_SUCCESS );
}


/****************************************************************************
 0x2080	���[�^�萔�ǂݏ����擪�A�h���X
****************************************************************************/
PRM_RSLT fnCalcMotorParameterAddress(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	MENCV *MencV;

	MencV = ((AXIS_HANDLE*)Axis)->MencV;
	switch(Cmd)
	{
	case PRM_ACCCMD_WRITE:
	case PRM_ACCCMD_EEPWRITE:
	case PRM_ACCCMD_RECALC:
		MencV->MotPrmAddress = (USHORT)*pValue;
		break;
	case PRM_ACCCMD_READ:
	default:
		*pValue = (LONG)MencV->MotPrmAddress;
		break;
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
 0x2081	���[�^�萔�ǂݏ����T�C�Y
****************************************************************************/
PRM_RSLT fnCalcMotorParameterSize(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	MENCV *MencV;

	MencV = ((AXIS_HANDLE*)Axis)->MencV;
	switch(Cmd)
	{
	case PRM_ACCCMD_WRITE:
	case PRM_ACCCMD_EEPWRITE:
	case PRM_ACCCMD_RECALC:
		MencV->MotPrmSize = (USHORT)*pValue;
		break;
	case PRM_ACCCMD_READ:
	default:
		*pValue = (LONG)MencV->MotPrmSize;
		break;
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
 0x2082	���[�^�萔�ǂݏ�������
****************************************************************************/
PRM_RSLT fnGetMotorDownloadState(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	USHORT	mode;

	mode = FunGetOpeModeRegister(((AXIS_HANDLE*)Axis)->FnManager,FUN_CH1);
	if((mode == 0x200B) || (mode == 0x200C))
	{
		*pValue = (LONG)(((AXIS_HANDLE*)Axis)->FnManager)->FunExeState[0].State;
	}
	else
	{
		*pValue = 0;
	}

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
 0x208C Parameter Download/Upload Status	<S00C>
****************************************************************************/
PRM_RSLT fnGetPrmDownloadState(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	USHORT	mode;

	mode = FunGetOpeModeRegister(((AXIS_HANDLE*)Axis)->FnManager,FUN_CH1);
	if((mode == 0x200F) || (mode == 0x2010))
	{
		*pValue = (LONG)(((AXIS_HANDLE*)Axis)->FnManager)->FunExeState[0].State;
	}
	else
	{
		*pValue = 0;
	}

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
 2095H:�A�h�o���X�g�`�s�t�B���^�^�C�v
****************************************************************************/
PRM_RSLT	FnAdatFilType( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue )
{
	ONEPRMTUNE	*OnePrmTune;
	OnePrmTune = ((AXIS_HANDLE*)Axis)->OnePrmTune;

	if( (*pValue >= 1) && (*pValue <= 3) )
	{
		OnePrmTune->OpeReg.OnePrmFilType = (USHORT)*pValue;
	}
	else
	{
		return( PRM_RSLT_LIMIT_ERR );
	}
	return( PRM_RSLT_SUCCESS );
}

/****************************************************************************
 20A0H:A�^���U�@�\�������[�h�ݒ�
****************************************************************************/
PRM_RSLT	FnAvibTuneAvibMode( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue )
{
	PRM_RSLT	rc;
	AVIBTUNE	*AvibTune;
	AvibTune = ((AXIS_HANDLE*)Axis)->AvibTune;

	rc = RpiFunSetAtypeVibTuningMode( AvibTune, (USHORT)*pValue );
	if( (rc != PRM_RSLT_SUCCESS) && (rc != PRM_RSLT_CONTINUE) )
	{
		return( rc );
	}
	return( PRM_RSLT_SUCCESS );

}

/****************************************************************************
 20A2H:A�^���U�@�\�������g��
****************************************************************************/
PRM_RSLT	FnAvibTuneAvibFreq( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue )
{
	PRM_RSLT	rc;
	AVIBTUNE	*AvibTune;
	AvibTune = ((AXIS_HANDLE*)Axis)->AvibTune;

	rc = RpiFunSetAtypeVibTuningFrequency( AvibTune, (USHORT)*pValue );
	if( (rc != PRM_RSLT_SUCCESS) && (rc != PRM_RSLT_CONTINUE) )
	{
		return( rc );
	}
	return( PRM_RSLT_SUCCESS );

}

/****************************************************************************
 20A3H:A�^���U�@�\�����Q�C��
****************************************************************************/
PRM_RSLT	FnAvibTuneAvibGain( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue )
{
	PRM_RSLT	rc;
	AVIBTUNE	*AvibTune;
	AvibTune = ((AXIS_HANDLE*)Axis)->AvibTune;

	rc = RpiFunSetAtypeVibTuningGain( AvibTune, (USHORT)*pValue );
	if( (rc != PRM_RSLT_SUCCESS) && (rc != PRM_RSLT_CONTINUE) )
	{
		return( rc );
	}
	return( PRM_RSLT_SUCCESS );

}

/****************************************************************************
 20B0H:�U���}���t��MFC�������g��
****************************************************************************/
PRM_RSLT FnMfcTuneMdlVibFreq( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue )
{
	PRM_RSLT ErrRes;

	ErrRes = RpiMfcTuningSetFreq( (((AXIS_HANDLE*)Axis)->MfcTune), (USHORT)*pValue );
	return( ErrRes );
}

/****************************************************************************
 20B2H : �U���}���@�\�`���[�j���O���g��2
****************************************************************************/
PRM_RSLT FnMfcTuneMdlVibFreq2( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue )
{
	PRM_RSLT	ErrRes;
	MFCTUNE		*MfcTune;
	VIBSUPFIL	*VibSupFil;
	PRMDATA		*Prm;
	BPRMDAT		*Bprm;

	MfcTune = ((AXIS_HANDLE*)Axis)->MfcTune;
	VibSupFil = &(((AXIS_HANDLE*)Axis)->BaseControls)->VibSupFil;
	Prm = (((AXIS_HANDLE*)Axis)->UniPrms)->Prm;
	Bprm = (((AXIS_HANDLE*)Axis)->UniPrms)->Bprm;

	ErrRes = RpiMfcTuningSetFreq2( MfcTune, (USHORT)*pValue );
	if( ErrRes == PRM_RSLT_SUCCESS )
	{
		Prm->ff_frq = MfcTune->OpeReg.MdlVibFreq2;
//		PosMngCalculatePrmVibSupFil( VibSupFil, /* <S065> */
		PcmdFilCalculatePrmVibSupFil( VibSupFil,
									  Prm->ff_frq,
									  Prm->ff_fil,
									  Bprm->SvCycleNs );
	}
	return( ErrRes );
}

/****************************************************************************
 20B3H:�U���}���@�\�`���[�j���O���g��2�␳
****************************************************************************/
PRM_RSLT FnMfcTuneMdlVibFil2( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue )
{
	PRM_RSLT	ErrRes;
	MFCTUNE		*MfcTune;
	VIBSUPFIL	*VibSupFil;
	PRMDATA		*Prm;
	BPRMDAT		*Bprm;

	MfcTune = ((AXIS_HANDLE*)Axis)->MfcTune;
	VibSupFil = &(((AXIS_HANDLE*)Axis)->BaseControls)->VibSupFil;
	Prm = (((AXIS_HANDLE*)Axis)->UniPrms)->Prm;
	Bprm = (((AXIS_HANDLE*)Axis)->UniPrms)->Bprm;

	ErrRes = RpiMfcTuningSetFreq2Comp( MfcTune, (USHORT)*pValue );
	if( ErrRes == PRM_RSLT_SUCCESS )
	{
		Prm->ff_fil = MfcTune->OpeReg.MdlVibFil2;
//		PosMngCalculatePrmVibSupFil( VibSupFil, /* <S065> */
		PcmdFilCalculatePrmVibSupFil( VibSupFil,
									  Prm->ff_frq,
									  Prm->ff_fil,
									  Bprm->SvCycleNs );
	}
	return( ErrRes );
}

/* <S14B> Start */
/****************************************************************************
 20B5H:���x���b�v���⏞��񏑍��݊m�F
****************************************************************************/
PRM_RSLT fnCalcRippleCmpInfo(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	RPLCMP				*RippleCmp;
	RippleCmp = &(Axis->BaseLoops->RippleCmp);

	*pValue = (LONG)(RippleCmp->RippleCmpState);

	return (PRM_RSLT_SUCCESS);
}
/****************************************************************************
 20B6H:���x���b�v���⏞�Q�C��
****************************************************************************/
PRM_RSLT fnCalcRippleCompGain(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
//	PRM_RSLT	ErrRes;
	RPLCMP		*RippleCmp;

	RippleCmp = &(Axis->BaseLoops->RippleCmp);


	if(Cmd == PRM_ACCCMD_READ)
	{
		*pValue = (LONG)(RippleCmp->RplCmpPrm.RplCompGainEle);
	}
	else
	{
		RippleCmp->RplCmpPrm.RplCompGainEle = (USHORT)*pValue;

		/* ���b�v���␳�p�����[�^�v�Z */
//		FnRegCalVelRippleCompE( Axis );
		FnRegCalVelRippleCompE( Axis->UniPrms->Bprm, &Axis->BaseLoops->RippleCmp );/* <S1FD> */
	}

//	return ErrRes;
	return (PRM_RSLT_SUCCESS);		/* ���������ϐ��C��<S191> */
}

/****************************************************************************
 20B7H:���x���b�v���⏞��1��������
****************************************************************************/
PRM_RSLT fnCalcRippleCompCycle1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
//	PRM_RSLT	ErrRes;
	RPLCMP		*RippleCmp;

	RippleCmp = &(Axis->BaseLoops->RippleCmp);

	if(Cmd == PRM_ACCCMD_READ)
	{
		*pValue = (LONG)(RippleCmp->RplCmpPrm.RplCompCycleEle[0]);
	}
	else
	{
		RippleCmp->RplCmpPrm.RplCompCycleEle[0] = (USHORT)*pValue;

		/* ���b�v���␳�p�����[�^�v�Z */
//		FnRegCalVelRippleCompE( Axis );
		FnRegCalVelRippleCompE( Axis->UniPrms->Bprm, &Axis->BaseLoops->RippleCmp );/* <S1FD> */
	}

//	return ErrRes;
	return (PRM_RSLT_SUCCESS);
}

/****************************************************************************
 20B8H:���x���b�v���⏞��1�����U���l
****************************************************************************/
PRM_RSLT fnCalcRippleCompAmplitude1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
//	PRM_RSLT	ErrRes;
	RPLCMP		*RippleCmp;

	RippleCmp = &(Axis->BaseLoops->RippleCmp);

	if(Cmd == PRM_ACCCMD_READ)
	{
		*pValue = (LONG)(RippleCmp->RplCmpPrm.RplCompAmplitudeEle[0]);
	}
	else
	{
		RippleCmp->RplCmpPrm.RplCompAmplitudeEle[0] = (SHORT)*pValue;

		/* ���b�v���␳�p�����[�^�v�Z */
//		FnRegCalVelRippleCompE( Axis );
		FnRegCalVelRippleCompE( Axis->UniPrms->Bprm, &Axis->BaseLoops->RippleCmp );/* <S1FD> */
	}

//	return ErrRes;
	return (PRM_RSLT_SUCCESS);		/* ���������ϐ��C��<S191> */
}

/****************************************************************************
 20B9H:���x���b�v���⏞��1�����ʑ�
****************************************************************************/
PRM_RSLT fnCalcRippleCompPhase1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
//	PRM_RSLT	ErrRes;
	RPLCMP		*RippleCmp;

	RippleCmp = &(Axis->BaseLoops->RippleCmp);

	if(Cmd == PRM_ACCCMD_READ)
	{
		*pValue = (LONG)(RippleCmp->RplCmpPrm.RplCompPhaseEle[0]);
	}
	else
	{
		RippleCmp->RplCmpPrm.RplCompPhaseEle[0] = (USHORT)*pValue;

		/* ���b�v���␳�p�����[�^�v�Z */
//		FnRegCalVelRippleCompE( Axis );
		FnRegCalVelRippleCompE( Axis->UniPrms->Bprm, &Axis->BaseLoops->RippleCmp );/* <S1FD> */
	}

//	return ErrRes;
	return (PRM_RSLT_SUCCESS);		/* ���������ϐ��C��<S191> */
}

/****************************************************************************
 20BAH:���x���b�v���⏞��2��������
****************************************************************************/
PRM_RSLT fnCalcRippleCompCycle2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
//	PRM_RSLT	ErrRes;
	RPLCMP		*RippleCmp;

	RippleCmp = &(Axis->BaseLoops->RippleCmp);

	if(Cmd == PRM_ACCCMD_READ)
	{
		*pValue = (LONG)(RippleCmp->RplCmpPrm.RplCompCycleEle[1]);
	}
	else
	{
		RippleCmp->RplCmpPrm.RplCompCycleEle[1] = (USHORT)*pValue;

		/* ���b�v���␳�p�����[�^�v�Z */
//		FnRegCalVelRippleCompE( Axis );
		FnRegCalVelRippleCompE( Axis->UniPrms->Bprm, &Axis->BaseLoops->RippleCmp );/* <S1FD> */
	}

//	return ErrRes;
	return (PRM_RSLT_SUCCESS);		/* ���������ϐ��C��<S191> */
}

/****************************************************************************
 20BBH:���x���b�v���⏞��2�����U���l
****************************************************************************/
PRM_RSLT fnCalcRippleCompAmplitude2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
//	PRM_RSLT	ErrRes;
	RPLCMP		*RippleCmp;

	RippleCmp = &(Axis->BaseLoops->RippleCmp);

	if(Cmd == PRM_ACCCMD_READ)
	{
		*pValue = (LONG)(RippleCmp->RplCmpPrm.RplCompAmplitudeEle[1]);
	}
	else
	{
		RippleCmp->RplCmpPrm.RplCompAmplitudeEle[1] = (SHORT)*pValue;

		/* ���b�v���␳�p�����[�^�v�Z */
//		FnRegCalVelRippleCompE( Axis );
		FnRegCalVelRippleCompE( Axis->UniPrms->Bprm, &Axis->BaseLoops->RippleCmp );/* <S1FD> */
	}

//	return ErrRes;
	return (PRM_RSLT_SUCCESS);		/* ���������ϐ��C��<S191> */
}

/****************************************************************************
 20BCH:���x���b�v���⏞��2�����ʑ�
****************************************************************************/
PRM_RSLT fnCalcRippleCompPhase2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
//	PRM_RSLT	ErrRes;
	RPLCMP		*RippleCmp;

	RippleCmp = &(Axis->BaseLoops->RippleCmp);

	if(Cmd == PRM_ACCCMD_READ)
	{
		*pValue = (LONG)(RippleCmp->RplCmpPrm.RplCompPhaseEle[1]);
	}
	else
	{
		RippleCmp->RplCmpPrm.RplCompPhaseEle[1] = (USHORT)*pValue;

		/* ���b�v���␳�p�����[�^�v�Z */
//		FnRegCalVelRippleCompE( Axis );
		FnRegCalVelRippleCompE( Axis->UniPrms->Bprm, &Axis->BaseLoops->RippleCmp );/* <S1FD> */
	}

//	return ErrRes;
	return (PRM_RSLT_SUCCESS);		/* ���������ϐ��C��<S191> */
}

/****************************************************************************
 20BDH:���x���b�v���⏞��3��������
****************************************************************************/
PRM_RSLT fnCalcRippleCompCycle3(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
//	PRM_RSLT	ErrRes;
	RPLCMP		*RippleCmp;

	RippleCmp = &(Axis->BaseLoops->RippleCmp);

	if(Cmd == PRM_ACCCMD_READ)
	{
		*pValue = (LONG)(RippleCmp->RplCmpPrm.RplCompCycleEle[2]);
	}
	else
	{
		RippleCmp->RplCmpPrm.RplCompCycleEle[2] = (USHORT)*pValue;

		/* ���b�v���␳�p�����[�^�v�Z */
//		FnRegCalVelRippleCompE( Axis );
		FnRegCalVelRippleCompE( Axis->UniPrms->Bprm, &Axis->BaseLoops->RippleCmp );/* <S1FD> */
	}

//	return ErrRes;
	return (PRM_RSLT_SUCCESS);		/* ���������ϐ��C��<S191> */
}

/****************************************************************************
 20BEH:���x���b�v���⏞��3�����U���l
****************************************************************************/
PRM_RSLT fnCalcRippleCompAmplitude3(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
//	PRM_RSLT	ErrRes;
	RPLCMP		*RippleCmp;

	RippleCmp = &(Axis->BaseLoops->RippleCmp);

	if(Cmd == PRM_ACCCMD_READ)
	{
		*pValue = (LONG)(RippleCmp->RplCmpPrm.RplCompAmplitudeEle[2]);
	}
	else
	{
		RippleCmp->RplCmpPrm.RplCompAmplitudeEle[2] = (SHORT)*pValue;

		/* ���b�v���␳�p�����[�^�v�Z */
//		FnRegCalVelRippleCompE( Axis );
		FnRegCalVelRippleCompE( Axis->UniPrms->Bprm, &Axis->BaseLoops->RippleCmp );/* <S1FD> */
	}

//	return ErrRes;
	return (PRM_RSLT_SUCCESS);		/* ���������ϐ��C��<S191> */
}

/****************************************************************************
 20BFH:���x���b�v���⏞��3�����ʑ�
****************************************************************************/
PRM_RSLT fnCalcRippleCompPhase3(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
//	PRM_RSLT	ErrRes;
	RPLCMP		*RippleCmp;

	RippleCmp = &(Axis->BaseLoops->RippleCmp);

	if(Cmd == PRM_ACCCMD_READ)
	{
		*pValue = (LONG)(RippleCmp->RplCmpPrm.RplCompPhaseEle[2]);
	}
	else
	{
		RippleCmp->RplCmpPrm.RplCompPhaseEle[2] = (USHORT)*pValue;

		/* ���b�v���␳�p�����[�^�v�Z */
//		FnRegCalVelRippleCompE( Axis );
		FnRegCalVelRippleCompE( Axis->UniPrms->Bprm, &Axis->BaseLoops->RippleCmp );/* <S1FD> */
	}

//	return ErrRes;
	return (PRM_RSLT_SUCCESS);		/* ���������ϐ��C��<S191> */
}

/****************************************************************************
 20C0H:���x���b�v���⏞��4��������
****************************************************************************/
PRM_RSLT fnCalcRippleCompCycle4(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
//	PRM_RSLT	ErrRes;
	RPLCMP		*RippleCmp;

	RippleCmp = &(Axis->BaseLoops->RippleCmp);

	if(Cmd == PRM_ACCCMD_READ)
	{
		*pValue = (LONG)(RippleCmp->RplCmpPrm.RplCompCycleEle[3]);
	}
	else
	{
		RippleCmp->RplCmpPrm.RplCompCycleEle[3] = (USHORT)*pValue;

		/* ���b�v���␳�p�����[�^�v�Z */
//		FnRegCalVelRippleCompE( Axis );
		FnRegCalVelRippleCompE( Axis->UniPrms->Bprm, &Axis->BaseLoops->RippleCmp );/* <S1FD> */
	}

//	return ErrRes;
	return (PRM_RSLT_SUCCESS);		/* ���������ϐ��C��<S1FD> */
}

/****************************************************************************
 20C1H:���x���b�v���⏞��4�����U���l
****************************************************************************/
PRM_RSLT fnCalcRippleCompAmplitude4(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
//	PRM_RSLT 	ErrRes;
	RPLCMP		*RippleCmp;

	RippleCmp = &(Axis->BaseLoops->RippleCmp);

	if(Cmd == PRM_ACCCMD_READ)
	{
		*pValue = (LONG)(RippleCmp->RplCmpPrm.RplCompAmplitudeEle[3]);
	}
	else
	{
		RippleCmp->RplCmpPrm.RplCompAmplitudeEle[3] = (SHORT)*pValue;

		/* ���b�v���␳�p�����[�^�v�Z */
//		FnRegCalVelRippleCompE( Axis );
		FnRegCalVelRippleCompE( Axis->UniPrms->Bprm, &Axis->BaseLoops->RippleCmp );/* <S1FD> */
	}

//	return ErrRes;
	return (PRM_RSLT_SUCCESS);		/* ���������ϐ��C��<S1FD> */
}

/****************************************************************************
 20C2H:���x���b�v���⏞��4�����ʑ�
****************************************************************************/
PRM_RSLT fnCalcRippleCompPhase4(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
//	PRM_RSLT	ErrRes;
	RPLCMP		*RippleCmp;

	RippleCmp = &(Axis->BaseLoops->RippleCmp);

	if(Cmd == PRM_ACCCMD_READ)
	{
		*pValue = (LONG)(RippleCmp->RplCmpPrm.RplCompPhaseEle[3]);
	}
	else
	{
		RippleCmp->RplCmpPrm.RplCompPhaseEle[3] = (USHORT)*pValue;

		/* ���b�v���␳�p�����[�^�v�Z */
//		FnRegCalVelRippleCompE( Axis );
		FnRegCalVelRippleCompE( Axis->UniPrms->Bprm, &Axis->BaseLoops->RippleCmp );/* <S1FD> */
	}

//	return ErrRes;
	return (PRM_RSLT_SUCCESS);		/* ���������ϐ��C��<S1FD> */
}

/****************************************************************************
 20C3H:���x���b�v���⏞��5��������
****************************************************************************/
PRM_RSLT fnCalcRippleCompCycle5(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
//	PRM_RSLT	ErrRes;
	RPLCMP		*RippleCmp;

	RippleCmp = &(Axis->BaseLoops->RippleCmp);

	if(Cmd == PRM_ACCCMD_READ)
	{
		*pValue = (LONG)(RippleCmp->RplCmpPrm.RplCompCycleEle[4]);
	}
	else
	{
		RippleCmp->RplCmpPrm.RplCompCycleEle[4] = (USHORT)*pValue;

		/* ���b�v���␳�p�����[�^�v�Z */
//		FnRegCalVelRippleCompE( Axis );
		FnRegCalVelRippleCompE( Axis->UniPrms->Bprm, &Axis->BaseLoops->RippleCmp );/* <S1FD> */
	}

//	return ErrRes;
	return (PRM_RSLT_SUCCESS);		/* ���������ϐ��C��<S1FD> */
}

/****************************************************************************
 20C4H:���x���b�v���⏞��5�����U���l
****************************************************************************/
PRM_RSLT fnCalcRippleCompAmplitude5(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
//	PRM_RSLT	ErrRes;
	RPLCMP		*RippleCmp;

	RippleCmp = &(Axis->BaseLoops->RippleCmp);

	if(Cmd == PRM_ACCCMD_READ)
	{
		*pValue = (LONG)(RippleCmp->RplCmpPrm.RplCompAmplitudeEle[4]);
	}
	else
	{
		RippleCmp->RplCmpPrm.RplCompAmplitudeEle[4] = (SHORT)*pValue;

		/* ���b�v���␳�p�����[�^�v�Z */
//		FnRegCalVelRippleCompE( Axis );
		FnRegCalVelRippleCompE( Axis->UniPrms->Bprm, &Axis->BaseLoops->RippleCmp );/* <S1FD> */
	}

//	return ErrRes;
	return (PRM_RSLT_SUCCESS);		/* ���������ϐ��C��<S1FD> */
}

/****************************************************************************
 20C5H:���x���b�v���⏞��5�����ʑ�
****************************************************************************/
PRM_RSLT fnCalcRippleCompPhase5(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
//	PRM_RSLT	ErrRes;
	RPLCMP		*RippleCmp;

	RippleCmp = &(Axis->BaseLoops->RippleCmp);

	if(Cmd == PRM_ACCCMD_READ)
	{
		*pValue = (LONG)(RippleCmp->RplCmpPrm.RplCompPhaseEle[4]);
	}
	else
	{
		RippleCmp->RplCmpPrm.RplCompPhaseEle[4] = (USHORT)*pValue;

		/* ���b�v���␳�p�����[�^�v�Z */
//		FnRegCalVelRippleCompE( Axis );
		FnRegCalVelRippleCompE( Axis->UniPrms->Bprm, &Axis->BaseLoops->RippleCmp );/* <S1FD> */
	}

//	return ErrRes;
	return (PRM_RSLT_SUCCESS);		/* ���������ϐ��C��<S1FD> */
}

/****************************************************************************
 20C6H:���x���b�v���⏞��6��������
****************************************************************************/
PRM_RSLT fnCalcRippleCompCycle6(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
//	PRM_RSLT	ErrRes;
	RPLCMP		*RippleCmp;

	RippleCmp = &(Axis->BaseLoops->RippleCmp);

	if(Cmd == PRM_ACCCMD_READ)
	{
		*pValue = (LONG)(RippleCmp->RplCmpPrm.RplCompCycleEle[5]);
	}
	else
	{
		RippleCmp->RplCmpPrm.RplCompCycleEle[5] = (USHORT)*pValue;

		/* ���b�v���␳�p�����[�^�v�Z */
//		FnRegCalVelRippleCompE( Axis );
		FnRegCalVelRippleCompE( Axis->UniPrms->Bprm, &Axis->BaseLoops->RippleCmp );/* <S1FD> */
	}

//	return ErrRes;
	return (PRM_RSLT_SUCCESS);		/* ���������ϐ��C��<S1FD> */
}
/****************************************************************************
 20C7H:���x���b�v���⏞��6�����U���l
****************************************************************************/
PRM_RSLT fnCalcRippleCompAmplitude6(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
//	PRM_RSLT	ErrRes;
	RPLCMP		*RippleCmp;

	RippleCmp = &(Axis->BaseLoops->RippleCmp);

	if(Cmd == PRM_ACCCMD_READ)
	{
		*pValue = (LONG)(RippleCmp->RplCmpPrm.RplCompAmplitudeEle[5]);
	}
	else
	{
		RippleCmp->RplCmpPrm.RplCompAmplitudeEle[5] = (SHORT)*pValue;

		/* ���b�v���␳�p�����[�^�v�Z */
//		FnRegCalVelRippleCompE( Axis );
		FnRegCalVelRippleCompE( Axis->UniPrms->Bprm, &Axis->BaseLoops->RippleCmp );/* <S1FD> */
	}

	//	return ErrRes;
		return (PRM_RSLT_SUCCESS);		/* ���������ϐ��C��<S1FD> */
}

/****************************************************************************
 20C8H:���x���b�v���⏞��6�����ʑ�
****************************************************************************/
PRM_RSLT fnCalcRippleCompPhase6(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
//	PRM_RSLT	ErrRes;
	RPLCMP		*RippleCmp;

	RippleCmp = &(Axis->BaseLoops->RippleCmp);

	if(Cmd == PRM_ACCCMD_READ)
	{
		*pValue = (LONG)(RippleCmp->RplCmpPrm.RplCompPhaseEle[5]);
	}
	else
	{
		RippleCmp->RplCmpPrm.RplCompPhaseEle[5] = (USHORT)*pValue;

		/* ���b�v���␳�p�����[�^�v�Z */
//		FnRegCalVelRippleCompE( Axis );
		FnRegCalVelRippleCompE( Axis->UniPrms->Bprm, &Axis->BaseLoops->RippleCmp );/* <S1FD> */
	}

//	return ErrRes;
	return (PRM_RSLT_SUCCESS);		/* ���������ϐ��C��<S1FD> */
}

/* <S14B> End */

/****************************************************************************
 0x20E0:�_�C�i�~�b�N�I�[�g�`���[�j���O�Q�C�����x���ݒ�
****************************************************************************/
PRM_RSLT	FnSetTLLvlGainLevel( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue )
{
	PRM_RSLT		ErrRes;
	TUNELESS_CTRL	*TuneLessCtrl;

	TuneLessCtrl = &(((AXIS_HANDLE*)Axis)->BaseControls->TuneLessCtrl);
	ErrRes = RpiFunSetDATGainLevel( TuneLessCtrl, (USHORT)*pValue );
	return( ErrRes );
}

/****************************************************************************
 0x20E1:�_�C�i�~�b�N�I�[�g�`���[�j���O���׃��x���ݒ�
****************************************************************************/
PRM_RSLT	FnSetTLLvlJratLevel( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue )
{
	PRM_RSLT	ErrRes;
	TUNELESS_CTRL	*TuneLessCtrl;

	TuneLessCtrl = &(((AXIS_HANDLE*)Axis)->BaseControls->TuneLessCtrl);
	ErrRes = RpiFunSetDATJratLevel( TuneLessCtrl, (USHORT)*pValue );
	return( ErrRes );
}

/****************************************************************************
 2108H:�g���[�X���샂�[�h
****************************************************************************/
PRM_RSLT fnCalcTraceMode(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRM_RSLT ErrRes;

	/* Sets trace mode. */

	/* Sets trace mode to Fun Manager. */
	ErrRes = FunSetTraceModeRegister(((AXIS_HANDLE*)Axis)->FnManager,
									 (ULONG)*pValue);
	if(ErrRes == PRM_RSLT_SUCCESS)
	{/* Success */

		/* Enter the Trace mode */
		ErrRes = DtrcWriteOpeModeTrcRegister( ((AXIS_HANDLE*)Axis)->TrcHndl,
																(USHORT)*pValue );
	}
	else
	{/* Error */
			ErrRes = PRM_RSLT_CALC_ERR;
	}

	return ErrRes;
}

/****************************************************************************
 2109H:�g���[�X����ݒ�
****************************************************************************/
PRM_RSLT fnCalcTraceCmd(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRM_RSLT ErrRes;

	/* Start the Trace mode */
	ErrRes = DtrcWriteOpeCmdTrcRegister( ((AXIS_HANDLE*)Axis)->TrcHndl,
								((AXIS_HANDLE*)Axis)->TraceBuffer,(USHORT)*pValue );
	return ErrRes;
}


/****************************************************************************
 2100H:�g���[�X���[�h
 2101H:�g���[�X�f�[�^��
 2102H:�g���[�X�o�b�t�@��
 2103H:�g���[�X�T���v�����O�Ԋu
 2104H:�g���[�X�T���v�����O����[us]
 2105H:�g���[�X�T���v�����O�Ԋu����
 2106H:�g���[�X�T���v�����O�Ԋu���
 2107H:�ő僌�[�X�o�b�t�@��
 2110H:�f�[�^�g���[�X�ΏۑI��
 2120H:I/O�f�[�^�g���[�X�ΏۑI��
 2130H:�g���[�X���A�h���X�w��
 2200H:�v���g���K�f�[�^��
 2201H:�g���K����
 2202H:�g���K���x��
 2204H:�g���K�ʒu
 2205H:�g���[�X����
 2206H:�A���g���[�X�����݃|�C���^
 2208H:�}���`�g���K���[�h
 2209H:�g���K����2
 220AH:�g���K���x��2
 ****************************************************************************/
PRM_RSLT fnCalcTraceRegisters(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG		ax;
	PRM_RSLT	ErrRes;
	AXIS_HANDLE	*AxisRsc;

	/* Sets the Trace Parameters */
	ErrRes = DtrcCalcParameter( ((AXIS_HANDLE*)Axis)->TrcHndl,
											((AXIS_HANDLE*)Axis)->TraceBuffer );

	/* Sets Trace and Table Drive Common Data */
	for( ax = 0; ax < NumberOfAxes; ax++ )
	{
		AxisRsc = GetAxisHandle(ax);
		TblDriveGetTraceRegData( &(AxisRsc->BaseControls->TblDrive),
								 ((AXIS_HANDLE*)Axis)->TrcHndl->TrcReg.DataSize,
								 ((AXIS_HANDLE*)Axis)->TrcHndl->TrcReg.SampIntv,
								 ((AXIS_HANDLE*)Axis)->TrcHndl->TrcExeP.DataSampIntv,
								 ((AXIS_HANDLE*)Axis)->TrcHndl->TrcExeP.TrcDnum );
	}
	return ErrRes;
}

/* <S0C9>	Start */
/****************************************************************************
 2110H:�f�[�^�AI/O�f�[�^�g���[�X1�`16�ΏۑI��
 ****************************************************************************/
PRM_RSLT fnCalcDataIoTraceSel(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)
{
	LONG		ax;
	PRM_RSLT	ErrRes;
	AXIS_HANDLE	*AxisRsc;
	MSGIF	*Hmsg_Com0;
	MSGIF	*Hmsg_Com1;
	TRCHNDL	*TrcHndl;
	ULONG	i;
	TRCREG	*TrcReg;
	TRCINDIVPRM	*TrcIncivPrm;

	/* Get Serial Message I/F for SigmaMsg/LcdOp/Memobus	*/
	Hmsg_Com0 = &SrlMsg[COM0_UART];		/* Serial CH:0 */
	Hmsg_Com1 = &SrlMsg[COM1_UART];		/* Serial CH:1 */


	TrcHndl = ((AXIS_HANDLE*)Axis)->TrcHndl;
	TrcReg  = &TrcHndl->TrcReg;
	TrcIncivPrm = &TrcHndl->TrcIncivPrm;

	/* Set Axis Address & Trace Data */
	for( i=0; i < TRC_CH_MAXNUM; i++ )
	{
		if( (Hmsg_Com0->CmdBuf[1] == 0x40) || (Hmsg_Com1->CmdBuf[1] == 0x40) )	/* 40H�R�}���h�ŃA�N�Z�X */
		{

			if(Hmsg_Com0->CmdBuf[1] == 0x40)		/* Serial CH:0 */
			{
				/* ���A�h���X�擾 */
				TrcIncivPrm->TrcAxis[i] = Hmsg_Com0->CmdBuf[5];
				TrcIncivPrm->IOTrcAxis[i] = Hmsg_Com0->CmdBuf[5];
			}
			else								/* Serial CH:1 */
			{
				/* ���A�h���X�擾 */
				TrcIncivPrm->TrcAxis[i] = Hmsg_Com1->CmdBuf[5];
				TrcIncivPrm->IOTrcAxis[i] = Hmsg_Com1->CmdBuf[5];
			}
		}
		else	/* 42H�R�}���h�ŃA�N�Z�X */
		{
			/* �g���[�X�Ώێ����u1�v�Ƃ��� */
			TrcIncivPrm->TrcAxis[i] = 0x0000;
			TrcIncivPrm->IOTrcAxis[i] = 0x000;
		}

		TrcIncivPrm->TrcData[i] = TrcReg->DataIOTrcSel[i] & 0x00FF;				/* �g���[�X�f�[�^�ݒ� */
		TrcIncivPrm->IOTrcData[i] = (TrcReg->DataIOTrcSel[i] >> 8) & 0x00FF;	/* I/O���ݒ� */
	}

	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
 2120H:�f�[�^�g���[�X�Ώێ��I���`�f�[�^�g���[�X�ێ��I��
 2130H:�f�[�^�g���[�X�Ώۃf�[�^�I��
 0x2140:I/O�g���[�X�Ώێ��I��
 0x2150:I/O�g���[�X�Ώۃf�[�^�I�� 
 ****************************************************************************/
PRM_RSLT fnCalcTraceDataSel(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)
{
	TRCHNDL	*TrcHndl;
	ULONG	i;
	TRCREG	*TrcReg;
	TRCINDIVPRM	*TrcIncivPrm;
	TrcHndl = ((AXIS_HANDLE*)Axis)->TrcHndl;
	TrcReg  = &TrcHndl->TrcReg;
	TrcIncivPrm = &TrcHndl->TrcIncivPrm;

	/* Set Axis Address & Trace Data */
	for( i=0; i < TRC_CH_MAXNUM; i++ )
	{
		TrcIncivPrm->TrcData[i] = TrcReg->TrcDataSel[i];		/* �f�[�^�g���[�X�Ώۃf�[�^�I��(1�`16)			*/
		TrcIncivPrm->TrcAxis[i] = TrcReg->TrcAxisSel[i];		/* �f�[�^�g���[�X�Ώێ��I��(1�`16)				*/
		TrcIncivPrm->IOTrcData[i] = TrcReg->IOTrcDataSel[i];	/* I/O�g���[�X1�Ώۃf�[�^�I��(1�`16)			*/
		TrcIncivPrm->IOTrcAxis[i] = TrcReg->IOTrcAxisSel[i];	/* I/O�g���[�X1�Ώێ��I��(1�`16)				*/
	}

//	return PRM_RSLT_SUCCESS;
	return( fnCalcTraceRegisters( Cmd, ArrayIdx, Axis, pValue ) );		/* <S208> */
}
/* <S0C9>	End */

/****************************************************************************
 2300H:�e�[�u���^�]���䃂�[�h
 2301H:�e�[�u���^�]��]�ʃ��~�b�g
 2303H:�e�[�u���^�]�@�\�I��
 ****************************************************************************/
PRM_RSLT fnCalcTblDriveRegisters(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRM_RSLT ErrRes;

/* <S0C9>	Start */
	TRCHNDL	*TrcHndl;
	ULONG	i;
	TRCREG	*TrcReg;
	TRCINDIVPRM	*TrcIncivPrm;
	TrcHndl = ((AXIS_HANDLE*)Axis)->TrcHndl;
	TrcReg  = &TrcHndl->TrcReg;
	TrcIncivPrm = &TrcHndl->TrcIncivPrm;
/* <S0C9>	End */

	/* Sets the Table Drive Parameters */
	ErrRes =
		TblDriveCalculateParameter( &(((AXIS_HANDLE*)Axis)->BaseControls->TblDrive),
									((AXIS_HANDLE*)Axis)->UniPrms->Bprm->XfbPulse1 );

/* <S0C9>	Start */
	/* Set Axis Address & Trace Data */
	for( i=0; i < TRC_CH_MAXNUM; i++ )
	{
		TrcIncivPrm->TrcData[i] = TrcReg->TrcDataSel[i];		/* �f�[�^�g���[�X�Ώۃf�[�^�I��(1�`16)			*/
		TrcIncivPrm->TrcAxis[i] = TrcReg->TrcAxisSel[i];		/* �f�[�^�g���[�X�Ώێ��I��(1�`16)				*/
		TrcIncivPrm->IOTrcData[i] = TrcReg->IOTrcDataSel[i];	/* I/O�g���[�X1�Ώۃf�[�^�I��(1�`16)			*/
		TrcIncivPrm->IOTrcAxis[i] = TrcReg->IOTrcAxisSel[i];	/* I/O�g���[�X1�Ώێ��I��(1�`16)				*/
	}
/* <S0C9>	End */

	/* ReCulc Trace Parameters for Trace Gain Calc */
	ErrRes = DtrcCalcParameter( ((AXIS_HANDLE*)Axis)->TrcHndl,
											((AXIS_HANDLE*)Axis)->TraceBuffer );

	return ErrRes;
}


/****************************************************************************
 2C00H:	���[�^�萔�G���A
****************************************************************************/
PRM_RSLT fnCalcMotorParameter(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	USHORT	mode;

	if(ArrayIdx >= 0x200)
	{
		ArrayIdx -= 0x200;
	}

	switch(Cmd)
	{
	case PRM_ACCCMD_WRITE:
	case PRM_ACCCMD_EEPWRITE:
		mode = FunGetOpeModeRegister(((AXIS_HANDLE*)Axis)->FnManager,FUN_CH1);
		if((mode == 0x200B) || (mode == 0x200C))
		{
			((AXIS_HANDLE*)Axis)->TraceBuffer[ArrayIdx] = (USHORT)*pValue;
		}
		break;
	case PRM_ACCCMD_READ:
		*pValue = (LONG)((AXIS_HANDLE*)Axis)->TraceBuffer[ArrayIdx];
		break;
	default:
		break;
	}
	return PRM_RSLT_SUCCESS;
}


/* <S061>	Start */
/****************************************************************************
0x2240	�A���[���g���[�X���s�c��
****************************************************************************/
PRM_RSLT fnAlmTrcRemainingNum(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
/* �{�֐��̏����̓A���[���g���[�X�@�\���������Ă���A�L���ɂ��܂��B */
#if 0

#define	MAX_ALM_TRC_NUM	3000	/* �s�����������ւ̃A���[���g���[�X�ő发�����݉� */

	*pValue = MAX_ALM_TRC_NUM - �A���[���g���[�X���s��;
#endif

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x2241	�A���[���g���[�X�������������ݏ��
****************************************************************************/
PRM_RSLT fnAlmTrcWritingStat(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
/* �{�֐��̏����̓A���[���g���[�X�@�\���������Ă���A�L���ɂ��܂��B */
#if 0

#define	NONE	0	/* �s�����������������݂Ȃ� */
#define	WRITING	1	/* �s�����������������ݏ�� */

	/* �A���[���g���[�X��Ԏ擾 */

	if(��Ԏ擾)	/* �s�����������������ݏ�� */
	{
		*pValue = WRITING;
	}
	else		/* �s�����������������݂Ȃ� */
	{
		*pValue = NONE;
	}

#endif

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x2242	�A���[���g���[�X�f�[�^�T�C�Y
****************************************************************************/
PRM_RSLT fnAlmTrcDataSize(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
/* �{�֐��̏����̓A���[���g���[�X�@�\���������Ă���A�L���ɂ��܂��B */
#if 0

#define	ALM_TRC_DATA_SIZE	4096	/* �s�����������������݂Ȃ� */




/* �A���[���g���[�X�f�[�^�T�C�Y	= �����g���[�X�� X �g���[�X�_��/1�{ X �A���[���g���[�X�w�b�_	*/
/*								= 7 x 500 + 596														*/
/*								= 4096[���W�X�^]													*/

	*pValue = ALM_TRC_DATA_SIZE;

#endif
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
0x2243	�A���[���g���[�X��~�v��
****************************************************************************/
PRM_RSLT fnAlmTrcStopReq(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
/* �{�֐��̏����̓A���[���g���[�X�@�\���������Ă���A�L���ɂ��܂��B */
#if 0

#define	ALM_TRC_DATA_SIZE	4096	/* �s�����������������݂Ȃ� */




/*
	�A���[���g���[�X�������������ݏ�� = 1�̏ꍇ�̓A���[���g���[�X���~���邱�Ƃ͂ł��܂���B
	�����I�ɑ҂���ԂƂȂ�܂��B
	
*/

if(�A���[����� = �������ݒ�)
{

	/* �������ݏ�Ԃ̏ꍇ�̏��������߂Ȃ���΂����Ȃ��B */
}
else/* �A���[���g���[�X�������ݒ��ł͂Ȃ� */
{
	/* �A���[���g���[�X��~�v��ON */
}

#endif
	return PRM_RSLT_SUCCESS;
}
/* <S061>	End */

/* <S0CB>	Start */
/***************************************************************************
2801H:	�Q�C�����@���[�^�d�����o����:U��
***************************************************************************/
PRM_RSLT FnSetCurDetectGainAdjU(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;
	Prm = Axis->UniPrms->Prm;

/*--------------------------------------------------------------------------------------------------*/
/*		�d�����o�[����																				*/
/*--------------------------------------------------------------------------------------------------*/
	/* U���d���[���� */
//	kx = (SHORT)(uCfgPrm->zadjiu);
//	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->AdinV.IuOffsetIn), kx, NO_LMT_CHK );
	/*----------------------------------------------------------------------------------------------*/
	/* V���d���[���� */
//	kx = (SHORT)(uCfgPrm->zadjiv);
//	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->AdinV.IvOffsetIn), kx, NO_LMT_CHK );
//	*pValue = (LONG)(Axis->SvAsicReg->MicroReg->IntAdv.IuIndata);

//	*pValue = (LONG)(Axis->SvAsicRegs->AsicMicroReg->AdinV.IuOffsetIn);	/* <S1B1> */
	*pValue = (LONG)(Axis->SvAsicRegs->AsicMicroReg->IntAdV.IuInData);	/* <S1B1> */

	return PRM_RSLT_SUCCESS;
}

/***************************************************************************
2802H:	�Q�C�����@���[�^�d�����o����:V��
***************************************************************************/
PRM_RSLT FnSetCurDetectGainAdjV(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

	PRMDATA	*Prm;
	Prm = Axis->UniPrms->Prm;

//	*pValue = (LONG)(Axis->SvAsicRegs->AsicMicroReg->AdinV.IvOffsetIn);	/* <S1B1> */
	*pValue = (LONG)(Axis->SvAsicRegs->AsicMicroReg->IntAdV.IvInData);	/* <S1B1> */

	return PRM_RSLT_SUCCESS;
}

/***************************************************************************
2803H:	PN����d��
***************************************************************************/
PRM_RSLT FnSetDcVoltx(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if (FLOAT_USE==TRUE)
	*pValue = Axis->PowerManager->PowerSts.DcVolt *
	                      Axis->PowerManager->DetectDcVolt.conf.Kdcvolt;
#else
	*pValue = MlibMulgain(Axis->PowerManager->PowerSts.DcVolt,
	                      Axis->PowerManager->DetectDcVolt.conf.Kdcvolt );
#endif /* FLOAT_USE */

	return PRM_RSLT_SUCCESS;
}

/***************************************************************************
2805H	�I�[�o�q�[�g�`�c���͒l���j�^�P
***************************************************************************/
PRM_RSLT FnInputOHAdMon1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)/* <S0E1> */
{

	CADC *pAdc;

	pAdc = Axis->pAdc;

//	*pValue = pAdc->Var[ADC3_OHIAD];/* Read OH A/D */
	*pValue = MlibMulgain(pAdc->Var[ADC3_OHIAD], OH_ADC2VOL);/* <S0E1> */

	return PRM_RSLT_SUCCESS;
}
/* <S0CB>	End */

/***************************************************************************
2806H	�I�[�o�q�[�g�`�c���͒l���j�^�Q
***************************************************************************//* <S0E1> */
PRM_RSLT FnInputOHAdMon2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

	CADC *pAdc;

	pAdc = Axis->pAdc;

	*pValue = MlibMulgain(pAdc->Var[ADC4_OHIAD], OH_ADC2VOL);

	return PRM_RSLT_SUCCESS;
}
#if 0	/* <S0FD> Delete Start : �d�l�ύX�ɂ��ȉ�2�̊֐����R�����g�A�E�g */
/* <S0E7> Start */
/***************************************************************************
2807H	�A�i���O���j�^PWM�o�͒������j�^U
***************************************************************************/
PRM_RSLT FnPwmAdjMonU(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	AMONOUT	*AmonOut;

	AmonOut = (AMONOUT*)&(Axis->AmonOut[0]);


	if(Cmd == PRM_ACCCMD_READ)
	{
		AmonOut[0].RegVoltMon_Flag_Ch0 = FALSE;
		*pValue = AmonOut[0].RegVoltMonSetVal_Ch0;
	}
	else
	{
		AmonOut[0].RegVoltMon_Flag_Ch0 = TRUE;
		FnOutputAnalogMonitor( Axis, *pValue, 0 );
	}

	return PRM_RSLT_SUCCESS;
}

/***************************************************************************
2808H	�A�i���O���j�^PWM�o�͒������j�^V
***************************************************************************/
PRM_RSLT FnPwmAdjMonV(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	AMONOUT	*AmonOut;

	AmonOut = (AMONOUT*)&(Axis->AmonOut[0]);


	if(Cmd == PRM_ACCCMD_READ)
	{
		AmonOut[0].RegVoltMon_Flag_Ch1 = FALSE;
		*pValue = AmonOut[1].RegVoltMonSetVal_Ch1;
	}
	else
	{
		AmonOut[0].RegVoltMon_Flag_Ch1 = TRUE;
		FnOutputAnalogMonitor( Axis, *pValue, 1 );
	}
	return PRM_RSLT_SUCCESS;
}
/* <S0E7> End */
#endif	/* <S0FD> Delete End:�d�l�ύX�ɂ�褏�L2�̊֐����R�����g�A�E�g */
/****************************************************************************
 3000H:	�G���W�j�A�����O�G���A
****************************************************************************/
PRM_RSLT fnAceessEngneeringInfo(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
//	EEP_DEVICE_HANDLE	*EepDevice; /* <S1D2> */
	UINT				AxisNo;
	USHORT				EepValue;

//	EepDevice = (Axis->RegManager->hPnReg)->hEepDev; /* <S1D2> */
	AxisNo = ((AXIS_HANDLE*)Axis)->AxisNo;
	EepValue = (USHORT)*pValue;

	switch(Cmd)
	{
	case PRM_ACCCMD_WRITE:
	case PRM_ACCCMD_EEPWRITE:
		/* write to EEPROM */
		EepdevWriteNoSumValues( 0, /* <S1D2> */
								EEP_NOSUM_ADDRESS(AxisNo, EEP_ENGINFO_OFS) + (ArrayIdx*2),
								&EepValue,
								1 );
		break;
	case PRM_ACCCMD_READ:
		/* read from EEPROM */
		EepdevReadValues( 0, /* <S1D2> */
						  EEP_NOSUM_ADDRESS(AxisNo, EEP_ENGINFO_OFS) + (ArrayIdx*2),
						  &EepValue,
						  1 );
		*pValue = (LONG)EepValue;
		break;
	default:
		break;
	}

	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
0x70000	EEPROM�_�C���N�g�A�N�Z�X�G���A(IF)
****************************************************************************/
static PRM_RSLT fnDirectEepromAcc(UINT devId, PRM_ACCCMD Cmd, UINT Address, LONG *pValue)
{
	USHORT EepValue;
	LONG err;

	EepValue = (USHORT)*pValue;

	switch(Cmd)
	{
	case PRM_ACCCMD_WRITE:
	case PRM_ACCCMD_EEPWRITE:
		/* write to EEPROM */
		err = EepdevWriteNoSumValues( devId, Address, &EepValue, 1 );
		break;
	case PRM_ACCCMD_READ:
		/* read from EEPROM */
		err = EepdevReadValues( devId, Address, &EepValue, 1 );
		*pValue = (LONG)EepValue;
		break;
	default:
		err = 0;
		break;
	}

	if(err != 0)
	{
		return PRM_RSLT_RWACC_ERR;
	}
	return PRM_RSLT_SUCCESS;
}

PRM_RSLT fnDirectEepromAccessIF(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
//	EEP_DEVICE_HANDLE	*EepDevice; /* <S1D2> */
//	EepDevice = (Axis->RegManager->hPnReg)->hEepDev; /* <S1D2> */
	return fnDirectEepromAcc(PRMDEF_EEPROM_IF, Cmd, ArrayIdx*2, pValue);
}

/****************************************************************************
0x74000	EEPROM�_�C���N�g�A�N�Z�X�G���A(COA)
****************************************************************************/
PRM_RSLT fnDirectEepromAccessCNV(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
//	EEP_DEVICE_HANDLE	*EepDevice;/* <S1D2> */
//	EepDevice = (Axis->RegManager->hPnReg)->hEepDev;/* <S1D2> */
	return fnDirectEepromAcc(PRMDEF_EEPROM_COA, Cmd, ArrayIdx*2, pValue);
}

/****************************************************************************
0x74080	EEPROM�_�C���N�g�A�N�Z�X�G���A(MBB)
****************************************************************************/
PRM_RSLT fnDirectEepromAccessMB(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
//	EEP_DEVICE_HANDLE	*EepDevice;/* <S1D2> */
//	EepDevice = (Axis->RegManager->hPnReg)->hEepDev;/* <S1D2> */
	return fnDirectEepromAcc(PRMDEF_EEPROM_MBB, Cmd, ArrayIdx*2, pValue);
}

/****************************************************************************
0x74800	EEPROM�_�C���N�g�A�N�Z�X�G���A(SDA)
****************************************************************************/
PRM_RSLT fnDirectEepromAccessAMP(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
//	EEP_DEVICE_HANDLE	*EepDevice;/* <S1D2> */
	UINT				idx;

	idx = (Axis->RegManager->hPnReg)->EepromBlockAddress + (ArrayIdx*2);
//	EepDevice = (Axis->RegManager->hPnReg)->hEepDev;/* <S1D2> */
	return fnDirectEepromAcc(PRMDEF_EEPROM_SDA, Cmd, idx, pValue);
}

/***************************************************************************
0x75000	�e�X�g���[�h
***************************************************************************/
PRM_RSLT fnTestMode(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	POW_MNG_HNDL	*hPowerManager;
	TEST_MODE		testMode;


	ASIC_HW_IF_STR		*pAsicHwReg;		/* <S0CB> */
	CHECK_MAIN_POWER	*PowerChecker;		/* <S0CB> */
	LONG				RlyDbWay;			/* <S0CB> */
//	POWER_STATUS		*pPowSts;			/* <S0CB> */
//	USHORT				FaultState;			/* <S0CB> */
//	BOOL				DBRequest;			/* <S0CB> */
//	BOOL				AdjCurAD;			/* <S0CB> */



	hPowerManager = Axis->PowerManager;
	testMode = (TEST_MODE)*pValue;

	pAsicHwReg = Axis->SvAsicRegs->AsicHwReg;				/* <S0CB> */
	PowerChecker = &hPowerManager->MainPowerChecker;		/* <S0CB> */
	RlyDbWay = PowerChecker->conf.RlyDbWay;					/* <S0CB> */
//	pPowSts = &hPowerManager->PowerSts;						/* <S0CB> */
//	FaultState |= Axis->CheckAlarm->faultState;				/* <S0CB> */
//	DBRequest |= Axis->BeSequence->DbOn;					/* <S0CB> */
//	AdjCurAD = Axis->FnCmnCtrl->KnlMode;					/* <S0CB> */


	switch(Cmd)
	{
		case PRM_ACCCMD_WRITE:
		case PRM_ACCCMD_EEPWRITE:
/* <S0CB>	Start */
/* if������switch���֕ύX(���򂪑�������) */
#if 0

			if( testMode == TEST_IF_WDTKICK )
			{
				HALdrv_KickIfWdt( );
			}
			else if( testMode == TEST_ASIC_WDTSTOP )
			{
				hPowerManager->MainPowerChecker.conf.TestMode4 = TRUE;
			}
			else if( testMode == TEST_ASIC_WDTKICK )
			{
				hPowerManager->MainPowerChecker.conf.TestMode4 = FALSE;
			}
			/* ���H�d���Ւf && DBCTRL_RLY_SEPARATE(DB, �˖h�����[����)��RELAY ON/OFF�����{���邱�ƁI */
			else if( testMode == TEST_RELAY_ON )
			{
				hPowerManager->MainPowerChecker.conf.TestMode5 = TRUE;
				/* ���[�^���X�e�X�g���[�h�łȂ��Ƃ�ON���� *//* <S1B7> */
				/* TODO: SGD-V�ł͈ȉ��̏�����OR���Ă���						*/
				/* (1) AxisRscC->BeSequence->StopMode == STOPMODE_DBSTOPFR		*/
				/* (2) AxisRscC->BeSequence->StopMode == STOPMODE_FREERUN		*/
				/* (3) AxisRscC->FnCmnCtrl->KnlMode == KPI_KMD_ADJCURAD			*/
				if( Axis->MotorLessTest->MotorLessTestMode == FALSE )
				{
					HALdrv_RelayControl( (void*)Axis->SvAsicRegs->AsicHwReg, TRUE ); /* <S0A2> */
				}
			}
			/* ���H�d���Ւf && DBCTRL_RLY_SEPARATE(DB, �˖h�����[����)��RELAY ON/OFF�����{���邱�ƁI */
			else if( testMode == TEST_RELAY_OFF )
			{
				hPowerManager->MainPowerChecker.conf.TestMode5 = FALSE;
				HALdrv_RelayControl( (void*)Axis->SvAsicRegs->AsicHwReg, FALSE ); /* <S0A2> */
			}
#endif

		switch(testMode)
		{
			case TEST_IF_WDTKICK:
				HALdrv_KickIfWdt( );
				break;

			case TEST_ASIC_WDTSTOP:
				hPowerManager->MainPowerChecker.conf.TestMode4 = TRUE;
				break;

			case TEST_ASIC_WDTKICK:
				hPowerManager->MainPowerChecker.conf.TestMode4 = FALSE;
				break;

			/* �˖h�����[ ON */
			case TEST_RELAY_ON: /* <S20E> */
				hPowerManager->MainPowerChecker.conf.TestMode5 = TRUE;
				switch ( RlyDbWay )
				{
				case DBCTRL_RLY_COMMON:			/* �˓��{�c�a�F�����[���� */
				case DBCTRL_TRIACK_SEPARATE:	/* �˓��F�����[�G�c�a�F�T�C���X�^ */
				case DBCTRL_RLY_SEPARATE:		/* �˓��F�����[�G�c�a�F�����[ */
					/* Relay On */
					HALdrv_RelayControl( (void*)Axis->SvAsicRegs->AsicHwReg, TRUE );
					break;
				case DBCTRL_TRIACK_COMMON:		/* �˓��{�c�a�F�T�C���X�^ */
					/* Relay On */
					HALdrv_RelayControl( (void*)Axis->SvAsicRegs->AsicHwReg, TRUE );
					/* DB Pulse Output */
					SHalDrv_DynamicBrakeON(pAsicHwReg);
					break;
				}
				break;

			/* �˖h�����[ OFF */
			case TEST_RELAY_OFF: /* <S20E> */
				hPowerManager->MainPowerChecker.conf.TestMode5 = FALSE;
				switch ( RlyDbWay )
				{
				case DBCTRL_RLY_COMMON:			/* �˓��{�c�a�F�����[���� */
				case DBCTRL_TRIACK_SEPARATE:	/* �˓��F�����[�G�c�a�F�T�C���X�^ */
				case DBCTRL_RLY_SEPARATE:		/* �˓��F�����[�G�c�a�F�����[ */
					/* Relay Off */
					HALdrv_RelayControl( (void*)Axis->SvAsicRegs->AsicHwReg, FALSE );
					break;
				case DBCTRL_TRIACK_COMMON:		/* �˓��{�c�a�F�T�C���X�^ */
					/* Relay Off */
					HALdrv_RelayControl( (void*)Axis->SvAsicRegs->AsicHwReg, FALSE );
					/* DB Pulse Output */
					SHalDrv_DynamicBrakeON(pAsicHwReg);
					break;
				}
				break;

			/* �񐶐��� ON */
			case TEST_REGENE_ON:
				hPowerManager->MainPowerChecker.conf.TestMode6 = TRUE;
				HALdrv_RegeneControl( TRUE );
				break;

			/* �񐶐��� OFF */
			case TEST_REGENE_OFF:
				hPowerManager->MainPowerChecker.conf.TestMode6 = FALSE;
				HALdrv_RegeneControl( FALSE );
				break;

			/* DB���� ON */
			case TEST_DB_ON: /* <S20E> */
				hPowerManager->MainPowerChecker.conf.TestMode7 = TRUE;
				switch ( RlyDbWay )
				{
				case DBCTRL_RLY_COMMON:			/* �˓��{�c�a�F�����[���� */
					/* Relay Off */
					HALdrv_RelayControl( (void*)Axis->SvAsicRegs->AsicHwReg, FALSE );
					break;
				case DBCTRL_TRIACK_SEPARATE:	/* �˓��F�����[�G�c�a�F�T�C���X�^ */
				case DBCTRL_RLY_SEPARATE:		/* �˓��F�����[�G�c�a�F�����[ */
					SHalDrv_DynamicBrakeON(pAsicHwReg);
					break;
				case DBCTRL_TRIACK_COMMON:		/* �˓��{�c�a�F�T�C���X�^ */
					/* Relay Off */
					HALdrv_RelayControl( (void*)Axis->SvAsicRegs->AsicHwReg, FALSE );
					/* DB Pulse Output */
					SHalDrv_DynamicBrakeON(pAsicHwReg);
					break;
				}
				break;

			/* DB���� OFF */
			case TEST_DB_OFF: /* <S20E> */
				hPowerManager->MainPowerChecker.conf.TestMode7 = FALSE;
				switch ( RlyDbWay )
				{
				case DBCTRL_RLY_SEPARATE:		/* �˓��F�����[�G�c�a�F�����[ */
					/* Relay On */
					HALdrv_RelayControl( (void*)Axis->SvAsicRegs->AsicHwReg, TRUE );
					break;
				case DBCTRL_RLY_COMMON:			/* �˓��{�c�a�F�����[���� */
				case DBCTRL_TRIACK_SEPARATE:	/* �˓��F�����[�G�c�a�F�T�C���X�^ */
					SHalDrv_DynamicBrakeOFF(pAsicHwReg);
					break;
				case DBCTRL_TRIACK_COMMON:		/* �˓��{�c�a�F�T�C���X�^ */
					/* Relay On */
					HALdrv_RelayControl( (void*)Axis->SvAsicRegs->AsicHwReg, TRUE );
					/* DB Pulse Output */
					SHalDrv_DynamicBrakeON(pAsicHwReg);
					break;
				}
				break;

			/* �Q�[�g�h���C�u�M�� ON */
			case TEST_GATE_DRIVE_ON:
				hPowerManager->MainPowerChecker.conf.TestMode8 = TRUE;
				SHalDrv_ChargePumpON(pAsicHwReg);	/* N���`���[�W�|���v ON */
				SHalDrv_ChargePumpON_P(pAsicHwReg);	/* P���`���[�W�|���v ON */
				break;

			/* �Q�[�g�h���C�u�M�� OFF */
			case TEST_GATE_DRIVE_OFF:
				hPowerManager->MainPowerChecker.conf.TestMode8 = FALSE;
				SHalDrv_ChargePumpOFF(pAsicHwReg);
				break;

/* <S19F> Start */
			/* �Q�[�g�h���C�u�M����ScanA�ɂ�ON/OFF�J�n(Duty = 50%, 8[kHz]��PWM�o�͊J�n) */
			case TEST_GATE_DRIVE_ON_OFF_START:
				hPowerManager->MainPowerChecker.conf.TestMode9 = TRUE;
				hPowerManager->MainPowerChecker.conf.GateDriveSigTestFlag = TRUE;	/* �Q�[�g�h���C�u�M����ԃt���O(ScanA) ON */
				SHalDrv_ChargePumpOFF(pAsicHwReg);									/* �Q�[�g�h���C�u�M�� OFF */
				break;

			/* �Q�[�g�h���C�u�M����ScanA�ɂ�ON/OFF��~(Duty = 50%, 8[kHz]��PWM�o�͒�~) */
			case TEST_GATE_DRIVE_ON_OFF_STOP:
				hPowerManager->MainPowerChecker.conf.TestMode9 = FALSE;
				hPowerManager->MainPowerChecker.conf.GateDriveSigTestFlag = FALSE;	/* �Q�Q�[�g�h���C�u�M����ԃt���O(ScanA) OFF */
				SHalDrv_ChargePumpOFF(pAsicHwReg);									/* �Q�[�g�h���C�u�M�� OFF */
				break;
/* <S19F> End */

			default:
				break;
		}
/* <S0CB>	End */
			break;

		default:
			break;
	}

	return PRM_RSLT_SUCCESS;
}

/***************************************************************************
0x2802	RAM monitor
***************************************************************************/
PRM_RSLT fnRamMonitor(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	static UINT32	RamAddress=0;

	switch(Cmd)
	{
	case PRM_ACCCMD_WRITE:
	case PRM_ACCCMD_EEPWRITE:
		RamAddress = *pValue;
		break;

	case PRM_ACCCMD_READ:
		if(RamAddress & 1)
		{	*pValue = *((UINT8 *)RamAddress);}
		else if(RamAddress & 2)
		{	*pValue = *((UINT16 *)RamAddress);}
		else
		{	*pValue = *((UINT32 *)RamAddress);}
		break;

	default:
		break;
	}

	return PRM_RSLT_SUCCESS;
}


/******************************* End of File *******************************/

