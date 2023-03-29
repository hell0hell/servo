/*****************************************************************************
Description		: Fn01E : Servo & Motor ID Display	Mode
-----------------------------------------------------------------------------
Author		: SHCORE,
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

Functions	  :
	-- APIs --
	FunSvMotOpIDBegin();			:Fn01E Servo & Motor ID Display Mode Begin
	FunSvMotOpIDMain();				:Fn01E Servo & Motor ID Display Mode Main
	FunSvMotOpIDEnd();				:Fn01E Servo & Motor ID Display Mode End
	-- Locals --
	LpxSvMotOpIDExe();				:Fn01E Servo & Motor ID Display Mode Execution
	FunServoIdDispDisplay();		:Fn01E Servo & Motor ID Display Data

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
ZH.QIN		2013.10.10		created
SL.Xue		2013.09.09		機種定義エリア変量を追加した

*****************************************************************************/

#include	"FunSvMotOpID.h"		/* Kernel API */
/* defines */
/*--------------------------------------------------------------------------------------------------*/
/* Operation Register 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define OPEMODE_REG1		(0x0000)		/* 1st Operation Mode Register */
#define OPESET_REG1			(0x0001)		/* 1st Operation Mode Set Register */

/*--------------------------------------------------------------------------------------------------*/
/* Info Register 																					*/
/*--------------------------------------------------------------------------------------------------*//* <S0D8> */
#if 0
#define INFOID800_REG		(0x0800)		/* 0x0800: 制御モジュール形式文字列 */
#define INFOID80C_REG		(0x080C)		/* 0x080C: 制御モジュールシリアルナンバー文字列 */
#define INFOID814_REG		(0x0814)		/* 0x0814: 制御モジュール製造年月 */
#endif
#define INFOID800_REG		(0x0800)		/* 0x0800: 制御モジュール形式文字列 */
#define INFOID808_REG		(0x0808)		/* 0x0808: 制御モジュールシリアルナンバー文字列 */
#define INFOID810_REG		(0x0810)		/* 0x0810: 制御モジュール製造年月 */

/*--------------------------------------------------------------------------------------------------*/
/* Prm Register 																					*/
/*--------------------------------------------------------------------------------------------------*/
#define PRMMOTW_REG			(0xF02)			/* 0xF02: モータ容量 */

/*--------------------------------------------------------------------------------------------------*/
/* Operation Mode	 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define FNNORMAL_MODE		(0x0000U) 		/* Normal Mode */
#define FNSVMOTOPID_MODE	(0xFFFFU)		/* Servo & Motor ID Display	Mode	*/

/*--------------------------------------------------------------------------------------------------*/
/* Operation Command 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define FUN_CONTINUE			0L				/*	Continue				*/
#define FUN_END					1L				/*	End						*/
/*--------------------------------------------------------------------------------------------------*/
#define	SQRT2					1414214

/*--------------------------------------------------------------------------------------------------*/
#define OPEREG_WRITE( idx, data, hmsg )   (COPEREG->Write( COPEREG, (idx), (data), hmsg ))
#define PRMREG_READX( idx, pResData, CheckType, hmsg )	(CPARAMX->Read( (COBJECT *)(CPARAMX),(idx),\
																	(pResData), (CheckType), hmsg ))
#define INFOREG_READX( idx, pResData, CheckType, hmsg )	(CINFREG->Read( (COBJECT *)(CINFREG),(idx),\
																	(pResData), (CheckType), hmsg ))

/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
/* APIs */
LONG FunSvMotOpIDBegin(FUNMSG *Fmsg);			/* Fn01E Servo & Motor ID Display Mode Begin		*/
LONG FunSvMotOpIDMain(FUNMSG *Fmsg);			/* Fn01E Servo & Motor ID Display Mode Main			*/
void FunSvMotOpIDEnd(FUNMSG *Fmsg);				/* Fn01E Servo & Motor ID Display Mode End			*/

/* Locals */
static LONG LpxSvMotOpIDExe(FUNMSG *Fmsg);			/* Fn01E Servo & Motor ID Display Mode Execution */
static void FunServoIdDispDisplay(FUNMSG *Fmsg);	/*	Fn01E Servo & Motor ID Display Data			*/

#if  SVFSEL_PANELOP == 0
#define LpxPnlSvMotOpIDExe( x ) (0)
#else
//<S166>static LONG LpxPnlSvMotOpIDExe(FUNMSG *Fmsg);		/* Panel Operator Fn01E Execution 				 */
#endif

/*****************************************************************************
  Description: Fn01E : Servo & Motor ID Display	Mode Main
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG		0: continue, 1: end
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
LONG FunSvMotOpIDMain(FUNMSG *Fmsg)
{
MSGIF *hMsg = Fmsg->Super;
LONG rc;
rc = FUN_CONTINUE;
/*--------------------------------------------------------------------------------------------------*/
	if( KPI_CHK_LopHmsg( hMsg ) == TRUE )							/* LCD Operator */
	{
		rc = LpxSvMotOpIDExe( Fmsg );
	}
/*--------------------------------------------------------------------------------------------------*/
	else if ( KPI_CHK_PopHmsg( hMsg ) == TRUE )						/* Panel Operator */
	{
		rc = FUN_END /* <S166> do not execute!! LpxPnlSvMotOpIDExe( Fmsg ) */;
	}
	else
	{
		/*do nothing	*/
	}
/*--------------------------------------------------------------------------------------------------*/
	return rc;
}



/*****************************************************************************
  Description: Fn01E : Servo & Motor ID Display	Mode Begin
------------------------------------------------------------------------------
  Parameters:
	Fmsg			Operator Function Message
  Return:
  	LONG			OK: Soft Reset Success, Other: Failure
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
LONG FunSvMotOpIDBegin(FUNMSG *Fmsg)
{
LONG	rc = OK;
BOOL ResFlag = TRUE;
FNSVMOTOPIDV  *pv = (FNSVMOTOPIDV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	if ( KPI_CHK_LopHmsg( Hmsg ) != TRUE ) { return (NG); } 		/* <S166> */
/*--------------------------------------------------------------------------------------------------*/
	pv->var.State = IDDISP_SEQ_INIT;
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
		rc = CINFREG->Search( (COBJECT *)CINFREG,			/* Search Register (Info 0x0800)		*/
				INFOID800_REG,
				0,
				&(pv->var.InfoReadIdx_id800),
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
		rc = CINFREG->Search( (COBJECT *)CINFREG,			/* Search Register (Info 0x0808)		*/
//				INFOID80C_REG, 						/* <S0D8> */
				INFOID808_REG, 						/* <S0D8> */
				0,
//				&(pv->var.InfoReadIdx_id80C), 						/* <S0D8> */
				&(pv->var.InfoReadIdx_id808), 						/* <S0D8> */
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
		rc = CINFREG->Search( (COBJECT *)CINFREG,			/* Search Register (Info 0x0810)		*/
//				INFOID814_REG, 						/* <S0D8> */
				INFOID810_REG, 						/* <S0D8> */
				0,
//				&(pv->var.InfoReadIdx_id814),
				&(pv->var.InfoReadIdx_id810),
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
	/* Set Servo & Motor ID Display	Mode to Mode Register 												*/
	/*--------------------------------------------------------------------------------------------------*/
	if ( ResFlag == TRUE )
	{
		rc = OPEREG_WRITE( pv->var.OpeModeIdx, FNSVMOTOPID_MODE , Hmsg );
	}
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}


/*****************************************************************************
  Description:	Fn01E : Servo & Motor ID Display Mode End
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	None
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
void FunSvMotOpIDEnd(FUNMSG *Fmsg)
{
FNSVMOTOPIDV  *pv = (FNSVMOTOPIDV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------*/
	OPEREG_WRITE( pv->var.OpeModeIdx, FNNORMAL_MODE, Hmsg );
}


/*****************************************************************************
  Description:	Fn01E : Servo & Motor ID Display Mode Execution
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	LONG			FUN_CONTINUE: Execution Continue, FUN_END: Execution End
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static LONG LpxSvMotOpIDExe(FUNMSG *Fmsg)
{
FNSVMOTOPIDV *pv = (FNSVMOTOPIDV *)(Fmsg->pVar);
OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
LONG rc = FUN_CONTINUE;													/* 0: continue, 1: end */
/*--------------------------------------------------------------------------------------------------*/
/*		SvMotOpID (Fn01E) Sequence																	*/
/*--------------------------------------------------------------------------------------------------*/
	if ( OpKeyMake.mode == TRUE )
	{
		pv->var.State = IDDISP_SEQ_END; 	/* 終了						*/
	}
	else if ( OpKeyMake.shup == TRUE)
	{
		pv->var.DPos = 0;					/* 初期表示					*/
	}
	else if ( OpKeyMake.shdown == TRUE)
	{
		pv->var.DPos = 1;					/* 拡張表示					*/
	}
		switch( pv->var.State )
		{
/*--------------------------------------------------------------------------------------------------*/
		case IDDISP_SEQ_INIT:
/*--------------------------------------------------------------------------------------------------*/
			pv->var.DPos = 0;
			pv->var.State = IDDISP_SEQ_DRV;
			break;
/*--------------------------------------------------------------------------------------------------*/
		case IDDISP_SEQ_DRV :
		case IDDISP_SEQ_MOT :
/*--------------------------------------------------------------------------------------------------*/
			if (OpKeyMake.data == TRUE )	/* 表示切替					*/
			{
				pv->var.State ++;
				pv->var.DPos = 0;
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case IDDISP_SEQ_ENC:
/*--------------------------------------------------------------------------------------------------*/
			if (OpKeyMake.data == TRUE )	/* 表示切替					*/
			{
			/*
			 * 	TODO:
				if ( (CCSW_CMDID != CMDID_NONE) && (Iprm.f.CmdOpSupport == TRUE) )
				{
					pv->var.State = IDDISP_SEQ_CMDOP;
				}
				else if ( (CCSW_SFTID != SFTID_NONE) && (Iprm.f.SftOpSupport == TRUE) )
				{
					pv->var.State = IDDISP_SEQ_ASCOP;
				}
				else if ( (CCSW_FBKID != FBKID_NONE) && (Iprm.f.FbkOpSupport == TRUE) )
				{
					pv->var.State = IDDISP_SEQ_FBOP;
				}
				else
				{
					pv->var.State = IDDISP_SEQ_DRV;
				}
			*/
				pv->var.State ++;	/* <S144> */
				pv->var.DPos = 0;
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case IDDISP_SEQ_CMDOP :
/*--------------------------------------------------------------------------------------------------*/
			if (OpKeyMake.data == TRUE )	/* 表示切替					*/
			{
			/*
			 * 	TODO:
				if ( (CCSW_SFTID != SFTID_NONE) && (Iprm.f.SftOpSupport == TRUE) )
				{
					pv->var.State = IDDISP_SEQ_ASCOP;
				}
				else if ( (CCSW_FBKID != FBKID_NONE) && (Iprm.f.FbkOpSupport == TRUE) )
				{
					pv->var.State = IDDISP_SEQ_FBOP;
				}
				else
				{
					pv->var.State = IDDISP_SEQ_DRV;
				}
				*/
				pv->var.State ++;	/* <S144> */
				pv->var.DPos = 0;
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case IDDISP_SEQ_ASCOP :
/*--------------------------------------------------------------------------------------------------*/
			if (OpKeyMake.data == TRUE )	/* 表示切替					*/
			{
			/*
			 * TODO:
				if ( (CCSW_FBKID != FBKID_NONE) && (Iprm.f.FbkOpSupport == TRUE) )
				{
					pv->var.State = IDDISP_SEQ_FBOP;
				}
				else
				{
					pv->var.State = IDDISP_SEQ_DRV;
				}
			*/
				pv->var.State ++;	/* <S144> */
				pv->var.DPos = 0;
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case IDDISP_SEQ_FBOP :
/*--------------------------------------------------------------------------------------------------*/
			if (OpKeyMake.data == TRUE )
			{
				pv->var.State = IDDISP_SEQ_DRV;
				pv->var.DPos = 0;
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case IDDISP_SEQ_END:
/*--------------------------------------------------------------------------------------------------*/
			FunSvMotOpIDEnd( Fmsg );
			rc = FUN_END;
			break;
		default:
			break;
		}

		FunServoIdDispDisplay( Fmsg );
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}

/*****************************************************************************
  Description:	Fn01E LCD Display Option ID
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	LONG			FUN_CONTINUE: Execution Continue, FUN_END: Execution End
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
void	FunLcdDispOptionID( FUNMSG *Fmsg, OPTCARD_INFO *OpInfo )
{
	LONG	i;
	UCHAR	strbuf[3];
	LONG	j;
	LONG    k;
	FNSVMOTOPIDV *pv = (FNSVMOTOPIDV *)(Fmsg->pVar);
	UCHAR *ResBuf = Fmsg->pTxBuf;					/* set response buffer */
/*--------------------------------------------------------------*/
/*		拡張表示処理対応										*/
/*--------------------------------------------------------------*/
		if ( pv->var.DPos == 0 )
		{				/* 初期表示								*/
			j = 0;		/* 表示開始位置 1文字目					*/
			k = 9;		/* ループ回数 LCD最大文字数(17文字)分	*/
		}
		else
		{				/* 拡張表示								*/
			j = 4;		/* 表示開始位置 9文字目					*/
			k = 12 - 4;	/* ループ回数 24-8=16文字分				*/
		}
	/*----------------------------------------------------------*/
		for ( i = 0; i < k; i++ )
		{
			/*--------------------------------------------------*/
			/* 形式表示											*/
			/*--------------------------------------------------*/
			strbuf[0] = OpInfo->ModelStr[i+j] & 0x00FF;
			strbuf[1] = ( OpInfo->ModelStr[i+j] & 0xFF00 ) >> 8;
			strbuf[2] = 0;
			RpxLcdSetStr( 2U, (USHORT)(2*i), strbuf, ResBuf );
			/*--------------------------------------------------*/
			/* シリアル番号表示									*/
			/*--------------------------------------------------*/
			strbuf[0] = OpInfo->SerNumStr[i+j] & 0x00FF;
			strbuf[1] = ( OpInfo->SerNumStr[i+j] & 0xFF00 ) >> 8;
			strbuf[2] = 0;
			RpxLcdSetStr( 3U, (USHORT)(2*i), strbuf, ResBuf );
		}
		if ( pv->var.DPos == 0 )
		{
			/* 製造年月日表示				*/
			RpxLcdSetDecPnt( 4U, 0U, (ULONG)(OpInfo->Date[0]), 2U, FALSE, 0U, ResBuf );
			RpxLcdSetStr( 4U, 2U, (UCHAR *)".", ResBuf );
			RpxLcdSetDecPnt( 4U, 3U, (ULONG)(OpInfo->Date[1]), 2U, FALSE, 0U, ResBuf );
			/* オプションカードＩＤ表示		*/
			RpxLcdSetValHex( 4U, 8U, OpInfo->IdType, 4U, ResBuf );
		}
		else
		{
			/* オプションカードＩＤ表示		*/
			RpxLcdSetValHex( 4U, 0U, OpInfo->IdType, 4U, ResBuf );
		}
		return;
}


/*****************************************************************************
  Description:	Fn01E Servo & Motor ID Display Data
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:

------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void FunServoIdDispDisplay(FUNMSG *Fmsg)
{
LONG	i;
LONG    j;
UCHAR	strbuf[3];
LONG	kx;
LONG    sx;
	UCHAR *ResBuf = Fmsg->pTxBuf;
FNSVMOTOPIDV *pv = (FNSVMOTOPIDV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
	MENCPRM			*MencP;
	OPCMD_HNDL		*CmdOpt;
	OPSFT_HNDL		*SftOpt;
	OPFBK_HNDL		*FbkOpt;
	MencP = &(Fmsg->Super->pAxRsc->UniPrms->Prm->MencP);
	CmdOpt = &(Fmsg->Super->pAxRsc->OptCardHdl->CmdOpt);
	SftOpt = &(Fmsg->Super->pAxRsc->OptCardHdl->SftOpt);
	FbkOpt = &(Fmsg->Super->pAxRsc->OptCardHdl->FbkOpt);

	INFOREG_READX( pv->var.InfoReadIdx_id800, &(pv->var.ResData_id800), MCHK_REG_READ, Hmsg );		/* <S0D8> *//* 不要？	*/
	INFOREG_READX( pv->var.InfoReadIdx_id808, &(pv->var.ResData_id808), MCHK_REG_READ, Hmsg );		/* <S0D8> *//* 不要？	*/
	INFOREG_READX( pv->var.InfoReadIdx_id810, &(pv->var.ResData_id810), MCHK_REG_READ, Hmsg );		/* <S0D8> */
	PRMREG_READX( pv->var.PrmReadIdx_motw, &(pv->var.ResData_motw), MCHK_REG_READ, Hmsg );

	/* clear Tx buffer */
	RpxLcdClrTxBuf( ResBuf );
	/* TITLE		*/
//	RpxLcdSetStr( 0U, 6U, (UCHAR *)"-SvMotOpID-", ResBuf );			/* <S154> */
	RpxLcdSetStr( 0U, 8U, (UCHAR *)"SvMotOpID", ResBuf );			/* <S154> */
	kx = MlibScalKxgain( 1000000, 1, SQRT2, &sx, -1 );
	if ( pv->var.DPos == 0 )
	{
		j = 0;
	}
	else
	{
		j = 8;
	}
	switch( pv->var.State )
	{
	case IDDISP_SEQ_DRV :			/* サーボパック情報表示処理			*/
		RpxLcdSetStr(1U, 0U, (UCHAR *)"Driver ", ResBuf);

		for(i = 0; i < 21 - j; i += 2)
		{
			/* 形式表示						*/
			strbuf[0] = Fmsg->Super->pAxRsc->ModuleID->IfInfo->ProductName[i+j];
			strbuf[1] = Fmsg->Super->pAxRsc->ModuleID->IfInfo->ProductName[i+j+1];
			strbuf[2] = 0;
			RpxLcdSetStr(2U, (USHORT)i, strbuf, ResBuf);
		}
		for(i = 0; i < 16 - j; i += 2)
		{
			/* シリアル番号表示				*/
			strbuf[0] = Fmsg->Super->pAxRsc->ModuleID->IfInfo->SerialNumber[i+j];
			strbuf[1] = Fmsg->Super->pAxRsc->ModuleID->IfInfo->SerialNumber[i+j+1];
			strbuf[2] = 0;
			RpxLcdSetStr(3U, (USHORT)i, strbuf, ResBuf);
		}
		if ( pv->var.DPos == 0 )
		{
			/* 製造年月日表示				*/
//			RpxLcdSetDecPnt(4U, 0U, (ULONG)( pv->var.ResData_id814 & 0x00FF ), 2U, FALSE, 0U, ResBuf); 			/* <S0D8> */
			RpxLcdSetDecPnt(4U, 0U, (ULONG)( ( pv->var.ResData_id810 & 0xFF00 ) >> 8 ), 2U, FALSE, 0U, ResBuf);		/* <S0D8> */
			RpxLcdSetStr(4U, 2U, (UCHAR *)".", ResBuf);
//			RpxLcdSetDecPnt(4U, 3U, (ULONG)( ( pv->var.ResData_id814 & 0xFF00 ) >> 8 ), 2U, FALSE, 0U, ResBuf);		/* <S0D8> */
			RpxLcdSetDecPnt(4U, 3U, (ULONG)( pv->var.ResData_id810 & 0x00FF ), 2U, FALSE, 0U, ResBuf);			/* <S0D8> */
			/* サーボパック入力電圧表示		*/
			RpxLcdSetDecPnt(4U, 6U, Fmsg->Super->pAxRsc->UniPrms->Bprm->AmpUnVolt, 3U, TRUE, 0U, ResBuf);
			RpxLcdSetStr(4U, 9U, (UCHAR *)"V,", ResBuf);
			/* サーボパック容量表示			*/
			RpxLcdSetDecPnt(4U, 11U, Fmsg->Super->pAxRsc->UniPrms->Prm->ampw, 5U, TRUE, 0U, ResBuf);
			RpxLcdSetStr(4U, 16U, (UCHAR *)"W", ResBuf);
		}
		else
		{
			/* サーボパック容量表示			*/
			RpxLcdSetDecPnt(4U, 0U, Fmsg->Super->pAxRsc->UniPrms->Prm->ampw, 5U, TRUE, 0U, ResBuf);
			RpxLcdSetStr(4U, 5U, (UCHAR *)"W,", ResBuf);
			/* サーボパック定格電流表示		*/
			RpxLcdSetDecPnt(4U, 8U, (ULONG)MlibMulgain( Fmsg->Super->pAxRsc->UniPrms->Prm->irats, kx ), 3U, TRUE, 1U, ResBuf);
			RpxLcdSetStr(4U, 12U, (UCHAR *)"Arms", ResBuf);
		}
	break;
	case IDDISP_SEQ_MOT :			/* モータ情報表示処理				*/
		RpxLcdSetStr(1, 0, (UCHAR *)"Motor  ", ResBuf);
		if( ( Fmsg->Super->pAxRsc->MencV->EncConnect == TRUE ) ||
			( Fmsg->Super->pAxRsc->UniPrms->Bprm->SencFbOptUse == TRUE ) )
		{
			for(i = 0; i < 21 - j; i += 2)
			{
				/* 形式表示						*/
				strbuf[0] = Fmsg->Super->pAxRsc->MencV->MotInfo.Model[i+j];
				strbuf[1] = Fmsg->Super->pAxRsc->MencV->MotInfo.Model[i+j+1];
				strbuf[2] = 0;
				RpxLcdSetStr(2U, (USHORT)i, strbuf, ResBuf);
			}
			for(i = 0; i < 16 - j; i += 2)
			{
				/* シリアル番号表示				*/
				strbuf[0] = Fmsg->Super->pAxRsc->MencV->MotInfo.SNo[i+j];
				strbuf[1] = Fmsg->Super->pAxRsc->MencV->MotInfo.SNo[i+j+1];
				strbuf[2] = 0;
				RpxLcdSetStr(3U, (USHORT)i, strbuf, ResBuf);
			}
			if ( pv->var.DPos == 0 )
			{
				/* 製造年月日表示				*/
				for(i = 0; i < 6; i += 2)
				{
					strbuf[0] = Fmsg->Super->pAxRsc->MencV->MotInfo.Date[i];
					strbuf[1] = Fmsg->Super->pAxRsc->MencV->MotInfo.Date[i+1];
					strbuf[2] = 0;
					RpxLcdSetStr(4U, (USHORT)i, strbuf, ResBuf);
				}
				/* モータ入力電圧表示		*/
				RpxLcdSetDecPnt(4U, 6U, Fmsg->Super->pAxRsc->UniPrms->Bprm->MotUnVolt, 3U, TRUE, 0U, ResBuf);
				RpxLcdSetStr(4U, 9U, (UCHAR *)"V,", ResBuf);
				/* モータ容量表示			*/
				RpxLcdSetDecPnt(4U, 11U, pv->var.ResData_motw, 5U, TRUE, 0U, ResBuf);
				RpxLcdSetStr(4U, 16U, (UCHAR *)"W", ResBuf);
			}
			else
			{
				/* モータ容量表示			*/
				RpxLcdSetDecPnt(4U, 0U, pv->var.ResData_motw, 5U, TRUE, 0U, ResBuf);
				RpxLcdSetStr(4U, 5U, (UCHAR *)"W,", ResBuf);
				/* モータ定格電流表示		*/
				RpxLcdSetDecPnt(4U, 8U, (ULONG)MlibMulgain( MencP->irat, kx ), 3U, TRUE, 1U, ResBuf);
				RpxLcdSetStr(4U, 12U, (UCHAR *)"Arms", ResBuf);
			}
		}
		else
		{
			RpxLcdSetStr(2U, 0U, (UCHAR *)" Not connect     ", ResBuf);
		}
	break;

	case IDDISP_SEQ_ENC :			/* エンコーダ情報表示処理			*/
		RpxLcdSetStr(1U, 0U, (UCHAR *)"Encoder", ResBuf);
		if( ( Fmsg->Super->pAxRsc->MencV->EncConnect == TRUE ) ||
			( Fmsg->Super->pAxRsc->UniPrms->Bprm->SencFbOptUse == TRUE ) )
		{
			for(i = 0; i < 16 - j; i += 2)
			{
				/* 形式表示						*/
				strbuf[0] = Fmsg->Super->pAxRsc->MencV->EncInfo.Model[i+j];
				strbuf[1] = Fmsg->Super->pAxRsc->MencV->EncInfo.Model[i+j+1];
				strbuf[2] = 0;
				RpxLcdSetStr(2U, (USHORT)i, strbuf, ResBuf);
				/* シリアル番号表示				*/
				strbuf[0] = Fmsg->Super->pAxRsc->MencV->EncInfo.SNo[i+j];
				strbuf[1] = Fmsg->Super->pAxRsc->MencV->EncInfo.SNo[i+j+1];
				strbuf[2] = 0;
				RpxLcdSetStr(3U, (USHORT)i, strbuf, ResBuf);
			}
			/* 製造年月日表示				*/
			for(i = 0; i < 6; i += 2)
			{
				strbuf[0] = Fmsg->Super->pAxRsc->MencV->EncInfo.Date[i];
				strbuf[1] = Fmsg->Super->pAxRsc->MencV->EncInfo.Date[i+1];
				strbuf[2] = 0;
				RpxLcdSetStr(4U, (USHORT)i, strbuf, ResBuf);
			}
			/* エンコーダビット数表示		*/
			RpxLcdSetDecPnt(4U, 6U, (ULONG)MencP->bit_dp.b.l, 2U, TRUE, 0U, ResBuf);
			RpxLcdSetStr(4U, 8U, (UCHAR *)"bit-", ResBuf);
			/* エンコーダタイプ表示			*/
		//	if( ( MencP->typm_e.w & 0xf000 ) == TRUE )
			if( ( MencP->typm_e.w & 0xf000 ) )	/* <S11F> */
			{
				RpxLcdSetStr(4U, 12U, (UCHAR *)"ABS", ResBuf);
			}
			else
			{
				RpxLcdSetStr(4U, 12U, (UCHAR *)"INC", ResBuf);
			}
		}
		else
		{
			RpxLcdSetStr(2U, 0U, (UCHAR *)" Not connect     ", ResBuf);
		}
		break;

	case IDDISP_SEQ_CMDOP :
		RpxLcdSetStr(1U, 0U, (UCHAR *)"Command Option", ResBuf);
		if ( CmdOpt->conf.OpInfo.f.Connect == TRUE )
		{
			FunLcdDispOptionID( Fmsg, &CmdOpt->conf.OpInfo );
		}
		else
		{
			RpxLcdSetStr(2U, 0U, (UCHAR *)" Not connect     ", ResBuf);
		}
		break;

	case IDDISP_SEQ_ASCOP :
		RpxLcdSetStr(1U, 0U, (UCHAR *)"Safety Option", ResBuf);
		if ( SftOpt->conf.OpInfo.f.Connect == TRUE )
		{
			if( SftOpt->conf.OpInfo.IfType == SFTOP_ADVANCED_SAFETY_IF )
			{
				FunLcdDispOptionID( Fmsg, &SftOpt->conf.OpInfo );
			}
			else
			{
				RpxLcdSetStr(2U, 0U, (UCHAR *)" Not available   ", ResBuf);
			}
		}
		else
		{
			RpxLcdSetStr(2U, 0U, (UCHAR *)" Not connect     ", ResBuf);
		}
		break;

	case IDDISP_SEQ_FBOP :
		RpxLcdSetStr(1U, 0U, (UCHAR *)"Feedback Option", ResBuf);
		if ( ( Fmsg->Super->pAxRsc->UniPrms->Bprm->SencFbOptUse == TRUE ) ||
			( Fmsg->Super->pAxRsc->UniPrms->Bprm->FencUse == TRUE ) )
		{
			if ( FbkOpt->conf.OpInfo.IfType != FBKOP_FULLCLS_ONLY )
			{
				FunLcdDispOptionID( Fmsg, &FbkOpt->conf.OpInfo );
			}
			else
			{
				RpxLcdSetStr(2U, 0U, (UCHAR *)" Not available   ", ResBuf);
			}
		}
		else
		{
			RpxLcdSetStr(2U, 0U, (UCHAR *)" Not connect     ", ResBuf);
		}
		break;

	default:
		break;
	}
}


/***************************************** end of file **********************************************/


