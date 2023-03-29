/*****************************************************************************
Description		: A-type Vibration Suppression Control Tuning
-----------------------------------------------------------------------------
Author		: SHCORE
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

Functions	  :
	-- APIs --
	FunAtypeVibTuningMain();				:Fn204 A-type Vibration Suppression Control Tuning mode Display
	FunAtypeVibTuningBegin();				:Fn204 A-type Vibration Suppression Control Tuning mode Begin
	FunAtypeVibTuningEnd();					:Fn204 A-type Vibration Suppression Control Tuning mode End

	-- Locals --
	LpxLcdAtypeVibTuningExe();				:LCD Operator Fn204 Execution

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
SL.Xue		2013.06.24		created
SL.Xue		2013.09.09		DONE、ERRORの判定は追加した

*****************************************************************************/

#include	"FunAVibSup.h"


/*--------------------------------------------------------------------------------------------------*/
/* Operation Register 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define OPEMODE_REG1			(0x0000)		/* 1st Operation Mode Register */
#define OPESET_REG1				(0x0001)		/* 1st Operation Mode Set Register */
#define OPEAVIBMODE_REG1		(0x0A0)			/* 0x0A0: A型制振機能調整モード設定 Data */
#define OPEAVIBSTATE_REG1		(0x0A1)			/* 0x0A1: A型制振機能調整完了 Data */
#define OPEAVIBFREQ_REG1		(0x0A2)			/* 0x0A2: A型制振機能調整周波数 Data */
#define OPEAVIBGAIN_REG1		(0x0A3)			/* 0x0A3: A型制振機能調整ゲイン Data */
#define OPVIBMRF0FREQ			(0x400)			/* 0x400: 振動モニタ共振周波数１ */

/*--------------------------------------------------------------------------------------------------*/
/* Operation Mode	 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define FNNORMAL_MODE			(0x0000)		/* Normal Mode	*/
#define FNAVIBSUP_MODE			(0x1204) 		/* A-type Vibration Suppression Control Tuning Mode */

/*--------------------------------------------------------------------------------------------------*/
/* Operation Command 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define AVIBSUPCMD_AVIBMODE0 	(0x0U)			/* 0：周波数自動検出モード */
#define AVIBSUPCMD_AVIBMODE1 	(0x1U)			/* 1：周波数手動検出モード */
#define AVIB_STS_COMP			(0x1U)			/* 振動検出完了：検出完了 */
#define AVIB_STS_TIMEOUT		(0x2U)			/* 振動検出完了：検出不可(タイムアウト) */
#define AVIB_STS_RANGEOV		(0x3U)			/* 振動検出完了：検出不可(周波数範囲外) */
#define AVIB_STS_NONVIB			(0x4U)			/* 振動検出完了：検出不可(周波数特定不可)*/
#define AVIBSUPCMD_EXE2			(0x0008)		/* 実行２（補助実行）*/
#define AVIBSUPCMD_EXE1			(0x0001)		/* 実行 */
#define AVIBSUPCMD_PRMPRESERVE	(0x0007)		/* パラメータ保存 */

/*--------------------------------------------------------------------------------------------------*/
/* Prm Register 																					*/
/*--------------------------------------------------------------------------------------------------*/
#define PRMAVIBFRQ_REG1			(0x161)			/* 0x161: A型制振周波数 Data */
#define PRMAVIBDAMP_REG1		(0x163)			/* 0x163: A型制振ダンピングゲイン Data */
#define PRMVIBNORM_REG1			(0x312)			/* 0x312: 振動検出レベル(回転) Data */

/*--------------------------------------------------------------------------------------------------*/
#define AVIBFTUN_MAX			(0x4U)			/* 周波数調整最大桁 */
#define AVIBGTUN_MAX			(0x2U)			/* ゲイン調整最大桁 */

/*--------------------------------------------------------------------------------------------------*/
#define FUN_CONTINUE			(0L)			/*Continue	*/
#define FUN_END					(1L)			/*End		*/

/*--------------------------------------------------------------------------------------------------*/
#define OPEREG_WRITE( idx, data, hmsg )   (COPEREG->Write( COPEREG, (idx), (data), hmsg ))
#define PRMREG_WRITE( idx, data, hmsg )   (CPARAMX->Write( CPARAMX, (idx), (data), hmsg ))	/*<S054>*/
#define STSREG_READX( idx, pResData, CheckType, hmsg )    (COPEREG->Read( COPEREG, (idx), (pResData),\
																	(CheckType), hmsg ))  /* <S073? */
#define PRMREG_READX( idx, pResData, CheckType, hmsg )    (CPARAMX->Read( (COBJECT *)(CPARAMX), (idx),\
															 	 	 (pResData), (CheckType), hmsg )) /* <S073> */

/****************************************************************************************************/
/*																									*/
/*		Function List																				*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Public Function																				*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunAtypeVibTuningBegin(FUNMSG *Fmsg); 		/* Fn204 A-type Vibration Suppression Control Tuning Mode Begin	*/
LONG FunAtypeVibTuningMain(FUNMSG *Fmsg);		/* Fn204 A-type Vibration Suppression Control Tuning Mode Main	*/
void FunAtypeVibTuningEnd(FUNMSG *Fmsg);		/* Fn204 A-type Vibration Suppression Control Tuning Mode End	*/

/*--------------------------------------------------------------------------------------------------*/
/*		Private Function																			*/
/*--------------------------------------------------------------------------------------------------*/
static void AtypeVibTuningDisplay(FUNMSG *Fmsg); 		/*  A type Vibration Tuning mode Display 	*/
static LONG LpxLcdAtypeVibTuningExe(FUNMSG *Fmsg);		/* LCD Operator Fn204 Execution 			*/
#if  SVFSEL_PANELOP == 0
#define LpxPnlAtypeVibTuningExe( x ) (OK)
#else
LONG LpxPnlAtypeVibTuningExe(FUNMSG *hMsg);				/* Panel Operator Fn205 Execution 			*/
#endif

/*****************************************************************************
  Description: Fn204 A-type Vibration Suppression Control Tuning Mode Display
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		0: continue, 1: end 
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
LONG FunAtypeVibTuningMain(FUNMSG *Fmsg)
{
	MSGIF *hMsg = Fmsg->Super;
	LONG rc;
	rc = FUN_CONTINUE;
/*--------------------------------------------------------------------------*/
	if( KPI_CHK_LopHmsg( hMsg ) == TRUE )					/* LCD Operator */
	{
		rc = LpxLcdAtypeVibTuningExe( Fmsg );
	} 
/*--------------------------------------------------------------------------*/
	else if ( KPI_CHK_PopHmsg( hMsg ) == TRUE )				/*Panel Operator*/
	{
		rc = LpxPnlAtypeVibTuningExe( Fmsg );
	}
/*--------------------------------------------------------------------------*/
	else
	{
		/*	Do Nothing	*/
	}
/*--------------------------------------------------------------------------*/
	return rc;
}


/*****************************************************************************
  Description: Fn204 A-type Vibration Suppression Control Tuning mode Begin
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		OK: Success, Other: Failure 
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
LONG FunAtypeVibTuningBegin(FUNMSG *Fmsg)
{
LONG	rc = OK;
BOOL ResFlag = TRUE;
FNAVIBSUPV  *pv = (FNAVIBSUPV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	pv->var.State = AVIBSUP_INIT;
/*--------------------------------------------------------------------------------------------------*/
	LpxPrmChangeOpeDspMode();								/* すべてのユーザー定数を表示に強制変更		*///<S197>
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
		rc = CPARAMX->Search( (COBJECT *)CPARAMX,			/* Search Register (Parameter 0x163)	*/
					PRMAVIBDAMP_REG1,
					0,
					&(pv->var.PrmAvibdampIdx),
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
		rc = COPEREG->Search( COPEREG,						/* Search Register (Operation 0x0A0 )	*/
					OPEAVIBMODE_REG1,
					0,
					&(pv->var.OpeAvibModeIdx),
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
		rc = COPEREG->Search( COPEREG,						/* Search Register (Operation 0x0A2 )	*/
					OPEAVIBFREQ_REG1,
					0,
					&(pv->var.OpeAvibFreqIdx),
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
		rc = CPARAMX->Search( (COBJECT *)CPARAMX,			/* Search Register (Parameter 0x161 )	*/
					PRMAVIBFRQ_REG1,
					0,
					&(pv->var.PrmAvibfrqIdx),
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
		rc = COPEREG->Search( COPEREG,						/* Search Register (Operation 0x0A3)	*/
					OPEAVIBGAIN_REG1,
					0,
					&(pv->var.OpeAvibGainIdx),
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
		rc = COPEREG->Search( COPEREG,						/* Search Register (Operation 0x0A1)	*/
					OPEAVIBSTATE_REG1,
					0,
					&(pv->var.OpeAvibStateIdx),
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
		rc = CPARAMX->Search( (COBJECT *)CPARAMX,			/* Search Register (Parameter 0x312 )	*/
					PRMVIBNORM_REG1,
					0,
					&(pv->var.PrmVibnormIdx),
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
		rc = COPEREG->Search( COPEREG,						/* Search Register (Operation 0x400)	*/
					OPVIBMRF0FREQ,
					0,
					&(pv->var.OpeVibmRf0FreqIdx),
					MCHK_REG_READ,
					Hmsg);
		if( rc != OK )
		{
			ResFlag = FALSE;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/* Set AVibSup Mode to Mode Register 																*/
/*--------------------------------------------------------------------------------------------------*/
	if ( ResFlag == TRUE )
	{
		rc = OPEREG_WRITE( pv->var.OpeModeIdx, FNAVIBSUP_MODE, Hmsg );
	}
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}


/*****************************************************************************
  Description: Fn204 A-type Vibration Suppression Control Tuning Mode End
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	none
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
void FunAtypeVibTuningEnd(FUNMSG *Fmsg)
{
FNAVIBSUPV  *pv = (FNAVIBSUPV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------*/
	LpxPrmReChangeOpeDspMode();				/* デジオペ表示設定を元に戻す		*///<S197>
/*--------------------------------------------------------------------------*/
	OPEREG_WRITE( pv->var.OpeModeIdx, FNNORMAL_MODE, Hmsg );
}


/*****************************************************************************
  Description: Fn204 A type Vibration Tuning mode Display Data
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void AtypeVibTuningDisplay(FUNMSG *Fmsg)
{
	FNAVIBSUPV *pv = (FNAVIBSUPV *)(Fmsg->pVar);
	UCHAR *ResBuf = Fmsg->pTxBuf;
	
	/* TITLE	*/
//	RpxLcdSetStr( 0U, 8U, (UCHAR *)"-Vib Sup-", ResBuf );		/* <S154> */
	RpxLcdSetStr( 0U, 10U, (UCHAR *)"Vib Sup", ResBuf );		/* <S154> */
	RpxLcdClrLineBlnk( 2U, ResBuf );
	RpxLcdClrLineBlnk( 3U, ResBuf );

	if ( (pv->var.State == AVIBSUP_INIT) || (pv->var.State == AVIBSUP_MODESEL) )
	{
		RpxLcdSetStr( 1U, 0U, (UCHAR *)"                 ", ResBuf );
		RpxLcdSetStr( 3U, 0U, (UCHAR *)"                 ", ResBuf );
		RpxLcdSetStr( 4U, 0U, (UCHAR *)"                 ", ResBuf );
		RpxLcdSetStr( 2U, 0U, (UCHAR *)"Tuning Mode = ", ResBuf );
		RpxLcdSetDecPnt( 2U, 14U, pv->var.ResAvibModeData, 1U, (USHORT)FALSE, 0, ResBuf );
		RpxLcdSetBlnk( 2U, 14U, 1U, ResBuf );
	}
	else
	{
		RpxLcdSetStr( 2U, 0U, (UCHAR *)" freq =      Hz  ", ResBuf );
		if ( pv->var.ResAvibFreqData == 0U )
		{
			RpxLcdSetStr( 2U, 8U, (UCHAR *)"----", ResBuf );
		}
		else
		{
			RpxLcdSetDecPnt( 2U, 8U, pv->var.ResAvibFreqData, 4U, (USHORT)FALSE, 0, ResBuf );
		}
		RpxLcdSetStr( 3U, 0U, (UCHAR *)" damp =          ", ResBuf );
		RpxLcdSetDecPnt( 3U, 8U, pv->var.ResAvibGainData, 4U, (USHORT)FALSE, 0, ResBuf );

		switch ( pv->var.State )
		{
			case AVIBSUP_TRGWAIT :
			case AVIBSUP_MEASURE :
				RpxLcdSetBlnk( 2U, 7U, 5U, ResBuf );
				break;

			case AVIBSUP_FREQTUNE :
				RpxLcdSetBlnk( 2U, (11U - pv->var.DPos), 1U, ResBuf );
				break;

			case AVIBSUP_GAINTUNE :
				RpxLcdSetBlnk( 3U, (11U - pv->var.DPos), 1U, ResBuf );
				break;
			case AVIBSUP_COMP :
			case AVIBSUP_SET :
			case AVIBSUP_END :
			default:
				break;
		}
	}
	return;
}


/*****************************************************************************
  Description: Fn204 A-type Vibration Suppression Control Tuning mode Execution
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		FUN_CONTINUE: Execution Continue, FUN_END: Execution End
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static LONG LpxLcdAtypeVibTuningExe(FUNMSG *Fmsg)
{
FNAVIBSUPV *pv = (FNAVIBSUPV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
UCHAR *ResBuf = Fmsg->pTxBuf;
OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
BLINKFLG *pBlinkFlg = (BLINKFLG *)(Fmsg->pBlnkKick);
LONG tmp;
LONG i;
ULONG data;
DETVIBOBS 	*DetVibObs;
RESVIB 		*ResVib;
LONG rc = FUN_CONTINUE;														 /* 0: continue, 1: end */
DetVibObs = &(Fmsg->Super->pAxRsc->DetVib->DetVibObs);
ResVib = &(Fmsg->Super->pAxRsc->BaseControls->ResVib);
/*--------------------------------------------------------------------------------------------------*/
		/* clear Tx buffer */
		RpxLcdClrTxBuf( ResBuf );
		AtypeVibTuningDisplay( Fmsg );
/*--------------------------------------------------------------------------------------------------*/
/*		AVibSup (Fn204) Sequence																	*/
/*--------------------------------------------------------------------------------------------------*/
		if( OpKeyMake.mode == TRUE )
		{
			pv->var.State = AVIBSUP_END;
		}
		switch( pv->var.State )
		{
/*--------------------------------------------------------------------------------------------------*/
		case AVIBSUP_INIT: 													/* Initial State		*/
/*--------------------------------------------------------------------------------------------------*/
			pv->var.Match = FALSE;
			STSREG_READX( pv->var.OpeAvibModeIdx, &(pv->var.ResAvibModeData), MCHK_REG_READ, Hmsg );
			STSREG_READX( pv->var.OpeAvibGainIdx, &(pv->var.ResAvibGainData), MCHK_REG_READ, Hmsg );
			PRMREG_READX( pv->var.PrmAvibdampIdx, &(pv->var.ResAvibdampData), MCHK_REG_READ, Hmsg );
			STSREG_READX( pv->var.OpeAvibFreqIdx, &(pv->var.ResAvibFreqData), MCHK_REG_READ, Hmsg );
			PRMREG_READX( pv->var.PrmAvibfrqIdx, &(pv->var.ResAvibfrqData), MCHK_REG_READ, Hmsg );
			if( ResVib->AresVibSetting == TRUE )
			{
				pv->var.ResAvibModeData = AVIBSUPCMD_AVIBMODE1;
				OPEREG_WRITE( pv->var.OpeAvibModeIdx, pv->var.ResAvibModeData, Hmsg );
				pv->var.ResAvibGainData = pv->var.ResAvibdampData;
			}
			else
			{
				pv->var.ResAvibModeData = AVIBSUPCMD_AVIBMODE0;
				OPEREG_WRITE( pv->var.OpeAvibModeIdx, pv->var.ResAvibModeData, Hmsg );
				pv->var.ResAvibGainData = 0U;
			}
			pv->var.State = AVIBSUP_MODESEL;
			break;
/*--------------------------------------------------------------------------------------------------*/
		case AVIBSUP_MODESEL: 												/* Modesel State		*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.up == TRUE )
			{
				pv->var.ResAvibModeData = AVIBSUPCMD_AVIBMODE1;
				OPEREG_WRITE( pv->var.OpeAvibModeIdx, pv->var.ResAvibModeData, Hmsg );
			}
			else if( OpKeyMake.down == TRUE )
			{
				pv->var.ResAvibModeData = AVIBSUPCMD_AVIBMODE0;
				OPEREG_WRITE( pv->var.OpeAvibModeIdx, pv->var.ResAvibModeData, Hmsg );
			}
			else if( OpKeyMake.data == TRUE )
			{
				if( pv->var.ResAvibModeData == AVIBSUPCMD_AVIBMODE0 )
				{
					OPEREG_WRITE( pv->var.OpeSetIdx, AVIBSUPCMD_EXE2, Hmsg );
					pv->var.ResAvibFreqData = 0U;
					pv->var.State = AVIBSUP_TRGWAIT;
					KpiRstLongTimer( &( pv->var.LongTimer ) );
				}
				else if ( pv->var.ResAvibModeData == AVIBSUPCMD_AVIBMODE1 )
				{
					OPEREG_WRITE( pv->var.OpeSetIdx, AVIBSUPCMD_EXE1, Hmsg );
					pv->var.ResAvibFreqData = pv->var.ResAvibfrqData / 10U;
					if( ResVib->AresVibSetting == TRUE )
					{
						pv->var.Match = TRUE;
						pv->var.State = AVIBSUP_GAINTUNE;
					}
					else
					{
						pv->var.State = AVIBSUP_FREQTUNE;
					}
				}
				else
				{
					/* do nothing */
				}
			}
			else
			{
				/* do nothing */
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case AVIBSUP_TRGWAIT: 												/* Trgwait State		*/
/*--------------------------------------------------------------------------------------------------*/
			PRMREG_READX( pv->var.PrmVibnormIdx, &(pv->var.ResVibnormData), MCHK_REG_READ, Hmsg );
			if( ( DetVibObs->var.MaxSpdErr - DetVibObs->var.MinSpdErr ) > pv->var.ResVibnormData )
			{
				pv->var.State = AVIBSUP_MEASURE;
			}
			else
			{
				if ( KpiGetLongTimer( &( pv->var.LongTimer ) ) >= 8000U )
				{
					pv->var.State = AVIBSUP_MODESEL;
				}
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case AVIBSUP_MEASURE: 												/* Measure State		*/
/*--------------------------------------------------------------------------------------------------*/
			STSREG_READX( pv->var.OpeAvibStateIdx, &(pv->var.ResAvibStateData), MCHK_REG_READ, Hmsg );
			STSREG_READX( pv->var.OpeVibmRf0FreqIdx, &(pv->var.ResVibmRf0FreqData), MCHK_REG_READ, Hmsg );
			if( pv->var.ResAvibStateData == AVIB_STS_COMP )
			{
				if( ( pv->var.ResVibmRf0FreqData > AVIB_FRQ_MIN ) && ( pv->var.ResVibmRf0FreqData < AVIB_FRQ_MAX ) )
				{
					pv->var.ResAvibFreqData = pv->var.ResVibmRf0FreqData;
					OPEREG_WRITE( pv->var.OpeAvibFreqIdx, pv->var.ResAvibFreqData, Hmsg );
					pv->var.State = AVIBSUP_COMP;
				}
				else
				{
					pv->var.State = AVIBSUP_TRGWAIT;
				}
			}
			else if( pv->var.ResAvibStateData == AVIB_STS_RANGEOV )
			{
				pv->var.State = AVIBSUP_TRGWAIT;
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case AVIBSUP_COMP: 													/* Comp State			*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.data == TRUE )
			{
				OPEREG_WRITE( pv->var.OpeSetIdx, AVIBSUPCMD_EXE1, Hmsg );
				pv->var.State = AVIBSUP_SET;
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case AVIBSUP_SET: 													/* Set State			*/
/*--------------------------------------------------------------------------------------------------*/
			pv->var.Match = TRUE;
			if( 0 /* lrc != OK */ )
			{
				pBlinkFlg->error = TRUE;
			}
			pv->var.State = AVIBSUP_GAINTUNE;
			break;
/*--------------------------------------------------------------------------------------------------*/
		case AVIBSUP_FREQTUNE: 												/* Freqtune State		*/
/*--------------------------------------------------------------------------------------------------*/
			tmp = 1;
			for ( i = 0; (ULONG)i < pv->var.DPos; i++ )
			{
				tmp = tmp *  10;
			}
			if( OpKeyMake.data == TRUE )
			{
				if( pv->var.Match == FALSE )
				{
					pv->var.State = AVIBSUP_SET;
				}
				else
				{
					/* EEPROMに記入する判定 */
					if ( OPEREG_WRITE( pv->var.OpeSetIdx, AVIBSUPCMD_PRMPRESERVE, Hmsg ) != OK )
					{
						pBlinkFlg->error = TRUE;
					}
					else
					{
						pBlinkFlg->done = TRUE;
					}
				}
			}
			else if( OpKeyMake.shup == TRUE )
			{
				if( pv->var.DPos < ( AVIBFTUN_MAX - 1U ) )
				{
					pv->var.DPos++;						/* 桁移動	*/
				}
				else
				{
					pv->var.DPos = 0U;
				}
			}
			else if( OpKeyMake.shdown == TRUE )
			{
				if( ( pv->var.DPos > 0U ) && ( pv->var.DPos < AVIBFTUN_MAX ) )
				{
					pv->var.DPos--;						/* 桁移動	*/
				}
				else
				{
					pv->var.DPos = ( AVIBFTUN_MAX - 1U );
				}
			}
			else if( OpKeyMake.up == TRUE )
			{
				data = pv->var.ResAvibFreqData + (ULONG)tmp;
				data = data * 10;
				pv->var.ResAvibFreqData = TunePrmLimitUnsigned( data, &pndef_avibfrq ) / 10;
					OPEREG_WRITE( pv->var.OpeAvibFreqIdx, pv->var.ResAvibFreqData, Hmsg );
				OPEREG_WRITE( pv->var.OpeSetIdx, AVIBSUPCMD_EXE1, Hmsg );
			}
			else if( OpKeyMake.down == TRUE )
			{
				data = pv->var.ResAvibFreqData - (ULONG)tmp;
				data = data * 10;
				pv->var.ResAvibFreqData = TunePrmLimitUnsigned( data, &pndef_avibfrq ) / 10;
				OPEREG_WRITE( pv->var.OpeAvibFreqIdx, pv->var.ResAvibFreqData, Hmsg );
				OPEREG_WRITE( pv->var.OpeSetIdx, AVIBSUPCMD_EXE1, Hmsg );
			}
			else if( OpKeyMake.scroll == TRUE )
			{
				pv->var.DPos = 0U;
				pv->var.State = AVIBSUP_GAINTUNE;
			}
			else 
			{
				/* do nothing */
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case AVIBSUP_GAINTUNE: 												/* Gaintune State		*/
/*--------------------------------------------------------------------------------------------------*/
			tmp = 1;
			for ( i = 0; (ULONG)i < pv->var.DPos; i++ )
			{
				tmp = tmp *  10;
			}
			if ( pv->var.DPos >= AVIBGTUN_MAX )
			{
				pv->var.DPos = AVIBGTUN_MAX - 1U;
			}
			if( OpKeyMake.data == TRUE )
			{
				if( pv->var.Match == FALSE )
				{
					pv->var.State = AVIBSUP_SET;
				}
				else
				{
					/* EEPROMに記入する判定 */
					if ( OPEREG_WRITE( pv->var.OpeSetIdx, AVIBSUPCMD_PRMPRESERVE, Hmsg ) != OK )
					{
						pBlinkFlg->error = TRUE;
					}
					else
					{
						pBlinkFlg->done = TRUE;
					}
				}
			}
			else if( OpKeyMake.shup == TRUE )
			{
				if( pv->var.DPos < ( AVIBGTUN_MAX - 1U ) )
				{
					pv->var.DPos++;					/* 桁移動	*/
				}
				else
				{
					pv->var.DPos = 0U;
				}
			}
			else if( OpKeyMake.shdown == TRUE )
			{
				if( ( pv->var.DPos > 0U ) && ( pv->var.DPos < AVIBGTUN_MAX ) )
				{
					pv->var.DPos--;					/* 桁移動	*/
				}
				else
				{
					pv->var.DPos = AVIBGTUN_MAX - 1U;
				}
			}
			else if( OpKeyMake.up == TRUE )
			{
				data = pv->var.ResAvibGainData + (ULONG)tmp;
				pv->var.ResAvibGainData = TunePrmLimitUnsigned( data, &pndef_avibdamp );
				OPEREG_WRITE( pv->var.OpeAvibGainIdx, pv->var.ResAvibGainData, Hmsg );
				OPEREG_WRITE( pv->var.OpeSetIdx, AVIBSUPCMD_EXE1, Hmsg );
			}
			else if( OpKeyMake.down == TRUE )
			{
				data = pv->var.ResAvibGainData - (ULONG)tmp;
				pv->var.ResAvibGainData = TunePrmLimitUnsigned( data, &pndef_avibdamp );
				OPEREG_WRITE( pv->var.OpeAvibGainIdx, pv->var.ResAvibGainData, Hmsg );
				OPEREG_WRITE( pv->var.OpeSetIdx, AVIBSUPCMD_EXE1, Hmsg );
			}
			else if( OpKeyMake.scroll == TRUE )
			{
				pv->var.DPos = 0U;
				pv->var.State = AVIBSUP_FREQTUNE;
			}
			else
			{
				/* do nothing */
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case AVIBSUP_END: 													/* End State			*/
/*--------------------------------------------------------------------------------------------------*/
			FunAtypeVibTuningEnd( Fmsg );
			rc = FUN_END;
			break;
		default:
			break;
		}
	return (rc);
}


/***************************************** end of file **********************************************/
