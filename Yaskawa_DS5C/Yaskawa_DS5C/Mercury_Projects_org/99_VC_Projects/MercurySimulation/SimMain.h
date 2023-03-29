#ifndef	_SIM_MAIN_H /* 再読み込み防止用 */
#define	_SIM_MAIN_H /* 再読み込み防止用 */
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		SimMain.h : Simulation定義																	*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	初版	2010.04.20	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include	<windows.h>

#define		SIM_AXISNUM				1
#define		SIMULATION_SAMPLING		0.0000078125F

/* サーボ制御用定義 */
#define INTAD_EXE_TIMING	2
#define SCANA_EXE_TIMING	8
#define SCANB_EXE_TIMING	16
#define SCANC_EXE_TIMING	256
#define ROUND_EXE_TIMING	6400

/* サーボトレース定義 */
#define TRACE_BUF_SIZE		500
#define TRACE_NUMBER		3
#define TRACE_DATA_LENGTH	3 /* I/Oなし, 符号＋データ16bit */

#define TRACE_DATANUM		1000

typedef struct _TRACE_TARGET {
	UCHAR			TraceData;
	UCHAR			TraceAxis;
} TRACE_TARGET;

typedef struct _TRACE_INFO {
	TRACE_TARGET	TraceTarget[3];
	UCHAR			TriggerItem;
	UCHAR			TriggerMode;
	UCHAR			SeriesTraceMode;
	ULONG			TraceSampling;
	LONG			TriggerLevel;
	LONG			PreTrigger;
} TRACE_INFO;

typedef struct _TRACE_AXIS_TABLE {
	UCHAR		Index;
	LPCTSTR		AxisNumber;
} TRACE_AXIS_TABLE;

TRACE_AXIS_TABLE TraceAxisTable[] = {
	{ 0x00, TEXT("1") },
	{ 0x01, TEXT("2") },
	{ 0x02, TEXT("3") },
	{ 0x03, TEXT("4") },
	{ 0x04, TEXT("5") },
	{ 0x05, TEXT("6") },
	{ 0x06, TEXT("7") },
	{ 0x07, TEXT("8") },
};


typedef struct _TRACE_TABLE {
	UCHAR		Index;
	LPCTSTR		TargetName;
} TRACE_TABLE;

TRACE_TABLE TraceItemTable[] = {
	{ 0x01, TEXT("トルク/推力指令") },
	{ 0x02, TEXT("フィードバック速度") },
	{ 0x03, TEXT("指令速度") },
	{ 0x04, TEXT("位置指令速度") },
	{ 0x05, TEXT("位置偏差") },
	{ 0x16, TEXT("位置アンプ偏差") },
	{ 0x17, TEXT("モータ負荷位置間偏差") },
	{ 0x18, TEXT("速度フィードフォワード") },
	{ 0x19, TEXT("トルク/推力フィードフォワード") },
};
#define		TRACE_ITEM_NUMBER	(sizeof TraceItemTable / sizeof TraceItemTable[0])

typedef struct _TRACE_GAIN_TABLE {
	double		GainValue;
	LPCTSTR		GainView;
} TRACE_GAIN_TABLE;

TRACE_GAIN_TABLE TraceGainTable[] = {
	{ 0.0001,	TEXT("0.0001") },
	{ 0.001,	TEXT("0.001") },
	{ 0.01,		TEXT("0.01") },
	{ 0.1,		TEXT("0.1") },
	{ 1.0,		TEXT("1.0") },
	{ 10,		TEXT("10") },
	{ 100,		TEXT("100") },
	{ 1000,		TEXT("1000") },
	{ 10000,	TEXT("10000") },
};
#define		TRACE_GAIN_NUMBER	(sizeof TraceGainTable / sizeof TraceGainTable[0])



#endif	/* _SIM_MAIN_H */
/***************************************** end of file **********************************************/
