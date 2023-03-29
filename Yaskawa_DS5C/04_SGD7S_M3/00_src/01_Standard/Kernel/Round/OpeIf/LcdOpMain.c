/****************************************************************************************************/
/*																									*/
/*																									*/
/*		LcdOpMain.c : LCDオペレータ処理モジュールメイン												*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*	機 能 : SigmaMessageによりＬＣＤオペレータと通信し、オペレータからの各種操作を実行する。		*/
/*																									*/
/*			1)モニタ機能 : 指定モニタ変数(UnXXX)の表示												*/
/*			2)パラメータ : 指定パラメータ(PnXXX)の表示,編集,変更									*/
/*			3)補助機能	 : 指定補助機能(FnXXX)の実行と実行結果の表示								*/
/*			4)ParamCopy  : オペレータ内蔵EEPROMを使用したユーザパラメータの保存と復元				*/
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
/*		Rev.1.00 : 2002.11.14  M.Suga																*/
/*		Rev.1.10 : 2008.04.11  T.Taniguchi  Under Modification										*/
/*		Rev.1.20 : 2011.03.19  K.Ozaki      Adaptation of Converter LCD Operator <0BJ14>			*/
/*		Rev.1.30 : 2012.04.xx  K.Ozaki      Revised for Mercury 									*/
/*		Rev.1.40 : 2013.10.24  K.Ozaki      <S031> Parameter Copy 									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "KnlApi.h"			/* Kernel API */
#include "LcdOp.h"			/* LcdOpIf Common Header */
#include "Monitor.h"		/* Un Monitor */
#include "Parameter.h"		/* Parameter */
#include "Function.h"		/* Fn Funstion */


/****************************************************************************************************/
/*																									*/
/*		Lcd Operator Constant Definition															*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		LCDオペレータにロードするフォント															*/
/*--------------------------------------------------------------------------------------------------*/
const	UCHAR	LcdExtFont[] = {				/* LCDオペレータにロードするフォント				*/
		0xe0, 0xe0, 0xe0, 0xe0, 0xe0,			/* Code 0x10	ビット(0)							*/
		0x07, 0x07, 0x07, 0x07, 0x07,			/* Code 0x11	ビット(1)							*/
/*--------------------------------------------------------------------------------------------------*/
		0x24, 0x64, 0xe7, 0x26, 0x24,			/* Code 0x12	Sample character					*/
		0x97, 0x49, 0x5b, 0x0e, 0x00,			/* Code 0x13	Sample character					*/
/*--------------------------------------------------------------------------------------------------*/
		0x0e, 0x5b, 0x49, 0x57, 0xcf,			/* Code 0x14	Sample character					*/
		0x57, 0x49, 0x5b, 0x0e, 0x00,			/* Code 0x15	Sample character					*/
/*--------------------------------------------------------------------------------------------------*/
		0x00, 0xff, 0x00, 0xff, 0x00,			/* Code 0x16										*/
		0x00, 0xff, 0x00, 0xff, 0x00,			/* Code 0x17										*/
		0x00, 0xff, 0x00, 0xff, 0x00,			/* Code 0x18										*/
		0x00, 0xff, 0x00, 0xff, 0x00,			/* Code 0x19										*/
		0x00, 0xff, 0x00, 0xff, 0x00,			/* Code 0x1A										*/
		0x00, 0xff, 0x00, 0xff, 0x00,			/* Code 0x1B										*/
		0x00, 0xff, 0x00, 0xff, 0x00,			/* Code 0x1C										*/
		0x00, 0xff, 0x00, 0xff, 0x00,			/* Code 0x1D										*/
		0x00, 0xff, 0x00, 0xff, 0x00,			/* Code 0x1E										*/
		0x1c, 0x3e, 0x3e, 0x3e, 0x1c			/* Code 0x1F										*/
};
/*--------------------------------------------------------------------------------------------------*/
/*		Display Information Definition																*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct {
		LOPDIT	DspDef[4];
} LOPDITDEF;
typedef const LOPDITDEF CLOPDITDEF;
/*--------------------------------------------------------------------------------------------------*/
/*		Initial Display for Rotary Axis																*/
/*--------------------------------------------------------------------------------------------------*/
const	LOPDITDEF	LopDitDef_Rotary[] = {
		CINITDISP_PRMMON,						/* PRM/MON */
		CINITDISP_ZSET,                         /* ZSET */
		CINITDISP_JOG,                          /* JOG */
		CINITDISP_PJOG                          /* PJOG */
};
/*--------------------------------------------------------------------------------------------------*/
/*		Initial Display for Linear Axis																*/
/*--------------------------------------------------------------------------------------------------*/
const	LOPDITDEF	LopDitDef_Linear[] = {
		CINITDISP_PRMMON_SGDV_L,
		CINITDISP_ZSET_SGDV_L,
		CINITDISP_JOG_SGDV_L,
		CINITDISP_PJOG_SGDV_L
};

/*--------------------------------------------------------------------------------------------------*/
/*		LCD Operator Dummy Message Declaration														*/
/*--------------------------------------------------------------------------------------------------*/
DECL_DUMMY_MSG();

/*<11J42>追加ここから*/
#if CCSW_CMDID == CMDID_EXIST	
const DEGOP_PRMRNG	LcdOptPrmLmtTbl[4][4] =	/* 指令オプションパラメータ編集用最大上下限値テーブル */
{
	{	/* ニブル表示	*/
		{0xFFFF, 0x0000},			/* 最大上限値, 最大下限値（ワード符号無し）	*/
		{0xFFFF, 0x0000},			/* 最大上限値, 最大下限値（ワード符号付き）	*/
		{0xFFFFFFFF, 0x00000000},	/* 最大上限値, 最大下限値（ロング符号無し）	*/
		{0xFFFFFFFF, 0x00000000}	/* 最大上限値, 最大下限値（ロング符号付き）	*/
	},

	{	/* 10進表示	*/
		{0xFFFF, 0x0000},			/* 最大上限値, 最大下限値（ワード符号無し）	*/
		{0x7FFF, 0xFFFF8000},		/* 最大上限値, 最大下限値（ワード符号付き）	*/
		{0xFFFFFFFF, 0x00000000},	/* 最大上限値, 最大下限値（ロング符号無し）	*/
		{0x7FFFFFFF, 0x80000000}	/* 最大上限値, 最大下限値（ロング符号付き）	*/
	},

	{	/* 16進表示	*/
		{0xFFFF, 0x0000},			/* 最大上限値, 最大下限値（ワード符号無し）	*/
		{0x7FFF, 0xFFFF8000},		/* 最大上限値, 最大下限値（ワード符号付き）	*/
		{0xFFFFFFFF, 0x00000000},	/* 最大上限値, 最大下限値（ロング符号無し）	*/
		{0x7FFFFFFF, 0x80000000}	/* 最大上限値, 最大下限値（ロング符号付き）	*/
	},

	{	/* ビット表示（未サポート）	*/
		{0x00FF, 0x0000},			/* 最大上限値, 最大下限値（ワード符号無し）	*/
		{0x00FF, 0x0000},			/* 最大上限値, 最大下限値（ワード符号付き）	*/
		{0x000000FF, 0x00000000},	/* 最大上限値, 最大下限値（ロング符号無し）	*/
		{0x000000FF, 0x00000000}	/* 最大上限値, 最大下限値（ロング符号付き）	*/
	}
};
#endif
/*<11J42>追加ここまで*/

/****************************************************************************************************/
/*																									*/
/*		Lcd Operator Variable Definition															*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		LCD Operator DispInfo Variables																*/
/*--------------------------------------------------------------------------------------------------*/
LOPDSP		LopDsp;						/* ＬＣＤオペレータ：表示情報								*/
LCDEEP		LcdEep[MAX_AXIS_NUMBER];	/* ＬＣＤオペレータ：表示EEPROM情報					 <S208> */
LCDEEP		*pLcdEep;					/* ＬＣＤ表示情報ポインタ							 <S208> */
USHORT		LcdWtReq;					/* ＬＣＤ表示情報EEPROM書き込み要求					 <S208> */
USHORT		LcdWtCnt;					/* ＬＣＤ表示情報書き込み中カウンタフラグ			 <S208> */
ULONG		LcdRomAdr;					/* ＬＣＤ表示情報書き込みEEPROMアドレス				 <S208> */
ULONG		LcdRomBaseAdr;				/* ＬＣＤ表示情報書き込みEEPROMアドレス				 <S208> */
USHORT		*LcdMemAdr;					/* ＬＣＤ表示情報書き込みメモリーアドレス			 <S208> */

//TODO : minaosi
//#if CCSW_CMDID == CMDID_EXIST			/*<11J42>*/
//DEGOP_OPTCOM	LcdOpt[LCDPMDSP_MAX];
//#endif
//#pragma section sdata end
/*--------------------------------------------------------------------------------------------------*/
/*		LCD Operator Common Variables																*/
/*--------------------------------------------------------------------------------------------------*/
extern MSGIF LopMsg;					/* TODO: 消すLcdOpe：Message I/F							*/

/*--------------------------------------------------------------------------------------------------*/
LOPV		LopV;						/* ＬＣＤオペレータ：共通変数								*/
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W（2軸対応）のみ	*//* <S0E4> */
DRVSEL		DrvSel;						/* ＬＣＤオペレータ：ドライブ（軸）選択	構造体						*/
#endif
/*--------------------------------------------------------------------------------------------------*/
OPKEYFLG	OpKeyMk;					/* キーMAKE(OFF->ON)フラグ									*/
OPKEYFLG	OpKeyBrk;					/* キーBREAK(ON->OFF)フラグ									*/
OPKEYFLG	OpKeyLvl;					/* キーレベルフラグ											*/
OPKEYFLG	OpKeyHld;					/* キー長押しフラグ											*/
/*--------------------------------------------------------------------------------------------------*/
BLINKFLG	OpBlnkFlg;					/* ブリンクフラグ											*/
BLINKFLG	OpBlnkKickFlg;				/* ブリンク起動フラグ										*/
ULONG		OpBlnkTime;					/* ブリンクフラグブリンク時間測定用							*/
/*--------------------------------------------------------------------------------------------------*/
FUN_MANAGER FnManagerSto;				/* 補助機能継続用指令保存									*/
/*--------------------------------------------------------------------------------------------------*/
AXIS_HANDLE	*pAxRsc;					/* TODO: あとで消す 										*/
/*--------------------------------------------------------------------------------------------------*/


// TODO: minaosi
#if CCSW_CMDID == CMDID_EXIST			/*<11J42>*/
DEGOP_OPTCOM	LcdOpt[LCDPMDSP_MAX];
BLINKFLG	   *LcdOptBlnkKick;
#endif
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		Function List																				*/
/*																									*/
/****************************************************************************************************/
LONG	RpxLcdOpMain(						/* Lcd Operator Main Program							*/
		MSGIF	*hMsg,						/* Message Handle 										*/
		DRVSEL	*pDrvSel		);			/* Drive Selection Data for Multi-Axis System			*/
#if		CCSW_HWID == HWID_JPCV
LONG	RpxLcdOpRemoteMst(					/* Lcd Operator Main Program for Remote Operation(Mst)	*/
		DRVSEL	*pDrvSel		);			/* Drive Selection Data for Multi-Axis System			*/
#endif
/*--------------------------------------------------------------------------------------------------*/
void	RpxLcdOpFinish( MSGIF *hMsg );		/* Lcd Operator Finish Procedure						*/
/*--------------------------------------------------------------------------------------------------*/
//void	RpxLcdOpInitDispInfo( void );		/* Lcd Operator Init. Display Info. Table				*/
void	RpxLcdOpInitDispInfo(  AXIS_HANDLE* );	/* Lcd Operator Init. Display Info. Table	 <S213> */
/*--------------------------------------------------------------------------------------------------*/
void	RpxLcdClrTxBuf(						/* Clear ResBuf(TxBuf)									*/
		UCHAR	*ResBuf			);			/* 応答バッファアドレス									*/
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
/*		Local Functions																				*/
/*--------------------------------------------------------------------------------------------------*/
void	LcdOpInit( MSGIF *hMsg );
void	LcdVarInit( void );
/*--------------------------------------------------------------------------------------------------*/
void	LcdKeyTranslate( MSGIF *hMsg );
/*--------------------------------------------------------------------------------------------------*/
//void	LcdInitDispInfoTable( void );
void	LcdInitDispInfoTable( AXIS_HANDLE* );	/* <S213> */
SHORT	LcdSaveDispInfoTable( USHORT dispID );
/*--------------------------------------------------------------------------------------------------*/
void	LcdResSend( MSGIF *hMsg );
void	RpxLcdSetResStatus( UCHAR *ResBuf );					/* <S031> */
void	LcdSetResLedSts( USHORT *pLedSts );
void	LcdSetResBlink( UCHAR *ResBuf, USHORT *pLedSts );
/*--------------------------------------------------------------------------------------------------*/
void 	RpxLcdSetBlink (UCHAR *ResBuf);							/* <S031> */
/*--------------------------------------------------------------------------------------------------*/
LONG	LcdModeChg ( DRVSEL *pDrvSel );
/*--------------------------------------------------------------------------------------------------*/
//static LONG LpxStartDispModeCv( DRVSEL *pDrvSel, LONG *DrvSelEnd );			/* <S031> Lcd Prm Copy 	*/
static LONG LpxStartDispModeSv( DRVSEL *pDrvSel );							/* <S031> Lcd Prm Copy 	*/
void	LopDspDataLoad( LONG AxisNo );										/* <S208>				*/

/*--------------------------------------------------------------------------------------------------*/
/*		LCDオペレータ処理実行テーブル 																*/
/*--------------------------------------------------------------------------------------------------*/
void (* const LcdOpeProcTable[LCDOM_DISPNUM])(MSGIF *hMsg, DRVSEL *pDrvSel) = {
	LcdStartDisp,							/* 0:初期画面表示										*/
	LcdPrmMonMain,							/* 1:パラメータ/モニタ画面								*/
	LcdFun, 								/* 2:補助機能実行画面									*/
	LcdSelectUnit,							/* 3:ユニット選択画面									*/
	LcdSelectAxis,							/* 4:軸選択画面											*/
	LcdPasswd,								/* 5:パスワード入力画面モード							*/
	LcdError,								/* 6:エラー状態(無応答)									*/
__LCD_PRM_COPY_MAIN( RpxLcdPrmCopyMain ),	/* 7:<S031> パラメータコピー画面						*/
};


/****************************************************************************************************/
/*																									*/
/*		Lcd Operator Main Program																	*/
/*																									*/
/****************************************************************************************************/
LONG	RpxLcdOpMain( MSGIF *hMsg, DRVSEL *pDrvSel )
{
LONG	DrvSelEnd = FALSE;
UCHAR	*CmdBuf = hMsg->CmdBuf;
CSIF 	*Sif = hMsg->Sif;
/*--------------------------------------------------------------------------------------------------*/
/* <S208> */
	if( (LONG)pLcdEep != (LONG)( &LcdEep[ hMsg->pAxRsc->AxisNo ] ) )
	{
		LopDspDataLoad( hMsg->pAxRsc->AxisNo );
	}
/* <S208> */
/*--------------------------------------------------------------------------------------------------*/
	pAxRsc = hMsg->pAxRsc; /* あとで消す */	
/*--------------------------------------------------------------------------------------------------*/
	switch ( CmdBuf[1] )
	{
/*--------------------------------------------------------------------------------------------------*/
		case LCD_CMD_INIT: /*		コマンド : 0x64													*/
/*--------------------------------------------------------------------------------------------------*/
			CLCDFUN->Finish( CLCDFUN, hMsg );					/* Finish Executing Function */
			LcdOpInit( hMsg );
			LopV.CurMsgCmd = CmdBuf[1];							/* <S031> Current Command */
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W（2軸対応）のみ	*//* <S0E4> */
			/* 	ＬＣＤオンライン1スタート	*/
			LopV.Online = LCD_ONLINE1;							/* 0x64コマンド受信で、ONLINE1 とする	*/
#endif
			break;
/*--------------------------------------------------------------------------------------------------*/
		case LCD_CMD_NOR:  /*		コマンド : 0x60													*/
		case LCD_CMD_LOAD: /*		<S031> コマンド : 0x61											*/
		case LCD_CMD_SAVE: /*		<S031> コマンド : 0x62											*/
/*--------------------------------------------------------------------------------------------------*/
			LopV.CurMsgCmd = CmdBuf[1];							/* <S031> Current Command */
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W（2軸対応）のみ	*//* <S0E4> */
			if( LopV.Online == LCD_ONLINE1 )
			{
				LcdVarInit( );
				/*	現在表示中の軸番号初期値を1へ（とりあえず0でない場合に2軸と判定）	*/
				if(NumberOfAxes > 1)
				{	/*	2軸動作時の1軸選択でスタート	*/
					DrvSel.Unit[0].DispCurAxis = 1;
				}
				else
				{	/*	1軸動作時	*/
					DrvSel.Unit[0].DispCurAxis = 0;
				}
				/*	現在表示軸データの選択	*/
				LcdDspDatSelAxs( DrvSel.Unit[0].DispCurAxis );
			}
#else /* <S0E4> */
			if( LopV.Online == LCD_OFFLINE )
			{
				LcdVarInit( );
			}
#endif /* <S0E4> */
			LopV.Online = LCD_ONLINE2;
	/*----------------------------------------------------------------------------------------------*/
	/*			Key Input																			*/
	/*----------------------------------------------------------------------------------------------*/
			LcdKeyTranslate( hMsg );
	/*----------------------------------------------------------------------------------------------*/
	/*			Alarm Reset 																		*/
	/*----------------------------------------------------------------------------------------------*/
			if( OpKeyMk.almrst )
			{
				KriResetSystemAlarm();
			}
	/*----------------------------------------------------------------------------------------------*/
	/*			Change Display Mode																	*/
	/*----------------------------------------------------------------------------------------------*/
			DrvSelEnd = LcdModeChg( pDrvSel );
	/*----------------------------------------------------------------------------------------------*/
	/*			Create Display Data 																*/
	/*----------------------------------------------------------------------------------------------*/
			LcdOpeProcTable[ LopV.OpMode ]( hMsg, pDrvSel );
			break;
	/*----------------------------------------------------------------------------------------------*/
		case LCD_CMD_DIAG:						/* 0x63: <S031>										*/
	/*----------------------------------------------------------------------------------------------*/
		default:
			Sif->StxRes( Sif, hMsg->ResBuf, 0 ); /* no response */
			return (DrvSelEnd);
	}
/*--------------------------------------------------------------------------------------------------*/
/*		Send Response																				*/
/*--------------------------------------------------------------------------------------------------*/
	LcdResSend( hMsg );
/*--------------------------------------------------------------------------------------------------*/
/*	Drive Selection end																				*/
/*--------------------------------------------------------------------------------------------------*/
	if (DrvSelEnd) { LCD_TRANSMESSAGE(); }	
/*--------------------------------------------------------------------------------------------------*/
	return( DrvSelEnd );
}

#if		CCSW_HWID == HWID_JPCV
/****************************************************************************************************/
/*																									*/
/*		Lcd Operator Main Program for Remote Operation												*/
/*																									*/
/****************************************************************************************************/
LONG	RpxLcdOpRemoteMst( DRVSEL *pDrvSel )
{
LONG	DrvSelEnd = TRUE;
UCHAR	*CmdBuf = LopMsg.CmdBuf;
UCHAR	*ResBuf = LopMsg.ResBuf;
ULONG	idx = pDrvSel->DispCurIdx;
ULONG	TxSize = ResBuf[2];
UCHAR	*pBuf;
ULONG	Axis = pDrvSel->Unit[idx].DispCurAxis + 1;
USHORT	LedSts;
/*--------------------------------------------------------------------------------------------------*/
/*		コマンド解析																				*/
/*--------------------------------------------------------------------------------------------------*/
	switch ( CmdBuf[1] )
	{
	/*----------------------------------------------------------------------------------------------*/
		case 0x64 : /*	コマンド : 0x64																*/
	/*----------------------------------------------------------------------------------------------*/
			LcdOpInit( );
	/*----------------------------------------------------------------------------------------------*/
	/*		チェックサムセット																		*/
	/*----------------------------------------------------------------------------------------------*/
			TxSize = LPX_GET_MSGLEN( ResBuf );					/* Get Msg Length					*/
			LPX_SET_MSGLEN( ResBuf, MlibCnvLittle16(TxSize) );	/* Convert Endian(Big/Little)		*/
			MlibSetSum16SM( ResBuf, TxSize );					/* Set ChkSum16 for SigmaMessage	*/
	/*----------------------------------------------------------------------------------------------*/
			DrvSelEnd = FALSE;
			Axis = 0xFF;
	/*----------------------------------------------------------------------------------------------*/
	/* 		メッセージ転送																			*/
	/*----------------------------------------------------------------------------------------------*/
			KriAddLbusMsgQueue ( LMQUEID_MSGTRANSFER, 
								 (Axis << 16) | pDrvSel->Unit[idx].Address,
								 sizeof(dummyMsg),
								 dummyMsg );
			break;

	/*----------------------------------------------------------------------------------------------*/
		case 0x60 :/*	コマンド : 0x60																*/
	/*----------------------------------------------------------------------------------------------*/
	/*			Key Input																			*/
	/*----------------------------------------------------------------------------------------------*/
			LcdKeyTranslate( hMsg );
	/*----------------------------------------------------------------------------------------------*/
	/* 		応答メッセージ確認																		*/
	/*----------------------------------------------------------------------------------------------*/
			if ( KriGetLbusMsg ( LMQUEID_MSGTRANSFER, &pBuf, &TxSize ) )
			{
				KpiRstLongTimer ( &LopV.StDispTime );
				MlibCopyByteMemory ( pBuf, ResBuf, TxSize);
			}
	/*----------------------------------------------------------------------------------------------*/
	/* 		ドライブ転送モード切替																	*/
	/*----------------------------------------------------------------------------------------------*/
			switch ( LopV.DrvOpStat )
			{
	/*----------------------------------------------------------------------------------------------*/
				case DRVOPST_WAITKEYSEND: /* wait for mode key break */
	/*----------------------------------------------------------------------------------------------*/
					pBuf = dummyMsg;
					if ( OpKeyBrk.mode )
					{
						LopV.DrvOpStat = DRVOPST_SENDKEYINFO;
					}
					break;

	/*----------------------------------------------------------------------------------------------*/
				case DRVOPST_SENDKEYINFO: /* sending key information */
	/*----------------------------------------------------------------------------------------------*/
					pBuf = CmdBuf;
					if( OpKeyMk.read ) 
					{ 
						DrvSelEnd = FALSE; /* drive selection end */
						Axis = 0xFF; 
					}
					else if ( KpiGetLongTimer( &LopV.StDispTime ) > 5000 ) 
					{
						DrvSelEnd = FALSE; /* drive selection end */
						Axis = 0xFF; 
						LopV.OpMode = LCDOM_SelectUnit;
					} 
					break;

	/*----------------------------------------------------------------------------------------------*/
				case DRVOPST_FORCECON:  /* force connection */
	/*----------------------------------------------------------------------------------------------*/
					pBuf = CmdBuf;
					if ( (KpiGetLongTimer( &LopV.StDispTime ) > 5000) )
					{
						DrvSelEnd = FALSE;
						Axis = 0xFF;
						TxSize = 0;
					}
					else if ( OpKeyMk.read )
					{
						if ( RpxChkSelectedDrv(pDrvSel) )
						{
							LopV.OpMode = LCDOM_SelectUnit;
							DrvSelEnd = FALSE;
							Axis = 0xFF;
							LopMsg.LopAccessLvl = ACCLVL_SYSTEM; /* copy the access level to the converter */
						}
						else
						{
							OpBlnkKickFlg.no_op = 1;
						}
					}
					break;

	/*----------------------------------------------------------------------------------------------*/
				default: /* error */
	/*----------------------------------------------------------------------------------------------*/
					pBuf = dummyMsg;
					break;
			}
	/*----------------------------------------------------------------------------------------------*/
			LcdSetResBlink (ResBuf, &LedSts); 					/* Blink */
			MlibSetSum16SM( ResBuf, TxSize );					/* Set ChkSum16 for SigmaMessage	*/
	/*----------------------------------------------------------------------------------------------*/
	/* 		メッセージ転送																			*/
	/*----------------------------------------------------------------------------------------------*/
			KriAddLbusMsgQueue ( LMQUEID_MSGTRANSFER, 
								 (Axis << 16) | pDrvSel->Unit[idx].Address,
								 sizeof(dummyMsg),
								 pBuf );
			break;
	/*----------------------------------------------------------------------------------------------*/
		default:
			TxSize = 0; /* No Response */
			break;
	}
/*--------------------------------------------------------------------------------------------------*/
/*		Send Response																				*/
/*--------------------------------------------------------------------------------------------------*/
	LopMsg.StxResMsg( ResBuf, TxSize );						/* 送信開始指令							*/
/*--------------------------------------------------------------------------------------------------*/
	return( DrvSelEnd );
}
#endif


/****************************************************************************************************/
/*																									*/
/*		Lcd Operator Finish Procedure																*/
/*																									*/
/****************************************************************************************************/
void	RpxLcdOpFinish( MSGIF *hMsg )
{
/*--------------------------------------------------------------------------------------------------*/
/*		Finish FnXXX Function																		*/
/*--------------------------------------------------------------------------------------------------*/
		CLCDFUN->Finish( CLCDFUN, hMsg );		/* Finish Executing Function 						*/
/*--------------------------------------------------------------------------------------------------*/
/*		Set Online, PrmCopy, etc																	*/
/*--------------------------------------------------------------------------------------------------*/
		//LopV.Online = LCD_ONLINE1;			/* 一度はフォントロードしているので、ONLINE1 とする	*/
		LopV.Online = LCD_OFFLINE;				/* 													*/
//TODO		RoutV.f.PrmCopy = FALSE;			/* パラメータコピー中の場合、中断する			*/
		pAxRsc->FnCmnCtrl->f.PrmCopy = FALSE;	/* <S031> パラメータコピー中の場合、中断する		*/
//TODO		LopMsg.LopAccessLvl = 0;			/* アクセスレベルクリア							*/
#if		CCSW_HWID != HWID_JPCV
//TODO:		RoutV.f.ReadyBlink = FALSE;
#endif
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		Lcd Operator Init Display Info. Table														*/
/*																									*/
/****************************************************************************************************/
//void	RpxLcdOpInitDispInfo( void )
void	RpxLcdOpInitDispInfo( AXIS_HANDLE *Axis )	/* <S213> */
{
//		LcdInitDispInfoTable( );				/* 表示情報テーブル初期化							*/
		LcdInitDispInfoTable( Axis );			/* 表示情報テーブル初期化				 <S213> 	*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		Lcd Initialize																				*/
/*																									*/
/****************************************************************************************************/
void	LcdOpInit( MSGIF *hMsg )
{
USHORT	i;
UCHAR	*pByte;
UCHAR	*CmdBuf = hMsg->CmdBuf;
UCHAR	*ResBuf = hMsg->ResBuf;

/*--------------------------------------------------------------------------------------------------*/
/*		Init Variables																				*/
/*--------------------------------------------------------------------------------------------------*/
		LcdVarInit( );
		RpxLcdPrmCopyInit( );
		LopV.Online = LCD_ONLINE1;
/*--------------------------------------------------------------------------------------------------*/
/*		64H 応答メッセージ作成																		*/
/*--------------------------------------------------------------------------------------------------*/
		ResBuf[LCDMSGR_AXIDX]  = CmdBuf[0];					/* 軸アドレス							*/
		ResBuf[LCDMSGR_FUNIDX] = CmdBuf[1];					/* コマンド								*/
		LPX_SET_MSGLEN( ResBuf, LCDMSGR_LEN64 );			/* サイズ(バイト長)						*/
/*--------------------------------------------------------------------------------------------------*/
		for( i=0, pByte=&(ResBuf[LCDMSGR_FONIDX64]); i < (5*16); i++ )
		{
			pByte[i] = LcdExtFont[i];
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		LCD Operator variables Initialize															*/
/*																									*/
/****************************************************************************************************/
void	LcdVarInit( void )
{
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W（2軸対応）のみ	*//* <S0E4> */
	LOPV	*lcd_globals;
	lcd_globals = &LopV;
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		構造体変数Lcdのクリア																		*/
/*--------------------------------------------------------------------------------------------------*/
		MlibResetByteMemory( (void *)&LopV, sizeof(LOPV) );

#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W（2軸対応）のみ	*//* <S0E4> */
		/*	ドライブ（軸）選択データポインタの初期化（ＬＣＤオペ軸管理データ）	*/
		lcd_globals->pDrvSel = &DrvSel;
#endif

#if CCSW_CMDID == CMDID_EXIST			/*<11J42>*/
/*--------------------------------------------------------------------------------------------------*/
/* 		構造体変数LcdOptのクリア																	*/
/*--------------------------------------------------------------------------------------------------*/
		MlibResetByteMemory((void *)&LcdOpt, sizeof(LcdOpt));
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		非0値のセット																				*/
/*--------------------------------------------------------------------------------------------------*/
		KpiRstLongTimer( &LopV.StDispTime );		/* 起動時表示時間計測用							*/
/*--------------------------------------------------------------------------------------------------*/
/*		ブリンク変数のクリア																		*/
/*--------------------------------------------------------------------------------------------------*/
		*((UINT *)&OpBlnkFlg) = 0;					/* Clear Blink Flag								*/
		*((UINT *)&OpBlnkKickFlg) = 0;				/* Clear Blink Kick Flag						*/
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		Lcd Operator Key Translation																*/
/*																									*/
/****************************************************************************************************/
void	LcdKeyTranslate( MSGIF *hMsg )
{
#define KEYHOLDCNTR (4)						/* @@ */
UCHAR	*CmdBuf = hMsg->CmdBuf;
USHORT	LcdRawKey = LPX_GET_LCDKEY( CmdBuf );

/*--------------------------------------------------------------------------------------------------*/
/*		開始処理																					*/
/*--------------------------------------------------------------------------------------------------*/
		MlibCnvLittle16( LcdRawKey, LcdRawKey );
		*((UINT *)&OpKeyHld) = *((UINT *)&OpKeyMk) = *((UINT *)&OpKeyBrk) = *((UINT *)&OpKeyLvl) = 0;
/*--------------------------------------------------------------------------------------------------*/
		if( *((UINT *)&OpBlnkFlg) != 0 )	/* ブリンク中はキー入力を無視する	*/
		{
			LopV.RawKeyLast = LcdRawKey; return;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		キー入力レベルの処理																		*/
/*--------------------------------------------------------------------------------------------------*/
		if( LcdRawKey & LCDKEY_MODE		){ OpKeyLvl.mode   = TRUE;}
		if( LcdRawKey & LCDKEY_SHUP		){ OpKeyLvl.shup   = TRUE;}
		if( LcdRawKey & LCDKEY_UP		){ OpKeyLvl.up     = TRUE;}
		if( LcdRawKey & LCDKEY_DOWN		){ OpKeyLvl.down   = TRUE;}
		if( LcdRawKey & LCDKEY_SCRL		){ OpKeyLvl.scroll = TRUE;}
		if( LcdRawKey & LCDKEY_SVON		){ OpKeyLvl.svon   = TRUE;}
		if( LcdRawKey & LCDKEY_DATA		){ OpKeyLvl.data   = TRUE;}
		if( LcdRawKey & LCDKEY_SHDOWN	){ OpKeyLvl.shdown = TRUE;}
		if( LcdRawKey & LCDKEY_READ		){ OpKeyLvl.read   = TRUE;}
		if( LcdRawKey & LCDKEY_WRITE	){ OpKeyLvl.write  = TRUE;}
		if( LcdRawKey & LCDKEY_ALMRST	){ OpKeyLvl.almrst = TRUE;}
		if((LcdRawKey & LCDKEY_RST) == LCDKEY_RST ){ OpKeyLvl.rst = TRUE;}
/*--------------------------------------------------------------------------------------------------*/
/*		キー入力に変化があった時のみセット(=チャタリング防止)										*/
/*--------------------------------------------------------------------------------------------------*/
		if( LcdRawKey != LopV.RawKeyLast )
		{
			LopV.KeyHoldCnt = 0;
		/*------------------------------------------------------------------------------------------*/
		/*		キー入力MAKEの処理																	*/
		/*------------------------------------------------------------------------------------------*/
			switch( LcdRawKey )
			{
			case LCDKEY_MODE:	OpKeyMk.mode   = TRUE;	break;
			case LCDKEY_SHUP:	OpKeyMk.shup   = TRUE;	break;
			case LCDKEY_UP:		OpKeyMk.up     = TRUE;	break;
			case LCDKEY_DOWN:	OpKeyMk.down   = TRUE;	break;
			case LCDKEY_SCRL:	OpKeyMk.scroll = TRUE;	break;
			case LCDKEY_SVON:	OpKeyMk.svon   = TRUE;	break;
			case LCDKEY_DATA:	OpKeyMk.data   = TRUE;	break;
			case LCDKEY_SHDOWN:	OpKeyMk.shdown = TRUE;	break;
			case LCDKEY_READ:	OpKeyMk.read   = TRUE;	break;
			case LCDKEY_WRITE:	OpKeyMk.write  = TRUE;	break;
			case LCDKEY_ALMRST:	OpKeyMk.almrst = TRUE;	break;
			case LCDKEY_RST:	OpKeyMk.rst    = TRUE;	break;
			default: break;
			}
		/*------------------------------------------------------------------------------------------*/
		/*		キー入力BREAKの処理																	*/
		/*------------------------------------------------------------------------------------------*/
			switch( LopV.RawKeyLast )
			{
			case LCDKEY_MODE:	OpKeyBrk.mode   = TRUE;	break;
			case LCDKEY_SHUP:	OpKeyBrk.shup   = TRUE;	break;
			case LCDKEY_UP:		OpKeyBrk.up     = TRUE;	break;
			case LCDKEY_DOWN:	OpKeyBrk.down   = TRUE;	break;
			case LCDKEY_SCRL:	OpKeyBrk.scroll = TRUE;	break;
			case LCDKEY_SVON:	OpKeyBrk.svon   = TRUE;	break;
			case LCDKEY_DATA:	OpKeyBrk.data   = TRUE;	break;
			case LCDKEY_SHDOWN:	OpKeyBrk.shdown = TRUE; break;
			case LCDKEY_READ:	OpKeyBrk.read   = TRUE;	break;
			case LCDKEY_WRITE:	OpKeyBrk.write  = TRUE;	break;
			case LCDKEY_ALMRST:	OpKeyBrk.almrst = TRUE;	break;
			case LCDKEY_RST:	OpKeyBrk.rst    = TRUE;	break;
			default: break;
			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		キー入力に変化がない場合: リピート, ホールド処理											*/
/*--------------------------------------------------------------------------------------------------*/
		else
		{
			switch( LcdRawKey )
			{
			case LCDKEY_SHUP:
				if(      LopV.KeyHoldCnt <  KEYHOLDCNTR ){ LopV.KeyHoldCnt++;}
				else if( LopV.KeyHoldCnt == KEYHOLDCNTR ){ LopV.KeyHoldCnt++; OpKeyMk.shup = TRUE;}
				else if( LopV.KeyHoldCnt == KEYHOLDCNTR+1  ){ LopV.KeyHoldCnt--;}
				break;
			case LCDKEY_UP:
				if(      LopV.KeyHoldCnt <  KEYHOLDCNTR ){ LopV.KeyHoldCnt++;}
				else if( LopV.KeyHoldCnt == KEYHOLDCNTR ){ LopV.KeyHoldCnt++; OpKeyMk.up = TRUE;}
				else if( LopV.KeyHoldCnt == KEYHOLDCNTR+1 ){ LopV.KeyHoldCnt--;}
				break;
			case LCDKEY_DOWN:
				if(      LopV.KeyHoldCnt <  KEYHOLDCNTR ){ LopV.KeyHoldCnt++;}
				else if( LopV.KeyHoldCnt == KEYHOLDCNTR ){ LopV.KeyHoldCnt++; OpKeyMk.down = TRUE;}
				else if( LopV.KeyHoldCnt == KEYHOLDCNTR+1 ){ LopV.KeyHoldCnt--;}
				break;
			case LCDKEY_SHDOWN:
				if(      LopV.KeyHoldCnt <  KEYHOLDCNTR ){ LopV.KeyHoldCnt++;}
				else if( LopV.KeyHoldCnt == KEYHOLDCNTR ){ LopV.KeyHoldCnt++; OpKeyMk.shdown = TRUE;}
				else if( LopV.KeyHoldCnt == KEYHOLDCNTR+1 ){ LopV.KeyHoldCnt--;}
				break;
	/*----------------------------------------------------------------------------------------------*/
	/*		Set Key Hold Status (About 3 Second.)													*/
	/*----------------------------------------------------------------------------------------------*/
			case LCDKEY_READ:
				if(		LopV.KeyHoldCnt < 35 ){ LopV.KeyHoldCnt++;}
				else if ( LopV.KeyHoldCnt == 35){ LopV.KeyHoldCnt++; OpKeyHld.read = TRUE;}
				break;
			default:
				break;
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		LopV.RawKeyLast = LcdRawKey;
		return;
}



/****************************************************************************************************/
/*																									*/
/*		Lcd Start Display																			*/
/*																									*/
/****************************************************************************************************/
void	LcdStartDisp( MSGIF *hMsg, DRVSEL *pDrvSel )
{
UCHAR	*ResBuf = hMsg->ResBuf;

/*--------------------------------------------------------------------------------------------------*/
/*		表示																						*/
/*--------------------------------------------------------------------------------------------------*/
	RpxLcdClrTxBuf( ResBuf );
	RpxLcdSetStr( 1, 0, (UCHAR *)CSTR_INITDSP_L1,  ResBuf );
	RpxLcdSetStr( 2, 2, (UCHAR *)CSTR_INITDSP_L2,  ResBuf );
/*--------------------------------------------------------------------------------------------------*/
/*		<S031> Get Parameter Copy Information														*/
/*--------------------------------------------------------------------------------------------------*/
	LopV.PrmInfoGetDone = RpxLcdPrmInfoGet( );
/*--------------------------------------------------------------------------------------------------*/
	return;
}
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W（2軸対応）のみ	*//* <S0E4> */
/****************************************************************************************************/
/*																									*/
/*		Lcd Select Axis mode 																		*/
/*																									*/
/****************************************************************************************************/
void	LcdSelectAxis( MSGIF *hMsg, DRVSEL *pDrvSel )
{
AXIS_HANDLE		*pAxRsc;		/*	軸単位の構造体	*/
LONG	i;
static	UCHAR	SelAxsNo = 1;	/*	選択軸番号（1 or 2）の初期値	*/
UCHAR	*ResBuf = hMsg->ResBuf;
UCHAR	*pSetSts;	/* <S19B> */
/*--------------------------------------------------------------------------------------------------*/
/*		Lcd Select Axis mode																			*/
/*--------------------------------------------------------------------------------------------------*/
	/*	注意：ユニット数はとりあえず1個固定	*/
	if( OpKeyBrk.data )
	{
		/*	何もしない	*/
	}
	else if( OpKeyMk.almrst )
	{
		/*	アラームクリア処理	*/
		/*	注意：とりあえず現在選択中の軸のみクリアする	*/
//		KriResetSystemAlarm();
		pAxRsc = GetAxisHandle(SelAxsNo - 1);
		ALMSetAlarmResetRequest( pAxRsc->AlmManager );
	}
	else if( OpKeyMk.up )
	{
		--SelAxsNo;			/*軸番号-1*/
		if(SelAxsNo < 1)
		{	/*	最大軸番号へ	*/
			SelAxsNo = NumberOfAxes;
		}
		/*	現在表示軸データの選択	*/
		LcdDspDatSelAxs( SelAxsNo );
	}
	else if( OpKeyMk.down )
	{
		++SelAxsNo;			/*軸番号+1*/
		if(SelAxsNo > NumberOfAxes)
		{	/*	最小軸番号へ	*/
			SelAxsNo = 1;
		}
		/*	現在表示軸データの選択	*/
		LcdDspDatSelAxs( SelAxsNo );
	}
	else if( OpKeyMk.scroll )
	{
		/*	何もしない	*/
	}
	/*------------------------------------------------------------------------------------------*/
	/*		表示処理																			*/
	/*------------------------------------------------------------------------------------------*/
	RpxLcdClrTxBuf( ResBuf );
	RpxLcdSetStr( 0, 0, (UCHAR *)CSTR_SELAXS_TITLE, ResBuf );	/* TITLE表示	*/
	/*------------------------------------------------------------------------------------------*/
	/*	軸選択画面の表示	*/
	for( i = 0; i < NumberOfAxes; i++ )					/* Display four lines */
	{
		RpxLcdSetStr( i+1, 0, (UCHAR *)"Axis-0", ResBuf );
		RpxLcdSetValHex( i+1, 6, i+1, 1, ResBuf );		/*	1桁の軸番号表示			*/
		DrvSel.Unit[0].DispCurAxis = i + 1;				/*	アラームコードの軸番号セット	*/
		pAxRsc = GetAxisHandle(i);
		/*	アラームコードの取得	*/
		DrvSel.Unit[0].Axis[i].AlmCode = ALMGetAlarmInfo( pAxRsc->AlmManager );
		if( DrvSel.Unit[0].Axis[i].AlmCode != 0x0000 )
		{	/*	1軸分のアラームコードの表示	*/
			RpxLcdSetStr(    i+1, 9, (UCHAR *)"A.", (UCHAR *)ResBuf);
			RpxLcdSetValHex( i+1, 11, DrvSel.Unit[0].Axis[i].AlmCode, 3, (UCHAR *)ResBuf);
		}else
		{	/* 1軸分のステータスコードの表示 *//* <S19B> */
			/*--------------------------------------------------------------------------------------------------*/
			/*		コンバータステータス表示処理																*/
			/*--------------------------------------------------------------------------------------------------*/
					if( CHK_BB()				    ){ pSetSts = (UCHAR *)"*BB";	}
					else if( CHK_PDET() 			){ pSetSts = (UCHAR *)"*P DET";	}
					else							 { pSetSts = (UCHAR *)"*RUN";	}
				/*----------------------------------------------------------------------------------------------*/
					if( CHK_TUNING() 				){ pSetSts = (UCHAR *)"*ADJ";	}
				/*----------------------------------------------------------------------------------------------*/
					if( !CHK_JOGMODE() && !CHK_ZRETMODE() )
					{
						if( CHK_POT() && CHK_NOT()	){ pSetSts = (UCHAR *)"*PT NT";	}
						else if( CHK_POT()			){ pSetSts = (UCHAR *)"*P-OT";	}
						else if( CHK_NOT()			){ pSetSts = (UCHAR *)"*N-OT";	}
						else if( CHK_PLS()			){ pSetSts = (UCHAR *)"*P-LS";	}
						else if( CHK_NLS()			){ pSetSts = (UCHAR *)"*N-LS";	}
					}
				/*----------------------------------------------------------------------------------------------*/
					if( CHK_ESTOP()					){ pSetSts = (UCHAR *)"*ESTOP";	}
				/*----------------------------------------------------------------------------------------------*/
					if( CHK_FSTP()					){ pSetSts = (UCHAR *)"*FSTP";	} 		/* <S1F5> */
				/*----------------------------------------------------------------------------------------------*/
					if( CHK_HBB() 					){ pSetSts = (UCHAR *)"*HBB";	}
				/*----------------------------------------------------------------------------------------------*/
					/* モータレステストモードでなければポインタを1進める *//* <S1B7> */
//					if( 1 							){ pSetSts = pSetSts + 1;		}
					if( pAxRsc->MotorLessTest->MotorLessTestMode == FALSE ){ pSetSts = pSetSts + 1;		}
			/*--------------------------------------------------------------------------------------------------*/
			/*		ステータス表示文字列設定処理																*/
			/*--------------------------------------------------------------------------------------------------*/
//-					RpxLcdSetStr( 0, 2, pSetSts, ResBuf );
					RpxLcdSetStr(  i+1, 9, (UCHAR *)pSetSts, (UCHAR *)ResBuf );
			/*--------------------------------------------------------------------------------------------------*/
		}	/* <S19B> */
	}
	RpxLcdSetBlnk( SelAxsNo, 0, 17, (UCHAR *)ResBuf );		/* Blink current line */
/*-------------------------------------------------------------------------------------------------*/
/*	ブリンク＆ステータス表示処理																*/
/*--------------------------------------------------------------------------------------------------*/
	RpxLcdSetResStatus (ResBuf);
	RpxLcdSetBlink (ResBuf);
	/*	現在表示軸番号の最終値設定	*/
	DrvSel.Unit[0].DispCurAxis = SelAxsNo;
	/*	現在表示軸データの選択	*/
	LcdDspDatSelAxs( SelAxsNo );
	return;
}
/* <S19B> */
/****************************************************************************************************/
/*	オペレータのユーザー定数表示選択	注意）"MsgMngr.c"内のLpxPrmGetOpeDspMode()の流用						*/
/****************************************************************************************************/
USHORT PrmGetOpeDspMode( AXIS_HANDLE *pAxRsc)
{
	PRMDATA		*Prm;
	USHORT		rc;

	Prm = pAxRsc->UniPrms->Prm;		/* パラメータ設定データ */
	if( (Prm->b_prmB & 0x000F) == OPEDSP_SETUPPRM )
	{
		/* セットアップ用ユーザー定数のみ表示 */
		rc = OPEDSP_SETUPPRM;
	}
	else if( (Prm->b_prmB & 0x000F) == OPEDSP_ALLPRM )
	{
		/* すべてのユーザー定数を表示 */
		rc = OPEDSP_ALLPRM;
	}
	else
	{
		/* Do Nothing */
	}
	return(rc);
}	/* <S19B> */
/****************************************************************************************************/
/*		Lcd Display Data Select Axis 																*/
/*		Input -> axs_no  0／1：1軸データ選択　2：2軸データ選択											*/
/****************************************************************************************************/
void	LcdDspDatSelAxs( UINT axs_no )
{
	/*	表示する軸の選択	*/
	if(axs_no != 0)
	{
		/*	データ取り出し軸ポインターのセット	*/
		LopMsg.pAxRsc = GetAxisHandle(axs_no - 1);
		/*	ラスト軸インデックス（＝0 ｏｒ 1）のセット	*/
		LopMsg.LastAxisIdx = axs_no - 1;
	}else
	{
		/*	データ取り出し軸ポインターを1軸へセット	*/
		LopMsg.pAxRsc = GetAxisHandle(0);
		/*	ラスト軸インデックス（＝0）のセット	*/
		LopMsg.LastAxisIdx = 0;
	}
	/*	選択 された軸のパラメータをセット	*//* <S19B> */
	LopMsg.OpeDspMode = PrmGetOpeDspMode(LopMsg.pAxRsc);	/* Set User Paramete Display Mode	*/
}

#elif CSW_BOARD_TYPE == SGD7S_TYPE	/*	SGD7S（単軸時）のみ	*//* <S0E4> */
/****************************************************************************************************/
/*																									*/
/*		Lcd Select Drive Mode																		*/
/*																									*/
/****************************************************************************************************/
void	LcdSelectAxis( MSGIF *hMsg, DRVSEL *pDrvSel )
{
#if 0
UCHAR	*ResBuf = LopMsg.ResBuf;
LONG	idx = pDrvSel->DispCurIdx;
LONG 	i;
USHORT	LedSts;
/*--------------------------------------------------------------------------------------------------*/
	RpxLcdClrTxBuf( ResBuf );
/*--------------------------------------------------------------------------------------------------*/
	if( OpKeyMk.up )
	{
		if( pDrvSel->Unit[idx].DispCurAxis > 0 )
		{
			pDrvSel->Unit[idx].DispCurAxis --;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
	else if( OpKeyMk.down )
	{
		if( pDrvSel->Unit[idx].DispCurAxis < (pDrvSel->Unit[idx].AxisNumber - 1) )
		{
			pDrvSel->Unit[idx].DispCurAxis ++;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		表示処理																					*/
/*--------------------------------------------------------------------------------------------------*/
	RpxLcdSetStr( 0, 0, (UCHAR *)"-AXIS SELECTION-", ResBuf );
	for( i=0; i < 3; i++ )
	{
		if( i >= pDrvSel->Unit[idx].AxisNumber ){ break; }
/*------------------------------------------------------------------------------------------*/
		RpxLcdSetStr( i+1, 0, (UCHAR *)"Axis-", ResBuf );
		RpxLcdSetValHex( i+1, 5, i+1, 2, ResBuf );
/*------------------------------------------------------------------------------------------*/
		if( pDrvSel->Unit[idx].Axis[i].AlmCode != 0x0000 )
		{
			RpxLcdSetStr(    i+1, 9, (UCHAR *)"A.", (UCHAR *)ResBuf);
			RpxLcdSetValHex( i+1, 11, pDrvSel->Unit[idx].Axis[i].AlmCode, 3, (UCHAR *)ResBuf);
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		選択中ドライブのブリンク																	*/
/*--------------------------------------------------------------------------------------------------*/
	RpxLcdSetBlnk( (pDrvSel->Unit[idx].DispCurAxis + 1), 0, 8, ResBuf );
/*--------------------------------------------------------------------------------------------------*/
/*		点滅文字表示処理																			*/
/*--------------------------------------------------------------------------------------------------*/
	LcdSetResBlink(ResBuf, &LedSts);
#if		CCSW_HWID == HWID_JPCV
/*--------------------------------------------------------------------------------------------------*/
/*		アラームリセット																			*/
/*--------------------------------------------------------------------------------------------------*/
	if ( OpKeyMk.almrst )
	{
		ResBuf = LopMsg.CmdBuf;
	}
	else
	{
		ResBuf = dummyMsg;
	}
/*--------------------------------------------------------------------------------------------------*/
/*		ReadyLedを点滅させるためにDummyのメッセージを送信											*/
/*--------------------------------------------------------------------------------------------------*/
	KriAddLbusMsgQueue ( LMQUEID_MSGTRANSFER,
				((pDrvSel->Unit[ idx ].DispCurAxis + 1) << 16) | pDrvSel->Unit[ idx ].Address,
				sizeof(dummyMsg),
				ResBuf );
#endif
#endif
	return;
}
#endif /* <S0E4S0E4> */


#if		CCSW_HWID == HWID_JPCV
void	LcdSelectUnit(MSGIF *hMsg, DRVSEL *pDrvSel )
{
UCHAR	*ResBuf = LopMsg.ResBuf;
LONG	idx = pDrvSel->DispCurIdx;
LONG i,j,k;
USHORT	LedSts;

/*--------------------------------------------------------------------------------------------------*/
	RpxLcdClrTxBuf( ResBuf );
/*--------------------------------------------------------------------------------------------------*/
	if( OpKeyMk.up )
	{
		if( pDrvSel->DispCurIdx > 0 )
		{
			pDrvSel->DispCurIdx--;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
	else if( OpKeyMk.down )
	{
		if( pDrvSel->DispCurIdx < (pDrvSel->DispNumber - 1) )
		{
			pDrvSel->DispCurIdx++;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		Update DispTopIdx																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( pDrvSel->DispCurIdx < pDrvSel->DispTopIdx )
	{
		pDrvSel->DispTopIdx--;
	}
	else if( pDrvSel->DispCurIdx >= (pDrvSel->DispTopIdx + 4) )
	{
		pDrvSel->DispTopIdx++;
	}
/*--------------------------------------------------------------------------------------------------*/
/*		表示処理																					*/
/*--------------------------------------------------------------------------------------------------*/
	RpxLcdSetStr( 0, 0, (UCHAR *)"-UNIT SELECTION-", ResBuf );
	for( i=0; i < 4; i++ )
	{
		j = pDrvSel->DispTopIdx + i;
		if( j >= pDrvSel->DispNumber ){ break;}
/*------------------------------------------------------------------------------------------*/
		if( pDrvSel->Unit[j].Type == DRVSEL_TYPE_CONVERTER )
		{
			RpxLcdSetStr( i+1, 0, (UCHAR *)"Converter", ResBuf );
		}
		else if( pDrvSel->Unit[j].Type == DRVSEL_TYPE_COMMCPU )
		{
			RpxLcdSetStr( i+1, 0, (UCHAR *)"Comm. CPU", ResBuf );
		}
		else
		{
			RpxLcdSetStr( i+1, 0, (UCHAR *)"UNIT-Adr", ResBuf );
			RpxLcdSetValHex( i+1, 8, (pDrvSel->Unit[j].Address & (0x0F)), 2, ResBuf );
		}
		/*------------------------------------------------------------------------------------------*/
		for ( k=0; k < pDrvSel->Unit[j].AxisNumber; k++)
		{
			if( pDrvSel->Unit[j].Axis[k].AlmCode != 0x0000 )
			{
				RpxLcdSetStr(    i+1, 11, (UCHAR *)"A.", (UCHAR *)ResBuf);
				RpxLcdSetValHex( i+1, 13, pDrvSel->Unit[j].Axis[k].AlmCode, 3, (UCHAR *)ResBuf);
				break;
			}
		}
	}

/*--------------------------------------------------------------------------------------------------*/
/*		選択中ドライブのブリンク																	*/
/*--------------------------------------------------------------------------------------------------*/
	RpxLcdSetBlnk( (pDrvSel->DispCurIdx - pDrvSel->DispTopIdx + 1), 0, 10, ResBuf );
/*--------------------------------------------------------------------------------------------------*/
/*		点滅文字表示処理																			*/
/*--------------------------------------------------------------------------------------------------*/
	LcdSetResBlink(ResBuf, &LedSts);
	return;
}
#endif

#if		CCSW_HWID == HWID_JPCV
/****************************************************************************************************/
/*																									*/
/*		Lcd Password input Mode																		*/
/*																									*/
/****************************************************************************************************/
void	LcdPasswd( MSGIF *hMsg, DRVSEL *pDrvSel )
{
USHORT	FnIdx = LopV.PasswdIdx;
UCHAR	*ResBuf = LopMsg.ResBuf;
USHORT	LedSts;

/*--------------------------------------------------------------------------------------------------*/
/*		表示処理																					*/
/*--------------------------------------------------------------------------------------------------*/
		RpxLcdClrTxBuf( ResBuf );
		RpxFunExeLcd( OpKeyMk, OpKeyBrk, OpKeyLvl, FnIdx, ResBuf, &OpBlnkKickFlg, &LopMsg );
		RpxLcdSetStr( 0, 0, (UCHAR *)"-UNIT SELECTION-", ResBuf );		/* Display Title			*/
/*--------------------------------------------------------------------------------------------------*/
/*		点滅文字表示処理																			*/
/*--------------------------------------------------------------------------------------------------*/
		LcdSetResBlink(ResBuf, &LedSts);
/*--------------------------------------------------------------------------------------------------*/
		return;
}
#endif

/****************************************************************************************************/
/*																									*/
/*		Lcd Error Mode																				*/
/*																									*/
/****************************************************************************************************/
void	LcdError( MSGIF *hMsg, DRVSEL *pDrvSel )
{
UCHAR	*ResBuf = LopMsg.ResBuf;
		LPX_SET_MSGLEN( ResBuf, 0);		/* size = zero	*/
		return;
}

/****************************************************************************************************/
/*																									*/
/*		TxBuf Clear																					*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	[注意]  0x60以外の応答の場合は、本ルーチン実行後 かつ LcdResSend()コール前に					*/
/*			サイズフィールド(*(USHORT *)(&(ResBuf[LCDMSGR_LENIDX])))を書きかえる必要がある			*/
/*																									*/
/****************************************************************************************************/
void	RpxLcdClrTxBuf(						/* Clear ResBuf(TxBuf)									*/
		UCHAR	*ResBuf			)			/* 応答バッファアドレス									*/
{
USHORT *pBuf;
/*--------------------------------------------------------------------------------------------------*/
/*		メッセージヘッダ,etc																		*/
/*--------------------------------------------------------------------------------------------------*/
		ResBuf[LCDMSGR_AXIDX]  = 0;						/* 軸アドレス(仮)							*/
		ResBuf[LCDMSGR_FUNIDX] = 0x60;					/* コマンド(仮)								*/
		LPX_SET_MSGLEN( ResBuf, LCDMSGR_LEN60 );		/* 0x60応答サイズ(仮)						*/
		LPX_SET_LEDSTS( ResBuf, 0 );					/* LED表示情報								*/
/*--------------------------------------------------------------------------------------------------*/
/*		画面名 : 点線表示, 表示濃度, LCD点滅設定, 2行目文字サイズ設定								*/
/*--------------------------------------------------------------------------------------------------*/
		pBuf = (USHORT *)&ResBuf[LCDMSGR_DSPIDX];
		do { 
			*(pBuf++) = 0;                                         /* clear data */
		} while ( pBuf < ((USHORT *)&ResBuf[LCDMSGR_CHRIDX]) );
		ResBuf[LCDMSGR_DSPIDX] = 5;						           /* 画面表示形式	*/
/*--------------------------------------------------------------------------------------------------*/
/*		LCD文字設定																					*/
/*--------------------------------------------------------------------------------------------------*/
		do { 
			*(pBuf++) = 0x2020;                                    /* set blank (' ' == 0x20) */
		} while ( pBuf < ((USHORT *)&ResBuf[LCDMSGR_SUMIDX60]) );
/*--------------------------------------------------------------------------------------------------*/
		return;
}






/****************************************************************************************************/
/*																									*/
/*		Pn/Un Display Data Set																		*/
/*																									*/
/****************************************************************************************************/
void	RpxLcdSetPnUnData(					/* Pn/Un Display Data Set								*/
		USHORT	Lpos,						/* 開始行位置(最上行が0)								*/
		USHORT	Dpos,						/* 開始桁位置(左端が0)									*/
		ULONG	Data,						/* 表示データ											*/
		USHORT	IsLong,						/* ロングパラメータ時TRUE								*/
		USHORT	Base,						/* 底(表示属性)											*/
		USHORT	Sign,						/* 符号有無(表示属性)									*/
		SHORT	Decimals,					/* 小数点以下桁数(表示属性)								*/
		UCHAR	*ResBuf			)			/* 応答バッファアドレス									*/
{
UINT	digit;								/* <S18B> */
/*--------------------------------------------------------------------------------------------------*/
/*		ビットデータ表示																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( Base == DSPDEF_BASEBIT )
		{
			RpxLcdSetBit( Lpos, Dpos, (UCHAR)Data, ResBuf );
			return;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		ロングデータ表示																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( IsLong )
		{
			if( Base == DSPDEF_BASEDEC )	/* 底が10進	*/
			{
				if( Sign == DSPDEF_W_SIGN )
				{
					Data = (ULONG)RpxLcdSetSignChar( Lpos, Dpos, (LONG)Data, ResBuf );
					digit = ((Decimals == 0) ? 10 : 9);			/* <S18B> */
					RpxLcdSetDecPnt( Lpos, (USHORT)(Dpos+1), Data, digit /*10*/, FALSE, Decimals, ResBuf );
				}
				else
				{
					RpxLcdSetDecPnt( Lpos, Dpos, Data, 10, FALSE, Decimals, ResBuf );
				}
			}
			else							/* 底が10進以外	*/
			{
				if( Base == DSPDEF_BASENBL )
				{
					RpxLcdSetStr( Lpos, Dpos, (UCHAR *)"n.", ResBuf );
				}
				else
				{
					RpxLcdSetStr( Lpos, Dpos, (UCHAR *)"H.", ResBuf );
				}
				RpxLcdSetValHex( Lpos, (USHORT)(Dpos+2), Data, 8, ResBuf );
			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		ワードデータ表示																			*/
/*--------------------------------------------------------------------------------------------------*/
		else
		{
			if( Base == DSPDEF_BASEDEC )	/* 底が10進	*/
			{
				if(Sign == DSPDEF_W_SIGN)
				{
					Data = (ULONG)RpxLcdSetSignChar( Lpos, Dpos, (SHORT)Data, ResBuf );
					RpxLcdSetDecPnt( Lpos, (USHORT)(Dpos+1), Data, 5, FALSE, Decimals, ResBuf );
				}
				else
				{
					RpxLcdSetDecPnt( Lpos, Dpos, Data, 5, FALSE, Decimals, ResBuf );
				}
			}
			else							/* 底が10進以外	*/
			{
				if( Base == DSPDEF_BASENBL )
				{
					RpxLcdSetStr( Lpos, Dpos, (UCHAR *)"n.", ResBuf );
				}
				else
				{
					RpxLcdSetStr( Lpos, Dpos, (UCHAR *)"H.", ResBuf );
				}
				RpxLcdSetValHex( Lpos, (USHORT)(Dpos+2), Data, 4, ResBuf );
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		Lcd Operator 符号付10進数表示																*/
/*																									*/
/****************************************************************************************************/
void	RpxLcdSetSignedDecData(				/* Lcd Operator 符号付10進数表示						*/
		USHORT	Lpos,						/* 開始行位置(最上行が0)								*/
		USHORT	Dpos,						/* 開始桁位置(左端が0)									*/
		LONG	Data,						/* 表示データ											*/
		USHORT	Digit,						/* 表示する桁数(小数点含まず)							*/
		USHORT	SupFlg,						/* TRUE=上位桁が0の場合、'0'を表示しない				*/
		SHORT	Decimals,					/* 小数点以下桁数(正の場合のみ有効)						*/
		UCHAR	*ResBuf			)			/* 応答バッファアドレス									*/
{

/*--------------------------------------------------------------------------------------------------*/
/*		Set Sign and Dec String																		*/
/*--------------------------------------------------------------------------------------------------*/
		Data = RpxLcdSetSignChar( Lpos, Dpos, Data, ResBuf );
		RpxLcdSetDecPnt( Lpos, (USHORT)(Dpos+1), Data, Digit, SupFlg, Decimals, ResBuf );
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		Lcd Operator String Set																		*/
/*																									*/
/****************************************************************************************************/
void	RpxLcdSetStr(						/* Lcd Operator String Set								*/
		USHORT	Lpos,						/* 開始行位置(最上行が0)								*/
		USHORT	Dpos,						/* 開始桁位置(左端が0)									*/
		UCHAR	*pStr,						/* 表示する文字列アドレス								*/
		UCHAR	*ResBuf			)			/* 応答バッファアドレス									*/
{
LONG	Idx = LCDMSGR_CHRIDX + (Lpos*18) + Dpos;

/*--------------------------------------------------------------------------------------------------*/
/*		Set String																					*/
/*--------------------------------------------------------------------------------------------------*/
		while( *pStr != 0 )
		{
			ResBuf[Idx++] = *(pStr++);
			if( Idx >= LCDMSGR_SUMIDX60 ) break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		Lcd Operator Sign Char Set																	*/
/*																									*/
/****************************************************************************************************/
LONG	RpxLcdSetSignChar(					/* Lcd Operator Sign Char Set							*/
		USHORT	Lpos,						/* 開始行位置(最上行が0)								*/
		USHORT	Dpos,						/* 開始桁位置(左端が0)									*/
		LONG	Data,						/* 表示するデータ										*/
		UCHAR	*ResBuf			)			/* 応答バッファアドレス									*/
{
LONG	Idx = LCDMSGR_CHRIDX + (Lpos*18) + Dpos;

/*--------------------------------------------------------------------------------------------------*/
/*		Set Sign String																				*/
/*--------------------------------------------------------------------------------------------------*/
		if(      Data >  0 ){ ResBuf[Idx] = '+';}
		else if( Data == 0 ){ ResBuf[Idx] = ' ';}
		else				{ ResBuf[Idx] = '-'; Data = -Data;}
/*--------------------------------------------------------------------------------------------------*/
		return( Data );
}



/****************************************************************************************************/
/*																									*/
/*		Lcd Operator Decimal w/point Set															*/
/*																									*/
/****************************************************************************************************/
void	RpxLcdSetDecPnt(					/* Lcd Operator Decimal w/point Set						*/
		USHORT	Lpos,						/* 開始行位置(最上行が0)								*/
		USHORT	Dpos,						/* 開始桁位置(左端が0)									*/
		ULONG	Data,						/* 表示データ											*/
		USHORT	Digit,						/* 表示する桁数(小数点含まず)							*/
		USHORT	SupFlg,						/* TRUE=上位桁が0の場合、'0'を表示しない				*/
		SHORT	Decimals,					/* 小数点以下桁数(正の場合のみ有効)						*/
		UCHAR	*ResBuf			)			/* 応答バッファアドレス									*/
{
LONG	i,j;
ULONG	DivWk, DecWk;
LONG	Idx = LCDMSGR_CHRIDX + (Lpos*18) + Dpos;

/*--------------------------------------------------------------------------------------------------*/
/*		Check Digit																					*/
/*--------------------------------------------------------------------------------------------------*/
		if( (Digit == 0) || (10 < Digit) )
		{
			return;
		}
/*--------------------------------------------------------------------------------------------------*/
		if( Digit < 10 )									/* 上位桁を落とす						*/
		{
			if( Data > MlibDecPowerTbl[Digit] ){ Data %= MlibDecPowerTbl[Digit];}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Set Dec String																				*/
/*--------------------------------------------------------------------------------------------------*/
		for( i=0; i < Digit; i++ )
		{
			j = Digit - 1 - i;								/* Digit Index							*/
			DivWk = MlibDecPowerTbl[j];						/* 割る数: 例 1000, 100, 10, 1			*/
			DecWk = Data / DivWk;							/* Display Dec Value					*/
			Data  = Data - (DivWk * DecWk);					/* Update  Data for Next				*/
		/*------------------------------------------------------------------------------------------*/
			if( SupFlg && (DecWk == 0) )
			{
				ResBuf[Idx++] = ' ';						/* 上位の0はブランク表示とする			*/
			}
			else
			{
				SupFlg = FALSE;
				ResBuf[Idx++] = (UCHAR)('0' + DecWk);
			}
			if( (j != 0) && (j == Decimals) ){ ResBuf[Idx++] = '.';}
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		Lcd Operator HEX Set																		*/
/*																									*/
/****************************************************************************************************/
void	RpxLcdSetValHex(					/* Lcd Operator HEX Set									*/
		USHORT	Lpos,						/* 開始行位置(最上行が0)								*/
		USHORT	Dpos,						/* 開始桁位置(左端が0)									*/
		ULONG	Data,						/* 表示データ											*/
		USHORT	Digit,						/* 表示する桁数											*/
		UCHAR	*ResBuf			)			/* 応答バッファアドレス									*/
{
LONG	i,j;
LONG	HexWk;
LONG	Idx = LCDMSGR_CHRIDX + (Lpos*18) + Dpos;

/*--------------------------------------------------------------------------------------------------*/
/*		Check Digit																					*/
/*--------------------------------------------------------------------------------------------------*/
		if( (Digit == 0) || (8 < Digit) )
		{
			return;
		}
/*--------------------------------------------------------------------------------------------------*/
		if( Digit < 8 )
		{
			Data &= ((0x1L << (Digit*4)) -1);				/* 上位桁を落とす						*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Set Hex String																				*/
/*--------------------------------------------------------------------------------------------------*/
		for( i=0; i < Digit; i++ )
		{
			j = Digit - 1 - i;								/* Digit Index							*/
			HexWk = (Data >> (j*4)) & 0x0F;					/* Display Hex Value					*/
			ResBuf[Idx+i] = (HexWk < 0xA)? (UCHAR)('0' + HexWk) : (UCHAR)('A' + (HexWk - 0xA));
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		Lcd Operator BIT Set																		*/
/*																									*/
/****************************************************************************************************/
void	RpxLcdSetBit(						/* Lcd Operator BIT Set									*/
		USHORT	Lpos,						/* 開始行位置(最上行が0)								*/
		USHORT	Dpos,						/* 開始桁位置(左端が0)									*/
		UCHAR	Data,						/* 表示データ											*/
		UCHAR	*ResBuf			)			/* 応答バッファアドレス									*/
{
LONG	i;
LONG	Idx = LCDMSGR_CHRIDX + (Lpos*18) + Dpos;

		for(i = 0; i < 8; i++)
		{
			ResBuf[Idx+i] = (Data & (0x1 << (8-i-1)))? 0x11: 0x10;
		}
		return;
}



/****************************************************************************************************/
/*																									*/
/*		Lcd Operator LCD Blink Set																	*/
/*																									*/
/****************************************************************************************************/
void	RpxLcdSetBlnk(						/* Lcd Operator LCD Blink Set							*/
		USHORT	Lpos,						/* 開始行位置(最上行が0)								*/
		USHORT	Dpos,						/* 開始桁位置(左端が0)									*/
		USHORT	Length,						/* 文字数												*/
		UCHAR	*ResBuf			)			/* 応答バッファアドレス									*/
{
LONG	i;
ULONG	BlnkWk;

/*--------------------------------------------------------------------------------------------------*/
/*		Check Arguments																				*/
/*--------------------------------------------------------------------------------------------------*/
		if( (Lpos >= 5) || (Dpos >= 17) || (Length == 0) )
		{
			return;		/* 引数不適	*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Set DposWk,LenWk																			*/
/*--------------------------------------------------------------------------------------------------*/
		Length = ((Dpos+Length) > 17)? (17-Dpos) : Length;		/* 長すぎる場合、右端まで			*/
		if( Dpos == 0 )
		{
			Length = (Length <= 2)? 1 : (Length - 1);			/* セットする点滅ビット数			*/
		}
		else
		{
			Dpos = Dpos - 1;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Set Blink																					*/
/*--------------------------------------------------------------------------------------------------*/
		for( BlnkWk=0,i=0; i < Length; i++ )
		{
			BlnkWk |= ( 1 << (Dpos+i) );
		}
		MlibCnvLittle16( BlnkWk, BlnkWk );
		*(USHORT *)(&(ResBuf[LCDMSGR_DSPIDX+4+Lpos*2])) |= BlnkWk; 
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		Lcd Operator LCD Blink Stop																	*/
/*																									*/
/****************************************************************************************************/
void	RpxLcdClrLineBlnk(					/* Lcd Operator LCD Blink Stop							*/
		USHORT	Lpos,						/* 開始行位置(最上行が0)								*/
		UCHAR	*ResBuf			)			/* 応答バッファアドレス									*/
{
		if( Lpos >= 5 ){ return;}	/* 引数不適	*/
		*(USHORT *)(&(ResBuf[LCDMSGR_DSPIDX+4+Lpos*2])) = 0x0000;
		return;
}




#if	0	/* <S208> */
/****************************************************************************************************/
/*																									*/
/*		Initialize Display Inf. Table																*/
/*																									*/
/****************************************************************************************************/
void	LcdInitDispInfoTable( void )
{
LONG	i,rc;
LONG	dispID;
LONG	ChkSum;
LONG	IdxWrk;
//LCDEEP	LcdEep;
LCDEEP	LcdEep;			/* <S18F> */
USHORT	UnIdx;			/* <S18F> */
USHORT	PnIdx;			/* <S18F> */
#if		CCSW_HWID == HWID_JPCV
CLOPDITDEF	*pLopDitDef= LopDitDef_Rotary;
#else
CLOPDITDEF	*pLopDitDef= /* TODO: (Kprm.AmpType==AMPTYPE_LINEAR)? LopDitDef_Linear :*/ LopDitDef_Rotary;
#endif
		
#if 1 /* TODO */	
COBJECT *pObjMon = (COBJECT *)CUNMONX;
COBJECT *pObjPrm = (COBJECT *)CPARAMX;
		for(dispID = 0; dispID < LCDPMDSP_MAX; dispID++)
		{
			//KpiResetWatchdog( ); 
			for( i=0; i < 4; i++ )
			{
				LopDsp.InfTbl[dispID][i].PnUn = pLopDitDef[dispID].DspDef[i].PnUn;
			/*--------------------------------------------------------------------------------------*/
				pObjMon->Search( pObjMon, pLopDitDef[dispID].DspDef[i].UnIdx, 
									0, &IdxWrk, MCHK_NONE, &LopMsg );
				LopDsp.InfTbl[dispID][i].UnIdx = IdxWrk;
			/*--------------------------------------------------------------------------------------*/
				pObjPrm->Search( pObjPrm, pLopDitDef[dispID].DspDef[i].PnIdx, 
									0, &IdxWrk, PCHK_NONE, &LopMsg );
				LopDsp.InfTbl[dispID][i].PnIdx = IdxWrk;
			/*--------------------------------------------------------------------------------------*/
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		LopDsp.CurFunIndx = 0 /* TODO: RpxFunSearchIdxOp( LcdEep.Old_FnNo, 0, &LopMsg ) */;
/*--------------------------------------------------------------------------------------------------*/
#endif
//#if 0 /* minaosi */
#if 1	/* <S18F> */
/*--------------------------------------------------------------------------------------------------*/
/*		ROMデフォルト値で初期化 (ここでPn/Un番号をインデックスに変換する)							*/
/*--------------------------------------------------------------------------------------------------*/
		for(dispID = 0; dispID < LCDPMDSP_MAX; dispID++)
		{
			/* it takes about 600 us while the cpu processes loop below */
			//KpiResetWatchdog( ); /* <0BJ14> */	/* <S18F> */
			/*--------------------------------------------------------------------------------------*/
			for( i=0; i < 4; i++ )
			{
			/*--------------------------------------------------------------------------------------*/
				LopDsp.InfTbl[dispID][i].PnUn = pLopDitDef[dispID].DspDef[i].PnUn;
			/*--------------------------------------------------------------------------------------*/
				//rc = RpiMonSearch( pLopDitDef[dispID].DspDef[i].UnIdx, 0, &IdxWrk, MCHK_NONE, &LopMsg );
				UnIdx = pLopDitDef[dispID].DspDef[i].UnIdx;									/* <S18F> */
				rc = CUNMONX->Search( (COBJECT *)CUNMONX, UnIdx, 0, &IdxWrk, MCHK_NONE, &LopMsg );		/* <S18F> */
				LopDsp.InfTbl[dispID][i].UnIdx = (rc==OK)? IdxWrk : 0;
			/*--------------------------------------------------------------------------------------*/
				//rc = RpxPrmSearchIdx( pLopDitDef[dispID].DspDef[i].PnIdx, 0, &IdxWrk, PCHK_NONE, &LopMsg );
				PnIdx = pLopDitDef[dispID].DspDef[i].PnIdx;									/* <S18F> */
				rc = CPARAMX->Search( (COBJECT *)CPARAMX, PnIdx, 0, &IdxWrk, PCHK_NONE, &LopMsg );		/* <S18F> */
				LopDsp.InfTbl[dispID][i].PnIdx = (rc==OK)? IdxWrk : 0;
			/*--------------------------------------------------------------------------------------*/
			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		EEPROM読み出し																				*/
/*--------------------------------------------------------------------------------------------------*/
		//KriReadEepromData( EEPADR_LCDINF, &LcdEep, LCDEEP_WDSIZE );
		KriReadEepromData( EEPADR_LCDINF, (USHORT*)(&LcdEep), LCDEEP_WDSIZE );		/* <S18F> */
		for( ChkSum=0, i=0; i < LCDEEP_WDSIZE; i++ )
		{
			ChkSum += *((USHORT *)(&LcdEep) + i);
		}
		if( (USHORT)ChkSum != 0 ){ return;}
/*--------------------------------------------------------------------------------------------------*/
/*		EEPROM情報で上書き																			*/
/*--------------------------------------------------------------------------------------------------*/
		for( dispID=0; dispID < LCDPMDSP_MAX; dispID++ )
		{
			if( (LcdEep.accesslvl[dispID] != 0) && 					/* 無効データでない				*/
				(LopMsg.AccessLvl >= LcdEep.accesslvl[dispID]) &&	/* アクセスレベルが合っている	*/
				( ( (LopMsg.AccessLvl == ACCLVL_USER1) &&			/* ユーザ定数選択表示可能		*/
				    /*( (Iprm.OpeDspMode == OPEDSP_ALLPRM) ||*/
				    ( (LopMsg.OpeDspMode == OPEDSP_ALLPRM) ||		/* <S18F> */
				      (LcdEep.Old_OpeDspMode == OPEDSP_SETUPPRM) ) ) ||
				  (LopMsg.AccessLvl >= ACCLVL_USER2) ) )
			{
				//KpiResetWatchdog( ); /* <0BJ14> */	/* <S18F> */
				/*----------------------------------------------------------------------------------*/
				for( i=0; i < 4; i++ )
				{
					LONG ValidFlag = FALSE;
					//if(	RpiMonSearch( LcdEep.dispinf[dispID].UnNo[i],
					//		0, &IdxWrk, MCHK_ACCS_LVL | MCHK_SP_USE, &LopMsg ) == OK	)
					UnIdx = LcdEep.dispinf[dispID].UnNo[i];									/* <S18F> */
					if( CUNMONX->Search( (COBJECT *)CUNMONX, UnIdx,							/* <S18F> */
							0, &IdxWrk, MCHK_ACCS_LVL | MCHK_SP_USE, &LopMsg ) == OK )		/* <S18F> */
					{
						ValidFlag = TRUE;
						LopDsp.InfTbl[dispID][i].UnIdx = IdxWrk;
					}
					/*------------------------------------------------------------------------------*/
					//if( RpxPrmSearchIdx( LcdEep.dispinf[dispID].PnNo[i],
					//		0, &IdxWrk, PCHK_ACCS_LVL | PCHK_SP_USE, &LopMsg ) == OK	)
					PnIdx = LcdEep.dispinf[dispID].PnNo[i];									/* <S18F> */
					if( CPARAMX->Search( (COBJECT *)CPARAMX, PnIdx,							/* <S18F> */
							0, &IdxWrk, PCHK_ACCS_LVL | PCHK_SP_USE, &LopMsg ) == OK )		/* <S18F> */
					{
						ValidFlag = TRUE;
						LopDsp.InfTbl[dispID][i].PnIdx = IdxWrk;
					}
					/*------------------------------------------------------------------------------*/
					if( ValidFlag )		/* Pn or Un が有効な場合のみPn/Un種類をセットする	*/
					{
						LopDsp.InfTbl[dispID][i].PnUn = LcdEep.dispinf[dispID].PnUn[i];
					}
				}
				/*----------------------------------------------------------------------------------*/
			}
		}
	/*----------------------------------------------------------------------------------------------*/
		//LopDsp.CurFunIndx = RpxFunSearchIdxOp( LcdEep.Old_FnNo, 0, &LopMsg );
		LopDsp.CurFunIndx = CLCDFUN->SearchB( CLCDFUN, LcdEep.Old_FnNo, 0, &LopMsg );	/* <S18F> */
/*--------------------------------------------------------------------------------------------------*/
#endif
		return;
}



/****************************************************************************************************/
/*																									*/
/*		Save Display Inf. Table																		*/
/*																									*/
/****************************************************************************************************/
SHORT	LcdSaveDispInfoTable( USHORT dispID )
{
//#if 0 /* minaosi */
#if 1	/* <S18F> */
LONG	i;
LONG	ChkSum;
LCDEEP	LcdEep;
#if CCSW_CMDID == CMDID_EXIST		/*<11J42>*/
LOPDIT	*LcdInfTblDef;
USHORT	first_lcdinf_save = FALSE;
		/* 電源投入後最初のWRITEキー押下時 */
		if( LcdOpt[dispID].f.LcdInfSaved == FALSE )
		{
			/* Default LcdInfTblの決定	*/
			if( Kprm.AmpType == AMPTYPE_LINEAR )
			{
				LcdInfTblDef = (LOPDIT *)(&(LopDitDef_Linear[dispID]));
			}
			else
			{
				LcdInfTblDef = (LOPDIT *)(&(LopDitDef_Rotary[dispID]));
			}
			/* 初回WRITEキー押下フラグクリア */
			LcdOpt[dispID].f.LcdInfSaved = TRUE;
			first_lcdinf_save = TRUE;
		}
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		EEPROM読み出し																				*/
/*--------------------------------------------------------------------------------------------------*/
		//KriReadEepromData( EEPADR_LCDINF, &LcdEep, LCDEEP_WDSIZE );
		KriReadEepromData( EEPADR_LCDINF, (USHORT*)(&LcdEep), LCDEEP_WDSIZE );		/* <S18F> */
		for( ChkSum=0, i=0; i < LCDEEP_WDSIZE; i++ )
		{
			ChkSum += *((USHORT *)(&LcdEep) + i);
		}
		if( (USHORT)ChkSum != 0 )
		{
			for( i=0; i < LCDPMDSP_MAX; i++ )				/* サムが合わない場合は、				*/
			{												/* 情報を無効扱いにする					*/
				LcdEep.accesslvl[i] = 0;					/* (dsipIDだけは後で生かす)				*/
			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		EEPROM書込み																				*/
/*--------------------------------------------------------------------------------------------------*/
		LcdEep.accesslvl[dispID] = LopMsg.AccessLvl;
		//LcdEep.Old_OpeDspMode = Iprm.OpeDspMode;
		LcdEep.Old_OpeDspMode = LopMsg.OpeDspMode;									/* <S18F> */
		for( i=0; i < 4; i++ )
		{
#if CCSW_CMDID == CMDID_EXIST
			if( LcdOpt[dispID].Prm[i].State.Editing == TRUE )
			{
				if( first_lcdinf_save == TRUE )
				{	/* オプションパラメータ編集中の行はROMデフォルト値を設定 */
					LcdEep.dispinf[dispID].PnUn[i] = LcdInfTblDef[i].PnUn;
					LcdEep.dispinf[dispID].UnNo[i] = LcdInfTblDef[i].UnIdx;	/* Un番号 */
					LcdEep.dispinf[dispID].PnNo[i] = LcdInfTblDef[i].PnIdx;	/* Pn番号 */
				}
			}
			else
			{
#endif
#if 0			/* <S18F> */
				LcdEep.dispinf[dispID].PnUn[i] = LopDsp.InfTbl[dispID][i].PnUn;
				LcdEep.dispinf[dispID].UnNo[i] = UnMonTbl[LopDsp.InfTbl[dispID][i].UnIdx].UnNo;
				LcdEep.dispinf[dispID].PnNo[i] = PnPrmTbl[LopDsp.InfTbl[dispID][i].PnIdx].PnNo;
#else
				USHORT	Numx;
				USHORT	UnIdx = LopDsp.InfTbl[dispID][i].UnIdx;
				USHORT	PnIdx = LopDsp.InfTbl[dispID][i].PnIdx;
				LcdEep.dispinf[dispID].PnUn[i] = LopDsp.InfTbl[dispID][i].PnUn;
				Numx = CUNMONX->GetNumber( (CMONITOR *)CUNMONX, UnIdx, &LopMsg );
				LcdEep.dispinf[dispID].UnNo[i] = Numx;
				Numx = CPARAMX->GetNumber( (CPARAM *)CPARAMX, PnIdx, &LopMsg );
				LcdEep.dispinf[dispID].PnNo[i] = Numx;
#endif
#if CCSW_CMDID == CMDID_EXIST
			}
#endif
		}
	/*----------------------------------------------------------------------------------------------*/
		for( ChkSum=0, i=0; i < (LCDEEP_WDSIZE - 1); i++ )
		{
			ChkSum += *((USHORT *)(&LcdEep) + i);
		}
		LcdEep.chksum = (USHORT)(-ChkSum);
		//KriWriteEepromData( EEPADR_LCDINF, &LcdEep, LCDEEP_WDSIZE );
		KriWriteEepromData( EEPADR_LCDINF, (USHORT*)(&LcdEep), LCDEEP_WDSIZE );			/* <S18F> */
/*--------------------------------------------------------------------------------------------------*/
#endif
		return( OK );
}

#else	/* <S208> */
/****************************************************************************************************/
/*																									*/
/*		Initialize Display Inf. Table																*/
/*																									*/
/****************************************************************************************************/
void	LcdInitDispInfoTable( AXIS_HANDLE *Axis )
{
LONG	i;
LONG	rc;
LONG	ax;
LONG	dispID;
LONG	ChkSum;
LONG	IdxWrk;
LONG	ValidFlag;
USHORT	UnIdx;
USHORT	PnIdx;

//CLOPDITDEF	*pLopDitDef= LopDitDef_Rotary;
CLOPDITDEF	*pLopDitDef;									/* <S214> */
AXIS_HANDLE *AxisRsc;										/* <S214> */

	for( ax = 0 ; ax < MAX_AXIS_NUMBER ; ax++ )
	{
/* <S214> */
		AxisRsc = &Axis[ax];
		if( AxisRsc->MencV->AxisMotType == MOTTYPE_LINEAR )	{ pLopDitDef = LopDitDef_Linear; }
		else												{ pLopDitDef = LopDitDef_Rotary; }
/* <S214> */
/*--------------------------------------------------------------------------------------------------*/
/*		ROMデフォルト値で初期化 (ここでPn/Un番号をインデックスに変換する)							*/
/*--------------------------------------------------------------------------------------------------*/
		for( dispID = 0 ; dispID < LCDPMDSP_MAX ; dispID++ )
		{
			/*--------------------------------------------------------------------------------------*/
			for( i = 0 ; i < 4 ; i++ )
			{
			/*--------------------------------------------------------------------------------------*/
				LopDsp.InfTbl[dispID][i].PnUn = pLopDitDef[dispID].DspDef[i].PnUn;
			/*--------------------------------------------------------------------------------------*/
				UnIdx = pLopDitDef[dispID].DspDef[i].UnIdx;
				rc = CUNMONX->Search( (COBJECT *)CUNMONX, UnIdx, 0, &IdxWrk, MCHK_NONE, &LopMsg );
				LopDsp.InfTbl[dispID][i].UnIdx = ( rc == OK )? IdxWrk : 0;
			/*--------------------------------------------------------------------------------------*/
				PnIdx = pLopDitDef[dispID].DspDef[i].PnIdx;
				rc = CPARAMX->Search( (COBJECT *)CPARAMX, PnIdx, 0, &IdxWrk, PCHK_NONE, &LopMsg );
				LopDsp.InfTbl[dispID][i].PnIdx = ( rc == OK )? IdxWrk : 0;
			/*--------------------------------------------------------------------------------------*/
			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		EEPROM読み出し																				*/
/*--------------------------------------------------------------------------------------------------*/
		KriReadEepromData( EEP_NOSUM_ADDRESS( ax, EEP_LCDINF_OFS ), (USHORT*)(&LcdEep[ax]), LCDEEP_WDSIZE );
		for( ChkSum = 0, i = 0 ; i < LCDEEP_WDSIZE ; i++ )
		{
			ChkSum += *((USHORT *)(&LcdEep) + i);
		}
		if( (USHORT)ChkSum == 0 )
		{
/*--------------------------------------------------------------------------------------------------*/
/*		EEPROM情報で上書き																			*/
/*--------------------------------------------------------------------------------------------------*/
			for( dispID = 0 ; dispID < LCDPMDSP_MAX ; dispID++ )
			{
				if(( LcdEep[ax].accesslvl[dispID] != 0 ) && 			/* 無効ﾃﾞｰﾀでない			*/
				   ( LopMsg.AccessLvl >= LcdEep[ax].accesslvl[dispID] ) &&	/* ｱｸｾｽﾚﾍﾞﾙが合っている	*/
				   ((( LopMsg.AccessLvl == ACCLVL_USER1 ) &&			/* ﾕｰｻﾞ定数選択表示可能		*/
				     (( LopMsg.OpeDspMode == OPEDSP_ALLPRM ) ||
				      ( LcdEep[ax].Old_OpeDspMode == OPEDSP_SETUPPRM ))) ||
				   ( LopMsg.AccessLvl >= ACCLVL_USER2 )))
				{
					/*------------------------------------------------------------------------------*/
					for( i = 0; i < 4 ; i++ )
					{
						ValidFlag = FALSE;
						UnIdx = LcdEep[ax].dispinf[dispID].UnNo[i];
						if( CUNMONX->Search( (COBJECT *)CUNMONX, UnIdx,
								0, &IdxWrk, MCHK_ACCS_LVL | MCHK_SP_USE, &LopMsg ) == OK )
						{
							ValidFlag = TRUE;
							LopDsp.InfTbl[dispID][i].UnIdx = IdxWrk;
						}
						/*--------------------------------------------------------------------------*/
						PnIdx = LcdEep[ax].dispinf[dispID].PnNo[i];
						if( CPARAMX->Search( (COBJECT *)CPARAMX, PnIdx,
								0, &IdxWrk, PCHK_ACCS_LVL | PCHK_SP_USE, &LopMsg ) == OK )
						{
							ValidFlag = TRUE;
							LopDsp.InfTbl[dispID][i].PnIdx = IdxWrk;
						}
						/*--------------------------------------------------------------------------*/
						if( ValidFlag )		/* Pn or Un が有効な場合のみPn/Un種類をセットする	*/
						{
							LopDsp.InfTbl[dispID][i].PnUn = LcdEep[ax].dispinf[dispID].PnUn[i];
						}
					}
					/*------------------------------------------------------------------------------*/
				}
			}
		/*------------------------------------------------------------------------------------------*/
			LopDsp.CurFunIndx = CLCDFUN->SearchB( CLCDFUN, LcdEep[ax].Old_FnNo, 0, &LopMsg );
		}
/*--------------------------------------------------------------------------------------------------*/
/*		EEPROM情報初期化																			*/
/*--------------------------------------------------------------------------------------------------*/
		for( dispID = 0 ; dispID < LCDPMDSP_MAX ; dispID++ )
		{
			/*--------------------------------------------------------------------------------------*/
			for( i = 0 ; i < 4 ; i++ )
			{
				UnIdx = LopDsp.InfTbl[dispID][i].UnIdx;
				PnIdx = LopDsp.InfTbl[dispID][i].PnIdx;
				LcdEep[ax].dispinf[dispID].PnUn[i] = LopDsp.InfTbl[dispID][i].PnUn;
				IdxWrk = CUNMONX->GetNumber( (CMONITOR *)CUNMONX, UnIdx, &LopMsg );
				LcdEep[ax].dispinf[dispID].UnNo[i] = IdxWrk;
				IdxWrk = CPARAMX->GetNumber( (CPARAM *)CPARAMX, PnIdx, &LopMsg );
				LcdEep[ax].dispinf[dispID].PnNo[i] = IdxWrk;
			}
		}
	/*----------------------------------------------------------------------------------------------*/
		for( ChkSum = 0, i = 0 ; i < ( LCDEEP_WDSIZE - 1 ) ; i++ )
		{
			ChkSum += *((USHORT *)( &LcdEep[ax] ) + i );
		}
		LcdEep[ax].chksum = (USHORT)(-ChkSum);
	/*----------------------------------------------------------------------------------------------*/
	}
	/*----------------------------------------------------------------------------------------------*/
	return;
}


/****************************************************************************************************/
/*																									*/
/*		Save Display Inf. Table																		*/
/*																									*/
/****************************************************************************************************/
SHORT	LcdSaveDispInfoTable( USHORT dispID )
{
LONG	i;
LONG	ChkSum;
USHORT	Numx;
USHORT	UnIdx;
USHORT	PnIdx;

#if CCSW_CMDID == CMDID_EXIST
LOPDIT	*LcdInfTblDef;
USHORT	first_lcdinf_save = FALSE;
		/* 電源投入後最初のWRITEキー押下時 */
		if( LcdOpt[dispID].f.LcdInfSaved == FALSE )
		{
			/* Default LcdInfTblの決定	*/
			if( Kprm.AmpType == AMPTYPE_LINEAR )
			{
				LcdInfTblDef = (LOPDIT *)(&(LopDitDef_Linear[dispID]));
			}
			else
			{
				LcdInfTblDef = (LOPDIT *)(&(LopDitDef_Rotary[dispID]));
			}
			/* 初回WRITEキー押下フラグクリア */
			LcdOpt[dispID].f.LcdInfSaved = TRUE;
			first_lcdinf_save = TRUE;
		}
#endif

/*--------------------------------------------------------------------------------------------------*/
		if( LcdWtReq == TRUE )	return( NG );					/* 書き込み中						*/
/*--------------------------------------------------------------------------------------------------*/
/*		EEPROM読み出し																				*/
/*--------------------------------------------------------------------------------------------------*/
		for( ChkSum=0, i = 0 ; i < LCDEEP_WDSIZE ; i++ )
		{
			ChkSum += *((USHORT *)( pLcdEep ) + i );
		}
		if( (USHORT)ChkSum != 0 )
		{
			for( i = 0; i < LCDPMDSP_MAX ; i++ )			/* サムが合わない場合は、				*/
			{												/* 情報を無効扱いにする					*/
				pLcdEep->accesslvl[i] = 0;					/* (dsipIDだけは後で生かす)				*/
			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		EEPROM書込み編集																			*/
/*--------------------------------------------------------------------------------------------------*/
		pLcdEep->accesslvl[dispID] = LopMsg.AccessLvl;
		pLcdEep->Old_OpeDspMode    = LopMsg.OpeDspMode;
		for( i = 0 ; i < 4 ; i++ )
		{
#if CCSW_CMDID == CMDID_EXIST
			if( LcdOpt[dispID].Prm[i].State.Editing == TRUE )
			{
				if( first_lcdinf_save == TRUE )
				{	/* オプションパラメータ編集中の行はROMデフォルト値を設定 */
					pLcdEep->dispinf[dispID].PnUn[i] = LcdInfTblDef[i].PnUn;
					pLcdEep->dispinf[dispID].UnNo[i] = LcdInfTblDef[i].UnIdx;	/* Un番号 */
					pLcdEep->dispinf[dispID].PnNo[i] = LcdInfTblDef[i].PnIdx;	/* Pn番号 */
				}
			}
			else
			{
#endif
				UnIdx = LopDsp.InfTbl[dispID][i].UnIdx;
				PnIdx = LopDsp.InfTbl[dispID][i].PnIdx;
				pLcdEep->dispinf[dispID].PnUn[i] = LopDsp.InfTbl[dispID][i].PnUn;
				Numx = CUNMONX->GetNumber( (CMONITOR *)CUNMONX, UnIdx, &LopMsg );
				pLcdEep->dispinf[dispID].UnNo[i] = Numx;
				Numx = CPARAMX->GetNumber( (CPARAM *)CPARAMX, PnIdx, &LopMsg );
				pLcdEep->dispinf[dispID].PnNo[i] = Numx;
#if CCSW_CMDID == CMDID_EXIST
			}
#endif
		}
	/*----------------------------------------------------------------------------------------------*/
		for( ChkSum=0, i=0; i < (LCDEEP_WDSIZE - 1); i++ )
		{
			ChkSum += *((USHORT *)( pLcdEep ) + i );
		}
		pLcdEep->chksum = (USHORT)(-ChkSum);
/*--------------------------------------------------------------------------------------------------*/
/*		EEPROM書込み要求																			*/
/*--------------------------------------------------------------------------------------------------*/
		LcdRomAdr = LcdRomBaseAdr;
		LcdMemAdr = (USHORT*)( pLcdEep );
		LcdWtCnt  = LCDEEP_WDSIZE;
		LcdWtReq  = TRUE;
/*--------------------------------------------------------------------------------------------------*/
		return( OK );
}


/****************************************************************************************************/
/*																									*/
/*		表示テーブルロード																			*/
/*																									*/
/****************************************************************************************************/
void	LopDspDataLoad( LONG AxisNo )
{
LONG	i;
LONG	dispID;
LONG	IdxWrk;
LONG	ValidFlag;
USHORT	UnIdx;
USHORT	PnIdx;
	
	pLcdEep       = &LcdEep[ AxisNo ];
	LcdRomBaseAdr = EEP_NOSUM_ADDRESS( AxisNo, EEP_LCDINF_OFS );

	for( dispID = 0 ; dispID < LCDPMDSP_MAX ; dispID++ )
	{
		/*------------------------------------------------------------------------------------------*/
		for( i = 0; i < 4 ; i++ )
		{
			ValidFlag = FALSE;
			UnIdx = pLcdEep->dispinf[dispID].UnNo[i];
			if( CUNMONX->Search( (COBJECT *)CUNMONX, UnIdx,
					0, &IdxWrk, MCHK_ACCS_LVL | MCHK_SP_USE, &LopMsg ) == OK )
			{
				ValidFlag = TRUE;
				LopDsp.InfTbl[dispID][i].UnIdx = IdxWrk;
			}
			/*--------------------------------------------------------------------------*/
			PnIdx = pLcdEep->dispinf[dispID].PnNo[i];
			if( CPARAMX->Search( (COBJECT *)CPARAMX, PnIdx,
					0, &IdxWrk, PCHK_ACCS_LVL | PCHK_SP_USE, &LopMsg ) == OK )
			{
				ValidFlag = TRUE;
				LopDsp.InfTbl[dispID][i].PnIdx = IdxWrk;
			}
			/*--------------------------------------------------------------------------*/
			if( ValidFlag )		/* Pn or Un が有効な場合のみPn/Un種類をセットする	*/
			{
				LopDsp.InfTbl[dispID][i].PnUn = pLcdEep->dispinf[dispID].PnUn[i];
			}
		}
		/*------------------------------------------------------------------------------*/
	}
		/*------------------------------------------------------------------------------------------*/
	LopDsp.CurFunIndx = CLCDFUN->SearchB( CLCDFUN, pLcdEep->Old_FnNo, 0, &LopMsg );
}


/****************************************************************************************************/
/*																									*/
/*		EEPROM書き込み																				*/
/*																									*/
/****************************************************************************************************/
void	LcdEep2EepRomWrite( UINT devId )
{
	if( LcdWtReq == TRUE )
	{
		if( LcdWtCnt != 0 )
		{
			EepdevWriteNoSumValues( devId, LcdRomAdr, LcdMemAdr, 1 );
			LcdMemAdr += 1;
			LcdRomAdr += 2;
			LcdWtCnt--;
		}
		if( LcdWtCnt == 0 )
		{
			LcdWtReq = FALSE;
		}
	}
}
/*--------------------------------------------------------------------------------------------------*/
#endif	/* <S208> */

/****************************************************************************************************/
/*																									*/
/*		Lcd Response Send																			*/
/*																									*/
/****************************************************************************************************/
void	LcdResSend( MSGIF *hMsg )
{
LONG	TxSize;
//USHORT	LedSts  = 0;//<S0C7>
UCHAR	*CmdBuf = hMsg->CmdBuf;
UCHAR	*ResBuf = hMsg->ResBuf;
CSIF *Sif = hMsg->Sif;
/*--------------------------------------------------------------------------------------------------*/
/*		ブリンクとステータスは各画面処理の中で作成する。											*/
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/*		ヘッダ部セット																				*/
/*--------------------------------------------------------------------------------------------------*/
		ResBuf[LCDMSGR_AXIDX]  = CmdBuf[0];					/* 軸アドレス							*/
		ResBuf[LCDMSGR_FUNIDX] = LopV.CurMsgCmd /* CmdBuf[1] */;/* <S031> コマンド					*/
/*--------------------------------------------------------------------------------------------------*/
/*		チェックサムセット																			*/
/*--------------------------------------------------------------------------------------------------*/
		TxSize = LPX_GET_MSGLEN( ResBuf );					/* Get Tx Size 							*/
		MlibCnvLittle16( TxSize, TxSize );					/* Convert Endian(Big/Little)			*/
		LPX_SET_MSGLEN( ResBuf, TxSize  );					/* Set Message Length 					*/
		MlibSetSum16SM( ResBuf, TxSize );					/* Set ChkSum16 for SigmaMessage		*/
/*--------------------------------------------------------------------------------------------------*/
/*		送信指令																					*/
/*--------------------------------------------------------------------------------------------------*/
		Sif->StxRes( Sif, ResBuf, TxSize );					/* 送信開始指令							*/
		KpiRstLongTimer( &LopMsg.ComChkTime );				/* For RES/CMD Timeout check			*/
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		Lcd Send Response : Set Status																*/
/*																									*/
/****************************************************************************************************/
void	RpxLcdSetResStatus( UCHAR *ResBuf )					/* <S031> */
{
USHORT	CurAlarm;
UCHAR	*pSetSts;
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W（2軸対応）のみ	*//* <S0E4> */
if(LopV.OpMode == LCDOM_SelectAxis)
{	/*	軸選択画面では1行目タイトルの現状維持	*/
	RpxLcdSetStr(0, 0, (UCHAR *)CSTR_SELAXS_TITLE, (UCHAR *)ResBuf);
	return;
}
else
{	/*	軸選択画面以外	*/
/*--------------------------------------------------------------------------------------------------*/
/*		軸番号表示																					*/
/*--------------------------------------------------------------------------------------------------*/
	/*	軸番号をＡＳＣＩＩコードへ変換	*/
	ResBuf[LCDMSGR_CHRIDX+0] = DrvSel.Unit[0].DispCurAxis + 0x30 ;
	ResBuf[LCDMSGR_CHRIDX+1] = ':';
/*--------------------------------------------------------------------------------------------------*/
/*		アラームコード・ワーニングコード表示														*/
/*--------------------------------------------------------------------------------------------------*/
	/* 選択軸のポインターをセットしてアラーム情報を取得 *//* <S1A4> */
	pAxRsc = (AXIS_HANDLE*)GetAxisHandle(DrvSel.Unit[0].DispCurAxis - 1);
	if( (CurAlarm = KpiGetAlarmInfo()) != 0x0000 )
	{
		RpxLcdSetStr(0, 2, (UCHAR *)"A.", (UCHAR *)ResBuf);
		RpxLcdSetValHex(0, 4, CurAlarm, 3, (UCHAR *)ResBuf);
		return;
	}
}
#else
/*--------------------------------------------------------------------------------------------------*/
/*		軸番号表示																					*/
/*--------------------------------------------------------------------------------------------------*/
	ResBuf[LCDMSGR_CHRIDX+0] = CCHR_AXISNUM ;
	ResBuf[LCDMSGR_CHRIDX+1] = ':';
/*--------------------------------------------------------------------------------------------------*/
/*		アラームコード・ワーニングコード表示														*/
/*--------------------------------------------------------------------------------------------------*/
	if( (CurAlarm = KpiGetAlarmInfo()) != 0x0000 )
	{
		RpxLcdSetStr(0, 2, (UCHAR *)"A.", (UCHAR *)ResBuf);
		RpxLcdSetValHex(0, 4, CurAlarm, 3, (UCHAR *)ResBuf);
		return;
	}
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		コンバータステータス表示処理																*/
/*--------------------------------------------------------------------------------------------------*/
		if( CHK_BB()				    ){ pSetSts = (UCHAR *)"*BB";	}
		else if( CHK_PDET() 			){ pSetSts = (UCHAR *)"*P DET";	}
		else							 { pSetSts = (UCHAR *)"*RUN";	}
	/*----------------------------------------------------------------------------------------------*/
		if( CHK_TUNING() 				){ pSetSts = (UCHAR *)"*ADJ";	}
	/*----------------------------------------------------------------------------------------------*/
		if( !CHK_JOGMODE() && !CHK_ZRETMODE() )
		{
			if( CHK_POT() && CHK_NOT()	){ pSetSts = (UCHAR *)"*PT NT";	}
			else if( CHK_POT()			){ pSetSts = (UCHAR *)"*P-OT";	}
			else if( CHK_NOT()			){ pSetSts = (UCHAR *)"*N-OT";	}
			else if( CHK_PLS()			){ pSetSts = (UCHAR *)"*P-LS";	}
			else if( CHK_NLS()			){ pSetSts = (UCHAR *)"*N-LS";	}
		}
	/*----------------------------------------------------------------------------------------------*/
		if( CHK_ESTOP()					){ pSetSts = (UCHAR *)"*ESTOP";	}
	/*----------------------------------------------------------------------------------------------*/
		if( CHK_FSTP()					){ pSetSts = (UCHAR *)"*FSTP";	} 			/* <S1F5> */
	/*----------------------------------------------------------------------------------------------*/
		if( CHK_HBB() 					){ pSetSts = (UCHAR *)"*HBB";	}
	/*----------------------------------------------------------------------------------------------*/
		/* モータレステストモードでなければポインタを1進める *//* <S1B7> */
//		if( 1 							){ pSetSts = pSetSts + 1;		}
		if( pAxRsc->MotorLessTest->MotorLessTestMode == FALSE ){ pSetSts = pSetSts + 1;		}
/*--------------------------------------------------------------------------------------------------*/
/*		ステータス表示文字列設定処理																*/
/*--------------------------------------------------------------------------------------------------*/
		RpxLcdSetStr( 0, 2, pSetSts, ResBuf );
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		Lcd Send Response : Set LED Status															*/
/*																									*/
/****************************************************************************************************/
void	LcdSetResLedSts( USHORT *pLedSts )
{
/*--------------------------------------------------------------------------------------------------*/
/*		LED表示処理																					*/
/*--------------------------------------------------------------------------------------------------*/
		if( CHK_SVON()							){ *pLedSts |= LCDLED_SVON;}
		if( CHK_TGON() 							){ *pLedSts |= LCDLED_TGON;}
		if( CHK_CHG()							){ *pLedSts |= LCDLED_CHG; }
/*--------------------------------------------------------------------------------------------------*/
		if( CHK_COIN()							){ *pLedSts |= LCDLED_COIN;}
		if( CHK_VCMP() 							){ *pLedSts |= LCDLED_COIN;}
		if( CHK_TRQMODE() 						){ *pLedSts |= LCDLED_COIN;}
/*--------------------------------------------------------------------------------------------------*/
		if( CHK_PREFON()						){ *pLedSts |= LCDLED_REF;}
		if( CHK_VREFON()						){ *pLedSts |= LCDLED_REF;}
		if( CHK_TREFON()						){ *pLedSts |= LCDLED_REF;}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		Lcd Send Response : Set Blink																*/
/*																									*/
/****************************************************************************************************/
void	LcdSetResBlink( UCHAR *ResBuf, USHORT *pLedSts )
{
/*--------------------------------------------------------------------------------------------------*/
/*		ブリンクフラグ処理																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( *((UINT *)&OpBlnkFlg) != 0 )				/* 現在ブリンク中である						*/
		{
			if( KpiGetLongTimer( &OpBlnkTime ) > 1500 )
			{
				*((UINT *)&OpBlnkFlg) = 0;				/* ブリンク終了								*/
			}
		}
		else if( *((UINT *)&OpBlnkKickFlg) != 0 )		/* ブリンク開始要求あり						*/
		{
			OpBlnkFlg = OpBlnkKickFlg;					/* Set Blink Flag							*/
			*((UINT *)&OpBlnkKickFlg) = 0;				/* Reset Blink Kick Flag					*/
			KpiRstLongTimer( &OpBlnkTime );				/* Reset Blink Timer						*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		ブリンク表示データ設定処理																	*/
/*--------------------------------------------------------------------------------------------------*/
		if( OpBlnkFlg.PrmEewrite )
		{	/* アクティブ行の右辺全体	*/
			RpxLcdSetBlnk((USHORT)(LopV.ActiveLine+1), 2+3+1, 11, (UCHAR *)ResBuf);
		}
		else if( OpBlnkFlg.read || LopV.LedBlnkSw.read )			/* READキー LED		*/
		{
			*pLedSts |= LCDLED_READB;
		}
		else if( OpBlnkFlg.write || LopV.LedBlnkSw.write )		/* WRITEキー LED	*/
		{
			*pLedSts |= LCDLED_WRITEB;
		}
		else if( OpBlnkFlg.no_op )
		{
//			RpxLcdSetStr(0, 0, (UCHAR *)"NO-OP   ", (UCHAR *)ResBuf);	/*<S054>*/
//			RpxLcdSetBlnk(0, 0, 8, (UCHAR *)ResBuf);
			RpxLcdSetStr(0, 0, (UCHAR *)"NO-OP  ", (UCHAR *)ResBuf);		/*<S054>*//*<S098>*/
			RpxLcdSetBlnk(0, 0, 7, (UCHAR *)ResBuf);
		}
		else if( OpBlnkFlg.error )
		{
//			RpxLcdSetStr(0, 0, (UCHAR *)"ERROR   ", (UCHAR *)ResBuf);	/*<S054>*/
//			RpxLcdSetBlnk(0, 0, 8, (UCHAR *)ResBuf);
			RpxLcdSetStr(0, 0, (UCHAR *)"ERROR  ", (UCHAR *)ResBuf);		/*<S054>*//*<S098>*/
			RpxLcdSetBlnk(0, 0, 7, (UCHAR *)ResBuf);
		}
		else if( OpBlnkFlg.done )
		{
//			RpxLcdSetStr(0, 0, (UCHAR *)"DONE    ", (UCHAR *)ResBuf);	/*<S054>*/
//			RpxLcdSetBlnk(0, 0, 8, (UCHAR *)ResBuf);
			RpxLcdSetStr(0, 0, (UCHAR *)"DONE   ", (UCHAR *)ResBuf);		/*<S054>*//*<S098>*/
			RpxLcdSetBlnk(0, 0, 7, (UCHAR *)ResBuf);
		}
		else if( OpBlnkFlg.end )
		{
//			RpxLcdSetStr(0, 0, (UCHAR *)"END     ", (UCHAR *)ResBuf);	/*<S054>*/
//			RpxLcdSetBlnk(0, 0, 8, (UCHAR *)ResBuf);
			RpxLcdSetStr(0, 0, (UCHAR *)"END    ", (UCHAR *)ResBuf);		/*<S054>*//*<S098>*/
			RpxLcdSetBlnk(0, 0, 7, (UCHAR *)ResBuf);
		}
/*--------------------------------------------------------------------------------------------------*/
		if( LopV.LedBlnkSw.read  ){ *pLedSts |= LCDLED_READB;}
		if( LopV.LedBlnkSw.write ){ *pLedSts |= LCDLED_WRITEB;}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		Set LED and Blink 																			*/
/*																									*/
/****************************************************************************************************/
void 	RpxLcdSetBlink (UCHAR *ResBuf)				/* <S031> 	*/
{
USHORT LedSts = 0;
	
	LcdSetResLedSts( &LedSts );						/* Set LED Status						*/
	LcdSetResBlink( ResBuf, &LedSts );				/* Set Blink							*/
	MlibCnvLittle16( LedSts, LedSts );				/* Convert Endian 						*/
	LPX_SET_LEDSTS( ResBuf, LedSts );				/* Set LED Status						*/
	*((UINT *)&LopV.LedBlnkSw) = 0;					/* Reset RED Blink Switch				*/
}

/****************************************************************************************************/
/*																									*/
/*		Change the Lcd operator display mode <S031>													*/
/*																									*/
/****************************************************************************************************/
LONG	LcdModeChg ( DRVSEL *pDrvSel )
{
LONG DrvSelEnd = FALSE; /* Drive Selection end flag */
#if		CCSW_HWID == HWID_JPCV
USHORT FnIdx; /* Fn index of fn010 */
ULONG TxSize;
UCHAR *pBuf;
#endif
/*--------------------------------------------------------------------------------------------------*/
	switch (LopV.OpMode)
	{
/*--------------------------------------------------------------------------------------------------*/
		case LCDOM_StartDisp : 	/* Start display mode */
/*--------------------------------------------------------------------------------------------------*/
#if		CCSW_HWID == HWID_JPCV
			LopV.OpMode = LpxStartDispModeCv( pDrvSel, &DrvSelEnd ); 
#else
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W（2軸対応）のみ	*/
			/* デジオペとの初期通信ブロック情報取得終了時 *//* <S19B> */
			if( RpxChkPrmCopyInfoGet( ) )
			{
				/*	現在の表示軸番号で2軸動作を判定	*/
				if(DrvSel.Unit[0].DispCurAxis != 0)
				{	/*	2軸動作の場合	*/
					LopV.OpMode = LCDOM_SelectAxis;
				}else
				{	/*	1軸動作の場合	*/
					LopV.OpMode = LCDOM_PrmMon;
				}
			}
#elif CSW_BOARD_TYPE == SGD7S_TYPE	/*	SGD7S（単軸時）のみ	*/
			LopV.OpMode = LpxStartDispModeSv( pDrvSel );
#endif
#endif
			break;

/*--------------------------------------------------------------------------------------------------*/
		case LCDOM_PrmMon : /* parameter/monitor mode */
/*--------------------------------------------------------------------------------------------------*/
			if ( /* OPWM_None */ 0 == LopV.EewriteMode ) 
			{ /* when eeprom is not written */
				if ( OpKeyMk.read ) 
				{
#if		CCSW_HWID == HWID_JPCV
					LopV.OpMode = (!pDrvSel) ? LCDOM_PrmMon : LCDOM_SelectUnit;
#else
					LopV.OpMode = (!pDrvSel) ? LCDOM_PrmMon : LCDOM_SelectAxis;
#endif
				}
				else if ( OpKeyMk.mode )
				{
					LopV.OpMode = LCDOM_Fun;
				}
				/*----------------------------------------------------------------------------------*/
				if ( LopV.OpMode != LCDOM_PrmMon) 
				{ 
					LcdRstPnUnDspData();  /* reset Pn/Un display data */
				}
			}
			break;

/*--------------------------------------------------------------------------------------------------*/
		case LCDOM_Fun : /* Fn function mode */
/*--------------------------------------------------------------------------------------------------*/
			if ( LopV.FunMode == LCDFM_Num )
			{
				if( OpKeyMk.read )
				{
	#if		CCSW_HWID == HWID_JPCV
					LopV.OpMode = (!pDrvSel) ? LCDOM_Fun : LCDOM_SelectUnit;
	#else
					LopV.OpMode = (!pDrvSel) ? LCDOM_Fun : LCDOM_SelectAxis;
	#endif
				}
				else if ( OpKeyMk.mode )
				{
//					LopV.OpMode = LCDOM_PrmMon ;
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W（2軸対応）のみ	*/
					/*	現在の表示軸番号で2軸動作を判定	*/
					if(DrvSel.Unit[0].DispCurAxis != 0)
					{	/*	2軸動作の場合パラメータコピーＯｒ軸選択へループ（01/08）	*/
						LopV.OpMode = (( SVFSEL_LCDPRMCOPY_USE == 1 ) ? LCDOM_PrmCopy : LCDOM_SelectAxis);
					}else
					{	/*	1軸動作の場合	*/
						LopV.OpMode = (( SVFSEL_LCDPRMCOPY_USE == 1 ) ? LCDOM_PrmCopy : LCDOM_PrmMon);
					}
#elif CSW_BOARD_TYPE == SGD7S_TYPE	/*	SGD7S（単軸時）のみ	*/
					LopV.OpMode = (( SVFSEL_LCDPRMCOPY_USE == 1 ) ? LCDOM_PrmCopy : LCDOM_PrmMon);
#endif
				}
			}
			else if ( LopV.FunMode == LCDFM_Exe )
			{
#if 0
				if( OpKeyMk.read )
				{
					RpxFunFinishLcd( LopDsp.CurFunIndx, &LopMsg );	/* finish the executiing Fn function */
					LopV.FunMode = LCDFM_Num;
	#if		CCSW_HWID == HWID_JPCV
					LopV.OpMode = (!pDrvSel) ? LCDOM_Fun : LCDOM_SelectUnit;
	#else
					LopV.OpMode = (!pDrvSel) ? LCDOM_Fun : LCDOM_SelectAxis;
	#endif
				}
				else if( RpxFunEndChkLcd( LopDsp.CurFunIndx, &LopMsg ) )
				{
					RpxFunFinishLcd( LopDsp.CurFunIndx, &LopMsg );  /* finish the executiing Fn function */
					LopV.FunMode = LCDFM_Num;
				}
#endif
			}
			break;

/*--------------------------------------------------------------------------------------------------*/
		case LCDOM_SelectUnit : /* Unit selection mode */
/*--------------------------------------------------------------------------------------------------*/
#if		CCSW_HWID == HWID_JPCV
			if ( OpKeyHld.read )
			{
				//if (RpxFunSearchIdx( LCDPASSWD_FNNUM, 0, &FnIdx, 0, &LopMsg ) == OK)
				if (CLCDFUN->Search( CLCDFUN, LCDPASSWD_FNNUM, 0, &FnIdx, 0, &LopMsg ) == OK)	/* <S18F> */
				{ /* found index of Fn010 */
					if (RpxFunExeBeginLcd( FnIdx, &LopMsg ) == TRUE )
					{ /* execution of Fn010 */
						LopV.OpMode = LCDOM_Passwd;
					}
					LopV.PasswdIdx = FnIdx;
				}
			}
			else if( OpKeyMk.mode )
			{
				if ( RpxChkSelectedDrv(pDrvSel) )
				{
					if ( pDrvSel->Unit[pDrvSel->DispCurIdx].Type == DRVSEL_TYPE_DRVAXIS )
					{
						if( pDrvSel->Unit[pDrvSel->DispCurIdx].AxisNumber > 1 ) 
						{ /* multi axis drive is selected */
							LopV.OpMode = LCDOM_SelectAxis;
							KpiRstLongTimer( &LopV.StDispTime ); /* reset timer */
						}
						else 
						{ /* single axis drive is selected */
							LopV.DrvOpStat = DRVOPST_WAITKEYSEND; /* waiting for mode key break */
							DrvSelEnd = TRUE;                     /* drive selection end */
						}
					}
					else if ( pDrvSel->Unit[pDrvSel->DispCurIdx].Type == DRVSEL_TYPE_CONVERTER)
					{ /* converter is selected */
						LopV.OpMode = LCDOM_PrmMon;
					}
				}
				else
				{
					OpBlnkKickFlg.no_op = 1;
				}
			}
#endif
			break;

/*--------------------------------------------------------------------------------------------------*/
		case LCDOM_SelectAxis : /* axis selection mode */
/*--------------------------------------------------------------------------------------------------*/
#if		CCSW_HWID == HWID_JPCV
			if( OpKeyMk.read )
			{
				/* finish drive lcd operator */
				KriAddLbusMsgQueue ( LMQUEID_MSGTRANSFER, 
						(0xFF << 16) | pDrvSel->Unit[ pDrvSel->DispCurIdx ].Address,
						sizeof(dummyMsg),
						dummyMsg );
				LopV.OpMode = LCDOM_SelectUnit;
			}
			else if( OpKeyMk.mode )
			{
				if ( RpxChkSelectedDrv(pDrvSel) )
				{
					LopV.DrvOpStat = DRVOPST_WAITKEYSEND; /* waiting for mode key break */
					DrvSelEnd = TRUE;                     /* drive selection end */
				}
				else
				{
					OpBlnkKickFlg.no_op = 1;
				}
			}
	#if 1
			else 
			{
				if ( KriGetLbusMsg ( LMQUEID_MSGTRANSFER, &pBuf, &TxSize ) )
				{ 
					KpiRstLongTimer( &LopV.StDispTime ); /* reset timer */
				}
				else if ( KpiGetLongTimer ( &LopV.StDispTime ) > 5000 )
				{
					LopV.OpMode = LCDOM_SelectUnit; /* timeout */
				}
			}
	#endif
			break;
#else
			if( OpKeyMk.mode )
			{
				LopV.OpMode = LCDOM_PrmMon;
				DrvSelEnd = TRUE;
			}
			break;
#endif
/*--------------------------------------------------------------------------------------------------*/
		case LCDOM_Passwd : /* password input mode */
/*--------------------------------------------------------------------------------------------------*/
#if		CCSW_HWID == HWID_JPCV
			FnIdx = LopV.PasswdIdx;
			if( RpxFunEndChkLcd( FnIdx, &LopMsg ) )
			{ 
//TODO				RpxFunFinishLcd( FnIdx, &LopMsg ); /* finish Fn010 */
				LopV.FunMode = LCDFM_Num;
				LopV.OpMode = LCDOM_SelectUnit;
			}
#endif
			break;
/*--------------------------------------------------------------------------------------------------*/
		case LCDOM_PrmCopy: 				/* Parameter Copy Mode */
/*--------------------------------------------------------------------------------------------------*/
			if ( ( OpKeyMk.mode ) && ( ! RpxChkPrmCopyExe() ) )
			{
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W（2軸対応）のみ	*/
				if(DrvSel.Unit[0].DispCurAxis != 0)
				{	/*	2軸動作の場合軸選択へループ（01/08）	*/
					LopV.OpMode = LCDOM_SelectAxis;
				}else
				{	/*	1軸動作の場合パラメータ／モニタへ	*/
					LopV.OpMode = LCDOM_PrmMon;
				}
#elif CSW_BOARD_TYPE == SGD7S_TYPE	/*	SGD7S（単軸時）のみ	*/
				LopV.OpMode = LCDOM_PrmMon;
#endif
			}

/*--------------------------------------------------------------------------------------------------*/
		default: /* error state */
/*--------------------------------------------------------------------------------------------------*/
			break;
	}
	return DrvSelEnd;
}

#if CCSW_CMDID == CMDID_EXIST		/*<11J42>追加ここから*/
/****************************************************************************************************/
/*		Lcd Operator for Command Option : Returns Next Option Parameter Number						*/
/****************************************************************************************************/
USHORT	LcdOptGetNextOpPrmNo(USHORT cmd, USHORT cur_svidx, USHORT digit, DEGOP_PRM *optprm)
{
USHORT	optprmno = 0x0000;
USHORT	svprmno = PnPrmTbl[cur_svidx].PnNo;

		switch(cmd)
		{
			case DEGOP_READ_UP:
				/* 一度オプションパラメータ下限値を下回っている場合 */
				if( optprm->State.ExceedLwrLmt == TRUE )
				{
					optprmno = svprmno + (0x01 << (digit*4));
				}
				/* 一度もオプションパラメータ下限値を下回っていない場合 */
				else
				{
					optprmno = optprm->CurrPnNo + (0x01 << (digit*4));
				}

				/* サーボパラメータ上限値チェック */
				if( optprmno >= 0x1000 ) {optprmno = 0xFFF;}
				break;

			case DEGOP_READ_DOWN:
				/* 一度オプションパラメータ上限値を上回っている場合 */
				if( optprm->State.ExceedUprLmt == TRUE )
				{
					optprmno = svprmno - (0x01 << (digit*4));
				}
				/* 一度もオプションパラメータ上限値を上回っていない場合 */
				else
				{
					/* サーボパラメータ下限値チェック */
					if( optprm->CurrPnNo >= (0x01 << (digit*4)))
					{
						optprmno = optprm->CurrPnNo - (0x01 << (digit*4));
					}
				}
				break;

			default:
				break;
		}

		return(optprmno);
}



/****************************************************************************************************/
/*		Lcd Operator for Command Option : Returns Next Servo Parameter Index						*/
/****************************************************************************************************/
USHORT	LcdOptGetNextSvPrmIdx(USHORT cmd, USHORT cur_svidx, DEGOP_OPTCOM *lcdopt)
{
USHORT	svprmidx;
DEGOP_PRM	*optprm = &lcdopt->Prm[LopV.ActiveLine];

		switch(cmd)
		{
			case DEGOP_READ_UP:
				/* 領域を超えた初回 */
				if( optprm->State.ExceedUprLmt == FALSE )
				{
					/* 次回オプションパラメータ番号が0x0B00以下になることはないためOK */
					svprmidx = RpxPrmSearchIdxOp(optprm->NextPnNo, cur_svidx, &LopMsg);
				}
				/* 領域を超えて２回目以降（システムパラメータ領域の編集） */
				else
				{
					svprmidx = optprm->NextSvIdx; /* サーボパラメータインデックス更新 */
				}
				break;

			case DEGOP_READ_DOWN:
				/* 領域を抜けた初回 */
				if( optprm->State.ExceedLwrLmt == FALSE )
				{
					/* 次回オプションパラメータ番号が0x0B00よりも小さい時はその番号を検索 */
					if( optprm->NextPnNo < OPT_PNNO_VIRTUAL )
					{
						svprmidx = RpxPrmSearchIdxOp(optprm->NextPnNo, cur_svidx, &LopMsg);
					}
					/* 次回オプションパラメータ番号が0x0B00以上の時は0x0AFFを検索 */
					else
					{
						svprmidx = RpxPrmSearchIdxOp((OPT_PNNO_VIRTUAL-1), cur_svidx, &LopMsg);
					}
				}
				/* 領域を抜けて２回目以降（ユーザパラメータ領域の編集） */
				else
				{
					svprmidx = optprm->NextSvIdx; /* サーボパラメータインデックス更新 */
				}
				break;

			default:
				break;
		}

		return(svprmidx);

}



/****************************************************************************************************/
/*		Lcd Operator for Command Option : Request Option Parameter Read								*/
/****************************************************************************************************/
LONG	LcdOptSetOpPrmReadReq(USHORT cmd, USHORT srchno, DEGOP_OPTCOM *lcdopt)
{
LONG	rc;

		/* 次回パラメータ番号が下限値以下の場合 */
		if( srchno <= lcdopt->PrmRange.LwrLmt )
		{
			rc = OpiReqDegOpPrmRead( cmd, lcdopt->PrmRange.LwrLmt);	/* 下限値でリミットしてオプションパラメータ読み出し */
		}
		/* 次回パラメータ番号が上限値以上の場合 */
		else if( srchno >= lcdopt->PrmRange.UprLmt )
		{
			rc = OpiReqDegOpPrmRead( cmd, lcdopt->PrmRange.UprLmt);	/* 上限値でリミットしてオプションパラメータ読み出し */
		}
		/* それ以外 */
		else
		{
			rc = OpiReqDegOpPrmRead( cmd, srchno);					/* 次のオプションパラメータ読み出しコマンド発行 */
		}

		if( rc == DEGOP_SUCCESS )
		{
			/* 応答データ待ちセット */
			lcdopt->Prm[LopV.ActiveLine].State.RcvWait = TRUE;
			/* タイムアウト用タイマリセット */
			KpiRstLongTimer( &lcdopt->ComTimeLong );
		}

		return(rc);
}



/****************************************************************************************************/
/*		Lcd Operator for Command Option : Update Option Parameter Data from Option					*/
/****************************************************************************************************/
LONG	LcdOptUpdateResponseData(USHORT dispID, DEGOP_OPTCOM *lcdopt)
{
	USHORT prmno, idx_tmp;
	DEGOP_ATTR attr;
	LONG data, rc;
	DEGOP_PRM *optprm = &lcdopt->Prm[LopV.ActiveLine];

/*--------------------------------------------------------------------------------------------------*/
/*		オプションカード応答解析																	*/
/*--------------------------------------------------------------------------------------------------*/

		/* パラメータ読み出し/書き込み要求に対する応答取得 */
		rc = OpiGetDegOpPrmValue( &prmno, &attr, &data );

		/* 応答結果解析 */
		if( rc == DEGOP_SUCCESS )	/* 正常終了 */
		{
			/* 現在のオプションパラメータ更新 */
			optprm->CurrPnNo = prmno & 0xFFF;
			optprm->Attr = attr;
			optprm->Data = data;
			optprm->Size = optprm->Attr.ByteSize;

			/* パラメータ編集中セット */
			optprm->State.Editing = TRUE;
			/* 応答データ待ちクリア */
			optprm->State.RcvWait = FALSE;

		}
		else if( rc == DEGOP_BUSY )	/* 処理継続中 */
		{
			if ( KpiGetLongTimer( &lcdopt->ComTimeLong ) > OPT_COM_TIMEOUT )	/* 200msec経過	*/
			{
				/* 応答データ待ちクリア */
				optprm->State.RcvWait = FALSE;
			}
		}
		else if( (rc == DEGOP_REQ_ERR) || (rc == DEGOP_RCV_ERR) )	/* 異常終了 */
		{
			/* 応答データ待ちクリア */
			optprm->State.RcvWait = FALSE;
		}
		else	/* あり得ないパス */
		{
			/* 応答データ待ちクリア */
			optprm->State.RcvWait = FALSE;
		}

/*--------------------------------------------------------------------------------------------------*/
/*		オプションカード応答完了後の処理															*/
/*--------------------------------------------------------------------------------------------------*/
		if( optprm->State.RcvWait == FALSE ) 
		{
	/*----------------------------------------------------------------------------------------------*/
	/*		サーボパラメータインデックス更新（オプション領域突入初回時）							*/
	/*----------------------------------------------------------------------------------------------*/
			if( optprm->State.Editing == TRUE )
			{
			/* オプションパラメータ領域突入初回において正常応答時には */
			/* 仮想オプションパラメータインデックスに現在インデックスを更新 */
				if( optprm->State.ExceedUprLmt == TRUE )
				{
					LopDsp.InfTbl[dispID][LopV.ActiveLine].PnIdx = optprm->NextSvIdx;
					optprm->State.ExceedUprLmt = FALSE;
				}
				else if( optprm->State.ExceedLwrLmt == TRUE )
				{
					LopDsp.InfTbl[dispID][LopV.ActiveLine].PnIdx = optprm->NextSvIdx;
					optprm->State.ExceedLwrLmt = FALSE;
				}
			}
			else
			{
			/* オプションパラメータ領域突入初回においてタイムアウト/エラー時には */
			/* 次に表示可能なサーボインデックスに現在インデックスを更新 */
				if( optprm->State.ExceedUprLmt == TRUE )
				{
					/* 仮想オプションパラメータ（PnB00）を表示させないための処置 */
					LopDsp.InfTbl[dispID][LopV.ActiveLine].PnIdx = 
						RpxPrmSearchIdxOp((OPT_PNNO_VIRTUAL-1), optprm->NextSvIdx, &LopMsg);
					optprm->State.ExceedUprLmt = FALSE;
				}
				else if( optprm->State.ExceedLwrLmt == TRUE )
				{
					/* 仮想オプションパラメータ（PnB00）を表示させないための処置 */
					idx_tmp = RpxPrmSearchIdxOp((OPT_PNNO_VIRTUAL+1), optprm->NextSvIdx, &LopMsg);
					if( PnPrmTbl[idx_tmp].PnNo > OPT_PNNO_VIRTUAL )
					{
						LopDsp.InfTbl[dispID][LopV.ActiveLine].PnIdx = idx_tmp;
						optprm->State.ExceedLwrLmt = FALSE;
					}
					else
					{
						LopDsp.InfTbl[dispID][LopV.ActiveLine].PnIdx = 
							RpxPrmSearchIdxOp((OPT_PNNO_VIRTUAL-1), idx_tmp, &LopMsg);
					}
				}
			}
	/*----------------------------------------------------------------------------------------------*/
	/*		ブリンク起動																			*/
	/*----------------------------------------------------------------------------------------------*/
			if( lcdopt->f.PrmEepWrite == TRUE )
			{
				if( rc == DEGOP_SUCCESS )
				{
					LcdOptBlnkKick->PrmEewrite = 1;	/* 右辺ブリンク起動					*/
				}
				else
				{
					LcdOptBlnkKick->no_op = 1;		/* NO-OPブリンク起動				*/
				}
				lcdopt->f.PrmEepWrite = FALSE;		/* EEPROM書き込み要求フラグリセット */
				LopV.EewriteMode = OPWM_UpdtBlnk;	/* 状態遷移 						*/
			}
			else if( lcdopt->f.PrmRamWrite == TRUE )
			{
				if( rc != DEGOP_SUCCESS )
				{
					LcdOptBlnkKick->no_op = 1;		/* NO-OPブリンク起動				*/
				}
				lcdopt->f.PrmRamWrite = FALSE;		/* RAM書き込み要求フラグリセット	*/
			}
		}

		return(optprm->State.RcvWait);

}



/****************************************************************************************************/
/*		Lcd Operator for Command Option : Parameter Blink x position								*/
/****************************************************************************************************/
USHORT	LcdOptSetDataBlinkDPos(DEGOP_PRM *optprm)
{

	USHORT	pos_tmp = LopV.DataDigit - 1 - LopV.CurDigit;	/* "H."および小数点を未考慮の位置	*/

	if(optprm->Attr.Base == DSPDEF_BASEDEC)	/* 底が10進	*/
	{
		if(optprm->Attr.Sign == DSPDEF_W_SIGN)
		{
			pos_tmp++;	/* 符号分	*/
		}
		if( optprm->Attr.Decimals > 0)
		{
			if(LopV.CurDigit < (USHORT)optprm->Attr.Decimals) pos_tmp++;	/* 小数点分	*/
		}
	}
	else										/* 底が10進以外	*/
	{
		pos_tmp += 2;	/* "n." or "H." の分	*/
	}
	return(pos_tmp);
}
#endif
/*<11J42>追加ここまで*/


/****************************************************************************************************/
/*																									*/
/*	<S031> Drive 用 																				*/
/*																									*/
/****************************************************************************************************/
LONG LpxStartDispModeSv( DRVSEL *pDrvSel )
{
LONG CurMode = LCDOM_StartDisp;

	if( RpxChkPrmCopyInfoGet( ) )
	{
		CurMode = (!pDrvSel) ? LCDOM_PrmMon : LCDOM_SelectAxis;
	}

	return (CurMode);
}


/****************************************************************************************************/
/*																									*/
/*	Converter 用 																					*/
/*																									*/
/****************************************************************************************************/
#if		CCSW_HWID == HWID_JPCV
LONG LpxStartDispModeCv( DRVSEL *pDrvSel, LONG *DrvSelEnd )
{
LONG rc;
LONG CurMode = LCDOM_StartDisp;
	if (pDrvSel->UpdateStep > DRVSEL_UPD_MKUNITLIST)
	{
		if (( rc = RpxGetForceConnectDrive(pDrvSel) ) >= 0)
		{
			pDrvSel->DispCurIdx = rc;          					/* set unit index */
			pDrvSel->Unit[rc].DispCurAxis = 0; 					/* set first axis */
			LopV.DrvOpStat = DRVOPST_FORCECON; 					/* force connection */
			*DrvSelEnd = TRUE;                  				/* drive selection end */
			CurMode = LCDOM_Error;             					/* do not return to unit selection mode */
		}
		else
		{
			CurMode = LCDOM_SelectUnit;
		}
	}
	else if ( KpiGetLongTimer(&LopV.StDispTime) > 10000 )
	{
		CUrMode = LCDOM_SelectUnit;
	}
	
	return (CurMode);
}
#endif

/***************************************** end of file **********************************************/
