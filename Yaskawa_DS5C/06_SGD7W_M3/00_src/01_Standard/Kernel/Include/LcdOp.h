/****************************************************************************************************/
/*																									*/
/*																									*/
/*		LcdOp.h : LCD Operator Common Header File													*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1. LcdオペレータＩＦ定義																	*/
/*																									*/
/*		2. パネルオペレータＩＦ定義																	*/
/*																									*/
/*		3. 																							*/
/*																									*/
/*		4. 																							*/
/*																									*/
/*		5. 																							*/
/*																									*/
/*		6. 																							*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.1.00 : 2013.04.15  K.Ozaki																*/
/*								LcdOp.h <= RndApi.h + RndRpx.h +LcdOp.c								*/
/*		Rev.1.10 : 2013.9.26  K.Ozaki	<S00C>														*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#ifndef ___LCDOP___
#define ___LCDOP___

#include "Object.h"

#define ADJUST_STRUCT_ALIGN( x ) 
#define CCSW_HWID  (0x55AA) /* Dummy */
#define CCSW_CMDID  (0xAA55) /* Dummy */


/****************************************************************************************************/
/*																									*/
/*		オペレータＩＦマクロ定義																	*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		SGDV互換性確保マクロ(見直し中)																*/
/*--------------------------------------------------------------------------------------------------*/
#define KpiGetAlarmInfo() ALMGetAlarmInfo( pAxRsc->AlmManager ) /* TODO: 暫定 */
#define KriResetSystemAlarm() ALMSetAlarmResetRequest( pAxRsc->AlmManager ) /* TODO: 暫定 */

#if 0 															/* SGDV and Jupiter					*/
/*--------------------------------------------------------------------------------------------------*/
/*	Status Display Check Macro																		*/
/*--------------------------------------------------------------------------------------------------*/
#define CHK_BB()       ( ! RinV.f.BaseEnable )
#define CHK_PDET()     ( ! MencV.f.PhaseReady )
#define CHK_TUNING()   ( RoutV.f.TuningRun )
#define CHK_JOGMODE()  ( BoutV.CtrlMode.b.cm == CTRLMODE_JOG )
#define CHK_ZRETMODE() ( BoutV.CtrlMode.b.cm == CTRLMODE_ZSRCH )
#define CHK_POT()      ( CoutV.f.Sv.Pot )
#define CHK_NOT()      ( CoutV.f.Sv.Not )
#define CHK_PLS()      ( CoutV.f.Sv.Pls )
#define CHK_NLS()      ( CoutV.f.Sv.Nls )
#define CHK_ESTOP()    ( CoutV.f.Sv.EmgStop )
#define CHK_HBB()      ( CoutV.f.Sv.HwbbState )

/*--------------------------------------------------------------------------------------------------*/
/*	Status LED Check Macro																			*/
/*--------------------------------------------------------------------------------------------------*/
#define CHK_SVON()     ( ! CHK_BB() )
#define CHK_TGON()     ( CoutV.f.Ctrl.MotorMoving )
#define CHK_CHG()      ( CoutV.f.Seq.MainPowerOn )
#define CHK_COIN()     ( BoutV.f.Ctrl.Coin ) 
#define CHK_VCMP()     ( CoutV.f.Ctrl.ReachSpd )
#define CHK_TRQMODE()  ( BoutV.CtrlMode.b.cm == CTRLMODE_TRQ )
#define CHK_PREFON()   ( CoutV.f.Sv.Prefon )
#define CHK_VREFON()   ( CoutV.f.Sv.Vrefon )
#define CHK_TREFON()   ( CoutV.f.Sv.Trefon )

#elif CCSW_HWID == HWID_JPCV										/* Jupiter Converter 			*/

/*--------------------------------------------------------------------------------------------------*/
/*	Status Display Check Macro																		*/
/*--------------------------------------------------------------------------------------------------*/
#define CHK_BB()       ( ! RinV.f.BaseEnable )
#define CHK_PDET()     ( 0 ) 
#define CHK_TUNING()   ( 0 )
#define CHK_JOGMODE()  ( 0 )
#define CHK_ZRETMODE() ( 0 )
#define CHK_POT()      ( 0 )
#define CHK_NOT()      ( 0 )
#define CHK_PLS()      ( 0 )
#define CHK_NLS()      ( 0 )
#define CHK_ESTOP()    ( 0 )
#define CHK_HBB()      ( 0 )

/*--------------------------------------------------------------------------------------------------*/
/*	Status LED Check Macro																			*/
/*--------------------------------------------------------------------------------------------------*/
#define CHK_SVON()     ( ! CHK_BB() )
#define CHK_TGON()     ( RinV.f.DrvAxRunning )
#define CHK_CHG()      ( RinV.f.DcPowerOn )
#define CHK_COIN()     ( 0 ) 
#define CHK_VCMP()     ( 0 )
#define CHK_TRQMODE()  ( 0 )
#define CHK_PREFON()   ( RinV.f.DrvReady )
#define CHK_VREFON()   ( 0 )
#define CHK_TREFON()   ( 0 )

#else 																/* Mercury ???	 				*/

/*--------------------------------------------------------------------------------------------------*/
/*	Status Display Check Macro																		*/
/*--------------------------------------------------------------------------------------------------*/
#define CHK_BB()       (!(pAxRsc->SeqCtrlOut->BeComplete))
#define CHK_PDET()     (!(pAxRsc->MencV->PhaseReady))
#define CHK_TUNING()   ( pAxRsc->FnCmnCtrl->State.TuningRun )
#define CHK_JOGMODE()  ( pAxRsc->BaseCtrlOut->CtrlModeOut.b.cm == CTRLMODE_JOG )
#define CHK_ZRETMODE() ( pAxRsc->BaseCtrlOut->CtrlModeOut.b.cm == CTRLMODE_ZSRCH )
#define CHK_POT()      ( pAxRsc->SeqCtrlOut->PotSigSts | (pAxRsc->BaseCtrlOut->CmdSeqBit & POSITIVE_OT_BIT))
#define CHK_NOT()      ( pAxRsc->SeqCtrlOut->NotSigSts | (pAxRsc->BaseCtrlOut->CmdSeqBit & NEGATIVE_OT_BIT))
#define CHK_PLS()      ( 0/* TODO */ )
#define CHK_NLS()      ( 0/* TODO */ )
#define CHK_ESTOP()    ( 0/* TODO */ )
#define CHK_HBB()      ( pAxRsc->SeqCtrlOut->HwbbSts )
#define CHK_ALARM()	   ( pAxRsc->AlmManager->Status.AlmFlag )	/* <S0FF> */
#define CHK_FSTP()	   (pAxRsc->SeqCtrlOut->FstpSts )												  /* <S1F5> */
#define CHK_MOTOR_LESS_TEST()	( pAxRsc->MotorLessTest->MotorLessTestMode == TRUE )				  /* <S1B7> */

/*--------------------------------------------------------------------------------------------------*/
/*	Status LED Check Macro																			*/
/*--------------------------------------------------------------------------------------------------*/
#define CHK_SVON()     ( ! CHK_BB() )
#define CHK_TGON()     ( pAxRsc->SeqCtrlOut->TgOnSts )
#define CHK_CHG()      ( pAxRsc->PowerManager->PowerSts.PowerOn )
#define CHK_COIN()     ( pAxRsc->BaseControls->PosCtrlSts.CoinSignal )
#define CHK_VCMP()     ( pAxRsc->SeqCtrlOut->VcmpSts )
#define CHK_TRQMODE()  ( pAxRsc->BaseCtrlOut->CtrlModeOut.b.cm == CTRLMODE_TRQ ) 
#define CHK_PREFON()   ( pAxRsc->SeqCtrlOut->f.Prefon ) 
#define CHK_VREFON()   ( pAxRsc->SeqCtrlOut->f.Vrefon ) 
#define CHK_TREFON()   ( pAxRsc->SeqCtrlOut->f.Trefon ) 
#define CHK_PERRCLR()  ( pAxRsc->SeqCtrlOut->f.PerrClr )

#endif

/*--------------------------------------------------------------------------------------------------*/
/*		表示時の底																					*/
/*--------------------------------------------------------------------------------------------------*/
#define DSPDEF_BASENBL			(OBJDEF_BASENBL)			/* 属性：ニブル表示						*/
#define DSPDEF_BASEDEC			(OBJDEF_BASEDEC)			/* 属性：10進表示						*/
#define DSPDEF_BASEHEX			(OBJDEF_BASEHEX)			/* 属性：16進表示						*/
#define DSPDEF_BASEBIT			(OBJDEF_BASEBIT)			/* 属性：ビット表示						*/

/*--------------------------------------------------------------------------------------------------*/
/*		符号付きか否か																				*/
/*--------------------------------------------------------------------------------------------------*/
#define DSPDEF_W_SIGN			(TRUE)			/* 符号付											*/
#define DSPDEF_NOSIGN			(FALSE)			/* 符号なし											*/

/*--------------------------------------------------------------------------------------------------*/
/*		LCD Operator PRM/MON Display IDs															*/
/*--------------------------------------------------------------------------------------------------*/
#define	LCDPMDSP_MAX		4					/* 画面数											*/
#define	LCDPMDSP_PM			0					/* PRM/MON画面										*/
#define	LCDPMDSP_ZSRCH		1					/* 原点サーチ画面									*/
#define	LCDPMDSP_JOG		2					/* JOG画面											*/
#define	LCDPMDSP_PJOG		3					/* PROGRAM JOG画面									*/

/*--------------------------------------------------------------------------------------------------*/
/*		LCD Operator Drive Selection 																*/
/*--------------------------------------------------------------------------------------------------*/
#define DRVSEL_MAXUNIT 1
#define DRVSEL_MAXAXISPERUNIT 3

/*--------------------------------------------------------------------------------------------------*/
/*		Drive Selection Type Definition																*/
/*--------------------------------------------------------------------------------------------------*/
#define DRVSEL_TYPE_DRVAXIS		0			/* Drive Axis											*/
#define DRVSEL_TYPE_COMMCPU		1			/* Communication CPU									*/
#define DRVSEL_TYPE_CONVERTER	2			/* Converter for Converter Operator						*/

/****************************************************************************************************/
/*																									*/
/*		Sigma Message for LCD operator Message definitions (Unit: Byte)								*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Sigma Message for LCD operator Message definitions (Unit: Byte)								*/
/*--------------------------------------------------------------------------------------------------*/
#define LCDMSG_AXSIZ		1			/* 軸アドレスサイズ											*/
#define LCDMSG_FUNSIZ		1			/* コマンドサイズ											*/
#define LCDMSG_LENSIZ		2			/* メッセージ長サイズ										*/
/*--------------------------------------------------------------------------------------------------*/
#define LCDMSGR_LEDSIZ		2			/* LEDステータスサイズ										*/
#define LCDMSGR_DSPSIZ		16			/* 表示ステータスサイズ										*/
#define LCDMSGR_CHRSIZ		(5*18)		/* 表示文字サイズ											*/
#define LCDMSGR_SUMSIZ		2			/* チェックサムサイズ										*/
#define LCDMSGR_FONSIZ		(5*16)		/* フォントデータサイズ										*/
/*--------------------------------------------------------------------------------------------------*/
#define	LCDMSG_HDRSIZ		(LCDMSG_AXSIZ  + LCDMSG_FUNSIZ  + LCDMSG_LENSIZ)
#define	LCDMSGR_BDYSIZ		(LCDMSGR_LEDSIZ + LCDMSGR_DSPSIZ + LCDMSGR_CHRSIZ)
/*--------------------------------------------------------------------------------------------------*/
/*		0x64/0x60応答メッセージ長																	*/
/*--------------------------------------------------------------------------------------------------*/
#define LCDMSGR_LEN60		(LCDMSG_HDRSIZ + LCDMSGR_BDYSIZ + LCDMSGR_SUMSIZ)
#define LCDMSGR_LEN64		(LCDMSG_HDRSIZ + LCDMSGR_FONSIZ + LCDMSGR_SUMSIZ)
/*--------------------------------------------------------------------------------------------------*/
/*		Check FEXE_LCDBUF_SIZE																		*/
/*--------------------------------------------------------------------------------------------------*/
#if 0 //TODO:
#if		FEXE_LCDBUF_SIZE != LCDMSGR_LEN60
******* Build Error : FEXE_LCDBUF_SIZE/LCDMSGR_LEN60 Unmatch Error *******
#endif
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		Message Index																				*/
/*--------------------------------------------------------------------------------------------------*/
#define LCDMSGR_AXIDX		0								/* 軸アドレスインデックス				*/
#define LCDMSGR_FUNIDX		(LCDMSGR_AXIDX+LCDMSG_AXSIZ)	/* 軸アドレスインデックス				*/
#define LCDMSGR_LENIDX		(LCDMSGR_FUNIDX+LCDMSG_FUNSIZ)	/* メッセージ長インデックス				*/
#define LCDMSGR_LEDIDX		(LCDMSGR_LENIDX+LCDMSG_LENSIZ)	/* LEDステータスインデックス			*/
#define LCDMSGR_DSPIDX		(LCDMSGR_LEDIDX+LCDMSGR_LEDSIZ)	/* 表示ステータスインデックス			*/
#define LCDMSGR_CHRIDX		(LCDMSGR_DSPIDX+LCDMSGR_DSPSIZ)	/* 表示文字インデックス					*/
#define LCDMSGR_SUMIDX60	(LCDMSGR_CHRIDX+LCDMSGR_CHRSIZ)	/* チェックサムインデックス(60用)		*/
#define LCDMSGR_FONIDX64	(LCDMSGR_LENIDX+LCDMSG_LENSIZ)	/* フォントデータインデックス(64用)		*/
/*--------------------------------------------------------------------------------------------------*/
#define LCDMSGC_KEYIDX		LCDMSGR_LEDIDX					/* キーコードインデックス(コマンド)		*/
/*--------------------------------------------------------------------------------------------------*/
/*		Message Buffer Operation Macro																*/
/*--------------------------------------------------------------------------------------------------*/
#define LPX_GET_LCDKEY( Buf )		(*(USHORT*)&(Buf[LCDMSGC_KEYIDX]))
/*--------------------------------------------------------------------------------------------------*/
#define LPX_GET_MSGLEN( Buf )		(*(USHORT*)&(Buf[LCDMSGR_LENIDX]))
#define LPX_SET_MSGLEN( Buf, Set )	(*(USHORT*)&(Buf[LCDMSGR_LENIDX])) = Set
/*--------------------------------------------------------------------------------------------------*/
#define LPX_GET_LEDSTS( Buf )		(*(USHORT*)&(Buf[LCDMSGR_LEDIDX]))
#define LPX_SET_LEDSTS( Buf, Set )	(*(USHORT*)&(Buf[LCDMSGR_LEDIDX])) = Set
/*--------------------------------------------------------------------------------------------------*/

/****************************************************************************************************/
/*																									*/
/*		Lcd Operator Macro Definition																*/
/*																									*/
/****************************************************************************************************/
//#if 0 /* TODO: */
#if 1														 				/* <S18F> */
typedef	struct	{
		struct	{								/* 表示情報											*/
		USHORT	PnUn[4];						/* Pn/Un種類	0 = Un, 1 = Pn						*/
		USHORT	UnNo[4];						/* Un番号											*/
		USHORT	PnNo[4];						/* Pn番号											*/
		}		dispinf[LCDPMDSP_MAX];			/* index: PRM/MON Display ID						*/
	/*----------------------------------------------------------------------------------------------*/
		USHORT	accesslvl[LCDPMDSP_MAX];		/* アクセスレベル(0xFFFFはデータ無効)				*/
		USHORT	Old_OpeDspMode;					/* 前回のオペレータのユーザー定数表示選択			*/
		USHORT	Old_FnNo;						/* 保存されているFn番号								*/
		USHORT	chksum;							/* チェックサム										*/
} LCDEEP;
#define	LCDEEP_WDSIZE		55
/*--------------------------------------------------------------------------------------------------*/
//#if		LCDEEP_WDSIZE != EEPSIZE_LCDINF									/* <S18F> */
//******* Build Error : LCDEEP_WDSIZE/EEPSIZE_LCDINF Unmatch Error *******
//#endif
#define	EEPADR_LCDINF	EEP_NOSUM_ADDRESS( 0, EEP_LCDINF_OFS )				/* <S18F> */
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		LCD Operator Keys																			*/
/*--------------------------------------------------------------------------------------------------*/
#define LCDKEY_MODE		0x0001
#define LCDKEY_SHUP		0x0002
#define LCDKEY_UP		0x0004
#define LCDKEY_DOWN		0x0008
#define LCDKEY_SCRL		0x0010
#define LCDKEY_SVON		0x0020
#define LCDKEY_DATA		0x0040
#define LCDKEY_SHDOWN	0x0080
#define LCDKEY_READ		0x0100
#define LCDKEY_WRITE	0x0200
#define LCDKEY_ALMRST	0x0400
#define LCDKEY_RST		(LCDKEY_UP | LCDKEY_DOWN)
/*--------------------------------------------------------------------------------------------------*/
/*		LCD Operator LED display bits																*/
/*--------------------------------------------------------------------------------------------------*/
#define LCDLED_REF		0x0001
#define LCDLED_TGON		0x0002
#define LCDLED_SVON		0x0004
#define LCDLED_COIN		0x0008
/*--------------------------------------------------------------------------------------------------*/
#define LCDLED_READ		0x0020
#define LCDLED_WRITE	0x0040
#define LCDLED_CHG		0x0080
/*--------------------------------------------------------------------------------------------------*/
#define LCDLED_REFB		0x0100
#define LCDLED_TGONB	0x0200
#define LCDLED_SVONB	0x0400
#define LCDLED_COINB	0x0800
/*--------------------------------------------------------------------------------------------------*/
#define LCDLED_READB	0x2000
#define LCDLED_WRITEB	0x4000
#define LCDLED_CHGB		0x8000

/*--------------------------------------------------------------------------------------------------*/
/*		Online modes (for LopV.Online)																*/
/*--------------------------------------------------------------------------------------------------*/
#define LCD_OFFLINE		0				/* LCDオペレータが接続されていない							*/
#define LCD_ONLINE1		1				/* フォントをロードした(0x64コマンド)						*/
#define LCD_ONLINE2		2				/* 通信確立した(フォントロードおよびLCD関係変数初期化済)	*/
/*--------------------------------------------------------------------------------------------------*/
/*		Edit modes (for LopV.EditMode)																*/
/*--------------------------------------------------------------------------------------------------*/
#define LCDEM_PnUnNum	0				/* Pn/Un番号編集中											*/
#define LCDEM_PnUnSel	1				/* Pn/Un選択中												*/
#define LCDEM_PnData	2				/* パラメータ値編集中										*/
/*--------------------------------------------------------------------------------------------------*/
/*		Pn/Un selection (for Display Inf Tables)													*/
/*--------------------------------------------------------------------------------------------------*/
#define UnSelected		0				/* Un選択中													*/
#define PnSelected		1				/* Pn選択中													*/
/*--------------------------------------------------------------------------------------------------*/
/*		Operator Parameter Write Modes																*/
/*--------------------------------------------------------------------------------------------------*/
#define	OPWM_None				0					/*												*/
#define	OPWM_UpdtWait			1					/*												*/
#define	OPWM_UpdtBlnk			2					/* LCDのみ										*/
#define	OPWM_InfWrWait			3					/* LCDのみ										*/
/*--------------------------------------------------------------------------------------------------*/
/*		Function modes (for LopV.FunMode)															*/
/*--------------------------------------------------------------------------------------------------*/
#define LCDFM_Num		0				/* Fn番号選択中												*/
#define LCDFM_Exe		1				/* Pn実行中													*/
/*--------------------------------------------------------------------------------------------------*/
/*		Function number of the Password inpu mode (fn010)											*/
/*--------------------------------------------------------------------------------------------------*/
#define LCDPASSWD_FNNUM (0x010)

#if CCSW_CMDID == CMDID_EXIST			/*<11J42>*/
#define LCDFM_OPTLIST_RES	2	/* オプション補助機能リスト取得コマンドレスポンス処理状態			*/
#define LCDFM_OPTBEGIN_RES	3	/* オプション補助機能開始コマンドレスポンス処理状態					*/
#define LCDFM_OPTEXE		4	/* オプション補助機能実行中											*/
#define LCDFM_OPTMODE_RES	5	/* オプション補助機能操作コマンドレスポンス処理状態					*/
#define LCDFM_OPTEND_RES	6	/* オプション補助機能終了コマンドレスポンス処理状態					*/
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		Drive operation status (for LopV.DrvOpStat)													*/
/*--------------------------------------------------------------------------------------------------*/
#define DRVOPST_WAITKEYSEND 0			/* キー情報転送待ち		*/
#define DRVOPST_SENDKEYINFO 1			/* キー情報転送中		*/
#define DRVOPST_FORCECON    2			/* 強制接続中	*/
#define DRVOPST_ERROR	    3			/* エラー状態	*/


/****************************************************************************************************/
/*																									*/
/*		Lcd Operator Message I/F																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		補足：#defineが複雑なので、構造体定義+#defineに変更する。(将来用)							*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		デジオペ通信コマンドコード																	*/
/*--------------------------------------------------------------------------------------------------*/
#define	LCD_CMD_NOR				(0x60)			/* デジオペ 通常通信								*/
#define	LCD_CMD_LOAD			(0x61)			/* デジオペ EEPROM読出し							*/
#define	LCD_CMD_SAVE			(0x62)			/* デジオペ EEPROM書込み							*/
#define	LCD_CMD_DIAG			(0x63)			/* デジオペ 自己診断コマンド						*/
#define	LCD_CMD_INIT			(0x64)			/* デジオペ 電源投入コマンド(初期通信)				*/
/*--------------------------------------------------------------------------------------------------*/
/*		通常通信：受送信バッファデータ構造定義														*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{								/* CmdCode : 0x60									*/
		UCHAR	AxisAdr;						/* 軸アドレス										*/
		UCHAR	CmdCode;						/* コマンドコード									*/
		USHORT	Length;							/* 受信メッセージ長(Byte数)							*/
	/*----------------------------------------------------------------------------------------------*/
		USHORT	OpeKey;							/* オペレータ操作キー								*/
	/*----------------------------------------------------------------------------------------------*/
		USHORT	ChkSum;							/* チェックサム										*/
} RXBUF_NOR;									/* 使用方法：RxBuf=(RXBUF_NOR*)LopMsg.CmdBuf)		*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{								/* CmdCode : 0x60									*/
		UCHAR	AxisAdr;						/* 軸アドレス										*/
		UCHAR	CmdCode;						/* コマンドコード									*/
		USHORT	Length;							/* 応答メッセージ長(Byte数)							*/
	/*----------------------------------------------------------------------------------------------*/
		USHORT	LedSts;							/* ＬＥＤ表示情報									*/
		UCHAR	DispSts[16];					/* ステータス表示									*/
		UCHAR	DispLine[5][17+1];				/* 画面表示情報(Title + 4*DispLine)					*/
	/*----------------------------------------------------------------------------------------------*/
		USHORT	ChkSum;							/* チェックサム										*/
} TXBUF_NOR;									/* 使用方法：TxBuf=(TXBUF_NOR*)LopMsg.ResBuf)		*/
/*--------------------------------------------------------------------------------------------------*/
#define	LCD_RES_NOR_LEN  (4 + 2+16+(5*18) + 2)	/* 応答メッセージ長(Byte数)							*/
/*--------------------------------------------------------------------------------------------------*/
/*		電源投入：受送信バッファデータ構造定義(初期通信)											*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{								/* CmdCode : 0x64									*/
		UCHAR	AxisAdr;						/* 軸アドレス										*/
		UCHAR	CmdCode;						/* コマンドコード									*/
		USHORT	Length;							/* 応答メッセージ長(Byte数)							*/
		USHORT	ChkSum;							/* チェックサム										*/
} RXBUF_INIT;									/* 使用方法：RxBuf=(TXBUF_INIT*)LopMsg.CmdBuf)		*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{								/* CmdCode : 0x64									*/
		UCHAR	AxisAdr;						/* 軸アドレス										*/
		UCHAR	CmdCode;						/* コマンドコード									*/
		USHORT	Length;							/* 応答メッセージ長(Byte数)							*/
	/*----------------------------------------------------------------------------------------------*/
		UCHAR	LcdFont[(5*16)];				/* ＬＣＤオペレータにロードするフォント				*/
	/*----------------------------------------------------------------------------------------------*/
		USHORT	ChkSum;							/* チェックサム										*/
} TXBUF_INIT;									/* 使用方法：TxBuf=(TXBUF_INIT*)LopMsg.ResBuf)		*/
/*--------------------------------------------------------------------------------------------------*/
#define	LCD_RES_INIT_LEN  (4 + (5*16) + 2)		/* 応答メッセージ長(Byte数)							*/
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/*		<S031> LcdPrmCopy LOADコマンド送受信バッファデータ構造定義									*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{								/* CmdCode : 0x61									*/
		UCHAR	AxisAdr;						/* 軸アドレス										*/
		UCHAR	CmdCode;						/* コマンドコード									*/
		USHORT	Length;							/* 受信メッセージ長(Byte数)							*/
	/*----------------------------------------------------------------------------------------------*/
		USHORT	OpeKey;							/* オペレータ操作キー								*/
		USHORT	Data[8];						/* 受信データ										*/
	/*----------------------------------------------------------------------------------------------*/
		USHORT	ChkSum;							/* チェックサム										*/
} RXBUF_LOAD;									/* 使用方法：RxBuf=(RXBUF_LOAD*)LopMsg.CmdBuf)		*/
/*--------------------------------------------------------------------------------------------------*/
#define	LCD_RES_LOAD_LEN  (LCD_RES_NOR_LEN  +  4)	/* 応答メッセージ長(Byte数)						*/
/*--------------------------------------------------------------------------------------------------*/
/*		@@ LcdPrmCopy SAVEコマンド送受信バッファデータ構造定義										*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{								/* CmdCode : 0x62									*/
		UCHAR	AxisAdr;						/* 軸アドレス										*/
		UCHAR	CmdCode;						/* コマンドコード									*/
		USHORT	Length;							/* 応答メッセージ長									*/
	/*----------------------------------------------------------------------------------------------*/
		USHORT	LedSts;							/* ＬＥＤ表示情報									*/
		UCHAR	DispSts[16];					/* ステータス表示									*/
		UCHAR	DispLine[5][17+1];				/* 画面表示情報(Title + 4*DispLine)					*/
	/*----------------------------------------------------------------------------------------------*/
		USHORT	EepAddr;						/* EEPROMアドレス(WORD)								*/
		USHORT	DataNum;						/* 送信データ数(0～8)								*/
		USHORT	Data[8];						/* 送信データ										*/
	/*----------------------------------------------------------------------------------------------*/
		USHORT	ChkSum;							/* チェックサム										*/
	/*----------------------------------------------------------------------------------------------*/
		UCHAR	DispLineBlink[5];				/* Work : 画面表示ブリンク情報						*/
} TXBUF_SAVE;									/* 使用方法：TxBuf=(TXBUF_SAVE*)LopMsg.ResBuf)		*/
/*--------------------------------------------------------------------------------------------------*/
#define	LCD_RES_SAVE_LEN  (LCD_RES_LOAD_LEN + 16)	/* 応答メッセージ長(Byte数)						*/

/****************************************************************************************************/
/*																									*/
/*		Operator I/F Struct Definition																*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		オペレータ操作キー定義																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct {
		UINT	mode		:1;					/* Mode Key											*/
		UINT	up			:1;					/* Up Key											*/
		UINT	down		:1;					/* Down Key											*/
		UINT	shup		:1;					/* Shift-up Key : <-								*/
		UINT	shdown		:1;					/* Shift-down Key : ->								*/
		UINT	data		:1;					/* Data Key											*/
		UINT	svon		:1;					/* Svon Key											*/
		UINT	almrst		:1;					/* Alarm Reset Key									*/
		UINT	scroll		:1;					/* Scroll Key										*/
		UINT	read		:1;					/* Read Key											*/
		UINT	write		:1;					/* Write Key										*/
		UINT	rst			:1;					/* Up+Down同時押下									*/
} OPKEYFLG;
/*--------------------------------------------------------------------------------------------------*/
/*		オペレータ点滅用フラグ定義																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct {
		UINT	PrmEewrite	:1;					/* Writing Parameter to EEPROM						*/
		UINT	read		:1;					/* Read												*/
		UINT	write		:1;					/* Write											*/
		UINT	no_op		:1;					/* No Operation										*/
		UINT	error		:1;					/* Error											*/
		UINT	done		:1;					/* Done												*/
		UINT	end			:1;					/* End												*/
} BLINKFLG;
/*--------------------------------------------------------------------------------------------------*/
/*		LED表示データ																				*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct {
		UCHAR	LedCode[5];						/* 表示するコード									*/
		UCHAR	LedBlnkSw[5];					/* 1の部分がブリンク対象							*/
} LEDDSP;
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		Operation modes (for Lcd.OpMode)															*/
/*--------------------------------------------------------------------------------------------------*/
enum _LCDOM_DISPSTATE {
		LCDOM_StartDisp = 0,		/* 0:スタート画面表示中											*/
		LCDOM_PrmMon,				/* 1:PRM/MON画面モード											*/
		LCDOM_Fun,					/* 2:補助機能画面モード											*/
		LCDOM_SelectUnit,			/* 3:ユニット選択画面モード										*/
		LCDOM_SelectAxis,			/* 4:軸選択画面モード											*/
		LCDOM_Passwd,				/* 5:パスワード入力画面モード									*/
		LCDOM_Error,				/* 6:エラー状態(無応答)											*/
		LCDOM_PrmCopy,				/* 7:<S031> パラメータコピー									*/
		LCDOM_DISPNUM
};

/*--------------------------------------------------------------------------------------------------*/
/*		LCD Op ブリンクスイッチ: TRUEの間ブリンクする												*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct {
	UINT	read	:1;				/* Readキー LED用												*/
	UINT	write	:1;				/* Writeキー LED用												*/
} LCDBLNKSW;

/*--------------------------------------------------------------------------------------------------*/
/*		表示情報テーブル																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct {
	USHORT	PnUn;					/* Pn/Un種類	0 = Un, 1 = Pn									*/
	USHORT	UnIdx;					/* Unインデックス PnUN == Un時のみ有効 <ROMテーブルのみUn番号>	*/
	USHORT	PnIdx;					/* Pnインデックス PnUN == Pn時のみ有効 <ROMテーブルのみPn番号>	*/
}	LOPDIT;

/*--------------------------------------------------------------------------------------------------*/
/*		LCD Operator DispInfo Variables																*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct {
	USHORT	CurFunIndx;				/* 補助機能現在位置インデックス									*/
	LOPDIT	InfTbl[4][4];			/* 表示情報テーブル(PnXxx/UnXxx)								*/
} LOPDSP;

/*--------------------------------------------------------------------------------------------------*/
/*		LCD Operator Drive Selection Variables	<0BJ14>												*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct {
	UCHAR	DispNumber;				/* Display Number												*/
	UCHAR	DispCurIdx;				/* Display Current Index										*/
	UCHAR	DispTopIdx;				/* Display TopLine Index										*/
	UCHAR	Spare00001;				/* Spare														*/
/*--------------------------------------------------------------------------------------------------*/
	struct	{						/* Display Drive Data											*/
		UCHAR	Type;				/* Unit Type (Converter, Drive)									*/
		UCHAR	Address;			/* Unit Address		 											*/
		UCHAR	AxisNumber;			/* Drive Axis Number											*/
		UCHAR	DispCurAxis;		/* Display Current Index										*/
/*--------------------------------------------------------------------------------------------------*/
		struct {
			UCHAR	Type;			/* Drive Type (Converter, DriveAxis)							*/
			UCHAR	AxisNo;			/* Drive Axis Number											*/
			USHORT	AlmCode;		/* Drive Alarm Code												*/
		} Axis[DRVSEL_MAXAXISPERUNIT];
/*--------------------------------------------------------------------------------------------------*/
	} Unit[DRVSEL_MAXUNIT];
/*--------------------------------------------------------------------------------------------------*/
	ADJUST_STRUCT_ALIGN( Dummy )	/* Adjust HEW Structure to Long Align							*/
} DRVSEL;

/*--------------------------------------------------------------------------------------------------*/
/*		LCD Operator Common Variables																*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct {
	UCHAR	CurMsgCmd;				/* 今回メッセージコマンド <0BJ14>								*/
	UCHAR	DrvSelDispMode;			/* ドライブ選択画面モード <0BJ14> 								*/
	USHORT	PasswdIdx;				/* Fn010のindex <0BJ14>											*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	Online;					/* LCDオペレータ接続状態										*/
	USHORT	OpMode;					/* LCDオペレータ動作状態										*/
	ULONG	StDispTime;				/* 起動時表示計測用												*/
	USHORT	RawKeyLast;				/* キーデータ前回値												*/
	USHORT	KeyHoldCnt;				/* キーホールド・リピート用カウント								*/
	USHORT	ActiveLine;				/* アクティブ行													*/
	USHORT	EditMode;				/* 編集モード													*/
	USHORT	DataDigit;				/* データ桁数													*/
	USHORT	CurDigit;				/* 現在編集位置													*/
	USHORT	EewriteMode;			/* EEPROM書込み状態												*/
	USHORT	FunMode;				/* LCDオペレータ補助機能状態									*/
/*--------------------------------------------------------------------------------------------------*/
	LCDBLNKSW  LedBlnkSw;			/* ブリンクスイッチ												*/
	DRVSEL	*pDrvSel;				/* ドライブ選択データポインタ									*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT  PrmInfoGetDone;			/* <S031> Parameter Information Get Complete					*/
/*--------------------------------------------------------------------------------------------------*/
} LOPV;



/****************************************************************************************************/
/*																									*/
/*		Operator I/F Functions 																		*/
/*																									*/
/****************************************************************************************************/
LONG	RpxLcdOpMain(						/* Lcd Operator Main Program							*/
		MSGIF	*hMsg,						/* Message Interface									*/
		DRVSEL	*pDrvSel	);				/* Drive Selection Data for Multi-Axis System			*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxLcdOpFinish( MSGIF *hMsg );		/* Lcd Operator Finish Procedure						*/
/*--------------------------------------------------------------------------------------------------*/
//void	RpxLcdOpInitDispInfo( void );		/* Lcd Operator Init. Display Info. Table				*/
void	RpxLcdOpInitDispInfo(  AXIS_HANDLE* );	/* Lcd Operator Init. Display Info. Table  <S213> 	*/

/*--------------------------------------------------------------------------------------------------*/
/*		ＬＣＤオペレータ内部関数																	*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxLcdClrTxBuf(						/* Clear ResBuf(TxBuf)									*/
		UCHAR	*ResBuf			);			/* 応答バッファアドレス									*/
/*--------------------------------------------------------------------------------------------------*/
USHORT	RpxLcdPnUnEditSub(					/* Pn/Un Edit Mode Procedure							*/
		USHORT	dispID,						/* ディスプレーID(LCDPMDSP_PM/JOG/etc)					*/
		OPKEYFLG *pKeyMk,					/* キー押下情報のアドレス								*/
		BLINKFLG *pBlnkKickFlg	);			/* ブリンク起動フラグポインター							*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxLcdMkDspDataPnUnLines(			/* Make Display Data for Pn/Un Lines					*/
		USHORT	dispID,						/* ディスプレーID(LCDPMDSP_PM/JOG/etc)					*/
		USHORT	BlnkSupFlg,					/* ブリンク抑止フラグ									*/
		UCHAR	*ResBuf			);			/* 応答バッファアドレス									*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxLcdRstPnUnDspData( void );		/* Reset Pn/UN Display Data 							*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxLcdSetPnUnData(					/* Pn/Un Display Data Set								*/
		USHORT	Lpos,						/* 開始行位置(最上行が0)								*/
		USHORT	Dpos,						/* 開始桁位置(左端が0)									*/
		ULONG	Data,						/* 表示データ											*/
		USHORT	IsLong,						/* ロングパラメータ時TRUE								*/
		USHORT	Base,						/* 底(表示属性)											*/
		USHORT	Sign,						/* 符号有無(表示属性)									*/
		SHORT	Decimals,					/* 小数点以下桁数(表示属性)								*/
		UCHAR	*ResBuf			);			/* 応答バッファアドレス									*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxLcdSetSignedDecData(				/* Lcd Operator 符号付10進数表示						*/
		USHORT	Lpos,						/* 開始行位置(最上行が0)								*/
		USHORT	Dpos,						/* 開始桁位置(左端が0)									*/
		LONG	Data,						/* 表示データ											*/
		USHORT	Digit,						/* 表示する桁数(小数点含まず)							*/
		USHORT	SupFlg,						/* TRUE=上位桁が0の場合、'0'を表示しない				*/
		SHORT	Decimals,					/* 小数点以下桁数(正の場合のみ有効)						*/
		UCHAR	*ResBuf			);			/* 応答バッファアドレス									*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxLcdSetStr(						/* Lcd Operator String Set								*/
		USHORT	Lpos,						/* 開始行位置(最上行が0)								*/
		USHORT	Dpos,						/* 開始桁位置(左端が0)									*/
		UCHAR	*pStr,						/* 表示する文字列アドレス								*/
		UCHAR	*ResBuf			);			/* 応答バッファアドレス									*/
/*--------------------------------------------------------------------------------------------------*/
LONG	RpxLcdSetSignChar(					/* Lcd Operator Sign Char Set							*/
		USHORT	Lpos,						/* 開始行位置(最上行が0)								*/
		USHORT	Dpos,						/* 開始桁位置(左端が0)									*/
		LONG	Data,						/* 表示するデータ										*/
		UCHAR	*ResBuf			);			/* 応答バッファアドレス									*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxLcdSetDecPnt(					/* Lcd Operator Decimal w/point Set						*/
		USHORT	Lpos,						/* 開始行位置(最上行が0)								*/
		USHORT	Dpos,						/* 開始桁位置(左端が0)									*/
		ULONG	Data,						/* 表示データ											*/
		USHORT	Digit,						/* 表示する桁数(小数点含まず)							*/
		USHORT	SupFlg,						/* TRUE=上位桁が0の場合、'0'を表示しない				*/
		SHORT	Decimals,					/* 小数点以下桁数(正の場合のみ有効)						*/
		UCHAR	*ResBuf			);			/* 応答バッファアドレス									*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxLcdSetValHex(					/* Lcd Operator HEX Set									*/
		USHORT	Lpos,						/* 開始行位置(最上行が0)								*/
		USHORT	Dpos,						/* 開始桁位置(左端が0)									*/
		ULONG	Data,						/* 表示データ											*/
		USHORT	Digit,						/* 表示する桁数											*/
		UCHAR	*ResBuf			);			/* 応答バッファアドレス									*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxLcdSetBit(						/* Lcd Operator BIT Set									*/
		USHORT	Lpos,						/* 開始行位置(最上行が0)								*/
		USHORT	Dpos,						/* 開始桁位置(左端が0)									*/
		UCHAR	Data,						/* 表示データ											*/
		UCHAR	*ResBuf			);			/* 応答バッファアドレス									*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxLcdSetBlnk(						/* Lcd Operator LCD Blink Set							*/
		USHORT	Lpos,						/* 開始行位置(最上行が0)								*/
		USHORT	Dpos,						/* 開始桁位置(左端が0)									*/
		USHORT	Length,						/* 文字数												*/
		UCHAR	*ResBuf			);			/* 応答バッファアドレス									*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxLcdClrLineBlnk(					/* Lcd Operator LCD Blink Stop							*/
		USHORT	Lpos,						/* 開始行位置(最上行が0)								*/
		UCHAR	*ResBuf			);			/* 応答バッファアドレス									*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxLcdSetResStatus( UCHAR *ResBuf );/* <S031> Lcd Operator Set Status						*/
void 	RpxLcdSetBlink (UCHAR *ResBuf);		/* <S031> Lcd Set Blink									*/


/*--------------------------------------------------------------------------------------------------*/
/*		<S031> ＬＣＤパラメータコピーＩＦ関数														*/
/*--------------------------------------------------------------------------------------------------*/
#if	( SVFSEL_LCDPRMCOPY_USE == 1 )
void	RpxLcdPrmCopyInit( void );			/* パラメータコピー機能初期化処理						*/
LONG	RpxLcdPrmInfoGet( void );			/* パラメータブロック情報取得処理						*/
void	RpxLcdPrmCopyMain( MSGIF *Hmsg, DRVSEL *pDrvSel );	/* パラメータコピー機能メイン処理		*/
#define __LCD_PRM_COPY_MAIN(x)  (x)
#define RpxChkPrmCopyInfoGet( ) (LopV.PrmInfoGetDone)
#else
#define	RpxLcdPrmCopyInit( ) {;}
#define	RpxLcdPrmInfoGet( )  (TRUE)
#define	RpxLcdPrmCopyMain( ) {;}
#define __LCD_PRM_COPY_MAIN(x)  (LcdError)
#define RpxChkPrmCopyInfoGet( ) (TRUE)
#endif
/*------------------------------------------------------------------------------------------*///<S197>
/*		オペレータユーザー定数表示 強制変更関数（Pn00Bで表示しないパラメータに強制的にアクセスするため）			*/
/*------------------------------------------------------------------------------------------*/
void LpxPrmChangeOpeDspMode(void);			/* すべてのユーザー定数を表示						*/
void LpxPrmReChangeOpeDspMode(void);		/* 表示設定を元に戻す								*/

/****************************************************************************************************/
/*																									*/
/*		オペレータＩＦ 表示情報テーブル																*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
// Jupiter Converter
/*--------------------------------------------------------------------------------------------------*/
#define CINITDISP_PRMMON_JPCV  { \
		UnSelected, 0x000, 0x000,	/* Un, Un000, Pn000		PRM/MON						*/ \
		UnSelected, 0x007, 0x001,	/* Un, Un007, Pn001									*/ \
		UnSelected, 0x009, 0x002,	/* Un, Un009, Pn002									*/ \
		UnSelected, 0x00A, 0x00B	/* Un, Un00A, Pn00B									*/ \
	}
#define CINITDISP_ZSET_JPCV  { \
		UnSelected, 0x000, 0x000,	/* Un, Un000, Pn000		原点サーチ					*/ \
		UnSelected, 0x002, 0x001,	/* Un, Un002, Pn001									*/ \
		UnSelected, 0x003, 0x002,	/* Un, Un003, Pn002									*/ \
		UnSelected, 0x00D, 0x00B	/* Un, Un00D, Pn00B									*/ \
	}
#define  CINITDISP_JOG_JPCV  { \
		PnSelected, 0x000, 0x304,	/* Pn, Un000, Pn304		JOG							*/ \
		UnSelected, 0x000, 0x001,	/* Un, Un000, Pn001									*/ \
		UnSelected, 0x002, 0x002,	/* Un, Un002, Pn002									*/ \
		UnSelected, 0x00D, 0x00B	/* Un, Un00D, Pn00B									*/ \
	}
#define  CINITDISP_PJOG_JPCV  { \
		PnSelected, 0x000, 0x531,	/* Pn, Un000, Pn531		Program JOG					*/ \
		PnSelected, 0x007, 0x533,	/* Pn, Un007, Pn533									*/ \
		PnSelected, 0x00C, 0x534,	/* Pn, Un00C, Pn534									*/ \
		PnSelected, 0x00D, 0x536	/* Pn, Un00D, Pn536									*/ \
	}
/*--------------------------------------------------------------------------------------------------*/
// Jupiter Spindle Analog I/F
/*--------------------------------------------------------------------------------------------------*/
#define CINITDISP_PRMMON_JPAN  { \
		UnSelected, 0x000, 0x006,	/* Un, Un000, Pn006		PRM/MON						*/ \
		UnSelected, 0x002, 0x007,	/* Un, Un002, Pn007									*/ \
		UnSelected, 0x008, 0x008,	/* Un, Un008, Pn008									*/ \
		UnSelected, 0x00D, 0x00B	/* Un, Un00D, Pn00B									*/ \
	}
#define CINITDISP_ZSET_JPAN  { \
		UnSelected, 0x000, 0x006,	/* Un, Un000, Pn006		原点サーチ					*/ \
		UnSelected, 0x002, 0x007,	/* Un, Un002, Pn007									*/ \
		UnSelected, 0x003, 0x008,	/* Un, Un003, Pn008									*/ \
		UnSelected, 0x00D, 0x00B	/* Un, Un00D, Pn00B									*/ \
	}
#define CINITDISP_JOG_JPAN  { \
		PnSelected, 0x000, 0x304,	/* Pn, Un000, Pn304		JOG							*/ \
		UnSelected, 0x000, 0x007,	/* Un, Un000, Pn007									*/ \
		UnSelected, 0x002, 0x008,	/* Un, Un002, Pn008									*/ \
		UnSelected, 0x00D, 0x00B	/* Un, Un00D, Pn00B									*/ \
	}
#define CINITDISP_PJOG_JPAN  { \
		PnSelected, 0x000, 0x531,	/* Pn, Un000, Pn531		Program JOG					*/ \
		PnSelected, 0x007, 0x533,	/* Pn, Un007, Pn533									*/ \
		PnSelected, 0x00C, 0x534,	/* Pn, Un00C, Pn534									*/ \
		PnSelected, 0x00D, 0x536	/* Pn, Un00D, Pn536									*/ \
	}
/*--------------------------------------------------------------------------------------------------*/
// SGDV Standard (Rotary)
/*--------------------------------------------------------------------------------------------------*/
#define CINITDISP_PRMMON_SGDV  { \
		UnSelected, 0x000, 0x000,	/* Un, Un000, Pn000		PRM/MON						*/ \
		UnSelected, 0x002, 0x001,	/* Un, Un002, Pn001									*/ \
		UnSelected, 0x008, 0x002,	/* Un, Un008, Pn002									*/ \
		UnSelected, 0x00D, 0x00B	/* Un, Un00D, Pn00B									*/ \
	}
#define CINITDISP_ZSET_SGDV  { \
		UnSelected, 0x000, 0x000,	/* Un, Un000, Pn000		原点サーチ					*/ \
		UnSelected, 0x002, 0x001,	/* Un, Un002, Pn001									*/ \
		UnSelected, 0x003, 0x002,	/* Un, Un003, Pn002									*/ \
		UnSelected, 0x00D, 0x00B	/* Un, Un00D, Pn00B									*/ \
	}
#define  CINITDISP_JOG_SGDV  { \
		PnSelected, 0x000, 0x304,	/* Pn, Un000, Pn304		JOG							*/ \
		UnSelected, 0x000, 0x001,	/* Un, Un000, Pn001									*/ \
		UnSelected, 0x002, 0x002,	/* Un, Un002, Pn002									*/ \
		UnSelected, 0x00D, 0x00B	/* Un, Un00D, Pn00B									*/ \
	}
#define  CINITDISP_PJOG_SGDV  { \
		PnSelected, 0x000, 0x531,	/* Pn, Un000, Pn531		Program JOG					*/ \
		PnSelected, 0x007, 0x533,	/* Pn, Un007, Pn533									*/ \
		PnSelected, 0x00C, 0x534,	/* Pn, Un00C, Pn534									*/ \
		PnSelected, 0x00D, 0x536	/* Pn, Un00D, Pn536									*/ \
	}

/*--------------------------------------------------------------------------------------------------*/
// SGDV Standard (Linear)
/*--------------------------------------------------------------------------------------------------*/
#define CINITDISP_PRMMON_SGDV_L  { \
		UnSelected, 0x000, 0x000,	/* Un, Un000, Pn000		PRM/MON						*/ \
		UnSelected, 0x002, 0x001,	/* Un, Un002, Pn001									*/ \
		UnSelected, 0x008, 0x002,	/* Un, Un008, Pn002									*/ \
		UnSelected, 0x00D, 0x00B	/* Un, Un00D, Pn00B									*/ \
	}
#define CINITDISP_ZSET_SGDV_L  { \
		UnSelected, 0x000, 0x000,	/* Un, Un000, Pn000		原点サーチ					*/ \
		UnSelected, 0x002, 0x001,	/* Un, Un002, Pn001									*/ \
		UnSelected, 0x003, 0x002,	/* Un, Un003, Pn002									*/ \
		UnSelected, 0x00D, 0x00B	/* Un, Un00D, Pn00B									*/ \
	}
#define  CINITDISP_JOG_SGDV_L  { \
		PnSelected, 0x000, 0x383,	/* Pn, Un000, Pn383		JOG							*/ \
		UnSelected, 0x000, 0x001,	/* Un, Un000, Pn001									*/ \
		UnSelected, 0x002, 0x002,	/* Un, Un002, Pn002									*/ \
		UnSelected, 0x00D, 0x00B	/* Un, Un00D, Pn00B									*/ \
	}
#define  CINITDISP_PJOG_SGDV_L  { \
		PnSelected, 0x000, 0x531,	/* Pn, Un000, Pn531		Program JOG					*/ \
		PnSelected, 0x007, 0x585,	/* Pn, Un007, Pn585									*/ \
		PnSelected, 0x00C, 0x534,	/* Pn, Un00C, Pn534									*/ \
		PnSelected, 0x00D, 0x536	/* Pn, Un00D, Pn536									*/ \
	}

/*--------------------------------------------------------------------------------------------------*/
/*  Mercury Standard ?																				*/
/*--------------------------------------------------------------------------------------------------*/
#define CINITDISP_PRMMON  CINITDISP_PRMMON_SGDV
#define CINITDISP_ZSET    CINITDISP_ZSET_SGDV
#define CINITDISP_JOG     CINITDISP_JOG_SGDV
#define CINITDISP_PJOG    CINITDISP_PJOG_SGDV

/****************************************************************************************************/
/*																									*/
/*		オペレータＩＦ関数定義																		*/
/*																									*/
/****************************************************************************************************/
#if		CCSW_HWID == HWID_JPCV
void	LcdStartDisp( MSGIF *hMsg, DRVSEL *pDrvSel );	/* 0:初期画面表示							*/
void	LcdPrmMonMain( MSGIF *hMsg, DRVSEL *pDrvSel );	/* 1:パラメータ/モニタ画面					*/
void	LcdFun( MSGIF *hMsg, DRVSEL *pDrvSel );			/* 2:補助機能実行画面						*/
void	LcdSelectUnit( MSGIF *hMsg, DRVSEL *pDrvSel );	/* 3:ユニット選択画面						*/
void	LcdSelectAxis( MSGIF *hMsg, DRVSEL *pDrvSel );	/* 4:軸選択画面								*/
void	LcdPasswd( MSGIF *hMsg, DRVSEL *pDrvSel );		/* 5:パスワード入力画面モード				*/
void	LcdError( MSGIF *hMsg, DRVSEL *pDrvSel );		/* 6:エラー状態(無応答)						*/
#else
void	LcdStartDisp( MSGIF *hMsg, DRVSEL *pDrvSel );
void	LcdPrmMonMain( MSGIF *hMsg, DRVSEL *pDrvSel );
void	LcdFun( MSGIF *hMsg, DRVSEL *pDrvSel );
//#define	LcdSelectUnit LcdSelectDrive
#define LcdSelectUnit LcdError
void	LcdSelectAxis( MSGIF *hMsg, DRVSEL *pDrvSel );
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W（２軸対応）のみ	*//* <S0E4> */
void	LcdDspDatSelAxs( UINT ax_no );	/*	Lcd Display Data Select Axis	*/
#endif
#define	LcdPasswd LcdError
void	LcdError( MSGIF *hMsg, DRVSEL *pDrvSel );
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		CMD Option Function Definition																*/
/*--------------------------------------------------------------------------------------------------*/
#if CCSW_CMDID != CMDID_EXIST

#define	LcdOptGetNextOpPrmNo(x1, x2, x3, x4) (0)
#define	LcdOptGetNextSvPrmIdx(x1, x2, x3)    (0)
#define	LcdOptSetOpPrmReadReq(x1, x2, x3)    (0)
#define	LcdOptUpdateResponseData(x1, x2)     (0)
#define	LcdOptSetDataBlinkDPos(x1)  (0)
#define LCD_CHKCMD_RCVWAIT() ( TRUE )

#else

USHORT	LcdOptGetNextOpPrmNo(USHORT, USHORT, USHORT, DEGOP_PRM *);
USHORT	LcdOptGetNextSvPrmIdx(USHORT, USHORT, DEGOP_OPTCOM *);
LONG	LcdOptSetOpPrmReadReq(USHORT, USHORT, DEGOP_OPTCOM *);
LONG	LcdOptUpdateResponseData(USHORT, DEGOP_OPTCOM *);
USHORT	LcdOptSetDataBlinkDPos(DEGOP_PRM *);
#define LCD_CHKCMD_RCVWAIT() ( LcdOpt[LCDPMDSP_PM].Prm[LopV.ActiveLine].State.RcvWait != TRUE )

#endif

/*--------------------------------------------------------------------------------------------------*/
/*		初期表示																					*/
/*--------------------------------------------------------------------------------------------------*/
#if		CCSW_HWID == HWID_JPCV
#define CSTR_INITDSP_L1 "Unit Detecting"
#define CSTR_INITDSP_L2 "Please Wait..."
#else
#define CSTR_INITDSP_L1 "Initializing"
#define CSTR_INITDSP_L2 "Please Wait..."
#endif
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W（２軸対応）のみ	*//* <S0E4> */
#define CSTR_SELAXS_TITLE "-AXIS SELECTION-"	/*	軸選択画面１行目のタイトル表示	*/
#elif CSW_BOARD_TYPE == SGD7S_TYPE	/*	SGD7S（単軸時）のみ	*/
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		軸番号表示																					*/
/*--------------------------------------------------------------------------------------------------*/
#if		CCSW_HWID == HWID_JPCV
#define CCHR_AXISNUM 'C'
#else
#define CCHR_AXISNUM '1' /* Single Axis */
#endif

/* <S113> Start */
/*--------------------------------------------------------------------------------------------------*/
/*	オペレータのユーザー定数表示選択																*/
/*--------------------------------------------------------------------------------------------------*/
#define OPEDSP_SETUPPRM			0x0000		/* SETUP用ユーザ定義パラメータのみ						*/
#define OPEDSP_ALLPRM			0x0001		/* 全ユーザ定義パラメータ								*/
/* <S113> End */


/****************************************************************************************************/
/*																									*/
/*		パラメータＩＦ定義																			*/
/*																									*/
/****************************************************************************************************/
#if 0 /* SGDV */
/*--------------------------------------------------------------------------------------------------*/
USHORT	RpxMonSearchIdxOp(					/* モニタテーブルのインデックス検索処理					*/
		LONG	SrchNo,						/* モニタ番号											*/
		LONG	BaseIdx,					/* 検索開始インデックス									*/
		HMSGIF	Hmsg			);			/* メッセージIFハンドル									*/
/*--------------------------------------------------------------------------------------------------*/
LONG	RpxPrmSearchIdxOp(					/* パラメータテーブルのインデックス検索処理				*/
		LONG	SrchNo,						/* パラメータ番号										*/
		LONG	BaseIndex,					/* 検索開始インデックス									*/
		HMSGIF	Hmsg			);			/* IFハンドル											*/
/*--------------------------------------------------------------------------------------------------*/
#define CHK_WRITEINHIBIT()  (Iprm.f.PrmWrtInhibit )
#else
//#define RpxMonSearchIdxOp( x1, x2, x3 ) (0) /* TODO */
//#define RpxPrmSearchIdxOp( x1, x2, x3 ) (0) /* TODO */

#define CHK_WRITEINHIBIT()  (pAxRsc->RegManager->hPnReg->WriteAccessLock)	/* check write inhibit */
#endif

/****************************************************************************************************/
/*																									*/
/*		パネルオペレータＩＦ定義																	*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		パネルオペレータＩＦ関数																	*/
/*--------------------------------------------------------------------------------------------------*/
#if SVFSEL_PANELOP_USE == 0											/* Not Use Panel */
/*--------------------------------------------------------------------------------------------------*/
#define RpxPnlOpMain( x )							{ ;}
#define RpxPnlOpInit( )								{ ;}
#define RpxPnlOpMonInit( x )						{ ;}
#define RpxPnlOpFinish( x )							{ ;}
/*--------------------------------------------------------------------------------------------------*/
#define RpxLedSetStatusBits( x1, x2 )				{ ;}
#define RpxLedSetValDec( x1, x2, x3, x4, x5, x6 )	{ ;}
#define RpxLedSetValHex( x1, x2, x3, x4 ) 			{ ;}
#define RpxLedSetStr( x1, x2, x3, x4 )				{ ;}
#define RpxLedDispMonDecWord( x1, x2, x3, x4 )		{ ;}
#define RpxLedDispMonDecLong( x1, x2, x3, x4, x5 )	{ ;}
#define RpxLedSetBlnk( x1, x2 )						{ ;}
/*--------------------------------------------------------------------------------------------------*/
#else
/*--------------------------------------------------------------------------------------------------*/
void	RpxPnlOpMain( HMSGIF Hmsg );		/* Panel Operator Main Program							*/
void	RpxPnlOpInit( void );				/* Panel Operator Initialize							*/
void	RpxPnlOpMonInit( HMSGIF Hmsg );		/* Panel Monitor Initialize								*/
void	RpxPnlOpFinish( HMSGIF Hmsg );		/* Panel Operator Finish								*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxLedDispMonDecWord(				/* LEDオペレータ モニター表示(10進ワード符号有/無)		*/
		USHORT	data,						/* 表示する値											*/
		LEDDSP	*leddspp,					/* LED表示データのアドレス								*/
		USHORT	decimals,					/* 小数点以下桁数										*/
		USHORT	sign			);			/* 符号の有無											*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxLedDispMonDecLong(				/* LEDオペレータ モニター表示(10進ロング符号有/無)		*/
		ULONG	ldata,						/* ldata:表示する値										*/
		LEDDSP	*leddspp,					/* LED表示データのアドレス								*/
		USHORT	dsp_pos,					/* 現在表示位置											*/
		USHORT	decimals,					/* 小数点以下桁数										*/
		USHORT	sign			);			/* 符号有無												*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxLedSetStatusBits(				/* LEDオペレータ ステータスビット表示情報セット処理		*/
		LEDDSP	*leddspp,					/* LED表示データのアドレス								*/
		HMSGIF	Hmsg );						/* 後で消す */
/*--------------------------------------------------------------------------------------------------*/
void	RpxLedSetValDec(					/* LEDオペレータ 表示値(10進)セット処理					*/
		USHORT	data,						/* 表示する値											*/
		LEDDSP	*leddspp,					/* LED表示データのアドレス								*/
		USHORT	startdgt,					/* 開始桁												*/
		USHORT	digit,						/* 処理桁数												*/
		USHORT	sup_flg,					/* 0表示抑止フラグ										*/
		USHORT	decimals		);			/* 小数点以下桁数										*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxLedSetValHex(					/* LEDオペレータ 表示値(16進)セット処理					*/
		USHORT	data,						/* 表示する値											*/
		LEDDSP	*leddspp,					/* LED表示データのアドレス								*/
		USHORT	startdgt,					/* 開始桁												*/
		USHORT	digit			);			/* 処理桁数												*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxLedSetStr(						/* LED Operator String Set								*/
		UCHAR	*str_ptr,					/* 表示する文字列アドレス								*/
		LEDDSP	*leddspp,					/* LED表示データのアドレス								*/
		USHORT	startdgt,					/* 開始桁												*/
		USHORT	length			);			/* 表示文字数											*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxLedSetBlnk(						/* LED Operator Blink Set								*/
		LEDDSP	*leddspp,					/* LED表示データのアドレス								*/
		UCHAR	blnkptn			);			/* 点滅パターン(1bit=1文字分)							*/
/*--------------------------------------------------------------------------------------------------*/
#endif

/****************************************************************************************************/
/*																									*/
/*		オペレータＩＦ メッセージ転送																*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		LCD Operator Dummy Message Declaration														*/
/*--------------------------------------------------------------------------------------------------*/
#if		CCSW_HWID == HWID_JPCV
	#define DECL_DUMMY_MSG()            \
		const UCHAR dummyMsg[] = {      \
			0x00, /* Slave Address */   \
			0x60, /* Function code */   \
			0x08, /* Message length */  \
			0x00, /* Message length */  \
			0x00, /* Key information */ \
			0x00, /* Key information */ \
			0xF8, /* check sum */       \
			0x9F  /* check sum */       \
		}
#else
	#define DECL_DUMMY_MSG() /* nothing */ 
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		LCD Operator Message Transfar Function 														*/
/*--------------------------------------------------------------------------------------------------*/
#if		CCSW_HWID == HWID_JPCV
	#define LCD_TRANSMESSAGE() { \
		LONG Axis = pDrvSel->Unit[ pDrvSel->DispCurIdx ].DispCurAxis + 1; \
		KriAddLbusMsgQueue ( LMQUEID_MSGTRANSFER, \
							 (Axis << 16) | pDrvSel->Unit[ pDrvSel->DispCurIdx ].Address, \
							 sizeof(dummyMsg), \
							 dummyMsg ); \
		KpiRstLongTimer (&LopV.StDispTime); /* For timeout measure */  \
	}
#else
	#define LCD_TRANSMESSAGE() {;}
#endif

/****************************************************************************************************/
/*																									*/
/*		オペレータＩＦ変数定義																		*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		LCD Operator Axis Variables																	*/
/*--------------------------------------------------------------------------------------------------*/
extern	LOPDSP	LopDsp;						/*														*/
extern	LCDEEP	LcdEep[MAX_AXIS_NUMBER];	/* ＬＣＤオペレータ：表示EEPROM情報				 <S208> */
extern	LCDEEP	*pLcdEep;					/* ＬＣＤ表示情報ポインタ						 <S208> */
extern	USHORT	LcdWtReq;					/* ＬＣＤ表示情報EEPROM書き込み要求				 <S208> */
extern	USHORT	LcdWtCnt;					/* ＬＣＤ表示情報書き込み中カウンタフラグ		 <S208> */
extern	ULONG	LcdRomAdr;					/* ＬＣＤ表示情報書き込みEEPROMアドレス			 <S208> */
extern	ULONG	LcdRomBaseAdr;				/* ＬＣＤ表示情報書き込みEEPROMアドレス			 <S208> */
extern	USHORT	*LcdMemAdr;					/* ＬＣＤ表示情報書き込みメモリーアドレス		 <S208> */
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W（２軸対応）のみ	*//* <S0E4> */
extern	DRVSEL	DrvSel;					/* ＬＣＤオペレータ：ドライブ（軸）選択	*/
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		LCD Operator Common Variables																*/
/*--------------------------------------------------------------------------------------------------*/
extern	LOPV		LopV;					/*														*/
extern	OPKEYFLG	OpKeyMk;				/*														*/
extern	OPKEYFLG	OpKeyBrk;				/*														*/
extern	OPKEYFLG	OpKeyLvl;				/*														*/
extern	BLINKFLG	OpBlnkFlg;				/*														*/
extern	BLINKFLG	OpBlnkKickFlg;			/*														*/
extern	ULONG		OpBlnkTime;				/*														*/
extern  FUNMSG		OpFunMsg;				/*														*/

extern	FUN_MANAGER	FnManagerSto;			/*														*/
#endif
/***************************************** end of file **********************************************/
