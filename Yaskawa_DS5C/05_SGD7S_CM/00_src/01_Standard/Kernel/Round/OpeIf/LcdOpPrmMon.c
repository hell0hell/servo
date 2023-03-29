/****************************************************************************************************/
/*																									*/
/*																									*/
/*		LcdOpPrmMon.c : LCDオペレータ処理モジュール:PnUn画面										*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*	機 能 : SigmaMessageによりＬＣＤオペレータと通信し、オペレータからの各種操作を実行する。		*/
/*																									*/
/*			1)モニタ機能 : 指定モニタ変数(UnXXX)の表示												*/
/*			2)パラメータ : 指定パラメータ(PnXXX)の表示,編集,変更									*/
/*			3)																						*/
/*			4)																						*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.1.30 : 2012.04.xx  K.Ozaki      Revised for Mercury 									*/
/*		Rev.1.40 : 2013.09.26  K.Ozaki		<S00C>													*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "KnlApi.h"			/* Kernel API */
#include "LcdOp.h"			/* LcdOpIf Common Header */
#include "Monitor.h"		/* Un (Monitor) Common Header */
#include "Parameter.h"		/* Pn (Parameter) Common Header */
#include "PrmAccessIf.h"	/* Parameter Access Management Header */

extern MSGIF LopMsg;					/* TODO: 消すLcdOpe：Message I/F										*/
#define RpxPrmUpdateEepromIdx( Idx ) (OK) /* TODO: */

/****************************************************************************************************/
/*																									*/
/*		Function List																				*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		ＬＣＤオペレータＩＦ関数																	*/
/*--------------------------------------------------------------------------------------------------*/
void	LcdPrmMonMain( MSGIF *hMsg, DRVSEL *pDrvSel );		/* PRM/MON Display Main Function		*/
void	LcdRstPnUnDspData( void );							/* Reset Pn/UN Display Data 			*/

/*--------------------------------------------------------------------------------------------------*/
/*		Local Functions																				*/
/*--------------------------------------------------------------------------------------------------*/
USHORT	LcdPnUnEditSub(										/* Pn/Un Edit Mode Procedure			*/
		USHORT	dispID,										/* ディスプレーID(LCDPMDSP_PM/JOG/etc)	*/
		OPKEYFLG *pKeyMk,									/* キー押下情報のアドレス				*/
		BLINKFLG *pBlnkKickFlg	);							/* ブリンク起動フラグポインター			*/
/*--------------------------------------------------------------------------------------------------*/
USHORT	LcdPnUnNumEdit( 
		USHORT dispID, 
		OPKEYFLG *pKeyMk, 
		BLINKFLG *pBlnkKickFlg );
/*--------------------------------------------------------------------------------------------------*/
USHORT	LcdPnUnSel( 
		USHORT dispID, 
		OPKEYFLG *pKeyMk,
		BLINKFLG *pBlnkKickFlg );
/*--------------------------------------------------------------------------------------------------*/
USHORT	LcdPnDataEdit( 
		USHORT dispID,
		OPKEYFLG *pKeyMk,
		BLINKFLG *pBlnkKickFlg );
/*--------------------------------------------------------------------------------------------------*/
void	LcdMkDspDataPnUnLines(			/* Make Display Data for Pn/Un Lines					*/
		USHORT	dispID,						/* ディスプレーID(LCDPMDSP_PM/JOG/etc)					*/
		USHORT	BlnkSupFlg,					/* ブリンク抑止フラグ									*/
		UCHAR	*ResBuf			);			/* 応答バッファアドレス									*/
/*--------------------------------------------------------------------------------------------------*/
USHORT	LcdPrmDataBlinkDpos( 
		CPRMDEF *pPrmDef );
/*--------------------------------------------------------------------------------------------------*/


/****************************************************************************************************/
/*																									*/
/*		Lcd PRM/MON Display Mode Main Function														*/
/*																									*/
/****************************************************************************************************/
void	LcdPrmMonMain( MSGIF *hMsg, DRVSEL *pDrvSel )
{
UCHAR	*ResBuf = hMsg->ResBuf;
/*--------------------------------------------------------------------------------------------------*/
/*		編集処理																					*/
/*--------------------------------------------------------------------------------------------------*/
		if( LCD_CHKCMD_RCVWAIT() ) 			/** オプションからの応答データ待ち以外の時は編集処理 **/
		{
			LcdPnUnEditSub(LCDPMDSP_PM, &OpKeyMk, &OpBlnkKickFlg);
		}
/*--------------------------------------------------------------------------------------------------*/
/*		表示データ作成処理 : Clear ResBuf															*/
/*--------------------------------------------------------------------------------------------------*/
		RpxLcdClrTxBuf( ResBuf );
/*--------------------------------------------------------------------------------------------------*/
/*		表示データ作成処理 : TITLE																	*/
/*--------------------------------------------------------------------------------------------------*/
//		if( (hMsg->AccessLvl == ACCLVL_USER1) /* TODO: &&  (Iprm.OpeDspMode == OPEDSP_SETUPPRM) */)	/* <S113> */
		if( (hMsg->AccessLvl == ACCLVL_USER1) &&  (hMsg->OpeDspMode == OPEDSP_SETUPPRM) )			/* <S113> */
		{
//			RpxLcdSetStr(0, 8, (UCHAR *)"-PRM/MON-", (UCHAR *)ResBuf);		/* <S154> */
			RpxLcdSetStr(0, 10, (UCHAR *)"PRM/MON", (UCHAR *)ResBuf);		/* <S154> */
		}
		else
		{
//			RpxLcdSetStr(0, 8, (UCHAR *)"-ALL PRM-", (UCHAR *)ResBuf);				/* <S154> */
			RpxLcdSetStr(0, 10, (UCHAR *)"ALL PRM", (UCHAR *)ResBuf);				/* <S154> */
		}
/*--------------------------------------------------------------------------------------------------*/
/*		表示データ作成処理 : Pn/Un Lines															*/
/*--------------------------------------------------------------------------------------------------*/
		LcdMkDspDataPnUnLines( LCDPMDSP_PM, FALSE, (UCHAR *)ResBuf );
/*--------------------------------------------------------------------------------------------------*/
/*		ブリンク＆ステータス表示処理																*/
/*--------------------------------------------------------------------------------------------------*/
		RpxLcdSetResStatus (ResBuf);			/* <S031> */
		RpxLcdSetBlink (ResBuf);				/* <S031> */
/*--------------------------------------------------------------------------------------------------*/
		return;
}

/****************************************************************************************************/
/*																									*/
/*		Pn/Un Edit mode (戻り値:TRUE=モードキー押下が検知されたことを示す)							*/
/*																									*/
/****************************************************************************************************/
USHORT	LcdPnUnEditSub(						/* Pn/Un Edit mode										*/
		USHORT	dispID,						/* ディスプレーID(LCDPMDSP_PM/JOG/etc)					*/
		OPKEYFLG *pKeyMk,					/* キー押下情報のアドレス								*/
		BLINKFLG *pBlnkKickFlg	)			/* ブリンク起動フラグポインター							*/
{
USHORT	ModeChg = FALSE;
#if CCSW_CMDID == CMDID_EXIST				/*<11J42>*/
LONG	rc_tmp;
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		Switch by EEPROM Write Mode																	*/
/*--------------------------------------------------------------------------------------------------*/
		switch( LopV.EewriteMode )
		{
	/*----------------------------------------------------------------------------------------------*/
		case OPWM_None:			/* パラメータ値表示・編集処理										*/
	/*----------------------------------------------------------------------------------------------*/
			switch( LopV.EditMode )
			{
			case LCDEM_PnUnNum:
				ModeChg = LcdPnUnNumEdit( dispID, pKeyMk, pBlnkKickFlg );
				break;
			case LCDEM_PnUnSel:
				ModeChg = LcdPnUnSel( dispID, pKeyMk, pBlnkKickFlg );
				break;
			case LCDEM_PnData:
				ModeChg = LcdPnDataEdit( dispID, pKeyMk, pBlnkKickFlg );
#if CCSW_CMDID == CMDID_EXIST				/*<11J42>*/
				if( ModeChg == TRUE )
				{
					/* オプションパラメータ編集中の時 */
					if( LcdOpt[dispID].Prm[LopV.ActiveLine].State.Editing == TRUE )
					{
						/* オプションパラメータ書き込み要求セット(不揮発性メモリ領域) */
						rc_tmp = OpiReqDegOpPrmWrite( LcdOpt[dispID].Prm[LopV.ActiveLine].CurrPnNo,
													  LcdOpt[dispID].Prm[LopV.ActiveLine].Size,
													  LcdOpt[dispID].Prm[LopV.ActiveLine].Data );
						if( rc_tmp == DEGOP_SUCCESS )
						{
							/* 応答データ待ちセット */
							LcdOpt[dispID].Prm[LopV.ActiveLine].State.RcvWait = TRUE;
							/* タイムアウト用タイマリセット */
							KpiRstLongTimer( &LcdOpt[dispID].ComTimeLong );
							/* 応答データ受信処理コール(注：正常終了時のみ更新) */
							rc_tmp = LcdOptUpdateResponseData(dispID, &LcdOpt[dispID]);
							while(rc_tmp == TRUE)
							{
								rc_tmp = LcdOptUpdateResponseData(dispID, &LcdOpt[dispID]);
							}
						}
					}
					else
					{
						rc_tmp = RpxPrmUpdateEepromIdx(LopDsp.InfTbl[dispID][LopV.ActiveLine].PnIdx);
						while(rc_tmp == PRM_EEPROM_WRITE_WAIT || rc_tmp == PRM_EEPROM_WRITE_FAIL)
						{
							rc_tmp = RpxPrmUpdateEepromIdx(LopDsp.InfTbl[dispID][LopV.ActiveLine].PnIdx);
						}
					}
				}
#endif
	 			/* 
				 * * Jupiter ではDataｷｰ押下時のみEEPROMを更新する *
				 */
				break;
			default:
				break;
			}
			break;
	/*----------------------------------------------------------------------------------------------*/
		case OPWM_UpdtWait:		/* EEPROM書き込み(要求登録)待ち処理									*/
	/*----------------------------------------------------------------------------------------------*/
			switch( RpxPrmUpdateEepromIdx( LopDsp.InfTbl[dispID][LopV.ActiveLine].PnIdx ) )
			{
			case OK:	/* 正常	*/
				pBlnkKickFlg->PrmEewrite = 1;	/* ブリンク起動	*/
				LopV.EewriteMode = OPWM_UpdtBlnk;
				break;
			default:
				break;
			}
			break;
	/*----------------------------------------------------------------------------------------------*/
		case OPWM_UpdtBlnk:		/* パラメータEEPROM書き込み時ブリンク処理							*/
	/*----------------------------------------------------------------------------------------------*/
			if( OpBlnkFlg.PrmEewrite == 0 )
			{
				LopV.EewriteMode = OPWM_None;
				LopV.EditMode = LCDEM_PnUnNum;
				LopV.CurDigit = 0;
			}
			break;
	/*----------------------------------------------------------------------------------------------*/
		case OPWM_InfWrWait:	/* 画面構成情報EEPROM書き込み不可時処理								*/
	/*----------------------------------------------------------------------------------------------*/
			if( LcdSaveDispInfoTable( dispID ) == OK )
			{
				pBlnkKickFlg->write = TRUE;
				LopV.EewriteMode = OPWM_None;
			}
			break;
	/*----------------------------------------------------------------------------------------------*/
		default:
	/*----------------------------------------------------------------------------------------------*/
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return( ModeChg );
}


/****************************************************************************************************/
/*																									*/
/*		Pn/Un Number Edit mode																		*/
/*																									*/
/****************************************************************************************************/
/*		dispID: ディスプレーID(LCDPMDSP_PM=PRM/MON画面, LCDPMDSP_JOG=JOG画面 etc.), 				*/
/*		pKeyMk: キー押下情報のアドレス, pBlnkKickFlg: ブリンク起動フラグポインター					*/
/*		戻り値: TRUE=モードキー押下が検知されたことを示す											*/
/****************************************************************************************************/
USHORT	LcdPnUnNumEdit( USHORT dispID, OPKEYFLG *pKeyMk, BLINKFLG *pBlnkKickFlg )
{
LONG	i;
CPRMDEF	*pPrmDef;
USHORT	Numx;
USHORT	PnUn  = LopDsp.InfTbl[dispID][LopV.ActiveLine].PnUn;
USHORT	UnIdx = LopDsp.InfTbl[dispID][LopV.ActiveLine].UnIdx;
USHORT	PnIdx = LopDsp.InfTbl[dispID][LopV.ActiveLine].PnIdx;
#if CCSW_CMDID == CMDID_EXIST			/*<11J42>*/
USHORT	idx_tmp, cur_num_tmp, cur_idx_tmp;
DEGOP_PRM	*OpPrm = &LcdOpt[dispID].Prm[LopV.ActiveLine];
LONG rc;
#endif
AXIS_HANDLE *pAxRsc = LopMsg.pAxRsc;										/* TODO: 後で消す */

/*--------------------------------------------------------------------------------------------------*/
/*		Pn/Un Number Edit																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( pKeyMk->mode )
		{
			return( TRUE );
		}
/*--------------------------------------------------------------------------------------------------*/
		else if( pKeyMk->up )
		{
			if( PnUn == UnSelected )
			{
				Numx = CUNMONX->GetNumber( CUNMONX, UnIdx, &LopMsg ) + (0x01 << (LopV.CurDigit*4));
				if( Numx >= 0x1000 ){ Numx = 0xFFF;}
				LopDsp.InfTbl[dispID][LopV.ActiveLine].UnIdx = 
					CUNMONX->SearchB( (COBJECT *)CUNMONX, Numx, UnIdx, &LopMsg );
			}
			else if( PnUn == PnSelected )
			{
/*<11J42>*/
#if CCSW_CMDID == CMDID_EXIST
/*--------------------------------------------------------------------------------------------------*/
/*			初期情報取得																			*/
/*--------------------------------------------------------------------------------------------------*/
			/* 現在サーボパラメータインデックス */
			cur_idx_tmp = LopDsp.InfTbl[dispID][LopV.ActiveLine].PnIdx;
			/* 現在サーボパラメータ番号 */
			cur_num_tmp = PnPrmTbl[cur_idx_tmp].PnNo;
			/* 次回サーボパラメータ番号 */
			Numx = cur_num_tmp + (0x01 << (LopV.CurDigit*4));
			if(Numx >= 0x1000)	Numx = 0x0FFF;
			/* 次回サーボパラメータインデックス */
			idx_tmp = RpxPrmSearchIdxOp(Numx, cur_idx_tmp, &LopMsg);
			OpPrm->NextSvIdx = idx_tmp;

/*--------------------------------------------------------------------------------------------------*/
/*			デジオペ通信状態取得																	*/
/*--------------------------------------------------------------------------------------------------*/
			rc = OpiGetDegOpState(&LcdOpt[dispID].PrmRange);

/*--------------------------------------------------------------------------------------------------*/
/*			現在編集中のパラメータ領域設定															*/
/*--------------------------------------------------------------------------------------------------*/
			/* ユーザパラメータ領域 */
			if( cur_num_tmp < OPT_PNNO_VIRTUAL )
			{
				OpPrm->State.ExceedLwrLmt = TRUE;					/* 下限値オーバフラグセット		*/
				OpPrm->CurrPnNo = LcdOpt[dispID].PrmRange.LwrLmt;	/* 現在オプションPrm番号更新	*/
			}
			/* システムパラメータ領域 */
			else if( cur_num_tmp > OPT_PNNO_VIRTUAL )
			{
				OpPrm->State.ExceedUprLmt = TRUE;					/* 上限値オーバフラグセット		*/
				OpPrm->CurrPnNo = LcdOpt[dispID].PrmRange.UprLmt;	/* 現在オプションPrm番号更新	*/
			}

/*--------------------------------------------------------------------------------------------------*/
/*			デジオペ通信有効時																		*/
/*--------------------------------------------------------------------------------------------------*/
			if( rc == DEGOP_ENABLE )
			{
			/*--------------------------------------------------------------------------------------*/
			/*	システム/オプションパラメータ領域の編集												*/
			/*--------------------------------------------------------------------------------------*/
				if( PnPrmTbl[idx_tmp].PnNo >= OPT_PNNO_VIRTUAL )
				{
					/* 次回オプションパラメータ番号を取得 */
					OpPrm->NextPnNo = LcdOptGetNextOpPrmNo( DEGOP_READ_UP, cur_idx_tmp, LopV.CurDigit, OpPrm );

				/*----------------------------------------------------------------------------------*/
				/*	オプションパラメータ領域を超えた場合											*/
				/*----------------------------------------------------------------------------------*/
					if( OpPrm->NextPnNo >= LcdOpt[dispID].PrmRange.UprLmt )
					{
						/* オプションパラメータ領域以上にパラメータが存在する時（システムレベル） 	*/
						if( PnPrmTbl[idx_tmp].PnNo > OPT_PNNO_VIRTUAL )
						{
							/* オプションパラメータ編集中フラグリセット */
							OpPrm->State.Editing = FALSE;
							/* サーボパラメータインデックス更新 */
							LopDsp.InfTbl[dispID][LopV.ActiveLine].PnIdx = 
								LcdOptGetNextSvPrmIdx( DEGOP_READ_UP, cur_idx_tmp, &LcdOpt[dispID] );
						}
						/* オプションパラメータ領域以上にパラメータが存在しない時（ユーザレベル） 	*/
						else
						{
							/* パラメータ読み出し要求セット */
							LcdOptSetOpPrmReadReq( DEGOP_READ_UP, OpPrm->NextPnNo, &LcdOpt[dispID] );
						}
					}
				/*----------------------------------------------------------------------------------*/
				/*	オプションパラメータ領域内にいる場合											*/
				/*----------------------------------------------------------------------------------*/
					else
					{
						/* パラメータ読み出し要求セット */
						LcdOptSetOpPrmReadReq( DEGOP_READ_UP, OpPrm->NextPnNo, &LcdOpt[dispID] );
					}
				}
			/*--------------------------------------------------------------------------------------*/
			/*	ユーザパラメータ領域の編集															*/
			/*--------------------------------------------------------------------------------------*/
				else
				{
					/* オプションパラメータ編集中フラグリセット */
					OpPrm->State.Editing = FALSE;
					/* サーボパラメータインデックス更新 */
					LopDsp.InfTbl[dispID][LopV.ActiveLine].PnIdx = idx_tmp;
				}
			}
/*--------------------------------------------------------------------------------------------------*/
/*			デジオペ通信処理中																		*/
/*--------------------------------------------------------------------------------------------------*/
			else if( rc == DEGOP_BUSY )
			{
				;
			}
/*--------------------------------------------------------------------------------------------------*/
/*			デジオペ通信無効時																		*/
/*--------------------------------------------------------------------------------------------------*/
			else
			{
				/* オプションパラメータ状態フラグリセット */
				MlibResetByteMemory((void *)&OpPrm->State, sizeof(OpPrm->State));

				/* 仮想オプションパラメータ（PnB00）を表示させないための処置 */
				if( PnPrmTbl[idx_tmp].PnNo == OPT_PNNO_VIRTUAL )
				{
					idx_tmp = RpxPrmSearchIdxOp((OPT_PNNO_VIRTUAL+1), cur_idx_tmp, &LopMsg);
					if( PnPrmTbl[idx_tmp].PnNo > OPT_PNNO_VIRTUAL )
					{
						LopDsp.InfTbl[dispID][LopV.ActiveLine].PnIdx = idx_tmp;
					}
					else
					{
						LopDsp.InfTbl[dispID][LopV.ActiveLine].PnIdx = 
							RpxPrmSearchIdxOp((OPT_PNNO_VIRTUAL-1), idx_tmp, &LopMsg);
					}
				}
				else
				{
					LopDsp.InfTbl[dispID][LopV.ActiveLine].PnIdx = idx_tmp;
				}
			}
#else		//CCSW_CMDID != CMDID_EXIST 
				Numx = CPARAMX->GetNumber( CPARAMX, PnIdx, &LopMsg ) + (0x01 << (LopV.CurDigit*4));
				if( Numx >= 0x1000 ){ Numx = 0xFFF;}
				LopDsp.InfTbl[dispID][LopV.ActiveLine].PnIdx = CPARAMX->SearchB( (COBJECT *)CPARAMX, Numx, PnIdx, &LopMsg );
#endif
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		else if( pKeyMk->down )
		{
			if( PnUn == UnSelected )
			{
				if( CUNMONX->GetNumber( CUNMONX, UnIdx, &LopMsg ) < (0x01 << (LopV.CurDigit*4)))
				{
					Numx = 0x000;
				}
				else
				{
					Numx = CUNMONX->GetNumber( CUNMONX, UnIdx, &LopMsg ) - (0x01 << (LopV.CurDigit*4));
				}
				LopDsp.InfTbl[dispID][LopV.ActiveLine].UnIdx = 
						CUNMONX->SearchB( (COBJECT *)CUNMONX, Numx, UnIdx, &LopMsg );
			}
			else if( PnUn == PnSelected )
			{
/*<11J42>*/
#if CCSW_CMDID == CMDID_EXIST
/*--------------------------------------------------------------------------------------------------*/
/*			初期情報取得																			*/
/*--------------------------------------------------------------------------------------------------*/
			/* 現在サーボパラメータインデックス */
			cur_idx_tmp = LopDsp.InfTbl[dispID][LopV.ActiveLine].PnIdx;
			/* 現在サーボパラメータ番号 */
			cur_num_tmp = PnPrmTbl[cur_idx_tmp].PnNo;
			/* 次回サーボパラメータ番号 */
			if(cur_num_tmp < (0x01 << (LopV.CurDigit*4)))
				Numx = 0x0000;
			else
				Numx = cur_num_tmp - (0x01 << (LopV.CurDigit*4));
			/* 次回サーボパラメータインデックス */
			idx_tmp = RpxPrmSearchIdxOp(Numx, cur_idx_tmp, &LopMsg);
			OpPrm->NextSvIdx = idx_tmp;

/*--------------------------------------------------------------------------------------------------*/
/*			デジオペ通信状態取得																	*/
/*--------------------------------------------------------------------------------------------------*/
			rc = OpiGetDegOpState(&LcdOpt[dispID].PrmRange);

/*--------------------------------------------------------------------------------------------------*/
/*			現在編集中のパラメータ領域設定															*/
/*--------------------------------------------------------------------------------------------------*/
			/* ユーザパラメータ領域 */
			if( cur_num_tmp < OPT_PNNO_VIRTUAL )
			{
				OpPrm->State.ExceedLwrLmt = TRUE;					/* 下限値オーバフラグセット		*/
				OpPrm->CurrPnNo = LcdOpt[dispID].PrmRange.LwrLmt;	/* 現在オプションPrm番号更新	*/
			}
			/* システムパラメータ領域 */
			else if( cur_num_tmp > OPT_PNNO_VIRTUAL )
			{
				OpPrm->State.ExceedUprLmt = TRUE;					/* 上限値オーバフラグセット		*/
				OpPrm->CurrPnNo = LcdOpt[dispID].PrmRange.UprLmt;	/* 現在オプションPrm番号更新	*/
			}

/*--------------------------------------------------------------------------------------------------*/
/*			デジオペ通信有効時																		*/
/*--------------------------------------------------------------------------------------------------*/
			if( rc == DEGOP_ENABLE )
			{
			/*--------------------------------------------------------------------------------------*/
			/*	ユーザ/オプションパラメータ領域の編集												*/
			/*--------------------------------------------------------------------------------------*/
				if( PnPrmTbl[idx_tmp].PnNo <= OPT_PNNO_VIRTUAL )
				{
					/* 次回オプションパラメータ番号を取得 */
					OpPrm->NextPnNo = LcdOptGetNextOpPrmNo( DEGOP_READ_DOWN, cur_idx_tmp, LopV.CurDigit, OpPrm );

				/*----------------------------------------------------------------------------------*/
				/*	オプションパラメータ領域を抜けた場合											*/
				/*----------------------------------------------------------------------------------*/
					if( OpPrm->NextPnNo < LcdOpt[dispID].PrmRange.LwrLmt )
					{
						/* オプションパラメータ編集中フラグリセット */
						OpPrm->State.Editing = FALSE;
						/* サーボパラメータインデックス更新 */
						LopDsp.InfTbl[dispID][LopV.ActiveLine].PnIdx = 
							LcdOptGetNextSvPrmIdx( DEGOP_READ_DOWN, cur_idx_tmp, &LcdOpt[dispID] );
					}
				/*----------------------------------------------------------------------------------*/
				/*	オプションパラメータ領域内にいる場合											*/
				/*----------------------------------------------------------------------------------*/
					else
					{
						/* パラメータ読み出し要求セット */
						LcdOptSetOpPrmReadReq( DEGOP_READ_DOWN, OpPrm->NextPnNo, &LcdOpt[dispID] );
					}
				}
			/*--------------------------------------------------------------------------------------*/
			/*	システムパラメータ領域の編集														*/
			/*--------------------------------------------------------------------------------------*/
				else
				{
					/* オプションパラメータ編集中フラグリセット */
					OpPrm->State.Editing = FALSE;
					/* サーボパラメータインデックス更新 */
					LopDsp.InfTbl[dispID][LopV.ActiveLine].PnIdx = idx_tmp;
				}
			}
/*--------------------------------------------------------------------------------------------------*/
/*			デジオペ通信処理中																		*/
/*--------------------------------------------------------------------------------------------------*/
			else if( rc == DEGOP_BUSY )
			{
				;
			}
/*--------------------------------------------------------------------------------------------------*/
/*			デジオペ通信無効時																		*/
/*--------------------------------------------------------------------------------------------------*/
			else
			{
				/* オプションパラメータ状態フラグリセット */
				MlibResetByteMemory((void *)&OpPrm->State, sizeof(OpPrm->State));

				/* 仮想オプションパラメータ（PnB00）を表示させないための処置 */
				if( PnPrmTbl[idx_tmp].PnNo == OPT_PNNO_VIRTUAL )
				{
					LopDsp.InfTbl[dispID][LopV.ActiveLine].PnIdx = 
						RpxPrmSearchIdxOp((OPT_PNNO_VIRTUAL-1), cur_idx_tmp, &LopMsg);
				}
				else
				{
					LopDsp.InfTbl[dispID][LopV.ActiveLine].PnIdx = idx_tmp;
				}
			}
#else		//CCSW_CMDID != CMDID_EXIST
				if( CPARAMX->GetNumber( CPARAMX, PnIdx, &LopMsg ) < (0x01 << (LopV.CurDigit*4)) )
				{
					Numx = 0x000;
				}
				else
				{
					Numx = CPARAMX->GetNumber( CPARAMX, PnIdx, &LopMsg ) - (0x01 << (LopV.CurDigit*4));
				}
				LopDsp.InfTbl[dispID][LopV.ActiveLine].PnIdx = 
						CPARAMX->SearchB( (COBJECT *)CPARAMX, Numx, PnIdx, &LopMsg );
#endif
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		else if( pKeyMk->shup )
		{
			if(      LopV.CurDigit == (3-1) ){ LopV.EditMode = LCDEM_PnUnSel;}
			else if( LopV.CurDigit <  (3-1) ){ LopV.CurDigit++;}
		}
/*--------------------------------------------------------------------------------------------------*/
		else if( pKeyMk->shdown )
		{
			if(      LopV.CurDigit == 0 ){ LopV.EditMode = LCDEM_PnUnSel;}
			else if( 0 < LopV.CurDigit  ){ LopV.CurDigit--;}
		}
/*--------------------------------------------------------------------------------------------------*/
		else if( pKeyMk->data )
		{
			if( PnUn == UnSelected )
			{
				LopV.CurDigit = 0;
			}
			else if( PnUn == PnSelected )
			{
				if( CHK_WRITEINHIBIT() && (LopMsg.AccessLvl != ACCLVL_SYSTEM) )
				{
					OpBlnkKickFlg.no_op = 1;
				}
				else
				{
					LopV.EditMode = LCDEM_PnData;
					LopV.CurDigit = 0;
/*<11J42>*/
#if CCSW_CMDID == CMDID_EXIST		
					if( OpPrm->State.Editing == TRUE )
					{							/* オプションパラメータ編集中 */
						if(OpPrm->IsLong)/* ロングパラメータ	*/
						{
							LopV.DataDigit = (OpPrm->Attr.Base == DSPDEF_BASEDEC)? 10: 8;
						}
						else						/* ワードパラメータ	*/
						{
							LopV.DataDigit = (OpPrm->Attr.Base == DSPDEF_BASEDEC)? 5: 4;
						}
					}
					else
					{
#endif
						pPrmDef = GET_PNPRMDEF( CPARAMX->pPrmTbl, PnIdx );		/* パラメータ定義	*/
						if( CPARAMX->isLong( CPARAMX, PnIdx, &LopMsg ) )		/* ロングパラメータ	*/
						{
							UINT dot = ((CHK_PNPRM_SIGNED( pPrmDef ) 			/* <S18B> */
										&& (GET_PNPRM_DECIMALS( CPARAMX, pPrmDef, &LopMsg )>0)) ? 1 : 0);
							LopV.DataDigit = ( GET_PNPRMBASE( pPrmDef ) == DSPDEF_BASEDEC)? (10-dot): 8;	/* <S18B> */
						}
						else													/* ワードパラメータ	*/
						{
							LopV.DataDigit = ( GET_PNPRMBASE( pPrmDef ) == DSPDEF_BASEDEC)? 5: 4;
						}
#if CCSW_CMDID == CMDID_EXIST		/*<11J42>*/
					}
#endif
				}
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		else if( pKeyMk->scroll )
		{
			LopV.ActiveLine = (LopV.ActiveLine == 0)? 3 : (--LopV.ActiveLine);
			LopV.CurDigit = 0;
		}
/*--------------------------------------------------------------------------------------------------*/
		else if( pKeyMk->write )
		{
			if( LcdSaveDispInfoTable( dispID ) == OK )
			{
				pBlnkKickFlg->write = TRUE;
			}
			else
			{
				LopV.EewriteMode = OPWM_InfWrWait;
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		else if( pKeyMk->rst )
		{
		/*------------------------------------------------------------------------------------------*/
		/*		画面内にあるリセット可能モニタは、リセット実施										*/
		/*------------------------------------------------------------------------------------------*/
			for( i = 0; i < 4; i++ )
			{
				if ( LopDsp.InfTbl[dispID][i].PnUn == UnSelected )
				{
					CUNMONX->Clear( (COBJECT *)CUNMONX, LopDsp.InfTbl[dispID][i].UnIdx, &LopMsg );
				}
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		return( FALSE );
}

/****************************************************************************************************/
/*																									*/
/*		Pn/Un Select mode																			*/
/*																									*/
/****************************************************************************************************/
/*		dispID: ディスプレーID(LCDPMDSP_PM=PRM/MON画面, LCDPMDSP_JOG=JOG画面 etc.), 				*/
/*		pKeyMk: キー押下情報のアドレス, pBlnkKickFlg: ブリンク起動フラグポインター					*/
/*		戻り値: TRUE=モードキー押下が検知されたことを示す											*/
/****************************************************************************************************/
USHORT	LcdPnUnSel( USHORT dispID, OPKEYFLG *pKeyMk, BLINKFLG *pBlnkKickFlg )
{
CPRMDEF		*pPrmDef; 														/* PRMDEF	*pPrmDef; */
USHORT	PnUn  = LopDsp.InfTbl[dispID][LopV.ActiveLine].PnUn;
USHORT	PnIdx = LopDsp.InfTbl[dispID][LopV.ActiveLine].PnIdx;
#if CCSW_CMDID == CMDID_EXIST		/*<11J42>*/
	DEGOP_PRM	*OpPrm = &LcdOpt[dispID].Prm[LopV.ActiveLine];
#endif
AXIS_HANDLE *pAxRsc = LopMsg.pAxRsc;										/* TODO: 後で消す */
/*--------------------------------------------------------------------------------------------------*/
/*		Pn/Un Select																				*/
/*--------------------------------------------------------------------------------------------------*/
		if( pKeyMk->mode )
		{
			return( TRUE );
		}
/*--------------------------------------------------------------------------------------------------*/
		else if( (pKeyMk->up) || (pKeyMk->down) )
		{
			if( PnUn == UnSelected )
			{
				LopDsp.InfTbl[dispID][LopV.ActiveLine].PnUn = PnSelected;
			}
			else if( PnUn == PnSelected )
			{
				LopDsp.InfTbl[dispID][LopV.ActiveLine].PnUn = UnSelected;
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		else if( pKeyMk->shup )
		{
			LopV.EditMode = LCDEM_PnUnNum;
			LopV.CurDigit = 0;
		}
/*--------------------------------------------------------------------------------------------------*/
		else if( pKeyMk->shdown )
		{
			LopV.EditMode = LCDEM_PnUnNum;
			LopV.CurDigit = 3-1;
		}
/*--------------------------------------------------------------------------------------------------*/
		else if( pKeyMk->data )
		{
			if( PnUn == UnSelected )
			{
				LopV.EditMode = LCDEM_PnUnNum;
				LopV.CurDigit = 0;
			}
			else if( PnUn == PnSelected )
			{
				if( CHK_WRITEINHIBIT() /* Iprm.f.PrmWrtInhibit */ && (LopMsg.AccessLvl != ACCLVL_SYSTEM) )
				{
					OpBlnkKickFlg.no_op = 1;
				}
				else
				{
					LopV.EditMode = LCDEM_PnData;
					LopV.CurDigit = 0;
#if CCSW_CMDID == CMDID_EXIST		/*<11J42>*/
					if( OpPrm->State.Editing == TRUE )
					{							/* オプションパラメータ編集中 */
						if(OpPrm->IsLong)/* ロングパラメータ	*/
						{
							LopV.DataDigit = (OpPrm->Attr.Base == DSPDEF_BASEDEC)? 10: 8;
						}
						else						/* ワードパラメータ	*/
						{
							LopV.DataDigit = (OpPrm->Attr.Base == DSPDEF_BASEDEC)? 5: 4;
						}
					}
					else
					{
#endif
						pPrmDef = GET_PNPRMDEF( CPARAMX->pPrmTbl, PnIdx ) /* (PRMDEF *)PnPrmTbl[PnIdx].PrmDefPtr */; /* パラメータ定義	*/
						if( CPARAMX->isLong( CPARAMX, PnIdx, &LopMsg ) )	/* ロングパラメータ	*/
						{
							UINT dot = ((CHK_PNPRM_SIGNED( pPrmDef ) 			/* <S18B> */
										&& (GET_PNPRM_DECIMALS( CPARAMX, pPrmDef, &LopMsg )>0)) ? 1 : 0);
							LopV.DataDigit = ( GET_PNPRMBASE( pPrmDef ) == DSPDEF_BASEDEC)? (10-dot): 8;	/* <S18B> */
						}
						else								/* ワードパラメータ	*/
						{
							LopV.DataDigit = ( GET_PNPRMBASE( pPrmDef ) == DSPDEF_BASEDEC)? 5: 4;
						}
#if CCSW_CMDID == CMDID_EXIST		/*<11J42>*/
					}
#endif
				}
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		else if( pKeyMk->scroll )
		{
			LopV.EditMode = LCDEM_PnUnNum;
			LopV.ActiveLine = (LopV.ActiveLine == 0)? 3 : (--LopV.ActiveLine);
			LopV.CurDigit = 0;
		}
/*--------------------------------------------------------------------------------------------------*/
		else if( pKeyMk->write )
		{
			if( LcdSaveDispInfoTable(dispID) == OK )
			{
				pBlnkKickFlg->write = TRUE;
			}
			else
			{
				LopV.EewriteMode = OPWM_InfWrWait;
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		return(FALSE);
}


/****************************************************************************************************/
/*																									*/
/*		Pn Data Edit mode																			*/
/*																									*/
/****************************************************************************************************/
/*		dispID: ディスプレーID(LCDPMDSP_PM=PRM/MON画面, LCDPMDSP_JOG=JOG画面 etc.)					*/
/*		pKeyMk: キー押下情報のアドレス, pBlnkKickFlg: ブリンク起動フラグポインター					*/
/*		戻り値: TRUE=モードキー押下が検知されたことを示す											*/
/****************************************************************************************************/
USHORT	LcdPnDataEdit( USHORT dispID, OPKEYFLG *pKeyMk, BLINKFLG *pBlnkKickFlg )
{
LONG	rc;
ULONG	PnData;
USHORT	PnIdx = LopDsp.InfTbl[dispID][LopV.ActiveLine].PnIdx;
#if CCSW_CMDID == CMDID_EXIST		/*<11J42>*/
	DEGOP_PRM	*OpPrm = &LcdOpt[dispID].Prm[LopV.ActiveLine];
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		Pn Data Edit																				*/
/*--------------------------------------------------------------------------------------------------*/
		if(pKeyMk->mode)
		{
			return(TRUE);
		}
/*--------------------------------------------------------------------------------------------------*/
		else if(pKeyMk->up)
		{
#if CCSW_CMDID == CMDID_EXIST		/*<11J42>*/
		/* オプションパラメータ編集中 */
			if( OpPrm->State.Editing == TRUE )
			{
				/* データ編集（上方向） */
				PnData = RpiPrmDataDigitInc( OpPrm->Data, LopV.CurDigit,
												LcdOptPrmLmtTbl[OpPrm->Attr.Base][(OpPrm->IsLong<<1)|(OpPrm->Attr.Sign)].UprLmt,
												OpPrm->Attr.Base,
												OpPrm->IsLong,
												OpPrm->Attr.Sign );
				/* 一時オプションパラメータ書き込み要求セット */
				rc = OpiReqDegOpPrmWrite( (OpPrm->CurrPnNo|0x1000), OpPrm->Size, PnData );

				if( rc == DEGOP_SUCCESS )
				{
					/* RAM書き込み要求フラグセット */
					LcdOpt[dispID].f.PrmRamWrite = TRUE;
					/* ブリンクキックフラグへのポインタ保存 */
					LcdOptBlnkKick = pBlnkKickFlg;
					/* 応答データ待ちセット */
					OpPrm->State.RcvWait = TRUE;
					/* タイムアウト用タイマリセット */
					KpiRstLongTimer( &LcdOpt[dispID].ComTimeLong );
				}
				else
				{
					OpBlnkKickFlg.no_op = 1;
				}
			}
			else
			{
#endif
				rc = CPARAMX->DigitInc( CPARAMX, PnIdx, LopV.CurDigit, &LopMsg );
				if( rc == PERR_CALC ){ OpBlnkKickFlg.error = 1;}
#if CCSW_CMDID == CMDID_EXIST		/*<11J42>*/
			}
#endif
		}
/*--------------------------------------------------------------------------------------------------*/
		else if(pKeyMk->down)
		{	/* データ編集（下方向） */
#if CCSW_CMDID == CMDID_EXIST
			if( OpPrm->State.Editing == TRUE )
			{	/* オプションパラメータ編集中 */
				PnData = RpiPrmDataDigitDec( OpPrm->Data, LopV.CurDigit,
												LcdOptPrmLmtTbl[OpPrm->Attr.Base][(OpPrm->IsLong<<1)|(OpPrm->Attr.Sign)].LwrLmt,
												OpPrm->Attr.Base,
												OpPrm->IsLong,
												OpPrm->Attr.Sign );
				/* 一時オプションパラメータ書き込み要求セット */
				rc = OpiReqDegOpPrmWrite( (OpPrm->CurrPnNo|0x1000), OpPrm->Size, PnData );

				if( rc == DEGOP_SUCCESS )
				{
					/* RAM書き込み要求フラグセット */
					LcdOpt[dispID].f.PrmRamWrite = TRUE;
					/* ブリンクキックフラグへのポインタ保存 */
					LcdOptBlnkKick = pBlnkKickFlg;
					/* 応答データ待ちセット */
					OpPrm->State.RcvWait = TRUE;
					/* タイムアウト用タイマリセット */
					KpiRstLongTimer( &LcdOpt[dispID].ComTimeLong );
				}
				else
				{
					OpBlnkKickFlg.no_op = 1;
				}
			}
			else
			{
#endif
				rc = CPARAMX->DigitDec( CPARAMX, PnIdx, LopV.CurDigit, &LopMsg );
				if( rc == PERR_CALC ){ OpBlnkKickFlg.error = 1;}
#if CCSW_CMDID == CMDID_EXIST		/*<11J42>*/
			}
#endif
		}
/*--------------------------------------------------------------------------------------------------*/
		else if(pKeyMk->shup)
		{
			LopV.CurDigit = (LopV.CurDigit == (LopV.DataDigit-1))? 0 : (++LopV.CurDigit);
		}
/*--------------------------------------------------------------------------------------------------*/
		else if(pKeyMk->shdown)
		{
			LopV.CurDigit = (LopV.CurDigit == 0)? (LopV.DataDigit-1) : (--LopV.CurDigit);
		}
/*--------------------------------------------------------------------------------------------------*/
		else if(pKeyMk->data)
		{
#if CCSW_CMDID == CMDID_EXIST		/*<11J42>*/
			if( OpPrm->State.Editing == TRUE )
			{			/* オプションパラメータ編集中 */
				/* オプションパラメータ書き込み要求セット */
				rc = OpiReqDegOpPrmWrite( OpPrm->CurrPnNo, OpPrm->Size, OpPrm->Data );

				if( rc == DEGOP_SUCCESS )
				{
					/* EEPROM書き込み要求フラグセット */
					LcdOpt[dispID].f.PrmEepWrite = TRUE;
					/* ブリンクキックフラグへのポインタ保存 */
					LcdOptBlnkKick = pBlnkKickFlg;
					/* 応答データ待ちセット */
					OpPrm->State.RcvWait = TRUE;
					/* タイムアウト用タイマリセット */
					KpiRstLongTimer( &LcdOpt[dispID].ComTimeLong );
				}
				else
				{
					OpBlnkKickFlg.no_op = 1;
					LopV.EditMode = LCDEM_PnUnNum;
					LopV.CurDigit = 0;
				}
			}
			else
			{
#endif

				rc = PrmWriteValueToEeprom( LopMsg.pAxRsc->RegManager->hPnReg,						  /* <S099> */
						PnPrmTbl[PnIdx].Attribute,
						0 );
				switch( rc )																		  /* <S098> */
				{
				case PRM_RSLT_SUCCESS:			/* 正常												*/
					pBlnkKickFlg->PrmEewrite = 1;	/* ブリンク起動	*/
					LopV.EewriteMode = OPWM_UpdtBlnk;
					break;
				/* 現状Queueがないため、以下コメントアウト Y.Oka */
//				case PRM_EEPROM_WRITE_FAIL:		/* まだQueueが空いていない							*/
//					LopV.EewriteMode = OPWM_UpdtWait;
//					break;
				case PRM_RSLT_RAMONLY:			/* RAMのみのパラメータ								*/
				case PRM_RSLT_NO_DIFF:			/* 書き込む必要がない(EEPROM値が書き込み値と同じ)	*/
					LopV.EditMode = LCDEM_PnUnNum;
					LopV.CurDigit = 0;
					break;
				default:
					break;
				}

#if CCSW_CMDID == CMDID_EXIST		/*<11J42>*/
			}
#endif
		}
/*--------------------------------------------------------------------------------------------------*/
		else if(pKeyMk->scroll)
		{
			/* 何もしない	*/
		}
/*--------------------------------------------------------------------------------------------------*/
		else if(pKeyMk->write)
		{
			if(LcdSaveDispInfoTable(dispID) == OK)
			{
				pBlnkKickFlg->write = TRUE;
			}
			else
			{
				LopV.EewriteMode = OPWM_InfWrWait;
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		return(FALSE);
}


/****************************************************************************************************/
/*																									*/
/*		Make Display Data for Pn/Un Lines															*/
/*																									*/
/****************************************************************************************************/
void	LcdMkDspDataPnUnLines(				/* Make Display Data for Pn/Un Lines					*/
		USHORT	dispID,						/* ディスプレーID(LCDPMDSP_PM/JOG/etc)					*/
		USHORT	BlnkSupFlg,					/* ブリンク抑止フラグ									*/
		UCHAR	*ResBuf			)			/* 応答バッファアドレス									*/
{
USHORT	i, Idx;
ULONG	Ldata;
CPRMDEF	*pPrmDef;
CMONDEF	*pMonDef; 							/* MONDEF	*pMonDef; */
LOPDIT	*pInf = (LOPDIT *)(&(LopDsp.InfTbl[dispID][0]));
#if CCSW_CMDID == CMDID_EXIST		/*<11J42>*/
USHORT j;
DEGOP_PRM *optprm;
DEGOP_PRM *act_optprm = &LcdOpt[dispID].Prm[LopV.ActiveLine];

		if( act_optprm->State.RcvWait == TRUE )
		{			/* オプションからの応答データ待ちの時は受信処理 */
			LcdOptUpdateResponseData(dispID, &LcdOpt[dispID]);
		}
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		Make Display Data for Pn/Un Lines															*/
/*--------------------------------------------------------------------------------------------------*/
		for(i = 0; i < 4; i++)
		{
			if(pInf[i].PnUn == UnSelected)
			{
				Idx = pInf[i].UnIdx;
				pMonDef = GET_UNMONDEF( CUNMONX->pUnMonTbl, Idx );
				RpxLcdSetStr((USHORT)(i+1), 0, (UCHAR *)"Un", ResBuf);
				RpxLcdSetValHex((USHORT)(i+1), 2, (ULONG) CUNMONX->GetNumber( CUNMONX, Idx, &LopMsg ), 3, ResBuf);
				RpxLcdSetStr((USHORT)(i+1), 5, (UCHAR *)"=", ResBuf);
				CUNMONX->Read( (COBJECT *)CUNMONX, Idx, &Ldata, MCHK_ACCS_LVL, &LopMsg );   /* Ldata <-- モニタ値	*/ /* <S073> */
				RpxLcdSetPnUnData((USHORT)(i+1), 
					6, 
					Ldata, 
					CUNMONX->isLong( CUNMONX, Idx, &LopMsg),	
					CHK_UNBASE( pMonDef ),
					pMonDef->Sign, 
					CHK_UNDECIMALS( pMonDef ),
					ResBuf);
			}
			else if(pInf[i].PnUn == PnSelected)
			{
#if CCSW_CMDID == CMDID_EXIST
				optprm = &LcdOpt[dispID].Prm[i];
				/* オプションパラメータ編集中の時 */
				if( optprm->State.Editing == TRUE )
				{
					/* 属性チェック＆セット */
					optprm->IsLong = (optprm->Size == 4)? TRUE : FALSE;
					if( optprm->Attr.Base == DSPDEF_BASEBIT )	/* 現状、ビット表示のサポートなし */
					{ optprm->Attr.Base = DSPDEF_BASEHEX; }		/* 強制的に16進表示 */
					optprm->Attr.Decimals = 0;					/* 現状、小数点表示のサポートなし */

					/* 各行にて編集中のPn番号がアクティブ行のそれと同じ場合はアクティブ行のPn番号の値に統一 */
					if( i != LopV.ActiveLine )
					{
						if( optprm->CurrPnNo == act_optprm->CurrPnNo )
						{
							optprm->Data = act_optprm->Data;
						}
					}

					/* 各画面にて編集中のPn番号がアクティブ行のそれと同じ場合はアクティブ行のPn番号の値に統一 */
					for(j = 0; j < LCDPMDSP_MAX; j++)
					{
						if( j != dispID )
						{
							if( LcdOpt[j].Prm[i].CurrPnNo == act_optprm->CurrPnNo )
							{
								LcdOpt[j].Prm[i].Data = act_optprm->Data;
							}
						}
					}

					/* 応答データ表示 */
					RpxLcdSetStr((USHORT)(i+1), 0, (UCHAR *)"Pn", ResBuf);
					RpxLcdSetValHex((USHORT)(i+1), 2, (ULONG)optprm->CurrPnNo, 3, ResBuf);
					RpxLcdSetStr((USHORT)(i+1), 5, (UCHAR *)"=", ResBuf);
					RpxLcdSetPnUnData( (USHORT)(i+1), 6, optprm->Data,
										(USHORT)optprm->IsLong, (USHORT)optprm->Attr.Base,
										(USHORT)optprm->Attr.Sign, optprm->Attr.Decimals, ResBuf);
				}
				else
				{
#endif
					Idx = pInf[i].PnIdx;
					pPrmDef = GET_PNPRMDEF( CPARAMX->pPrmTbl, Idx );
					RpxLcdSetStr((USHORT)(i+1), 0, (UCHAR *)"Pn", ResBuf);
					RpxLcdSetValHex((USHORT)(i+1), 2, CPARAMX->GetNumber( CPARAMX, Idx, &LopMsg ), 3, ResBuf);
					RpxLcdSetStr((USHORT)(i+1), 5, (UCHAR *)"=", ResBuf);
					CPARAMX->Read( (COBJECT *)CPARAMX, Idx, &Ldata, PCHK_OPE_READ, &LopMsg );	/* Ldata <-- パラメータ値	*/  /* <S073> */
					RpxLcdSetPnUnData( 
						(USHORT)(i+1), 
						6, 
						Ldata,
						CPARAMX->isLong( CPARAMX, Idx, &LopMsg ),
						GET_PNPRMBASE( pPrmDef ),
						CHK_PNPRM_SIGNED( pPrmDef ), 
						/*GET_PNPRM_DECIMALS( pPrmDef ), */
						GET_PNPRM_DECIMALS( CPARAMX, pPrmDef, &LopMsg ),	/* <S14F> */ 
						ResBuf);
				}
#if CCSW_CMDID == CMDID_EXIST		/*<11J42>*/
			}
#endif
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Set Blink in the active line																*/
/*--------------------------------------------------------------------------------------------------*/
		if(!BlnkSupFlg)
		{
			switch(LopV.EditMode)
			{
			case LCDEM_PnUnNum:
				RpxLcdSetBlnk((USHORT)(LopV.ActiveLine+1), (USHORT)(2+3-1 - LopV.CurDigit), 1, ResBuf);
				break;
			case LCDEM_PnUnSel:
				RpxLcdSetBlnk((USHORT)(LopV.ActiveLine+1), 0, 2, ResBuf);
				break;
			case LCDEM_PnData:
#if CCSW_CMDID == CMDID_EXIST		/*<11J42>*/
				if( act_optprm->State.Editing == TRUE )
				{	/* オプションパラメータ編集中の時 */
					RpxLcdSetBlnk((USHORT)(LopV.ActiveLine+1),
								  (USHORT)(2+3+1 + LcdOptSetDataBlinkDPos(act_optprm)), 1, ResBuf);
				}
				else
				{
#endif
					Idx = pInf[LopV.ActiveLine].PnIdx;
					pPrmDef =GET_PNPRMDEF( CPARAMX->pPrmTbl, Idx ); /* (PRMDEF *)PnPrmTbl[Idx].PrmDefPtr; */
					RpxLcdSetBlnk( (USHORT)(LopV.ActiveLine+1),
							   (USHORT)(2+3+1 + LcdPrmDataBlinkDpos(pPrmDef)), 1, ResBuf );
#if CCSW_CMDID == CMDID_EXIST		/*<11J42>*/
				}
#endif
				break;
			default:
				break;
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}

/****************************************************************************************************/
/*																									*/
/*		Reset Pn/UN Display Data 																	*/
/*																									*/
/****************************************************************************************************/
void	LcdRstPnUnDspData( void )
{
		LopV.ActiveLine = 0;
		LopV.EditMode = LCDEM_PnUnNum;
		LopV.EewriteMode = 0 /* OPWM_None */;
		LopV.CurDigit = 0;
		return;
}

/****************************************************************************************************/
/*																									*/
/*		Lcd Operator Parameter Blink Data position 													*/
/*																									*/
/****************************************************************************************************/
USHORT	LcdPrmDataBlinkDpos( CPRMDEF *pPrmDef )
{
USHORT	Dpos;
USHORT	Decimals;

/*--------------------------------------------------------------------------------------------------*/
/*		Set Dpos, Decimals																			*/
/*--------------------------------------------------------------------------------------------------*/
		Dpos = LopV.DataDigit - 1 - LopV.CurDigit;			/* "H."および小数点を未考慮の位置		*/
		/*Decimals = GET_PNPRM_DECIMALS( pPrmDef );*/		/* 小数点位置の取得						*/
		Decimals = GET_PNPRM_DECIMALS( CPARAMX, pPrmDef, &LopMsg );		 /* <S14F> 小数点位置の取得	*/
/*--------------------------------------------------------------------------------------------------*/
/*		Modify Dpos by Sign,Decimals,etc															*/
/*--------------------------------------------------------------------------------------------------*/
		if( GET_PNPRMBASE( pPrmDef ) == DSPDEF_BASEDEC )	/* 底が10進	*/
		{
			if( CHK_PNPRM_SIGNED( pPrmDef )  == DSPDEF_W_SIGN )
			{
				Dpos++;	/* 符号分	*/
			}
			if( (Decimals > 0) && (LopV.CurDigit < (USHORT)Decimals) )
			{
				Dpos++;	/* 小数点分	*/
			}
		}
		else												/* 底が10進以外	*/
		{
			Dpos += 2;	/* "n." or "H." の分	*/
		}
/*--------------------------------------------------------------------------------------------------*/
		return( Dpos );
}

