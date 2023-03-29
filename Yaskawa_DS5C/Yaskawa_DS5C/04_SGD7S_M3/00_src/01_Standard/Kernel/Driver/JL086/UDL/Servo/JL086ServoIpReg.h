/****************************************************************************************************/
/*																									*/
/*																									*/
/*		JL086ServoIpReg.h : JL-086 SERVOIPレジスタ定義ヘッダファイル								*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1. JL-086 SERVOIPレジスタ定義																*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		2014.02.19 : Made by Excel Macro															*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#ifndef _JL086_SERVOIP_REG_H
#define _JL086_SERVOIP_REG_H



/****************************************************************************************************/
/*																									*/
/*		ASIC JL-086 SERVOIP H/Wレジスタ構造体定義													*/
/*																									*/
/****************************************************************************************************/
typedef struct _ASIC_HW_IF_STR {
	VULONG	AREG_PWMU;				/* 0x0000 アナログモニタPWM出力U								*/
	VULONG	AREG_PWMV;				/* 0x0004 アナログモニタPWM出力V								*/
	VULONG	AREG_IOSTAT;			/* 0x0008 汎用同期シリアル出力＆送信バッファ転送				*/
	VULONG	SPARE0;					/* 0x000C Reserve												*/
	VULONG	AREG_ITDO;				/* 0x0010 初期化時用タイマ(RD)									*/
	VULONG	AREG_TIM1SET;			/* 0x0014 タイマ1設定レジスタ									*/
	VULONG	AREG_UARTSTS;			/* 0x0018 UARTステータス										*/
	VULONG	AREG_UARTXD;			/* 0x001C 汎用非同期ポート送信データ(WR)＆受信データ(RD)		*/
	VULONG	AREG_DIVCSEV;			/* 0x0020 エンコーダパルス出力分周比設定						*/
	VULONG	SPARE1[4];				/* 0x0024-0x0030 Reserve										*/
	VULONG	AREG_INT1SET;			/* 0x0034 INT1割込み設定&アクノリッジ							*/
	VULONG	AREG_PFNCSET;			/* 0x0038 端子機能設定											*/
	VULONG	SPARE2;					/* 0x003C Reserve												*/
/*--------------------------------------------------------------------------------------------------*/
	VULONG	AREG_FBCSET;			/* 0x0040 分周入力フィルタ設定									*/
	VULONG	AREG_POCSET;			/* 0x0044 指令カウンタ設定										*/
	VULONG	AREG_DINCSET;			/* 0x0048 分周機能設定											*/
	VULONG	AREG_LODSET;			/* 0x004C ロード設定											*/
	VULONG	AREG_T_FBLD;			/* 0x0050 FBカウンタ設定										*/
	VULONG	AREG_T_CMDLD;			/* 0x0054 CMDカウンタ設定										*/
	VULONG	AREG_ALMCLR;			/* 0x0058 アラームラッチリセット								*/
	VULONG	AREG_ALMSET;			/* 0x005C アラーム設定											*/
	VULONG	AREG_IOSTATX;			/* 0x0060 汎用入力												*/
	VULONG	AREG_FLTSTAT;			/* 0x0064 故障入力												*/
	VULONG	SPARE3;					/* 0x0068 Reserve												*/
	VULONG	AREG_SETREG1;			/* 0x006C 内部設定レジスタ										*/
	VULONG	AREG_SETREG2;			/* 0x0070 機能選択レジスタ										*/
	VULONG	AREG_CLKO;				/* 0x0074 シリアルエンコーダIF伝送速度設定レジスタ				*/
	VULONG	AREG_CSTP;				/* 0x0078 機能停止選択レジスタ("1"で停止)						*/
	VULONG	AREG_SEPGSET00;			/* 0x007C シリアルエンコーダIF設定：CH0設定0					*/
/*--------------------------------------------------------------------------------------------------*/
	VULONG	AREG_SETREG3;			/* 0x0080 回生機能設定レジスタ									*/
	VULONG	AREG_SEPGSET01;			/* 0x0084 シリアルエンコーダIF設定：CH0設定1					*/
	VULONG	AREG_SEPGSET10;			/* 0x0088 シリアルエンコーダIF設定：CH1設定0					*/
	VULONG	AREG_SEPGSET11;			/* 0x008C シリアルエンコーダIF設定：CH1設定1					*/
	VULONG	SPARE4;					/* 0x0090 Reserve												*/
	VULONG	AREG_FCCSET;			/* 0x0094 指令フィルタ設定レジスタ								*/
	VULONG	AREG_T_FLC;				/* 0x0098 フルクローズドカウンタ設定							*/
	VULONG	AREG_ESYNC;				/* 0x009C シリアルエンコーダ同期カウンタ外部同期				*/
	VULONG	AREG_SE0_TXD01;			/* 0x00A0 シリアルエンコーダCH0送信データ0,1					*/
	VULONG	AREG_SE0_TXD23;			/* 0x00A4 シリアルエンコーダCH0送信データ2,3					*/
	VULONG	AREG_SE0_TXD45;			/* 0x00A8 シリアルエンコーダCH0送信データ4,5					*/
	VULONG	AREG_SE0_TXD67;			/* 0x00AC シリアルエンコーダCH0送信データ6,7					*/
	VULONG	AREG_SE0_RXD01;			/* 0x00B0 シリアルエンコーダCH0受信データ0,1					*/
	VULONG	AREG_SE0_RXD23;			/* 0x00B4 シリアルエンコーダCH0受信データ2,3					*/
	VULONG	AREG_SE0_RXD45;			/* 0x00B8 シリアルエンコーダCH0受信データ4,5					*/
	VULONG	AREG_SE0_RXD67;			/* 0x00BC シリアルエンコーダCH0受信データ6,7					*/
/*--------------------------------------------------------------------------------------------------*/
	VULONG	SPARE5[8];				/* 0x00C0-0x00DC Reserve										*/
	VULONG	AREG_GSSRD;				/* 0x00E0 汎用同期シリアル受信データ							*/
	VULONG	SPARE6;					/* 0x00E4 Reserve												*/
	VULONG	AREG_CMDCLML;			/* 0x00E8 指令カウンタCMLラッチ値								*/
	VULONG	AREG_SPRGCMD0;			/* 0x00EC SENC：CH0コマンド/レスポンスバッファ					*/
	VULONG	AREG_CDLTD;				/* 0x00F0 シリアルエンコーダCH0カウンタラッチ値					*/
	VULONG	AREG_CMDLDL;			/* 0x00F4 指令カウンタロードラッチ値							*/
	VULONG	AREG_SPRGCMD1;			/* 0x00F8 SENC：CH1コマンド/レスポンスバッファ					*/
	VULONG	AREG_SEDLLD;			/* 0x00FC シリアルエンコーダ遅延同期ロード値					*/
/*--------------------------------------------------------------------------------------------------*/
	VULONG	SPARE7[2];				/* 0x0100-0x0104 Reserve										*/
	VULONG	AREG_TSTEELWL;			/* 0x0108 EEPROM IF Lowワード値(テスト用)						*/
	VULONG	AREG_TSTDCCNT;			/* 0x010C 分周DDA現在値											*/
	VULONG	AREG_TSTT1CNT;			/* 0x0110 タイマT1値(テスト用)									*/
	VULONG	SPARE8;					/* 0x0114 Reserve												*/
	VULONG	AREG_TSTPWMUCNT;		/* 0x0118 PWMUカウンタ下位8ビット値(テスト用)					*/
	VULONG	AREG_TSTPWMVCNT;		/* 0x011C PWMVカウンタ下位8ビット値(テスト用)					*/
	VULONG	AREG_SDMCONF1;			/* 0x0120 デシメーションフィルタ設定							*/
	VULONG	AREG_SDMCONF2;			/* 0x0124 オーバレンジ検出マスクビット設定、試験設定			*/
	VULONG	AREG_PWMCCD;			/* 0x0128 PWMキャリアカウンタ現在値								*/
	VULONG	SPARE9;					/* 0x012C Reserve												*/
	VULONG	AREG_DYSETUP;			/* 0x0130 U相P側オンディレイ時間設定							*/
	VULONG	AREG_DYSETUN;			/* 0x0134 U相N側オンディレイ時間設定							*/
	VULONG	AREG_DYSETVP;			/* 0x0138 V相P側オンディレイ時間設定							*/
	VULONG	AREG_DYSETVN;			/* 0x013C V相N側オンディレイ時間設定							*/
/*--------------------------------------------------------------------------------------------------*/
	VULONG	AREG_DYSETWP;			/* 0x0140 W相P側オンディレイ時間設定							*/
	VULONG	AREG_DYSETWN;			/* 0x0144 W相N側オンディレイ時間設定							*/
	VULONG	SPARE10[2];				/* 0x0148-0x014C Reserve										*/
	VULONG	AREG_ZDR;				/* 0x0150 パルス変換原点補正設定								*/
	VULONG	AREG_ZENBR;				/* 0x0154 原点通過ビット位置設定								*/
	VULONG	AREG_DIVHSR;			/* 0x0158 分周回路ヒステリシス設定								*/
	VULONG	SPARE11;				/* 0x015C Reserve												*/
	VULONG	AREG_PUMPON;			/* 0x0160 チャージポンプ設定									*/
	VULONG	AREG_DIVSET;			/* 0x0164 分周機能設定											*/
	VULONG	AREG_PLSET;				/* 0x0168 パルス変換設定										*/
	VULONG	AREG_WDT1SET;			/* 0x016C 演算部用WDT設定										*/
	VULONG	AREG_POSET0;			/* 0x0170 シリアルPG-パルス変換位置設定							*/
	VULONG	AREG_POSET1;			/* 0x0174 シリアルPG-パルス変換原点補正1						*/
	VULONG	AREG_POSET2;			/* 0x0178 シリアルPG-パルス変換原点補正2						*/
	VULONG	AREG_POTEST;			/* 0x017C シリアルPG-パルス変換設定L(テスト用)					*/
/*--------------------------------------------------------------------------------------------------*/
	VULONG	AREG_POSET0H;			/* 0x0180 シリアルPG-パルス変換位置設定bit16-23					*/
	VULONG	SPARE12;				/* 0x0184 Reserve												*/
	VULONG	AREG_POSET12H;			/* 0x0188 シリアルPG-パルス変換原点補正H						*/
	VULONG	AREG_WDT2SET;			/* 0x018C ホスト用WDT設定										*/
	VULONG	AREG_DCTRIM;			/* 0x0190 分周出力原点補正設定(Bit0-15)							*/
	VULONG	AREG_ADSET1;			/* 0x0194 AD設定												*/
	VULONG	AREG_ADSET2;			/* 0x0198 指令AD設定：基本クロック								*/
	VULONG	AREG_ADSET3;			/* 0x019C 指令AD設定：IFサイクル								*/
	VULONG	AREG_ADSET4;			/* 0x01A0 指令AD設定：ラッチタイミング							*/
	VULONG	AREG_ADSET5;			/* 0x01A4 指令AD設定：コントロールCH0							*/
	VULONG	AREG_ADSET6;			/* 0x01A8 指令AD設定：コントロールCH1							*/
	VULONG	AREG_ADSYNC;			/* 0x01AC 指令ADサイクル同期									*/
	VULONG	AREG_ADFCNT1;			/* 0x01B0 電流AD設定：基本クロック								*/
	VULONG	AREG_ADFCNT2;			/* 0x01B4 電流AD設定：IFサイクル								*/
	VULONG	AREG_ADFCNT3;			/* 0x01B8 電流AD設定：ラッチタイミング							*/
	VULONG	AREG_ADFCNT4;			/* 0x01BC 電流AD設定：コントロールCH0							*/
/*--------------------------------------------------------------------------------------------------*/
	VULONG	AREG_ADFCNT5;			/* 0x01C0 電流AD設定：コントロールCH1							*/
	VULONG	AREG_ADFSYNC;			/* 0x01C4 電流ADサイクル同期									*/
	VULONG	AREG_INT0SET;			/* 0x01C8 INT0割込み設定										*/
	VULONG	AREG_ACULH;				/* 0x01CC 指令AD：CH0ラッチデータ(Bit16-23)						*/
	VULONG	AREG_INT0TRG;			/* 0x01D0 INT0割込み同期ポート/カウンタ値読出し					*/
	VULONG	AREG_DCNTV;				/* 0x01D4 分周カウンタ設定値(Bit0-15)							*/
	VULONG	AREG_ACUL;				/* 0x01D8 指令AD：CH0ラッチデータ								*/
	VULONG	AREG_ACVL;				/* 0x01DC 指令AD：CH1ラッチデータ(Bit0-15)						*/
	VULONG	AREG_AFUL;				/* 0x01E0 電流AD：U相電流ラッチデータ							*/
	VULONG	AREG_AFVL;				/* 0x01E4 電流AD：V相電流ラッチデータ							*/
	VULONG	AREG_CMDAD1;			/* 0x01E8 指令ADデータ1(テスト用)								*/
	VULONG	AREG_CMDAD2;			/* 0x01EC 指令ADデータ2(テスト用)								*/
	VULONG	AREG_CURAD1;			/* 0x01F0 電流ADデータ1(テスト用)								*/
	VULONG	AREG_CURAD2;			/* 0x01F4 電流ADデータ2(テスト用)								*/
	VULONG	AREG_DDACT;				/* 0x01F8 シリアルPG-パルス変換DDA値(テスト用)					*/
	VULONG	AREG_ACVLH;				/* 0x01FC 指令AD：CH1ラッチデータ(Bit16-23)						*/
/*--------------------------------------------------------------------------------------------------*/
	VULONG	AREG_STIMLT5;			/* 0x0200 ラッチタイマラッチ値1d								*/
	VULONG	AREG_STIMLT6;			/* 0x0204 ラッチタイマラッチ値2d								*/
	VULONG	AREG_STIMLT7;			/* 0x0208 ラッチタイマラッチ値3d								*/
	VULONG	AREG_STIMLT8;			/* 0x020C エンコーダ読込み時のタイマラッチ値					*/
	VULONG	AREG_SFBLT0;			/* 0x0210 CA端子立ち上がり時のフィードバック位置				*/
	VULONG	AREG_SFBLT1;			/* 0x0214 CA端子立ち下がり時のフィードバック位置				*/
	VULONG	AREG_SFBLT2;			/* 0x0218 CB端子立ち上がり時のフィードバック位置				*/
	VULONG	AREG_SFBLT3;			/* 0x021C CB端子立ち下がり時のフィードバック位置				*/
	VULONG	AREG_SFBLT4;			/* 0x0220 CC端子立ち上がり時のフィードバック位置				*/
	VULONG	AREG_SFBLT5;			/* 0x0224 CC端子立ち下がり時のフィードバック位置				*/
	VULONG	SPARE13;				/* 0x0228 Reserve												*/
	VULONG	AREG_OCDCONF1;			/* 0x022C OC検出デシメーションフィルタ動作モード設定			*/
	VULONG	AREG_OCLVUP;			/* 0x0230 U相上限OC検出レベル									*/
	VULONG	AREG_TIM;				/* 0x0234 ラッチタイマ現在値									*/
	VULONG	AREG_OCLVUM;			/* 0x0238 U相下限OC検出レベル									*/
	VULONG	AREG_OCLVVP;			/* 0x023C V相上限OC検出レベル									*/
/*--------------------------------------------------------------------------------------------------*/
	VULONG	AREG_NCTCSET;			/* 0x0240 指令用NCT設定											*/
	VULONG	AREG_AFULH;				/* 0x0244 電流AD：CH0ラッチデータ(Bit16-23)						*/
	VULONG	AREG_AFVLH;				/* 0x0248 電流AD：CH1ラッチデータ(Bit16-23)						*/
	VULONG	AREG_ACMC4;				/* 0x024C 指令マルチプレクサチャネル4							*/
	VULONG	AREG_NCTCTM;			/* 0x0250 指令用NCT：タイマカウンタ値							*/
	VULONG	AREG_NCTCFB;			/* 0x0254 指令用NCT：FBカウンタ値								*/
	VULONG	AREG_NCTCCD;			/* 0x0258 指令用NCT：CDカウンタ値								*/
	VULONG	AREG_NCTCTP;			/* 0x025C 指令用NCT：TPカウンタ値								*/
	VULONG	AREG_POTESTH;			/* 0x0260 シリアルPG-パルス変換設定H(テスト用)					*/
	VULONG	AREG_HINT1;				/* 0x0264 演算部割込みポート1									*/
	VULONG	AREG_HINT2;				/* 0x0268 演算部割込みポート2									*/
	VULONG	AREG_ACMC5;				/* 0x026C 指令マルチプレクサチャネル5							*/
	VULONG	AREG_SRESET;			/* 0x0270 ソフトリセット										*/
	VULONG	AREG_NCTRSTL;			/* 0x0274 NCTリセット											*/
	VULONG	AREG_ACMC6;				/* 0x0278 指令マルチプレクサチャネル6							*/
	VULONG	AREG_ACMC7;				/* 0x027C 指令マルチプレクサチャネル7							*/
/*--------------------------------------------------------------------------------------------------*/
	VULONG	AREG_SE1_TXD01;			/* 0x0280 シリアルエンコーダCH1送信データ0,1					*/
	VULONG	AREG_SE1_TXD23;			/* 0x0284 シリアルエンコーダCH1送信データ2,3					*/
	VULONG	AREG_SE1_TXD45;			/* 0x0288 シリアルエンコーダCH1送信データ4,5					*/
	VULONG	AREG_SE1_TXD67;			/* 0x028C シリアルエンコーダCH1送信データ6,7					*/
	VULONG	AREG_SE1_RXD01;			/* 0x0290 シリアルエンコーダCH1受信データ0,1					*/
	VULONG	AREG_SE1_RXD23;			/* 0x0294 シリアルエンコーダCH1受信データ2,3					*/
	VULONG	AREG_SE1_RXD45;			/* 0x0298 シリアルエンコーダCH1受信データ4,5					*/
	VULONG	AREG_SE1_RXD67;			/* 0x029C シリアルエンコーダCH1受信データ6,7					*/
	VULONG	AREG_OCSDMCONF3;		/* 0x02A0 OC検出設定3											*/
	VULONG	AREG_OCSDMCONF4;		/* 0x02A4 OC検出設定4											*/
	VULONG	AREG_OCVADSYNC;			/* 0x02A8 OC検出用ADサイクル同期（データ任意）					*/
	VULONG	AREG_OCADU;				/* 0x02AC OC検出用U相ADデータ									*/
	VULONG	AREG_OCADV;				/* 0x02B0 OC検出用V相ADデータ									*/
	VULONG	AREG_OCADW;				/* 0x02B4 OC検出用W相ADデータ									*/
	VULONG	SPARE14[2];				/* 0x02B8-0x02BC Reserve										*/
/*--------------------------------------------------------------------------------------------------*/
	VULONG	AREG_STIMLT0;			/* 0x02C0 ラッチタイマラッチ値0									*/
	VULONG	AREG_STIMLT1;			/* 0x02C4 ラッチタイマラッチ値1									*/
	VULONG	AREG_STIMLT2;			/* 0x02C8 ラッチタイマラッチ値2									*/
	VULONG	AREG_STIMLT3;			/* 0x02CC ラッチタイマラッチ値3									*/
	VULONG	AREG_STIMLT4;			/* 0x02D0 ラッチタイマラッチ値4									*/
	VULONG	AREG_STIMSTAT;			/* 0x02D4 ラッチタイマ設定										*/
	VULONG	AREG_ENCOFS;			/* 0x02D8 フィードバック位置補正値(符号付)						*/
	VULONG	AREG_STCLRLT;			/* 0x02DC ラッチステータスのクリア条件							*/
	VULONG	AREG_SADAT_A;			/* 0x02E0 エンコーダADデータA									*/
	VULONG	AREG_SADAT_AA;			/* 0x02E4 エンコーダADデータAA									*/
	VULONG	AREG_SADAT_B;			/* 0x02E8 エンコーダADデータB									*/
	VULONG	AREG_SADAT_BB;			/* 0x02EC エンコーダADデータBB									*/
	VULONG	AREG_EIFSET1;			/* 0x02F0 エンコーダADIF設定1									*/
	VULONG	AREG_EIFSET2;			/* 0x02F4 エンコーダADIF設定2									*/
	VULONG	AREG_SRCNT0;			/* 0x02F8 シリアルエンコーダIF0伝送周期設定						*/
	VULONG	AREG_SRCNT1;			/* 0x02FC シリアルエンコーダIF1伝送周期設定						*/
/*--------------------------------------------------------------------------------------------------*/
	VULONG	AREG_FBZRND;			/* 0x0300 フィードバックカウンタZRNラッチデータ					*/
	VULONG	AREG_FBCLT;				/* 0x0304 フィードバックカウンタロードラッチデータ				*/
	VULONG	AREG_FBPUD;				/* 0x0308 フィードバックカウンタPUラッチデータ					*/
	VULONG	AREG_FBPCD;				/* 0x030C フィードバックカウンタPCラッチデータ					*/
	VULONG	AREG_TCNT;				/* 0x0310 フィードバックタイマ現在値							*/
	VULONG	AREG_T1LT;				/* 0x0314 フィードバックタイマA/Bパルスラッチ値					*/
	VULONG	AREG_T2LT;				/* 0x0318 フィードバックタイマキャリアラッチ値					*/
	VULONG	AREG_T3LT;				/* 0x031C フィードバックタイマT1直前値ラッチ値					*/
	VULONG	AREG_FCCFZRNL;			/* 0x0320 フルクローズドカウンタZRNラッチデータ					*/
	VULONG	AREG_FCLDL;				/* 0x0324 フルクローズドカウンタロードラッチデータ				*/
	VULONG	AREG_DCTRIMH;			/* 0x0328 分周出力原点補正設定(Bit16-23)						*/
	VULONG	AREG_DCNTVH;			/* 0x032C 分周カウンタ設定値(Bit16-23)							*/
	VULONG	AREG_OCLVVM;			/* 0x0330 V相負値OC検出レベル									*/
	VULONG	AREG_OCLVWP;			/* 0x0334 W相正値OC検出レベル									*/
	VULONG	AREG_OCLVWM;			/* 0x0338 W相負値OC検出レベル									*/
	VULONG	AREG_FLTRANGE;			/* 0x033C OCアラームフィルタ回数設定							*/
/*--------------------------------------------------------------------------------------------------*/
	VULONG	AREG_SDMCONF3;			/* 0x0340 デシメーションフィルタ設定3							*/
	VULONG	AREG_SDMCONF4;			/* 0x0344 デシメーションフィルタ設定4							*/
	VULONG	AREG_VADSYNC;			/* 0x0348 電圧検出用ADサイクル同期(レジスタ書き込み同期)		*/
	VULONG	AREG_AVL;				/* 0x034C 電圧検出用ADラッチデータ								*/
	VULONG	AREG_VAD;				/* 0x0350 電圧検出用ADデータ									*/
	VULONG	SPARE15;				/* 0x0354 Reserve												*/
	VULONG	AREG_SETMSK;			/* 0x0358 マスク設定およびO2CDAT								*/
	VULONG	AREG_DMASET;			/* 0x035C DMA設定												*/
	VULONG	AREG_ENCTXDLY0;			/* 0x0360 エンコーダch0送信開始遅延時間設定						*/
	VULONG	AREG_ENCTXDLY1;			/* 0x0364 エンコーダch1送信開始遅延時間設定						*/
	VULONG	AREG_ENCRDDLY0;			/* 0x0368 エンコーダ位置補正開始ch0遅延時間設定					*/
	VULONG	AREG_ENCRDDLY1;			/* 0x036C エンコーダ位置補正開始ch1遅延時間設定					*/
	VULONG	AREG_INTASET;			/* 0x0370 ScanA割込み設定										*/
	VULONG	AREG_INTBSET;			/* 0x0374 ScanB割込み設定										*/
	VULONG	AREG_INTCSET;			/* 0x0378 ScanC割込み設定										*/
	VULONG	AREG_INTNSET;			/* 0x037C ScanN割込み設定										*/
/*--------------------------------------------------------------------------------------------------*/
	VULONG	AREG_INTCYCLE;			/* 0x0380 割込み周期設定										*/
	VULONG	AREG_INTMPESET;			/* 0x0384 マイクロエンコーダ処理割込み設定						*/
	VULONG	AREG_INTSET;			/* 0x0388 割込み設定											*/
	VULONG	SPARE16;				/* 0x038C Reserve												*/
	VULONG	AREG_ENCFLAG;			/* 0x0390 エンコーダ補正設定およびアラーム						*/
	VULONG	AREG_PGDCAL;			/* 0x0394 位置データ関連設定									*/
	VULONG	AREG_PULSENO;			/* 0x0398 1回転エンコーダパルス数設定							*/
	VULONG	SPARE17;				/* 0x039C Reserve												*/
	VULONG	AREG_ACCERRLVL;			/* 0x03A0 加速度エラーレベル設定								*/
	VULONG	SPARE18;				/* 0x03A4 Reserve												*/
	VULONG	AREG_SPGFAIL;			/* 0x03A8 累計エラーレベル設定									*/
	VULONG	AREG_LCHCLKNUM;			/* 0x03AC ラッチ周期中クロック数設定							*/
	VULONG	AREG_SACLKNUM;			/* 0x03B0 演算周期中クロック数設定								*/
	VULONG	AREG_BASTIM;			/* 0x03B4 ベースタイマ値ロードラッチデータ						*/
	VULONG	AREG_LASTBASTIM;		/* 0x03B8 前回ベースタイマ値ロードラッチデータ					*/
	VULONG	AREG_HENKAITITIM;		/* 0x03BC 変化位置タイマ値ロードラッチデータ					*/
/*--------------------------------------------------------------------------------------------------*/
	VULONG	SPARE19;				/* 0x03C0 Reserve												*/
	VULONG	AREG_COMERRCNT;			/* 0x03C4 通信エラーカウンタ値ロードラッチデータ				*/
	VULONG	AREG_ACCERRCNT;			/* 0x03C8 加速度エラーカウンタ値ロードラッチデータ				*/
	VULONG	AREG_RSTSET;			/* 0x03CC リセット/セット設定									*/
	VULONG	AREG_SPGRXF;			/* 0x03D0 受信フラグ設定										*/
	VULONG	AREG_SPGD12;			/* 0x03D4 エンコーダ受信データ設定								*/
	VULONG	AREG_SPGD34;			/* 0x03D8 エンコーダ受信データ設定								*/
	VULONG	AREG_SPGD5;				/* 0x03DC エンコーダ受信データ設定								*/
	VULONG	AREG_CURMOTPOS;			/* 0x03E0 補正後位置データ レジスタラッチデータ					*/
	VULONG	AREG_LSTMOTPOS;			/* 0x03E4 前回の補正後位置データ レジスタラッチデータ			*/
	VULONG	AREG_CURRCVPOS;			/* 0x03E8 補正無し位置データ レジスタラッチデータ				*/
	VULONG	AREG_LSTRCVPOS;			/* 0x03EC 前回の補正無し位置データ レジスタラッチデータ			*/
	VULONG	AREG_CURRXPOS0_L;		/* 0x03F0 受信データ レジスタラッチデータ Lo					*/
	VULONG	AREG_CURRXPOS0_H;		/* 0x03F4 受信データ レジスタラッチデータ Hi					*/
	VULONG	AREG_LSTRXPOS0_L;		/* 0x03F8 前回の受信データ レジスタラッチデータ Lo				*/
	VULONG	AREG_LSTRXPOS0_H;		/* 0x03FC 前回の受信データ レジスタラッチデータ Hi				*/
/*--------------------------------------------------------------------------------------------------*/
	VULONG	MREG_UPLDA;				/* 0x0400 ASIC MicroPrg Load Pointer							*/
	VULONG	MREG_UPLD;				/* 0x0404 ASIC MicroPrg Load Buffer								*/
	VULONG	MREG_CTSTW;				/* 0x0408 制御status(READ-ONLY)									*/
	VULONG	MREG_CTSTR;				/* 0x040C 制御cmdport(WRITE-ONLY)								*/
	VULONG	MREG_HSUR;				/* 0x0410 ホスト指令ポート(WRITE-ONLY)							*/
} ASIC_HW_IF_STR;
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		ASIC JL-086 SERVOIP μプログラムレジスタ構造体定義											*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Axis information register																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _AXIS_INFO {
	LONG	AxisNum;			/* 0xC0007000 : 制御軸数											*/
} AXIS_INFO;

/*--------------------------------------------------------------------------------------------------*/
/*		32 bit registers																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _ACRV {
	DWREG	IdIntgl;			/* 0xC0007004 : ACRd Integral										*/
	DWREG	IqIntgl;			/* 0xC0007008 : ACRq Integral										*/
	DWREG	VdFil;				/* 0xC000700C : ACRd Filter Output									*/
	DWREG	VqFil;				/* 0xC0007010 : ACRq Filter Output									*/
} ACRV;

/*--------------------------------------------------------------------------------------------------*/
/*		16 bit registers																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _STSFLG {
	SHORT	BbSetW;				/* 0xC0007014 : soft BB set(BBSET)									*/
	SHORT	FltStsW;			/* 0xC0007016 : fault status(FLTIN)									*/
	SHORT	CtrlStsRW;			/* 0xC0007018 : controll flag/status(CTSTR,CTSTW)					*/
	SHORT	CNTSTS;				/* 0xC000701A : counter status(FBCSTS)								*/
	SHORT	FccStsMon;			/* 0xC000701C : full closed counter status(FCCST)					*/
	SHORT	IntglFlg;			/* 0xC000701E : 積分飽和フラグ										*/
} STSFLG;

/*--------------------------------------------------------------------------------------------------*/
/*		voltage FF																					*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _VCMPV {
	SHORT	LdC;				/* 0xC0007020 : Ld  * omega											*/
	SHORT	LqC;				/* 0xC0007022 : Lq  * omega											*/
	SHORT	MagC;				/* 0xC0007024 : Phi * omega											*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	VdOut;				/* 0xC0007026 : Vd output voltage									*/
	SHORT	VqOut;				/* 0xC0007028 : Vq output voltage									*/
	SHORT	VdComp;				/* 0xC000702A : Vd compensation data								*/
	SHORT	VqComp;				/* 0xC000702C : Vq compensation data								*/
	SHORT	VuOut;				/* 0xC000702E : vu output data (to PWM)								*/
	SHORT	VvOut;				/* 0xC0007030 : vv output data (to PWM)								*/
	SHORT	VwOut;				/* 0xC0007032 : vw output data (to PWM)								*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	VdRef;				/* 0xC0007034 : vdref												*/
	SHORT	VqRef;				/* 0xC0007036 : vqref												*/
	SHORT	Vmax2;				/* 0xC0007038 : 電圧指令ベクトル最大値(8192*1.27)					*/
	SHORT	V12;				/* 0xC000703A : 電圧指令ベクトル(√(VdOut^2+VqOut^2))				*/
} VCMPV;

/*--------------------------------------------------------------------------------------------------*/
/*		trigonometric function value																*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _SINTBL {
	SHORT	SinT;				/* 0xC000703C : sin theta											*/
	SHORT	CosT;				/* 0xC000703E : cos theta											*/
	SHORT	SinT2;				/* 0xC0007040 : sin(theta + 2pi/3)									*/
	SHORT	CosT2;				/* 0xC0007042 : cos(theta + 2pi/3)									*/
	SHORT	SinT3;				/* 0xC0007044 : sin(theta - 2pi/3)									*/
	SHORT	CosT3;				/* 0xC0007046 : cos(theta - 2pi/3)									*/
} SINTBL;

/*--------------------------------------------------------------------------------------------------*/
/*		A/D Stop Error detect																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _ADSTOP {
	SHORT	ADRst;				/* 0xC0007048 : 													*/
	SHORT	ADERRCNT;			/* 0xC000704A : ASIC AD Error Counter								*/
	SHORT	IRIUP;				/* 0xC000704C : Previous IRIU										*/
	SHORT	IRIUCNT;			/* 0xC000704E : same IRIU counter									*/
	SHORT	IRIVP;				/* 0xC0007050 : Previous IRIV										*/
	SHORT	IRIVCNT;			/* 0xC0007052 : same IRIV counter									*/
} ADSTOP;

/*--------------------------------------------------------------------------------------------------*/
/*		CPU I/F Data																				*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _ADINV {
	SHORT	IuOffsetIn;			/* 0xC0007054 : A/D transfer offset for iu							*/
	SHORT	IvOffsetIn;			/* 0xC0007056 : A/D transfer offset for iv							*/
	SHORT	KcuIn;				/* 0xC0007058 : A/D transfer gain for iu							*/
	SHORT	KcvIn;				/* 0xC000705A : A/D transfer gain for iv							*/
	SHORT	IdIn;				/* 0xC000705C : Id reference										*/
	SHORT	IqIn;				/* 0xC000705E : Iq reference										*/
	SHORT	PhaseHIn;			/* 0xC0007060 : PHA input											*/
	SHORT	VelIn;				/* 0xC0007062 : VEL input											*/
/*--------------------------------------------------------------------------------------------------*/
/*		voltage compensation																		*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	VdRefIn;			/* 0xC0007064 : vdref input											*/
	SHORT	VqRefIn;			/* 0xC0007066 : vqref input											*/
/*--------------------------------------------------------------------------------------------------*/
/*		Torque Limit																				*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	TLimPIn;			/* 0xC0007068 : +tlim												*/
	SHORT	TLimMIn;			/* 0xC000706A : -tlim												*/
} ADINV;

/*--------------------------------------------------------------------------------------------------*/
/*		static variable																				*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _INTADP {
	SHORT	Kcu;				/* 0xC000706C : A/D transfer gain for iu							*/
	SHORT	Kcv;				/* 0xC000706E : A/D transfer gain for iv							*/
/*--------------------------------------------------------------------------------------------------*/
/*		for over modulation																			*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	Kmod;				/* 0xC0007070 : Over modulation gain/offset							*/
	SHORT	Kvv;				/* 0xC0007072 : AVR													*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	Kc;					/* 0xC0007074 : Current conversion Gain								*/
	SHORT	Ld;					/* 0xC0007076 : d axis Inductance									*/
	SHORT	Lq;					/* 0xC0007078 : q axis Inductance									*/
	SHORT	Mag;				/* 0xC000707A : Magnetic flux (Phi)									*/
	SHORT	KdP;				/* 0xC000707C : d axis propotion gain (PI control)					*/
	SHORT	KqP;				/* 0xC000707E : q axis propotion gain (PI control)					*/
	SHORT	KdI;				/* 0xC0007080 : d axis integral time (gain)							*/
	SHORT	KqI;				/* 0xC0007082 : q axis integral time (gain)							*/
	SHORT	VdLim;				/* 0xC0007084 : d axis integral limit								*/
	SHORT	VqLim;				/* 0xC0007086 : q axis integral limit								*/
	SHORT	KvvIn;				/* 0xC0007088 : Voltage Compasation gain							*/
	SHORT	OnDelayLvl;			/* 0xC000708A : On delay change level								*/
	SHORT	Tfil;				/* 0xC000708C : Filter time constant								*/
	SHORT	Vmax;				/* 0xC000708E : voltage limit data (Vmax^2)							*/
	SHORT	OnDelayComp;		/* 0xC0007090 : On-delay compensation								*/
	SHORT	CtrlSw;				/* 0xC0007092 : Control Flag										*/
	USHORT	CrFreq;				/* 0xC0007094 : carrier freq reference(change while BB)				*/
	SHORT	CtrlSw2;			/* 0xC0007096 : Control Flag2										*/
	SHORT	Dummy0;				/* 0xC0007098 : for Alignment										*/
/*--------------------------------------------------------------------------------------------------*/
/*		notch1																						*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	Kf11;				/* 0xC000709A : 													*/
	SHORT	Kf12;				/* 0xC000709C : 													*/
	SHORT	Kf13;				/* 0xC000709E : 													*/
	SHORT	Kf14;				/* 0xC00070A0 : 													*/
/*--------------------------------------------------------------------------------------------------*/
/*		notch2																						*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	Kf21;				/* 0xC00070A2 : 													*/
	SHORT	Kf22;				/* 0xC00070A4 : 													*/
	SHORT	Kf23;				/* 0xC00070A6 : 													*/
	SHORT	Kf24;				/* 0xC00070A8 : 													*/
/*--------------------------------------------------------------------------------------------------*/
/*		notch3																						*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	Kf31;				/* 0xC00070AA : 													*/
	SHORT	Kf32;				/* 0xC00070AC : 													*/
	SHORT	Kf33;				/* 0xC00070AE : 													*/
	SHORT	Kf34;				/* 0xC00070B0 : 													*/
/*--------------------------------------------------------------------------------------------------*/
/*		notch4																						*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	Kf41;				/* 0xC00070B2 : 													*/
	SHORT	Kf42;				/* 0xC00070B4 : 													*/
	SHORT	Kf43;				/* 0xC00070B6 : 													*/
	SHORT	Kf44;				/* 0xC00070B8 : 													*/
/*--------------------------------------------------------------------------------------------------*/
/*		notch5																						*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	Kf51;				/* 0xC00070BA : 													*/
	SHORT	Kf52;				/* 0xC00070BC : 													*/
	SHORT	Kf53;				/* 0xC00070BE : 													*/
	SHORT	Kf54;				/* 0xC00070C0 : 													*/
/*--------------------------------------------------------------------------------------------------*/
/*		notch6																						*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	Kf61;				/* 0xC00070C2 : 													*/
	SHORT	Kf62;				/* 0xC00070C4 : 													*/
	SHORT	Kf63;				/* 0xC00070C6 : 													*/
	SHORT	Kf64;				/* 0xC00070C8 : 													*/
/*--------------------------------------------------------------------------------------------------*/
/*		LPF																							*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	TLpf;				/* 0xC00070CA : LPF1 gain											*/
	SHORT	TLpf2;				/* 0xC00070CC : LPF2 gain											*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	MotResist;			/* 0xC00070CE : Moter resistance(r1/I_BASE*V_BASE)					*/
	SHORT	OnDelaySlope;		/* 0xC00070D0 : Dead-time comp. gain								*/
	SHORT	L_dIdt;				/* 0xC00070D2 : 													*/
	SHORT	FccRst;				/* 0xC00070D4 : FCCST Reset(for TEST)								*/
	SHORT	Dummy;				/* 0xC00070D6 : for Alignment										*/
} INTADP;

/*--------------------------------------------------------------------------------------------------*/
/*		Current Loop Control Data																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _INTADV {
	SHORT	IuOffset;			/* 0xC00070D8 : A/D transfer offset for iu							*/
	SHORT	IvOffset;			/* 0xC00070DA : A/D transfer offset for iv							*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	IuInData;			/* 0xC00070DC : iu input data (from A/D)							*/
	SHORT	IvInData;			/* 0xC00070DE : iv input data (from A/D)							*/
	SHORT	IdInData;			/* 0xC00070E0 : Id Input											*/
	SHORT	IqInData;			/* 0xC00070E2 : Iq Input											*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	CrFreqW;			/* 0xC00070E4 : Carrier freq Now									*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	IuOut;				/* 0xC00070E6 : Iu reference(for dead-time cmp)						*/
	SHORT	IvOut;				/* 0xC00070E8 : Iv reference(for dead-time cmp)						*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	V1;					/* 0xC00070EA : V1( = SQRT(VdOut^2+VqOut^2) )						*/
	SHORT	Vcent;				/* 0xC00070EC : Central voltage										*/
	SHORT	Dummy1;				/* 0xC00070EE : for Alignment										*/
/*--------------------------------------------------------------------------------------------------*/
/*		for LPF																						*/
/*--------------------------------------------------------------------------------------------------*/
	DWREG	IqOut1Lpf;			/* 0xC00070F0 : LPF1 Output(Low)									*/
	DWREG	IqOut2Lpf;			/* 0xC00070F4 : LPF2 Output(Low)									*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	IqRef;				/* 0xC00070F8 : Iq Reference after limit							*/
	SHORT	TLimP;				/* 0xC00070FA : +tlim												*/
	SHORT	TLimM;				/* 0xC00070FC : -tlim												*/
/*--------------------------------------------------------------------------------------------------*/
/*		for monitor																					*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	IqMon;				/* 0xC00070FE : IQ monitor											*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	IdDataP;			/* 0xC0007100 : 													*/
	SHORT	IqDataP;			/* 0xC0007102 : 													*/
	SHORT	KEangle;			/* 0xC0007104 : 機械角→電気角変換係数								*/
	SHORT	Dummy2;				/* 0xC0007106 : for Alignment										*/
	DWREG	IdLfil;				/* 0xC0007108 : L(di/dt)フィルタ									*/
	DWREG	IqLfil;				/* 0xC000710C : 													*/
/*--------------------------------------------------------------------------------------------------*/
/*		for axis q monitor																			*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	IqDistIn;			/* 0xC0007110 : q軸外乱トルク入力									*/
	SHORT	IqDist;				/* 0xC0007112 : q軸外乱トルク										*/
	SHORT	IqMonFil;			/* 0xC0007114 : q軸電流指令モニタ(フィルタ後)						*/
	SHORT	IqOfRef;			/* 0xC0007116 : q軸電流指令(外乱トルク加算後)						*/
/*--------------------------------------------------------------------------------------------------*/
/*		for notch filter1																			*/
/*--------------------------------------------------------------------------------------------------*/
	DWREG	IqOut1L;			/* 0xC0007118 : filter1 output(Low)									*/
	DWREG	IqOut1PL;			/* 0xC000711C : filter1 output 1delay(Low)							*/
	DWREG	IqOut1PPL;			/* 0xC0007120 : filter1 output 2delay(Low)							*/
	DWREG	IqIn1PL;			/* 0xC0007124 : filter1 input 1delay(Low)							*/
	DWREG	IqIn1PPL;			/* 0xC0007128 : filter1 input 2delay(Low)							*/
	SHORT	IQO1;				/* 0xC000712C : filter1 output(short type)							*/
	SHORT	Dummy3;				/* 0xC000712E : for Alignment										*/
	DWREG	IqOut1BufL;			/* 0xC0007130 : filter1 output buffer(Low)							*/
/*--------------------------------------------------------------------------------------------------*/
/*		for notch filter2																			*/
/*--------------------------------------------------------------------------------------------------*/
	DWREG	IqOut2L;			/* 0xC0007134 : filter2 output(Low)									*/
	DWREG	IqOut2PL;			/* 0xC0007138 : filter2 output 1delay(Low)							*/
	DWREG	IqOut2PPL;			/* 0xC000713C : filter2 output 2delay(Low)							*/
	DWREG	IqIn2PL;			/* 0xC0007140 : filter2 input 1delay(Low)							*/
	DWREG	IqIn2PPL;			/* 0xC0007144 : filter2 input 2delay(Low)							*/
	SHORT	IQO2;				/* 0xC0007148 : filter2 output(short type)							*/
	SHORT	Dummy4;				/* 0xC000714A : for Alignment										*/
	DWREG	IqOut2BufL;			/* 0xC000714C : filter2 output buffer(Low)							*/
/*--------------------------------------------------------------------------------------------------*/
/*		for notch filter3																			*/
/*--------------------------------------------------------------------------------------------------*/
	DWREG	IqOut3L;			/* 0xC0007150 : filter3 output(Low)									*/
	DWREG	IqOut3PL;			/* 0xC0007154 : filter3 output 1delay(Low)							*/
	DWREG	IqOut3PPL;			/* 0xC0007158 : filter3 output 2delay(Low)							*/
	DWREG	IqIn3PL;			/* 0xC000715C : filter3 input 1delay(Low)							*/
	DWREG	IqIn3PPL;			/* 0xC0007160 : filter3 input 2delay(Low)							*/
	SHORT	IqOut3;				/* 0xC0007164 : filter3 output(short type)							*/
	SHORT	Dummy5;				/* 0xC0007166 : for Alignment										*/
	DWREG	IqOut3BufL;			/* 0xC0007168 : filter3 output buffer(Low)							*/
/*--------------------------------------------------------------------------------------------------*/
/*		for notch filter4																			*/
/*--------------------------------------------------------------------------------------------------*/
	DWREG	IqOut4L;			/* 0xC000716C : filter4 output(Low)									*/
	DWREG	IqOut4PL;			/* 0xC0007170 : filter4 output 1delay(Low)							*/
	DWREG	IqOut4PPL;			/* 0xC0007174 : filter4 output 2delay(Low)							*/
	DWREG	IqIn4PL;			/* 0xC0007178 : filter4 input 1delay(Low)							*/
	DWREG	IqIn4PPL;			/* 0xC000717C : filter4 input 2delay(Low)							*/
	SHORT	IQO4;				/* 0xC0007180 : filter4 output(short type)							*/
	SHORT	Dummy6;				/* 0xC0007182 : for Alignment										*/
	DWREG	IqOut4BufL;			/* 0xC0007184 : filter4 output buffer(Low)							*/
/*--------------------------------------------------------------------------------------------------*/
/*		for notch filter5																			*/
/*--------------------------------------------------------------------------------------------------*/
	DWREG	IqOut5L;			/* 0xC0007188 : filter5 output(Low)									*/
	DWREG	IqOut5PL;			/* 0xC000718C : filter5 output 1delay(Low)							*/
	DWREG	IqOut5PPL;			/* 0xC0007190 : filter5 output 2delay(Low)							*/
	DWREG	IqIn5PL;			/* 0xC0007194 : filter5 input 1delay(Low)							*/
	DWREG	IqIn5PPL;			/* 0xC0007198 : filter5 input 2delay(Low)							*/
	SHORT	IQO5;				/* 0xC000719C : filter5 output(short type)							*/
	SHORT	Dummy7;				/* 0xC000719E : for Alignment										*/
	DWREG	IqOut5BufL;			/* 0xC00071A0 : filter5 output buffer(Low)							*/
/*--------------------------------------------------------------------------------------------------*/
/*		for notch filter6																			*/
/*--------------------------------------------------------------------------------------------------*/
	DWREG	IqOut6L;			/* 0xC00071A4 : filter6 output(Low)									*/
	DWREG	IqOut6PL;			/* 0xC00071A8 : filter6 output 1delay(Low)							*/
	DWREG	IqOut6PPL;			/* 0xC00071AC : filter6 output 2delay(Low)							*/
	DWREG	IqIn6PL;			/* 0xC00071B0 : filter6 input 1delay(Low)							*/
	DWREG	IqIn6PPL;			/* 0xC00071B4 : filter6 input 2delay(Low)							*/
	SHORT	IQO6;				/* 0xC00071B8 : filter6 output(short type)							*/
	SHORT	Dummy8;				/* 0xC00071BA : for Alignment										*/
	DWREG	IqOut6BufL;			/* 0xC00071BC : filter6 output buffer(Low)							*/
} INTADV;

/*--------------------------------------------------------------------------------------------------*/
/*		for Current Observer																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _DOBSP {
	SHORT	TsPerL;				/* 0xC00071C0 : Ts/L ( オブザーバ用パラメータ )						*/
	SHORT	Gobs;				/* 0xC00071C2 : g ( オブザーバの極 )								*/
	SHORT	RLTs;				/* 0xC00071C4 : 1-R･Ts/L ( オブザーバ用パラメータ )					*/
	SHORT	FilObsGain;			/* 0xC00071C6 : フィルタゲイン										*/
/*--------------------------------------------------------------------------------------------------*/
} DOBSP;

/*--------------------------------------------------------------------------------------------------*/
/*		for Current Observer																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _DOBSV {
	DWREG	LpfIlq;				/* 0xC00071C8 : q軸ローパスフィルタ変数								*/
	DWREG	HpfIlq;				/* 0xC00071CC : q軸ハイパスフィルタ変数								*/
	SHORT	IqObsOut;			/* 0xC00071D0 : q軸オブザーバ出力									*/
	SHORT	IqObsFreq;			/* 0xC00071D2 : q軸振動成分											*/
	DWREG	LpfIld;				/* 0xC00071D4 : d軸ローパスフィルタ変数								*/
	DWREG	HpfIld;				/* 0xC00071D8 : d軸ハイパスフィルタ変数								*/
	SHORT	IdObsOut;			/* 0xC00071DC : d軸オブザーバ出力									*/
	SHORT	IdObsFreq;			/* 0xC00071DE : d軸振動成分											*/
	SHORT	DmpGain;			/* 0xC00071E0 : 電流オブザーバダンピングゲイン						*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	Dummy;				/* 0xC00071E2 : for Alignment										*/
} DOBSV;

/*--------------------------------------------------------------------------------------------------*/
/*		for Phase Interpolate																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _PHASEV {
	SHORT	PHAL;				/* 0xC00071E4 : Phase(Low)											*/
	SHORT	PhaseH;				/* 0xC00071E6 : Phase(High)											*/
	SHORT	PhaseIp;			/* 0xC00071E8 : 位相補間量											*/
	SHORT	PhaseIpF;			/* 0xC00071EA : 位相補間フラグ(0：しない、1：する)					*/
	SHORT	PhaseIpIn;			/* 0xC00071EC : 位相補間量(CPU→ASIC受け渡し用)						*/
	SHORT	PhaseIpFIn;			/* 0xC00071EE : 位相補間フラグ(CPU→ASIC受け渡し用)					*/
} PHASEV;

/*--------------------------------------------------------------------------------------------------*/
/*		for Encoder IF																				*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _ENCIFV {
	SHORT	AmpType;			/* 0xC00071F0 : Rotary Type or Linear Type							*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	Dummy;				/* 0xC00071F2 : for Alignment										*/
	DWREG	RcvPosX0;			/* 0xC00071F4 : Motor Encoder Position(受信位置)					*/
	DWREG	RcvPosX1;			/* 0xC00071F8 : Motor Encoder Position(受信位置)					*/
	DWREG	RcvPosX2;			/* 0xC00071FC : Motor Encoder Position(受信位置)					*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	MotPosSftR;			/* 0xC0007200 : Shift Data for Motor Position Cal.					*/
	SHORT	MotPosSftX;			/* 0xC0007202 : Shift Data for Motor Position Cal.					*/
	SHORT	MotPosSign;			/* 0xC0007204 : Sign  Data for Motor Position Cal.					*/
	SHORT	DivOutSft;			/* 0xC0007206 : 分周出力シフト数									*/
	DWREG	AccErrLv;			/* 0xC0007208 : Motor Encoder Acc. Error Check Low					*/
/*--------------------------------------------------------------------------------------------------*/
	DWREG	DivPls;				/* 0xC000720C : 分周出力パルス										*/
/*--------------------------------------------------------------------------------------------------*/
	DWREG	DivOutGain;			/* 0xC0007210 : 分周出力ゲイン(リニア)								*/
	DWREG	DivPos;				/* 0xC0007214 : 分周出力パルス(リニア)								*/
	DWREG	DivPlsRem;			/* 0xC0007218 : 分周出力パルス余り(リニア)							*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	SPGFail;			/* 0xC000721C : S-PG受信失敗許容回数								*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	BitIprm;			/* 0xC000721E : Bit Initial Parameter								*/
	SHORT	BitData;			/* 0xC0007220 : Bit Data											*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	IncPlsReq;			/* 0xC0007222 : PAO IncrePulse Output Request						*/
	SHORT	PAOSeqCmd;			/* 0xC0007224 : PAO Serial Output Sequence							*/
	SHORT	PlsOSetCmd;			/* 0xC0007226 : パルス出力回路初期化要求							*/
} ENCIFV;

/*--------------------------------------------------------------------------------------------------*/
/*		分周パルス出力関連：HostCPU --> Asic 定周期転送用											*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _DIVPLSV {
	SHORT	IncPlsReqIn;		/* 0xC0007228 : PAO IncrePulse Output Request						*/
	SHORT	PAOSeqCmdIn;		/* 0xC000722A : PAO Serial Output Sequence							*/
	SHORT	PlsOSetCmdIn;		/* 0xC000722C : パルス出力回路初期化要求							*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	DivSetIn;			/* 0xC000722E : 分周機能設定入力									*/
	SHORT	PoSet1In;			/* 0xC0007230 : パルス変換原点補正1									*/
	SHORT	PoSet2In;			/* 0xC0007232 : パルス変換原点補正2									*/
/*--------------------------------------------------------------------------------------------------*/
	DWREG	IncInitPlsIn;		/* 0xC0007234 : 初期インクレパルス									*/
	DWREG	IncInitRemIn;		/* 0xC0007238 : 初期インクレパルス余り								*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	AccCntClrReq;		/* 0xC000723C : 加速度チェック開始カウントクリア要求				*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	Dummy;				/* 0xC000723E : for Alignment										*/
/*--------------------------------------------------------------------------------------------------*/
/*		Register for Library																		*/
/*--------------------------------------------------------------------------------------------------*/
	DWREG	Argu0;				/* 0xC0007240 : Argument0											*/
	DWREG	Argu1;				/* 0xC0007244 : Argument1											*/
	SHORT	Argu2;				/* 0xC0007248 : Argument2											*/
	SHORT	Argu2H;				/* 0xC000724A : Argument2 High Word									*/
	DWREG	Ret0;				/* 0xC000724C : Return Register										*/
	DWREG	Kx;					/* 0xC0007250 : kx													*/
	DWREG	Sx;					/* 0xC0007254 : sx													*/
	DWREG	Iu0;				/* 0xC0007258 : iu[0]												*/
	SHORT	Iu1;				/* 0xC000725C : iu[1]												*/
	SHORT	Iu1H;				/* 0xC000725E : iu[1] High Word										*/
} DIVPLSV;

/*--------------------------------------------------------------------------------------------------*/
/*		for Weaken Field																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _WEAKFV {
	SHORT	WfKpVLIn;			/* 0xC0007260 : 電圧FB比例ゲイン(下位16bit)							*/
	SHORT	WfKpVHIn;			/* 0xC0007262 : 電圧FB比例ゲイン(上位16bit)							*/
	SHORT	WfKiVLIn;			/* 0xC0007264 : 電圧FB積分ゲイン(下位16bit)							*/
	SHORT	WfKiVHIn;			/* 0xC0007266 : 電圧FB積分ゲイン(上位16bit)							*/
	SHORT	WfV1MaxIn;			/* 0xC0007268 : 電圧指令制限レベル									*/
	SHORT	WfIdRefLimIn;		/* 0xC000726A : d軸電流指令リミット									*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	WfIntegLim;			/* 0xC000726C : 電圧FB積分リミット									*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	IdOut;				/* 0xC000726E : Id reference										*/
	SHORT	IqOut;				/* 0xC0007270 : Iq reference										*/
	SHORT	Vel;				/* 0xC0007272 : Velocity (omega)									*/
/*--------------------------------------------------------------------------------------------------*/
	DWREG	WfKpV;				/* 0xC0007274 : 電圧FB比例ゲイン(下位16bit)							*/
	DWREG	WfKiV;				/* 0xC0007278 : 電圧FB積分ゲイン(下位16bit)							*/
	SHORT	WfV1Max;			/* 0xC000727C : 電圧指令制限レベル									*/
	SHORT	WfIdRefLim;			/* 0xC000727E : d軸電流指令リミット									*/
	SHORT	WfVqMax;			/* 0xC0007280 : q軸電圧指令リミット									*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	Dummy;				/* 0xC0007282 : for Alignment										*/
	DWREG	WfIntgl;			/* 0xC0007284 : 電圧FB積分値										*/
	SHORT	WfVdRef;			/* 0xC0007288 : d軸電圧指令											*/
	SHORT	WfVqRef;			/* 0xC000728A : q軸電圧指令											*/
} WEAKFV;

/*--------------------------------------------------------------------------------------------------*/
/*		for make PWM Carrer Counter																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _PWMV {
	SHORT	PwmCntT2;			/* 0xC000728C : PWMキャリアカウンタT2								*/
	SHORT	PwmCntT1;			/* 0xC000728E : PWMキャリアカウンタT1								*/
	SHORT	PwmCntT0;			/* 0xC0007290 : PWMキャリアカウンタT0								*/
	SHORT	Dummy;				/* 0xC0007292 : for Alignment										*/
} PWMV;

/*--------------------------------------------------------------------------------------------------*/
/*		Version define register																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _VER_INFO {
	SHORT	MswVer;				/* 0xC0007294 : ソフトバージョン情報								*/
	SHORT	TstVer;				/* 0xC0007296 : テストバージョン情報								*/
	SHORT	YspVer;				/* 0xC0007298 : Ｙ仕様バージョン情報								*/
	SHORT	Dummy;				/* 0xC000729A : for Alignment										*/
} VER_INFO;

/*--------------------------------------------------------------------------------------------------*/
/*		Common work register																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _COMWORKS {
	SHORT	WREG82;				/* 0xC000729C : Work  Register 82									*/
	SHORT	WREG83;				/* 0xC000729E : Work  Register 83									*/
	SHORT	WREG84;				/* 0xC00072A0 : Work  Register 84									*/
	SHORT	WREG85;				/* 0xC00072A2 : Work  Register 85									*/
	SHORT	WREG86;				/* 0xC00072A4 : Work  Register 86									*/
	SHORT	WREG87;				/* 0xC00072A6 : Work  Register 87									*/
	SHORT	WREG88;				/* 0xC00072A8 : Work  Register 88									*/
	SHORT	WREG89;				/* 0xC00072AA : Work  Register 89									*/
	SHORT	WREG95;				/* 0xC00072AC : Work  Register 95									*/
	SHORT	WREG100;			/* 0xC00072AE : Work  Register 100									*/
	SHORT	WREG101;			/* 0xC00072B0 : Work  Register 101									*/
	SHORT	WREG102;			/* 0xC00072B2 : Work  Register 102									*/
	SHORT	WREG103;			/* 0xC00072B4 : Work  Register 103									*/
	SHORT	WREG104;			/* 0xC00072B6 : Work  Register 104									*/
	SHORT	WREG109;			/* 0xC00072B8 : Work  Register 109									*/
	SHORT	Dummy;				/* 0xC00072BA : for Alignment										*/
} COMWORKS;

typedef struct _HOST_WOKS {
	DWREG	HTMP0;				/* 0xC00072BC : HOST Interrupt Temp0								*/
	DWREG	HTMP2;				/* 0xC00072C0 : HOST Interrupt Temp2								*/
	DWREG	HTMP4;				/* 0xC00072C4 : HOST Interrupt Temp4								*/
	DWREG	HTMP6;				/* 0xC00072C8 : HOST Interrupt Temp6								*/
	SHORT	HTMP7;				/* 0xC00072CC : HOST Interrupt Temp7								*/
	SHORT	HTMP8;				/* 0xC00072CE : HOST Interrupt Temp8								*/
} HOST_WOKS;

typedef struct _ROUND_WORKS {
	SHORT	WREG140;			/* 0xC00072D0 : Work Register 140									*/
	SHORT	WREG141;			/* 0xC00072D2 : Work Register 141									*/
	SHORT	WREG142;			/* 0xC00072D4 : Work Register 142									*/
	SHORT	WREG143;			/* 0xC00072D6 : Work Register 143									*/
	SHORT	WREG144;			/* 0xC00072D8 : Work Register 144									*/
	SHORT	WREG145;			/* 0xC00072DA : Work Register 145									*/
	SHORT	WREG146;			/* 0xC00072DC : Work Register 146									*/
	SHORT	WREG147;			/* 0xC00072DE : Work Register 147									*/
	SHORT	WREG148;			/* 0xC00072E0 : Work Register 148									*/
	SHORT	WREG149;			/* 0xC00072E2 : Work Register 149									*/
} ROUND_WORKS;

typedef struct _AD_WORKS {
	DWREG	TMP0;				/* 0xC00072E4 : AD Interrupt Temp0									*/
	DWREG	TMP2;				/* 0xC00072E8 : AD Interrupt Temp2									*/
	DWREG	TMP4;				/* 0xC00072EC : AD Interrupt Temp4									*/
	DWREG	TMP6;				/* 0xC00072F0 : AD Interrupt Temp6									*/
	DWREG	TMP8;				/* 0xC00072F4 : AD Interrupt Temp8									*/
} AD_WORKS;

/*--------------------------------------------------------------------------------------------------*/
/*		初期化処理用ワーク																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _INITWK {
	SHORT	IN_WK0;				/* 0xC00072F8 : 初期化処理用 Work0 Lo								*/
	SHORT	IN_WK0H;			/* 0xC00072FA : 初期化処理用 Work0 Hi								*/
	SHORT	IN_WK1;				/* 0xC00072FC : 初期化処理用 Work1 Lo								*/
	SHORT	IN_WK1H;			/* 0xC00072FE : 初期化処理用 Work1 Hi								*/
} INITWK;




/****************************************************************************************************/
/*		Axis Control Structure definition															*/
/****************************************************************************************************/
typedef struct _ASIC_U_IF_STR {
	AXIS_INFO		AxisInfo;	/* 軸情報変数														*/
	ACRV			AcrV;		/* 電流制御用変数													*/
	STSFLG			StsFlg;		/* ステータスフラグ													*/
	VCMPV			VcmpV;		/* 電圧補償演算用変数												*/
	SINTBL			SinTbl;		/* sinテーブル														*/
	ADSTOP			AdStop;		/* 電流制御停止エラー検出用構造体									*/
	ADINV			AdinV;		/* ホストCPU受け渡し変数											*/
	INTADP			IntAdP;		/* 電流割込みパラメータ												*/
	INTADV			IntAdV;		/* 電流割込み変数													*/
	DOBSP			DobsP;		/* d軸オブザーバ用パラメータ										*/
	DOBSV			DobsV;		/* d軸オブザーバ用変数												*/
	PHASEV			PhaseV;		/* 位相演算用変数													*/
	ENCIFV			EncIfV;		/* エンコーダインタフェース変数										*/
	DIVPLSV			DivPlsV;	/* 分周パルス用変数													*/
	WEAKFV			WeakFV;		/* 弱め界磁用変数													*/
	PWMV			PwmV;		/* PWMキャリアカウンタ格納変数										*/
	USHORT			CtrlStsIn;	/* μプログラム制御ステータス(CPU⇒μプログラム)					*/
	USHORT			CtrlStsOut;	/* μプログラム制御ステータス(μプログラム⇒CPU)					*/
	SHORT			Dummy[305];	/* Dummy buffer for Axis offset										*/
} ASIC_U_IF_STR;
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*		CSW (Control Select Switch )																*/
/****************************************************************************************************/
typedef struct _MICRO_CSW
{
	USHORT	data;
} MICRO_CSW;
#define ICLR		0x0001			/* bit.0  積分リミット時処理：ホールド(0)／クリア(1)			*/
#define ISEL		0x0002			/* bit.1  Ld/Lq補償：電流FB(0)／電流指令(1)						*/
//;--				0x0004			/* bit.2  --Reserve--											*/
#define OBSSEL		0x0008			/* bit.3  Current Observer Select bit							*/
#define F1DSABL	0x0010			/* bit.4  Notch filter1 Disable									*/
#define F2DSABL	0x0020			/* bit.5  Notch filter2 Disable									*/
#define LPFDSABL	0x0040			/* bit.6  Low Pass filter1 Disable								*/
#define LPFCDSABL	0x0080			/* bit.7  Low Pass filter2 Disable								*/
#define OVMSEL1	0x0100			/* bit.8  if bit8,9 = 01 then OVMOD1							*/
#define OVMSEL2	0x0200			/* bit.9  if bit8,9 = 10 or 11 then OVMOD2  					*/
#define OVMMOD		0x0400			/* bit.10 modulation modification enable						*/
#define V_FB		0x0800			/* bit.11 電圧FB方式弱め界磁選択								*/
#define DIDTSEL	0x1000			/* bit.12 L di/dt cmp select(1:di/dt,0:def)						*/
#define V_FB2		0x2000			/* bit.13 電圧FB方式弱め界磁選択2								*/
#define INT_ST		0x4000			/* bit.14 飽和時積分停止										*/
#define F3DSABL	0x8000			/* bit.15 Notch filter3 Disable									*/


/****************************************************************************************************/
/*		CSW2 (Control Select Switch2 )																*/
/****************************************************************************************************/
#define F4DSABL	0x0001			/* bit.0  Notch filter4 Disable									*/
#define F5DSABL	0x0002			/* bit.1  Notch filter5 Disable									*/
#define F6DSABL	0x0004			/* bit.2  Notch filter6 Disable									*/

/****************************************************************************************************/
/*		ビットパラメータ ( 初期化時セット CPU→ASIC )												*/
/****************************************************************************************************/
typedef struct _MICRO_BITIPRM
{
	USHORT	data;
} MICRO_BITIPRM;
#define UPGDIVOUT	0x0001			/* bit.0  マイクロ分周機能使用選択								*/
//;--				0x0002			/* bit.1  --Reserve--											*/
//;--				0x0004			/* bit.2  --Reserve--											*/
//;--				0x0008			/* bit.3  --Reserve--											*/
//;--				0x0010			/* bit.4  --Reserve--											*/
//;--				0x0020			/* bit.5  --Reserve--											*/
//;--				0x0040			/* bit.6  --Reserve--											*/
//;--				0x0080			/* bit.7  --Reserve--											*/
//;--				0x0100			/* bit.8  --Reserve--											*/
//;--				0x0200			/* bit.9  --Reserve--											*/
//;--				0x0400			/* bit.10 --Reserve--											*/
//;--				0x0800			/* bit.11 --Reserve--											*/
//;--				0x1000			/* bit.12 --Reserve--											*/
//;--				0x2000			/* bit.13 --Reserve--											*/
//;--				0x4000			/* bit.14 --Reserve--											*/
//;--				0x8000			/* bit.15 --Reserve--											*/


/****************************************************************************************************/
/*		ビットデータ ( 初期化時セット CPU→ASIC、スキャン処理 ASIC→CPU )							*/
/****************************************************************************************************/
typedef struct _MICRO_BITDAT
{
	USHORT	data;
} MICRO_BITDAT;
#define SERXERR		0x0001			/* bit.0  Senc Receive Error									*/
#define ACCCHKENA	0x0002			/* bit.1  Encoder Acc. Check Enable Flag						*/
//;--				0x0004			/* bit.2  --Reserve--											*/
//;--				0x0008			/* bit.3  --Reserve--											*/
//;--				0x0010			/* bit.4  --Reserve--											*/
//;--				0x0020			/* bit.5  --Reserve--											*/
//;--				0x0040			/* bit.6  --Reserve--											*/
//;--				0x0080			/* bit.7  --Reserve--											*/
//;--				0x0100			/* bit.8  --Reserve--											*/
//;--				0x0200			/* bit.9  --Reserve--											*/
//;--				0x0400			/* bit.10 --Reserve--											*/
//;--				0x0800			/* bit.11 --Reserve--											*/
//;--				0x1000			/* bit.12 --Reserve--											*/
//;--				0x2000			/* bit.13 --Reserve--											*/
//;--				0x4000			/* bit.14 --Reserve--											*/
//;--				0x8000			/* bit.15 --Reserve--											*/



/****************************************************************************************************/
/*		μプログラム制御定義																		*/
/****************************************************************************************************/
/*		Bit-0 : μプログラム実行(1)／停止(0)														*/
/*		Bit-3 : レジスタロック(電流検出調整用)														*/
/*		Bit-4 : ベースブロック(サーボＯＦＦ)														*/
/****************************************************************************************************/
#define	MPCMD_PRGSTOP		0x0000			/* μプログラム実行停止									*/
#define	MPCMD_PRGSTART		0x0011			/* μプログラム実行開始									*/
#define	MPCMD_SVON			0x0001			/* μプログラムサーボON									*/
#define	MPCMD_SVOFF			0x0011			/* μプログラムサーボOFF								*/



/****************************************************************************************************/
/*		制御指令コマンド定義  MREG_HSUR																*/
/****************************************************************************************************/
/*		MPCMD_SYNC1      Bit-0 : 同期フラグ1(通常の更新用)											*/
/*		MPCMD_SYNC2      Bit-1 : 同期フラグ2(位相のみ更新用)										*/
/*		MPCMD_RENEWAL    Bit-2 : μプログラム指令更新												*/
/*		MPCMD_CURADSYNC  Bit-3 : 電流検出AD割り込み同期												*/
/****************************************************************************************************/
#define	MPCMD_SYNC1			0x0001			/* 														*/
#define	MPCMD_SYNC2			0x0002			/* ゼロ制御なし											*/
#define	MPCMD_RENEWAL		0x0004			/* ゼロ速停止モード										*/
#define	MPCMD_CURADSYNC		0x0008			/* ゼロクランプモード									*/



/****************************************************************************************************/
/*																									*/
/*		ASIC JL-086 SERVOIP レジスタアクセス用構造体定義											*/
/*																									*/
/****************************************************************************************************/
typedef struct {
	ASIC_HW_IF_STR		*AsicHwReg;			/* ASIC H/Wレジスタ										*/
	ASIC_U_IF_STR		*AsicMicroReg;		/* ASIC Microレジスタ									*/
	MICRO_BITIPRM		MicroBitIprm;		/* MicroBitIprmレジスタ									*/
	MICRO_BITDAT		MicroBitDat;		/* MicroBitDatレジスタ									*/
	MICRO_CSW			MicroCsw;			/* MicroCswレジスタ										*/
	MICRO_CSW			MicroCsw2;			/* MicroCswレジスタ										*//*<<S16F>>*/
	BOOL				MicroRunningSts;	/* Microステータス確認用								*/
#if defined(_VC_DEBUG_) || defined(_ROM_SIM_DEBUG_)
	USHORT				*pDummyMicroSram;	/* Dummy buffer for Micro Program download				*/
#endif /* _VC_DEBUG_ */
} ASICS;







/***************************************** end of file **********************************************/
#endif
