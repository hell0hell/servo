/****************************************************************************************************/
/*																									*/
/*																									*/
/*		AnlgTrqRefManAdj.c : Analog Torque Reference Offset Manual Adjustment (Fn00B)				*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	Function:																						*/
/*			1) Fn00B Analog Torque Reference Offset Manual Adjustment Mode Begin					*/
/*			2) Fn00B Analog Torque Reference Offset Manual Adjustment Main							*/
/*			3) Fn00B Analog Torque Reference Offset Manual Adjustment End							*/
/*			4) Fn00B Analog Torque Reference Offset Manual Adjustment Display Data					*/
/*			5) Fn00B Execution (LCD Operator)														*/
/*			6) Fn00B Execution (Panel Operator) 													*/
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
typedef struct _FnRefTrqManAdjV {
	struct{
		INT  State;						/* Torque Reference Offset Manual Adjustment Execution State */
		LONG OpeModeIdx;				/* Operation Mode Register Index */
		LONG OpeSetIdx;					/* Operation Set Register Index */
		LONG DispAdiData;				/* Display Data */
		LONG MonRegIdx;					/* Monitor Register Index */		/* <S0FA> */
		LONG MonRegData;				/* Monitor Register Read Data */	/* <S0FA> */
//		LONG Reserved[(256/4)-4]; 		/* Reserved: All functions can use ram variables at least 256 bytes. */	/* <S0FA> */
		LONG Reserved[(256/4)-6]; 		/* Reserved: All functions can use ram variables at least 256 bytes. */	/* <S0FA> */
	}var;
} FNREFTRQMANADJV;

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
#define FNANLGTRQREFMANADJ_MODE		(0x100BU)	/* Fn00B : Torque Reference Offset Manual Adjustment*/

/*--------------------------------------------------------------------------------------------------*/
/* Monitor Register 																	<S0FA>		*/
/*--------------------------------------------------------------------------------------------------*/
#define MON_REG_CMD_TRQ	(0x0002)	/* Monitor Register -Command Torque-(42H:0x8001C004/40H:0xE002)	*/

/*--------------------------------------------------------------------------------------------------*/
/* Minimum & Maximum Values 																		*/
/*--------------------------------------------------------------------------------------------------*/
#define	ZADJT_MAX			(127)
#define	ZADJT_MIN			(-127)

/*--------------------------------------------------------------------------------------------------*/
#define FUN_CONTINUE			(0L)			/*Continue	*/
#define FUN_END					(1L)			/*End		*/

/*--------------------------------------------------------------------------------------------------*/
#define OPEREG_WRITE( idx, data, hmsg )   (COPEREG->Write( COPEREG, (idx), (data), hmsg ))
#define UNREG_WRITE( idx, data, hmsg )   (CUNREGX->Write( (COBJECT *)CUNREGX, (idx), (data), hmsg ))
#define UNREG_READX( idx, pResData, CheckType, hmsg )    (CUNREGX->Read( (COBJECT *)(CUNREGX), (idx),\
															 	 	 (pResData), (CheckType), hmsg ))


/* Fn00B : Analog Torque ReferenceOffset Manual Adjust */
enum _enum_Anlg_Trq_Ref_Man_Ref_State {
	ANLG_TRQ_REF_MAN_ADJ_IDLE = 0,
	ANLG_TRQ_REF_MAN_ADJ_EXE,
	ANLG_TRQ_REF_MAN_ADJ_END,
	ANLG_TRQ_REF_MAN_ADJ_EXIT,			/* <S162>  */
};

/************************************************************************************************************************/
/*																														*/
/*		Function List																									*/
/*																														*/
/************************************************************************************************************************/
/*----------------------------------------------------------------------------------------------------------------------*/
/*		Public Function																									*/
/*----------------------------------------------------------------------------------------------------------------------*/
LONG FunAnlgTrqRefManAdjBegin(FUNMSG *Fmsg);		/* Fn00B Analog Torque Reference Offset Manual Adjust Mode Begin	*/
LONG FunAnlgTrqRefManAdjMain(FUNMSG *Fmsg);			/* Fn00B Analog Torque Reference Offset Manual Adjust Mode Main		*/
void FunAnlgTrqRefManAdjEnd(FUNMSG *Fmsg);			/* Fn00B Analog Torque Reference Offset Manual Adjust Mode End		*/
void FunAnlgTrqRefManAdjustDisplay( FUNMSG *Fmsg );	/* Fn00B Analog Torque Reference Offset Manual Adjust Display Data	*/


/*--------------------------------------------------------------------------------------------------*/
/*		Private Function																			*/
/*--------------------------------------------------------------------------------------------------*/
static LONG LpxAnlgTrqRefManAdjExe(FUNMSG *Fmsg);		/* Fn00B Execution (LCD Operator)			*/

#if  SVFSEL_PANELOP_USE != 1
#define LpxPnlAnlgTrqRefManAdjExe( x ) (1)
#else
LONG LpxPnlAnlgTrqRefManAdjExe(FUNMSG *hMsg);			/* Fn00B Execution (Panel Operator)			*/
#endif


/*****************************************************************************
  Description: Fn00B Analog Torque Reference Offset Manual Adjustment Main
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		0: continue, 1: end
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
LONG FunAnlgTrqRefManAdjMain(FUNMSG *Fmsg)
{
	MSGIF *hMsg = Fmsg->Super;
	LONG rc;
	rc = FUN_CONTINUE;
/*--------------------------------------------------------------------------*/
	if( KPI_CHK_LopHmsg( hMsg ) == TRUE )					/* LCD Operator */
	{
		rc = LpxAnlgTrqRefManAdjExe( Fmsg );
	}
/*--------------------------------------------------------------------------*/
	else if ( KPI_CHK_PopHmsg( hMsg ) == TRUE )				/*Panel Operator*/
	{
		rc = LpxPnlAnlgTrqRefManAdjExe( Fmsg );
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
  Description:  Fn00B Analog Torque Reference Offset Manual Adjustment Begin
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		OK: Success, Others: Failure
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
LONG FunAnlgTrqRefManAdjBegin(FUNMSG *Fmsg)
{
LONG	rc = OK;
BOOL ResFlag = TRUE;
FNREFTRQMANADJV  *pv = (FNREFTRQMANADJV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	pv->var.State = ANLG_TRQ_REF_MAN_ADJ_IDLE;

/*--------------------------------------------------------------------------------------------------*/
/*	Search Register																					*/
/*--------------------------------------------------------------------------------------------------*/

	/* Search Register (Operation Mode Register : 42H�F0x80004000/40H�F0x2000 (= Nomal Mode ) */
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
	/* Search Register (Operation Mode Command Register : 42H�F0x80004002/40H�F0x2001) */
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
	/* Search Register (Monitor Register -Command Torque- => 42H�F0x8001C004/40H�F0xE002 */
	if ( ResFlag == TRUE )
	{
		rc = CUNREGX->Search( (COBJECT *)CUNREGX,
				MON_REG_CMD_TRQ,
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
/*--------------------------------------------------------------------------------------------------------------*/
/* Set Analog Torque Reference Offset Manual Adjust Mode (Fn00B) to Mode Register(42H�F0x80004000/40H�F0x2000)	*/
/*--------------------------------------------------------------------------------------------------------------*/
	if ( ResFlag == TRUE )
	{
		rc = OPEREG_WRITE( pv->var.OpeModeIdx, FNANLGTRQREFMANADJ_MODE, Hmsg );
	}
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}


/*****************************************************************************
  Description:  Fn00B Analog Torque Reference Offset Manual Adjustment
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	none
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
void FunAnlgTrqRefManAdjEnd(FUNMSG *Fmsg)
{
FNREFTRQMANADJV  *pv = (FNREFTRQMANADJV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;

/*--------------------------------------------------------------*/
/* Set Normal to Mode Register(42H�F0x80004000/40H�F0x2000)		*/
/*--------------------------------------------------------------*/
	OPEREG_WRITE( pv->var.OpeModeIdx, FNNORMAL_MODE, Hmsg );
}


/*****************************************************************************
  Description: Fn00B Current Offset Manual Adjustment Display Data
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	none
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
void FunAnlgTrqRefManAdjustDisplay( FUNMSG *Fmsg )
{
	FNREFTRQMANADJV *pv = (FNREFTRQMANADJV *)(Fmsg->pVar);
	UCHAR *ResBuf = Fmsg->pTxBuf;
	MSGIF *Hmsg = Fmsg->Super;
	AXIS_HANDLE *pAxRsc = Hmsg->pAxRsc;
	LONG Zadjtp_Mon;

	/* Un002 : �g���N�w�߃��j�^ */
//	Zadjtp_Mon = pAxRsc->UnMonitor->UnTrqRef; /* <S0FA> */
/* <S0FA> Start */
	/* ���j�^���W�X�^ -�����g���N/���͎w��(Un002)- (42H�F0x8001C004/40H�F0xE002) �Ǐo�� */
	UNREG_READX( pv->var.MonRegIdx, (ULONG *)( &(pv->var.MonRegData) ), MCHK_REG_READ, Hmsg );
	Zadjtp_Mon = pv->var.MonRegData;
/* <S0FA> End */

	/* Contents */
	RpxLcdSetStr( 2U, 2U, (UCHAR *)"ZADJT=", ResBuf );
	RpxLcdSetStr( 3U, 2U, (UCHAR *)"Tref =", ResBuf );

	/* �����l */
	RpxLcdSetSignedDecData( 2U, 8U, pv->var.DispAdiData, 5U, (USHORT)FALSE, 0U, ResBuf );
	/* �g���N�w�߃��j�^�l */
	RpxLcdSetSignedDecData( 3U, 8U, Zadjtp_Mon, 5U, (USHORT)FALSE, 0U, ResBuf );

	RpxLcdClrLineBlnk( 4U, ResBuf );
	RpxLcdSetBlnk( 3U, 14U, 1U, ResBuf );

}


/*****************************************************************************
  Description: LCD Operator Fn00B Execution
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		FUN_CONTINUE: Execution Continue, FUN_END: Execution End
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
static LONG LpxAnlgTrqRefManAdjExe(FUNMSG *Fmsg)
{
#if  SVFSEL_ADC_USE == 1										/* <S148> */
	FNREFTRQMANADJV *pv = (FNREFTRQMANADJV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
	AXIS_HANDLE *pAxRsc = Hmsg->pAxRsc;
	PRMDATA	*Prm = pAxRsc->UniPrms->Prm;
	REGIF_CONFIG_T	*RegCfg = pAxRsc->RegManager->hPnReg;
	UCHAR *ResBuf = Fmsg->pTxBuf;
	OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
	BLINKFLG *pBlinkFlg = (BLINKFLG *)(Fmsg->pBlnkKick);
	LONG rc = FUN_CONTINUE;									/* 0: continue, 1: end	*/
//	SHORT Zadjtr_Tmp;
	DBYTEX			zadjtp_n_tmp;	/* <S142> */
	CADC *pAdc = pAxRsc->pAdc;		/* <S170> */

/*--------------------------------------------------------------------------------------------------*/
	/* clear Tx buffer */
	RpxLcdClrTxBuf( ResBuf );

	/* TITLE */
	RpxLcdSetStr( 1U, 1U, (UCHAR *)"-Torque Adjust-", ResBuf );


/*--------------------------------------------------------------------------------------------------*/
/*		Analog Torque Reference Offset Manual Adjustmen (Fn00B) Sequence							*/
/*--------------------------------------------------------------------------------------------------*/
		switch( pv->var.State )
		{
/*--------------------------------------------------------------------------------------------------*/
		case ANLG_TRQ_REF_MAN_ADJ_IDLE: 											/* Idle State 	*/
/*--------------------------------------------------------------------------------------------------*/
				pv->var.State = ANLG_TRQ_REF_MAN_ADJ_EXE;
			break;

/*--------------------------------------------------------------------------------------------------*/
		case ANLG_TRQ_REF_MAN_ADJ_EXE: 												/* Exe state	*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = ANLG_TRQ_REF_MAN_ADJ_END;
			}
			else if ( OpKeyMake.up == TRUE )
			{
				/* PnE56 : �g���N�w�߃[����(������/������)�̒l�ǂݍ��� */
//				Zadjtr_Tmp = (SHORT)((Prm->zadjtp_n.w)& 0xFFFF);
				zadjtp_n_tmp.w = (SHORT)((Prm->zadjtp_n.w)& 0xFFFF);	/* <S142> */

				/* �\���f�[�^�X�V */
//				pv->var.DispAdiData ++;				/* <S142> */

				/* PnE56 : �g���N�w�߃[����(������/������) �X�V */
#if 0	/* <S142> */
				if( Zadjtr_Tmp < ZADJT_MAX )
				{
					Zadjtr_Tmp += 1;

				}
				
				/* PnE56 : �g���N�w�߃[����(������/������)�ɍX�V�����l���������� */
				Prm->zadjtp_n.w = Zadjtr_Tmp;
#else
				if( ((CHAR)zadjtp_n_tmp.b.l < ZADJT_MAX) && ((CHAR)zadjtp_n_tmp.b.h < ZADJT_MAX) )
				{
					(CHAR)zadjtp_n_tmp.b.l++;
					(CHAR)zadjtp_n_tmp.b.h++;
				}

				/*  PnE56 : �g���N�w�߃[����(������/������)�ɍX�V�����l���������� */
				Prm->zadjtp_n.w = zadjtp_n_tmp.w;
				pAdc->P.ZadjTrqP = (CHAR)zadjtp_n_tmp.b.l;				/* <S170> */
				pAdc->P.ZadjTrqN = (CHAR)zadjtp_n_tmp.b.h;				/* <S170> */
#endif
			}
			else if ( OpKeyMake.down == TRUE )
			{
				/* PnE56 : �g���N�w�߃[����(������/������)�̒l�ǂݍ��� */
//				Zadjtr_Tmp = (SHORT)((Prm->zadjtp_n.w)& 0xFFFF);
				zadjtp_n_tmp.w = (SHORT)((Prm->zadjtp_n.w)& 0xFFFF);	/* <S142> */

				/* �\���f�[�^�X�V */
//				pv->var.DispAdiData --;				�E/* <S142> */

				/* PnE56 : �g���N�w�߃[����(������/������) �X�V */
#if 0	/* <S142> */
				if( Zadjtr_Tmp > ZADJT_MIN )
				{
					Zadjtr_Tmp -= 1;
				}

				/* PnE56 : �g���N�w�߃[����(������/������)�ɍX�V�����l���������� */
				Prm->zadjtp_n.w = Zadjtr_Tmp;
#else
				if( ((CHAR)zadjtp_n_tmp.b.l > ZADJT_MIN) && ((CHAR)zadjtp_n_tmp.b.h > ZADJT_MIN) )
				{
					(CHAR)zadjtp_n_tmp.b.l--;
					(CHAR)zadjtp_n_tmp.b.h--;
				}

				/*  PnE56 : �g���N�w�߃[����(������/������)�ɍX�V�����l���������� */
				Prm->zadjtp_n.w = zadjtp_n_tmp.w;
				pAdc->P.ZadjTrqP = (CHAR)zadjtp_n_tmp.b.l;				/* <S170> */
				pAdc->P.ZadjTrqN = (CHAR)zadjtp_n_tmp.b.h;				/* <S170> */
#endif
			}
			else if ( OpKeyMake.data == TRUE )
			{
				pBlinkFlg->done = TRUE;

				/* PnE56 : �g���N�w�߃[����(������/������)��EepRom�֏������� */
				PrmStoreValueToEeprom( RegCfg, &pndef_zadjtp_n, 0 );

			}
			else
			{
				/*do nothing*/
			}

			break;
/*--------------------------------------------------------------------------------------------------*/
		case ANLG_TRQ_REF_MAN_ADJ_END: 												/* End state	*/
/*--------------------------------------------------------------------------------------------------*/
			FunCurrentManAdjEnd( Fmsg );
			rc = FUN_END;
			break;
		default:
			break;
		}
		pv->var.DispAdiData = (LONG)(*((CHAR *)(&(Prm->zadjtp_n.b.l))));			/* <S142> */
		FunAnlgTrqRefManAdjustDisplay( Fmsg );
	return (rc);
#else															/* <S148> */
	BLINKFLG 	*BlinkFlag	= (BLINKFLG *)(Fmsg->pBlnkKick);

		FunCurrentManAdjEnd( Fmsg );
		BlinkFlag->no_op	= TRUE;
		return( FUN_END );										/* <S148> */
#endif            /* <S09F> */
}


/*****************************************************************************
  Description: PANEL Operator Fn00B Execution
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		FUN_CONTINUE: Execution Continue, FUN_END: Execution End
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
#if  SVFSEL_PANELOP_USE == 1
static LONG LpxPnlAnlgTrqRefManAdjExe(FUNMSG *Fmsg)
{
	FNREFTRQMANADJV *pv = (FNREFTRQMANADJV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
	AXIS_HANDLE *pAxRsc = Hmsg->pAxRsc;
	PRMDATA	*Prm = pAxRsc->UniPrms->Prm;
	REGIF_CONFIG_T	*RegCfg = pAxRsc->RegManager->hPnReg;
	LEDDSP *DspData = (LEDDSP *)&(Fmsg->pTxBuf[PNLRSP_LEDCODE_IDX]);		/* set display data buffer	*/
	OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
	LONG rc = FUN_CONTINUE;													/* 0: continue, 1: end	*/
//	SHORT Zadjtr_Tmp;
	DBYTEX			zadjtp_n_tmp;	/* <S142> */
	BLINKFLG *BlinkFlag = (BLINKFLG *)(Fmsg->pBlnkKick);					/* <S148> */
	CADC *pAdc = pAxRsc->pAdc;		/* <S170> */

/*--------------------------------------------------------------------------------------------------*/
/*		CurManuAdj (Fn00F) Sequence																	*/
/*--------------------------------------------------------------------------------------------------*/
		switch( pv->var.State )
		{
/*--------------------------------------------------------------------------------------------------*/
		case ANLG_TRQ_REF_MAN_ADJ_IDLE: 											/* Idle State 	*/
/*--------------------------------------------------------------------------------------------------*/
#if 0	/* <S148> */
//				pv->var.State = ANLG_TRQ_REF_MAN_ADJ_END;
				pv->var.State = ANLG_TRQ_REF_MAN_ADJ_EXE;		/* <S142> */
#else
			//if( OpKeyMake.data == TRUE )
			if( OpKeyMake.mode == TRUE )					/* <S162>  */
			{
				pv->var.State = ANLG_TRQ_REF_MAN_ADJ_END;
			}
			//else if ( OpKeyMake.mode == TRUE )
			else if ( OpKeyMake.data == TRUE )				/* <S162>  */
			{
				pv->var.State = ANLG_TRQ_REF_MAN_ADJ_EXE;
			}
#endif
			break;

/*--------------------------------------------------------------------------------------------------*/
		case ANLG_TRQ_REF_MAN_ADJ_EXE: 												/* Exe state	*/
/*--------------------------------------------------------------------------------------------------*/
//			if( OpKeyMake.data == TRUE )
//			{
//				pv->var.State = ANLG_TRQ_REF_MAN_ADJ_END;
//			}
//			else if ( OpKeyMake.up == TRUE )
			if ( OpKeyMake.up == TRUE )				/* <S148> */
			{
				/* PnE56 : �g���N�w�߃[����(������/������)�̒l�ǂݍ��� */
//				Zadjtr_Tmp = (SHORT)((Prm->zadjtp_n.w)& 0xFFFF);
				zadjtp_n_tmp.w = (SHORT)((Prm->zadjtp_n.w)& 0xFFFF);	/* <S142> */

				/* �\���f�[�^�X�V */
//				pv->var.DispAdiData ++;					/* <S142> */

				/* PnE56 : �g���N�w�߃[����(������/������) �X�V */
#if 0	/* <S142> */
				if( Zadjtr_Tmp < ZADJT_MAX )
				{
					Zadjtr_Tmp += 1;

				}

				/* PnE56 : �g���N�w�߃[����(������/������)�ɍX�V�����l���������� */
				Prm->zadjtp_n.w = Zadjtr_Tmp;
#else
				if( ((CHAR)zadjtp_n_tmp.b.l < ZADJT_MAX) && ((CHAR)zadjtp_n_tmp.b.h < ZADJT_MAX) )
				{
					(CHAR)zadjtp_n_tmp.b.l++;
					(CHAR)zadjtp_n_tmp.b.h++;
				}

				/*  PnE56 : �g���N�w�߃[����(������/������)�ɍX�V�����l���������� */
				Prm->zadjtp_n.w = zadjtp_n_tmp.w;
				pAdc->P.ZadjTrqP = (CHAR)zadjtp_n_tmp.b.l;				/* <S170> */
				pAdc->P.ZadjTrqN = (CHAR)zadjtp_n_tmp.b.h;				/* <S170> */
#endif
			}
			else if ( OpKeyMake.down == TRUE )
			{
				/* PnE56 : �g���N�w�߃[����(������/������)�̒l�ǂݍ��� */
//				Zadjtr_Tmp = (SHORT)((Prm->zadjtp_n.w)& 0xFFFF);
				zadjtp_n_tmp.w = (SHORT)((Prm->zadjtp_n.w)& 0xFFFF);	/* <S142> */

				/* �\���f�[�^�X�V */
//				pv->var.DispAdiData --;					/* <S142> */

				/* PnE56 : �g���N�w�߃[����(������/������) �X�V */
#if 0	/* <S142> */
				if( Zadjtr_Tmp > ZADJT_MIN )
				{
					Zadjtr_Tmp -= 1;
				}

				/* PnE56 : �g���N�w�߃[����(������/������)�ɍX�V�����l���������� */
				Prm->zadjtp_n.w = Zadjtr_Tmp;
#else
				if( ((CHAR)zadjtp_n_tmp.b.l > ZADJT_MIN) && ((CHAR)zadjtp_n_tmp.b.h > ZADJT_MIN) )
				{
					(CHAR)zadjtp_n_tmp.b.l--;
					(CHAR)zadjtp_n_tmp.b.h--;
				}

				/*  PnE56 : �g���N�w�߃[����(������/������)�ɍX�V�����l���������� */
				Prm->zadjtp_n.w = zadjtp_n_tmp.w;
				pAdc->P.ZadjTrqP = (CHAR)zadjtp_n_tmp.b.l;				/* <S170> */
				pAdc->P.ZadjTrqN = (CHAR)zadjtp_n_tmp.b.h;				/* <S170> */
#endif
			}
#if 0	/* <S148> */
			else if ( OpKeyMake.mode == TRUE )
			{
				/* PnE56 : �g���N�w�߃[����(������/������)��EepRom�֏������� */
				PrmStoreValueToEeprom( RegCfg, &pndef_zadjtp_n, 0 );
			}
			else
			{
				/*do nothing*/
			}
#else
			//else if(( OpKeyMake.mode == TRUE )||( OpKeyMake.shup == TRUE ))
			else if( OpKeyMake.mode == TRUE )									/* <S162>  */
			{
				/* PnE56 : �g���N�w�߃[����(������/������)��EepRom�֏������� */
				if( PrmStoreValueToEeprom( RegCfg, &pndef_zadjtp_n, 0 ) == 0 )
				{
					BlinkFlag->done 	= TRUE;
				}
				else
				{
					BlinkFlag->error 	= TRUE;
				}
				//pv->var.State = ANLG_TRQ_REF_MAN_ADJ_IDLE;
				pv->var.State = ANLG_TRQ_REF_MAN_ADJ_EXIT;		/* <S162>  */
			}
#endif
			break;
/*--------------------------------------------------------------------------------------------------*/
		case ANLG_TRQ_REF_MAN_ADJ_EXIT:		/* <S162>  */
			if( OpKeyMake.data == TRUE )
			{
				pv->var.State = ANLG_TRQ_REF_MAN_ADJ_END;
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case ANLG_TRQ_REF_MAN_ADJ_END: 												/* End state		*/
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
		/* �p�l���\���f�[�^�쐬	*/
#if 0	/* <S148> */
		pv->var.DispAdiData = (LONG)(*((CHAR *)(&(Prm->zadjtp_n.b.l))));			/* <S142> */
		RpxLedSetStr( (UCHAR *)"  trq", DspData, 4U, 5U );
		RpxLedSetStr( (UCHAR *)".", DspData, 4U, 1U );
		RpxLedDispMonDecWord( (ULONG)( pv->var.DispAdiData & 0xFFFF ), DspData, 0U, DSPDEF_W_SIGN );
#else
		//if( pv->var.State == ANLG_TRQ_REF_MAN_ADJ_IDLE )
		if(( pv->var.State == ANLG_TRQ_REF_MAN_ADJ_IDLE )||( pv->var.State == ANLG_TRQ_REF_MAN_ADJ_EXIT ))	/* <S162>  */
		{
			RpxLedSetStr( (UCHAR *)"  trq", DspData, 4U, 5U );
			RpxLedSetStr( (UCHAR *)".", DspData, 4U, 1U );
			RpxLedSetStatusBits( DspData, Hmsg );
		}
		else if( pv->var.State == ANLG_TRQ_REF_MAN_ADJ_EXE )
		{
			pv->var.DispAdiData = (LONG)(*((CHAR *)(&(Prm->zadjtp_n.b.l))));
			RpxLedDispMonDecWord( (ULONG)( pv->var.DispAdiData & 0xFFFF ), DspData, 0U, DSPDEF_W_SIGN );
		}
#endif
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}
#endif
/***************************************** end of file **********************************************/
