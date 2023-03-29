/****************************************************************************************************/
/*																									*/
/*																									*/
/*		JL076Reg.h : JL-076B/077Bレジスタ定義ヘッダファイル											*/
/*			※ファイル名はJL086だが、中身はJL076※													*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1. JL-076B/077Bレジスタ定義																	*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		2012.08.06 : Made by Excel Macro															*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#ifndef _JL076REG_H // 再読み込み防止用



/****************************************************************************************************/
/*																									*/
/*		ASIC JL-076B/077B H/Wレジスタ構造体定義														*/
/*																									*/
/****************************************************************************************************/
typedef struct ASIC_HW_IF_STR {
	VUSHORT	AREG_PWMU;				/* 0x0000 アナログモニタPWM出力U								*/
	VUSHORT	AREG_PWMV;				/* 0x0002 アナログモニタPWM出力V								*/
	VUSHORT	AREG_IOSTAT1;			/* 0x0004 汎用同期シリアル出力1＆送信バッファ転送				*/
	VUSHORT	AREG_IOSTAT2;			/* 0x0006 汎用同期シリアル出力2									*/
	VUSHORT	AREG_ITDO;				/* 0x0008 初期化時用タイマ(RD)									*/
	VUSHORT	AREG_TIM1SET;			/* 0x000A タイマ1設定レジスタ									*/
	VUSHORT	AREG_UARTSTS;			/* 0x000C UARTステータス										*/
	VUSHORT	AREG_UARTTXD;			/* 0x000E 汎用非同期ポート送信データ(WR)＆受信データ(RD)		*/
	VUSHORT	AREG_DIVCSEV;			/* 0x0010 エンコーダパルス出力分周比設定						*/
	VUSHORT	AREG_EPRSET;			/* 0x0012 EEPROM：IF設定										*/
	VUSHORT	AREG_EPRLW;				/* 0x0014 EEPROM：送信データ下位ワード							*/
	VUSHORT	AREG_EPRHW;				/* 0x0016 EEPROM：送信データ上位ワード							*/
	VUSHORT	AREG_EPRCNT;			/* 0x0018 EEPROM：クロック数設定＆送信開始						*/
	VUSHORT	AREG_INT1SET;			/* 0x001A INT1割込み設定&アクノリッジ							*/
	VUSHORT	AREG_PFNCSET;			/* 0x001C 端子機能設定											*/
	VUSHORT	SPARE0;					/* 0x001E Reserve												*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	AREG_FBCSET;			/* 0x0020 分周入力フィルタ設定									*/
	VUSHORT	AREG_POCSET;			/* 0x0022 指令カウンタ設定										*/
	VUSHORT	AREG_DINCSET;			/* 0x0024 分周機能設定											*/
	VUSHORT	AREG_LODSET;			/* 0x0026 ロード設定											*/
	VUSHORT	AREG_T_FBLD;			/* 0x0028 FBカウンタ設定										*/
	VUSHORT	AREG_T_CMDLD;			/* 0x002A CMDカウンタ設定										*/
	VUSHORT	AREG_ALMCLR;			/* 0x002C アラームラッチリセット								*/
	VUSHORT	AREG_ALMSET;			/* 0x002E アラーム設定											*/
	VUSHORT	AREG_IOSTATX;			/* 0x0030 汎用入力												*/
	VUSHORT	AREG_FLTSTAT;			/* 0x0032 故障入力												*/
	VUSHORT	SPARE1;					/* 0x0034 Reserve												*/
	VUSHORT	AREG_SETREG1;			/* 0x0036 内部設定レジスタ										*/
	VUSHORT	AREG_SETREG2;			/* 0x0038 機能選択レジスタ										*/
	VUSHORT	AREG_CLKO;				/* 0x003A シリアルエンコーダIF伝送速度設定レジスタ				*/
	VUSHORT	AREG_CSTP;				/* 0x003C 機能停止選択レジスタ("1"で停止)						*/
	VUSHORT	AREG_SEPGSET00;			/* 0x003E シリアルエンコーダIF設定：CH0設定0					*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	AREG_SETREG3;			/* 0x0040 回生機能設定レジスタ									*/
	VUSHORT	AREG_SEPGSET01;			/* 0x0042 シリアルエンコーダIF設定：CH0設定1					*/
	VUSHORT	AREG_SEPGSET10;			/* 0x0044 シリアルエンコーダIF設定：CH1設定0					*/
	VUSHORT	AREG_SEPGSET11;			/* 0x0046 シリアルエンコーダIF設定：CH1設定1					*/
	VUSHORT	AREG_PIOSET;			/* 0x0048 拡張I/O設定レジスタ									*/
	VUSHORT	AREG_FCCSET;			/* 0x004A 指令フィルタ設定レジスタ								*/
	VUSHORT	AREG_T_FLC;				/* 0x004C フルクローズドカウンタ設定							*/
	VUSHORT	AREG_ESYNC;				/* 0x004E シリアルエンコーダ同期カウンタ外部同期				*/
	VUSHORT	AREG_SE0_TXD0;			/* 0x0050 シリアルエンコーダCH0送信データ0						*/
	VUSHORT	AREG_SE0_TXD1;			/* 0x0052 シリアルエンコーダCH0送信データ1						*/
	VUSHORT	AREG_SE0_TXD2;			/* 0x0054 シリアルエンコーダCH0送信データ2						*/
	VUSHORT	AREG_SE0_TXD3;			/* 0x0056 シリアルエンコーダCH0送信データ3						*/
	VUSHORT	AREG_SE0_TXD4;			/* 0x0058 シリアルエンコーダCH0送信データ4						*/
	VUSHORT	AREG_SE0_TXD5;			/* 0x005A シリアルエンコーダCH0送信データ5						*/
	VUSHORT	AREG_SE0_TXD6;			/* 0x005C シリアルエンコーダCH0送信データ6						*/
	VUSHORT	AREG_SE0_TXD7;			/* 0x005E シリアルエンコーダCH0送信データ7						*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	AREG_SE0_RXD0;			/* 0x0060 シリアルエンコーダCH0受信データ0						*/
	VUSHORT	AREG_SE0_RXD1;			/* 0x0062 シリアルエンコーダCH0受信データ1						*/
	VUSHORT	AREG_SE0_RXD2;			/* 0x0064 シリアルエンコーダCH0受信データ2						*/
	VUSHORT	AREG_SE0_RXD3;			/* 0x0066 シリアルエンコーダCH0受信データ3						*/
	VUSHORT	AREG_SE0_RXD4;			/* 0x0068 シリアルエンコーダCH0受信データ4						*/
	VUSHORT	AREG_SE0_RXD5;			/* 0x006A シリアルエンコーダCH0受信データ5						*/
	VUSHORT	AREG_SE0_RXD6;			/* 0x006C シリアルエンコーダCH0受信データ6						*/
	VUSHORT	AREG_SE0_RXD7;			/* 0x006E シリアルエンコーダCH0受信データ7						*/
	VUSHORT	AREG_GSSRDL;			/* 0x0070 汎用同期シリアル受信データL							*/
	VUSHORT	AREG_GSSRDH;			/* 0x0072 汎用同期シリアル受信データH							*/
	VUSHORT	AREG_CMDCLML;			/* 0x0074 指令カウンタCMLラッチ値								*/
	VUSHORT	AREG_SPRG0CR;			/* 0x0076 SENC：CH0コマンド/レスポンスバッファ					*/
	VUSHORT	AREG_CDLTD;				/* 0x0078 シリアルエンコーダCH0カウンタラッチ値					*/
	VUSHORT	AREG_CMDLDL;			/* 0x007A 指令カウンタロードラッチ値							*/
	VUSHORT	AREG_SPRG1CR;			/* 0x007C SENC：CH1コマンド/レスポンスバッファ					*/
	VUSHORT	AREG_SEDLLD;			/* 0x007E シリアルエンコーダ遅延同期ロード値					*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	AREG_ADCTL;				/* 0x0080 汎用ADIF制御											*/
	VUSHORT	SPARE2;					/* 0x0082 Reserve												*/
	VUSHORT	AREG_TSTEELWL;			/* 0x0084 EEPROM IF Lowワード値(テスト用)						*/
	VUSHORT	AREG_TSTDCCNT;			/* 0x0086 分周DDA現在値											*/
	VUSHORT	AREG_TSTT1CNT;			/* 0x0088 タイマT1値(テスト用)									*/
	VUSHORT	SPARE3;					/* 0x008A Reserve												*/
	VUSHORT	AREG_TSTPWMUCNT;		/* 0x008C PWMUカウンタ下位8ビット値(テスト用)					*/
	VUSHORT	AREG_TSTPWMVCNT;		/* 0x008E PWMVカウンタ下位8ビット値(テスト用)					*/
	VUSHORT	AREG_SDMCONF1;			/* 0x0090 デシメーションフィルタ設定							*/
	VUSHORT	AREG_SDMCONF2;			/* 0x0092 オーバレンジ検出マスクビット設定、試験設定			*/
	VUSHORT	AREG_PWMCCD;			/* 0x0094 PWMキャリアカウンタ現在値								*/
	VUSHORT	SPARE4;					/* 0x0096 Reserve												*/
	VUSHORT	AREG_DYSETUP1;			/* 0x0098 U相P側オンディレイ時間設定1							*/
	VUSHORT	AREG_DYSETUN1;			/* 0x009A U相N側オンディレイ時間設定1							*/
	VUSHORT	AREG_DYSETVP1;			/* 0x009C V相P側オンディレイ時間設定1							*/
	VUSHORT	AREG_DYSETVN1;			/* 0x009E V相N側オンディレイ時間設定1							*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	AREG_DYSETWP1;			/* 0x00A0 W相P側オンディレイ時間設定1							*/
	VUSHORT	AREG_DYSETWN1;			/* 0x00A2 W相N側オンディレイ時間設定1							*/
	VUSHORT	SPARE5[2];				/* 0x00A4-0x00A6 Reserve										*/
	VUSHORT	AREG_ZDR;				/* 0x00A8 パルス変換原点補正設定								*/
	VUSHORT	AREG_ZENBR;				/* 0x00AA 原点通過ビット位置設定								*/
	VUSHORT	AREG_DIVHSR;			/* 0x00AC 分周回路ヒステリシス設定								*/
	VUSHORT	SPARE6;					/* 0x00AE Reserve												*/
	VUSHORT	AREG_PUMPON;			/* 0x00B0 チャージポンプ設定									*/
	VUSHORT	AREG_DIVSET;			/* 0x00B2 分周機能設定											*/
	VUSHORT	AREG_PLSET;				/* 0x00B4 パルス変換設定										*/
	VUSHORT	AREG_WDT1SET;			/* 0x00B6 演算部用WDT設定										*/
	VUSHORT	AREG_POSET0;			/* 0x00B8 シリアルPG-パルス変換位置設定							*/
	VUSHORT	AREG_POSET1;			/* 0x00BA シリアルPG-パルス変換原点補正1						*/
	VUSHORT	AREG_POSET2;			/* 0x00BC シリアルPG-パルス変換原点補正2						*/
	VUSHORT	AREG_POTEST;			/* 0x00BE シリアルPG-パルス変換設定L(テスト用)					*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	AREG_POSET0H;			/* 0x00C0 シリアルPG-パルス変換位置設定bit16-23					*/
	VUSHORT	AREG_PGDCAL;			/* 0x00C2 シリアルPG-パルス変換シフト数,データ長設定			*/
	VUSHORT	AREG_POSET12H;			/* 0x00C4 シリアルPG-パルス変換原点補正H						*/
	VUSHORT	AREG_WDT2SET;			/* 0x00C6 ホスト用WDT設定										*/
	VUSHORT	AREG_DCTRIM;			/* 0x00C8 分周出力原点補正設定(Bit0-15)							*/
	VUSHORT	AREG_ADSET1;			/* 0x00CA AD設定												*/
	VUSHORT	AREG_ADSET2;			/* 0x00CC 指令AD設定：基本クロック								*/
	VUSHORT	AREG_ADSET3;			/* 0x00CE 指令AD設定：IFサイクル								*/
	VUSHORT	AREG_ADSET4;			/* 0x00D0 指令AD設定：ラッチタイミング							*/
	VUSHORT	AREG_ADSET5;			/* 0x00D2 指令AD設定：コントロールCH0							*/
	VUSHORT	AREG_ADSET6;			/* 0x00D4 指令AD設定：コントロールCH1							*/
	VUSHORT	AREG_ADSYNC;			/* 0x00D6 指令ADサイクル同期									*/
	VUSHORT	AREG_ADFCNT1;			/* 0x00D8 電流AD設定：基本クロック								*/
	VUSHORT	AREG_ADFCNT2;			/* 0x00DA 電流AD設定：IFサイクル								*/
	VUSHORT	AREG_ADFCNT3;			/* 0x00DC 電流AD設定：ラッチタイミング							*/
	VUSHORT	AREG_ADFCNT4;			/* 0x00DE 電流AD設定：コントロールCH0							*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	AREG_ADFCNT5;			/* 0x00E0 電流AD設定：コントロールCH1							*/
	VUSHORT	AREG_ADFSYNC;			/* 0x00E2 電流ADサイクル同期									*/
	VUSHORT	AREG_INT0SET;			/* 0x00E4 INT0割込み設定										*/
	VUSHORT	AREG_ACULH;				/* 0x00E6 指令AD：CH0ラッチデータ(Bit16-23)						*/
	VUSHORT	AREG_INT0TRG;			/* 0x00E8 INT0割込み同期ポート/カウンタ値読出し					*/
	VUSHORT	AREG_DCNTV;				/* 0x00EA 分周カウンタ設定値(Bit0-15)							*/
	VUSHORT	AREG_ACUL;				/* 0x00EC 指令AD：CH0ラッチデータ								*/
	VUSHORT	AREG_ACVLL;				/* 0x00EE 指令AD：CH1ラッチデータ(Bit0-15)						*/
	VUSHORT	AREG_AFUL;				/* 0x00F0 電流AD：U相電流ラッチデータ							*/
	VUSHORT	AREG_AFVL;				/* 0x00F2 電流AD：V相電流ラッチデータ							*/
	VUSHORT	AREG_CMDAD1;			/* 0x00F4 指令ADデータ1(テスト用)								*/
	VUSHORT	AREG_CMDAD2;			/* 0x00F6 指令ADデータ2(テスト用)								*/
	VUSHORT	AREG_CURAD1;			/* 0x00F8 電流ADデータ1(テスト用)								*/
	VUSHORT	AREG_CURAD2;			/* 0x00FA 電流ADデータ2(テスト用)								*/
	VUSHORT	AREG_DDACT;				/* 0x00FC シリアルPG-パルス変換DDA値(テスト用)					*/
	VUSHORT	AREG_ACVLH;				/* 0x00FE 指令AD：CH1ラッチデータ(Bit16-23)						*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	AREG_MAD0;				/* 0x0100 汎用ADチャネル0データ									*/
	VUSHORT	AREG_MAD1;				/* 0x0102 汎用ADチャネル1データ									*/
	VUSHORT	AREG_MAD2;				/* 0x0104 汎用ADチャネル2データ									*/
	VUSHORT	AREG_MAD3;				/* 0x0106 汎用ADチャネル3データ									*/
	VUSHORT	AREG_MAD4;				/* 0x0108 汎用ADチャネル4データ									*/
	VUSHORT	AREG_MAD5;				/* 0x010A 汎用ADチャネル5データ									*/
	VUSHORT	AREG_MAD6;				/* 0x010C 汎用ADチャネル6データ									*/
	VUSHORT	AREG_MAD7;				/* 0x010E 汎用ADチャネル7データ									*/
	VUSHORT	AREG_MADCTL;			/* 0x0110 汎用ADIF設定											*/
	VUSHORT	AREG_MADT12;			/* 0x0112 汎用ADIFチップセレクトタイミング設定					*/
	VUSHORT	AREG_MADT34;			/* 0x0114 汎用ADIFクロックタイミング設定						*/
	VUSHORT	AREG_MADT56;			/* 0x0116 汎用ADIFマルチプレクサイネーブル設定					*/
	VUSHORT	AREG_MUMSK;				/* 0x0118 汎用ADIFマルチプレクサチャネルマスク設定				*/
	VUSHORT	AREG_I2CCNT;			/* 0x011A I2C EEPROM ポートコントロール							*/
	VUSHORT	AREG_I2CDATA;			/* 0x011C I2C EEPROM 送信データ									*/
	VUSHORT	AREG_I2CDI;				/* 0x011E I2C EEPROM 受信データ									*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	AREG_NCTCSET;			/* 0x0120 指令用NCT設定											*/
	VUSHORT	AREG_AFULH;				/* 0x0122 電流AD：CH0ラッチデータ(Bit16-23)						*/
	VUSHORT	AREG_AFVLH;				/* 0x0124 電流AD：CH1ラッチデータ(Bit16-23)						*/
	VUSHORT	AREG_ACMC4;				/* 0x0126 指令マルチプレクサチャネル4							*/
	VUSHORT	AREG_NCTCTM;			/* 0x0128 指令用NCT：タイマカウンタ値							*/
	VUSHORT	AREG_NCTCFB;			/* 0x012A 指令用NCT：FBカウンタ値								*/
	VUSHORT	AREG_NCTCCD;			/* 0x012C 指令用NCT：CDカウンタ値								*/
	VUSHORT	AREG_NCTCTP;			/* 0x012E 指令用NCT：TPカウンタ値								*/
	VUSHORT	AREG_SP_130;			/* 0x0130 シリアルPG-パルス変換設定H(テスト用)					*/
	VUSHORT	AREG_HINT1;				/* 0x0132 演算部割込みポート1									*/
	VUSHORT	AREG_HINT2;				/* 0x0134 演算部割込みポート2									*/
	VUSHORT	AREG_ACMC5;				/* 0x0136 指令マルチプレクサチャネル5							*/
	VUSHORT	AREG_SRESET;			/* 0x0138 ソフトリセット										*/
	VUSHORT	AREG_NCTRSTL;			/* 0x013A NCTリセット											*/
	VUSHORT	AREG_ACMC6;				/* 0x013C 指令マルチプレクサチャネル6							*/
	VUSHORT	AREG_ACMC7;				/* 0x013E 指令マルチプレクサチャネル7							*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	AREG_SE1_TXD0;			/* 0x0140 シリアルエンコーダCH1送信データ0						*/
	VUSHORT	AREG_SE1_TXD1;			/* 0x0142 シリアルエンコーダCH1送信データ1						*/
	VUSHORT	AREG_SE1_TXD2;			/* 0x0144 シリアルエンコーダCH1送信データ2						*/
	VUSHORT	AREG_SE1_TXD3;			/* 0x0146 シリアルエンコーダCH1送信データ3						*/
	VUSHORT	AREG_SE1_TXD4;			/* 0x0148 シリアルエンコーダCH1送信データ4						*/
	VUSHORT	AREG_SE1_TXD5;			/* 0x014A シリアルエンコーダCH1送信データ5						*/
	VUSHORT	AREG_SE1_TXD6;			/* 0x014C シリアルエンコーダCH1送信データ6						*/
	VUSHORT	AREG_SE1_TXD7;			/* 0x014E シリアルエンコーダCH1送信データ7						*/
	VUSHORT	AREG_SE1_RXD0;			/* 0x0150 シリアルエンコーダCH1受信データ0						*/
	VUSHORT	AREG_SE1_RXD1;			/* 0x0152 シリアルエンコーダCH1受信データ1						*/
	VUSHORT	AREG_SE1_RXD2;			/* 0x0154 シリアルエンコーダCH1受信データ2						*/
	VUSHORT	AREG_SE1_RXD3;			/* 0x0156 シリアルエンコーダCH1受信データ3						*/
	VUSHORT	AREG_SE1_RXD4;			/* 0x0158 シリアルエンコーダCH1受信データ4						*/
	VUSHORT	AREG_SE1_RXD5;			/* 0x015A シリアルエンコーダCH1受信データ5						*/
	VUSHORT	AREG_SE1_RXD6;			/* 0x015C シリアルエンコーダCH1受信データ6						*/
	VUSHORT	AREG_SE1_RXD7;			/* 0x015E シリアルエンコーダCH1受信データ7						*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	AREG_STIMLT0;			/* 0x0160 ラッチタイマラッチ値0									*/
	VUSHORT	AREG_STIMLT1;			/* 0x0162 ラッチタイマラッチ値1									*/
	VUSHORT	AREG_STIMLT2;			/* 0x0164 ラッチタイマラッチ値2									*/
	VUSHORT	AREG_STIMLT3;			/* 0x0166 ラッチタイマラッチ値3									*/
	VUSHORT	AREG_STIMLT4;			/* 0x0168 ラッチタイマラッチ値4									*/
	VUSHORT	AREG_STIMSET;			/* 0x016A ラッチタイマ設定										*/
	VUSHORT	AREG_HPDO;				/* 0x016C 拡張I/O出力データ										*/
	VUSHORT	AREG_HPDI;				/* 0x016E 拡張I/O入力データ										*/
	VUSHORT	AREG_SADAT_A;			/* 0x0170 エンコーダADデータA									*/
	VUSHORT	AREG_SADAT_AA;			/* 0x0172 エンコーダADデータAA									*/
	VUSHORT	AREG_SADAT_B;			/* 0x0174 エンコーダADデータB									*/
	VUSHORT	AREG_SADAT_BB;			/* 0x0176 エンコーダADデータBB									*/
	VUSHORT	AREG_EIFSET1;			/* 0x0178 エンコーダADIF設定1									*/
	VUSHORT	AREG_EIFSET2;			/* 0x017A エンコーダADIF設定2									*/
	VUSHORT	AREG_SRCNT0;			/* 0x017C シリアルエンコーダIF0伝送周期設定						*/
	VUSHORT	AREG_SRCNT1;			/* 0x017E シリアルエンコーダIF1伝送周期設定						*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	AREG_FBZRND;			/* 0x0180 フィードバックカウンタZRNラッチデータ					*/
	VUSHORT	AREG_FBCLT;				/* 0x0182 フィードバックカウンタロードラッチデータ				*/
	VUSHORT	AREG_FBPUD;				/* 0x0184 フィードバックカウンタPUラッチデータ					*/
	VUSHORT	AREG_FBPCD;				/* 0x0186 フィードバックカウンタPCラッチデータ					*/
	VUSHORT	AREG_TCNT;				/* 0x0188 フィードバックタイマ現在値							*/
	VUSHORT	AREG_T1LT;				/* 0x018A フィードバックタイマA/Bパルスラッチ値					*/
	VUSHORT	AREG_T2LT;				/* 0x018C フィードバックタイマキャリアラッチ値					*/
	VUSHORT	AREG_T3LT;				/* 0x018E フィードバックタイマT1直前値ラッチ値					*/
	VUSHORT	AREG_FCCFZRNL;			/* 0x0190 フルクローズドカウンタZRNラッチデータ					*/
	VUSHORT	AREG_FCLDL;				/* 0x0192 フルクローズドカウンタロードラッチデータ				*/
	VUSHORT	AREG_DCTRIMH;			/* 0x0194 分周出力原点補正設定(Bit16-23)						*/
	VUSHORT	AREG_DCNTVH;			/* 0x0196 分周カウンタ設定値(Bit16-23)							*/
	VUSHORT	AREG_MPTCY;				/* 0x0198 M-Portスレーブタイマ周期設定							*/
	VUSHORT	AREG_M_TXS;				/* 0x019A スレーブ送信要因選択/割込み要因						*/
	VUSHORT	AREG_M_COM;				/* 0x019C M-Port通信制御										*/
	VUSHORT	AREG_M_STS;				/* 0x019E M-Port通信ステータス									*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	SPARE7[48];				/* 0x01A0-0x01FE Reserve										*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_UPLDA;				/* 0x0200 ASIC MicroPrg Load Pointer							*/
	VUSHORT	MREG_UPLD;				/* 0x0202 ASIC MicroPrg Load Buffer								*/
	VUSHORT	MREG_CTSTW;				/* 0x0204 制御status(READ-ONLY)									*/
	VUSHORT	MREG_CTSTR;				/* 0x0206 制御cmdport(WRITE-ONLY)								*/
	VUSHORT	MREG_HSUR;				/* 0x0208 ホスト指令ポート(WRITE-ONLY)							*/
}ASIC_HW_IF_STR;
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		ASIC JL-076B/077B μプログラムレジスタ構造体定義											*/
/*																									*/
/****************************************************************************************************/
typedef struct MICRO_CYC_OUTPUTS {
	VUSHORT	MREG_BITCMDI;			/* 0x7D40 Bit Command from Host									*/
	VUSHORT	MREG_CTRLMDI;			/* 0x7D42 Control Mode from Host								*/
	VUSHORT	MREG_ZCTRMDI;			/* 0x7D44 Zero Control Mode from Host							*/
	VUSHORT	MREG_DCVOLTA;			/* 0x7D46 弱め界磁パラメータ計算用PN電圧平均値					*/
	VUSHORT	MREG_TLMTP24IN;			/* 0x7D48 正側トルク制限値 [2^24/MaxTrq] Lo						*/
	VUSHORT	MREG_TLMTP24INH;		/* 0x7D4A 正側トルク制限値 [2^24/MaxTrq] Hi						*/
	VUSHORT	MREG_TLMTM24IN;			/* 0x7D4C 負側トルク制限値 [2^24/MaxTrq] Lo						*/
	VUSHORT	MREG_TLMTM24INH;		/* 0x7D4E 負側トルク制限値 [2^24/MaxTrq] Hi						*/
	VUSHORT	MREG_DPOSREFI;			/* 0x7D50 dPosition Reference from Host Lo						*/
	VUSHORT	MREG_DPOSREFIH;			/* 0x7D52 dPosition Reference from Host Hi						*/
	VUSHORT	MREG_SPDREFI;			/* 0x7D54 Speed Reference from Host Lo							*/
	VUSHORT	MREG_SPDREFIH;			/* 0x7D56 Speed Reference from Host Hi							*/
	VUSHORT	MREG_TRQREFI;			/* 0x7D58 Torque Reference from Host(2^24) Lo					*/
	VUSHORT	MREG_TRQREFIH;			/* 0x7D5A Torque Reference from Host(2^24) Hi					*/
	VUSHORT	MREG_TRQTBL;			/* 0x7D5C テーブル運転/外乱トルク指令 Lo						*/
	VUSHORT	MREG_TRQTBLH;			/* 0x7D5E テーブル運転/外乱トルク指令 Hi						*/
	VUSHORT	MREG_SPDFBI;			/* 0x7D60 速度FB補償入力 Lo										*/
	VUSHORT	MREG_SPDFBIH;			/* 0x7D62 速度FB補償入力 Hi										*/
	VUSHORT	MREG_KVVIN;				/* 0x7D64 電圧補正ゲイン										*/
	VUSHORT	MREG_Z_IQDISTIN;		/* 0x7D66 q軸外乱トルク入力 (for dummy write)					*/
} MICRO_CYC_OUTPUTS;
/*--------------------------------------------------------------------------------------------------*/

typedef struct MICRO_CYC_INPUTS {
	VUSHORT	MREG_BITDAT;			/* 0x7D80 Bit Data												*/
	VUSHORT	MREG_RXALMCODE;			/* 0x7D82 Alarm Code											*/
	VUSHORT	MREG_CPHDIST;			/* 0x7D84 Motor Position from C-Phase Lo						*/
	VUSHORT	MREG_CPHDISTH;			/* 0x7D86 Motor Position from C-Phase Hi						*/
	VUSHORT	MREG_MOTPOSERR;			/* 0x7D88 Position Error(32bits) Lo								*/
	VUSHORT	MREG_MOTPOSERRH;		/* 0x7D8A Position Error(32bits) Hi								*/
	VUSHORT	MREG_MONSPDFFC;			/* 0x7D8C Speed Feedforward Compensation Lo						*/
	VUSHORT	MREG_MONSPDFFCH;		/* 0x7D8E Speed Feedforward Compensation Hi						*/
	VUSHORT	MREG_TRQREFOA;			/* 0x7D90 Torque Reference Output (Average) Lo					*/
	VUSHORT	MREG_TRQREFOAH;			/* 0x7D92 Torque Reference Output (Average) Hi					*/
	VUSHORT	MREG_TRQFILOUT;			/* 0x7D94 Torque Filter Output Lo								*/
	VUSHORT	MREG_TRQFILOUTH;		/* 0x7D96 Torque Filter Output Hi								*/
	VUSHORT	MREG_MONSPDREF;			/* 0x7D98 Speed Reference (Position Loop Output) Lo				*/
	VUSHORT	MREG_MONSPDREFH;		/* 0x7D9A Speed Reference (Position Loop Output) Hi				*/
	VUSHORT	MREG_RSPMOTPOS;			/* 0x7D9C 応答タイミング時MOTPOS作成用(LONG) Lo					*/
	VUSHORT	MREG_RSPMOTPOSH;		/* 0x7D9E 応答タイミング時MOTPOS作成用(LONG) Hi					*/
	VUSHORT	MREG_MOTSPDA;			/* 0x7DA0 Average Motor Speed Lo  [2^24/OvrSpd]					*/
	VUSHORT	MREG_MOTSPDAH;			/* 0x7DA2 Average Motor Speed Hi  [2^24/OvrSpd]					*/
	VUSHORT	MREG_IDO;				/* 0x7DA4 d軸電流指令											*/
	VUSHORT	MREG_IQMON;				/* 0x7DA6 IQREFのモニタ用										*/
	VUSHORT	MREG_IQMONA;			/* 0x7DA8 IQREF(平均値)のモニタ用								*/
	VUSHORT	MREG_CTSTRW;			/* 0x7DAA 制御フラグ/ステータス									*/
} MICRO_CYC_INPUTS;
/*--------------------------------------------------------------------------------------------------*/

typedef struct ASIC_U_IF_STR {
	VUSHORT	MREG_ZEROR;				/* 0x7000 ; Zero register(Low) */
	VUSHORT	MREG_ZERORH;			/* 0x7002 ; Zero register(High) */
	VUSHORT	MREG_IDIL;				/* 0x7004 ; ACRd Integral(Low) */
	VUSHORT	MREG_IDIH;				/* 0x7006 ; ACRd Integral(High) */
	VUSHORT	MREG_IQIL;				/* 0x7008 ; ACRq Integral(Low) */
	VUSHORT	MREG_IQIH;				/* 0x700A ; ACRq Integral(High) */
	VUSHORT	MREG_VDFL;				/* 0x700C ; ACRd Filter Output(Low) */
	VUSHORT	MREG_VDFH;				/* 0x700E ; ACRd Filter Output(High) */
	VUSHORT	MREG_VQFL;				/* 0x7010 ; ACRq Filter Output(Low) */
	VUSHORT	MREG_VQFH;				/* 0x7012 ; ACRq Filter Output(High) */
	VUSHORT	MREG_PHAL;				/* 0x7014 ; Phase(Low) */
	VUSHORT	MREG_PHA;				/* 0x7016 ; Phase(High) */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_BBSETW;			/* 0x7018 ; soft BB set(BBSET) */
	VUSHORT	MREG_FLTINW;			/* 0x701A ; fault status(FLTIN) */
	VUSHORT	MREG_CTSTRW;			/* 0x701C ; controll flag/status(CTSTR,CTSTW) */
	VUSHORT	MREG_IUOF;				/* 0x701E ; A/D transfer offset for iu */
	VUSHORT	MREG_IVOF;				/* 0x7020 ; A/D transfer offset for iv */
	VUSHORT	MREG_KCU;				/* 0x7022 ; A/D transfer gain for iu */
	VUSHORT	MREG_KCV;				/* 0x7024 ; A/D transfer gain for iv */
	VUSHORT	MREG_IDO;				/* 0x7026 ; Id reference */
	VUSHORT	MREG_IQO;				/* 0x7028 ; Iq reference */
	VUSHORT	MREG_VEL;				/* 0x702A ; Velocity (omega) */
	VUSHORT	MREG_CNTSTS;			/* 0x702C ; counter status(FBCSTS) */
	VUSHORT	MREG_FCCSTMON;			/* 0x702E ; full closed counter status(FCCST) */
/*--------------------------------------------------------------------------------------------------*/
/*		counter																						*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_NCNT;				/* 0x7030 ; FB count */
	VUSHORT	MREG_DNCNT;				/* 0x7032 ; dNCNT */
/*--------------------------------------------------------------------------------------------------*/
/*		voltage FF																					*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_LDC;				/* 0x7034 ; Ld  * omega */
	VUSHORT	MREG_LQC;				/* 0x7036 ; Lq  * omega */
	VUSHORT	MREG_MAGC;				/* 0x7038 ; Phi * omega */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_SINT;				/* 0x703A ; sin theta */
	VUSHORT	MREG_COST;				/* 0x703C ; cos theta */
	VUSHORT	MREG_SINT2;				/* 0x703E ; sin(theta + 2pi/3) */
	VUSHORT	MREG_COST2;				/* 0x7040 ; cos(theta + 2pi/3) */
	VUSHORT	MREG_SINT3;				/* 0x7042 ; sin(theta - 2pi/3) */
	VUSHORT	MREG_COST3;				/* 0x7044 ; cos(theta - 2pi/3) */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_IUD;				/* 0x7046 ; iu input data (from A/D) */
	VUSHORT	MREG_IVD;				/* 0x7048 ; iv input data (from A/D) */
	VUSHORT	MREG_IDD;				/* 0x704A ; Id Input */
	VUSHORT	MREG_IQD;				/* 0x704C ; Iq Input */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_VD;				/* 0x704E ; Vd output voltage */
	VUSHORT	MREG_VQ;				/* 0x7050 ; Vq output voltage */
	VUSHORT	MREG_VDC;				/* 0x7052 ; Vd compensation data */
	VUSHORT	MREG_VQC;				/* 0x7054 ; Vq compensation data */
	VUSHORT	MREG_VU;				/* 0x7056 ; vu output data (to PWM) */
	VUSHORT	MREG_VV;				/* 0x7058 ; vv output data (to PWM) */
	VUSHORT	MREG_VW;				/* 0x705A ; vw output data (to PWM) */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_Dummy1;			/* 0x705C ; Dummy1 */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_CRFQW;				/* 0x705E ; Carrier freq Now */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_IUO;				/* 0x7060 ; Iu reference(for dead-time cmp) */
	VUSHORT	MREG_IVO;				/* 0x7062 ; Iv reference(for dead-time cmp) */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_V1;				/* 0x7064 ; V1( = SQRT(VD^2+VQ^2) ) */
	VUSHORT	MREG_VC;				/* 0x7066 ; Central voltage */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_VDREF;				/* 0x7068 ; vdref */
	VUSHORT	MREG_VQREF;				/* 0x706A ; vqref */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_Dummy2[10];		/* 0x706C - 0x707E */
/*--------------------------------------------------------------------------------------------------*/
/*		for LPF																						*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_IQO1FL;			/* 0x7080 ; LPF1 Output(Low) */
	VUSHORT	MREG_IQO1F;				/* 0x7082 ; LPF1 Output(High) */
	VUSHORT	MREG_IQOFL;				/* 0x7084 ; LPF2 Output(Low) */
	VUSHORT	MREG_IQOF;				/* 0x7086 ; LPF2 Output(High) */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_IQREF;				/* 0x7088 ; Iq Reference after limit(5/23) */
	VUSHORT	MREG_TLMTP;				/* 0x708A ; +tlim 5/23	 */
	VUSHORT	MREG_TLMTM;				/* 0x708C ; -tlim 5/23 */
/*--------------------------------------------------------------------------------------------------*/
/*		for monitor																					*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_IQMON;				/* 0x708E ; IQ monitor */
/*--------------------------------------------------------------------------------------------------*/
/*		for notch filter2 work(output previous) 													*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_IQOP;				/* 0x7090 ; Filter2 output previous(Low) */
	VUSHORT	MREG_IQOPH;				/* 0x7092 ; Filter2 output previous(High) */
/*--------------------------------------------------------------------------------------------------*/
/*		for over modulation																			*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_KM;				/* 0x7094 ; Over modulation gain/offset */
	VUSHORT	MREG_KVV;				/* 0x7096 ; AVR */
/*--------------------------------------------------------------------------------------------------*/
/*		A/D Stop Error detect																		*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_ADRST;				/* 0x7098 ;  */
/*--------------------------------------------------------------------------------------------------*/
/*		A/D Stop Error detect																		*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_ADERRCNT;			/* 0x709A ; ASIC AD Error Counter */
	VUSHORT	MREG_IRIUP;				/* 0x709C ; Previous IRIU */
	VUSHORT	MREG_IRIUCNT;			/* 0x709E ; same IRIU counter */
	VUSHORT	MREG_IRIVP;				/* 0x70A0 ; Previous IRIV */
	VUSHORT	MREG_IRIVCNT;			/* 0x70A2 ; same IRIV counter */
/*--------------------------------------------------------------------------------------------------*/
/*		reserved																					*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_WREG82;			/* 0x70A4 ; Work  Register 82 */
	VUSHORT	MREG_WREG83;			/* 0x70A6 ; Work  Register 83 */
	VUSHORT	MREG_WREG84;			/* 0x70A8 ; Work  Register 84 */
	VUSHORT	MREG_WREG85;			/* 0x70AA ; Work  Register 85 */
	VUSHORT	MREG_WREG86;			/* 0x70AC ; Work  Register 86 */
	VUSHORT	MREG_WREG87;			/* 0x70AE ; Work  Register 87 */
	VUSHORT	MREG_WREG88;			/* 0x70B0 ; Work  Register 88 */
	VUSHORT	MREG_WREG89;			/* 0x70B2 ; Work  Register 89 */
/*--------------------------------------------------------------------------------------------------*/
/*		deferential																					*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_WREG90;			/* 0x70B4 ; Work  Register 90 */
//	VDDFL	
//	IDDP	
	VUSHORT	MREG_WREG91;			/* 0x70B6 ; Work  Register 91 */
//	VDDFH	
	VUSHORT	MREG_WREG92;			/* 0x70B8 ; Work  Register 92 */
//	VQDFL	
//	IQDP	
	VUSHORT	MREG_WREG93;			/* 0x70BA ; Work  Register 93 */
//	VQDFH	
	VUSHORT	MREG_KEANGLE;			/* 0x70BC ; 機械角→電気角変換係数 */
	VUSHORT	MREG_WREG95;			/* 0x70BE ; Work  Register 95 */
	VUSHORT	MREG_IDFL;				/* 0x70C0 ; L(di/dt)フィルタ */
	VUSHORT	MREG_IDFH;				/* 0x70C2 ; */
	VUSHORT	MREG_IQFL;				/* 0x70C4 ; */
	VUSHORT	MREG_IQFH;				/* 0x70C6 ; */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_WREG100;			/* 0x70C8 ; Work  Register 100 */
	VUSHORT	MREG_WREG101;			/* 0x70CA ; Work  Register 101 */
	VUSHORT	MREG_WREG102;			/* 0x70CC ; Work  Register 102 */
	VUSHORT	MREG_WREG103;			/* 0x70CE ; Work  Register 103 */
	VUSHORT	MREG_WREG104;			/* 0x70D0 ; Work  Register 104 */
	VUSHORT	MREG_KF31;				/* 0x70D2 ; */
	VUSHORT	MREG_KF32;				/* 0x70D4 ; */
	VUSHORT	MREG_KF33;				/* 0x70D6 ; */
	VUSHORT	MREG_KF34;				/* 0x70D8 ; */
	VUSHORT	MREG_WREG109;			/* 0x70DA ; Work  Register 109	← 使ってない */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_Dummy3[6];			/* 0x70DC - 0x70E6 */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_VMAX2;				/* 0x70E8 ; 電圧指令ベクトル最大値(8192*1.27) */
	VUSHORT	MREG_V12;				/* 0x70EA ; 電圧指令ベクトル(√(VD^2+VQ^2)) */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_Dummy4[10];		/* 0x70EC - 0x70FE */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_FLG;				/* 0x7100 ; 積分飽和ﾌﾗｸﾞ */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_Dummy5;			/* 0x7102 ; */
/*--------------------------------------------------------------------------------------------------*/
/*		for INT_HOST																				*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_HTMP0;				/* 0x7104 ; HOST Interrupt Temp0(130) */
	VUSHORT	MREG_HTMP1;				/* 0x7106 ; HOST Interrupt Temp1(131) */
	VUSHORT	MREG_HTMP2;				/* 0x7108 ; HOST Interrupt Temp2(132) */
	VUSHORT	MREG_HTMP3;				/* 0x710A ; HOST Interrupt Temp3(133) */
	VUSHORT	MREG_HTMP4;				/* 0x710C ; HOST Interrupt Temp4(134) */
	VUSHORT	MREG_HTMP5;				/* 0x710E ; HOST Interrupt Temp5(135) */
	VUSHORT	MREG_HTMP6;				/* 0x7110 ; HOST Interrupt Temp6(136) */
	VUSHORT	MREG_HTMP7;				/* 0x7112 ; HOST Interrupt Temp7(137) */
	VUSHORT	MREG_HTMP8;				/* 0x7114 ; HOST Interrupt Temp8(138) */
	VUSHORT	MREG_HTMP9;				/* 0x7116 ; HOST Interrupt Temp9(139) */
/*--------------------------------------------------------------------------------------------------*/
/*		for ROUND																					*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_WREG140;			/* 0x7118 ; Work  Register 140 */
	VUSHORT	MREG_WREG141;			/* 0x711A ; Work  Register 141 */
	VUSHORT	MREG_WREG142;			/* 0x711C ; Work  Register 142 */
	VUSHORT	MREG_WREG143;			/* 0x711E ; Work  Register 143 */
	VUSHORT	MREG_WREG144;			/* 0x7120 ; Work  Register 144 */
	VUSHORT	MREG_WREG145;			/* 0x7122 ; Work  Register 145 */
	VUSHORT	MREG_WREG146;			/* 0x7124 ; Work  Register 146 */
	VUSHORT	MREG_WREG147;			/* 0x7126 ; Work  Register 147 */
	VUSHORT	MREG_WREG148;			/* 0x7128 ; Work  Register 148 */
	VUSHORT	MREG_WREG149;			/* 0x712A ; Work  Register 149 */
/*--------------------------------------------------------------------------------------------------*/
/*		for INT_AD																					*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_TMP0;				/* 0x712C ; */
	VUSHORT	MREG_TMP1;				/* 0x712E ; */
	VUSHORT	MREG_TMP2;				/* 0x7130 ; */
	VUSHORT	MREG_TMP3;				/* 0x7132 ; */
	VUSHORT	MREG_TMP4;				/* 0x7134 ; */
	VUSHORT	MREG_TMP5;				/* 0x7136 ; */
	VUSHORT	MREG_TMP6;				/* 0x7138 ; */
	VUSHORT	MREG_TMP7;				/* 0x713A ; */
	VUSHORT	MREG_TMP8;				/* 0x713C ; */
	VUSHORT	MREG_TMP9;				/* 0x713E ; */
/*--------------------------------------------------------------------------------------------------*/
/*		CPUkからの受け取りレジスタ																	*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_IUOFIN;			/* 0x7140 ; A/D transfer offset for iu */
	VUSHORT	MREG_IVOFIN;			/* 0x7142 ; A/D transfer offset for iv */
	VUSHORT	MREG_KCUIN;				/* 0x7144 ; A/D transfer gain for iu */
	VUSHORT	MREG_KCVIN;				/* 0x7146 ; A/D transfer gain for iv */
	VUSHORT	MREG_IDIN;				/* 0x7148 ; Id reference */
	VUSHORT	MREG_IQIN;				/* 0x714A ; Iq reference */
	VUSHORT	MREG_PHAIN;				/* 0x714C ; PHA input */
	VUSHORT	MREG_VELIN;				/* 0x714E ; VEL input */
/*--------------------------------------------------------------------------------------------------*/
/*		static variable																				*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_KC;				/* 0x7150 ; Current conversion Gain */
	VUSHORT	MREG_LD;				/* 0x7152 ; d axis Inductance */
	VUSHORT	MREG_LQ;				/* 0x7154 ; q axis Inductance */
	VUSHORT	MREG_MAG;				/* 0x7156 ; Magnetic flux (Phi) */
	VUSHORT	MREG_KDP;				/* 0x7158 ; d axis propotion gain (PI control) */
	VUSHORT	MREG_KQP;				/* 0x715A ; q axis propotion gain (PI control) */
	VUSHORT	MREG_KDI;				/* 0x715C ; d axis integral time (gain) */
	VUSHORT	MREG_KQI;				/* 0x715E ; q axis integral time (gain) */
	VUSHORT	MREG_VDLIM;				/* 0x7160 ; d axis integral limit */
	VUSHORT	MREG_VQLIM;				/* 0x7162 ; q axis integral limit */
	VUSHORT	MREG_KVVIN;				/* 0x7164 ; Voltage Compasation gain */
	VUSHORT	MREG_ODLVL;				/* 0x7166 ; On delay change level(97.5.26 mo) */
	VUSHORT	MREG_TFIL;				/* 0x7168 ; Filter time constant */
	VUSHORT	MREG_VMAX;				/* 0x716A ; voltage limit data (Vmax^2) */
	VUSHORT	MREG_ONDV;				/* 0x716C ; On-delay compensation */
	VUSHORT	MREG_CSW;				/* 0x716E ; Control Flag */
	VUSHORT	MREG_CRFQ;				/* 0x7170 ; carrier freq reference(change while BB) */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_Dummy6[3];			/* 0x7172 - 0x7176 */
/*--------------------------------------------------------------------------------------------------*/
/*		voltage compensation																		*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_VDRIN;				/* 0x7178 ; vdref input */
	VUSHORT	MREG_VQRIN;				/* 0x717A ; vqref input */
/*--------------------------------------------------------------------------------------------------*/
/*		notch1																						*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_KF1;				/* 0x717C */
	VUSHORT	MREG_KF2;				/* 0x717E */
	VUSHORT	MREG_KF3;				/* 0x7180 */
	VUSHORT	MREG_KF4;				/* 0x7182 */
/*--------------------------------------------------------------------------------------------------*/
/*		notch2																						*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_KF21;				/* 0x7184 */
	VUSHORT	MREG_KF22;				/* 0x7186 */
	VUSHORT	MREG_KF23;				/* 0x7188 */
	VUSHORT	MREG_KF24;				/* 0x718A */
/*--------------------------------------------------------------------------------------------------*/
/*		LPF																							*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_TLPF;				/* 0x718C ; LPF1 gain */
	VUSHORT	MREG_TLPFC;				/* 0x718E ; LPF2 gain */
/*--------------------------------------------------------------------------------------------------*/
/*		Torque Limit																				*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_TLMTPIN;			/* 0x7190 ; +tlim */
	VUSHORT	MREG_TLMTMIN;			/* 0x7192 ; -tlim */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_R1;				/* 0x7194 ; Moter resistance(r1/I_BASE*V_BASE) */
	VUSHORT	MREG_KONDV;				/* 0x7196 ; Dead-time comp. gain */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_Dummy7[2];			/* 0x7198 - 0x719A */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_L_DIDT;			/* 0x719C ; */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_FCCRST;			/* 0x719E ; FCCST Reset(for TEST) */


/*--------------------------------------------------------------------------------------------------*/
/*		for Current Observer																		*/
/*--------------------------------------------------------------------------------------------------*/
// パラメータ用
	VUSHORT	MREG_TSPL;				/* 0x71A0 ; Ts/L ( オブザーバ用パラメータ ) */
	VUSHORT	MREG_GOBS;				/* 0x71A2 ; g ( オブザーバの極 ) */
	VUSHORT	MREG_RLTS;				/* 0x71A4 ; 1-R･Ts/L ( オブザーバ用パラメータ ) */
	VUSHORT	MREG_FILOBS;			/* 0x71A6 ; フィルタゲイン */
// 計算用
	VUSHORT	MREG_LFILQL;			/* 0x71A8 ; q軸ローパスフィルタ変数 ( Low byte ) */
	VUSHORT	MREG_LFILQH;			/* 0x71AA ; q軸ローパスフィルタ変数 ( High byte ) */
	VUSHORT	MREG_HFILQL;			/* 0x71AC ; q軸ハイパスフィルタ変数 ( Low byte ) */
	VUSHORT	MREG_HFILQH;			/* 0x71AE ; q軸ハイパスフィルタ変数 ( High byte ) */
	VUSHORT	MREG_IQOBS;				/* 0x71B0 ; q軸オブザーバ出力 */
	VUSHORT	MREG_IQOBSF;			/* 0x71B2 ; q軸振動成分 */
	VUSHORT	MREG_LFILDL;			/* 0x71B4 ; d軸ローパスフィルタ変数 ( Low byte ) */
	VUSHORT	MREG_LFILDH;			/* 0x71B6 ; d軸ローパスフィルタ変数 ( High byte ) */
	VUSHORT	MREG_HFILDL;			/* 0x71B8 ; d軸ハイパスフィルタ変数 ( Low byte ) */
	VUSHORT	MREG_HFILDH;			/* 0x71BA ; d軸ハイパスフィルタ変数 ( High byte ) */
	VUSHORT	MREG_IDOBS;				/* 0x71BC ; d軸オブザーバ出力 */
	VUSHORT	MREG_IDOBSF;			/* 0x71BE ; d軸振動成分 */
	VUSHORT	MREG_DMPG;				/* 0x71C0 ; 電流オブザーバダンピングゲイン */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_Dummy8[31];		/* 0x71C2 - 0x71FE */
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		for Phase Interpolate			<V112>														*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_PHAIP;				/* 0x7200 ; 位相補間量 */
	VUSHORT	MREG_PHAIPFLG;			/* 0x7202 ; 位相補間フラグ(0：しない、1：する) */
	VUSHORT	MREG_PHAIPIN;			/* 0x7204 ; 位相補間量(CPU→ASIC受け渡し用) */
	VUSHORT	MREG_PHAIPFLGIN;		/* 0x7206 ; 位相補間フラグ(CPU→ASIC受け渡し用) */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_Dummy9[12];		/* 0x7208 - 0x721E */
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		for Encoder IF																		<V720>	*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_AMPTYPE;			/* 0x7220 ; Rotary Type or Linear Type */
	VUSHORT	MREG_Dummy10;			/* 0x7222 ; Dummy */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_RCVPOSX0;			/* 0x7224 ; Motor Encoder Position(受信位置) Low */
	VUSHORT	MREG_RCVPOSX0H;			/* 0x7226 ; Motor Encoder Position(受信位置) High */
	VUSHORT	MREG_RCVPOSX1;			/* 0x7228 ; Motor Encoder Position(受信位置) Low */
	VUSHORT	MREG_RCVPOSX1H;			/* 0x722A ; Motor Encoder Position(受信位置) High */
	VUSHORT	MREG_RCVPOSX2;			/* 0x722C ; Motor Encoder Position(受信位置) Low */
	VUSHORT	MREG_RCVPOSX2H;			/* 0x722E ; Motor Encoder Position(受信位置) High */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_MPOSSFTR;			/* 0x7230 ; Shift Data for Motor Position Cal. */
	VUSHORT	MREG_MPOSSFTX;			/* 0x7232 ; Shift Data for Motor Position Cal. */
	VUSHORT	MREG_MPOSSIGN;			/* 0x7234 ; Sign  Data for Motor Position Cal. */
	VUSHORT	MREG_DIVOUTSFT;			/* 0x7236 ; 分周出力シフト数 */
	VUSHORT	MREG_ACCERRLVL;			/* 0x7238 ; Motor Encoder Acc. Error Check Low */
	VUSHORT	MREG_ACCERRLVLH;		/* 0x723A ; Motor Encoder Acc. Error Check High */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_DIVPLS;			/* 0x723C ; 分周出力パルス Low */
	VUSHORT	MREG_DIVPLSH;			/* 0x723E ; 分周出力パルス High */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_DIVOUTGAIN;		/* 0x7240 ; 分周出力ゲイン(リニア) Low */
	VUSHORT	MREG_DIVOUTGAINH;		/* 0x7242 ; 分周出力ゲイン(リニア) High */
	VUSHORT	MREG_DIVPOS;			/* 0x7244 ; 分周出力パルス(リニア) Low */
	VUSHORT	MREG_DIVPOSH;			/* 0x7246 ; 分周出力パルス(リニア) High */
	VUSHORT	MREG_DIVREM;			/* 0x7248 ; 分周出力パルス余り(リニア) Low */
	VUSHORT	MREG_DIVREMH;			/* 0x724A ; 分周出力パルス余り(リニア) High */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_SPGFAIL;			/* 0x724C ; S-PG受信失敗許容回数 */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_BITIPRM;			/* 0x724E ; Bit Initial Parameter */
	VUSHORT	MREG_BITDAT;			/* 0x7250 ; Bit Data */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_INCPLSREQ;			/* 0x7252 ; PAO IncrePulse Output Request */
	VUSHORT	MREG_PAOSEQCMD;			/* 0x7254 ; PAO Serial Output Sequence */
	VUSHORT	MREG_POSETCMD;			/* 0x7256 ; パルス出力回路初期化要求 */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_Dummy11[4];		/* 0x7258 - 0x725E */
/*--------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------*/
/*		分周パルス出力関連：HostCPU --> Asic 定周期転送用									<V720>	*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_INCPLSREQIN;		/* 0x7260 ; PAO IncrePulse Output Request */
	VUSHORT	MREG_PAOSEQCMDIN;		/* 0x7262 ; PAO Serial Output Sequence */
	VUSHORT	MREG_POSETCMDIN;		/* 0x7264 ; パルス出力回路初期化要求 */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_DIVSETIN;			/* 0x7266 ; 分周機能設定入力 */
	VUSHORT	MREG_POSET1IN;			/* 0x7268 ; パルス変換原点補正1 */
	VUSHORT	MREG_POSET2IN;			/* 0x726A ; パルス変換原点補正2 */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_INCPLSIN;			/* 0x726C ; 初期インクレパルス Low */
	VUSHORT	MREG_INCPLSINH;			/* 0x726E ; 初期インクレパルス High */
	VUSHORT	MREG_INCREMIN;			/* 0x7270 ; 初期インクレパルス余り Low */
	VUSHORT	MREG_INCREMINH;			/* 0x7272 ; 初期インクレパルス余り High */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_ACCCNTCLRIN; 		/* 0x7274 ; 加速度チェック開始カウントクリア要求 */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_Dummy12[197];		/* 0x7276 - 0x73FE */
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		for axis q monitor				<V224>														*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_IQDISTIN;			/* 0x7400 ; q軸外乱トルク入力 */
	VUSHORT	MREG_IQDIST;			/* 0x7402 ; q軸外乱トルク */
	VUSHORT	MREG_IQMONFIL;			/* 0x7404 ; q軸電流指令モニタ(フィルタ後) */
	VUSHORT	MREG_IQOFREF;			/* 0x7406 ; q軸電流指令(外乱トルク加算後) */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_Dummy13[76];		/* 0x7408 - 0x749E */
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		for notch filter1 				<V388>														*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_IQO1L;				/* 0x74A0 ; filter1 output(Low) */
	VUSHORT	MREG_IQO1H;				/* 0x74A2 ; filter1 output(High) */
	VUSHORT	MREG_IQO1PL;			/* 0x74A4 ; filter1 output 1delay(Low) */
	VUSHORT	MREG_IQO1PH;			/* 0x74A6 ; filter1 output 1delay(High) */
	VUSHORT	MREG_IQO1PPL;			/* 0x74A8 ; filter1 output 2delay(Low) */
	VUSHORT	MREG_IQO1PPH;			/* 0x74AA ; filter1 output 2delay(High) */
	VUSHORT	MREG_IQIPL;				/* 0x74AC ; filter1 input 1delay(Low) */
	VUSHORT	MREG_IQIPH;				/* 0x74AE ; filter1 input 1delay(High) */
	VUSHORT	MREG_IQIPPL;			/* 0x74B0 ; filter1 input 2delay(Low) */
	VUSHORT	MREG_IQIPPH;			/* 0x74B2 ; filter1 input 2delay(High) */
	VUSHORT	MREG_IQO1S;				/* 0x74B4 ; filter1 output(short type) */
	VUSHORT	MREG_IQO1SH;			/* 0x74B6 ; filter1 output(short type) */
	VUSHORT	MREG_IQO1BUFL;			/* 0x74B8 ; filter1 output buffer(Low) */
	VUSHORT	MREG_IQO1BUFH;			/* 0x74BA ; filter1 output buffer(Low) */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_Dummy14[2];		/* 0x74BC - 0x74BE */
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		for notch filter2 				<V388>														*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_IQO2L;				/* 0x74C0 ; filter2 output(Low) */
	VUSHORT	MREG_IQO2H;				/* 0x74C2 ; filter2 output(High) */
	VUSHORT	MREG_IQO2PL;			/* 0x74C4 ; filter2 output 1delay(Low) */
	VUSHORT	MREG_IQO2PH;			/* 0x74C6 ; filter2 output 1delay(High) */
	VUSHORT	MREG_IQO2PPL;			/* 0x74C8 ; filter2 output 2delay(Low) */
	VUSHORT	MREG_IQO2PPH;			/* 0x74CA ; filter2 output 2delay(High) */
	VUSHORT	MREG_IQI2PL;			/* 0x74CC ; filter2 input 1delay(Low) */
	VUSHORT	MREG_IQI2PH;			/* 0x74CE ; filter2 input 1delay(High) */
	VUSHORT	MREG_IQI2PPL;			/* 0x74D0 ; filter2 input 2delay(Low) */
	VUSHORT	MREG_IQI2PPH;			/* 0x74D2 ; filter2 input 2delay(High) */
	VUSHORT	MREG_IQO2S;				/* 0x74D4 ; filter2 output(short type) */
	VUSHORT	MREG_IQO2SH;			/* 0x74D6 ; filter2 output(short type) */
	VUSHORT	MREG_IQO2BUFL;			/* 0x74D8 ; filter2 output buffer(Low) */
	VUSHORT	MREG_IQO2BUFH;			/* 0x74DA ; filter2 output buffer(Low) */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_Dummy15[2];		/* 0x74DC - 0x74DE */
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		for notch filter3 				<V388>														*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_IQO3L;				/* 0x74E0 ; filter3 output(Low) */
	VUSHORT	MREG_IQO3H;				/* 0x74E2 ; filter3 output(High) */
	VUSHORT	MREG_IQO3PL;			/* 0x74E4 ; filter3 output 1delay(Low) */
	VUSHORT	MREG_IQO3PH;			/* 0x74E6 ; filter3 output 1delay(High) */
	VUSHORT	MREG_IQO3PPL;			/* 0x74E8 ; filter3 output 2delay(Low) */
	VUSHORT	MREG_IQO3PPH;			/* 0x74EA ; filter3 output 2delay(High) */
	VUSHORT	MREG_IQI3PL;			/* 0x74EC ; filter3 input 1delay(Low) */
	VUSHORT	MREG_IQI3PH;			/* 0x74EE ; filter3 input 1delay(High) */
	VUSHORT	MREG_IQI3PPL;			/* 0x74F0 ; filter3 input 2delay(Low) */
	VUSHORT	MREG_IQI3PPH;			/* 0x74F2 ; filter3 input 2delay(High) */
	VUSHORT	MREG_IQO3S;				/* 0x74F4 ; filter3 output(short type) */
	VUSHORT	MREG_IQO3SH;			/* 0x74F6 ; filter3 output(short type) */
	VUSHORT	MREG_IQO3BUFL;			/* 0x74F8 ; filter3 output buffer(Low) */
	VUSHORT	MREG_IQO3BUFH;			/* 0x74FA ; filter3 output buffer(Low) */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_Dummy16[130];		/* 0x74FC - 0x75FE */
/*--------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------*/
/*		for Weaken Field		<V214>																*/
/*--------------------------------------------------------------------------------------------------*/
//----- CPUから渡される変数 -----
	VUSHORT	MREG_WFKPVLIN;			/* 0x7600 ; 電圧FB比例ゲイン(下位16bit) */
	VUSHORT	MREG_WFKPVHIN;			/* 0x7602 ; 電圧FB比例ゲイン(上位16bit) */
	VUSHORT	MREG_WFKIVLIN;			/* 0x7604 ; 電圧FB積分ゲイン(下位16bit) */
	VUSHORT	MREG_WFKIVHIN;			/* 0x7606 ; 電圧FB積分ゲイン(上位16bit) */
	VUSHORT	MREG_WFV1MAXIN;			/* 0x7608 ; 電圧指令制限レベル */
	VUSHORT	MREG_WFIDREFLIMIN;		/* 0x760A ; d軸電流指令リミット */

//----- CPUから初期化時に渡される変数 -----
	VUSHORT	MREG_WFINTEGLIM;		/* 0x760C 	; 電圧FB積分リミット */
	VUSHORT	MREG_Dummy17;			/* 0x760E 	; Dummy */

//----- マイクロで扱う変数 -----
	VUSHORT	MREG_WFKPVL;			/* 0x7610 ; 電圧FB比例ゲイン(下位16bit) */
	VUSHORT	MREG_WFKPVH;			/* 0x7612 ; 電圧FB比例ゲイン(上位16bit) */
	VUSHORT	MREG_WFKIVL;			/* 0x7614 ; 電圧FB積分ゲイン(下位16bit) */
	VUSHORT	MREG_WFKIVH;			/* 0x7616 ; 電圧FB積分ゲイン(上位16bit) */
	VUSHORT	MREG_WFV1MAX;			/* 0x7618 ; 電圧指令制限レベル */
	VUSHORT	MREG_WFIDREFLIM;		/* 0x761A ; d軸電流指令リミット */
	VUSHORT	MREG_WFVQMAX;			/* 0x761C ; q軸電圧指令リミット */
	VUSHORT	MREG_Dummy18;			/* 0x761E ; Dummy */
	VUSHORT	MREG_WFINTEGL;			/* 0x7620 ; 電圧FB積分値 */
	VUSHORT	MREG_WFINTEGH;			/* 0x7622 ; 電圧FB積分値 */
	VUSHORT	MREG_WFVDREF;			/* 0x7624 ; d軸電圧指令 */
	VUSHORT	MREG_WFVQREF;			/* 0x7626 ; q軸電圧指令 */
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_Dummy19[236];		/* 0x7628 - 0x77FE */
/*--------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------*/
/*		マイクロ演算部ソフトバージョン情報 : マイクロ演算部→ホストCPU転送用				<V720>	*/
/*--------------------------------------------------------------------------------------------------*/
	VUSHORT	MREG_MSWVERSION;		/* 0x7800 ; ソフトバージョン情報 */
	VUSHORT	MREG_TSTVERSION;		/* 0x7802 ; テストバージョン情報 */
	VUSHORT	MREG_YSPVERSION;		/* 0x7804 ; Ｙ仕様バージョン情報 */

}ASIC_U_IF_STR;
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*		ビットパラメータ ( 初期化時セット CPU→ASIC )												*/
/****************************************************************************************************/
typedef struct MICRO_BITIPRM
{
	USHORT	data;
} MICRO_BITIPRM;
#define		BITIPRM_SGM3ENC			BIT0	/* bit0  : Sigma-3 Encoder								*/
#define		BITIPRM_IPSPDCONTROL	BIT1	/* bit1  : IP Speed Control								*/
#define		BITIPRM_SPDICTRLKEEP	BIT2	/* bit2  : Speed Integral Control Keep					*/
#define		BITIPRM_DIVOUT			BIT3	/* bit3  : モータエンコーダ分周出力選択					*/
#define		BITIPRM_RVSDIR			BIT4	/* bit4  : 逆回転モード									*/
#define		BITIPRM_SERIALCONV		BIT5	/* bit5  : Use Serial Convert Unit						*/
#define		BITIPRM_POLESENSOR		BIT6	/* bit6  : Select Pole Sensor(1:ポールセンサあり)		*/
#define		BITIPRM_PHSUWV			BIT7	/* bit7  : パワー出力相順ＵＷＶ(1:UWV相順)				*/
#define		BITIPRM_EHSPDOBSON		BIT8	/* bit8  : 位相補償速度オブザーバ選択(1:有効)			*/
#define		BITIPRM_TUNELESSON		BIT9	/* bit9  : 調整レス選択(1:有効)							*/
#define		BITIPRM_TLSPDPHACOMP	BIT10	/* bit10 : 調整レス速度制御時位相補償(1:有効)			*/
#define		BITIPRM_FORCETLINVLD	BIT11	/* bit11 : 調整レス強制無効要求(1:強制無効)				*/
#define		BITIPRM_TLSPDDETCHNG	BIT12	/* bit12 : 調整レス速度検出切り替え設定(1:切り替えあり)	*/
#define		BITIPRM_LPASSFIL3ON		BIT13	/* bit13 : トルク補償後LPF要求 (1:トルク補償後LPFあり)	*/
#define		BITIPRM_MOTWEAKENFIELD	BIT14	/* bit14 : モータ弱め界磁選択							*/
#define		BITIPRM_CARRERCHNG		BIT15	/* bit15 : 停止時キャリア切り替え選択(1:有効)			*/



/****************************************************************************************************/
/*		ビットデータ ( 初期化時セット CPU→ASIC、スキャン処理 ASIC→CPU )							*/
/****************************************************************************************************/
typedef struct MICRO_BITDAT
{
	USHORT	data;
} MICRO_BITDAT;
#define		BITDAT_SERXERR			BIT0	/* bit0  : Senc Receive Error							*/
#define		BITDAT_CSET				BIT1	/* bit1  : Cphase Set									*/
#define		BITDAT_CPASS			BIT2	/* bit2  : Cphase Pass									*/
#define		BITDAT_ORGOK			BIT3	/* bit3  : Motor Origin OK								*/
#define		BITDAT_DIVCPASS			BIT4	/* bit4  : DivCphase Pass								*/
#define		BITDAT_CPOSLATCH		BIT5	/* bit5  : Cphase Position Latch Signal					*/
#define		BITDAT_ACCCHKENABLE		BIT6	/* bit6  : Encoder Acc. Check Enable Flag				*/
#define		BITDAT_PCONACTFLG		BIT7	/* bit7  : P制御状態フラグ								*/
#define		BITDAT_SVONSPDLMT		BIT8	/* bit8  : サーボON時速度制限中Flag						*/
#define		BITDAT_SPDCLAMP			BIT9	/* bit9  : トルク制御時速度制限中Flag					*/
#define		BITDAT_ENCDATANG		BIT10	/* bit10 : エンコーダデータ無効Flag						*/
#define		BITDAT_AC90ALM			BIT11	/* bit11 : A.C90(エンコーダ通信異常)					*/
#define		BITDAT_AC91ALM			BIT12	/* bit12 : A.C91(エンコーダ通信位置データ加速度異常)	*/
#define		BITDAT_AC20ALM			BIT13	/* bit13 : A.C20(位相誤検出)							*/
#define		BITDAT_AC21ALM			BIT14	/* bit14 : A.C21(ポールセンサ異常)						*/
#define		BITDAT_ABF3ALM			BIT15	/* bit15 : A.BF3(ScanA同期異常)							*/



/****************************************************************************************************/
/*		ビットデータ1 ( ASIC専用 )																	*/
/****************************************************************************************************/
typedef struct MICRO_BITDAT1
{
	USHORT	data;
} MICRO_BITDAT1;
#define		BITDAT1_HENKATSPDCMP	BIT0	/* bit0  : 変化位置タイマによる速度補正フラグ			*/
#define		BITDAT1_SPDMAFILFLG		BIT1	/* bit1  : モータ速度の移動平均処理用フラグ				*/
#define		BITDAT1_MODFLG			BIT2	/* bit2  : ベースタイマ速度演算実行フラグ				*/
#define		BITDAT1_LASTMODFLG		BIT3	/* bit3  : 前回ベースタイマ速度演算実行フラグ			*/
#define		BITDAT1_LASTSERXERR		BIT4	/* bit4  : Last Senc Receive Error						*/
#define		BITDAT1_PERCLRSTS		BIT5	/* bit5  : Perr Clear Status							*/
//									BIT6	/* bit6  : 												*/
//									BIT7	/* bit7  : 												*/
//									BIT8	/* bit8  : 												*/
//									BIT9	/* bit9  : 												*/
//									BIT10	/* bit10 : 												*/
//									BIT11	/* bit11 : 												*/
//									BIT12	/* bit12 : 												*/
//									BIT13	/* bit13 : 												*/
//									BIT14	/* bit14 : 												*/
//									BIT15	/* bit15 : 												*/



/****************************************************************************************************/
/*		ビットコマンド信号 ( スキャン処理 CPU→ASIC )												*/
/****************************************************************************************************/
typedef struct MICRO_BITCMD
{
	USHORT	data;
} MICRO_BITCMD;
#define		BITCMD_PCON				BIT0	/* bit0  : Pcon											*/
#define		BITCMD_MODESWON			BIT1	/* bit1  : Mode Switch On								*/
#define		BITCMD_PERCLRCMD		BIT2	/* bit2  : Perr Clear Command							*/
#define		BITCMD_PICVCLRCMD		BIT3	/* bit3  : Position Integral Control Data Clear Command	*/
#define		BITCMD_SCANAPASSREQ		BIT4	/* bit4  : Pass through ScanA process Command			*/
#define		BITCMD_TBLDRVON			BIT5	/* bit5  : Table Drive Request  (1:テーブル運転)		*/
#define		BITCMD_CLATCHCLR		BIT6	/* bit6  : Cphase Latch Clear Request  (1:C相ラッチ状態クリア)	*/
#define		BITCMD_TUNELESSREQ		BIT7	/* bit7  : Tuning Less Control Request  (1:調整レス有効)	*/
#define		BITCMD_DOBSON			BIT8	/* bit8  : 摩擦補償選択  (1:摩擦補償有効)				*/
#define		BITCMD_AUTOGAINCHNG		BIT9	/* bit9  : 自動ゲイン切り替え選択  (1:自動ゲイン切り替え)	*/
#define		BITCMD_GAINCHNGON		BIT10	/* bit10 : ゲイン切り替え要求  (1:第二ゲイン有効)		*/
#define		BITCMD_BBSVCREFCLR		BIT11	/* bit11 : BBSvcRefClr Request Flag  (1:指令クリア)		*/
#define		BITCMD_TRQFILINVLD		BIT12	/* bit12 : トルク指令フィルタ無効選択  (1:フィルタなし)	*/
#define		BITCMD_LPFINTEGINIT		BIT13	/* bit13 : 一次LPF積分初期化要求  (1:積分初期化あり)	*/
#define		BITCMD_LPASSFIL2ON		BIT14	/* bit14 : 二次LPF要求  (1:二次LPFあり)					*/
//									BIT15	/* bit15 : 												*/



/****************************************************************************************************/
/*		制御条件指定ビット定義 (MREG_CSW)															*/
/****************************************************************************************************/
typedef struct MICRO_CSW
{
	USHORT	data;
} MICRO_CSW;
#define		CSW_ICLR				BIT0	/* bit0  : 積分リミット時処理：ホールド(0)／クリア(1)	*/
#define		CSW_ISEL				BIT1	/* bit1  : Ld/Lq補償：電流FB(0)／電流指令(1)			*/
//									BIT2	/* bit2  : --Reserve--									*/
#define		CSW_OBSSEL				BIT3	/* bit3  : Current Observer Select bit					*/
#define		CSW_F1DSABL				BIT4	/* bit4  : Notch filter1 Disable						*/
#define		CSW_F2DSABL				BIT5	/* bit5  : Notch filter2 Disable						*/
#define		CSW_LPFDSABL			BIT6	/* bit6  : Low Pass filter1 Disable						*/
#define		CSW_LPFCDSABL			BIT7	/* bit7  : Low Pass filter2 Disable						*/
#define		CSW_OVMSEL1				BIT8	/* bit8  : Over modulation select(bit 8,9)				*/
#define		CSW_OVMSEL2				BIT9	/* bit9  : if bit8,9 = 10 or 11 then OVMOD2  if bit8,9 = 01 then OVMOD1	*/
#define		CSW_OVMMOD				BIT10	/* bit10 : modulation modification enable				*/
#define		CSW_V_FB				BIT11	/* bit11 : 電圧FB方式弱め界磁選択						*/
#define		CSW_DIDTSEL				BIT12	/* bit12 : L di/dt cmp select(1:di/dt,0:def)			*/
#define		CSW_V_FB2				BIT13	/* bit13 : 電圧FB方式弱め界磁選択2						*/
#define		CSW_INT_ST				BIT14	/* bit14 : 飽和時積分停止								*/
#define		CSW_F3DSABL				BIT15	/* bit15 : Notch filter3 Disable						*/



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
/*		ASIC JL-076B/077B レジスタアクセス用構造体定義												*/
/*																									*/
/****************************************************************************************************/
typedef struct  {
	ASIC_HW_IF_STR		*AsicHwReg;			/* ASIC H/Wレジスタ										*/
	ASIC_U_IF_STR		*AsicMicroReg;		/* ASIC Microレジスタ									*/
	ASIC_HW_IF_STR		*AsicComHwReg;		/* ASIC H/Wレジスタ(全軸同時アクセス)					*/
	ASIC_U_IF_STR		*AsicComMicroReg;	/* ASIC Microレジスタ(全軸同時アクセス)					*/
	MICRO_BITIPRM		MicroBitIprm;		/* MicroBitIprmレジスタ									*/
	MICRO_BITDAT		MicroBitDat;		/* MicroBitDatレジスタ									*/
	MICRO_BITCMD		MicroBitCmd;		/* MicroBitCmdレジスタ									*/
	MICRO_CSW			MicroCsw;			/* MicroCswレジスタ										*/
	BOOL				MicroRunningSts;	/* Microステータス確認用								*/
	MICRO_CYC_OUTPUTS	*uDMAOutBuffer;		/* Micro DMA Output buffer								*/
	MICRO_CYC_INPUTS	*uDMAInBuffer;		/* Micro DMA Input buffer								*/

#if defined(_VC_DEBUG_)
	USHORT				*pDummyMicroSram;	/* Dummy buffer for Micro Program download				*/
#endif /* _VC_DEBUG_ */

} ASICS;



/***************************************** end of file **********************************************/
#define _JL076REG_H // 再読み込み防止用
#endif
