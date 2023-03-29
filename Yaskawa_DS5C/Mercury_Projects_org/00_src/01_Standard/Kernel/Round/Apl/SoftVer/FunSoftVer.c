/*****************************************************************************
Description		: Fn012 : Software Version Display
-----------------------------------------------------------------------------
Author		: SHCORE
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

Functions	  :
	-- APIs --
	FunSoftVersionMain();					:Fn012 : Software Version Display mode Main
	FunSoftVersionBegin();					:Fn012 : Software Version Display mode Begin
	FunSoftVersionEnd();					:Fn012 : Software Version Display mode End

	-- Locals --
	FunLcdDispVersion();					:DispVersion Display
	FunSoftVersionDisplay();				:Fn012 Software Version Display Data
	LpxLcdFunSoftVersionExe();				:LCD Operator Fn012 Execution
	LpxPnlFunSoftVersionExe();				:PANEL Operator Fn012 Execution

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
SL.Xue		2013.10.12		created

*****************************************************************************/

#include	"FunSoftVer.h"


/*--------------------------------------------------------------------------------------------------*/
/* Operation Register 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define OPEMODE_REG1			(0x0000)		/* 1st Operation Mode Register */
#define OPESET_REG1				(0x0001)		/* 1st Operation Mode Set Register */

/*--------------------------------------------------------------------------------------------------*/
/* Operation Mode 																					*/
/*--------------------------------------------------------------------------------------------------*/
#define FNNORMAL_MODE			(0x0000U)		/* Normal Mode	*/
#define FNSOFTVER_MODE			(0xFFFFU)		/* Software Version Display Mode */

/*--------------------------------------------------------------------------------------------------*/
/* Prm Command 																						*/
/*--------------------------------------------------------------------------------------------------*/
#define PRMENCVER_REG			(0xF01)			/* 0xF01: エンコーダソフトバージョン */

/*--------------------------------------------------------------------------------------------------*/
/* Info Command 																					*/
/*--------------------------------------------------------------------------------------------------*/
#define INFOID005_REG			(0x0005)		/* 0x0005: 制御モジュールファームウェアバージョン */
#define INFOID006_REG			(0x0006)		/* 0x0006: 制御モジュールファームウェアバージョン */
#define INFOID007_REG			(0x0007)		/* 0x0007: 制御モジュールY仕様番号 */

/*--------------------------------------------------------------------------------------------------*/
/* フィードバックオプションＩＤ定義 																*/
/*--------------------------------------------------------------------------------------------------*/
#define CCSW_FBKID				0				/* dummy */
#define FBKID_NONE				0

/*--------------------------------------------------------------------------------------------------*/
/* 指令オプションＩＤ定義 																			*/
/*--------------------------------------------------------------------------------------------------*/
#define CMDID_NONE				0

/*--------------------------------------------------------------------------------------------------*/
/* アクセスレベル(注意：0 は使用不可) 																*/
/*--------------------------------------------------------------------------------------------------*/
//#define ACCLVL_SYSTEM			3				/* システム */	/* <S101> */
#define ACCLVL_SYSTEM			4				/* システム */	/* <S101> */

/*--------------------------------------------------------------------------------------------------*/
/* セーフティオプションＩＤ定義 																	*/
/*--------------------------------------------------------------------------------------------------*/
#define SFTID_NONE				0

/*--------------------------------------------------------------------------------------------------*/
/* SGDV MECHATROLINK-III形 																			*/
/*--------------------------------------------------------------------------------------------------*/
#define	CCSW_SFTID				SFTID_NONE		/* Safety ID */

/*--------------------------------------------------------------------------------------------------*/
/* ネットワークＩＤ定義	<V009>																		*/
/*--------------------------------------------------------------------------------------------------*/
#define	NETID_NONE			0

/*--------------------------------------------------------------------------------------------------*/
/* ΣVmini パルス基板																				*/
/*--------------------------------------------------------------------------------------------------*/
#define	CCSW_NETID				NETID_NONE		/* Network ID */

/*--------------------------------------------------------------------------------------------------*/
#define FUN_CONTINUE			(0L)			/*Continue	*/
#define FUN_END					(1L)			/*End		*/

/*--------------------------------------------------------------------------------------------------*/
#define OPEREG_WRITE( idx, data, hmsg )		(COPEREG->Write( COPEREG, (idx), (data), hmsg ))
#define PRMREG_READX( idx, pResData, CheckType, hmsg )	(CPARAMX->Read( (COBJECT *)(CPARAMX),(idx),\
																	(pResData), (CheckType), hmsg ))  /* <S073> */
#define INFOREG_READX( idx, pResData, CheckType, hmsg )	(CINFREG->Read( (COBJECT *)(CINFREG),(idx),\
																	(pResData), (CheckType), hmsg ))


/****************************************************************************************************/
/*																									*/
/*		Function List																				*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Public Function																				*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunSoftVersionBegin(FUNMSG *Fmsg);					/* Fn012 Software Version Display Mode Begin*/
LONG FunSoftVersionMain(FUNMSG *Fmsg);					/* Fn012 Software Version Display Mode Main	*/
void FunSoftVersionEnd(FUNMSG *Fmsg);					/* Fn012 Software Version Display Mode End	*/

/*--------------------------------------------------------------------------------------------------*/
/*		Private Function																			*/
/*--------------------------------------------------------------------------------------------------*/
static void	FunLcdDispVersion(FUNMSG *Fmsg, LONG Line,
		UCHAR *Str, USHORT State, USHORT Ver, USHORT YVer, USHORT TVer);	/* DispVersion Display	*/
static void	FunSoftVersionDisplay(FUNMSG *Fmsg);		/* Fn012 Software Version Display Data		*/
static LONG LpxLcdFunSoftVersionExe(FUNMSG *Fmsg);		/* LCD Operator Fn012 Execution 			*/

#if  SVFSEL_PANELOP_USE == 0
#define LpxPnlFunSoftVersionExe( x ) (1)
#else
static LONG LpxPnlFunSoftVersionExe(FUNMSG *hMsg);		/* Panel Operator Fn012 Execution 			*/
#endif

/*****************************************************************************
  Description: Fn012 Software Version Display Mode Main
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		0: continue, 1: end 
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
LONG FunSoftVersionMain(FUNMSG *Fmsg)
{
	MSGIF *hMsg = Fmsg->Super;
	LONG rc;
	rc = FUN_CONTINUE;
/*--------------------------------------------------------------------------*/
	if( KPI_CHK_LopHmsg( hMsg ) == TRUE )					/* LCD Operator */
	{
		rc = LpxLcdFunSoftVersionExe( Fmsg );
	} 
/*--------------------------------------------------------------------------*/
	else if ( KPI_CHK_PopHmsg( hMsg ) == TRUE )				/*Panel Operator*/
	{
		rc = LpxPnlFunSoftVersionExe( Fmsg );
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
  Description: Fn012 Software Version Display Mode Begin
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		OK: Success, Others: Failure 
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
LONG FunSoftVersionBegin(FUNMSG *Fmsg)
{
LONG	rc = OK;
BOOL ResFlag = TRUE;
FNSOFTVERHV  *pv = (FNSOFTVERHV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	pv->var.State = VERDISP_SEQ_DRV;
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
		rc = CPARAMX->Search( (COBJECT *)CPARAMX,			/* Search Register (Prm 0xF01)			*/
				PRMENCVER_REG,
				0,
				&(pv->var.PrmReadIdx_encver),
				MCHK_REG_READ,
				Hmsg);
		if( rc != OK )
		{
			ResFlag = FALSE;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
#if 0	/* <S0D1> Delete Start */
	if( ResFlag == TRUE )
	{
		rc = CINFREG->Search( (COBJECT *)CINFREG,			/* Search Register (Info 0x0005)		*/
				INFOID005_REG,
				0,
				&(pv->var.InfoReadIdx_id005),
				MCHK_REG_READ,
				Hmsg);
		if( rc != OK )
		{
			ResFlag = FALSE;
		}
	}
#endif	/* <S0D1> Delete End */
/*--------------------------------------------------------------------------------------------------*/
	if( ResFlag == TRUE )
	{
		rc = CINFREG->Search( (COBJECT *)CINFREG,			/* Search Register (Info 0x0007)		*/
				INFOID007_REG,
				0,
				&(pv->var.InfoReadIdx_id007),
				MCHK_REG_READ,
				Hmsg);
		if( rc != OK )
		{
			ResFlag = FALSE;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/* Set SoftVer Mode to Mode Register 																*/
/*--------------------------------------------------------------------------------------------------*/
	if ( ResFlag == TRUE )
	{
		rc = OPEREG_WRITE( pv->var.OpeModeIdx, FNSOFTVER_MODE, Hmsg );
	}
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}


/*****************************************************************************
  Description: Fn012 Software Version Display Mode End
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	none
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
void FunSoftVersionEnd(FUNMSG *Fmsg)
{
FNSOFTVERHV  *pv = (FNSOFTVERHV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------*/
	OPEREG_WRITE( pv->var.OpeModeIdx, FNNORMAL_MODE, Hmsg );
}


/*****************************************************************************
  Description: DispVersion Display
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:

------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void	FunLcdDispVersion(FUNMSG *Fmsg, LONG Line,
				UCHAR *Str, USHORT State, USHORT Ver, USHORT YVer, USHORT TVer)
{
	UCHAR *ResBuf = Fmsg->pTxBuf;
	MSGIF *Hmsg = Fmsg->Super;

	RpxLcdSetStr( (USHORT)Line, 0U, (UCHAR *)Str, ResBuf );
	switch ( State )
	{
		case 0:
			if ( Hmsg->AccessLvl == ACCLVL_SYSTEM )
			{
				RpxLcdSetStr( (USHORT)Line, 11U, (UCHAR *)"t", ResBuf );
				RpxLcdSetValHex( (USHORT)Line, 12U, TVer, 4U, ResBuf );
			}
			/*fall through*/
		case 1:
			RpxLcdSetStr( (USHORT)( Line + 1 ), 1U, (UCHAR *)"Ver. = ", ResBuf );
			RpxLcdSetValHex( (USHORT)( Line + 1), 7U, Ver, 4U, ResBuf );
			if ( YVer != 0 )
			{
				RpxLcdSetStr( (USHORT)( Line + 1 ), 11U, (UCHAR *)"_", ResBuf );
				RpxLcdSetValHex( (USHORT)( Line + 1 ), 12U, YVer, 4U, ResBuf );
			}
			else
			{
				RpxLcdSetStr( (USHORT)( Line + 1 ), 11U, (UCHAR *)"      ", ResBuf );
			}
			break;
		case 2:
			RpxLcdSetStr( (USHORT)( Line + 1 ), 0U, (UCHAR *)" Not connect     ", ResBuf );
			break;
		case 3:
			RpxLcdSetStr( (USHORT)( Line + 1 ), 0U, (UCHAR *)" Not available   ", ResBuf );
			break;
		default :
			break;
	}
}


/*****************************************************************************
  Description: Fn012 Software Version Display Data
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:

------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void	FunSoftVersionDisplay(FUNMSG *Fmsg)
{
	UCHAR *ResBuf = Fmsg->pTxBuf;
	MSGIF *Hmsg = Fmsg->Super;
	FNSOFTVERHV *pv = (FNSOFTVERHV *)(Fmsg->pVar);
	OPFBK_HNDL		*FbkOpt;
	OPCMD_HNDL		*CmdOpt;
	OPSFT_HNDL		*SftOpt;
	CmdOpt = &(Fmsg->Super->pAxRsc->OptCardHdl->CmdOpt);
	FbkOpt = &(Fmsg->Super->pAxRsc->OptCardHdl->FbkOpt);
	SftOpt = &(Fmsg->Super->pAxRsc->OptCardHdl->SftOpt);

	/* clear Tx buffer */
	RpxLcdClrTxBuf( ResBuf );
	/* TITLE		*/
//	RpxLcdSetStr( 0U, 7U, (UCHAR *)"-Soft Ver-", ResBuf );		/* <S154> */
	RpxLcdSetStr( 0U, 9U, (UCHAR *)"Soft Ver", ResBuf );		/* <S154> */
	switch( pv->var.State )
	{
		case VERDISP_SEQ_DRV :
		case VERDISP_SEQ_YSPEC :
		case VERDISP_SEQ_ENC :
			PRMREG_READX( pv->var.PrmReadIdx_encver, &(pv->var.ResData_encver), MCHK_REG_READ, Hmsg );
//			INFOREG_READX( pv->var.InfoReadIdx_id005, &(pv->var.ResData_id005), MCHK_REG_READ, Hmsg );	/* <S0D1> Delete */
			INFOREG_READX( pv->var.InfoReadIdx_id007, &(pv->var.ResData_id007), MCHK_REG_READ, Hmsg );

//			FunLcdDispVersion( Fmsg, 1, "DRIVER", 0, ( (pv->var.ResData_id005 & 0xF0 ) >> 4),			/* <S0D1> Delete */
//											pv->var.ResData_id007, (pv->var.ResData_id005 & 0x0F) );	/* <S0D1> Delete */

			FunLcdDispVersion( Fmsg, 1, "DRIVER", 0, ( (SERVO_FIRM_VER & 0xFFFF0000 ) >> 16),			/* <S0D1> */
											pv->var.ResData_id007, (SERVO_FIRM_VER & 0x0000FFFF) );		/* <S0D1> */

			if( Fmsg->Super->pAxRsc->UniPrms->Bprm->SencFbOptUse == FALSE )
			{
				if( Fmsg->Super->pAxRsc->MencV->EncConnect == TRUE )
				{
					FunLcdDispVersion( Fmsg, 3, "ENCODER", 1, pv->var.ResData_encver, 0, 0 );
				}
				else
				{
					FunLcdDispVersion( Fmsg, 3, "ENCODER", 2, 0, 0, 0 );
				}
			}
			break;
		case VERDISP_SEQ_OPTENC :
			if( FbkOpt->conf.OpInfo.f.Connect == TRUE )
			{
				if( ( FbkOpt->conf.OpInfo.IfType == FBKOP_FULLCLS_ONLY ) &&
					( Fmsg->Super->pAxRsc->UniPrms->Bprm->FencUse == TRUE ) &&
					Fmsg->Super->pAxRsc->FencV->EncConnect )
				{
					FunLcdDispVersion( Fmsg, 1, "OPTION ENCODER", 1, Fmsg->Super->pAxRsc->FencV->SoftVer, 0, 0 );
				}
				else
				{
					FunLcdDispVersion( Fmsg, 1, "OPTION ENCODER", 2, 0, 0, 0 );
				}
			}
			else
			{
				FunLcdDispVersion( Fmsg, 1, "OPTION ENCODER", 2, 0, 0, 0 );
			}
			break;
		case VERDISP_SEQ_MII :
			//FunLcdDispVersion( Fmsg, 1, "MECHATROLINK-IF", 1, SvtReg.IfMswVersion, SvtReg.IfYspVersion, 0 );
			break;
		case VERDISP_SEQ_CMD :
			if( CmdOpt->conf.OpInfo.f.Connect == TRUE  )
			{
				if( CmdOpt->conf.OpInfo.IfType == CMDOP_NOT_SUPPORTED )
				{
					FunLcdDispVersion( Fmsg, 1, "COMMAND", 1, CmdOpt->conf.OpInfo.SoftVer, 0, 0 );
				}
				else
				{
					FunLcdDispVersion( Fmsg, 1, "COMMAND", 1, CmdOpt->conf.OpInfo.SoftVer,
							CmdOpt->conf.OpInfo.SoftYspVer, CmdOpt->conf.OpInfo.SoftTstVer );
				}
			}
			else
			{
				FunLcdDispVersion( Fmsg, 1, "COMMAND", 2, 0, 0, 0 );
			}
			break;
		case VERDISP_SEQ_SAFETY :
		case VERDISP_SEQ_YSAFETY :
			if( SftOpt->conf.OpInfo.f.Connect == TRUE )
			{
				if( SftOpt->conf.OpInfo.IfType == SFTOP_ADVANCED_SAFETY_IF )
				{
					FunLcdDispVersion( Fmsg, 1, "SAFETY", 0, SftOpt->conf.OpInfo.SoftVer,
							SftOpt->conf.OpInfo.SoftYspVer, SftOpt->conf.OpInfo.SoftTstVer );
				}
				else
				{
					FunLcdDispVersion( Fmsg, 1, "SAFETY", 3, 0, 0, 0 );
				}
			}
			else
			{
				FunLcdDispVersion( Fmsg, 1, "SAFETY", 2, 0, 0, 0 );
			}
			break;
		case VERDISP_SEQ_FB :
		case VERDISP_SEQ_YFB :
			if( FbkOpt->conf.OpInfo.f.Connect == TRUE )
			{
				if( ( FbkOpt->conf.OpInfo.f.Connect == FBKOP_SCNV_MODULE ) &&
						( ( Fmsg->Super->pAxRsc->UniPrms->Bprm->SencFbOptUse == TRUE ) ||
						( Fmsg->Super->pAxRsc->UniPrms->Bprm->FencUse == TRUE ) ) )
				{
					if( ( FbkOpt->var.f.PrmSumErrAsSemi == TRUE ) || ( FbkOpt->var.f.PrmSumErrAsFulc == TRUE ) )
					{
						FunLcdDispVersion( Fmsg, 1, "FEEDBACK", 3, 0, 0, 0 );
					}
					else
					{
						FunLcdDispVersion( Fmsg, 1, "FEEDBACK", 1, FbkOpt->conf.OpInfo.SoftVer,
								FbkOpt->conf.OpInfo.SoftYspVer, FbkOpt->conf.OpInfo.SoftTstVer );
					}
				}
				else if ( Fmsg->Super->pAxRsc->UniPrms->Bprm->FencUse == TRUE )
				{
					FunLcdDispVersion( Fmsg, 1, "FEEDBACK", 3, 0, 0, 0 );
				}
				else
				{
					FunLcdDispVersion( Fmsg, 1, "FEEDBACK", 2, 0, 0, 0 );
				}
			}
			else
			{
				FunLcdDispVersion( Fmsg, 1, "FEEDBACK", 2, 0, 0, 0 );
			}
			break;
		case VERDISP_SEQ_BOOT1 :
			//FunLcdDispVersion( Fmsg, 1, "BOOT PROGRAM2", 1, Kprm.Bt2MswVersion, Kprm.Bt2TstVersion, 0 );
			RpxLcdSetStr( 3U, 0U, (UCHAR *)"BOOT CHECKSUM2", ResBuf );
			RpxLcdSetStr( 4U, 1U, (UCHAR *)"Sum. = ", ResBuf );
			//RpxLcdSetValHex( 4U, 7U, Kprm.Bt2CheckSum, 4U, ResBuf );
			break;
		case VERDISP_SEQ_END :
			break;
		default :
			break;
	}
}


/*****************************************************************************
  Description: LCD Operator Fn012 Execution
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		FUN_CONTINUE: Execution Continue, FUN_END: Execution End
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
static LONG LpxLcdFunSoftVersionExe(FUNMSG *Fmsg)
{
FNSOFTVERHV *pv = (FNSOFTVERHV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
LONG rc = FUN_CONTINUE;														/* 0: continue, 1: end	*/
OPFBK_HNDL		*FbkOpt;
FbkOpt = &(Fmsg->Super->pAxRsc->OptCardHdl->FbkOpt);
/*--------------------------------------------------------------------------------------------------*/
/*		SoftVer (Fn012) Sequence																	*/
/*--------------------------------------------------------------------------------------------------*/
		if( OpKeyMake.mode == TRUE )
		{
			pv->var.State = VERDISP_SEQ_END;
		}
		switch( pv->var.State )
		{
/*--------------------------------------------------------------------------------------------------*/
		case VERDISP_SEQ_DRV: 												/* Drv State			*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.data == TRUE )
			{
				if ( ( CCSW_FBKID != FBKID_NONE ) && ( 1 /* Iprm.f.FbkOpSupport == TRUE */ ) )
				{
					if( FbkOpt->conf.OpInfo.IfType == FBKOP_SCNV_MODULE )
					{
						if( ( CCSW_CMDID != CMDID_NONE ) && ( 1 /* Iprm.f.CmdOpSupport == TRUE */ ) )
						{
							if( Hmsg->AccessLvl == ACCLVL_SYSTEM )
							{
								pv->var.State = VERDISP_SEQ_MII;
							}
							else
							{
								pv->var.State = VERDISP_SEQ_CMD;
							}
						}
						else if ( ( CCSW_SFTID != SFTID_NONE ) && ( 1 /* Iprm.f.SftOpSupport == TRUE */ ) )
						{
							pv->var.State = VERDISP_SEQ_SAFETY;
						}
						else
						{
							pv->var.State = VERDISP_SEQ_FB;
						}
					}
					else
					{
						pv->var.State = VERDISP_SEQ_OPTENC;
					}
				}
				else if ( ( CCSW_NETID != NETID_NONE ) && ( 1 /* Iprm.f.CmdOpSupport == TRUE */ ) )
				{
					if( Hmsg->AccessLvl == ACCLVL_SYSTEM )
					{
						pv->var.State = VERDISP_SEQ_MII;
					}
					else
					{
						pv->var.State = VERDISP_SEQ_CMD;
					}
				}
				else if ( ( CCSW_SFTID != SFTID_NONE ) && ( 1 /* Iprm.f.SftOpSupport == TRUE */ ) )
				{
					pv->var.State = VERDISP_SEQ_SAFETY;
				}
				else if ( Hmsg->AccessLvl == ACCLVL_SYSTEM )
				{
					pv->var.State = VERDISP_SEQ_BOOT1;
				}
				else
				{
					/* do nothing */
				}
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case VERDISP_SEQ_OPTENC: 											/* Optenc State			*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.data == TRUE )
			{
				if( ( CCSW_CMDID != CMDID_NONE ) && ( 1 /* Iprm.f.CmdOpSupport == TRUE */ ) )
				{
					if( Hmsg->AccessLvl == ACCLVL_SYSTEM )
					{
						pv->var.State = VERDISP_SEQ_MII;
					}
					else
					{
						pv->var.State = VERDISP_SEQ_CMD;
					}
				}
				else if ( ( CCSW_SFTID != SFTID_NONE ) && ( 1 /* Iprm.f.SftOpSupport == TRUE */ ) )
				{
					pv->var.State = VERDISP_SEQ_SAFETY;
				}
				else
				{
					pv->var.State = VERDISP_SEQ_FB;
				}
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case VERDISP_SEQ_MII: 												/* Mii State			*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.data == TRUE )
			{
				pv->var.State = VERDISP_SEQ_CMD;
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case VERDISP_SEQ_CMD: 												/* Cmd State			*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.data == TRUE )
			{
				if( ( CCSW_SFTID != SFTID_NONE ) && ( 1 /* Iprm.f.SftOpSupport == TRUE */ ) )
				{
					pv->var.State = VERDISP_SEQ_SAFETY;
				}
				else if ( ( CCSW_FBKID != FBKID_NONE ) && ( 1 /* Iprm.f.FbkOpSupport == TRUE */ )  )
				{
					pv->var.State = VERDISP_SEQ_FB;
				}
				else if ( Hmsg->AccessLvl == ACCLVL_SYSTEM )
				{
					pv->var.State = VERDISP_SEQ_BOOT1;
				}
				else
				{
					pv->var.State = VERDISP_SEQ_DRV;
				}
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case VERDISP_SEQ_SAFETY: 											/* Safety State			*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.data == TRUE )
			{
				if( ( CCSW_FBKID != FBKID_NONE ) && ( 1 /* Iprm.f.FbkOpSupport == TRUE */ ) )
				{
					pv->var.State = VERDISP_SEQ_FB;
				}
				else if ( Hmsg->AccessLvl == ACCLVL_SYSTEM )
				{
					pv->var.State = VERDISP_SEQ_BOOT1;
				}
				else
				{
					pv->var.State = VERDISP_SEQ_DRV;
				}
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case VERDISP_SEQ_FB: 												/* Fb State				*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.data == TRUE )
			{
				if( Hmsg->AccessLvl == ACCLVL_SYSTEM )
				{
					pv->var.State = VERDISP_SEQ_BOOT1;
				}
				else
				{
					pv->var.State = VERDISP_SEQ_DRV;
				}
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
	case VERDISP_SEQ_BOOT1: 												/* Boot1 State			*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.data == TRUE )
			{
				pv->var.State = VERDISP_SEQ_DRV;
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case VERDISP_SEQ_END: 												/* End state			*/
/*--------------------------------------------------------------------------------------------------*/
			FunSoftVersionEnd( Fmsg );
			rc = FUN_END;
			break;
		default:
			break;
		}
		FunSoftVersionDisplay( Fmsg );
	return (rc);
}


/*****************************************************************************
  Description: PANEL Operator Fn012 Execution
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		FUN_CONTINUE: Execution Continue, FUN_END: Execution End
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
#if  SVFSEL_PANELOP_USE == 1
static LONG LpxPnlFunSoftVersionExe(FUNMSG *Fmsg)
{
FNSOFTVERHV *pv = (FNSOFTVERHV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
LEDDSP *DspData = (LEDDSP *)&(Fmsg->pTxBuf[PNLRSP_LEDCODE_IDX]);		/* set display data buffer	*/
OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
LONG rc = FUN_CONTINUE;													/* 0: continue, 1: end		*/
OPSFT_HNDL		*SftOpt;
OPFBK_HNDL		*FbkOpt;
SftOpt = &(Fmsg->Super->pAxRsc->OptCardHdl->SftOpt);
FbkOpt = &(Fmsg->Super->pAxRsc->OptCardHdl->FbkOpt);
/*--------------------------------------------------------------------------------------------------*/
/*		SoftVer (Fn012) Sequence																	*/
/*--------------------------------------------------------------------------------------------------*/
//		INFOREG_READX( pv->var.InfoReadIdx_id005, &(pv->var.ResData_id005), MCHK_REG_READ, Hmsg );	/* <S0D1> Delete */
		INFOREG_READX( pv->var.InfoReadIdx_id007, &(pv->var.ResData_id007), MCHK_REG_READ, Hmsg );
		if( OpKeyMake.data == TRUE )
		{
			pv->var.State = VERDISP_SEQ_END;
		}
		switch( pv->var.State )
		{
/*--------------------------------------------------------------------------------------------------*/
		case VERDISP_SEQ_DRV: 												/* Drv State 			*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.mode == TRUE )
			{
				if ( pv->var.ResData_id007 == 0 /* SvSw.pv->var.ResData_id007 == 0 */ )
				{
					pv->var.State = VERDISP_SEQ_ENC;
				}
				else
				{
					pv->var.State = VERDISP_SEQ_YSPEC;
				}
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case VERDISP_SEQ_YSPEC: 											/* Yspec State 			*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = VERDISP_SEQ_ENC;
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case VERDISP_SEQ_ENC: 												/* Enc State 			*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.mode == TRUE )
			{
				if ( ( CCSW_FBKID != FBKID_NONE ) && ( 1 /* Iprm.f.FbkOpSupport == TRUE */ ) )
				{
					pv->var.State = VERDISP_SEQ_OPTENC;
				}
				else if ( ( CCSW_SFTID != SFTID_NONE ) && ( 1 /* Iprm.f.SftOpSupport == TRUE */ ) )
				{
					pv->var.State = VERDISP_SEQ_SAFETY;
				}
				else
				{
					pv->var.State = VERDISP_SEQ_DRV;
				}
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case VERDISP_SEQ_OPTENC: 											/* Optenc State 		*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.mode == TRUE )
			{
				if ( ( CCSW_SFTID != SFTID_NONE ) && ( 1 /* Iprm.f.SftOpSupport == TRUE */ ) )
				{
					pv->var.State = VERDISP_SEQ_SAFETY;
				}
				else
				{
					pv->var.State = VERDISP_SEQ_FB;
				}
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case VERDISP_SEQ_SAFETY: 											/* Safety State 		*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.mode == TRUE )
			{
				if ( SftOpt->conf.OpInfo.YNumber == 0xFFFF )
				{
					if ( ( CCSW_FBKID != FBKID_NONE ) && ( 1 /* Iprm.f.FbkOpSupport == TRUE */ ) )
					{
						pv->var.State = VERDISP_SEQ_FB;
					}
					else
					{
						pv->var.State = VERDISP_SEQ_DRV;
					}
				}
				else
				{
					pv->var.State = VERDISP_SEQ_YSAFETY;
				}
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case VERDISP_SEQ_YSAFETY: 											/* Ysafety State 		*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.mode == TRUE )
			{
				if ( ( CCSW_FBKID != FBKID_NONE ) && ( 1 /* Iprm.f.FbkOpSupport == TRUE */ ) )
				{
					pv->var.State = VERDISP_SEQ_FB;
				}
				else
				{
					pv->var.State = VERDISP_SEQ_DRV;
				}
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case VERDISP_SEQ_FB: 												/* Fb State 			*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.mode == TRUE )
			{
				if ( FbkOpt->conf.OpInfo.YNumber == 0xFFFF )
				{
					pv->var.State = VERDISP_SEQ_DRV;
				}
				else
				{
					pv->var.State = VERDISP_SEQ_YFB;
				}
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case VERDISP_SEQ_YFB: 												/* Yfb State 			*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = VERDISP_SEQ_DRV;
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case VERDISP_SEQ_END: 												/* End state 			*/
/*--------------------------------------------------------------------------------------------------*/
			FunSoftVersionEnd( Fmsg );
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
		switch( pv->var.State )
		{
			case VERDISP_SEQ_DRV :
				RpxLedSetStr( (UCHAR *)"r.", DspData, 4U, 2U );
//				RpxLedSetValHex( ( ( pv->var.ResData_id005 & 0xF0 ) >> 4 ), DspData, 3U, 4U );	/* <S0D1> Delete */
				RpxLedSetValHex(  ( (SERVO_FIRM_VER & 0xFFFF0000 ) >> 16 ), DspData, 3U, 4U );	/* <S0D1> */
				break;
			case VERDISP_SEQ_YSPEC :
				RpxLedSetStr( (UCHAR *)"U.", DspData, 4U, 2U );
				RpxLedSetValHex( pv->var.ResData_id007, DspData, 3U, 4U );
				break;
			case VERDISP_SEQ_ENC :
				PRMREG_READX( pv->var.PrmReadIdx_encver, &(pv->var.ResData_encver), MCHK_REG_READ, Hmsg );
				RpxLedSetStr( (UCHAR *)"E.", DspData, 4U, 2U );
				RpxLedSetValHex( pv->var.ResData_encver, DspData, 3U, 4U );
				break;
			case VERDISP_SEQ_OPTENC :
				RpxLedSetStr( (UCHAR *)"o.", DspData, 4U, 2U );
				RpxLedSetValHex( Fmsg->Super->pAxRsc->FencV->SoftVer, DspData, 3U, 4U );
				break;
			case VERDISP_SEQ_SAFETY :
				RpxLedSetStr( (UCHAR *)"S.", DspData, 4U, 2U );
				RpxLedSetValHex( SftOpt->conf.OpInfo.SoftVer, DspData, 3U, 4U );
				break;
			case VERDISP_SEQ_YSAFETY :
				RpxLedSetStr( (UCHAR *)"b.", DspData, 4U, 2U );
				RpxLedSetValHex( SftOpt->conf.OpInfo.SoftYspVer, DspData, 3U, 4U );
				break;
			case VERDISP_SEQ_FB :
				RpxLedSetStr( (UCHAR *)"F.", DspData, 4U, 2U );
				RpxLedSetValHex( FbkOpt->conf.OpInfo.SoftVer, DspData, 3U, 4U );
				break;
			case VERDISP_SEQ_YFB :
				RpxLedSetStr( (UCHAR *)"c.", DspData, 4U, 2U );
				RpxLedSetValHex( FbkOpt->conf.OpInfo.SoftYspVer, DspData, 3U, 4U );
				break;
			case VERDISP_SEQ_MII :
			case VERDISP_SEQ_CMD :
			case VERDISP_SEQ_BOOT1 :
			case VERDISP_SEQ_END :
				break;
			default:
				break;
		}
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}
#endif


/***************************************** end of file **********************************************/
