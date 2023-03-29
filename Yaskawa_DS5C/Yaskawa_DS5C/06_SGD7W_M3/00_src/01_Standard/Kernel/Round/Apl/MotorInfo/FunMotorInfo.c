/*****************************************************************************
Description		: Fn011 : Motot Type & Y Specification Display
-----------------------------------------------------------------------------
Author		: SHCORE
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

Functions	  :
	-- APIs --
	FunMotorTypeMain();					:Fn011 : Motot Type & Y Specification Display mode Main
	FunMotorTypeBegin();				:Fn011 : Motot Type & Y Specification Display mode Begin
	FunMotorTypeEnd();					:Fn011 : Motot Type & Y Specification Display mode End

	-- Locals --
	FunExpDivCal();						:指数値算出 10^x / 10^y
	FunDispDecToHex();					:表示用データ変換
	FunMotorTypeDisplay();				:Fn011 Motot Type & Y Specification Display Data
	LpxLcdFunMotorTypeExe();			:LCD Operator Fn011 Execution
	LpxPnlFunMotorTypeExe();			:PANEL Operator Fn011 Execution

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
SL.Xue		2013.10.12		created
SL.Xue		2013.12.12		コメントアウトした変数が見つかって追加した

*****************************************************************************/

#include	"FunMotorInfo.h"


/*--------------------------------------------------------------------------------------------------*/
/* Operation Register 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define OPEMODE_REG1			(0x0000)		/* 1st Operation Mode Register */
#define OPESET_REG1				(0x0001)		/* 1st Operation Mode Set Register */

/*--------------------------------------------------------------------------------------------------*/
/* Operation Mode 																					*/
/*--------------------------------------------------------------------------------------------------*/
#define FNNORMAL_MODE			(0x0000U)		/* Normal Mode	*/
#define FNMOTORINFO_MODE		(0xFFFFU)		/* Motot Type & Y Specification Display Mode */

/*--------------------------------------------------------------------------------------------------*/
/* Prm Register 																					*/
/*--------------------------------------------------------------------------------------------------*/
#define PRMTYPME_REG			(0xF00)			/* 0xF00: 下位Byte：モータ形式(8bit), 上位Byte：(入力電圧(4bit), エンコーダタイプ(4bit)) */
#define PRMEXPMOTWSPD_REG		(0xF19)			/* 0xF19: 下位Byte：定格出力指数、上位Byte：回転数指数 */
#define PRMMOTW_REG				(0xF02)			/* 0xF02: モータ容量 */
#define PRMBITDP_REG			(0xF03)			/* 0xF03: エンコーダビット数, 位置データ小数点位置 */
#define PRMYOPT_REG				(0xE13)			/* 0xE13: Ｙ仕様番号 */

/*--------------------------------------------------------------------------------------------------*/
#define FUN_CONTINUE			(0L)			/*Continue	*/
#define FUN_END					(1L)			/*End		*/

/*--------------------------------------------------------------------------------------------------*/
/* Fn011 Motot Type & Y Specification Display 														*/
/*--------------------------------------------------------------------------------------------------*/
#define	PNL_MTYPEDISP_MAX		3U

/*--------------------------------------------------------------------------------------------------*/
#define OPEREG_WRITE( idx, data, hmsg )   (COPEREG->Write( COPEREG, (idx), (data), hmsg ))
#define PRMREG_READX( idx, pResData, CheckType, hmsg )	(CPARAMX->Read( (COBJECT *)(CPARAMX),(idx),\
																	(pResData), (CheckType), hmsg ))  /* <S073> */

/****************************************************************************************************/
/*																									*/
/*		Function List																				*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Public Function																				*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunMotorTypeBegin(FUNMSG *Fmsg); 			/* Motot Type & Y Specification Display Mode Begin	*/
LONG FunMotorTypeMain(FUNMSG *Fmsg);			/* Motot Type & Y Specification Display Mode Main	*/
void FunMotorTypeEnd(FUNMSG *Fmsg);				/* Motot Type & Y Specification Displayr Mode End	*/

/*--------------------------------------------------------------------------------------------------*/
/*		Private Function																			*/
/*--------------------------------------------------------------------------------------------------*/
static CHAR FunExpDivCal(CHAR x, CHAR y, ULONG *data);		/* 指数値算出 10^x / 10^y				*/
static USHORT FunDispDecToHex(USHORT x);					/* 表示用データ変換						*/
static void FunMotorTypeDisplay(FUNMSG *Fmsg);				/* Fn011 Display						*/
static LONG LpxLcdFunMotorTypeExe(FUNMSG *Fmsg);			/* LCD Operator Fn011 Execution			*/

#if  SVFSEL_PANELOP_USE == 0
#define LpxPnlFunMotorTypeExe( x ) (1)
#else
static LONG LpxPnlFunMotorTypeExe(FUNMSG *hMsg);	/* Panel Operator Fn011 Execution 				*/
#endif

/*****************************************************************************
  Description: Motot Type & Y Specification Display mode Main
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		0: continue, 1: end 
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
LONG FunMotorTypeMain(FUNMSG *Fmsg)
{
	MSGIF *hMsg = Fmsg->Super;
	LONG rc;
	rc = FUN_CONTINUE;
/*--------------------------------------------------------------------------*/
	if( KPI_CHK_LopHmsg( hMsg ) == TRUE )					/* LCD Operator */
	{
		rc = LpxLcdFunMotorTypeExe( Fmsg );
	} 
/*--------------------------------------------------------------------------*/
	else if ( KPI_CHK_PopHmsg( hMsg ) == TRUE )				/*Panel Operator*/
	{
		rc = LpxPnlFunMotorTypeExe( Fmsg );
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
  Description: Motot Type & Y Specification Display mode Begin
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		OK: Success, Others: Failure 
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
LONG FunMotorTypeBegin(FUNMSG *Fmsg)
{
LONG	rc = OK;
BOOL ResFlag = TRUE;
FNMOTORINFOHV  *pv = (FNMOTORINFOHV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	pv->var.State = FNMOTORINFOST_INIT;
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
	if( ResFlag == TRUE )
	{
		rc = CPARAMX->Search( (COBJECT *)CPARAMX,			/* Search Register (Prm 0xF00)			*/
				PRMTYPME_REG,
				0,
				&(pv->var.PrmReadIdx_typm_e),
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
		rc = CPARAMX->Search( (COBJECT *)CPARAMX,			/* Search Register (Prm 0xF19)			*/
				PRMEXPMOTWSPD_REG,
				0,
				&(pv->var.PrmReadIdx_exp_motw_spd),
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
		rc = CPARAMX->Search( (COBJECT *)CPARAMX,			/* Search Register (Prm 0xF02)			*/
				PRMMOTW_REG,
				0,
				&(pv->var.PrmReadIdx_motw),
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
		rc = CPARAMX->Search( (COBJECT *)CPARAMX,			/* Search Register (Prm 0xF03)			*/
				PRMBITDP_REG,
				0,
				&(pv->var.PrmReadIdx_bit_dp),
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
		rc = CPARAMX->Search( (COBJECT *)CPARAMX,			/* Search Register (Prm 0xE13)			*/
				PRMYOPT_REG,
				0,
				&(pv->var.PrmReadIdx_yopt),
				MCHK_REG_READ,
				Hmsg);
		if( rc != OK )
		{
			ResFlag = FALSE;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/* Set PlsCntClr Mode to Mode Register 																*/
/*--------------------------------------------------------------------------------------------------*/
	if ( ResFlag == TRUE )
	{
		rc = OPEREG_WRITE( pv->var.OpeModeIdx, FNMOTORINFO_MODE, Hmsg );
	}
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}


/*****************************************************************************
  Description: Motot Type & Y Specification Display Mode End
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	none
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
void FunMotorTypeEnd(FUNMSG *Fmsg)
{
FNMOTORINFOHV  *pv = (FNMOTORINFOHV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------*/
	OPEREG_WRITE( pv->var.OpeModeIdx, FNNORMAL_MODE, Hmsg );
}


/*****************************************************************************
  Description: 指数値算出 10^x / 10^y
------------------------------------------------------------------------------
  Parameters:
	x;
	y;
	data;
  Return:
	n;
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
static CHAR FunExpDivCal(CHAR x, CHAR y, ULONG *data)
{
	CHAR	n;
	UCHAR	i;
	ULONG	m;

	n = x - y;
	i = ( n < 0 )? -n : n;

	for ( m = 1; i != 0; i-- )
	{
		m = ( m << 3 ) + ( m << 1 );
	}
	*data = m;

	return n;
}


/*****************************************************************************
  Description: 表示用データ変換
------------------------------------------------------------------------------
  Parameters:
	x;
  Return:
	ans;
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
static USHORT FunDispDecToHex(USHORT x)
{
	USHORT	q;
	USHORT	ans;
	USHORT	work;

	if( x > 9999 )
	{
		ans = 9999;
	}

	q = x / 1000;
	ans = q * 0x1000;
	work = x - q * 1000;
	q = work / 100;
	ans += q * 0x100;
	work -= q * 100;
	q = work / 10;
	ans += q * 0x10;
	work -= q * 10;
	ans += work;
	
	return(ans);
}


/*****************************************************************************
  Description: Fn011 Motot Type & Y Specification Display Data
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	none
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
static void FunMotorTypeDisplay(FUNMSG *Fmsg)
{
	CHAR	exp;
	USHORT	i;
	USHORT	work;
	ULONG	sol;
	ULONG	lwork;
	ULONG	lwork1;
	FNMOTORINFOHV *pv = (FNMOTORINFOHV *)(Fmsg->pVar);
	UCHAR *ResBuf = Fmsg->pTxBuf;
	MSGIF *Hmsg = Fmsg->Super;
	OPFBK_HNDL		*FbkOpt;
	FbkOpt = &(Fmsg->Super->pAxRsc->OptCardHdl->FbkOpt);
	
	/* Clear	*/
	RpxLcdClrTxBuf( ResBuf );

	if( Fmsg->Super->pAxRsc->UniPrms->Bprm->SencFbOptUse == TRUE )
	{
//		RpxLcdSetStr( 0, 6, (UCHAR *)"-FBOptInfo-", ResBuf );		/* <S154> */
		RpxLcdSetStr( 0, 8, (UCHAR *)"FBOptInfo", ResBuf );			/* <S154> */
	}
	else
	{
//		RpxLcdSetStr(0, 6, (UCHAR *)"-MotorInfo-", ResBuf );		/* <S154> */
		RpxLcdSetStr(0, 8, (UCHAR *)"MotorInfo", ResBuf );			/* <S154> */
	}
	if( ( ( Fmsg->Super->pAxRsc->UniPrms->Bprm->SencFbOptUse == TRUE ) &&
			( ( FbkOpt->conf.EncInfo.Model[0] == '\0' ) && ( FbkOpt->conf.MotInfo.Model[0] == '\0' ) ) )
		|| ( FbkOpt->var.f.PrmSumErrAsSemi == TRUE ) )
	{
		RpxLcdSetStr( 1U, 0U, (UCHAR *)"                 ", ResBuf );
		RpxLcdSetStr( 2U, 0U, (UCHAR *)" Not available   ", ResBuf );
		RpxLcdSetStr( 3U, 0U, (UCHAR *)"                 ", ResBuf );
	}
	else if ( Fmsg->Super->pAxRsc->MencV->EncConnect == TRUE )
	{
		/* モータ機種, 入力電圧		*/
		if( Fmsg->Super->pAxRsc->UniPrms->Bprm->AmpDcinType == 1 )
		{
			RpxLcdSetStr( 1U, 0U, (UCHAR *)"TYPE    DC   V ", ResBuf );
		}
		else
		{
			RpxLcdSetStr( 1U, 0U, (UCHAR *)"TYPE    AC   V ", ResBuf );
		}
		PRMREG_READX( pv->var.PrmReadIdx_typm_e, &(pv->var.ResData_typm_e), MCHK_REG_READ, Hmsg );
		RpxLcdSetValHex( 1U, 5U, (ULONG)( pv->var.ResData_typm_e & 0x00FF ), 2U, ResBuf );
		RpxLcdSetDecPnt( 1U, 10U, Fmsg->Super->pAxRsc->UniPrms->Bprm->MotUnVolt, 3, (USHORT)TRUE, 0, ResBuf );
		
		/* モータ容量	*/
		RpxLcdSetStr( 2U, 13U, (UCHAR *)"W ", ResBuf );
		PRMREG_READX( pv->var.PrmReadIdx_exp_motw_spd, &(pv->var.ResData_exp_motw_spd), MCHK_REG_READ, Hmsg );
		PRMREG_READX( pv->var.PrmReadIdx_motw, &(pv->var.ResData_motw), MCHK_REG_READ, Hmsg );
		exp = FunExpDivCal( ( pv->var.ResData_exp_motw_spd & 0x00FF ), 0, &sol );
		if( exp > 0 )
		{
			lwork = pv->var.ResData_motw * (USHORT)sol;
		}
		else if ( exp < 0 )
		{
			lwork = pv->var.ResData_motw / (USHORT)sol;
		}
		else
		{
			lwork = pv->var.ResData_motw;
		}
		lwork1 = lwork;
		i = 0;
		while( lwork1 > 0 )
		{
			i++;
			lwork1 = lwork1 / 10;
		}
		if( ( i > 0 ) && ( i < 10 ) ) /* ULONG 10進最大桁数	*/
		{
			RpxLcdSetDecPnt( 2U, (USHORT)(13 - i), lwork, i, (USHORT)FALSE, 0, ResBuf );
		}
		else
		{
			RpxLcdSetStr( 2U, 8U, (UCHAR *)"    ?W ", ResBuf );
		}
		
		/* エンコーダタイプ, エンコーダ分解能	*/
		RpxLcdSetStr( 3U, 0U, (UCHAR *)"ENCODER      bit", ResBuf );
		RpxLcdSetValHex( 3U, 8U, (ULONG)( ( ( ( pv->var.ResData_typm_e & 0xFF00 ) >> 8 )& 0xF0 ) >> 4 ), 2U, ResBuf );
		PRMREG_READX( pv->var.PrmReadIdx_bit_dp, &(pv->var.ResData_bit_dp), MCHK_REG_READ, Hmsg );
		work = FunDispDecToHex( ( pv->var.ResData_bit_dp & 0x00FF ) );
		RpxLcdSetValHex( 3U, 11U, (ULONG)work, 2U, ResBuf);
	}
	else
	{
		RpxLcdSetStr( 1U, 0U, (UCHAR *)"                 ", ResBuf );
		RpxLcdSetStr( 2U, 0U, (UCHAR *)" Not connect     ", ResBuf );
		RpxLcdSetStr( 3U, 0U, (UCHAR *)"                 ", ResBuf );
	}
	
	/* Y仕様	*/
	PRMREG_READX( pv->var.PrmReadIdx_yopt, &(pv->var.ResData_yopt), MCHK_REG_READ, Hmsg );
	if( pv->var.ResData_yopt != 0 )
	{
		RpxLcdSetStr( 4U, 0U, (UCHAR *)"Yspec   Y", ResBuf );
		RpxLcdSetDecPnt( 4U, 9U, (pv->var.ResData_yopt), 4U, (USHORT)FALSE, 0U, ResBuf );
	}
	else
	{
		RpxLcdSetStr( 4U, 0U, (UCHAR *)"                ", ResBuf );
	}
}

/*****************************************************************************
  Description: LCD Operator Fn011 Execution
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		FUN_CONTINUE: Execution Continue, FUN_END: Execution End
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
static LONG LpxLcdFunMotorTypeExe(FUNMSG *Fmsg)
{
FNMOTORINFOHV *pv = (FNMOTORINFOHV *)(Fmsg->pVar);
OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
LONG rc = FUN_CONTINUE;														/* 0: continue, 1: end	*/
/*--------------------------------------------------------------------------------------------------*/
/*		MotorInfo (Fn011) Sequence																	*/
/*--------------------------------------------------------------------------------------------------*/
		switch( pv->var.State )
		{
/*--------------------------------------------------------------------------------------------------*/
		case FNMOTORINFOST_INIT: 											/* Initial State 		*/
/*--------------------------------------------------------------------------------------------------*/
			pv->var.State = FNMOTORINFOST_EXE;
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNMOTORINFOST_EXE: 											/* Exe State 			*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = FNMOTORINFOST_END;
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNMOTORINFOST_END: 											/* End state 			*/
/*--------------------------------------------------------------------------------------------------*/
			FunMotorTypeEnd( Fmsg );
			rc = FUN_END;
			break;
		default:
			break;
		}
		FunMotorTypeDisplay( Fmsg );
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}


/*****************************************************************************
  Description: PANEL Operator Fn011 Execution
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		FUN_CONTINUE: Execution Continue, FUN_END: Execution End
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
#if  SVFSEL_PANELOP_USE == 1
static LONG LpxPnlFunMotorTypeExe(FUNMSG *Fmsg)
{
FNMOTORINFOHV *pv = (FNMOTORINFOHV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
LEDDSP *DspData = (LEDDSP *)&(Fmsg->pTxBuf[PNLRSP_LEDCODE_IDX]);		/* set display data buffer	*/
OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
USHORT work;
ULONG	lwork;
CHAR	exp;
ULONG	sol;
LONG rc = FUN_CONTINUE;													/* 0: continue, 1: end		*/
/*--------------------------------------------------------------------------------------------------*/
/*		MotorInfo (Fn011) Sequence																	*/
/*--------------------------------------------------------------------------------------------------*/
		switch( pv->var.State )
		{
/*--------------------------------------------------------------------------------------------------*/
		case FNMOTORINFOST_INIT: 											/* Initial State 		*/
/*--------------------------------------------------------------------------------------------------*/
			pv->var.DPos = FNMOTORINFOST_TYPM_E_L;
			pv->var.State = FNMOTORINFOST_EXE;
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNMOTORINFOST_EXE: 											/* Exe State 			*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.data == TRUE )
			{
				pv->var.State = FNMOTORINFOST_END;
			}
			else if ( OpKeyMake.mode == TRUE )
			{
				if( pv->var.DPos < PNL_MTYPEDISP_MAX )
				{
					pv->var.DPos++;
				}
				else
				{
					pv->var.DPos = FNMOTORINFOST_TYPM_E_L;
				}
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNMOTORINFOST_END: 											/* End state 			*/
/*--------------------------------------------------------------------------------------------------*/
			FunMotorTypeEnd( Fmsg );
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
		/* パネル表示データ作成	*/
		switch( pv->var.DPos )
		{
			case FNMOTORINFOST_TYPM_E_L:
				RpxLedSetStr( (UCHAR *)"F.", DspData, 4U, 2U );
				PRMREG_READX( pv->var.PrmReadIdx_typm_e, &(pv->var.ResData_typm_e ), MCHK_REG_READ, Hmsg );
				work = ( ( ( ( ( pv->var.ResData_typm_e & 0xFF00 ) >> 8 ) ) & 0x0F) << 8) | ( pv->var.ResData_typm_e & 0x00FF );
				RpxLedSetValHex( work, DspData, 3U, 4U );
				break;
			case FNMOTORINFOST_MOTW:
				RpxLedSetStr( (UCHAR *)"P.", DspData, 4U, 2U );
				PRMREG_READX( pv->var.PrmReadIdx_exp_motw_spd, &(pv->var.ResData_exp_motw_spd ), MCHK_REG_READ, Hmsg );
				PRMREG_READX( pv->var.PrmReadIdx_motw, &(pv->var.ResData_motw ), MCHK_REG_READ, Hmsg );
				exp = FunExpDivCal( ( pv->var.ResData_exp_motw_spd & 0x00FF ), 1, &sol );
				if( exp > 0 )
				{
					lwork = pv->var.ResData_motw * (USHORT)sol;
				}
				else if ( exp < 0 )
				{
					lwork = pv->var.ResData_motw / (USHORT)sol;
				}
				else
				{
					lwork = pv->var.ResData_motw;
				}
				RpxLedSetValDec( (USHORT)lwork, DspData, 3U, 4U, (USHORT)FALSE, 0U );
				break;
			case FNMOTORINFOST_TYPM_E_H:
				RpxLedSetStr( (UCHAR *)"E.", DspData, 4U, 2U );
				PRMREG_READX( pv->var.PrmReadIdx_bit_dp, &(pv->var.ResData_bit_dp), MCHK_REG_READ, Hmsg );
				PRMREG_READX( pv->var.PrmReadIdx_typm_e, &(pv->var.ResData_typm_e ), MCHK_REG_READ, Hmsg );
				work = FunDispDecToHex( ( pv->var.ResData_bit_dp & 0x00FF ) );
				work = ( work | ( ( ( ( pv->var.ResData_typm_e & 0xFF00 ) >> 8 ) & 0xF0 ) << 4 ) );
				RpxLedSetValHex( work, DspData, 3U, 4U );
				break;
			case FNMOTORINFOST_YOPT:
				PRMREG_READX( pv->var.PrmReadIdx_yopt, &(pv->var.ResData_yopt ), MCHK_REG_READ, Hmsg );
				RpxLedSetStr( (UCHAR *)"Y.", DspData, 4U, 2U );
				RpxLedSetValDec( pv->var.ResData_yopt, DspData, 3U, 4U, (USHORT)FALSE, 0U );
				break;
			default:
				break;
		}
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}
#endif

/***************************************** end of file **********************************************/
