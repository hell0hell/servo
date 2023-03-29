/****************************************************************************/
/*																			*/
/*																			*/
/*		 Ｕｔｉｌｉｔｙ ライブラリ ヘッダファイル							*/
/*																			*/
/*																			*/
/****************************************************************************/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/************** Copyright Yaskawa Electric Corporation **********************/
/*	【改版履歴】															*/
/*		Ver1.00																*/
/*																			*/
/*																			*/
/****************************************************************************/
#ifndef		UTILITY_H_
#define		UTILITY_H_

/* includes */
#include "Basedef.h"

extern ULONG TimerCnt;

#define UtilGetTimeDiff(time)	((TimerCnt - time)>>3)	/* unit:[ms] */
#define UtilGetCrtTime()		(TimerCnt)				/* unit:[0.25ms] */
#define UtilSetTimerCnt()		(TimerCnt++)	/* ScanB毎にカウントアップ */

/* forward declarations */
extern void *UtilMemSet(void *s,UCHAR c,LONG n);
extern void *UtilMemCpy(void *s1,void *s2,LONG n);
extern char *UtilStrCpy(char *s1, char *s2);
extern LONG UtilGetDWData(UCHAR *data_ptr);
extern SHORT UtilGetWData(UCHAR *data_ptr);
extern VOID UtilSetDWData(UCHAR *buf_ptr, LONG data);
extern VOID UtilSetWData(UCHAR *buf_ptr, SHORT data);
extern VOID UtilSetLLData(VOID *buf_ptr, VOID *data_ptr);

#endif /* UTILITY_H_ */
