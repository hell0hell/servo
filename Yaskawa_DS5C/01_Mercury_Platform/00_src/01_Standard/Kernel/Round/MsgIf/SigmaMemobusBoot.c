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
//#include 	"Information.h"						/* Information */
//#include	"UsbFunctionIF.h"					/* TODO: 後で消す */
//#include	"Memory.h"							/* Data Trace */
#include	"FirmDownload.h"					/* FirmDownload(50h)	<S077>	*/
#include	"MemobusFunctions.h"				/* <S150> */
#include	"HwDeviceIf.h"						/* <S1A2> */

/* <boot> >>>>> */
#ifndef NULL
#define NULL	0
#endif
/* <<<<< <boot> */
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
void	RpxMsgifFuncExecResponse( HMSGIF Hmsg );		/* MessageI/F：補助機能実行結果応答処理		*/
/*--------------------------------------------------------------------------------------------------*/
/*		Lpx関数定義：関数名変更自由																	*/
/*--------------------------------------------------------------------------------------------------*/
void	LpxMbusLoopbackTest( HMSGIF Hmsg );				/* Memobus：ループバックテスト				*/
/*--------------------------------------------------------------------------------------------------*/
void	LpxMbusWriteOneRegister( HMSGIF Hmsg );			/* Memobus：単一レジスタ書込み				*/
/*--------------------------------------------------------------------------------------------------*/
void	LpxMbusReadSeqRegisters( HMSGIF Hmsg );			/* Memobus：連続レジスタ読出し				*/
void	LpxMbusWriteSeqRegisters( HMSGIF Hmsg );		/* Memobus：連続レジスタ書込み				*/
/*--------------------------------------------------------------------------------------------------*/
void	LpxMbusReadMaxNumberOfRegisters( HMSGIF Hmsg );	/* Memobus：最大レジスタ個数読出し			*/
/*--------------------------------------------------------------------------------------------------*/
LONG	LpxGetMaxNumberOfRdRegisters( HMSGIF Hmsg, LONG FixedLen );
LONG	LpxGetMaxNumberOfWrRegisters( HMSGIF Hmsg, LONG FixedLen );
/*--------------------------------------------------------------------------------------------------*/
void	LpxMbusSendErrorResponse( HMSGIF Hmsg, ULONG ErrCode );
LONG	LpxRegifRegNoError( LONG RegNo, LONG Number, USHORT *RdBuf, HMSGIF Hmsg );
/*--------------------------------------------------------------------------------------------------*/
/****************************************************************************************************/

/****************************************************************************************************/
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

/****************************************************************************************************/
/*																									*/
/*		MessageI/F : Main Procedure																	*/
/*																									*/
/****************************************************************************************************/
void	RpxMsgifMainProcedure( HMSGIF Hmsg )
{
/*--------------------------------------------------------------------------------------------------*/
/*		Reset Hmsg Variables																		*/
/*--------------------------------------------------------------------------------------------------*/
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
			 switch( Hmsg->CmdBuf[3] )
			 {
//			 case SFC_WR_ONEREG: LpxMbusWriteOneRegister( Hmsg ); break;
			 case SFC_RD_SEQREG: LpxMbusReadSeqRegisters( Hmsg ); break;
//			 case SFC_RD_MAXNUM: LpxMbusReadMaxNumberOfRegisters( Hmsg ); break;
			 default: LpxMbusSendErrorResponse( Hmsg, SMBUS_EC_FNCDERR ); break;
			 }
			 break;
 
		case MFC_SYSDWNLD:
			 MbusFuncSystemDownLoad( Hmsg );
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


/****************************************************************************************************/
/*																									*/
/*		Memobus：単一レジスタ書込み																	*/
/*																									*/
/****************************************************************************************************/
void	LpxMbusWriteOneRegister( HMSGIF Hmsg )
{
#if 0	/*<boot>*/
LONG	rc;
USHORT*	wCmdBuf = (USHORT *)(Hmsg->CmdBuf);
USHORT*	wResBuf = (USHORT *)(Hmsg->ResBuf);
LONG	RegNo   = MlibCnvBig16( wCmdBuf[2+s] );
/*--------------------------------------------------------------------------------------------------*/
/*		レジスタ書込み完了応答作成																	*/
/*--------------------------------------------------------------------------------------------------*/
		wResBuf[0] = wCmdBuf[0];						/* AxisAddr, FuncCode						*/
		wResBuf[1] = wCmdBuf[1];						/* SubFuncCode								*/
/*--------------------------------------------------------------------------------------------------*/
/*		応答データ送信指令																			*/
/*--------------------------------------------------------------------------------------------------*/
		MSGIF_SEND( Hmsg->ResBuf, 10+(2*s) );		/* Start Transmit Response Message			*/
		return;
#endif	/*<boot>*/
}



/****************************************************************************************************/
/*																									*/
/*		Memobus：連続レジスタ読出し																	*/
/*																									*/
/****************************************************************************************************/
void	LpxMbusReadSeqRegisters( HMSGIF Hmsg )
{
LONG	i;
LONG	Length;
LONG	s = LPX_GET_SLOT_WORDLEN( Hmsg->CmdBuf[2] );
USHORT*	wCmdBuf = (USHORT *)(Hmsg->CmdBuf);
USHORT*	wResBuf = (USHORT *)(Hmsg->ResBuf);
LONG	RegNo   = MlibCnvBig16( wCmdBuf[2+s] );
LONG	RegNum  = MlibCnvBig16( wCmdBuf[3+s] );
USHORT	*Ptr;
ULONG	BoardId;	/*<S1A2>*/
/*--------------------------------------------------------------------------------------------------*/
/*		レジスタ番号判定																			*/
/*--------------------------------------------------------------------------------------------------*/
		switch( RegNo )
		{
		case 0xC800:	/* サーボパック形式 */
//<S1A2>			Ptr = (USHORT*)"SGD7X-XXXXXXX\0\0\0";
			BoardId = HALdrv_ReadBoardID();
			switch( BoardId )
			{
			case BOARD_ID_S_AP:
				Ptr = (USHORT*)"SGD7S-XXA00A\0\0\0\0";
				break;
			case BOARD_ID_S_M2:
				Ptr = (USHORT*)"SGD7S-XXA10A\0\0\0\0";
				break;
			case BOARD_ID_S_M3:
				Ptr = (USHORT*)"SGD7S-XXA20A\0\0\0\0";
				break;
			case BOARD_ID_W_M3:
				Ptr = (USHORT*)"SGD7W-XXA20A\0\0\0\0";
				break;
			default:
				Ptr = (USHORT*)"SGD7X-XXAXXA\0\0\0\0";
				break;
			}

			break;
		case 0xC009:	/* サーボ容量 */
			Ptr = (USHORT*)"\x00\x32\x00\x00";
			break;
		case 0xC00B:	/* サーボ電圧 */
			Ptr = (USHORT*)"\x01\x01";
			break;
		case 0xC001:	/* パラメータセットID */
			Ptr = (USHORT*)"\x00\x00\x00\x00\x00\x00\x00\x00";
			break;
		case 0xC007:	/* Y仕様 */
			Ptr = (USHORT*)"\x00\x00";
			break;
		default:
			LpxMbusSendErrorResponse( Hmsg, REGIF_DAMTHERR2 );
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
//		for( i=0; i < RegNum; i++ ){ wResBuf[3+s+i] = MlibCnvBig16( wResBuf[3+s+i] );}
		for( i=0; i < RegNum; i++ )
		{
			wResBuf[3+s+i] = Ptr[i];
		}
/*--------------------------------------------------------------------------------------------------*/
/*		CRC-16作成＆応答データ送信指令																*/
/*--------------------------------------------------------------------------------------------------*/
		Length = (RegNum * 2) + 8 + (2*s);				/* ResMsg Length							*/
		MlibSetCRC16MB(  Hmsg->ResBuf, Length );		/* Set CRC-16 to ResBuf						*/
		MSGIF_SEND( Hmsg->ResBuf, Length );				/* Start Transmit Response Message			*/
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


/***************************************** end of file **********************************************/
