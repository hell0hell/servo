/****************************************************************************
Description	: Trigger I/O functions
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2012 All Rights Reserved

Project       : SGDZ-BS63/80

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
H.N.   2012.03.05     created

****************************************************************************/
#include "IngramGlobals.h"
#include "HwDeviceIf.h"

/* define */
#define HW_PLD_TRIGIO_DELAYTIME_ADDR_L	(HW_PLD_BASE_ADDRESS + 0x90001C)	/* OUTE 出力遅延時間設定レジスタL */
#define HW_PLD_TRIGIO_DELAYTIME_ADDR_H	(HW_PLD_BASE_ADDRESS + 0x90001E)	/* OUTF 出力遅延時間設定レジスタH */
#define HW_PLD_TRIGIO_OUTPIN_ADDR		(HW_PLD_BASE_ADDRESS + 0x900002)	/* OUT1 出力端子設定レジスタ */
#define HW_PLD_TRIGIO_ONTIME_ADDR		(HW_PLD_BASE_ADDRESS + 0x90000C)	/* OUT6 ON時間設定レジスタ */
#define HW_PLD_TRIGIO_STS_ADDR			(HW_PLD_BASE_ADDRESS + 0x100020)	/* IN10 状態モニタ出力レジスタ */

#define TRIG_IO_NORMAL 					0
#define TRIG_IO_MANUAL 					1
#define ENC_COM_DELAY 					131		/* エンコーダ通信遅れ[0.5us] 		 65.625us */
#define ENC_SAMPLING_TIME				125		/* 位置情報のサンプリングタイム[0.5us] 62.5us */
#define DELAYTIME_CALC_RANGE			3		/* トリガI/O出慮k時間算出エリア[62.5us] 250us */

/* function prototype */

/* Trigger I/O HAL Driver */
static void hALdrv_WriteTrigIoDelayTimeL(USHORT delayTime, USHORT tbl_no);
static void hALdrv_WriteTrigIoDelayTimeH(USHORT delayTime, USHORT tbl_no);
static void hALdrv_WriteTrigIoPinSetting(USHORT pinNo, USHORT tbl_no);
static void hALdrv_WriteTrigIoOnTime(USHORT onTime, USHORT tbl_no);
static USHORT hALdrv_ReadTrigIoSts(void);

/* Trigger I/O パラメータ・レジスタ計算関数 */
PRM_RSLT pncal_trigIoTbl1_axisNo(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue);
PRM_RSLT pncal_trigIoTbl1_onTime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue);
PRM_RSLT pncal_trigIoTbl2_axisNo(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue);
PRM_RSLT pncal_trigIoTbl2_onTime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue);
PRM_RSLT pncal_trigIoTbl3_axisNo(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue);
PRM_RSLT pncal_trigIoTbl3_onTime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue);
PRM_RSLT pncal_trigIoTbl4_axisNo(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue);
PRM_RSLT pncal_trigIoTbl4_onTime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue);
PRM_RSLT pncal_trigIoTbl5_axisNo(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue);
PRM_RSLT pncal_trigIoTbl5_onTime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue);
PRM_RSLT pncal_trigIoTbl6_axisNo(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue);
PRM_RSLT pncal_trigIoTbl6_onTime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue);
PRM_RSLT pncal_trigIoTbl7_axisNo(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue);
PRM_RSLT pncal_trigIoTbl7_onTime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue);
PRM_RSLT pncal_trigIoTbl8_axisNo(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue);
PRM_RSLT pncal_trigIoTbl8_onTime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue);

PRM_RSLT fnCalcTriggerIOStsMon(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue);
PRM_RSLT fnCalcTriggerIOControl(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue);

/* Trigger I/O アプリケーション処理 */
static LONG calcDelayTime(TRIGIO_INFO *trigWk, LONG crtPos, SHORT bufPos);
void TrigIOSetDelayTime(AXIS_HANDLE	*Axis);
void TrigIOInitialize(AXIS_HANDLE *Axis);

/****************************************************************************/
/*																			*/
/*																			*/
/*		Trigger I/O HAL Driver 定義											*/
/*																			*/
/*																			*/
/****************************************************************************/


/****************************************************************************/
/* 概要：トリガIO 出力遅延時間設定処理										*/
/****************************************************************************/
/* 引数		:	delayTime		遅延時間[us]								*/
/*			:	tbl_no			テーブル番号								*/
/* 戻り値	:	なし														*/
/****************************************************************************/
static void hALdrv_WriteTrigIoDelayTimeL(USHORT delayTime, USHORT tbl_no)
{
	*((USHORT*)(HW_PLD_TRIGIO_DELAYTIME_ADDR_L+tbl_no*0x100)) = delayTime;
}

static void hALdrv_WriteTrigIoDelayTimeH(USHORT delayTime, USHORT tbl_no)
{
	*((USHORT*)(HW_PLD_TRIGIO_DELAYTIME_ADDR_H+tbl_no*0x100)) = delayTime;
}

/****************************************************************************/
/* 概要：トリガIO 出力端子設定処理											*/
/****************************************************************************/
/* 引数		:	pinNo			ピン番号									*/
/*			:	tbl_no			テーブル番号								*/
/* 戻り値	:	なし														*/
/****************************************************************************/
static void hALdrv_WriteTrigIoPinSetting(USHORT pinNo, USHORT tbl_no)
{
	*((VUSHORT*)(HW_PLD_TRIGIO_OUTPIN_ADDR + tbl_no*0x100)) = pinNo;
}

/****************************************************************************/
/* 概要：トリガIO ON時間設定処理											*/
/****************************************************************************/
/* 引数		:	onTime			ON時間[us]									*/
/*			:	tbl_no			テーブル番号								*/
/* 戻り値	:	なし														*/
/****************************************************************************/
static void hALdrv_WriteTrigIoOnTime(USHORT onTime, USHORT tbl_no)
{
	*((VUSHORT*)(HW_PLD_TRIGIO_ONTIME_ADDR + tbl_no*0x100)) = onTime;
}

/****************************************************************************/
/* 概要：トリガIO状態モニタ取得処理											*/
/****************************************************************************/
/*			:	tbl_no			テーブル番号								*/
/* 戻り値	:	トリガIO状態モニタ出力										*/
/****************************************************************************/
static USHORT hALdrv_ReadTrigIoSts(void)
{
	return *((VUSHORT*)(HW_PLD_TRIGIO_STS_ADDR));
}

/****************************************************************************/
/*																			*/
/*																			*/
/*		Trigger I/O パラメータ・レジスタ計算関数 定義						*/
/*																			*/
/*																			*/
/****************************************************************************/

/****************************************************************************/
/*		Pn642 : トリガIOテーブル1 軸番号、出力ピン、通過方向				*/
/****************************************************************************/
PRM_RSLT pncal_trigIoTbl1_axisNo(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	USHORT wk1, wk2, i;
	TRIGIO_INFO *trigWk;	/* トリガIO設定情報 */
	
	trigWk = &Axis->TrigInfo[0];

	wk1 = trigWk->trigConfig;
	trigWk->axisNo  =   wk1 & 0x000F;
	trigWk->pinNo   =  (wk1 & 0x00F0)>>4;
	wk2 = (wk1 & 0x0F00)>>8;
	if(wk2 == 1)
	{	trigWk->passDir = -1;	}
	else
	{	trigWk->passDir = 1;	}
	hALdrv_WriteTrigIoPinSetting(trigWk->pinNo, 0);// pinNo を ServoFPGA へセット
	
	for(i=0;i<TRIG_IO_LSTPOS_MAX;i++)
	{/* 選択された軸の MotAbsPosをバッファにセット */
		trigWk->lstPos[i] = (Axis+trigWk->axisNo)->MencV->MotAbsPos;
	}
		
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************/
/*		Pn643 : トリガIOテーブル1 IO出力時間[us]							*/
/****************************************************************************/
PRM_RSLT pncal_trigIoTbl1_onTime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	hALdrv_WriteTrigIoOnTime(Axis->TrigInfo[0].onTime, 0);	// onTime を ServoFPGA へセット
	return PRM_RSLT_SUCCESS;
}

/* テーブル2 */
/****************************************************************************/
/*		Pn64A : トリガIOテーブル2 軸番号、出力ピン、通過方向				*/
/****************************************************************************/
PRM_RSLT pncal_trigIoTbl2_axisNo(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	USHORT wk1, wk2, i;
	TRIGIO_INFO *trigWk;	/* トリガIO設定情報 */
	
	trigWk = &Axis->TrigInfo[1];

	wk1 = trigWk->trigConfig;
	trigWk->axisNo  =   wk1 & 0x000F;
	trigWk->pinNo   =  (wk1 & 0x00F0)>>4;
	wk2 = (wk1 & 0x0F00)>>8;
	if(wk2 == 1)
	{	trigWk->passDir = -1;	}
	else
	{	trigWk->passDir = 1;	}
	hALdrv_WriteTrigIoPinSetting(trigWk->pinNo, 1);// pinNo を ServoFPGA へセット
	
	for(i=0;i<TRIG_IO_LSTPOS_MAX;i++)
	{/* 選択された軸の MotAbsPosをバッファにセット */
		trigWk->lstPos[i] = (Axis+trigWk->axisNo)->MencV->MotAbsPos;
	}
	
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************/
/*		Pn64B : トリガIOテーブル2 IO出力時間[us]							*/
/****************************************************************************/
PRM_RSLT pncal_trigIoTbl2_onTime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	hALdrv_WriteTrigIoOnTime(Axis->TrigInfo[1].onTime, 1);	// onTime を ServoFPGA へセット
	return PRM_RSLT_SUCCESS;
}

/* テーブル3 */
/****************************************************************************/
/*		Pn652 : トリガIOテーブル3 軸番号、出力ピン、通過方向				*/
/****************************************************************************/
PRM_RSLT pncal_trigIoTbl3_axisNo(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	USHORT wk1, wk2, i;
	TRIGIO_INFO *trigWk;	/* トリガIO設定情報 */
	
	trigWk = &Axis->TrigInfo[2];

	wk1 = trigWk->trigConfig;
	trigWk->axisNo  =   wk1 & 0x000F;
	trigWk->pinNo   =  (wk1 & 0x00F0)>>4;
	wk2 = (wk1 & 0x0F00)>>8;
	if(wk2 == 1)
	{	trigWk->passDir = -1;	}
	else
	{	trigWk->passDir = 1;	}
	hALdrv_WriteTrigIoPinSetting(trigWk->pinNo, 2);// pinNo を ServoFPGA へセット
	
	for(i=0;i<TRIG_IO_LSTPOS_MAX;i++)
	{/* 選択された軸の MotAbsPosをバッファにセット */
		trigWk->lstPos[i] = (Axis+trigWk->axisNo)->MencV->MotAbsPos;
	}
	
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************/
/*		Pn653 : トリガIOテーブル3 IO出力時間[us]							*/
/****************************************************************************/
PRM_RSLT pncal_trigIoTbl3_onTime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	hALdrv_WriteTrigIoOnTime(Axis->TrigInfo[2].onTime, 2);	// onTime を ServoFPGA へセット
	return PRM_RSLT_SUCCESS;
}

/* テーブル4 */
/****************************************************************************/
/*		Pn65A : トリガIOテーブル4 軸番号、出力ピン、通過方向				*/
/****************************************************************************/
PRM_RSLT pncal_trigIoTbl4_axisNo(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	USHORT wk1, wk2, i;
	TRIGIO_INFO *trigWk;	/* トリガIO設定情報 */
	
	trigWk = &Axis->TrigInfo[3];

	wk1 = trigWk->trigConfig;
	trigWk->axisNo  =   wk1 & 0x000F;
	trigWk->pinNo   =  (wk1 & 0x00F0)>>4;
	wk2 = (wk1 & 0x0F00)>>8;
	if(wk2 == 1)
	{	trigWk->passDir = -1;	}
	else
	{	trigWk->passDir = 1;	}
	hALdrv_WriteTrigIoPinSetting(trigWk->pinNo, 3);// pinNo を ServoFPGA へセット
	
	for(i=0;i<TRIG_IO_LSTPOS_MAX;i++)
	{/* 選択された軸の MotAbsPosをバッファにセット */
		trigWk->lstPos[i] = (Axis+trigWk->axisNo)->MencV->MotAbsPos;
	}
	
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************/
/*		Pn65B : トリガIOテーブル4 IO出力時間[us]							*/
/****************************************************************************/
PRM_RSLT pncal_trigIoTbl4_onTime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	hALdrv_WriteTrigIoOnTime(Axis->TrigInfo[3].onTime, 3);	// onTime を ServoFPGA へセット
	return PRM_RSLT_SUCCESS;
}

/* テーブル5 */
/****************************************************************************/
/*		Pn662 : トリガIOテーブル5 軸番号、出力ピン、通過方向				*/
/****************************************************************************/
PRM_RSLT pncal_trigIoTbl5_axisNo(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	USHORT wk1, wk2, i;
	TRIGIO_INFO *trigWk;	/* トリガIO設定情報 */
	
	trigWk = &Axis->TrigInfo[4];

	wk1 = trigWk->trigConfig;
	trigWk->axisNo  =   wk1 & 0x000F;
	trigWk->pinNo   =  (wk1 & 0x00F0)>>4;
	wk2 = (wk1 & 0x0F00)>>8;
	if(wk2 == 1)
	{	trigWk->passDir = -1;	}
	else
	{	trigWk->passDir = 1;	}
	hALdrv_WriteTrigIoPinSetting(trigWk->pinNo, 4);// pinNo を ServoFPGA へセット
	
	for(i=0;i<TRIG_IO_LSTPOS_MAX;i++)
	{/* 選択された軸の MotAbsPosをバッファにセット */
		trigWk->lstPos[i] = (Axis+trigWk->axisNo)->MencV->MotAbsPos;
	}
	
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************/
/*		Pn663 : トリガIOテーブル5 IO出力時間[us]							*/
/****************************************************************************/
PRM_RSLT pncal_trigIoTbl5_onTime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	hALdrv_WriteTrigIoOnTime(Axis->TrigInfo[4].onTime, 4);	// onTime を ServoFPGA へセット
	return PRM_RSLT_SUCCESS;
}

/* テーブル6 */
/****************************************************************************/
/*		Pn66A : トリガIOテーブル6 軸番号、出力ピン、通過方向				*/
/****************************************************************************/
PRM_RSLT pncal_trigIoTbl6_axisNo(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	USHORT wk1, wk2, i;
	TRIGIO_INFO *trigWk;	/* トリガIO設定情報 */
	
	trigWk = &Axis->TrigInfo[5];

	wk1 = trigWk->trigConfig;
	trigWk->axisNo  =   wk1 & 0x000F;
	trigWk->pinNo   =  (wk1 & 0x00F0)>>4;
	wk2 = (wk1 & 0x0F00)>>8;
	if(wk2 == 1)
	{	trigWk->passDir = -1;	}
	else
	{	trigWk->passDir = 1;	}
	hALdrv_WriteTrigIoPinSetting(trigWk->pinNo, 5);// pinNo を ServoFPGA へセット
	
	for(i=0;i<TRIG_IO_LSTPOS_MAX;i++)
	{/* 選択された軸の MotAbsPosをバッファにセット */
		trigWk->lstPos[i] = (Axis+trigWk->axisNo)->MencV->MotAbsPos;
	}
	
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************/
/*		Pn66B : トリガIOテーブル6 IO出力時間[us]							*/
/****************************************************************************/
PRM_RSLT pncal_trigIoTbl6_onTime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	hALdrv_WriteTrigIoOnTime(Axis->TrigInfo[5].onTime, 5);	// onTime を ServoFPGA へセット
	return PRM_RSLT_SUCCESS;
}

/* テーブル7 */
/****************************************************************************/
/*		Pn672 : トリガIOテーブル7 軸番号、出力ピン、通過方向				*/
/****************************************************************************/
PRM_RSLT pncal_trigIoTbl7_axisNo(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	USHORT wk1, wk2, i;
	TRIGIO_INFO *trigWk;	/* トリガIO設定情報 */
	
	trigWk = &Axis->TrigInfo[6];

	wk1 = trigWk->trigConfig;
	trigWk->axisNo  =   wk1 & 0x000F;
	trigWk->pinNo   =  (wk1 & 0x00F0)>>4;
	wk2 = (wk1 & 0x0F00)>>8;
	if(wk2 == 1)
	{	trigWk->passDir = -1;	}
	else
	{	trigWk->passDir = 1;	}
	hALdrv_WriteTrigIoPinSetting(trigWk->pinNo, 6);// pinNo を ServoFPGA へセット
	
	for(i=0;i<TRIG_IO_LSTPOS_MAX;i++)
	{/* 選択された軸の MotAbsPosをバッファにセット */
		trigWk->lstPos[i] = (Axis+trigWk->axisNo)->MencV->MotAbsPos;
	}
	
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************/
/*		Pn673 : トリガIOテーブル7 IO出力時間[us]							*/
/****************************************************************************/
PRM_RSLT pncal_trigIoTbl7_onTime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	hALdrv_WriteTrigIoOnTime(Axis->TrigInfo[6].onTime, 6);	// onTime を ServoFPGA へセット
	return PRM_RSLT_SUCCESS;
}

/* テーブル8 */
/****************************************************************************/
/*		Pn67A : トリガIOテーブル8 軸番号、出力ピン、通過方向				*/
/****************************************************************************/
PRM_RSLT pncal_trigIoTbl8_axisNo(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	USHORT wk1, wk2, i;
	TRIGIO_INFO *trigWk;	/* トリガIO設定情報 */
	
	trigWk = &Axis->TrigInfo[7];

	wk1 = trigWk->trigConfig;
	trigWk->axisNo  =   wk1 & 0x000F;
	trigWk->pinNo   =  (wk1 & 0x00F0)>>4;
	wk2 = (wk1 & 0x0F00)>>8;
	if(wk2 == 1)
	{	trigWk->passDir = -1;	}
	else
	{	trigWk->passDir = 1;	}
	hALdrv_WriteTrigIoPinSetting(trigWk->pinNo, 7);// pinNo を ServoFPGA へセット
	
	for(i=0;i<TRIG_IO_LSTPOS_MAX;i++)
	{/* 選択された軸の MotAbsPosをバッファにセット */
		trigWk->lstPos[i] = (Axis+trigWk->axisNo)->MencV->MotAbsPos;
	}
	
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************/
/*		Pn67B : トリガIOテーブル8 IO出力時間[us]							*/
/****************************************************************************/
PRM_RSLT pncal_trigIoTbl8_onTime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	hALdrv_WriteTrigIoOnTime(Axis->TrigInfo[7].onTime, 7);	// onTime を ServoFPGA へセット
	return PRM_RSLT_SUCCESS;
}

/* レジスタ */
/****************************************************************************/
/*		 0x2300：トリガIO状態モニタレジスタ									*/
/****************************************************************************/
PRM_RSLT fnCalcTriggerIOStsMon(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	switch(Cmd)
	{
	case PRM_ACCCMD_READ:
		// wk = Servo FPGAのデータを読み込み;
		*pValue = (LONG)hALdrv_ReadTrigIoSts();
		break;	
	default:
		/* do nothing */
		break;
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************/
/*		 0x2301：トリガIO制御レジスタ										*/
/****************************************************************************/
PRM_RSLT fnCalcTriggerIOControl(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

	USHORT	wk,wk1,wk2,tbl_no;
	BOOL	trigIOEnable_wk[8];	/* トリガIO有効 [0:無効、1:有効] */
	BOOL	trigIOMode_wk[8];	/* モード [0:通常モード、1:マニュアル出力] */
	TRIGIO_INFO *trigWk;	/* トリガIO設定情報 */
	
	trigWk = &Axis->TrigInfo[0];
	
	switch(Cmd)
	{
	case PRM_ACCCMD_WRITE:
	case PRM_ACCCMD_EEPWRITE:
		wk = (USHORT)*pValue;	
		wk1 = wk & 0x00FF;
		wk2 = (wk & 0xFF00)>>8;

#if 0
		ここで グローバル変数をセットしてしまうと、ScanAのトリガIO処理が実行されてしまうのでNG
		/* トリガIO出力モード */
		trigWk->trigIOMode =  wk2 & 0x01;
		(trigWk+1)->trigIOMode = (wk2 & 0x02)>>1;
		(trigWk+2)->trigIOMode = (wk2 & 0x04)>>2;
		(trigWk+3)->trigIOMode = (wk2 & 0x08)>>3;
		(trigWk+4)->trigIOMode = (wk2 & 0x10)>>4;
		(trigWk+5)->trigIOMode = (wk2 & 0x20)>>5;
		(trigWk+6)->trigIOMode = (wk2 & 0x40)>>6;
		(trigWk+7)->trigIOMode = (wk2 & 0x80)>>7;
	
		/* トリガIO出力機能有効 */
		trigWk->trigIOEnable =  wk1 & 0x01;
		(trigWk+1)->trigIOEnable = (wk1 & 0x02)>>1;
		(trigWk+2)->trigIOEnable = (wk1 & 0x04)>>2;
		(trigWk+3)->trigIOEnable = (wk1 & 0x08)>>3;
		(trigWk+4)->trigIOEnable = (wk1 & 0x10)>>4;
		(trigWk+5)->trigIOEnable = (wk1 & 0x20)>>5;
		(trigWk+6)->trigIOEnable = (wk1 & 0x40)>>6;
		(trigWk+7)->trigIOEnable = (wk1 & 0x80)>>7;
#endif
		/* トリガIO出力モード */
		trigIOMode_wk[0] =  wk2 & 0x01;
		trigIOMode_wk[1] = (wk2 & 0x02)>>1;
		trigIOMode_wk[2] = (wk2 & 0x04)>>2;
		trigIOMode_wk[3] = (wk2 & 0x08)>>3;
		trigIOMode_wk[4] = (wk2 & 0x10)>>4;
		trigIOMode_wk[5] = (wk2 & 0x20)>>5;
		trigIOMode_wk[6] = (wk2 & 0x40)>>6;
		trigIOMode_wk[7] = (wk2 & 0x80)>>7;

		/* トリガIO出力機能有効 */
		trigIOEnable_wk[0] =  wk1 & 0x01;
		trigIOEnable_wk[1] = (wk1 & 0x02)>>1;
		trigIOEnable_wk[2] = (wk1 & 0x04)>>2;
		trigIOEnable_wk[3] = (wk1 & 0x08)>>3;
		trigIOEnable_wk[4] = (wk1 & 0x10)>>4;
		trigIOEnable_wk[5] = (wk1 & 0x20)>>5;
		trigIOEnable_wk[6] = (wk1 & 0x40)>>6;
		trigIOEnable_wk[7] = (wk1 & 0x80)>>7;		
		
		for( tbl_no = 0; tbl_no < 8; tbl_no++ )
		{			
//			if((trigWk+tbl_no)->trigIOEnable == TRUE) 
			if(trigIOEnable_wk[tbl_no] == TRUE) 
			{
//				if((trigWk+tbl_no)->trigIOMode == TRIG_IO_NORMAL)	
				if(trigIOMode_wk[tbl_no]  == TRIG_IO_NORMAL)	
				{/* 初期値として 0xFFFF をセット */
					hALdrv_WriteTrigIoDelayTimeH(0x0000, tbl_no);
					hALdrv_WriteTrigIoDelayTimeL(0xFFFF, tbl_no);				
				}
				else
				{/* ManualDelay を 出力遅延時間設定レジスタにセット */
					wk1 = (USHORT)((trigWk+tbl_no)->manualDelay & 0x0000FFFF);
					wk2 = (USHORT)(((trigWk+tbl_no)->manualDelay & 0xFFFF0000) >> 16);				
					hALdrv_WriteTrigIoDelayTimeH(wk2, tbl_no);
					hALdrv_WriteTrigIoDelayTimeL(wk1, tbl_no);					
				}
			}
			else
			{/* トリガIO無効の場合は 出力遅延時間として0x7FFFFFFFをセット */
				hALdrv_WriteTrigIoDelayTimeH(0x7FFF, tbl_no);
				hALdrv_WriteTrigIoDelayTimeL(0xFFFF, tbl_no);
				(trigWk+tbl_no)->posReady = FALSE;	/* ポジション準備未完 */	
			}
		}
		
		/* トリガIO出力モード */
		trigWk->trigIOMode =  trigIOMode_wk[0];
		(trigWk+1)->trigIOMode = trigIOMode_wk[1];
		(trigWk+2)->trigIOMode = trigIOMode_wk[2];
		(trigWk+3)->trigIOMode = trigIOMode_wk[3];
		(trigWk+4)->trigIOMode = trigIOMode_wk[4];
		(trigWk+5)->trigIOMode = trigIOMode_wk[5];
		(trigWk+6)->trigIOMode = trigIOMode_wk[6];
		(trigWk+7)->trigIOMode = trigIOMode_wk[7];
	
		/* トリガIO出力機能有効 */
		trigWk->trigIOEnable =  trigIOEnable_wk[0];
		(trigWk+1)->trigIOEnable = trigIOEnable_wk[1];
		(trigWk+2)->trigIOEnable = trigIOEnable_wk[2];
		(trigWk+3)->trigIOEnable = trigIOEnable_wk[3];
		(trigWk+4)->trigIOEnable = trigIOEnable_wk[4];
		(trigWk+5)->trigIOEnable = trigIOEnable_wk[5];
		(trigWk+6)->trigIOEnable = trigIOEnable_wk[6];
		(trigWk+7)->trigIOEnable = trigIOEnable_wk[7];
		
		break;

	case PRM_ACCCMD_READ:
	default:
		wk1 = hALdrv_ReadTrigIoSts();
		wk  = wk1 & 0xFF;
	
		wk |= trigWk->trigIOMode << 8;
		wk |= (trigWk+1)->trigIOMode << 9;
		wk |= (trigWk+2)->trigIOMode << 10;
		wk |= (trigWk+3)->trigIOMode << 11;
		wk |= (trigWk+4)->trigIOMode << 12;
		wk |= (trigWk+5)->trigIOMode << 13;	
		wk |= (trigWk+6)->trigIOMode << 14;			
		wk |= (trigWk+7)->trigIOMode << 15;	
		
		*pValue = (LONG)wk;
		break;
	}
	return PRM_RSLT_SUCCESS;
}
// ScanAでの処理との競合は大丈夫か？ ⇒　問題なし

/****************************************************************************/
/*																			*/
/*																			*/
/*		Trigger I/O アプリケーション処理 定義								*/
/*																			*/
/*																			*/
/****************************************************************************/

/****************************************************************************
  Trigger IO モジュール初期化処理 
****************************************************************************/
void TrigIOInitialize(AXIS_HANDLE *Axis)
{
	TRIGIO_INFO *trigWk;	/* トリガIO設定情報 */
	USHORT tbl_no;			/* テーブル番号 */
	
	for( tbl_no = 0; tbl_no < 8; tbl_no++ )
	{
		trigWk = &Axis->TrigInfo[tbl_no];
		MlibResetByteMemory( trigWk, sizeof(TRIGIO_INFO) );
	}
}

/****************************************************************************
  Trigger IO トリガI/O出力時間算出処理 
****************************************************************************/
static LONG calcDelayTime(TRIGIO_INFO *trigWk, LONG crtPos, SHORT bufPos)
{
	LONG delayTime;			/* I/O出力遅れ時間 */
	LONG dtrigPos,dcrtPos;	/* トリガ位置までの距離、前回値と今回値の差分 */
	
	dtrigPos = trigWk->trigPosition - crtPos;
	dcrtPos  = crtPos - trigWk->lstPos[bufPos];
	
	if(trigWk->passDir*dtrigPos >= 0)
	{/* トリガI/Oポジション有効 */
		trigWk->posReady = TRUE;
	}
	
	if(trigWk->posReady == TRUE)
	{
	    if(crtPos == trigWk->lstPos[bufPos])
		{/* 速度ゼロの場合 */
			delayTime = 0x7FFFFFFF;
		}
		else if(MlibABS(dtrigPos) > 0x7FFFFFFF/(ENC_SAMPLING_TIME*(trigWk->mSpdCycle+1)))
		{/* トリガI/O出力時間算出処理がオーバフローする場合 */
			delayTime = 0x7FFFFFFF;
//			delayTime = (0x7FFFFFFF/MlibABS(dcrtPos) - ENC_COM_DELAY)/2 -trigWk->onAdjTime;
		}
		else
		{/* 通常時 */
			if(trigWk->passDir*dcrtPos>0)
			{/* 通過方向=トリガ方向 */
				delayTime = (trigWk->passDir*dtrigPos*ENC_SAMPLING_TIME*(trigWk->mSpdCycle+1)
					/MlibABS(dcrtPos) - ENC_COM_DELAY)/2 - trigWk->onAdjTime;
			}
			else
			{/* 通過方向が逆の場合 */
					delayTime = 0x7FFFFFFF;	
			}
		}	
	}
	else
	{
		delayTime = 0x7FFFFFFF;
	}
	
	return delayTime;
}

/****************************************************************************
  Setting Trigger IO Delay Time for SeroFPGA 
****************************************************************************/
void TrigIOSetDelayTime(AXIS_HANDLE	*Axis)
{
	LONG delayTime = 0xFFFF;/* トリガI/O出力遅延時間[us] */
	USHORT tbl_no;			/* テーブル番号 */
	LONG crtPos;			/* 現在の位置 */
	TRIGIO_INFO *trigWk;	/* トリガIO設定情報 */
	SHORT bufPos;			/* リングバッファポジション */
	
	for( tbl_no = 0; tbl_no < 8; tbl_no++ )
	{
		trigWk = &Axis->TrigInfo[tbl_no];
		crtPos = (Axis+trigWk->axisNo)->MencV->MotAbsPos;
	
		if((trigWk->trigIOMode == TRIG_IO_NORMAL) && (trigWk->trigIOEnable == TRUE))
		{
			/* バッファ内の位置計算 */
			bufPos = trigWk->fbPosBufPtr - trigWk->mSpdCycle;
			if(bufPos < 0){bufPos = TRIG_IO_LSTPOS_MAX + bufPos;}
		
			/* delayTime計算 */
			delayTime = calcDelayTime(trigWk, crtPos, bufPos);
			
			/* ServoFPGA設定範囲に加工 */
			if(delayTime > 0xFFFF)
			{
				delayTime = 0xFFFF;
			}
			else if(delayTime <= 0)
			{
				trigWk->trigIOEnable = FALSE;	// 一度I/O出力をしたら終了
				delayTime = 0;
			}
			/* delayTimeの値をServoFPGAにセット */
			hALdrv_WriteTrigIoDelayTimeL((USHORT)delayTime, tbl_no);			
		}
		
		/* バッファの更新 */
		trigWk->fbPosBufPtr++;
		if(trigWk->fbPosBufPtr >= TRIG_IO_LSTPOS_MAX)
		{trigWk->fbPosBufPtr = 0;}			
		trigWk->lstPos[trigWk->fbPosBufPtr] = crtPos;
	}	
}
// マニュアルモードのときはパラメータ計算関数にてマニュアル遅延時間をレジスタにセット
// MotAbsPos の前回値はグローバルに保持していないので、TrigIo構造体に保持する
// エンコーダベースタイマ値はJL-077からCPU側は読み込んでいない。
