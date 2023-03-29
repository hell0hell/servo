/****************************************************************************************************/
/*																									*/
/*																									*/
/*		MprgSgdv.inc : ＳＧＤＶマイクロプログラム													*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.1.26 : 2006.04.06  M.Hisatsune	・L(di/dt)フィルタレジスタ追加							*/
/*																									*/
/*		Rev.3.01 : 2007.01.19  S.Ohta		・電流オブザーバ処理追加	<V038>	<V076>				*/
/*		Rev.3.02 : 2007.02.04  J.Watanabe	・電流位相補間処理追加	<V112>							*/
/*		Rev.3.03 : 2007.02.24  J.Watanabe	・弱め界磁制御機能		<V214>							*/
/*											・不具合修正、コメント整理								*/
/*		Rev.3.04 : 2007.02.28  S.Ohta		・q軸電流モニタ，外乱入力追加					<V224>	*/
/*		Rev.3.07 : 2007.03.23  S.Ohta		・ノッチフィルタ，2次フィルタ計算方法変更 		<V388>	*/
/*		Rev.3.09 : 2007.05.24  S.Ohta		・ノッチフィルタ不具合修正						<V502>	*/
/*		Rev.3.10 : 2007.10.04  S.Ohta		・弱め界磁制御処理変更(新弱め界磁追加)			<V531>	*/
/*		Rev.3.12 : 2007.11.28  S.Ohta		・ファイル名のタイトルを変更							*/
/*												MprgSgds.inc	-> MprgSgdv.inc						*/
/*												ＳＧＤＳ		-> ＳＧＤＶ							*/
/*		Rev.3.13 : 2009.12.17  G.Yamazaki	・マイクロ演算部による分周パルス出力機能追加	<V720>	*/
/*		Rev.3.15 : 2011.09.17  Y.Takei		 additional NotchFilter4,5,6 					<V894>	*/
/*																									*/
/****************************************************************************************************/
#ifndef _MPRGSGDV_H_

#include "Basedef.h"




/****************************************************************************************************/
/*																									*/
/*		マイクロ演算部ソフトバージョン定義													<V720>	*/
/*																									*/
/****************************************************************************************************/
SHORT	MSW_VER;				/* ADR:0x0003: ソフトバージョン(製品用)								*/
SHORT	TST_VER;				/* ADR:0x0000: テストバージョン(評価用)								*/
SHORT	YSP_VER;				/* ADR:0x0000: Ｙ仕様バージョン(製品用)								*/

/****************************************************************************************************/
/*																									*/
/*		H/W Register																				*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		READ register																				*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	IuAD;					/* ADR:0x07d0: AD data U											*/
SHORT	IvAD;					/* ADR:0x07d1: AD data V											*/
//   	--;						/* ADR:0x07d2: 														*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	IFLAG;					/* ADR:0x07d3: Flag status											*/
								//  bit.0 : Z(zero)
								//  bit.1 : S(sign'-')
								//  bit.2 : C(carry)
								//  bit.3 : B(bollow)
								//  bit.4 : LB(limit)
								//  bit.5 : ZD(zero div)
								//  bit.6 : DOVF(over-flow div)
								//  bit.8 : ILS0(interrupt0)
								//  bit.9 : ILS1(interrupt1)
								//  bit.A : ILS2(interrupt2)
/*--------------------------------------------------------------------------------------------------*/
DWREG	MacL;					/* ADR:0x07d4: MPLY 0-15 bit										*/
DWREG	MacH;					/* ADR:0x07d6: MPLY 32-47 bit										*/
SHORT	DivRem;					/* ADR:0x07d8: Divide modulo										*/
SHORT	HSUR0;					/* ADR:0x07d9: ホスト指令ポート０									*/
SHORT	HSUR1;					/* ADR:0x07da: ホスト指令ポート１									*/
SHORT	HSUR2;					/* ADR:0x07db: ホスト指令ポート２									*/
SHORT	HSUR3;					/* ADR:0x07dc: ホスト指令ポート３									*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	CTSTR;					/* ADR:0x07dd: controll status										*/
								//  bit.0 : '1'固定
								//  bit.1 : CTRD1
								//  bit.2 : CTRD2
								//  bit.3 : CTRD3(RLOCK)
								//  bit.4 : CTRD4(BB)
								//  bit.5 : CTRD5
								//  bit.6 : CTRD6
								//  bit.7 : CTRD7
/*--------------------------------------------------------------------------------------------------*/
SHORT	INT1SET;				/* ADR:0x07de: INT1 status read clr									*/
								//  bit.0 : EADINTAC
								//  bit.1 : MPINTAC
								//  bit.2 : CNVSTAC
								//  bit.3 : INT0AC
								//  bit.4 : CMDINTAC
								//  bit.5 : SPG0INTAC
								//  bit.6 : SPG1INTAC
								//  bit.7 : URTINTAC
/*--------------------------------------------------------------------------------------------------*/
SHORT	FLTIN;					/* ADR:0x07df: fault status(FLTSTAT)								*/
								//  bit.0 : HBLK0(Sync-Serial bit0)
								//  bit.1 : HBLK1(Sync-Serial bit1)
								//  bit.2 : HBLK2(Sync-Serial bit2)
								//  bit.3 : '0'固定
								//  bit.4 : OVDAT(OV)
								//  bit.5 : OCDAT(OC)
								//  bit.6 : OVLG(Degital filter fault:0)
								//  bit.7 : DBON1LT
								//  bit.8 : DBON2LT
								//  bit.A : BB(Base-Block)
								//  bit.B : GUP2(Give-up host:0)
								//  bit.C : OVLGCH1(fault:0)
								//  bit.D : OVLGCH2(fault:0)
								//  bit.E : OVLGCH3(fault:0)
								//  bit.F : CRUP(carrier counter status:up=1)
/*--------------------------------------------------------------------------------------------------*/
SHORT	FBCSET;					/* ADR:0x07e0: counter status										*/
								//  bit.0:PZD(test)
								//  bit.1 : DADAT(division out A)
								//  bit.2 : DBDAT(division out B)
								//  bit.3 : DCDAT(division out C)
								//  bit.4 : PADAT(PA)
								//  bit.5 : PBDAT(PB)
								//  bit.6 : PCDAT(PC)
								//  bit.7 : ZRE
								//  bit.8 : CADAT(CA input)
								//  bit.9 : CBDAT(CB input)
								//  bit.A : CCDAT(CC input)
								//  bit.B : CLE(CLM latch end)
								//  bit.C : PUI(U)
								//  bit.D : PVI(V)
								//  bit.E : PWI(W)
								//  bit.F : UE
/*--------------------------------------------------------------------------------------------------*/
SHORT	FCCST;					/* ADR:0x07e1: SDM status											*/
								//  bit.0 : SDM1TER(decimation1 over)
								//  bit.1 : SDM2TER(decimation2 over)
								//  bit.2 : SDM3TER(decimation3 over)
								//  bit.3 : SDM1TER(decimation1 mclk miss)
								//  bit.4 : SDM2TER(decimation2 mclk miss)
								//  bit.5 : SDM3TER(decimation3 mclk miss)
								//  bit.7 : Encoder AD Latch Complete flag)
								//  bit.8 : RXFLG0(Serial-Enc0 receive flag)
								//  bit.9 : RXFLG1(Serial-Enc1 receive flag)
/*--------------------------------------------------------------------------------------------------*/
SHORT	CRUDP;					/* ADR:0x07e2: Y-Tcounter 											*/
								//  bit.0-2 : for generic A/D
								//  bit.3   : '1'固定
								//  bit.4-6 : for decimation1
								//  bit.8-A : for decimation2
/*--------------------------------------------------------------------------------------------------*/
//   	--;						/* ADR:0x07e3: 														*/
SHORT	OPCCLM;					/* ADR:0x07e4: OP counter CLM latch									*/
//   	--;						/* ADR:0x07e5: 														*/
//   	--;						/* ADR:0x07e6: 														*/
SHORT	OPCLT;					/* ADR:0x07e7: OP counter load latch								*/
//   	--;						/* ADR:0x07e8: 														*/
//   	--;						/* ADR:0x07e9: 														*/
SHORT	SRPG1RD2;				/* ADR:0x07ea: Serial-PG1 receive word2								*/
SHORT	SRPG1RD3;				/* ADR:0x07eb: Serial-PG1 receive word3								*/
SHORT	SRPG1RD4;				/* ADR:0x07ec: Serial-PG1 receive word4								*/
SHORT	SRPG1RD5;				/* ADR:0x07ed: Serial-PG1 receive word5								*/
SHORT	SRPG1RD6;				/* ADR:0x07ee: Serial-PG1 receive word6								*/
SHORT	SRPG1RD7;				/* ADR:0x07ef: Serial-PG1 receive word7								*/
/*--------------------------------------------------------------------------------------------------*/
DWREG	SftMacL;				/* ADR:0x07f0: BRL shifter 0-15 bit									*/
DWREG	SftMacH;				/* ADR:0x07f2: BRL shifter 32-47 bit								*/
SHORT	CAD0D;					/* ADR:0x00f4: Operation AD0 data									*/
SHORT	CAD1D;					/* ADR:0x00f5: Operation AD1 data Low								*/
SHORT	CAD1H;					/* ADR:0x00f6: Operation AD1 data high								*/
//   	--;						/* ADR:0x00f7: 														*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	ILR;					/* ADR:0x07f8: interrupt level read									*/
								//  bit.0-2 : interrupt0
								//  bit.4-6 : interrupt1
								//  bit.8-A : interrupt2
								//  bit.C-E : interrupt3
/*--------------------------------------------------------------------------------------------------*/
//   	--;						/* ADR:0x07f9: 														*/
SHORT	SRPG0RD2;				/* ADR:0x07fa: serial PG0 Rx word 2									*/
SHORT	SRPG0RD3;				/* ADR:0x07fb: serial PG0 Rx word 3									*/
SHORT	SRPG0RD4;				/* ADR:0x07fc: serial PG0 Rx word 4									*/
SHORT	SRPG0RD5;				/* ADR:0x07fd: serial PG0 Rx word 5									*/
SHORT	SRPG0RD6;				/* ADR:0x07fe: serial PG0 Rx word 6									*/
SHORT	SRPG0RD7;				/* ADR:0x07ff: serial PG0 Rx word 7									*/
//;
/*--------------------------------------------------------------------------------------------------*/
/*		WRITE register																				*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	UPDO;					/* ADR:0x07c0: 拡張IO for debug											<V720>	*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	OUTPT;					/* ADR:0x07d0: INT2 port											*/
SHORT	WDPT;					/* ADR:0x07d1: WDT trigger port(WDT1L)								*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	BBSET;					/* ADR:0x07d2: soft BB set											*/
								//  bit.2 : soft BB
								//  bit.8 : EADINTEN(エンコーダAD変換完了割込みイネーブル)
								//  bit.9 : MPINTEN(M-Port割込みイネーブル)
								//  bit.A : CNVSTEN(enable CONVSTL int)
								//  bit.B : INT0EN
								//  bit.C : CMDINTEN
								//  bit.D : SPG0INTEN(シリアルエンコーダ0／C相エッジ割込みイネーブル)
								//  bit.E : SPG1INTEN(シリアルエンコーダ1割込みイネーブル)
								//  bit.F : UTINTEN(非同期シリアル送受信完割込みイネーブル)
/*--------------------------------------------------------------------------------------------------*/
SHORT	CRST;					/* ADR:0x07d3: Carrer start											*/
								//  bit.0 : CRSTL(PWMキャリア開始指令)
								//  bit.1 : CC40M(キャリアカウンタクロック設定　0:80MHz　1:40MHz)
SHORT	SOD0L;					/* ADR:0x07d4: Sync-Serial0 Low										*/
SHORT	SOD0H;					/* ADR:0x07d5: Sync-Serial0 High									*/
SHORT	SOD1L;					/* ADR:0x07d6: Sync-Serial1 Low										*/
SHORT	SOD1H;					/* ADR:0x07d7: Sync-Serial1 High									*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	SDMECLR;				/* ADR:0x07d8: SDM status clr										*/
								//  bit.0 : Decimation1 error clear
								//  bit.1 : Decimation2 error clear
								//  bit.2 : Decimation3 error clear
								//  bit.3 : Decimation1-3 MCLK error clear
/*--------------------------------------------------------------------------------------------------*/
SHORT	ADSYNC;					/* ADR:0x07d9: AD sync												*/
//   	--;						/* ADR:0x07da: 														*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	PWMOS;					/* ADR:0x07db: PWM out sellect										*/
								//  bit.0 : PWMOSEL(0:Triangle,1:Space-Vec)
								//  bit.3 : LV2L(0:2level,1:3level)
								//  bit.4 : MBCMPH(0:Normal,1:Nx<--X2)
								//  bit.5 : LDTYPE(0:Carrier,1:T0(servo))
								//  bit.6 : FLDTYPE(0:Bottom,1:Top&Bottom)
								//  bit.7 : SAWL(0:SAW type,1:Normal)
								//  bit.8 : SAWDWL(0:Down,1:Up)
/*--------------------------------------------------------------------------------------------------*/
SHORT	CRSET1;					/* ADR:0x07dc: Carrer setting										*/
								//  bit.0-2 : T&B counter clear set
/*--------------------------------------------------------------------------------------------------*/
SHORT	CTSTW;					/* ADR:0x07dd: controll status										*/
								//  bit.0 : STD0
								//  bit.1 : STD1
								//  bit.2 : STD2
								//  bit.3 : STD3(D/QLIM)
								//  bit.4 : STD4
								//  bit.5 : STD5(TLIM)
								//  bit.6 : STD6
								//  bit.7 : STD7
/*--------------------------------------------------------------------------------------------------*/
SHORT	ADT0;					/* ADR:0x07de: carrer 1/3 point set									*/
SHORT	CRFRQ;					/* ADR:0x07df: Carrer frequency										*/
SHORT	PWMP0;					/* ADR:0x07e0: PWM pattern 1 0										*/
SHORT	PWMP2;					/* ADR:0x07e1: PWM pattern 3 2										*/
SHORT	PWMP4;					/* ADR:0x07e2: PWM pattern 5 4										*/
SHORT	PWMP6;					/* ADR:0x07e3: PWM pattern 7 6										*/
SHORT	PWMP8;					/* ADR:0x07e4: PWM pattern 9 8										*/
SHORT	PWMP10;					/* ADR:0x07e5: PWM pattern 11 10									*/
SHORT	PWMP12;					/* ADR:0x07e6: PWM pattern 12										*/
SHORT	PWMT0;					/* ADR:0x07e7: PWM timer 0											*/
SHORT	PWMT1;					/* ADR:0x07e8: PWM timer 1											*/
SHORT	PWMT2;					/* ADR:0x07e9: PWM timer 2											*/
SHORT	PWMT3;					/* ADR:0x07ea: PWM timer 3											*/
SHORT	PWMT4;					/* ADR:0x07eb: PWM timer 4											*/
SHORT	PWMT5;					/* ADR:0x07ec: PWM timer 5											*/
SHORT	PWMT6;					/* ADR:0x07ed: PWM timer 6											*/
SHORT	PWMT7;					/* ADR:0x07ee: PWM timer 7											*/
SHORT	PWMT8;					/* ADR:0x07ef: PWM timer 8											*/
SHORT	PWMT9;					/* ADR:0x07f0: PWM timer 9											*/
SHORT	PWMT10;					/* ADR:0x07f1: PWM timer 10											*/
SHORT	PWMT11;					/* ADR:0x07f2: PWM timer 11											*/
//   	--;						/* ADR:0x07f3: 														*/
SHORT	ISA0;					/* ADR:0x07f4: interrupt 0 address									*/
SHORT	ISA1;					/* ADR:0x07f5: interrupt 1 address									*/
SHORT	ISA2;					/* ADR:0x07f6: interrupt 2 address									*/
SHORT	ISA3;					/* ADR:0x07f7: interrupt 3 address									*/
SHORT	ILW;					/* ADR:0x07f8: interrupt level write								*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	DIVSET;					/* ADR:0x07f9: counter divide set									*/
								// bit.0   : PCMASK
								// bit.A-B : division C out select
								//		   (00:normal,01:div
								//		    10:normal,11:div(00|01))
/*--------------------------------------------------------------------------------------------------*/
SHORT	PCVS0;					/* ADR:0x07fa: PWM-pulse convert set0								*/
SHORT	PCVS1;					/* ADR:0x07fb: PWM-pulse convert set1								*/
SHORT	PCVS2;					/* ADR:0x07fc: PWM-pulse convert set2								*/
SHORT	EIX;					/* ADR:0x07fd: interrupt enable										*/
SHORT	DIX;					/* ADR:0x07fe: interrupt disable									*/
//   	-;						/* ADR:0x07ff: 														*/
/*--------------------------------------------------------------------------------------------------*/
/*		Flags																						*/
/*--------------------------------------------------------------------------------------------------*/



typedef struct {
/****************************************************************************************************/
/*																									*/
/*		Work Register : WREGBASE address = 08007000h												*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		32 bit registers																			*/
/*--------------------------------------------------------------------------------------------------*/
DWREG	ZEROR;					/* ADR:0x0000: (WREGBASE+000) ; Zero register(Low)					*/
DWREG	IDIL;					/* ADR:0x0002: (WREGBASE+004) ; ACRd Integral(Low)					*/
DWREG	IQIL;					/* ADR:0x0004: (WREGBASE+008) ; ACRq Integral(Low)					*/
DWREG	VDFL;					/* ADR:0x0006: (WREGBASE+00C) ; ACRd Filter Output(Low)				*/
DWREG	VQFL;					/* ADR:0x0008: (WREGBASE+010) ; ACRq Filter Output(Low)				*/
SHORT	PHAL;					/* ADR:0x000a: (WREGBASE+014) ; Phase(Low)							*/
SHORT	PHA;					/* ADR:0x000b: (WREGBASE+016) ; Phase(High)							*/
/*--------------------------------------------------------------------------------------------------*/
/*		16 bit registers																			*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	BBSETW;					/* ADR:0x000c: (WREGBASE+018) ; soft BB set(BBSET)					*/
SHORT	FLTINW;					/* ADR:0x000d: (WREGBASE+01A) ; fault status(FLTIN)					*/
SHORT	CTSTRW;					/* ADR:0x000e: (WREGBASE+01C) ; controll flag/status(CTSTR,CTSTW)	*/
SHORT	IUOF;					/* ADR:0x000f: (WREGBASE+01E) ; A/D transfer offset for iu			*/
SHORT	IVOF;					/* ADR:0x0010: (WREGBASE+020) ; A/D transfer offset for iv			*/
SHORT	KCU;					/* ADR:0x0011: (WREGBASE+022) ; A/D transfer gain for iu			*/
SHORT	KCV;					/* ADR:0x0012: (WREGBASE+024) ; A/D transfer gain for iv			*/
SHORT	IDO;					/* ADR:0x0013: (WREGBASE+026) ; Id reference						*/
SHORT	IQO;					/* ADR:0x0014: (WREGBASE+028) ; Iq reference						*/
SHORT	VEL;					/* ADR:0x0015: (WREGBASE+02A) ; Velocity (omega)					*/
SHORT	CNTSTS;					/* ADR:0x0016: (WREGBASE+02C) ; counter status(FBCSTS)				*/
SHORT	FCCSTMON;				/* ADR:0x0017: (WREGBASE+02E) ; full closed counter status(FCCST)	*/
/*--------------------------------------------------------------------------------------------------*/
/*		counter																						*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	NCNT;					/* ADR:0x0018: (WREGBASE+030) ; FB count							*/
SHORT	DNCNT;					/* ADR:0x0019: (WREGBASE+032) ; dNCNT								*/
/*--------------------------------------------------------------------------------------------------*/
/*		voltage FF																					*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	LDC;					/* ADR:0x001a: (WREGBASE+034) ; Ld  * omega							*/
SHORT	LQC;					/* ADR:0x001b: (WREGBASE+036) ; Lq  * omega							*/
SHORT	MAGC;					/* ADR:0x001c: (WREGBASE+038) ; Phi * omega							*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	SINT;					/* ADR:0x001d: (WREGBASE+03A) ; sin theta							*/
SHORT	COST;					/* ADR:0x001e: (WREGBASE+03C) ; cos theta							*/
SHORT	SINT2;					/* ADR:0x001f: (WREGBASE+03E) ; sin(theta + 2pi/3)					*/
SHORT	COST2;					/* ADR:0x0020: (WREGBASE+040) ; cos(theta + 2pi/3)					*/
SHORT	SINT3;					/* ADR:0x0021: (WREGBASE+042) ; sin(theta - 2pi/3)					*/
SHORT	COST3;					/* ADR:0x0022: (WREGBASE+044) ; cos(theta - 2pi/3)					*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	IUD;					/* ADR:0x0023: (WREGBASE+046) ; iu input data (from A/D)			*/
SHORT	IVD;					/* ADR:0x0024: (WREGBASE+048) ; iv input data (from A/D)			*/
SHORT	IDD;					/* ADR:0x0025: (WREGBASE+04A) ; Id Input							*/
SHORT	IQD;					/* ADR:0x0026: (WREGBASE+04C) ; Iq Input							*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	VD;						/* ADR:0x0027: (WREGBASE+04E) ; Vd output voltage					*/
SHORT	VQ;						/* ADR:0x0028: (WREGBASE+050) ; Vq output voltage					*/
SHORT	VDC;					/* ADR:0x0029: (WREGBASE+052) ; Vd compensation data				*/
SHORT	VQC;					/* ADR:0x002a: (WREGBASE+054) ; Vq compensation data				*/
SHORT	VU;						/* ADR:0x002b: (WREGBASE+056) ; vu output data (to PWM)				*/
SHORT	VV;						/* ADR:0x002c: (WREGBASE+058) ; vv output data (to PWM)				*/
SHORT	VW;						/* ADR:0x002d: (WREGBASE+05A) ; vw output data (to PWM)				*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	Dummy1;					/* ADR:0x002e: (WREGBASE+05C) ;										*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	CRFQW;					/* ADR:0x002f: (WREGBASE+05E) ; Carrier freq Now					*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	IUO;					/* ADR:0x0030: (WREGBASE+060) ; Iu reference(for dead-time cmp)		*/
SHORT	IVO;					/* ADR:0x0031: (WREGBASE+062) ; Iv reference(for dead-time cmp)		*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	V1;						/* ADR:0x0032: (WREGBASE+064) ; V1( = SQRT(VD^2+VQ^2) )				*/
SHORT	VC;						/* ADR:0x0033: (WREGBASE+066) ; Central voltage						*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	VDREF;					/* ADR:0x0034: (WREGBASE+068) ; vdref								*/
SHORT	VQREF;					/* ADR:0x0035: (WREGBASE+06A) ; vqref								*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	Dummy2[10];				/* 0x706C - 0x707E */

/*--------------------------------------------------------------------------------------------------*/
/*		for notch filter output																		*/
/*--------------------------------------------------------------------------------------------------*/
//   	IQO1L;					/* ADR:0x0036: (WREGBASE+06C) ; Filter1 output(Low)			; <V388> 削除	*/
//   	IQO1;					/* ADR:0x0037: (WREGBASE+06E) ; Filter1 output(High)			; <V388> 削除	*/
//   	IQO2L;					/* ADR:0x0038: (WREGBASE+070) ; Filter2 output(Low)			; <V388> 削除	*/
//   	IQO2;					/* ADR:0x0039: (WREGBASE+072) ; Filter2 output(High)			; <V388> 削除	*/
/*--------------------------------------------------------------------------------------------------*/
/*		for notch filter1/2 work(input previous) 													*/
/*--------------------------------------------------------------------------------------------------*/
//   	IQIP;					/* ADR:0x003a: (WREGBASE+074) ; previous IQIN				; <V388> 削除	*/
//   	IQIPP;					/* ADR:0x003b: (WREGBASE+076) ; previous IQIP				; <V388> 削除	*/
//   	IQI2P;					/* ADR:0x003c: (WREGBASE+078) ; previous IQO1				; <V388> 削除	*/
//   	IQI2PP;					/* ADR:0x003d: (WREGBASE+07A) ; previous IQI2P				; <V388> 削除	*/
/*--------------------------------------------------------------------------------------------------*/
/*		for notch filter1 work(output previous) 													*/
/*--------------------------------------------------------------------------------------------------*/
//   	IQO1P;					/* ADR:0x003e: (WREGBASE+07C) ; Filter1 output previous(Low)	; <V388> 削除	*/
//   	IQO1PH;					/* ADR:0x003f: (WREGBASE+07E) ; Filter1 output previous(High); <V388> 削除	*/
/*--------------------------------------------------------------------------------------------------*/
/*		for LPF																						*/
/*--------------------------------------------------------------------------------------------------*/
DWREG	IQO1FL;					/* ADR:0x0040: (WREGBASE+080) ; LPF1 Output(Low)					*/
DWREG	IQOFL;					/* ADR:0x0042: (WREGBASE+084) ; LPF2 Output(Low)					*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	IQREF;					/* ADR:0x0044: (WREGBASE+088) ; Iq Reference after limit(5/23)		*/
SHORT	TLMTP;					/* ADR:0x0045: (WREGBASE+08A) ; +tlim 5/23							*/
SHORT	TLMTM;					/* ADR:0x0046: (WREGBASE+08C) ; -tlim 5/23							*/
/*--------------------------------------------------------------------------------------------------*/
/*		for monitor																					*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	IQMON;					/* ADR:0x0047: (WREGBASE+08E) ; IQ monitor							*/
/*--------------------------------------------------------------------------------------------------*/
/*		for notch filter2 work(output previous) 													*/
/*--------------------------------------------------------------------------------------------------*/
DWREG	IQOP;					/* ADR:0x0048: (WREGBASE+090) ; Filter2 output previous(Low)		*/
/*--------------------------------------------------------------------------------------------------*/
/*		for over modulation																			*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	KM;						/* ADR:0x004a: (WREGBASE+094) ; Over modulation gain/offset			*/
SHORT	KVV;					/* ADR:0x004b: (WREGBASE+096) ; AVR									*/
/*--------------------------------------------------------------------------------------------------*/
/*		A/D Stop Error detect																		*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	ADRST;					/* ADR:0x004c: (WREGBASE+098) ;										*/
/*--------------------------------------------------------------------------------------------------*/
/*		A/D Stop Error detect																		*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	ADERRCNT;				/* ADR:0x004d: (WREGBASE+09A) ; ASIC AD Error Counter				*/
SHORT	IRIUP;					/* ADR:0x004e: (WREGBASE+09C) ; Previous IRIU						*/
SHORT	IRIUCNT;				/* ADR:0x004f: (WREGBASE+09E) ; same IRIU counter					*/
SHORT	IRIVP;					/* ADR:0x0050: (WREGBASE+0A0) ; Previous IRIV						*/
SHORT	IRIVCNT;				/* ADR:0x0051: (WREGBASE+0A2) ; same IRIV counter					*/
/*--------------------------------------------------------------------------------------------------*/
/*		reserved																					*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	WREG82;					/* ADR:0x0052: Work  Register 82									*/
SHORT	WREG83;					/* ADR:0x0053: Work  Register 83									*/
SHORT	WREG84;					/* ADR:0x0054: Work  Register 84									*/
SHORT	WREG85;					/* ADR:0x0055: Work  Register 85									*/
SHORT	WREG86;					/* ADR:0x0056: Work  Register 86									*/
SHORT	WREG87;					/* ADR:0x0057: Work  Register 87									*/
SHORT	WREG88;					/* ADR:0x0058: Work  Register 88									*/
SHORT	WREG89;					/* ADR:0x0059: Work  Register 89									*/
/*--------------------------------------------------------------------------------------------------*/
/*		deferential																					*/
/*--------------------------------------------------------------------------------------------------*/
DWREG	WREG90;					/* ADR:0x005a: Work  Register 90									*/
#define VDDFL		WREG90.l	/* 																	*/
#define IDDP		WREG90.s[0]	/* 																	*/
#define VDDFH		WREG90.s[1]	/* 																	*/
DWREG	WREG92;					/* ADR:0x005c: Work  Register 92									*/
#define VQDFL		WREG92.l	/* 																	*/
#define IQDP		WREG92.s[0]	/* 																	*/
#define VQDFH		WREG92.s[1]	/* 																	*/
SHORT	KEANGLE;				/* ADR:0x005e: (0BC)	; 機械角→電気角変換係数<V013>				*/
SHORT	WREG95;					/* ADR:0x005f: Work  Register 95									*/
DWREG	IDFL;					/* ADR:0x0060: L(di/dt)フィルタ										*/
DWREG	IQFL;					/* ADR:0x0062: 														*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	WREG100;				/* ADR:0x0064: (WREGBASE+0C8);Work  Register 100					*/
SHORT	WREG101;				/* ADR:0x0065: Work  Register 101									*/
SHORT	WREG102;				/* ADR:0x0066: Work  Register 102									*/
SHORT	WREG103;				/* ADR:0x0067: Work  Register 103									*/
SHORT	WREG104;				/* ADR:0x0068: Work  Register 104									*/
SHORT	KF31;					/* ADR:0x0069: (WREGBASE+0D2)										*/
SHORT	KF32;					/* ADR:0x006a: (WREGBASE+0D4)										*/
SHORT	KF33;					/* ADR:0x006b: (WREGBASE+0D6)										*/
SHORT	KF34;					/* ADR:0x006c: (WREGBASE+0D8)										*/
SHORT	WREG109;				/* ADR:0x006d: Work  Register 109	← 使ってない					*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	Dummy3[6];				/* 0x70DC - 0x70E6 */
/*--------------------------------------------------------------------------------------------------*/
SHORT	VMAX2;					/* ADR:0x0074: (WREGBASE+0E8)	; 電圧指令ベクトル最大値(8192*1.27)	<V531>	*/
SHORT	V12;					/* ADR:0x0075: (WREGBASE+0EA)	; 電圧指令ベクトル(√(VD^2+VQ^2))	<V531>	*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	Dummy4[10];				/* 0x70EC - 0x70FE */
/*--------------------------------------------------------------------------------------------------*/
SHORT	FLG;					/* ADR:0x0080: (WREGBASE+100)	; 積分飽和ﾌﾗｸﾞ						*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	Dummy5;					/* 0x7102 ; */
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		for INT_HOST																				*/
/*--------------------------------------------------------------------------------------------------*/
DWREG	HTMP0;					/* ADR:0x0082: (WREGBASE+104)	; HOST Interrupt Temp0(130)			*/
DWREG	HTMP2;					/* ADR:0x0084: HOST Interrupt Temp2(132)							*/
DWREG	HTMP4;					/* ADR:0x0086: HOST Interrupt Temp4(134)							*/
SHORT	HTMP6;					/* ADR:0x0088: HOST Interrupt Temp6(136)							*/
SHORT	HTMP7;					/* ADR:0x0089: HOST Interrupt Temp7(137)							*/
DWREG	HTMP8;					/* ADR:0x008a: HOST Interrupt Temp8(138)							*/
/*--------------------------------------------------------------------------------------------------*/
/*		for ROUND																					*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	WREG140;				/* ADR:0x008c: (WREGBASE+118)	; Work  Register 140				*/
SHORT	WREG141;				/* ADR:0x008d: Work  Register 141									*/
SHORT	WREG142;				/* ADR:0x008e: Work  Register 142									*/
SHORT	WREG143;				/* ADR:0x008f: Work  Register 143									*/
SHORT	WREG144;				/* ADR:0x0090: Work  Register 144									*/
SHORT	WREG145;				/* ADR:0x0091: Work  Register 145									*/
SHORT	WREG146;				/* ADR:0x0092: Work  Register 146									*/
SHORT	WREG147;				/* ADR:0x0093: Work  Register 147									*/
SHORT	WREG148;				/* ADR:0x0094: Work  Register 148									*/
SHORT	WREG149;				/* ADR:0x0095: Work  Register 149									*/
/*--------------------------------------------------------------------------------------------------*/
/*		for INT_AD																					*/
/*--------------------------------------------------------------------------------------------------*/
DWREG	TMP0;					/* ADR:0x0096: (WREGBASE+12C)										*/
DWREG	TMP2;					/* ADR:0x0098: (WREGBASE+130)										*/
DWREG	TMP4;					/* ADR:0x009a: (WREGBASE+134)										*/
DWREG	TMP6;					/* ADR:0x009c: (WREGBASE+138)										*/
DWREG	TMP8;					/* ADR:0x009e: (WREGBASE+13C)										*/
/*--------------------------------------------------------------------------------------------------*/
/*		CPUkからの受け取りレジスタ																	*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	IUOFIN;					/* ADR:0x00a0: (WREGBASE+140) ; A/D transfer offset for iu			*/
SHORT	IVOFIN;					/* ADR:0x00a1: (WREGBASE+142) ; A/D transfer offset for iv			*/
SHORT	KCUIN;					/* ADR:0x00a2: (WREGBASE+144) ; A/D transfer gain for iu			*/
SHORT	KCVIN;					/* ADR:0x00a3: (WREGBASE+146) ; A/D transfer gain for iv			*/
SHORT	IDIN;					/* ADR:0x00a4: (WREGBASE+148) ; Id reference						*/
SHORT	IQIN;					/* ADR:0x00a5: (WREGBASE+14A) ; Iq reference						*/
SHORT	PHAIN;					/* ADR:0x00a6: (WREGBASE+14C) ; PHA input							*/
SHORT	VELIN;					/* ADR:0x00a7: (WREGBASE+14E) ; VEL input							*/
/*--------------------------------------------------------------------------------------------------*/
/*		static variable																				*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	KC;						/* ADR:0x00a8: (WREGBASE+150) ; Current conversion Gain				*/
SHORT	LD;						/* ADR:0x00a9: (WREGBASE+152) ; d axis Inductance					*/
SHORT	LQ;						/* ADR:0x00aa: (WREGBASE+154) ; q axis Inductance					*/
SHORT	MAG;					/* ADR:0x00ab: (WREGBASE+156) ; Magnetic flux (Phi)					*/
SHORT	KDP;					/* ADR:0x00ac: (WREGBASE+158) ; d axis propotion gain (PI control)	*/
SHORT	KQP;					/* ADR:0x00ad: (WREGBASE+15A) ; q axis propotion gain (PI control)	*/
SHORT	KDI;					/* ADR:0x00ae: (WREGBASE+15C) ; d axis integral time (gain)			*/
SHORT	KQI;					/* ADR:0x00af: (WREGBASE+15E) ; q axis integral time (gain)			*/
SHORT	VDLIM;					/* ADR:0x00b0: (WREGBASE+160) ; d axis integral limit				*/
SHORT	VQLIM;					/* ADR:0x00b1: (WREGBASE+162) ; q axis integral limit				*/
SHORT	KVVIN;					/* ADR:0x00b2: (WREGBASE+164) ; Voltage Compasation gain			*/
SHORT	ODLVL;					/* ADR:0x00b3: (WREGBASE+166) ; On delay change level(97.5.26 mo)	*/
SHORT	TFIL;					/* ADR:0x00b4: (WREGBASE+168) ; Filter time constant				*/
SHORT	VMAX;					/* ADR:0x00b5: (WREGBASE+16A) ; voltage limit data (Vmax^2)			*/
SHORT	ONDV;					/* ADR:0x00b6: (WREGBASE+16C) ; On-delay compensation				*/
SHORT	CSW;					/* ADR:0x00b7: (WREGBASE+16E) ; Control Flag						*/
SHORT	CRFQ;					/* ADR:0x00b8: (WREGBASE+170) ; carrier freq reference(change while BB)	*/
SHORT	CSW2;					/* ADR:0x00b9: (WREGBASE+172) ; Control Flag 2	<V894>				*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	Dummy6[2];				/* 0x7172 - 0x7176 */
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		voltage compensation																		*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	VDRIN;					/* ADR:0x00bc: (WREGBASE+178) ; vdref input							*/
SHORT	VQRIN;					/* ADR:0x00bd: (WREGBASE+17A) ; vqref input							*/
/*--------------------------------------------------------------------------------------------------*/
/*		notch1																						*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	KF1;					/* ADR:0x00be: (WREGBASE+17C)										*/
SHORT	KF2;					/* ADR:0x00bf: (WREGBASE+17E)										*/
SHORT	KF3;					/* ADR:0x00c0: (WREGBASE+180)										*/
SHORT	KF4;					/* ADR:0x00c1: (WREGBASE+182)										*/
/*--------------------------------------------------------------------------------------------------*/
/*		notch2																						*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	KF21;					/* ADR:0x00c2: (WREGBASE+184)										*/
SHORT	KF22;					/* ADR:0x00c3: (WREGBASE+186)										*/
SHORT	KF23;					/* ADR:0x00c4: (WREGBASE+188)										*/
SHORT	KF24;					/* ADR:0x00c5: (WREGBASE+18A)										*/
/*--------------------------------------------------------------------------------------------------*/
/*		LPF																							*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	TLPF;					/* ADR:0x00c6: (WREGBASE+18C) ; LPF1 gain							*/
SHORT	TLPFC;					/* ADR:0x00c7: (WREGBASE+18E) ; LPF2 gain							*/
/*--------------------------------------------------------------------------------------------------*/
/*		Torque Limit																				*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	TLMTPIN;				/* ADR:0x00c8: (WREGBASE+190) ; +tlim								*/
SHORT	TLMTMIN;				/* ADR:0x00c9: (WREGBASE+192) ; -tlim								*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	R1;						/* ADR:0x00ca: (WREGBASE+194) ; Moter resistance(r1/I_BASE*V_BASE)	*/
SHORT	KONDV;					/* ADR:0x00cb: (WREGBASE+196) ; Dead-time comp. gain				*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	Dummy7[2];				/* 0x7198 - 0x719A */
/*--------------------------------------------------------------------------------------------------*/
SHORT	L_DIDT;					/* ADR:0x00ce: (WREGBASE+19C) ;										*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	FCCRST;					/* ADR:0x00cf: (WREGBASE+19E) ; FCCST Reset(for TEST)				*/


/*--------------------------------------------------------------------------------------------------*/
/*		for Current Observer																		*/
/*--------------------------------------------------------------------------------------------------*/
// パラメータ用
SHORT	TSPL;					/* ADR:0x00d0: Ts/L ( オブザーバ用パラメータ )			ADDR = 080071A0	*/
SHORT	GOBS;					/* ADR:0x00d1: g ( オブザーバの極 )					ADDR = 080071A2	*/
SHORT	RLTS;					/* ADR:0x00d2: 1-R･Ts/L ( オブザーバ用パラメータ )		ADDR = 080071A4	*/
SHORT	FILOBS;					/* ADR:0x00d3: フィルタゲイン							ADDR = 080071A6	*/
// 計算用
DWREG	LFILQL;					/* ADR:0x00d4: q軸ローパスフィルタ変数 ( Low byte )	ADDR = 080071A8	*/
DWREG	HFILQL;					/* ADR:0x00d6: q軸ハイパスフィルタ変数 ( Low byte )	ADDR = 080071AC	*/
SHORT	IQOBS;					/* ADR:0x00d8: q軸オブザーバ出力						ADDR = 080071B0	*/
SHORT	IQOBSF;					/* ADR:0x00d9: q軸振動成分								ADDR = 080071B2	*/
DWREG	LFILDL;					/* ADR:0x00da: d軸ローパスフィルタ変数 ( Low byte )	ADDR = 080071B4	*/
DWREG	HFILDL;					/* ADR:0x00dc: d軸ハイパスフィルタ変数 ( Low byte )	ADDR = 080071B8	*/
SHORT	IDOBS;					/* ADR:0x00de: d軸オブザーバ出力						ADDR = 080071BC	*/
SHORT	IDOBSF;					/* ADR:0x00df: d軸振動成分								ADDR = 080071BE	*/
SHORT	DMPG;					/* ADR:0x00e0: 電流オブザーバダンピングゲイン			ADDR = 080071C0	*/

/*--------------------------------------------------------------------------------------------------*/
/*		notch4		<V894>																			*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	KF41;					/* ADR:0x00e1: ADDR = 080071C2										*/
SHORT	KF42;					/* ADR:0x00e2: ADDR = 080071C4										*/
SHORT	KF43;					/* ADR:0x00e3: ADDR = 080071C6										*/
SHORT	KF44;					/* ADR:0x00e4: ADDR = 080071C8										*/

/*--------------------------------------------------------------------------------------------------*/
/*		notch5		<V894>																			*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	KF51;					/* ADR:0x00e5: ADDR = 080071CA										*/
SHORT	KF52;					/* ADR:0x00e6: ADDR = 080071CC										*/
SHORT	KF53;					/* ADR:0x00e7: ADDR = 080071CE										*/
SHORT	KF54;					/* ADR:0x00e8: ADDR = 080071D0										*/

/*--------------------------------------------------------------------------------------------------*/
/*		notch6		<V894>																			*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	KF61;					/* ADR:0x00e9: ADDR = 080071D2										*/
SHORT	KF62;					/* ADR:0x00ea: ADDR = 080071D4										*/
SHORT	KF63;					/* ADR:0x00eb: ADDR = 080071D6										*/
SHORT	KF64;					/* ADR:0x00ec: ADDR = 080071D8										*/

/*--------------------------------------------------------------------------------------------------*/
SHORT	Dummy8[19];				/* 0x71DA - 0x71FE */
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		for Phase Interpolate			<V112>														*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	PHAIP;					/* ADR:0x0100: 位相補間量								ADDR = 08007200	*/
SHORT	PHAIPFLG;				/* ADR:0x0101: 位相補間フラグ(0：しない、1：する)		ADDR = 08007202	*/
SHORT	PHAIPIN;				/* ADR:0x0102: 位相補間量(CPU→ASIC受け渡し用)			ADDR = 08007204	*/
SHORT	PHAIPFLGIN;				/* ADR:0x0103: 位相補間フラグ(CPU→ASIC受け渡し用)		ADDR = 08007206	*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	Dummy9[12];				/* 0x7208 - 0x721E */
/*--------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------*/
/*		for Encoder IF																		<V720>	*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	AMPTYPE;				/* ADR:0x0110: Rotary Type or Linear Type				ADDR = 08007220	*/
SHORT	Dummy10;				/* 0x7222 ; Dummy */
/*--------------------------------------------------------------------------------------------------*/
DWREG	RCVPOSX0;				/* ADR:0x0112: Motor Encoder Position(受信位置) Low		ADDR = 08007224	*/
DWREG	RCVPOSX1;				/* ADR:0x0114: Motor Encoder Position(受信位置) Low		ADDR = 08007228	*/
DWREG	RCVPOSX2;				/* ADR:0x0116: Motor Encoder Position(受信位置) Low		ADDR = 0800722C	*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	MPOSSFTR;				/* ADR:0x0118: Shift Data for Motor Position Cal.		ADDR = 08007230	*/
SHORT	MPOSSFTX;				/* ADR:0x0119: Shift Data for Motor Position Cal.		ADDR = 08007232	*/
SHORT	MPOSSIGN;				/* ADR:0x011a: Sign  Data for Motor Position Cal.		ADDR = 08007234	*/
SHORT	DIVOUTSFT;				/* ADR:0x011b: 分周出力シフト数							ADDR = 08007236	*/
DWREG	ACCERRLVL;				/* ADR:0x011c: Motor Encoder Acc. Error Check Low		ADDR = 08007238	*/
/*--------------------------------------------------------------------------------------------------*/
DWREG	DIVPLS;					/* ADR:0x011e: 分周出力パルス Low						ADDR = 0800723C	*/
/*--------------------------------------------------------------------------------------------------*/
DWREG	DIVOUTGAIN;				/* ADR:0x0120: 分周出力ゲイン(リニア) Low				ADDR = 08007240	*/
DWREG	DIVPOS;					/* ADR:0x0122: 分周出力パルス(リニア) Low				ADDR = 08007244	*/
DWREG	DIVREM;					/* ADR:0x0124: 分周出力パルス余り(リニア) Low			ADDR = 08007248	*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	SPGFAIL;				/* ADR:0x0126: S-PG受信失敗許容回数						ADDR = 0800724C	*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	BITIPRM;				/* ADR:0x0127: Bit Initial Parameter						ADDR = 0800724E	*/
SHORT	BITDAT;					/* ADR:0x0128: Bit Data									ADDR = 08007250	*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	INCPLSREQ;				/* ADR:0x0129: PAO IncrePulse Output Request				ADDR = 08007252	*/
SHORT	PAOSEQCMD;				/* ADR:0x012a: PAO Serial Output Sequence				ADDR = 08007254	*/
SHORT	POSETCMD;				/* ADR:0x012b: パルス出力回路初期化要求					ADDR = 08007256	*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	Dummy11[4];				/* 0x7258 - 0x725E */
/*--------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------*/
/*		分周パルス出力関連：HostCPU --> Asic 定周期転送用									<V720>	*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	INCPLSREQIN;			/* ADR:0x0130: PAO IncrePulse Output Request				ADDR = 08007260	*/
SHORT	PAOSEQCMDIN;			/* ADR:0x0131: PAO Serial Output Sequence				ADDR = 08007262	*/
SHORT	POSETCMDIN;				/* ADR:0x0132: パルス出力回路初期化要求					ADDR = 08007264	*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	DIVSETIN;				/* ADR:0x0133: 分周機能設定入力							ADDR = 08007266	*/
SHORT	POSET1IN;				/* ADR:0x0134: パルス変換原点補正1						ADDR = 08007268	*/
SHORT	POSET2IN;				/* ADR:0x0135: パルス変換原点補正2						ADDR = 0800726A	*/
/*--------------------------------------------------------------------------------------------------*/
DWREG	INCPLSIN;				/* ADR:0x0136: 初期インクレパルス Low					ADDR = 0800726C	*/
DWREG	INCREMIN;				/* ADR:0x0138: 初期インクレパルス余り Low				ADDR = 08007270	*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	ACCCNTCLRIN;			/* ADR:0x013a: 加速度チェック開始カウントクリア要求		ADDR = 08007274	*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	Dummy12[5];				/* 0x7276 - 0x7280 */
/*--------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------*/
/*		初期化処理用ワーク																	<V720>	*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	IN_WK0;					/* ADR:0x0140: 初期化処理用 Work0						ADDR = 08007280	*/
SHORT	IN_WK0H;				/* ADR:0x0141: Work0						ADDR = 08007282			*/
SHORT	IN_WK1;					/* ADR:0x0142: Work1						ADDR = 08007284			*/
SHORT	IN_WK1H;				/* ADR:0x0143: Work1						ADDR = 08007286			*/

/*--------------------------------------------------------------------------------------------------*/
/*		@INT_ENC用ワーク																	<V720>	*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	EN_WK0;					/* ADR:0x0144: Encoder Interrup	Work0 					ADDR = 08007288	*/
SHORT	EN_WK0H;				/* ADR:0x0145: Work0					ADDR = 0800728A				*/
SHORT	EN_WK1;					/* ADR:0x0146: Work1					ADDR = 0800728C				*/
SHORT	EN_WK1H;				/* ADR:0x0147: Work1					ADDR = 0800728E				*/
/*--------------------------------------------------------------------------------------------------*/
DWREG	RXPOSL;					/* ADR:0x0148: Encoder Position Data Low					ADDR = 08007290	*/
SHORT	RXFLG0;					/* ADR:0x014a: エンコーダ受信チェックフラグ				ADDR = 08007294	*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	Dummy13[6];				/* 0x7294 - 0x72A0 */
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		分周パルス更新処理(@INT_HOST)用ワーク												<V720>	*/
/*--------------------------------------------------------------------------------------------------*/
DWREG	DV_WK0;					/* ADR:0x0150: 分周パルス更新処理用	Work0				ADDR = 080072A0	*/
DWREG	DV_WK1;					/* ADR:0x0152: Work1				ADDR = 080072A4					*/
/*--------------------------------------------------------------------------------------------------*/
DWREG	RXPOSL0;				/* ADR:0x0154: Encoder Position Data Low					ADDR = 080072A8	*/
DWREG	RCVPOSX;				/* ADR:0x0156: Motor Encoder Position Low				ADDR = 080072AC	*/
DWREG	LRCVPOSX;				/* ADR:0x0158: Last Motor Encoder Position Low			ADDR = 080072B0	*/
/*--------------------------------------------------------------------------------------------------*/
DWREG	DMOTPOS;				/* ADR:0x015a: Delta Motor Position Low					ADDR = 080072B4	*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	MSTERRCNT;				/* ADR:0x015c: Encoder Mst. Error Counter				ADDR = 080072B8	*/
SHORT	ACCCHKCNT;				/* ADR:0x015d: ADDR = 080072BA										*/
DWREG	MOTACC;					/* ADR:0x015e: Motor Acc.	(2Word)						ADDR = 080072BC	*/
/*--------------------------------------------------------------------------------------------------*/
DWREG	INCPLS;					/* ADR:0x0160: 初期インクレパルス出力 Low				ADDR = 080072C0	*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	DIVSETW;				/* ADR:0x0162: 分周機能設定								ADDR = 080072C4	*/
SHORT	POSET1W;				/* ADR:0x0163: パルス変換原点補正1						ADRR = 080072C6	*/
SHORT	POSET2W;				/* ADR:0x0164: パルス変換原点補正2						ADDR = 080072C8	*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	Dummy14[91];			/* 0x72CA - 0x737E */
/*--------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------*/
/*		Register for Library																<V720>	*/
/*--------------------------------------------------------------------------------------------------*/
DWREG	AG0;					/* ADR:0x01c0: Argument0									ADDR = 08007380	*/
DWREG	AG1;					/* ADR:0x01c2: Argument1									ADDR = 08007384	*/
SHORT	AG2;					/* ADR:0x01c4: Argument2									ADDR = 08007388	*/
SHORT	AG2H;					/* ADR:0x01c5: Argument2 High Word						ADDR = 0800738A	*/
DWREG	R0;						/* ADR:0x01c6: Return Register							ADDR = 0800738C	*/
DWREG	KX;						/* ADR:0x01c8: kx										ADDR = 08007390	*/
DWREG	SX;						/* ADR:0x01ca: sx										ADDR = 08007394	*/
DWREG	IU0;					/* ADR:0x01cc: iu[0]										ADDR = 08007398	*/
SHORT	IU1;					/* ADR:0x01ce: iu[1]										ADDR = 0800739C	*/
SHORT	IU1H;					/* ADR:0x01cf: iu[1]										ADDR = 0800739E	*/
SHORT	LWK0;					/* ADR:0x01d0: swk0 for Library							ADDR = 080073A0	*/
SHORT	LWK0H;					/* ADR:0x01d1: swk0 for Library							ADDR = 080073A2	*/
DWREG	LWK1;					/* ADR:0x01d2: swk1 for Library							ADDR = 080073A4	*/
DWREG	LWK2;					/* ADR:0x01d4: swk2 for Library							ADDR = 080073A8	*/
DWREG	LWK3;					/* ADR:0x01d6: swk3 for Library							ADDR = 080073AC	*/
DWREG	LWK4;					/* ADR:0x01d8: wk4 for Library							ADDR = 080073B0	*/
SHORT	LWK5;					/* ADR:0x01da: wk5 for Library							ADDR = 080073B4	*/
SHORT	LWK5H;					/* ADR:0x01db: wk5 for Library							ADDR = 080073B6	*/
SHORT	LWK6;					/* ADR:0x01dc: wk6 for Library							ADDR = 080073B8	*/
SHORT	LWK6H;					/* ADR:0x01dd: wk6 for Library							ADDR = 080073BA	*/
SHORT	LWK7;					/* ADR:0x01de: wk7 for Library							ADDR = 080073BC	*/
SHORT	LWK7H;					/* ADR:0x01df: wk7 for Library							ADDR = 080073BE	*/
SHORT	LWK8;					/* ADR:0x01e0: wk8 for Library							ADDR = 080073C0	*/
SHORT	LWK8H;					/* ADR:0x01e1: wk8 for Library							ADDR = 080073C2	*/
SHORT	LWK9;					/* ADR:0x01e2: wk9 for Library							ADDR = 080073C4	*/
SHORT	LWK9H;					/* ADR:0x01e3: wk9 for Library							ADDR = 080073C6	*/
DWREG	LWK10;					/* ADR:0x01e4: wk10 for Library							ADDR = 080073C8	*/
DWREG	LWK11;					/* ADR:0x01e6: wk11 for Library							ADDR = 080073CC	*/
SHORT	LWK12;					/* ADR:0x01e8: wk12 for Library							ADDR = 080073D0	*/
SHORT	LWK12H;					/* ADR:0x01e9: wk12 for Library							ADDR = 080073D2	*/
SHORT	MLIBTST;				/* ADR:0x01ea: for MprgMlibTest.src						ADDR = 080073D4	*/
SHORT	MLIBTSTH;				/* ADR:0x01eb: for MprgMlibTest.src						ADDR = 080073D6	*/
DWREG	ONER;					/* ADR:0x01ec: one value register						ADDR = 080073D8	*/
DWREG	NONER;					/* ADR:0x01ee: -1 value register							ADDR = 080073DC	*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	Dummy15[16];			/* 0x73E0 - 0x73FE */
/*--------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------*/
/*		for axis q monitor				<V224>														*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	IQDISTIN;				/* ADR:0x0200: q軸外乱トルク入力							ADDR = 08007400	*/
SHORT	IQDIST;					/* ADR:0x0201: q軸外乱トルク								ADDR = 08007402	*/
SHORT	IQMONFIL;				/* ADR:0x0202: q軸電流指令モニタ(フィルタ後)				ADDR = 08007404	*/
SHORT	IQOFREF;				/* ADR:0x0203: q軸電流指令(外乱トルク加算後)				ADDR = 08007406	*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	Dummy16[76];				/* 0x7408 - 0x749E */
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		for notch filter1 				<V388>														*/
/*--------------------------------------------------------------------------------------------------*/
DWREG	IQO1L;					/* ADR:0x0250: filter1 output(Low)						ADDR = 080074A0	*/
DWREG	IQO1PL;					/* ADR:0x0252: filter1 output 1delay(Low)				ADDR = 080074A4	*/
DWREG	IQO1PPL;				/* ADR:0x0254: filter1 output 2delay(Low)				ADDR = 080074A8	*/
DWREG	IQIPL;					/* ADR:0x0256: filter1 input 1delay(Low)					ADDR = 080074AC	*/
DWREG	IQIPPL;					/* ADR:0x0258: filter1 input 2delay(Low)					ADDR = 080074B0	*/
//   	IQO1;					/* ADR:0x026a: filter1 output(short type)				ADDR = 080074B4		// <V502>	*/
SHORT	IQO1;					/* ADR:0x025a: filter1 output(short type)				ADDR = 080074B4		// <V502>	*/
DWREG	IQO1BUFL;				/* ADR:0x025c: filter1 output buffer(Low)				ADDR = 080074B8		// <V502>	*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	Dummy17[2];				/* 0x74BC - 0x74BE */
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		for notch filter2 				<V388>														*/
/*--------------------------------------------------------------------------------------------------*/
DWREG	IQO2L;					/* ADR:0x0260: filter2 output(Low)						ADDR = 080074C0	*/
DWREG	IQO2PL;					/* ADR:0x0262: filter2 output 1delay(Low)				ADDR = 080074C4	*/
DWREG	IQO2PPL;				/* ADR:0x0264: filter2 output 2delay(Low)				ADDR = 080074C8	*/
DWREG	IQI2PL;					/* ADR:0x0266: filter2 input 1delay(Low)					ADDR = 080074CC	*/
DWREG	IQI2PPL;				/* ADR:0x0268: filter2 input 2delay(Low)					ADDR = 080074D0	*/
SHORT	IQO2;					/* ADR:0x026a: filter2 output(short type)				ADDR = 080074D4	*/
DWREG	IQO2BUFL;				/* ADR:0x026c: filter2 output buffer(Low)				ADDR = 080074D8		// <V502>	*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	Dummy18[2];				/* 0x74DC - 0x74DE */
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		for notch filter3 				<V388>														*/
/*--------------------------------------------------------------------------------------------------*/
DWREG	IQO3L;					/* ADR:0x0270: filter3 output(Low)						ADDR = 080074E0	*/
DWREG	IQO3PL;					/* ADR:0x0272: filter3 output 1delay(Low)				ADDR = 080074E4	*/
DWREG	IQO3PPL;				/* ADR:0x0274: filter3 output 2delay(Low)				ADDR = 080074E8	*/
DWREG	IQI3PL;					/* ADR:0x0276: filter3 input 1delay(Low)					ADDR = 080074EC	*/
DWREG	IQI3PPL;				/* ADR:0x0278: filter3 input 2delay(Low)					ADDR = 080074F0	*/
SHORT	IQO3;					/* ADR:0x027a: filter3 output(short type)				ADDR = 080074F4	*/
DWREG	IQO3BUFL;				/* ADR:0x027c: filter3 output buffer(Low)				ADDR = 080074F8		// <V502>	*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	Dummy19[2];				/* 0x74FC - 0x74FE */
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		for notch filter4 				<V894>														*/
/*--------------------------------------------------------------------------------------------------*/
DWREG	IQO4L;					/* ADR:0x0280: filter4 output(Low)						ADDR = 08007500	*/
DWREG	IQO4PL;					/* ADR:0x0282: filter4 output 1delay(Low)				ADDR = 08007504	*/
DWREG	IQO4PPL;				/* ADR:0x0284: filter4 output 2delay(Low)				ADDR = 08007508	*/
DWREG	IQI4PL;					/* ADR:0x0286: filter4 input 1delay(Low)					ADDR = 0800750C	*/
DWREG	IQI4PPL;				/* ADR:0x0288: filter4 input 2delay(Low)					ADDR = 08007510	*/
SHORT	IQO4;					/* ADR:0x028a: filter4 output(short type)				ADDR = 08007514	*/
DWREG	IQO4BUFL;				/* ADR:0x028c: filter4 output buffer(Low)				ADDR = 08007518	*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	Dummy20[2];				/* 0x751C - 0x751E */
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		for notch filter5 				<V894>														*/
/*--------------------------------------------------------------------------------------------------*/
DWREG	IQO5L;					/* ADR:0x0290: filter5 output(Low)						ADDR = 08007520	*/
DWREG	IQO5PL;					/* ADR:0x0292: filter5 output 1delay(Low)				ADDR = 08007524	*/
DWREG	IQO5PPL;				/* ADR:0x0294: filter5 output 2delay(Low)				ADDR = 08007528	*/
DWREG	IQI5PL;					/* ADR:0x0296: filter5 input 1delay(Low)					ADDR = 0800752C	*/
DWREG	IQI5PPL;				/* ADR:0x0298: filter5 input 2delay(Low)					ADDR = 08007530	*/
SHORT	IQO5;					/* ADR:0x029a: filter5 output(short type)				ADDR = 08007534	*/
DWREG	IQO5BUFL;				/* ADR:0x029c: filter5 output buffer(Low)				ADDR = 08007538	*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	Dummy21[2];				/* 0x753C - 0x753E */
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		for notch filter6 				<V894>														*/
/*--------------------------------------------------------------------------------------------------*/
DWREG	IQO6L;					/* ADR:0x02a0: filter6 output(Low)						ADDR = 08007540	*/
DWREG	IQO6PL;					/* ADR:0x02a2: filter6 output 1delay(Low)				ADDR = 08007544	*/
DWREG	IQO6PPL;				/* ADR:0x02a4: filter6 output 2delay(Low)				ADDR = 08007548	*/
DWREG	IQI6PL;					/* ADR:0x02a6: filter6 input 1delay(Low)					ADDR = 0800754C	*/
DWREG	IQI6PPL;				/* ADR:0x02a8: filter6 input 2delay(Low)					ADDR = 08007550	*/
SHORT	IQO6;					/* ADR:0x02aa: filter6 output(short type)				ADDR = 08007554	*/
DWREG	IQO6BUFL;				/* ADR:0x02ac: filter6 output buffer(Low)				ADDR = 08007558	*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	Dummy22[82];			/* 0x755C - 0x75FE */
/*--------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------*/
/*		for Weaken Field		<V214>																*/
/*--------------------------------------------------------------------------------------------------*/
//----- CPUから渡される変数 -----
SHORT	WFKPVLIN;				/* ADR:0x0300: 電圧FB比例ゲイン(下位16bit)						ADDR = 08007600	*/
SHORT	WFKPVHIN;				/* ADR:0x0301: 電圧FB比例ゲイン(上位16bit)						ADDR = 08007602	*/
SHORT	WFKIVLIN;				/* ADR:0x0302: 電圧FB積分ゲイン(下位16bit)						ADDR = 08007604	*/
SHORT	WFKIVHIN;				/* ADR:0x0303: 電圧FB積分ゲイン(上位16bit)						ADDR = 08007606	*/
SHORT	WFV1MAXIN;				/* ADR:0x0304: 電圧指令制限レベル								ADDR = 08007608	*/
SHORT	WFIDREFLIMIN;			/* ADR:0x0305: d軸電流指令リミット								ADDR = 0800760A	*/
//
//----- CPUから初期化時に渡される変数 -----
SHORT	WFINTEGLIM;				/* ADR:0x0306: 電圧FB積分リミット								ADDR = 0800760C	*/
//
//----- マイクロで扱う変数 -----
SHORT	Dummy23;				/* 0800760E */
DWREG	WFKPVL;					/* ADR:0x0308: 電圧FB比例ゲイン(下位16bit)						ADDR = 08007610	*/
DWREG	WFKIVL;					/* ADR:0x030a: 電圧FB積分ゲイン(下位16bit)						ADDR = 08007614	*/
SHORT	WFV1MAX;				/* ADR:0x030c: 電圧指令制限レベル								ADDR = 08007618	*/
SHORT	WFIDREFLIM;				/* ADR:0x030d: d軸電流指令リミット								ADDR = 0800761A	*/
SHORT	WFVQMAX;				/* ADR:0x030e: q軸電圧指令リミット								ADDR = 0800761C	*/
SHORT	Dummy24;				/* 0x761E ; Dummy */
DWREG	WFINTEGL;				/* ADR:0x0310: 電圧FB積分値										ADDR = 08007620	*/
SHORT	WFVDREF;				/* ADR:0x0312: d軸電圧指令										ADDR = 08007624	*/
SHORT	WFVQREF;				/* ADR:0x0313: q軸電圧指令										ADDR = 08007626	*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	Dummy25[236];			/* 0x7628 - 0x77FE */
/*--------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------*/
/*		マイクロ演算部ソフトバージョン情報 : マイクロ演算部→ホストCPU転送用				<V720>	*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	MSWVERSION;				/* ADR:0x0400: ソフトバージョン情報								ADDR = 08007800	*/
SHORT	TSTVERSION;				/* ADR:0x0401: テストバージョン情報								ADDR = 08007802	*/
SHORT	YSPVERSION;				/* ADR:0x0402: Ｙ仕様バージョン情報								ADDR = 08007804	*/
} JL076_MICRO_HANDLE;
extern	JL076_MICRO_HANDLE	AxisHdl[2];

/*--------------------------------------------------------------------------------------------------*/
/*		AMPTYPE : 回転型/リニア型選択														<V720>	*/
/*--------------------------------------------------------------------------------------------------*/
SHORT	ROTARY;					/* ADR:0x0000: 0	: 回転型										*/
#define LINEAR		0x0001		/* 1	: リニア型													*/

/*--------------------------------------------------------------------------------------------------*/
/*		PAOSEQCMD : PaoSeqMcmd Interface Definition											<V720>	*/
/*--------------------------------------------------------------------------------------------------*/
#define PAONOCMD	0x0000		/* No Command              (Host CPU:KPX_PAOSEQ_NOCMD)				*/
#define PAOPLSOUT	0x0020		/* IncPulse Output Request (Host CPU:KPX_IPLSOUT_REQUEST)			*/

/*--------------------------------------------------------------------------------------------------*/
/*		POSETCMD : パルス出力回路初期化要求　(絶対値授受シーケンスのパルス出力回路初期化用)	<V720>	*/
/*--------------------------------------------------------------------------------------------------*/
#define POSETNOCMD	0x0000		/* 要求なし															*/
#define POSETCMD00	0x0001		/* パルス変換位置に0000hを強制セット要求							*/
#define POSETCMDFF	0x0002		/* パルス変換位置にFFFFhを強制セット要求							*/

/*--------------------------------------------------------------------------------------------------*/
/*		BITIPRM																				<V720>	*/
/*--------------------------------------------------------------------------------------------------*/
#define UPGDIVOUT	0x0001		/* bit0 : マイクロ分周機能使用選択									*/
//      equ			0x0002		/* bit1 : 空き														*/
//      equ			0x0004		/* bit2 : 空き														*/
//      equ			0x0008		/* bit3 : 空き														*/
//      equ			0x0010		/* bit4 : 空き														*/
//      equ			0x0020		/* bit5 : 空き														*/
//      equ			0x0040		/* bit6 : 空き														*/
//      equ			0x0080		/* bit7 : 空き														*/
//      equ			0x0100		/* bit8 : 空き														*/
//      equ			0x0200		/* bit9 : 空き														*/
//      equ			0x0400		/* bitA : 空き														*/
//      equ			0x0800		/* bitB : 空き														*/
//      equ			0x1000		/* bitC : 空き														*/
//      equ			0x2000		/* bitD : 空き														*/
//      equ			0x4000		/* bitE : 空き														*/
//      equ			0x8000		/* bitF : 空き														*/


/*--------------------------------------------------------------------------------------------------*/
/*		BITDAT 																				<V720>	*/
/*--------------------------------------------------------------------------------------------------*/
#define SERXERR		0x0001		/* bit0 : Senc Receive Error										*/
#define ACCCHKENA	0x0002		/* bit1 : Encoder Acc. Check Enable Flag							*/
//      equ			0x0004		/* bit2 : 空き														*/
//      equ			0x0008		/* bit3 : 空き														*/
//      equ			0x0010		/* bit4 : 空き														*/
//      equ			0x0020		/* bit5 : 空き														*/
//      equ			0x0040		/* bit6 : 空き														*/
//      equ			0x0080		/* bit7 : 空き														*/
//      equ			0x0100		/* bit8 : 空き														*/
//      equ			0x0200		/* bit9 : 空き														*/
//      equ			0x0400		/* bitA : 空き														*/
//      equ			0x0800		/* bitB : 空き														*/
//      equ			0x1000		/* bitC : 空き														*/
//      equ			0x2000		/* bitD : 空き														*/
//      equ			0x4000		/* bitE : 空き														*/
//      equ			0x8000		/* bitF : 空き														*/

/****************************************************************************************************/
/*																									*/
/*		SGDM immidiate Definition																	*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		CSW ( control select switch ) 																*/
/*--------------------------------------------------------------------------------------------------*/
#define ICLR		0x0001		/* bit.0 : Integration clear bit									*/
#define ISEL		0x0002		/* bit.1 : Voltage compensation select bit							*/
//      --			0x0004		/* bit.2 : ----														*/
//      --			0x0008		/* bit.3 : ----														*/
#define OBSSEL		0x0008		/* bit.3 : Current Observer Select bit	; <V038>	<V076>			*/
#define F1DSABL		0x0010		/* bit.4 : Notch filter1 Disable									*/
#define F2DSABL		0x0020		/* bit.5 : Notch filter2 Disable									*/
#define LPFDSABL	0x0040		/* bit.6 : Low Pass filter1 Disable									*/
#define LPFCDSABL	0x0080		/* bit.7 : Low Pass filter2 Disable									*/
#define OVMSEL1		0x0100		/* bit.8 : Over modulation select(bit 8,9)							*/
#define OVMSEL2		0x0200		/* bit.9 : if bit8,9 = 10 or 11  then OVMOD2						*/
								//         if bit8,9 = 01        then OVMOD1
#define OVMMOD		0x0400		/* bit.A : modulation modification enable							*/
#define V_FB		0x0800		/* bit.B : 電圧FB方式弱め界磁選択									*/
#define DIDTSEL		0x1000		/* bit.C : L di/dt cmp select(1:di/dt,0:def)						*/
//      ANTIWU		0x2000		/* bit.D : anti-windup test											*/
#define V_FB2		0x2000		/* bit.D : 電圧FB方式弱め界磁選択2	<V531>							*/
#define INT_ST		0x4000		/* bit.E : 飽和時積分停止											*/
#define F3DSABL		0x8000		/* bit.F : Notch filter3 Disable									*/

/*--------------------------------------------------------------------------------------------------*/
/*		CSW2 ( control select switch 2 ) <V894>														*/
/*--------------------------------------------------------------------------------------------------*/
#define F4DSABL		0x0001		/* bit.0 : Notch filter4 Disable									*/
#define F5DSABL		0x0002		/* bit.1 : Notch filter5 Disable									*/
#define F6DSABL		0x0004		/* bit.2 : Notch filter6 Disable									*/

/*--------------------------------------------------------------------------------------------------*/
/*		CPORT read data			  bit	address 4a0h												*/
/*--------------------------------------------------------------------------------------------------*/
//      --			0x0001		/* bit.0 :															*/
//      --			0x0002		/* bit.1 :															*/
//      --			0x0004		/* bit.2 :															*/
#define RLOCK		0x0008		/* bit.3 : Transer register lock status								*/
#define BB			0x0010		/* bit.4 : Base block bit											*/
/*--------------------------------------------------------------------------------------------------*/
/*		status set				  bit																*/
/*--------------------------------------------------------------------------------------------------*/
//      --			0x0004		/* bit.2 :															*/
#define DLIM		0x0008		/* bit.3 : D-axis integration limit status							*/
#define QLIM		0x0008		/* bit.3 : Q-axis integration limit status							*/
//      --			0x0010		/* bit.4 :															*/
#define TLIM		0x0020		/* bit.5 : Torque Limit status										*/
#define IHOLD		0x0040		/* bit.6 : Anti-windup test											*/
//      --			0x0080		/* bit.7 :															*/
/*--------------------------------------------------------------------------------------------------*/
/*		status clear			  bit																*/
/*--------------------------------------------------------------------------------------------------*/
//      --			0xfffb		/* bit.2 :															*/
#define DLIMI		0xfff7		/* bit.3 :															*/
#define QLIMI		0xfff7		/* bit.3 :															*/
//      --			0xffef		/* bit.4 :															*/
#define TLIMI		0xffdf		/* bit.5 :															*/
#define IHOLDI		0xffbf		/* bit.6 : Anti-windup test											*/
//      NON			0xff7f		/* bit.7 :															*/
/*--------------------------------------------------------------------------------------------------*/
/*		static value																				*/
/*--------------------------------------------------------------------------------------------------*/
#define PI2			0x4000		/* 90deg															*/
#define PI23		0x5555		/* 120deg															*/



#define _MPRGSGDV_H_
#endif /* _MPRGSGDV_H_ */
/***************************************** end of file **********************************************/
