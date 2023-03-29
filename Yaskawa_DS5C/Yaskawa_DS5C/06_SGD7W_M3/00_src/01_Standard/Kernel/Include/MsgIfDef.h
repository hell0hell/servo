/****************************************************************************************************/
/*																									*/
/*																									*/
/*		MsgIfDef.h : Message Interface Define Header File											*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1. Message Interface Data Definition														*/
/*																									*/
/*		2. Message Interface Macro Definition														*/
/*																									*/
/*		3.																							*/
/*																									*/
/*		4.																							*/
/*																									*/
/*		5.																							*/
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
/*		Rev.1.00 : 2008.06.11  T.Taniguchi															*/
/*		Rev.1.01 : 2013.04.15  K.Ozaki																*/
/*		Rev.1.02 : 2013.08.10  K.Ozaki		##Panel Operator##										*/
/*																									*/
/*																									*/
/****************************************************************************************************/

#ifndef		MSFGIF_H_
#define		MSFGIF_H_

/****************************************************************************************************/
/*																									*/
/*		Define Function Code, Error Code, etc														*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Main Function Code Definition																*/
/*--------------------------------------------------------------------------------------------------*/
#define	MFC_NONE			0x00				/* 不使用											*/
#define	MFC_LOOPBACK		0x08				/* ループバックテスト								*/
#define MFC_SVXMSG16		0x40				/* サーボ拡張メッセージ								*/
#define MFC_SVXMSG32		0x41				/* サーボ拡張メッセージ								*/
#define	MFC_MECHATROLINK	0x42				/* MECHATROLINK Message								*/
#define	MFC_SYSDWNLD		0x50				/* システムダウンロード								*/
#define	MFC_RELAY 			0x51				/* 中継コマンド										*/

/*--------------------------------------------------------------------------------------------------*/
/*		Sub Function Code Definition for 40H/41H													*/
/*--------------------------------------------------------------------------------------------------*/
#define	SFC_RD_ONEREG		0x40				/* 単一レジスタ読出し								*/
#define	SFC_WR_ONEREG		0x06				/* 単一レジスタ書込み								*/
#define	SFC_RD_SEQREG		0x03				/* 連続レジスタ読出し								*/
#define	SFC_WR_SEQREG		0x10				/* 連続レジスタ書込み								*/
#define	SFC_RD_RANREG		0x0D				/* 非連続レジスタ読出し								*/
#define	SFC_WR_RANREG		0x0E				/* 非連続レジスタ書込み								*/
#define	SFC_RD_MAXNUM		0x41				/* 最大レジスタ個数読出し							*/
/*--------------------------------------------------------------------------------------------------*/
#define	SFC_RD_SEQMEM		0x50				/* 連続メモリ読出し									*/
#define	SFC_WR_SEQMEM		0x51				/* 連続メモリ書込み									*/

/*--------------------------------------------------------------------------------------------------*/
/*		Sub Function Code Definition for 42H														*/
/*--------------------------------------------------------------------------------------------------*/
#define	SFC_RD_REQREG_42	0x01				/* メモリの読み出し										*/
#define	SFC_WR_SEQREG_42	0x02				/* メモリの書き込み										*/
#define	SFC_RD_RANREG_42	0x03				/* メモリの読み出し(非連続)								*/
#define	SFC_WR_RANREG_42	0x04				/* メモリの書き込み(非連続)								*/
#define	SFC_RD_MAXNUM_42	0x11				/* 最大メッセージサイズ読み出し							*/
#define	SFC_RD_PRDCTID_42	0x7F				/* 機種情報読み出し									*/
/*--------------------------------------------------------------------------------------------------*/
#define	MBUS42_HEADERSIZE	0x08				/* 42Hコマンドヘッダー部サイズ								*/
#define	MBUS42_ADRSIZE		0x04				/* 42Hコマンドアドレス部サイズ								*/
#define MBUS42_DATASIZEL	0x04				/* 42Hコマンドデータ部サイズ（非連続書き込み時）				*/
#define	MBUS42_CRC_SIZE		0x02				/* 42HコマンドCRC部サイズ								*/
#define	MBUS42_DTYPE_B		0x01				/* データ型：Byte型(8bit)								*/
#define	MBUS42_DTYPE_S		0x02				/* データ型：Short型(16bit)							*/
#define	MBUS42_DTYPE_L		0x03				/* データ型：Long型(32bit)								*/
/* <S150> >>>>> */
/*--------------------------------------------------------------------------------------------------*/
/*		Sub Function Code Definition for 51H														*/
/*--------------------------------------------------------------------------------------------------*/
#define MBUS51_FIX_SIZE			0x0E			/* 中継コマンド固定サイズ								*/
#define MBUS51_PASS_DATA_SIZE	0x08			/* 中継コマンド可変サイズ(このサイズの倍数で変化)				*/
#define MBUS51_MAX_ROUTENUM		0x0A			/* 中継段数　最大値									*/
/* <<<<< <S150> */


/*--------------------------------------------------------------------------------------------------*/
/*		Download			SubFuncCode		   S:Support, -:Not Support								*/
/*--------------------------------------------------------------------------------------------------*/
#define	SFC_DLID_RD			0x0000				/* S : ＩＤ情報読出し								*/
#define	SFC_SWVER_RD		0x0001				/* S : ﾌﾟﾛｸﾞﾗﾑ書込み条件取得						*/
#define	SFC_DLWAY_RD		0x0002				/* S : ﾌﾟﾛｸﾞﾗﾑ書込み条件取得						*/
#define	SFC_DL_START		0x0003				/* S : ﾀﾞｳﾝﾛｰﾄﾞ開始									*/
#define	SFC_DL_PRGDEL		0x0004				/* S : ﾌﾞﾛｯｸ消去									*/
#define	SFC_DL_PRGWR		0x0005				/* S : ﾌﾟﾛｸﾞﾗﾑ書込み								*/
#define	SFC_DL_DELSND		0x0006				/* - : ﾌﾞﾛｯｸ消去情報(消去情報転送のみ.実消去しない)	*/
#define	SFC_DL_PRGSND		0x0007				/* - : ﾌﾟﾛｸﾞﾗﾑ送信(送信のみ.実書込しない)			*/
#define	SFC_DL_PRGDELA		0x0008				/* - : ﾌﾞﾛｯｸ一括消去指令							*/
#define	SFC_DL_PRGWRA		0x0009				/* - : ﾌﾟﾛｸﾞﾗﾑ一括書込み指令						*/
#define	SFC_DL_END			0x000A				/* S : ﾀﾞｳﾝﾛｰﾄﾞ終了指令								*/
#define	SFC_DL_PRGRD		0x000B				/* S : ﾌﾟﾛｸﾞﾗﾑ読出し指令							*/
#define	SFC_DL_REBOOT		0x000C				/* S : ﾌﾟﾛｸﾞﾗﾑ再起動指令							*/
#define	SFC_DL_SIFCHG		0x000D				/* S : ｼﾘｱﾙＩＦ速度変更								*/
#define	SFC_DL_HIS_RD		0x000E				/* S : ﾀﾞｳﾝﾛｰﾄﾞ履歴読出し							*/
#define	SFC_DL_HIS_WR		0x000F				/* S : ﾀﾞｳﾝﾛｰﾄﾞ履歴読書込み							*/
/*--------------------------------------------------------------------------------------------------*/
/*		Error Code Definition																		*/
/*--------------------------------------------------------------------------------------------------*/
#define ECD_ERR_MFC			0x00				/* Error : Main Function Code						*/
#define ECD_ERR_SFC			0x00				/* Error : Sub  Function Code						*/
/*--------------------------------------------------------------------------------------------------*/
/*		SigmaMemobus Error Code Definition															*/
/*--------------------------------------------------------------------------------------------------*/
#define	SMBUS_EC_FNCDERR	(0x01 | 0x200)		/* ファンクションコード異常							*/
#define	SMBUS_EC_REGNOERR	(0x02 | 0x200)		/* レジスタ番号不良									*/
#define	SMBUS_EC_DANUMERR	(0x03 | 0x200)		/* 個数不良異常										*/
/*--------------------------------------------------------------------------------------------------*/
#define	SMBUS_SEC_REGNOERR	(0x30 | 0x200)		/* レジスタ番号不良(レベル高)						*/
#define	SMBUS_SEC_ACCSERR	(0x31 | 0x200)		/* アクセス制限エラー								*/
#define	SMBUS_SEC_DALMTERR	(0x32 | 0x200)		/* 設定値範囲外エラー								*/
#define	SMBUS_SEC_DAMTHERR	(0x33 | 0x200)		/* データ整合エラー 								*/
#define	SMBUS_SEC_CONDERR	(0x34 | 0x200)		/* 条件エラー										*/
#define	SMBUS_SEC_INTLOCK	(0x35 | 0x200)		/* 処理競合エラー									*/
/*--------------------------------------------------------------------------------------------------*/
/*		送信開始時の応答・無応答定義																*/
/*--------------------------------------------------------------------------------------------------*/
#define	RESMT_COMPLETE		0x01				/* MsgType : 完了応答								*/
#define	RESMT_NORESPONSE	0x00				/* MsgType : 無応答(メッセージ異常)					*/
/*--------------------------------------------------------------------------------------------------*/
/*		シグマメッセージエラーコード定義															*/
/*--------------------------------------------------------------------------------------------------*/
#define	SMSGEC_FNCDERR		0x10				/* ファンクションコード異常							*/
#define	SMSGEC_SEQXERR		0x10				/* 通信シーケンス異常(継続伝送)		@@@仕様未定@@@	*/
#define	SMSGEC_CMDXERR		0x14				/* ダウンロード開始コマンド異常		@@@仕様未定@@@	*/
/*--------------------------------------------------------------------------------------------------*/
/*		補助機能応答ホールド																		*/
/*--------------------------------------------------------------------------------------------------*/
#define	REGRES_NOHOLD		0x00				/* 補助機能応答ホールド解除			@@@見直し要@@@	*/
#define	REGRES_HOLD			0x01				/* 補助機能応答ホールド				@@@見直し要@@@	*/

/****************************************************************************************************/
/*																									*/
/*		Register IF Data Definition																	*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		レジスタＩＦ関数正常戻り値定義																*/
/*--------------------------------------------------------------------------------------------------*/
#define	REGIF_COMPLETE		0x00		/* 処理完了							*/
#define	REGIF_CONTINUE		0x01		/* 処理継続							*/
/*--------------------------------------------------------------------------------------------------*/
/*		レジスタＩＦ関数異常戻り値定義(シグマメッセージエラーコードと兼用)	*/
/*--------------------------------------------------------------------------------------------------*/
#define	REGIF_CMDERR		0x10		/* レジスタＩＦコマンド異常			*/
#define	REGIF_REGNOERR		0x11		/* レジスタ番号異常 				*/
#define	REGIF_DALMTERR		0x12		/* データ異常(上下限異常)   		*/
#define	REGIF_DACALERR		0x13		/* データ異常(演算異常) 			*/
#define	REGIF_DAMTHERR		0x14		/* データ整合性異常 				*/
#define	REGIF_DAMTHERR2		(0x14 | 0x100)		/* データ整合性異常 		*/
#define	REGIF_DANUMERR		0x15		/* データ個数異常   				*/
#define	REGIF_FOPEERR1		0x30		/* 他ＣＨによる操作モード占有中 	*/
#define	REGIF_FOPEERR2		0x31		/* 他ＣＨによる操作モード占有発生	*/
#define	REGIF_FOPEERR3		0x3F		/* 他ＣＨとの処理競合				*/
#define	REGIF_FOPEERR4		0x3F		/* 補助機能実行異常					*/


/****************************************************************************************************/
/*																									*/
/*		Message Interface Struct Definition															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		Hmsg識別用Flag設定	|	PnlOpe	LcdOpe	SifMsg	UsbMsg	NetMsg  LnkMsg						*/
/*	  ----------------------+-------------------------------------------------------------------	*/
/*		XxxMsg.f.OpeMsg		|	TRUE	TRUE														*/
/*		XxxMsg.f.PopMsg		|	TRUE																*/
/*		XxxMsg.f.LopMsg		|			TRUE														*/
/*	  ----------------------+-------------------------------------------------------------------	*/
/*		XxxMsg.f.ComMsg		|					TRUE	TRUE	TRUE	TRUE						*/
/*		XxxMsg.f.SifMsg		|					TRUE												*/
/*		XxxMsg.f.UsbMsg		|							TRUE										*/
/*		XxxMsg.f.NetMsg		|									TRUE								*/
/*		XxxMsg.f.LnkMsg		|											TRUE						*/
/*	  ----------------------+-------------------------------------------------------------------	*/
/*																									*/
/*																									*/
/****************************************************************************************************/
typedef	struct	_MSGIF {						/* Message IF Definition 							*/
	/*----------------------------------------------------------------------------------------------*/
		struct	{								/* TODO:Hmsg識別用フラグ							*/
		UINT	OpeMsgIF		:1;				/* オペレータ識別フラグ								*/
		UINT	PopMsgIF		:1;				/* パネルオペレータ識別フラグ						*/
		UINT	LopMsgIF		:1;				/* ＬＣＤオペレータ識別フラグ						*/
		UINT	XxxOpeIF		:1;				/* 予備												*/
	/*----------------------------------------------------------------------------------------------*/
		UINT	ComMsgIF		:1;				/* 通信メッセージ識別フラグ							*/
		UINT	SifMsgIF		:1;				/* シリアル通信メッセージ識別フラグ					*/
		UINT	UsbMsgIF		:1;				/* ＵＳＢ通信メッセージ識別フラグ					*/
		UINT	NetMsgIF		:1;				/* ネットワークメッセージ識別フラグ					*/
		UINT	LnkMsgIF		:1;				/* リンク通信メッセージ識別フラグ					*/
		UINT	MlnkMsgIF		:1;				/* メカトロリンク通信メッセージ識別フラグ			*//*<MEMO_001T>*/
	/*----------------------------------------------------------------------------------------------*/
//		UINT	SpareFlags		:23;			/* Spare											*/
		UINT	SpareFlags		:22;			/* Spare											*//*<S150>*/
		} f;									/*													*/
/*--------------------------------------------------------------------------------------------------*/
	CSIF   *Sif;								/* Pointer to the Serial interface					*/
	UCHAR  *CmdBuf;								/* Pointer to the Command Message Buffer			*/
	UCHAR  *ResBuf;								/* Pointer to the Response Message Buffer			*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	RcvBufSize;							/* Receive Message Buffer Size						*/
	USHORT	SndBufSize;							/* Send Message Buffer Size							*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	CmdMsgLen;							/* Command Message Length							*/
	USHORT	AccessLvl;							/* Access Level USER1/USER2/SYSTEM)					*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	ComMode;							/* Communication Mode (LCDOPE,MSG,etc)				*/
	USHORT	ComPortType;						/* Channel Information ( not used )					*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	ErrRno;								/* Error Register Number							*/
	USHORT	LstErrRno;							/* Last Error Register Number						*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	FunSts;								/* Function Execution Status						*/
	UCHAR	LastAxisIdx;						/* Multi-Axis : Last Axis Index						*/
	UCHAR	ReserveChar0;						/* Reserved											*/
/*--------------------------------------------------------------------------------------------------*/
	ULONG	ComChkTime;							/* Timeout Timer									*/
/*--------------------------------------------------------------------------------------------------*/
#ifndef	_MERCURY_PLATFORM_			/*<S190>*/
	AXIS_HANDLE *pAxRsc;						/* TODO: あとで消す 								*/
#else	/*_MERCURY_PLATFORM_*/
	void		 *pAxRsc;						/* TODO: あとで消す 								*/
#endif	/*_MERCURY_PLATFORM_*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	OpeDspMode;							/* オペレータのユーザー定数表示選択					*/	/* <S113> */
	USHORT	LstOpeDspMode;						/* オペレータのユーザー定数表示選択					*/	/* <S197> */
#ifndef	_MERCURY_PLATFORM_			/*<S190>*/
	MEMOBUS_ADDR_MAP	*AxisMap;				/* <S150> */
#else	/*_MERCURY_PLATFORM_*/
	void	*AxisMap;				/* <S150> */
#endif	/*_MERCURY_PLATFORM_*/
//	LONG  (*StxFunc)( UCHAR *, USHORT );		/* TODO: 応答メッセージ送信開始ＡＰＩ関数	*/
//	void  (*ResFunc)( struct _MSGIF * );		/* TODO: Ｆｎ実行結果応答メッセージ処理関数*/
/*----------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------*/
//	USHORT	FunExeIdx;							/* TODO:補助機能実行テーブル指定インデックス	*/
//	USHORT	FnStsToOpi;							/* TODO:補助機能実行結果(OPTION-SERVO I/F用)	*/
/*----------------------------------------------------------------------------------------------*/
//	USHORT	FunResHold;							/* TODO:補助機能応答ホールド				*/
/*----------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------*/
//	USHORT	ComTimeShort;						/* TODO:オプション通信タイムアウト計測用	*/
//	USHORT	RcvWait;							/* TODO:SCメッセージ通信受信待ちフラグ	*/
/*----------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------*/
//	void *PrmDef;								/* TODO: Pointor to the Parameter Definition 	*/	
//	void *MonDef;								/* TODO: Pointer to the Monitor Definition 		*/	
//	void *FunDef;								/* TODO: Pointer to the Fn Definition 			*/
//TODO: void *AxDef;							/* TODO: Pointer to the Axis Definition			*/
} MSGIF;
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W（2軸対応）のみ	*//* <S0E4> */
/****************************************************************************************************/
/*		EXTERN LIST																					*/
/****************************************************************************************************/
/* M-3 Message Interface */
extern	MSGIF	LopMsg;					/* LCD Operator Messsage I/F 						*/
#elif CSW_BOARD_TYPE == SGD7S_TYPE	/*	SGD7S（単軸時）のみ	*/
#endif
typedef	MSGIF*	HMSGIF;							/* 同上用ハンドル定義				*/

#if 0											
/*--------------------------------------------------------------------------------------------------*/
/* 
 * 以下Jupiter(参考用)								
 */
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	_MSGIF {						/* メッセージＩＦ用構造体定義						*/
/*--------------------------------------------------------------------------------------------------*/
		struct	{								/* Hmsg識別用フラグ(初期化時に設定)					*/
		UINT	OpeMsgIF		:1;				/* オペレータ識別フラグ								*/
		UINT	PopMsgIF		:1;				/* パネルオペレータ識別フラグ						*/
		UINT	LopMsgIF		:1;				/* ＬＣＤオペレータ識別フラグ						*/
		UINT	XxxOpeIF		:1;				/* 予備												*/
	/*----------------------------------------------------------------------------------------------*/
		UINT	ComMsgIF		:1;				/* 通信メッセージ識別フラグ							*/
		UINT	SifMsgIF		:1;				/* シリアル通信メッセージ識別フラグ					*/
		UINT	UsbMsgIF		:1;				/* ＵＳＢ通信メッセージ識別フラグ					*/
		UINT	NetMsgIF		:1;				/* ネットワークメッセージ識別フラグ					*/
		UINT	LnkMsgIF		:1;				/* リンク通信メッセージ識別フラグ					*/
	/*----------------------------------------------------------------------------------------------*/
		UINT	SpareFlags		:23;			/* Spare											*/
		} f;									/*													*/
/*--------------------------------------------------------------------------------------------------*/
		UCHAR  *CmdBuf;							/* 指令メッセージバッファポインタ					*/
		UCHAR  *ResBuf;							/* 応答メッセージバッファポインタ					*/
	/*----------------------------------------------------------------------------------------------*/
		USHORT	CmdBufSize;						/* 指令メッセージバッファサイズ(Byte数)				*/
		USHORT	ResBufSize;						/* 応答メッセージバッファサイズ(Byte数)				*/
	/*----------------------------------------------------------------------------------------------*/
		UCHAR	ComMode;						/* 通信モード(接続中/非接続/etc)					*/
		UCHAR	AccessLvl;						/* アクセスレベル(USER1/USER2/SYSTEM)				*/
		USHORT	CmdMsgLen;						/* 指令メッセージ長									*/
		ULONG	ComChkTime;						/* 通信タイムアウト計測用							*/
	/*----------------------------------------------------------------------------------------------*/
		UCHAR	FunIdx;							/* 補助機能テーブルインデックス						*/
		USHORT	FunSts;							/* 補助機能実行結果									*/
		USHORT	ErrRno;							/* 異常レジスタ番号									*/
		USHORT	LstErrRno;						/* 前回コマンドの異常レジスタ番号					*/
	/*----------------------------------------------------------------------------------------------*/
		UCHAR	FunResHold;						/* 補助機能応答ホールド								*/
		UCHAR	LastAxisIdx;					/* 多軸ドライブ：前回軸インデクス					*/
	/*----------------------------------------------------------------------------------------------*/
		USHORT	DldAllPrmRdCount;				/* Read  Count  for Download All Parameter			*/
		USHORT	DldAllPrmRdChkSum;				/* Read  ChkSum for Download All Parameter			*/
		USHORT	DldAllPrmWrCount;				/* Write Count  for Download All Parameter			*/
		USHORT	DldAllPrmWrChkSum;				/* Write ChkSum for Download All Parameter			*/
	/*----------------------------------------------------------------------------------------------*/
		ULONG	LopAccessLvl;					/* LCD Operator access level for Converter <0BJ14>	*/
} MSGIF, *HMSGIF;								/* 構造体型名＆ハンドル定義							*/
#endif

/*--------------------------------------------------------------------------------------------------*/
/*
 * 	Message Handle Check Macro Definition
 *
 *
 */
/*--------------------------------------------------------------------------------------------------*/
#define	KPI_CHK_OpeHmsg( Hmsg )		( (Hmsg)->f.OpeMsgIF )
#define	KPI_CHK_LopHmsg( Hmsg )		( (Hmsg)->f.LopMsgIF )
#define	KPI_CHK_ComHmsg( Hmsg )		( (Hmsg)->f.ComMsgIF )
#define	KPI_CHK_SifHmsg( Hmsg )		( (Hmsg)->f.SifMsgIF )
#define	KPI_CHK_UsbHmsg( Hmsg )		( (Hmsg)->f.UsbMsgIF )
#define	KPI_CHK_NetHmsg( Hmsg )		( (Hmsg)->f.NetMsgIF )
#define	KPI_CHK_LnkHmsg( Hmsg )		( (Hmsg)->f.LnkMsgIF )
#define	KPI_CHK_PopHmsg( Hmsg )		( (Hmsg)->f.PopMsgIF )

/*--------------------------------------------------------------------------------------------------*/
/*
 *	Massage Handle Initialization
 *
 *  KRI_INIT_MSGIF() : Initialize Message Handle
 *  KRI_BIND_MSGIF() : Bind a Message Handle to Other Message Handle
 *
 */
/*--------------------------------------------------------------------------------------------------*/
#define KRI_INIT_MSGIF( hmsg, csif )	{ 		/* bind CSIF to Message IF */ 	\
		MlibResetLongMemory( hmsg, sizeof(MSGIF)/4 );	/* reset memory */		\
		(hmsg)->Sif = (csif);					/* set sif */			\
		(hmsg)->CmdBuf = (csif)->RxBuf;			/* set cmd buf */ 		\
		(hmsg)->ResBuf = (csif)->TxBuf;			/* set res buf */		\
		(hmsg)->RcvBufSize = (csif)->RcvBufSize;/* set rcv buf size */ \
		(hmsg)->SndBufSize = (csif)->RcvBufSize;/* set send buf size */\
	}
/*--------------------------------------------------------------------------------------------------*/
#define KRI_BIND_MSGIF( hmsg, src )	{ 			/* bind Message IF to other Message IF */ 	\
		(hmsg)->Sif = (src)->Sif;				/* set sif */			\
		(hmsg)->CmdBuf = (src)->CmdBuf;			/* set cmd buf */ 		\
		(hmsg)->ResBuf = (src)->ResBuf;			/* set res buf */		\
		(hmsg)->RcvBufSize = (src)->RcvBufSize; /* set rcv buf size */ \
		(hmsg)->SndBufSize = (src)->RcvBufSize; /* set send buf size */\
	}

/*--------------------------------------------------------------------------------------------------*/
/*
 * Message Handle Flag Initialization
 * 
 * KRI_SET_FLG_LCDOPE() : for Lcd Operator
 * KRI_SET_FLG_SRLMSG() : for UART message 
 * KRI_SET_FLG_USBMSG() : for USB message
 * KRI_SET_FLG_PNLOPE() : for Panel Operator
 *
 */
/*--------------------------------------------------------------------------------------------------*/
#define KRI_SET_FLG_LCDOPE(hMsg)	{ \
					*(UINT *)&((hMsg)->f) = (UINT)0; \
					(hMsg)->f.OpeMsgIF = 1; \
					(hMsg)->f.LopMsgIF = 1; \
				}
#define KRI_SET_FLG_SRLMSG(hMsg)	{ \
					*(UINT *)&((hMsg)->f) = (UINT)0; \
					(hMsg)->f.ComMsgIF = 1; \
					(hMsg)->f.SifMsgIF = 1; \
				}
#define KRI_SET_FLG_USBMSG(hMsg)	{ \
					*(UINT *)&((hMsg)->f) = (UINT)0; \
					(hMsg)->f.ComMsgIF = 1; \
					(hMsg)->f.UsbMsgIF = 1; \
				}
#define KRI_SET_FLG_PNLOPE(hMsg)	{ \
					*(UINT *)&((hMsg)->f) = (UINT)0; \
					(hMsg)->f.OpeMsgIF = 1; \
					(hMsg)->f.PopMsgIF = 1; \
				}
/* <S150> >>>>> */
#define KRI_SET_FLG_MLNKMSG(hMsg)	{ \
					*(UINT *)&((hMsg)->f) = (UINT)0; \
					(hMsg)->f.ComMsgIF = 1; \
					(hMsg)->f.MlnkMsgIF = 1; \
				}
/* <<<<< <S150> */


/*--------------------------------------------------------------------------------------------------*/
/*
 *  Message Check Macro
 *
 *  KPI_CHK_OPEMSG() : LCD Operator
 *  KPI_CHK_SYSDLMSG() : Download Message
 *  KPI_CHK_M3MSG() : Mechatrolink Message
 *
 */
/*--------------------------------------------------------------------------------------------------*/
#define	KPI_CHK_OPEMSG( fc ) ((0x60 <= fc) && (fc <= 0x64))		/* オペレータメッセージチェック		*/
#define	KPI_CHK_SYSDLMSG( fc ) (0x50 == fc)						/* ダウンロードメッセージチェック	*/
#define	KPI_CHK_M3MSG( fc ) (0x42 == fc)						/* M3メッセージチェック				*/

/*--------------------------------------------------------------------------------------------------*/
/*		Serial Modes (for SerComMode)																*/
/*--------------------------------------------------------------------------------------------------*/
#define SCM_NOCOM		0							/* 接続機器なし									*/
#define SCM_LCDCOM		1							/* LCDオペレータ								*/
#define SCM_MSGCOM		2							/* Message機器(PC,etc)							*/
#define SCM_TSTCOM		3							/* Tester機器(Debug)							*/

/*--------------------------------------------------------------------------------------------------*/
/*		アクセスレベル(注意：0 は使用不可)															*/
/*--------------------------------------------------------------------------------------------------*/
#define ACCLVL_USER1			1				/* ユーザー１										*/
#define ACCLVL_USER2			2				/* ユーザー２										*/
#define ACCLVL_USER3			3				/* ユーザー３										*//* <S0FE> */
//#define ACCLVL_SYSTEM			3				/* システム											*//* <S0FE> */
#define ACCLVL_SYSTEM			4				/* システム											*//* <S0FE> *//*--------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------*/
/*
 * Function Message Definition 
 * 
 * Lcd operator and panel operator use this for executing  function.
 *
 */
/*--------------------------------------------------------------------------------------------------*/
typedef struct _FUNMSG {
	MSGIF	*Super;							/* @@ Super Class (Message Interface) */
/*--------------------------------------------------------------------------------------------------*/
	UINT 	*pKeyMk;						/* Pointer to the Key Make (OFF->ON) Flag */
	UINT 	*pKeyBrk;						/* Pointer to the Key Break(ON->OFF) Flag */
	UINT 	*pKeyLvl;						/* Pointer to the Key Level Flag */
	UINT	*pBlnkKick;						/* Pointer to the Blink Flag */
	UCHAR	*pTxBuf;						/* Pointer to the Display Inforamtion Buffer */
/*--------------------------------------------------------------------------------------------------*/
	void	*pVar;							/* Pointer to the Function common variable area */
} FUNMSG;


/*--------------------------------------------------------------------------------------------------*/
/*
 * Panel Operator Message Definition
 *
 * Command :
 * 		includes panel button information. (RinK.Pnl)
 * 
 * Response :
 * 		includes panel led display code information. (RoutK.PnlLed)
 *
 */
/*--------------------------------------------------------------------------------------------------*/
#define PNL_N_LED  (5)
#define PNLCMD_PNLBTN_IDX (0)
typedef struct _PnlCmdMsg {
//	UCHAR Magic[2];						/* Magic Number { 'P', 'O' } */
	UCHAR PnlBtn;						/* Panel Operator Input Button Switch Code */
} PNLCMDMSG;

#define PNLRSP_LEDCODE_IDX (0)
typedef struct _PnlRspMsg {
//	UCHAR Magic[2];						/* Magic Number { 'P', 'O' } */
	/*----------------------------------------------------------------------------------------------*/
	/*
	 * LEDDSP;
	 *	UCHAR	LedCode[5]			:	表示するコード	
	 *	UCHAR	LedBlnkSw[5]		:	1の部分がブリンク対象
	 */
	/*----------------------------------------------------------------------------------------------*/
	UCHAR LedCode[PNL_N_LED];			/* Panel Operator Output Led Code */
	UCHAR LedBlnkSw[PNL_N_LED];			/* Panel Operator Blink Switch */
} PNLRSPMSG;



#endif /* MSFGIF_H_ */
/***************************************** end of file **********************************************/
