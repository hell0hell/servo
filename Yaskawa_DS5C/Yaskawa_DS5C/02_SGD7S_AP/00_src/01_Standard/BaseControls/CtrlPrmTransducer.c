/****************************************************************************************************/
/*																									*/
/*																									*/
/*		制御用パラメータ変数一括変換処理定義														*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*			1. 調整レス、外乱オブザーバ等のパラメータ変数一括変換処理								*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	初版	2013.08.16	Y.Oka	For Mercury												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "CtrlPrmTransducer.h"
#include "Mlib.h"
#include "Klib.h"



/****************************************************************************************************/
/*																									*/
/*		制御用パラメータ変数一括変換サービス初期化処理												*/
/*																									*/
/****************************************************************************************************/
void	TranlateCtrlPrmInitialize( TRANSDUCE_CTRL *TransDuceCtrl )
{
	/* 変換サービス用変数初期化 */
	MlibResetLongMemory( TransDuceCtrl, sizeof(*TransDuceCtrl)>>2 );
	/* Initial時はDIRECTモード */
	TransDuceCtrl->TranslateMode = TMODE_DIRECT;
}



/****************************************************************************************************/
/*																									*/
/*		制御用パラメータ変数一括変換要求処理														*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		概要：	調整レスや外乱オブザーバ等のパラメータを変更した場合、複数の関連する変数を同時に	*/
/*				切り替える必要がある。SGDVでは割り込み禁止により実現していたが、SGD7シリーズでは	*/
/*				本関数を用いて一括変換要求を行い、ScanAもしくはScanB処理にて実際に変換を実行する	*/
/*				ことで割禁処理のリスクを回避する。													*/
/*				なお、Initial中で割り込みが起動されない場合は、本処理にて直接変換を実施する。		*/
/*																									*/
/*		注意：	サイズの最大は32個とし、LONGサイズ(32bit)での転送を行う。本関数を使用して変数値を	*/
/*				更新するためには、転送元、転送先がそれぞれ4byteでアライメントされている必要がある。	*/
/*				また、2ms以上経過しても転送が完了しない場合はタイムアウトとなる。					*/
/*																									*/
/****************************************************************************************************/
LONG	TranlateCtrlPrmRequest( TRANSDUCE_CTRL *TransDuceCtrl, void *Data, ULONG *Address,
																		UCHAR Size, CHAR Timing )
{
	LONG			i;
	LONG			ErrSts;
	ULONG			TimeOut;

	ErrSts = TR_SUCCESS;

	/* Check Interrupt Starting */
	if( TransDuceCtrl->TranslateMode == TMODE_DIRECT )
	{ /* Interrupt Stopped State */
		for( i=0; i<Size; i++ )
		{
			( (ULONG*)Address )[i] = ( (ULONG*)Data )[i];
		}
	}
	else
	{ /* Interrupt Active State */
		/* Check Request Data Size */
		if( Size > TR_DATA_MAX )
		{
			ErrSts = TR_DATA_ERROR;
		}

		/* Check Src Data Address and Dst Data Address */
		if( (Data == NULL) || (Address == NULL) )
		{
			ErrSts = TR_DATA_ERROR;
		}

		if( ErrSts != TR_DATA_ERROR )
		{
			/* Set Target Address */
			TransDuceCtrl->TargetAddress = Address;

			/* Set Parameter Data */
			for( i=0; i<Size; i++ )
			{
				TransDuceCtrl->DataArray[i] = ( (ULONG*)Data )[i];
			}

			/* Set Prameter Number (4byte unit) */
			TransDuceCtrl->TranslateSize = Size;

			/* Set Translate Request and Timing(ScanA or ScanB) */
			TransDuceCtrl->TranslateSts = Timing;

			/* Check and Wait Finish Last Translation */
			TimeOut = 0;
			while( TransDuceCtrl->TranslateSts != TR_READY )
			{
				/* Wait for Ready */
				KlibWaitus( KPI_SACYCLEUS );
				TimeOut++;
				if( TimeOut > (KPI_SCCYCLEMS * KPI_SACOUNTMS) )
				{
					ErrSts = TR_DATA_ERROR;
					TransDuceCtrl->TranslateSts = TR_READY;											  /* <S066> */
					break;																			  /* <S002> */
				}
			}
		}
	}
	return	ErrSts;
}



#pragma arm section code = "CALL_SCANA" /* <S1D2> */
/****************************************************************************************************/
/*																									*/
/*		制御用パラメータ変数一括変換処理															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		概要：	一括変換要求を確認し、要求がある場合には対象データを対象アドレスへコピーする。		*/
/*																									*/
/*		注意：	ScanAもしくはScanBにて要求を確認し、自Scanでの変換要求があった場合に本処理をコール	*/
/*				する。割り込み処理先頭、少なくとも変換対象変数を使用する処理の実行前に変換を行う	*/
/*				必要がある。																		*/
/*																									*/
/****************************************************************************************************/
void	TranlateCtrlPrm( TRANSDUCE_CTRL *TransDuceCtrl )
{
	LONG	i;

	/* 変換実行 */
	for( i=0; i<TransDuceCtrl->TranslateSize; i++ )
	{
		TransDuceCtrl->TargetAddress[i] = TransDuceCtrl->DataArray[i];
	}

	/* 変換完了 */
	TransDuceCtrl->TranslateSts = TR_READY;

	return;
}
#pragma arm section code /* <S1D2> */


/***************************************** end of file **********************************************/
