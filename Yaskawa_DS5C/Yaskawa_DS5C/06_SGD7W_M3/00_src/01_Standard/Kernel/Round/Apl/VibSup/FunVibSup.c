/*****************************************************************************
Description		: Model Following Control with Vibration Suppression Tuning
-----------------------------------------------------------------------------
Author		: SHCORE
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

Functions	  :
	-- APIs --
	FunVibMfcTuningMain();				:Fn205 Model Following Control with Vibration Suppression Tuning mode Display
	FunVibMfcTuningBegin();				:Fn205 Model Following Control with Vibration Suppression Tuning mode Begin
	FunVibMfcTuningEnd();				:Fn205 Model Following Control with Vibration Suppression Tuning mode End

	-- Locals --
	LpxLcdVibMfcTuningExe();			:LCD Operator Fn205 Execution
	VibMfcTuningDisplay();				:Fn205 Model Following Control with Vibration Suppression Tuning mode Display
	VibMfcTuningCompCheck();			:Fn205 Model Following Control with Vibration Suppression Tuning Check

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
SL.Xue		2013.07.01		created
SL.Xue		2013.12.09		DONE�AERROR�̔���͒ǉ�����
SL.Xue		2013.12.12		MfcGetParamEnableSts�𔻒�����Ƃ��Ēǉ�����

*****************************************************************************/

#include	"FunVibSup.h"


/*--------------------------------------------------------------------------------------------------*/
/* Operation Register 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define OPEMODE_REG1			(0x0000)		/* 1st Operation Mode Register */
#define OPESET_REG1				(0x0001)		/* 1st Operation Mode Set Register */
#define OPEMDLVIBFREQ_REG1		(0x0B0)			/* 0x0B0: �U���}���t��MFC�������g�� */

/*--------------------------------------------------------------------------------------------------*/
/* Operation Mode	 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define FNNORMAL_MODE			(0x0000)		/* Normal Mode	*/
#define FNVIBSUP_MODE			(0x1205) 		/* VibSup Mode */

/*--------------------------------------------------------------------------------------------------*/
/* Operation Command	 																			*/
/*--------------------------------------------------------------------------------------------------*/
#define VIBSUP_EXE				(0x0001)		/* VibSup ���s */
#define VIBSUP_PRMPRESERVE		(0x0007)		/* VibSup �p�����[�^�ۑ� */
#define VIBMFC_STS_COMP			(0U)			/* �����X�e�[�^�X�F�ݒ芮���^�C�j�V����				*/
#define VIBMFC_STS_WAIT			(1U)			/* �����X�e�[�^�X�F�ݒ�҂�							*/
#define VIBMFC_STS_TMOUT		(2U)			/* �����X�e�[�^�X�F�ݒ莸�s(�^�C���A�E�g) 			*/

/*--------------------------------------------------------------------------------------------------*/
/* Prm Register 																					*/
/*--------------------------------------------------------------------------------------------------*/
#define PRMMDLANTFRQ_REG1		(0x145)			/* 0x145: �U���}���P���g��A */
#define PRMMDLRESFRQ_REG1		(0x146)			/* 0x146: �U���}���P���g��B */

/*--------------------------------------------------------------------------------------------------*/
/* Un Register 																						*/
/*--------------------------------------------------------------------------------------------------*/
#define UNRVIBFREQUENCY_REG1	(0x107)			/* 0x107: �c���U�����g�� */

/*--------------------------------------------------------------------------------------------------*/
/* �U�����o�����l																					*/
/*--------------------------------------------------------------------------------------------------*/
#define VIBSUP_DET_FRQ_MIN		(10U)			/* Fn023���̎c���U�����o���g������ 1.0Hz			*/
#define VIBSUP_DET_FRQ_MAX		(1000U)			/* Fn023���̎c���U�����o���g����� 100.0Hz			*/

/*--------------------------------------------------------------------------------------------------*/
#define VIBSUP_TUN_MAX			(3U)			/* �Q�C�������ő包									*/
#define VIBSUP_TUN_MIN			(1U)			/* �Q�C��/���g�������ŏ���							*/

/*--------------------------------------------------------------------------------------------------*/
#define FUN_CONTINUE			(0L)			/*Continue	*/
#define FUN_END					(1L)			/*End		*/

/*--------------------------------------------------------------------------------------------------*/
#define OPEREG_WRITE( idx, data, hmsg )   (COPEREG->Write( COPEREG, (idx), (data), hmsg ))
#define PRMREG_WRITE( idx, data, hmsg )   (CPARAMX->Write( CPARAMX, (idx), (data), hmsg ))	/*<S054>*/
#define STSREG_READX( idx, pResData, CheckType, hmsg )    (COPEREG->Read( COPEREG, (idx), (pResData),\
																	(CheckType), hmsg ))  /* <S073> */
#define PRMREG_READX( idx, pResData, CheckType, hmsg )    (CPARAMX->Read( (COBJECT *)(CPARAMX), (idx),\
															 	 	 (pResData), (CheckType), hmsg ))  /* <S073> */
#define UNREG_READX( idx, pResData, CheckType, hmsg )    (CUNREGX->Read( (COBJECT *)(CUNREGX), (idx),\
															 	 	 (pResData), (CheckType), hmsg ))  /* <S073> */

/****************************************************************************************************/
/*																									*/
/*		Function List																				*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Public Function																				*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunVibMfcTuningBegin(FUNMSG *Fmsg); 	/* Fn205 Model Following Control with Vibration Suppression Tuning Mode Begin	*/
LONG FunVibMfcTuningMain(FUNMSG *Fmsg);		/* Fn205 Model Following Control with Vibration Suppression Tuning Mode Main	*/
void FunVibMfcTuningEnd(FUNMSG *Fmsg);		/* Fn205 Model Following Control with Vibration Suppression Tuning Mode End	*/

/*--------------------------------------------------------------------------------------------------*/
/*		Private Function																			*/
/*--------------------------------------------------------------------------------------------------*/
static void VibMfcTuningDisplay(FUNMSG *Fmsg);			/* Fn205 Model Following Control with Vibration Suppression Tuning mode Display */
static void VibMfcTuningCompCheck(FUNMSG *Fmsg);		/* Fn205 Model Following Control with Vibration Suppression Tuning Check */
static LONG LpxLcdVibMfcTuningExe(FUNMSG *Fmsg);		/* LCD Operator Fn205 Execution 			*/

/*****************************************************************************
  Description: Fn205 Model Following Control with Vibration Suppression Tuning Mode Display
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		0: continue, 1: end 
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
LONG FunVibMfcTuningMain(FUNMSG *Fmsg)
{
	MSGIF *hMsg = Fmsg->Super;
	LONG rc;
	rc = FUN_CONTINUE;
/*--------------------------------------------------------------------------*/
	if( KPI_CHK_LopHmsg( hMsg ) == TRUE )					/* LCD Operator */
	{
		rc = LpxLcdVibMfcTuningExe( Fmsg );
	} 
/*--------------------------------------------------------------------------*/
	return rc;
}


/*****************************************************************************
  Description: Fn205 Model Following Control with Vibration Suppression Tuning mode Begin
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		OK: Success, Other: Failure 
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
LONG FunVibMfcTuningBegin(FUNMSG *Fmsg)
{
LONG	rc = OK;
BOOL ResFlag = TRUE;
FNVIBSUPV  *pv = (FNVIBSUPV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	pv->var.State = VIBSUP_INIT;
/*--------------------------------------------------------------------------------------------------*/
	LpxPrmChangeOpeDspMode();								/* ���ׂẴ��[�U�[�萔��\���ɋ����ύX		*///<S197>
/*--------------------------------------------------------------------------------------------------*/
/*	Search Register																					*/
/*--------------------------------------------------------------------------------------------------*/
	rc = COPEREG->Search( COPEREG, 							/* Search Register (Operation Mode)		*/
				OPEMODE_REG1,
				0,
				&(pv->var.OpeModeIdx),
				MCHK_REG_READ,
				Hmsg);
	if( rc != OK )
	{
		ResFlag = FALSE;
	}
/*--------------------------------------------------------------------------------------------------*/
	if ( ResFlag == TRUE )
	{
		rc = COPEREG->Search( COPEREG,						/* Search Register (Operation Command)	*/
					OPESET_REG1,
					0,
					&(pv->var.OpeSetIdx),
					MCHK_REG_READ,
					Hmsg);
		if( rc != OK )
		{
			ResFlag = FALSE;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
	if ( ResFlag == TRUE )
	{
		rc = CPARAMX->Search( (COBJECT *)CPARAMX,			/* Search Register (Parameter 0x145)	*/
					PRMMDLANTFRQ_REG1,
					0,
					&(pv->var.PrmMdlantfrqIdx),
					MCHK_REG_READ,
					Hmsg);
		if( rc != OK )
		{
			ResFlag = FALSE;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
	if ( ResFlag == TRUE )
	{
		rc = CPARAMX->Search( (COBJECT *)CPARAMX,			/* Search Register (Parameter 0x146)	*/
					PRMMDLRESFRQ_REG1,
					0,
					&(pv->var.PrmMdlresfrqIdx),
					MCHK_REG_READ,
					Hmsg);
		if( rc != OK )
		{
			ResFlag = FALSE;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
	if ( ResFlag == TRUE )
	{
		rc = COPEREG->Search( COPEREG,						/* Search Register (Operation 0x0B0)	*/
					OPEMDLVIBFREQ_REG1,
					0,
					&(pv->var.OpeMdlVibFreqIdx),
					MCHK_REG_READ,
					Hmsg);
		if( rc != OK )
		{
			ResFlag = FALSE;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
	if ( ResFlag == TRUE )
	{
		rc = CUNREGX->Search( (COBJECT *)CUNREGX,					/* Search Register (Un 0x107)	*/
					UNRVIBFREQUENCY_REG1,
					0,
					&(pv->var.UnRvibFrequencyIdx),
					MCHK_REG_READ,
					Hmsg);
		if( rc != OK )
		{
			ResFlag = FALSE;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/* Set VibSup Mode to Mode Register 																*/
/*--------------------------------------------------------------------------------------------------*/
	if ( ResFlag == TRUE )
	{
		rc = OPEREG_WRITE( pv->var.OpeModeIdx, FNVIBSUP_MODE, Hmsg );
	}
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}


/*****************************************************************************
  Description: Fn205 Model Following Control with Vibration Suppression Tuning Mode End
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	none
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
void FunVibMfcTuningEnd(FUNMSG *Fmsg)
{
FNVIBSUPV  *pv = (FNVIBSUPV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------*/
	LpxPrmReChangeOpeDspMode();				/* �f�W�I�y�\���ݒ�����ɖ߂�		*///<S197>
/*--------------------------------------------------------------------------*/
	OPEREG_WRITE( pv->var.OpeModeIdx, FNNORMAL_MODE, Hmsg );
}



/*****************************************************************************
  Description: Fn205 Model Following Control with Vibration Suppression Tuning mode Display Data
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void VibMfcTuningDisplay(FUNMSG *Fmsg)
{
	FNVIBSUPV *pv = (FNVIBSUPV *)(Fmsg->pVar);
	UCHAR *ResBuf = Fmsg->pTxBuf;
	ULONG	BlinkVal;

	RpxLcdClrLineBlnk( 3U, ResBuf );

	/* TITLE	*/
//	RpxLcdSetStr( 0U, 8U, (UCHAR *)"-Vib Sup-", ResBuf );			/* <S154> */
	RpxLcdSetStr( 0U, 10U, (UCHAR *)"Vib Sup", ResBuf );			/* <S154> */

	RpxLcdSetStr( 1U, 0U, (UCHAR *)"                 ", ResBuf );
	RpxLcdSetStr( 4U, 0U, (UCHAR *)"                 ", ResBuf );
	RpxLcdSetStr( 2U, 0U, (UCHAR *)"Measure f=     Hz", ResBuf );
	RpxLcdSetStr( 3U, 0U, (UCHAR *)"Setting f=     Hz", ResBuf );

	if ( ( pv->var.DetFreq >= VIBSUP_DET_FRQ_MIN ) && ( pv->var.DetFreq <= VIBSUP_DET_FRQ_MAX) )
	{
		RpxLcdSetDecPnt( 2U, 10U, pv->var.DetFreq, 4U, (USHORT)FALSE, 1, ResBuf );
	}
	else
	{
		RpxLcdSetStr( 2U, 10U, (UCHAR *)"---.-", ResBuf );
	}
	if ( pv->var.DPos == 0U )
	{
		BlinkVal = 0U;
	}
	else
	{
		BlinkVal = 1U;
	}
	RpxLcdSetDecPnt( 3U, 10U, pv->var.ResMdlVibFreqData, 4U, (USHORT)FALSE, 1, ResBuf );
	RpxLcdSetBlnk( 3U, ( 14U - BlinkVal - pv->var.DPos ), 1U, ResBuf );

	if ( pv->var.Match == FALSE )
	{
		RpxLcdSetBlnk( 3U, 0U, 7U, ResBuf );
	}
	return;
}


/*****************************************************************************
  Description: Fn205 Model Following Control with Vibration Suppression Tuning Check
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		FUN_CONTINUE: Execution Continue, FUN_END: Execution End
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void VibMfcTuningCompCheck(FUNMSG *Fmsg)
{
	FNVIBSUPV *pv = (FNVIBSUPV *)(Fmsg->pVar);
	MFCTRL *MFControl;
	MSGIF *Hmsg = Fmsg->Super;
	MFControl = &(Fmsg->Super->pAxRsc->BaseControls->MFControl);

	PRMREG_READX( pv->var.PrmMdlresfrqIdx, &(pv->var.ResMdlresfrqData), MCHK_REG_READ, Hmsg );
	if( ( MfcGetParamEnableSts( &(Fmsg->Super->pAxRsc->BaseControls->MFControl) ) == TRUE ) &&
		( MFControl->conf.MFCModelSetting == VIBMFC_ONE_BASE ) &&
		( pv->var.ResMdlVibFreqData == pv->var.ResMdlantfrqData ) && ( pv->var.ResMdlVibFreqData == pv->var.ResMdlresfrqData ) )
	{
		pv->var.Match = FALSE;
	}
	else
	{
		pv->var.Match = TRUE;
	}
}


/*****************************************************************************
  Description: Fn205 Model Following Control with Vibration Suppression Tuning mode Execution
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		FUN_CONTINUE: Execution Continue, FUN_END: Execution End
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static LONG LpxLcdVibMfcTuningExe(FUNMSG *Fmsg)
{
FNVIBSUPV *pv = (FNVIBSUPV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
UCHAR *ResBuf = Fmsg->pTxBuf;
OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
BLINKFLG *pBlinkFlg = (BLINKFLG *)(Fmsg->pBlnkKick);
LONG tmp;
LONG i;
ULONG data;
MFCTRL *MFControl;
LONG rc = FUN_CONTINUE;														 /* 0: continue, 1: end */
MFControl = &(Fmsg->Super->pAxRsc->BaseControls->MFControl);
/*--------------------------------------------------------------------------------------------------*/
		/* clear Tx buffer */
		RpxLcdClrTxBuf( ResBuf );
/*--------------------------------------------------------------------------------------------------*/
/*		VibSup (Fn205) Sequence																		*/
/*--------------------------------------------------------------------------------------------------*/
		switch( pv->var.State )
		{
/*--------------------------------------------------------------------------------------------------*/
		case VIBSUP_INIT: 													/* Initial State 		*/
/*--------------------------------------------------------------------------------------------------*/
			PRMREG_READX( pv->var.PrmMdlantfrqIdx, &(pv->var.ResMdlantfrqData), MCHK_REG_READ, Hmsg );
			STSREG_READX( pv->var.OpeMdlVibFreqIdx, &(pv->var.ResMdlVibFreqData), MCHK_REG_READ, Hmsg );
			pv->var.ResMdlVibFreqData = pv->var.ResMdlantfrqData;
			if( pv->var.ResMdlVibFreqData < VIBSUP_DET_FRQ_MIN )
			{
				pv->var.ResMdlVibFreqData = VIBSUP_DET_FRQ_MIN;
			}
			else if( pv->var.ResMdlVibFreqData > VIBSUP_DET_FRQ_MAX )
			{
				pv->var.ResMdlVibFreqData = VIBSUP_DET_FRQ_MAX;
			}
			else
			{
				/* do nothing */
			}
			pv->var.State = VIBSUP_ADJUST;
			break;
/*--------------------------------------------------------------------------------------------------*/
		case VIBSUP_ADJUST: 													/*  adjust state	*/
/*--------------------------------------------------------------------------------------------------*/
			UNREG_READX( pv->var.UnRvibFrequencyIdx, &(pv->var.ResRvibFrequencyData), MCHK_REG_READ, Hmsg );
			pv->var.DetFreq = pv->var.ResRvibFrequencyData;
			tmp = 1;
			for ( i = 0; (ULONG)i < pv->var.DPos; i++ )
			{
				tmp = tmp * 10;
			}
			if( OpKeyMake.up == TRUE )
			{
				data = pv->var.ResMdlVibFreqData + (ULONG)tmp;
				if( data > VIBSUP_DET_FRQ_MAX )
				{
					pv->var.ResMdlVibFreqData = VIBSUP_DET_FRQ_MAX;
					PRMREG_WRITE( pv->var.PrmMdlantfrqIdx, pv->var.ResMdlVibFreqData, Hmsg );
				}
				else
				{
					pv->var.ResMdlVibFreqData = data;
					PRMREG_WRITE( pv->var.PrmMdlantfrqIdx, pv->var.ResMdlVibFreqData, Hmsg );
				}
			}
			else if( OpKeyMake.down == TRUE )
			{
				data = pv->var.ResMdlVibFreqData - (ULONG)tmp;
				if( data < VIBSUP_DET_FRQ_MIN )
				{
					pv->var.ResMdlVibFreqData = VIBSUP_DET_FRQ_MIN;
					PRMREG_WRITE( pv->var.PrmMdlantfrqIdx, pv->var.ResMdlVibFreqData, Hmsg );
				}
				else
				{
					pv->var.ResMdlVibFreqData = data;
					PRMREG_WRITE( pv->var.PrmMdlantfrqIdx, pv->var.ResMdlVibFreqData, Hmsg );
				}
			}
			else if( OpKeyMake.data  == TRUE )
			{
				if( pv->var.Match == TRUE )
				{
					/* EEPROM�ɋL�����锻�� */
					if ( OPEREG_WRITE( pv->var.OpeSetIdx, VIBSUP_PRMPRESERVE, Hmsg ) != OK )
					{
						pBlinkFlg->error = TRUE;
					}
					else
					{
						pBlinkFlg->done = TRUE;
					}
				}
				else
				{
					OPEREG_WRITE( pv->var.OpeSetIdx, VIBSUP_EXE, Hmsg );
					pv->var.State = VIBSUP_SETREQ;
				}
			}
			else if( OpKeyMake.shup == TRUE )
			{
				if( pv->var.DPos < ( VIBSUP_TUN_MAX - 1U ) )
				{
					pv->var.DPos++;
				}
				else
				{
					pv->var.DPos = 0U;
				}
			}
			else if( OpKeyMake.shdown == TRUE )
			{
				if( ( pv->var.DPos > 0U ) && ( pv->var.DPos < VIBSUP_TUN_MAX ) )
				{
					pv->var.DPos--;
				}
				else
				{
					pv->var.DPos = VIBSUP_TUN_MAX - 1U;
				}
			}
			else if( OpKeyMake.scroll == TRUE )
			{
				if( ( pv->var.DetFreq >= VIBSUP_DET_FRQ_MIN ) && ( pv->var.DetFreq <= VIBSUP_DET_FRQ_MAX ) )
				{
					pv->var.ResMdlVibFreqData = pv->var.DetFreq;
					PRMREG_WRITE( pv->var.PrmMdlantfrqIdx, pv->var.ResMdlVibFreqData, Hmsg );
				}
			}
			else if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = VIBSUP_END;
			}
			VibMfcTuningCompCheck( Fmsg );
			break;
/*--------------------------------------------------------------------------------------------------*/
		case VIBSUP_SETREQ: 													/*  setreq state	*/
/*--------------------------------------------------------------------------------------------------*/
			if( MFControl->conf.MFCModelSetting == TRUE )
			{
				pv->var.State = VIBSUP_WAITMFC;
			}
			else
			{
				pv->var.State = VIBSUP_WAITNORM;
			}
			pv->var.ResMdlantfrqData = pv->var.ResMdlVibFreqData;
			pv->var.ResMdlresfrqData = pv->var.ResMdlVibFreqData;
			KpiRstLongTimer( &( pv->var.FfWaitCntr ) );
			break;
/*--------------------------------------------------------------------------------------------------*/
		case VIBSUP_WAITMFC: 													/*  waitmfc state	*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = VIBSUP_END;
			}
			else
			{
				if( MfcGetParamEnableSts( &(Fmsg->Super->pAxRsc->BaseControls->MFControl) ) == TRUE )
				{
					pv->var.State = VIBSUP_SETCOMP;
				}
				else
				{
					if( KpiGetLongTimer( &( pv->var.FfWaitCntr ) ) > VIBMFC_FF_WAITTMOUT )
					{
						pv->var.State = VIBSUP_SETCOMP;
					}
				}
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case VIBSUP_WAITNORM: 													/*  waitnorm state	*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = VIBSUP_END;
			}
			else
			{
				if( MfcGetParamEnableSts( &(Fmsg->Super->pAxRsc->BaseControls->MFControl) ) == TRUE )
				{
					pv->var.State = VIBSUP_SETCOMP;
				}
				else
				{
					if( KpiGetLongTimer( &( pv->var.FfWaitCntr ) ) > VIBMFC_FF_WAITTMOUT )
				{
					pv->var.State = VIBSUP_SETCOMP;
				}
			}
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case VIBSUP_SETCOMP: 													/*  setcomp state	*/
/*--------------------------------------------------------------------------------------------------*/
			pv->var.Match = TRUE;
			pv->var.State = VIBSUP_ADJUST;
			break;
/*--------------------------------------------------------------------------------------------------*/
		case VIBSUP_END: 														/*  end state		*/
/*--------------------------------------------------------------------------------------------------*/
			FunVibMfcTuningEnd( Fmsg );
			rc = FUN_END;
			break;
		default:
			break;
		}
		VibMfcTuningDisplay( Fmsg );
	return (rc);
}


/***************************************** end of file **********************************************/
