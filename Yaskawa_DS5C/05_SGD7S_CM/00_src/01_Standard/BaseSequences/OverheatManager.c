/****************************************************************************************************/
/*																									*/
/*																									*/
/*		OverheatManager.c : ���x�ߔM�Ď�������`					<S0ED>							*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �擾�������x���Ď����A���x�ߔM�A���[��/���[�j���O���o�͂���B							*/
/*																									*/
/*			�܂��AUn�\�����鉷�x�f�[�^�̍쐬��A�ݒu�����j�^�l���v�Z���AAPI�ɂď��񋟂�			*/
/*			�s���B																					*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	����	2014.1.22	Y.Oka	For Mercury												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include	"OverheatManager.h"
#include	"Mlib.h"



/****************************************************************************************************/
/*		API's																						*/
/****************************************************************************************************/
/*	void	OhmInitialize( CHECK_OH* pCheckOh );													*/
/*	void	EnvMonInitialize( ENVMON* pEnvMon );													*/
/*	void	OhmCheckOverHeatAlm( ALMMAN_HANDLE *AlmMngr, CHECK_OH *CheckOh,							*/
/*														SHORT CtrlBoadTemp, SHORT PowerBoadTemp );	*/
/*	void	AdetCheckTempDeviceErr( ALMMAN_HANDLE *AlmMngr, USHORT CtrlBoadErrCnt,					*/
/*																		USHORT PowerBoadErrCnt );	*/
/*	void	AdetInstEnvMonitor( ENVMON *EnvMon, SHORT AmpTemp, SHORT MotTemp );						*/
/****************************************************************************************************/



/****************************************************************************************************/
/*		Static Function Definition																	*/
/****************************************************************************************************/
static LONG	AdetCheckFirstBoardTmpr( CHECK_INTTMPR *IntTmprAlm, SHORT Temperature );
static LONG	AdetCheckBoardTmpr( CHECK_INTTMPR *IntTmprAlm, SHORT Temperature );
static void	AdetSetBoardTmprAlm( ALMMAN_HANDLE *AlmMngr, LONG DetectAlm, LONG BoardSel );
/****************************************************************************************************/



/****************************************************************************************************/
/*																									*/
/*		�I�[�o�q�[�g�Ď����W���[��������										<S03A>				*/
/*																									*/
/****************************************************************************************************/
void OhmInitialize( CHECK_OH* pCheckOh )
{
	MlibResetLongMemory( &pCheckOh->var, sizeof( pCheckOh->var )/4 );
	MlibResetLongMemory( &pCheckOh->CtrlTmprAlm.var, sizeof( pCheckOh->CtrlTmprAlm.var )/4 );
	MlibResetLongMemory( &pCheckOh->PwrTmprAlm.var, sizeof( pCheckOh->PwrTmprAlm.var )/4 );
	MlibResetLongMemory( &pCheckOh->var, sizeof( pCheckOh->var )/4 );
}



/****************************************************************************************************/
/*																									*/
/*		�ݒu�����j�^������													<S03A>				*/
/*																									*/
/****************************************************************************************************/
void EnvMonInitialize( ENVMON* pEnvMon )
{
	MlibResetLongMemory( &pEnvMon->var, sizeof( pEnvMon->var )/4 );

}



/****************************************************************************************************/
/*																									*/
/*		�T�[�{�p�b�N�I�[�o�q�[�g�Ď�											<S03A>				*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : ���x���oIC����̉��x�����Ď����A�j��E����s�ǂ�h���B								*/
/*																									*/
/*	����d�����������OH�A���[���N���A�\���x�Ƃ̔�r���s���A�����Ă���ꍇ�͑��A���[���Ƃ���B	*/
/*	������Ԃ�1�b�Ԍp�����A���̌�ʏ�̉��x�Ď��ֈڍs����B�ʏ�̉��x�Ď����̓��[�j���O���x����		*/
/*	�����ă��[�j���O�o�͂��s���A���̌�p�����Ďg�p�����ꍇ�ɂ̓A���[���ւƈڍs����B				*/
/*																									*/
/****************************************************************************************************/
void	OhmCheckOverHeatAlm( ALMMAN_HANDLE *AlmMngr, CHECK_OH *CheckOh,
														SHORT CtrlBoadTemp, SHORT PowerBoadTemp )
{
//	SHORT	MaxTemp;	/* <S050> */
	LONG	DetAlm;		/* <S0DE> */

	/*----------------------------------------------------------------------------------------------*/
	/*	���C���֘AUn���j�^�f�[�^�쐬											<S0DE>		 		*/
	/*----------------------------------------------------------------------------------------------*/
	/* ���x���o�l���Z */
	CheckOh->var.CtrlBoadTempSum += CtrlBoadTemp;
	CheckOh->var.PowerBoadTempSum += PowerBoadTemp;
	if( ++CheckOh->var.TemperatureSumCnt >= 10 )
	{
		/* Un170 : ���ϓ��C���x[0.01deg] */
		CheckOh->UnMon.UnAverageTemp =
				((CheckOh->var.CtrlBoadTempSum + CheckOh->var.PowerBoadTempSum) >> 1);
		/* Un171 : �������͉��x[0.1deg] */
		CheckOh->UnMon.UnCtrlBoadTemp = (SHORT)(CheckOh->var.CtrlBoadTempSum * 0.1f);
		/* Un172 : �p���[����͉��x[0.1deg] */
		CheckOh->UnMon.UnPowerBoadTemp = (SHORT)(CheckOh->var.PowerBoadTempSum * 0.1f);
		/* ������ */
		CheckOh->var.CtrlBoadTempSum = 0;
		CheckOh->var.PowerBoadTempSum = 0;
		CheckOh->var.TemperatureSumCnt = 0;
	}

	/*----------------------------------------------------------------------------------------------*/
	/*	�I�[�o�[�q�[�g�Ď�														<S0DE>		 		*/
	/*----------------------------------------------------------------------------------------------*/
	if( CheckOh->var.DetTempFirstPass == FALSE )
	{ /* ���x���o���� */
		/* �������x�ُ�A���[���`�F�b�N���� */
		DetAlm = AdetCheckFirstBoardTmpr( &(CheckOh->CtrlTmprAlm), CtrlBoadTemp );
		/* ���x�ُ�A���[���ݒ菈�� */
		AdetSetBoardTmprAlm( AlmMngr, DetAlm, CTRL_BOARD );

		/* �p���[����x�ُ�A���[���`�F�b�N���� */
		DetAlm = AdetCheckFirstBoardTmpr( &(CheckOh->PwrTmprAlm), PowerBoadTemp );
		/* ���x�ُ�A���[���ݒ菈�� */
		AdetSetBoardTmprAlm( AlmMngr, DetAlm, POWER_BOARD );

		/* �o�ߎ��ԃJ�E���g�A�b�v */
		CheckOh->var.DetTempFistPastTime++;
		if( CheckOh->var.DetTempFistPastTime > KPI_SCANC_MS(1000) )
		{
			/* �����Ď��ʉ߃t���O�Z�b�g */
			CheckOh->var.DetTempFirstPass = TRUE;
		}
	}
	else
	{ /* �ʏ펞 */
		/* �������x�ُ�A���[���`�F�b�N���� */
		DetAlm = AdetCheckBoardTmpr( &(CheckOh->CtrlTmprAlm), CtrlBoadTemp );
		/* ���x�ُ�A���[���ݒ菈�� */
		AdetSetBoardTmprAlm( AlmMngr, DetAlm, CTRL_BOARD );

		/* �p���[����x�ُ�A���[���`�F�b�N���� */
		DetAlm = AdetCheckBoardTmpr( &(CheckOh->PwrTmprAlm), PowerBoadTemp );
		/* ���x�ُ�A���[���ݒ菈�� */
		AdetSetBoardTmprAlm( AlmMngr, DetAlm, POWER_BOARD );
	}

	return;
}



/*****************************************************************************
  Description: ��������x�ُ�A���[���`�F�b�N���񏈗�				<S0DE>
------------------------------------------------------------------------------
  Parameters:
	IntTempAlm				�������x�ُ�
	Temperature				����x���
  Return:
	rc						�߂�l
------------------------------------------------------------------------------
  Note:
	��������x�ُ�A���[�����Ď�����
*****************************************************************************/
static LONG	AdetCheckFirstBoardTmpr( CHECK_INTTMPR *IntTmprAlm, SHORT Temperature )
{
	LONG	rc;
	rc = CHK_OVERHEAT_NO_OPERATION;

	if( Temperature > IntTmprAlm->conf.BrdAlmClrTmpr )
	{ /* ��������x���ُ�N���A�\���x�𒴂��Ă��� */
//		if( IntTmprAlm->var.DirectAlmDetTime > KPI_SCANC_MS( 500 ) ) /* <S1D1> */
		if( IntTmprAlm->var.AlmDetectTimer > KPI_SCANC_MS( 10000 ) ) /* <S1D1> */
		{
			IntTmprAlm->var.BrdState = BRD_ALARM_DURING;
			rc = CHK_OVERHEAT_SET_ALARM;
		}
		else
		{
//			IntTmprAlm->var.BrdState++;				/* <S109> */
			IntTmprAlm->var.AlmDetectTimer++;		/* <S109> */
		}
	}
	else
	{
//		IntTmprAlm->var.BrdState = 0;				/* <S109> */
		IntTmprAlm->var.AlmDetectTimer = 0;			/* <S109> */
	}
	return( rc );
}



/*****************************************************************************
  Description: ��������x�ُ�A���[���`�F�b�N����					<S0DE>
------------------------------------------------------------------------------
  Parameters:
	IntTempAlm				�������x�ُ�
	Temperature				����x���
  Return:
	rc						�߂�l
------------------------------------------------------------------------------
  Note:
	��������x�ُ�A���[�����Ď�����
*****************************************************************************/
static LONG	AdetCheckBoardTmpr( CHECK_INTTMPR *IntTmprAlm, SHORT Temperature )
{
	LONG	rc;
	rc = CHK_OVERHEAT_NO_OPERATION;

	switch ( IntTmprAlm->var.BrdState )
	{
	case BRD_TEMP_NORMAL:
		/* �������x���x�����x�𒴂����ꍇ */
		if( Temperature > IntTmprAlm->conf.BrdWrnTmpr )
		{
//			if( IntTmprAlm->var.WrnDetectTimer > KPI_SCANC_MS( 500 ) ) /* <S11D> */
			if( IntTmprAlm->var.WrnDetectTimer > KPI_SCANC_MS( 10000 ) )
			{
				rc = CHK_OVERHEAT_SET_WARNING;
//				IntTmprAlm->var.IntTmprTimeCnt = 0U;					/* <S1D1> */
				IntTmprAlm->var.BrdState = BRD_WRN_DURING;
			}
			else
			{
				IntTmprAlm->var.WrnDetectTimer++;
			}
		}
		else
		{
			IntTmprAlm->var.WrnDetectTimer = 0;
		}
		break;

	case BRD_WRN_DURING:
//		IntTmprAlm->var.IntTmprTimeCnt++; /* <S1D1> */
		/* ���[�j���O��Ԃ��K��̎��Ԃ𒴂����ꍇ�A�܂��ُ팟�o��臒l���x�𒴂����ꍇ�A�܂��A���[������ *//* <S1D1> */
//		if( (IntTmprAlm->var.IntTmprTimeCnt >= (IntTmprAlm->conf.BrdAlmDetTime * KPI_SCANC_MS( 1000 )))
//			|| (Temperature > IntTmprAlm->conf.BrdAlmTmpr) )
		if( (Temperature > IntTmprAlm->conf.BrdAlmTmpr) )
		{
			if( IntTmprAlm->var.AlmDetectTimer > KPI_SCANC_MS( 10000 ) )/* <S1D1> */
			{
				rc = CHK_OVERHEAT_SET_ALARM;
//				IntTmprAlm->var.IntTmprTimeCnt = 0U;					/* <S1D1> */
				IntTmprAlm->var.BrdState = BRD_ALARM_DURING;
			}
			else
			{
				IntTmprAlm->var.AlmDetectTimer++;
			}
		}
		/* �������x���x�����x����������ꍇ */
		else if( Temperature < IntTmprAlm->conf.BrdWrnTmpr )
		{
			rc = CHK_OVERHEAT_CLR_WARNING;
//			IntTmprAlm->var.IntTmprTimeCnt = 0U;						/* <S1D1> */
			IntTmprAlm->var.BrdState = BRD_TEMP_NORMAL;
		}
		else
		{
			rc = CHK_OVERHEAT_SET_WARNING;								/* <S1D1> */
			IntTmprAlm->var.AlmDetectTimer = 0;							/* <S1D1> */
		}
		break;

	case BRD_ALARM_DURING:
		/* ��������x���ُ�N���A�\���x�𒴂��Ă��� */
		if( Temperature > IntTmprAlm->conf.BrdAlmClrTmpr )
		{
			rc = CHK_OVERHEAT_SET_ALARM;
		}
		else
		{
			IntTmprAlm->var.BrdState = BRD_TEMP_NORMAL;
		}
		break;

	default:
		IntTmprAlm->var.BrdState = BRD_TEMP_NORMAL;
//		IntTmprAlm->var.IntTmprTimeCnt = 0U;							/* <S1D1> */
		break;

	}
	return( rc );
}



/*****************************************************************************
  Description: �������x�ُ�A���[���ݒ菈��							<S0DE>
------------------------------------------------------------------------------
  Parameters:
	AlmMngr					�A���[�����
	DetectAlm				�A���[���o�͔���
	BoardSel				��Z���N�g
  Return:

------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void	AdetSetBoardTmprAlm( ALMMAN_HANDLE *AlmMngr, LONG DetectAlm, LONG BoardSel )
{
	if( BoardSel == CTRL_BOARD )
	{
		if( DetectAlm != CHK_OVERHEAT_NO_OPERATION )
		{
			if( DetectAlm == CHK_OVERHEAT_SET_ALARM )
			{
				ALMClearGlobalAlarm( WRN_CTRLBRD );			/* ���[�j���O(A.912)���� *//* <S11D> */
				ALMSetGlobalAlarm( ALM_CTRLBRD );			/* �A���[��(A.7A1)���o�͂��� *//* <S11D> */
			}
			else if( DetectAlm == CHK_OVERHEAT_SET_WARNING )
			{
				ALMSetGlobalAlarm( WRN_CTRLBRD );			/* ���[�j���O(A.912)���o�͂��� *//* <S11D> */
			}
			else if( DetectAlm == CHK_OVERHEAT_CLR_WARNING )
			{
				ALMClearGlobalAlarm( WRN_CTRLBRD );			/* ���[�j���O(A.912)���� *//* <S11D> */
			}
			else
			{
				/* do nothing */
			}
		}
	}
	else if( BoardSel == POWER_BOARD )
	{
		if( DetectAlm != CHK_OVERHEAT_NO_OPERATION )
		{
			if( DetectAlm == CHK_OVERHEAT_SET_ALARM )
			{
				ALMClearGlobalAlarm(WRN_PWRBRD );				/* ���[�j���O(A.913)���� *//* <S11D> */
				ALMSetGlobalAlarm( ALM_PWRBRD );			/* �A���[��(A.7A2)���o�͂��� *//* <S11D> */
			}
			else if( DetectAlm == CHK_OVERHEAT_SET_WARNING )
			{
				ALMSetGlobalAlarm( WRN_PWRBRD );			/* ���[�j���O(A.913)���o�͂��� *//* <S11D> */
			}
			else if( DetectAlm == CHK_OVERHEAT_CLR_WARNING )
			{
				ALMClearGlobalAlarm( WRN_PWRBRD );				/* ���[�j���O(A.913)���� *//* <S11D> */
			}
			else
			{
				/* do nothing */
			}
		}
	}
	else
	{
		/* do nothing */
	}
}



/****************************************************************************************************/
/*																									*/
/*		�T�[�{�p�b�N���x���o���Ď�												<S0DE>				*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : ���x���oIC�̒ʐM�G���[���Ď�����B														*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
void	AdetCheckTempDeviceErr( ALMMAN_HANDLE *AlmMngr, USHORT CtrlBoadErrCnt, USHORT PowerBoadErrCnt )
{
	if( (CtrlBoadErrCnt >= 3) || (PowerBoadErrCnt >= 3) )
	{
		ALMSetServoAlarm( AlmMngr, WRN_PWRBRD );			/* ���[�j���O(A.913)���o�͂���			*/
	}
	return;
}



/****************************************************************************************************/
/*																									*/
/*		�A���v�����[�^�ݒu�����j�^���Z����									<S03A>				*/
/*																									*/
/****************************************************************************************************/
void	AdetInstEnvMonitor( ENVMON *EnvMon, SHORT AmpTemp, SHORT MotTemp )
{
	EnvMon->var.AmpEnvMonSum += AmpTemp; /* [0.1��] */
	EnvMon->var.MotEnvMonSum += MotTemp; /* [��] */

	EnvMon->var.EnvMonSumCnt2s++;
	if( EnvMon->var.EnvMonSumCnt2s >= 1000 )
	{
	/*----------------------------------------------------------------------------------------------*/
	/*	�ݒu�����j�^�l�̌v�Z(���j�^�l�X�V:2000ms����)												*/
	/*----------------------------------------------------------------------------------------------*/
		/* Un025 : �A���v�ݒu�����j�^ [0.001%] */
		EnvMon->var.UnInstEnvMonAmp = EnvMon->var.AmpEnvMonSum;
		/* Un026 : ���[�^�ݒu�����j�^ [0.001%] */
		EnvMon->var.UnInstEnvMonMot = EnvMon->var.MotEnvMonSum;
		/* ������ */
		EnvMon->var.AmpEnvMonSum = 0;
		EnvMon->var.MotEnvMonSum = 0;
		EnvMon->var.EnvMonSumCnt2s = 0;
	}
}


/***************************************** end of file **********************************************/
