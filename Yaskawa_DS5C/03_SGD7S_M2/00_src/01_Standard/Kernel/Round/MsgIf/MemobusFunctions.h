#ifndef	_MEMOBUS_FUNCTIONS_H /* 再読み込み防止用 */
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		MemobusFunctions.h : Memobus処理関数定義													*/
/*																									*/
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
/*																									*/
/*																									*/
/*	Note	:	初版	2010.09.03	T.Kira	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
/* MEMOBUS メッセージ	製品情報読み出し用コンパイルスイッチ */
//#define _INFO_ID_READ_CHECK_	1	/* <S061>	 */	/* <S205> Delete */


/****************************************************************************************************/
/*																									*/
/*		Macro Definition																			*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		MEMOBUS メッセージ	ファンクションコード													*/
/*--------------------------------------------------------------------------------------------------*/
#define	MBUS_LBAK_08H			0x08			/* ループバックコマンド						*/
#define	MBUS_FC_42H				0x42			/* MEMOBUS メッセージファンクションコード			*/
#define	MBUS_SYSDL_50H			0x50			/* MEMOBUS ファームダウンロードコード			*/
#define	MBUS_RELAY_51H 			0x51			/* 中継コマンド										*/


/*--------------------------------------------------------------------------------------------------*/
/*		MEMOBUS メッセージ	サブファンクションコード												*/
/*--------------------------------------------------------------------------------------------------*/
#define	MBUS_RD_CMEM_42H		0x01			/* メモリ内容読み出し（連続）						*/
#define	MBUS_WR_CMEM_42H		0x02			/* メモリへの書き込み（連続）						*/
#define	MBUS_RD_NCMEM_42H		0x03			/* メモリ内容読み出し（非連続）						*/
#define	MBUS_WR_NCMEM_42H		0x04			/* メモリへの書き込み（非連続）						*/
#define	MBUS_RD_MAXSIZE_42H		0x11			/* 最大サイズ読み出し								*/
//#ifdef __INFO_ID_READ_CHECK__	/* <S205> Delete */
#define	MBUS_RD_INFO_42H		0x7f			/* 製品情報読み出し									*/	/* <S061> */	/* <S205> :0xff(暫定) ⇒ 0x7fへ変更 */
												/* (サブファンクションコードは未定であるため		*/	/* <S061> */
												/* 仮の値としています。)							*/	/* <S061> */
//#endif	/* <S205> Delete */

#if 0	//FirmDownload.hに移動
/*--------------------------------------------------------------------------------------------------*/
/*		MEMOBUS ファームダウンロード	サブファンクションコード									*/
/*--------------------------------------------------------------------------------------------------*/
#define	MBUS_DL_ID_RD			0x0000			/* ID情報読み出し										*/
#define	MBUS_DL_SWVER_RD		0x0001			/* バージョン読出し										*/
#define	MBUS_DL_WAY_RD			0x0002			/* プログラム書込み条件取得								*/
#define	MBUS_DL_START			0x0003			/* ダウンロード開始指令									*/
#define	MBUS_DL_PRGDEL			0x0004			/* ブロック消去指令										*/
#define	MBUS_DL_PRGWR			0x0005			/* プログラム書込み指令									*/
#define	MBUS_DL_DELSND			0x0006			/* ブロック消去情報指令(消去情報転送のみ.実消去しない)		*/
#define	MBUS_DL_PRGSND			0x0007			/* プログラム送信指令(プログラム送信のみ.実書込しない)		*/
#define	MBUS_DL_PRGDELA			0x0008			/* ブロック一括消去指令									*/
#define	MBUS_DL_PRGWRA			0x0009			/* プログラム一括書込み指令								*/
#define	MBUS_DL_END				0x000A			/* ダウンロード終了指令									*/
#define	MBUS_DL_PRGRD			0x000B			/* プログラム読出し指令									*/
#define	MBUS_DL_REBOOT			0x000C			/* プログラム再起動指令									*/
#define	MBUS_DL_SIFCHG			0x000D			/* シリアルＩＦ速度変更									*/
#define	MBUS_DL_HIS_RD			0x000E			/* ダウンロード履歴読出し								*/
#define	MBUS_DL_HIS_WR			0x000F			/* ダウンロード履歴書込み								*/
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		エラーコード定義																			*/
/*--------------------------------------------------------------------------------------------------*/
#define	FUNCCODE_ERROR			0x01			/* ファンクションコードエラー						*/
#define	MEMADRES_ERROR			0x02			/* メモリアドレスエラー								*/
#define	MODETYPE_ERROR			0x03			/* モード／データ型エラー							*/
#define	DATANUMB_ERROR			0x04			/* 個数不良											*/
#define	LMITACCS_ERROR			0x05			/* アクセス制限エラー								*/
#define	RANGOUTS_ERROR			0x06			/* 設定値範囲外エラー								*/
#define	DATAADJT_ERROR			0x07			/* データ整合エラー									*/
#define	CONDTION_ERROR			0x08			/* 条件エラー										*/
#define	COMPETON_ERROR			0x09			/* 処理競合エラー									*/
#define SELAXIS_ERROR			0x10			/* 軸指定エラー										*/
/*--------------------------------------------------------------------------------------------------*/
#define MBUS_HEAD_BASE_SIZE		0x0C			/* MEMOBUSヘッダ基本サイズ(Byte)アドレス分含む		*/
#define MBUS_HEAD_FIX_SIZE		0x08			/* MEMOBUSヘッダ固定サイズ(Byte)					*/
/*--------------------------------------------------------------------------------------------------*/
#define MBUS_RLY_FIX_SIZE		0x0E			/* 中継コマンド固定サイズ							*/
#define MBUS_RLY_PASS_DATA_SIZE	0x08			/* 中継コマンド可変サイズ(このサイズの倍数で変化)	*/
#define MBUS_RLY_MAX_ROUTENUM	0x0A			/* 中継段数　最大値									*/
/*--------------------------------------------------------------------------------------------------*/
//#define __EXAD_CHECK__
//#define __MYAD_CHECK__
//#define __RCVLEN_CHECK__

/* <S061>	Start */
//#ifdef __INFO_ID_READ_CHECK__	/* <S205> Delete */
/*----------------------------------------------------------------------------------------------------------*/
/*		MEMOBUS メッセージ	製品情報読み出し アドレス情報													*/
/*----------------------------------------------------------------------------------------------------------*/
#define MBUS_IDRD_HEAD_BASE_SIZE		0x0C		/* MEMOBUS(製品情報読み出し) ヘッダ基本サイズ(Byte)		*/
#define MBUS_IDRD_RSP_HEAD_BASE_SIZE	0x0D		/* MEMOBUS(製品情報読み出し) 応答ヘッダ基本サイズ(Byte)	*/
#define MBUS_IDRD_SLOT_NUM				6			/* MEMOBUS(製品情報読み出し) スロット数					*/
#define MBUS_IDRD_ID_NUM				7			/* MEMOBUS(製品情報読み出し) ID数						*/

/*----------------------------------------------------------------------------------------------------------*/
/*		オペレータのパラメータ表示選択 確認																	*/
/*----------------------------------------------------------------------------------------------------------*/
#define OPEDSP_SETUPPRM		0x0000		/* SETUP用ユーザ定義パラメータのみ									*/
#define OPEDSP_ALLPRM		0x0001		/* 全ユーザ定義パラメータ											*/
#define	SETUP_PRM_MASK		0x80		/* セットアップパラメータマスク										*/
#define WRITE_NG			0x0000		/* 書き込みOK														*/
#define WRITE_OK			0x0001		/* 書き込みNG														*/
//#endif	/* <S205> Delete */
/* <S061>	End */

/****************************************************************************************************/
/*		STRUCT DEFINITION																			*/
/****************************************************************************************************/



/****************************************************************************************************/
/*		EXTERN LIST																					*/
/****************************************************************************************************/





/***************************************** end of file **********************************************/
#define _MEMOBUS_FUNCTIONS_H /* 再読み込み防止用 */
#endif /* _MEMOBUS_FUNCTIONS_H */

