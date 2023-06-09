/****************************************************************************************************/
/*																									*/
/*																									*/
/*		MonMngr.c : Monitor Management Module														*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1) モニタデータ読み出し処理																	*/
/*																									*/
/*		2) モニタ変数クリア処理																		*/
/*																									*/
/*		3) モニタテーブルのインデックス検索処理														*/
/*																									*/
/*		4) モニタレジスタ読み出し処理(レジスタＩＦ)													*/
/*																									*/
/*																									*/
/*																									*/
/*	注意：モニタ管理テーブルを、UnMonTbl[] と UnRegTbl[] に分けたので注意すること (Rev.1.10)		*/
/*		  テーブルインデクスによる処理は、各テーブルで独立の処理とすること！！！					*/
/*			UnMonTbl[] : オペレータＩＦ用															*/
/*			UnRegTbl[] : レジスタＩＦ用(SigmaMessage,etc)											*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.1.00 : 2002.10.01  M.Kawatani															*/
/*		Rev.1.10 : 2005.03.04  T.Taniguchi															*/
/*		Rev.1.20 : 2009.06.03  T.Taniguchi															*/
/*		Rev.1.30 : 2011.04.06  K.Ozaki <0BJ14>														*/
/*		Rev.1.40 : 2013.09.26  K.Ozaki	<S00C>														*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "Monitor.h"



/****************************************************************************************************/
/*																									*/
/*		関数一覧																					*/
/*																									*/
/****************************************************************************************************/
void	RpxInitMonManager( void );			/* Monitor Manager Initialize							*/
void	RpxPrstMonManager( void );			/* Monitor Manager Parameter Reset						*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxMonClearVarIdx(					/* モニタ変数クリア処理									*/
		COBJECT *this,						/* this Pointer 										*/
		LONG	Index,						/* モニタ管理テーブルIndex								*/
		MSGIF *hMsg );						/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
/*		レジスタＩＦ																				*/
/*--------------------------------------------------------------------------------------------------*/
#if 0
LONG	RpxReadMonRegister(					/* モニタレジスタ読出し関数								*/
		LONG	RegNo,						/* レジスタ番号											*/
		LONG	Number,						/* 読み出しデータ個数									*/
		USHORT	*RdBuf,						/* 読み出しデータバッファポインタ						*/
		HMSGIF	Hmsg			);			/* メッセージIFハンドル									*/
/*--------------------------------------------------------------------------------------------------*/
LONG	LpxReadMonRegister(					/* モニタレジスタ読出し処理(インデクス指定)				*/
		LONG	Index,						/* モニタ管理テーブルIndex								*/
		ULONG	*pResData,					/* 読み出したモニタ値を格納するアドレス					*/
		ULONG	CheckType,					/* エラーチェックタイプ									*/
		HMSGIF	Hmsg			);			/* メッセージIFハンドル									*/
/*--------------------------------------------------------------------------------------------------*/
LONG	RpxMonSearchRegIdx(					/* モニタレジスタのインデックス検索処理（Round内提供）	*/
		LONG	RegNo,						/* レジスタ番号											*/
		LONG	*pFindIdx,					/* 検索結果インデックス									*/
		ULONG	CheckType		);			/* エラーチェックタイプ									*/
/*--------------------------------------------------------------------------------------------------*/
LONG	LpxMonSearchRegIdx(					/* モニタレジスタのインデックス検索処理					*/
		LONG	RegNo,						/* レジスタ番号											*/
		LONG	*pFindIdx		);			/* 検索結果インデックス									*/
/*--------------------------------------------------------------------------------------------------*/
#endif



/****************************************************************************************************/
/*																									*/
/*		Private Macro Definition																	*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Get UnMonitor Variable Address for Single/Multi Drive										*/
/*--------------------------------------------------------------------------------------------------*/
#ifndef CFG_GET_AXIS_VAR_ADR
#define LPX_GET_UNVAR_ADR( pVar )		(pVar)
#else
#define LPX_GET_UNVAR_ADR( pVar )		CFG_GET_AXIS_VAR_ADR( (pVar) )
#endif
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		Get Un Monitor Number	 																	*/
/*--------------------------------------------------------------------------------------------------*/
#define GET_UNMONNUMBER( tbl, idx )  ( (tbl)[(idx)].Number )


/****************************************************************************************************/
/*																									*/
/*		モニタ管理用変数定義																		*/
/*																									*/
/****************************************************************************************************/
CMONITOR LMonitor[MONITOR_NUMBERX];				/* Monitor Accesser Instance 						*/
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		Dummy																						*/
/*																									*/
/****************************************************************************************************/
LONG	LpxMonWriteDummy( void *this, LONG Index, ULONG data, MSGIF *hMsg ) { return OK; }



/****************************************************************************************************/
/*																									*/
/*		モニタ管理初期化処理																		*/
/*																									*/
/****************************************************************************************************/
void	RpxInitMonManager( void )
{
/*--------------------------------------------------------------------------------------------------*/
/*		Set	Table Entry Number																		*/
/*--------------------------------------------------------------------------------------------------*/
		CUNMONX->UnTblEntNum = UNMONTBL_ENTNUM;		    /* Set UnMonTbl Entry Number				*/
		CUNMONX->pUnMonTbl = UNMONTBL_PTR;				/* Set UnMonTbl Pointer						*/
/*--------------------------------------------------------------------------------------------------*/
/*		Set	Monitor Access Method 																	*/
/*--------------------------------------------------------------------------------------------------*/
		CUNMONX->Read  = RpxObjRead; 					/* Read UnMonitor */
//		CUNMONX->ReadX = RpxObjReadIdx; 				/* Read UnMonitor */
		CUNMONX->ReadS  = RpxObjReadS; 					/* <S073> Read UnMonitor */
		CUNMONX->Search = RpxObjSearch; 				/* Search Index (Binary Search) */
		CUNMONX->SearchB = RpxObjSearchIdx; 			/* Search Index (Linear Search) */
		CUNMONX->ReadArray = RpxObjReadArray;			/* Read UnMonitor(Array) */
		CUNMONX->Write = LpxMonWriteDummy;				/* Dummy Function */
		CUNMONX->WriteS = LpxMonWriteDummy;				/* <S073> Dummy Function */
/*--------------------------------------------------------------------------------------------------*/
		CUNMONX->Clear = RpxMonClearVarIdx; 			/* Clear Monitor */
/*--------------------------------------------------------------------------------------------------*/
		CUNMONX->GetNumber = RpxObjGetNumber;			/* Get Object Number */
		CUNMONX->isLong = RpxObjChkLong;				/* Check Object is Long */
/*--------------------------------------------------------------------------------------------------*/
		CUNMONX->SearchS = RpxObjSearchSub;				/* <S073> */
		CUNMONX->SearchBS = RpxObjSearchIdxSub;			/* <S073> */
		CUNMONX->WriteArray = RpxObjWriteArray;			/* <S073> */
#if MONITOR_NUMBERX >= 2 
/*--------------------------------------------------------------------------------------------------*/
/*		Set	Table Entry Number																		*/
/*--------------------------------------------------------------------------------------------------*/
		CUNREGX->UnTblEntNum = UNREGTBL_ENTNUM;		    /* Set UnRegTbl Entry Number				*/
		CUNREGX->pUnMonTbl = UNREGTBL_PTR;				/* Set UnRegTbl Pointer						*/
/*--------------------------------------------------------------------------------------------------*/
/*		Set	Monitor Access Method 																	*/
/*--------------------------------------------------------------------------------------------------*/
		CUNREGX->Read  = RpxObjRead; 					/* Read UnRegister */
//		CUNREGX->ReadX = RpxObjReadIdx; 				/* Read UnRegister */
		CUNREGX->ReadS  = RpxObjReadS; 					/* <S073> Read UnRegister */
		CUNREGX->Search = RpxObjSearch; 				/* Search Index */
		CUNREGX->SearchB = RpxObjSearchIdx;				/* Search Index */
		CUNREGX->ReadArray = RpxObjReadArray;			/* Read UnRegister(Array) */
		CUNREGX->Write = LpxMonWriteDummy;				/* Dummy Function */
		CUNREGX->WriteS = LpxMonWriteDummy;				/* <S073> Dummy Function */
		CUNREGX->Clear = RpxMonClearVarIdx; 			/* Clear Monitor */
/*--------------------------------------------------------------------------------------------------*/
		CUNREGX->GetNumber = RpxObjGetNumber;			/* Get Object Number */
		CUNREGX->isLong = RpxObjChkLong;				/* Check Object is Long */
/*--------------------------------------------------------------------------------------------------*/
		CUNREGX->SearchS = RpxObjSearchSub;				/* <S073> */
		CUNREGX->SearchBS = RpxObjSearchIdxSub;			/* <S073> */
		CUNREGX->WriteArray = RpxObjWriteArray;			/* <S073> */
#endif
		return;
}



/****************************************************************************************************/
/*																									*/
/*		モニタ管理パラメータリセット処理															*/
/*																									*/
/****************************************************************************************************/
void	RpxPrstMonManager( void )
{
		return;
}


/****************************************************************************************************/
/*																									*/
/*		モニタ変数クリア処理																		*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 指定インデックスのモニタ変数をゼロクリアする。											*/
/*																									*/
/*																									*/
/****************************************************************************************************/
void	RpxMonClearVarIdx(
		COBJECT *this,							/* this Pointer */
		LONG	Index,							/* モニタ管理テーブルIndex							*/
		MSGIF *hMsg )							/* Message Interface */
{
#if 0	/* <S144> */
LONG	rc;
CMONDEF	*pUnDef = GET_UNMONDEF( this->pTbl, Index ) 	/* UnMonTbl[Index].MonDefPtr */; 
/*--------------------------------------------------------------------------------------------------*/
/*		Check Clear Enable																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( CHK_UNCLEAR_ENABLE( pUnDef ) )
		{
			return;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		モニタ対象ＣＰＵのチェック																	*/
/*--------------------------------------------------------------------------------------------------*/
#if		CFG_MASTER_CPU == 1
		if( pUnDef->MtcIsComIf )
		{
			rc = DpiDegOpClearUnMonData( UnMonTbl[Index].UnNo );
			return;
		}
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		@@@ この処理はＮＧ : エクセル表を変更すると正常に動作しなくなる @@@							*/
/*--------------------------------------------------------------------------------------------------*/
//		if( Index == 0x0012 )			// Un013がクリアされる時は使用している余りをクリア
//		{										/* エクセル表を変更すると、Index値が変わる			*/
//			CoutRrst.RemCntr2 = 0;				/* このようなバグは、発見するのが難しい				*/
//		}										/* Indexや番号を直接使用したソフトは再利用出来ない	*/
/*--------------------------------------------------------------------------------------------------*/
/*		Clear Moniter Data																			*/
/*--------------------------------------------------------------------------------------------------*/
#if 0 /* TODO: Mercury 用に見直し */
		if( pUnDef->ClrVarFun )
		{
			pUnDef->FuncPtr( UNCAL_CLRVAR, pUnDef, NULL );
		}
		else if( pUnDef->IsLong )
		{
			*(ULONG *)LPX_GET_UNVAR_ADR( pUnDef->UnVar.Pulong ) = 0;
		}
		else
		{
			*(USHORT *)LPX_GET_UNVAR_ADR( pUnDef->UnVar.Pushort ) = 0;
		}
#endif
#else
LONG	pVal;
void	*ptr;
CMONDEF	*pUnDef = GET_UNMONDEF( this->pTbl, Index ) 	/* UnMonTbl[Index].MonDefPtr */; 
/*--------------------------------------------------------------------------------------------------*/
		if( CHK_UNCLEAR_ENABLE( pUnDef ) )
		{
			if( NULL != pUnDef->GetRamPtrCallback )
			{
				ptr = pUnDef->GetRamPtrCallback( Index, hMsg->pAxRsc );
				if( pUnDef->DataLength == 4 )
				{
					*(ULONG  *)(ptr) = 0;
				}
				else if( pUnDef->DataLength == 2 )
				{
					*(USHORT *)(ptr) = 0;
				}
				else if( pUnDef->DataLength == 1 )
				{
					*(UCHAR  *)(ptr) = 0;
				}
			}
			else
			{
				if( NULL != pUnDef->PrmCalCallback )
				{
					pUnDef->PrmCalCallback( PRM_ACCCMD_WRITE, Index, hMsg->pAxRsc, &pVal );
				}
			}
		}
#endif
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		モニタテーブルのインデックス検索処理														*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 指定されるモニタ番号からモニタテーブルのインデックスを検索する。						*/
/*			<0BJ14> ２分探索で検索を行う処理に変更。見つからなかった場合、*pFindIdx=0となります。	*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#if 0
LONG	RpiMonSearch(						/* 戻値 : ステータス									*/
		COBJECT *this,						/* this Pointer 										*/
		LONG	SrchNo,						/* モニタ番号											*/
		LONG	BaseIdx,					/* 検索開始インデックス									*/
		LONG	*pFindIdx,					/* 検索結果インデックス									*/
		ULONG	CheckType,					/* エラーチェックタイプ									*/
		HMSGIF	Hmsg		)				/* メッセージIFハンドル									*/
{
CMONTBL *pUnMonTbl = GET_UNMONTBL( this );
	if ( LpxSearchUnMonTblIdx( 
						pUnMonTbl /* &UnMonTbl[0] */, 
						this->TblEntNum  /* MonMngP.UnMonTblEntNum */, 
						SrchNo, 
						pFindIdx ) == OK ) 
	{
/*--------------------------------------------------------------------------------------------------*/
/*		インデクスチェック																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( *pFindIdx >= this->UnTblEntNum /* (LONG)MonMngP.UnMonTblEntNum */ )
		{
			;				/* モニタ番号エラー										*/
		}
		else
/*--------------------------------------------------------------------------------------------------*/
		if ( LpxMonCheckAttrIdx( GET_UNMONDEF( pUnMonTbl, *pFindIdx ), *pFindIdx, CheckType, Hmsg ) == OK ) {
			return (OK);
		}
	}
	*pFindIdx = 0; 
	return (NG); 								/* return (MERR_NO); */
}
#endif


/****************************************************************************************************/
/*																									*/
/*		モニタテーブルのインデックス検索処理														*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 指定されるモニタ番号からモニタテーブルのインデックスを検索する。						*/
/*			該当番号が見付からない場合は、条件に該当する次のIndexを戻す。							*/
/*			<0BJ14> コード整理。																	*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#if 0
USHORT	RpxMonSearchIdxOp(					/* 戻値 : 検索結果インデックス							*/
		COBJECT *this,						/* this Pointer 										*/
		LONG	SrchNo,						/* モニタ番号											*/
		LONG	BaseIdx,					/* 検索開始インデックス									*/
		HMSGIF	Hmsg		)				/* メッセージIFハンドル									*/
{
	return (USHORT)LpxSearchUnMonTblNextIdx( 
					GET_UNMONTBL( this ) /* &UnMonTbl[0] */, 
					this->UnTblEntNum,
					SrchNo, 
					BaseIdx, 
					Hmsg );
}
#endif


/****************************************************************************************************/
/*																									*/
/*		モニタ管理テーブル検索																		*/
/*																									*/
/****************************************************************************************************/
#if		CFG_MASTER_CPU == 1
LONG	LpxSearchUnMonTblMdef(				/* モニタ管理テーブル検索								*/
		CMONDEF	*pUnDef,					/* モニタ定義											*/
		LONG	*pFindIdx		)			/* 検索結果インデックス									*/
{
LONG	i;
/*--------------------------------------------------------------------------------------------------*/
/*		Search UnMonTbl[]																			*/
/*--------------------------------------------------------------------------------------------------*/
		for( i=0; i < MonMngP.UnMonTblEntNum; i++ )
		{
			if( pUnDef == UnMonTbl[i].MonDefPtr )
			{
				*pFindIdx = i; return( OK );
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		return( NG );
}
#endif

/****************************************************************************************************/
/*																									*/
/*		モニタ管理テーブル検索(モニタ番号指定)														*/
/*																									*/
/****************************************************************************************************/
#if 0
LONG	LpxSearchUnMonTblIdx(				/* モニタ管理テーブル検索(モニタ番号指定)				*/
		CMONTBL *MonTbl,					/* 検索モニタ定義										*/
		LONG	Size,						/* モニタテーブルサイズ									*/
		LONG	UnNumber,					/* Unモニタ番号											*/
		LONG	*pFindIdx		)			/* 検索結果インデックス									*/
{
LONG is;	/* start index */
LONG ie;	/* end index */
LONG in;	/* intermediate index */
/*--------------------------------------------------------------------------------------------------*/
	is=0;
	ie=Size-1;
/*--------------------------------------------------------------------------------------------------*/
	while (TRUE)  /* binary search */
	{
		if (ie < is) { return (NG); }
		in = (is+ie)>>1;
		if (UnNumber == GET_UNMONNUMBER( MonTbl, in ) /* MonTbl[in].UnNo */ ){ *pFindIdx = in; return (OK); }
		if (UnNumber < GET_UNMONNUMBER( MonTbl, in ) /* MonTbl[in].UnNo */){ ie = in-1; }
		else { is = in+1; }
	}
/*--------------------------------------------------------------------------------------------------*/
}
#endif
/****************************************************************************************************/
/*																									*/
/*		モニタ管理テーブル検索(モニタ番号指定。見つからなければ、次のINDEXを探す)					*/
/*																									*/
/****************************************************************************************************/
#if 0
LONG	LpxSearchUnMonTblNextIdx(			/* モニタ管理テーブル検索(モニタ番号指定)				*/
		CMONTBL	*MonTbl,					/* 検索モニタ定義テーブル								*/
		LONG	EntNum,						/* Table Entry Number									*/
		LONG	UnNo,						/* モニタ番号											*/
		LONG	StIdx,						/* 検索開始インデックス									*/
		HMSGIF	Hmsg)						/* メッセージIFハンドル									*/
{
#define UNSEARCH_FORWARD (1)
#define UNSEARCH_BACKWARD (-1)
LONG idx;
LONG rtIdx; /* return index */
LONG dir; /* Search direction */
/*--------------------------------------------------------------------------------------------------*/
	rtIdx = StIdx;
/*--------------------------------------------------------------------------------------------------*/
	if ( UnNo >= GET_UNMONNUMBER( MonTbl, StIdx ) /* UnMonTbl[StIdx].UnNo */ )
	{ /* forward search */
		dir = UNSEARCH_FORWARD;
	}
	else
	{ /* backword search */
		dir = UNSEARCH_BACKWARD;
	}
/*--------------------------------------------------------------------------------------------------*/
	for ( idx = StIdx; (idx < EntNum /* MonMngP.UnMonTblEntNum */) && (idx >= 0); idx = idx + dir )
	{
		if ( LpxMonCheckAttrIdx( GET_UNMONDEF( MonTbl, idx ), idx, MCHK_OPE_READ, Hmsg ) != OK ) { continue; }
		if ( GET_UNMONNUMBER( MonTbl, idx ) /* MonTbl[idx].UnNo */ > MONDEF_MAX_UnNO ){ break; }
		/*------------------------------------------------------------------------------------------*/
		rtIdx = idx; /* visible index */
		if ( (dir == UNSEARCH_FORWARD ) && ( GET_UNMONNUMBER( MonTbl, idx ) /* MonTbl[idx].UnNo */ >= UnNo ) ){ break; }
		else if ( (dir == UNSEARCH_BACKWARD ) && ( GET_UNMONNUMBER( MonTbl, idx ) /* MonTbl[idx].UnNo */ <= UnNo ) ){ break; }
		/*------------------------------------------------------------------------------------------*/
	}
/*--------------------------------------------------------------------------------------------------*/
	return rtIdx;
}
#endif

/****************************************************************************************************/
/*																									*/
/*		モニタ属性のチェック処理(モニタ定義指定)													*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : CheckTypeより決まる条件に従って、指定したモニタが妥当かチェックする。					*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#if 0
LONG	LpxMonCheckAttrMdef(
		CMONDEF	*pUnDef,					/* モニタ定義											*/
		ULONG	CheckType,					/* チェックタイプ										*/
		HMSGIF	Hmsg		)				/* メッセージIFハンドル									*/
{
/*--------------------------------------------------------------------------------------------------*/
/*		Check AccessLevel																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( (CheckType & MCHK_ACCS_LVL) && (Hmsg != NULL) )
		{
			if(	(pUnDef->AccessLevel) > Hmsg->AccessLvl )
			{
				return( MERR_LEVEL );
			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Check Special Use																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( CheckType & MCHK_SP_USE )
		{
			if( CHK_UNSPUSE() )
			{
				return( MERR_CHKSPUSE );
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		return( OK );
}
#endif


/****************************************************************************************************/
/*																									*/
/*		モニタ属性のチェック処理																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : CheckTypeより決まる条件に従って、指定したモニタが妥当かチェックする。					*/
/*			UnMonTbl用																				*/
/*																									*/
/****************************************************************************************************/
#if 0
LONG	LpxMonCheckAttrIdx(					/* 戻値 : ステータス									*/
		CMONDEF	*pUnDef,					/* モニタ定義											*/
		LONG	Index,						/* パラメータ管理テーブルIndex							*/
		ULONG	CheckType,					/* エラーチェックタイプ									*/
		HMSGIF	Hmsg		)				/* メッセージIFハンドル									*/
{
LONG	rc = OK ;
/*--------------------------------------------------------------------------------------------------*/
/*		インデクスチェック																			*/
/*--------------------------------------------------------------------------------------------------*/
#if 0
		if( Index >= (LONG)MonMngP.UnMonTblEntNum )
		{
			return( MERR_NO );				/* モニタ番号エラー										*/
		}
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		モニタ属性チェック																			*/
/*--------------------------------------------------------------------------------------------------*/
		rc = LpxMonCheckAttrMdef( pUnDef /* UnMonTbl[Index].MonDefPtr */, CheckType, Hmsg );
/*--------------------------------------------------------------------------------------------------*/
		return( rc );
}
#endif



/****************************************************************************************************/
/*																									*/
/*		モニタレジスタ読出し関数																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 指定されたレジスタ番号から指定個数のレジスタデータを読み出す。							*/
/*																									*/
/*	注 意 : 上位からのモニタレジスタのサイズ指定(SHORT/LONG)はない									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#if 0
LONG	RpxReadMonRegister(
		LONG	RegNo,						/* レジスタ番号											*/
		LONG	Number,						/* 読出しデータ個数										*/
		USHORT	*RdBuf,						/* 読出しデータバッファポインタ							*/
		HMSGIF	Hmsg		)				/* メッセージIFハンドル									*/
{
LONG	i;
LONG	j;
ULONG	UnData;
LONG	FindIndex;
LONG	RegNoX = RegNo & 0x0FFF;
/*--------------------------------------------------------------------------------------------------*/
/*		レジスタ番号からモニタインデックス取得														*/
/*--------------------------------------------------------------------------------------------------*/
		if( LpxMonSearchRegIdx( RegNoX, &FindIndex ) != OK )
		{
			Hmsg->ErrRno = (USHORT)RegNo;					/* 異常レジスタ番号						*/
			return( REGIF_REGNOERR );						/* レジスタ番号異常(開始REG番号)		*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		指定レジスタ番号から指定サイズ読み込み														*/
/*--------------------------------------------------------------------------------------------------*/
		for( j=FindIndex, i=0; i < Number; j++ )
		{
		/*------------------------------------------------------------------------------------------*/
		/*		Read Register																		*/
		/*------------------------------------------------------------------------------------------*/
			if( LpxReadMonRegister( j, &UnData, MCHK_REG_READ, Hmsg ) != OK )
			{
				Hmsg->ErrRno = (USHORT)(RegNo + i);			/* 異常レジスタ番号						*/
				return( REGIF_REGNOERR );					/* レジスタ番号異常						*/
			}
		/*------------------------------------------------------------------------------------------*/
		/*		Check RegNo																			*/
		/*------------------------------------------------------------------------------------------*/
			if( UnRegTbl[j].UnNo != (RegNoX + i) )
			{
				Hmsg->ErrRno = (USHORT)(RegNo + i);			/* 異常レジスタ番号						*/
				return( REGIF_REGNOERR );					/* レジスタ番号異常						*/
			}
		/*------------------------------------------------------------------------------------------*/
		/*		SHORT Register																		*/
		/*------------------------------------------------------------------------------------------*/
			if( UnRegTbl[j].MonDefPtr->IsLong != TRUE )
			{
				RdBuf[i++] = (USHORT)UnData;				/* 読出しデータ							*/
			}
		/*------------------------------------------------------------------------------------------*/
		/*		LONG  Register																		*/
		/*------------------------------------------------------------------------------------------*/
			else
			{
				if( (i+1) < Number )
				{
					RdBuf[i+0] = (USHORT)UnData;			/* 読出しデータ(下位ワード)				*/
					RdBuf[i+1] = (USHORT)(UnData >> 16);	/* 読出しデータ(上位ワード)				*/
					i = i + 2;
				}
				else
				{
					Hmsg->ErrRno = (USHORT)(RegNo + i);		/* 異常レジスタ番号						*/
					return( REGIF_DAMTHERR );				/* データ整合異常(LONG境界)				*/
				}
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		return( OK );
}
#endif



/****************************************************************************************************/
/*																									*/
/*		モニタレジスタ読出し																		*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 指定されたモニタデータを１個読み出す。													*/
/*																									*/
/*	注 意 : 上位からのモニタレジスタのサイズ指定(SHORT/LONG)はない									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#if 0
LONG	LpxReadMonRegister(					/* モニタレジスタ読出し									*/
		LONG	Index,						/* モニタ管理テーブルIndex								*/
		ULONG	*pResData,					/* 読出したモニタ値を格納するアドレス					*/
		ULONG	CheckType,					/* エラーチェックタイプ									*/
		HMSGIF	Hmsg		)				/* メッセージIFハンドル									*/
{
LONG	rc;
CMONDEF	*pUnDef;
/*--------------------------------------------------------------------------------------------------*/
/*		Check RegTblIndex																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( Index >= MonMngP.UnRegTblEntNum )
		{
			return( REGIF_REGNOERR );				/* レジスタ番号異常(開始REG番号)				*/
		}
		pUnDef = UnRegTbl[Index].MonDefPtr;
/*--------------------------------------------------------------------------------------------------*/
/*		モニタ属性のチェック																		*/
/*--------------------------------------------------------------------------------------------------*/
		rc = LpxMonCheckAttrMdef( pUnDef, CheckType, Hmsg ) ;
		if( rc != OK )
		{
			if( rc == MERR_CHKSPUSE )
			{
				*pResData = 0;						/* 読出しデータ									*/
				return( OK );						/* 正常リターンとする							*/
			}
			else
			{
				return( REGIF_REGNOERR );			/* レジスタ番号異常								*/
			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		モニタ対象ＣＰＵのチェック																	*/
/*--------------------------------------------------------------------------------------------------*/
#if		CFG_MASTER_CPU == 1
		if( pUnDef->MtcIsComIf )
		{
			LONG RdByteSize = ( pUnDef->IsLong )? 4 : 2;
			rc = DpiDegOpReadOneRegister( (UnRegTbl[Index].UnNo | 0xE000), RdByteSize, pResData );
			return( rc );
		}
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		Read Monitor Data																			*/
/*--------------------------------------------------------------------------------------------------*/
		*pResData = pUnDef->FuncPtr( UNCAL_MONCAL, pUnDef, Hmsg );
/*--------------------------------------------------------------------------------------------------*/
		return( OK );
}
#endif



/****************************************************************************************************/
/*																									*/
/*		モニタレジスタのインデックス検索処理（Round内提供）											*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 指定されたレジスタ番号のモニタレジスタのインデックスを検索する。						*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#if 0
LONG	RpxMonSearchRegIdx(					/* 戻値 : ステータス									*/
		LONG	RegNo,						/* レジスタ番号											*/
		LONG	*pFindIdx,					/* 検索結果インデックス									*/
		ULONG	CheckType		)			/* エラーチェックタイプ									*/
{
LONG	rc;
/*--------------------------------------------------------------------------------------------------*/
/*		RegNo検索																					*/
/*--------------------------------------------------------------------------------------------------*/
		rc = LpxMonSearchRegIdx( RegNo, pFindIdx );
		if( rc != OK ){ return( REGIF_REGNOERR );}
/*--------------------------------------------------------------------------------------------------*/
/*		属性チェック																				*/
/*--------------------------------------------------------------------------------------------------*/
		rc = LpxMonCheckAttrMdef( UnRegTbl[*pFindIdx].MonDefPtr, CheckType, NULL );
		if( rc != OK ){ return( REGIF_REGNOERR );}
/*--------------------------------------------------------------------------------------------------*/
		return( OK );
}
#endif



/****************************************************************************************************/
/*																									*/
/*		モニタレジスタのインデックス検索処理														*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 指定されたレジスタ番号のモニタレジスタのインデックスを検索する。						*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#if 0
LONG	LpxMonSearchRegIdx(					/* 戻値 : ステータス									*/
		LONG	RegNo,						/* レジスタ番号											*/
		LONG	*pFindIdx		)			/* 検索結果インデックス									*/
{
LONG	i,w;
LONG	EntNum = MonMngP.UnRegTblEntNum;
/*--------------------------------------------------------------------------------------------------*/
/*		検索開始Indexの設定																			*/
/*--------------------------------------------------------------------------------------------------*/
		w = (1 << MlibSrhbiton( EntNum - 1, SRH_FROM_MSB ));	/* 分割幅初期値設定					*/
		i = ( RegNo >= UnRegTbl[w].UnNo )? (EntNum - w) : 0;	/* 検索開始Index設定				*/
/*--------------------------------------------------------------------------------------------------*/
/*		モニタテーブル検索(２分探索)																*/
/*--------------------------------------------------------------------------------------------------*/
		for( w = w>>1; w > 0; w = w>>1 )						/* 検索Loop							*/
		{
			if( RegNo >= UnRegTbl[i+w].UnNo ){ i = i + w;}		/* 比較＆Index更新					*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		検索結果処理																				*/
/*--------------------------------------------------------------------------------------------------*/
		if( RegNo != UnRegTbl[i+w].UnNo )
		{
			return( NG );										/* 番号不一致、エラーリターン		*/
		}
		else
		{
			*pFindIdx = i;										/* 検索結果インデクス格納			*/
		}
/*--------------------------------------------------------------------------------------------------*/
		return( OK );
}
#endif







/***************************************** end of file **********************************************/
