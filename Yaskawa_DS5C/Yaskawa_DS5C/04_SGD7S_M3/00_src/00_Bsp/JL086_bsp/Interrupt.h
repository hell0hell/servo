/************************************************************************************************************/
/*																											*/
/*																											*/
/*		Interrupt.h : Mercury 割り込み関連処理																*/
/*																											*/
/*																											*/
/************************************************************************************************************/
/*																											*/
/*																											*/
/*		割り込み設定ヘッダファイル																			*/
/*																											*/
/*																											*/
/************** Copyright (C) Yaskawa Electric Corporation **************************************************/
/*																											*/
/*		Rev.1.00 : 2013.07.14  Y.Tanaka		初版															*/
/*		Rev.1.01 : 2013.07.30  Y.Tanaka		コード整理														*/
/*																											*/
/************************************************************************************************************/


/************************************************************************************************************/
/*																											*/
/*		PROTOTYPE																							*/
/*																											*/
/************************************************************************************************************/
extern	void	InitInterrupt( void );							/* 割り込み関連初期化処理					*/
extern	void	IntIgnore( void );								/* 無限ループ割り込み						*/
extern	void	Common_IRQHandler0( void );						/* ARMPF_VIC_ISC0用共通割り込みハンドラ		*/
extern	void	Common_IRQHandler1( void );						/* ARMPF_VIC_ISC1用共通割り込みハンドラ		*/
extern	void	ScanAIntHandler( void );						/* ScanA割り込み前後処理					*/
extern	void	ScanBIntHandler( void );						/* ScanB割り込み前後処理					*/
extern	void	ScanCIntHandler( void );						/* ScanC割り込み前後処理					*/
extern	BOOL	GetIntUDLStatus(UCHAR num);						/* INTUDL*割込み要求取得処理				*///<S0C7>
extern	void 	ReqIntUDLStatusClear( UCHAR num );				/* INTUDL*割込み要求クリア処理				*///<S0C7>


/************************************************************************************************************/
/*		test																									*/
/*		外部関数extern宣言																					*/
/*																											*/
/************************************************************************************************************/
extern void __EI( void );											/* 割り込み許可設定						*/
extern void __DI( void );											/* 割り込み禁止設定						*/


/************************************************************************************************************/
/*																											*/
/*		マクロ定義																							*/
/*																											*/
/************************************************************************************************************/
#define	SetEnableInterrupt()	__EI()								/* 割り込み許可設定(__EI)				*/
#define	SetDisableInterrupt()	__DI()								/* 割り込み禁止設定(__DI)				*/


/************************************************************************************************************/
/*																											*/
/*		構造体定義																							*/
/*																											*/
/************************************************************************************************************/
typedef struct{
//	UCHAR	ISL;					/* 割り込み実行部 0/1 - IRQ/FIQ											*///全要因一括"IRQ"設定する
//	UCHAR	IEN;					/* 割り込み要求許可 0/1 - マスク/許可									*///全要因一括"許可"設定する
	UCHAR	PLS;					/* 割り込み検出タイプ設定 0/1 - レベル/エッジ							*/
	UCHAR	EDGC;					/* 割り込み有効レベル/エッジ指定										*/
									/* エッジ検出時：0/1/2/3 - 検出なし/立上り/立下り/両エッジ				*/
									/* レベル検出時：0/1/2/3 - 検出なし/ハイ/ロー/検出なし					*/
	void (*RegHandler)(void);		/* 登録用ハンドラ					*/
}INTVECINITTBL;

typedef struct{
	ULONG	Priority;				/* 割り込み有効レベル/エッジ指定										*/
	void (*IntHandler)(void);		/* 割り込みハンドラ					*/
}INTVECTBL;


/************************************************************************************************************/
/*																											*/
/*		変数宣言																							*/
/*																											*/
/************************************************************************************************************/
extern const INTVECINITTBL InitIntVecTbl[64];
extern const INTVECTBL IntVecTbl[64];

enum INTUDL_NUM
{
INTUDL0 = 0,	/* 0: ScanA		*/
INTUDL1,        /* 1: ScanB		*/
INTUDL2,        /* 2: ScanC		*/
INTUDL3,        /* 3: ScanN		*/
INTUDL4,        /* 4: INT1		*/
INTUDL5,        /* 5: INT2		*/
INTUDL6,        /* 6: M3_INT0	*/
INTUDL7,        /* 7: M3_INT1	*/
};

#if (CSW_NETIF_TYPE == NETIF_M2_TYPE)
#define	INTUDL_ML INTUDL4
#else
#define	INTUDL_ML INTUDL7
#endif
/*-------end of file----------------------------------------------------------------------------------------*/
