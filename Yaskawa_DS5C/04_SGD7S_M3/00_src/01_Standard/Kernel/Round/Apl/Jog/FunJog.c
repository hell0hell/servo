/****************************************************************************************************/
/*																									*/
/*																									*/
/*		FunJog.c : Jog Function (Fn002) Sample Source												*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	Function:																						*/
/*			1)Execute Jog Mode(Fn002)																*/
/*			2)Jog Mode Display																		*/
/*			3)Edit Jog Parameter																	*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.1.00 : 2013.05.15  K.Ozaki																*/
/*		Rev.1.01 : 2013.08.10  K.Ozaki	Add Panel Operator Function									*/
/*		Rev.1.02 : 2013.12.28  K.Ozaki	Bug Fix														*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include	"KnlApi.h"		/* Kernel API */
#include	"LcdOp.h"		/* LCD Operator */
#include 	"Function.h"	/* Fn Function */
#include	"Operation.h"	/* Servo Operation */

/****************************************************************************************************/
/*																									*/
/*		Variable Definition																			*/
/*																									*/
/****************************************************************************************************/
/**
 * Fn002 Jog Variable
 */
typedef struct _FnJogV {
	LONG  State;				/* Jog (Fn002) Execution State										*///<S0C7>
	LONG  OpeModeIdx;			/* Operation Mode Register Index									*/
	LONG  OpeSetIdx;			/* Operation Set Register Index										*/
	ULONG DispTime;				/* Display Time (Panel Operator)									*/
	LONG  Reserved[(256/4)-4]; 	/* Reserved: All functions can use ram variables at least 256 bytes.*/
} FNJOGV;

/****************************************************************************************************/
/*																									*/
/*		Constant Definition																			*/
/*																									*/
/****************************************************************************************************/
#define	JOG_DRV				0x00
#define	JOG_END				0x01 			/* （予約：未使用） */
#define	JOG_ERR				0x02
/*--------------------------------------------------------------------------------------------------*/
/* Operation Register 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define OPEMODE_REG1		(0x0000)		/* 1st Operation Mode Register */
#define OPESET_REG1			(0x0001)		/* 1st Operation Mode Set Register */
#define SAFESTOP_REG		(0x0008)		/* Safety Stop Time */
/*--------------------------------------------------------------------------------------------------*/
/* Operation Mode	 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define FNJOG_MODE			(0x1002) 		/* Jog Mode */
/*--------------------------------------------------------------------------------------------------*/
/* Operation Command 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define JOGCMD_STOP 		(0x0)
#define JOGCMD_RUN 			(0x1)
#define JOGCMD_FORWARD 		(0x2)
#define JOGCMD_REVERSE 		(0x3)
#define JOGCMD_SVONOFF 		(0x4)
/*--------------------------------------------------------------------------------------------------*/
#define OPEREG_WRITE( idx, data, hmsg )   (COPEREG->Write( COPEREG, (idx), (data), hmsg ))

/*--------------------------------------------------------------------------------------------------*/
enum _enumFnJogState
{
	FNJOGST_INIT = 0,
	FNJOGST_SVOFF,
	FNJOGST_SVON,
	FNJOGST_END
};
/*--------------------------------------------------------------------------------------------------*/

/****************************************************************************************************/
/*																									*/
/*		Function List																				*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Public Function																				*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunJogBegin(FUNMSG *Fmsg); 						/* Fn002 Jog Mode Begin						*/
LONG FunJogMain(FUNMSG *Fmsg);							/* Fn002 Jog Mode Main						*/
void FunJogEnd(FUNMSG *Fmsg);							/* Fn002 Jog Mode End						*/

/*--------------------------------------------------------------------------------------------------*/
/*		Private Function																			*/
/*--------------------------------------------------------------------------------------------------*/
LONG LpxLcdJogExe(FUNMSG *hMsg);						/* LCD Operator Fn002 Execution 			*/
#if  SVFSEL_PANELOP_USE != 1
#define LpxPnlJogExe( x ) (1)
#else
LONG LpxPnlJogExe(FUNMSG *hMsg);						/* Panel Operator Fn002 Execution 			*/
#endif

/****************************************************************************************************/
/*																									*/
/*		Fn002 Jog Mode Display																		*/
/*																									*/
/****************************************************************************************************/
LONG FunJogMain(FUNMSG *Fmsg)
{
MSGIF *hMsg = Fmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	if( KPI_CHK_LopHmsg( hMsg ) )							/* LCD Operator */
	{
		return LpxLcdJogExe( Fmsg );
	} 
/*--------------------------------------------------------------------------------------------------*/
	else if ( KPI_CHK_PopHmsg( hMsg ) )						/* Panel Operator */
	{
		return LpxPnlJogExe( Fmsg );
	}
/*--------------------------------------------------------------------------------------------------*/
}


/****************************************************************************************************/
/*																									*/
/*		Fn002 Jog Mode Begin																		*/
/*																									*/
/****************************************************************************************************/
LONG FunJogBegin(FUNMSG *Fmsg)
{
LONG	i;			/* <S0E4> */
BOOL	SpMode;		/* <S0E4> */
LONG	rc = OK;
FNJOGV  *pv = (FNJOGV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
/*	Check Special Mode																<S0E4>			*/
/*--------------------------------------------------------------------------------------------------*/
//	if( Hmsg->pAxRsc->UniPrms->Prm->b_prm0 & 0x2000 )
	if( Hmsg->pAxRsc->UniPrms->Prm->syssw3 & 0x8000 )		/* PnE0D.Bit15 = 1 2軸同時JOG修正<S147> */
	{
		SpMode = TRUE;
		while( Hmsg->pAxRsc->AxisNo != 0 )
		{
			Hmsg->pAxRsc--;
		}
	}
	else
	{
		SpMode = FALSE;
	}

/*--------------------------------------------------------------------------------------------------*/
	pv->State = FNJOGST_INIT;
/*--------------------------------------------------------------------------------------------------*/
/*	Search Register																					*/
/*--------------------------------------------------------------------------------------------------*/
	rc = COPEREG->Search( COPEREG, 							/* Search Register (Operation Mode)		*/
				OPEMODE_REG1,
				0,
				&(pv->OpeModeIdx),
				MCHK_REG_READ,
				Hmsg);
	if( rc != OK ) { return (rc); }
/*--------------------------------------------------------------------------------------------------*/
	rc = COPEREG->Search( COPEREG,							/* Search Register (Operation Command)	*/
				OPESET_REG1,
				0,
				&(pv->OpeSetIdx),
				MCHK_REG_READ,
				Hmsg);
	if( rc != OK ) { return (rc); }
/*--------------------------------------------------------------------------------------------------*/
/* Set Jog Mode to Mode Register 																	*/
/*--------------------------------------------------------------------------------------------------*/
/* for debug *//* <S0E4> */
	if( SpMode == TRUE )
	{
		for( i=0; i<NumberOfAxes; i++, Hmsg->pAxRsc++ )
		{
			rc |= OPEREG_WRITE( pv->OpeModeIdx, FNJOG_MODE, Hmsg );
		}
		Hmsg->pAxRsc -= NumberOfAxes;
	}
	else
/* for debug */
	rc = OPEREG_WRITE( pv->OpeModeIdx, FNJOG_MODE, Hmsg );
	if ( rc != OK ){ return (rc); }
/*--------------------------------------------------------------------------------------------------*/
	KpiRstLongTimer( &(pv->DispTime) );						/* Panel Operator 						*/
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}


/****************************************************************************************************/
/*																									*/
/*		Fn002 Jog Mode End																			*/
/*																									*/
/****************************************************************************************************/
void FunJogEnd(FUNMSG *Fmsg)
{
LONG	i;			/* <S0E4> */
BOOL	SpMode;		/* <S0E4> */
FNJOGV  *pv = (FNJOGV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
/*	Check Special Mode																<S0E4>			*/
/*--------------------------------------------------------------------------------------------------*/
#if	(CSW_BOARD_TYPE != SGD7S_TYPE)
//	if( Hmsg->pAxRsc->UniPrms->Prm->b_prm0 & 0x2000 )
	if( Hmsg->pAxRsc->UniPrms->Prm->syssw3 & 0x8000 )		/* PnE0D.Bit15 = 1 <S140> */
	{
		SpMode = TRUE;
		while( Hmsg->pAxRsc->AxisNo != 0 )
		{
			Hmsg->pAxRsc--;
		}
	}
	else
#endif	/* #if	(CSW_BOARD_TYPE != SGD7S_TYPE) <S140> */
	{
		SpMode = FALSE;
	}
/*--------------------------------------------------------------------------------------------------*/
/* for debug *//* <S0E4> */
	if( SpMode == TRUE )
	{
		for( i=0; i<NumberOfAxes; i++, Hmsg->pAxRsc++ )
		{
			OPEREG_WRITE( pv->OpeSetIdx, JOGCMD_STOP, Hmsg );
			OPEREG_WRITE( pv->OpeModeIdx, 0, Hmsg );
		}
		Hmsg->pAxRsc -= NumberOfAxes;
	}
	else
	/* for debug *//* <S0E4> */
	{
		OPEREG_WRITE( pv->OpeSetIdx, JOGCMD_STOP, Hmsg );
		OPEREG_WRITE( pv->OpeModeIdx, 0, Hmsg );
	}
}


/****************************************************************************************************/
/*																									*/
/*		Fn002 Jog mode Execution (LCD Operator)														*/
/*																									*/
/****************************************************************************************************/
LONG LpxLcdJogExe(FUNMSG *Fmsg)
{
LONG	i;			/* <S0E4> */
BOOL	SpMode;		/* <S0E4> */
FNJOGV *pv = (FNJOGV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
CHAR *ResBuf = Fmsg->pTxBuf;
LONG  IgnrBlnk = FALSE;
OPKEYFLG OpKeyMk = *(OPKEYFLG *)(Fmsg->pKeyMk);
OPKEYFLG OpKeyLvl = *(OPKEYFLG *)(Fmsg->pKeyLvl);
LONG rc = 0;													/* 0: continue, 1: end */
LONG regRslt=0;
AXIS_HANDLE *pAxRsc = Hmsg->pAxRsc;								/* <S1CF> */
/*--------------------------------------------------------------------------------------------------*/
/*	Check Special Mode																<S0E4>			*/
/*--------------------------------------------------------------------------------------------------*/
#if	(CSW_BOARD_TYPE != SGD7S_TYPE)
//		if( Hmsg->pAxRsc->UniPrms->Prm->b_prm0 & 0x2000 )
		if( Hmsg->pAxRsc->UniPrms->Prm->syssw3 & 0x8000 )		/* PnE0D.Bit15 = 1 <S140> */
		{
			SpMode = TRUE;
			while( Hmsg->pAxRsc->AxisNo != 0 )
			{
				Hmsg->pAxRsc--;
			}
		}
		else
#endif	/* #if	(CSW_BOARD_TYPE != SGD7S_TYPE) */
		{
			SpMode = FALSE;
		}
/*--------------------------------------------------------------------------------------------------*/
		RpxLcdClrTxBuf( ResBuf ); 								/* clear Tx buffer */
//		RpxLcdSetStr(0, 12, (UCHAR *)"-JOG-", ResBuf );			/* set title *//* <S154> */
		RpxLcdSetStr(0, 14, (UCHAR *)"JOG", ResBuf );			/* set title *//* <S154> */
/*--------------------------------------------------------------------------------------------------*/
/*		Jog (Fn002) Sequence																		*/
/*--------------------------------------------------------------------------------------------------*/
		switch( pv->State )
		{
/*--------------------------------------------------------------------------------------------------*/
		case FNJOGST_INIT: 										/* Initial State 					*/
/*--------------------------------------------------------------------------------------------------*/
			pv->State = FNJOGST_SVOFF;
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNJOGST_SVOFF: 									/* Servo off state					*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMk.mode )
			{
				pv->State = FNJOGST_END;
			}
			else if ( OpKeyMk.svon )
			{
				/* for debug *//* <S0E4> */
				if( SpMode == TRUE )
				{
					for( i=0; i<NumberOfAxes; i++, Hmsg->pAxRsc++ )
					{
						regRslt |= OPEREG_WRITE( pv->OpeSetIdx, JOGCMD_SVONOFF, Hmsg );			/* servo on */
					}
					Hmsg->pAxRsc -= NumberOfAxes;		/*<S140>*/
					if( regRslt == PRM_RSLT_SUCCESS )
					{
						pv->State = FNJOGST_SVON;
					}
					else
					{
						((BLINKFLG *)(Fmsg->pBlnkKick))->no_op = 1;						/* <S0B4> No Operation */
						for( i=0; i<NumberOfAxes; i++, Hmsg->pAxRsc++ )		/*<S140>*/
						{
							OPEREG_WRITE( pv->OpeSetIdx, JOGCMD_SVONOFF, Hmsg );
						}
						Hmsg->pAxRsc -= NumberOfAxes;
					}
				}
				/* for debug *//* <S0E4> */
				else
				{
					if ( PRM_RSLT_SUCCESS == OPEREG_WRITE( pv->OpeSetIdx, JOGCMD_SVONOFF, Hmsg ) )		/* <S0B4> set servo on */
					{
						pv->State = FNJOGST_SVON;
					}
					else
					{
						((BLINKFLG *)(Fmsg->pBlnkKick))->no_op = 1;						/* <S0B4> No Operation */
					}
				}
			}
			else
			{ 											
				/* edit parameter */
				LcdPnUnEditSub( LCDPMDSP_JOG, Fmsg->pKeyMk, Fmsg->pBlnkKick );	
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNJOGST_SVON: 												/* Servo on state */
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMk.mode )
			{
				pv->State = FNJOGST_END;
			}
			else if ( OpKeyMk.svon )
			{
				/* for debug *//* <S0E4S0E4> */
				if( SpMode == TRUE )
				{
					for( i=0; i<NumberOfAxes; i++, Hmsg->pAxRsc++ )
					{
						OPEREG_WRITE( pv->OpeSetIdx, JOGCMD_SVONOFF, Hmsg );
					}
					Hmsg->pAxRsc -= NumberOfAxes;
				}
				else
				{
					/* for debug *//* <S0E4> */
					OPEREG_WRITE( pv->OpeSetIdx, JOGCMD_SVONOFF, Hmsg );
				}
				pv->State = FNJOGST_SVOFF; 								/* servo off */
			}
			else if( CHK_BB() )				/* <S1CF> */
			{
				pv->State = FNJOGST_SVOFF;
			}
			else 
			{
				if ( OpKeyLvl.up )
				{
					/* for debug *//* <S0E4> */
					if( SpMode == TRUE )
					{
						for( i=0; i<NumberOfAxes; i++, Hmsg->pAxRsc++ )
						{
							OPEREG_WRITE( pv->OpeSetIdx, JOGCMD_FORWARD, Hmsg );
						}
						Hmsg->pAxRsc -= NumberOfAxes;
					}
					else
					{
						/* for debug *//* <S0E4> */
						OPEREG_WRITE( pv->OpeSetIdx, JOGCMD_FORWARD, Hmsg );
					}
				}
				else if ( OpKeyLvl.down )
				{
					/* for debug *//* <S0E4> */
					if( SpMode == TRUE )
					{
						for( i=0; i<NumberOfAxes; i++, Hmsg->pAxRsc++ )
						{
							OPEREG_WRITE( pv->OpeSetIdx, JOGCMD_REVERSE, Hmsg );
						}
						Hmsg->pAxRsc -= NumberOfAxes;
					}
					else
					{
						/* for debug *//* <S0E4> */
						OPEREG_WRITE( pv->OpeSetIdx, JOGCMD_REVERSE, Hmsg );
					}
				}
				else
				{
					/* for debug *//* <S0E4> */
					if( SpMode == TRUE )
					{
						for( i=0; i<NumberOfAxes; i++, Hmsg->pAxRsc++ )
						{
							OPEREG_WRITE( pv->OpeSetIdx, JOGCMD_STOP, Hmsg );
						}
						Hmsg->pAxRsc -= NumberOfAxes;
					}
					else
					{
						/* for debug *//* <S0E4> */
						OPEREG_WRITE( pv->OpeSetIdx, JOGCMD_STOP, Hmsg );
					}
				}
			}

			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNJOGST_END: 												/* End state */
/*--------------------------------------------------------------------------------------------------*/
			FunJogEnd( Fmsg );
			rc = 1;
			break;
		default: 
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
		IgnrBlnk = ( FNJOGST_SVON == pv->State );					
		LcdMkDspDataPnUnLines(LCDPMDSP_JOG, IgnrBlnk, ResBuf ); 		/* make Pn/Un lines */
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}

/****************************************************************************************************/
/*																									*/
/*		Fn002 Jog mode Execution (Panel Operator)													*/
/*																									*/
/****************************************************************************************************/
#if  SVFSEL_PANELOP_USE == 1
LONG LpxPnlJogExe(FUNMSG *Fmsg)
{
FNJOGV *pv = (FNJOGV *)(Fmsg->pVar);							/* get Variable Pointer */
MSGIF *Hmsg = Fmsg->Super;										/* message interface */
LEDDSP *DspData = (LEDDSP *)&(Fmsg->pTxBuf[PNLRSP_LEDCODE_IDX]);/* set display data buffer */
OPKEYFLG OpKeyMk = *(OPKEYFLG *)(Fmsg->pKeyMk);					/* get key make flag */
OPKEYFLG OpKeyLvl = *(OPKEYFLG *)(Fmsg->pKeyLvl);				/* get key level flag */
LONG rc = 0;													/* 0: continue, 1: end */
AXIS_HANDLE *pAxRsc = Hmsg->pAxRsc;								/* TODO: 後で消す */
/*--------------------------------------------------------------------------------------------------*/
/*		Jog (Fn002) Sequence																		*/
/*--------------------------------------------------------------------------------------------------*/
		switch( pv->State )
		{
/*--------------------------------------------------------------------------------------------------*/
		case FNJOGST_INIT: 										/* Initial State 					*/
/*--------------------------------------------------------------------------------------------------*/
			pv->State = FNJOGST_SVOFF;
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNJOGST_SVOFF: 									/* Servo off state					*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMk.data )
			{
				pv->State = FNJOGST_END;
			}
			else if ( OpKeyMk.mode )
			{
				if ( PRM_RSLT_SUCCESS == OPEREG_WRITE( pv->OpeSetIdx, JOGCMD_SVONOFF, Hmsg ) )		/* <S0B4> set servo on */
				{
					pv->State = FNJOGST_SVON;
				}
				else
				{
					((BLINKFLG *)(Fmsg->pBlnkKick))->no_op = 1;						/* <S0B4> No Operation */
				}
			}
			else
			{ 											
				/* edit parameter */
				//LcdPnUnEditSub( LCDPMDSP_JOG, Fmsg->pKeyMk, Fmsg->pBlnkKick );	
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNJOGST_SVON: 												/* Servo on state */
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMk.data )
			{
				pv->State = FNJOGST_END;
			}
			else if ( OpKeyMk.mode )
			{
				OPEREG_WRITE( pv->OpeSetIdx, JOGCMD_SVONOFF, Hmsg );
				pv->State = FNJOGST_SVOFF; 								/* servo off */
			}
			else if ( CHK_BB() )										/* <S0B4> */
			{
				pv->State = FNJOGST_SVOFF; 								/* servo off */
			}
			else 
			{
				if ( OpKeyLvl.up )
				{
					OPEREG_WRITE( pv->OpeSetIdx, JOGCMD_FORWARD, Hmsg );
				}
				else if ( OpKeyLvl.down )
				{
					OPEREG_WRITE( pv->OpeSetIdx, JOGCMD_REVERSE, Hmsg );
				}
				else
				{
					OPEREG_WRITE( pv->OpeSetIdx, JOGCMD_STOP, Hmsg );
				}
			}

			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNJOGST_END: 												/* End state */
/*--------------------------------------------------------------------------------------------------*/
			FunJogEnd( Fmsg );
			rc = 1;
			break;
		default: 
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Make Display Data																			*/
/*--------------------------------------------------------------------------------------------------*/
		RpxLedSetStr( (UCHAR *)". JoG", DspData, 4, 5 );
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
			if( !( (KpiGetLongTimer( &(pv->DispTime) )/100) & 0x04) )
			{
				RpxLedSetStr( (UCHAR *)". Hbb", DspData, 4, 5 );	/* set HBB */
			}
		}
		RpxLedSetStatusBits( DspData, Hmsg );						/* set panel status bits */
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}
#endif

/***************************************** end of file **********************************************/
