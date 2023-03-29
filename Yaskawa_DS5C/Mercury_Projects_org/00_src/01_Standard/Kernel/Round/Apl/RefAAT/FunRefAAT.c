/*****************************************************************************
Description		:Fn202  Reference Input Type Advanced Auto Tuning Mode
-----------------------------------------------------------------------------
Author		: SHCORE,
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

Functions	  :
	-- APIs --
	FunRefInRefInAdvAutoTuningBegin();					:Fn202  Reference Input Type Advanced Auto Tuning Mode Begin
	FunRefInAdvAutoTuningMain();						:Fn202  Reference Input Type Advanced Auto Tuning Mode Display
	FunRefInAdvAutoTuningEnd();							:Fn202  Reference Input Type Advanced Auto Tuning Mode End

	-- Locals --
	LpxRefInAdvAutoTuningExe();							:Fn202  Reference Input Type Advanced Auto Tuning Mode Execution
	RefTuningModeSet();									:Fn202  Reference Input Type Advanced Auto Tuning Mode Set
	RefAdatJratSet();									:Fn202 Reference Input Type Advanced Auto Tuning Jrat Set
	RefAdatLcdDisplay();								:Fn202 Reference Input Type Advanced Auto Tuning Mode LCD Display
	RefAdatStartWaitingDisplay();						:Fn202 Reference Input Type Advanced Auto Tuning Mode Waiting Display
	RefAdatModeSelectDisplay();							:Fn202 Reference Input Type Advanced Auto Tuning Mode Select Display
	RefAdatJSetDisplay();								:Fn202 Reference Input Type Advanced Auto Tuning Mode JSet Display
	RefAdatGainTuneDisplay();							:Fn202 Reference Input Type Advanced Auto Tuning Mode GainTune Display
	RefAdatFinishOpe();									:Fn202 Reference Input Type Advanced Auto Tuning Mode Finish Operator

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
ZH.QIN		2013.06.07		created
SL.Xue		2013.12.12		AdatRefInExeModeExeCheck処理を修正した
I.M			2014.02.19		Modify <S195>

*****************************************************************************/

#include "FunRefAAT.h"

/*--------------------------------------------------------------------------------------------------*/
/* Operation Register 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define OPEMODE_REG1			(0x0000)		/* 1st Operation Mode Register */
#define OPESET_REG1				(0x0001)		/* 1st Operation Mode Set Register */
#define OPEAATLVL_REG1			(0x0091)		/* 0x091: アドバンストＡＴレベル */
#define OPEAATSTS_REG1			(0x0094)		/* 0x094: アドバンストＡＴ完了確認 */
#define OPEAATFILTYPE_REG1		(0x0095)		/* 0x095: アドバンストＡＴフィルタタイプ */
/*--------------------------------------------------------------------------------------------------*/
/* Parameter Register 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define PNLOOPHZ_REG1			(0x0100)		/* Pn100 速度ループゲイン	 */
#define	PNPITIME_REG1			(0x0101)		/* Pn101 速度ループ積分時定数 */
#define	PNPOSGN_REG1			(0x0102)		/* Pn102 位置ループゲイン		*/
#define	PNMDLGN_REG1			(0x0141)		/* モデル追従制御ゲイン*/
/*--------------------------------------------------------------------------------------------------*/
/* Operation Mode	 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define FNNORMAL_MODE			(0x0000U) 		/* Normal Mode */
#define FNREFAAT_MODE			(0x1202U) 		/* Reference Input Type Advanced Auto Tuning Mode 	*/
/*--------------------------------------------------------------------------------------------------*/
/* Operation Command 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define REF_AAT_STOP 			(0x0)			/*stop command */
#define REF_AAT_EXE 			(0x1)			/*execution command*/
#define REF_AAT_WRITEPRM		(0x7)			/*write parameter command */
/*--------------------------------------------------------------------------------------------------*/
#define FUN_CONTINUE			0L				/*Continue				*/
#define FUN_END					1L				/*End					*/
/*--------------------------------------------------------------------------------------------------*/
#define REF_AAT_SCROLL_MIN		0L				/* 内部指令AAT設定画面スクロール下限				*/
#define REF_AAT_SCROLL_MAX		1L				/* 内部指令AAT設定画面スクロール上限				*/
#define REF_AAT_MODE_MIN		1U				/* モード設定：調整モード最小値						*/
#define REF_AAT_MODE_MAX		3U				/* モード設定：調整モード最大値						*/
#define REF_AAT_TYPE_MIN		1U				/* Type設定：調整モード最小値						*/
#define REF_AAT_TYPE_MAX		3U				/* Type設定：調整モード最小値						*/
/*--------------------------------------------------------------------------------------------------*/
#define	AAT_STS_DONE			0x01		/* チューニング完了										*/
/*--------------------------------------------------------------------------------------------------*/
#define	AAT_LEVEL_NOR			1U			/* 調整レベル＝標準										*/
#define	AAT_LEVEL_POS			2U			/* 調整レベル＝位置決め専用								*/
#define	AAT_LEVEL_POS_ST		3U			/* 調整レベル＝位置決め専用(オーバーシュート重視)		*/
/*--------------------------------------------------------------------------------------------------*/
#define Ref_AAT_STS_COINNOWAIT	0x0000		/* COIN待ちなしステータス								*/
#define	Ref_AAT_STS_COINERR		0x0100		/* COIN待ちエラーステータス								*/
#define	Ref_AAT_STS_COINWAIT	0x0200		/* COIN待ち中ステータス									*/
#define	Ref_AAT_STS_COINWARN	0x0300		/* COIN待ち中解除ステータス								*/

/*--------------------------------------------------------------------------------------------------*/
/* 			Mode Select 																			*/
/*--------------------------------------------------------------------------------------------------*/
enum _enumFnRefAATMode
{
	AAT_LVL = 0L,
	AAT_TYPE		= 1L
};

/*--------------------------------------------------------------------------------------------------*/
/* 			Jrat State																				*/
/*--------------------------------------------------------------------------------------------------*/
enum _enumFnRefAATJratSts
{
	JRAT_STS_DRVWAIT,						/* 初期設定完了 入力待ち								*/
	JRAT_STS_END
};

/*--------------------------------------------------------------------------------------------------*/
#define OPEREG_WRITE( idx, data, hmsg )   (COPEREG->Write( COPEREG, (idx), (data), hmsg ))
#define STSREG_READX( idx, pResData, CheckType, hmsg )   (COPEREG->Read( COPEREG, (idx), (pResData), \
																		(CheckType), hmsg ))   /* <S073> */
#define PRMREG_READX( idx, pResData, CheckType, hmsg )    (CPARAMX->Read( (COBJECT *)(CPARAMX),(idx),\
																	(pResData), (CheckType), hmsg ))  /* <S073> */


/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
/* APIs */
LONG FunRefInAdvAutoTuningBegin(FUNMSG *Fmsg);						/* Fn202 Reference Input Type Advanced Auto Tuning Mode Begin		*/
LONG FunRefInAdvAutoTuningMain(FUNMSG *Fmsg);						/* Fn202 Reference Input Type Advanced Auto Tuning Mode Main		*/
void FunRefInAdvAutoTuningEnd(FUNMSG *Fmsg);						/* Fn202 Reference Input Type Advanced Auto Tuning Mode End			*/

/* Locals */
static LONG LpxRefInAdvAutoTuningExe(FUNMSG *Fmsg);					/* Fn202 Reference Input Type Advanced Auto Tuning Mode Exe	*/
static LONG	RefAdatTuningModeSet( FUNMSG *Fmsg );					/* Fn202 Reference Input Type Advanced Auto Tuning Mode Set*/
static void	RefAdatLcdDisplay( FUNMSG *Fmsg );						/* Fn202 Reference Input Type Advanced Auto Tuning Mode LCD Display*/
static void	RefAdatStartWaitingDisplay( FUNMSG *Fmsg );				/* Fn202 Reference Input Type Advanced Auto Tuning Mode Waiting Display*/
static void	RefAdatModeSelectDisplay( FUNMSG *Fmsg );				/* Fn202 Reference Input Type Advanced Auto Tuning Mode Select Display*/
static void	RefAdatJSetDisplay( FUNMSG *Fmsg );						/* Fn202 Reference Input Type Advanced Auto Tuning Mode JSet Display*/
static void	RefAdatGainTuneDisplay( FUNMSG *Fmsg );					/* Fn202 Reference Input Type Advanced Auto Tuning Mode GainTune Display*/
static LONG	RefAdatFinishOpe( FUNMSG *Fmsg );						/* Fn202 Reference Input Type Advanced Auto Tuning Mode Finish Operator*/
#if  SVFSEL_PANELOP == 0
#define LpxPnlRefInAdvAutoTuningExe( x ) (0)
#else
LONG LpxPnlRefInAdvAutoTuningExe(FUNMSG *hMsg);						/* Panel Operator Fn202 Execution 		*/
#endif


/*****************************************************************************
  Description: Fn202 Reference Input Type Advanced Auto Tuning Mode Main
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	LONG			0: continue, 1: end 
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
LONG FunRefInAdvAutoTuningMain(FUNMSG *Fmsg)
{
MSGIF *hMsg = Fmsg->Super;
LONG rc;
rc = FUN_CONTINUE;
/*--------------------------------------------------------------------------------------------------*/
	if( KPI_CHK_LopHmsg( hMsg ) == TRUE )							/* LCD Operator */
	{
		rc = LpxRefInAdvAutoTuningExe( Fmsg );
	}
/*--------------------------------------------------------------------------------------------------*/
	else if ( KPI_CHK_PopHmsg( hMsg ) == TRUE )						/* Panel Operator */
	{
		rc = LpxPnlRefInAdvAutoTuningExe( hMsg );
	}
	else
	{
		/*do nothing	*/
	}
/*--------------------------------------------------------------------------------------------------*/
	return rc;
}

/*****************************************************************************
  Description: Fn202 Reference Input Type Advanced Auto Tuning Mode Begin
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	LONG			OK: Success, Others: Failure 
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
LONG FunRefInAdvAutoTuningBegin(FUNMSG *Fmsg)
{
LONG	rc = OK;
BOOL ResFlag = TRUE;
FNREFAATV  *pv = (FNREFAATV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	pv->var.State = REF_AAT_SEQ_INIT;
	pv->var.ModeSetPtr = REF_AAT_SCROLL_MIN;
	pv->var.JratSetPtr = JRAT_STS_DRVWAIT;
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
	if( ResFlag == TRUE )
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
	if( ResFlag == TRUE )
	{
		rc = COPEREG->Search( COPEREG,						/* Search Register (AAT Level 0x2091 Command)	*/
			OPEAATLVL_REG1,
			0,
			&(pv->var.AdatLevelIdx),
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
		rc = COPEREG->Search( COPEREG,						/* Search Register (AAT Type 0x2095 Command)	*/
				OPEAATFILTYPE_REG1,
			0,
			&(pv->var.AdatFilTypeIdx),
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
		rc = COPEREG->Search( COPEREG,						/* Search Register (AAT State 0x2094 Command)	*/
				OPEAATSTS_REG1,
			0,
			&(pv->var.AdatStateIdx),
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
		rc = CPARAMX->Search( (COBJECT *)CPARAMX,			/* Search Register ( Loophz Command)	*/
				PNLOOPHZ_REG1,
				0,
				&(pv->var.LoophzIdex),
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
		rc = CPARAMX->Search( (COBJECT *)CPARAMX,			/* Search Register (Pitime Command)	*/
				PNPITIME_REG1,
				0,
				&(pv->var.PitimeIdex),
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
		rc = CPARAMX->Search( (COBJECT *)CPARAMX,			/* Search Register (Posgn Command)	*/
				PNPOSGN_REG1,
				0,
				&(pv->var.PosgnIdex),
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
		rc = CPARAMX->Search( (COBJECT *)CPARAMX,			/* Search Register (Mdlgn Command)	*/
				PNMDLGN_REG1,
				0,
				&(pv->var.MdlgnIdex),
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
		rc = OPEREG_WRITE( pv->var.OpeModeIdx, FNREFAAT_MODE, Hmsg );
	}
	return (rc);
}

/*****************************************************************************
  Description:	Fn202 Reference Input Type Advanced Auto Tuning Mode End
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	None
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
void FunRefInAdvAutoTuningEnd(FUNMSG *Fmsg)
{
FNREFAATV  *pv = (FNREFAATV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------*/
	LpxPrmReChangeOpeDspMode();				/* デジオペ表示設定を元に戻す		*///<S197>
/*--------------------------------------------------------------------------*/
	OPEREG_WRITE( pv->var.OpeSetIdx, REF_AAT_STOP, Hmsg );
	OPEREG_WRITE( pv->var.OpeModeIdx, FNNORMAL_MODE, Hmsg );
}


/*****************************************************************************
  Description:	Fn202 Reference Input Type Advanced Auto Tuning Mode Exe
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	LONG			FUN_CONTINUE: Execution Continue, FUN_END: Execution End
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static LONG LpxRefInAdvAutoTuningExe(FUNMSG *Fmsg)
{
FNREFAATV *pv = (FNREFAATV *)(Fmsg->pVar);
OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
MSGIF *Hmsg = Fmsg->Super;
LONG rc = FUN_CONTINUE;														/* 0: continue, 1: end */
BLINKFLG *pBlinkFlg = (BLINKFLG *)(Fmsg->pBlnkKick);
ADATV	*AdatV;
AdatV = &(Fmsg->Super->pAxRsc->AdatHndl->AdatV);
/*--------------------------------------------------------------------------------------------------*/
		if( AdatV->f.DrvErrDetect == TRUE )
		{
			if( AdatV->ErrState <= ADAT_NG  )
			{
				pv->var.State = REF_AAT_SEQ_END;
			}
		}

		if ( OpKeyMake.mode == TRUE )
		{
			pv->var.State = REF_AAT_SEQ_RETURN;
		}

/*--------------------------------------------------------------------------------------------------*/
/*		Reference Input Type Advanced Auto Tuning Mode Sequence										*/
/*--------------------------------------------------------------------------------------------------*/
		switch( pv->var.State )
		{
/*--------------------------------------------------------------------------------------------------*/
		case REF_AAT_SEQ_INIT :												/* 初期状態 			*/
/*--------------------------------------------------------------------------------------------------*/
			pv->var.State = REF_AAT_SEQ_MODESET;
			break;
/*--------------------------------------------------------------------------------------------------*/
		case REF_AAT_SEQ_MODESET :									/* チューニングモード設定状態	*/
/*--------------------------------------------------------------------------------------------------*/
			rc = RefAdatTuningModeSet( Fmsg );
			break;
/*--------------------------------------------------------------------------------------------------*/
		case REF_AAT_SEQ_JRATSET :									/* イナーシャ設定状態			*/
/*--------------------------------------------------------------------------------------------------*/
			if ( OpKeyMake.up || OpKeyMake.down)
			{
				if(OK == OPEREG_WRITE( pv->var.OpeSetIdx, REF_AAT_EXE , Hmsg ))
				{
					pv->var.State = REF_AAT_TUNNING_WAIT;
				}
				else
				{
					pBlinkFlg->no_op = TRUE;
				}
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case REF_AAT_TUNNING_WAIT:									/* チューニング待ち完了状態		*/
/*--------------------------------------------------------------------------------------------------*/
			STSREG_READX( pv->var.AdatStateIdx, (ULONG *)(&pv->var.AdatState), MCHK_REG_READ, Hmsg );
			if ( pv->var.AdatState == AAT_STS_DONE )
			{
				pv->var.State = REF_AAT_SEQ_ENDDISP;
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case REF_AAT_SEQ_ENDDISP:									/* 終了表示状態					*/
/*--------------------------------------------------------------------------------------------------*/
			pBlinkFlg->end = TRUE;
			pv->var.State = REF_AAT_SEQ_END;
			break;
/*--------------------------------------------------------------------------------------------------*/
		case REF_AAT_SEQ_END:										/* 最終設定状態					*/
/*--------------------------------------------------------------------------------------------------*/
			rc = RefAdatFinishOpe( Fmsg );
			break;
/*--------------------------------------------------------------------------------------------------*/
		case REF_AAT_SEQ_WAIT:										/* 終了待ち状態					*/
/*--------------------------------------------------------------------------------------------------*/
			break;
/*--------------------------------------------------------------------------------------------------*/
		case REF_AAT_SEQ_RETURN:									/* 後処理状態					*/
/*--------------------------------------------------------------------------------------------------*/
			FunRefInAdvAutoTuningEnd( Fmsg );
			rc = FUN_END;
			break;
		}
		RefAdatLcdDisplay( Fmsg );
	return (rc);
}

/*****************************************************************************
  Description:	Fn202 Reference Input Type Advanced Auto Tuning Mode Set
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	LONG			FUN_CONTINUE: Execution Continue
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static LONG	RefAdatTuningModeSet( FUNMSG *Fmsg )
{
	OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
	FNREFAATV *pv = (FNREFAATV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;

	STSREG_READX( pv->var.AdatModeIdx, &(pv->var.AdatMode), MCHK_REG_READ, Hmsg );
	STSREG_READX( pv->var.AdatLevelIdx, &(pv->var.AdatLevel), MCHK_REG_READ, Hmsg );
	STSREG_READX( pv->var.AdatFilTypeIdx, &(pv->var.AdatFilType), MCHK_REG_READ, Hmsg );
	if ( OpKeyMake.data == TRUE )
	{
		pv->var.State = REF_AAT_SEQ_JRATSET;
		pv->var.DPos = 0;
	}
	else if ( OpKeyMake.scroll == TRUE )
	{
		if ( pv->var.ModeSetPtr > REF_AAT_SCROLL_MIN )
		{
			pv->var.ModeSetPtr --;
		}
		else
		{
			pv->var.ModeSetPtr = REF_AAT_SCROLL_MAX;
		}
		pv->var.DPos = 0;
	}
	else
	{
		/* do nothing	*/
	}
	switch ( pv->var.ModeSetPtr )
	{
		case AAT_LVL:
			if ( OpKeyMake.up == TRUE)
			{
				if ( pv->var.AdatLevel < REF_AAT_MODE_MAX )
				{
					pv->var.AdatLevel ++;
					OPEREG_WRITE( pv->var.AdatLevelIdx, pv->var.AdatLevel, Hmsg );
				}
			}
			else if ( OpKeyMake.down == TRUE )
			{
				if ( pv->var.AdatLevel > REF_AAT_MODE_MIN)
				{
					pv->var.AdatLevel --;
					OPEREG_WRITE( pv->var.AdatLevelIdx, pv->var.AdatLevel, Hmsg );
				}
			}
			else
			{
				/* do nothing	*/
			}
		break;
		case AAT_TYPE:
			if ( OpKeyMake.up == TRUE )
			{
				if ( pv->var.AdatFilType < REF_AAT_TYPE_MAX )
				{
					pv->var.AdatFilType ++;
					OPEREG_WRITE( pv->var.AdatFilTypeIdx, pv->var.AdatFilType, Hmsg );
				}
			}
			else if ( OpKeyMake.down == TRUE)
			{
				if (pv->var.AdatFilType > REF_AAT_TYPE_MIN)
				{
					pv->var.AdatFilType --;
					OPEREG_WRITE( pv->var.AdatFilTypeIdx, pv->var.AdatFilType, Hmsg );
				}
			}
			else
			{
				/* do nothing  */

			}
		break;
	}
	return FUN_CONTINUE;
}


/*****************************************************************************
  Description:	Fn202 Reference Input Type Advanced Auto Tuning Mode LCD Display
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	None
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void	RefAdatLcdDisplay( FUNMSG *Fmsg )
{
FNREFAATV *pv = (FNREFAATV *)(Fmsg->pVar);
UCHAR *ResBuf = Fmsg->pTxBuf;

	/* Clear	*/
	RpxLcdClrTxBuf( ResBuf );
		/* TITLE	*/
//	RpxLcdSetStr( 0U, 6U, (UCHAR *)"Advanced AT", ResBuf );		/* <S154> */
	RpxLcdSetStr( 0U, 14U, (UCHAR *)"AAT", ResBuf );			/* <S154> */

	RpxLcdSetStr( 1U, 0U, (UCHAR *)"                 ", ResBuf );
	RpxLcdSetStr( 2U, 0U, (UCHAR *)"                 ", ResBuf );
	RpxLcdSetStr( 3U, 0U, (UCHAR *)"                 ", ResBuf );
	RpxLcdSetStr( 4U, 0U, (UCHAR *)"                 ", ResBuf );
	RpxLcdClrLineBlnk( 0U, ResBuf );
	RpxLcdClrLineBlnk( 1U, ResBuf );
	RpxLcdClrLineBlnk( 2U, ResBuf );
	RpxLcdClrLineBlnk( 3U, ResBuf );
	RpxLcdClrLineBlnk( 4U, ResBuf );

		switch ( pv->var.State )
		{
			case REF_AAT_SEQ_INIT :
				RefAdatStartWaitingDisplay( Fmsg );
				break;

			case REF_AAT_SEQ_MODESET :
				RefAdatModeSelectDisplay( Fmsg );
				break;

			case REF_AAT_SEQ_JRATSET :
				RefAdatJSetDisplay( Fmsg );
				break;

			case REF_AAT_TUNNING_WAIT:
				RpxLcdSetBlnk( 0U, 0U, 5U, ResBuf );
				/*fall through*/
			case REF_AAT_SEQ_ENDDISP:
				/*fall through*/
			case REF_AAT_SEQ_END:
				/*fall through*/
			case REF_AAT_SEQ_WAIT:
				/*fall through*/
			case REF_AAT_SEQ_RETURN:
				RefAdatGainTuneDisplay( Fmsg );
				break;
			default:
				break;
		}
		return;
}
/*****************************************************************************
  Description:	Fn202 Reference Input Type Advanced Auto Tuning Mode Waiting Display
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	None
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void	RefAdatStartWaitingDisplay( FUNMSG *Fmsg )
{
	UCHAR *ResBuf = Fmsg->pTxBuf;
	/* TITLE	*/
	RpxLcdSetStr( 2U, 0U, (UCHAR *)" Motor Stop Wait ", ResBuf );
	RpxLcdSetBlnk( 2U, 0U, 17U, ResBuf );
	return;
}

/*****************************************************************************
  Description:	Fn202 Reference Input Type Advanced Auto Tuning Mode Select Display
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	None
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void	RefAdatModeSelectDisplay( FUNMSG *Fmsg )
{
FNREFAATV *pv = (FNREFAATV *)(Fmsg->pVar);
UCHAR *ResBuf = Fmsg->pTxBuf;
		/* TITLE	*/
	RpxLcdSetStr( 2U, 0U, (UCHAR *)" Mode=   Type=   ", ResBuf );
	/* Tuning Level	*/
	RpxLcdSetDecPnt( 2U, 6U, (ULONG) pv->var.AdatLevel, 1U, (USHORT)FALSE, 0, ResBuf );
	/* Tuning Type	*/
	RpxLcdSetDecPnt( 2U, 14U, (ULONG) pv->var.AdatFilType, 1U, (USHORT)FALSE, 0, ResBuf );
	/* カーソル点滅設定	*/
	switch ( pv->var.ModeSetPtr )
	{
		case AAT_LVL:
			RpxLcdSetBlnk( 2U, 6U, 1U, ResBuf );
			break;

		case AAT_TYPE:
			RpxLcdSetBlnk( 2U, 14U, 1U, ResBuf );
			break;

		default:
			break;
	}
	return;
}

/*****************************************************************************
  Description:	Fn202 Reference Input Type Advanced Auto Tuning Mode JSet Display
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	None
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void	RefAdatJSetDisplay( FUNMSG *Fmsg )
{
FNREFAATV *pv = (FNREFAATV *)(Fmsg->pVar);
UCHAR *ResBuf = Fmsg->pTxBuf;
MSGIF *Hmsg = Fmsg->Super;
ADATV *AdatV;
AdatV = &(Fmsg->Super->pAxRsc->AdatHndl->AdatV);

	/* TITLE	*/
	RpxLcdSetStr( 1U, 0U, (UCHAR *)"Pn103=", ResBuf );
	RpxLcdSetStr( 2U, 0U, (UCHAR *)"Pn100=", ResBuf );
	RpxLcdSetStr( 3U, 0U, (UCHAR *)"Pn101=", ResBuf );

	/* Gain	*/
	switch ( AdatV->JratErr )
	{
		case ADAT_JRAT_OK :
			RpxLcdSetDecPnt( 1U, 6U, AdatV->Jrate, 5U, FALSE, 0U, ResBuf );
			break;

		case ADAT_JRAT_ERR1 :
		case ADAT_JRAT_ERR2 :
		case ADAT_JRAT_ERR3 :
		case ADAT_JRAT_ERR4 :
		case ADAT_JRAT_ERR5 :
			RpxLcdSetStr( 1U, 6U, (UCHAR *)"Err", ResBuf );
			RpxLcdSetDecPnt( 1U, 9U, (ULONG)AdatV->JratErr, 1U, FALSE, 0U, ResBuf );
			RpxLcdSetBlnk( 1U, 6U, 4U, ResBuf );
			break;

		default :
			break;
	}
	PRMREG_READX( pv->var.LoophzIdex, &(pv->var.DispGain), MCHK_REG_READ, Hmsg );
	RpxLcdSetDecPnt( 2U, 6U, pv->var.DispGain, 5U, (USHORT)FALSE, 1, ResBuf );
	PRMREG_READX( pv->var.PitimeIdex, &(pv->var.DispGain), MCHK_REG_READ, Hmsg );
	RpxLcdSetDecPnt( 3U, 6U, pv->var.DispGain, 5U, (USHORT)FALSE, 2, ResBuf );

	if ( ( pv->var.AdatLevel == AAT_LEVEL_POS ) || ( pv->var.AdatLevel == AAT_LEVEL_POS_ST ) )
	{
		RpxLcdSetStr( 4U, 0U, (UCHAR *)"Pn141=", ResBuf );
		PRMREG_READX( pv->var.MdlgnIdex, &(pv->var.DispGain), MCHK_REG_READ, Hmsg );
		RpxLcdSetDecPnt( 4U, 6U, pv->var.DispGain, 5U, (USHORT)FALSE, 1, ResBuf );
	}
	else
	{
		RpxLcdSetStr( 4U, 0U, (UCHAR *)"Pn102=", ResBuf );
		PRMREG_READX( pv->var.PosgnIdex, &(pv->var.DispGain), MCHK_REG_READ, Hmsg );
		RpxLcdSetDecPnt( 4U, 6U, pv->var.DispGain, 5U, (USHORT)FALSE, 1, ResBuf );
	}
	return;
}
/*****************************************************************************
  Description:	Fn202 Reference Input Type Advanced Auto Tuning Mode GainTune Display
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	None
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void	RefAdatGainTuneDisplay( FUNMSG *Fmsg  )
{
UCHAR *ResBuf = Fmsg->pTxBuf;
FNREFAATV *pv = (FNREFAATV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
ADATV *AdatV;
AdatV = &(Fmsg->Super->pAxRsc->AdatHndl->AdatV);

	/* TITLE	*/
	RpxLcdSetStr( 1U, 0U, (UCHAR *)"Pn103=", ResBuf );
	RpxLcdSetStr( 2U, 0U, (UCHAR *)"Pn100=", ResBuf );
	RpxLcdSetStr( 3U, 0U, (UCHAR *)"Pn101=", ResBuf );

	/* Gain	*/
	switch ( AdatV->JratErr )
	{
		case ADAT_JRAT_OK :
			RpxLcdSetDecPnt( 1U, 6U, AdatV->Jrate, 5U, FALSE, 0U, ResBuf );
			break;

		case ADAT_JRAT_ERR1 :
		case ADAT_JRAT_ERR2 :
		case ADAT_JRAT_ERR3 :
		case ADAT_JRAT_ERR4 :
		case ADAT_JRAT_ERR5 :
			RpxLcdSetStr( 1U, 6U, (UCHAR *)"Err", ResBuf );
			RpxLcdSetDecPnt( 1U, 9U, (ULONG)AdatV->JratErr, 1U, FALSE, 0U, ResBuf );
			RpxLcdSetBlnk( 1U, 6U, 4U, ResBuf );
			break;

		default :
			break;
	}
	PRMREG_READX( pv->var.LoophzIdex, &(pv->var.DispGain), MCHK_REG_READ, Hmsg );
	RpxLcdSetDecPnt( 2U, 6U, pv->var.DispGain, 5U, (USHORT)FALSE, 1, ResBuf );
	PRMREG_READX( pv->var.PitimeIdex, &(pv->var.DispGain), MCHK_REG_READ, Hmsg );
	RpxLcdSetDecPnt( 3U, 6U, pv->var.DispGain, 5U, (USHORT)FALSE, 2, ResBuf );

	if ( ( pv->var.AdatLevel == ADAT_LEVEL_POS )							
			|| ( pv->var.AdatLevel == ADAT_LEVEL_POS_ST ) )					
	{
		RpxLcdSetStr( 4U, 0U, (UCHAR *)"Pn141=", ResBuf );
		PRMREG_READX( pv->var.MdlgnIdex, &(pv->var.DispGain), MCHK_REG_READ, Hmsg );
		RpxLcdSetDecPnt( 4U, 6U, pv->var.DispGain, 5U, (USHORT)FALSE, 1, ResBuf );
	}
	else
	{
		RpxLcdSetStr( 4U, 0U, (UCHAR *)"Pn102=", ResBuf );
		PRMREG_READX( pv->var.PosgnIdex, &(pv->var.DispGain), MCHK_REG_READ, Hmsg );
		RpxLcdSetDecPnt( 4U, 6U, pv->var.DispGain, 5U, (USHORT)FALSE, 1, ResBuf );
	}
	if ( ( AdatV->CoinWaitStat == Ref_AAT_STS_COINWAIT ) || ( AdatV->CoinWaitStat == Ref_AAT_STS_COINWARN ) )
	{	/* COIN待ち中警告出力	*/
		RpxLcdSetStr( 1U, 0U, (UCHAR *)"     WAITING     ", ResBuf );
		RpxLcdSetBlnk( 1U, 0U, 17U, ResBuf );
	}

	return;

}
/*****************************************************************************
  Description:	Fn202 Reference Input Type Advanced Auto Tuning Mode Finish Operator
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	LONG			FUN_CONTINUE: Execution Continue
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static LONG	RefAdatFinishOpe( FUNMSG *Fmsg )
{
FNREFAATV *pv = (FNREFAATV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
BLINKFLG *pBlinkFlg = (BLINKFLG *)(Fmsg->pBlnkKick);
ADATV	*AdatV;
AdatV = &(Fmsg->Super->pAxRsc->AdatHndl->AdatV);
	switch( AdatV->ErrState )
	{
	case ADAT_OK:											/* 正常終了							*/
	if ( OpKeyMake.data == TRUE)
	{
		OPEREG_WRITE( pv->var.OpeSetIdx, REF_AAT_WRITEPRM , Hmsg );
			pBlinkFlg->done = TRUE;
		pv->var.State = REF_AAT_SEQ_WAIT;
	}
		break;

	case ADAT_COMERR:										/* 動作中の通信エラー				*/
		pBlinkFlg->no_op = TRUE;
		pv->var.State = REF_AAT_SEQ_WAIT;					/* 現在のシーケンスポインタを保存	*/
		break;

	default:												/* その他のエラーの場合				*/
		pBlinkFlg->error = TRUE;
		pv->var.State = REF_AAT_SEQ_WAIT;					/* 現在のシーケンスポインタを保存	*/
		break;
	}
	return FUN_CONTINUE;
}

/***************************************** end of file **********************************************/

