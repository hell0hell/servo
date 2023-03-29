/****************************************************************************************************/
/*																									*/
/*																									*/
/*		PrmMngr.c : パラメータ管理モジュール														*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*	機 能 :	ドライブソフトウェアの全パラメータの管理を行う。										*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.1.00 : 2002.08.21  K.Hamasuna															*/
/*		Rev.1.10 : 2008.04.24  T.Taniguchi															*/
/*		Rev.1.20 : 2011.04.06  K.Ozaki <0BJ14>														*/
/*		Rev.1.30 : 2013.05.08  K.Ozaki 																*/
/*		Rev.1.40 : 2013.09.26  K.Ozaki <S00C>														*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "Parameter.h"		/* #include "Round.h" */



/****************************************************************************************************/
/*																									*/
/*		Global Variable Declaration																	*/
/*																									*/
/****************************************************************************************************/
CPARAM	CParam[CPARAM_ENT_NUM];						/* Parameter Accessor Instance */
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		Private Macro Definition																	*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Parameter Number Define																		*/
/*--------------------------------------------------------------------------------------------------*/
#define	ENC_PRM_NUM						0x0f00
#define ENC_PRM_WRT_DISABLE				0
/*--------------------------------------------------------------------------------------------------*/
/*		Long Parameter Operation Macro for Big/Little Endian										*/
/*--------------------------------------------------------------------------------------------------*/
/*		Macro Argument     :   lwd    hwd															*/
/*		LongPrm Word Align : {LoNum, HiNum} : ULONGOF_REGDATA( (hwd<<16) + lwd ) <-- SGDV			*/
/*		LongPrm Word Align : {HiNum, LoNum} : ULONGOF_REGDATA( (lwd<<16) + hwd ) <-- ????			*/
/*--------------------------------------------------------------------------------------------------*/
#if		CCSW_BIG_ENDIAN == 1
#define GET_LWD_REGDATA( xlong )		(USHORT)((xlong)>>0 )
#define GET_HWD_REGDATA( xlong )		(USHORT)((xlong)>>16)
#define ULONGOF_REGDATA( lwd, hwd )		(ULONG)((((USHORT)(hwd))<<16) + (USHORT)(lwd))
#else
#define GET_LWD_REGDATA( xlong )		(USHORT)((xlong)>>0 )
#define GET_HWD_REGDATA( xlong )		(USHORT)((xlong)>>16)
#define ULONGOF_REGDATA( lwd, hwd )		(ULONG)((((USHORT)(hwd))<<16) + (USHORT)(lwd))
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		Get Parameter Variable Address for Single/Multi Drive										*/
/*--------------------------------------------------------------------------------------------------*/
#if 1		/* Mercury */
#define LPX_SET_PRMVAR_RAM( pndef, data, hmsg, save ) {                                             \
		if( NULL != (pndef)->GetRamPtrCallback )                                                    \
		{                                                                                           \
		void *ptr = (pndef)->GetRamPtrCallback( 0, hmsg->pAxRsc );                                  \
				switch (((pndef)->DataLength) )                                                     \
				{                                                                                   \
				case 4:                                                                             \
					(save) = _OBJ_LONG( ptr, (pndef)->Sign );                                       \
					*(ULONG *)(ptr) = (data);                                                       \
					break;                                                                          \
				case 2:                                                                             \
					(save) = _OBJ_SHORT( ptr, (pndef)->Sign );                                      \
					*(USHORT *)(ptr) = (data);                                                      \
					break;                                                                          \
				default:                                                                            \
					(save) = _OBJ_CHAR( ptr, (pndef)->Sign );                                       \
					*(UCHAR *)(ptr) = (data);                                                       \
					break;                                                                          \
				}                                                                                   \
		}                                                                                           \
}
#endif
#if 0		/* Jupiter */
#ifndef CFG_GET_AXIS_VAR_ADR
#define LPX_GET_PRMVAR_ADR( pVar )		((USHORT*)(pVar))
#else
#define LPX_GET_PRMVAR_ADR( pVar )		((USHORT*)CFG_GET_AXIS_VAR_ADR( (pVar) ))
#endif

#define LPX_SET_PRMVAR_RAM( pndef, data, hmsg, save ) {                                             \
		pPrmVar = LPX_GET_PRMVAR_ADR( pRealDef->RamPtr );		/* 変数アドレス取得					*/
		if( pRealDef->Attr.VarIsLong )							/* LONGパラメータの場合 			*/
		{
			PrmSave = *(ULONG*)pPrmVar;							/* LONGデータ退避					*/
			*(ULONG*)pPrmVar = PrmData;							/* LONGデータ書込み					*/
		}
		else
		{
			PrmSave = *(USHORT*)pPrmVar;						/* SHORTデータ退避					*/
			*(USHORT*)pPrmVar = (USHORT)PrmData;				/* SHORTデータ書込み				*/
		}
}
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		Restore Parameter Variable 																	*/
/*--------------------------------------------------------------------------------------------------*/
#define LPX_RESTORE_PARAMETER( pndef, hmsg, save ) {                                                \
		if( NULL != (pndef)->GetRamPtrCallback )                                                    \
		{                                                                                           \
		void *ptr = (pndef)->GetRamPtrCallback( 0, hmsg->pAxRsc );                                  \
				switch (((pndef)->DataLength) )                                                     \
				{                                                                                   \
				case 4:                                                                             \
					*(ULONG *)(ptr) = (save);                                                       \
					break;                                                                          \
				case 2:                                                                             \
					*(USHORT *)(ptr) = (save);                                                      \
					break;                                                                          \
				default:                                                                            \
					*(UCHAR *)(ptr) = (save);                                                       \
					break;                                                                          \
				}                                                                                   \
		}                                                                                           \
}
#if 0 /* Jupiter */
			if( pRealDef->Attr.VarIsLong )
			{
				*(ULONG*)pPrmVar = PrmSave;						/* LONGデータ復旧					*/
			}
			else
			{
				*(USHORT*)pPrmVar = (USHORT)PrmSave;			/* SHORTデータ復旧					*/
			}
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		 Parameter Calculation Function 															*/
/*--------------------------------------------------------------------------------------------------*/
#define LPX_CALC_PARAMETER( pndef, rc, hmsg ) {                                                     \
		if ( NULL != (pndef)->PrmCalCallback )                                                      \
		{                                                                                           \
			(rc) = ((((pndef)->PrmCalCallback)( PRM_ACCCMD_WRITE,                                   \
						0 /* @@array index@@ */,                                                    \
						hmsg->pAxRsc, &PrmData ) >= 0) ? TRUE : FALSE );                            \
		}                                                                                           \
		else                                                                                        \
		{                                                                                           \
			(rc) = TRUE;                                                                            \
		}                                                                                           \
}

/*--------------------------------------------------------------------------------------------------*/
/*		Check the parameter is long or not		 													*/
/*--------------------------------------------------------------------------------------------------*/
#define CHK_PNPRM_ISLONG( pndef )   (((pndef)->DataLength) == 4 ? TRUE : FALSE)

/*--------------------------------------------------------------------------------------------------*/
/*		Adjust Operator Input <S149>																*/
/*--------------------------------------------------------------------------------------------------*/
#define LPX_ADJOPEINC( pndef, aidx, rc, hmsg ) {                                                    \
		if ( NULL != (pndef)->PrmCalCallback )                                                      \
		{                                                                                           \
			(rc) = ((pndef)->PrmCalCallback)( PRM_ACCCMD_ADJINCDATA,                                \
						(aidx) ,	                                                                \
						hmsg->pAxRsc, &(rc) );                                                      \
		}                                                                                           \
		else                                                                                        \
		{                                                                                           \
			(rc) = 0;                                                                               \
		}                                                                                           \
}

/*--------------------------------------------------------------------------------------------------*/
/*		Adjust Operator Input <S149>																*/
/*--------------------------------------------------------------------------------------------------*/
#define LPX_ADJOPEDEC( pndef, aidx, rc, hmsg ) {                                                    \
		if ( NULL != (pndef)->PrmCalCallback )                                                      \
		{                                                                                           \
			(rc) = ((pndef)->PrmCalCallback)( PRM_ACCCMD_ADJDECDATA,                                \
						(aidx) ,	                                                                \
						hmsg->pAxRsc, &(rc) );                                                      \
		}                                                                                           \
		else                                                                                        \
		{                                                                                           \
			(rc) = 0;                                                                               \
		}                                                                                           \
}


/****************************************************************************************************/
/*																									*/
/*		Function List																				*/
/*																									*/
/****************************************************************************************************/

/*--------------------------------------------------------------------------------------------------*/
/*		Public Functions																			*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxInitPrmManager( void );			/* パラメータ管理初期化処理								*/
void	RpxPrstPrmManager( void );			/* パラメータ管理パラメータリセット処理					*/

/*--------------------------------------------------------------------------------------------------*/
/*		Public Functions																			*/
/*--------------------------------------------------------------------------------------------------*/
LONG	RpiPrmWriteRam(						/* Write a Parameter (RAM)								*/
		CPARAM  *this,						/* this Pointer											*/
		CPRMDEF *pPrmDef,					/* Pointer to the PrmDef								*/
		ULONG	Data,						/* Parameter Value 										*/
		HMSGIF	Hmsg );						/* Message Interface									*/
/*--------------------------------------------------------------------------------------------------*/
LONG	RpxPrmDigitInc(						/* Increment Parameter Digit (RAM)						*/
		CPARAM  *this,						/* this Pointer											*/
		LONG	Index,						/* Parameter Index										*/
		ULONG	Digit,						/* Digit(0~)											*/
		MSGIF  *hMsg );						/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
LONG	RpxPrmDigitDec(						/* Decrement Parameter Digit (RAM)						*/
		CPARAM  *this,						/* this Pointer											*/
		LONG	Index,						/* Parameter Index										*/
		ULONG	Digit,						/* Digit(0~)											*/
		MSGIF  *hMsg );						/* Message Interface 									*/

/*--------------------------------------------------------------------------------------------------*/
/*		Private Functions																			*/
/*--------------------------------------------------------------------------------------------------*/
LONG	LpxPrmLimitCheckPdef(				/* パラメータの上下限チェック処理						*/
		CPRMDEF *pPrmDef,					/* パラメータ定義ポインタ								*/
		ULONG	Data			);			/* データ												*/
/*--------------------------------------------------------------------------------------------------*/
LONG	LpxPrmWriteRam(						/* パラメータ書き込み処理								*/
		CPRMDEF *pRealDef,					/* パラメータ定義ポインタ								*/
		ULONG	PrmData,					/* 書き込むパラメータ値									*/
		HMSGIF	Hmsg			);			/* メッセージハンドル									*/
/*--------------------------------------------------------------------------------------------------*/
ULONG	LpxPrmDataDigitInc(					/* データ値の桁インクリメント							*/
		ULONG	Data,						/* 元のデータ値											*/
		ULONG	Digit,						/* 指定桁(0～)											*/
		ULONG	UpperLimit,					/* 最大値												*/
		ULONG	Base,						/* データの底(DSPDEF_BASENBL, BASEDEC, BASEHEX)			*/
		ULONG	IsLong,						/* ロングフラグ(TRUE, FALSE)							*/
		ULONG	Sign			);			/* 符号有無(DSPDEF_W_SIGN, NOSIGN)						*/
/*--------------------------------------------------------------------------------------------------*/
ULONG	LpxPrmDataDigitDec(					/* データ値の桁デクリメント								*/
		ULONG	Data,						/* 元のデータ値											*/
		ULONG	Digit,						/* 指定桁(0～)											*/
		ULONG	LowerLimit,					/* 最小値												*/
		ULONG	Base,						/* データの底(DSPDEF_BASENBL, BASEDEC, BASEHEX)			*/
		ULONG	IsLong,						/* ロングフラグ(TRUE, FALSE)							*/
		ULONG	Sign			);			/* 符号有無(DSPDEF_W_SIGN, NOSIGN)						*/
/*--------------------------------------------------------------------------------------------------*/
USHORT	RpxPrmGetDecimals(					/* <S14F> 												*/
		CPARAM  *this,						/* this Pointer											*/
		CPRMDEF	*prmdef,					/* パラメータ定義へのポインタ							*/
		MSGIF	*hMsg			);			/* メッセージハンドル									*/
/*--------------------------------------------------------------------------------------------------*/





/****************************************************************************************************/
/*																									*/
/*		パラメータ管理初期化処理																	*/
/*																									*/
/****************************************************************************************************/
void	RpxInitPrmManager( void )
{
/*--------------------------------------------------------------------------------------------------*/
/*		Set	Table Entry Number																		*/
/*--------------------------------------------------------------------------------------------------*/
		CPARAMX->PrmTblEntNum = PRMTBL_ENTNUM;		    /* Set PrmTbl Entry Number					*/
		CPARAMX->pPrmTbl = PRMTBL_PTR;					/* Set PrmTbl Pointer						*/
/*--------------------------------------------------------------------------------------------------*/
/*		Set	Parameter Access Method 																*/
/*--------------------------------------------------------------------------------------------------*/
		CPARAMX->Read  = RpxObjRead; 					/* TODO: */
//		CPARAMX->ReadX = RpxObjReadIdx; 				/* TODO: */
		CPARAMX->ReadS  = RpxObjReadS; 					/* <S073> */
		CPARAMX->Search = RpxObjSearch; 				/* TODO: */
		CPARAMX->SearchB = RpxObjSearchIdx; 			/* TODO: */
		CPARAMX->ReadArray = RpxObjReadArray; 			/* TODO: */
		CPARAMX->Write = RpxObjWrite;					/* Write Ram Parameter */
		CPARAMX->WriteS = RpxObjWriteS;					/* <S073> */ 
/*--------------------------------------------------------------------------------------------------*/
		CPARAMX->DigitInc = RpxPrmDigitInc;				/* Increment Ram Parameter */
		CPARAMX->DigitDec = RpxPrmDigitDec;				/* Decrement Ram Parameter */
/*--------------------------------------------------------------------------------------------------*/
		CPARAMX->GetNumber = RpxObjGetNumber;			/* Get Object Number */
		CPARAMX->isLong = RpxObjChkLong;				/* Check Object is Long */
/*--------------------------------------------------------------------------------------------------*/
		CPARAMX->SearchS = RpxObjSearchSub;				/* <S073> */
		CPARAMX->SearchBS = RpxObjSearchIdxSub;			/* <S073> */
		CPARAMX->WriteArray = RpxObjWriteArray;			/* <S073> */
/*--------------------------------------------------------------------------------------------------*/
		CPARAMX->PrmGetDecimals = RpxPrmGetDecimals;	/* <S14F> */
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		パラメータ管理パラメータリセット処理														*/
/*																									*/
/****************************************************************************************************/
void	RpxPrstPrmManager( void )
{
		return;
}

/****************************************************************************************************/
/*																									*/
/*		Write Parameter to RAM																		*/
/*																									*/
/****************************************************************************************************/
LONG	RpiPrmWriteRam(						/* Write Parameter to RAM								*/
		CPARAM  *this,						/* this Pointer											*/
		CPRMDEF *pPrmDef,					/* Pointer to the PrmDef								*/
		ULONG	Data,						/* Parameter Value 										*/
		HMSGIF	Hmsg )						/* Message Interface									*/
{
LONG	i;
LONG	rc;
/*TODO: WRPINF	WrPrmInf; */
CPRMDEF	*pRealDef = LPX_GET_pRealDef( pPrmDef );
/*--------------------------------------------------------------------------------------------------*/
/*		パラメータコピー時のチェック																*/
/*--------------------------------------------------------------------------------------------------*/
		if( 0 /* TODO: RoutV.f.PrmCopy */ )
		{
			return( PERR_PRMCOPY );
		}
/*--------------------------------------------------------------------------------------------------*/
/*		パラメータ書き込み権チェック	@@@見直し要@@@												*/
/*--------------------------------------------------------------------------------------------------*/
		if( 0 /* TODO: LpxCheckPrmWriteHandle( Hmsg, FnPnChk, pPrmDef ) != OK */ )					
		{
			return( PERR_WRITE_HANDLE );
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Fn実行時のパラメータ変更チェック															*/
/*--------------------------------------------------------------------------------------------------*/
		if( 0 /* TODO: RpiFunPrmWriteInhibitChk( ) */ )
		{
			return( PERR_INH );
		}
/*--------------------------------------------------------------------------------------------------*/
/*		データ範囲エラー																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( LpxPrmLimitCheckPdef( pPrmDef, Data ) != OK )
		{
			return( PERR_RANGE );
		}
/*--------------------------------------------------------------------------------------------------*/
/*		通常パラメータ書込み処理																	*/
/*--------------------------------------------------------------------------------------------------*/
		if( 1 /* TODO: pPrmDef->Attr.FrwAliasPrm == FALSE */ )
		{
			rc = LpxPrmWriteRam( pRealDef, Data /* LpxPrmIfDataToPrmVarData( pPrmDef, Data ) */, Hmsg );
		}
/*--------------------------------------------------------------------------------------------------*/
/*		別名特殊パラメータ書込み処理																*/
/*--------------------------------------------------------------------------------------------------*/
		else
		{
			if( 1 /* TODO: pPrmDef->FuncPtr( PNCAL_WRITEALIASPRM, Data, (ULONG)&WrPrmInf ) != TRUE */ )
			{
				return( PERR_RANGE );
			}
		/*------------------------------------------------------------------------------------------*/
//			for( rc=OK, i=0; i < 0; i++ )/* TODO: WrPrmInf.Count */
//			{
//				rc = NG /* LpxCheckPrmWriteHandle( Hmsg, FnPnChk, WrPrmInf.pPrmDef[i] ) */;
//				if( rc != OK ){ return( PERR_WRITE_HANDLE );}
//				rc = NG /* LpxPrmWriteRam( WrPrmInf.pPrmDef[i], WrPrmInf.WrtData[i], Hmsg ) */;
//				if( rc != OK ){ return( rc );}
//			}
		}
/*--------------------------------------------------------------------------------------------------*/
		return( rc );
}

/****************************************************************************************************/
/*																									*/
/*		パラメータ値の桁インクリメント																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	概要:	指定桁をインクリメント。																*/
/*																									*/
/*	注意:	パラメータの読み出し、リミット、インクリメント、書き換えを一括で行う処理に変更。		*/
/*																									*/
/*	戻値:	パラメータ書き換えと同様のエラーステータス。											*/
/*																									*/
/****************************************************************************************************/
LONG	RpxPrmDigitInc(						/* Increment Parameter Digit (RAM)						*/
		CPARAM  *this,						/* this Pointer											*/
		LONG	Index,						/* Parameter Index										*/
		ULONG	Digit,						/* Digit(0~)											*/
		MSGIF  *hMsg )						/* Message Interface 									*/
{
ULONG	t_data;
ULONG   data; 
CPRMDEF *pPrmDef  = GET_PNPRMDEF( this->pPrmTbl, Index ) /* PnPrmTbl[Index].PrmDefPtr */;
CPRMDEF	*pRealDef = LPX_GET_pRealDef( pPrmDef );
/*--------------------------------------------------------------------------------------------------*/
/*		Read Parameter Value 																		*/
/*--------------------------------------------------------------------------------------------------*/
		this->Read( (COBJECT *)this, Index, &data, PCHK_OPE_READ, hMsg );
/*--------------------------------------------------------------------------------------------------*/
/*		Increment Digit																				*/
/*--------------------------------------------------------------------------------------------------*/
		t_data = LpxPrmDataDigitInc( 
					data                          /* Current Data */, 
					Digit                         /* Digit(0~) */, 
					GET_PNPRM_UPPERLMT( pPrmDef ) /* RpxPrmUpperLimitIdx(Index) */,
					GET_PNPRMBASE( pPrmDef )      /* (UCHAR)pPrmDef->Attr.Base */,
					CHK_PNPRM_ISLONG( pPrmDef )   /* (UCHAR)pPrmDef->Attr.IfIsLong */,
					CHK_PNPRM_SIGNED( pPrmDef )   /* (UCHAR)pPrmDef->Attr.Sign */ );
/*--------------------------------------------------------------------------------------------------*/
/*		Limit Check																					*/
/*--------------------------------------------------------------------------------------------------*/
//		if( pPrmDef->Attr.Base != DSPDEF_BASENBL )
//		{
//			if( PrmLimitCheckIdx(Index, t_data) != OK )
//			{
//				t_data = RpxPrmUpperLimitIdx(Index);
//			}
//		}
/*--------------------------------------------------------------------------------------------------*/
/*		Adjust Operator Input <S149>																*/
/*--------------------------------------------------------------------------------------------------*/
		if( GET_PNPRM_ADJOPE( pPrmDef ) /* pPrmDef->Attr.AdjOpeIn */ )
		{
			LPX_ADJOPEINC( pPrmDef, Index, t_data, hMsg );
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Write Incremented Parameter																	*/
/*--------------------------------------------------------------------------------------------------*/
		if( data ^ t_data )
		{
			return (this->Write( this, Index, t_data, hMsg ));
		}
		else
		{
			return (OK);
		}
/*--------------------------------------------------------------------------------------------------*/
//	return( t_data );
}



/****************************************************************************************************/
/*																									*/
/*		データ値の桁インクリメント																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	概要:	指定桁をインクリメントした値を取得する。												*/
/*			インクリメントされた値が最大値を超えている場合、最大値を戻す。							*/
/*																									*/
/*	戻値:	インクリメントされた値または最大値														*/
/*																									*/
/****************************************************************************************************/
ULONG	LpxPrmDataDigitInc(					/* データ値の桁インクリメント							*/
		ULONG	Data,						/* 元のデータ値											*/
		ULONG	Digit,						/* 指定桁(0～)											*/
		ULONG	UpperLimit,					/* 最大値												*/
		ULONG	Base,						/* データの底(DSPDEF_BASENBL, BASEDEC, BASEHEX)			*/
		ULONG	IsLong,						/* ロングフラグ(TRUE, FALSE)							*/
		ULONG	Sign			)			/* 符号有無(DSPDEF_W_SIGN, NOSIGN)						*/
{
LONG	nbl_data;
ULONG	t_data = Data;
/*--------------------------------------------------------------------------------------------------*/
/*		ニブルの場合																				*/
/*--------------------------------------------------------------------------------------------------*/
		if( Base == OBJDEF_BASENBL )
		{
			nbl_data = (CHAR)((Data & (0x0f << (Digit*4))) >> (Digit*4));
			nbl_data += 1;
			if(nbl_data > (CHAR)((UpperLimit & (0x0f << (Digit*4))) >> (Digit*4)))
			{
				nbl_data = (CHAR)((UpperLimit & (0x0f << (Digit*4))) >> (Digit*4));
			}
			t_data = t_data - (t_data & (0x0f << (Digit*4))) + (nbl_data << (Digit*4));
		}
/*--------------------------------------------------------------------------------------------------*/
/*		ニブル以外の場合																			*/
/*--------------------------------------------------------------------------------------------------*/
		else
		{
		/*------------------------------------------------------------------------------------------*/
		/*		WORDサイズで符号付																	*/
		/*------------------------------------------------------------------------------------------*/
			if(!IsLong && (Sign == OBJDEF_W_SIGN))
			{
				/* 符号付ワードの場合、符号拡張しておく	*/
				t_data = (LONG)((SHORT)(Data & 0xFFFF));
			}
		/*------------------------------------------------------------------------------------------*/
		/*		インクリメントデータ																*/
		/*------------------------------------------------------------------------------------------*/
			t_data += ((Base == OBJDEF_BASEDEC)? MlibDecPowerTbl[Digit]: ((ULONG)0x01 << (Digit*4)));
		/*------------------------------------------------------------------------------------------*/
		/*		符号が付いていない場合																*/
		/*------------------------------------------------------------------------------------------*/
			if(Sign == OBJDEF_NOSIGN)
			{
				if(t_data < Data || t_data > UpperLimit)
				{	/* オーバーフローになった場合 or 上限値よりも大きい場合	*/
					t_data = UpperLimit;
				}
			}
		/*------------------------------------------------------------------------------------------*/
		/*		符号付の場合																		*/
		/*------------------------------------------------------------------------------------------*/
			else if(Sign == OBJDEF_W_SIGN)
			{
				if((LONG)t_data < (LONG)Data || (LONG)t_data > (LONG)UpperLimit)
				{	/* 正側オーバーフローになった場合 or 上限値よりも大きい場合	*/
					t_data = UpperLimit;
				}
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		return( t_data );
}


/****************************************************************************************************/
/*																									*/
/*		Upper and lower parameter limit check														*/
/*																									*/
/****************************************************************************************************/
LONG	LpxPrmLimitCheckPdef(				/* Check Parameter Limit								*/
		CPRMDEF *pPrmDef,					/* Pointer to the PrmDef								*/
		ULONG	Data			)			/* Check Data											*/
{
ULONG	i;
/*--------------------------------------------------------------------------------------------------*/
/*		Nibble Parameter																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( GET_PNPRMBASE( pPrmDef ) /* ->Attr.Base */ == OBJDEF_BASENBL ) 
		{
			ULONG	mask = 0x0F;
			ULONG	imax = ( CHK_PNPRM_ISLONG( pPrmDef ) /* ->Attr.IfIsLong */ )? 8 : 4;
		/*------------------------------------------------------------------------------------------*/
			for( i=0; i < imax; i++ )
			{
				if( (Data & mask) < (pPrmDef->LowerLimit & mask) ){ return(NG);}
				if( (Data & mask) > (pPrmDef->UpperLimit & mask) ){ return(NG);}
				mask = (mask << 4);
			}
			return(OK);
		}
/*--------------------------------------------------------------------------------------------------*/
/*		データ範囲チェック：符号付き																*/
/*--------------------------------------------------------------------------------------------------*/
		else if( CHK_PNPRM_SIGNED( pPrmDef ) /*->Attr.Sign */ == OBJDEF_W_SIGN )
		{
			if( (LONG)Data > (LONG)pPrmDef->UpperLimit ){ return( NG );}
			if( (LONG)Data < (LONG)pPrmDef->LowerLimit ){ return( NG );}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		データ範囲チェック：符号なし																*/
/*--------------------------------------------------------------------------------------------------*/
		else
		{
			if( (ULONG)Data > (ULONG)pPrmDef->UpperLimit ){ return( NG );}
			if( (ULONG)Data < (ULONG)pPrmDef->LowerLimit ){ return( NG );}
		}
/*--------------------------------------------------------------------------------------------------*/
		return( OK );
}

/****************************************************************************************************/
/*																									*/
/*		Write Parameter to RAM 																		*/
/*																									*/
/****************************************************************************************************/
LONG	LpxPrmWriteRam(						/* パラメータ書き込み処理								*/
		CPRMDEF *pRealDef,					/* パラメータ定義ポインタ								*/
		ULONG	PrmData,					/* 書き込むパラメータ値									*/
		HMSGIF	Hmsg			)			/* メッセージハンドル									*/
{
LONG	rc;
ULONG	PrmSave;
/* void	*pPrmVar; */
/* ULONG	ComIF; */
ULONG	ErrFlag = 0;
/*--------------------------------------------------------------------------------------------------*/
/*		通信ＩＦ専用パラメータ処理																	*/
/*--------------------------------------------------------------------------------------------------*/
#if		CFG_MASTER_CPU == 1
		if( (pRealDef->Attr.PrmDefIdx == 1) && (pRealDef->Attr.DrvComPrm != TRUE) )
		{
			USHORT	PrmSize = pRealDef->Attr.VarIsLong? 4:2;
			rc = DpiDegOpWriteOneRegister( pRealDef->PnNo, PrmSize, PrmData );
			if( rc != OK ){ return( rc );}
		}
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		Update Parameter RAM Area																	*/
/*--------------------------------------------------------------------------------------------------*/
		LPX_SET_PRMVAR_RAM( pRealDef, PrmData, Hmsg, PrmSave );

/*--------------------------------------------------------------------------------------------------*/
/*		パラメータ計算実行チェック																	*/
/*--------------------------------------------------------------------------------------------------*/
		if( 0 /* TODO: (pRealDef->ChkSpUse & Bprm.ChkSpUse) == 0x00 */ ){ return( OK );}
#if		CFG_MASTER_CPU == 1
		if( (pRealDef->Attr.PrmDefIdx == 1) && (pRealDef->Attr.DrvComPrm != TRUE) ){ return( OK );}
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		パラメータ計算実行処理																		*/
/*--------------------------------------------------------------------------------------------------*/
/*		注意：パラメータ計算エラーが起きた場合の処理												*/
/*		エンコーダ通信異常・エンコーダ通信位置データ加速度異常・エンコーダ通信タイマ異常・			*/
/*		エンコーダエコーバック異常アラーム発生時は計算エラー処理しないこと。(SGDSより)				*/
/*--------------------------------------------------------------------------------------------------*/
		KriResetParamError( );									/* Reset Error Flag					*/
		/* ComIF = (KPI_CHK_ComHmsg( Hmsg ))? 1 : 0 ;*/				/* ComIf(1)/Operator(0)				*/
		LPX_CALC_PARAMETER( pRealDef, rc, Hmsg ); 				/* rc = (*pRealDef->FuncPtr)( PNCAL_PRMCAL, ComIF, 0 );	*/ /* パラメータ計算					*/
		//TODO: エンコーダ通信異常時の計算エラー回避処理(INGRAMではやってない)
		if( (/* (Alm.PrmErrInfo.ErrFlag == TRUE) ||*/ (rc != TRUE)) /* && !Alm.AlmAttr.f.NoPceRepair */)
		{
			ErrFlag = 1;
		}

/*--------------------------------------------------------------------------------------------------*/
/*		Drive&ComIF共通パラメータ処理																*/
/*--------------------------------------------------------------------------------------------------*/
#if		CFG_MASTER_CPU == 1
		if( (ErrFlag == 0) && (pRealDef->Attr.DrvComPrm == TRUE) )
		{
			USHORT	PrmSize = pRealDef->Attr.VarIsLong? 4:2;
			if( DpiDegOpWriteOneRegister( pRealDef->PnNo, PrmSize, PrmData ) != OK ){ ErrFlag = 1;}
		}
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		パラメータ計算エラーが起きた場合の処理														*/
/*--------------------------------------------------------------------------------------------------*/
		if( ErrFlag != 0 )
		{
			LPX_RESTORE_PARAMETER( pRealDef, Hmsg, PrmSave );
		/*------------------------------------------------------------------------------------------*/
			if( rc == TRUE )
			{
				LPX_CALC_PARAMETER( pRealDef, rc, Hmsg ); /* (*pRealDef->FuncPtr)( PNCAL_PRMCAL, ComIF, 0 );*/	/* パラメータ計算					*/
			}
			return( PERR_CALC );
		}
/*--------------------------------------------------------------------------------------------------*/
		return( OK );
}

/****************************************************************************************************/
/*																									*/
/*		パラメータ値の桁デクリメント																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	 概要:	指定桁をデクリメントした値を取得する。													*/
/*			デクリメントされた値が最小値よりも小さい場合、最小値を戻す。							*/
/*																									*/
/*	注意:	この関数にはユーザレベル等のチェック機能はありません。									*/
/*			事前に、RpxPrmSearchIdx() あるいは PrmAttributeChkIdx()でチェックを行い、				*/
/*			OKであることを確認後、使用してください。												*/
/*																									*/
/*	戻値:	デクリメントされた値または最小値														*/
/*																									*/
/****************************************************************************************************/
LONG	RpxPrmDigitDec(						/* Decrement Parameter Digit (RAM)						*/
		CPARAM  *this,						/* this Pointer											*/
		LONG	Index,						/* Parameter Index										*/
		ULONG	Digit,						/* Digit(0~)											*/
		MSGIF   *hMsg )						/* Message Interface 									*/
{
ULONG	t_data;
ULONG 	data;
CPRMDEF *pPrmDef = GET_PNPRMDEF( this->pPrmTbl, Index ) /* PnPrmTbl[Index].PrmDefPtr */;
CPRMDEF	*pRealDef = LPX_GET_pRealDef( pPrmDef );
/*--------------------------------------------------------------------------------------------------*/
/*		Read Parameter Value 																		*/
/*--------------------------------------------------------------------------------------------------*/
		this->Read( (COBJECT *)this, Index, &data, PCHK_OPE_READ, hMsg );
/*--------------------------------------------------------------------------------------------------*/
/*		Decrement Digit																				*/
/*--------------------------------------------------------------------------------------------------*/
		t_data = LpxPrmDataDigitDec( 
					data							/* Data */, 
					Digit							/* Digit (0~) */, 
					GET_PNPRM_LOWERLMT( pPrmDef )	/* RpxPrmLowerLimitIdx(Index) */,
					GET_PNPRMBASE( pPrmDef )		/* (UCHAR)pPrmDef->Attr.Base */,
					CHK_PNPRM_ISLONG( pPrmDef ) 	/* (UCHAR)pPrmDef->Attr.IfIsLong */,
					CHK_PNPRM_SIGNED( pPrmDef )		/* (UCHAR)pPrmDef->Attr.Sign */			);
/*--------------------------------------------------------------------------------------------------*/
/*		Limit Check																					*/
/*--------------------------------------------------------------------------------------------------*/
//		if( pPrmDef->Attr.Base != DSPDEF_BASENBL )
//		{
//			if(PrmLimitCheckIdx(Index, t_data) != OK)
//			{
//				t_data = RpxPrmLowerLimitIdx(Index);
//			}
//		}
/*--------------------------------------------------------------------------------------------------*/
/*		Adjust Operator Input <S149>																*/
/*--------------------------------------------------------------------------------------------------*/
		if( GET_PNPRM_ADJOPE( pPrmDef ) /* pPrmDef->Attr.AdjOpeIn */ )
		{
			LPX_ADJOPEDEC( pPrmDef, Index, t_data, hMsg );
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Write Decremented Parameter																	*/
/*--------------------------------------------------------------------------------------------------*/
		if( data ^ t_data )
		{
			return (this->Write( this, Index, t_data, hMsg ));
		}
		else
		{
			return (OK);
		}
/*--------------------------------------------------------------------------------------------------*/
//		return( t_data );
}



/****************************************************************************************************/
/*																									*/
/*		データ値の桁デクリメント																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	概要:	指定桁をデクリメントした値を取得する。													*/
/*			デクリメントされた値が最小値よりも小さい場合、最小値を戻す。							*/
/*																									*/
/*	戻値:	デクリメントされた値または最小値														*/
/*																									*/
/****************************************************************************************************/
ULONG	LpxPrmDataDigitDec(					/* データ値の桁デクリメント								*/
		ULONG	Data,						/* 元のデータ値											*/
		ULONG	Digit,						/* 指定桁(0～)											*/
		ULONG	LowerLimit,					/* 最小値												*/
		ULONG	Base,						/* データの底(DSPDEF_BASENBL, BASEDEC, BASEHEX)			*/
		ULONG	IsLong,						/* ロングフラグ(TRUE, FALSE)							*/
		ULONG	Sign			)			/* 符号有無(DSPDEF_W_SIGN, NOSIGN)						*/
{
LONG	nbl_data;
ULONG	t_data = Data;
/*--------------------------------------------------------------------------------------------------*/
/*		ニブルの場合																				*/
/*--------------------------------------------------------------------------------------------------*/
		if( Base == OBJDEF_BASENBL )
		{
			nbl_data = (CHAR)((Data & (0x0f << (Digit*4))) >> (Digit*4));
			nbl_data -= 1;
			if(nbl_data < (CHAR)((LowerLimit & (0x0f << (Digit*4))) >> (Digit*4)))
			{
				nbl_data = (CHAR)((LowerLimit & (0x0f << (Digit*4))) >> (Digit*4));
			}
			t_data = t_data - (t_data & (0x0f << (Digit*4))) + (nbl_data << (Digit*4));
		}
/*--------------------------------------------------------------------------------------------------*/
/*		ニブル以外の場合																			*/
/*--------------------------------------------------------------------------------------------------*/
		else
		{
		/*------------------------------------------------------------------------------------------*/
		/*		WORDサイズで符号付																	*/
		/*------------------------------------------------------------------------------------------*/
			if(!IsLong && (Sign == OBJDEF_W_SIGN))
			{
				/* 符号付ワードの場合、符号拡張しておく	*/
				t_data = (LONG)((SHORT)(Data & 0xFFFF));
			}
		/*------------------------------------------------------------------------------------------*/
		/*		デクリメントデータ																	*/
		/*------------------------------------------------------------------------------------------*/
			t_data -= ((Base == OBJDEF_BASEDEC)? MlibDecPowerTbl[Digit]: ((ULONG)0x01 << (Digit*4)));
		/*------------------------------------------------------------------------------------------*/
		/*		符号が付いていない場合																*/
		/*------------------------------------------------------------------------------------------*/
			if( Sign == OBJDEF_NOSIGN )
			{
				if(t_data > Data || t_data < LowerLimit)
				{	/* オーバーフローになった場合 or 下限値よりも小さい場合	*/
					t_data = LowerLimit;
				}
			}
		/*------------------------------------------------------------------------------------------*/
 		/*		符号付の場合																		*/
		/*------------------------------------------------------------------------------------------*/
 			else if( Sign == OBJDEF_W_SIGN )
			{
				if((LONG)t_data > (LONG)Data || (LONG)t_data < (LONG)LowerLimit)
				{	/* 負側オーバーフローになった場合 or 下限値よりも小さい場合	*/
					t_data = LowerLimit;
				}
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		return( t_data );
}

/* <S14F> */
/****************************************************************************************************/
/*																									*/
/*		パラメータ小数点位置の取得																	*/
/*																									*/
/****************************************************************************************************/
USHORT	RpxPrmGetDecimals(
		CPARAM  *this,						/* this Pointer											*/
		CPRMDEF	*prmdef,					/* パラメータ定義へのポインタ							*/
		MSGIF	*hMsg		)				/* メッセージハンドル									*/
{
	AXIS_HANDLE *pAxRsc		= hMsg->pAxRsc;
	USHORT		Decimals	= prmdef->Decimals;

	if( pAxRsc->MencV->LowSpdMotor )
	{
		switch( prmdef->RegNumber )
		{
			case	0x0301:
			case	0x0302:
			case	0x0303:
			case	0x0304:
			case	0x0493:
			case	0x0494:
			case	0x0533:
							Decimals++;
							break;
			default:
							break;
		}
	}
	
	return( Decimals );
}

/***************************************** end of file **********************************************/
