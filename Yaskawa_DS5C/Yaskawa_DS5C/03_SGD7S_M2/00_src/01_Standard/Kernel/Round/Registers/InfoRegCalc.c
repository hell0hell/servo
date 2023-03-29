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
0x0000	サーボ機種ID
****************************************************************************/
PRM_RSLT EInfReadServoID(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = SERVO_ID;
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x0001	サーボパラメータセットID(bit0～bit8)
****************************************************************************/
PRM_RSLT EInfReadServoPrmVer(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG ServoPrmVer;

	ServoPrmVer = (LONG)(ServoPrmVer | ((PRM_FORMAT_VERSION << 8)|(PRM_DATA_VERSION)));
	*pValue = ServoPrmVer;
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x0005	制御モジュールファームウェアバージョン(Y仕様Ver, Y仕様Ver改版番号)
0x0006	制御モジュールファームウェアバージョン(ファームウェアベースVer)
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
0x0007	制御モジュールY仕様番号
****************************************************************************/
PRM_RSLT EInfReadYspecVer(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG YspecVer;

	YspecVer = (LONG)SERVO_SPECIAL_SPEC_VER;

	*pValue = YspecVer;
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x000B	サーボパック入力電圧／出力電圧
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
0x000C	モータID
****************************************************************************/
PRM_RSLT EInfReadMotorID(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = (LONG)(Axis->UniPrms->Prm)->MencP.typm_e.b.l;
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
0x000D	エンコーダタイプ／分解能
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
0x000E	エンコーダファームウェアバージョン
****************************************************************************/
PRM_RSLT EInfReadEncoderVer(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = (LONG)(Axis->UniPrms->Prm)->MencP.encver;
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
0x000F	モータ容量[W]
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
0x0011	モータ電圧[V]
****************************************************************************/
PRM_RSLT EInfReadMotorVoltage(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = (LONG)(Axis->UniPrms->Bprm)->MotVcode;
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
0x0012	絶対値エンコーダタイプ
****************************************************************************/
PRM_RSLT EInfReadEncoderType(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG x;
	x = (LONG)(Axis->UniPrms->Prm)->MencP.typm_e.b.h;
	*pValue = ((x >> 4) & 0x0F) - 1;		/* 2,1 --> 1,0					*/
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
0x001C	モータ定格速度 [回転型：min-1,リニア型：mm/s]
****************************************************************************/
PRM_RSLT EInfReadMotorRatedSpeed(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = (LONG)(Axis->UniPrms->Prm)->MencP.vrat_max.b.l;
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
0x001D	モータ最大速度 [回転型：min-1,リニア型：mm/s]
****************************************************************************/
PRM_RSLT EInfReadMotorMaxSpeed(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = (LONG)(Axis->UniPrms->Prm)->MencP.vrat_max.b.h;
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
0x001E	モータ速度指数 [10のべき乗]
****************************************************************************/
PRM_RSLT EInfReadMotorSpeedExp(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = (LONG)(Axis->UniPrms->Prm)->MencP.exp_motw_spd.b.h + 2;
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
0x001F	モータ定格トルク [回転型：N･m, リニア型：N]
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

	x = MlibScalKskxkx( MaxTrq, 10000, PerMaxTrq, NULL, -30 );	/* 100*定格トルク	*/
	y = MlibScalKskxkx( MaxTrq, 100,   1,         NULL, -30 );	/* 100*最大トルク	*/
	unit = ( Axis->MencV->AxisMotType == MOTTYPE_LINEAR ) ? 1 : 100;
	kx = MlibScalKxgain( rattrq, maxtrq, unit, &sx,  -1 );
	z = MlibScalKskskx( kx, ExpTrq, 1, NULL, -30 );				/* 100*最大トルク	*/
	for( i=-2; y >= 10000; i++ ){ x /= 10; y /= 10; z /= 10; }	/* スケーリング		*/

	*pValue = x;						/* 定格トルク [Nm,N]						*/
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
0x0020	モータ最大トルク [回転型：N･m, リニア型：N]
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

	x = MlibScalKskxkx( MaxTrq, 10000, PerMaxTrq, NULL, -30 );	/* 100*定格トルク	*/
	y = MlibScalKskxkx( MaxTrq, 100,   1,         NULL, -30 );	/* 100*最大トルク	*/
	unit = ( Axis->MencV->AxisMotType == MOTTYPE_LINEAR ) ? 1 : 100;
	kx = MlibScalKxgain( rattrq, max_trq, unit, &sx,  -1 );
	z = MlibScalKskskx( kx, ExpTrq, 1, NULL, -30 );				/* 100*最大トルク	*/
	for( i=-2; y >= 10000; i++ ){ x /= 10; y /= 10; z /= 10; }	/* スケーリング		*/

	*pValue = z;						/* 最大トルク [Nm,N]						*/
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
0x0021	モータトルク指数[10のべき乗]
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

	x = MlibScalKskxkx( MaxTrq, 10000, PerMaxTrq, NULL, -30 );	/* 100*定格トルク	*/
	y = MlibScalKskxkx( MaxTrq, 100,   1,         NULL, -30 );	/* 100*最大トルク	*/
	unit = ( Axis->MencV->AxisMotType == MOTTYPE_LINEAR ) ? 1 : 100;
	kx = MlibScalKxgain( rattrq, max_trq, unit, &sx,  -1 );
	z = MlibScalKskskx( kx, ExpTrq, 1, NULL, -30 );				/* 100*最大トルク	*/
	for( i=-2; y >= 10000; i++ ){ x /= 10; y /= 10; z /= 10; }	/* スケーリング		*/

	*pValue = i;						/* トルク指数 */
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
0x0022	エンコーダ分解能[回転型：pulse/rev,リニア・フルク型：pulse/pitch]
****************************************************************************/
PRM_RSLT EInfReadEncoderResolution(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG x;
	x = (LONG)(Axis->UniPrms->Bprm)->XfbPulse2;
	*pValue = (LONG)((ArrayIdx == 0) ? x : ((ULONG)x >> 16));
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x0024	マルチターンリミット値
****************************************************************************/
PRM_RSLT EInfReadMultiTurnLimit(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG x;
	x = (LONG)((Axis->UniPrms->Prm)->MencP.limmlt);
	*pValue = (LONG)((ArrayIdx == 0) ? x : ((ULONG)x >> 16));
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x0026	出力可能最大トルク　[回転型：N･m, リニア型：N]
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

	x = MlibScalKskxkx( MaxTrq, 10000, PerMaxTrq, NULL, -30 );	/* 100*定格トルク	*/
	y = MlibScalKskxkx( MaxTrq, 100,   1,         NULL, -30 );	/* 100*最大トルク	*/
	unit = ( Axis->MencV->AxisMotType == MOTTYPE_LINEAR ) ? 1 : 100;
	kx = MlibScalKxgain( rattrq, max_trq, unit, &sx,  -1 );
	z = MlibScalKskskx( kx, ExpTrq, 1, NULL, -30 );				/* 100*最大トルク	*/
	for( i=-2; y >= 10000; i++ ){ x /= 10; y /= 10; z /= 10; }	/* スケーリング		*/

	*pValue = (y < z) ? y : z;	/* 出力可能最大トルク[Nm,N]*/
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
0x0027	動作可能モータ最高速度 [回転型：min-1,リニア型：mm/s]
****************************************************************************/
PRM_RSLT EInfReadMaxSpeedRef(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = (LONG)(Axis->UniPrms->Bprm)->MaxVel;
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
0x0028	フルクスケールタイプ
****************************************************************************/
PRM_RSLT EinfReadFullcScaleType(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = (Axis->UniPrms->Bprm)->FencUse & Axis->FencV->AbsoEncoder;
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x0029	絶対値リニアスケール原点位置設定
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
0x002A	リニアスケールピッチ[pm]
****************************************************************************/
PRM_RSLT EInfReadLinearScalePitch(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG x;
	x = (LONG)(Axis->UniPrms->Bprm)->MonScalePitch;
	*pValue = (LONG)((ArrayIdx == 0) ? x : ((ULONG)x >> 16));
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
0x002C	リニアスケールピッチ指数[10のべき乗]
****************************************************************************/
PRM_RSLT EInfReadLinearScalePitchExp(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = (LONG)(Axis->UniPrms->Bprm)->MonScaleExp;
	return PRM_RSLT_SUCCESS;
}

/* <S0C9>	Start */
/****************************************************************************
0x0030	サーボパックファームウェアブートバージョン
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
0x0040	指令オプションカード機種ID
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
0x0041	指令オプションカードファームウェアバージョン
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
0x0043	指令オプションカードY仕様番号
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
0x0044	セーフティモジュール機種ID
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
0x0045	セーフティモジュールファームウェアバージョン
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
0x0047	セーフティモジュールY仕様番号
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
0x0048	フィードバックオプションカード機種ID
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
0x0049	フィードバックオプションカードファームウェアバージョン
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
0x004B	フィードバックオプションカードY仕様番号
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
0x004C	フィードバックオプション内モータID
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
0x004D	フィードバックオプション内モータ容量[W]
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
0x004F	フィードバックオプション内モータ電圧[V]
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
0x0050	フィードバックオプション内エンコーダタイプ／分解能
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
0x0051	フィードバックオプション内エンコーダ分解能
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
0x0053	フィードバックオプション内絶対値エンコーダタイプ
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
0x0054	フィードバックオプション内エンコーダファームウェアバージョン
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
0x0800	制御モジュール形式文字列
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
0x0808	制御モジュールシリアルナンバー文字列
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
0x0810	制御モジュール製造年月
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
0x0811	モータ形式文字列
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
0x081C	モータシリアルナンバー文字列
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
0x0824	モータ製造年月
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
0x0825	エンコーダ形式領域
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
0x082D	エンコーダシリアルNo領域
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
0x0835	エンコーダ製造年月領域
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
0x0845	制御軸数
****************************************************************************//* <S0D6> */
PRM_RSLT EinfReadNumberOfAxes(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = NumberOfAxes;
	return PRM_RSLT_SUCCESS;
}
/* <S171> End */

/* <S0C9>	Start */
/****************************************************************************
0x0864	指令オプションカード形式文字列
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
0x0870	指令オプションカードシリアルNo領域
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
0x087C	指令オプションカード製造年月領域
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
0x087D	セーフティモジュール形式文字列
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
0x0889	セーフティモジュールシリアルナンバー文字列
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
0x0895	セーフティモジュール製造年月
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
0x0896	フィードバックオプションカード形式文字列
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
0x08A2	フィードバックオプションカードシリアルナンバー文字列
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
0x08AE	フィードバックオプションカード製造年月
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
0x08AF	フィードバックオプション内モータ形式文字列
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
0x08BA	フィードバックオプション内モータシリアルナンバー文字列
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
0x08C2	フィードバックオプション内モータ製造年月
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
0x08C3	フィードバックオプション内エンコーダ形式文字列
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
0x08CB	フィードバックオプション内エンコーダシリアルナンバー文字列
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
0x08D3	フィードバックオプション内エンコーダ製造年月
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
0x08E0	入力信号 有効/無効情報
****************************************************************************/
PRM_RSLT EInfReadInputSignalInfo(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)
{
	*pValue = INPUT_SIGNAL_INFO;

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x08E2	出力信号 有効/無効情報
****************************************************************************/
PRM_RSLT EInfReadOutputSignalInfo(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)
{

	*pValue = OUTPUT_SIGNAL_INFO;

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x08E4	Safety入出力信号 有効/無効情報
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

	if( (Prm->opdetsw & 0x0100) == 0 )	/* PnE09 bit8 = 0:HWBB機能 未対応 */
	{
			*pValue = 0;				/* /HBWW1, /HBWW2 : 未サポート(無効) */
	}
	else	/* PnE09 bit8 = 1:HWBB機能 対応 */
	{
			*pValue = 0x00000003;	/* /HBWW1, /HBWW2 : サポート(有効) */
	}
/* <S174> End */
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x08F0	サーボパックBTO番号
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
0x0910	サーボパック特殊仕様種別
****************************************************************************/
PRM_RSLT EInfReadServoSpecialSpec(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)
{
	LONG ServoSpecialSpec;

	/*-------------------------------*/
	/* 特殊種別種別		内容		 */
	/*-------------------------------*/
	/* 		0x00		標準仕様	 */
	/* 		0x01		FT仕様		 */
	/* 		0x02		EX仕様		 */
	/* 		0x03		Y仕様		 */
	/*-------------------------------*/

	ServoSpecialSpec = (LONG)(SERVO_SPECIAL_SPEC_VER >> 16);
	*pValue = ServoSpecialSpec;

	return PRM_RSLT_SUCCESS;

}
/* <S0C9>	End */

/******************************* End of File *******************************/
