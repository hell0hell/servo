/****************************************************************************************************/
/*																									*/
/*																									*/
/*		CheckAlarm.c : �A���[���Ď�������`															*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �A���[���Ď�����																		*/
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
#include "MLib.h"
#include "KLib.h"
#include "HwDeviceIf.h"
#include "SerialEncIf.h"
#include "AlarmManager.h"
#include "CheckAlarm.h"

/* <S154> Start */
/*--------------------------------------------------------------------------------------------------*/
/*			�ߕ��׉��Z��ԑI��																			*/
/*--------------------------------------------------------------------------------------------------*/
#define NORMAL_AREA			1U
#define LOWSPD_AREA			2U
#define LOCK_AREA			3U

/*--------------------------------------------------------------------------------------------------*/
/*		�����������x�ُ�A���[���`�F�b�N������`												*/
/*--------------------------------------------------------------------------------------------------*/
#define MINUTE		60000U

/* <S154> End */

/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
static void	AdetCheckSencAlarmCode( MENCV *MencV, ALMMAN_HANDLE *AlmMngr );
//static void	AdetExeCheckOLAmpMot( LONG AbsMotSpd, LONG Ixsqr, CHECK_OL *ol_ptr, ALMMAN_HANDLE *AlmMngr );			/* <S154> */
static void	AdetExeCheckOLAmpMot( LONG AbsMotSpd, LONG Ixsqr, CHECK_OL *ol_ptr, ALMMAN_HANDLE *AlmMngr, UCHAR Mode );	/* <S154> */
static BOOL	AdetPcalOverLoadLevelAmpLow( CHECK_ALARM *CheckAlarm, LONG AbsMotSpd); 										/* <S154> */




/****************************************************************************************************/
/*																									*/
/*		Initialize Servo Alarm Check Variables														*/
/*																									*/
/****************************************************************************************************/
void	AdetInitCheckAlarm( CHECK_ALARM *pChkAlm, ULONG AlmOLInfo )
{
/*--------------------------------------------------------------------------------------------------*/
/*		�A���[���`�F�b�N�p�S�ϐ��[���N���A															*/
/*--------------------------------------------------------------------------------------------------*/
	MlibResetLongMemory( &(pChkAlm->MotOL.var), sizeof( pChkAlm->MotOL.var )/4 );
	MlibResetLongMemory( &(pChkAlm->AmpOL.var), sizeof( pChkAlm->AmpOL.var )/4 );
	MlibResetLongMemory( &(pChkAlm->HwbbTiming.var), sizeof( pChkAlm->HwbbTiming.var )/4 );

/*--------------------------------------------------------------------------------------------------*/
/*		�A���v�����[�^�ߕ��׃`�F�b�N�p�ϐ�������													*/
/*--------------------------------------------------------------------------------------------------*/
	if( AlmOLInfo & ALMDEF_OLINFO_OLF )
	{
#if 1	/* <S154> */
		pChkAlm->MotOL.var.SumImax = (pChkAlm->MotOL.conf.NormalArea.AlmLvlmax >> 1);
		pChkAlm->MotOL.var.SumImid = (pChkAlm->MotOL.conf.NormalArea.AlmLvlmid >> 1);
		pChkAlm->AmpOL.var.SumImax = (pChkAlm->AmpOL.conf.NormalArea.AlmLvlmax >> 1);
		pChkAlm->AmpOL.var.SumImid = (pChkAlm->AmpOL.conf.NormalArea.AlmLvlmid >> 1);
#else	/* <S154> */
		pChkAlm->MotOL.var.SumImax = (pChkAlm->MotOL.conf.AlmLvlmax >> 1);
		pChkAlm->MotOL.var.SumImid = (pChkAlm->MotOL.conf.AlmLvlmid >> 1);
		pChkAlm->AmpOL.var.SumImax = (pChkAlm->AmpOL.conf.AlmLvlmax >> 1);
		pChkAlm->AmpOL.var.SumImid = (pChkAlm->AmpOL.conf.AlmLvlmid >> 1);
#endif	/* <S154> */
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�c�a�ߕ��׃`�F�b�N�p�ϐ�������																*/
/*--------------------------------------------------------------------------------------------------*/
	if( AlmOLInfo & ALMDEF_OLINFO_DBOL )
	{
		pChkAlm->DbOL.var.DbolPowSum = (pChkAlm->DbOL.conf.DbolAlmLevel >> 1);
	}

}



/****************************************************************************************************/
/*																									*/
/*		�`�r�h�b�n�[�h���o�ُ�`�F�b�N����															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �`�r�h�b�n�[�h�E�F�A�����o����ُ���Ď�����											*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
USHORT	AdetCheckAsicHwAlarm( CHECK_ALARM *pChkAlm, ASICS *SvAsic,
										BOOL EncDisable, ALMMAN_HANDLE *AlmMngr )
{
	USHORT 	faultState;
	USHORT	microFault;

	faultState = SHalDrv_GetFaultStatus(SvAsic->AsicHwReg, SHAL_FAULTS);
/*----------------------------------------------------------------------------------------------*/
/*		Micro�ُ�																				*/
/*----------------------------------------------------------------------------------------------*/
	if( (faultState & SHAL_FAULT_BB) == 0 )
	{ /* FLTSTAT_bit10  0:�x�[�X�u���b�N���� */
//		microFault = SvAsic->AsicMicroReg->MREG_FCCSTMON;
		microFault = SHalDrv_GetMicroFaultSts( SvAsic->AsicMicroReg );
		if( (microFault & 0x0001) != 0 )
		{
			ALMSetServoAlarm( AlmMngr, ALM_CURAD_U );	/* A.B31 : �d�����o�ُ�P(�t��)			*/
		}
		if( (microFault & 0x0002) != 0 )
		{
			ALMSetServoAlarm( AlmMngr, ALM_CURAD_V );	/* A.B32 : �d�����o�ُ�Q(�u��)			*/
		}
	}

/*--------------------------------------------------------------------------------------------------*/
/*		Host-CPU : �E�H�b�`�h�b�O�ُ�																*/
/*--------------------------------------------------------------------------------------------------*/
	if( (faultState & SHAL_FAULT_WDOUT) != 0 )
	{ /* FLTSTAT_bit9  1:�z�X�gCPU�pWDT�^�C���I�[�o�[ */
		/* A.BF4 : �V�X�e���A���[���S(CpuWDT) */
		ALMSetServoAlarm( AlmMngr, ALM_JL056_GUP );
	}

/*--------------------------------------------------------------------------------------------------*/
/*		Asic-CPU : �E�H�b�`�h�b�O�ُ�																*/
/*--------------------------------------------------------------------------------------------------*/
	if( ((faultState & SHAL_FAULT_GUP1) != 0) && (EncDisable == FALSE) )
	{ /* FLTSTAT_bit11 1:�}�C�N�����Z��WDT�^�C���I�[�o�[ */
		if( pChkAlm->Gup1Timer >= KPI_SCANC_MS( 8 ))
		{
			/* A.B33 : �d�����o�ُ�R(MicroWDT) */
			//ALMSetServoAlarm( AlmMngr, ALM_JL056_GUP1 );
			ALMSetGlobalAlarm( ALM_JL056_GUP1 );/*<S1BC>*/
			pChkAlm->Gup1Timer = KPI_SCANC_MS( 8 );
		}
		pChkAlm->Gup1Timer++;
	}
	else
	{
		pChkAlm->Gup1Timer = 0;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�ߓd��(OC)�`�F�b�N																			*/
/*--------------------------------------------------------------------------------------------------*/
#if 0/* <S126>:�{�֐��ł͎��{���Ȃ� */
	{/* 0����	*/
	if( (SHalDrv_GetMicroWdtStatus(SvAsic->AsicHwReg) == 0)
		&& ((faultState & SHAL_FAULT_OC) != 0) )
	{ /* WDT1SET_bit6  0:�}�C�N�����Z���pWDT���쒆 && FLTSTAT_bit5  1:�ߓd���ُ픭�� */
		/* A.100 : �ߓd��(�n�b) */
		ALMSetServoAlarm( AlmMngr, ALM_OC );
	}
	}
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		�q�[�g�V���N�ߔM(OH)�`�F�b�N																*/
/*--------------------------------------------------------------------------------------------------*/
#if 0	/* �ȉ�MD�̏��� <S058> */
	if( (faultState & SHAL_FAULT_OH) != 0 )
	{ /* FLTSTAT_bit4  1:�q�[�g�V���N�ߔM�ُ픭��(INGRAM�ł�JL-077B��OV�|�[�g�ɂ�OH���o) */
		ALMSetServoAlarm( AlmMngr, ALM_OH );				/* A.7A0 : �q�[�g�V���N�ߔM				*/
	}
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		�ߓd��(OV)�`�F�b�N <S058>																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( (faultState & SHAL_FAULT_OV) != 0 )
	{
//-		SHalDrv_BaseBlock( SvAsic->AsicHwReg );		/* �`�r�h�b�x�[�X�u���b�N						*/
		/* BaseBlock */
		SHalDrv_BaseBlock(SvAsic->AsicHwReg, SvAsic->AsicMicroReg); /* <S0E4> */

		ALMSetServoAlarm( AlmMngr, ALM_OV );		/* A.400 : �ߓd��(�n�u)							*/
	}

	pChkAlm->faultState = faultState;
	return faultState;
}


/****************************************************************************************************/
/*																									*/
/*		�`�r�h�b�n�[�h���o�ُ�`�F�b�N����															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �`�r�h�b�n�[�h�E�F�A�����o����ُ���Ď�����											*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
void	AdetCheckOCAlarmDetections( ALMMAN_HANDLE *AlmMngr, ASICS *SvAsic, LONG axisNo, BOOL BeSts )
{
	USHORT 	faultState;

	if(axisNo == 0)
	{/* 0����	*/
		faultState = SHalDrv_GetFaultStatus(SvAsic->AsicHwReg, SHAL_FAULTS);
//		if( (SHalDrv_GetMicroWdtStatus(SvAsic->AsicHwReg) == 0)		/* <S137> */
		if(( BeSts == TRUE )
			&& ((faultState & SHAL_FAULT_OC1) != 0) )
		{ /* WDT1SET_bit6  0:�}�C�N�����Z���pWDT���쒆 && FLTSTAT_bit5  1:�ߓd���ُ픭�� */
			/* A.100 : �ߓd��(�n�b) */
			ALMSetServoAlarm( AlmMngr, ALM_OC );
		}
	}
	else
	{/* 1����	*/
		faultState = SHalDrv_GetSetreg1Status(SvAsic->AsicHwReg);
//		if( (SHalDrv_GetMicroWdtStatus((ASIC_HW_IF_STR *)(0xC0001000)) == 0)		/* <S137> */
		if(( BeSts == TRUE )
			&& ((faultState & SHAL_FAULT_OC2) != 0) )
		{ /* WDT1SET_bit6  0:�}�C�N�����Z���pWDT���쒆 && FLTSTAT_bit5  1:�ߓd���ُ픭�� */
			/* A.100 : �ߓd��(�n�b) */
			ALMSetServoAlarm( AlmMngr, ALM_OC );
		}
	}

	/* Get OC Status for Auto Test *//* <S1C9> */
	faultState = SHalDrv_GetFaultStatus(SvAsic->AsicHwReg, SHAL_FAULTS);
	AlmMngr->OCAlmOccurFlag1 = ((faultState & SHAL_FAULT_OC1) != 0) ? TRUE : FALSE;					  /* <Oka05> */
	faultState = SHalDrv_GetSetreg1Status(SvAsic->AsicHwReg);
	AlmMngr->OCAlmOccurFlag2 = ((faultState & SHAL_FAULT_OC2) != 0) ? TRUE : FALSE;					  /* <Oka05> */

}



/*<S1F5> START*/
/*****************************************************************************
  Description: ���[�^�ߓd�����o�`�F�b�N����
------------------------------------------------------------------------------
  Parameters:
	AlmMngr             �A���[���Ǘ��h�e�f�[�^
	SvAsic				ASIC�A�h���X�e�[�u���ւ̃|�C���^
	MencP				�G���R�[�_�p�����[�^��`
	BeSts               �x�[�X�C�l�[�u���������
  Return:

------------------------------------------------------------------------------
  Note:

*****************************************************************************/
void	AdetCheckMotorOverCurrent( ALMMAN_HANDLE *AlmMngr, ASICS *SvAsic, BOOL BeSts, BOOL BaseEnableSts )
{
	USHORT 	faultState;

//	if( BaseEnableSts == TRUE )
//	{ /* SVON���̎�������ꍇ */
//		faultState = SHalDrv_GetSetreg1Status(SvAsic->AsicHwReg);
//		if(( BeSts == TRUE ) && ( ((faultState & SHAL_FAULT_OCALMULT) != 0) ||
//				((faultState & SHAL_FAULT_OCALMVLT) != 0) || ((faultState & SHAL_FAULT_OCALMWLT) != 0) ) )
//		{/* U ���AV ���AW�������ꂩ�̃A���[�����b�`�l���u�ُ픭���v�ł������ꍇ */
//			ALMSetServoAlarm( AlmMngr, ALM_OVERCURRENT );		/* A.101 : ���[�^�ߓd��				*/
//		}
//	}
/* <S1F7> */
	if( BeSts == TRUE )
	{
		faultState = SHalDrv_GetSetreg1Status(SvAsic->AsicHwReg);
		if( faultState & ( SHAL_FAULT_OCALMULT | SHAL_FAULT_OCALMVLT | SHAL_FAULT_OCALMWLT ))
		{
			ALMSetServoAlarm( AlmMngr, ALM_OVERCURRENT );		/* A.101 : ���[�^�ߓd��				*/
		}
	}
}
/*<S1F5> END*/



/****************************************************************************************************/
/*																									*/
/*		�V���A���G���R�[�_�A���[���Ď�����(Called from ScanC)										*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �V���A���G���R�[�_�ʐM�ُ�,�I�[�o�q�[�g�y�уA���[���R�[�h���Ď�����B					*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
void	AdetCheckSencAlarm( ALMMAN_HANDLE *AlmMngr, MENCV *MencV, USHORT RxAlmCode )
{
/*--------------------------------------------------------------------------------------------------*/
/*		Update Motor Encoder Ready Flag																*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->EncConnect && (!MencV->EncDisable) )
	{
		MencV->EncReady = MencV->SenReady;
	}
	else
	{
		MencV->EncReady = FALSE;
	}
	MencV->RxAlmCode = RxAlmCode;
/*--------------------------------------------------------------------------------------------------*/
/*		�`�F�b�N���s : �ʐM���� and �X�L�����`���s��												*/
/*--------------------------------------------------------------------------------------------------*/
	if( (MencV->SeRxErr == FALSE) && (MencV->EncDisable == FALSE) )
	{
		AdetCheckSencBaseTimer( MencV, AlmMngr );			/* �G���R�[�_�x�[�X�^�C�}�[�Ď�<S0B9>	*/
		AdetCheckSencAlarmCode( MencV, AlmMngr );			/* �G���R�[�_�A���[���R�[�h�Ď�			*/
	/*----------------------------------------------------------------------------------------------*/
//		if( MencV->AbsoEncoder == TRUE ) /* <S03A> */
		if( (MencV->AbsoEncoder == TRUE) || (MencV->IncDetOhFlg == TRUE) )
		{
			AdetCheckSencOverHeat( MencV, AlmMngr );		/* �G���R�[�_�I�[�o�q�[�g�Ď�			*/
		}
	/*----------------------------------------------------------------------------------------------*/
		if( MencV->ChkAlarmCnt < 10 )
		{
			MencV->ChkAlarmCnt++;							/* �`�F�b�N�J�E���^�̍X�V				*/
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�`�F�b�N����s : �ʐM�ُ� or �X�L�����`��~��												*/
/*--------------------------------------------------------------------------------------------------*/
	else
	{
		MencV->ChkAlarmCnt = 0;								/* �`�F�b�N�J�E���^�̃N���A				*/
	}

}



/****************************************************************************************************/
/*																									*/
/*		�V���A���G���R�[�_�x�[�X�^�C�}�[�Ď�<S0B9>													*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �V���A���G���R�[�_���o�P���[�h���ɁA�G���R�[�_�̃^�C���x�[�X�^�C�}���ω�				*/
/*			���Ȃ������ꍇ�A�G���R�[�_�ʐM�ُ�A���[���𔭐�����B									*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
void	AdetCheckSencBaseTimer( MENCV *MencV, ALMMAN_HANDLE *AlmMngr )
{
	LONG	BaseTimer;
	BaseTimer = MencV->BaseTimer;								/* �^�C���x�[�X�^�C�}����l			*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->ChkAlarmCnt >= 2 )								/* �R��ڈȍ~�`�F�b�N����			*/
	{
		if( BaseTimer == MencV->ChkBaseTimer )					/* �^�C���x�[�X�^�C�}�ω�����?		*/
		{
			/* �G���R�[�_�ʐM�ُ�`�F�b�N */
			if( !ALMCheckEachState( AlmMngr, ALM_PGCOM ) )
			{
				ALMSetServoAlarm( AlmMngr, ALM_PGCOM2 );		/* A.C92 : �G���R�[�_�ʐM�^�C�}�ُ�	*/
			}
		}
	}
/*--------------------------------------------------------------------------------------------------*/
	MencV->ChkBaseTimer = BaseTimer;							/* �^�C���x�[�X�^�C�}�O��l�X�V		*/

}



/****************************************************************************************************/
/*																									*/
/*		�V���A���G���R�[�_�A���[���R�[�h�Ď�														*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �o�P���[�h�V���A���G���R�[�_��M�A���[���R�[�h���A�u�\�G���R�[�_��						*/
/*			�C���N�������^���G���R�[�_�ɕ����Ĕ��ʂ���B											*/
/*			��M�Ɏ��s���Ă�����A���[���R�[�h�p�ϐ�(p1mode_d0)���N���A����B						*/
/*			��M�����̏ꍇ�̂݃A���[�����m�F����A�������ǂݏo�����O�̎�M���s���m�F����B			*/
/*			�ǂݍ��񂾃A���[���R�[�h���Q��ǂ݃t�B���^����������ŁA�A�u�\�܂��̓C���N��			*/
/*			��F�����Ċe�A���[���R�[�h��͊֐����R�[������B										*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static void	AdetCheckSencAlarmCode( MENCV *MencV, ALMMAN_HANDLE *AlmMngr )
{
	LONG	AlmCode;
/*--------------------------------------------------------------------------------------------------*/
/*		�G���R�[�_�A���[���R�[�h�Ď�����															*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->ChkAlarmCnt >= 2 )						/* �R��ڈȍ~�`�F�b�N����				*/
	{
		MencV->ChkAlmCode[0] =
			( ((MencV->ChkAlmCode[1] | MencV->RxAlmCode) & MencV->ChkAlmCode[0] )
				| (MencV->ChkAlmCode[1] & MencV->RxAlmCode) );
		MencV->ChkAlmCode[1] = MencV->RxAlmCode;

		/* �A���[���R�[�h���� */
		AlmCode = MencV->ChkAlmCode[0];

		if( MencV->AbsoEncoder == TRUE )
		{ /* �A�u�\�G���R�[�_ */
			if( MencV->AxisMotType == MOTTYPE_LINEAR )
			{ /* ���j�A�^ */
			/*--------------------------------------------------------------------------------------------------*/
			/*		EnDat�X�P�[�����}�X�N																		*/
			/*--------------------------------------------------------------------------------------------------*/
				if( MencV->SerialConvType == SCONVTYPE_ENDAT_ABS )
				{
					AlmCode = ( AlmCode & 0x0b );
				}
			/*--------------------------------------------------------------------------------------------------*/
			/*		Mitutoyo, SMS�X�P�[�����}�X�N																*/
			/*--------------------------------------------------------------------------------------------------*/
				else
				{
					AlmCode = ( AlmCode & 0x3a );
				}

				if( AlmCode & 0x01 )
				{
					/* A.890 : �G���R�[�_�X�P�[���G���[ */
					ALMSetServoAlarm( AlmMngr, ALM_SCL_ERR );
				}
			/*----------------------------------------------------------------------------------------------*/
				if( AlmCode & 0x02 )
				{
					/* A.891 : �G���R�[�_���W���[���G���[ */
					ALMSetServoAlarm( AlmMngr, ALM_MOD_ERR );
				}
			/*----------------------------------------------------------------------------------------------*/
				if( AlmCode & 0x08 )
				{
					/* A.840 : �G���R�[�_�f�[�^�A���[�� */
					ALMSetServoAlarm( AlmMngr, ALM_PG4 );
				}
			/*----------------------------------------------------------------------------------------------*/
				if( AlmCode & 0x10 )
				{
					/* A.850 : �G���R�[�_�I�[�o�X�s�[�h */
					ALMSetServoAlarm( AlmMngr, ALM_PG5 );
				}
			/*----------------------------------------------------------------------------------------------*/
				if( AlmCode & 0x20 )
				{
					/* A.860 : �G���R�[�_�I�[�o�q�[�g */
					ALMSetServoAlarm( AlmMngr, ALM_PG6 );
				}
			}
			else
			{ /* ��]�^ */
				if( MencV->AbsoEncType != MENC_MABSO_TYPE )
				{ /* �C���N���g�p�� or �P��]�A�u�\���}�X�N */
					AlmCode = ( AlmCode & 0x3a );
				}

				/* �A���[���R�[�h�`�F�b�N���� */
				if( AlmCode & 0x01 )
				{ /* A.810 : �G���R�[�_�o�b�N�A�b�v�A���[�� */
					ALMSetServoAlarm( AlmMngr, ALM_PG1 );
				}
				if( AlmCode & 0x02 )
				{ /* A.820 : �G���R�[�_�T���`�F�b�N�A���[�� */
					ALMSetServoAlarm( AlmMngr, ALM_PG2 );
				}
				if( AlmCode & 0x04 )
				{
					if( (4000 > KlibGetTimeAfterPowerOn()) && (MencV->BatWrnMode == FALSE) )
					{ /* A.830 : �G���R�[�_�o�b�e���A���[�� */
						ALMSetServoAlarm( AlmMngr, ALM_PG3 );
					}
			/*------------------------------------------------------------------------------------------*/
					else if(MencV->BatWrnMode == TRUE)
					{ /* A.930 : �G���R�[�_�o�b�e�����[�j���O */
						ALMSetServoAlarm( AlmMngr, WRN_PGBAT );
					}
				}
				else
				{ /* A.930 : �G���R�[�_�o�b�e�����[�j���O */
					ALMClearWarning( AlmMngr, WRN_PGBAT );
				}
			/*----------------------------------------------------------------------------------------------*/
				if( AlmCode & 0x08 )
				{ /* A.840 : �G���R�[�_�f�[�^�A���[�� */
					ALMSetServoAlarm( AlmMngr, ALM_PG4 );
				}
			/*----------------------------------------------------------------------------------------------*/
				if( AlmCode & 0x10 )
				{ /* A.850 : �G���R�[�_�I�[�o�X�s�[�h */
					ALMSetServoAlarm( AlmMngr, ALM_PG5 );
				}
			/*----------------------------------------------------------------------------------------------*/
				if( AlmCode & 0x20 )
				{ /* A.860 : �G���R�[�_�I�[�o�q�[�g */
					ALMSetServoAlarm( AlmMngr, ALM_PG6 );
				}
			}
		}
		else
		{ /* �C���N���G���R�[�_ */
			/*	�A���[���R�[�h�`�F�b�N���� */
			if( AlmCode & 0x02 )
			{ /* A.820 : �G���R�[�_�T���`�F�b�N�A���[�� */
				ALMSetServoAlarm( AlmMngr, ALM_PG2 );
			}
			if( AlmCode & 0x08 )
			{ /* A.840 : �G���R�[�_�f�[�^�A���[�� */
				ALMSetServoAlarm( AlmMngr, ALM_PG4 );
			}
		}
	}
/*--------------------------------------------------------------------------------------------------*/
	else													/* �Q��ڂ܂ł̓`�F�b�N���Ȃ�			*/
	{
		MencV->ChkAlmCode[0] = 0;						/* �A���[���R�[�h���Z�b�g				*/
		MencV->ChkAlmCode[1] = 0;						/* �A���[���R�[�h���Z�b�g				*/
	}

}



/****************************************************************************************************/
/*																									*/
/*		�V���A���G���R�[�_�I�[�o�q�[�g�Ď�															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �G���R�[�_���C���̌��o�y�уI�[�o�[�q�[�g�̌��o���s���B									*/
/*																									*/
/*			0x00 �` 0xD7 :  0 �` 215 deg															*/
/*			0xFF �` 0xD8 : -1 �` -40 deg															*/
/*																									*/
/*																									*/
/****************************************************************************************************/
void	AdetCheckSencOverHeat( MENCV *MencV, ALMMAN_HANDLE *AlmMngr )
{
//	LONG	enc_temperatur; /* <S03A> */
	USHORT	enc_temperatur;

//	enc_temperatur = MencV->RxAlmCode & 0xff00;	/* <S03A> */
//	enc_temperatur = enc_temperatur >> 8;		/* <S03A> */
	enc_temperatur = (MencV->RxAlmCode >> 8) & 0x00ff;

/*--------------------------------------------------------------------------------------------------*/
/*		�G���R�[�_���C���ݒ�																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( enc_temperatur < 0xd8 )
	{
//		MencV->Temperature = enc_temperatur;					/* �G���R�[�_���C��(��)				*//* <S03A> */
		MencV->Temperature = (UCHAR)enc_temperatur;				/* �G���R�[�_���C��(��)				*/
	}
	else
	{
//		MencV->Temperature = -enc_temperatur;					/* �G���R�[�_���C��(��)				*//* <S03A> */
		MencV->Temperature = (CHAR)enc_temperatur;				/* �G���R�[�_���C��(��)				*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�G���R�[�_���C���`�F�b�N																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->Temperature > PG_OH_LEVEL )					/* ���x���x���`�F�b�N					*/
	{
		if( MencV->OverHeatCnt < KPI_SCANC_MS( 500 ) )		/* �G���[�J�E���^�`�F�b�N				*/
		{
			MencV->OverHeatCnt++;							/* Count up Error Counter				*/
		}
		else												/* 500ms�ȏ�A���ŃA���[��				*/
		{
			ALMSetServoAlarm( AlmMngr, ALM_PG6 );			/* A.860 : �G���R�[�_�I�[�o�q�[�g		*/
		}
	}
	else
	{
		MencV->OverHeatCnt = 0;							/* �G���[�J�E���^���Z�b�g				*/
	}
}



/****************************************************************************************************/
/*																									*/
/*		�ߑ��x(�n�r)�`�F�b�N����																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : ���[�^��]���x���ߑ��x���o���x���𒴂����ꍇ�A�ߑ��x�A���[���𔭐�����B				*/
/*			�ߑ��x���o���x���́A�G���R�[�_�p�����[�^�̃��[�^�ő��]���x(PnF05.��ʃo�C�g)�A		*/
/*			�n�r���o���x��(PnF06.���ʃo�C�g)�ƃV�X�e���萔�̂n�r���o���x��(PnE62.��ʃo�C�g)		*/
/*			�ɂ���Č��܂邪�A24bit(0x1000000)�Ƀm�[�}���C�Y����Ă���B							*/
/*																									*/
/*	���Ӂ@�F�g�p���Ă��郂�[�^��]���x�̍X�V������250us�ł��邽�ߌ��������K�v						*/
/****************************************************************************************************/
void	AdetCheckOverSpeed( ALMMAN_HANDLE *AlmMngr, CHECK_ALARM *pChkAlm, LONG MotSpd, BPRMDAT *Bprm )
{
	LONG	AbsMotSpd;

	AbsMotSpd = MlibLABS( MotSpd );
/*--------------------------------------------------------------------------------------------------*/
/*		�ߑ��x(�n�r)�`�F�b�N																		*/
/*--------------------------------------------------------------------------------------------------*/
//	if( AbsMotSpd >= Bprm->NorOvrSpd )
	if( AbsMotSpd >= Bprm->OsAlmLvl )	/* <S1D0> */
	{
		pChkAlm->OSCount++;
		if( pChkAlm->OSCount >= Bprm->OSCountMax )
		{
			pChkAlm->OSCount = Bprm->OSCountMax;
			ALMSetServoAlarm( AlmMngr, ALM_OSF1 );			/* A.510 : �ߑ��x(�n�r)�A���[��			*/
		}
	}
	else
	{
		pChkAlm->OSCount = 0;
	}
}



/****************************************************************************************************/
/*																									*/
/*		A.511:�����p���X�o�͉ߑ��x�`�F�b�N����									<S02D>				*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : ���[�^��]���x���ߑ��x���o���x���𒴂����ꍇ�A�ߑ��x�A���[���𔭐�����B				*/
/*			�ߑ��x���o���x���́A�G���R�[�_�p�����[�^�̃��[�^�ő��]���x(PnF05.��ʃo�C�g)�A		*/
/*			�n�r���o���x��(PnF06.���ʃo�C�g)�ƃV�X�e���萔�̂n�r���o���x��(PnE62.��ʃo�C�g)		*/
/*			�ɂ���Č��܂邪�A24bit(0x1000000)�Ƀm�[�}���C�Y����Ă���B							*/
/*																									*/
/****************************************************************************************************/
void	AdetCheckDivOverSpeed( ALMMAN_HANDLE *AlmMngr, LONG MotSpd, LONG DivOvrSpd )
{
	LONG	AbsMotSpd;

	AbsMotSpd = MlibLABS( MotSpd );

/*--------------------------------------------------------------------------------------------------*/
/*		�����o�̓p���X�ߑ��x�`�F�b�N																*/
/*--------------------------------------------------------------------------------------------------*/
	if( ALMCheckEachState( AlmMngr, ALM_OSF1 ) ) /*<S079>*/
	{
		;											/* A.510���́A�����o�̓p���X�ߑ��x�`�F�b�N����	*/
	}
	else if( AbsMotSpd >= DivOvrSpd )
	{
		ALMSetServoAlarm( AlmMngr, ALM_OSDIV );				/* A.511 : �����p���X�o�͉ߑ��x			*/
	}
/*--------------------------------------------------------------------------------------------------*/

}



/****************************************************************************************************/
/*																									*/
/*		�A���v�����[�^�ߕ��׃`�F�b�N����		�ᑬ�n�k�F�r�f�c�r����								*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : ���[�^�A�A���v�̋��e�͈͂𒴂��ēd�������ꂽ�ꍇ�A�ُ폈������B						*/
/*			���[�^�A�A���v�͕ʂ̉ߕ��ד��������̂ŁA�����ď�������B								*/
/*																									*/
/*			�g���N�w�߁�d���A�d��^2�唭�M�ʂƍl���A�g���N�w�߂̂Q���ϕ����邱�Ƃɂ��			*/
/*			���M�ʂ����߂�B��i�g���N�ȉ��ł͉��x�͋��e�l���ŖO�a����ƍl���A						*/
/*																									*/
/*			��(�g���N�w��^2�|��i�g���N^2)���ߕ��׌��o���x��										*/
/*																									*/
/*			�ƂȂ����ꍇ�A�ߕ��׈ُ�Ƃ݂Ȃ��B														*/
/*																									*/
/*			���ۂ̉ߕ��ד����̓g���N�w�߂̂Q��v�Z�ƍ���Ȃ��̂ŁA�Q��v�Z�Q�{�ŋߎ�����B			*/
/*			�傫���g���N�ŒZ���Ԃŉߕ��׌��o�ɂȂ�����u���ߕ��ׁA��i�g���N���z�����ӂ��			*/
/*			���Ԃ�������ߕ��׌��o�ɂȂ����A���ߕ��ׂƋ�ʂ���B									*/
/*			���������̓s���łł������o�ł��邪�A�u���ߕ��ׂ͋@�B�̂��ݍ��݂ȂǃA�N�V�f���g			*/
/*			�ɂ��ُ�̈Ӗ������������A�A���ߕ��ׂ͌o���ω��ɂ�蕉�ׂ��傫���Ȃ����Ɖ���			*/
/*			�ł���B����āA�Q�̉ߕ��׌��o�̃A���[���R�[�h�𕪂��ċ�ʂ��邱�Ƃɂ���B			*/
/*																									*/
/*			�V�X�e���⏕�@�\�ŉߕ��׌��o���}�X�N�����ꍇ�A���o������ʂ�Ȃ��B						*/
/*																									*/
/****************************************************************************************************/
//void	AdetCheckOverLoadAmpMot( ALMMAN_HANDLE *AlmMngr, CHECK_ALARM *CheckAlarm,
//								 LONG IdRefMon, LONG IqRefMon, LONG AbsMotSpd, BOOL EncConnect )	/* <S154> */
BOOL	AdetCheckOverLoadAmpMot( ALMMAN_HANDLE *AlmMngr, CHECK_ALARM *CheckAlarm,
								 LONG IdRefMon, LONG IqRefMon, LONG AbsMotSpd, BOOL EncConnect )	/* <S154> */
{
	LONG		Ixsqr;
	CHECK_OL	*MotOL;
	CHECK_OL	*AmpOL;
	CHECK_LOWOL *LowOL;		/* <S154> */
	LOWOLCHKP	*LowOLChkP;	/* <S154> */
	BOOL		ret;		/* <S154> */


	/* ���[�J���ϐ��̏����� */
	Ixsqr =	(IdRefMon * IdRefMon) + (IqRefMon * IqRefMon);
	MotOL = &(CheckAlarm->MotOL);
	AmpOL = &(CheckAlarm->AmpOL);
	LowOL = &(CheckAlarm->AmpLowOL);	/* <S154> */
	LowOLChkP = &(LowOL->LowOLChkP);	/* <S154> */
	ret = TRUE;							/* <S154> */


	/****************************************************************************************/
	/*		���[�^�ߕ��׃A���[���`�F�b�N													*/
	/****************************************************************************************/
	/*--------------------------------------------------------------------------------------*/
	/* 	���[�^�ʏ��Ԃn�k�`�F�b�N															*/
	/*--------------------------------------------------------------------------------------*/
//	AdetExeCheckOLAmpMot( AbsMotSpd, Ixsqr, MotOL, AlmMngr );				/* <S154> */
	AdetExeCheckOLAmpMot( AbsMotSpd, Ixsqr, MotOL, AlmMngr, NORMAL_AREA );	/* <S154> */

/* <S154> Start */
	/****************************************************************************************/
	/*		�A���v�ߕ��׃A���[���`�F�b�N													*/
	/****************************************************************************************/
#if	CSW_SVCF_CHKAMPLOL == 1	/* �ᑬ�n�k����FCSW_SVCF_CHKAMPLOL(�@���`�ɂ��ᑬ�n�k�@�\�̗L��)	*/
	/*--------------------------------------------------------------------------------------*/
	/*	�A���v�ʏ��Ԃn�k�`�F�b�N�F�ᑬ�n�k���s�s�A�܂��́A���[�^���x���ʏ���		*/
	/*--------------------------------------------------------------------------------------*/
	if( (LowOLChkP->conf.LowSpdOLChkEnable == (UCHAR)FALSE) || (AbsMotSpd > LowOLChkP->conf.LowSpdOLchg) )
	{
		AdetExeCheckOLAmpMot( AbsMotSpd, Ixsqr, AmpOL, AlmMngr, NORMAL_AREA);
	}
	/*--------------------------------------------------------------------------------------*/
	/*	�A���v�ᑬ��Ԃn�k�`�F�b�N�F���[�^���x���ᑬ���									*/
	/* 		AdetPcalOverLoadLevelAmpLow( )�c�A���v�ᑬ��ԉߕ��׃`�F�b�N�p�p�����[�^�v�Z�֐�	*/
	/*		AdetExeCheckOLAmpMot( )�c�A���v�ᑬ��Ԃn�k�`�F�b�N�֐�							*/
	/*--------------------------------------------------------------------------------------*/
	else if( (AbsMotSpd <= LowOLChkP->conf.LowSpdOLchg) && (LowOLChkP->conf.LockSpdOLchg <= AbsMotSpd) )
	{
		ret = AdetPcalOverLoadLevelAmpLow( CheckAlarm, AbsMotSpd);
		AdetExeCheckOLAmpMot( AbsMotSpd, Ixsqr, AmpOL, AlmMngr, LOWSPD_AREA);
	}
	/*--------------------------------------------------------------------------------------*/
	/*	�A���v���b�N��Ԃn�k�`�F�b�N�F���[�^���x�����b�N���								*/
	/*--------------------------------------------------------------------------------------*/
	else
	{
		AdetExeCheckOLAmpMot( AbsMotSpd, Ixsqr, AmpOL, AlmMngr,LOCK_AREA);
	}
#else
	/*--------------------------------------------------------------------------------------*/
	/*	�ᑬ�n�k�Ȃ�																		*/
	/* 	�A���v�ʏ��Ԃn�k�`�F�b�N															*/
	/*--------------------------------------------------------------------------------------*/
		AdetExeCheckOLAmpMot( AbsMotSpd, Ixsqr, AmpOL, AlmMngr, NORMAL_AREA);
#endif
/* <S154> End */

	/****************************************************************************************/
	/*		�ߕ��׃��[�j���O�`�F�b�N														*/
	/****************************************************************************************/
	if( EncConnect == TRUE )				/* �G���R�[�_���ڑ��ꍇ�̓��[�j���O���o���Ȃ�	*/
	{
		if( MotOL->var.WrnInfo || AmpOL->var.WrnInfo )
		{
			ALMSetServoAlarm( AlmMngr, WRN_OLF );			/* A.910 : �ߕ��׃��[�j���O���o	*/
		}
		else
		{
			ALMClearWarning( AlmMngr, WRN_OLF );			/* A.910 : �ߕ��׃��[�j���O����	*/
		}
	}
	return ret;		/* <S154> */
}

/* <S154> Start */
#if		( CSW_SVCF_CHKAMPLOL == 1 ) 	
/*****************************************************************************
  Description:		�A���v�ᑬ�ߕ��׃��x���̌v�Z(�I�����C��)
------------------------------------------------------------------------------
  Parameters:
	AlmMngr;		�A���[���Ǘ��f�[�^�|�C���g
	CheckAlarm;		�A���[���Ǘ��f�[�^�|�C���g
	AbsMotSpd;		Absolute Motor Speed
  Return:
	BOOL			TRUE �A���[���Ȃ�
					FALSE	�A���[������
------------------------------------------------------------------------------
  Note:
	���[�^�ᑬ��]���ɁA�n�k�p�����[�^�𑬓x�ɉ����ĘA���I�ɕ�Ԃ��Ăn�k�J�[�u��A���I�ɐؑւ���B
	���[�^��]���x�͒ᑬ��ԓ��ł���΁A���Ԃn�k�d���ƍő�n�k���Ԃ����[�^���x�ɉ����Čv�Z����B
	�v�Z�������Ԃn�k�d���ƍő�n�k���Ԃ��g�p���āA�ᑬ��Ԃ̂n�k���o�A���[�����x����ݒ肷��B
*****************************************************************************/
static BOOL	AdetPcalOverLoadLevelAmpLow( CHECK_ALARM *CheckAlarm, LONG AbsMotSpd)
{
	LONG	Ix;
	LONG	Iy;
	LONG	Ibsqr;
	LONG	Ixsqr;
	LONG	Iysqr;
	LONG	Izsqr;
	LONG	wk;
	BOOL	ret;

	OLPRM *AmpLow;
	CHECK_OLP *AmpLowOLP;
	CHECK_OL *MotOLP;
	CHECK_OL *AmpOLP;
	CHECK_LOWOL *LowOL;
	LowOL = &(CheckAlarm->AmpLowOL);
	MotOLP = &(CheckAlarm->MotOL);
	AmpOLP = &(CheckAlarm->AmpOL);
	AmpLow = &(LowOL->LowOLPrm.AmpLow);
	AmpLowOLP = &(AmpOLP->conf.LowSpdArea);
	ret = TRUE;


	/********************************************************************************************/
	/*		�ᑬ��Ԓ��Ԃn�k�d���ƒᑬ��ԍő�n�k���Ԃ̎Z�o									*/
	/********************************************************************************************/
	/*------------------------------------------------------------------------------------------*/
	/*	�ᑬ��Ԓ��Ԃn�k�d���̌v�Z�F	Ix														*/
	/*------------------------------------------------------------------------------------------*/
	/*	wk = (���[�^���x - ���b�N��ԕϋȓ_���x) * (���Ԃn�k�d���ϊ��W��) 						*/
	/*	�ᑬ��Ԓ��Ԃn�k�d�� = ���b�N��ԕϋȓ_�n�k�d�� �{ wk									*/
	/*------------------------------------------------------------------------------------------*/
	wk = MlibMulgain( (AbsMotSpd - LowOL->LowOLChkP.conf.LockSpdOLchg), LowOL->LowOLChkP.conf.TransOLImid );
	Ix = LowOL->LowOLChkP.conf.LowSpdOffsetImid + wk;

	/*------------------------------------------------------------------------------------------*/
	/*	�ᑬ��ԍő�n�k���Ԃ̌v�Z�F	Iy														*/
	/*------------------------------------------------------------------------------------------*/
	/*	wk = (���[�^���x-���b�N��ԕϋȓ_���x) * (�ő�n�k���ԕϊ��W��) 						*/
	/*	�ᑬ��ԍő�n�k���� = ���b�N��ԕϋȓ_�n�k���ԁ{wk										*/
	/*------------------------------------------------------------------------------------------*/
	wk = (LONG)MlibSqrtu32( AbsMotSpd - LowOL->LowOLChkP.conf.LockSpdOLchg );
	wk = MlibMulgain( wk, LowOL->LowOLChkP.conf.TransOLTmax );
	Iy = LowOL->LowOLChkP.conf.LowSpdOffsetTmax + wk;

	/*------------------------------------------------------------------------------------------*/
	/*	�ᑬ��ԘA���x�[�X�d��^2�F		Ibsqrmid,	Ibsqr										*/
	/*------------------------------------------------------------------------------------------*/
	Ibsqr = AmpOLP->conf.NormalArea.Ibsqrmid;
	AmpLowOLP->Ibsqrmid = Ibsqr;						/* �A���n�k�x�[�X�d��^2�̃Z�b�g				*/

	/********************************************************************************************/
	/*		�ᑬ��ԘA���ߕ��׃��x���̌v�Z														*/
	/********************************************************************************************/
	/*																							*/
	/*		Ibsqr = Ibse * Ibse																	*/
	/*																							*/
	/*				  (Imid^2 - Ibse^2) * Tmid													*/
	/*		AlmLvl = ----------------------------												*/
	/*						  Cycle																*/
	/*																							*/
	/********************************************************************************************/
	/*------------------------------------------------------------------------------------------*/
	/* 	���Ԃn�k�d���̃`�F�b�N�F	Ix															*/
	/*------------------------------------------------------------------------------------------*/
	Ix = (Ix > 0x7FFF)? 0x7FFF : Ix;
	Ixsqr = (Ix * Ix) >> 16;						/* ���Ԃn�k�d��^2							*/

	/*------------------------------------------------------------------------------------------*/
	/*	�A���n�k���o���x���A���x���Q�C���̌v�Z													*/
	/*------------------------------------------------------------------------------------------*/
	/*	Izsqr		�F�A���n�k���o�A���[�����x���̕��q��(���Ԃn�k�d��^2 - �A���n�k�x�[�X�d��^2)	*/
	/*	AlmLvlmid	�F�A���n�k�A���[�����o���x��												*/
	/*	OlLvlmidGain�F�A���n�k���o���x���Q�C��													*/
	/*------------------------------------------------------------------------------------------*/
	Izsqr = Ixsqr - Ibsqr;
	AmpLowOLP->AlmLvlmid = MlibScalKxgain( Izsqr, 100*AmpLow->Tmid, KPI_SCCYCLEMS, NULL, -30 );
	AmpLowOLP->OlLvlmidGain = MlibScalKxgain( AmpOLP->conf.NormalArea.AlmLvlmid, 1, AmpLowOLP->AlmLvlmid, NULL, 24 );
	if( AmpLowOLP->AlmLvlmid >= 0x40000000 )
	{
//			ALMSetPramError( AlmMngr, pndef_olspdlvl.RegNumber);			/* A.040 : �p�����[�^�ݒ�ُ�				*/
		ret = FALSE;
	}

	/********************************************************************************************/
	/*		�ᑬ��ԏu���ߕ��׃��x���̌v�Z														*/
	/********************************************************************************************/
	/*																							*/
	/*				 (Imid^2 * Tmid) - (Imax^2 * Tmax)											*/
	/*		Ibsqr = -----------------------------------											*/
	/*						 (Tmid - Tmax)														*/
	/*																							*/
	/*				  (Imax^2 - Ibsqr) * Tmax													*/
	/*		AlmLvl = ----------------------------												*/
	/*						   Cycle															*/
	/*																							*/
	/********************************************************************************************/
	/*------------------------------------------------------------------------------------------*/
	/*	�ʏ��ԍő�n�k�d���̃`�F�b�N�F	OlPrm->Imax											*/
	/*------------------------------------------------------------------------------------------*/
	if( AmpLow->Imax > 0x7FFF )						/* ���L�l��Б��̐ݒ�l�ɍ��킹��			*/
	{
		AmpLowOLP->Ibsqrmax = MotOLP->conf.NormalArea.Ibsqrmax;		/* �ᑬ��Ԃ̏u���n�k�x�[�X�d��				*/
		AmpLowOLP->AlmLvlmax = AmpOLP->conf.NormalArea.AlmLvlmax;	/* �ᑬ��Ԃ̏u���n�k���x��					*/
		AmpLowOLP->WrnLvlmax = AmpOLP->conf.NormalArea.WrnLvlmax;	/* �ᑬ��Ԃ̏u���n�k���[�j���O���x��		*/
//			return;
	}
	else
	{
		Ix = AmpLow->Imax;

		Iysqr = (Ix * Ix) >> 16;						/* Iysqr�F�ő�n�k�d��^2					*/

		/*------------------------------------------------------------------------------------------*/
		/*	�u���n�k�x�[�X�d��^2�̌v�Z�F	Ibsqr													*/
		/*------------------------------------------------------------------------------------------*/
		wk = (Ixsqr * AmpLow->Tmid) - (Iysqr * Iy);		/* �u���n�k�x�[�X�d���̕��q���v�Z 			*/
		Ibsqr = ( wk ) / ( AmpLow->Tmid - Iy);			/* Ibsqr�F�u���n�k�x�[�X�d��^2				*/

		/*------------------------------------------------------------------------------------------*/
		/* �u���n�k�x�[�X�d��^2�̕��q���`�F�b�N 													*/
		/*------------------------------------------------------------------------------------------*/
		if( wk > 0 )
		{
			/*--------------------------------------------------------------------------------------*/
			/* �u���n�k�x�[�X�d��^2�̃`�F�b�N														*/
			/*--------------------------------------------------------------------------------------*/
			if( Ibsqr < 0 )									/* ���L�l��Б��̐ݒ�l�ɍ��킹��		*/
			{
				AmpLowOLP->Ibsqrmax = MotOLP->conf.NormalArea.Ibsqrmax;		/* �u���n�k�x�[�X�d�� 					*/
			}
			/*--------------------------------------------------------------------------------------*/
			/* �u���n�k�x�[�X�d��^2�ƘA���n�k�x�[�X�d��^2�̑召�֌W�`�F�b�N 						*/
			/*--------------------------------------------------------------------------------------*/
			if( Ibsqr < AmpLowOLP->Ibsqrmid )					/* ���L�l��A���n�k�̐ݒ�l�ɍ��킹��	*/
			{
				AmpLowOLP->Ibsqrmax  = AmpLowOLP->Ibsqrmid;		/* �u���n�k�x�[�X�d��^2					*/
				AmpLowOLP->OlLvlmaxGain = AmpLowOLP->OlLvlmidGain;/* �u���n�k���x���Q�C��				*/
			}
			/*--------------------------------------------------------------------------------------*/
			/* �u���n�k�x�[�X�d��^2�̐ݒ�A�u���n�k���x���A���x���Q�C���̌v�Z						*/
			/*--------------------------------------------------------------------------------------*/
			/*	Izsqr		�F�u���n�k���o�A���[�����x���̕��q��(�ő�n�k�d��^2 - �u���x�[�X�d��^2)	*/
			/*	Ibsqrmax	�F�u���n�k�x�[�X�d��^2													*/
			/*	AlmLvlmax	�F�u���n�k�A���[�����o���x��											*/
			/*	OlLvlmaxGain�F�u���n�k���o���x���Q�C��												*/
			/*--------------------------------------------------------------------------------------*/
			else
			{
				Izsqr = Iysqr - Ibsqr;
				AmpLowOLP->Ibsqrmax  = Ibsqr;
				AmpLowOLP->AlmLvlmax = MlibScalKxgain( Izsqr, (100*Iy), KPI_SCCYCLEMS, NULL, -30 );
				AmpLowOLP->OlLvlmaxGain = MlibScalKxgain( AmpOLP->conf.NormalArea.AlmLvlmax, 1, AmpLowOLP->AlmLvlmax, NULL, 24 );
				if( AmpLowOLP->AlmLvlmax >= 0x40000000 )
				{
//						ALMSetPramError( AlmMngr, pndef_olspdlvl.RegNumber);		/* A.040 : �p�����[�^�ݒ�ُ�			*/
					ret = FALSE;
				}
			}
		}

		/********************************************************************************************/
		/*	�ᑬ��Ԃ̘A���n�k���x���A�u���n�k���x���A�A�����[�j���O���x���A�u�����[�j���O���x���ݒ�*/
		/*		�ᑬ��Ԃ̌��o���x����ʏ��Ԃ̌��o���x���ɐݒ肷��								*/
		/********************************************************************************************/
		AmpLowOLP->AlmLvlmid = AmpOLP->conf.NormalArea.AlmLvlmid;		/* �ᑬ��Ԃ̘A���n�k���x��					*/
		AmpLowOLP->AlmLvlmax = AmpOLP->conf.NormalArea.AlmLvlmax;		/* �ᑬ��Ԃ̏u���n�k���x��					*/
		AmpLowOLP->WrnLvlmid = AmpOLP->conf.NormalArea.WrnLvlmid;		/* �ᑬ��Ԃ̘A���n�k���[�j���O���x��		*/
		AmpLowOLP->WrnLvlmax = AmpOLP->conf.NormalArea.WrnLvlmax;		/* �ᑬ��Ԃ̏u���n�k���[�j���O���x��		*/
	}
	return	ret;
}
/*--------------------------------------------------------------------------------------------------*/
#endif
/* <S154> End */

/****************************************************************************************************/
/*																									*/
/*		�A���v�����[�^�ߕ��׃`�F�b�N�������s�֐�													*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �d���w�߂̂Q�悩��x�[�X�l�����Z�������̂�ώZ����B									*/
/*			�ώZ�l���ߕ��׃A���[�����o���x�����z������ߕ��׃A���[���Ƃ���B						*/
/*			�ώZ�l�����̏ꍇ�́A���x�㏸�͒�~���Ă���Ɣ��f����ώZ�l�͂O�Ń��~�b�g����B			*/
/*			�ώZ�l���V�X�e���萔�Ō��߂�ꂽ�����ɑ�������l�ɒB�����烏�[�j���O�Ƃ���B			*/
/*																									*/
/****************************************************************************************************/
//static void	AdetExeCheckOLAmpMot( LONG AbsMotSpd, LONG Ixsqr, CHECK_OL *ol_ptr, ALMMAN_HANDLE *AlmMngr )			/* <S154> */
static void	AdetExeCheckOLAmpMot( LONG AbsMotSpd, LONG Ixsqr, CHECK_OL *ol_ptr, ALMMAN_HANDLE *AlmMngr, UCHAR Mode )	/* <S154> */
{
	CHECK_OLP	*CheckOLP;		/* <S154> */
	LONG	Sumx;
	LONG	Ixsqr_s;
	LONG	reduceLoad;
//	LONG	reduceLoad_s;
	LONG	ltmp;

	/* ���[�J���ϐ��̏����� */
	Ixsqr_s = Ixsqr >> 16;

/* <S154> Start */
/*--------------------------------------------------------------------------------------------------*/
/*		�ߕ��׉��Z��ԑI��																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( Mode == NORMAL_AREA )
	{
		CheckOLP = &(ol_ptr->conf.NormalArea);
	}
	else if( Mode == LOWSPD_AREA )
	{
		CheckOLP = &(ol_ptr->conf.LowSpdArea);
	}
	else if( Mode == LOCK_AREA )
	{
		CheckOLP = &(ol_ptr->conf.LowSpdArea);
	}
	else
	{
		CheckOLP = &(ol_ptr->conf.NormalArea);
	}
/* <S154> End */

#if (FLOAT_USE==TRUE)
//	reduceLoad = 16384 - (LONG)( (float)AbsMotSpd * ol_ptr->conf.CoolRateGain);		/* ������p��	*/	/* <S154> */
	reduceLoad = 16384 - (LONG)( (float)AbsMotSpd * CheckOLP->CoolRateGain);		/* ������p��	*/	/* <S154> */
#else
//	reduceLoad = 16384 - MlibMulgain( AbsMotSpd, ol_ptr->conf.CoolRateGain );		/* ������p��	*/	/* <S154> */
	reduceLoad = 16384 - MlibMulgain( AbsMotSpd, CheckOLP->CoolRateGain );			/* ������p��	*/	/* <S154> */
#endif /* FLOAT_USE */
//	reduceLoad_s = reduceLoad >> 14;

/*--------------------------------------------------------------------------------------------------*/
/*		�J�n����																					*/
/*--------------------------------------------------------------------------------------------------*/
	ol_ptr->var.WrnInfo = 0x00;					/* ���[�j���O��񃊃Z�b�g					*/

	/********************************************************************************************/
	/*		�u���ߕ��ׂ̃`�F�b�N																*/
	/********************************************************************************************/
#if 1	/* <S154> */
#if (FLOAT_USE==TRUE)
	if( CheckOLP->CoolRateGain == 0 )
	{
		ltmp = (LONG)( (float)(Ixsqr_s - CheckOLP->Ibsqrmax) * CheckOLP->OlLvlmaxGain );
	}
	else
	{
		ltmp = (LONG)( (float)((((SHORT)Ixsqr_s * (SHORT)reduceLoad)>>14) - CheckOLP->Ibsqrmax) * CheckOLP->OlLvlmaxGain );
	}
#else
	if( CheckOLP->CoolRateGain == 0 )
	{
		ltmp = MlibMulgain( (Ixsqr_s - CheckOLP->Ibsqrmax), CheckOLP->OlLvlmaxGain );
	}
	else
	{
	//	ltmp = MlibMulgain( (((SHORT)Ixsqr_s * (SHORT)reduceLoad_s) - CheckOLP->Ibsqrmax), CheckOLP->OlLvlmaxGain );
		ltmp = MlibMulgain( ((((SHORT)Ixsqr_s * (SHORT)reduceLoad)>>14) - CheckOLP->Ibsqrmax), CheckOLP->OlLvlmaxGain );
	}
#endif /* FLOAT_USE */
#else	/* <S154> */
#if (FLOAT_USE==TRUE)
	if( ol_ptr->conf.CoolRateGain == 0 )
	{
		ltmp = (LONG)( (float)(Ixsqr_s - ol_ptr->conf.Ibsqrmax) * ol_ptr->conf.OlLvlmaxGain );
	}
	else
	{
		ltmp = (LONG)( (float)((((SHORT)Ixsqr_s * (SHORT)reduceLoad)>>14) - ol_ptr->conf.Ibsqrmax) * ol_ptr->conf.OlLvlmaxGain );
	}
#else
	if( ol_ptr->conf.CoolRateGain == 0 )
	{
		ltmp = MlibMulgain( (Ixsqr_s - ol_ptr->conf.Ibsqrmax), ol_ptr->conf.OlLvlmaxGain );
	}
	else
	{
	//	ltmp = MlibMulgain( (((SHORT)Ixsqr_s * (SHORT)reduceLoad_s) - ol_ptr->conf.Ibsqrmax), ol_ptr->conf.OlLvlmaxGain );
		ltmp = MlibMulgain( ((((SHORT)Ixsqr_s * (SHORT)reduceLoad)>>14) - ol_ptr->conf.Ibsqrmax), ol_ptr->conf.OlLvlmaxGain );
	}
#endif /* FLOAT_USE */
#endif	/* <S154> */
	Sumx = (LONG)( ol_ptr->var.SumImax + ltmp );

	/*------------------------------------------------------------------------------------------*/
//	if( Sumx >= ol_ptr->conf.AlmLvlmax )			/* �ώZ�l >= �A���[�����x��					*/	/* <S154> */
	if( Sumx >= CheckOLP->AlmLvlmax )				/* �ώZ�l >= �A���[�����x��					*/	/* <S154> */
	{
		ol_ptr->var.SumImax = Sumx;
		ALMSetServoAlarm( AlmMngr, ALM_OLF1 );		/* A.710 : �ߕ���(�u���ő啉��)				*/
	}
//	if( Sumx >= ol_ptr->conf.WrnLvlmax )			/* �ώZ�l >= ���[�j���O���x�� 				*/	/* <S154> */
	if( Sumx >= CheckOLP->WrnLvlmax )				/* �ώZ�l >= ���[�j���O���x�� 				*/	/* <S154> */
	{
		ol_ptr->var.SumImax = Sumx;
		ol_ptr->var.WrnInfo |= 0x01;				/* ���[�j���O���ݒ�						*/
	}
	else if( Sumx >= 0 )							/* �ώZ�l >= 0								*/
	{
		ol_ptr->var.SumImax = Sumx;
	}
	else											/* �ώZ�l < 0								*/
	{
		ol_ptr->var.SumImax = 0;
	}

	/********************************************************************************************/
	/*		�A���ߕ��ׂ̃`�F�b�N																*/
	/********************************************************************************************/
#if 1	/* <S154> */
#if (FLOAT_USE==TRUE)
	if( CheckOLP->CoolRateGain == 0 )
	{
		ltmp = (LONG)( (float)(Ixsqr_s - CheckOLP->Ibsqrmid) * CheckOLP->OlLvlmidGain );
	}
	else
	{
		ltmp = (LONG)( (float)((((SHORT)Ixsqr_s * (SHORT)reduceLoad)>>14) - CheckOLP->Ibsqrmid) * CheckOLP->OlLvlmidGain );
	}
#else
	if( CheckOLP->CoolRateGain == 0 )
	{
		ltmp = MlibMulgain( (Ixsqr_s - CheckOLP->Ibsqrmid), CheckOLP->OlLvlmidGain );
	}
	else
	{
	//	ltmp = MlibMulgain( ( ((SHORT)Ixsqr_s * (SHORT)reduceLoad_s) - CheckOLP->Ibsqrmid), CheckOLP->OlLvlmidGain );
		ltmp = MlibMulgain( ((((SHORT)Ixsqr_s * (SHORT)reduceLoad)>>14) - CheckOLP->Ibsqrmid), CheckOLP->OlLvlmidGain );
	}
#endif /* FLOAT_USE */
#else	/* <S154> */
#if (FLOAT_USE==TRUE)
	if( ol_ptr->conf.CoolRateGain == 0 )
	{
		ltmp = (LONG)( (float)(Ixsqr_s - ol_ptr->conf.Ibsqrmid) * ol_ptr->conf.OlLvlmidGain );
	}
	else
	{
		ltmp = (LONG)( (float)((((SHORT)Ixsqr_s * (SHORT)reduceLoad)>>14) - ol_ptr->conf.Ibsqrmid) * ol_ptr->conf.OlLvlmidGain );
	}
#else
	if( ol_ptr->conf.CoolRateGain == 0 )
	{
		ltmp = MlibMulgain( (Ixsqr_s - ol_ptr->conf.Ibsqrmid), ol_ptr->conf.OlLvlmidGain );
	}
	else
	{
	//	ltmp = MlibMulgain( ( ((SHORT)Ixsqr_s * (SHORT)reduceLoad_s) - ol_ptr->conf.Ibsqrmid), ol_ptr->conf.OlLvlmidGain );
		ltmp = MlibMulgain( ((((SHORT)Ixsqr_s * (SHORT)reduceLoad)>>14) - ol_ptr->conf.Ibsqrmid), ol_ptr->conf.OlLvlmidGain );
	}
#endif /* FLOAT_USE */
#endif	/* <S154> */
	Sumx = (LONG)( ol_ptr->var.SumImid + ltmp );

	/*------------------------------------------------------------------------------------------*/
//	if( Sumx >= ol_ptr->conf.AlmLvlmid )			/* �ώZ�l >= �A���[�����x��					*/	/* <S154> */
	if( Sumx >= CheckOLP->AlmLvlmid )				/* �ώZ�l >= �A���[�����x��					*/	/* <S154> */
	{
		ol_ptr->var.SumImid = Sumx;
		ALMSetServoAlarm( AlmMngr, ALM_OLF2 );		/* A.720 : �ߕ���(�A���ő啉��)				*/
	}
//	if( Sumx >= ol_ptr->conf.WrnLvlmid )			/* �ώZ�l >= ���[�j���O���x��				*/	/* <S154> */
	if( Sumx >= CheckOLP->WrnLvlmid )				/* �ώZ�l >= ���[�j���O���x��				*/	/* <S154> */
	{
		ol_ptr->var.SumImid = Sumx;
		ol_ptr->var.WrnInfo |= 0x02;				/* ���[�j���O���ݒ�						*/
	}
	else if( Sumx >= 0 )							/* �ώZ�l >= 0								*/
	{
		ol_ptr->var.SumImid = Sumx;
	}
	else											/* �ώZ�l < 0								*/
	{
		ol_ptr->var.SumImid = 0;
	}
}



/****************************************************************************************************/
/*																									*/
/*		�c�a�ߕ��׃`�F�b�N����																		*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �c�a�d�͐ώZ�l���v�Z���A�ߕ��׃��x�����z�����ꍇ�A�c�a�ߕ��׃A���[���Ƃ���B			*/
/*			�܂��A���ςc�a�d�͒l���v�Z����B														*/
/*			���x�㏸�ɂ��c�a�ߕ��ׂ����o����B													*/
/*																									*/
/*																									*/
/****************************************************************************************************/
void	AdetCheckDbrakeOverLoad( ALMMAN_HANDLE *AlmMngr, CHECK_DBOL *DbOL, BOOL DbOn, LONG AbsMotSpd )
{
	LONG	x;
	LONG	DbPower;

/*--------------------------------------------------------------------------------------------------*/
/*		�c�a�d�͂̌v�Z																				*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*					 PowerMax * Speed * Speed														*/
/*		DbPower  = --------------------------- = Kdbpower * (Speed>>9) * (Speed>>9)					*/
/*					   SpdLevel * SpdLevel															*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
	if( DbOn )
	{
		if( AbsMotSpd >= DbOL->conf.DbolSpdLevel )
		{
			DbPower = DbOL->conf.DbolPowMax;
		}
		else
		{
			x = (AbsMotSpd >> 9);
#if (FLOAT_USE==TRUE)
			DbPower = x * x * DbOL->conf.Kdbpower;
#else
			DbPower = MlibMulgainSx48( (x * x), DbOL->conf.Kdbpower );
#endif /* FLOAT_USE */
		}
	}
	else
	{
		DbPower = 0;
		DbOL->var.DbHeat = 0;							/* ���x�㏸�ɂ��DB�ߕ��אώZ�l�N���A	*/
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�c�a�d�͐ώZ�l�̌v�Z																		*/
/*--------------------------------------------------------------------------------------------------*/
	DbOL->var.DbolPowSum += (DbPower - DbOL->conf.DbolPowBase );
	/*----------------------------------------------------------------------------------------------*/
	/*	DB�d�͐ώZ�l�I�[�o�[�t���[�΍�																*/
	/*	��DB��ԂŊO�͂ɂ���ĉ񂳂ꑱ�����DB�d�͐ώZ�l���I�[�o�[�t���[����̂Ń��~�b�g������ǉ�	*/
	/*		���݂̃��[�^�萔����P�T���v���ŐώZ�����ő�l(DbOL->conf.DbolPowMax)�́A				*/
	/*		�����̌���18bit�����ł��邽�߁A���~�b�g���x����2^30�Ƃ���B								*/
	/*----------------------------------------------------------------------------------------------*/
	if( DbOL->var.DbolPowSum > 0x40000000)
	{
		DbOL->var.DbolPowSum = 0x40000000;
	}
	if( DbOL->var.DbolPowSum < 0 )
	{
		DbOL->var.DbolPowSum = 0;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�c�a�ߕ��ׂ̃`�F�b�N																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( DbOL->var.DbolPowSum >= DbOL->conf.DbolAlmLevel )
	{
		/* A.730 : �c�a�ߕ��� */
		ALMSetServoAlarm( AlmMngr, ALM_DBOL );
	}

/*--------------------------------------------------------------------------------------------------*/
/*		Un00B : �c�a��R����d�͗p�f�[�^(10sec�c�a����d�͕��ϒl)	CoutV.UnMeanDbPower				*/
/*		Un144 : �c�a��R����d�͗p�f�[�^(2ms�c�a����d�͒l)			CoutV.UnDBPower					*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	DbOL->var.UnDbPower = DbPower * DbOL->conf.KundbpowerCycle;
#else
	DbOL->var.UnDbPower = MlibMulgain( DbPower, DbOL->conf.KundbpowerCycle );
#endif /* FLOAT_USE */
	DbOL->var.DbPowerSum += DbPower;

	DbOL->var.DbMeanCycle++;
	if( DbOL->var.DbMeanCycle >= KPI_SCANC_MS( 10000 ) )
	{
#if (FLOAT_USE==TRUE)
		DbOL->var.UnMeanDbPower = DbOL->var.DbPowerSum * DbOL->conf.Kundbpower;
#else
		DbOL->var.UnMeanDbPower = MlibMulgain( DbOL->var.DbPowerSum, DbOL->conf.Kundbpower );
#endif /* FLOAT_USE */
		DbOL->var.DbPowerSum  = 0;
		DbOL->var.DbMeanCycle = 0;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		���x�㏸�ɂ��c�a�ߕ��׃��[�j���O�E�A���[�����o����										*/
/*--------------------------------------------------------------------------------------------------*/
	if ( DbOL->conf.DbOHeatCheck )
	{
		DbOL->var.DbHeat += (DbPower - DbOL->conf.DbolPowBase );
		if ( DbOL->var.DbHeat >= DbOL->conf.DbOHeatAlmLvl )
		{
			/* A.921 : ���x�㏸�ɂ��c�a�ߕ��׃��[�j���O�N���A */
			ALMClearWarning( AlmMngr, WRN_DBOL_LVL );
			/* ���[�j���O���o���t���O�N���A */
			DbOL->var.DbOHeatWarn = FALSE;
			/* A.731 : ���x�㏸�ɂ��c�a�ߕ��׃Z�b�g */
			ALMSetServoAlarm( AlmMngr, ALM_DBOL_LVL );
		}
		else if ( (DbOL->var.DbHeat >= DbOL->conf.DbOHeatWrnLvl)
				&& (DbOL->var.DbOHeatWarn == FALSE) )
		{
			/* A.921 : ���x�㏸�ɂ��c�a�ߕ��׃��[�j���O�Z�b�g */
			ALMSetServoAlarm( AlmMngr, WRN_DBOL_LVL );
			/* ���[�j���O���o���t���O�Z�b�g */
			DbOL->var.DbOHeatWarn = TRUE;
		}
	}

	if ( (DbOL->var.DbolPowSum == 0) && (DbOL->var.DbOHeatWarn == TRUE) )
	{
		/* A.921 : ���x�㏸�ɂ��c�a�ߕ��׃��[�j���O�N���A */
		ALMClearWarning( AlmMngr, WRN_DBOL_LVL );
		/* ���[�j���O���o���t���O�N���A */
		DbOL->var.DbOHeatWarn = FALSE;
		/* �ώZ�l�N���A */
		DbOL->var.DbHeat = 0;
	}

}

/****************************************************************************************************/
/*																									*/
/*		���x���b�v���⏞���s��v���[�j���O�`�F�b�N����											*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : EEPROM�ɕۑ������O��ڑ����̃��[�^�̃V���A���ԍ����A����ڑ��������[�^�̃V���A���ԍ���	*/
/*          ��v���Ȃ��ꍇ�ɁA���x���b�v���⏞���s��v���[�j���O�����o����						*/
/*																									*/
/*																									*/
/****************************************************************************************************//*<S1FD>*/
void	AdetCheckRippleWrn( ALMMAN_HANDLE *AlmMngr, BOOL RippleWrnState )
{
	/* ���x���b�v���⏞���s��v���[�j���O��Ԏ擾 */
	if( RippleWrnState == FALSE )
	{
		ALMSetServoAlarm( AlmMngr, WRN_RIPPLE );	/* A.942 : ���x���b�v���⏞���s��v���o */
	}
	else
	{
		ALMClearWarning( AlmMngr, WRN_RIPPLE );		/* A.942 : ���x���b�v���⏞���s��v���� */
	}
}

#if	0	/*<S1F6>*/
/****************************************************************************************************/
/*																									*/
/*		���͐M���^�C�~���O�ُ�`�F�b�N����															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : BBON1LT/BBON2LT�M�����A�N�e�B�u�ɂȂ鎞�ԍ����p�����[�^�ݒ�l�ȏ�̎��ԍ��̏ꍇ��		*/
/*			�A���[���Ƃ���B����́A�Q�d���M���ُ̈�i�n�[�h�E�F�A�̌̏�A���͐M���̒f���j��		*/
/*			���o���邽�߂̏��u�ł���B�{�����ł́A�Z�[�t�e�B�I�v�V�����J�[�h�̗L���ɂ��			*/
/*			�A���[���R�[�h�i���ޗL/��=A.Eb2/A.Eb1�j����уA���[�����o�ɗp����ݒ�l					*/
/*			�i�J�[�h�L/��= PnE4C/Pn620�j��ύX����B												*/
/*																									*/
/****************************************************************************************************/
BOOL	AdetCheckBbonSignal( CHECK_HWBB_TIMING *pHwbbTmg, BOOL LackOneBbon )
{
	BOOL err;

	err = FALSE;
	/*--------------------------------------------------------------------------------------------------*/
	/*		BBON�M�����̓^�C�~���O�ُ�`�F�b�N����														*/
	/*--------------------------------------------------------------------------------------------------*/
		if( LackOneBbon )
		{
			pHwbbTmg->var.BbonTimeDiffCnt += KPI_SCCYCLEMS;
			if( pHwbbTmg->var.BbonTimeDiffCnt > pHwbbTmg->conf.MaxBbonTimeDiff )
			{
				/* �����ݒ�l�ŃN�����v */
				pHwbbTmg->var.BbonTimeDiffCnt = pHwbbTmg->conf.MaxBbonTimeDiff;

				/* A.Eb1 : ��~�M�����͎��ԍ��ߑ�(�J�[�h��) */
				err = TRUE;
			}
		}
		else		/* <S0E5> */
		{
			pHwbbTmg->var.BbonTimeDiffCnt = 0;
		}
		return err;
}
#endif	/*<S1F6>*/

#if 0	/*<S1F5>*/
/****************************************************************************************************/
/*																									*/
/*		�Z�[�t�e�B�@�\�@�����M���ُ팟�o���� (Test Pulse Diagnostic)					<S0F1>		*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �A���v���̓����M��(DPIN1/DPIN2�M��)���p���X�M���łȂ��ꍇ�ُ͈팟�o����					*/
/*			HWBB��Ԃ̏ꍇ�ُ͈팟�o���Ȃ�															*/
/*																									*/
/****************************************************************************************************/
BOOL	AdetTestPulseDiagnostic( BOOL DpInState, BOOL DpInLastState, BOOL CheckEnable, BOOL Bbon )	/*<S1DC>*/
{
	BOOL err;

	err = FALSE;

#if((PRODUCT_SERIES_SEVEN  ==  TRUE) && ( CSW_BOARD_TYPE == SGD7S_TYPE ) ) || ( CSW_HW_HWBB_SUPPORT == TRUE)		/* SGD7W��A.EC8,A.EC9�댟�o���C��<S105> */
/*--------------------------------------------------------------------------------------------------*/
/*		DPIN�M���`�F�b�N�����iBBON=OFF�FHWBB��Ԃ̓`�F�b�N���Ȃ��j									*/
/*--------------------------------------------------------------------------------------------------*/
	if( CheckEnable )
	{
		/*------------------------------------------------------------------------------------------*/
		/*	DPIN�M����4ms�����̃p���X�M���łȂ��ꍇ�ُ͈�											*/
		/*------------------------------------------------------------------------------------------*/
		if( !(DpInState^DpInLastState) )
		{
			err = TRUE;
		}
	}
/* <S1DC> >>>>> */
	else if( Bbon )
	{
		/*------------------------------------------------------------------------------------------*/
		/*	BBON=OFF(HIGH)�̂Ƃ��ADPIN�M����HIGH�łȂ��ꍇ�ُ͈�										*/
		/*------------------------------------------------------------------------------------------*/
		if( DpInState )
		{
			err = TRUE;
		}
	}
/* <<<<< <S1DC> */
#endif

	return err;
}
#endif	/*<S1F5>*/

#if 0
/****************************************************************************************************/
/*																									*/
/*		�Q�[�g�h���C�u�ُ�`�F�b�N����																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �n�[�h�I��HWBB���o��H�����݂��A���A�\�t�g�I��HWBB�@�\�𖳌��Ƃ���@��ɂ����āA		*/
/*			HWBB��Ԃ����o�����ꍇ�ɃA���[���Ƃ���B�m�C�Y����BBON�M�����A�N�e�B�u�ɂȂ�n�[�h		*/
/*			���C���[�h�x�[�X�u���b�N��ԂɂȂ����ꍇ���l���������̂ŁA�\�t�g�E�F�A�ł�HWBB���o		*/
/*			�����͑��点�Ă����AHWBB��Ԃ����o�����ۂɂ́wHBB�x�\���͂����ɃQ�[�g�h���C�u�ُ�		*/
/*			�A���[���Ƃ���B�{���̓n�[�h�E�F�A�ɂ�HWBB��Ԃɂ͂Ȃ蓾�Ȃ����u������Ă��邪		*/
/*			�\�t�g�E�F�A�I�ɂ��C���^���b�N�͂Ƃ��Ă����B											*/
/*																									*/
/****************************************************************************************************/
BOOL	AdetCheckGateDriveDefect( DET_HWBB_STS *pDetHwbbSts )
{
/*--------------------------------------------------------------------------------------------------*/
/*		�Q�[�g�h���C�u�ُ�`�F�b�N����																*/
/*--------------------------------------------------------------------------------------------------*/
	return pDetHwbbSts->GateDrvDefect;
}
#endif



/****************************************************************************************************/
/*																									*/
/*		���[�^�\���`�F�b�N����																		*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : ���[�^��(�t,�u,�v��)�̌�z���ɂ��\���̌��o���s���B									*/
/*			�A���[���̌��o�͓d���������̍ŏ��̃��[�^�̓����n�߂���������B							*/
/*																									*/
/*			���o�����́A																			*/
/*			����]�`�̏ꍇ��																		*/
/*				�\���A���[���}�X�N���Ă��Ȃ�														*/
/*			  ���d��������T�[�{�I�����ă��[�^�������n�߂Ă���0.5msec�ȓ�							*/
/*			  �����xFB�ƃg���N�w�߂��t�ɐ�															*/
/*			  ���O��g���N�w�߂ƍ���g���N�w�߂����ɐ�												*/
/*			  ���g���N�w�߁��\�����o�g���N															*/
/*			  �����xFB���\�����o���x																*/
/*			  ���g���N�w�߂�0�łȂ�																	*/
/*			  �����񑬓x���O�񑬓x(������)															*/
/*			  �����񑬓x���ߋ��ɔ��������ő呬�x													*/
/*			�̏�Ԃ��A20ms�A�������ꍇ�ɃA���[���Ƃ���B											*/
/*																									*/
/*			�����j�A�̏ꍇ��																		*/
/*				���xFB�ƃg���N�w�߂��t�ɐ�															*/
/*			  ���O��g���N�w�߂ƍ���g���N�w�߂����ɐ�												*/
/*			  ���g���N�w�߁��\�����o�g���N															*/
/*			  �����xFB���\�����o���x																*/
/*			  ���g���N�w�߂�0�łȂ�																	*/
/*			  �����񑬓x���O�񑬓x(������)															*/
/*			  �����񑬓x���ߋ��ɔ��������ő呬�x													*/
/*			�̏�Ԃ��A14ms�A�������ꍇ�ɃA���[���Ƃ���B											*/
/*																									*/
/****************************************************************************************************/
void	AdetCheckMotorOverrun( ALMMAN_HANDLE *AlmMngr, CHECK_OVRRUN *OvrRun,
								 LONG MotSpd, LONG TrqRefMon, LONG BaseEnableSts, BOOL MotTypeLinear )
{
	LONG	Condition0;
	LONG	Condition1;
	LONG	DetectTime;
	LONG	AbsTrqRefMon;
	LONG	AbsMotSpd;

	AbsTrqRefMon = MlibLABS( TrqRefMon );
	AbsMotSpd = MlibLABS( MotSpd );

	if(MotTypeLinear != FALSE)
	{ /* ���j�A�^ */
		Condition0 = TRUE;
		Condition1 = FALSE;
		DetectTime = KPI_SCANC_MS( 14 );
		OvrRun->OvrPeakMotSpd = 0;				/* �ő呬�x�펞�N���A */
	}
	else
	{ /* ��]�^ */
		/* BaseEnable && �d����������0.5sec */
		if( (BaseEnableSts == 0) && (OvrRun->OvrChkCnt < KPI_SCANC_MS( 500 )) )
		{
			Condition0 = TRUE;
		}
		else
		{
			Condition0 = FALSE;
		}
		/* ���x �� OS���x��1/10? */
		if( (OvrRun->OvrChkCnt != 0) || (AbsMotSpd >= MOTSPD_OS_10) )
		{
			Condition1 = TRUE;
		}
		else
		{
			Condition1 = FALSE;
		}
		DetectTime = KPI_SCANC_MS( 20 );
	}

	if( Condition0 )
	{
		if( Condition1 )
		{
			OvrRun->OvrChkCnt++;
		}
	/*----------------------------------------------------------------------------------------------*/
		if( ((MotSpd ^ TrqRefMon) < 0)						/* ���xFB�ƃg���N�w�߂̋ɐ��͋t?	*/
			&& ((TrqRefMon ^ OvrRun->OvrTrqRef) >= 0)		/* �O��g���N�ƍ���g���N�͓��ɐ�?	*/
			&& (AbsTrqRefMon >= OvrRun->OvrTrqLevel)		/* �g���N�w�� �� �\�����o�g���N?	*/
			&& (AbsMotSpd >= OvrRun->OvrSpdLevel)			/* ���xFB     �� �\�����o���x?		*/
			&& (TrqRefMon != 0) )							/* �g���N�w�߂�0�łȂ�?				*/
		{
			if( AbsMotSpd >= OvrRun->OvrAbsMotSpd )
			{ /* ���񑬓x �� �O�񑬓x? */
				if( AbsMotSpd >= OvrRun->OvrPeakMotSpd )
				{ /* ���񑬓x �� �ߋ��̍ő呬�x? */
					if( OvrRun->OvrAlmCnt >= DetectTime )
					{ /* ���o���Ԍo��? */
						/* A.C10 : �\���h�~���o */
						ALMSetServoAlarm( AlmMngr, ALM_RWF );
					}
					else
					{
						/* �\�����o�J�E���^�̃J�E���g�A�b�v */
						OvrRun->OvrAlmCnt++;
					}
				}
				else
				{
					/* �\�����o�J�E���^�̃N���A */
					OvrRun->OvrAlmCnt = 0;
				}
			}
			else
			{
				if( OvrRun->OvrAbsMotSpd > OvrRun->OvrPeakMotSpd )
				{ /* �O�񑬓x �� �ő呬�x? */
					/* �ő呬�x�̍X�V */
					OvrRun->OvrPeakMotSpd = OvrRun->OvrAbsMotSpd;
				}
				/* �\�����o�J�E���^�̃N���A */
				OvrRun->OvrAlmCnt = 0;
			}
		}
		else
		{
			/* �\�����o�J�E���^�̃N���A */
			OvrRun->OvrAlmCnt = 0;
		}
	}

	/* �O��g���N�̍X�V */
	OvrRun->OvrTrqRef = TrqRefMon;
	/* �O�񃂁[�^���x�̍X�V */
	OvrRun->OvrAbsMotSpd = AbsMotSpd;
}



/****************************************************************************************************/
/*																									*/
/*		�T�[�{�I�����ʒu�΍��ߑ�`�F�b�N����														*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �ʒu���䃂�[�h���̃T�[�{�I�����̈ʒu�΍����Ď�����B									*/
/*																									*/
/*			1) �ʒu�΍����ʒu�΍��ߑ�A���[�����x���𒴂��Ă���ꍇ�A�A���[���Ƃ���B				*/
/*			2) �ʒu�΍����ʒu�΍��ߑ像�[�j���O���x���𒴂��Ă���ꍇ�A���[�j���O�Ƃ���B			*/
/*																									*/
/****************************************************************************************************/
void	AdetCheckSvonPerrOverFlow( ALMMAN_HANDLE *AlmMngr, CHECK_OVRPOSERR *OvrPosErr, LONG PositionError )
{
	LONG	AbsPosError;

	/* ���[�J���ϐ��̏����� */
	AbsPosError = MlibLABS( PositionError );

#if 0
/*--------------------------------------------------------------------------------------------------*/
/*		���䃂�[�h�`�F�b�N																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( pSeqCtrl->CtrlMode.b.cm != CTRLMODE_POS )
	{
		ALMClearWarning( AlmMngr, WRN_BEOF );			/* A.901 : �T�[�{�I�����ʒu�΍��ߑ�			*/
		return;
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�x�[�X�C�l�[�u���v���`�F�b�N																*/
/*--------------------------------------------------------------------------------------------------*/
	if( (pSeqCtrl->DbBeReq == FALSE) || (pSeqCtrl->BeReqSeqOut == TRUE) )
	{ /* �x�[�X�C�l�[�u���v���Ȃ� || �T�[�{�I������ */
		ALMClearWarning( AlmMngr, WRN_BEOF );			/* A.901 : �T�[�{�I�����ʒu�΍��ߑ�			*/
		return;
	}
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu�΍��`�F�b�N(�x�[�X�C�l�[�u���v������ && �T�[�{�I�������̏ꍇ)							*/
/*--------------------------------------------------------------------------------------------------*/
	if( AbsPosError >= OvrPosErr->SvonPerAlmLevel )
	{
		ALMSetServoAlarm( AlmMngr, ALM_BEOF );			/* A.D01 : �T�[�{�I�����ʒu�΍��ߑ�			*/
	}
	/*----------------------------------------------------------------------------------------------*/
	if( AbsPosError >= OvrPosErr->SvonPerWrnLevel )
	{
		ALMSetServoAlarm( AlmMngr, WRN_BEOF );			/* A.901 : �T�[�{�I�����ʒu�΍��ߑ�			*/
	}
	else
	{
		ALMClearWarning( AlmMngr, WRN_BEOF );			/* A.901 : �T�[�{�I�����ʒu�΍��ߑ�			*/
	}
}



/****************************************************************************************************/
/*																									*/
/*		�ʒu�΍��ߑ�`�F�b�N����																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �ʒu���䃂�[�h���̈ʒu�΍����Ď�����B													*/
/*																									*/
/*			1) �ʒu�΍����ʒu�΍��ߑ�A���[�����x���𒴂��Ă���ꍇ�A�A���[���Ƃ���B				*/
/*			2) �ʒu�΍����ʒu�΍��ߑ像�[�j���O���x���𒴂��Ă���ꍇ�A���[�j���O�Ƃ���B			*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
void	AdetCheckPerrOverFlow( ALMMAN_HANDLE *AlmMngr, CHECK_OVRPOSERR *OvrPosErr,
										BOOL PosCtrlMode, LONG PositionError, BOOL SvonSpdLmt )
{
	LONG	AbsPosError;

	/* ���[�J���ϐ��̏����� */
	AbsPosError = MlibLABS( PositionError );

/*--------------------------------------------------------------------------------------------------*/
/*		���䃂�[�h�`�F�b�N																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( FALSE == PosCtrlMode )
	{
		/* A.900 : �ʒu�΍��ߑ像�[�j���O */
		ALMClearWarning( AlmMngr, WRN_OF );
		return;
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu�΍��`�F�b�N																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( AbsPosError >= OvrPosErr->PerAlmLevel )
	{
		if( SvonSpdLmt )
		{
			/* A.D02 : �T�[�{�I�������x�����ʒu�΍��ߑ� */
			ALMSetServoAlarm( AlmMngr, ALM_BEVLMTOF );
		}
		else
		{
			/* A.D00 : �ʒu�΍��ߑ� */
			ALMSetServoAlarm( AlmMngr, ALM_OF );
		}
	}
	/*----------------------------------------------------------------------------------------------*/
	if( AbsPosError >= OvrPosErr->PerWrnLevel )
	{
		/* A.900 : �ʒu�΍��ߑ像�[�j���O */
		ALMSetServoAlarm( AlmMngr, WRN_OF );
	}
	else
	{
		/* A.900 : �ʒu�΍��ߑ像�[�j���O */
		ALMClearWarning( AlmMngr, WRN_OF );
	}
}

#if	(CSW_SVCF_FULC_MODE != CSW_FULC_NO_SUPPORT)
/****************************************************************************************************/
/*																									*/
/*		���[�^���׈ʒu�ԕ΍��ߑ�`�F�b�N����														*/
/*																									*/
/****************************************************************************************************//* <S04D> */
static BOOL LpxCheckSemiFullPerrOverFlow(CHECK_FENCPOSERR *ChkFposErr,
                                         	   BOOL PosCtrlMode, LONG MencPos, LONG FencPos)
{
	LONG	err_d10;
	LONG	wk;
	LONG	dMotPos, dFencPos;
	LONG	MtPGSubUnit, ExtPGSubUnit;

	err_d10 = FALSE;

	dMotPos = MencPos - ChkFposErr->MencPosLast;
	dFencPos = FencPos - ChkFposErr->FencPosLast;
	ChkFposErr->MencPosLast = MencPos;
	ChkFposErr->FencPosLast = FencPos;

	if( FALSE == PosCtrlMode )
	{/* �ʒu���䃂�[�h�ȊO�̎�	*/
		MlibResetLongMemory( &ChkFposErr->sfperra, sizeof(ChkFposErr->sfperra)/4 );
		KlibRstLongTimer(&ChkFposErr->FSOFAlmCnt);
		return err_d10;
	}

	MtPGSubUnit = MlibEgearRvscnv( dMotPos, &ChkFposErr->Egear, &ChkFposErr->MtPGRem );		/*  ���ʒu	[�w�ߒP��]	*/
	ExtPGSubUnit = MlibEgearRvscnv( dFencPos, &ChkFposErr->Egear, &ChkFposErr->ExtPGRem );	/*  ���ʒu	[�w�ߒP��]	*/
	MlibPerrcala( ExtPGSubUnit, MtPGSubUnit, &ChkFposErr->sfgear, &ChkFposErr->sfperra );

	if ( MlibAbs32( FencPos- ChkFposErr->FencPosOneTurn ) >= ChkFposErr->fcpgpls )
	{
		ChkFposErr->FencPosOneTurn = FencPos;

		/* ���[�^1��]���͕΍���������(Pn52A)�����炷	*/
		if ( ChkFposErr->fpgcutrate == 100 )
		{
			MlibResetLongMemory( &ChkFposErr->sfperra, sizeof(ChkFposErr->sfperra)/4 );	/* �ʒu�Ǘ��p�ϐ�������	*/
		}
		else
		{
#if (FLOAT_USE==TRUE)
			wk = ChkFposErr->sfperra.per32s * ChkFposErr->fpgcutratelv;
#else
			wk = MlibMulgain32( ChkFposErr->sfperra.per32s, ChkFposErr->fpgcutratelv );
#endif /* FLOAT_USE */
			MlibPerrcala( -wk, 0, &ChkFposErr->sfgear, &ChkFposErr->sfperra );
		}
	}

	if ( ChkFposErr->fpgoverLevel != 0 )
	{/* �t���N���[�Y�h���� && Pn51C:Pn51B != 0	*/
		if ( (ChkFposErr->sfperra.per64[1] > 0) ||
			 (ChkFposErr->sfperra.per64[1] < -1) ||
			 (MlibAbs32(ChkFposErr->sfperra.per64[0]) >= (LONG)ChkFposErr->fpgoverLevel) )
		{
			/* 20ms �p���ŃA���[��	*/
			if ( KlibGetLongTimerMs(ChkFposErr->FSOFAlmCnt) >= 20 )
			{	/* A.D10 : ���[�^�|���׈ʒu�ԕ΍��ߑ�		*/
//				KPI_ALARMSET( ALM_FSOF );
				err_d10 = TRUE;
			}
		}
		else
		{
			KlibRstLongTimer(&ChkFposErr->FSOFAlmCnt);
		}
	}
	else
	{
		KlibRstLongTimer(&ChkFposErr->FSOFAlmCnt);
	}

	return err_d10;
}

/****************************************************************************************************/
/*																									*/
/*		���[�^���׈ʒu�ԕ΍��ߑ�`�F�b�N����														*/
/*																									*/
/****************************************************************************************************//* <S04D> */
void AdetCheckFullcEncoderAlarm(CHECK_FENCPOSERR *ChkFposErr, MENCV *FencV,
                            	BOOL PosCtrlMode, LONG MencPos, LONG FencPos, ALMMAN_HANDLE *AlmMngr )
{
	UCHAR AlmCode;

	/*----------------------------------------------------------------------*/
	/*		�A���[���R�[�h�`�F�b�N�iBIT 0�`3�j								*/
	/*----------------------------------------------------------------------*/
	AlmCode = FencGetFencAlarm(FencV);
	switch(AlmCode & 0x0F)
	{
	case 0x01:
		if(FencV->SerialConvType == SCONVTYPE_ENDAT_ABS)
		{	/* A.8A0 : �e�o�f�X�P�[���ُ� */
			ALMSetServoAlarm(AlmMngr, ALM_FPGFSCALE);
		}
		break;
	case 0x02:
		/* A.8A1 : �e�o�f���W���[���ُ�	*/
		ALMSetServoAlarm(AlmMngr, ALM_FPGMODULE);
		break;
	case 0x03:
		if(FencV->SerialConvType == SCONVTYPE_ENDAT_ABS)
		{	/* A.8A0 : �e�o�f�X�P�[���ُ� */
			ALMSetServoAlarm(AlmMngr, ALM_FPGFSCALE);
		}
		/* A.8A1 : �e�o�f���W���[���ُ�	*/
		ALMSetServoAlarm(AlmMngr, ALM_FPGMODULE);
		break;
	case 0x08:
		if(FencV->AbsoEncoder != FALSE)
		{	/* A.8A3 : �e�o�f�|�W�V�����ُ�(�A�u�\) */
			ALMSetServoAlarm(AlmMngr, ALM_FPGPOS);
		}
		else
		{	/* A.8A2 : �e�o�f�Z���T�ُ�(�C���N��) */
			ALMSetServoAlarm(AlmMngr, ALM_FPGSEN);
		}
		break;
	case 0x09:
		if(FencV->AbsoEncoder != FALSE)
		{
			if(FencV->SerialConvType == SCONVTYPE_ENDAT_ABS)
			{	/* A.8A0 : �e�o�f�X�P�[���ُ� */
				ALMSetServoAlarm(AlmMngr, ALM_FPGFSCALE);
			}
			/* A.8A3 : �e�o�f�|�W�V�����ُ�(�A�u�\) */
			ALMSetServoAlarm(AlmMngr, ALM_FPGPOS);
		}
		else
		{	 /* A.8A2 : �e�o�f�Z���T�ُ�(�C���N��) */
			ALMSetServoAlarm(AlmMngr, ALM_FPGSEN);
		}
		break;
	case 0x0A:
		/* A.8A1 : �e�o�f���W���[���ُ� */
		ALMSetServoAlarm(AlmMngr, ALM_FPGMODULE);
		if(FencV->AbsoEncoder != FALSE)
		{	/* A.8A3 : �e�o�f�|�W�V�����ُ�(�A�u�\) */
			ALMSetServoAlarm(AlmMngr, ALM_FPGPOS);
		}
		else
		{	/* A.8A2 : �e�o�f�Z���T�ُ�(�C���N��) */
			ALMSetServoAlarm(AlmMngr, ALM_FPGSEN);
		}
		break;
	case 0x0B:
		/* A.8A1 : �e�o�f���W���[���ُ�	*/
		ALMSetServoAlarm(AlmMngr, ALM_FPGMODULE);
		if(FencV->AbsoEncoder != FALSE)
		{
			if(FencV->SerialConvType == SCONVTYPE_ENDAT_ABS)
			{	/* A.8A0 : �e�o�f�X�P�[���ُ� */
				ALMSetServoAlarm(AlmMngr, ALM_FPGFSCALE);
			}
			/* A.8A3 : �e�o�f�|�W�V�����ُ�(�A�u�\)	*/
			ALMSetServoAlarm(AlmMngr, ALM_FPGPOS);
		}
		else
		{	/* A.8A2 : �e�o�f�Z���T�ُ�(�C���N��) */
			ALMSetServoAlarm(AlmMngr, ALM_FPGSEN);
		}
		break;
	default:
		break; /* �A���[���Ȃ�	*/
	}

	/*----------------------------------------------------------------------*/
	/*		�A���[���R�[�h�`�F�b�N�iBIT 4�`7�j								*/
	/*----------------------------------------------------------------------*/
	if(0 != (AlmCode & 0x10))
	{	/* A.8A5 : �e�o�f���޽�߰�ވُ� */
		ALMSetServoAlarm(AlmMngr, ALM_FPGOVSPD);
	}
	if(0 != (AlmCode & 0x20))
	{	/* A.8A6 : �e�o�f����˰Ĉُ� */
		ALMSetServoAlarm(AlmMngr, ALM_FPGOVHEAT);
	}

	/*----------------------------------------------------------------------*/
	/*		���[�^���׈ʒu�ԕ΍��ߑ�`�F�b�N								*/
	/*----------------------------------------------------------------------*/
	if(FALSE != LpxCheckSemiFullPerrOverFlow(ChkFposErr, PosCtrlMode, MencPos, FencPos))
	{	/* A.D10 : ���[�^�|���׈ʒu�ԕ΍��ߑ� */
		ALMSetServoAlarm(AlmMngr, ALM_FSOF);
	}

}


#endif	// (CSW_SVCF_FULC_MODE != CSW_FULC_NO_SUPPORT)



/***************************************** end of file **********************************************/
