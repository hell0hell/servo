/*****************************************************************************
Description		: Fn203 One Parameter Tuning
-----------------------------------------------------------------------------
Author		: SHCORE
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

Functions	  :
	-- APIs --
	FunOneParamTuningMain();				:Fn203 One Parameter Tuning Mode Display
	FunOneParamTuningBegin();				:Fn203 One Parameter Tuning Mode Begin
	FunOneParamTuningEnd();					:Fn203 One Parameter Tuning Mode End

	-- Locals --
	LpxLcdFunOneParamTuningExe();			:LCD Operator Fn203 Execution
	FunOnePrmTuningModeInitSet();			:Fn203 Mode Initial Set
	FunOneParamTuningDisplay();				:Fn203 Mode Main Display
	FunOneParamTuningLcdInitDisplay();		:LCD Initial Display
	FunOneParamTuningLcdTuneDisplay();		:LCD Tuning Level Display
	FunOneParamTuningLcdGainDisplay();		:LCD Gain Display
	FunOneParamTuningLcdSelectDisplay();	:LCD Select Display

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
K.Liu		2013.06.20		created
K.Liu		2013.09.02		Add Panel Operator Function
K.Liu		2013.12.09		Modify
*****************************************************************************/
#include "FunOnePrmTun.h"
#include "Parameter.h"
#include "Monitor.h"

/*--------------------------------------------------------------------------------------------------*/
/* Operation Register 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define	OPEMODE_REG1			(0x0000)		/* 1st Operation Mode Register 						*/
#define	OPESET_REG1				(0x0001)		/* 1st Operation Mode Set Register					*/
#define	OPEADATFILTP_REG		(0x0095)		/* Filter Type Setting Register 					*/
#define	OPESPATMODE_REG			(0x0050)		/* One Parameter Tuning Mode Register 				*/
#define	OPESPATLVL_REG			(0x0051)		/* One Parameter Tuning Level Operation Register 	*/
#define	OPESPATFFLVL_REG		(0x0052)		/* One Parameter Tuning FF Level Operation Register */
#define	OPESPATLVLCHGST_REG		(0x0053)		/* One Parameter Tuning FF Level Changing Completed Register */
#define	OPESPATANOTCHST_REG		(0x0054)		/* One Parameter Tuning Auto Notch Completed Register */
#define	OPESPATARESST_REG		(0x0055)		/* One Parameter Tuning Auto 'A' Completed Register */
/*--------------------------------------------------------------------------------------------------*/
#define	PRMLOOPHZ_REG			(0x0100)		/* Speed Loop Gain Register 		*/
#define	PRMPITIME_REG			(0x0101)		/* Speed Loop 積分時定数 Register 	*/
#define	PRMPOSGN_REG			(0x0102)		/* Position Loop Gain Register 		*/
#define	PRMJART_REG				(0x0103)		/* Jart Moment Rate Register 		*/
#define	PRMMDGN_REG				(0x0141)		/* モデル追従制御ゲイン Register 	*/
#define	PRMAVIBSW_REG			(0x0160)		/* 制振制御関連スイッチ	 Register 	*/
#define	PRMTFUNCSW_REG			(0x0408)		/* トルク関係機能スイッチ Register 	*/
/*--------------------------------------------------------------------------------------------------*/
#define UNACTGAIN_REG			(0x001D)		/* Un01D  : 有効ゲイン Register 	*/
/*--------------------------------------------------------------------------------------------------*/
/* Operation Mode	 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define	FNONEPRMTUN_MODE		(0x1203U) 		/* Fn203 One Parameter Tuning Mode 	*/
#define	FNNORMAL_MODE			(0x0000U)		/* Normal Mode						*/
/*--------------------------------------------------------------------------------------------------*/
/* Operation Command 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define	ONEPRMTUNCMD_STOP		(0x0000)
#define	ONEPRMTUNCMD_EXE	 	(0x0001)
#define	ONEPRMTUNCMD_PRMUPD 	(0x0007)		/* Parameter Update 		*/
#define	ONEPRMTUNCMD_EXE2	 	(0x0008)		/* Auxiliary Execution		*//* TODO:修正値：0x0008*/
/*--------------------------------------------------------------------------------------------------*/
#define	FUN_CONTINUE			0				/* Continue */
#define	FUN_END					1				/* End		*/
/*--------------------------------------------------------------------------------------------------*/

/****************************************************************************************************/
/*		Constant Definition																			*/
/****************************************************************************************************/
#define	OPTUNE_FLAG_FBSEL		0				/* フィードバックゲイン選択							*/
#define	OPTUNE_FLAG_FFSEL		1				/* フィードフォワードゲイン選択						*/
/*--------------------------------------------------------------------------------------------------*/
#define	OPTUNE_JARTTUN_MAX		5U				/* 慣性モーメント/重量比調整最大桁					*/
/*--------------------------------------------------------------------------------------------------*/
#define	OPTUNE_SPDMODE_MIN		0U				/* 1パラモード下限値								*/
#define	OPTUNE_SPDMODE_MAX		1U				/* 1パラモード上限値	速度制御 の場合				*/
#define	OPTUNE_POSMODE_MIN		0U				/* 1パラモード下限値								*/
#define	OPTUNE_POSMODE_MAX		3U				/* 1パラモード上限値	位置制御の場合				*/
/*--------------------------------------------------------------------------------------------------*/
#define	OPTUNE_FILTYPE_MAX		3U				/* One Parameter Tuning Filter Type Max				*/
#define	OPTUNE_FILTYPE_MIN		1U				/* One Parameter Tuning Filter Type Min				*/
/*--------------------------------------------------------------------------------------------------*/
#define OPTUNE_FFTMOUT			2U				/* FFレベル変更完了状態：変更失敗(タイムアウト)				*/
/*--------------------------------------------------------------------------------------------------*/
enum ARES_STATE		/* Ａ型制振自動設定結果 */
{
	OPTUNE_ST_ARES_INIT	= 0x00,	/* Ａ型制振設定初期				*/
	OPTUNE_ST_ARES_OK	= 0x01,	/* Ａ型制振設定正常完了			*/
	OPTUNE_ST_ARES_ERR	= 0x02	/* Ａ型制振上下限異常			*/
};


/****************************************************************************************************/
/*																									*/
/*		Function List																				*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Public Function																				*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunOneParamTuningBegin(FUNMSG *Fmsg); 		/* Fn203 One Parameter Tuning Mode Begin	*/
LONG FunOneParamTuningMain(FUNMSG *Fmsg);		/* Fn203 One Parameter Tuning Mode Display	*/
void FunOneParamTuningEnd(FUNMSG *Fmsg);		/* Fn203 One Parameter Tuning Mode End		*/

/*--------------------------------------------------------------------------------------------------*/
/*		Private Function																			*/
/*--------------------------------------------------------------------------------------------------*/
static LONG LpxLcdOneParamTuningExe(FUNMSG *Fmsg);				/* LCD Operator Fn203 Execution 	*/
static void FunOnePrmTuningModeInitSet( FUNMSG *Fmsg );			/* Fn203 Mode Initial Set 			*/
static void FunOneParamTuningDisplay( FUNMSG *Fmsg );			/* Fn203 Mode Main Display 			*/
static void	FunOneParamTuningLcdInitDisplay( FUNMSG *Fmsg );	/* LCD Initial Display  			*/
static void	FunOneParamTuningLcdTuneDisplay( FUNMSG *Fmsg );	/* LCD Tuning Level Display 		*/
static void	FunOneParamTuningLcdGainDisplay( FUNMSG *Fmsg );	/* LCD Gain Display 				*/
static void	FunOneParamTuningLcdSelectDisplay( FUNMSG *Fmsg );	/* LCD Select Display 				*/


#if  SVFSEL_PANELOP_USE != 1
#define LpxPnlOneParamTuningExe( x ) (1)
#define FunPnlOneParamTuningDisplay( x )	()
#else
static LONG LpxPnlOneParamTuningExe(FUNMSG *hMsg);				/* Panel Operator Fn203 Execution 	*/
static void FunPnlOneParamTuningDisplay( FUNMSG *Fmsg );		/* Panel Operator Display */
#endif

#define OPEREG_WRITE( idx, data, hmsg )   (COPEREG->Write( COPEREG, (idx), (data), hmsg ))
#define STSREG_READX( idx, pResData, CheckType, hmsg )   (COPEREG->Read( COPEREG, (idx), (pResData), \
																		(CheckType), hmsg ))  /* <S073> */

#define PRMREG_WRITE( idx, data, hmsg )   (CPARAMX->Write( (CPARAMX), (idx), (data), hmsg ))
#define PRMREG_READX( idx, pResData, CheckType, hmsg )	(CPARAMX->Read( (COBJECT *)(CPARAMX),(idx),\
																	(pResData), (CheckType), hmsg ))  /* <S073> */
																	
#define UNREG_READX( idx, pResData, CheckType, hmsg )    (CUNREGX->Read( (COBJECT *)(CUNREGX), (idx),\
															 	 	 (pResData), (CheckType), hmsg ))  /* <S073> */

/*****************************************************************************
  Description: Fn203 One Parameter Tuning Mode Display
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		0: Continue, 1: End
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
LONG FunOneParamTuningMain(FUNMSG *Fmsg)
{
	MSGIF *hMsg = Fmsg->Super;
	LONG rc = 0;
/*--------------------------------------------------------------------------------------------------*/
	if( KPI_CHK_LopHmsg( hMsg ) == (ULONG)(TRUE) )					/* LCD Operator */
	{
		rc = LpxLcdOneParamTuningExe( Fmsg );
	}
/*--------------------------------------------------------------------------------------------------*/
	else if ( KPI_CHK_PopHmsg( hMsg ) == TRUE )						/* Panel Operator */
	{
		rc = LpxPnlOneParamTuningExe( Fmsg );
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
  Description: Fn203 One Parameter Tuning Mode Begin
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		OK: Success, Others: Failure
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
LONG FunOneParamTuningBegin(FUNMSG *Fmsg)
{
	LONG	rc = OK;
	BOOL ResFlag = TRUE;
	FNONEPRMTUNV  *pv = (FNONEPRMTUNV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	pv->var.State = FNONEPRMTUNST_INIT;
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
		rc = COPEREG->Search( COPEREG,					/* Search Register (Filter Type Setting)	*/
				OPEADATFILTP_REG,
				0,
				&(pv->var.OpeAdatFilTypeIdx),
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
		rc = COPEREG->Search( COPEREG,				/* Search Register (One Parameter Tuning Mode)	*/
				OPESPATMODE_REG,
				0,
				&(pv->var.OpeSpatModeIdx),
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
		rc = COPEREG->Search( COPEREG,				/* Search Register (One Parameter Tuning Level)	*/
				OPESPATLVL_REG,
				0,
				&(pv->var.OpeSpatLevelIdx),
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
		rc = COPEREG->Search( COPEREG,			/* Search Register (One Parameter Tuning FF Level)	*/
				OPESPATFFLVL_REG,
				0,
				&(pv->var.OpeSpatFFLevelIdx),
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
		rc = COPEREG->Search( COPEREG,		/* Search Register (Tuning Level Changing Completed)	*/
				OPESPATLVLCHGST_REG,
				0,
				&(pv->var.OpeSpatLvlChgStIdx),
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
		rc = COPEREG->Search( COPEREG,			/* Search Register (Tuning Auto Notch Completed)	*/
				OPESPATANOTCHST_REG,
				0,
				&(pv->var.OpeSpatANotchStIdx),
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
		rc = COPEREG->Search( COPEREG,				/* Search Register (Tuning Auto 'A' Completed)	*/
				OPESPATARESST_REG,
				0,
				&(pv->var.OpeSpatAresStIdx),
				MCHK_REG_READ,
				Hmsg);
		if( rc != OK )
		{
			ResFlag = FALSE;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
	/*	Parameter Table Search			*/
/*--------------------------------------------------------------------------------------------------*/
	if( ResFlag == TRUE )
	{
		rc = CPARAMX->Search( (COBJECT *)CPARAMX,			/* Search Register (Spead Loop Gain)	*/
				PRMLOOPHZ_REG,
				0,
				&(pv->var.PrmLoopHzIdx),
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
		rc = CPARAMX->Search( (COBJECT *)CPARAMX,		/* Search Register (Speed Loop 積分時定数)	*/
				PRMPITIME_REG,
				0,
				&(pv->var.PrmPiTimeIdx),
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
		rc = CPARAMX->Search( (COBJECT *)CPARAMX,		/* Search Register (Position Loop Gain)		*/
				PRMPOSGN_REG,
				0,
				&(pv->var.PrmPosGnIdx),
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
		rc = CPARAMX->Search( (COBJECT *)CPARAMX,			/* Search Register (慣性モーメント比)	*/
				PRMJART_REG,
				0,
				&(pv->var.PrmJratIdx),
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
		rc = CPARAMX->Search( (COBJECT *)CPARAMX,		/* Search Register (モデル追従制御ゲイン)	*/
				PRMMDGN_REG,
				0,
				&(pv->var.PrmMdlGnIdx),
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
		rc = CPARAMX->Search( (COBJECT *)CPARAMX,		/* Search Register (制振制御関連スイッチ)	*/
				PRMAVIBSW_REG,
					0,
				&(pv->var.PrmAvibSwIdx),
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
		rc = CPARAMX->Search( (COBJECT *)CPARAMX,		/* Search Register (トルク関係機能スイッチ)	*/
				PRMTFUNCSW_REG,
				0,
				&(pv->var.PrmTFuncSwIdx),
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
		rc = CUNREGX->Search( (COBJECT *)CUNREGX,		/* Search Register (Un014  : 有効ゲイン)	*/
				UNACTGAIN_REG,
				0,
				&(pv->var.UnActGainIdx),
				MCHK_REG_READ,
				Hmsg);
	}
	if( rc == OK )
	{
		rc = OPEREG_WRITE( pv->var.OpeModeIdx, FNONEPRMTUN_MODE, Hmsg );
	}
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}


/*****************************************************************************
  Description: Fn203 One Parameter Tuning Mode End
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	None
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
void FunOneParamTuningEnd(FUNMSG *Fmsg)
{
	FNONEPRMTUNV  *pv = (FNONEPRMTUNV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------*/
	LpxPrmReChangeOpeDspMode();				/* デジオペ表示設定を元に戻す		*///<S197>
/*--------------------------------------------------------------------------*/
	OPEREG_WRITE( pv->var.OpeSetIdx, ONEPRMTUNCMD_STOP, Hmsg );
	OPEREG_WRITE( pv->var.OpeModeIdx, FNNORMAL_MODE, Hmsg );
}


/*****************************************************************************
  Description: Fn203 One Parameter Tuning Mode Execution(LCD Operator)
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		FUN_CONTINUE: Execution Continue, FUN_END: Execution End
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static LONG LpxLcdOneParamTuningExe(FUNMSG *Fmsg)
{
	FNONEPRMTUNV *pv = (FNONEPRMTUNV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
	OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
	BLINKFLG *BlinkFlag = (BLINKFLG *)(Fmsg->pBlnkKick);
	SHORT	CalcTmp, i;
	ULONG	ulWork;
	LONG rc = FUN_CONTINUE;													/* 0: continue, 1: end */
	ONEPRMTUNE	*OnePrmTune;
	OnePrmTune = Fmsg->Super->pAxRsc->OnePrmTune;

	/* Un01D 判定追加 */
	UNREG_READX( pv->var.UnActGainIdx, ( ULONG * )( &(pv->var.UnActGain) ), MCHK_REG_READ, Hmsg );
	if ( pv->var.UnActGain != 1 )
	{
		if ( OpKeyMake.mode == TRUE )
		{
			pv->var.State = FNONEPRMTUNST_END;
		}
		else if ( (USHORT)(*(Fmsg->pKeyMk)) != 0U )
		{
			BlinkFlag->no_op = TRUE;
		}
		else
		{
			/* Do Nothing */
		}
	}

	switch( pv->var.State )
	{
/*--------------------------------------------------------------------------------------------------*/
		case FNONEPRMTUNST_INIT: 											/* Initial State 		*/
/*--------------------------------------------------------------------------------------------------*/
			FunOnePrmTuningModeInitSet( Fmsg );

			pv->var.AdatFilType = 2U;
			OPEREG_WRITE( pv->var.OpeAdatFilTypeIdx, pv->var.AdatFilType, Hmsg );

			pv->var.TunSel = OPTUNE_FLAG_FBSEL;
			pv->var.DPos = 0U;			/* 点滅桁数  Initial */

			/* 慣性モーメント比	 Initial */
			PRMREG_READX(pv->var.PrmJratIdx, &(pv->var.Jrate), MCHK_REG_READ, Hmsg);

			/* Initial Variable */
			STSREG_READX(pv->var.OpeSpatLevelIdx, &(pv->var.SpatLevel), MCHK_REG_READ, Hmsg);
			STSREG_READX(pv->var.OpeSpatFFLevelIdx, &(pv->var.SpatFFLevel), MCHK_REG_READ, Hmsg);
			pv->var.FfSetting = (USHORT)(pv->var.SpatLevel);
			pv->var.SpatLevel = ( ( pv->var.SpatLevel / 10U ) * 10U );
			pv->var.SpatFFLevel = ( ( pv->var.FfSetting / 10U ) * 10U );
			pv->var.FfSetting = ( ( pv->var.FfSetting / 10U ) * 10U );
			pv->var.DigMax = 2U;

			OPEREG_WRITE( pv->var.OpeModeIdx, FNNORMAL_MODE, Hmsg );
			pv->var.State = FNONEPRMTUNST_JSET;
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNONEPRMTUNST_JSET: 									/* 慣性モーメント比設定処理		*/
/*--------------------------------------------------------------------------------------------------*/
			/* チューニングレベル増減用データ	*/
			CalcTmp = 1;
			for(i = 0; (UCHAR)(i) < pv->var.DPos; i++)
			{
				CalcTmp = CalcTmp * 10;
			}
			
			/* Process Key */
			if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = FNONEPRMTUNST_END;
			}
			else if ( OpKeyMake.data == TRUE )
			{
				pv->var.DPos = 0U;
				/* Parameter Update */
				PRMREG_WRITE( pv->var.PrmJratIdx, pv->var.Jrate, Hmsg );	/*To Be Determined */
				pv->var.State = FNONEPRMTUNST_MODESEL;
			}
			else
			{
				if ( OpKeyMake.up == TRUE )
				{
					ulWork = pv->var.Jrate + ( ULONG )CalcTmp;
					pv->var.Jrate = (ULONG)TunePrmLimitSigned( (ULONG)(ulWork), &pndef_jrate );
				}
				else if ( OpKeyMake.down == TRUE )
				{
					ulWork = pv->var.Jrate - ( ULONG )CalcTmp;
					pv->var.Jrate = (ULONG)TunePrmLimitSigned( (ULONG)(ulWork), &pndef_jrate );
				}
				else if ( OpKeyMake.shup == TRUE )
				{
					if( pv->var.DPos < ( OPTUNE_JARTTUN_MAX - 1U ) )
					{
						pv->var.DPos++;	/* 桁移動	*/
					}
					else
					{
						pv->var.DPos = 0U;
					}
				}
				else if ( OpKeyMake.shdown == TRUE )
				{
					if( ( pv->var.DPos > 0U ) && ( pv->var.DPos <= ( OPTUNE_JARTTUN_MAX - 1U ) ) )
					{
						pv->var.DPos--;	/* 桁移動	*/
					}
					else
					{
						pv->var.DPos = OPTUNE_JARTTUN_MAX - 1U;
					}
				}
				else
				{
					/* Do Nothing */
				}
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNONEPRMTUNST_MODESEL: 									/*	 Mode Selection 		*/
/*--------------------------------------------------------------------------------------------------*/
			/* Process Key */
			if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = FNONEPRMTUNST_END;
			}
			else if ( OpKeyMake.data == TRUE )
			{
				pv->var.DPos = 0U;
				OPEREG_WRITE( pv->var.OpeSpatModeIdx, pv->var.SpatMode, Hmsg );
				OPEREG_WRITE( pv->var.OpeAdatFilTypeIdx, pv->var.AdatFilType, Hmsg );

/*--------------------------------------------------------------------------------------------------*/
/* Set Fn203 One Parameter Tuning Mode to Mode Register 											*/
/*--------------------------------------------------------------------------------------------------*/
				OPEREG_WRITE( pv->var.OpeModeIdx, FNONEPRMTUN_MODE, Hmsg );
				pv->var.State = FNONEPRMTUNST_PREDISP;
			}
			else if ( OpKeyMake.scroll == TRUE )
			{
				pv->var.DPos = 0U;
				pv->var.State = FNONEPRMTUNST_FILSEL;
			}
			else
			{
				if ( OpKeyMake.up == TRUE )
				{
					if( pv->var.SpatMode < pv->var.ModeMax )
					{
						pv->var.SpatMode++;
					}
					else
					{
						pv->var.SpatMode = pv->var.ModeMax;
					}
				}
				else if ( OpKeyMake.down == TRUE )
				{
					if( pv->var.SpatMode > pv->var.ModeMin )
					{
						pv->var.SpatMode--;
					}
					else
					{
						pv->var.SpatMode = pv->var.ModeMin;
					}
				}
				else
				{
					/* Do Nothing */
				}
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNONEPRMTUNST_FILSEL: 										/* Filter Type Selection	*/
/*--------------------------------------------------------------------------------------------------*/
			/* Process Key */
			if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = FNONEPRMTUNST_END;
			}
			else if ( OpKeyMake.data == TRUE )
			{
				pv->var.DPos = 0U;
				OPEREG_WRITE( pv->var.OpeSpatModeIdx, pv->var.SpatMode, Hmsg );
				OPEREG_WRITE( pv->var.OpeAdatFilTypeIdx, pv->var.AdatFilType, Hmsg );

/*--------------------------------------------------------------------------------------------------*/
/* Set Fn203 One Parameter Tuning Mode to Mode Register 											*/
/*--------------------------------------------------------------------------------------------------*/
				OPEREG_WRITE( pv->var.OpeModeIdx, FNONEPRMTUN_MODE, Hmsg );
				pv->var.State = FNONEPRMTUNST_PREDISP;
			}
			else if ( OpKeyMake.scroll == TRUE )
			{
				pv->var.DPos = 0U;
				pv->var.State = FNONEPRMTUNST_MODESEL;
			}
			else
			{
				if ( OpKeyMake.up == TRUE )
				{
					if( pv->var.AdatFilType < OPTUNE_FILTYPE_MAX )
					{
						pv->var.AdatFilType++;
					}
				}
				else if ( OpKeyMake.down == TRUE )
				{
					if( pv->var.AdatFilType > OPTUNE_FILTYPE_MIN )
					{
						pv->var.AdatFilType--;
					}
				}
				else
				{
					/* Do Nothing */
				}
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNONEPRMTUNST_PREDISP: 								/* 調整前ゲイン表示処理 		*/
/*--------------------------------------------------------------------------------------------------*/
			/* Process Key */
			if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = FNONEPRMTUNST_END;
			}
			else if ( OpKeyMake.data == TRUE )
			{
//				OPEREG_WRITE( pv->var.OpeSetIdx, ONEPRMTUNCMD_EXE, Hmsg );
				pv->var.State = FNONEPRMTUNST_TUNING;
			}
			else
			{
				/* Do Nothing */
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNONEPRMTUNST_TUNING: 									/* 1パラ調整処理				*/
/*--------------------------------------------------------------------------------------------------*/
			STSREG_READX(pv->var.OpeSpatLvlChgStIdx, &(pv->var.ResData), MCHK_REG_READ, Hmsg);
			if ( pv->var.ResData == OPTUNE_FFTMOUT )			/* FF変更失敗(タイムアウト)		*/
			{
				STSREG_READX(pv->var.OpeSpatFFLevelIdx, &(pv->var.SpatFFLevel), MCHK_REG_READ, Hmsg);
				pv->var.FfSetting = (USHORT)(pv->var.SpatFFLevel);		/* FFレベルを元に戻す			*/
			}

			CalcTmp = 10;
			for(i = 0; (UCHAR)(i) < pv->var.DPos; i++)
			{
				CalcTmp = CalcTmp * 10;
			}
			/* Process Key */
			if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = FNONEPRMTUNST_END;
			}
			else if ( OpKeyMake.data == TRUE )
			{
				OPEREG_WRITE( pv->var.OpeSetIdx, ONEPRMTUNCMD_EXE, Hmsg );
				pv->var.State = FNONEPRMTUNST_POSTDISP;
			}
			else if ( OpKeyMake.svon == TRUE )
			{
				OPEREG_WRITE( pv->var.OpeSetIdx, ONEPRMTUNCMD_EXE2, Hmsg );
			}
			else if ( OpKeyMake.scroll == TRUE )		/* FF/FB Tuning Level Selection 	*/
			{
				if ( pv->var.TunSel == OPTUNE_FLAG_FBSEL )
				{
					pv->var.TunSel = OPTUNE_FLAG_FFSEL;
				}
				else
				{
					pv->var.TunSel = OPTUNE_FLAG_FBSEL;
				}
			}
			else
			{
				if ( OpKeyMake.up == TRUE )
				{
					if ( pv->var.TunSel == OPTUNE_FLAG_FBSEL )
					{
						STSREG_READX(pv->var.OpeSpatLevelIdx, &(pv->var.SpatLevel), MCHK_REG_READ, Hmsg);
						ulWork = pv->var.SpatLevel + (ULONG)(CalcTmp);
						pv->var.SpatLevel = (ULONG)TunePrmLimitSigned( (ULONG)(ulWork), &pndef_loophz );
						OPEREG_WRITE( pv->var.OpeSpatLevelIdx, pv->var.SpatLevel, Hmsg );
					}
					else
					{
						ulWork = pv->var.FfSetting + (ULONG)(CalcTmp);
						pv->var.FfSetting = (ULONG)TunePrmLimitSigned( (ULONG)(ulWork), &pndef_mdlgn );
					}
				}
				else if ( OpKeyMake.down == TRUE )
				{
					if( OnePrmTune->var.NotchChk == FALSE )	/* Notch Check Switch */
					{
						if ( pv->var.TunSel == OPTUNE_FLAG_FBSEL )
						{
							STSREG_READX(pv->var.OpeSpatLevelIdx, &(pv->var.SpatLevel), MCHK_REG_READ, Hmsg);
							ulWork = pv->var.SpatLevel - (ULONG)(CalcTmp);
							pv->var.SpatLevel = (ULONG)TunePrmLimitSigned( (ULONG)(ulWork), &pndef_loophz );
							OPEREG_WRITE( pv->var.OpeSpatLevelIdx, pv->var.SpatLevel, Hmsg );
						}
						else
						{
							ulWork = pv->var.FfSetting - (ULONG)(CalcTmp);
							pv->var.FfSetting = (ULONG)TunePrmLimitSigned( (ULONG)(ulWork), &pndef_mdlgn );
						}
					}
				}
				else if ( OpKeyMake.shup == TRUE )		/* Shift Up 	*/
				{
					if( pv->var.DPos < ( pv->var.DigMax - 1U ) )
					{
						pv->var.DPos++;					/* 桁移動		*/
					}
					else
					{
						pv->var.DPos = 0U;
					}
				}
				else if ( OpKeyMake.shdown == TRUE )		/* Shift Down 	*/
				{
					if( ( pv->var.DPos > 0U ) && ( pv->var.DPos <= ( pv->var.DigMax - 1U ) ) )
					{
						pv->var.DPos--;					/* 桁移動		*/
					}
					else
					{
						pv->var.DPos = pv->var.DigMax - 1U;
					}
				}
				else
				{
					/* Do Nothing */
				}
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNONEPRMTUNST_POSTDISP: 									/* 調整後ゲイン表示処理		*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = FNONEPRMTUNST_END;
			}
			else if ( OpKeyMake.data == TRUE )
			{
				if ( OPEREG_WRITE( pv->var.OpeSetIdx, ONEPRMTUNCMD_PRMUPD, Hmsg ) == OK )
				{
					BlinkFlag->done = TRUE;
				}
				else
				{
					BlinkFlag->error = TRUE;
				}
			}
			else if ( OpKeyMake.shup == TRUE )
			{
				pv->var.State = FNONEPRMTUNST_TUNING;
			}
			else
			{
				/* Do Nothing */
			}
			break;

/*--------------------------------------------------------------------------------------------------*/
		case FNONEPRMTUNST_END: 												/* 終了処理			*/
/*--------------------------------------------------------------------------------------------------*/
			FunOneParamTuningEnd( Fmsg );
			rc = FUN_END;
			break;

		default:
			break;
	}
	FunOneParamTuningDisplay( Fmsg );
	return rc;
}

/*****************************************************************************
  Description: Fn203 One Parameter Tuning Mode Initial Set
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	None
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void FunOnePrmTuningModeInitSet( FUNMSG *Fmsg )
{
	FNONEPRMTUNV *pv = (FNONEPRMTUNV *)(Fmsg->pVar);
	MSGIF *hMsg = Fmsg->Super;
	UCHAR CtrlMode = Fmsg->Super->pAxRsc->BaseControls->CtrlModeSet.CtrlMode.b.cm;
	BOOL BaseEnable = Fmsg->Super->pAxRsc->SeqCtrlOut->BaseEnableReq;

//	STSREG_READX(pv->var.OpeSpatModeIdx, &(pv->var.SpatMode), MCHK_REG_READ, Hmsg);
//	if( pv->var.SpatMode <= 1 )
//	if( ((CtrlMode != CTRLMODE_POS) && (BaseEnable == TRUE)) ||
	if( (CtrlMode != CTRLMODE_POS) || (KPI_CHK_PopHmsg( hMsg ) == TRUE) )
	{		/* 速度制御 の場合 */
		pv->var.ModeMax = OPTUNE_SPDMODE_MAX;
		pv->var.ModeMin = OPTUNE_SPDMODE_MIN;
		pv->var.SpatMode = 0U;
	}
	else
	{		/* 位置制御の場合 */
		pv->var.ModeMax = OPTUNE_POSMODE_MAX;
		pv->var.ModeMin = OPTUNE_POSMODE_MIN;
		pv->var.SpatMode = 2U;
	}
	return;
}

/*****************************************************************************
  Description: Fn203 One Parameter Tuning Mode Main Display
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	None
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void FunOneParamTuningDisplay( FUNMSG *Fmsg )
{
	FNONEPRMTUNV *pv = (FNONEPRMTUNV *)(Fmsg->pVar);
	UCHAR *ResBuf = Fmsg->pTxBuf;

	RpxLcdClrTxBuf( ResBuf ); 					/* clear Tx buffer */
	/* TITLE	*/
//	RpxLcdSetStr( 0U, 6U, (UCHAR *)"-OnePrmTun-", ResBuf );		/* <S154> */
	RpxLcdSetStr( 0U, 8U, (UCHAR *)"OnePrmTun", ResBuf );		/* <S154> */
	RpxLcdClrLineBlnk( 1U, ResBuf );
	RpxLcdClrLineBlnk( 2U, ResBuf );
	RpxLcdClrLineBlnk( 3U, ResBuf );
	RpxLcdClrLineBlnk( 4U, ResBuf );

	switch ( pv->var.State )
	{
		case FNONEPRMTUNST_INIT:
			/* Fall Through */
		case FNONEPRMTUNST_JSET:
			FunOneParamTuningLcdInitDisplay( Fmsg );
			break;

		case FNONEPRMTUNST_MODESEL:
			FunOneParamTuningLcdSelectDisplay( Fmsg );
			/* Blink Set	*/
			RpxLcdSetBlnk( 3U, 14U, 1U, ResBuf );
			RpxLcdClrLineBlnk( 4U, ResBuf );
			break;

		case FNONEPRMTUNST_FILSEL:
			FunOneParamTuningLcdSelectDisplay( Fmsg );
			/* Blink Set	*/
			RpxLcdSetBlnk( 4U, 14U, 1U, ResBuf );
			RpxLcdClrLineBlnk( 3U, ResBuf );
			break;

		case FNONEPRMTUNST_PREDISP:		/* チューニング前のデータ表示 LCD専用	*/
			/* Fall Through */
		case FNONEPRMTUNST_POSTDISP:		/* チューニング後のデータ表示 LCD専用	*/
			FunOneParamTuningLcdGainDisplay( Fmsg );
			break;

		case FNONEPRMTUNST_TUNING:
//			/* Fall Through */
//		case FNONEPRMTUNST_END:
			FunOneParamTuningLcdTuneDisplay( Fmsg );
			break;

		default:
			break;
	}
	return ;
}

/*****************************************************************************
  Description: Fn203 One Parameter Tuning LCD Initial Dispaly
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	None
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void	FunOneParamTuningLcdInitDisplay( FUNMSG *Fmsg )
{
	FNONEPRMTUNV *pv = (FNONEPRMTUNV *)(Fmsg->pVar);
	UCHAR *ResBuf = Fmsg->pTxBuf;

	/* TITLE	*/
	RpxLcdSetStr( 2U, 0U, (UCHAR *)"Pn103=", ResBuf );
	/* Tuning Mode	*/
	RpxLcdSetDecPnt( 2U, 7U, (ULONG)pv->var.Jrate, 5U, (USHORT)(FALSE), 0, ResBuf );
	RpxLcdSetBlnk( 2U, (USHORT)(11U - pv->var.DPos), 1U, ResBuf );

	return;
}

/*****************************************************************************
  Description: Fn203 One Parameter Tuning Select Display
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	None
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void	FunOneParamTuningLcdSelectDisplay( FUNMSG *Fmsg )
{
	FNONEPRMTUNV *pv = (FNONEPRMTUNV *)(Fmsg->pVar);
	UCHAR *ResBuf = Fmsg->pTxBuf;

	/* TITLE	*/
	RpxLcdSetStr( 1U, 0U, (UCHAR *)"Setting", ResBuf );
	RpxLcdSetStr( 2U, 0U, (UCHAR *)"                 ", ResBuf );
	RpxLcdSetStr( 3U, 0U, (UCHAR *)"Tuning Mode = ", ResBuf );
	/* Tuning Mode	*/
	RpxLcdSetDecPnt( 3U, 14U, (ULONG)pv->var.SpatMode, 1U, (USHORT)(FALSE), 0, ResBuf );
	/* Tuning Type	*/
	RpxLcdSetStr( 4U, 7U, (UCHAR *)"Type = ", ResBuf );
	RpxLcdSetDecPnt( 4U, 14U, (ULONG)pv->var.AdatFilType, 1U, (USHORT)(FALSE), 0, ResBuf );

	return;
}

/*****************************************************************************
  Description: Fn203 One Parameter Tuning Gain Display
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	None
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void	FunOneParamTuningLcdGainDisplay( FUNMSG *Fmsg )
{
	FNONEPRMTUNV *pv = (FNONEPRMTUNV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
	UCHAR *ResBuf = Fmsg->pTxBuf;

	RpxLcdSetStr( 1U, 0U, (UCHAR *)"Pn100=           ", ResBuf );
	RpxLcdSetStr( 2U, 0U, (UCHAR *)"Pn101=           ", ResBuf );

	/* Gain	*/
	PRMREG_READX(pv->var.PrmLoopHzIdx, &(pv->var.ResData), MCHK_REG_READ, Hmsg);
	RpxLcdSetDecPnt( 1U, 6U, pv->var.ResData, 5U, (USHORT)(FALSE), 1, ResBuf );
	PRMREG_READX(pv->var.PrmPiTimeIdx, &(pv->var.ResData), MCHK_REG_READ, Hmsg);
	RpxLcdSetDecPnt( 2U, 6U, pv->var.ResData, 5U, (USHORT)(FALSE), 2, ResBuf );

	if ( ( pv->var.SpatMode == 2U )
			|| ( pv->var.SpatMode == 3U ) )		/* 位置制御の場合 */
	{
		RpxLcdSetStr( 3U, 0U, (UCHAR *)"Pn141=           ", ResBuf );
		PRMREG_READX(pv->var.PrmMdlGnIdx, &(pv->var.ResData), MCHK_REG_READ, Hmsg);
		RpxLcdSetDecPnt( 3U, 6U, pv->var.ResData, 5U, (USHORT)(FALSE), 1, ResBuf );
	}
	else
	{
		RpxLcdSetStr( 3U, 0U, (UCHAR *)"Pn102=           ", ResBuf );
		PRMREG_READX(pv->var.PrmPosGnIdx, &(pv->var.ResData), MCHK_REG_READ, Hmsg);
		RpxLcdSetDecPnt( 3U, 6U, pv->var.ResData, 5U, (USHORT)(FALSE), 1, ResBuf );
	}
	RpxLcdSetStr( 4U, 0U, (UCHAR *)"                 ", ResBuf );

	PRMREG_READX(pv->var.PrmTFuncSwIdx, &(pv->var.ResData), MCHK_REG_READ, Hmsg);
	if ( (pv->var.ResData & 0x000FU) != 0x0000U )
	{
		RpxLcdSetStr( 4U, 1U, (UCHAR *)"NF1", ResBuf );
	}
	if ( (pv->var.ResData & 0x0F00U) != 0x0000U )
	{
		RpxLcdSetStr( 4U, 5U, (UCHAR *)"NF2", ResBuf );
	}
	/*----------------------------------------------------------------------*/
	/*	Ａ型制振ON表示														*/
	/*----------------------------------------------------------------------*/
	PRMREG_READX(pv->var.PrmAvibSwIdx, &(pv->var.ResData), MCHK_REG_READ, Hmsg);
	if ( (pv->var.ResData & 0x000FU) != 0x0000U )
	{
		RpxLcdSetStr( 4U, 11U, (UCHAR *)"ARES", ResBuf );
	}
	return;
}

/*****************************************************************************
  Description: Fn203 One Parameter Tuning Data Display
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	None
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void	FunOneParamTuningLcdTuneDisplay( FUNMSG *Fmsg )
{
	FNONEPRMTUNV *pv = (FNONEPRMTUNV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
	UCHAR *ResBuf = Fmsg->pTxBuf;
	LONG lWork;
	ONEPRMTUNE	*OnePrmTune;
	BASE_CTRL_HNDL	*pBaseCtrl;

	OnePrmTune = Fmsg->Super->pAxRsc->OnePrmTune;
	pBaseCtrl = Fmsg->Super->pAxRsc->BaseControls;

	if ( ( pv->var.SpatMode == 2U )
			|| ( pv->var.SpatMode == 3U ) )		
	{
		/* チューニングレベル変更	*/
		RpxLcdSetStr( 1U, 0U, (UCHAR *)"FF Level =       ", ResBuf );
		RpxLcdSetStr( 2U, 0U, (UCHAR *)"FB Level =       ", ResBuf );
		RpxLcdSetStr( 3U, 0U, (UCHAR *)"                 ", ResBuf );
		RpxLcdSetStr( 4U, 0U, (UCHAR *)"                 ", ResBuf );

		RpxLcdSetDecPnt( 1U, 11U, (ULONG)pv->var.FfSetting, 5U, (USHORT)(FALSE), 1, ResBuf );
		RpxLcdSetDecPnt( 2U, 11U, (ULONG)pv->var.SpatLevel, 5U, (USHORT)(FALSE), 1, ResBuf );
		if ( pv->var.TunSel == OPTUNE_FLAG_FFSEL )
		{
			RpxLcdSetBlnk( 1U, (USHORT)(14U - pv->var.DPos), 1U, ResBuf );
		}
		else
		{
			RpxLcdSetBlnk( 2U, (USHORT)(14U - pv->var.DPos), 1U, ResBuf );
		}

		/* FF Level 変更失敗 if 条件一定成立 */
		if ( MfcGetParamEnableSts( &(pBaseCtrl->MFControl) ) == FALSE )
		{
			RpxLcdSetBlnk( 1U, 0U, 8U, ResBuf );
		}
	}
	else
	{
		/* チューニングレベル変更	*/
		RpxLcdSetStr( 1U, 0U, (UCHAR *)"                 ", ResBuf );
		RpxLcdSetStr( 2U, 0U, (UCHAR *)"Level =          ", ResBuf );
		RpxLcdSetStr( 3U, 0U, (UCHAR *)"                 ", ResBuf );
		RpxLcdSetStr( 4U, 0U, (UCHAR *)"                 ", ResBuf );
		RpxLcdSetDecPnt( 2U, 7U, (ULONG)(pv->var.SpatLevel/10U), 4U, (USHORT)(FALSE), 0, ResBuf );
		RpxLcdSetBlnk( 2U, (USHORT)(10U - pv->var.DPos), 1U, ResBuf );
	}

	PRMREG_READX(pv->var.PrmTFuncSwIdx, &(pv->var.ResData), MCHK_REG_READ, Hmsg);
	if ( (pv->var.ResData & 0x000FU) != 0x0000U )
	{
		RpxLcdSetStr( 4U, 1U, (UCHAR *)"NF1", ResBuf );
	}
	if ( (pv->var.ResData & 0x0F00U) != 0x0000U )
	{
		RpxLcdSetStr( 4U, 6U, (UCHAR *)"NF2", ResBuf );
	}

	/*----------------------------------------------------------------------*/
	/*	Ａ型制振ON表示														*/
	/*----------------------------------------------------------------------*/
	PRMREG_READX(pv->var.PrmAvibSwIdx, &(pv->var.ResData), MCHK_REG_READ, Hmsg);
	if ( (pv->var.ResData & 0x000FU) != 0x0000U )
	{
		RpxLcdSetStr( 4U, 11U, (UCHAR *)"ARES", ResBuf );
	}

	if ( (OnePrmTune->var.FftReq == TRUE) && (OnePrmTune->var.NotchChk == TRUE) )
	{
		RpxLcdSetStr( 3U, 0U, (UCHAR *)" Auto Setting    ", ResBuf );
		RpxLcdSetBlnk( 3U, 0U, 18U, ResBuf );
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

	/*----------------------------------------------------------------------*/
	/*	Ａ型制振自動設定F表示												*/
	/*----------------------------------------------------------------------*/
//	STSREG_READX(pv->var.OpeSpatAresStIdx, &(pv->var.ResData), MCHK_REG_READ, Hmsg);
//	lWork = ( pv->var.ResData >> 4 ) & 0x0FU;
	switch( OnePrmTune->var.ARes1Stat )		//	switch ( lWork )
	{
		case OPTUNE_ST_ARES_OK:				/* Ａ型制振設定正常完了	*/
			RpxLcdSetBlnk( 4U, 11U, 4U, ResBuf );
			break;

		case OPTUNE_ST_ARES_ERR:			/* Ａ型制振上下限異常	*/
			RpxLcdSetStr( 4U, 11U, (UCHAR *)"AERR", ResBuf );
			RpxLcdSetBlnk( 4U, 11U, 4U, ResBuf );
			break;

		case OPTUNE_ST_ARES_INIT :
			RpxLcdClrLineBlnk( 4U, ResBuf );
			break;

		default:
			break;
	}
	return;
}



/*****************************************************************************
  Description: Fn203 One Parameter Tuning Mode Execution(Panel Operator)
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		FUN_CONTINUE: Execution Continue, FUN_END: Execution End
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
#if  SVFSEL_PANELOP_USE == 1
static LONG LpxPnlOneParamTuningExe(FUNMSG *Fmsg)
{
	FNONEPRMTUNV *pv = (FNONEPRMTUNV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
	OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
	BLINKFLG *BlinkFlag = (BLINKFLG *)(Fmsg->pBlnkKick);
	SHORT	CalcTmp, i;
	ULONG	ulWork;
	LONG rc = FUN_CONTINUE;													/* 0: continue, 1: end */
	ONEPRMTUNE	*OnePrmTune;
	OnePrmTune = Fmsg->Super->pAxRsc->OnePrmTune;

	/* Un01D判定追加 */
	UNREG_READX( pv->var.UnActGainIdx, ( ULONG * )( &(pv->var.UnActGain) ), MCHK_REG_READ, Hmsg );
	if ( pv->var.UnActGain != 1 )
	{
		if ( OpKeyMake.data == TRUE )
		{
			pv->var.State = FNONEPRMTUNST_END;
		}
		else if ( (USHORT)(*(Fmsg->pKeyMk)) != 0U )
		{
			BlinkFlag->no_op = TRUE;
			return rc;
		}
		else
		{
			/* Do Nothing */
		}
	}
	switch( pv->var.State )
	{
/*--------------------------------------------------------------------------------------------------*/
		case FNONEPRMTUNST_INIT: 											/* Initial State 		*/
/*--------------------------------------------------------------------------------------------------*/
			FunOnePrmTuningModeInitSet( Fmsg );

			pv->var.AdatFilType = 2U;
			OPEREG_WRITE( pv->var.OpeAdatFilTypeIdx, pv->var.AdatFilType, Hmsg );

			pv->var.DPos = 0U;			/* 点滅桁数  Initial */

			/* Initial Variable */
			STSREG_READX(pv->var.OpeSpatLevelIdx, &(pv->var.SpatLevel), MCHK_REG_READ, Hmsg);
			pv->var.SpatLevel = ( ( pv->var.SpatLevel / 10U ) * 10U );
			pv->var.DigMax = 2U;

			OPEREG_WRITE( pv->var.OpeModeIdx, FNNORMAL_MODE, Hmsg );
			pv->var.State = FNONEPRMTUNST_MODESEL;
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNONEPRMTUNST_MODESEL: 									/*	 Mode Selection 		*/
/*--------------------------------------------------------------------------------------------------*/
			/* Process Key */
			if( OpKeyMake.data == TRUE )
			{
				pv->var.State = FNONEPRMTUNST_END;
			}
			else if ( OpKeyMake.shup == TRUE )
			{
				pv->var.DPos = 0U;
				OPEREG_WRITE( pv->var.OpeSpatModeIdx, pv->var.SpatMode, Hmsg );
/*--------------------------------------------------------------------------------------------------*/
/* Set Fn203 One Parameter Tuning Mode to Mode Register 											*/
/*--------------------------------------------------------------------------------------------------*/
				OPEREG_WRITE( pv->var.OpeModeIdx, FNONEPRMTUN_MODE, Hmsg );
				pv->var.State = FNONEPRMTUNST_TUNING;
			}
			else
			{
				if ( OpKeyMake.up == TRUE )
				{
					if( pv->var.SpatMode < pv->var.ModeMax )
					{
						pv->var.SpatMode++;
					}
					else
					{
						pv->var.SpatMode = pv->var.ModeMax;
					}
				}
				else if ( OpKeyMake.down == TRUE )
				{
					if( pv->var.SpatMode > pv->var.ModeMin )
					{
						pv->var.SpatMode--;
					}
					else
					{
						pv->var.SpatMode = pv->var.ModeMin;
					}
				}
				else
				{
					/* Do Nothing */
				}
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNONEPRMTUNST_TUNING: 									/* 1パラ調整処理				*/
/*--------------------------------------------------------------------------------------------------*/
			/* Process Key */
			if( OpKeyMake.data == TRUE )
			{
				pv->var.State = FNONEPRMTUNST_END;
			}
			else if ( OpKeyMake.mode == TRUE )
			{
				if ( OPEREG_WRITE( pv->var.OpeSetIdx, ONEPRMTUNCMD_PRMUPD, Hmsg ) == OK )
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
				CalcTmp = 10;
				for(i = 0; (UCHAR)(i) < pv->var.DPos; i++)
				{
					CalcTmp = CalcTmp * 10;
				}
				if ( OpKeyMake.up == TRUE )
				{
					STSREG_READX(pv->var.OpeSpatLevelIdx, &(pv->var.SpatLevel), MCHK_REG_READ, Hmsg);
					ulWork = pv->var.SpatLevel + (ULONG)(CalcTmp);
					pv->var.SpatLevel = (ULONG)TunePrmLimitSigned( (ULONG)(ulWork), &pndef_loophz );
					OPEREG_WRITE( pv->var.OpeSpatLevelIdx, pv->var.SpatLevel, Hmsg );
					OPEREG_WRITE( pv->var.OpeSetIdx, ONEPRMTUNCMD_EXE, Hmsg );
				}
				else if ( OpKeyMake.down == TRUE )
				{
					if( OnePrmTune->var.NotchChk == FALSE )	/* Notch Check Switch */
					{
						STSREG_READX(pv->var.OpeSpatLevelIdx, &(pv->var.SpatLevel), MCHK_REG_READ, Hmsg);
						ulWork = pv->var.SpatLevel - (ULONG)(CalcTmp);
						pv->var.SpatLevel = (ULONG)TunePrmLimitSigned( (ULONG)(ulWork), &pndef_loophz );
						OPEREG_WRITE( pv->var.OpeSpatLevelIdx, pv->var.SpatLevel, Hmsg );
						OPEREG_WRITE( pv->var.OpeSetIdx, ONEPRMTUNCMD_EXE, Hmsg );
					}
				}
				else if ( OpKeyMake.shup == TRUE )		/* Shift Up 	*/
				{
					if( pv->var.DPos < ( pv->var.DigMax - 1U ) )
					{
						pv->var.DPos++;					/* 桁移動		*/
					}
					else
					{
						pv->var.DPos = 0U;
					}
				}
				else
				{
					/* Do Nothing */
				}
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNONEPRMTUNST_END: 												/* 終了処理			*/
/*--------------------------------------------------------------------------------------------------*/
			FunOneParamTuningEnd( Fmsg );
			rc = FUN_END;
			break;

		default:
			break;
	}
	FunPnlOneParamTuningDisplay( Fmsg );
	return rc;
}


/*****************************************************************************
  Description: Fn203 One Parameter Tuning Mode Display(Panel Operator )
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	None
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void FunPnlOneParamTuningDisplay( FUNMSG *Fmsg )
{
	FNONEPRMTUNV *pv = (FNONEPRMTUNV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
	LEDDSP *DspData = (LEDDSP *)&(Fmsg->pTxBuf[PNLRSP_LEDCODE_IDX]);/* set display data buffer */

	/* ALL Clear	*/
	MlibResetByteMemory( DspData, sizeof(LEDDSP) );	 					/* clear Tx buffer		*/

	switch ( pv->var.State )
	{
		case FNONEPRMTUNST_INIT:
			/* Fall Through */
		case FNONEPRMTUNST_JSET:
			/* Fall Through */
		case FNONEPRMTUNST_MODESEL:
			/*led="d   x"*/
			RpxLedSetStr( (UCHAR *)"d", DspData, 4U, 1U );
			RpxLedSetValDec( (USHORT)(pv->var.SpatMode), DspData, 0U, 1U, FALSE, 0U );
			break;

		case FNONEPRMTUNST_TUNING:
			/* Fall Through */
		case FNONEPRMTUNST_END:
			RpxLedSetStr( (UCHAR *)"L", DspData, 4U, 1U );
			STSREG_READX(pv->var.OpeSpatLevelIdx, &(pv->var.SpatLevel), MCHK_REG_READ, Hmsg);
			RpxLedSetValDec( (USHORT)(pv->var.SpatLevel/10U), DspData, 3U, 4U, FALSE, 0U );
			RpxLedSetBlnk( DspData, (UCHAR)(1U << pv->var.DPos) );
			break;

		default:
			break;
	}
	return ;
}
#endif	/* #if  SVFSEL_PANELOP_USE == 1 */




/***************************************** end of file **********************************************/
