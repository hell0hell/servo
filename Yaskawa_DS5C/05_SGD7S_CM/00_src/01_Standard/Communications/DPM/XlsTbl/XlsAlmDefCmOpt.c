/****************************************************************************************************/
/*																									*/
/*																									*/
/*		XlsAlmDefCmOpt.c : アラーム定義ソースファイル												*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1. アラーム定義テーブル																		*/
/*																									*/
/*		2. ワーニング定義テーブル																	*/
/*																									*/
/*		3. アラーム／ワーニング定義個数																*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		2009.08.03 : Made by Excel Macro															*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "XlsDefCmOpt.h"





/****************************************************************************************************/
/*																									*/
/*		アラーム定義テーブル																		*/
/*																									*/
/****************************************************************************************************/
#pragma ghs section rodata="AlmDefTblUser"
const	ALMDEF	AlmDefTblUser[] = {
/*     Code  Attr OL/Stop Out  Led  AttrEx Spare													*/
/*--------------------------------------------------------------------------------------------------*/
	{ 0x04A, 0x0C, 0x0F, 0x00, 0x07, 0x00, 0x00 },	/* ID-128: オプションパラメータ設定異常			*/
	{ 0xB6A, 0x0C, 0x0F, 0x00, 0x07, 0x00, 0x00 },	/* ID-129: 通信ASIC設定異常						*/
	{ 0xB6B, 0x0C, 0x01, 0x00, 0x07, 0x00, 0x00 },	/* ID-130: 通信ASICシステム異常					*/
	{ 0xBFB, 0x0C, 0x0F, 0x00, 0x07, 0x00, 0x00 },	/* ID-131: ScanN実行停止						*/
	{ 0xE00, 0x0C, 0x01, 0x06, 0x07, 0x00, 0x00 },	/* ID-132: オプション同期アクセス要求受信タイムアウト	*/
	{ 0xE03, 0x04, 0x0F, 0x06, 0x07, 0x00, 0x00 },	/* ID-133: DPRAM Checksum error					*/
	{ 0xE40, 0x04, 0x01, 0x06, 0x07, 0x00, 0x00 },	/* ID-134: MECHATROLINK 伝送周期設定異常 for IDM	*/
	{ 0xE50, 0x04, 0x01, 0x06, 0x07, 0x00, 0x00 },	/* ID-135: ネットワーク同期エラー				*/
	{ 0xE51, 0x04, 0x01, 0x06, 0x07, 0x00, 0x00 },	/* ID-136: ネットワーク同期確立失敗				*/
	{ 0xE60, 0x04, 0x01, 0x06, 0x07, 0x00, 0x00 },	/* ID-137: ネットワーク受信エラー				*/
	{ 0xE61, 0x04, 0x01, 0x06, 0x07, 0x00, 0x00 },	/* ID-138: ネットワーク同期フレーム受信異常		*/
	{ 0xEA0, 0x0C, 0x0F, 0x06, 0x07, 0x00, 0x00 },	/* ID-139: サーボパック故障						*/
	{ 0xEA1, 0x0C, 0x0F, 0x06, 0x07, 0x00, 0x00 },	/* ID-140: サーボパック初期アクセス異常			*/
	{ 0xEA2, 0x04, 0x01, 0x06, 0x07, 0x00, 0x00 },	/* ID-141: サーボパックWDC異常					*/
	{ 0xED0, 0x04, 0x01, 0x06, 0x07, 0x00, 0x00 },	/* ID-142: サーボパック応答タイムアウト			*/
	{ 0xED1, 0x04, 0x01, 0x06, 0x07, 0x00, 0x00 },	/* ID-143: コマンド実行未完						*/
};
#pragma ghs section rodata=default
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		ワーニング定義テーブル																		*/
/*																									*/
/****************************************************************************************************/
#pragma ghs section rodata="WrnDefTblUser"
const	ALMDEF	WrnDefTblUser[] = {
/*     Code  Attr OL/Stop Out  Led  AttrEx Spare													*/
/*--------------------------------------------------------------------------------------------------*/
	{ 0x94A, 0x07, 0x00, 0x04, 0x00, 0x00, 0x00 },	/* ID-064: データ設定警告(ユーザ定数番号) for IDM	*/
	{ 0x94B, 0x07, 0x00, 0x04, 0x00, 0x00, 0x00 },	/* ID-065: データ設定警告(データ範囲外) for IDM	*/
	{ 0x94C, 0x07, 0x00, 0x04, 0x00, 0x00, 0x00 },	/* ID-066: データ設定警告(演算異常) for IDM		*/
	{ 0x94D, 0x07, 0x00, 0x04, 0x00, 0x00, 0x00 },	/* ID-067: データ設定警告(データサイズ異常) for IDM	*/
	{ 0x94E, 0x07, 0x00, 0x04, 0x00, 0x00, 0x00 },	/* ID-068: データ設定警告(ラッチモード異常) for IDM	*/
	{ 0x95A, 0x07, 0x00, 0x05, 0x00, 0x00, 0x00 },	/* ID-069: コマンド警告(条件外) for IDM			*/
	{ 0x95B, 0x07, 0x00, 0x05, 0x00, 0x00, 0x00 },	/* ID-070: コマンド警告(未サポート) for IDM		*/
	{ 0x95C, 0x07, 0x00, 0x05, 0x00, 0x00, 0x00 },	/* ID-071: コマンド警告(ユーザ定数による条件外) for IDM	*/
	{ 0x95D, 0x07, 0x00, 0x05, 0x00, 0x00, 0x00 },	/* ID-072: コマンド警告(コマンドの干渉) for IDM	*/
	{ 0x95E, 0x07, 0x00, 0x05, 0x00, 0x00, 0x00 },	/* ID-073: コマンド警告(サブコマンド不可) for IDM	*/
	{ 0x95F, 0x07, 0x00, 0x05, 0x00, 0x00, 0x00 },	/* ID-074: コマンド警告(ポートの干渉) for IDM	*/
	{ 0x960, 0x07, 0x00, 0x06, 0x00, 0x00, 0x00 },	/* ID-075: MECHATROLINK通信警告					*/
};
#pragma ghs section rodata=default
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		アラーム／ワーニング定義個数																*/
/*																									*/
/****************************************************************************************************/
const	USHORT	AlmDefTblUserEntNum = sizeof(AlmDefTblUser)/sizeof(AlmDefTblUser[0]);
const	USHORT	WrnDefTblUserEntNum = sizeof(WrnDefTblUser)/sizeof(WrnDefTblUser[0]);







/***************************************** end of file **********************************************/
