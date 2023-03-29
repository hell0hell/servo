/****************************************************************************************************/
/*																									*/
/*																									*/
/*		MemobusFunctions.c : MEMOBUSメッセージ通信管理												*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : MEMOBUSメッセージ通信ＩＦ処理を行う。													*/
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
/*	Note	:	初版	2010.09.06	T.Kira	For INGRAM												*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "KnlApi.h"
#include "MemobusIF.h"
#include "MemobusFunctions.h"
//#include "WaitFunc.h"
#include "PrmAccessIf.h"
//#include "MLib.h"

/* <S205> Delete Start */
/* <S150>:『メッセージ通信処理共通化』に伴い、「SigmaMemobus.c」へ移動 */
#if 0
/* <S061>	Start */
#ifdef _INFO_ID_READ_CHECK_
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
/* 00:サーボパック */	{	2,			2,			4,			2,				2,				4,					2			},
/* 01:CMD */			{	2,			0,			0,			2,				2,				0,					0			},
/* 02:Safty */			{	2,			0,			0,			2,				2,				0,					0			},
/* 03:FB */				{	2,			0,			0,			2,				0,				0,					0			},
/* 04:モータ */			{	2,			0,			0,			2,				0,				0,					0			},
/* 05:エンコーダ */		{	2,			0,			0,			2,				2,				0,					0			}
};

/* 読み出しIDのデータ数Table */
const CHAR IdDataNumTbl[MBUS_IDRD_SLOT_NUM][MBUS_IDRD_ID_NUM]=
{
						/* 	ID0(型式)	ID1(仕様)	ID2(仕様№)	ID3(シリアル№)	ID4(Soft Ver)	ID5(レジスタ Ver)	ID6(BTO №)	 */
/* 00:サーボパック */	{	8,			1,			1,			8,				2,				1,					32			},
/* 01:CMD */			{	8,			0,			0,			8,				2,				0,					0			},
/* 02:Safty */			{	12,			0,			0,			12,				2,				0,					0			},
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
#endif
/* <S061>	End */
#endif
/* <S205> Delete End */


/****************************************************************************************************/
/*																									*/
/*		PROTOTYPE																					*/
/*																									*/
/****************************************************************************************************/
#if 0	/* <S150> */
static void	  mbusFuncMsgManager( MEMOBUS_IF *MbusIfPtr );
static UCHAR *mbusFuncGetRcvAdr(MEMOBUS_IF *MbusIfPtr);
static UCHAR *mbusFuncGetTrnsAdr(MEMOBUS_IF *MbusIfPtr);
#ifdef __MYAD_CHECK__
static ULONG  mbusFuncChkMyMsg( MEMOBUS_IF *MbusIfPtr, UCHAR *CmdBuf );
#endif

static void	mbusLoopbackTest( MEMOBUS_IF *MbusIfPtr );

static void	mbusFuncReadMemory42H( MEMOBUS_IF *MbusIfPtr, LONG SubFuncCode );
static void	mbusFuncWriteMemory42H( MEMOBUS_IF *MbusIfPtr, LONG SubFuncCode );
static void	mbusFuncReadMaxSize42H( MEMOBUS_IF *MbusIfPtr, LONG SubFuncCode );
static void mbusFuncErrorManager42H( MEMOBUS_IF *MbusIfPtr, LONG ErrCode, ULONG ErrAddress );

//
static LONG mbusFuncShortSwap( void *Buf );
static LONG mbusFuncLongSwap( void *Buf );

//
static void	mbusFuncSetArrayLongData( UCHAR *Buf, ULONG data, UINT AccessSize );
static void	mbusFuncSetArrayShortData( UCHAR *Buf, USHORT data, UINT AccessSize );

//
static LONG mbusFuncSetValueByteData( UCHAR *Buf, LONG BlockNum, LONG dIndex );
static LONG mbusFuncSetValueShortData( UCHAR *Buf, LONG BlockNum, LONG dIndex );
static LONG mbusFuncSetValueLongData( UCHAR *Buf, LONG BlockNum, LONG dIndex );

static REG_MANAGER_HANDLE *mbusFuncConvAddress(MEMOBUS_ADDR_MAP *AxisMap, ULONG *MemAddress );
static UCHAR mbusFuncCheckBlockAccess(UCHAR *CmdBuf, ULONG CurrentAddress, 
													UINT AccessSize, LONG BlockNum, LONG dIndex);

#ifdef _INFO_ID_READ_CHECK_
static void mbusFuncReadProductInfo42H( MEMOBUS_IF *MbusIfPtr, LONG SubFuncCode );		/* <S061> */
static void mbusFuncSetSpecialSpecIdData( MEMOBUS_IF *MbusIfPtr, LONG RspBuffOffset);	/* <S061> */
static LONG mbusFuncSetIdData( MEMOBUS_IF *MbusIfPtr, LONG ReadIdRegAdd, UCHAR ReadDataSize, UCHAR ReadDataNum, LONG RspBuffOffset);		/* <S061> */
#endif
#endif	/*<S150>*/

/****************************************************************************************************/
/*																									*/
/*		Memobus IF Initialize																		*/
/*																									*/
/****************************************************************************************************/
#if 0	/* <S150> */
void	MbusFuncInitialize(MEMOBUS_IF *MbusIfPtr, MEMOBUS_ADDR_MAP *AxisCfg, LONG node_address )
{	
	INT i;

	for(i = 0; i < MAX_AXIS_NUMBER; i++)
	{
		MbusIfPtr->AxisMap[i] = AxisCfg[i];
	}

	/* ノードアドレス	*/
	MbusIfPtr->NodeAddress = node_address;
}
#endif	/* <S150> */

#if 0	/* <S150> */
/****************************************************************************************************/
/*																									*/
/*		メッセージ解析＆実行処理																	*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		戻り値：送信バッファ長																		*/
/*				(0のときは送信アボートして無応答とする)												*/
/*																									*/
/****************************************************************************************************/
USHORT	MbusFuncMsgProcedure( MEMOBUS_IF *MbusIfPtr )
{
	LONG	Offset;
	LONG	RouteNum;
	LONG	Length;
	UCHAR  *RxBuf;
	UCHAR  *TxBuf;

	MbusIfPtr->RelayHdSize = 0;	/* 中継コマンドヘッダサイズ初期化	*/
	MbusIfPtr->TrnbLength  = 0;	/* 送信バッファサイズ初期化			*/

	/* 受信、送信データ先頭アドレス */
	RxBuf = &MbusIfPtr->RxBuf[MBUS_HDROFS];
	TxBuf = &MbusIfPtr->TxBuf[MBUS_HDROFS];

	/* 受信、送信バッファポインタ取得	*/
	MbusIfPtr->CmdBuf = mbusFuncGetRcvAdr(  MbusIfPtr );
	MbusIfPtr->ResBuf = mbusFuncGetTrnsAdr( MbusIfPtr );

	/*----------------------------------------------------------------------------------------------*/
	/* MEMOBUSコマンドの解析																		*/
	/*----------------------------------------------------------------------------------------------*/
	switch( RxBuf[1] )
	{	
		case MBUS_LBAK_08H:
			/*--------------------------------------------------------------------------------------*/
			/*		ループバックコマンド																	*/
			/*--------------------------------------------------------------------------------------*/
			mbusLoopbackTest( MbusIfPtr );
			break;

		case MBUS_FC_42H:
			/*--------------------------------------------------------------------------------------*/
			/*		MEMOBUSファンクションコード														*/
			/*--------------------------------------------------------------------------------------*/
			/* 受信データ長(バイト単位)チェック */
			if(( MbusIfPtr->RcvbLength < MBUS_MINSIZE ) || ( MbusIfPtr->RcvbLength > MBUS_MAXSIZE ))
			{
				return(0);
			}
#ifdef __MYAD_CHECK__
			/* 自局アドレスチェック	*/
			else if(FALSE == mbusFuncChkMyMsg( MbusIfPtr, RxBuf ))
			{
				return(0);	
			}
#endif	
#ifdef __EXAD_CHECK__
			/* 拡張アドレスチェック */
			else if(MbusIfPtr->CmdBuf[2] != 0xFF)
			{
				mbusFuncErrorManager42H( MbusIfPtr, SELAXIS_ERROR, 0);
			}
#endif	
			else
			{
				/* メッセージ処理実行 */
				mbusFuncMsgManager( MbusIfPtr );
			}
			break;	
		
#if 1	//2013.09.04
		case MBUS_SYSDL_50H:
			/*--------------------------------------------------------------------------------------*/
			/*		MEMOBUSファームダウンロードコード														*/
			/*--------------------------------------------------------------------------------------*/
			/* 受信データ長(バイト単位)チェック */
			if(( MbusIfPtr->RcvbLength < MBUS_MINSIZE ) || ( MbusIfPtr->RcvbLength > MBUS_MAXSIZE ))
			{
				return(0);
			}
			else
			{
				/* ファームダウンロード処理実行 */
				MbusFuncSystemDownLoad( MbusIfPtr );
			}
			break;
#endif

		case MBUS_RELAY_51H:		
			/*--------------------------------------------------------------------------------------*/
			/*		中継メッセージファンクション													*/
			/*--------------------------------------------------------------------------------------*/
			/* 中継段数	*/
			RouteNum = (RxBuf[5] & 0x0F);				

			/* 中継コマンド長 */
			MbusIfPtr->RelayHdSize = (USHORT)(RouteNum * MBUS_RLY_PASS_DATA_SIZE + MBUS_RLY_FIX_SIZE);

			/* 受信・送信バッファポインタ取得	*/
			MbusIfPtr->CmdBuf = mbusFuncGetRcvAdr( MbusIfPtr );
			MbusIfPtr->ResBuf = mbusFuncGetTrnsAdr( MbusIfPtr );
			
			/* ヘッダのオフセット長を算出 */
			Offset = MbusIfPtr->RelayHdSize + MBUS_HDROFS;					
			
			/* データ部コピー */
			MlibCopyByteMemory( TxBuf, RxBuf, MbusIfPtr->RelayHdSize);

			TxBuf[4] |= 0x80;					/* 応答フラグ						*/
			TxBuf[5]  = (UCHAR)RouteNum;		/* 通過段数クリア&中継段数セット	*/

			/* 中継段数オーバー or 中継段数と通過段数が異なる場合はエラー */
			if( (RouteNum > MBUS_RLY_MAX_ROUTENUM) || (RouteNum != (RxBuf[5] >> 4)) )
			{/* 中継段数エラー */
#if 0
				MbusIfPtr->TxBuf[Offset - 6] |= 0x80;
				MbusIfPtr->TxBuf[Offset + 0] = 0x00;
				MbusIfPtr->TxBuf[Offset + 1] = MsgPtr->RxBuf[MBUS_HDROFS + 5];
				MbusIfPtr->TxBuf[Offset + 2] = 0x00;
				MbusIfPtr->TxBuf[Offset + 3] = 0x03;
				//MbusIfPtr->TrnwLength = Offset + 2;		/* エラー返信サイズ＝2Word				*/
				MbusIfPtr->TrnbLength = Offset + 4;			/* エラー返信サイズ＝4byte				*/
				//MbusIfPtr->MsgAnaSts = M3MSG_APLLAYER_ERR;
#endif
				/* 受信メッセージエラー */
				return(0);		
			}
			else
			{/* 中継段数正常 */

				/* 受信データサイズ(バイト単位)を格納 */
				MbusIfPtr->RcvbLength =(USHORT)(((ULONG)MbusIfPtr->RxBuf[Offset-4] << 24) 
											 | ( (ULONG)MbusIfPtr->RxBuf[Offset-3] << 16) 
											 | ( (ULONG)MbusIfPtr->RxBuf[Offset-2] <<  8) 
											 |   (ULONG)MbusIfPtr->RxBuf[Offset-1] <<  0);

				/* 受信データ長(バイト単位)チェック */
				if(( MbusIfPtr->RcvbLength < MBUS_MINSIZE ) || ( MbusIfPtr->RcvbLength > MBUS_MAXSIZE ))
				{
					/* 受信メッセージエラー */
					return(0);	
				}
#ifdef __MYAD_CHECK__
				/* 自局アドレスチェック	*/
				else if( (FALSE == mbusFuncChkMyMsg( MbusIfPtr, RxBuf )) 
					  || (FALSE == mbusFuncChkMyMsg( MbusIfPtr, MbusIfPtr->CmdBuf )))
				{
					return(0);	
				}
#endif
				/* ファンクションコードチェック */
				else if( MbusIfPtr->CmdBuf[1] != MBUS_FC_42H )
				{
					/* ファンクションコードエラー */
					mbusFuncErrorManager42H( MbusIfPtr, FUNCCODE_ERROR, 0);	
				}
#ifdef __EXAD_CHECK__
				/* 拡張アドレスチェック */
				else if(MbusIfPtr->CmdBuf[2] != 0xFF)
				{
					mbusFuncErrorManager42H( MbusIfPtr, SELAXIS_ERROR, 0);
				}
#endif
				else
				{
					/* メッセージ処理実行 */
					mbusFuncMsgManager( MbusIfPtr );			
				}

				/* 送信データサイズに中継コマンド分のサイズを加算 */
				MbusIfPtr->TrnbLength = MbusIfPtr->TrnbLength + MBUS_HDROFS + MbusIfPtr->RelayHdSize;

				/* 中継コマンドサイズを算出 */
				Length = MbusIfPtr->TrnbLength - Offset;

				/* 送信データサイズ(バイト単位)を設定 */
				mbusFuncSetArrayLongData( &MbusIfPtr->TxBuf[ Offset - 4 ], (ULONG)Length, sizeof(LONG) );
			}
			break;

		default:
			/*--------------------------------------------------------------------------------------*/
			/*		ファンクションコード　サポート外												*/
			/*--------------------------------------------------------------------------------------*/
			/* 自局アドレスチェック	*/
#ifdef __MYAD_CHECK__
			if(FALSE == mbusFuncChkMyMsg( MbusIfPtr, RxBuf ))
			{
				/* 自局宛てでない→アボート		*/
				return(MbusIfPtr->TrnbLength);	
			}
			else
			{
				/* 自局宛て→ファンクションエラー	*/
				mbusFuncErrorManager42H( MbusIfPtr, FUNCCODE_ERROR, 0);
			}
#else
			/* 自局宛て→ファンクションエラー	*/
			mbusFuncErrorManager42H( MbusIfPtr, FUNCCODE_ERROR, 0);
#endif
			break;
	}

	/*----------------------------------------------------------------------------------------------*/
	/*	奇数バイトの送信の場合最後の１個はごみなので0クリアしておく									*/
	/*----------------------------------------------------------------------------------------------*/
	if( MbusIfPtr->TrnbLength & 1 )
	{
		/* 0クリア	*/
		MbusIfPtr->TxBuf[ MbusIfPtr->TrnbLength ] = 0;
	}

	return( MbusIfPtr->TrnbLength );
}

#ifdef __MYAD_CHECK__
/****************************************************************************************************/
/*																									*/
/*		自局宛メッセージのチェック																	*/
/*																									*/
/****************************************************************************************************/
ULONG	mbusFuncChkMyMsg( MEMOBUS_IF *MbusIfPtr,UCHAR *CmdBuf )		
{

	/*----------------------------------------------------------------------------------------------*/
	/*		今回受信したメッセージのアドレス=="0" or 自局アドレスの場合TRUEを返す					*/
	/*----------------------------------------------------------------------------------------------*/
	if(( CmdBuf[0] == 0x00 )
	 ||( CmdBuf[0] == MbusIfPtr->NodeAddress ))
	{
		/* 自局宛 */
		return( TRUE );	
	}
	else
	{
		/* 自局宛ではない */
		return( FALSE );			
	}

}
#endif

/****************************************************************************************************/
/*																									*/
/*		MEMOBUS MESSAGE MANAGER																		*/
/*																									*/
/****************************************************************************************************/
static void	mbusFuncMsgManager( MEMOBUS_IF *MbusIfPtr )
{
	LONG	SubFuncCode;
	UCHAR  *CmdBuf;	
	
	CmdBuf = MbusIfPtr->CmdBuf;		/* 受信メッセージ先頭アドレス */
	SubFuncCode = (LONG)CmdBuf[4];	/* サブファンクションコード   */
	
	//KpiRstLongTimer( &(MbusIfPtr->ComTimeLong) );		/* コマンド応答タイムアウト用	*/
	#if 0 /* 書き込み禁止処理現状未対応 */
	RpiSetAccsLvl( &Access, MsgPtr->AccsLvl );
	#endif

	switch( SubFuncCode )/* サブファンクションコード */
	{
		case MBUS_RD_CMEM_42H:		/* 01H メモリの読み出し（連続） */
			mbusFuncReadMemory42H( MbusIfPtr, SubFuncCode );
			break;
		case MBUS_WR_CMEM_42H:		/* 02H メモリの書き込み（連続)	*/
			mbusFuncWriteMemory42H( MbusIfPtr, SubFuncCode );
			break;
		case MBUS_RD_NCMEM_42H:		/* 03H メモリの読み出し（非連続)*/
			mbusFuncReadMemory42H( MbusIfPtr, SubFuncCode );
			break;
		case MBUS_WR_NCMEM_42H:		/* 04H メモリの書き込み（非連続)*/
			mbusFuncWriteMemory42H( MbusIfPtr, SubFuncCode );
			break;
		case MBUS_RD_MAXSIZE_42H:	/* 11H 最大サイズ読み出し		*/
			mbusFuncReadMaxSize42H( MbusIfPtr, SubFuncCode );
			break;

/* <S061>	Start */
#ifdef _INFO_ID_READ_CHECK_
		case MBUS_RD_INFO_42H:	/* ffH(未定) 製品情報読み出し	*/
			mbusFuncReadProductInfo42H( MbusIfPtr, SubFuncCode );
			break;
#endif
/* <S061>	End */

		default:					/* コマンド不一致				*/
			mbusFuncErrorManager42H( MbusIfPtr, FUNCCODE_ERROR, 0 );
			break;
	}

	/* 送信データ長チェック */
	if(( MbusIfPtr->TrnbLength > (USHORT)MBUS_MAXSIZE ) || ( MbusIfPtr->TrnbLength < MBUS_MINSIZE ) )
	{
		/* 送信データ長異常	*/
		mbusFuncErrorManager42H( MbusIfPtr, DATANUMB_ERROR, 0 );
	}
}

/****************************************************************************************************/
/*																									*/
/*		Memobus：異常応答返信処理																	*/
/*																									*/
/****************************************************************************************************/
static void	mbusFuncErrorManager42H( MEMOBUS_IF *MbusIfPtr, LONG ErrorCode, ULONG ErrorAddress )
{
	UCHAR  *CmdBuf;
	UCHAR  *ResBuf;

	/* 受信、送信データ先頭アドレス */
	CmdBuf = MbusIfPtr->CmdBuf;
	ResBuf = MbusIfPtr->ResBuf;

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
	MbusIfPtr->TrnbLength = MBUS_HEAD_BASE_SIZE;

/* <S056>	Start */
	/* レジスタ読み出し:MEMOBUSエラー情報(エラー発生アドレス) */
	MbusIfPtr->ErrRno = ErrorAddress;
/* <S056>	End */
}

/****************************************************************************************************/
/*																									*/
/*		Memobus：ループバックテスト																	*/
/*																									*/
/****************************************************************************************************/
static void	mbusLoopbackTest( MEMOBUS_IF *MbusIfPtr )
{
ULONG	i;
USHORT	*CmdBuf = (USHORT *)(MbusIfPtr->CmdBuf);
USHORT	*ResBuf = (USHORT *)(MbusIfPtr->ResBuf);

/*--------------------------------------------------------------------------------------------------*/
/*		応答データ作成 (CRC-16計算は不要)															*/
/*--------------------------------------------------------------------------------------------------*/
		for ( i = 0; i < /*MbusStdCmd[ FuncCodeIdx ].PacketSize*/ 8 / 2; i++ )
		{
			ResBuf[i] = CmdBuf[i];
		}
//		MbusIfPtr->TrnbLength = sizeof(MbusIfPtr->RxBuf[ MSG_MAXBUFSZB ]);		/* Set size of Response data	*/
		MbusIfPtr->TrnbLength = 8;		/* Set size of Response data	*/

		return;
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
/*		Memobus：メモリ読み出し（連続／非連続共通）													*/
/*																									*/
/****************************************************************************************************/
static void	mbusFuncReadMemory42H( MEMOBUS_IF *MbusIfPtr, LONG SubFuncCode )
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

	CmdBuf = MbusIfPtr->CmdBuf;
	ResBuf = MbusIfPtr->ResBuf;

	Value  = 0;
	//Mode      = (CmdBuf[5] >> 4);					/* Mode		*/
	DataType  = (CmdBuf[5] & 0x0F);					/* DataType */
	DataNum   = (CmdBuf[6] << 8) + CmdBuf[7];		/* DataNum	*/
	

	/* データ型チェック */
	if( (DataType != 1) && (DataType != 2) && (DataType != 3))	
	{
		/* モード/データ型エラー*/
		mbusFuncErrorManager42H( MbusIfPtr, MODETYPE_ERROR, 0 );
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
	if(  (CmdLength != MbusIfPtr->RcvbLength )
		|| (RspLength > (MBUS_MAXSIZE - MBUS_HEAD_FIX_SIZE)) || (DataNum < MBUS_MIN_REGNUM)) 
#else
	if( (RspLength > (MBUS_MAXSIZE - MBUS_HEAD_FIX_SIZE)) || (DataNum < MBUS_MIN_REGNUM)) 
#endif
	{	/* 個数不良エラー */
		mbusFuncErrorManager42H( MbusIfPtr, DATANUMB_ERROR, BeginAddress );	
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
		hRegMngr = mbusFuncConvAddress(MbusIfPtr->AxisMap,&AccessPrm.MemAddr);		
		if( hRegMngr == NULL )
		{	/* メモリアドレスエラー */
			//mbusFuncErrorManager42H( MbusIfPtr, ret, CurrentAddress );//kira_1202
			mbusFuncErrorManager42H( MbusIfPtr, SELAXIS_ERROR, CurrentAddress );
			return;
		}

		/* 格納するデータ位置までのオフセットを計算	*/
		Offset = MBUS_HEAD_FIX_SIZE + i*AccessSize;

		/* パラメータ属性取得 */
		if( NULL == RegMngrGetAttrByMemAddr(hRegMngr, &AccessPrm) )
		{	/* メモリアドレスエラー	*/
			mbusFuncErrorManager42H( MbusIfPtr, MEMADRES_ERROR, CurrentAddress );
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
					mbusFuncErrorManager42H( MbusIfPtr, DATANUMB_ERROR, CurrentAddress );
					return;
				}
			}
			i = i + (BlockNum-1);				/* ブロック読み出し回数分iを加算する	*/
		}
		else
		{	
			/* 個数不良 */
			mbusFuncErrorManager42H( MbusIfPtr, DATANUMB_ERROR, CurrentAddress );
			return;
		}

		/* データ読み出し */
		AccessPrm.IfHandle = MbusIfPtr;
		ret = (LONG)PrmReadValue( &AccessPrm, &Value, RegSize);
		if( PRM_RSLT_SUCCESS != ret )
		{
			mbusFuncErrorManager42H( MbusIfPtr, ret, CurrentAddress );
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
	MbusIfPtr->TrnbLength = MBUS_HEAD_FIX_SIZE + AccessSize*DataNum;	/* 送信バッファサイズ決定 */
}

/****************************************************************************************************/
/*																									*/
/*		Memobus：メモリ書き込み（連続／非連続共通）													*/
/*																									*/
/****************************************************************************************************/
static void	mbusFuncWriteMemory42H( MEMOBUS_IF *MbusIfPtr, LONG SubFuncCode )
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
	CmdBuf = MbusIfPtr->CmdBuf;
	ResBuf = MbusIfPtr->ResBuf;

//	Mode     = (CmdBuf[5] >> 4);					/* モード	*/
	DataType = (CmdBuf[5] & 0x0F);					/* データ型 */
	DataNum  = (CmdBuf[6] << 8) + CmdBuf[7];		/* データ数	*/

	/* データ型チェック */
	if( (DataType != 1) && (DataType != 2) && (DataType != 3))	
	{	/* モード/データ型エラー*/
		mbusFuncErrorManager42H( MbusIfPtr, MODETYPE_ERROR, 0 );
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
	if( (MbusIfPtr->RcvbLength != CmdLength ) || (RspLength > MaxSize) || (DataNum < MBUS_MIN_REGNUM) )
#else
	if( (RspLength > MaxSize) || (DataNum < MBUS_MIN_REGNUM) )
#endif
	{	/* 個数不良 */
		mbusFuncErrorManager42H( MbusIfPtr, DATANUMB_ERROR, BeginAddress );		
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
		hRegMngr = mbusFuncConvAddress(MbusIfPtr->AxisMap,&AccessPrm.MemAddr);		
		if( hRegMngr == NULL )
		{	/* メモリアドレスエラー */
			//mbusFuncErrorManager42H( MbusIfPtr, ret, CurrentAddress );
			mbusFuncErrorManager42H( MbusIfPtr, SELAXIS_ERROR, CurrentAddress );//kira_1202
			return;
		}

		/* パラメータ属性取得 */
		if( NULL == RegMngrGetAttrByMemAddr(hRegMngr, &AccessPrm) )
		{	/* メモリアドレスエラー	*/
			mbusFuncErrorManager42H( MbusIfPtr, MEMADRES_ERROR, CurrentAddress );
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
					mbusFuncErrorManager42H( MbusIfPtr, DATANUMB_ERROR, CurrentAddress );
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
			mbusFuncErrorManager42H( MbusIfPtr, DATANUMB_ERROR, CurrentAddress );
			return;
		}

		/* データ書き込み */
		AccessPrm.IfHandle = MbusIfPtr;
		ret = (LONG)PrmWriteValue( &AccessPrm, &Value, RegSize);
		if(0 > ret)
		{
			mbusFuncErrorManager42H( MbusIfPtr, ret, CurrentAddress );
			return;
		}
	}

	MlibCopyByteMemory( ResBuf, CmdBuf, MBUS_HEAD_FIX_SIZE);	/* 応答データ作成 */
	MbusIfPtr->TrnbLength = MBUS_HEAD_FIX_SIZE;					/* 送信バッファサイズ決定 */

}

/****************************************************************************************************/
/*																									*/
/*		Memobus：最大サイズ読み出し																	*/
/*																									*/
/****************************************************************************************************/
static void	mbusFuncReadMaxSize42H( MEMOBUS_IF *MbusIfPtr, LONG SubFuncCode )
{
	LONG	MsgSize;

	UCHAR  *CmdBuf;//受信バッファ先頭ポインタ
	UCHAR  *ResBuf;//送信バッファ先頭ポインタ

	/* 受信、送信データ先頭アドレス格納 */
	CmdBuf = MbusIfPtr->CmdBuf;
	ResBuf = MbusIfPtr->ResBuf;

#ifdef __RCVLEN_CHECK__
	/*----------------------------------------------------------------------------------------------*/
	/*		受信データ長チェック																	*/
	/*----------------------------------------------------------------------------------------------*/
	/* 受信データ長が不正 */
	if( MbusIfPtr->RcvbLength != MBUS_HEAD_FIX_SIZE )
	{	/* 個数不良エラー	*/
		mbusFuncErrorManager42H( MbusIfPtr, DATANUMB_ERROR, BeginAddress );		
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
	MbusIfPtr->TrnbLength = MBUS_HEAD_BASE_SIZE;

}
#endif	/* <S150> */

/****************************************************************************************************/
/*																									*/
/*	メッセージバッファ入れ替え関数																	*/
/*																									*/
/****************************************************************************************************/
void	mbusFuncBufSwap( UCHAR *Buf )
{
#ifdef __BIG_ENDIAN__
/* BigEndianのとき */

	LONG	i;
	LONG	Num;
	UCHAR	tmp[4];

	Num = MBUS_MAXSIZE / 4;

	for( i=0; i<Num; i++ )
	{
		tmp[0] = Buf[i*4    ]; 
		tmp[1] = Buf[i*4 + 1]; 
		tmp[2] = Buf[i*4 + 2]; 
		tmp[3] = Buf[i*4 + 3]; 

		Buf[i*4    ] = tmp[3];
		Buf[i*4 + 1] = tmp[2];
		Buf[i*4 + 2] = tmp[1];
		Buf[i*4 + 3] = tmp[0];
	}
#else
/* LittleEndianのとき */
	/* 未確認 */
	/*何もしない*/

#endif
}

#if 0	/* <S150> */
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

/****************************************************************************************************/
/*																									*/
/*		MEMOBUSメッセージ受信バッファアドレス提供関数												*/
/*																									*/
/*			戻り値：送信バッファの先頭アドレス														*/
/*																									*/
/****************************************************************************************************/
static UCHAR	*mbusFuncGetRcvAdr( MEMOBUS_IF *MbusIfPtr )
{
	return( &MbusIfPtr->RxBuf[ MbusIfPtr->RelayHdSize + MBUS_HDROFS ] );		
}

/****************************************************************************************************/
/*																									*/
/*		MEMOBUSメッセージ送信バッファアドレス提供関数												*/
/*																									*/
/*			戻り値：受信バッファの先頭アドレス														*/
/*																									*/
/****************************************************************************************************/
 static UCHAR	*mbusFuncGetTrnsAdr( MEMOBUS_IF *MbusIfPtr )
{
	return( &MbusIfPtr->TxBuf[ MbusIfPtr->RelayHdSize + MBUS_HDROFS ] );		
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

/* <S061>	Start */
#ifdef _INFO_ID_READ_CHECK_
/****************************************************************************************************/
/*																									*/
/*		Memobus：製品情報読み出し																	*/
/*																									*/
/****************************************************************************************************/
static void mbusFuncReadProductInfo42H( MEMOBUS_IF *MbusIfPtr, LONG SubFuncCode )
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

	LONG	ReadIdRegAdd;
	UCHAR	ReadDataSize;
	UCHAR	ReadDataNum;

	LONG	i;
	LONG	j;
	
	LONG	ret;
	LONG	CurrentDataLength;

	/* 受信、送信データ先頭アドレス */
	UCHAR  *CmdBuf;	
	UCHAR  *ResBuf;

	CmdBuf = MbusIfPtr->CmdBuf;
	ResBuf = MbusIfPtr->ResBuf;

	/* 変数初期化 */
	ret = 0;
	NextCmdBuffOffset = 0;							/* 次回ブロック数格納バッファオフセット */
	RspBuffOffset = MBUS_IDRD_RSP_HEAD_BASE_SIZE;	/* RspBuffデータ部オフセット */
	RspBuffSize = MBUS_IDRD_RSP_HEAD_BASE_SIZE;		/* RspBuffデータサイズ */
	BlockSize = CmdBuf[MBUS_IDRD_HEAD_BASE_SIZE];	/* 読み出しブロック数取得 */

	/* ブロック数チェック  */
	if( BlockSize == 0) 
	{	/* 設定値範囲外エラー  */
		mbusFuncErrorManager42H( MbusIfPtr, RANGOUTS_ERROR, 0 );	
		return;
	}

	for ( i = 0; i < BlockSize; i++ )
	{
		CmdBuffOffset = MBUS_IDRD_HEAD_BASE_SIZE + 1 + NextCmdBuffOffset;	/* 読み出すブロックサイズが格納されているCmdBuffのオフセット算出 */
		ReadBlockSize =  CmdBuf[CmdBuffOffset];								/* 読み出すブロックサイズの取得 */

		/* ブロックサイズチェック */
		if( (ReadBlockSize < 3) || (ReadBlockSize > MBUS_IDRD_ID_NUM + 2) )
		{
			mbusFuncErrorManager42H( MbusIfPtr, DATANUMB_ERROR, 0 );	
			return;
		}
		else
		{
			ReadSlotNo = CmdBuf[CmdBuffOffset + 1];			/* 「スロット番号」取得 */
			/* スロット番号チェック  */
			if( ReadSlotNo > (MBUS_IDRD_SLOT_NUM - 1) )
			{
				/* 設定値範囲外エラー */
				mbusFuncErrorManager42H( MbusIfPtr, RANGOUTS_ERROR, 0 );	
				return;
			}

			ReadAxisNo = CmdBuf[CmdBuffOffset + 2];			/* 「軸番号」取得 */
			ResBuf[RspBuffOffset] = ReadBlockSize;			/* 「ブロックサイズ」をレスポンスバッファに格納 */
			ResBuf[RspBuffOffset + 1] = ReadSlotNo;			/* 「スロット番号」をレスポンスバッファに格納 */
			ResBuf[RspBuffOffset + 2] = ReadAxisNo;			/* 「軸番号」をレスポンスバッファに格納 */

			RspBuffSize = RspBuffSize + 3;					/* 受信データサイズ更新 */
			RspBuffOffset = RspBuffOffset + 3;				/* 応答バッファオフセット更新 */

			/* 応答バッファサイズチェック */
			if( RspBuffSize > MBUS_MAXSIZE )
			{	/* 個数不良エラー */
				mbusFuncErrorManager42H( MbusIfPtr, DATANUMB_ERROR, 0 );	
				return;
			}

			/* 読み出し製品情報取得 */
			for ( j = 0; j < (ReadBlockSize - 2 ) ; j++ )
			{
				ReadIdNo =  CmdBuf[CmdBuffOffset + (3 + j)];	/* 読み出すID */;

				/* ID-No.チェック */
				if( ReadIdNo > (MBUS_IDRD_ID_NUM - 1) )
				{
					/* 設定値範囲外エラー */
					mbusFuncErrorManager42H( MbusIfPtr, RANGOUTS_ERROR, 0 );	
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
						mbusFuncErrorManager42H( MbusIfPtr, DATANUMB_ERROR, ReadIdRegAdd );	
						return;
					}


					if( ReadIdNo == 0x02 )	/* ID2(仕様)読み出しの場合 */
					{
						mbusFuncSetSpecialSpecIdData( MbusIfPtr, RspBuffOffset);	/* 仕様取得 */
						RspBuffOffset = RspBuffOffset + CurrentDataLength;			/* 応答バッファ(データ部)オフセット更新 */
					}
					else					/* ID2(仕様)読み出し以外の場合 */
					{
						ret = mbusFuncSetIdData( MbusIfPtr, ReadIdRegAdd, ReadDataSize, ReadDataNum, RspBuffOffset);
						if(ret !=0 )
						{
							/* エラー */
							mbusFuncErrorManager42H( MbusIfPtr, ret, ReadIdRegAdd );
							return;
						}
						else
						{
							RspBuffOffset = RspBuffOffset + CurrentDataLength;	/* 応答バッファ(データ部)オフセット更新 */

						}
					}
				}
			}
			
			NextCmdBuffOffset = NextCmdBuffOffset + ReadBlockSize + 1;	/* 次回ブロック数格納バッファオフセット更新 */
		}
	}

	MlibCopyByteMemory( ResBuf, CmdBuf, MBUS_IDRD_HEAD_BASE_SIZE+1);	/* 応答データ作成 */
	MbusIfPtr->TrnbLength = RspBuffSize;								/* 送信バッファサイズ決定 */
	
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
static LONG mbusFuncSetIdData( MEMOBUS_IF *MbusIfPtr, LONG ReadIdRegAdd, UCHAR ReadDataSize, UCHAR ReadDataNum, LONG RspBuffOffset)
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

	CmdBuf = MbusIfPtr->CmdBuf;
	ResBuf = MbusIfPtr->ResBuf;

		ret = 0;

	for ( i = 0; i < ReadDataNum; i++ )
	{
			CurrentAddress = ReadIdRegAdd + (2*i);

		/* 仮想メモリ　→　実アドレス変換 */
		AccessPrm.MemAddr = CurrentAddress;
		hRegMngr = mbusFuncConvAddress(MbusIfPtr->AxisMap,&AccessPrm.MemAddr);		
		if( hRegMngr == NULL )
		{	/* メモリアドレスエラー */
			mbusFuncErrorManager42H( MbusIfPtr, SELAXIS_ERROR, CurrentAddress );
			ret = SELAXIS_ERROR;
			return ret;
		}

		/* パラメータ属性取得 */
		if( NULL == RegMngrGetAttrByMemAddr(hRegMngr, &AccessPrm) )
		{	/* メモリアドレスエラー	*/
			mbusFuncErrorManager42H( MbusIfPtr, MEMADRES_ERROR, CurrentAddress );
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
			mbusFuncErrorManager42H( MbusIfPtr, DATANUMB_ERROR, CurrentAddress );
			ret = DATANUMB_ERROR;
			return ret;
		}

		/* データ読み出し */
		AccessPrm.IfHandle = MbusIfPtr;
		ret = (LONG)PrmReadValue( &AccessPrm, &Value, RegSize);
		if( PRM_RSLT_SUCCESS != ret )
		{
			mbusFuncErrorManager42H( MbusIfPtr, ret, CurrentAddress );
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
static void mbusFuncSetSpecialSpecIdData( MEMOBUS_IF *MbusIfPtr, LONG RspBuffOffset)
{

	CHAR ServoSpecialSpec[4];

	/* 受信、送信データ先頭アドレス */
	UCHAR  *ResBuf;
	ResBuf = MbusIfPtr->ResBuf;	

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

#endif
/* <S061>	End */
#endif	/* <S150> */

/***************************************** end of file **********************************************/
