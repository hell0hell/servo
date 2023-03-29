/****************************************************************************************************/
/*																									*/
/*																									*/
/*		FirmDownload.h : MEMOBUSファームダウンロード管理											*/
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




/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		ダウンロード条件定義																		*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
#define	MPUNO		0x00						/* ＩＦ基板上のCPUをMPU #0とする				*/
#define	FLASH_NUM	0x01						/* フラッシュメモリ個数							*/
/*--------------------------------------------------------------------------------------------------*/
#define	SYSDL_STS1	0x0001						/* bit0=1	:システムダウンロード対応			*/
													/* bit1=0	:機種IDコード無し					*/
													/* bit2-15	:予約								*/
/*--------------------------------------------------------------------------------------------------*/
#define	SYSDL_STS2	0x0001						/* bit0=1	:#0 Flash システムダウンロード対応	*/
													/* bit1-15	:#1-#15 Flash なし					*/
/*--------------------------------------------------------------------------------------------------*/
#define	SYSDL_STS3	0x0000						/* bit0=1	:#0 Flash ﾏﾆﾌｧｸﾁｬID有り				*/
													/* bit1-15	:#1-#15 Flash なし					*/
/*--------------------------------------------------------------------------------------------------*/
#define	SYSDL_STS4	0x0000						/* bit0=1	:#0 Flash ﾃﾞﾊﾞｲｽID有り				*/
													/* bit1-15	:#1-#15 Flash なし					*/
/*--------------------------------------------------------------------------------------------------*/
#define	SYSDL_WAY	0x0000						/* bit0-3=0	:フラッシュ書込み方法選択			*/
													/*				=0:通信とフラッシュ書込み並行	*/
/*--------------------------------------------------------------------------------------------------*/
#define	SYSDL_1DLEN	0x0100						/* １メッセージあたりの処理バイト数(=256byte)	*/


/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		ダウンロードコマンド関連定義																*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/*		MEMOBUS ファームダウンロード	サブファンクションコード									*/
/*--------------------------------------------------------------------------------------------------*/
#define	MBUS_DL_ID_RD			0x00			/* ID情報読み出し									*/
#define	MBUS_DL_SWVER_RD		0x01			/* バージョン読出し									*/
#define	MBUS_DL_WAY_RD			0x02			/* プログラム書込み条件取得							*/
#define	MBUS_DL_START			0x03			/* ダウンロード開始指令								*/
#define	MBUS_DL_PRGDEL			0x04			/* ブロック消去指令									*/
#define	MBUS_DL_PRGWR			0x05			/* プログラム書込み指令								*/
#define	MBUS_DL_DELSND			0x06			/* ブロック消去情報指令(消去情報転送のみ.実消去しない)		*/
#define	MBUS_DL_PRGSND			0x07			/* プログラム送信指令(プログラム送信のみ.実書込しない)		*/
#define	MBUS_DL_PRGDELA			0x08			/* ブロック一括消去指令								*/
#define	MBUS_DL_PRGWRA			0x09			/* プログラム一括書込み指令							*/
#define	MBUS_DL_END				0x0A			/* ダウンロード終了指令								*/
#define	MBUS_DL_PRGRD			0x0B			/* プログラム読出し指令								*/
#define	MBUS_DL_REBOOT			0x0C			/* プログラム再起動指令								*/
#define	MBUS_DL_SIFCHG			0x0D			/* シリアルＩＦ速度変更								*/
#define	MBUS_DL_HIS_RD			0x0E			/* ダウンロード履歴読出し							*/
#define	MBUS_DL_HIS_WR			0x0F			/* ダウンロード履歴書込み							*/
#define	MBUS_DL_CHKSUM			0x10			/* チェックサム結果取得								*/ /* <S146> */
#define MBUS_DL_SF_IDX			3				/* サブコマンド位置									*/ /* <S150> */
#define MBUS_DL_DATANUM_LEN		10				/* プログラム書込み指令時のデータ数までのパケット長		*/ /* <S150> */
/*--------------------------------------------------------------------------------------------------*/
/*		サブファンクション応答サイズ																*/
/*--------------------------------------------------------------------------------------------------*/
#define	MBUS_DL_ID_RD_RSPSIZE		22			/* ID情報読み出し						<S077>		*/
#define	MBUS_DL_SWVER_RD_RSPSIZE	14			/* バージョン読出し					<S138>			*/
#define	MBUS_DL_WAY_RD_RSPSIZE		10			/* プログラム書込み条件取得							*/
#define	MBUS_DL_START_RSPSIZE		8			/* ダウンロード開始指令								*/
#define	MBUS_DL_PRGDEL_RSPSIZE		14			/* ブロック消去指令									*/
#define	MBUS_DL_PRGWR_RSPSIZE		16			/* プログラム書込み指令								*/
#define	MBUS_DL_DELSND_RSPSIZE		12			/* ブロック消去情報指令(消去情報転送のみ.実消去しない)		*/
#define	MBUS_DL_PRGSND_RSPSIZE		14			/* プログラム送信指令(プログラム送信のみ.実書込しない)		*/
#define	MBUS_DL_PRGDELA_RSPSIZE		8			/* ブロック一括消去指令								*/
#define	MBUS_DL_PRGWRA_RSPSIZE		8			/* プログラム一括書込み指令							*/
#define	MBUS_DL_END_RSPSIZE			8			/* ダウンロード終了指令								*/
//#define	MBUS_DL_PRGRD_RSPSIZE		x			/* プログラム読出し指令								*/
#define	MBUS_DL_REBOOT_RSPSIZE		6			/* プログラム再起動指令								*/
//<S041>　#define	MBUS_DL_SIFCHG_RSPSIZE		10			/* シリアルＩＦ速度変更								*/
#define	MBUS_DL_SIFCHG_RSPSIZE		12			/* シリアルＩＦ速度変更								*/
#define	MBUS_DL_HIS_RD_RSPSIZE		8			/* ダウンロード履歴読出し							*/
#define	MBUS_DL_HIS_WR_RSPSIZE		6			/* ダウンロード履歴書込み							*/
#define	MBUS_DL_CHKSUM_RSPSIZE		10			/* チェックサム結果取得								*/ /* <S146> */
#define MBUS_DL_ERROR_RSPSIZE		5			/* エラー											*/
#define MBUS_DL_MIN_RSPSIZE			5			/* メッセージ最小サイズ								*/
/*--------------------------------------------------------------------------------------------------*/
/*		MEMOBUS ファームダウンロード	レスポンス													*/
/*--------------------------------------------------------------------------------------------------*/
#define	MBUS_OK		0
#define	MBUS_NG		-1
#define	MBUS_BUSY	1


/* <S1A3> >>>>> */
/*--------------------------------------------------------------------------------------------------*/
/*		サムチェック用定義																			*/
/*--------------------------------------------------------------------------------------------------*/
#define	CHKSUM_OK		0
#define	CHKSUM_NG		-1
#define	CHKSUM_BUSY		1

#define CHKSUM_MAIN_PHASE_MASK	0xFF00							/* メインフェーズ用マスクパターン	*/
#define CHKSUM_MAIN_PHASE_SHFT	8								/* メインフェーズ用右シフト数		*/
#define CHKSUM_MAIN_PHASE_ADD	0x0100							/* メインフェーズ用加算値			*/
#define CHKSUM_SUB_PHASE_MASK	0x00FF							/* サブフェーズ用マスクパターン		*/
#define CHKSUM_PHASE_END		0xFFFF							/* チェックサムエンドパターン		*/

#define CHKSUM_BLOCK_SIZE		0x400							/* 1スキャンでの処理バイト数		*/
																/* ※ 1ビットのみONにすること(0x400, 0x200, 0x100など)	*/
#define CHKSUM_BLOCK_MASK		(CHKSUM_BLOCK_SIZE - 1)			/* 1ブロック終了判定用マスクパターン*/
/* <<<<< <S1A3> */

/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		ダウンロード定義構造体																		*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct{
	UCHAR	MpuNo;									/* ＭＰＵ番号									*/
	UCHAR	FlushNum;								/* フラッシュメモリ個数							*/
	USHORT	DlSts1;									/* ダウンロードステータス１						*/
														/*   Bit0：ダウンロード対応 0/1-非対応/対応		*/
														/*   Bit1：機種IDコード有無	0/1-無/有			*/
														/*   Bit2-15：予約領域							*/
	USHORT	DlSts2;									/* ダウンロードステータス２						*/
														/*   Bit0 ：Flash #0	ﾀﾞｳﾝﾛｰﾄﾞ 0/1-可/不可	*/
														/*   Bit1 ：Flash #1	ﾀﾞｳﾝﾛｰﾄﾞ 0/1-可/不可	*/
														/*   Bit2 ：Flash #2	ﾀﾞｳﾝﾛｰﾄﾞ 0/1-可/不可	*/
														/*   Bit3 ：Flash #3	ﾀﾞｳﾝﾛｰﾄﾞ 0/1-可/不可	*/
														/*   Bit4 ：Flash #4	ﾀﾞｳﾝﾛｰﾄﾞ 0/1-可/不可	*/
														/*   Bit5 ：Flash #5	ﾀﾞｳﾝﾛｰﾄﾞ 0/1-可/不可	*/
														/*   Bit6 ：Flash #6    ﾀﾞｳﾝﾛｰﾄﾞ 0/1-可/不可	*/
														/*   Bit7 ：Flash #7	ﾀﾞｳﾝﾛｰﾄﾞ 0/1-可/不可	*/
														/*   Bit8 ：Flash #8	ﾀﾞｳﾝﾛｰﾄﾞ 0/1-可/不可	*/
														/*   Bit9 ：Flash #9	ﾀﾞｳﾝﾛｰﾄﾞ 0/1-可/不可	*/
														/*   Bit10：Flash #10	ﾀﾞｳﾝﾛｰﾄﾞ 0/1-可/不可	*/
														/*   Bit11：Flash #11	ﾀﾞｳﾝﾛｰﾄﾞ 0/1-可/不可	*/
														/*   Bit12：Flash #12	ﾀﾞｳﾝﾛｰﾄﾞ 0/1-可/不可	*/
														/*   Bit13：Flash #13	ﾀﾞｳﾝﾛｰﾄﾞ 0/1-可/不可	*/
														/*   Bit14：Flash #14	ﾀﾞｳﾝﾛｰﾄﾞ 0/1-可/不可	*/
														/*   Bit15：Flash #15	ﾀﾞｳﾝﾛｰﾄﾞ 0/1-可/不可	*/
	USHORT	DlSts3;									/* ダウンロードステータス３						*/
														/*   Bit0 ：Flash #0	ﾏﾆﾌｧｸﾁｬID 0/1-無/有		*/
														/*   Bit1 ：Flash #1	ﾏﾆﾌｧｸﾁｬID 0/1-無/有		*/
														/*   Bit2 ：Flash #2	ﾏﾆﾌｧｸﾁｬID 0/1-無/有		*/
														/*   Bit3 ：Flash #3	ﾏﾆﾌｧｸﾁｬID 0/1-無/有		*/
														/*   Bit4 ：Flash #4	ﾏﾆﾌｧｸﾁｬID 0/1-無/有		*/
														/*   Bit5 ：Flash #5	ﾏﾆﾌｧｸﾁｬID 0/1-無/有		*/
														/*   Bit6 ：Flash #6	ﾏﾆﾌｧｸﾁｬID 0/1-無/有		*/
														/*   Bit7 ：Flash #7	ﾏﾆﾌｧｸﾁｬID 0/1-無/有		*/
														/*   Bit8 ：Flash #8	ﾏﾆﾌｧｸﾁｬID 0/1-無/有		*/
														/*   Bit9 ：Flash #9	ﾏﾆﾌｧｸﾁｬID 0/1-無/有		*/
														/*   Bit10：Flash #10	ﾏﾆﾌｧｸﾁｬID 0/1-無/有		*/
														/*   Bit11：Flash #11	ﾏﾆﾌｧｸﾁｬID 0/1-無/有		*/
														/*   Bit12：Flash #12	ﾏﾆﾌｧｸﾁｬID 0/1-無/有		*/
														/*   Bit13：Flash #13	ﾏﾆﾌｧｸﾁｬID 0/1-無/有		*/
														/*   Bit14：Flash #14	ﾏﾆﾌｧｸﾁｬID 0/1-無/有		*/
														/*   Bit15：Flash #15	ﾏﾆﾌｧｸﾁｬID 0/1-無/有		*/
	USHORT	DlSts4;									/* ダウンロードステータス４						*/
														/*   Bit0 ：Flash #0	ﾃﾞﾊﾞｲｽID  0/1-無/有		*/
														/*   Bit1 ：Flash #1	ﾃﾞﾊﾞｲｽID  0/1-無/有		*/
														/*   Bit2 ：Flash #2	ﾃﾞﾊﾞｲｽID  0/1-無/有		*/
														/*   Bit3 ：Flash #3	ﾃﾞﾊﾞｲｽID  0/1-無/有		*/
														/*   Bit4 ：Flash #4	ﾃﾞﾊﾞｲｽID  0/1-無/有		*/
														/*   Bit5 ：Flash #5	ﾃﾞﾊﾞｲｽID  0/1-無/有		*/
														/*   Bit6 ：Flash #6	ﾃﾞﾊﾞｲｽID  0/1-無/有		*/
														/*   Bit7 ：Flash #7	ﾃﾞﾊﾞｲｽID  0/1-無/有		*/
														/*   Bit8 ：Flash #8	ﾃﾞﾊﾞｲｽID  0/1-無/有		*/
														/*   Bit9 ：Flash #9	ﾃﾞﾊﾞｲｽID  0/1-無/有		*/
														/*   Bit10：Flash #10	ﾃﾞﾊﾞｲｽID  0/1-無/有		*/
														/*   Bit11：Flash #11	ﾃﾞﾊﾞｲｽID  0/1-無/有		*/
														/*   Bit12：Flash #12	ﾃﾞﾊﾞｲｽID  0/1-無/有		*/
														/*   Bit13：Flash #13	ﾃﾞﾊﾞｲｽID  0/1-無/有		*/
														/*   Bit14：Flash #14	ﾃﾞﾊﾞｲｽID  0/1-無/有		*/
														/*   Bit15：Flash #15	ﾃﾞﾊﾞｲｽID  0/1-無/有		*/
	USHORT	DlWay;									/* システムダウンロード方法選択					*/
														/*   Bit0-3 ：ダウンロード方法					*/
														/*   Bit4-15：予約領域							*/
	USHORT	Prg1WrSize;								/* １メッセージ当たりの書込みバイト数指定		*/
	USHORT	DlStatus;								/* ダウンロードステータス							*/
														/*   Bit0 ：ダウンロードモード 0/1-OFF/ON		*/
														/*   Bit1 ：Flash消去中 	0/1-OFF/ON			*/
														/*   Bit2 ：Flash書き込み中	0/1-OFF/ON			*/
														/*   Bit3 ：Flash読み出し中	0/1-OFF/ON			*/
														/*   Bit4-5：処理状態(00/01/10/11)				*/
														/*   			なし/busy/応答許可/予約			*/
	ULONG	NowBps;										/*	システムダウンロード現在のＢＰＳ値	<S041> */
	ULONG	BpsChkTime;									/* Bps Timeout Timer <S041>	*/
/* <S146> >>>>> */
	/* Message -> Sum check */
	BOOL	MicroSumCheckF;							/* マイクロコード部サムチェック要否フラグ			*/
	BOOL	MirSumCheckF;							/* ミラー部サムチェック要否フラグ					*/
	BOOL	RtmSumCheckF;							/* ランタイム部サムチェック要否フラグ				*/
	BOOL	BootSumCheckF;							/* ブート部サムチェック要否フラグ					*/
	BOOL	CheckSumRequestF;						/* チェックサム処理開始依頼フラグ					*/
	BOOL	CheckSumCancelF;						/* チェックサム処理キャンセル依頼フラグ				*/
	/* Sum check -> Message */
	BOOL	CheckSumCalcF;							/* チェックサム処理実行中フラグ						*/
	BOOL	CheckSumEndF;							/* チェックサム処理終了フラグ						*/
	BOOL	CheckSumResult;							/* チェックサム処理結果	(TRUE:OK, FALSE:NG)			*/
	/* for Sum check */
	USHORT	CheckSumPhase;							/* チェックサムフェーズ								*/ /*<S182>*/
/* <S1A3> >>>>> */
	USHORT	CheckSumVal;							/* サム値計算用										*/
	UCHAR	*CheckSumMem;							/* メモリポインタ									*/
	LONG	CheckSumIdx;							/* 処理位置											*/
/* <<<<< <S1A3> */
/* <<<<< <S146> */
} SYSDL;

extern SYSDL	SysDl;

/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		マクロ定義																					*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
#define SYSDL_DLMODE_RESET()	(SysDl.DlStatus = 0x0000)			/* ダウンロードモードBit操作	*/
#define SYSDL_DLMODE_ON()		(SysDl.DlStatus |= 0x0001)			/* ダウンロードモードBit操作	*/
#define SYSDL_DLMODE_OFF()		(SysDl.DlStatus &= 0xFFFE)
#define SYSDL_CHK_DLMODE_ON()	((SysDl.DlStatus & 0x0001) == 1)
#define SYSDL_CHK_DLMODE_OFF()	((SysDl.DlStatus & 0x0001) != 1)
/*--------------------------------------------------------------------------------------------------*/
#define SYSDL_ERASE_ON()		(SysDl.DlStatus |= 0x0002)			/* Flash Erase Bit操作			*/
#define SYSDL_ERASE_OFF()		(SysDl.DlStatus &= 0xFFFD)
#define SYSDL_CHK_ERASE_ON()	((SysDl.DlStatus & 0x0002) == 1)
#define SYSDL_CHK_ERASE_OFF()	((SysDl.DlStatus & 0x0002) != 1)
/*--------------------------------------------------------------------------------------------------*/
#define SYSDL_WRITE_ON()		(SysDl.DlStatus |= 0x0004)			/* Flash Write Bit操作			*/
#define SYSDL_WRITE_OFF()		(SysDl.DlStatus &= 0xFFFB)
#define SYSDL_CHK_WRITE_ON()	((SysDl.DlStatus & 0x0004) == 1)
#define SYSDL_CHK_WRITE_OFF()	((SysDl.DlStatus & 0x0004) != 1)
/*--------------------------------------------------------------------------------------------------*/
#define SYSDL_READ_ON()			(SysDl.DlStatus |= 0x0008)			/* Flash Read Bit操作			*/
#define SYSDL_READ_OFF()		(SysDl.DlStatus &= 0xFFF7)
#define SYSDL_CHK_READ_ON()		((SysDl.DlStatus & 0x0008) == 1)
#define SYSDL_CHK_READ_OFF()	((SysDl.DlStatus & 0x0008) != 1)
/*--------------------------------------------------------------------------------------------------*/
#define SYSDL_CHK_FLASH_BUSY()	((SysDl.DlStatus & 0x000E) != 0)	/* Flash操作中確認 TRUEで動作中	*/
/*--------------------------------------------------------------------------------------------------*/


/* 仮対応（最終的にはマクロorAPI？） */
#ifdef _TEST_BOARD_		/* for MICROSSP */
#define	SYSDL_CHK_BASE_BLOCK( )			(((*(VUSHORT*)(0x08000032)) & 0x0400) != 0)
#else					/* for SS-Pro7 */
#define	SYSDL_CHK_BASE_BLOCK( )			(((*(VUSHORT*)(0xC0000064)) & 0x0400) != 0)
#endif

