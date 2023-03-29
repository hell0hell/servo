#ifndef _OVERHEAT_MANAGER_H
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		OverheatManager.h : 温度過熱監視処理定義							<S0ED>					*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 温度過熱監視に使用する処理を定義														*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	初版	2014.1.22	Y.Oka	For Mercury												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include	"Basedef.h"
#include	"AlarmManager.h"

/*--------------------------------------------------------------------------------------------------*/
/*		内部温度用定義																				*//* <S0DE> */
/*--------------------------------------------------------------------------------------------------*/
#define CTRL_BOARD_TMPR				0U			/* 制御回路温度										*/
#define POWER_BOARD_TMPR			1U			/* パワー回路温度									*/
#define SERXERR_THREE				2U			/* 通信エラーのカウンタが三回発生した				*/
#define CTRL_BOARD					1			/* 制御基板											*/
#define POWER_BOARD					2			/* パワー基板										*/
#define CHK_OVERHEAT_NO_OPERATION	0x0000		/* アラームなし(処理不要)							*/
#define CHK_ERROR_SET_ALARM			0x0001		/* 内部温度検出部アラーム出力						*/
#define CHK_OVERHEAT_SET_ALARM		0x0002		/* 内部温度異常アラーム出力							*/
#define CHK_OVERHEAT_SET_WARNING	0x0003		/* 内部温度ワーニング出力							*/
#define CHK_OVERHEAT_CLR_WARNING	0x0004		/* 内部温度ワーニング解除							*/



/*--------------------------------------------------------------------------------------------------*/
/*		設置環境モニタ用構造体定義												<S03A>				*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct ENVMON {
	struct	{							/* 設置環境モニタ用定数定義									*/
		SHORT	AmpEnvMonOffset;			/* アンプ設置環境モニタオフセット値						*/
		SHORT	MotEnvMonOffset;			/* モータ設置環境モニタオフセット値						*/
	} conf;
	struct	{							/* 設置環境モニタ用変数定義									*/
		USHORT	EnvMonSumCnt20ms;			/* 設置環境モニタ積算カウント(20ms測定用)				*/
		USHORT	EnvMonSumCnt2s;				/* 設置環境モニタ積算カウント(2s測定用)					*/
		LONG	AmpEnvMonSum;				/* アンプ設置環境モニタ積算値							*//* <S050> */
		LONG	MotEnvMonSum;				/* モータ設置環境モニタ積算値							*//* <S050> */
		LONG	UnInstEnvMonAmp;			/* Un025 : 設置環境モニタ(2s更新)	[0.001%]			*//* <S050> */
		LONG	UnInstEnvMonMot;			/* Un026 : 設置環境モニタ(2s更新)	[0.001%]			*//* <S050> */
	} var;
} ENVMON;


/*--------------------------------------------------------------------------------------------------*/
/*		オーバーヒート監視用構造体定義											<S03A>				*/
/*--------------------------------------------------------------------------------------------------*/
enum _enumBoardTmprState/* <S0DE> */
{
	BRD_TEMP_NORMAL		= 0L,							/* 基板温度正常								*/
	BRD_WRN_DURING		= 1L,							/* 基板警告発生中							*/
	BRD_ALARM_DURING	= 2L,							/* 基板アラーム状態中						*/
	BRD_ERROR			= 3L							/* 基板ERROR								*/
};

typedef	struct CHECK_INTTMPR {/* <S0DE> */
	struct	{
		USHORT	BrdAlmTmpr;								/* 基板異常温度[10℃]						*/
		USHORT	BrdWrnTmpr;								/* 基板警告温度[10℃]						*/
		USHORT	BrdAlmClrTmpr;							/* 基板異常クリア可能温度[10℃]				*/
		USHORT	BrdAlmDetTime;							/* 基板異常温度検出時間[10℃]				*/
	} conf;
	struct	{
		enum _enumBoardTmprState	BrdState;			/* 基板状態									*/
//		ULONG						IntTmprTimeCnt;		/* アラーム出力のためのカウンタ				*//* <S1D1> */
		ULONG						SerxErrCnt;			/* 通信エラーのカウンタ						*/
//		USHORT	DirectAlmDetTime;						/* 基板異常温度検出時間[10℃]				*//* <S1D1> */
		USHORT	WrnDetectTimer;							/* 基板警告温度検出時間[10秒]				*//* <S1D1> */
		USHORT	AlmDetectTimer;							/* 基板異常温度検出時間[10秒]				*//* <S1D1> */
	} var;
} CHECK_INTTMPR;

typedef	struct CHECK_OH {
	CHECK_INTTMPR	CtrlTmprAlm;			/* 制御基板温度異常監視用								*//* <S0DE> */
	CHECK_INTTMPR	PwrTmprAlm;				/* パワー基板温度異常監視用								*//* <S0DE> */
	struct	{
		SHORT	CtrlBoadTempSum;			/* 制御基板周囲温度演算用								*//* <S050> */
		SHORT	PowerBoadTempSum;			/* パワー基板周囲温度演算用								*//* <S050> */
		USHORT	TemperatureSumCnt;			/* オーバーヒート演算用									*/
		USHORT	TemperatureErrCnt;			/* オーバーヒート異常カウント							*/
		USHORT	TemperatureMonCnt;			/* オーバーヒートチェック経過時間カウント				*/
		USHORT	TemperatureScanCnt;			/* オーバーヒート異常スキャンカウント					*/
		LONG	DetTempFistPastTime;		/* 初期状態監視用カウンタ								*/
		BOOL	DetTempFirstPass;			/* 温度検出初回監視用フラグ								*/
	} var;
/*--------------------------------------------------------------------------------------------------*/
	struct	{
		SHORT	UnAverageTemp;				/* Un170 : 平均内気温度				[0.01deg]			*//* <S050> */
		SHORT	UnCtrlBoadTemp;				/* Un171 : 制御基板周囲温度			[0.1deg]			*//* <S050> */
		SHORT	UnPowerBoadTemp;			/* Un172 : パワー基板周囲温度		[0.1deg]			*//* <S050> */
	} UnMon;
} CHECK_OH;



/****************************************************************************************************/
/*		Global's																						*/
/****************************************************************************************************/
void	OhmInitialize( CHECK_OH* pCheckOh );
void	EnvMonInitialize( ENVMON* pEnvMon );
void	OhmCheckOverHeatAlm( ALMMAN_HANDLE *AlmMngr, CHECK_OH *CheckOh,
													SHORT CtrlBoadTemp, SHORT PowerBoadTemp );
void	AdetCheckTempDeviceErr( ALMMAN_HANDLE *AlmMngr, USHORT CtrlBoadErrCnt,
																	USHORT PowerBoadErrCnt );
void	AdetInstEnvMonitor( ENVMON *EnvMon, SHORT AmpTemp, SHORT MotTemp );



#define _OVERHEAT_MANAGER_H
#endif	// _OVERHEAT_MANAGER_H
/***************************************** end of file **********************************************/
