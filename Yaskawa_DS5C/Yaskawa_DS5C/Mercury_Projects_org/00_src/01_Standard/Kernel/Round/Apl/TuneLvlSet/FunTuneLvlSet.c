/*****************************************************************************
Description		: Fn200 Dynamic Auto Tuning
-----------------------------------------------------------------------------
Author		: SHCORE
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

Functions	  :
	-- APIs --
	FunDynamicAutoTuningMain();			:Fn200 Dynamic Auto Tuning Mode Display
	FunDynamicAutoTuningBegin();		:Fn200 Dynamic Auto Tuning Mode Begin
	FunDynamicAutoTuningEnd();			:Fn200 Dynamic Auto Tuning Mode End

	-- Locals --
	LpxLcdDynamicAutoTuningExe();		:LCD Operator Fn200 Execution
	FunDynamicAutoTuningDisplay();		:Fn200 Display
	FunDynamicAutoTuningLcdJLevelTuneDisplay();		:J Level Display
	FunDynamicAutoTuningLcdGnLevelTuneDisplay();	:Gain Level Display

	LpxPnlDynamicAutoTuningExe();		:Panel Operator Fn200 Execution
	FunPnlDynamicAutoTuningDisplay();	:Fn200 Display ( Panel Operator Display )

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
K.Liu		2013.06.20		created
K.Liu		2013.08.29		Add Panel Operator Function
K.Liu		2013.12.09		Modify
*****************************************************************************/
#include "FunTuneLvlSet.h"
#include "Parameter.h"

/*--------------------------------------------------------------------------------------------------*/
/* Operation Register 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define	OPEMODE_REG1			(0x0000)			/* 1st Operation Mode Register 					*/
#define	OPESET_REG1				(0x0001)			/* 1st Operation Mode Set Register				*/
#define	OPEDATGNLVL_REG			(0x00E0)			/* Dat Gain Level Register 						*/
#define	OPEDATJRATLVL_REG		(0x00E1)			/* Dat Jrat Level Register 						*/
#define	OPEANOTCHST_REG			(0x0054)			/* 自動ノッチ設定結果状況 Register 				*/
/*--------------------------------------------------------------------------------------------------*/
#define	PRMTFUNCSW_REG			(0x0408)			/* orque トルク関係機能スイッチ Register 		*/
/*--------------------------------------------------------------------------------------------------*/
/* Operation Mode	 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define	FNTUNELVLSET_MODE		(0x1200U) 			/* Fn200 Dynamic Auto Tuning Mode 	*/
#define	FNNORMAL_MODE			(0x0000U)			/* Normal Mode						*/
/*--------------------------------------------------------------------------------------------------*/
/* Operation Command 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define	TUNELVLSETCMD_STOP	 	(0x0000)
#define	TUNELVLSETCMD_EXE	 	(0x0001)
#define	TUNELVLSETCMD_UP	 	(0x0002)
#define	TUNELVLSETCMD_DOWN	 	(0x0003)
#define	TUNELVLSETCMD_PRMUPD 	(0x0007)			/* Parameter Update 	*/
#define	TUNELVLSETCMD_EXE2	 	(0x0008)			/* Auxiliary Execution	*/
/*--------------------------------------------------------------------------------------------------*/
#define	FUN_CONTINUE			0					/* Continue 			*/
#define	FUN_END					1					/* End 					*/

/****************************************************************************************************/
/*		Constant Definition																			*/
/****************************************************************************************************/
//#define	DAT_GAINLEVEL_MAX	4				/* 調整レスゲインレベル最大値						*//* <S1C7> */
#define		DAT_GAINLEVEL_MAX	7				/* 調整レスゲインレベル最大値						*//* <S1C7> */
#define	DAT_GAINLEVEL_MIN		0U					/* 調整レスゲインレベル最小値					*/
#define	DAT_JRATLEVEL_MAX		2U					/* 調整レスイナーシャレベル最大値				*/
#define	DAT_JRATLEVEL_MIN		0U					/* 調整レスイナーシャレベル最小値				*/
/*--------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------*/
/* Fn200 Dynamic Auto Tuning State																	*/
/*--------------------------------------------------------------------------------------------------*/

/****************************************************************************************************/
/*																									*/
/*		Function List																				*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Public Function																				*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunDynamicAutoTuningBegin(FUNMSG *Fmsg); 		/* Fn200 Dynamic Auto Tuning Mode Begin		*/
LONG FunDynamicAutoTuningMain(FUNMSG *Fmsg);		/* Fn200 Dynamic Auto Tuning Mode Display	*/
void FunDynamicAutoTuningEnd(FUNMSG *Fmsg);			/* Fn200 Dynamic Auto Tuning Mode End		*/

/*--------------------------------------------------------------------------------------------------*/
/*		Private Function																			*/
/*--------------------------------------------------------------------------------------------------*/
static LONG LpxLcdDynamicAutoTuningExe(FUNMSG *Fmsg);			/* LCD Operator Fn200 Execution 	*/
static void FunDynamicAutoTuningDisplay( FUNMSG *Fmsg );		/* Fn200 Display 					*/
static void FunDynamicAutoTuningLcdJLevelTuneDisplay( UCHAR *ResBuf, USHORT JartLevel );	/* J Level Display */
static void FunDynamicAutoTuningLcdGnLevelTuneDisplay( FUNMSG *Fmsg );	/* Gain Level Display */

#if  SVFSEL_PANELOP_USE != 1
#define LpxPnlDynamicAutoTuningExe( x ) (1)
#define FunPnlDynamicAutoTuningDisplay( x )	()
#else
LONG LpxPnlDynamicAutoTuningExe(FUNMSG *hMsg);					/* Panel Operator Fn200 Execution 	*/
static void FunPnlDynamicAutoTuningDisplay( FUNMSG *Fmsg );		/* Fn200 LED Display 				*/
#endif

#define OPEREG_WRITE( idx, data, hmsg )   (COPEREG->Write( COPEREG, (idx), (data), hmsg ))
#define STSREG_READX( idx, pResData, CheckType, hmsg )   (COPEREG->Read( COPEREG, (idx), (pResData), \
																		(CheckType), hmsg ))  /* <S073> */
#define PRMREG_READX( idx, pResData, CheckType, hmsg )	(CPARAMX->Read( (COBJECT *)(CPARAMX),(idx),\
																	(pResData), (CheckType), hmsg ))  /* <S073> */

/*****************************************************************************
  Description: Fn200 Dynamic Auto Tuning Mode Display
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		0: Continue, 1: End
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
LONG FunDynamicAutoTuningMain(FUNMSG *Fmsg)
{
	MSGIF *hMsg = Fmsg->Super;
	LONG rc = 0;
/*--------------------------------------------------------------------------------------------------*/
	if( KPI_CHK_LopHmsg( hMsg ) == (ULONG)(TRUE) )					/* LCD Operator */
	{
		rc = LpxLcdDynamicAutoTuningExe( Fmsg );
	}
/*--------------------------------------------------------------------------------------------------*/
	else if ( KPI_CHK_PopHmsg( hMsg ) == TRUE )						/* Panel Operator */
	{
		rc = LpxPnlDynamicAutoTuningExe( Fmsg );
	}
/*--------------------------------------------------------------------------------------------------*/
	else
	{
		/* Do Nothing */
	}
/*--------------------------------------------------------------------------------------------------*/
	return rc;
}

/*****************************************************************************
  Description: Fn200 Dynamic Auto Tuning Mode Begin
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		OK: Success, Others: Failure
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
LONG FunDynamicAutoTuningBegin(FUNMSG *Fmsg)
{
	LONG	rc = OK;
	BOOL ResFlag = TRUE;
	FNTUNELVLSETV  *pv = (FNTUNELVLSETV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	pv->var.State = FNTUNELVLSETST_INIT;
/*--------------------------------------------------------------------------------------------------*/
	LpxPrmChangeOpeDspMode();								/* すべてのユーザー定数を表示に強制変更		*///<S197>
/*--------------------------------------------------------------------------------------------------*/
/*	Search Register																					*/
/*--------------------------------------------------------------------------------------------------*/
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
		rc = COPEREG->Search( COPEREG, 							/* Search Register	(0x2054)		*/
				OPEANOTCHST_REG,
				0,
				&(pv->var.OpeSpatANotchStIdx),
				MCHK_REG_READ,
				Hmsg);
		if(rc != OK)
		{
			ResFlag = FALSE;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
	if ( ResFlag == TRUE )
	{
		rc = COPEREG->Search( COPEREG, 							/* Search Register	(0x20E0)		*/
				OPEDATGNLVL_REG,
				0,
				&(pv->var.OpeDatGnLevelIdx),
				MCHK_REG_READ,
				Hmsg);
		if(rc != OK)
		{
			ResFlag = FALSE;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
	if ( ResFlag == TRUE )
	{
		rc = COPEREG->Search( COPEREG, 							/* Search Register	(0x20E1)		*/
				OPEDATJRATLVL_REG,
				0,
				&(pv->var.OpeDatJratLevelIdx),
				MCHK_REG_READ,
				Hmsg);
		if(rc != OK)
		{
			ResFlag = FALSE;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
	/* Parameter Table Search */
/*--------------------------------------------------------------------------------------------------*/
	if ( ResFlag == TRUE )
	{
		rc = CPARAMX->Search( (COBJECT *)CPARAMX, 				/* Search Register	(0x0408)		*/
				PRMTFUNCSW_REG,
				0,
				&(pv->var.PrmTFuncSwIdx),
				MCHK_REG_READ,
				Hmsg);
		if(rc != OK)
		{
			ResFlag = FALSE;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/* 	Set Fn200 Dynamic Auto Tuning Mode to Mode Register 											*/
/*--------------------------------------------------------------------------------------------------*/
	if ( ResFlag == TRUE )
	{
		rc = OPEREG_WRITE( pv->var.OpeModeIdx, FNTUNELVLSET_MODE, Hmsg );
	}
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}


/*****************************************************************************
  Description: Fn200 Dynamic Auto Tuning Mode End
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	None
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
void FunDynamicAutoTuningEnd(FUNMSG *Fmsg)
{
	FNTUNELVLSETV  *pv = (FNTUNELVLSETV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------*/
	LpxPrmReChangeOpeDspMode();				/* デジオペ表示設定を元に戻す		*///<S197>
/*--------------------------------------------------------------------------*/
	OPEREG_WRITE( pv->var.OpeSetIdx, TUNELVLSETCMD_STOP, Hmsg );
	OPEREG_WRITE( pv->var.OpeModeIdx, FNNORMAL_MODE, Hmsg );
}


/*****************************************************************************
  Description: Fn200 Dynamic Auto Tuning Mode Execution( LCD Operator )
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		FUN_CONTINUE: Execution Continue, FUN_END: Execution End
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static LONG LpxLcdDynamicAutoTuningExe(FUNMSG *Fmsg)
{
	FNTUNELVLSETV *pv = (FNTUNELVLSETV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
	OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
	BLINKFLG *BlinkFlag = (BLINKFLG *)(Fmsg->pBlnkKick);
	LONG rc = FUN_CONTINUE;													/* 0: continue, 1: end */

/*--------------------------------------------------------------------------------------------------*/
/*			Fn200 Dynamic Auto Tuning Sequence														*/
/*--------------------------------------------------------------------------------------------------*/
	switch( pv->var.State )
	{
/*--------------------------------------------------------------------------------------------------*/
		case FNTUNELVLSETST_INIT: 											/* Initial State 		*/
/*--------------------------------------------------------------------------------------------------*/
			STSREG_READX(pv->var.OpeDatJratLevelIdx, &(pv->var.ResData), MCHK_REG_READ, Hmsg);
			pv->var.DatJratLevel = (USHORT)pv->var.ResData;
			pv->var.State = FNTUNELVLSETST_JLEVELSEL;

			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNTUNELVLSETST_JLEVELSEL:										/* J Level Selection 	*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = FNTUNELVLSETST_END;
			}
			else if ( OpKeyMake.data == TRUE )
			{
				pv->var.State = FNTUNELVLSETST_TUNING;
			}
			else
			{
				if ( OpKeyMake.up == TRUE )		/* Jrat Level Up */
				{
					if( pv->var.DatJratLevel < DAT_JRATLEVEL_MAX )
					{
						pv->var.DatJratLevel++;
					}
					else
					{
						pv->var.DatJratLevel = DAT_JRATLEVEL_MAX;
					}
					/* チューニングレベル設定失敗時　error表示処理 */
					if ( OPEREG_WRITE( pv->var.OpeDatJratLevelIdx, pv->var.DatJratLevel, Hmsg ) != OK )
					{
						BlinkFlag->error = TRUE;
					}
				}
				else if ( OpKeyMake.down == TRUE )		/* Jrat Level Down */
				{
					if ( pv->var.DatJratLevel > DAT_JRATLEVEL_MIN )
					{
						pv->var.DatJratLevel--;
					}
					else
					{
						pv->var.DatJratLevel = DAT_JRATLEVEL_MIN;
					}
					/* チューニングレベル設定失敗時　error表示処理 */
					if ( OPEREG_WRITE( pv->var.OpeDatJratLevelIdx, pv->var.DatJratLevel, Hmsg ) != OK )
					{
						BlinkFlag->error = TRUE;
					}
				}
				else
				{
					/*	Do Nothing	*/
				}
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNTUNELVLSETST_TUNING: 										/* Gain Level Set		*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = FNTUNELVLSETST_END;
			}
			else if( OpKeyMake.svon == TRUE )				/* Execution 2 */
			{
				OPEREG_WRITE( pv->var.OpeSetIdx, TUNELVLSETCMD_EXE2, Hmsg );
			}
			else if ( OpKeyMake.data == TRUE )			/* Parameter Update */
			{
				/*EEPROM書込み処理値　done、error表示操作処理 */
				if ( OPEREG_WRITE( pv->var.OpeSetIdx, TUNELVLSETCMD_PRMUPD, Hmsg ) == OK )
				{
					BlinkFlag->done = TRUE;
				}
				else
				{
					BlinkFlag->error = TRUE;
				}
			}
			else
			{
				if( OpKeyMake.up == TRUE )
				{
					OPEREG_WRITE( pv->var.OpeSetIdx, TUNELVLSETCMD_UP, Hmsg );		/* UP Key */
				}
				else if ( OpKeyMake.down == TRUE )
				{
					OPEREG_WRITE( pv->var.OpeSetIdx, TUNELVLSETCMD_DOWN, Hmsg );	/* Down Key */
				}
				else if ( OpKeyMake.shup == TRUE )
				{
					OPEREG_WRITE( pv->var.OpeSetIdx, TUNELVLSETCMD_STOP, Hmsg );	/* Shift Up Key */
				}
				else
				{
					/*	Do Nothing	*/
				}
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNTUNELVLSETST_END: 												/*	End state		*/
/*--------------------------------------------------------------------------------------------------*/
			FunDynamicAutoTuningEnd( Fmsg );
			rc = FUN_END;
			break;
		default:
			break;
		}

	/* Dynamic Auto Tuning Display */
	FunDynamicAutoTuningDisplay( Fmsg );
	return (rc);
}

/*****************************************************************************
  Description: Fn200 Dynamic Auto Tuning Display( LCD Display )
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	None
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void FunDynamicAutoTuningDisplay( FUNMSG *Fmsg )
{
	FNTUNELVLSETV *pv = (FNTUNELVLSETV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
	UCHAR *ResBuf = Fmsg->pTxBuf;

	/* ALL Clear	*/
	RpxLcdClrTxBuf( ResBuf ); 												/* clear Tx buffer		*/

	RpxLcdClrLineBlnk( 1U, ResBuf );
	RpxLcdClrLineBlnk( 2U, ResBuf );
	RpxLcdClrLineBlnk( 3U, ResBuf );
	RpxLcdClrLineBlnk( 4U, ResBuf );
	/* TITLE	*/
//	RpxLcdSetStr( 0U, 6U, (UCHAR *)"-TuneLvlSet-", ResBuf );		/* <S154> */
	RpxLcdSetStr( 0U, 8U, (UCHAR *)"TunLvlSet", ResBuf );			/* <S154> */

	switch ( pv->var.State )
	{
		case FNTUNELVLSETST_INIT:
			/* Fall Through */
		case FNTUNELVLSETST_JLEVELSEL:
			STSREG_READX(pv->var.OpeDatJratLevelIdx, &(pv->var.ResData), MCHK_REG_READ, Hmsg);
			pv->var.DatJratLevel = (USHORT)pv->var.ResData;
			FunDynamicAutoTuningLcdJLevelTuneDisplay( ResBuf, pv->var.DatJratLevel );
			/* Blink Set	*/
			RpxLcdSetBlnk( 2U, 8U, 1U, ResBuf );
			break;

		case FNTUNELVLSETST_TUNING:
			FunDynamicAutoTuningLcdGnLevelTuneDisplay( Fmsg );
			RpxLcdSetBlnk( 2U, 9U, 1U, ResBuf );
			break;
		case FNTUNELVLSETST_END:
		default:
			break;
	}
	return ;
}

/*****************************************************************************
  Description: Fn200 Dynamic Auto Tuning J Level Set Display Data
------------------------------------------------------------------------------
  Parameters:
	ResBuf;		Display Data Buffer
	JartLevel;	J Level Value
  Return:
	None
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void FunDynamicAutoTuningLcdJLevelTuneDisplay( UCHAR *ResBuf, USHORT JartLevel )
{

	/* TITLE	*/
	RpxLcdSetStr( 1U, 0U, (UCHAR *)"                 ", ResBuf );
	RpxLcdSetStr( 2U, 0U, (UCHAR *)" Mode =          ", ResBuf );
	RpxLcdSetStr( 3U, 0U, (UCHAR *)"                 ", ResBuf );
	/* J Level	*/
	RpxLcdSetDecPnt( 2U, 8U, (ULONG)JartLevel, 1U, (USHORT)(FALSE), 0, ResBuf );
	return;
}


/*****************************************************************************
  Description: Fn200 Dynamic Auto Tuning Gain Level Set Display Data
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	None
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void FunDynamicAutoTuningLcdGnLevelTuneDisplay( FUNMSG *Fmsg )
{
	FNTUNELVLSETV *pv = (FNTUNELVLSETV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
	UCHAR *ResBuf = Fmsg->pTxBuf;

	/* Gain Level Update */
	RpxLcdSetStr( 1U, 0U, (UCHAR *)"                 ", ResBuf );
	RpxLcdSetStr( 2U, 0U, (UCHAR *)" Level =         ", ResBuf );
	RpxLcdSetStr( 3U, 0U, (UCHAR *)"                 ", ResBuf );

	STSREG_READX(pv->var.OpeDatGnLevelIdx, &(pv->var.ResData), MCHK_REG_READ, Hmsg);
	pv->var.DatGnLevel = (USHORT)pv->var.ResData;
	RpxLcdSetDecPnt( 2U, 9U, (ULONG)pv->var.DatGnLevel, 1U, (USHORT)(FALSE), 0, ResBuf );

	RpxLcdSetStr( 4U, 0U, (UCHAR *)"                 ", ResBuf );	/* 表示クリア	*/

	PRMREG_READX(pv->var.PrmTFuncSwIdx, &(pv->var.ResData), MCHK_REG_READ, Hmsg);
	if ( ( pv->var.ResData & 0x000FU ) != 0x0000U )
	{
		RpxLcdSetStr( 4U, 1U, (UCHAR *)"NF1 ", ResBuf );	
	}
	if ( ( pv->var.ResData & 0x0F00U ) != 0x0000U )
	{
		RpxLcdSetStr( 4U, 6U, (UCHAR *)"NF2 ", ResBuf );	
	}

	switch( RpiGetANotchSetStat(Fmsg->Super->pAxRsc->ANotchSeq) & 0x0F)
	{
		case OPTUNE_STS_NOTCH1_OK:		/* １段目ノッチ設定正常完了	*/
		RpxLcdSetBlnk( 4U, 1U, 3U, ResBuf );
			break;
		case OPTUNE_STS_NOTCH2_OK:		/* ２段目ノッチ設定正常完了	*/
		RpxLcdSetBlnk( 4U, 6U, 3U, ResBuf );
			break;
		case OPTUNE_STS_NOTCHQ1_ERR:	/* 1段目ノッチＱ値異常		*/
		RpxLcdSetStr( 4U, 1U, (UCHAR *)"ERR1", ResBuf );
		RpxLcdSetBlnk( 4U, 1U, 4U, ResBuf );
			break;
		case OPTUNE_STS_NOTCHQ2_ERR:	/* ２段目ノッチＱ値異常		*/
		RpxLcdSetStr( 4U, 6U, (UCHAR *)"ERR2", ResBuf );
		RpxLcdSetBlnk( 4U, 6U, 4U, ResBuf );
			break;
		case OPTUNE_STS_NOTCH_INIT:
		RpxLcdClrLineBlnk( 4U, ResBuf );
			break;
		default:
			break;
	}

	return;
}


/*****************************************************************************
  Description: Fn200 Dynamic Auto Tuning Mode Execution( Panel Operator )
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		FUN_CONTINUE: Execution Continue, FUN_END: Execution End
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
#if  SVFSEL_PANELOP_USE == 1
static LONG LpxPnlDynamicAutoTuningExe(FUNMSG *Fmsg)
{
	FNTUNELVLSETV *pv = (FNTUNELVLSETV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
	OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
	BLINKFLG *BlinkFlag = (BLINKFLG *)(Fmsg->pBlnkKick);
	LONG rc = FUN_CONTINUE;													/* 0: continue, 1: end */

/*--------------------------------------------------------------------------------------------------*/
/*			Fn200 Dynamic Auto Tuning Sequence														*/
/*--------------------------------------------------------------------------------------------------*/
	switch( pv->var.State )
	{
/*--------------------------------------------------------------------------------------------------*/
		case FNTUNELVLSETST_INIT: 											/* Initial State 		*/
/*--------------------------------------------------------------------------------------------------*/
			STSREG_READX(pv->var.OpeDatJratLevelIdx, &(pv->var.ResData), MCHK_REG_READ, Hmsg);
			pv->var.DatJratLevel = (USHORT)pv->var.ResData;
			pv->var.State = FNTUNELVLSETST_JLEVELSEL;

			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNTUNELVLSETST_JLEVELSEL:										/* J Level Selection 	*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.data == TRUE )
			{
				pv->var.State = FNTUNELVLSETST_END;
			}
			else if ( OpKeyMake.mode == TRUE )
			{
				pv->var.State = FNTUNELVLSETST_TUNING;
			}
			else
			{
				if ( OpKeyMake.up == TRUE )		/* Jrat Level Up */
				{
					if( pv->var.DatJratLevel < DAT_JRATLEVEL_MAX )
					{
						pv->var.DatJratLevel++;
					}
					else
					{
						pv->var.DatJratLevel = DAT_JRATLEVEL_MAX;
					}
					/* チューニングレベル設定失敗時　error表示処理 */
					if ( OPEREG_WRITE( pv->var.OpeDatJratLevelIdx, pv->var.DatJratLevel, Hmsg ) != OK )
					{
						BlinkFlag->error = TRUE;
					}
				}
				else if ( OpKeyMake.down == TRUE )		/* Jrat Level Down */
				{
					if ( pv->var.DatJratLevel > DAT_JRATLEVEL_MIN )
					{
						pv->var.DatJratLevel--;
					}
					else
					{
						pv->var.DatJratLevel = DAT_JRATLEVEL_MIN;
					}
					/* チューニングレベル設定失敗時　error表示処理 */
					if ( OPEREG_WRITE( pv->var.OpeDatJratLevelIdx, pv->var.DatJratLevel, Hmsg ) != OK )
					{
						BlinkFlag->error = TRUE;
					}
				}
				else
				{
					/*	Do Nothing	*/
				}
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNTUNELVLSETST_TUNING: 										/* Gain Level Set		*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.data == TRUE )
			{
				pv->var.State = FNTUNELVLSETST_END;
			}
			else if( OpKeyMake.shup == TRUE )				/* Execution 2 */
			{
				OPEREG_WRITE( pv->var.OpeSetIdx, TUNELVLSETCMD_EXE2, Hmsg );
			}
			else if ( OpKeyMake.mode == TRUE )			/* Parameter Update */
			{
				/*EEPROM書込み処理値　done、error表示操作処理 */
				if ( OPEREG_WRITE( pv->var.OpeSetIdx, TUNELVLSETCMD_PRMUPD, Hmsg ) == OK )
				{
					BlinkFlag->done = TRUE;
				}
				else
				{
					BlinkFlag->error = TRUE;
				}
			}
			else
			{
				if( OpKeyMake.up == TRUE )
				{
					OPEREG_WRITE( pv->var.OpeSetIdx, TUNELVLSETCMD_UP, Hmsg );		/* UP Key */
				}
				else if ( OpKeyMake.down == TRUE )
				{
					OPEREG_WRITE( pv->var.OpeSetIdx, TUNELVLSETCMD_DOWN, Hmsg );	/* Down Key */
				}
				else
				{
					/*	Do Nothing	*/
				}
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNTUNELVLSETST_END: 												/*	End state		*/
/*--------------------------------------------------------------------------------------------------*/
			FunDynamicAutoTuningEnd( Fmsg );
			rc = FUN_END;
			break;
		default:
			break;
		}

	/* Dynamic Auto Tuning Display ( LED Display ) */
	FunPnlDynamicAutoTuningDisplay( Fmsg );
	return (rc);
}


/*****************************************************************************
  Description: Fn200 Dynamic Auto Tuning Display( LED Display )
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	None
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void FunPnlDynamicAutoTuningDisplay( FUNMSG *Fmsg )
{
	FNTUNELVLSETV *pv = (FNTUNELVLSETV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
	LEDDSP *DspData = (LEDDSP *)&(Fmsg->pTxBuf[PNLRSP_LEDCODE_IDX]);/* set display data buffer */

	/* ALL Clear	*/
	MlibResetByteMemory( DspData, sizeof(LEDDSP) );	 					/* clear Tx buffer		*/

	switch ( pv->var.State )
	{
		case FNTUNELVLSETST_INIT:
			/* Fall Through */
		case FNTUNELVLSETST_JLEVELSEL:
			/*led="d   x"*/
			RpxLedSetStr( (UCHAR *)"d", DspData, 4U, 1U );
			STSREG_READX(pv->var.OpeDatJratLevelIdx, &(pv->var.ResData), MCHK_REG_READ, Hmsg);
			pv->var.DatJratLevel = (USHORT)(pv->var.ResData);
			RpxLedSetValDec( (USHORT)(pv->var.DatJratLevel), DspData, 0U, 1U, FALSE, 0U );
			break;

		case FNTUNELVLSETST_TUNING:
			/* Fall Through */
		case FNTUNELVLSETST_END:
			RpxLedSetStr( (UCHAR *)"L", DspData, 4U, 1U );
			STSREG_READX(pv->var.OpeDatGnLevelIdx, &(pv->var.ResData), MCHK_REG_READ, Hmsg);
			pv->var.DatGnLevel = (USHORT)pv->var.ResData;
			RpxLedSetValDec( (USHORT)(pv->var.DatGnLevel), DspData, 0U, 1U, FALSE, 0U );
			break;

		default:
			break;
	}
	return ;
}



#endif	/* #if  SVFSEL_PANELOP_USE == 1 */




/***************************************** end of file **********************************************/
