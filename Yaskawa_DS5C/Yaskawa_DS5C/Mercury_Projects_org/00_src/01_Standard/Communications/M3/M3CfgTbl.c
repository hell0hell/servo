/****************************************************************************
Description	: 通信設定定義
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : Mercury

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
I.M.   2010.10.18     created

****************************************************************************/
#include "Basedef.h"
#include "RegTable.h"
#include "ComCfg.h"


/****************************************************************************
 対応伝送周期情報定義
****************************************************************************/
#define TIM_TBL_NUM			12
const CFG_TIM_TBL CfgTimTbl[TIM_TBL_NUM] = {
	{500,1,1},		/* 125us,250us,250us */
	{1000,1,1},		/* 250us,250us,250us */
	{2000,1,1},		/* 500us,500us,250us */
	{3000,1,1},		/* 750us,750us,250us */
	{4000,1,1},		/* 1000us,1000us,250us */
	{6000,1,1},		/* 1500us,1500us,250us */
	{8000,1,1},		/* 2000us,2000us,250us */
	{10000,1,1},	/* 2500us,2500us,250us */
	{12000,1,1},	/* 3000us,3000us,250us */
	{14000,1,1},	/* 3500us,3500us,250us */
	{16000,1,1},	/* 4000us,4000us,250us */
	{0xFFFF,1,1},	/* Table End */
};

/****************************************************************************
 通信IF型式定義
****************************************************************************//* <S155> */
UCHAR COM_MODEL_STR[24] =
{"MECHATROLINK3-IF"};


#if 0
#include "alarm.h"
#include "ComAlmMngr.h"
/****************************************************************************
 MECHATROLINKアラーム/ワーニング定義テーブル	
****************************************************************************/
const NET_ALM_DEF net_alm_tbl[] = {
	{0x04A, ALM_OPTPRM_ERR,		TYP_ALM | RST_DISABLE, 0x00 | M3STD_DEV_ALM , 0x00 },/* パラメータ設定異常 */
	{0x51A, ALM_TRACE_SPD_OF,	TYP_ALM | RST_ENABLE,  0x00 | M3STD_DEV_ALM , 0x00 },/* 追従速度過大アラーム */
	{0xBFB, ALM_SCANN_STOP,		TYP_ALM | RST_DISABLE, 0x00 | M3STD_DEV_ALM , 0x00 },/* 処理時間オーバ(ScanN2) */
	{0xB6A, ALM_COMLSI_ERR1,	TYP_ALM | RST_DISABLE, 0x00 | M3STD_DEV_ALM , 0x00 },/* JL-080初期化失敗 */
	{0xB6B, ALM_COMLSI_ERR2,	TYP_ALM | RST_DISABLE, 0x00 | M3STD_DEV_ALM , 0x00 },/* JL-080内部システムエラー */
	{0xE40, ALM_TCYC_ERR,		TYP_ALM | RST_ENABLE,  0x00 | M3STD_DEV_ALM , 0x00 },/* MECHATROLINK伝送周期設定異常 */
	{0xE41, ALM_CD_LEN_ERR,		TYP_ALM | RST_DISABLE, 0x00 | M3STD_DEV_ALM , 0x00 },/* MECHATROLINK通信データサイズ設定異常 */
	{0xE42, ALM_AXIS_NO_ERR,	TYP_ALM | RST_DISABLE, 0x00 | M3STD_DEV_ALM , 0x00 },/* MECHATROLINK局アドレス設定異常 */
	{0xE50, ALM_COMSYNC,		TYP_ALM | RST_ENABLE,  0xC0 | M3STD_COMM_ALM, 0x00 },/* MECHATROLINK WDT異常 */
	{0xE51, ALM_COMSYNC_TOUT,	TYP_ALM | RST_ENABLE,  0xC0 | M3STD_COMM_ALM, 0x00 },/* MECHATROLINK WDT異常(同期未完) */
	{0xE60, ALM_COM,			TYP_ALM | RST_ENABLE,  0x90 | M3STD_COMM_ALM, 0x00 },/* MECHATROLINK通信異常 */
	{0xE61, ALM_COM_INT_ERR,	TYP_ALM | RST_ENABLE,  0xB0 | M3STD_COMM_ALM, 0x00 },/* ネットワーク同期間隔異常 */
	{0xE62, ALM_COM_FCS, 		TYP_ALM | RST_ENABLE,  0x80 | M3STD_COMM_ALM, 0x00 },/* ネットワークFCSエラー */
	{0xE63, ALM_COM_GBL, 		TYP_ALM | RST_ENABLE,  0xA0 | M3STD_COMM_ALM, 0x00 },/* グローバルデータ受信エラー */
	{0xE64, ALM_MON_SLV_COM,	TYP_ALM | RST_ENABLE,  0x00 | M3STD_DEV_ALM , 0x00 },/* 他局モニタデータ受信エラー */
	{0xE6A, ALM_COM_RCV_ASYNC,	TYP_ALM | RST_ENABLE,  0x90 | M3STD_COMM_ALM , 0x00 },/* ネットワーク受信エラー（イベントドリブン通信） */
	{0xE6B, ALM_COM_TOUT_ASYNC, TYP_ALM | RST_ENABLE,  0x00 | M3STD_COMM_ALM , 0x00 },/* ネットワーク受信タイムアウト（イベントドリブン通信） */
	{0xE6C, ALM_CON_SEND_ASYNC, TYP_ALM | RST_ENABLE,  0x00 | M3STD_COMM_ALM , 0x00 },/* ネットワーク送信異常（イベントドリブン通信） */
	{0xEA0, ALM_NOANS,			TYP_ALM | RST_DISABLE, 0x00 | M3STD_DEV_ALM , 0x00 },/* サーボパック故障 */
	{0xEA1, ALM_SVINIT_ERR,		TYP_ALM | RST_DISABLE, 0x00 | M3STD_DEV_ALM , 0x00 },/* サーボ初期アクセス異常 */
	{0xEA2, ALM_SVWDC_ERR,		TYP_ALM | RST_DISABLE, 0x00 | M3STD_DEV_ALM , 0x00 },/* サーボWDC異常 */
	{0xED0, ALM_SVANS_TOUT,		TYP_ALM | RST_ENABLE,  0x00 | M3STD_DEV_ALM , 0x00 },/* サーボ未応答 */
	{0xED1, ALM_CMDEXEC,		TYP_ALM | RST_ENABLE,  0x00 | M3STD_DEV_ALM , 0x00 },/* コマンド実行未完 */

	{0x960, WRN_COM,			TYP_WRN | RST_ENABLE,  0x20 | M3STD_COMM_ALM, 0x00 },/* MECHATROLINK通信警告 */
	{0x962, WRN_COM_FCS,		TYP_WRN | RST_ENABLE,  0x10 | M3STD_COMM_ALM, 0x00 },/* FCS異常警告 */
	{0x963, WRN_COM_GBL,		TYP_WRN | RST_ENABLE,  0x30 | M3STD_COMM_ALM, 0x00 },/* グローバルデータ通信警告 */
	{0x964, WRN_SLV_COM,		TYP_WRN | RST_ENABLE,  0x00 | M3STD_COMM_ALM, 0x00 },/* 他局モニタデータ通信警告 */
	{0x96A, WRN_COM_RCV_ASYNC,	TYP_WRN | RST_ENABLE,  0x20 | M3STD_COMM_ALM, 0x00 },/* ネットワーク受信警告(イベントドリブン通信) */
	{0x94A, WRN_PRM_NO,			TYP_WRN | RST_ENABLE,  0x90 | M3STD_CMD_ALM , 0x00 },/* データ設定警告(ユーザ定数番号) */
	{0x94B, WRN_DATA_AREA,		TYP_WRN | RST_ENABLE,  0x90 | M3STD_CMD_ALM , 0x00 },/* データ設定警告(データ範囲外) */
	{0x94C, WRN_DATA_CALC,		TYP_WRN | RST_ENABLE,  0x90 | M3STD_CMD_ALM , 0x00 },/* データ設定警告(演算異常) */
	{0x94D, WRN_DATA_SIZE,		TYP_WRN | RST_ENABLE,  0x90 | M3STD_CMD_ALM , 0x00 },/* データ設定警告(データサイズ異常) */
	{0x94E, WRN_DATA_LTMOD,		TYP_WRN | RST_ENABLE,  0xA0 | M3STD_CMD_ALM , 0x00 },/* データ設定警告(ラッチモード異常) */
	{0x95A, WRN_CMD_CNDTN,		TYP_WRN | RST_ENABLE,  0xA0 | M3STD_CMD_ALM , 0x00 },/* コマンド警告(条件外) */
	{0x95B, WRN_CMD_NOSPT,		TYP_WRN | RST_ENABLE,  0x80 | M3STD_CMD_ALM , 0x00 },/* コマンド警告(未サポート) */
	{0x95C, WRN_CMD_PRM,		TYP_WRN | RST_ENABLE,  0xA0 | M3STD_CMD_ALM , 0x00 },/* コマンド警告(ユーザ定数による条件外) */
	{0x95D, WRN_CMD_CNFLCT,		TYP_WRN | RST_ENABLE,  0xA0 | M3STD_CMD_ALM , 0x00 },/* コマンド警告(コマンドの干渉) */
	{0x95E, WRN_SUBCMD,			TYP_WRN | RST_ENABLE,  0xB0 | M3STD_CMD_ALM , 0x00 },/* コマンド警告(サブコマンド不可) */
	{0x95F, WRN_FUNC_CNFLCT,	TYP_WRN | RST_ENABLE,  0x80 | M3STD_CMD_ALM , 0x00 },/* コマンド警告(ポートの干渉) */
	{0x97A, WRN_COM_PHASE,		TYP_WRN | RST_ENABLE,  0xC0 | M3STD_CMD_ALM , 0x00 },/* フェーズ異常 */
	{0x97B, WRN_DATA_CLAMP,		TYP_WRN | RST_ENABLE,  0x10 | M3STD_CMD_ALM , 0x00 },/* データ設定警告(データクランプ動作) */
	{0x97C, WRN_ACT_COP_STP,	TYP_WRN | RST_ENABLE,  0x00 | M3STD_CMD_ALM , 0x00 },/* 動作警告(協調停止動作) */
	{0xFFFF,ALM_NOANS,			TYP_WRN | RST_ENABLE,  0x00 | M3STD_DEV_ALM , 0x00 } /* end of table */
};
#endif

#if 0
/****************************************************************************
 デバイス情報定義
****************************************************************************/
const ULONG	null_data[32] 
		= {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000};
const ULONG	VenderIDCode 	= 0x00000000;			/* 01H ベンダーIDコード		(株)安川電機			*/
const ULONG	DeviceIDCode 	= 0x02240001;			/* 02H デバイスコード		Σ-Ⅴ-MDシリーズ		*/
const ULONG	MdiFileVer	 	= 0x00001000;			/* 04H 機器定義(MDI)ファイルバージョン				*/
const ULONG	ExAddress 		= 0x00000008;			/* 05H 拡張アドレス設定用(有効軸数)	(>_<)仮設定		*/
const ULONG ProfileType01 	= 0x00000010;			/* 10H プロファイルタイプ１							*/
const ULONG	stdProfileVer	= 0x00000030;			/* 11H プロファイルバージョン１						*/
const ULONG ProfileType02 	= 0x000000FF;			/* 12H プロファイルタイプ２							*/
const ULONG	m2ProfileVer	= 0x00000000;			/* 13H プロファイルバージョン２						*/
const ULONG	ukProfileType	= 0x000000FF;			/* 14H プロファイルタイプ３							*/
const ULONG	ukProfileVer	= 0x00000000;			/* 14H プロファイルバージョン３						*/
const ULONG MinTcyc 		= 25000;				/* 16H 伝送周期最小値								*/
const ULONG	MaxTcyc			= 400000;				/* 17H 伝送周期最大値								*/
const ULONG TcycGranularity = 0x00000003;			/* 18H 伝送周期刻み									*/
const ULONG MinAplcyc 		= 25000;				/* 19H 通信周期最小値								*/
const ULONG	MaxAplcyc		= 3200000;				/* 1AH 通信周期最大値								*/
const ULONG TrByteList 		= 0x0000000E;			/* 1BH 伝送バイト数（リスト）						*/
const ULONG	SupComMode		= 0x00000002;			/* 20H 通信モード対応 サイクリック ＆ メッセージ	*/
const ULONG	SupMainCmd[8]							/* 30H メインコマンド対応リスト						*/
		= {0x6000E079, 0x36F7001F, 0x00000003, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000};
const ULONG	SupSubCmd[8]							/* 38H サブコマンド対応リスト						*/
		= {0x60000061, 0x00010000, 0x00000003, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000};
const ULONG	SupComPrm[8]							/* 40H 共通パラメータ対応リスト						*/
		= {0x00001FFE, 0x000003FE, 0x000003FE, 0x000000FE, 0x000FFFFE, 0x00000000, 0x00000000, 0x00000000};



/* 初期デバイス情報テーブル */
/* (>_<)Y.Oka */
const DEV_INFO_TBL devInfoTbl[] = {
	{0x0000,	RESRVE,	4,		(void *)&null_data				},	/* Resrve														0000 - 0003 */
	{0x0001,	BINARY,	4,		(void *)&VenderIDCode			},	/* ベンダーIDコード												0004 - 0007 */
	{0x0002,	BINARY,	4,		(void *)&DeviceIDCode			},	/* デバイスコード												0008 - 000B */
	{0x0003,	BINARY,	4,		(void *)&null_data				},	/* デバイスバージョン											000C - 000F */
	{0x0004,	BINARY,	4,		(void *)&null_data				},	/* 機器定義(MDI)ファイルバージョン								0010 - 0013 */
	{0x0005,	BINARY,	4,		(void *)&ExAddress				},	/* 拡張アドレス設定(有効軸数)									0014 - 0017 */
	{0x0006,	ASCII,	32,		(void *)&null_data				},	/* シリアル番号													0018 - 0037 */
	{0x000E,	RESRVE,	8,		(void *)&null_data				},	/* Resrve														0038 - 003F */
	{0x0010,	BINARY,	4,		(void *)&ProfileType01			},	/* プロファイルタイプ１（プライマリ）							0040 - 0043 */
	{0x0011,	BINARY,	4,		(void *)&stdProfileVer			},	/* プロファイルバージョン１（プライマリ）						0044 - 0047 */
	{0x0012,	BINARY,	4,		(void *)&ProfileType02			},	/* プロファイルタイプ２											0048 - 004B */
	{0x0013,	BINARY,	4,		(void *)&m2ProfileVer			},	/* プロファイルバージョン２										004C - 004F */
	{0x0014,	BINARY,	4,		(void *)&ukProfileType			},	/* プロファイルタイプ３											0050 - 0053 */
	{0x0015,	BINARY,	4,		(void *)&ukProfileVer			},	/* プロファイルバージョン３										0054 - 0057 */
	{0x0016,	BINARY,	4,		(void *)&MinTcyc				},	/* 伝送周期最小値												0058 - 005B */
	{0x0017,	BINARY,	4,		(void *)&MaxTcyc				},	/* 伝送周期最大値												005C - 005F */
	{0x0018,	BINARY,	4,		(void *)&TcycGranularity		},	/* 伝送周期刻み（GRANULARITY）									0060 - 0063 */
	{0x0019,	BINARY,	4,		(void *)&MinAplcyc				},	/* 通信周期最小値												0064 - 0067 */
	{0x001A,	BINARY,	4,		(void *)&MaxAplcyc				},	/* 通信周期最大値												0068 - 006B */
	{0x001B,	BINARY,	4,		(void *)&TrByteList				},	/* 伝送バイト数													006C - 006F */
	{0x001C,	BINARY,	4,		(void *)&null_data				},	/* 伝送バイト数(現在設定値)										0070 - 0073 */
	{0x001D,	BINARY,	4,		(void *)&ProfileType01			},	/* プロファイルタイプ（現在選択値）								0074 - 0077 */
	{0x001E,	RESRVE,	8,		(void *)&null_data				},	/* Resrve														0078 - 007F */
	{0x0020,	BINARY,	4,		(void *)&SupComMode				},	/* 通信モード対応												0080 - 0083 */
	{0x0021,	RESRVE,	60,		(void *)&null_data				},	/* Resrve														0084 - 00BF */
	{0x0030,	ARRAY,	32,		(void *)&SupMainCmd				},	/* メインコマンド対応リスト										00C0 - 00DF */
	{0x0038,	ARRAY,	32,		(void *)&SupSubCmd				},	/* サブコマンド対応リスト										00E0 - 00FF */
	{0x0040,	ARRAY,	32,		(void *)&SupComPrm				},	/* 共通パラメータ対応リスト										0100 - 011F */
	{0x0048,	RESRVE,	224,	(void *)&null_data				},	/* Resrve														0120 - 01FF */
	{0x0080,	ASCII,	32,		(void *)&null_data				},	/* 主デバイス名称（製品形式）									0200 - 021F */
	{0x0081,	RESRVE,	32,		(void *)&null_data				},	/* Resrve														0220 - 023F */
	{0x0090,	ASCII,	32,		(void *)&null_data				},	/* サブデバイス１名称（モータ形式）								0240 - 025F */
	{0x0098,	BINARY,	4,		(void *)&null_data				},	/* サブデバイス１バージョン										0260 - 0263 */
	{0x0099,	RESRVE,	28,		(void *)&null_data				},	/* Resrve														0264 - 027F */
	{0x00A0,	ASCII,	32,		(void *)&null_data				},	/* サブデバイス２名称（外部エンコーダ形式）						0280 - 029F */
	{0x00A8,	BINARY,	4,		(void *)&null_data				},	/* サブデバイス２バージョン（外部エンコーダソフトバージョン）	02A0 - 02A3 */
	{0x00A9,	RESRVE,	28,		(void *)&null_data				},	/* Resrve														02A4 - 02BF */
	{0x00B0,	ASCII,	32,		(void *)&null_data				},	/* サブデバイス３名称											02C0 - 02DF */
	{0x00B8,	BINARY,	4,		(void *)&null_data				},	/* サブデバイス３バージョン										02E0 - 02E3 */
	{0x00B9,	RESRVE,	28,		(void *)&null_data				},	/* Resrve														02E4 - 02FF */
	{0x00C0,	ASCII,	32,		(void *)&null_data				},	/* サブデバイス４名称（Safty OptionUnitの形式）					0300 - 031F */
	{0x00C8,	BINARY,	4,		(void *)&null_data				},	/* サブデバイス４バージョン（Safty OptionUnitのｿﾌﾄﾊﾞｰｼﾞｮﾝ）		0320 - 0323 */
	{0x00C9,	RESRVE,	28,		(void *)&null_data				},	/* Resrve														0324 - 033F */
	{0x00D0,	ASCII,	32,		(void *)&null_data				},	/* サブデバイス５名称（Feedback OptinUnitの形式）				0340 - 035F */
	{0x00D8,	BINARY,	4,		(void *)&null_data				},	/* サブデバイス５バージョン（Feedback OptinUnitのｿﾌﾄﾊﾞｰｼﾞｮﾝ）	0360 - 0363 */
	{0x00D9,	RESRVE,	28,		(void *)&null_data				},	/* Resrve														0364 - 037F */
	{0x00E0,	ASCII,	32,		(void *)&null_data				},	/* サブデバイス６名称											0380 - 039F */
	{0x00E8,	BINARY,	4,		(void *)&null_data				},	/* サブデバイス６バージョン										03A0 - 03A3 */
	{0x00E9,	RESRVE,	28,		(void *)&null_data				},	/* Resrve														03A4 - 03BF */
	{0x00F0,	RESRVE,	64,		(void *)&null_data				},	/* Resrve														03C0 - 03FF */
	{0x0100,	RESRVE,	0xFC00,	(void *)&null_data				},	/* Resrve														0400 - FFFF */
	{0xFFFF,	0,		0,		(void *)&null_data				}	/* end of table																*/
};
#endif

/***************************************** end of file **********************************************/
