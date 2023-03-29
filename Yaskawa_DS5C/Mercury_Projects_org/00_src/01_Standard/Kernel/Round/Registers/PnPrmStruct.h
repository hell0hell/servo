/****************************************************************************
Description	: Definition of the Parameter(Pn) Structure
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
I.M.   2010.5.22     created

****************************************************************************/
#ifndef __PN_PRM_STRUCT_H
#define __PN_PRM_STRUCT_H

#include "Basedef.h"
#include "EncData.h"
#include "MotorPoleFind.h"



#define VENDOR_IOPRM_NUM	4						/* SVCMD_IO(出力)VendorSpecificsI/O割り付けパラメータ数		*//* <S1C0> */

/****************************************************************************
	ユニット共通パラメータ構造体定義
****************************************************************************/
typedef struct CMN_PRMDATA {
	USHORT	b_prm6;					/* Pn006 : 機能選択応用スイッチ６									*//* 2軸目アナログモニタ対応<S103> */
	USHORT	b_prm7;					/* Pn007 : 機能選択応用スイッチ７									*//* 2軸目アナログモニタ対応<S103> */
//	USHORT	b_prm8;					/* Pn008 : 機能選択応用スイッチ８									*/
	USHORT	b_prmE;					/* Pn00E : 機能選択応用スイッチＥ									*//* <S0A6> */
	USHORT	b_prmF;					/* Pn00F : 機能選択応用スイッチＦ									*//* <S0A6> */
	USHORT	AxisAddress[2];			/* Pn010 : スロット別軸アドレス										*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	UniPrmSel;				/* Pn020 : 桁毎ユニット共通パラメータ設定							*//* <Oka06> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	CycleSwitch;			/* Pn041 : スキャン周期切り替えスイッチ								*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	acoftm;					/* Pn509 : 瞬停保持時間												*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	monoffset1;				/* Pn550 : アナログモニタ１オフセット電圧							*//* <S0DF> */
	USHORT	monoffset2;				/* Pn551 : アナログモニタ２オフセット電圧							*//* <S0DF> */
	USHORT	mongain1;				/* Pn552 : アナログモニタ1倍率										*//* <S0DF> */
	USHORT	mongain2;				/* Pn553 : アナログモニタ2倍率										*//* <S0DF> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	powmon_unitime ;		/* Pn55A : 消費電力モニタ単位時間									*//* <S0A6> *//* <S0DF> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	regres;					/* Pn600 : 回生抵抗容量												*/
	USHORT	exreger;				/* Pn603 : 回生抵抗値												*/

//	LONG	TrgioPosition[8];		/* Pn6xx : Trigger I/O */
//	USHORT	TrgioCfg[8];			/* Pn6xx : Trigger I/O */
//	USHORT	TrgioOnTime[8];			/* Pn6xx : Trigger I/O */
//	USHORT	TrgioAdjTime[8];		/* Pn6xx : Trigger I/O */
//	LONG	TrgIoDelayTime[8];		/* Pn6xx : Trigger I/O */

	USHORT	syssw2;					/* PnE0C : システムスイッチ２										*/
	DBYTEX	InstEnvTmp_sw1;			/* PnE0F : 下位:設置環境モニタ基準温度, 上位:機能制限スイッチ１		*//* <S03A> */
	USHORT	reger;					/* PnE1A : 回生抵抗値(デフォルト)									*//* <S0F4> */
	USHORT	wre;					/* PnE1C : 回生抵抗容量(デフォルト)									*/
	USHORT	kvvfil;					/* PnE3E : 主回路電圧補正用ローパスフィルタ時定数					*/
	USHORT	mnrng0;					/* PnE42 : モニタ１ゲイン											*//* <S016> *//* <S0DF> */
	USHORT	mnrng1;					/* PnE43 : モニタ２ゲイン											*//* <S016> *//* <S0DF> */
	ULONG 	mnptr0l;				/* PnE48 : モニタ１アドレス											*//* <S0DF> */
	ULONG 	mnptr1l;				/* PnE4A : モニタ２アドレス											*//* <S0DF> */
	DBYTEX	mnaz0_1;				/* PnE58 : モニタ１,２ゼロ調										*//* <S016> */
	DBYTEX	mnag0_1;				/* PnE59 : モニタ１,２ゲイン調										*//* <S016> */
	DBYTEX	pgadj;					/* PnE5A : 下位:PG電源電圧調, 上位:空き								*/
	DBYTEX	pnadj;					/* PnE5B : 下位:P-N電圧検出レベル微調, 上位:P-N電圧検出レベルゼロ調	*/

	USHORT	NonRegOvlvl;			/* PnE71 : 回生抵抗非内蔵機種で回生不使用時のＯＶ検出レベル			*//* <S0F7> */
	USHORT	pnvoltgn;				/* PnE70 : Ｐ竏窒m電圧検出レベル									*/
	USHORT	ovlvl;					/* PnE71 : ＯＶ検出レベル											*/
	USHORT	regonlvl;				/* PnE72 : 回生オンレベル											*/
	USHORT	uvlvl;					/* PnE73 : ＵＶ検出レベル											*/
	USHORT	uvfil;					/* PnE74 : ＵＶ検出フィルタ時定数									*/
	USHORT	regoffhys;				/* PnE78 : 回生オフヒステリシス幅									*/
	USHORT	vdetadfil;				/* PnE79 : 主回路検出ＡＤ用ローパスフィルタ時定数					*/
	USHORT	vdefilsw;				/* PnE7A : 主回路検出フィルタ選択スイッチ							*/
	USHORT	UvWrnLvl;				/* PnE7B : ＵＶワーニング検出レベル									*/

	USHORT	VdetADMax;				/* PnE7C : PnE70[V]入力時のA/D値 */
	USHORT	ConvW;					/* PnE7F : コンバータ容量(システム)									*/
#if 0 /* Σ-7不要のため削除 *//* <S0F3> */
	USHORT	CnvCrtADNormMax;		/* PnE80 : コンバータ電流検出AD=32767時の電流値 */
	USHORT	CnvCrtRatAD;			/* PnE81 : コンバータ連続過負荷検出AD値(仮) */
	USHORT	CnvCrtMaxAD;			/* PnE82 : コンバータ瞬時過負荷検出AD値(仮) */
	USHORT	CnvCrtRatTime;			/* PnE83 : コンバータ連続過負荷検出時間(仮)*/
	USHORT	CnvCrtMaxTime;			/* PnE84 : コンバータ瞬時過負荷検出時間(仮) */
#endif /* Σ-7不要のため削除 */
	/* <S154> Start */
	USHORT FanTemperatureOffset;    /* PnE80 : 	内蔵FAN寿命寿命計算用周囲温度オフセット	*/
	USHORT FanTimeOffset;			/* PnE81 :  FAN交換オフセット時間					*/
	USHORT RushLifeTime;			/* PnE82 :  突防回路寿命							*/
	/*  Start */
	ULONG RushSwichCount;			/* PnE84 :  現在の突防リレー動作回数				*/
//	USHORT RushSwichCount;			/* PnE83 :  現在の突防リレー動作回数（下位16bit）		*/
//	USHORT DBLifeTime;				/* PnE84 :  DB回路寿命							*/
//	USHORT DBSwichCount;			/* PnE85 : 	現在の現在のDB動作回数（下位16bit）		*/
//	USHORT DRSwithingCount;			/* PnE86 :  現在の突防/DBリレー動作回数（上位） 0,1桁目：
//												現在の突防動作回数（上位8bit） 2,3桁目：現在のDB動作回数（上位8bit）  */
	/* <S192> End */
	/* <S154> End */
	USHORT	ChgWaitTime;			/* PnE88 : ＤＣ充電完了待ち時間	<S053>								*/
	USHORT	PrdyWaitTime;			/* PnE89 : パワーレディ待ち時間	<S053>								*/
	USHORT	DischCompVlt;			/* PnE8A : 急速放電継続閾値電圧	<S1D1>								*/
	/* <S21D> Start */
	USHORT	regsynonlvl;			/* PnE9A : 回生同期時の回生ONレベル調整値							*/
	USHORT	regsynontime;			/* PnE9B : 回生同期時の回生ON待ち時間								*/
	/* <S21D> End */
	USHORT	ctrlboardalmtmpr;		/* PnE9C : 制御基板異常温度/警告温度								*//* <S0E2> */
	USHORT	pwrboardalmtmpr;		/* PnE9D : パワー基板異常温度/警告温度								*//* <S0E2> */
	USHORT	ctrlboardalmclrtmpr;	/* PnE9E : 制御基板異常クリア可能温度/異常温度検出時間				*//* <S0E2> */
	USHORT	pwrboardalmclrtmpr;		/* PnE9F : パワー基板異常クリア可能温度/異常温度検出時間			*//* <S0E2> */

	USHORT	FanLockTime;			/* PnEE5 : サーボパック時間内蔵ファン停止時間						*//* <S0A3> */
	USHORT	SvonFanLock_Time;		/* PnEE6 : サーボオン時内蔵ファン停止アラーム検出時間				*/	/* <S219> */

#if 0 /* Σ-7不要のため削除 *//* <S0F3> */
	USHORT	ohfil;					/* PnEF0 : 温度過熱検出フィルタ時定数								*/
	SHORT	mbohlvl;				/* PnEF1 : ユニット内部温度過熱アラームレベル						*/
	SHORT	convohlvl;				/* PnEF2 : コンバータ温度過熱アラームレベル							*/
	SHORT	ifohlvl;				/* PnEF3 : 制御基板温度過熱アラームレベル							*/
	USHORT	ohwrnlvl;				/* PnEF4 : 温度過熱ワーニングレベル									*/
#endif /* Σ-7不要のため削除 */
	USHORT	DcBaseLvl;				/* PnEF5 : 主回路電圧検出補正用 基準電圧							*/

	ULONG	SeqInputInf;			/* PnEF6 */
	ULONG	SeqOutputInf;			/* PnEF8 */
} CMN_PRMDATA;



/****************************************************************************
	パラメータデータ構造体定義
****************************************************************************/
typedef struct PRMDATA
{
/*------------------------------------------------------------------------------------------------------*/
	USHORT	b_prm0;					/* Pn000 : 機能選択基本スイッチ０									*/
	USHORT	b_prm1;					/* Pn001 : 機能選択応用スイッチ１									*/
	USHORT	b_prm2;					/* Pn002 : 機能選択応用スイッチ２									*/
//	USHORT	b_prm6;					/* Pn006 : 機能選択応用スイッチ６									*//* <S103> */
//	USHORT	b_prm7;					/* Pn007 : 機能選択応用スイッチ７									*//* <S103> */
	USHORT	b_prm8;					/* Pn008 : 機能選択応用スイッチ８									*/
	USHORT	b_prm9;					/* Pn009 : 機能選択応用スイッチ９									*/
	USHORT	b_prmA;					/* Pn00A : 機能選択応用スイッチＡ									*//* <S0A6> */
	USHORT	b_prmB;					/* Pn00B : 機能選択応用スイッチＢ									*/
	USHORT	b_prmC;					/* Pn00C : 機能選択応用スイッチＣ									*//* <S0A6> */
	USHORT	b_prmD;					/* Pn00D : 機能選択応用スイッチＤ									*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	FullcSlot;				/* Pn040 : フルクローズエンコーダ接続スロット番号					*/
	USHORT	b_prm80;				/* Pn080 : 機能選択応用スイッチ８０									*/
	USHORT	b_prm81;				/* Pn081 : 機能選択応用スイッチ８１									*//* <S0A6> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	loophz;					/* Pn100 : 速度ループゲイン											*/
	USHORT	pitime;					/* Pn101 : 速度ループ積分時定数										*/
	USHORT	posgn;					/* Pn102 : 位置ループゲイン											*/
	USHORT	jrate;					/* Pn103 : 慣性モーメント比											*/
	USHORT	loophz2;				/* Pn104 : 第２速度ループゲイン										*/
	USHORT	pitime2;				/* Pn105 : 第２速度ループ積分時定数									*/
	USHORT	posgn2;					/* Pn106 : 第２位置ループゲイン										*/
	USHORT	ffgn;					/* Pn109 : フィードフォワード										*/
	USHORT	fffil;					/* Pn10A : フィードフォワードフィルタ時定数							*/
	USHORT	gnmode;					/* Pn10B : ゲイン関係応用スイッチ									*/
	USHORT	mdswlv;					/* Pn10C : モードスイッチ(トルク指令)								*/
	USHORT	mswrf;					/* Pn10D : モードスイッチ(速度指令)									*/
	USHORT	mswacc;					/* Pn10E : モードスイッチ(加速度)									*/
	USHORT	msplvl;					/* Pn10F : モードスイッチ(位置偏差)									*/
	USHORT	pitfil;					/* Pn11F : 位置積分時定数											*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	dobgn;					/* Pn121 : 摩擦補償ゲイン											*/
	USHORT	dobgn2;					/* Pn122 : 第２摩擦補償ゲイン										*/
	USHORT	dtrqgn;					/* Pn123 : 摩擦補償係数												*/
	SHORT	dlpfil;					/* Pn124 : 摩擦補償周波数補正										*/
	USHORT	dobjgn;					/* Pn125 : 摩擦補償ゲイン補正										*/
	USHORT	evobgn;					/* Pn127 : 速度オブザーバゲイン										*/
	USHORT	evobjgn;				/* Pn128 : 速度オブザーバ位置補償ゲイン								*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	gnswfil1;				/* Pn131 : ゲイン切り替え時間１										*/
	USHORT	gnswfil2;				/* Pn132 : ゲイン切り替え時間２										*/
	USHORT	gnswwait1;				/* Pn135 : ゲイン切り替え待ち時間１									*/
	USHORT	gnswwait2;				/* Pn136 : ゲイン切り替え待ち時間２									*/
	USHORT	gnsw1;					/* Pn139 : 自動ゲイン切り替え関係スイッチ１							*/
	USHORT	Pn13B;					/* Pn13B : 速度オブザーバローパスフィルタ時定数						*/
	USHORT	KcGainRatio;			/* Pn13D : 電流ゲインレベル											*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	mdlsw;					/* Pn140 : モデル追従制御関連スイッチ								*/
	USHORT	mdlgn;					/* Pn141 : モデル追従制御ゲイン [0.1/s]								*/
	USHORT	mdlzeta;				/* Pn142 : モデル追従制御ゲイン補正									*/
	USHORT	mdlptff;				/* Pn143 : モデル追従制御バイアス(正転方向)							*/
	USHORT	mdlntff;				/* Pn144 : モデル追従制御バイアス(逆転方向)							*/
	USHORT	mdlantfrq;				/* Pn145 : 振動抑制１周波数Ａ										*/
	USHORT	mdlresfrq;				/* Pn146 : 振動抑制１周波数Ｂ										*/
	USHORT	mdlvff;					/* Pn147 : モデル追従制御速度フィードフォワード補償					*/
	USHORT	mdlgn2;					/* Pn148 : 第２モデル追従制御ゲイン [0.1/s]							*/
	USHORT	mdlzeta2;				/* Pn149 : 第２モデル追従制御ゲイン補正								*/
	USHORT	ff_frq;					/* Pn14A : 振動抑制２周波数											*/
	USHORT	ff_fil;					/* Pn14B : 振動抑制２補正											*/
	USHORT	mfctype;				/* Pn14F : 制御関連スイッチ											*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	avibsw;					/* Pn160 : 制振制御関連スイッチ										*/
	USHORT	avibfrq;				/* Pn161 : Ａ型制振周波数											*/
	USHORT	avibgn;					/* Pn162 : Ａ型制振ゲイン補正										*/
	USHORT	avibdamp;				/* Pn163 : Ａ型制振ダンピングゲイン									*/
	SHORT	avibfil1;				/* Pn164 : Ａ型制振フィルタ時定数１補正								*/
	SHORT	avibfil2;				/* Pn165 : Ａ型制振フィルタ時定数２補正								*/
	USHORT	avibdamp2;				/* Pn166 : 3慣性制振ダンピングゲイン				<S03E>		*/
	SHORT	avibfrq2;				/* Pn167 : 3慣性制振周波数						<S03E>		*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	DatLevel;				/* Pn170 : ダイナミックオートチューニング関連スイッチ				*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	jrateSw;				/* Pn17A : 予約パラメータ（イナーシャ比切り替え関係スイッチ）		*//* <S0A6> */
	USHORT	jrate2;					/* Pn17B : 第2イナーシャ比											*//* <S0A6> */
	USHORT	jrateChgTime1;			/* Pn17C : イナーシャ比切り替え時間１								*//* <S0A6> */
	USHORT	jrateChgTime2;			/* Pn17D : イナーシャ比切り替え時間２								*//* <S0A6> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	mswrf_l;				/* Pn181 : モードスイッチ(速度指令)									*/
	USHORT	mswacc_l;				/* Pn182 : モードスイッチ(加速度)									*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	divlessSw;				/* Pn190 : 予約パラメータ（偏差レス・ｽ関連スイッチ）					*//* <S0A6> */
	USHORT	velffgain;				/* Pn191 : 速度フィードフォワードゲイン								*//* <S0A6> */
	USHORT	trqffgain;				/* Pn192 : トルクフィードフォワードゲイン							*//* <S0A6> */
	USHORT	trqfftime;				/* Pn193 : トルクフィードフォワードフィルタ時定数					*//* <S0A6> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	prstsw;					/* Pn200 : 位置制御指令形態選択スイッチ								*/
	USHORT	mtlmt;					/* Pn205 : マルチターンリミット設定									*/
	USHORT	cmfilsw;				/* Pn207 : 位置指令機能スイッチ										*/
	USHORT	expbias1;				/* Pn209 : 位置指令加減速バイアス									*/
	ULONG	fcplsml;				/* Pn20A : 外付けスケールピッチ数									*/
	ULONG	ratb2l;					/* Pn20E : 電子ギア比(分子)											*/
/*------------------------------------------------------------------------------------------------------*/
	ULONG	rata2l;					/* Pn210 : 電子ギア比(分母)											*/
	ULONG	pgrat2l;				/* Pn212 : ＰＧ分周比(パルス単位)									*/
	USHORT	expfil;					/* Pn216 : 位置指令加減速時定数										*/
	USHORT	mafil;					/* Pn217 : 位置指令移動平均時間										*/
	USHORT	MulFrq;					/* Pn218 : 指令パルス入力倍率										*/
	USHORT	highrad;				/* Pn21B : HIGH filter average rad                                 *//*<S0C3>*/
	/*------------------------------------------------------------------------------------------------------*/
	USHORT	prefHfilacctime;		/* Pn21B : 位置指令高次フィルタ加減速時間							*//* <S0A6> */
	USHORT	prefHfilsmooth;			/* Pn21C : 位置指令高次フィルタスムージング割合						*//* <S0A6> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT  fcsel;					/* Pn22A : フルクローズ制御選択スイッチ								*//* <S04D> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	cmctrlexsw;				/* Pn230 : 位置制御拡張機能スイッチ									*/
	LONG	blcm2l;					/* Pn231 : バックラッシ補正量										*/
	USHORT	blfiltime;				/* Pn233 : バックラッシ補正時定数									*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	perrclrsigtime;			/* Pn240 : 位置偏差クリア信号入力最小時間間隔						*//* <S0A6> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	encbaudrate;			/* Pn250 : エンコーダ通信速度選択									*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	scalepgrat;				/* Pn281 : エンコーダ出力分解能										*/
	ULONG	scalepitch2l;			/* Pn282 : リニアスケールピッチ										*/
/*------------------------------------------------------------------------------------------------------*/
	ULONG	psencernum;				/* Pn2D0 : 予約パラメータ（位置センサ出力数）						*//* <S0A6> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	vrefgn;					/* Pn300 : 速度指令入力ゲイン										*/
	USHORT	speed1;					/* Pn301 : 内部設定速度1(回転)										*/
	USHORT	speed2;					/* Pn302 : 内部設定速度2(回転)										*/
	USHORT	speed3;					/* Pn303 : 内部設定速度3(回転)										*/
	USHORT	jogspd;					/* Pn304 : ＪＯＧ速度(回転)											*/
	USHORT	sfsacc;					/* Pn305 : ソフトスタート加速時間									*/
	USHORT	sfsdec;					/* Pn306 : ソフトスタート減速時間									*/
	USHORT	srfil;					/* Pn307 : 速度指令フィルタ時定数									*/
	USHORT	fbfil;					/* Pn308 : 速度Ｆ/Ｂフィルタ時定数									*/
	USHORT	svoffdectime;			/* 0x30A : サーボオフ及び強制停止時の減速時間						*//* <S0A6> */
	USHORT	vrefmafil;				/* 0x30B : 速度指令移動平均時間										*//* <S0A6> */
	USHORT	vffmafil;				/* 0x30C : 速度フィードフォワード移動平均時間						*//* <S0DF> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	vibsel;					/* Pn310 : 振動検出スイッチ											*/
	USHORT	vibsens;				/* Pn311 : 振動検出感度												*/
	USHORT	vibnorm;				/* Pn312 : 振動検出レベル(回転)										*/
	USHORT	maxvel;					/* 0x316 : モータ最高速度(回転)										*//* <S0DF> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	Pn324;					/* Pn324 : 慣性モーメント同定開始レベル								*/
	USHORT	Pn325;					/* Pn325 : 慣性モーメント同定中の振動検出レベル(回転)				*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	speed1_l;				/* Pn380 : 内部設定速度1(リニア)									*/
	USHORT	speed2_l;				/* Pn381 : 内部設定速度2(リニア)									*/
	USHORT	speed3_l;				/* Pn382 : 内部設定速度3(リニア)									*/
	USHORT	jogspd_l;				/* Pn383 : ＪＯＧ速度(リニア)										*/
	USHORT	vibnorm_l;				/* Pn384 : 振動検出レベル(リニア)									*/
	USHORT	maxvel_l;				/* Pn385 : モータ最高速度(リニア)									*/
	USHORT	Pn387;					/* Pn386 : 慣性モーメント同定中の振動検出レベル(リニア)				*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	trefgn;					/* Pn400 : トルク指令入力ゲイン										*/
	USHORT	trqfil11;				/* Pn401 : １段目第１トルク指令フィルタ時定数						*/
	USHORT	tlmtf;					/* Pn402 : 正転トルク制限											*/
	USHORT	tlmtr;					/* Pn403 : 逆転トルク制限											*/
	USHORT	outlmf1;				/* Pn404 : 正転側外部トルク制限										*/
	USHORT	outlmr1;				/* Pn405 : 逆転側外部トルク制限										*/
	USHORT	emgtrq;					/* Pn406 : 非常停止トルク											*/
	USHORT	tcrlmt;					/* Pn407 : トルク制御時の速度制限									*/
	USHORT	tfuncsw;				/* Pn408 : トルク関係機能スイッチ									*/
	USHORT	ntchfil1;				/* Pn409 : １段目ノッチフィルタ周波数								*/
	USHORT	ntchq1;					/* Pn40A : １段目ノッチフィルタＱ値									*/
	USHORT	ntchdamp1;				/* Pn40B : １段目ノッチフィルタ深さ									*/
	USHORT	ntchfil2;				/* Pn40C : ２段目ノッチフィルタ周波数								*/
	USHORT	ntchq2;					/* Pn40D : ２段目ノッチフィルタＱ値									*/
	USHORT	ntchdamp2;				/* Pn40E : ２段目ノッチフィルタ深さ									*/
	USHORT	secfil;					/* Pn40F : ２段目２次トルク指令フィルタ周波数						*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	secq;					/* Pn410 : ２段目２次トルク指令フィルタＱ値							*/
	USHORT	trqfil12;				/* Pn412 : １段目第２トルク指令フィルタ時定数						*/
	USHORT	treffil;				/* Pn415 : T-REFフィルタ時定数										*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	ntchfil3;				/* Pn416 : 3段目ノッチフィルタ周波数								*//* <S0A6> */
	USHORT	ntchq3;					/* Pn417 : 3段目ノッチフィルタQ値									*//* <S0A6> */
	USHORT	ntchdamp3;				/* Pn418 : 3段目ノッチフィルタ深さ									*//* <S0A6> */
	USHORT	ntchfil4;				/* Pn419 : 4段目ノッチフィルタ周波数								*//* <S0A6> */
	USHORT	ntchq4;					/* Pn41A : 4段目ノッチフィルタQ値									*//* <S0A6> */
	USHORT	ntchdamp4;				/* Pn41B : 4段目ノッチフィルタ深さ									*//* <S0A6> */
	USHORT	ntchfil5;				/* Pn41C : 5段目ノッチフィルタ周波数								*//* <S0A6> */
	USHORT	ntchq5;					/* Pn41D : 5段目ノッチフィルタQ値									*//* <S0A6> */
	USHORT	ntchdamp5;				/* Pn41E : 5段目ノッチフィルタ深さ									*//* <S0A6> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	tfuncsw2;				/* Pn41F : トルク(推力)関係機能スイッチ２							*//* <S0A6> */
	USHORT	RippleCompSw;			/* Pn423 : トルク補償関係機能スイッチ								*//* <S0A6> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	UvTrqLmt;				/* Pn424 : 主回路電圧降下時トルク制限 [%]							*/
	USHORT	UvTrqLmtOffFil;			/* Pn425 : 主回路電圧降下時トルク制限解除時間						*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	tffmafil;				/* 0x426 : トルクフィードフォワード移動平均時間						*//* <S0DF> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	VelRippleCompSpd;		/* Pn427 : 速度リップル補償有効速度									*/	/* <S14B> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	swpminfrq;				/* Pn453 : 掃引開始周波数											*/
	USHORT	swpmaxfrq;				/* Pn454 : 掃引終了周波数											*/
	USHORT	detfrqlmt;				/* Pn455 : 検出共振周波数下限値										*/
	USHORT	ezffttrq;				/* Pn456 : 掃引トルク指令振幅										*/
	USHORT	ezfftsw;				/* Pn457 : ＦＦＴ関係機能スイッチ									*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	anotchsw;				/* Pn460 : ノッチフィルタ調整スイッチ								*/
	USHORT	anotchsw2;				/* Pn466 : ノッチフィルタ調整スイッチ2								*//* <S0A6> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	gcomptrq;				/* Pn470 : 重力補償トルク											*//* <S0A6> */
	USHORT	pqfriqcomptrq;			/* Pn471 : 正方向クーロン摩擦補償トルク								*//* <S0A6> */
	USHORT	nqfriqcomptrq;			/* Pn472 : 負方向クーロン摩擦補償トルク								*//* <S0A6> */
	USHORT	vfrigcomptrq;			/* Pn473 : 粘性摩擦補償トルク										*//* <S0A6> */
	USHORT	spdhys;					/* Pn474 : 内部設定速度hysteresis									*//* <S0CD> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	tcrlmt_l;				/* Pn480 : 推力制御時の速度制限										*/
	USHORT	tlmtf_l;				/* Pn483 : 正転推力制限												*/
	USHORT	tlmtr_l;				/* Pn484 : 逆転推力制限												*/

	MPFIND_UPRM PdetP;				/* Pn481 - Pn49B : 磁極検出パラメータ 								*/

	/*------------------------------------------------------------------------------------------------------*/
	USHORT	clamp;					/* Pn501 : ゼロクランプレベル										*/
	USHORT	tgonlv;					/* Pn502 : 回転検出レベル											*/
	USHORT	vcmplv;					/* Pn503 : 速度一致信号出力幅										*/
	USHORT	brktim;					/* Pn506 : ブレーキ指令竏茶Tーボオフ遅れ時間							*/
	USHORT	brkspd;					/* Pn507 : ブレーキ指令出力速度レベル								*/
	USHORT	brkwai;					/* Pn508 : サーボオフ竏茶uレーキ指令待ち時間							*/
	USHORT	insel1;					/* Pn50A : 入力信号選択1											*/
	USHORT	insel2;					/* Pn50B : 入力信号選択2											*/
	USHORT	insel3;					/* Pn50C : 入力信号選択3											*/
	USHORT	insel4;					/* Pn50D : 入力信号選択4											*/
	USHORT	outsel1;				/* Pn50E : 出力信号選択1											*/
	USHORT	outsel2;				/* Pn50F : 出力信号選択2											*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	outsel3;				/* Pn510 : 出力信号選択3											*/
	USHORT	inselA;					/* Pn511 : 入力信号選択5											*/
//	USHORT	sorev;					/* Pn512 : 出力信号反転設定											*//* <S04B> */
	USHORT	sorev1;					/* Pn512 : 出力信号反転設定1										*//* <S04B> */
//	USHORT	outselA;				/* Pn513 : 出力信号選択4											*//* <S04B> */
	USHORT	sorev2;					/* Pn513 : 出力信号反転設定2										*//* <S04B> */
	USHORT	outsel4;				/* 0x514 : 出力信号選択5											*//* <S0A6> */
	USHORT	insel5;					/* Pn515 : 入力信号選択6											*/
	USHORT	insel6;					/* 0x516 : 入力信号選択7											*//* <S0A6> */

	USHORT	outsel6;				/* Pn517 : 出力信号選択6											*/
	USHORT	outsel7;				/* Pn518 : 出力信号選択7											*/
	USHORT	insel7;					/* Pn519 : 入力信号選択8											*//* <S0A6> */
	ULONG	fpgoverlv2l;			/* Pn51B : モータ-負荷位置間偏差過大検出レベル						*/
	USHORT	overwrnlv;				/* Pn51E : 位置偏差過大ワーニングレベル								*/
/*------------------------------------------------------------------------------------------------------*/
	ULONG	overlv2l;				/* Pn520 : 位置偏差過大アラームレベル								*/
	ULONG	coinlv2l;				/* Pn522 : 位置決め完了幅											*/
	ULONG	nearlv2l;				/* Pn524 : ＮＥＡＲ信号幅											*/
	ULONG	svonoverlv2l;			/* Pn526 : サーボオン時位置偏差過大アラームレベル					*/
	USHORT	svonoverwrnlv;			/* Pn528 : サーボオン時位置偏差過大ワーニングレベル					*/
	USHORT	svonspdlmtlv;			/* Pn529 : サーボオン時速度制限レベル								*/
	USHORT	fpgcutrate;				/* Pn52A : フルクローズ1回転当たりの乗算値							*/
	USHORT	olwarlvl;				/* Pn52B : 過負荷ワーニングレベル									*/
	USHORT	mtbastrq_drt;			/* Pn52C : モータ過負荷検出ベース電流ディレーティング				*/
	USHORT	svbastrq_drt;			/* Pn52D : 単相電源入力選択時サーボパック過負荷検出ベース電流ディレーティング	*//* <S0A6> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	powon_mon	;			/* 0x52F : 電源投入時のモニタ表示									*//* <S0A6> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	pjogsw;					/* Pn530 : プログラムＪＯＧ運転関係スイッチ							*/
	ULONG	pjogdistl;				/* Pn531 : プログラムＪＯＧ移動距離									*/
	USHORT	pjogrotspd;				/* Pn533 : プログラムＪＯＧ移動速度									*/
	USHORT	pjogacctm;				/* Pn534 : プログラムＪＯＧ加減速時間								*/
	USHORT	pjogwaittm;				/* Pn535 : プログラムＪＯＧ待ち時間									*/
	USHORT	pjognum;				/* Pn536 : プログラムＪＯＧ移動回数									*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	outalm_no1 ;			/* Pn53A : 信号出力アラーム・ワーニング番号1						*//* <S0A6> */
	USHORT	outalm_no2 ;			/* Pn53B : 信号出力アラーム・ワーニング番号2						*//* <S0A6> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	adatgmax;				/* Pn540 : ゲインリミット											*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	trcalm_no;				/* Pn548 : トレース指定アラーム番号									*//* <S0A6> */
/*------------------------------------------------------------------------------------------------------*/
//	USHORT	monoffset1;				/* Pn550 : アナログモニタ１オフセット電圧							*//* <S0DF> */
//	USHORT	monoffset2;				/* Pn551 : アナログモニタ２オフセット電圧							*//* <S0DF> */
//	USHORT	mongain1;				/* Pn552 : アナログモニタ1倍率										*//* <S0DF> */
//	USHORT	mongain2;				/* Pn553 : アナログモニタ2倍率										*//* <S0DF> */
/*------------------------------------------------------------------------------------------------------*/
//	USHORT	powmon_unitime ;		/* Pn55A : 消費電力モニタ単位時間									*//* <S0A6> *//* <S0DF> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	remdetw;				/* Pn560 : 残留振動検出幅											*/
	USHORT	ovserrdetlvl;			/* Pn561 : オーバーシュート検出レベル								*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	clamp_l;				/* Pn580 : ゼロクランプレベル										*/
	USHORT	tgonlv_l;				/* Pn581 : 回転検出レベル											*//* <S003> */
	USHORT	vcmplv_l;				/* Pn582 : 速度一致信号出力幅										*//* <S003> */
	USHORT	brkspd_l;				/* Pn583 : ブレーキ指令出力速度レベル								*/
	USHORT	svonspdlmtlv_l;			/* Pn584 : サーボオン時速度制限レベル								*/
	USHORT	pjoglinspd;				/* Pn585 : プログラムＪＯＧ最高速度(リニア)							*/
	USHORT	msrcra;					/* Pn586 : モータ自走冷却率											*/
	USHORT	pldetsw;				/* Pn587 : 絶対値スケール用磁極検出実行選択							*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	pot_selsw;				/* Pn590 : POT入力信号設定											*/
	USHORT	not_selsw;				/* Pn591 : NOT入力信号設定											*/
	USHORT	dec_selsw;				/* Pn592 : DEC入力信号設定											*/
	USHORT	ext_selsw[3];			/* Pn593 : EXT入力信号設定											*/
//	USHORT	bk_selsw;				/* Pn596 : BK出力信号設定											*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	fstp_selsw;				/* Pn597 : <S1B5> FSTP入力信号設定									*/
	USHORT	pcl_selsw;				/* Pn598 : <S1B5> P-CL入力信号設定									*/
	USHORT	ncl_selsw;				/* Pn599 : <S1B5> N-CL入力信号設定									*/
	USHORT	clinkans_selsw;			/* Pn59A : <S1B5> CLINKANS入力信号設定								*/
	USHORT	cmcans_selsw;			/* Pn59B : <S1B5> CMCANS入力信号設定								*/
	USHORT	convready_selsw;		/* Pn59C : <S1B5> CONV_READY入力信号設定							*/
	USHORT	convmcon_selsw;			/* Pn59D : <S1B5> CONV_MCON入力信号設定								*/
	USHORT	coin_selsw;				/* Pn5B0 : <S1B5> COIN出力信号設定									*/
	USHORT	vcmp_selsw;				/* Pn5B1 : <S1B5> VCMP出力信号設定									*/
	USHORT	tgon_selsw;				/* Pn5B2 : <S1B5> TGON出力信号設定									*/
	USHORT	srdy_selsw;				/* Pn5B3 : <S1B5> S-RDY出力信号設定									*/
	USHORT	clt_selsw;				/* Pn5B4 : <S1B5> CLT出力信号設定									*/
	USHORT	vlt_selsw;				/* Pn5B5 : <S1B5> VLT出力信号設定									*/
	USHORT	bk_selsw;				/* Pn5B6 : <S1B5> BK出力信号設定									*/
	USHORT	warn_selsw;				/* Pn5B7 : <S1B5> WARN出力信号設定									*/
	USHORT	near_selsw;				/* Pn5B8 : <S1B5> NEAR出力信号設定									*/
	USHORT	cpulse_selsw;			/* Pn5B9 : <S1B5> C-PULSE出力信号設定								*/
	USHORT	clink_selsw;			/* Pn5BA : <S1B5> CLINK出力信号設定									*/
	USHORT	cmcon_selsw;			/* Pn5BB : <S1B5> CMCON出力信号設定									*/
	USHORT	pm_selsw;				/* Pn5BC : <S1B5> PM出力信号設定									*/
	USHORT	dao_selsw[2];			/* Pn5BD : <S1B5> DAO出力信号設定									*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	dbres;					/* Pn601 : ＤＢ抵抗容量												*/
//	USHORT	dber;					/* Pn604 : ＤＢ抵抗値												*//* <S0A6> *//* <S0F5> */
	USHORT	exdbr;					/* Pn604 : ＤＢ抵抗値												*//* <S0F5> */
	USHORT	bbondiff0;				/* Pn620 : HWBB信号タイミング時間									*/
	USHORT	saftyfuncSw;			/* Pn621 : 安全機能応用選択スイッチ									*//* <S0A6> */
	USHORT	safeindecA;				/* Pn622 : アクティブモード内部減速定数A							*//* <S0A6> */
	USHORT	safeindecB;				/* Pn623 : アクティブモード内部減速定数B							*//* <S0A6> */
	USHORT	safeindecVel;			/* Pn624 : アクティブモード内部減速停止速度							*//* <S0A6> */
	USHORT	safeStopTime;			/* Pn625 : アクティブモード停止継続時間								*//* <S0A6> */
	ULONG	safePerrlvl;			/* Pn626 : アクティブモード解除位置偏差レベル						*//* <S0A6> */
	USHORT	safeVreflvl;			/* Pn628 : アクティブモード解除速度指令レベル						*//* <S0A6> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	sq_b_prm0;				/* Pn800 : SynqNet機能選択スイッチ０								*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	comCtrl;				/* Pn800 : 通信制御機能選択											*/
	USHORT	sLimitSw;				/* Pn801 : ソフトリミット機能選択									*/
	USHORT	cmdCtrlSw;				/* Pn802 : コマンド動作設定											*/
	USHORT	zpWidth;				/* Pn803 : 原点位置範囲												*/
#if 0/* <S07F>:型変更 */
	USHORT	p_sLimitL;				/* Pn804 : 正側ソフトリミット値 L									*/
	USHORT	p_sLimitH;				/* Pn805 : 正側ソフトリミット値 H									*/
	USHORT	n_sLimitL;				/* Pn806 : 負側ソフトリミット値 L									*/
	USHORT	n_sLimitH;				/* Pn807 : 負側ソフトリミット値 H									*/
	USHORT	absOffsetL;				/* Pn808 : アブソＰＧ原点位置オフセット L							*/
	USHORT	absOffsetH;				/* Pn809 : アブソＰＧ原点位置オフセット H							*/
#else
	LONG	p_sLimitL;				/* Pn804 : 正側ソフトリミット値 L									*/
	LONG	n_sLimitL;				/* Pn806 : 負側ソフトリミット値 L									*/
	LONG	absOffsetL;				/* Pn808 : アブソＰＧ原点位置オフセット L							*/
#endif/* <S07F>:型変更 */
	USHORT	accRate1;				/* Pn80A : １段目直線加速定数										*/
	USHORT	accRate2;				/* Pn80B : ２段目直線加速定数										*/
	USHORT	accChgSpd;				/* Pn80C : 加速定数切替え速度										*/
	USHORT	decRate1;				/* Pn80D : １段目直線減速定数										*/
	USHORT	decRate2;				/* Pn80E : ２段目直線減速定数										*/
	USHORT	decChgSpd;				/* Pn80F : 減速定数切替え速度										*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	filExpBias;				/* Pn810 : 指数関数加減速バイアス									*/
	USHORT	filExpTb;				/* Pn811 : 指数関数加減速時定数										*/
	USHORT	filMaTb;				/* Pn812 : 移動平均時間												*/
	USHORT	resrv2;					/* Pn813 : 予約														*/
#if 0/* <S07F>:型変更 */
	USHORT	exDistL;				/* Pn814 : 外部位置決め最終走行距離 L								*/
	USHORT	exDistH;				/* Pn815 : 外部位置決め最終走行距離 H								*/
#else
	LONG	exDistL;				/* Pn814 : 外部位置決め最終走行距離 L								*/
#endif/* <S07F>:型変更 */
	USHORT	zretDir;				/* Pn816 : 原点復帰モード設定										*/
	USHORT	zretSpd1;				/* Pn817 : 原点復帰アプローチ速度１									*/
	USHORT	zretSpd2;				/* Pn818 : 原点復帰アプローチ速度２									*/
#if 0/* <S07F>:型変更 */
	USHORT	zretDistL;				/* Pn819 : 原点復帰最終走行距離 L									*/
	USHORT	zretDistH;				/* Pn81A : 原点復帰最終走行距離 H									*/
#else
	LONG	zretDistL;				/* Pn819 : 原点復帰最終走行距離 L									*/
#endif/* <S07F>:型変更 */
	USHORT	resrv3;					/* Pn81B : 予約														*/
	USHORT	resrv4;					/* Pn81C : 予約														*/
	USHORT	resrv5;					/* Pn81D : 予約														*/
	USHORT	ioMonSel;				/* Pn81E : 入力信号モニタ選択										*/
	USHORT	optBitSw;				/* Pn81F : コマンドデータ割付選択									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	p_ltLimitL;				/* Pn820 : 正側ラッチ可能領域 L										*/
	LONG	n_ltLimitL;				/* Pn822 : 逆側ラッチ可能領域 L										*/
//	USHORT	n_ltLimitH;				/* Pn823 : 逆側ラッチ可能領域 H										*/
	USHORT	opMon1;					/* Pn824 : オプションモニタ１選択									*/
	USHORT	opMon2;					/* Pn825 : オプションモニタ２選択									*/
	USHORT	decRate1Emg;			/* Pn826 : 非常停止用１段目直線減速定数								*/
	USHORT	decRate2Emg;			/* Pn827 : 非常停止用２段目直線減速定数								*/
	USHORT	decChgSpdEmg;			/* Pn828 : 非常停止用減速定数切替え速度								*/
	USHORT	svOffTime;				/* Pn829 : SVOFF待ち時間(減速停止SVOFF時)							*/
	USHORT	optbit1;				/* Pn82A : OPTION機能ビット割付１									*/
	USHORT	optbit2;				/* Pn82B : OPTION機能ビット割付２									*/
	USHORT	optbit3;				/* Pn82C : OPTION機能ビット割付３									*/
	USHORT	optbit4;				/* Pn82D : OPTION機能ビット割付４									*/
	USHORT	optbit5;				/* Pn82E : OPTION機能ビット割付５									*/
	USHORT	resrv6;					/* Pn82F : 予約														*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	rotrefL;				/* Pn830 : 機械一回転当たりの指令単位量L							*/
	USHORT	rotrefH;				/* Pn831 : 機械一回転当たりの指令単位量H							*/
	USHORT	st_num;					/* Pn832 : 機械一回転当たりのステーション数							*/
	USHORT	rateChgSw;				/* Pn833 : POSING定数選択											*/
	ULONG	accRate12L;				/* Pn834 : １段目直線加速定数 2nd L									*/
	ULONG	accRate22L;				/* Pn836 : ２段目直線加速定数 2nd L									*/
	ULONG	accChgSpd2L;			/* Pn838 : 加速定数切替え速度 2nd L									*/
	ULONG	decRate12L;				/* Pn83A : １段目直線減速定数 2nd L									*/
	ULONG	decRate22L;				/* Pn83C : ２段目直線減速定数 2nd L									*/
	ULONG	decChgSpd2L;			/* Pn83E : 減速定数切替え速度 2nd L									*/
/*--------------------------------------------------------------------------------------------------*/
	ULONG	decRateEmg2L;			/* Pn840 : 非常停止用２段目直線減速定数　2nd L						*/
	LONG	zretSpd12L;				/* Pn842 : 原点復帰アプローチ速度１ 2nd L							*/
	LONG	zretSpd22L;				/* Pn844 : 原点復帰アプローチ速度２ 2nd L							*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	ltSeqNum;				/* Pn850 : 連続ラッチシーケンス数									*/
	USHORT	ltLoopNum;				/* Pn851 : 連続ラッチ回数											*/
	USHORT	ltSeqSet1;				/* Pn852 : 連続ラッチシーケンス設定1-4								*/
	USHORT	ltSeqSet2;				/* Pn853 : 連続ラッチシーケンス設定4-8								*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	svcmdIoOut[VENDOR_IOPRM_NUM];	/* Pn855~: SVCMD_IO出力Vendor Specific I/O領域割付機能X				*//* <S1C0> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	cmdIo1;					/* Pn860 : SVCMD_IO(入力信号モニタ)割付機能1						*/
	USHORT	cmdIo2;					/* Pn861 : SVCMD_IO(入力信号モニタ)割付機能2						*/
	USHORT	cmdIo3;					/* Pn862 : SVCMD_IO(入力信号モニタ)割付機能3						*/
	USHORT	cmdIo4;					/* Pn863 : SVCMD_IO(入力信号モニタ)割付機能4						*/
	USHORT	cmdIo5;					/* Pn864 : SVCMD_IO(入力信号モニタ)割付機能5						*/
	USHORT	cmdIo6;					/* Pn865 : SVCMD_IO(入力信号モニタ)割付機能6						*/
	USHORT	cmdIo7;					/* Pn866 : SVCMD_IO(入力信号モニタ)割付機能7						*/
	USHORT	cmdIoOut1;				/* Pn868 : SVCMD_IO(出力信号モニタ)割付機能1						*/
	USHORT	cmdIoOut2;				/* Pn869 : SVCMD_IO(出力信号モニタ)割付機能2						*/
	USHORT	cmdIoOut3;				/* Pn86A : SVCMD_IO(出力信号モニタ)割付機能2						*//* <S1C0> */
	USHORT	p_zone1L;				/* Pn870 : 正側ZONE1信号出力境界位置								*/
	USHORT	n_zone1L;				/* Pn872 : 負側ZONE1信号出力境界位置								*/
	USHORT	p_zone2L;				/* Pn874 : 正側ZONE2信号出力境界位置								*/
	USHORT	n_zone2L;				/* Pn876 : 負側ZONE2信号出力境界位置								*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	nodeAdr;				/* Pn880 : 局アドレス設定											*/
	USHORT	trByte;					/* Pn881 : 伝送バイト数												*/
	USHORT	comTCycle;				/* Pn882 : 伝送周期													*/
	USHORT	comAplCycle;			/* Pn883 : 通信周期												*/
	USHORT	commErrBehavior;
//	USHORT	comErrSts1;
//	USHORT	comErrSts2;
	USHORT	comDataCtrl;
	USHORT	comErrCnt;				/* Pn88A : 受信エラーカウンタ										*/

/*------------------------------------------------------------------------------------------------------*/
 #if (CSW_NETIF_TYPE == NETIF_M2_TYPE)												/* MECATROLINK- M2	*/
	USHORT	comErrSts1;
	USHORT	comErrSts2;
/* <S17F>型と要素数変更 */
	USHORT	almCmd[16];				/* Pn890 : アラーム時、通信CMDデータ								*//* <S0BD> */
	USHORT	almRsp[16];				/* Pn8A0 : アラーム時、通信RSPデータ								*//* <S0BD> */
	USHORT	comCmd[16];				/* Pn8B0 : オンライン、通信CMDデータ								*//* <S0BD> */
	USHORT	comRsp[16];				/* Pn8C0 : オンライン、通信RSPデータ								*//* <S0BD> */
/*------------------------------------------------------------------------------------------------------*/
 #elif (CSW_NETIF_TYPE == NETIF_M3_TYPE)					/*  MECATROLINK-M3	*/
	ULONG	comErrSts1L;
	ULONG	comErrSts2L;
/* <S0CA> Start *//* <S17F>型と要素数変更 */
	USHORT	almCmd[24];				/* Pn890 : アラーム時、通信CMDデータ								*//* <S0BD> */
	USHORT	almRsp[24];				/* Pn8A8 : アラーム時、通信RSPデータ								*//* <S0BD> */
	USHORT	comCmd[24];				/* Pn8C0 : オンライン、通信CMDデータ								*//* <S0BD> */
	USHORT	comRsp[24];				/* Pn8D8 : オンライン、通信RSPデータ								*//* <S0BD> */
/* <S0CA> End */
 #endif
	/*--------------------------------------------------------------------------------------------------*/
	USHORT	bnkNum;					/* Pn900 : バンク数													*/
	USHORT	bnkMemNum;				/* Pn901 : バンクメンバ数											*/
	/*--------------------------------------------------------------------------------------------------*/
	USHORT	bnkMember[15];			/* Pn902 : バンクメンバ												*//* <S080> */
	USHORT	bnkData[0x3F];			/* Pn920~Pn95F :各バンクメンバ値									*//* <S080> */
	/*--------------------------------------------------------------------------------------------------*/
	USHORT	prmVer;					/* Pn9E0 : パラメータVer.											*/
	USHORT	spdDivision;			/* Pn9E1 : 速度分解能設定											*/
	USHORT	CmdOpSw;				/* Pn9E2 : Command-Option機能SW										*/
	USHORT	sys_rsrv2;				/* Pn9E3 : 															*/
	USHORT	sys_rsrv3;				/* Pn9E4 : 															*//*------------------------------------------------------------------------------------------------------*/
	USHORT	posing_mode;			/* Pn8FF : 	POSINGモード選択										*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	monAdr;					/* Pn980 : 	他局モニタアドレス 										*/
	USHORT	monDataSize;			/* Pn981 : 	他局通信データサイズ									*/
	USHORT	monFuncSel;				/* Pn982 : 	他局モニタ機能選択スイッチ								*/
	USHORT	monDataSel;				/* Pn983 : 	停止動作の入力指令　モニタ設定							*/
	USHORT	trceStpSpd;				/* Pn984 : 	追従停止速度											*/
	USHORT	monAdj;					/* Pn985 : 	協調停止調整パラメータ									*/
/*------------------------------------------------------------------------------------------------------*/
	ULONG	m3_encTypeL;			/* PnA02 : エンコーダタイプ選択 L									*/
	ULONG	m3_motorTypeL;			/* PnA04 : モータタイプ選択 L										*/
	ULONG	m3_fulcTypeL;			/* PnA06 : セミクローズ／フルクローズ選択 L							*/
	ULONG	m3_ratSpdL;				/* PnA08 : 定格回転速度 L											*/
	ULONG	m3_permitSpdL;			/* PnA0A : 最大出力可能速度 L										*/
	ULONG	m3_spdMultiplierL;		/* PnA0C : 速度乗数 L												*/
	ULONG	m3_ratTrqL;				/* PnA0E : 定格トルク L												*/
/*------------------------------------------------------------------------------------------------------*/
	ULONG	m3_permitTrqL;			/* PnA10 : 最大出力可能トルク L										*/
	ULONG	m3_trqMultiplierL;		/* PnA12 : トルク乗数 L												*/
	ULONG	m3_reslutionL;			/* PnA14 : 分解能(ロータリ) L										*/
	ULONG	m3_scalepitchL;			/* PnA16 : リニアスケールピッチ L									*/
	ULONG	m3_scalepitchH;			/* PnA17 : リニアスケールピッチ H									*/
	ULONG	m3_pulsePerPitchL;		/* PnA18 : スケールピッチ当りのパルス数（リニア） L					*/
/*------------------------------------------------------------------------------------------------------*/
	LONG	m3_ratbL;				/* PnA42 : 電子ギア比（分子）										*/
	LONG	m3_rataL;				/* PnA44 : 電子ギア比（分母）										*/
	LONG	m3_absOffsetL;			/* PnA46 : アブソPG原点位置オフセット L								*/
	ULONG	m3_mtLimitL;			/* PnA48 : マルチターンリミット設定 L								*/
	ULONG	m3_limitSwL;			/* PnA4A : リミット設定 L											*/
	LONG	m3_psLimitL;			/* PnA4C : 正側ソフトリミット値 L									*/
	LONG	m3_reserve1L;			/* 予備	*/
/*------------------------------------------------------------------------------------------------------*/
	LONG	m3_nsLimitL;			/* PnA50 : 負側ソフトリミット値 L									*/
	LONG	m3_reserve2L;			/* 予備	*/
/*------------------------------------------------------------------------------------------------------*/
	ULONG	m3_velUnitL;			/* PnA82 : 速度単位選択 L											*/
	ULONG	m3_velBase4L;			/* PnA84 : 速度基本単位選択 L										*/
	ULONG	m3_posUnitL;			/* PnA86 : 位置単位選択 L											*/
	ULONG	m3_posBaseL;			/* PnA88 : 位置基本単位選択 L										*/
	ULONG	m3_accUnitL;			/* PnA8A : 加速度単位選択 L											*/
	ULONG	m3_accBase4L;			/* PnA8C : 加速度基本単位選択 L										*/
	ULONG	m3_trqUnitL;			/* PnA8E : トルク単位選択 L											*/
/*------------------------------------------------------------------------------------------------------*/
	ULONG	m3_trqBase4L;			/* PnA90 : トルク基本単位選択 L										*/
	ULONG	m3_supportUnitL;		/* PnA92 : 対応単位系 L												*/
/*------------------------------------------------------------------------------------------------------*/
	ULONG	m3_KvL;					/* PnAC2 : 速度ループゲイン L										*/
	ULONG	m3_TviL;				/* PnAC4 : 速度ループ積分時定数 L									*/
	ULONG	m3_KpL;					/* PnAC6 : 位置ループゲイン L										*/
	ULONG	m3_FFGainL;				/* PnAC8 : フィードフォワード補償 L									*/
	ULONG	m3_TpiL;				/* PnACA : 位置ループ積分時定数 L									*/
	ULONG	m3_psetWidthL;			/* PnACC : 位置決め完了幅 L											*/
	ULONG	m3_nearWidthL;			/* PnACE : 位置決め近傍幅  L										*/
/*------------------------------------------------------------------------------------------------------*/
	ULONG	m3_filExpTbL;			/* PnB02 : 指数関数加減速時定数 L									*/
	ULONG	m3_filMaTbL;			/* PnB04 : 移動平均時間 L											*/
	LONG	m3_exDistL;				/* PnB06 : 外部信号位置決め最終走行距離 L							*/
	ULONG	m3_zretSpd1L;			/* PnB08 : 原点復帰アプローチ速度 L									*/
	ULONG	m3_zretSpd2L;			/* PnB0A : 原点復帰クリープ速度 L									*/
	LONG	m3_zretDistL;			/* PnB0C : 原点復帰最終走行距離 L									*/
	ULONG	m3_cPrmMon1L;			/* PnB0E : 固定モニタ選択１ L										*/
/*------------------------------------------------------------------------------------------------------*/
	ULONG	m3_cPrmMon2L;			/* PnB10 : 固定モニタ選択２ L										*/
	ULONG	m3_selMon1L;			/* PnB12 : SEL_MONのモニタ選択１ L									*/
	ULONG	m3_selMon2L;			/* PnB14 : SEL_MONのモニタ選択２ L									*/
	ULONG	m3_zpWidthL;			/* PnB16 : 原点検出幅 L												*/
	ULONG	m3_pTrqLmtL;			/* PnB18 : 正転トルク制限値 L										*/
	ULONG	m3_nTrqLmtL;			/* PnB1A : 逆転トルク制限値 L										*/
	ULONG	m3_zSpdLvL;				/* PnB1C : ゼロ速度検出幅 L											*/
	ULONG	m3_vCmpLvL;				/* PnB1E : 速度一致信号検出幅 L										*/
/*------------------------------------------------------------------------------------------------------*/
	ULONG	m3_enaCmdFeildL;		/* PnB20 : サーボコマンド制御フィールド内の有効／無効選択 L			*/
	ULONG	m3_enabStatFeildL;		/* PnB22 : サーボステータス制御フィールド内の有効／無効選択 L		*/
	ULONG	m3_enaOutbitL;			/* PnB24 : I/Oビット定義の有効／無効選択（出力側） L				*/
	ULONG	m3_enaInbitL;			/* PnB26 : I/Oビット定義の有効／無効選択（入力側） L				*/
	ULONG	m3_Pn50A;				/* PnB28 : 正側オーバートラベル信号値割付値 L						*/
	ULONG	m3_Pn50B;				/* PnB29 : 負側オーバートラベル信号値割付値 L						*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	almmsk1;				/* PnE05 : アラームマスク１											*/
	USHORT	almmsk2;				/* PnE06 : アラームマスク２											*/
	USHORT 	opdetsw;				/* PnE09 : オプションカード／セーフティ機能対応制限スイッチ			*//* <S04D> */
	USHORT	syssw1;					/* PnE0B : システムスイッチ１										*/
	USHORT	syssw3;					/* PnE0D : システムスイッチ３										*/
	USHORT	comfil;					/* PnE0E : 指令入力フィルタ選択時の指令フィルタ設定					*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	paswrd;					/* PnE10 : パラメータパスワード										*/
	DBYTEX	styp;					/* PnE11 : 下位:サーボパック形式コード, 上位:入力電圧コード			*/
	USHORT	ampw;					/* PnE12 : サーボパック容量											*/
	USHORT	yopt;					/* PnE13 : Ｙ仕様番号												*/
	USHORT	irats;					/* PnE14 : サーボパック定格電流										*/
	USHORT	imaxs;					/* PnE15 : サーボパック最大電流										*/
	DBYTEX	ibs_md;					/* PnE17 : 下位:過負荷検出ベース電流, 上位:過負荷検出中間電流		*/
	DBYTEX	imdt_mxt;				/* PnE18 : 下位:過負荷検出中間電流時間, 上位:過負荷検出最大電流時間	*/
	USHORT	olwrat;					/* PnE19 : ＯＬ警告比												*/
	USHORT	dbr;					/* PnE1B : ＤＢ抵抗値												*/
	USHORT	wdb;					/* PnE1D : ＤＢ抵抗容量(デフォルト)									*/
	DBYTEX	spgfail;				/* PnE1E : 下位:S-PG受信失敗許容回数, 上位:FS-PG受信失敗許容回数	*/
	DBYTEX	PnE1F;					/* PnE1F : 下位:突入抵抗過負荷用電源投入許容回数, 上位:(予備)		*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	ignd;					/* PnE20 : 電流ループゲイン(D)										*/
	USHORT	ignq;					/* PnE21 : 電流ループゲイン(Q)										*/
	USHORT	kid;					/* PnE22 : 電流ループ積分時定数(D)									*/
	USHORT	kiq;					/* PnE23 : 電流ループ積分時定数(Q)									*/
	USHORT	limintd;				/* PnE24 : 電流ループ積分制限値(D)									*/
	USHORT	limintq;				/* PnE25 : 電流ループ積分制限値(Q)									*/
	USHORT	ifil;					/* PnE26 : 電流ループフィルタ時定数									*/
	USHORT	shant;					/* PnE28 : 電流検出ゲイン											*/
	USHORT	vrevgn;					/* PnE2A : 電圧補正ゲイン											*/
	USHORT	lmdata;					/* PnE2B : 電圧制限値												*/
	USHORT	pwmf;					/* PnE2C : ＰＷＭ周波数												*/
	DBYTEX	odt_c;					/* PnE2D : 下位:オンディレイ時間, 上位:オンディレイ補償定数			*/
	DBYTEX	odlv_slp;				/* PnE2E : 下位:オンディレイ補償変更レベル, 上位:補償傾き			*/
	USHORT	dq_sw;					/* PnE2F : ＤＱ用スイッチ											*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	phscmp;					/* PnE34 : 位相補償													*/
	USHORT	kdd;					/* PnE35 : 電流ループ微分時間(D)									*/
	USHORT	kdq;					/* PnE36 : 電流ループ微分時間(Q)									*/
	USHORT	dfild;					/* PnE37 : 電流ループ微分フィルタ時定数(D)							*/
	USHORT	dfilq;					/* PnE38 : 電流ループ微分フィルタ時定数(Q)							*/
	USHORT	dq_sw2;					/* PnE39 : 電流制御スイッチ２										*/
	USHORT	motresq;				/* PnE3B : モータ共振抑制用ノッチフィルタQ値						*/
	USHORT	PnE3C;					/* PnE3C : N側スイッチON最小時間									*/
	USHORT	PnE3D;					/* PnE3D : サンプル＆ホールド時間									*/
	USHORT	fltrange;				/* PnE3F : モータ過電流検出フィルタ回数								*/ /* <S1F5> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	tplsfil;				/* PnE40 : DRV内部信号異常検出フィルタ回数							*//* <S1F6> */
//	USHORT	mnrng0;					/* PnE42 : モニタ１ゲイン											*//* <S016> *//* <S0DF> */
//	USHORT	mnrng1;					/* PnE43 : モニタ２ゲイン											*//* <S016> *//* <S0DF> */
	DBYTEX	ipg_rwt;				/* PnE44 : 下位:i-P制御割合, 上位:暴走検出トルク					*/
	USHORT	PnE45;					/* PnE45 : 低速ＯＬ：ベース電流[%]，ＯＬ特性変曲点[電気角10r/min]	*/
	USHORT	PnE46;					/* PnE46 : 低速ＯＬ：中間電流[10%]，最大電流[10%]					*/
	USHORT	PnE47;					/* PnE47 : 低速ＯＬ：中間電流時間[0.1s]，最大電流時間[0.1s]			*/
//	ULONG /*USHORT*/	mnptr0l;	/* PnE48 : <S074> モニタ１アドレス									*//* <S0DF> */
//	ULONG /*USHORT*/	mnptr1l;	/* PnE4A : <S074> モニタ２アドレス									*//* <S0DF> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	zadjiu;					/* PnE50 : 電流検出ゼロ調(U相)										*/
	USHORT	zadjiv;					/* PnE51 : 電流検出ゼロ調(V相)										*/
	DBYTEX	gadjiu_v;				/* PnE52 : 電流検出ゲイン調(U,V相)									*/
//	USHORT	zadjvl;					/* PnE53 : 速度指令ゼロ調											*/	/* <S0A9> データ型変更 */
	LONG	zadjvl;					/* PnE53 : 速度指令ゼロ調											*/	/* <S0A9> データ型変更 */
	USHORT	zadjvh;					/* PnE54 : 予約定数(扱わないでください)								*/
	USHORT	gadjv;					/* PnE55 : 速度指令ゲイン調											*/
	DBYTEX	zadjtp_n;				/* PnE56 : トルク指令ゼロ調(正入力/負入力)							*/
	DBYTEX	gadjtp_n;				/* PnE57 : トルク指令ゲイン調(正入力/負入力)						*/
/*------------------------------------------------------------------------------------------------------*/
	DBYTEX	ratbt_mt;				/* PnE60 : 下位:過負荷検出ベース電流, 上位:過負荷検出中間電流		*/
	DBYTEX	ratmtt_2;				/* PnE61 : 下位:過負荷検出中間時間, 上位:過負荷検出中間電流２		*/
	DBYTEX	ratmt2_os;				/* PnE62 : 下位:過負荷検出中間時間２, 上位:ＯＳ検出レベル			*/
	USHORT	PnE63;					/* PnE63 : アラームトレース機能選択スイッチ							*/
	USHORT	PnE64;					/* PnE64 : アラームトレース機能サンプリング間隔						*/
	USHORT	PnE65;					/* PnE65 : アラームトレース機能トレース１対象選択					*/
	USHORT	PnE66;					/* PnE66 : アラームトレース機能トレース２対象選択					*/
	USHORT	PnE67;					/* PnE67 : アラームトレース機能トレース３対象選択					*/
	USHORT	PnE68;					/* PnE68 : アラームトレース機能トレース１実アドレス指定				*/
	USHORT	PnE6A;					/* PnE6A : アラームトレース機能トレース２実アドレス指定				*/
	USHORT	PnE6C;					/* PnE6C : アラームトレース機能トレース３実アドレス指定				*/
	/* <S192> Start */
/*------------------------------------------------------------------------------------------------------*/
	USHORT DBLifeTime;				/* PnE83 :  DB回路寿命							*/
	ULONG DBSwichCount;				/* PnE86 : 	現在のDB動作回数				*//* <S192> */
	/* <S192> End */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	scnvorgoffset;			/* PnE8C : シリアル変換ユニット原点オフセット						*/
	USHORT	phsofst;				/* PnE8D : 電流零位相位置											*/
/*------------------------------------------------------------------------------------------------------*/
	DBYTEX	olspdlvl;				/* PnE90 : 低速過負荷検出速度レベル									*/
	DBYTEX	ollspdprm;				/* PnE91 : 低速過負荷検出パラメータ									*/
	USHORT	dbol_warn;				/* PnE93 : ＤＢ過負荷検出電力１										*/
	USHORT	dbol_alm;				/* PnE94 : ＤＢ過負荷検出電力２										*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	ifil2;					/* PnEA4 : 静音モード電流ループフィルタ時定数						*/
	USHORT	pwmf2;					/* PnEA5 : 静音モードPWM周波数										*/
	USHORT	ilimlvl;				/* PnEA6 : キャリア周波数切り替え電流閾値							*/
	USHORT	pwmf3;					/* PnEA7 : 第2PWM周波数												*/
	USHORT	fbfil2;					/* PnEA8 : 第２速度Ｆ/Ｂフィルタ時定数								*/
	USHORT	kv;						/* PnEB0 : 電圧フィードバックループ比例ゲイン						*/
	USHORT	tv;						/* PnEB1 : 電圧フィードバック積分時定数								*/
	USHORT	idlim;					/* PnEB2 : 電圧フィードバック積分リミット							*/
	USHORT	v1max;					/* PnEB3 : 電圧指令制限レベル										*/
	USHORT	vmaxid;					/* PnEB4 : d軸電流指令リミット用電圧制限レベル						*/
	USHORT	vpnavg;					/* PnEB5 : 主回路電圧平均時間										*/
/*------------------------------------------------------------------------------------------------------*/
	USHORT	PnED0;					/* PnED0 : 自動ノッチ完了状態										*/
	ULONG	PnED1;					/* PnED1 : 自動ノッチ最大振動値										*/
	USHORT	PnED3;					/* PnED3 : ノッチ周波数１回目補正係数１								*/
	USHORT	PnED4;					/* PnED4 : ノッチ周波数１回目補正係数２								*/
	USHORT	PnED5;					/* PnED5 : ノッチ周波数１回目補正範囲閾値							*/
	USHORT	PnED6;					/* PnED6 : ノッチ周波数２回目補正係数１								*/
	USHORT	PnED7;					/* PnED7 : ノッチ周波数２回目補正係数２								*/
	USHORT	PnED8;					/* PnED8 : ノッチ周波数２回目補正範囲閾値							*/
	USHORT	PnED9;					/* PnED9 : 自動ノッチ振動チェック時の停止判別待ち時間				*/
	USHORT	TunLessGain;			/* PnEDA : 調整ﾚｽ外乱ｵﾌﾞｻﾞｰﾊﾞｹﾞｲﾝ									*/
	USHORT	TunLessStiff;			/* PnEDB : 調整ﾚｽｲﾅｰｼｬ補正ｹﾞｲﾝ										*/
	USHORT	TunLessTrqLpf;			/* PnEDC : 調整ﾚｽ外乱ﾄﾙｸﾛｰﾊﾟｽﾌｨﾙﾀ時定数								*/
	USHORT	TunLessVfbLpf;			/* PnEDD : 調整ﾚｽ速度FBﾛｰﾊﾟｽﾌｨﾙﾀ時定数								*/
/*------------------------------------------------------------------------------------------------------*/
	DBYTEX	MLess_vrat_max;			/* PnEE0 : モータレステスト機能仮想モータ速度						*//* <S1B7> */
	DBYTEX	MLess_oslv_pol;			/* PnEE1 : モータレステスト機能仮想モータＯＳ検出レベル, ポール数	*//* <S1B7> */
	USHORT	Mless_jmot;				/* PnEE2 : モータレステスト機能仮想モータロータイナーシャ			*//* <S1B7> */
	DBYTEX	Mless_exp_spd_jmot;		/* PnEE3 : モータレステスト機能仮想モータ指数パラメータ				*//* <S1B7> */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	dq_sw_L;				/* PnEE8 : ＤＱ用スイッチ(リニア)									*/
	USHORT	v1max_L;				/* PnEE9 : 電圧指令制限レベル(リニア)								*/
	USHORT	TunLessGain_L;			/* PnEEA : 調整ﾚｽ外乱ｵﾌﾞｻﾞｰﾊﾞｹﾞｲﾝ(リニア)							*/
	USHORT	TunLessStiff_L;			/* PnEEB : 調整ﾚｽｲﾅｰｼｬ補正ｹﾞｲﾝ(リニア)								*/
/*------------------------------------------------------------------------------------------------------*/
	MENCPRM	MencP;					/* PnF00 - PnF1E : エンコーダパラメータ */
/*------------------------------------------------------------------------------------------------------*/
	USHORT	RippleCompGainE;		/* PnFA0 : 速度リップル補償ゲイン(電気角)							*/	/* <S14B> */
	USHORT	RippleCompCycleE[6];	/* PnFA1, PnFA4, …, PnFB0 : 速度リップル補償成分周期(電気角)		*/	/* <S14B> */
	SHORT	RippleCompAmplitudeE[6];/* PnFA2, PnFA5, …, PnFB1 : 速度リップル補償成分振幅(電気角)		*/	/* <S14B> */
	USHORT	RippleCompPhaseE[6];	/* PnFA3, PnFA6, …, PnFB2 : 速度リップル補償成分位相(電気角)		*/	/* <S14B> */
/*------------------------------------------------------------------------------------------------------*/

} PRMDATA;



#endif //__PN_PRM_STRUCT_H

