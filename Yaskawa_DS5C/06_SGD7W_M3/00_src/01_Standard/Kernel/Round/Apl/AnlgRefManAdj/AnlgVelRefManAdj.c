/****************************************************************************************************/
/*																									*/
/*																									*/
/*		AnlgVelRefManAdj.c : Analog Velocity Reference Offset Manual Adjustment (Fn00A)				*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	Function:																						*/
/*			1) Fn00A Analog Velocity Reference Offset Manual Adjustment Mode Begin					*/
/*			2) Fn00A Analog Velocity Reference Offset Manual Adjustment Main						*/
/*			3) Fn00A Analog Velocity Reference Offset Manual Adjustment End							*/
/*			4) Fn00A Analog Velocity Reference Offset Manual Adjustment Display Data				*/
/*			5) Fn00A Execution (LCD Operator)														*/
/*			6) Fn00A Execution (Panel Operator) 													*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.1.00 : 2013.09.11  K.Ozaki	<S00C>														*/
/*		Rev.1.01 : 2013.12.22  T.Masuo	<S0A9> Remake For LCD Operator & Panel Operator				*/
/*																									*/
/****************************************************************************************************/
#include "KnlApi.h"					/* Kernel API */
#include "LcdOp.h"					/* LCD Operator */
#include "Monitor.h"				/* Un Monitor */
#include "Function.h"				/* Fn Function */
#include "Operation.h"				/* Servo Operation */
#include "PnPrmListTbl.h"			/* Register and Parameter Access Interface */
#include "MercuryGlobals.h"			/* Global Data Definitions */
#include "PrmAccessIf.h"			/* Register and Parameter Access Interface */
#include "FunManager.h"				/* Utility Function Manager */
#include "FunListDef.h"				/* Utility Function Header Fole */


/****************************************************************************************************/
/*																									*/
/*		Variable Definition																			*/
/*																									*/
/****************************************************************************************************/
typedef struct _FnRefVelManAdjV {
	struct{
		INT  State;						/* Analog Velocity Reference Offset Manual Adjustment Execution State */
		LONG OpeModeIdx;				/* Operation Mode Register Index */
		LONG OpeSetIdx;					/* Operation Set Register Index */
		LONG DispAdiData;				/* Display Data */
		LONG MonRegIdx;					/* Monitor Register Index */		/* <S0FA> */
		LONG MonRegData;				/* Monitor Register Read Data */	/* <S0FA> */
//		LONG Reserved[(256/4)-4]; 		/* Reserved: All functions can use ram variables at least 256 bytes. */	/* <S0FA> */
		LONG Reserved[(256/4)-6]; 		/* Reserved: All functions can use ram variables at least 256 bytes. */	/* <S0FA> */
	}var;
} FNREFVELMANADJV;

/****************************************************************************************************/
/*																									*/
/*		Constant Definition																			*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/* Operation Register 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define OPEMODE_REG1	(0x0000)	/* 1st Operation Mode Register(42H:0x80004000/40H:0x2000)		*/
#define OPESET_REG1		(0x0001)	/* 1st Operation Mode Set Register(42H:0x80004002/40H:0x2001)	*/

/*--------------------------------------------------------------------------------------------------*/
/* Operation Mode	 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define FNNORMAL_MODE				(0x0000U)	/* Normal Mode */
#define FNANLGVELREFMANADJ_MODE		(0x100AU)	/* Fn00A : Velocity Reference Offset Manual Adjustment Mode	*/

/*--------------------------------------------------------------------------------------------------*/
/* Monitor Register 																	<S0FA>		*/
/*--------------------------------------------------------------------------------------------------*/
#define MON_REG_CMD_SPD	(0x0001)	/* Monitor Register -Command Speed-(42H:0x8001C002/40H:0xE001)	*/

/*--------------------------------------------------------------------------------------------------*/
/* Minimum & Maximum Values 																		*/
/*--------------------------------------------------------------------------------------------------*/
#if	( SVFSEL_VREFAD_24BIT != 0 )
#define	ZADJV_MAX			52500000
#define	ZADJV_MIN			-52500000
#define	ZADJV_RATE			3500			/* 52500000÷15000 = 3500	*/
#else
#define	ZADJV_MAX			240000
#define	ZADJV_MIN			-240000
#define	ZADJV_RATE			16				/* 240000÷15000 = 16		*/
#endif

/*--------------------------------------------------------------------------------------------------*/
#define FUN_CONTINUE			(0L)		/*Continue	*/
#define FUN_END					(1L)		/*End		*/

/*--------------------------------------------------------------------------------------------------*/
#define OPEREG_WRITE( idx, data, hmsg )   (COPEREG->Write( COPEREG, (idx), (data), hmsg ))
#define UNREG_WRITE( idx, data, hmsg )   (CUNREGX->Write( (COBJECT *)CUNREGX, (idx), (data), hmsg ))
#define UNREG_READX( idx, pResData, CheckType, hmsg )    (CUNREGX->Read( (COBJECT *)(CUNREGX), (idx),\
															 	 	 (pResData), (CheckType), hmsg ))

/* Fn00A : Analog Velocity Reference Offset Manual Adjust */
enum _enum_Anlg_Vel_Ref_Man_Ref_State {
	ANLG_VEL_REF_MAN_ADJ_IDLE = 0,
	ANLG_VEL_REF_MAN_ADJ_EXE ,
	ANLG_VEL_REF_MAN_ADJ_END,
	ANLG_VEL_REF_MAN_ADJ_EXIT,				/* <S162>  */
};


/********************************************************************************************************************************/
/*																																*/
/*		Function List																											*/
/*																																*/
/********************************************************************************************************************************/
/*------------------------------------------------------------------------------------------------------------------------------*/
/*		Public Function																											*/
/*------------------------------------------------------------------------------------------------------------------------------*/
LONG FunAnlgVelRefManAdjBegin(FUNMSG *Fmsg);		/* Fn00A Analog Velocity Reference Offset Manual Adjustment Mode Begin		*/
LONG FunAnlgVelRefManAdjMain(FUNMSG *Fmsg);			/* Fn00A Analog Velocity Reference Offset Manual Adjustment Main			*/
void FunAnlgVelRefManAdjEnd(FUNMSG *Fmsg);			/* Fn00A Analog Velocity Reference Offset Manual Adjustment End				*/
void FunAnlgVelRefManAdjustDisplay( FUNMSG *Fmsg );	/* Fn00A Analog Velocity Reference Offset Manual Adjustment Display Data	*/

/*------------------------------------------------------------------------------------------------------------------------------*/
/*		Private Function																										*/
/*------------------------------------------------------------------------------------------------------------------------------*/
static LONG LpxAnlgVelRefManAdjExe(FUNMSG *Fmsg);		/* Fn00A Execution (LCD Operator) */

#if  SVFSEL_PANELOP_USE != 1
#define LpxPnlAnlgVelRefManAdjExe( x ) (1)
#else
LONG LpxPnlAnlgVelRefManAdjExe(FUNMSG *hMsg);			/* Fn00A Execution (Panel Operator) */
#endif

/*****************************************************************************
  Description: Fn00A Analog Velocity Reference Offset Manual Adjustment Main
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		0: continue, 1: end
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
LONG FunAnlgVelRefManAdjMain(FUNMSG *Fmsg)
{
	MSGIF *hMsg = Fmsg->Super;
	LONG rc;
	rc = FUN_CONTINUE;
/*--------------------------------------------------------------------------*/
	if( KPI_CHK_LopHmsg( hMsg ) == TRUE )					/* LCD Operator */
	{
		rc = LpxAnlgVelRefManAdjExe( Fmsg );
	}
/*--------------------------------------------------------------------------*/
	else if ( KPI_CHK_PopHmsg( hMsg ) == TRUE )				/*Panel Operator*/
	{
		rc = LpxPnlAnlgVelRefManAdjExe( Fmsg );
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
  Description:  Fn00A Analog Velocity Reference Offset Manual Adjustment Begin
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		OK: Success, Others: Failure
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
LONG FunAnlgVelRefManAdjBegin(FUNMSG *Fmsg)
{
	LONG	rc = OK;
	BOOL ResFlag = TRUE;
	FNREFVELMANADJV  *pv = (FNREFVELMANADJV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;

/*--------------------------------------------------------------------------------------------------*/
	pv->var.State = ANLG_VEL_REF_MAN_ADJ_IDLE;

/*--------------------------------------------------------------------------------------------------*/
/*	Search Register																					*/
/*--------------------------------------------------------------------------------------------------*/

	/* Search Register (Operation Mode Register : 42H：0x80004000/40H：0x2000 (= Nomal Mode ) */
	if ( ResFlag == TRUE )
	{
		rc = COPEREG->Search( COPEREG, 							
					OPEMODE_REG1,
					0,
					&(pv->var.OpeModeIdx),
					MCHK_REG_READ,
					Hmsg);
		if( rc != OK )
		{
			ResFlag = FALSE;
			return (rc);
		}
	}
/*--------------------------------------------------------------------------------------------------*/
	/* Search Register (Operation Mode Command Register : 42H：0x80004002/40H：0x2001) */
	if ( ResFlag == TRUE )
	{
		rc = COPEREG->Search( COPEREG,
					OPESET_REG1,
					0,
					&(pv->var.OpeSetIdx),
					MCHK_REG_READ,
					Hmsg);
		if( rc != OK )
		{
			ResFlag = FALSE;
			return (rc);
		}
	}

/* <S0FA> Start */
/*--------------------------------------------------------------------------------------------------*/
	/* Search Register (Monitor Register -Command Speed- => 42H：0x8001C002/40H：0xE001 */
	if ( ResFlag == TRUE )
	{
		rc = CUNREGX->Search( (COBJECT *)CUNREGX,
				MON_REG_CMD_SPD,
				0,
				&(pv->var.MonRegIdx),
				MCHK_REG_READ,
				Hmsg);
		if( rc != OK )
		{
			ResFlag = FALSE;
			return (rc);
		}
	}
/* <S0FA> End */
/*------------------------------------------------------------------------------------------------------------------*/
/* Set Analog Velocity Reference Offset Manual Adjust Mode (Fn00A) to Mode Register(42H：0x80004000/40H：0x2000)	*/
/*------------------------------------------------------------------------------------------------------------------*/
	if ( ResFlag == TRUE )
	{
		rc = OPEREG_WRITE( pv->var.OpeModeIdx, FNANLGVELREFMANADJ_MODE, Hmsg );
	}
/*-------------------------------------------------------------------------------------------------------------------*/
	return (rc);
}


/*****************************************************************************
  Description: Fn00A Analog Velocity Reference Offset Manual Adjustment End
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	none
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
void FunAnlgVelRefManAdjEnd(FUNMSG *Fmsg)
{
FNREFVELMANADJV  *pv = (FNREFVELMANADJV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;

/*--------------------------------------------------------------*/
/* Set Normal to Mode Register(42H：0x80004000/40H：0x2000)		*/
/*--------------------------------------------------------------*/
	OPEREG_WRITE( pv->var.OpeModeIdx, FNNORMAL_MODE, Hmsg );
}


/*************************************************************************************
  Description: Fn00A Analog Velocity Reference Offset Manual Adjustment Display Data
--------------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	none
--------------------------------------------------------------------------------------
  Note:
	
*************************************************************************************/
void FunAnlgVelRefManAdjustDisplay( FUNMSG *Fmsg )
{
	FNREFVELMANADJV *pv = (FNREFVELMANADJV *)(Fmsg->pVar);
	UCHAR *ResBuf = Fmsg->pTxBuf;
	MSGIF *Hmsg = Fmsg->Super;
	AXIS_HANDLE *pAxRsc = Hmsg->pAxRsc;
	LONG Zadjvl_Mon;

	/* Un001 : 速度指令モニタ [2^24/OvrSpd] */
//	Zadjvl_Mon = pAxRsc->UnMonitor->UnSpdRef;	/* <S0FA> */
/* <S0FA> Start */
	/* モニタレジスタ -指令速度(=Un001)- (42H：0x8001C002/40H：0xE001) 読出し */
	UNREG_READX( pv->var.MonRegIdx, (ULONG *)( &(pv->var.MonRegData) ), MCHK_REG_READ, Hmsg );
	Zadjvl_Mon = pv->var.MonRegData;
/* <S0FA> End */

	/* Contents */
	RpxLcdSetStr( 2U, 2U, (UCHAR *)"ZADJV=", ResBuf );
	RpxLcdSetStr( 3U, 2U, (UCHAR *)"Vref =", ResBuf );

	/* 調整値 */
	RpxLcdSetSignedDecData( 2U, 8U, pv->var.DispAdiData, 5U, (USHORT)FALSE, 0U, ResBuf );
	/* 速度指令モニタ値 */
	RpxLcdSetSignedDecData( 3U, 8U, Zadjvl_Mon, 5U, (USHORT)FALSE, 0U, ResBuf );

	RpxLcdClrLineBlnk( 4U, ResBuf );
	RpxLcdSetBlnk( 3U, 14U, 1U, ResBuf );

}

/*****************************************************************************
  Description: LCD Operator Fn00A Execution
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		FUN_CONTINUE: Execution Continue, FUN_END: Execution End
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
static LONG LpxAnlgVelRefManAdjExe(FUNMSG *Fmsg)
{
#if  SVFSEL_ADC_USE == 1										/* <S148> */
	FNREFVELMANADJV *pv = (FNREFVELMANADJV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
	AXIS_HANDLE *pAxRsc = Hmsg->pAxRsc;
	PRMDATA	*Prm = pAxRsc->UniPrms->Prm;
	REGIF_CONFIG_T	*RegCfg = pAxRsc->RegManager->hPnReg;
	CADC	*pAdc =  pAxRsc->pAdc;	/* <S0F9> */
	UCHAR *ResBuf = Fmsg->pTxBuf;
	OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
	BLINKFLG *pBlinkFlg = (BLINKFLG *)(Fmsg->pBlnkKick);
	LONG rc = FUN_CONTINUE;									/* 0: continue, 1: end	*/
	LONG Zadjvl_Tmp;

/*--------------------------------------------------------------------------------------------------*/
	/* clear Tx buffer */
	RpxLcdClrTxBuf( ResBuf );

	/* TITLE */
	RpxLcdSetStr( 1U, 1U, (UCHAR *)"-Velocity Adjust-", ResBuf );

/*--------------------------------------------------------------------------------------------------*/
/*		Analog Velocity Reference Offset Manual Adjustmen (Fn00A) Sequence							*/
/*--------------------------------------------------------------------------------------------------*/
		switch( pv->var.State )
		{
/*--------------------------------------------------------------------------------------------------*/
		case ANLG_VEL_REF_MAN_ADJ_IDLE: 											/* Idle State 	*/
/*--------------------------------------------------------------------------------------------------*/
				pv->var.State = ANLG_VEL_REF_MAN_ADJ_EXE;
			break;

/*--------------------------------------------------------------------------------------------------*/
		case ANLG_VEL_REF_MAN_ADJ_EXE: 												/* Exe state	*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = ANLG_VEL_REF_MAN_ADJ_END;
			}
			else if ( OpKeyMake.up == TRUE )
			{
				/* PnE53 : 速度指令ゼロ調の値読み込み */
				Zadjvl_Tmp = Prm->zadjvl;

				/* 表示データ更新 */
//				pv->var.DispAdiData ++;						/* <S142> */

				/* PnE53 : 速度指令ゼロ調 更新 */
				Zadjvl_Tmp += ZADJV_RATE;

				if( Zadjvl_Tmp > ZADJV_MAX )
				{
					Zadjvl_Tmp = ZADJV_MAX;
				}

				/*  PnE53 : 速度指令ゼロ調に更新した値をRAMへ書き込み */
				Prm->zadjvl = Zadjvl_Tmp;

				/* PnE53(速度指令ゼロ調)を速度指令ＡＤ値演算時に使用するZero Adjustに格納 */	/* <S0F9> */
				pAdc->P.ZadjVelo = Prm->zadjvl;		/* <S0F9> */

			}
			else if ( OpKeyMake.down == TRUE )
			{
				/* PnE53 : 速度指令ゼロ調の値読み込み */
				Zadjvl_Tmp = Prm->zadjvl;

				/* 表示データ更新 */
//				pv->var.DispAdiData --;						/* <S142> */

				Zadjvl_Tmp -= ZADJV_RATE;

				if( Zadjvl_Tmp < ZADJV_MIN )
				{
					Zadjvl_Tmp = ZADJV_MIN;
				}

				/*  PnE53 : 速度指令ゼロ調に更新した値をRAMへ書き込み */
				Prm->zadjvl = Zadjvl_Tmp;

				/* PnE53(速度指令ゼロ調)を速度指令ＡＤ値演算時に使用するZero Adjustに格納 */	/* <S0F9> */
				pAdc->P.ZadjVelo = Prm->zadjvl;		/* <S0F9> */

			}
			else if ( OpKeyMake.data == TRUE )
			{
				pBlinkFlg->done = TRUE;

				/*  PnE53 : 速度指令ゼロ調をEepRomへ書き込み */
				PrmStoreValueToEeprom( RegCfg, &pndef_zadjvl, 0 );
			}
			else
			{
				/*do nothing*/
			}

			break;
/*--------------------------------------------------------------------------------------------------*/
		case ANLG_VEL_REF_MAN_ADJ_END: 												/* End state	*/
/*--------------------------------------------------------------------------------------------------*/
			FunCurrentManAdjEnd( Fmsg );
			rc = FUN_END;
			break;
		default:
			break;
		}

/*--------------------------------------------------------------------------------------------------*/
/*		Make Display Data																			*/
/*--------------------------------------------------------------------------------------------------*/
		pv->var.DispAdiData = Prm->zadjvl / ZADJV_RATE;			/* <S142> */
		FunAnlgVelRefManAdjustDisplay( Fmsg );
	return (rc);
#else															/* <S148> */
	BLINKFLG 	*BlinkFlag	= (BLINKFLG *)(Fmsg->pBlnkKick);

		FunCurrentManAdjEnd( Fmsg );
		BlinkFlag->no_op	= TRUE;
		return( FUN_END );										/* <S148> */
#endif            /* <S09F> */
}


/*****************************************************************************
  Description: PANEL Operator Fn00A Execution
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		FUN_CONTINUE: Execution Continue, FUN_END: Execution End
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
#if  SVFSEL_PANELOP_USE == 1
static LONG LpxPnlAnlgVelRefManAdjExe(FUNMSG *Fmsg)
{
	FNREFVELMANADJV *pv = (FNREFVELMANADJV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
	AXIS_HANDLE *pAxRsc = Hmsg->pAxRsc;
	PRMDATA	*Prm = pAxRsc->UniPrms->Prm;
	REGIF_CONFIG_T	*RegCfg = pAxRsc->RegManager->hPnReg;
	CADC	*pAdc =  pAxRsc->pAdc;	/* <S0F9> */
	LEDDSP *DspData = (LEDDSP *)&(Fmsg->pTxBuf[PNLRSP_LEDCODE_IDX]);		/* set display data buffer	*/
	OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
	LONG rc = FUN_CONTINUE;													/* 0: continue, 1: end	*/
	LONG Zadjvl_Tmp;
	BLINKFLG *BlinkFlag = (BLINKFLG *)(Fmsg->pBlnkKick);					/* <S148> */



/*--------------------------------------------------------------------------------------------------*/
/*		CurManuAdj (Fn00F) Sequence																	*/
/*--------------------------------------------------------------------------------------------------*/
		switch( pv->var.State )
		{
/*--------------------------------------------------------------------------------------------------*/
		case ANLG_VEL_REF_MAN_ADJ_IDLE: 											/* Idle State 	*/
/*--------------------------------------------------------------------------------------------------*/
#if 0	/* <S148> */
				pv->var.State = ANLG_VEL_REF_MAN_ADJ_EXE;
#else
			//if( OpKeyMake.data == TRUE )
			if( OpKeyMake.mode == TRUE )			/* <S162>  */
			{
				pv->var.State = ANLG_VEL_REF_MAN_ADJ_END;
			}
			//else if ( OpKeyMake.mode == TRUE )
			else if ( OpKeyMake.data == TRUE )		/* <S162>  */
			{
				pv->var.State = ANLG_VEL_REF_MAN_ADJ_EXE;
			}
#endif
			break;

/*--------------------------------------------------------------------------------------------------*/
		case ANLG_VEL_REF_MAN_ADJ_EXE: 												/* Exe state	*/
/*--------------------------------------------------------------------------------------------------*/
//			if( OpKeyMake.data == TRUE )
//			{
//				pv->var.State = ANLG_VEL_REF_MAN_ADJ_END;
//			}
//			else if ( OpKeyMake.up == TRUE )
			if ( OpKeyMake.up == TRUE )				/* <S148> */
			{
				/* PnE53 : 速度指令ゼロ調の値読み込み */
				Zadjvl_Tmp = Prm->zadjvl;

				/* 表示データ更新 */
//				pv->var.DispAdiData ++;						/* <S142> */

				/* PnE53 : 速度指令ゼロ調 更新 */
				Zadjvl_Tmp += ZADJV_RATE;

				if( Zadjvl_Tmp > ZADJV_MAX )
				{
					Zadjvl_Tmp = ZADJV_MAX;
				}

				/*  PnE53 : 速度指令ゼロ調に更新した値をRAMへ書き込み */
				Prm->zadjvl = Zadjvl_Tmp;

				/* PnE53(速度指令ゼロ調)を速度指令ＡＤ値演算時に使用するZero Adjustに格納 */	/* <S0F9> */
				pAdc->P.ZadjVelo = Prm->zadjvl;	/* <S0F9> */

			}
			else if ( OpKeyMake.down == TRUE )
			{
				/* PnE53 : 速度指令ゼロ調の値読み込み */
				Zadjvl_Tmp = Prm->zadjvl;

				/* 表示データ更新 */
//				pv->var.DispAdiData --;						/* <S142> */

				Zadjvl_Tmp -= ZADJV_RATE;

				if( Zadjvl_Tmp < ZADJV_MIN )
				{
					Zadjvl_Tmp = ZADJV_MIN;
				}

				/*  PnE53 : 速度指令ゼロ調に更新した値をRAMへ書き込み */
				Prm->zadjvl = Zadjvl_Tmp;

				/* PnE53(速度指令ゼロ調)を速度指令ＡＤ値演算時に使用するZero Adjustに格納 */	/* <S0F9> */
				pAdc->P.ZadjVelo = Prm->zadjvl;	/* <S0F9> */

			}
#if 0	/* <S148> */
			else if ( OpKeyMake.mode == TRUE )
			{
				/*  PnE53 : 速度指令ゼロ調をEepRomへ書き込み */
				PrmStoreValueToEeprom( RegCfg, &pndef_zadjvl, 0 );
			}
			else
			{
				/*do nothing*/
			}
#else
			//else if(( OpKeyMake.mode == TRUE )||( OpKeyMake.shup == TRUE ))
			else if( OpKeyMake.mode == TRUE )									/* <S162>  */
			{
				/*  PnE53 : 速度指令ゼロ調をEepRomへ書き込み */
				if( PrmStoreValueToEeprom( RegCfg, &pndef_zadjvl, 0 ) == 0 )
				{
					BlinkFlag->done 	= TRUE;
				}
				else
				{
					BlinkFlag->error 	= TRUE;
				}
				//pv->var.State = ANLG_VEL_REF_MAN_ADJ_IDLE;
				pv->var.State = ANLG_VEL_REF_MAN_ADJ_EXIT;		/* <S162>  */
			}
#endif
			break;
/*--------------------------------------------------------------------------------------------------*/
		case ANLG_VEL_REF_MAN_ADJ_EXIT:						/* <S162>  */
			if( OpKeyMake.data == TRUE )
			{
				pv->var.State = ANLG_VEL_REF_MAN_ADJ_END;
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case ANLG_VEL_REF_MAN_ADJ_END: 												/* End state		*/
/*--------------------------------------------------------------------------------------------------*/
			FunCurrentManAdjEnd( Fmsg );
			rc = FUN_END;
			break;
		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Make Display Data																			*/
/*--------------------------------------------------------------------------------------------------*/
		/* パネル表示データ作成	*/
#if 0	/* <S148> */
		pv->var.DispAdiData = Prm->zadjvl / ZADJV_RATE;			/* <S142> */
		RpxLedSetStr( (UCHAR *)"  SPd", DspData, 4U, 5U );
		RpxLedSetStr( (UCHAR *)".", DspData, 4U, 1U );
		RpxLedDispMonDecWord( (ULONG)( pv->var.DispAdiData & 0xFFFFFFFF ), DspData, 0U, DSPDEF_W_SIGN );
#else
		//if( pv->var.State == ANLG_VEL_REF_MAN_ADJ_IDLE )
		if(( pv->var.State == ANLG_VEL_REF_MAN_ADJ_IDLE )||( pv->var.State == ANLG_VEL_REF_MAN_ADJ_EXIT ))	/* <S162>  */
		{
			RpxLedSetStr( (UCHAR *)"  SPd", DspData, 4U, 5U );
			RpxLedSetStr( (UCHAR *)".", DspData, 4U, 1U );
			RpxLedSetStatusBits( DspData, Hmsg );
		}
		else if( pv->var.State == ANLG_VEL_REF_MAN_ADJ_EXE )
		{
			pv->var.DispAdiData = Prm->zadjvl / ZADJV_RATE;
			RpxLedDispMonDecWord( (ULONG)( pv->var.DispAdiData & 0xFFFFFFFF ), DspData, 0U, DSPDEF_W_SIGN );
		}
#endif
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}
#endif
/***************************************** end of file **********************************************/
