/************************************************************************************************************/
/*																											*/
/*																											*/
/*		Interrupt.c : Mercury 割り込み関連処理																*/
/*																											*/
/*																											*/
/************************************************************************************************************/
/*																											*/
/*																											*/
/*		割り込み設定ソースファイル																			*/
/*																											*/
/*																											*/
/************** Copyright (C) Yaskawa Electric Corporation **************************************************/
/*																											*/
/*		Rev.1.00 : 2013.07.14  Y.Tanaka		初版															*/
/*																											*/
/************************************************************************************************************/
#include	"BaseDef.h"
#include	"ARMPF_VIC.h"
#include	"Interrupt.h"
#include	"Klib.h"
#include	"HwDeviceIf.h"

/****************************************************************************************************/
/*																									*/
/*		PROTOTYPE																					*/
/*																									*/
/****************************************************************************************************/
void 	InitInterrupt( void );							/* 割り込み関連初期化処理				*/
void 	InitInterruptStart( void );						/* 割り込みスタート						*/
void 	IntIgnore( void );									/* 無限ループ割り込み					*/
void 	Common_IRQHandler0( void );						/* ARMPF_VIC_ISC0用共通割り込みハンドラ	*/
void 	Common_IRQHandler1( void );						/* ARMPF_VIC_ISC1用共通割り込みハンドラ	*/
void 	ScanAIntHandler( void );							/* ScanA割り込み前後処理				*/
void 	ScanBIntHandler( void );							/* ScanB割り込み前後処理				*/
void 	ScanCIntHandler( void );							/* ScanC割り込み前後処理				*/
static	ULONG SearchIRQReqBit( VULONG value );			/* 割り込み要因検出処理					*/
BOOL	GetIntUDLStatus( UCHAR num );						/* INTUDL*割込み要求取得処理			*///<S0C7>
void 	ReqIntUDLStatusClear( UCHAR num );				/* INTUDL*割込み要求クリア処理			*///<S0C7>

/****************************************************************************************************/
/*																									*/
/*		外部関数extern宣言																			*/
/*																									*/
/****************************************************************************************************/
extern void SysScanAInt( void );
extern void SysScanBInt( void );
extern void SysScanCInt( void );
extern void __SET_VE( void );


/****************************************************************************************************/
/*																									*/
/*		VARIABLE																					*/
/*																									*/
/****************************************************************************************************/

#ifdef _MERCURY_PLATFORM_	/* 暗号化フォーマット対応 <S146> */
/****************************************************************************************************/
/*																									*/
/*		割り込み関連初期化処理																		*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		動作：VICのレジスタ設定、割り込み関数セット													*/
/*		引数：なし																					*/
/*		戻り値：なし																				*/
/*																									*/
/****************************************************************************************************/
void InitInterrupt(void)
{
	LONG i,j;
	ULONG PlsRegValue[2] = {0};
	ULONG EdgcRegValue[4] = {0};
	ULONG LvlRegValue[4] = {0};

//	__DI();

//	InitializeVIC();
	/* 割り込みイネーブルクリア/エッジ検出ビットクリア設定	*/
	for(i=0;i<2;i++)
	{
		ARMPF_REG_WRITE( (ARMPF_VIC_IEC0 + (i*4)), ARMPF_LONGALL1 );
		ARMPF_REG_WRITE( (ARMPF_VIC_PIC0 + (i*4)), ARMPF_LONGALL1 );
	}
	/* 割り込みをすべてIRQに設定	*/
	ARMPF_REG_WRITE( ARMPF_VIC_ISL0, ARMPF_LONGALL0 );
	ARMPF_REG_WRITE( ARMPF_VIC_ISL1, ARMPF_LONGALL0 );

	/* テーブルデータを読み込んでVICに設定	*/
	/* 割り込み検出方法設定	*/
	for(i=0;i<2;i++)
	{
		for(j=0;j<32;j++)
		{
			PlsRegValue[i] |= (InitIntVecTbl[ j+i*32 ].PLS << j);
		}
		ARMPF_REG_WRITE( (ARMPF_VIC_PLS0 + (i*4)) , PlsRegValue[i] );		/* レベル/エッジ設定	*/
	}
}
#else	/* _MERCURY_PLATFORM_ */	/* 暗号化フォーマット対応 <S146> */

#pragma arm section code = "INIT_VECT_CODE"
/****************************************************************************************************/
/*																									*/
/*		割り込み関連初期化処理																		*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		動作：VICのレジスタ設定、割り込み関数セット													*/
/*		引数：なし																					*/
/*		戻り値：なし																				*/
/*																									*/
/****************************************************************************************************/
void InitInterrupt(void)
{
	LONG i,j;
	ULONG PlsRegValue[2] = {0};
	ULONG EdgcRegValue[4] = {0};
	ULONG LvlRegValue[4] = {0};

//	__DI();

//	InitializeVIC();
	/* 割り込みイネーブルクリア/エッジ検出ビットクリア設定	*/
	for(i=0;i<2;i++)
	{
		ARMPF_REG_WRITE( (ARMPF_VIC_IEC0 + (i*4)), ARMPF_LONGALL1 );
		ARMPF_REG_WRITE( (ARMPF_VIC_PIC0 + (i*4)), ARMPF_LONGALL1 );
	}
	/* 割り込みをすべてIRQに設定	*/
	ARMPF_REG_WRITE( ARMPF_VIC_ISL0, ARMPF_LONGALL0 );
	ARMPF_REG_WRITE( ARMPF_VIC_ISL1, ARMPF_LONGALL0 );

	/* テーブルデータを読み込んでVICに設定	*/
	/* 割り込み検出方法設定	*/
	for(i=0;i<2;i++)
	{
		for(j=0;j<32;j++)
		{
			PlsRegValue[i] |= (InitIntVecTbl[ j+i*32 ].PLS << j);
		}
		ARMPF_REG_WRITE( (ARMPF_VIC_PLS0 + (i*4)) , PlsRegValue[i] );		/* レベル/エッジ設定	*/
	}

	for(i=0;i<4;i++)
	{
		for(j=0;j<16;j++)
		{
			if(InitIntVecTbl[j+i*16].PLS == ARMPF_VIC_EDGE)
			{/*エッジ検出の場合*/
				EdgcRegValue[i] |= (InitIntVecTbl[ j+i*16 ].EDGC << (j*2));
			}
			else if(InitIntVecTbl[j+i*16].PLS == ARMPF_VIC_LVL)
			{/*レベル検出の場合*/
				LvlRegValue[i] |= (InitIntVecTbl[ j+i*16 ].EDGC << (j*2));
			}
		}
		ARMPF_REG_WRITE( (ARMPF_VIC_EDGC0 + (i*4)) , EdgcRegValue[i] );		/* エッジ検出方法設定	*/
		ARMPF_REG_WRITE( (ARMPF_VIC_LVLC0 + (i*4)) , LvlRegValue[i] );		/* レベル検出方法設定	*/
	}

	/* 割り込み優先レベルマスクすべて解除	*/
	ARMPF_REG_WRITE( ARMPF_VIC_PRLC, ARMPF_WORDALL1 );

	/* 割り込み優先度/割り込みハンドラ設定	*/
	for(i=0;i<64;i++)
	{
		ARMPF_REG_WRITE( (ARMPF_VIC_PRL0 + (i*4)), IntVecTbl[i].Priority );	/* 割り込み優先度設定	*/
		ARMPF_REG_WRITE( (ARMPF_VIC_VAD0 + (i*4)), (ULONG)(InitIntVecTbl[i].RegHandler) );	/* 割り込みハンドラ設定	*/
	}

	/* 割り込みの許可/マスクを設定	*/
//	ARMPF_REG_WRITE( ARMPF_VIC_IEN0, 0x00000007 );	/* UDL割り込みのみ有効 for debug */
//	ARMPF_REG_WRITE( ARMPF_VIC_IEN0, 0x003C0007 );	/* UDL+IntervalTimer割り込みのみ有効 for debug */
//	ARMPF_REG_WRITE( ARMPF_VIC_IEN0, 0x003C0000 );	/* IntervalTimer割り込みのみ有効 for debug */
//	ARMPF_REG_WRITE( ARMPF_VIC_IEN1, 0x0FDE00C0 );
	ARMPF_REG_WRITE( ARMPF_VIC_IEN0, ARMPF_LONGALL0 );
	ARMPF_REG_WRITE( ARMPF_VIC_IEN1, ARMPF_LONGALL0 );

	__SET_VE();		/* VIC有効			*/
}


/****************************************************************************************************/
/*																									*/
/*		割り込み開始処理																			*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		動作：割り込み要因クリア後、割り込み許可													*/
/*		引数：なし																					*/
/*		戻り値：なし																				*/
/*																									*/
/****************************************************************************************************/
void InitInterruptStart(void)
{


	/* 割り込みエッジ検出ビットクリア設定	*/
	ARMPF_REG_WRITE( ARMPF_VIC_PIC0, ARMPF_LONGALL1 );
	ARMPF_REG_WRITE( ARMPF_VIC_PIC1, ARMPF_LONGALL1 );		/*<S008>*/

	/* CP15 割込み有効	*/
	__EI();



	/* 割り込みエッジ検出ビットクリア設定	*/
	ARMPF_REG_WRITE( ARMPF_VIC_PIC0, ARMPF_LONGALL1 );
	ARMPF_REG_WRITE( ARMPF_VIC_PIC1, ARMPF_LONGALL1 );		/*<S008>*/

	/* 割り込みの許可/マスクを設定	*//* VIC_IEN0を有効にするとすぐにScanが動き出すのでVIC_IEN1を先にマスク解除する <S008>*/
#if 0	/* <S150> USB割り込みを使用しない(ポーリング化) */
	ARMPF_REG_WRITE( ARMPF_VIC_IEN1, 0x000000C0 );				/* USB割込みのみ有効*/
#endif
	ARMPF_REG_WRITE( ARMPF_VIC_IEN0, 0x00000007 | (1<<29) );	/* UDL割り込みとI2C割り込みのみ有効 for debug */

}

/****************************************************************************************************/
/*																									*/
/*		無限ループ																					*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		動作：規定していない割り込みで実行															*/
/*		引数：なし																					*/
/*		戻り値：なし																				*/
/*																									*/
/****************************************************************************************************/
void IntIgnore(void)
{
	while(1);
}


#pragma arm section code = "VECT_CODE"
/****************************************************************************************************/
/*																									*/
/*		ARMPF_VIC_ISC0用共通割り込みハンドラ														*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		動作：ARMPF_VIC_ISC0に登録されている割り込みの要因検索とハンドラ呼び出し					*/
/*		引数：なし																					*/
/*		戻り値：なし																				*/
/*																									*/
/****************************************************************************************************/
void Common_IRQHandler0( void )
{
	ULONG	IRQNumber;
	ULONG	priority;
	ULONG	mask;

//IRQHandlerで割り込み禁止にしているので不要
//	/* __DI */

	/* 割り込み要因ビットが立っているところを探してIRQNumberにセットする */
	IRQNumber = SearchIRQReqBit(ARMPF_REG_READ(ARMPF_VIC_ISC0));	/* ARMPF_VIC_ISC0のビットが立っているところを検索 */
	if( IRQNumber < 32 )		/* ARMPF_VIC_ISC0に割り込み要因が見つかった場合 */
	{
		/* 割り込み要因をクリア */
		ARMPF_REG_WRITE( ARMPF_VIC_PIC0, (0x1 << IRQNumber) );

		/* 同じ優先度以下の割り込みをマスク */
		priority = (0x1 << IntVecTbl[IRQNumber].Priority);
		mask = ( priority ^ (-priority) );	/* 同優先度以下のビットを立てる		*/
		ARMPF_REG_WRITE( ARMPF_VIC_PRLM, mask );

		__EI();
		IntVecTbl[IRQNumber].IntHandler();		/* 割り込みハンドラコール		*/
		__DI();

		/* 同じ優先度以下の割り込みをマスククリア */
		ARMPF_REG_WRITE( ARMPF_VIC_PRLC, mask );
	}

	/* 割り込み処理終了を通知(HVAにダミーライト) */
	ARMPF_REG_WRITE( ARMPF_VIC_HVA, IRQNumber );
	__EI();

}


/****************************************************************************************************/
/*																									*/
/*		ARMPF_VIC_ISC1用共通割り込みハンドラ														*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		動作：ARMPF_VIC_ISC1に登録されている割り込みの要因検索とハンドラ呼び出し					*/
/*		引数：なし																					*/
/*		戻り値：なし																				*/
/*																									*/
/****************************************************************************************************/
void Common_IRQHandler1( void )
{
	ULONG	IRQNumber;
	ULONG	priority;
	ULONG	mask;

//IRQHandlerで割り込み禁止にしているので不要
//	/* __DI */

	/* 割り込み要因ビットが立っているところを探してIRQNumberにセットする */
	IRQNumber = SearchIRQReqBit(ARMPF_REG_READ(ARMPF_VIC_ISC1));	/* ARMPF_VIC_ISC1のビットが立っているところを検索 */
	if( IRQNumber < 32 )		/* ARMPF_VIC_ISC1に割り込み要因が見つかった場合 */
	{
		/* 割り込み要因をクリア */
		ARMPF_REG_WRITE( ARMPF_VIC_PIC1, (0x1 << IRQNumber) );

		/* 同じ優先度以下の割り込みをマスク */
		priority = (0x1 << IntVecTbl[IRQNumber+32].Priority);		/*<S00B>*/
		mask = ( priority ^ (-priority) );	/* 同優先度以下のビットを立てる		*/
		ARMPF_REG_WRITE( ARMPF_VIC_PRLM, mask );

		__EI();
		IntVecTbl[IRQNumber+32].IntHandler();		/* 割り込みハンドラコール		*/		/*<S008>*/
		__DI();

		/* 同じ優先度以下の割り込みをマスククリア */
		ARMPF_REG_WRITE( ARMPF_VIC_PRLC, mask );
	}

	/* 割り込み処理終了を通知(HVAにダミーライト) */
	ARMPF_REG_WRITE( ARMPF_VIC_HVA, IRQNumber );
	__EI();

}


/****************************************************************************************************/
/*																									*/
/*		ScanA割り込み前後処理																		*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		動作：ScanA割り込み要因のクリア/マスクなどの前後処理										*/
/*		引数：なし																					*/
/*		戻り値：なし																				*/
/*																									*/
/****************************************************************************************************/
void ScanAIntHandler( void )
{
//	ULONG	priority;
//	ULONG	mask;

	HALdrv_OutputDebugSP( INTERRUPT_ScanA );		/* 割込みの分岐処理でポート出力に変更<S042> */

//IRQHandlerで割り込み禁止にしているので不要
//	/* __DI */

	/* 割り込み要因をクリア */
	ARMPF_REG_WRITE( ARMPF_VIC_PIC0, 0x01 );

	/* 同じ優先度以下の割り込みをマスク *//* 削除,  <S0E2> */
//	priority = (0x1 << IntVecTbl[0].Priority);
//	mask = ( priority ^ (-priority) );	/* 同優先度以下のビットを立てる		*/
//	ARMPF_REG_WRITE( ARMPF_VIC_PRLM, mask );

//	__EI(); /* <S01E> */
	SysScanAInt();		/* 割り込みハンドラコール		*/
//	__DI(); /* <S01E> */

	/* 同じ優先度以下の割り込みをマスククリア *//* <S0E2> */
//	ARMPF_REG_WRITE( ARMPF_VIC_PRLC, mask );

	/* 割り込み処理終了を通知(HVAにダミーライト) */
	ARMPF_REG_WRITE( ARMPF_VIC_HVA, 0 );
//	__EI(); /* <S01E> */

	HALdrv_ClearDebugSP( INTERRUPT_ScanA );		/* 割込みの分岐処理でポート出力に変更<S042> */

}


/****************************************************************************************************/
/*																									*/
/*		ScanB割り込み前後処理																		*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		動作：ScanB割り込み要因のクリア/マスクなどの前後処理										*/
/*		引数：なし																					*/
/*		戻り値：なし																				*/
/*																									*/
/****************************************************************************************************/
void ScanBIntHandler( void )
{
	ULONG	priority;
	ULONG	mask;

	HALdrv_OutputDebugSP( INTERRUPT_ScanB );		/* 割込みの分岐処理でポート出力に変更<S042> */

//IRQHandlerで割り込み禁止にしているので不要
//	/* __DI */

	/* 割り込み要因をクリア */
	ARMPF_REG_WRITE( ARMPF_VIC_PIC0, 0x02 );

	/* 同じ優先度以下の割り込みをマスク */
	priority = (0x1 << IntVecTbl[1].Priority);
	mask = ( priority ^ (-priority) );	/* 同優先度以下のビットを立てる		*/
	ARMPF_REG_WRITE( ARMPF_VIC_PRLM, mask );

	__EI();
	SysScanBInt();		/* 割り込みハンドラコール		*/
	__DI();

	/* 同じ優先度以下の割り込みをマスククリア */
	ARMPF_REG_WRITE( ARMPF_VIC_PRLC, mask );

	/* 割り込み処理終了を通知(HVAにダミーライト) */
	ARMPF_REG_WRITE( ARMPF_VIC_HVA, 1 );
//	__EI(); /* <S01E> */

	HALdrv_ClearDebugSP( INTERRUPT_ScanB );		/* 割込みの分岐処理でポート出力に変更<S042> */

}


/****************************************************************************************************/
/*																									*/
/*		ScanC割り込み前後処理																		*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		動作：ScanC割り込み要因のクリア/マスクなどの前後処理										*/
/*		引数：なし																					*/
/*		戻り値：なし																				*/
/*																									*/
/****************************************************************************************************/
void ScanCIntHandler( void )
{
	ULONG	priority;
	ULONG	mask;

#if CSW_NETIF_TYPE	!= NETIF_ANLGPLS_TYPE
	HALdrv_OutputDebugSP( INTERRUPT_ScanC );		/* 割込みの分岐処理でポート出力に変更<S042> */
#endif	//#if CSW_NETIF_TYPE	!= NETIF_ANLGPLS_TYPE

//IRQHandlerで割り込み禁止にしているので不要
//	/* __DI */

	/* 割り込み要因をクリア */
	ARMPF_REG_WRITE( ARMPF_VIC_PIC0, 0x04 );

	/* 同じ優先度以下の割り込みをマスク */
	priority = (0x1 << IntVecTbl[2].Priority);
	mask = ( priority ^ (-priority) );	/* 同優先度以下のビットを立てる		*/
	ARMPF_REG_WRITE( ARMPF_VIC_PRLM, mask );

	__EI();
	SysScanCInt();		/* 割り込みハンドラコール		*/
	__DI();

	/* 同じ優先度以下の割り込みをマスククリア */
	ARMPF_REG_WRITE( ARMPF_VIC_PRLC, mask );

	/* 割り込み処理終了を通知(HVAにダミーライト) */
	ARMPF_REG_WRITE( ARMPF_VIC_HVA, 2 );
//	__EI(); /* <S01E> */

#if CSW_NETIF_TYPE	!= NETIF_ANLGPLS_TYPE
	HALdrv_ClearDebugSP( INTERRUPT_ScanC );		/* 割込みの分岐処理でポート出力に変更<S042> */
#endif	//#if CSW_NETIF_TYPE	!= NETIF_ANLGPLS_TYPE

}


/****************************************************************************************************/
/*																									*/
/*		割り込み要因検出処理																		*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		動作：ARMPF_VIC_ISCx(x=0,1)から割り込み要因が立っているものを探す							*/
/*		引数：ARMPF_VIC_ISCxの値																	*/
/*		戻り値：ARMPF_VIC_ISCxでビットが立っているところ(0～31)										*/
/*				何も立っていない場合は32															*/
/*																									*/
/****************************************************************************************************/
static	ULONG SearchIRQReqBit(VULONG value)
{
	ULONG	rc;
	
	rc = (value & (-value)) - 1;
    rc = (rc & 0x55555555) + (rc >> 1 & 0x55555555);
    rc = (rc & 0x33333333) + (rc >> 2 & 0x33333333);
    rc = (rc & 0x0f0f0f0f) + (rc >> 4 & 0x0f0f0f0f);
    rc = (rc & 0x00ff00ff) + (rc >> 8 & 0x00ff00ff);
    return (rc & 0x0000ffff) + (rc >>16 & 0x0000ffff);
}

/****************************************************************************************************/
/*																									*/
/*	INTUDL*割込み要求取得処理																		*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		動作：割り込み入力ステータス・レジスタからUDLの割込み状態を取得する							*/
/*		引数：なし																					*/
/*		戻り値：TRUE or FALSE																		*/
/*																									*/
/****************************************************************************************************/
BOOL GetIntUDLStatus(UCHAR num)
{
	BOOL	rc;
	VULONG	sts;
	
	sts = ARMPF_REG_READ(ARMPF_VIC_RAIS0);
	if((sts >> num) & 0x00000001)
	{/* SyncStatus	*/
		rc = TRUE;
	}
	else
	{/* UnSyncStatus	*/
		rc = FALSE;
	}
	return rc;
}

/****************************************************************************************************/
/*																									*/
/*	INTUDL*割込み要求クリア処理																		*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		動作：UDLの割込みをクリアする																*/
/*		引数：なし																					*/
/*		戻り値：TRUE or FALSE																		*/
/*																									*/
/****************************************************************************************************/
void ReqIntUDLStatusClear(UCHAR num)
{
	VULONG	sts;
	
	sts = ARMPF_REG_READ(ARMPF_VIC_PIC0);
	sts = (sts | (0x01 << num));
	ARMPF_REG_WRITE(ARMPF_VIC_PIC0, sts);
	
	return;
}
#endif /* _MERCURY_PLATFORM_ */	/* 暗号化フォーマット対応 <S146> */
/*-------end of file----------------------------------------------------------------------------------------*/
