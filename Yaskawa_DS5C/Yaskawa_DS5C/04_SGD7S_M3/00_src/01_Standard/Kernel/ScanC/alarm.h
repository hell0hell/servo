/*****************************************************************************
Description	: Alarm Manager
-----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

-----------------------------------------------------------------------------
Changes		:
Name   Date          Description
-----------------------------------------------------------------------------
I.M.   2010.11.2     created

*****************************************************************************/
#ifndef ALARM_H
#define ALARM_H

/*--------------------------------------------------------------------------*/
/*		アラーム属性定義													*/
/*--------------------------------------------------------------------------*/
#define ALMDEF_ATTR_WARNING		0x01		/* ワーニング(1),アラーム(0)	*/
#define ALMDEF_ATTR_NOEEPROM	0x02		/* EEPROM非格納(1),EEPROM格納(0)*/
#define ALMDEF_ATTR_COMALARM	0x04		/* 通信Alarm(1),アンプAlarm(0)	*/
#define ALMDEF_ATTR_NOTRESET	0x08		/* リセット不可(1),リセット可(0)*/
#define ALMDEF_ATTR_MENCDANG	0x10		/* モータエンコーダデータ無効(1)*/
#define ALMDEF_ATTR_POSNG		0x20		/* 位置データ無効(1)			*/
/*--------------------------------------------------------------------------*/
/*		ＯＬアラーム情報定義												*/
/*--------------------------------------------------------------------------*/
#define ALMDEF_OLINFO_OLF		0x01		/* 過負荷						*/
#define ALMDEF_OLINFO_RGOL		0x02		/* 回生過負荷					*/
#define ALMDEF_OLINFO_DBOL		0x04		/* ＤＢ過負荷					*/
/*--------------------------------------------------------------------------*/
/*		アラーム拡張属性定義												*/
/*--------------------------------------------------------------------------*/
#define ALMDEF_M3ATTR_CMDALM	0x01		/* M3コマンドアラーム(1)		*/
#define ALMDEF_M3ATTR_COMALM	0x02		/* M3通信アラーム(1)			*/
#define ALMDEF_EXATTR_INIALM	0x04		/* 起動時検出アラーム(1)		*/
#define ALMDEF_M3ATTR_CODEMASK	0xF0		/* M3アラームコードマスク		*/


/*****************************************************************************
*	アラームID定義
*****************************************************************************/
typedef enum ALMID_T {
	NO_ALARM		= 0,		/* No Alarm */

	/* サーボアラームＩＤ定義 */
	ALM_EEPROM1,				/* A.020 : ユーザー定数チェックサム異常							*/
	ALM_EEPROM2,				/* A.021 : パラメータフォーマット異常							*/
	ALM_EEPROM3,				/* A.022 : システムチェックサム異常								*/
	ALM_EEPROM4,				/* A.023 : パラメータパスワード異常								*/
	ALM_BOOT1,					/* A.024 : Flash初回起動時異常(工場試験機初回用アラーム)		*//* 暗号化フォーマット対応 <S146> */
	ALM_BOOT2,					/* A.025 : ランタイムファーム部異常(ミラー起動時アラーム)		*//* 暗号化フォーマット対応 <S146> */
	ALM_VDC,					/* A.030 : 主回路検出部異常										*/
	ALM_PRM,					/* A.040 : パラメータ設定異常									*/
	ALM_DIVPRM,					/* A.041 : 分周パルス出力設定異常								*/
	ALM_PRMUNMATCH,				/* A.042 : パラメータ組合わせ異常								*/
	ALM_SEMIFULLPRM,			/* A.044 : セミクフルクパラメータ設定異常						*/
	ALM_UNMATCH,				/* A.050 : モータ/サーボ容量アンマッチ							*/
	ALM_NOTSUPPORT,				/* A.051 : 製品未サポートアラーム								*/
	ALM_PWMAMP_SLOT,			/* A.052 : PWMアンプ接続アラーム								*/
	ALM_MODULE_CHANGE,			/* A.053 : モジュール構成変更アラーム							*/
	ALM_SCALE,					/* A.080 : スケールピッチ設定異常								*/
	ALM_SVONCMD,				/* A.0B0 : サーボオン指令無効アラーム							*/
	ALM_OC,						/* A.100 : 過電流(ＯＣ)											*/
	ALM_OVERCURRENT,			/* A.101 : モータ過電流検出										*//* <S1F5> */
	ALM_RLYON_ACK,				/* A.230 : RLYONアンサー異常									*/
	ALM_RG,						/* A.300 : 回生異常												*/
	ALM_RGOL,					/* A.320 : 回生過負荷											*/
	ALM_WIR,					/* A.330 : 主回路配線エラー										*/
	/* <S21D> Start */
	ALM_CONVIO,					/* A.331 : 電源関連入力信号異常									*/
	ALM_CAPALINK,				/* A.332 : コンデンサバンク接続アンサー異常						*/
	ALM_CAPAMC,					/* A.333 : コンデンサバンクMCアンサー異常						*/
	/* <S21D> End */
	ALM_OV,						/* A.400 : 過電圧(ＯＶ)											*/
	ALM_PUV,					/* A.410 : 不足電圧												*/
	ALM_CONVFLT,				/* A.42A : コンバータ異常 for IDM								*/
	ALM_PCOV,					/* A.450 : 主回路コンデンサ過電圧								*/
	ALM_OSF1,					/* A.510 : 過速度(ＯＳ)											*/
	ALM_OSDIV,					/* A.511 : 分周パルス出力過速度									*/
	ALM_VIB,					/* A.520 : 振動アラーム(発振アラーム)							*/
	ALM_AT,						/* A.521 : オートチューニングアラーム							*/
	ALM_VELPRM,					/* A.550 : 最高速度設定異常										*/
	ALM_OLF1,					/* A.710 : 過負荷(瞬時最大負荷)									*/
	ALM_OLF2,					/* A.720 : 過負荷(連続最大負荷)									*/
	ALM_DBOL,					/* A.730 : ＤＢ過負荷											*/
	ALM_DBOL_LVL,				/* A.731 : ＤＢ過負荷											*/
	ALM_RSOL,					/* A.740 : 突入抵抗過負荷										*/
	ALM_OH,						/* A.7A0 : ヒートシンク過熱										*/
	ALM_CTRLBRD,				/* A.7A1 : 内部温度異常1（制御基板温度異常）					*//* <S0DE> */
	ALM_PWRBRD,					/* A.7A2 : 内部温度異常2（パワー基板温度異常）					*//* <S0DE> */
	ALM_INTTEMP,				/* A.7A3 : 内部温度検出部異常									*//* <S0DE> */
	ALM_CTRLBDOH,				/* A.7AA : 制御基板温度異常 for IDM								*/
	ALM_FANLOCK,				/* A.7AB : サーボパック内蔵ファンの停止							*/
	ALM_PG1,					/* A.810 : エンコーダバックアップアラーム						*/
	ALM_PG2,					/* A.820 : エンコーダサムチェックアラーム						*/
	ALM_PG3,					/* A.830 : エンコーダバッテリアラーム							*/
	ALM_PG4,					/* A.840 : エンコーダデータアラーム								*/
	ALM_PG5,					/* A.850 : エンコーダオーバスピード								*/
	ALM_PG6,					/* A.860 : エンコーダオーバヒート								*/
	ALM_SCL_ERR,				/* A.890 : エンコーダスケールエラー								*/
	ALM_MOD_ERR,				/* A.891 : エンコーダモジュールエラー							*/
	ALM_FPGFSCALE,				/* A.8A0 : フルクローズシリアルエンコーダスケール異常			*/
	ALM_FPGMODULE,				/* A.8A1 : フルクローズシリアルエンコーダモジュール異常			*/
	ALM_FPGSEN,					/* A.8A2 : フルクローズシリアルエンコーダセンサ異常(インクリメンタル)	*/
	ALM_FPGPOS,					/* A.8A3 : ＦＰＧポジション異常(アブソ)							*/
	ALM_FPGOVSPD,				/* A.8A5 : フルクローズシリアルエンコーダオーバスピード異常		*/
	ALM_FPGOVHEAT,				/* A.8A6 : フルクローズシリアルエンコーダオーバーヒート異常		*/
	ALM_VREFADERR,				/* A.B10 : 速度指令Ａ／Ｄ異常									*/
	ALM_VREFADCHK,				/* A.B11 : 速度指令Ａ／Ｄデータ異常								*/
	ALM_TREFADERR,				/* A.B20 : トルク指令Ａ／Ｄ異常									*/
	ALM_CURAD_U,				/* A.B31 : 電流検出異常１(Ｕ相)									*/
	ALM_CURAD_V,				/* A.B32 : 電流検出異常２(Ｖ相)									*/
	ALM_JL056_GUP1,				/* A.B33 : 電流検出異常３(電流検出器)							*/
	ALM_HWBB_UNSUPPORT,			/* A.B40 : ゲートドライブ異常									*/
	ALM_SAMP_OVER,				/* A.BF0 : システムアラーム０(内部プログラム処理異常)			*/
	ALM_STACK_OVF,				/* A.BF1 : システムアラーム１(内部プログラム動作異常)			*/
	ALM_UPROG,					/* A.BF2 : システムアラーム２(電流制御処理部プログラム処理異常)	*/
	ALM_SCANA_STOP,				/* A.BF3 : システムアラーム３(エンコーダI/F処理部異常)			*/
	ALM_JL056_GUP,				/* A.BF4 : システムアラーム４(CPU WDT異常)						*/
	ALM_RWF,					/* A.C10 : 暴走防止検出											*/
	ALM_PHASE,					/* A.C20 : 位相誤検出											*/
	ALM_POLESENSOR,				/* A.C21 : ポールセンサ異常										*/
	ALM_PHASEANGLE,				/* A.C22 : 位相情報不一致										*/
	ALM_POLE,					/* A.C50 : 磁極検出失敗											*/
	ALM_PFINDSTOP,				/* A.C51 : 磁極検出時オーバートラベル検出						*/
	ALM_PDET,					/* A.C52 : 磁極検出未完											*/
	ALM_PDET_MOVEOVER,			/* A.C53 : 磁極検出可動範囲超過									*/
	ALM_PDET_NG,				/* A.C54 : 磁極検出失敗2										*/
	ALM_PGCLR,					/* A.C80 : エンコーダクリア異常(マルチターンリミット設定異常)	*/
	ALM_PGCOM,					/* A.C90 : エンコーダ通信異常									*/
	ALM_PGCOM1,					/* A.C91 : エンコーダ通信位置データ加速度異常					*/
	ALM_PGCOM2,					/* A.C92 : エンコーダ通信タイマ異常								*/
	ALM_PGP,					/* A.CA0 : エンコーダパラメータ異常								*/
	ALM_PGECHO,					/* A.CB0 : エンコーダエコーバック異常							*/
	ALM_MTLMT,					/* A.CC0 : マルチターンリミット値不一致							*/
	ALM_FPGCOM,					/* A.CF1 : フィードバックオプションカード通信異常(受信失敗)		*/
	ALM_FPGCOMTIMER,			/* A.CF2 : フィードバックオプションカード通信異常(タイマ停止)	*/
	ALM_OF,						/* A.D00 : 位置偏差過大											*/
	ALM_BEOF,					/* A.D01 : サーボオン時位置偏差過大								*/
	ALM_BEVLMTOF,				/* A.D02 : サーボオン時速度制限位置偏差過大						*/
	ALM_FSOF,					/* A.D10 : モータ－負荷位置間偏差過大							*/
	ALM_POSOF,					/* A.D30 : 位置データオーバー									*/
	ALM_OPSET,					/* A.E00 : オプション同期アクセス要求受信タイムアウト			*/
	ALM_TIMEOUT,				/* A.E01 : オプション運転開始要求受信タイムアウト				*/
	ALM_WDC_ERR,				/* A.E02 : WDC＋SyncFlag同期異常								*/
	ALM_OPDETECT,				/* A.E07 : オプション基板検出エラー								*/
	ALM_DGOP_TOUT,				/* A.E08 : ＤＰＲＡＭデジオペ通信タイムアウト DigOpCom			*/
	ALM_SC_TOUT,				/* A.E09 : DPRAM S->C 通信ﾀｲﾑｱｳﾄ								*/
	ALM_CMDOP_DET,				/* A.E70 : 指令ｵﾌﾟｼｮﾝｶｰﾄﾞ検出失敗								*/
	ALM_SFTOP_DET,				/* A.E71 : ｾｰﾌﾃｨｵﾌﾟｼｮﾝｶｰﾄﾞ検出失敗								*/
	ALM_FBKOP_DET,				/* A.E72 : ﾌｨｰﾄﾞﾊﾞｯｸｵﾌﾟｼｮﾝｶｰﾄﾞ検出失敗							*/
	ALM_CMDOP_NSUP,				/* A.E73 : 指令ｵﾌﾟｼｮﾝｶｰﾄﾞ未ｻﾎﾟｰﾄ								*/
	ALM_SFTOP_NSUP,				/* A.E74 : ｾｰﾌﾃｨｵﾌﾟｼｮﾝｶｰﾄﾞ未ｻﾎﾟｰﾄ								*/
	ALM_FBKOP_NSUP,				/* A.E75 : ﾌｨｰﾄﾞﾊﾞｯｸｵﾌﾟｼｮﾝｶｰﾄﾞ未ｻﾎﾟｰﾄ							*/
	ALM_CMDOP_UNMATCH,			/* A.E80 : 指令ｵﾌﾟｼｮﾝｶｰﾄﾞ検出不一致								*/
	ALM_SMC_ERR,				/* A.EB0 : ｾｰﾌﾃｨ機能Drv監視回路異常								*/
	ALM_BBON_ERR0,				/* A.EB1 : ｾｰﾌﾃｨ機能用入力信号異常								*/
	ALM_BBON_ERR1,				/* A.EB2 : ｾｰﾌﾃｨ機能Drv内部信号異常								*/
	ALM_SV_COMERR1,				/* A.EB3 : ｾｰﾌﾃｨ機能Drv通信異常１								*/
	ALM_SV_COMERR2,				/* A.EB4 : ｾｰﾌﾃｨ機能Drv通信異常２								*/
	ALM_SV_COMERR3,				/* A.EB5 : ｾｰﾌﾃｨ機能Drv通信異常３								*/
	ALM_SV_COMDATA,				/* A.EB6 : ｾｰﾌﾃｨ機能Drv通信データ異常							*/
	ALM_OP_SYSERR1,				/* A.EC0 : ｾｰﾌﾃｨｵﾌﾟｼｮﾝｶｰﾄﾞｼｽﾃﾑ異常１							*/
	ALM_OP_SYSERR2,				/* A.EC1 : ｾｰﾌﾃｨｵﾌﾟｼｮﾝｶｰﾄﾞｼｽﾃﾑ異常２							*/
	ALM_OP_PRMERR,				/* A.EC2 : ｾｰﾌﾃｨｵﾌﾟｼｮﾝｶｰﾄﾞﾊﾟﾗﾒｰﾀ設定異常						*/
	ALM_OP_MOTTYP,				/* A.EC3 : ｾｰﾌﾃｨｵﾌﾟｼｮﾝｶｰﾄﾞﾓｰﾀﾀｲﾌﾟ不一致							*/
	ALM_OP_COMERR1,				/* A.EC4 : ｾｰﾌﾃｨｵﾌﾟｼｮﾝｶｰﾄﾞ通信異常１							*/
	ALM_OP_COMERR2,				/* A.EC5 : ｾｰﾌﾃｨｵﾌﾟｼｮﾝｶｰﾄﾞ通信異常２							*/
	ALM_OP_COMERR3,				/* A.EC6 : ｾｰﾌﾃｨｵﾌﾟｼｮﾝｶｰﾄﾞ通信異常３							*/
	ALM_OP_OVERRUN,				/* A.EC7 : ｾｰﾌﾃｨｵﾌﾟｼｮﾝｶｰﾄﾞ停止指令異常							*/
	ALM_DPIN1_ERR,				/* A.EC8 : ｾｰﾌﾃｨ機能Drv内部信号異常１					<S0F1>	*/
	ALM_DPIN2_ERR,				/* A.EC9 : ｾｰﾌﾃｨ機能Drv内部信号異常２					<S0F1>	*/
	ALM_PWPHASE,				/* A.F10 : 電源欠相												*/

//	ALM_MBOH,					/* A.7A1 : ユニット内部温度過熱									*/
//	ALM_CONVOH,					/* A.7A2 : コンバータ温度過熱									*/
//	ALM_IFOH,					/* A.7A3 : 制御基板温度過熱										*/

	ALM_WDC_BIT_ERR,			/* A.E0A : WDC同期異常											*/
	ALM_PRM_TRANSLATE,			/* A.BF5 : システムアラーム５(パラメータ一括転送異常)			*//* <S066> */
	ALM_I2C_DRVERR,				/* A.BF6 : システムアラーム６(EEPROMドライバ異常)				*//* <S098> */

	/*	MECHATROLINKアラームＩＤ定義	*/
	ALM_OPTPRM_ERR,				/* A.04A : オプションパラメータ設定異常							*/
	ALM_TRACE_SPD_OF,			/* A.51A : 追従速度過大アラーム									*/
	ALM_COMLSI_ERR1,			/* A.B6A : 通信ASIC設定異常										*/
	ALM_COMLSI_ERR2,			/* A.B6B : 通信ASICシステム異常									*/
	ALM_SCANN_STOP,				/* A.BFB : ScanN実行停止										*/
	ALM_TCYC_ERR,				/* A.E40 : MECHATROLINK 伝送周期設定異常						*/
	ALM_CD_LEN_ERR,				/* A.E41 : MECHATROLINK通信データサイズ設定異常					*/
	ALM_AXIS_NO_ERR,			/* A.E42 : MECHATROLINK局アドレス設定異常						*/
	ALM_COMSYNC,				/* A.E50 : ネットワーク同期エラー								*/
	ALM_COMSYNC_TOUT,			/* A.E51 : ネットワーク同期確立失敗								*/
	ALM_COM,					/* A.E60 : ネットワーク受信エラー								*/
	ALM_COM_INT_ERR,			/* A.E61 : ネットワーク同期間隔異常								*/
	ALM_COM_FCS,				/* A.E62 : ネットワークFCSエラー								*/
	ALM_COM_GBL,				/* A.E63 : グローバルデータ受信エラー							*/
	ALM_MON_SLV_COM,			/* A.E64 : 他局モニタデータ受信エラー							*/
	ALM_COM_RCV_ASYNC,			/* A.E6A : ネットワーク受信エラー（イベントドリブン通信）		*/
	ALM_COM_TOUT_ASYNC,			/* A.E6B : ネットワーク受信タイムアウト（イベントドリブン通信）	*/
	ALM_CON_SEND_ASYNC,			/* A.E6C : ネットワーク送信異常（イベントドリブン通信）			*/
	ALM_NOANS,					/* A.EA0 : サーボパック故障										*/
	ALM_SVINIT_ERR,				/* A.EA1 : サーボパック初期アクセス異常							*/
	ALM_SVWDC_ERR,				/* A.EA2 : サーボパックWDC異常									*/
	ALM_SVANS_TOUT,				/* A.ED0 : サーボパック応答タイムアウト							*/
	ALM_CMDEXEC,				/* A.ED1 : コマンド実行未完										*/

	/* サーボワーニングＩＤ定義 */
	WRN_OF,						/* A.900 : 位置偏差過大											*/
	WRN_BEOF,					/* A.901 : サーボオン時位置偏差過大								*/
	WRN_OLF,					/* A.910 : 過負荷												*/
	WRN_VIB,					/* A.911 : 振動(発振)											*/
	WRN_CTRLBRD,				/* A.912 : 内部温度警告1（制御基板温度警告）					*//* <S0DE> */
	WRN_PWRBRD,					/* A.913 : 内部温度警告2（パワー基板温度警告）					*//* <S0DE> */
	WRN_RGOLF,					/* A.920 : 回生過負荷											*/
	WRN_DBOL_LVL,				/* A.921 : ＤＢ過負荷											*/
	WRN_FANLOCK,				/* A.923 : サーボパック内蔵ファン停止							*//* <S0A3> */
	WRN_EXREG,					/* A.924 : 回生警告											*//* <S21D> */
	WRN_PGBAT,					/* A.930 : 絶対値エンコーダバッテリ異常							*/
	WRN_FPGBAT,					/* A.931 : ＦＰＧバッテリ異常									*/
	WRN_DATA,					/* A.940 : データ異常											*/
	WRN_BOOT,					/* A.941 : 電源再投入要ユーザー定数変更							*/
	WRN_RIPPLE,					/* A.942 : リップル補正情報不一致								*/
	WRN_CMD,					/* A.950 : コマンド異常											*/
	WRN_UV,						/* A.971 : 不足電圧												*/
	WRN_OPT,					/* A.9F0 : オプション異常										*/

	WRN_MBOH,					/* A.981 : ユニット内部温度過熱									*/
	WRN_CONVOH,					/* A.982 : コンバータ温度過熱									*/
	WRN_IFOH,					/* A.983 : 制御基板温度過熱										*/
	WRN_OT,						/* A.9A0 : OT信号検出ワーニング									*/

	/*	MECHATROLINKワーニングＩＤ定義 */
	WRN_PRM_NO,					/* A.94A : データ設定警告(ユーザ定数番号)						*/
	WRN_DATA_AREA,				/* A.94B : データ設定警告(データ範囲外)							*/
	WRN_DATA_CALC,				/* A.94C : データ設定警告(演算異常)								*/
	WRN_DATA_SIZE,				/* A.94D : データ設定警告(データサイズ異常)						*/
	WRN_DATA_LTMOD,				/* A.94E : データ設定警告(ラッチモード異常)						*/
	WRN_CMD_CNDTN,				/* A.95A : コマンド警告(条件外)									*/
	WRN_CMD_NOSPT,				/* A.95B : コマンド警告(未サポート)								*/
	WRN_CMD_PRM,				/* A.95C : コマンド警告(ユーザ定数による条件外)					*/
	WRN_CMD_CNFLCT,				/* A.95D : コマンド警告(コマンドの干渉) 						*/
	WRN_SUBCMD,					/* A.95E : コマンド警告(サブコマンド不可) 						*/
	WRN_FUNC_CNFLCT,			/* A.95F : コマンド警告(ポートの干渉) 							*/
	WRN_COM,					/* A.960 : MECHATROLINK通信警告									*/
	WRN_COM_FCS,				/* A.962 : FCS異常警告											*/
	WRN_COM_GBL,				/* A.963 : グローバルデータ通信警告								*/
	WRN_SLV_COM,				/* A.964 : 他局モニタデータ通信警告								*/
	WRN_COM_RCV_ASYNC,			/* A.96A : ネットワーク受信警告(イベントドリブン通信)			*/
	WRN_COM_PHASE,				/* A.97A : フェーズ異常											*/
	WRN_DATA_CLAMP,				/* A.97B : データ設定警告(データクランプ動作)					*/
	WRN_ACT_COP_STP,			/* A.97C : 動作警告（協調停止動作）								*/
	WRN_PREVENTMAINT,			/* A.9B0:予防保全ワーニング										*/ /* <S154> */

} ALMID_T;


/* アラーム定義構造体 */
typedef struct ALM_TBL_DEF {
	USHORT	AlmCode;				/* アラームコード						*/
	UCHAR	AlmAttr;				/* アラーム属性							*/
	UCHAR	StopMode;				/* アラーム発生時の停止方法,ＯＬ属性	*/
	UCHAR	AlmCodeOut;				/* アラームコード出力信号				*/
	UCHAR	AlmLedOut;				/* アラームＬＥＤ出力信号				*/
	UCHAR	AlmAttrEx;				/* アラーム拡張属性						*/
	UCHAR	Spare;					/* Spare								*/
} ALM_TBL_DEF;
typedef const ALM_TBL_DEF*	CPALM_TBL_DEF;


/* Glogals */
extern const ALM_TBL_DEF	servo_alm_tbl[];	/* アラームテーブル			*/
extern const LONG			AlmDefTblEntNum;	/* アラームテーブル要素数	*/


#endif /* ALARM_H */
