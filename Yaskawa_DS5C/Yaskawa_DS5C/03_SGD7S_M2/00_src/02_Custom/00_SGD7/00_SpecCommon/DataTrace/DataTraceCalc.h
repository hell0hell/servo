/****************************************************************************
Description	: Function list of the Data Trace
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
T.Kira   2010.11.01     created

****************************************************************************/
#ifndef __DATA_TRACE_CALC_H
#define __DATA_TRACE_CALC_H

#include "Basedef.h"
#include "PrmAccessIf.h"

/* 数値トレースデータ取得関数	*/
LONG	GetVarData_AmonTrqRef( void *Axis );				/* 0x01,0x81: トルク/推力モニタ					*/
LONG	GetVarData_MotSpdSmp( void *Axis );					/* 0x02,0x82: モータ速度						*/
LONG	GetVarData_AmonSpdRef( void *Axis );				/* 0x03,0x83: 速度指令							*/
LONG	GetVarData_dPosCmdSmp( void *Axis );				/* 0x04,0x84: 位置指令速度						*/
LONG	GetVarData_AmonPosErra( void *Axis );				/* 0x05,0x85: 位置偏差							*/
LONG	GetVarData_AmonDisTrq( void *Axis );				/* 0x13	    : 摩擦補償トルク／推力				*/
LONG	GetVarData_MonResVib( void *Axis );					/* 0x14		: 制振差速度						*/
LONG	GetVarData_TrqGravity( void *Axis );				/*  0x15	: TrqGravity						*/	/*<S0CD>*/
LONG	GetVarData_AmonPosErrx( void *Axis );				/* 0x16,0x96: 位置アンプ偏差					*/
LONG	GetVarData_sfperra_per32s( void *Axis );			/* 0x17,0x97: モータ負荷間位置偏差				*/
LONG	GetVarData_SpdFFCx( void *Axis );					/* 0x18,0x98: 速度フィードフォワード			*/
LONG	GetVarData_TrqFFCx( void *Axis );					/* 0x19,0x99: トルク／推力フィードフォワード	*/
LONG	GetVarData_TrefBeforeDtrq( void *Axis );			/* 0x1E		: トルク指令(外乱前)				*/
LONG	GetVarData_TrcCtrlTrqRef( void *Axis );				/* 0x1F		: トルク指令－摩擦補償トルク		*/
LONG	GetVarData_TrqRefi( void *Axis );					/* 0x20		: トルク指令(外乱前・フィルタ前)	*/
LONG	GetVarData_AmonActGain( void *Axis );				/* 0x21     : 有効ゲイン						*/
LONG	GetVarData_dPosRefi( void *Axis );					/* 0x22,0xA2: 位置ループ前位置指令速度			*/	/* <S187> */
LONG	GetVarData_DcVoltx( void *Axis );					/* 0x23		: 主回路母線電圧					*/
LONG	GetVarData_UnCrntDet( void *Axis );					/* 0x24     : 電流検出値						*/
LONG	GetVarData_UnCrntRef( void *Axis );					/* 0x26	    : 累積負荷率						*/
LONG	GetVarData_TrcRgPower( void *Axis );				/* 0x27		: 回生負荷率						*/
LONG	GetVarData_TrcDbPower( void *Axis );				/* 0x28		: DB抵抗消費電力					*/
LONG	GetVarData_FencSpdSmp( void *Axis );				/* 0x29		: 外部エンコーダ速度				*/
LONG	GetVarData_DeltaPos( void *Axis );					/* 0x40		: 位置差分							*/
LONG	GetVarData_dPcmdEgear( void *Axis );				/* 0x41     : 指令差分							*/
LONG	GetVarData_DeltaPos( void *Axis );					/* 0x42		: モータ位置差分					*/
LONG	GetVarData_dFencPos( void *Axis );					/* 0x43		: 負荷位置差分						*/
LONG	GetVarData_MotPhase( void *Axis );					/* 0x44		: 電流位相							*/
LONG	GetVarData_InterSignalX_L( void *Axis );			/* 0x60		: 内部信号モニタ(下位)				*/
LONG	GetVarData_InterSignalX_H( void *Axis );			/* 0x61		: 内部信号モニタ(上位)				*/
LONG	GetVarData_InternalInputSignals_L( void *Axis );	/* 0x62		: 内部入力信号モニタ(下位)			*/
LONG	GetVarData_InternalInputSignals_H( void *Axis );	/* 0x63		: 内部入力信号モニタ(上位)			*/
LONG	GetVarData_InternalOutputSignals_L( void *Axis );	/* 0x64		: 内部出力信号モニタ(下位)			*/
LONG	GetVarData_InternalOutputSignals_H( void *Axis );	/* 0x65		: 内部出力信号モニタ(上位)			*/
LONG	GetVarData_VelRippleTrqRefBeforeComp( void *Axis );	/* 0xAA		: 速度リップル 補償前トルク/推力指令*/	/* <S14B> */
LONG	GetVarData_CurrentPhase( void *Axis );				/* 0xC4		: 電流位相							*/	/* <S14B> */
LONG	GetVarData_TrcDcVoltFili( void *Axis );				/* 0xC5		:主回路母線電圧(フィルタ前)			*/	/* <S199> */
LONG	GetVarData_M3_Apos( void *Axis );					/* 0x100	: M-Ⅲフィードバック位置(APOS)		*/			/* <S164> : 設定値変更 0xD0⇒0x100 */
LONG	GetVarData_M3_Cpos( void *Axis );					/* 0x101	: M-Ⅲ現在指令位置(CPOS)			*/			/* <S164> : 設定値変更 0xD1⇒0x101 */
LONG	GetVarData_M3_Tpos( void *Axis );					/* 0x102	: M-Ⅲ目標位置(TPOS)				*/			/* <S164> : 設定値変更 0xD2⇒0x102 */
LONG	GetVarData_M3_Lpos1( void *Axis );					/* 0x103	: M-Ⅲラッチ位置1(LPOS1)			*/			/* <S164> : 設定値変更 0xD3⇒0x103 */
LONG	GetVarData_M3_Lpos2( void *Axis );					/* 0x104	: M-Ⅲラッチ位置2(LPOS2)			*/			/* <S164> : 設定値変更 0xD4⇒0x104 */
LONG	GetVarData_M3_Fspd( void *Axis );					/* 0x105	: M-Ⅲフィードバック速度(FSPD)		*/			/* <S164> : 設定値変更 0xD5⇒0x105 */
LONG	GetVarData_M3_Cspd( void *Axis );					/* 0x106	: M-Ⅲ現在位置指令速度(CSPD)		*/			/* <S164> : 設定値変更 0xD6⇒0x106 */
LONG	GetVarData_M3_Trq( void *Axis );					/* 0x107	: M-Ⅲトルク/推力指令(TRQ)			*/			/* <S164> : 設定値変更 0xD7⇒0x107 */
LONG	GetVarData_M3_TrqLim( void *Axis );					/* 0x108	: M-Ⅲトルク制限指令(TRQ_LIM)		*/			/* <S164> : 設定値変更 0xD8⇒0x108 */
LONG	GetVarData_M3_VLim( void *Axis );					/* 0x109	: M-Ⅲ速度制限指令(V_LIM)			*/			/* <S164> : 設定値変更 0xD9⇒0x109 */
LONG	GetVarData_M3_M3_Alam( void *Axis );				/* 0x10A	: M-Ⅲ現在発生アラームコード(ALARM)	*/			/* <S164> : 設定値変更 0xDA⇒0x10A */
LONG	GetVarData_M3_RCmd( void *Axis );					/* 0x10B	: M-Ⅲ現在実行コマンドコード(RCMD)	*/			/* <S164> : 設定値変更 0xDB⇒0x10B */
LONG	GetVarData_M3_SubRCmd( void *Axis );				/* 0x10C	: M-Ⅲ現在実行サブコマンドコード(SUB_RCMD)*/	/* <S164> : 設定値変更 0xDC⇒0x10C */
LONG	GetVarData_M3_Cmd_0( void *Axis );					/* 0x110	: M-Ⅲコマンドデータ( 0～ 3byte)	*/			/* <S164> : 設定値変更 0xE0⇒0x110 */
LONG	GetVarData_M3_Cmd_1( void *Axis );					/* 0x111	: M-Ⅲコマンドデータ( 4～ 7byte)	*/			/* <S164> : 設定値変更 0xE1⇒0x111 */
LONG	GetVarData_M3_Cmd_2( void *Axis );					/* 0x112	: M-Ⅲコマンドデータ( 8～11byte)	*/			/* <S164> : 設定値変更 0xE2⇒0x112 */
LONG	GetVarData_M3_Cmd_3( void *Axis );					/* 0x113	: M-Ⅲコマンドデータ(12～15byte)	*/			/* <S164> : 設定値変更 0xE3⇒0x113 */
LONG	GetVarData_M3_Cmd_4( void *Axis );					/* 0x114	: M-Ⅲコマンドデータ(16～19byte)	*/			/* <S164> : 設定値変更 0xE4⇒0x114 */
LONG	GetVarData_M3_Cmd_5( void *Axis );					/* 0x115	: M-Ⅲコマンドデータ(20～23byte)	*/			/* <S164> : 設定値変更 0xE5⇒0x115 */
LONG	GetVarData_M3_Cmd_6( void *Axis );					/* 0x116	: M-Ⅲコマンドデータ(24～27byte)	*/			/* <S164> : 設定値変更 0xE6⇒0x116 */
LONG	GetVarData_M3_Cmd_7( void *Axis );					/* 0x117	: M-Ⅲコマンドデータ(28～31byte)	*/			/* <S164> : 設定値変更 0xE7⇒0x117 */
LONG	GetVarData_M3_Cmd_8( void *Axis );					/* 0x118	: M-Ⅲコマンドデータ(32～35byte)	*/			/* <S164> : 設定値変更 0xE8⇒0x118 */
LONG	GetVarData_M3_Cmd_9( void *Axis );					/* 0x119	: M-Ⅲコマンドデータ(36～39byte)	*/			/* <S164> : 設定値変更 0xE9⇒0x119 */
LONG	GetVarData_M3_Cmd_a( void *Axis );					/* 0x11A	: M-Ⅲコマンドデータ(40～43byte)	*/			/* <S164> : 設定値変更 0xEA⇒0x11A */
LONG	GetVarData_M3_Cmd_b( void *Axis );					/* 0x11B	: M-Ⅲコマンドデータ(44～47byte)	*/			/* <S164> : 設定値変更 0xEB⇒0x11B */
LONG	GetVarData_M3_Rsp_0( void *Axis );					/* 0x120	: M-Ⅲレスポンスデータ( 0～ 3byte)	*/			/* <S164> : 設定値変更 0xF0⇒0x120 */
LONG	GetVarData_M3_Rsp_1( void *Axis );					/* 0x121	: M-Ⅲレスポンスデータ( 4～ 7byte)	*/			/* <S164> : 設定値変更 0xF1⇒0x121 */
LONG	GetVarData_M3_Rsp_2( void *Axis );					/* 0x122	: M-Ⅲレスポンスデータ( 8～11byte)	*/			/* <S164> : 設定値変更 0xF2⇒0x122 */
LONG	GetVarData_M3_Rsp_3( void *Axis );					/* 0x123	: M-Ⅲレスポンスデータ(12～15byte)	*/			/* <S164> : 設定値変更 0xF3⇒0x123 */
LONG	GetVarData_M3_Rsp_4( void *Axis );					/* 0x124	: M-Ⅲレスポンスデータ(16～19byte)	*/			/* <S164> : 設定値変更 0xF4⇒0x124 */
LONG	GetVarData_M3_Rsp_5( void *Axis );					/* 0x125	: M-Ⅲレスポンスデータ(20～23byte)	*/			/* <S164> : 設定値変更 0xF5⇒0x125 */
LONG	GetVarData_M3_Rsp_6( void *Axis );					/* 0x126	: M-Ⅲレスポンスデータ(24～27byte)	*/			/* <S164> : 設定値変更 0xF6⇒0x126 */
LONG	GetVarData_M3_Rsp_7( void *Axis );					/* 0x127	: M-Ⅲレスポンスデータ(28～31byte)	*/			/* <S164> : 設定値変更 0xF7⇒0x127 */
LONG	GetVarData_M3_Rsp_8( void *Axis );					/* 0x128	: M-Ⅲレスポンスデータ(32～35byte)	*/			/* <S164> : 設定値変更 0xF8⇒0x128 */
LONG	GetVarData_M3_Rsp_9( void *Axis );					/* 0x129	: M-Ⅲレスポンスデータ(36～39byte)	*/			/* <S164> : 設定値変更 0xF9⇒0x129 */
LONG	GetVarData_M3_Rsp_a( void *Axis );					/* 0x12A	: M-Ⅲレスポンスデータ(40～43byte)	*/			/* <S164> : 設定値変更 0xFA⇒0x12A */
LONG	GetVarData_M3_Rsp_b( void *Axis );					/* 0x12B	: M-Ⅲレスポンスデータ(44～47byte)	*/			/* <S164> : 設定値変更 0xFB⇒0x12B */
LONG	GetVarData_AdrShortData( void *Axis );				/* 0x7F,0xFF: 実アドレス指定(SHORT)				*/
LONG	GetVarData_AdrLongData( void *Axis );				/* 0x7F,0xFF: 実アドレス指定(LONG)				*/
LONG	GetVarData_Dummy( void *Axis );						/* ****		: ダミー変数						*/
LONG	GetVarData_SqnetAdrData( void *Axis );				/* ****		: 実アドレス指定(SynqNet)			*/
LONG	GetVarData_SqnetAdrData2( void *Axis );			/* ****		: 実アドレス指定(SynqNet)			*/

LONG GetVarData_AmonAdrOutCH0H( void *Axis );				/* ****<S074>: 実アドレス指定(AMON_CH:0 SHORT)  */
LONG GetVarData_AmonAdrOutCH0W( void *Axis );				/* ****<S074>: 実アドレス指定(AMON_CH:0 LONG)   */
LONG GetVarData_AmonAdrOutCH1H( void *Axis );				/* ****<S074>: 実アドレス指定(AMON_CH:1 SHORT)  */
LONG GetVarData_AmonAdrOutCH1W( void *Axis );				/* ****<S074>: 実アドレス指定(AMON_CH:1 LONG)   */


/* 数値トレースデータのゲイン計算関数	*/
LONG	LpxPcalMotSpeedCalGain( void* Axis );				/* 0x02,0x82: モータ速度						*/
LONG	LpxPcalMotSpeedCalGain2( void* Axis );				/* 0x04,0x84: 位置指令速度						*/
LONG	LpxPcalPcmdSpeedCalGain( void* Axis );				/* 0x22		: 位置ループ前指令速度				*/

/* ビットトレースデータ取得関数	*/
BOOL	GetBitData_SvonReq( void *Axis );					/* 0x01	: サーボオン指令(/S-ON)					*/
BOOL	GetBitData_Pcon( void *Axis );						/* 0x02	: P制御指令(/P-CON)						*/
BOOL	GetBitData_Pot( void *Axis );						/* 0x03	: 正転禁止(P-OT)						*/
BOOL	GetBitData_Not( void *Axis );						/* 0x04	: 逆転禁止(N-OT)						*/
BOOL	GetBitData_AlmRst( void *Axis );					/* 0x05	: アラームリセット(/ALMRST)				*/
BOOL	GetBitData_Pcl( void *Axis );						/* 0x06	: 正側トルク制限(/P-CL)					*/
BOOL	GetBitData_Ncl( void *Axis );						/* 0x07	: 負側トルク制限(/N-CL)					*/
/* <S164> Start */
BOOL	GetBitData_SpdD( void *Axis );						/* 0x08	: ＳＰＤ－Ｄ信号(/SPD-D)				*/
BOOL	GetBitData_SpdA( void *Axis );						/* 0x09	: ＳＰＤ－Ａ信号(/SPD-A)				*/
BOOL	GetBitData_SpdB( void *Axis );						/* 0x0A	: ＳＰＤ－Ｂ信号(/SPD-B)				*/
BOOL	GetBitData_Csel( void *Axis );						/* 0x0B	: Ｃ－ＳＥＬ信号(/C-SEL)				*/
BOOL	GetBitData_Zclamp( void *Axis );					/* 0x0C	: ゼロクランプ信号(/ZCLAMP)				*/
BOOL	GetBitData_Inhibit( void *Axis );					/* 0x0D	: パルス指令無視信号(/INHIBIT)			*/
/* <S164> End */
BOOL	GetBitData_Gsel( void *Axis );						/* 0x0E	: G-SEL1信号(/G-SEL1)					*/
BOOL	GetBitData_Pdet( void *Axis );						/* 0x0F : 磁極検出指令(/P-DET)					*/
BOOL	GetBitData_Estp( void *Axis );						/* 0x11	: 非常停止(/E-STP)						*/	/* <S164> */
BOOL	GetBitData_Dec( void *Axis );						/* 0x12 : 減速信号(/DEC)						*/
BOOL	GetBitData_Ext1( void *Axis );						/* 0x13 : EXT1割込み要求(/EXT1)					*/
BOOL	GetBitData_Ext2( void *Axis );						/* 0x14 : EXT2割込み要求(/EXT2)					*/
BOOL	GetBitData_Ext3( void *Axis );						/* 0x15 : EXT3割込み要求(/EXT3)					*/
BOOL	GetBitData_Fstp( void *Axis);						/* 0x16 : FSTP割込み要求(/FSTP)  <S1F5>					*/
BOOL	GetBitData_Sen( void *Axis );						/* 0x19 : SEN信号(SEN)							*/
/* <S164> Start */
BOOL	GetBitData_Puls( void *Axis );						/* 0x1A	: パルス指令入力有無(PULS)				*/
BOOL	GetBitData_Sign( void *Axis );						/* 0x1B	: 符号指令入力有無(SIGN)				*/
BOOL	GetBitData_Clr( void *Axis );						/* 0x1C	: 偏差クリア信号有無(CLR)				*/
BOOL	GetBitData_Psel( void *Axis );						/* 0x1D	: 指令パルス入力倍率切替信号(/PSEL)		*/
/* <S164> End */
BOOL	GetBitData_HWBB1( void *Axis );						/* 0x20	: HWBB1信号(/HWBB1)						*/
BOOL	GetBitData_HWBB2( void *Axis );						/* 0x20	: HWBB2信号(/HWBB2)						*//* <S064> */
BOOL	GetBitData_AlmFlag( void *Axis );					/* 0x80	: アラーム発生(ALM)						*/
BOOL	GetBitData_Coin( void *Axis );						/* 0x81	: 位置決め完了(/COIN)					*/
BOOL	GetBitData_Vcmp( void *Axis );						/* 0x82	: 速度一致(/V-CMP)						*/
BOOL	GetBitData_Tgon( void *Axis );						/* 0x83	: モータ回転検出(/TGON)					*/
BOOL	GetBitData_SvRdy( void *Axis );						/* 0x84	: サーボレディ(/S-RDY)					*/
BOOL	GetBitData_Clt( void *Axis );						/* 0x85 : 電流制限中(CLT)						*/
BOOL	GetBitData_Vlt( void *Axis );						/* 0x86 : 速度制限中(/VLT)						*/
BOOL	GetBitData_Bk( void *Axis );						/* 0x87	: ブレーキ保持(/BK)						*/
BOOL	GetBitData_WrnFlag( void *Axis );					/* 0x88	: ワーニング発生(/WARN)					*/
BOOL	GetBitData_Near( void *Axis );						/* 0x89	: 位置決め近傍(/NEAR)					*/
BOOL	GetBitData_Cplue( void *Axis );						/* 0x8A : C相出力(/C-PLUS)						*/
/* <S164> Start */
BOOL	GetBitData_Alo1( void *Axis );						/* 0x8E	: アラームコード出力１(AL01)			*/
BOOL	GetBitData_Alo2( void *Axis );						/* 0x8F	: アラームコード出力２(AL02)			*/
BOOL	GetBitData_Alo3( void *Axis );						/* 0x90	: アラームコード出力３(AL03)			*/
BOOL	GetBitData_Pao( void *Axis );						/* 0x91	: 分周パルス出力Ａ相(PAO)				*/
BOOL	GetBitData_Pbo( void *Axis );						/* 0x92	: 分周パルス出力Ｂ相(PBO)				*/
BOOL	GetBitData_Pco( void *Axis );						/* 0x93	: 分周パルス出力Ｃ相(PCO)				*/
BOOL	GetBitData_Psela( void *Axis );						/* 0x94	: 指令パルス入力倍率切替応答(/PSELA)	*/
/* <S164> End */
BOOL	GetBitData_Acon( void *Axis );						/* 0xC0	: 主回路電源オン(ACON)					*/
BOOL	GetBitData_Refz( void *Axis );						/* 0xC1	: 指令払い出しゼロ(REFZ)				*/
BOOL	GetBitData_Pdetcmp( void *Axis );					/* 0xC2 : 磁極検出完了(PDETCMP)					*/
BOOL	GetBitData_Den( void *Axis );						/* 0xC8	: 指令払い出し完了(DEN)					*/
BOOL	GetBitData_Pset( void *Axis );						/* 0xC9	: 位置決め完了(PSET)					*/
BOOL	GetBitData_M3CmdRdy( void *Axis );					/* 0xCA : M-Ⅲコマンドレディ(CMDRDY)			*/
//BOOL	GetBitData_Dummy( void *Axis );						/* ****	: ダミー変数							*/

#endif //__DATA_TRACE_CALC_H

