/****************************************************************************
Description	: MECHATROLINK Register and Parameter Access Interface
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : Mercury

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------

****************************************************************************/
#ifndef _ML_REG_MANAGER_H_

#include "ComManager.h"
#include "UnitPrmStruct.h"
#include "MtPrm.h"


#define DEV_MAX_STR			32			/* 型式最大バイトサイズ		→	ID_RDのサイズ(32)に合わせる */


/* レジスタアクセスコマンドコード */
typedef enum REG_CMD_TYPE {
	REG_CMD_NONE = 0,					/* 0:No Operation						 */
	REG_CMD_PRMRD,						/* 1:レジスタリード						 */
	REG_CMD_PRMWR,						/* 2:レジスタライト						 */
	REG_CMD_RECALC,						/* 3:パラメータ再計算					 */
	REG_CMD_ALMHISTCLR,					/* 4:サーボアラーム履歴クリア			 */
	REG_CMD_SYSRST,						/* 5:システムリセット					 */
	REG_CMD_CHGCYC,						/* 6:Task周期変更によるパラメータ再計算	 */
	REG_CMD_INITPRM,					/* 7:パラメータ初期化					 */
	REG_CMD_IDRD,						/* 8:パラメータ初期化					 */
}REG_CMD_TYPE;


/* レジスタアクセス構造体 */
typedef struct REG_DATA_ {
	ULONG		Number;					/* レジスタ番号							 */
	LONG		Length;					/* レジスタ個数							 */
	LONG		AccessSize;				/* アクセスサイズ(1,2,4byte)			 */
	LONG		offset;					/* オフセット							 *//* <S**1> */
	BOOL		EepWrite;				/* EEPROMライト有効						 */
	LONG		*pBuf;					/* レジスタデータ格納先					 */
	LONG		BufferSize;				/* バッファサイズ						 */
} REG_DATA;


/* REG Manager構造体 */
typedef struct REG_HNDL_ {
	volatile REG_CMD_TYPE	accCode;	/* コマンドコード						 */
	VLONG			accSts;				/* ステータス							 */
	ULONG			accSeq;				/* レジスタアクセス実行シーケンス状態	 */
	BOOL			accWaitBusy;		/* レジスタアクセスBusy状態				 */
	REG_DATA		*accData;			/* レジスタアクセスデータ				 */
	LONG			accBuffer[32];		/* レジスタアクセス用バッファ			 */

	struct {
		MTPRM_INI	mt;					/* モーション初期パラメータデータ		 */
		UCHAR		rateChgSw;			/* Posing定数選択 						 *//* <S004> */
		UCHAR		monStop;			/* モニタ停止							 *//* <S0BD> */
	} iniPrm;							/* 初期パラメータロードデータ			 */
//	DEV_INFO		devInfo;			/* デバイス情報							 */
//	DEV_INFO_TBL	devInfoTable[DEV_TBL_SIZE];	/* デバイス情報					 */

	MT_PRM			*mtPrm;				/* モーションパラメータRAMアドレス		 */
	COM_NET_PRM		*netPrm;			/* 通信パラメータ						 */
	PRMDATA			*Prm;				/* パラメータRAMアドレス				 */
	void			*hSvRegMngr;		/* レジスタアクスセスハンドル			 */
} REG_HNDL;



#define _ML_REG_MANAGER_H_
#endif /* _ML_REG_MANAGER_H_ */


