/****************************************************************************************************/
/*																									*/
/*																									*/
/*		SigmaMemobus.c : Memobus I/F Module															*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : メモバスメッセージ処理(Message解析,Read/WriteRegisters)を行う。							*/
/*																									*/
/*			 1) Loopback Test																		*/
/*			 2) System Download																		*/
/*			 3) Read  One Register																	*/
/*			 4) Write One Register																	*/
/*			 5) Read  Sequential Registers															*/
/*			 6) Write Sequential Registers															*/
/*			 7) Read  Random     Registers															*/
/*			 8) Write Random     Registers															*/
/*			 9) Others																				*/
/*																									*/
/*	注 意 : 本モジュールは、複数箇所から参照される。												*/
/*			本モジュール内で、静的変数の定義は行わないこと。										*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.1.00 : 2008.02.08  T.Taniguchi															*/
/*		Rev.1.01 : 2013.07.03  K.Ozaki																*/
/*		Rev.1.02 : 2013.09.26  K.Ozaki	<S00C>														*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include 	"KnlApi.h" 							//#include "Round.h"
#include	"Monitor.h"							/* Un Monitor */
#include	"Parameter.h"						/* Parameter */
#include 	"Operation.h"						/* Fn Function */
#include 	"Information.h"						/* Information */
#include	"UsbFunctionIF.h"					/* TODO: 後で消す */
#include	"Memory.h"							/* Data Trace */
#include	"FirmDownload.h"					/* FirmDownload(50h)	<S077>	*/
#include	"MemobusFunctions.h"				/* <S150> */

/* <S205>	Start */
/****************************************************************************************************/
/*																									*/
/*		VARIABLE																					*/
/*																									*/
/****************************************************************************************************/
/* 読み出しIDのレジスタアドレスTable */
const LONG IdRegAddTbl[MBUS_IDRD_SLOT_NUM][MBUS_IDRD_ID_NUM]=
{
						/* 	ID0(型式)	ID1(仕様)	ID2(仕様№)	ID3(シリアル№)	ID4(Soft Ver)	ID5(レジスタ Ver)	ID6(BTO №)	 */
/* 00:サーボパック */	{	0x80019000,	0x80018000,	0,			0x80019010,		0x8001800A,		0x80018200,			0x800191E0	},
/* 01:CMD */			{	0x800190C8,	0,			0,			0x800190E0,		0x80018082,		0,					0			},
/* 02:Safty */			{	0x800190FA,	0,			0,			0x80019112,		0x8001808A,		0,					0			},
/* 03:FB */				{	0x8001912C,	0,			0,			0x80019144,		0,				0,					0			},
/* 04:モータ */			{	0x80019022,	0,			0,			0x80019038,		0,				0,					0			},
/* 05:エンコーダ */		{	0x8001904A,	0,			0,			0x8001905A,		0x8001801C,		0,					0			}
};

/* 読み出しIDのデータサイズTable[byte] */
const CHAR IdDataSizeTbl[MBUS_IDRD_SLOT_NUM][MBUS_IDRD_ID_NUM]=
{
						/* 	ID0(型式)	ID1(仕様)	ID2(仕様№)	ID3(シリアル№)	ID4(Soft Ver)	ID5(レジスタ Ver)	ID6(BTO №)	 */
/* 00:サーボパック */	{	2,			2,			4,			2,				4,				4,					2			},
/* 01:CMD */			{	2,			0,			0,			2,				4,				0,					0			},
/* 02:Safty */			{	2,			0,			0,			2,				4,				0,					0			},
/* 03:FB */				{	2,			0,			0,			2,				0,				0,					0			},
/* 04:モータ */			{	2,			0,			0,			2,				0,				0,					0			},
/* 05:エンコーダ */		{	2,			0,			0,			2,				2,				0,					0			}
};

/* 読み出しIDのデータ数Table */
const CHAR IdDataNumTbl[MBUS_IDRD_SLOT_NUM][MBUS_IDRD_ID_NUM]=
{
						/* 	ID0(型式)	ID1(仕様)	ID2(仕様№)	ID3(シリアル№)	ID4(Soft Ver)	ID5(レジスタ Ver)	ID6(BTO №)	 */
/* 00:サーボパック */	{	8,			1,			1,			8,				1,				1,					32			},
/* 01:CMD */			{	8,			0,			0,			8,				1,				0,					0			},
/* 02:Safty */			{	12,			0,			0,			12,				1,				0,					0			},
/* 03:FB */				{	12,			0,			0,			12,				0,				0,					0			},
/* 04:モータ */			{	11,			0,			0,			8,				0,				0,					0			},
/* 05:エンコーダ */		{	8,			0,			0,			8,				1,				0,					0			}
};

/*  数字(0～9)のＡＳＣＩＩコードテーブル */
const	UCHAR	NumAsciiTbl[ ] =
		{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

/*  文字のＡＳＣＩＩコードテーブル */
const	UCHAR	StrAsciiTbl[ ] =
		{ 'F', 'E', 'Y'};
/* <S205>	End */

/****************************************************************************************************/
/*																									*/
/*		関数一覧																					*/
/*																									*/
/****************************************************************************************************/
extern SHORT	MbusFuncSystemDownLoad( HMSGIF Hmsg );	/*<S150>*/

/*--------------------------------------------------------------------------------------------------*/
/*		Rpx関数定義：関数名変更不可																	*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxMsgifMainProcedure( HMSGIF Hmsg );			/* MessageI/F：メイン処理					*/
void	RpxMsgifFinishProcedure( HMSGIF Hmsg );			/* MessageI/F：終了処理						*/
#if 0
LONG	RpxMsgifCheckRecvMessage( HMSGIF Hmsg );		/* MessageI/F：受信メッセージチェック		*/
#endif
LONG	RpxMsgifGetCmdMsgSubAddr( HMSGIF Hmsg );		/* MessageI/F：CmdMsgサブアドレス取得		*/
void	RpxMsgifSendErrResSubAddr( HMSGIF Hmsg );		/* MessageI/F：SubAddr異常応答送信処理		*/
void	RpxMsgifFuncExecResponse( HMSGIF Hmsg );		/* MessageI/F：補助機能実行結果応答処理		*/
/*--------------------------------------------------------------------------------------------------*/
/*		Rki関数定義：Round API Function for Kernel													*/
/*--------------------------------------------------------------------------------------------------*/
#if 0
LONG	RkiMsgifGetCmdMsgLength(						/* 指令メッセージ長取得						*/
		UCHAR	*CmdBuf,								/* Command Message Buffer					*/
		LONG	CmdBufLen	);							/* Command Message Buffer Length			*/
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		Lpx関数定義：関数名変更自由																	*/
/*--------------------------------------------------------------------------------------------------*/
void	LpxMbusLoopbackTest( HMSGIF Hmsg );				/* Memobus：ループバックテスト				*/
#if 0
void	LpxMbusSystemDownLoad( HMSGIF Hmsg );			/* Memobus：システムダウンロード			*/
#endif
#if 0	/* <S150> */
void	LpxMbusSysDLRes( HMSGIF Hmsg, SHORT dlSts );	/* Memobus：システムダウンロードレスポンス作成*/
#endif
/*--------------------------------------------------------------------------------------------------*/
void	LpxMbusReadOneRegister( HMSGIF Hmsg );			/* Memobus：単一レジスタ読出し				*/
void	LpxMbusWriteOneRegister( HMSGIF Hmsg );			/* Memobus：単一レジスタ書込み				*/
/*--------------------------------------------------------------------------------------------------*/
void	LpxMbusReadSeqRegisters( HMSGIF Hmsg );			/* Memobus：連続レジスタ読出し				*/
void	LpxMbusWriteSeqRegisters( HMSGIF Hmsg );		/* Memobus：連続レジスタ書込み				*/
/*--------------------------------------------------------------------------------------------------*/
void	LpxMbusReadRandomRegisters( HMSGIF Hmsg );		/* Memobus：非連続レジスタ読出し			*/
void	LpxMbusWriteRandomRegisters( HMSGIF Hmsg );		/* Memobus：非連続レジスタ書込み			*/
/*--------------------------------------------------------------------------------------------------*/
void	LpxMbusReadMaxNumberOfRegisters( HMSGIF Hmsg );	/* Memobus：最大レジスタ個数読出し			*/
/*--------------------------------------------------------------------------------------------------*/
void	LpxMbusReadSeqMemories( HMSGIF Hmsg );			/* Memobus：連続メモリ読出し				*/
void	LpxMbusWriteSeqMemories( HMSGIF Hmsg );			/* Memobus：連続メモリ書込み				*/
/*--------------------------------------------------------------------------------------------------*/
LONG	LpxGetMaxNumberOfRdRegisters( HMSGIF Hmsg, LONG FixedLen );
LONG	LpxGetMaxNumberOfWrRegisters( HMSGIF Hmsg, LONG FixedLen );
/*--------------------------------------------------------------------------------------------------*/
void	LpxMbusSendErrorResponse( HMSGIF Hmsg, ULONG ErrCode );
LONG	LpxRegifRegNoError( LONG RegNo, LONG Number, USHORT *RdBuf, HMSGIF Hmsg );
/*--------------------------------------------------------------------------------------------------*/
#if 0	/* <S150> */
void	LpxMbusTstUsbProcedure( HMSGIF Hmsg );			/* IngramのMemobus							*/
void	LpxMbusTstUartProcedure( HMSGIF Hmsg );			/* IngramのMemobus							*/
#endif	/* <S150> */
/****************************************************************************************************/
/* <S150> >>>>> */
void LpxMbusMechatrolink( HMSGIF Hmsg, LONG *MsgLen );	/* メカトロリンクコマンド					*/
void LpxMbusLink( HMSGIF Hmsg );						/* 中継メッセージファンクション				*/
static void	mbusFuncReadMemory42H( HMSGIF Hmsg, LONG SubFuncCode, LONG *TrnbLength );
static void	mbusFuncWriteMemory42H( HMSGIF Hmsg, LONG SubFuncCode, LONG *TrnbLength );
static void	mbusFuncReadMaxSize42H( HMSGIF Hmsg, LONG SubFuncCode, LONG *TrnbLength );
static void mbusFuncErrorManager42H( HMSGIF Hmsg, LONG ErrCode, ULONG ErrAddress, LONG *TrnbLength );

static LONG mbusFuncLongSwap( void *Buf );
static LONG mbusFuncShortSwap( void *Buf );

static void	mbusFuncSetArrayLongData( UCHAR *Buf, ULONG data, UINT AccessSize );
static void	mbusFuncSetArrayShortData( UCHAR *Buf, USHORT data, UINT AccessSize );

static LONG mbusFuncSetValueByteData( UCHAR *Buf, LONG BlockNum, LONG dIndex );
static LONG mbusFuncSetValueShortData( UCHAR *Buf, LONG BlockNum, LONG dIndex );
static LONG mbusFuncSetValueLongData( UCHAR *Buf, LONG BlockNum, LONG dIndex );

static REG_MANAGER_HANDLE *mbusFuncConvAddress(MEMOBUS_ADDR_MAP *AxisMap, ULONG *MemAddress );
static UCHAR mbusFuncCheckBlockAccess(UCHAR *CmdBuf, ULONG CurrentAddress, 
													UINT AccessSize, LONG BlockNum, LONG dIndex);
/* <<<<< <S150> */

/* <S205> Start */
/*--------------------------------------------------------------------------------------------------*/
/*	製品情報読み出し																				*/
/*--------------------------------------------------------------------------------------------------*/
static void mbusFuncReadProductInfo42H( HMSGIF Hmsg, LONG SubFuncCode, LONG *TrnbLength );
static void mbusFuncSetSpecialSpecIdData( HMSGIF Hmsg, LONG RspBuffOffset);
static LONG mbusFuncSetIdData( HMSGIF Hmsg, LONG ReadIdRegAdd, UCHAR ReadDataSize, UCHAR ReadDataNum, LONG RspBuffOffset, LONG *TrnbLength );
/*--------------------------------------------------------------------------------------------------*/
/* <S205> End */

/*<11J42> add start*/
#if 0 &&  CCSW_CMDID == CMDID_EXIST
void	MemobusRespTrnsStart( HMSGIF, USHORT, USHORT );	/*             送信開始設定					*/

ULONG	RpxSmsgTransOptRspMessage( HMSGIF );			/* オプションカード通信応答データ転送処理	*/
void	MemobusTransCmdOptCmdMsg( HMSGIF );				/* CmdOpt：コマンドデータ転送 (TOOL→OPT)	*/
ULONG	MemobusTransCmdOptRspMsg( HMSGIF );				/* CmdOpt：レスポンスデータ転送 (OPT→TOOL)	*/
#define	SMBUS_MPU_MASK		0xf0
#define	SMBUS_MPU0			0x00
#define	SMBUS_MPU1			0x10
#define	SMBUS_MPU2			0x20
#define	SMBUS_MPU3			0x30
#endif
/*<11J42> add end*/
/****************************************************************************************************/

/*--------------------------------------------------------------------------------------------------*/
/*		<S166>																						*/
/*--------------------------------------------------------------------------------------------------*/
static LONG mbusCheckOperationWriteHandle( REG_ACC_T *accessprm, MSGIF *Hmsg );
static void mbusFreeOperationWriteHandle( REG_ACC_T *accessprm, MSGIF *Hmsg );
static LONG	mbusGetOpeRegWriteHandle( OPACCSV *Accs, _enum_OPMODE mode, MSGIF *Hmsg );
static void	mbusFreeOpeRegWriteHandle( OPACCSV *Accs, _enum_OPMODE mode, MSGIF *Hmsg );
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		Parameter Register Read/Write																*/
/*--------------------------------------------------------------------------------------------------*/
LONG	LpxReadPrmRegister(					/* パラメータ読み出し(レジスタ指定)						*/
		LONG	RegNo,						/* レジスタ番号											*/
		LONG	Number,						/* 読出しデータ個数(WORD個数)							*/
		USHORT	*RdBuf,						/* 読出しデータバッファ									*/
		HMSGIF	Hmsg			);			/* 読出しIFハンドル										*/
LONG	LpxWritePrmRegister(				/* パラメータ書き込み処理(レジスタ指定)					*/
		LONG	RegNo,						/* レジスタ番号											*/
		LONG	Number,						/* 書き込みデータ個数(WORD個数)							*/
		USHORT	*RdBuf,						/* 書き込みデータバッファ								*/
		HMSGIF	Hmsg			);			/* 読出しIFハンドル										*/
/*--------------------------------------------------------------------------------------------------*/
/*		Common Object Register Read/Write															*/
/*--------------------------------------------------------------------------------------------------*/
LONG	LpxReadComObject(					/* パラメータ読み出し(レジスタ指定)						*/
		LONG	RegNo,						/* レジスタ番号											*/
		LONG	Number,						/* 読出しデータ個数(WORD個数)							*/
		USHORT	*RdBuf,						/* 読出しデータバッファ									*/
		HMSGIF	Hmsg			);			/* 読出しIFハンドル										*/
LONG	LpxWriteComObject(					/* パラメータ書き込み処理(レジスタ指定)					*/
		LONG	RegNo,						/* レジスタ番号											*/
		LONG	Number,						/* 書き込みデータ個数(WORD個数)							*/
		USHORT	*RdBuf,						/* 書き込みデータバッファ								*/
		HMSGIF	Hmsg			);			/* 読出しIFハンドル										*/
/*--------------------------------------------------------------------------------------------------*/
/*		Memory Object Register Read/Write															*/
/*--------------------------------------------------------------------------------------------------*/
LONG	LpxReadMemObject(					/* パラメータ読み出し(レジスタ指定)						*/
		LONG	RegNo,						/* レジスタ番号											*/
		LONG	Number,						/* 読出しデータ個数(WORD個数)							*/
		USHORT	*RdBuf,						/* 読出しデータバッファ									*/
		HMSGIF	Hmsg			);			/* 読出しIFハンドル										*/
LONG	LpxWriteMemObject(					/* パラメータ書き込み処理(レジスタ指定)					*/
		LONG	RegNo,						/* レジスタ番号											*/
		LONG	Number,						/* 書き込みデータ個数(WORD個数)							*/
		USHORT	*RdBuf,						/* 書き込みデータバッファ								*/
		HMSGIF	Hmsg			);			/* 読出しIFハンドル										*/



/****************************************************************************************************/
/*																									*/
/*		レジスタＩＦ関数テーブル																	*/
/*																									*/
/****************************************************************************************************/
#define	RFTBLIDX( RegNo )	((RegNo>>12) & 0x0F)			/* Get RegFuncTbl Index					*/
/*--------------------------------------------------------------------------------------------------*/
typedef	LONG	(*REGFUN)( LONG RegNo, LONG Number, USHORT *RdBuf, HMSGIF Hmsg );
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{
	REGFUN	ReadRegister;			/* Read  Register Function										*/
	REGFUN	WriteRegister;			/* Write Register Function										*/
//	UCHAR	MblkWchk;				/* @@ Multi-Block Write Check									*/
//	UCHAR	Spare;					/* @@ Spare														*/
	COBJECT	*pObject;				/* Pointer to the Register Object								*/
} REGFUNTBL;
/*--------------------------------------------------------------------------------------------------*/
const	REGFUNTBL	RegFuncTbl[16] = {
/*	ReadRegister			WriteRegister		 pObject 											*/
/*--------------------------------------------------------------------------------------------------*/
 {	LpxReadPrmRegister,		LpxWritePrmRegister, (COBJECT *)CPARAMX	},	/* 0 : TODO: Parameter Non Volatile		*/
 {	LpxReadPrmRegister,		LpxWritePrmRegister, (COBJECT *)CPARAMX	},	/* 1 : Parameter Volatile		*/
 {	LpxReadComObject,		LpxWriteComObject,	 (COBJECT *)COPEREG },	/* 2 : Operation Register	*/
 {	LpxRegifRegNoError,		LpxRegifRegNoError,	NULL },					/* 3 : TODO: Enginee.  Register	*/
 {	LpxReadMemObject,		LpxWriteMemObject,	(COBJECT *)CTRCMEM },	/* 4 : TODO: DataTrace Register	*/
 {	LpxReadMemObject,		LpxWriteMemObject,	(COBJECT *)CTRCMEM },	/* 5 : TODO: DataTrace Register	*/
 {	LpxReadMemObject,		LpxWriteMemObject,	(COBJECT *)CTRCMEM },	/* 6 : TODO: DataTrace Register	*/
 {	LpxReadMemObject,		LpxWriteMemObject,	(COBJECT *)CTRCMEM },	/* 7 : TODO: DataTrace Register	*/
 {	LpxRegifRegNoError,	    LpxRegifRegNoError,	NULL },					/* 8 : Reserved				*/
 {	LpxRegifRegNoError,	    LpxRegifRegNoError,	NULL },					/* 9 : Reserved				*/
 {	LpxRegifRegNoError,	    LpxRegifRegNoError,	NULL },					/* A : Reserved				*/
 {	LpxRegifRegNoError,	    LpxRegifRegNoError,	NULL },					/* B : Reserved				*/
 {	LpxReadComObject,		LpxRegifRegNoError,	(COBJECT *)CINFREG },	/* C : TypeInfo  Register	*/
 {	LpxRegifRegNoError,	    LpxRegifRegNoError,	NULL },					/* D : ParamDef  Register	*/
 {	LpxReadComObject,		LpxRegifRegNoError,	(COBJECT *)CUNREGX },	/* E : UnMonitor Register	*/
 {	LpxRegifRegNoError,		LpxRegifRegNoError,	NULL },					/* F : Parameter Download(JPDRV)	*/
#if 0
 {	RpxReadPrmRegister,		RpxWritePrmRegister,	0x00, 0x00, },	/* 0 : Parameter Normal			*/
 {	RpxReadPrmRegister,		RpxWritePrmRegister,	0x00, 0x00, },	/* 1 : Parameter Temp.			*/
 {	RpxReadOpeRegister,		RpxWriteOpeRegister,	0x01, 0x00, },	/* 2 : Operation Register		*/
 {	RpxReadEngRegister,		RpxWriteEngRegister,	0x00, 0x00, },	/* 3 : Enginee.  Register		*/
 {	RpxReadTraceRegister,	RpxWriteTraceRegister,	0x00, 0x00, },	/* 4 : DataTrace Register		*/
 {	RpxReadTraceRegister,	RpxWriteTraceRegister,	0x00, 0x00, },	/* 5 : DataTrace Register		*/
 {	RpxReadTraceRegister,	RpxWriteTraceRegister,	0x00, 0x00, },	/* 6 : DataTrace Register		*/
 {	RpxReadTraceRegister,	RpxWriteTraceRegister,	0x00, 0x00, },	/* 7 : DataTrace Register		*/
 {	LpxRegifRegNoError,	    LpxRegifRegNoError,		0x00, 0x00, },	/* 8 : Reserved					*/
 {	LpxRegifRegNoError,	    LpxRegifRegNoError,		0x00, 0x00, },	/* 9 : Reserved					*/
 {	LpxRegifRegNoError,	    LpxRegifRegNoError,		0x00, 0x00, },	/* A : Reserved					*/
 {	LpxRegifRegNoError,	    LpxRegifRegNoError,		0x00, 0x00, },	/* B : Reserved					*/
 {	RpxReadTyInfRegister,	LpxRegifRegNoError,		0x00, 0x00, },	/* C : TypeInfo  Register		*/
 {	LpxRegifRegNoError,	    LpxRegifRegNoError,		0x00, 0x00, },	/* D : ParamDef  Register		*/
 {	RpxReadMonRegister,		LpxRegifRegNoError,		0x00, 0x00, },	/* E : UnMonitor Register		*/
 {	RpxReadPdlRegister,		RpxWritePdlRegister,	0x00, 0x00, },	/* F : Parameter Download		*/
#endif
};
/*--------------------------------------------------------------------------------------------------*/
#define Lpx_GetObject( regno )   (RegFuncTbl[ (regno >> 12) & 0x0F ].pObject)
#define OBJ_WRITE( obj, idx, data, hmsg ) ((obj)->Write( (obj), (idx), (data), hmsg ))
#define OBJ_WRITE_SUB( obj, idx, subidx, data, hmsg ) ((obj)->WriteS( (obj), (idx), (subidx), (data), hmsg ))
#define OBJ_READ( obj, idx, pdata, hmsg ) ((obj)->Read( (obj), (idx), (pdata), MCHK_REG_READ, hmsg ))  /* <S073> */
#define OBJ_READ_SUB( obj, idx, subidx, pdata, hmsg ) ((obj)->ReadS( (obj), (idx), (subidx), (pdata), MCHK_REG_READ, hmsg ))  /* <S073> */
#define OBJ_SEARCH( obj, num, pidx, hmsg ) ((obj)->Search( (obj), (num), 0, pidx, MCHK_REG_READ, hmsg ))
#define OBJ_SEARCH_SUB( obj, num, pidx, psubidx, hmsg )  \
		((obj)->SearchS( (obj), (num), 0, pidx, psubidx, MCHK_REG_READ, hmsg ))  /* <S073> */
/*--------------------------------------------------------------------------------------------------*/
#if		CCSW_BIG_ENDIAN == 1
#define GET_LWD_REGDATA( xlong )		(USHORT)((xlong)>>0 )
#define GET_HWD_REGDATA( xlong )		(USHORT)((xlong)>>16)
#define ULONGOF_REGDATA( lwd, hwd )		(ULONG)((((USHORT)(hwd))<<16) + (USHORT)(lwd))
#else
#define GET_LWD_REGDATA( xlong )		(USHORT)((xlong)>>0 )
#define GET_HWD_REGDATA( xlong )		(USHORT)((xlong)>>16)
#define ULONGOF_REGDATA( lwd, hwd )		(ULONG)((((USHORT)(hwd))<<16) + (USHORT)(lwd))
#endif

/****************************************************************************************************/
/*																									*/
/*		Define Macros, etc																			*/
/*																									*/
/****************************************************************************************************/
#define LPX_CHK_SLOT( SfcH )			(SfcH == 0x01)
#define LPX_GET_SLOT_BYTELEN( SfcH )	((LPX_CHK_SLOT( SfcH ))? 2 : 0)
#define LPX_GET_SLOT_WORDLEN( SfcH )	((LPX_CHK_SLOT( SfcH ))? 1 : 0)
/*--------------------------------------------------------------------------------------------------*/
/*	Test Macro																						*/
/*--------------------------------------------------------------------------------------------------*/
#undef MlibCnvBig16
#define MlibCnvBig16( x ) ( (__rev( x )>>16) )  /* ARMv5はREV16命令がない */
#define MSGIF_SEND( resbuf, reslen )   ( Hmsg->Sif->StxRes( Hmsg->Sif, (resbuf), (reslen) ) )

/*--------------------------------------------------------------------------------------------------*/
#define Lpx_GetMaxSubIdx( tbl, idx )  /* <S073> */\
			( (GET_OBJDEF( tbl, idx )->DataLength * GET_OBJDEF( tbl, idx )->NumberOfArray)>>1)


/****************************************************************************************************/
/*																									*/
/*		MessageI/F : Main Procedure																	*/
/*																									*/
/****************************************************************************************************/
void	RpxMsgifMainProcedure( HMSGIF Hmsg )
{
		LONG	MsgLen;									/* <S150> */
/*--------------------------------------------------------------------------------------------------*/
/*		Reset Hmsg Variables																		*/
/*--------------------------------------------------------------------------------------------------*/
		Hmsg->FunSts    = 0;							/* 補助機能実行結果							*/
		Hmsg->LstErrRno = Hmsg->ErrRno;					/* 異常レジスタ番号前回値					*/
		Hmsg->ErrRno    = 0xFFFF;						/* 異常レジスタ番号リセット					*/
/*--------------------------------------------------------------------------------------------------*/
/*		Check Function Code and Call the Function													*/
/*--------------------------------------------------------------------------------------------------*/
		switch( Hmsg->CmdBuf[1] )
		{
		case MFC_LOOPBACK:
			 LpxMbusLoopbackTest( Hmsg );
			 break;
		case MFC_SVXMSG16:
#if 0 && CCSW_CMDID == CMDID_EXIST		/*<11J42>*/
			 if( (Hmsg->CmdBuf[2] == 0x01) && ((Hmsg->CmdBuf[4] & SMBUS_MPU_MASK) == SMBUS_MPU1))
			 {
				switch( Hmsg->CmdBuf[3] )
				{
				case SFC_RD_ONEREG:
				case SFC_WR_ONEREG:
				case SFC_RD_SEQREG:
				case SFC_WR_SEQREG:
				case SFC_RD_RANREG:
				case SFC_WR_RANREG:
				case SFC_RD_MAXNUM:
					MemobusTransCmdOptCmdMsg( Hmsg );
					break;
				default: LpxMbusSendErrorResponse( Hmsg, SMBUS_EC_FNCDERR ); break;
				}
				break;
			 }
#endif
			 switch( Hmsg->CmdBuf[3] )
			 {
			 case SFC_RD_ONEREG: LpxMbusReadOneRegister( Hmsg ); break;
			 case SFC_WR_ONEREG: LpxMbusWriteOneRegister( Hmsg ); break;
			 case SFC_RD_SEQREG: LpxMbusReadSeqRegisters( Hmsg ); break;
			 case SFC_WR_SEQREG: LpxMbusWriteSeqRegisters( Hmsg ); break;
			 case SFC_RD_RANREG: LpxMbusReadRandomRegisters( Hmsg ); break;
			 case SFC_WR_RANREG: LpxMbusWriteRandomRegisters( Hmsg ); break;
			 case SFC_RD_MAXNUM: LpxMbusReadMaxNumberOfRegisters( Hmsg ); break;
			 default: LpxMbusSendErrorResponse( Hmsg, SMBUS_EC_FNCDERR ); break;
			 }
			 break;
		case MFC_SVXMSG32:
			 switch( Hmsg->CmdBuf[3] )
		 	 {
			 case SFC_RD_SEQMEM: LpxMbusReadSeqMemories( Hmsg ); break;
			 case SFC_WR_SEQMEM: LpxMbusWriteSeqMemories( Hmsg ); break;
			 default: LpxMbusSendErrorResponse( Hmsg, SMBUS_EC_FNCDERR ); break;
			 }
			 break;
		case MFC_MECHATROLINK:
#if 0	/*<S150>*/
			 if( Hmsg->f.UsbMsgIF ) { LpxMbusTstUsbProcedure( Hmsg ); }
			 else { LpxMbusTstUartProcedure( Hmsg ); /* TODO: */ }
#else
			 LpxMbusMechatrolink( Hmsg, &MsgLen );
			 /* データ送信 */
			 MSGIF_SEND( Hmsg->ResBuf, MsgLen );
#endif
			 break;
//#if (SVFSEL_SWDOWNLOAD != 0)
		case MFC_SYSDWNLD:
#if 0	/*<S150>*/
//			 LpxMbusSystemDownLoad( Hmsg );
			{
				MEMOBUS_IF *MbusIf;
				SHORT dlSts;
				MbusIf = (MEMOBUS_IF*)MbusFuncSysDLHmsg2MbusIf(Hmsg);
				dlSts = MbusFuncSystemDownLoad(MbusIf);
//				MbusFuncSysDLMbusIf2Hmsg(Hmsg);
				if( dlSts != MBUS_BUSY )		/* BUSY以外であれば応答を返す */
				{
//<S073>			LpxMbusSysDLRes( Hmsg, dlSts );
					MlibSetCRC16MB( Hmsg->ResBuf, MbusIf->TrnbLength/* <S073> <S077> */ );
					MSGIF_SEND( Hmsg->ResBuf, MbusIf->TrnbLength/*@@ CRC*/ );			/* <S073> <S077> */
				}
			}
#else
			 MbusFuncSystemDownLoad( Hmsg );
#endif
			 break;
//#endif
		case MFC_RELAY:
#if 0	/*<S150>*/
			 if( Hmsg->f.UsbMsgIF ) { LpxMbusTstUsbProcedure( Hmsg ); }
			 else{ LpxMbusSendErrorResponse( Hmsg, SMBUS_EC_FNCDERR ); /*TODO:*/ }
#else
			 LpxMbusLink( Hmsg );
#endif
			 break;

		default:
			 LpxMbusSendErrorResponse( Hmsg, SMBUS_EC_FNCDERR );
			 break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		MessageI/F : 指令メッセージ長取得(for Kernel\Driver\SifMbus.c)								*/
/*																									*/
/****************************************************************************************************/
#define LPX_GET_CMDMSGLEN( i, Mul, Add )\
		if( CmdBufLen < (i+2) ){ return( 0 );}\
		else{ return( (((CmdBuf[i]<<8) + CmdBuf[i+1]) * Mul) + Add );}
/*--------------------------------------------------------------------------------------------------*/
#if 0
LONG	RkiMsgifGetCmdMsgLength( UCHAR *CmdBuf, LONG CmdBufLen )
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
			 /* unreachable return( -1 ); */
		case MFC_SVXMSG32:
			 switch( CmdBuf[3] )
			 {
			 case SFC_RD_SEQMEM: return( 12 + s );
			 case SFC_WR_SEQMEM: LPX_GET_CMDMSGLEN( 8 + s, 2, 12 + s );
			 default: return( -1 );
			 }
			 /* unreachable return( -1 ); */
		case MFC_SYSDWNLD:								/* @@@ Add Later @@@						*/
			 return( -1 );
		default:
			 if( KPI_CHK_OPEMSG( CmdBuf[1] ) )
			 {
				return( CmdBuf[2] + (CmdBuf[3]<<8) );
			 }
			 return( -1 );
		}
/*--------------------------------------------------------------------------------------------------*/
		/* unreachable return( -1 ); */									/* Unknown Function Code					*/
}
#endif



/****************************************************************************************************/
/*																									*/
/*		MessageI/F : メッセージＩＦ終了処理															*/
/*																									*/
/****************************************************************************************************/
void	RpxMsgifFinishProcedure( HMSGIF Hmsg )
{
		RpxFreeOpeRegWriteHandleAll( Hmsg );	/* <S166> 操作レジスタ書込み権強制解放処理			*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		MessageI/F : 受信メッセージチェック															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 受信メッセージのチェックを行う。														*/
/*																									*/
/*			1) 受信メッセージ長のチェック															*/
/*			2) 受信メッセージのサムチェック															*/
/*																									*/
/*			注) 軸アドレスのチェックは、通信ドライバで行う。										*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#if 0
LONG	RpxMsgifCheckRecvMessage( HMSGIF Hmsg )
{
LONG	MsgLen = Hmsg->CmdMsgLen;
/*--------------------------------------------------------------------------------------------------*/
/*		SigmaMessage (LCD-Operater)																	*/
/*--------------------------------------------------------------------------------------------------*/
		if( KPI_CHK_OPEMSG( Hmsg->CmdBuf[1] ) )
		{
			if( MsgLen & 0x0001 ){ return( NG );}
			return( MlibChkSum16SM( Hmsg->CmdBuf, MsgLen ) );
		}
/*--------------------------------------------------------------------------------------------------*/
/*		SigmaMemobus (PC, etc)																		*/
/*--------------------------------------------------------------------------------------------------*/
		else
		{
			return( MlibChkCRC16MB( Hmsg->CmdBuf, MsgLen ) );
		}
}
#endif


/****************************************************************************************************/
/*																									*/
/*		MessageI/F : CmdMsgサブアドレス取得															*/
/*																									*/
/****************************************************************************************************/
LONG	RpxMsgifGetCmdMsgSubAddr( HMSGIF Hmsg )
{
UCHAR	*CmdBuf = Hmsg->CmdBuf;
#ifdef MCCSW_JPDRVMX
#define CSUBADDR_IDX (4)			/* Jupiter Multi Axis Drive : CmdBuf[4] */
#define CSUBADDR_OFS (0)			/* 01H: 1st axis / 02H: 2nd axis */
#else
#define CSUBADDR_IDX (5)			/* Mercury : CmdBuf[5] */
#define CSUBADDR_OFS (1)			/* 00H: 1st axis / 01H: 2nd axis */
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		Check Function Code																			*/
/*--------------------------------------------------------------------------------------------------*/
		switch( CmdBuf[1] )
		{
		case MFC_LOOPBACK:
			 return( -1 );
		case MFC_SVXMSG16:
			 if( CmdBuf[2] == 0x01 ){ return( CmdBuf[CSUBADDR_IDX] + CSUBADDR_OFS );}
			 break;
		case MFC_SVXMSG32:
			 if( CmdBuf[2] == 0x01 ){ return( CmdBuf[CSUBADDR_IDX] + CSUBADDR_OFS );}
			 break;
		case MFC_SYSDWNLD:
			 if( CmdBuf[2] == 0x01 ){ return( CmdBuf[CSUBADDR_IDX] + CSUBADDR_OFS );}
			 break;
		default:
			 break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return( -2 );
}



/****************************************************************************************************/
/*																									*/
/*		MessageI/F : SubAddr異常応答送信処理														*/
/*																									*/
/****************************************************************************************************/
void	RpxMsgifSendErrResSubAddr( HMSGIF Hmsg )
{
		LpxMbusSendErrorResponse( Hmsg, SMBUS_SEC_ACCSERR );
		return;
}



/****************************************************************************************************/
/*																									*/
/*		MessageI/F : 補助機能実行結果応答処理														*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 補助機能実行の指令メッセージの場合、補助機能の実行が終了するまでは、処理完了か			*/
/*			処理異常が判断できないため、すぐには、応答メッセージが作成できない。					*/
/*			本関数は、補助機能の実行が終了した時点で、補助機能管理から呼び出され、補助機能			*/
/*			実行結果を基に応答メッセージを作成する。												*/
/*																									*/
/****************************************************************************************************/
void	RpxMsgifFuncExecResponse( HMSGIF Hmsg )
{
LONG	i,x;
LONG	ResLen;
USHORT	*wCmdBuf = (USHORT *)(Hmsg->CmdBuf);
USHORT	*wResBuf = (USHORT *)(Hmsg->ResBuf);
/*--------------------------------------------------------------------------------------------------*/
/*		補助機能実行処理異常時																		*/
/*--------------------------------------------------------------------------------------------------*/
		if( (Hmsg->FunSts != REGIF_COMPLETE) && (Hmsg->FunSts != REGIF_CONTINUE) )
		{
			LpxMbusSendErrorResponse( Hmsg, Hmsg->FunSts );
			return;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		応答メッセージバイト数計算																	*/
/*--------------------------------------------------------------------------------------------------*/
		ResLen = (Hmsg->CmdBuf[1] == MFC_SVXMSG16)? 10 : 12;
		ResLen = ResLen + LPX_GET_SLOT_BYTELEN( Hmsg->CmdBuf[2] );
/*--------------------------------------------------------------------------------------------------*/
/*		レジスタ書込み完了応答作成																	*/
/*--------------------------------------------------------------------------------------------------*/
		for( x=(ResLen>>1)-1, i=0 ; i < x ; i++ )
		{
			wResBuf[i] = wCmdBuf[i];					/* Set Response Data						*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		CRC-16作成＆応答データ送信指令																*/
/*--------------------------------------------------------------------------------------------------*/
		MlibSetCRC16MB(  Hmsg->ResBuf, ResLen );		/* Set CRC-16 to ResBuf						*/
		MSGIF_SEND( Hmsg->ResBuf, ResLen );		/* Start Transmit Response Message			*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		Memobus：ループバックテスト																	*/
/*																									*/
/****************************************************************************************************/
void	LpxMbusLoopbackTest( HMSGIF Hmsg )
{
ULONG	i;
USHORT	*wCmdBuf = (USHORT *)(Hmsg->CmdBuf);
USHORT	*wResBuf = (USHORT *)(Hmsg->ResBuf);
/*--------------------------------------------------------------------------------------------------*/
/*		応答データ作成																				*/
/*--------------------------------------------------------------------------------------------------*/
		for( i=0; i < (8/2); i++ )
		{
			wResBuf[i] = wCmdBuf[i];					/* Set LoopBack Response					*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		CRC-16作成＆応答データ送信指令																*/
/*--------------------------------------------------------------------------------------------------*/
/*		MlibSetCRC16MB(  Hmsg->ResBuf, 8 ); */			/* CRC-16の計算は不要						*/
		MSGIF_SEND( Hmsg->ResBuf, 8 );				/* Start Transmit Response Message			*/
		return;
}


#if 0	/* <S150> */
#if (SVFSEL_SWDOWNLOAD != 0)
/****************************************************************************************************/
/*																									*/
/*		Memobus：システムダウンロード																*/
/*																									*/
/****************************************************************************************************/
void	LpxMbusSystemDownLoad( HMSGIF Hmsg )
{

LONG	errorFlg = FALSE;
USHORT	*CmdBufW = (USHORT *)(Hmsg->CmdBuf);
USHORT	*ResBufW = (USHORT *)(Hmsg->ResBuf);
USHORT	usDmy=1;
USHORT	TrnsLen;
//Jpdrv	MBUSFUNCTBL		MbusFunc = MbusFuncTbl[ FuncCodeIdx ];
USHORT	SubFuncCode = (((UCHAR)Hmsg->CmdBuf[2] << 8) | (UCHAR)Hmsg->CmdBuf[3]);
/*--------------------------------------------------------------------------------------------------*/
/*		ダウンロードコマンド解析＆応答																*/
/*--------------------------------------------------------------------------------------------------*/
		switch ( SubFuncCode & 0x00FF )
		{
		/*------------------------------------------------------------------------------------------*/
		/*		ダウンロード条件取得																*/
		/*------------------------------------------------------------------------------------------*/
		  case	SFC_DLID_RD :
				usDmy	= KsysP.SysDlFlushNum;					/* Flash数							*/
				ResBufW[0]	= CmdBufW[0];						/* AxisAdr, FunctionCode			*/
				ResBufW[1]	= CmdBufW[1];						/* SubFunctionCode					*/
				ResBufW[2]	= CmdBufW[2];						/* MPU番号							*/
				ResBufW[3]	= (USHORT)MlibByteSwap16( (LONG )KsysP.SysDlFlushNum );	/* Flush数		*/
				ResBufW[4]	= (USHORT)MlibByteSwap16( (LONG )KsysP.SysDlSts1 );	/* DownLoad Sts1	*/
				ResBufW[5]	= (USHORT)MlibByteSwap16( (LONG )KsysP.SysDlSts2 );	/* DownLoad Sts2	*/
				ResBufW[6]	= (USHORT)MlibByteSwap16( (LONG )KsysP.SysDlSts3 );	/* DownLoad Sts3	*/
				ResBufW[7]	= (USHORT)MlibByteSwap16( (LONG )KsysP.SysDlSts4 );	/* DownLoad Sts4	*/
				ResBufW[8]	= (USHORT)MlibByteSwap16( (LONG )KsysP.DriveID   );	/* ServoID			*/
				ResBufW[9]	= (USHORT)MlibByteSwap16( (LONG )BootSw.ExtRomSizeKB );	/* Flash #0 Manu ID	*/
				ResBufW[10]	= 0;								/* Flash #0 Device ID				*/
				TrnsLen = 24;
				break;
		/*------------------------------------------------------------------------------------------*/
		/*		ソフトバージョン読出し																*/
		/*------------------------------------------------------------------------------------------*/
		  case	SFC_SWVER_RD :
				ResBufW[0] = CmdBufW[0];						/* AxisAdr, FunctionCode			*/
				ResBufW[1] = CmdBufW[1];						/* SubFunctionCode					*/
				ResBufW[2] = CmdBufW[2];						/* MPU番号							*/
				ResBufW[3] = 0x0200;							/* Ver定義個数=2					*/
				ResBufW[4] = (USHORT)MlibByteSwap16((LONG )KsysP.Bt2MswVersion );	/* Boot2 Ver	*/
				ResBufW[5] = (USHORT)MlibByteSwap16((LONG )DrvSw.MswVersion ); 		/* Runtime Ver	*/
				TrnsLen = 14;
				break;
		/*------------------------------------------------------------------------------------------*/
		/*		ダウンロード条件取得																*/
		/*------------------------------------------------------------------------------------------*/
		  case	SFC_DLWAY_RD :	/* ダウンロード書込み条件取得 */
				ResBufW[0] = CmdBufW[0];						/* AxisAdr, FunctionCode			*/
				ResBufW[1] = CmdBufW[1];						/* SubFunctionCode					*/
				ResBufW[2] = CmdBufW[2];						/* MPU番号							*/
				ResBufW[3] = (USHORT)MlibByteSwap16((LONG )KsysP.SysDlWay );	/* ダウンロード方法	*/
				ResBufW[4] = (USHORT)MlibByteSwap16((LONG )KsysP.SysDlPrgbLen );/* 1MsgPRG書込ｻｲｽﾞ	*/
				TrnsLen = 12;
				break;
		/*------------------------------------------------------------------------------------------*/
		/*		ダウンロード開始要求																*/
		/*------------------------------------------------------------------------------------------*/
		  case	SFC_DL_START :	/* ダウンロード開始要求 */
				RpxSetDlMode( Hmsg );							/* ダウンロード開始					*/
				/*----------------------------------------------------------------------------------*/
				/* 開始したらここへは戻ってこない.開始出来ない場合に異常応答(整合性異常) 			*/
				/*----------------------------------------------------------------------------------*/
				LpxMbusSendErrorResponse( Hmsg, REGIF_DAMTHERR2 );
				errorFlg = TRUE;
				break;
		/*------------------------------------------------------------------------------------------*/
		/*		ダウンロード履歴読出し																*/
		/*------------------------------------------------------------------------------------------*/
		  case	SFC_DL_HIS_RD :
				ResBufW[0] = CmdBufW[0];						/* AxisAdr, FunctionCode			*/
				ResBufW[1] = CmdBufW[1];						/* SubFunctionCode					*/
				ResBufW[2] = CmdBufW[2];						/* MPU番号							*/
				ResBufW[3] = (USHORT)MlibByteSwap16((LONG )KsysP.SwUpDateHis );	/* ダウンロード履歴	*/
				TrnsLen = 10;
				break;
		/*------------------------------------------------------------------------------------------*/
		/*		未サポートサブファンクション														*/
		/*		ダウンロード開始要求以降のみ有効なコマンド											*/
		/*			→条件エラー																	*/
		/*------------------------------------------------------------------------------------------*/
		  default:
				LpxMbusSendErrorResponse( Hmsg, REGIF_DAMTHERR2 );
				errorFlg = TRUE;
				break;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		正常応答送信																				*/
/*--------------------------------------------------------------------------------------------------*/
		if( errorFlg == FALSE )
		{
		/*------------------------------------------------------------------------------------------*/
		/*	CRC-16作成＆応答データ送信指令															*/
		/*------------------------------------------------------------------------------------------*/
			MlibSetCRC16MB( Hmsg->ResBuf, TrnsLen );
			MSGIF_SEND( Hmsg->ResBuf, TrnsLen );			/* Start Transmit Response Message	*/
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}
#else
/****************************************************************************************************/
/*																									*/
/*		Memobus：システムダウンロード																*/
/*																									*/
/****************************************************************************************************/
void	LpxMbusSysDLRes( HMSGIF Hmsg, SHORT dlSts )
{

LONG	errorFlg = FALSE;
USHORT	*CmdBufW = (USHORT *)(Hmsg->CmdBuf);
USHORT	*ResBufW = (USHORT *)(Hmsg->ResBuf);
USHORT	TrnsLen;
USHORT	SubFuncCode = (((UCHAR)Hmsg->CmdBuf[2] << 8) | (UCHAR)Hmsg->CmdBuf[3]);

/*--------------------------------------------------------------------------------------------------*/
/*		ダウンロードコマンド解析＆応答																*/
/*--------------------------------------------------------------------------------------------------*/
		switch ( SubFuncCode & 0x00FF )
		{
		/*------------------------------------------------------------------------------------------*/
		/*		ﾌﾞﾛｯｸ消去															*/
		/*------------------------------------------------------------------------------------------*/
		  case	SFC_DL_PRGDEL :
				ResBufW[0]	= CmdBufW[0];						/* AxisAdr, FunctionCode			*/
				ResBufW[1]	= CmdBufW[1];						/* SubFunctionCode					*/
				ResBufW[2]	= CmdBufW[2];						/* MPU番号							*/
				ResBufW[3]	= CmdBufW[3];						/* Flush番号		*/
				ResBufW[4]	= CmdBufW[4];						/* 書込みアドレス上位ワード	*/
				ResBufW[5]	= CmdBufW[5];						/* 書込みアドレス下位ワード	*/
				if(dlSts == 0)
				{
					TrnsLen = 14;
				}
				else
				{
					LpxMbusSendErrorResponse( Hmsg, REGIF_DAMTHERR2 );
					errorFlg = TRUE;
				}
				break;
		/*------------------------------------------------------------------------------------------*/
		/*		ﾌﾟﾛｸﾞﾗﾑ書込み																*/
		/*------------------------------------------------------------------------------------------*/
		  case	SFC_DL_PRGWR :
				ResBufW[0]	= CmdBufW[0];						/* AxisAdr, FunctionCode			*/
				ResBufW[1]	= CmdBufW[1];						/* SubFunctionCode					*/
				ResBufW[2]	= CmdBufW[2];						/* MPU番号							*/
				ResBufW[3]	= CmdBufW[3];						/* Flush番号		*/
				ResBufW[4]	= CmdBufW[4];						/* データ数	*/
				ResBufW[5]	= CmdBufW[5];						/* 書込みアドレス上位ワード	*/
				ResBufW[6]	= CmdBufW[6];						/* 書込みアドレス下位ワード	*/
				if(dlSts == 0)
				{
					TrnsLen = 16;
				}
				else
				{
					LpxMbusSendErrorResponse( Hmsg, REGIF_DAMTHERR2 );
					errorFlg = TRUE;
				}
				break;
		/*------------------------------------------------------------------------------------------*/
		/*		ｼﾘｱﾙＩＦ速度変更				<S041>											*/
		/*------------------------------------------------------------------------------------------*/
		  case	SFC_DL_SIFCHG:
				ResBufW[0]	= CmdBufW[0];						/*+ AxisAdr, FunctionCode			*/
				ResBufW[1]	= CmdBufW[1];						/*+ SubFunctionCode					*/
				ResBufW[2]	= CmdBufW[2];						/*+ MPU番号							*/
				ResBufW[3]	= CmdBufW[3];						/*+ 転送速度上位ワード	*/
				ResBufW[4]	= CmdBufW[4];						/*+ 転送速度下位ワード	*/
				if(dlSts == 0)
				{
					TrnsLen = 12;
				}
				else
				{
					LpxMbusSendErrorResponse( Hmsg, REGIF_DAMTHERR2 );
					errorFlg = TRUE;
				}
				break;
		/*------------------------------------------------------------------------------------------*/
		/*		未サポートサブファンクション														*/
		/*		ダウンロード開始要求以降のみ有効なコマンド											*/
		/*			→条件エラー																	*/
		/*------------------------------------------------------------------------------------------*/
		  default:
				LpxMbusSendErrorResponse( Hmsg, REGIF_DAMTHERR2 );
				errorFlg = TRUE;
				break;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		正常応答送信																				*/
/*--------------------------------------------------------------------------------------------------*/
		if( errorFlg == FALSE )
		{
		/*------------------------------------------------------------------------------------------*/
		/*	CRC-16作成＆応答データ送信指令															*/
		/*------------------------------------------------------------------------------------------*/
			MlibSetCRC16MB( Hmsg->ResBuf, TrnsLen );
			MSGIF_SEND( Hmsg->ResBuf, TrnsLen );			/* Start Transmit Response Message	*/
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}
#endif
#endif	/* <S150> */


/****************************************************************************************************/
/*																									*/
/*		Memobus：単一レジスタ読出し																	*/
/*																									*/
/****************************************************************************************************/
void	LpxMbusReadOneRegister( HMSGIF Hmsg )
{
LONG	rc;
LONG	s = LPX_GET_SLOT_WORDLEN( Hmsg->CmdBuf[2] );
USHORT*	wCmdBuf = (USHORT *)(Hmsg->CmdBuf);
USHORT*	wResBuf = (USHORT *)(Hmsg->ResBuf);
LONG	RegNo   = MlibCnvBig16( wCmdBuf[2+s] );
/*--------------------------------------------------------------------------------------------------*/
/*		レジスタ読出し処理																			*/
/*--------------------------------------------------------------------------------------------------*/
		rc = RegFuncTbl[ RFTBLIDX(RegNo) ].ReadRegister( RegNo, 1, &wResBuf[2+s], Hmsg );
/*--------------------------------------------------------------------------------------------------*/
/*		レジスタ読出し異常チェック																	*/
/*--------------------------------------------------------------------------------------------------*/
		if( rc != REGIF_COMPLETE )
		{
			LpxMbusSendErrorResponse( Hmsg, rc );
			return;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		レジスタ読出し完了応答作成																	*/
/*--------------------------------------------------------------------------------------------------*/
		wResBuf[0] = wCmdBuf[0];						/* AxisAddr, FuncCode						*/
		wResBuf[1] = wCmdBuf[1];						/* SubFuncCode								*/
		if( s ){ wResBuf[2] = wCmdBuf[2];}				/* Slot Number								*/
		wResBuf[2+s] = MlibCnvBig16( wResBuf[2+s] );	/* Read Register Data						*/
/*--------------------------------------------------------------------------------------------------*/
/*		CRC-16作成＆応答データ送信指令																*/
/*--------------------------------------------------------------------------------------------------*/
		MlibSetCRC16MB(  Hmsg->ResBuf, 8+(2*s) );		/* Set CRC-16 to ResBuf						*/
		MSGIF_SEND( Hmsg->ResBuf, 8+(2*s) );		/* Start Transmit Response Message			*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		Memobus：単一レジスタ書込み																	*/
/*																									*/
/****************************************************************************************************/
void	LpxMbusWriteOneRegister( HMSGIF Hmsg )
{
LONG	rc;
LONG	s = LPX_GET_SLOT_WORDLEN( Hmsg->CmdBuf[2] );
USHORT*	wCmdBuf = (USHORT *)(Hmsg->CmdBuf);
USHORT*	wResBuf = (USHORT *)(Hmsg->ResBuf);
LONG	RegNo   = MlibCnvBig16( wCmdBuf[2+s] );
/*--------------------------------------------------------------------------------------------------*/
/*		レジスタ書込み処理																			*/
/*--------------------------------------------------------------------------------------------------*/
		wResBuf[3+s] = MlibCnvBig16( wCmdBuf[3+s] );
		rc = RegFuncTbl[ RFTBLIDX(RegNo) ].WriteRegister( RegNo, 1, &wResBuf[3+s], Hmsg );
/*--------------------------------------------------------------------------------------------------*/
/*		応答チェック (補助機能実行中)																*/
/*--------------------------------------------------------------------------------------------------*/
		if( rc == REGIF_CONTINUE )
		{												/* 補助機能実行後、応答を返信するので		*/
			return;										/* ここでは、何もしない						*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		レジスタ書込み異常チェック																	*/
/*--------------------------------------------------------------------------------------------------*/
		else if( rc != REGIF_COMPLETE )
		{
			LpxMbusSendErrorResponse( Hmsg, rc );
			return;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		レジスタ書込み完了応答作成																	*/
/*--------------------------------------------------------------------------------------------------*/
		wResBuf[0] = wCmdBuf[0];						/* AxisAddr, FuncCode						*/
		wResBuf[1] = wCmdBuf[1];						/* SubFuncCode								*/
		if( s ){ wResBuf[2] = wCmdBuf[2];}				/* Slot Number								*/
		wResBuf[2+s] = wCmdBuf[2+s];					/* RegNo									*/
		wResBuf[3+s] = wCmdBuf[3+s];					/* WriteData								*/
		wResBuf[4+s] = wCmdBuf[4+s];					/* CRC-16									*/
/*--------------------------------------------------------------------------------------------------*/
/*		応答データ送信指令																			*/
/*--------------------------------------------------------------------------------------------------*/
		MSGIF_SEND( Hmsg->ResBuf, 10+(2*s) );		/* Start Transmit Response Message			*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		Memobus：連続レジスタ読出し																	*/
/*																									*/
/****************************************************************************************************/
void	LpxMbusReadSeqRegisters( HMSGIF Hmsg )
{
LONG	i, rc;
LONG	s = LPX_GET_SLOT_WORDLEN( Hmsg->CmdBuf[2] );
USHORT*	wCmdBuf = (USHORT *)(Hmsg->CmdBuf);
USHORT*	wResBuf = (USHORT *)(Hmsg->ResBuf);
LONG	RegNo   = MlibCnvBig16( wCmdBuf[2+s] );
LONG	RegNum  = MlibCnvBig16( wCmdBuf[3+s] );
/*--------------------------------------------------------------------------------------------------*/
/*		レジスタ読出し個数数チェック																*/
/*--------------------------------------------------------------------------------------------------*/
		if( (RegNum < 1) || (RegNum > LpxGetMaxNumberOfRdRegisters( Hmsg, 8+(2*s) )) )
		{
			LpxMbusSendErrorResponse( Hmsg, SMBUS_EC_DANUMERR );
			return;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		レジスタ読出し処理																			*/
/*--------------------------------------------------------------------------------------------------*/
		rc = RegFuncTbl[ RFTBLIDX(RegNo) ].ReadRegister( RegNo, RegNum, &wResBuf[3+s], Hmsg );
/*--------------------------------------------------------------------------------------------------*/
/*		レジスタ読出し異常チェック																	*/
/*--------------------------------------------------------------------------------------------------*/
		if( rc != REGIF_COMPLETE )
		{
			LpxMbusSendErrorResponse( Hmsg, rc );
			return;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		レジスタ読出し完了応答作成																	*/
/*--------------------------------------------------------------------------------------------------*/
		wResBuf[0] = wCmdBuf[0];						/* AxisAddr, FuncCode						*/
		wResBuf[1] = wCmdBuf[1];						/* SubFuncCode								*/
		if( s ){ wResBuf[2] = wCmdBuf[2];}				/* Slot Number								*/
		wResBuf[2+s] = MlibCnvBig16( RegNum * 2 );		/* Data Byte Number							*/
/*--------------------------------------------------------------------------------------------------*/
//#if		defined( CCSW_LITTLE_ENDIAN ) TODO
		for( i=0; i < RegNum; i++ ){ wResBuf[3+s+i] = MlibCnvBig16( wResBuf[3+s+i] );}
//#endif
/*--------------------------------------------------------------------------------------------------*/
/*		CRC-16作成＆応答データ送信指令																*/
/*--------------------------------------------------------------------------------------------------*/
		i = (RegNum * 2) + 8 + (2*s);					/* ResMsg Length							*/
		MlibSetCRC16MB(  Hmsg->ResBuf, i );				/* Set CRC-16 to ResBuf						*/
		MSGIF_SEND( Hmsg->ResBuf, i );				/* Start Transmit Response Message			*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		Memobus：連続レジスタ書込み																	*/
/*																									*/
/****************************************************************************************************/
void	LpxMbusWriteSeqRegisters( HMSGIF Hmsg )
{
LONG	i, rc;
LONG	s = LPX_GET_SLOT_WORDLEN( Hmsg->CmdBuf[2] );
USHORT*	wCmdBuf = (USHORT *)(Hmsg->CmdBuf);
USHORT*	wResBuf = (USHORT *)(Hmsg->ResBuf);
LONG	RegNo   = MlibCnvBig16( wCmdBuf[2+s] );
LONG	RegNum  = MlibCnvBig16( wCmdBuf[3+s] );
/*--------------------------------------------------------------------------------------------------*/
/*		レジスタ書込み個数数チェック																*/
/*--------------------------------------------------------------------------------------------------*/
		if( (RegNum < 1) || (RegNum > LpxGetMaxNumberOfWrRegisters( Hmsg, 12+(2*s) )) )
		{
			LpxMbusSendErrorResponse( Hmsg, SMBUS_EC_DANUMERR );
			return;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		レジスタ書込み処理																			*/
/*--------------------------------------------------------------------------------------------------*/
#if		defined( CCSW_BIG_ENDIAN )
		rc = RegFuncTbl[ RFTBLIDX(RegNo) ].WriteRegister( RegNo, RegNum, &wCmdBuf[5+s], Hmsg );
#else
		for( i=0; i < RegNum; i++ ){ wResBuf[5+s+i] = MlibCnvBig16( wCmdBuf[5+s+i] );}
		rc = RegFuncTbl[ RFTBLIDX(RegNo) ].WriteRegister( RegNo, RegNum, &wResBuf[5+s], Hmsg );
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		応答チェック (補助機能実行中)																*/
/*--------------------------------------------------------------------------------------------------*/
		if( rc == REGIF_CONTINUE )
		{												/* 補助機能実行後、応答を返信するので		*/
			return;										/* ここでは、何もしない						*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		レジスタ書込み異常チェック																	*/
/*--------------------------------------------------------------------------------------------------*/
		else if( rc != REGIF_COMPLETE )
		{
			LpxMbusSendErrorResponse( Hmsg, rc );
			return;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		レジスタ書込み完了応答作成																	*/
/*--------------------------------------------------------------------------------------------------*/
		wResBuf[0] = wCmdBuf[0];						/* AxisAddr, FuncCode						*/
		wResBuf[1] = wCmdBuf[1];						/* SubFuncCode								*/
		if( s ){ wResBuf[2] = wCmdBuf[2];}				/* Slot Number								*/
		wResBuf[2+s] = wCmdBuf[2+s];					/* RegNo									*/
		wResBuf[3+s] = wCmdBuf[3+s];					/* Write Number								*/
/*--------------------------------------------------------------------------------------------------*/
/*		CRC-16作成＆応答データ送信指令																*/
/*--------------------------------------------------------------------------------------------------*/
		MlibSetCRC16MB(  Hmsg->ResBuf, 10+(2*s) );		/* Set CRC-16 to ResBuf						*/
		MSGIF_SEND( Hmsg->ResBuf, 10+(2*s) );		/* Start Transmit Response Message			*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		Memobus：非連続レジスタ読出し																*/
/*																									*/
/****************************************************************************************************/
void	LpxMbusReadRandomRegisters( HMSGIF Hmsg )
{
LONG	rc;
LONG	i,j,x;
LONG	RegNo;
LONG	s = LPX_GET_SLOT_WORDLEN( Hmsg->CmdBuf[2] );
USHORT*	wCmdBuf = (USHORT *)(Hmsg->CmdBuf);
USHORT*	wResBuf = (USHORT *)(Hmsg->ResBuf);
LONG	RegNum  = MlibCnvBig16( wCmdBuf[2+s] );
/*--------------------------------------------------------------------------------------------------*/
/*		レジスタ読出し個数数チェック																*/
/*--------------------------------------------------------------------------------------------------*/
		if( (RegNum < 1) || (RegNum > LpxGetMaxNumberOfRdRegisters( Hmsg, 8+(2*s) )) )
		{
			LpxMbusSendErrorResponse( Hmsg, SMBUS_EC_DANUMERR );
			return;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		レジスタ読出し処理																			*/
/*--------------------------------------------------------------------------------------------------*/
		for( i=0; i < RegNum; i=i+j )
		{
			RegNo = MlibCnvBig16( wCmdBuf[3+s+i] );
			for( j=1; j < (RegNum - i); j++ )
			{
				x = MlibCnvBig16( wCmdBuf[3+s+i+j] );
				if( x != (RegNo + j) ) break;
			}
		/*------------------------------------------------------------------------------------------*/
			rc = RegFuncTbl[ RFTBLIDX(RegNo) ].ReadRegister( RegNo, j, &wResBuf[3+s+i], Hmsg );
			if( rc != REGIF_COMPLETE )
			{
				LpxMbusSendErrorResponse( Hmsg, rc );
				return;
			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		レジスタ読出し完了応答作成																	*/
/*--------------------------------------------------------------------------------------------------*/
		wResBuf[0] = wCmdBuf[0];						/* AxisAddr, FuncCode						*/
		wResBuf[1] = wCmdBuf[1];						/* SubFuncCode								*/
		if( s ){ wResBuf[2] = wCmdBuf[2];}				/* Slot Number								*/
		wResBuf[2+s] = MlibCnvBig16( RegNum * 2 );		/* Data Byte Number							*/
/*--------------------------------------------------------------------------------------------------*/
//#if		defined( CCSW_LITTLE_ENDIAN ) TODO
		for( i=0; i < RegNum; i++ ){ wResBuf[3+s+i] = MlibCnvBig16( wResBuf[3+s+i] );}
//#endif
/*--------------------------------------------------------------------------------------------------*/
/*		CRC-16作成＆応答データ送信指令																*/
/*--------------------------------------------------------------------------------------------------*/
		i = (RegNum * 2) + 8 + (2*s);					/* ResMsg Length							*/
		MlibSetCRC16MB(  Hmsg->ResBuf, i );				/* Set CRC-16 to ResBuf						*/
		MSGIF_SEND( Hmsg->ResBuf, i );				/* Start Transmit Response Message			*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		Memobus：非連続レジスタ書込み																*/
/*																									*/
/****************************************************************************************************/
void	LpxMbusWriteRandomRegisters( HMSGIF Hmsg )
{
LONG	rc;
LONG	i,j,x;
LONG	RegNo;
LONG	s = LPX_GET_SLOT_WORDLEN( Hmsg->CmdBuf[2] );
USHORT*	wCmdBuf = (USHORT *)(Hmsg->CmdBuf);
USHORT*	wResBuf = (USHORT *)(Hmsg->ResBuf);
LONG	RegNum  = MlibCnvBig16( wCmdBuf[2+s] );
/*--------------------------------------------------------------------------------------------------*/
/*		レジスタ書込み個数数チェック																*/
/*--------------------------------------------------------------------------------------------------*/
		if( (RegNum < 1) || (RegNum > (LpxGetMaxNumberOfWrRegisters( Hmsg, 10+(2*s) ) >> 1) ) )
		{
			LpxMbusSendErrorResponse( Hmsg, SMBUS_EC_DANUMERR );
			return;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		レジスタ書込み処理																			*/
/*--------------------------------------------------------------------------------------------------*/
		for( i=0; i < RegNum; i=i+j )
		{
			RegNo = MlibCnvBig16( wCmdBuf[4+s+(2*i)] );
			for( j=1; j < (RegNum - i); j++ )
			{
				x = MlibCnvBig16( wCmdBuf[4+s+(2*(i+j))] );
				if( x != (RegNo + j) ) break;
			}
		/*------------------------------------------------------------------------------------------*/
			for( x=0; x < j; x++ )
			{
				wResBuf[x] = MlibCnvBig16( wCmdBuf[4+s+(2*(i+x))+1] );
			}
		/*------------------------------------------------------------------------------------------*/
			rc = RegFuncTbl[ RFTBLIDX(RegNo) ].WriteRegister( RegNo, j, &wResBuf[0], Hmsg );
			if( rc != REGIF_COMPLETE )
			{
				LpxMbusSendErrorResponse( Hmsg, rc );
				return;
			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		レジスタ書込み完了応答作成																	*/
/*--------------------------------------------------------------------------------------------------*/
		wResBuf[0] = wCmdBuf[0];						/* AxisAddr, FuncCode						*/
		wResBuf[1] = wCmdBuf[1];						/* SubFuncCode								*/
		if( s ){ wResBuf[2] = wCmdBuf[2];}				/* Slot Number								*/
		wResBuf[2+s] = wCmdBuf[2+s];					/* RegNum									*/
/*--------------------------------------------------------------------------------------------------*/
/*		CRC-16作成＆応答データ送信指令																*/
/*--------------------------------------------------------------------------------------------------*/
		MlibSetCRC16MB(  Hmsg->ResBuf, 8+(2*s) );		/* Set CRC-16 to ResBuf						*/
		MSGIF_SEND( Hmsg->ResBuf, 8+(2*s) );		/* Start Transmit Response Message			*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		Memobus：最大レジスタ個数読出し																*/
/*																									*/
/****************************************************************************************************/
void	LpxMbusReadMaxNumberOfRegisters( HMSGIF Hmsg )
{
LONG	i;
LONG	MaxNum;
LONG	s = LPX_GET_SLOT_WORDLEN( Hmsg->CmdBuf[2] );
USHORT*	wCmdBuf = (USHORT *)(Hmsg->CmdBuf);
USHORT*	wResBuf = (USHORT *)(Hmsg->ResBuf);
/*--------------------------------------------------------------------------------------------------*/
/*		Check SFC with Slot																			*/
/*--------------------------------------------------------------------------------------------------*/
		i = LPX_GET_SLOT_BYTELEN( Hmsg->CmdBuf[4+(2*s)] );
/*--------------------------------------------------------------------------------------------------*/
/*		Check Function Code and Calculate MaxNumber													*/
/*--------------------------------------------------------------------------------------------------*/
		switch( Hmsg->CmdBuf[5+(2*s)] )
		{
		case SFC_RD_ONEREG: MaxNum = 1; break;
		case SFC_WR_ONEREG: MaxNum = 1; break;
		case SFC_RD_SEQREG: MaxNum = LpxGetMaxNumberOfRdRegisters( Hmsg, i +  8 ); break;
		case SFC_WR_SEQREG: MaxNum = LpxGetMaxNumberOfWrRegisters( Hmsg, i + 12 ); break;
		case SFC_RD_RANREG: MaxNum = LpxGetMaxNumberOfRdRegisters( Hmsg, i +  8 ); break;
		case SFC_WR_RANREG: MaxNum = LpxGetMaxNumberOfWrRegisters( Hmsg, i + 10 ) >> 1; break;
		case SFC_RD_SEQMEM: MaxNum = LpxGetMaxNumberOfRdRegisters( Hmsg, i +  8 ); break;
		case SFC_WR_SEQMEM: MaxNum = LpxGetMaxNumberOfWrRegisters( Hmsg, i + 12 ); break;
		default: 			MaxNum = 0; break;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		応答データ作成																				*/
/*--------------------------------------------------------------------------------------------------*/
		wResBuf[0] = wCmdBuf[0];						/* AxisAddr, FuncCode						*/
		wResBuf[1] = wCmdBuf[1];						/* SubFuncCode								*/
		if( s ){ wResBuf[2] = wCmdBuf[2];}				/* Slot Number								*/
		wResBuf[2+s] = wCmdBuf[2+s];					/* SubFuncCode for MaxNum					*/
		wResBuf[3+s] = MlibCnvBig16( MaxNum );			/* MaxNumber of Registers					*/
/*--------------------------------------------------------------------------------------------------*/
/*		CRC-16作成＆応答データ送信指令																*/
/*--------------------------------------------------------------------------------------------------*/
		MlibSetCRC16MB(  Hmsg->ResBuf, 10+(2*s) );		/* Set CRC-16 to ResBuf						*/
		MSGIF_SEND( Hmsg->ResBuf, 10+(2*s) );		/* Start Transmit Response Message			*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		Memobus：連続メモリ読出し																	*/
/*																									*/
/****************************************************************************************************/
void	LpxMbusReadSeqMemories( HMSGIF Hmsg )
{
//LONG	i, rc;
LONG	i;
LONG	s = LPX_GET_SLOT_WORDLEN( Hmsg->CmdBuf[2] );
USHORT*	wCmdBuf = (USHORT *)(Hmsg->CmdBuf);
USHORT*	wResBuf = (USHORT *)(Hmsg->ResBuf);
ULONG	MemNum  = MlibCnvBig16( wCmdBuf[4+s] );
ULONG	MemAddr = (MlibCnvBig16( wCmdBuf[2+s] ) << 16) +  MlibCnvBig16( wCmdBuf[3+s] );
/*--------------------------------------------------------------------------------------------------*/
/*		メモリ読出し個数数チェック																	*/
/*--------------------------------------------------------------------------------------------------*/
		if( (MemNum < 1) || (MemNum > LpxGetMaxNumberOfRdRegisters( Hmsg, 8+(2*s) )) )
		{
			LpxMbusSendErrorResponse( Hmsg, SMBUS_EC_DANUMERR );
			return;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		メモリ読出しアドレスチェック																*/
/*--------------------------------------------------------------------------------------------------*/
		if ( 0 /* KpiCheckReadMemAddr( MemAddr, MemNum) == MEMDEF_READ_NG  */)
		{
			LpxMbusSendErrorResponse( Hmsg, SMBUS_SEC_ACCSERR );
			return;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		メモリ読出し応答データ作成																	*/
/*--------------------------------------------------------------------------------------------------*/
		wResBuf[0] = wCmdBuf[0];						/* AxisAddr, FuncCode						*/
		wResBuf[1] = wCmdBuf[1];						/* SubFuncCode								*/
		if( s ){ wResBuf[2] = wCmdBuf[2];}				/* Slot Number								*/
		wResBuf[2+s] = wCmdBuf[4+s];					/* Data Word Number							*/
/*--------------------------------------------------------------------------------------------------*/
		for( i=0; i < MemNum; i++ )
		{
			wResBuf[3+s+i] = MlibCnvBig16( ((USHORT*)MemAddr)[i] );
		}
/*--------------------------------------------------------------------------------------------------*/
/*		CRC-16作成＆応答データ送信指令																*/
/*--------------------------------------------------------------------------------------------------*/
		i = (MemNum * 2) + 8 + (2*s);					/* ResMsg Length							*/
		MlibSetCRC16MB(  Hmsg->ResBuf, i );				/* Set CRC-16 to ResBuf						*/
		MSGIF_SEND( Hmsg->ResBuf, i );				/* Start Transmit Response Message			*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		Memobus：連続メモリ書込み																	*/
/*																									*/
/****************************************************************************************************/
void	LpxMbusWriteSeqMemories( HMSGIF Hmsg )
{
LONG	i;
LONG	s = LPX_GET_SLOT_WORDLEN( Hmsg->CmdBuf[2] );
USHORT*	wCmdBuf = (USHORT *)(Hmsg->CmdBuf);
USHORT*	wResBuf = (USHORT *)(Hmsg->ResBuf);
ULONG	MemNum  = MlibCnvBig16( wCmdBuf[4+s] );
ULONG	MemAddr = (MlibCnvBig16( wCmdBuf[2+s] ) << 16) +  MlibCnvBig16( wCmdBuf[3+s] );
/*--------------------------------------------------------------------------------------------------*/
/*		メモリ書込み個数数チェック																	*/
/*--------------------------------------------------------------------------------------------------*/
		if( (MemNum < 1) || (MemNum > LpxGetMaxNumberOfWrRegisters( Hmsg, 12+(2*s) )) )
		{
			LpxMbusSendErrorResponse( Hmsg, SMBUS_EC_DANUMERR );
			return;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		メモリ書込みアドレスチェック																*/
/*--------------------------------------------------------------------------------------------------*/
		if ( 0 /* KpiCheckWriteMemAddr( MemAddr, MemNum ) == MEMDEF_WRITE_NG */)
		{
			LpxMbusSendErrorResponse( Hmsg, SMBUS_SEC_ACCSERR );
			return;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		メモリ書込み処理																			*/
/*--------------------------------------------------------------------------------------------------*/
		for( i=0; i < MemNum; i++ )
		{
			((USHORT*)MemAddr)[i] = MlibCnvBig16( wCmdBuf[5+s+i] );
		}
/*--------------------------------------------------------------------------------------------------*/
/*		メモリ書込み応答データ作成																	*/
/*--------------------------------------------------------------------------------------------------*/
		wResBuf[0] = wCmdBuf[0];						/* AxisAddr, FuncCode						*/
		wResBuf[1] = wCmdBuf[1];						/* SubFuncCode								*/
		if( s ){ wResBuf[2] = wCmdBuf[2];}				/* Slot Number								*/
		wResBuf[2+s] = wCmdBuf[2+s];					/* Memory Address							*/
		wResBuf[3+s] = wCmdBuf[3+s];					/* Memory Address							*/
		wResBuf[4+s] = wCmdBuf[4+s];					/* Data Word Number							*/
/*--------------------------------------------------------------------------------------------------*/
/*		CRC-16作成＆応答データ送信指令																*/
/*--------------------------------------------------------------------------------------------------*/
		MlibSetCRC16MB(  Hmsg->ResBuf, 12 + (2*s) );	/* Set CRC-16 to ResBuf						*/
		MSGIF_SEND( Hmsg->ResBuf, 12 + (2*s) );	/* Start Transmit Response Message			*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		最大読出しレジスタ個数の取得																*/
/*																									*/
/****************************************************************************************************/
LONG	LpxGetMaxNumberOfRdRegisters( HMSGIF Hmsg, LONG FixedLen )
{
		return( ((LONG)Hmsg->SndBufSize - FixedLen) >> 1 );
}



/****************************************************************************************************/
/*																									*/
/*		最大書込みレジスタ個数の取得																*/
/*																									*/
/****************************************************************************************************/
LONG	LpxGetMaxNumberOfWrRegisters( HMSGIF Hmsg, LONG FixedLen )
{
		return( ((LONG)Hmsg->RcvBufSize - FixedLen) >> 1 );
}



/****************************************************************************************************/
/*																									*/
/*		Memobus：異常応答返信処理																	*/
/*																									*/
/****************************************************************************************************/
void	LpxMbusSendErrorResponse( HMSGIF Hmsg, ULONG ErrCode )
{
UCHAR	*CmdBuf = (UCHAR *)(Hmsg->CmdBuf);
UCHAR	*ResBuf = (UCHAR *)(Hmsg->ResBuf);
/*--------------------------------------------------------------------------------------------------*/
/*		異常応答データ作成																			*/
/*--------------------------------------------------------------------------------------------------*/
		switch ( (USHORT)ErrCode )
		{
		case REGIF_CMDERR	: ErrCode = SMBUS_EC_FNCDERR;	break;
		case REGIF_REGNOERR	: ErrCode = SMBUS_SEC_ACCSERR;	break;
		case REGIF_DALMTERR	: ErrCode = SMBUS_SEC_DALMTERR;	break;
		case REGIF_DACALERR	:
		case REGIF_DAMTHERR	: ErrCode = SMBUS_SEC_DAMTHERR;	break;
		case REGIF_DAMTHERR2: ErrCode = SMBUS_SEC_CONDERR;	break;
		case REGIF_DANUMERR	: ErrCode = SMBUS_EC_DANUMERR;	break;
		case REGIF_FOPEERR1	:
		case REGIF_FOPEERR2	:
		case REGIF_FOPEERR3	: ErrCode = SMBUS_SEC_INTLOCK;	break;
		default:			 /* ErrCode = ErrCode; */		break;	/* <S004> */
		}
/*--------------------------------------------------------------------------------------------------*/
/*		異常応答データ作成																			*/
/*--------------------------------------------------------------------------------------------------*/
		ResBuf[0] = CmdBuf[0];							/* AxisAddr									*/
		ResBuf[1] = CmdBuf[1] | 0x80;					/* Function Code							*/
		ResBuf[2] = (UCHAR)(ErrCode);					/* Error Code								*/
/*--------------------------------------------------------------------------------------------------*/
/*		CRC-16作成＆応答データ送信指令																*/
/*--------------------------------------------------------------------------------------------------*/
		MlibSetCRC16MB(  Hmsg->ResBuf, 5 );				/* Set CRC-16 to ResBuf						*/
		MSGIF_SEND( Hmsg->ResBuf, 5 );				/* Start Transmit Response Message			*/
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		レジスタＩＦ関数：レジスタ番号異常															*/
/*																									*/
/****************************************************************************************************/
LONG	LpxRegifRegNoError( LONG RegNo, LONG Number, USHORT *RwBuf, HMSGIF Hmsg )
{
		Hmsg->ErrRno = (USHORT)RegNo;					/* 異常レジスタ番号							*/
		return ( SMBUS_SEC_REGNOERR );					/* レジスタ番号異常							*/
}

#if 0 && CCSW_CMDID == CMDID_EXIST
/****************************************************************************************************/
/*																									*/
/*		オプションカード通信応答データ転送処理														*/
/*																									*/
/****************************************************************************************************/
ULONG	RpxSmsgTransOptRspMessage( HMSGIF Hmsg )
{
ULONG	rc;

/*--------------------------------------------------------------------------------------------------*/
/*		シリアル通信データ応答転送																	*/
/*--------------------------------------------------------------------------------------------------*/
		rc = MemobusTransCmdOptRspMsg( Hmsg );		/* 指令オプションカード (SCメッセージ通信)		*/

		return( rc );
}



/****************************************************************************************************/
/*																									*/
/*		指令オプションカードへのコマンドデータ転送設定 (SigmaWin+等の上位ツールから)				*/
/*																									*/
/****************************************************************************************************/
void	MemobusTransCmdOptCmdMsg( HMSGIF Hmsg )
{
LONG	size = (LONG)(Hmsg->CmdMsgLen);
USHORT	*CmdBufW = (USHORT *)(Hmsg->CmdBuf);

/*--------------------------------------------------------------------------------------------------*/
/*		指令オプション通信状態取得																	*/
/*--------------------------------------------------------------------------------------------------*/
		if( ( Hmsg->f.ComMsgIF ||  Hmsg->f.UsbMsgIF ) && (OpiGetSCMsgState() == SCMSG_ENABLE) )
		{
			OpiReqSCMsgTrans( CmdBufW, size);						/* SCメッセージ送信開始			*/
			KpiRstLongTimer( &Hmsg->ComTimeLong );					/* タイムアウト用タイマリセット */
			Hmsg->RcvWait = TRUE;									/* SCメッセージ通信受信待ちセット*/
		}
		else
		{
			LpxMbusSendErrorResponse( Hmsg, SMBUS_SEC_ACCSERR );	/* アクセス制限エラー (0x31)	*/
		}

		return;
}



/****************************************************************************************************/
/*																									*/
/*		指令オプションカードからのレスポンスデータ転送設定 (SigmaWin+等の上位ツールへ)				*/
/*																									*/
/****************************************************************************************************/
ULONG	MemobusTransCmdOptRspMsg( HMSGIF Hmsg )
{
LONG	rc,size;
USHORT	*ResBufW = (USHORT *)(Hmsg->ResBuf);

/*--------------------------------------------------------------------------------------------------*/
/*		SCメッセージ受信待ち時は応答取得処理実行													*/
/*--------------------------------------------------------------------------------------------------*/
		if( Hmsg->RcvWait == TRUE )
		{
/*--------------------------------------------------------------------------------------------------*/
/*			MEMOBUSメッセージコマンドに対する応答取得												*/
/*--------------------------------------------------------------------------------------------------*/
			rc = OpiGetSCMsgRspData( ResBufW, &size );

/*--------------------------------------------------------------------------------------------------*/
/*			応答データ解析																			*/
/*--------------------------------------------------------------------------------------------------*/
			if( rc == SCMSG_SUCCESS )
			{/* 正常終了 */
				Hmsg->StxResMsg( Hmsg->ResBuf, (USHORT)size );		/* Start Transmit Response Message	*/
				Hmsg->RcvWait = FALSE;								 /* 受信待ちクリア			*/
			}
			else if( rc == SCMSG_BUSY )
			{/* 処理継続中 */
				if ( KpiGetLongTimer( &Hmsg->ComTimeLong ) > SCM_COM_TIMEOUT ) /* 1000msec経過	*/
				{
					OpiCancelSCMsgReq( );							/* SCメッセージ受信待ちキャンセル	*/
					Hmsg->StxResMsg( Hmsg->ResBuf, 0 );					/* 無応答データ送信開始	*/
					Hmsg->RcvWait = FALSE;								 /* 受信待ちクリア			*/
				}
			}
			else if( (rc == SCMSG_REQ_ERR) || (rc == SCMSG_RCV_ERR) )
			{/* 異常終了 */
				Hmsg->StxResMsg( Hmsg->ResBuf, 0 );							/* 無応答データ送信開始	*/
				Hmsg->RcvWait = FALSE;									 /* 受信待ちクリア			*/
			}
			else
			{
				LpxMbusSendErrorResponse( Hmsg, SMBUS_SEC_ACCSERR );	 /* アクセス制限エラー(0x31)*/
				Hmsg->RcvWait = FALSE;									 /* 受信待ちクリア			*/
			}
			return( TRUE );		/* Hmsg->RcvWait == TRUEで入ってきた場合はとにかくTRUEで返す。　*/
								/* そうしないとFALSEで戻った直後にコマンド解析を再度してしまう。*/
		}
		else
		{
			return( Hmsg->RcvWait );
		}
}
#endif


/****************************************************************************************************/
/*																									*/
/*		パラメータ読み出し(レジスタ指定)															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	概要:	指定されるパラメータを指定個数(WORD)読み出す。											*/
/*			Reads the specified Parameter value														*/
/*																									*/
/*	戻値:	ステータス																				*/
/*																									*/
/****************************************************************************************************/
LONG	LpxReadPrmRegister(					/* パラメータ読み出し(レジスタ指定)						*/
		LONG	RegNo,						/* レジスタ番号											*/
		LONG	Number,						/* 読出しデータ個数(WORD個数)							*/
		USHORT	*RdBuf,						/* 読出しデータバッファ									*/
		HMSGIF	Hmsg			)			/* 読出しIFハンドル										*/
{
//LONG	rc;
LONG	i,idx;
LONG	pn_data;
LONG	RegNoX = RegNo & 0x0FFF;
COBJECT *pObj = Lpx_GetObject( RegNo );		/* Object Pointer */
//CPRMTBL	*pTbl = GET_PNPRMTBL( pObj );		/* Parameter Table */
/*--------------------------------------------------------------------------------------------------*/
/*		レジスタアクセスエリアチェック																*/
/*--------------------------------------------------------------------------------------------------*/
		if( ((RegNoX + Number) >= 0x1000)
		 ||	((RegNoX < 0x0800) && ((RegNoX + Number) >= 0x0800))
		 || ((RegNoX < 0x0D00) && ((RegNoX + Number) >= 0x0D00))	)
		{
			Hmsg->ErrRno = (USHORT)RegNo;				/* 異常レジスタ番号							*/
			return( REGIF_DAMTHERR );					/* データ整合異常(グループ境界)				*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		レジスタ番号検索																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( OK != OBJ_SEARCH( pObj, RegNoX, &idx, Hmsg ) )
		{
			Hmsg->ErrRno = (USHORT)RegNo;				/* 異常レジスタ番号							*/
			return( REGIF_REGNOERR );					/* レジスタ番号異常(REG番号無し)			*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		開始レジスタ整合チェック																	*/
/*--------------------------------------------------------------------------------------------------*/
		if( pObj->GetNumber( pObj, idx, Hmsg ) != RegNoX )
		{
			Hmsg->ErrRno = (USHORT)RegNo;				/* 異常レジスタ番号							*/
			return( REGIF_DAMTHERR );					/* データ整合異常(LONG境界)					*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		レジスタデータ読出し																		*/
/*--------------------------------------------------------------------------------------------------*/
		for( i=0; i < Number; i++, idx++ )
		{
		/*------------------------------------------------------------------------------------------*/
		/*	システムパラメータ読出しチェック(行った方が良いかも！)									*/
		/*------------------------------------------------------------------------------------------*/
#if 0
			if( 0 && (PnPrmTbl[idx].PrmDefPtr->AccessLevel >= ACCLVL_SYSTEM) )
			{
				if( Hmsg->AccessLvl < ACCLVL_SYSTEM )
				{
					Hmsg->ErrRno = RegNo + i;			/* 異常レジスタ番号							*/
					return( REGIF_REGNOERR );			/* レジスタ番号異常							*/
				}
			}
#endif
		/*------------------------------------------------------------------------------------------*/
		/*	レジスタ番号の連続性チェック															*/
		/*------------------------------------------------------------------------------------------*/
			if( pObj->GetNumber( pObj, idx, Hmsg ) != (RegNoX + i) )
			{
				Hmsg->ErrRno = RegNo + i;				/* 異常レジスタ番号							*/
				return( REGIF_REGNOERR );				/* レジスタ番号異常(REG番号非連続)			*/
			}
		/*------------------------------------------------------------------------------------------*/
		/*	パラメータ読込み																		*/
		/*------------------------------------------------------------------------------------------*/
			OBJ_READ( pObj, idx, (ULONG*)&pn_data, Hmsg );
#if 0
//			rc = RpxPrmReadIdx( idx, (ULONG *)(&pn_data), PCHK_REG_READ );	/* <11J45>削除 */

			/* <11J45>追加 Start */
			if(Hmsg->f.NotConvScale == TRUE)
			{
				rc = RpxPrmReadIdxNotConvScale( idx, (ULONG *)(&pn_data), PCHK_REG_READ ); /* スケール変換しない */
			}
			else
			{
				rc = RpxPrmReadIdx( idx, (ULONG *)(&pn_data), PCHK_REG_READ );				/* スケール変換する */
			}
			/* <11J45>追加 End */

			if(rc != OK)
			{
				Hmsg->ErrRno = RegNo + i;				/* 異常レジスタ番号							*/
				switch(rc)
				{
				case PERR_DFIT:
					return( REGIF_DAMTHERR );			/* データ整合異常(LONG境界)					*/
					break;
				case PERR_PRMCOPY:
					return( REGIF_FOPEERR3 );			/* 処理不可(他ＣＨとの処理競合)				*/
					break;
				default:
					return( REGIF_REGNOERR );			/* レジスタ番号異常							*/
					break;
				}
			}
#endif
		/*------------------------------------------------------------------------------------------*/
		/*	読込みデータはショート/ロング?															*/
		/*------------------------------------------------------------------------------------------*/
			if( pObj->isLong( pObj, idx, Hmsg ) )
			{
				RdBuf[i+0]= GET_LWD_REGDATA( pn_data );	/* LONG  Parameter Low  Word				*/
				RdBuf[i+1]= GET_HWD_REGDATA( pn_data );	/* LONG  Parameter High Word				*/
				i++;
			}
			else
			{
				RdBuf[i] = (USHORT)(pn_data);			/* SHORT Parameter							*/
			}
#if 0
//			if( PnPrmTbl[idx].PrmDefPtr->Attr.IfIsLong == FALSE )/* <11J45>削除 */
			if (((PnPrmTbl[idx].PrmDefPtr->Attr.IfIsLong == FALSE)&&( Hmsg->f.NotConvScale == FALSE))
				||((PnPrmTbl[idx].PrmDefPtr->Attr.VarIsLong == FALSE)&&( Hmsg->f.NotConvScale == TRUE)))/* <11J45> */
			{
				RdBuf[i] = (USHORT)(pn_data);			/* SHORT Parameter							*/
			}
			else if( (i+1) < Number )
			{
				RdBuf[i+0]= GET_LWD_REGDATA( pn_data );	/* LONG  Parameter Low  Word				*/
				RdBuf[i+1]= GET_HWD_REGDATA( pn_data );	/* LONG  Parameter High Word				*/
				i++;
			}
			else
			{
				Hmsg->ErrRno = RegNo + i;				/* 異常レジスタ番号							*/
				return( REGIF_DAMTHERR );				/* データ整合異常(LONG境界)					*/
			}
#endif
		}
/*--------------------------------------------------------------------------------------------------*/
		return( REGIF_COMPLETE );
}

/****************************************************************************************************/
/*																									*/
/*		パラメータ書き込み処理(レジスタ指定)														*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	概要:	レジスタで指定されるパラメータ値をRAMへ書き込む。										*/
/*																									*/
/*	戻値:	ステータス																				*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
LONG	LpxWritePrmRegister(				/* パラメータ書き込み処理(レジスタ指定)					*/
		LONG	RegNo,						/* レジスタ番号											*/
		LONG	Number,						/* 書き込みデータ個数(WORD個数)							*/
		USHORT	*WrBuf,						/* 書き込みデータバッファ								*/
		HMSGIF	Hmsg			)			/* 書き込みIFハンドル									*/
{
LONG	rc;
LONG	i,idx;
ULONG	pn_data;
LONG	RegNoX = RegNo & 0x0FFF;
COBJECT *pObj = Lpx_GetObject( RegNo );		/* Object Pointer */
CPRMTBL	*pTbl = GET_PNPRMTBL( pObj );		/* Parameter Table */
/*--------------------------------------------------------------------------------------------------*/
/*		レジスタアクセスエリアチェック																*/
/*--------------------------------------------------------------------------------------------------*/
		if( ((RegNoX + Number) >= 0x1000)
		 ||	((RegNoX < 0x0800) && ((RegNoX + Number) >= 0x0800))
		 || ((RegNoX < 0x0D00) && ((RegNoX + Number) >= 0x0D00))	)
		{
			Hmsg->ErrRno = (USHORT)RegNo;				/* 異常レジスタ番号							*/
			return( REGIF_DAMTHERR );					/* データ整合異常(グループ境界)				*/
		}
#if 0 
/*--------------------------------------------------------------------------------------------------*/
/*		システム定数パラメータチェック																*/
/*--------------------------------------------------------------------------------------------------*/
		if( (OpeReg.PrmAccess == FALSE) &&
			((RegNoX + Number) >= 0x0D00) && (Hmsg->AccessLvl != ACCLVL_SYSTEM) )
		{
			i = (RegNoX >= 0x0D00)? 0 : (0x0D00 - RegNoX);
			Hmsg->ErrRno = RegNo + i;					/* 異常レジスタ番号							*/
			return( REGIF_REGNOERR );					/* レジスタ番号異常							*/
		}
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		エンコーダ定数パラメータチェック															*/
/*--------------------------------------------------------------------------------------------------*/
#if 0
		if( ENC_PRM_WRT_DISABLE && ((RegNoX + Number) >= ENC_PRM_NUM) )
		{
			i = (RegNoX >= 0x0F00)? 0 : (ENC_PRM_NUM - RegNoX);
			Hmsg->ErrRno = RegNo + i;					/* 異常レジスタ番号							*/
			return( REGIF_REGNOERR );					/* レジスタ番号異常							*/
		}
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		レジスタ番号検索																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( OK != OBJ_SEARCH( pObj, RegNoX, &idx, Hmsg ) )
		{
			Hmsg->ErrRno = (USHORT)RegNo;				/* 異常レジスタ番号							*/
			return( REGIF_REGNOERR );					/* レジスタ番号異常(REG番号無し)			*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		開始レジスタ整合チェック																	*/
/*--------------------------------------------------------------------------------------------------*/
		if( pObj->GetNumber( pObj, idx, Hmsg ) != RegNoX )
		{
			Hmsg->ErrRno = (USHORT)RegNo;				/* 異常レジスタ番号							*/
			return( REGIF_DAMTHERR );					/* データ整合異常(LONG境界)					*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		システム定数パラメータチェック								<12J49>追加					 	*/
/*--------------------------------------------------------------------------------------------------*/
#if 0
/* レジスタ番号検索で取得した、「idx」を使用するために、本処理をこの場所へ持ってきている。		 	*/
		if( (pPrm->prmwr_spn == 0x0001) 									   /* Pn030 = 0x0001(モータPrm書き込み有効) */
		 && (PnPrmTbl[idx].PrmDefPtr->Attr.MotorPrmAmp == PRMDEF_MOTPRM_AMP)   /* アンプ内のモータPrm(主軸モータPrm) 	*/
		 && (PnPrmTbl[idx].PrmDefPtr != (&pndef_MotPrmSumChkEnb)) 			   /* PnFFFは除く 							*/
		 && (Hmsg->f.LnkMsgIF == TRUE) )									   /* SVPRM_WRを使用	 					*/
		{
			;
		}
		else if( (OpeReg.PrmAccess == FALSE) &&
				((RegNoX + Number) >= 0x0D00) && (Hmsg->AccessLvl != ACCLVL_SYSTEM) )
		{
			i = (RegNoX >= 0x0D00)? 0 : (0x0D00 - RegNoX);
			Hmsg->ErrRno = RegNo + i;					/* 異常レジスタ番号							*/
			return( REGIF_REGNOERR );					/* レジスタ番号異常							*/
		}
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		レジスタデータ書込み禁止チェック															*/
/*--------------------------------------------------------------------------------------------------*/
//		if( RndWrtDisable.FunA.f.PrmReg || RndWrtDisable.FunB.f.PrmReg )
//		{
//			Hmsg->ErrRno = (USHORT)RegNo;				/* 異常レジスタ番号							*/
//			return( REGIF_REGNOERR );					/* レジスタ番号異常							*/
//		}
/*--------------------------------------------------------------------------------------------------*/
/*		レジスタデータ書込み																		*/
/*--------------------------------------------------------------------------------------------------*/
		for( i=0; i < Number; i++, idx++ )
		{
		/*------------------------------------------------------------------------------------------*/
		/*	パラメータアクセスレベルチェック														*/
		/*------------------------------------------------------------------------------------------*/
#if 0 /* TODO :*/
			if( (OpeReg.PrmAccess == FALSE) && (PrmAttributeChkIdx(idx, PCHK_ACCS_LVL, Hmsg) != OK) )
			{
				Hmsg->ErrRno = RegNo + i;				/* 異常レジスタ番号							*/
				return( REGIF_REGNOERR );				/* レジスタ番号異常							*/
			}
#endif
		/*------------------------------------------------------------------------------------------*/
		/*	レジスタ番号の連続性チェック															*/
		/*------------------------------------------------------------------------------------------*/
			if( pObj->GetNumber( pObj, idx, Hmsg ) != (RegNoX + i) )
			{
				Hmsg->ErrRno = RegNo + i;				/* 異常レジスタ番号							*/
				return( REGIF_REGNOERR );				/* レジスタ番号異常(REG番号非連続)			*/
			}
		/*------------------------------------------------------------------------------------------*/
		/*	書込みデータはロング?																	*/
		/*------------------------------------------------------------------------------------------*/
			if( pObj->isLong( pObj, idx, Hmsg ) )
			{
				if( (i + 1) < Number )
				{
					pn_data = ULONGOF_REGDATA( WrBuf[i], WrBuf[i+1] );
				}
				else
				{
					Hmsg->ErrRno = RegNo + i;			/* 異常レジスタ番号							*/
					return( REGIF_DAMTHERR );			/* データ整合異常(LONG境界)					*/
				}
			}
			else
			{
				pn_data = WrBuf[i];
				if( CHK_PNPRM_SIGNED( GET_PNPRMDEF( pTbl, idx ) ) )
				{
					pn_data = (LONG)((SHORT)pn_data); 	/* 符号付ワードの場合、符号拡張しておく	*/
				}
			}
#if 0
			if( PnPrmTbl[idx].PrmDefPtr->Attr.IfIsLong )
			{	/* LONG データ	*/
				if( (i + 1) < Number )
				{
					pn_data = ULONGOF_REGDATA( RdBuf[i], RdBuf[i+1] );
				}
				else
				{
					Hmsg->ErrRno = RegNo + i;			/* 異常レジスタ番号							*/
					return( REGIF_DAMTHERR );			/* データ整合異常(LONG境界)					*/
				}
			}
			else
			{	/* WORD データ		*/
				pn_data = RdBuf[i];
				/* WORDサイズで符号付	*/
				if( PnPrmTbl[idx].PrmDefPtr->Attr.Sign == DSPDEF_W_SIGN )
				{
					/* 符号付ワードの場合、符号拡張しておく	*/
					pn_data = (LONG)((SHORT)pn_data);
				}
			}
#endif
		/*------------------------------------------------------------------------------------------*/
		/*	パラメータRAM書込み																		*/
		/*------------------------------------------------------------------------------------------*/
			rc = OBJ_WRITE( pObj, idx, pn_data, Hmsg );
			if(  rc != OK )
			{
				Hmsg->ErrRno = RegNo + i;				/* 異常レジスタ番号							*/
				switch(rc)
				{
				case OERR_DFIT:
					 return( REGIF_DAMTHERR );			/* データ整合異常(LONG境界)					*/
//					 break;//<S0C7>statement is unreachable
				case OERR_RONLY:
					 return( REGIF_REGNOERR );			/* レジスタ番号異常							*/
//					 break;//<S0C7>statement is unreachable
				case OERR_CALC:
					 return( REGIF_DACALERR );			/* データ異常(演算異常) 					*/
//					 break;//<S0C7>statement is unreachable
				case OERR_RANGE:
					 return( REGIF_DALMTERR );			/* データ異常(上下限異常)					*/
//					 break;//<S0C7>statement is unreachable
				case OERR_PRMCOPY:
				case OERR_WRITE_HANDLE:
					 return( REGIF_FOPEERR3 );			/* 処理不可(他ＣＨとの処理競合)				*/
//					 break;//<S0C7>statement is unreachable
				default:
					 return( REGIF_REGNOERR );			/* レジスタ番号異常							*/
//					 break;//<S0C7>statement is unreachable
				}
			}
#if 0
			rc = RpxPrmWriteRamIdx( idx, pn_data, PCHK_REG_WRITE, Hmsg );
			if( rc != OK )
			{
				Hmsg->ErrRno = RegNo + i;				/* 異常レジスタ番号							*/
				switch(rc)
				{
				case PERR_DFIT:
					 return( REGIF_DAMTHERR );			/* データ整合異常(LONG境界)					*/
					 break;
				case PERR_RONLY:
					 return( REGIF_REGNOERR );			/* レジスタ番号異常							*/
					 break;
				case PERR_CALC:
					 return( REGIF_DACALERR );			/* データ異常(演算異常) 					*/
					 break;
				case PERR_RANGE:
					 return( REGIF_DALMTERR );			/* データ異常(上下限異常)					*/
					 break;
				case PERR_PRMCOPY:
				case PERR_WRITE_HANDLE:
					 return( REGIF_FOPEERR3 );			/* 処理不可(他ＣＨとの処理競合)				*/
					 break;
				default:
					 return( REGIF_REGNOERR );			/* レジスタ番号異常							*/
					 break;
				}
			}
#endif
		/*------------------------------------------------------------------------------------------*/
		/*	パラメータEEPROM書込み																	*/
		/*------------------------------------------------------------------------------------------*/
//#if 0		/* 40H EEPROM書込み対応<S111>	*/
			if( !(RegNo & 0x1000) )
			{
#if 0 /* <12J49>削除 */
				rc = RpiPrmUpdateNvmem( PnPrmTbl[idx].PrmDefPtr );
				if( rc != OK && rc != PRM_EEPROM_RAM_NO_DIFF )
				{
					return( REGIF_DAMTHERR );
				}
			}

#endif /* <12J49>削除End */

#if 0	/* 40H EEPROM書込み対応<S111>	*/ /* <12J49>追加 */
                if( (rc = LpxChkSumFlagSave(PnPrmTbl[idx].PrmDefPtr, Hmsg)) == TRUE)
                {
                    rc = RpiPrmUpdateNvmem( PnPrmTbl[idx].PrmDefPtr );
                }

                if( rc != OK && rc != PRM_EEPROM_RAM_NO_DIFF )
                {
                    return( REGIF_DAMTHERR );
				}
#endif	/* 40H EEPROM書込み対応<S111>	*/ /* <12J49>追加End */
/* 40H EEPROM書込み対応<S111>	*/
				rc = PrmWriteValueToEeprom( Hmsg->pAxRsc->RegManager->hPnReg, PnPrmTbl[idx].Attribute, 0 );
				if( (rc != PRM_RSLT_SUCCESS) && (rc != PRM_RSLT_NO_DIFF) )
				{
					return( REGIF_DAMTHERR );
				}
/* 40H EEPROM書込み対応<S111>	*/
			}
//#endif		/* 40H EEPROM書込み対応<S111>	*/

		/*------------------------------------------------------------------------------------------*/
		/*	LONGデータの場合																		*/
		/*------------------------------------------------------------------------------------------*/
			if( pObj->isLong( pObj, idx, Hmsg ) )
			{
				i++;
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		return( REGIF_COMPLETE );
}

/****************************************************************************************************/
/*																									*/
/*		レジスタ読み出し(操作エリア、機種情報、etc)													*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	概要:	指定されるパラメータを指定個数(WORD)読み出す。											*/
/*			Reads the specified Parameter value														*/
/*																									*/
/*	戻値:	ステータス																				*/
/*																									*/
/****************************************************************************************************/
LONG	LpxReadComObject(					/* パラメータ読み出し(レジスタ指定)						*/
		LONG	RegNo,						/* レジスタ番号											*/
		LONG	Number,						/* 読出しデータ個数(WORD個数)							*/
		USHORT	*RdBuf,						/* 読出しデータバッファ									*/
		HMSGIF	Hmsg			)			/* 読出しIFハンドル										*/
{
//LONG	rc;
LONG	i,idx;
LONG	subidx;								/* <S073> */
LONG	pn_data;
LONG	RegNoX = RegNo & 0x0FFF;
COBJECT *pObj = Lpx_GetObject( RegNo );		/* Object Pointer */
CPRMTBL	*pTbl = GET_PNPRMTBL( pObj );		/* Parameter Table */
/*--------------------------------------------------------------------------------------------------*/
/*		レジスタアクセスエリアチェック																*/
/*--------------------------------------------------------------------------------------------------*/
		if( 0 )
		{
			Hmsg->ErrRno = (USHORT)RegNo;				/* 異常レジスタ番号							*/
			return( REGIF_DAMTHERR );					/* データ整合異常(グループ境界)				*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		レジスタ番号検索																			*/
/*--------------------------------------------------------------------------------------------------*/
//<S073>		if( OK != OBJ_SEARCH( pObj, RegNoX, &idx, Hmsg ) )
		if( OK != OBJ_SEARCH_SUB( pObj, RegNoX, &idx, &subidx, Hmsg ) )	/* <S073> */
		{
			Hmsg->ErrRno = (USHORT)RegNo;				/* 異常レジスタ番号							*/
			return( REGIF_REGNOERR );					/* レジスタ番号異常(REG番号無し)			*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		開始レジスタ整合チェック																	*/
/*--------------------------------------------------------------------------------------------------*/
//<S073>		if( pObj->GetNumber( pObj, idx, Hmsg ) != RegNoX )
		if( (pObj->GetNumber( pObj, idx, Hmsg ) +subidx )!= RegNoX )	/* <S073> */
		{
			Hmsg->ErrRno = (USHORT)RegNo;				/* 異常レジスタ番号							*/
			return( REGIF_DAMTHERR );					/* データ整合異常(LONG境界)					*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		レジスタデータ読出し																		*/
/*--------------------------------------------------------------------------------------------------*/
		for( i=0; i < Number; i++, idx++ )
		{
		/*------------------------------------------------------------------------------------------*/
		/*	システムパラメータ読出しチェック(行った方が良いかも！)									*/
		/*------------------------------------------------------------------------------------------*/
#if 0
			if( 0 && (PnPrmTbl[idx].PrmDefPtr->AccessLevel >= ACCLVL_SYSTEM) )
			{
				if( Hmsg->AccessLvl < ACCLVL_SYSTEM )
				{
					Hmsg->ErrRno = RegNo + i;			/* 異常レジスタ番号							*/
					return( REGIF_REGNOERR );			/* レジスタ番号異常							*/
				}
			}
#endif
		/*------------------------------------------------------------------------------------------*/
		/*	レジスタ番号の連続性チェック															*/
		/*------------------------------------------------------------------------------------------*/
//<S073>			if( pObj->GetNumber( pObj, idx, Hmsg ) != (RegNoX + i) )
			if( (pObj->GetNumber( pObj, idx, Hmsg ) +subidx) != (RegNoX + i) ) /* <S073> */
			{
				Hmsg->ErrRno = RegNo + i;				/* 異常レジスタ番号							*/
				return( REGIF_REGNOERR );				/* レジスタ番号異常(REG番号非連続)			*/
			}
		/*------------------------------------------------------------------------------------------*/
		/*	パラメータ読込み																		*/
		/*------------------------------------------------------------------------------------------*/
#if 1	/* <S073> Array対応 */
			do {
				/*----------------------------------------------------------------------------------*/
				/*	読込みデータはショート/ロング?													*/
				/*----------------------------------------------------------------------------------*/
				if( pObj->isLong( pObj, idx, Hmsg ) )
				{
					OBJ_READ_SUB( pObj, idx, subidx, (ULONG*)&pn_data, Hmsg );
					RdBuf[i+0]= GET_LWD_REGDATA( pn_data );		/* LONG  Parameter Low  Word		*/
					RdBuf[i+1]= GET_HWD_REGDATA( pn_data );		/* LONG  Parameter High Word		*/
					i++; subidx++;								/* increment index */
				}
				else
				{
					OBJ_READ_SUB( pObj, idx, subidx, (ULONG*)&pn_data, Hmsg );
					RdBuf[i] = (USHORT)(pn_data);				/* SHORT Parameter					*/
				}
				/*----------------------------------------------------------------------------------*/
				i++; subidx++;									/* increment index */
			} 
			while ( (i < Number) && (subidx < Lpx_GetMaxSubIdx( pTbl, idx ) ) );
			subidx = 0;
#else
			OBJ_READ( pObj, idx, &pn_data, Hmsg );
		/*------------------------------------------------------------------------------------------*/
		/*	読込みデータはショート/ロング?															*/
		/*------------------------------------------------------------------------------------------*/
			if( pObj->isLong( pObj, idx, Hmsg ) )
			{
				RdBuf[i+0]= GET_LWD_REGDATA( pn_data );	/* LONG  Parameter Low  Word				*/
				RdBuf[i+1]= GET_HWD_REGDATA( pn_data );	/* LONG  Parameter High Word				*/
				i++;
			}
			else
			{
				RdBuf[i] = (USHORT)(pn_data);			/* SHORT Parameter							*/
			}
#endif
		}
/*--------------------------------------------------------------------------------------------------*/
		return( REGIF_COMPLETE );
}



/****************************************************************************************************/
/*																									*/
/*		パラメータ書き込み処理(レジスタ指定)														*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	概要:	レジスタで指定されるパラメータ値をRAMへ書き込む。										*/
/*																									*/
/*	戻値:	ステータス																				*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
LONG	LpxWriteComObject(					/* パラメータ書き込み処理(レジスタ指定)					*/
		LONG	RegNo,						/* レジスタ番号											*/
		LONG	Number,						/* 書き込みデータ個数(WORD個数)							*/
		USHORT	*WrBuf,						/* 書き込みデータバッファ								*/
		HMSGIF	Hmsg			)			/* 書き込みIFハンドル									*/
{
LONG	rc;
LONG	i,idx;
LONG	subidx;								/* <S073> */
ULONG	pn_data;
LONG	RegNoX = RegNo & 0x0FFF;
COBJECT *pObj = Lpx_GetObject( RegNo );		/* Object Pointer */
CPRMTBL	*pTbl = GET_PNPRMTBL( pObj );		/* Parameter Table */
/*--------------------------------------------------------------------------------------------------*/
/*		レジスタアクセスエリアチェック																*/
/*--------------------------------------------------------------------------------------------------*/
		if( 0 )
		{
			Hmsg->ErrRno = (USHORT)RegNo;				/* 異常レジスタ番号							*/
			return( REGIF_DAMTHERR );					/* データ整合異常(グループ境界)				*/
		}
#if 0 
/*--------------------------------------------------------------------------------------------------*/
/*		システム定数パラメータチェック																*/
/*--------------------------------------------------------------------------------------------------*/
		if( (OpeReg.PrmAccess == FALSE) &&
			((RegNoX + Number) >= 0x0D00) && (Hmsg->AccessLvl != ACCLVL_SYSTEM) )
		{
			i = (RegNoX >= 0x0D00)? 0 : (0x0D00 - RegNoX);
			Hmsg->ErrRno = RegNo + i;					/* 異常レジスタ番号							*/
			return( REGIF_REGNOERR );					/* レジスタ番号異常							*/
		}
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		エンコーダ定数パラメータチェック															*/
/*--------------------------------------------------------------------------------------------------*/
#if 0
		if( ENC_PRM_WRT_DISABLE && ((RegNoX + Number) >= ENC_PRM_NUM) )
		{
			i = (RegNoX >= 0x0F00)? 0 : (ENC_PRM_NUM - RegNoX);
			Hmsg->ErrRno = RegNo + i;					/* 異常レジスタ番号							*/
			return( REGIF_REGNOERR );					/* レジスタ番号異常							*/
		}
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		レジスタ番号検索																			*/
/*--------------------------------------------------------------------------------------------------*/
//<S073>		if( OK != OBJ_SEARCH( pObj, RegNoX, &idx, Hmsg ) )
		if( OK != OBJ_SEARCH_SUB( pObj, RegNoX, &idx, &subidx, Hmsg ) )	/* <S073> */
		{
			Hmsg->ErrRno = (USHORT)RegNo;				/* 異常レジスタ番号							*/
			return( REGIF_REGNOERR );					/* レジスタ番号異常(REG番号無し)			*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		開始レジスタ整合チェック																	*/
/*--------------------------------------------------------------------------------------------------*/
//<S073>		if( pObj->GetNumber( pObj, idx, Hmsg ) != RegNoX )
		if( (pObj->GetNumber( pObj, idx, Hmsg ) +subidx )!= RegNoX )	/* <S073> */
		{
			Hmsg->ErrRno = (USHORT)RegNo;				/* 異常レジスタ番号							*/
			return( REGIF_DAMTHERR );					/* データ整合異常(LONG境界)					*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		システム定数パラメータチェック								<12J49>追加					 	*/
/*--------------------------------------------------------------------------------------------------*/
#if 0
/* レジスタ番号検索で取得した、「idx」を使用するために、本処理をこの場所へ持ってきている。		 	*/
		if( (pPrm->prmwr_spn == 0x0001) 									   /* Pn030 = 0x0001(モータPrm書き込み有効) */
		 && (PnPrmTbl[idx].PrmDefPtr->Attr.MotorPrmAmp == PRMDEF_MOTPRM_AMP)   /* アンプ内のモータPrm(主軸モータPrm) 	*/
		 && (PnPrmTbl[idx].PrmDefPtr != (&pndef_MotPrmSumChkEnb)) 			   /* PnFFFは除く 							*/
		 && (Hmsg->f.LnkMsgIF == TRUE) )									   /* SVPRM_WRを使用	 					*/
		{
			;
		}
		else if( (OpeReg.PrmAccess == FALSE) &&
				((RegNoX + Number) >= 0x0D00) && (Hmsg->AccessLvl != ACCLVL_SYSTEM) )
		{
			i = (RegNoX >= 0x0D00)? 0 : (0x0D00 - RegNoX);
			Hmsg->ErrRno = RegNo + i;					/* 異常レジスタ番号							*/
			return( REGIF_REGNOERR );					/* レジスタ番号異常							*/
		}
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		レジスタデータ書込み禁止チェック															*/
/*--------------------------------------------------------------------------------------------------*/
//		if( RndWrtDisable.FunA.f.PrmReg || RndWrtDisable.FunB.f.PrmReg )
//		{
//			Hmsg->ErrRno = (USHORT)RegNo;				/* 異常レジスタ番号							*/
//			return( REGIF_REGNOERR );					/* レジスタ番号異常							*/
//		}
/*--------------------------------------------------------------------------------------------------*/
/*		レジスタデータ書込み																		*/
/*--------------------------------------------------------------------------------------------------*/
		for( i=0; i < Number; i++, idx++ )
		{
		/*------------------------------------------------------------------------------------------*/
		/*	TODO: パラメータアクセスレベルチェック													*/
		/*------------------------------------------------------------------------------------------*/
		/*------------------------------------------------------------------------------------------*/
		/*	レジスタ番号の連続性チェック															*/
		/*------------------------------------------------------------------------------------------*/
//<S073>			if( pObj->GetNumber( pObj, idx, Hmsg ) != (RegNoX + i) )
			if( (pObj->GetNumber( pObj, idx, Hmsg ) +subidx) != (RegNoX + i) )	/* <S073> */
			{
				Hmsg->ErrRno = RegNo + i;				/* 異常レジスタ番号							*/
				return( REGIF_REGNOERR );				/* レジスタ番号異常(REG番号非連続)			*/
			}
#if 1	/* <S073> Array */
		/*------------------------------------------------------------------------------------------*/
		/*	パラメータ書込み																		*/
		/*------------------------------------------------------------------------------------------*/
			do {
				/*----------------------------------------------------------------------------------*/
				/*	書込みデータはロング?															*/
				/*----------------------------------------------------------------------------------*/
				if( pObj->isLong( pObj, idx, Hmsg ) )
				{
					if( (i + subidx + 1) < Number )
					{
						pn_data = ULONGOF_REGDATA( WrBuf[i+subidx], WrBuf[i+subidx+1] );
					}
					else
					{
						Hmsg->ErrRno = RegNo + i;			/* 異常レジスタ番号						*/
						return( REGIF_DAMTHERR );			/* データ整合異常(LONG境界)				*/
					}
				}
				else
				{
					pn_data = WrBuf[i];
					if( CHK_PNPRM_SIGNED( GET_PNPRMDEF( pTbl, idx ) ) )
					{
						pn_data = (LONG)((SHORT)pn_data); 	/* 符号付ワードの場合、符号拡張しておく	*/
					}
				}
				/*----------------------------------------------------------------------------------*/
				/*	パラメータRAM書込み																*/
				/*----------------------------------------------------------------------------------*/
				if( pObj->isLong( pObj, idx, Hmsg ) )
				{
					rc = OBJ_WRITE_SUB( pObj, idx, subidx, pn_data, Hmsg );		/* @@ */
					i++;
					subidx++;
				}
				else
				{
					rc = OBJ_WRITE_SUB( pObj, idx, subidx, pn_data, Hmsg );		/* @@ */
				}
				/*----------------------------------------------------------------------------------*/
				/*	Error Check																		*/
				/*----------------------------------------------------------------------------------*/
				if(  rc != OK )
				{
					Hmsg->ErrRno = RegNo + i;				/* 異常レジスタ番号						*/
					switch(rc)
					{
					case OERR_DFIT:
						 return( REGIF_DAMTHERR );			/* データ整合異常(LONG境界)				*/
//						 break;//<S0C7>statement is unreachable
					case OERR_RONLY:
						 return( REGIF_REGNOERR );			/* レジスタ番号異常						*/
//						 break;//<S0C7>statement is unreachable
					case OERR_CALC:
						 return( REGIF_DACALERR );			/* データ異常(演算異常) 				*/
//						 break;//<S0C7>statement is unreachable
					case OERR_RANGE:
						 return( REGIF_DALMTERR );			/* データ異常(上下限異常)				*/
//						 break;//<S0C7>statement is unreachable
					case OERR_PRMCOPY:
					case OERR_WRITE_HANDLE:
						 return( REGIF_FOPEERR3 );			/* 処理不可(他ＣＨとの処理競合)			*/
//						 break;//<S0C7>statement is unreachable
					default:
						 return( REGIF_REGNOERR );			/* レジスタ番号異常						*/
//						 break;//<S0C7>statement is unreachable
					}
				}
			} while ((i < Number) && (subidx < Lpx_GetMaxSubIdx( pTbl, idx ) ) );
			subidx = 0;
#else	
		/*------------------------------------------------------------------------------------------*/
		/*	書込みデータはロング?																	*/
		/*------------------------------------------------------------------------------------------*/
			if( pObj->isLong( pObj, idx, Hmsg ) )
			{
				if( (i + 1) < Number )
				{
					pn_data = ULONGOF_REGDATA( WrBuf[i], WrBuf[i+1] );
				}
				else
				{
					Hmsg->ErrRno = RegNo + i;			/* 異常レジスタ番号							*/
					return( REGIF_DAMTHERR );			/* データ整合異常(LONG境界)					*/
				}
			}
			else
			{
				pn_data = WrBuf[i];
				if( CHK_PNPRM_SIGNED( GET_PNPRMDEF( pTbl, idx ) ) )
				{
					pn_data = (LONG)((SHORT)pn_data); 	/* 符号付ワードの場合、符号拡張しておく	*/
				}
			}
		/*------------------------------------------------------------------------------------------*/
		/*	パラメータRAM書込み																		*/
		/*------------------------------------------------------------------------------------------*/
			rc = OBJ_WRITE( pObj, idx, pn_data, Hmsg );
			if(  rc != OK )
			{
				Hmsg->ErrRno = RegNo + i;				/* 異常レジスタ番号							*/
				switch(rc)
				{
				case OERR_DFIT:
					 return( REGIF_DAMTHERR );			/* データ整合異常(LONG境界)					*/
					 break;
				case OERR_RONLY:
					 return( REGIF_REGNOERR );			/* レジスタ番号異常							*/
					 break;
				case OERR_CALC:
					 return( REGIF_DACALERR );			/* データ異常(演算異常) 					*/
					 break;
				case OERR_RANGE:
					 return( REGIF_DALMTERR );			/* データ異常(上下限異常)					*/
					 break;
				case OERR_PRMCOPY:
				case OERR_WRITE_HANDLE:
					 return( REGIF_FOPEERR3 );			/* 処理不可(他ＣＨとの処理競合)				*/
					 break;
				default:
					 return( REGIF_REGNOERR );			/* レジスタ番号異常							*/
					 break;
				}
			}
		/*------------------------------------------------------------------------------------------*/
		/*	TODO: パラメータEEPROM書込み															*/
		/*------------------------------------------------------------------------------------------*/

		/*------------------------------------------------------------------------------------------*/
		/*	LONGデータの場合																		*/
		/*------------------------------------------------------------------------------------------*/
			if( pObj->isLong( pObj, idx, Hmsg ) )
			{
				i++;
			}
#endif
		}
/*--------------------------------------------------------------------------------------------------*/
		return( REGIF_COMPLETE );
}


/****************************************************************************************************/
/*																									*/
/*		Read Memory Object																			*/
/*																									*/
/****************************************************************************************************/
LONG	LpxReadMemObject(					/* Read Memory Object									*/
		LONG	RegNo,						/* レジスタ番号											*/
		LONG	Number,						/* 読出しデータ個数(WORD個数)							*/
		USHORT	*RdBuf,						/* 読出しデータバッファ									*/
		HMSGIF	Hmsg			)			/* 読出しIFハンドル										*/
{
LONG	i;
LONG	pn_data;
COBJECT *pObj = Lpx_GetObject( RegNo );		/* Object Pointer */
LONG	idx = RegNo - 0x4000;				/* Trace Data Index */
/*--------------------------------------------------------------------------------------------------*/
/*		レジスタデータ読出し																		*/
/*--------------------------------------------------------------------------------------------------*/
		for( i=0; i < Number; i++, idx++ )
		{
		/*------------------------------------------------------------------------------------------*/
		/*	パラメータ読込み																		*/
		/*------------------------------------------------------------------------------------------*/
			OBJ_READ( pObj, idx, (ULONG*)&pn_data, Hmsg );
			RdBuf[i] = (USHORT)(pn_data);			
		}
/*--------------------------------------------------------------------------------------------------*/
		return( REGIF_COMPLETE );
}

/****************************************************************************************************/
/*																									*/
/*		Write Memory Object																			*/
/*																									*/
/****************************************************************************************************/
LONG	LpxWriteMemObject(					/* Write Memory Object									*/
		LONG	RegNo,						/* レジスタ番号											*/
		LONG	Number,						/* 書き込みデータ個数(WORD個数)							*/
		USHORT	*WrBuf,						/* 書き込みデータバッファ								*/
		HMSGIF	Hmsg			)			/* 書き込みIFハンドル									*/
{
LONG	i;
ULONG	pn_data;
COBJECT *pObj = Lpx_GetObject( RegNo );		/* Object Pointer */
LONG	idx = RegNo - 0x4000;				/* Trace Data Index */
/*--------------------------------------------------------------------------------------------------*/
/*		レジスタデータ書込み																		*/
/*--------------------------------------------------------------------------------------------------*/
		for( i=0; i < Number; i++, idx++ )
		{
			pn_data = WrBuf[i];
		/*------------------------------------------------------------------------------------------*/
		/*	パラメータRAM書込み																		*/
		/*------------------------------------------------------------------------------------------*/
			OBJ_WRITE( pObj, idx, pn_data, Hmsg );
		}
/*--------------------------------------------------------------------------------------------------*/
		return( REGIF_COMPLETE );
}




 /****************************************************************************************************/
/*																									*/
/*		Memobus：IngramのMemobus																	*/
/*																									*/
/****************************************************************************************************/
#if 0	/* <S150> */
void	LpxMbusTstUsbProcedure( HMSGIF Hmsg )
{
LONG	MsgLen;
CSIF	*Sif = Hmsg->Sif;
USB_MBUS_SERVER *pUsbMbusServer = (USB_MBUS_SERVER *)Sif->Com;
	MsgLen = MbusFuncMsgProcedure( &(pUsbMbusServer->MbusIf) ); 				/* メッセージ解析＆実行処理 */
	MlibCopyByteMemory( Hmsg->ResBuf, pUsbMbusServer->MbusIf.TxBuf, MsgLen  );	/* TODO: あとで消す */
	MSGIF_SEND( Hmsg->ResBuf, MsgLen );											/* Start Tx */
	return;
}
#endif	/* <S150> */

 /****************************************************************************************************/
/*																									*/
/*		Memobus：IngramのMemobus																	*/
/*																									*/
/****************************************************************************************************/
#if 0	/* <S150> */
void	LpxMbusTstUartProcedure( HMSGIF Hmsg )
{
LONG	MsgLen;
CSIF	*Sif = Hmsg->Sif;
UART_MBUS_SERVER *pUartMbusServer = (UART_MBUS_SERVER *)Sif->ComMbus42;
	MsgLen = MbusFuncMsgProcedure( &(pUartMbusServer->MbusIf) ); 				/* メッセージ解析＆実行処理 */
	MsgLen += 2;
	MlibSetCRC16(pUartMbusServer->MbusIf.TxBuf, MsgLen);
	Hmsg->ErrRno = pUartMbusServer->MbusIf.ErrRno;								/*  レジスタ読み出し:MEMOBUSエラー情報 */	/* <S056> */
	MlibCopyByteMemory( Hmsg->ResBuf, pUartMbusServer->MbusIf.TxBuf, MsgLen  );	/* TODO: あとで消す */
	MSGIF_SEND( Hmsg->ResBuf, MsgLen );											/* Start Tx */
	return;
}
#endif	/* <S150> */

/* <S150> >>>>> */
/****************************************************************************************************/
/*																									*/
/*		MEMOBUS MESSAGE MANAGER																		*/
/*																									*/
/****************************************************************************************************/
void LpxMbusMechatrolink( HMSGIF Hmsg, LONG *MsgLen )
{
	LONG	SubFuncCode;
	UCHAR	*CmdBuf;

	CmdBuf = Hmsg->CmdBuf;		/* 受信メッセージ先頭アドレス */
	SubFuncCode = (LONG)CmdBuf[4];	/* サブファンクションコード   */
	
	//KpiRstLongTimer( &(MbusIfPtr->ComTimeLong) );		/* コマンド応答タイムアウト用	*/
	#if 0 /* 書き込み禁止処理現状未対応 */
	RpiSetAccsLvl( &Access, MsgPtr->AccsLvl );
	#endif

	switch( SubFuncCode )/* サブファンクションコード */
	{
		case MBUS_RD_CMEM_42H:		/* 01H メモリの読み出し（連続） */
			mbusFuncReadMemory42H( Hmsg, SubFuncCode, MsgLen );
			break;
		case MBUS_WR_CMEM_42H:		/* 02H メモリの書き込み（連続)	*/
			mbusFuncWriteMemory42H( Hmsg, SubFuncCode, MsgLen );
			break;
		case MBUS_RD_NCMEM_42H:		/* 03H メモリの読み出し（非連続)*/
			mbusFuncReadMemory42H( Hmsg, SubFuncCode, MsgLen );
			break;
		case MBUS_WR_NCMEM_42H:		/* 04H メモリの書き込み（非連続)*/
			mbusFuncWriteMemory42H( Hmsg, SubFuncCode, MsgLen );
			break;
		case MBUS_RD_MAXSIZE_42H:	/* 11H 最大サイズ読み出し		*/
			mbusFuncReadMaxSize42H( Hmsg, SubFuncCode, MsgLen );
			break;

/* <S061>	Start */
//#ifdef _INFO_ID_READ_CHECK_	/* <S205> Delete */
		case MBUS_RD_INFO_42H:	/* ffH(未定) 製品情報読み出し	*/
			mbusFuncReadProductInfo42H( Hmsg, SubFuncCode, MsgLen );
			break;
//#endif						/* <S205> Delete */
/* <S061>	End */

		default:					/* コマンド不一致				*/
			mbusFuncErrorManager42H( Hmsg, FUNCCODE_ERROR, 0, MsgLen );
			break;
	}

	/* 送信データ長チェック */
	if(( *MsgLen > MBUS_MAXSIZE ) || ( *MsgLen < MBUS_MINSIZE ) )
	{
		/* 送信データ長異常	*/
		mbusFuncErrorManager42H( Hmsg, DATANUMB_ERROR, 0, MsgLen );
	}

	 /* メカトロリンクからのコマンドでなければ、CRCを付加する */
	 if( !Hmsg->f.MlnkMsgIF )
	 {
		/* CRC付加 */
		*MsgLen += 2;
		MlibSetCRC16MB( Hmsg->ResBuf, *MsgLen );
	 }
}


/****************************************************************************************************/
/*																									*/
/*		MEMOBUS MESSAGE MANAGER																		*/
/*																									*/
/****************************************************************************************************/
void LpxMbusLink( HMSGIF Hmsg )
{
	LONG	Offset;
	LONG	RouteNum;
	LONG	Length;
	LONG	MsgLen;
	UCHAR	*CmdBuf;
	UCHAR	*ResBuf;
	USHORT	RelayHdSize;

	/* 変数初期化 */
	MsgLen = 0;

	CmdBuf = Hmsg->CmdBuf;		/* 受信メッセージ先頭アドレス */
	ResBuf = Hmsg->ResBuf;		/* 送信メッセージ先頭アドレス */

	/* 中継段数	*/
	RouteNum = (CmdBuf[5] & 0x0F);				

	/* 中継コマンド長 */
	RelayHdSize = (USHORT)(RouteNum * MBUS_RLY_PASS_DATA_SIZE + MBUS_RLY_FIX_SIZE);

	/* 受信・送信バッファポインタ取得	*/
	Hmsg->CmdBuf = CmdBuf + RelayHdSize;
	Hmsg->ResBuf = ResBuf + RelayHdSize;

	/* ヘッダのオフセット長を算出 */
	Offset = RelayHdSize;

	/* データ部コピー */
	MlibCopyByteMemory( ResBuf, CmdBuf, RelayHdSize);

	ResBuf[4] |= 0x80;					/* 応答フラグ						*/
	ResBuf[5]  = (UCHAR)RouteNum;		/* 通過段数クリア&中継段数セット	*/

	/* 中継段数オーバー or 中継段数と通過段数が異なる場合はエラー */
	if( (RouteNum > MBUS_RLY_MAX_ROUTENUM) || (RouteNum != (CmdBuf[5] >> 4)) )
	{/* 中継段数エラー */

		/* 受信メッセージエラー */

		/* メッセージ先頭に戻す */
		Hmsg->CmdBuf = CmdBuf;		/* 受信メッセージ先頭アドレス */
		Hmsg->ResBuf = ResBuf;		/* 送信メッセージ先頭アドレス */

		/* データ送信（無応答） */
		MSGIF_SEND( Hmsg->ResBuf, MsgLen );

		return;
	}
	else
	{/* 中継段数正常 */

		/* 受信データサイズ(バイト単位)を格納 */
		Hmsg->CmdMsgLen =(USHORT)( ((ULONG)CmdBuf[Offset-4] << 24) 
								 | ((ULONG)CmdBuf[Offset-3] << 16) 
								 | ((ULONG)CmdBuf[Offset-2] <<  8) 
								 | ((ULONG)CmdBuf[Offset-1] <<  0) );

		/* 受信データ長(バイト単位)チェック */
		if(( Hmsg->CmdMsgLen < MBUS_MINSIZE ) || ( Hmsg->CmdMsgLen > MBUS_MAXSIZE ))
		{
			/* 受信メッセージエラー */

			/* メッセージ先頭に戻す */
			Hmsg->CmdBuf = CmdBuf;		/* 受信メッセージ先頭アドレス */
			Hmsg->ResBuf = ResBuf;		/* 送信メッセージ先頭アドレス */

			/* データ送信（無応答） */
			MSGIF_SEND( Hmsg->ResBuf, MsgLen );

			return;	
		}
#ifdef __MYAD_CHECK__
		/* 自局アドレスチェック	*/
		else if( (FALSE == mbusFuncChkMyMsg( Hmsg, CmdBuf )) 
			  || (FALSE == mbusFuncChkMyMsg( Hmsg, Hmsg->CmdBuf )))
		{
			/* メッセージ先頭に戻す */
			Hmsg->CmdBuf = CmdBuf;		/* 受信メッセージ先頭アドレス */
			Hmsg->ResBuf = ResBuf;		/* 送信メッセージ先頭アドレス */

			/* データ送信（無応答） */
			MSGIF_SEND( Hmsg->ResBuf, MsgLen );

			return;
		}
#endif
		/* ファンクションコードチェック */
		else if( Hmsg->CmdBuf[1] != MBUS_FC_42H )
		{
			/* ファンクションコードエラー */
			mbusFuncErrorManager42H( Hmsg, FUNCCODE_ERROR, 0, &MsgLen );
		}
#ifdef __EXAD_CHECK__
		/* 拡張アドレスチェック */
		else if(Hmsg->CmdBuf[2] != 0xFF)
		{
			mbusFuncErrorManager42H( Hmsg, SELAXIS_ERROR, 0, &MsgLen );
		}
#endif
		else
		{
			/* メッセージ処理実行 */
			LpxMbusMechatrolink( Hmsg, &MsgLen );
		}

		/* メッセージ先頭に戻す */
		Hmsg->CmdBuf = CmdBuf;		/* 受信メッセージ先頭アドレス */
		Hmsg->ResBuf = ResBuf;		/* 送信メッセージ先頭アドレス */

		/* 送信データサイズに中継コマンド分のサイズを加算 */
		MsgLen += RelayHdSize;

		/* データ部のサイズを算出 */
		Length = MsgLen - Offset;

		/* 送信データサイズ(バイト単位)を設定 */
		mbusFuncSetArrayLongData( &Hmsg->ResBuf[ Offset - 4 ], (ULONG)Length, sizeof(LONG) );

		/* データ送信 */
		MSGIF_SEND( Hmsg->ResBuf, MsgLen );
	}
}

/****************************************************************************************************/
/*																									*/
/*		Memobus：メモリ読み出し（連続／非連続共通）													*/
/*																									*/
/****************************************************************************************************/
static void	mbusFuncReadMemory42H( HMSGIF Hmsg, LONG SubFuncCode, LONG *TrnbLength )
{
#ifdef __RCVLEN_CHECK__
	LONG	CmdLength;
#endif
//	UCHAR	Mode; 
	UCHAR	DataType;
	UCHAR	dIndex;
	USHORT	DataNum;
	UINT	AccessSize,RegSize;
	REG_ACC_T AccessPrm;
	LONG	i;
	LONG	ret;
	LONG	Value;
	LONG	Offset;
	LONG	RspLength;
	LONG	BlockNum;
	ULONG	BeginAddress;
	ULONG	CurrentAddress;
	REG_MANAGER_HANDLE	*hRegMngr;

	/* 受信、送信データ先頭アドレス */
	UCHAR  *CmdBuf;	
	UCHAR  *ResBuf;

	CmdBuf = Hmsg->CmdBuf;
	ResBuf = Hmsg->ResBuf;

	Value  = 0;
	//Mode      = (CmdBuf[5] >> 4);					/* Mode		*/
	DataType  = (CmdBuf[5] & 0x0F);					/* DataType */
	DataNum   = (CmdBuf[6] << 8) + CmdBuf[7];		/* DataNum	*/
	

	/* データ型チェック */
	if( (DataType != 1) && (DataType != 2) && (DataType != 3))	
	{
		/* モード/データ型エラー*/
		mbusFuncErrorManager42H( Hmsg, MODETYPE_ERROR, 0, TrnbLength );
		return;	
	}
	AccessSize   = (UCHAR)(0x0001 << (DataType-1));					/* アクセスサイズ		  */
	BeginAddress = mbusFuncLongSwap( &CmdBuf[MBUS_HEAD_FIX_SIZE]);	/* 読み出しデータアドレス */


	/* 受信データ長、送信データ長、データ個数チェック */
	if (CmdBuf[4] == MBUS_RD_CMEM_42H)
	{/* 連続メモリ読み出し */ 
#ifdef __RCVLEN_CHECK__
		CmdLength = MBUS_HEAD_BASE_SIZE;	/* 受信データ長(メッセージ全長) */
#endif
		RspLength = AccessSize * DataNum;	/* 送信データ長(データ部)		*/	
	}
	else
	{/* 非連続メモリ読み出し */ 
		//CmdLength = MBUS_HEAD_FIX_SIZE + 4 * DataNum;
		RspLength = 4 * DataNum;		
	}
#ifdef __RCVLEN_CHECK__
	if(  (CmdLength != Hmsg->CmdMsgLen )
		|| (RspLength > (MBUS_MAXSIZE - MBUS_HEAD_FIX_SIZE)) || (DataNum < MBUS_MIN_REGNUM)) 
#else
	if( (RspLength > (MBUS_MAXSIZE - MBUS_HEAD_FIX_SIZE)) || (DataNum < MBUS_MIN_REGNUM)) 
#endif
	{	/* 個数不良エラー */
		mbusFuncErrorManager42H( Hmsg, DATANUMB_ERROR, BeginAddress, TrnbLength );
		return;
	}

	for ( i = 0; i < DataNum; i++ )
	{
		if (CmdBuf[4] == MBUS_RD_CMEM_42H)
		{/* 連続メモリ読み出し */ 
			CurrentAddress = BeginAddress + (AccessSize * i);/* 読み出しデータアドレス	*/
		}
		else
		{/* 非連続メモリ読み出し */ 
			CurrentAddress = mbusFuncLongSwap( &CmdBuf[MBUS_HEAD_FIX_SIZE + 4*i] );			
		}

		/* 仮想メモリ　→　実アドレス変換 */
		AccessPrm.MemAddr = CurrentAddress;
		//hRegMngr = mbusFuncConvAddress(MbusIfPtr->AxisMap,&AccessPrm.MemAddr,&ret);		
		hRegMngr = mbusFuncConvAddress(Hmsg->AxisMap,&AccessPrm.MemAddr);		
		if( hRegMngr == NULL )
		{	/* メモリアドレスエラー */
			//mbusFuncErrorManager42H( MbusIfPtr, ret, CurrentAddress );//kira_1202
			mbusFuncErrorManager42H( Hmsg, SELAXIS_ERROR, CurrentAddress, TrnbLength );
			return;
		}

		/* 格納するデータ位置までのオフセットを計算	*/
		Offset = MBUS_HEAD_FIX_SIZE + i*AccessSize;

		/* パラメータ属性取得 */
		if( NULL == RegMngrGetAttrByMemAddr(hRegMngr, &AccessPrm) )
		{	/* メモリアドレスエラー	*/
			mbusFuncErrorManager42H( Hmsg, MEMADRES_ERROR, CurrentAddress, TrnbLength );
			return;
		}
		/* 読み出しパラメータサイズ */
		RegSize = AccessPrm.cPrmDef->DataLength;

		/*------------------------------------------------------------------------------------------*/
		/*	ブロック読み出し																		*/
		/*------------------------------------------------------------------------------------------*/
		BlockNum = RegSize/AccessSize;		/* ブロック読み出し回数 */

		if( (BlockNum > 0) && ( DataNum > (i + BlockNum - 1)) )
		{
			if(SubFuncCode == MBUS_RD_NCMEM_42H)
			{/* 非連続メモリ読み出し */

				dIndex = 4;

				/* アドレス連続チェック */
				ret = mbusFuncCheckBlockAccess( &CmdBuf[MBUS_HEAD_FIX_SIZE + dIndex*i], 
																CurrentAddress, AccessSize,BlockNum,dIndex);
				if( ret != 0 )
				{	
					/* アドレス指定が連続していない場合エラー */
					mbusFuncErrorManager42H( Hmsg, DATANUMB_ERROR, CurrentAddress, TrnbLength );
					return;
				}
			}
			i = i + (BlockNum-1);				/* ブロック読み出し回数分iを加算する	*/
		}
		else
		{	
			/* 個数不良 */
			mbusFuncErrorManager42H( Hmsg, DATANUMB_ERROR, CurrentAddress, TrnbLength );
			return;
		}

		/* データ読み出し */
		AccessPrm.IfHandle = Hmsg;
		ret = (LONG)PrmReadValue( &AccessPrm, &Value, RegSize);
		if( PRM_RSLT_SUCCESS != ret )
		{
			mbusFuncErrorManager42H( Hmsg, ret, CurrentAddress, TrnbLength );
			return;
		}

		/* 送信データセット */
		switch (RegSize)
		{
			case 1:		/* byte型 */
				ResBuf[Offset] = (UCHAR)(Value);
				break;
			case 2:		/* short型 */
				mbusFuncSetArrayShortData( &ResBuf[Offset], (USHORT)Value, AccessSize );
				break;
			case 4:		/* long型 */
				mbusFuncSetArrayLongData( &ResBuf[Offset], (ULONG)Value, AccessSize );
				break;
		}
	}

	MlibCopyByteMemory( ResBuf, CmdBuf, MBUS_HEAD_FIX_SIZE);			/* 応答データ作成 */
	*TrnbLength = MBUS_HEAD_FIX_SIZE + AccessSize*DataNum;	/* 送信バッファサイズ決定 */
}

/****************************************************************************************************/
/*																									*/
/*		Memobus：メモリ書き込み（連続／非連続共通）													*/
/*																									*/
/****************************************************************************************************/
static void	mbusFuncWriteMemory42H( HMSGIF Hmsg, LONG SubFuncCode, LONG *TrnbLength )
{
#ifdef __RCVLEN_CHECK__
	LONG	CmdLength;
#endif
//	UCHAR	Mode;
	USHORT	DataNum;
	UCHAR	AccessSize;
	UCHAR	DataType;
	UCHAR	dIndex;
	REG_ACC_T AccessPrm;
	LONG	i;
	LONG	ret;
	LONG	Value;
	LONG	Offset;
	LONG	MaxSize;
	LONG	RspLength;
	LONG	BlockNum;
	ULONG	BeginAddress; 
	ULONG	CurrentAddress;
	UINT	RegSize;
	REG_MANAGER_HANDLE	*hRegMngr;

	UCHAR  *CmdBuf;	/* 受信バッファ */
	UCHAR  *ResBuf;	/* 送信バッファ */

	/* 受信、送信データ先頭アドレス格納 */
	CmdBuf = Hmsg->CmdBuf;
	ResBuf = Hmsg->ResBuf;

//	Mode     = (CmdBuf[5] >> 4);					/* モード	*/
	DataType = (CmdBuf[5] & 0x0F);					/* データ型 */
	DataNum  = (CmdBuf[6] << 8) + CmdBuf[7];		/* データ数	*/

	/* データ型チェック */
	if( (DataType != 1) && (DataType != 2) && (DataType != 3))	
	{	/* モード/データ型エラー*/
		mbusFuncErrorManager42H( Hmsg, MODETYPE_ERROR, 0, TrnbLength );
		return;	
	}
	AccessSize   = (UCHAR)(0x0001 << (DataType-1));					/* アクセスサイズ			*/
	BeginAddress = mbusFuncLongSwap( &CmdBuf[MBUS_HEAD_FIX_SIZE] );	/* 書き込みデータアドレス	*/

	/* 受信データ長、送信データ長、データ個数チェック */
	if (CmdBuf[4] == MBUS_WR_CMEM_42H)
	{/* 連続メモリ書き込み */
#ifdef __RCVLEN_CHECK__
		CmdLength = MBUS_HEAD_BASE_SIZE + AccessSize * DataNum;	/* 受信データ長(メッセージ全長) */
#endif
		RspLength = AccessSize * DataNum;						/* 送信データ長(データ部)		*/
		MaxSize   = MBUS_MAXSIZE - MBUS_HEAD_BASE_SIZE;			/* 最大バッファサイズ(データ部)	*/
	}
	else
	{/* 非連続メモリ書き込み */ 
#ifdef __RCVLEN_CHECK__
		CmdLength = MBUS_HEAD_FIX_SIZE + 8 * DataNum;
#endif
		RspLength = 8 * DataNum;
		MaxSize   = MBUS_MAXSIZE - MBUS_HEAD_FIX_SIZE;
	}

#ifdef __RCVLEN_CHECK__
	if( (Hmsg->CmdMsgLen != CmdLength ) || (RspLength > MaxSize) || (DataNum < MBUS_MIN_REGNUM) )
#else
	if( (RspLength > MaxSize) || (DataNum < MBUS_MIN_REGNUM) )
#endif
	{	/* 個数不良 */
		mbusFuncErrorManager42H( Hmsg, DATANUMB_ERROR, BeginAddress,TrnbLength );		
		return;
	}

	for ( i = 0; i < DataNum; i++ )
	{ 
		if (CmdBuf[4] == MBUS_WR_CMEM_42H)
		{/* 連続メモリ書き込み */
			CurrentAddress   = BeginAddress + (AccessSize * i);	/* 書き込みデータアドレス	*/
			Offset = MBUS_HEAD_BASE_SIZE + i*AccessSize;		/* 書き込みデータオフセット */
			dIndex = AccessSize;
		}
		else
		{/* 非連続メモリ書き込み */ 
			CurrentAddress = mbusFuncLongSwap( &CmdBuf[MBUS_HEAD_FIX_SIZE + 8*i] );
			Offset = MBUS_HEAD_BASE_SIZE + i*sizeof(LONG)*2;
			dIndex = sizeof(LONG)*2;
		}

		/* 仮想メモリ → 実メモリアドレス変換 */
		AccessPrm.MemAddr = CurrentAddress;
		hRegMngr = mbusFuncConvAddress(Hmsg->AxisMap,&AccessPrm.MemAddr);		
		if( hRegMngr == NULL )
		{	/* メモリアドレスエラー */
			mbusFuncErrorManager42H( Hmsg, SELAXIS_ERROR, CurrentAddress, TrnbLength );
			return;
		}

		/* パラメータ属性取得 */
		if( NULL == RegMngrGetAttrByMemAddr(hRegMngr, &AccessPrm) )
		{	/* メモリアドレスエラー	*/
			mbusFuncErrorManager42H( Hmsg, MEMADRES_ERROR, CurrentAddress, TrnbLength );
			return;
		}

		/* パラメータサイズ取得 */
		RegSize = AccessPrm.cPrmDef->DataLength;

		/*------------------------------------------------------------------------------------------*/
		/*	ブロック書き込み																		*/
		/*------------------------------------------------------------------------------------------*/
		BlockNum = RegSize/AccessSize;		/* ブロック書き込み回数 */

		if( (BlockNum > 0) && ( DataNum > (i + BlockNum-1)) )
		{
			if(SubFuncCode == MBUS_WR_NCMEM_42H)
			{/* 非連続メモリ書き込み */

				/* アドレス連続チェック */
				ret = mbusFuncCheckBlockAccess( &CmdBuf[MBUS_HEAD_FIX_SIZE + dIndex*i], 
																CurrentAddress, AccessSize, BlockNum, dIndex);
				if( ret != 0 )
				{	/* アドレス指定が連続していない場合エラー */
					mbusFuncErrorManager42H( Hmsg, DATANUMB_ERROR, CurrentAddress, TrnbLength );
					return;
				}
			}
			i = i + (BlockNum-1);	/* ブロック書き込み回数分iを加算する	*/
			Value = 0;				/* 書き込み値初期化						*/

			switch(AccessSize)
			{
				case 1:
					Value = mbusFuncSetValueByteData( &CmdBuf[Offset], BlockNum, dIndex );
					break;
				case 2:
					Value = mbusFuncSetValueShortData( &CmdBuf[Offset], BlockNum, dIndex );
					break;
				case 4:
					Value = mbusFuncSetValueLongData( &CmdBuf[Offset], BlockNum, dIndex );
					break;
				default:/* ここに来ることはない */
					break;
			}
		}
		else
		{	/* 個数不良 */
			mbusFuncErrorManager42H( Hmsg, DATANUMB_ERROR, CurrentAddress, TrnbLength );
			return;
		}

		/*------------------------------------------------------------------------------------------*/
		/*	<S166> 操作レジスタ書き込み権取得														*/
		/*------------------------------------------------------------------------------------------*/
		ret = mbusCheckOperationWriteHandle( &AccessPrm, Hmsg );
		if ( ret != OK ) { 
			mbusFuncErrorManager42H( Hmsg, COMPETON_ERROR, CurrentAddress, TrnbLength );
		}
		/*------------------------------------------------------------------------------------------*/

		/* データ書き込み */
		AccessPrm.IfHandle = Hmsg;
		ret = (LONG)PrmWriteValue( &AccessPrm, &Value, RegSize);
		if(0 > ret)
		{
			mbusFuncErrorManager42H( Hmsg, ret, CurrentAddress, TrnbLength );
			return;
		}

		/*------------------------------------------------------------------------------------------*/
		/*	<S166> 操作レジスタ書き込み権解放														*/
		/*------------------------------------------------------------------------------------------*/
		mbusFreeOperationWriteHandle( &AccessPrm, Hmsg );
	}

	MlibCopyByteMemory( ResBuf, CmdBuf, MBUS_HEAD_FIX_SIZE);	/* 応答データ作成 */
	*TrnbLength = MBUS_HEAD_FIX_SIZE;							/* 送信バッファサイズ決定 */

}

/****************************************************************************************************/
/*																									*/
/*		Memobus：最大サイズ読み出し																	*/
/*																									*/
/****************************************************************************************************/
static void	mbusFuncReadMaxSize42H( HMSGIF Hmsg, LONG SubFuncCode, LONG *TrnbLength )
{
	LONG	MsgSize;

	UCHAR  *CmdBuf;//受信バッファ先頭ポインタ
	UCHAR  *ResBuf;//送信バッファ先頭ポインタ

	/* 受信、送信データ先頭アドレス格納 */
	CmdBuf = Hmsg->CmdBuf;
	ResBuf = Hmsg->ResBuf;

#ifdef __RCVLEN_CHECK__
	/*----------------------------------------------------------------------------------------------*/
	/*		受信データ長チェック																	*/
	/*----------------------------------------------------------------------------------------------*/
	/* 受信データ長が不正 */
	if( Hmsg->CmdMsgLen != MBUS_HEAD_FIX_SIZE )
	{	/* 個数不良エラー	*/
		mbusFuncErrorManager42H( Hmsg, DATANUMB_ERROR, BeginAddress, TrnbLength );
		return;
	}
#endif

	/* 最大サイズ読み出しサイズ */
	MsgSize = MBUS_MAXSIZE - MBUS_HEAD_FIX_SIZE;

	/* 応答データ作成 */
	ResBuf[0]  = CmdBuf[0];								/* 0:Slave Addrress		*/
	ResBuf[1]  = CmdBuf[1];								/* 1:Function Code		*/
	ResBuf[2]  = CmdBuf[2];								/* 2:Extended Address	*/
	ResBuf[3]  = CmdBuf[3];								/* 3:Reserve1			*/
	ResBuf[4]  = CmdBuf[4];								/* 4:Sub Function Code	*/
	ResBuf[5]  = CmdBuf[5];								/* 5:Mode,DataType 		*/
	ResBuf[6]  = 0;										/* 6:DataNumH			*/
	ResBuf[7]  = 0;										/* 7:DataNumL			*/
	mbusFuncSetArrayLongData( &ResBuf[8], (ULONG)MsgSize, 4);	/* 8-11 MaxMessageSize	*/

	/* 送信バッファサイズ決定 */
	*TrnbLength = MBUS_HEAD_BASE_SIZE;

}

/****************************************************************************************************/
/*																									*/
/*		Memobus：異常応答返信処理																	*/
/*																									*/
/****************************************************************************************************/
static void	mbusFuncErrorManager42H( HMSGIF Hmsg, LONG ErrorCode, ULONG ErrorAddress, LONG *TrnbLength )
{
	UCHAR  *CmdBuf;
	UCHAR  *ResBuf;

	/* 受信、送信データ先頭アドレス */
	CmdBuf = Hmsg->CmdBuf;
	ResBuf = Hmsg->ResBuf;

	/* 異常応答データ作成 */
	ResBuf[0]  =  CmdBuf[0];						/* 0:Slave Addrress		*/
	ResBuf[1]  = (CmdBuf[1] | 0x80);				/* 1:Function Code		*/
	ResBuf[2]  =  CmdBuf[2];						/* 2:Extended Address	*/
	ResBuf[3]  =  CmdBuf[3];						/* 3:Reserve1			*/
	ResBuf[4]  =  CmdBuf[4];						/* 4:Sub Function Code	*/
	ResBuf[5]  =  (UCHAR)ErrorCode;					/* 5:Error Code			*/
	ResBuf[6]  =  0x00;								/* 6:Reserve2			*/
	ResBuf[7]  =  0x00;								/* 7:Reserve3			*/
	mbusFuncSetArrayLongData( &ResBuf[8], ErrorAddress, sizeof(LONG) );	/* 8-11:ErrorAddress	*/

	/* エラー返信サイズ＝12byte	*/
	*TrnbLength = MBUS_HEAD_BASE_SIZE;

	/* レジスタ読み出し:MEMOBUSエラー情報(エラー発生アドレス) */
	Hmsg->ErrRno = ErrorAddress;
}

/****************************************************************************************************/
/*																									*/
/*		メッセージバッファ入れ替え関数(ULONG)														*/
/*																									*/
/****************************************************************************************************/
static LONG	mbusFuncLongSwap( void *Buf )
{
	ULONG data;
#ifdef __BIG_ENDIAN__	
	/* BigEndianのとき */
	data =  (ULONG)((*( UCHAR *)Buf	     << 24) 
		 	      + (*((UCHAR *)Buf + 1) << 16)
			      + (*((UCHAR *)Buf + 2) <<  8)
				  + (*((UCHAR *)Buf + 3)      ) );
#else					
	/* LittleEndianのとき */
	/* 未確認 */
	//↓は逆
	//data = (ULONG)((*( UCHAR *)Buf	         ) 
	//	         + (*((UCHAR *)Buf + 1) <<  8)
	//		     + (*((UCHAR *)Buf + 2) << 16)
	//		  	 + (*((UCHAR *)Buf + 3) << 24) );
	data = MlibCnvBig32( *((ULONG *)Buf) );
#endif
	return data;
}

/****************************************************************************************************/
/*																									*/
/*		メッセージバッファ入れ替え関数(USHORT)														*/
/*																									*/
/****************************************************************************************************/
static LONG	mbusFuncShortSwap( void *Buf )
{
	ULONG data;
#ifdef __BIG_ENDIAN__	
	/* BigEndianのとき */
	data = (ULONG)((*( UCHAR *)Buf      << 8) 
		         + (*((UCHAR *)Buf + 1)     ) );
#else					
	/* LittleEndianのとき */
	/* 未確認 */
/* <S001> */
//	data = (ULONG)((*( UCHAR *)Buf           ) 
//		 	     + (*((UCHAR *)Buf + 1) << 8 ));
	data = (ULONG)((*( UCHAR *)Buf      << 8) 
		         + (*((UCHAR *)Buf + 1)     ) );
/* <S001> */
#endif
	return data;
}

/****************************************************************************************************/
/*																									*/
/*		メッセージバッファ格納関数(ULONG)															*/
/*																									*/
/****************************************************************************************************/
static void	mbusFuncSetArrayLongData( UCHAR *Buf, ULONG data, UINT AccessSize )
{
#ifdef __BIG_ENDIAN__
/* BigEndianのとき */
	switch(AccessSize)
	{
		case 1:
			Buf[3] = (UCHAR)((data) >> 24);
			Buf[2] = (UCHAR)((data) >> 16);
			Buf[1] = (UCHAR)((data) >>  8);
			Buf[0] = (UCHAR)( data );
			break;
		case 2:
			Buf[2] = (UCHAR)((data) >> 24);
			Buf[3] = (UCHAR)((data) >> 16);
			Buf[0] = (UCHAR)((data) >>  8);
			Buf[1] = (UCHAR)( data );
			break;
		case 4:
			Buf[0] = (UCHAR)((data) >> 24);
			Buf[1] = (UCHAR)((data) >> 16);
			Buf[2] = (UCHAR)((data) >>  8);
			Buf[3] = (UCHAR)( data );
			break;
		default:
			break;
	}
#else
/* LittleEndianのとき */
/* <S001> */
	/* 未確認 */
	//↓逆
	//Buf[3] = (UCHAR)((data) >> 24);
	//Buf[2] = (UCHAR)((data) >> 16);
	//Buf[1] = (UCHAR)((data) >>  8);
	//Buf[0] = (UCHAR)( data );
//	*(ULONG *)Buf = MlibCnvBig32( data );

	switch(AccessSize)
	{
		case 1:
			Buf[3] = (UCHAR)((data) >> 24);
			Buf[2] = (UCHAR)((data) >> 16);
			Buf[1] = (UCHAR)((data) >>  8);
			Buf[0] = (UCHAR)( data );
			break;
		case 2:
			Buf[2] = (UCHAR)((data) >> 24);
			Buf[3] = (UCHAR)((data) >> 16);
			Buf[0] = (UCHAR)((data) >>  8);
			Buf[1] = (UCHAR)( data );
			break;
		case 4:
			Buf[0] = (UCHAR)((data) >> 24);
			Buf[1] = (UCHAR)((data) >> 16);
			Buf[2] = (UCHAR)((data) >>  8);
			Buf[3] = (UCHAR)( data );
			break;
		default:
			break;
	}
/* <S001> */
#endif
}

/****************************************************************************************************/
/*																									*/
/*		メッセージバッファ格納関数(USHORT)															*/
/*																									*/
/****************************************************************************************************/
static void	mbusFuncSetArrayShortData( UCHAR *Buf, USHORT data, UINT AccessSize)
{
#ifdef __BIG_ENDIAN__	
/* BigEndianのとき */
	switch(AccessSize)
	{
		case 1:
			Buf[1] = (UCHAR)((data) >> 8);
			Buf[0] = (UCHAR)( data );
			break;
		case 2:
			Buf[0] = (UCHAR)((data) >> 8);
			Buf[1] = (UCHAR)( data );
			break;
		default:
			break;
	}
#else					
/* LittleEndianのとき */
/* <S001> */
	/* ↓逆 */
	//Buf[1] = (UCHAR)((data) >> 8);
	//Buf[0] = (UCHAR)( data );
//	Buf[0] = (UCHAR)((data) >> 8);
//	Buf[1] = (UCHAR)( data );

	switch(AccessSize)
	{
		case 1:
			Buf[1] = (UCHAR)((data) >> 8);
			Buf[0] = (UCHAR)( data );
			break;
		case 2:
			Buf[0] = (UCHAR)((data) >> 8);
			Buf[1] = (UCHAR)( data );
			break;
		default:
			break;
	}
/* <S001> */
#endif
}

/****************************************************************************************************/
/*																									*/
/*		仮想メモリ→実メモリ変換関数																*/
/*		（アドレス範囲チェックし、軸番号、パラメータ番号を格納する）								*/
/*																									*/
/*			戻り値：REG_MANAGER_HANDLE															*/
/*																									*/
/****************************************************************************************************/
static REG_MANAGER_HANDLE *mbusFuncConvAddress(MEMOBUS_ADDR_MAP *AxisMap, ULONG *MemAddress )
{
	ULONG	mem_address, i;
	ULONG	AxisNumber;

	mem_address = *MemAddress;
	AxisNumber = (mem_address & 0x00F00000) >> 20;

	if(AxisNumber >= MAX_AXIS_NUMBER)
	{
		return NULL;
	}
	
	for(i = 0; i < MAX_AXIS_NUMBER; i++)
	{
		if(AxisMap[i].AxisAddress == AxisNumber)
		{
			return AxisMap[i].hRegMngr;
		}
	}

	return NULL;
}

/****************************************************************************************************/
/*																									*/
/*		Memobus：アドレス連続チェック（非連続　読み出し／書き込み共通）								*/
/*																									*/
/****************************************************************************************************/
static UCHAR mbusFuncCheckBlockAccess(UCHAR *CmdBuf, ULONG CurrentAddress, 
													UINT AccessSize, LONG BlockNum, LONG dIndex)
{
	LONG  j;
	LONG  cnt;
	ULONG NextAddress;

	cnt=0;
	for( j=0; j<(BlockNum-1); j++ )
	{
		/* アドレス更新	*/
		NextAddress = mbusFuncLongSwap( &CmdBuf[ dIndex*(j+1) ]);
		
		if( (NextAddress - CurrentAddress) == (AccessSize*(j+1)) )
		{/* アドレス指定が連続している場合 */
			cnt++;	
		}
	}
	if( cnt != (BlockNum-1) )
	{	/* アドレス指定が連続していない場合エラー */
		return MEMADRES_ERROR;
	}
	return 0;
}

/****************************************************************************************************/
/*																									*/
/*		メッセージデータ格納関数(Byte)																*/
/*																									*/
/****************************************************************************************************/
static LONG	mbusFuncSetValueByteData( UCHAR *Buf, LONG BlockNum, LONG dIndex )
{
	LONG Value,j;
	LONG BitShift;
	Value = 0;

	for( j=0; j<BlockNum; j++ )
	{
		BitShift = 8 * sizeof(CHAR) * j;
		Value += (LONG)(Buf[ dIndex*j ] << BitShift );
	}
	return Value;
}

/****************************************************************************************************/
/*																									*/
/*		メッセージデータ格納関数(SHORT)																*/
/*																									*/
/****************************************************************************************************/
static LONG	mbusFuncSetValueShortData( UCHAR *Buf, LONG BlockNum, LONG dIndex )
{
	LONG Value,j;
	LONG BitShift;
	Value = 0;

	for( j=0; j<BlockNum; j++ )
	{
		BitShift = 8 * sizeof(SHORT) * j;
		Value += (mbusFuncShortSwap( (void*)Buf ) << BitShift);
		Buf += dIndex;
	}
	return Value;
}

/****************************************************************************************************/
/*																									*/
/*		メッセージデータ格納関数(LONG)																*/
/*																									*/
/****************************************************************************************************/
static LONG mbusFuncSetValueLongData( UCHAR *Buf, LONG BlockNum, LONG dIndex )
{
	LONG Value,j;
	LONG BitShift;
	Value = 0;

	for( j=0; j<BlockNum; j++ )
	{
		BitShift = 8 * sizeof(LONG) * j;
		Value += (mbusFuncLongSwap( (void*)Buf ) << BitShift);
		Buf += dIndex;
	}
	return Value;
}
/* <<<<< <S150> */

/****************************************************************************************************/
/*																									*/
/*		<S166> Check and Get the Operation Write Handle												*/
/*																									*/
/****************************************************************************************************/
LONG mbusCheckOperationWriteHandle( REG_ACC_T *accessprm, MSGIF *Hmsg )
{
_enum_OPMODE modex;
ULONG  memaddr = accessprm->MemAddr & 0xFF0FFFFF;
PREGIF_CONFIG_T pRegCfg = accessprm->hRegCfg;
AXIS_HANDLE *pAxRsc = (AXIS_HANDLE *)pRegCfg->PrmCalParam;				/* 暫定 */
/*--------------------------------------------------------------------------------------------------*/
	switch ( memaddr )
	{
	/*----------------------------------------------------------------------------------------------*/
	case 0x80004000:						/* MODE1 */
	/*----------------------------------------------------------------------------------------------*/
		modex = OPMODE_OPE1;
		break;
	/*----------------------------------------------------------------------------------------------*/
	case 0x80004002:						/* MODE1 */
	/*----------------------------------------------------------------------------------------------*/
		modex = OPMODE_OPE1;
		break;
	/*----------------------------------------------------------------------------------------------*/
	case 0x80004008:						/* MODE2 */
	/*----------------------------------------------------------------------------------------------*/
		modex = OPMODE_OPE2;
		break;
	/*----------------------------------------------------------------------------------------------*/
	case 0x8000400A:						/* MODE2 */
	/*----------------------------------------------------------------------------------------------*/
		modex = OPMODE_OPE2;
		break;
	/*----------------------------------------------------------------------------------------------*/
	case 0x8000400C:						/* TRACE */
	/*----------------------------------------------------------------------------------------------*/
		modex = OPMODE_TRACE;
		break;
	/*----------------------------------------------------------------------------------------------*/
	case 0x8000400E:						/* TRACE */
	/*----------------------------------------------------------------------------------------------*/
		modex = OPMODE_TRACE;
		break;
	/*----------------------------------------------------------------------------------------------*/
	default:
	/*----------------------------------------------------------------------------------------------*/
		modex = OPMODE_NULL;
		break;
	}
/*--------------------------------------------------------------------------------------------------*/
	if ( modex != OPMODE_NULL )
	{
		if ( mbusGetOpeRegWriteHandle( pAxRsc->pAccessOp, modex, Hmsg ) != TRUE )
		{
			return (NG);					/* 他ch競合 */
		}
	}
/*--------------------------------------------------------------------------------------------------*/
	return (OK);
}

/****************************************************************************************************/
/*																									*/
/*		<S166> Free Operation Write Handle															*/
/*																									*/
/****************************************************************************************************/
void mbusFreeOperationWriteHandle( REG_ACC_T *accessprm, MSGIF *Hmsg )
{
_enum_OPMODE modex;
FUN_EXECH ch;
ULONG  memaddr = accessprm->MemAddr & 0xFF0FFFFF;
PREGIF_CONFIG_T pRegCfg = accessprm->hRegCfg;
AXIS_HANDLE *pAxRsc = (AXIS_HANDLE *)pRegCfg->PrmCalParam;				/* 暫定 */
FUN_MANAGER 	*FunMngr = pAxRsc->FnManager;							/* 暫定 */
/*--------------------------------------------------------------------------------------------------*/
	switch ( memaddr )
	{
	/*----------------------------------------------------------------------------------------------*/
	case 0x80004000:						/* MODE1 */
	/*----------------------------------------------------------------------------------------------*/
		ch = FUN_CH1;
		modex = OPMODE_OPE1;
		break;
	/*----------------------------------------------------------------------------------------------*/
	case 0x80004002:						/* MODE1 */
	/*----------------------------------------------------------------------------------------------*/
		ch = FUN_CH1;
		modex = OPMODE_OPE1;
		break;
	/*----------------------------------------------------------------------------------------------*/
	case 0x80004008:						/* MODE2 */
	/*----------------------------------------------------------------------------------------------*/
		ch = FUN_CH2;
		modex = OPMODE_OPE2;
		break;
	/*----------------------------------------------------------------------------------------------*/
	case 0x8000400A:						/* MODE2 */
	/*----------------------------------------------------------------------------------------------*/
		ch = FUN_CH2;
		modex = OPMODE_OPE2;
		break;
	/*----------------------------------------------------------------------------------------------*/
	case 0x8000400C:						/* TRACE */
	/*----------------------------------------------------------------------------------------------*/
		ch = FUN_CH1;		/* @@ */
		modex = OPMODE_TRACE;
		break;
	/*----------------------------------------------------------------------------------------------*/
	case 0x8000400E:						/* TRACE */
	/*----------------------------------------------------------------------------------------------*/
		ch = FUN_CH1;		/* @@ */
		modex = OPMODE_TRACE;
		break;
	/*----------------------------------------------------------------------------------------------*/
	default:
	/*----------------------------------------------------------------------------------------------*/
		ch = FUN_CH1;		/* @@ */
		modex = OPMODE_NULL;
		break;
	}
/*--------------------------------------------------------------------------------------------------*/
/*	Free Handle																						*/
/*--------------------------------------------------------------------------------------------------*/
	if ( ( modex == OPMODE_OPE1 ) || ( modex == OPMODE_OPE2 ) )
	{
		if ( 0x000 == FunGetOpeModeRegister( FunMngr, ch ) )
		{
			mbusFreeOpeRegWriteHandle( pAxRsc->pAccessOp, modex, Hmsg );
		}
	}
	else if ( modex == OPMODE_TRACE )
	{
		if( 0x000 == FunMngr->AxCommon->TraceMode )
		{
			mbusFreeOpeRegWriteHandle( pAxRsc->pAccessOp, modex, Hmsg );
		}
	}
/*--------------------------------------------------------------------------------------------------*/
	return;
}

/****************************************************************************************************/
/*																									*/
/*		<S166> 操作レジスタ書込み権取得																*/
/*																									*/
/****************************************************************************************************/
LONG	mbusGetOpeRegWriteHandle( OPACCSV *Accs, _enum_OPMODE mode, MSGIF *Hmsg )
{
/*--------------------------------------------------------------------------------------------------*/
/*		ハンドルチェック																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( Accs->Hmsg[mode] == NULL )
		{
			Accs->Hmsg[mode] = Hmsg;					/* ハンドルが未使用であれば獲得				*/
			return( TRUE );								/* ハンドル獲得成功							*/
		}
		else if( Accs->Hmsg[mode] == Hmsg )
		{
			return( TRUE );								/* ハンドルが同じであればＯＫ				*/
		}
/*--------------------------------------------------------------------------------------------------*/
		return( FALSE );								/* ハンドル獲得失敗							*/

}

/****************************************************************************************************/
/*																									*/
/*		操作レジスタ書込み権解放処理																*/
/*																									*/
/****************************************************************************************************/
void	mbusFreeOpeRegWriteHandle( OPACCSV *Accs, _enum_OPMODE mode, MSGIF *Hmsg )
{
/*--------------------------------------------------------------------------------------------------*/
/*		Free Operator Write Handle (Called from FunMngr)											*/
/*--------------------------------------------------------------------------------------------------*/
		if( Accs->Hmsg[mode] == Hmsg )
		{
			Accs->Hmsg[mode]   = NULL;					/* Free Register I/F Write Handle			*/
		}
/*--------------------------------------------------------------------------------------------------*/
		return;

}

/* <S205> Start */
/****************************************************************************************************/
/*																									*/
/*		Memobus：製品情報読み出し																	*/
/*																									*/
/****************************************************************************************************/
static void mbusFuncReadProductInfo42H( HMSGIF Hmsg, LONG SubFuncCode, LONG *TrnbLength )
{
	UCHAR	BlockSize;
	UCHAR	ReadBlockSize;
	UCHAR	ReadSlotNo;
	UCHAR	ReadAxisNo;
	UCHAR	ReadIdNo;

	LONG	CmdBuffOffset;
	LONG	NextCmdBuffOffset;
	LONG	RspBuffOffset;
	LONG	RspBuffSize;
	LONG	ReadBlockStartOffset;


	LONG	ReadIdRegAdd;
	UCHAR	ReadDataSize;
	UCHAR	ReadDataNum;

	LONG	i;
	LONG	j;
	
	LONG	ret;
	LONG	CurrentDataLength;
	LONG	BlockDataSize;


	/* 受信、送信データ先頭アドレス */
	UCHAR  *CmdBuf;	
	UCHAR  *ResBuf;

	CmdBuf = Hmsg->CmdBuf;
	ResBuf = Hmsg->ResBuf;

	/* 変数初期化 */
	ret = 0;
	NextCmdBuffOffset = 0;							/* 次回ブロック数格納バッファオフセット */
	BlockDataSize = 0;							/* 各ブロクに格納されるデータサイズ(スロット番号 ～ 最終データ) */
	RspBuffOffset = MBUS_IDRD_RSP_HEAD_BASE_SIZE;	/* RspBuffデータ部オフセット */
	RspBuffSize = MBUS_IDRD_RSP_HEAD_BASE_SIZE;		/* RspBuffデータサイズ */
	BlockSize = CmdBuf[MBUS_IDRD_HEAD_BASE_SIZE];	/* 読み出しブロック数取得 */

	/* ブロック数チェック  */
	if( BlockSize == 0) 
	{	/* 設定値範囲外エラー */
		mbusFuncErrorManager42H( Hmsg, RANGOUTS_ERROR, 0, TrnbLength );
		return;
	}

	for ( i = 0; i < BlockSize; i++ )
	{
		CmdBuffOffset = MBUS_IDRD_HEAD_BASE_SIZE + 1 + NextCmdBuffOffset;	/* 読み出すブロックサイズが格納されているCmdBuffのオフセット算出 */
		ReadBlockSize =  CmdBuf[CmdBuffOffset];								/* 読み出すブロックサイズ(読出すIDの数)の取得 */

		/* ブロックサイズチェック */
		if( (ReadBlockSize < 4) || (ReadBlockSize > MBUS_IDRD_ID_NUM + 3) )
		{
			/* 個数不良エラー */
			mbusFuncErrorManager42H( Hmsg, DATANUMB_ERROR, 0, TrnbLength );
			return;
		}
		else
		{
			ReadSlotNo = CmdBuf[CmdBuffOffset + 1];			/* 「スロット番号」取得 */
			/* スロット番号チェック  */
			if( ReadSlotNo > (MBUS_IDRD_SLOT_NUM - 1) )
			{
				/* 設定値範囲外エラー */
				mbusFuncErrorManager42H( Hmsg, RANGOUTS_ERROR, 0, TrnbLength );
				return;
			}

			ReadAxisNo = CmdBuf[CmdBuffOffset + 2];			/* 「軸番号」取得 */
			ReadBlockStartOffset = RspBuffOffset;			/* ブロック読出し開始オフセット */
			ResBuf[RspBuffOffset + 1] = ReadSlotNo;			/* 「スロット番号」をレスポンスバッファに格納 */
			ResBuf[RspBuffOffset + 2] = ReadAxisNo;			/* 「軸番号」をレスポンスバッファに格納 */

			RspBuffSize = RspBuffSize + 3;					/* 受信データサイズ更新 */
			RspBuffOffset = RspBuffOffset + 3;				/* 応答バッファオフセット更新 */

			/* 応答バッファサイズチェック */
			if( RspBuffSize > MBUS_MAXSIZE )
			{	/* 個数不良エラー */
				mbusFuncErrorManager42H( Hmsg, DATANUMB_ERROR, 0, TrnbLength );
				return;
			}

			/* 読み出し製品情報取得 */
			for ( j = 0; j < (ReadBlockSize - 3 ) ; j++ )		/* ReadBlockSize : ブロックサイズ ～ 最終のID */
			{
				ReadIdNo =  CmdBuf[CmdBuffOffset + (3 + j)];	/* 読み出すID */;

				/* ID-No.チェック */
				if( ReadIdNo > (MBUS_IDRD_ID_NUM - 1) )
				{
					/* 設定値範囲外エラー */
					mbusFuncErrorManager42H( Hmsg, RANGOUTS_ERROR, 0, TrnbLength );
					return;
				}

				ReadIdRegAdd = IdRegAddTbl[ReadSlotNo][ReadIdNo];		/* 読み出すIDのレジスタアドレス取得 */
				ReadDataSize = IdDataSizeTbl[ReadSlotNo][ReadIdNo];		/* 読み出すIDのデータサイズ取得 */
				ReadDataNum = IdDataNumTbl[ReadSlotNo][ReadIdNo];		/* 読み出すIDのデータ数取得 */

				if( (ReadIdRegAdd == 0) && (ReadDataSize == 0) && (ReadDataNum == 0) )	/* 読み出しIDデータなし */
				{
					CurrentDataLength = 1;								/* データ長 = 2[byte]とする */
					ResBuf[RspBuffOffset] = ReadIdNo;					/* 「ID」をレスポンスバッファに格納 */
					ResBuf[RspBuffOffset + 1] = CurrentDataLength;		/* 「読み出すIDのデータサイズ」をレスポンスバッファに格納 */
					ResBuf[RspBuffOffset + 2] = 0x00;					/* 「読み出すIDのデータ」 = 0x00を設定 */
					RspBuffOffset = RspBuffOffset + 3;					/* 応答バッフ(データ部)ァオフセット更新 */
					RspBuffSize = RspBuffSize + 3;						/* 受信データサイズ更新 */
				}
				else	/* 読み出しIDデータあり */
				{
					CurrentDataLength = ReadDataSize * ReadDataNum;			/* 受信データ長[byte]算出 */
					ResBuf[RspBuffOffset] = ReadIdNo;						/* 「ID」をレスポンスバッファに格納 */
					ResBuf[RspBuffOffset + 1] = CurrentDataLength;			/* 「読み出すIDのデータサイズ」をレスポンスバッファに格納 */
					RspBuffOffset = RspBuffOffset + 2;						/* 応答バッファオフセット更新 */
					RspBuffSize = RspBuffSize + CurrentDataLength + 2;		/* 受信データサイズ更新 */

					/* 応答バッファサイズチェック */
					if( (RspBuffSize > MBUS_MAXSIZE) || (ReadDataNum < MBUS_MIN_REGNUM))
					{	/* 個数不良エラー */
						mbusFuncErrorManager42H( Hmsg, DATANUMB_ERROR, 0, TrnbLength );
						return;
					}

					if( ReadIdNo == 0x02 )	/* ID2(仕様)読み出しの場合 */
					{
						mbusFuncSetSpecialSpecIdData( Hmsg, RspBuffOffset);		/* 仕様取得 */
						RspBuffOffset = RspBuffOffset + CurrentDataLength;		/* 応答バッファ(データ部)オフセット更新 */
					}
					else					/* ID2(仕様)読み出し以外の場合 */
					{
						ret = mbusFuncSetIdData( Hmsg, ReadIdRegAdd, ReadDataSize, ReadDataNum, RspBuffOffset, TrnbLength);	/* 製品情報取得(ID2以外) */
						if(ret !=0 )
						{	/* 製品情報取得失敗 */
							/* エラー(エラー種別はretの内容による) */
							mbusFuncErrorManager42H( Hmsg, ret, ReadIdRegAdd, TrnbLength );
							return;
						}
						else
						{	/* 製品情報取得成功 */
							RspBuffOffset = RspBuffOffset + CurrentDataLength;	/* 応答バッファ(データ部)オフセット更新 */

						}
					}
				}

				BlockDataSize += CurrentDataLength + 2;	/* 読み出したブロックサイズ[byte] : ID-No. + データサイズ + データ数 */
			}
			/* レスポンスのブロックサイズ = ブロックサイズ + スロット番号 + 軸番号 + 各読み出したデータ(ID-No. + 読出しデータサイズ + 読み出したデータ) */
			ResBuf[ReadBlockStartOffset] = 3 + BlockDataSize;			/* ブロックサイズ + 「ブロックサイズ」をレスポンスバッファに格納 */
			NextCmdBuffOffset = NextCmdBuffOffset + ReadBlockSize;		/* 次回ブロック数格納バッファオフセット更新(ブロックサイズをどうするかで上の処理と帰る) */

			BlockDataSize = 0;											/* 読み出したブロックサイズ[byte] 0クリア */
		}
	}

	MlibCopyByteMemory( ResBuf, CmdBuf, MBUS_IDRD_HEAD_BASE_SIZE+1);	/* 応答データ作成 */
	*TrnbLength = RspBuffSize;											/* 送信バッファサイズ決定 */

}

/****************************************************************************************************/
/*																									*/
/*		製品ID格納関数																				*/
/*			ID0:製品型式																			*/
/*			ID1:機種ID																				*/
/*			ID3:シリアルNo.																			*/
/*			ID4:Soft Ver.																			*/
/*			ID5:レジスタ Ver.																		*/
/*			ID6:BTO番号																				*/
/*																									*/
/****************************************************************************************************/
static LONG mbusFuncSetIdData( HMSGIF Hmsg, LONG ReadIdRegAdd, UCHAR ReadDataSize, UCHAR ReadDataNum, LONG RspBuffOffset, LONG *TrnbLength )
{
	UCHAR	DataType;
	UCHAR	dIndex;
	USHORT	DataNum;
	UINT	AccessSize,RegSize;
	REG_ACC_T AccessPrm;
	LONG	i;
	LONG	ret;
	LONG	Value;
	LONG	RspLength;
	LONG	BlockNum;
	ULONG	BeginAddress;
	ULONG	CurrentAddress;
	REG_MANAGER_HANDLE	*hRegMngr;

	/* 受信、送信データ先頭アドレス */
	UCHAR  *CmdBuf;	
	UCHAR  *ResBuf;

	CmdBuf = Hmsg->CmdBuf;
	ResBuf = Hmsg->ResBuf;

		ret = 0;

	for ( i = 0; i < ReadDataNum; i++ )
	{
			CurrentAddress = ReadIdRegAdd + (2*i);

		/* 仮想メモリ　→　実アドレス変換 */
		AccessPrm.MemAddr = CurrentAddress;
		hRegMngr = mbusFuncConvAddress(Hmsg->AxisMap,&AccessPrm.MemAddr);		
		if( hRegMngr == NULL )
		{	/* メモリアドレスエラー */
			mbusFuncErrorManager42H( Hmsg, SELAXIS_ERROR, CurrentAddress, TrnbLength );
			ret = SELAXIS_ERROR;
			return ret;
		}

		/* パラメータ属性取得 */
		if( NULL == RegMngrGetAttrByMemAddr(hRegMngr, &AccessPrm) )
		{	/* メモリアドレスエラー	*/
			mbusFuncErrorManager42H( Hmsg, MEMADRES_ERROR, CurrentAddress, TrnbLength );
			ret = MEMADRES_ERROR;
			return ret;
		}
		/* 読み出しパラメータサイズ */
		RegSize = AccessPrm.cPrmDef->DataLength;

		/*------------------------------------------------------------------------------------------*/
		/*	ブロック読み出し																		*/
		/*------------------------------------------------------------------------------------------*/
		BlockNum = RegSize/ReadDataSize;		/* ブロック読み出し回数 */

		if( (BlockNum > 0) && ( ReadDataNum > (i + BlockNum - 1)) )
		{
			i = i + (BlockNum-1);				/* ブロック読み出し回数分iを加算する	*/
		}
		else
		{	
			/* 個数不良 */
			mbusFuncErrorManager42H( Hmsg, DATANUMB_ERROR, CurrentAddress, TrnbLength );
			ret = DATANUMB_ERROR;
			return ret;
		}

		/* データ読み出し */
		AccessPrm.IfHandle = Hmsg;
		ret = (LONG)PrmReadValue( &AccessPrm, &Value, RegSize);
		if( PRM_RSLT_SUCCESS != ret )
		{
			mbusFuncErrorManager42H( Hmsg, ret, CurrentAddress, TrnbLength );
			return ret;
		}

		/* 送信データセット */
		switch (RegSize)
		{
			case 1:		/* byte型 */
				ResBuf[RspBuffOffset + (i*RegSize)] = (UCHAR)(Value);
				break;
			case 2:		/* short型 */
				mbusFuncSetArrayShortData( &ResBuf[RspBuffOffset + (i*RegSize)], (USHORT)Value, ReadDataSize );
				break;
			case 4:		/* long型 */
				mbusFuncSetArrayLongData( &ResBuf[RspBuffOffset + (i*RegSize)], (ULONG)Value, ReadDataSize );
				break;
		}
		
	}

	
	return ret;
}

/****************************************************************************************************/
/*																									*/
/*		製品ID2:仕様格納関数																		*/
/*																									*/
/****************************************************************************************************/
static void mbusFuncSetSpecialSpecIdData( HMSGIF Hmsg, LONG RspBuffOffset)
{

	CHAR ServoSpecialSpec[4];

	/* 受信、送信データ先頭アドレス */
	UCHAR  *ResBuf;
	ResBuf = Hmsg->ResBuf;	

	/* 特殊仕様番号(SERVO_SPECIAL_SPEC_VER)取得 */
	ServoSpecialSpec[0] = (CHAR)(0x000f & (SERVO_SPECIAL_SPEC_VER >> 12) );
	ServoSpecialSpec[1] = (CHAR)(0x000f & (SERVO_SPECIAL_SPEC_VER >> 8) );
	ServoSpecialSpec[2] = (CHAR)(0x000f & (SERVO_SPECIAL_SPEC_VER >> 4) );
	ServoSpecialSpec[3] = (CHAR)(0x000f & (SERVO_SPECIAL_SPEC_VER) );

	/* 特殊仕様種別を応答バッファに格納 */
	if( ServoSpecialSpec[3] == 0x00 )	/* 標準仕様の場合 */
	{
		/* 0x00 = 標準仕様：'0' */
		ResBuf[RspBuffOffset] = NumAsciiTbl[ServoSpecialSpec[3]];

	}
	else	/* 特殊仕様の場合 */
	{
		/* 0x01 = FT仕様：'F' */
		/* 0x02 = EX仕様：'E' */
		/* 0x03 = Y仕様 ：'Y' */
		ResBuf[RspBuffOffset] = StrAsciiTbl[(ServoSpecialSpec[3] -  1)];

	}

	/* 特殊仕様番号(ASCIIコード)を応答バッファに格納 */
	ResBuf[RspBuffOffset + 1] = NumAsciiTbl[ServoSpecialSpec[2]];
	ResBuf[RspBuffOffset + 2] = NumAsciiTbl[ServoSpecialSpec[1]];
	ResBuf[RspBuffOffset + 3] = NumAsciiTbl[ServoSpecialSpec[0]];

	return;
}
/* <S205> End */
/***************************************** end of file **********************************************/
