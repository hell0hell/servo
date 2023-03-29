/****************************************************************************************************/
/*																									*/
/*																									*/
/*		UnitPrmStruct.h : パラメータ関連構造体定義													*/
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
#ifndef	_UNIT_PRM_STRUCT_H_
#define	_UNIT_PRM_STRUCT_H_


#include "PnPrmStruct.h"
#include "Bprm.h"


#if 0
/****************************************************************************************************/
/*																									*/
/*		初期パラメータ定義 : 初期化処理で計算するパラメータ (電源再投入で変更有効)					*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		初期パラメータ定義																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct IPRMDAT
{
/*--------------------------------------------------------------------------------------------------*/
//		BOOL	RvsDir;					/* 逆回転モード												*/
//		BOOL	FastCtrl;				/* サーボ高速制御選択										*/
//		BOOL	WrnOutSel;				/* ワーニングコード出力選択									*/
//		BOOL	SpdIctrlKeep;			/* 速度制御積分保持											*/
//		BOOL	RegRxIn;				/* 回生抵抗内蔵												*/
//		BOOL	Dcbus;					/* ＤＣ電源入力選択											*/
//		BOOL	Ac3Phase;				/* 電源入力３相												*/
//		BOOL	DetectAcOn2;			/* 電源欠相検出方式											*/
//		BOOL	OhdetPort;				/* ヒートシンク過熱検出をポートで行う						*/
//		BOOL	DbTriack;				/* ＤＢ方式トライアック										*/
//		BOOL	PerClrMecha;			/* 位置偏差クリア補正の基準(機械位置を基準にした補正位置)	*/
//		BOOL	AutoRun;				/* 試験用自動運転モード										*/
//		BOOL	BatWrnMode;				/* エンコーダバッテリ電圧低下をワーニングとする				*/
//		BOOL	TrefTrqFF;				/* トルク指令をトルクＦＦとして使用(アナログパルスのみ)		*/
//		BOOL	TrefTrqLmt;				/* トルク指令を外部トルク制限として使用						*/
//		BOOL	TrefTrqLmtCL;			/* トルク指令を外部トルク制限として使用(P-CL,N-CL有効時)	*/
//		BOOL	VrefSpdLmt;				/* 速度指令を外部速度制限として使用							*/
//		BOOL	PosConvMode;			/* モータ位置指令単位変換モード設定							*/
//		BOOL	CtrlSW3Step;			/* 制御切り替え選択設定										*/
//		BOOL	ComCycleFil;			/* 通信周期移動平均選択設定									*/
//		BOOL	IPSpdControl;			/* Ｉ−Ｐ制御選択設定										*/
//		BOOL	CmdPDT;					/* 指令予測選択設定											*/
//		BOOL	VrefSpdFF;				/* 速度指令を速度ＦＦとして使用								*/
//		BOOL	OvrSpdLimit;			/* 速度制限値に過速度検出速度、Pn407の小さい方を使用		*/
//		BOOL	CtrlModeCspdPos;		/* ＳＧＤＢコンパチ制御モード切替え：接点速度／位置制御		*/
//		BOOL	CspdChangeCtrl;			/* ＳＧＤＢコンパチ制御モード切替え：接点速度／位置、速度、トルク制御 */
//		BOOL	EhVobs;					/* 位相補償速度オブザーバ選択設定							*/
//		BOOL	DbOHeatCheck;			/* 温度上昇によるＤＢ過負荷検出処理の有無選択				*/
//		BOOL	G2AlmStopMode;			/* G2アラームの停止方法										*/
//		BOOL	Vobs;					/* 速度ＦＢ補正機能設定										*/
//		BOOL	MpKvvFlg;				/* 主回路電圧補正機能選択	[0:OFF,1:ON]					*/
//		BOOL	TuningLess;				/* 調整レス設定												*/
//		BOOL	TuningLessVComp;		/* 調整レス位相進み補償設定									*/
//		BOOL	HwbbSupport	;			/* ＨＷＢＢ機能対応選択										*/
//		BOOL	UvTrqLmtCtrl;			/* 不足電圧ワーニング時トルク制限制御選択					*/
//		BOOL	PoleDetectEnd;			/* 電流零位相位置保存実行フラグ	[0:未完、1:完了]			*/
//		BOOL	SpdDetMaFil;			/* モータ速度検出用移動平均フィルタ有効/無効選択			*/
	/*----------------------------------------------------------------------------------------------*/
//★<you> RvsDirを参照すること		CHAR	DirSign;				/* 回転モード演算用(+1/-1)		*/
//		UCHAR	AxisAddr;				/* 軸アドレス(シリアル通信用)								*/
//		UCHAR	StopMode;				/* サーボオフ及びアラーム発生時の停止モード					*/
//		UCHAR	OtStopMode;				/* オーバトラベル(ＯＴ)時の停止方法							*/
//		UCHAR	MspdMafNum;				/* モータ速度検出移動平均回数								*/

//		UCHAR	PcmdFilter;				/* 位置指令フィルタ選択設定									*/
//		UCHAR	BacklashDir;			/* バックラッシ補正方向選択設定								*/
//		UCHAR	BVIBControl;			/* ゆれどめ制御選択設定										*/
//		UCHAR	PDTPrmMethod;			/* 予測パラメータ設定方式選択設定							*/

//		UCHAR	PDTPrmTp;				/* 予測パラメータTp選択設定									*/
//		UCHAR	PerClrMode;				/* 位置偏差クリア動作選択設定								*/
//		UCHAR	PerClrSigSel;			/* 位置偏差クリア信号形態選択設定							*/
//		UCHAR	PerClrLvlSel;			/* 位置偏差クリア信号レベル選択設定							*/

//		UCHAR	CoinOut;				/* /COIN出力タイミング選択設定								*/
//		UCHAR	VdetMode;				/* 主回路母線電圧検出方式									*/
//		UCHAR	VdetFollowWidthMax;		/* PWMによる主回路検出時可変調幅最大値						*/
//		LONG	VdcdetAlmFilter;		/* 主回路検出異常アラームフィルタ		(1=250us)			*/

//		LONG	OvdetAlmFilter;			/* 過電圧アラームフィルタ				(1=250us)			*/
//		LONG	RegStartFilter;			/* 回生動作開始フィルタ					(1=250us)			*/
//		LONG	RegStopFilter;			/* 回生動作開始フィルタ					(1=250us)			*/
//		LONG	RgSumSel;				/* 回生過負荷検出用初期値割合選択							*/
//		LONG	RlyDbWay;				/* 突入/DB制御方法選択設定 									*/

//		LONG	MFC_type;				/* MFC処理タイプ											*/
//		LONG	TuningLessType;			/* 調整レス処理タイプ										*/
//		UCHAR	padding;				// padding for 4byte alignment
	/*----------------------------------------------------------------------------------------------*/
//		USHORT	CtrlModeA;				/* 制御モードＡ												*/
//		USHORT	CtrlModeB;				/* 制御モードＢ												*/
	/*----------------------------------------------------------------------------------------------*/
//		USHORT	VdetPrmOk;				/* 主回路検出関連パラメータＯＫ	(1=OK)						*/
//		USHORT	OvLevel;				/* 過電圧(OV)検出レベル										*/
//		USHORT	UvLevel;				/* 不足電圧(UV)検出レベル									*/
//		USHORT	Uv2Level;				/* ＵＶ２(DB制動用ChargePump駆動許可電圧)検出レベル			*/
//		USHORT	RegOnLevel;				/* 回生Ｔｒオンレベル										*/
//		USHORT	RegOffLevel;			/* 回生Ｔｒオフレベル										*/
//		USHORT	UvWrnLevel;				/* 不足電圧(UV)ワーニング検出レベル							*/
//		USHORT	padding2;				// padding for 4byte alignment
//		LONG	VdetAdNorm;				/* 主回路検出ＡＤ変換結果正規化定数							*/
	/*----------------------------------------------------------------------------------------------*/
//		LONG	mtlmt;					/* マルチターンリミット設定 (Prm --> Iprm)					*/
	/*----------------------------------------------------------------------------------------------*/
//		LONG	TuningLessMode;			/* 調整レスモード											*/
/*----------------------------------------------------------------------------------------------*/
//	KSGAIN	PcmdOvrSpd;				/* 位置指令過速度(ＯＳ)			[pulse/SvCycle]				*/
	/*----------------------------------------------------------------------------------------------*/
//		LONG	TrefonLevel;			/* トルク指令入力ＯＮレベル		[2^24/MaxTrq]				*/
//		LONG	OrgSrchSpeed;			/* 原点サーチ速度				[2^24/OvrSpd]				*/
	/*----------------------------------------------------------------------------------------------*/
//		BOOL	CarrChange;				/* キャリア周波数切り替えフラグ [0:OFF,1:ON]				*/
//		BOOL	CarrState;				/* キャリア周波数切り替えステータス [0:Norm,1:High]			*/
//		LONG	IqLimitA;				/* キャリア切り替え電流閾値A	[(15000/MaxTrq)^2]			*/
//		LONG	IqLimitB;				/* キャリア切り替え電流閾値B	[2^24/MaxTrq]				*/
//		LONG	VelLimitA;				/* キャリア切り替え速度閾値A	[2^24/OvrSpd]				*/
//		LONG	VelLimitB;				/* キャリア切り替え速度閾値B	[2^24/OvrSpd]				*/
//		LONG	NormCarrCnt;			/* 通常キャリアカウント値		[-]							*/
//		LONG	HighCarrCnt;			/* 切り替え高キャリアカウント値	[-]							*/
	/*----------------------------------------------------------------------------------------------*/
//		LONG	SpdFbLpassFil;			/* モータ静音用速度ＦＢLPF設定	[0.01ms]		<V423>		*/
	/*----------------------------------------------------------------------------------------------*/
//		LONG	VdetAdFil;				/* 主回路検出ＡＤフィルタゲイン	(KIOPから移動) 2010.06.08 Y.Oka ユニット共通とする必要あり	*/
	/*----------------------------------------------------------------------------------------------*/
//		KSGAIN	LinearCoolRate100PerGain;	/* モータ自走冷却率100%ゲイン				<V519>		*/
	/*----------------------------------------------------------------------------------------------*/
} IPRMDAT;


/****************************************************************************************************/
/*																									*/
/*		共通パラメータ定義 : オンライン変更可能な共通パラメータ(複数モジュールで使用)				*/
/*																									*/
/****************************************************************************************************/
typedef	struct GPRMDAT
{
/*--------------------------------------------------------------------------------------------------*/
//		BOOL	NotchFil2;				/* ２段目ノッチフィルタ使用／不使用							*/
//		BOOL	LpassFil2;				/* ２次遅れフィルタ(CPU)使用／不使用						*/
//		BOOL	LpassFil3;				/* ﾄﾙｸ補償後１次遅れフィルタ(CPU)使用／不使用				*//*<V371>*/
//		BOOL	Dobs;					/* 外乱オブザーバ選択設定									*/
//		BOOL	AresVibControl;			/* Ａ型制振制御設定											*/
//		BOOL	AvffFilter;				/* 振動抑制フィルタ設定										*/
//		BOOL	MFCFF;					/* ＭＦＣ中の外部ＦＦ(速度，ﾄﾙｸ)有効／無効設定				*/
//		BOOL	VelRippleComp;			/* リップル補正機能設定										*//*<V072>*/
//		BOOL	TuningLess;				/* 調整レス設定												*//*<V108>*/
//		BOOL	TuningLessVComp;		/* 調整レス位相進み補償設定									*//*<V323>*/
//		BOOL	AtGsel;					/* 自動ゲイン切替選択設定									*//*<V177>*/
//		BOOL	PdetEnable;				/* パラメータによる磁極検出設定								*//*<V249>*/
	/*----------------------------------------------------------------------------------------------*/
//		LONG	MSW;					/* モードスイッチ選択										*/
//		LONG	MFCModel;				/* ＭＦＣ用機構系モデル選択設定								*/
	/*----------------------------------------------------------------------------------------------*/
//		LONG	AtGselTrgA;				/* 自動ゲイン切替条件Ａ選択設定								*//* <V177> */
	/*----------------------------------------------------------------------------------------------*/
//		LONG	MSWTrqLevel;			/* モードスイッチトルクレベル				 [2^24/MaxTrq]	*/
//		LONG	MSWSpdLevel;			/* モードスイッチ速度レベル					 [2^24/OvrSpd]	*/
//		LONG	MSWAccLevel;			/* モードスイッチ加速度レベル				 [NorSpd/ScanC]	*/
//		LONG	MSWErrLevel;			/* モードスイッチ偏差レベル					 [Pulse, mm]	*/
	/*----------------------------------------------------------------------------------------------*/
//		LONG	InterSpdLevel[4];		/* 内部設定速度レベル						 [2^24/OvrSpd]	*/
//		LONG	TConSpdLmtLevel;		/* トルク制御時の速度制限レベル				 [2^24/OvrSpd]	*/
	/*----------------------------------------------------------------------------------------------*/
//		LONG	CoinLevel;				/* 位置決め完了幅								[指令単位]	*/
//		LONG	NearLevel;				/* Ｎｅａｒ信号幅								[指令単位]	*/
//		LONG	VcmpSpdLevel;			/* 速度一致検出幅							 [2^24/OvrSpd]	*/
//		LONG	TgonSpdLevel;			/* モータ回転検出レベル						 [2^24/OvrSpd]	*/
//		LONG	VibCheckLevel;			/* 振動検出レベル						 	 [2^24/OvrSpd]	*/
//		LONG	VibCheckLevelForTunLess;/* 自動ノッチ用振動検出レベル			 	 [2^24/OvrSpd]	*//*<V109>*/
//		LONG	JstVibCheckLevel;		/* 慣性モーメント同定中の振動検出レベル	 	 [2^24/OvrSpd]	*/
	/*----------------------------------------------------------------------------------------------*/
//		LONG	BkonSpdLevel;			/* ブレーキ指令出力速度レベル(Pn507)		 [2^24/OvrSpd]	*/
//		LONG	BkonWaitTime;			/* サーボオフブレーキ指令待ち時間(Pn508)			  [ms]	*/
//		LONG	BkonDelayTime;			/* ブレーキ指令サーボオフ遅れ時間(Pn506)			  [ms]	*/
//		LONG	ZclampSpdLevel;			/* ゼロクランプ速度レベル(Pn501)			 [2^24/OvrSpd]	*/
	/*----------------------------------------------------------------------------------------------*/
//		LONG	PerAlmLevel;			/* 位置偏差過大アラームレベル					[指令単位]	*/
//		LONG	PerWrnLevel;			/* 位置偏差過大ワーニングレベル					[指令単位]	*/
//		LONG	SvonPerAlmLevel;		/* サーボオン時位置偏差過大アラームレベル		[指令単位]	*/
//		LONG	SvonPerWrnLevel;		/* サーボオン時位置偏差過大ワーニングレベル		[指令単位]	*/
//		LONG	SvonSpdLmtLevel;		/* サーボオン時速度制限レベル				 [2^24/OvrSpd]	*/
//		LONG	SvonSpdLmtValue;		/* サーボオン時速度制限値					 [2^24/OvrSpd]	*/
	/*----------------------------------------------------------------------------------------------*/
//		ULONG	fpgoverLevel;			/* モータ-負荷位置間偏差過大検出レベル			[指令単位]	*/
//		LONG	fpgcutratelv;			/* フルクローズ1回転当たりの乗算値					   [%]	*/
//		LONG	fcpgpls;				/* 外付けスケールピッチ数						   [P/Rev]	*/
	/*----------------------------------------------------------------------------------------------*/
//		LONG	FwIntTrqLmt;			/* 正転内部トルク制限						 [2^24/MaxTrq]	*/
//		LONG	RvIntTrqLmt;			/* 逆転内部トルク制限						 [2^24/MaxTrq]	*/
//		LONG	FwExtTrqLmt;			/* 正転外部トルク制限						 [2^24/MaxTrq]	*/
//		LONG	RvExtTrqLmt;			/* 逆転外部トルク制限						 [2^24/MaxTrq]	*/
//		LONG	EstopTrqLmt;			/* 非常停止トルク							 [2^24/MaxTrq]	*/
//		LONG	GrvCompTorque;			/* 重力補償トルク							 [2^24/MaxTrq]	*/
//		LONG	UvTrqLmt;				/* 主回路電圧降下時トルク制限				 [2^24/MaxTrq]	*//* <V200> */
//		LONG	DeltaUvTrqLmt;			/* 主回路電圧降下時トルク制限復帰トルク		 [2^24/MaxTrq]	*//* <V200> */
	/*----------------------------------------------------------------------------------------------*/
//		LONG	RemVibDetWidth;			/* 残留振動周波数検出幅							[pls]		*/
//		LONG	RemVibFil;				/* 残留振動周波数検出用位置偏差ﾌｨﾙﾀ周波数		[Hz]		*/
	/*----------------------------------------------------------------------------------------------*/
//		LONG	OvsErrDetLvl;			/* 許容オーバーシュート量					[指令単位]			*//* <V113> */
	/*----------------------------------------------------------------------------------------------*/
//		VUSHORT	*AsicMonAdr0;			/* Asic Register Monitor Address CH0						*/
//		VUSHORT	*AsicMonAdr1;			/* Asic Register Monitor Address CH1						*/
	/*----------------------------------------------------------------------------------------------*/
		/* リニア用 */
//		LONG	LinearCoolRateGain;		/* モータ自走冷却率ゲイン						<V019>		*/
/*--------------------------------------------------------------------------------------------------*/
} GPRMDAT;
#endif


//****************************************************************************************************
//
//	パラメータアクセスハンドル
//
//****************************************************************************************************
typedef struct UNI_PRM_STR {
//	IPRMDAT			*iprm_ptr;
//	GPRMDAT			*gprm_ptr;
	BPRMDAT			*Bprm;
	PRMDATA			*Prm;
	CMN_PRMDATA		*Cprm;
} UNI_PRM_STR;

#endif	/* _UNIT_PRM_STRUCT_H_ */
/***************************************** end of file **********************************************/
