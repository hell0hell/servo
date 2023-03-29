/****************************************************************************************************/
/*																									*/
/*																									*/
/*		FlashIF.c : Flash処理インタフェース															*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : SoC内およびオプションモジュールのFlashのＩＦ処理を行う。								*/
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
/*	Note	:	初版	2013.07.09	Y.Tanaka														*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "FlashIF.h"
#include "KLib.h"


/****************************************************************************************************/
/*																									*/
/*		PROTOTYPE																					*/
/*																									*/
/****************************************************************************************************/
void	InitSFlashSoC( void );				/* 本体SerialFlash初期化		*/
UCHAR	SFlashModeChgSoC( UCHAR SPI, UCHAR Read, UCHAR PreFetch, UCHAR Com );		/* 本体SerialFlashモード変更		*/
LONG	SFlashEraseSectorSoC( ULONG EraseAddr );	/* 本体SerialFlashセクター消去	*/
LONG	SFlashWritePageSoC( ULONG WriteAddr, ULONG *WriteBuf );		/* 本体SerialFlashページライト	*/
LONG	SFlashReadByteSoC( ULONG ReadAddr, UCHAR *ReadBuf );		/* 本体SerialFlashバイトリード	*/
//LONG	SFlashEraseSectorOption( ULONG EraseAddr );		/* オプション基板SerialFlashブロック消去(DPM)	*/
//LONG	SFlashWritePageOption( ULONG WriteAddr, ULONG *WriteBuf );	/* オプション基板SerialFlashページライト(DPM)	*/

/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		VARIABLE																					*/
/*																									*/
/****************************************************************************************************/
typedef struct{
	struct{
		UCHAR	SPI;	/* SPImode 0/1 - SPI0/SPI3								*/
		UCHAR	Read;	/* read mode 0/1 - normal/fast							*/
		UCHAR	PreFetch;	/* pre fetch mode 0/1 - off/on						*/
		UCHAR	Com;	/* Communication mode 0/1 - rom/direct					*/
	}Mode;
	struct{
		UCHAR	erase;	/* erase status 0/1 - none/busy							*/
		UCHAR	write;	/* write status 0/1 - none/busy							*/
		UCHAR	read;	/* read status 0/1 - none/busy							*/
		UCHAR	Comm;	/* any accese status 0/1 - none/busy					*/
	}Sts;
}SFLASHVAL;
static	SFLASHVAL	SFlashV;


/****************************************************************************************************/
/*																									*/
/*		本体SerialFlash初期化																		*/
/*																									*/
/****************************************************************************************************/
void InitSFlashSoC( void )
{
	SerialRomInit( );
#ifdef	_MERCURY_PLATFORM_			/*<S190>*/
    MlibResetByteMemory( &SFlashV, sizeof(SFlashV) );
#endif	/*_MERCURY_PLATFORM_*/
}


/****************************************************************************************************/
/*																									*/
/*		本体SerialFlashモード変更																	*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/****************************************************************************************************/
UCHAR	SFlashModeChgSoC( UCHAR SPI, UCHAR Read, UCHAR PreFetch, UCHAR Com )
{
	UCHAR	rc;
	
	SFlashV.Mode.SPI = SPI;
	SFlashV.Mode.Read = Read;
	SFlashV.Mode.PreFetch = PreFetch;
	SFlashV.Mode.Com = Com;
//	rc = SerialRomModeChange( SFlashV.Mode );
	rc = 0;
	
	return( rc );
}


/****************************************************************************************************/
/*																									*/
/*		本体SerialFlashセクター消去																	*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		動作：SerialFlashのセクター消去コマンド発行/終了監視										*/
/*		引数：																						*/
/*			ULONG EraseAddr	:消去するセクターの先頭アドレス(0xFFE00000から64KB(0x10000)刻み)		*/
/*		戻り値：OK(0)/BUSY(1)/NG_ADDR(-1)/NG_CMD(-2)												*/
/*			OK:セクター消去動作終了																	*/
/*			BUSY:Erase実行中(継続)																	*/
/*			NG_ADDR:設定アドレス異常																*/
/*			NG_CMD:他コマンド(write/read)実行中														*/
/*																									*/
/****************************************************************************************************/
LONG	SFlashEraseSectorSoC( ULONG EraseAddr )
{
	LONG rc;		/* -1/0/1 - NG/OK/BUSY	*/

	if( EraseAddr < 0xFFE00000 )
	{
		rc = -1/*NG_ADDR*/;
	}
	else if( (EraseAddr & 0x0000FFFF) != 0x0 )
	{
		rc = -1/*NG_ADDR*/;
	}
	else
	{
		if( SFlashV.Sts.Comm == 0/*NONE*/ )	/* 実行中コマンドなし */
		{
			/* Sector Erase */
			SFlashV.Sts.Comm = 1/*BUSY*/;
			SFlashV.Sts.erase = 1/*BUSY*/;
			SerialRomSectorErase( EraseAddr );
			rc = 1/*BUSY*/;
		}
		else	/* 実行中コマンドあり */
		{
			if( SFlashV.Sts.erase == 1/*BUSY*/ )	/* Erase実行中 */
			{
				SFlashV.Sts.erase = (SFlashChkStatus()&0x01);		/* Erase実行状態確認 */
				if( SFlashV.Sts.erase == 0 )		/* Erase終了 */
				{
					SFlashV.Sts.Comm = 0/*OK*/;
					rc = 0/*OK*/;
				}
				else
				{
					rc = 1/*BUSY*/;
				}
			}
			else	/* 他の処理実行中 */
			{
				rc = -2/*NG_CMD*/;
			}
		}
	}

	return( rc );

}


/****************************************************************************************************/
/*																									*/
/*		本体SerialFlashページライト																	*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		動作：指定されたMPUのSerialFlashページライトコマンド発行									*/
/*		引数：																						*/
/*			ULONG WriteAddr	:書き込むページの先頭アドレス											*/
/*			ULONG* WriteBuf	:書き込みデータバッファ													*/
/*		戻り値：OK(0)/BUSY(1)/NG_ADDR(-1)/NG_CMD(-2)												*/
/*			OK		:動作終了																		*/
/*			BUSY	:実行中(継続)																	*/
/*			NG_ADDR	:設定アドレス異常																*/
/*			NG_CMD	:他コマンド(write/read)実行中													*/
/*																									*/
/****************************************************************************************************/
LONG	SFlashWritePageSoC( ULONG WriteAddr, ULONG *WriteBuf )
{
	LONG rc;		/* -1/0/1 - NG/OK/BUSY	*/

	if( WriteAddr < 0xFFE00000 )
	{
		rc = -1/*NG_ADDR*/;
	}
	else if( (WriteAddr & 0x000000FF) != 0x0 )
	{
		rc = -1/*NG_ADDR*/;
	}
	else
	{
		if( SFlashV.Sts.Comm == 0/*NONE*/ )	/* 実行中コマンドなし */
		{
			/* Page Program */
			SFlashV.Sts.Comm = 1/*BUSY*/;
			SFlashV.Sts.write = 1/*BUSY*/;
			SerialRomPageProgram( WriteAddr, WriteBuf, 0x40 );
			rc = 1/*BUSY*/;
		}
		else	/* 実行中コマンドあり */
		{
			if( SFlashV.Sts.write == 1/*BUSY*/ )	/* Write実行中 */
			{
				SFlashV.Sts.write = (SFlashChkStatus()&0x01);		/* Write実行状態確認 */
				if( SFlashV.Sts.write == 0 )		/* Write終了 */
				{
					SFlashV.Sts.Comm = 0/*OK*/;
					rc = 0/*OK*/;
				}
				else
				{
					rc = 1/*BUSY*/;
				}
			}
			else	/* 他の処理実行中 */
			{
				rc = -2/*NG_CMD*/;
			}
		}
	}

	return( rc );

}


/****************************************************************************************************/
/*																									*/
/*		本体SerialFlashバイトリード																	*/
/*																									*/
/****************************************************************************************************/
LONG	SFlashReadByteSoC( ULONG ReadAddr, UCHAR *ReadBuf )
{
	INT nSize;
	UCHAR pBuff[10] = {0};

	nSize = sizeof(pBuff);
	SerialRomReadDataBytes( ReadAddr, pBuff, nSize );
}



