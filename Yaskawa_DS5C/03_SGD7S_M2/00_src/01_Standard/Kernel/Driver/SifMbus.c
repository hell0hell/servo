/****************************************************************************************************/
/*																									*/
/*																									*/
/*		SifMbus.c : シリアル通信インターフェイス													*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : シリアル通信の送受信を行う。															*/
/*																									*/
/*			1) シリアル通信指令メッセージを受信し、受信完了通知を行う。								*/
/*			2) シリアル通信応答メッセージの送信開始要求を受け、送信を開始する。						*/
/*			3) 送受信監視シーケンスにて、シリアル通信データの受信と送信の制御を行う。				*/
/*			4) メッセージのヘッダ解析は、MsgMngr.cにて行うように変更。								*/
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
/*																									*/
/*		Rev.2.xx : 2013.4.23	K.Ozaki																*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include	"KnlApi.h"
#include	"MsgIfDef.h"
#include	"UartMsgIF.h" /* Zantei */
#include 	"FirmDownload.h"		/* <S041> */

#define SIF_DEBUG (0)		/* 1:Debug Mode */

/****************************************************************************************************/
/*																									*/
/*		関数一覧																					*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Ｋｐｘ関数定義：関数名変更不可																*/
/*--------------------------------------------------------------------------------------------------*/
void	KpxSifMsgManager( void );			/* シリアル通信メッセージ送受信監視						*/
void	KpxInitSifMsgManager( 				/* シリアル通信メッセージ管理初期化						*/
				LONG ch,					/* UART channel number (0,1,2)							*/
				CSIFCOMPORT *Com,			/* UART Com port 										*/
				LONG Axis);					/* Axis Number											*/
//<S150>				void *pUartMbusServer);		/* Zantei USB同様後で消す								*/
extern LONG RkiMsgifGetCmdMsgLength( UCHAR *CmdBuf, LONG CmdBufLen );	/* <S150> */

/*--------------------------------------------------------------------------------------------------*/
/*		Ｌｐｘ関数定義：関数名変更自由																*/
/*--------------------------------------------------------------------------------------------------*/
void	LpxSifStartRxDMA( CSIF *this );		/* ＳＩＦ受信ＤＭＡスタート処理				*/
void	LpxSifStopxRxDMA( CSIF *this );		/* ＳＩＦ受信ＤＭＡストップ処理							*/
void	LpxSifStartTxDMA( CSIF *this );		/* ＳＩＦ送信ＤＭＡスタート処理							*/
/*--------------------------------------------------------------------------------------------------*/
//LONG	LpxSifChkRxHeader( CSIF *this );	/* Check receive message header							*/
//LONG	LpxSifChkRxMsgEnd( CSIF *this );	/* Check receive message end							*/
LONG	LpxSifMbusChkMyMsg( CSIF *this );	/* Check receive message address						*/
#if 0	/* RkiMsgifGetCmdMsgLengthを使用する <S150> */
LONG	LpxSifGetCmdMsgLength(				/* Check recieve message length							*/
				UCHAR *CmdBuf,
				LONG CmdBufLen );
#endif
/*--------------------------------------------------------------------------------------------------*/
//LONG	LpxSifCheckRxDMA( CSIF *this );		/* ＤＭＡ受信確認										*//* @@DMA不使用 */
LONG	LpxSifCheckRxData( CSIF *this,
						   LONG mode );		/* UART受信確認											*/
LONG	LpxSifSetTxData( CSIF *this );		/* UART送信処理											*/

/*--------------------------------------------------------------------------------------------------*/
/* 		Serial interface function																	*/
/*		(引数、戻り値変更不可																		*/
/*--------------------------------------------------------------------------------------------------*/
void	LpiSifMsgManager( CSIF *this );		/* シリアル通信メッセージ送受信監視						*/
/*--------------------------------------------------------------------------------------------------*/
LONG	LpiCheckRxSerialMsg( 				/* シリアル通信指令メッセージ受信チェック				*/
		CSIF *this,							/* serial I/F class pointer								*/
		USHORT *pCmdMsgLen );				/* 指令メッセージ長格納先ポインタ						*/
/*--------------------------------------------------------------------------------------------------*/
LONG	LpiStartTxSerialMsg(				/* シリアル通信応答メッセージ送信開始指令				*/
		CSIF	*this,						/* serial I/F class pointer								*/
		UCHAR*	TxBuf,						/* 送信データバッファ									*/
		LONG	TxSize		);				/* 送信データサイズ(ByteSize,無応答時はゼロ)			*/
/*--------------------------------------------------------------------------------------------------*/
LONG	LpiStartTxSerialMsgNoResponse(		/* シリアル通信応答メッセージ送信開始指令(無応答用)		*/
		CSIF	*this,						/* serial I/F class pointer								*/
		UCHAR*	TxBuf,						/* 送信データバッファ									*/
		LONG	TxSize		);				/* 送信データサイズ(ByteSize,無応答時はゼロ)			*/
/*--------------------------------------------------------------------------------------------------*/

/****************************************************************************************************/
/*																									*/
/*		変数定義																					*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
CSIF	LSif[SIF_N_CH];						/* Serial Communication Management IF Class				*/
/*--------------------------------------------------------------------------------------------------*/
/* <S150> >>>>> */
typedef	struct {							/*														*/
	UCHAR	RxBuf[MSG_MAXBUFSZB];			/* 受信データバッファ									*/
	UCHAR	TxBuf[MSG_MAXBUFSZB];			/* 送信データバッファ									*/
} MSIFV;
/*--------------------------------------------------------------------------------------------------*/
MSIFV	MSifV[SIF_N_CH];					/* Local Variables for Serial Message				*/
/* <<<<< <S150> */
/****************************************************************************************************/
/*																									*/
/*		Constant Definition																			*/
/*																									*/
/****************************************************************************************************/
#undef RESMT_COMPLETE
#define	RESMT_COMPLETE		(0x01)			/* MsgType : 完了応答									*/
#undef RESMT_NORESPONSE
#define	RESMT_NORESPONSE	(0x00)			/* MsgType : 無応答(メッセージ異常)						*/
/*--------------------------------------------------------------------------------------------------*/
#define	SIFREQ_STARTTX		(0x01)			/* 送信開始指令											*/
#define	SIFREQ_NOANSWER		(0x02)			/* 無応答指令											*/
/*--------------------------------------------------------------------------------------------------*/
#define	MSG_ERROR			(0x00)			/* メッセージ異常				for MEMOBUS				*/
#define	LENGTH_FIX			(0x01)			/* 受信メッセージ長確定			for MEMOBUS				*/
#define	LENGTH_NOFIX		(0x02)			/* 受信メッセージ長未確定		for MEMOBUS				*/

#define	DIGOP_MINSIZE		(6)				/* デジオペメッセージ最小バイト数						*/
#define	DIGOP_MAXSIZE		(256)			/* デジオペメッセージ最大バイト数						*/
#define	MBUS_MINSIZE		(8)				/* MEMOBUSメッセージ最小バイト数						*/
#define	MBUS_MAXSIZE		(256)			/* MEMOBUSメッセージ最大バイト数						*/
/*--------------------------------------------------------------------------------------------------*/
#define	SIF_GETHEADER		(0)				/* UARTデータ受信 ヘッダー部取得モード(先頭2Byte以上)	*/
#define	SIF_GETALLDATA		(1)				/* UARTデータ受信 全データ取得モード					*/
/*--------------------------------------------------------------------------------------------------*/
#define	SIF_RX_SUCCESS		(1)				/* UARTメッセージ受信成功								*/
#define	SIF_RX_CONTINUE		(0)				/* UARTメッセージ受信継続								*/
#define	SIF_RX_ERROREND		(-1)			/* UARTメッセージ異常終了								*/
/*--------------------------------------------------------------------------------------------------*/
const LONG DefBaudRate[8] = {               /* Serial Communication Baudrate Definition             */
	0x00002580,     /*   9600 bps */
	0x00004b00,     /*  19200 bps */
	0x00009600,     /*  38400 bps */
	0x0000E100,     /*  57600 bps(reserved) */
	0x0001C200,     /* 115200 bps(reserved) */
	0x0001F400,     /* 128000 bps(reserved) */
	0x0003D090,     /* 250000 bps(reserved) */
	0x000F4240,     /*1000000 bps(reserved) */
};
#define LPX_CHK_BRTCHG( com, bidx )          /* check baurate parameter */                           \
		( DefBaudRate[ (bidx) ] != ((com)->BRT) )  
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		ＳＩＦエラー定義																			*/
/*																									*/
/****************************************************************************************************/
#define SET_SIFERR( csif, code )	{ (csif)->ErrCod = code; ++((csif)->ErrCnt);}
/*--------------------------------------------------------------------------------------------------*/
/*		Error Code Definition																		*/
/*--------------------------------------------------------------------------------------------------*/
#define	SIFERR_RXTMOVR		0x01			/* 伝文内受信タイムオーバ								*/
#define SIFERR_RXDMAERR 	0x02			/* 受信DMAエラー										*/
#define	SIFERR_RXHDRERR		0x03			/* 受信ヘッダエラー										*/
/*--------------------------------------------------------------------------------------------------*/
#define	SIFERR_USERERR		0x10			/* ユーザエラー(無応答)									*/
#define	SIFERR_OTHERERR		0xFF			/* その他エラー											*/
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/*		Timer Value Definition																		*/
/*--------------------------------------------------------------------------------------------------*/
#define SIF_TXWAITTIME		  20			/* 受信完了からの送信開始待ち時間 [ms]					*/
#define	SIF_RXTIMEOVER		 100			/* 伝文内受信タイムオーバ設定値   [ms]					*/
/*--------------------------------------------------------------------------------------------------*/

/****************************************************************************************************/
/*																									*/
/*		ＳＩＦ＆ＤＭＡインターフェイスマクロ定義													*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		DMA Receive Size Definition																	*/
/*--------------------------------------------------------------------------------------------------*/
#define	DMA_RXSIZE			(SIF_RX_BUFSIZE - 1)  		/* DMA Receive RxBuf  Size					*/
/*--------------------------------------------------------------------------------------------------*/
#define LPX_GET_RXDMA_CNT( Com ) ( *(Com->RxDMA_CNT) )
/*--------------------------------------------------------------------------------------------------*/
#define LPX_V850_BUG1( ) {;} 
/*--------------------------------------------------------------------------------------------------*/
#define LPX_TXCHKF(Com)  (((*((Com)->TXCHKF))&0x040))	/* Check TX FIFO EMPTY						*/
#define LPX_TXCHKL(Com)  (((*((Com)->TXCHKF))&0x020))	/* Check TX LATCH EMPTY						*/
#define LPX_CHK_RXRDY( com ) ((*((com)->RXCHKF)) & 0x01)

/****************************************************************************************************/
/*																									*/
/*		シリアル通信メッセージ初期化処理															*/
/*																									*/
/****************************************************************************************************/
void	KpxInitSifMsgManager( LONG ch, CSIFCOMPORT *Com, LONG Axis
//<S150>			,void *pUartMbusServer				/* zantei */
		)
{
CSIF *pSif = &(LSif[ch]);										/* get serial IF pointer */
/*--------------------------------------------------------------------------------------------------*/
/*  DMA停止(念のため)																				*/
/*--------------------------------------------------------------------------------------------------*/
	Com->RXDISABLE( );											/* Disable SIF Receive				*/
	Com->STOP_RxDMA( );											/* Stop SIF RxDMA					*/
/*--------------------------------------------------------------------------------------------------*/
	MlibResetLongMemory( pSif, sizeof( CSIF )/4 );				/* reset data	*/
/*--------------------------------------------------------------------------------------------------*/
	pSif->Com = Com; 											/* set com port */
	pSif->AxisNumber = Axis;									/* set axis number */
	pSif->DoRxTx = LpiSifMsgManager;							/* RxTx sequence */
	pSif->ChkCmd = LpiCheckRxSerialMsg;							/* check receive cmd method */
	pSif->StxRes = LpiStartTxSerialMsg;							/* start tx response method */
/*--------------------------------------------------------------------------------------------------*/
	pSif->RcvBufSize = SIF_RX_BUFSIZE;							/* rcv buf size */
	pSif->SndBufSize = SIF_TX_BUFSIZE;							/* send buf size */
/*--------------------------------------------------------------------------------------------------*/
//	pSif->pBrtPrm = &(Iprm.SifCfg[ ch ].BaudRateSel);           /* TODO:set baudrate parameter pointer   */
/*--------------------------------------------------------------------------------------------------*/

	/*----------------------------------------------------------------------------------------------*/
	/*	以下暫定SigmaWin+対応 正式に42Hコマンドに対応すること										*/
	/*----------------------------------------------------------------------------------------------*/
/* <S150> >>>>> */
//	pSif->ComMbus42 = pUartMbusServer;							/* TODO: Zantei(USB同様後で消す)	*/
////	((UART_MBUS_SERVER*)pUartMbusServer)->MbusIf.RxBuf = pSif->RxBuf;	/* <S01C> */
////	((UART_MBUS_SERVER*)pUartMbusServer)->MbusIf.TxBuf = pSif->TxBuf;	/* <S01C> */
//	pSif->RxBuf = ((UART_MBUS_SERVER*)pUartMbusServer)->MbusIf.RxBuf;	/* <S01C> */
//	pSif->TxBuf = ((UART_MBUS_SERVER*)pUartMbusServer)->MbusIf.TxBuf;	/* <S01C> */
	pSif->RxBuf = MSifV[ch].RxBuf;								/* rcv buf */
	pSif->TxBuf = MSifV[ch].TxBuf;								/* send buf */
/* <<<<< <S150> */

	return;
}

/****************************************************************************************************/
/*																									*/
/*		シリアル通信メッセージ定周期監視処理 ( ScanC Cycle )										*/
/*																									*/
/****************************************************************************************************/
void	KpxSifMsgManager( void )
{
	if( UART0_PORT_USE == 1) { LSif[COM0_UART].DoRxTx( &(LSif[COM0_UART]) ); /* UART ch:0 */ }
	if( UART1_PORT_USE == 1) { LSif[COM1_UART].DoRxTx( &(LSif[COM1_UART]) ); /* UART ch:1 */ }
	return;
}


/****************************************************************************************************/
/*																									*/
/*		シリアル通信メッセージ送受信監視シーケンス													*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 送受信監視シーケンスにより、シリアル通信データの受信と送信の制御を行う。				*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*			0) 初期STEP		: 変数のリセットを行い、受信開始処理STEPに遷移する。					*/
/*			1) 受信開始処理 : ＳＩＦ受信ＤＭＡをスタートし、受信待ちSTEPに遷移する。				*/
/*			2) 受信待ち		: ＳＩＦ受信ＤＭＡを監視し、1byte以上受信した場合は、					*/
/*							  1byte受信STEPに遷移する。												*/
/*			3) 1byte受信    : ＳＩＦ受信ＤＭＡを監視し、4byte以上受信した場合は、					*/
/*							  4byte受信STEPに遷移する。												*/
/*							  本STEPから、伝文内受信タイムオーバを監視する。						*/
/*			4) 4byte受信    : ＳＩＦ受信ＤＭＡを監視し、全メッセージを受信した場合は				*/
/*							  (ヘッダ部のメッセージ長で判断)、受信完了処理STEPに遷移する。			*/
/*							  本STEPまで、伝文内受信タイムオーバを監視する。						*/
/*			5) 受信完了処理	: ＳＩＦ受信ＤＭＡをストップする。										*/
/*							  継続伝送モードであれば、継続伝送処理STEPに遷移する。					*/
/*							  通常伝送モードであれば、受信メッセージのチェックを行い、				*/
/*							  自局宛てのメッセージであれば、受信完了ステータスをセットし、			*/
/*							  送信開始待ちSTEPに遷移する。											*/
/*							  自局宛てのメッセージでなければ、受信開始処理STEPに遷移する。			*/
/*			6) 送信開始待ち	: アプリからの送信開始要求により送信開始処理STEPに遷移する。			*/
/*							  なお、アプリからの送信開始要求は本STEPにいる場合のみ受け付ける。		*/
/*							  継続伝送モード移行チェック時間内に、アプリからの送信開始要求			*/
/*							  がない場合は、継続伝送モードとし、継続応答メッセージを作成し、		*/
/*							  送信開始処理STEPに遷移する。											*/
/*			7) 送信開始処理 : ＳＩＦ送信ＤＭＡとＳＩＦ受信ＤＭＡをスタートし、						*/
/*							  送信完了待ちSTEPに遷移する。											*/
/*			8) 送信完了待ち : ＳＩＦ送信ＤＭＡの送信完了を監視し、ＤＭＡ送信完了で、				*/
/*							  送信完了確認STEPに遷移する。											*/
/*			9) 送信完了確認	: ＳＩＦの送信完了を監視し、ＳＩＦ送信完了で、送信完了処理STEP 			*/
/*							  に遷移する。															*/
/*		   10) 送信完了処理	: ＳＩＦを送信Disableとし、受信待ちSTEPに遷移する。						*/
/*																									*/
/*			*) 継続伝送処理 : 継続伝送モード時の完了確認メッセージに対する応答処理を行う。			*/
/*							  なお、応答メッセージ作成後、送信開始処理STEPに遷移する。				*/
/*																									*/
/*			*) 各種異常処理 : 受信タイムオーバ等の異常処理を行う。									*/
/*							  異常応答を返信する場合は、送信開始処理STEPに遷移する。				*/
/*							  異常応答を返信しない場合(SumCheckErrなど)は、初期STEPに戻る。			*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
enum	SQSTEP	{
	SQSTEP_INITIAL = 0,
	SQSTEP_STARTRX,
	SQSTEP_WAITRX,
	SQSTEP_CHKHEADER,
	SQSTEP_RXCMPLT,
	SQSTEP_WAITSTX,
	SQSTEP_STARTTX,
	SQSTEP_WAITETX,
	SQSTEP_CHKASIF,
	SQSTEP_TXCMPLT,
/*--------------------------------------------------------------------------------------------------*/
	SQSTEP_SIFRXERR,
};
/*--------------------------------------------------------------------------------------------------*/
void	LpiSifMsgManager( CSIF *this )
{
USHORT	timer;
LONG	rc;
ULONG	UartSpeed;				/*+	UART転送速度(bps)	　<S041> */
LONG	DataAddr;				/*+	送信データアドレス <S041>	*/
UCHAR	*pTxBuf = this->pTxRes;	/*+	送信データポインター先頭 <S041>	*/
register CSIFCOMPORT *Com = (CSIFCOMPORT *)this->Com;
/*--------------------------------------------------------------------------------------------------*/
/*		シリアル通信送受信監視シーケンス 															*/
/*--------------------------------------------------------------------------------------------------*/
		switch( this->SqStep )
		{
/*--------------------------------------------------------------------------------------------------*/
		case SQSTEP_INITIAL: 	/* Initial Step														*/
/*--------------------------------------------------------------------------------------------------*/
			Com->RSTRXSTS( );										/* reset recv. status			*/
//TODO		Com->SETBRT( Com, DefBaudRate[ *(this->pBrtPrm) ] );	/* set baudrate					*/
			Com->RESET( );											/* reset serial					*/
/*--------------------------------------------------------------------------------------------------*/
			this->ReqStx = 0x00;									/* reset Tx request				*/
			this->SqStep = SQSTEP_STARTRX;							/* start Rx						*/
/*--------------------------------------------------------------------------------------------------*/
			break;
/*--------------------------------------------------------------------------------------------------*/
		case SQSTEP_STARTRX: 	/* Start Receive Message											*/
/*--------------------------------------------------------------------------------------------------*/
//			LpxSifStartRxDMA( this );								/* 受信ＤＭＡスタート			*//* @@DMA不使用 */
			Com->RXENABLE( );										/* Enable SIF Receive			*/
			this->SqStep = SQSTEP_WAITRX;							/* 受信待ちSTEPに遷移			*/
			this->RxCmdCnt = 0;										/* 受信データ数クリア			*/
			break;
/*--------------------------------------------------------------------------------------------------*/
		case SQSTEP_WAITRX:		/* Wait Receive Message												*/
/*--------------------------------------------------------------------------------------------------*/
			if( Com->CHK_RXERR() )                  				/* Check Serial Com. Error		*/
			{ /* OverRun, Parity, Framing Error */
				Com->RXDISABLE( );									/* Disable SIF Receive			*/
//				Com->STOP_RxDMA( );									/* Stop SIF RxDMA				*//* @@DMA不使用 */
				this->SqStep = SQSTEP_INITIAL;						/* Goto Initial Step			*/
				break;
			}
			else
			{ /* Communication Error is Notiong */
				rc = LpxSifCheckRxData( this, SIF_GETHEADER );		/* Check Receive data			*/
				if( rc == SIF_RX_SUCCESS )
				{ /* Get Header Data */
					this->SqStep = LpxSifMbusChkMyMsg( this );	/* TODO: 受信メッセージのチェック処理	*/
//					this->SqStep = SQSTEP_CHKHEADER;				/* Check the Header Data		*/
//					this->RxCmdCnt = rc;							/* Set Rx Count					*/
//					this->SqStep = SQSTEP_RXCMPLT;					/* Receive Complete				*/
				}
				else if( rc == SIF_RX_ERROREND )
				{ /* Error End */
					Com->RXDISABLE( );								/* Disable SIF Receive			*/
					this->SqStep = SQSTEP_INITIAL;					/* Goto Initial Step			*/
				}
				else /* SIF_RX_CONTINUE */
				{
//					if(	SysDl.DlStatus == 0x0001 )	/* <S041> */
					if(	SYSDL_CHK_DLMODE_ON() )	/* <S077> */
					{	/* Bps Time Out Check 10sec	*/
						if( KpiGetLongTimer( &(SysDl.BpsChkTime) ) > 10000 )
						{	/*	ダウンロード開始状態から10秒間次へ進まない場合19200bpsへ戻す	*/
							if(SysDl.NowBps != 19200)
							{	/* 現在のＢＰＳ値が19200bps以外のみ */
								KpxInitCpuSerialIF( COM0_UART, 19200, 8, 2, 1);
							}
							KpiRstLongTimer( &(SysDl.BpsChkTime) );	/* Reset BpsChkTimer			*/
							SYSDL_DLMODE_RESET();					/* DownLoad mode stop <S077>	*/
						}
					}
					; /* CONTINUE */
				}
			}
			break;
#if 0
/*--------------------------------------------------------------------------------------------------*/
		case SQSTEP_CHKHEADER:	/* receive complete													*/
/*--------------------------------------------------------------------------------------------------*/
			this->SqStep = LpxSifMbusChkMyMsg( this );	/* TODO: 受信メッセージのチェック処理		*/
			break;
#endif
/*--------------------------------------------------------------------------------------------------*/
		case SQSTEP_RXCMPLT:	/* receive complete													*/
/*--------------------------------------------------------------------------------------------------*/
			if( Com->CHK_RXERR() )                  				/* Check Serial Com. Error		*/
			{ /* OverRun, Parity, Framing Error */
				Com->RXDISABLE( );									/* Disable SIF Receive			*/
//				Com->STOP_RxDMA( );									/* Stop SIF RxDMA				*//* @@DMA不使用 */
				this->SqStep = SQSTEP_INITIAL;						/* Goto Initial Step			*/
//				break;
			}
			else
			{ /* Communication Error is Notiong */
				rc = LpxSifCheckRxData( this, SIF_GETALLDATA );		/* Check Receive data			*/
				if( rc == SIF_RX_SUCCESS )
				{ /* Get Header Data */
//					LpxSifStopxRxDMA( this );						/* Stop RxDMA					*/
					Com->RXDISABLE( );								/* Disable SIF Receive			*/
					KpiRstLongTimer( &(this->StxTimer) );			/* Reset TxTimer				*//* @@DMA不使用 */
//					this->SqStep = LpxSifMbusChkMyMsg( this );/* TODO: 受信メッセージのチェック処理	*/
					this->SqStep = SQSTEP_WAITSTX;					/* Check the Header Data		*/
				}
				else if( rc == SIF_RX_ERROREND )
				{ /* Error End */
					Com->RXDISABLE( );								/* Disable SIF Receive			*/
					this->SqStep = SQSTEP_INITIAL;					/* Goto Initial Step			*/
				}
				else /* SIF_RX_CONTINUE */
				{
					; /* CONTINUE */
				}
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case SQSTEP_WAITSTX:	/* 送信開始待ち 													*/
/*--------------------------------------------------------------------------------------------------*/
			if( this->ReqStx == SIFREQ_STARTTX )					/* 送信開始要求チェック			*/
			{
				this->ReqStx  = 0x00;								/* Reset ReqStx					*/
				this->SqStep  = SQSTEP_STARTTX;						/* 送信開始処理STEPに遷移		*/
			}
			else if( this->ReqStx == SIFREQ_NOANSWER )				/* 無応答(MsgErr)要求チェック	*/
			{
				this->ReqStx  = 0x00;								/* Reset ReqStx					*/
				this->SqStep  = SQSTEP_INITIAL;						/* 初期ステップに戻る			*/
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case SQSTEP_STARTTX:	/* 送信開始処理														*/
/*--------------------------------------------------------------------------------------------------*/
			timer = KpiGetLongTimer( &(this->StxTimer) );	/* すぐに応答Messageを返信すると、		*/
			if( timer >= SIF_TXWAITTIME )					/* 相手が受信できない場合があるので、	*/
			{												/* ここで待時間を調整する				*/
#if TX_DMA_USE == 1
				LpxSifStartTxDMA( this );					/* 送信ＤＭＡスタート					*//* @@DMA不使用 */
#else
				Com->TXENABLE( );							/* SIF Transmit Enable					*/
				this->TxResCnt = this->TxResLen;			/* Get Data Size						*/
#endif
				this->SqStep = SQSTEP_WAITETX;
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case SQSTEP_WAITETX:	/* 送信データ設定													*/
/*--------------------------------------------------------------------------------------------------*/
#if TX_DMA_USE == 1
			if( Com->CHK_TxDMA_TC( ) )								/* ＤＭＡ送信完了チェック		*//* @@DMA不使用 */
#else
			if( TRUE == LpxSifSetTxData(this) )						/* UART送信処理					*/
#endif
			{ /* When All Data Tx Complete */
				this->SqStep = SQSTEP_CHKASIF;						/* 送信完了確認STEPに遷移		*/
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case SQSTEP_CHKASIF:	/* 送信完了確認														*/
/*--------------------------------------------------------------------------------------------------*/
			if( LPX_TXCHKF( Com ) )				    /* ＳＩＦ送信完了チェック					*/
			{/*シリアル速度変更時変更完了確認<S041>*/
				DataAddr = 0;	/*	送信データ先頭	*/
				/*　システムダウンロード時のみ　*/
				if(pTxBuf[DataAddr+1] == 0x50)
				{
					/*　シリアルＩＦ速度変更指令の応答時のみ　*/
					if((pTxBuf[DataAddr+2]==0x00) && (pTxBuf[DataAddr+3]==0x0D))
					{
						/*　切替転送速度データ作成　*/
						UartSpeed = pTxBuf[DataAddr+9];
						UartSpeed += (pTxBuf[DataAddr+8] * 0x100);
						UartSpeed += (pTxBuf[DataAddr+7] * 0x10000);
						UartSpeed += (pTxBuf[DataAddr+6] * 0x1000000);
					/*	転送速度切替の実行	*/
					KpxInitCpuSerialIF( COM0_UART, UartSpeed, 8, 2, 1);	/*	指定転送速度で	*/
					}
				}
				this->SqStep = SQSTEP_TXCMPLT;		/* 送信完了処理STEPに遷移					*/
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case SQSTEP_TXCMPLT:	/* 送信完了処理 													*/
/*--------------------------------------------------------------------------------------------------*/
			Com->STOP_TxDMA( );							/* stop DMA 								*/
			Com->TXDISABLE( );							/* Transmit Disable							*/
			LPX_V850_BUG1( );							/* V850E2/ME3：TxDMAの誤動作対策			*/
/*--------------------------------------------------------------------------------------------------*/
//			if ( LPX_CHK_BRTCHG( Com, *(this->pBrtPrm) ) )             /* Check baudrate chg  */
//			{
//				Com->RXDISABLE( );					                 /* Disable SIF Receive	*/
//				Com->STOP_RxDMA( );					                 /* Stop SIF RxDMA		*/
//				this->SqStep = SQSTEP_INITIAL;				                 /* Initial step		*/
//			}
/*--------------------------------------------------------------------------------------------------*/
//			else 
			{
				//this->SqStep = SQSTEP_WAITRX;			/* 受信待ちSTEPに遷移						*/
				this->SqStep = SQSTEP_INITIAL;			/* Goto Initial Step					*/
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case SQSTEP_SIFRXERR:	/* 受信異常 														*/
/*--------------------------------------------------------------------------------------------------*/
//			LpxSifStopxRxDMA( this );					/* Stop RxDMA								*//* @@DMA不使用 */
			Com->RXDISABLE( );							/* Disable SIF Receive						*/
			this->SqStep = SQSTEP_INITIAL;				/* Goto Initial Step						*/
			break;
/*--------------------------------------------------------------------------------------------------*/
		default:
/*--------------------------------------------------------------------------------------------------*/
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
	return;
}



/****************************************************************************************************/
/*																									*/
/*		MEMOBUSメッセージ受信完了確認ＡＰＩ関数サービス（Round用）									*/
/*																									*/
/*			戻り値：MEMOBUSメッセージを正常に受信した場合	: TRUE									*/
/*					未受信の場合							: FALSE									*/
/*																									*/
/*			呼び出し側は、この関数の戻り値がTRUEであることを確認して受信バッファを参照してください	*/
/*			マネージャは、次にDpxSetSifMbusTrnsStart()によって送信開始指令を受けるまで受信バッファ	*/
/*			にはアクセスしません。																	*/
/****************************************************************************************************/
LONG	LpiCheckRxSerialMsg( CSIF *this, USHORT *pCmdMsgLen )
{
/*--------------------------------------------------------------------------------------------------*/
/*		受信完了時																					*/
/*--------------------------------------------------------------------------------------------------*/
		if( this->SqStep == SQSTEP_WAITSTX )
		{
			*pCmdMsgLen = this->RxCmdCnt;
			//*pCmdMsgLen = this->RxCmdLen;
			return( TRUE );
		}
/*--------------------------------------------------------------------------------------------------*/
/*		受信未完時																					*/
/*--------------------------------------------------------------------------------------------------*/
		return( FALSE );
}


/****************************************************************************************************/
/*																									*/
/*		MEMOBUSメッセージ送信開始要求ＡＰＩ関数サービス（Round用）									*/
/*																									*/
/*			戻り値：																				*/
/*						(1)正常終了																	*/
/*						(0)シーケンス異常															*/
/*																									*/
/****************************************************************************************************/
LONG	LpiStartTxSerialMsg(				/* シリアル通信応答メッセージ送信開始指令				*/
		CSIF	*this,						/* serial I/F class pointer								*/
		UCHAR*	TxBuf,						/* 送信データバッファ									*/
		LONG	TxSize		)				/* 送信データサイズ(ByteSize,無応答時はゼロ)			*/
{
/*--------------------------------------------------------------------------------------------------*/
/*		応答メッセージタイプのチェック																*/
/*--------------------------------------------------------------------------------------------------*/
		if( this->SqStep != SQSTEP_WAITSTX )		/* シーケンスチェック							*/
		{
			return( FALSE );						/* 異常リターン									*/
		}
		else if( TxSize > 0 )						/* 送信データサイズチェック						*/
		{
			this->pTxRes   = TxBuf;					/* 応答バッファアドレス設定						*/
			this->TxResLen = (USHORT)TxSize;		/* 応答メッセージ長(Byte数)設定					*/
			this->ReqStx   = REQSTX_STARTTX;		/* 送信開始リクエスト設定						*/
			return( TRUE );
		}
		else
		{
			this->pTxRes   = NULL;					/* 応答バッファアドレス設定						*/
			this->TxResLen = 0;						/* 応答メッセージ長(Byte数)設定					*/
			this->ReqStx   = REQSTX_NORESPONSE;		/* 無応答リクエスト設定							*/
			return( TRUE );
		}
/*--------------------------------------------------------------------------------------------------*/
//		return( FALSE );							/* 異常リターン									*/
}


/****************************************************************************************************/
/*																									*/
/*		MEMOBUSメッセージ送信開始要求ＡＰＩ関数サービス（強制無応答）								*/
/*																									*/
/*			戻り値：																				*/
/*						(1)正常終了																	*/
/*						(0)シーケンス異常															*/
/*																									*/
/****************************************************************************************************/
LONG	LpiStartTxSerialMsgNoResponse(		/* シリアル通信応答メッセージ送信開始指令(無応答用)		*/
		CSIF	*this,						/* serial I/F class pointer								*/
		UCHAR*	TxBuf,						/* 送信データバッファ									*/
		LONG	TxSize		)				/* 送信データサイズ(ByteSize,無応答時はゼロ)			*/
{
	return LpiStartTxSerialMsg( this, NULL, 0 );
}



/****************************************************************************************************/
/*																									*/
/*		ＳＩＦ受信ＤＭＡスタート処理																*/
/*																									*/
/****************************************************************************************************/
void	LpxSifStartRxDMA( CSIF *this )
{
CSIFCOMPORT *pCom = (CSIFCOMPORT *)this->Com;						/* get com port 				*/
/*--------------------------------------------------------------------------------------------------*/
/*		Set RxDMA 																					*/
/*--------------------------------------------------------------------------------------------------*/
		this->RxDmaScnt = DMA_RXSIZE;
		pCom->SETX_RxDMA( (UINT)(pCom->RXBUF), (UINT)(this->RxBuf), this->RxDmaScnt ); 
/*--------------------------------------------------------------------------------------------------*/
/*		ＤＭＡ＆ＳＩＦスタート処理																	*/
/*--------------------------------------------------------------------------------------------------*/
		pCom->START_RxDMA( );							/* Enable RxDMA								*/
		pCom->RXENABLE( );								/* Enable SIF Receive						*/
/*--------------------------------------------------------------------------------------------------*/
		return;

}


/****************************************************************************************************/
/*																									*/
/*		ＳＩＦ受信ＤＭＡストップ処理																*/
/*																									*/
/****************************************************************************************************/
void	LpxSifStopxRxDMA( CSIF *this )
{
CSIFCOMPORT *Com = (CSIFCOMPORT *)this->Com;
/*--------------------------------------------------------------------------------------------------*/
/*		ＳＩＦストップ処理																			*/
/*--------------------------------------------------------------------------------------------------*/
		Com->RXDISABLE( );								/* Disable SIF Receive						*/
		Com->STOP_RxDMA( );								/* Stop SIF RxDMA							*/
/*--------------------------------------------------------------------------------------------------*/
		return;

}


/****************************************************************************************************/
/*																									*/
/*		ＳＩＦ送信ＤＭＡスタート処理																*/
/*																									*/
/****************************************************************************************************/
void	LpxSifStartTxDMA( CSIF *this )
{
CSIFCOMPORT *Com = this->Com;
/*--------------------------------------------------------------------------------------------------*/
/*		ＤＭＡ転送開始																				*/
/*--------------------------------------------------------------------------------------------------*/
		Com->TXENABLE( );								/* SIF Transmit Enable						*/
		KlibWaitus( 1 );								/* Wait	1 us								*/
		Com->SETX_TxDMA( ((UINT)(this->pTxRes)),      	/* source address 							*/
						(UINT)(Com->TXBUF), 		    /* destination address						*/
						(this->TxResLen) ); 		    /* tx count									*/
		Com->START_TxDMA( );						    /* Start SIF TxDMA							*/
		//KlibWaitus(1);								/* Wait 1us									*/
		//Com->SWTRG_TxDMA( );							/* Start Tx ( software trigger )            */
/*--------------------------------------------------------------------------------------------------*/
		return;

}



/****************************************************************************************************/
/*																									*/
/*		UARTデータ送信＆確認																		*/
/*																									*/
/****************************************************************************************************/
LONG	LpxSifSetTxData( CSIF *this )
{
	LONG		i;
	LONG		DataAddr;
	LONG		rc = FALSE;
	UCHAR		*pTxBuf = this->pTxRes;
	CSIFCOMPORT	*Com = (CSIFCOMPORT *)this->Com;

	/* Set Current TxData Address */
	DataAddr = this->TxResLen - this->TxResCnt;

	if( this->TxResCnt != 0 )
	{ /* When Tx Request Data Exist */
//		if( LPX_TXCHKL(Com) )
		if( Com->CHK_TXEMPTY( ) )
		{ /* Tx FIFO is Empty */
			for( i=0; i<16; i++ )											/* 16 = FIFO size		*/
			{
				*(Com->TXBUF) = pTxBuf[ DataAddr++ ];						/* get a charactor		*/
				this->TxResCnt--;											/* Tx Cnt Decriment		*/

				if( this->TxResCnt == 0 )
				{ /* TXDATA is Noting */
					rc = TRUE;
					break;
				}
			}
		}
	}

	return ( rc );
}



/****************************************************************************************************/
/*																									*/
/*		UARTデータ受信確認																			*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		1) FIFOを使用した2ms周期のデータ受信。														*/
/*																									*/
/****************************************************************************************************/
LONG	LpxSifCheckRxData( CSIF *this, LONG mode )
{
	LONG		i;
	LONG		HeaderSize;
	LONG		rc = SIF_RX_CONTINUE;
	UCHAR		*pRxBuf = this->RxBuf;
	CSIFCOMPORT	*Com = (CSIFCOMPORT *)this->Com;

	if( Com->CHK_TIMEOUT( ) || Com->CHK_RXRDY( ) ) 	/* Check Charactor Timeout or Trigger Interrupt	*/
	{ /* When exist RXDATA */
		KpiRstLongTimer( &(this->RxTimer) );								/* Reset RxTimer		*/
		for( i=0; i<16; i++ )												/* 16 = FIFO size		*/
		{
			if( !LPX_CHK_RXRDY(Com) )
			{ /* RXDATA not ready */
				break;
			}
			pRxBuf[ this->RxCmdCnt++ ] = *(Com->RXBUF);						/* get a charactor		*/
		}
	}

	if( mode == SIF_GETHEADER )
	{ /* ヘッダー取得要求 */
		/* ヘッダーサイズ取得 */
		if( KPI_CHK_OPEMSG( pRxBuf[1]) ){ HeaderSize = DIGOP_MINSIZE; }
/* <S150> >>>>> */
		/* ファームダウンロード時のヘッダ長 */
		else if( KPI_CHK_SYSDLMSG( pRxBuf[1] ) )
		{
			HeaderSize = MBUS_MINSIZE;	/* 通常 */
			if( this->RxCmdCnt > MBUS_DL_SF_IDX )
			{
				switch( pRxBuf[MBUS_DL_SF_IDX] )
				{
				/* プログラム書込み／送信指令はデータ数までをヘッダサイズとする */
				case MBUS_DL_PRGWR:
					/* fall through */
				case MBUS_DL_PRGSND:
					HeaderSize = MBUS_DL_DATANUM_LEN;
					break;
				default:
					break;
				}
			}
		}
/* <<<<< <S150> */
		else							{ HeaderSize = MBUS_MINSIZE; }

		if( this->RxCmdCnt >= HeaderSize )
		{
			rc = SIF_RX_SUCCESS;										/* ヘッダー受信完了		*/
		}
	}
	else /* mode == SIF_GETALLDATA */
	{ /* 全データ取得要求 */
		if( this->RxCmdCnt >= this->RxCmdLen )
		{
			rc = SIF_RX_SUCCESS;										/* 全データ受信完了		*/
		}
	}

	/* Check Time Over */
	if( (rc == SIF_RX_CONTINUE) && (SIF_RXTIMEOVER <= KlibGetLongTimerMs(this->RxTimer)) )
	{ /* 伝文内受信タイムオーバ */
		KpiRstLongTimer( &(this->RxTimer) );							/* Reset RxTimer		*/
		SET_SIFERR( this, SIFERR_RXTMOVR );								/* 伝文内受信T.O.		*/
		rc = SIF_RX_ERROREND;											/* エラー終了			*/
	}


	if( mode == SIF_GETALLDATA )
	{
//		RxCmdCtr = this->RxCmdCnt;
	}

	return ( rc );
}



/****************************************************************************************************/
/*																									*/
/*		ＤＭＡ受信確認																				*/
/*																									*/
/*		1)MICROSSPのUART FIFOを使用する。															*/
/*																									*/
/****************************************************************************************************/
LONG	LpxSifCheckRxDMA( CSIF *this )
{
int i;
int rxcnt;
CSIFCOMPORT *Com = (CSIFCOMPORT *)this->Com;
UCHAR *pRxBuf = this->RxBuf;

/* for debug */
//ULONG *IIRFCR_Address;
//VULONG IIRFCR_data;
//IIRFCR_Address = (ULONG*)0xEFFF4808;
/* for debug */
/*--------------------------------------------------------------------------------------------------*/
	if ( Com->CHK_TIMEOUT( ) ) 								/* Check Charactor Timeout Interrupt */
//	IIRFCR_data = ((VULONG)*IIRFCR_Address & 0x0F);
//	if ( (IIRFCR_data == 0x0C) || (IIRFCR_data == 0x04) )
//	if ( (IIRFCR_data & 0x0F) != 0x01 )
	{
	/*----------------------------------------------------------------------------------------------*/
		Com->RXDISABLE( );									/* Disable SIF Receive */
		Com->STOP_RxDMA( );									/* stop DMA */
		this->RxDmaRcnt = LPX_GET_RXDMA_CNT( Com );			/* save RxDMA count	*/
		rxcnt = this->RxDmaScnt - this->RxDmaRcnt;			/* set receive count */
//		rxcnt = 0;
	/*----------------------------------------------------------------------------------------------*/
		for( i=0; i<16; i++)								/* 16 = FIFO size */
		{
			if ( ! LPX_CHK_RXRDY( Com ) ) { break; }		/* check Rx ready flag */
			pRxBuf[ rxcnt++ ] = *(Com->RXBUF);				/* get a charactor */
		}
	/*----------------------------------------------------------------------------------------------*/
		if( rxcnt == 0 ) { Com->START_RxDMA( );	}			/* Enable RxDMA							*/
	/*----------------------------------------------------------------------------------------------*/
		return (rxcnt);
	}
	else if( Com->CHK_RxDMA_TC( ) )							/* Check Transaction Complete */
	{
		Com->RXDISABLE( );									/* Disable SIF Receive */
		Com->STOP_RxDMA( );									/* stop DMA */
		this->RxDmaRcnt = LPX_GET_RXDMA_CNT( Com );			/* save RxDMA count	*/
		rxcnt = this->RxDmaScnt - this->RxDmaRcnt;			/* set receive count */
		return (rxcnt);
	}
/*--------------------------------------------------------------------------------------------------*/
	return (0);
}



/****************************************************************************************************/
/*																									*/
/*		自局宛メッセージのチェック																	*/
/*																									*/
/****************************************************************************************************/
LONG	LpxSifMbusChkMyMsg( CSIF *this )	/* 自局アドレスチェック									*/
{
		if( SIF_DEBUG == 1 )
		{
			return ( SQSTEP_WAITSTX );							/* debug */
		}
/*--------------------------------------------------------------------------------------------------*/
/*		今回受信したメッセージのアドレス部=="0" or ==自局アドレスの場合TRUEを返す					*/
/*		局アドレス=="0"の場合、メッセージは処理するが無応答とする。									*/
/*		ただし、デジオペメッセージ、ダウンロードメッセージは応答を返す。							*/
/*--------------------------------------------------------------------------------------------------*/
		if( KPI_CHK_OPEMSG( this->RxBuf[1] ) )
		{														/* Operator Message					*/
//<S150>			this->RxCmdLen = LpxSifGetCmdMsgLength(				/* Get Message Length				*/
			this->RxCmdLen = RkiMsgifGetCmdMsgLength(			/* Get Message Length				*/
									this->RxBuf,
									this->RcvBufSize );
			this->StxRes = LpiStartTxSerialMsg;                 /* set method for start Tx			*/
//			return ( SQSTEP_WAITSTX );							/* @@DMA不使用						*/
			return ( SQSTEP_RXCMPLT );
		}
		else if ( KPI_CHK_SYSDLMSG( this->RxBuf[1] ) )
		{														/* System Download Message			*/
//<S150>			this->RxCmdLen = LpxSifGetCmdMsgLength(				/* Get Message Length				*/
			this->RxCmdLen = RkiMsgifGetCmdMsgLength(			/* Get Message Length				*/
									this->RxBuf,
									this->RcvBufSize );
			this->StxRes = LpiStartTxSerialMsg;                 /* set method for start Tx			*/
//			return ( SQSTEP_WAITSTX );							/* @@DMA不使用						*/
			return ( SQSTEP_RXCMPLT );
		}
		else if( this->RxBuf[0] == 0x00 )
		{														/* Other Message( address = 0 )		*/
			this->StxRes = LpiStartTxSerialMsgNoResponse;       /* set method for no response		*/
//			return ( SQSTEP_WAITSTX );							/* @@DMA不使用						*/
			return ( SQSTEP_RXCMPLT );
		}
		else if ( this->RxBuf[0] == (this->AxisNumber) )
		{														/* Other Message(address = my addr)	*/
//<S150>			this->RxCmdLen = LpxSifGetCmdMsgLength(				/* Get Message Length				*/
			this->RxCmdLen = RkiMsgifGetCmdMsgLength(			/* Get Message Length				*/
									this->RxBuf,
									this->RcvBufSize );
			this->StxRes = LpiStartTxSerialMsg;                 /* set method for start Tx			*/
//			return ( SQSTEP_WAITSTX );							/* @@DMA不使用						*/
			return ( SQSTEP_RXCMPLT );
		}
		else
		{
			return ( SQSTEP_INITIAL );
		}
}



/****************************************************************************************************/
/*																									*/
/*		MessageI/F : 指令メッセージ長取得															*/
/*																									*/
/****************************************************************************************************/
#if 0	/* <S150> 削除 */
#define LPX_GET_CMDMSGLEN( i, Mul, Add )\
		if( CmdBufLen < (i+2) ){ return( 0 );}\
		else{ return( (((CmdBuf[i]<<8) + CmdBuf[i+1]) * Mul) + Add );}
#define LPX_CHK_SLOT( SfcH )			(SfcH == 0x01)
#define LPX_GET_SLOT_BYTELEN( SfcH )	((LPX_CHK_SLOT( SfcH ))? 2 : 0)
#define LPX_GET_SLOT_WORDLEN( SfcH )	((LPX_CHK_SLOT( SfcH ))? 1 : 0)
/*--------------------------------------------------------------------------------------------------*/
LONG	LpxSifGetCmdMsgLength( UCHAR *CmdBuf, LONG CmdBufLen )
{
LONG	s = LPX_GET_SLOT_BYTELEN( CmdBuf[2] );
/*--------------------------------------------------------------------------------------------------*/
/*		Check CmdBufLen																				*/
/*--------------------------------------------------------------------------------------------------*/
		if( CmdBufLen < 4 )
		{
			return( 0 );								/* CmdBufLen is Short						*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Check Function Code and Calculate CmdMsgLen													*/
/*--------------------------------------------------------------------------------------------------*/
		switch( CmdBuf[1] )
		{
		case MFC_LOOPBACK:
			 return( 8 );
		case MFC_SVXMSG16:
			 switch( CmdBuf[3] )
			 {
			 case SFC_RD_ONEREG: return(  8 + s );
			 case SFC_WR_ONEREG: return( 10 + s );
			 case SFC_RD_SEQREG: return( 10 + s );
			 case SFC_WR_SEQREG: LPX_GET_CMDMSGLEN( 6 + s, 2, 12 + s );
			 case SFC_RD_RANREG: LPX_GET_CMDMSGLEN( 4 + s, 2,  8 + s );
			 case SFC_WR_RANREG: LPX_GET_CMDMSGLEN( 4 + s, 4, 10 + s );
			 case SFC_RD_MAXNUM: return(  8 + s );
			 default: return( -1 );
			 }
//			 return( -1 );//<S0C7>statement is unreachable
		case MFC_SVXMSG32:
			 switch( CmdBuf[3] )
			 {
			 case SFC_RD_SEQMEM: return( 12 + s );
			 case SFC_WR_SEQMEM: LPX_GET_CMDMSGLEN( 8 + s, 2, 12 + s );
			 default: return( -1 );
			 }
//			 return( -1 );//<S0C7>statement is unreachable
		case MFC_MECHATROLINK:
			 switch( CmdBuf[4] )
			 {
			 case SFC_RD_REQREG_42:
				 return( MBUS42_HEADERSIZE + MBUS42_ADRSIZE + MBUS42_CRC_SIZE );
			 case SFC_RD_RANREG_42:
			 	return( MBUS42_HEADERSIZE + (MBUS42_ADRSIZE * ((CmdBuf[6]<<8)|CmdBuf[7])) + MBUS42_CRC_SIZE );
			 case SFC_WR_SEQREG_42:
			 	switch( CmdBuf[5] & 0x0F )
			 	{
		 		case MBUS42_DTYPE_B: /* データ型：Byte(8bit) */
				 	return( MBUS42_HEADERSIZE + MBUS42_ADRSIZE + (1 * ((CmdBuf[6]<<8)|CmdBuf[7])) + MBUS42_CRC_SIZE );
		 		case MBUS42_DTYPE_S: /* データ型：Byte(16bit) */
				 	return( MBUS42_HEADERSIZE + MBUS42_ADRSIZE + (2 * ((CmdBuf[6]<<8)|CmdBuf[7])) + MBUS42_CRC_SIZE );
		 		case MBUS42_DTYPE_L: /* データ型：Byte(32bit) */
				 	return( MBUS42_HEADERSIZE + MBUS42_ADRSIZE + (4 * ((CmdBuf[6]<<8)|CmdBuf[7])) + MBUS42_CRC_SIZE );
			 	default: return( -1 );
			 	}
			 case SFC_WR_RANREG_42:
			 	return( MBUS42_HEADERSIZE + ((MBUS42_ADRSIZE + 4) * ((CmdBuf[6]<<8)|CmdBuf[7])) + MBUS42_CRC_SIZE );
			 case SFC_RD_MAXNUM_42:
				 	return( MBUS42_HEADERSIZE + MBUS42_CRC_SIZE );
			 case SFC_RD_PRDCTID_42:
			 default: return( -1 );
			 }
		case MFC_SYSDWNLD:								/* @@@ Add Later @@@						*/
			 return( MbusFuncSysDLGetMsgLength(CmdBuf) );
		default:
			 if( KPI_CHK_OPEMSG( CmdBuf[1] ) )
			 {
				return( CmdBuf[2] + (CmdBuf[3]<<8) );
			 }
			 return( -1 );
		}
/*--------------------------------------------------------------------------------------------------*/
		/* unreachable return( -1 );  */				/* Unknown Function Code					*/
}
#endif	/* <S150> */

/***************************************** end of file **********************************************/
