/****************************************************************************************************/
/*																									*/
/*																									*/
/*		AnlgRefAutoAdj.c : Analog Reference Offset Auto Adjust (Fn009) Sample Source				*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	Function:																						*/
/*			1)Fn009 Analog Reference Offset Auto Adjustment Mode Main								*/
/*			2)Fn009 Analog Reference Offset Auto Adjustment Mode Begin								*/
/*			3)Fn009 Analog Reference Offset Auto Adjustment Mode End								*/
/*			4)Fn009 Analog Reference Offset Auto Adjustment Mode Execution(Digital Operator)		*/
/*			5)Fn009 Analog Reference Offset Auto Adjustment (Panel Operator)						*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.1.00 : 2013.09.11  K.Ozaki	<S00C>														*/
/*		Rev.1.01 : 2013.10.23  K.Ozaki	<S031> Remove Compile Warning								*/
/*		Rev.1.02 : 2013.12.22  T.Masuo	<S0A9> Remake For LCD Operator & Panel Operator				*/
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
/* Fn009 Mode Variable */
typedef struct _FnRefAutoAdjV {
	struct {
		INT State;						/* Analog Reference Auto Adjustment (Fn009) Execution State */
		LONG OpeModeIdx;				/* Operation Mode Register Index */
		LONG OpeSetIdx;					/* Operation Set Register Index */
		ULONG ResData;					/* Response Data */
		ULONG DispTime;
	}var;
	LONG Reserved[(256/4)-5];			/* Reserved: All functions can use ram variables at least 256 bytes. */
} FNREFAUTOADJV;

/****************************************************************************************************/
/*																									*/
/*		Constant Definition																			*/
/*																									*/
/****************************************************************************************************/
#define OK		(0x0000)
#define NG		(0x0001)

/*--------------------------------------------------------------------------------------------------*/
/* Operation Register 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define OPEMODE_REG1	(0x0000)	/* 1st Operation Mode Register(42H:0x80004000/40H:0x2000)		*/
#define OPESET_REG1		(0x0001)	/* 1st Operation Mode Set Register(42H:0x80004002/40H:0x2001)	*/

/*--------------------------------------------------------------------------------------------------*/
/* Operation Mode	 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define FNNORMAL_MODE				(0x0000U) 		/* Normal Mode									*/
#define FNANLGREFAUTOADJ_MODE		(0x1009U)		/* Analog Reference Offset Auto Adjustment Mode	*/


/*--------------------------------------------------------------------------------------------------*/
/* Operation Command 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define CMD_END 		(0x0)
#define CMD_EXE			(0x1)

/*--------------------------------------------------------------------------------------------------*/
#define FUN_CONTINUE			(0L)			/*Continue	*/
#define FUN_END					(1L)			/*End		*/

/*--------------------------------------------------------------------------------------------------*/
#define OPEREG_WRITE( idx, data, hmsg )   (COPEREG->Write( COPEREG, (idx), (data), hmsg ))
#define PRMREG_WRITE( idx, data, hmsg )   (CPARAMX->Write( CPARAMX, (idx), (data), hmsg ))
#define STSREG_READX( idx, pResData, CheckType, hmsg )   (COPEREG->Read( COPEREG, (idx), (pResData), \
																		(CheckType), hmsg ))

enum _enum_Anlg_Ref_Auto_Ref_State {
	ANLG_REF_AUTO_ADJ_IDLE = 0,
	ANLG_REF_AUTO_ADJ_EXE,
	ANLG_REF_AUTO_ADJ_END,
};

/********************************************************************************************************/
/*																										*/
/*		Function List																					*/
/*																										*/
/********************************************************************************************************/
/*------------------------------------------------------------------------------------------------------*/
/*		Public Function																					*/
/*------------------------------------------------------------------------------------------------------*/
LONG FunAnlgRefAutoAdjBegin(FUNMSG *Fmsg);	/* Fn009 Analog Reference Offset Auto Adjustment Mode Begin	*/
LONG FunAnlgRefAutoAdjMain(FUNMSG *Fmsg);	/* Fn009 Analog Reference Offset Auto Adjustment Mode Display*/
void FunAnlgRefAutoAdjEnd(FUNMSG *Fmsg);	/* Fn009 Analog Reference Offset Auto Adjustment Mode End	*/

/*------------------------------------------------------------------------------------------------------*/
/*		Private Function																				*/
/*------------------------------------------------------------------------------------------------------*/
static LONG LpxAnlgRefAutoAdjExe(FUNMSG *Fmsg);		/* Fn009 Execution (LCD Operator)					*/

#if  SVFSEL_PANELOP_USE != 1
#define LpxPnlAnlgRefAutoAdjExe( x ) (1)
#else
LONG LpxPnlAnlgRefAutoAdjExe(FUNMSG *hMsg);			/* Fn009 Execution (Panel Operator) 				*/
#endif


/*****************************************************************************
  Description: Fn009 Analog Reference Offset Auto Adjustment Mode Main
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	LONG			0: continue, 1: end 
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
LONG FunAnlgRefAutoAdjMain(FUNMSG *Fmsg)
{
	MSGIF *hMsg = Fmsg->Super;
	LONG rc = FUN_CONTINUE;

/*--------------------------------------------------------------------------------------------------*/
	if( KPI_CHK_LopHmsg( hMsg ) )							/* LCD Operator */
	{
		return LpxAnlgRefAutoAdjExe( Fmsg );
	} 
	else if ( KPI_CHK_PopHmsg( hMsg ) )						/* Panel Operator */
	{
		return LpxPnlAnlgRefAutoAdjExe( Fmsg );
	}
	else
	{
		/* Do Nothing */
	}

	return (rc);
}

/*****************************************************************************
  Description: Fn009 Analog Reference Offset Auto Adjustment Mode Begin
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	LONG			OK: Success, Others: Failure 
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
LONG FunAnlgRefAutoAdjBegin(FUNMSG *Fmsg)
{
	LONG	rc = OK;
	BOOL ResFlag = TRUE;

	FNREFAUTOADJV  *pv = (FNREFAUTOADJV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;

/*--------------------------------------------------------------------------------------------------*/
	pv->var.State = ANLG_REF_AUTO_ADJ_IDLE;

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
/*------------------------------------------------------------------------------------------------------*/
/* Set Analog Reference Offset Auto Adjust Mode(Fn009) to Mode Register(42H：0x80004000/40H：0x2000)	*/
/*------------------------------------------------------------------------------------------------------*/
	if ( ResFlag == TRUE )
	{

			rc = OPEREG_WRITE( pv->var.OpeModeIdx, FNANLGREFAUTOADJ_MODE, Hmsg );
	}
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
	KpiRstLongTimer( &pv->var.DispTime );						/* Panel Operator 					*/
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}

/*****************************************************************************
  Description: Fn009 Analog Reference Offset Auto Adjustment Mode End
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	None
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
void FunAnlgRefAutoAdjEnd(FUNMSG *Fmsg)
{
	FNREFAUTOADJV  *pv = (FNREFAUTOADJV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;

/*--------------------------------------------------------------*/
/* Set Normal to Mode Register(42H：0x80004000/40H：0x2000)		*/
/*--------------------------------------------------------------*/
	OPEREG_WRITE( pv->var.OpeModeIdx, FNNORMAL_MODE, Hmsg );
}

/*******************************************************************************************
  Description: Fn009 Analog Reference Offset Auto Adjustment Mode Execution(Digital Operator)
--------------------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	LONG			FUN_CONTINUE: Execution Continue, FUN_END: Execution End
--------------------------------------------------------------------------------------------
  Note:

********************************************************************************************/
static LONG LpxAnlgRefAutoAdjExe(FUNMSG *Fmsg)
{
#if  SVFSEL_ADC_USE == 1            /* <S09F> */
	FNREFAUTOADJV *pv = (FNREFAUTOADJV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
	UCHAR *ResBuf = Fmsg->pTxBuf;
	OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
	BLINKFLG *BlinkFlag = (BLINKFLG *)(Fmsg->pBlnkKick);
	LONG rc = FUN_CONTINUE;													/* 0: continue, 1: end */
	AXIS_HANDLE *pAxRsc = Hmsg->pAxRsc;
	SEQ_CTRL_OUT	*SeqCtrlOut;
	BASE_CTRL_HNDL		*BaseControls;
	ASIC_HW_IF_STR	*AsicHwReg;	/* <S0FC> */
	FUN_MANAGER *fnp;
	fnp = pAxRsc->FnManager;

	SeqCtrlOut = pAxRsc->SeqCtrlOut;
	BaseControls = pAxRsc->BaseControls;
	AsicHwReg = pAxRsc->SvAsicRegs->AsicHwReg;	/* <S0FC> */

/*--------------------------------------------------------------------------------------------------*/
		RpxLcdClrTxBuf( ResBuf ); 								/* clear Tx buffer */
		/* set title */
		RpxLcdSetStr( 1U, 1U, (UCHAR *)"Ref Adjust", ResBuf );
		RpxLcdSetStr( 2U, 1U, (UCHAR *)"of Motor Current", ResBuf );
		/* Contents */
		RpxLcdSetStr( 3U, 2U, (UCHAR *)"Start :[DATA]", ResBuf );
		RpxLcdSetStr( 4U, 2U, (UCHAR *)"Return:[SET]", ResBuf );

/*--------------------------------------------------------------------------------------------------*/
/*		Motor Current Offset Auto Adjustment (Fn00E) Sequence										*/
/*--------------------------------------------------------------------------------------------------*/
		switch( pv->var.State )
		{
/*--------------------------------------------------------------------------------------------------*/
		case ANLG_REF_AUTO_ADJ_IDLE: 										/* Idle State			*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = ANLG_REF_AUTO_ADJ_END;
			}
			else if( OpKeyMake.data == TRUE )
			{
				/* 実行条件 */
//				if( (SeqCtrlOut->MainPowerOn == FALSE)	/* <S0FC> */
//					|| (SeqCtrlOut->HwbbSts != FALSE)	/* <S0FC> */
//					|| (SeqCtrlOut->MotStop == FALSE)	/* <S0FC> */
//					|| (BaseControls->BeComplete == TRUE) )	/* <S0FC> */
				if( SHalDrv_GetFaultStatus(AsicHwReg, SHAL_FAULT_BB) == 0 )		/* Base Enable */	/* <S0FC> */
				{
					BlinkFlag->no_op = TRUE;
				}
				/* 実行(操作設定レジスタに0x0001を書き込み) */
				else
				{
				pv->var.State = ANLG_REF_AUTO_ADJ_EXE;
					FnManagerSto = *fnp;
					if ( OPEREG_WRITE( pv->var.OpeSetIdx, CMD_EXE, Hmsg ) == OK )
					{
						BlinkFlag->done = TRUE;
					}
					else
					{
						BlinkFlag->error = TRUE;
					}
				}
			}
			else
			{
				/*do nothing	*/
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case ANLG_REF_AUTO_ADJ_EXE: 										/* 	Exec State			*/
/*--------------------------------------------------------------------------------------------------*/
			/* 操作設定レジスタ(42H：0x80004002/40H：0x2001)の値取得 */
			STSREG_READX( pv->var.OpeModeIdx, &(pv->var.ResData), MCHK_REG_READ, Hmsg );

			if( ( pv->var.ResData )  == FNNORMAL_MODE)	/* 操作モード = ノーマルモード */
			{
				if(*((UINT *)&OpBlnkFlg) != 0) break;
				*fnp = FnManagerSto;
				pv->var.State = ANLG_REF_AUTO_ADJ_IDLE;
			}
			break;

/*--------------------------------------------------------------------------------------------------*/
		case ANLG_REF_AUTO_ADJ_END: 										/* 	End State			*/
/*--------------------------------------------------------------------------------------------------*/
			FunCurrentAutoAdjEnd( Fmsg );
			fnp->cFunMode[FUN_CH1] = NULL;
			fnp->FunExeState[FUN_CH1].CmdCode = FCMD_NONE;
			fnp->FunExeState[FUN_CH1].State = 0;
			fnp->AxCommon->modeLock = FALSE;
			rc = FUN_END;
			break;
		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
		if ( pv->var.State == ANLG_REF_AUTO_ADJ_EXE )
		{
			RpxLcdSetBlnk( 1U, 0U, 15U, ResBuf );
			RpxLcdSetBlnk( 2U, 0U, 17U, ResBuf );
		}
		else
		{
			RpxLcdClrLineBlnk( 1U, ResBuf );
			RpxLcdClrLineBlnk( 2U, ResBuf );
		}
/*--------------------------------------------------------------------------------------------------*/

	return (rc);
#else												/* <S148> */
	BLINKFLG 	*BlinkFlag	= (BLINKFLG *)(Fmsg->pBlnkKick);

		FunCurrentAutoAdjEnd( Fmsg );
		BlinkFlag->no_op					= TRUE;
		return( FUN_END );							/* <S148> */
#endif            /* <S09F> */
}

/*****************************************************************************
  Description: Fn009 Analog Reference Offset Auto Adjustment (Panel Operator)
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	LONG			FUN_CONTINUE: Execution Continue, FUN_END: Execution End
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
#if SVFSEL_PANELOP_USE == 1
static LONG LpxPnlAnlgRefAutoAdjExe(FUNMSG *Fmsg)
{
	FNREFAUTOADJV *pv = (FNREFAUTOADJV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;											/* message interface */
	LEDDSP *DspData = (LEDDSP *)&(Fmsg->pTxBuf[PNLRSP_LEDCODE_IDX]);	/* set display data buffer */
	OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);					/* get key make flag */
	BLINKFLG *BlinkFlag = (BLINKFLG *)(Fmsg->pBlnkKick);
	LONG rc = FUN_CONTINUE;												/* 0: continue, 1: end */
	AXIS_HANDLE *pAxRsc = Hmsg->pAxRsc;
	SEQ_CTRL_OUT	*SeqCtrlOut;
	BASE_CTRL_HNDL		*BaseControls;

	FUN_MANAGER *fnp;
	fnp = pAxRsc->FnManager;

	SeqCtrlOut = pAxRsc->SeqCtrlOut;
	BaseControls = pAxRsc->BaseControls;


/*--------------------------------------------------------------------------------------------------*/
/*		Motor Current Offset Auto Adjustment (Fn00E) Sequence										*/
/*--------------------------------------------------------------------------------------------------*/
		switch( pv->var.State )
		{
/*--------------------------------------------------------------------------------------------------*/
		case ANLG_REF_AUTO_ADJ_IDLE: 										/* Idle State			*/
/*--------------------------------------------------------------------------------------------------*/

			if( OpKeyMake.data == TRUE )
			{
				pv->var.State = ANLG_REF_AUTO_ADJ_END;
			}
			else if( OpKeyMake.mode == TRUE )
			{
				if( (SeqCtrlOut->MainPowerOn == FALSE)
					|| (SeqCtrlOut->HwbbSts != FALSE)
					|| (SeqCtrlOut->MotStop == FALSE)
					|| (BaseControls->BeComplete == TRUE) )
				{
					BlinkFlag->no_op = TRUE;
			}
			else
			{
					pv->var.State = ANLG_REF_AUTO_ADJ_EXE;
					FnManagerSto = *fnp;
					if ( OPEREG_WRITE( pv->var.OpeSetIdx, CMD_EXE, Hmsg ) == OK )
					{
						BlinkFlag->done = TRUE;
					}
					else
					{
						BlinkFlag->error = TRUE;
					}
				}
			}
			else
			{
				/*do nothing	*/
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case ANLG_REF_AUTO_ADJ_EXE: 										/* 	Exec State			*/
/*--------------------------------------------------------------------------------------------------*/
			STSREG_READX( pv->var.OpeModeIdx, &(pv->var.ResData), MCHK_REG_READ, Hmsg );
			if( ( pv->var.ResData )  == FNNORMAL_MODE)
			{
				if( *((UINT *)&OpBlnkFlg ) != 0) break;
				*fnp = FnManagerSto;
				pv->var.State = ANLG_REF_AUTO_ADJ_IDLE;
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case ANLG_REF_AUTO_ADJ_END: 										/* 	End State			*/
/*--------------------------------------------------------------------------------------------------*/
			FunCurrentAutoAdjEnd( Fmsg );
			fnp->cFunMode[FUN_CH1] = NULL;
			fnp->FunExeState[FUN_CH1].CmdCode = FCMD_NONE;
			fnp->FunExeState[FUN_CH1].State = 0;
			fnp->AxCommon->modeLock = FALSE;
			rc = FUN_END;
			break;
		default:
			break;
		}

		/* ALL Clear	*/
		MlibResetByteMemory( DspData, sizeof(LEDDSP) );	 					/* clear Tx buffer		*/

		/* led = "REF_o" */
		RpxLedSetStr((UCHAR *)"REF_o", DspData, 4U, 5U);

		/* Hard-wired Baseblock Display <V068> */
		if( CHK_HBB() /* CoutV.f.Sv.HwbbState */ )
		{
			if(((KlibGetLongTimerMs(pv->var.DispTime)/100) & 0x04) == 0 )
			{
				RpxLedSetStr((UCHAR *)". Hbb", DspData, 4U, 5U);
			}
		}

	return (rc);
}

#endif
/***************************************** end of file **********************************************/
