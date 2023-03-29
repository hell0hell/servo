/****************************************************************************
Description	: JL086 ServoIP Hardware Definition
----------------------------------------------------------------------------
Author			: Yaskawa Electric Corp.,
					Copyright (c) 2012 All Rights Reserved

Project			: Mercury

Functions		:

----------------------------------------------------------------------------
Changes			:
	Name	Date		Description
  ------------------------------------------------------------------------
	Y.Oka	2012.08.06	created

****************************************************************************/
#ifndef _JL076_USE_

#include "Basedef.h"
#include "HwDeviceIf.h"
#include "JL086ServoIpHw.h"
#include "KLib.h"
#include "EncData.h"/* <S004> */

#include "ARMPF.h"
#include "ARMPF_SCU.h"

//#include "MpConstTbl.h"

/* <S068> */
#define TABLESTART	0xC0007A00
#define TABLESIZE 	64

//<S146>extern CSHORT OVMODTBLG[16][2];
//extern CSHORT OVMODTBLO[16][2];//<S0C7>
/* <S068> */

/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
/* Low Level functions */
void	SHal_InitAsicSystem( ASIC_HW_IF_STR* );
void	SHal_InitAsicAlarm( ASIC_HW_IF_STR* );
void	SHal_InitAsicInterrupt( ASIC_HW_IF_STR* );
void	SHal_InitAsicSencIF( ASIC_HW_IF_STR* );
void	SHal_InitAsicSerialIF( ASIC_HW_IF_STR* );
LONG	SHal_InitAsicPulseIF( ASIC_HW_IF_STR *pAsicHwReg, USHORT prstsw, USHORT syssw1, USHORT comfil );
void	SHal_InitAsicAdInterface( ASIC_HW_IF_STR* pAsicHwReg );
void	SHal_InitAsicTimer( USHORT syssw2, ASIC_HW_IF_STR *pAsicHwReg );
void	SHal_InitAsicRegenerate( ASIC_HW_IF_STR *pAsicHwReg );
void	SHal_InitAsicPwmControl( ASIC_HW_IF_STR *pAsicHwReg, LONG odt_c );



/****************************************************************************************************/
/*																									*/
/*		ASICソフトウェアリセット処理(軸共通処理)									<S0E4>			*/
/*																									*/
/****************************************************************************************************/
void	SHal_AsicSwReset( ASIC_HW_IF_STR *pAsicHwReg )
{
	/*----------------------------------------------------------------------------------------------*/
	/*		クロック機能の初期化(JL-076でソフトリセット前にクロック有効が必要)						*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SETREG1	= 0x00000005;	/* ASIC Write Enable								*/
	pAsicHwReg->AREG_CSTP		= 0x00000000;	/* ASIC Clock Stop Selection						*/
	pAsicHwReg->AREG_SETREG1	= 0x00000001;	/* ASIC Write Disable								*/

	/*----------------------------------------------------------------------------------------------*/
	/*		ソフトリセット																			*/
	/*----------------------------------------------------------------------------------------------*/
#if 0	/* JL-086A UDLクロック同期不具合対策		*/
	pAsicHwReg->AREG_SETREG1	= 0x00000009;	/* ASIC S/W Reset Enable							*/
	KlibWaitus( 1 );							/* Wait												*/
	pAsicHwReg->AREG_SRESET		= 0x00000000;	/* ASIC Soft Reset									*/
	KlibWaitus( 1 );							/* Wait												*/
	pAsicHwReg->AREG_SETREG1	= 0x00000001;	/* ASIC S/W Reset Disable							*/
#else	/* UDLクロック停止→再開でUDLリセットと同様の効果がある		*/
	SCU_CLK_STOP(UDL_SERVO_CLK);
	KlibWaitus(1);
	SCU_CLK_START(UDL_SERVO_CLK);
	KlibWaitus(1);
#endif
}


/****************************************************************************************************/
/*																									*/
/*		ASIC初期化処理A(パラメータ非依存)															*/
/*																									*/
/****************************************************************************************************/
void	SHal_InitAsicA( ASIC_HW_IF_STR *AsicHwReg )
{
	SHal_InitAsicSystem( AsicHwReg );				/* システム制御機能(Clock,WDT,etc)の初期化		*/
	SHal_InitAsicAlarm( AsicHwReg );				/* アラーム機能の初期化							*/
	SHal_InitAsicInterrupt( AsicHwReg );			/* 割込み機能の初期化							*/
	SHal_InitAsicSencIF( AsicHwReg );				/* シリアルエンコーダＩＦ初期化					*/
	SHal_InitAsicSerialIF( AsicHwReg );				/* シリアルＩＦ初期化							*/
	SHal_InitAsicAdInterface( AsicHwReg );			/* ＡＤインターフェイス初期化					*/

	return;
}



/****************************************************************************************************/
/*																									*/
/*		ASIC初期化処理B(パラメータ依存部)															*/
/*																									*/
/****************************************************************************************************/
BOOL	SHal_InitAsicB( ASIC_HW_IF_STR* pAsicHwReg, SVIP_INIT_PRM *pSvipInitPrm )
{
	BOOL	ret;
	ret = SVIP_NOALM;

	/*------------------------------------------------------------------------------------------*/
	/*		シリアルエンコーダＩＦレジスタ初期化												*/
	/*------------------------------------------------------------------------------------------*/
	if( 0 != (pSvipInitPrm->syssw1 & 0x2000) )
	{ /* PnE0B.D == 1 クロストーク無効 */
		pAsicHwReg->AREG_SEPGSET00 &= (0xffffffff & ~BIT1);
		pAsicHwReg->AREG_SEPGSET10 &= (0xffffffff & ~BIT1);
	}

	/* パルスIF初期化 */
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)
	ret = SHal_InitAsicPulseIF( pAsicHwReg,
								pSvipInitPrm->prstsw,
								pSvipInitPrm->syssw1,
								pSvipInitPrm->comfil );
#endif

	/* タイマ機能の初期化 */
#ifndef CSW_CSTR_DB_REVERSE
	/* タイマ機能の初期化 <2010.12.2 削除>
		論理逆転にしているため、初期化中(WDTがたたけるまで)は、DBを有効にしない */
	SHal_InitAsicTimer( pSvipInitPrm->syssw2, pAsicHwReg );
#endif

	/* 回生機能の初期化(初期化処理の場合) */
	SHal_InitAsicRegenerate( pAsicHwReg );

	/* PWM制御機能の初期化 */
	SHal_InitAsicPwmControl( pAsicHwReg, pSvipInitPrm->odt_c.b.l );

#ifdef CSW_MICRO_DEBUG_MODE
	/* テストモード時のINT2信号出力設定 */
	SHal_SetMicroDebugMode( pAsicHwReg );
#endif

	return ret;
}



/****************************************************************************************************/
/*																									*/
/*		ASIC初期化処理B(パラメータ依存部)	(パラメータリセット用)									*/
/*																									*/
/****************************************************************************************************/
BOOL	SHal_PrstAsicB( ASIC_HW_IF_STR* pAsicHwReg, SVIP_INIT_PRM *pSvipInitPrm )
{
	BOOL	ret;
	ret = SVIP_NOALM;

	/* パルスIF初期化 */
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)
	ret = SHal_InitAsicPulseIF( pAsicHwReg,
								pSvipInitPrm->prstsw,
								pSvipInitPrm->syssw1,
								pSvipInitPrm->comfil );
#endif

#ifndef CSW_CSTR_DB_REVERSE
	/* タイマ機能の初期化 */
	SHal_InitAsicTimer( pSvipInitPrm->syssw2, pAsicHwReg );
#endif

	/* PWM制御機能の初期化 */
	SHal_InitAsicPwmControl( pAsicHwReg, pSvipInitPrm->odt_c.b.l);

	return ret;
}



/****************************************************************************************************/
/*																									*/
/*		システム制御機能(Clock,WDT,etc)の初期化														*/
/*																									*/
/****************************************************************************************************/
void	SHal_InitAsicSystem( ASIC_HW_IF_STR *pAsicHwReg )
{

//LONG work;//<S0C7>

#if	(CSW_SVCF_FULC_MODE == CSW_FULC_BASIC_SUPPORT)
	/*----------------------------------------------------------------------------------------------*/
	/*		ﾌｨｰﾄﾞﾊﾞｯｸｵﾌﾟｼｮﾝによるフルクローズドおよびセミクローズドの位置補正モジュール設定は、		*/
	/*		2軸目に設定する必要がある。																*/
	/*----------------------------------------------------------------------------------------------*//* <S04D> */
ASIC_HW_IF_STR *pAsicHwReg2;

	pAsicHwReg2 = (ASIC_HW_IF_STR*)((UCHAR*)pAsicHwReg + 0x1000);
#endif/* (CSW_SVCF_FULC_MODE == CSW_FULC_BASIC_SUPPORT)	*/

//-	SHal_AsicSwReset()ですでに行われているので不要 /* <S0E4> */
//-	/*----------------------------------------------------------------------------------------------*/
//-	/*		クロック機能の初期化(JL-076でソフトリセット前にクロック有効が必要)						*/
//-	/*----------------------------------------------------------------------------------------------*/
//-	pAsicHwReg->AREG_SETREG1	= 0x00000005;	/* ASIC Write Enable								*/
//-	pAsicHwReg->AREG_CSTP		= 0x00000000;	/* ASIC Clock Stop Selection						*/
//-	pAsicHwReg->AREG_SETREG1	= 0x00000001;	/* ASIC Write Disable								*/

#if	(CSW_SVCF_FULC_MODE == CSW_FULC_BASIC_SUPPORT)
	/*----------------------------------------------------------------------------------------------*/
	/*		ﾌｨｰﾄﾞﾊﾞｯｸｵﾌﾟｼｮﾝによるフルクローズドおよびセミクローズドの位置補正モジュール設定は、		*/
	/*		2軸目に設定する必要がある。																*/
	/*----------------------------------------------------------------------------------------------*//* <S04D> */
	pAsicHwReg2->AREG_SETREG1	= 0x00000005;	/* ASIC Write Enable								*/
	pAsicHwReg2->AREG_CSTP		= 0x00000000;	/* ASIC Clock Stop Selection						*/
	pAsicHwReg2->AREG_SETREG1	= 0x00000001;	/* ASIC Write Disable								*/
#endif/* (CSW_SVCF_FULC_MODE == CSW_FULC_BASIC_SUPPORT)	*/

//-	SHal_AsicSwReset()ですでに行われているので不要 /* <S0E4> */
//	/*----------------------------------------------------------------------------------------------*/
//	/*		ソフトリセット																			*/
//	/*----------------------------------------------------------------------------------------------*/
//#if 0	/* JL-086A UDLクロック同期不具合対策		*/
//	pAsicHwReg->AREG_SETREG1	= 0x00000009;	/* ASIC S/W Reset Enable							*/
//	KlibWaitus( 1 );							/* Wait												*/
//	pAsicHwReg->AREG_SRESET		= 0x00000000;	/* ASIC Soft Reset									*/
//	KlibWaitus( 1 );							/* Wait												*/
//	pAsicHwReg->AREG_SETREG1	= 0x00000001;	/* ASIC S/W Reset Disable							*/
//#else	/* UDLクロック停止→再開でUDLリセットと同様の効果がある		*/
//	SCU_CLK_STOP(UDL_SERVO_CLK);
//	KlibWaitus(1);
//	SCU_CLK_START(UDL_SERVO_CLK);
//	KlibWaitus(1);
//#endif

	/*----------------------------------------------------------------------------------------------*/
	/*		クロック機能の初期化																	*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SETREG1	= 0x00000005;	/* ASIC Write Enable								*/
	/*----------------------------------------------------------------------------------------------*/
#if 0 /* <S0004> 同様の処理が直下にもあるためｺﾒﾝﾄｱｳﾄ */
	pAsicHwReg->AREG_CSTP		= 0x04028410;	/* ASIC Clock Stop Selection						*/
												/*	Encoder ADIF, NCT, Full Closed Cntr,			*/
												/*	New Incre Decoder, OC Detect Clock Stop			*/
	
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)
	pAsicHwReg->AREG_CSTP		|= 0x00080000;	/*  M-II Slave I/F Clock Stop						*/
#elif (CSW_NETIF_TYPE == NETIF_M2_TYPE)
//	pAsicHwReg->AREG_CSTP		|= 0x00004000;	/*  Pulse Counter Clock Stop						*/
#else /* (CSW_NETIF_TYPE == NETIF_M3_TYPE || NETIF_CM_TYPE) */
//	pAsicHwReg->AREG_CSTP		|= 0x00084000;	/*  M-II Slave I/F, Pulse Counter Clock Stop		*/
	pAsicHwReg->AREG_CSTP		|= 0x00080000;	/*  M-II Slave I/F									*/
#endif
#endif /* <S0004> 同様の処理が直下にもあるためｺﾒﾝﾄｱｳﾄ */
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_CSTP		= (0x04028410	/* ASIC Clock Stop Selection						*/
												/*	Encoder ADIF, NCT, Full Closed Cntr,			*/
												/*	New Incre Decoder, OC Detect Clock Stop			*/
								 | (1<<2)		/* bit2: Reference A/D clock Stop 					*/
								 | (1<<14)  	/* bit14: Command Pulse Counter clock Stop			*/
								 | (1<<19)   ); /* bit19: M-II  Slave I/F Clock Stop				*/
								 
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)
	pAsicHwReg->AREG_CSTP		&= ~((1<<2) 	/*  Ref A/D clock enable							*/
									| (1<<14));	/*  Pulse Counter clock enable						*/
#elif (CSW_NETIF_TYPE == NETIF_M2_TYPE)
	pAsicHwReg->AREG_CSTP		&= ~((1<<19)	/*  M-II Slave I/F Clock Enable						*/
									| (1<<14));	/*  Pulse Counter clock enable	(for Latch)			*//* <S0004> */
#else /* (CSW_NETIF_TYPE == NETIF_M3_TYPE || NETIF_CM_TYPE) */
//	pAsicHwReg->AREG_CSTP		&= 0xFFFFFFFF;	/*  												*//* <S0004>削除 */
	pAsicHwReg->AREG_CSTP		&= ~(1<<14);	/*  Pulse Counter clock enable	(for Latch)			*//* <S0004> */
#endif
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SETREG1	= 0x00000001;	/* ASIC Write Disable								*/

#if	(CSW_SVCF_FULC_MODE == CSW_FULC_BASIC_SUPPORT)
	/*----------------------------------------------------------------------------------------------*/
	/*		ﾌｨｰﾄﾞﾊﾞｯｸｵﾌﾟｼｮﾝによるフルクローズドおよびセミクローズドの位置補正モジュール設定は、		*/
	/*		2軸目に設定する必要がある。																*/
	/*----------------------------------------------------------------------------------------------*//* <S04D> */
	pAsicHwReg2->AREG_SETREG1	= 0x00000005;	/* ASIC Write Enable								*/
	pAsicHwReg2->AREG_CSTP	&= ~(1<<14);		/* clock enable	(for Position compensate )			*/
	pAsicHwReg2->AREG_CSTP	&= ~(1<<25);		/* clock enable	(for Position compensate )			*/
	pAsicHwReg2->AREG_SETREG1	= 0x00000001;	/* ASIC Write Disable								*/
#endif/* (CSW_SVCF_FULC_MODE == CSW_FULC_BASIC_SUPPORT)	*/

	/*----------------------------------------------------------------------------------------------*/
	/*		演算部(MicroProgram)ウォッチドッグ機能＆パルス変換Ｃ相マスクビットの初期化				*/
	/*----------------------------------------------------------------------------------------------*/
//	pAsicHwReg->AREG_WDT1SET	= 0x00000150;	/* Set C-Phase Mask(bit8), WDT Stop(bit6),			*/
	pAsicHwReg->AREG_WDT1SET	= 0x00000168;	/* Set C-Phase Mask(bit8), WDT Stop(bit6),			@@TEST*/
												/*		and Micro WDT Time Over(0x10*16us=256us)	*/
	/*----------------------------------------------------------------------------------------------*/
	/*		ホスト(CPU)ウォッチドッグ機能の初期化													*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_WDT2SET	= 0x00000020;	/* Set CPU WDT Time Over(64us*0x20=2048us)			*/

	/*----------------------------------------------------------------------------------------------*/
	/*		アナログモニタ機能の初期化																*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_PWMU		= 0x00000000;	/* Set Zero for Initial Out							*/
	pAsicHwReg->AREG_PWMV		= 0x00000000;	/* Set Zero for Initial Out							*/


#if 0
/********************************************************************************************************************************
		DEBUG
********************************************************************************************************************************/ 
	/*----------------------------------------------------------------------------------------------*/
	/*		クロック機能の初期化(JL-076でソフトリセット前にクロック有効が必要)						*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg2->AREG_SETREG1	= 0x00000005;	/* ASIC Write Enable								*/
	pAsicHwReg2->AREG_CSTP		= 0x00000000;	/* ASIC Clock Stop Selection						*/
	pAsicHwReg2->AREG_SETREG1	= 0x00000001;	/* ASIC Write Disable								*/

	/*----------------------------------------------------------------------------------------------*/
	/*		ソフトリセット																			*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg2->AREG_SETREG1	= 0x00000009;	/* ASIC S/W Reset Enable							*/
	KlibWaitus( 1 );							/* Wait												*/
	pAsicHwReg2->AREG_SRESET		= 0x00000000;	/* ASIC Soft Reset									*/
	KlibWaitus( 1 );							/* Wait												*/
	pAsicHwReg2->AREG_SETREG1	= 0x00000001;	/* ASIC S/W Reset Disable							*/

	/*----------------------------------------------------------------------------------------------*/
	/*		クロック機能の初期化																	*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg2->AREG_SETREG1	= 0x00000005;	/* ASIC Write Enable								*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg2->AREG_CSTP		= 0x04028410;	/* ASIC Clock Stop Selection						*/
												/*	Encoder ADIF, NCT, Full Closed Cntr,			*/
												/*	New Incre Decoder, OC Detect Clock Stop			*/
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)
	pAsicHwReg2->AREG_CSTP		|= 0x00080000;	/*  M-II Slave I/F Clock Stop						*/
#elif (CSW_NETIF_TYPE == NETIF_M2_TYPE)
	pAsicHwReg2->AREG_CSTP		|= 0x00004000;	/*  Pulse Counter Clock Stop						*/
#else /* (CSW_NETIF_TYPE == NETIF_M3_TYPE || NETIF_CM_TYPE) */
	pAsicHwReg2->AREG_CSTP		|= 0x00084000;	/*  M-II Slave I/F, Pulse Counter Clock Stop		*/
#endif
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg2->AREG_SETREG1	= 0x00000001;	/* ASIC Write Disable								*/
/********************************************************************************************************************************
		DEBUG
********************************************************************************************************************************/
#endif

	return;
}



/****************************************************************************************************/
/*																									*/
/*		アラーム機能の初期化																		*/
/*																									*/
/****************************************************************************************************/
void	SHal_InitAsicAlarm( ASIC_HW_IF_STR *pAsicHwReg )
{
	/*----------------------------------------------------------------------------------------------*/
	/*		アラーム設定																			*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_ALMSET		= 0x00000000;	/* HWGBON:Invalid, DBONL:DBON						*/

	/*----------------------------------------------------------------------------------------------*/
	/*		極性＆フィルタ設定 : OV, OC, VCAP, BBON1, BBON2											*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SETREG1	= 0x00000005;	/* ASIC Write Enable								*/
//	pAsicHwReg->AREG_SETREG2	= 0x000038F0;	/* OV, OC : ActiveLow, Filter valid					*/
												/*	BBON1-2 : ActiveHigh, Filter invalid			*/
												/*	Ondelay Counter Clock : 320MHz					*/
												/*	INT1L Recovery Time : 640ns						*/
//	pAsicHwReg->AREG_SETREG2	= 0x000021F0;	/* Ondelay Counter Clock : 26.7MHz					*//* @@暫定 */
//	pAsicHwReg->AREG_SETREG2	= 0x000029F0;	/* Ondelay Counter Clock : 80MHz					*//* @@暫定,設定値間違い修正<S017> */
	pAsicHwReg->AREG_SETREG2	= 0x00002950;	/* Ondelay Counter Clock : 80MHz					*//* @@暫定,ノイズ対応にBBONにフィルタ追加 *//*<S141>*/
	pAsicHwReg->AREG_SETREG2	|= BIT9;		/* Carrer Counter Sync				<S019>				*/
	pAsicHwReg->AREG_SETREG1	= 0x00000001;	/* ASIC Write Disable								*/

	/*----------------------------------------------------------------------------------------------*/
	/*		アラームマスク設定 : OV入力, OC入力, OC2入力, OC検出									*/
	/*----------------------------------------------------------------------------------------------*/
	//	pAsicHwReg->AREG_SETMSK	= 0x0000000F;	/* All Alarm Mask									*/
#if CSW_BOARD_TYPE == SGD7S_TYPE /* <S0A2> */
	pAsicHwReg->AREG_SETMSK		= 0x0000000D;	/* OV, OC2, OC_UVW Mask								*//* <S01B> */
#else /* CSW_BOARD_TYPE == SGD7W_TYPE *//* <S0A2> */
	pAsicHwReg->AREG_SETMSK		= 0x00000009;	/* OV, OC_UVW Mask									*//* <S0A2> */
#endif /* CSW_BOARD_TYPE == SGD7S_TYPE *//* <S0A2> */
	/*----------------------------------------------------------------------------------------------*/
	/*		アラームクリア																			*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_ALMCLR		= 0x0000FFFF;	/* Clear All Alarm									*/

	return;
}



/****************************************************************************************************/
/*																									*/
/*		割込み機能の初期化																			*/
/*																									*/
/****************************************************************************************************/
void	SHal_InitAsicInterrupt( ASIC_HW_IF_STR *pAsicHwReg )
{
	/*----------------------------------------------------------------------------------------------*/
	/*		割込イネーブルレジスタ設定																*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_INT1SET	= 0x00000000;	/* Disable All Interrupt							*/

	/*----------------------------------------------------------------------------------------------*/
	/*		割込用タイマレジスタ設定																*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_INT0SET	= 0x00000000;	/* INT0 Frequency									*/

	/*----------------------------------------------------------------------------------------------*/
	/*		各割り込み遅延時間設定																	*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_ENCTXDLY0	= 0x00000000;	/* CH0 Tx Delay : 0ns								*/
	pAsicHwReg->AREG_ENCTXDLY1	= 0x00000000;	/* CH1 Tx Delay : 0ns								*/
	/*----------------------------------------------------------------------------------------------*/
	/*		エンコーダリード割込み遅延時間設定														*/
	/*		初期値には0を格納しておき、エンコーダの通信周期(8Mbps/4Mbps)に従って再設定する			*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_ENCRDDLY0	= 0x00000000;	/* CH0 Read Interrupt Delay : 0ns <S009><S09C>		*/
	pAsicHwReg->AREG_ENCRDDLY1	= 0x00000000;	/* CH1 Read Interrupt Delay : 0ns					*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_INTASET	= 0x00000078;	/* ScanA Interrupt Delay : 1500ns					*/
												/* (jitter & Position Compensation Delay)			*/
	pAsicHwReg->AREG_INTBSET	= 0x000000C8;	/* ScanB Interrupt Delay : 2500ns (ScanA + 1.0us)	*/
	pAsicHwReg->AREG_INTCSET	= 0x00000118;	/* ScanC Interrupt Delay : 3500ns (ScanB + 1.0us)	*/
	pAsicHwReg->AREG_INTNSET	= 0x00000168;	/* ScanN Interrupt Delay : 4500ns (ScanC + 1.0us)	*/
												/* ScanN is non-use									*/
	/*----------------------------------------------------------------------------------------------*/
	/*		各割り込み周期設定																		*/
	/*----------------------------------------------------------------------------------------------*/
//	pAsicHwReg->AREG_INTCYCLE	= 0x00011f10;	/* ScanA = Encoder Communication Cycle		62.5us	*/
												/* ScanB = Encoder Communication Cycle * 2	125us	*/
												/* ScanC = Encoder Communication Cycle * 32	2ms		*/
												/* ScanN = Encoder Communication Cycle * 2	125us	*/
	pAsicHwReg->AREG_INTCYCLE	= 0x00000000;	/* 割り込み起動後に再設定　					<S1D3>	*/

	/*----------------------------------------------------------------------------------------------*/
	/*		μプログラムエンコーダ処理割り込み設定													*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_INTMPESET	= 0x00000000;	/* ScanA完了後に処理したいため使用しない			*/

	/*----------------------------------------------------------------------------------------------*/
	/*		外部への割り込み信号出力選択															*/
	/*----------------------------------------------------------------------------------------------*/
#if (CSW_NETIF_TYPE == NETIF_M3_TYPE)
	pAsicHwReg->AREG_INTSET		= 0x00000005;	/* MECHATROLINK-3									*/
#else
	pAsicHwReg->AREG_INTSET		= 0x00000000;	/* MECHATROLINK-2 or Non-use						*/
#endif
	return;
}



/****************************************************************************************************/
/*																									*/
/*		シリアルエンコーダＩＦ初期化																*/
/*																									*/
/****************************************************************************************************/
void	SHal_InitAsicSencIF( ASIC_HW_IF_STR *pAsicHwReg )
{
	/*----------------------------------------------------------------------------------------------*/
	/*		シリアルエンコーダ選択																	*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_PFNCSET	&= 0xFFFFFF80;	/* 端子機能設定 : SESEL0:S-PG0, SESEL1:S-PG1		*/

	/*----------------------------------------------------------------------------------------------*/
	/*		シリアルエンコーダＩＦレジスタ初期化													*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SEPGSET00	= 0x00000006;	/* CH0,1共通 CrossTalk Avoidance ON, Delay Sync		*/
	pAsicHwReg->AREG_SEPGSET10	= 0x00000006;	/* CH0,1共通 ManchesterCode Squelch ON				*/
#if 0/* 通信開始時に同期要求を行う。初期化では、ディセーブルとする。	*/
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)
	/* Nothing Additional Establishment */
#elif (CSW_NETIF_TYPE == NETIF_M2_TYPE)
	pAsicHwReg->AREG_SEPGSET00	|= 0x00000020;	/*  Encoder CH0 Sync with M-II Network				*/
	pAsicHwReg->AREG_SEPGSET10	|= 0x00000020;	/*  Encoder CH1 Sync with M-II Network				*/
#elif (CSW_NETIF_TYPE == NETIF_M3_TYPE)
	pAsicHwReg->AREG_SEPGSET00	|= 0x00000080;	/*  Encoder CH0 Sync with M-III Network				*/
	pAsicHwReg->AREG_SEPGSET10	|= 0x00000080;	/*  Encoder CH1 Sync with M-III Network				*/
#else /* (CSW_NETIF_TYPE == NETIF_CM_TYPE) */
	pAsicHwReg->AREG_SEPGSET00	|= 0x00000001;	/*  Encoder CH0 Sync with SYNCL Signal				*/
	pAsicHwReg->AREG_SEPGSET10	|= 0x00000001;	/*  Encoder CH1 Sync with SYNCL Signal				*/
#endif
#endif
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SEPGSET01	= 0x00000001;	/* CH0,1共通 Slave Address : 01h					*/
	pAsicHwReg->AREG_SEPGSET11	= 0x00000001;	/* CH0,1共通 16Byte P0 Master Mode					*/

	/*----------------------------------------------------------------------------------------------*/
	/*		遅延同期時間設定																		*/
	/*----------------------------------------------------------------------------------------------*/
//	pAsicHwReg->AREG_SEDLLD		= 0x00000000;	/* Delay Sync Time : (n + 1) * 125 = 125[ns]
	pAsicHwReg->AREG_SEDLLD		= 0x0000001F;	/* Delay Sync Time : (0x1F + 1) * 125 = 4000[ns]	*/

	/*----------------------------------------------------------------------------------------------*/
	/*		シリアルエンコーダ送信データレジスタ初期化												*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SE0_TXD01	= 0x00000000;	/* CH0 Transmit Register							*/
	pAsicHwReg->AREG_SE0_TXD23	= 0x00000000;	/* CH0 Transmit Register							*/
	pAsicHwReg->AREG_SE0_TXD45	= 0x00000000;	/* CH0 Transmit Register							*/
	pAsicHwReg->AREG_SE0_TXD67	= 0x00000000;	/* CH0 Transmit Register							*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SE1_TXD01	= 0x00000000;	/* CH1 Transmit Register							*/
	pAsicHwReg->AREG_SE1_TXD23	= 0x00000000;	/* CH1 Transmit Register							*/
	pAsicHwReg->AREG_SE1_TXD45	= 0x00000000;	/* CH1 Transmit Register							*/
	pAsicHwReg->AREG_SE1_TXD67	= 0x00000000;	/* CH1 Transmit Register							*/

	return;
}



/****************************************************************************************************/
/*																									*/
/*		シリアルＩＦ初期化																			*/
/*																									*/
/****************************************************************************************************/
void	SHal_InitAsicSerialIF( ASIC_HW_IF_STR *pAsicHwReg )
{
	/*----------------------------------------------------------------------------------------------*/
	/*		汎用同期シリアルＩＦの初期化															*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_IOSTAT	= KPX_AREG_IOSTAT_INIOUT;	/* Set Initial Output Data to High Word	*/

	/*----------------------------------------------------------------------------------------------*/
	/*		非同期シリアルＩＦの初期化																*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_UARTSTS	= 0x00000000;	/* UART Status Register								*/

	return;
}



/****************************************************************************************************/
/*																									*/
/*		パルスＩＦ初期化																			*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		ASIC 指令カウンタ設定値テーブル																*/
/*--------------------------------------------------------------------------------------------------*/
const	USHORT	AsicPocsetTable[7] =
{
	0x0650,		/* 0 : ~A,~B, 101=Sign+Pulse		*/
	0x0640,		/* 1 : ~A,~B, 100=CW+CCW			*/
	0x0630,		/* 2 : ~A,~B, 011=(A+B)*1			*/
	0x0610,		/* 3 : ~A,~B, 001=(A+B)*2			*/
	0x0600,		/* 4 : ~A,~B, 000=(A+B)*4			*/
	0x0050,		/* 5 :  A, B, 101=Sign+Pulse		*/
	0x0040,		/* 6 :  A, B, 100=CW+CCW			*/
};
/*--------------------------------------------------------------------------------------------------*/
LONG	SHal_InitAsicPulseIF( ASIC_HW_IF_STR *pAsicHwReg, USHORT prstsw, USHORT syssw1, USHORT comfil )
{
	LONG	i;
	USHORT	uswk;
	LONG	ret;
	ret = FALSE;

/*--------------------------------------------------------------------------------------------------*/
/*		指令カウンタ設定																			*/
/*--------------------------------------------------------------------------------------------------*/
	i = prstsw & 0x0F;									/* 指令パルス形態							*/
	if( i > 6 )											/* パラメータ設定範囲チェック				*/
	{
		uswk = 0;
		ret = TRUE;										/* パラメータ設定エラー						*/
	}
	else
	{
		uswk = AsicPocsetTable[i];						/* カウントモード,etc						*/
	}
	/*----------------------------------------------------------------------------------------------*/
	if( ((prstsw >> 4) & 0x0F) < 2 )
	{
		uswk = ( uswk | 0x0800 );						/* CC極性設定								*/
	}

/*--------------------------------------------------------------------------------------------------*/
/*		CA,CB フィルタ設定																			*/
/*--------------------------------------------------------------------------------------------------*/
	switch( (prstsw >> 12) & 0x0F )
	{
	case 0x00:											/* ラインドライバ信号用指令入力フィルタ1	*/
		uswk = (uswk | ((syssw1 >> 4) & 0x3) );			/* (PnE0B.4-5)にて設定						*/
		break;
	case 0x01:											/* オープンコレクタ信号用指令入力フィルタ	*/
		uswk = (uswk | ((comfil) & 0x3) );				/* (PnE0E.0-1)にて設定						*/
		break;
	case 0x02:											/* ラインドライバ信号用指令入力フィルタ2	*/
		uswk = (uswk | ((comfil >> 8) & 0x3) );			/* (PnE0E.8-9)にて設定						*/
		break;
	default:
		uswk = (uswk | ((syssw1 >> 4) & 0x3) );			/* (PnE0B.4-5)にて設定						*/
		break;
	}
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_POCSET = uswk;						/* 指令カウンタ設定							*/
	uswk = ( uswk | 0x0100 );							/* CLMラッチイネーブル設定					*/
	pAsicHwReg->AREG_POCSET = uswk;						/* 指令カウンタ設定							*/
	pAsicHwReg->AREG_LODSET = LODSET_SET_LDCMD;			/* カウンタロード設定						*/

/*--------------------------------------------------------------------------------------------------*/
/*		指令フィルタ設定																			*/
/*--------------------------------------------------------------------------------------------------*/
	switch( (prstsw >> 12) & 0x0F )
	{
	case 0x00:											/* ラインドライバ信号用指令入力フィルタ1	*/
		pAsicHwReg->AREG_FCCSET = syssw1 & 0x0F00;		/* (PnE0B.8-11)にて設定						*/
		break;
	case 0x01:											/* オープンコレクタ信号用指令入力フィルタ	*/
		pAsicHwReg->AREG_FCCSET = (comfil<<4) & 0x0F00;	/* (PnE0E.4-7)にて設定						*/
		break;
	case 0x02:											/* ラインドライバ信号用指令入力フィルタ2	*/
		pAsicHwReg->AREG_FCCSET = (comfil>>4) & 0x0F00;	/* (PnE0E.12-15)にて設定					*/
		break;
	default:
		pAsicHwReg->AREG_FCCSET = syssw1 & 0x0F00;		/* (PnE0B.8-11)にて設定						*/
		break;
	}

#if 0 /* 2012.11.01 Y.Oka (>_<) 分周出力現状未対応 */
※分周設定は別関数に分けること
/*--------------------------------------------------------------------------------------------------*/
/*		シリアルエンコーダパルス変換設定(Bit数,変換周期)											*/
/*--------------------------------------------------------------------------------------------------*/
		if( Kprm.f.FencUse == TRUE )					/* フルクエンコーダ分周出力					*/
		{
			ASIC_WR( AREG_PLSET, 0x3400 );				/* 変換Bit数:16bit, 変換周期:64us			*//* <V501> */
		}
		else if( MencP.f.PlsTrn13bit == TRUE )			/* パルス変換13bit							*/
		{
			ASIC_WR( AREG_PLSET, 0x3100 );				/* 変換Bit数:13bit, 変換周期:64us			*/
		}
		else
		{
			ASIC_WR( AREG_PLSET, 0x3400 );				/* 変換Bit数:16bit, 変換周期:64us			*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		シリアルエンコーダパルス変換設定(位置,原点,etc)												*/
/*--------------------------------------------------------------------------------------------------*/
		KPI_PCMASK_ON( );								/* パルス変換Ｃ相マスク(念のため)			*/
		ASIC_WR( AREG_DIVSET, 0x0102 );					/* パルス変換書込み側設定:HostCPU			*//* <JL076_TEST> */
		ASIC_WR( AREG_POSET0, 0xFFFF );					/* パルス変換位置設定						*/
		ASIC_WR( AREG_POSET0H, 0x0000 );				/* パルス変換位置設定bit16-23				*/
		ASIC_WR( AREG_POTEST, 0xFFFF );					/* パルス変換カウンタ設定					*/
		ASIC_WR( AREG_POSET1, 0x0000 );					/* パルス変換原点補正１						*/
		ASIC_WR( AREG_POSET2, 0xFFFF );					/* パルス変換原点補正２						*/

/*--------------------------------------------------------------------------------------------------*/
/*		分周入力フィルタ設定																		*/
/*--------------------------------------------------------------------------------------------------*/
		ASIC_WR( AREG_FBCSET, 0x0005 );					/* 分周入力フィルタ設定						*/

/*--------------------------------------------------------------------------------------------------*/
/*		分周パルス計算＆ＡＳＩＣ設定																*/
/*--------------------------------------------------------------------------------------------------*/
		if( MencP.f.DivOut == TRUE )
		{
			KpxSetMencDividedPulse( );					/* モータエンコーダ分周パルス計算＆設定		*/
		}
		else if( FencP.f.DivOut == TRUE )
		{
			KpxSetFencDividedPulse( );					/* フルクエンコーダ分周パルス計算＆設定		*/
		}
#endif
/*--------------------------------------------------------------------------------------------------*/
	return ret;
}



/****************************************************************************************************/
/*																									*/
/*		A/Dインターフェイス初期化																	*/
/*																									*/
/****************************************************************************************************/
void	SHal_InitAsicAdInterface( ASIC_HW_IF_STR *pAsicHwReg )
{
/*--------------------------------------------------------------------------------------------------*/
/*		共通A/D設定(指令A/D & 電流検出A/D)															*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_ADSET1		= 0x00001043;	/* Command AD 16Bit AD I/F							*/
												/* Current AD 10Bit Decimation Filter				*/
												/* Current AD Sync Invalid							*/

/*--------------------------------------------------------------------------------------------------*/
/*		指令A/Dインターフェイス初期化																*/
/*--------------------------------------------------------------------------------------------------*/
//	pAsicHwReg->AREG_ADSET2		= 0x0000C0CE;	/* AD Clock, AD Frequency, Base Frequency			*/
	pAsicHwReg->AREG_ADSET2		= 0x0000D4CE;	/* AD Clock, AD Frequency, Base Frequency			*/
//	pAsicHwReg->AREG_ADSET3		= 0x00008280;	/* AD Start Timing, AD I/F Cycle					*/
	pAsicHwReg->AREG_ADSET3		= 0x0000A7A0;	/* AD Start Timing, AD I/F Cycle					*/
//	pAsicHwReg->AREG_ADSET4		= 0x0000DADA;	/* AD Latch Timing, AD Latch Timing					*/
	pAsicHwReg->AREG_ADSET4		= 0x0000FEFE;	/* AD Latch Timing, AD Latch Timing					*/
//	pAsicHwReg->AREG_ADSET5		= 0x000080DB;	/* AD Control H to L Edge, L to H Edge				*/
	pAsicHwReg->AREG_ADSET5		= 0x0000A5A1;	/* AD Control H to L Edge, L to H Edge				*/
//	pAsicHwReg->AREG_ADSET6		= 0x000080DB;	/* AD Control H to L Edge, L to H Edge				*/
	pAsicHwReg->AREG_ADSET6		= 0x0000A5A1;	/* AD Control H to L Edge, L to H Edge				*/

/*--------------------------------------------------------------------------------------------------*/
/*		電流検出A/Dインターフェイス初期化			   												*/
/*--------------------------------------------------------------------------------------------------*/
	/* Mode2 */									/* 電流制御周期 = 15.625us(64kHz)					*/
	pAsicHwReg->AREG_ADFCNT1	= 0x000000FB;	/* 電流ADIF基本クロック周波数 = 3.2MHz				*/
	pAsicHwReg->AREG_ADFCNT2	= 0x00000038;	/* ADIF Cycle, (256 - 38h) / 3.2MHz = 62.5us		*/
	pAsicHwReg->AREG_ADFCNT3	= 0x00000000;	/* AD Latch Timing, AD Latch Timing					*/
	pAsicHwReg->AREG_ADFCNT4	= 0x00003839;	/* AD Control H to L Edge, L to H Edge				*/
	pAsicHwReg->AREG_ADFCNT5	= 0x00000000;	/* AD Control H to L Edge, L to H Edge				*/

/*--------------------------------------------------------------------------------------------------*/
/*		A/Dデシメーションフィルタ初期化(電流検出A/D & 電圧検出A/D)									*/
/*--------------------------------------------------------------------------------------------------*/
//	pAsicHwReg->AREG_SDMCONF1	= 0x00001864;	/* CH1,2,3 Mode2, 電流検出分解能 = 12.8bit			*/
												/* CH1固定でA/D変換完了割り込み				*/
if( CSW_DECIMATION == 0x0000 )	/* <S202> */
{
	pAsicHwReg->AREG_SDMCONF1	= 0x0000187B;	/* CH1,2 Mode3, 電流検出分解能 = 10.9bit				*//* <S1F2> */
}												/* CH3 Mode1, 電圧検出分解能= 13.5bit				*//* <S1F2> */
else
{
	pAsicHwReg->AREG_SDMCONF1	= 0x0000185B;	/* CH1,2,3 Mode3, 電流検出分解能 = 10.9bit			*//* @@ */
}												/* CH1固定でA/D変換完了割り込み						*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SDMCONF2	= 0x00003A47;	/* プリトリガ計算:1/8回, 並列個数:4					*/
												/* CH1,2:20MHz Clock Out, CH3 External Clock In		*/
												/* Over range Detect:Mask							*/
/*--------------------------------------------------------------------------------------------------*/
if( CSW_DECIMATION == 0x0000 )	/* <S202> */
{
	pAsicHwReg->AREG_SDMCONF3	= 0x00001F06;	/* 電圧検出周期 = 250us(4kHz)							*//* <S1F2> */
}												/* 電流ADIF基本クロック周波数 = 1.6MHz					*/
												/* ADIF Cycle, (256-06h)*(16-Fh)/16 = 250us			*//* <S1F2> */
else
{
	pAsicHwReg->AREG_SDMCONF3	= 0x00001638;	/* 電圧検出周期 = 125us(8kHz)							*/
}
												/* MSB反転あり										*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SDMCONF4	= 0x00003839;	/* AD Control H to L Edge, L to H Edge				*/

/*--------------------------------------------------------------------------------------------------*/
/*		OC検出デシメーションフィルタ初期化	OC検出 内容未検討 2013.07.12 Y.Oka (>_<)				*/
/*--------------------------------------------------------------------------------------------------*/
//	pAsicHwReg->AREG_OCDCONF1	= 0x0000000F;	/* CH1,2 Mode3 電流検出分解能 = 11.9bit				*/	/* <S1F7> */
//	pAsicHwReg->AREG_OCSDMCONF3	= 0x00000000;	/* @@												*/	/* <S1F7> */
//	pAsicHwReg->AREG_OCSDMCONF4	= 0x00000000;	/* @@												*/	/* <S1F7> */

/*--------------------------------------------------------------------------------------------------*/
/*		OC検出レベル設定																			*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_OCLVUP		= 0x0000FFFF;	/* @@U相上限OC検出レベル							*/
	pAsicHwReg->AREG_OCLVUM		= 0x0000FFFF;	/* @@U相上限OC検出レベル							*/
	pAsicHwReg->AREG_OCLVVP		= 0x0000FFFF;	/* @@U相上限OC検出レベル							*/
	pAsicHwReg->AREG_OCLVVM		= 0x0000FFFF;	/* @@U相上限OC検出レベル							*/
	pAsicHwReg->AREG_OCLVWP		= 0x0000FFFF;	/* @@U相上限OC検出レベル							*/
	pAsicHwReg->AREG_OCLVWM		= 0x0000FFFF;	/* @@U相上限OC検出レベル							*/


	return;
}



/****************************************************************************************************/
/*																									*/
/*		タイマ機能の初期化																			*/
/*																									*/
/****************************************************************************************************/
void	SHal_InitAsicTimer( USHORT syssw2, ASIC_HW_IF_STR *pAsicHwReg )
{
/*--------------------------------------------------------------------------------------------------*/
/*		DBパルス設定：DB方式 = サイリスタ															*/
/*--------------------------------------------------------------------------------------------------*/
	if( syssw2 & 0x0010 )						/* PnE0C.5:4 = x:1 : サイリスタ						*/
	{											/* LowByte:Frequency,  HighByte:HighPulseWidth		*/
		pAsicHwReg->AREG_TIM1SET = 0x0000EB9C;	/* TB=(256 - 156)[us], TW=(255 - 235)[us]			*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		DBパルス設定：DB方式 = リレー																*/
/*--------------------------------------------------------------------------------------------------*/
	else										/* PnE0C.5:4 = x:0 : リレー							*/
	{											/* LowByte:Frequency,  HighByte:HighPulseWidth		*/
		pAsicHwReg->AREG_TIM1SET = 0x0000009C;	/* TB=(256 - 156)[us], TW=(255 - 0)[us]				*/
	}

	return;
}



/****************************************************************************************************/
/*																									*/
/*		回生機能の初期化																			*/
/*																									*/
/****************************************************************************************************/
void	SHal_InitAsicRegenerate( ASIC_HW_IF_STR *pAsicHwReg )
{
/*--------------------------------------------------------------------------------------------------*/
/*		回生設定																					*/
/*--------------------------------------------------------------------------------------------------*/
/*		Bit13 :	REGOFF	: 0:回生制動する, 1:回生制動しない											*/
/*		Bit14 :	DISCH	: 0:通常動作,     1:急速放電開始											*/
/*		Bit15 : GFTON   : 0:通常動作,     1:地絡テスト(BB発生時有効)								*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SETREG3	= 0x00000000;	/* REGOFF=0, DISCH=0, GFTON=0						*/

	return;
}



/****************************************************************************************************/
/*																									*/
/*		PWM制御機能の初期化																			*/
/*																									*/
/****************************************************************************************************/
void	SHal_InitAsicPwmControl( ASIC_HW_IF_STR *pAsicHwReg, LONG odt_c )
{
	ULONG	OnDelay;

/*--------------------------------------------------------------------------------------------------*/
/*		オンディレイ時間設定																		*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SETREG1	= 0x00000005;	/* ASIC Write Enable								*/
//	pAsicHwReg->AREG_SETREG2	&= (~BIT12);	/* Clock Select : 80MHz								*/
//	pAsicHwReg->AREG_SETREG2	|= (BIT11);		/* Clock Select : 80MHz								*/
/* for debug (INGRAMでは必要であった) */
//	pAsicHwReg->AREG_SETREG2		|= BIT9;	/* Carrer Counter Sync								*/
/* for debug (INGRAMでは必要であった) */
	/*----------------------------------------------------------------------------------------------*/
	OnDelay = (ULONG)((ASIC_CLKMHZ * odt_c) / 10);	/* 320[MHz] * Td[0.1us] / 10					*/
	pAsicHwReg->AREG_DYSETUP	= OnDelay;		/* UP1 : U-Phase,Positive-Side						*/
	pAsicHwReg->AREG_DYSETUN	= OnDelay;		/* UN1 : U-Phase,Negative-Side						*/
	pAsicHwReg->AREG_DYSETVP	= OnDelay;		/* VP1 : V-Phase,Positive-Side						*/
	pAsicHwReg->AREG_DYSETVN	= OnDelay;		/* VN1 : V-Phase,Negative-Side						*/
	pAsicHwReg->AREG_DYSETWP	= OnDelay;		/* WP1 : W-Phase,Positive-Side						*/
	pAsicHwReg->AREG_DYSETWN	= OnDelay;		/* WN1 : W-Phase,Negative-Side						*/
	pAsicHwReg->AREG_SETREG1	= 0x00000001;	/* ASIC Write Disable								*/

	return;
}

/****************************************************************************************************/
/*																									*/
/*		エンコーダリード割込みch0遅延時間の初期化													*/
/*																									*/
/****************************************************************************************************//*<S09C>*/
void SHal_InitEncRdDly_ch0( ASIC_HW_IF_STR *pAsicHwReg, USHORT PgBaudRate )
{
	LONG delay;

	/* エンコーダリード割込みch0遅延時間設定 */
	if( PgBaudRate == SENC_COMRATE_8MBPS )
	{
		delay = ENCRDDLY_8MBPS;
	}
	else
	{
		delay = ENCRDDLY_4MBPS;
	}
	/* エンコーダリード割込みch0遅延時間設定 */
	pAsicHwReg->AREG_ENCRDDLY0	= delay;	/* CH0 Read Interrupt Delay */

}

/****************************************************************************************************/
/*																									*/
/*		エンコーダリード割込みch1遅延時間の初期化													*/
/*																									*/
/****************************************************************************************************//*<S09C>*/
void SHal_InitEncRdDly_ch1( ASIC_HW_IF_STR *pAsicHwReg, USHORT PgBaudRate )
{
	LONG delay;

	/* エンコーダリード割込みch1遅延時間設定 */
	if( PgBaudRate == SENC_COMRATE_8MBPS )
	{
		delay	= ENCRDDLY_8MBPS;
	}
	else
	{
		delay	= ENCRDDLY_4MBPS;
	}
	pAsicHwReg->AREG_ENCRDDLY1	= delay;	/* CH1 Read Interrupt Delay */
}

/****************************************************************************************************/
/*																									*/
/*		エンコーダリード割込み遅延時間の初期化														*/
/*																									*/
/****************************************************************************************************//*<S09C>*/
//void SHal_InitEncRdDly( ASIC_HW_IF_STR *pAsicHwReg, USHORT PgBaudRate ) /* <S0E4> */
void SHal_InitEncRdDly( ASIC_HW_IF_STR *pAsicHwReg, USHORT PgBaudRate, UINT ch ) /* <S0E4> */
{
//	if(pAsicHwReg->AREG_ENCRDDLY0 == 0)	/* SGD7Sのとき、およびSGD7W 0軸目のとき						*//* <S0E4> */
	if(ch == 0)	/* SGD7Sのとき、およびSGD7W 0軸目のとき *//* <S0E4> */
	{
		/* エンコーダリード割込みch0遅延時間設定 */
		SHal_InitEncRdDly_ch0( pAsicHwReg, PgBaudRate );
	}
	else								/* SGD7W 1軸目のとき										*/
	{
		/* エンコーダリード割込みch1遅延時間設定 */
		SHal_InitEncRdDly_ch1( pAsicHwReg, PgBaudRate );
	}
}
/****************************************************************************************************/
/*																									*/
/*		エンコーダ位置補正機能の初期化																*/
/*																									*/
/****************************************************************************************************//* <S004> */
void SHal_InitEncPosComp( ASIC_HW_IF_STR *pAsicHwReg, SVIP_INIT_PCOMP *SvipInitPcomp )/*<S09C>変数名変更*/
{
	UCHAR BitNo;
	UCHAR Dpoint;
	LONG PosSftR;
	LONG PosSftX;
	LONG PosSign;
	LONG PosLeng;

	BitNo  = SvipInitPcomp->bit_dp.b.l;
	Dpoint = SvipInitPcomp->bit_dp.b.h;

	if( SvipInitPcomp->AxisMotType  == MOTTYPE_ROTARY)
	{	/* 回転形の場合 */
		PosLeng = 0;
		PosSftR = 24 - BitNo;
		PosSftX = 32 - BitNo - (80 - Dpoint);	/* JL-086の仕様書と異なるので注意(仕様書のミス)	*//* <S210>:共通から回転形のみに */
	}
	else
	{	/* リニアの場合 */
		PosLeng = HPOS_TO_XPOS;
		PosSftR = 24 - PosLeng;
		PosSftX = LMX_MPOS_SFT;					/* <S210> */
	}
	PosSign = (SvipInitPcomp->RevAsmMotor == TRUE) ? 1 : 0;

	/* エンコーダ補正設定およびアラーム：EncFlag(0390h)	*/
	pAsicHwReg->AREG_ENCFLAG = ( ENCINT << TRGSEL )						/* 起動方法設定 			*/
							 + ( SvipInitPcomp->AxisMotType<<MOT_AMPTYPE )/* 回転型/リニア型		*/
							 + ( POSDATSEL_11 << POSDAT_SEL );			/* 位置補正イネーブル設定	*/

	/* シフト量設定：PGDCAL(0394h)					*/
	pAsicHwReg->AREG_PGDCAL = ( PosSftX << POS_SFTX )					/*　位置データシフト量1		*/
							+ ( PosSftR << POS_SFTR )					/*　位置データシフト量2		*/
							+ ( PosLeng << POS_LENG )					/*　位置データ長			*/
							+ ( PosSign << POS_SIGN );					/*　位置データ符号			*/

	/* 1回転エンコーダパルス数設定:PulseNo(0398h)	*/
	pAsicHwReg->AREG_PULSENO = SvipInitPcomp->PulseNo;

	/* 加速度エラーレベル:AccErrLvl(03A0h)			*/
	pAsicHwReg->AREG_ACCERRLVL = SvipInitPcomp->AccErrLvl;

	/* 累計エラーレベル:SPGFAIL(03A8h)	 			*/
	pAsicHwReg->AREG_SPGFAIL = SvipInitPcomp->spgfail.b.l;

	/* ラッチ周期中クロック数:LchClkNum(03ACh) 		*/
	pAsicHwReg->AREG_LCHCLKNUM = ENCLCHCLKNUM;

	/* 演算周期中クロック数:SaclkNum(03B0h) 		*/
	pAsicHwReg->AREG_SACLKNUM = SAENCCLKNUM;

	/* リセット設定:RSTSET(03CCh)	 				*/
	pAsicHwReg->AREG_RSTSET = ( 1 << ALMCLR0 )					/* ComAlarmクリア					*/
							+ ( 1 << ALMCLR1 )					/* AccAlarmクリア					*/
							+ ( 1 << ACENCLR )					/* 加速度チェックイネーブルクリア	*/
							+ ( 1 << SRECLR )					/* 受信エラーフラグクリア			*/
							+ ( 1 << SRCCLR );					/* 受信完了フラグクリア				*/
}

/****************************************************************************************************/
/*																									*/
/*		エンコーダ位置補正機能(加速度チェックレベル)の初期化										*/
/*																									*/
/* フィードバックオプション関連は、Ch1を使用する。													*/
/* Ch1で位置補正モジュールを使用する場合は、1軸目(+0x1000)の領域に設定する必要がある。				*/
/****************************************************************************************************//* <S004> */
void SHal_InitEncPosCompAccLvl( ASIC_HW_IF_STR *pAsicHwReg )
{
	/* 初期値0のためエンコーダに接続する前に暫定値を設定しておく。	*/
	/* 加速度エラーを検出してしまうため							*/

	/* 加速度エラーレベル:AccErrLvl(03A0h)			*/
	pAsicHwReg->AREG_ACCERRLVL = 0x007FFFFF;

}
/****************************************************************************************************/
/*																									*/
/*		タイマラッチ機能の初期化																	*/
/*																									*/
/****************************************************************************************************//* <S004> */
BOOL SHalDrv_InitLatchSignal(ASIC_HW_IF_STR *AsicHwReg, SHAL_LATCH_STS_ADR* LtStsAdr,
												USHORT PrmInselA, BOOL* ExtLtEnable)
{
LONG	i;
BOOL	rc;
UCHAR	prmSet[3];
UCHAR	prmChek[3];

	/* ローカル変数設定	*/
	prmSet[0] = (PrmInselA >>  4) & 0x0F;
	prmSet[1] = (PrmInselA >>  8) & 0x0F;
	prmSet[2] = (PrmInselA >> 12) & 0x0F;
	rc = TRUE;

	/* 外部ラッチ入力信号選択	*/
	for( i=0; i < 3; i++ )
	{
		switch( prmSet[i] )
		{
		case KPI_EXT1_LOWLEVEL:		/* <S1D6> SI4, LowLevel			*/
					LtStsAdr[i].STimShift = 13;
					LtStsAdr[i].STimLtAdr = &AsicHwReg->AREG_STIMLT5;
					LtStsAdr[i].SFbLtAdr = &AsicHwReg->AREG_SFBLT1;
					ExtLtEnable[i] = TRUE;
					prmChek[i]=HW_SI4;
					break;
		case KPI_EXT2_LOWLEVEL:		/* <S1D6> SI5, LowLevel			*/
					LtStsAdr[i].STimShift = 14;
					LtStsAdr[i].STimLtAdr = &AsicHwReg->AREG_STIMLT6;
					LtStsAdr[i].SFbLtAdr = &AsicHwReg->AREG_SFBLT3;
					ExtLtEnable[i] = TRUE;
					prmChek[i]=HW_SI5;
					break;
		case KPI_EXT3_LOWLEVEL:		/* <S1D6> SI6, LowLevel			*/
					LtStsAdr[i].STimShift = 15;
					LtStsAdr[i].STimLtAdr = &AsicHwReg->AREG_STIMLT7;
					LtStsAdr[i].SFbLtAdr = &AsicHwReg->AREG_SFBLT5;
					ExtLtEnable[i] = TRUE;
					prmChek[i]=HW_SI6;
					break;
		case KPI_EXT1_HIGHLEVEL:	/* <S1D6> SI4, HighLevel		*/
					LtStsAdr[i].STimShift = 9;
					LtStsAdr[i].STimLtAdr = &AsicHwReg->AREG_STIMLT1;
					LtStsAdr[i].SFbLtAdr = &AsicHwReg->AREG_SFBLT0;
					ExtLtEnable[i] = TRUE;
					prmChek[i]=HW_SI4;
					break;
		case KPI_EXT2_HIGHLEVEL:	/* <S1D6> SI5, HighLevel		*/
					LtStsAdr[i].STimShift = 10;
					LtStsAdr[i].STimLtAdr = &AsicHwReg->AREG_STIMLT2;
					LtStsAdr[i].SFbLtAdr = &AsicHwReg->AREG_SFBLT2;
					ExtLtEnable[i] = TRUE;
					prmChek[i]=HW_SI5;
					break;
		case KPI_EXT3_HIGHLEVEL:	/* <S1D6> SI6, HighLevel		*/
					LtStsAdr[i].STimShift = 11;
					LtStsAdr[i].STimLtAdr = &AsicHwReg->AREG_STIMLT3;
					LtStsAdr[i].SFbLtAdr = &AsicHwReg->AREG_SFBLT4;
					ExtLtEnable[i] = TRUE;
					prmChek[i]=HW_SI6;
					break;
		default:	/* Not Select				*/
					LtStsAdr[i].STimShift = 0;
					LtStsAdr[i].STimLtAdr = 0;
					LtStsAdr[i].SFbLtAdr = 0;
					ExtLtEnable[i] = FALSE;
					prmChek[i]=HW_SI_NO_DEFINE;
					break;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		ラッチ選択整合性チェック																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( prmChek[0] == prmChek[1] )
	{
		if( prmChek[0] != 0 ){	rc = FALSE;}
	}
	else if( prmChek[0] == prmChek[2] )
	{
		if( prmChek[0] != 0 ){	rc = FALSE;}
	}
	else if( prmChek[1] == prmChek[2] )
	{
//		if( prmChek[0] != 0 ){	rc = FALSE;}
		if( prmChek[1] != 0 ){	rc = FALSE;}	/* <S226> */
	}
/*--------------------------------------------------------------------------------------------------*/
	return rc;

}

/****************************************************************************************************/
/*																									*/
/*		マイクロプログラムのロード処理																*/
/*																									*/
/****************************************************************************************************/
BOOL	SHal_LoadAsicMicroProgram( ASICS *SvAsicRegs, CULONG *src_adr, LONG u_size)
{
	LONG				i;
#ifndef _VC_DEBUG_ /* 2012.08.24 Y.Oka Make Mercury Base Software */
	VULONG				dummy;
#endif /* _VC_DEBUG_ */ /* 2012.08.24 Y.Oka Make Mercury Base Software */
	CULONG				*PrgSrc;							/* MicroProgram Src  Address			*/
	LONG				PrgSize;							/* MicroProgram Word Size				*/
	ASIC_HW_IF_STR		*pAsicHwReg;

	pAsicHwReg = SvAsicRegs->AsicHwReg;
/*--------------------------------------------------------------------------------------------------*/
/*		μプログラムの選択																			*/
/*--------------------------------------------------------------------------------------------------*/
	PrgSrc  = src_adr;										/* Set Src  Address						*/
	PrgSize = u_size;										/* Set Word Size						*/
	if( (PrgSize % 2) != 0 )								/* サイズチェック						*/
	{
		return TRUE;	/* A.BF2 : システムアラーム２(Micro異常) */
	}

/*--------------------------------------------------------------------------------------------------*/
/*		μプログラムの書込み許可																	*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SETREG1 = 0x00000005;					/* ASIC Write Enable					*/
	pAsicHwReg->MREG_CTSTR = MPCMD_PRGSTOP;					/* μプログラム実行停止					*/
	SvAsicRegs->MicroRunningSts = FALSE;					/* μプログラム実行停止(CPU管理)		*/
/*--------------------------------------------------------------------------------------------------*/
/*		μプログラムの転送処理																		*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->MREG_UPLDA	= 0;
	for( i=0; i<PrgSize; i++ )
	{

#ifndef _VC_DEBUG_ /* 2012.08.24 Y.Oka Make Mercury Base Software */

		/* 2010.05.21 Y.Oka Wait用 */
		dummy = pAsicHwReg->AREG_TSTT1CNT;

		/* ロードポインタチェック */
		if( ( pAsicHwReg->MREG_UPLDA ) != ((i%2)<<14)+(i/2) )
		{
			return TRUE;	/* A.BF2 : システムアラーム２(Micro異常) */
		}

		pAsicHwReg->MREG_UPLD	= PrgSrc[i];		/* μプログラム書き込み					*/

#else /* _VC_DEBUG_ */ /* 2012.08.24 Y.Oka Make Mercury Base Software */

		/* μプログラム書き込み */
		*(ULONG*)(&SvAsicRegs->pDummyMicroSram[i*2]) = *(ULONG*)(&PrgSrc[i]);

#endif /* _VC_DEBUG_ */ /* 2012.08.24 Y.Oka Make Mercury Base Software */

	}
/*--------------------------------------------------------------------------------------------------*/
/*		μプログラムのベリファイ処理																*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->MREG_UPLDA	= 0;
	for( i=0; i<PrgSize; i++ )
	{

#ifndef _VC_DEBUG_ /* 2012.08.24 Y.Oka Make Mercury Base Software */

		/* 2010.05.21 Y.Oka Wait用 */
		dummy = pAsicHwReg->AREG_TSTT1CNT;

		/* ロードポインタチェック */
		if( ( pAsicHwReg->MREG_UPLDA ) != ((i%2)<<14)+(i/2) )
		{
			return TRUE;	/* A.BF2 : システムアラーム２(Micro異常) */
		}

		/* μプログラム読み出し＆データチェック */
		if( ( pAsicHwReg->MREG_UPLD ) != PrgSrc[i] )
		{
			return TRUE;	/* A.BF2 : システムアラーム２(Micro異常) */
		}

#else /* _VC_DEBUG_ */ /* 2012.08.24 Y.Oka Make Mercury Base Software */

		/* μプログラム読み出し＆データチェック */
		if( SvAsicRegs->pDummyMicroSram[i] != PrgSrc[i] )
		{
			return TRUE;	/* A.BF2 : システムアラーム２(Micro異常) */
		}

#endif /* _VC_DEBUG_ */ /* 2012.08.24 Y.Oka Make Mercury Base Software */
	}

/*--------------------------------------------------------------------------------------------------*/
/*		ＡＳＩＣ書込み禁止																			*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SETREG1	= 0x00000001;				/* ASIC Write Disable					*/

	return FALSE;
}



/****************************************************************************************************/
/*																									*/
/*		マイクロプログラムレジスタのリセット														*/
/*																									*/
/****************************************************************************************************/
BOOL	SHal_ResetAsicMicroRegister( ASICS *SvAsicRegs )
{
	LONG				i;									/* Loop Index							*/
	VULONG				vulwk;								/* ULONG Work							*/
	VULONG				Dummy;								/* Dummy for Read						*/
	VULONG				*WkReg;								/* MicroProgram Work Register Address	*/
	ASIC_HW_IF_STR		*pAsicHwReg;

	WkReg = (VULONG*)(SvAsicRegs->AsicMicroReg);			/* MicroProgram Work Register Address	*/
	pAsicHwReg = SvAsicRegs->AsicHwReg;

/*--------------------------------------------------------------------------------------------------*/
/*		マイクロプログラム実行停止(念のため)														*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SETREG1	= 0x00000005;				/* ASIC Write Enable					*/
	pAsicHwReg->MREG_CTSTR		= MPCMD_PRGSTOP;			/* μプログラム実行停止					*/
	pAsicHwReg->AREG_SETREG1	= 0x00000001;				/* ASIC Write Disable					*/
	SvAsicRegs->MicroRunningSts = FALSE;					/* μプログラム実行停止(CPU管理)		*/

/*--------------------------------------------------------------------------------------------------*/
/*		ワークレジスタのチェック＆クリア															*/
/*--------------------------------------------------------------------------------------------------*/
	for( i=0; i<(ASIC_MREGSIZE/4); i++ )					/* Write to WkReg[i]					*/
	{
		WkReg[i]	= (ULONG)(i);							/* Write 0,1,2,3,4,5,...				*/
	}
	/*----------------------------------------------------------------------------------------------*/
	for( i=0; i<(ASIC_MREGSIZE/4); i++ )					/* Check WkReg[i]						*/
	{
		vulwk = WkReg[i];									/* Read WkReg[i]						*/
		if( vulwk != (i) )									/* Check Read Data						*/
		{
			return TRUE;	/* A.BF2 : システムアラーム２(Micro異常) */
		}
	}
/*--------------------------------------------------------------------------------------------------*/
	for( i=0; i<(ASIC_MREGSIZE/4); i++ )					/* Check and Clear WkReg[i]				*/
	{
		WkReg[i]	= 0x5A5A5A5A;							/* Write 0x5A5A5A5A to WkReg[i]			*/
		Dummy = pAsicHwReg->AREG_PWMU;						/* Dummy Read							*/
		vulwk = WkReg[i];									/* Read and Check						*/
		if( vulwk != 0x5A5A5A5A )							/* Check Read Data						*/
		{
			return TRUE;	/* A.BF2 : システムアラーム２(Micro異常) */
		}
	/*----------------------------------------------------------------------------------------------*/
		WkReg[i]	= 0xA5A5A5A5;							/* Write 0xA5A5A5A5 to WkReg[i]			*/
		Dummy = pAsicHwReg->AREG_PWMU;						/* Dummy Read							*/
		vulwk = WkReg[i];									/* Read and Check						*/
		if( vulwk != 0xA5A5A5A5 )							/* Check Read Data						*/
		{
			return TRUE;	/* A.BF2 : システムアラーム２(Micro異常) */
		}
	/*----------------------------------------------------------------------------------------------*/
		WkReg[i]	= 0x00000000;							/* Clear WkReg[i]						*/
		Dummy = pAsicHwReg->AREG_PWMU;						/* Dummy Read							*/
		vulwk = WkReg[i];									/* Read and Check						*/
		if( vulwk != 0x00000000 )							/* Check Read Data						*/
		{
			return TRUE;	/* A.BF2 : システムアラーム２(Micro異常) */
		}
	}
	return FALSE;
}

/****************************************************************************************************/
/*																									*/
/*		マイクロプログラムテーブルデータのロード処理					<NeCO>_130919				*/
/*																									*/
/****************************************************************************************************/
BOOL	SHal_LoadAsicMicroTable( USHORT *asic_adr, USHORT *src_adr, ULONG array_size)
{
	LONG				i;									/* Loop Index							*/
	
	for( i=0; i<array_size; i++ )							/* Write to AsicReg[i]					*/
	{
		asic_adr[i] = src_adr[i];
	}
	return FALSE;
}

// <S146>
#define JL086_MCAREA_S_TOP		(0xFFE70000)
#define	JL086UprgSz_SGD7_S		*(CULONG*)(JL086_MCAREA_S_TOP)
#define	YecAsicPrg_SGD7_S_Addr	(JL086_MCAREA_S_TOP+4)
#if (CSW_BOARD_TYPE == SGD7S_TYPE)
#define	JL086UprgSz_SGD7		JL086UprgSz_SGD7_S
#define	YecAsicPrg_SGD7_Addr	YecAsicPrg_SGD7_S_Addr
#elif	(CSW_BOARD_TYPE == SGD7W_TYPE)
#define JL086_MCAREA_W_TOP		(0xFFE75000)
#define	JL086UprgSz_SGD7		*(CULONG*)(JL086_MCAREA_W_TOP)
#define	YecAsicPrg_SGD7_Addr	(JL086_MCAREA_W_TOP+4)
#endif
#define	YecAsicPrg_SGD7			(CULONG*)(YecAsicPrg_SGD7_Addr)
#define	OVMODTBLG				(CUSHORT*)((CULONG*)YecAsicPrg_SGD7_S_Addr + JL086UprgSz_SGD7_S)
/****************************************************************************************************/
/*																									*/
/*		ＡＳＩＣマイクロプログラムの初期化															*/
/*																									*/
/****************************************************************************************************/
BOOL	SHal_InitMicroProgram( ASICS *SvAsicRegs )
{
	LONG rst_err, load_err, table_err;

	/* マイクロプログラムのロード処理 */
	load_err = SHal_LoadAsicMicroProgram( SvAsicRegs, YecAsicPrg_SGD7, (JL086UprgSz_SGD7) );

	/* マイクロプログラムレジスタのクリア */
	rst_err = SHal_ResetAsicMicroRegister( SvAsicRegs );

	/* マイクロプログラムテーブルデータのロード処理 */ /* <NeCO>_130919 */
	table_err = SHal_LoadAsicMicroTable( (USHORT *)TABLESTART, (USHORT *)OVMODTBLG, TABLESIZE );

	
	return (load_err | rst_err | table_err);
}


/****************************************************************************************************/
/*																									*/
/*		ASIC マイクロプログラムスタート処理															*/
/*																									*/
/****************************************************************************************************/
void	SHal_StartMicroProgram( ASICS *SvAsicRegs )
{

/*--------------------------------------------------------------------------------------------------*/
/*		Start ASIC Current-A/D, Reference-A/D, Micro-Program										*/
/*--------------------------------------------------------------------------------------------------*/
	/* Current-A/D Cycle Synchronize */
	SvAsicRegs->AsicHwReg->AREG_ADFSYNC = 0x0000;
	/* Reference-A/D Cycle Synchronize */
	SvAsicRegs->AsicHwReg->AREG_ADSYNC = 0x0000;
/*----------------------------------------------------------------------------------------------*/
	/* Clear Alarm */
	SvAsicRegs->AsicHwReg->AREG_ALMCLR = 0x802F;
	/* ASIC Write Enable */
	SvAsicRegs->AsicHwReg->AREG_SETREG1 = 0x0005L;
	/* Start Micro-Program */
	// ★(MregCtstr = MPCMD_PRGSTART) MregCtstr更新を忘れずに!
	SvAsicRegs->AsicHwReg->MREG_CTSTR = MPCMD_PRGSTART;
	SvAsicRegs->MicroRunningSts = TRUE;
	/* Set ASIC Start Register */
	SvAsicRegs->AsicHwReg->AREG_SETREG1 = 0x0001;
}



/****************************************************************************************************/
/*																									*/
/*		ASIC マイクロプログラムストップ処理															*/
/*																									*/
/****************************************************************************************************/
void	SHal_StopMicroProgram( ASICS *SvAsicRegs )
{
/*--------------------------------------------------------------------------------------------------*/
/*		Stop ASIC Micro-Program																		*/
/*--------------------------------------------------------------------------------------------------*/
	/* ASIC Write Enable */
	SvAsicRegs->AsicHwReg->AREG_SETREG1 = 0x0005L;
	/* Start Micro-Program */
	// ★(MregCtstr = MPCMD_PRGSTART) MregCtstr更新を忘れずに!
	SvAsicRegs->AsicHwReg->MREG_CTSTR = MPCMD_PRGSTOP;
	SvAsicRegs->MicroRunningSts = FALSE;
}



#ifdef CSW_MICRO_DEBUG_MODE
/****************************************************************************************************/
/*																									*/
/*	システムクロック（フィードバックタイマ）差分取得関数 （ 1Clock : 37.5[ns] ）											*/
/*																									*/
/****************************************************************************************************/
void SHal_SetMicroDebugMode( ASIC_HW_IF_STR *pAsicHwReg )
{
	/* Output INT2 Interrupt Signal from TP5 */
	pAsicHwReg->MREG_DBOS = 0x00000900;
}
#endif



/****************************************************************************************************/
/*																									*/
/*	システムクロック→[ns]変換 μプログラム処理時間測定用											*/
/*																									*/
/****************************************************************************************************/
ULONG SHal_ClockToTime( USHORT clock_diff )
{
	float time_in_ns;
	time_in_ns = (float)clock_diff * (float)(37.5);
	return (ULONG)time_in_ns; /* [ns] */
}


/****************************************************************************************************/
/*																									*/
/*	システムクロック（フィードバックタイマ）取得関数 					）											*/
/*																									*/
/****************************************************************************************************/
USHORT SHal_GetSysClock( ASICS *SvAsicRegs )
{
	/* ASICのフィードバックカウンタ(16bit : 1Clock : 37.5[ns])を返す */
	return SvAsicRegs->AsicHwReg->AREG_TCNT;
}

/****************************************************************************************************/
/*																									*/
/*	システムクロック（フィードバックタイマ）差分取得関数 （ 1Clock : 37.5[ns] ）											*/
/*																									*/
/****************************************************************************************************/
USHORT SHal_Get_dClock( USHORT last_clk , USHORT clk )
{
	USHORT d_clk;

	if( clk > last_clk )
	{
		d_clk = clk - last_clk;
	}
	else
	{/* オーバーフロー 対策*/
		d_clk = ( 0xFFFF - last_clk ) + clk;
	}
	/* 今回クロック(clk)と前回クロック(last_clk)の差分を返す */
	return d_clk;
}



#endif /* _JL076_USE_ */
/***************************************** end of file **********************************************/
