/****************************************************************************/
/*																			*/
/*		SC Uart data Control ソースファイル									*/
/*																			*/
/****************************************************************************/
/*																			*/
/*	Target model: SigmaV Command-Option	ServoDrive							*/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/****************************************************************************/
/*	History:																*/
/*	2007.9.12	created														*/
/*																			*/
/*																			*/
/************** Copyright(c) Yaskawa Electric Corporation *******************/

#define	KPI_IOREG


/* includes */
#include "KnlStd.h"
#include "DeviceMngr.h"
#include "SC_UartDrv.h"


/* defines */
/*--------------------------------------------------------------------------------------------------*/
#define	MBUS_MINSIZE		5				/* MEMOBUSメッセージ最小バイト数						*/
#define	MBUS_MAXSIZE		256				/* MEMOBUSメッセージ最大バイト数						*/
/*--------------------------------------------------------------------------------------------------*/
#define	SIF_RXTIMEOVER		 100			/* 伝文内受信タイムオーバ設定値   [ms]					*/
/*--------------------------------------------------------------------------------------------------*/
#define	SQSTEP_INITIAL			 0
#define	SQSTEP_STARTRX			 1
#define	SQSTEP_WAITRX			 2
#define	SQSTEP_RX1ST	  		 3
#define	SQSTEP_RX2ND	  		 4
#define	SQSTEP_RXLAST	  		 5
#define	SQSTEP_RXCMPLT			 6
#define	SQSTEP_WAITSTX			 7
#define	SQSTEP_WAITSTX2			 8
#define	SQSTEP_STARTTX			 9
#define	SQSTEP_WAITETX			10
#define	SQSTEP_CHKASIF	 		11
#define	SQSTEP_TXCMPLT			12
#define	SQSTEP_RXTMOVER			13
#define	SQSTEP_RXSEQERR			14
#define	SQSTEP_XMDCHKRM			15
#define	SQSTEP_XMDMKANS			16


/****************************************************************************************************/
/*																									*/
/*		V850 ME3 ＣＰＵ：シリアルインタフェイス定義													*/
/*																									*/
/****************************************************************************************************/
#define	CPU_SIF2_RXBUF		UB0RX							/* Serial Receive  Data	Buffer			*/
//#define	CPU_SIF2_TXBUF		UB0TX							/* Serial Transmit Data Buffer			*/
#define	CPU_SIF2_RXSTS		UB0STR							/* Serial Receive  Status Reg.			*/
//#define	CPU_SIF2_RXCHKF		URIF0							/* Serial Receive  Check Flag (Int.Req)	*/
//#define	CPU_SIF2_TXCHKF		UTIF0							/* Serial Transmit Check Flag (Int.Req)	*/
/*--------------------------------------------------------------------------------------------------*/
//#define	KPX_SIF2_RSTRXSTS()		{ CPU_SIF2_RXSTS = 0x00;}		/* Serial Reset Rx Status			*/
//#define	KPX_SIF2_CHKTXBUSY()	( UB0TSF == 1 )					/* Serial TxBuf    Busy   Check		*/
#define	KPX_SIF2_CHKTXFNSH()	( UB0TSF == 0x00 )				/* Serial Transmit Finish Check		*/
/*--------------------------------------------------------------------------------------------------*/
#define	KPX_SIF2_RXENABLE()		( UB0RXE=0x01 )					/* Serial Receive  Enable			*/
#define	KPX_SIF2_RXDISABLE()	( UB0RXE=0x00 )					/* Serial Receive  Disable			*/
/*--------------------------------------------------------------------------------------------------*/
#define	KPX_SIF2_TXENABLE()		( UB0TXE=0x01 )					/* Serial Transmit Enable			*/
#define	KPX_SIF2_TXDISABLE()	( UB0TXE=0x00 )					/* Serial Transmit Disable			*/
/*--------------------------------------------------------------------------------------------------*/

/****************************************************************************************************/
/*																									*/
/*		ＣＰＵ：ＤＭＡインタフェイス定義															*/
/*																									*/
/****************************************************************************************************/
//#define SIF2_SETERR( code )	{ OpSif.ErrCod = code; ++OpSif.ErrCnt;}
#define SIF2_SETERR( code )	 (OpSif.ErrCod = (code))
/*--------------------------------------------------------------------------------------------------*/
/*		Address Definition for Setting SIF DMA														*/
/*--------------------------------------------------------------------------------------------------*/
#define	SIF2_SET_RxDTFR		0x3B							/* RxDMA TrgSel:INTSR0 (UART IntRx)		*/
#define	SIF2_SET_TxDTFR		0x3C							/* TxDMA TrgSel:INTST0 (UART IntTx)		*/
/*--------------------------------------------------------------------------------------------------*/
#define	SIF2_SET_RxDADC		0xE800							/* RxDMA Addr. Control Register			*/
#define	SIF2_SET_TxDADC		0xB200							/* TxDMA Addr. Control Register			*/
/*--------------------------------------------------------------------------------------------------*/
#define	SIF2_SET_RXB_HIADR	0x0FFF							/* RxDMA Serial RXB0 High Address		*/
#define	SIF2_SET_RXB_LOADR	0xFA06							/* RxDMA Serial RXB0 Low  Address		*/
#define	SIF2_SET_TXB_HIADR	0x0FFF							/* TxDMA Serial TXB0 High Address		*/
#define	SIF2_SET_TXB_LOADR	0xFA08							/* TxDMA Serial TXB0 Low  Address		*/
/*--------------------------------------------------------------------------------------------------*/
/*		DMA I/F Register Definition for SIF															*/
/*--------------------------------------------------------------------------------------------------*/
#define	SIF2_RxDBC			DBC0							/* RxDMA Transfer Count Register		*/
#define	SIF2_RxDSAL			DSA0L							/* RxDMA Source Address Register		*/
#define	SIF2_RxDSAH			DSA0H							/* RxDMA Source Address Register		*/
#define	SIF2_RxDDAL			DDA0L							/* RxDMA Destination Address Register	*/
#define	SIF2_RxDDAH			DDA0H							/* RxDMA Destination Address Register	*/
#define	SIF2_RxDADC			DADC0							/* RxDMA Addressing Control Register  	*/
#define	SIF2_RxDTFR			DTFR0							/* RxDMA Trigger Factor Register		*/
#define	SIF2_RxDCHC			DCHC0							/* RxDMA Channel Control Register		*/
#define	SIF2_RxTC			TC0								/* RxDMA Transfer Complete Flag			*/
/*--------------------------------------------------------------------------------------------------*/
#define	SIF2_TxDBC			DBC1							/* TxDMA Transfer Count Register		*/
#define	SIF2_TxDSAL			DSA1L							/* TxDMA Source Address Register		*/
#define	SIF2_TxDSAH			DSA1H							/* TxDMA Source Address Register		*/
#define	SIF2_TxDDAL			DDA1L							/* TxDMA Destination Address Register	*/
#define	SIF2_TxDDAH			DDA1H							/* TxDMA Destination Address Register	*/
#define	SIF2_TxDADC			DADC1							/* TxDMA Addressing Control Register  	*/
#define	SIF2_TxDTFR			DTFR1							/* TxDMA Trigger Factor Register		*/
#define	SIF2_TxDCHC			DCHC1							/* TxDMA Channel Control Register		*/
#define	SIF2_TxTC			TC1								/* TxDMA Transfer Complete Flag			*/
#define	SIF2_TxDF			DF1								/* TxDMA Transfer Request Flag			*/

/*--------------------------------------------------------------------------------------------------*/
/*		Stop DMA CPU Bug Fix for V850E/MA1															*/
/*--------------------------------------------------------------------------------------------------*/
#define KPX_TC01_RxDMA_BUGFIX( )
#define KPX_TC01_TxDMA_BUGFIX( )
/*--------------------------------------------------------------------------------------------------*/
#define KPX_STOP_DMA01_BUGFIXA( )
#define KPX_STOP_DMA01_BUGFIXB( )
/*--------------------------------------------------------------------------------------------------*/
/*		SIF RxDMA/TxDMA Operation Macro																*/
/*--------------------------------------------------------------------------------------------------*/
#define KPX_SIF2_STOP_RxDMA( )	 (SIF2_RxDCHC = 0x04)		/* Stop SIF RxDMA (DMA CH0)				*/
#define KPX_SIF2_STOP_TxDMA( )	 (SIF2_TxDCHC = 0x04)		/* Stop SIF TxDMA (DMA CH1)				*/
/*--------------------------------------------------------------------------------------------------*/
#define KPX_SIF2_ENABLE_RxDMA( )	(SIF2_RxDCHC = 0x01)		/* Enable SIF RxDMA					*/
#define KPX_SIF2_ENABLE_TxDMA( )	(SIF2_TxDCHC = 0x01)		/* Enable SIF TxDMA					*/
#define KPX_SIF2_SWTRIG_TxDMA( )	(SIF2_TxDCHC = 0x03)		/* S/W Trigger SIF TxDMA			*/
/*--------------------------------------------------------------------------------------------------*/
//#define KPX_SIF_RESET_TxDMA( )	{ SIF_TxDTFR = SIF_SET_TxDTFR;}	/* Reset TxDMA Request Flag		*/
#define KPX_SIF2_RESET_TxDMA( )	(SIF2_TxDF = 0x00)			/* Reset TxDMA Request Flag				*/
/*--------------------------------------------------------------------------------------------------*/
/*		V850E2/ME3 UART送信+DMAの送信開始不具合対策用												*/
/*			( TxDMA DTFR3（送信DMAの要因レジスタ）をクリアした後、DCHC3（送信DMA制御）を停止		*/
/*			  RxDMA 操作を外す。-> NEC-ELから提示された修正方法										*/
/*--------------------------------------------------------------------------------------------------*/
#define KPX_SIF2_RESET_TxDMA_BUGFIXA( )	do{						/* Reset SIF TxDMA					*/\
		SIF2_TxDF   = 0x00;										/* Reset DMA Request 				*/\
		SIF2_TxDCHC = 0x04;										/* Stop TxDMA						*/\
		SIF2_TxDCHC = 0x00;										/* DMA Channel Control Register		*/\
		SIF2_TxDF   = 0x00;										/* Reset TxDMA Request Flag			*/\
		}while(0)
/*--------------------------------------------------------------------------------------------------*/
#define KPX_SIF2_RxDMA_CNT( )	( SIF2_RxDBC )				/* SIF RxDMA Count						*/
#define KPX_SIF2_TxDMA_CNT( )	( SIF2_TxDBC )				/* SIF TxDMA Count						*/
/*--------------------------------------------------------------------------------------------------*/
#define KPX_SIF2_CHK_RxDMA_TC( )	( SIF2_RxTC != 0)		/* Check RxDMA Transfer Complete		*/
#define KPX_SIF2_CHK_TxDMA_TC( )	( SIF2_TxTC != 0)		/* Check TxDMA Transfer Complete		*/
/*--------------------------------------------------------------------------------------------------*/
#define	KPX_SIF2_CHK_TxDMA_DTFR( )	( (SIF2_TxDTFR & 0x80) != 0 )/* Check DMA END for only V850ME3	*/
/*--------------------------------------------------------------------------------------------------*/
#define KPX_SIF2_INIT_RxDMA( ) do{								/* Init SIF RxDMA					*/\
		SIF2_RxDSAL = SIF2_SET_RXB_LOADR;						/* DMA Src Low  Address Register	*/\
		SIF2_RxDSAH = SIF2_SET_RXB_HIADR;						/* DMA Src High Address Register	*/\
		SIF2_RxDADC = SIF2_SET_RxDADC;							/* DMA Addressing Control Register	*/\
		SIF2_RxDTFR = SIF2_SET_RxDTFR;							/* DMA Trigger Register				*/\
		SIF2_RxDCHC = 0x00;										/* DMA Channel Control Register		*/\
		}while(0)
/*--------------------------------------------------------------------------------------------------*/
#define KPX_SIF2_INIT_TxDMA( ) do{								/* Init SIF TxDMA					*/\
		SIF2_TxDDAL = SIF2_SET_TXB_LOADR;						/* DMA Des. Low  Address Register	*/\
		SIF2_TxDDAH = SIF2_SET_TXB_HIADR;						/* DMA Des. High Address Register	*/\
		SIF2_TxDADC = SIF2_SET_TxDADC;							/* DMA Addressing Control Register	*/\
		SIF2_TxDTFR = SIF2_SET_TxDTFR;							/* DMA Trigger Register				*/\
		SIF2_TxDCHC = 0x00;										/* DMA Channel Control Register		*/\
		}while(0)
/*--------------------------------------------------------------------------------------------------*/
#define KPX_SIF2_SETX_RxDMA( Adr, Cnt ) do{						/* Set SIF RxDMA					*/\
		SIF2_RxDDAL = KPX_SIF_SET_DMA_LOADR( Adr );				/* DMA Source Low  Address Register	*/\
		SIF2_RxDDAH = KPX_SIF_SET_DMA_HIADR( Adr );				/* DMA Source High Address Register	*/\
		SIF2_RxDBC  = (Cnt);									/* DMA Count Register				*/\
		}while(0)
/*--------------------------------------------------------------------------------------------------*/
#define KPX_SIF2_SETX_TxDMA( Adr, Cnt ) do{						/* Set SIF TxDMA					*/\
		SIF2_TxDF   = 0x00;										/* Reset DMA Request Flag			*/\
		SIF2_TxDSAL = KPX_SIF_SET_DMA_LOADR( Adr );				/* DMA Source Low  Address Register	*/\
		SIF2_TxDSAH = KPX_SIF_SET_DMA_HIADR( Adr );				/* DMA Source High Address Register	*/\
		SIF2_TxDBC  = (Cnt);									/* DMA Count Register				*/\
		}while(0)
/*--------------------------------------------------------------------------------------------------*/



/* globals */
#pragma ghs startzda
static SIF		OpSif;								/* シリアル通信ＩＦ変数									*/
#pragma ghs endzda


/* forward declarations */
#ifdef __cplusplus
extern	"C"	{
#endif

/* Private functions */
static void	opSifStartRxDMA(const UCHAR * pRxBuf, USHORT rxSz );
static void	opSifStartTxDMA(const UCHAR * pTxBuf, USHORT transSz );
static void	opSifStopxRxDMA( void );
static USHORT	opMsg1stAnalyze(const UCHAR *pRcvBuf, UCHAR *TblIdx, UCHAR *ConfBLen);
static USHORT	opMsg2ndAnalyze(const UCHAR *pRcvBuf, UCHAR TblIdx);

#ifdef __cplusplus
}
#endif



/****************************************************************************
****************************************************************************/
/* SC Message Data Exchange Functions */
/****************************************************************************
****************************************************************************/
#ifdef	__ghs__
#pragma ghs section text=".textEX"
#endif

/*****************************************************************************
	SC Message状態取得
-----------------------------------------------------------------------------
paramter:	none
return;		SCMSG_ENABLE	- ready state
			SCMSG_BUSY		- not ready state
*****************************************************************************/
LONG MLOpiGetSCMsgState(void)
{
	LONG	rc = SCMSG_BUSY;
	if( (OpSif.SqStep == SQSTEP_WAITSTX) && (OpSif.f.MemobusMode != FALSE) )
	{
		rc = SCMSG_ENABLE;
	}
	return rc;
}

/*****************************************************************************
	SC Messageデータ転送要求発行
-----------------------------------------------------------------------------
paramter:	pMsgBuf			- pointer to message data
			bSz				- size of message data
return;		SCMSG_BUSY		- request error
			SCMSG_SUCCESS	- normal end
*****************************************************************************/
LONG MLOpiReqSCMsgTrans(USHORT *pMsgBuf, LONG bSz)
{
	LONG	i, rc;
	UCHAR	*pUCbuf = (UCHAR*)pMsgBuf;

	if(OpSif.SqStep != SQSTEP_WAITSTX)
	{/* 処理中 */
		rc = SCMSG_BUSY;
	}
	else
	{
	/* レスポンスセット */
		for(i = 0; i < bSz; i++)
		{
			OpSif.TxBuf[i] = pUCbuf[i];
		}
		OpSif.MbusTrnLength = (USHORT)bSz;
		KPX_SIF2_RESET_TxDMA_BUGFIXA( );	/* V850E2/ME3：TxDMA誤動作対策		*/
		opSifStartTxDMA(OpSif.TxBuf, (USHORT)(bSz - 1)); 			/* 送信ＤＭＡスタート	*/

		OpSif.RxDbcSv = sizeof(OpSif.RxBuf) - 1;
		opSifStartRxDMA(OpSif.RxBuf, OpSif.RxDbcSv);	/* 受信ＤＭＡスタート	*/
		OpSif.SqStep = SQSTEP_WAITETX;		/* 次の指令Messageをすぐに受けられる*/
		rc = SCMSG_SUCCESS;
	}
	return rc;
}

/*****************************************************************************
	SC Message応答データ取得
-----------------------------------------------------------------------------
paramter:	pMsgBuf			- pointer to message data
			bSz				- size of message data
return;		SCMSG_BUSY		- busy state
			SCMSG_SUCCESS	- normal end
*****************************************************************************/
LONG MLOpiGetSCMsgRspData(USHORT *pMsgBuf, LONG *bSz)
{
	LONG	i, rc;
	UCHAR	*pUCbuf = (UCHAR*)pMsgBuf;

	*bSz = 0;

	if(OpSif.SqStep != SQSTEP_WAITSTX)
	{/* 処理中 */
		rc = SCMSG_BUSY;
	}
	else
	{
	/* レスポンスセット */
		for(i = 0; i < OpSif.RcvbLength; i++)
		{
			pUCbuf[i] = OpSif.RxBuf[i];
		}
		*bSz = (LONG)OpSif.RcvbLength;
		rc = SCMSG_SUCCESS;
	}
	return rc;
}

/*****************************************************************************
	SC Messageキャンセル要求
-----------------------------------------------------------------------------
paramter:	none
return;		none
*****************************************************************************/
void MLOpiCancelSCMsgReq(void)
{
	if(OpSif.SqStep != SQSTEP_WAITSTX)
	{
		KPX_SIF2_TXDISABLE( );			/* Transmit Disable						*/
		opSifStopxRxDMA( );				/* Stop RxDMA							*/
		KPX_SIF2_RESET_TxDMA_BUGFIXA();	/* Stop TxDMA							*/
		KPX_SIF2_INIT_RxDMA( );			/* Init SIF RxDMA for RXB0  --> RxBuf	*/
		KPX_SIF2_INIT_TxDMA( );			/* Init SIF TxDMA for TxBuf --> TXB0	*/
		OpSif.SqStep = SQSTEP_INITIAL;	/* Goto Initial Step					*/
	}

	return;
}


#ifdef	__ghs__
#pragma ghs section text=default
#endif



#ifdef	__ghs__
#pragma ghs section text=".textEX"
#endif

/*****************************************************************************
	SC Message有効設定
*****************************************************************************/
void OpxEnableSCSif( USHORT data )
{
	OpSif.f.MemobusMode = (data != 0) ? TRUE: FALSE;
}


/****************************************************************************************************/
/*																									*/
/*		シリアル通信メッセージ初期化処理															*/
/*																									*/
/****************************************************************************************************/
void	OpxInitSCSerialMsg( void )
{
		KpiResetWatchdog( );							/* Reset Watchdog							*/
/*--------------------------------------------------------------------------------------------------*/
/*		シリアル通信ＩＦデータ初期化																*/
/*--------------------------------------------------------------------------------------------------*/
		MlibResetLongMemory( &OpSif, sizeof( OpSif )/4 );	/* Reset All Data to Zero					*/
/*--------------------------------------------------------------------------------------------------*/
/*		シリアル通信ＩＦ初期化																		*/
/*--------------------------------------------------------------------------------------------------*/
//		KPX_SIF2_TXDISABLE( );							/* Transmit Disable							*/
//		KPX_SIF2_RXDISABLE( );							/* Transmit Disable							*/
		KpxInitCpuSerialIF( 0, 19200, 0x0E );			/* Initialize CPU SIF CH0					*/
/*--------------------------------------------------------------------------------------------------*/
/*		シリアル通信用ＤＭＡ初期化																	*/
/*--------------------------------------------------------------------------------------------------*/
		KPX_SIF2_INIT_RxDMA( );							/* Init SIF RxDMA for RXB0  --> RxBuf		*/
		KPX_SIF2_RESET_TxDMA_BUGFIXA();
		KPX_SIF2_INIT_TxDMA( );							/* Init SIF TxDMA for TxBuf --> TXB0		*/

/*--------------------------------------------------------------------------------------------------*/
		KpiResetWatchdog( );							/* Reset Watchdog							*/
		return;
}

#ifdef	__ghs__
#pragma ghs section text=default
#endif



/****************************************************************************************************/
/*																									*/
/*		シリアル通信メッセージ定周期監視処理 ( ScanC Cycle )										*/
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
void	OpxSCSifMsgManager( void )
{
USHORT	timer, ucwk;
LONG	rxcnt;
/*--------------------------------------------------------------------------------------------------*/
/*		Reset Error Status																			*/
/*--------------------------------------------------------------------------------------------------*/
		if((CPU_SIF2_RXSTS & 0x7) != 0)						/* Read	Error check						*/
		{
			ucwk = CPU_SIF2_RXBUF;							/* Dummy Read							*/
			CPU_SIF2_RXSTS = 0;								/* Reset Error Status					*/
		}

/*--------------------------------------------------------------------------------------------------*/
/*		シリアル通信送受信監視シーケンス (受信:DMA-CH0,SIF-RXB0, 送信:DMA-CH1,SIF-TXB0)				*/
/*--------------------------------------------------------------------------------------------------*/
		switch( OpSif.SqStep )
		{
		case SQSTEP_INITIAL: /*--------------------------- 初期ステップ ----------------------------*/
			OpSif.SqStep = SQSTEP_WAITSTX;				/* 受信開始処理Stepに遷移					*/
			break;
		case SQSTEP_WAITSTX: /*--------------------------- 送信開始待ち ----------------------------*/
			break;

		case SQSTEP_WAITETX: /*--------------------------- 送信完了待ち ----------------------------*/
			if( KPX_SIF2_CHK_TxDMA_TC( ) )				/* ＤＭＡ送信完了チェック					*/
			{
				KPX_TC01_TxDMA_BUGFIX( );					/* @@@V850E/MA1不具合対応@@@				*/
				OpSif.SqStep = SQSTEP_TXCMPLT;			/* 送信完了確認STEPに遷移					*/
			}
			break;
		case SQSTEP_TXCMPLT: /*-------------------------- 送信完了処理 -----------------------------*/
			if( KPX_SIF2_CHKTXFNSH( ) )					/* ＳＩＦ送信完了チェック					*/
			{
				KPX_SIF2_TXDISABLE( );					/* Transmit Disable							*/
				if( KPX_SIF2_CHK_TxDMA_DTFR( ) )		/* ＳＩＦ送信完了チェック					*/
				{										/* V850E2/ME3：TxDMAの誤動作対策			*/
					KPX_SIF2_RESET_TxDMA( );			/* V850E2/ME3：TxDMAの誤動作対策			*/
				}										/* V850E2/ME3：TxDMAの誤動作対策			*/
				OpSif.SqStep = SQSTEP_WAITRX;			/* 受信待ちSTEPに遷移						*/
			}
			break;


		case SQSTEP_WAITRX: /*---------------------------- 受信待ち --------------------------------*/
			if( KPX_SIF2_RxDMA_CNT() != OpSif.RxDbcSv )		/* Check Receive						*/
			{
				OpSif.RxDbcRv = (USHORT)KPX_SIF2_RxDMA_CNT( );/* Save  RxDMA Count					*/
				KpiRstShortTimer( &OpSif.RxTimer );			/* Reset RxTimer						*/
				OpSif.SqStep = SQSTEP_RX1ST;				/* 1byte受信STEPに遷移					*/
			}
			break;
		case SQSTEP_RX1ST:/*------------------------------ 最初の４バイト受信(１次解析)-------------*/
			if( KPX_SIF2_RxDMA_CNT( ) != OpSif.RxDbcRv )	/* 受信チェック							*/
			{
				OpSif.RxDbcRv = (USHORT)KPX_SIF2_RxDMA_CNT( );/* Save  RxDMA Count					*/
				KpiRstShortTimer( &OpSif.RxTimer );			/* Reset RxTimer						*/
				/*----------------------------------------------------------------------------------*/
				rxcnt = OpSif.RxDbcSv - OpSif.RxDbcRv;		/* Receive Count						*/
				if( rxcnt >= 4 )							/* Receive 4 byte ?						*/
				{
					OpSif.RcvbLength = opMsg1stAnalyze(OpSif.RxBuf,/* 初回のメッセージ解析			*/
											 &OpSif.MbusTblIndex, &OpSif.ConfbLength);
					OpSif.SqStep = (UCHAR)((OpSif.RcvbLength != 0) ? SQSTEP_RXLAST: SQSTEP_RX2ND);
				}
			}
			else											/* 受信なし								*/
			{
				timer = KpiGetShortTimer(&OpSif.RxTimer);	/* Get Timer							*/
				if( timer > SIF_RXTIMEOVER )				/* Check Receive TimeOver				*/
				{
					OpSif.SqStep = SQSTEP_RXTMOVER;			/* 受信タイムオーバ処理STEPに遷移		*/
				}
			}
			break;
		case SQSTEP_RX2ND:	/*---------------------------- 受信メッセージ長解析部まで受信(2nd解析)--*/
			if( KPX_SIF2_RxDMA_CNT( ) != OpSif.RxDbcRv )	/* 受信チェック							*/
			{
				OpSif.RxDbcRv = (USHORT)KPX_SIF2_RxDMA_CNT( );/* Save  RxDMA Count					*/
				KpiRstShortTimer( &OpSif.RxTimer );			/* Reset RxTimer						*/
				/*----------------------------------------------------------------------------------*/
				rxcnt = OpSif.RxDbcSv - OpSif.RxDbcRv;		/* Receive Count						*/
				if( rxcnt > OpSif.ConfbLength )				/* メッセージ長定義部まで受信 ?			*/
				{
					OpSif.RcvbLength = opMsg2ndAnalyze(OpSif.RxBuf, OpSif.MbusTblIndex);
					OpSif.SqStep = SQSTEP_RXLAST;			/* 最終受信STEPに遷移					*/
				}
			}
			else											/* 受信なし								*/
			{
				timer = KpiGetShortTimer(&OpSif.RxTimer);	/* Get Timer							*/
				if( timer > SIF_RXTIMEOVER )				/* Check Receive TimeOver				*/
				{
					OpSif.SqStep = SQSTEP_RXTMOVER;			/* 受信タイムオーバ処理STEPに遷移		*/
				}
			}
			break;
		case SQSTEP_RXLAST:	/*---------------------------- メッセージ全体受信 ----------------------*/
			if( KPX_SIF2_RxDMA_CNT( ) != OpSif.RxDbcRv )	/* 受信チェック							*/
			{
				OpSif.RxDbcRv = (USHORT)KPX_SIF2_RxDMA_CNT( );/* Save  RxDMA Count					*/
				KpiRstShortTimer( &OpSif.RxTimer );			/* Reset RxTimer						*/
				/*----------------------------------------------------------------------------------*/
				rxcnt = OpSif.RxDbcSv - OpSif.RxDbcRv;		/* Receive Count						*/
				if( rxcnt >= OpSif.RcvbLength )				/* Receive Complete ?					*/
				{
					opSifStopxRxDMA( );						/* Stop RxDMA							*/
					KpiRstShortTimer( &OpSif.SxTimer );		/* Reset SxTimer						*/
					OpSif.SqStep = SQSTEP_WAITSTX;			/* Adr=0 or 自局宛:送信開始待ちへ		*/
				}
			}
			else											/* 受信なし								*/
			{
				timer = KpiGetShortTimer(&OpSif.RxTimer);	/* Get Timer							*/
				if( timer > SIF_RXTIMEOVER )				/* Check Receive TimeOver				*/
				{
					OpSif.SqStep = SQSTEP_RXTMOVER;			/* 受信タイムオーバ処理STEPに遷移		*/
				}
				/*----------------------------------------------------------------------------------*/
				if( KPX_SIF2_CHK_RxDMA_TC( ) )				/* ＤＭＡ停止チェック					*/
				{
					KPX_TC01_RxDMA_BUGFIX( );					/* @@@V850E/MA1不具合対応@@@			*/
					OpSif.ErrFlag = TRUE;					/* ＳＩＦエラーフラグセット				*/
					SIF2_SETERR( 0xFF );					/* Set ErrCode and ++ErrCnt				*/
					OpSif.SqStep = SQSTEP_INITIAL;			/* 初期STEPに遷移						*/
				}
			}
			break;

/*--------------------------------------------------------------------------------------------------*/
/*		伝文内受信タイムオーバ異常処理																*/
/*--------------------------------------------------------------------------------------------------*/
		case SQSTEP_RXTMOVER: /*-------------------------- 受信タイムオーバ ------------------------*/
			OpSif.RcvbLength = OpSif.RxDbcSv - OpSif.RxDbcRv;
			OpSif.ErrFlag = TRUE;						/* ＳＩＦエラーフラグセット					*/
			opSifStopxRxDMA( );							/* Stop RxDMA								*/
			OpSif.SqStep = SQSTEP_INITIAL;				/* Goto Initial Step						*/
			break;
		default: /*--------------------------------------- デフォルト ------------------------------*/
			break;
		}
/*--------------------------------------------------------------------------------------------------*/

		return;
}



/****************************************************************************************************/
/*																									*/
/*		ＳＩＦ受信ＤＭＡスタート処理																*/
/*																									*/
/****************************************************************************************************/
static void	opSifStartRxDMA(const UCHAR * pRxBuf, USHORT rxSz )
{
/*--------------------------------------------------------------------------------------------------*/
/*		ＤＭＡアドレス設定 : 通常伝送モード															*/
/*--------------------------------------------------------------------------------------------------*/
		KPX_SIF2_SETX_RxDMA( pRxBuf, rxSz );			/* Set DMA CH0 Address/Count			*/
/*--------------------------------------------------------------------------------------------------*/
/*		ＤＭＡ＆ＳＩＦスタート処理																	*/
/*--------------------------------------------------------------------------------------------------*/
		KPX_SIF2_ENABLE_RxDMA( );						/* Enable RxDMA								*/
		KPX_SIF2_RXENABLE( );							/* Enable SIF Receive						*/
/*--------------------------------------------------------------------------------------------------*/
		return;

}


/****************************************************************************************************/
/*																									*/
/*		ＳＩＦ受信ＤＭＡストップ処理																*/
/*																									*/
/****************************************************************************************************/
static void	opSifStopxRxDMA( void )
{
/*--------------------------------------------------------------------------------------------------*/
/*		ＳＩＦストップ処理																			*/
/*--------------------------------------------------------------------------------------------------*/
		KPX_SIF2_RXDISABLE( );							/* Disable SIF Receive						*/
/*--------------------------------------------------------------------------------------------------*/
/*		ＤＭＡストップ処理								@@@ V850E/MA1不具合対策 @@@					*/
/*--------------------------------------------------------------------------------------------------*/
		KPI_DI( );										/* Disable Interrupt						*/
	/*----------------------------------------------------------------------------------------------*/
		KPX_STOP_DMA01_BUGFIXA( );						/* BugFix for V850E/MA1						*/
	/*----------------------------------------------------------------------------------------------*/
		KPX_SIF2_STOP_RxDMA( );							/* Stop SIF RxDMA							*/
		KPX_SIF2_STOP_TxDMA( );							/* Stop SIF TxDMA							*/
	/*----------------------------------------------------------------------------------------------*/
		KPX_STOP_DMA01_BUGFIXB( );						/* BugFix for V850E/MA1		 				*/
	/*----------------------------------------------------------------------------------------------*/
		KPI_EI( );										/* Enable Interrupt							*/
/*--------------------------------------------------------------------------------------------------*/
		return;
}


/****************************************************************************************************/
/*																									*/
/*		ＳＩＦ送信ＤＭＡスタート処理																*/
/*																									*/
/****************************************************************************************************/
static void	opSifStartTxDMA(const UCHAR * pTxBuf, USHORT transSz  )
{
		KPX_SIF2_TXENABLE( );							/* SIF Transmit Enable						*/
/*--------------------------------------------------------------------------------------------------*/
/*		ＤＭＡ転送アドレス＆転送カウント設定														*/
/*--------------------------------------------------------------------------------------------------*/
		KPX_SIF2_SETX_TxDMA( pTxBuf,  transSz);
/*--------------------------------------------------------------------------------------------------*/
/*		ＤＭＡ転送開始																				*/
/*--------------------------------------------------------------------------------------------------*/
		KPX_SIF2_ENABLE_TxDMA( );						/* Enable SIF TxDMA							*/
		KlibWaitus( 1 );								/* Wait 1 us								*/
		KPX_SIF2_SWTRIG_TxDMA( );						/* Start SIF TxDMA (Software Trigger)		*/
/*--------------------------------------------------------------------------------------------------*/
		return;
}







/*--------------------------------------------------------------------------------------------------*/
/*		サポートコマンド登録テーブル																*/
/*--------------------------------------------------------------------------------------------------*/
/*	定義方法：以下のようにレスポンスメッセージ長が求められるように定義する							*/
/*																									*/
/*		レスポンスメッセージバイト長 = FixPktSz + AltSz*AltDataSz									*/
/*			( AltSz = レスポンスのAltSzPosで指定したバイト位置の値 (AltSzLenのサイズでリード) )		*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct tagMBUSRSP_TBL {
		UCHAR	FuncCode;			/* ファンクション・コード定義									*/
		USHORT	FuncSubCode;		/* ファンクション・サブコード定義								*/
		UCHAR	FixPktSz;			/* 固定部のバイトサイズ											*/
		UCHAR	AltSzPos;			/* 可変長データ数定義バイト位置									*/
		UCHAR	AltSzLen;			/* 可変長データ数のデータ長										*/
		UCHAR	AltDataSz;			/* 可変長データ単位（バイト数）									*/
} MBUSRSP_TBL;
static const MBUSRSP_TBL MbusRspTbl[] = {
/*--------------------------------------------------------------------------------------------------*/
/*	メインコード		サブコード			固定長		定義位置	定義サイズ	データ単位			*/
/*--------------------------------------------------------------------------------------------------*/
/*	  標準コマンド登録テーブル																		*/
	{ S_FCID_SREG_RD,	0x0000,				0x05,		0x02,		0x01,		0x01	},
	{ S_FCID_SREG_WR,	0x0000,				0x08,		0x00,		0x00,		0x00	},
	{ S_FCID_LOOPBACK,	0x0000,				0x08,		0x00,		0x00,		0x00	},
	{ S_FCID_PREG_WR,	0x0000,				0x08,		0x00,		0x00,		0x00	},
/*--------------------------------------------------------------------------------------------------*/
/*	  拡張コマンド(１６ビットアクセス用)登録テーブル												*/
/*--------------------------------------------------------------------------------------------------*/
	{ EX16_FCID,		E_SFCID_REG_RD,		0x08,		0x04,		0x02,		0x01	},
	{ EX16_FCID,		E_SFCID_SREG_WR,	0x0A,		0x00,		0x00,		0x00	},
	{ EX16_FCID,		E_SFCID_DCREG_RD,	0x08,		0x04,		0x02,		0x01	},
	{ EX16_FCID,		E_SFCID_DCREG_WR,	0x08,		0x00,		0x00,		0x00	},
	{ EX16_FCID,		E_SFCID_PREG_WR,	0x0A,		0x00,		0x00,		0x00	},
	{ EX16_FCID,		E_SFCID_SREG_RD,	0x08,		0x00,		0x00,		0x00	},
	{ EX16_FCID,		E_SFCID_MAXSIZE_RD,	0x0A,		0x00,		0x00,		0x00	},
/*--------------------------------------------------------------------------------------------------*/
	{ EX16_FCID,		E_SFCID_REG_RD_SL,	0x0A,		0x06,		0x02,		0x01	},
	{ EX16_FCID,		E_SFCID_SREG_WR_SL,	0x0C,		0x00,		0x00,		0x00	},
	{ EX16_FCID,		E_SFCID_DCREG_RD_SL,0x0A,		0x06,		0x02,		0x01	},
	{ EX16_FCID,		E_SFCID_DCREG_WR_SL,0x0A,		0x00,		0x00,		0x00	},
	{ EX16_FCID,		E_SFCID_PREG_WR_SL,	0x0C,		0x00,		0x00,		0x00	},
	{ EX16_FCID,		E_SFCID_SREG_RD_SL,	0x0A,		0x00,		0x00,		0x00	},
	{ EX16_FCID,		E_SFCID_MAXSIZE_RD_SL,0x0C,		0x00,		0x00,		0x00	},
/*--------------------------------------------------------------------------------------------------*/
/*	  拡張コマンド(３２ビットアクセス用)登録テーブル												*/
/*--------------------------------------------------------------------------------------------------*/
	{ EX32_FCID,		E_SFCID_REG_RD,		0x08,		0x04,		0x02,		0x01	},
	{ EX32_FCID,		E_SFCID_SREG_WR,	0x0C,		0x00,		0x00,		0x00	},
	{ EX32_FCID,		E_SFCID_DCREG_RD,	0x08,		0x04,		0x02,		0x01	},
	{ EX32_FCID,		E_SFCID_DCREG_WR,	0x08,		0x00,		0x00,		0x00	},
	{ EX32_FCID,		E_SFCID_PREG_WR,	0x0C,		0x00,		0x00,		0x00	},
	{ EX32_FCID,		E_SFCID_SREG_RD,	0x08,		0x00,		0x00,		0x00	},
	{ EX32_FCID,		E_SFCID_MAXSIZE_RD,	0x0A,		0x00,		0x00,		0x00	},
	{ EX32_FCID,		E_SFCID_MEM_RD,		0x08,		0x04,		0x02,		0x02	},
	{ EX32_FCID,		E_SFCID_MEM_WR,		0x0C,		0x00,		0x00,		0x00	},
/*--------------------------------------------------------------------------------------------------*/
	{ EX32_FCID,		E_SFCID_REG_RD_SL,	0x0A,		0x06,		0x02,		0x01	},
	{ EX32_FCID,		E_SFCID_SREG_WR_SL,	0x0E,		0x00,		0x00,		0x00	},
	{ EX32_FCID,		E_SFCID_DCREG_RD_SL,0x0A,		0x06,		0x02,		0x01	},
	{ EX32_FCID,		E_SFCID_DCREG_WR_SL,0x0A,		0x00,		0x00,		0x00	},
	{ EX32_FCID,		E_SFCID_PREG_WR_SL,	0x0E,		0x00,		0x00,		0x00	},
	{ EX32_FCID,		E_SFCID_SREG_RD_SL,	0x0A,		0x00,		0x00,		0x00	},
	{ EX32_FCID,		E_SFCID_MAXSIZE_RD_SL,0x0C,		0x00,		0x00,		0x00	},
/*--------------------------------------------------------------------------------------------------*/
/*	  ダウンロードコマンド登録テーブル																*/
/*--------------------------------------------------------------------------------------------------*/
	{ S_FCID_SYSDL,		S_SFCID_DLID_RD,	0x14,		0x06,		0x01,		0x04	},
	{ S_FCID_SYSDL,		S_SFCID_SWVER_RD,	0x0E,		0x00,		0x00,		0x00	},
	{ S_FCID_SYSDL,		S_SFCID_DLWAY_RD,	0x0C,		0x00,		0x00,		0x00	},
	{ S_FCID_SYSDL,		S_SFCID_DL_START,	0x08,		0x00,		0x00,		0x00	},
	{ S_FCID_SYSDL,		S_SFCID_DL_PRGDEL,	0x0E,		0x00,		0x00,		0x00	},
	{ S_FCID_SYSDL,		S_SFCID_DL_PRGWR,	0x10,		0x00,		0x00,		0x00	},
	{ S_FCID_SYSDL,		S_SFCID_DL_DELSND,	0x0E,		0x00,		0x00,		0x00	},
	{ S_FCID_SYSDL,		S_SFCID_DL_PRGSND,	0x10,		0x00,		0x00,		0x00	},
	{ S_FCID_SYSDL,		S_SFCID_DL_PRGWRA,	0x08,		0x00,		0x00,		0x00	},
	{ S_FCID_SYSDL,		S_SFCID_DL_END,		0x08,		0x00,		0x00,		0x00	},
	{ S_FCID_SYSDL,		S_SFCID_DL_PRGRD,	0x10,		0x08,		0x02,		0x01	},
	{ S_FCID_SYSDL,		S_SFCID_DL_REBOOT,	0x08,		0x00,		0x00,		0x00	},
	{ S_FCID_SYSDL,		S_SFCID_DL_SIFCHG,	0x0C,		0x00,		0x00,		0x00	},
	{ S_FCID_SYSDL,		S_SFCID_DL_HIS_RD,	0x0A,		0x00,		0x00,		0x00	},
	{ S_FCID_SYSDL,		S_SFCID_DL_HIS_WR,	0x08,		0x00,		0x00,		0x00	},
	/* Delimiter Code.  Don't Change!! */
	{ 0xFF,				0xFFFF,				0xFF,		0xFF,		0xFF,		0xFF	}
};





/****************************************************************************************************/
/*																									*/
/*		通信開始後２回目のメッセージ長解析															*/
/*																									*/
/*			メッセージ長を確定可能な部分まで受信した後この関数をCALLします。						*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/****************************************************************************************************/
static USHORT	opMsg2ndAnalyze(const UCHAR *pRcvBuf, UCHAR TblIdx)
{
	LONG  pos; 
	ULONG  AltSz = 0;
	const MBUSRSP_TBL *tblptr = &MbusRspTbl[TblIdx];

	pos = (LONG)tblptr->AltSzPos;
	switch(tblptr->AltSzLen)
	{
	case 1: AltSz = pRcvBuf[pos]; 
		break;
	case 2: AltSz = ((ULONG)pRcvBuf[pos]<<8) | (ULONG)pRcvBuf[pos+1]; 
		break;
	case 4: AltSz =  ((ULONG)pRcvBuf[pos]<<24) | ((ULONG)pRcvBuf[pos+1]<<16)
				   | ((ULONG)pRcvBuf[pos+2]<< 8) | ((ULONG)pRcvBuf[pos+3]); 
		break;
	default: break;
	}

	return ((USHORT)(tblptr->FixPktSz + (AltSz * tblptr->AltDataSz)));
}


/****************************************************************************************************/
/*																									*/
/*		MEMOBUS通信開始後初回のメッセージ長解析														*/
/*																									*/
/*			MEMOBUSは可変長メッセージであるが、SigmaMessageとは異なり直接メッセージ長を表すデータが	*/
/*			ない。従って、受信途中でパケットタイプやデータ数によりメッセージ長(受信データ長)を計算	*/
/*			する必要がある。																		*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*		(1)サポートコマンドかどうかの判定															*/
/*		(2)メッセージ長(受信データ長)の計算															*/
/*--------------------------------------------------------------------------------------------------*/
/****************************************************************************************************/
static USHORT	opMsg1stAnalyze(const UCHAR *pRcvBuf, UCHAR *TblIdx, UCHAR *ConfBLen)
{
	USHORT	i, rc = 0; 
	UCHAR	code = pRcvBuf[1]; 
	USHORT	subcode = (((USHORT)pRcvBuf[2]<<8) | (USHORT)pRcvBuf[3]);
	const MBUSRSP_TBL *tblptr = MbusRspTbl;

	*ConfBLen = 0;	/* 受信メッセージ長定義箇所のバイト位置		*/
	*TblIdx = 0;	/* MEMOBUSテーブルアクセスインデックス		*/
/*--------------------------------------------------------------------------------------------------*/
/*	ファンクションサブコードの検索																	*/
/*--------------------------------------------------------------------------------------------------*/
	rc = MBUS_MAXSIZE;										/* 受信メッセージ長解析未完セット		*/
	for( i = 0 ; (i < MCOM_MAX_REGIST) && (tblptr[i].FuncCode != 0xff) ; i++ )
	{
		if( (( code == STD_FCID )&&( tblptr[i].FuncCode == code )) 
			|| (( tblptr[i].FuncCode == code )&&( tblptr[i].FuncSubCode == subcode )) )
		{/*	該当コード発見時の処理																	*/

			if(tblptr[i].AltDataSz != 0)
			{/*	可変長パケットの場合																*/
				*ConfBLen = tblptr[i].AltSzPos + tblptr[i].AltSzLen;
				rc = 0;										/* 受信メッセージ長解析未完セット		*/
			}
			else
			{/*	固定長パケットの場合																*/
				rc = tblptr[i]. FixPktSz;					/* 受信メッセージ長記憶					*/
			}
			break;											/* 検索処理完了につき、LOOP終了			*/
		}
	}
	*TblIdx = (UCHAR)i;										/* MEMOBUSテーブルアクセスインデックス		*/
/*--------------------------------------------------------------------------------------------------*/
/*	ファンクションエラー																			*/
/*--------------------------------------------------------------------------------------------------*/
	if((code & 0x80) != 0)
	{
		rc = 5;
	}

	return( rc );
}







