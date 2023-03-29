/****************************************************************************************************/
/*																									*/
/*																									*/
/*		OpeMngr.c : Operation Management															*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1) Read Object Data																			*/
/*																									*/
/*		2) Search Obeject 																			*/
/*																									*/
/*		3) Write Object Data																		*/
/*																									*/
/*		4) Check Object Access Level																*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.1.00 : 2013.05.08  K.Ozaki																*/
/*		Rev.1.10 : 2013.09.26  K.Ozaki	<S00C>														*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "Operation.h"

/****************************************************************************************************/
/*																									*/
/*		Function Prototypes																			*/
/*																									*/
/****************************************************************************************************/

/*--------------------------------------------------------------------------------------------------*/
/*		Public Function																				*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxInitOpeManager( void );			/* Initialize Function 									*/
void	RpxPrstOpeManager( void );			/* Parameter Reset Function 							*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxFreeOpeRegWriteHandleAll( MSGIF *Hmsg );	/* <S166> Free All Operation Write Handle		*/
/*--------------------------------------------------------------------------------------------------*/
void	LpxWriteOpeEndRegister(  MSGIF *Hmsg );											/* <S212> 	*/
extern	MSGIF LopMsg;
extern	MSGIF PopMsg;
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		Private Function																			*/
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		<S166>																						*/
/*--------------------------------------------------------------------------------------------------*/
LONG	RpxOpeRegWrite(	COBJECT *this, LONG Index, ULONG Data, MSGIF *hMsg );
LONG	RpxOpeRegWriteS( COBJECT *this, LONG Index,  LONG SubIndex, ULONG Data,	MSGIF *hMsg );
static LONG LpxCheckOperationWriteHandle( COBJECT *this, LONG Index, MSGIF *hMsg );
static LONG	LpxGetOpeRegWriteHandle( COBJECT *this, _enum_OPMODE mode, MSGIF *Hmsg );
static void	LpxFreeOpeRegWriteHandle( COBJECT *this, _enum_OPMODE mode, MSGIF *Hmsg );
/*--------------------------------------------------------------------------------------------------*/


/****************************************************************************************************/
/*																									*/
/*		Private Macro Definition																	*/
/*																									*/
/****************************************************************************************************/
#define ACCESS_FREE 0
#define ACCESS_GET  1


/****************************************************************************************************/
/*																									*/
/*		Valiable Definition																			*/
/*																									*/
/****************************************************************************************************/
COBJECT LOperation[OPEREG_NUMBERX];
/*--------------------------------------------------------------------------------------------------*/






/****************************************************************************************************/
/*																									*/
/*		Initialize Function																			*/
/*																									*/
/****************************************************************************************************/
void	RpxInitOpeManager( void )
{
/*--------------------------------------------------------------------------------------------------*/
/*		Set	Table Entry Number																		*/
/*--------------------------------------------------------------------------------------------------*/
		COPEREG->TblEntNum = (USHORT)OPEREGTBL_ENTNUM;
		COPEREG->pTbl = (void *)OPEREGTBL_PTR;
/*--------------------------------------------------------------------------------------------------*/
		COPEREG->Read = RpxObjRead;
//		COPEREG->ReadX = RpxObjReadIdx;
		COPEREG->ReadS = RpxObjReadS;					/* <S073> */
		COPEREG->Search = RpxObjSearch;
		COPEREG->SearchB = RpxObjSearchIdx;
		COPEREG->ReadArray = RpxObjReadArray;
//<S166>		COPEREG->Write = RpxObjWrite;
//<S166>		COPEREG->WriteS = RpxObjWriteS;					/* <S073> */
		COPEREG->Write = RpxOpeRegWrite;				/* <S166> */
		COPEREG->WriteS = RpxOpeRegWriteS;				/* <S166> */
/*--------------------------------------------------------------------------------------------------*/
		COPEREG->GetNumber = RpxObjGetNumber;			/* Get Object Number */
		COPEREG->isLong = RpxObjChkLong;				/* Check Object is Long */
/*--------------------------------------------------------------------------------------------------*/
		COPEREG->SearchS = RpxObjSearchSub;				/* <S073> */
		COPEREG->SearchBS = RpxObjSearchIdxSub;			/* <S073> */
		COPEREG->WriteArray = RpxObjWriteArray;			/* <S073> */
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		Parameter Reset Function																	*/
/*																									*/
/****************************************************************************************************/
void	RpxPrstOpeManager( void )
{
		return;
}


/****************************************************************************************************/
/*																									*/
/*		Write Operation Register																	*/
/*																									*/
/****************************************************************************************************/
LONG	RpxOpeRegWrite(	COBJECT *this, LONG Index, ULONG Data, MSGIF *hMsg )
{
LONG rc;
AXIS_HANDLE		*pAxRsc = hMsg->pAxRsc;					/* 後で消す */
FUN_MANAGER 	*FunMngr = pAxRsc->FnManager;
/*--------------------------------------------------------------------------------------------------*/
	rc = LpxCheckOperationWriteHandle( this, Index, hMsg );		/* check write handle */
	if ( OK == rc ) { rc = RpxObjWrite( this, Index, Data, hMsg ); }	/* write register */
/*--------------------------------------------------------------------------------------------------*/
/* 	Free Handle 																					*/
/*--------------------------------------------------------------------------------------------------*/
	if( 0x000 == FunGetOpeModeRegister( FunMngr, FUN_CH1 ) )
	{
		LpxFreeOpeRegWriteHandle( COPEREG, OPMODE_OPE1, hMsg );
	}
	if( 0x000 == FunGetOpeModeRegister( FunMngr, FUN_CH2 ) )
	{
		LpxFreeOpeRegWriteHandle( COPEREG, OPMODE_OPE2, hMsg );
	}
	if( 0x000 == FunMngr->AxCommon->TraceMode )
	{
		LpxFreeOpeRegWriteHandle( COPEREG, OPMODE_TRACE, hMsg );
	}
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}

/****************************************************************************************************/
/*																									*/
/*		Write Operation Register																	*/
/*																									*/
/****************************************************************************************************/
LONG	RpxOpeRegWriteS( COBJECT *this, LONG Index,  LONG SubIndex, ULONG Data,	MSGIF *hMsg )
{
LONG rc;
AXIS_HANDLE		*pAxRsc = hMsg->pAxRsc;					/* 後で消す */
FUN_MANAGER 	*FunMngr = pAxRsc->FnManager;
/*--------------------------------------------------------------------------------------------------*/
	rc = LpxCheckOperationWriteHandle( this, Index, hMsg );				
	if ( OK == rc ) { rc = RpxObjWriteS( this, Index, SubIndex, Data, hMsg ); }	
/*--------------------------------------------------------------------------------------------------*/
/* 	Free Handle 																					*/
/*--------------------------------------------------------------------------------------------------*/
	if( 0x000 == FunGetOpeModeRegister( FunMngr, FUN_CH1 ) )
	{
		LpxFreeOpeRegWriteHandle( COPEREG, OPMODE_OPE1, hMsg );
	}
	if( 0x000 == FunGetOpeModeRegister( FunMngr, FUN_CH2 ) )
	{
		LpxFreeOpeRegWriteHandle( COPEREG, OPMODE_OPE2, hMsg );
	}
	if( 0x000 == FunMngr->AxCommon->TraceMode )
	{
		LpxFreeOpeRegWriteHandle( COPEREG, OPMODE_TRACE, hMsg );
	}
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}

/****************************************************************************************************/
/*																									*/
/*		操作レジスタ書込み権取得																	*/
/*																									*/
/****************************************************************************************************/
LONG	LpxGetOpeRegWriteHandle( COBJECT *this, _enum_OPMODE mode, MSGIF *Hmsg )
{
AXIS_HANDLE		*pAxRsc = Hmsg->pAxRsc;					/* 後で消す */
OPACCSV 		*Accs = pAxRsc->pAccessOp;				/* 後で消す */
/*--------------------------------------------------------------------------------------------------*/
/*		ハンドルチェック																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( Accs->Hmsg[mode] == NULL )
		{
			Accs->Hmsg[mode] = Hmsg;					/* ハンドルが未使用であれば獲得				*/
			return( TRUE );								/* ハンドル獲得成功							*/
		}
		else if( Accs->Hmsg[mode] == Hmsg )
		{
			return( TRUE );								/* ハンドルが同じであればＯＫ				*/
		}
/*--------------------------------------------------------------------------------------------------*/
		return( FALSE );								/* ハンドル獲得失敗							*/

}

/****************************************************************************************************/
/*																									*/
/*		Check Operation Write Handle																*/
/*																									*/
/****************************************************************************************************/
LONG LpxCheckOperationWriteHandle( COBJECT *this, LONG Index, MSGIF *hMsg )
{
_enum_OPMODE modex;
LONG	accsx;
COBJTBL *pObjTbl = GET_OBJTBL( this );
COBJDEF	*pObjDef = GET_OBJDEF( pObjTbl, Index );
/*--------------------------------------------------------------------------------------------------*/
/*		Check Operation Mode																		*/
/*--------------------------------------------------------------------------------------------------*/
	switch( pObjDef->RegNumber )
	{
	/*----------------------------------------------------------------------------------------------*/
		case 0x0000:											/* 0x2000:MODE1 */
	/*----------------------------------------------------------------------------------------------*/
			accsx = ACCESS_GET;
			modex = OPMODE_OPE1;
			break;

	/*----------------------------------------------------------------------------------------------*/
		case 0x0001:											/* 0x2001:MODE1 */
	/*----------------------------------------------------------------------------------------------*/
			accsx = ACCESS_GET;
			modex = OPMODE_OPE1;
			break;

	/*----------------------------------------------------------------------------------------------*/
		case 0x0004:											/* 0x2004:MODE2 */
	/*----------------------------------------------------------------------------------------------*/
			accsx = ACCESS_GET;
			modex = OPMODE_OPE2;
			break;

	/*----------------------------------------------------------------------------------------------*/
		case 0x0005:											/* 0x2005:MODE2 */
	/*----------------------------------------------------------------------------------------------*/
			accsx = ACCESS_GET;
			modex = OPMODE_OPE2;
			break;

	/*----------------------------------------------------------------------------------------------*/
		case 0x0006:											/* 0x2006:TRACE */
	/*----------------------------------------------------------------------------------------------*/
			accsx = ACCESS_GET;
			modex = OPMODE_TRACE;
			break;

	/*----------------------------------------------------------------------------------------------*/
		case 0x0007:											/* 0x2007:TRACE */
	/*----------------------------------------------------------------------------------------------*/
			accsx = ACCESS_GET;
			modex = OPMODE_TRACE;
			break;

	/*----------------------------------------------------------------------------------------------*/
		default:												/* OTHER */
	/*----------------------------------------------------------------------------------------------*/
			accsx = ACCESS_GET;
			modex = OPMODE_NULL;
			break;
	}
/*--------------------------------------------------------------------------------------------------*/
/*		Get Operation Register Access Handle														*/
/*--------------------------------------------------------------------------------------------------*/
	if ( modex != OPMODE_NULL )
	{
		if ( accsx == ACCESS_GET )
		{
			if ( LpxGetOpeRegWriteHandle( this, modex, hMsg ) != TRUE )
			{
				return (OERR_WRITE_HANDLE);					/* 他ch競合 */
			}
		}
		else if ( accsx == ACCESS_FREE )
		{
			LpxFreeOpeRegWriteHandle( this, modex, hMsg );
		}
	}
/*--------------------------------------------------------------------------------------------------*/
	return (OK);
}


/****************************************************************************************************/
/*																									*/
/*		操作レジスタ書込み権解放処理																*/
/*																									*/
/****************************************************************************************************/
void	LpxFreeOpeRegWriteHandle( COBJECT *this, _enum_OPMODE mode, MSGIF *Hmsg )
{
AXIS_HANDLE		*pAxRsc = Hmsg->pAxRsc;					/* 後で消す */
OPACCSV 		*Accs = pAxRsc->pAccessOp;				/* 後で消す */
/*--------------------------------------------------------------------------------------------------*/
/*		Free Operator Write Handle (Called from FunMngr)											*/
/*--------------------------------------------------------------------------------------------------*/
		if( Accs->Hmsg[mode] == Hmsg )
		{
			Accs->Hmsg[mode]   = NULL;					/* Free Register I/F Write Handle			*/
		}
/*--------------------------------------------------------------------------------------------------*/
		return;

}

/****************************************************************************************************/
/*																									*/
/* 		<S166> Free All Operation Write Handle														*/
/*																									*/
/****************************************************************************************************/
void	RpxFreeOpeRegWriteHandleAll( MSGIF *Hmsg )
{
	LpxFreeOpeRegWriteHandle( COPEREG, OPMODE_OPE1, Hmsg );
	LpxFreeOpeRegWriteHandle( COPEREG, OPMODE_OPE2, Hmsg );
	LpxFreeOpeRegWriteHandle( COPEREG, OPMODE_TRACE, Hmsg );

	LpxWriteOpeEndRegister( Hmsg );														/* <S212> 	*/

	return;
}


/****************************************************************************************************/
/*																									*/
/* 		<S212> 操作終了レジスタ書込み処理															*/
/*																									*/
/****************************************************************************************************/
void	LpxWriteOpeEndRegister(  MSGIF *Hmsg )
{
	AXIS_HANDLE			*Axis;
	FUN_MANAGER 		*FunMngr;
	CPFUNTBL			cFunTable1;
	CPFUNTBL			cFunTable2;
	LONG				ax;
	
	if(( Hmsg != &PopMsg )&&( Hmsg != &LopMsg ))
	{
		for( ax = 0 ; ax < MAX_AXIS_NUMBER ; ax++ )
		{
			Axis	= (AXIS_HANDLE*)Hmsg->AxisMap[ax].Axis;
			FunMngr	= Axis->FnManager;
			
			cFunTable1 = FunMngr->cFunMode[ FUN_CH1 ];
			if(( cFunTable1 != NULL )&&( NULL !=  cFunTable1->FunDefAttr->LeaveFunc ))
			{	/* Clear Opration mode. */
				cFunTable1->FunDefAttr->LeaveFunc( &FunMngr->FunExeState[ FUN_CH1 ], (void*)Axis );
				FunMngr->cFunMode[ FUN_CH1 ] = NULL;
				cFunTable1 = NULL;
			}
			
			cFunTable2 = FunMngr->cFunMode[ FUN_CH2 ];
			if(( cFunTable2 != NULL )&&( NULL !=  cFunTable2->FunDefAttr->LeaveFunc ))
			{	/* Clear Opration mode. */
				cFunTable2->FunDefAttr->LeaveFunc( &FunMngr->FunExeState[ FUN_CH2], (void*)Axis );
				FunMngr->cFunMode[ FUN_CH2 ] = NULL;
				cFunTable2 = NULL;
			}
			
			FunMngr->AxCommon->modeLock  = FALSE;				/* 操作ﾓｰﾄﾞﾛｯｸ解除						*/
			
			/* Unlock the register write access */
			if(( FunMngr->AxCommon->TraceMode == 0 )&&( cFunTable1 == NULL )&&( cFunTable2 == NULL ))
			{
				RegMngrFreeWriteOpeRegister( (REG_MANAGER_HANDLE*)FunMngr->RegManager );
			}
			
			if( Axis->TrcHndl != NULL )
			{
				DtrcWriteOpeModeTrcRegister( Axis->TrcHndl, 0 );
			}
		}
	}
}




/***************************************** end of file **********************************************/
