/****************************************************************************************************/
/*																									*/
/*																									*/
/*		LcdPrmCopy.c : ＬＣＤオペレータ パラメータコピー機能										*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*	機 能 : ＬＣＤオペレータ内蔵EEPROMを使用して、													*/
/*			ユーザパラメータの保存(書込み)／復元(読出し)を行う。									*/
/*																									*/
/*			1)パラメータ読出し : LOAD-Command(0x61)を使用してパラメータの読出しを行う				*/
/*			2)パラメータ書込み : SAVE-Command(0x62)を使用してパラメータの書込みを行う				*/
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
/*		Rev.1.00 : 2003.06.25  S.Tokuhisa															*/
/*		Rev.1.10 : 2008.12.16  T.Taniguchi	Change PrmDef/Eeprom for LongPrm						*/
/*		Rev.1.20 : 2013.10.24  K.Ozaki		Change for Mercury										*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "KnlApi.h"
#include "LcdOp.h"
#include "LcdPdl.h"			/* Parameter Download */
#include "Information.h"	/* Information */
#include "Memory.h"			/* Trace Memory */

/****************************************************************************************************/
/*																									*/
/*		Macro Definition																			*/
/*																									*/
/****************************************************************************************************/
#define OBJ_READ( obj, idx, pdata ) ((obj)->Read( (obj), (idx), (pdata), MCHK_REG_READ, &LopMsg ))  /* <S073> */
#define OBJ_SEARCH( obj, num, pidx ) ((obj)->Search( (obj), (num), 0, (pidx), MCHK_REG_READ, &LopMsg ))

#undef MlibCnvLittle16
#define MlibCnvLittle16( x ) (x)					/* 後で消す */

/* TODO: 引数の仕様がSGDV,Jupiterと違う!! 不具合のもとになるので、本処理はNGです */
#define MlibCopyByteMemory( x, y, z )		MlibCopyByteMemory( y, x, z )  

#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W（２軸対応）のみ	*//* <S19B> */
#define Lpx_ChkDoubleBlock( typeid ) (0) /* とりあえず7Sと同じへ */
#elif CSW_BOARD_TYPE == SGD7S_TYPE	/*	SGD7S（単軸時）のみ	*/
#define Lpx_ChkDoubleBlock( typeid ) (( (typeid) & 0x00FF) == SVID_SGD7W_M3) /* TODO: M3以外のSGD7W追加 */
#endif/* <S19B> */

/****************************************************************************************************/
/*																									*/
/*		1. Define Const/Macro																		*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	[ 状態一覧 ]																					*/
/*																									*/
/*	------------------------------------------------------------------------------------------------*/
/*	メインモード(PrmCopyV.MainMode)																	*/
/*	------------------------------------------------------------------------------------------------*/
/*	PRMCOPY_MAINMODE_MODE_SELECT					機能選択 										*/
/*	PRMCOPY_MAINMODE_MODE_EXE						機能実行 										*/
/*		--------------------------------------------------------------------------------------------*/
/*		機能モード(PrmCopyV.SubMode)																*/
/*		--------------------------------------------------------------------------------------------*/
/*		PRMCOPY_SUBMODE_SAVE						SAVE(Servo -> DgOp)			 					*/
/*		PRMCOPY_SUBMODE_LOAD						LOAD(DgOp -> Servo) 							*/
/*		PRMCOPY_SUBMODE_VERIFY						VERIFY(Servo <-> DgOp)							*/
/*		PRMCOPY_SUBMODE_DELETE						DELETE(DgOp)	 								*/
/*			----------------------------------------------------------------------------------------*/
/*			各機能共通実行シーケンス(PrmCopyV.SubModeSeq)											*/
/*			----------------------------------------------------------------------------------------*/
/*			PRMCOPY_SUBMODE_SEQ_INIT				機能初期処理									*/
/*			PRMCOPY_SUBMODE_SEQ_BLOCK_SELECT		パラメータブロック選択							*/
/*			PRMCOPY_SUBMODE_SEQ_EXE_CONFIRM			実行開始確認 									*/
/*			PRMCOPY_SUBMODE_SEQ_EXE					実行 											*/
/*				------------------------------------------------------------------------------------*/
/*				実行シーケンス(PrmCopyV.ExeSeq)														*/
/*				------------------------------------------------------------------------------------*/
/*				----- SAVE(サーボパック -> デジオペ) or DELETE(デジオペ) ---------------------------*/
/*				PRMCOPY_SAVE_SEQ_INIT				実行初期処理									*/
/*				PRMCOPY_SAVE_SEQ_HEADER_DEL_REQ		ヘッダ情報削除コマンド要求						*/
/*				PRMCOPY_SAVE_SEQ_HEADER_DEL_RSP		ヘッダ情報削除コマンド応答確認					*/
/*				PRMCOPY_SAVE_SEQ_PRM_SEND			パラメータデータ伝送コマンド送信				*/
/*				PRMCOPY_SAVE_SEQ_PRM_RSP			パラメータデータ伝送コマンド応答確認			*/
/*				PRMCOPY_SAVE_SEQ_HEADER_SEND		ヘッダ情報登録コマンド送信						*/
/*				PRMCOPY_SAVE_SEQ_HEADER_RSP			ヘッダ情報登録コマンド応答確認					*/
/*				PRMCOPY_SAVE_SEQ_COMPLETE			実行完了画面表示中								*/
/*				PRMCOPY_SAVE_SEQ_CANCEL				中断画面表示中									*/
/*				------------------------------------------------------------------------------------*/
/*				----- LOAD(デジオペ -> サーボパック) or VERIFY(サーボパック <-> デジオペ) ----------*/
/*				PRMCOPY_LOAD_SEQ_INIT				実行初期処理									*/
/*				PRMCOPY_LOAD_SEQ_RECONFIRM			LOAD実行前再確認								*/
/*				PRMCOPY_LOAD_SEQ_PRM_READ_REQ		パラメータREADコマンド要求						*/
/*				PRMCOPY_LOAD_SEQ_PRM_READ_RSP		パラメータREADコマンド応答確認 & 処理実行		*/
/*				PRMCOPY_LOAD_SEQ_COMPLETE			実行完了画面表示中								*/
/*				PRMCOPY_LOAD_SEQ_CANCEL				中断画面表示中									*/
/*				PRMCOPY_VERIFY_SEQ_MATCHED			VERIFY結果 一致画面表示中						*/
/*				PRMCOPY_VERIFY_SEQ_UNMATCHED		VERIFY結果 不一致画面表示中						*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		メインモード(PrmCopyV.MainMode)																*/
/*--------------------------------------------------------------------------------------------------*/
#define	PRMCOPY_MAINMODE_MODE_SELECT		0	/* 機能選択 										*/
#define	PRMCOPY_MAINMODE_MODE_EXE			1	/* 機能実行 										*/
#define	PRMCOPY_MAINMODE_ERROR_BIT	   0x0100	/* エラー発生中ビット								*/
/*--------------------------------------------------------------------------------------------------*/
/*		機能実行モード(PrmCopyV.SubMode)															*/
/*--------------------------------------------------------------------------------------------------*/
#define	PRMCOPY_SUBMODE_SAVE				0	/* SAVE(Servo -> DgOp)			 					*/
#define	PRMCOPY_SUBMODE_LOAD				1	/* LOAD(DgOp -> Servo)			 					*/
#define	PRMCOPY_SUBMODE_VERIFY				2	/* VERIFY(サーボパック <-> デジオペ)				*/
#define	PRMCOPY_SUBMODE_DELETE				3	/* DELETE(デジオペ) 								*/
#define	PRMCOPY_SUBMODE_MAX					4	/*													*/
/*--------------------------------------------------------------------------------------------------*/
/*		機能実行シーケンス(PrmCopyV.SubModeSeq)														*/
/*--------------------------------------------------------------------------------------------------*/
#define PRMCOPY_SUBMODE_SEQ_INIT			0	/* 機能初期処理										*/
#define PRMCOPY_SUBMODE_SEQ_BLOCK_SELECT	1	/* ブロック選択 									*/
#define PRMCOPY_SUBMODE_SEQ_EXE_CONFIRM		2	/* 実行開始確認 									*/
#define PRMCOPY_SUBMODE_SEQ_EXE				3	/* 実行 											*/
/*--------------------------------------------------------------------------------------------------*/
/*		処理実行シーケンス(PrmCopyV.ExeSeq)															*/
/*--------------------------------------------------------------------------------------------------*/
/* SAVE(Servo -> DgOp) or DELETE(DgOp)処理								 							*/
#define	PRMCOPY_SAVE_SEQ_INIT				0	/* 実行初期処理										*/
#define	PRMCOPY_SAVE_SEQ_HEADER_DEL_REQ		1	/* ヘッダ情報削除コマンド要求						*/
#define PRMCOPY_SAVE_SEQ_HEADER_DEL_RSP		2	/* ヘッダ情報削除コマンド応答確認					*/
#define PRMCOPY_SAVE_SEQ_PRM_SEND			3	/* パラメータデータ伝送コマンド送信					*/
#define PRMCOPY_SAVE_SEQ_PRM_RSP			4	/* パラメータデータ伝送コマンド応答確認				*/
#define PRMCOPY_SAVE_SEQ_HEADER_SEND		5	/* ヘッダ情報登録コマンド送信						*/
#define PRMCOPY_SAVE_SEQ_HEADER_RSP			6	/* ヘッダ情報登録コマンド応答確認					*/
#define PRMCOPY_SAVE_SEQ_COMPLETE			7	/* 実行完了画面表示中								*/
#define PRMCOPY_SAVE_SEQ_CANCEL				8	/* 中断画面表示中									*/
/*--------------------------------------------------------------------------------------------------*/
#define PRMCOPY_SAVE_SEQ_PRM_READ_REQ		9	/* @@ パラメータデータEEPROM読出し					*/
#define PRMCOPY_SAVE_SEQ_PRM_READ_WAIT		10	/* @@ パラメータデータEEPROM読出し					*/
/*--------------------------------------------------------------------------------------------------*/
/* LOAD(DgOp -> Servo) or VERIFY(Servo <-> DgOp)処理					 							*/
#define PRMCOPY_LOAD_SEQ_INIT				0	/* 実行初期処理										*/
#define PRMCOPY_LOAD_SEQ_RECONFIRM			1	/* 実行前再確認										*/
#define PRMCOPY_LOAD_SEQ_PRM_READ_REQ		2	/* パラメータREADコマンド要求						*/
#define PRMCOPY_LOAD_SEQ_PRM_READ_RSP		3	/* パラメータREADコマンド応答確認					*/
#define PRMCOPY_LOAD_SEQ_COMPLETE			4	/* 実行完了画面表示中								*/
#define PRMCOPY_LOAD_SEQ_CANCEL				5	/* 中断画面表示中									*/
#define PRMCOPY_VERIFY_SEQ_MATCHED			6	/* VERIFY結果 一致画面表示中						*/
#define PRMCOPY_VERIFY_SEQ_UNMATCHED		7	/* VERIFY結果 不一致画面表示中						*/
#define PRMCOPY_LOAD_SEQ_STORE_REQ          8   /* @@ Parameter Store Request						*/
#define PRMCOPY_LOAD_SEQ_STORE_WAIT         9   /* @@ Parameter Store Wait							*/
#define PRMCOPY_LOAD_SEQ_FILL_REQ			10  /* @@ Fill Parameter Request						*/       
#define PRMCOPY_LOAD_SEQ_FILL_WAIT			11  /* @@ Fill Parameter Request						*/       
/* デジオペパラメータブロック管理情報取得					 										*/
#define PRMCOPY_INFOGET_SEQ_INIT			0	/* 実行初期処理										*/
#define PRMCOPY_INFOGET_SEQ_MNG_READ_REQ	1	/* パラメータ管理情報要求							*/
#define PRMCOPY_INFOGET_SEQ_MNG_READ_RSP	2	/* パラメータ管理情報取得							*/
#define PRMCOPY_INFOGET_SEQ_COMPLETE		3	/* 実行完了											*/
#define PRMCOPY_INFOGET_SEQ_CANCEL			4	/* 実行中断											*/
/* エラー画面表示中																					*/
#define PRMCOPY_ERROR_NONE					0	/* エラー表示なし									*/
#define PRMCOPY_ERROR_NOT_AVAILABLE			1	/* 実行不可エラー									*/
#define PRMCOPY_ERROR_NO_DATA				2	/* 選択データなしエラー								*/
#define PRMCOPY_ERROR_PRM_UNMATCH			3	/* パラメータ型式アンマッチエラー					*/
/****************************************************************************************************/
/*		デジオペ通信コマンド関係定義																*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		デジオペ通信コマンドコード																	*/
/*--------------------------------------------------------------------------------------------------*/
#define	LCD_RCVCMD_ALARM_BIT			0x80	/* デジオペ 受信コマンドアラームビット				*/
#define	LCD_RCVCMD_MASK					((UCHAR)(~LCD_RCVCMD_ALARM_BIT))	/* コマンド受信マスク	*/
/*--------------------------------------------------------------------------------------------------*/
/*		デジオペ通信インターフェース　項目別サイズ(Byte)											*/
/*--------------------------------------------------------------------------------------------------*/
/* ヘッダ部 */
#define LCD_MSG_AXIS_SIZE				1		/* 軸情報サイズ										*/
#define LCD_MSG_CMD_SIZE				1		/* コマンドコードサイズ								*/
#define LCD_MSG_LEN_SIZE				2		/* 送信バッファ長サイズ								*/
/* ヘッダ部トータルサイズ */
#define LCD_MSG_HEADER_SIZE				( LCD_MSG_AXIS_SIZE + LCD_MSG_CMD_SIZE + LCD_MSG_LEN_SIZE )
/* 表示データ部 */
#define LCD_MSG_LED_SIZE				2		/* LEDステータスサイズ								*/
#define LCD_MSG_DSP_SIZE				16		/* 表示ステータスサイズ								*/
#define LCD_MSG_CHR_SIZE				90		/* 表示文字サイズ(18カラム*5行)						*/
/* 表示データ部トータルサイズ */
#define LCD_MSG_DISPDATA_SIZE			( LCD_MSG_LED_SIZE + LCD_MSG_DSP_SIZE + LCD_MSG_CHR_SIZE )
/* デジオペEEPROMコマンド部 */
#define LCD_MSG_EEADR_SIZE				2		/* デジオペEEPROMアドレスサイズ						*/
#define LCD_MSG_DATANUM_SIZE			2		/* データ数サイズ									*/
/* チェックサム */
#define LCD_MSG_CHKSUM_SIZE				2		/* チェックサムサイズ								*/
#define LCD_MSG_OP_KEY_SIZE				2		/* キー情報サイズ(受信時のみ)						*/
#define LCD_MSG_DATA_UNIT_SIZE			2		/* １データサイズ(2Byte)							*/
#define LCD_MSG_DATA_GROUP_NUM			8		/* １回の通信で送受信できるデータ数(サム値を除く)	*/
/*--------------------------------------------------------------------------------------------------*/
/*		デジオペ通信インターフェース　コマンド別サイズ(Byte)										*/
/*--------------------------------------------------------------------------------------------------*/
/* 通常コマンド送信時(0x60)、データ長(サム値を除く) */
#define LCD_TX_NORMAL_TOTAL_SIZE	(LCD_MSG_HEADER_SIZE + LCD_MSG_DISPDATA_SIZE)
/* 通常コマンド受信時(0x60)、データ長 */
#define LCD_RX_NORMAL_TOTAL_SIZE	(LCD_MSG_HEADER_SIZE + LCD_MSG_OP_KEY_SIZE + LCD_MSG_CHKSUM_SIZE)
/*--------------------------------------------------------------------------------------------------*/
/*		デジオペ通信インターフェース　項目インデックス												*/
/*--------------------------------------------------------------------------------------------------*/
/* 送受信共通エリア  */
#define LCD_MSG_AXIS_IDX				0
#define LCD_MSG_CMD_IDX					( LCD_MSG_AXIS_IDX + LCD_MSG_AXIS_SIZE )
#define LCD_MSG_LEN_IDX					( LCD_MSG_CMD_IDX + LCD_MSG_CMD_SIZE )
/* 送信データ項目(Servo -> DgOp)インデックス */
#define LCD_TX_MSG_EEADR_IDX			LCD_TX_NORMAL_TOTAL_SIZE
#define LCD_TX_MSG_DATANUM_IDX			( LCD_TX_MSG_EEADR_IDX + LCD_MSG_EEADR_SIZE )
#define LCD_TX_MSG_DATA_IDX				( LCD_TX_MSG_DATANUM_IDX + LCD_MSG_DATANUM_SIZE )
#define LCD_TX_MSG_LOADSUM_IDX			LCD_TX_MSG_DATA_IDX
/* 受信データ項目(DgOp -> Servo)インデックス */
#define LCD_RX_MSG_DATANUM_IDX			( LCD_RX_NORMAL_TOTAL_SIZE - LCD_MSG_CHKSUM_SIZE )
#define LCD_RX_MSG_DATA_IDX				( LCD_RX_MSG_DATANUM_IDX + LCD_MSG_DATANUM_SIZE )
/****************************************************************************************************/
/*		リターンコード定義																			*/
/****************************************************************************************************/
#define	RC_RETRY_OVER					-2		/* リトライ回数オーバーフロー						*/
#define	RC_NG							-1		/* 異常												*/
#define	RC_OK							0		/* 正常												*/
#define	RC_PASS							1		/* なにも処理せず									*/
#define	RC_COMPLETE						2		/* 全処理完了										*/
/****************************************************************************************************/
/*		その他定義																					*/
/****************************************************************************************************/
#define	TIME_SECOND_2					2000	/* 2000ms = 2秒										*/
#define	TIME_500MSEC					500		/* 500ms											*/
#define	TIME_1000MSEC					1000	/* 1000ms											*/
#define	RETRY_CNT_MAX					3		/* デジオペ通信リトライ回数MAX						*/
#define LCD_DISP_LINE_CHAR_MAX			17		/* 画面表示カラムMAX								*/
#define PRMCOPY_DISP_TITLE_MAX			1		/* タイトル表示行MAX								*/
#define PRMCOPY_DISP_LIST_MAX			4		/* リスト表示行MAX(タイトル行を除く)				*/
#define PRMCOPY_DISP_LINE_MAX			(PRMCOPY_DISP_TITLE_MAX + PRMCOPY_DISP_LIST_MAX) /*表示行MAX*/
#define PRMCOPY_VERIFY_ERR_DISP_TBL_MAX	20		/* VERIFY不一致エラーリストテーブル件数MAX(5画面)	*/
#define	PRMCOPY_BLOCK_WORD_MAX			512		/* デジオペEEPROMのパラメータ１ブロックのワード数	*/
/* デジオペEEPROM パラメータ管理情報 */
#define	PRMCOPY_MNG_INFO_TOP0			0xE00	/* 領域０：先頭アドレス   (Word Addr)				*/
#define	PRMCOPY_MNG_INFO_SIZ0			0x003	/* 領域０：管理情報サイズ (Word Size)				*/
#define	PRMCOPY_MNG_INFO_TOP1			0xE20	/* 領域１：先頭アドレス   (Word Addr)				*/
#define	PRMCOPY_MNG_INFO_SIZ1			0x010	/* 領域１：管理情報サイズ (Word Size)				*/
#define	PRMCOPY_MNG_INFO_BLOCK_MAX		7		/* パラメータブロック数								*/
#define	PRMCOPY_MNG_DATA_NONE			((USHORT)0xFFFF)	/* パラメータ管理テーブル設定なしデータ	*/
/* パラメータ管理情報処理タイプ */
#define	PRMCOPY_MNG_INFODATA_CLEAR		0		/* クリア											*/
#define	PRMCOPY_MNG_INFODATA_SET		1		/* データセット										*/
#define	PRMCOPY_MNG_INFODATA_RECIEVE	2		/* 受信データ取得									*/
#define	LCD_DISP_LINE_1					0		/* LCDデジオペ 表示行 １							*/
#define	LCD_DISP_LINE_2					1		/* LCDデジオペ 表示行 ２							*/
#define	LCD_DISP_LINE_3					2		/* LCDデジオペ 表示行 ３							*/
#define	LCD_DISP_LINE_4					3		/* LCDデジオペ 表示行 ４							*/
#define	LCD_DISP_LINE_5					4		/* LCDデジオペ 表示行 ５							*/
/****************************************************************************************************/
/*		マクロ定義																					*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		デジオペEEPROMアドレス算出 : パラメータブロックNOとパラメータIndexNoより算出				*/
/*--------------------------------------------------------------------------------------------------*/
#define PRMCOPY_PRM_ADDR_GET(a,b)		(USHORT)((a * PRMCOPY_BLOCK_WORD_MAX) + b)
/*--------------------------------------------------------------------------------------------------*/
/*		受信バッファ：コマンドコード取得															*/
/*--------------------------------------------------------------------------------------------------*/
#define PRMCOPY_CMD_CODE_GET( )			( LopMsg.CmdBuf[LCD_MSG_CMD_IDX] )
/*--------------------------------------------------------------------------------------------------*/
/*		受信バッファ：受信データ取得																*/
/*--------------------------------------------------------------------------------------------------*/
#define PRMCOPY_CMD_DATA_GET( a )		MlibCnvLittle16( \
		*(USHORT *)&LopMsg.CmdBuf[ LCD_RX_MSG_DATA_IDX + (a * LCD_MSG_DATA_UNIT_SIZE) ] )
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		2. LCD Operator Message I/F																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		補足：#defineが複雑なので、構造体定義+#defineに変更する。(将来用)							*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		デジオペ通信コマンドコード																	*/
/*--------------------------------------------------------------------------------------------------*/
#define	LCD_CMD_ALMBIT		0x80				/* デジオペ 受信コマンドアラームビット				*/
#define	LCD_CMD_ALMMSK		(~LCD_CMD_ALMBIT)	/* デジオペ 受信コマンドアラームマスク				*/
/*--------------------------------------------------------------------------------------------------*/
/*		送信バッファデータ構造定義(現在使用中)														*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{								/* CmdCode : 0x62									*/
		UCHAR	AxisAdr;						/* 軸アドレス										*/
		UCHAR	CmdCode;						/* コマンドコード(0x61～0x64)						*/
		USHORT	Length;							/* 送信メッセージ長									*/
		USHORT	LedSts;							/* ＬＥＤ表示情報									*/
		UCHAR	DispSts[16];					/* 表示形式											*/
		UCHAR	DispLine[PRMCOPY_DISP_LINE_MAX][LCD_DISP_LINE_CHAR_MAX+1];	/* 画面表示情報			*/
		USHORT	EepAddr;						/* デジオペEEPROMアドレス(WORD)						*/
		USHORT	DataNum;						/* 送受信データ数(0～8)								*/
		USHORT	Data[LCD_MSG_DATA_GROUP_NUM];	/* 送受信データ										*/
		USHORT	ChkSum;							/* チェックサム										*/
	/*----------------------------------------------------------------------------------------------*/
		UCHAR	DispLineBlink[PRMCOPY_DISP_LINE_MAX];		/* Work : 画面表示ブリンク情報			*/
} PRM_COPY_TXBUF;
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		3. Structure Definition for Variables														*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		パラメータコピー管理用パラメータ															*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct	{
		USHORT	TypeID;							/* PrmCopyデータ製品識別ID							*/
		ADJUST_STRUCT_ALIGN( Dummy )			/* Adjust HEW Structure to Long Align				*/
} PRM_COPY_MNGP;
/*--------------------------------------------------------------------------------------------------*/
/*		パラメータコピー機能作業用変数																*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct	{
		USHORT	MainMode;						/* メインモード(機能選択 or 機能実行)				*/
		USHORT	SubMode;						/* 機能モード(SAVE ～ DELETE)						*/
		USHORT	SubModeSeq;						/* 各機能共通実行シーケンス(機能初期処理 ～ 実行)	*/
		USHORT	ExeSeq;							/* 実行シーケンス(機能別)							*/
	/*----------------------------------------------------------------------------------------------*/
		USHORT	BlockNo;						/* パラメータブロックNO								*/
		USHORT	ErrorNo;						/* エラーメッセージ画面NO							*/
		USHORT	RetryCnt;						/* デジオペ通信リトライ回数							*/
//		USHORT	UserChkSum;						/* ユーザーチェックサム計算用						*/
		USHORT	BlockMngAreaNo;					/* ブロック管理情報エリア番号(アクセス管理用)		*/
	/*----------------------------------------------------------------------------------------------*/
		ULONG	DispHoldTime;					/* ２秒間画面表示タイマー							*/
		ULONG	DispKeepTime;					/* 表示固定用タイマ									*/
	/*----------------------------------------------------------------------------------------------*/
		USHORT	PrmReadIndex;					/* パラメータReadインデックス						*/
		USHORT	PrmReadIndex_Last;				/* 前回パラメータReadインデックス					*/
		USHORT	PrmWriteIndex;					/* パラメータWriteインデックス						*/
		USHORT	PrmWriteIndex_Last;				/* 前回パラメータWriteインデックス					*/
	/*----------------------------------------------------------------------------------------------*/
//		USHORT	PrmIndex;						/* パラメータ検索インデックス(Bit0:LongL/H Flag)	*/
//		USHORT	PrmIndex_Last;					/* 前回パラメータ検索インデックス					*/
	/*----------------------------------------------------------------------------------------------*/
		USHORT	VerifyErrListNo;				/* VERIFY不一致リストNO								*/
		USHORT	VerifyErrTblCnt;				/* VERIFY不一致リストテーブル使用件数				*/
		USHORT	VerifyErrTotalCnt;				/* VERIFY不一致発生件数								*/
		USHORT	VerifyErrPrmIdxTbl[PRMCOPY_VERIFY_ERR_DISP_TBL_MAX]; /* VERIFY不一致リストテーブル	*/
	/*----------------------------------------------------------------------------------------------*/
		USHORT	*TraceBuffer;					/* @@ Pointer the Work Buffer						*/
		USHORT	VerifyNumber;					/* @@ Number of Parameters for Verify 				*/
		USHORT	Reserved1;						/* @@												*/
} PRM_COPY_INFO;
/*--------------------------------------------------------------------------------------------------*/
/*		ＬＣＤオペレータ内蔵ＥＥＰＲＯＭのパラメータブロック管理情報								*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct	{
		USHORT	ServoID;						/* サーボパック製品ＩＤ								*/
		USHORT	AmpWatt;						/* サーボパック容量[Watt]							*/
		USHORT	AmpVolt;						/* サーボパック出力電圧コード						*/
		USHORT	FormatID;						/* パラメータフォーマットＩＤ　						*/
} PRM_MNGB_INFO;
/*--------------------------------------------------------------------------------------------------*/
typedef struct	{
		USHORT	SvPrm;							/* サーボユーザ定数登録数							*/
//		USHORT	UsrPrm;							/* オプションユーザ定数登録数						*/
} PRM_ENTNUM_INFO;
/*--------------------------------------------------------------------------------------------------*/
/*		PRM_COPY_INFO.PrmIndex Operation Macro														*/
/*--------------------------------------------------------------------------------------------------*/
/*		PrmIndex :   0 bit : LongPrm LowWord/HighWord Flag											*/
/*		PrmIndex : F-1 bit : All Parameter Manage Table PnPrmTbl[] Index							*/
/*--------------------------------------------------------------------------------------------------*/
#define LPX_GET_HiWdFlag( Idx )		((Idx) & 0x01)
//#define LPX_GET_PrmDefPtr( Idx )	(PnPrmTbl[((Idx)>>1)].PrmDefPtr)
#define LPX_INC_SrhIndex( Idx )		((Idx) += 2)
//#define	LPX_CHK_VarIsLong( Idx )	(PnPrmTbl[((Idx)>>1)].PrmDefPtr->Attr.VarIsLong)
//#define LPX_INC_PrmIndex( Idx )		if( LPX_CHK_VarIsLong( Idx ) ){ (Idx) += 1;}else{ (Idx) += 2;}
/*--------------------------------------------------------------------------------------------------*/
/*		Long Parameter Operation Macro for Big/Little Endian										*/
/*--------------------------------------------------------------------------------------------------*/
/*		Macro Argument     :   lwd    hwd															*/
/*		LongPrm Word Align : {LoNum, HiNum} : ULONGOF_REGDATA( (hwd<<16) + lwd ) <-- SGDV			*/
/*		LongPrm Word Align : {HiNum, LoNum} : ULONGOF_REGDATA( (lwd<<16) + hwd ) <-- ????			*/
/*--------------------------------------------------------------------------------------------------*/
#if		CCSW_BIG_ENDIAN == 1
#define GET_LWD_EEPDATA( xlong )	(USHORT)((xlong)>>0 )
#define GET_HWD_EEPDATA( xlong )	(USHORT)((xlong)>>16)
#define ULONGOF_EEPDATA( lwd, hwd )	(ULONG)((((USHORT)(hwd))<<16) + (USHORT)(lwd))
#else
#define GET_LWD_EEPDATA( xlong )	(USHORT)((xlong)>>0 )
#define GET_HWD_EEPDATA( xlong )	(USHORT)((xlong)>>16)
#define ULONGOF_EEPDATA( lwd, hwd )	(ULONG)((((USHORT)(hwd))<<16) + (USHORT)(lwd))
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		Check EEPROM Parameter																		*/
/*--------------------------------------------------------------------------------------------------*/
#define LPX_CHK_EEP_PRM( pDef )		( (pDef->Attr.Place == PRMDEF_RAM_EEPROM) && \
									  (pDef->RamPtr != NULL) && (pDef->Attr.AliasPrm == FALSE) )
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		5. Const Data Definition																	*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		サーボ機種ＩＤテーブル(表示文字列検索テーブル)												*/
/*--------------------------------------------------------------------------------------------------*/
static	const	SVIDTBL			SvIdTbl[] = { SVIDTBL_ENTRY_DEF };
#define	SVIDTBL_ENTRY_NUM		(sizeof(SvIdTbl)/sizeof(SvIdTbl[0]))
/*--------------------------------------------------------------------------------------------------*/
/*		アンプ容量表示文字列検索テーブル															*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct
{
	USHORT 	AmpVolt;
	USHORT	AmpWatt;
    UCHAR 	DispStr[10];
	UCHAR	DispStrW[10];	
} AMP_WATT_TBL;
/*--------------------------------------------------------------------------------------------------*/
const	AMP_WATT_TBL	AmpWattTbl[] = {
	{	0,	30,		"1R0",	"A3"	},		/*   30 W		100V	*/
/*--------------------------------------------------------------------------------------------------*/
	{	1,	50,		"R70",	"A5"	},		/*   50 W		200V	*/
	{	1,	100,	"R90",	"01"	},		/*  100 W				*/
	{	1,	200,	"1R6",	"02"	},		/*  200 W				*/
	{	1,	400,	"2R8",	"04"	},		/*  400 W				*/
	{	1,	500,	"3R8",	"05"	},		/*  500 W				*/
	{	1,	750,	"5R5",	"08"	},		/*  750 W				*/
	{	1,	1000,	"7R6",	"10"	},		/*  1.0 kW				*/
	{	1,	1500,	"120",	"15"	},		/*  1.5 kW				*/
	{	1,	2000,	"180",	"20"	},		/*  2.0 kW				*/
	{	1,	3000,	"200",	"30"	},		/*  3.0 kW				*/
	{	1,	5000,	"330",	"50"	},		/*  5.0 kW				*/
	{	1,	6000,	"470",	"60"	},		/*  6.0 kW				*/
	{	1,	7500,	"550",	"75"	},		/*  7.5 kW				*/
	{	1,	11000,	"590",	"1A"	},		/* 11.0 kW				*/
	{	1,	15000,	"780",	"1E"	},		/* 15.0 kW				*/
/*--------------------------------------------------------------------------------------------------*/
	{	2,	500,	"1R9",	"05"	},		/*  500 W		400V	*/
	{	2,	1000,	"3R5",	"10"	},		/*  1.0 kW				*/
	{	2,	1500,	"5R4",	"15"	},		/*  1.5 kW				*/
	{	2,	2000,	"8R4",	"20"	},		/*  2.0 kW				*/
	{	2,	3000,	"120",	"30"	},		/*  3.0 kW				*/
	{	2,	5000,	"170",	"50"	},		/*  5.0 kW				*/
	{	2,	6000,	"210",	"60"	},		/*  6.0 kW				*/
	{	2,	7500,	"250",	"75"	},		/*  7.5 kW				*/
	{	2,	11000,	"280",	"1A"	},		/* 11.0 kW				*/
	{	2,	15000,	"370",	"1E"	},		/* 15.0 kW				*/
/*--------------------------------------------------------------------------------------------------*/
	{	5,	50,		"R70",	"A5"	},		/*   50 W		倍電圧	*/
	{	5,	100,	"R90",	"01"	},		/*  100 W				*/
	{	5,	200,	"1R6",	"02"	},		/*  200 W				*/
	{	5,	400,	"2R8",	"04"	},		/*  400 W				*/
};
/*--------------------------------------------------------------------------------------------------*/
#define	AMP_WATT_TBL_IDX_MAX	(sizeof(AmpWattTbl)/sizeof(AMP_WATT_TBL))
/*--------------------------------------------------------------------------------------------------*/
/*		入力電圧表示文字列検索テーブル																*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct
{
	USHORT 	Code;
	UCHAR 	DispStr[10];
} AMP_VOLT_TBL;
/*--------------------------------------------------------------------------------------------------*/
const	AMP_VOLT_TBL	AmpVoltTbl[] =
{
	{	0,		"B"		},		/* AC 100V 				*/
	{	1,		"A"		},		/* AC 200V 				*/
	{	2,		"D"		},		/* AC 400V 				*/
	{	5,		"F"		},		/* 倍電圧(AC 100V x 2)	*/
};
/*--------------------------------------------------------------------------------------------------*/
#define	AMP_VOLT_TBL_IDX_MAX	(sizeof(AmpVoltTbl)/sizeof(AMP_VOLT_TBL))
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		5. Const String Data Definition																*/
/*																									*/
/****************************************************************************************************/
/****************************************************************************************************/
/*		画面タイトル(画面右上に表示されるモード別のタイトル)										*/
/****************************************************************************************************/
#if 0		/* <S154> */
const UCHAR PrmCopyDispTitleTbl[][LCD_DISP_LINE_CHAR_MAX+1] =
{
	"           -COPY-",			/* [00] : メインタイトル	PRMCOPY_DISPTITLE_MAIN				*/
	"       -SERVO\176OP-",			/* [01] : SAVEタイトル		PRMCOPY_DISPTITLE_SAVE				*/
	"       -OP\176SERVO-",			/* [02] : LOADタイトル		PRMCOPY_DISPTITLE_LOAD				*/
	"         -VERIFY-",			/* [03] : VERIFYタイトル	PRMCOPY_DISPTITLE_VERIFY			*/
	"           -LIST-",			/* [04] : DELETEタイトル	PRMCOPY_DISPTITLE_DELETE			*/
};
#else		/* <S154> */
const UCHAR PrmCopyDispTitleTbl[][LCD_DISP_LINE_CHAR_MAX+1] =
{
	"             COPY",			/* [00] : メインタイトル	PRMCOPY_DISPTITLE_MAIN				*/
	"         SERVO\176OP",			/* [01] : SAVEタイトル		PRMCOPY_DISPTITLE_SAVE				*/
	"         OP\176SERVO",			/* [02] : LOADタイトル		PRMCOPY_DISPTITLE_LOAD				*/
	"           VERIFY",			/* [03] : VERIFYタイトル	PRMCOPY_DISPTITLE_VERIFY			*/
	"             LIST",			/* [04] : DELETEタイトル	PRMCOPY_DISPTITLE_DELETE			*/
};
#endif
/*--------------------------------------------------------------------------------------------------*/
enum PrmCopyDispTitleTblIndex		/* タイトル表示文字列テーブル　インデックスＮＯ定義				*/
{
	PRMCOPY_DISPTITLE_MAIN = 0,
	PRMCOPY_DISPTITLE_SAVE,
	PRMCOPY_DISPTITLE_LOAD,
	PRMCOPY_DISPTITLE_VERIFY,
	PRMCOPY_DISPTITLE_DELETE,
};
/****************************************************************************************************/
/*		機能選択画面(機能一覧)																		*/
/****************************************************************************************************/
const UCHAR PrmCopyDispModeStrTbl[][LCD_DISP_LINE_CHAR_MAX+1] =
{
	"1:SERVO\176OP",				/* [00] : SAVE 				PRMCOPY_DISPMODE_SAVE				*/
	"2:OP\176SERVO",				/* [01] : LOAD				PRMCOPY_DISPMODE_LOAD				*/
	"3:VERIFY",						/* [02] : VERIFY			PRMCOPY_DISPMODE_VERIFY				*/
	"4:LIST",						/* [03] : DELETE			PRMCOPY_DISPMODE_DELETE				*/
};
/*--------------------------------------------------------------------------------------------------*/
enum PrmCopyDispModeTblIndex		/* 機能一覧表示文字列テーブル　インデックスＮＯ定義				*/
{
	PRMCOPY_DISPMODE_SAVE = 0,
	PRMCOPY_DISPMODE_LOAD,
	PRMCOPY_DISPMODE_VERIFY,
	PRMCOPY_DISPMODE_DELETE,
};
/****************************************************************************************************/
/*		パラメータブロック選択画面(表示番号)														*/
/****************************************************************************************************/
const UCHAR PrmCopyDispBlockStrTbl[][4] =
{
	"00:",							/* [00] : PRMCOPY_DISPBLOCK_NO_00								*/
	"01:",							/* [01] : PRMCOPY_DISPBLOCK_NO_01								*/
	"02:",							/* [02] : PRMCOPY_DISPBLOCK_NO_02								*/
	"03:",							/* [03] : PRMCOPY_DISPBLOCK_NO_03								*/
	"04:",							/* [04] : PRMCOPY_DISPBLOCK_NO_04								*/
	"05:",							/* [05] : PRMCOPY_DISPBLOCK_NO_05								*/
	"06:",							/* [06] : PRMCOPY_DISPBLOCK_NO_06								*/
};
/*--------------------------------------------------------------------------------------------------*/
enum PrmCopyDispBlockTblIndex		/* パラメータブロック表示No文字列テーブル　インデックスNo定義	*/
{
	PRMCOPY_DISPBLOCK_NO_00 = 0,
	PRMCOPY_DISPBLOCK_NO_01,
	PRMCOPY_DISPBLOCK_NO_02,
	PRMCOPY_DISPBLOCK_NO_03,
	PRMCOPY_DISPBLOCK_NO_04,
	PRMCOPY_DISPBLOCK_NO_05,
	PRMCOPY_DISPBLOCK_NO_06,
};
/****************************************************************************************************/
/*		その他表示文字列																			*/
/****************************************************************************************************/
const UCHAR PrmCopyDispStrTbl[][LCD_DISP_LINE_CHAR_MAX+1] =
{
	"                 ",			/* [00]:PRMCOPY_DISPSTR_SPACE 				*/
	"  Start :[WRITE]",				/* [01]:PRMCOPY_DISPSTR_START_WRITE			*/
	"  Start :[READ]",				/* [02]:PRMCOPY_DISPSTR_START_READ			*/
	"  Return:[MODE]",				/* [03]:PRMCOPY_DISPSTR_RETURN_MODE			*/
	"   Reading",					/* [04]:PRMCOPY_DISPSTR_READING				*/
	"   Writing",					/* [05]:PRMCOPY_DISPSTR_WRITING				*/
	"   Verifying",					/* [06]:PRMCOPY_DISPSTR_VERIFYING			*/
	"   Deleting",					/* [07]:PRMCOPY_DISPSTR_DELETING			*/
	"    Parameters..",				/* [08]:PRMCOPY_DISPSTR_PARAMETERS_DOT		*/
	"   Completed",					/* [09]:PRMCOPY_DISPSTR_COMPLETED			*/
	"    Canceled",					/* [10]:PRMCOPY_DISPSTR_CANCELED			*/
	"   Unmatched",					/* [11]:PRMCOPY_DISPSTR_UNMATCHED_RIGHT		*/
	"    Parameters",				/* [12]:PRMCOPY_DISPSTR_PARAMETERS_RIGHT	*/
	"   No Data",					/* [13]:PRMCOPY_DISPSTR_NO_DATA				*/
	"  Not Available",				/* [14]:PRMCOPY_DISPSTR_NOT_AVAILABLE		*/
	"****",							/* [15]:PRMCOPY_DISPSTR_ASTER				*/
	"-",							/* [16]:PRMCOPY_DISPSTR_HIFUN				*/
	" All Parameters",				/* [17]:PRMCOPY_DISPSTR_ALL_PARAMETERS		*/
	"  are matched",				/* [18]:PRMCOPY_DISPSTR_ARE_MATCHED			*/
	"Unmatched",					/* [19]:PRMCOPY_DISPSTR_UNMATCHED_LEFT		*/
	"Parameters",					/* [20]:PRMCOPY_DISPSTR_PARAMETERS_LEFT		*/
	"<<SERVO>>",					/* [21]:PRMCOPY_DISPSTR_SERVO				*/
	"<< OP >>",						/* [22]:PRMCOPY_DISPSTR_OP					*/
	"<<OP-SERVO>>",					/* [23]:PRMCOPY_DISPSTR_OP_SERVO			*/
	"??",							/* [24]:PRMCOPY_DISPSTR_QUESTION_2			*/
	"A",							/* [25]:PRMCOPY_DISPSTR_A					*/
	"F",							/* [26]:PRMCOPY_DISPSTR_F					*/
	"?",							/* [27]:PRMCOPY_DISPSTR_QUESTION_1			*/
	"  FILE DELETE    ",			/* [28]:PRMCOPY_DISPSTR_FILE_DELETE			*/
	"Pn",							/* [29]:PRMCOPY_DISPSTR_PN					*/
	"To Many Errors",				/* [30]:PRMCOPY_DISPSTR_TO_MANY_ERRORS		*/
	"File List Loading",			/* [31]:PRMCOPY_DISPSTR_FILE_LIST_LOADING	*/
	"  Please Wait...",				/* [32]:PRMCOPY_DISPSTR_PLEASE_WAIT			*/
	"  Reconfirm",					/* [33]:PRMCOPY_DISPSTR_RECONFIRM			*/
	"   OK :[DATA]",				/* [34]:PRMCOPY_DISPSTR_OK_DATA				*/
	"   NO :[MODE]",				/* [35]:PRMCOPY_DISPSTR_NO_MODE				*/
	"????",							/* [36]:PRMCOPY_DISPSTR_QUESTION_4			*/
	"???",							/* [37]:PRMCOPY_DISPSTR_QUESTION_3			*/
};
/*--------------------------------------------------------------------------------------------------*/
enum PrmCopyDispStrTblIndex			/* 表示文字列テーブル　インデックスＮＯ定義	*/
{
	PRMCOPY_DISPSTR_SPACE = 0,
	PRMCOPY_DISPSTR_START_WRITE,
	PRMCOPY_DISPSTR_START_READ,
	PRMCOPY_DISPSTR_RETURN_MODE,
	PRMCOPY_DISPSTR_READING,
	PRMCOPY_DISPSTR_WRITING,
	PRMCOPY_DISPSTR_VERIFYING,
	PRMCOPY_DISPSTR_DELETING,
	PRMCOPY_DISPSTR_PARAMETERS_DOT,
	PRMCOPY_DISPSTR_COMPLETED,
	PRMCOPY_DISPSTR_CANCELED,
	PRMCOPY_DISPSTR_UNMATCHED_RIGHT,
	PRMCOPY_DISPSTR_PARAMETERS_RIGHT,
	PRMCOPY_DISPSTR_NO_DATA,
	PRMCOPY_DISPSTR_NOT_AVAILABLE,
	PRMCOPY_DISPSTR_ASTER,
	PRMCOPY_DISPSTR_HIFUN,
	PRMCOPY_DISPSTR_ALL_PARAMETERS,
	PRMCOPY_DISPSTR_ARE_MATCHED,
	PRMCOPY_DISPSTR_UNMATCHED_LEFT,
	PRMCOPY_DISPSTR_PARAMETERS_LEFT,
	PRMCOPY_DISPSTR_SERVO,
	PRMCOPY_DISPSTR_OP,
	PRMCOPY_DISPSTR_OP_SERVO,
	PRMCOPY_DISPSTR_QUESTION_2,
	PRMCOPY_DISPSTR_A,
	PRMCOPY_DISPSTR_F,
	PRMCOPY_DISPSTR_QUESTION_1,
	PRMCOPY_DISPSTR_FILE_DELETE,
	PRMCOPY_DISPSTR_PN,
	PRMCOPY_DISPSTR_TO_MANY_ERRORS,
	PRMCOPY_DISPSTR_FILE_LIST_LOADING,
	PRMCOPY_DISPSTR_PLEASE_WAIT,
	PRMCOPY_DISPSTR_RECONFIRM,
	PRMCOPY_DISPSTR_OK_DATA,
	PRMCOPY_DISPSTR_NO_MODE,
	PRMCOPY_DISPSTR_QUESTION_4,
	PRMCOPY_DISPSTR_QUESTION_3,
};
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		6. Variables Definition																		*/
/*																									*/
/****************************************************************************************************/
//#pragma section sidata begin
PRM_COPY_MNGP	PrmCopyP;
PRM_COPY_INFO	PrmCopyV;
PRM_MNGB_INFO	PrmMngTbl[PRMCOPY_MNG_INFO_BLOCK_MAX];
PRM_ENTNUM_INFO	PrmEntNum;
extern MSGIF	LopMsg;							/* ＬＣＤオペレータ：メッセージＩＦデータ			*/
//#pragma section sidata end
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		7. Functions Prototype List																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	[ 関数構成 ]																					*/
/*																									*/
/*	RpxLcdPrmCopyMain()							パラメータコピーメイン								*/
/*	|																								*/
/*	|  PrmCopyInit()							初期処理											*/
/*	|  |  PrmCopyCmdInit()						デジオペ通信初期処理								*/
/*	|  |  PrmCopyNormalCmdSet()					デフォルトで通常通信コマンドセット					*/
/*	|  |  |																							*/
/*	|  PrmCopyOpMain()							キー操作メイン処理									*/
/*	|  |  PrmCopyModeSelect()					機能選択											*/
/*	|  |  |  RpxLcdPrmCopyInit() 				初期化処理(構造体変数全クリア等、初回のみ)			*/
/*	|  |  |  PrmCopyInit()						初期処理(通信バッファクリア等、毎回)				*/
/*	|  |  |  PrmCopyModeSelectCheck()			機能選択チェック処理			 					*/
/*	|  |  PrmCopyModeExe()						機能実行											*/
/*	|  |  |  PrmCopyBlockDispListMake() 		ブロック一覧表示テーブル作成処理					*/
/*	|  |  |  PrmCopyBlockSelect()				ブロック選択処理				 					*/
/*	|  |  |  |	PrmCopyBlockSelectCheck()		ブロック選択チェック処理							*/
/*	|  |  |  |	PrmCopyBlockTypeCheck()			ブロックタイプチェック処理							*/
/*	|  |  |  PrmCopyExeConfirm()				実行開始確認処理									*/
/*	|  |  |  PrmCopySaveKeyOp()					SAVE(Servo -> DgOp)中 Key処理						*/
/*	|  |  |  PrmCopySaveOrDeleteExe()			SAVE OR DELETE(DgOp)実行							*/
/*	|  |  |  |	PrmCopyPrmMngTblOp()			デジオペ パラメータブロック管理情報編集処理			*/
/*	|  |  |  |	PrmCopySaveCmdSet()				通信作業領域にSAVEコマンドセット					*/
/*	|  |  |  |	PrmCopyCmdRspCheck()			デジオペ通信コマンド応答確認処理					*/
/*	|  |  |  |	PrmCopyUsrPrmSearch()			ユーザーパラメータ検索								*/
/*	|  |  |  |	KpiRstLongTimer()				タイマーリセット									*/
/*	|  |  |  |	KpiGetLongTimer()				タイマー値取得										*/
/*	|  |  |  PrmCopyLoadKeyOp()					LOAD(DgOp -> Servo)中 Key処理						*/
/*	|  |  |  PrmCopyVerifyKeyOp()				VERIFY(Servo <-> DgOp)中 Key処理					*/
/*	|  |  |  PrmCopyLoadOrVerifyExe()			LOAD OR VERIFY実行									*/
/*	|  |  |  |  PrmCopyLoadCmdSet()				通信作業領域にLOADコマンドセット					*/
/*	|  |  |  |  PrmCopyCmdRspCheck()			デジオペ通信コマンド応答確認処理					*/
/*	|  |  |  |  PrmCopyLoadExePrmWrite()		１回受信分(最大８個)のパラメータを					*/
/*	|  |  |  |  |									サーボパックのEEPROMへ書き込む処理				*/
/*	|  |  |  |  PrmCopyVerifyExePrmVerify()		１回受信分(最大８個)のパラメータと					*/
/*	|  |  |  |  |									サーボパックのパラメータ(EEPROM)とVERIFY		*/
/*	|  |  |  |  |  PrmCopyUsrPrmSearch()		ユーザーパラメータIndex取得							*/
/*	|  |  |  |  KpiRstLongTimer()				タイマーリセット									*/
/*	|  |  |  |  KpiGetLongTimer()				タイマー値取得										*/
/*	|  |  PrmCopyErrorOp()						エラー発生中処理									*/
/*	|  |  |																							*/
/*	|  PrmCopyDispMain()						画面表示データ作成メイン処理						*/
/*	|  |  PrmCopyDispModeSelect()				機能選択画面　						表示データ作成	*/
/*	|  |  PrmCopyDispBlockSelect()				ブロック選択画面　					表示データ作成	*/
/*	|  |  PrmCopyDispExeConfirm()				実行開始確認画面　					表示データ作成	*/
/*	|  |  PrmCopyDispSaveExe()					SAVE(Servo -> DgOp)実行中画面		表示データ作成	*/
/*	|  |  PrmCopyDispLoadExe()					LOAD(DgOp -> Servo)実行中画面		表示データ作成	*/
/*	|  |  PrmCopyDispVerifyExe()				VERIFY(Servo <-> DgOp)実行中画面	表示データ作成	*/
/*	|  |  |  PrmCopyDispVerifyUnmatchList()		VERIFY結果 不一致画面　				表示データ作成	*/
/*	|  |  PrmCopyDispDeleteExe()				DELETE実行中画面　					表示データ作成	*/
/*	|  |  PrmCopyDispError()					エラーメッセージ画面　				表示データ作成	*/
/*	|  |  |																							*/
/*	|  PrmCopySetSendBuf()						通信作業領域から、送信バッファへデータセット		*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	[ 関数一覧 ]																					*/
/*																									*/
/*		-------------------------------------------------------------------------------------------	*/
/*		RpxLcdPrmCopyMain()				パラメータコピー機能メイン									*/
/*		-------------------------------------------------------------------------------------------	*/
/*		PrmCopyOpInit()					初期処理													*/
/*		PrmCopyOpMain()					キー操作メイン処理											*/
/*		PrmCopyModeSelect()				機能選択													*/
/*		PrmCopyModeExe()				機能実行													*/
/*		PrmCopyBlockSelect()			パラメータブロック選択										*/
/*		PrmCopyBlockSelectCheck()		ブロック選択チェック処理									*/
/*		PrmCopyBlockTypeCheck()			ブロックタイプチェック処理									*/
/*		PrmCopyExeConfirm()				実行開始確認処理											*/
/*		PrmCopySaveOrDeleteExe()		SAVE(Servo -> DgOp) or DELETE(DgOp)実行						*/
/*		PrmCopyLoadOrVerifyExe()		LOAD(DgOp -> Servo)実行										*/
/*		PrmCopySaveKeyOp()				SAVE(Servo -> DgOp)中  Key処理								*/
/*		PrmCopyLoadKeyOp()				LOAD(DgOp -> Servo)中  Key処理								*/
/*		PrmCopyVerifyKeyOp()			VERIFY(Servo <-> DgOp)中  Key処理							*/
/*		PrmCopyLoadExePrmWrite()		１回受信分(最大８個)のパラメータを							*/
/*												サーボパックのEEPROMへ書き込む						*/
/*		PrmCopyVerifyExePrmVerify()		１回受信分(最大８個のパラメータと							*/
/*												サーボパックのパラメータ(EEPROM)とのVERIFY			*/
/*		PrmCopyModeSelectCheck()		機能選択チェック処理										*/
/*		PrmCopyErrorOp()				エラー発生中処理											*/
/*		PrmCopyUsrPrmSearch()			ユーザーパラメータ検索処理									*/
/*		ConvertHexToStr()				HEXから文字列への変換処理									*/
/*		-------------------------------------------------------------------------------------------	*/
/*		PrmCopyDispMain()				画面表示データ作成処理メイン								*/
/*		PrmCopyDispError()				エラーメッセージ画面　				表示データ作成			*/
/*		PrmCopyDispModeSelect()			機能選択画面　						表示データ作成			*/
/*		PrmCopyDispBlockSelect()		ブロック選択画面　					表示データ作成			*/
/*		PrmCopyDispExeConfirm()			実行開始確認画面　					表示データ作成			*/
/*		PrmCopyDispSaveExe()			SAVE(Servo -> DgOp)実行中画面　		表示データ作成			*/
/*		PrmCopyDispLoadExe()			LOAD(DgOp -> Servo)実行中画面　		表示データ作成			*/
/*		PrmCopyDispVerifyExe()			VERIFY(Servo <-> DgOp)実行中画面	表示データ作成			*/
/*		PrmCopyDispVerifyUnmatchList()	VERIFY結果 不一致画面				表示データ作成			*/
/*		PrmCopyDispDeleteExe()			DELETE(DgOp)実行中画面				表示データ作成			*/
/*		PrmCopyDispBlockHeader()		ブロック選択後画面					ヘッダー作成			*/
/*		PrmCopyMakeBlockDispLine()		ブロック表示データ作成処理			表示データ作成			*/
/*		PrmCopyDispInit()				デジオペ初期画面					表示データ作成			*/
/*		PrmCopyStrCopy()				文字列コピー												*/
/*		PrmCopyStrCopy2()				文字列コピー(コピー先のアドレス更新後の値を返す)			*/
/*		-------------------------------------------------------------------------------------------	*/
/*		RpxLcdPrmCopyInit()				パラメータコピー機能初期化									*/
/*		RpxLcdPrmInfoGet()				デジオペパラメータブロック管理情報取得						*/
/*		PrmCopyCmdRspCheck()			デジオペ通信コマンド応答確認								*/
/*		PrmCopyPrmMngTblOp()			デジオペパラメータ管理情報操作								*/
/*		PrmCopyCmdInit()				デジオペ通信  初期化										*/
/*		PrmCopyNormalCmdSet()			デジオペ通信  通常コマンド(0x60)作業領域セット				*/
/*		PrmCopyLoadCmdSet()				デジオペ通信  LOADコマンド(0x61)作業領域セット				*/
/*		PrmCopySaveCmdSet()				デジオペ通信  SAVEコマンド(0x62)作業領域セット				*/
/*		PrmCopySetSendBuf()				送信バッファセット処理										*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		ＬＣＤパラメータコピーＩＦ関数																*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxLcdPrmCopyInit( void );			/* パラメータコピー機能初期化処理						*/
LONG	RpxLcdPrmInfoGet( void );			/* パラメータブロック情報取得処理						*/
//LONG	RpxLcdPrmCopyMain( void );			/* パラメータコピー機能メイン処理						*/
void	RpxLcdPrmCopyMain( MSGIF *Hmsg, DRVSEL *pDrvSel );	/* パラメータコピー機能メイン処理		*/
/*--------------------------------------------------------------------------------------------------*/
/*		ＬＣＤパラメータコピーローカル関数															*/
/*--------------------------------------------------------------------------------------------------*/
void	PrmCopyInit(void);
LONG	PrmCopyOpMain(void);
LONG	PrmCopyModeSelect(void);
void	PrmCopyModeExe(void);
void	PrmCopyBlockSelect(void);
void	PrmCopyExeConfirm(void);
void	PrmCopyLoadOrVerifyExe(void);
void	PrmCopySaveOrDeleteExe(void);
SHORT	PrmCopyLoadExePrmWrite(void);
SHORT	PrmCopyVerifyExePrmVerify(void);
void	PrmCopySaveKeyOp(void);
void	PrmCopyLoadKeyOp(void);
void	PrmCopyVerifyKeyOp(void);
void	PrmCopyErrorOp(void);
SHORT	PrmCopyModeSelectCheck(void);
void	PrmCopyDispMain(void);
void	PrmCopyDispError(void);
void	PrmCopyDispModeSelect(void);
void	PrmCopyDispBlockSelectMain(void);
void	PrmCopyDispBlockSelect(void);
void	PrmCopyDispExeConfirm(void);
void	PrmCopyDispSaveExe(void);
void	PrmCopyDispLoadExe(void);
void	PrmCopyDispVerifyExe(void);
void	PrmCopyDispVerifyUnmatchList(void);
void	PrmCopyDispDeleteExe(void);
void	PrmCopyDispBlockHeader(void);
void	PrmCopyMakeBlockDispLine( UCHAR *, LONG );
void	PrmCopyDispInit(void);
/*--------------------------------------------------------------------------------------------------*/
SHORT	PrmCopyBlockSelectCheck(void);
LONG	PrmCopyBlockTypeCheck( LONG BlockNo );
void	PrmCopyCmdInit(void);
SHORT	PrmCopyCmdRspCheck( USHORT );
void	PrmCopyPrmMngTblOp( UCHAR, USHORT );
void	PrmCopyStrCopy( UCHAR *, UCHAR * );
void	PrmCopyStrCopy2( UCHAR **, UCHAR * );
//SHORT	PrmCopyUsrPrmSearch( USHORT *, const PRMTBL *, const USHORT );
void	ConvertHexToStr( USHORT, UCHAR *, USHORT );
/*--------------------------------------------------------------------------------------------------*/
void	PrmCopyNormalCmdSet( void );
void	PrmCopyLoadCmdSet( USHORT, USHORT );
void	PrmCopySaveCmdSet( USHORT, USHORT, USHORT * );
/*--------------------------------------------------------------------------------------------------*/
void	PrmCopyLoadMngInfoCmdSet( LONG, LONG );
void	PrmCopySaveMngInfoCmdSet( LONG, LONG );
/*--------------------------------------------------------------------------------------------------*/
void	PrmCopySetSendBuf(void);
USHORT	PrmCopyGetEntNum( const PRMTBL *, const USHORT );
//LONG	PrmCopyPrmRead( USHORT, const PRMTBL *, const USHORT, USHORT * );
/*--------------------------------------------------------------------------------------------------*/





/****************************************************************************************************/
/*																									*/
/*		パラメータコピー機能初期化処理																*/
/*																									*/
/****************************************************************************************************/
void	RpxLcdPrmCopyInit( void )
{
LONG idx, ret;
ULONG DriveID = 0x00;					/* <S073> */
ULONG IfMdleId = 0x00;					/* <S073> */
/*--------------------------------------------------------------------------------------------------*/
/*		パラメータコピー作業用変数全クリア															*/
/*--------------------------------------------------------------------------------------------------*/
		MlibResetByteMemory( (void *)&PrmCopyV, sizeof(PRM_COPY_INFO) );
/*--------------------------------------------------------------------------------------------------*/
/*		パラメータコピー管理用パラメータ設定														*/
/*--------------------------------------------------------------------------------------------------*/
#if 0
#if		CCSW_HWID == HWID_JPCV
		PrmCopyP.TypeID = KsysP.DriveID;
#else
		PrmCopyP.TypeID = (((PtiReg.IfMdleId & 0x00FF) << 8) | (KsysP.DriveID & 0x00FF));
#endif
#endif
/*--------------------------------------------------------------------------------------------------*/
		ret = OBJ_SEARCH( CINFREG, 0xC000 & 0x0FFF /* <S073> */, &idx ); 	/* @@ 0xC000: DriveId */
		if ( ret == OK ) { OBJ_READ( CINFREG, idx, &DriveID ); }  			/* @@ 0xC000: DriveId */
/*--------------------------------------------------------------------------------------------------*/
/*	
 *		TODO:
 *		レジスタ0xC013はMercuryに存在していないため、検索結果がNGになる。
 *
 */
/*--------------------------------------------------------------------------------------------------*/
		ret = OBJ_SEARCH( CINFREG, 0xC013 & 0x0FFF /* <S073> */, &idx ); 	/* @@ 0xC013: IfModuleId */
		if ( ret == OK ){ OBJ_READ( CINFREG, idx, &IfMdleId );  }			/* @@ 0xC013: IfModuleId */
		PrmCopyP.TypeID = (((IfMdleId & 0x00FF) << 8) | (DriveID & 0x00FF));
/*--------------------------------------------------------------------------------------------------*/
/*		Free Trace Buffer																			*/
/*--------------------------------------------------------------------------------------------------*/
		CTRCMEM->FreeBuffer( CTRCMEM, &PrmCopyV );
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		パラメータコピー機能メイン処理																*/
/*																									*/
/****************************************************************************************************/
void	RpxLcdPrmCopyMain( MSGIF *Hmsg, DRVSEL *pDrvSel )
{
		PrmCopyInit();						/* 初期処理		 										*/
		/* ModeChg = */PrmCopyOpMain();		/* キー操作メイン処理									*/
		PrmCopyDispMain();					/* 画面表示データ作成メイン処理							*/
		PrmCopySetSendBuf();				/* 通信作業領域から、送信バッファへデータセット			*/
		//return( ModeChg );
/*--------------------------------------------------------------------------------------------------*/
		RpxLcdSetResStatus ( (UCHAR *)Hmsg->ResBuf );	/* Set Status */
		RpxLcdSetBlink ( (UCHAR *)Hmsg->ResBuf );		/* Set Blink */
/*--------------------------------------------------------------------------------------------------*/
		return;
}


/****************************************************************************************************/
/*																									*/
/*		Checks Parameter Copy Execution Status														*/
/*																									*/
/****************************************************************************************************/
LONG RpxChkPrmCopyExe( void )
{
	return ( PrmCopyV.MainMode == PRMCOPY_MAINMODE_MODE_EXE ); 
}


/****************************************************************************************************/
/*																									*/
/*		初期処理																					*/
/*																									*/
/****************************************************************************************************/
void	PrmCopyInit( void )
{
		PrmCopyCmdInit();					/* デジオペ通信初期処理									*/
		PrmCopyNormalCmdSet(); 				/* デフォルトで通常通信コマンド(0x60)セット 			*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		キー操作メイン処理																			*/
/*																									*/
/****************************************************************************************************/
LONG	PrmCopyOpMain( void )
{
LONG	ModeChg = FALSE;

/*--------------------------------------------------------------------------------------------------*/
/*		Key Operation Main																			*/
/*--------------------------------------------------------------------------------------------------*/
		switch( PrmCopyV.MainMode )
		{
		/*------------------------------------------------------------------------------------------*/
		/*		機能選択																			*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_MAINMODE_MODE_SELECT:
			ModeChg = PrmCopyModeSelect();
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		機能実行																			*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_MAINMODE_MODE_EXE:
			PrmCopyModeExe();
			break;
		/*------------------------------------------------------------------------------------------*/
		default:
			/* エラー発生中処理 */
			PrmCopyErrorOp();
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return( ModeChg );
}



/****************************************************************************************************/
/*																									*/
/*		機能選択処理																				*/
/*																									*/
/*			"1:SERVO->OP",		[00] : SAVE 														*/
/*			"2:OP->SERVO",		[01] : LOAD															*/
/*			"3:VERIFY",			[02] : VERIFY														*/
/*			"4:DELETE",			[03] : DELETE														*/
/*																									*/
/****************************************************************************************************/
LONG	PrmCopyModeSelect( void )
{
LONG	ModeChg = FALSE;

/*--------------------------------------------------------------------------------------------------*/
/*		[MODE] key																					*/
/*--------------------------------------------------------------------------------------------------*/
		if( OpKeyMk.mode )
		{
			RpxLcdPrmCopyInit();					/* 初期化処理(構造体変数全クリア等、初回のみ)	*/
			PrmCopyInit();							/* 初期処理(通信バッファクリア等、毎回)			*/
			ModeChg  = TRUE;						/* モード変更：パラメータ編集・モニタ画面へ		*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		[↓] key																					*/
/*--------------------------------------------------------------------------------------------------*/
		else if( OpKeyMk.down )
		{
			if( PrmCopyV.SubMode < PRMCOPY_SUBMODE_MAX - 1 )
			{
				PrmCopyV.SubMode++;								/* 次の機能へ */
			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		[↑] key																					*/
/*--------------------------------------------------------------------------------------------------*/
		else if( OpKeyMk.up )
		{
			if( PrmCopyV.SubMode > 0 )
			{
				PrmCopyV.SubMode--;								/* 前の機能へ */
			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		[DATA] key																					*/
/*--------------------------------------------------------------------------------------------------*/
		else if( OpKeyMk.data )
		{
			if( PrmCopyModeSelectCheck() == RC_OK )				/* 機能選択チェック					*/
			{
				PrmCopyV.MainMode = PRMCOPY_MAINMODE_MODE_EXE;	/* 機能実行中状態セット				*/
				PrmCopyV.BlockNo = 0;							/* 選択中機能ＮＯを初期化(先頭)		*/
				PrmCopyV.SubModeSeq = PRMCOPY_SUBMODE_SEQ_INIT;	/* 選択機能初期処理へ				*/
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		return( ModeChg );
}



/****************************************************************************************************/
/*																									*/
/*		機能実行処理																				*/
/*																									*/
/****************************************************************************************************/
void	PrmCopyModeExe( void )
{
/*--------------------------------------------------------------------------------------------------*/
/*		実行処理シーケンス																			*/
/*--------------------------------------------------------------------------------------------------*/
		switch( PrmCopyV.SubModeSeq )
		{
		/*------------------------------------------------------------------------------------------*/
		/*		機能初期処理																		*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SUBMODE_SEQ_INIT:
			/* ブロック選択へ */
			PrmCopyV.SubModeSeq = PRMCOPY_SUBMODE_SEQ_BLOCK_SELECT;
		/*------------------------------------------------------------------------------------------*/
		/*		ブロック選択																		*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SUBMODE_SEQ_BLOCK_SELECT:
			PrmCopyBlockSelect();				/* ブロック選択処理				 					*/
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		実行開始確認																		*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SUBMODE_SEQ_EXE_CONFIRM:
			PrmCopyExeConfirm();				/* 実行開始確認処理				 					*/
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		実行																				*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SUBMODE_SEQ_EXE:
			switch( PrmCopyV.SubMode )
			{
				/*----------------------------------------------------------------------------------*/
				/*		SAVE(Servo -> DgOp)															*/
				/*----------------------------------------------------------------------------------*/
				case PRMCOPY_SUBMODE_SAVE:
					PrmCopySaveKeyOp();			/* Save中 キー処理 									*/
					PrmCopySaveOrDeleteExe();	/* Save処理実行										*/
					break;
				/*----------------------------------------------------------------------------------*/
				/*		DELETE(DgOp)																*/
				/*----------------------------------------------------------------------------------*/
				case PRMCOPY_SUBMODE_DELETE:
					PrmCopySaveOrDeleteExe();	/* Delete処理実行									*/
					break;
				/*----------------------------------------------------------------------------------*/
				/*		LOAD(DgOp -> Servo)															*/
				/*----------------------------------------------------------------------------------*/
				case PRMCOPY_SUBMODE_LOAD:
					PrmCopyLoadKeyOp();			/* Load中 キー処理 									*/
					PrmCopyLoadOrVerifyExe();	/* Load処理実行										*/
					break;
				/*----------------------------------------------------------------------------------*/
				/*		VERIFY(Servo <-> DgOp)														*/
				/*----------------------------------------------------------------------------------*/
				case PRMCOPY_SUBMODE_VERIFY:
					PrmCopyVerifyKeyOp();		/* VERIFY中 キー処理 								*/
					PrmCopyLoadOrVerifyExe();	/* VERIFY処理実行									*/
					break;
				/*----------------------------------------------------------------------------------*/
				default:
					break;
			}
			break;
		/*------------------------------------------------------------------------------------------*/
		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		パラメータブロック選択処理																	*/
/*																									*/
/****************************************************************************************************/
void	PrmCopyBlockSelect(void)
{
SHORT	rc;
ULONG	wkx;
ULONG	BlockSft = (Lpx_ChkDoubleBlock( PrmCopyP.TypeID ) ? 1 : 0 ); 	/* @@ SGD7W対応 */
ULONG	BlockMax = ((PRMCOPY_MNG_INFO_BLOCK_MAX >> BlockSft) - 1);		/* @@ SGD7W対応 */
/*--------------------------------------------------------------------------------------------------*/
/*		[MODE] key																					*/
/*--------------------------------------------------------------------------------------------------*/
		if( OpKeyMk.mode )
		{
			PrmCopyV.MainMode = PRMCOPY_MAINMODE_MODE_SELECT;	/* 機能選択画面へ */
		}
/*--------------------------------------------------------------------------------------------------*/
/*		[↑] key																					*/
/*--------------------------------------------------------------------------------------------------*/
		else if( OpKeyMk.up )
		{
			/* パラメータブロック表示スクロールアップ */
			wkx = PrmCopyV.BlockNo >> BlockSft;
			if( wkx == 0 )
			{
				wkx = BlockMax;
			}
			else
			{
				wkx--;
			}
			PrmCopyV.BlockNo = wkx << BlockSft;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		[↓] key																					*/
/*--------------------------------------------------------------------------------------------------*/
		else if( OpKeyMk.down )
		{
			/* パラメータブロック表示スクロールダウン */
			wkx = PrmCopyV.BlockNo >> BlockSft;
			if( wkx == BlockMax )
			{
				wkx = 0;
			}
			else
			{
				wkx++;
			}
			PrmCopyV.BlockNo = wkx << BlockSft;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		[DATA] key																					*/
/*--------------------------------------------------------------------------------------------------*/
		else if( OpKeyMk.data )
		{
			PrmCopyV.DispHoldTime = 0;					/* ２秒間画面表示カウンタークリア			*/
			rc = PrmCopyBlockSelectCheck();				/* ブロック選択可能チェック					*/
			if( rc != RC_OK ){ return;}
			PrmCopyV.SubModeSeq = PRMCOPY_SUBMODE_SEQ_EXE_CONFIRM;	/* 実行開始確認画面へ			*/
			PrmCopyV.ExeSeq = PRMCOPY_SAVE_SEQ_INIT;				/* 実行シーケンスを初期処理へ	*/
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		パラメータブロック選択チェック処理															*/
/*																									*/
/****************************************************************************************************/
SHORT PrmCopyBlockSelectCheck(void)
{
/*--------------------------------------------------------------------------------------------------*/
/*		実行処理シーケンス																			*/
/*--------------------------------------------------------------------------------------------------*/
		switch( PrmCopyV.SubMode )
		{
		/*------------------------------------------------------------------------------------------*/
		/*		SAVE(Servo -> DgOp)																	*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SUBMODE_SAVE:
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		LOAD(DgOp -> Servo)																	*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SUBMODE_LOAD:
			/* "****"表示を選択した時は、データなしエラー */
			if( PrmMngTbl[PrmCopyV.BlockNo].FormatID == PRMCOPY_MNG_DATA_NONE )
			{
				/* 選択不可エラー表示画面へ */
				PrmCopyV.ErrorNo = PRMCOPY_ERROR_NO_DATA;
				PrmCopyV.MainMode |= PRMCOPY_MAINMODE_ERROR_BIT;
				return( RC_NG );
			}
			/* パラメータ型式が違う */
			if( PrmCopyBlockTypeCheck( PrmCopyV.BlockNo ) != RC_OK )
			{
				/* パラメータ型式エラー表示画面へ */
				PrmCopyV.ErrorNo = PRMCOPY_ERROR_PRM_UNMATCH;
				PrmCopyV.MainMode |= PRMCOPY_MAINMODE_ERROR_BIT;
				return( RC_NG );
			}
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		VERIFY(Servo <-> DgOp)																*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SUBMODE_VERIFY:
			/* "****"表示を選択した時は、データなしエラー */
			if( PrmMngTbl[PrmCopyV.BlockNo].FormatID == PRMCOPY_MNG_DATA_NONE )
			{
				/* 選択不可エラー表示画面へ */
				PrmCopyV.ErrorNo = PRMCOPY_ERROR_NO_DATA;
				PrmCopyV.MainMode |= PRMCOPY_MAINMODE_ERROR_BIT;
				return( RC_NG );
			}
			/* サーボパック型式が違う */
			if( PrmMngTbl[PrmCopyV.BlockNo].ServoID != PrmCopyP.TypeID )
			{
				/* パラメータ型式エラー表示画面へ */
				PrmCopyV.ErrorNo = PRMCOPY_ERROR_PRM_UNMATCH;
				PrmCopyV.MainMode |= PRMCOPY_MAINMODE_ERROR_BIT;
				return( RC_NG );
			}
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		DELETE(DgOp)																		*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SUBMODE_DELETE:
			/* "****"表示を選択した時は、データなしエラー */
			if( PrmMngTbl[PrmCopyV.BlockNo].FormatID == PRMCOPY_MNG_DATA_NONE )
			{
				/* 選択不可エラー表示画面へ */
				PrmCopyV.ErrorNo = PRMCOPY_ERROR_NO_DATA;
				PrmCopyV.MainMode |= PRMCOPY_MAINMODE_ERROR_BIT;
				return( RC_NG );
			}
			break;
		/*------------------------------------------------------------------------------------------*/
		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return( RC_OK );
}


/****************************************************************************************************/
/*																									*/
/*		パラメータブロックタイプチェック処理														*/
/*																									*/
/****************************************************************************************************/
LONG	PrmCopyBlockTypeCheck( LONG BlockNo )
{
PRMDATA *pPrm = LopMsg.pAxRsc->UniPrms->Prm;			/* TODO: 後で消す */
/*--------------------------------------------------------------------------------------------------*/
/*		Block Type Check																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( PrmMngTbl[BlockNo].ServoID != PrmCopyP.TypeID )
		{
			return( RC_NG );
		}
//		else if( PrmMngTbl[BlockNo].FormatID > FORMAT_ID )
		else if( PrmMngTbl[BlockNo].FormatID > EEP_FORMAT_ID )
		{
			return( RC_NG );
		}
		else if( PrmMngTbl[BlockNo].AmpWatt != pPrm->ampw )
		{
			return( RC_NG );
		}
		else if( (PrmMngTbl[BlockNo].AmpVolt & 0x0F) != (pPrm->styp.b.h & 0x0F) )
		{
			return( RC_NG );
		}
/*--------------------------------------------------------------------------------------------------*/
		return( RC_OK );
}



/****************************************************************************************************/
/*																									*/
/*		実行開始確認処理																			*/
/*																									*/
/****************************************************************************************************/
void PrmCopyExeConfirm(void)
{
AXIS_HANDLE *pAxRsc = LopMsg.pAxRsc;			/* TODO: 後で消す */
/*--------------------------------------------------------------------------------------------------*/
/*		[MODE] key																					*/
/*--------------------------------------------------------------------------------------------------*/
		if( OpKeyMk.mode )
		{
			/* 機能初期処理後、ブロック選択へ戻る */
			PrmCopyV.SubModeSeq = PRMCOPY_SUBMODE_SEQ_INIT;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		[READ] key																					*/
/*--------------------------------------------------------------------------------------------------*/
		else if( OpKeyMk.read )
		{
			/* SAVE(Servo -> DgOp) or VERIFY(Servo <-> DgOp)処理中 */
			if( PrmCopyV.SubMode == PRMCOPY_SUBMODE_SAVE || PrmCopyV.SubMode == PRMCOPY_SUBMODE_VERIFY )
			{
				PrmCopyV.TraceBuffer = CTRCMEM->GetBuffer( CTRCMEM, &PrmCopyV );	/* get buffer 	*/
				if ( PrmCopyV.TraceBuffer != NULL )
				{
					/* 機能処理実行へ */
					PrmCopyV.SubModeSeq = PRMCOPY_SUBMODE_SEQ_EXE;
				}
				else
				{
					/* 実行不可エラー表示画面へ */
					PrmCopyV.ErrorNo = PRMCOPY_ERROR_NOT_AVAILABLE;
					PrmCopyV.MainMode |= PRMCOPY_MAINMODE_ERROR_BIT;
				}
			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		[WRITE] key																					*/
/*--------------------------------------------------------------------------------------------------*/
		else if( OpKeyMk.write )
		{
			/* LOAD(DgOp -> Servo) or DELETE(DgOp)処理中	*/
			if( PrmCopyV.SubMode == PRMCOPY_SUBMODE_LOAD || PrmCopyV.SubMode == PRMCOPY_SUBMODE_DELETE )
			{
				/* サーボオン中は実行不可エラー	*/
				if( ! CHK_BB() /*RinV.f.BaseEnable*/ )
				{
					/* 実行不可エラー表示画面へ */
					PrmCopyV.ErrorNo = PRMCOPY_ERROR_NOT_AVAILABLE;
					PrmCopyV.MainMode |= PRMCOPY_MAINMODE_ERROR_BIT;
				}
				else if ( ( PrmCopyV.TraceBuffer = CTRCMEM->GetBuffer( CTRCMEM, &PrmCopyV ) ) == NULL )
				{
					/* 実行不可エラー表示画面へ */
					PrmCopyV.ErrorNo = PRMCOPY_ERROR_NOT_AVAILABLE;
					PrmCopyV.MainMode |= PRMCOPY_MAINMODE_ERROR_BIT;
				}
				else
				{
					/* 機能処理実行へ */
					PrmCopyV.SubModeSeq = PRMCOPY_SUBMODE_SEQ_EXE;
				}
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		SAVE(Servo -> DgOp) or DELETE(DgOp)実行処理													*/
/*																									*/
/****************************************************************************************************/
void	PrmCopySaveOrDeleteExe( void )
{
SHORT	rc;
USHORT	prm_data;
USHORT	cmd_data_idx;
USHORT	send_data[LCD_MSG_DATA_GROUP_NUM];
AXIS_HANDLE *pAxRsc = LopMsg.pAxRsc;		/* TODO: 後で消す */
/*--------------------------------------------------------------------------------------------------*/
/*		処理実行シーケンス																			*/
/*--------------------------------------------------------------------------------------------------*/
		switch( PrmCopyV.ExeSeq )
		{
		/*==========================================================================================*/
		/*	０．初期処理																			*/
		/*==========================================================================================*/
		/*------------------------------------------------------------------------------------------*/
		/*		実行初期処理																		*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SAVE_SEQ_INIT:
			PrmCopyV.RetryCnt = 0;								/* 通信リトライカウントクリア		*/
			PrmCopyV.DispHoldTime = 0;							/* ２秒間画面表示カウンタークリア	*/
			PrmCopyV.DispKeepTime = 0;
			PrmCopyV.BlockMngAreaNo = 0;						/* ブロック管理情報領域０の削除		*/
			PrmCopyV.ExeSeq = PRMCOPY_SAVE_SEQ_HEADER_DEL_REQ;	/* ヘッダ情報削除コマンド要求へ		*/
			break;
		/*==========================================================================================*/
		/*	１．デジオペEEPROMの該当パラメータブロックの管理情報を削除								*/
		/*==========================================================================================*/
		/*------------------------------------------------------------------------------------------*/
		/*		ヘッダ情報削除コマンド要求															*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SAVE_SEQ_HEADER_DEL_REQ:
			/* デジオペ パラメータブロック管理情報クリア */
			PrmCopyPrmMngTblOp( PRMCOPY_MNG_INFODATA_CLEAR, PrmCopyV.BlockNo );
			/* 通信作業領域にSAVEコマンド(0x62)セット処理 */
			PrmCopySaveMngInfoCmdSet( PrmCopyV.BlockNo, PrmCopyV.BlockMngAreaNo );
			/* ヘッダ情報削除コマンド応答確認へ */
			PrmCopyV.ExeSeq = PRMCOPY_SAVE_SEQ_HEADER_DEL_RSP;
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		ヘッダ情報削除コマンド応答確認														*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SAVE_SEQ_HEADER_DEL_RSP:
			/* デジオペ通信コマンド応答確認処理 */
			rc = PrmCopyCmdRspCheck( LCD_CMD_SAVE );
			/*----- 受信正常 -----------------------------------------------------------------------*/
			if( rc == RC_OK )
			{
				/* 通信リトライカウントクリア */
				PrmCopyV.RetryCnt = 0;
//				PrmCopyV.PrmIndex = 0;
				PrmCopyV.PrmReadIndex = 0;
				PrmCopyV.PrmWriteIndex = 0;
			  /*------------------------------------------------------------------------------------*/
				if( PrmCopyV.BlockMngAreaNo == 0 )						/* 管理情報領域０の場合		*/
				{
					PrmCopyV.BlockMngAreaNo = 1;						/* 管理情報領域１の削除		*/
					PrmCopyV.ExeSeq = PRMCOPY_SAVE_SEQ_HEADER_DEL_REQ;	/* ヘッダ情報削除処理		*/
				}
				else if( PrmCopyV.SubMode == PRMCOPY_SUBMODE_SAVE )		/* パラメータ保存処理の場合	*/
				{
//Mercury			PrmCopyV.ExeSeq = PRMCOPY_SAVE_SEQ_PRM_SEND;		/* パラメータデータ送信へ	*/
					PrmCopyV.ExeSeq = PRMCOPY_SAVE_SEQ_PRM_READ_REQ;	/* @@ パラメータデータ読出し*/
				}
				else													/* パラメータ削除処理の場合	*/
				{
					PrmCopyV.ExeSeq = PRMCOPY_SAVE_SEQ_COMPLETE;		/* 完了画面表示へ			*/
				}
			}
			/*----- アラームコードを受信した -------------------------------------------------------*/
			else if( (rc == RC_NG) || (rc == RC_PASS) )
			{
				/* ヘッダ情報削除コマンド要求へ戻る */
				PrmCopyV.ExeSeq = PRMCOPY_SAVE_SEQ_HEADER_DEL_REQ;
			}
			/*----- リトライ回数オーバー -----------------------------------------------------------*/
			else if( rc == RC_RETRY_OVER )
			{
				/* 処理中断へ */
				PrmCopyV.ExeSeq = PRMCOPY_SAVE_SEQ_CANCEL;
			}
			/*----- 受信したコマンドコードが送信したコードと違う -----------------------------------*/
			else
			{
				;/* 引き続き受信待ち(rc == RC_PASS) */
			}
			break;
		/*==========================================================================================*/
		/*	２．デジオペEEPROMへ、全てのユーザーパラメータを8WORDづつ送信							*/
		/*==========================================================================================*/
		/*------------------------------------------------------------------------------------------*/
		/*		Read Upload Data from Eeprom														*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SAVE_SEQ_PRM_READ_REQ:
			if ( KriRequestReadLcdPdlEeprom( pAxRsc->pRwPdlMng, PrmCopyV.TraceBuffer, 
									(void *)&LcdPdlTbl[0], LCDPDLTBL_ENTNUM  ) == OK )
			{
				PrmCopyV.ExeSeq = PRMCOPY_SAVE_SEQ_PRM_READ_WAIT;
			}
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		Wait for Completing Read Data from Eeprom											*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SAVE_SEQ_PRM_READ_WAIT:
			if (  KriChkDoneReadLcdPdlEeprom( pAxRsc->pRwPdlMng ) )
			{
				PrmCopyV.ExeSeq = PRMCOPY_SAVE_SEQ_PRM_SEND;
			}
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		パラメータデータ伝送コマンド送信													*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SAVE_SEQ_PRM_SEND:
			/* リトライ時に、インデックスを戻す為の保存 */
			PrmCopyV.PrmReadIndex_Last = PrmCopyV.PrmReadIndex;
			PrmCopyV.PrmWriteIndex_Last = PrmCopyV.PrmWriteIndex;
//			PrmCopyV.PrmIndex_Last = PrmCopyV.PrmIndex;
			/* １回送信分の８個のパラメータデータを取得 */
			for( cmd_data_idx = 0; cmd_data_idx < LCD_MSG_DATA_GROUP_NUM; cmd_data_idx++ )
			{
				if ( PrmCopyV.PrmReadIndex >= LCDPDLTBL_ENTNUM ) { break; }	/* Data End */
				prm_data = PrmCopyV.TraceBuffer[ PrmCopyV.PrmReadIndex ];
				/* 送信バッファにパラメータデータをセット */
				send_data[cmd_data_idx] = prm_data;
				PrmCopyV.PrmReadIndex++;
			}
			/* デジオペEEPROMパラメータアドレス更新 */
			PrmCopyV.PrmWriteIndex += cmd_data_idx;
			/* 通信作業領域にSAVEコマンド(0x62)セット処理 */
			PrmCopySaveCmdSet( PRMCOPY_PRM_ADDR_GET(PrmCopyV.BlockNo, PrmCopyV.PrmWriteIndex_Last), 
							cmd_data_idx, send_data );
			/* パラメータデータ伝送コマンド応答確認へ */
			PrmCopyV.ExeSeq = PRMCOPY_SAVE_SEQ_PRM_RSP;
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		パラメータデータ伝送コマンド応答確認												*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SAVE_SEQ_PRM_RSP:
			/* デジオペ通信コマンド応答確認処理 */
			rc = PrmCopyCmdRspCheck( LCD_CMD_SAVE );
			/*----- 受信正常 -----------------------------------------------------------------------*/
			if( rc == RC_OK )
			{
				/* 通信リトライカウントクリア */
				PrmCopyV.RetryCnt = 0;
				/* パラメータデータを全て送信完了 */
				if( PrmCopyV.PrmReadIndex >= PrmEntNum.SvPrm )
				{
					PrmCopyV.BlockMngAreaNo = 0;					/* 管理情報領域０の書き込み		*/
					PrmCopyV.ExeSeq = PRMCOPY_SAVE_SEQ_HEADER_SEND;	/* 次のコマンド(管理情報伝送)へ */
				}
				else
				{
					/* 引き続き次のパラメータデータ伝送を行う */
					PrmCopyV.ExeSeq = PRMCOPY_SAVE_SEQ_PRM_SEND;
				}
			}
			/*----- アラームコードを受信した -------------------------------------------------------*/
			else if( (rc == RC_NG) || (rc == RC_PASS) )
			{
				/* パラメータインデックスを元に戻す */
				PrmCopyV.PrmReadIndex = PrmCopyV.PrmReadIndex_Last;
				PrmCopyV.PrmWriteIndex = PrmCopyV.PrmWriteIndex_Last;
//				PrmCopyV.PrmIndex = PrmCopyV.PrmIndex_Last;
				/* パラメータデータ伝送コマンド送信へ戻ってリトライ */
				PrmCopyV.ExeSeq = PRMCOPY_SAVE_SEQ_PRM_SEND;
			}
			/*----- リトライ回数オーバー -----------------------------------------------------------*/
			else if( rc == RC_RETRY_OVER )
			{
				/* 処理中断へ */
				PrmCopyV.ExeSeq = PRMCOPY_SAVE_SEQ_CANCEL;
			}
			/*----- 受信したコマンドコードが送信したコードと違う -----------------------------------*/
			else /* if( rc == RC_PASS ) */
			{
				;/* 引き続き受信待ち */
			}
			break;
		/*==========================================================================================*/
		/*	３．デジオペEEPROMへ、該当パラメータブロックの管理情報を送信							*/
		/*==========================================================================================*/
		/*------------------------------------------------------------------------------------------*/
		/*		ヘッダ情報登録コマンド送信															*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SAVE_SEQ_HEADER_SEND:
			/* デジオペ パラメータブロック管理情報セット */
			PrmCopyPrmMngTblOp( PRMCOPY_MNG_INFODATA_SET, PrmCopyV.BlockNo );
			/* 通信作業領域にSAVEコマンド(0x62)セット処理 */
			PrmCopySaveMngInfoCmdSet( PrmCopyV.BlockNo, PrmCopyV.BlockMngAreaNo );
			/* ヘッダ情報登録コマンド応答確認へ */
			PrmCopyV.ExeSeq = PRMCOPY_SAVE_SEQ_HEADER_RSP;
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		ヘッダ情報登録コマンド応答確認														*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SAVE_SEQ_HEADER_RSP:
			/* デジオペ通信コマンド応答確認 */
			rc = PrmCopyCmdRspCheck( LCD_CMD_SAVE );
			/*----- 受信正常 -----------------------------------------------------------------------*/
			if( rc == RC_OK )
			{
				PrmCopyV.RetryCnt = 0;								/* 通信リトライカウントクリア	*/
				if( PrmCopyV.BlockMngAreaNo == 0 )					/* 管理情報領域０の場合			*/
				{
					PrmCopyV.BlockMngAreaNo = 1;					/* 管理情報領域１の書き込み		*/
					PrmCopyV.ExeSeq = PRMCOPY_SAVE_SEQ_HEADER_SEND;	/* 管理情報送信へ				*/
				}
				else
				{
					PrmCopyV.ExeSeq = PRMCOPY_SAVE_SEQ_COMPLETE;	/* 完了画面表示へ				*/
				}
			}
			/*----- アラームコードを受信した -------------------------------------------------------*/
			else if( (rc == RC_NG) || (rc == RC_PASS) )
			{
				/* ヘッダ情報登録コマンド送信へ戻る */
				PrmCopyV.ExeSeq = PRMCOPY_SAVE_SEQ_HEADER_SEND;
			}
			/*----- リトライ回数オーバー -----------------------------------------------------------*/
			else if( rc == RC_RETRY_OVER )
			{
				/* 処理中断へ */
				PrmCopyV.ExeSeq = PRMCOPY_SAVE_SEQ_CANCEL;
			}
			/*----- 受信したコマンドコードが送信したコードと違う -----------------------------------*/
			else
			{
				;/* 引き続き受信待ち */
			}
			break;
		/*==========================================================================================*/
		/*	４．デジオペ実行完了画面、又は、中断画面を約２秒間表示するために実行シーケンス保持		*/
		/*==========================================================================================*/
		/*------------------------------------------------------------------------------------------*/
		/*		デジオペ実行完了画面表示中															*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SAVE_SEQ_COMPLETE:
		/*------------------------------------------------------------------------------------------*/
		/*		デジオペ中断画面表示中																*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SAVE_SEQ_CANCEL:
			/* 画面表示初回 */
			if( PrmCopyV.DispHoldTime == 0 )
			{
				/* タイマーリセット */
				KpiRstLongTimer( &PrmCopyV.DispHoldTime );
				break;
			}
			/* ２秒間経過した */
			if( KpiGetLongTimer(&PrmCopyV.DispHoldTime) > TIME_SECOND_2 )
			{
				CTRCMEM->FreeBuffer( CTRCMEM, &PrmCopyV );	/* @@ */
				/* 機能初期処理後、ブロック選択へ戻る */
				PrmCopyV.SubModeSeq = PRMCOPY_SUBMODE_SEQ_INIT;
			}
			break;
		/*------------------------------------------------------------------------------------------*/
		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		LOAD(DgOp -> Servo)、又は、VERIFY(Servo <-> DgOp)実行処理									*/
/*																									*/
/****************************************************************************************************/
void	PrmCopyLoadOrVerifyExe( void )
{
SHORT	rc, rc2;
AXIS_HANDLE *pAxRsc = LopMsg.pAxRsc;		/* TODO: 後で消す */
/*--------------------------------------------------------------------------------------------------*/
/*		処理実行シーケンス																			*/
/*--------------------------------------------------------------------------------------------------*/
		switch( PrmCopyV.ExeSeq )
		{
		/*==========================================================================================*/
		/*	０．初期処理																			*/
		/*==========================================================================================*/
		/*------------------------------------------------------------------------------------------*/
		/*		実行初期処理																		*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_LOAD_SEQ_INIT:
			/* パラメータコピー機能において、パラメータ一括書き換え中フラグをON */
			pAxRsc->FnCmnCtrl->f.PrmCopy = TRUE; /* RoutV.f.PrmCopy = TRUE; */
			/* 通信リトライカウントクリア */
			PrmCopyV.RetryCnt = 0;
			PrmCopyV.PrmReadIndex = 0;
			PrmCopyV.PrmWriteIndex = 0;
//			PrmCopyV.PrmIndex = 0;
//			PrmCopyV.UserChkSum = 0;
			PrmCopyV.VerifyErrTblCnt = 0;
			PrmCopyV.VerifyErrTotalCnt = 0;
			/* ２秒間画面表示カウンタークリア */
			PrmCopyV.DispHoldTime = 0;
			PrmCopyV.DispKeepTime = 0;
			/* VERIFY処理時 */
			if( PrmCopyV.SubMode == PRMCOPY_SUBMODE_VERIFY )
			{
				/* パラメータREADコマンド要求へ */
				PrmCopyV.ExeSeq = PRMCOPY_LOAD_SEQ_FILL_REQ;
				break;
			}
			/* LOAD処理時、実行前再確認へ */
			PrmCopyV.ExeSeq = PRMCOPY_LOAD_SEQ_PRM_READ_REQ;
			break;
		/*------------------------------------------------------------------------------------------*/
		/*	@@	Read Verify Data from Eeprom														*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_LOAD_SEQ_FILL_REQ:
			if ( KriRequestReadLcdPdlEeprom( pAxRsc->pRwPdlMng, PrmCopyV.TraceBuffer, 
									(void*)&LcdPdlTbl[0], LCDPDLTBL_ENTNUM  ) == OK )
			{
				PrmCopyV.ExeSeq = PRMCOPY_LOAD_SEQ_FILL_WAIT;
			}
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		Wait for Completing Read Data from Eeprom											*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_LOAD_SEQ_FILL_WAIT:
			if ( KriChkDoneReadLcdPdlEeprom( pAxRsc->pRwPdlMng ) )
			{
				PrmCopyV.ExeSeq = PRMCOPY_LOAD_SEQ_PRM_READ_REQ;
			}
			break;
		/*------------------------------------------------------------------------------------------*/
		/*------------------------------------------------------------------------------------------*/
		/*		LOAD処理時、実行前再確認処理														*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_LOAD_SEQ_RECONFIRM:
			break;
		/*==========================================================================================*/
		/*	１．デジオペEEPROMから該当ブロックのユーザーパラメータを								*/
		/*		8WORDづつ受信しながら、サーボパックのEEPROMへ書き込むか、又は、						*/
		/*		サーボパックのパラメータとVERIFYを行う												*/
		/*==========================================================================================*/
		/*------------------------------------------------------------------------------------------*/
		/*		パラメータREADコマンド要求															*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_LOAD_SEQ_PRM_READ_REQ:
			/* 通信作業領域にLOADコマンド(0x61)セット */
			PrmCopyLoadCmdSet( PRMCOPY_PRM_ADDR_GET(PrmCopyV.BlockNo, PrmCopyV.PrmReadIndex), 
							LCD_MSG_DATA_GROUP_NUM );
			/* ヘッダ情報削除コマンド応答確認へ */
			PrmCopyV.ExeSeq = PRMCOPY_LOAD_SEQ_PRM_READ_RSP;
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		コマンド応答確認																	*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_LOAD_SEQ_PRM_READ_RSP:
			/* デジオペ通信コマンド応答確認処理 */
			rc = PrmCopyCmdRspCheck( LCD_CMD_LOAD );
			/*----- 受信正常 -----------------------------------------------------------------------*/
			if( rc == RC_OK )
			{
				/* 読み込み先インデックスを更新(+8) */
				PrmCopyV.PrmReadIndex += LCD_MSG_DATA_GROUP_NUM;
				/* 処理タイプ判定 */
				if( PrmCopyV.SubMode == PRMCOPY_SUBMODE_LOAD )
				{
					/********************************************************************************/
					/*	１回受信分の最大８個のパラメータをサーボパックのEEPROMへ書き込む処理 		*/
					/********************************************************************************/
					rc2 = PrmCopyLoadExePrmWrite();
				}
				else	/* PRMCOPY_SUBMODE_VERIFY */
				{
					/********************************************************************************/
					/* 	１回受信分の最大８個のパラメータと 											*/
					/* 		サーボパックのパラメータ(EEPROM)とVERIFYを行う							*/
					/********************************************************************************/
					rc2 = PrmCopyVerifyExePrmVerify();
				}
				/* パラメータ書き込み完了 */
				if( rc2 == RC_OK )
				{
					/* 通信リトライカウントクリア */
					PrmCopyV.RetryCnt = 0;
					/* 引き続き次のデータ受信を行う */
					PrmCopyV.ExeSeq = PRMCOPY_LOAD_SEQ_PRM_READ_REQ;
					break;
				}
				/* パラメータ全て書き込み完了 -> パラメータすべて受信完了<S00?> */
				else if( rc2 == RC_COMPLETE )
				{
					if( PrmCopyV.SubMode == PRMCOPY_SUBMODE_LOAD )
					{
						/* 全ユーザパラメータをEEPROMに書込み */
						PrmCopyV.ExeSeq = PRMCOPY_LOAD_SEQ_STORE_REQ;
					}
					else
					{
						/* 処理完了へ */
						PrmCopyV.ExeSeq = PRMCOPY_LOAD_SEQ_COMPLETE;
					}
				}
				/* パラメータ書き込み要求失敗 */
				else if( rc2 == RC_NG )
				{
					/* 再度、同じパラメータデータ受信へ */
					PrmCopyV.ExeSeq = PRMCOPY_LOAD_SEQ_PRM_READ_REQ;
					PrmCopyV.PrmReadIndex = PrmCopyV.PrmWriteIndex;
				}
			}
			/*----- アラームコードを受信した -------------------------------------------------------*/
			else if( (rc == RC_NG) || (rc == RC_PASS) )
			{
				/* 再度、同じパラメータデータ受信へ */
				PrmCopyV.ExeSeq = PRMCOPY_LOAD_SEQ_PRM_READ_REQ;
			}
			/*----- リトライ回数オーバー -----------------------------------------------------------*/
			else if( rc == RC_RETRY_OVER )
			{
				/* 処理中断へ */
				PrmCopyV.ExeSeq = PRMCOPY_LOAD_SEQ_CANCEL;
			}
			/*----- 受信したコマンドコードが送信したコードと違う -----------------------------------*/
			else
			{
				/* 引き続き受信待ち */
			}
			break;
		/*==========================================================================================*/
		/*	２．デジオペ実行完了画面、又は、中断画面を約２秒間表示するために実行シーケンス保持		*/
		/*==========================================================================================*/
		/*------------------------------------------------------------------------------------------*/
		/*		デジオペ実行完了画面表示中															*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_LOAD_SEQ_COMPLETE:
		/*------------------------------------------------------------------------------------------*/
		/*		デジオペ中断画面表示中																*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_LOAD_SEQ_CANCEL:
			/* 画面表示初回 */
			if( PrmCopyV.DispHoldTime == 0 )
			{
				/* パラメータコピー機能において、パラメータ一括書き換え中フラグをOFF */
				//RoutV.f.PrmCopy = FALSE;
				pAxRsc->FnCmnCtrl->f.PrmCopy = FALSE;			/* @@ chk */
				/* ２秒間表示タイマーリセット */
				KpiRstLongTimer( &PrmCopyV.DispHoldTime );
				break;
			}
			/* ２秒間表示した */
			if( KpiGetLongTimer(&PrmCopyV.DispHoldTime) > TIME_SECOND_2 )
			{
				CTRCMEM->FreeBuffer( CTRCMEM, &PrmCopyV );	/* @@ */
				PrmCopyV.DispHoldTime = 0;
				/* LOAD 処理時 */
				if( PrmCopyV.SubMode == PRMCOPY_SUBMODE_LOAD )
				{
//<S0AA>			KPI_WARNINGSET(WRN_BOOT);		/* A.941 : 電源再投入要ユーザー定数変更			*/
					ALMSetServoAlarm( pAxRsc->AlmManager, WRN_BOOT );	/* <S0AA> A.941 : */
					/* 機能初期処理後、ブロック選択へ戻る */
					PrmCopyV.SubModeSeq = PRMCOPY_SUBMODE_SEQ_INIT;
				}
				else	/* VERIFY 処理時 */
				{
					/* キャンセルの場合 */
					if( PrmCopyV.ExeSeq == PRMCOPY_LOAD_SEQ_CANCEL )
					{
						/* 機能初期処理後、ブロック選択へ戻る */
						PrmCopyV.SubModeSeq = PRMCOPY_SUBMODE_SEQ_INIT;
					}
					else if( PrmCopyV.VerifyErrTotalCnt == 0 )	/* VERIFY結果 全一致 */
					{
						/* VERIFY結果 一致画面表示へ */
						PrmCopyV.ExeSeq = PRMCOPY_VERIFY_SEQ_MATCHED;
					}
					else	/* VERIFY結果 不一致あり */
					{
						PrmCopyV.VerifyErrListNo = 0;
						/* VERIFY結果不一致画面表示へ */
						PrmCopyV.ExeSeq = PRMCOPY_VERIFY_SEQ_UNMATCHED;
					}
				}
			}
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		VERIFY結果一致画面表示中															*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_VERIFY_SEQ_MATCHED:
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		VERIFY結果不一致画面表示中															*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_VERIFY_SEQ_UNMATCHED:
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		Store Parameters to Eeprom															*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_LOAD_SEQ_STORE_REQ:
		/*------------------------------------------------------------------------------------------*/
			if ( KriRequestWriteLcdPdlEeprom( pAxRsc->pRwPdlMng, PrmCopyV.TraceBuffer, 
									(void*)&LcdPdlTbl[0], LCDPDLTBL_ENTNUM  ) == OK )
			{
				PrmCopyV.ExeSeq = PRMCOPY_LOAD_SEQ_STORE_WAIT;
			}
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		Store Parameters to Eeprom															*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_LOAD_SEQ_STORE_WAIT:
			if (  KriChkDoneWriteLcdPdlEeprom( pAxRsc->pRwPdlMng ) )
			{
				PrmCopyV.ExeSeq = PRMCOPY_LOAD_SEQ_COMPLETE;
			}
			break;
		/*------------------------------------------------------------------------------------------*/
		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		SAVE(Servo -> DgOp)中  Key処理																*/
/*																									*/
/****************************************************************************************************/
void	PrmCopySaveKeyOp( void )
{
/*--------------------------------------------------------------------------------------------------*/
/*		処理実行シーケンス																			*/
/*--------------------------------------------------------------------------------------------------*/
		switch( PrmCopyV.ExeSeq )
		{
		case PRMCOPY_SAVE_SEQ_COMPLETE :	/* デジオペ実行完了画面表示中 */
			break;
		default :
			/*-----	[MODE] key ---------------------------------------------------------------------*/
			if( OpKeyMk.mode )
			{
				/* 実行処理中断へ */
				PrmCopyV.ExeSeq = PRMCOPY_SAVE_SEQ_CANCEL;
			}
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		LOAD(DgOp -> Servo)中  Key処理																*/
/*																									*/
/****************************************************************************************************/
void	PrmCopyLoadKeyOp( void )
{
/*--------------------------------------------------------------------------------------------------*/
/*		処理実行シーケンス																			*/
/*--------------------------------------------------------------------------------------------------*/
		switch( PrmCopyV.ExeSeq )
		{
		/*------------------------------------------------------------------------------------------*/
		/*		LOAD処理時、実行前再確認中															*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_LOAD_SEQ_RECONFIRM:
			/*-----	[MODE] key ---------------------------------------------------------------------*/
			if( OpKeyMk.mode )
			{
				/* 機能初期処理後、ブロック選択へ戻る */
				PrmCopyV.SubModeSeq = PRMCOPY_SUBMODE_SEQ_INIT;
			}
			/*-----	[DATA] key ---------------------------------------------------------------------*/
			else if( OpKeyMk.data )
			{
				/* パラメータREADコマンド要求へ */
				PrmCopyV.ExeSeq = PRMCOPY_LOAD_SEQ_PRM_READ_REQ;
			}
			break;
		/*------------------------------------------------------------------------------------------*/
		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		VERIFY(サーボパック <-> デジオペ)中  Key処理												*/
/*																									*/
/****************************************************************************************************/
void	PrmCopyVerifyKeyOp( void )
{
/*--------------------------------------------------------------------------------------------------*/
/*		処理実行シーケンス																			*/
/*--------------------------------------------------------------------------------------------------*/
		switch( PrmCopyV.ExeSeq )
		{
		/*------------------------------------------------------------------------------------------*/
		/*		VERIFY結果 一致画面表示中															*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_VERIFY_SEQ_MATCHED:
			/*-----	[MODE] key ---------------------------------------------------------------------*/
			if( OpKeyMk.mode )
			{
				/* 機能初期処理後、ブロック選択へ戻る */
				PrmCopyV.SubModeSeq = PRMCOPY_SUBMODE_SEQ_INIT;
			}
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		VERIFY結果 不一致画面表示中															*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_VERIFY_SEQ_UNMATCHED:
			/*----- [MODE] key ---------------------------------------------------------------------*/
			if( OpKeyMk.mode )
			{
				/* 機能初期処理後、ブロック選択へ戻る */
				PrmCopyV.SubModeSeq = PRMCOPY_SUBMODE_SEQ_INIT;
			}
			/*----- [↑] key -----------------------------------------------------------------------*/
			else if( OpKeyMk.up )
			{
				/* パラメータブロック表示スクロールアップ */
				if( PrmCopyV.VerifyErrListNo > 0 )
					PrmCopyV.VerifyErrListNo--;
			}
			/*----- [↓] key -----------------------------------------------------------------------*/
			else if( OpKeyMk.down )
			{
				/* パラメータブロック表示スクロールダウン */
				if( PrmCopyV.VerifyErrListNo + PRMCOPY_DISP_LIST_MAX < PrmCopyV.VerifyErrTblCnt )
					PrmCopyV.VerifyErrListNo++;
			}
			break;
		default:
			/*-----	[MODE] key ---------------------------------------------------------------------*/
			if( OpKeyMk.mode )
			{
				/* 処理中断メッセージ画面表示へ */
				PrmCopyV.ExeSeq = PRMCOPY_LOAD_SEQ_CANCEL;
			}
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		１回受信分の最大８個のパラメータをサーボパックのEEPROMへ書き込む処理						*/
/*		<MERCURY>EEPROM書込みキューは使わない処理に変更												*/
/*																									*/
/****************************************************************************************************/
SHORT	PrmCopyLoadExePrmWrite(void)
{
USHORT	cmd_data_idx;
USHORT	dgop_prm_data;
/*--------------------------------------------------------------------------------------------------*/
/*		１回受信分の最大８個のパラメータを処理														*/
/*--------------------------------------------------------------------------------------------------*/
		for( cmd_data_idx = 0; cmd_data_idx < LCD_MSG_DATA_GROUP_NUM; cmd_data_idx++ )
		{
		/*------------------------------------------------------------------------------------------*/
		/*		受信したパラメータを取得															*/
		/*------------------------------------------------------------------------------------------*/
			dgop_prm_data = PRMCOPY_CMD_DATA_GET( cmd_data_idx );
		/*------------------------------------------------------------------------------------------*/
			if (( PrmCopyV.PrmWriteIndex >= 2 ) && ( PrmCopyV.PrmWriteIndex >= PrmCopyV.TraceBuffer[ 1 ] ))
			{
				return RC_COMPLETE; 
			}
			PrmCopyV.TraceBuffer[ PrmCopyV.PrmWriteIndex ] = dgop_prm_data;
			PrmCopyV.PrmWriteIndex++;
		}
/*--------------------------------------------------------------------------------------------------*/
		return( RC_OK );
}



/****************************************************************************************************/
/*																									*/
/*		１回受信分の最大８個のパラメータとサーボパックのパラメータ(EEPROM)とVERIFYを行う処理		*/
/*																									*/
/****************************************************************************************************/
SHORT	PrmCopyVerifyExePrmVerify(void)
{
USHORT	cmd_data_idx;
USHORT	prm_data, dgop_prm_data;
//USHORT	nPrmVerify;	
/*--------------------------------------------------------------------------------------------------*/
/*		１回受信分の８個のパラメータデータを処理													*/
/*--------------------------------------------------------------------------------------------------*/
		for( cmd_data_idx = 0; cmd_data_idx < LCD_MSG_DATA_GROUP_NUM; cmd_data_idx++ )
		{
		/*------------------------------------------------------------------------------------------*/
		/*		パラメータを取得																	*/
		/*------------------------------------------------------------------------------------------*/
			/* サーボ側パラメータ取得 */
			prm_data = PrmCopyV.TraceBuffer[ PrmCopyV.PrmWriteIndex ];
			/* デジオペから受信したパラメータを取得 */
			dgop_prm_data = PRMCOPY_CMD_DATA_GET( cmd_data_idx );
		/*------------------------------------------------------------------------------------------*/
		/*		データ数を取得																		*/
		/*------------------------------------------------------------------------------------------*/
			if ( PrmCopyV.PrmWriteIndex == 1 )
			{
				PrmCopyV.VerifyNumber = MlibMIN( prm_data, dgop_prm_data );
			}
		/*------------------------------------------------------------------------------------------*/
		/*		旧Prmと新Servoの比較：旧Prmにないパラメータは比較しない。							*/
		/*		新Prmと旧Servoの比較：旧Servoにないパラメータは比較しない。							*/
		/*------------------------------------------------------------------------------------------*/
			else if ( PrmCopyV.PrmWriteIndex >= 2 ) 
			{
				//nPrmVerify = MlibMIN( PrmCopyV.TraceBuffer[1], LcdPdlTbl[1].PnNo );
				if ( PrmCopyV.PrmWriteIndex >= PrmCopyV.VerifyNumber /* nPrmVerify */)
				{
					return( RC_COMPLETE );				/* 全検索終了								*/
				}
			}
		/*------------------------------------------------------------------------------------------*/
		/*		データ不一致																		*/
		/*------------------------------------------------------------------------------------------*/
			if (( LcdPdlTbl[ PrmCopyV.PrmWriteIndex ].pPrmDef != NULL ) && ( prm_data != dgop_prm_data ))
			{
				PrmCopyV.VerifyErrTotalCnt++;			/* VERIFYエラー発生件数インクリメント		*/
				if( PrmCopyV.VerifyErrTblCnt < PRMCOPY_VERIFY_ERR_DISP_TBL_MAX )
				{
					PrmCopyV.VerifyErrPrmIdxTbl[PrmCopyV.VerifyErrTblCnt] = PrmCopyV.PrmWriteIndex;
					PrmCopyV.VerifyErrTblCnt++;
				}
			}
		/*------------------------------------------------------------------------------------------*/
			PrmCopyV.PrmWriteIndex++;
		}
/*--------------------------------------------------------------------------------------------------*/
		return( RC_OK );
}



/****************************************************************************************************/
/*																									*/
/*		機能選択チェック処理																		*/
/*																									*/
/****************************************************************************************************/
SHORT	PrmCopyModeSelectCheck(void)
{
AXIS_HANDLE *pAxRsc = LopMsg.pAxRsc;
/*--------------------------------------------------------------------------------------------------*/
/*		機能が実行可能かどうかチェックを行う														*/
/*--------------------------------------------------------------------------------------------------*/
		switch( PrmCopyV.SubMode )
		{
		/*------------------------------------------------------------------------------*/
		/*		SAVE(Servo -> DgOp)														*/
		/*------------------------------------------------------------------------------*/
		case PRMCOPY_SUBMODE_SAVE:
			break;
		/*------------------------------------------------------------------------------*/
		/*		DELETE(DgOp)															*/
		/*------------------------------------------------------------------------------*/
		case PRMCOPY_SUBMODE_DELETE:
			break;
		/*------------------------------------------------------------------------------*/
		/*		LOAD(DgOp -> Servo)														*/
		/*------------------------------------------------------------------------------*/
		case PRMCOPY_SUBMODE_LOAD:
			/* サーボオン中は実行不可エラー	*/
			if( ! CHK_BB() /*RinV.f.BaseEnable*/ )
			{
				/* 実行不可エラー表示画面へ */
				PrmCopyV.ErrorNo = PRMCOPY_ERROR_NOT_AVAILABLE;
				PrmCopyV.MainMode |= PRMCOPY_MAINMODE_ERROR_BIT;
				return( RC_NG );
			}
			/* パラメータ書き込み禁止中は実行不可エラー */
			if( CHK_WRITEINHIBIT() /* Iprm.f.PrmWrtInhibit */ && (LopMsg.AccessLvl != ACCLVL_SYSTEM) )
			{
				/* 実行不可エラー表示画面へ */
				PrmCopyV.ErrorNo = PRMCOPY_ERROR_NOT_AVAILABLE;
				PrmCopyV.MainMode |= PRMCOPY_MAINMODE_ERROR_BIT;
				return( RC_NG );
			}
			pAxRsc->FnCmnCtrl->KnlMode = KPI_KMD_PARAMCPY;	/* @@ Executing Parameter Copy */
			break;
		/*------------------------------------------------------------------------------*/
		/*		VERIFY(Servo <-> DgOp)													*/
		/*------------------------------------------------------------------------------*/
		case PRMCOPY_SUBMODE_VERIFY:
			break;
		/*------------------------------------------------------------------------------*/
		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return( OK );
}



/****************************************************************************************************/
/*																									*/
/*		エラー発生中処理																			*/
/*																									*/
/****************************************************************************************************/
void	PrmCopyErrorOp( void )
{
/*--------------------------------------------------------------------------------------------------*/
/*		エラー発生中処理																			*/
/*--------------------------------------------------------------------------------------------------*/
		switch( PrmCopyV.ErrorNo )
		{
		/*------------------------------------------------------------------------------------------*/
		/*		実行不可エラー																		*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_ERROR_NOT_AVAILABLE:
		/*------------------------------------------------------------------------------------------*/
		/*		選択不可エラー																		*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_ERROR_NO_DATA:
		/*------------------------------------------------------------------------------------------*/
		/*		パラメータ型式エラー																*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_ERROR_PRM_UNMATCH:
			/* 画面表示初回 */
			if( PrmCopyV.DispHoldTime == 0 )
			{
				/* タイマーリセット */
				KpiRstLongTimer( &PrmCopyV.DispHoldTime );
			}
			/* ２秒間表示した */
			if( KpiGetLongTimer(&PrmCopyV.DispHoldTime) > TIME_SECOND_2 )
			{
				/* ２秒間画面表示カウンタークリア */
				PrmCopyV.DispHoldTime = 0;
				/* エラー状態クリア */
				PrmCopyV.ErrorNo = PRMCOPY_ERROR_NONE;
				PrmCopyV.MainMode &= ~PRMCOPY_MAINMODE_ERROR_BIT;
				break;
			}
			return;
		/*------------------------------------------------------------------------------------------*/
		/*		エラー発生なし																		*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_ERROR_NONE:
			break;
		/*------------------------------------------------------------------------------------------*/
		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		ユーザーパラメータ検索処理																	*/
/*																									*/
/****************************************************************************************************/
#if 0
SHORT	PrmCopyUsrPrmSearch( USHORT *SrchIdx, const PRMTBL *PrmTbl, const USHORT EntNum )
{
CPRMDEF	*PrmDefPtr;
ULONG	EntNumX = 2 * EntNum;
/*--------------------------------------------------------------------------------------------------*/
/*		Check LongPrm																				*/
/*--------------------------------------------------------------------------------------------------*/
		if( LPX_GET_HiWdFlag( (*SrchIdx) ) )
		{
			return( RC_OK );						/* 該当パラメータあり (LongHighWord)			*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		全パラメータの中から、ユーザーパラメータを検索												*/
/*--------------------------------------------------------------------------------------------------*/
		for(   ; *SrchIdx < EntNumX; LPX_INC_SrhIndex( (*SrchIdx) ) )
		{
			PrmDefPtr = LPX_GET_PrmDefPtr( (*SrchIdx) );
			if(	LPX_CHK_EEP_PRM( PrmDefPtr ) )
			{
				UCHAR  AccLvl = PrmDefPtr->AccessLevel;
				if( (AccLvl == ACCLVL_USER1) || (AccLvl == ACCLVL_USER2) )
				{
					return( RC_OK );				/* 該当パラメータあり							*/
				}
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		return( RC_NG );							/* 該当パラメータなし(全検索終了)				*/
}
#endif



/****************************************************************************************************/
/*																									*/
/*		ＨＥＸから文字列への変換処理																*/
/*																									*/
/****************************************************************************************************/
void	ConvertHexToStr( USHORT HexData, UCHAR *str, USHORT digit )
{
LONG	i,x;

/*--------------------------------------------------------------------------------------------------*/
/*		Check Digit																					*/
/*--------------------------------------------------------------------------------------------------*/
		if( (digit == 0) || (8 < digit) )
		{
			return;
		}
		if( digit < 8 )
		{
			HexData &= ((0x1L << (digit*4)) -1);		/* 上位桁を落とす	*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Convert to Hex String																		*/
/*--------------------------------------------------------------------------------------------------*/
		for( i=0; i < digit; i++, str++ )
		{
			x = (HexData >> ((digit-1-i)*4)) & 0x0F;
			if( x < 0xA ){ *str = (UCHAR)('0' + x);}
			else		 { *str = (UCHAR)('A' + (x - 0xA));}
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		画面表示データ作成処理メイン																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*										 01234567890123456											*/
/*		画面表示エリア(17桁 X 5行)		0															*/
/*										1															*/
/*										2															*/
/*										3															*/
/*										4															*/
/*																									*/
/****************************************************************************************************/
void	PrmCopyDispMain( void )
{
/*--------------------------------------------------------------------------------------------------*/
/*		画面表示データ作成処理																		*/
/*--------------------------------------------------------------------------------------------------*/
		switch( PrmCopyV.MainMode )
		{
		/*------------------------------------------------------------------------------------------*/
		/*		機能選択時																			*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_MAINMODE_MODE_SELECT:
			PrmCopyDispModeSelect();
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		機能実行時																			*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_MAINMODE_MODE_EXE:
			switch( PrmCopyV.SubModeSeq )
			{
				/*----------------------------------------------------------------------------------*/
				/*		ブロック選択																*/
				/*----------------------------------------------------------------------------------*/
				case PRMCOPY_SUBMODE_SEQ_BLOCK_SELECT:
					PrmCopyDispBlockSelect();
					break;
				/*----------------------------------------------------------------------------------*/
				/*		実行開始確認																*/
				/*----------------------------------------------------------------------------------*/
				case PRMCOPY_SUBMODE_SEQ_EXE_CONFIRM:
					PrmCopyDispExeConfirm();
					break;
				/*----------------------------------------------------------------------------------*/
				/*		処理実行中																	*/
				/*----------------------------------------------------------------------------------*/
				case PRMCOPY_SUBMODE_SEQ_EXE:
					switch( PrmCopyV.SubMode )
					{
						/*--------------------------------------------------------------------------*/
						/*		SAVE(Servo -> DgOp)実行中											*/
						/*--------------------------------------------------------------------------*/
						case PRMCOPY_SUBMODE_SAVE:
							PrmCopyDispSaveExe();
							break;
						/*--------------------------------------------------------------------------*/
						/*		LOAD(DgOp -> Servo)実行中											*/
						/*--------------------------------------------------------------------------*/
						case PRMCOPY_SUBMODE_LOAD:
							PrmCopyDispLoadExe();
							break;
						/*--------------------------------------------------------------------------*/
						/*		VERIFY(Servo <-> DgOp)実行中										*/
						/*--------------------------------------------------------------------------*/
						case PRMCOPY_SUBMODE_VERIFY:
							PrmCopyDispVerifyExe();
							break;
						/*--------------------------------------------------------------------------*/
						/*		DELETE(DgOp)実行中													*/
						/*--------------------------------------------------------------------------*/
						case PRMCOPY_SUBMODE_DELETE:
							PrmCopyDispDeleteExe();
							break;
						/*--------------------------------------------------------------------------*/
						default:
							break;
					}
					break;
				/*----------------------------------------------------------------------------------*/
				default:
					break;
			}
			break;
		/*------------------------------------------------------------------------------------------*/
		default:
			PrmCopyDispError();		/* エラーメッセージ画面　表示データ作成 */
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		エラーメッセージ画面　表示データ作成処理													*/
/*																									*/
/****************************************************************************************************/
void PrmCopyDispError(void)
{
PRM_COPY_TXBUF	*TxBuf = (PRM_COPY_TXBUF*)LopMsg.ResBuf;

/*--------------------------------------------------------------------------------------------------*/
/*		エラーメッセージ表示																		*/
/*--------------------------------------------------------------------------------------------------*/
		switch( PrmCopyV.ErrorNo )
		{
		/*------------------------------------------------------------------------------------------*/
		/*		実行不可エラー																		*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_ERROR_NOT_AVAILABLE:
			/* １行目にパラメータコピーメインタイトル "-COPY-" を表示 */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_1],
							(UCHAR *)PrmCopyDispTitleTbl[PRMCOPY_DISPTITLE_MAIN] );
			/* ４行目に "Not Available" を表示 */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_4],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_NOT_AVAILABLE] );
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		選択不可エラー																		*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_ERROR_NO_DATA:
			/* パラメータブロック選択後、画面ヘッダー表示 */
			PrmCopyDispBlockHeader();
			/* ４行目に "No Data" を表示 */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_4],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_NO_DATA] );
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		パラメータ型式エラー																*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_ERROR_PRM_UNMATCH:
			/* パラメータブロック選択後、画面ヘッダー表示 */
			PrmCopyDispBlockHeader();
			/* ３行目に "Unmatched" を表示 */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_3],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_UNMATCHED_RIGHT] );
			/* ４行目に "Parameters" を表示 */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_4],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_PARAMETERS_RIGHT] );
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		エラー発生なし																		*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_ERROR_NONE:
			break;
		/*------------------------------------------------------------------------------------------*/
		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		機能選択画面　表示データ作成処理															*/
/*																									*/
/****************************************************************************************************/
void	PrmCopyDispModeSelect(void)
{
PRM_COPY_TXBUF	*TxBuf = (PRM_COPY_TXBUF*)LopMsg.ResBuf;

/*--------------------------------------------------------------------------------------------------*/
/*		１行目にパラメータコピーメインタイトル "-COPY-" を表示										*/
/*--------------------------------------------------------------------------------------------------*/
		PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_1],
							(UCHAR *)PrmCopyDispTitleTbl[PRMCOPY_DISPTITLE_MAIN] );
/*--------------------------------------------------------------------------------------------------*/
/*		２行目に機能１ "1:SERVO -> OP" を表示														*/
/*--------------------------------------------------------------------------------------------------*/
		PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_2],
							(UCHAR *)PrmCopyDispModeStrTbl[PRMCOPY_DISPMODE_SAVE] );
/*--------------------------------------------------------------------------------------------------*/
/*		３行目に機能２ "2:OP -> SERVO" を表示														*/
/*--------------------------------------------------------------------------------------------------*/
		PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_3],
							(UCHAR *)PrmCopyDispModeStrTbl[PRMCOPY_DISPMODE_LOAD] );
/*--------------------------------------------------------------------------------------------------*/
/*		４行目に機能３ "3:VERIFY" を表示															*/
/*--------------------------------------------------------------------------------------------------*/
		PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_4],
							(UCHAR *)PrmCopyDispModeStrTbl[PRMCOPY_DISPMODE_VERIFY] );
/*--------------------------------------------------------------------------------------------------*/
/*		５行目に機能４ "4:DELETE" を表示															*/
/*--------------------------------------------------------------------------------------------------*/
		PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_5],
							(UCHAR *)PrmCopyDispModeStrTbl[PRMCOPY_DISPMODE_DELETE] );
/*--------------------------------------------------------------------------------------------------*/
/*		ブリンクセット																				*/
/*--------------------------------------------------------------------------------------------------*/
		TxBuf->DispLineBlink[PrmCopyV.SubMode+1] = TRUE;
/*--------------------------------------------------------------------------------------------------*/
		return;
}


/****************************************************************************************************/
/*																									*/
/*		ブロック選択画面　表示データ作成処理														*/
/*																									*/
/****************************************************************************************************/
void PrmCopyDispBlockSelect(void)
{
USHORT	i, block_list_idx;
PRM_COPY_TXBUF	*TxBuf = (PRM_COPY_TXBUF*)LopMsg.ResBuf;

/*--------------------------------------------------------------------------------------------------*/
/*		１行目に機能別タイトルを表示																*/
/*--------------------------------------------------------------------------------------------------*/
		PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_1],
						(UCHAR *)PrmCopyDispTitleTbl[PrmCopyV.SubMode+1] );
/*--------------------------------------------------------------------------------------------------*/
/*		２行目から５行目まで、ブロックリスト "0X:SGDS-XXX" を表示									*/
/*--------------------------------------------------------------------------------------------------*/
		block_list_idx = PrmCopyV.BlockNo;
		for( i = 0; i < PRMCOPY_DISP_LIST_MAX; i++ )
		{
			PrmCopyMakeBlockDispLine( TxBuf->DispLine[i+1], block_list_idx );
			if( (++block_list_idx) == PRMCOPY_MNG_INFO_BLOCK_MAX )
			{
				block_list_idx = 0;
			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		ブロックリスト表示の１行目(画面上は２行目)をブリンク										*/
/*--------------------------------------------------------------------------------------------------*/
		TxBuf->DispLineBlink[LCD_DISP_LINE_2] = TRUE;
		TxBuf->DispLineBlink[LCD_DISP_LINE_3] = 			/* @@ 2 line blink when SGD7W connected */
				Lpx_ChkDoubleBlock( PrmCopyP.TypeID ); 		
	
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		実行開始確認画面　表示データ作成処理														*/
/*																									*/
/****************************************************************************************************/
void PrmCopyDispExeConfirm(void)
{
PRM_COPY_TXBUF	*TxBuf = (PRM_COPY_TXBUF*)LopMsg.ResBuf;

/*--------------------------------------------------------------------------------------------------*/
/*		パラメータブロック選択後画面ヘッダー表示													*/
/*--------------------------------------------------------------------------------------------------*/
		PrmCopyDispBlockHeader();
/*--------------------------------------------------------------------------------------------------*/
/*			処理実行シーケンス																		*/
/*--------------------------------------------------------------------------------------------------*/
		switch( PrmCopyV.SubMode )
		{
		/*------------------------------------------------------------------------------------------*/
		/*		SAVE(Servo -> DgOp)実行中															*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SUBMODE_SAVE:
		/*------------------------------------------------------------------------------------------*/
		/*		VERIFY(Servo <-> DgOp)実行中														*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SUBMODE_VERIFY:
			/* ３行目に "Start :[READ]" を表示 */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_3],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_START_READ] );
			/* ４行目に "Return:[MODE]" を表示 */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_4],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_RETURN_MODE] );
			/* ３行目をブリンク */
			TxBuf->DispLineBlink[LCD_DISP_LINE_3] = TRUE;
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		LOAD(DgOp -> Servo)実行中															*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SUBMODE_LOAD:
			/* ３行目に "Start :[WRITE]" を表示 */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_3],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_START_WRITE] );
			/* ４行目に "Return:[MODE]" を表示 */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_4],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_RETURN_MODE] );
			/* ３行目をブリンク */
			TxBuf->DispLineBlink[LCD_DISP_LINE_3] = TRUE;
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		パラメータ削除(DgOp)実行中															*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SUBMODE_DELETE:
			/* ３行目に "FILE DELETE" を表示 */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_3],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_FILE_DELETE] );
			/* ４行目に "Start :[WRITE]" を表示 */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_4],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_START_WRITE] );
			/* ５行目に "Return:[MODE]" を表示 */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_5],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_RETURN_MODE] );
			/* ４行目をブリンク */
			TxBuf->DispLineBlink[LCD_DISP_LINE_4] = TRUE;
			break;
		/*------------------------------------------------------------------------------------------*/
		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		パラメータSAVE(Servo -> DgOp)実行中画面　表示データ作成処理									*/
/*																									*/
/****************************************************************************************************/
void	PrmCopyDispSaveExe(void)
{
PRM_COPY_TXBUF	*TxBuf = (PRM_COPY_TXBUF*)LopMsg.ResBuf;

/*--------------------------------------------------------------------------------------------------*/
/*		パラメータブロック選択後画面ヘッダー表示													*/
/*--------------------------------------------------------------------------------------------------*/
		PrmCopyDispBlockHeader();
/*--------------------------------------------------------------------------------------------------*/
/*		処理実行シーケンス																			*/
/*--------------------------------------------------------------------------------------------------*/
		switch( PrmCopyV.ExeSeq )
		{
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SAVE_SEQ_HEADER_DEL_REQ:	/* ヘッダ情報削除コマンド要求						*/
		case PRMCOPY_SAVE_SEQ_HEADER_DEL_RSP:	/* ヘッダ情報削除コマンド応答確認					*/
		case PRMCOPY_SAVE_SEQ_HEADER_SEND:		/* ヘッダ情報登録コマンド送信						*/
		case PRMCOPY_SAVE_SEQ_HEADER_RSP:		/* ヘッダ情報登録コマンド応答確認					*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SAVE_SEQ_PRM_SEND:			/* パラメータデータ伝送コマンド送信					*/
		case PRMCOPY_SAVE_SEQ_PRM_RSP:			/* パラメータデータ伝送コマンド応答確認				*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SAVE_SEQ_PRM_READ_REQ:		/* @@ パラメータデータEEPROM読出し					*/
		case PRMCOPY_SAVE_SEQ_PRM_READ_WAIT:	/* @@ パラメータデータEEPROM読出し					*/
		/*------------------------------------------------------------------------------------------*/
			if ( KpiGetLongTimer( &PrmCopyV.DispKeepTime ) < TIME_500MSEC )
			{
				/* ３行目に "Reading" を表示 */
				PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_3],
								(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_READING] );
				/* ４行目に "Parameters.." を表示 */
				PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_4],
								(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_PARAMETERS_DOT] );
				/* ５行目に "<<SERVO>>" を表示 */
				PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_5],
								(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_SERVO] );
			}
			else if ( KpiGetLongTimer( &PrmCopyV.DispKeepTime ) < TIME_1000MSEC )
			{
				/* ３行目に "Writing" を表示 */
				PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_3],
								(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_WRITING] );
				/* ４行目に "Parameters.." を表示 */
				PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_4],
								(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_PARAMETERS_DOT] );
				/* ５行目に "<< OP >>" を表示 */
				PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_5],
								(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_OP] );
			}
			else
			{
				KpiRstLongTimer( &PrmCopyV.DispKeepTime );
			}
			/* [Read]キー LED 点滅 */
			LopV.LedBlnkSw.read = TRUE;
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		デジオペ実行完了画面表示中															*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SAVE_SEQ_COMPLETE:
			/* ４行目に "Completed" を表示 */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_4],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_COMPLETED] );
			/* [Read]キー LED 点滅停止 */
			LopV.LedBlnkSw.read = FALSE;
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		デジオペ中断画面表示中																*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SAVE_SEQ_CANCEL:
			/* ４行目に "Canceled" を表示 */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_4],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_CANCELED] );
			/* [Read]キー LED 点滅停止 */
			LopV.LedBlnkSw.read = FALSE;
			break;
		/*------------------------------------------------------------------------------------------*/
		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		パラメータLOAD(DgOp -> Servo)実行中画面　表示データ作成処理									*/
/*																									*/
/****************************************************************************************************/
void	PrmCopyDispLoadExe(void)
{
PRM_COPY_TXBUF	*TxBuf = (PRM_COPY_TXBUF*)LopMsg.ResBuf;

/*--------------------------------------------------------------------------------------------------*/
/*		パラメータブロック選択後画面ヘッダー表示													*/
/*--------------------------------------------------------------------------------------------------*/
		PrmCopyDispBlockHeader();
/*--------------------------------------------------------------------------------------------------*/
/*		処理実行シーケンス																			*/
/*--------------------------------------------------------------------------------------------------*/
		switch( PrmCopyV.ExeSeq )
		{
		/*------------------------------------------------------------------------------------------*/
		/*		LOAD処理時、実行前再確認処理														*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_LOAD_SEQ_RECONFIRM:
			/* ３行目に "Reconfirm" を表示 */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_3],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_RECONFIRM] );
			/* ４行目に "OK :[DATA]" を表示 */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_4],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_OK_DATA] );
			/* ５行目に "NO :[MODE]" を表示 */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_5],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_NO_MODE] );
			/* ３行目をブリンク */
			TxBuf->DispLineBlink[LCD_DISP_LINE_3] = TRUE;
			break;
		/*------------------------------------------------------------------------------------------*/
		/* 		パラメータREADコマンド要求															*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_LOAD_SEQ_PRM_READ_REQ:
		/*------------------------------------------------------------------------------------------*/
		/* 		パラメータREADコマンド応答確認														*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_LOAD_SEQ_PRM_READ_RSP:
			if ( KpiGetLongTimer( &PrmCopyV.DispKeepTime ) < TIME_500MSEC )
			{
				/* ３行目に "Reading" を表示 */
				PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_3],
								(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_READING] );
				/* ４行目に "Parameters.." を表示 */
				PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_4],
								(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_PARAMETERS_DOT] );
				/* ５行目に "<< OP >>" を表示 */
				PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_5],
								(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_OP] );
			}
			else if ( KpiGetLongTimer( &PrmCopyV.DispKeepTime ) < TIME_1000MSEC )
			{
				/* ３行目に "Writing" を表示 */
				PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_3],
								(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_WRITING] );
				/* ４行目に "Parameters.." を表示 */
				PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_4],
								(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_PARAMETERS_DOT] );
				/* ５行目に "<<SERVO>>" を表示 */
				PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_5],
								(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_SERVO] );
			}
			else
			{
				KpiRstLongTimer( &PrmCopyV.DispKeepTime );
			}
			/* [Write]キー LED 点滅 */
			LopV.LedBlnkSw.write = TRUE;
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		デジオペ実行完了画面表示中															*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_LOAD_SEQ_COMPLETE:
			/* ４行目に "Completed" を表示 */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_4],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_COMPLETED] );
			/* [Write]キー LED 点滅停止 */
			LopV.LedBlnkSw.write = FALSE;
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		デジオペ中断画面表示中																*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_LOAD_SEQ_CANCEL:
			/* ４行目に "Canceled" を表示 */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_4],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_CANCELED] );
			/* [Write]キー LED 点滅停止 */
			LopV.LedBlnkSw.write = FALSE;
			break;
		/*------------------------------------------------------------------------------------------*/
		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		VERIFY(Servo <-> DgOp)実行中画面　表示データ作成処理										*/
/*																									*/
/****************************************************************************************************/
void	PrmCopyDispVerifyExe(void)
{
PRM_COPY_TXBUF	*TxBuf = (PRM_COPY_TXBUF*)LopMsg.ResBuf;

/*--------------------------------------------------------------------------------------------------*/
/*		処理実行シーケンスが、VERIFY結果 不一致画面表示中以外の時									*/
/*--------------------------------------------------------------------------------------------------*/
		if( PrmCopyV.ExeSeq != PRMCOPY_VERIFY_SEQ_UNMATCHED )
		{
			/* パラメータブロック選択後画面ヘッダー表示 */
			PrmCopyDispBlockHeader();
		}
/*--------------------------------------------------------------------------------------------------*/
/*		処理実行シーケンス																			*/
/*--------------------------------------------------------------------------------------------------*/
		switch( PrmCopyV.ExeSeq )
		{
		/*------------------------------------------------------------------------------------------*/
		/* 		パラメータREADコマンド要求中														*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_LOAD_SEQ_PRM_READ_REQ:
		/*------------------------------------------------------------------------------------------*/
		/* 		パラメータREADコマンド応答確認中													*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_LOAD_SEQ_PRM_READ_RSP:
			if ( KpiGetLongTimer( &PrmCopyV.DispKeepTime ) < TIME_500MSEC )
			{
				/* ３行目に "Reading" を表示 */
				PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_3],
								(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_READING] );
				/* ４行目に "Parameters.." を表示 */
				PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_4],
								(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_PARAMETERS_DOT] );
				/* ５行目に "<< OP >>" を表示 */
				PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_5],
								(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_OP] );
			}
			else if ( KpiGetLongTimer( &PrmCopyV.DispKeepTime ) < TIME_1000MSEC )
			{
				/* ３行目に "Verifying" を表示 */
				PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_3],
								(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_VERIFYING] );
				/* ４行目に "Parameters.." を表示 */
				PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_4],
								(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_PARAMETERS_DOT] );
				/* ５行目に "<<OP-SERVO>>" を表示 */
				PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_5],
								(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_OP_SERVO] );
			}
			else
			{
				KpiRstLongTimer( &PrmCopyV.DispKeepTime );
			}
			/* [Read]キー LED 点滅 */
			LopV.LedBlnkSw.read = TRUE;
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		デジオペ実行完了画面表示中															*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_LOAD_SEQ_COMPLETE:
			/* ４行目に "Completed" を表示 */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_4],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_COMPLETED] );
			/* [Read]キー LED 点滅停止 */
			LopV.LedBlnkSw.read = FALSE;
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		デジオペ中断画面表示中																*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_LOAD_SEQ_CANCEL:
			/* ４行目に "Canceled" を表示 */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_4],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_CANCELED] );
			/* [Read]キー LED 点滅停止 */
			LopV.LedBlnkSw.read = FALSE;
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		VERIFY結果 一致画面表示中															*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_VERIFY_SEQ_MATCHED:
			/* ３行目に "All Parameters" を表示 */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_3],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_ALL_PARAMETERS] );
			/* ４行目に "  are matched" を表示 */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_4],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_ARE_MATCHED] );
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		VERIFY結果 不一致画面表示中															*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_VERIFY_SEQ_UNMATCHED:
			PrmCopyDispVerifyUnmatchList();
			break;
		/*------------------------------------------------------------------------------------------*/
		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		VERIFY結果 不一致画面　表示データ作成処理													*/
/*																									*/
/****************************************************************************************************/
void	PrmCopyDispVerifyUnmatchList(void)
{
LONG	i;
LONG	PrmIdx;
LONG	list_tbl_idx;
UCHAR	*disp_str_buf;
PRM_COPY_TXBUF	*TxBuf = (PRM_COPY_TXBUF*)LopMsg.ResBuf;

/*--------------------------------------------------------------------------------------------------*/
/*		１行目に機能別タイトルを表示																*/
/*--------------------------------------------------------------------------------------------------*/
		PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_1],
						(UCHAR *)PrmCopyDispTitleTbl[PrmCopyV.SubMode+1] );
/*--------------------------------------------------------------------------------------------------*/
/*		２行目から５行目まで、不一致パラメータNOを表示												*/
/*--------------------------------------------------------------------------------------------------*/
		list_tbl_idx = PrmCopyV.VerifyErrListNo;
		for( i = 0; i < PRMCOPY_DISP_LIST_MAX; i++, list_tbl_idx++ )
		{
			/* 表示するデータがある */
			if( list_tbl_idx < PrmCopyV.VerifyErrTblCnt )
			{
				disp_str_buf = TxBuf->DispLine[i+1];
				PrmIdx = PrmCopyV.VerifyErrPrmIdxTbl[list_tbl_idx];
				if( list_tbl_idx < (PRMCOPY_VERIFY_ERR_DISP_TBL_MAX - 1) )
				{
					/* "Pn"セット */
					MlibCopyByteMemory( (UCHAR *)&PrmCopyDispStrTbl[PRMCOPY_DISPSTR_PN][0],
										(UCHAR *)disp_str_buf, 2 );
					/* 不一致パラメータが、ユーザーレベル１の場合 */
					if( LcdPdlTbl[PrmIdx].pPrmDef->AccessLevel == ACCLVL_USER1 )
					{
						/* パラメータ番号セット */
						ConvertHexToStr( LcdPdlTbl[PrmIdx].PnNo, (disp_str_buf+2), 3 );
					}
					else
					{
						/* "***"セット */
						MlibCopyByteMemory( (UCHAR *)&PrmCopyDispStrTbl[PRMCOPY_DISPSTR_ASTER][0],
											(UCHAR *)(disp_str_buf+2), 3 );
					}
				}
				else if( list_tbl_idx == (PRMCOPY_VERIFY_ERR_DISP_TBL_MAX - 1) )
				{
					/* "To Many Errors"セット */
					MlibCopyByteMemory( (UCHAR *)&PrmCopyDispStrTbl[PRMCOPY_DISPSTR_TO_MANY_ERRORS][0],
										(UCHAR *)disp_str_buf, 14 );
				}
			}
		/*------------------------------------------------------------------------------------------*/
			if( i == 1 )
			{
				/* ３行目に "Unmatched" を表示 */
				PrmCopyStrCopy( &TxBuf->DispLine[i+1][7],
								(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_UNMATCHED_LEFT] );
			}
			else if( i == 2 )
			{
				/* ４行目に "Parameters" を表示 */
				PrmCopyStrCopy( &TxBuf->DispLine[i+1][7],
								(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_PARAMETERS_LEFT] );
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		DELETE(DgOp)実行中画面　表示データ作成処理													*/
/*																									*/
/****************************************************************************************************/
void	PrmCopyDispDeleteExe(void)
{
PRM_COPY_TXBUF	*TxBuf = (PRM_COPY_TXBUF*)LopMsg.ResBuf;

/*--------------------------------------------------------------------------------------------------*/
/*		パラメータブロック選択後画面ヘッダー表示													*/
/*--------------------------------------------------------------------------------------------------*/
		PrmCopyDispBlockHeader( );
/*--------------------------------------------------------------------------------------------------*/
/*		処理実行シーケンス																			*/
/*--------------------------------------------------------------------------------------------------*/
		switch( PrmCopyV.ExeSeq )
		{
		/*------------------------------------------------------------------------------------------*/
		/*		ヘッダ情報削除コマンド要求															*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SAVE_SEQ_HEADER_DEL_REQ:
		/*------------------------------------------------------------------------------------------*/
		/*		ヘッダ情報削除コマンド応答確認														*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SAVE_SEQ_HEADER_DEL_RSP:
			/* ３行目に "Deleting" を表示 */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_3],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_DELETING] );
			/* ４行目に "Parameters.." を表示 */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_4],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_PARAMETERS_DOT] );
			/* [Write]キー LED 点滅 */
			LopV.LedBlnkSw.write = TRUE;
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		デジオペ実行完了画面表示中															*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SAVE_SEQ_COMPLETE:
			/* ４行目に "Completed" を表示 */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_4],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_COMPLETED] );
			/* [Write]キー LED 点滅停止 */
			LopV.LedBlnkSw.write = FALSE;
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		デジオペ中断画面表示中																*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_SAVE_SEQ_CANCEL:
			/* ４行目に "Canceled" を表示 */
			PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_4],
							(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_CANCELED] );
			/* [Write]キー LED 点滅停止 */
			LopV.LedBlnkSw.write = FALSE;
			break;
		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		パラメータブロック選択後画面 ヘッダー表示データ作成処理										*/
/*																									*/
/****************************************************************************************************/
void PrmCopyDispBlockHeader( void )
{
PRM_COPY_TXBUF	*TxBuf = (PRM_COPY_TXBUF*)LopMsg.ResBuf;

/*--------------------------------------------------------------------------------------------------*/
/*		１行目に機能別タイトルを表示																*/
/*--------------------------------------------------------------------------------------------------*/
		PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_1],
						(UCHAR *)PrmCopyDispTitleTbl[PrmCopyV.SubMode+1] );
/*--------------------------------------------------------------------------------------------------*/
/*		２行目に実行対象ブロック "0X:SGDS-XXX" を表示												*/
/*--------------------------------------------------------------------------------------------------*/
		PrmCopyMakeBlockDispLine( TxBuf->DispLine[LCD_DISP_LINE_2], PrmCopyV.BlockNo );
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		デジオペ初期画面　表示データ作成処理														*/
/*																									*/
/****************************************************************************************************/
void	PrmCopyDispInit( void )
{
PRM_COPY_TXBUF	*TxBuf = (PRM_COPY_TXBUF*)LopMsg.ResBuf;

/*--------------------------------------------------------------------------------------------------*/
/*		２行目に "File List Loading" を表示															*/
/*--------------------------------------------------------------------------------------------------*/
		PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_2],
						(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_FILE_LIST_LOADING] );
/*--------------------------------------------------------------------------------------------------*/
/*		３行目に "Please Wait..." を表示															*/
/*--------------------------------------------------------------------------------------------------*/
		PrmCopyStrCopy( TxBuf->DispLine[LCD_DISP_LINE_3],
						(UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_PLEASE_WAIT] );
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		ブロック表示データ作成処理																	*/
/*																									*/
/****************************************************************************************************/
void	PrmCopyMakeBlockDispLine( UCHAR *StrBuf, LONG BlockIdx )
{
LONG	i;
LONG	ServoId;

/*--------------------------------------------------------------------------------------------------*/
/*		ラインクリア																				*/
/*--------------------------------------------------------------------------------------------------*/
		PrmCopyStrCopy( StrBuf, (UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_SPACE] );
/*--------------------------------------------------------------------------------------------------*/
/*		ブロックNOセット																			*/
/*--------------------------------------------------------------------------------------------------*/
		PrmCopyStrCopy2( &StrBuf, (UCHAR *)PrmCopyDispBlockStrTbl[ BlockIdx ] );
/*--------------------------------------------------------------------------------------------------*/
/*		サーボパックアンマッチの場合 : "****"セット													*/
/*--------------------------------------------------------------------------------------------------*/
		if( PrmMngTbl[BlockIdx].FormatID == PRMCOPY_MNG_DATA_NONE )
		{
			PrmCopyStrCopy2( &StrBuf, (UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_ASTER] );
			return;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		サーボパック製品名セット(例"SGDS-")															*/
/*--------------------------------------------------------------------------------------------------*/
		for( i = 0; i < SVIDTBL_ENTRY_NUM; i++ )
		{
			if( (PrmMngTbl[BlockIdx].ServoID & 0x00FF) == SvIdTbl[i].Id )
			{
				ServoId = i;	/* アンプ容量表示文字列セットで使用*/
				PrmCopyStrCopy2( &StrBuf, (UCHAR *)&SvIdTbl[i].Name );
				break;
			}
			else if( i >= (SVIDTBL_ENTRY_NUM - 1) )		/* 検索不一致 : "????"セット			*/
			{
				ServoId = i;
				PrmCopyStrCopy2( &StrBuf, (UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_QUESTION_4] );
			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		"-" セット																					*/
/*--------------------------------------------------------------------------------------------------*/
		PrmCopyStrCopy2( &StrBuf, (UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_HIFUN] );
/*--------------------------------------------------------------------------------------------------*/
/*		アンプ容量表示文字列セット																	*/
/*--------------------------------------------------------------------------------------------------*/
		for( i = 0; i < AMP_WATT_TBL_IDX_MAX; i++ )
		{
			if(( (PrmMngTbl[BlockIdx].AmpVolt & 0x000f) == AmpWattTbl[i].AmpVolt ) &&
			   ( PrmMngTbl[BlockIdx].AmpWatt == AmpWattTbl[i].AmpWatt ))
			{
				/*----------------------------------------------------------------------------------*/
				/*	製品により[W]表示と[Arms]記号表示を分ける 										*/
				/*----------------------------------------------------------------------------------*/
				if( SvIdTbl[ServoId].Disp == WATT)			/* [W]表示								*/
				{
					PrmCopyStrCopy2( &StrBuf, (UCHAR *)&AmpWattTbl[i].DispStrW[0] );
				}
				else if( SvIdTbl[ServoId].Disp == AMPERE)	/* [Arms]記号表示						*/
				{
					PrmCopyStrCopy2( &StrBuf, (UCHAR *)&AmpWattTbl[i].DispStr[0] );
				}
				else										/* その他："???"セット					*/
				{
					PrmCopyStrCopy2( &StrBuf, (UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_QUESTION_3] );
				}
				break;
			}
			else if( i >= (AMP_WATT_TBL_IDX_MAX - 1) )		/* 検索不一致 : "???"セット				*/
			{
				PrmCopyStrCopy2( &StrBuf, (UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_QUESTION_3] );
			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		入力電圧表示文字列セット																	*/
/*--------------------------------------------------------------------------------------------------*/
		for( i = 0; i < AMP_VOLT_TBL_IDX_MAX; i++ )
		{
			if( (PrmMngTbl[BlockIdx].AmpVolt & 0x000f) == AmpVoltTbl[i].Code )
			{
				PrmCopyStrCopy2( &StrBuf, (UCHAR *)&AmpVoltTbl[i].DispStr[0] );
				break;
			}
			else if( i >= (AMP_VOLT_TBL_IDX_MAX - 1) )		/* 検索不一致 : "?"セット				*/
			{
				PrmCopyStrCopy2( &StrBuf, (UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_QUESTION_1] );
			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		インターフェイス仕様表示文字列セット														*/
/*--------------------------------------------------------------------------------------------------*/
		for( i = 0; i < SVIDTBL_ENTRY_NUM; i++ )
		{
			if( (PrmMngTbl[BlockIdx].ServoID & 0x00FF) == SvIdTbl[i].Id )
			{
				PrmCopyStrCopy2( &StrBuf, (UCHAR *)&SvIdTbl[i].IfTyp );
				break;
			}
			else if( i >= (SVIDTBL_ENTRY_NUM - 1) )		/* 検索不一致 : "????"セット			*/
			{
				PrmCopyStrCopy2( &StrBuf, (UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_QUESTION_4] );
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		文字列コピー処理																			*/
/*																									*/
/****************************************************************************************************/
void	PrmCopyStrCopy( UCHAR *ToStr, UCHAR *FromStr )
{
		while( *FromStr != 0 )
		{
			*(ToStr++) = *(FromStr++);
		}
}



/****************************************************************************************************/
/*																									*/
/*		文字列コピー処理(コピー先のアドレスを更新して返す)											*/
/*																									*/
/****************************************************************************************************/
void	PrmCopyStrCopy2( UCHAR **ToStr, UCHAR *FromStr )
{
		while( *FromStr != 0)
		{
			*((*ToStr)++) = *(FromStr++);
		}
}



/****************************************************************************************************/
/*																									*/
/*		デジオペパラメータブロック管理情報取得処理(デジオペ接続時に１回行う)						*/
/*																									*/
/****************************************************************************************************/
LONG	RpxLcdPrmInfoGet( void )
{
LONG	i;
SHORT	rc;
LONG	ModeChg = FALSE;

/*--------------------------------------------------------------------------------------------------*/
/*		開始処理																					*/
/*--------------------------------------------------------------------------------------------------*/
		PrmCopyCmdInit();						/* デジオペ通信初期処理								*/
		PrmCopyNormalCmdSet();					/* 通信作業領域に、通常コマンド(0x60)セット			*/
		PrmCopyDispInit();						/* デジオペ初期画面表示データセット					*/
/*--------------------------------------------------------------------------------------------------*/
/*		処理実行シーケンス																			*/
/*--------------------------------------------------------------------------------------------------*/
		switch( PrmCopyV.ExeSeq )
		{
		/*------------------------------------------------------------------------------------------*/
		/*		実行初期処理																		*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_INFOGET_SEQ_INIT:
			/* 通信リトライカウントクリア */
			PrmCopyV.RetryCnt = 0;
			PrmCopyV.BlockNo = 0;
			PrmCopyV.BlockMngAreaNo = 0;
			/* パラメータブロック管理情報READコマンド要求へ */
			PrmCopyV.ExeSeq = PRMCOPY_INFOGET_SEQ_MNG_READ_REQ;
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		デジオペパラメータブロック管理情報LOADコマンド要求									*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_INFOGET_SEQ_MNG_READ_REQ:
			/* 通信作業領域にLOADコマンド(0x61)セット */
			PrmCopyLoadMngInfoCmdSet( PrmCopyV.BlockNo, PrmCopyV.BlockMngAreaNo );
			/* パラメータブロック管理情報READコマンド応答確認へ */
			PrmCopyV.ExeSeq = PRMCOPY_INFOGET_SEQ_MNG_READ_RSP;
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		コマンド応答確認																	*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_INFOGET_SEQ_MNG_READ_RSP:
			/* デジオペ通信コマンド応答確認処理 */
			rc = PrmCopyCmdRspCheck( LCD_CMD_LOAD );
			/*----- 受信正常 -----------------------------------------------------------------------*/
			if( rc == RC_OK )
			{
				/* 通信リトライカウントクリア */
				PrmCopyV.RetryCnt = 0;
				/* デジオペ パラメータブロック管理情報受信データ取得 */
				PrmCopyPrmMngTblOp( PRMCOPY_MNG_INFODATA_RECIEVE, PrmCopyV.BlockNo );
			  /*------------------------------------------------------------------------------------*/
				if( PrmCopyV.BlockMngAreaNo == 0 )
				{
					PrmCopyV.BlockMngAreaNo = 1;
				}
				else
				{
					PrmCopyV.BlockNo++;
					PrmCopyV.BlockMngAreaNo = 0;
				}
			  /*------------------------------------------------------------------------------------*/
				/* パラメータ管理情報を全て取得した */
				if( PrmCopyV.BlockNo >= PRMCOPY_MNG_INFO_BLOCK_MAX )
				{
					/* 管理情報取得完了へ */
					PrmCopyV.ExeSeq = PRMCOPY_INFOGET_SEQ_COMPLETE;
				}
				else
				{
					/* 引き続き次のデータ伝送を行う */
					PrmCopyV.ExeSeq = PRMCOPY_INFOGET_SEQ_MNG_READ_REQ;
				}
			}
			/*----- アラームコードを受信した -------------------------------------------------------*/
			else if( (rc == RC_NG) || (rc == RC_PASS) )
			{
				/* デジオペパラメータブロック管理情報READコマンド送信へ戻る */
				PrmCopyV.ExeSeq = PRMCOPY_INFOGET_SEQ_MNG_READ_REQ;
			}
			/*----- リトライ回数オーバー -----------------------------------------------------------*/
			else if( rc == RC_RETRY_OVER )
			{
				/* デジオペ パラメータブロック管理情報クリア */
				for( i = 0; i < PRMCOPY_MNG_INFO_BLOCK_MAX; i++ )
				{
					PrmCopyPrmMngTblOp( PRMCOPY_MNG_INFODATA_CLEAR, i );
				}
				/* 処理中断へ */
				PrmCopyV.ExeSeq = PRMCOPY_INFOGET_SEQ_CANCEL;
			}
			/*----- 受信したコマンドコードが送信したコードと違う -----------------------------------*/
			else
			{
				/* 引き続き受信待ち */
			}
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		実行完了																			*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_INFOGET_SEQ_COMPLETE:
		/*------------------------------------------------------------------------------------------*/
		/*		実行中断																			*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_INFOGET_SEQ_CANCEL:
			ModeChg = TRUE;
			break;
		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		終了処理																					*/
/*--------------------------------------------------------------------------------------------------*/
	 	PrmCopySetSendBuf();		/* 通信作業領域から、送信バッファへデータセット					*/
		PrmEntNum.SvPrm = LCDPDLTBL_ENTNUM; /* PrmCopyGetEntNum( &PnPrmTbl[0], KsysP.PnPrmTblEntNum ); */
/*--------------------------------------------------------------------------------------------------*/
		return( ModeChg );
}



/****************************************************************************************************/
/*																									*/
/*		デジオペ通信コマンド応答確認処理															*/
/*																									*/
/****************************************************************************************************/
SHORT	PrmCopyCmdRspCheck( USHORT SendCmd )
{
UCHAR	RecvCmd = PRMCOPY_CMD_CODE_GET( );				/* 受信コマンドコード取得					*/

/*--------------------------------------------------------------------------------------------------*/
/*		Check Receive Cmd																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( (RecvCmd & LCD_RCVCMD_MASK) != SendCmd )	/* 受信コマンド != 送信コマンド				*/
		{
			return( RC_PASS );
		}
		else if( (RecvCmd & LCD_RCVCMD_ALARM_BIT) )		/* アラームビットが立っている				*/
		{
			if( PrmCopyV.RetryCnt > RETRY_CNT_MAX )
			{
				return( RC_RETRY_OVER );				/* リトライ回数が、許容範囲を越えた			*/
			}
			else
			{
				PrmCopyV.RetryCnt++;					/* 通信リトライカウントインクリメント		*/
			}
			return( RC_NG );
		}
/*--------------------------------------------------------------------------------------------------*/
		return( RC_OK );								/* 正常										*/
}



/****************************************************************************************************/
/*																									*/
/*		デジオペパラメータ管理情報操作																*/
/*																									*/
/****************************************************************************************************/
void	PrmCopyPrmMngTblOp( UCHAR flag, USHORT block_no )
{
PRMDATA *pPrm = LopMsg.pAxRsc->UniPrms->Prm;			/* TODO: 後で消す */
/*--------------------------------------------------------------------------------------------------*/
/*		Switch by flag																				*/
/*--------------------------------------------------------------------------------------------------*/
		switch( flag )
		{
		/*------------------------------------------------------------------------------------------*/
		/*		管理情報クリア																		*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_MNG_INFODATA_CLEAR:
			PrmMngTbl[block_no].ServoID		= PRMCOPY_MNG_DATA_NONE;
			PrmMngTbl[block_no].AmpWatt		= PRMCOPY_MNG_DATA_NONE;
			PrmMngTbl[block_no].AmpVolt		= PRMCOPY_MNG_DATA_NONE;
			PrmMngTbl[block_no].FormatID 	= PRMCOPY_MNG_DATA_NONE;
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		管理情報セット																		*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_MNG_INFODATA_SET:
			PrmMngTbl[block_no].ServoID		= PrmCopyP.TypeID;
			PrmMngTbl[block_no].AmpWatt		= pPrm->ampw;
			PrmMngTbl[block_no].AmpVolt		= (USHORT)pPrm->styp.b.h;
			PrmMngTbl[block_no].FormatID 	= EEP_FORMAT_ID; /* EepInfo.YecFmtId; */
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		管理情報受信データ取得																*/
		/*------------------------------------------------------------------------------------------*/
		case PRMCOPY_MNG_INFODATA_RECIEVE:
			if( PrmCopyV.BlockMngAreaNo == 0 )
			{
				PrmMngTbl[block_no].ServoID  = 0xFFFF;
				PrmMngTbl[block_no].AmpWatt  = PRMCOPY_CMD_DATA_GET( 0 );
				PrmMngTbl[block_no].AmpVolt  = PRMCOPY_CMD_DATA_GET( 1 );
				PrmMngTbl[block_no].FormatID = PRMCOPY_CMD_DATA_GET( 2 );
			/*------------------------------------------------------------------------------------*/
				if( PrmMngTbl[block_no].AmpWatt != 0xFFFF )
				{
					PrmMngTbl[block_no].ServoID = SVID_SGDS;
				}
			}
			else if( PrmMngTbl[block_no].ServoID == 0xFFFF )
			{
				if( PrmMngTbl[block_no].FormatID != 0xFFFF )
				{
					PrmMngTbl[block_no].ServoID  = PRMCOPY_CMD_DATA_GET( 0 );
					PrmMngTbl[block_no].AmpWatt  = PRMCOPY_CMD_DATA_GET( 1 );
					PrmMngTbl[block_no].AmpVolt  = PRMCOPY_CMD_DATA_GET( 2 );
					PrmMngTbl[block_no].FormatID = PRMCOPY_CMD_DATA_GET( 3 );
				}
			}
			break;
		/*------------------------------------------------------------------------------------------*/
		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		デジオペ通信初期化処理																		*/
/*																									*/
/****************************************************************************************************/
void	PrmCopyCmdInit( void )
{
LONG	i;
PRM_COPY_TXBUF	*TxBuf = (PRM_COPY_TXBUF*)LopMsg.ResBuf;

/*--------------------------------------------------------------------------------------------------*/
/*		送信バッファクリア																			*/
/*--------------------------------------------------------------------------------------------------*/
		MlibResetByteMemory( (void *)LopMsg.ResBuf, sizeof(PRM_COPY_TXBUF) );
/*--------------------------------------------------------------------------------------------------*/
/*		デジオペ通信作業領域初期化処理																*/
/*--------------------------------------------------------------------------------------------------*/
		for( i = 0; i < PRMCOPY_DISP_LINE_MAX; i++ )
		{
			PrmCopyStrCopy( TxBuf->DispLine[i], (UCHAR *)PrmCopyDispStrTbl[PRMCOPY_DISPSTR_SPACE] );
			TxBuf->DispLineBlink[i] = FALSE;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		デジオペ通信  通常コマンド(0x60)作業領域セット												*/
/*																									*/
/****************************************************************************************************/
void	PrmCopyNormalCmdSet( void )
{
PRM_COPY_TXBUF	*TxBuf = (PRM_COPY_TXBUF*)LopMsg.ResBuf;
#define LCD_CMD_NORMAL_MSGLEN  LCD_TX_NORMAL_TOTAL_SIZE + LCD_MSG_CHKSUM_SIZE

/*--------------------------------------------------------------------------------------------------*/
/*		ヘッダ設定																					*/
/*--------------------------------------------------------------------------------------------------*/
		TxBuf->CmdCode = LCD_CMD_NOR;				/* 通常コマンド(0x60)セット					*/
		TxBuf->Length  = LCD_CMD_NORMAL_MSGLEN;			/* 送信バッファ長セット						*/
		TxBuf->DispSts[0] = 5;							/* 表示形式セット							*/
/*--------------------------------------------------------------------------------------------------*/
	return;
}



/****************************************************************************************************/
/*																									*/
/*		デジオペ通信  LOADコマンド(0x61)作業領域セット												*/
/*																									*/
/****************************************************************************************************/
void	PrmCopyLoadCmdSet( USHORT DopEepAddr, USHORT DataNum )
{
PRM_COPY_TXBUF	*TxBuf = (PRM_COPY_TXBUF*)LopMsg.ResBuf;
#define LCD_CMD_LOAD_MSGLEN  LCD_TX_MSG_LOADSUM_IDX + LCD_MSG_CHKSUM_SIZE

/*--------------------------------------------------------------------------------------------------*/
/*		ヘッダ設定																					*/
/*--------------------------------------------------------------------------------------------------*/
		TxBuf->CmdCode = LCD_CMD_LOAD;					/* LOADコマンド(0x61)セット 				*/
		TxBuf->Length  = LCD_CMD_LOAD_MSGLEN;			/* 送信メッセージ長セット					*/
		TxBuf->EepAddr = DopEepAddr;					/* デジオペEEPROMアドレスセット 			*/
		TxBuf->DataNum = DataNum;						/* データ数セット 							*/
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		デジオペ通信  SAVEコマンド(0x62)作業領域セット												*/
/*																									*/
/****************************************************************************************************/
void	PrmCopySaveCmdSet( USHORT DopEepAddr, USHORT DataNum, USHORT *pData )
{
LONG	i;
PRM_COPY_TXBUF	*TxBuf = (PRM_COPY_TXBUF*)LopMsg.ResBuf;
#define LCD_CMD_SAVE_DATLEN  (DataNum * LCD_MSG_DATA_UNIT_SIZE)
#define LCD_CMD_SAVE_MSGLEN  LCD_TX_MSG_DATA_IDX + LCD_MSG_CHKSUM_SIZE + LCD_CMD_SAVE_DATLEN

/*--------------------------------------------------------------------------------------------------*/
/*		ヘッダ設定																					*/
/*--------------------------------------------------------------------------------------------------*/
		TxBuf->CmdCode = LCD_CMD_SAVE;					/* SAVEコマンド(0x62)セット 				*/
		TxBuf->Length  = LCD_CMD_SAVE_MSGLEN;			/* 送信メッセージ長セット					*/
		TxBuf->EepAddr = DopEepAddr;					/* デジオペEEPROMアドレスセット 			*/
		TxBuf->DataNum = DataNum;						/* データ数セット 							*/
/*--------------------------------------------------------------------------------------------------*/
/*		データセット(MAX:8)																			*/
/*--------------------------------------------------------------------------------------------------*/
		for( i = 0; i < DataNum; i++ )
		{
			TxBuf->Data[i] = *(pData++);
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		デジオペ通信 : 管理情報ロードコマンドデータ設定												*/
/*																									*/
/****************************************************************************************************/
void	PrmCopyLoadMngInfoCmdSet( LONG BlockNo, LONG BlockMngAreaNo )
{
USHORT			DataNum, EepSize, EepBase;
PRM_COPY_TXBUF	*TxBuf = (PRM_COPY_TXBUF*)LopMsg.ResBuf;

/*--------------------------------------------------------------------------------------------------*/
/*		管理情報領域０ : 既存管理情報(SGDS互換)														*/
/*--------------------------------------------------------------------------------------------------*/
		if( BlockMngAreaNo == 0 )
		{
			DataNum = 3;										/* ロードデータ数					*/
			EepSize = PRMCOPY_MNG_INFO_SIZ0;					/* EEPROM管理情報サイズ				*/
			EepBase = PRMCOPY_MNG_INFO_TOP0;					/* EEPROMベースアドレス				*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		管理情報領域１：拡張管理情報(SVOS追加)														*/
/*--------------------------------------------------------------------------------------------------*/
		else
		{
			DataNum = 8;										/* ロードデータ数					*/
			EepSize = PRMCOPY_MNG_INFO_SIZ1;					/* EEPROM管理情報サイズ				*/
			EepBase = PRMCOPY_MNG_INFO_TOP1;					/* EEPROMベースアドレス				*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		送信バッファデータ設定																		*/
/*--------------------------------------------------------------------------------------------------*/
		TxBuf->CmdCode = LCD_CMD_LOAD;							/* ロードコマンドセット				*/
		TxBuf->EepAddr = (USHORT)(EepBase + (EepSize*BlockNo));	/* EEPROMアドレスセット				*/
		TxBuf->DataNum = DataNum;								/* ロードデータ数セット				*/
		TxBuf->Length  = LCD_TX_MSG_LOADSUM_IDX + LCD_MSG_CHKSUM_SIZE;
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		デジオペ通信 : 管理情報保存コマンドデータ設定												*/
/*																									*/
/****************************************************************************************************/
void	PrmCopySaveMngInfoCmdSet( LONG BlockNo, LONG BlockMngAreaNo )
{
USHORT			DataNum, EepSize, EepBase;
PRM_COPY_TXBUF	*TxBuf = (PRM_COPY_TXBUF*)LopMsg.ResBuf;

/*--------------------------------------------------------------------------------------------------*/
/*		管理情報領域０ : 既存管理情報(SGDS互換)														*/
/*--------------------------------------------------------------------------------------------------*/
/*		注意：SGDSが、"???"を表示するように、AmpWatt,AmpVoltには、0xFFFFを書き込む					*/
/*--------------------------------------------------------------------------------------------------*/
		if( BlockMngAreaNo == 0 )
		{
			DataNum = 3;										/* 保存データ数						*/
			EepSize = PRMCOPY_MNG_INFO_SIZ0;					/* EEPROM管理情報サイズ				*/
			EepBase = PRMCOPY_MNG_INFO_TOP0;					/* EEPROMベースアドレス				*/
		/*------------------------------------------------------------------------------------------*/
			TxBuf->Data[0] = 0xFFFF;							/* データセット(MAX:8)				*/
			TxBuf->Data[1] = 0xFFFF;							/* データセット(MAX:8)				*/
			TxBuf->Data[2] = PrmMngTbl[BlockNo].FormatID;		/* データセット(MAX:8)				*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		管理情報領域１：拡張管理情報(SVOS追加)														*/
/*--------------------------------------------------------------------------------------------------*/
		else
		{
			DataNum = 8;										/* 保存データ数						*/
			EepSize = PRMCOPY_MNG_INFO_SIZ1;					/* EEPROM管理情報サイズ				*/
			EepBase = PRMCOPY_MNG_INFO_TOP1;					/* EEPROMベースアドレス				*/
		/*------------------------------------------------------------------------------------------*/
			TxBuf->Data[0] = PrmMngTbl[BlockNo].ServoID;		/* データセット(MAX:8)				*/
			TxBuf->Data[1] = PrmMngTbl[BlockNo].AmpWatt;		/* データセット(MAX:8)				*/
			TxBuf->Data[2] = PrmMngTbl[BlockNo].AmpVolt;		/* データセット(MAX:8)				*/
			TxBuf->Data[3] = PrmMngTbl[BlockNo].FormatID;		/* データセット(MAX:8)				*/
			TxBuf->Data[4] = 0xFFFF;							/* データセット(MAX:8)				*/
			TxBuf->Data[5] = 0xFFFF;							/* データセット(MAX:8)				*/
			TxBuf->Data[6] = 0xFFFF;							/* データセット(MAX:8)				*/
			TxBuf->Data[7] = 0xFFFF;							/* データセット(MAX:8)				*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		送信バッファデータ設定																		*/
/*--------------------------------------------------------------------------------------------------*/
		TxBuf->CmdCode = LCD_CMD_SAVE;							/* 保存コマンドセット				*/
		TxBuf->EepAddr = (USHORT)(EepBase + (EepSize*BlockNo));	/* EEPROMアドレスセット				*/
		TxBuf->DataNum = DataNum;								/* 保存データ数セット				*/
		TxBuf->Length  = LCD_TX_MSG_DATA_IDX
						 + (DataNum * LCD_MSG_DATA_UNIT_SIZE) + LCD_MSG_CHKSUM_SIZE;
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		通信作業領域から、送信バッファへデータセット処理											*/
/*																									*/
/****************************************************************************************************/
void	PrmCopySetSendBuf( void )
{
LONG	i;
PRM_COPY_TXBUF	*TxBuf = (PRM_COPY_TXBUF*)LopMsg.ResBuf;

/*--------------------------------------------------------------------------------------------------*/
/*		コマンドセット																				*/
/*--------------------------------------------------------------------------------------------------*/
		LopV.CurMsgCmd = TxBuf->CmdCode;
/*--------------------------------------------------------------------------------------------------*/
/*		画面表示データセット																		*/
/*--------------------------------------------------------------------------------------------------*/
		for( i = 0; i < PRMCOPY_DISP_LINE_MAX; i++ )
		{
			if( TxBuf->DispLineBlink[i] )
			{
				RpxLcdSetBlnk( i, 0, LCD_DISP_LINE_CHAR_MAX, (UCHAR *)TxBuf );
			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		応答データエンディアン変換 (TxBuf->Length/ChkSumは、LcdOp.cで処理)							*/
/*--------------------------------------------------------------------------------------------------*/
#if		defined( CCSW_BIG_ENDIAN )
		switch( TxBuf->CmdCode )
		{
		case LCD_CMD_LOAD:	/* デジオペ EEPROM読出し	*/
			 TxBuf->EepAddr = MlibCnvLittle16( TxBuf->EepAddr );
			 TxBuf->DataNum = MlibCnvLittle16( TxBuf->DataNum );
			 break;
		case LCD_CMD_SAVE:	/* デジオペ EEPROM書込み	*/
			 for( i=0; i < TxBuf->DataNum; i++ )
			 {
				TxBuf->Data[i] = MlibCnvLittle16( TxBuf->Data[i] );
			 }
		   /*---------------------------------------------------------------------------------------*/
			 TxBuf->EepAddr = MlibCnvLittle16( TxBuf->EepAddr );
			 TxBuf->DataNum = MlibCnvLittle16( TxBuf->DataNum );
			 break;
		default:
			 break;
		}
#endif
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		パラメータテーブルから、ユーザレベルの登録数を算出											*/
/*																									*/
/****************************************************************************************************/
#if 0
USHORT	PrmCopyGetEntNum( const PRMTBL *PrmTbl, const USHORT EntNum )
{
LONG	i;
CPRMDEF	*pPrmDef;
USHORT	PrmNum = 0;

/*--------------------------------------------------------------------------------------------------*/
/*		Count User Parameter																		*/
/*--------------------------------------------------------------------------------------------------*/
		for ( i=0; i < EntNum; i++ )
		{
			pPrmDef = PrmTbl[i].PrmDefPtr;
			if(	LPX_CHK_EEP_PRM( pPrmDef ) )
			{
				ULONG	AccLvl = pPrmDef->AccessLevel;
				if( (AccLvl == ACCLVL_USER1) || (AccLvl == ACCLVL_USER2) )
				{
					if( pPrmDef->Attr.VarIsLong ){ PrmNum += 2;}else{ PrmNum += 1;}
				}
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		return( PrmNum );
}
#endif



/****************************************************************************************************/
/*																									*/
/*		パラメータEEPROM読み出し処理																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	概要: 指定されるパラメータをEEPROMからWORDサイズ1個読み出す。									*/
/*		  Reads the specified Parameter value from EEPROM.											*/
/*	注意: 上位からのサイズ指定はない。パラメータコピー機能専用処理									*/
/*																									*/
/****************************************************************************************************/
#if 0
LONG	PrmCopyPrmRead( USHORT Index, const PRMTBL *PrmTbl, const USHORT EntNum, USHORT *ResData )
{
ULONG	PrmData;
CPRMDEF *pPrmDef;

/*--------------------------------------------------------------------------------------------------*/
/*		Check Index																					*/
/*--------------------------------------------------------------------------------------------------*/
		if( Index >= (2 * EntNum) )
		{
			return( PERR_NO );				/* パラメータ番号エラー */
		}
/*--------------------------------------------------------------------------------------------------*/
/*		EEPROMデータ読み出し																		*/
/*--------------------------------------------------------------------------------------------------*/
		pPrmDef = LPX_GET_PrmDefPtr( Index );
		PrmData = KriReadEepromPrm( pPrmDef );
/*--------------------------------------------------------------------------------------------------*/
/*		EEPROM読出データ設定																		*/
/*--------------------------------------------------------------------------------------------------*/
		if( pPrmDef->Attr.VarIsLong != TRUE )
		{
			*ResData = (USHORT)PrmData;
		}
		else if( LPX_GET_HiWdFlag( Index ) == 0 )
		{
			*ResData = GET_LWD_EEPDATA( PrmData );
		}
		else
		{
			*ResData = GET_HWD_EEPDATA( PrmData );
		}
/*--------------------------------------------------------------------------------------------------*/
		return( OK );
}
#endif







/***************************************** end of file **********************************************/
