/*****************************************************************************
Description		: Fn206 Easy FFT
-----------------------------------------------------------------------------
Author		: SHCORE
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

Functions	  :
	-- APIs --
	FunEasyFftMain();				:Fn206 Easy FFT Mode Display
	FunEasyFftBegin();				:Fn206 Easy FFT Mode Begin
	FunEasyFftEnd();				:Fn206 Easy FFT Mode End

	-- Locals --
	LpxLcdDynamicAutoTuningExe();	:LCD Operator Fn206 Execution
	FunEasyFftDisplay();			:Fn206 Easy FFT Display
	LpxPnlDynamicAutoTuningExe();	:Panel Operator Fn206 Execution(Panel Operator Function)
	FunPnlEasyFftDisplay();			:Fn206 Easy FFT Display(Panel Operator)

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
K.Liu		2013.06.20		created
K.Liu		2013.08.30		Add Panel Operator Function
K.Liu		2013.12.09		Modify
*****************************************************************************/
#include "FunEasyFFT.h"
#include "Parameter.h"

/*--------------------------------------------------------------------------------------------------*/
/* Operation Register 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define	OPEMODE_REG1		(0x0000)				/* 1st Operation Mode Register 					*/
#define	OPESET_REG1			(0x0001)				/* 1st Operation Mode Set Register				*/
#define	OPEEZFFTST_REG		(0x0061)				/* 0x2061 : EasyFFT完了確認 Register 			*/
#define	OPEEZFFTTRQ_REG		(0x0062)				/* 0x2062 : EasyFFTトルク振幅 Register 			*/
#define	OPEEZFFTDETF_REG	(0x0410)				/* 0x2410 : EasyFFT検出周波数 Register 			*/
#define	OPEEZFFTFIL1_REG	(0x0411)				/* 0x2411 : EasyFFTフィルタ周波数１ Register	*/
#define	OPEEZFFTFIL2_REG	(0x0412)				/* 0x2412 : EasyFFTフィルタ周波数2 Register 	*/
/*--------------------------------------------------------------------------------------------------*/
#define	PRMTFUNCSW_REG		(0x0408)				/* トルク関係機能スイッチ Register				*/
#define	PRMEZFFTTRQ_REG		(0x0456)				/* Pn456 : 掃引トルク指令振幅	Register		*/
/*--------------------------------------------------------------------------------------------------*/
/* Operation Mode	 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define	FNEASYFFT_MODE		(0x1206U) 				/* Fn203 One Parameter Tuning Mode 				*/
#define	FNNORMAL_MODE		(0x0000U)				/* Normal Mode									*/
/*--------------------------------------------------------------------------------------------------*/
/* Operation Command 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define	EASYFFTCMD_STOP	 		(0x0000)
#define	EASYFFTCMD_FORWARD 		(0x0002)
#define	EASYFFTCMD_REVERSE 		(0x0003)
#define	EASYFFTCMD_SVONOFF 		(0x0004)
#define	EASYFFTCMD_PRMUPD 		(0x0007)		/* Parameter Update */
/*--------------------------------------------------------------------------------------------------*/
#define	FUN_CONTINUE			0				/* 	Continue 		*/
#define	FUN_END					1				/* 	End				*/
/*--------------------------------------------------------------------------------------------------*/
#define	EZFFT_DPOSMAX			2U				/* 掃引トルク調整最大桁	*/
/*--------------------------------------------------------------------------------------------------*/
/* Easy FFT Execution State */
enum EASYFFT_EXE_STATE
{
	EASYFFT_STS_END,			/* 完了					*/
	EASYFFT_STS_RUN,			/* 未完了				*/
	EASYFFT_STS_DETNG,			/* ピーク値検出不能		*/
	EASYFFT_STS_ERR				/* 異常終了				*/
};

/****************************************************************************************************/
/*																									*/
/*		Function List																				*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Public Function																				*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunEasyFftBegin(FUNMSG *Fmsg); 		/* Fn206 Easy FFT Mode Begin	*/
LONG FunEasyFftMain(FUNMSG *Fmsg);			/* Fn206 Easy FFT Mode Display	*/
void FunEasyFftEnd(FUNMSG *Fmsg);			/* Fn206 Easy FFT Mode End		*/

/*--------------------------------------------------------------------------------------------------*/
/*		Private Function																			*/
/*--------------------------------------------------------------------------------------------------*/
static LONG LpxLcdEasyFftExe(FUNMSG *Fmsg);						/* LCD Operator Fn206 Execution 	*/
static void	FunEasyFftDisplay( FUNMSG *Fmsg , USHORT EzFftState );	/* Fn206 Display ( LCD Display )*/

#if  SVFSEL_PANELOP_USE != 1
#define LpxPnlEasyFftExe( x ) (1)
#define FunPnlEasyFftDisplay( x, y )	()
#else
LONG LpxPnlEasyFftExe(FUNMSG *hMsg);							/* Panel Operator Fn206 Execution 	*/
static void	FunPnlEasyFftDisplay( FUNMSG *Fmsg , USHORT EzFftState );	/* Fn206 Display(Panel Display)	*/
#endif

#define OPEREG_WRITE( idx, data, hmsg )   (COPEREG->Write( COPEREG, (idx), (data), hmsg ))
#define STSREG_READX( idx, pResData, CheckType, hmsg )   (COPEREG->Read( COPEREG, (idx), (pResData), \
																		(CheckType), hmsg ))  /* <S073> */
#define PRMREG_READX( idx, pResData, CheckType, hmsg )	(CPARAMX->Read( (COBJECT *)(CPARAMX),(idx),\
																	(pResData), (CheckType), hmsg ))  /* <S073> */

/*****************************************************************************
  Description: Fn206 Easy FFT Mode Display
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		0: Continue, 1: End
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
LONG FunEasyFftMain(FUNMSG *Fmsg)
{
	MSGIF *hMsg = Fmsg->Super;
	LONG rc = 0;
/*--------------------------------------------------------------------------------------------------*/
	if( KPI_CHK_LopHmsg( hMsg ) == (ULONG)(TRUE) )					/* LCD Operator		*/
	{
		rc = LpxLcdEasyFftExe( Fmsg );
	}
/*--------------------------------------------------------------------------------------------------*/
	else if ( KPI_CHK_PopHmsg( hMsg ) == TRUE )						/* Panel Operator	*/
	{
		rc = LpxPnlEasyFftExe( Fmsg );
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
  Description: Fn206 Easy FFT Mode Begin
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		OK: Success, Others: Failure
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
LONG FunEasyFftBegin(FUNMSG *Fmsg)
{
	LONG	rc = OK;
	BOOL ResFlag = TRUE;
	FNEASYFFTV  *pv = (FNEASYFFTV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	pv->var.State = FNEASYFFTST_INIT;
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
		rc = COPEREG->Search( COPEREG,					/* Search Register (Operation Command)		*/
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
		rc = COPEREG->Search( COPEREG,				/* Search Register ( 0x2061 : EasyFFT完了確認)	*/
				OPEEZFFTST_REG,
				0,
				&(pv->var.OpeEzFftStateIdx),
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
		rc = COPEREG->Search( COPEREG,			/* Search Register ( 0x2062 : EasyFFTトルク振幅)	*/
				OPEEZFFTTRQ_REG,
				0,
				&(pv->var.OpeEzFftTrqIdx),
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
		rc = COPEREG->Search( COPEREG,		/* Search Register (0x2410 : EasyFFT検出周波数 Register)*/
				OPEEZFFTDETF_REG,
				0,
				&(pv->var.OpeEzFftDetfIdx),
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
		rc = COPEREG->Search( COPEREG,		/* Search Register (0x2411 : EasyFFTフィルタ周波数１)	*/
				OPEEZFFTFIL1_REG,
				0,
				&(pv->var.OpeEzFftFil1Idx),
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
		rc = COPEREG->Search( COPEREG,		/* Search Register (0x2412 : EasyFFTフィルタ周波数2)	*/
				OPEEZFFTFIL2_REG,
				0,
				&(pv->var.OpeEzFftFil2Idx),
				MCHK_REG_READ,
				Hmsg);
		if( rc != OK )
		{
			ResFlag = FALSE;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
	/* Parameter Table Search */
/*--------------------------------------------------------------------------------------------------*/
	if( ResFlag == TRUE )
	{
		rc = CPARAMX->Search( (COBJECT *)CPARAMX,		/* Search Register ( トルク関係機能スイッチ)*/
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
/* 						Set Fn206 Easy FFT Mode to Mode Register 									*/
/*--------------------------------------------------------------------------------------------------*/
	if ( ResFlag == TRUE )
	{
		rc = OPEREG_WRITE( pv->var.OpeModeIdx, FNEASYFFT_MODE, Hmsg );
		KpiRstLongTimer( &(pv->var.DispTime) );						/* Panel Operator 				*/
	}
/*--------------------------------------------------------------------------------------------------*/
//	KpiRstLongTimer( &(pv->var.DispTime) );						/* Panel Operator 					*/
/*--------------------------------------------------------------------------------------------------*/
	return rc;
}


/*****************************************************************************
  Description: Fn206 Easy FFT Mode End
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	None
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
void FunEasyFftEnd(FUNMSG *Fmsg)
{
	FNEASYFFTV  *pv = (FNEASYFFTV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------*/
	OPEREG_WRITE( pv->var.OpeSetIdx, EASYFFTCMD_STOP, Hmsg );
	OPEREG_WRITE( pv->var.OpeModeIdx, FNNORMAL_MODE, Hmsg );
}


/*****************************************************************************
  Description: Fn206 Easy FFT Mode Execution( LCD Operator )
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		FUN_CONTINUE: Execution Continue, FUN_END: Execution End
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static LONG LpxLcdEasyFftExe(FUNMSG *Fmsg)
{
	FNEASYFFTV *pv = (FNEASYFFTV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
	OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
	BLINKFLG *BlinkFlag = (BLINKFLG *)(Fmsg->pBlnkKick);
	SHORT	CalcTmp, i;
	ULONG	Data;
	AXIS_HANDLE *pAxRsc = Hmsg->pAxRsc;										/* TODO: 後で消す		*/
	LONG rc = FUN_CONTINUE;													/* 0: continue, 1: end */

/*--------------------------------------------------------------------------------------------------*/
/*							Fn206 Easy FFT Sequence													*/
/*--------------------------------------------------------------------------------------------------*/
	switch( pv->var.State )
	{
/*--------------------------------------------------------------------------------------------------*/
		case FNEASYFFTST_INIT: 												/* Initial State 		*/
/*--------------------------------------------------------------------------------------------------*/
			STSREG_READX(pv->var.OpeEzFftTrqIdx, &(pv->var.ResData), MCHK_REG_READ, Hmsg);
			pv->var.EzFftTrq = (USHORT)pv->var.ResData;

			pv->var.DPos = 0U;

			pv->var.State = FNEASYFFTST_READY;
			break;

/*--------------------------------------------------------------------------------------------------*/
		case FNEASYFFTST_READY: 										/* Prepare For Running		*/
/*--------------------------------------------------------------------------------------------------*/
			CalcTmp = 1;
			for(i = 0; (UCHAR)(i) < pv->var.DPos; i++)
			{
				CalcTmp = CalcTmp * 10;
			}
			/* Process Key */
			if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = FNEASYFFTST_END;
			}
			else if ( OpKeyMake.svon == TRUE )		/* Servo On */
			{
				if ( OPEREG_WRITE( pv->var.OpeSetIdx, EASYFFTCMD_SVONOFF, Hmsg ) != OK )
				{
					BlinkFlag->no_op = TRUE;
				}
			}
			else
			{
				if ( OpKeyMake.up == TRUE )
				{
					Data = pv->var.EzFftTrq + (USHORT)(CalcTmp);
					pv->var.EzFftTrq = TunePrmLimitUnsigned( Data, &pndef_ezffttrq );
					OPEREG_WRITE( pv->var.OpeEzFftTrqIdx, pv->var.EzFftTrq, Hmsg );
				}
				else if ( OpKeyMake.down == TRUE )
				{
					Data = pv->var.EzFftTrq - (ULONG)(CalcTmp);
					pv->var.EzFftTrq = TunePrmLimitUnsigned( Data, &pndef_ezffttrq );
					OPEREG_WRITE( pv->var.OpeEzFftTrqIdx, pv->var.EzFftTrq, Hmsg );

					if ( ( Data  < (ULONG)(CalcTmp) ) && ( CalcTmp != 1 ) )
					{
						pv->var.DPos--;
					}
				}
				else if ( OpKeyMake.shup == TRUE )
				{
					if( pv->var.DPos < ( EZFFT_DPOSMAX - 1U ) )
					{
						pv->var.DPos++;			/* 桁移動	*/
						if ( pv->var.EzFftTrq < (USHORT)( CalcTmp * 10 ) )
						{
							pv->var.DPos--;
						}
					}
					else
					{
						pv->var.DPos = 0U;
					}
				}
				else if ( OpKeyMake.shdown == TRUE )
				{
					if( ( pv->var.DPos > 0U ) && ( pv->var.DPos < EZFFT_DPOSMAX ) )
					{
						pv->var.DPos--;			/* 桁移動	*/
					}
					else
					{
						pv->var.DPos = EZFFT_DPOSMAX - 1U;
						if ( pv->var.EzFftTrq < (USHORT)( CalcTmp * 10 ) )
						{
							pv->var.DPos--;
						}
					}
				}
				else
				{
					/* Do Nothing */
				}
			}

			if ( CHK_SVON() == TRUE )
			{
				pv->var.State = FNEASYFFTST_DRIVE;
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNEASYFFTST_DRIVE: 											/* 運転動作中			*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = FNEASYFFTST_END;
			}
			else if ( OpKeyMake.svon == TRUE )		/* Servo Off */
			{
				OPEREG_WRITE( pv->var.OpeSetIdx, EASYFFTCMD_SVONOFF, Hmsg );
			}
			else
			{
				if ( OpKeyMake.up == TRUE )
				{
					if ( OPEREG_WRITE( pv->var.OpeSetIdx, EASYFFTCMD_FORWARD, Hmsg ) == OK)
					{
						pv->var.State = FNEASYFFTST_WAIT;
					}
					else
					{
						BlinkFlag->error = TRUE;
					}
				}
				else if ( OpKeyMake.down == TRUE )
				{
					if ( OPEREG_WRITE( pv->var.OpeSetIdx, EASYFFTCMD_REVERSE, Hmsg ) == OK )
					{
						pv->var.State = FNEASYFFTST_WAIT;
					}
					else
					{
						BlinkFlag->error = TRUE;
					}
				}
				else
				{
					/* Do Nothing */
				}
			}

			if ( CHK_SVON() == FALSE )
			{
				pv->var.State = FNEASYFFTST_READY;
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNEASYFFTST_WAIT: 								/* Waiting For Completed		*/
/*--------------------------------------------------------------------------------------------------*/
			STSREG_READX(pv->var.OpeEzFftStateIdx, &(pv->var.ResData), MCHK_REG_READ, Hmsg);
			pv->var.EzFftStateVal = (USHORT)(pv->var.ResData);
			if ( pv->var.EzFftStateVal != (ULONG)EASYFFT_STS_RUN )
			{
				pv->var.State = FNEASYFFTST_RESULT;
			}

			if ( OpKeyMake.mode == TRUE )
			{
				pv->var.State = FNEASYFFTST_END;
			}
			else if ( OpKeyMake.svon == TRUE )			/* Servo Off */
			{
				OPEREG_WRITE( pv->var.OpeSetIdx, EASYFFTCMD_SVONOFF, Hmsg );
			}
			else
			{
				/* Do Nothing */
			}

			if ( CHK_SVON() == FALSE )
			{
				pv->var.State = FNEASYFFTST_READY;
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNEASYFFTST_RESULT: 								/* 運転完了(フィルタ設定待ち)		*/
/*--------------------------------------------------------------------------------------------------*/
			if ( OpKeyMake.mode == TRUE )
			{
				pv->var.State = FNEASYFFTST_END;
			}
			else if ( OpKeyMake.svon == TRUE )			/* Servo Off */
			{
				OPEREG_WRITE( pv->var.OpeSetIdx, EASYFFTCMD_SVONOFF, Hmsg );
			}
			else if ( OpKeyMake.data == TRUE )			/* Servo Off */
			{
				if ( OPEREG_WRITE( pv->var.OpeSetIdx, EASYFFTCMD_PRMUPD, Hmsg ) == OK )
				{
					BlinkFlag->done = TRUE;
				}
				else
				{
					BlinkFlag->error = TRUE;
				}

				pv->var.State = FNEASYFFTST_READY;
			}
			else if ( OpKeyMake.up == TRUE )
			{
				if ( OPEREG_WRITE( pv->var.OpeSetIdx, EASYFFTCMD_FORWARD, Hmsg ) != OK )
				{
					BlinkFlag->error = TRUE;
				}
				else
				{
					pv->var.State = FNEASYFFTST_WAIT;
				}
			}
			else if ( OpKeyMake.down == TRUE )
			{
				if ( OPEREG_WRITE( pv->var.OpeSetIdx, EASYFFTCMD_REVERSE, Hmsg ) != OK )
				{
					BlinkFlag->error = TRUE;
				}
				else
				{
					pv->var.State = FNEASYFFTST_WAIT;
				}
			}
			else if ( OpKeyMake.shup == TRUE )
			{
				pv->var.State = FNEASYFFTST_READY;
			}
			else
			{
				/* Do Nothing */
			}

			if ( CHK_SVON() == FALSE )
			{
				pv->var.State = FNEASYFFTST_READY;
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNEASYFFTST_END: 													/* 終了処理 		*/
/*--------------------------------------------------------------------------------------------------*/
			FunEasyFftEnd( Fmsg );
			rc = FUN_END;
			break;

		default:
			break;
	}

	/* Fn206 Easy FFT Display */
	FunEasyFftDisplay( Fmsg , (USHORT)(pv->var.EzFftStateVal) );
	return rc;
}

/*****************************************************************************
  Description: Fn206 Easy FFT Display　( LCD Display )
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
	EzFftState;		Easy FFT Complete State
  Return:
	None
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void	FunEasyFftDisplay( FUNMSG *Fmsg , USHORT EzFftState )
{
	FNEASYFFTV *pv = (FNEASYFFTV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
	UCHAR *ResBuf = Fmsg->pTxBuf;
	AXIS_HANDLE *pAxRsc = Hmsg->pAxRsc;										/* TODO: 後で消す		*/
	USHORT	NotchSw;

	RpxLcdClrTxBuf( ResBuf ); 					/* clear Tx buffer */

	/* TITLE	*/
//	RpxLcdSetStr( 0U, 7U, (UCHAR *)"-Easy FFT-", ResBuf );		/* <S154> */
	RpxLcdSetStr( 0U, 9U, (UCHAR *)"Easy FFT", ResBuf );		/* <S154> */
	RpxLcdSetStr( 1U, 0U, (UCHAR *)"                 ", ResBuf );
	RpxLcdSetStr( 2U, 0U, (UCHAR *)"Input =     %    ", ResBuf );
	RpxLcdSetStr( 3U, 0U, (UCHAR *)"                 ", ResBuf );
	RpxLcdSetStr( 4U, 0U, (UCHAR *)"                 ", ResBuf );
	RpxLcdClrLineBlnk( 1U, ResBuf );
	RpxLcdClrLineBlnk( 2U, ResBuf );

	RpxLcdSetDecPnt( 2U, 8U, (ULONG)pv->var.EzFftTrq, 3U, (USHORT)(TRUE), 0, ResBuf );

	if ( CHK_SVON() == FALSE )
	{
		RpxLcdSetBlnk( 2U, (USHORT)(10U - pv->var.DPos), 1U, ResBuf );
	}

	PRMREG_READX(pv->var.PrmTFuncSwIdx, &(pv->var.ResData), MCHK_REG_READ, Hmsg);
	NotchSw = ( USHORT )( pv->var.ResData );
	if ( (NotchSw & 0x000FU) != 0x0000U )
	{
		RpxLcdSetStr( 1U, 12U, (UCHAR *)"*", ResBuf );
	}

	if ( (NotchSw & 0x0F00U) != 0x0000U )
	{
		RpxLcdSetStr( 1U, 15U, (UCHAR *)"*", ResBuf );
	}

	switch ( pv->var.State )
	{
		case FNEASYFFTST_INIT :
			/* Fall Through */
		case FNEASYFFTST_READY :
			RpxLcdSetStr( 1U, 0U, (UCHAR *)"Setting", ResBuf );
			break;

		case FNEASYFFTST_DRIVE :
			RpxLcdSetStr( 1U, 0U, (UCHAR *)"Ready", ResBuf );
			break;

		case FNEASYFFTST_WAIT :
			RpxLcdSetStr( 1U, 0U, (UCHAR *)"Measure", ResBuf );
			break;

		case FNEASYFFTST_RESULT :
			switch ( (LONG)EzFftState )
			{
				case EASYFFT_STS_END :
					RpxLcdSetStr( 1U, 0U, (UCHAR *)"Result", ResBuf );
					RpxLcdSetStr( 3U, 0U, (UCHAR *)"Res =        Hz", ResBuf );

					STSREG_READX(pv->var.OpeEzFftDetfIdx, &(pv->var.ResData), MCHK_REG_READ, Hmsg);
					RpxLcdSetDecPnt( 3U, 8U, pv->var.ResData, 4U, (USHORT)(FALSE), 0, ResBuf );

					switch ( NotchSw & 0x0F0FU )
					{
						case 0x0000 :
						case 0x0100 :
							RpxLcdSetStr( 4U, 0U, (UCHAR *)"Filter1      Hz", ResBuf );
							STSREG_READX(pv->var.OpeEzFftFil1Idx, &(pv->var.ResData), MCHK_REG_READ, Hmsg);
							RpxLcdSetDecPnt( 4U, 8U, pv->var.ResData, 4U, (USHORT)(FALSE), 0, ResBuf );
							break;

						case 0x0001 :
							RpxLcdSetStr( 4U, 0U, (UCHAR *)"Filter2      Hz", ResBuf );
							STSREG_READX(pv->var.OpeEzFftFil2Idx, &(pv->var.ResData), MCHK_REG_READ, Hmsg);
							RpxLcdSetDecPnt( 4U, 8U, pv->var.ResData, 4U, (USHORT)(FALSE), 0, ResBuf );
							break;

						case 0x0101 :
						default :
							RpxLcdSetStr( 4U, 0U, (UCHAR *)"                 ", ResBuf );
							break;
					}
					break;

//				case EASYFFT_STS_RUN :
//					RpxLcdSetStr( 1U, 0U, (UCHAR *)"Measure", ResBuf );
//					break;

				case EASYFFT_STS_DETNG :
					/* Fall Through */
				case EASYFFT_STS_ERR :
					RpxLcdSetStr( 1U, 0U, (UCHAR *)"No Measure", ResBuf );
					RpxLcdSetStr( 3U, 0U, (UCHAR *)"Res =   ---- Hz", ResBuf );
					break;

				default :
					break;
			}
			break;

		case FNEASYFFTST_END :
		default:
			break;
	}
		return;
}



/*****************************************************************************
  Description: Fn206 Easy FFT Mode Execution( Panel Operator )
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		FUN_CONTINUE: Execution Continue, FUN_END: Execution End
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
#if  SVFSEL_PANELOP_USE == 1
static LONG LpxPnlEasyFftExe(FUNMSG *Fmsg)
{
	FNEASYFFTV *pv = (FNEASYFFTV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
	OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
	BLINKFLG *BlinkFlag = (BLINKFLG *)&(Fmsg->pBlnkKick);
	AXIS_HANDLE *pAxRsc = Hmsg->pAxRsc;										/* TODO: 後で消す		*/
	SHORT	CalcTmp, i;
	ULONG	Data;
	LONG rc = FUN_CONTINUE;													/* 0: continue, 1: end */

/*--------------------------------------------------------------------------------------------------*/
/*							Fn206 Easy FFT Sequence													*/
/*--------------------------------------------------------------------------------------------------*/
	switch( pv->var.State )
	{
/*--------------------------------------------------------------------------------------------------*/
		case FNEASYFFTST_INIT: 												/* Initial State 		*/
/*--------------------------------------------------------------------------------------------------*/
			STSREG_READX(pv->var.OpeEzFftTrqIdx, &(pv->var.ResData), MCHK_REG_READ, Hmsg);
			pv->var.EzFftTrq = (USHORT)pv->var.ResData;

			pv->var.DPos = 0U;

			pv->var.State = FNEASYFFTST_TRQSET;
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNEASYFFTST_TRQSET: 										/* 掃引トルク設定			*/
/*--------------------------------------------------------------------------------------------------*/
			CalcTmp = 1;
			for(i = 0; (UCHAR)(i) < pv->var.DPos; i++)
			{
				CalcTmp = CalcTmp * 10;
			}
			/* Process Key */
			if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = FNEASYFFTST_END;
			}
			else if ( OpKeyMake.data == TRUE )		/* Servo On */
			{
				pv->var.State = FNEASYFFTST_READY;
			}
			else
			{
				if ( OpKeyMake.up == TRUE )
				{
					Data = pv->var.EzFftTrq + (USHORT)(CalcTmp);
					pv->var.EzFftTrq = TunePrmLimitUnsigned( Data, &pndef_ezffttrq );
					OPEREG_WRITE( pv->var.OpeEzFftTrqIdx, pv->var.EzFftTrq, Hmsg );
				}
				else if ( OpKeyMake.down == TRUE )
				{
					Data = pv->var.EzFftTrq - (ULONG)(CalcTmp);
					pv->var.EzFftTrq = TunePrmLimitUnsigned( Data, &pndef_ezffttrq );
					OPEREG_WRITE( pv->var.OpeEzFftTrqIdx, pv->var.EzFftTrq, Hmsg );

					if ( ( Data  < (ULONG)(CalcTmp) ) && ( CalcTmp != 1 ) )
					{
						pv->var.DPos--;
					}
				}
				else if ( OpKeyMake.shup == TRUE )
				{
					if( pv->var.DPos < ( EZFFT_DPOSMAX - 1U ) )
					{
						pv->var.DPos++;			/* 桁移動	*/
						if ( pv->var.EzFftTrq < (USHORT)( CalcTmp * 10 ) )
						{
							pv->var.DPos--;
						}
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
		case FNEASYFFTST_READY: 										/* Prepare For Running		*/
/*--------------------------------------------------------------------------------------------------*/
			/* Process Key */
			if( OpKeyMake.data == TRUE )
			{
				pv->var.State = FNEASYFFTST_END;
			}
			else if ( OpKeyMake.mode == TRUE )		/* Servo On */
			{
				if ( OPEREG_WRITE( pv->var.OpeSetIdx, EASYFFTCMD_SVONOFF, Hmsg ) != OK )
				{
					BlinkFlag->no_op = TRUE;
				}
			}
			else
			{
				/* Do Nothing */
			}

			if ( CHK_SVON() == TRUE )
			{
				pv->var.State = FNEASYFFTST_DRIVE;
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNEASYFFTST_DRIVE: 											/* 運転動作中			*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.data == TRUE )
			{
				pv->var.State = FNEASYFFTST_END;
			}
			else if ( OpKeyMake.mode == TRUE )		/* Servo Off */
			{
				OPEREG_WRITE( pv->var.OpeSetIdx, EASYFFTCMD_SVONOFF, Hmsg );
			}
			else
			{
				if ( OpKeyMake.up == TRUE )
				{
					if ( OPEREG_WRITE( pv->var.OpeSetIdx, EASYFFTCMD_FORWARD, Hmsg ) == OK)
					{

						pv->var.State = FNEASYFFTST_WAIT;
					}
					else
					{
						BlinkFlag->error = TRUE;
					}
				}
				else if ( OpKeyMake.down == TRUE )
				{
					if ( OPEREG_WRITE( pv->var.OpeSetIdx, EASYFFTCMD_REVERSE, Hmsg ) == OK )
					{
						pv->var.State = FNEASYFFTST_WAIT;
					}
					else
					{
						BlinkFlag->error = TRUE;
					}
				}
				else
				{
					/* Do Nothing */
				}
			}

			if ( CHK_SVON() == FALSE )
			{
				pv->var.State = FNEASYFFTST_READY;
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNEASYFFTST_WAIT: 								/* Waiting For Completed		*/
/*--------------------------------------------------------------------------------------------------*/
			STSREG_READX(pv->var.OpeEzFftStateIdx, &(pv->var.ResData), MCHK_REG_READ, Hmsg);
			pv->var.EzFftStateVal = (USHORT)(pv->var.ResData);
			if ( pv->var.EzFftStateVal != (ULONG)EASYFFT_STS_RUN )
			{
				pv->var.State = FNEASYFFTST_RESULT;
			}

			if ( OpKeyMake.data == TRUE )
			{
				pv->var.State = FNEASYFFTST_END;
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNEASYFFTST_RESULT: 								/* 運転完了(フィルタ設定待ち)		*/
/*--------------------------------------------------------------------------------------------------*/
			if ( OpKeyMake.data == TRUE )
			{
				pv->var.State = FNEASYFFTST_END;
			}
			else if ( OpKeyMake.mode == TRUE )			/* Servo Off */
			{
				if ( OPEREG_WRITE( pv->var.OpeSetIdx, EASYFFTCMD_PRMUPD, Hmsg ) == OK )
				{
					BlinkFlag->done = TRUE;
				}
				else
				{
					BlinkFlag->error = TRUE;
				}

				pv->var.State = FNEASYFFTST_READY;
			}
			else if ( OpKeyMake.up == TRUE )
			{
				if ( OPEREG_WRITE( pv->var.OpeSetIdx, EASYFFTCMD_FORWARD, Hmsg ) != OK )
				{
					BlinkFlag->error = TRUE;
				}
				else
				{
					pv->var.State = FNEASYFFTST_WAIT;
				}
			}
			else if ( OpKeyMake.down == TRUE )
			{
				if ( OPEREG_WRITE( pv->var.OpeSetIdx, EASYFFTCMD_REVERSE, Hmsg ) != OK )
				{
					BlinkFlag->error = TRUE;
				}
				else
				{
					pv->var.State = FNEASYFFTST_WAIT;
				}
			}
			else if ( OpKeyMake.shup == TRUE )
			{
				pv->var.State = FNEASYFFTST_READY;
			}
			else
			{
				/* Do Nothing */
			}

			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNEASYFFTST_END: 													/* 終了処理 		*/
/*--------------------------------------------------------------------------------------------------*/
			FunEasyFftEnd( Fmsg );
			rc = FUN_END;
			break;

		default:
			break;
	}


	/* Fn206 Easy FFT Display ( Panel Operator ) */
	FunPnlEasyFftDisplay( Fmsg , (USHORT)(pv->var.EzFftStateVal) );
	return rc;
}

/*****************************************************************************
  Description: Fn206 Easy FFT Display( Panel Operator )
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
	EzFftState;		Easy FFT Complete State
  Return:
	None
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void	FunPnlEasyFftDisplay( FUNMSG *Fmsg , USHORT EzFftState )
{
	FNEASYFFTV *pv = (FNEASYFFTV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
	LEDDSP *DspData = (LEDDSP *)&(Fmsg->pTxBuf[PNLRSP_LEDCODE_IDX]);/* set display data buffer */
	AXIS_HANDLE *pAxRsc = Hmsg->pAxRsc;								/* TODO: 後で消す */

	/* ALL Clear	*/
	MlibResetByteMemory( DspData, sizeof(LEDDSP) );	 					/* clear Tx buffer		*/

	RpxLedSetBlnk( DspData, 0x00U );
	switch ( pv->var.State )
	{
		case FNEASYFFTST_INIT :
			/* Fall Through */
		case FNEASYFFTST_TRQSET :
			RpxLedSetStr( (UCHAR *)"In", DspData, 4U, 2U );
			RpxLedSetValDec( (USHORT)pv->var.EzFftTrq, DspData, 2U, 3U, TRUE, 0U );
			RpxLedSetBlnk( DspData, (UCHAR)(1 << pv->var.DPos) );
			break;

		case FNEASYFFTST_READY :
			RpxLedSetStr( (UCHAR *)"F.    ", DspData, 4U, 6U );
			break;

		case FNEASYFFTST_DRIVE :
			RpxLedSetStr( (UCHAR *)"..run", DspData, 4U, 5U );
			break;

		case FNEASYFFTST_WAIT :
			RpxLedSetStr( (UCHAR *)"E_FFt", DspData, 4U, 5U );
			RpxLedSetBlnk( DspData, 0x1FU );
			break;

		case FNEASYFFTST_RESULT :
			switch ( (LONG)EzFftState )
			{
				case EASYFFT_STS_END :
					RpxLedSetStr( (UCHAR *)"F.", DspData, 4U, 2U );
					STSREG_READX(pv->var.OpeEzFftDetfIdx, &(pv->var.ResData), MCHK_REG_READ, Hmsg);
					RpxLedSetValDec( (USHORT)pv->var.ResData, DspData, 3U, 4U, FALSE, 0U );
					break;
//
//				case EASYFFT_STS_RUN :
//					RpxLedSetStr( (UCHAR *)"E_FFt", DspData, 4U, 5U );
//					RpxLedSetBlnk( DspData, 0x1FU );
//					break;

				case EASYFFT_STS_DETNG :
					/* Fall Through */
				case EASYFFT_STS_ERR :
					RpxLedSetStr( (UCHAR *)"F.----", DspData, 4U, 6U );
					break;

				default :
					break;
			}
			break;

		case FNEASYFFTST_END :
		default:
			break;
	}

	/*------------------------------------------------------------------------------------------*/
	/*
	 * Hard-wired Baseblock Display <V068>
	 *
	 *     set "Hbb" to the panel once every 100*4 msec. (SGDV-AP specification)
	 *
	 */
	/*------------------------------------------------------------------------------------------*/
	if( CHK_HBB() /* CoutV.f.Sv.HwbbState */ )
	{
		if( !( (KpiGetLongTimer( &(pv->var.DispTime) )/100U) & 0x04U) )
		{
			RpxLedSetStr( (UCHAR *)". Hbb", DspData, 4U, 5U );	/* set HBB */
		}
	}

	return;
}
#endif



/***************************************** end of file **********************************************/
