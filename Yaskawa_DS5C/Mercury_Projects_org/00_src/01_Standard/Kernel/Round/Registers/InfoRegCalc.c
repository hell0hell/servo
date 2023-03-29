/****************************************************************************
  Description   : Operation Register Table
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corp., Copyright (c) 2010
                  All Rights Reserved

  Project       : INGRAM

  Create Date   : 2010-10-20

****************************************************************************/
#include "Basedef.h"
#include "MercuryGlobals.h"
#include "PrmAccessIf.h"
#include "SpecCommon.h"		/* <S0C9> */

/* <S0C9>	Start */
/****************************************************************************
0x0000	�T�[�{�@��ID
****************************************************************************/
PRM_RSLT EInfReadServoID(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = SERVO_ID;
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x0001	�T�[�{�p�����[�^�Z�b�gID(bit0�`bit8)
****************************************************************************/
PRM_RSLT EInfReadServoPrmVer(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG ServoPrmVer;

	ServoPrmVer = (LONG)(ServoPrmVer | ((PRM_FORMAT_VERSION << 8)|(PRM_DATA_VERSION)));
	*pValue = ServoPrmVer;
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x0005	���䃂�W���[���t�@�[���E�F�A�o�[�W����(Y�d�lVer, Y�d�lVer���Ŕԍ�)
0x0006	���䃂�W���[���t�@�[���E�F�A�o�[�W����(�t�@�[���E�F�A�x�[�XVer)
****************************************************************************/
PRM_RSLT EInfReadFirmVersion(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG ServoFirmVer;

//	ServoFirmVer = (LONG)SERVO_FIRM_VER>>16;	/* <S0D1> */
	ServoFirmVer = (LONG)(( (SERVO_FIRM_VER>>16)<<16 ) | SERVO_SPECIAL_SPEC_VER);	/* <S0D1> */
	*pValue = ServoFirmVer;
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
0x0007	���䃂�W���[��Y�d�l�ԍ�
****************************************************************************/
PRM_RSLT EInfReadYspecVer(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG YspecVer;

	YspecVer = (LONG)SERVO_SPECIAL_SPEC_VER;

	*pValue = YspecVer;
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x000B	�T�[�{�p�b�N���͓d���^�o�͓d��
****************************************************************************/
PRM_RSLT EInfReadServoVoltage(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG ServoInVcode;
	LONG ServoOutVcode;
	LONG ServoVcode;

	ServoInVcode = (LONG)(Axis->UniPrms->Bprm->AmpInVcode); 
	ServoOutVcode = (LONG)(Axis->UniPrms->Bprm->AmpOutVcode); 

	ServoVcode = (ServoOutVcode << 8) | ServoInVcode;

	*pValue = ServoVcode;


	return PRM_RSLT_SUCCESS;
}

/* <S0C9>	End */

/****************************************************************************
0x000C	���[�^ID
****************************************************************************/
PRM_RSLT EInfReadMotorID(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = (LONG)(Axis->UniPrms->Prm)->MencP.typm_e.b.l;
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
0x000D	�G���R�[�_�^�C�v�^����\
****************************************************************************/
PRM_RSLT EInfReadEncoderTypeAndBits(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG x;
	x = (LONG)(Axis->UniPrms->Prm)->MencP.typm_e.b.h;
	x = (x & 0xF0) ? 0x0100L : 0x0000L;
	x |= (LONG)(Axis->UniPrms->Prm)->MencP.bit_dp.b.l;
	*pValue = x;
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
0x000E	�G���R�[�_�t�@�[���E�F�A�o�[�W����
****************************************************************************/
PRM_RSLT EInfReadEncoderVer(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = (LONG)(Axis->UniPrms->Prm)->MencP.encver;
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
0x000F	���[�^�e��[W]
****************************************************************************/
PRM_RSLT EInfReadMotorWattage(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG x;
	x = (LONG)(Axis->UniPrms->Bprm)->MotW;
	x = MlibScalKskxkx( x, 1, 1, NULL, -30 );

	*pValue = (LONG)((ArrayIdx == 0) ? x : ((ULONG)x >> 16));

	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
0x0011	���[�^�d��[V]
****************************************************************************/
PRM_RSLT EInfReadMotorVoltage(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = (LONG)(Axis->UniPrms->Bprm)->MotVcode;
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
0x0012	��Βl�G���R�[�_�^�C�v
****************************************************************************/
PRM_RSLT EInfReadEncoderType(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG x;
	x = (LONG)(Axis->UniPrms->Prm)->MencP.typm_e.b.h;
	*pValue = ((x >> 4) & 0x0F) - 1;		/* 2,1 --> 1,0					*/
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
0x001C	���[�^��i���x [��]�^�Fmin-1,���j�A�^�Fmm/s]
****************************************************************************/
PRM_RSLT EInfReadMotorRatedSpeed(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = (LONG)(Axis->UniPrms->Prm)->MencP.vrat_max.b.l;
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
0x001D	���[�^�ő呬�x [��]�^�Fmin-1,���j�A�^�Fmm/s]
****************************************************************************/
PRM_RSLT EInfReadMotorMaxSpeed(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = (LONG)(Axis->UniPrms->Prm)->MencP.vrat_max.b.h;
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
0x001E	���[�^���x�w�� [10�ׂ̂���]
****************************************************************************/
PRM_RSLT EInfReadMotorSpeedExp(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = (LONG)(Axis->UniPrms->Prm)->MencP.exp_motw_spd.b.h + 2;
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
0x001F	���[�^��i�g���N [��]�^�FN�m, ���j�A�^�FN]
****************************************************************************/
PRM_RSLT EInfReadMotorRatedTorque(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG	i, unit;
	LONG	x,y,z;
	LONG	kx,sx;
	KSGAIN	MaxTrq, ExpTrq;
	LONG	PerMaxTrq;
	LONG	rattrq, maxtrq;

	MaxTrq = (Axis->UniPrms->Bprm)->MaxTrq;
	PerMaxTrq = (LONG)(Axis->UniPrms->Bprm)->PerMaxTrq;
	ExpTrq = (Axis->UniPrms->Bprm)->ExpTrq;
	rattrq = (LONG)(Axis->UniPrms->Prm)->MencP.rattrq;
	maxtrq = (LONG)(Axis->UniPrms->Prm)->MencP.maxtrq;

	x = MlibScalKskxkx( MaxTrq, 10000, PerMaxTrq, NULL, -30 );	/* 100*��i�g���N	*/
	y = MlibScalKskxkx( MaxTrq, 100,   1,         NULL, -30 );	/* 100*�ő�g���N	*/
	unit = ( Axis->MencV->AxisMotType == MOTTYPE_LINEAR ) ? 1 : 100;
	kx = MlibScalKxgain( rattrq, maxtrq, unit, &sx,  -1 );
	z = MlibScalKskskx( kx, ExpTrq, 1, NULL, -30 );				/* 100*�ő�g���N	*/
	for( i=-2; y >= 10000; i++ ){ x /= 10; y /= 10; z /= 10; }	/* �X�P�[�����O		*/

	*pValue = x;						/* ��i�g���N [Nm,N]						*/
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
0x0020	���[�^�ő�g���N [��]�^�FN�m, ���j�A�^�FN]
****************************************************************************/
PRM_RSLT EInfReadMotorMaxTorque(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG	i,unit;
	LONG	x,y,z;
	LONG	kx,sx;
	KSGAIN	MaxTrq, ExpTrq;
	LONG	PerMaxTrq;
	LONG	rattrq, max_trq;

	MaxTrq = (Axis->UniPrms->Bprm)->MaxTrq;
	PerMaxTrq = (LONG)(Axis->UniPrms->Bprm)->PerMaxTrq;
	ExpTrq = (Axis->UniPrms->Bprm)->ExpTrq;
	rattrq = (LONG)(Axis->UniPrms->Prm)->MencP.rattrq;
	max_trq = (LONG)(Axis->UniPrms->Prm)->MencP.maxtrq;

	x = MlibScalKskxkx( MaxTrq, 10000, PerMaxTrq, NULL, -30 );	/* 100*��i�g���N	*/
	y = MlibScalKskxkx( MaxTrq, 100,   1,         NULL, -30 );	/* 100*�ő�g���N	*/
	unit = ( Axis->MencV->AxisMotType == MOTTYPE_LINEAR ) ? 1 : 100;
	kx = MlibScalKxgain( rattrq, max_trq, unit, &sx,  -1 );
	z = MlibScalKskskx( kx, ExpTrq, 1, NULL, -30 );				/* 100*�ő�g���N	*/
	for( i=-2; y >= 10000; i++ ){ x /= 10; y /= 10; z /= 10; }	/* �X�P�[�����O		*/

	*pValue = z;						/* �ő�g���N [Nm,N]						*/
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
0x0021	���[�^�g���N�w��[10�ׂ̂���]
****************************************************************************/
PRM_RSLT EInfReadMotorTorqueExp(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG	i, unit;
	LONG	x,y,z;
	LONG	kx,sx;
	KSGAIN	MaxTrq, ExpTrq;
	LONG	PerMaxTrq;
	LONG	rattrq, max_trq;

	MaxTrq = (Axis->UniPrms->Bprm)->MaxTrq;
	PerMaxTrq = (LONG)(Axis->UniPrms->Bprm)->PerMaxTrq;
	ExpTrq = (Axis->UniPrms->Bprm)->ExpTrq;
	rattrq = (LONG)(Axis->UniPrms->Prm)->MencP.rattrq;
	max_trq = (LONG)(Axis->UniPrms->Prm)->MencP.maxtrq;

	x = MlibScalKskxkx( MaxTrq, 10000, PerMaxTrq, NULL, -30 );	/* 100*��i�g���N	*/
	y = MlibScalKskxkx( MaxTrq, 100,   1,         NULL, -30 );	/* 100*�ő�g���N	*/
	unit = ( Axis->MencV->AxisMotType == MOTTYPE_LINEAR ) ? 1 : 100;
	kx = MlibScalKxgain( rattrq, max_trq, unit, &sx,  -1 );
	z = MlibScalKskskx( kx, ExpTrq, 1, NULL, -30 );				/* 100*�ő�g���N	*/
	for( i=-2; y >= 10000; i++ ){ x /= 10; y /= 10; z /= 10; }	/* �X�P�[�����O		*/

	*pValue = i;						/* �g���N�w�� */
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
0x0022	�G���R�[�_����\[��]�^�Fpulse/rev,���j�A�E�t���N�^�Fpulse/pitch]
****************************************************************************/
PRM_RSLT EInfReadEncoderResolution(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG x;
	x = (LONG)(Axis->UniPrms->Bprm)->XfbPulse2;
	*pValue = (LONG)((ArrayIdx == 0) ? x : ((ULONG)x >> 16));
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x0024	�}���`�^�[�����~�b�g�l
****************************************************************************/
PRM_RSLT EInfReadMultiTurnLimit(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG x;
	x = (LONG)((Axis->UniPrms->Prm)->MencP.limmlt);
	*pValue = (LONG)((ArrayIdx == 0) ? x : ((ULONG)x >> 16));
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x0026	�o�͉\�ő�g���N�@[��]�^�FN�m, ���j�A�^�FN]
****************************************************************************/
PRM_RSLT EInfReadMaxTorqueRef(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG	i, unit;
	LONG	x,y,z;
	LONG	kx,sx;
	KSGAIN	MaxTrq, ExpTrq;
	LONG	PerMaxTrq;
	LONG	rattrq, max_trq;

	MaxTrq = (Axis->UniPrms->Bprm)->MaxTrq;
	PerMaxTrq = (LONG)(Axis->UniPrms->Bprm)->PerMaxTrq;
	ExpTrq = (Axis->UniPrms->Bprm)->ExpTrq;
	rattrq = (LONG)(Axis->UniPrms->Prm)->MencP.rattrq;
	max_trq = (LONG)(Axis->UniPrms->Prm)->MencP.maxtrq;

	x = MlibScalKskxkx( MaxTrq, 10000, PerMaxTrq, NULL, -30 );	/* 100*��i�g���N	*/
	y = MlibScalKskxkx( MaxTrq, 100,   1,         NULL, -30 );	/* 100*�ő�g���N	*/
	unit = ( Axis->MencV->AxisMotType == MOTTYPE_LINEAR ) ? 1 : 100;
	kx = MlibScalKxgain( rattrq, max_trq, unit, &sx,  -1 );
	z = MlibScalKskskx( kx, ExpTrq, 1, NULL, -30 );				/* 100*�ő�g���N	*/
	for( i=-2; y >= 10000; i++ ){ x /= 10; y /= 10; z /= 10; }	/* �X�P�[�����O		*/

	*pValue = (y < z) ? y : z;	/* �o�͉\�ő�g���N[Nm,N]*/
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
0x0027	����\���[�^�ō����x [��]�^�Fmin-1,���j�A�^�Fmm/s]
****************************************************************************/
PRM_RSLT EInfReadMaxSpeedRef(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = (LONG)(Axis->UniPrms->Bprm)->MaxVel;
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
0x0028	�t���N�X�P�[���^�C�v
****************************************************************************/
PRM_RSLT EinfReadFullcScaleType(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = (Axis->UniPrms->Bprm)->FencUse & Axis->FencV->AbsoEncoder;
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x0029	��Βl���j�A�X�P�[�����_�ʒu�ݒ�
****************************************************************************/
PRM_RSLT EinfReadAbsScaleZeroSetting(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	MENCV	*MencV;

	MencV = ((Axis->UniPrms->Bprm)->FencUse != FALSE)
			? Axis->FencV: Axis->MencV;

	if((MencV->AbsoEncType != MENC_INCRE_TYPE)
		&& (MencV->SerialConvType == SCONVTYPE_MITUTOYO_ABS))
	{
		*pValue = 1;
	}
	else
	{
		*pValue = 0;
	}
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
0x002A	���j�A�X�P�[���s�b�`[pm]
****************************************************************************/
PRM_RSLT EInfReadLinearScalePitch(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG x;
	x = (LONG)(Axis->UniPrms->Bprm)->MonScalePitch;
	*pValue = (LONG)((ArrayIdx == 0) ? x : ((ULONG)x >> 16));
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
0x002C	���j�A�X�P�[���s�b�`�w��[10�ׂ̂���]
****************************************************************************/
PRM_RSLT EInfReadLinearScalePitchExp(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = (LONG)(Axis->UniPrms->Bprm)->MonScaleExp;
	return PRM_RSLT_SUCCESS;
}

/* <S0C9>	Start */
/****************************************************************************
0x0030	�T�[�{�p�b�N�t�@�[���E�F�A�u�[�g�o�[�W����
****************************************************************************/
PRM_RSLT EInfReadServoFirmwareBootVer(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)
{
#if 0
/* TODO */
	LONG ServoFirmwareBootVer;
	*pValue = ServoFirmwareBootVer;
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x0040	�w�߃I�v�V�����J�[�h�@��ID
****************************************************************************/
PRM_RSLT EInfReadCmdCardID(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)
{
#if 0
/* TODO */
	LONG CmdCardID;
	*pValue = CmdCardID;
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x0041	�w�߃I�v�V�����J�[�h�t�@�[���E�F�A�o�[�W����
****************************************************************************/
PRM_RSLT EInfReadCmdCardFirmwareVer(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)
{
#if 0
/* TODO */
	LONG CmdCardFirmwareVer;
	*pValue = CmdCardFirmwareVer;
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x0043	�w�߃I�v�V�����J�[�hY�d�l�ԍ�
****************************************************************************/
PRM_RSLT EInfReadCmdCardYspecVer(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)
{
#if 0
/* TODO */
	LONG CmdCardYspecVer;
	*pValue = CmdCardYspecVer;
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x0044	�Z�[�t�e�B���W���[���@��ID
****************************************************************************/
PRM_RSLT EInfReadSafetyID(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)
{
#if 0
/* TODO */
	LONG SafetyID;
	*pValue = SafetyID;
#endif

	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
0x0045	�Z�[�t�e�B���W���[���t�@�[���E�F�A�o�[�W����
****************************************************************************/
PRM_RSLT EInfReadSafetyFirmwareVer(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)
{
#if 0
/* TODO */
	LONG SafetyFirmwareVer;
	*pValue = SafetyFirmwareVer;
#endif

	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
0x0047	�Z�[�t�e�B���W���[��Y�d�l�ԍ�
****************************************************************************/
PRM_RSLT EInfReadSafetyYspecVer(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)
{
#if 0
/* TODO */
	LONG SafetyYspecVer;
	*pValue = SafetyYspecVer;
#endif

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x0048	�t�B�[�h�o�b�N�I�v�V�����J�[�h�@��ID
****************************************************************************/
PRM_RSLT EInfReadFbID(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)
{
#if 0
/* TODO */
	LONG FbID;
	*pValue = FbID;
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x0049	�t�B�[�h�o�b�N�I�v�V�����J�[�h�t�@�[���E�F�A�o�[�W����
****************************************************************************/
PRM_RSLT EInfReadFbFirmwareVer(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)
{
#if 0
/* TODO */
	LONG FbFirmwareVer;
	*pValue = FbFirmwareVer;
#endif
	return PRM_RSLT_SUCCESS;
}
/****************************************************************************
0x004B	�t�B�[�h�o�b�N�I�v�V�����J�[�hY�d�l�ԍ�
****************************************************************************/
PRM_RSLT EInfReadFbYspecVer(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)
{
#if 0
/* TODO */
	LONG FbYspecVer;
	*pValue = FbYspecVer;
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x004C	�t�B�[�h�o�b�N�I�v�V���������[�^ID
****************************************************************************/
PRM_RSLT EInfReadFbMotorID(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)
{
#if 0
/* TODO */
	LONG FbMotorID;
	*pValue = FbMotorID;
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x004D	�t�B�[�h�o�b�N�I�v�V���������[�^�e��[W]
****************************************************************************/
PRM_RSLT EInfReadFbMotorWattage(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)
{
#if 0
/* TODO */
	LONG FbMotorWattage;
	*pValue = FbMotorWattage;
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x004F	�t�B�[�h�o�b�N�I�v�V���������[�^�d��[V]
****************************************************************************/
PRM_RSLT EInfReadFbMotorVoltage(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)
{
#if 0
/* TODO */
	LONG FbMotorVoltage;
	*pValue = FbMotorVoltage;
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x0050	�t�B�[�h�o�b�N�I�v�V�������G���R�[�_�^�C�v�^����\
****************************************************************************/
PRM_RSLT EInfReadFbEncoderTypeAndBits(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)
{
#if 0
/* TODO */
	LONG FbEncoderTypeAndBits;
	*pValue = FbEncoderTypeAndBits;
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x0051	�t�B�[�h�o�b�N�I�v�V�������G���R�[�_����\
****************************************************************************/
PRM_RSLT EInfReadFbEncoderBits(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)
{
#if 0
/* TODO */
	LONG FbEncoderBits;
	*pValue = FbEncoderBits;
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x0053	�t�B�[�h�o�b�N�I�v�V��������Βl�G���R�[�_�^�C�v
****************************************************************************/
PRM_RSLT EInfReadFbAbsEncoderType(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)
{
#if 0
	Long FbAbsEncoderType;
	*pValue = FbAbsEncoderType;
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x0054	�t�B�[�h�o�b�N�I�v�V�������G���R�[�_�t�@�[���E�F�A�o�[�W����
****************************************************************************/
PRM_RSLT EInfReadFbEncoderVer(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)
{
#if 0
/* TODO */
	LONG FbEncoderVer;
	*pValue = FbEncoderVer;
#endif
	return PRM_RSLT_SUCCESS;
}

/* <S0C9>	End */

/****************************************************************************
0x0800	���䃂�W���[���`��������
****************************************************************************//* <S0D6> */
PRM_RSLT EinfReadModuleIDProductName(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	USHORT	usDate;

	usDate = (USHORT)(Axis->ModuleID->IfInfo->ProductName[ArrayIdx*2 + 1] & 0x00FF);
	usDate |= (USHORT)((Axis->ModuleID->IfInfo->ProductName[ArrayIdx*2] & 0x00FF) << 8);
	
	*pValue = (LONG)usDate;
	
	return PRM_RSLT_SUCCESS;
}
/****************************************************************************
0x0808	���䃂�W���[���V���A���i���o�[������
****************************************************************************//* <S0D6> */
PRM_RSLT EinfReadModuleIDSerialNumber(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	USHORT	usDate;

	usDate = (USHORT)(Axis->ModuleID->IfInfo->SerialNumber[ArrayIdx*2 + 1] & 0x00FF);
	usDate |= (USHORT)((Axis->ModuleID->IfInfo->SerialNumber[ArrayIdx*2] & 0x00FF) << 8);
	
	*pValue = (LONG)usDate;
	
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x0810	���䃂�W���[�������N��
****************************************************************************//* <S0D6> */
PRM_RSLT EinfReadModuleIDDate(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	USHORT	usDate;

	usDate = (USHORT)((Axis->ModuleID->IfInfo->Data & 0x00FF)<<8);
	usDate |= (USHORT)((Axis->ModuleID->IfInfo->Data & 0xFF00)>>8);
	
	*pValue = (LONG)usDate;
	
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x0811	���[�^�`��������
****************************************************************************//* <S0D6> */
PRM_RSLT EinfReadMotorProductName(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	USHORT	usDate;

	usDate = (USHORT)(Axis->MencV->MotInfo.Model[ArrayIdx*2 + 1] & 0x00FF);
	usDate |= (USHORT)((Axis->MencV->MotInfo.Model[ArrayIdx*2] & 0x00FF) << 8);
	
	*pValue = (LONG)usDate;
	
	return PRM_RSLT_SUCCESS;
}
/****************************************************************************
0x081C	���[�^�V���A���i���o�[������
****************************************************************************//* <S0D6> */
PRM_RSLT EinfReadMotorSerialNumber(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	USHORT	usDate;

	usDate = (USHORT)(Axis->MencV->MotInfo.SNo[ArrayIdx*2 + 1] & 0x00FF);
	usDate |= (USHORT)((Axis->MencV->MotInfo.SNo[ArrayIdx*2] & 0x00FF) << 8);
	
	*pValue = (LONG)usDate;
	
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
0x0824	���[�^�����N��
****************************************************************************//* <S0D6> */
PRM_RSLT EinfReadMotorManufactureDate(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	UCHAR	*px;
	USHORT	usDate;

	usDate = 0;
	px = Axis->MencV->MotInfo.Date;
	if(px[0] != '\0')
	{
		usDate = (((px[0]-'0')*10 + (px[1]-'0'))<<8)
						+ ((px[3]-'0')*10 + (px[4]-'0'));
	}
	*pValue = (LONG)usDate;
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x0825	�G���R�[�_�`���̈�
****************************************************************************//* <S0D6> */
PRM_RSLT EinfReadEncoderProductName(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	USHORT	usDate;

	usDate = (USHORT)(Axis->MencV->EncInfo.Model[ArrayIdx*2 + 1] & 0x00FF);
	usDate |= (USHORT)((Axis->MencV->EncInfo.Model[ArrayIdx*2] & 0x00FF) << 8);
	
	*pValue = (LONG)usDate;
	
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x082D	�G���R�[�_�V���A��No�̈�
****************************************************************************//* <S0D6> */
PRM_RSLT EinfReadEncoderSerialNumber(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	USHORT	usDate;

	usDate = (USHORT)(Axis->MencV->EncInfo.SNo[ArrayIdx*2 + 1] & 0x00FF);
	usDate |= (USHORT)((Axis->MencV->EncInfo.SNo[ArrayIdx*2] & 0x00FF) << 8);
	
	*pValue = (LONG)usDate;
	
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x0835	�G���R�[�_�����N���̈�
****************************************************************************//* <S0D6> */
PRM_RSLT EinfReadEncoderManufactureDate(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	UCHAR	*px;
	USHORT	usDate;

	usDate = 0;
	px = Axis->MencV->EncInfo.Date;
	if(px[0] != '\0')
	{
		usDate = (((px[0]-'0')*10 + (px[1]-'0'))<<8)
						+ ((px[3]-'0')*10 + (px[4]-'0'));
	}
	*pValue = (LONG)usDate;
	return PRM_RSLT_SUCCESS;
}

/* <S171> Start */
/****************************************************************************
0x0845	���䎲��
****************************************************************************//* <S0D6> */
PRM_RSLT EinfReadNumberOfAxes(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = NumberOfAxes;
	return PRM_RSLT_SUCCESS;
}
/* <S171> End */

/* <S0C9>	Start */
/****************************************************************************
0x0864	�w�߃I�v�V�����J�[�h�`��������
****************************************************************************/
PRM_RSLT EInfReadCmdType(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)
{
#if 0
/* TODO */
	LONG CmdType;
	*pValue = CmdCardYspecVer;
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x0870	�w�߃I�v�V�����J�[�h�V���A��No�̈�
****************************************************************************/
PRM_RSLT EInfReadCmdSerialNo(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)
{
#if 0
/* TODO */
	LONG CmdSerialNo;
	*pValue = CmdCardYspecVer;
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x087C	�w�߃I�v�V�����J�[�h�����N���̈�
****************************************************************************/
PRM_RSLT EInfReadCmdManufactureDate(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)
{
#if 0
/* TODO */
	LONG CmdManufactureDate;
	*pValue = CmdCardYspecVer;
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x087D	�Z�[�t�e�B���W���[���`��������
****************************************************************************/
PRM_RSLT EInfReadSafetyType(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)
{
#if 0
/* TODO */
	LONG SafetyType;
	*pValue = CmdCardYspecVer;
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x0889	�Z�[�t�e�B���W���[���V���A���i���o�[������
****************************************************************************/
PRM_RSLT EInfReadSafetySerialNo(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)
{
#if 0
/* TODO */
	LONG SafetySerialNo;
	*pValue = CmdCardYspecVer;
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x0895	�Z�[�t�e�B���W���[�������N��
****************************************************************************/
PRM_RSLT EInfReadSafetyManufactureDate(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)
{
#if 0
/* TODO */
	LONG SafetyManufactureDate;
	*pValue = CmdCardYspecVer;
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x0896	�t�B�[�h�o�b�N�I�v�V�����J�[�h�`��������
****************************************************************************/
PRM_RSLT EInfReadFbType(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)
{
#if 0
/* TODO */
	LONG FbType;
	*pValue = CmdCardYspecVer;
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x08A2	�t�B�[�h�o�b�N�I�v�V�����J�[�h�V���A���i���o�[������
****************************************************************************/
PRM_RSLT EInfReadFbSerialNo(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)
{
#if 0
/* TODO */
	LONG FbSerialNo;
	*pValue = CmdCardYspecVer;
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x08AE	�t�B�[�h�o�b�N�I�v�V�����J�[�h�����N��
****************************************************************************/
PRM_RSLT EInfReadFbManufactureDate(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)
{
#if 0
/* TODO */
	LONG FbManufactureDate;
	*pValue = CmdCardYspecVer;
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x08AF	�t�B�[�h�o�b�N�I�v�V���������[�^�`��������
****************************************************************************/
PRM_RSLT EInfReadFbMotorType(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)
{
#if 0
/* TODO */
	LONG FbMotorType;
	*pValue = CmdCardYspecVer;
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x08BA	�t�B�[�h�o�b�N�I�v�V���������[�^�V���A���i���o�[������
****************************************************************************/
PRM_RSLT EInfReadFbMotorSerialNo(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)
{
#if 0
/* TODO */
	LONG FbMotorSerialNo;
	*pValue = CmdCardYspecVer;
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x08C2	�t�B�[�h�o�b�N�I�v�V���������[�^�����N��
****************************************************************************/
PRM_RSLT EInfReadFbMotorManufactureDate(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)
{
#if 0
/* TODO */
	LONG FbMotorManufactureDate;
	*pValue = CmdCardYspecVer;
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x08C3	�t�B�[�h�o�b�N�I�v�V�������G���R�[�_�`��������
****************************************************************************/
PRM_RSLT EInfReadFbEncoderType(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)
{
#if 0
/* TODO */
	LONG FbEncoderType;
	*pValue = CmdCardYspecVer;
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x08CB	�t�B�[�h�o�b�N�I�v�V�������G���R�[�_�V���A���i���o�[������
****************************************************************************/
PRM_RSLT EInfReadFbEncoderSerialNo(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)
{
#if 0
/* TODO */
	LONG FbEncoderSerialNo;
	*pValue = CmdCardYspecVer;
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x08D3	�t�B�[�h�o�b�N�I�v�V�������G���R�[�_�����N��
****************************************************************************/
PRM_RSLT EInfReadFbEncoderManufactureDate(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)
{
#if 0
/* TODO */
	LONG FbEncoderManufactureDate;
	*pValue = CmdCardYspecVer;
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x08E0	���͐M�� �L��/�������
****************************************************************************/
PRM_RSLT EInfReadInputSignalInfo(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)
{
	*pValue = INPUT_SIGNAL_INFO;

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x08E2	�o�͐M�� �L��/�������
****************************************************************************/
PRM_RSLT EInfReadOutputSignalInfo(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)
{

	*pValue = OUTPUT_SIGNAL_INFO;

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x08E4	Safety���o�͐M�� �L��/�������
****************************************************************************/
PRM_RSLT EInfReadSafetyIOSignalInfo(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)
{
#if 0
/* TODO */
	LONG SafetyIOSignalInfo;
	*pValue = CmdCardYspecVer;
#endif
/* <S174> Start */
	PRMDATA		*Prm;
	Prm = ((AXIS_HANDLE*)Axis)->UniPrms->Prm;

	if( (Prm->opdetsw & 0x0100) == 0 )	/* PnE09 bit8 = 0:HWBB�@�\ ���Ή� */
	{
			*pValue = 0;				/* /HBWW1, /HBWW2 : ���T�|�[�g(����) */
	}
	else	/* PnE09 bit8 = 1:HWBB�@�\ �Ή� */
	{
			*pValue = 0x00000003;	/* /HBWW1, /HBWW2 : �T�|�[�g(�L��) */
	}
/* <S174> End */
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x08F0	�T�[�{�p�b�NBTO�ԍ�
****************************************************************************/
PRM_RSLT EInfReadServoBtoNo(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)
{
#if 0
/* TODO */
	LONG ServoBtoNo;
	*pValue = CmdCardYspecVer;
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x0910	�T�[�{�p�b�N����d�l���
****************************************************************************/
PRM_RSLT EInfReadServoSpecialSpec(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)
{
	LONG ServoSpecialSpec;

	/*-------------------------------*/
	/* �����ʎ��		���e		 */
	/*-------------------------------*/
	/* 		0x00		�W���d�l	 */
	/* 		0x01		FT�d�l		 */
	/* 		0x02		EX�d�l		 */
	/* 		0x03		Y�d�l		 */
	/*-------------------------------*/

	ServoSpecialSpec = (LONG)(SERVO_SPECIAL_SPEC_VER >> 16);
	*pValue = ServoSpecialSpec;

	return PRM_RSLT_SUCCESS;

}
/* <S0C9>	End */

/******************************* End of File *******************************/
