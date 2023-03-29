/************************************************************************************/
/*																					*/
/*																					*/
/*		アラームID定義 ヘッダファイル												*/
/*																					*/
/*																					*/
/************************************************************************************/
/*																					*/
/*																					*/
/*																					*/
/*																					*/
/*																					*/
/*																					*/
/************** Copyright Yaskawa Electric Corporation ******************************/
/*																					*/
/*																					*/
/*																					*/
/*																					*/
/************************************************************************************/
#ifndef		ERR_NO_H_
#define		ERR_NO_H_

//#include "XlsAlmDef.h"

/* defines */
#define ALM_ID_MASK	0xFF		/* アラームIDマスク 							*/

#define ERR_NONE		0x0
#define ERR_WAIT_COMP	0x1

enum ERROR_NO_ {
	ERR_COM_PRM = (LONG)0x80000000,	/* A.04A : パラメータ設定異常					*//*<S0C7>*/
	ERR_COM_SCAN,					/* A.BFB : 処理時間オーバ(ScanN2)				*/
	ERR_COM_LSISET,					/* A.B6A : JL-080初期化失敗						*/
	ERR_COM_LSISYS,					/* A.B6B : JL-080内部システムエラー				*/
	ERR_COM_TCYC,					/* A.E40 : MECHATROLINK伝送周期設定異常			*/
	ERR_COM_WDC,					/* A.E50 : MECHATROLINK WDT異常					*/
	ERR_SYNC_TOUT,					/* A.E51 : MECHATROLINK WDT異常(同期未完)		*/
	ERR_COM_RCVSTP,					/* A.E60 : MECHATROLINK通信異常					*/
	ERR_GBL_SYNC,					/* A.E61 : MECHATROLINK伝送周期ずれ				*/
	ERR_SYN_REQ,					/* A.EA0 : サーボパック故障						*/
	ERR_SYN_FAIL,					/* A.EA1 : サーボ初期アクセス異常				*/
	ERR_SV_WDC,						/* A.EA2 : サーボWDC異常						*/
	ERR_SV_NOANS,					/* A.ED0 : サーボ未応答							*/
	ERR_CMD_FAIL,					/* A.ED1 : コマンド実行未完						*/

	ERR_COM_RCV,					/* A.960 : MECHATROLINK通信警告					*/
	ERR_PRM_NO,						/* A.94A : データ設定警告(ユーザ定数番号)		*/
	ERR_DATA_SET,					/* A.94B : データ設定警告(データ範囲外)			*/
	ERR_DATA_CALC,					/* A.94C : データ設定警告(演算異常)				*/
	ERR_DATA_SZ,					/* A.94D : データ設定警告(データサイズ異常)		*/
	ERR_DATA_LTMOD,					/* A.94E : データ設定警告(ラッチモード異常)		*/
	ERR_CMD_CNDTN,					/* A.95A : コマンド警告(条件外)					*/
	ERR_CMD_NOSUP,					/* A.95B : コマンド警告(未サポート)				*/
	ERR_CMD_PRM,					/* A.95C : コマンド警告(ユーザ定数による条件外)	*/
	ERR_CMD_CNFLCT,					/* A.95D : コマンド警告(ラッチコマンドの干渉)	*/
	ERR_SUBCMD,						/* A.95E : コマンド警告(メイン-サブコマンド干渉)*/
	ERR_FUNC_CNFLCT,				/* A.95F : コマンド警告(ポートの干渉)			*/
};

#endif		/*ERR_NO_H_*/

