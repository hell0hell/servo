/****************************************************************************/
/*																			*/
/*																			*/
/*		Motion Control ヘッダファイル										*/
/*																			*/
/*																			*/
/****************************************************************************/
/*	Histroy:																*/
/*		Rev2.01 2007.12.14	POSING用加速度指令追加							*/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/************** Copyright Yaskawa Electric Corporation **********************/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/****************************************************************************/
#ifndef		MOTION_CTRL_H_
#define		MOTION_CTRL_H_

//#include "DataType.h"
//#include "ComCfg.h"
//#include "System.h"
#include "MtPRM.h"
#include "MtGenerator.h"
#include "Position.h"
#include "ServoIf.h"
//#include "AccFilter.h"
#include "PRMBank.h"


/* defines */

enum MT_OTSTATE {
	OTSTAT_NORMAL=0,	/* Disable状態 */
	OTSTAT_STOP,		/* 停止中状態 */
	OTSTAT_WAITNORMAL,	/* OT解除待ち状態 */
	OTSTAT_RETRACTION,	/* 引き戻し動作状態 <V679> */
};

#if defined( CCSW_SFTID ) && ( CCSW_SFTID == SFTID_EXIST )					/*<V717>Add:Start*/
enum MT_AMSTATE {
	AMSTAT_NORMAL=0,	/* Disable状態 */
	AMSTAT_STOP,		/* 停止中状態 */
	AMSTAT_WAITNORMAL,	/* OT解除待ち状態 */
};
#endif												/*<V717>Add:End  */


#define MTLT_NO_SEQ_REQ	0	/* ラッチシーケンスなし状態 */
#define MTLT_SEQ_REQ	1	/* ラッチシーケンス起動状態 */
#define MTLT_SEQ_DONE	2	/* ラッチシーケンス完了状態 */

#define MTLT_CMP_STS	0x11	/* ラッチ完了状態 */


/* forward declarations */
/* Public functions */
extern INT32 MtcReqLatch(MTHNDL* hdl, UCHAR mode, UCHAR sgnl);
extern VOID MtcCancelLatch(MTHNDL* hdl, UCHAR mode);

/* 操作関数マクロ定義 */

/*--------------------------------------------------------------------------------------------------*/
/* ラッチモニタ番号Table <V715> 																	*/
/*--------------------------------------------------------------------------------------------------*/
extern UCHAR LtMonTbl[];


#endif /* MOTION_CTRL_H_ */
