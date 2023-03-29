/****************************************************************************
  Description   : Callback functions for Monitor Register
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corp., Copyright (c) 2010
                  All Rights Reserved

  Project       : INGRAM

  Create Date   : 2010-10-20

****************************************************************************/
#include "Basedef.h"
#include "MercuryGlobals.h"
#include "PrmAccessIf.h"
#include "HwDeviceIf.h"
#include "JL086ServoIpHw.h"
#include "KLib.h"
/* ---futami---> */
#include "OverheatManager.h"
/* <---futami--- */
#include "KnlApi.h"

/* <S056>	Start */
#include "HwDeviceIf.h"
#if (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE)	/* AnalogIF�ȊO */
#include "MtCtrl.h"
#include"MtApi.h"
extern 	MCI_CMD		cmd;
#else
//#include "Anlg.h"	/* <S0D0> Delete */
#include "ScanN.h"	/* <S0D0> :Anlg.h��ScanN.h��include���Ă���ׁA�R�����g�A�E�g����B*/
#endif

extern MSGIF	SrlMsg[SIF_N_CH];			/* Serial Message I/F for SigmaMsg/LcdOp/Memobus	*/
/* <S056>	End */

/****************************************************************************
0x000	���[�^��]/�ړ����x	SpdFbk	min-1, mm/s
****************************************************************************/
PRM_RSLT UnReadMotorSpeed(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG    val ;

	/* �b��I�ɃR�����g�A�E�g101227_kira ScanC�ɂď��� */
	//val = MlibMulgainSx48(Axis->pMonData->UnMotSpd, (Axis->prm_hndl_ptr->iprm_ptr)->Kmonspd);
	val = Axis->UnMonitor->UnMotSpd;//kira�ǉ�

	*pValue = MlibLimitul(val, 0x00007FFF, 0xFFFF8000);
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x001	���x�w��	SpdRef	min-1, mm/s
****************************************************************************/
PRM_RSLT UnReadVelocityRef(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG    val ;

//	val = MlibMulgainSx48(Axis->pMonData->UnSpdRef, (Axis->prm_hndl_ptr->iprm_ptr)->Kmonspd);
#if (FLOAT_USE==TRUE)
	val = Axis->BaseControls->CtrlCmdMngr.SpdRefo * (Axis->UniPrms->Bprm)->Kmonspd;
#else
	val = MlibMulgainSx48(Axis->BaseControls->CtrlCmdMngr.SpdRefo, (Axis->UniPrms->Bprm)->Kmonspd);
#endif /* FLOAT_USE */
	*pValue = MlibLimitul(val, 0x00007FFF, 0xFFFF8000);
	return PRM_RSLT_SUCCESS;
}
PRM_RSLT UnReadSpeedFeedForward(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG    val ;

//	val = MlibMulgainSx48(Axis->pMonData->UnSpdRef, (Axis->prm_hndl_ptr->iprm_ptr)->Kmonspd);
#if (FLOAT_USE==TRUE)
	val = Axis->BaseControls->CtrlCmdMngr.SpdFFCx * (Axis->UniPrms->Bprm)->Kmonspd;
#else
	val = MlibMulgainSx48(Axis->BaseControls->CtrlCmdMngr.SpdFFCx, (Axis->UniPrms->Bprm)->Kmonspd);
#endif /* FLOAT_USE */
	*pValue = MlibLimitul(val, 0x00007FFF, 0xFFFF8000);
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x002	�����g���N/���͎w��	TrqRef	%
****************************************************************************/
PRM_RSLT UnReadTorqueRef(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	/* �b��I�ɃR�����g�A�E�g101227_kira ScanC�ɂď��� */
	//*pValue = MlibMulgain(Axis->pMonData->UnTrqRef, (Axis->prm_hndl_ptr->iprm_ptr)->Kmontrq);
	*pValue = Axis->UnMonitor->UnTrqRef;//kira�ǉ�
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x003	�d�C�p1(32�r�b�g10�i�\��)	MotAngPls	pulse
****************************************************************************/
PRM_RSLT UnReadMotAngle(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = Axis->UnMonitor->UnMotAngle;
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x005	�d�C�p2(U��0deg����̊p�x)	MotAngDeg	deg
****************************************************************************/
PRM_RSLT UnReadMotPhase(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = Axis->UnMonitor->UnMotPhase;
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x006	���͐M�����j�^	InSignal
****************************************************************************/
#if 0
PRM_RSLT UnReadDigitalInputSignals(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	HWSIN sig;
	sig = HALdrv_ReadInputSignals( );
	*pValue = (LONG)sig.dw;
	return PRM_RSLT_SUCCESS;
}
#else
PRM_RSLT UnReadDigitalInputSignals(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = HALdrv_ReadMonInputSignals( Axis->AxisNo );
	return PRM_RSLT_SUCCESS;
}
#endif

/****************************************************************************
0x007	�o�͐M�����j�^	OutSignal
****************************************************************************/
#if 0
PRM_RSLT UnReadDigitalOutputSignals(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	HWSOUT sig;
	sig = HALdrv_ReadOuputSignals();
	*pValue = (LONG)sig.dw;
	return PRM_RSLT_SUCCESS;
}
#else
PRM_RSLT UnReadDigitalOutputSignals(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
//	*pValue = HALdrv_ReadMonOutputSignals( Axis->AxisNo );	/* <S1D7>:Un006�d�l�ύX */
	*pValue = HALdrv_ReadMonAllOutputSignals( );			/* <S1D7>:Un006�d�l�ύX */
	return PRM_RSLT_SUCCESS;
}
#endif

/****************************************************************************
0x008	���͎w�߃p���X���x	PosRefSpd	min-1, mm/s
****************************************************************************/
PRM_RSLT UnReadCmdSpeedCounter(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = Axis->UnMonitor->UnPcmd64ms;
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x00B	�ݐϕ��ח�	MotLoad %
****************************************************************************/
PRM_RSLT UnReadIsqrSum(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = Axis->UnMonitor->UnIsqrSum;
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x00C	�񐶕��ח�	RegLoad	%
****************************************************************************/
PRM_RSLT UnReadRegenePower(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
//	*pValue = Axis->UnMonitor->UnMeanRgPower;
	*pValue = Axis->PowerManager->RegeneCtrl.UnMeanRgPower; /*<S11E>*/
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x00D	�c�a��R����d��	DbrxPower	%
****************************************************************************/
PRM_RSLT UnReadDBPower(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = Axis->UnMonitor->UnMeanDbPower;
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x010	���[�^�G���R�[�_�ʒu	PfbkCounter	�G���R�[�_�p���X�P��
****************************************************************************/
PRM_RSLT UnReadMotorEncoderPosition(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
//	*pValue = Axis->BaseCtrlOut->MotPos;
	*pValue = Axis->UnMonitor->UnPfbkCntr;
	if( Cmd == PRM_ACCCMD_WRITE )		/* <S144> */
	{/* reset monitor value */
		Axis->UnMonitor->UnPfbkCntr = 0;
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x012	�t���N���[�Y�h�G���R�[�_�ʒu	FposCounter	�t���N�G���R�[�_�p���X�P��
****************************************************************************/
PRM_RSLT UnReadFullcEncoderPosition(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = (Axis->UniPrms->Bprm->FencUse != FALSE) ? Axis->BaseCtrlOut->FencPos : 0;
	if( Cmd == PRM_ACCCMD_WRITE )		/* <S144> */
	{/* reset monitor value */
		if( Axis->UniPrms->Bprm->FencUse != FALSE )
		{
			Axis->BaseCtrlOut->FencPos = 0;
		}
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x016	���׉ғ�����	PowerOnTime	100ms
****************************************************************************/
PRM_RSLT UnReadTimeStamp(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = KlibGetTimeStamp();
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x01A	�z�[���Z���T���
****************************************************************************/
PRM_RSLT UnReadPoleSensorInfo(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if 0 /* 2012.08.31 Y.Oka ����ScanA��CpuScanA�� */
	*pValue = (Axis->SvAsicRegs->AsicMicroReg)->MREG_POLEINFO;
#endif
	return PRM_RSLT_SUCCESS;
}

/* <S056>	Start */
/****************************************************************************
0x01B	�t�B�[�h�o�b�N�ʒu[�w�ߒP��]
****************************************************************************/
PRM_RSLT UnReadPfbkCntr(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = Axis->UnMonitor->UnPfbkCntr2;
	if( Cmd == PRM_ACCCMD_WRITE )		/* <S144> */
	{/* reset monitor value */
		Axis->UnMonitor->UnPfbkCntr2 = 0;
		Axis->UnMonitor->RemCntr2    = 0;
	}
	return PRM_RSLT_SUCCESS;
}

/* <S056>	End */


/****************************************************************************
0x01D	�L���Q�C�����j�^
****************************************************************************/
PRM_RSLT UnReadActGain(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = Axis->BaseControls->GainChange.ActGain;
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x01E	�Z�[�t�e�B���o�͐M�����j�^
****************************************************************************/
PRM_RSLT UnReadSafetySignals(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG hwbb;

	if( Axis->OptCardHdl->SftOpt.conf.OpInfo.f.Connect )	/* <S0C1> */
	{
			/* �Z�[�t�e�B�I�v�V�����L��̎� */
		*pValue = 0;
	}
	else
	{
			/* �Z�[�t�e�B�I�v�V�����Ȃ��̎� */
		hwbb = Axis->IoSignals->DetHwbbSts.Bbon1;
		hwbb |= (Axis->IoSignals->DetHwbbSts.Bbon2 << 1);
		*pValue = hwbb ^ 0x03;
	}

	return PRM_RSLT_SUCCESS;
}

/* <S154> Start */
/*****************************************************************************
  Description:	0x050 : ����FAN�����c����
------------------------------------------------------------------------------
  Parameters:
	Cmd;			PRM_ACCCMD
	ArrayIdx;		Array Index
	AXIS_HANDLE;	�� �n���h��
	pValue;			value
  Return:
	PRM_RSLT�G		PRM_RSLT_SUCCESS:����I���APRM_RSLT_CONTINUE�F����I�� �p����(Fn�p)�Ȃ�
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
PRM_RSLT UnReadFanLifeTime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
 #if ( CSW_SVCF_LIFEPRED_USE == TRUE )
    *pValue =  (LONG)Axis->hLifeTime->FanLifeTimeHdl.var.FanSurvivor ;
#else
    *pValue =  0;
#endif
	return PRM_RSLT_SUCCESS;
}
/*****************************************************************************
  Description:	0x051 : �R���f���T�����c����
------------------------------------------------------------------------------
  Parameters:
	Cmd;			PRM_ACCCMD
	ArrayIdx;		Array Index
	AXIS_HANDLE;	�� �n���h��
	pValue;			value
  Return:
	PRM_RSLT�G		PRM_RSLT_SUCCESS:����I���APRM_RSLT_CONTINUE�F����I�� �p����(Fn�p)�Ȃ�
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
PRM_RSLT UnReadCapaLifeTime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if ( CSW_SVCF_LIFEPRED_USE == TRUE )
	*pValue =  (LONG)Axis->hLifeTime->CapaLifeTimeHdl.var.CapaSurvivor ;
#else
	*pValue =  0;
#endif
	return PRM_RSLT_SUCCESS;
}

/*****************************************************************************
  Description:	0x052 : �˖h��H�����c����
------------------------------------------------------------------------------
  Parameters:
	Cmd;			PRM_ACCCMD
	ArrayIdx;		Array Index
	AXIS_HANDLE;	�� �n���h��
	pValue;			value
  Return:
	PRM_RSLT�G		PRM_RSLT_SUCCESS:����I���APRM_RSLT_CONTINUE�F����I�� �p����(Fn�p)�Ȃ�
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
PRM_RSLT UnReadRushLifeTime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if ( CSW_SVCF_LIFEPRED_USE == TRUE )
	*pValue =  (LONG)Axis->hLifeTime->RushLifeTimeHdl.var.RushSurvivor ;
#else
	*pValue =  0;
#endif
	return PRM_RSLT_SUCCESS;
}

/*****************************************************************************
  Description:	0x053 : DB��H�����c����
------------------------------------------------------------------------------
  Parameters:
	Cmd;			PRM_ACCCMD
	ArrayIdx;		Array Index
	AXIS_HANDLE;	�� �n���h��
	pValue;			value
  Return:
	PRM_RSLT�G		PRM_RSLT_SUCCESS:����I���APRM_RSLT_CONTINUE�F����I�� �p����(Fn�p)�Ȃ�
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
PRM_RSLT UnReadDBLifeTime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if ( CSW_SVCF_LIFEPRED_USE == TRUE )
//	*pValue =  (LONG)Axis->hLifeTime->DBLifeTimeHdl.var.DBSurvivor ;	/* <S180> */
	*pValue =  (LONG)Axis->hDBLifeTime->var.DBSurvivor ;				/* <S180> */
#else
	*pValue =  0;
#endif
	return PRM_RSLT_SUCCESS;
}

/*****************************************************************************
  Description:	0x054 : �˖h�����[�����
------------------------------------------------------------------------------
  Parameters:
	Cmd;			PRM_ACCCMD
	ArrayIdx;		Array Index
	AXIS_HANDLE;	�� �n���h��
	pValue;			value
  Return:
	PRM_RSLT�G		PRM_RSLT_SUCCESS:����I���APRM_RSLT_CONTINUE�F����I�� �p����(Fn�p)�Ȃ�
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
PRM_RSLT UnReadRushSwitchingCount(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if ( CSW_SVCF_LIFEPRED_USE == TRUE )
	*pValue =  (LONG)Axis->hLifeTime->RushLifeTimeHdl.var.RushSwitchCount ;
#else
	*pValue =  0;
#endif
	return PRM_RSLT_SUCCESS;
}

/*****************************************************************************
  Description:	0x056 : DB�����[�����
------------------------------------------------------------------------------
  Parameters:
	Cmd;			PRM_ACCCMD
	ArrayIdx;		Array Index
	AXIS_HANDLE;	�� �n���h��
	pValue;			value
  Return:
	PRM_RSLT�G		PRM_RSLT_SUCCESS:����I���APRM_RSLT_CONTINUE�F����I�� �p����(Fn�p)�Ȃ�
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
PRM_RSLT UnReadDBSwitchingCount(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if ( CSW_SVCF_LIFEPRED_USE == TRUE )
//	*pValue =  (LONG)Axis->hLifeTime->DBLifeTimeHdl.var.DBSwitchCount ;		/* <S180> */
	*pValue =  (LONG)Axis->hDBLifeTime->var.DBSwitchCount;					/* <S180> */
#else
	*pValue =  0;
#endif
	return PRM_RSLT_SUCCESS;
}
/* <S154> End */

/****************************************************************************
0x02C	���[�^��i���x [��]�^�Fmin-1,���j�A�^�Fmm/s]			<S11C>
****************************************************************************/
PRM_RSLT UnReadMotorRatedSpeed(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)
{
	LONG	lwk;

	lwk = 100 * (LONG)(((AXIS_HANDLE*)Axis)->UniPrms->Prm)->MencP.vrat_max.b.l;
	*pValue = MlibScalKskxkx( (((AXIS_HANDLE*)Axis)->UniPrms->Bprm)->ExpSpd, lwk, 1, NULL, -24 );

	return PRM_RSLT_SUCCESS;

}

/****************************************************************************
0x02C	���[�^�ő呬�x [��]�^�Fmin-1,���j�A�^�Fmm/s]		<S11C>
****************************************************************************/
PRM_RSLT UnReadMotorMaxSpeed(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)
{
	LONG	lwk;
	
	lwk = 100 * (LONG)(((AXIS_HANDLE*)Axis)->UniPrms->Bprm)->MaxVel;
	*pValue = MlibScalKskxkx( (LONG)(((AXIS_HANDLE*)Axis)->UniPrms->Bprm)->ExpSpd, lwk, 1, NULL, -24 );
	
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x02C	�T�[�{�p�b�N�ݒu�����j�^ [%]						<S03A>
****************************************************************************/
PRM_RSLT	UnAmpInstEnvMon( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue )
{
	LONG	lwk;

	/*----------------------------------------------------------------------*/
	/*		�ݒu�����j�^���Z	�F0.1���~2ms�~1000�񕪂̐ώZ�l				*/
	/*----------------------------------------------------------------------*/
//	lwk = Axis->CheckAlarm->EnvMon.var.UnInstEnvMonAmp/1000
//					+ Axis->CheckAlarm->EnvMon.conf.AmpEnvMonOffset;		  /* <S050> */
#if 0/* <S1D7>:SyscMakeMonitorData( )�ō쐬���� */
	lwk = (Axis->EnvMonitor->var.UnInstEnvMonAmp * 0.0001)
									+ Axis->EnvMonitor->conf.AmpEnvMonOffset;
	*pValue = lwk;
#endif
	*pValue = Axis->UnMonitor->UnInstEnvMonAmp;	/* <S1D7> */

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x02D	�T�[�{���[�^�ݒu�����j�^ [%]						<S03A>
****************************************************************************/
PRM_RSLT	UnMotInstEnvMon( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue )
{
	LONG	lwk;

	/*----------------------------------------------------------------------*/
	/*		�ݒu�����j�^���Z	�F1���~2ms�~1000�񕪂̐ώZ�l						*/
	/*----------------------------------------------------------------------*/
//	lwk = Axis->CheckAlarm->EnvMon.var.UnInstEnvMonMot/1000
//					+ Axis->CheckAlarm->EnvMon.conf.MotEnvMonOffset;		  /* <S050> */
#if 0/* <S1D7>:SyscMakeMonitorData( )�ō쐬���� */
	lwk = (Axis->EnvMonitor->var.UnInstEnvMonMot * 0.001)
										+ Axis->EnvMonitor->conf.MotEnvMonOffset;
	*pValue = lwk;
#endif
	*pValue = Axis->UnMonitor->UnInstEnvMonMot;	/* <S1D7> */

	return PRM_RSLT_SUCCESS;
}

/* <S056>	Start */
/****************************************************************************
0x002E	���H�d���l
****************************************************************************/
PRM_RSLT UnReadDcVolt(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)
{
	*pValue = GET_PN_DC_VOLT( );

	return PRM_RSLT_SUCCESS;
}
/* <S056>	End */

/****************************************************************************
0x050-0x53	�������j�^ [0.01%] �b��� TODO						<S03A>
****************************************************************************/
PRM_RSLT	UnLifeTimeMon( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue )
{
	*pValue = 1;

	return PRM_RSLT_SUCCESS;
}

/* <S056>	Start */
/****************************************************************************
0x054	�˖h�����[�����
****************************************************************************/
PRM_RSLT	UnReadRelayCnt( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue )
{
	/* TODO */
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x056	DB�����[�����
****************************************************************************/
PRM_RSLT	UnReadDBRelayCnt( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue )
{
	/* TODO */
	return PRM_RSLT_SUCCESS;
}
/* <S056>	End */

/****************************************************************************
0x080	���Ɍ��o���������ő�ړ���	MpfMovePosP	0.01mm
****************************************************************************/
PRM_RSLT UnReadMpfMaxPositiveMoveDist(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG	lwk;
	LONG	lwk2;

	if( Axis->MencV->AxisMotType == MOTTYPE_ROTARY )
	{
		lwk = Axis->MpFind->UnMpfMovePosP / 10;
#if (FLOAT_USE==TRUE)
		lwk2 = ( (float)lwk * Axis->MencV->Kmovpos );
#else
		lwk2 = MlibMulgain( lwk, Axis->MencV->Kmovpos );
#endif /* FLOAT_USE */
	}
	else
	{
		lwk = Axis->MpFind->UnMpfMovePosP;
#if (FLOAT_USE==TRUE)
		lwk2 = ( (float)lwk * Axis->MencV->Kmovpos );
#else
		lwk2 = MlibMulgain( lwk, Axis->MencV->Kmovpos );
#endif /* FLOAT_USE */
		if( lwk2 > 32767 )
		{
			lwk2 = 32767;
		}
	}
	*pValue = lwk2;
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x081	���Ɍ��o���t�����ő�ړ���	MpfMovePosN	0.01mm
****************************************************************************/
PRM_RSLT UnReadMpfMaxNegativeMoveDist(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG	lwk;
	LONG	lwk2;

	if( Axis->MencV->AxisMotType == MOTTYPE_ROTARY )
	{
		lwk = Axis->MpFind->UnMpfMovePosN / 10;
#if (FLOAT_USE==TRUE)
		lwk2 = ( (float)lwk * Axis->MencV->Kmovpos );
#else
		lwk2 = MlibMulgain( lwk, Axis->MencV->Kmovpos );
#endif /* FLOAT_USE */
	}
	else
	{
		lwk = Axis->MpFind->UnMpfMovePosN;
#if (FLOAT_USE==TRUE)
		lwk2 = ( (float)lwk * Axis->MencV->Kmovpos );
#else
		lwk2 = MlibMulgain( lwk, Axis->MencV->Kmovpos );
#endif /* FLOAT_USE */
		if( lwk2 < -32767 )
		{
			lwk2 = -32767;
		}
	}
	*pValue = lwk2;
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x082	���Ɍ��o���v����	MpfFindingTime	0.1s
****************************************************************************/
PRM_RSLT UnReadMpfDetectingTime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG scantime;
	scantime = (Axis->UniPrms->Bprm)->SvCycleUs;
	*pValue = (Axis->MpFind->UnMpFindingTime * scantime) / 100000;
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x083	���Ɍ��o�ʑ��I�t�Z�b�g��	MpfPhaseOffset	0.1deg
****************************************************************************/
PRM_RSLT UnReadMpfPhaseOffset(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG	lwk;

	lwk = (Axis->MpFind->MpfPhaseOffset * (SHORT)3600) >> 16;
	if( lwk < 0 )
	{
		lwk += 3600;
	}
	*pValue = lwk;
	return PRM_RSLT_SUCCESS;
}

/* <S056>	Start */
/****************************************************************************
0xE084	���j�A�X�P�[���s�b�`[pm]
****************************************************************************/
PRM_RSLT UnReadLinearScalePitch(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG x;
	x = (LONG)(Axis->UniPrms->Bprm)->MonScalePitch;
	*pValue = (LONG)((ArrayIdx == 0) ? x : ((ULONG)x >> 16));
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
0xE086	���j�A�X�P�[���s�b�`�w��[10�ׂ̂���]
****************************************************************************/
PRM_RSLT UnReadLinearScalePitchExp(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = (LONG)(Axis->UniPrms->Bprm)->MonScaleExp;
	return PRM_RSLT_SUCCESS;
}
/* <S056>	End */

/****************************************************************************
0x087	���Ɍ��o���胂�j�^
****************************************************************************/
PRM_RSLT UnReadMpfStatus(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG *MPJudgeMonitor;

	MPJudgeMonitor = Axis->MpFind->MPJudgeMonitor;
	*pValue = MPJudgeMonitor[0] + (MPJudgeMonitor[1] * 10) + (MPJudgeMonitor[2] * 100);

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x104	�V���A���G���R�[�_�ʐM�ُ��
****************************************************************************/
PRM_RSLT UnReadEncoderErrorCount(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = ((Axis->SvAsicRegs->AsicHwReg)->AREG_COMERRCNT - Axis->MencV->EncComErrCnt);		/*<S009>*//* <S217> */
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x107	�c���U�����g��	[0.1Hz]
****************************************************************************/
PRM_RSLT UnReadRvibFrequency(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = Axis->RemVibFreq->mon.UnRvibFrequency;				/* <S0DA> */
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x110	�����M�����j�^
****************************************************************************/
PRM_RSLT UnReadInterSignalX(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG	sig, foo;

/* <S0D0> Start */
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)	/* AnalogIF */	
	NINV *pninv = &NinV;
#endif
/* <S0D0> End */

	/* 0bit ���H�I�� */
	sig = (Axis->SeqCtrlOut->MainPowerOn << 0);
	/* bit1 �G���R�[�_���f�B */
	sig |= (Axis->MencV->EncReady << 1);
	/* bit2 ���[�^�ʓd(�v��) */
	sig |= (Axis->BeSequence->DbSeqData.DbBeReq << 2);
	/* bit3 �_�C�i�~�b�N�u���[�L�I�� */
	sig |= (Axis->BeSequence->DbOn << 3);

	/* bit4 �t��]�i�ړ��j���[�h */
	sig |= (Axis->UniPrms->Bprm->RvsDir << 4);
	/* bit5 ���[�h�X�C�b�`�I�� */
	sig |= (Axis->BaseControls->ModeSwData.var.ModeSWFlag << 5);

	switch( Axis->BaseCtrlOut->CtrlModeOut.b.cm )
	{
	case CTRLMODE_POS:
		/* bit8 �ʒu�w�ߓ��͒� */
		sig |= ((Axis->SeqMotSts->var.dPcmdCnt != 0) << 8);
		/* bit9 �ʒu�w�ߕ��� */
		sig |= ((Axis->SeqMotSts->var.dPcmdCnt >= 0) << 9);
		break;
	case CTRLMODE_TRQ:
		/* bit7 �g���N�w�ߓ��͒� */
		sig |= ((Axis->BaseControls->CtrlCmdMngr.TrqRefo != 0) << 7);
		break;
	case CTRLMODE_SPD:
	default:
		/* bit6 ���x�w�ߓ��͒� */
		sig |= ((Axis->BaseControls->CtrlCmdMngr.SpdRefi != 0) << 6);
		break;
	}

	/* bit12 �𗬓d�������� */
	sig |= (Axis->PowerManager->PowerSts.Acon << 12);
	/* bit13 �˓���R�Z�������[�I�� */
	sig |= (Axis->PowerManager->PowerSts.ChargeOk << 13);
	/* bit14 �񐶂s���I�� */
	sig |= (HALdrv_GetRegeneControlState() << 14);
	/* bit15 �񐶈ُ팟�o�I�� */
	sig |= (Axis->PowerManager->RegeneCtrl.RegChk.RegeAlmSignal << 15);

	/* bit16 ���[�^�ʓd(���) */
	sig |= (Axis->BeSequence->BeReqSeqOut << 16);
	/* bit17 �\�� */
	/* bit18 �ߓd�� */
	sig |= (ALMCheckEachState(Axis->AlmManager, ALM_OC) << 18);
	/* bit19 ���_���ʉߊ��� */
	sig |= ((Axis->MencV->RxAlmCode & 0x0040) << (19-6));

	/* bit20 �C�i�[�V�����蒆 */
//	sig |= (BoutV.f.InertiaTuning << 20);
	/* bit21 ���Ɍ��o�� */
	foo = ( (Axis->BeSequence->BeReqSeqOut != FALSE)
			&& (Axis->MencV->PhaseReady == FALSE) );
	sig |= (foo << 21);
	/* bit22 ���ɔF������ */
	sig |= (Axis->MencV->PhaseReady << 22);

	/* bit23 ���b�v���⏞��  */
//	sig |= ( ( Axis->BaseLoops->RippleCmp.RplCmpPrm.RplCmpUse ) << 23 );	/* <S14B> */
	sig |= ( ( Axis->BaseLoops->RippleCmp.RippleCompMon ) << 23 );/* <S1FD> */


/* <S0D0> Start */
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)	/* AnalogIF */	
	/* bit26 CLR�M��  */
	sig |= ( ( pninv->NrmBits.CLR ) << 26 );
#endif
/* <S0D0> End */

	/**
	 * @@ bit24: PULS signal
	 *    bit25: SIGN signal
	 *    bit27: PSEL signal
	 */
	*pValue = sig;
	return PRM_RSLT_SUCCESS;
}

/* <S21D> Start�@*/
/****************************************************************************
0x110	�����M�����j�^ 1
****************************************************************************/
PRM_RSLT UnReadInterSignalX1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG	sig, foo;

/* <S0D0> Start */
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)	/* AnalogIF */
	NINV *pninv = &NinV;
#endif
/* <S0D0> End */

	/* 0bit ���H�I�� */
	sig = (Axis->SeqCtrlOut->MainPowerOn << 0);
	/* bit1 �G���R�[�_���f�B */
	sig |= (Axis->MencV->EncReady << 1);
	/* bit2 ���[�^�ʓd(�v��) */
	sig |= (Axis->BeSequence->DbSeqData.DbBeReq << 2);
	/* bit3 �_�C�i�~�b�N�u���[�L�I�� */
	sig |= (Axis->BeSequence->DbOn << 3);

	/* bit4 �t��]�i�ړ��j���[�h */
	sig |= (Axis->UniPrms->Bprm->RvsDir << 4);
	/* bit5 ���[�h�X�C�b�`�I�� */
	sig |= (Axis->BaseControls->ModeSwData.var.ModeSWFlag << 5);

	switch( Axis->BaseCtrlOut->CtrlModeOut.b.cm )
	{
	case CTRLMODE_POS:
		break;
	case CTRLMODE_TRQ:
		/* bit7 �g���N�w�ߓ��͒� */
		sig |= ((Axis->BaseControls->CtrlCmdMngr.TrqRefo != 0) << 7);
		break;
	case CTRLMODE_SPD:
	default:
		/* bit6 ���x�w�ߓ��͒� */
		sig |= ((Axis->BaseControls->CtrlCmdMngr.SpdRefi != 0) << 6);
		break;
	}

	*pValue = sig;
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x111	�����M�����j�^ 2
****************************************************************************/
PRM_RSLT UnReadInterSignalX2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG	sig, foo;

/* <S0D0> Start */
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)	/* AnalogIF */
	NINV *pninv = &NinV;
#endif
/* <S0D0> End */

	sig = 0;
	switch( Axis->BaseCtrlOut->CtrlModeOut.b.cm )
	{
	case CTRLMODE_POS:
		/* bit0 �ʒu�w�ߓ��͒� */
		sig |= ((Axis->SeqMotSts->var.dPcmdCnt != 0) << 0);
		/* bit1 �ʒu�w�ߕ��� */
		sig |= ((Axis->SeqMotSts->var.dPcmdCnt >= 0) << 1);
		break;
	case CTRLMODE_TRQ:
		break;
	case CTRLMODE_SPD:
	default:
		break;
	}

	/* bit4 �𗬓d�������� */
	sig |= (Axis->PowerManager->PowerSts.Acon << 4);
	/* bit5 �˓���R�Z�������[�I�� */
	sig |= (Axis->PowerManager->PowerSts.ChargeOk << 5);
	/* bit6 �񐶂s���I�� */
	sig |= (HALdrv_GetRegeneControlState() << 6);
	/* bit7 �񐶈ُ팟�o�I�� */
	sig |= (Axis->PowerManager->RegeneCtrl.RegChk.RegeAlmSignal << 7);

	*pValue = sig;
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x112	�����M�����j�^ 3
****************************************************************************/
PRM_RSLT UnReadInterSignalX3(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG	sig, foo;

/* <S0D0> Start */
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)	/* AnalogIF */
	NINV *pninv = &NinV;
#endif
/* <S0D0> End */

	sig = 0;
	/* bit0 ���[�^�ʓd(���) */
	sig |= (Axis->BeSequence->BeReqSeqOut << 0);
	/* bit1 �\�� */
	/* bit2 �ߓd�� */
	sig |= (ALMCheckEachState(Axis->AlmManager, ALM_OC) << 2);
	/* bit3 ���_���ʉߊ��� */
	sig |= ((Axis->MencV->RxAlmCode & 0x0040) >> 3);

	/* bit20 �C�i�[�V�����蒆 */
//	sig |= (BoutV.f.InertiaTuning << 20);
	/* bit5 ���Ɍ��o�� */
	foo = ( (Axis->BeSequence->BeReqSeqOut != FALSE)
			&& (Axis->MencV->PhaseReady == FALSE) );
	sig |= (foo << 5);
	/* bit6 ���ɔF������ */
	sig |= (Axis->MencV->PhaseReady << 6);

	/* bit7 ���b�v���⏞��  */
//	sig |= ( ( Axis->BaseLoops->RippleCmp.RplCmpPrm.RplCmpUse ) << 23 );	/* <S14B> */
	sig |= ( ( Axis->BaseLoops->RippleCmp.RippleCompMon ) << 7 );/* <S1FD> */


	*pValue = sig;
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x113	�����M�����j�^4
****************************************************************************/
PRM_RSLT UnReadInterSignalX4(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG	sig, foo;

/* <S0D0> Start */
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)	/* AnalogIF */
	NINV *pninv = &NinV;
#endif
/* <S0D0> End */

	sig = 0;
/* <S0D0> Start */
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)	/* AnalogIF */
	/* bit2 CLR�M��  */
	sig |= ( ( pninv->NrmBits.CLR ) << 2 );
#endif
/* <S0D0> End */

	/**
	 * @@ bit24: PULS signal
	 *    bit25: SIGN signal
	 *    bit27: PSEL signal
	 */
	*pValue = sig;
	return PRM_RSLT_SUCCESS;
}
/* <S21D> End�@*/
/****************************************************************************
0x120	�������͐M�����j�^
****************************************************************************/
PRM_RSLT UnReadInternalInputSignals(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG	rv;

	/* Bit0 to Bit7 */
	rv = ((0 != (Axis->BaseCtrlOut->CmdSeqBit & SERVO_ON_BIT)) << 0);
	//rv |= ((0 != (Axis->BaseCtrlOut->CmdSeqBit & ENBPCTRL_BIT)) << 1);
	rv |= ((0 != (Axis->SvControlIf->CmdCtrl.CmdCtrlBit & ENBPCTRL_BIT)) << 1);		/* <S162>  */
	//rv |= (Axis->SeqCtrlOut->PotSigSts << 2);
	//rv |= (Axis->SeqCtrlOut->NotSigSts << 3);
	rv |= (((Axis->BaseCtrlOut->CmdSeqBit >> POSITIVE_OT_BITNO) & 0x1) << 2);
	rv |= (((Axis->BaseCtrlOut->CmdSeqBit >> NEGATIVE_OT_BITNO) & 0x1) << 3);

	//rv |= (Axis->AlmManager->AlmRst << 4);
	rv |= ((0 != (Axis->BaseCtrlOut->CmdSeqBit & ALM_RESET_BIT)) << 4);				/* <S162>  */
	rv |= (Axis->BaseControls->TrqLimitData.var.PclSignal << 5);
	rv |= (Axis->BaseControls->TrqLimitData.var.NclSignal << 6);
	rv |= ((0 != (Axis->BaseCtrlOut->CmdSeqBit & SENSOR_ON_BIT)) << 7);

	/* Bit8 to Bit13, Bit15 are reserved */

	/* Bit14 */
	rv |= (Axis->BaseControls->GselNum & 0x3) << 14;

	/* Bit16 to Bit21 */
	rv |= ((0 != (Axis->BaseCtrlOut->CmdSeqBit & POL_DETECT_BIT)) << 16);
	rv |= (Axis->IoSignals->FstpSignal.SignalStatus.v << 17);		/* <S1F5> */
	rv |= (Axis->IoSignals->DecSignal.SignalStatus.v << 18);		/* <S045> */
	rv |= (Axis->IoSignals->ExtSignal[0].SignalStatus.v << 19);		/* <S045> */

	rv |= (Axis->IoSignals->ExtSignal[1].SignalStatus.v << 20);		/* <S045> */
	rv |= (Axis->IoSignals->ExtSignal[2].SignalStatus.v << 21);		/* <S045> */

	/* Bit22 to Bit31 are reserved */

	*pValue = rv;
	return PRM_RSLT_SUCCESS;
}

/* <S21D> Start�@*/
/****************************************************************************
0x120	�������͐M�����j�^1
****************************************************************************/
PRM_RSLT UnReadInternalInputSignals1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG	rv;

	/* Bit0 to Bit7 */
	rv = ((0 != (Axis->BaseCtrlOut->CmdSeqBit & SERVO_ON_BIT)) << 0);
	//rv |= ((0 != (Axis->BaseCtrlOut->CmdSeqBit & ENBPCTRL_BIT)) << 1);
	rv |= ((0 != (Axis->SvControlIf->CmdCtrl.CmdCtrlBit & ENBPCTRL_BIT)) << 1);		/* <S162>  */
	//rv |= (Axis->SeqCtrlOut->PotSigSts << 2);
	//rv |= (Axis->SeqCtrlOut->NotSigSts << 3);
	rv |= (((Axis->BaseCtrlOut->CmdSeqBit >> POSITIVE_OT_BITNO) & 0x1) << 2);
	rv |= (((Axis->BaseCtrlOut->CmdSeqBit >> NEGATIVE_OT_BITNO) & 0x1) << 3);

	//rv |= (Axis->AlmManager->AlmRst << 4);
	rv |= ((0 != (Axis->BaseCtrlOut->CmdSeqBit & ALM_RESET_BIT)) << 4);				/* <S162>  */
	rv |= (Axis->BaseControls->TrqLimitData.var.PclSignal << 5);
	rv |= (Axis->BaseControls->TrqLimitData.var.NclSignal << 6);
	rv |= ((0 != (Axis->BaseCtrlOut->CmdSeqBit & SENSOR_ON_BIT)) << 7);


	*pValue = rv;
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x121	�������͐M�����j�^2
****************************************************************************/
PRM_RSLT UnReadInternalInputSignals2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG	rv;

	/* Bit0 to Bit5, Bit7 are reserved */

	/* Bit7 */
	rv = (Axis->BaseControls->GselNum & 0x3) << 6;

	*pValue = rv;
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x122	�������͐M�����j�^3
****************************************************************************/
PRM_RSLT UnReadInternalInputSignals3(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG	rv;


	/* Bit0 to Bit5 */
	rv = ((0 != (Axis->BaseCtrlOut->CmdSeqBit & POL_DETECT_BIT)) << 0);
	rv |= (Axis->IoSignals->FstpSignal.SignalStatus.v << 1);		/* <S1F5> */
	rv |= (Axis->IoSignals->DecSignal.SignalStatus.v << 2);		/* <S045> */
	rv |= (Axis->IoSignals->ExtSignal[0].SignalStatus.v << 3);		/* <S045> */

	rv |= (Axis->IoSignals->ExtSignal[1].SignalStatus.v << 4);		/* <S045> */
	rv |= (Axis->IoSignals->ExtSignal[2].SignalStatus.v << 5);		/* <S045> */


	*pValue = rv;
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
0x123	�������͐M�����j�^4
****************************************************************************/
UnReadInternalInputSignals4(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG	rv;
	HWSIN	SeqInputSignals;
	USHORT	faultState;

	SeqInputSignals = HALdrv_ReadInputSignals( );

	/* Bit0 : �񐶈ُ팟�o���� */
	rv = SeqInputSignals.flag.RegeneAlm << 0;		/* ALRG�M��				*/

	/* Bit1 : �R���o�[�^�o�͐M�����1 */
	rv |= SeqInputSignals.flag.Acon1 << 1;			/* ACON1�M��			*/

	/* Bit2 : �R���o�[�^�o�͐M�����2 */
	rv |= SeqInputSignals.flag.Acon2 << 2;			/* ACON2�M��			*//* <S1C9> */

	/* Bit3 : �ߓd�����o����(A.100:�ߓd��(OC1)���������t���O) */
	rv |= Axis->AlmManager->OCAlmOccurFlag1 << 3;	/* OC1�M��				*//* <S1C9> */

	/* Bit4 : �ߓd�����o����(A.100:�ߓd��(OC2)���������t���O) */
	rv |= Axis->AlmManager->OCAlmOccurFlag2 << 4;	/* OC2�M��				*//* <S1C9> */

	/* Bit5 : �t�@���A���[�����o���͐M�� */
	rv |= SeqInputSignals.flag.FanAlm << 5;			/* FANALM�M��			*/

	/* Bit6 to Bit31 are reserved */
	*pValue = rv;
	return PRM_RSLT_SUCCESS;
}
/* <S21D> End�@*/

#if 0	/* <S21D> Start */
/* <S1A0> Start */
/****************************************************************************
0x122	�������͐M�����j�^2
****************************************************************************/
UnReadInternalInputSignals2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG	rv;
	HWSIN	SeqInputSignals;
	USHORT	faultState;

	SeqInputSignals = HALdrv_ReadInputSignals( );

	/* Bit0 : �񐶈ُ팟�o���� */
	rv = SeqInputSignals.flag.RegeneAlm << 0;		/* ALRG�M��				*/

	/* Bit1 : �R���o�[�^�o�͐M�����1 */
	rv |= SeqInputSignals.flag.Acon1 << 1;			/* ACON1�M��			*/

	/* Bit2 : �R���o�[�^�o�͐M�����2 */
	rv |= SeqInputSignals.flag.Acon2 << 2;			/* ACON2�M��			*//* <S1C9> */

	/* Bit3 : �ߓd�����o����(A.100:�ߓd��(OC1)���������t���O) */
	rv |= Axis->AlmManager->OCAlmOccurFlag1 << 3;	/* OC1�M��				*//* <S1C9> */

	/* Bit4 : �ߓd�����o����(A.100:�ߓd��(OC2)���������t���O) */
	rv |= Axis->AlmManager->OCAlmOccurFlag2 << 4;	/* OC2�M��				*//* <S1C9> */

	/* Bit5 : �t�@���A���[�����o���͐M�� */
	rv |= SeqInputSignals.flag.FanAlm << 5;			/* FANALM�M��			*/

	/* Bit6 to Bit31 are reserved */
	*pValue = rv;
	return PRM_RSLT_SUCCESS;
}
/* <S1A0> End */
#endif	/* <S21D> End */

/****************************************************************************
0x130	�����o�͐M�����j�^
****************************************************************************/
PRM_RSLT UnReadInternalOutputSignals(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG	rv;
	BOOL	alm, wrn;
	VULONG	AregFbcset;			/* ASIC JL-086 SERVOIP H/W���W�X�^ �������̓t�B���^�ݒ�(0x0040) */	/* <S1FA> */
	AregFbcset = ((AXIS_HANDLE*)Axis)->SvAsicRegs->AsicHwReg->AREG_FBCSET;							/* <S1FA> */

	ALMGetStatus(Axis->AlmManager, &alm, &wrn);										/* bit00 : ALARM */
	rv = alm << 0;
	rv |= (Axis->BaseControls->PosCtrlSts.CoinSignal << 1);							/* bit01 : COIN */
	rv |= (Axis->SeqCtrlOut->VcmpSts << 2);											/* bit02 : V-CMP */
	rv |= (Axis->SeqCtrlOut->TgOnSts << 3);											/* bit03 : TGON */

	rv |= (Axis->BeSequence->ServoReady << 4);										/* bit04 : S-RDY */
	rv |= (((Axis->SvControlIf->RspCtrl.RspCtrlBit >> TRQ_CLAMP_BITNO) & 1) << 5);	/* bit05 : CLT */
	rv |= (((Axis->SvControlIf->RspCtrl.RspCtrlBit >> SPD_CLAMP_BITNO) & 1) << 6);	/* bit06 : VLT */
	rv |= ((!(Axis->SeqCtrlOut->BrkSigSts)) << 7);									/* bit07 : BK */

	rv |= wrn << 8;																	/* bit08 : WARN */
	rv |= (Axis->BaseControls->PosCtrlSts.NearSignal << 9);							/* bit09 : NEAR */
	if(Axis->UniPrms->Bprm->FencUse != FALSE)										/* bit10 : C-PULSE */
	{
		rv |= (Axis->FencV->CposOk & (Axis->FencV->MposFromCpos == 0)) << 10;
	}
	else
	{
		rv |= (Axis->MencV->CposOk & (Axis->MencV->MposFromCpos == 0)) << 10;
	}

	rv |= (((Axis->PowerManager->MainPowerChecker.var.PreSigOut) & 1) << 11);		/* bit11 : �\�h�ۑS�M���o�� */	/* <S172> */

	/* bit12 �` bit13 : �\�� */
	/* bit14 : DAW1(��TO DO) */
	/* bit15 : DAW1(��TO DO) */

	/* <S1FA> Start */
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)
	rv |= (Axis->AlmManager->AlmCodeOut.Alo1) << 16;								/* bit16 : ALO1(AP�`�̂�) */
	rv |= (Axis->AlmManager->AlmCodeOut.Alo2) << 17;								/* bit17 : ALO2(AP�`�̂�) */
	rv |= (Axis->AlmManager->AlmCodeOut.Alo3) << 18;								/* bit18 : ALO3(AP�`�̂�) */
#endif	/* CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE :AP�` */

	rv |= ((AregFbcset >> 9) & 0x0001) << 19;										/* bit19 : PAO */
	rv |= ((AregFbcset >> 10) & 0x0001) << 20;										/* bit20 : PBO */
	rv |= ((AregFbcset >> 11) & 0x0001) << 21;										/* bit21 : PCO */

#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)
	rv |= ( (SqoV.Sigs & SQO_SIGS_PSELA ) >> 19 ) << 22;							/* bit22 : PSELA(AP�`�̂�) */
#endif	/* CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE :AP�` */
	/* <S1FA> End */

	*pValue = rv;
	return PRM_RSLT_SUCCESS;
}

/* <S21D> Start */
/****************************************************************************
0x130	�����o�͐M�����j�^1
****************************************************************************/
PRM_RSLT UnReadInternalOutputSignals1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG	rv;
	BOOL	alm, wrn;
	VULONG	AregFbcset;			/* ASIC JL-086 SERVOIP H/W���W�X�^ �������̓t�B���^�ݒ�(0x0040) */	/* <S1FA> */
	AregFbcset = ((AXIS_HANDLE*)Axis)->SvAsicRegs->AsicHwReg->AREG_FBCSET;							/* <S1FA> */

	ALMGetStatus(Axis->AlmManager, &alm, &wrn);										/* bit00 : ALARM */
	rv = alm << 0;
	rv |= (Axis->BaseControls->PosCtrlSts.CoinSignal << 1);							/* bit01 : COIN */
	rv |= (Axis->SeqCtrlOut->VcmpSts << 2);											/* bit02 : V-CMP */
	rv |= (Axis->SeqCtrlOut->TgOnSts << 3);											/* bit03 : TGON */

	rv |= (Axis->BeSequence->ServoReady << 4);										/* bit04 : S-RDY */
	rv |= (((Axis->SvControlIf->RspCtrl.RspCtrlBit >> TRQ_CLAMP_BITNO) & 1) << 5);	/* bit05 : CLT */
	rv |= (((Axis->SvControlIf->RspCtrl.RspCtrlBit >> SPD_CLAMP_BITNO) & 1) << 6);	/* bit06 : VLT */
	rv |= ((!(Axis->SeqCtrlOut->BrkSigSts)) << 7);									/* bit07 : BK */


	*pValue = rv;
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x131	�����o�͐M�����j�^2
****************************************************************************/
PRM_RSLT UnReadInternalOutputSignals2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG	rv;
	BOOL	alm, wrn;
	VULONG	AregFbcset;			/* ASIC JL-086 SERVOIP H/W���W�X�^ �������̓t�B���^�ݒ�(0x0040) */	/* <S1FA> */
	AregFbcset = ((AXIS_HANDLE*)Axis)->SvAsicRegs->AsicHwReg->AREG_FBCSET;							/* <S1FA> */

	ALMGetStatus(Axis->AlmManager, &alm, &wrn);										/* bit00 : ALARM */

	rv = wrn << 0;																	/* bit00 : WARN */
	rv |= (Axis->BaseControls->PosCtrlSts.NearSignal << 1);							/* bit01 : NEAR */
	if(Axis->UniPrms->Bprm->FencUse != FALSE)										/* bit02 : C-PULSE */
	{
		rv |= (Axis->FencV->CposOk & (Axis->FencV->MposFromCpos == 0)) << 2;
	}
	else
	{
		rv |= (Axis->MencV->CposOk & (Axis->MencV->MposFromCpos == 0)) << 2;
	}

	rv |= (((Axis->PowerManager->MainPowerChecker.var.PreSigOut) & 1) << 3);		/* bit03 : �\�h�ۑS�M���o�� */	/* <S172> */

	/* bit12 �` bit13 : �\�� */
	/* bit14 : DAW1(��TO DO) */
	/* bit15 : DAW1(��TO DO) */


	*pValue = rv;
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x132	�����o�͐M�����j�^3
****************************************************************************/
PRM_RSLT UnReadInternalOutputSignals3(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG	rv;
	BOOL	alm, wrn;
	VULONG	AregFbcset;			/* ASIC JL-086 SERVOIP H/W���W�X�^ �������̓t�B���^�ݒ�(0x0040) */	/* <S1FA> */
	AregFbcset = ((AXIS_HANDLE*)Axis)->SvAsicRegs->AsicHwReg->AREG_FBCSET;							/* <S1FA> */

	rv = 0;
	/* <S1FA> Start */
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)
	rv |= (Axis->AlmManager->AlmCodeOut.Alo1) << 0;								/* bit0 : ALO1(AP�`�̂�) */
	rv |= (Axis->AlmManager->AlmCodeOut.Alo2) << 1;								/* bit1 : ALO2(AP�`�̂�) */
	rv |= (Axis->AlmManager->AlmCodeOut.Alo3) << 2;								/* bit2 : ALO3(AP�`�̂�) */
#endif	/* CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE :AP�` */

	rv |= ((AregFbcset >> 9) & 0x0001) << 3;										/* bit3 : PAO */
	rv |= ((AregFbcset >> 10) & 0x0001) << 4;										/* bit4 : PBO */
	rv |= ((AregFbcset >> 11) & 0x0001) << 5;										/* bit5 : PCO */

#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)
	rv |= ( (SqoV.Sigs & SQO_SIGS_PSELA ) >> 19 ) << 6;							/* bit6 : PSELA(AP�`�̂�) */
#endif	/* CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE :AP�` */
	/* <S1FA> End */

	*pValue = rv;
	return PRM_RSLT_SUCCESS;
}
/* <S21D> End */

/****************************************************************************
0x140	���H����d�� V
****************************************************************************/
PRM_RSLT UnReadDCVoltage(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
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

/****************************************************************************
0x141	�d�����o�l	IqRate	%
****************************************************************************/
PRM_RSLT UnReadIqRate(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
//<S0DA>	*pValue = 0;
	*pValue = 
		MlibMulgain( Axis->UnMonitor->UnCrntDet, (Axis->UniPrms->Bprm)->Kmoncrnt ); /* <S0DA> */
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x142	�ݐϕ��ח�	MotLoad2ms	%�i����2ms�j
****************************************************************************/
PRM_RSLT UnReadMotorLoad2ms(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if (FLOAT_USE==TRUE)
	*pValue = Axis->UnMonitor->UnCrntRef * Axis->UniPrms->Bprm->Kmoncrnt;
#else
	*pValue = MlibMulgain( Axis->UnMonitor->UnCrntRef, Axis->UniPrms->Bprm->Kmoncrnt );
#endif /* FLOAT_USE */
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x143	�񐶕��ח�	RegerRate2ms	%�i����2ms�j
****************************************************************************/
PRM_RSLT UnReadRegeneLoad2ms(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = Axis->UnMonitor->UnRgPower;
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x144	�c�a��R����d��	DbrxPower2ms	%�i����2ms�j
****************************************************************************/
PRM_RSLT UnReadDbPower2ms(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = Axis->UnMonitor->UnDbPower;
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x145	�ő�ݐϕ��ח�	MotLoad10s	%�i����10s�j
****************************************************************************/
PRM_RSLT UnReadMaxMotorLoad10s(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = Axis->UnMonitor->UnIsqrSumMax;
	return PRM_RSLT_SUCCESS;
}

/* <S056>	Start */
/****************************************************************************
0xE148	T-REF���j�^
****************************************************************************/
PRM_RSLT UnReadTrefAdMon(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)
	ANLGV *panlg = &AnlgV;
	LONG Torque;

	Torque = panlg->TrqRefAd;
	*pValue = MlibMulgain( Torque, (Axis->UniPrms->Bprm)->Kmontrq ); /* [2^24/MaxTrq �� [ % ]	*/
#endif
	return PRM_RSLT_SUCCESS;

}

/****************************************************************************
0xE149	V-REF���j�^
****************************************************************************/
PRM_RSLT UnReadVrefAdMon(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)
	ANLGV *panlg = &AnlgV;
	LONG Speed;

	Speed = panlg->SpdRefAd;
	*pValue = MlibMulgainSx48( Speed, (Axis->UniPrms->Bprm)->Kmonspd );	/* [2^24/MaxSpd �� [ min-1 , mm/s ] */
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0xE14A	���͎w�߃p���X���g��
****************************************************************************/
PRM_RSLT UnReadDPulseHwCntMon(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)
	ANLGV *panlg = &AnlgV;

	/* �w�߃p���X���͔{���@�\�ǉ��ɂăI�[�o�[�t���[�������邽�߂̑΍� */
	//*pValue = MlibMulgain30( Axis->UnMonitor->UnPcmd64ms, 0x137d0000 );		   /* 1000��64=15.625 ==> 0x137d0000   */
	*pValue = MlibMulgain30( Axis->UnMonitor->UnPcmd64Sum, 0x137d0000 );	/* <S1B6> */
#endif
	return PRM_RSLT_SUCCESS;
}

/* <S056>	End */

/****************************************************************************
0x14C	d���d���w�߃��j�^	IdrefRate	%
****************************************************************************/
PRM_RSLT UnReadIdRefRate(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if (FLOAT_USE==TRUE)
	*pValue = Axis->CtrlLoopOut->IdRefMon * 
	                       Axis->UniPrms->Bprm->Kmoncrnt;
#else
	*pValue = MlibMulgain( Axis->CtrlLoopOut->IdRefMon,
	                       Axis->UniPrms->Bprm->Kmoncrnt );
#endif /* FLOAT_USE */
	return PRM_RSLT_SUCCESS;
}

/* <S056>	Start */
/****************************************************************************
0xE14D	MEMOBUS�G���[���i�G���[�����A�h���X�j���G���[���Ȃ�����0xFFFF
****************************************************************************/
PRM_RSLT UnReadMemobusErrorInfo(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

	LONG	ErrRegAdd;
	MSGIF	*Hmsg_Com0;
	MSGIF	*Hmsg_Com1;

	/* Get Serial Message I/F for SigmaMsg/LcdOp/Memobus	*/
	Hmsg_Com0 = &SrlMsg[COM0_UART];		/* Serial CH:0 */
	Hmsg_Com1 = &SrlMsg[COM1_UART];		/* Serial CH:1 */

	if( (Hmsg_Com0->CmdBuf[1] != 0x00) || (Hmsg_Com1->CmdBuf[1] != 0x00) )
	{
		if(Hmsg_Com0->CmdBuf[1] != 0x00)	/* Serial CH:0 */
		{
			//ErrRegAdd = (LONG)(Hmsg_Com0->ErrRno);
			ErrRegAdd = (LONG)(Hmsg_Com0->LstErrRno);
		}
		else								/* Serial CH:1 */
		{
			//ErrRegAdd = (LONG)(Hmsg_Com1->ErrRno);
			ErrRegAdd = (LONG)(Hmsg_Com1->LstErrRno);
		}
	}

	if(ErrRegAdd == 0)
	{
		*pValue = 0xffff;
	}
	else
	{
		*pValue = ErrRegAdd;
	}
	return PRM_RSLT_SUCCESS;
}
/* <S056>	End */

/* <S154>	Start  */
/*****************************************************************************
0x170	 ���ϓ������x		[0.01��]
*****************************************************************************/
PRM_RSLT UnReadAvgIntTmrp(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if ( CSW_SVCF_LIFEPRED_USE == TRUE )
	*pValue = Axis->hLifeTime->AvgTmprHdl.var.AvgIntTmpr;
#else
	*pValue = Axis->OverHeat->UnMon.UnAverageTemp;		/* <S183>�����f�f�������ɐ����ƃp���[��̕��ϒl��\������ */
#endif
	return PRM_RSLT_SUCCESS;
}
/* <S154>	End */
/* <S154>	Start  */
/****************************************************************************
0x1D5 : �����M�����j�^ [�|]
****************************************************************************/
PRM_RSLT UnReadAlmLtMonInterSig0(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

	*pValue = (Axis->hAlmLatchHndl->LatchMon.InterSignal & 0x000000FF);

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x1D6 : �����M�����j�^ [��]
****************************************************************************/
PRM_RSLT UnReadAlmLtMonInterSig1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

	*pValue = ( (Axis->hAlmLatchHndl->LatchMon.InterSignal & 0x0000FF00) >> 8 );

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x1D7 : �����M�����j�^ [�O]
****************************************************************************/
PRM_RSLT UnReadAlmLtMonInterSig2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

	*pValue = ( (Axis->hAlmLatchHndl->LatchMon.InterSignal & 0x00FF0000) >> 16 );

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x1D8 : �������͐M�����j�^ [�|]
****************************************************************************/
PRM_RSLT UnReadAlmLtMonInSig0(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

	*pValue = (Axis->hAlmLatchHndl->LatchMon.InSignal & 0x000000FF);

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x1D9 : �������͐M�����j�^ [��]
****************************************************************************/
PRM_RSLT UnReadAlmLtMonInSig1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

	*pValue = ( (Axis->hAlmLatchHndl->LatchMon.InSignal & 0x0000FF00) >> 8 ) ;

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x1DA : �������͐M�����j�^ [�O]
****************************************************************************/
PRM_RSLT UnReadAlmLtMonInSig2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

	*pValue = ( (Axis->hAlmLatchHndl->LatchMon.InSignal & 0x00FF0000) >> 16 ) ;

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x1DB : �������͐M�����j�^ [�l]
****************************************************************************/
PRM_RSLT UnReadAlmLtMonInSig3(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

	*pValue = ( (Axis->hAlmLatchHndl->LatchMon.InSignal & 0xFF000000) >> 24 ) ;

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x1DC : �����o�͐M�����j�^ [�|]
****************************************************************************/
PRM_RSLT UnReadAlmLtMonOutSig0(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

	*pValue = (Axis->hAlmLatchHndl->LatchMon.OutSignal & 0x000000FF) ;

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x1DD : �����o�͐M�����j�^ [��]
****************************************************************************/
PRM_RSLT UnReadAlmLtMonOutSig1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

	*pValue = ( (Axis->hAlmLatchHndl->LatchMon.OutSignal & 0x0000FF00) >> 8 ) ;

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x1DE : �����o�͐M�����j�^ [�O]
****************************************************************************/
PRM_RSLT UnReadAlmLtMonOutSig2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

	*pValue = ( (Axis->hAlmLatchHndl->LatchMon.OutSignal & 0x00FF0000) >> 16 ) ;

	return PRM_RSLT_SUCCESS;
}
/* <S154>	End  */
/****************************************************************************
0x200	�G���R�[�_���C�����x���j�^	MencTemp	��
****************************************************************************/
PRM_RSLT UnReadMotroEncoderTemperature(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = Axis->MencV->Temperature;
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x210	���[�U�p�����[�^CRC�v�Z�l
****************************************************************************/

/*--------------------------------------------------------------------------*/
/*		CRC16�v�Z���[�J���֐�												*/
/*--------------------------------------------------------------------------*/
static void cal_crc16(UCHAR *Dbuf, LONG Length, ULONG *pa)
{
LONG	i;
ULONG	a = *pa;
ULONG	b = 0xA001;	/* ���������� */

		for( i=0; i<Length; i++ )
		{
			a = a ^ (ULONG)Dbuf[i];
			a = (a & 0x01)? ((a>>1)^b) : (a>>1);		/* 1	*/
			a = (a & 0x01)? ((a>>1)^b) : (a>>1);		/* 2	*/
			a = (a & 0x01)? ((a>>1)^b) : (a>>1);		/* 3	*/
			a = (a & 0x01)? ((a>>1)^b) : (a>>1);		/* 4	*/
			a = (a & 0x01)? ((a>>1)^b) : (a>>1);		/* 5	*/
			a = (a & 0x01)? ((a>>1)^b) : (a>>1);		/* 6	*/
			a = (a & 0x01)? ((a>>1)^b) : (a>>1);		/* 7	*/
			a = (a & 0x01)? ((a>>1)^b) : (a>>1);		/* 8	*/
		}
		*pa = a;
		return;
}

/*--------------------------------------------------------------------------*/
/*		0x210	���[�U�p�����[�^CRC�v�Z�@���W�X�^�R�[���o�b�N�֐�			*/
/*--------------------------------------------------------------------------*/
PRM_RSLT UnReadUsrPrmCrcValue(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	CPRMTBL		*pTbl;
	void		*pDstData;
	ULONG		i,ArraySize;
	ULONG		crData=0xFFFF;	/* CRC-16 �����l */
	UINT		accLv,usSize;

	for(pTbl=Axis->RegManager->hPnReg->PrmListTable; pTbl->Number != 0xFFFF; pTbl++)
	{
		accLv = pTbl->Attribute->AccessLevel;
		if(accLv == 1)
		{/* �A�N�Z�X���x��1�̃p�����[�^�̂݉��Z�ΏۂƂ��� */
			if(NULL != pTbl->Attribute->GetRamPtrCallback)
			{
				usSize = pTbl->Attribute->DataLength;
				ArraySize = (pTbl->Attribute->NumberOfArray != 0) ? 
							pTbl->Attribute->NumberOfArray : 1;
				for(i = 0; i < ArraySize; i++)
				{	/* Get Parameter ram address */
					pDstData = pTbl->Attribute->GetRamPtrCallback(i, Axis->RegManager->hPnReg->GetRamPtrParam);
					cal_crc16( (UCHAR*)pDstData, usSize, &crData);	/* CRC-16�v�Z */
				}	
			}
		}
	}
	*pValue = (USHORT)crData;
	
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x300	2ms�������CPU���ח�	CurExeTimeS	ns
****************************************************************************/
PRM_RSLT UnReadCPULoad(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = (LONG)KlibClockToTime(Axis->TaskProcessTime->crtTimePer2ms);
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x302	2ms�������CPU�ő啉�ח�	MaxExeTimeS	ns
****************************************************************************/
PRM_RSLT UnReadMaxCPULoad(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	if(Cmd == PRM_ACCCMD_READ)
	{/* read access */
		*pValue = (LONG)KlibClockToTime(Axis->TaskProcessTime->maxTimePer2ms);
	}
	else
	{/* reset monitor value */
		Axis->TaskProcessTime->maxTimePer2ms = 0;
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x304	ScanB���ݏ�������	CurExeTimeB	ns
****************************************************************************/
PRM_RSLT UnReadScanBProcessTime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = (LONG)KlibClockToTime(Axis->TaskProcessTime->SB_crtTime);
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x306	ScanB�ő又������	MaxExeTimeB	ns
****************************************************************************/
PRM_RSLT UnReadScanBMaxProcessTime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	if(Cmd == PRM_ACCCMD_READ)
	{/* read access */
		*pValue = (LONG)KlibClockToTime(Axis->TaskProcessTime->SB_maxTime);
	}
	else
	{/* reset monitor value */
		Axis->TaskProcessTime->SB_maxTime = 0;
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x308	ScanC���ݏ�������	CurExeTimeC	ns
****************************************************************************/
PRM_RSLT UnReadScanCProcessTime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = (LONG)KlibClockToTime(Axis->TaskProcessTime->SC_crtTime);
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x30A	ScanC�ő又������	MaxExeTimeC	ns
****************************************************************************/
PRM_RSLT UnReadScanCMaxProcessTime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	if(Cmd == PRM_ACCCMD_READ)
	{/* read access */
		*pValue = (LONG)KlibClockToTime(Axis->TaskProcessTime->SC_maxTime);
	}
	else
	{/* reset monitor value */
		Axis->TaskProcessTime->SC_maxTime = 0;
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x310	��IntAD���ݏ�������	CurExeTimeMI	ns
****************************************************************************/
PRM_RSLT UnReadMIntADProcessTime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if 0 /* 2012.08.31 Y.Oka ���ʏ������ԑ��茻�󖢑Ή��� */
	*pValue = (LONG)SHal_ClockToTime(Axis->SvAsicRegs->AsicMicroReg->MREG_INTADCTIME);
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x312	��IntAD�ő又������	MaxExeTimeMI	ns
****************************************************************************/
PRM_RSLT UnReadMIntADMaxProcessTime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if 0 /* 2012.08.31 Y.Oka ���ʏ������ԑ��茻�󖢑Ή��� */
	if(Cmd == PRM_ACCCMD_READ)
	{/* read access */
		*pValue = (LONG)SHal_ClockToTime(Axis->SvAsicRegs->AsicMicroReg->MREG_INTADMTIME);
	}
	else
	{/* reset monitor value */
		Axis->SvAsicRegs->AsicMicroReg->MREG_INTADMTIME = 0;
	}
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x314	��ScanA���ݏ�������	CurExeTimeMA	ns
****************************************************************************/
PRM_RSLT UnReadMScanAProcessTime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if 0 /* 2012.08.31 Y.Oka ���ʏ������ԑ��茻�󖢑Ή��� */
	*pValue = (LONG)SHal_ClockToTime(Axis->SvAsicRegs->AsicMicroReg->MREG_SCANACTIME);
#endif
	*pValue = (LONG)KlibClockToTime(Axis->TaskProcessTime->SA_crtTime);		/* �}�C�N���p�ɂȂ��Ă��邪�ЂƂ܂������ɒǉ�	<S043> */
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x316	��ScanA�ő又������	MaxExeTimeMA	ns
****************************************************************************/
PRM_RSLT UnReadMScanAMaxProcessTime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if 0 /* 2012.08.31 Y.Oka ���ʏ������ԑ��茻�󖢑Ή��� */
	if(Cmd == PRM_ACCCMD_READ)
	{/* read access */
		*pValue = (LONG)SHal_ClockToTime(Axis->SvAsicRegs->AsicMicroReg->MREG_SCANAMTIME);
	}
	else
	{/* reset monitor value */
		Axis->SvAsicRegs->AsicMicroReg->MREG_SCANAMTIME = 0;
	}
#else		/* �}�C�N���p�ɂȂ��Ă��邪�ЂƂ܂������ɒǉ�	<S043> */
	if(Cmd == PRM_ACCCMD_READ)
	{/* read access */
		*pValue = (LONG)KlibClockToTime(Axis->TaskProcessTime->SA_maxTime);
	}
	else
	{/* reset monitor value */
		Axis->TaskProcessTime->SA_maxTime = 0;
	}
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x318	��Round���ݏ�������	CurExeTimeMR	ns
****************************************************************************/
PRM_RSLT UnReadMRoundProcessTime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if 0 /* 2012.08.31 Y.Oka ���ʏ������ԑ��茻�󖢑Ή��� */
	*pValue = (LONG)SHal_ClockToTime(Axis->SvAsicRegs->AsicMicroReg->MREG_ROUNDCTIME);
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x31A	��Round�ő又������	MaxExeTimeMR	ns
****************************************************************************/
PRM_RSLT UnReadMRoundMaxProcessTime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if 0 /* 2012.08.31 Y.Oka ���ʏ������ԑ��茻�󖢑Ή��� */
	if(Cmd == PRM_ACCCMD_READ)
	{/* read access */
		*pValue = (LONG)SHal_ClockToTime(Axis->SvAsicRegs->AsicMicroReg->MREG_ROUNDMTIME);
	}
	else
	{/* reset monitor value */
		Axis->SvAsicRegs->AsicMicroReg->MREG_ROUNDMTIME = 0;
	}
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x500	�A���[�������f�[�^	AlmTrace
****************************************************************************/
PRM_RSLT UnReadAlarmHistory(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	ULONG tmStamp;
	*pValue = (LONG)ALMGetAlarmHistory(Axis->AlmManager, ArrayIdx, &tmStamp);
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x50B	���x�w�ߒ����p�f�[�^	<S00C>
****************************************************************************/
PRM_RSLT uncal_VrefAdjData(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if SVFSEL_ADC_USE == 1

LONG	x;
	#if ( SVFSEL_VREFAD_24BIT != 0 )
	LONG	y = 3500;
	#else
	LONG	y = 16;
	#endif

	x = Axis->UniPrms->Prm->zadjvl;				/* Zadjv : Max.52500000 */
	*pValue = ( x / y );						/* 52500000 / 3500 = 15000 */

#if 0
	CADC *pAdc = Axis->pAdc;

	x = pAdc->P.ZadjVelo;			/* Zadjv : Max.52500000						*/
	*pValue = ( x / y );			/* 52500000 / 3500 = 15000					*/
#endif

#else /* SVFSEL_ADC_USE == 1 */
	*pValue = 0;
#endif

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x50C	�g���N�w�ߒ����p�f�[�^		<S00C>
****************************************************************************/
PRM_RSLT uncal_TrefAdjData(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if SVFSEL_ADC_USE == 1
//LONG	x;
//	CADC *pAdc = &LAdc[Axis->AxisNo];

//	x = pAdc->P.ZadjTrqP;
	//*pValue = x;
//	*pValue = Axis->pAdc->P.ZadjTrqP;

	*pValue = (Axis->UniPrms->Prm->zadjtp_n.b.l & 0xFF);

#else
	*pValue = 0;
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		0x050D : �A�i���O���j�^�iCH1�j�뒲�p�f�[�^													*/
/****************************************************************************************************/
PRM_RSLT uncal_AmonZadjCh1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = (Axis->UniPrms->Cprm->mnaz0_1.b.l & 0xFF);
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		0x050E : �A�i���O���j�^�iCH2�j�뒲�p�f�[�^													*/
/****************************************************************************************************/
PRM_RSLT uncal_AmonZadjCh2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = (Axis->UniPrms->Cprm->mnaz0_1.b.h & 0xFF);
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		0x050F : �A�i���O���j�^�iCH1�j�Q�C�����f�[�^												*/
/****************************************************************************************************/
PRM_RSLT uncal_AmonGadjCh1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = (Axis->UniPrms->Cprm->mnag0_1.b.l & 0xFF);
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		0x0510 : �A�i���O���j�^�iCH2�j�Q�C�����f�[�^												*/
/****************************************************************************************************/
PRM_RSLT uncal_AmonGadjCh2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = (Axis->UniPrms->Cprm->mnag0_1.b.h & 0xFF);
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		0x0511 : �d�����o�iU���j�뒲�p�f�[�^														*/
/****************************************************************************************************/
PRM_RSLT uncal_CurZadjUphase(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = (SHORT)Axis->UniPrms->Prm->zadjiu;
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		0x0512 : �d�����o�iV���j�뒲�p�f�[�^														*/
/****************************************************************************************************/
PRM_RSLT uncal_CurZadjVphase(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = (SHORT)Axis->UniPrms->Prm->zadjiv;
	return PRM_RSLT_SUCCESS;
}


/* <S056>	Start */
/****************************************************************************
0x513	���[�^�^�C�v
****************************************************************************/
PRM_RSLT UnReadMotorID(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = (LONG)(Axis->UniPrms->Prm)->MencP.typm_e.b.l;
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x514	���[�^�e��
****************************************************************************/
PRM_RSLT UnReadMotorWattage(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG x;
	x = (LONG)(Axis->UniPrms->Bprm)->MotW;
	x = MlibScalKskxkx( x, 1, 1, NULL, -30 );

	*pValue = (LONG)((ArrayIdx == 0) ? x : ((ULONG)x >> 16));

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x515	�G���R�[�_�^�C�v
****************************************************************************/
PRM_RSLT UnReadEncoderType(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG x;
	x = (LONG)(Axis->UniPrms->Prm)->MencP.typm_e.b.h;
	*pValue = ((x >> 4) & 0x0F) - 1;		/* 2,1 --> 1,0					*/
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x516	Y�d�l
****************************************************************************/
PRM_RSLT UnReadYspecVer(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG YspecVer;

	YspecVer = (LONG)SERVO_SPECIAL_SPEC_VER;

	*pValue = YspecVer;
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x517	�T�[�{�p�b�N�\�t�g�o�[�W����
****************************************************************************/
PRM_RSLT UnReadFirmVerBase(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG ServoFirmVer;

//	ServoFirmVer = (LONG)SERVO_FIRM_VER>>16;				/* <S0D1> */
	ServoFirmVer = (LONG)((USHORT)(SERVO_FIRM_VER>>16));	/* <S0D1> */

	*pValue = ServoFirmVer;
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x518	�G���R�[�_�\�t�g�o�[�W����
****************************************************************************/
PRM_RSLT UnReadEncoderVer(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = (LONG)(Axis->UniPrms->Prm)->MencP.encver;
	return PRM_RSLT_SUCCESS;
}
/* <S056>	End */

/* <S049>	Start */
/****************************************************************************
0x51B	�T�[�{�X�e�[�^�X���j�^
****************************************************************************/
PRM_RSLT UnReadSvStatus(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = (LONG)(Axis->UnMonitor->SvStatus);
	return PRM_RSLT_SUCCESS;
}
/* <S049>	End */

/****************************************************************************
0x51C	���䃂�[�h���j�^	CtrlMode
****************************************************************************/
PRM_RSLT UnReadServoControlMode(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG   CtrlModex;
	CtrlModex = (LONG)Axis->BaseCtrlOut->CtrlModeOut.b.cm;
	switch(Axis->BaseCtrlOut->CtrlModeOut.b.cm)
	{
	case CTRLMODE_JOG:  /* �i�n�f�^�]��-�h*/
	case CTRLMODE_ZSRCH:/* ���_�T�[�` */
		*pValue = CTRLMODE_SPD;
		break;

	case CTRLMODE_POS:  /* �ʒu���䃂�[�h */
	case CTRLMODE_SPD:  /* ���x���䃂�[�h */
	case CTRLMODE_TRQ:  /* �g���N���䃂�[�h */
	default:            /* �{�P�[�X�͖��� */
		*pValue = CtrlModex;
		break;
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x52A	�A���[�������^�C���X�^���v	AlmTmStamp
****************************************************************************/
PRM_RSLT UnReadAlarmTimeStamp(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	ALMGetAlarmHistory(Axis->AlmManager, ArrayIdx, (ULONG*)pValue);
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x700	�T���v�����O���Ԃ̒P��	SamplingTime
****************************************************************************/
PRM_RSLT UnReadSamplingTimeUnit(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = Axis->UniPrms->Bprm->SvCycleUs;
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x701	�f�[�^��16384�̎��̃g���N MaxTorque
****************************************************************************/
PRM_RSLT UnReadMaxTorqueConvertValue(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	/* �ő�g���N[%] */
	*pValue = (LONG)(Axis->UniPrms->Bprm->PerMaxTrq);
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x702	�f�[�^��16384�̎��̑��x MaxSpeed
****************************************************************************/
PRM_RSLT UnReadMaxSpeedConvertValue(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if (FLOAT_USE==TRUE)
	*pValue = 0x01000000 * (LONG)Axis->UniPrms->Bprm->Kmonspd ;
#else
	*pValue = MlibMulgainSx48( 0x01000000, (LONG)Axis->UniPrms->Bprm->Kmonspd );
#endif /* FLOAT_USE */
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x704	�f�[�^��16384�̎��̑��x�̎w��  MaxSpeed
****************************************************************************/
PRM_RSLT UnReadMaxSpeedExpConvertValue(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = 0;
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x705	����������	NewServoOpmode
****************************************************************************/
PRM_RSLT UnReadInternalServoMode(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG Opmode = 0;
	MOTLESSTST		*MotorLessTest;	/* ���[�^���X�e�X�g���[�hIF��`		*//* <S1B7> */

	MotorLessTest = Axis->MotorLessTest;									/* <S1B7> */

	/* Bit0�`7  :�\�� */
	/* Bit8�`11 : */
	if((Axis->UniPrms->Bprm)->FencUse != FALSE)
	{ /* �t���N�N���[�Y�h */
		Opmode |= 0x00000100;
	}

	if( Axis->MencV->LowSpdMotor )
	{ /* DD���|�^*/
		Opmode |= 0x00000800;
	}
	else
	{
		if( Axis->MencV->AxisMotType == MOTTYPE_ROTARY )
		{/* ��] */
			Opmode |= 0x00000200;
		}
		if( Axis->MencV->AxisMotType == MOTTYPE_LINEAR )
		{/* ���j�A */
			Opmode |= 0x00000400;
		}
	}

	/* Bit12�`15 */
	if( Axis->MencV->ScaleWriteEnable )
	{/* �X�P�[�������݉� */
		Opmode |= 0x00002000;
	}

	/* ���[�^���X�e�X�g���[�h�̐ݒ� *//* <S1B7> */
	if( MotorLessTest->MotorLessTestMode == TRUE )
	{/* ���[�^���X�e�X�g���[�h */
		Opmode |= 0x00008000;
	}

	*pValue = Opmode;
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
0x710	RSW/DSW���͏��
****************************************************************************/
PRM_RSLT UnReadHSWInputsStatus(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	ULONG rsw,dsw;
	dsw = (ULONG)HALdrv_GetDipSwitchSetting();
	rsw = (ULONG)HALdrv_GetStationAddress();
	*pValue = (LONG)(rsw | (dsw << 8));
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x712	FB����
****************************************************************************/
PRM_RSLT UnReadFBboardID(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if 0 /* 0x712 is only INGRAM */
	*pValue = (LONG)HALdrv_ReadBoardID(HW_FBB_MODULE);
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x713	MB����
****************************************************************************/
PRM_RSLT UnReadMBboardID(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if 0 /* 0x713 is only INGRAM */
	*pValue = (LONG)HALdrv_ReadBoardID(HW_MB_MODULE);
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x714	�R���o�[�^���דd��(AD�l)
****************************************************************************/
PRM_RSLT UnReadConverterCurrentAD(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if 0 /* 0x714 is only INGRAM */
	*pValue = HALdrv_GetConvCurFromAd();
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x716	�R���o�[�^�V���A�����̓f�[�^
****************************************************************************/
PRM_RSLT UnReadConverterSerialInputs(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if 0 /* 2012.09.07 Y.Oka for Mercury Base (only INGRAM) */
	CONV_INPUT CnvInputs;
	CnvInputs = HALdrv_ReadConverterSignals();
	*pValue = CnvInputs.dw;
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x718	���j�b�g�������x
****************************************************************************/
PRM_RSLT UnReadMbTemperature(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = 0;//OhmGetTemperature( &Axis->OhInfo[MBOH] );
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x719	�R���o�[�^���x
****************************************************************************/
PRM_RSLT UnReadCnvTemperature(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue =0;// OhmGetTemperature( &Axis->OhInfo[COAOH] );
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x71A	�������x
****************************************************************************/
PRM_RSLT UnReadIfTemperature(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue =0;// OhmGetTemperature( &Axis->OhInfo[COAOH] );/* �b�菈��	*/
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x720	(R)�u�[�g�\�t�g�E�F�A�o�[�W����
****************************************************************************/
PRM_RSLT UnReadBootsoftVersion(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = (LONG)HALdrv_GetBootVersion( ArrayIdx, R_BOOTSOFT_VERSION );
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x724	(R)�u�[�g�X�g���b�v�o�[�W����
****************************************************************************/
PRM_RSLT UnReadBootstrapVersion(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = (LONG)(HALdrv_GetBootVersion( ArrayIdx, R_BOOTSTRAP_VERSION ));
	return PRM_RSLT_SUCCESS;
}
/* <S154> Start */
/****************************************************************************
0x800 : ���[�^��]/�ړ����x [min-1, mm/s]
****************************************************************************/
PRM_RSLT UnReadAlmLtSpdFbk(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

	*pValue = Axis->hAlmLatchHndl->LatchMon.MotSpd;

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x801 : ���x�w�� [min-1, mm/s]
****************************************************************************/
PRM_RSLT UnReadAlmLtSpdRef(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

	*pValue = Axis->hAlmLatchHndl->LatchMon.SpdRef;

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x802 : �����g���N/���͎w�� [%]
****************************************************************************/
PRM_RSLT UnReadAlmLtTrqRef(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

	*pValue = Axis->hAlmLatchHndl->LatchMon.TrqRef;

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x803 : ���͎w�߃p���X���x [min-1, mm/s]
****************************************************************************/
PRM_RSLT UnReadAlmLtPosRefSpd(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

	*pValue = Axis->hAlmLatchHndl->LatchMon.PcmdSpd;

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x804 : �΍��J�E���^(�ʒu�΍���) [pulse]
****************************************************************************/
PRM_RSLT UnReadAlmLtPosErrCntr(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

	*pValue = Axis->hAlmLatchHndl->LatchMon.PosErr;

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x810 : �����M�����j�^
****************************************************************************/
PRM_RSLT UnReadAlmLtMonInterSig(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

	*pValue = Axis->hAlmLatchHndl->LatchMon.InterSignal ;

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x814 : �������͐M�����j�^
****************************************************************************/
PRM_RSLT UnReadAlmLtMonInSig(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

	*pValue = Axis->hAlmLatchHndl->LatchMon.InSignal ;

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x818 : �����o�͐M�����j�^
****************************************************************************/
PRM_RSLT UnReadAlmLtMonOutSig(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

	*pValue = Axis->hAlmLatchHndl->LatchMon.OutSignal ;

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x820 : ���H����d�� [V]
****************************************************************************/
PRM_RSLT UnReadAlmLtDcVoltx(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

	*pValue = Axis->hAlmLatchHndl->LatchMon.DcVoltage ;

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x821 : �d�����o�l [%]
****************************************************************************/
PRM_RSLT UnReadAlmLtIqRate(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

	*pValue = Axis->hAlmLatchHndl->LatchMon.IFBMon ;

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x822 : �ݐϕ��ח� [%(����2ms)]
****************************************************************************/
PRM_RSLT UnReadAlmLtMotLoad2ms(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

	*pValue = Axis->hAlmLatchHndl->LatchMon.MotLoad ;

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x823 : �񐶕��ח� [%(����2ms)]
****************************************************************************/
PRM_RSLT UnReadAlmLtRegerRate2ms(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

	*pValue = Axis->hAlmLatchHndl->LatchMon.RgPower ;

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x824 : �c�a��R����d�� [%(����2ms)]
****************************************************************************/
PRM_RSLT UnReadAlmLtDbrxPower2ms(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

	*pValue = Axis->hAlmLatchHndl->LatchMon.DbPower ;

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x825 : �ő�ݐϕ��ח� [%]
****************************************************************************/
PRM_RSLT UnReadAlmLtMotLoad10s(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

	*pValue = Axis->hAlmLatchHndl->LatchMon.MaxMotLoad10s ;

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x826 : �������[�����g��/�d�ʔ� [%]
****************************************************************************/
PRM_RSLT UnReadAlmLtJLRatio(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

	*pValue = Axis->hAlmLatchHndl->LatchMon.JLRatio ;

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x830 : �V���A���G���R�[�_�ʐM�ُ�� [�|]
****************************************************************************/
PRM_RSLT UnReadAlmLtSencComErrCnt(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

	*pValue = Axis->hAlmLatchHndl->LatchMon.EncComErrCnt ;

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x831 : MECHATROLINK�ʐM�ُ�� [�|]
****************************************************************************/
PRM_RSLT UnReadAlmLtM2ComErrCnt(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

	*pValue = Axis->hAlmLatchHndl->LatchMon.M2ComErrCnt ;

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x832 : �Z�[�t�e�B���o�͐M�����j�^ [�|]
****************************************************************************/
PRM_RSLT UnReadAlmLtMonSafetyIOSig(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

	*pValue = Axis->hAlmLatchHndl->LatchMon.SafetyIOSignal ;

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x833 : �A���[���ԍ� [�|]
****************************************************************************/
PRM_RSLT UnReadAlmLtAlmCode(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

	*pValue = Axis->hAlmLatchHndl->LatchMon.AlmCode ;

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0x834 : �^�C���X�^���v [�|]
****************************************************************************/
PRM_RSLT UnReadAlmLtTmStamp(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

	*pValue = Axis->hAlmLatchHndl->LatchMon.TmStamp ;

	return PRM_RSLT_SUCCESS;
}
/* <S154> End */
/* <S056>	Start */
/****************************************************************************
0xC00	�t�B�[�h�o�b�N�ʒu(APOS)
****************************************************************************/
PRM_RSLT UnReadApos(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

#if ( (CSW_NETIF_TYPE == NETIF_M2_TYPE) || (CSW_NETIF_TYPE == NETIF_M3_TYPE) )	/* M-2IF or M-3IF */
	MT_HNDL		*mt_hdl;

	mt_hdl  = &Axis->NetAxisIf->mt_hdl;

	*pValue = mt_hdl->psnHdl.Apos;

#endif

	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
0xC02	���ݎw�߈ʒu(CPOS)
****************************************************************************/
PRM_RSLT UnReadCpos(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

#if ( (CSW_NETIF_TYPE == NETIF_M2_TYPE) || (CSW_NETIF_TYPE == NETIF_M3_TYPE) )	/* M-2IF or M-3IF */
	MT_HNDL		*mt_hdl;

	mt_hdl  = &Axis->NetAxisIf->mt_hdl;

	*pValue = mt_hdl->psnHdl.Pos;
#endif


	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0xC04	�ʒu�΍�(PERR)
****************************************************************************/
PRM_RSLT UnReadPerr(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if ( (CSW_NETIF_TYPE == NETIF_M2_TYPE) || (CSW_NETIF_TYPE == NETIF_M3_TYPE) )	/* M-2IF or M-3IF */
	MT_HNDL		*mt_hdl;

	mt_hdl  = &Axis->NetAxisIf->mt_hdl;

	*pValue = mt_hdl->psnHdl.Perr;
#endif
	return PRM_RSLT_SUCCESS;
}
/****************************************************************************
0xC06	�ڕW�ʒu(TPOS)
****************************************************************************/
PRM_RSLT UnReadTpos(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

#if ( (CSW_NETIF_TYPE == NETIF_M2_TYPE) || (CSW_NETIF_TYPE == NETIF_M3_TYPE) )	/* M-2IF or M-3IF */
	MT_HNDL		*mt_hdl;

	mt_hdl  = &Axis->NetAxisIf->mt_hdl;

	*pValue = mt_hdl->psnHdl.Tpos;
#endif

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0xC08	���b�`�ʒu�P(LPOS1)
****************************************************************************/
PRM_RSLT UnReadLpos1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

#if ( (CSW_NETIF_TYPE == NETIF_M2_TYPE) || (CSW_NETIF_TYPE == NETIF_M3_TYPE) )	/* M-2IF or M-3IF */
	MT_HNDL		*mt_hdl;

	mt_hdl  = &Axis->NetAxisIf->mt_hdl;

#if (CSW_NETIF_TYPE == NETIF_M3_TYPE)	/* M3IF */
	*pValue = mt_hdl->psnHdl.Lpos[0];
#else
	*pValue = mt_hdl->psnHdl.Lpos;		/* M2IF */
#endif

#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0xC0A	���b�`�ʒu�Q(LPOS2)
****************************************************************************/
PRM_RSLT UnReadLpos2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

#if(CSW_NETIF_TYPE == NETIF_M3_TYPE)	/* M-3IF */
	MT_HNDL		*mt_hdl;

	mt_hdl  = &Axis->NetAxisIf->mt_hdl;

	*pValue = mt_hdl->psnHdl.Lpos[1];

#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0xC0C	���b�`�ʒu�R(LPOS3)
****************************************************************************/
PRM_RSLT UnReadLpos3(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

#if (CSW_NETIF_TYPE == NETIF_M3_TYPE)	/* M-3IF */
	MT_HNDL		*mt_hdl;

	mt_hdl  = &Axis->NetAxisIf->mt_hdl;

	*pValue = mt_hdl->psnHdl.Lpos[2];

#endif

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0xC0E	�ڕW���x(TSPD)
****************************************************************************/
PRM_RSLT UnReadTspd(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

#if ( (CSW_NETIF_TYPE == NETIF_M2_TYPE) || (CSW_NETIF_TYPE == NETIF_M3_TYPE) )	/* M-2IF or M-3IF */
	MT_HNDL		*mt_hdl;

	mt_hdl  = &Axis->NetAxisIf->mt_hdl;

	*pValue = mt_hdl->cmdData.TSpeed;

#endif

	return PRM_RSLT_SUCCESS;

}

/****************************************************************************
0xC10	�t�B�[�h�o�b�N���x(FSPD)
****************************************************************************/
PRM_RSLT UnReadFbspd(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

#if ( (CSW_NETIF_TYPE == NETIF_M2_TYPE) || (CSW_NETIF_TYPE == NETIF_M3_TYPE) )	/* M-2IF or M-3IF */
	MT_HNDL		*mt_hdl;

	mt_hdl  = &Axis->NetAxisIf->mt_hdl;

	*pValue = mt_hdl->psnHdl.Fbspd;

#endif


	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0xC12	���݈ʒu�w�ߑ��x(CSPD)
****************************************************************************/
PRM_RSLT UnReadCspd(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

#if ( (CSW_NETIF_TYPE == NETIF_M2_TYPE) || (CSW_NETIF_TYPE == NETIF_M3_TYPE) )	/* M-2IF or M-3IF */
	MT_HNDL		*mt_hdl;

	mt_hdl  = &Axis->NetAxisIf->mt_hdl;
	*pValue = mt_hdl->cmdData.Velocity;

#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0xC14	�g���N�����l(TRQ_LIM)
****************************************************************************/
PRM_RSLT UnReadTrqLim(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

#if ( (CSW_NETIF_TYPE == NETIF_M2_TYPE) || (CSW_NETIF_TYPE == NETIF_M3_TYPE) )	/* M-2IF or M-3IF */
/* TODO */
#endif

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0xC16	���x�����l(SPD_LIM)
****************************************************************************/
PRM_RSLT UnReadSpdLim(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if ( (CSW_NETIF_TYPE == NETIF_M2_TYPE) || (CSW_NETIF_TYPE == NETIF_M3_TYPE) )	/* M-2IF or M-3IF */

	SERVO_CONTROL_IF	*SvControlIf;

	SvControlIf		= Axis->SvControlIf;

	*pValue = SvControlIf->NetSpdLmt;
#endif

	return PRM_RSLT_SUCCESS;
}
/* <S056>	End */

/****************************************************************************
0xC30	M-�V�ʐM�t�F�[�Y
****************************************************************************/
PRM_RSLT UnReadM3CommPhase(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if (CSW_NETIF_TYPE == NETIF_M3_TYPE)
	*pValue = Axis->NetAxisIf->com_hdl.Phase;
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0xC31	M-�V���ݎ��s�R�}���h�R�[�h(RCMD)
****************************************************************************/
PRM_RSLT UnReadM3MainCommandCode(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if (CSW_NETIF_TYPE == NETIF_M3_TYPE)
	*pValue = Axis->NetAxisIf->cmd_hdl.cmdInfo.crtCmd;
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0xC32	M-�V���ݎ��s�T�u�R�}���h�R�[�h(SUB_RCMD)
****************************************************************************/
PRM_RSLT UnReadM3SubCommandCode(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if (CSW_NETIF_TYPE == NETIF_M3_TYPE)
	EX_CMD_INFO *exCmdInfo;
	exCmdInfo = &(Axis->NetAxisIf->cmd_hdl.exCmdInfo);
	*pValue = exCmdInfo->cmdBuf.stdData.mon.cmd;
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
		Un���j�^ �_�~�[�֐�		<S020>
****************************************************************************/
PRM_RSLT UnReadDummy(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = 0;
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
0xF0000050	���͐M�����j�^	InSignal
****************************************************************************//* <S1C2> */
PRM_RSLT UnReadDigitalAllInputSignals1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = (HALdrv_ReadMonAllInputSignals(  ) & 0x000000FF);
	
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0xF0000051	���͐M�����j�^	InSignal
****************************************************************************//* <S1C2> */
PRM_RSLT UnReadDigitalAllInputSignals2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = ((HALdrv_ReadMonAllInputSignals( )>>8) & 0x000000FF);
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0xF0000052	�o�͐M�����j�^	OutSignal
****************************************************************************//* <S1C2> */
PRM_RSLT UnReadDigitalAllOutputSignals(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	*pValue = HALdrv_ReadMonAllOutputSignals( );
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
0xF0000010	�o�͐M�����j�^	OutSignal                                 <S225> 
****************************************************************************/
PRM_RSLT UnMaxSpdOrMaxDiv(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	if( Axis->MencV->AxisMotType == MOTTYPE_LINEAR )
	{
		if( Axis->UniPrms->Prm->b_prm80 & 0xF000 )
		{/* �����o�͐ݒ�Œ�ōő呬�x���Z�o */
			*pValue = Axis->MencV->DivInfo.MaxDivSpd;
		}
		else
		{/* �ō����x�Œ�ŕ����o�͐ݒ���Z�o */
			*pValue = Axis->MencV->DivInfo.MaxDivRat;
		}
		
	}
	else
	{
		*pValue = 0;
	}
	
	return PRM_RSLT_SUCCESS;
}


/******************************* End of File *******************************/
