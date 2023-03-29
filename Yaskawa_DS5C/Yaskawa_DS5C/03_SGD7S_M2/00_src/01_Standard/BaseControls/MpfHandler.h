#ifndef MPF_HANDLER_H
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		MpfHandler.h : 磁極検出用ハンドル定義														*/
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
/*		磁極検出実行用状態管理変数定義																*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct MPFEXEINFO {				/* 構造体定義												*/
	BOOL	HaltCondition;					/* Over Travel or not Servo Ready State					*/
	BOOL	MotStopSts;						/* モータ停止状態										*/
	LONG	dMotPosition;					/* 位置FB増分値						[pulse/ScanB]		*/
	LONG	TrefMonitor;					/* トルク指令出力(平均値)			[2^24/MaxTrq]		*/
	LONG	SpdFbMonitor;					/* 速度FB入力						[2^24/OvrSpd]		*/
	KSGAIN	KVfbFil;						/* 速度FBフィルタゲイン									*/
	KSGAIN	Kphasepos;						/* Gain for Motor Phase Position Cal.					*/
	KSGAIN	Kmovpos;						/* Gain for Linear Motor Move Position Cal.				*/
} MPFEXEINFO;



/*--------------------------------------------------------------------------------------------------*/
/*		磁極検出用変数定義																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct MPFIND {						/* 構造体定義											*/
	struct	{								/* 磁極検出用定数データ定義								*/
		KSGAIN	Kv;								/* 速度ループゲイン				[0.1Hz]				*/
		KSGAIN	Ki;								/* 速度ループ積分ゲイン			[0.01ms]			*/
		KSGAIN	InteglessGain;					/* 積分レスゲイン				[%]					*/
		KSGAIN	Klpf;							/* 1次遅れフィルタゲイン							*/
		LONG	MaxSpd;							/* 速度指令ピーク値				[2^24/OvrSpd]		*/
		LONG	dSpd;							/* 速度指令差分値				[2^24/OvrSpd/Cycle]	*/
		LONG	ReverseSpd;						/* 逆走レベル(速度指令時)		[2^24/OvrSpd]		*/
		LONG	ReverseSpd2;					/* 逆走レベル(待ち時間時)		[2^24/OvrSpd]		*/
		LONG	OverSpd;						/* 速度オーバー検出値			[2^24/OvrSpd]		*/
		LONG	DetectRange;					/* 可動範囲						[0.01mm]			*/
		LONG	MaxTrq;							/* 推力指令ピーク値				[2^24/MaxTrq]		*/
		LONG	dTrq;							/* 推力指令差分値				[2^24/MaxTrq/Cycle]	*/
		LONG	ErrOKLevel;						/* 検出誤差OKレベル				[Pluse]				*/
		LONG	AccTime;						/* 加減速時間					[Times/Cycle]		*/
		LONG	ClmpTime;						/* 一定速時間					[Times/Cycle]		*/
		LONG	WaitTime;						/* 待ち時間						[Times/Cycle]		*/
		LONG	InteglessTime;					/* 積分レス開始時間				[Times/Cycle]		*/
		LONG	IncTime;						/* 推力指令加減時間				[Times/Cycle]		*/
		LONG	ConstTime;						/* 推力指一定時間				[Times/Cycle]		*/
		LONG	RepeatNum;						/* 磁極検出繰り返し回数								*/
	} conf;
	/*----------------------------------------------------------------------------------------------*/
	struct	{								/* 磁極検出用定数データ定義								*/
		BOOL	Fail;							/* エラー発生(速度偏差 or トルク制限)				*/
		BOOL	Direction;						/* 運転指令方向										*/
		BOOL	AxisChange;						/* d-q軸トルク印加フラグ							*/
		BOOL	ClrIntegral;					/* 積分バッファクリア								*/
		BOOL	PhaseChange;					/* 位相変更											*/
		BOOL	ZeroTorque;						/* ゼロトルク指令出力フラグ							*/
		BOOL	RangeFix;						/* 検出領域確定										*/
		BOOL	InteglessON;					/* 0：積分レス無効、1：積分レス実行					*/
		BOOL	ReverseLevel;					/* 逆走判定レベル(0：速度指令中、1：待ち時間中)		*/
		BOOL	OSFail;							/* 過速度オーバー									*/
		BOOL	PhaseConfFlg;					/* 磁極検出結果確認用トルク制御要求フラグ			*/
		BOOL	NoGood;							/* 角度計算エラー情報(0：OK、1：ゲインを上げる、	*/
	/*----------------------------------------------------------------------------------------------*/
		LONG	SpdErr;							/* 速度偏差											*/
		LONG	Ivar64[2];						/* 速度ループ積分バッファ							*/
		LONG	dAxisMaxTrqPlus;				/* 正方向d軸最大トルク								*/
		LONG	qAxisMaxTrqPlus;				/* 正方向q軸最大トルク								*/
		LONG	dAxisMaxTrqMinus;				/* 負方向d軸最大トルク								*/
		LONG	qAxisMaxTrqMinus;				/* 負方向q軸最大トルク								*/
		LONG	dAxisMaxSpdPlus;				/* 正方向d軸最大速度								*/
		LONG	qAxisMaxSpdPlus;				/* 正方向q軸最大速度								*/
		LONG	dAxisMaxSpdMinus;				/* 負方向d軸最大速度								*/
		LONG	qAxisMaxSpdMinus;				/* 負方向q軸最大速度								*/
		LONG	AxisErr;						/* d-q軸ずれ										*/
		LONG	MovPos;							/* 磁極検出移動量									*/
		LONG	TrqRefPhaConf;					/* 磁極検出確認用推力指令							*/
		LONG	MovPosPhaConf;					/* 磁極検出確認用位置								*/
		LONG	PhsOffsetComp;					/* 磁極検出位置補正量								*/
		LONG	SpdOffset;						/* 動作開始前の速度									*/
	/*----------------------------------------------------------------------------------------------*/
		LONG	AdjSwpDeg;						/* 調整位相変更刻み									*/
		LONG	AdjDegree;						/* 調整位相											*/
		LONG	PastTime;						/* 運転開始後経過時間								*/
		LONG	RefOutTime;						/* 時間ワークバッファ(速度指令作成関数内のみ)		*/
		LONG	PhsOffsetCalRslt;				/* 磁極検出位置(理論値算出結果)						*/
		LONG	PhaseOffsetCalComp;				/* 磁極検出動作確認後補正量(モニタ用)				*/
		LONG	DegReverse[4];					/* 逆走時の角度										*/
		LONG	DegRev;							/* 逆走時の合成角度									*/
		LONG	DegFinal;						/* 最終合成角度										*/
	/*----------------------------------------------------------------------------------------------*/
		LONG	KvGain;							/* ゲイン自動調整パラメータ							*/
		LONG	Step;							/* 磁極検出シーケンスステップ						*/
		LONG	RefOutStep;						/* 指令出力シーケンスステップ						*/
		LONG	RangeFixStep;					/* 磁極領域検出シーケンスステップ					*/
		LONG	Times;							/* 繰り返し回数										*/
		LONG	FailTimes;						/* 失敗回数											*/
		LONG	AxisErrSign;					/* d-q軸ずれ方向(トルク)							*/
		LONG	AreaCheckStep;					/* 8分割領域チェックステップ						*/
		LONG	ReverseCtr;						/* 逆走回数											*/
		LONG	ConfirmStep;					/* 磁極検出結果確認ステップ							*/
		LONG	SpdFbFilo;						/* 速度FBフィルタ出力	<V302>						*/
	} var;
	BOOL	MpfSeqStartReq;						/* 磁極検出シーケンス開始要求						*/
	LONG	MpFindSpdRef;						/* 磁極検出速度指令出力								*/
	LONG	TrqFilOut;							/* トルクフィルタ出力								*/
	LONG	MpfPhaseOffset;						/* 磁極検出位相オフセット 		[65536/360deg]		*/
	LONG	MPJudgeMonitor[3];					/* 磁極検出判定モニタ								*/
	LONG	UnMpFindingTime;					/* Un082  : 磁極検出所要時間		[0.1s]			*/
	LONG	UnMpfMovePosP;						/* Un080  : 磁極検出正方向最大移動量[0.01mm]		*/
	LONG	UnMpfMovePosN;						/* Un081  : 磁極検出逆方向最大移動量[0.01mm]		*/
} MPFIND;


#define MPF_HANDLER_H
#endif /* MPF_HANDLER_H */
/***************************************** end of file **********************************************/
