#ifndef	GSEL_HANDLER_H
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		GselHandler.h : ゲイン切り替え用ハンドル定義												*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	初版	2010.02.23	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/

/****************************************************************************************************/
/*		STRUCT DEFINITION																			*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		サーボゲイン切替用パラメータ変数定義														*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct GSELGAINS {
	KSGAIN		Kp;									/* 位置ループゲイン								*/
	KSGAIN		Kv;									/* 速度ループゲイン								*/
	KSGAIN		Kv2;								/* 速度ループゲイン２							*/
	KSGAIN		Kv2Inv;								/* 速度ループゲイン２逆数						*/
	KSGAIN		Kvi;								/* 速度ループ積分ゲイン							*/
	KSGAIN		Klpf;								/* １次遅れトルクフィルタゲイン					*/
	KSGAIN		KpInv;								/* 位置ループゲイン逆数 SvCycle単位(SGDV未使用)	*/
	KSGAIN		VirtualPosKp;						/* 仮想位置偏差計算用ゲイン						*/
	KSGAIN		ZcKp;								/* ゼロクランプ用位置ループゲイン				*/
} GSELGAINS;


#if 0 /* 2010.11.10 Y.Oka 未使用のためコメントアウト */
typedef	struct MICROGAINS {
	long		Kp;									/* 位置ループゲイン								*/
	long		Kv;									/* 速度ループゲイン								*/
	long		Kvi;								/* 速度ループ積分ゲイン							*/
	long		Klpf;								/* １次遅れトルクフィルタゲイン					*/
} MICROGAINS;
#endif /* #if 0 */


#if 0		/* <S09B> */
typedef	struct GSELDOBS {
	KSGAIN		Ks;									/* 外乱オブザーバKsゲイン						*/
	KSGAIN		Ksj;								/* 外乱オブザーバKsjゲイン						*/
} GSELDOBS;
#else 		/* <S09B> */
typedef	struct GSELDOBS {					/* 外乱オブザーバ用パラメータ定義						*/
	KSGAIN				Ks;						/* 外乱オブザーバゲイン								*/
	KSGAIN				Kj;						/* 外乱オブザーバイナーシャゲイン					*/
	KSGAIN				Ksj;					/* 外乱オブザーバトルクゲイン						*/
	KSGAIN				Kd;						/* 外乱トルク係数									*/
	KSGAIN				Klpf;					/* 外乱オブザーバローパスフィルタゲイン				*/
} GSELDOBS;
#endif

#if 0	// 不要のため削除
typedef struct GSELSIG {						/* 自動ゲイン切替条件信号							*/
	BOOL		CoinOn;								/* COIN Onで切替								*/
	BOOL		CoinOff;							/* COIN Offで切替								*/
	BOOL		NearOn;								/* NEAR Onで切替								*/
	BOOL		NearOff;							/* NEAR Offで切替								*/
	BOOL		RefZOn;								/* REFZ Onで切替								*/
	BOOL		RefZOff;							/* REFZ Offで切替								*/
} GSELSIG;
#endif

#if 0	/* <S09B> */
typedef	struct ATGSEL {							/* 自動ゲイン切替用パラメータ＆変数定義				*/
		struct	{								/* 自動ゲイン切替用パラメータ						*/
		LONG		Wait;							/* ゲイン切替待ち時間							*/
/* 2010.11.10 Y.Oka 自動ゲイン切り替え時のスムージング未対応のためコメントアウト　*/
//		LONG		AmondGain;						/* アナログモニタ出力用 ⊿ゲイン				*/
//		GSELGAINS	dGain;							/* ⊿ゲイン										*/
	} conf;
	/*----------------------------------------------------------------------------------------------*/
	struct	{									/* 自動ゲイン切替用変数								*/
		LONG		Timer;							/* ゲイン切替待ち時間用タイマ					*/
/* 2010.11.10 Y.Oka 自動ゲイン切り替え時のスムージング未対応のためコメントアウト　*/
//		GSELGAINS	*prevGain;						/* 前回ゲイン									*/
//		GSELGAINS	*nextGain;						/* 次回ゲイン									*/
	} var;
} ATGSEL;
#endif


#if 0 /* 2010.11.10 Y.Oka 自動ゲイン切り替え時のスムージング未対応のためコメントアウト　*/
typedef	struct ATGSELDOBS {						/* 自動ゲイン切替用パラメータ&変数定義(摩擦補償専用)*/
		struct	{								/* 自動ゲイン切替用パラメータ						*/
		GSELDOBS	dGain;							/* ⊿ゲイン										*/
	} conf;
	/*----------------------------------------------------------------------------------------------*/
	struct	{									/* 自動ゲイン切替用変数								*/
		GSELDOBS	*prevGain;						/* 前回ゲイン									*/
		GSELDOBS	*nextGain;						/* 次回ゲイン									*/
	} var;
} ATGSELDOBS;
#endif /* #if 0 */

/*--------------------------------------------------------------------------------------------------*/
/*		<S09B> 自動ゲイン切替用パラメータ変数定義													*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct ATGSEL {							/* 自動ゲイン切替用パラメータ＆変数定義				*/
		struct	{								/* 自動ゲイン切替用パラメータ						*/
		LONG		Wait;							/* ゲイン切替待ち時間							*/
		LONG		AmondGain;						/* アナログモニタ出力用 ⊿ゲイン				*/
		LONG		SwTime;							/* ゲイン切替時間[ms]							*/
		LONG		SvCycleUs;						/* 制御周期[us]									*/
		LONG		GselTrqFilMin;					/* @@											*/
		GSELGAINS	dGain;							/* ⊿ゲイン										*/
		GSELDOBS	dGainDobs;						/* ⊿ゲイン(外乱オブザーバ)						*/
	} conf;
	/*----------------------------------------------------------------------------------------------*/
	struct	{									/* 自動ゲイン切替用変数								*/
		LONG		Timer;							/* ゲイン切替待ち時間用タイマ					*/
		GSELGAINS	*prevGain;						/* 前回ゲイン									*/
		GSELGAINS	*nextGain;						/* 次回ゲイン									*/
		GSELDOBS	*prevGainDobs;					/* 前回ゲイン(外乱オブザーバ)					*/
		GSELDOBS	*nextGainDobs;					/* 次回ゲイン(外乱オブザーバ)					*/
	} var;
} ATGSEL;

typedef	struct GAIN_CHNG_HNDL {
//	GSELSIG			GselSig;			/* 自動ゲイン切替条件信号									*/
	ATGSEL			AtGselA;			/* 自動ゲイン切替用変数(TrgA)								*/
	ATGSEL			AtGselB;			/* 自動ゲイン切替用変数(TrgB)								*/
	GSELGAINS		GselGains[6];		/* サーボゲイン切替用パラメータ変数							*/
	GSELDOBS		GselDobs[3];		/* ゲイン切り替え用パラメータ変数(摩擦補償専用)				*/
	BOOL			AtGsel;				/* 自動ゲイン切替選択設定									*/
	LONG			AtGselTrgA;			/* 自動ゲイン切替条件Ａ選択設定								*/
	LONG			AmonActGain;		/* AnaMon : 有効ゲイン										*/
	LONG			ActGain;			/* Un014  : 有効ゲイン										*/
} GAIN_CHNG_HNDL;



#define	GSEL_HANDLER_H
#endif /* GSEL_HANDLER_H */
/***************************************** end of file **********************************************/
