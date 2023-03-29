/****************************************************************************************************/
/*																									*/
/*																									*/
/*		XlsPrmSgdvCmOpt.c : パラメータ定義ソースファイル											*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1. パラメータ定義																			*/
/*																									*/
/*		2. パラメータ管理テーブル																	*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		2013.01.11 : Made by Excel Macro															*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "XlsPrmCmOpt.h"





/****************************************************************************************************/
/*																									*/
/*		パラメータ変数定義																			*/
/*																									*/
/****************************************************************************************************/
#pragma ghs startzda
OPPRMDAT	OpPrm;
#pragma ghs endzda





/****************************************************************************************************/
/*																									*/
/*		パラメータ定義																				*/
/*																									*/
/****************************************************************************************************/
/****************************************************************************************************/
/*		Pn800 : 通信制御機能選択																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comCtrl = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASENBL,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0290,
/* 処理関数ポインタ				*/	pcal_comCtrl,
/* RAMアドレス					*/	&(OpPrm.comCtrl),
/* 下限値						*/	(USHORT)(	0x0000),
/* 上限値						*/	(USHORT)(	0x0F73),
};



/****************************************************************************************************/
/*		Pn801 : ソフトリミット機能選択																*/
/****************************************************************************************************/
const	PRMDEF	pndef_sLimitSw = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASENBL,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0292,
/* 処理関数ポインタ				*/	pcal_sLimitSw,
/* RAMアドレス					*/	&(OpPrm.sLimitSw),
/* 下限値						*/	(USHORT)(	0x0000),
/* 上限値						*/	(USHORT)(	0x0103),
};



/****************************************************************************************************/
/*		Pn802 : コマンド動作設定																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_cmdCtrlSw = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASENBL,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0294,
/* 処理関数ポインタ				*/	pcal_cmdCtrlSw,
/* RAMアドレス					*/	&(OpPrm.cmdCtrlSw),
/* 下限値						*/	(USHORT)(	0x0000),
/* 上限値						*/	(USHORT)(	0x1111),
};



/****************************************************************************************************/
/*		Pn803 : 原点位置範囲 [[ref]]																*/
/****************************************************************************************************/
const	PRMDEF	pndef_zpWidth = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0296,
/* 処理関数ポインタ				*/	pcal_zpWidth,
/* RAMアドレス					*/	&(OpPrm.zpWidth),
/* 下限値						*/	(USHORT)(	0),
/* 上限値						*/	(USHORT)(	250),
};



/****************************************************************************************************/
/*		Pn804 : 正側ソフトリミット値 L [[ref]]														*/
/****************************************************************************************************/
const	PRMDEF	pndef_p_sLimitL = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_W_SIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0298,
/* 処理関数ポインタ				*/	pcal_p_sLimitL,
/* RAMアドレス					*/	&(OpPrm.p_sLimitL),
/* 下限値						*/	(USHORT)(	0x0001),
/* 上限値						*/	(USHORT)(	0xFFFF),
};



/****************************************************************************************************/
/*		Pn805 : 正側ソフトリミット値 H [[ref]]														*/
/****************************************************************************************************/
const	PRMDEF	pndef_p_sLimitH = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_W_SIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x029A,
/* 処理関数ポインタ				*/	pcal_p_sLimitL,
/* RAMアドレス					*/	&(OpPrm.p_sLimitH),
/* 下限値						*/	(USHORT)(	0xC000),
/* 上限値						*/	(USHORT)(	0x3FFF),
};



/****************************************************************************************************/
/*		Pn806 : 負側ソフトリミット値 L [[ref]]														*/
/****************************************************************************************************/
const	PRMDEF	pndef_n_sLimitL = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_W_SIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x029C,
/* 処理関数ポインタ				*/	pcal_n_sLimitL,
/* RAMアドレス					*/	&(OpPrm.n_sLimitL),
/* 下限値						*/	(USHORT)(	0x0001),
/* 上限値						*/	(USHORT)(	0xFFFF),
};



/****************************************************************************************************/
/*		Pn807 : 負側ソフトリミット値 H [[ref]]														*/
/****************************************************************************************************/
const	PRMDEF	pndef_n_sLimitH = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_W_SIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x029E,
/* 処理関数ポインタ				*/	pcal_n_sLimitL,
/* RAMアドレス					*/	&(OpPrm.n_sLimitH),
/* 下限値						*/	(USHORT)(	0xC000),
/* 上限値						*/	(USHORT)(	0x3FFF),
};



/****************************************************************************************************/
/*		Pn808 : アブソＰＧ原点位置オフセット L [[ref]]												*/
/****************************************************************************************************/
const	PRMDEF	pndef_absOffsetL = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_W_SIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x02A0,
/* 処理関数ポインタ				*/	pcal_absOffsetL,
/* RAMアドレス					*/	&(OpPrm.absOffsetL),
/* 下限値						*/	(USHORT)(	0x0001),
/* 上限値						*/	(USHORT)(	0xFFFF),
};



/****************************************************************************************************/
/*		Pn809 : アブソＰＧ原点位置オフセット H [[ref]]												*/
/****************************************************************************************************/
const	PRMDEF	pndef_absOffsetH = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_W_SIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x02A2,
/* 処理関数ポインタ				*/	pcal_absOffsetL,
/* RAMアドレス					*/	&(OpPrm.absOffsetH),
/* 下限値						*/	(USHORT)(	0xC000),
/* 上限値						*/	(USHORT)(	0x3FFF),
};



/****************************************************************************************************/
/*		Pn80A : １段目直線加速定数 [[1000ref/s^s]]													*/
/****************************************************************************************************/
const	PRMDEF	pndef_accRate1 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x02A4,
/* 処理関数ポインタ				*/	pcal_accRate1,
/* RAMアドレス					*/	&(OpPrm.accRate1),
/* 下限値						*/	(USHORT)(	1),
/* 上限値						*/	(USHORT)(	65535),
};



/****************************************************************************************************/
/*		Pn80B : ２段目直線加速定数 [[1000ref/s^s]]													*/
/****************************************************************************************************/
const	PRMDEF	pndef_accRate2 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x02A6,
/* 処理関数ポインタ				*/	pcal_accRate2,
/* RAMアドレス					*/	&(OpPrm.accRate2),
/* 下限値						*/	(USHORT)(	1),
/* 上限値						*/	(USHORT)(	65535),
};



/****************************************************************************************************/
/*		Pn80C : 加速定数切替え速度 [[100ref/s]]														*/
/****************************************************************************************************/
const	PRMDEF	pndef_accChgSpd = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x02A8,
/* 処理関数ポインタ				*/	pcal_accChgSpd,
/* RAMアドレス					*/	&(OpPrm.accChgSpd),
/* 下限値						*/	(USHORT)(	0),
/* 上限値						*/	(USHORT)(	65535),
};



/****************************************************************************************************/
/*		Pn80D : １段目直線減速定数 [[1000ref/s^s]]													*/
/****************************************************************************************************/
const	PRMDEF	pndef_decRate1 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x02AA,
/* 処理関数ポインタ				*/	pcal_decRate1,
/* RAMアドレス					*/	&(OpPrm.decRate1),
/* 下限値						*/	(USHORT)(	1),
/* 上限値						*/	(USHORT)(	65535),
};



/****************************************************************************************************/
/*		Pn80E : ２段目直線減速定数 [[1000ref/s^s]]													*/
/****************************************************************************************************/
const	PRMDEF	pndef_decRate2 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x02AC,
/* 処理関数ポインタ				*/	pcal_decRate2,
/* RAMアドレス					*/	&(OpPrm.decRate2),
/* 下限値						*/	(USHORT)(	1),
/* 上限値						*/	(USHORT)(	65535),
};



/****************************************************************************************************/
/*		Pn80F : 減速定数切替え速度 [[100ref/s]]														*/
/****************************************************************************************************/
const	PRMDEF	pndef_decChgSpd = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x02AE,
/* 処理関数ポインタ				*/	pcal_decChgSpd,
/* RAMアドレス					*/	&(OpPrm.decChgSpd),
/* 下限値						*/	(USHORT)(	0),
/* 上限値						*/	(USHORT)(	65535),
};



/****************************************************************************************************/
/*		Pn810 : 指数関数加減速バイアス [[100ref/s]]													*/
/****************************************************************************************************/
const	PRMDEF	pndef_filExpBias = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x02B0,
/* 処理関数ポインタ				*/	pcal_filExpBias,
/* RAMアドレス					*/	&(OpPrm.filExpBias),
/* 下限値						*/	(USHORT)(	0),
/* 上限値						*/	(USHORT)(	65535),
};



/****************************************************************************************************/
/*		Pn811 : 指数関数加減速時定数 [[0.1ms]]														*/
/****************************************************************************************************/
const	PRMDEF	pndef_filExpTb = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	1,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x02B2,
/* 処理関数ポインタ				*/	pcal_filExpTb,
/* RAMアドレス					*/	&(OpPrm.filExpTb),
/* 下限値						*/	(USHORT)(0),
/* 上限値						*/	(USHORT)(5100),
};



/****************************************************************************************************/
/*		Pn812 : 移動平均時間 [[0.1ms]]																*/
/****************************************************************************************************/
const	PRMDEF	pndef_filMaTb = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	1,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x02B4,
/* 処理関数ポインタ				*/	pcal_filMaTb,
/* RAMアドレス					*/	&(OpPrm.filMaTb),
/* 下限値						*/	(USHORT)(0),
/* 上限値						*/	(USHORT)(5100),
};



/****************************************************************************************************/
/*		Pn813 : 予約																				*/
/****************************************************************************************************/
const	PRMDEF	pndef_resrv2 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x02B6,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.resrv2),
/* 下限値						*/	(USHORT)(	0),
/* 上限値						*/	(USHORT)(	65535),
};



/****************************************************************************************************/
/*		Pn814 : 外部位置決め最終走行距離 L [[ref]]													*/
/****************************************************************************************************/
const	PRMDEF	pndef_exDistL = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_W_SIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x02B8,
/* 処理関数ポインタ				*/	pcal_exDistL,
/* RAMアドレス					*/	&(OpPrm.exDistL),
/* 下限値						*/	(USHORT)(	0x0001),
/* 上限値						*/	(USHORT)(	0xFFFF),
};



/****************************************************************************************************/
/*		Pn815 : 外部位置決め最終走行距離 H [[ref]]													*/
/****************************************************************************************************/
const	PRMDEF	pndef_exDistH = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_W_SIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x02BA,
/* 処理関数ポインタ				*/	pcal_exDistL,
/* RAMアドレス					*/	&(OpPrm.exDistH),
/* 下限値						*/	(USHORT)(	0xC000),
/* 上限値						*/	(USHORT)(	0x3FFF),
};



/****************************************************************************************************/
/*		Pn816 : 原点復帰モード設定																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_zretDir = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASENBL,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x02BC,
/* 処理関数ポインタ				*/	pcal_zretDir,
/* RAMアドレス					*/	&(OpPrm.zretDir),
/* 下限値						*/	(USHORT)(	0x0),
/* 上限値						*/	(USHORT)(	0x1),
};



/****************************************************************************************************/
/*		Pn817 : 原点復帰アプローチ速度１ [[100ref/s]]												*/
/****************************************************************************************************/
const	PRMDEF	pndef_zretSpd1 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x02BE,
/* 処理関数ポインタ				*/	pcal_zretSpd1,
/* RAMアドレス					*/	&(OpPrm.zretSpd1),
/* 下限値						*/	(USHORT)(	0),
/* 上限値						*/	(USHORT)(	65535),
};



/****************************************************************************************************/
/*		Pn818 : 原点復帰アプローチ速度２ [[100ref/s]]												*/
/****************************************************************************************************/
const	PRMDEF	pndef_zretSpd2 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x02C0,
/* 処理関数ポインタ				*/	pcal_zretSpd2,
/* RAMアドレス					*/	&(OpPrm.zretSpd2),
/* 下限値						*/	(USHORT)(	0),
/* 上限値						*/	(USHORT)(	65535),
};



/****************************************************************************************************/
/*		Pn819 : 原点復帰最終走行距離 L [[ref]]														*/
/****************************************************************************************************/
const	PRMDEF	pndef_zretDistL = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_W_SIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x02C2,
/* 処理関数ポインタ				*/	pcal_zretDistL,
/* RAMアドレス					*/	&(OpPrm.zretDistL),
/* 下限値						*/	(USHORT)(	0x0001),
/* 上限値						*/	(USHORT)(	0xFFFF),
};



/****************************************************************************************************/
/*		Pn81A : 原点復帰最終走行距離 H [[ref]]														*/
/****************************************************************************************************/
const	PRMDEF	pndef_zretDistH = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_W_SIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x02C4,
/* 処理関数ポインタ				*/	pcal_zretDistL,
/* RAMアドレス					*/	&(OpPrm.zretDistH),
/* 下限値						*/	(USHORT)(	0xC000),
/* 上限値						*/	(USHORT)(	0x3FFF),
};



/****************************************************************************************************/
/*		Pn81B : 予約																				*/
/****************************************************************************************************/
const	PRMDEF	pndef_resrv3 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x02C6,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.resrv3),
/* 下限値						*/	(USHORT)(	0),
/* 上限値						*/	(USHORT)(	65535),
};



/****************************************************************************************************/
/*		Pn81C : 予約																				*/
/****************************************************************************************************/
const	PRMDEF	pndef_resrv4 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x02C8,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.resrv4),
/* 下限値						*/	(USHORT)(	0),
/* 上限値						*/	(USHORT)(	65535),
};



/****************************************************************************************************/
/*		Pn81D : 予約																				*/
/****************************************************************************************************/
const	PRMDEF	pndef_resrv5 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x02CA,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.resrv5),
/* 下限値						*/	(USHORT)(	0),
/* 上限値						*/	(USHORT)(	65535),
};



/****************************************************************************************************/
/*		Pn81E : 入力信号モニタ選択																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_ioMonSel = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASENBL,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x02CC,
/* 処理関数ポインタ				*/	pcal_ioMonSel,
/* RAMアドレス					*/	&(OpPrm.ioMonSel),
/* 下限値						*/	(USHORT)(	0),
/* 上限値						*/	(USHORT)(	0x7777),
};



/****************************************************************************************************/
/*		Pn81F : コマンドデータ割付選択																*/
/****************************************************************************************************/
const	PRMDEF	pndef_optBitSw = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASENBL,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x02CE,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.optBitSw),
/* 下限値						*/	(USHORT)(	0x0),
/* 上限値						*/	(USHORT)(	0x1111),
};



/****************************************************************************************************/
/*		Pn820 : 正側ラッチ可能領域 L [[ref]]														*/
/****************************************************************************************************/
const	PRMDEF	pndef_p_ltLimitL = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_W_SIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x02D0,
/* 処理関数ポインタ				*/	pcal_p_ltLimitL,
/* RAMアドレス					*/	&(OpPrm.p_ltLimitL),
/* 下限値						*/	(USHORT)(	0x0000),
/* 上限値						*/	(USHORT)(	0xFFFF),
};



/****************************************************************************************************/
/*		Pn821 : 正側ラッチ可能領域 H [[ref]]														*/
/****************************************************************************************************/
const	PRMDEF	pndef_p_ltLimitH = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_W_SIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x02D2,
/* 処理関数ポインタ				*/	pcal_p_ltLimitL,
/* RAMアドレス					*/	&(OpPrm.p_ltLimitH),
/* 下限値						*/	(USHORT)(	0x8000),
/* 上限値						*/	(USHORT)(	0x7FFF),
};



/****************************************************************************************************/
/*		Pn822 : 逆側ラッチ可能領域 L [[ref]]														*/
/****************************************************************************************************/
const	PRMDEF	pndef_n_ltLimitL = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_W_SIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x02D4,
/* 処理関数ポインタ				*/	pcal_n_ltLimitL,
/* RAMアドレス					*/	&(OpPrm.n_ltLimitL),
/* 下限値						*/	(USHORT)(	0x0000),
/* 上限値						*/	(USHORT)(	0xFFFF),
};



/****************************************************************************************************/
/*		Pn823 : 逆側ラッチ可能領域 H [[ref]]														*/
/****************************************************************************************************/
const	PRMDEF	pndef_n_ltLimitH = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_W_SIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x02D6,
/* 処理関数ポインタ				*/	pcal_n_ltLimitL,
/* RAMアドレス					*/	&(OpPrm.n_ltLimitH),
/* 下限値						*/	(USHORT)(	0x8000),
/* 上限値						*/	(USHORT)(	0x7FFF),
};



/****************************************************************************************************/
/*		Pn824 : オプションモニタ１選択																*/
/****************************************************************************************************/
const	PRMDEF	pndef_opMon1 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x02D8,
/* 処理関数ポインタ				*/	pcal_opMon1,
/* RAMアドレス					*/	&(OpPrm.opMon1),
/* 下限値						*/	(USHORT)(	0),
/* 上限値						*/	(USHORT)(	0xFFFF),
};



/****************************************************************************************************/
/*		Pn825 : オプションモニタ２選択																*/
/****************************************************************************************************/
const	PRMDEF	pndef_opMon2 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x02DA,
/* 処理関数ポインタ				*/	pcal_opMon2,
/* RAMアドレス					*/	&(OpPrm.opMon2),
/* 下限値						*/	(USHORT)(	0),
/* 上限値						*/	(USHORT)(	0xFFFF),
};



/****************************************************************************************************/
/*		Pn826 : 非常停止用１段目直線減速定数 [[1000ref/s^s]]										*/
/****************************************************************************************************/
const	PRMDEF	pndef_decRate1Emg = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER2,
/* EEPROMアドレス				*/	0x02DC,
/* 処理関数ポインタ				*/	pcal_decRate1Emg,
/* RAMアドレス					*/	&(OpPrm.decRate1Emg),
/* 下限値						*/	(USHORT)(	1),
/* 上限値						*/	(USHORT)(	65535),
};



/****************************************************************************************************/
/*		Pn827 : 非常停止用２段目直線減速定数 [[1000ref/s^s]]										*/
/****************************************************************************************************/
const	PRMDEF	pndef_decRate2Emg = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x02DE,
/* 処理関数ポインタ				*/	pcal_decRate2Emg,
/* RAMアドレス					*/	&(OpPrm.decRate2Emg),
/* 下限値						*/	(USHORT)(	1),
/* 上限値						*/	(USHORT)(	65535),
};



/****************************************************************************************************/
/*		Pn828 : 非常停止用減速定数切替え速度 [[100ref/s]]											*/
/****************************************************************************************************/
const	PRMDEF	pndef_decChgSpdEmg = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER2,
/* EEPROMアドレス				*/	0x02E0,
/* 処理関数ポインタ				*/	pcal_decChgSpdEmg,
/* RAMアドレス					*/	&(OpPrm.decChgSpdEmg),
/* 下限値						*/	(USHORT)(	0),
/* 上限値						*/	(USHORT)(	65535),
};



/****************************************************************************************************/
/*		Pn829 : SVOFF待ち時間(減速停止SVOFF時)														*/
/****************************************************************************************************/
const	PRMDEF	pndef_svOffTime = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x02E2,
/* 処理関数ポインタ				*/	pcal_svOffTime,
/* RAMアドレス					*/	&(OpPrm.svOffTime),
/* 下限値						*/	(USHORT)(	0),
/* 上限値						*/	(USHORT)(	65535),
};



/****************************************************************************************************/
/*		Pn82A : OPTION機能ビット割付１																*/
/****************************************************************************************************/
const	PRMDEF	pndef_optbit1 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASENBL,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x02E4,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.optbit1),
/* 下限値						*/	(USHORT)(	0x0000),
/* 上限値						*/	(USHORT)(	0x1E1E),
};



/****************************************************************************************************/
/*		Pn82B : OPTION機能ビット割付２																*/
/****************************************************************************************************/
const	PRMDEF	pndef_optbit2 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASENBL,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x02E6,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.optbit2),
/* 下限値						*/	(USHORT)(	0x0000),
/* 上限値						*/	(USHORT)(	0x1F1F),
};



/****************************************************************************************************/
/*		Pn82C : OPTION機能ビット割付３																*/
/****************************************************************************************************/
const	PRMDEF	pndef_optbit3 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASENBL,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x02E8,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.optbit3),
/* 下限値						*/	(USHORT)(	0x0000),
/* 上限値						*/	(USHORT)(	0x1F1F),
};



/****************************************************************************************************/
/*		Pn82D : OPTION機能ビット割付４																*/
/****************************************************************************************************/
const	PRMDEF	pndef_optbit4 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASENBL,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x02EA,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.optbit4),
/* 下限値						*/	(USHORT)(	0x0000),
/* 上限値						*/	(USHORT)(	0x1F1C),
};



/****************************************************************************************************/
/*		Pn82E : OPTION機能ビット割付５																*/
/****************************************************************************************************/
const	PRMDEF	pndef_optbit5 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASENBL,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x02EC,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.optbit5),
/* 下限値						*/	(USHORT)(	0x0000),
/* 上限値						*/	(USHORT)(	0x1D1F),
};



/****************************************************************************************************/
/*		Pn82F : 予約																				*/
/****************************************************************************************************/
const	PRMDEF	pndef_resrv6 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASENBL,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER2,
/* EEPROMアドレス				*/	0x02EE,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.resrv6),
/* 下限値						*/	(USHORT)(	0x0000),
/* 上限値						*/	(USHORT)(	0x1D1F),
};



/****************************************************************************************************/
/*		Pn830 : 機械一回転当たりの指令単位量L [[ref]]												*/
/****************************************************************************************************/
const	PRMDEF	pndef_rotrefL = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER2,
/* EEPROMアドレス				*/	0x02F0,
/* 処理関数ポインタ				*/	pcal_rotrefL,
/* RAMアドレス					*/	&(OpPrm.rotrefL),
/* 下限値						*/	(USHORT)(	0x0001),
/* 上限値						*/	(USHORT)(	0xFFFF),
};



/****************************************************************************************************/
/*		Pn831 : 機械一回転当たりの指令単位量H [[ref]]												*/
/****************************************************************************************************/
const	PRMDEF	pndef_rotrefH = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER2,
/* EEPROMアドレス				*/	0x02F2,
/* 処理関数ポインタ				*/	pcal_rotrefL,
/* RAMアドレス					*/	&(OpPrm.rotrefH),
/* 下限値						*/	(USHORT)(	0x0000),
/* 上限値						*/	(USHORT)(	0x7FFF),
};



/****************************************************************************************************/
/*		Pn832 : 機械一回転当たりのステーション数													*/
/****************************************************************************************************/
const	PRMDEF	pndef_st_num = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER2,
/* EEPROMアドレス				*/	0x02F4,
/* 処理関数ポインタ				*/	pcal_st_num,
/* RAMアドレス					*/	&(OpPrm.st_num),
/* 下限値						*/	(USHORT)(0),
/* 上限値						*/	(USHORT)(32767),
};



/****************************************************************************************************/
/*		Pn833 : POSING定数選択																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_rateChgSw = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASENBL,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x02F6,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.rateChgSw),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0x0001),
};



/****************************************************************************************************/
/*		Pn834 : １段目直線加速定数 2nd L [[1000ref/s^s]]											*/
/****************************************************************************************************/
const	PRMDEF	pndef_accRate12L = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x02F8,
/* 処理関数ポインタ				*/	pcal_accRate12L,
/* RAMアドレス					*/	&(OpPrm.accRate12L),
/* 下限値						*/	(USHORT)(0x0001),
/* 上限値						*/	(USHORT)(0x0000),
};



/****************************************************************************************************/
/*		Pn835 : １段目直線加速定数 2nd H [[1000ref/s^s]]											*/
/****************************************************************************************************/
const	PRMDEF	pndef_accRate12H = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x02FA,
/* 処理関数ポインタ				*/	pcal_accRate12L,
/* RAMアドレス					*/	&(OpPrm.accRate12H),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0x0140),
};



/****************************************************************************************************/
/*		Pn836 : ２段目直線加速定数 2nd L [[1000ref/s^s]]											*/
/****************************************************************************************************/
const	PRMDEF	pndef_accRate22L = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x02FC,
/* 処理関数ポインタ				*/	pcal_accRate22L,
/* RAMアドレス					*/	&(OpPrm.accRate22L),
/* 下限値						*/	(USHORT)(0x0001),
/* 上限値						*/	(USHORT)(0x0000),
};



/****************************************************************************************************/
/*		Pn837 : ２段目直線加速定数 2nd H [[1000ref/s^s]]											*/
/****************************************************************************************************/
const	PRMDEF	pndef_accRate22H = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x02FE,
/* 処理関数ポインタ				*/	pcal_accRate22L,
/* RAMアドレス					*/	&(OpPrm.accRate22H),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0x0140),
};



/****************************************************************************************************/
/*		Pn838 : 加速定数切替え速度 2nd L [[ref/s]]													*/
/****************************************************************************************************/
const	PRMDEF	pndef_accChgSpd2L = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0300,
/* 処理関数ポインタ				*/	pcal_accChgSpd2L,
/* RAMアドレス					*/	&(OpPrm.accChgSpd2L),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0x0000),
};



/****************************************************************************************************/
/*		Pn839 : 加速定数切替え速度 2nd H [[ref/s]]													*/
/****************************************************************************************************/
const	PRMDEF	pndef_accChgSpd2H = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0302,
/* 処理関数ポインタ				*/	pcal_accChgSpd2L,
/* RAMアドレス					*/	&(OpPrm.accChgSpd2H),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0x7D00),
};



/****************************************************************************************************/
/*		Pn83A : １段目直線減速定数 2nd L [[1000ref/s^s]]											*/
/****************************************************************************************************/
const	PRMDEF	pndef_decRate12L = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0304,
/* 処理関数ポインタ				*/	pcal_decRate12L,
/* RAMアドレス					*/	&(OpPrm.decRate12L),
/* 下限値						*/	(USHORT)(0x0001),
/* 上限値						*/	(USHORT)(0x0000),
};



/****************************************************************************************************/
/*		Pn83B : １段目直線減速定数 2nd H [[1000ref/s^s]]											*/
/****************************************************************************************************/
const	PRMDEF	pndef_decRate12H = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0306,
/* 処理関数ポインタ				*/	pcal_decRate12L,
/* RAMアドレス					*/	&(OpPrm.decRate12H),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0x0140),
};



/****************************************************************************************************/
/*		Pn83C : ２段目直線減速定数 2nd L [[1000ref/s^s]]											*/
/****************************************************************************************************/
const	PRMDEF	pndef_decRate22L = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0308,
/* 処理関数ポインタ				*/	pcal_decRate22L,
/* RAMアドレス					*/	&(OpPrm.decRate22L),
/* 下限値						*/	(USHORT)(0x0001),
/* 上限値						*/	(USHORT)(0x0000),
};



/****************************************************************************************************/
/*		Pn83D : ２段目直線減速定数 2nd H [[1000ref/s^s]]											*/
/****************************************************************************************************/
const	PRMDEF	pndef_decRate22H = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x030A,
/* 処理関数ポインタ				*/	pcal_decRate22L,
/* RAMアドレス					*/	&(OpPrm.decRate22H),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0x0140),
};



/****************************************************************************************************/
/*		Pn83E : 減速定数切替え速度 2nd L [[ref/s]]													*/
/****************************************************************************************************/
const	PRMDEF	pndef_decChgSpd2L = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x030C,
/* 処理関数ポインタ				*/	pcal_decChgSpd2L,
/* RAMアドレス					*/	&(OpPrm.decChgSpd2L),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0x0000),
};



/****************************************************************************************************/
/*		Pn83F : 減速定数切替え速度 2nd H [[ref/s]]													*/
/****************************************************************************************************/
const	PRMDEF	pndef_decChgSpd2H = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x030E,
/* 処理関数ポインタ				*/	pcal_decChgSpd2L,
/* RAMアドレス					*/	&(OpPrm.decChgSpd2H),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0x7D00),
};



/****************************************************************************************************/
/*		Pn840 : 非常停止用２段目直線減速定数　2nd L [[1000ref/s^s]]									*/
/****************************************************************************************************/
const	PRMDEF	pndef_decRateEmg2L = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0310,
/* 処理関数ポインタ				*/	pcal_decRateEmg2L,
/* RAMアドレス					*/	&(OpPrm.decRateEmg2L),
/* 下限値						*/	(USHORT)(0x0001),
/* 上限値						*/	(USHORT)(0x0000),
};



/****************************************************************************************************/
/*		Pn841 : 非常停止用２段目直線減速定数　2nd H [[1000ref/s^s]]									*/
/****************************************************************************************************/
const	PRMDEF	pndef_decRateEmg2H = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0312,
/* 処理関数ポインタ				*/	pcal_decRateEmg2L,
/* RAMアドレス					*/	&(OpPrm.decRateEmg2H),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0x0140),
};



/****************************************************************************************************/
/*		Pn842 : 原点復帰アプローチ速度１ 2nd L [[100ref/s]]											*/
/****************************************************************************************************/
const	PRMDEF	pndef_zretSpd12L = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x06,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0345,
/* 処理関数ポインタ				*/	pcal_zretSpd12,
/* RAMアドレス					*/	&(OpPrm.zretSpd12L),
/* 下限値						*/	(USHORT)(	0),
/* 上限値						*/	(USHORT)(0x0000),
};



/****************************************************************************************************/
/*		Pn843 : 原点復帰アプローチ速度１ 2nd H [[100ref/s]]											*/
/****************************************************************************************************/
const	PRMDEF	pndef_zretSpd12H = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x06,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0347,
/* 処理関数ポインタ				*/	pcal_zretSpd12,
/* RAMアドレス					*/	&(OpPrm.zretSpd12H),
/* 下限値						*/	(USHORT)(	0),
/* 上限値						*/	(USHORT)(0x0140),
};



/****************************************************************************************************/
/*		Pn844 : 原点復帰アプローチ速度２ 2nd L [[100ref/s]]											*/
/****************************************************************************************************/
const	PRMDEF	pndef_zretSpd22L = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x06,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0349,
/* 処理関数ポインタ				*/	pcal_zretSpd22,
/* RAMアドレス					*/	&(OpPrm.zretSpd22L),
/* 下限値						*/	(USHORT)(	0),
/* 上限値						*/	(USHORT)(0x0000),
};



/****************************************************************************************************/
/*		Pn845 : 原点復帰アプローチ速度２ 2nd H [[100ref/s]]											*/
/****************************************************************************************************/
const	PRMDEF	pndef_zretSpd22H = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x06,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x034B,
/* 処理関数ポインタ				*/	pcal_zretSpd22,
/* RAMアドレス					*/	&(OpPrm.zretSpd22H),
/* 下限値						*/	(USHORT)(	0),
/* 上限値						*/	(USHORT)(0x0140),
};



/****************************************************************************************************/
/*		Pn850 : 連続ラッチシーケンス数																*/
/****************************************************************************************************/
const	PRMDEF	pndef_ltSeqNum = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0314,
/* 処理関数ポインタ				*/	pcal_ltSeqNum,
/* RAMアドレス					*/	&(OpPrm.ltSeqNum),
/* 下限値						*/	(USHORT)(0),
/* 上限値						*/	(USHORT)(8),
};



/****************************************************************************************************/
/*		Pn851 : 連続ラッチ回数																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_ltLoopNum = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0316,
/* 処理関数ポインタ				*/	pcal_ltLoopNum,
/* RAMアドレス					*/	&(OpPrm.ltLoopNum),
/* 下限値						*/	(USHORT)(0),
/* 上限値						*/	(USHORT)(255),
};



/****************************************************************************************************/
/*		Pn852 : 連続ラッチシーケンス設定1-4															*/
/****************************************************************************************************/
const	PRMDEF	pndef_ltSeqSet1 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASENBL,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0318,
/* 処理関数ポインタ				*/	pcal_ltSeqSet1,
/* RAMアドレス					*/	&(OpPrm.ltSeqSet1),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0x3333),
};



/****************************************************************************************************/
/*		Pn853 : 連続ラッチシーケンス設定4-8															*/
/****************************************************************************************************/
const	PRMDEF	pndef_ltSeqSet2 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASENBL,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x031A,
/* 処理関数ポインタ				*/	pcal_ltSeqSet2,
/* RAMアドレス					*/	&(OpPrm.ltSeqSet2),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0x3333),
};



/****************************************************************************************************/
/*		Pn880 : 局アドレス設定																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_nodeAdr = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.nodeAdr),
/* 下限値						*/	(USHORT)(0),
/* 上限値						*/	(USHORT)(65535),
};



/****************************************************************************************************/
/*		Pn881 : 伝送バイト数																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_trByte = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.trByte),
/* 下限値						*/	(USHORT)(0),
/* 上限値						*/	(USHORT)(65535),
};



/****************************************************************************************************/
/*		Pn882 : 伝送周期																			*/
/****************************************************************************************************/
const	PRMDEF	pndef_comTCycle = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.comTCycle),
/* 下限値						*/	(USHORT)(0),
/* 上限値						*/	(USHORT)(65535),
};



/****************************************************************************************************/
/*		Pn883 : 通信周期																			*/
/****************************************************************************************************/
const	PRMDEF	pndef_comAplCycle = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.comAplCycle),
/* 下限値						*/	(USHORT)(0),
/* 上限値						*/	(USHORT)(65535),
};



/****************************************************************************************************/
/*		Pn88A : 受信エラーカウンタ																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comErrCnt = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.comErrCnt),
/* 下限値						*/	(USHORT)(0),
/* 上限値						*/	(USHORT)(0),
};



/****************************************************************************************************/
/*		Pn88B : (予約)																				*/
/****************************************************************************************************/
const	PRMDEF	pndef_comErrSts1 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER2,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.comErrSts1),
/* 下限値						*/	(USHORT)(0),
/* 上限値						*/	(USHORT)(0),
};



/****************************************************************************************************/
/*		Pn88C : 通信エラー発生時の、エラーステータス(累積)											*/
/****************************************************************************************************/
const	PRMDEF	pndef_comErrSts2 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER2,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.comErrSts2),
/* 下限値						*/	(USHORT)(0),
/* 上限値						*/	(USHORT)(0),
};



/****************************************************************************************************/
/*		Pn88F : 通信CMD/RSPデータ制御																*/
/****************************************************************************************************/
const	PRMDEF	pndef_comDataCtrl = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASENBL,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER2,
/* EEPROMアドレス				*/	0x031C,
/* 処理関数ポインタ				*/	pcal_comDataCtrl,
/* RAMアドレス					*/	&(OpPrm.comDataCtrl),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn890 : アラーム時、通信CMDデータ															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almCmd0 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.almCmd[0]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn891 : アラーム時、通信CMDデータ															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almCmd1 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.almCmd[1]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn892 : アラーム時、通信CMDデータ															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almCmd2 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.almCmd[2]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn893 : アラーム時、通信CMDデータ															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almCmd3 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.almCmd[3]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn894 : アラーム時、通信CMDデータ															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almCmd4 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.almCmd[4]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn895 : アラーム時、通信CMDデータ															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almCmd5 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.almCmd[5]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn896 : アラーム時、通信CMDデータ															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almCmd6 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.almCmd[6]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn897 : アラーム時、通信CMDデータ															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almCmd7 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.almCmd[7]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn898 : アラーム時、通信CMDデータ															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almCmd8 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.almCmd[8]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn899 : アラーム時、通信CMDデータ															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almCmd9 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.almCmd[9]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn89A : アラーム時、通信CMDデータ															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almCmd10 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.almCmd[10]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn89B : アラーム時、通信CMDデータ															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almCmd11 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.almCmd[11]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn89C : アラーム時、通信CMDデータ															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almCmd12 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.almCmd[12]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn89D : アラーム時、通信CMDデータ															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almCmd13 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.almCmd[13]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn89E : アラーム時、通信CMDデータ															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almCmd14 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.almCmd[14]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn89F : アラーム時、通信CMDデータ															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almCmd15 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.almCmd[15]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8A0 : アラーム時、通信RSPデータ															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almRsp0 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.almRsp[0]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8A1 : アラーム時、通信RSPデータ															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almRsp1 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.almRsp[1]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8A2 : アラーム時、通信RSPデータ															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almRsp2 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.almRsp[2]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8A3 : アラーム時、通信RSPデータ															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almRsp3 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.almRsp[3]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8A4 : アラーム時、通信RSPデータ															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almRsp4 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.almRsp[4]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8A5 : アラーム時、通信RSPデータ															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almRsp5 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.almRsp[5]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8A6 : アラーム時、通信RSPデータ															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almRsp6 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.almRsp[6]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8A7 : アラーム時、通信RSPデータ															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almRsp7 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.almRsp[7]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8A8 : アラーム時、通信RSPデータ															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almRsp8 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.almRsp[8]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8A9 : アラーム時、通信RSPデータ															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almRsp9 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.almRsp[9]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8AA : アラーム時、通信RSPデータ															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almRsp10 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.almRsp[10]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8AB : アラーム時、通信RSPデータ															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almRsp11 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.almRsp[11]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8AC : アラーム時、通信RSPデータ															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almRsp12 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.almRsp[12]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8AD : アラーム時、通信RSPデータ															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almRsp13 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.almRsp[13]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8AE : アラーム時、通信RSPデータ															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almRsp14 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.almRsp[14]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8AF : アラーム時、通信RSPデータ															*/
/****************************************************************************************************/
const	PRMDEF	pndef_almRsp15 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.almRsp[15]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8B0 : Online通信CMDデータ																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comCmd0 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER2,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.comCmd[0]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8B1 : Online通信CMDデータ																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comCmd1 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER2,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.comCmd[1]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8B2 : Online通信CMDデータ																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comCmd2 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER2,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.comCmd[2]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8B3 : Online通信CMDデータ																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comCmd3 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER2,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.comCmd[3]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8B4 : Online通信CMDデータ																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comCmd4 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER2,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.comCmd[4]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8B5 : Online通信CMDデータ																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comCmd5 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER2,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.comCmd[5]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8B6 : Online通信CMDデータ																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comCmd6 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER2,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.comCmd[6]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8B7 : Online通信CMDデータ																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comCmd7 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER2,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.comCmd[7]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8B8 : Online通信CMDデータ																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comCmd8 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER2,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.comCmd[8]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8B9 : Online通信CMDデータ																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comCmd9 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER2,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.comCmd[9]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8BA : Online通信CMDデータ																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comCmd10 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER2,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.comCmd[10]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8BB : Online通信CMDデータ																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comCmd11 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER2,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.comCmd[11]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8BC : Online通信CMDデータ																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comCmd12 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER2,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.comCmd[12]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8BD : Online通信CMDデータ																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comCmd13 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER2,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.comCmd[13]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8BE : Online通信CMDデータ																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comCmd14 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER2,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.comCmd[14]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8BF : Online通信CMDデータ																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comCmd15 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER2,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.comCmd[15]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8C0 : Online通信RSPデータ																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comRsp0 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER2,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.comRsp[0]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8C1 : Online通信RSPデータ																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comRsp1 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER2,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.comRsp[1]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8C2 : Online通信RSPデータ																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comRsp2 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER2,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.comRsp[2]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8C3 : Online通信RSPデータ																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comRsp3 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER2,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.comRsp[3]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8C4 : Online通信RSPデータ																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comRsp4 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER2,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.comRsp[4]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8C5 : Online通信RSPデータ																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comRsp5 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER2,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.comRsp[5]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8C6 : Online通信RSPデータ																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comRsp6 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER2,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.comRsp[6]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8C7 : Online通信RSPデータ																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comRsp7 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER2,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.comRsp[7]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8C8 : Online通信RSPデータ																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comRsp8 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER2,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.comRsp[8]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8C9 : Online通信RSPデータ																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comRsp9 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER2,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.comRsp[9]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8CA : Online通信RSPデータ																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comRsp10 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER2,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.comRsp[10]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8CB : Online通信RSPデータ																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comRsp11 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER2,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.comRsp[11]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8CC : Online通信RSPデータ																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comRsp12 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER2,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.comRsp[12]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8CD : Online通信RSPデータ																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comRsp13 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER2,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.comRsp[13]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8CE : Online通信RSPデータ																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comRsp14 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER2,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.comRsp[14]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8CF : Online通信RSPデータ																	*/
/****************************************************************************************************/
const	PRMDEF	pndef_comRsp15 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER2,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.comRsp[15]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8E0 : 指令Option用 Option Card ID															*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardID = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.OpCardID),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8E1 : 指令Option用 Option 動作設定														*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardCtrlSw = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASENBL,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.OpCardCtrlSw),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0x0001),
};



/****************************************************************************************************/
/*		Pn8E2 : 指令Option用 Option S/W Ver.														*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardSwVerL = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.OpCardSwVerL),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8E3 : 指令Option用 Option S/W Ver.														*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardSwVerH = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.OpCardSwVerH),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8E4 : 指令Option用 Option Y Spec. No.														*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardYNo = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.OpCardYNo),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8E5 : 指令Option用 ManufactureDate.														*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardDate = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.OpCardDate),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8E6 : 指令Option用 サーボSW設定															*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardSvSw = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.OpCardSvSw),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8E8 : 指令Option用 アラーム設定															*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardAlmCode = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_SetOpCardAlarm,
/* RAMアドレス					*/	&(OpPrm.OpCardAlmCode),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8E9 : 指令Option用 アラーム設定															*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardAlmAttr = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	TRUE,
/* :1: ワード順					*/	1,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_SetOpCardAlarm,
/* RAMアドレス					*/	&(OpPrm.OpCardAlmAttr),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8F0 : 指令Option用 Option Model															*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardModel0 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.OpCardModel[0]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8F1 : 指令Option用 Option Model															*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardModel1 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.OpCardModel[1]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8F2 : 指令Option用 Option Model															*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardModel2 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.OpCardModel[2]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8F3 : 指令Option用 Option Model															*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardModel3 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.OpCardModel[3]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8F4 : 指令Option用 Option Model															*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardModel4 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.OpCardModel[4]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8F5 : 指令Option用 Option Model															*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardModel5 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.OpCardModel[5]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8F6 : 指令Option用 Option Model															*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardModel6 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.OpCardModel[6]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8F7 : 指令Option用 Option Model															*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardModel7 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.OpCardModel[7]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8F8 : 指令Option用 Serial No																*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardSNo0 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.OpCardSNo[0]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8F9 : 指令Option用 Serial No																*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardSNo1 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.OpCardSNo[1]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8FA : 指令Option用 Serial No																*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardSNo2 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.OpCardSNo[2]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8FB : 指令Option用 Serial No																*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardSNo3 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.OpCardSNo[3]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8FC : 指令Option用 Serial No																*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardSNo4 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.OpCardSNo[4]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8FD : 指令Option用 Serial No																*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardSNo5 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.OpCardSNo[5]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8FE : 指令Option用 Serial No																*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardSNo6 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.OpCardSNo[6]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn8FF : 指令Option用 Serial No																*/
/****************************************************************************************************/
const	PRMDEF	pndef_OpCardSNo7 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.OpCardSNo[7]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn900 : バンク数																			*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkNum = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x031E,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.bnkNum),
/* 下限値						*/	(USHORT)(0),
/* 上限値						*/	(USHORT)(16),
};



/****************************************************************************************************/
/*		Pn901 : バンクメンバ数																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkMemNum = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEDEC,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0320,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.bnkMemNum),
/* 下限値						*/	(USHORT)(0),
/* 上限値						*/	(USHORT)(15),
};



/****************************************************************************************************/
/*		Pn902 : バンクメンバ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkMember0 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0322,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.bnkMember[0x0]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0x08FF),
};



/****************************************************************************************************/
/*		Pn903 : バンクメンバ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkMember1 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0324,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.bnkMember[0x1]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0x08FF),
};



/****************************************************************************************************/
/*		Pn904 : バンクメンバ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkMember2 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0326,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.bnkMember[0x2]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0x08FF),
};



/****************************************************************************************************/
/*		Pn905 : バンクメンバ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkMember3 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0328,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.bnkMember[0x3]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0x08FF),
};



/****************************************************************************************************/
/*		Pn906 : バンクメンバ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkMember4 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x032A,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.bnkMember[0x4]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0x08FF),
};



/****************************************************************************************************/
/*		Pn907 : バンクメンバ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkMember5 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x032C,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.bnkMember[0x5]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0x08FF),
};



/****************************************************************************************************/
/*		Pn908 : バンクメンバ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkMember6 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x032E,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.bnkMember[0x6]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0x08FF),
};



/****************************************************************************************************/
/*		Pn909 : バンクメンバ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkMember7 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0330,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.bnkMember[0x7]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0x08FF),
};



/****************************************************************************************************/
/*		Pn90A : バンクメンバ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkMember8 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0332,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.bnkMember[0x8]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0x08FF),
};



/****************************************************************************************************/
/*		Pn90B : バンクメンバ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkMember9 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0334,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.bnkMember[0x9]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0x08FF),
};



/****************************************************************************************************/
/*		Pn90C : バンクメンバ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkMemberA = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0336,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.bnkMember[0xA]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0x08FF),
};



/****************************************************************************************************/
/*		Pn90D : バンクメンバ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkMemberB = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0338,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.bnkMember[0xB]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0x08FF),
};



/****************************************************************************************************/
/*		Pn90E : バンクメンバ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkMemberC = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x033A,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.bnkMember[0xC]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0x08FF),
};



/****************************************************************************************************/
/*		Pn90F : バンクメンバ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkMemberD = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x033C,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.bnkMember[0xD]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0x08FF),
};



/****************************************************************************************************/
/*		Pn910 : バンクメンバ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkMemberE = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x033E,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.bnkMember[0xE]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0x08FF),
};



/****************************************************************************************************/
/*		Pn920 : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData00 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData00,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x00]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn921 : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData01 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData01,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x01]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn922 : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData02 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData02,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x02]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn923 : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData03 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData03,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x03]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn924 : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData04 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData04,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x04]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn925 : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData05 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData05,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x05]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn926 : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData06 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData06,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x06]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn927 : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData07 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData07,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x07]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn928 : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData08 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData08,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x08]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn929 : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData09 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData09,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x09]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn92A : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData0A = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData0A,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x0A]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn92B : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData0B = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData0B,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x0B]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn92C : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData0C = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData0C,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x0C]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn92D : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData0D = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData0D,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x0D]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn92E : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData0E = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData0E,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x0E]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn92F : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData0F = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData0F,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x0F]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn930 : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData10 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData10,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x10]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn931 : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData11 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData11,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x11]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn932 : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData12 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData12,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x12]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn933 : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData13 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData13,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x13]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn934 : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData14 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData14,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x14]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn935 : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData15 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData15,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x15]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn936 : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData16 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData16,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x16]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn937 : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData17 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData17,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x17]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn938 : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData18 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData18,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x18]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn939 : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData19 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData19,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x19]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn93A : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData1A = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData1A,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x1A]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn93B : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData1B = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData1B,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x1B]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn93C : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData1C = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData1C,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x1C]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn93D : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData1D = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData1D,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x1D]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn93E : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData1E = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData1E,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x1E]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn93F : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData1F = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData1F,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x1F]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn940 : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData20 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData20,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x20]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn941 : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData21 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData21,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x21]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn942 : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData22 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData22,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x22]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn943 : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData23 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData23,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x23]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn944 : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData24 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData24,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x24]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn945 : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData25 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData25,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x25]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn946 : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData26 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData26,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x26]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn947 : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData27 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData27,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x27]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn948 : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData28 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData28,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x28]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn949 : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData29 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData29,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x29]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn94A : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData2A = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData2A,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x2A]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn94B : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData2B = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData2B,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x2B]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn94C : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData2C = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData2C,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x2C]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn94D : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData2D = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData2D,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x2D]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn94E : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData2E = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData2E,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x2E]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn94F : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData2F = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData2F,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x2F]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn950 : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData30 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData30,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x30]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn951 : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData31 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData31,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x31]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn952 : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData32 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData32,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x32]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn953 : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData33 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData33,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x33]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn954 : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData34 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData34,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x34]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn955 : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData35 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData35,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x35]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn956 : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData36 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData36,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x36]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn957 : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData37 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData37,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x37]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn958 : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData38 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData38,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x38]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn959 : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData39 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData39,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x39]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn95A : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData3A = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData3A,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x3A]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn95B : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData3B = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData3B,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x3B]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn95C : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData3C = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData3C,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x3C]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn95D : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData3D = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData3D,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x3D]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn95E : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData3E = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData3E,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x3E]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn95F : バンクデータ																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_bnkData3F = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pcal_bnkData3F,
/* RAMアドレス					*/	&(OpPrm.bnkData[0x3F]),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};



/****************************************************************************************************/
/*		Pn9E0 : パラメータVer.																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_prmVer = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_SYSTEM,
/* EEPROMアドレス				*/	0x0340 | PRMDEF_NODEFTPRM,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.prmVer),
/* 下限値						*/	(USHORT)(0),
/* 上限値						*/	(USHORT)(65535),
};



/****************************************************************************************************/
/*		Pn9E1 : 速度分解能設定																		*/
/****************************************************************************************************/
const	PRMDEF	pndef_spdDivision = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NEEDBOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_SYSTEM,
/* EEPROMアドレス				*/	0x0341 | PRMDEF_NODEFTPRM,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.spdDivision),
/* 下限値						*/	(USHORT)(0),
/* 上限値						*/	(USHORT)(23),
};



/****************************************************************************************************/
/*		Pn9E2 : Command-Option機能SW																*/
/****************************************************************************************************/
const	PRMDEF	pndef_CmdOpSw = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_SYSTEM,
/* EEPROMアドレス				*/	0x0342 | PRMDEF_NODEFTPRM,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.CmdOpSw),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0x1111),
};



/****************************************************************************************************/
/*		Pn9E3 : 																					*/
/****************************************************************************************************/
const	PRMDEF	pndef_sys_rsrv2 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_SYSTEM,
/* EEPROMアドレス				*/	0x0343 | PRMDEF_NODEFTPRM,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.sys_rsrv2),
/* 下限値						*/	(USHORT)(0),
/* 上限値						*/	(USHORT)(65535),
};



/****************************************************************************************************/
/*		Pn9E4 : 																					*/
/****************************************************************************************************/
const	PRMDEF	pndef_sys_rsrv3 = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_RAM_EEPROM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_WRITABLE,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_SYSTEM,
/* EEPROMアドレス				*/	0x0344 | PRMDEF_NODEFTPRM,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.sys_rsrv3),
/* 下限値						*/	(USHORT)(0),
/* 上限値						*/	(USHORT)(65535),
};



/****************************************************************************************************/
/*		PnB00 : 仮想指令オプションパラメータ														*/
/****************************************************************************************************/
const	PRMDEF	pndef_cmdopv = {
/* 属性指定(Bitfield)			*/	{
/* :1: ロングパラメータ			*/	FALSE,
/* :1: ワード順					*/	0,
/* :1: 調整パラメータか？		*/	PRMDEF_NORMAL,
/* :1: 電源再立上げ要/不要		*/	PRMDEF_NON_BOOT,
/* :1: 格納場所					*/	PRMDEF_ONLY_RAM,
/* :2: 表示時の底、				*/	DSPDEF_BASEHEX,
/* :1: 表示時の符号有無			*/	DSPDEF_NOSIGN,
/* :3: 小数点以下桁数			*/	0,
/* :1: 小数点位置変更(LSM)		*/	FALSE,
/* :1: リードオンリー			*/	PRMDEF_READONLY,
/* :1: オペレータ入力調整		*/	FALSE,
/* :X: 予備						*/	0,
/* :8: FormatRev				*/	0x00,
/* :8: 単位						*/	UNIT_NONE },
/* 専用使用CHECK｜OPE表示選択	*/	KPI_CHKSPUSE_COMMON | KPI_LCDOPEDSPSEL_SETUP,
/* アクセスレベル				*/	ACCLVL_USER1,
/* EEPROMアドレス				*/	0x0000,
/* 処理関数ポインタ				*/	pncal_dummy,
/* RAMアドレス					*/	&(OpPrm.cmdopv),
/* 下限値						*/	(USHORT)(0x0000),
/* 上限値						*/	(USHORT)(0xFFFF),
};





/****************************************************************************************************/
/*																									*/
/*		パラメータ管理テーブル																		*/
/*																									*/
/****************************************************************************************************/
#pragma ghs section rodata="PnPrmTblUser"
CPRMTBL	PnPrmTblUser[] = {
/*  Idx			  PrmNo   PrmDef																	*/
/*--------------------------------------------------------------------------------------------------*/
/*    0 */		{ 0x800, &pndef_comCtrl							},
/*    1 */		{ 0x801, &pndef_sLimitSw						},
/*    2 */		{ 0x802, &pndef_cmdCtrlSw						},
/*    3 */		{ 0x803, &pndef_zpWidth							},
/*    4 */		{ 0x804, &pndef_p_sLimitL						},
/*    5 */		{ 0x805, &pndef_p_sLimitH						},
/*    6 */		{ 0x806, &pndef_n_sLimitL						},
/*    7 */		{ 0x807, &pndef_n_sLimitH						},
/*    8 */		{ 0x808, &pndef_absOffsetL						},
/*    9 */		{ 0x809, &pndef_absOffsetH						},
/*   10 */		{ 0x80A, &pndef_accRate1						},
/*   11 */		{ 0x80B, &pndef_accRate2						},
/*   12 */		{ 0x80C, &pndef_accChgSpd						},
/*   13 */		{ 0x80D, &pndef_decRate1						},
/*   14 */		{ 0x80E, &pndef_decRate2						},
/*   15 */		{ 0x80F, &pndef_decChgSpd						},
/*   16 */		{ 0x810, &pndef_filExpBias						},
/*   17 */		{ 0x811, &pndef_filExpTb						},
/*   18 */		{ 0x812, &pndef_filMaTb							},
/*   19 */		{ 0x813, &pndef_resrv2							},
/*   20 */		{ 0x814, &pndef_exDistL							},
/*   21 */		{ 0x815, &pndef_exDistH							},
/*   22 */		{ 0x816, &pndef_zretDir							},
/*   23 */		{ 0x817, &pndef_zretSpd1						},
/*   24 */		{ 0x818, &pndef_zretSpd2						},
/*   25 */		{ 0x819, &pndef_zretDistL						},
/*   26 */		{ 0x81A, &pndef_zretDistH						},
/*   27 */		{ 0x81B, &pndef_resrv3							},
/*   28 */		{ 0x81C, &pndef_resrv4							},
/*   29 */		{ 0x81D, &pndef_resrv5							},
/*   30 */		{ 0x81E, &pndef_ioMonSel						},
/*   31 */		{ 0x81F, &pndef_optBitSw						},
/*   32 */		{ 0x820, &pndef_p_ltLimitL						},
/*   33 */		{ 0x821, &pndef_p_ltLimitH						},
/*   34 */		{ 0x822, &pndef_n_ltLimitL						},
/*   35 */		{ 0x823, &pndef_n_ltLimitH						},
/*   36 */		{ 0x824, &pndef_opMon1							},
/*   37 */		{ 0x825, &pndef_opMon2							},
/*   38 */		{ 0x826, &pndef_decRate1Emg						},
/*   39 */		{ 0x827, &pndef_decRate2Emg						},
/*   40 */		{ 0x828, &pndef_decChgSpdEmg					},
/*   41 */		{ 0x829, &pndef_svOffTime						},
/*   42 */		{ 0x82A, &pndef_optbit1							},
/*   43 */		{ 0x82B, &pndef_optbit2							},
/*   44 */		{ 0x82C, &pndef_optbit3							},
/*   45 */		{ 0x82D, &pndef_optbit4							},
/*   46 */		{ 0x82E, &pndef_optbit5							},
/*   47 */		{ 0x82F, &pndef_resrv6							},
/*   48 */		{ 0x830, &pndef_rotrefL							},
/*   49 */		{ 0x831, &pndef_rotrefH							},
/*   50 */		{ 0x832, &pndef_st_num							},
/*   51 */		{ 0x833, &pndef_rateChgSw						},
/*   52 */		{ 0x834, &pndef_accRate12L						},
/*   53 */		{ 0x835, &pndef_accRate12H						},
/*   54 */		{ 0x836, &pndef_accRate22L						},
/*   55 */		{ 0x837, &pndef_accRate22H						},
/*   56 */		{ 0x838, &pndef_accChgSpd2L						},
/*   57 */		{ 0x839, &pndef_accChgSpd2H						},
/*   58 */		{ 0x83A, &pndef_decRate12L						},
/*   59 */		{ 0x83B, &pndef_decRate12H						},
/*   60 */		{ 0x83C, &pndef_decRate22L						},
/*   61 */		{ 0x83D, &pndef_decRate22H						},
/*   62 */		{ 0x83E, &pndef_decChgSpd2L						},
/*   63 */		{ 0x83F, &pndef_decChgSpd2H						},
/*   64 */		{ 0x840, &pndef_decRateEmg2L					},
/*   65 */		{ 0x841, &pndef_decRateEmg2H					},
/*   66 */		{ 0x842, &pndef_zretSpd12L						},
/*   67 */		{ 0x843, &pndef_zretSpd12H						},
/*   68 */		{ 0x844, &pndef_zretSpd22L						},
/*   69 */		{ 0x845, &pndef_zretSpd22H						},
/*   70 */		{ 0x850, &pndef_ltSeqNum						},
/*   71 */		{ 0x851, &pndef_ltLoopNum						},
/*   72 */		{ 0x852, &pndef_ltSeqSet1						},
/*   73 */		{ 0x853, &pndef_ltSeqSet2						},
/*   74 */		{ 0x880, &pndef_nodeAdr							},
/*   75 */		{ 0x881, &pndef_trByte							},
/*   76 */		{ 0x882, &pndef_comTCycle						},
/*   77 */		{ 0x883, &pndef_comAplCycle						},
/*   78 */		{ 0x88A, &pndef_comErrCnt						},
/*   79 */		{ 0x88B, &pndef_comErrSts1						},
/*   80 */		{ 0x88C, &pndef_comErrSts2						},
/*   81 */		{ 0x88F, &pndef_comDataCtrl						},
/*   82 */		{ 0x890, &pndef_almCmd0							},
/*   83 */		{ 0x891, &pndef_almCmd1							},
/*   84 */		{ 0x892, &pndef_almCmd2							},
/*   85 */		{ 0x893, &pndef_almCmd3							},
/*   86 */		{ 0x894, &pndef_almCmd4							},
/*   87 */		{ 0x895, &pndef_almCmd5							},
/*   88 */		{ 0x896, &pndef_almCmd6							},
/*   89 */		{ 0x897, &pndef_almCmd7							},
/*   90 */		{ 0x898, &pndef_almCmd8							},
/*   91 */		{ 0x899, &pndef_almCmd9							},
/*   92 */		{ 0x89A, &pndef_almCmd10						},
/*   93 */		{ 0x89B, &pndef_almCmd11						},
/*   94 */		{ 0x89C, &pndef_almCmd12						},
/*   95 */		{ 0x89D, &pndef_almCmd13						},
/*   96 */		{ 0x89E, &pndef_almCmd14						},
/*   97 */		{ 0x89F, &pndef_almCmd15						},
/*   98 */		{ 0x8A0, &pndef_almRsp0							},
/*   99 */		{ 0x8A1, &pndef_almRsp1							},
/*  100 */		{ 0x8A2, &pndef_almRsp2							},
/*  101 */		{ 0x8A3, &pndef_almRsp3							},
/*  102 */		{ 0x8A4, &pndef_almRsp4							},
/*  103 */		{ 0x8A5, &pndef_almRsp5							},
/*  104 */		{ 0x8A6, &pndef_almRsp6							},
/*  105 */		{ 0x8A7, &pndef_almRsp7							},
/*  106 */		{ 0x8A8, &pndef_almRsp8							},
/*  107 */		{ 0x8A9, &pndef_almRsp9							},
/*  108 */		{ 0x8AA, &pndef_almRsp10						},
/*  109 */		{ 0x8AB, &pndef_almRsp11						},
/*  110 */		{ 0x8AC, &pndef_almRsp12						},
/*  111 */		{ 0x8AD, &pndef_almRsp13						},
/*  112 */		{ 0x8AE, &pndef_almRsp14						},
/*  113 */		{ 0x8AF, &pndef_almRsp15						},
/*  114 */		{ 0x8B0, &pndef_comCmd0							},
/*  115 */		{ 0x8B1, &pndef_comCmd1							},
/*  116 */		{ 0x8B2, &pndef_comCmd2							},
/*  117 */		{ 0x8B3, &pndef_comCmd3							},
/*  118 */		{ 0x8B4, &pndef_comCmd4							},
/*  119 */		{ 0x8B5, &pndef_comCmd5							},
/*  120 */		{ 0x8B6, &pndef_comCmd6							},
/*  121 */		{ 0x8B7, &pndef_comCmd7							},
/*  122 */		{ 0x8B8, &pndef_comCmd8							},
/*  123 */		{ 0x8B9, &pndef_comCmd9							},
/*  124 */		{ 0x8BA, &pndef_comCmd10						},
/*  125 */		{ 0x8BB, &pndef_comCmd11						},
/*  126 */		{ 0x8BC, &pndef_comCmd12						},
/*  127 */		{ 0x8BD, &pndef_comCmd13						},
/*  128 */		{ 0x8BE, &pndef_comCmd14						},
/*  129 */		{ 0x8BF, &pndef_comCmd15						},
/*  130 */		{ 0x8C0, &pndef_comRsp0							},
/*  131 */		{ 0x8C1, &pndef_comRsp1							},
/*  132 */		{ 0x8C2, &pndef_comRsp2							},
/*  133 */		{ 0x8C3, &pndef_comRsp3							},
/*  134 */		{ 0x8C4, &pndef_comRsp4							},
/*  135 */		{ 0x8C5, &pndef_comRsp5							},
/*  136 */		{ 0x8C6, &pndef_comRsp6							},
/*  137 */		{ 0x8C7, &pndef_comRsp7							},
/*  138 */		{ 0x8C8, &pndef_comRsp8							},
/*  139 */		{ 0x8C9, &pndef_comRsp9							},
/*  140 */		{ 0x8CA, &pndef_comRsp10						},
/*  141 */		{ 0x8CB, &pndef_comRsp11						},
/*  142 */		{ 0x8CC, &pndef_comRsp12						},
/*  143 */		{ 0x8CD, &pndef_comRsp13						},
/*  144 */		{ 0x8CE, &pndef_comRsp14						},
/*  145 */		{ 0x8CF, &pndef_comRsp15						},
/*  146 */		{ 0x8E0, &pndef_OpCardID						},
/*  147 */		{ 0x8E1, &pndef_OpCardCtrlSw					},
/*  148 */		{ 0x8E2, &pndef_OpCardSwVerL					},
/*  149 */		{ 0x8E3, &pndef_OpCardSwVerH					},
/*  150 */		{ 0x8E4, &pndef_OpCardYNo						},
/*  151 */		{ 0x8E5, &pndef_OpCardDate						},
/*  152 */		{ 0x8E6, &pndef_OpCardSvSw						},
/*  153 */		{ 0x8E8, &pndef_OpCardAlmCode					},
/*  154 */		{ 0x8E9, &pndef_OpCardAlmAttr					},
/*  155 */		{ 0x8F0, &pndef_OpCardModel0					},
/*  156 */		{ 0x8F1, &pndef_OpCardModel1					},
/*  157 */		{ 0x8F2, &pndef_OpCardModel2					},
/*  158 */		{ 0x8F3, &pndef_OpCardModel3					},
/*  159 */		{ 0x8F4, &pndef_OpCardModel4					},
/*  160 */		{ 0x8F5, &pndef_OpCardModel5					},
/*  161 */		{ 0x8F6, &pndef_OpCardModel6					},
/*  162 */		{ 0x8F7, &pndef_OpCardModel7					},
/*  163 */		{ 0x8F8, &pndef_OpCardSNo0						},
/*  164 */		{ 0x8F9, &pndef_OpCardSNo1						},
/*  165 */		{ 0x8FA, &pndef_OpCardSNo2						},
/*  166 */		{ 0x8FB, &pndef_OpCardSNo3						},
/*  167 */		{ 0x8FC, &pndef_OpCardSNo4						},
/*  168 */		{ 0x8FD, &pndef_OpCardSNo5						},
/*  169 */		{ 0x8FE, &pndef_OpCardSNo6						},
/*  170 */		{ 0x8FF, &pndef_OpCardSNo7						},
/*  171 */		{ 0x900, &pndef_bnkNum							},
/*  172 */		{ 0x901, &pndef_bnkMemNum						},
/*  173 */		{ 0x902, &pndef_bnkMember0						},
/*  174 */		{ 0x903, &pndef_bnkMember1						},
/*  175 */		{ 0x904, &pndef_bnkMember2						},
/*  176 */		{ 0x905, &pndef_bnkMember3						},
/*  177 */		{ 0x906, &pndef_bnkMember4						},
/*  178 */		{ 0x907, &pndef_bnkMember5						},
/*  179 */		{ 0x908, &pndef_bnkMember6						},
/*  180 */		{ 0x909, &pndef_bnkMember7						},
/*  181 */		{ 0x90A, &pndef_bnkMember8						},
/*  182 */		{ 0x90B, &pndef_bnkMember9						},
/*  183 */		{ 0x90C, &pndef_bnkMemberA						},
/*  184 */		{ 0x90D, &pndef_bnkMemberB						},
/*  185 */		{ 0x90E, &pndef_bnkMemberC						},
/*  186 */		{ 0x90F, &pndef_bnkMemberD						},
/*  187 */		{ 0x910, &pndef_bnkMemberE						},
/*  188 */		{ 0x920, &pndef_bnkData00						},
/*  189 */		{ 0x921, &pndef_bnkData01						},
/*  190 */		{ 0x922, &pndef_bnkData02						},
/*  191 */		{ 0x923, &pndef_bnkData03						},
/*  192 */		{ 0x924, &pndef_bnkData04						},
/*  193 */		{ 0x925, &pndef_bnkData05						},
/*  194 */		{ 0x926, &pndef_bnkData06						},
/*  195 */		{ 0x927, &pndef_bnkData07						},
/*  196 */		{ 0x928, &pndef_bnkData08						},
/*  197 */		{ 0x929, &pndef_bnkData09						},
/*  198 */		{ 0x92A, &pndef_bnkData0A						},
/*  199 */		{ 0x92B, &pndef_bnkData0B						},
/*  200 */		{ 0x92C, &pndef_bnkData0C						},
/*  201 */		{ 0x92D, &pndef_bnkData0D						},
/*  202 */		{ 0x92E, &pndef_bnkData0E						},
/*  203 */		{ 0x92F, &pndef_bnkData0F						},
/*  204 */		{ 0x930, &pndef_bnkData10						},
/*  205 */		{ 0x931, &pndef_bnkData11						},
/*  206 */		{ 0x932, &pndef_bnkData12						},
/*  207 */		{ 0x933, &pndef_bnkData13						},
/*  208 */		{ 0x934, &pndef_bnkData14						},
/*  209 */		{ 0x935, &pndef_bnkData15						},
/*  210 */		{ 0x936, &pndef_bnkData16						},
/*  211 */		{ 0x937, &pndef_bnkData17						},
/*  212 */		{ 0x938, &pndef_bnkData18						},
/*  213 */		{ 0x939, &pndef_bnkData19						},
/*  214 */		{ 0x93A, &pndef_bnkData1A						},
/*  215 */		{ 0x93B, &pndef_bnkData1B						},
/*  216 */		{ 0x93C, &pndef_bnkData1C						},
/*  217 */		{ 0x93D, &pndef_bnkData1D						},
/*  218 */		{ 0x93E, &pndef_bnkData1E						},
/*  219 */		{ 0x93F, &pndef_bnkData1F						},
/*  220 */		{ 0x940, &pndef_bnkData20						},
/*  221 */		{ 0x941, &pndef_bnkData21						},
/*  222 */		{ 0x942, &pndef_bnkData22						},
/*  223 */		{ 0x943, &pndef_bnkData23						},
/*  224 */		{ 0x944, &pndef_bnkData24						},
/*  225 */		{ 0x945, &pndef_bnkData25						},
/*  226 */		{ 0x946, &pndef_bnkData26						},
/*  227 */		{ 0x947, &pndef_bnkData27						},
/*  228 */		{ 0x948, &pndef_bnkData28						},
/*  229 */		{ 0x949, &pndef_bnkData29						},
/*  230 */		{ 0x94A, &pndef_bnkData2A						},
/*  231 */		{ 0x94B, &pndef_bnkData2B						},
/*  232 */		{ 0x94C, &pndef_bnkData2C						},
/*  233 */		{ 0x94D, &pndef_bnkData2D						},
/*  234 */		{ 0x94E, &pndef_bnkData2E						},
/*  235 */		{ 0x94F, &pndef_bnkData2F						},
/*  236 */		{ 0x950, &pndef_bnkData30						},
/*  237 */		{ 0x951, &pndef_bnkData31						},
/*  238 */		{ 0x952, &pndef_bnkData32						},
/*  239 */		{ 0x953, &pndef_bnkData33						},
/*  240 */		{ 0x954, &pndef_bnkData34						},
/*  241 */		{ 0x955, &pndef_bnkData35						},
/*  242 */		{ 0x956, &pndef_bnkData36						},
/*  243 */		{ 0x957, &pndef_bnkData37						},
/*  244 */		{ 0x958, &pndef_bnkData38						},
/*  245 */		{ 0x959, &pndef_bnkData39						},
/*  246 */		{ 0x95A, &pndef_bnkData3A						},
/*  247 */		{ 0x95B, &pndef_bnkData3B						},
/*  248 */		{ 0x95C, &pndef_bnkData3C						},
/*  249 */		{ 0x95D, &pndef_bnkData3D						},
/*  250 */		{ 0x95E, &pndef_bnkData3E						},
/*  251 */		{ 0x95F, &pndef_bnkData3F						},
/*  252 */		{ 0x9E0, &pndef_prmVer							},
/*  253 */		{ 0x9E1, &pndef_spdDivision						},
/*  254 */		{ 0x9E2, &pndef_CmdOpSw							},
/*  255 */		{ 0x9E3, &pndef_sys_rsrv2						},
/*  256 */		{ 0x9E4, &pndef_sys_rsrv3						},
/*  257 */		{ 0xB00, &pndef_cmdopv							},
};
#pragma ghs section rodata=default
/*--------------------------------------------------------------------------------------------------*/
const	USHORT	PnPrmTblUserEntNum = sizeof(PnPrmTblUser)/sizeof(PnPrmTblUser[0]);







/***************************************** end of file **********************************************/
