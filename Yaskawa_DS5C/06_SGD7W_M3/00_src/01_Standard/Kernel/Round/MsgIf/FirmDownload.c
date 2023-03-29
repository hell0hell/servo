/****************************************************************************************************/
/*																									*/
/*																									*/
/*		FirmDownload.c : MEMOBUSファームダウンロード管理											*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : MEMOBUSファームダウンロード通信ＩＦ処理を行う。											*/
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
/*	Note	:	初版	2013.07.08	Y.Tanaka	For Mercury											*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "MemobusIF.h"
#include "MemobusFunctions.h"
#ifndef	_MERCURY_PLATFORM_			/*<S190>*/
#include "PrmAccessIf.h"
#endif	/*_MERCURY_PLATFORM_*/
#include "MLib.h"
#include "FirmDownload.h"

//@@仮対応
#ifndef	_MERCURY_PLATFORM_			/*<S190>*/
#include "MercuryGlobals.h"
#endif	/*_MERCURY_PLATFORM_*/
#include "Sif.h"
#include "MsgIfDef.h"
#include "KnlApi.h"

#include "elf.h"	/* 暗号化フォーマット対応 <S146> */
#ifdef _MERCURY_PLATFORM_	/* <S190> */
#include "SpecCommon.h"
//<S1A2>#define SERVO_ID			0x071	/* サーボ機種ID : SFD7S_M2 = 0x0071 */
extern USHORT	ServoId4Boot;				/* サーボ機種ID (ブート失敗時に設定する)*/
#define SERVO_ID	ServoId4Boot
#endif	/* <S190> */


/****************************************************************************************************/
/*																									*/
/*		PROTOTYPE																					*/
/*																									*/
/****************************************************************************************************/
SHORT	MbusFuncSystemDownLoad( HMSGIF Hmsg );
LONG	MbusFuncSysDLGetMsgLength( UCHAR *CmdBuf );
void	MbusFuncSystemDownLoadCheckSum( void );	/* <S146> */
void	MbusFuncSysDlInitWork( void );	/* <S1A2> */

static SHORT	mbusFuncSysDLReadID50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength );				/* 00H ID情報読み出し	*/
static SHORT	mbusFuncSysDLReadSwVersion50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength );		/* 01H バージョン読出し	*/
static SHORT	mbusFuncSysDLReadCondition50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength );		/* 02H プログラム書込み条件取得	*/
static SHORT	mbusFuncSysDLStart50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength );				/* 03H ダウンロード開始指令		*/
static SHORT	mbusFuncSysDLEraseBlock50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength );			/* 04H ブロック消去指令	*/
static SHORT	mbusFuncSysDLWriteBlock50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength );			/* 05H プログラム書込み指令		*/
static SHORT	mbusFuncSysDLRecvEraseInfo50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength );		/* 06H ブロック消去情報指令(消去情報転送のみ.実消去しない)		*/
static SHORT	mbusFuncSysDLRecvWriteInfo50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength );		/* 07H プログラム送信指令(プログラム送信のみ.実書込しない)		*/
static SHORT	mbusFuncSysDLEraseAll50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength );			/* 08H ブロック一括消去指令		*/
static SHORT	mbusFuncSysDLWriteAll50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength );			/* 09H プログラム一括書込み指令		*/
static SHORT	mbusFuncSysDLFinish50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength);				/* 0AH ダウンロード終了指令		*/
static SHORT	mbusFuncSysDLReadProg50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength);			/* 0BH プログラム読出し指令		*/
static SHORT	mbusFuncSysDLReboot50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength);				/* 0CH プログラム再起動指令		*/
static SHORT	mbusFuncSysDLChangeSifComSpd50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength);		/* 0DH シリアルＩＦ速度変更		*/
static SHORT	mbusFuncSysDLReadHistory50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength);			/* 0EH ダウンロード履歴読出し		*/
static SHORT	mbusFuncSysDLWriteHistory50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength);		/* 0FH ダウンロード履歴書込み		*/
static SHORT	mbusFuncSysDLGetCheckSumResult50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength);	/* 10H チェックサム結果取得		*//*<S146>*/
static SHORT	mbusFuncErrorManager50H( HMSGIF Hmsg, LONG ErrorCode, USHORT *TrnbLength);			/* 異常応答返信処理 */
/*--------------------------------------------------------------------------------------------------*/
#ifndef	_MERCURY_PLATFORM_			/*<S190>*/
static void	mbusFuncSetArrayLongData( UCHAR *Buf, ULONG data, UINT AccessSize );
#endif	/*_MERCURY_PLATFORM_*/
/*--------------------------------------------------------------------------------------------------*/
static SHORT	SysDLEraseBlock( ULONG EraseAddr, UCHAR MpuNo, UCHAR FlashNum );					/* ブロック消去処理				*/
static SHORT	SysDLWritePage( ULONG WriteAddr, ULONG* WriteBuf, UCHAR MpuNo, UCHAR FlashNum );	/* ページ書き込み処理			*/
/*--------------------------------------------------------------------------------------------------*/
/* <S146> >>>>> */
static SHORT	checkElfZipSum( UCHAR *pTopAddress );												/* ELFZIPファイルのサム値チェック	*/
static SHORT	checkAreaSum( UCHAR *pTopAddress, LONG Length );									/* 指定領域のサム値チェック			*/
static SHORT	checkSum( void );																	/* 受信バイナリのサム値チェック		*/
/* <<<<< <S146> */

/****************************************************************************************************/
/*																									*/
/*		VARIABLE																					*/
/*																									*/
/****************************************************************************************************/
SYSDL	SysDl;

/****************************************************************************************************/
/*																									*/
/*		MEMOBUS FirmDownload MANAGER																*/
/*																									*/
/****************************************************************************************************/
SHORT	MbusFuncSystemDownLoad( HMSGIF Hmsg )
{
	UCHAR	*CmdBuf;
	UCHAR	*ResBuf;
	UCHAR	SubFuncCode;
	UCHAR	MpuNo;
	USHORT	TrnbLength;
	SHORT	rc = MBUS_BUSY;

//	SysDl.DlStatus = 0x0001;	/* for debug */
	if(SYSDL_CHK_DLMODE_ON())			/* <S077> */
	{
		KpiRstLongTimer( &(SysDl.BpsChkTime) );	/* Reset BpsChkTimer	<S041>		*/
	}

	CmdBuf = Hmsg->CmdBuf;		/* 受信メッセージ先頭アドレス */
	ResBuf = Hmsg->ResBuf;		/* 送信メッセージ先頭アドレス */

	SubFuncCode = CmdBuf[3];	/* サブファンクションコード   */
	MpuNo = CmdBuf[4];
	
	//KpiRstLongTimer( &(Hmsg->ComTimeLong) );		/* コマンド応答タイムアウト用	*/
	#if 0 /* 書き込み禁止処理現状未対応 */
	RpiSetAccsLvl( &Access, MsgPtr->AccsLvl );
	#endif

	/* 共通応答データ作成 */
	ResBuf[0] = CmdBuf[0];	/* slave address	*/
	ResBuf[1] = CmdBuf[1];	/* function code	*/
	ResBuf[2] = CmdBuf[2];	/* sub function code high	*/
	ResBuf[3] = CmdBuf[3];	/* sub function code low	*/
	ResBuf[4] = CmdBuf[4];	/* MPU No	*/
	ResBuf[5] = 0x00;	/* reserved	*/

	switch( SubFuncCode )/* サブファンクションコード */
	{
		case MBUS_DL_ID_RD:			/* 00H ID情報読み出し */
			rc = mbusFuncSysDLReadID50H( Hmsg, MpuNo, &TrnbLength );
			break;
		case MBUS_DL_SWVER_RD:			/* 01H バージョン読出し */
			rc = mbusFuncSysDLReadSwVersion50H( Hmsg, MpuNo, &TrnbLength );
			break;
		case MBUS_DL_WAY_RD:		/* 02H プログラム書込み条件取得	*/
			rc = mbusFuncSysDLReadCondition50H( Hmsg, MpuNo, &TrnbLength );
			break;
		case MBUS_DL_START:		/* 03H ダウンロード開始指令		*/
			rc = mbusFuncSysDLStart50H( Hmsg, MpuNo, &TrnbLength );
			break;
		case MBUS_DL_PRGDEL:		/* 04H ブロック消去指令	*/
			rc = mbusFuncSysDLEraseBlock50H( Hmsg, MpuNo, &TrnbLength );
			break;
		case MBUS_DL_PRGWR:	/* 05H プログラム書込み指令		*/
			rc = mbusFuncSysDLWriteBlock50H( Hmsg, MpuNo, &TrnbLength );
			break;
		case MBUS_DL_DELSND:	/* 06H ブロック消去情報指令(消去情報転送のみ.実消去しない)		*/
			rc = mbusFuncSysDLRecvEraseInfo50H( Hmsg, MpuNo, &TrnbLength );
			break;
		case MBUS_DL_PRGSND:	/* 07H プログラム送信指令(プログラム送信のみ.実書込しない)		*/
			rc = mbusFuncSysDLRecvWriteInfo50H( Hmsg, MpuNo, &TrnbLength );
			break;
		case MBUS_DL_PRGDELA:	/* 08H ブロック一括消去指令		*/
			rc = mbusFuncSysDLEraseAll50H( Hmsg, MpuNo, &TrnbLength );
			break;
		case MBUS_DL_PRGWRA:	/* 09H プログラム一括書込み指令		*/
			rc = mbusFuncSysDLWriteAll50H( Hmsg, MpuNo, &TrnbLength );
			break;
		case MBUS_DL_END:	/* 0AH ダウンロード終了指令		*/
			rc = mbusFuncSysDLFinish50H( Hmsg, MpuNo, &TrnbLength );
			break;
		case MBUS_DL_PRGRD:	/* 0BH プログラム読出し指令		*/
			rc = mbusFuncSysDLReadProg50H( Hmsg, MpuNo, &TrnbLength );
			break;
		case MBUS_DL_REBOOT:	/* 0CH プログラム再起動指令		*/
			rc = mbusFuncSysDLReboot50H( Hmsg, MpuNo, &TrnbLength );
			break;
		case MBUS_DL_SIFCHG:	/* 0DH シリアルＩＦ速度変更		*/
			rc = mbusFuncSysDLChangeSifComSpd50H( Hmsg, MpuNo, &TrnbLength );
			break;
		case MBUS_DL_HIS_RD:	/* 0EH ダウンロード履歴読出し		*/
			rc = mbusFuncSysDLReadHistory50H( Hmsg, MpuNo, &TrnbLength );
			break;
		case MBUS_DL_HIS_WR:	/* 0FH ダウンロード履歴書込み		*/
			rc = mbusFuncSysDLWriteHistory50H( Hmsg, MpuNo, &TrnbLength );
			break;
		case MBUS_DL_CHKSUM:	/* 10H チェックサム結果取得		*/ /* <S146> */
			rc = mbusFuncSysDLGetCheckSumResult50H( Hmsg, MpuNo, &TrnbLength );
			break;
		default:					/* コマンド不一致				*/
			rc = mbusFuncErrorManager50H( Hmsg, FUNCCODE_ERROR, &TrnbLength );
			break;
	}

	if( rc != MBUS_BUSY )
	{
		/* 送信データ長チェック */
		if(( TrnbLength > MBUS_MAXSIZE ) || ( TrnbLength < MBUS_DL_MIN_RSPSIZE ) )
		{
			/* 送信データ長異常	*/
			rc = mbusFuncErrorManager50H( Hmsg, DATANUMB_ERROR, &TrnbLength );
		}

		/* CRC付加 */
		MlibSetCRC16MB( Hmsg->ResBuf, TrnbLength );
	}
	else
	{
		/* 無応答 */
		TrnbLength = 0;
	}

	/* データ送信 */
	Hmsg->Sif->StxRes( Hmsg->Sif, Hmsg->ResBuf, TrnbLength );

	return( rc );

}


/****************************************************************************************************/
/*																									*/
/*		Memobus：00H ID情報読み出し																	*/
/*																									*/
/****************************************************************************************************/
static SHORT	mbusFuncSysDLReadID50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength )
{
	UCHAR	*ResBuf;
	SHORT	rc = MBUS_OK;

	/* 送信データ先頭アドレス		*/
	ResBuf = Hmsg->ResBuf;

	*TrnbLength = MBUS_DL_ID_RD_RSPSIZE;		/* 送信バッファサイズ決定 */

	switch( MpuNo )
	{
	case 0x00:	 /* MainCPU(SoC JL-086) */
		ResBuf[6] = 0x00;	/* reserved	*/
		ResBuf[7] = FLASH_NUM;	/* Flash Num	*/
#if 0	//最終的には構造体を準備する
		ResBuf[8] = (SysDl.DlSts1 >> 8);	/* Download Status1	*/
		ResBuf[9] = (SysDl.DlSts1 & 0x00FF);	/* Download Status1	*/
		ResBuf[10] = (SysDl.DlSts2 >> 8);	/* Download Status2	*/
		ResBuf[11] = (SysDl.DlSts2 & 0x00FF);	/* Download Status2	*/
		ResBuf[12] = (SysDl.DlSts3 >> 8);	/* Download Status3	*/
		ResBuf[13] = (SysDl.DlSts3 & 0x00FF);	/* Download Status3	*/
		ResBuf[14] = (SysDl.DlSts4 >> 8);	/* Download Status4	*/
		ResBuf[15] = (SysDl.DlSts4 & 0x00FF);	/* Download Status4	*/
//		ResBuf[16] = 0x00;	/* Servo ID	*///標準型(ｱﾅﾛｸﾞ/ﾊﾟﾙｽ列形)で仮対応
//		ResBuf[17] = 0x28;	/* Servo ID	*///標準型(ｱﾅﾛｸﾞ/ﾊﾟﾙｽ列形)で仮対応
		ResBuf[16] = (UCHAR)(SERVO_ID>>0);	/* Servo ID	*/
		ResBuf[17] = (UCHAR)(SERVO_ID>>2);	/* Servo ID	*/
#else	//デバッグ用（固定値）
		ResBuf[8] = (SYSDL_STS1 >> 8);	/* Download Status1	*/
		ResBuf[9] = (SYSDL_STS1 & 0x00FF);	/* Download Status1	*/
		ResBuf[10] = (SYSDL_STS2 >> 8);	/* Download Status2	*/
		ResBuf[11] = (SYSDL_STS2 & 0x00FF);	/* Download Status2	*/
		ResBuf[12] = (SYSDL_STS3 >> 8);	/* Download Status3	*/
		ResBuf[13] = (SYSDL_STS3 & 0x00FF);	/* Download Status3	*/
		ResBuf[14] = (SYSDL_STS4 >> 8);	/* Download Status4	*/
		ResBuf[15] = (SYSDL_STS4 & 0x00FF);	/* Download Status4	*/
//		ResBuf[16] = 0x00;	/* Servo ID	*///標準型(ｱﾅﾛｸﾞ/ﾊﾟﾙｽ列形)で仮対応
//		ResBuf[17] = 0x28;	/* Servo ID	*///標準型(ｱﾅﾛｸﾞ/ﾊﾟﾙｽ列形)で仮対応
		ResBuf[16] = (UCHAR)(SERVO_ID>>4);	/* Servo ID <S150>	*/
		ResBuf[17] = (UCHAR)(SERVO_ID>>0);	/* Servo ID <S150>	*/
#endif
		ResBuf[18] = 0x00;	/* Flash0 Manufacture ID	*///Flash数が2つ以上になった時の拡張検討が必要（2つ以上になるの？）
		ResBuf[19] = 0x00;	/* Flash0 Manufacture ID	*/
		ResBuf[20] = 0x00;	/* Flash0 Device ID	*/
		ResBuf[21] = 0x00;	/* Flash0 Device ID	*/
		break;
	case 0x01:		/* OptionCPU (NetX etc...) */
		/* とりあえず空にしておく	*/
		break;
	default:
		mbusFuncErrorManager50H( Hmsg, RANGOUTS_ERROR, TrnbLength );
		rc = MBUS_NG;

		break;
	}

	return( rc );

}


/****************************************************************************************************/
/*																									*/
/*		Memobus：01H バージョン読出し																*/
/*																									*/
/****************************************************************************************************/
static SHORT	mbusFuncSysDLReadSwVersion50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength )
{
	UCHAR	*ResBuf;

	/* 送信データ先頭アドレス		*/
	ResBuf = Hmsg->ResBuf;

	*TrnbLength = MBUS_DL_SWVER_RD_RSPSIZE;			/* 送信バッファサイズ決定 */

	switch( MpuNo )
	{
	case 0x00:	 /* MainCPU(SoC JL-086) */
		ResBuf[6] = 0x00;						/* Ver定義個数=2					*/
		ResBuf[7] = 0x02;						/* Ver定義個数=2					*/
#if 0	//最終的には構造体を準備する
		ResBuf[8] = ( Kprm.Bt2MswVersion >> 8 );		/* Boot2 Ver		*/
		ResBuf[9] = ( Kprm.Bt2MswVersion & 0x00FF );	/* Boot2 Ver		*/
		ResBuf[10] = ( SvSw.MswVersion >> 8 );	 		/* Runtime Ver		*/
		ResBuf[11] = ( SvSw.MswVersion & 0x00FF ); 		/* Runtime Ver		*/
#else	//デバッグ用（固定値）
		ResBuf[8] = (SERVO_BOOT_VER >> 4);		/* Boot2 Ver		*/
		ResBuf[9] = (SERVO_BOOT_VER >> 0);		/* Boot2 Ver		*/
//<S072>		ResBuf[10] = 0x00;		/* Runtime Ver		*/
//<S072>		ResBuf[11] = 0x01; 		/* Runtime Ver		*/
		ResBuf[10] = (SERVO_FIRM_VER >> 24 ) & 0x00FF;		/* <S072> TODO: 後で消す */
		ResBuf[11] = (SERVO_FIRM_VER >> 16 ) & 0x00FF;		/* <S072> TODO: 後で消す */
#endif
		break;
	case 0x01:		/* OptionCPU (NetX etc...) */
		/* とりあえず空にしておく	*/
		break;
	default:
		mbusFuncErrorManager50H( Hmsg, RANGOUTS_ERROR, TrnbLength );
		break;
	}

	return (MBUS_OK);								/* <S072> 戻り値がなかった */
}


/****************************************************************************************************/
/*																									*/
/*		Memobus：02H プログラム書込み条件取得														*/
/*																									*/
/****************************************************************************************************/
static SHORT	mbusFuncSysDLReadCondition50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength )
{
	UCHAR	*ResBuf;
	SHORT	rc = MBUS_OK;

	/* 送信データ先頭アドレス		*/
	ResBuf = Hmsg->ResBuf;

	*TrnbLength = MBUS_DL_WAY_RD_RSPSIZE;		/* 送信バッファサイズ決定 */

	switch( MpuNo )
	{
	case 0x00:	 /* MainCPU(SoC JL-086) */
	case 0x01:	/* OptionCPU (NetX etc...) *//* 共通でいける？ */
		ResBuf[6] = ( SYSDL_WAY >> 8 );		/* ダウンロード書き込み条件		*/
		ResBuf[7] = ( SYSDL_WAY & 0x00FF );	/* ダウンロード書き込み条件		*/
		ResBuf[8] = ( SYSDL_1DLEN >> 8 );	 		/* 1メッセージあたりのバイト数		*/
		ResBuf[9] = ( SYSDL_1DLEN & 0x00FF ); 		/* 1メッセージあたりのバイト数		*/
		break;
	default:
		mbusFuncErrorManager50H( Hmsg, RANGOUTS_ERROR, TrnbLength );
		break;
	}

	return( rc );

}


/****************************************************************************************************/
/*																									*/
/*		Memobus：03H ダウンロード開始指令															*/
/*																									*/
/****************************************************************************************************/
static SHORT	mbusFuncSysDLStart50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength )
{
//	UCHAR	*ResBuf;
	SHORT	rc = MBUS_OK;

	/* 送信データ先頭アドレス		*/
//	ResBuf = Hmsg->ResBuf;
	
	*TrnbLength = MBUS_DL_START_RSPSIZE;		/* 送信バッファサイズ決定 */

	switch( MpuNo )
	{
	case 0x00:	 /* MainCPU(SoC JL-086) */
		/* ベースブロックであればダウンロード開始	*/
		if( SYSDL_CHK_BASE_BLOCK() )
		{
			SYSDL_DLMODE_ON();	/* download mode on	*/
/* <S146> >>>>> */
			/* Sum check flag => off */
			SysDl.MicroSumCheckF = FALSE;
			SysDl.MirSumCheckF = FALSE;
			SysDl.RtmSumCheckF = FALSE;
			SysDl.BootSumCheckF = FALSE;
			/* Roundへの依頼関連フラグ設定 */
			SysDl.CheckSumRequestF = FALSE;
			if( SysDl.CheckSumCalcF == TRUE )
			{
				SysDl.CheckSumCancelF = TRUE;
			}
#ifdef	_MERCURY_PLATFORM_ 		/*<S190>*/
			else
			{
				SysDl.CheckSumCancelF = FALSE;
			}
#endif	/* _MERCURY_PLATFORM_ *//*<S190>*/
			SysDl.CheckSumEndF = FALSE;
/* <<<<< <S146> */
			KpiRstLongTimer( &(SysDl.BpsChkTime) );			/* Reset BpsChkTimer	<S041>	*/
		}
		else
		{
			SYSDL_DLMODE_OFF();	/* download mode off	*/
			mbusFuncErrorManager50H( Hmsg, CONDTION_ERROR, TrnbLength );
		}
		break;
	case 0x01:		/* OptionCPU (NetX etc...) */
		/* とりあえず空にしておく	*/
		/* ベースブロックと上位との通信確認 */
		break;
	default:
		mbusFuncErrorManager50H( Hmsg, RANGOUTS_ERROR, TrnbLength );
		break;
	}

	return( rc );

}


/****************************************************************************************************/
/*																									*/
/*		Memobus：04H ブロック消去指令									<S077>					*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		動作：指定されたMPUのSerialFlashセクター消去コマンド発行									*/
/*		引数：消去するセクターの先頭アドレス														*/
/*		戻り値：OK(0)/BUSY(1)/NG_ADDR(-1)/NG_CMD(-2)												*/
/*			OK:セクター消去動作終了																	*/
/*			BUSY:Erase実行中(継続)																	*/
/*			NG_ADDR:設定アドレス異常																*/
/*			NG_CMD:他コマンド(write/read)実行中														*/
/*																									*/
/****************************************************************************************************/
static SHORT	mbusFuncSysDLEraseBlock50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength )
{
	UCHAR	*CmdBuf;
	UCHAR	*ResBuf;
	UCHAR	FlashNum;		/* 消去先Flash番号 */
	ULONG	EraseAddr;		/* 消去先Flash先頭アドレス */
	LONG	EraseSts;		/* ブロック消去命令実行ステータス */
	SHORT	rc;

	EraseSts = 0/*ERASE_OK*/;
	rc = MBUS_OK;
	
	CmdBuf = Hmsg->CmdBuf;		/* 受信メッセージ先頭アドレス */
	ResBuf = Hmsg->ResBuf;		/* 送信メッセージ先頭アドレス */

	ResBuf[6] = CmdBuf[6];		/* Flash No		*/
	ResBuf[7] = CmdBuf[7];		/* reserved		*/
	ResBuf[8] = CmdBuf[8];	 	/* erase address		*/
	ResBuf[9] = CmdBuf[9];	 	/* erase address		*/
	ResBuf[10] = CmdBuf[10]; 	/* erase address		*/
	ResBuf[11] = CmdBuf[11]; 	/* erase address		*/

	if(SYSDL_CHK_DLMODE_ON())	/* ダウンロードモード中なら実行	*/
	{
		switch( MpuNo )
		{
		case 0x00:	 /* MainCPU(SoC JL-086) */
			SYSDL_ERASE_ON();		/* Start Erase Flash		*/
			FlashNum = ResBuf[6];	/* FlashNum *//* いらない気がする。Flashとアドレスの整合性を取る？ */
			EraseAddr = ((ResBuf[8]<<24)|(ResBuf[9]<<16)|(ResBuf[10]<<8)|(ResBuf[11]));	/* 0xFFE00000から64KB(0x10000)刻み	*/
#ifndef _MERCURY_PLATFORM_			/*<S190>*/
			EraseSts = SysDLEraseBlock( EraseAddr, MpuNo, FlashNum );
#else			/*<S190>*/
			/* ブートでメッセージ処理を行うときは、ブート領域は触らない */
			if( EraseAddr < FLS_BOOT_TOP_ADDRESS )
			{
				EraseSts = SysDLEraseBlock( EraseAddr, MpuNo, FlashNum );
			}
#endif	/* _MERCURY_PLATFORM_ *//*<S190>*/
			if( EraseSts == 0/*ERASE_OK*/ )
			{
				SYSDL_ERASE_OFF();	/* Finish Erase Flash		*/
/* <S146> >>>>> */
				/* Set sum check flag */
				if( FLS_MICRO_CODE_ADDRESS <= EraseAddr )
				{
					if( EraseAddr < FLS_MIRROR_EFW_ADDRESS )
					{
						SysDl.MicroSumCheckF = TRUE;
					}
					else if( EraseAddr < FLS_CODE_EFW_ADDRESS )
					{
						SysDl.MirSumCheckF = TRUE;
					}
					else if( EraseAddr < FLS_BOOT_TOP_ADDRESS )
					{
						SysDl.RtmSumCheckF = TRUE;
					}
					else if( EraseAddr <= FLS_BOOT_BTM_ADDRESS )
					{
						SysDl.BootSumCheckF = TRUE;
					}
				}
/* <<<<< <S146> */
			}
			else if( EraseSts == 1/*ERASE_BUSY*/ )
			{
				rc = MBUS_BUSY;
			}
			else	/* ERASE_NG */
			{
				rc = MBUS_NG;
			}
			break;
		case 0x01:		/* OptionCPU (NetX etc...) */
			/* とりあえず空にしておく	*/
			/* ベースブロックと上位との通信確認 */
			break;
		default:
			mbusFuncErrorManager50H( Hmsg, RANGOUTS_ERROR, TrnbLength );
			break;
		}
	}
	else
	{
		mbusFuncErrorManager50H( Hmsg, CONDTION_ERROR, TrnbLength );
	}

	if( (EraseSts != 1)/*ERASE_BUSY*/ && ((ResBuf[1] & 0x80) == 0))
	{
		*TrnbLength = MBUS_DL_PRGDEL_RSPSIZE;		/* 送信バッファサイズ決定 */
	}
	
	return( rc );

}


/****************************************************************************************************/
/*																									*/
/*		Memobus：05H プログラム書込み指令									<S077>				*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		動作：指定されたMPUのSerialFlash書き込みコマンド発行										*/
/*		引数：																						*/
/*			HMSGIF *Hmsg：書き込み先先頭アドレス													*/
/*			UCHAR MpuNo：MPU番号(0-16)																*/
/*		戻り値：OK(0)/BUSY(1)/NG_ADDR(-1)/NG_CMD(-2)												*/
/*			OK:書き込み動作終了																		*/
/*			BUSY:実行中(継続)																		*/
/*			NG_ADDR:設定アドレス異常																*/
/*			NG_CMD:他コマンド(write/read)実行中														*/
/*																									*/
/****************************************************************************************************/
static SHORT	mbusFuncSysDLWriteBlock50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength )
{
	UCHAR	*CmdBuf;
	UCHAR	*ResBuf;
	UCHAR	FlashNum;		/* 書込み先Flash番号 */
	ULONG	WriteAddr;		/* 書込みFlash先頭アドレス */
	LONG	WriteSts;		/* 書込み命令実行ステータス */
	ULONG	WriteBuf[(SYSDL_1DLEN>>2)];		/* 書込みデータバッファ */
	USHORT	WriteDataNum;
	LONG	i,j;
	SHORT	rc;

	WriteSts = 0/*WRITE_OK*/;
	rc = MBUS_OK;
	
	/* 受信データ先頭アドレス		*/
	CmdBuf = Hmsg->CmdBuf;		/* 受信メッセージ先頭アドレス */
	ResBuf = Hmsg->ResBuf;		/* 送信メッセージ先頭アドレス */

	ResBuf[6] = CmdBuf[6];		/* Flash No		*/
	ResBuf[7] = CmdBuf[7];		/* reserved		*/
	ResBuf[8] = CmdBuf[8];	 	/* data num(Hi)		*/
	ResBuf[9] = CmdBuf[9];	 	/* data num(Lo)		*/
	ResBuf[10] = CmdBuf[10]; 	/* write address		*/
	ResBuf[11] = CmdBuf[11]; 	/* write address		*/
	ResBuf[12] = CmdBuf[12]; 	/* write address		*/
	ResBuf[13] = CmdBuf[13]; 	/* write address		*/

	WriteDataNum = ( (ResBuf[8]<<2) | ResBuf[9] );
	if( SYSDL_1DLEN < WriteDataNum )
	{
		mbusFuncErrorManager50H( Hmsg, RANGOUTS_ERROR, TrnbLength );
	}
	else
	{
		for(i=14,j=0;j<(SYSDL_1DLEN>>2);i+=4,j++)
		{
			WriteBuf[j] = (((CmdBuf[i+3]<<24))|((CmdBuf[i+2]<<16))|((CmdBuf[i+1]<<8))|(CmdBuf[i]));
		}

		if(SYSDL_CHK_DLMODE_ON())	/* ダウンロードモード中なら実行	*/
		{
			switch( MpuNo )
			{
			case 0x00:	 /* MainCPU(SoC JL-086) */
				SYSDL_ERASE_ON();		/* Start Erase Flash		*/
				FlashNum = ResBuf[6];	/* FlashNum *//* いらない気がする。Flashとアドレスの整合性を取る？ */

				SYSDL_WRITE_ON();		/* Start Write Flash		*/
				FlashNum = ResBuf[6];	/* FlashNum *//* いらない気がする。Flashとアドレスの整合性を取る？ */
				WriteAddr = ((ResBuf[10]<<24)|(ResBuf[11]<<16)|(ResBuf[12]<<8)|(ResBuf[13]));	/* 0xFFE00000から64KB(0x10000)刻み	*/
#ifndef _MERCURY_PLATFORM_			/*<S190>*/
				WriteSts = SysDLWritePage( WriteAddr, WriteBuf, MpuNo, FlashNum );
#else			/*<S190>*/
				/* ブートでメッセージ処理を行うときは、ブート領域は触らない */
				if( WriteAddr < FLS_BOOT_TOP_ADDRESS )
				{
					WriteSts = SysDLWritePage( WriteAddr, WriteBuf, MpuNo, FlashNum );
				}
#endif	/* _MERCURY_PLATFORM_ *//*<S190>*/
				if( WriteSts == 0/*WRITE_OK*/ )
				{
					SYSDL_WRITE_OFF();	/* Finish Write Flash		*/
				}
				else if( WriteSts == 1/*WRITE_BUSY*/ )
				{
					rc = MBUS_BUSY;
				}
				else	/* WRITE_NG */
				{
					rc = MBUS_NG;
				}
				break;
			case 0x01:		/* OptionCPU (NetX etc...) */
				/* とりあえず空にしておく	*/
				/* ベースブロックと上位との通信確認 */
				break;
			default:
				mbusFuncErrorManager50H( Hmsg, RANGOUTS_ERROR, TrnbLength );
				break;
			}
		}
		else
		{
			mbusFuncErrorManager50H( Hmsg, CONDTION_ERROR, TrnbLength );
		}
	}

	if( (WriteSts != 1)/*WRITE_BUSY*/ && ((ResBuf[1] & 0x80) == 0) )
	{
		*TrnbLength = MBUS_DL_PRGWR_RSPSIZE;		/* 送信バッファサイズ決定 */
	}
	
	return( rc );

}


/****************************************************************************************************/
/*																									*/
/*		Memobus：06H ブロック消去情報指令(消去情報転送のみ.実消去しない)							*/
/*																									*/
/****************************************************************************************************/
static SHORT	mbusFuncSysDLRecvEraseInfo50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength )
{
//	UCHAR	*ResBuf;//<S0C7>

	/* 送信データ先頭アドレス		*/
//	ResBuf = Hmsg->ResBuf;//<S0C7>

	*TrnbLength = MBUS_DL_DELSND_RSPSIZE;		/* 送信バッファサイズ決定 */
	return MBUS_OK;//<S0C7>
}


/****************************************************************************************************/
/*																									*/
/*		Memobus：07H プログラム送信指令(プログラム送信のみ.実書込しない)																	*/
/*																									*/
/****************************************************************************************************/
static SHORT	mbusFuncSysDLRecvWriteInfo50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength )
{
//	UCHAR	*ResBuf;

	/* 送信データ先頭アドレス		*/
//	ResBuf = Hmsg->ResBuf;//<S0C7>

	*TrnbLength = MBUS_DL_PRGSND_RSPSIZE;		/* 送信バッファサイズ決定 */
	return MBUS_OK;//<S0C7>
}


/****************************************************************************************************/
/*																									*/
/*		Memobus：08H ブロック一括消去指令															*/
/*																									*/
/****************************************************************************************************/
static SHORT	mbusFuncSysDLEraseAll50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength )
{
//	UCHAR	*ResBuf;//<S0C7>

	/* 送信データ先頭アドレス		*/
//	ResBuf = Hmsg->ResBuf;//<S0C7>

	*TrnbLength = MBUS_DL_PRGDELA_RSPSIZE;		/* 送信バッファサイズ決定 */
	return MBUS_OK;//<S0C7>
}


/****************************************************************************************************/
/*																									*/
/*		Memobus：09H プログラム一括書込み指令														*/
/*																									*/
/****************************************************************************************************/
static SHORT	mbusFuncSysDLWriteAll50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength )
{
//	UCHAR	*ResBuf;//<S0C7>

	/* 送信データ先頭アドレス		*/
//	ResBuf = Hmsg->ResBuf;//<S0C7>

	*TrnbLength = MBUS_DL_PRGWRA_RSPSIZE;		/* 送信バッファサイズ決定 */
	return MBUS_OK;//<S0C7>
}


/****************************************************************************************************/
/*																									*/
/*		Memobus：0AH ダウンロード終了指令									<S077>				*/
/*																									*/
/****************************************************************************************************/
static SHORT	mbusFuncSysDLFinish50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength )
{
//	UCHAR	*CmdBuf;//<S0C7>
//	UCHAR	*ResBuf;
	SHORT	rc = MBUS_OK;
//<S146>	SHORT	rslt = OK;

	/* 送信データ先頭アドレス		*/
//	ResBuf = Hmsg->ResBuf;

	*TrnbLength = MBUS_DL_END_RSPSIZE;		/* 送信バッファサイズ決定 */

	switch( MpuNo )
	{
	case 0x00:	 /* MainCPU(SoC JL-086) */
		/* ベースブロックであればダウンロード開始	*/
		if(SYSDL_CHK_DLMODE_ON())	/* ダウンロードモード中なら実行	*/
		{
/* <S146> */
//			//rslt = checkSum();		/* firmware check sum		*/
//			if( NG == rslt )
//			{
//				rc = MBUS_NG;
//				mbusFuncErrorManager50H( Hmsg, CONDTION_ERROR, TrnbLength );
//			}
//			else
//			{
//				SYSDL_DLMODE_RESET();
//			}
			SYSDL_DLMODE_RESET();
			SysDl.CheckSumRequestF = TRUE;	/* チェックサム処理開始依頼 */
		}
		else
		{
			mbusFuncErrorManager50H( Hmsg, CONDTION_ERROR, TrnbLength );
		}
		break;
	case 0x01:		/* OptionCPU (NetX etc...) */
		/* とりあえず空にしておく	*/
		/* ベースブロックと上位との通信確認 */
		break;
	default:
		mbusFuncErrorManager50H( Hmsg, RANGOUTS_ERROR, TrnbLength );
		break;
	}

	return( rc );

}


/****************************************************************************************************/
/*																									*/
/*		Memobus：0BH プログラム読出し指令															*/
/*																									*/
/****************************************************************************************************/
static SHORT	mbusFuncSysDLReadProg50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength )
{
//	UCHAR	*ResBuf;//<S0C7>

	/* 送信データ先頭アドレス		*/
//	ResBuf = Hmsg->ResBuf;//<S0C7>

	*TrnbLength = MBUS_DL_START_RSPSIZE;		/* 送信バッファサイズ決定 */
	return MBUS_OK;//<S0C7>
}


/****************************************************************************************************/
/*																									*/
/*		Memobus：0CH プログラム再起動指令															*/
/*																									*/
/****************************************************************************************************/
static SHORT	mbusFuncSysDLReboot50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength )
{
//	UCHAR	*ResBuf;//<S0C7>

	/* 送信データ先頭アドレス		*/
//	ResBuf = Hmsg->ResBuf;//<S0C7>

	*TrnbLength = MBUS_DL_REBOOT_RSPSIZE;		/* 送信バッファサイズ決定 */
	return MBUS_OK;//<S0C7>
}


/****************************************************************************************************/
/*																									*/
/*		Memobus：0DH シリアルＩＦ速度変更						<S041>								*/
/*																									*/
/****************************************************************************************************/
static SHORT	mbusFuncSysDLChangeSifComSpd50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength )
{
//UCHAR	*ResBuf;  /* <S072> */
SHORT	rc = MBUS_OK;  /* <S072> */
USHORT	*CmdBufW = (USHORT *)(Hmsg->CmdBuf);  /* <S072> */
USHORT	*ResBufW = (USHORT *)(Hmsg->ResBuf);  /* <S072> */

	/* 送信データ先頭アドレス */
//	ResBuf = Hmsg->CmdBuf;

	*TrnbLength = MBUS_DL_SIFCHG_RSPSIZE;		/* 送信バッファサイズ決定 */

	switch( MpuNo )
	{
	case 0x00:	 /* MainCPU(SoC JL-086) */
//		ResBufW[0]	= CmdBufW[0];						/*+ <S072> AxisAdr, FunctionCode			*/
//		ResBufW[1]	= CmdBufW[1];						/*+ <S072> SubFunctionCode					*/
//		ResBufW[2]	= CmdBufW[2];						/*+ <S072> MPU番号							*/
		ResBufW[3]	= CmdBufW[3];						/*+ <S072> 転送速度上位ワード	*/
		ResBufW[4]	= CmdBufW[4];						/*+ <S072> 転送速度下位ワード	*/
		/* とりあえず空にしておく	*/
		break;
	case 0x01:		/* OptionCPU (NetX etc...) */
		/* とりあえず空にしておく	*/
		break;
	default:
		mbusFuncErrorManager50H( Hmsg, RANGOUTS_ERROR, TrnbLength );
		break;
	}

	return( rc );
}


/****************************************************************************************************/
/*																									*/
/*		Memobus：0EH ダウンロード履歴読出し															*/
/*																									*/
/****************************************************************************************************/
static SHORT	mbusFuncSysDLReadHistory50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength )
{
//	UCHAR	*ResBuf;//<S0C7>

	/* 送信データ先頭アドレス		*/
//	ResBuf = Hmsg->ResBuf;//<S0C7>

	*TrnbLength = MBUS_DL_HIS_RD_RSPSIZE;		/* 送信バッファサイズ決定 */

	return (MBUS_OK);									/* <S072> 戻り値がなかった */
}


/****************************************************************************************************/
/*																									*/
/*		Memobus：0FH ダウンロード履歴書込み															*/
/*																									*/
/****************************************************************************************************/
static SHORT	mbusFuncSysDLWriteHistory50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength )
{
//	UCHAR	*ResBuf;//<S0C7>

	/* 送信データ先頭アドレス		*/
//	ResBuf = Hmsg->ResBuf;//<S0C7>

	*TrnbLength = MBUS_DL_HIS_WR_RSPSIZE;		/* 送信バッファサイズ決定 */

	return (MBUS_OK);									/* <S072> 戻り値がなかった */
}


/* <S146> >>>>> */
/****************************************************************************************************/
/*																									*/
/*		Memobus：10H チェックサム結果取得															*/
/*																									*/
/****************************************************************************************************/
static SHORT	mbusFuncSysDLGetCheckSumResult50H( HMSGIF Hmsg, UCHAR MpuNo, USHORT *TrnbLength )
{
	UCHAR	*ResBuf;
	UCHAR	Status;
	SHORT	rc = MBUS_OK;

	Status = 0x40;	/* その他 */

	/* 送信データ先頭アドレス		*/
	ResBuf = Hmsg->ResBuf;
	
	*TrnbLength = MBUS_DL_CHKSUM_RSPSIZE;		/* 送信バッファサイズ決定 */

	switch( MpuNo )
	{
	case 0x00:	 /* MainCPU(SoC JL-086) */
		/* 現在の状態をセットして返す	*/
		if( SysDl.CheckSumCalcF == TRUE )
		{
			Status = 0x80;
		}
		else if( SysDl.CheckSumEndF == TRUE )
		{
			if( SysDl.CheckSumResult == TRUE )
			{
				Status = 0x00;	/* チェックサムOK */
			}
			else
			{
				Status = 0x01;	/* チェックサムNG */
			}
		}
		ResBuf[6] = Status;
		ResBuf[7] = 0x00;
		break;
	case 0x01:		/* OptionCPU (NetX etc...) */
		/* とりあえず空にしておく	*/
		/* ベースブロックと上位との通信確認 */
		break;
	default:
		mbusFuncErrorManager50H( Hmsg, RANGOUTS_ERROR, TrnbLength );
		break;
	}
	
	return( rc );
}
/* <<<<< <S146> */


/****************************************************************************************************/
/*																									*/
/*		Memobus：異常応答返信処理																	*/
/*																									*/
/****************************************************************************************************/
static SHORT	mbusFuncErrorManager50H( HMSGIF Hmsg, LONG ErrorCode, USHORT *TrnbLength )
{
	UCHAR  *CmdBuf;
	UCHAR  *ResBuf;

	/* 受信、送信データ先頭アドレス */
	CmdBuf = Hmsg->CmdBuf;
	ResBuf = Hmsg->ResBuf;

	/* 異常応答データ作成 */
	ResBuf[0]  =  CmdBuf[0];						/* 0:Slave Addrress		*/
	ResBuf[1]  = (CmdBuf[1] | 0x80);				/* 1:Function Code		*/
	ResBuf[2]  =  (UCHAR)ErrorCode;					/* 4:Error Code			*/

	/* エラー返信サイズ＝5byte	*/
	*TrnbLength = MBUS_DL_ERROR_RSPSIZE;

	return (MBUS_OK);								/* <S072> 戻り値がなかった */
}

#ifndef	_MERCURY_PLATFORM_			/*<S190>*/
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
	/* 未確認 */
	Buf[3] = (UCHAR)((data) >> 24);
	Buf[2] = (UCHAR)((data) >> 16);
	Buf[1] = (UCHAR)((data) >>  8);
	Buf[0] = (UCHAR)( data );
#endif
}
#endif	/*_MERCURY_PLATFORM_*//*<S190>*/


/****************************************************************************************************/
/*																									*/
/*		Flashブロック消去処理																		*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		動作：指定されたMPUのSerialFlashセクター消去コマンド発行									*/
/*		引数：																						*/
/*			ULONG EraseAddr	:消去するセクターの先頭アドレス											*/
/*			UCHAR MpuNo		:消去するMPU番号														*/
/*			UCHAR FlashNum	:消去するMPUのFlash番号（拡張用）										*/
/*		戻り値：OK(0)/BUSY(1)/NG_ADDR(-1)/NG_CMD(-2)												*/
/*			OK:セクター消去動作終了																	*/
/*			BUSY:Erase実行中(継続)																	*/
/*			NG_ADDR:設定アドレス異常																*/
/*			NG_CMD:他コマンド(write/read)実行中														*/
/*																									*/
/****************************************************************************************************/
static SHORT	SysDLEraseBlock( ULONG EraseAddr, UCHAR MpuNo, UCHAR FlashNum )
{
	LONG	EraseSts;
	
	EraseSts = 0/*ERASE_OK*/;
	
	switch( MpuNo )
	{
		case 0x00:	 /* MainCPU(SoC JL-086) */
			EraseSts = SFlashEraseSectorSoC( EraseAddr );
			break;
		case 0x01:		/* OptionCPU (NetX etc...) */
//			EraseSts = FlashEraseSectorOption( EraseAddr );
			break;
		default:
			EraseSts = -1/*ERASE_MPUNO_ERROR*/;
			break;
	}

	return( EraseSts );
}


/****************************************************************************************************/
/*																									*/
/*		Flashページ書き込み処理																		*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		動作：指定されたMPUのSerialFlashページライトコマンド発行									*/
/*		引数：																						*/
/*			ULONG WriteAddr	:書き込むページの先頭アドレス											*/
/*			ULONG* WriteBuf	:書き込みデータバッファ													*/
/*			UCHAR MpuNo		:書き込むMPU番号														*/
/*			UCHAR FlashNum	:書き込むMPUのFlash番号（拡張用）										*/
/*		戻り値：OK(0)/BUSY(1)/NG_ADDR(-1)/NG_CMD(-2)												*/
/*			OK		:動作終了																		*/
/*			BUSY	:実行中(継続)																	*/
/*			NG_ADDR	:設定アドレス異常																*/
/*			NG_CMD	:他コマンド(write/read)実行中													*/
/*																									*/
/****************************************************************************************************/
static SHORT	SysDLWritePage( ULONG WriteAddr, ULONG* WriteBuf, UCHAR MpuNo, UCHAR FlashNum )
{
	LONG	WriteSts;
	
	WriteSts = 0/*WRITE_OK*/;
	
	switch( MpuNo )
	{
		case 0x00:	 /* MainCPU(SoC JL-086) */
			WriteSts = SFlashWritePageSoC( WriteAddr, WriteBuf );
			break;
		case 0x01:		/* OptionCPU (NetX etc...) */
//			WriteSts = FlashWriteSectorOption( WriteAddr );
			break;
		default:
			WriteSts = -1/*WRITE_MPUNO_ERROR*/;
			break;
	}

	return( WriteSts );
}


/****************************************************************************************************/
/*																									*/
/*		FirmDownload : 指令メッセージ長取得 																*/
/*																									*/
/****************************************************************************************************/
LONG	MbusFuncSysDLGetMsgLength( UCHAR *CmdBuf )
{
	UCHAR	SubFuncCode;
	SHORT	dlsize;
	LONG	lret = -1;

	SubFuncCode = CmdBuf[3];	/* サブファンクションコード   */


	switch( SubFuncCode )/* サブファンクションコード */
	{
		case MBUS_DL_ID_RD:			/* 00H ID情報読み出し */
			lret = 8;
			break;
		case MBUS_DL_SWVER_RD:			/* 01H バージョン読出し */
			lret = 8;
			break;
		case MBUS_DL_WAY_RD:		/* 02H プログラム書込み条件取得	*/
			lret = 8;
			break;
		case MBUS_DL_START:		/* 03H ダウンロード開始指令		*/
			lret = 8;
			break;
		case MBUS_DL_PRGDEL:		/* 04H ブロック消去指令	*/
			lret = 14;
			break;
		case MBUS_DL_PRGWR:	/* 05H プログラム書込み指令		*/
			dlsize = ((CmdBuf[8]<<8)|(CmdBuf[9]));
			if( (dlsize<0) || (dlsize>32768) )	/* データ数確認	*/
			{
				break;
			}
			lret = dlsize + 16;
			break;
		case MBUS_DL_DELSND:	/* 06H ブロック消去情報指令(消去情報転送のみ.実消去しない)		*/
			lret = 14;
			break;
		case MBUS_DL_PRGSND:	/* 07H プログラム送信指令(プログラム送信のみ.実書込しない)		*/
			dlsize = ((CmdBuf[8]<<8)|(CmdBuf[9]));
			if( (dlsize<0) || (dlsize>32768) )	/* データ数確認	*/
			{
				break;
			}
			lret = dlsize + 16;
			break;
		case MBUS_DL_PRGDELA:	/* 08H プログラム一括消去指令		*/
			lret = 8;
			break;
		case MBUS_DL_PRGWRA:	/* 09H プログラム一括書込み指令		*/
			lret = 8;
			break;
		case MBUS_DL_END:	/* 0AH ダウンロード終了指令		*/
			lret = 8;
			break;
		case MBUS_DL_PRGRD:	/* 0BH プログラム読出し指令		*/
			lret = 14;
			break;
		case MBUS_DL_REBOOT:	/* 0CH プログラム再起動指令		*/
			lret = 8;
			break;
		case MBUS_DL_SIFCHG:	/* 0DH シリアルＩＦ速度変更		*/
//<S041>		lret = 14;
			lret = 12;
			break;
		case MBUS_DL_HIS_RD:	/* 0EH ダウンロード履歴読出し		*/
			lret = 8;
			break;
		case MBUS_DL_HIS_WR:	/* 0FH ダウンロード履歴書込み		*/
			lret = 10;
			break;
		case MBUS_DL_CHKSUM:	/* 10H チェックサム結果取得		*/ /* <S146> */
			lret = 8;
			break;
		default:					/* コマンド不一致				*/
			break;
	}

	return( lret );

}


/* 暗号化フォーマット対応 <S146>  >>>>> */
/****************************************************************************************************/
/*																									*/
/*		MEMOBUS FirmDownload チェックサム処理														*/
/*																									*/
/****************************************************************************************************/
/* <S182> フェーズ管理用に書き直しているため処理内履歴はなし */
/* <S1A3> サブフェーズ追加対応(処理内履歴はなし) */
void	MbusFuncSystemDownLoadCheckSum( void )
{
	SHORT	rc;			/* 戻り値		*/

	/* 変数初期化 */
	rc = CHKSUM_OK;

	/* チェックサム処理要求確認 */
	if( SysDl.CheckSumRequestF == TRUE )
	{
		/* チェックサム処理要求OFF */
		SysDl.CheckSumRequestF = FALSE;

		/* 処理中設定 */
		SysDl.CheckSumCalcF = TRUE;

		/* 処理フェーズ初期化 */
		SysDl.CheckSumPhase = 0;
	}

	/* 処理中ならチェックサム計算実施 */
	if( SysDl.CheckSumCalcF == TRUE )
	{
		/* キャンセル要求確認 */
		if( SysDl.CheckSumCancelF == FALSE )
		{
			/* チェックサム計算 */
			rc = checkSum();

			/* 結果設定 */
			/* 最終フェーズまで完了 */
			if( (rc == CHKSUM_OK) && (SysDl.CheckSumPhase == CHKSUM_PHASE_END) )
			{
				/* 結果 : OK */
				SysDl.CheckSumResult = TRUE;
				/* 終了を設定 */
				SysDl.CheckSumEndF = TRUE;
			}
			/* NGの場合 */
			else if( rc == CHKSUM_NG )
			{
				/* 結果 : NG */
				SysDl.CheckSumResult = FALSE;
				/* 終了を設定 */
				SysDl.CheckSumEndF = TRUE;
			}
		}

		/* キャンセル要求ありか、チェック終了なら処理中をOFF */
		if( (SysDl.CheckSumCancelF == TRUE) || (SysDl.CheckSumEndF == TRUE)  )
		{
			/* 処理中OFF */
			SysDl.CheckSumCalcF = FALSE;
		}
	}

	return;
}


/****************************************************************************************************/
/*																									*/
/*		ELFZIPファイルのサム値チェック		 														*/
/*																									*/
/****************************************************************************************************/
/* <S1A3> 複数スキャンでチェックするように変更(処理内履歴はなし) */
static SHORT	checkElfZipSum( UCHAR *pTopAddress )
{
	SHORT			rc;					/* 戻り値							*/
	FIRM_HEADER_T	*pHeader;			/* ファームヘッダへのポインタ		*/
	UINT32			FirmFileSize;		/* ファイルサイズ					*/
	UINT32			*pZipLocalHeader;	/* ZIPローカルヘッダへのポインタ	*/
	LONG			i;					/* ループカウンタ					*/

	/* 変数初期化 */
	rc = CHKSUM_BUSY;

	pHeader = (FIRM_HEADER_T *)pTopAddress;
	FirmFileSize = pHeader->zip_file_size + sizeof(FIRM_HEADER_T);

	switch( SysDl.CheckSumPhase & CHKSUM_SUB_PHASE_MASK )
	{
	case 0:
		/*------------------*/
		/* ファイルチェック */
		/*------------------*/
		/* 明らかに異常なファイルサイズをチェック */
		if( (pHeader->zip_file_size < sizeof(UINT32))
			|| (pHeader->zip_file_size > FLS_CODE_AREA_SIZE)
			|| (FirmFileSize > FLS_CODE_AREA_SIZE) )
		{
			rc = CHKSUM_NG;
		}

		/* ZIPシグネチャチェック */
		if( rc != CHKSUM_NG )
		{
			pZipLocalHeader = (UINT32 *)(pTopAddress + sizeof(FIRM_HEADER_T));

			if( *pZipLocalHeader != ZIP_LOCAL_HEADER_SIGNATURE )
			{
				rc = CHKSUM_NG;
			}
		}

		/* 初期化 */
		if( rc != CHKSUM_NG )
		{
			SysDl.CheckSumVal = 0;
			SysDl.CheckSumMem = pTopAddress;
			SysDl.CheckSumIdx = 0;
			SysDl.CheckSumPhase++;
		}
		break;

	case 1:
		/* 全域を加算 */
		while( SysDl.CheckSumIdx < FirmFileSize )
		{
			SysDl.CheckSumVal += (UINT16)*SysDl.CheckSumMem;	/* 1バイトずつ加算		*/
			SysDl.CheckSumMem++;
			SysDl.CheckSumIdx++;
			/* 1ブロック分処理したら一旦終了 */
			if( !(SysDl.CheckSumIdx & CHKSUM_BLOCK_MASK ) )
			{
				break;
			}
		}
		/* 最後まで到達したらサム値チェック */
		if( SysDl.CheckSumIdx >= FirmFileSize )
		{
			/*--------------*/
			/* サムチェック */
			/*--------------*/
			/* ファームヘッダ部の全体チェックサム値部分を減算 */
			SysDl.CheckSumMem = (UCHAR *)&(pHeader->all_check_sum);
			for( i = 0; i < sizeof(pHeader->all_check_sum); i++ )
			{
				SysDl.CheckSumVal -= (UINT16)*SysDl.CheckSumMem;	/* 1バイトずつ減算		*/
				SysDl.CheckSumMem++;
			}
			/* チェックサム値加算	*/
			SysDl.CheckSumVal += pHeader->all_check_sum;

			/* 結果判定 */
			if( SysDl.CheckSumVal == 0 )
			{
				rc = CHKSUM_OK;
			}
			else
			{
				rc = CHKSUM_NG;
			}
		}
		break;

	default:
		rc = CHKSUM_NG;
		break;
	}

	return rc;
}


/****************************************************************************************************/
/*																									*/
/*		指定領域のサム値チェック		 															*/
/*																									*/
/****************************************************************************************************/
/* <S1A3> 複数スキャンでチェックするように変更(処理内履歴はなし) */
static SHORT	checkAreaSum( UCHAR *pTopAddress, LONG Length )
{
	SHORT	rc;					/* 戻り値			*/

	/* 変数初期化 */
	rc = CHKSUM_BUSY;

	switch( SysDl.CheckSumPhase & CHKSUM_SUB_PHASE_MASK )
	{
	case 0:	/* 初期化 */
		SysDl.CheckSumVal = 0;
		SysDl.CheckSumMem = pTopAddress;
		SysDl.CheckSumIdx = 0;
		SysDl.CheckSumPhase++;
		break;

	case 1:	/* 全域加算＆チェックサム確認 */
		while( SysDl.CheckSumIdx < Length )
		{
			SysDl.CheckSumVal += (USHORT)*SysDl.CheckSumMem;	/* 1バイトずつ加算		*/
			SysDl.CheckSumMem++;
			SysDl.CheckSumIdx++;
			/* 1ブロック分処理したら一旦終了 */
			if( !(SysDl.CheckSumIdx & CHKSUM_BLOCK_MASK) )
			{
				break;
			}
		}
		/* 最後まで到達したらサム値チェック */
		if( SysDl.CheckSumIdx >= Length )
		{
			/* チェックサム値加算	*/
			SysDl.CheckSumVal += *((USHORT *)SysDl.CheckSumMem);

			/* 結果判定 */
			if( SysDl.CheckSumVal == 0 )
			{
				rc = CHKSUM_OK;
			}
			else
			{
				rc = CHKSUM_NG;
			}
		}
		break;
	default:
		rc = CHKSUM_NG;
		break;
	}

	return rc;
}


/****************************************************************************************************/
/*																									*/
/*		受信バイナリのサム値チェック		 														*/
/*																									*/
/****************************************************************************************************/
/* <S182> フェーズ管理用に書き直しているため処理内履歴はなし */
/* <S1A3> 複数スキャンでチェックするように変更(処理内履歴はなし) */
static SHORT	checkSum( void )
{
	SHORT	rc;					/* 戻り値			*/
	UINT32	*pZipLocalHeader;	/* ZIPローカルヘッダへのポインタ	*/

	/* 変数初期化 */
	rc = CHKSUM_OK;

	/* 今回の各メインフェーズの処理を実行 */
	switch( SysDl.CheckSumPhase >> CHKSUM_MAIN_PHASE_SHFT )
	{
	case 0:
	/*------------------*/
	/* ミラー部チェック */
	/*------------------*/
		if( SysDl.MirSumCheckF == TRUE )
		{
			/* 工場出荷用簡易判定 */
			pZipLocalHeader = (UINT32 *)(FLS_MIRROR_EFW_ADDRESS + sizeof(FIRM_HEADER_T));
			if( (*pZipLocalHeader != ZIP_LOCAL_HEADER_SIGNATURE)
				&& ( *((UCHAR *)FLS_FACTORY_PASSWD_ADDRESS) != 0xFF) )
			{
				/* ZIPシグネチャが存在せず、パスワードが設定されていれば、工場出荷時よりチェック不要 */
			}
			else
			{
				/* ミラー部のサムチェック */
				rc = checkElfZipSum( (UCHAR *)FLS_MIRROR_EFW_ADDRESS );
			}
		}
		break;

	case 1:
	/*----------------------*/
	/* ランタイム部チェック */
	/*----------------------*/
		if( SysDl.RtmSumCheckF == TRUE )
		{
			/* ランタイム部のサムチェック */
			rc = checkElfZipSum( (UCHAR *)FLS_CODE_EFW_ADDRESS );
		}
		break;

	case 2:
	/*------------------*/
	/* ブート部チェック */
	/*------------------*/
		if( SysDl.BootSumCheckF == TRUE )
		{
			rc = checkAreaSum( (UCHAR *)FLS_BOOT_TOP_ADDRESS, FLS_BOOT_SUM_OFFSET );
		}
		break;

	case 3:
	/*--------------------------*/
	/* マイクロコード部チェック */
	/*--------------------------*/
		if( SysDl.MicroSumCheckF == TRUE )
		{
			rc = checkAreaSum( (UCHAR *)FLS_MICRO_CODE_ADDRESS, FLS_MICRO_SUM_OFFSET );
		}
		break;

	default:
	/*--------------*/
	/* 最終フェーズ */
	/*--------------*/
		SysDl.CheckSumPhase = CHKSUM_PHASE_END;
		break;
	}

	if( (rc == CHKSUM_OK) && (SysDl.CheckSumPhase != CHKSUM_PHASE_END) )
	{
		/* フェーズ更新 */
		SysDl.CheckSumPhase = (SysDl.CheckSumPhase + CHKSUM_MAIN_PHASE_ADD) & CHKSUM_MAIN_PHASE_MASK;
	}

	return rc;
}
/* <<<<< 暗号化フォーマット対応 <S146> */

/* <S1A2> >>>>> */
/****************************************************************************************************/
/*																									*/
/*		ワークのクリア						 														*/
/*																									*/
/****************************************************************************************************/
void MbusFuncSysDlInitWork( void )
{
	MlibResetByteMemory( &SysDl, sizeof(SysDl) );
	return;
}
/* <<<<< <S1A2> */
/***************************************** end of file **********************************************/
