/*****************************************************************************
Description		:Fn201  Advanced Auto Tuning Mode
-----------------------------------------------------------------------------
Author		: SHCORE,
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

Functions	  :
	-- APIs --
	FunAdvAutoTuningBegin();					:Fn201  Advanced Auto Tuning Mode Begin
	FunAdvAutoTuningMain();						:Fn201  Advanced Auto Tuning Mode Display
	FunAdvAutoTuningEnd();						:Fn201  Advanced Auto Tuning Mode End

	-- Locals --
	LpxAdvAutoTuningExe();						:Fn201  Advanced Auto Tuning Mode Execution
	AdatTuningModeSet();						:Fn201  Advanced Auto Tuning Mode Set
	AdatJratSet();								:Fn201  Advanced Auto Tuning Jrat Set
	AdatLcdDisplay();							:Fn201 Advanced Auto Tuning Mode LCD Display
	AdatStartWaitingDisplay();					:Fn201 Advanced Auto Tuning Mode Waiting Display
	AdatModeSelectDisplay();					:Fn201 Advanced Auto Tuning Mode Select Display
	AdatPcalRevFromDistance();
	AdatJSetDisplay();							:Fn201 Advanced Auto Tuning Mode JSet Display
	AdatGainTuneDisplay();						:Fn201 Advanced Auto Tuning Mode GainTune Display
	AdatFinishOpe();							:Fn201 Advanced Auto Tuning Mode Finish Operator

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
ZH.QIN		2013.06.07		created
K.Liu		2013.12.10		Modify
I.M			2014.02.19		Modify <S195>
*****************************************************************************/

#include "FunAAT.h"

/*--------------------------------------------------------------------------------------------------*/
/* Operation Register 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define OPEMODE_REG1		(0x0000)		/* 1st Operation Mode Register */
#define OPESET_REG1			(0x0001)		/* 1st Operation Mode Set Register */
#define OPETIME_REG1		(0x0008)		/* 安全停止時間[s] 		*/
#define OPEAATMODE_REG1		(0x0090)		/* 0x090: アドバンストＡＴモード */
#define OPEAATLVL_REG1		(0X0091)		/* 0x091: アドバンストＡＴレベル */
#define OPEAATMOV_REG1		(0X0092)		/* 0x092: アドバンストＡＴ移動距離 */
#define OPEAATSTS_REG1		(0X0094)		/* 0x094: アドバンストＡＴ完了確認 */
#define OPEAATFILTYPE_REG1	(0X0095)		/* 0x095: アドバンストＡＴフィルタタイプ */
/*--------------------------------------------------------------------------------------------------*/
/* Parameter Register 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define PNLOOPHZ_REG1		(0x0100)		/* Pn100 速度ループゲイン	 */
#define	PNPITIME_REG1		(0x0101)		/* Pn101 速度ループ積分時定数 */
#define	PNPOSGN_REG1		(0x0102)		/* Pn102 位置ループゲイン		*/
#define	PNJRATE_REG1		(0x0103)		/* Pn103 : 慣性モーメント比		*/
#define	PNMDLGN_REG1		(0x0141)		/* Pn141 モデル追従制御ゲイン*/
//#define	PNSYSSW1_REG1		(0x0E0B)		/* PnE0B : システムスイッチ１							*///<S197>
/*--------------------------------------------------------------------------------------------------*/
/* Operation Mode	 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define FNNORMAL_MODE		(0x0000U) 		/* Normal Mode */
#define FNAAT_MODE			(0x1201U) 		/* Advanced Auto Tuning Mode */
/*--------------------------------------------------------------------------------------------------*/
/* Monitor Register																					*/
/*--------------------------------------------------------------------------------------------------*/
#define MONINTMODESATE_REG1	(0x0705)		/* 内部動作状態											*///<S197>
/*--------------------------------------------------------------------------------------------------*/
/* Operation Command 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define AAT_STOP 			(0x0)			/*stop command */
#define AAT_FORWARD 		(0x2)			/*forward command */
#define AAT_REVERSE 		(0x3)			/*reverse command */
#define AAT_SVON 			(0x4)			/*servono command */
#define AAT_WRITEPRM		(0x7)			/*write parameter command */
/*--------------------------------------------------------------------------------------------------*/
#define FUN_CONTINUE		0L				/*Continue				*/
#define FUN_END				1L				/*End					*/
/*--------------------------------------------------------------------------------------------------*/
#define AAT_SCROLL_MIN		0L				/* 内部指令AAT設定画面スクロール下限					*/
#define AAT_SCROLL_MAX		3L				/* 内部指令AAT設定画面スクロール上限					*/
#define AAT_MODE_MIN		1U				/* モード設定：調整モード最小値							*/
#define AAT_MODE_MAX		3U				/* モード設定：調整モード最大値							*/
#define AAT_TYPE_MIN		1U				/* Type設定：調整モード最小値							*/
#define AAT_TYPE_MAX		3U				/* Type設定：調整モード最小値							*/
#define	AAT_DTUN_MAX		5				/* 移動距離調整桁										*/
#define	AAT_DIST_MAX		99990000		/* 移動距離最大値										*/
/*--------------------------------------------------------------------------------------------------*/
#define	AAT_STS_INIT		0x00			/* イニシャル/中断										*/
#define	AAT_STS_DONE		0x01			/* チューニング完了										*/
#define	AAT_STS_JERR		0x31			/* イナーシャ同定不良									*/
#define	AAT_STS_JEND		0x51			/* イナーシャ同定完了									*/
/*--------------------------------------------------------------------------------------------------*/
#define	AAT_LEVEL_NOR		1U				/* 調整レベル＝標準										*/
#define	AAT_LEVEL_POS		2U				/* 調整レベル＝位置決め専用								*/
#define	AAT_LEVEL_POS_ST	3U				/* 調整レベル＝位置決め専用(オーバーシュート重視)		*/
/*--------------------------------------------------------------------------------------------------*/
/* 			Mode Select 																			*/
/*--------------------------------------------------------------------------------------------------*/
enum _enumFnAATMode
{
	AAT_MODE = 0U,
	AAT_LVL 	= 1U,
	AAT_TYPE 	= 2U,
	AAT_MOV = 3U,
};
/*--------------------------------------------------------------------------------------------------*/
/* 			Jrat State																				*/
/*--------------------------------------------------------------------------------------------------*/
enum _enumFnAATJratSts
{
	JRAT_STS_DRVWAIT,
	JRAT_STS_RUNNING,
	JRAT_STS_JEND_WAIT,
	JRAT_STS_END
};

/*--------------------------------------------------------------------------------------------------*/
#define OPEREG_WRITE( idx, data, hmsg )   (COPEREG->Write( COPEREG, (idx), (data), hmsg ))
#define STSREG_READX( idx, pResData, CheckType, hmsg )   (COPEREG->Read( COPEREG, (idx), (pResData), \
																		(CheckType), hmsg )) /* <S073> */
#define PRMREG_READX( idx, pResData, CheckType, hmsg )    (CPARAMX->Read( (COBJECT *)(CPARAMX),(idx),\
																	(pResData), (CheckType), hmsg )) /* <S073> */

/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
/* APIs */
LONG FunAdvAutoTuningBegin(FUNMSG *Fmsg);				/* Fn201 Advanced Auto Tuning Mode Begin	*/
LONG FunAdvAutoTuningMain(FUNMSG *Fmsg);				/* Fn201 Advanced Auto Tuning Mode Main		*/
void FunAdvAutoTuningEnd(FUNMSG *Fmsg);					/* Fn201 Advanced Auto Tuning Mode End		*/

/* Locals */
static LONG LpxAdvAutoTuningExe(FUNMSG *Fmsg);			/* Fn201 Advanced Auto Tuning Mode Exe		*/
static void	AdatTuningInitial( FUNMSG *Fmsg );			/* Fn201 Advanced Auto Tuning Mode Initial	*/
static LONG	AdatTuningModeSet( FUNMSG *Fmsg );			/* Fn201 Advanced Auto Tuning Mode Set		*/
static void	AdatLcdDisplay( FUNMSG *Fmsg );				/* Fn201 Advanced Auto Tuning Mode LCD Display*/
static void	AdatStartWaitingDisplay( FUNMSG *Fmsg );	/* Fn201 Advanced Auto Tuning Mode Waiting Display */
static void	AdatModeSelectDisplay( FUNMSG *Fmsg );		/* Fn201 Advanced Auto Tuning Mode Select Display*/
static LONG	AdatPcalRevFromDistance( LONG distance, ADATP *AdatP );
static void	AdatJSetDisplay( FUNMSG *Fmsg );			/* Fn201 Advanced Auto Tuning Mode JSet Display */
static void	AdatGainTuneDisplay( FUNMSG *Fmsg );		/* Fn201 Advanced Auto Tuning Mode GainTune Display*/
static LONG	AdatFinishOpe( FUNMSG *Fmsg, SHORT err_status );/* Fn201 Advanced Auto Tuning Mode Finish Operator*/


/*****************************************************************************
  Description: Fn201 Advanced Auto Tuning Mode Main
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	LONG			0: continue, 1: end 
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
LONG FunAdvAutoTuningMain(FUNMSG *Fmsg)
{
MSGIF *hMsg = Fmsg->Super;
LONG rc;
rc = FUN_CONTINUE;
/*--------------------------------------------------------------------------------------------------*/
	if( KPI_CHK_LopHmsg( hMsg ) == TRUE )							/* LCD Operator */
	{
		rc = LpxAdvAutoTuningExe( Fmsg );
	}
/*--------------------------------------------------------------------------------------------------*/
	return rc;
}

/*****************************************************************************
  Description: Fn201 Advanced Auto Tuning Mode Begin
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	LONG			OK: Success, Others: Failure 
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
LONG FunAdvAutoTuningBegin(FUNMSG *Fmsg)
{
LONG	rc = OK;
BOOL ResFlag = TRUE;
FNAATV  *pv = (FNAATV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	pv->var.State = AAT_SEQ_INIT;
	pv->var.DrvErrDetect = FALSE;
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
	if( ResFlag == TRUE )
	{
		rc = COPEREG->Search( COPEREG,					/* Search Register (AAT Mode 0x2090  Command)	*/
			OPEAATMODE_REG1,
			0,
			&(pv->var.AdatModeIdx),
			MCHK_REG_READ,
			Hmsg);
		if( rc != OK )
		{
			ResFlag = FALSE;
		}

	}
	if( ResFlag == TRUE )
	{
		rc = COPEREG->Search( COPEREG,					/* Search Register (AAT Level 0x2091 Command)	*/
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
	if( ResFlag == TRUE )
	{
		rc = COPEREG->Search( COPEREG,					/* Search Register (AAT Movement 0x2092 Command)	*/
				OPEAATMOV_REG1,
			0,
			&(pv->var.AdatMovepIdx),
			MCHK_REG_READ,
			Hmsg);
		if( rc != OK )
		{
			ResFlag = FALSE;
		}

	}
	if( ResFlag == TRUE )
	{
		rc = COPEREG->Search( COPEREG,					/* Search Register (AAT Type 0x2095 Command)	*/
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
	if( ResFlag == TRUE )
	{
		rc = COPEREG->Search( COPEREG,					/* Search Register (AAT State 0x2094 Command)	*/
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
	if( ResFlag == TRUE )
	{
		rc = CPARAMX->Search( (COBJECT *)CPARAMX,			/* Search Register ( Pn100 Loophz Command)	*/
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
	if( ResFlag == TRUE )
	{
		rc = CPARAMX->Search( (COBJECT *)CPARAMX,			/* Search Register (Pn101  Pitime Command)	*/
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
	if( ResFlag == TRUE )
	{
		rc = CPARAMX->Search( (COBJECT *)CPARAMX,			/* Search Register (Pn102 Posgn Command)	*/
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
	if( ResFlag == TRUE )
	{
		rc = CPARAMX->Search( (COBJECT *)CPARAMX,			/* Search Register (Pn103 Jrate Command)	*/
				PNJRATE_REG1,
				0,
				&(pv->var.JrateIdex),
				MCHK_REG_READ,
				Hmsg);
		if( rc != OK )
		{
			ResFlag = FALSE;
		}
	}
	if( ResFlag == TRUE )
	{
		rc = CPARAMX->Search( (COBJECT *)CPARAMX,			/* Search Register (Pn141 Mdlgn Command)	*/
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
	if( ResFlag == TRUE )
	{
//		rc = CPARAMX->Search( (COBJECT *)CPARAMX,			/* Search Register (PnE0B Syssw1 Command)	*/
//				MONINTMODESATE_REG1
//				0,
//				&(pv->var.Syssw1Idex),
//				MCHK_REG_READ,
//				Hmsg);
		rc = CUNREGX->Search( (COBJECT *)CUNREGX,			/* Search Register (Internal Mode State E705)	*///<S197>
				MONINTMODESATE_REG1,
				0,
				&(pv->var.IntModeStateIdex),
				MCHK_REG_READ,
				Hmsg);
		if( rc != OK )
		{
			ResFlag = FALSE;
		}
	}
	if( ResFlag == TRUE )
	{
		rc = CPARAMX->Search( (COBJECT *)COPEREG,			/* Search Register (Time Command)			*/
				OPETIME_REG1,
				0,
				&(pv->var.TimeCntrIdex),
				MCHK_REG_READ,
				Hmsg);
		}
	if( rc == OK )
	{
		/* Set Advanced Auto Tuning Mode to Mode Register 		*/
		AdatTuningInitial(Fmsg);
		rc = OPEREG_WRITE( pv->var.OpeModeIdx, FNAAT_MODE, Hmsg );
	}
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}

/*****************************************************************************
  Description:	Fn201 Advanced Auto Tuning Mode End
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	None
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
void FunAdvAutoTuningEnd(FUNMSG *Fmsg)
{
FNAATV  *pv = (FNAATV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------*/
	LpxPrmReChangeOpeDspMode();				/* デジオペ表示設定を元に戻す		*///<S197>
/*--------------------------------------------------------------------------*/
	OPEREG_WRITE( pv->var.OpeSetIdx, AAT_STOP, Hmsg );
	OPEREG_WRITE( pv->var.OpeModeIdx, FNNORMAL_MODE, Hmsg );
	OPEREG_WRITE( pv->var.TimeCntrIdex, pv->var.SafeTime, Hmsg );

}

/*****************************************************************************
  Description:	Fn201 Advanced Auto Tuning Mode Exe
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	LONG			FUN_CONTINUE: Execution Continue, FUN_END: Execution End
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static LONG LpxAdvAutoTuningExe(FUNMSG *Fmsg)
{
FNAATV *pv = (FNAATV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
UCHAR *ResBuf = Fmsg->pTxBuf;
OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
LONG rc = FUN_CONTINUE;													/* 0: continue, 1: end */
ADATP	*AdatP = &(Fmsg->Super->pAxRsc->AdatHndl->AdatP);
ADATV *AdatV = &(Fmsg->Super->pAxRsc->AdatHndl->AdatV);
BLINKFLG *BlinkFlag = (BLINKFLG *)(Fmsg->pBlnkKick);

	RpxLcdClrTxBuf( ResBuf ); 					/* clear Tx buffer */
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

//	if( AdatV->f.DrvErrDetect == TRUE )
	if( pv->var.DrvErrDetect == TRUE )
	{
		if ( AdatV->ErrState <= ADAT_NG )
		{
			pv->var.State = AAT_SEQ_END;
		}
	}

		if ( OpKeyMake.mode == TRUE )
		{
			pv->var.State = AAT_SEQ_RETURN;
		}
		else if ( OpKeyMake.svon == TRUE )
		{
			if( ALMGetAlarmInfo( AdatP->AlmManager ) != 0x00 		/* アラームある 			*/
				|| ( AdatP->SeqCtrlOut->HwbbSts == TRUE) 				/* HWBBある				*/
				|| (AdatP->SeqCtrlOut->MainPowerOn == FALSE) 			/* 主回路OFF				*/
				|| (AdatP->SeqCtrlOut->OverTrvlSts == TRUE) )			/* OTある				*/
			{
				BlinkFlag->no_op = TRUE;
			}
			else
			{
//				if( AdatP->FnCmnCtrl->FnSvonReq != FALSE )
//				{
//					OPEREG_WRITE( pv->var.OpeSetIdx, AAT_SVON, Hmsg );
//					pv->var.State = AAT_SEQ_INIT;
//				}
			}
		}
		else
		{
			/*do nothing	*/
		}

/*--------------------------------------------------------------------------------------------------*/
/*		Advanced Auto Tuning Mode Sequence															*/
/*--------------------------------------------------------------------------------------------------*/
		switch( pv->var.State )
		{
		/*--------------------------------------------------------------------------------------*/
		/*	初期状態																			*/
		/*--------------------------------------------------------------------------------------*/
		case AAT_SEQ_INIT :
			AdatTuningInitial( Fmsg );
			pv->var.State = AAT_SEQ_MODESET;
			pv->var.DrvErrDetect = TRUE;
			OPEREG_WRITE( pv->var.OpeModeIdx, FNNORMAL_MODE, Hmsg );
			break;
		/*--------------------------------------------------------------------------------------*/
		/*	チューニングモード設定状態															*/
		/*--------------------------------------------------------------------------------------*/
		case AAT_SEQ_MODESET :
			rc = AdatTuningModeSet( Fmsg );
			AdatModeSelectDisplay( Fmsg );
			pv->var.SafeStopCmd = 0;

			break;
		/*--------------------------------------------------------------------------------------*/
		/*	イナーシャ設定状態																	*/
		/*--------------------------------------------------------------------------------------*/
		case AAT_SEQ_JRATSET :
			if(OpKeyMake.svon)
			{
				OPEREG_WRITE( pv->var.OpeSetIdx, AAT_SVON, Hmsg );
				if(pv->var.AdatMode != FALSE)
				{
					pv->var.State = AAT_TUNNING_WAIT;
				}

			}

			STSREG_READX( pv->var.AdatStateIdx, (ULONG *)(&pv->var.AdatState), MCHK_REG_READ, Hmsg );
			if ( pv->var.AdatState == ADAT_STS_JEND )
			{
				pv->var.SafeStopCmd = 0;
				if(OpKeyMake.data)
				{
					OPEREG_WRITE( pv->var.OpeSetIdx, AAT_WRITEPRM, Hmsg );
					BlinkFlag->done = TRUE;
					pv->var.State = AAT_TUNNING_WAIT;
				}
				else if(OpKeyMake.up)
				{
					pv->var.SafeStopCmd = AAT_FORWARD;
					OPEREG_WRITE( pv->var.OpeSetIdx, pv->var.SafeStopCmd, Hmsg );
					pv->var.State = AAT_TUNNING_WAIT;
				}
				else if(OpKeyMake.down)
				{
					pv->var.SafeStopCmd = AAT_REVERSE;
					OPEREG_WRITE( pv->var.OpeSetIdx, pv->var.SafeStopCmd, Hmsg );
					pv->var.State = AAT_TUNNING_WAIT;
				}
				else
				{
					;
				}
			}
			else
			{
				if(pv->var.SafeStopCmd == 0)
				{
					if(OpKeyMake.up)
					{
						pv->var.SafeStopCmd = AAT_FORWARD;
						OPEREG_WRITE( pv->var.OpeSetIdx, pv->var.SafeStopCmd, Hmsg );
					}
					else if(OpKeyMake.down)
					{
						pv->var.SafeStopCmd = AAT_REVERSE;
						OPEREG_WRITE( pv->var.OpeSetIdx, pv->var.SafeStopCmd, Hmsg );
					}
					else
					{
						;
					}
				}
				else
				{
					OPEREG_WRITE( pv->var.OpeSetIdx, pv->var.SafeStopCmd, Hmsg );
				}
			}
			break;

		case AAT_TUNNING_WAIT:
			STSREG_READX( pv->var.AdatStateIdx, (ULONG *)(&pv->var.AdatState), MCHK_REG_READ, Hmsg );
			if ( pv->var.AdatState == AAT_STS_DONE )
			{
				pv->var.State = AAT_SEQ_ENDDISP;
				BlinkFlag->end = TRUE;
			}
			else
			{
				if(OpKeyMake.svon)
				{
					OPEREG_WRITE( pv->var.OpeSetIdx, AAT_SVON, Hmsg );
				}
				if(pv->var.SafeStopCmd == 0)
				{
					if(OpKeyMake.up)
					{
						pv->var.SafeStopCmd = AAT_FORWARD;
						OPEREG_WRITE( pv->var.OpeSetIdx, pv->var.SafeStopCmd, Hmsg );
					}
					else if(OpKeyMake.down)
					{
						pv->var.SafeStopCmd = AAT_REVERSE;
						OPEREG_WRITE( pv->var.OpeSetIdx, pv->var.SafeStopCmd, Hmsg );
					}
					else
					{
					}
				}
				else
				{
					OPEREG_WRITE( pv->var.OpeSetIdx, pv->var.SafeStopCmd, Hmsg );
				}
			}
//			rc = AdatJratSet( Fmsg );
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	終了表示状態																		*/
		/*--------------------------------------------------------------------------------------*/
		case AAT_SEQ_ENDDISP:
			pv->var.State = AAT_SEQ_END;
			break;
		/*--------------------------------------------------------------------------------------*/
		/*	最終設定状態																		*/
		/*--------------------------------------------------------------------------------------*/
		case AAT_SEQ_END:
			rc = AdatFinishOpe( Fmsg, AdatV->ErrState );
			break;
		/*--------------------------------------------------------------------------------------*/
		/*	終了待ち状態																		*/
		/*--------------------------------------------------------------------------------------*/
		case AAT_SEQ_WAIT:
			break;
		/*--------------------------------------------------------------------------------------*/
		/*	後処理状態																			*/
		/*--------------------------------------------------------------------------------------*/
		case AAT_SEQ_RETURN:
			FunAdvAutoTuningEnd( Fmsg );
			rc = FUN_END;
			break;
		}
		AdatLcdDisplay( Fmsg );
	return (rc);
}

/*****************************************************************************
  Description:	Fn201 アドバンストオートチューニング初期化処理
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	NONE
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void	AdatTuningInitial( FUNMSG *Fmsg )
{
	FNAATV *pv = (FNAATV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;

	pv->var.ModeSetPtr = AAT_SCROLL_MIN;
	pv->var.JratSetPtr = JRAT_STS_DRVWAIT;
//	pv->var.ServonFlag = FALSE;

	pv->var.AdatMode = FALSE;	/* デジオペはデフォルトON(<=調整レス有効で一旦OFFにすると戻す手段がない) */
	OPEREG_WRITE(pv->var.AdatModeIdx, pv->var.AdatMode, Hmsg);
//	STSREG_READX(pv->var.AdatModeIdx, (ULONG*)&(pv->var.AdatMode), MCHK_REG_READ, Hmsg);
	STSREG_READX(pv->var.AdatLevelIdx, (ULONG*)&(pv->var.AdatLevel), MCHK_REG_READ, Hmsg);
	STSREG_READX(pv->var.AdatFilTypeIdx, &(pv->var.AdatFilType), MCHK_REG_READ, Hmsg);
	STSREG_READX(pv->var.AdatMovepIdx, (ULONG*)&(pv->var.AdatMovep), MCHK_REG_READ, Hmsg);
	pv->var.AdatMovep *= 1000;
	STSREG_READX( pv->var.TimeCntrIdex,&(pv->var.SafeTime), MCHK_REG_READ, Hmsg );			/* 安全停止時間Read */


	return;
}

/*****************************************************************************
  Description:	Fn201 Advanced Auto Tuning Mode Set
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	LONG			FUN_CONTINUE: Execution Continue
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static LONG	AdatTuningModeSet( FUNMSG *Fmsg )
{
	OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
	FNAATV *pv = (FNAATV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
	LONG	calc_tmp;
	LONG  i;
	LONG	data;
	ADATV *AdatV;
	BLINKFLG *BlinkFlag;
	UCHAR	UpdateFlag;

	calc_tmp = 0;
	i = 0;
	data = 0;
	AdatV = &(Fmsg->Super->pAxRsc->AdatHndl->AdatV);
	BlinkFlag = (BLINKFLG *)(Fmsg->pBlnkKick);
	UpdateFlag = 0U;

	if ( OpKeyMake.data == TRUE )
	{
		if ( MlibAbs32( AdatV->MoveDistance ) < AdatV->MinDistance )
		{
			BlinkFlag->error = TRUE;
		}
		else
		{
			/*TODO:アドバンストオートチューニング応答レベル設定*/

			OPEREG_WRITE( pv->var.OpeModeIdx, FNAAT_MODE, Hmsg );
			OPEREG_WRITE( pv->var.TimeCntrIdex, 65535, Hmsg );

			pv->var.State = AAT_SEQ_JRATSET;
		}
		pv->var.DPos = 0;
	}
	else if ( OpKeyMake.scroll == TRUE )
	{
		/* ｽｸﾛｰﾙ押下で、MODE → STROKE → LEVEL → TYPE	*/
		/*				 ↑＿＿＿＿＿＿＿＿＿＿＿＿↓	*/
		/* の順に設定事項を変更する						*/
		if ( pv->var.ModeSetPtr > AAT_SCROLL_MIN )
		{
			pv->var.ModeSetPtr --;
		}
		else
		{
			pv->var.ModeSetPtr = AAT_SCROLL_MAX;
		}
		pv->var.DPos = 0;
	}
	else
	{
		/* do nothing	*/
	}
	switch ( pv->var.ModeSetPtr )
	{
	/*----------------------------------------------------------------------------------*/
	/*	イナーシャ同定あり／なし														*/
	/*----------------------------------------------------------------------------------*/
		case AAT_MODE:
			if ( OpKeyMake.up == TRUE )
			{
				pv->var.AdatMode = TRUE;		/* イナーシャ同定なし		*/
				UpdateFlag = 1U;
			}
			else if ( OpKeyMake.down == TRUE )
			{
				pv->var.AdatMode = FALSE;		/* イナーシャ同定あり		*/
				UpdateFlag = 1U;
			}
			else
			{
				/*do nothing */
			}
		break;
	/*----------------------------------------------------------------------------------*/
	/*	調整レベル		1:標準 2:位置決め専用											*/
	/*----------------------------------------------------------------------------------*/
		case AAT_LVL:
			if ( OpKeyMake.up == TRUE)
			{
				if ( pv->var.AdatLevel < AAT_MODE_MAX )
				{
					pv->var.AdatLevel ++;
					UpdateFlag = ( 1U << 1 );
				}
			}
			else if ( OpKeyMake.down == TRUE )
			{
				if ( pv->var.AdatLevel > AAT_MODE_MIN)
				{
					pv->var.AdatLevel --;
					UpdateFlag = ( 1U << 1 );
				}
			}
			else
			{
				/* do nothing	*/
			}
		break;
	/*----------------------------------------------------------------------------------*/
	/*	フィルタタイプ	0～3															*/
	/*----------------------------------------------------------------------------------*/
		case AAT_TYPE:
			if ( OpKeyMake.up == TRUE )
			{
				if ( pv->var.AdatFilType < AAT_TYPE_MAX )
				{
					pv->var.AdatFilType ++;
					UpdateFlag = ( 1U << 2 );
				}
			}
			else if ( OpKeyMake.down == TRUE)
			{
				if (pv->var.AdatFilType > AAT_TYPE_MIN)
				{
					pv->var.AdatFilType --;
					UpdateFlag = ( 1U << 2 );
				}
			}
			else
			{
				/*do nothing 	*/
			}
		break;
	/*----------------------------------------------------------------------------------*/
	/*	移動距離																		*/
	/*----------------------------------------------------------------------------------*/
		case AAT_MOV:
			calc_tmp = 1000;
			for ( i = 0; i < pv->var.DPos; i++ )
			{
				calc_tmp = calc_tmp * 10;
			}
			if ( OpKeyMake.shup == TRUE )
			{
				if ( pv->var.DPos < (AAT_DTUN_MAX - 1) )
				{
					pv->var.DPos ++;		/* 桁移動	*/
				}
				else
				{
					pv->var.DPos = 0;
				}
			}
			else if ( OpKeyMake.shdown == TRUE )
			{
				if( (pv->var.DPos > 0) && (pv->var.DPos < AAT_DTUN_MAX) )
				{
					pv->var.DPos --;			/* 桁移動	*/
				}
				else
				{
					pv->var.DPos = AAT_DTUN_MAX - 1;
				}
			}
			else if ( OpKeyMake.up == TRUE )
			{
				data = ((pv->var.AdatMovep + calc_tmp) / 1000) * 1000;
				if (data > AAT_DIST_MAX)
				{
					pv->var.AdatMovep = AAT_DIST_MAX;
				}
				else
				{
					pv->var.AdatMovep = data;
				}
//				pv->var.AdatMovep /= 1000;
				UpdateFlag = ( 1U << 3 );
			}
			else if ( OpKeyMake.down == TRUE )
			{
				data = ((pv->var.AdatMovep - calc_tmp) / 1000) * 1000;
				if ( data < -AAT_DIST_MAX )
				{
					pv->var.AdatMovep = -AAT_DIST_MAX;
				}
				else
				{
					pv->var.AdatMovep = data;
				}
//				pv->var.AdatMovep /= 1000;
				UpdateFlag = ( 1U << 3 );
			}
			else
			{
				/* do nothing	*/
			}
			break;
		default:
		break;
	}

	switch( UpdateFlag )
	{
		case 1:
			/* Set Mode to Mode Register 		*/
			OPEREG_WRITE( pv->var.AdatModeIdx, pv->var.AdatMode, Hmsg );
			break;
		case 2:
			/* Set Level to  Level Register 		 */
			OPEREG_WRITE( pv->var.AdatLevelIdx, pv->var.AdatLevel, Hmsg );
			break;
		case 4:
			/* Set FilType to FilType Register 		*/
			OPEREG_WRITE( pv->var.AdatFilTypeIdx, pv->var.AdatFilType, Hmsg );
			break;
		case 8:
			/* Set Move to Move Register 		*/
			data = pv->var.AdatMovep / 1000;
			OPEREG_WRITE( pv->var.AdatMovepIdx, data, Hmsg );
			break;
		default:
		break;
	}

	return FUN_CONTINUE;
}

/*****************************************************************************
  Description:	Fn201 Advanced Auto Tuning Mode LCD Display
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	None
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void	AdatLcdDisplay( FUNMSG *Fmsg )
{
FNAATV *pv = (FNAATV *)(Fmsg->pVar);
UCHAR *ResBuf = Fmsg->pTxBuf;

//	RpxLcdClrTxBuf( ResBuf ); 					/* clear Tx buffer */
//		/* TITLE	*/
//	RpxLcdSetStr( 0U, 6U, (UCHAR *)"Advanced AT", ResBuf );
//
//	RpxLcdSetStr( 1U, 0U, (UCHAR *)"                 ", ResBuf );
//	RpxLcdSetStr( 2U, 0U, (UCHAR *)"                 ", ResBuf );
//	RpxLcdSetStr( 3U, 0U, (UCHAR *)"                 ", ResBuf );
//	RpxLcdSetStr( 4U, 0U, (UCHAR *)"                 ", ResBuf );
//	RpxLcdClrLineBlnk( 0U, ResBuf );
//	RpxLcdClrLineBlnk( 1U, ResBuf );
//	RpxLcdClrLineBlnk( 2U, ResBuf );
//	RpxLcdClrLineBlnk( 3U, ResBuf );
//	RpxLcdClrLineBlnk( 4U, ResBuf );

	switch ( pv->var.State )
	{
		case AAT_SEQ_INIT :
			AdatStartWaitingDisplay( Fmsg );
			break;

//		case AAT_SEQ_MODESET :
//			AdatModeSelectDisplay( Fmsg );
//			break;

		case AAT_SEQ_JRATSET :
			AdatJSetDisplay( Fmsg );
			break;

		case AAT_TUNNING_WAIT:
			RpxLcdSetBlnk( 0U, 0U, 5U, ResBuf );
			/*fall through*/
		case AAT_SEQ_ENDDISP:
			/*fall through*/
		case AAT_SEQ_END:
			/*fall through*/
		case AAT_SEQ_WAIT:
			/*fall through*/
		case AAT_SEQ_RETURN:
			AdatGainTuneDisplay( Fmsg );
			break;
		default:
			break;
	}
	return;
}
/*****************************************************************************
  Description:	Fn201 Advanced Auto Tuning Mode Waiting Display
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	None
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void	AdatStartWaitingDisplay( FUNMSG *Fmsg )
{
	UCHAR *ResBuf = Fmsg->pTxBuf;
	/* TITLE	*/
	RpxLcdSetStr( 2U, 0U, (UCHAR *)" Motor Stop Wait ", ResBuf );
	RpxLcdSetBlnk( 2U, 0U, 17U, ResBuf );
	return;
}

/*****************************************************************************
  Description:	Fn201 Advanced Auto Tuning Mode Select Display
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	None
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void	AdatModeSelectDisplay( FUNMSG *Fmsg )
{
SHORT	dpoint;
LONG	dist;
ULONG	rev;
FNAATV *pv = (FNAATV *)(Fmsg->pVar);
UCHAR *ResBuf = Fmsg->pTxBuf;
MSGIF *Hmsg = Fmsg->Super;
ADATP	*AdatP = &(Fmsg->Super->pAxRsc->AdatHndl->AdatP);

		/* TITLE	*/
	RpxLcdSetStr( 2U, 0U, (UCHAR *)" Mode=   Type=   ", ResBuf );
	/* Tuning Level	*/
	RpxLcdSetDecPnt( 2U, 6U, (ULONG) pv->var.AdatLevel, 1U, (USHORT)FALSE, 0, ResBuf );
	/* Tuning Type	*/
	RpxLcdSetDecPnt( 2U, 14U, (ULONG) pv->var.AdatFilType, 1U, (USHORT)FALSE, 0, ResBuf );

		/* TITLE	*/
	RpxLcdSetStr( 1U, 0U, (UCHAR *)" Jcalc=          ", ResBuf );
	RpxLcdSetStr( 3U, 0U, (UCHAR *)" Stroke=         ", ResBuf );

//	PRMREG_READX( pv->var.Syssw1Idex, (ULONG *)(&pv->var.AmpType), MCHK_REG_READ, Hmsg );	//<S197>
//	if( (pv->var.AmpType & 0x0002) == MOTTYPE_LINEAR )
	PRMREG_READX( pv->var.IntModeStateIdex, (ULONG *)(&pv->var.AmpType), MCHK_REG_READ, Hmsg );
	if( (pv->var.AmpType & 0x0400) == MOTTYPE_LINEAR )
	{
		RpxLcdSetStr( 4U, 0U, (UCHAR *)"      (      )mm", ResBuf );
	}
	else
	{
		RpxLcdSetStr( 4U, 0U, (UCHAR *)"      (      )rev", ResBuf );
	}
			/* Tuning Mode	*/
	if(pv->var.AdatMode == FALSE)												/* <V157> */
	{
		RpxLcdSetStr( 1U, 7U, (UCHAR *)"ON ", ResBuf );
	}
	else
	{
		RpxLcdSetStr( 1U, 7U, (UCHAR *)"OFF", ResBuf );
	}

	/* Tuning Stroke	*/
	dist = MlibAbs32( pv->var.AdatMovep );
	if ( pv->var.AdatMovep >= 0 )
	{
		RpxLcdSetStr( 3U, 8U, (UCHAR *)"+", ResBuf );
	}
	else
	{
		RpxLcdSetStr( 3U, 8U, (UCHAR *)"-", ResBuf );
	}
	RpxLcdSetDecPnt( 3U, 9U, (ULONG)dist, 8U, (USHORT)FALSE, 0, ResBuf );
	/* Tuning Revolution	*/
	if ( AdatP->MencV->LowSpdMotor == TRUE )
	{
		dpoint = 2;
	}
	else
	{
		dpoint = 1;
	}
	rev = (ULONG)AdatPcalRevFromDistance( dist, AdatP );
	if ( rev > 65535U )
	{
		RpxLcdSetStr( 4U, 7U, (UCHAR *)"******", ResBuf );
	}
	else
	{
		RpxLcdSetDecPnt( 4U, 7U, rev, 5U, (USHORT)FALSE, dpoint, ResBuf );
	}

		/* カーソル点滅設定	*/
	switch ( pv->var.ModeSetPtr )
	{
		case AAT_MODE:
			RpxLcdSetBlnk( 1U, 7U, 3U, ResBuf );								/* <V128> */
			break;

		case AAT_LVL:
			RpxLcdSetBlnk( 2U, 6U, 1U, ResBuf );								/* <V128> */
			break;

		case AAT_TYPE:
			RpxLcdSetBlnk( 2U, 14U, 1U, ResBuf );
			break;

		case AAT_MOV:
			RpxLcdSetBlnk( 3U, (USHORT)(13 - pv->var.DPos), 1U, ResBuf );
			break;

		default:
			break;
	}
	return;
}
/*****************************************************************************
  Description:	アドバンストオートチューニング移動距離単位変換処理
------------------------------------------------------------------------------
  Parameters:
	distance;			distance
	AdatP				アドバンスドオートチューニング データポインタ
  Return:
	LONG
------------------------------------------------------------------------------
  Note:
  	  概要:	移動距離単位変換を行う [指令単位] → [0.1rev]
  	  	  	  	  	  	  	  	  	  	  [0.1mm] for LINEAR
*****************************************************************************/
static LONG	AdatPcalRevFromDistance( LONG distance, ADATP *AdatP )
{

	LONG	sx;
	LONG	kx;
	LONG	lwork;
	POS_MNG_HNDL *PosManager = &(AdatP->BaseCtrlData->PosManager);

/*--------------------------------------------------------------------------------------*/
		lwork = (AdatP->MencV->LowSpdMotor)? 100 : 10;
/*--------------------------------------------------------------------------------------*/

		kx = MlibScalKxgain( lwork, 1, AdatP->UniPrms->Bprm->XfbPulse1, &sx, 0 );
		kx = MlibPcalKxgain( kx, PosManager->conf.Egear.b, PosManager->conf.Egear.a, &sx, 0 );
		kx = MlibPcalKxgain( kx, distance, 1, &sx, -30 );

		return ( kx );
}

/*****************************************************************************
  Description:	Fn201 Advanced Auto Tuning Mode JSet Display
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	None
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void	AdatJSetDisplay( FUNMSG *Fmsg )
{
FNAATV *pv = (FNAATV *)(Fmsg->pVar);
UCHAR *ResBuf = Fmsg->pTxBuf;
MSGIF *Hmsg = Fmsg->Super;
ULONG	Jrate;
ADATV *AdatV = &(Fmsg->Super->pAxRsc->AdatHndl->AdatV);

	/* TITLE	*/
	RpxLcdSetStr( 1U, 0U, (UCHAR *)"Pn103=", ResBuf );
	RpxLcdSetStr( 2U, 0U, (UCHAR *)"Pn100=", ResBuf );
	RpxLcdSetStr( 3U, 0U, (UCHAR *)"Pn101=", ResBuf );

	/* Gain	*/
	switch ( AdatV->JratErr )
	{
		case ADAT_JRAT_OK :
			Jrate = AdatV->Jrate;
			RpxLcdSetDecPnt( 1U, 6U, Jrate, 5U, FALSE, 0U, ResBuf );
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

	if ( ( pv->var.AdatLevel == AAT_LEVEL_POS )													/* <V128> */
			|| ( pv->var.AdatLevel == AAT_LEVEL_POS_ST ) )										/* <V213> */
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
//	STSREG_READX(pv->var.AdatStateIdx, (ULONG *)(&pv->var.AdatState), MCHK_REG_READ, Hmsg);
//	pv->var.AdatState = pv->var.AdatState & 0xFF;
//	if ( pv->var.AdatState != AAT_STS_JEND  )
	if ((AdatV->PrgJogStat != ADAT_PJOG_NOP) && (AdatV->PrgJogStat != ADAT_PJOG_FIN) )
	{
		RpxLcdSetBlnk( 0U, 0U, 5U, ResBuf );
		RpxLcdSetBlnk( 1U, 6U, 5U, ResBuf );
	}
	return;
}
/*****************************************************************************
  Description:	Fn201 Advanced Auto Tuning Mode GainTune Display
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	None
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void	AdatGainTuneDisplay( FUNMSG *Fmsg  )
{
UCHAR *ResBuf = Fmsg->pTxBuf;
FNAATV *pv = (FNAATV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
ULONG	Jrate;
ADATV *AdatV = &(Fmsg->Super->pAxRsc->AdatHndl->AdatV);

	/* TITLE	*/
	RpxLcdSetStr( 1U, 0U, (UCHAR *)"Pn103=", ResBuf );
	RpxLcdSetStr( 2U, 0U, (UCHAR *)"Pn100=", ResBuf );
	RpxLcdSetStr( 3U, 0U, (UCHAR *)"Pn101=", ResBuf );

	/* Gain	*/
	switch ( AdatV->JratErr )
	{
		case ADAT_JRAT_OK :
			Jrate = AdatV->Jrate;
			RpxLcdSetDecPnt( 1U, 6U, Jrate, 5U, FALSE, 0U, ResBuf );
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

	if ( ( pv->var.AdatLevel == AAT_LEVEL_POS )							
			|| ( pv->var.AdatLevel == AAT_LEVEL_POS_ST ) )				
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

	if ( (AdatV->CoinWaitStat == ADAT_STS_COINWAIT)
	  || (AdatV->CoinWaitStat == ADAT_STS_COINWARN) )
	{
		RpxLcdSetStr( 1U, 0U, (UCHAR *)"     WAITING     ", ResBuf );
		RpxLcdSetBlnk( 1U, 0U, 17U, ResBuf );
	}

	return;
}
/*****************************************************************************
  Description:	Fn201 Advanced Auto Tuning Mode Finish Operator
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
	err_status;			Error Status
  Return:
	LONG			FUN_CONTINUE: Execution Continue
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static LONG	AdatFinishOpe( FUNMSG *Fmsg, SHORT err_status )
{
FNAATV *pv = (FNAATV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
BLINKFLG *BlinkFlag = (BLINKFLG *)(Fmsg->pBlnkKick);

	pv->var.DrvErrDetect = FALSE;
	/*----------------------------------------------------------------------------------------------*/
	/*	エラー処理																					*/
	/*----------------------------------------------------------------------------------------------*/
	switch(err_status)
	{
		case ADAT_OK:											/* 正常終了							*/
	if ( OpKeyMake.data == TRUE )
	{
		OPEREG_WRITE( pv->var.OpeSetIdx, AAT_WRITEPRM , Hmsg );
				BlinkFlag->done = TRUE;
		pv->var.State = AAT_SEQ_WAIT;
	}
			break;

		case ADAT_JST_ERR:										/* イナーシャ同定開始エラー			*/
		case ADAT_NOTRDYERR:									/* 運転準備未完エラー				*/
			BlinkFlag->no_op = TRUE;
			pv->var.State = AAT_SEQ_WAIT;						/* 現在のシーケンスポインタを保存	*/
			break;

		case ADAT_COMERR:										/* 動作中の通信エラー				*/
			BlinkFlag->no_op = TRUE;
			pv->var.State = AAT_SEQ_WAIT;						/* 現在のシーケンスポインタを保存	*/
			break;

		default:												/* その他のエラーの場合				*/
			BlinkFlag->error = TRUE;
			pv->var.State = AAT_SEQ_WAIT;						/* 現在のシーケンスポインタを保存	*/
			break;
	}

	return FUN_CONTINUE;
}

/***************************************** end of file **********************************************/

