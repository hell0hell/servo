/****************************************************************************************************/
/*																									*/
/*																									*/
/*		InitJL076_sub.c : JL-076初期化関連サブ関数モジュール										*/
/*			※ファイル名はJL086だが、中身はJL076※													*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : JL-076初期化関連サブ関数を定義。														*/
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
/*	Note	:	初版	2009.06.04	Y.Oka															*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"
#include "HwDeviceIf.h"
#include "JL086ServoIpHw.h"
#include "KLib.h"


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
	TempICI2Cdrv_Initialize( AsicHwReg);			/* 温度センサ初期化								*//* <TempSensorIC> */

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
		pAsicHwReg->AREG_SEPGSET00 &= (0xffff & ~BIT1);
		pAsicHwReg->AREG_SEPGSET10 &= (0xffff & ~BIT1);
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
	/*----------------------------------------------------------------------------------------------*/
	/*		クロック機能の初期化(JL-076でソフトリセット前にクロック有効が必要)						*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SETREG1	= 0x0005;		/* ASIC Write Enable								*/
	pAsicHwReg->AREG_CSTP		= 0x0000;		/* ASIC Clock Stop Selection						*/
	pAsicHwReg->AREG_SETREG1	= 0x0001;		/* ASIC Write Disable								*/

	/*----------------------------------------------------------------------------------------------*/
	/*		ソフトリセット																			*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SETREG1	= 0x0009;		/* ASIC S/W Reset Enable							*/
	KlibWaitus( 1 );							/* Wait												*/
	pAsicHwReg->AREG_SRESET		= 0x0000;		/* ASIC Soft Reset									*/
	KlibWaitus( 1 );							/* Wait												*/
	pAsicHwReg->AREG_SETREG1	= 0x0001;		/* ASIC S/W Reset Disable							*/

	/*----------------------------------------------------------------------------------------------*/
	/*		クロック機能の初期化																	*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SETREG1	= 0x0005;		/* ASIC Write Enable								*/
	pAsicHwReg->AREG_CSTP		= 0x0000;		/* ASIC Clock Stop Selection						*/
												/*	Encoder ADIF, FB Pulse Cntr,					*/
												/*	New Incre Decoder,Dedband Comp. Clock Stop		*/
	pAsicHwReg->AREG_SETREG1	= 0x0001;		/* ASIC Write Disable								*/

	/*----------------------------------------------------------------------------------------------*/
	/*		演算部(MicroProgram)ウォッチドッグ機能＆パルス変換Ｃ相マスクビットの初期化				*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_WDT1SET	= 0x0150;		/* Set C-Phase Mask(bit8), WDT Stop(bit6),			*/
												/*		and Micro WDT Time Over(0x10*16us=256us)	*/
	/*----------------------------------------------------------------------------------------------*/
	/*		ホスト(CPU)ウォッチドッグ機能の初期化													*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_WDT2SET	= 0x0020;		/* Set CPU WDT Time Over(64us*0x20=2048us)			*/

	/*----------------------------------------------------------------------------------------------*/
	/*		アナログモニタ機能の初期化																*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_PWMU		= 0x0000;		/* Set Zero for Initial Out							*/
	pAsicHwReg->AREG_PWMV		= 0x0000;		/* Set Zero for Initial Out							*/

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
	pAsicHwReg->AREG_ALMSET		= 0x0000;		/* HWGBON:Invalid, DBONL:DBON						*/

	/*----------------------------------------------------------------------------------------------*/
	/*		極性＆フィルタ設定 : OV, OC, VCAP, BBON1, BBON2											*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SETREG1	= 0x0005;		/* ASIC Write Enable								*/
	pAsicHwReg->AREG_SETREG2	= 0x30F0;		/* OV, OC : ActiveLow, Filter valid					*/
	pAsicHwReg->AREG_SETREG1	= 0x0001;		/* ASIC Write Disable								*/

	/*----------------------------------------------------------------------------------------------*/
	/*		アラームクリア																			*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_ALMCLR		= 0xFFFF;		/* Clear All Alarm									*/

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
	pAsicHwReg->AREG_INT1SET	= 0x0000;		/* Disable All Interrupt							*/

	/*----------------------------------------------------------------------------------------------*/
	/*		割込用タイマレジスタ設定																*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_INT0SET	= 0x0000;		/* INT0 Frequency									*/

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
	pAsicHwReg->AREG_PFNCSET	&= 0xFF80;		/* 端子機能設定 : SESEL0:S-PG0, SESEL1:S-PG1		*/

	/*----------------------------------------------------------------------------------------------*/
	/*		シリアルエンコーダＩＦレジスタ初期化													*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SEPGSET00	= 0x1006;		/* CH0,1共通 CrossTalk Avoidance ON,				*/
	pAsicHwReg->AREG_SEPGSET10	= 0x1006;		/* CH0,1共通 ManchesterCode Squelch ON,				*/
												/* CH0,1共通 NetworkSync OFF						*/
	pAsicHwReg->AREG_SEPGSET01	= 0x0001;		/* CH0,1共通 Slave Address : 01h					*/
	pAsicHwReg->AREG_SEPGSET11	= 0x0001;		/* CH0,1共通 16Byte P0 Master Mode					*/

	/*----------------------------------------------------------------------------------------------*/
	/*		シリアルエンコーダ送信データレジスタ初期化												*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SE0_TXD0	= 0x0000;		/* CH0 Transmit Register							*/
	pAsicHwReg->AREG_SE0_TXD1	= 0x0000;		/* CH0 Transmit Register							*/
	pAsicHwReg->AREG_SE0_TXD2	= 0x0000;		/* CH0 Transmit Register							*/
	pAsicHwReg->AREG_SE0_TXD3	= 0x0000;		/* CH0 Transmit Register							*/
	pAsicHwReg->AREG_SE0_TXD4	= 0x0000;		/* CH0 Transmit Register							*/
	pAsicHwReg->AREG_SE0_TXD5	= 0x0000;		/* CH0 Transmit Register							*/
	pAsicHwReg->AREG_SE0_TXD6	= 0x0000;		/* CH0 Transmit Register							*/
	pAsicHwReg->AREG_SE0_TXD7	= 0x0000;		/* CH0 Transmit Register							*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SE1_TXD0	= 0x0000;		/* CH1 Transmit Register							*/
	pAsicHwReg->AREG_SE1_TXD1	= 0x0000;		/* CH1 Transmit Register							*/
	pAsicHwReg->AREG_SE1_TXD2	= 0x0000;		/* CH1 Transmit Register							*/
	pAsicHwReg->AREG_SE1_TXD3	= 0x0000;		/* CH1 Transmit Register							*/
	pAsicHwReg->AREG_SE1_TXD4	= 0x0000;		/* CH1 Transmit Register							*/
	pAsicHwReg->AREG_SE1_TXD5	= 0x0000;		/* CH1 Transmit Register							*/
	pAsicHwReg->AREG_SE1_TXD6	= 0x0000;		/* CH1 Transmit Register							*/
	pAsicHwReg->AREG_SE1_TXD7	= 0x0000;		/* CH1 Transmit Register							*/

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
	pAsicHwReg->AREG_IOSTAT2	= KPX_AREG_IOSTAT2_INIOUT;	/* Set Initial Output Data to High Word	*/
	pAsicHwReg->AREG_IOSTAT1	= KPX_AREG_IOSTAT1_INIOUT;	/* Set Initial Output Data to Low  Word	*/

	/*----------------------------------------------------------------------------------------------*/
	/*		非同期シリアルＩＦの初期化																*/
	/*----------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_UARTSTS	= 0x0000;		/* UART Status Register								*/

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
//	uswk = ( uswk | 0x0100 );							/* CLMラッチイネーブル設定					*/
		;												/* @@ CLMラッチディセーブル					*/
	pAsicHwReg->AREG_POCSET = uswk;						/* 指令カウンタ設定							*/
	MlibWaitus(1);										/* @@ 念のため								*/
	pAsicHwReg->AREG_POCSET = ( uswk | 0x0100 );		/* @@ CLMラッチイネーブル設定				*/
	
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
/*		ＡＤインターフェイス初期化																	*/
/*																									*/
/****************************************************************************************************/
void	SHal_InitAsicAdInterface( ASIC_HW_IF_STR *pAsicHwReg )
{
/*--------------------------------------------------------------------------------------------------*/
/*		共通ＡＤ設定(指令ＡＤ＆電流検出ＡＤ)														*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_ADSET1		= 0x1043;		/* Command AD 16Bit AD I/F							*/
												/* Current AD 10Bit Decimation Filter				*/
												/* Current AD Sync Invalid							*/

/*--------------------------------------------------------------------------------------------------*/
/*		指令ＡＤインターフェイス初期化			Comment : MSB --> LSB								*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_ADSET2		= 0xC0CE;		/* AD Clock, AD Frequency, Base Frequency			*/
	pAsicHwReg->AREG_ADSET3		= 0x8280;		/* AD Start Timing, AD I/F Cycle					*/
	pAsicHwReg->AREG_ADSET4		= 0xDADA;		/* AD Latch Timing, AD Latch Timing					*/
	pAsicHwReg->AREG_ADSET5		= 0x80DB;		/* AD Control H to L Edge, L to H Edge				*/
	pAsicHwReg->AREG_ADSET6		= 0x80DB;		/* AD Control H to L Edge, L to H Edge				*/

/*--------------------------------------------------------------------------------------------------*/
/*		電流検出ＡＤインターフェイス初期化			   Comment : MSB --> LSB						*/
/*--------------------------------------------------------------------------------------------------*/
	/* Mode3 */									/* 電流制御周期 = 15.625us(64kHz)					*/
	pAsicHwReg->AREG_ADFCNT1	= 0x00FB;		/* 電流ADIF基本クロック周波数 = 3.2MHz				*/
	pAsicHwReg->AREG_ADFCNT2	= 0x0038;		/* ADIF Cycle, (256 - 38h) / 3.2MHz = 62.5us		*/
	pAsicHwReg->AREG_ADFCNT3	= 0x0000;		/* AD Latch Timing, AD Latch Timing					*/
	pAsicHwReg->AREG_ADFCNT4	= 0x3839;		/* AD Control H to L Edge, L to H Edge				*/
	pAsicHwReg->AREG_ADFCNT5	= 0x0000;		/* AD Control H to L Edge, L to H Edge				*/

/*--------------------------------------------------------------------------------------------------*/
/*		電流検出ＡＤデシメーションフィルタ初期化													*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SDMCONF1	= 0x181B;		/* CH1,2,3 Mode3, 電流検出分解能 = 11.9bit			*/
	pAsicHwReg->AREG_SDMCONF2	= 0x2A47;		/* プリトリガ計算:1/8回, 並列個数:4					*/
												/* Overrange Detect:Mask, Mclock:10MHz Clock Out	*/
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
		pAsicHwReg->AREG_TIM1SET = 0xEB9C;		/* TB=(256 - 156)[us], TW=(255 - 235)[us]			*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		DBパルス設定：DB方式 = リレー																*/
/*--------------------------------------------------------------------------------------------------*/
	else										/* PnE0C.5:4 = x:0 : リレー							*/
	{											/* LowByte:Frequency,  HighByte:HighPulseWidth		*/
		pAsicHwReg->AREG_TIM1SET = 0x009C;		/* TB=(256 - 156)[us], TW=(255 - 0)[us]				*/
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
	pAsicHwReg->AREG_SETREG3	= 0x0000;		/* REGOFF=0, DISCH=0, GFTON=0						*/

	return;
}



/****************************************************************************************************/
/*																									*/
/*		PWM制御機能の初期化																			*/
/*																									*/
/****************************************************************************************************/
void	SHal_InitAsicPwmControl( ASIC_HW_IF_STR *pAsicHwReg, LONG odt_c )
{
	USHORT	OnDelay;

/*--------------------------------------------------------------------------------------------------*/
/*		オンディレイ時間設定																		*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SETREG1	= 0x0005;		/* ASIC Write Enable								*/
	pAsicHwReg->AREG_SETREG2	&= (~BIT12);	/* Clock Select : 80MHz								*/
	pAsicHwReg->AREG_SETREG2	|= (BIT11);		/* Clock Select : 80MHz								*/
/* for debug (INGRAMでは必要であった) */
//	pAsicHwReg->AREG_SETREG2		|= BIT9;	/* Carrer Counter Sync								*/
/* for debug (INGRAMでは必要であった) */
	/*----------------------------------------------------------------------------------------------*/
	OnDelay = (USHORT)((ASIC_CLKMHZ * odt_c) / 10);	/* 80[MHz] * Td[0.1us] / 10						*/
	pAsicHwReg->AREG_DYSETUP1	= OnDelay;		/* UP1 : U-Phase,Positive-Side						*/
	pAsicHwReg->AREG_DYSETUN1	= OnDelay;		/* UN1 : U-Phase,Negative-Side						*/
	pAsicHwReg->AREG_DYSETVP1	= OnDelay;		/* VP1 : V-Phase,Positive-Side						*/
	pAsicHwReg->AREG_DYSETVN1	= OnDelay;		/* VN1 : V-Phase,Negative-Side						*/
	pAsicHwReg->AREG_DYSETWP1	= OnDelay;		/* WP1 : W-Phase,Positive-Side						*/
	pAsicHwReg->AREG_DYSETWN1	= OnDelay;		/* WN1 : W-Phase,Negative-Side						*/
	pAsicHwReg->AREG_SETREG1	= 0x0001;		/* ASIC Write Disable								*/

	return;
}



/****************************************************************************************************/
/*																									*/
/*		マイクロプログラムのロード処理																*/
/*																									*/
/****************************************************************************************************/
BOOL	SHal_LoadAsicMicroProgram( ASICS *SvAsicRegs, CUSHORT *src_adr, LONG u_size)
{
	LONG				i;
#ifndef _VC_DEBUG_ /* 2012.08.24 Y.Oka Make Mercury Base Software */
	VULONG				dummy;
#endif /* _VC_DEBUG_ */ /* 2012.08.24 Y.Oka Make Mercury Base Software */
	CUSHORT				*PrgSrc;							/* MicroProgram Src  Address			*/
	LONG				PrgSize;							/* MicroProgram Word Size				*/
	ASIC_HW_IF_STR		*pAsicHwReg;

	pAsicHwReg = SvAsicRegs->AsicHwReg;
/*--------------------------------------------------------------------------------------------------*/
/*		μプログラムの選択																			*/
/*--------------------------------------------------------------------------------------------------*/
	PrgSrc  = src_adr;										/* Set Src  Address						*/
	PrgSize = u_size;										/* Set Word Size						*/
	if( (PrgSize % 3) != 0 )								/* サイズチェック						*/
	{
		return TRUE;	/* A.BF2 : システムアラーム２(Micro異常) */
	}

/*--------------------------------------------------------------------------------------------------*/
/*		μプログラムの書込み許可																	*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SETREG1 = 0x0005 ;						/* ASIC Write Enable					*/
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
//		dummy = pAsicHwReg->AREG_TSTT1CNT;

		/* ロードポインタチェック */
		if( ( pAsicHwReg->MREG_UPLDA ) != ((i%3)<<14)+(i/3) )
		{
			return TRUE;	/* A.BF2 : システムアラーム２(Micro異常) */
		}

		pAsicHwReg->MREG_UPLD	= PrgSrc[i];				/* μプログラム書き込み					*/

#else /* _VC_DEBUG_ */ /* 2012.08.24 Y.Oka Make Mercury Base Software */

		/* μプログラム書き込み */
		SvAsicRegs->pDummyMicroSram[i] = PrgSrc[i];

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
		if( ( pAsicHwReg->MREG_UPLDA ) != ((i%3)<<14)+(i/3) )
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
	pAsicHwReg->AREG_SETREG1	= 0x0001;							/* ASIC Write Disable					*/

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
	VUSHORT				vuswk;								/* USHORT Work							*/
	VULONG				Dummy;								/* Dummy for Read						*/
	VUSHORT				*WkReg;								/* MicroProgram Work Register Address	*/
	ASIC_HW_IF_STR		*pAsicHwReg;

	WkReg = (VUSHORT*)(SvAsicRegs->AsicMicroReg);			/* MicroProgram Work Register Address	*/
	pAsicHwReg = SvAsicRegs->AsicHwReg;

/*--------------------------------------------------------------------------------------------------*/
/*		マイクロプログラム実行停止(念のため)														*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SETREG1	= 0x0005;					/* ASIC Write Enable					*/
	pAsicHwReg->MREG_CTSTR		= MPCMD_PRGSTOP;			/* μプログラム実行停止					*/
	pAsicHwReg->AREG_SETREG1	= 0x0001;					/* ASIC Write Disable					*/
	SvAsicRegs->MicroRunningSts = FALSE;					/* μプログラム実行停止(CPU管理)		*/

/*--------------------------------------------------------------------------------------------------*/
/*		ワークレジスタのチェック＆クリア															*/
/*--------------------------------------------------------------------------------------------------*/
	for( i=0; i<(ASIC_MREGSIZE/2); i++ )					/* Write to WkReg[i]					*/
	{
		WkReg[i]	= (USHORT)(i);							/* Write 0,1,2,3,4,5,...				*/
	}
	/*----------------------------------------------------------------------------------------------*/
	for( i=0; i<(ASIC_MREGSIZE/2); i++ )					/* Check WkReg[i]						*/
	{
		vuswk = WkReg[i];									/* Read WkReg[i]						*/
		if( vuswk != (i) )									/* Check Read Data						*/
		{
			return TRUE;	/* A.BF2 : システムアラーム２(Micro異常) */
		}
	}
/*--------------------------------------------------------------------------------------------------*/
	for( i=0; i<(ASIC_MREGSIZE/2); i++ )					/* Check and Clear WkReg[i]				*/
	{
		WkReg[i]	= 0x5A5A;								/* Write 0x5A5A to WkReg[i]				*/
		Dummy = pAsicHwReg->AREG_PWMU;						/* Dummy Read							*/
		vuswk = WkReg[i];									/* Read and Check						*/
		if( vuswk != 0x5A5A )								/* Check Read Data						*/
		{
			return TRUE;	/* A.BF2 : システムアラーム２(Micro異常) */
		}
	/*----------------------------------------------------------------------------------------------*/
		WkReg[i]	= 0xA5A5;								/* Write 0xA5A5 to WkReg[i]				*/
		Dummy = pAsicHwReg->AREG_PWMU;						/* Dummy Read							*/
		vuswk = WkReg[i];									/* Read and Check						*/
		if( vuswk != 0xA5A5 )								/* Check Read Data						*/
		{
			return TRUE;	/* A.BF2 : システムアラーム２(Micro異常) */
		}
	/*----------------------------------------------------------------------------------------------*/
		WkReg[i]	= 0x0000;								/* Clear WkReg[i]						*/
		Dummy = pAsicHwReg->AREG_PWMU;						/* Dummy Read							*/
		vuswk = WkReg[i];									/* Read and Check						*/
		if( vuswk != 0x0000 )								/* Check Read Data						*/
		{
			return TRUE;	/* A.BF2 : システムアラーム２(Micro異常) */
		}
	}

	return FALSE;
}



/****************************************************************************************************/
/*																									*/
/*		ＡＳＩＣマイクロプログラムの初期化															*/
/*																									*/
/****************************************************************************************************/
BOOL	SHal_InitMicroProgram( ASICS *SvAsicRegs )
{
	LONG rst_err, load_err;

	/* マイクロプログラムのロード処理 */
//	load_err = SHal_LoadAsicMicroProgram( SvAsicRegs, YecAsicPrg_INGRAM, (JL076UprgSz_INGRAM) );
	load_err = SHal_LoadAsicMicroProgram( SvAsicRegs, YecAsicPrg_SGDV, (JL076UprgSz_SGDV) );

	/* マイクロプログラムレジスタのクリア */
	rst_err = SHal_ResetAsicMicroRegister( SvAsicRegs );

	return (load_err | rst_err);
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
	SvAsicRegs->AsicHwReg->AREG_ADSYNC = 0x0000;	// ★ 指令ADは使用しないため不要では？	nov29
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



/***************************************** end of file **********************************************/
