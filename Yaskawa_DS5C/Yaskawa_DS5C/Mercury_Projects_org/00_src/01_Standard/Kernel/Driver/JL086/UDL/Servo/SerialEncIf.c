/****************************************************************************
Description	: Serial Encoder Command Interface
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2010 All Rights Reserved

Project       : Mercury

----------------------------------------------------------------------------
Changes	      :
	Name   Date          Description
	------------------------------------------------------------------------
	Y.Oka  2010.02.23    created
	I.M.   2011.02.08    reviewed
	Y.Oka  2012.08.20    remaked

****************************************************************************/
#include "Basedef.h"
#include "SerialEncIf.h"
#include "MLib.h"
#include "KLib.h"

#define	ERR5DEG		910			/* 電気角5°の時の電流位相値[2^16/360deg] */
#define CMD_SND_NUM	50			/* マルチターンリセット コマンド送信回数 */		/* <S0BB> */
#define BUSSY		2			/* マルチターンリセット コマンド送信回数 */		/* <S0BB> */


/* Open Key Table : システムモードへのパスワード 'ASKAWA SABS'	*/
static const USHORT OpenKeyTbl[8] =
{
	0x0000, 0x5341, 0x414B, 0x4157, 0x5320, 0x4241, 0x2053, 0x2020
};


#if defined(_VC_DEBUG_) /* 2012.08.20 Y.Oka Make Mercury Base Software */
extern	const UCHAR MotorPrm[];
#endif



/****************************************************************************
  Description: P0モード切り替え処理
-----------------------------------------------------------------------------
  Parameters	:
		MENCV *MencV				: Pointer to the Encoder work Handle
		UINT ch						: Channel number of the ASICS
		ASIC_HW_IF_STR *AsicHwReg	: Pointer to the ASIC register
  Return		:
		none
*****************************************************************************/
void SencInitChannel( MENCV *MencV, UINT ch, ASIC_HW_IF_STR *encAsicHwReg, ASIC_HW_IF_STR *posCmpAsicHwReg )
{
	MlibResetByteMemory(MencV, sizeof(MENCV));

	if(ch == 0)
	{
		MencV->EncIf.ch = 0;
		MencV->EncIf.SencSetReg = encAsicHwReg;/* <S04D> */
		MencV->EncIf.PosCmpSetReg = posCmpAsicHwReg;/* <S04D> */

		/* 2012.08.20 Y.Oka Make Mercury Base Software */
		/* ★シリアルエンコーダI/F再検討要★ */

		/* Ch0送信バッファアドレス */
		MencV->EncIf.TxBuf = (VULONG*)&(encAsicHwReg->AREG_SE0_TXD01);	/* <S04D>:rename */
		/* Ch0受信バッファアドレス */
		MencV->EncIf.RxBuf = (VULONG*)&(encAsicHwReg->AREG_SE0_RXD01);	/* <S04D>:rename */

		/* ★シリアルエンコーダI/F再検討要★ */
		/* 2012.08.20 Y.Oka Make Mercury Base Software */

		/* Ch0受信完了フラグアドレス */
		MencV->EncIf.RxFlg = &(encAsicHwReg->AREG_SEPGSET01);	/* <S04D>:rename */
		/* Ch送信開始用書き込みレジスタアドレス */
		MencV->EncIf.TxStart = &(encAsicHwReg->AREG_ESYNC);	/* <S04D>:rename */
		/* Ch0送信開始コマンドデータ(ESYNC0) */
		MencV->EncIf.TxCmd = KPX_CDAT_ENC0_ESYNC;/* <S04D>:rename */
	}
	else
	{
		MencV->EncIf.ch = 1;
		MencV->EncIf.SencSetReg = encAsicHwReg;/* <S04D> */
		MencV->EncIf.PosCmpSetReg = posCmpAsicHwReg;/* <S04D> */

		/* 2012.08.20 Y.Oka Make Mercury Base Software */
		/* ★シリアルエンコーダI/F再検討要★ */

		/* Ch1送信バッファアドレス */
		MencV->EncIf.TxBuf = (VULONG*)&(encAsicHwReg->AREG_SE1_TXD01);	/* <S04D>:rename */
		/* Ch1受信バッファアドレス */
		MencV->EncIf.RxBuf = (VULONG*)&(encAsicHwReg->AREG_SE1_RXD01);	/* <S04D>:rename */

		/* ★シリアルエンコーダI/F再検討要★ */
		/* 2012.08.20 Y.Oka Make Mercury Base Software */

		/* Ch1受信完了フラグアドレス */
		MencV->EncIf.RxFlg = &(encAsicHwReg->AREG_SEPGSET11);	/* <S04D>:rename */
		/* Ch送信開始用書き込みレジスタアドレス */
		MencV->EncIf.TxStart = &(encAsicHwReg->AREG_ESYNC);	/* <S04D>:rename */
		/* Ch1送信開始コマンドデータ(ESYNC1) */
		MencV->EncIf.TxCmd = KPX_CDAT_ENC1_ESYNC;
	}
}


/****************************************************************************
  Description: P0モード切り替え処理(初期化処理用)				<S0F8>
-----------------------------------------------------------------------------
  Parameters	:
		MENCV *MencV				: Pointer to the Encoder work Handle
  Return		:
		none
*****************************************************************************/
void SencInitChangeP0Mode( MENCV *MencV )
{
	/* Change P0 Mode */
	SHalDrv_InitChangeToP0Mode( MencV->EncIf.SencSetReg, MencV->EncIf.ch );
}


/****************************************************************************
  Description: P0モード切り替え処理
-----------------------------------------------------------------------------
  Parameters	:
		MENCV *MencV				: Pointer to the Encoder work Handle
  Return		:
		none
*****************************************************************************/
void SencChangeP0Mode( MENCV *MencV )
{
	/* Change P0 Mode */
	SHalDrv_ChangeToP0Mode( MencV->EncIf.SencSetReg, MencV->EncIf.ch );	/* <S04D> 引数変更 */

	/* Set Interrupt Cycle (P0mode) Request <S1D3> */
	if( MencV->EncIf.ch == 0 )
	{
		MencV->SetP0ModeIntCycleReq = TRUE;
	}
}

/****************************************************************************
  Description: P1モード切り替え処理
-----------------------------------------------------------------------------
  Parameters	:
		MENCV *MencV				: Pointer to the Encoder work Handle
  Return		:
		none
*****************************************************************************/
void SencChangeP1Mode( MENCV *MencV )
{
	/* Change P1 Mode */
	SHalDrv_ChangeToP1Mode( MencV->EncIf.SencSetReg, MencV->EncIf.ch );	/* <S04D> 引数変更 */
	MencV->EncComErrCnt = SHalDrv_EncAlmReset( MencV->EncIf.SencSetReg );	/* <S217> */

	/* Start ScanA */
	MencV->AccChkWaitCnt = 0;
	MencV->AccChkEnable = FALSE;

	/* Set Interrupt Cycle (P1mode) Request <S1D3> */
	if( MencV->EncIf.ch == 0 )
	{
		MencV->SetP1ModeIntCycleReq = TRUE;
	}
}

/****************************************************************************
  Description: P0モード割り込み周期設定処理						<S1D3>
-----------------------------------------------------------------------------
  Parameters	:
		MENCV *MencV				: Pointer to the Encoder work Handle
  Return		:
		none
*****************************************************************************/
void SencSetIntCycleP0Mode( MENCV *MencV )
{
	if( (MencV->SetP0ModeIntCycleReq == TRUE) && (MencV->ChangeP1toP0Enable == TRUE) )
	{
		/* P0:伝送周期 125us */
		MencV->EncIf.SencSetReg->AREG_SRCNT0 = 0x000003E7;
		/* ScanA=125us, ScanB=125us, ScanC=2ms, ScanN=125us(未使用)	*/
		MencV->EncIf.SencSetReg->AREG_INTCYCLE = 0x00000f00;
		MencV->SetP0ModeIntCycleReq = FALSE;
	}
	MencV->ChangeP1toP0Enable = FALSE;
}


/****************************************************************************
  Description: P1モード割り込み周期設定処理						<S1D3>
-----------------------------------------------------------------------------
  Parameters	:
		MENCV *MencV				: Pointer to the Encoder work Handle
  Return		:
		none
*****************************************************************************/
void SencSetIntCycleP1Mode( MENCV *MencV )
{
	if( MencV->SetP1ModeIntCycleReq == TRUE )
	{
		/* P1:伝送周期 62.5us */
//		MencV->EncIf.SencSetReg->AREG_SRCNT0 = 0x000001F3;
		/* ScanA=62.5us, ScanB=125us, ScanC=2ms, ScanN=125us(未使用) */
		MencV->EncIf.SencSetReg->AREG_INTCYCLE = 0x00011f10;
		MencV->SetP1ModeIntCycleReq = FALSE;
	}
	MencV->ChangeP1toP0Enable = TRUE;
}

/****************************************************************************
*
*		シリアルエンコーダ : Ｐ０モードコマンド送信処理
*
****************************************************************************/
static void SencSendP0Command( SENCX *Enc, USHORT Cmd, USHORT data )
{
	LONG			i;

	/*	送信データの設定 */
	switch(Cmd)
	{
	case SPG_PREAD:
	case SPG_PWRITE:
	case SPG_MTLMTCHG:
//		Enc->TxBuf[1] = data;
		Enc->TxData.UsBuf[1] = data;
		break;

	case SPG_OPENKEY:		/* シーケンス見直し <S03F>*/
		for( i=1; i < 8; i++ )
		{
//			Enc->TxBuf[i] = OpenKeyTbl[i];
			Enc->TxData.UsBuf[i] = OpenKeyTbl[i];
		}
		break;

	case SPG_ZEROSET:
	case SPG_CLOSEKEY:
	default:
		for( i=1; i < 8; i++ )
		{
//			Enc->TxBuf[i] = data;
			Enc->TxData.UsBuf[i] = data;
		}
		break;
	}

	/*	コマンドコードの設定 */
	Enc->TxData.UsBuf[0] = Cmd;

	/* Send Command Data to Encoder */
	SHalDrv_SencSetTxData( (void*)Enc->TxBuf, (void*)&(Enc->TxData) );

#if	0 /* P0モード中にScanAのタイミングがずれるため無効化  暫定処置 <S0F2> */
	/* Set Encode Transmission Start Request to JL-086 */
	SHalDrv_SencStartTx( (void*)Enc->TxStart, (void*)&(Enc->TxCmd) );
#endif

	return;
}


/****************************************************************************
*
*		シリアルエンコーダ : Ｐ０モードコマンド送信開始＆受信データ取得処理
*
****************************************************************************/
static ULONG SencSendAndReceiveData( SENCX *Enc )
{
	ULONG	RxFlg;

	/* 2012.11.07 Y.Oka ★送信済み？要確認★ */
	/* Send Command Data to Encoder */
//	SHalDrv_SencSetTxData( (void*)Enc->TxBuf, (void*)&(Enc->TxData) );
	/* エンコーダ通信 送信開始 */
//	SHalDrv_SencStartTx( (void*)Enc->TxStart, (void*)&(Enc->TxCmd) );

	/* エンコーダ通信 受信ステータス取得 */
	RxFlg = SHalDrv_SencGetRxStatus( (void*)Enc->RxFlg );
	/* エンコーダ通信 受信データ取得 */
	SHalDrv_SencGetRxData( (void*)Enc->RxBuf, (void*)&(Enc->RxData) );

	return RxFlg;
}


/****************************************************************************
*
*		シリアルエンコーダ : Ｐ０モードコマンド応答チェック処理
*
****************************************************************************/
static LONG SencCheckP0CmdEchoBak( ENC_DATA_BUF *RxData, ULONG RxFlg, USHORT Cmd, USHORT data )
{
	LONG	ret;
	ret = OK;

	/*----------------------------------------------------------------------*/
	/*	コマンド応答の受信完了チェック										*/
	/*----------------------------------------------------------------------*/
	if( (RxFlg & 0x8000) == 0 )
	{ /* 受信未完了 */
		//ret = NG;		/* <S0BB> Delete */
		ret = BUSSY;	/* マルチターンリセット コマンド送信回数 */		/* <S0BB> */
	}
	else
	{
		switch(Cmd)
		{
		/*----------------------------------------------------------------------*
		*	コマンド応答の受信内容チェック (BUSY=0,COMERR=0,ｺﾏﾝﾄﾞｴｺｰﾊﾞｯｸあり)
		*   ※CLOSEKEYはCOMERRRを見ない
		*----------------------------------------------------------------------*/
		case SPG_CLOSEKEY:
			if( (RxData->EncRspKeyCntrl.sdata.header & 0x01FF) != SPG_CLOSEKEY )
			{
				ret = NG;
			}
			break;

		/*----------------------------------------------------------------------*/
		/*	READ/WRITEコマンドの受信データチェック (ｱﾄﾞﾚｽ+ﾊﾞｲﾄ数が正しい)		*/
		/*----------------------------------------------------------------------*/
		case SPG_PREAD:
		case SPG_PWRITE:
			if( ((RxData->EncRspPrmAccess.sdata.header & 0x03FF) != (Cmd & 0x00FF))
				|| ((RxData->EncRspPrmAccess.sdata.Operand) != data) )
			{
				ret = NG;
			}
			break;
		/*----------------------------------------------------------------------*/
		/*	SGD7Wの2軸目でFn008, Fn013が失敗する暫定処置			<S134>		*/
		/*----------------------------------------------------------------------*/
#if	0
		case SPG_PMODEOFF:
		case SPG_MTRST:
		case SPG_MTLMTCHG:
			if( (RxData->EncRspNormal.sdata.header & 0x02FF) != (Cmd & 0x00FF) )
			{
				/* エコーバックが0x00で返ってくる暫定処置 */
				if( (RxData->EncRspNormal.sdata.header & 0x3FFF) == 0x0000)
				{
					ret = BUSSY;
				}
				else
				{
					ret = NG;
				}
			}
			break;
#endif
		default:
			if( (RxData->EncRspNormal.sdata.header & 0x03FF) != (Cmd & 0x00FF) )
			{
				ret = NG;
			}
			break;
		}
	}

	return( ret );
}

/****************************************************************************
*
*		シリアルエンコーダ : コマンド送信＆応答チェック
*
****************************************************************************/
static BOOL SencSend1Command( SENCX *Enc, USHORT cmd )
{
	LONG			i;
	ULONG			RxFlg;

	for( i = 0; i < 128; i++ )
	{
		/* Set Command Code */
		Enc->TxData.EncCmdNormal.sdata.CmdCode = cmd;

		/* Send Command Data to Encoder */
		SHalDrv_SencSetTxData( (void*)Enc->TxBuf, (void*)&(Enc->TxData) );
		/* Set Encode Transmission Start Request to JL-086 */
		SHalDrv_SencStartTx( (void*)Enc->TxStart, (void*)&(Enc->TxCmd) );

		KlibWaitus( 250 );

		/* Get Encode Receive Status from JL-086 */
		RxFlg = SHalDrv_SencGetRxStatus( (void*)Enc->RxFlg );
		/* Get Encode Received Data from JL-086 */
		SHalDrv_SencGetRxData( (void*)Enc->RxBuf, (void*)&(Enc->RxData) );

		/* Check Receive Status and Response Header */
//		if( ((RxFlg & 0x8000) != 0)
//			&& ((Enc->RxData.EncRspNormal.sdata.header & 0x03FF) == cmd) )
		/* cmd = SPG_OPENKEY(0x594F)のときにTRUEにならないため変更 */ /*<S018>*/
		if( ((RxFlg & 0x8000) != 0)
			&& ((Enc->RxData.EncRspNormal.sdata.header & 0x03FF) == (cmd & 0x00FF)) )
		{
			return( TRUE );
		}


	}

	return( FALSE );
}



/****************************************************************************
*
*		シリアルエンコーダ : 拡張コマンド送信 (Nop + Cmd)
*
****************************************************************************/
static BOOL SencSendXCommand( SENCX *Enc, USHORT cmd )
{
	LONG	i;

	for( i = 0; i < 4; i++ )
	{
		if( SencSend1Command( Enc, SPG_NOP ) != FALSE )
		{
			if( SencSend1Command( Enc, cmd ) != FALSE )
			{
				return( TRUE );
			}
		}
		else
		{
			return( FALSE );
		}
	}
	/*----------------------------------------------------------------------*/
	/*	Error : EchoBack Command											*/
	/*----------------------------------------------------------------------*/
	return( FALSE );
}



/****************************************************************************
*
*		シリアルエンコーダ : エコーバックチェック
*
****************************************************************************/
static SENC_ERROR SencCheckEchoBack( SENCX *Enc, USHORT data )
{
	LONG			i, j;
	SENC_ERROR		err;

	/* Initialize local value */
	err = SENCERR_SUCCESS;
	for( j = 0; j < ECHO_DATA_NUM; j++ )
	{
		Enc->TxData.EncCmdNormal.sdata.TestPattern[j] = data;
	}

	/*----------------------------------------------------------------------*/
	/*	Send EchoBack Command and Check Answer								*/
	/*----------------------------------------------------------------------*/
	for( i = 0; i < 4; i++ )
	{
		if( SencSend1Command( Enc, SPG_NOP ) != FALSE )
		{
			if( SencSend1Command( Enc, SPG_ECHO ) != FALSE )
			{
				/* Check the Echo Back */
				for( j = 0; j < ECHO_DATA_NUM; j++ )
				{
					if( Enc->RxData.EncRspNormal.sdata.Echo[j] != data )
					{
						err = SENCERR_PGECHO;
						break;
					}
				}
			}
			else
			{
				err = SENCERR_PGECHO;
			}
		}
		else
		{
			err = SENCERR_PGCOM;
		}
	}

	return err;
}



/****************************************************************************
*
*		シリアルエンコーダ : パラメータ(8byte)の読み込み
*
****************************************************************************/
BOOL SencReadParam8byte( SENCX *Enc, USHORT Addr, UCHAR *PrmBuf )
{
#if !defined(_VC_DEBUG_) /* 2012.08.20 Y.Oka Make Mercury Base Software */
	LONG			i;
	USHORT			Operand;
	USHORT			rdVal;
	ULONG			RxFlg;

	/*----------------------------------------------------------------------*/
	/*	NOPコマンドの送信＆応答の受信										*/
	/*----------------------------------------------------------------------*/
	if(FALSE != SencSend1Command(Enc, SPG_NOP))
	{
		/*------------------------------------------------------------------*/
		/*	コマンドの送信＆応答の受信										*/
		/*------------------------------------------------------------------*/
		Operand = Addr | 0x0800;								/* Make  Operand */
		Enc->TxData.EncCmdPrmAccess.sdata.Operand = Operand;	/* Write Operand */
		Enc->TxData.EncCmdPrmAccess.sdata.CmdCode = SPG_PREAD;	/* Write Command */

		for( i = 0; i < 128; i++ )
		{
			/* Send Command Data to Encoder */
			SHalDrv_SencSetTxData( (void*)Enc->TxBuf, (void*)&(Enc->TxData) );
			/* Set Encode Transmission Start Request to JL-086 */
			SHalDrv_SencStartTx( (void*)Enc->TxStart, (void*)&(Enc->TxCmd) );

			KlibWaitus( 250 );

			/* Get Encode Receive Status from JL-086 */
			RxFlg = SHalDrv_SencGetRxStatus( (void*)Enc->RxFlg );
			/* Get Encode Received Data from JL-086 */
			SHalDrv_SencGetRxData( (void*)Enc->RxBuf, (void*)&(Enc->RxData) );


			if( ((RxFlg & 0x8000) != 0)
				&& ((Enc->RxData.EncRspPrmAccess.sdata.header & 0x03FF) == SPG_PREAD)
				&& (Enc->RxData.EncRspPrmAccess.sdata.Operand == Operand) )
			{
				rdVal = Enc->RxData.EncRspPrmAccess.sdata.PrmData[0];
				PrmBuf[0] = (UCHAR)rdVal;
				PrmBuf[1] = (UCHAR)(rdVal >> 8);

				rdVal = Enc->RxData.EncRspPrmAccess.sdata.PrmData[1];
				PrmBuf[2] = (UCHAR)rdVal;
				PrmBuf[3] = (UCHAR)(rdVal >> 8);

				rdVal = Enc->RxData.EncRspPrmAccess.sdata.PrmData[2];
				PrmBuf[4] = (UCHAR)rdVal;
				PrmBuf[5] = (UCHAR)(rdVal >> 8);

				rdVal = Enc->RxData.EncRspPrmAccess.sdata.PrmData[3];
				PrmBuf[6] = (UCHAR)rdVal;
				PrmBuf[7] = (UCHAR)(rdVal >> 8);

				return( TRUE );
			}
		}
	}

	return( FALSE );

#else /* !defined(_VC_DEBUG_) */ /* 2012.08.20 Y.Oka Make Mercury Base Software */

	*(ULONG*)&(PrmBuf[0]) = *(ULONG*)&MotorPrm[Addr];
	*(ULONG*)&(PrmBuf[4]) = *(ULONG*)&MotorPrm[Addr+4];
	return( TRUE );

#endif /* !defined(_VC_DEBUG_) */ /* 2012.08.20 Y.Oka Make Mercury Base Software */
}



/****************************************************************************
*
*		シリアルエンコーダ : 対象バンクの切替を行う
*
****************************************************************************//* <S0D7> */
BOOL SencBankChange( SENCX *Enc, USHORT BankNum )
{
	LONG			i;
	ULONG			RxFlg;

	/*----------------------------------------------------------------------*/
	/*	NOPコマンドの送信＆応答の受信										*/
	/*----------------------------------------------------------------------*/
	if(FALSE != SencSend1Command(Enc, SPG_NOP))
	{
		/* Set Command Code */
		Enc->TxData.EncCmdBank.sdata.CmdCode = SPG_BANK;
		/* Set Bank Number */
		Enc->TxData.EncCmdBank.sdata.SBankNum = (BankNum & 0x00FF);
		
		for( i = 0; i < 128; i++ )
		{	
			/* Send Command Data to Encoder */
			SHalDrv_SencSetTxData( (void*)Enc->TxBuf, (void*)&(Enc->TxData) );
			/* Set Encode Transmission Start Request to JL-086 */
			SHalDrv_SencStartTx( (void*)Enc->TxStart, (void*)&(Enc->TxCmd) );
	
			KlibWaitus( 250 );
	
			/* Get Encode Receive Status from JL-086 */
			RxFlg = SHalDrv_SencGetRxStatus( (void*)Enc->RxFlg );
			/* Get Encode Received Data from JL-086 */
			SHalDrv_SencGetRxData( (void*)Enc->RxBuf, (void*)&(Enc->RxData) );
	
			/* Check Receive Status and Response Header */
			if( ((RxFlg & 0x8000) != 0)
				&& ((Enc->RxData.EncRspBank.sdata.header & 0x03FF) == SPG_BANK)
				&& ((Enc->RxData.EncRspBank.sdata.Echo_BankNum & 0x00FF) == (BankNum & 0x00FF)) )
			{
				return( TRUE );
			}
	
	
		}
	}
	return( FALSE );
	
}


#define	MOTPRM_EX_ADDR		0x005C											  /* <S0DC>	*/
#define	BANK6_FULL_SIZE		0x0100											  /* <S0DC>	*/
#define	BANK6_USE_SIZE		0x0040											  /* <S0DC>	*/
#define	BANK0_USE_SIZE		0x00A0											  /* <S0DC>	*/
#define	BANK6_NOUSE_SUM		(BANK6_FULL_SIZE - BANK6_USE_SIZE - 1) * 0xFF	  /* <S0DC>	*/
/****************************************************************************
*
*		シリアルエンコーダ : 全パラメータの読み込み処理
*
****************************************************************************/
SENC_ERROR SencReadParamAll( MENCV *MencV, UCHAR *PrmBuf )
{
	USHORT		addr;
	USHORT		sumx;
	UCHAR		*tmpBuf;
	UCHAR		workBuf[8];
	SENC_ERROR	err;

	sumx = 0;
	err = SENCERR_SUCCESS;
	/*----------------------------------------------------------------------*/
	/*		モータパラメータ読み込み処理(Bank0：Σ-Ⅴ互換エリア)			*/
	/*----------------------------------------------------------------------*/
	/* Bank0から値を取得する												*/
//	for( addr = 0; addr < 0x00A0; addr += 8 )								  /* <S0DC> */
	for( addr = 0; addr < BANK0_USE_SIZE; addr += 8 )						  /* <S0DC> */
	{
		tmpBuf = &PrmBuf[addr];
		if( SencReadParam8byte( &MencV->EncIf, addr, tmpBuf ) == FALSE )
		{
			MencV->EncConnect = FALSE;
			return( SENCERR_PGCOM );	/* 異常リターン(通信異常) */
		}
		sumx += (tmpBuf[0] + tmpBuf[1] + tmpBuf[2] + tmpBuf[3]
					+ tmpBuf[4] + tmpBuf[5] + tmpBuf[6] + tmpBuf[7]);
	}
	/*----------------------------------------------------------------------*/
	/*		サムチェック													*/
	/*----------------------------------------------------------------------*/
#if !defined(_VC_DEBUG_) /* 2012.08.20 Y.Oka Make Mercury Base Software */
	/* Bank0(00ｈ～9Eh)のサムチェック										*/
	if( (sumx & 0x00FF) != 0 )
	{
		MencV->EncConnect = FALSE;
		err = SENCERR_PRMSUM;	/* 異常リターン(サムチェック) */
	}
#endif /* #if !defined(_VC_DEBUG_) */ /* 2012.08.20 Y.Oka Make Mercury Base Software */

/* <S0D7>:start */
	/*----------------------------------------------------------------------*/
	/*		モータパラメータ読み込み処理(Bank6：Σ-7拡張エリア)				*/
	/*----------------------------------------------------------------------*/
	if( (PrmBuf[MOTPRM_EX_ADDR] & 0x01) == 0 )					/* <S0DC>	*//* <S0ED> */
	{ /* Bank6から値を取得する */
		sumx = 0;
		if(SencBankChange( &MencV->EncIf, 6 ) != FALSE)	/* Bankを6に変更する。	*/
		{
//			for( addr = 0; addr < 0x0040; addr += 8 )							  /* <S0DC> */
			for( addr = 0; addr < BANK6_USE_SIZE; addr += 8 )					  /* <S0DC> */
			{
//				tmpBuf = &PrmBuf[addr + 0x00A0];								  /* <S0DC> */
				tmpBuf = &PrmBuf[addr + BANK0_USE_SIZE];						  /* <S0DC> */
				if( SencReadParam8byte( &MencV->EncIf, addr, tmpBuf ) == FALSE )
				{
					MencV->EncConnect = FALSE;
					return( SENCERR_PGCOM );	/* 異常リターン(通信異常) */
				}
				sumx += (tmpBuf[0] + tmpBuf[1] + tmpBuf[2] + tmpBuf[3]
						 + tmpBuf[4] + tmpBuf[5] + tmpBuf[6] + tmpBuf[7]);
			}

			/* 未使用領域データ加算(処理時間短縮のため読み出さない)				*//* <S0DC> */
			sumx += BANK6_NOUSE_SUM;

			/* Bank6のサムチェックを取得する。									*/
//			addr = 0x00FF;														  /* <S0DC> */
			addr = 0x00F8;														  /* <S0DC> */
			if( SencReadParam8byte( &MencV->EncIf, addr, workBuf ) == FALSE )
			{
				MencV->EncConnect = FALSE;
				return( SENCERR_PGCOM );			/* 異常リターン(通信異常) */
			}
			sumx += workBuf[7];
			/* Bank0に戻す														*/
			SencBankChange( &MencV->EncIf, 0 );			/* Bankを0に変更する。	*/
		}

		/*----------------------------------------------------------------------*/
		/*		サムチェック													*/
		/* 取得した値が全てFFhだった場合は、Bank6の新規ﾊﾟﾗﾒｰﾀに対応していない	*/
		/*----------------------------------------------------------------------*/
		/* Bank6(00ｈ～3Fh)のサムチェック										*/
		if( ((sumx & 0x00FF) != 0)			/* Bank6の新規パラメータ対応		*/
				&& (sumx != (0xFF * 0x41)) )	/* Bank6の新規パラメータ未対応考慮	*/
		{
			MencV->EncConnect = FALSE;
			err = SENCERR_PRMSUM;				/* 異常リターン(サムチェック)	*/
		}
	}
/* <S0D7>:end */	

	return( err );
}



/****************************************************************************
*
*		シリアルエンコーダ : 通信速度設定
*
****************************************************************************/
static BOOL SencSendSetComCommand( SENCX *Enc, UCHAR BaudRate )
{
	LONG			i;
	ULONG			RxFlg;

	for( i = 0; i < 128; i++ )
	{
		/* Set Command Code */
		Enc->TxData.EcnCmdSetCom.sdata.CmdCode = SPG_SETCOM;

		/* Set Baud Rate */
		Enc->TxData.EcnCmdSetCom.cdata.blank = 0x00;
		Enc->TxData.EcnCmdSetCom.cdata.BaudRate = BaudRate;

		/* Send Command Data to Encoder */
		SHalDrv_SencSetTxData( (void*)Enc->TxBuf, (void*)&(Enc->TxData) );
		/* Set Encode Transmission Start Request to JL-086 */
		SHalDrv_SencStartTx( (void*)Enc->TxStart, (void*)&(Enc->TxCmd) );

		KlibWaitus( 250 );

		/* Get Encode Receive Status from JL-086 */
		RxFlg = SHalDrv_SencGetRxStatus( (void*)Enc->RxFlg );
		/* Get Encode Received Data from JL-086 */
		SHalDrv_SencGetRxData( (void*)Enc->RxBuf, (void*)&(Enc->RxData) );

		/* Check Receive Status and Response Header */
		if( ((RxFlg & 0x8000) != 0)
			&& ((Enc->RxData.EcnRspSetCom.sdata.header & 0x03FF) == SPG_SETCOM)
			&& (Enc->RxData.EcnRspSetCom.cdata.Echo == SPG_SETCOM)
			&& (Enc->RxData.EcnRspSetCom.cdata.BaudRate == BaudRate) )
		{
			return( TRUE );
		}
	}

	return( FALSE );
}



/****************************************************************************************************/
/*																									*/
/*		シリアルエンコーダ : 接続確認処理															*/
/*																									*/
/****************************************************************************************************/
/*		エンコーダ接続チェックのため、２秒間ＢＲＥＡＫコマンドを送信し続ける。						*/
/****************************************************************************************************/
SENC_ERROR SencChkConnect( SENCX *Enc )/* <S04D> */
{
	ULONG		PastTime;	/* <S223> */
	ULONG		doTimer;	/* <S223> */
	SENC_ERROR	err;

	err = SENCERR_SUCCESS;

	/*	Send Break Command and Check Answer 1st time */
	if( FALSE == SencSend1Command(Enc, SPG_BREAK) )
	{ /* Connection Error */
		/*	Send Break Command and Check Answer Retry */
		if( FALSE == SencSend1Command(Enc, SPG_BREAK) )
		{
			err = SENCERR_PGCOM;
		}
		else{ /* Valid Connection */ }
	}
	else{ /* Valid Connection */ }

	/*----------------------------------------------------------------------------------------------*/
	/*	Send Break Command and Check Answer															*/
	/*	(For Mitutoyo-Abs Scale and EnDat Interface Scale )											*/
	/*----------------------------------------------------------------------------------------------*/
	/*	待ち時間 2sec																				*/
	/*		ミツトヨアブソスケール、EnDat I/Fスケールでは、起動のために2sec待ちが必要				*/
	/*	ToDo:実測結果と合わせて確認する必要あり。 													*/
	/*	ToDo:BREAKコマンドの応答がSUCCESSで本襠処理が抜けるため、問題は発生しないと思われる。		*/
	/*----------------------------------------------------------------------------------------------*//* <S223> */
	if( err != SENCERR_SUCCESS )
	{
		/* タイマ初期化 */
		KlibRstLongTimer(&doTimer);
		do
		{
			if( TRUE == SencSend1Command(Enc, SPG_BREAK) )				/* Wait 16ms=125us*128		*/
			{/* 応答確認OK */
				err = SENCERR_SUCCESS;
			}
			PastTime = KlibGetLongTimerMs( doTimer );					/* unit:[32ns/count]->[ms] */
		}while( (ENCODER_WAIT_TIMEOUT > PastTime)&&(err != SENCERR_SUCCESS) );

	}
	return	err;
}



/****************************************************************************************************/
/*																									*/
/*		シリアルエンコーダ : エコーバック確認処理													*/
/*																									*/
/****************************************************************************************************/
static SENC_ERROR SencChkEchoback( SENCX *Enc )
{
	SENC_ERROR	err;
	err = SENCERR_SUCCESS;

	/*	Send ECHO Command and Check Answer	*/
	err = SencCheckEchoBack( Enc, 0xFFFF );
	if( err == SENCERR_SUCCESS )
	{
	/*----------------------------------------------------------------------------------------------*/
	/* Initialize Communication																		*/
	/*----------------------------------------------------------------------------------------------*/
		/* Send Nop Command and Check Answer */
		if( FALSE == SencSend1Command(Enc, SPG_NOP) )
		{
			/* Send Nop Command and Check Answer Retry */
			if( FALSE == SencSend1Command(Enc, SPG_NOP) )
			{
				err = SENCERR_PGCOM;
			}
			else{ /* Valid Connection */ }
		}
		else{ /* Valid Connection */ }

		if( err == SENCERR_SUCCESS )
		{
			/*	Send ECHO Command and Check Answer	*/
			err = SencCheckEchoBack( Enc, 0x0000 );
		}
		else{ /* Connection Error */ }
	}
	else{ /* Connection Error */ }

	return( err );
}



/****************************************************************************************************/
/*																									*/
/*		シリアルエンコーダ : 接続プロセス															*/
/*																									*/
/****************************************************************************************************/
SENC_ERROR SencConnect( MENCV *MencV, ASIC_HW_IF_STR *AsicHwReg, USHORT EncBaudrate,  
										OPFBK_HNDL *FbkOpt, BOOL SencFbOptUse )/* <S04D>:FbkOpt,SencFbOptUse追加 */
{
#if !defined(_VC_DEBUG_) /* 2012.08.20 Y.Oka Make Mercury Base Software */

	SENCX 		*Enc;
	SENC_ERROR 	err;

	Enc = &MencV->EncIf;
	MencV->EncConnect = FALSE;			 	/* エンコーダ接続Flag		*/
	MencV->AbsoEncType = MENC_INCRE_TYPE;	/* 絶対値エンコーダタイプ	*/
	MencV->EncType = MENCTYPE_SERIAL;
	err = SENCERR_SUCCESS;

/* 2013.09.17 Y.Oka 可読性向上のため、外側へ移動 */ /* <S003> */
/*--------------------------------------------------------------------------------------------------*/
/*		開始処理(P0モード設定)																		*/
/*--------------------------------------------------------------------------------------------------*/
//	SencChangeP0Mode( MencV );

/*--------------------------------------------------------------------------------------------------*/
/*		通信速度8Mbps/4Mbps設定																		*/
/*--------------------------------------------------------------------------------------------------*/
//	SHalDrv_SetSencComRate( AsicHwReg, EncBaudrate );												/* <S04D> */
//	SHalDrv_SetSencComRate( AsicHwReg, EncBaudrate, MencV->EncIf.ch );
	SHalDrv_SetSencComRate( MencV->EncIf.SencSetReg, EncBaudrate, MencV->EncIf.ch );				  /* <S0E4> */			

/*--------------------------------------------------------------------------------------------------*/
/*		エンコーダ接続確認																			*/
/*--------------------------------------------------------------------------------------------------*/
	if((SencFbOptUse == TRUE ) && (FbkOpt->var.f.ChekedConnect))/* <S04D> フィードバックオプション時処理追加 */
	{
		err = SENCERR_SUCCESS/*FbkOpt->var.f.Connect*/;//<S0C7>
	}
	else
	{
		err = SencChkConnect( Enc );
	}
/*--------------------------------------------------------------------------------------------------*/
/*		エンコーダ未接続時の処理																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( err != SENCERR_SUCCESS )
	{
		; /* @@アラームマスク追加 */
		/* TODO:未接続時は、以下の処理を実施するのか？MencV->EncConnect = FALSE;にしてReturnすべき？*/
		/* TODO:エコーバックチェックによりエラー値が上書きされる。*/
	}

/*--------------------------------------------------------------------------------------------------*/
/*		エコーバック確認																			*/
/*--------------------------------------------------------------------------------------------------*/
	err = SencChkEchoback( Enc );

	if( err == SENCERR_SUCCESS )
	{
		MencV->EncConnect = TRUE;									 	/* エンコーダ接続Flag		*/
	}
	else
	{
		MencV->EncConnect = FALSE;									 	/* エンコーダ接続Flag		*/
	}

	return	err;

#else /* !defined(_VC_DEBUG_) */ /* 2012.08.20 Y.Oka Make Mercury Base Software */

//	SHalDrv_SetSencComRate( AsicHwReg, SENC_COMRATE_4MBPS );										/* <S04D> */
	SHalDrv_SetSencComRate( AsicHwReg, EncBaudrate, MencV->EncIf->ch );
	MencV->EncConnect = TRUE;			 	/* エンコーダ接続Flag		*/
	return SENCERR_SUCCESS;

#endif /* !defined(_VC_DEBUG_) */ /* 2012.08.20 Y.Oka Make Mercury Base Software */

}



/****************************************************************************************************/
/*																									*/
/*		シリアルエンコーダ : 通信速度切り替え処理													*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	概要 ：	要求のあった通信速度への切り替え要求を行う。切り替えはSET_COM(0xFA)を使用する。			*/
/*			SET_COMコマンド送信前に、通信速度切り替えおよび要求された通信速度に対応しているか		*/
/*			どうかをエンコーダ定数より判定する。													*/
/*																									*/
/*			通信速度切り替え、要求通信速度いずれかに対応できていない機種であれば、SET_COM			*/
/*			コマンドは送信せず、エラーなしで戻る仕様とする。アラームとはしない。					*/
/*																									*/
/****************************************************************************************************//*<S09C>*/
SENC_ERROR SencExchngBaudrate( MENCV *MencV, MENC_INIT_PRM *MencInitPrm,
								ASIC_HW_IF_STR *AsicHwReg, UCHAR BaudRate, BOOL *EncExchgSupported )
{
	BOOL		Supported;
	SENCX 		*Enc;
	SENC_ERROR	err;

	Enc = &MencV->EncIf;
	err = SENCERR_SUCCESS;
	Supported = TRUE;

	/*----------------------------------------------------------------------------------------------*/
	/*		Check if the Encoder Supports Baud Rate Exchange										*/
	/*----------------------------------------------------------------------------------------------*/
	if( (SENCBR_SUPPORT & MencInitPrm->MencP->chng_baudrate) == FALSE )
	{ /* Connected Encoder doesn't support Baud Rate Exchange */
		Supported = FALSE;
	}
	else
	{ /* Connected Encoder doesn't support Baud Rate Exchange */
		/*------------------------------------------------------------------------------------------*/
		/*		Check if the Encoder Supports Request Baud Rate										*/
		/*------------------------------------------------------------------------------------------*/
		if( BaudRate == SENC_COMRATE_8MBPS )			/* Exchange to 8MBps						*/
		{
			if( (SENCBR_8MBPS_SUP & MencInitPrm->MencP->chng_baudrate) == FALSE )
			{
				Supported = FALSE;
			}
			else
			{
				Supported = TRUE;
			}
		}
		else if( BaudRate == SENC_COMRATE_4MBPS )			/* Exchange to 4MBps		<S076>		*/
		{
			if( (SENCBR_8MBPS_SUP & MencInitPrm->MencP->chng_baudrate) == FALSE )
			{
				Supported = FALSE;
			}
			else
			{
				Supported = TRUE;
			}
		}
		else										/* Exchange to other speed (Not Supported)	*/
		{ 
			Supported = FALSE;
		}
	}

	/*----------------------------------------------------------------------------------------------*/
	/*		Request Baud Rate Exchange to Encoder													*/
	/*----------------------------------------------------------------------------------------------*/
	if( Supported == TRUE )
	{
		if( FALSE == SencSendSetComCommand(Enc, BaudRate+1) )		/* +1はSET_COMコマンド用補正	*/
		{ /* SET_COM Command Answer Back Error */
			err |= SENCERR_BRCHNGFAIL;
		}
		else
		{ /* SET_COM Command Answer Back OK */
			/* Wait 40ms */
			KlibWaitms( 40 );

			/*--------------------------------------------------------------------------------------*/
			/*		Baud Rate Setting to Servo UDL													*/
			/*--------------------------------------------------------------------------------------*/
//			SHalDrv_SetSencComRate( AsicHwReg, BaudRate, MencV->EncIf.ch );								/* <S04D>:引数追加 */
			SHalDrv_SetSencComRate( MencV->EncIf.SencSetReg, BaudRate, MencV->EncIf.ch );								/* <S04D>:引数追加 */
			MencV->PgBaudRate = BaudRate;			/* <S192> */

			/*--------------------------------------------------------------------------------------*/
			/*		Send ECHO Command and Check Answer												*/
			/*--------------------------------------------------------------------------------------*/
			err |= SencCheckEchoBack( Enc, 0x0000 );
		}
	}
	else
	{
		; /* Connected Encoder doesn't support Baud Rate Exchange */
	}

	/*----------------------------------------------------------------------------------------------*/
	/*		Set Baud Rate Exchange State															*/
	/*----------------------------------------------------------------------------------------------*/
	*EncExchgSupported = Supported;

	return err;
}

/****************************************************************************
*
*		シリアルエンコーダ : 初回位置データを読み込み
*
****************************************************************************/
SENC_ERROR SencReadInitialP1Data( MENCV *MencV )
{
	LONG	i, cnt;
	VBOOL	ReadOk;
	SENCX	*Enc;
	ULONG	RxFlg;

	Enc = &MencV->EncIf;
	RxFlg = 0;

	/* レジスタ同期解除 */
//	AsicHwReg->AREG_SEPGSET01 |= (BIT8);

	for( i = 0; i < 5; i++ )	/* Retry 5 Times for Linear			*/
	{
		/* Start Send Command */
		ReadOk = TRUE;
		if(MencV->EncIf.ch == 0 )/* <S04D>:Ch1ﾓｰﾄﾞ追加 */
		{/* ch0で接続確認	*/
			Enc->TxCmd = KPX_CDAT_ENC0_ESYNC;		/* <S04D>:rename */
		}
		else
		{/* ch1で接続確認	*/
			Enc->TxCmd = KPX_CDAT_ENC1_ESYNC;
		}

#if 0
		/* 送信開始チェック(受信現在値=0となったらOK) */
		cnt = 0;
		while( (Enc->RxFlg[0] & 0x1000) != 0 )
		{
			KlibWaitus(1);
			if((++cnt) > 65)
			{	ReadOk = FALSE; break;	}
		}
#endif
		/* 受信完了チェック(ラッチ値=現在値=1となったらOK) */
		cnt = 0;
		while( (RxFlg & 0x9000) != 0x9000 )
		{
			/* Get Encode Receive Status from JL-086 */
			RxFlg = SHalDrv_SencGetRxStatus( (void*)Enc->RxFlg );

			KlibWaitus(1);
			if((++cnt) > 65)
			{
				ReadOk = FALSE;
				break;
			}
		}

		if(ReadOk != FALSE)
		{
			/* 2012.08.18 Y.Oka Make Mercury Base Software */
			/* Get Encode Received Data from JL-086 */
			SHalDrv_SencGetRxData( (void*)Enc->RxBuf, (void*)&(Enc->RxData) );

			/* Set Encoder Data to Internal Variables */
			MencV->RxAlmCode		= Enc->RxData.EncRspP1Mode.sdata[2];
			MencV->RxTimex.w.l		= Enc->RxData.EncRspP1Mode.sdata[3];
			MencV->RxTimex.w.h		= Enc->RxData.EncRspP1Mode.sdata[4];
			MencV->RxPosL[0].w.l	= Enc->RxData.EncRspP1Mode.sdata[5];
			MencV->RxPosL[0].w.h	= Enc->RxData.EncRspP1Mode.sdata[6];
			MencV->RxPosH[0]		= Enc->RxData.EncRspP1Mode.sdata[7];

			/* 2012.08.18 Y.Oka Make Mercury Base Software */


			/*	アラームチェック : インクレ時 (アブソ時は不要)	*/
			if( (MencV->AbsoEncoder != FALSE)
				|| ((MencV->RxAlmCode & 0x00B5) == 0x0001) )
			{ /* 位置データ読み込み完了 */
//				ReadOk = TRUE;
				break;
			}
			else
			{
				ReadOk = FALSE;
			}
		}
	}

	/* レジスタ同期解除 */
//	AsicHwReg->AREG_SEPGSET01 &= ~(BIT8);

	/*	データ読み込み正常チェック	*/
	if( ReadOk == FALSE )
	{
		MencV->EncDataNG = TRUE;	/* エンコーダデータ無効				*/
		return( SENCERR_PGCOM );
	}

	return SENCERR_SUCCESS;
}

/****************************************************************************
*
*		シリアルエンコーダ情報からのＩＤ情報設定
*
****************************************************************************/
void SencSetIDINFO( USHORT addr, UCHAR *buffer, IDINFO *idinfo )
{
	LONG	i;
	SHORT	swk;

	switch ( addr )
	{
	case 0x00 :
	case 0x20 :
		 for ( i = 0; i < 8; i++ )
		 {
			idinfo->Model[i] = buffer[i];
		 }
		 break;

	case 0x08 :
	case 0x28 :
		 for ( i = 0; i < 6; i++ )
		 {
			idinfo->Model[8+i] = buffer[i];
		 }
		 swk = (SHORT) ( buffer[6] / 10 );
		 idinfo->Date[0] = MlibAsciiTbl[swk];
		 idinfo->Date[1] = MlibAsciiTbl[buffer[6]-(swk*10)];
		 idinfo->Date[2] = '.';
		 swk = (SHORT) ( buffer[7] / 10 );
		 idinfo->Date[3] = MlibAsciiTbl[swk];
		 idinfo->Date[4] = MlibAsciiTbl[buffer[7]-(swk*10)];
		 break;

	case 0x10 :
	case 0x30 :
		 for ( i = 0; i < 8; i++ )
		 {
			idinfo->SNo[i] = buffer[i];
		 }
		 break;

	case 0x18 :
	case 0x38 :
		 for ( i = 0; i < 6; i++ )
		 {
			idinfo->SNo[8+i] = buffer[i];
		 }
		 swk = ((SHORT)buffer[7] << 8) | (SHORT)buffer[6];
		 if((USHORT)swk > 9999 ) swk = 0;					/* <S214> */
		 idinfo->SNo[14] = MlibAsciiTbl[swk/1000];
		 swk = swk % 1000;
		 idinfo->SNo[15] = MlibAsciiTbl[swk/100];
		 swk = swk % 100;
		 idinfo->SNo[16] = MlibAsciiTbl[swk/10];
		 swk = swk % 10;
		 idinfo->SNo[17] = MlibAsciiTbl[swk];
		 break;

	case 0x98 :
		 for ( i = 0; i < 7; i++ )
		 {
			idinfo->Model[14+i] = buffer[i];
		 }
		 break;
	default :
		 break;
	}
}


/****************************************************************************
*
*		ＩＤ情報への終端コード挿入
*
****************************************************************************/
void SencFillNullIDINFO( IDINFO *idinfo )
{
	INT i;
	UCHAR ucwkl;

	/* Fill Null to Model Information End */
	for(i = 0; i < sizeof(idinfo->Model); i++)
	{
		ucwkl = idinfo->Model[i];
		if((ucwkl == ' ') || (ucwkl == '\0') || (ucwkl > 0x7F))
		{
			break;
		}
	}
	for( /*i = 0*/; i < sizeof(idinfo->Model); i++)
	{
		idinfo->Model[i] = '\0';
	}

	/* Fill Null to Date Information End */
	idinfo->Date[sizeof(idinfo->Date) - 1] = '\0';

	/* Fill Null to Serial No. Information End */
	for(i = 0; i < sizeof(idinfo->SNo); i++)
	{
		ucwkl = idinfo->SNo[i];
		if((ucwkl == ' ') || (ucwkl == '\0') || (ucwkl > 0x7F))
		{
			break;
		}
	}
	for( /*i = 0*/; i < sizeof(idinfo->SNo); i++)
	{
		idinfo->SNo[i] = '\0';
	}

}



/*--------------------------------------------------------------------------*/
/*	Origin Reset Data Table													*/
/*--------------------------------------------------------------------------*/
#define	SCNVID_JL029		0x0000		/* Serial Converter with JL-029	*/
#define	SCNVID_JL059		0x0001		/* Serial Converter with JL-059	*/
#define	SCNVID_UNKNOWN		0xFFFF		/* Unknown Serial Converter		*/
/*--------------------------------------------------------------------------*/
static const USHORT OrgRstTbl[2][4] = {
    { SCNVID_JL029, 0x8047, 0x0006, 0x0000 }, /* Serial Converter with JL-029	*/
    { SCNVID_JL059, 0x8029, 0x0011, 0x0000 }, /* Serial Converter with JL-059	*/
};

/****************************************************************************
*
*	シリアルエンコーダ：原点リセットデータテーブルの取得
*
****************************************************************************/
static CUSHORT *SencGetOrgRstTbl( IDINFO *pEncInfo )
{
	LONG TblIdx;
	USHORT ScnvId;

	/* Check Serial Converter Model for ScnvId */
	ScnvId = SCNVID_JL059;
	if((pEncInfo->Model[5] == 'A') || (pEncInfo->Model[5] == 'C'))
	{
		ScnvId = SCNVID_JL029;
	}

	/* Search Origin Reset Table */
	for(TblIdx = 0; TblIdx < sizeof(OrgRstTbl)/sizeof(OrgRstTbl[0]); TblIdx++)
	{
		if(OrgRstTbl[TblIdx][0] == ScnvId) /* Check ScnvID	*/
		{
			return (&OrgRstTbl[TblIdx][0]); /* Get Origin Reset Table */
		}
	}
	return (NULL);
}


/****************************************************************************
 *
 *	シリアルエンコーダ：原点リセットコマンドの送信
 *	シリアルコンバータに対して、原点リセットを行う
 *
 ****************************************************************************/
SENC_ERROR SencSendRstOrgCmd( MENCV *MencV )
{
	LONG 			i;
	SENC_ERROR		rcx;
	CUSHORT			*OrgRstTbl;
	SENCX			*Enc;

	Enc = &MencV->EncIf;
	rcx = SENCERR_SUCCESS; /* Send Complete */

	if(MencV->OrgClrByWrAdrCmd != FALSE)
	{
		/*------------------------------------------------------------------*/
		/*	Get Origin Reset Table											*/
		/*------------------------------------------------------------------*/
		OrgRstTbl = SencGetOrgRstTbl(MencV->pActEncInfo);						/* <S04D> mod */
		if(OrgRstTbl == NULL)
		{/* No Table (Unknown Serial Converter)	*/
			return SENCERR_PGCOM;/* A.CF1 : ＦＰＧ通信異常	*/
		}
		/*------------------------------------------------------------------*/
		/*	Send Open Key Command											*/
		/*------------------------------------------------------------------*/
		//for(i = 0; i < 7; i++)/*<S018>*/
		for(i = 1; i < 8; i++)
		{
			//Enc->TxData.EncCmdKeyCntrl.sdata.KeyWord[i] = OpenKeyTbl[i];
			Enc->TxData.UsBuf[i]  = OpenKeyTbl[i];
		}

		if(FALSE == SencSendXCommand(Enc, SPG_OPENKEY))
		{
			rcx = SENCERR_PGCOM;/* A.CF1 : ＦＰＧ通信異常	*/
		}
		/*------------------------------------------------------------------*/
		/*	Set Send Data for WriteAddress Command							*/
		/*------------------------------------------------------------------*/
		Enc->TxData.UsBuf[1] = OrgRstTbl[1];			/* Set Send Data	*/
		Enc->TxData.UsBuf[2] = OrgRstTbl[2];			/* Set Send Data	*/
		for(i = 3; i < 8; i++)
		{
			Enc->TxData.UsBuf[i] = 0x0000U;
		}
		/*------------------------------------------------------------------*/
		/*	Send Write Address Command										*/
		/*------------------------------------------------------------------*/
		if(FALSE == SencSendXCommand(Enc, SPG_WRADR))
		{
			rcx = SENCERR_PGCOM;/* A.CF1 : ＦＰＧ通信異常	*/
		}
		/*------------------------------------------------------------------*/
		/*	Send Close Key Command											*/
		/*------------------------------------------------------------------*/
		if(FALSE == SencSendXCommand(Enc, SPG_CLOSEKEY))
		{
			rcx = SENCERR_PGCOM;/* A.CF1 : ＦＰＧ通信異常	*/
		}
	}
	else
	{
		/*------------------------------------------------------------------*/
		/*	Send Origin Clear Command										*/
		/*------------------------------------------------------------------*/
		if(MencV->AbsoEncoder == FALSE)
		{
			if(FALSE == SencSendXCommand(Enc, SPG_ORGCLR))
			{
				rcx = SENCERR_PGCOM; /* A.CF1 : ＦＰＧ通信異常	*/
			}
		}
	}

	return (rcx);
}



/****************************************************************************
  Description: シリアルエンコーダマルチターン(多回転量)リセット処理
-----------------------------------------------------------------------------
  Parameters	:
		MENCV *MencV				: Pointer to the Encoder work Handle
		ASIC_HW_IF_STR *AsicHwReg	: Pointer to the ASIC register
  Return		:
		SENC_ERROR
*****************************************************************************/
SENC_ERROR SencResetMultiturn(MENCV *MencV, ULONG limmlt, ULONG bit_dp_b_h)
{
	ULONG			ulwk0;
	ULONG			ulwk1;
	SENC_ERROR		rslt;
	SENCX			*Enc;
	ULONG			RxFlg;
	LONG			RsltChkEncRsp;	/* <S0BB> */

	Enc = &MencV->EncIf;
	rslt = SENCERR_BUSY;
#if 0		/*  <S0BB> Delete Start*/
	switch(MencV->MtrSeqStep)
	{
	case  0: /* Initial Step */
		/* Stop ScanA */
		MencV->SenReady = FALSE;	/* Reset SenReady		*/
		MencV->EncDisable = TRUE;	/* Set Encoder Disable	*/

		/*	シリアルエンコーダP0モード設定 */
		SencChangeP0Mode(MencV);

		/* Check SigmaIII Encoder */
		if(MencV->Sigma3Encoder != FALSE)
		{
			/* プロテクトモード解除	*/
			SencSendP0Command(Enc, SPG_PMODEOFF, 0);
			MencV->MtrSeqStep = 1;
		}
		else
		{
			/* マルチターンリセットコマンド	*/
			SencSendP0Command(Enc, SPG_MTRST, 0);
			MencV->MtrSeqStep = 2;
		}
		KlibRstLongTimer(&MencV->MtrSeqWait);
		break;

	case  1: /* SigmaIII Encoder Mode */
		if(KlibGetLongTimerMs(MencV->MtrSeqWait) > 100)
		{ /* プロテクトモード解除完了 */
			KlibRstLongTimer(&MencV->MtrSeqWait);

			/* エンコーダ通信 送信開始 & 受信データ取得 */
			SencSendP0Command(Enc, SPG_PMODEOFF, 0);		/*<S03F>*/

			/* @@超暫定対応@@ 多軸NGのため、見直すこと	<S03F> */
			KlibWaitus( 250 );

			RxFlg = SencSendAndReceiveData( Enc );
			/* エンコーダ通信 応答チェック */
			if( 0 == SencCheckP0CmdEchoBak(&Enc->RxData, RxFlg, SPG_PMODEOFF, 0) )		/*<S03F>*/
			{
				/* マルチターンリセットコマンド	*/
				SencSendP0Command(Enc, SPG_MTRST, 0);
				MencV->MtrSeqStep = 2;
			}
			else
			{/* プロテクトモード解除失敗	*/
				/*	シリアルエンコーダP1モード設定 */
				SencChangeP1Mode(MencV);

				MencV->MtrSeqStep = 0;
				rslt = SENCERR_PGCOM;
			}
		}
		break;

	case  2: /* Multi-turn Reset Command */
		/* プロテクトモード解除完了待ち<S03F> */
		if(KlibGetLongTimerMs(MencV->MtrSeqWait) > 100)
		{
			KlibRstLongTimer(&MencV->MtrSeqWait);

			/* エンコーダ通信 送信開始 & 受信データ取得 */
			SencSendP0Command(Enc, SPG_MTRST, 0);		/*<S03F>*/

			/* @@超暫定対応@@ 多軸NGのため、見直すこと <S03F>*/
			KlibWaitus( 250 );

			/* エンコーダ通信 送信開始 & 受信データ取得 */
			RxFlg = SencSendAndReceiveData( Enc );
			/* エンコーダ通信 応答チェック */
			if( 0 == SencCheckP0CmdEchoBak(&Enc->RxData, RxFlg, SPG_MTRST, 0) )		/*<S03F>*/
			{/* リセット完了 */
				MencV->MtrSeqStep = 3;
			}
			else
			{/* プロテクトモード解除失敗	*/
				MencV->MtrSeqStep = 0;
				rslt = SENCERR_PGCOM;
			}
			/*	シリアルエンコーダP1モード設定 */
			SencChangeP1Mode(MencV);
		}
		break;

#if 0	/*<S03F>*/
	case  3: /* Check Multi-turn data */
		if(KlibGetLongTimerMs(MencV->MtrSeqWait) > 4)
		{
			/* 2012.08.18 Y.Oka Make Mercury Base Software */
			/* ★Serial Encoder I/F 変更予定★ */

			/*	受信データ読み込み */
//			ulwk0 = MencV->RxPosL[0].w.h = (USHORT)((MencV->EncIf.AsicHwReg)->AREG_SE0_RXD67);
//			ulwk1 = MencV->RxPosH[0] = ((MencV->EncIf.AsicHwReg)->AREG_SE0_RXD67 >> 16);

			/* ScanAで読み込み済みのはず Y.Oka */
			/* ★割禁をどうするか・・・★ */
			ulwk0 = MencV->RxPosL[0].w.h;
			ulwk1 = MencV->RxPosH[0];
			/* ★割禁をどうするか・・・★ */

			ulwk1 = (USHORT)(((ulwk1 << 16) | (USHORT)ulwk0) >> (bit_dp_b_h - 64));

			if( (ulwk1 <= 1) ||			/* ulwk1 = 1      : CCWに１回転	*/
				(ulwk1 == limmlt) )		/* ulwk1 = LIMMTT : CW に１回転	*/
			{
				rslt = SENCERR_SUCCESS;
			}
			else
			{
				rslt = SENCERR_PGCOM;
			}
			MencV->MtrSeqStep = 0;
		}
		break;
#endif	/*#if 0 <S03F> */

	case  3: /* Check Multi-turn data <S03F> */
		if(KlibGetLongTimerMs(MencV->MtrSeqWait) > 4)
		{
			/*	受信データ読み込み */
			ulwk0 = MencV->RxPosL[0].w.h = (USHORT)(MencV->EncIf.SencSetReg)->AREG_SE0_RXD67;
			ulwk1 = MencV->RxPosH[0] = (USHORT)((MencV->EncIf.SencSetReg)->AREG_SE0_RXD67 >> 16);

			ulwk1 = (USHORT)(((ulwk1 << 16) | (USHORT)ulwk0) >> (bit_dp_b_h - 64));

			if( (ulwk1 <= 1) ||			/* ulwk1 = 1      : CCWに１回転	*/
				(ulwk1 == limmlt) )		/* ulwk1 = LIMMTT : CW に１回転	*/
			{
				rslt = SENCERR_SUCCESS;
			}
			else
			{
				rslt = SENCERR_PGCOM;
			}
			MencV->MtrSeqStep = 0;
		}
		break;

	default:
		break;
	}
#endif	/* <S0BB> Delete End */
/*------------------------------------------------------------------------------------------------------------------------------------------------*/
/* <S0BB>  Add Start*/
	switch(MencV->MtrSeqStep)
	{

	/* Initial Step */
	case  0:
		/* Stop ScanA */
		MencV->SenReady = FALSE;		/* Reset SenReady		*/
		MencV->EncDisable = TRUE;		/* Set Encoder Disable	*/

		/*	シリアルエンコーダP0モード設定 */
		SencChangeP0Mode(MencV);

		MencV->MtrSeqStep = 1;
		break;

	/* Check SigmaIII Encoder Step */
	case  1:
		/* Check SigmaIII Encoder */
		if(MencV->Sigma3Encoder != FALSE)
		{
			MencV->MtrSeqStep = 2;
		}
		else
		{
			MencV->MtrSeqStep = 5;
		}
		break;

	/* SigmaIII Encoder : Clear Protect Mode Step */
	case  2:
		/* プロテクトモード解除 送信 */
		SencSendP0Command(Enc, SPG_PMODEOFF, 0);
		MencV->MtrSeqStep = 3;
		break;

	/* SigmaIII Encoder : プロテクトモード解除コマンド 送信 Step */
	case  3:
		/* プロテクトモード解除 送信 */
		SencSendP0Command(Enc, SPG_PMODEOFF, 0);

		/* 送信回数 更新 */
		MencV->FnMturnClrCmdSndNum ++;

		MencV->MtrSeqStep = 4;
		break;

	/* SigmaIII Encoder : Check Protect Mode Step */
	case  4:
		/* エンコーダ通信 送信開始 & 受信データ取得 */
		RxFlg = SencSendAndReceiveData( Enc );

		/* エンコーダ通信 応答チェック */
		RsltChkEncRsp = SencCheckP0CmdEchoBak(&Enc->RxData, RxFlg, SPG_PMODEOFF, 0);
		if( RsltChkEncRsp == 0 )			/* 受信正常完了 プロテクトモード解除成功 */
		{
			MencV->FnMturnClrCmdSndNum = 0;	/* 送信回数 0クリア */
			MencV->MtrSeqStep = 5;
		}
		else if( RsltChkEncRsp == BUSSY )	/* エンコーダ通信応答 受信中 */
		{
			MencV->MtrSeqStep = 3;

			/* 送信回数が100回を超えたらエラーとする  */
			if( MencV->FnMturnClrCmdSndNum > CMD_SND_NUM )
			{
				/*	シリアルエンコーダP1モード設定 */
				SencChangeP1Mode(MencV);

				MencV->MtrSeqStep = 0;
				//rslt = SENCERR_PGCOM;
				rslt = SENCERR_PGCLR;/* <S1BD> */
			}
		}
		else
		{/* プロテクトモード解除失敗	*/
			/*	シリアルエンコーダP1モード設定 */
			SencChangeP1Mode(MencV);

			MencV->MtrSeqStep = 0;
			//rslt = SENCERR_PGCOM;
			rslt = SENCERR_PGCLR;/* <S1BD> */
		}
		break;

	/* Multi-turn Reset Command Step */
	case  5:
		/* マルチターンリセットコマンド	*/
		SencSendP0Command(Enc, SPG_MTRST, 0);
		MencV->MtrSeqStep = 6;
		break;

	/* Multi-turn Reset Command : マルチターンリセットコマンド 送信 Step */
	case  6:
		/* マルチターンリセットコマンド 送信 */
		SencSendP0Command(Enc, SPG_MTRST, 0);

		/* 送信回数 更新 */
		MencV->FnMturnClrCmdSndNum ++;

		MencV->MtrSeqStep = 7;
		break;

	/* Check Multi-turn Reset Step */
	case  7:
		/* エンコーダ通信 送信開始 & 受信データ取得 */
		RxFlg = SencSendAndReceiveData( Enc );

        /* リセット処理実行中(BUSY状態) 暫定処置のため見直し必要 <S10F> */
        KlibRstLongTimer(&MencV->MtrSeqWait);
        while( ((Enc->RxData.UsBuf[0] & 0x0300) == 0x0100) &&
                (KlibGetLongTimerMs(MencV->MtrSeqWait) < 100) )
        {
            RxFlg = SencSendAndReceiveData( Enc );
        }

		/* エンコーダ通信 応答チェック */
		RsltChkEncRsp= SencCheckP0CmdEchoBak(&Enc->RxData, RxFlg, SPG_MTRST, 0);

		if( RsltChkEncRsp == 0 )			/* 受信正常完了 マルチターンリセット完了 */
		{/* リセット完了 */
			MencV->FnMturnClrCmdSndNum = 0;	/* 送信回数 0クリア */
			MencV->MtrSeqStep = 8;
		}
		else if( RsltChkEncRsp == BUSSY )	/* エンコーダ通信応答 受信中 */
		{
			MencV->MtrSeqStep = 6;

			/* 送信回数が100回を超えたらエラーとする  */
			if( MencV->FnMturnClrCmdSndNum > CMD_SND_NUM )
			{
				MencV->MtrSeqStep = 0;
				//rslt = SENCERR_PGCOM;
				rslt = SENCERR_PGCLR;/* <S1BD> */
			}
		}
		else
		{/* プロテクトモード解除失敗	*/
			MencV->MtrSeqStep = 0;
			//rslt = SENCERR_PGCOM;
			rslt = SENCERR_PGCLR;/* <S1BD> */
		}
		/*	シリアルエンコーダP1モード設定 */
		SencChangeP1Mode(MencV);

		/* 現在経過時間取得 */
		KlibRstLongTimer(&MencV->MtrSeqWait);
		break;

	/* Wait 2ms Step */
	case  8:
		/* マルチターンリセット完了から2[ms]経過を確認 */
		if(KlibGetLongTimerMs(MencV->MtrSeqWait) > 2)
		{
			MencV->MtrSeqStep = 9;
		}
		break;


	/* Check Multi-turn data Step */
	case  9:
		/*	受信データ読み込み */
		if( Enc->ch == 0 )			/* <S143> */
		{
			ulwk0 = MencV->RxPosL[0].w.h = (USHORT)(MencV->EncIf.SencSetReg)->AREG_SE0_RXD67;
			ulwk1 = MencV->RxPosH[0] = (USHORT)((MencV->EncIf.SencSetReg)->AREG_SE0_RXD67 >> 16);
		}
		else
		{
			ulwk0 = MencV->RxPosL[0].w.h = (USHORT)(MencV->EncIf.SencSetReg)->AREG_SE1_RXD67;
			ulwk1 = MencV->RxPosH[0] = (USHORT)((MencV->EncIf.SencSetReg)->AREG_SE1_RXD67 >> 16);
		}

		ulwk1 = (USHORT)(((ulwk1 << 16) | (USHORT)ulwk0) >> (bit_dp_b_h - 64));

		if( (ulwk1 <= 1) ||			/* ulwk1 = 1      : CCWに１回転	*/
			(ulwk1 == limmlt) )		/* ulwk1 = LIMMTT : CW に１回転	*/
		{
			rslt = SENCERR_SUCCESS;
		}
		else
		{
			//rslt = SENCERR_PGCOM;
			rslt = SENCERR_PGCLR;/* <S1BD> */
		}
		MencV->MtrSeqStep = 0;
		break;

	default:
		break;
	}
/* <S0BB>  Add End*/

	return rslt;
}

/****************************************************************************
  Description: シリアルエンコーダマルチターンリミット変更処理
-----------------------------------------------------------------------------
  Parameters	:
		MENCV *MencV				: Pointer to the Encoder work Handle
		ASIC_HW_IF_STR *AsicHwReg	: Pointer to the ASIC register
  Return		:
		SENC_ERROR
*****************************************************************************/
SENC_ERROR SencChangeMultiturnLimit(MENCV *MencV, USHORT mtlmt)
{
	SENC_ERROR		rslt;
	ULONG			RxFlg;
	SENCX			*Enc;

	Enc = &MencV->EncIf;
	rslt = SENCERR_BUSY;

	switch(MencV->MtrSeqStep)
	{
	case  0: /* Initial Step */
		/* Stop ScanA */
		MencV->SenReady = FALSE;	/* Reset SenReady		*/
		MencV->EncDisable = TRUE;	/* Set Encoder Disable	*/

		/*	シリアルエンコーダP0モード設定 */
		SencChangeP0Mode(MencV);

		/* Check SigmaIII Encoder */
		if(MencV->Sigma3Encoder != FALSE)
		{
			/* プロテクトモード解除	*/
			SencSendP0Command(Enc, SPG_PMODEOFF, 0);
			MencV->MtrSeqStep = 1;
		}
		else
		{
			/* マルチターンリミット変更コマンド	*/
			SencSendP0Command(Enc, SPG_MTLMTCHG, mtlmt);
			MencV->MtrSeqStep = 2;
		}
		KlibRstLongTimer(&MencV->MtrSeqWait);
		break;

	case  1: /* SigmaIII Encoder Mode */
		if(KlibGetLongTimerMs(MencV->MtrSeqWait) > 100)
		{
			KlibRstLongTimer(&MencV->MtrSeqWait);

			/* エンコーダ通信 送信開始 & 受信データ取得 */
			SencSendP0Command(Enc, SPG_PMODEOFF, 0);	/*<S03F>*/

			/* @@超暫定対応@@ 多軸NGのため、見直すこと <S03F> */
			KlibWaitus( 250 );

			/* エンコーダ通信 送信開始 & 受信データ取得 <S03F> */
			RxFlg = SencSendAndReceiveData( Enc );
			/* エンコーダ通信 応答チェック */
			if(0 == SencCheckP0CmdEchoBak(&Enc->RxData, RxFlg, SPG_PMODEOFF, 0))
			{
				/* マルチターンリミット変更コマンド	*/
				SencSendP0Command(Enc, SPG_MTLMTCHG, mtlmt);
				MencV->MtrSeqStep = 2;
			}
			else	/* プロテクトモード解除失敗	*/
			{
				/*	シリアルエンコーダP1モード設定 */
				SencChangeP1Mode(MencV);

				MencV->MtrSeqStep = 0;
				rslt = SENCERR_PGCOM;
			}
		}
		break;

	case  2: /* Multi-turn Reset Command */
		if(KlibGetLongTimerMs(MencV->MtrSeqWait) > 100)
		{
			KlibRstLongTimer(&MencV->MtrSeqWait);

			/* エンコーダ通信 送信開始 & 受信データ取得 */
			SencSendP0Command(Enc, SPG_MTLMTCHG, 0);	/*<S03F>*/

			/* @@超暫定対応@@ 多軸NGのため、見直すこと <S03F> */
			KlibWaitus( 250 );

			/* エンコーダ通信 送信開始 & 受信データ取得 */
			RxFlg = SencSendAndReceiveData( Enc );
			/* エンコーダ通信 応答チェック */
			if( (0 == SencCheckP0CmdEchoBak(&Enc->RxData, RxFlg, SPG_MTLMTCHG, 0))
				&& (mtlmt == Enc->RxData.EncRspMTLimit.sdata.MultiTurnLimit) )
			{
				rslt = SENCERR_SUCCESS;
			}
			else	/* プロテクトモード解除失敗	*/
			{
				MencV->MtrSeqStep = 0;
				rslt = SENCERR_PGCOM;
			}
			/*	シリアルエンコーダP1モード設定 */
			SencChangeP1Mode(MencV);
		}
		break;

	default:
		break;
	}

	return rslt;
}

/****************************************************************************
  Description: 電流零位相設定
-----------------------------------------------------------------------------
  Parameters	:
		MENCV *MencV				: Pointer to the Encoder work Handle
		ASIC_HW_IF_STR *AsicHwReg	: Pointer to the ASIC register
  Return		:
		SENC_ERROR
*****************************************************************************/
SENC_ERROR SencWriteMotorPhaseOffset( MENCV *MencV, USHORT PhaseOffset )
{
	LONG			wkl1, wkl2;
	USHORT			wkus;
	SENCX			*Enc;
	ULONG			RxFlg;
	SENC_ERROR		err;

	Enc = &MencV->EncIf;
	err = SENCERR_BUSY;

	switch(MencV->PoleWrSeqStep)
	{
	/*----------------------------------------------------------------------*/
	case  0: /* NOPコマンド発行処理											*/
	/*----------------------------------------------------------------------*/
		SencChangeP0Mode(MencV);
		SencSendP0Command(Enc, SPG_NOP, 0x0000);
		MencV->PoleWrSeqStep = 1;
		KlibRstLongTimer(&MencV->PoleWrSeqWait);
		break;
	/*----------------------------------------------------------------------*/
	case  1: /* NOPコマンドエコー確認処理 ＆ OPEN_KEYコマンド発行処理		*/
	/*----------------------------------------------------------------------*/
		/* エンコーダ通信 送信開始 & 受信データ取得 */
		RxFlg = SencSendAndReceiveData( Enc );
		/* エンコーダ通信 応答チェック */
		if(0 == SencCheckP0CmdEchoBak(&Enc->RxData, RxFlg, SPG_NOP, 0x0000))
		{
			/* OPEN_KEYコマンド発行 */
			SencSendP0Command(Enc, SPG_OPENKEY, 0x0000);
			KlibRstLongTimer(&MencV->PoleWrSeqWait);
			MencV->PoleWrSeqStep = 2;
		}
		else
		{
			if(KlibGetLongTimerMs(MencV->PoleWrSeqWait) > 20)
			{
				err = SENCERR_PGCOM;/* 書込み失敗処理へ	*/
			}
		}
		break;
	/*----------------------------------------------------------------------*/
	case  2: /* OPEN_KEYコマンドエコー確認処理								*/
	/*----------------------------------------------------------------------*/
		/* エンコーダ通信 送信開始 & 受信データ取得 */
		RxFlg = SencSendAndReceiveData( Enc );
		/* エンコーダ通信 応答チェック */
		if(0 == SencCheckP0CmdEchoBak(&Enc->RxData, RxFlg, SPG_OPENKEY, 0x0000))
		{
			/* READ_PRMコマンド発行 */
			SencSendP0Command(Enc, SPG_PREAD, 0x029e);
			KlibRstLongTimer(&MencV->PoleWrSeqWait);
			MencV->PoleWrSeqStep = 6;
		}
		else
		{
			if(KlibGetLongTimerMs(MencV->PoleWrSeqWait) > 20)
			{
				err = SENCERR_PGCOM;/* 書込み失敗処理へ	*/
			}
		}
		break;
	/*----------------------------------------------------------------------*/
	case  6: /* READ_PRMコマンドエコー確認処理 ＆ NOPコマンド発行処理		*/
	/*----------------------------------------------------------------------*/
		/* エンコーダ通信 送信開始 & 受信データ取得 */
		RxFlg = SencSendAndReceiveData( Enc );
		/* エンコーダ通信 応答チェック */
		if(0 == SencCheckP0CmdEchoBak(&Enc->RxData, RxFlg, SPG_PREAD, 0x029e))
		{
			/* チェックサム読み込み	*/
			MencV->PoleWrSum = Enc->RxData.EncRspPrmAccess.sdata.PrmData[0];
			/* NOPコマンド発行 */
			SencSendP0Command(Enc, SPG_NOP, 0x0000);
			KlibRstLongTimer(&MencV->PoleWrSeqWait);
			MencV->PoleWrSeqStep = 7;
		}
		else
		{
			if(KlibGetLongTimerMs(MencV->PoleWrSeqWait) > 20)
			{
				err = SENCERR_PGCOM;/* 書込み失敗処理へ	*/
			}
		}
		break;
	/*----------------------------------------------------------------------*/
	case  7: /* NOPコマンドエコー確認処理 ＆ READ_PRMコマンド発行処理		*/
	/*----------------------------------------------------------------------*/
		/* エンコーダ通信 送信開始 & 受信データ取得 */
		RxFlg = SencSendAndReceiveData( Enc );
		/* エンコーダ通信 応答チェック */
		if(0 == SencCheckP0CmdEchoBak(&Enc->RxData, RxFlg, SPG_NOP, 0x0000))
		{
			/* READ_PRMコマンド発行 */
			SencSendP0Command(Enc, SPG_PREAD, 0x0250);
			KlibRstLongTimer(&MencV->PoleWrSeqWait);
			MencV->PoleWrSeqStep = 8;
		}
		else
		{
			if(KlibGetLongTimerMs(MencV->PoleWrSeqWait) > 20)
			{
				err = SENCERR_PGCOM;/* 書込み失敗処理へ	*/
			}
		}
		break;
	/*----------------------------------------------------------------------*/
	case  8: /* READ_PRMコマンドエコー確認処理 ＆ WRITE_PRMコマンド発行処理	*/
	/*----------------------------------------------------------------------*/
		/* エンコーダ通信 送信開始 & 受信データ取得 */
		RxFlg = SencSendAndReceiveData( Enc );
		/* エンコーダ通信 応答チェック */
		if(0 == SencCheckP0CmdEchoBak(&Enc->RxData, RxFlg, SPG_PREAD, 0x0250))
		{
			/*	書込み用チェックサム計算 */
			wkus				= Enc->RxData.EncRspPrmAccess.sdata.PrmData[0];
			wkl1				= (wkus & 0x00ff);
			wkl2				= ((wkus & 0xff00) >> 8);
			wkus				= (USHORT)(wkl1 + wkl2);
			wkl1				= (PhaseOffset & 0x00ff);
			wkl2				= ((PhaseOffset & 0xff00) >> 8);
			wkus				= (USHORT)((wkl1 + wkl2 - wkus) << 8);
			MencV->PoleWrSum	= (MencV->PoleWrSum & 0x00ff) | ((MencV->PoleWrSum - wkus) & 0xff00);

			/* 電流位相情報をセット		*/
			Enc->TxData.EncCmdPrmAccess.sdata.PrmData[0] = (USHORT)PhaseOffset;
			/* WRITE_PRMコマンド発行	*/
			SencSendP0Command(Enc, SPG_PWRITE, 0x0250);
			KlibRstLongTimer(&MencV->PoleWrSeqWait);
			MencV->PoleWrSeqStep = 9;
		}
		else
		{
			if(KlibGetLongTimerMs(MencV->PoleWrSeqWait) > 20)
			{
				err = SENCERR_PGCOM;/* 書込み失敗処理へ	*/
			}
		}
		break;
	/*----------------------------------------------------------------------*/
	case  9: /* WRITE_PRMコマンドエコーチェックおよびベリファイ処理			*/
	/*----------------------------------------------------------------------*/
		/* エンコーダ通信 送信開始 & 受信データ取得 */
		RxFlg = SencSendAndReceiveData( Enc );
		/* エンコーダ通信 応答チェック */
		if( (0 == SencCheckP0CmdEchoBak(&Enc->RxData, RxFlg, SPG_PWRITE, 0x0250))
			&& ((Enc->RxData.EncRspPrmAccess.sdata.PrmData[0] & 0xffff) == (PhaseOffset & 0xffff)) )
		{
			/* NOPコマンド発行 */
			SencSendP0Command(Enc, SPG_NOP, 0x0000 );
			KlibRstLongTimer(&MencV->PoleWrSeqWait);
			MencV->PoleWrSeqStep = 10;
		}
		else
		{
			if(KlibGetLongTimerMs(MencV->PoleWrSeqWait) > 200)
			{
				err = SENCERR_PGCOM;/* 書込み失敗処理へ	*/
			}
		}
		break;
	/*----------------------------------------------------------------------*/
	case  10: /* NOPコマンドエコー確認処理 ＆ WRITE_PRMコマンド発行処理		*/
	/*----------------------------------------------------------------------*/
		/* エンコーダ通信 送信開始 & 受信データ取得 */
		RxFlg = SencSendAndReceiveData( Enc );
		/* エンコーダ通信 応答チェック */
		if(0 == SencCheckP0CmdEchoBak(&Enc->RxData, RxFlg, SPG_NOP, 0x0000))
		{
			/* チェックサム値をセット */
			Enc->TxData.EncCmdPrmAccess.sdata.PrmData[0] = (USHORT)MencV->PoleWrSum;
			/* WRITE_PRMコマンド発行 */
			SencSendP0Command(Enc, SPG_PWRITE, 0x029e );
			KlibRstLongTimer(&MencV->PoleWrSeqWait);
			MencV->PoleWrSeqStep = 11;
		}
		else
		{
			if(KlibGetLongTimerMs(MencV->PoleWrSeqWait) > 20)
			{
				err = SENCERR_PGCOM;/* 書込み失敗処理へ	*/
			}
		}
		break;
	/*----------------------------------------------------------------------*/
	case  11: /* WRITE_PRMコマンドエコー確認およびベリファイ処理			*/
	/*----------------------------------------------------------------------*/
		/* エンコーダ通信 送信開始 & 受信データ取得 */
		RxFlg = SencSendAndReceiveData( Enc );
		/* エンコーダ通信 応答チェック */
		if( (0 == SencCheckP0CmdEchoBak(&Enc->RxData, RxFlg, SPG_PWRITE, 0x029e))
			&& (Enc->RxData.EncRspPrmAccess.sdata.PrmData[0] == MencV->PoleWrSum) )
		{
			/* CLOSE_KEYコマンド発行 */
			SencSendP0Command(Enc, SPG_CLOSEKEY, 0x0000 );
			KlibRstLongTimer(&MencV->PoleWrSeqWait);
			MencV->PoleWrSeqStep = 15;
		}
		else
		{
			if(KlibGetLongTimerMs(MencV->PoleWrSeqWait) > 200)
			{
				err = SENCERR_PGCOM;/* 書込み失敗処理へ	*/
			}
		}
		break;
	/*----------------------------------------------------------------------*/
	case  15: /* CLOSE_KEYコマンドエコーチェック処理						*/
	/*----------------------------------------------------------------------*/
		/* エンコーダ通信 送信開始 & 受信データ取得 */
		RxFlg = SencSendAndReceiveData( Enc );
		/* エンコーダ通信 応答チェック */
		if(0 == SencCheckP0CmdEchoBak(&Enc->RxData, RxFlg, SPG_CLOSEKEY, 0x0000))
		{
			MencV->PoleWrSeqStep = 16;
			KlibRstLongTimer(&MencV->PoleWrSeqWait);
			SencChangeP1Mode(MencV);
		}
		else
		{
			if(KlibGetLongTimerMs(MencV->PoleWrSeqWait) > 40)
			{
				err = SENCERR_PGCOM;/* 書込み失敗処理へ	*/
			}
		}
		break;
	/*----------------------------------------------------------------------*/
	case  16: /* スケール書込み完了通知処理									*/
	/*----------------------------------------------------------------------*/
		if(KlibGetLongTimerMs(MencV->PoleWrSeqWait) > 20)
		{
			err = SENCERR_SUCCESS;
			MencV->PoleWrSeqStep	= 0;
		}
		break;

	default:
		break;
	}

	/* エラー処理：P1モードへ */
	if((err != SENCERR_SUCCESS) && (err != SENCERR_BUSY))
	{
		SencChangeP1Mode(MencV);
		MencV->PoleWrSeqStep	= 0;
	}

	return err;
}

/****************************************************************************
  Description: スケール原点設定
-----------------------------------------------------------------------------
  Parameters	:
		MENCV *MencV				: Pointer to the Encoder work Handle
		ASIC_HW_IF_STR *AsicHwReg	: Pointer to the ASIC register
  Return		:
		SENC_ERROR
*****************************************************************************/
SENC_ERROR SencResetScaleZeroPosition(MENCV *MencV)
{
	LONG			wkl1, wkl2;
	SENCX			*Enc;
	ULONG			RxFlg;
	SENC_ERROR		err;

	Enc = &MencV->EncIf;
	err = SENCERR_BUSY;

	switch(MencV->PoleWrSeqStep)
	{
	/*----------------------------------------------------------------------*/
	case  0: /* NOPコマンド発行処理											*/
	/*----------------------------------------------------------------------*/
		SencChangeP0Mode(MencV);
		SencSendP0Command(Enc, SPG_NOP, 0x0000);
		MencV->PoleWrSeqStep = 1;
		KlibRstLongTimer(&MencV->PoleWrSeqWait);
		break;
	/*----------------------------------------------------------------------*/
	case  1: /* NOPコマンドエコー確認処理 ＆ OPEN_KEYコマンド発行処理		*/
	/*----------------------------------------------------------------------*/
		/* エンコーダ通信 送信開始 & 受信データ取得 */
		RxFlg = SencSendAndReceiveData( Enc );
		/* エンコーダ通信 応答チェック */
		if(0 == SencCheckP0CmdEchoBak(&Enc->RxData, RxFlg, SPG_NOP, 0x0000))
		{
			/* OPEN_KEYコマンド発行 */
			SencSendP0Command(Enc, SPG_OPENKEY, 0x0000);
			KlibRstLongTimer(&MencV->PoleWrSeqWait);
			MencV->PoleWrSeqStep = 2;
		}
		else
		{
			if(KlibGetLongTimerMs(MencV->PoleWrSeqWait) > 20)
			{
				err = SENCERR_PGCOM;/* 書込み失敗処理へ	*/
			}
		}
		break;

	/*----------------------------------------------------------------------*/
	case  2: /* OPEN_KEYコマンドエコー確認処理								*/
	/*----------------------------------------------------------------------*/
		/* エンコーダ通信 送信開始 & 受信データ取得 */
		RxFlg = SencSendAndReceiveData( Enc );
		/* エンコーダ通信 応答チェック */
		if(0 == SencCheckP0CmdEchoBak(&Enc->RxData, RxFlg, SPG_OPENKEY, 0x0000))
		{
			/* ZEROSETコマンド発行 */
			SencSendP0Command(Enc, SPG_ZEROSET, 0x0000 );
			KlibRstLongTimer(&MencV->PoleWrSeqWait);
			MencV->PoleWrSeqStep = 13;
		}
		else
		{
			if(KlibGetLongTimerMs(MencV->PoleWrSeqWait) > 20)
			{
				err = SENCERR_PGCOM;/* 書込み失敗処理へ	*/
			}
		}
		break;

	/*----------------------------------------------------------------------*/
	case  13: /* ZEROSETコマンドエコー確認									*/
	/*----------------------------------------------------------------------*/
		/* エンコーダ通信 送信開始 & 受信データ取得 */
		RxFlg = SencSendAndReceiveData( Enc );
		/* エンコーダ通信 応答チェック */
		if(0 == SencCheckP0CmdEchoBak(&Enc->RxData, RxFlg, SPG_ZEROSET, 0x0000))
		{
			/* CLOSE_KEYコマンド発行 */
			SencSendP0Command(Enc, SPG_CLOSEKEY, 0x0000 );
			KlibRstLongTimer(&MencV->PoleWrSeqWait);
			MencV->PoleWrSeqStep = 15;
		}
		else
		{
			if(KlibGetLongTimerMs(MencV->PoleWrSeqWait) > 800)
			{
				err = SENCERR_PGCOM;/* 書込み失敗処理へ	*/
			}
		}
		break;

	/*----------------------------------------------------------------------*/
	case  15: /* CLOSE_KEYコマンドエコーチェック処理						*/
	/*----------------------------------------------------------------------*/
		/* エンコーダ通信 送信開始 & 受信データ取得 */
		RxFlg = SencSendAndReceiveData( Enc );
		/* エンコーダ通信 応答チェック */
		if(0 == SencCheckP0CmdEchoBak(&Enc->RxData, RxFlg, SPG_CLOSEKEY, 0x0000))
		{
			MencV->PoleWrSeqStep = 16;
			KlibRstLongTimer(&MencV->PoleWrSeqWait);
			SencChangeP1Mode(MencV);
		}
		else
		{
			if(KlibGetLongTimerMs(MencV->PoleWrSeqWait) > 40)
			{
				err = SENCERR_PGCOM;/* 書込み失敗処理へ	*/
			}
		}
		break;
	/*----------------------------------------------------------------------*/
	case  16: /* 位置データ取り込み比較										*/
	/*----------------------------------------------------------------------*/
		if(KlibGetLongTimerMs(MencV->PoleWrSeqWait) > 20)
		{
			MencV->PoleWrSeqStep	= 0;
	//		wkl1 = MencV->RcvPosX[0] >> LMX_MPOS_SFT;
			wkl1 = 0;
			wkl2 = MlibMulgainNolim( ERR5DEG, MencV->Kinvphase );
			if( wkl2 > MlibLABS(wkl1) )
			{	/* ±５°位置ズレなし */
				err = SENCERR_SUCCESS;
			}
			else
			{	/* ±５°位置ズレあり */
				err = SENCERR_PRMDATA;
			}
		}
		break;

	default:
		break;
	}

	/* エラー処理：P1モードへ */
	if((err != SENCERR_SUCCESS) && (err != SENCERR_BUSY))
	{
		SencChangeP1Mode(MencV);
		MencV->PoleWrSeqStep	= 0;
	}

	return err;
}


#define MPRM_TRANS_BYTE		12		/* モータ定数転送バイト数		*/
/****************************************************************************
  Description: モータ定数スケール読み込み
-----------------------------------------------------------------------------
  Parameters	:
		MENCV *MencV		: Pointer to the Encoder work Handle
		USHORT address		: Address of the Encoder Parameter
		UINT num			: byte size to be read
		USHORT *buffer		: Pointer to buffer
  Return		:
		SENC_ERROR
*****************************************************************************/
SENC_ERROR SencReadMotorParameter(MENCV *MencV, USHORT address, UINT num, USHORT *buffer)
{
	SENC_ERROR		err;
	UINT 			MotPrmAddress, MotPrmSize;
	UINT 			i;
	UINT			offset;
	USHORT 			rdval;
	SENCX			*Enc;
	ULONG			RxFlg;

	Enc = &MencV->EncIf;
	err = SENCERR_BUSY;

	switch(MencV->MtrSeqStep)
	{
	/*----------------------------------------------------------------------*/
	case 0: /* 初期化処理/NOPコマンド発行処理								*/
	/*----------------------------------------------------------------------*/
		MencV->EncDisable = TRUE;
		SencChangeP0Mode(MencV);

		KlibRstLongTimer(&MencV->MtrSeqWait);
		MencV->MotPrmNum = 0;
		MencV->MtrSeqStep = 1;

		SencSendP0Command(Enc, SPG_NOP, 0x0000);
		break;
	/*----------------------------------------------------------------------*/
	case 1: /* NOPコマンドエコーチェック&READ_RRM発行処理					*/
	/*----------------------------------------------------------------------*/
		/* エンコーダ通信 送信開始 & 受信データ取得 */
		RxFlg = SencSendAndReceiveData( Enc );
		/* エンコーダ通信 応答チェック */
		if(0 == SencCheckP0CmdEchoBak(&Enc->RxData, RxFlg, SPG_NOP, 0x0000))
		{
			/* READ_PRMコマンドを発行 */
			offset = MencV->MotPrmNum * MPRM_TRANS_BYTE;
			MotPrmAddress = address + offset;
			MotPrmSize = ((num - offset) > MPRM_TRANS_BYTE) ?
			             MPRM_TRANS_BYTE : num - offset;

			SencSendP0Command(Enc, SPG_PREAD,
			                  (USHORT)(MotPrmSize << 8 | MotPrmAddress));

			KlibRstLongTimer(&MencV->MtrSeqWait);
			MencV->MtrSeqStep = 2;
		}
		else
		{
			if(KlibGetLongTimerMs(MencV->MtrSeqWait) > 20)
			{
				/*	シリアルエンコーダP1モード設定 */
				SencChangeP1Mode(MencV);
				MencV->MtrSeqStep = 0;
				err = SENCERR_PGCOM;
			}
		}
		break;
	/*----------------------------------------------------------------------*/
	case 2: /* READ_PRMコマンドエコーチェックおよびデータ読み込み処理		*/
	/*----------------------------------------------------------------------*/
		offset = MencV->MotPrmNum * MPRM_TRANS_BYTE;
		MotPrmSize = ((num - offset) > MPRM_TRANS_BYTE) ?
		             MPRM_TRANS_BYTE : num - offset;

		/* エンコーダ通信 送信開始 & 受信データ取得 */
		RxFlg = SencSendAndReceiveData( Enc );
		/* エンコーダ通信 応答チェック */
		if(0 == SencCheckP0CmdEchoBak(&Enc->RxData, RxFlg, SPG_PREAD,
								Enc->TxData.EncCmdPrmAccess.sdata.Operand))
		/* 2012.08.18 Y.Oka Make Mercury Base Software */
		{
			for(i = 0; i < ((MotPrmSize+1)>>1); i++)
			{
				/* 2012.08.18 Y.Oka Make Mercury Base Software */
				rdval = Enc->RxData.EncRspPrmAccess.sdata.PrmData[i];
				/* 2012.08.18 Y.Oka Make Mercury Base Software */
				buffer[(offset>>1)+i] = rdval;
			}
			MencV->MotPrmNum++;
			offset = MencV->MotPrmNum * MPRM_TRANS_BYTE;
			if(num > offset)
			{
				/* NOPコマンド発行 */
				SencSendP0Command(Enc, SPG_NOP, 0x0000);
				MencV->MtrSeqStep = 1;
			}
			else
			{
				/*	シリアルエンコーダP1モード設定 */
				SencChangeP1Mode(MencV);
				MencV->MtrSeqStep = 3;
			}
			KlibRstLongTimer(&MencV->MtrSeqWait);
		}
		else
		{
			if(KlibGetLongTimerMs(MencV->MtrSeqWait) > 20)
			{
				/*	シリアルエンコーダP1モード設定 */
				SencChangeP1Mode(MencV);
				MencV->MtrSeqStep = 0;
				err = SENCERR_PGCOM;
			}
		}
		break;
	/*----------------------------------------------------------------------*/
	case  3: /* 完了通知処理									*/
	/*----------------------------------------------------------------------*/
		if(KlibGetLongTimerMs(MencV->MtrSeqWait) > 20)
		{
			MencV->EncDisable = FALSE;
			err = SENCERR_SUCCESS;
			MencV->MtrSeqStep	= 0;
		}
		break;

	default:
		break;
	}

	return err;
}

/****************************************************************************
  Description: モータ定数スケール書き込み実行
-----------------------------------------------------------------------------
  Parameters	:
		MENCV *MencV		: Pointer to the Encoder work Handle
		USHORT address		: Address of the Encoder Parameter
		UINT num			: byte size to be read
		USHORT *buffer		: Pointer to buffer
  Return		:
		SENC_ERROR
*****************************************************************************/
SENC_ERROR SencWriteMotorParameter(MENCV *MencV, USHORT address, UINT num, USHORT *buffer)
{
	SENC_ERROR		err;
	UINT 			MotPrmAddress, MotPrmSize;
	UINT 			i, offset;
	SENCX			*Enc;
	ULONG			RxFlg;

	Enc = &MencV->EncIf;
	err = SENCERR_BUSY;

	switch(MencV->MtrSeqStep)
	{
	/*----------------------------------------------------------------------*/
	case 0: /* 初期化処理/NOPコマンド発行処理								*/
	/*----------------------------------------------------------------------*/
		MencV->SenReady = FALSE; /* 電源再投入までサーボオンさせない */
		MencV->EncDisable = TRUE;
		SencChangeP0Mode(MencV);

		KlibRstLongTimer(&MencV->MtrSeqWait);
		MencV->MotPrmNum = 0;
		MencV->MtrSeqStep = 1;

		SencSendP0Command(Enc, SPG_NOP, 0x0000);
		break;
	/*----------------------------------------------------------------------*/
	case  1: /* NOPコマンドエコー確認処理 ＆ OPEN_KEYコマンド発行処理		*/
	/*----------------------------------------------------------------------*/
		/* エンコーダ通信 送信開始 & 受信データ取得 */
		RxFlg = SencSendAndReceiveData( Enc );
		/* エンコーダ通信 応答チェック */
		if(0 == SencCheckP0CmdEchoBak(&Enc->RxData, RxFlg, SPG_NOP, 0x0000))
		{
			/* OPEN_KEYコマンド発行 */
			SencSendP0Command(Enc, SPG_OPENKEY, 0x0000);
			KlibRstLongTimer(&MencV->MtrSeqWait);
			MencV->MtrSeqStep = 2;
		}
		else
		{
			if(KlibGetLongTimerMs(MencV->MtrSeqWait) > 20)
			{
				/*	シリアルエンコーダP1モード設定 */
				SencChangeP1Mode(MencV);
				MencV->MtrSeqStep = 0;
				err = SENCERR_PGCOM;/* 書込み失敗処理へ	*/
			}
		}
		break;
	/*----------------------------------------------------------------------*/
	case  2: /* OPEN_KEYコマンドエコー確認処理								*/
	/*----------------------------------------------------------------------*/
		/* エンコーダ通信 送信開始 & 受信データ取得 */
		RxFlg = SencSendAndReceiveData( Enc );
		/* エンコーダ通信 応答チェック */
		if(0 == SencCheckP0CmdEchoBak(&Enc->RxData, RxFlg, SPG_OPENKEY, 0x0000))
		{
			/* NOPコマンド発行 */
			SencSendP0Command(Enc, SPG_NOP, 0x0000);
			KlibRstLongTimer(&MencV->MtrSeqWait);
			MencV->MtrSeqStep = 3;
		}
		else
		{
			if(KlibGetLongTimerMs(MencV->MtrSeqWait) > 20)
			{
				/*	シリアルエンコーダP1モード設定 */
				SencChangeP1Mode(MencV);
				MencV->MtrSeqStep = 0;
				err = SENCERR_PGCOM;/* 書込み失敗処理へ	*/
			}
		}
		break;
	/*----------------------------------------------------------------------*/
	case 3: /* NOPコマンドエコーチェック&WRITE_RRM発行処理					*/
	/*----------------------------------------------------------------------*/
		/* エンコーダ通信 送信開始 & 受信データ取得 */
		RxFlg = SencSendAndReceiveData( Enc );
		/* エンコーダ通信 応答チェック */
		if(0 == SencCheckP0CmdEchoBak(&Enc->RxData, RxFlg, SPG_NOP, 0x0000))
		{
			/* WRITE_PRMコマンドを発行 */
			offset = MencV->MotPrmNum * MPRM_TRANS_BYTE;
			MotPrmAddress = address + offset;
			MotPrmSize = ((num - offset) > MPRM_TRANS_BYTE) ?
						  MPRM_TRANS_BYTE : num - offset;

			for(i = 0; i < ((MotPrmSize+1)>>1); i++)
			{
				Enc->TxData.EncCmdPrmAccess.sdata.PrmData[i] = buffer[(offset>>1)+i];
			}

			SencSendP0Command(Enc, SPG_PWRITE, (USHORT)(MotPrmSize << 8 | MotPrmAddress));

			KlibRstLongTimer(&MencV->MtrSeqWait);
			MencV->MtrSeqStep = 4;
		}
		else
		{
			if(KlibGetLongTimerMs(MencV->MtrSeqWait) > 20)
			{
				/*	シリアルエンコーダP1モード設定 */
				SencChangeP1Mode(MencV);
				MencV->MtrSeqStep = 0;
				err = SENCERR_PGCOM;
			}
		}
		break;
	/*----------------------------------------------------------------------*/
	case 4: /* WRITE_PRMコマンドエコーチェックおよびデータ読み込み処理		*/
	/*----------------------------------------------------------------------*/
		/* エンコーダ通信 送信開始 & 受信データ取得 */
		RxFlg = SencSendAndReceiveData( Enc );
		/* エンコーダ通信 応答チェック */
		if(0 == SencCheckP0CmdEchoBak(&Enc->RxData, RxFlg,
				SPG_PWRITE, Enc->TxData.EncCmdPrmAccess.sdata.Operand))
		{
			MencV->MotPrmNum++;
			offset = MencV->MotPrmNum * MPRM_TRANS_BYTE;
			if(num > offset)
			{
				/* NOPコマンド発行 */
				SencSendP0Command(Enc, SPG_NOP, 0x0000);
				MencV->MtrSeqStep = 3;
			}
			else
			{
				/* CLOSE_KEYコマンド発行 */
				SencSendP0Command(Enc, SPG_CLOSEKEY, 0x0000 );
				MencV->MtrSeqStep = 5;
			}
			KlibRstLongTimer(&MencV->MtrSeqWait);
		}
		else
		{
			if(KlibGetLongTimerMs(MencV->MtrSeqWait) > MPRM_TRANS_BYTE * 20)
			{
				/*	シリアルエンコーダP1モード設定 */
				SencChangeP1Mode(MencV);
				MencV->MtrSeqStep = 0;
				err = SENCERR_PGCOM;
			}
		}
		break;
	/*----------------------------------------------------------------------*/
	case  5: /* CLOSE_KEYコマンドエコーチェック処理						*/
	/*----------------------------------------------------------------------*/
		/* エンコーダ通信 送信開始 & 受信データ取得 */
		RxFlg = SencSendAndReceiveData( Enc );
		/* エンコーダ通信 応答チェック */
		if(0 == SencCheckP0CmdEchoBak(&Enc->RxData, RxFlg, SPG_CLOSEKEY, 0x0000))
		{
			/*	シリアルエンコーダP1モード設定 */
			SencChangeP1Mode(MencV);
			KlibRstLongTimer(&MencV->MtrSeqWait);
			MencV->MtrSeqStep = 6;
		}
		else
		{
			if(KlibGetLongTimerMs(MencV->MtrSeqWait) > 40)
			{
				SencChangeP1Mode(MencV);
				MencV->MtrSeqStep = 0;
				err = SENCERR_PGCOM;/* 書込み失敗処理へ	*/
			}
		}
		break;
	/*----------------------------------------------------------------------*/
	case  6: /* 完了通知処理									*/
	/*----------------------------------------------------------------------*/
		if(KlibGetLongTimerMs(MencV->MtrSeqWait) > 20)
		{
			MencV->EncDisable = FALSE;
			err = SENCERR_SUCCESS;
			MencV->MtrSeqStep	= 0;
		}
		break;

	default:
		break;
	}

	return err;
}



/***************************************** end of file **********************************************/
