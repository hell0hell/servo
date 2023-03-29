/****************************************************************************************************/
/*																									*/
/*																									*/
/*		XlsTrcDef.c : トレース定義ソースファイル													*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1. 数値トレース定義テーブル																	*/
/*																									*/
/*		2. ビットトレース定義テーブル																*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "XlsTrcDef.h"
#include "MercuryGlobals.h"
#include "DataTraceCalc.h"

/*--------------------------------------------------------------------------------------------------*/
/*  注意： エクセルマクロ非対応。																	*/
/*  以下を設定する必要あり。																		*/
/*  Sel					：数値トレース選択番号														*/
/*  SftL				：左シフト量																*/
/*  SftR				：右シフト量																*/
/*	AvrgSel				：平均化する:TRUE、平均化しない:FALSE										*/
/*	GetVerData Function	：関数ポインタ(トレースする変数の値を返す関数)								*/
/*	GainCal Function	：関数ポインタ(SftL,SftR以外にゲイン換算が必要なときにゲイン係数を返す関数)	*/
/*--------------------------------------------------------------------------------------------------*/
/****************************************************************************************************/
/*																									*/
/*		数値トレース定義テーブル																	*/
/*																									*/
/****************************************************************************************************/
const	TVXDEF	TrcVarTbl[] = {
/*  Sel  SftL SftR AvrgSel	Get TraceVarData Func       Gain Calcurate Func							*/
/*--------------------------------------------------------------------------------------------------*/
  { 0x00, 0,   0,   FALSE,	NULL,						NULL                      },/* トレース指定なし					*/
  { 0x01, 0,  10,   FALSE,	&GetVarData_AmonTrqRef,		NULL                      },/* トルク指令／推力指令				*/
  { 0x02, 0,  10,	TRUE,	&GetVarData_MotSpdSmp,		&LpxPcalMotSpeedCalGain2  },/* モータ速度						*/
  { 0x03, 0,  10,	FALSE,	&GetVarData_AmonSpdRef,		NULL	                  },/* 速度指令							*/
  { 0x04, 0,  10, 	TRUE,	&GetVarData_dPosCmdSmp,		&LpxPcalPcmdSpeedCalGain  },/* 位置指令速度						*/
  { 0x05, 0,   0, 	FALSE,	&GetVarData_AmonPosErra,	NULL                      },/* 位置偏差							*/
//{ 0x10, 0,  10,   FALSE,	&GetVarData_Dummy,			NULL					  },/* オブザーバ速度					*//* 非対応 */
//{ 0x11, 0,  10,   FALSE,	&GetVarData_Dummy,			NULL					  },/* モデルトルク指令／推力指令		*//* 非対応 */
//{ 0x12, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* モデル位置偏差					*//* 非対応 */
  { 0x13, 0,  10,   FALSE,	&GetVarData_AmonDisTrq,		NULL					  },/* 摩擦補償トルク／推力				*//* 非公開 */
  { 0x14, 0,  10,   FALSE,	&GetVarData_MonResVib,		NULL					  },/* 制振差速度						*//* 非公開 */
  { 0x15, 0,  10,   FALSE,	&GetVarData_TrqGravity,		NULL					  },/* 重力補償済みトルク指令／推力指令	*/	/*<S0CD>*/	/* 非対応⇒対応 */
  { 0x16, 0,   0,   FALSE,	&GetVarData_AmonPosErrx,	NULL	                  },/* 位置アンプ偏差					*/
  { 0x17, 0,   0,   FALSE,	&GetVarData_sfperra_per32s, NULL				      },/* モータ負荷間位置偏差				*/
  { 0x18, 0,  10,   FALSE,	&GetVarData_SpdFFCx,		NULL					  },/* 速度フィードフォワード			*/
  { 0x19, 0,  10,   FALSE,	&GetVarData_TrqFFCx,		NULL					  },/* トルク／推力フィードフォワード	*/
//{ 0x1A, 0,  10,   FALSE,	&GetVarData_Dummy,			NULL					  },/* MFC後の位置指令					*//* 非対応 */
//{ 0x1B, 0,  10,   FALSE,	&GetVarData_Dummy,			NULL					  },/* 加算トルク指令／推力指令			*//* 非対応 */
//{ 0x1C, 0,  10,   FALSE,	&GetVarData_Dummy,			NULL				      },/* 加算外乱トルク指令／推力指令		*//* 非対応 */
//{ 0x1D, 0,  10,   FALSE,	&GetVarData_Dummy,			NULL					  },/* 推定速度							*//* 非対応 */
  { 0x1E, 0,  10,   FALSE,	&GetVarData_TrefBeforeDtrq, NULL					  },/* トルク指令(外乱前)				*//* 非公開 */
  { 0x1F, 0,  10,   FALSE,	&GetVarData_TrcCtrlTrqRef,	NULL					  },/* トルク指令－摩擦補償トルク		*//* 非公開 */
  { 0x20, 0,  10,   FALSE,	&GetVarData_TrqRefi,		NULL					  },/* トルク指令(外乱前・フィルタ前)	*//* 非公開 */
  { 0x21, 0,  10,   FALSE,	&GetVarData_AmonActGain,	NULL					  },/* 有効ゲイン						*/
  { 0x22, 0,  10,   TRUE,	&GetVarData_dPosRefi,		&LpxPcalMotSpeedCalGain   },/* 位置ループ前指令速度				*/
  { 0x23, 0,   0,   FALSE,	&GetVarData_DcVoltx,		NULL					  },/* 主回路母線電圧					*/
  { 0x24, 0,   0,   FALSE,	&GetVarData_UnCrntDet,		NULL					  },/* 電流検出値						*/
//{ 0x25, 0,  10,   FALSE,	&GetVarData_Dummy,			NULL					  },/* 実効負荷率						*/
  { 0x26, 0,   0,   FALSE,	&GetVarData_UnCrntRef,		NULL					  },/* 累積負荷率						*/
  { 0x27, 0,   0,   FALSE,	&GetVarData_TrcRgPower,		NULL					  },/* 回生負荷率						*/
  { 0x28, 0,   0,   FALSE,	&GetVarData_TrcDbPower,		NULL					  },/* DB抵抗消費電力					*/
  { 0x29, 0,  10,   TRUE,	&GetVarData_FencSpdSmp,		NULL					  },/* 外部エンコーダ速度				*/
	//0x40と0x42は同じものを出力！0x40をフルクに変更！kira_110309
  { 0x40, 1,   0,   FALSE,	&GetVarData_DeltaPos,		NULL                      },/* 位置差分							*/
  { 0x41, 1,   0,   FALSE,	&GetVarData_dPcmdEgear,		NULL					  },/* 指令差分							*/
  { 0x42, 1,   0,   FALSE,	&GetVarData_DeltaPos,		NULL                      },/* モータ位置差分					*/
  { 0x43, 1,   0,   FALSE,	&GetVarData_dFencPos,		NULL					  },/* 負荷位置差分						*/
  { 0x44, 0,   2,   FALSE,	&GetVarData_MotPhase,		NULL					  },/* 電流位相							*/
//{ 0x50, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* SOMモータ速度					*//* 非対応 */
//{ 0x51, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* SOMモータ移動量					*//* 非対応 */
//{ 0x52, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* SOM監視速度A						*//* 非対応 */
//{ 0x53, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* SOM監視速度B						*//* 非対応 */
//{ 0x54, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* アクティブモード内部指令速度		*//* 非対応 */
//{ 0x55, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* SOM内部データ1					*//* 非対応 */
//{ 0x56, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* SOM内部データ2					*//* 非対応 */
  { 0x60, 0,   0,   FALSE,	&GetVarData_InterSignalX_L,	NULL					  },/* 内部信号モニタ(下位)forSynqNet	*/
  { 0x61, 0,   0,   FALSE,	&GetVarData_InterSignalX_H,	NULL					  },/* 内部信号モニタ(上位)forSynqNet	*/
  { 0x62, 0,   0,   FALSE,	&GetVarData_InternalInputSignals_L,		NULL		  },/* 内部入力信号モニタ(下位)forSynqNet*/
  { 0x63, 0,   0,   FALSE,	&GetVarData_InternalInputSignals_H,		NULL		  },/* 内部入力信号モニタ(上位)forSynqNet*/
  { 0x64, 0,   0,   FALSE,	&GetVarData_InternalOutputSignals_L,	NULL		  },/* 内部出力信号モニタ(下位)forSynqNet*/
  { 0x65, 0,   0,   FALSE,	&GetVarData_InternalOutputSignals_H,	NULL		  },/* 内部出力信号モニタ(上位)forSynqNet*/
  { 0x81, 0,   0,   FALSE,	&GetVarData_AmonTrqRef,		NULL                      },/* トルク指令／推力指令				*/
  { 0x82, 0,   0,	TRUE,	&GetVarData_MotSpdSmp,		&LpxPcalMotSpeedCalGain2  },/* モータ速度						*/
  { 0x83, 0,   0,   FALSE,	&GetVarData_AmonSpdRef,		NULL                      },/* 速度指令							*/
  { 0x84, 0,   0,   TRUE,	&GetVarData_dPosCmdSmp,		&LpxPcalPcmdSpeedCalGain  },/* 位置指令速度						*/
  { 0x85, 0,   0,   FALSE,	&GetVarData_AmonPosErra,	NULL                      },/* 位置偏差							*/
//{ 0x90, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* オブザーバ速度					*//* 非対応 */
//{ 0x91, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* モデルトルク指令／推力指令		*//* 非対応 */
//{ 0x92, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* モデル位置偏差					*//* 非対応 */
//{ 0x93, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* 摩擦補償トルク／推力				*//* 非対応 */
  { 0x94, 0,   0,   FALSE,	&GetVarData_MonResVib,		NULL					  },/* 制振差速度						*/
  { 0x95, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL				  	  },/* 重力補償済みトルク指令／推力指令	*//* <S0CD> */	/* 非対応⇒対応 */
  { 0x96, 0,   0,   FALSE,	&GetVarData_AmonPosErrx,	NULL	                  },/* 位置アンプ偏差					*/
  { 0x97, 0,   0,   FALSE,	&GetVarData_sfperra_per32s, NULL					  },/* モータ負荷間位置偏差				*/
  { 0x98, 0,   0,   FALSE,	&GetVarData_SpdFFCx,		NULL					  },/* 速度フィードフォワード			*/
  { 0x99, 0,   0,   FALSE,	&GetVarData_TrqFFCx,		NULL					  },/* トルク／推力フィードフォワード	*/
//{ 0x9A, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* MFC後の位置指令					*//* 非対応 */
//{ 0x9B, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* 加算トルク指令／推力指令			*//* 非対応 */
//{ 0x9C, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* 加算外乱トルク指令／推力指令		*//* 非対応 */
//{ 0x9D, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* 推定速度							*//* 非対応 */
//{ 0x9E, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* トルク指令(外乱前)				*//* 非対応 */
//{ 0x9F, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* トルク指令－摩擦補償トルク		*//* 非対応 */
//{ 0xA0, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* トルク指令(外乱前・フィルタ前)	*//* 非対応 */
//{ 0xA1, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* 有効ゲイン						*//* 非対応 */
  { 0xA2, 0,   0,   TRUE,	&GetVarData_dPosRefi,		&LpxPcalMotSpeedCalGain   },/* 位置ループ前指令速度				*/
  { 0xA9, 0,   0,   TRUE,	&GetVarData_FencSpdSmp,		NULL					  },/* 外部エンコーダ速度				*/
  { 0xAA, 0,   0,   FALSE,	&GetVarData_VelRippleTrqRefBeforeComp,	NULL		  },/* トルク指令（リップル補正前）		*/	/* <S14B><S18C> */
  { 0xC4, 0,   0,   FALSE,	&GetVarData_CurrentPhase,	NULL					  },/* 電流位相 						*/	/* <S14B><S18C> */
  { 0xC5, 0,   0,   FALSE,	&GetVarData_TrcDcVoltFili,	NULL					  },/* 主回路母線電圧（フィルタ前）		*/	/* <S199> *//* 非対応⇒対応 */
//{ 0xD0, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* SOMモータ速度					*//* 非対応 */
//{ 0xD1, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* SOMモータ移動量					*//* 非対応 */
//{ 0xD2, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* SOM監視速度A						*//* 非対応 */
//{ 0xD3, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* SOM監視速度B						*//* 非対応 */
//{ 0xD4, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* アクティブモード内部指令速度		*//* 非対応 */
//{ 0xD5, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* SOM内部データ1					*//* 非対応 */
//{ 0xD6, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* SOM内部データ2					*//* 非対応 */
#if (CSW_NETIF_TYPE == NETIF_M3_TYPE)
  { 0x100, 0,   0,   FALSE,	&GetVarData_M3_Apos,		NULL					  },/* フィードバック位置(APOS)			*/				/* <S164> : 設定値変更 0xD0⇒0x100 */
  { 0x101, 0,   0,   FALSE,	&GetVarData_M3_Cpos,		NULL					  },/* 現在指令位置(CPOS)				*/				/* <S164> : 設定値変更 0xD1⇒0x101 */
  { 0x102, 0,   0,   FALSE,	&GetVarData_M3_Tpos,		NULL					  },/* 目標位置(TPOS)					*/				/* <S164> : 設定値変更 0xD2⇒0x102 */
  { 0x103, 0,   0,   FALSE,	&GetVarData_M3_Lpos1,		NULL					  },/* ラッチ位置1(LPOS1)				*/				/* <S164> : 設定値変更 0xD3⇒0x103 */
  { 0x104, 0,   0,   FALSE,	&GetVarData_M3_Lpos2,		NULL					  },/* ラッチ位置2(LPOS2)				*/				/* <S164> : 設定値変更 0xD4⇒0x104 */
//{ 0x105, 0,   0,   FALSE,	&GetVarData_Dummy,			NULL					  },/* ラッチ位置3(LPOS3)				*//* 非対応 */	/* <S164> */
  { 0x106, 0,   0,   FALSE,	&GetVarData_M3_Fspd,		NULL					  },/* フィードバック速度(FSPD)			*/			/* <S164> : 設定値変更 0xD5⇒0x106 */
  { 0x107, 0,   0,   FALSE,	&GetVarData_M3_Cspd,		NULL					  },/* 現在位置指令速度(CSPD)			*/			/* <S164> : 設定値変更 0xD6⇒0x107 */
  { 0x108, 0,   0,   FALSE,	&GetVarData_M3_Trq,			NULL					  },/* トルク/推力指令(TRQ)				*/			/* <S164> : 設定値変更 0xD7⇒0x108 */
  { 0x109, 0,   0,   FALSE,	&GetVarData_M3_TrqLim,		NULL					  },/* トルク制限指令(TRQ_LIM)			*/			/* <S164> : 設定値変更 0xD8⇒0x109 */
  { 0x10A, 0,   0,   FALSE,	&GetVarData_M3_VLim,		NULL					  },/* 速度制限指令(V_LIM)				*/			/* <S164> : 設定値変更 0xD9⇒0x10A */
  { 0x10B, 0,   0,   FALSE,	&GetVarData_M3_M3_Alam,		NULL					  },/* 現在発生アラームコード(ALARM)	*/			/* <S164> : 設定値変更 0xDA⇒0x10B */
  { 0x10C, 0,   0,   FALSE,	&GetVarData_M3_RCmd,		NULL					  },/* M-Ⅲ現在実行コマンドコード(RCMD)	*/			/* <S164> : 設定値変更 0xDB⇒0x10C */
  { 0x10D, 0,   0,   FALSE,	&GetVarData_M3_SubRCmd,		NULL					  },/* M-Ⅲ現在実行サブコマンドコード(SUB_RCMD)	*/	/* <S164> : 設定値変更 0xDC⇒0x10D */
  { 0x110, 0,   0,   FALSE,	&GetVarData_M3_Cmd_0,		NULL					  },/* M-Ⅲコマンドデータ( 0～ 3byte)	*/			/* <S164> : 設定値変更 0xE0⇒0x110 */
  { 0x111, 0,   0,   FALSE,	&GetVarData_M3_Cmd_1,		NULL					  },/* M-Ⅲコマンドデータ( 4～ 7byte)	*/			/* <S164> : 設定値変更 0xE1⇒0x111 */
  { 0x112, 0,   0,   FALSE,	&GetVarData_M3_Cmd_2,		NULL					  },/* M-Ⅲコマンドデータ( 8～11byte)	*/			/* <S164> : 設定値変更 0xE2⇒0x112 */
  { 0x113, 0,   0,   FALSE,	&GetVarData_M3_Cmd_3,		NULL					  },/* M-Ⅲコマンドデータ(12～15byte)	*/			/* <S164> : 設定値変更 0xE3⇒0x113 */
  { 0x114, 0,   0,   FALSE,	&GetVarData_M3_Cmd_4,		NULL					  },/* M-Ⅲコマンドデータ(16～19byte)	*/			/* <S164> : 設定値変更 0xE4⇒0x114 */
  { 0x115, 0,   0,   FALSE,	&GetVarData_M3_Cmd_5,		NULL					  },/* M-Ⅲコマンドデータ(20～23byte)	*/			/* <S164> : 設定値変更 0xE5⇒0x115 */
  { 0x116, 0,   0,   FALSE,	&GetVarData_M3_Cmd_6,		NULL					  },/* M-Ⅲコマンドデータ(24～27byte)	*/			/* <S164> : 設定値変更 0xE6⇒0x116 */
  { 0x117, 0,   0,   FALSE,	&GetVarData_M3_Cmd_7,		NULL					  },/* M-Ⅲコマンドデータ(28～31byte)	*/			/* <S164> : 設定値変更 0xE7⇒0x117 */
  { 0x118, 0,   0,   FALSE,	&GetVarData_M3_Cmd_8,		NULL					  },/* M-Ⅲコマンドデータ(32～35byte)	*/			/* <S164> : 設定値変更 0xE8⇒0x118 */
  { 0x119, 0,   0,   FALSE,	&GetVarData_M3_Cmd_9,		NULL					  },/* M-Ⅲコマンドデータ(36～39byte)	*/			/* <S164> : 設定値変更 0xE9⇒0x119 */
  { 0x11A, 0,   0,   FALSE,	&GetVarData_M3_Cmd_a,		NULL					  },/* M-Ⅲコマンドデータ(40～43byte)	*/			/* <S164> : 設定値変更 0xEA⇒0x11A */
  { 0x11B, 0,   0,   FALSE,	&GetVarData_M3_Cmd_b,		NULL					  },/* M-Ⅲコマンドデータ(44～47byte)	*/			/* <S164> : 設定値変更 0xEB⇒0x11B */
  { 0x120, 0,   0,   FALSE,	&GetVarData_M3_Rsp_0,		NULL					  },/* M-Ⅲレスポンスデータ( 0～ 3byte)	*/			/* <S164> : 設定値変更 0xF0⇒0x120 */
  { 0x121, 0,   0,   FALSE,	&GetVarData_M3_Rsp_1,		NULL					  },/* M-Ⅲレスポンスデータ( 4～ 7byte)	*/			/* <S164> : 設定値変更 0xF1⇒0x121 */
  { 0x122, 0,   0,   FALSE,	&GetVarData_M3_Rsp_2,		NULL					  },/* M-Ⅲレスポンスデータ( 8～11byte)	*/			/* <S164> : 設定値変更 0xF2⇒0x122 */
  { 0x123, 0,   0,   FALSE,	&GetVarData_M3_Rsp_3,		NULL					  },/* M-Ⅲレスポンスデータ(12～15byte)	*/			/* <S164> : 設定値変更 0xF3⇒0x123 */
  { 0x124, 0,   0,   FALSE,	&GetVarData_M3_Rsp_4,		NULL					  },/* M-Ⅲレスポンスデータ(16～19byte)	*/			/* <S164> : 設定値変更 0xF4⇒0x124 */
  { 0x125, 0,   0,   FALSE,	&GetVarData_M3_Rsp_5,		NULL					  },/* M-Ⅲレスポンスデータ(20～23byte)	*/			/* <S164> : 設定値変更 0xF5⇒0x125 */
  { 0x126, 0,   0,   FALSE,	&GetVarData_M3_Rsp_6,		NULL					  },/* M-Ⅲレスポンスデータ(24～27byte)	*/			/* <S164> : 設定値変更 0xF6⇒0x126 */
  { 0x127, 0,   0,   FALSE,	&GetVarData_M3_Rsp_7,		NULL					  },/* M-Ⅲレスポンスデータ(28～31byte)	*/			/* <S164> : 設定値変更 0xF7⇒0x127 */
  { 0x128, 0,   0,   FALSE,	&GetVarData_M3_Rsp_8,		NULL					  },/* M-Ⅲレスポンスデータ(32～35byte)	*/			/* <S164> : 設定値変更 0xF8⇒0x128 */
  { 0x129, 0,   0,   FALSE,	&GetVarData_M3_Rsp_9,		NULL					  },/* M-Ⅲレスポンスデータ(36～39byte)	*/			/* <S164> : 設定値変更 0xF9⇒0x129 */
  { 0x12A, 0,   0,   FALSE,	&GetVarData_M3_Rsp_a,		NULL					  },/* M-Ⅲレスポンスデータ(40～43byte)	*/			/* <S164> : 設定値変更 0xFA⇒0x12A */
  { 0x12B, 0,   0,   FALSE,	&GetVarData_M3_Rsp_b,		NULL					  },/* M-Ⅲレスポンスデータ(44～47byte)	*/			/* <S164> : 設定値変更 0xFB⇒0x12B */
#endif //CSW_NETIF_TYPE == NETIF_M3_TYPE
};
/*--------------------------------------------------------------------------------------------------*/
const	USHORT	TrcVarTblEntNum = sizeof(TrcVarTbl)/sizeof(TrcVarTbl[0]);
/*--------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------*/
/*  注意： エクセルマクロ非対応。																	*/
/*  以下を設定する必要あり。																		*/
/*  Sel					：数値トレース選択番号														*/
/*	GetBitData Function	：関数ポインタ(トレースするビットの値を返す関数)							*/
/*--------------------------------------------------------------------------------------------------*/
/****************************************************************************************************/
/*																									*/
/*		ビットトレース定義テーブル																	*/
/*																									*/
/****************************************************************************************************/
const	TBXDEF	TrcBitTbl[] = {
/*  Sel　  Get BitData Function																		*/
/*--------------------------------------------------------------------------------------------------*/
  { 0x00,  NULL						},	/* トレース指定なし						*/
  { 0x01,  &GetBitData_SvonReq		},	/* サーボオン指令(/S-ON)				*/
  { 0x02,  &GetBitData_Pcon			},	/* P制御指令(/P-CON)					*/
  { 0x03,  &GetBitData_Pot			},	/* 正転禁止(P-OT)						*/
  { 0x04,  &GetBitData_Not			},	/* 逆転禁止(N-OT)						*/
  { 0x05,  &GetBitData_AlmRst		},	/* アラームリセット(/ALMRST)			*/
  { 0x06,  &GetBitData_Pcl			},	/* 正側トルク制限(/P-CL)				*/
  { 0x07,  &GetBitData_Ncl			},	/* 負側トルク制限(/N-CL)				*/
  { 0x08,  &GetBitData_SpdD			},	/* SPD-D信号(/SPD-D)					*//* 対応 */	/* <S164> *//* 非対応⇒対応 */
  { 0x09,  &GetBitData_SpdA			},	/* SPD-A信号(/SPD-A)					*//* 対応 */	/* <S164> *//* 非対応⇒対応 */
  { 0x0A,  &GetBitData_SpdB			},	/* SPD-B信号(/SPD-B)					*//* 対応 */	/* <S164> *//* 非対応⇒対応 */
  { 0x0B,  &GetBitData_Csel			},	/* C-SEL信号(/C-SEL)					*//* 対応 */	/* <S164> *//* 非対応⇒対応 */
  { 0x0C,  &GetBitData_Zclamp		},	/* ゼロクランプ信号(/ZCLAMP)			*//* 対応 */	/* <S164> *//* 非対応⇒対応 */
  { 0x0D,  &GetBitData_Inhibit		},	/* パルス指令無視信号(/INHIBIT)			*//* 対応 */	/* <S164> *//* 非対応⇒対応 */
  { 0x0E,  &GetBitData_Gsel			},	/* G-SEL1信号(/G-SEL1)					*/
  { 0x0F,  &GetBitData_Pdet			},	/* 磁極検出指令(/P-DET)					*/
//{ 0x10,  &BinV.f.Sv				},	/* G-SEL2信号(/GSEL-2)					*//* 非対応 */
  { 0x11,  &GetBitData_Estp			},	/* 非常停止(/EMG-STP)					*//* 非対応 */	/* <S199> *//* 非対応⇒対応 */
  { 0x12,  &GetBitData_Dec			},	/* 減速信号(/DEC)						*/
  { 0x13,  &GetBitData_Ext1			},	/* EXT1割込み要求(/EXT1)				*/
  { 0x14,  &GetBitData_Ext2			},	/* EXT2割込み要求(/EXT2)				*/
  { 0x15,  &GetBitData_Ext3			},	/* EXT3割込み要求(/EXT3)				*/
  { 0x16,  &GetBitData_Fstp			},	/* 強制停止/FSTP)						*/	/* <S1F5> */
//{ 0x16,  &ReadDummy.Ulong			},	/* 入力信号1(/INPUT1)					*//* 非対応 */
//{ 0x17,  &ReadDummy.Ulong			},	/* 入力信号2(/INPUT2)					*//* 非対応 */
//{ 0x18,  &ReadDummy.Ulong			},	/* 入力信号3(/INPUT3)					*//* 非対応 */
  { 0x19,  &GetBitData_Sen			},	/* SEN信号(SEN)							*/
  { 0x1A,  &GetBitData_Puls			},	/* パルス指令入力有無(PULS)				*//* 対応 */	/* <S164> *//* 非対応⇒対応 */	/* <S199> */
  { 0x1B,  &GetBitData_Sign			},	/* 符号指令入力有無(SIGN)				*//* 対応 */	/* <S164> *//* 非対応⇒対応 */	/* <S199> */
  { 0x1C,  &GetBitData_Clr			},	/* 偏差クリア信号有無(CLR)				*//* 対応 */	/* <S164> *//* 非対応⇒対応 */	/* <S199> */
  { 0x1D,  &GetBitData_Psel			},	/* 指令パルス入力倍率切替信号(/PSEL)	*//* 対応 */	/* <S164> *//* 非対応⇒対応 */	/* <S199> */
  { 0x20,  &GetBitData_HWBB1		},	/* HWBB1信号(/HWBB1)					*/
  { 0x21,  &GetBitData_HWBB2		},	/* HWBB2信号(/HWBB2)					*//* <S064> */
//{ 0x22,  &CoutSom.UnSomIOSignal	},	/* 安全要求入力信号A1					*//* 非対応 */
//{ 0x23,  &CoutSom.UnSomIOSignal	},	/* 安全要求入力信号A2					*//* 非対応 */
//{ 0x24,  &CoutSom.UnSomIOSignal	},	/* 安全要求入力信号B1					*//* 非対応 */
//{ 0x25,  &CoutSom.UnSomIOSignal	},	/* 安全要求入力信号B2					*//* 非対応 */
//{ 0x40,  &CoutSom.UnSomIOSignal	},	/* 安全要求入力状態A					*//* 非対応 */
//{ 0x41,  &CoutSom.UnSomIOSignal	},	/* 安全要求入力状態B					*//* 非対応 */
  { 0x80,  &GetBitData_AlmFlag		},	/* アラーム発生(ALM)					*/
  { 0x81,  &GetBitData_Coin			},	/* 位置決め完了(/COIN)					*/
  { 0x82,  &GetBitData_Vcmp			},	/* 速度一致(/V-CMP)						*/
  { 0x83,  &GetBitData_Tgon			},	/* モータ回転検出(/TGON)				*/
  { 0x84,  &GetBitData_SvRdy		},	/* サーボレディ(/S-RDY)					*/
  { 0x85,  &GetBitData_Clt			},	/* 電流制限中(CLT)						*/
  { 0x86,  &GetBitData_Vlt			},	/* 速度制限中(/VLT)						*/
  { 0x87,  &GetBitData_Bk			},	/* ブレーキ保持(/BK)					*/
  { 0x88,  &GetBitData_WrnFlag		},	/* ワーニング発生(/WARN)				*/
  { 0x89,  &GetBitData_Near			},	/* 位置決め近傍(/NEAR)					*/
  { 0x8A,  &GetBitData_Cplue		},	/* C相出力(/C-PLUS)						*/
//{ 0x8B,  &ReadDummy.Ulong			},	/* 出力信号1(/OUTPUT1)					*//* 非対応 */
//{ 0x8C,  &ReadDummy.Ulong			},	/* 出力信号2(/OUTPUT2)					*//* 非対応 */
//{ 0x8D,  &ReadDummy.Ulong			},	/* 出力信号3(/OUTPUT3)					*//* 非対応 */
  { 0x8E,  &GetBitData_Alo1			},	/* アラームコード出力1(AL01)			*//* 対応 */	/* <S164> *//* 非対応⇒対応 */
  { 0x8F,  &GetBitData_Alo2			},	/* アラームコード出力2(AL02)			*//* 対応 */	/* <S164> *//* 非対応⇒対応 */
  { 0x90,  &GetBitData_Alo3			},	/* アラームコード出力3(AL03)			*//* 対応 */	/* <S164> *//* 非対応⇒対応 */
  { 0x91,  &GetBitData_Pao			},	/* 分周パルス出力A相(PAO)				*//* 対応 */	/* <S164> *//* 非対応⇒対応 */
  { 0x92,  &GetBitData_Pbo			},	/* 分周パルス出力B相(PBO)				*//* 対応 */	/* <S164> *//* 非対応⇒対応 */
  { 0x93,  &GetBitData_Pco			},	/* 分周パルス出力C相(PCO)				*//* 対応 */	/* <S164> *//* 非対応⇒対応 */
  { 0x94,  &GetBitData_Psela		},	/* 指令パルス入力倍率切替応答(/PSELA)	*//* 対応 */	/* <S164> *//* 非対応⇒対応 */
//{ 0xA0,  &CoutSom.UnSomIOSignal	},	/* 外部機器モニタ信号A					*//* 非対応 */
//{ 0xA1,  &CoutSom.UnSomIOSignal	},	/* 外部機器モニタ信号B					*//* 非対応 */
  { 0xC0,  &GetBitData_Acon			},	/* 主回路電源オン(ACON)					*/
  { 0xC1,  &GetBitData_Refz			},	/* 指令払い出しゼロ(REFZ)				*/
  { 0xC2,  &GetBitData_Pdetcmp		},	/* 磁極検出完了(PDETCMP)				*/
//{ 0xC3,  &CoutSom.UnSomState		},	/* アクティブモード動作状態				*//* 非対応 */
//{ 0xC4,  &CoutSom.UnSomState		},	/* 安全機能動作状態-監視				*//* 非対応 */
//{ 0xC5,  &CoutSom.UnSomState		},	/* 安全機能動作状態-安全				*//* 非対応 */
//{ 0xC6,  &CoutSom.UnSomState		},	/* 安全機能動作状態-HWBB				*//* 非対応 */
//{ 0xC7,  &CoutSom.UnSomState		},	/* 安全機能動作状態-予備				*//* 非対応 */
  { 0xC8,  &GetBitData_Den			},	/* 指令払い出し完了(DEN)				*/
  { 0xC9,  &GetBitData_Pset			},	/* 位置決め完了(PSET)					*/
#if (CSW_NETIF_TYPE == NETIF_M3_TYPE)
  { 0xCA,  &GetBitData_M3CmdRdy		},	/* M-Ⅲコマンドレディ(CMDRDY)			*/
#endif //CSW_NETIF_TYPE == NETIF_M3_TYPE
};
/*--------------------------------------------------------------------------------------------------*/
const	USHORT	TrcBitTblEntNum = sizeof(TrcBitTbl)/sizeof(TrcBitTbl[0]);
/*--------------------------------------------------------------------------------------------------*/

/***************************************** end of file **********************************************/
