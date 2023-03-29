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

****************************************************************************/
#ifndef COM_CFG_H_

#include "alarm.h"


/* MECHATROLINK通信コンポーネントVer */
#define COM_VER_MAIN		0x03	/* M-3対応バージョン */
#define COM_VER_MINOR		0x00	/* BCD 表記 */
#define COM_VER_TEST		0		/* 追番 */
#define COM_IF_TYPE			0x0021	/* 未使用 */
#define COM_Y_NUMBER		0
#define COM_Y_VER			0x0
#define	COM_COMPONENT_VER	((COM_VER_MAIN<<8) | COM_VER_MINOR)
/* <S155>:start */
//#define	COM_MODEL_STR		"MECHATROLINK3-IF"
//#define	OPTIF_MODEL_STR		"MECHATROLINK3-IF"
extern UCHAR COM_MODEL_STR[24];
#define COM_MODULE_VER		COM_COMPONENT_VER | (((COM_Y_NUMBER<<4) | COM_Y_VER) << 16)
/* <S155>:end */
#define COM_M3DEV_ID		0x100	/* 通信デバイスCHIP_ID */

typedef struct CFG_TIM_TBL_ {
	USHORT	SuptCycle;		/* サポート伝送周期[0.25us] */
	UCHAR	reserve;		/* 予約 */
	UCHAR	SvIfDataCyc;	/* サーボＩＦのデータ交換周期(モーション周期)[Tio_base] */
} CFG_TIM_TBL;


/****************************************************************************
* MECHATROLINKエラーコード定義
*
* Bit 00 - 07: Alam ID 
* Bit 16 - 23: CMD_ALM/COM_ALM Code 
* Bit 24 - 27: Device alarm=0, CMD alarm=1, COM alarm=2
* Bit 27 - 29: reserved
* Bit 30     : warning=1, alarm = 0
* Bit 31     : alwasy 1 
****************************************************************************/
#define ERR_DEF_BIT		0x80000000
#define ERR_WRN_BIT		0x40000000
#define ERR_COM_BIT		0x02000000
#define ERR_CMD_BIT		0x01000000
#define ERR_SVID_MASK	0x000000FF

#define ERR_ALM_COM		(ERR_DEF_BIT | ERR_COM_BIT)
#define ERR_ALM_CMD		(ERR_DEF_BIT | ERR_CMD_BIT)
#define ERR_ALM_DEV		(ERR_DEF_BIT)
#define ERR_WRN_COM		(ERR_DEF_BIT | ERR_WRN_BIT | ERR_COM_BIT)
#define ERR_WRN_CMD		(ERR_DEF_BIT | ERR_WRN_BIT | ERR_CMD_BIT)
#define ERR_WRN_DEV		(ERR_DEF_BIT | ERR_WRN_BIT)
#define ERR_RSP(a)		(a<<16)
#define GET_ERR_RSP(a)	(((UINT)(a)>>16)&0xF)


enum ERROR_NO_ {
	ERR_NONE			= 0,	/* No Error */
	ERR_WAIT_COMP		= 1,	/* Busy status */

	ERR_COM_PRM			= (INT)(ERR_ALM_DEV | ERR_RSP(0) | ALM_OPTPRM_ERR),	/* A.04A : パラメータ設定異常 */
	ERR_TRACE_SPD_OF	= (INT)(ERR_ALM_DEV | ERR_RSP(0) | ALM_TRACE_SPD_OF),	/* A.51A : 追従速度過大アラーム */
	ERR_COM_SCAN		= (INT)(ERR_ALM_DEV | ERR_RSP(0) | ALM_SCANN_STOP),	/* A.BFB : 処理時間オーバ(ScanN2) */
	ERR_COM_LSISET		= (INT)(ERR_ALM_DEV | ERR_RSP(0) | ALM_COMLSI_ERR1), 	/* A.B6A : JL-100初期化失敗 */
	ERR_COM_LSISYS		= (INT)(ERR_ALM_DEV | ERR_RSP(0) | ALM_COMLSI_ERR2), 	/* A.B6B : JL-100内部システムエラー */
	ERR_COM_TCYC		= (INT)(ERR_ALM_DEV | ERR_RSP(0) | ALM_TCYC_ERR), 		/* A.E40 : MECHATROLINK伝送周期設定異常 */
	ERR_CD_LEN_ERR		= (INT)(ERR_ALM_DEV | ERR_RSP(0) | ALM_CD_LEN_ERR), 	/* A.E41 : MECHATROLINK通信データサイズ設定異常 */
	ERR_AXIS_NO_ERR		= (INT)(ERR_ALM_DEV | ERR_RSP(0) | ALM_AXIS_NO_ERR), 	/* A.E42 : MECHATROLINK局アドレス設定異常 */
	ERR_COM_WDC			= (INT)(ERR_ALM_COM | ERR_RSP(12)| ALM_COMSYNC), 		/* A.E50 : MECHATROLINK WDT異常 */
	ERR_SYNC_TOUT		= (INT)(ERR_ALM_COM | ERR_RSP(12)| ALM_COMSYNC_TOUT), 	/* A.E51 : MECHATROLINK WDT異常(同期未完) */
	ERR_COM_RCVSTP		= (INT)(ERR_ALM_COM | ERR_RSP(9) | ALM_COM), 			/* A.E60 : MECHATROLINK通信異常 */
	ERR_GBL_SYNC		= (INT)(ERR_ALM_COM | ERR_RSP(11)| ALM_COM_INT_ERR), 	/* A.E61 : MECHATROLINK伝送周期ずれ */
	ERR_COM_FCS_A		= (INT)(ERR_ALM_COM | ERR_RSP(8) | ALM_COM_FCS), 		/* A.E62 : MECHATROLINK FCS異常 */
	ERR_GBL_COM_A		= (INT)(ERR_ALM_COM | ERR_RSP(10)| ALM_COM_GBL), 		/* A.E63 : グローバルデータ受信異常 */
	ERR_MON_SLV_COM_A	= (INT)(ERR_ALM_DEV | ERR_RSP(0) | ALM_MON_SLV_COM), 	/* A.E64 : 他局モニタデータ受信エラー */
	ERR_COM_RCV_ASYNC_A	= (INT)(ERR_ALM_COM | ERR_RSP(9) | ALM_COM_RCV_ASYNC), 	/* A.E6A : ネットワーク受信エラー（イベントドリブン通信） */
	ERR_COM_TOUT_ASYNC	= (INT)(ERR_ALM_COM | ERR_RSP(0) | ALM_COM_TOUT_ASYNC), /* A.E6B : ネットワーク受信タイムアウト（イベントドリブン通信） */
	ERR_COM_SEND_ASYNC	= (INT)(ERR_ALM_COM | ERR_RSP(0) | ALM_CON_SEND_ASYNC), /* A.E6C : ネットワーク送信異常（イベントドリブン通信） */
	ERR_SYN_REQ			= (INT)(ERR_ALM_DEV | ERR_RSP(0) | ALM_NOANS), 			/* A.EA0 : サーボパック故障 */
	ERR_SYN_FAIL		= (INT)(ERR_ALM_DEV | ERR_RSP(0) | ALM_SVINIT_ERR), 	/* A.EA1 : サーボ初期アクセス異常 */
	ERR_SV_WDC			= (INT)(ERR_ALM_DEV | ERR_RSP(0) | ALM_SVWDC_ERR), 		/* A.EA2 : サーボWDC異常 */
	ERR_SV_NOANS		= (INT)(ERR_ALM_DEV | ERR_RSP(0) | ALM_SVANS_TOUT), 	/* A.ED0 : サーボ未応答 */
	ERR_CMD_FAIL		= (INT)(ERR_ALM_DEV | ERR_RSP(0) | ALM_CMDEXEC), 		/* A.ED1 : コマンド実行未完 */

	ERR_COM_RCV			= (INT)(ERR_WRN_COM | ERR_RSP(2) | WRN_COM), 			/* A.960 : MECHATROLINK通信警告	*/
	ERR_COM_FCS_W		= (INT)(ERR_WRN_COM | ERR_RSP(1) | WRN_COM_FCS), 		/* A.962 : MECHATROLINK FCS異常警告	*/
	ERR_GBL_COM_W		= (INT)(ERR_WRN_COM | ERR_RSP(3) | WRN_COM_GBL), 		/* A.963 : グローバルデータ受信警告	*/
	ERR_MON_SLV_COM_W	= (INT)(ERR_WRN_COM | ERR_RSP(0) | WRN_SLV_COM), 		/* A.964 : 他局モニタデータ受信ワーニング	*/
	ERR_COM_RCV_ASYNC_W	= (INT)(ERR_WRN_COM | ERR_RSP(2) | WRN_COM_RCV_ASYNC), 	/* A.96A : ネットワーク受信警告（イベントドリブン通信） */
	ERR_PRM_NO			= (INT)(ERR_WRN_CMD | ERR_RSP(9) | WRN_PRM_NO), 		/* A.94A : データ設定警告(ユーザ定数番号) */
	ERR_DATA_SET		= (INT)(ERR_WRN_CMD | ERR_RSP(9) | WRN_DATA_AREA), 		/* A.94B : データ設定警告(データ範囲外)	*/
	ERR_DATA_CALC		= (INT)(ERR_WRN_CMD | ERR_RSP(9) | WRN_DATA_CALC), 		/* A.94C : データ設定警告(演算異常) */
	ERR_DATA_SZ			= (INT)(ERR_WRN_CMD | ERR_RSP(9) | WRN_DATA_SIZE), 		/* A.94D : データ設定警告(データサイズ異常) */
	ERR_DATA_LTMOD		= (INT)(ERR_WRN_CMD | ERR_RSP(10)| WRN_DATA_LTMOD), 	/* A.94E : データ設定警告(ラッチモード異常) */
	ERR_CMD_CNDTN		= (INT)(ERR_WRN_CMD | ERR_RSP(10)| WRN_CMD_CNDTN), 		/* A.95A : コマンド警告(条件外) */
	ERR_CMD_NOSUP		= (INT)(ERR_WRN_CMD | ERR_RSP(8) | WRN_CMD_NOSPT), 		/* A.95B : コマンド警告(未サポート) */
	ERR_CMD_PRM			= (INT)(ERR_WRN_CMD | ERR_RSP(10)| WRN_CMD_PRM), 		/* A.95C : コマンド警告(ユーザ定数による条件外) */
	ERR_CMD_CNFLCT		= (INT)(ERR_WRN_CMD | ERR_RSP(10)| WRN_CMD_CNFLCT), 	/* A.95D : コマンド警告(ラッチコマンドの干渉) */
	ERR_SUBCMD			= (INT)(ERR_WRN_CMD | ERR_RSP(11)| WRN_SUBCMD), 		/* A.95E : コマンド警告(メイン-サブコマンド干渉) */
	ERR_FUNC_CNFLCT		= (INT)(ERR_WRN_CMD | ERR_RSP(8) | WRN_FUNC_CNFLCT), 	/* A.95F : コマンド警告(ポートの干渉) */
	ERR_COM_PHASE		= (INT)(ERR_WRN_CMD | ERR_RSP(12)| WRN_COM_PHASE), 		/* A.97A : フェーズ異常 */
	ERR_DATA_CLAMP		= (INT)(ERR_WRN_CMD | ERR_RSP(1) | WRN_DATA_CLAMP), 	/* A.97B : データ設定警告(データクランプ動作) */
	ERR_ACT_COP_STP		= (INT)(ERR_WRN_DEV | ERR_RSP(0) | WRN_ACT_COP_STP), 	/* A.97C : 動作警告（協調停止動作）*/
};

/****************************************************************************
	MECHATROLINKエラーステータス
****************************************************************************/
/* ワーニング定義 */
#define DATA_SET_WARNG1		ERR_PRM_NO		/* A.94A データ設定警告１ */
#define DATA_SET_WARNG2		ERR_DATA_SET	/* A.94B データ設定警告２ */
#define DATA_SET_WARNG3		ERR_DATA_CALC	/* A.94C データ設定警告３ */
#define DATA_SET_WARNG4		ERR_DATA_SZ		/* A.94D データ設定警告４ */
#define DATA_SET_WARNG5		ERR_DATA_LTMOD	/* A.94E データ設定警告５ */
#define DATA_SET_WARNG6		ERR_DATA_CLAMP	/* A.97B データ設定警告６ */

#define CMD_WARNG1			ERR_CMD_CNDTN	/* A.95A コマンド警告１ */
#define CMD_WARNG2			ERR_CMD_NOSUP	/* A.95B コマンド警告２ */
#define CMD_WARNG3			ERR_CMD_PRM		/* A.95C コマンド警告３ */
#define CMD_WARNG4			ERR_CMD_CNFLCT	/* A.95D コマンド警告４ */
#define CMD_WARNG5			ERR_SUBCMD		/* A.95E コマンド警告５ */
#define CMD_WARNG6			ERR_FUNC_CNFLCT	/* A.95F コマンド警告６ */
#define CMD_WARNG7			ERR_COM_PHASE	/* A.97A コマンド警告７ */

#define CMD_SYNC_FAULT		ERR_SYNC_TOUT	/* A.E51 Phase移行状態異常 */



extern const CFG_TIM_TBL CfgTimTbl[];



#define COM_CFG_H_
#endif /* COM_CFG_H_ */
/***************************************** end of file **********************************************/
