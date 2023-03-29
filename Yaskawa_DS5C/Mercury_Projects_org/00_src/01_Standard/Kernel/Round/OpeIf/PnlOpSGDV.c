/****************************************************************************************************/
/*																									*/
/*																									*/
/*		PnlOp.c : パネルオペレータ処理モジュール													*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*	機 能 : パネルオペレータからの各種操作を実行する。												*/
/*																									*/
/*			1)モニタ機能 : 指定モニタ変数(UnXXX)の表示												*/
/*			2)パラメータ : 指定パラメータ(PnXXX)の表示,編集,変更									*/
/*			3)補助機能	 : 指定補助機能(FnXXX)の実行と実行結果の表示								*/
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
/*		Rev.1.00 : 2002.08.20  M.Suga																*/
/*		Rev.1.10 : 2008.04.24  T.Taniguchi															*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "Round.h"





/****************************************************************************************************/
/*																									*/
/*		Panel Operator Macro Definition																*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/* Panel Operator LED Codes																			*/
/*																									*/
/* (digit location)        (7seg location)															*/
/*    bit:0																							*/
/*       --                 _   _   _   _   _ 														*/
/*    5 |  | 1             |_| |_| |_| |_| |_|														*/
/*       -- 6              |_| |_| |_| |_| |_|														*/
/*    4 |  | 2      led[?]: 4   3   2   1   0 														*/
/*       -- * 7																						*/
/*       3  logic			0:ON / 1:OFF															*/
/*--------------------------------------------------------------------------------------------------*/
#define	LEDCODE_B0		0x01
#define	LEDCODE_B1		0x02
#define	LEDCODE_B2		0x04
#define	LEDCODE_B3		0x08
#define	LEDCODE_B4		0x10
#define	LEDCODE_B5		0x20
#define	LEDCODE_B6		0x40
#define	LEDCODE_B7		0x80
/*--------------------------------------------------------------------------------------------------*/
#define	LEDCODE_ZERO	0x3F
#define	LEDCODE_1		0x06
#define	LEDCODE_2		0x5B
#define	LEDCODE_3		0x4F
#define	LEDCODE_4		0x66
#define	LEDCODE_5		0x6D
#define	LEDCODE_6		0x7D
#define	LEDCODE_7		0x27
#define	LEDCODE_8		0x7F
#define	LEDCODE_9		0x6F
#define	LEDCODE_A		0x77
#define	LEDCODE_B		0x7C
#define	LEDCODE_C		0x39
#define	LEDCODE_D		0x5E
#define	LEDCODE_E		0x79
#define	LEDCODE_F		0x71
#define	LEDCODE_G		0x3D
#define	LEDCODE_H		0x76
#define	LEDCODE_sH		0x74
#define	LEDCODE_J		0x0E
#define	LEDCODE_L		0x38
#define	LEDCODE_N		0x54
#define	LEDCODE_sO		0x5C
#define	LEDCODE_P		0x73
#define	LEDCODE_Q		0x67
#define	LEDCODE_R		0x50
#define	LEDCODE_T		0x78
#define	LEDCODE_U		0x3E
#define	LEDCODE_sU		0x1C
#define	LEDCODE_Y		0x6E
/*--------------------------------------------------------------------------------------------------*/
#define	LEDCODE_MINUS	0x40
#define	LEDCODE_POINT	0x80
#define	LEDCODE_BLANK	0x00
/*--------------------------------------------------------------------------------------------------*/
/* Panel Operator Buttons (Already Inverted)														*/
/*--------------------------------------------------------------------------------------------------*/
#define PNLKEY_MODE		0x1
#define PNLKEY_UP		0x2
#define PNLKEY_DOWN		0x4
#define PNLKEY_SHUP		0x8
#define PNLKEY_RST		(PNLKEY_UP | PNLKEY_DOWN)
#define PNLKEY_SWOFF	0x0
/*--------------------------------------------------------------------------------------------------*/
/* Panel Operator Display Modes																		*/
/*--------------------------------------------------------------------------------------------------*/
#define PNLMODE_STATUS		0
#define PNLMODE_FNNUM		1
#define PNLMODE_FNEXE		2
#define PNLMODE_PNNUM		3
#define PNLMODE_PNDATA		4
#define PNLMODE_UNNUM		5
#define PNLMODE_UNDATA		6
/*--------------------------------------------------------------------------------------------------*/
#define	PNLLED_BLINK		0xFF		/* パネルLED消灯マスク(点滅用)								*/
#define	PNLLED_NOBLINK		0x00		/* パネルLED点灯マスク(点滅用)								*/
/*--------------------------------------------------------------------------------------------------*/
#define	PNLMON_NOSET		0xFFF		/* 電源投入時モニタデフォルト								*/
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		Panel Operator Constant Definition															*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		LED Code Table																				*/
/*--------------------------------------------------------------------------------------------------*/
const	UCHAR	ledcodetbl[] =
{
	LEDCODE_ZERO, LEDCODE_1, LEDCODE_2, LEDCODE_3, LEDCODE_4, LEDCODE_5, LEDCODE_6, LEDCODE_7,
	LEDCODE_8,	  LEDCODE_9, LEDCODE_A, LEDCODE_B, LEDCODE_C, LEDCODE_D, LEDCODE_E, LEDCODE_F
};
/*--------------------------------------------------------------------------------------------------*/
/*		LED Convert Table																			*/
/*--------------------------------------------------------------------------------------------------*/
const	UCHAR	ledconvtbl[] =
{
/* '-', */		LEDCODE_MINUS,
/* '.', */		LEDCODE_POINT,
/* '/', */		LEDCODE_BLANK,
/* '0', */		LEDCODE_ZERO ,
/* '1', */		LEDCODE_1    ,
/* '2', */		LEDCODE_2    ,
/* '3', */		LEDCODE_3    ,
/* '4', */		LEDCODE_4    ,
/* '5', */		LEDCODE_5    ,
/* '6', */		LEDCODE_6    ,
/* '7', */		LEDCODE_7    ,
/* '8', */		LEDCODE_8    ,
/* '9', */		LEDCODE_9    ,
/* ':', */		LEDCODE_BLANK,
/* ';', */		LEDCODE_BLANK,
/* '<', */		LEDCODE_BLANK,
/* '=', */		LEDCODE_BLANK,
/* '>', */		LEDCODE_BLANK,
/* '?', */		LEDCODE_BLANK,
/* '@', */		LEDCODE_BLANK,
/* 'A', */		LEDCODE_A    ,
/* 'B', */		LEDCODE_B    ,
/* 'C', */		LEDCODE_C    ,
/* 'D', */		LEDCODE_D    ,
/* 'E', */		LEDCODE_E    ,
/* 'F', */		LEDCODE_F    ,
/* 'G', */		LEDCODE_G    ,
/* 'H', */		LEDCODE_H    ,
/* 'I', */		LEDCODE_1	 ,
/* 'J', */		LEDCODE_J    ,
/* 'K', */		LEDCODE_BLANK,
/* 'L', */		LEDCODE_L    ,
/* 'M', */		LEDCODE_BLANK,
/* 'N', */		LEDCODE_N    ,
/* 'O', */		LEDCODE_ZERO ,
/* 'P', */		LEDCODE_P    ,
/* 'Q', */		LEDCODE_Q    ,
/* 'R', */		LEDCODE_R    ,
/* 'S', */		LEDCODE_5    ,
/* 'T', */		LEDCODE_T    ,
/* 'U', */		LEDCODE_U    ,
/* 'V', */		LEDCODE_BLANK,
/* 'W', */		LEDCODE_BLANK,
/* 'X', */		LEDCODE_BLANK,
/* 'Y', */		LEDCODE_Y    ,
/* 'Z', */		LEDCODE_BLANK
};
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		Panel Operator Variable Definition															*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Panel Operator Structure Definition															*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{
/*--------------------------------------------------------------------------------------------------*/
	USHORT	LastAlarm;		/* アラーム前回値(アラーム発生時に状態表示モードに飛ぶために使用する)	*/
	USHORT	BtnLast;		/* パネルボタン前回値													*/
	ULONG	KeyTime;		/* パネルボタン長押し検知用												*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	DispMode;		/* 表示モード															*/
	LEDDSP	DspData;		/* 表示データ															*/
	ULONG	BlnkTime;		/* パネル表示点滅周期制御用												*/
	USHORT	CurPos;			/* パネル パラメータ現在桁/モニター現在表示部							*/
	USHORT	CurPosMax;		/* パネル パラメータ現在桁/モニター現在表示部 範囲						*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	CurFunIndx;		/* パネル補助機能現在位置インデックス									*/
	USHORT	CurPrmIndx;		/* パネルパラメータ現在位置インデックス									*/
	USHORT	PrmWrState;		/* パネルパラメータ書き込み時状態										*/
	USHORT	CurMonIndx;		/* パネルモニター現在位置インデックス									*/
/*--------------------------------------------------------------------------------------------------*/
} PNL_STRUC;
/*--------------------------------------------------------------------------------------------------*/
/*		Local Variable Definition																	*/
/*--------------------------------------------------------------------------------------------------*/
#pragma section sidata begin
PNL_STRUC	Pnl;			/* Panel Operator Variable												*/
extern MSGIF PopMsg;		/* Panel Operator Message I/F Data										*/
#pragma section sidata end
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		Panel Operator Function List																*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		パネルオペレータＩＦ関数																	*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxPnlOpMain( LONG ComMode );		/* Panel Operator Main Program							*/
void	RpxPnlOpInit( void );				/* Panel Operator Initialize							*/
void	RpxPnlOpMonInit( void );			/* Panel Monitor Initialize								*/
void	RpxPnlOpFinish( void );				/* Panel Operator Finish								*/
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
		LEDDSP	*leddspp		);			/* LED表示データのアドレス								*/
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
/*		Local Function Prototype Definition															*/
/*--------------------------------------------------------------------------------------------------*/
void	PnlKeyTranslate( void );			/* パネルオペレータ キー変換処理						*/
void	PnlStatusDispMode( void );			/* パネルオペレータ 状態表示処理						*/
void	PnlFunNoMode( void );				/* パネルオペレータ Fn番号編集モード					*/
void	PnlFunExeMode( void );				/* パネルオペレータ Fn実行モード						*/
void	PnlPrmNoMode( void );				/* パネルオペレータ Pn番号編集モード					*/
void	PnlPrmDataMode( void );				/* パネルオペレータ Pnデータ表示/編集モード				*/
void	PnlMonNoMode( void );				/* パネルオペレータ Un番号編集モード					*/
void	PnlMonDataMode( void );				/* パネルオペレータ Unデータ表示モード					*/
void	PnlLedBlink( void );				/* パネルオペレータ LEDブリンク処理						*/
/*--------------------------------------------------------------------------------------------------*/
void	PnlLedDispPrmDecWordUnsigned(		/* パネルオペレータ LED表示(パラメータ符号無10進ワード)	*/
		USHORT	data,						/* 表示する値											*/
		USHORT	cur_digit,					/* 現在カーソル位置										*/
		USHORT	decimals		);			/* 小数点以下桁数										*/
/*--------------------------------------------------------------------------------------------------*/
void	PnlLedDispPrmDecWordSigned(			/* パネルオペレータ LED表示(パラメータ符号付10進ワード)	*/
		SHORT	data,						/* 表示する値											*/
		USHORT	cur_digit,					/* 現在カーソル位置										*/
		USHORT	decimals, 					/* 小数点以下桁数										*/
		SHORT	llimit,						/* 下限値												*/
		SHORT	ulimit			);			/* 上限値												*/
/*--------------------------------------------------------------------------------------------------*/
void	PnlLedDispPrmHexNblWord(			/* パネルオペレータ LED表示(パラメータ16進ワード)		*/
		USHORT	data,						/* 表示する値											*/
		USHORT	cur_digit,					/* 現在カーソル位置										*/
		USHORT	base			);			/* 表示の底												*/
/*--------------------------------------------------------------------------------------------------*/
void	PnlLedDispPrmDecLong(				/* パネルオペレータ LED表示(10進ロング)					*/
		ULONG	ldata,						/* 表示する値											*/
		USHORT	cur_digit,					/* 現在カーソル位置										*/
		USHORT	decimals,					/* 小数点以下桁数										*/
		USHORT	sign			);			/* 符号有無												*/
/*--------------------------------------------------------------------------------------------------*/
void	PnlLedDispPrmHexLong(				/* パネルオペレータ LED表示(16進ロング)					*/
		ULONG	ldata,						/* 表示する値											*/
		USHORT	cur_digit		);			/* 現在カーソル位置										*/
/*--------------------------------------------------------------------------------------------------*/
void	PnlLedDispMonHexWord(				/* パネルオペレータ LED表示(モニター16進ワード)			*/
		USHORT	data			);			/* 表示する値											*/
/*--------------------------------------------------------------------------------------------------*/
void	PnlLedDispMonBit(					/* パネルオペレータ LED表示(モニタービットバイト)		*/
		UCHAR	data			);			/* 表示する値											*/
/*--------------------------------------------------------------------------------------------------*/
void	PnlLedSetKnlBuffer(					/* Panel LED Set to Kernel Variables					*/
		LEDDSP	*leddsp,					/*														*/
		UCHAR	*dist,						/*														*/
		LONG	size			);			/*														*/
/*--------------------------------------------------------------------------------------------------*/
void	PnlLedSetBlnkSw(					/* Panel LED Set to Same Data							*/
		LEDDSP	*leddsp,					/*														*/
		UCHAR	data,						/*														*/
		LONG	size,						/*														*/
		LONG	flag			);			/*														*/
/*--------------------------------------------------------------------------------------------------*/





/****************************************************************************************************/
/*																									*/
/*		Panel Operator Main Program																	*/
/*																									*/
/****************************************************************************************************/
void	RpxPnlOpMain( LONG ComMode )
{
USHORT	CurAlarm;
LONG	rc_tmp;

/*--------------------------------------------------------------------------------------------------*/
/*		シリアル通信非接続時																		*/
/*--------------------------------------------------------------------------------------------------*/
		if( ComMode == SCM_NOCOM )
		{
			CurAlarm = KpiGetAlarmInfo();
		/*------------------------------------------------------------------------------------------*/
		/*	アラーム発生時は状態表示モードへ														*/
		/*------------------------------------------------------------------------------------------*/
			if( (CurAlarm != 0) && ((CurAlarm & 0xF00) != 0x900) && (Pnl.LastAlarm == 0) )
			{
				Pnl.CurPos = 0;
				if(Pnl.DispMode == PNLMODE_FNEXE)	RpxFunFinishPnl(Pnl.CurFunIndx, &PopMsg);
				if(Pnl.DispMode == PNLMODE_PNDATA)
				{
					rc_tmp = RpxPrmUpdateEepromIdx(Pnl.CurPrmIndx);
					while(rc_tmp == PRM_EEPROM_WRITE_WAIT || rc_tmp == PRM_EEPROM_WRITE_FAIL)
					{
						rc_tmp = RpxPrmUpdateEepromIdx(Pnl.CurPrmIndx);
					}
				}
				Pnl.DispMode = PNLMODE_STATUS;
			}
			Pnl.LastAlarm = CurAlarm;
		/*------------------------------------------------------------------------------------------*/
		/*																							*/
		/*------------------------------------------------------------------------------------------*/
			PnlKeyTranslate();
			switch( Pnl.DispMode )
			{
		/*------------------------------------------------------------------------------------------*/
			case PNLMODE_STATUS:	/* 状態表示	*/
		/*------------------------------------------------------------------------------------------*/
				if(OpKeyMk.mode)
				{
					Pnl.DispMode = PNLMODE_FNNUM;
					Pnl.CurPos = 0;
				}
				else
				{
					PnlStatusDispMode();
					if(OpKeyMk.rst)
					{
						KriResetSystemAlarm();
						NdiClearAlarm();
					}
				}
					break;
		/*------------------------------------------------------------------------------------------*/
			case PNLMODE_FNNUM:		/* Fn番号	*/
		/*------------------------------------------------------------------------------------------*/
				if(OpKeyMk.mode)
				{
					Pnl.DispMode = PNLMODE_PNNUM;
					Pnl.CurPos = 0;
				}
				else if(OpKeyMk.data)
				{
					if(RpxFunExeBeginPnl(Pnl.CurFunIndx, &PopMsg) != TRUE)	/* Fn実行できない時	*/
					{
						OpBlnkKickFlg.no_op = 1;
					}
					else
					{
						Pnl.DispMode = PNLMODE_FNEXE;
						Pnl.CurPos = 0;
					}
				}
				else	PnlFunNoMode();
				break;
		/*------------------------------------------------------------------------------------------*/
			case PNLMODE_FNEXE:		/* Fn実行	*/
		/*------------------------------------------------------------------------------------------*/
				if(RpxFunEndChkPnl(Pnl.CurFunIndx, &PopMsg))
				{
					RpxFunFinishPnl(Pnl.CurFunIndx, &PopMsg);
					Pnl.CurPos = 0;
					Pnl.DispMode = PNLMODE_FNNUM;
				}
				else	PnlFunExeMode();
				break;
		/*------------------------------------------------------------------------------------------*/
			case PNLMODE_PNNUM:		/* Pn番号	*/
		/*------------------------------------------------------------------------------------------*/
				if(OpKeyMk.mode)
				{
					Pnl.DispMode = PNLMODE_UNNUM;
					Pnl.CurPos = 0;
				}
				else if(OpKeyMk.data)
				{
					if( Iprm.f.PrmWrtInhibit && (PopMsg.AccessLvl != ACCLVL_SYSTEM) )
					{
						OpBlnkKickFlg.no_op = 1;
					}
					else
					{
						Pnl.DispMode = PNLMODE_PNDATA;
						Pnl.PrmWrState = 0;
						Pnl.CurPos = 0;
					}
				}
				else	PnlPrmNoMode();
				break;
		/*------------------------------------------------------------------------------------------*/
			case PNLMODE_PNDATA:	/* Pnデータ	*/
		/*------------------------------------------------------------------------------------------*/
				PnlPrmDataMode();
				break;
		/*------------------------------------------------------------------------------------------*/
			case PNLMODE_UNNUM:		/* Un番号	*/
		/*------------------------------------------------------------------------------------------*/
				if(OpKeyMk.mode)
				{
					Pnl.DispMode = PNLMODE_STATUS;
				}
				else if(OpKeyMk.data)
				{
					Pnl.DispMode = PNLMODE_UNDATA;
					Pnl.CurPos = 0;
				}
				else	PnlMonNoMode();
				break;
		/*------------------------------------------------------------------------------------------*/
			case PNLMODE_UNDATA:	/* Unデータ	*/
		/*------------------------------------------------------------------------------------------*/
				PnlMonDataMode();
				break;
		/*------------------------------------------------------------------------------------------*/
			default:
		/*------------------------------------------------------------------------------------------*/
				break;
			}
			PnlLedBlink();
		}
/*--------------------------------------------------------------------------------------------------*/
/*		シリアル通信接続時																			*/
/*--------------------------------------------------------------------------------------------------*/
		else
		{
			PnlStatusDispMode();
			PnlLedSetBlnkSw( &Pnl.DspData, PNLLED_NOBLINK,
							 sizeof(RoutK.PnlLed), ((KpiGetLongTimer(&Pnl.BlnkTime)/100) & 0x02) );
		}
/*--------------------------------------------------------------------------------------------------*/
		PnlLedSetKnlBuffer( &Pnl.DspData, RoutK.PnlLed, sizeof(RoutK.PnlLed) );
		return;
}



/****************************************************************************************************/
/*																									*/
/*		パネルオペレータ キー変換処理																*/
/*																									*/
/****************************************************************************************************/
void	PnlKeyTranslate( void )
{
/*--------------------------------------------------------------------------------------------------*/
/*		変数初期化																					*/
/*--------------------------------------------------------------------------------------------------*/
//		*((UINT *)&OpKeyMk)  = 0;
//		*((UINT *)&OpKeyBrk) = 0;
//		*((UINT *)&OpKeyLvl) = 0;
		*((UINT *)&OpKeyMk)  = 	*((UINT *)&OpKeyBrk) = *((UINT *)&OpKeyLvl) = 0;
/*--------------------------------------------------------------------------------------------------*/
/*		ブリンク中はキー入力を無視する																*/
/*--------------------------------------------------------------------------------------------------*/
		if( *((UINT *)&OpBlnkFlg) != 0 )
		{
			Pnl.BtnLast = RinK.PnlBtn;
			return;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		キー入力レベルの処理																		*/
/*--------------------------------------------------------------------------------------------------*/
		if( RinK.PnlBtn & PNLKEY_MODE	){ OpKeyLvl.mode = TRUE;}
		if( RinK.PnlBtn & PNLKEY_UP		){ OpKeyLvl.up   = TRUE;}
		if( RinK.PnlBtn & PNLKEY_DOWN	){ OpKeyLvl.down = TRUE;}
		if( RinK.PnlBtn & PNLKEY_SHUP	){ OpKeyLvl.shup = TRUE;}
		if( (RinK.PnlBtn & PNLKEY_RST) == PNLKEY_RST ){ OpKeyLvl.rst = TRUE;}
/*--------------------------------------------------------------------------------------------------*/
/*		キー入力に変化があった時のみセット(=チャタリング防止)										*/
/*--------------------------------------------------------------------------------------------------*/
		if( RinK.PnlBtn != Pnl.BtnLast )
		{
		/*------------------------------------------------------------------------------------------*/
		/*	キー入力MAKEの処理																		*/
		/*------------------------------------------------------------------------------------------*/
			switch( RinK.PnlBtn )
			{
			case PNLKEY_MODE:	OpKeyMk.mode = TRUE; break;
			case PNLKEY_UP:		OpKeyMk.up   = TRUE; KpiRstLongTimer(&Pnl.KeyTime); break;
			case PNLKEY_DOWN:	OpKeyMk.down = TRUE; KpiRstLongTimer(&Pnl.KeyTime); break;
			case PNLKEY_SHUP:	OpKeyMk.shup = TRUE; KpiRstLongTimer(&Pnl.KeyTime); break;
			case PNLKEY_RST:	OpKeyMk.rst  = TRUE; break;
			default: break;
			}
		/*------------------------------------------------------------------------------------------*/
		/*	キー入力BREAKの処理																		*/
		/*------------------------------------------------------------------------------------------*/
			switch(Pnl.BtnLast)
			{
			case PNLKEY_MODE:	OpKeyBrk.mode = TRUE; break;
			case PNLKEY_UP:		OpKeyBrk.up   = TRUE; break;
			case PNLKEY_DOWN:	OpKeyBrk.down = TRUE; break;
			case PNLKEY_SHUP:	OpKeyBrk.shup = TRUE; break;
			case PNLKEY_RST:	OpKeyBrk.rst  = TRUE; break;
			default: break;
			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		キー入力に変化がない場合: リピート, ホールド処理											*/
/*--------------------------------------------------------------------------------------------------*/
		else
		{
			switch(RinK.PnlBtn)
			{
			case PNLKEY_UP:
				if(KpiGetLongTimer(&Pnl.KeyTime) >= 400)
				{	/* 400ms未満は何もしない	*/
					if(!((KpiGetLongTimer(&Pnl.KeyTime)/100) & 0x01))
					{	/* 経過時間 4xx,6xx,8xx...の時	*/
						OpKeyMk.up = TRUE;
					}
				}
				break;
			case PNLKEY_DOWN:
				if(KpiGetLongTimer(&Pnl.KeyTime) >= 400)
				{	/* 400ms未満は何もしない	*/
					if(!((KpiGetLongTimer(&Pnl.KeyTime)/100) & 0x01))
					{	/* 経過時間 4xx,6xx,8xx...の時	*/
						OpKeyMk.down = TRUE;
					}
				}
				break;
			case PNLKEY_SHUP:
				if(KpiGetLongTimer(&Pnl.KeyTime) >= 1000)
				{	/* 1s 以上で[DATA]キー入力相当	*/
					OpKeyMk.data = TRUE;
					KpiRstLongTimer(&Pnl.KeyTime);
				}
				break;
			default:
				break;
			}
		}
/*--------------------------------------------------------------------------------------------------*/
	Pnl.BtnLast = RinK.PnlBtn;
	return;
}



/****************************************************************************************************/
/*																									*/
/*		パネルオペレータ 状態表示処理																*/
/*																									*/
/****************************************************************************************************/
void	PnlStatusDispMode( void )
{
USHORT	CurAlarm;

/*--------------------------------------------------------------------------------------------------*/
/*		状態表示																					*/
/*--------------------------------------------------------------------------------------------------*/
		Pnl.DspData.LedCode[4] = LEDCODE_B7;
		Pnl.DspData.LedCode[3] = LEDCODE_BLANK;
		RpxLedSetStatusBits( &(Pnl.DspData) );
/*--------------------------------------------------------------------------------------------------*/
/*		アラーム発生中																				*/
/*--------------------------------------------------------------------------------------------------*/
		CurAlarm = KpiGetAlarmInfo();
		if( Alm.f.AlmFlag )
		{
			/* アラームコード表示	*/
			RpxLedSetValHex(CurAlarm, &(Pnl.DspData), 2, 3);
			PnlLedSetBlnkSw( &Pnl.DspData, PNLLED_BLINK, sizeof(RoutK.PnlLed), TRUE );
		}
/*--------------------------------------------------------------------------------------------------*/
/*		ワーニング発生中																			*/
/*--------------------------------------------------------------------------------------------------*/
		else if( CurAlarm != 0 )
		{
			/* ワーニングコード表示	*/
			RpxLedSetValHex(CurAlarm, &(Pnl.DspData), 2, 3);
		}
/*--------------------------------------------------------------------------------------------------*/
/*		通常表示(アラーム＆ワーニング無し)															*/
/*--------------------------------------------------------------------------------------------------*/
		else
		{
			if( RinV.f.BaseEnable )
			{
				if( !MencV.f.PhaseReady )
				{
					Pnl.DspData.LedCode[3] = LEDCODE_P;	/* " P-dt"	*/
					Pnl.DspData.LedCode[2] = LEDCODE_B6;
					Pnl.DspData.LedCode[1] = LEDCODE_D;
					Pnl.DspData.LedCode[0] = LEDCODE_T;
				}
				else
				{
					Pnl.DspData.LedCode[2] = LEDCODE_R;	/* "  run"	*/
					Pnl.DspData.LedCode[1] = LEDCODE_sU;
					Pnl.DspData.LedCode[0] = LEDCODE_N;
				}
			}
			else
			{
				Pnl.DspData.LedCode[2] = LEDCODE_BLANK;	/* "   bb"	*/
				Pnl.DspData.LedCode[1] = LEDCODE_B;
				Pnl.DspData.LedCode[0] = LEDCODE_B;
			}
		/*------------------------------------------------------------------------------------------*/
			if( BoutV.CtrlMode.b.cm != CTRLMODE_JOG && BoutV.CtrlMode.b.cm != CTRLMODE_ZSRCH )
			{
				if(CinV.f.Sv.Pot && CinV.f.Sv.Not)	/* "Pot" AND "not" */
				{
					if(!((KpiGetLongTimer(&Pnl.BlnkTime)/100) & 0x04))	/* 周期800ms	*/
					{
						Pnl.DspData.LedCode[2] = LEDCODE_P;
						Pnl.DspData.LedCode[1] = LEDCODE_sO;
						Pnl.DspData.LedCode[0] = LEDCODE_T;
					}
					else
					{
						Pnl.DspData.LedCode[2] = LEDCODE_N;
						Pnl.DspData.LedCode[1] = LEDCODE_sO;
						Pnl.DspData.LedCode[0] = LEDCODE_T;
					}
				}
				else if(CinV.f.Sv.Pot)				/* "Pot" */
				{
					Pnl.DspData.LedCode[2] = LEDCODE_P;
					Pnl.DspData.LedCode[1] = LEDCODE_sO;
					Pnl.DspData.LedCode[0] = LEDCODE_T;
				}
				else if(CinV.f.Sv.Not)				/* "Not" */
				{
					Pnl.DspData.LedCode[2] = LEDCODE_N;
					Pnl.DspData.LedCode[1] = LEDCODE_sO;
					Pnl.DspData.LedCode[0] = LEDCODE_T;
				}
			}
		/*------------------------------------------------------------------------------------------*/
			if( CoutV.f.Sv.HwbbState )				/* "Hbb" */
			{
				Pnl.DspData.LedCode[3] = LEDCODE_BLANK; /* 注：磁極検出中のOTはALMとなるため不要*/
				Pnl.DspData.LedCode[2] = LEDCODE_H;
				Pnl.DspData.LedCode[1] = LEDCODE_B;
				Pnl.DspData.LedCode[0] = LEDCODE_B;
			}
		/*------------------------------------------------------------------------------------------*/
			PnlLedSetBlnkSw( &Pnl.DspData, PNLLED_NOBLINK, sizeof(RoutK.PnlLed), TRUE );
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		パネルオペレータ Fn番号編集モード															*/
/*																									*/
/****************************************************************************************************/
void	PnlFunNoMode( void )
{
USHORT	i, fnnum_tmp;

/*--------------------------------------------------------------------------------------------------*/
/*		キー入力処理																				*/
/*--------------------------------------------------------------------------------------------------*/
		if( OpKeyMk.shup )
		{
			Pnl.CurPos = ((Pnl.CurPos+1) == 3)? 0: Pnl.CurPos+1;
		}
		else if( OpKeyMk.up )
		{
			fnnum_tmp = FnFunTbl[Pnl.CurFunIndx].FnNo + (0x01 << (Pnl.CurPos*4));
			if(fnnum_tmp >= 0x1000)	fnnum_tmp = 0xFFF;
			Pnl.CurFunIndx = RpxFunSearchIdxOp(fnnum_tmp, Pnl.CurFunIndx, &PopMsg);
		}
		else if( OpKeyMk.down )
		{
			if(FnFunTbl[Pnl.CurFunIndx].FnNo < (0x01 << (Pnl.CurPos*4)))
			{
				fnnum_tmp = 0x000;
			}
			else
			{
				fnnum_tmp = FnFunTbl[Pnl.CurFunIndx].FnNo - (0x01 << (Pnl.CurPos*4));
			}
			Pnl.CurFunIndx = RpxFunSearchIdxOp(fnnum_tmp, Pnl.CurFunIndx, &PopMsg);
		}
/*--------------------------------------------------------------------------------------------------*/
/*		表示処理																					*/
/*--------------------------------------------------------------------------------------------------*/
		Pnl.DspData.LedCode[4] = LEDCODE_F;
		Pnl.DspData.LedCode[3] = LEDCODE_N;
		RpxLedSetValHex(FnFunTbl[Pnl.CurFunIndx].FnNo, &(Pnl.DspData), 2, 3);
/*--------------------------------------------------------------------------------------------------*/
		for( i = 0; i < sizeof(RoutK.PnlLed); i++ )
		{
			Pnl.DspData.LedBlnkSw[i] = (i == Pnl.CurPos)?	0xFF: 0;
		}
/*--------------------------------------------------------------------------------------------------*/
	return;
}



/****************************************************************************************************/
/*																									*/
/*		パネルオペレータ Fn実行モード																*/
/*																									*/
/****************************************************************************************************/
void	PnlFunExeMode( void )
{
		RpxFunExePnl( OpKeyMk, OpKeyBrk, OpKeyLvl, Pnl.CurFunIndx,
							&(Pnl.DspData), &OpBlnkKickFlg, &PopMsg );
}



/****************************************************************************************************/
/*																									*/
/*		パネルオペレータ Pn番号編集モード															*/
/*																									*/
/****************************************************************************************************/
void	PnlPrmNoMode( void )
{
USHORT	i, pnnum_tmp;
PRMDEF	*prmdef_tmp;

/*--------------------------------------------------------------------------------------------------*/
/*		キー入力処理																				*/
/*--------------------------------------------------------------------------------------------------*/
		if( OpKeyMk.shup )
		{
			Pnl.CurPos = ((Pnl.CurPos+1) == 3)? 0: Pnl.CurPos+1;
		}
		else if( OpKeyMk.up )
		{
			pnnum_tmp = PnPrmTbl[Pnl.CurPrmIndx].PnNo + (0x01 << (Pnl.CurPos*4));
			if(pnnum_tmp >= 0x1000)	pnnum_tmp = 0xFFF;
			Pnl.CurPrmIndx = RpxPrmSearchIdxOp(pnnum_tmp, Pnl.CurPrmIndx, &PopMsg);
		}
		else if( OpKeyMk.down )
		{
			if(PnPrmTbl[Pnl.CurPrmIndx].PnNo < (0x01 << (Pnl.CurPos*4)))
			{
				pnnum_tmp = 0x000;
			}
			else
			{
				pnnum_tmp = PnPrmTbl[Pnl.CurPrmIndx].PnNo - (0x01 << (Pnl.CurPos*4));
			}
			Pnl.CurPrmIndx = RpxPrmSearchIdxOp(pnnum_tmp, Pnl.CurPrmIndx, &PopMsg);
		}
/*--------------------------------------------------------------------------------------------------*/
/*		現在桁最大値																				*/
/*--------------------------------------------------------------------------------------------------*/
		prmdef_tmp = (PRMDEF *)PnPrmTbl[Pnl.CurPrmIndx].PrmDefPtr;	/* パラメータ定義	*/
		if( prmdef_tmp->Attr.IfIsLong )	/* ロングパラメータ	*/
		{
			Pnl.CurPosMax = (prmdef_tmp->Attr.Base == DSPDEF_BASEDEC)? 10: 8;
		}
		else							/* ワードパラメータ	*/
		{
			if( prmdef_tmp->Attr.Base == DSPDEF_BASEDEC )
			{
				if( (prmdef_tmp->Attr.Sign == DSPDEF_W_SIGN) &&
					( (-10000 < (SHORT)RpxPrmLowerLimitIdx(Pnl.CurPrmIndx)) && 
					  ((SHORT)RpxPrmUpperLimitIdx(Pnl.CurPrmIndx) < 10000) )
				)
				{
					Pnl.CurPosMax = 4;	/* 符号含め5桁で足りる場合	*/
				}
				else
				{
					Pnl.CurPosMax = 5;
				}
			}
			else
			{
				Pnl.CurPosMax = 4;
			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		ＬＥＤ表示処理																				*/
/*--------------------------------------------------------------------------------------------------*/
		Pnl.DspData.LedCode[4] = LEDCODE_P;
		Pnl.DspData.LedCode[3] = LEDCODE_N;
		RpxLedSetValHex(PnPrmTbl[Pnl.CurPrmIndx].PnNo, &(Pnl.DspData), 2, 3);
/*--------------------------------------------------------------------------------------------------*/
		for( i = 0; i < sizeof(RoutK.PnlLed); i++ )
		{
			Pnl.DspData.LedBlnkSw[i] = (i == Pnl.CurPos)?	0xFF: 0;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		パネルオペレータ Pnデータ表示/編集モード													*/
/*																									*/
/****************************************************************************************************/
void	PnlPrmDataMode( void )
{
ULONG	ldata_tmp;
PRMDEF	*prmdef_tmp;
LONG	rc;

		prmdef_tmp = (PRMDEF *)PnPrmTbl[Pnl.CurPrmIndx].PrmDefPtr;	/* パラメータ定義	*/
/*--------------------------------------------------------------------------------------------------*/
/*		パネルオペレータ Pnデータ表示/編集処理														*/
/*--------------------------------------------------------------------------------------------------*/
		switch( Pnl.PrmWrState )
		{
	  /*--------------------------------------------------------------------------------------------*/
		case OPWM_None:	/* パラメータ値表示・編集処理	*/
	  /*--------------------------------------------------------------------------------------------*/
			RpxPrmReadIdx(Pnl.CurPrmIndx, &ldata_tmp, PCHK_OPE_READ);	/* ldata_tmp <-- Prm値		*/
			if( OpKeyMk.data | OpKeyMk.mode )	/* EEPROM書き込み処理	*/
			{
				switch(RpxPrmUpdateEepromIdx(Pnl.CurPrmIndx))
				{
				case OK:	/* 正常	*/
					OpBlnkKickFlg.PrmEewrite = 1;	/* ブリンク起動	*/
					break;
				case PRM_EEPROM_WRITE_FAIL:	/* まだQueueが空いていない	*/
					Pnl.PrmWrState = OPWM_UpdtWait;
					break;
				case PERR_NO:					/* RAMのみのパラメータ								*/
				case PRM_EEPROM_RAM_NO_DIFF:	/* 書き込む必要がない(EEPROM値が書き込み値と同じ)	*/
					Pnl.CurPos = 0;
					Pnl.DispMode = PNLMODE_PNNUM;	/* パラメータ番号モードへ	*/
					return;
					break;
				default:
					break;
				}
			}
			else if( OpKeyMk.shup )
			{
				Pnl.CurPos = ((Pnl.CurPos+1) == Pnl.CurPosMax)?
					0: Pnl.CurPos+1;
			}
			else if( OpKeyMk.up | OpKeyMk.down )
			{
				ldata_tmp = (OpKeyMk.up)? 
					RpxPrmRamDigitInc(Pnl.CurPrmIndx, ldata_tmp, Pnl.CurPos):
					RpxPrmRamDigitDec(Pnl.CurPrmIndx, ldata_tmp, Pnl.CurPos);
				rc = RpxPrmWriteRamIdx(Pnl.CurPrmIndx, ldata_tmp, PCHK_OPE_WRITE, &PopMsg);
				if( rc == PERR_CALC )
				{
					OpBlnkKickFlg.error = 1;
				}
			}
		/*------------------------------------------------------------------------------------------*/
		/*	表示処理																				*/
		/*------------------------------------------------------------------------------------------*/
			if( prmdef_tmp->Attr.IfIsLong )	/* ロングパラメータ	*/
			{
				if(prmdef_tmp->Attr.Base == DSPDEF_BASEDEC)	/* 底が10進	*/
				{
					PnlLedDispPrmDecLong(ldata_tmp, Pnl.CurPos, 
						(USHORT)RpxPrmGetDecimals( prmdef_tmp ), (USHORT)prmdef_tmp->Attr.Sign);
				}
				else								/* 底が10進以外	*/
				{
					PnlLedDispPrmHexLong(ldata_tmp, Pnl.CurPos);
				}
				
			}
			else							/* ワードパラメータ	*/
			{
				if(prmdef_tmp->Attr.Base == DSPDEF_BASEDEC)	/* 底が10進	*/
				{
					if(prmdef_tmp->Attr.Sign == DSPDEF_W_SIGN)
					{
						PnlLedDispPrmDecWordSigned((SHORT)ldata_tmp, Pnl.CurPos, 
							(USHORT)RpxPrmGetDecimals( prmdef_tmp ),
							(SHORT)RpxPrmLowerLimitIdx(Pnl.CurPrmIndx), 
							(SHORT)RpxPrmUpperLimitIdx(Pnl.CurPrmIndx));
					}
					else
					{
						PnlLedDispPrmDecWordUnsigned((USHORT)ldata_tmp, Pnl.CurPos, 
							(USHORT)RpxPrmGetDecimals( prmdef_tmp ) );
					}
				}
				else								/* 底が10進以外	*/
				{
					PnlLedDispPrmHexNblWord((USHORT)ldata_tmp, Pnl.CurPos, 
						(USHORT)prmdef_tmp->Attr.Base);
				}
			}
			break;
	  /*--------------------------------------------------------------------------------------------*/
		case OPWM_UpdtWait:	/* EEPROM書き込み(要求登録)処理	*/
	  /*--------------------------------------------------------------------------------------------*/
			switch(RpxPrmUpdateEepromIdx(Pnl.CurPrmIndx))
			{
			case OK:	/* 正常	*/
				OpBlnkKickFlg.PrmEewrite = 1;	/* ブリンク起動	*/
				Pnl.PrmWrState = OPWM_None;
				break;
			case PRM_EEPROM_RAM_NO_DIFF:	/* 書き込む必要がない(EEPROM値が書き込み値と同じ)	*/
				Pnl.PrmWrState = OPWM_None;
				Pnl.CurPos = 0;
				Pnl.DispMode = PNLMODE_PNNUM;	/* パラメータ番号モードへ	*/
				return;
				break;
			default:
				break;
			}
			break;
	  /*--------------------------------------------------------------------------------------------*/
		default:
	  /*--------------------------------------------------------------------------------------------*/
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		パネルオペレータ Un番号編集モード															*/
/*																									*/
/****************************************************************************************************/
void	PnlMonNoMode( void )
{
USHORT	i, unnum_tmp;
MONDEF	*mondef_tmp;

/*--------------------------------------------------------------------------------------------------*/
/*		キー入力処理																				*/
/*--------------------------------------------------------------------------------------------------*/
		if(OpKeyMk.shup)
		{
			Pnl.CurPos = ((Pnl.CurPos+1) == 3)? 0: Pnl.CurPos+1;
		}
		else if(OpKeyMk.up)
		{
			unnum_tmp = UnMonTbl[Pnl.CurMonIndx].UnNo + (0x01 << (Pnl.CurPos*4));
			Pnl.CurMonIndx = RpxMonSearchIdxOp(unnum_tmp, Pnl.CurMonIndx, &PopMsg);
		}
		else if(OpKeyMk.down)
		{
			if(UnMonTbl[Pnl.CurMonIndx].UnNo < (0x01 << (Pnl.CurPos*4)))
			{
				unnum_tmp = 0x000;
			}
			else
			{
				unnum_tmp = UnMonTbl[Pnl.CurMonIndx].UnNo - (0x01 << (Pnl.CurPos*4));
			}
			Pnl.CurMonIndx = RpxMonSearchIdxOp(unnum_tmp, Pnl.CurMonIndx, &PopMsg);
		}
/*--------------------------------------------------------------------------------------------------*/
/*		現在桁最大値																				*/
/*--------------------------------------------------------------------------------------------------*/
		mondef_tmp = (MONDEF *)UnMonTbl[Pnl.CurMonIndx].MonDefPtr;	/* モニタ定義	*/
		if(mondef_tmp->IfIsLong == TRUE)			/* サイズがロング	*/
		{
			Pnl.CurPosMax = 3;	/* 表示切替あり(下・中・上)	*/
		}
		else										/* サイズがワード以下	*/
		{
			Pnl.CurPosMax = 1;	/* 表示切替なし	*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		ＬＥＤ表示処理																				*/
/*--------------------------------------------------------------------------------------------------*/
		Pnl.DspData.LedCode[4] = LEDCODE_U;
		Pnl.DspData.LedCode[3] = LEDCODE_N;
		RpxLedSetValHex(UnMonTbl[Pnl.CurMonIndx].UnNo, &(Pnl.DspData), 2, 3);
/*--------------------------------------------------------------------------------------------------*/
		for( i = 0; i < sizeof(RoutK.PnlLed); i++ )
		{
			Pnl.DspData.LedBlnkSw[i] = (i == Pnl.CurPos)?	0xFF: 0;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		パネルオペレータ Unデータ表示モード															*/
/*																									*/
/****************************************************************************************************/
void	PnlMonDataMode( void )
{
ULONG	ldata_tmp;
MONDEF	*mondef_tmp;

/*--------------------------------------------------------------------------------------------------*/
/*		表示データ取得																				*/
/*--------------------------------------------------------------------------------------------------*/
		mondef_tmp = (MONDEF *)UnMonTbl[Pnl.CurMonIndx].MonDefPtr;	/* モニタ定義					*/
		RpxMonReadIdx( Pnl.CurMonIndx, &ldata_tmp, MCHK_ACCS_LVL );	/* ldata_tmp <-- モニタ値		*/
/*--------------------------------------------------------------------------------------------------*/
/*		キー入力処理																				*/
/*--------------------------------------------------------------------------------------------------*/
		if(OpKeyMk.data)
		{
			Pnl.CurPos = 0;
			Pnl.DispMode = PNLMODE_UNNUM;	/* パラメータ番号モードへ	*/
			return;
		}
		else if(OpKeyMk.shup)
		{
			Pnl.CurPos = ((Pnl.CurPos+1) == Pnl.CurPosMax)?  0 : Pnl.CurPos+1;
		}
		else if(OpKeyMk.rst)
		{
			RpxMonClearVarIdx( Pnl.CurMonIndx );
		}
/*--------------------------------------------------------------------------------------------------*/
/*		表示処理																					*/
/*--------------------------------------------------------------------------------------------------*/
		switch(mondef_tmp->Base)
		{
		case DSPDEF_BASEDEC:
			if(mondef_tmp->IfIsLong == FALSE)	/* ワード	*/
			{
				RpxLedDispMonDecWord((USHORT)ldata_tmp, &(Pnl.DspData),
					mondef_tmp->Decimals, mondef_tmp->Sign);
			}
			else								/* ロング	*/
			{
				RpxLedDispMonDecLong(ldata_tmp, &(Pnl.DspData), Pnl.CurPos, 
					mondef_tmp->Decimals, mondef_tmp->Sign);
			}
			break;
		case DSPDEF_BASEHEX:
			PnlLedDispMonHexWord((USHORT)ldata_tmp);	/* 現在はワードしかない	*/
			break;
		case DSPDEF_BASEBIT:
			PnlLedDispMonBit((UCHAR)ldata_tmp);			/* 下位8ビットを表示する	*/
			break;
		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		パネルオペレータ LEDブリンク処理															*/
/*																									*/
/****************************************************************************************************/
void	PnlLedBlink( void )
{
/*--------------------------------------------------------------------------------------------------*/
/*		ブリンク終了チェック																		*/
/*--------------------------------------------------------------------------------------------------*/
		if(*((UINT *)&OpBlnkFlg) != 0)
		{
			if(KpiGetLongTimer(&OpBlnkTime) > 1000)
			{	/* ブリンク期間が終了した	*/
				*((UINT *)&OpBlnkFlg) = 0;
			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		ブリンク開始時処理																			*/
/*--------------------------------------------------------------------------------------------------*/
		if(OpBlnkKickFlg.PrmEewrite)	/* パラメータ書込み時	*/
		{
			*((UINT *)&OpBlnkFlg) = *((UINT *)&OpBlnkKickFlg) = 0;
			OpBlnkFlg.PrmEewrite = 1;
			KpiRstLongTimer(&OpBlnkTime);
		}
		else if(OpBlnkKickFlg.no_op)	/* read/writeキーなし	*/
		{
			*((UINT *)&OpBlnkFlg) = *((UINT *)&OpBlnkKickFlg) = 0;
			OpBlnkFlg.no_op = 1;
			KpiRstLongTimer(&OpBlnkTime);
		}
		else if(OpBlnkKickFlg.error)
		{
			*((UINT *)&OpBlnkFlg) = *((UINT *)&OpBlnkKickFlg) = 0;
			OpBlnkFlg.error = 1;
			KpiRstLongTimer(&OpBlnkTime);
		}
		else if(OpBlnkKickFlg.done)
		{
			*((UINT *)&OpBlnkFlg) = *((UINT *)&OpBlnkKickFlg) = 0;
			OpBlnkFlg.done = 1;
			KpiRstLongTimer(&OpBlnkTime);
		}
		else if(OpBlnkKickFlg.end)
		{
			*((UINT *)&OpBlnkFlg) = *((UINT *)&OpBlnkKickFlg) = 0;
			OpBlnkFlg.end = 1;
			KpiRstLongTimer(&OpBlnkTime);
		}
/*--------------------------------------------------------------------------------------------------*/
/*		ブリンク中処理																				*/
/*--------------------------------------------------------------------------------------------------*/
		if(OpBlnkFlg.PrmEewrite)	/* パラメータ書込み時ブリンク	*/
		{
			PnlLedSetBlnkSw( &Pnl.DspData, PNLLED_BLINK, sizeof(RoutK.PnlLed), TRUE );
		}
		else if(OpBlnkFlg.no_op)	/* パネルにはread/writeキーはない	*/
		{
			/* "no_oP"	*/
			Pnl.DspData.LedCode[4] = LEDCODE_N;
			Pnl.DspData.LedCode[3] = LEDCODE_sO;
			Pnl.DspData.LedCode[2] = LEDCODE_B3;
			Pnl.DspData.LedCode[1] = LEDCODE_sO;
			Pnl.DspData.LedCode[0] = LEDCODE_P;
			PnlLedSetBlnkSw( &Pnl.DspData, PNLLED_BLINK, sizeof(RoutK.PnlLed), TRUE );
		}
		else if(OpBlnkFlg.error)
		{
			/* "Error"	*/
			Pnl.DspData.LedCode[4] = LEDCODE_E;
			Pnl.DspData.LedCode[3] = LEDCODE_R;
			Pnl.DspData.LedCode[2] = LEDCODE_R;
			Pnl.DspData.LedCode[1] = LEDCODE_sO;
			Pnl.DspData.LedCode[0] = LEDCODE_R;
			PnlLedSetBlnkSw( &Pnl.DspData, PNLLED_BLINK, sizeof(RoutK.PnlLed), TRUE );
		}
		else if(OpBlnkFlg.done)
		{
			/* " donE"	*/
			Pnl.DspData.LedCode[4] = LEDCODE_BLANK;
			Pnl.DspData.LedCode[3] = LEDCODE_D;
			Pnl.DspData.LedCode[2] = LEDCODE_sO;
			Pnl.DspData.LedCode[1] = LEDCODE_N;
			Pnl.DspData.LedCode[0] = LEDCODE_E;
			PnlLedSetBlnkSw( &Pnl.DspData, PNLLED_BLINK, sizeof(RoutK.PnlLed), TRUE );
		}
		else if(OpBlnkFlg.end)
		{
			/* "  End"	*/
			Pnl.DspData.LedCode[4] = LEDCODE_BLANK;
			Pnl.DspData.LedCode[3] = LEDCODE_BLANK;
			Pnl.DspData.LedCode[2] = LEDCODE_E;
			Pnl.DspData.LedCode[1] = LEDCODE_N;
			Pnl.DspData.LedCode[0] = LEDCODE_D;
			PnlLedSetBlnkSw( &Pnl.DspData, PNLLED_BLINK, sizeof(RoutK.PnlLed), TRUE );
		}
/*--------------------------------------------------------------------------------------------------*/
		PnlLedSetBlnkSw( &Pnl.DspData, PNLLED_NOBLINK, sizeof(RoutK.PnlLed),
						 ((KpiGetLongTimer(&Pnl.BlnkTime)/100) & 0x02) );
		return;
}



/****************************************************************************************************/
/*																									*/
/*		Panel Operator Initialize																	*/
/*																									*/
/****************************************************************************************************/
void	RpxPnlOpInit( void )
{
/*--------------------------------------------------------------------------------------------------*/
/*		構造体変数Pnlのクリア																		*/
/*--------------------------------------------------------------------------------------------------*/
		MlibResetByteMemory( (void *)&Pnl, sizeof(PNL_STRUC) );
/*--------------------------------------------------------------------------------------------------*/
/*		ブリンク変数のクリア																		*/
/*--------------------------------------------------------------------------------------------------*/
		*((UINT *)&OpBlnkFlg)     = 0;
		*((UINT *)&OpBlnkKickFlg) = 0;
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		Panel Monitor Initialize																	*/
/*																									*/
/****************************************************************************************************/
void	RpxPnlOpMonInit( void )
{
/*--------------------------------------------------------------------------------------------------*/
/*		Panel Monitor Initialize																	*/
/*--------------------------------------------------------------------------------------------------*/
		if( (pPrm->Pn52F != PNLMON_NOSET) && (KpiGetAlarmInfo( ) == 0) )
		{
			if( RpiMonSearch( pPrm->Pn52F, 0, &(Pnl.CurMonIndx), MCHK_ACCS_LVL, &PopMsg ) == OK )
			{
				Pnl.DispMode = PNLMODE_UNDATA;
				Pnl.CurPos = 0;
				if ( UnMonTbl[ Pnl.CurMonIndx ].MonDefPtr->IfIsLong == TRUE )	/* サイズがロング	*/
				{
					Pnl.CurPosMax = 3;	/* 表示切替あり(下・中・上)	*/
				}
				else										/* サイズがワード以下	*/
				{
					Pnl.CurPosMax = 1;	/* 表示切替なし	*/
				}
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		Panel Operator Finish																		*/
/*																									*/
/****************************************************************************************************/
void	RpxPnlOpFinish( void )
{
LONG	rc_tmp;

/*--------------------------------------------------------------------------------------------------*/
/*		実行中の補助機能を終了させる処理。実行中か否かの判定は関数内で行っている。					*/
/*--------------------------------------------------------------------------------------------------*/
		RpxFunFinishPnl( Pnl.CurFunIndx, &PopMsg );
/*--------------------------------------------------------------------------------------------------*/
/*		パラメータをEEPROMに書き込む必要がある場合の処理											*/
/*--------------------------------------------------------------------------------------------------*/
		rc_tmp = RpxPrmUpdateEepromIdx( Pnl.CurPrmIndx );
		while( rc_tmp == PRM_EEPROM_WRITE_WAIT || rc_tmp == PRM_EEPROM_WRITE_FAIL )	/* 未完	*/
		{
			rc_tmp = RpxPrmUpdateEepromIdx( Pnl.CurPrmIndx );
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		パネルオペレータ LED表示(パラメータ符号無10進ワード)										*/
/*																									*/
/****************************************************************************************************/
void	PnlLedDispPrmDecWordUnsigned(		/* パネルオペレータ LED表示(パラメータ符号無10進ワード)	*/
		USHORT	data,						/* 表示する値											*/
		USHORT	cur_digit,					/* 現在カーソル位置										*/
		USHORT	decimals		)			/* 小数点以下桁数										*/
{
USHORT	i;

/*--------------------------------------------------------------------------------------------------*/
/*		LED表示(パラメータ符号なし10進ワード)														*/
/*--------------------------------------------------------------------------------------------------*/
		if( cur_digit >= 5 ){ return;}
		RpxLedSetValDec( data, &(Pnl.DspData), 4, 5, FALSE, decimals );
		for( i = 0; i < sizeof(RoutK.PnlLed); i++ )
		{
			Pnl.DspData.LedBlnkSw[i] = (i == cur_digit)? 0xFF: 0;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		パネルオペレータ LED表示(パラメータ符号付10進ワード)										*/
/*																									*/
/****************************************************************************************************/
void	PnlLedDispPrmDecWordSigned(			/* パネルオペレータ LED表示(パラメータ符号付10進ワード)	*/
		SHORT	data,						/* 表示する値											*/
		USHORT	cur_digit,					/* 現在カーソル位置										*/
		USHORT	decimals, 					/* 小数点以下桁数										*/
		SHORT	llimit,						/* 下限値												*/
		SHORT	ulimit			)			/* 上限値												*/
{
USHORT	sdata_tmp, sdata_tmp1, sdata_tmp2;

/*--------------------------------------------------------------------------------------------------*/
/*		開始処理																					*/
/*--------------------------------------------------------------------------------------------------*/
		if( cur_digit >= 5 ){ return;}
		PnlLedSetBlnkSw( &Pnl.DspData, PNLLED_NOBLINK, sizeof(RoutK.PnlLed), TRUE );
		sdata_tmp = (data < 0)? -data: data;
/*--------------------------------------------------------------------------------------------------*/
/*		符号含め5桁で足りる場合																		*/
/*--------------------------------------------------------------------------------------------------*/
		if( (-10000 < llimit) && (ulimit < 10000) )
		{
			RpxLedSetValDec(sdata_tmp, &(Pnl.DspData), 3, 4, FALSE, decimals);
			Pnl.DspData.LedCode[4] = (data < 0)? LEDCODE_MINUS: LEDCODE_BLANK;
			Pnl.DspData.LedBlnkSw[cur_digit] = 0xFF;	/* カーソル桁	*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		符号含め5桁で足りない場合-->切替表示														*/
/*--------------------------------------------------------------------------------------------------*/
		else
		{
			if(Pnl.CurPos < 4)	/* 下4桁	*/
			{
				sdata_tmp1 = sdata_tmp/10000;
				sdata_tmp2 = sdata_tmp - sdata_tmp1*10000;
				RpxLedSetValDec(sdata_tmp2, &(Pnl.DspData), 3, 4, FALSE, decimals);
				Pnl.DspData.LedBlnkSw[cur_digit] = 0xFF;						/* カーソル桁	*/
				Pnl.DspData.LedBlnkSw[4] = Pnl.DspData.LedCode[4] = LEDCODE_B3;	/* 表示桁表示	*/
//				if( data < 0 ){ Pnl.DspData.LedCode[4] |= LEDCODE_POINT;}		/* 負表示の'.'	*/
			}
			else				/* 上1桁+符号	*/
			{
				sdata_tmp1 = sdata_tmp/10000;
				RpxLedSetValDec(sdata_tmp1, &(Pnl.DspData), 0, 1, FALSE, (USHORT)(decimals-4));
				Pnl.DspData.LedCode[2] = Pnl.DspData.LedCode[1] = LEDCODE_BLANK;
				Pnl.DspData.LedCode[3] = (data < 0)? LEDCODE_MINUS: LEDCODE_BLANK;
				Pnl.DspData.LedBlnkSw[cur_digit-4] = 0xFF;						/* カーソル桁	*/
				Pnl.DspData.LedBlnkSw[4] = Pnl.DspData.LedCode[4] = LEDCODE_B6;	/* 表示桁表示	*/
			}
			if( data < 0 ){ Pnl.DspData.LedCode[4] |= LEDCODE_POINT;}			/* 負表示の'.'	*/
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		パネルオペレータ LED表示(パラメータ16進ワード)												*/
/*																									*/
/****************************************************************************************************/
void	PnlLedDispPrmHexNblWord(			/* パネルオペレータ LED表示(パラメータ16進ワード)		*/
		USHORT	data,						/* 表示する値											*/
		USHORT	cur_digit,					/* 現在カーソル位置										*/
		USHORT	base			)			/* 表示の底												*/
{
USHORT	i;

/*--------------------------------------------------------------------------------------------------*/
/*		LED表示(パラメータ16進ワード)																*/
/*--------------------------------------------------------------------------------------------------*/
		if( cur_digit >= 4 ){ return;}
		Pnl.DspData.LedCode[4] = (base == DSPDEF_BASENBL)? 
			(LEDCODE_N | LEDCODE_POINT): (LEDCODE_H | LEDCODE_POINT);
		RpxLedSetValHex( data, &(Pnl.DspData), 3, 4 );
		for(i = 0; i < sizeof(RoutK.PnlLed); i++)
		{
			Pnl.DspData.LedBlnkSw[i] = (i == cur_digit)? 0xFF: 0;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		パネルオペレータ LED表示(10進ロング)														*/
/*																									*/
/****************************************************************************************************/
void	PnlLedDispPrmDecLong(				/* パネルオペレータ LED表示(10進ロング)					*/
		ULONG	ldata,						/* 表示する値											*/
		USHORT	cur_digit,					/* 現在カーソル位置										*/
		USHORT	decimals,					/* 小数点以下桁数										*/
		USHORT	sign			)			/* 符号有無												*/
{
USHORT	sdata_tmp2;
ULONG	ldata_abs, ldata_tmp2;

/*--------------------------------------------------------------------------------------------------*/
/*		開始処理																					*/
/*--------------------------------------------------------------------------------------------------*/
		if( cur_digit >= 10 ){ return;}
		PnlLedSetBlnkSw( &Pnl.DspData, PNLLED_NOBLINK, sizeof(RoutK.PnlLed), TRUE );
		ldata_abs = ((sign == DSPDEF_W_SIGN) && ((LONG)ldata < 0))?	-(LONG)ldata: ldata;
/*--------------------------------------------------------------------------------------------------*/
/*		下4桁																						*/
/*--------------------------------------------------------------------------------------------------*/
		if(Pnl.CurPos < 4)
		{
			ldata_tmp2 = ldata_abs/10000;
			sdata_tmp2 = (USHORT)(ldata_abs - ldata_tmp2*10000);
			RpxLedSetValDec(sdata_tmp2, &(Pnl.DspData), 3, 4, FALSE, decimals);
			Pnl.DspData.LedBlnkSw[cur_digit] = 0xFF;						/* カーソル桁	*/
			Pnl.DspData.LedBlnkSw[4] = Pnl.DspData.LedCode[4] = LEDCODE_B3;	/* 表示桁表示	*/
//			if( (sign == DSPDEF_W_SIGN) && ((LONG)ldata < 0) )
//			{
//				Pnl.DspData.LedCode[4] |= LEDCODE_POINT;					/* 負表示の'.'	*/
//			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		中4桁																						*/
/*--------------------------------------------------------------------------------------------------*/
		else if(Pnl.CurPos < 8)
		{
			ldata_tmp2 = ldata_abs/100000000;
			sdata_tmp2 = (USHORT)((ldata_abs - ldata_tmp2*100000000)/10000);
			RpxLedSetValDec(sdata_tmp2, &(Pnl.DspData), 3, 4, FALSE, (USHORT)(decimals-4));
			Pnl.DspData.LedBlnkSw[cur_digit-4] = 0xFF;						/* カーソル桁	*/
			Pnl.DspData.LedBlnkSw[4] = Pnl.DspData.LedCode[4] = LEDCODE_B6;	/* 表示桁表示	*/
//			if( (sign == DSPDEF_W_SIGN) && ((LONG)ldata < 0) )
//			{
//				Pnl.DspData.LedCode[4] |= LEDCODE_POINT;					/* 負表示の'.'	*/
//			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		上2桁																						*/
/*--------------------------------------------------------------------------------------------------*/
		else
		{
			Pnl.DspData.LedCode[3] = ((sign == DSPDEF_W_SIGN) && ((LONG)ldata < 0))?
				LEDCODE_MINUS : LEDCODE_BLANK;
			Pnl.DspData.LedCode[2] = LEDCODE_BLANK;
			sdata_tmp2 = (USHORT)(ldata_abs/100000000);
			RpxLedSetValDec(sdata_tmp2, &(Pnl.DspData), 1, 2, FALSE, (USHORT)(decimals-8));
			Pnl.DspData.LedBlnkSw[cur_digit-8] = 0xFF;						/* カーソル桁	*/
			Pnl.DspData.LedBlnkSw[4] = Pnl.DspData.LedCode[4] = LEDCODE_B0;	/* 表示桁表示	*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		負表示の'.'																					*/
/*--------------------------------------------------------------------------------------------------*/
		if( (sign == DSPDEF_W_SIGN) && ((LONG)ldata < 0) )
		{
			Pnl.DspData.LedCode[4] |= LEDCODE_POINT;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		パネルオペレータ LED表示(16進ロング)														*/
/*																									*/
/****************************************************************************************************/
void	PnlLedDispPrmHexLong(				/* パネルオペレータ LED表示(16進ロング)					*/
		ULONG	ldata,						/* 表示する値											*/
		USHORT	cur_digit		)			/* 現在カーソル位置										*/
{
USHORT	sdata_tmp2;

/*--------------------------------------------------------------------------------------------------*/
/*		開始処理																					*/
/*--------------------------------------------------------------------------------------------------*/
		if( cur_digit > 8 ){ return;}
		PnlLedSetBlnkSw( &Pnl.DspData, PNLLED_NOBLINK, sizeof(RoutK.PnlLed), TRUE );
/*--------------------------------------------------------------------------------------------------*/
/*		下4桁																						*/
/*--------------------------------------------------------------------------------------------------*/
		if(Pnl.CurPos < 4)
		{
			sdata_tmp2 = (USHORT)(ldata & 0xFFFF);
			RpxLedSetValHex(sdata_tmp2, &(Pnl.DspData), 3, 4);
			Pnl.DspData.LedBlnkSw[cur_digit] = 0xFF;						/* カーソル桁	*/
			Pnl.DspData.LedBlnkSw[4] = Pnl.DspData.LedCode[4] = LEDCODE_B3;	/* 表示桁表示	*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		上4桁																						*/
/*--------------------------------------------------------------------------------------------------*/
		else
		{
			sdata_tmp2 = (USHORT)((ldata >> 16) & 0xFFFF);
			RpxLedSetValHex(sdata_tmp2, &(Pnl.DspData), 3, 4);
			Pnl.DspData.LedBlnkSw[cur_digit-4] = 0xFF;						/* カーソル桁	*/
			Pnl.DspData.LedBlnkSw[4] = Pnl.DspData.LedCode[4] = LEDCODE_B6;	/* 表示桁表示	*/
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		パネルオペレータ LED表示(モニター16進ワード)												*/
/*																									*/
/****************************************************************************************************/
void	PnlLedDispMonHexWord(				/* パネルオペレータ LED表示(モニター16進ワード)			*/
		USHORT	data			)			/* 表示する値											*/
{
/*--------------------------------------------------------------------------------------------------*/
/*		LED表示(モニター16進ワード)																	*/
/*--------------------------------------------------------------------------------------------------*/
		Pnl.DspData.LedCode[4] = LEDCODE_H | LEDCODE_POINT;
		RpxLedSetValHex(data, &(Pnl.DspData), 3, 4);
		PnlLedSetBlnkSw( &Pnl.DspData, PNLLED_NOBLINK, sizeof(RoutK.PnlLed), TRUE );
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		パネルオペレータ LED表示(モニタービットバイト)												*/
/*																									*/
/****************************************************************************************************/
void	PnlLedDispMonBit(					/* パネルオペレータ LED表示(モニタービットバイト)		*/
		UCHAR	data			)			/* 表示する値											*/
{
USHORT	i;

/*--------------------------------------------------------------------------------------------------*/
/*		変数初期化																					*/
/*--------------------------------------------------------------------------------------------------*/
		for( i = 0; i < sizeof(RoutK.PnlLed); i++ )
		{
			Pnl.DspData.LedBlnkSw[i] = Pnl.DspData.LedCode[i] = 0;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		8ビットを表示																				*/
/*--------------------------------------------------------------------------------------------------*/
		for(i = 0; i < 8; i++)
		{
			switch(i & 1)
			{
			case 0:		/* 偶数番目 --> 右	*/
				Pnl.DspData.LedCode[i/2] |= (data & 1)? LEDCODE_B1 : LEDCODE_B2;
				break;
			case 1:		/* 奇数番目 --> 左	*/
				Pnl.DspData.LedCode[i/2] |= (data & 1)? LEDCODE_B5 : LEDCODE_B4;
				break;
			default:
				break;
			}
			data >>= 1;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Hi/Lo指標および区切り点の表示 																*/
/*--------------------------------------------------------------------------------------------------*/
		Pnl.DspData.LedCode[4] = LEDCODE_B1 | LEDCODE_B2 | LEDCODE_POINT;
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		LEDオペレータ モニター表示(10進ワード符号有/無し)											*/
/*																									*/
/****************************************************************************************************/
void	RpxLedDispMonDecWord(				/* LEDオペレータ モニター表示(10進ワード符号有/無)		*/
		USHORT	data,						/* 表示する値											*/
		LEDDSP	*leddspp,					/* LED表示データのアドレス								*/
		USHORT	decimals,					/* 小数点以下桁数										*/
		USHORT	sign			)			/* 符号の有無											*/
{
USHORT	data_tmp;

/*--------------------------------------------------------------------------------------------------*/
/*		モニター表示(10進ワード符号有/無し)															*/
/*--------------------------------------------------------------------------------------------------*/
		data_tmp = ((sign == DSPDEF_W_SIGN) && ((SHORT)data < 0))?	-data: data;
	/*----------------------------------------------------------------------------------------------*/
		if( sign == DSPDEF_W_SIGN )
		{
			data_tmp = ((SHORT)data < 0)?	-data: data;
			RpxLedSetValDec(data_tmp, leddspp, 3, 4, FALSE, decimals);
			leddspp->LedCode[4] = ((SHORT)data < 0)?	LEDCODE_MINUS: LEDCODE_BLANK;
		}
		else
		{
			RpxLedSetValDec(data, leddspp, 4, 5, FALSE, decimals);
		}
	/*----------------------------------------------------------------------------------------------*/
		PnlLedSetBlnkSw( leddspp, PNLLED_NOBLINK, sizeof(RoutK.PnlLed), TRUE );
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		LEDオペレータ モニター表示(10進ロング符号有/無)												*/
/*																									*/
/****************************************************************************************************/
void	RpxLedDispMonDecLong(				/* LEDオペレータ モニター表示(10進ロング符号有/無)		*/
		ULONG	ldata,						/* ldata:表示する値										*/
		LEDDSP	*leddspp,					/* LED表示データのアドレス								*/
		USHORT	dsp_pos,					/* 現在表示位置											*/
		USHORT	decimals,					/* 小数点以下桁数										*/
		USHORT	sign			)			/* 符号有無												*/
{
USHORT	sdata_tmp2;
ULONG	ldata_abs, ldata_tmp2;

/*--------------------------------------------------------------------------------------------------*/
/*		開始処理																					*/
/*--------------------------------------------------------------------------------------------------*/
		if( dsp_pos >= 3 ){ return;}
		PnlLedSetBlnkSw( leddspp, PNLLED_NOBLINK, sizeof(RoutK.PnlLed), TRUE );
		ldata_abs = ((sign == DSPDEF_W_SIGN) && ((LONG)ldata < 0))?	-(LONG)ldata: ldata;
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
		switch(dsp_pos)
		{
		case 0:
			ldata_tmp2 = ldata_abs/10000;
			sdata_tmp2 = (USHORT)(ldata_abs - ldata_tmp2*10000);
			RpxLedSetValDec(sdata_tmp2, leddspp, 3, 4, FALSE, decimals);
			leddspp->LedBlnkSw[4] = leddspp->LedCode[4] = LEDCODE_B3;	/* 表示桁表示	*/
//			if( (sign == DSPDEF_W_SIGN) && ((LONG)ldata < 0) )
//			{
//				leddspp->LedCode[4] |= LEDCODE_POINT;					/* 負表示の'.'	*/
//			}
			break;
		case 1:
			ldata_tmp2 = ldata_abs/100000000;
			sdata_tmp2 = (USHORT)((ldata_abs - ldata_tmp2*100000000)/10000);
			RpxLedSetValDec(sdata_tmp2, leddspp, 3, 4, FALSE, (USHORT)(decimals-4));
			leddspp->LedBlnkSw[4] = leddspp->LedCode[4] = LEDCODE_B6;	/* 表示桁表示	*/
//			if((sign == DSPDEF_W_SIGN) && ((LONG)ldata < 0))
//			{
//				leddspp->LedCode[4] |= LEDCODE_POINT;					/* 負表示の'.'	*/
//			}
			break;
		case 2:
			leddspp->LedCode[3] = ((sign == DSPDEF_W_SIGN) && ((LONG)ldata < 0))?
				LEDCODE_MINUS : LEDCODE_BLANK;
			leddspp->LedCode[2] = LEDCODE_BLANK;
			sdata_tmp2 = (USHORT)(ldata_abs/100000000);
			RpxLedSetValDec(sdata_tmp2, leddspp, 1, 2, FALSE, (USHORT)(decimals-8));
			leddspp->LedBlnkSw[4] = leddspp->LedCode[4] = LEDCODE_B0;	/* 表示桁表示	*/
			break;
		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		負表示の'.'																					*/
/*--------------------------------------------------------------------------------------------------*/
		if( (sign == DSPDEF_W_SIGN) && ((LONG)ldata < 0) )
		{
			leddspp->LedCode[4] |= LEDCODE_POINT;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		LEDオペレータ ステータスビット表示情報セット処理											*/
/*																									*/
/****************************************************************************************************/
void	RpxLedSetStatusBits(				/* LEDオペレータ ステータスビット表示情報セット処理		*/
		LEDDSP	*leddspp		)			/* LED表示データのアドレス								*/
{
/*--------------------------------------------------------------------------------------------------*/
/*		ステータスビット表示情報セット																*/
/*--------------------------------------------------------------------------------------------------*/
		if( BoutV.f.Ctrl.Coin || CoutV.f.Ctrl.ReachSpd || (BoutV.CtrlMode.b.cm == CTRLMODE_TRQ) )
		{
			leddspp->LedCode[4] |= LEDCODE_B0;
		}
		if( !(RinV.f.BaseEnable)					){ leddspp->LedCode[4] |= LEDCODE_B6;}
		if( CoutV.f.Ctrl.MotorMoving				){ leddspp->LedCode[3] |= LEDCODE_B0;}
		if( CoutV.f.Sv.Prefon || CoutV.f.Sv.Vrefon	){ leddspp->LedCode[3] |= LEDCODE_B6;}
		if( CoutV.f.Sv.Trefon || CoutV.f.Sv.PerrClr	){ leddspp->LedCode[3] |= LEDCODE_B3;}
		if( CoutV.f.Seq.MainPowerOn					){ leddspp->LedCode[3] |= LEDCODE_B7;}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		LEDオペレータ 表示値(10進)セット処理														*/
/*																									*/
/****************************************************************************************************/
void	RpxLedSetValDec(					/* LEDオペレータ 表示値(10進)セット処理					*/
		USHORT	data,						/* 表示する値											*/
		LEDDSP	*leddspp,					/* LED表示データのアドレス								*/
		USHORT	startdgt,					/* 開始桁												*/
		USHORT	digit,						/* 処理桁数												*/
		USHORT	sup_flg,					/* 0表示抑止フラグ										*/
		USHORT	decimals		)			/* 小数点以下桁数										*/
{
USHORT	i, var1_tmp, var2_tmp;
UCHAR	*ledbuf_ptr;

/*--------------------------------------------------------------------------------------------------*/
/*		開始処理																					*/
/*--------------------------------------------------------------------------------------------------*/
		if( digit > 5 ){ return;}
		ledbuf_ptr = &(leddspp->LedCode[startdgt]);
/*--------------------------------------------------------------------------------------------------*/
/*		上位桁を落とす																				*/
/*--------------------------------------------------------------------------------------------------*/
		if( data >= MlibDecPowerTbl[digit] )
		{
			var1_tmp = 	(USHORT)(data/MlibDecPowerTbl[digit]);
			data -= (USHORT)(var1_tmp*MlibDecPowerTbl[digit]);
		}
/*--------------------------------------------------------------------------------------------------*/
/*		表示値(10進)セット処理																		*/
/*--------------------------------------------------------------------------------------------------*/
		for(i = 0; i < digit; i++)
		{
			var1_tmp = (USHORT)MlibDecPowerTbl[digit-1-i];	/* 割る数: 例 1000, 100, 10, 1	*/
			var2_tmp = data/var1_tmp;
			if( sup_flg )	/* 上位の0はブランク表示とする	*/
			{
				if( var2_tmp == 0 )
				{
					*(ledbuf_ptr - i) = LEDCODE_BLANK;
				}
				else
				{
					*(ledbuf_ptr - i) = ledcodetbl[var2_tmp];
					sup_flg = FALSE;
				}
			}
			else
			{
				*(ledbuf_ptr - i) = ledcodetbl[var2_tmp];
			}
			data -= (var1_tmp * var2_tmp);
		}
/*--------------------------------------------------------------------------------------------------*/
		if( (0 < (SHORT)decimals) && ((SHORT)decimals < (SHORT)digit) )
		{
			*(ledbuf_ptr-(digit-1)+decimals) |= LEDCODE_POINT;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		LEDオペレータ 表示値(16進)セット処理														*/
/*																									*/
/****************************************************************************************************/
void	RpxLedSetValHex(					/* LEDオペレータ 表示値(16進)セット処理					*/
		USHORT	data,						/* 表示する値											*/
		LEDDSP	*leddspp,					/* LED表示データのアドレス								*/
		USHORT	startdgt,					/* 開始桁												*/
		USHORT	digit			)			/* 処理桁数												*/
{
USHORT	i, var1_tmp, var2_tmp;
UCHAR	*ledbuf_ptr;

/*--------------------------------------------------------------------------------------------------*/
/*		開始処理																					*/
/*--------------------------------------------------------------------------------------------------*/
		if( digit > 4 ){ return;}
		ledbuf_ptr = &(leddspp->LedCode[startdgt]);
/*--------------------------------------------------------------------------------------------------*/
/*		上位桁を落とす																				*/
/*--------------------------------------------------------------------------------------------------*/
		if( data >= (0x1 << (digit*4)) )
		{
			data &= ((0x1 << (digit*4)) -1);	/* 上位桁を落とす	*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		表示値(10進)セット処理																		*/
/*--------------------------------------------------------------------------------------------------*/
		for(i = 0; i < digit; i++)
		{
			var1_tmp = 0x1 << ((digit-1-i)*4);	/* 割る数: 例 0x1000, 0x100, 0x10, 0x1	*/
			var2_tmp = data/var1_tmp;
			*(ledbuf_ptr - i) = ledcodetbl[var2_tmp];
			data -= (var1_tmp * var2_tmp);
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		LED Operator String Set																		*/
/*																									*/
/****************************************************************************************************/
void	RpxLedSetStr(						/* LED Operator String Set								*/
		UCHAR	*str_ptr,					/* 表示する文字列アドレス								*/
		LEDDSP	*leddspp,					/* LED表示データのアドレス								*/
		USHORT	startdgt,					/* 開始桁												*/
		USHORT	length			)			/* 表示文字数											*/
{
USHORT	str_val;
USHORT	idx_tmp;
USHORT	str_idx, dgt_tmp;
UCHAR	*ledbuf_ptr;

/*--------------------------------------------------------------------------------------------------*/
/*		長さ0 or 開始位置が表示バッファ外															*/
/*--------------------------------------------------------------------------------------------------*/
		if( (length == 0) || (startdgt >= sizeof(RoutK.PnlLed)) )
		{
			return;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		開始処理																					*/
/*--------------------------------------------------------------------------------------------------*/
		ledbuf_ptr = &(leddspp->LedCode[0]);
		str_idx = 0;
		dgt_tmp = startdgt;
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
		while(*(str_ptr + str_idx) != 0)
		{
			str_val = (USHORT)*(str_ptr + str_idx);
			idx_tmp = str_val - '-';
		/*------------------------------------------------------------------------------------------*/
		/*	文字データ < '-' の場合																	*/
		/*------------------------------------------------------------------------------------------*/
			if(str_val < '-')
			{
				*(ledbuf_ptr + dgt_tmp) = LEDCODE_BLANK;
			}
		/*------------------------------------------------------------------------------------------*/
		/*	'-' <= 文字データ <= 'Z' の場合															*/
		/*------------------------------------------------------------------------------------------*/
			else if( str_val <= 'Z' )
			{
				if( (str_idx != 0) && 					/* 文字列先頭でなく		*/
					('.' == *(str_ptr + str_idx)) && 	/* '.'であって			*/
					('.' != *(str_ptr + str_idx -1))	/* 前の文字が'.'でない	*/
				)
				{	/* '.'以外の文字の後に'.'がある場合は前の文字に付加する。[5][.] -> [5.]	*/
					dgt_tmp++;
					*(ledbuf_ptr + dgt_tmp) |= LEDCODE_POINT;
				}
				else	*(ledbuf_ptr + dgt_tmp) = ledconvtbl[idx_tmp];
			}
		/*------------------------------------------------------------------------------------------*/
		/*	文字データが小文字の場合																*/
		/*------------------------------------------------------------------------------------------*/
			else if( ('a' <= str_val) && (str_val <= 'z') )
			{
				if(      'h' == str_val ){ *(ledbuf_ptr + dgt_tmp) = LEDCODE_sH;}
				else if( 'o' == str_val ){ *(ledbuf_ptr + dgt_tmp) = LEDCODE_sO;}
				else if( 'u' == str_val ){ *(ledbuf_ptr + dgt_tmp) = LEDCODE_sU;}
				else { *(ledbuf_ptr + dgt_tmp) = ledconvtbl[idx_tmp - ('a' - 'A')];}
			}
		/*------------------------------------------------------------------------------------------*/
		/*	上記以外の場合																			*/
		/*------------------------------------------------------------------------------------------*/
			else
			{
				if(      '_' == str_val ){ *(ledbuf_ptr + dgt_tmp) = LEDCODE_B3;}
				else if( '~' == str_val ){ *(ledbuf_ptr + dgt_tmp) = LEDCODE_B0;}
				else					 { *(ledbuf_ptr + dgt_tmp) = LEDCODE_BLANK;}
			}
		/*------------------------------------------------------------------------------------------*/
		/*	バッファ右端にきた																		*/
		/*------------------------------------------------------------------------------------------*/
			if( dgt_tmp == 0 )
			{
				if( ((str_idx+1) < length) && 				/* 次の入力文字があって		*/
					('.' == *(str_ptr + str_idx + 1)) && 	/* 次の文字が'.'であって	*/
					('.' != *(str_ptr + str_idx)) 	)		/* 今回の文字が'.'でない時	*/
				{
					*(ledbuf_ptr + dgt_tmp) |= LEDCODE_POINT;	/* '.'を付加する		*/
				}
				break;
			}
		/*------------------------------------------------------------------------------------------*/
			dgt_tmp--;
			str_idx++;
			if( str_idx >= length ){ break;}
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		LED Operator Blink Set																		*/
/*																									*/
/****************************************************************************************************/
void	RpxLedSetBlnk(						/* LED Operator Blink Set								*/
		LEDDSP	*leddspp,					/* LED表示データのアドレス								*/
		UCHAR	blnkptn			)			/* 点滅パターン(1bit=1文字分)							*/
{
SHORT	i;

/*--------------------------------------------------------------------------------------------------*/
/*		LED Operator Blink Set																		*/
/*--------------------------------------------------------------------------------------------------*/
		for( i = 0; i < sizeof(RoutK.PnlLed); i++ )
		{
			leddspp->LedBlnkSw[i] = (blnkptn & (1 << i))? 0xFF: 0;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		Panel LED Set to Kernel Variables															*/
/*																									*/
/****************************************************************************************************/
void	PnlLedSetKnlBuffer(					/* Panel LED Set to Kernel Variables					*/
		LEDDSP	*leddsp,					/*														*/
		UCHAR	*dist,						/*														*/
		LONG	size			)			/*														*/
{
LONG	i;

/*--------------------------------------------------------------------------------------------------*/
/*		Panel LED Set to Kernel Variables															*/
/*--------------------------------------------------------------------------------------------------*/
		for ( i = 0; i < size; i++ )
		{
			dist[i] = (UCHAR)(leddsp->LedCode[i]) & (~((UCHAR)(leddsp->LedBlnkSw[i])));
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		Panel LED Set to Same Data																	*/
/*																									*/
/****************************************************************************************************/
void	PnlLedSetBlnkSw(					/* Panel LED Set to Same Data							*/
		LEDDSP	*leddsp,					/*														*/
		UCHAR	data,						/*														*/
		LONG	size,						/*														*/
		LONG	flag			)			/*														*/
{
LONG	i;

/*--------------------------------------------------------------------------------------------------*/
/*		Panel LED Set to Same Data																	*/
/*--------------------------------------------------------------------------------------------------*/
		if ( flag != FALSE )
		{
			for ( i = 0; i < size; i++ )
			{
				leddsp->LedBlnkSw[i] = data;
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}








/***************************************** end of file **********************************************/
