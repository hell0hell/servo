/*****************************************************************************
Description		: Magnetic Pole Adjust
-----------------------------------------------------------------------------
Author		: SHCORE
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

Functions	  :
	-- APIs --
	FunMagnePoleAdjustMain();						:Fn080 Magnetic Pole Adjust mode Display
	FunMagnePoleAdjustBegin();						:Fn080 Magnetic Pole Adjust mode Begin
	FunMagnePoleAdjustEnd();						:Fn080 Magnetic Pole Adjust mode End

	-- Locals --
	LpxLcdMagnePoleAdjustExe();						:LCD Operator Fn080 Execution
	LpxPnlMagnePoleAdjustExe();						:PANEL Operator Fn080 Execution
	FunMagneticPoleAdjustLcdGnLevelTuneDisplay();	:Gain Level Set LCD Display
	FunMagneticPoleAdjustLcdPdetStartDisplay();		:Start LCD Display
	FunMagneticPoleAdjustLcdPdetExeDisplay();		:Exe LCD Display
	FunMagneticPoleAdjustLcdPdetEndDisplay();		:End LCD Display
	FunMagneticPoleAdjustDisplay();					:LCD 表示／LED 表示処理メイン
	FunMagneticPoleAdjustLedGnLevelTuneDisplay();	:Gain Level Set LED Display
	FunMagneticPoleAdjustLedPdetExeDisplay;			:Exe LED Display

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
SL.Xue		2013.06.25		created
SL.Xue		2013.12.12		error、done点滅処理を修正した

*****************************************************************************/

#include	"FunPoleDetect.h"


/*--------------------------------------------------------------------------------------------------*/
/* Operation Register 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define OPEMODE_REG1			(0x0000)		/* 1st Operation Mode Register */
#define OPESET_REG1				(0x0001)		/* 1st Operation Mode Set Register */
#define OPEPDETSTATE_REG1		(0x032)			/* 0x032: 磁極検出完了 */

/*--------------------------------------------------------------------------------------------------*/
/* Operation Mode	 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define FNNORMAL_MODE			(0x0000)		/* Normal Mode	*/
#define FNPOLEDETECT_MODE		(0x1080) 		/* Magnetic Pole Adjust Mode */

/*--------------------------------------------------------------------------------------------------*/
/* Operation Command 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define MPDET_NORMAL			(0x00U)			/* 通常		*/
#define MPDET_EXE				(0x01U)			/* 実行中	*/
#define MPDET_COMP				(0x02U)			/* 完了		*/
#define MPDET_ERROR				(0x03U)			/* 異常終了	*/
#define POLEDETECT_STOP			(0x00)			/* 中止 */
#define POLEDETECT_START		(0x01)			/* 実行 */

/*--------------------------------------------------------------------------------------------------*/
/* Un Register 																						*/
/*--------------------------------------------------------------------------------------------------*/
#define UNALMCODE_REG1			(0x050A)		/* 0x50A: 現在のアラーム状態 */

/*--------------------------------------------------------------------------------------------------*/
/* Un Command 																						*/
/*--------------------------------------------------------------------------------------------------*/
#define MTURNCLRCSTS_SUCCESS	(0x0000U)						/* 正常＇アラームなし （クリア成功）*/
#define MTURNCLRCSTS_ALM_PHASEANGLE	(0x0C22U)					/* 正常＇アラームA.C22 				*//* <S215> */

/*--------------------------------------------------------------------------------------------------*/
/* Prm Register 																						*/
/*--------------------------------------------------------------------------------------------------*/
#define PRMPDETLOOPHZ_REG1		(0x481)			/* 0x481: 磁極検出速度ループゲイン */

/*--------------------------------------------------------------------------------------------------*/
/* 磁極検出速度ループゲイン変更有効桁数定義	pfexe->App.DPos 										*/
/*--------------------------------------------------------------------------------------------------*/
#define OPPADJ_KVTUN_MAX		(4U)			/* 磁極検出速度ループゲイン最大桁数 */

/*--------------------------------------------------------------------------------------------------*/
#define FUN_CONTINUE			(0L)			/*Continue	*/
#define FUN_END					(1L)			/*End		*/

/*--------------------------------------------------------------------------------------------------*/
#define OPEREG_WRITE( idx, data, hmsg )   (COPEREG->Write( COPEREG, (idx), (data), hmsg ))
#define PRMREG_WRITE( idx, data, hmsg )   (CPARAMX->Write( CPARAMX, (idx), (data), hmsg ))
#define STSREG_READX( idx, pResData, CheckType, hmsg )    (COPEREG->Read( COPEREG, (idx), (pResData), \
																	(CheckType), hmsg ))   /* <S073> */
#define PRMREG_READX( idx, pResData, CheckType, hmsg )    (CPARAMX->Read( (COBJECT *)(CPARAMX), (idx),\
															 	 	 (pResData), (CheckType), hmsg ))   /* <S073> */
#define UNREG_READX( idx, pResData, CheckType, hmsg )    (CUNREGX->Read( (COBJECT *)(CUNREGX), (idx),\
															 	 	 (pResData), (CheckType), hmsg ))	/*<S054>*/  /* <S073> */

/****************************************************************************************************/
/*																									*/
/*		Function List																				*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Public Function																				*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunMagnePoleAdjustBegin(FUNMSG *Fmsg); 		/*	 Fn080 Magnetic Pole Adjust Mode Begin		*/
LONG FunMagnePoleAdjustMain(FUNMSG *Fmsg);			/*	 Fn080 Magnetic Pole Adjust Mode Main		*/
void FunMagnePoleAdjustEnd(FUNMSG *Fmsg);			/*	 Fn080 Magnetic Pole Adjust Mode End		*/

/*--------------------------------------------------------------------------------------------------*/
/*		Private Function																			*/
/*--------------------------------------------------------------------------------------------------*/
static void FunMagneticPoleAdjustLcdGnLevelTuneDisplay( FUNMSG *Fmsg );/* Gain Level Set LCD Display*/
static void FunMagneticPoleAdjustLcdPdetStartDisplay(FUNMSG *Fmsg);		/* Start LCD Display 		*/
static void FunMagneticPoleAdjustLcdPdetExeDisplay(FUNMSG *Fmsg);		/* Exe LCD Display 			*/
static void FunMagneticPoleAdjustLcdPdetEndDisplay(FUNMSG *Fmsg);		/* End LCD Display			*/
static void FunMagneticPoleAdjustDisplay(FUNMSG *Fmsg);		/* LCD 表示／LED 表示処理メイン			*/
static LONG LpxLcdMagnePoleAdjustExe(FUNMSG *Fmsg);			/* LCD Operator Fn080 Execution 		*/
#if  SVFSEL_PANELOP_USE == 0
#define LpxPnlMagnePoleAdjustExe( x ) (1)
#else
static LONG LpxPnlMagnePoleAdjustExe(FUNMSG *hMsg);			/* Panel Operator Fn080 Execution 		*/
static void FunMagneticPoleAdjustLedGnLevelTuneDisplay(FUNMSG *Fmsg);/* Gain Level Set LED Display	*/
static void FunMagneticPoleAdjustLedPdetExeDisplay(FUNMSG *Fmsg);		/* Exe LED Display			*/
#endif

/*****************************************************************************
  Description: Fn080 Magnetic Pole Adjust Mode Display
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		0: continue, 1: end 
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
LONG FunMagnePoleAdjustMain(FUNMSG *Fmsg)
{
	MSGIF *hMsg = Fmsg->Super;
	LONG rc;
	rc = FUN_CONTINUE;
/*--------------------------------------------------------------------------*/
	if( KPI_CHK_LopHmsg( hMsg ) == TRUE )					/* LCD Operator */
	{
		rc = LpxLcdMagnePoleAdjustExe( Fmsg );
	} 
/*--------------------------------------------------------------------------*/
	else if ( KPI_CHK_PopHmsg( hMsg ) == TRUE )				/*Panel Operator*/
	{
		rc = LpxPnlMagnePoleAdjustExe( Fmsg );
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
  Description: Fn080 Magnetic Pole Adjust mode Begin
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		OK: Success, Other: Failure 
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
LONG FunMagnePoleAdjustBegin(FUNMSG *Fmsg)
{
LONG	rc = OK;
BOOL ResFlag = TRUE;
FUNPOLEDETECTV  *pv = (FUNPOLEDETECTV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	pv->var.State = POLEADJ_INIT;
/*--------------------------------------------------------------------------------------------------*/
	LpxPrmChangeOpeDspMode();								/* すべてのユーザー定数を表示に強制変更	*//* <S20C> */
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
		rc = CPARAMX->Search( (COBJECT *)CPARAMX,			/* Search Register (Parameter 0x481)	*/
					PRMPDETLOOPHZ_REG1,
					0,
					&(pv->var.PrmPdetloophzIdx),
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
		rc = COPEREG->Search( COPEREG,						/* Search Register (Operation 0x032)	*/
					OPEPDETSTATE_REG1,
					0,
					&(pv->var.OpePdetStateIdx),
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
		rc = CUNREGX->Search( (COBJECT *)CUNREGX,				/* Search Register (Un Read 0x50A)	*/
					UNALMCODE_REG1,
					0,
					&(pv->var.UnReadIdx_AlmCode),
					MCHK_REG_READ,
					Hmsg);
		if( rc != OK )
		{
			ResFlag = FALSE;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/* Set PoleDetect Mode to Mode Register 															*/
/*--------------------------------------------------------------------------------------------------*/
	if ( ResFlag == TRUE )
	{
		rc = OPEREG_WRITE( pv->var.OpeModeIdx, FNPOLEDETECT_MODE, Hmsg );
	}
/*--------------------------------------------------------------------------------------------------*/
	KpiRstLongTimer( &( pv->var.DispTime ) );						/* Panel Operator				*/
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}


/*****************************************************************************
  Description: Fn080 Magnetic Pole Adjust Mode End
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	none
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
void FunMagnePoleAdjustEnd(FUNMSG *Fmsg)
{
FUNPOLEDETECTV  *pv = (FUNPOLEDETECTV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------*/
	LpxPrmReChangeOpeDspMode();				/* デジオペ表示設定を元に戻す	*//* <S20C> */
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
	OPEREG_WRITE( pv->var.OpeSetIdx, POLEDETECT_STOP, Hmsg );
	OPEREG_WRITE( pv->var.OpeModeIdx, FNNORMAL_MODE, Hmsg );
}


/*****************************************************************************
  Description: Fn080 Magnetic Pole Adjust Gain Level Set LCD Display Data
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void FunMagneticPoleAdjustLcdGnLevelTuneDisplay( FUNMSG *Fmsg )
{
	FUNPOLEDETECTV *pv = (FUNPOLEDETECTV *)(Fmsg->pVar);
	UCHAR *ResBuf = Fmsg->pTxBuf;
	/* 磁極検出速度ループゲインレベルＬＣＤ表示 */
	RpxLcdSetStr( 1U, 0U, (UCHAR *)" Magnetic Pole   ", ResBuf );
	RpxLcdSetStr( 2U, 0U, (UCHAR *)"     Detect      ", ResBuf );
	RpxLcdSetStr( 3U, 0U, (UCHAR *)" Level =         ", ResBuf );
	RpxLcdSetDecPnt( 3U, 9U, ( pv->var.PdetKvVal / 10U ), 4U, (USHORT)FALSE, 0, ResBuf );
	RpxLcdSetBlnk( 3U, ( 12U - pv->var.DPos ), 1U, ResBuf );
	return;
}


/*****************************************************************************
  Description: Fn080 Magnetic Pole Adjust Start LCD Display Data
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void FunMagneticPoleAdjustLcdPdetStartDisplay(FUNMSG *Fmsg)
{
	UCHAR *ResBuf = Fmsg->pTxBuf;
	/* 磁極検出開始前通電待ちＬＣＤ表示 */
	RpxLcdSetStr( 1U, 0U, (UCHAR *)" Magnetic Pole   ", ResBuf );
	RpxLcdSetStr( 2U, 0U, (UCHAR *)"     Detect      ", ResBuf );
	RpxLcdSetStr( 3U, 1U, (UCHAR *)"Start :[JOGSVON]", ResBuf );
	RpxLcdSetStr( 4U, 1U, (UCHAR *)"Return:[SET]", ResBuf );
	return;
}


/*****************************************************************************
  Description: Fn080 Magnetic Pole Adjust Exe LCD Display Data
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void FunMagneticPoleAdjustLcdPdetExeDisplay(FUNMSG *Fmsg)
{
	UCHAR *ResBuf = Fmsg->pTxBuf;
	/* 磁極検出実行中ＬＣＤ表示 */
	RpxLcdSetBlnk( 1U, 0U, 17U, ResBuf );
	RpxLcdSetBlnk( 2U, 0U, 17U, ResBuf );
	RpxLcdSetStr( 1U, 0U, (UCHAR *)" Magnetic Pole   ", ResBuf );
	RpxLcdSetStr( 2U, 0U, (UCHAR *)"  Adjustment     ", ResBuf );
	RpxLcdSetStr( 3U, 0U, (UCHAR *)"                 ", ResBuf );
	RpxLcdSetStr( 4U, 1U, (UCHAR *)"Return:[SET]", ResBuf );
	return;
}


/*****************************************************************************
  Description: Fn080 Magnetic Pole Adjust End LCD Display Data
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void FunMagneticPoleAdjustLcdPdetEndDisplay(FUNMSG *Fmsg)
{
	UCHAR *ResBuf = Fmsg->pTxBuf;
	/* 磁極検出完了ＬＣＤ表示（正常／異常関係なし）*/
	RpxLcdSetStr( 1U, 0U, (UCHAR *)" Magnetic Pole   ", ResBuf );
	RpxLcdSetStr( 2U, 0U, (UCHAR *)"     Detect      ", ResBuf );
	RpxLcdSetStr( 3U, 0U, (UCHAR *)"                 ", ResBuf );
	RpxLcdSetStr( 4U, 1U, (UCHAR *)"Return:[SET]", ResBuf );
	return;
}


/*****************************************************************************
  Description: LCD 表示／LED 表示処理メイン
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void FunMagneticPoleAdjustDisplay(FUNMSG *Fmsg)
{
	FUNPOLEDETECTV *pv = (FUNPOLEDETECTV *)(Fmsg->pVar);
	UCHAR *ResBuf = Fmsg->pTxBuf;

	/* TITLE	*/
//	RpxLcdSetStr( 0U, 6U, (UCHAR *)"Pole Detect", ResBuf );/* <S216> */
	/* ALL Clear	*/
	RpxLcdClrLineBlnk( 1U, ResBuf );
	RpxLcdClrLineBlnk( 2U, ResBuf );
	RpxLcdClrLineBlnk( 3U, ResBuf );
	RpxLcdClrLineBlnk( 4U, ResBuf );
	switch( pv->var.State )
	{
	case POLEADJ_INIT:
	case POLEADJ_GAINCHANGE:
		FunMagneticPoleAdjustLcdGnLevelTuneDisplay( Fmsg );
		break;
	case POLEADJ_EXE1:
	case POLEADJ_EXE2:
		FunMagneticPoleAdjustLcdPdetStartDisplay( Fmsg );
		break;
	case POLEADJ_EXEAFTER:
		FunMagneticPoleAdjustLcdPdetExeDisplay( Fmsg );
		break;
	case POLEADJ_FNERR:
	case POLEADJ_END:
		FunMagneticPoleAdjustLcdPdetEndDisplay( Fmsg );
		break;
	default:
		break;
	}
}


/*****************************************************************************
  Description: Fn080 Magnetic Pole Adjust mode Execution
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		FUN_CONTINUE: Execution Continue, FUN_END: Execution End
------------------------------------------------------------------------------
  Note:
	LCD Operator Fn080 Execution
*****************************************************************************/
static LONG LpxLcdMagnePoleAdjustExe(FUNMSG *Fmsg)
{
FUNPOLEDETECTV *pv = (FUNPOLEDETECTV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
UCHAR *ResBuf = Fmsg->pTxBuf;
OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
BLINKFLG *pBlinkFlg = (BLINKFLG *)(Fmsg->pBlnkKick);
AXIS_HANDLE *pAxRsc = Hmsg->pAxRsc;											/* TODO: 後で消す		*/
LONG tmp;
LONG i;
ULONG ulwork;
LONG rc = FUN_CONTINUE;														 /* 0: continue, 1: end */
/*--------------------------------------------------------------------------------------------------*/
		/* clear Tx buffer */
		RpxLcdClrTxBuf( ResBuf );
/*--------------------------------------------------------------------------------------------------*/
/*		POLEDETECT (Fn080) Sequence																	*/
/*--------------------------------------------------------------------------------------------------*/
		switch( pv->var.State )
		{
/*--------------------------------------------------------------------------------------------------*/
		case POLEADJ_INIT:													/* Initial State 		*/
/*--------------------------------------------------------------------------------------------------*/
			PRMREG_READX( pv->var.PrmPdetloophzIdx, &(pv->var.ResPdetloophzData), MCHK_REG_READ, Hmsg );
			pv->var.PdetKvVal = pv->var.ResPdetloophzData;
			pv->var.State = POLEADJ_GAINCHANGE;
			break;
/*--------------------------------------------------------------------------------------------------*/
		case POLEADJ_GAINCHANGE:											/* Gain Change state	*/
/*--------------------------------------------------------------------------------------------------*/
			tmp = 10;
			for(i = 0; (ULONG)i < pv->var.DPos; i++)
			{
				tmp = tmp * 10;
			}
			if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = POLEADJ_END;
			}
			else if( OpKeyMake.up == TRUE )
			{
				ulwork = pv->var.PdetKvVal + (ULONG)tmp;
				pv->var.PdetKvVal = TunePrmLimitUnsigned( ulwork, &pndef_pdetloophz );
			}
			else if( OpKeyMake.down == TRUE )
			{
				ulwork = pv->var.PdetKvVal - (ULONG)tmp;
				pv->var.PdetKvVal = TunePrmLimitUnsigned( ulwork, &pndef_pdetloophz );
			}
			else if( OpKeyMake.shup == TRUE )		/* 磁極検出速度ループゲイン変更桁移動：左移動 */
			{
				if( pv->var.DPos < ( OPPADJ_KVTUN_MAX - 1U ) )
				{
					pv->var.DPos++;
				}
				else
				{
					pv->var.DPos = 0U;
				}
			}
			else if( OpKeyMake.shdown == TRUE )		/* 磁極検出速度ループゲイン変更桁移動：右移動 */
			{
				if( ( pv->var.DPos > 0U ) && ( pv->var.DPos <= ( OPPADJ_KVTUN_MAX - 1U ) ) )
				{
					pv->var.DPos--;
				}
				else
				{
					pv->var.DPos = OPPADJ_KVTUN_MAX - 1U;
				}
			}
			else if( OpKeyMake.data == TRUE )
			{/* TODO: error表示操作処理 　*/
				pv->var.DPos = 0U;
				pv->var.State = POLEADJ_EXE1;
			}
			else
			{
				/* do nothing */
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case POLEADJ_EXE1:														/* start state		*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = POLEADJ_END;
			}
			else if( OpKeyMake.svon == TRUE )
			{
				if( Fmsg->Super->pAxRsc->BeSequence->SvReadyPhaseIgnore == TRUE )
				{
					/* TODO 電流零位相位置保存実行フラグクリア（PnE0D.5=0）	 */
					if( 0 )
					{
						pv->var.PdetReq = FALSE;
						pBlinkFlg->error = TRUE;
						pv->var.State = POLEADJ_FNERR;
					}
					else
					{
						pv->var.PdetReq = TRUE;
						pv->var.State = POLEADJ_EXE2;
					}
				}
				else
				{
					pv->var.PdetReq = FALSE;
					if( ( CHK_HBB() ) || ( !CHK_ALARM() ) )
					{
						pBlinkFlg->error = TRUE;
						pv->var.State = POLEADJ_FNERR;
					}
					else
					{
						pBlinkFlg->no_op = TRUE;
					}
				}
				OPEREG_WRITE( pv->var.OpeSetIdx, POLEDETECT_START, Hmsg );
			}
			else
			{
				/* do nothing */
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case POLEADJ_EXE2:														/* exe state		*/
/*--------------------------------------------------------------------------------------------------*/
			STSREG_READX( pv->var.OpePdetStateIdx, &(pv->var.ResPdetStateData), MCHK_REG_READ, Hmsg );
			if( pv->var.ResPdetStateData == MPDET_COMP )
			{/* 完了	*/
				pv->var.PdetReq = FALSE;
				pv->var.State = POLEADJ_EXEAFTER;
			}
			else if( pv->var.ResPdetStateData == MPDET_EXE)/* <S215> */
			{/* 実行中	*/
				if( OpKeyMake.mode == TRUE )
				{
					pv->var.State = POLEADJ_END;
				}
				else
				{
					/* do nothing */
				}
			}
			else
			{
				if( OpKeyMake.svon == TRUE )
				{
					if( Fmsg->Super->pAxRsc->BeSequence->SvReadyPhaseIgnore == TRUE )
					{
						pv->var.PdetReq = !pv->var.PdetReq;			/* キー交互操作で通電ＯＮ⇔ＯＦＦ */
					}
					else
					{
						pv->var.PdetReq = FALSE;
						if( ( CHK_HBB() ) || ( !CHK_ALARM() ) )
						{
							pBlinkFlg->error = TRUE;
							pv->var.State = POLEADJ_FNERR;
						}
						else
						{
							pBlinkFlg->no_op = TRUE;
						}
					}
				}
				else if( OpKeyMake.mode == TRUE )
				{
					pv->var.State = POLEADJ_END;
				}
				else
				{
					/* do nothing */
				}
			}

			if(( Fmsg->Super->pAxRsc->BeSequence->SvReadyPhaseIgnore == FALSE )&&( pv->var.ResPdetStateData != MPDET_EXE) )/* <S215> */
			{
				if( pv->var.PdetReq == TRUE )
				{
					pv->var.PdetReq = FALSE;
					pBlinkFlg->error = TRUE;
				}
				if( ( CHK_HBB() ) || ( !CHK_ALARM() ) )
				{
					pv->var.State = POLEADJ_FNERR;
				}
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case POLEADJ_EXEAFTER:													/* exeafter state	*/
/*--------------------------------------------------------------------------------------------------*/
			UNREG_READX( pv->var.UnReadIdx_AlmCode, (ULONG *)( &(pv->var.ResData_AlmCode) ), MCHK_REG_READ, Hmsg );
			if(( pv->var.ResData_AlmCode == MTURNCLRCSTS_SUCCESS )||(pv->var.ResData_AlmCode == MTURNCLRCSTS_ALM_PHASEANGLE))/* <S215> */
			{
				pv->var.State = POLEADJ_END;
			}
			else
			{
				pBlinkFlg->error = TRUE;
				pv->var.State = POLEADJ_FNERR;
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case POLEADJ_FNERR:														/* fnerr state		*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = POLEADJ_END;
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case POLEADJ_END:														/* End state		*/
/*--------------------------------------------------------------------------------------------------*/
			FunMagnePoleAdjustEnd( Fmsg );
			rc = FUN_END;
			break;
		default:
			break;
		}
		FunMagneticPoleAdjustDisplay( Fmsg );
	return (rc);
}


/*****************************************************************************
  Description: Fn080 Magnetic Pole Adjust Gain Level Set LED Display Data
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
#if  SVFSEL_PANELOP_USE == 1
static void FunMagneticPoleAdjustLedGnLevelTuneDisplay(FUNMSG *Fmsg)
{
	FUNPOLEDETECTV *pv = (FUNPOLEDETECTV *)(Fmsg->pVar);
	LEDDSP *DspData = (LEDDSP *)&(Fmsg->pTxBuf[PNLRSP_LEDCODE_IDX]);		/* set display data buffer */

	/* 磁極検出速度ループゲインレベルＬＥＤ表示	*/
	RpxLedSetStr( (UCHAR *)"L", DspData, 4U, 1U );
	RpxLedSetValDec( (USHORT)( pv->var.PdetKvVal / 10U ), DspData, 3U, 4U, (USHORT)FALSE, 0U );
	RpxLedSetBlnk( DspData, (UCHAR)( 1 << pv->var.DPos ) );
	return;
}

/*****************************************************************************
  Description: Fn080 Magnetic Pole Adjust Exe LED Display Data
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void FunMagneticPoleAdjustLedPdetExeDisplay(FUNMSG *Fmsg)
{
	MSGIF *Hmsg = Fmsg->Super;
	AXIS_HANDLE *pAxRsc = Hmsg->pAxRsc;										/* TODO: 後で消す		*/
	FUNPOLEDETECTV *pv = (FUNPOLEDETECTV *)(Fmsg->pVar);
	LEDDSP *DspData = (LEDDSP *)&(Fmsg->pTxBuf[PNLRSP_LEDCODE_IDX]);		/* set display data buffer */

	/* 磁極検出開始前通電待ち、磁極検出実行中、磁極検出完了ＬＥＤ表示	*/
	RpxLedSetBlnk( DspData, 0 );
	RpxLedSetStr( (UCHAR *)".Pdet", DspData, 4U, 5U );
	if( CHK_HBB() )
	{
		if( !( ( KpiGetLongTimer( &( pv->var.DispTime ) ) / 100U ) & 0x04U ) )
		{
			RpxLedSetStr( (UCHAR *)". Hbb", DspData, 4U, 5U );
		}
	}
	RpxLedSetStatusBits( DspData, Hmsg );
	return;
}


/*****************************************************************************
  Description: Fn080 Magnetic Pole Adjust mode Execution
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		FUN_CONTINUE: Execution Continue, FUN_END: Execution End
------------------------------------------------------------------------------
  Note:
	PANEL Operator Fn080 Execution
*****************************************************************************/
static LONG LpxPnlMagnePoleAdjustExe(FUNMSG *Fmsg)
{
FUNPOLEDETECTV *pv = (FUNPOLEDETECTV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
LEDDSP *DspData = (LEDDSP *)&(Fmsg->pTxBuf[PNLRSP_LEDCODE_IDX]);		/* set display data buffer	*/
BLINKFLG *pBlinkFlg = (BLINKFLG *)(Fmsg->pBlnkKick);
AXIS_HANDLE *pAxRsc = Hmsg->pAxRsc;											/* TODO: 後で消す		*/
LONG tmp;
LONG i;
ULONG ulwork;
LONG rc = FUN_CONTINUE;														 /* 0: continue, 1: end */
/*--------------------------------------------------------------------------------------------------*/
/*		POLEDETECT (Fn080) Sequence																	*/
/*--------------------------------------------------------------------------------------------------*/
		switch( pv->var.State )
		{
/*--------------------------------------------------------------------------------------------------*/
		case POLEADJ_INIT:													/* Initial State 		*/
/*--------------------------------------------------------------------------------------------------*/
			PRMREG_READX( pv->var.PrmPdetloophzIdx, &(pv->var.ResPdetloophzData), MCHK_REG_READ, Hmsg );
			pv->var.PdetKvVal = pv->var.ResPdetloophzData;
			pv->var.State = POLEADJ_GAINCHANGE;
			break;
/*--------------------------------------------------------------------------------------------------*/
		case POLEADJ_GAINCHANGE:											/* Gain Change state	*/
/*--------------------------------------------------------------------------------------------------*/
			tmp = 10;
			for(i = 0; (ULONG)i < pv->var.DPos; i++)
			{
				tmp = tmp * 10;
			}
			if( OpKeyMake.data == TRUE )
			{
				pv->var.State = POLEADJ_END;
			}
			else if( OpKeyMake.up == TRUE )
			{
				ulwork = pv->var.PdetKvVal + (ULONG)tmp;
				pv->var.PdetKvVal = TunePrmLimitUnsigned( ulwork, &pndef_pdetloophz );
			}
			else if( OpKeyMake.down == TRUE )
			{
				ulwork = pv->var.PdetKvVal - (ULONG)tmp;
				pv->var.PdetKvVal = TunePrmLimitUnsigned( ulwork, &pndef_pdetloophz );
			}
			else if( OpKeyMake.shup == TRUE )		/* 磁極検出速度ループゲイン変更桁移動：左移動 */
			{
				if( pv->var.DPos < ( OPPADJ_KVTUN_MAX - 1U ) )
				{
					pv->var.DPos++;
				}
				else
				{
					pv->var.DPos = 0U;
				}
			}
			else if( OpKeyMake.mode == TRUE )
			{	/* TODO: error表示操作処理 　*/
				pv->var.DPos = 0U;
				pv->var.State = POLEADJ_EXE1;
			}
			else
			{
				/* do nothing */
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case POLEADJ_EXE1:														/* start state		*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.data == TRUE )
			{
				pv->var.State = POLEADJ_END;
			}
			else if( OpKeyMake.mode == TRUE )
			{
				if( Fmsg->Super->pAxRsc->BeSequence->SvReadyPhaseIgnore == TRUE )
				{
					/* TODO 電流零位相位置保存実行フラグクリア（PnE0D.5=0）	 */
					if( 0 )
					{
						pv->var.PdetReq = FALSE;
						pBlinkFlg->error = TRUE;
						pv->var.State = POLEADJ_FNERR;
					}
					else
					{
						pv->var.PdetReq = TRUE;
				pv->var.State = POLEADJ_EXE2;
			}
				}
				else
				{
					pv->var.PdetReq = FALSE;
					if( ( CHK_HBB() ) || ( !CHK_ALARM() ) )
					{
						pBlinkFlg->error = TRUE;
						pv->var.State = POLEADJ_FNERR;
					}
					else
					{
						pBlinkFlg->no_op = TRUE;
					}
				}
				OPEREG_WRITE( pv->var.OpeSetIdx, POLEDETECT_START, Hmsg );
			}
			else
			{
				/* do nothing */
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case POLEADJ_EXE2:														/* exe state		*/
/*--------------------------------------------------------------------------------------------------*/
			STSREG_READX( pv->var.OpePdetStateIdx, &(pv->var.ResPdetStateData), MCHK_REG_READ, Hmsg );
			if( pv->var.ResPdetStateData == MPDET_COMP )
			{
				pv->var.PdetReq = FALSE;
				pv->var.State = POLEADJ_EXEAFTER;
			}
			else
			{
				if( OpKeyMake.mode == TRUE )
				{
					if( Fmsg->Super->pAxRsc->BeSequence->SvReadyPhaseIgnore == TRUE )
					{
						pv->var.PdetReq = !pv->var.PdetReq;			/* キー交互操作で通電ＯＮ⇔ＯＦＦ */
					}
					else
					{
						pv->var.PdetReq = FALSE;
						if( ( CHK_HBB() ) || ( !CHK_ALARM() ) )
						{
							pBlinkFlg->error = TRUE;
				pv->var.State = POLEADJ_FNERR;
			}
						else
						{
							pBlinkFlg->no_op = TRUE;
						}
					}
				}
				else if( OpKeyMake.data == TRUE )
			{
				pv->var.State = POLEADJ_END;
			}
			else
			{
					/* do nothing */
				}
			}

			if( Fmsg->Super->pAxRsc->BeSequence->SvReadyPhaseIgnore == FALSE )
			{
				if( pv->var.PdetReq == TRUE )
				{
					pv->var.PdetReq = FALSE;
					pBlinkFlg->error = TRUE;
				}
				if( ( CHK_HBB() ) || ( !CHK_ALARM() ) )
				{
					pv->var.State = POLEADJ_FNERR;
				}
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case POLEADJ_EXEAFTER:													/* exeafter state	*/
/*--------------------------------------------------------------------------------------------------*/
			UNREG_READX( pv->var.UnReadIdx_AlmCode, (ULONG *)( &(pv->var.ResData_AlmCode) ), MCHK_REG_READ, Hmsg );
			if( pv->var.ResData_AlmCode == MTURNCLRCSTS_SUCCESS )
			{
			pv->var.State = POLEADJ_END;
			}
			else
			{
				pBlinkFlg->error = TRUE;
				pv->var.State = POLEADJ_FNERR;
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case POLEADJ_FNERR:														/* fnerr state		*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.data == TRUE )
			{
				pv->var.State = POLEADJ_END;
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case POLEADJ_END:														/* End state		*/
/*--------------------------------------------------------------------------------------------------*/
			FunMagnePoleAdjustEnd( Fmsg );
			rc = FUN_END;
			break;
		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Make Display Data																			*/
/*--------------------------------------------------------------------------------------------------*/
		/* clear Tx buffer */
		MlibResetByteMemory( DspData, sizeof(LEDDSP) );
		/* ＬＥＤパネオペ画面表示 */
		switch( pv->var.State )
		{
		case POLEADJ_INIT:
		case POLEADJ_GAINCHANGE:
			FunMagneticPoleAdjustLedGnLevelTuneDisplay( Fmsg );
			break;
		case POLEADJ_EXE1:
		case POLEADJ_EXE2:
		case POLEADJ_EXEAFTER:
		case POLEADJ_FNERR:
		case POLEADJ_END:
			FunMagneticPoleAdjustLedPdetExeDisplay( Fmsg );
			break;
		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}
#endif


/***************************************** end of file **********************************************/
