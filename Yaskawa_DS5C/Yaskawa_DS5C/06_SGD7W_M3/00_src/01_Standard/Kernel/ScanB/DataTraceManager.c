/****************************************************************************************************/
/*																									*/
/*																									*/
/*		DataTraceManager.c : データトレース処理														*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : データトレース処理																		*/
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
/*	Note	:	初版	2010.10.26	T.Kira	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"
#include "MLib.h"
#include "XlsTrcDef.h"
#include "DataTraceCalc.h"
#include "DataTraceManager.h"
#include "Memory.h"					/* @@ */

/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
static BOOL	dtrcCalcTraceObject( TRCHNDL *TrcHndl, LONG Tno );
static LONG	dtrcSearchTrcBitTbl( TRCHNDL *TrcHndl, LONG SelNo );
//static LONG	dtrcCalcSetSampLlim( void* Axis, LONG Axno );
static void dtrcSetTrgParameter( LONG TrgNo, SHORT Adj, TRCEXEP *TrcExeP, TRCREG *TrcReg );
static void dtrcClearTrgParameter( TRCTRG *TrcTrg );
//
static LONG	dtrcGetMergeData( TRCEXEP *TrcExeP, TRCOBJ *Tobjpx );
static void	dtrcGetTrcData( TRCEXEP *TrcExeP, TRCOBJ *Tobjpx, LONG *VerData, LONG *BitData, LONG Idx );
static BOOL	dtrcCheckTriggerProcedure( TRCHNDL *TrcHndl );
static BOOL	dtrcCheckTrigger( LONG TrgNo, TRCHNDL *TrcHndl, LONG *TrgVar );
static BOOL	dtrcFinishOpeModeOperation( TRCHNDL *TrcHndl );
static void* dtrcConvAxisAddress( MEMOBUS_ADDR_MAP *AxisMap, LONG Address, LONG *AxisNum );
static void* dtrcGetTopAxisAddress( MEMOBUS_ADDR_MAP *AxisMap );
//
static BOOL	dtrcSetDataTraceCmd( TRCHNDL *TrcHndl, ULONG OpeModeTrc, ULONG OpeCmdTrc );
static BOOL	dtrcStartDataTrace( TRCHNDL *TrcHndl, UCHAR mode );
static BOOL	dtrcStopDataTrace( TRCHNDL *TrcHndl );
static BOOL	dtrcStartRingTrace( TRCHNDL *TrcHndl );
static BOOL	dtrcStopRingTrace( TRCHNDL *TrcHndl, LONG finmode );
//
static void	DtrcSelSetInitValue( TRCHNDL *TrcHndl );	/* <S0C9> */

/****************************************************************************************************/
/*																									*/
/*		データトレース管理初期化処理																*/
/*																									*/
/****************************************************************************************************/
void	DtrcInitDtrcManager( TRCHNDL *TrcHndl, USHORT *TraceBuffer, MEMOBUS_ADDR_MAP *AxisMap )
{
	LONG	EntNum,i;	
	TRCMNGP	*TrcMngP;
	TrcMngP = &TrcHndl->TrcMngP;

	/*----------------------------------------------------------------------------------------------*/
	/*		数値トレース定義２分検索幅の設定														*/
	/*----------------------------------------------------------------------------------------------*/
	EntNum = TRCVARTBL_ENTNUM;
	TrcMngP->TrcVarTblEntNum = (ULONG)EntNum;
	TrcMngP->TrcVarTblSrhIniW = (1 << MlibSrhbiton( EntNum - 1, SRH_FROM_MSB ));
	/*----------------------------------------------------------------------------------------------*/
	/*		ビットトレース定義２分検索幅の設定														*/
	/*----------------------------------------------------------------------------------------------*/
	EntNum = TRCBITTBL_ENTNUM;
	TrcMngP->TrcBitTblEntNum = (ULONG)EntNum;
	TrcMngP->TrcBitTblSrhIniW = (1 << MlibSrhbiton( EntNum - 1, SRH_FROM_MSB ));
	/*----------------------------------------------------------------------------------------------*/
	/*		データトレース管理データ変数リセット													*/
	/*----------------------------------------------------------------------------------------------*/
	MlibResetLongMemory( &TrcHndl->TrcMngV, sizeof(TrcHndl->TrcMngV)/4 );
	MlibResetLongMemory( &TrcHndl->TrcExeV, sizeof(TrcHndl->TrcExeV)/4 );
	/*----------------------------------------------------------------------------------------------*/
	/*		軸アドレス定義を取得																	*/
	/*----------------------------------------------------------------------------------------------*/
	for(i = 0; i < MAX_AXIS_NUMBER; i++)
	{
		TrcHndl->AxisMap[i] = AxisMap[i];
	}

/* <S0C9>	Start */
	/*----------------------------------------------------------------------------------------------*/
	/*		トレース対象データ選択、I/Oトレース対象データ選択の初期値設定							*/
	/*----------------------------------------------------------------------------------------------*/
	DtrcSelSetInitValue( TrcHndl );
/* <S0C9>	End */

#ifdef _DATA_TRACE_DBUG_
//デバッグ用
	TrcReg->TrcMode     = 0x0010;
	TrcReg->DataSize    = 0x0003;
	TrcReg->AreaSize    = 8192;
	TrcReg->SampIntv    = 0x0001;
	TrcReg->SampTime    = 250;
	TrcReg->SampLlim    = 1;
	TrcReg->SampUlim    = 255;
	TrcReg->MaxAreaSize = 8192;
	TrcReg->OpeModeTrc  = 0;
	TrcReg->OpeCmdTrc   = 0;

	TrcReg->PtrgDnum  = 0x0064;
	TrcReg->TrgSel    = 0x0100;
	TrcReg->TrgLevel  = 0x100;
	TrcReg->TrgPoint  = 0x0000;
	TrcReg->TrcState  = 0x0000;
	TrcReg->MTrgMode  = 0;
	TrcReg->MTrgSel   = 0;
	TrcReg->MTrgLevel = 0;
#endif
	/*----------------------------------------------------------------------------------------------*/
	/*		データトレースパラメータ計算															*/
	/*----------------------------------------------------------------------------------------------*/
	DtrcCalcParameter( TrcHndl, TraceBuffer );			/* データトレース実行パラメータ計算			*/

}

/****************************************************************************************************/
/*																									*/
/*		データトレース実行パラメータ計算処理														*/
/*																									*/
/****************************************************************************************************/
PRM_RSLT DtrcCalcParameter( TRCHNDL *TrcHndl, USHORT *TraceBuffer )
{
	ULONG	i;
	ULONG	x,y,z;
	SHORT	Adj;

	TRCREG	*TrcReg;
	TRCEXEP	*TrcExeP;
	TRCINDIVPRM	*TrcIncivPrm;	/* <S0C9> */

	TrcReg  = &TrcHndl->TrcReg;
	TrcExeP = &TrcHndl->TrcExeP;
	TrcIncivPrm = &TrcHndl->TrcIncivPrm;	/* <S0C9> */

	TrcExeP->TrcPrmOk = FALSE;								/* データトレースパラメータセットOK		*/
	/*----------------------------------------------------------------------------------------------*/
	/*		トレースサンプリング周期最小値設定														*/
	/*----------------------------------------------------------------------------------------------*/
	/*for( i=0; i < TRC_CH_MAXNUM; i++ )
	{
		x = dtrcCalcSetSampLlim( TrcHndl, i );
	
		if(x > TrcReg->SampLlim)
		{
			TrcReg->SampLlim = (USHORT)x;
		}
	}*/
/* <S0C9>	Delete Start */
	/*----------------------------------------------------------------------------------------------*/
	/*		トレースエリアサイズ																	*/
	/*----------------------------------------------------------------------------------------------*/
	//if ( TrcReg->AreaSize > TrcReg->MaxAreaSize )
	//{
	//	return PRM_RSLT_CALC_ERR;
	//}
/* <S0C9>	Delete End */
	/*----------------------------------------------------------------------------------------------*/
	/*		トレースデータタイプ																	*/
	/*----------------------------------------------------------------------------------------------*/
	if( TrcReg->DataSize == 1 || TrcReg->DataSize == 3 )				/* ショートデータトレース	*/
	{
		TrcExeP->TrcDataType = DATATRACE_SHORT;
	}
	else if( TrcReg->DataSize == 2 || TrcReg->DataSize == 4 )			/* ロングデータトレース		*/
	{
		TrcExeP->TrcDataType = DATATRACE_LONG;
	}
	else
	{
		return PRM_RSLT_CONDITION_ERR;
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		IOトレースタイプ																		*/
	/*----------------------------------------------------------------------------------------------*/
	if( TrcReg->DataSize == 1 || TrcReg->DataSize == 2 )
	{
		TrcExeP->TrcIOType = IODATA_MERGE;						/* IOデータを変数データに結合		*/
		Adj = 0;												/* トレース本数、ビットシフト調整値 */
	}
	else if( TrcReg->DataSize == 3 || TrcReg->DataSize == 4 )
	{
		TrcExeP->TrcIOType = IODATA_DIV;						/* IOデータを専用領域に格納			*/
		Adj = 1;												/* トレース本数、ビットシフト調整値 */
	}
	else
	{ /* 未定義 */
		TrcExeP->TrcIOType = IODATA_MERGE;						/* IOデータを変数データに結合		*/
		Adj = 0;												/* トレース本数、ビットシフト調整値 */
	}

	/*----------------------------------------------------------------------------------------------*/
	/*	トレースデータ上下限データ、ビットセット／クリアデータ										*/
	/*----------------------------------------------------------------------------------------------*/
	if(TrcExeP->TrcIOType == IODATA_MERGE )							/* IOデータを変数データに結合	*/
	{	 
		if( TrcExeP->TrcDataType == DATATRACE_SHORT )
		{	/* ショートデータトレース */
			TrcExeP->UpperLmt =  16383;
			TrcExeP->LowerLmt = -16384;
			TrcExeP->TrcBitSet = 0x8000;
			TrcExeP->TrcBitClr = ~TrcExeP->TrcBitSet;
		}
		else
		{	/* ロングデータトレース */
			TrcExeP->UpperLmt =  1073741823;
			TrcExeP->LowerLmt = -1073741824;
			TrcExeP->TrcBitSet = 0x80000000;
			TrcExeP->TrcBitClr = ~TrcExeP->TrcBitSet;
		}
	}
	else															/* IOデータを専用領域に格納		*/ 
	{
		if( TrcExeP->TrcDataType == DATATRACE_SHORT )
		{	/* ショートデータトレース */
			TrcExeP->UpperLmt =  32767;
			TrcExeP->LowerLmt = -32768;
			TrcExeP->TrcBitSet = 1;
			TrcExeP->TrcBitClr = 0;
		}
		else
		{	/* ロングデータトレース	*/
			TrcExeP->UpperLmt =  0x7FFFFFFF; /*  2147483647	*/
			TrcExeP->LowerLmt =  0x80000000; /* -2147483648 */
			TrcExeP->TrcBitSet = 1;
			TrcExeP->TrcBitClr = 0;
		}
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		トレースデータ本数																		*/
	/*----------------------------------------------------------------------------------------------*/
	x = TrcReg->TrcMode;
	if( (x < 1) || (x > TRC_CH_MAXNUM ) )
	{
		return PRM_RSLT_CALC_ERR;
	}

/* <S0C9>	Start */
	/*----------------------------------------------------------------------------------------------*/
	/*		トレースバッファ長、トレースエリアサイズ												*/
	/*																								*/
	/*		トレースバッファ長は、Σ-7シリーズで追加されたレジスタです。							*/
	/*		トレースバッファ長 = 0と設定された場合、Σ-Ⅴ互換モードとして「使用エリアサイズ」が		*/
	/*		有効となります。																		*/
	/*																								*/
	/*----------------------------------------------------------------------------------------------*/
	if ( TrcReg->TrcBuffSize != 0 )		/* トレースバッファ長 有効 (トレースバッファ長 ≠ 0) */
	{
		if ( TrcReg->TrcBuffSize > TrcReg->MaxTrcBuffSize )
		{
			return PRM_RSLT_CALC_ERR;
		}
		else
		{
			TrcReg->AreaSize = TrcReg->TrcBuffSize * TrcReg->TrcMode;
		}
	}
	else		/* トレースエリアサイズ 有効 (トレースバッファ長 = 0) */
	{
		if ( TrcReg->AreaSize > TrcReg->MaxAreaSize )
		{
			return PRM_RSLT_CALC_ERR;
		}
	}
/* <S0C9>	End */

	/*----------------------------------------------------------------------------------------------*/
	/*		プリトリガデータ数チェック																*/
	/*----------------------------------------------------------------------------------------------*/
//#if CSW_NET_TRG_USE == TRUE		/* <S012> *//* <S021> */
//	y = ( TrcExeP->TrcDataType == DATATRACE_SHORT ) ? x : (2*x);
//#else /* CSW_NET_TRG_USE */
//	y = ( TrcExeP->TrcDataType == DATATRACE_SHORT ) ? 1 : 2;		/* <S0C9>	Delete */
//#endif /* CSW_NET_TRG_USE */
	y = ( TrcExeP->TrcDataType == DATATRACE_SHORT ) ? x : (2*x);	/* <S0C9> */

	TrcExeP->TrcDnum = TrcReg->AreaSize / y;

	if( TrcReg->PtrgDnum >= TrcExeP->TrcDnum )
	{
		return PRM_RSLT_CALC_ERR;
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		トレースデータバッファ																	*/
	/*----------------------------------------------------------------------------------------------*/
	x = ( TrcExeP->TrcDataType == DATATRACE_SHORT ) ? TrcExeP->TrcDnum : ( 2 * TrcExeP->TrcDnum );
//	y = TRC_CH_MAXNUM + Adj;
	y = TRC_CH_MAXNUM;			/* <S1B6>  +Adjすると領域ｵｰﾊﾞｰとなる */
	z = TrcReg->TrcMode + Adj;
	for( i=0; i < y; i++ )
	{
		if( z > i )
		{
			TrcExeP->TrcObj[i].TrcBuf = TraceBuffer + (i*x);
		}
		else
		{
			TrcExeP->TrcObj[i].TrcBuf = NULL;
		}
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		トレースデータサンプリング間隔															*/
	/*----------------------------------------------------------------------------------------------*/
	if(TrcReg->SampIntv > 64)			/* オーバーフローしないサンプリング間隔は 2^(30-24)=64		*/
	{
		TrcExeP->DataSampIntv = 64;
	}
	else
	{
		TrcExeP->DataSampIntv = TrcReg->SampIntv;
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		トレース対象個別パラメータ計算															*/
	/*----------------------------------------------------------------------------------------------*/
	for( i=0; i < TRC_CH_MAXNUM; i++ )
	{
		if( dtrcCalcTraceObject( TrcHndl, i ) == FALSE )
		{
			return PRM_RSLT_CALC_ERR;
		}
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		トリガ条件設定																			*/
	/*----------------------------------------------------------------------------------------------*/
	dtrcSetTrgParameter( SINGLE_TRG, Adj,TrcExeP, TrcReg );

	/*----------------------------------------------------------------------------------------------*/
	/*		マルチトリガ条件設定																	*/
	/*----------------------------------------------------------------------------------------------*/
	if( (TrcReg->MTrgMode == 1) || (TrcReg->MTrgMode == 2) )
	{
		dtrcSetTrgParameter( MULTI_TRG, Adj, TrcExeP, TrcReg );
	}
	else
	{
		dtrcClearTrgParameter( &TrcExeP->TrcTrg[MULTI_TRG] );
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		データトレースパラメータ計算完了														*/
	/*----------------------------------------------------------------------------------------------*/
	TrcExeP->TrcPrmOk = TRUE;							/* データトレースパラメータＯＫセット		*/
	return( PRM_RSLT_SUCCESS );							/* パラメータ計算完了リターン				*/

}

/****************************************************************************************************/
/*																									*/
/*		データトレース トリガ条件設定																*/
/*																									*/
/****************************************************************************************************/
static void dtrcSetTrgParameter( LONG TrgNo, SHORT Adj, TRCEXEP *TrcExeP, TRCREG *TrcReg )
{
#if 0	/* <S0C9> Delete Start */
	LONG x,y,z;
	TRCTRG *TrcTrg;
#if CSW_NET_TRG_USE == FALSE		/* <S012> */
	TrcTrg = &TrcExeP->TrcTrg[TrgNo];
	
	x = (TrcReg->TrgSel       & 0x000F);									/* トリガデータ選択		*/
	y = (TrcReg->TrgSel >> 4) & 0x000F;										/* トリガ対象選択		*/
	z = (TrcReg->TrgSel >> 8) & 0x000F;										/* トリガ対象選択		*/

	if(z > 0)
	{
		if(y == 0)															/* 数値レベルトリガ		*/
		{
			TrcTrg->TrgEdge  = (USHORT)z;
			TrcTrg->TrgBitSetMsk = 0;
			TrcTrg->TrgBufx  = TrcExeP->TrcObj[x].TrcBuf;
			TrcTrg->TrgVsft  = ( TrcExeP->TrcDataType == DATATRACE_SHORT ) ? ( 17-Adj ) : ( 1-Adj );
			TrcTrg->TrgLevel = TrcReg->TrgLevel;
			TrcTrg->TrgSel   = ((TrcTrg->TrgEdge == 0)||(TrcTrg->TrgBufx == NULL))? 
																			0 : TrcReg->TrgSel;
		}
		else if( (0 < y)&&(y < TrcReg->TrcMode) )							/* ビットトリガ			*/
		{
			TrcTrg->TrgEdge  = (USHORT)z;
			TrcTrg->TrgBitSetMsk = ( TrcExeP->TrcIOType == IODATA_MERGE ) ? 
																TrcExeP->TrcBitSet : ( 0x01 << x );
			x  = ( TrcExeP->TrcIOType == IODATA_MERGE )? x : TrcReg->TrcMode;
			TrcTrg->TrgBufx  = TrcExeP->TrcObj[x].TrcBuf;
			TrcTrg->TrgVsft  = 0;
			TrcTrg->TrgLevel = 0;
			TrcTrg->TrgSel   = ((TrcTrg->TrgEdge == 0)||(TrcTrg->TrgBufx == NULL))? 
																			0 : TrcReg->TrgSel;
		}
	}
	else																	/* トリガ指定なし		*/
	{	/* トリガ変数クリア */
		dtrcClearTrgParameter( TrcTrg );
	}
#else /* CSW_NET_TRG_USE */		/* <S012> */
	TrcTrg = &TrcExeP->TrcTrg[TrgNo];

	x = (TrcReg->TrgSel       & 0x000F);									/* トリガデータ選択		*/
	y = (TrcReg->TrgSel >> 4) & 0x000F;										/* トリガタイプ選択		*/
//	y = TrcReg.TrgLevel.Long;												/* トリガデータ選択		*/

	if( (1 <= x)&&(x <= 7) )
	{ /* 数値レベルトリガ */
		TrcTrg->TrgEdge  = (USHORT)y;
		TrcTrg->TrgBufx  = TrcExeP->TrcObj[x-1].TrcBuf;
		TrcTrg->TrgVsft  = (TrcExeP->TrcDataType == DATATRACE_SHORT) ? ( 17-Adj ) : ( 1-Adj );
		TrcTrg->TrgLevel = TrcReg->TrgLevel;
		TrcTrg->TrgSel   = ((TrcTrg->TrgEdge == 0)||(TrcTrg->TrgBufx == NULL))? 0 : TrcReg->TrgSel;
	}
	else if( (x == 9)&&(1 <= TrcReg->TrgLevel)&&(TrcReg->TrgLevel <= 7) )
	{ /* ビットトリガ */
		TrcTrg->TrgEdge  = (USHORT)y;
		TrcTrg->TrgBufx  = TrcExeP->TrcObj[y-1].TrcBuf;
		TrcTrg->TrgVsft  = 0;
		TrcTrg->TrgLevel = 0;
		TrcTrg->TrgSel   = ((TrcTrg->TrgEdge == 0x00)||(TrcTrg->TrgBufx == NULL))? 0 : TrcReg->TrgSel;
	}
	else if( y == NET_TRG_SEL ) /* <S021> */
	{ /* ネットワークトリガ */
		TrcTrg->TrgEdge  = 0;
		TrcTrg->TrgBufx  = NULL;
		TrcTrg->TrgVsft  = 0;
		TrcTrg->TrgLevel = 0;
		TrcTrg->TrgSel   = TrcReg->TrgSel;
	}
	else
	{ /* トリガ指定なし */
		TrcTrg->TrgEdge  = 0;
		TrcTrg->TrgBufx  = NULL;
		TrcTrg->TrgVsft  = 0;
		TrcTrg->TrgLevel = 0;
		TrcTrg->TrgSel   = 0;
	}
#endif
#endif	/* <S0C9> Delete End */


/* <S0C9> Start */
//	LONG x,y,z; /* <Oka02> */
	LONG	TrgDataSel;
	LONG	TrgTypeSel;
	LONG	TrgDataSel2;
	LONG	TrgTypeSel2;
	TRCTRG	*TrcTrg;

#if 0	/* <S142> */
	TrcTrg = &TrcExeP->TrcTrg[TrgNo];

/* <Oka02> */
	if( TrgNo == MULTI_TRG )
	{
		TrcReg->TrgSel   = TrcReg->MTrgSel;
		TrcReg->TrgLevel = TrcReg->MTrgLevel;
	}
/* <Oka02> */

	/* トリガ情報取得 */
	TrgDataSel = (TrcReg->TrgSel & 0x000F);				/* トリガデータ選択		*/
	TrgTypeSel = (TrcReg->TrgSel >> 4) & 0x000F;		/* トリガ条件選択		*/
	TrgDataSel2 = (TrcReg->TrgSel >> 8) & 0x000F;		/* トリガデータ選択2	*/
	TrgTypeSel2 = (TrcReg->TrgSel >> 12) & 0x000F;		/* トリガ対象選択2		*/
#else
	USHORT	TrgSel;
	LONG	TrgLevel;
	
	TrcTrg = &TrcExeP->TrcTrg[TrgNo];

	if( TrgNo == SINGLE_TRG )
	{
		TrgSel   = TrcReg->TrgSel;
		TrgLevel = TrcReg->TrgLevel;
	}
	else
	{
		TrgSel   = TrcReg->MTrgSel;
		TrgLevel = TrcReg->MTrgLevel;
	}

	/* トリガ情報取得 */
	TrgDataSel = (TrgSel & 0x000F);				/* トリガデータ選択		*/
	TrgTypeSel = (TrgSel >> 4) & 0x000F;		/* トリガ条件選択		*/
	TrgDataSel2 =(TrgSel >> 8) & 0x000F;		/* トリガデータ選択2	*/
	TrgTypeSel2 = (TrgSel >> 12) & 0x000F;		/* トリガ対象選択2		*/
#endif

/* <S0D2> Start */
#if 0
/* 処理造り変えのためコメントアウト Start */
	if( TrgDataSel != 0 )
	{	/* トリガ指定あり */

		if( TrgDataSel < 15 )
		{	/* トリガ2未使用 */
			if( TrgTypeSel == NET_TRG_SEL )
			{ /* ネットワークトリガ */
				TrcTrg->TrgEdge  = 0;
				TrcTrg->TrgBitSetMsk = 0;
				TrcTrg->TrgBufx  = NULL;
				TrcTrg->TrgVsft  = 0;
				TrcTrg->TrgLevel = 0;
				TrcTrg->TrgSel   = TrcReg->TrgSel;
			}
			else
			{
				if( (1 <= TrgDataSel)&&(TrgDataSel <= 8) )
				{ /* 数値レベルトリガ */
					TrcTrg->TrgEdge  = (USHORT)TrgTypeSel;
					TrcTrg->TrgBitSetMsk = 0;
					TrcTrg->TrgBufx  = TrcExeP->TrcObj[TrgDataSel-1].TrcBuf;
					TrcTrg->TrgVsft  = (TrcExeP->TrcDataType == DATATRACE_SHORT) ? ( 17-Adj ) : ( 1-Adj );
					TrcTrg->TrgLevel = TrcReg->TrgLevel;
					TrcTrg->TrgSel   = ((TrcTrg->TrgEdge == 0)||(TrcTrg->TrgBufx == NULL))? 0 : TrcReg->TrgSel;
				}
				else if( (TrgDataSel == 9)&&(1 <= TrcReg->TrgLevel)&&(TrcReg->TrgLevel <= 7) )
				{ /* ビットトリガ */
					TrcTrg->TrgEdge  = (USHORT)TrgTypeSel;
					TrcTrg->TrgBitSetMsk = ( TrcExeP->TrcIOType == IODATA_MERGE ) ? TrcExeP->TrcBitSet : ( 0x01 << x );
					TrcTrg->TrgBufx  = TrcExeP->TrcObj[TrgTypeSel-1].TrcBuf;
					TrcTrg->TrgVsft  = 0;
					TrcTrg->TrgLevel = 0;
					TrcTrg->TrgSel   = ((TrcTrg->TrgEdge == 0x00)||(TrcTrg->TrgBufx == NULL))? 0 : TrcReg->TrgSel;
				}
			}
		}
		else
		{/* トリガ2使用 */
			TrcTrg->TrgEdge  = (USHORT)TrgTypeSel2;
			TrcTrg->TrgBitSetMsk = 0;
			TrcTrg->TrgBufx  = TrcExeP->TrcObj[TrgDataSel2-1].TrcBuf;
			TrcTrg->TrgVsft  = (TrcExeP->TrcDataType == DATATRACE_SHORT) ? ( 17-Adj ) : ( 1-Adj );
			TrcTrg->TrgLevel = TrcReg->TrgLevel;
			TrcTrg->TrgSel   = ((TrcTrg->TrgEdge == 0)||(TrcTrg->TrgBufx == NULL))? 0 : TrcReg->TrgSel;
		}
	}
	else
	{ /* トリガ指定なし */
		TrcTrg->TrgEdge  = 0;
		TrcTrg->TrgBitSetMsk = 0;
		TrcTrg->TrgBufx  = NULL;
		TrcTrg->TrgVsft  = 0;
		TrcTrg->TrgLevel = 0;
		TrcTrg->TrgSel   = 0;
	}
/* 処理造り変えのためコメントアウト End */
#endif
/* <S0C9> End */


/*****************************************/
/*	ここから作り直した処理 <S0D2>		*/
/****************************************/
	if( TrgTypeSel == NET_TRG_SEL )
	{ /* ネットワークトリガ */
		TrcTrg->TrgEdge  = 0;
		TrcTrg->TrgBitSetMsk = 0;
		TrcTrg->TrgBufx  = NULL;
		TrcTrg->TrgVsft  = 0;
		TrcTrg->TrgLevel = 0;
//		TrcTrg->TrgSel   = TrcReg->TrgSel;
		TrcTrg->TrgSel   = TrgSel;	/* <S142> */
	}
	else
	{ /* ネットワークトリガ以外 */
		if( TrgDataSel != 0 )
		{ /* トリガ指定あり */
			if( TrgDataSel < 15 )
			{	/* トリガ2未使用 */
					if( (1 <= TrgDataSel)&&(TrgDataSel <= 8) )
					{ /* 数値レベルトリガ */
						TrcTrg->TrgEdge  = (USHORT)TrgTypeSel;
						TrcTrg->TrgBitSetMsk = 0;
						TrcTrg->TrgBufx  = TrcExeP->TrcObj[TrgDataSel-1].TrcBuf;
						TrcTrg->TrgVsft  = (TrcExeP->TrcDataType == DATATRACE_SHORT) ? ( 17-Adj ) : ( 1-Adj );
//						TrcTrg->TrgLevel = TrcReg->TrgLevel;
						TrcTrg->TrgLevel = TrgLevel;			/* <S142> */
//						TrcTrg->TrgSel   = ((TrcTrg->TrgEdge == 0)||(TrcTrg->TrgBufx == NULL))? 0 : TrcReg->TrgSel;
						TrcTrg->TrgSel   = ((TrcTrg->TrgEdge == 0)||(TrcTrg->TrgBufx == NULL))? 0 : TrgSel;		/* <S142> */
					}
//					else if( (TrgDataSel == 9)&&(1 <= TrcReg->TrgLevel)&&(TrcReg->TrgLevel <= 7) )
					else if( (TrgDataSel == 9)&&(1 <= TrgLevel)&&(TrgLevel <= 7) )	/* <S142> */
					{ /* ビットトリガ */
						TrcTrg->TrgEdge  = (USHORT)TrgTypeSel;
//						TrcTrg->TrgBitSetMsk = ( TrcExeP->TrcIOType == IODATA_MERGE ) ? TrcExeP->TrcBitSet : ( 0x01 << x ); /* <Oka02> */
						TrcTrg->TrgBitSetMsk = ( TrcExeP->TrcIOType == IODATA_MERGE ) ? TrcExeP->TrcBitSet : ( 0x01 << TrgDataSel ); /* <Oka02> */
						TrcTrg->TrgBufx  = TrcExeP->TrcObj[TrgDataSel-1].TrcBuf;
						TrcTrg->TrgVsft  = 0;
						TrcTrg->TrgLevel = 0;
//						TrcTrg->TrgSel   = ((TrcTrg->TrgEdge == 0x00)||(TrcTrg->TrgBufx == NULL))? 0 : TrcReg->TrgSel;
						TrcTrg->TrgSel   = ((TrcTrg->TrgEdge == 0x00)||(TrcTrg->TrgBufx == NULL))? 0 : TrgSel;	/* <S142> */
					}
			}
			else
			{ /* トリガ2使用 */
				if( TrgTypeSel2 == 0 )
				{/* 数値レベルトリガ */
						TrcTrg->TrgEdge  = (USHORT)TrgTypeSel;
						TrcTrg->TrgBitSetMsk = 0;
						TrcTrg->TrgBufx  = TrcExeP->TrcObj[TrgDataSel2].TrcBuf;
						TrcTrg->TrgVsft  = (TrcExeP->TrcDataType == DATATRACE_SHORT) ? ( 17-Adj ) : ( 1-Adj );
//						TrcTrg->TrgLevel = TrcReg->TrgLevel;
						TrcTrg->TrgLevel = TrgLevel;	/* <S142> */
//						TrcTrg->TrgSel   = ((TrcTrg->TrgEdge == 0)||(TrcTrg->TrgBufx == NULL))? 0 : TrcReg->TrgSel;
						TrcTrg->TrgSel   = ((TrcTrg->TrgEdge == 0)||(TrcTrg->TrgBufx == NULL))? 0 : TrgSel;	/* <S142> */
				}
				else
				{ /* ビットトリガ */
					TrcTrg->TrgEdge  = (USHORT)TrgTypeSel;
//					TrcTrg->TrgBitSetMsk = ( TrcExeP->TrcIOType == IODATA_MERGE ) ? TrcExeP->TrcBitSet : ( 0x01 << x ); /* <Oka02> */
					TrcTrg->TrgBitSetMsk = ( TrcExeP->TrcIOType == IODATA_MERGE ) ? TrcExeP->TrcBitSet : ( 0x01 << TrgDataSel2 ); /* <Oka02> */
//					TrcTrg->TrgBufx  = TrcExeP->TrcObj[TrgDataSel2].TrcBuf;		/* <Oka02> */
					TrcTrg->TrgBufx  = TrcExeP->TrcObj[TrcReg->TrcMode].TrcBuf;	/* <Oka02> */
					TrcTrg->TrgVsft  = 0;
					TrcTrg->TrgLevel = 0;
//					TrcTrg->TrgSel   = ((TrcTrg->TrgEdge == 0x00)||(TrcTrg->TrgBufx == NULL))? 0 : TrcReg->TrgSel;
					TrcTrg->TrgSel   = ((TrcTrg->TrgEdge == 0x00)||(TrcTrg->TrgBufx == NULL))? 0 : TrgSel;	/* <S142> */
				}
			}
		}
		else
		{ /* トリガ指定なし */
			TrcTrg->TrgEdge  = 0;
			TrcTrg->TrgBitSetMsk = 0;
			TrcTrg->TrgBufx  = NULL;
			TrcTrg->TrgVsft  = 0;
			TrcTrg->TrgLevel = 0;
			TrcTrg->TrgSel   = 0;
		}
	}
/* <S0D2> End */

}

/****************************************************************************************************/
/*																									*/
/*		データトレース トリガ変数クリア																*/
/*																									*/
/****************************************************************************************************/
static void dtrcClearTrgParameter( TRCTRG *TrcTrg )
{
	TrcTrg->TrgSel   = 0;
	TrcTrg->TrgEdge  = 0;
	TrcTrg->TrgVsft  = 0;
	TrcTrg->TrgLevel = 0;
	TrcTrg->TrgBitSetMsk = 0;
	TrcTrg->TrgBufx  = NULL;
}

/****************************************************************************************************/
/*																									*/
/*		データトレース 軸番号変換関数																*/
/*																									*/
/****************************************************************************************************/
static void* dtrcConvAxisAddress( MEMOBUS_ADDR_MAP *AxisMap, LONG Address, LONG *AxisNum )
{
	LONG i;
//	for(i = 0; i < NumberOfAxes; i++)
	for(i = 0; i < MAX_AXIS_NUMBER; i++)
	{
		if(AxisMap[i].AxisAddress == Address)
		{
			*AxisNum = i;
			return AxisMap[i].Axis;
		}
	}
	*AxisNum = -1;
	return NULL;
}

/****************************************************************************************************/
/*																									*/
/*		データトレース AXIS先頭アドレス取得関数														*/
/*																									*/
/****************************************************************************************************/
static void* dtrcGetTopAxisAddress( MEMOBUS_ADDR_MAP *AxisMap )
{
	return AxisMap[0].Axis;
}

/****************************************************************************************************/
/*																									*/
/*		データトレース対象個別パラメータ計算処理													*/
/*																									*/
/****************************************************************************************************/
BOOL	dtrcCalcTraceObject( TRCHNDL *TrcHndl, LONG Tno )
{
	LONG	i,AxisAddress;
	LONG	TrcSelNum,AxisNum;
	void	*AxisPtr;

	TRCREG	*TrcReg;
	TRCEXEP	*TrcExeP;
	TRCOBJ	*TrcObj;
	TRCINDIVPRM	*TrcIncivPrm;				/* <S0C9> */

	TrcReg  = &TrcHndl->TrcReg;
	TrcExeP = &TrcHndl->TrcExeP;
	TrcObj  = &TrcHndl->TrcExeP.TrcObj[Tno];
	TrcIncivPrm = &TrcHndl->TrcIncivPrm;	/* <S0C9> */
	AxisPtr = NULL;

	/*----------------------------------------------------------------------------------------------*/
	/*		トレース選択チェック																	*/
	/*----------------------------------------------------------------------------------------------*/
	if( TrcObj->TrcBuf == NULL )
	{
		return( TRUE );
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		トレースオブジェクトの初期化															*/
	/*----------------------------------------------------------------------------------------------*/
	TrcObj->TrcExeflg = 0x00;
	TrcObj->Adr.LongAdrData = NULL;
	MlibResetByteMemory( &TrcObj->TrcV, sizeof(TrcObj->TrcV) );
	MlibResetByteMemory( &TrcObj->TrcB, sizeof(TrcObj->TrcB) );
	/*----------------------------------------------------------------------------------------------*/
	/*		数値トレース対象データ設定																*/
	/*----------------------------------------------------------------------------------------------*/
/* <S0C9>	Start */
//	TrcSelNum = TrcReg->TrcSel[Tno] & 0x00FF;
//	AxisAddress = (TrcReg->TrcSel[Tno] >> 8) & 0x00FF;					

	TrcSelNum = TrcIncivPrm->TrcData[Tno];
	AxisAddress = TrcIncivPrm->TrcAxis[Tno];
/* <S0C9>	End */
	AxisPtr = dtrcConvAxisAddress( TrcHndl->AxisMap, AxisAddress, &AxisNum );
	if( (TrcSelNum == 0x7F) || (TrcSelNum == 0xFF) )							/* 実アドレス指定	*/
	{		
		if( (LONG*)(TrcReg->TrcAdr[Tno]) == 0x00000000 )
		{
			/* アドレス指定が初期値の場合はダミー変数をトレース */
			TrcObj->TrcV.GetData = GetVarData_Dummy;
		}
		else
		{
			if( TrcExeP->TrcDataType == DATATRACE_SHORT )
			{
				/* ショートデータトレース	*/
				TrcObj->Adr.ShortAdrData = (USHORT*)TrcReg->TrcAdr[Tno];	
				TrcObj->TrcV.GetData = GetVarData_AdrShortData;
			}
			else
			{
				/* ロングデータトレース		*/
				TrcObj->Adr.LongAdrData = TrcReg->TrcAdr[Tno];	
				TrcObj->TrcV.GetData = GetVarData_AdrLongData;
			}
		}
		TrcObj->TrcV.AvrgSel = FALSE;
		TrcObj->TrcV.VxSftL = 0;
		TrcObj->TrcV.VxSftR = ( TrcExeP->TrcDataType == DATATRACE_SHORT )? 16 : 0;
		TrcObj->TrcV.VxGain = 1;
		TrcObj->TrcV.AxisPtr = dtrcGetTopAxisAddress( TrcHndl->AxisMap );
/* <S0C9>	Start */
//		TrcReg->IOTrcSel[Tno] = 0x00;
		TrcIncivPrm->IOTrcData[Tno] = 0x00;
/* <S0C9>	End */	}
	else if( (AxisNum < 0) || (AxisNum > (MAX_AXIS_NUMBER - 1)) )				/* 軸設定範囲外		*/
	{	
		return( FALSE );
	}
	else if( (i = DtrcSearchTrcVarTbl( TrcHndl, TrcSelNum )) >= 0 )				/* 数値トレース設定	*/
	{
		/*------------------------------------------------------------------------------------------*/
		/*	トレース平均化選択（TRUE⇒平均化する、FALSE⇒平均化しない）								*/
		/*------------------------------------------------------------------------------------------*/
		TrcObj->TrcV.AvrgSel = TrcVarTbl[i].AvrgSel;
		/*------------------------------------------------------------------------------------------*/
		/*	トレース変数を取得する関数ポインタを設定												*/
		/*------------------------------------------------------------------------------------------*/
		if( NULL != TrcVarTbl[i].GetVarData )
		{
			TrcObj->TrcV.GetData = TrcVarTbl[i].GetVarData;
			TrcObj->TrcV.AxisPtr = AxisPtr;
		}
		else
		{
			TrcObj->TrcV.GetData = GetVarData_Dummy;
			TrcObj->TrcV.AxisPtr = NULL;
		}
		/*------------------------------------------------------------------------------------------*/
		/* トレース変数のゲインを計算する関数ポインタを設定（デフォルト:ゲイン=1）					*/
		/*------------------------------------------------------------------------------------------*/
		if( NULL != TrcVarTbl[i].GetVarGain && TrcObj->TrcV.AxisPtr != NULL )
		{
			TrcObj->TrcV.VxGain = TrcVarTbl[i].GetVarGain( TrcObj->TrcV.AxisPtr );
		}
		else
		{
			TrcObj->TrcV.VxGain = 1;
		}
		/*------------------------------------------------------------------------------------------*/
		/*	ビットシフト量を設定																	*/
		/*------------------------------------------------------------------------------------------*/
		TrcObj->TrcV.VxSftL = TrcVarTbl[i].VxSftL;
		TrcObj->TrcV.VxSftR = TrcVarTbl[i].VxSftR;
	}
	else
	{	/* トレース対象なし */
		return( FALSE );
	}

#if 0 /* アドレスが奇数となる場合あり @@CHECK */  /* <S001> */
	/*----------------------------------------------------------------------------------------------*/
	/*		数値トレース変数アドレスチェック														*/
	/*----------------------------------------------------------------------------------------------*/
	if( (LONG)TrcObj->TrcV.GetData & 0x01 )
	{	/* 設定アドレスエラー */
		TrcObj->TrcExeflg  = 0x00;
		TrcObj->Adr.LongAdrData = NULL;
		MlibResetByteMemory( &TrcObj->TrcV, sizeof(TrcObj->TrcV) );
		return( FALSE );
	}
#endif /* <S001> */

	/*----------------------------------------------------------------------------------------------*/
	/*		ビットトレース対象データ設定															*/
	/*----------------------------------------------------------------------------------------------*/
/* <S0C9>	Start */
//	TrcSelNum = (TrcReg->IOTrcSel[Tno] & 0xFF);
//	AxisAddress = (TrcReg->IOTrcSel[Tno] >> 8) & 0xFF;

	TrcSelNum = TrcIncivPrm->IOTrcData[Tno];
	AxisAddress = TrcIncivPrm->IOTrcAxis[Tno];
/* <S0C9>	End */

	AxisPtr = dtrcConvAxisAddress( TrcHndl->AxisMap, AxisAddress, &AxisNum );

	if( (AxisNum < 0) || (AxisNum > MAX_AXIS_NUMBER - 1) )					/* 軸設定範囲外			*/
	{	
		return( FALSE );
	}
	if( (i = dtrcSearchTrcBitTbl( TrcHndl, TrcSelNum )) >= 0 )				/* ビットトレース設定	*/
	{
		if( NULL != TrcBitTbl[i].GetBitData )
		{
			TrcObj->TrcB.GetData = TrcBitTbl[i].GetBitData;
			TrcObj->TrcB.AxisPtr = AxisPtr;
		}
		else
		{
			TrcObj->TrcB.GetData = NULL;
			TrcObj->TrcB.AxisPtr = NULL;
		}
	}
	else
	{
		return( FALSE );
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		トレース実行フラグ設定																	*/
	/*----------------------------------------------------------------------------------------------*/
	TrcObj->TrcExeflg = 0x01;
	return( TRUE );
}

/****************************************************************************************************/
/*																									*/
/*		データトレース対象変数テーブル検索処理														*/
/*																									*/
/****************************************************************************************************/
LONG	DtrcSearchTrcVarTbl( TRCHNDL *TrcHndl, LONG SelNo )
{
	LONG	i,w;
	LONG	EntNum;

	const	TVXDEF *TvxTbl;

	TRCMNGP	*TrcMngP;
	TrcMngP = &TrcHndl->TrcMngP;

	/*----------------------------------------------------------------------------------------------*/
	/*		検索用Dataの設定																		*/
	/*----------------------------------------------------------------------------------------------*/
	TvxTbl = TrcVarTbl;										/* 定義テーブルポインタ					*/
	EntNum = TrcMngP->TrcVarTblEntNum;						/* 定義テーブル登録個数					*/
	w = TrcMngP->TrcVarTblSrhIniW;							/* 分割幅初期値設定						*/
	/*----------------------------------------------------------------------------------------------*/
	/*		検索開始Indexの設定																		*/
	/*----------------------------------------------------------------------------------------------*/
		i = ( SelNo >= TvxTbl[w].SelNo )? (EntNum - w) : 0;	/* 検索開始Index設定					*/
	/*----------------------------------------------------------------------------------------------*/
	/*		テーブル検索																			*/
	/*----------------------------------------------------------------------------------------------*/
	for( w = w>>1; w > 0; w = w>>1 )						/* 検索Loop								*/
	{
		if( SelNo >= TvxTbl[i+w].SelNo ){ i = i + w; }		/* 比較＆Index更新						*/
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		検索結果のチェック																		*/
	/*----------------------------------------------------------------------------------------------*/
	if( SelNo == TvxTbl[i].SelNo )							/* 検索結果チェック						*/
	{
		return( i );										/* 検索完了								*/
	}
	else
	{
		return( -1 );										/* 検索失敗(SelNo無し)					*/
	}

}

/****************************************************************************************************/
/*																									*/
/*		データトレース対象ビットテーブル検索処理													*/
/*																									*/
/****************************************************************************************************/
LONG	dtrcSearchTrcBitTbl( TRCHNDL *TrcHndl, LONG SelNo )
{
	LONG	i,w;
	LONG	EntNum;

	const	TBXDEF* TbxTbl;
	
	TRCMNGP	*TrcMngP;
	TrcMngP = &TrcHndl->TrcMngP;

	/*----------------------------------------------------------------------------------------------*/
	/*		検索用Dataの設定																		*/
	/*----------------------------------------------------------------------------------------------*/
	TbxTbl = TrcBitTbl;										/* 定義テーブルポインタ					*/
	EntNum = TrcMngP->TrcBitTblEntNum;						/* 定義テーブル登録個数					*/
	w = TrcMngP->TrcBitTblSrhIniW;							/* 分割幅初期値設定						*/
	/*----------------------------------------------------------------------------------------------*/
	/*		検索開始Indexの設定																		*/
	/*----------------------------------------------------------------------------------------------*/
	i = ( SelNo >= TbxTbl[w].SelNo )? (EntNum - w) : 0;		/* 検索開始Index設定					*/
	/*----------------------------------------------------------------------------------------------*/
	/*		テーブル検索																			*/
	/*----------------------------------------------------------------------------------------------*/
	for( w = w>>1; w > 0; w = w>>1 )						/* 検索Loop								*/
	{
		if( SelNo >= TbxTbl[i+w].SelNo ){ i = i + w;}		/* 比較＆Index更新						*/
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		検索結果のチェック																		*/
	/*----------------------------------------------------------------------------------------------*/
	if( SelNo == TbxTbl[i].SelNo )							/* 検索結果チェック						*/
	{
		return( i );										/* 検索完了								*/
	}
	else
	{
		return( -1 );										/* 検索失敗(SelNo無し)					*/
	}
}

/****************************************************************************************************/
/*																									*/
/*		トレース操作モードレジスタ書込み処理														*/
/*																									*/
/****************************************************************************************************/
PRM_RSLT	DtrcWriteOpeModeTrcRegister( TRCHNDL *TrcHndl, USHORT WriteData )
{
	/*----------------------------------------------------------------------------------------------*/
	/*		通常モード設定																			*/
	/*----------------------------------------------------------------------------------------------*/
	if( WriteData == 0 )
	{
		dtrcFinishOpeModeOperation( TrcHndl );				/* 操作モード操作終了処理				*/
		return( PRM_RSLT_SUCCESS );
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		トレースモード設定：単発 or 連続														*/
	/*----------------------------------------------------------------------------------------------*/
	else if( (WriteData == OPEMODE_DATATRACE) || (WriteData == OPEMODE_RINGTRACE) )
	{
		if ( TrcHndl->TrcExeV.Opened == TRUE )
		{
			return ( PRM_RSLT_CALC_ERR );
		}
		/*------------------------------------------------------------------------------------------*/
		/* @@ PrmCopy機能がTraceBufferを使用するため、排他にする 									*/
		/*------------------------------------------------------------------------------------------*/
		//else if ( CTRCMEM->GetBuffer( CTRCMEM, (void *)TrcHndl ) == NULL )
		//{
		//	return ( PRM_RSLT_NOACCRIGHT_ERR );								/* 要確認 */
		//}
		/*------------------------------------------------------------------------------------------*/
		else
		{
			TrcHndl->TrcExeV.Opened = TRUE;
			TrcHndl->TrcExeV.OpmRun = TRUE;
			return( PRM_RSLT_SUCCESS );
		}
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		その他																					*/
	/*----------------------------------------------------------------------------------------------*/
	else
	{
		return( PRM_RSLT_CALC_ERR );
	}
}

/****************************************************************************************************/
/*																									*/
/*		操作モード操作終了処理																		*/
/*																									*/
/****************************************************************************************************/
static BOOL	dtrcFinishOpeModeOperation( TRCHNDL *TrcHndl )
{
	TRCREG	*TrcReg;
	TrcReg = &TrcHndl->TrcReg;

	/*----------------------------------------------------------------------------------------------*/
	/*		モードチェック＆停止処理																*/
	/*----------------------------------------------------------------------------------------------*/
	if( TrcReg->OpeModeTrc == OPEMODE_NOCMD )
	{														
		dtrcStopDataTrace( TrcHndl );								/* コマンドなしの場合の処理		*/
	}
	else if( TrcReg->OpeModeTrc == OPEMODE_DATATRACE )
	{														
		dtrcStopDataTrace( TrcHndl );								/* 単発データトレース停止処理	*/
	}
	else if( TrcReg->OpeModeTrc == OPEMODE_RINGTRACE )				
	{
		dtrcStopRingTrace( TrcHndl, TRUE );							/* 連続データトレース停止処理	*/
	}
	//TrcHndl->TrcReg.OpeModeTrc = 0x0000;							/* リセット操作モード			*/
	TrcHndl->TrcReg.OpeCmdTrc  = 0x0000;							/* リセット操作コマンド			*/

	//CTRCMEM->FreeBuffer( CTRCMEM, (void *)TrcHndl );				/* @@ Free TraceBuffer			*/
	return( TRUE );
}

/****************************************************************************************************/
/*																									*/
/*		トレース操作設定レジスタ書込み処理															*/
/*																									*/
/****************************************************************************************************/
PRM_RSLT	DtrcWriteOpeCmdTrcRegister( TRCHNDL *TrcHndl, USHORT *TraceBuffer, USHORT WriteData )
{
	PRM_RSLT ErrRes;
	LONG size;

	TRCREG	*TrcReg;
	TrcReg = &TrcHndl->TrcReg;

	/*----------------------------------------------------------------------------------------------*/
	/*		トレースモード：単発 or 連続															*/
	/*----------------------------------------------------------------------------------------------*/
	if( (TrcReg->OpeModeTrc == OPEMODE_DATATRACE ) || (TrcReg->OpeModeTrc == OPEMODE_RINGTRACE) )
	{
		/* バッファサイズチェック */
//#if CSW_NET_TRG_USE == TRUE		/* <S012> *//* <S021> */
//		size = TrcReg->AreaSize;
//#else /* CSW_NET_TRG_USE */
//		size = TrcReg->TrcMode * TrcReg->AreaSize;	/* <S0C9>	Delete */
		size = TrcReg->AreaSize;					/* <S0C9> */
//#endif /* CSW_NET_TRG_USE */
		if(size > TRC_BUF_SIZE)
		{
			return PRM_RSLT_CALC_ERR;
		}

		/* トレース操作コマンド設定処理 */
		if( TRUE == dtrcSetDataTraceCmd( TrcHndl, TrcReg->OpeModeTrc, WriteData ) )
		{ 
			ErrRes = PRM_RSLT_SUCCESS;
		}
		else
		{
			ErrRes = PRM_RSLT_CALC_ERR;		
		}
		return( ErrRes );
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		その他のモード																			*/
	/*----------------------------------------------------------------------------------------------*/
	else
	{
		return( PRM_RSLT_CALC_ERR );
	}
}

/****************************************************************************************************/
/*																									*/
/*		トレース操作コマンド設定処理																*/
/*																									*/
/****************************************************************************************************/
static BOOL	dtrcSetDataTraceCmd( TRCHNDL *TrcHndl, ULONG OpeModeTrc, ULONG WriteData )
{
	LONG	rc;
	TRCREG	*TrcReg;
	TrcReg = &TrcHndl->TrcReg;

	/*----------------------------------------------------------------------------------------------*/
	/*		トレースモード：単発																	*/
	/*----------------------------------------------------------------------------------------------*/
	if( TrcReg->OpeModeTrc == OPEMODE_DATATRACE )
	{
		if( WriteData == 0x0000 )
		{
			rc = dtrcStopDataTrace( TrcHndl );						/* 単発データトレース停止処理	*/
		}
		else if( WriteData == 0x0001 )
		{
			rc = dtrcStartDataTrace( TrcHndl, TRCCMD_DATATRACE );	/* 単発データトレース開始処理	*/
		}
		else if ( WriteData == 0x0011 )					
		{
			rc = dtrcStartDataTrace( TrcHndl, TRCCMD_CONTTRACE );
		}
		else
		{
			return( FALSE );										/* コマンド異常					*/
		}
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		トレースモード：連続																	*/
	/*----------------------------------------------------------------------------------------------*/
	else
	{
		if( WriteData == 0x0000 )
		{
			rc = dtrcStopRingTrace( TrcHndl, FALSE );				/* 連続データトレース停止処理	*/
		}
		else if( WriteData == 0x0001 )
		{
			rc = dtrcStartRingTrace( TrcHndl );						/* 連続データトレース開始処理	*/
		}
		else
		{
			return( FALSE );										/* コマンド異常					*/
		}
	}
	return( rc );
}

/****************************************************************************************************/
/*																									*/
/*		単発データトレーススタート処理																*/
/*																									*/
/****************************************************************************************************/
static BOOL	dtrcStartDataTrace( TRCHNDL *TrcHndl, UCHAR mode )
{
	/*----------------------------------------------------------------------------------------------*/
	/*		単発トレーススタートチェック															*/
	/*----------------------------------------------------------------------------------------------*/
	if ( (TrcHndl->TrcExeV.TrcCmd != TRCCMD_CONTTRACE) || (TrcHndl->TrcExeV.OpmRun != TRUE) )
	{
		if( TrcHndl->TrcExeV.TrcCmd != TRCCMD_NOCMD )
		{
			return( FALSE );
		}
		else if( TrcHndl->TrcExeP.TrcPrmOk != TRUE )
		{
			return( FALSE );
		}
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		単発トレーススタート処理																*/
	/*----------------------------------------------------------------------------------------------*/
	DtrcResetAverageVar( TrcHndl );							/* 平均化変数初期化						*/
	TrcHndl->TrcReg.TrcState = 0;							/* トレース実行状態リセット				*/
	//TrcHndl->TrcExeP.TrcObj[0].TrcExeflg &= 0x0F;			/* トレースＣＨ０実行許可				*/
	TrcHndl->TrcExeV.TrcCmd = mode;							/* 単発トレース実行指令					*/
	DtrcInitExecute( TrcHndl );								/* データトレース実行制御 初期化処理	*/
															/* トレース実行指令(orモード維持)		*/

	
	return( TRUE );
}

/****************************************************************************************************/
/*																									*/
/*		単発データトレースストップ処理																*/
/*																									*/
/****************************************************************************************************/
static BOOL	dtrcStopDataTrace( TRCHNDL *TrcHndl )
{
		TrcHndl->TrcExeV.TrcCmd = TRCCMD_NOCMD;
		TrcHndl->TrcExeV.OpmRun = FALSE;
		TrcHndl->TrcExeV.Opened = FALSE;
	
		return( TRUE );
}

/****************************************************************************************************/
/*																									*/
/*		連続データトレーススタート処理																*/
/*																									*/
/****************************************************************************************************/
static BOOL	dtrcStartRingTrace( TRCHNDL *TrcHndl )
{
		/*------------------------------------------------------------------------------------------*/
		/*		連続トレーススタートチェック														*/
		/*------------------------------------------------------------------------------------------*/
		if( TrcHndl->TrcExeV.TrcCmd != TRCCMD_NOCMD )
		{
			return( FALSE );
		}
		else if( TrcHndl->TrcExeP.TrcPrmOk != TRUE )
		{
			return( FALSE );
		}

		/*------------------------------------------------------------------------------------------*/
		/*		連続トレーススタート処理															*/
		/*------------------------------------------------------------------------------------------*/
		DtrcResetAverageVar( TrcHndl );							/* 平均化変数初期化					*/
		TrcHndl->TrcReg.TrcState   = 0;							/* トレース実行状態リセット			*/
		//TrcHndl->TrcReg.RngTrcPtr  = 0;						/* 連続トレース書込み位置リセット	*/
		//TrcHndl->TrcExeV.RngTrcCnt = 0;						/* 連続トレース書込み回数リセット	*/
		//TrcHndl->TrcExeP.TrcObj[0].TrcExeflg &= 0x0F;			/* トレースＣＨ０実行許可			*/
		TrcHndl->TrcExeV.TrcCmd = TRCCMD_RINGTRACE;				/* 連続トレース実行指令				*/
		DtrcInitExecuteRingTrc( TrcHndl );

		return( TRUE );

}

/****************************************************************************************************/
/*																									*/
/*		連続データトレースストップ処理																*/
/*																									*/
/****************************************************************************************************/
static BOOL	dtrcStopRingTrace( TRCHNDL *TrcHndl, LONG finmode )
{
		TrcHndl->TrcExeV.TrcCmd  = TRCCMD_NOCMD;
		TrcHndl->TrcExeV.OpmRun  = FALSE;
		TrcHndl->TrcReg.TrcState = 3;

		if(finmode == TRUE)	
		{
			TrcHndl->TrcExeV.Opened = FALSE;
		}
		return( TRUE );
}

/****************************************************************************************************/
/*																									*/
/*		データトレース実行制御　初期化処理（単発データトレース）									*/
/*																									*/
/****************************************************************************************************/
void DtrcInitExecute( TRCHNDL *TrcHndl )
{
	TrcHndl->TrcReg.TrgPoint = 0;
	TrcHndl->TrcExeV.TrcIndx = 0;
	TrcHndl->TrcExeV.TrcStep = ((TrcHndl->TrcExeP.TrcTrg[0].TrgSel != 0x0000) 
								|| (TrcHndl->TrcExeP.TrcTrg[1].TrgSel != 0x0000)) ? 1 : 5;
}

/****************************************************************************************************/
/*																									*/
/*		データトレース実行制御　初期化処理（連続データトレース）									*/
/*																									*/
/****************************************************************************************************/
void DtrcInitExecuteRingTrc( TRCHNDL *TrcHndl )
{
	TrcHndl->TrcReg.TrgPoint   = 0;							/* トリガ位置リセット				*/
	TrcHndl->TrcExeV.TrcIndx   = 0;							/* トレースインデクスリセット		*/
	TrcHndl->TrcExeV.TrcStep   = 7;							/* トレース実行ステップリセット		*/
	TrcHndl->TrcExeV.RngTrcCnt = 0;							/* 連続トレース書き込み回数リセット	*/
	TrcHndl->TrcReg.RngTrcPtr  = 0;							/* 連続トレース書込み位置リセット	*/
}

/****************************************************************************************************/
/*																									*/
/*		データトレース実行制御																		*/
/*																									*/
/****************************************************************************************************/
void	DtrcExecute( TRCHNDL *TrcHndl )
{
	ULONG	ulwk;

	TRCREG	*TrcReg;
	TRCEXEP	*TrcExeP;
	TRCEXEV	*TrcExeV;

	TrcReg  = &TrcHndl->TrcReg;
	TrcExeP = &TrcHndl->TrcExeP;
	TrcExeV = &TrcHndl->TrcExeV;

	/*----------------------------------------------------------------------------------------------*/
	/*		トレース実行指令チェック																*/
	/*----------------------------------------------------------------------------------------------*/
	if( TrcExeV->TrcCmd == TRCCMD_NOCMD )
	{
		TrcExeV->TrcStep = 0;
		TrcExeV->TrcSamp = 0;
		return;
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		トレースデータ更新処理																	*/
	/*----------------------------------------------------------------------------------------------*/
	if( TrcExeV->TrcSamp >= (TrcReg->SampIntv - TrcExeP->DataSampIntv) )
	{
		DtrcSetTrcVar( TrcHndl );
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		トレース実行サンプリングチェック														*/
	/*----------------------------------------------------------------------------------------------*/
	if( (++TrcExeV->TrcSamp >= TrcReg->SampIntv) || (TrcExeV->TrcStep == 0) )
	{
		TrcExeV->TrcSamp = 0;
	}
	else
	{
		return;
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		トレース実行シーケンス制御																*/
	/*----------------------------------------------------------------------------------------------*/
	switch( TrcExeV->TrcStep )
	{
		/*------------------------------------------------------------------------------------------*/
		/*		Data Trace for PreTrigger															*/
		/*------------------------------------------------------------------------------------------*/
		case  1:
			TrcReg->TrcState = 1;
			DtrcProcedure( TrcHndl, TrcExeV->TrcIndx );
			if( ++TrcExeV->TrcIndx >= TrcReg->PtrgDnum )
			{
				TrcExeV->TrcStep = 2;
			}
			if( TrcExeV->TrcIndx >= TrcExeP->TrcDnum ){ TrcExeV->TrcIndx = 0;}
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		Data Trace and Wait Trigger															*/
		/*------------------------------------------------------------------------------------------*/
		case  2:
			TrcExeV->TrgVar[1]  = TrcExeV->TrgVar[0];
			TrcExeV->MTrgVar[1] = TrcExeV->MTrgVar[0];
			DtrcProcedure( TrcHndl, TrcExeV->TrcIndx );
			if( dtrcCheckTriggerProcedure( TrcHndl ) == TRUE )
			{
				TrcReg->TrcState = 2;
				TrcReg->TrgPoint = (USHORT)TrcExeV->TrcIndx;
				TrcExeV->TrcStep = ((TrcExeP->TrcDnum - TrcReg->PtrgDnum) > 1)? 3 : 4;
			}
			if( ++TrcExeV->TrcIndx >= TrcExeP->TrcDnum ){ TrcExeV->TrcIndx = 0;}
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		Data Trace and Check End WithTrigger												*/
		/*------------------------------------------------------------------------------------------*/
		case  3:
			DtrcProcedure( TrcHndl, TrcExeV->TrcIndx );
			if( ++TrcExeV->TrcIndx >= TrcExeP->TrcDnum ){ TrcExeV->TrcIndx = 0;}
			if( TrcReg->TrgPoint >= TrcReg->PtrgDnum )
			{
				ulwk = TrcReg->TrgPoint - TrcReg->PtrgDnum;
			}
			else
			{
				ulwk = TrcReg->TrgPoint + TrcExeP->TrcDnum - TrcReg->PtrgDnum;
			}
			if( TrcExeV->TrcIndx == ulwk )
			{
				TrcExeV->TrcStep = 4;
			}
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		Data Trace End Procedure															*/
		/*------------------------------------------------------------------------------------------*/
		case  4:
			TrcReg->TrcState = 3;
			TrcExeV->TrcStep = 0;
			TrcExeV->TrcCmd  = TRCCMD_NOCMD;
			if ( TrcExeV->TrcCmd == TRCCMD_DATATRACE ) { TrcExeV->OpmRun = FALSE; }
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		Data Trace Procedure(TriggerLess)													*/
		/*------------------------------------------------------------------------------------------*/
		case  5:
			TrcReg->TrcState = 1;
			DtrcProcedure( TrcHndl, TrcExeV->TrcIndx );
			if( ++TrcExeV->TrcIndx >= TrcExeP->TrcDnum )
			{
				TrcExeV->TrcIndx = 0;
				TrcExeV->TrcStep = 6;
			}
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		Trace End Procedure(TriggerLess)													*/
		/*------------------------------------------------------------------------------------------*/
		case  6:
			TrcReg->TrcState = 3;
			TrcExeV->TrcStep = 0;
			TrcExeV->TrcCmd  = TRCCMD_NOCMD;
			if ( TrcExeV->TrcCmd == TRCCMD_DATATRACE ) { TrcExeV->OpmRun = FALSE; }
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		Ring Data Trace																		*/
		/*------------------------------------------------------------------------------------------*/
		case  7:
			TrcReg->TrcState = 1;
			DtrcProcedure( TrcHndl, TrcExeV->TrcIndx );
			if( ++TrcExeV->TrcIndx >= TrcExeP->TrcDnum )
			{
				TrcExeV->TrcIndx = 0;
				TrcExeV->RngTrcCnt++;
			}
			TrcReg->RngTrcPtr = (TrcExeV->RngTrcCnt << 16) +TrcExeV->TrcIndx;
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		Default(No Operation)																*/
		/*------------------------------------------------------------------------------------------*/
		case  0:
		default:
			break;
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		トレース専用変数：リセット処理															*/
	/*----------------------------------------------------------------------------------------------*/
	DtrcResetAverageVar( TrcHndl );
}

/****************************************************************************************************/
/*																									*/
/*		Set Trace Variable Function																	*/
/*																									*/
/****************************************************************************************************/
void	DtrcSetTrcVar( TRCHNDL *TrcHndl )
{
	LONG i;

	TRCREG	*TrcReg;
	TRCOBJ	*Tobjpx;

	TrcReg  = &TrcHndl->TrcReg;

	for( i=0; i<TrcReg->TrcMode; i++ )
	{
		Tobjpx = &TrcHndl->TrcExeP.TrcObj[i];
		
		/* 数値トレース */
		if(Tobjpx->TrcV.AvrgSel == TRUE )
		{
			/* Add Average Trace Variable */
			Tobjpx->TrcV.VxData += Tobjpx->TrcV.GetData( Tobjpx->TrcV.AxisPtr );
		}
		else
		{
			/* Set Normal Trace Variable */
			Tobjpx->TrcV.VxData = Tobjpx->TrcV.GetData( Tobjpx->TrcV.AxisPtr );
		}

		/* ビットトレース */
		if( Tobjpx->TrcB.GetData != NULL )
		{
			/* Set Bit Trace Variable */
			Tobjpx->TrcB.BtvData = Tobjpx->TrcB.GetData( Tobjpx->TrcB.AxisPtr );
		}
		else
		{
			Tobjpx->TrcB.BtvData = FALSE;		
		}
	}
}

/****************************************************************************************************/
/*																									*/
/*		Reset Average Variable Function																*/
/*																									*/
/****************************************************************************************************/
void	DtrcResetAverageVar( TRCHNDL *TrcHndl )
{
	LONG i;

	TRCREG	*TrcReg;
	TRCOBJ	*Tobjpx;

	TrcReg  = &TrcHndl->TrcReg;

	for(i=0;i<TrcReg->TrcMode;i++)
	{
		Tobjpx = &TrcHndl->TrcExeP.TrcObj[i];
		Tobjpx->TrcV.VxData = 0;
	}
}
/****************************************************************************************************/
/*																									*/
/*		Get Trace Data (Bit Data Merged)															*/
/*																									*/
/****************************************************************************************************/
static LONG	dtrcGetMergeData( TRCEXEP *TrcExeP, TRCOBJ *Tobjpx )
{
	LONG	Data;

	if( Tobjpx->TrcExeflg == 0x01 )
	{
#if (FLOAT_USE==TRUE)
		Data = Tobjpx->TrcV.VxData * Tobjpx->TrcV.VxGain;
#else
		Data = MlibMulgainNolim( Tobjpx->TrcV.VxData , Tobjpx->TrcV.VxGain );
#endif /*FLOAT_USE==TRUE*/
		Data = ( Data << Tobjpx->TrcV.VxSftL) >> Tobjpx->TrcV.VxSftR;
		Data = MlibLIMITUL( Data, TrcExeP->UpperLmt, TrcExeP->LowerLmt );

		if( Tobjpx->TrcB.GetData != NULL )
		{
			if( Tobjpx->TrcB.BtvData )
			{
				Data = (Data | TrcExeP->TrcBitSet);
			}
			else
			{
				Data = (Data & TrcExeP->TrcBitClr);
			}
		}
		return Data;
	}
	return 0;
}

/****************************************************************************************************/
/*																									*/
/*		Get Each Trace Data ( Veriable Data and Bit Data )											*/
/*																									*/
/****************************************************************************************************/
static void	dtrcGetTrcData( TRCEXEP *TrcExeP, TRCOBJ *Tobjpx, LONG *VerData, LONG *BitData, LONG Idx )
{
	LONG	Data;
	ULONG	TrcBitSet, TrcBitClr;

	if( Tobjpx->TrcExeflg == 0x01 )
	{
#if (FLOAT_USE==TRUE)
		Data = Tobjpx->TrcV.VxData * Tobjpx->TrcV.VxGain;
#else
		Data = MlibMulgainNolim( Tobjpx->TrcV.VxData , Tobjpx->TrcV.VxGain );
#endif /* FLOAT_USE */
		Data = ( Data << Tobjpx->TrcV.VxSftL) >> Tobjpx->TrcV.VxSftR;
		*VerData = MlibLIMITUL( Data, TrcExeP->UpperLmt, TrcExeP->LowerLmt );

		TrcBitSet = (0x00000001 << Idx );
		TrcBitClr = ~TrcBitSet;

		if( Tobjpx->TrcB.BtvData )
		{
			*BitData = (*BitData | TrcBitSet);
		}
		else
		{
			*BitData = (*BitData & TrcBitClr);
		}
	}
	else
	{
		*VerData = 0;
		*BitData = 0;
	}
}

/****************************************************************************************************/
/*																									*/
/*		Data Trace Procedure																		*/
/*																									*/
/****************************************************************************************************/
void	DtrcProcedure( TRCHNDL *TrcHndl, LONG Idx )
{

	ULONG i;
	LONG VerData, BitData;
	SHORT x,y;

	TRCREG	*TrcReg;
	TRCEXEP	*TrcExeP;
	TRCEXEV	*TrcExeV;
	TRCOBJ	*Tobjpx;
	TRCOBJ	*TobjpxB;

	BitData = 0;				/* 未初期化変数修正<S191> */

	TrcReg  = &TrcHndl->TrcReg;
	TrcExeP = &TrcHndl->TrcExeP;
	TrcExeV = &TrcHndl->TrcExeV;

	/*----------------------------------------------------------------------------------------------*/
	/*		ショートデータトレース																	*/
	/*----------------------------------------------------------------------------------------------*/
	if( TrcExeP->TrcDataType == DATATRACE_SHORT )	
	{
		if( TrcExeP->TrcIOType == IODATA_MERGE )						/* 上位ビットにIOデータ格納	*/
		{
			for(i=0;i<TrcReg->TrcMode;i++)
			{
				Tobjpx = &TrcExeP->TrcObj[i];		
				VerData = dtrcGetMergeData( TrcExeP, Tobjpx );	
				((SHORT*)Tobjpx->TrcBuf)[Idx] = (SHORT)VerData;
			}
		}
		else															/* 専用IOデータ領域に格納	*/
		{
			TobjpxB = &TrcExeP->TrcObj[TrcReg->TrcMode];		

			for(i=0;i<TrcReg->TrcMode;i++)
			{
				Tobjpx = &TrcExeP->TrcObj[i];		

				dtrcGetTrcData( TrcExeP, Tobjpx, &VerData, &BitData, i );
				((SHORT*)Tobjpx->TrcBuf)[Idx]  = (SHORT)(VerData); 
				((SHORT*)TobjpxB->TrcBuf)[Idx] = (SHORT)(BitData);
			}
		}
	
		if(TrcExeP->TrcTrg[0].TrgBufx != NULL)
		{
			TrcExeV->TrgVar[0] = ((SHORT*)TrcExeP->TrcTrg[0].TrgBufx)[Idx];	/* Update Trigger1 Data	*/	
		}
		if(TrcExeP->TrcTrg[1].TrgBufx != NULL)
		{
			TrcExeV->MTrgVar[0] = ((SHORT*)TrcExeP->TrcTrg[1].TrgBufx)[Idx];/* Update Trigger2 Data	*/
		}

	}
	/*----------------------------------------------------------------------------------------------*/
	/*		ロングデータトレース																	*/
	/*----------------------------------------------------------------------------------------------*/
	else											
	{
		if( TrcExeP->TrcIOType == IODATA_MERGE )						/* 上位ビットにIOデータ格納	*/
		{
			for(i=0;i<TrcReg->TrcMode;i++)
			{
				Tobjpx = &TrcExeP->TrcObj[i];		
				VerData = dtrcGetMergeData( TrcExeP, Tobjpx );	
				((SHORT *)Tobjpx->TrcBuf)[2*Idx    ] = (SHORT)(VerData);	
				((SHORT *)Tobjpx->TrcBuf)[2*Idx + 1] = (SHORT)(VerData >> 16);	
			}
		}
		else															/* 専用IOデータ領域に格納	*/
		{
			TobjpxB = &TrcExeP->TrcObj[TrcReg->TrcMode];		

			for(i=0;i<TrcReg->TrcMode;i++)
			{
				Tobjpx = &TrcExeP->TrcObj[i];		

				dtrcGetTrcData( TrcExeP, Tobjpx, &VerData, &BitData, i );
				((SHORT*)Tobjpx->TrcBuf)[2*Idx    ] = (SHORT)(VerData);	
				((SHORT*)Tobjpx->TrcBuf)[2*Idx + 1] = (SHORT)(VerData >> 16);	

				((SHORT*)TobjpxB->TrcBuf)[2*Idx    ] = (SHORT)(BitData);	
				((SHORT*)TobjpxB->TrcBuf)[2*Idx + 1] = (SHORT)(BitData >> 16);	
			}
		}
		
		if(TrcExeP->TrcTrg[0].TrgBufx != NULL)
		{
			x = ((SHORT*)TrcExeP->TrcTrg[0].TrgBufx)[2*Idx];					
			y = ((SHORT*)TrcExeP->TrcTrg[0].TrgBufx)[2*Idx +1];
			TrcExeV->TrgVar[0] = (y << 16) + x;							/* Update Trigger1 Data		*/
		}
		if(TrcExeP->TrcTrg[1].TrgBufx != NULL)
		{
			x = ((SHORT*)TrcExeP->TrcTrg[1].TrgBufx)[2*Idx];					
			y = ((SHORT*)TrcExeP->TrcTrg[1].TrgBufx)[2*Idx+1];			
			TrcExeV->MTrgVar[0] = (y << 16) + x;						/* Update Trigger2 Data		*/
		}
	}
}

/****************************************************************************************************/
/*																									*/
/*		データトレーストリガチェック処理															*/
/*																									*/
/****************************************************************************************************/
static BOOL	dtrcCheckTriggerProcedure( TRCHNDL *TrcHndl )
{
	LONG	x,y;
	TRCREG	*TrcReg;
	TRCEXEP	*TrcExeP;
	TRCEXEV *TrcExeV;

	TrcReg  = &TrcHndl->TrcReg;
	TrcExeP = &TrcHndl->TrcExeP;
	TrcExeV = &TrcHndl->TrcExeV;

	/*----------------------------------------------------------------------------------------------*/
	/*		トリガチェック１																		*/
	/*----------------------------------------------------------------------------------------------*/
#if CSW_NET_TRG_USE == TRUE		/* <S021> */
	if(((TrcExeP->TrcTrg[0].TrgSel >> 4) & 0x000F) == NET_TRG_SEL)
	{ /* ネットワークトリガ */
		x = TrcExeV->NetTrigger;
	}
	else if((TrcExeP->TrcTrg[0].TrgSel == 0x00))
#else
	if((TrcExeP->TrcTrg[0].TrgSel == 0x00))
#endif
	{
		x = TRUE;
		if( TrcReg->MTrgMode == 1 )	/* <S148> */
		{
			x = TRUE;										/* ANDトリガ							*/
		}
		else if( TrcReg->MTrgMode == 2 )
		{
			x = FALSE;										/* ORトリガ								*/
		}
	}
	else
	{
		x = dtrcCheckTrigger( SINGLE_TRG, TrcHndl, TrcExeV->TrgVar);
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		トリガチェック２																		*/
	/*----------------------------------------------------------------------------------------------*/
	if(TrcExeP->TrcTrg[1].TrgSel == 0x00)
	{
		y = TRUE;
		if( TrcReg->MTrgMode == 1 )	/* <S148> */
		{
			y = TRUE;										/* ANDトリガ							*/
		}
		else if( TrcReg->MTrgMode == 2 )
		{
			y = FALSE;										/* ORトリガ								*/
		}
	}
	else
	{
		y = dtrcCheckTrigger( MULTI_TRG, TrcHndl, TrcExeV->MTrgVar);
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		マルチトリガチェック																	*/
	/*----------------------------------------------------------------------------------------------*/
	if(TrcReg->MTrgMode == 1)								/* ANDトリガ							*/
	{
		return( x & y );
	}
	/*----------------------------------------------------------------------------------------------*/
	else if(TrcReg->MTrgMode == 2)							/* ORトリガ								*/
	{
		return( x | y );
	}
	/*----------------------------------------------------------------------------------------------*/
#if 0 /* <S021> */
#if CSW_NET_TRG_USE == TRUE		/* <S012> */
	else if(TrcReg->MTrgMode == NET_TRG_SEL)				/* ネットワークトリガ					*/
	{
		return( TrcExeV->NetTrigger );
	}
#endif
#endif
	else													/* 単一トリガ							*/
	{
		return( x );
	}
}


/****************************************************************************************************/
/*																									*/
/*		トリガチェック処理																			*/
/*																									*/
/****************************************************************************************************/
static BOOL	dtrcCheckTrigger( LONG TrgNo, TRCHNDL *TrcHndl, LONG *TrgVar )
{
	LONG	Data0;
	LONG	Data1;
//	ULONG	x;				/* <S0D2> */
	ULONG	TrgData;		/* <S0D2> */
	ULONG	TrgEdge;		/* <S0D2> */
	ULONG	TrgTarget;		/* <S0D2> */


	TRCREG	*TrcReg;
	TRCTRG	*TrcTrg;

	TrcReg  = &TrcHndl->TrcReg;
	TrcTrg  = TrcHndl->TrcExeP.TrcTrg;
	
#if 0	/* <S142> */
//	x = (TrcReg->TrgSel >> 4) & 0x0000000F;	/* トリガ対象選択 */
	TrgData = (TrcReg->TrgSel) & 0x0000000F;			/* トリガデータ選択 */	/* <S0D2> */
	TrgEdge = (TrcReg->TrgSel >> 4 ) & 0x0000000F;		/* トリガ条件選択 */	/* <S0D2> */
	TrgTarget = (TrcReg->TrgSel >> 12) & 0x0000000F;	/* トリガ対象選択 */	/* <S0D2> */
#else
	TrgData   = (TrcTrg[TrgNo].TrgSel      ) & 0x0000000F;	/* トリガデータ選択 */
	TrgEdge   = (TrcTrg[TrgNo].TrgSel >> 4 ) & 0x0000000F;	/* トリガ条件選択 */
	TrgTarget = (TrcTrg[TrgNo].TrgSel >> 12) & 0x0000000F;	/* トリガ対象選択 */
#endif

	/* <S0D2>  Start*/
#if CSW_NET_TRG_USE == TRUE		/* <S012> */
	/*----------------------------------------------------------------------------------------------*/
	/*		ネットワークトリガチェック(チェック項目なし)											*/
	/*----------------------------------------------------------------------------------------------*/
	if( TrgEdge == NET_TRG_SEL )
	{
		return( TRUE );
	}
#endif /* CSW_NET_TRG_USE */
	/* <S0D2> End */

	/*----------------------------------------------------------------------------------------------*/
	/*		ビットトリガチェック																	*/
	/*----------------------------------------------------------------------------------------------*/
//	if( (0 < x)&&(x < TrcReg->TrcMode) )	/* <S0D2> */
	if( ( TrgData == 0x0009) || ((TrgData == 0x000f)&&(TrgTarget == 0x0001) ) ) 	/* <S0D2> */
	{
		Data0 = (TrgVar[0] ^ TrgVar[1]) & TrcTrg[TrgNo].TrgBitSetMsk;
	/*----------------------------------------------------------------------------------------------*/
		if( Data0 == 0 )									/* ビット変化なし						*/
		{
			return( FALSE );
		}
	/*----------------------------------------------------------------------------------------------*/
		if( TrcTrg[TrgNo].TrgEdge & 0x01 )					/* 立上がりエッジトリガ					*/
		{
			if( TrgVar[0] & TrcTrg[TrgNo].TrgBitSetMsk )
			{
				return( TRUE );
			}
		}
	/*----------------------------------------------------------------------------------------------*/
		if( TrcTrg[TrgNo].TrgEdge & 0x02 )					/* 立下がりエッジトリガ					*/
		{
			if( TrgVar[1] & TrcTrg[TrgNo].TrgBitSetMsk )
			{
				return( TRUE );
			}
		}
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		数値レベルトリガチェック																*/
	/*----------------------------------------------------------------------------------------------*/
	//else if(x == 0)	/* <S0D2> */
	else if( (( TrgData > 0 ) && (TrgData <= 8)) || ( (( TrgData == 0x000f ) && (TrgTarget == 0x0000)) ) ) 	/* <S0D2> */
	{
		Data0 = (LONG)(TrgVar[0] << TrcTrg[TrgNo].TrgVsft) >> TrcTrg[TrgNo].TrgVsft;
		Data1 = (LONG)(TrgVar[1] << TrcTrg[TrgNo].TrgVsft) >> TrcTrg[TrgNo].TrgVsft;
	/*----------------------------------------------------------------------------------------------*/
		if( TrcTrg[TrgNo].TrgEdge == 0x04 )					/* レベルトリガ(以上)					*/
		{
			if( Data0 >= TrcTrg[TrgNo].TrgLevel )
			{
				return( TRUE );
			}
		}
	/*----------------------------------------------------------------------------------------------*/
		if( TrcTrg[TrgNo].TrgEdge == 0x05 )					/* レベルトリガ(以下)					*/
		{
			if( Data0 <= TrcTrg[TrgNo].TrgLevel )
			{
				return( TRUE );
			}
		}
	/*----------------------------------------------------------------------------------------------*/
	/* 変化エッジも合せて処理するため、bit0,1とのAND処理とする(レベルトリガ判断を前に持っていく)	*/
	/*----------------------------------------------------------------------------------------------*/
		if( TrcTrg[TrgNo].TrgEdge & 0x01 )					/* 立上がりエッジトリガ					*/
		{
			if( (Data1 < TrcTrg[TrgNo].TrgLevel) && (Data0 >= TrcTrg[TrgNo].TrgLevel) )
			{
				return( TRUE );
			}
		}
	/*----------------------------------------------------------------------------------------------*/
		if( TrcTrg[TrgNo].TrgEdge & 0x02 )					/* 立下がりエッジトリガ					*/
		{
			if( (Data1 > TrcTrg[TrgNo].TrgLevel) && (Data0 <= TrcTrg[TrgNo].TrgLevel) )
			{
				return( TRUE );
			}
		}
	}

#if 0	/* <S0D2> Delete Start */
#if CSW_NET_TRG_USE == TRUE		/* <S012> */
	/*----------------------------------------------------------------------------------------------*/
	/*		ネットワークトリガチェック(チェック項目なし)											*/
	/*----------------------------------------------------------------------------------------------*/
	else if( x == NET_TRG_SEL )
	{
		return( TRUE );
	}
#endif /* CSW_NET_TRG_USE */
#endif	/* <S0D2> Delete End */

	/*----------------------------------------------------------------------------------------------*/
		return( FALSE );
}

/****************************************************************************************************/
/*																									*/
/*		トレースサンプリング間隔取得用API															*/
/*																									*/
/****************************************************************************************************/
ULONG	DtrcGetSampIntv( TRCHNDL *TrcHndl )
{
	return	(TrcHndl->TrcExeP.DataSampIntv);
}


/****************************************************************************************************/
/*																									*/
/*		トレースサンプリング間隔取得用API															*/
/*																									*/
/****************************************************************************************************/
ULONG	DtrcGetTrcDataNum( TRCHNDL *TrcHndl )
{
	return	(TrcHndl->TrcExeP.TrcDnum);
}


/****************************************************************************************************/
/*																									*/
/*		トレーストリガ位置零設定用API																*/
/*																									*/
/****************************************************************************************************/
void	DtrcSetTrgPointToZero( TRCHNDL *TrcHndl )
{
	TrcHndl->TrcReg.TrgPoint = 0;
}


#if 0
/****************************************************************************************************/
/*																									*/
/*		トレースサンプリング周期最小値演算								<V508>						*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*					Egear.b * Hprm.Kmotspd  * 1000000												*/
/*		SampLlim = -----------------------------------												*/
/*					Egear.a * KPI_SBCYCLEUS *  2^23													*/
/*																									*/
/****************************************************************************************************/
LONG	dtrcCalcSetSampLlim( void* Axis, LONG Axno )
{
	LONG	kx,sx,plsno;
	LONG	Egear_a,Egear_b;
	BOOL	EgearDisable;
	AXIS_HANDLE *AxHdl;

	AxHdl =  ((AXIS_HANDLE*)Axis + Axno);
	EgearDisable = (((AXIS_HANDLE*)Axis + Axno)->BaseCtrlData->TblDrive.OpeReg.TblFuncSel & TBLFUNCBIT_EGEAR);

	/* 現状フルク未対応	*/	
	//if( Kprm.f.FencUse )
	//{
	//	plsno = Hprm.KmotspdFC;
	//}
	//else
	//{
		plsno = AxHdl->prm_hndl_ptr->hprm_ptr->Kmotspd;
	//}

	if( EgearDisable == FALSE )
	{
		Egear_a = AxHdl->BaseCtrlData->PosManager.conf.Egear.a;
		Egear_b = AxHdl->BaseCtrlData->PosManager.conf.Egear.b;
		kx = MlibScalKskxkx( plsno, Egear_b, Egear_a, &sx,  0 );
	}
	else
	{ /* 電子ギア無効 */
		kx = MlibScalKskxkx( plsno, 1, 1, &sx,  0 );
	}
	kx = MlibPcalKxgain( kx, 1000000, 0x7FFFFF, &sx, 0 );
	kx = MlibPcalKxgain( kx, 1, KPI_SBCYCLEUS, &sx, 0 );
	kx = MlibPcalKxaddx( kx, sx, 4194304, 23, &sx );		// kx = kx + 0.5
	kx = MlibPcalKxgain( kx, 1, 1, &sx, -24 );
	return( kx );
}
#endif



void	DtrcSetNetworkTrigger( TRCHNDL *TrcHndl, BOOL Trigger )		/* <S012> */
{
	TrcHndl->TrcExeV.NetTrigger = Trigger;
	return;
}

/* <S0C9>	Start */
/****************************************************************************************************/
/*																									*/
/*		トレース対象データ選択、I/Oトレース対象データ選択の初期値設定								*/
/*																									*/
/****************************************************************************************************/
void	DtrcSelSetInitValue( TRCHNDL *TrcHndl )
{
	LONG	i;
	TRCREG	*TrcReg;
	TRCINDIVPRM	*TrcIncivPrm;

	TrcReg = &TrcHndl->TrcReg;
	TrcIncivPrm = &TrcHndl->TrcIncivPrm;


	/* データ､I/Oトレース1～3対象データ選択　初期値設定 */
	TrcReg->DataIOTrcSel[0] = 0x0101;
	TrcReg->DataIOTrcSel[1] = 0x0204;
	TrcReg->DataIOTrcSel[2] = 0x8105;

	/* トレース1～3対象データ選択　初期値設定 */
	TrcReg->TrcDataSel[0] = 0x0001;
	TrcReg->TrcDataSel[1] = 0x0002;
	TrcReg->TrcDataSel[2] = 0x0003;

	/* I/Oトレース1～3対象データ選択　初期値設定 */
	TrcReg->IOTrcDataSel[0] = 0x0001;
	TrcReg->IOTrcDataSel[1] = 0x0002;
	TrcReg->IOTrcDataSel[2] = 0x0081;


}
/* <S0C9>	End */

/***************************************** end of file **********************************************/
