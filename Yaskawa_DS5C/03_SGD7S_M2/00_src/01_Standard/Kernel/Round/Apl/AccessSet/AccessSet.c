/****************************************************************************************************/
/*																									*/
/*																									*/
/*		AnlgRefManAdj.c : Rewriting Inhibition Processing (Fn010)		<S059>						*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	Function:																						*/
/*			1)Execute Rewriting Inhibition Processing Mode (Fn010)									*/
/*			2)Set AccessLevel																		*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.1.00 : 2013.11.29  T.Masuo																*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include	"KnlApi.h"			/* Kernel API */
#include	"LcdOp.h"			/* LCD Operator */
#include	"Function.h"		/* Fn Function */
#include	"Operation.h"		/* Servo Operation */
#include	"PnPrmListTbl.h"	/* Register and Parameter Access Interface */

/****************************************************************************************************/
/*																									*/
/*		Variable Definition																			*/
/*																									*/
/****************************************************************************************************/
/* Fn010 Mode Variable */
typedef struct _FnAcsSetV {
	INT State;						/* Rewriting Inhibition Processingt (Fn010) Execution State */
	INT	LPos;						/* Line Position */
	INT	DPos;						/* Digit Position */
	INT OpeModeIdx;					/* Operation Mode Register Index */
	INT OpeSetIdx;					/* Operation Set Register Index */
	INT PrmSetIdx;					/* Parameter Set Register Index */
	USHORT	PswdData;				/* Setting Data */
	INT Reserved[(256/4)-6-4];		/* Reserved: All functions can use ram variables at least 256 bytes. */
} FNACSSETV;


/****************************************************************************************************/
/*																									*/
/*		Constant Definition																			*/
/*																									*/
/****************************************************************************************************/
//#define OK		(0x0000)//<S0C7>incompatible redefinition of macro "OK"
//#define NG		(0x0001)//<S0C7>incompatible redefinition of macro "NG"

/*--------------------------------------------------------------------------------------------------*/
/* Operation Register 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define OPEMODE_REG1		(0x0000)		/* 1st Operation Mode Register */
#define OPESET_REG1			(0x0001)		/* 1st Operation Mode Set Register */
#define PRM_REG1			(0xE0D)			/* 0xE0D: PnE0D:システムスイッチ3 */

/*--------------------------------------------------------------------------------------------------*/
/* Operation Mode	 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define FNTEST_ACSSET_MODE	(0x8001) 	/* Force Rewriting Inhibition Processing Mode */
#define FNNORMAL_MODE		(0x0000)	/* normal mode	*/


/*--------------------------------------------------------------------------------------------------*/
/* Operation Command 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define ACSSET_END 			(0x0)
#define ACSSET_EXE			(0x1)

/*--------------------------------------------------------------------------------------------------*/
#define FUN_CONTINUE			(0L)			/*Continue	*/
#define FUN_END					(1L)			/*End		*/

/*--------------------------------------------------------------------------------------------------*/
#define OPEREG_WRITE( idx, data, hmsg )   (COPEREG->Write( COPEREG, (idx), (data), hmsg ))
#define PRMREG_WRITE( idx, data, hmsg )   (CPARAMX->Write( CPARAMX, (idx), (data), hmsg ))

enum _enumFnAcsSetState {
	ACS_SET_EXE = 0,
	ACS_SET_WAIT,
	ACS_SET_END,
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
LONG FunAccessSetMain(FUNMSG *Fmsg);
LONG FunAccessSetBegin(FUNMSG *Fmsg);
void FunAccessSetEnd(FUNMSG *Fmsg);

/*--------------------------------------------------------------------------------------------------*/
/*		Private Function																			*/
/*--------------------------------------------------------------------------------------------------*/
LONG LpxLcdAccessSetExe(FUNMSG *Fmsg);
LONG FunSetAccessLevel(FUNMSG *Fmsg);

#if  SVFSEL_PANELOP_USE == 0
#define LpxPnlAccessSetExe( x ) (1)
#else
static LONG LpxPnlAccessSetExe(FUNMSG *hMsg);		/* Panel Operator Fn10F Execution */
#endif


/****************************************************************************************************/
/*																									*/
/*		Fn010: Rewriting Inhibition Processing Mode 												*/
/*																									*/
/****************************************************************************************************/
LONG FunAccessSetMain(FUNMSG *Fmsg)
{
MSGIF *hMsg = Fmsg->Super;
LONG rc = FUN_CONTINUE;

/*--------------------------------------------------------------------------------------------------*/
	if( KPI_CHK_LopHmsg( hMsg ) )							/* LCD Operator */
	{
		return LpxLcdAccessSetExe( Fmsg );
	} 
	else if ( KPI_CHK_PopHmsg( hMsg ) )						/* Panel Operator */
	{
		return LpxPnlAccessSetExe( Fmsg );
	}
	else
	{
		/* Do Nothing */
	}

	return (rc);
}

/****************************************************************************************************/
/*																									*/
/* Fn010: Entry of Rewriting Inhibition Processing													*/
/*																									*/
/****************************************************************************************************/
LONG FunAccessSetBegin(FUNMSG *Fmsg)
{
	LONG		rc = FUN_CONTINUE;
	FNACSSETV	*pv = (FNACSSETV *)(Fmsg->pVar);
	MSGIF		*Hmsg = Fmsg->Super;

/*--------------------------------------------------------------------------------------------------*/
	pv->State = ACS_SET_EXE;	/* Reset Rewriting Inhibition Processing Sequenc */
	pv->DPos = 0;				/* Reset Digit Position */
	pv->LPos = 4;				/* Reset Line Position */
	pv->PswdData = 0;			/* Reset Setting Data */

/*--------------------------------------------------------------------------------------------------*/
/*	Search Register																					*/
/*--------------------------------------------------------------------------------------------------*/

	/* Search Register (Operation Mode == Nomal Mode ) */
	rc = COPEREG->Search( COPEREG, 							
				OPEMODE_REG1,
				0,
				&(pv->OpeModeIdx),
				MCHK_REG_READ,
				Hmsg);
	if( rc != OK ) { return (rc); }

/*--------------------------------------------------------------------------------------------------*/
	/* Search Register (Operation Mode Set Register) */
	rc = COPEREG->Search( COPEREG,
				OPESET_REG1,
				0,
				&(pv->OpeSetIdx),
				MCHK_REG_READ,
				Hmsg);
	if( rc != OK ) { return (rc); }

/*--------------------------------------------------------------------------------------------------*/
/* Set Fn010 Mode to Mode Register 																	*/
/*--------------------------------------------------------------------------------------------------*/
	rc = OPEREG_WRITE( pv->OpeModeIdx, FNTEST_ACSSET_MODE, Hmsg );
	if ( rc != OK ){ return (rc); }

	return (rc);
}

/****************************************************************************************************/
/*																									*/
/* Fn010: End of Rewriting Inhibition Processing													*/
/*																									*/
/****************************************************************************************************/
void FunAccessSetEnd(FUNMSG *Fmsg)
{
	FNACSSETV	*pv = (FNACSSETV *)(Fmsg->pVar);
	MSGIF		*Hmsg = Fmsg->Super;

	OPEREG_WRITE( pv->OpeSetIdx, ACSSET_END, Hmsg );
	OPEREG_WRITE( pv->OpeModeIdx, FNNORMAL_MODE, Hmsg );

}

/****************************************************************************************************/
/*																									*/
/*			Fn010 Rewriting Inhibition Processing (LCD Operator)									*/
/*																									*/
/****************************************************************************************************/
LONG LpxLcdAccessSetExe(FUNMSG *Fmsg)
{
	FNACSSETV 	*pv = (FNACSSETV *)(Fmsg->pVar);
	MSGIF 		*Hmsg = Fmsg->Super;
	UCHAR 		*ResBuf = Fmsg->pTxBuf;//<S0C7>
	OPKEYFLG 	OpKeyMk = *(OPKEYFLG *)(Fmsg->pKeyMk);
	BLINKFLG *BlinkFlag = (BLINKFLG *)(Fmsg->pBlnkKick);
	LONG 		rc = 0;	
//	LONG 		i;//<S0C7>
//	USHORT		LedSts;//<S0C7>


	/* clear Tx buffer */
	RpxLcdClrTxBuf( ResBuf );

	/* TITLE		*/
	RpxLcdSetStr(1U, 0U, (UCHAR *)"Parameter", ResBuf);
	RpxLcdSetStr(2U, 3U, (UCHAR *)"Write Protect", ResBuf);

	/* Contents		*/
	RpxLcdSetStr(4U, 5U, (UCHAR *)"P.", ResBuf);
	RpxLcdSetValHex(4U, 7U, (ULONG)(pv->PswdData), 4, ResBuf );		/* set unsigned long data */

	/* 点滅設定		*/
	RpxLcdClrLineBlnk( 4U, ResBuf );
	RpxLcdSetBlnk( 4U, (USHORT)( 10 - (pv->DPos) ), 1U, ResBuf );

/*--------------------------------------------------------------------------------------------------*/
/*			Fn010 : Rewriting Inhibition Processing Sequence										*/
/*--------------------------------------------------------------------------------------------------*/
	switch( pv->State )
	{
/*--------------------------------------------------------------------------------------------------*/
	case ACS_SET_EXE:													/* Execute State			*/
/*--------------------------------------------------------------------------------------------------*/
		OPEREG_WRITE( pv->OpeSetIdx, ACSSET_EXE, Hmsg );
		pv->State = ACS_SET_WAIT;
		break;
/*--------------------------------------------------------------------------------------------------*/
	case ACS_SET_WAIT:													/* Wate state				*/
/*--------------------------------------------------------------------------------------------------*/
		if( OpKeyMk.mode )
		{
			pv->State = ACS_SET_END;
		}
		else if( OpKeyMk.up )									/* Up Key */
		{
			if( ((pv->PswdData >> (4*pv->DPos)) & 0x0F) != 0x0F )
			{
				pv->PswdData = pv->PswdData  + ( 1 << (4*pv->DPos) );
			}
		}
		else if( OpKeyMk.down )									/* Down Key */
		{
			if( ((pv->PswdData >> (4*pv->DPos)) & 0x0F) != 0x00 )
			{
				pv->PswdData = pv->PswdData - ( 1 << (4*pv->DPos) );
			}
		}
		else if ( OpKeyMk.shup )								/* Shift-up Key ( <- Key ) */
		{
			if ( pv->DPos != 3 )
			{
				pv->DPos++;
			}
			else
			{
				pv->DPos = 0;
			}
		}
		else if ( OpKeyMk.shdown )								/* Shift-down Key ( -> Key ) */
		{
			if ( pv->DPos != 0 )
			{
				pv->DPos--;
			}
			else
			{
				pv->DPos = 3;
			}
		}
		else if ( OpKeyMk.data )
		{
			/* アクセルレベル設定 */
			if( FunSetAccessLevel( Fmsg ) )
			{
				/* アクセルレベル設定　異常完了の場合は"error"を表示 */
				BlinkFlag->error = TRUE;
			}
			else
			{
				/* アクセルレベル設定　正常完了の場合は"done"を表示 */
				BlinkFlag->done = TRUE;
			}
		}
		break;

/*--------------------------------------------------------------------------------------------------*/
	case ACS_SET_END: 													/* End state				*/
/*--------------------------------------------------------------------------------------------------*/

			FunAccessSetEnd( Fmsg );
			rc = FUN_END;

		break;
	default:
		break;
	}
	return (rc);
}

/****************************************************************************************************/
/*																									*/
/*			Fn010 Rewriting Inhibition Processing (PANEL Operator) Display Data						*/
/*																									*/
/****************************************************************************************************/
#if  SVFSEL_PANELOP_USE == 1
LONG LpxPnlAccessSetExe(FUNMSG *Fmsg)
{
	FNACSSETV *pv = (FNACSSETV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
	LEDDSP *DspData = (LEDDSP *)&(Fmsg->pTxBuf[PNLRSP_LEDCODE_IDX]);/* set display data buffer */
	OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
	BLINKFLG *BlinkFlag = (BLINKFLG *)(Fmsg->pBlnkKick);
	LONG rc = FUN_CONTINUE;													/* 0: continue, 1: end */
//	ULONG i;//<S0C7>

//	RpxLedSetStr((UCHAR *)"P.", DspData, 4U, 2U);	/* <S0EA> */
//	RpxLedSetValHex( pv->PswdData, DspData, 4U, 4U);	/* <S148> */

/*--------------------------------------------------------------------------------------------------*/
/*			Fn010 : Rewriting Inhibition Processing Sequence										*/
/*--------------------------------------------------------------------------------------------------*/
	switch( pv->State )
	{
/*--------------------------------------------------------------------------------------------------*/
	case ACS_SET_EXE:													/* Execute State			*/
/*--------------------------------------------------------------------------------------------------*/
		OPEREG_WRITE( pv->OpeSetIdx, ACSSET_EXE, Hmsg );
		pv->State = ACS_SET_WAIT;
		break;
/*--------------------------------------------------------------------------------------------------*/
	case ACS_SET_WAIT:													/* Wate state				*/
/*--------------------------------------------------------------------------------------------------*/
//		RpxLedSetStr((UCHAR *)"P.", DspData, 4U, 2U);	/* <S0EA> */	/* <S148> */

//		if( OpKeyMake.mode )
		if( OpKeyMake.data )	/* <S148> */
		{
			pv->State = ACS_SET_END;
		}
		else if( OpKeyMake.up )									/* Up Key */
		{
			if( ((pv->PswdData >> (4*pv->DPos)) & 0x0F) != 0x0F )
			{
				pv->PswdData = pv->PswdData  + ( 1 << (4*pv->DPos) );
			}
		}
		else if( OpKeyMake.down )									/* Down Key */
		{
			if( ((pv->PswdData >> (4*pv->DPos)) & 0x0F) != 0x00 )
			{
				pv->PswdData = pv->PswdData - ( 1 << (4*pv->DPos) );
			}
		}
		else if ( OpKeyMake.shup )								/* Shift-up Key ( <- Key ) */
		{
			if ( pv->DPos != 3 )
			{
				pv->DPos++;
			}
			else
			{
				pv->DPos = 0;
			}
		}
#if 0	/* <S148> */
		else if ( OpKeyMake.shdown )								/* Shift-down Key ( -> Key ) */
		{
			if ( pv->DPos != 0 )
			{
				pv->DPos--;
			}
			else
			{
				pv->DPos = 3;
			}
		}
		else if ( OpKeyMk.data )
#else
		else if ( OpKeyMk.mode )
#endif
		{
			/* アクセルレベル設定 */
			if( FunSetAccessLevel( Fmsg ))
			{
				/* アクセルレベル設定　異常完了の場合は"error"を表示 */
				BlinkFlag->error = TRUE;
			}
			else
			{
				/* アクセルレベル設定　正常完了の場合は"done"を表示 */
				BlinkFlag->done = TRUE;
			}

		}
		RpxLedSetStr((UCHAR *)"P.", DspData, 4U, 2U);					/* <S148> */
		RpxLedSetValHex( pv->PswdData, DspData, 3U, 4U);				/* <S148> */
		RpxLedSetBlnk( DspData, (UCHAR)(1 << pv->DPos) );				/* <S148> */
		break;

/*--------------------------------------------------------------------------------------------------*/
	case ACS_SET_END: 													/* End state				*/
/*--------------------------------------------------------------------------------------------------*/
		FunAccessSetEnd( Fmsg );
		rc = FUN_END;
		break;
	default:
		break;
	}
	return (rc);
}
#endif


/****************************************************************************************************/
/*																									*/
/*			Fn010 Rewriting Inhibition Processing (LCD Operator) Set AccessLevel					*/
/*																									*/
/****************************************************************************************************/
LONG FunSetAccessLevel( FUNMSG *Fmsg )
{
	FNACSSETV 		*pv = (FNACSSETV *)(Fmsg->pVar);
	MSGIF 			*Hmsg = Fmsg->Super;
	LONG			rc = OK;
//	USHORT			syssw3;//<S0C7>
	AXIS_HANDLE*	Axis;
	PRMDATA			*Prm;
	REGIF_CONFIG_T	*RegCfg;
	FUN_AXCOMMON	*AxCommon;	/*<S1B8>*/

	Axis = Hmsg->pAxRsc;
	Prm = Axis->UniPrms->Prm;
	RegCfg = Axis->RegManager->hPnReg;
	AxCommon = Axis->FnManager->AxCommon;	/*<S1B8>*/

	switch(pv->PswdData)
	{
	case WRITE_OK_SET_VAL:	/* 書込許可 */
			/* EEPROMにアクセルレベルを設定( PnE0D bit0 = 0 : 書込許可) */
			Prm->syssw3 = Prm->syssw3 & (~0x0001);
			PrmStoreValueToEeprom( RegCfg, &pndef_syssw3, 0 );
		break;

	case WRITE_NG_SET_VAL:	/* 書込禁止 */
			/* EEPROMにアクセルレベルを設定( PnE0D bit0 = 1 : 書込禁止) */
			Prm->syssw3 = Prm->syssw3 | 0x0001;
			PrmStoreValueToEeprom( RegCfg, &pndef_syssw3, 0 );
		break;

	case ACCLVL_USER1_SET_VAL:	/* ユーザレベル1設定 */
			/* EEPROMにアクセルレベルを設定( PnE0D bit1 = 0 : ユーザレベル1 ) */
			Prm->syssw3 = Prm->syssw3 & (~0x0002);
			PrmStoreValueToEeprom( RegCfg, &pndef_syssw3, 0 );

			/* Message IF(LCD Operator)のアクセルレベル設定 */
			Hmsg->AccessLvl = ACCLVL_USER1;
			/* システムのアクセルレベル設定 */ /*<S1B8>*/
			AxCommon->AccessLevel = ACCLVL_USER1;
		break;

	case ACCLVL_USER2_SET_VAL:	/* ユーザレベル2設定 */
			///Prm->syssw3 = Prm->syssw3 | 0x0002;					/* <S1B0> */
			//PrmStoreValueToEeprom( RegCfg, &pndef_syssw3, 0 );	/* <S1B0> */

			/* Message IF(LCD Operator)のアクセルレベル設定 */
			Hmsg->AccessLvl = ACCLVL_USER2;
			/* システムのアクセルレベル設定 */ /*<S1B8>*/
			AxCommon->AccessLevel = ACCLVL_USER2;
		break;

	case ACCLVL_USER3_SET_VAL:	/* ユーザレベル3設定 */
			/* EEPROMにアクセルレベルを設定(  PnE0D bit1 = 1 : ユーザレベル3 ) */	/* <S1B0> */
			Prm->syssw3 = Prm->syssw3 | 0x0002;										/* <S1B0> */
			PrmStoreValueToEeprom( RegCfg, &pndef_syssw3, 0 );						/* <S1B0> */

			/* Message IF(LCD Operator)のアクセルレベル設定 */
			Hmsg->AccessLvl = ACCLVL_USER3;
			/* システムのアクセルレベル設定 */ /*<S1B8>*/
			AxCommon->AccessLevel = ACCLVL_USER3;
		break;

	case ACCLVL_SYSTEM_SET_VAL:	/* システムレベル設定 */
			/* Message IF(LCD Operator)のアクセルレベル設定 */
			Hmsg->AccessLvl = ACCLVL_SYSTEM;
			/* システムのアクセルレベル設定 */ /*<S1B8>*/
			AxCommon->AccessLevel = ACCLVL_SYSTEM;
		break;

	default:
		/* Do Nothing */
		rc = NG;
		break;
	}

	return (rc);
}

/***************************************** end of file **********************************************/
