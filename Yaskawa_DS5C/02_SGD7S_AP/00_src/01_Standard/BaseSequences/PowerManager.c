/****************************************************************************************************/
/*																									*/
/*																									*/
/*		PowerManager.c : サーボ電源管理処理定義														*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : サーボドライブの電源の管理を行う。														*/
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
/*	Note	:	初版	2010.04.19	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"
#include "BaseCmnStruct.h"
#include "PowerManager.h"
#include "AlarmManager.h"
#include "MLib.h"
#include "KLib.h"
#include "HwDeviceIf.h"

/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
/*	void	CpxPowerManager( POW_MNG_HNDL *hPowerManager );											*/
/*	void	LpxRegeneControl( REGENE_CTRL *pRegCtrl, POWER_STATUS *PowSts, BOOL MotStop );			*/
/*	LONG	LpxDetectDcVolt( DET_DC_VOLT *pDetectDc, POWER_STATUS *PowSts, CHAR AdAdjust );			*/
/*	LONG	LpxCheckOverVoltage( CHECK_MAIN_POWER *PowerChecker, DET_DC_VOLT *pDetectDc,			*/
/*																		POWER_STATUS *PowSts );		*/
/*	LONG	LpxCheckRegeneOverLoad( REGENE_CTRL *pRegCtrl, ULONG *AlmOLInfo,						*/
/*																	ANLG_UN_MON_DATA *MonData );	*/
/*	void	SetRegeneOverLoadSts( LONG RegOlSts );													*/
/*	BOOL	LpxCheckRushRxOverLoad( CHECK_RUSH *pRush, POWER_STATUS *pPowerSts );					*/
/*	BOOL	LpxCheckRegeneAlarm( REGENE_CTRL *pRegCtrl, BOOL PowerOn,								*/
/*											ASIC_HW_IF_STR *pAsicHwReg, UNI_PRM_STR *uni_prm_ptr );	*/
/*	void	PcmInitPowerManager( POW_MNG_HNDL *hPowerManager, ULONG AlmOLInfo );					*/
/*	void	PcmCountRegeneOnTime( POW_MNG_HNDL *hPowerManager );									*/
/*	void	PcmInputPowerStatus( POW_MNG_HNDL *hPowerManager, BOOL Acon1, BOOL RegeneAlm );			*/
/*	void	IprmcalRegenePowerGain( CHECK_REGOL *RegOL,												*/
/*													CMN_PRMDATA *cmnprm_ptr, BPRMDAT *bprm_ptr );	*/
/*	void	PcmCalculateVdetAdFil( DET_DC_VOLT *pDetectDc, USHORT vdetadfil, LONG SvCycleUs );		*/
/****************************************************************************************************/
static	void	LpxAcPowerManager( CHECK_MAIN_POWER *PowerChecker, POWER_STATUS *PowSts );
//static	void	LpxDcPowerManager( CHECK_MAIN_POWER *PowerChecker, POWER_STATUS *PowSts, BOOL MotStopSts );		/* <S21D> */
static	void	LpxDcPowerManager( CHECK_MAIN_POWER *PowerChecker, POWER_STATUS *PowSts, 							/* <S21D> */
												BOOL MotStop, REGENE_CTRL *RegeneCtrl );
//static	void	LpxDischargeDcPower( CHECK_MAIN_POWER *PowerChecker, POWER_STATUS *PowSts,
//																			REGENE_CTRL *RegCtrl );		/* <S0F6> */
static	void	LpxDischargeDcPower( CHECK_MAIN_POWER *PowerChecker, POWER_STATUS *PowSts,
																	REGENE_CTRL *RegCtrl, BOOL BeSts );	/* <S0F6> */
static	void	LpxCheckAcOpenPhase( CHECK_MAIN_POWER *PowerChecker, POWER_STATUS *PowSts );
#if( CSW_HW_DCBUS == FALSE )
static	void	LpxDetectAcOpenACON1( CHECK_MAIN_POWER *PowerChecker, POWER_STATUS *PowSts );
static	void	LpxDetectAcOpenACON12( CHECK_MAIN_POWER *PowerChecker, POWER_STATUS *PowSts );
#endif	/* ( CSW_HW_DCBUS == FALSE ) */
static	void	LpxCheckDcUnderVoltage( CHECK_MAIN_POWER *PowerChecker, POWER_STATUS *PowSts );
static	void	LpxAdDetectDcVolt( DET_DC_VOLT *pDetectDc );
static	void	LpxCheckPcovAlarm( PCOV_ERR_DETECT *PcovErrDetect, POWER_STATUS *PowSts );	/* <S053> */
static	BOOL	DischargeSequence( CHECK_MAIN_POWER *PowerChecker, LONG DcVolt ); /* <S1C6> */



/****************************************************************************************************/
/*																									*/
/*		サーボ電源管理メイン																		*/
/*																									*/
/****************************************************************************************************/
//void	PcmPowerManager( POW_MNG_HNDL *hPowerManager, BOOL MotStopSts )								  /* <S0F6> */
void	PcmPowerManager( POW_MNG_HNDL *hPowerManager, BOOL MotStopSts, BOOL BeSts )					  /* <S0F6> */
{
	/* ＡＣ電源監視処理 */
	LpxAcPowerManager( &(hPowerManager->MainPowerChecker),
					   &(hPowerManager->PowerSts) );

	/* ＤＣ電源監視処理 */
	LpxDcPowerManager( &(hPowerManager->MainPowerChecker),
					   &(hPowerManager->PowerSts),
					   MotStopSts,
					   &(hPowerManager->RegeneCtrl) );	/* <S21D> */

	/* ＤＣ電源急速放電処理 */
	LpxDischargeDcPower( &(hPowerManager->MainPowerChecker),
						 &(hPowerManager->PowerSts),
						 &(hPowerManager->RegeneCtrl),
						 BeSts );																	  /* <S0F6> */

	/* ＡＣ電源欠相検出処理 */
	LpxCheckAcOpenPhase( &(hPowerManager->MainPowerChecker),
						 &(hPowerManager->PowerSts) );

	/* ＤＣ電源不足電圧監視処理 */
	LpxCheckDcUnderVoltage( &(hPowerManager->MainPowerChecker),
							&(hPowerManager->PowerSts) );

	/* 中間電位異常チェック処理 <S053> */
	LpxCheckPcovAlarm( &(hPowerManager->PcovErrDetect),
					   &(hPowerManager->PowerSts) );

	return;
}



/****************************************************************************************************/
/*																									*/
/*		ＡＣ電源監視処理																			*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	目 的 :	(1) 安全規格からの要求で、ＡＣ入力がオフした場合、										*/
/*				ＤＣ電源を放電して一定電圧以下まで下げなければならない。							*/
/*			(2) サーボオン信号をアクティブに固定して、ＡＣ電源のオン／オフで						*/
/*				モータ通電オン／オフをコントロールするユーザーがいる。								*/
/*				ＡＣ電源検出を行わないと電源が切れてもモータ通電が継続し危険である。				*/
/*																									*/
/*																									*/
/*	機 能 : ＡＣ入力電源オン／オフの検出を行う。													*/
/*																									*/
/*  		acon検出は、ＡＣラインの電圧状態をフォトカプラにより監視する。							*/
/*			aconinはフォトカプラ出力をソフトフィルタ（125us２回読み多数決）したものである。			*/
/*			フォトカプラは単極性であるため、電源周期の半波しかaconinを検出できない。				*/
/*			そのため、aconinは電源オン状態で、電源周期でオン／オフを繰り返す。						*/
/*			aconinがオフしても少なくとも電源半周期分は電源オフ(acon1==FALSE)と見なさない。			*/
/*																									*/
/*			また、瞬停保持と言う機能があり、ユーザー定数で設定した時間は電源がオフしても			*/
/*			制御を続ける仕様である。																*/
/*			これにより、電源オフ検出は、aconinがオフしてから電源半周期＋瞬停保持時間分待つ。		*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	void	LpxAcPowerManager( CHECK_MAIN_POWER *PowerChecker, POWER_STATUS *PowSts )
{
#if( CSW_HW_DCBUS == TRUE )
/* Mask Function */
		return;
#endif
/*<S201> Start */
/*--------------------------------------------------------------------------------------------------*/
/* ＡＣ電源ＯＮ検出の不感帯中のフラグ作成      */ 
/* Initial Acon Wait Timer< KPI_SCANC_MS( 4 )*/
/*--------------------------------------------------------------------------------------------------*/
 if(PowerChecker->var.AconWaitTimer < KPI_SCANC_MS( 4 ))
 {
   	PowSts->AconDetectReady=FALSE;
 }
 else
 {
   	PowSts->AconDetectReady=TRUE;
 }
/*<S201> End*/
/*--------------------------------------------------------------------------------------------------*/
/*		電源投入時初回のみ4ms待ち ACON信号対策														*/
/*--------------------------------------------------------------------------------------------------*/
	if( PowerChecker->var.AconWaitTimer < KPI_SCANC_MS( 4 ) )
	{
		PowerChecker->var.AconWaitTimer++;
		PowSts->Acon = FALSE;
	}
/*--------------------------------------------------------------------------------------------------*/
/*		ＡＣ電源ＯＮ検出処理																		*/
/*--------------------------------------------------------------------------------------------------*/
#if 0		/* <S21D> */
	else if( (PowSts->Acon1Sig != FALSE) ||
			 ((PowerChecker->conf.DetectAcOn2 != FALSE) && (PowSts->Acon2Sig != FALSE)) )
#else		/* <S21D> */
	else if( ( (PowerChecker->conf.Dcbus == FALSE)
			&& ( (PowSts->Acon1Sig != FALSE)
			  || ( (PowerChecker->conf.DetectAcOn2 != FALSE) && (PowSts->Acon2Sig != FALSE) ) ) )
		  || ( (PowerChecker->conf.Dcbus == TRUE)
			&& (PowSts->ConvRdySig == TRUE) ) )
#endif

	{
		PowSts->Acon = TRUE;									/* AC Power On						*/
		PowerChecker->var.AcoffChkTimer = 0;					/* Reset Power Off Check Timer		*/
	}
	/*----------------------------------------------------------------------------------------------*/
	else
	{
		/* <S21D> Start */
		/* 瞬停保持処理 */
#if 0
		/* 2011.04.04 Y.Oka <V762> */
		if( PowerChecker->conf.Dcbus )
		{
			PowSts->Acon = FALSE;								/* AC Power Off						*/
		}
//		if( PowerChecker->var.AcoffChkTimer < PowerChecker->conf.AcoffDetctTime )
		/* 2011.04.04 Y.Oka <V762> */
		else if( PowerChecker->var.AcoffChkTimer < PowerChecker->conf.AcoffDetctTime )
#else
		if( PowerChecker->var.AcoffChkTimer < PowerChecker->conf.AcoffDetctTime )
#endif
		/* <S21D> End */

		{
			PowerChecker->var.AcoffChkTimer += KPI_SCCYCLEMS;
			if( PowerChecker->var.AcoffChkTimer > 30 )
			{
				PowSts->PowerOnEep = FALSE;						/* Power Off for EEPROM				*/
			}
		}
		else
		{
			PowSts->Acon = FALSE;								/* AC Power Off						*/
			PowSts->PowerOnEep = FALSE;							/* Power Off for EEPROM				*/
		}
	}

	return;
}



/****************************************************************************************************/
/*																									*/
/*		ＤＣ電源監視処理																			*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 入力電源投入後のＤＣ電源充電状態を監視し、運転準備完了フラグを立てる。					*/
/*			充電完了後は、充電未完フラグをオフし、突入電流制限抵抗を短絡する指示をする。			*/
/*			容量により、突入電流制限抵抗の短絡動作はＤＢ回路オフの動作と回路を共通化して			*/
/*			いるものがある。突入電流制限抵抗の短絡動作は専用の処理モジュールにて行う。				*/
/*			充電に失敗した場合、充電失敗アラームとする。従来不足電圧と同アラームコードで			*/
/*			あったが、アラームコード体系の見直しにより区別する。									*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
//static	void	LpxDcPowerManager( CHECK_MAIN_POWER *PowerChecker, POWER_STATUS *PowSts, BOOL MotStop )		/* <S21D> */
static	void	LpxDcPowerManager( CHECK_MAIN_POWER *PowerChecker, POWER_STATUS *PowSts, 						/* <S21D> */
												BOOL MotStop, REGENE_CTRL *RegeneCtrl )

{
/*--------------------------------------------------------------------------------------------------*/
/*		テストモード																				*/
/*--------------------------------------------------------------------------------------------------*/
	if( PowerChecker->conf.TestMode1 == TRUE )
	{
		PowSts->PowerOn = TRUE;										/* Power On for Control			*/
		PowSts->PowerOnEep = FALSE;									/* Power On for EEPROM			*/
		PowSts->ChargeOk = TRUE;									/* Charge OK for rush			*/
	}
#if( CSW_HW_DCBUS == FALSE )
	else if( PowerChecker->conf.Dcbus == FALSE )
	{
	/*----------------------------------------------------------------------------------------------*/
	/*		AC電源入力モード																		*/
	/*----------------------------------------------------------------------------------------------*/
//		if( PowSts->Acon )				/* <S21D> */
		if( PowSts->Acon == TRUE )		/* <S21D> */
		{
			//if( PowerChecker->var.DconChkTimer >= POWREADY_WAIT )
			if( PowerChecker->var.DconChkTimer >= PowerChecker->conf.PowerReadyWaitTime ) /* <S053>  */
			{ /* パワーレディ待ち時間経過 */
				PowSts->PowerOn = TRUE;								/* Power On for Control			*/
				PowSts->PowerOnEep = TRUE;							/* Power On for EEPROM			*/
			}
			//else if( PowerChecker->var.DconChkTimer >= POWCHARG_WAIT )
			else if( PowerChecker->var.DconChkTimer >= PowerChecker->conf.PowerChargeWaitTime ) /* <S053> */
			{
				if( PowSts->DcVolt >= PowerChecker->conf.UvLevel )	/* DC電圧チェック				*/
				{
					/*------------------------------------------------------------------------------*/
					/* DB動作改善：回転中の主回路OFF → ON対策										*/
					/*------------------------------------------------------------------------------*/
					switch( PowerChecker->conf.RlyDbWay )
					{
					case DBCTRL_TRIACK_SEPARATE:		/* 突入：リレー；DB:サイリスタ 				*/
					case DBCTRL_RLY_SEPARATE:			/* 突入：リレー；DB：リレー 				*/
					case DBCTRL_TRIACK_COMMON:			/* 突入+DB:リレー+サイリスタ 				*/
						PowSts->ChargeOk = TRUE;		/* モータ回転中に関わらず					*/
						break;

					case DBCTRL_RLY_COMMON:				/* 突入＋ＤＢ：リレー共通 					*/
					default:							/* ありえない設定時 リレー共通と同じにする	*/
						//if(PowerChecker->var.DconChkTimer == POWCHARG_WAIT)
						if(PowerChecker->var.DconChkTimer == PowerChecker->conf.PowerChargeWaitTime )
						{
							if( MotStop == TRUE )
							{ /* モータ停止中 */
								/* 停止でチャージ完了 */
								PowSts->ChargeOk = TRUE;
							}
							else
							{ /* モータ回転中 */
								/* 回転中は留まる */
								//PowerChecker->var.DconChkTimer = POWCHARG_WAIT - 1;
								PowerChecker->var.DconChkTimer = PowerChecker->conf.PowerChargeWaitTime - 1;	/* <S053> */
							}
						}
						break;
					}
					PowerChecker->var.DconChkTimer++;
				}
				else
				{
					ALMSetGlobalAlarm( ALM_PUV );				/* A.410 : 不足電圧					*/
					PowSts->PowerOn = FALSE;
					PowSts->PowerOnEep = FALSE;
					PowSts->ChargeOk = FALSE;
					PowerChecker->var.DconChkTimer = 0;
				}
			}
			else
			{
				PowerChecker->var.DconChkTimer++;
			}
		}
	/*----------------------------------------------------------------------------------------------*/
	/*		ＡＣ電源ＯＦＦ																			*/
	/*----------------------------------------------------------------------------------------------*/
		else
		{
			PowSts->PowerOn  = FALSE;
			PowSts->PowerOnEep = FALSE;
			PowSts->ChargeOk = FALSE;
			PowerChecker->var.DconChkTimer = 0;
		}
	}
#endif
	else
	{
	/*----------------------------------------------------------------------------------------------*/
	/*		DC電源入力モード																		*/
	/*----------------------------------------------------------------------------------------------*/
#if 0	/* <S21D> */
		if( PowSts->DcVolt >= PowerChecker->conf.UvLevel )			/* DC電圧チェック				*/
		{
			//if( PowerChecker->var.DconChkTimer >= POWREADY_WAIT )
			if( PowerChecker->var.DconChkTimer >= PowerChecker->conf.PowerReadyWaitTime ) /* <S053>  */
			{ /* パワーレディ待ち時間経過 */
				PowSts->PowerOn = TRUE;								/* Power On for Control			*/
				PowSts->PowerOnEep = TRUE;							/* Power On for EEPROM			*/
			}
			else
			{
				//if( PowerChecker->var.DconChkTimer >= POWCHARG_WAIT )
				if( PowerChecker->var.DconChkTimer >= PowerChecker->conf.PowerChargeWaitTime ) /* <S053> */
				{
					/*------------------------------------------------------------------------------*/
					/* DB動作改善：回転中の主回路OFF → ON対策										*/
					/*------------------------------------------------------------------------------*/
					switch( PowerChecker->conf.RlyDbWay )
					{
					case DBCTRL_RLY_COMMON:				/* 突入＋ＤＢ：リレー共通 					*/
					default:							/* ありえない設定時 リレー共通と同じにする	*/
						//if(PowerChecker->var.DconChkTimer == POWCHARG_WAIT)
						if(PowerChecker->var.DconChkTimer == PowerChecker->conf.PowerChargeWaitTime ) /* <S053> */
						{
							if( MotStop == TRUE )
							{ /* モータ停止中 */
								/* 停止でチャージ完了 */
								PowSts->ChargeOk = TRUE;
							}
							else
							{ /* モータ回転中 */
								/* 回転中は留まる */
								//PowerChecker->var.DconChkTimer = POWCHARG_WAIT - 1;
								PowerChecker->var.DconChkTimer = PowerChecker->conf.PowerChargeWaitTime - 1; /* <S053> */
							}
						}
						break;
					case DBCTRL_TRIACK_SEPARATE:		/* 突入：リレー；DB:サイリスタ 				*/
					case DBCTRL_RLY_SEPARATE:			/* 突入：リレー；DB：リレー 				*/
					case DBCTRL_TRIACK_COMMON:			/* 突入+DB:リレー+サイリスタ 				*/
						PowSts->ChargeOk = TRUE;		/* 時間で突入回路の操作を行う				*/
						break;
					}
				}
				PowerChecker->var.DconChkTimer++;
			}
		}
		else
		{
			PowSts->PowerOn = FALSE;
			PowSts->PowerOnEep = FALSE;
			PowSts->ChargeOk = FALSE;
			PowerChecker->var.DconChkTimer = 0;
		}
	}
#else	/* <S21D> */
		if( PowSts->Acon == TRUE )
		{
			if( PowSts->DcVolt >= (LONG)PowerChecker->conf.UvLevel )		/* DC電圧チェック				*/
			{
				//if( PowerChecker->var.DconChkTimer >= POWREADY_WAIT )
				if( PowerChecker->var.DconChkTimer >= PowerChecker->conf.PowerReadyWaitTime ) /* <S053>  */
				{ /* パワーレディ待ち時間経過 */
					PowSts->PowerOn = TRUE;								/* Power On for Control			*/
					PowSts->PowerOnEep = TRUE;							/* Power On for EEPROM			*/
				}
				else
				{
					//if( PowerChecker->var.DconChkTimer >= POWCHARG_WAIT )
					if( PowerChecker->var.DconChkTimer >= PowerChecker->conf.PowerChargeWaitTime ) /* <S053> */
					{
						/*------------------------------------------------------------------------------*/
						/* DB動作改善：回転中の主回路OFF → ON対策										*/
						/*------------------------------------------------------------------------------*/
						switch( PowerChecker->conf.RlyDbWay )
						{
						case DBCTRL_RLY_COMMON:				/* 突入＋ＤＢ：リレー共通 					*/
						default:							/* ありえない設定時 リレー共通と同じにする	*/
							//if(PowerChecker->var.DconChkTimer == POWCHARG_WAIT)
							if(PowerChecker->var.DconChkTimer == PowerChecker->conf.PowerChargeWaitTime ) /* <S053> */
							{
								if( MotStop == TRUE )
								{ /* モータ停止中 */
									/* 停止でチャージ完了 */
									PowSts->ChargeOk = TRUE;
								}
								else
								{ /* モータ回転中 */
									/* 回転中は留まる */
									//PowerChecker->var.DconChkTimer = POWCHARG_WAIT - 1;
									PowerChecker->var.DconChkTimer = PowerChecker->conf.PowerChargeWaitTime - 1; /* <S053> */
								}
							}
							break;
						case DBCTRL_TRIACK_SEPARATE:		/* 突入：リレー；DB:サイリスタ 				*/
						case DBCTRL_RLY_SEPARATE:			/* 突入：リレー；DB：リレー 				*/
						case DBCTRL_TRIACK_COMMON:			/* 突入+DB:リレー+サイリスタ 				*/
							PowSts->ChargeOk = TRUE;		/* 時間で突入回路の操作を行う				*/
							break;
						}
					}
					PowerChecker->var.DconChkTimer++;
				}
			}
			else
			{
				if( PowerChecker->var.DconChkTimer >= (ULONG)PowerChecker->conf.PowerChargeWaitTime )
				{ /* ＤＣ充電完了待ち時間経過 */
					ALMSetGlobalAlarm( ALM_PUV );					/* A.410 : 不足電圧				*/
					PowSts->PowerOn = FALSE;
					PowSts->PowerOnEep = FALSE;
					PowerChecker->var.DconChkTimer = 0U;
				}
				PowerChecker->var.DconChkTimer++;

			}
		}
		else
		{
			PowSts->PowerOn = FALSE;
			PowSts->PowerOnEep = FALSE;
			PowerChecker->var.DconChkTimer = 0U;
		}
		PowSts->ChargeOk = PowSts->ConvMconSig;
#endif
	}
	/* <S21D> Start */
	/* コンデンサバンク制御	*/
	PowerChecker->var.ClinkOut = PowSts->ChargeOk;
	if( PowerChecker->conf.CapaLinkFlag == TRUE )			/* コンデンサバンク接続フラグ == TRUE		*/
	{
		if( PowerChecker->var.ClinkOut == TRUE )				/* コンデンサバンク接続出力信号 == ON	*/
		{
			if( PowSts->ClinkAnsSig == FALSE )					/* /CLINKANSが500ms以上変化しない場合	*/
			{
				if( PowerChecker->var.ClinkAnsErrChkTimer >= (ULONG)CLINKANSERRCHK_TIME )
				{
					ALMSetGlobalAlarm( ALM_CAPALINK );			/* A.332 : コンデンサバンク接続アンサー異常	*/
				}
				else
				{
					PowerChecker->var.ClinkAnsErrChkTimer++;
				}
			}
			else
			{
				PowerChecker->var.ClinkAnsErrChkTimer = 0U;
			}
		}
		else
		{
			PowerChecker->var.ClinkAnsErrChkTimer = 0U;
		}

		if( PowSts->ClinkAnsSig == TRUE )					/* コンデンサバンク接続監視信号（/CLINKANS）== ON	*/
		{
			if( PowerChecker->var.ClinkAnsTimer >= (ULONG)CMCON_WAIT )
			{
				PowerChecker->var.CmconOut = TRUE;
				if( PowSts->CmcAnsSig == FALSE )			/* 突防リレー状態監視信号（/CMCANS）が/CMCONと異なる場合	*/
				{
					if( PowerChecker->var.CmcAnsErrChkTimer >= (ULONG)CMCANSERRCHK_TIME )
					{
						ALMSetGlobalAlarm( ALM_CAPAMC );	/* A.333 : コンデンサバンクMCアンサー異常	*/
					}
					else
					{
						PowerChecker->var.CmcAnsErrChkTimer++;
					}
					PowerChecker->var.CapaChargeOk = FALSE;
				}
				else
				{
					PowerChecker->var.CmcAnsErrChkTimer = 0U;
					PowerChecker->var.CapaChargeOk = TRUE;
				}
			}
			else
			{
				PowerChecker->var.CmconOut = FALSE;
				PowerChecker->var.CmcAnsErrChkTimer = 0U;
				PowerChecker->var.ClinkAnsTimer++;
			}
		}
		else
		{
			PowerChecker->var.CmconOut = FALSE;
			PowerChecker->var.CmcAnsErrChkTimer = 0U;
			PowerChecker->var.ClinkAnsTimer = 0U;
		}
	}
	else
	{
		PowerChecker->var.CapaChargeOk = TRUE;
	}
	/* <S21D> End */
}



/****************************************************************************************************/
/*																									*/
/*		ＤＣ電源急速放電処理																		*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 主電源オフ時、DCラインに高電圧が残らないように、回生処理機能を流用して					*/
/*			急速放電処理を行う。																	*/
/*																									*/
/*			1) 400W以下では、標準では回生抵抗を内蔵していないので、本機能は実際には動作しない。		*/
/*			2) 制御電源投入時、通常処理に入って10ms間急速放電処理をしない。							*/
/*			   これは、ACONを半波で見ているため、制御・メイン同時投入された場合、電源半周期			*/
/*			   ACONを検出できずに、電源が入っているのに放電することを防ぐためである。				*/
/*			3) DC入力仕様があり、ユーザー定数で設定するようになっている。							*/
/*			   AC入力仕様のままで、DC入力されたり、逆の設定をされて、アンプを焼損したこと			*/
/*			   がある。																				*/
/*			4) 間違った設定の焼損対策として、ACON信号とDC電圧状態より、急速放電に					*/
/*			   インタロックを設ける。																*/
/*			   ACON==FALSEで、一定時間急速放電処理しても、DC電圧が下がらない場合、					*/
/*			   DC入力されているとみなし、配線アラームA.33を発生して、急速放電処理を止める。			*/
/*			   但し、400W以下は回生抵抗を内蔵していないので早く放電できないので、A.33を見ない。		*/
/*			5) DC入力仕様の設定なのにAC入力された場合もA.33アラームとする。							*/
/*			   DC入力仕様では、回生動作を止めるので、AC入力されるとダイオードブリッジに				*/
/*			   より回生エネルギーの逃げ場が無く過電圧になる。これを防ぐためである。					*/
/*			   DC入力仕様の時は、＋，－入力端子からDC入力しないといけない。							*/
/*			   DC入力仕様で回生動作を止めるのは、DC入力仕様では多軸をDCバスでつなぐ場合				*/
/*			   が一般的で、回生処理を行うと検出レベルの低い1軸に回生処理が集中し、回生処理回		*/
/*			   路が故障する危険があるからである。													*/
/*			   DC入力仕様で回生処理機能が欲しい場合はY仕様で対応する。								*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
//static	void	LpxDischargeDcPower( CHECK_MAIN_POWER *PowerChecker, POWER_STATUS *PowSts,
//																			REGENE_CTRL *RegCtrl )
static	void	LpxDischargeDcPower( CHECK_MAIN_POWER *PowerChecker, POWER_STATUS *PowSts,
																REGENE_CTRL *RegCtrl, BOOL BeSts )
{
#if( CSW_HW_DCBUS == TRUE )
/* Mask Function */
	return;
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		DC電源入力モード || テストモード1 : 急速放電処理をしない									*/
/*--------------------------------------------------------------------------------------------------*/
	if( (PowerChecker->conf.Dcbus == TRUE) || (PowerChecker->conf.TestMode1 == TRUE) )
	{
#if 0	/* <S21D> */
		PowSts->Discharge = FALSE;
		PowerChecker->var.DischChkTimer = KPI_SCANC_MS( 500 );
#else	/* <S21D> */
		if( (PowerChecker->conf.Dcbus == TRUE) && (PowerChecker->conf.DcDischarge == TRUE) )
		{
			if( PowSts->Discharge == TRUE )
			{
				if( PowerChecker->var.ConvInSigDischErrChkTimer >= (ULONG)CONVINSIGDISCHERRCHK_TIME )
				{
					if( (PowerChecker->var.DcVoltOld - PowSts->DcVolt) < DCVOLT10V )
					{
						ALMSetGlobalAlarm( ALM_CONVIO );			/* A.331 : 電源関連入力信号異常 */
					}
				}
				else
				{
					PowerChecker->var.ConvInSigDischErrChkTimer++;
				}
			}
			else
			{
				PowerChecker->var.ConvInSigDischErrChkTimer = 0U;
				PowerChecker->var.DcVoltOld = PowSts->DcVolt;
			}

			if( PowSts->Acon == TRUE )
			{ /* コンバータ準備完了 */
				PowSts->Discharge = FALSE;
				PowerChecker->var.DischChkTimer = (ULONG)KPI_SCANC_MS( 500 );
			}
			else if( PowerChecker->var.DischChkTimer >= (ULONG)KPI_SCANC_MS(500) )
			{
				PowSts->Discharge = TRUE;								/* 急速放電ON				*/
			}
			else
			{ /* do nothing */
				;
			}
		}
		else
		{
			PowSts->Discharge = FALSE;
			PowerChecker->var.DischChkTimer = (ULONG)KPI_SCANC_MS( 500 );
		}
#endif

/* 2011.04.04 Y.Oka <V762> */
#if 0
		if( (PowerChecker->conf.Dcbus == TRUE) && (PowSts->Acon == TRUE) )
		{
			ALMSetGlobalAlarm( ALM_WIR );					/* A.330 : 主回路配線エラー				*/
		}
#endif
		if( (PowerChecker->conf.Dcbus) && (PowSts->DcVolt > PowerChecker->conf.UvLevel) )
		{ /* DC入力選択 && DC電圧がUVレベルを超えている */
			if( (!PowSts->PowerOn) && (PowSts->Acon) )
			{ /* PowerOn=TRUEまでの240ms間でACON検出 */
				/* ACONカウンタインクリメント */
				PowerChecker->var.AconDetectCount++;
				if( PowerChecker->var.AconDetectCount >= KPI_SCANC_MS(10) )
				{ /* ACON検出後10ms経過 */
					/* A.330 : 主回路配線エラー */
					ALMSetGlobalAlarm( ALM_WIR );
					/* A.330の検出をラッチ */
					PowerChecker->var.Alm330Lt = TRUE;
					/* ACONカウンタ = 10ms */
					PowerChecker->var.AconDetectCount = KPI_SCANC_MS(10);
				}
			}
			else if( PowerChecker->var.Alm330Lt )
			{ /* 前スキャンにてA.330検出をラッチ */
				/* A.330 : 主回路配線エラー */
				ALMSetGlobalAlarm( ALM_WIR );
			}
			else
			{ /* DC電源入力(正常) */
				PowerChecker->var.Alm330Lt = FALSE;
				PowerChecker->var.AconDetectCount = 0;
			}
		}
		else
		{ /* DC電圧値がUVレベル以下 || DC入力選択でない場合(AC入力選択でTestMode) */
			PowerChecker->var.Alm330Lt = FALSE;
			PowerChecker->var.AconDetectCount = 0;
		}
/* 2011.04.04 Y.Oka <V762> */

	}
/*--------------------------------------------------------------------------------------------------*/
/*		AC電源入力モード : 急速放電処理を行う														*/
/*--------------------------------------------------------------------------------------------------*/
//	else if( PowSts->Acon == TRUE )																	  /* <S0F6> */
	else if( (PowSts->Acon == TRUE) || (BeSts == TRUE) )											  /* <S0F6> */
	{ /* ACON中 */
		PowSts->Discharge = FALSE;
		PowerChecker->var.DischChkTimer = KPI_SCANC_MS( 500 );
		PowerChecker->var.DischSeqNo = DISCH_INIT;
	}
	else if( PowerChecker->var.DischChkTimer >= KPI_SCANC_MS(500) )
	{ /* 通常はACONが落ちてBaseBlockならば、急速放電シーケンス開始 */
//		PowSts->Discharge = TRUE;												/* 急速放電ON		*//* <S1C6> */
		PowSts->Discharge = DischargeSequence( PowerChecker, PowSts->DcVolt );	/* 急速放電Sequence	*//* <S1C6> */
	}
/*--------------------------------------------------------------------------------------------------*/
/*		ＡＣ電源入力モード : 以下のパスは最初しか通らない 											*/
/*--------------------------------------------------------------------------------------------------*/
	else if( PowerChecker->var.DischChkTimer < KPI_SCANC_MS(10) )
	{ /* 制御電源投入時(Acon=LO)のみ通る処理。10ms間は急速放電をしない */
		PowerChecker->var.DischChkTimer++;
		PowSts->Discharge = FALSE;								/* 急速放電OFF						*/
	}
	else if( PowerChecker->var.DischChkTimer < KPI_SCANC_MS(310) )	/* 300ms間急速放電する			*/
	{
		PowerChecker->var.DischChkTimer++;
		PowerChecker->var.WireAlmDetWait = 0;
		PowSts->Discharge = TRUE;								/* 急速放電ON						*/
	}
	else														/* 310ms後にDC電圧が				*/
	{															/* 高いレベルにあったら異常とみなす	*/
		PowSts->Discharge = FALSE;								/* 急速放電OFF						*/
		if( (PowSts->DcVolt > DCVOLT60V)
			&& ((RegCtrl->conf.ResistorCapa != 0) || (RegCtrl->conf.RegRxIn)) )
		{ /* 電源半周期ACON検出できないため、電源半周期(10ms)+2ms経過後にA.330を検出する			*/
			if( PowerChecker->var.WireAlmDetWait > KPI_SCANC_MS(10) )
			{
				ALMSetGlobalAlarm( ALM_WIR );					/* A.330 : 主回路配線エラー			*/
			}
			else
			{
				PowerChecker->var.WireAlmDetWait++;
			}
		}
		else
		{
			PowerChecker->var.WireAlmDetWait = 0;
		}
	}
#if 0 /* 2010.04.19 Y.Oka ASICの指令は使用しないためコメントアウト */
/*--------------------------------------------------------------------------------------------------*/
/*		急速放電出力処理																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( CoutK.f.Discharge == TRUE )
	{
		KPI_DISCHARGEON( );									/* ASIC DisCharge ON					*/
	}
	else
	{
		KPI_DISCHARGEOFF( );								/* ASIC DisCharge OFF					*/
	}
#endif
	return;
}



/****************************************************************************************************/
/*																									*/
/*		急速放電シーケンス処理													<S1C6>				*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 急速放電実効制御を行う。																*/
/*																									*/
/*			DISCH_CHK_TIME[ms]ずつ、3回に渡り急速放電し、主回路電圧が低下するかどうかを確認する。	*/
/*			主回路電圧が低下しない場合は、急速放電は行わない。										*/
/*																									*/
/****************************************************************************************************/
static	BOOL	DischargeSequence( CHECK_MAIN_POWER *PowerChecker, LONG DcVolt )
{
	BOOL	DischReq;	/* 急速放電要求 */
	DischReq = FALSE;

	switch	( PowerChecker->var.DischSeqNo )
	{
	case DISCH_INIT: /* 急速放電イニシャル */
		PowerChecker->var.DischStartVolt = DcVolt;					/* 急速放電確認開始時電圧記憶	*/
		PowerChecker->var.DischSeqNo = DISCH_CHK1ST;				/* 急速放電確認1回目へ			*/
		PowerChecker->var.DischActTimer = 1;						/* 急速放電実施時間クリア		*/
		DischReq = TRUE;											/* 急速放電開始					*/
	break;

	case DISCH_CHK1ST: /* 急速放電確認1回目 */
		if( PowerChecker->var.DischActTimer == KPI_SCANC_MS(DISCH_CHK_TIME) )
		{ /* 急速放電時間DISCH_CHK_TIME(20ms)経過 */
			/* 1Scan分急速放電を止める */
			DischReq = FALSE;										/* 急速放電一時停止				*/
			PowerChecker->var.DischActTimer++;
		}
		else if( PowerChecker->var.DischActTimer > KPI_SCANC_MS(DISCH_CHK_TIME) )
		{
			if( (PowerChecker->var.DischStartVolt - DcVolt) < PowerChecker->conf.DischCheckVlt )	  /* <S1D1> */
			{ /* 電圧減少幅がPowerChecker->conf.DischCheckVlt(10V)以下の場合 */
				/* 急速放電に異常あり */
				DischReq = FALSE;									/* 急速放電停止					*/
				PowerChecker->var.DischSeqNo = DISCH_FINISH;		/* 完了ステップへ				*/
			}
			else
			{ /* 急速放電による電圧の減少が確認閾値を超えた場合 */
				DischReq = TRUE;									/* 急速放電再開					*/
				PowerChecker->var.DischStartVolt = DcVolt;			/* 急速放電確認開始時電圧記憶	*/
				PowerChecker->var.DischActTimer = 1;				/* 急速放電実施時間クリア		*/
				PowerChecker->var.DischSeqNo = DISCH_CHK2ND;		/* 急速放電確認2回目へ			*/
			}
		}
		else
		{
			DischReq = TRUE;										/* 確認用急速放電中				*/
			PowerChecker->var.DischActTimer++;
		}
	break;

	case DISCH_CHK2ND: /* 急速放電確認2回目 */
		if( PowerChecker->var.DischActTimer == KPI_SCANC_MS(DISCH_CHK_TIME) )
		{ /* 急速放電時間DISCH_CHK_TIME(20ms)経過 */
			/* 1Scan分急速放電を止める */
			DischReq = FALSE;										/* 急速放電一時停止				*/
			PowerChecker->var.DischActTimer++;
		}
		else if( PowerChecker->var.DischActTimer > KPI_SCANC_MS(DISCH_CHK_TIME) )
		{
			if( ((PowerChecker->var.DischStartVolt - DcVolt) < PowerChecker->conf.DischCheckVlt)	  /* <S1D1> */
				&& (DcVolt > PowerChecker->conf.DischCompVlt) )
			{ /* 電圧減少幅がPowerChecker->conf.DischCheckVlt(10V)以下 && 残留電圧がDischCompVlt(50V)以上 */
				/* 急速放電に異常あり */
				DischReq = FALSE;									/* 急速放電停止					*/
				PowerChecker->var.DischSeqNo = DISCH_FINISH;		/* 完了ステップへ				*/
			}
			else
			{ /* 急速放電による電圧の減少が確認閾値を超えた || 残留電圧がDischCompVlt(50V)以下		*/
				DischReq = TRUE;									/* 急速放電再開					*/
				PowerChecker->var.DischStartVolt = DcVolt;			/* 急速放電確認開始時電圧記憶	*/
				PowerChecker->var.DischActTimer = 1;				/* 急速放電実施時間クリア		*/
				PowerChecker->var.DischSeqNo = DISCH_CHK3RD;		/* 急速放電確認3回目へ			*/
			}
		}
		else
		{
			DischReq = TRUE;										/* 確認用急速放電中				*/
			PowerChecker->var.DischActTimer++;
		}
		break;

	case DISCH_CHK3RD: /* 急速放電確認3回目 */
		if( PowerChecker->var.DischActTimer == KPI_SCANC_MS(DISCH_CHK_TIME) )
		{ /* 急速放電時間DISCH_CHK_TIME(20ms)経過 */
			/* 1Scan分急速放電を止める */
			DischReq = FALSE;										/* 急速放電一時停止				*/
			PowerChecker->var.DischActTimer++;
		}
		else if( PowerChecker->var.DischActTimer > KPI_SCANC_MS(DISCH_CHK_TIME) )
		{
			if( ((PowerChecker->var.DischStartVolt - DcVolt) < PowerChecker->conf.DischCheckVlt)	  /* <S1D1> */
				&& (DcVolt > PowerChecker->conf.DischCompVlt) )
			{ /* 電圧減少幅がPowerChecker->conf.DischCheckVlt(10V)以下 && 残留電圧がDischCompVlt(50V)以上 */
				/* 急速放電に異常あり */
				DischReq = FALSE;									/* 急速放電停止					*/
				PowerChecker->var.DischSeqNo = DISCH_FINISH;		/* 完了ステップへ				*/
			}
			else
			{ /* 急速放電による電圧の減少が確認閾値を超えた || 残留電圧がDischCompVlt(50V)以下		*/
				DischReq = TRUE;									/* 急速放電再開					*/
				PowerChecker->var.DischStartVolt = DcVolt;			/* 急速放電確認開始時電圧記憶	*/
				PowerChecker->var.DischActTimer = 1;				/* 急速放電実施時間クリア		*/
				PowerChecker->var.DischSeqNo = DISCH_ACTIVE;		/* 急速放電本番へ				*/
			}
		}
		else
		{
			DischReq = TRUE;										/* 確認用急速放電中				*/
			PowerChecker->var.DischActTimer++;
		}
		break;

	case DISCH_ACTIVE: /* 急速放電 */
		if( PowerChecker->var.DischActTimer >= PowerChecker->conf.MainDischActTime )
		{ /* メイン急速放電時間(充電時間と同一)経過 */
			DischReq = FALSE;										/* 急速放電停止					*/
			PowerChecker->var.DischActTimer = 0;
			PowerChecker->var.DischSeqNo = DISCH_FINISH;			/* 完了ステップへ				*/
		}
		else
		{
			DischReq = TRUE;										/* 確認用急速放電中				*/
			PowerChecker->var.DischActTimer++;
		}
		break;

	case DISCH_FINISH: /* 急速放電完了 or 異常終了 (主回路再投入待ち) */
		DischReq = FALSE;											/* 急速放電停止					*/
																	/* 完了後は呼元でステップ初期化 */
		break;

	default: /* 本ケースへは来ない */
		DischReq = FALSE;											/* 急速放電停止					*/
		break;

	}

	return	DischReq;
}



/****************************************************************************************************/
/*																									*/
/*		ＡＣ電源欠相検出処理																		*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 電源欠相検出方式により、処理を分岐する。												*/
/*																									*/
/*			PnE0C bit2 = 0 : ACON1信号のみで、欠相検出を行う。										*/
/*			PnE0C bit2 = 1 : ACON1/ACON2信号で、欠相検出を行う。									*/
/*																									*/
/****************************************************************************************************/
static	void	LpxCheckAcOpenPhase( CHECK_MAIN_POWER *PowerChecker, POWER_STATUS *PowSts )
{

#if( CSW_HW_DCBUS == TRUE )
/* Mask Function */
	return;
#endif

	if( PowerChecker->conf.DetectAcOn2 != TRUE )
	{
		/* ＡＣ電源欠相検出処理 (ACON1信号のみで検出) */
		LpxDetectAcOpenACON1( PowerChecker, PowSts );
	}
	else
	{
		/* ＡＣ電源欠相検出処理 (ACON1/ACON2信号で検出) */
		LpxDetectAcOpenACON12( PowerChecker, PowSts );
	}
	return;
}



#if( CSW_HW_DCBUS == FALSE )
/****************************************************************************************************/
/*																									*/
/*		ＡＣ電源欠相検出処理	(ACON1信号のみで検出)												*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : ３相入力仕様のアンプに対して欠相検出を行う。											*/
/*			欠相検出をしないと、モータ高出力時ＤＣ電圧が降下し、電圧不足で出力が					*/
/*			出なくなる危険がある。																	*/
/*			また、コンデンサのリップル電流が増え、劣化が加速する。									*/
/*																									*/
/*			検出方式は、Acon1信号(1=AC電源ON, 0=AC電源OFF)が、										*/
/*			ベタLoなら正常、パルス状なら欠相、ベタHiなら主電源オフとする。							*/
/*																									*/
/*			欠相検出時間は1000msとし、ACON状態変化がLO->HIを100回以上で、							*/
/*			電源欠相アラーム(A.F10)とする。															*/
/*			欠相状態になった場合、50Hz電源で10ms毎にLO->HI状態変化を								*/
/*			ScanCにて検出できるという条件では、1000msで100回状態変化が起きる。						*/
/*			よって100回状態変化が起きた場合に電源欠相アラーム(A.F10)とする。						*/
/*																									*/
/*																									*/
/*					       __    __    __    __    __    __    __   								*/
/*			Acon1  _______|  |__|  |__|  |__|  |__|  |__|  |__|  |__								*/
/*																									*/
/*						  |<--------------------------->|											*/
/*								1000ms以上でアラーム												*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	void	LpxDetectAcOpenACON1( CHECK_MAIN_POWER *PowerChecker, POWER_STATUS *PowSts )
{
/*--------------------------------------------------------------------------------------------------*/
/*		ＡＣ電源欠相チェック処理																	*/
/*--------------------------------------------------------------------------------------------------*/
/* 2011.04.04 Y.Oka <V762> */
//	if( (PowerChecker->conf.TestMode1==FALSE) &&
//		(PowerChecker->conf.Ac3Phase==TRUE) && (PowSts->PowerOn==TRUE) )
	if( (PowerChecker->conf.TestMode1 == FALSE)
		&& (PowerChecker->conf.Ac3Phase == TRUE)
		&& (PowSts->PowerOn == TRUE)
		&& (PowerChecker->conf.Dcbus == FALSE) )
/* 2011.04.04 Y.Oka <V762> */
	{
	/*----------------------------------------------------------------------------------------------*/
	/*		欠相検出タイマー更新処理																*/
	/*----------------------------------------------------------------------------------------------*/
		if( PowerChecker->var.OpenChkTimer[0] != 0 )
		{
			PowerChecker->var.OpenChkTimer[0]++;
		}
	/*----------------------------------------------------------------------------------------------*/
	/*		Acon1信号エッジ検出 																	*/
	/*----------------------------------------------------------------------------------------------*/
		if( (PowSts->Acon1Sig == TRUE) && (PowSts->Acon1Sig_l == FALSE) )
		{
			PowerChecker->var.OpenChkTimer[1] = PowerChecker->var.OpenChkTimer[0];
			if( PowerChecker->var.OpenChkTimer[0] == 0 )
			{
				PowerChecker->var.OpenChkTimer[0] = 1;			/* タイマースタート					*/
			}
		}
		else if( (PowerChecker->var.OpenChkTimer[0] - PowerChecker->var.OpenChkTimer[1]) > KPI_SCANC_MS( 12 ) )
		{
			PowerChecker->var.OpenChkTimer[0] = 0;				/* エッジ未検出12ms以上でリセット	*/
			PowerChecker->var.OpenChkTimer[1] = 0;				/*									*/
		}
	/*----------------------------------------------------------------------------------------------*/
	/*		異常チェック : 欠相検出タイマー1000ms以上でアラーム										*/
	/*----------------------------------------------------------------------------------------------*/
		if( PowerChecker->var.OpenChkTimer[0] > ACOPEN_DETECT_WAIT )
		{
			ALMSetGlobalAlarm( ALM_PWPHASE );					/*  A.F10 : 電源欠相				*/

			PowerChecker->var.OpenChkTimer[0] = 0;
			PowerChecker->var.OpenChkTimer[1] = 0;
		}
	}
	else
	{
		PowerChecker->var.OpenChkTimer[0] = 0;
		PowerChecker->var.OpenChkTimer[1] = 0;
	}
	return;
}



/****************************************************************************************************/
/*																									*/
/*		ＡＣ電源欠相検出処理	(ACON1/ACON2信号で検出)												*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : ３相入力仕様のアンプに対して欠相検出を行う。											*/
/*			欠相検出をしないと、モータ高出力時ＤＣ電圧が降下し、電圧不足で出力が					*/
/*			出なくなる危険がある。																	*/
/*			また、コンデンサのリップル電流が増え、劣化が加速する。									*/
/*																									*/
/*			ACON1信号																				*/
/*				HI→LO→HI・・の場合: 正常															*/
/*				ベタLO の場合       : 電源OFF or R相欠相											*/
/*				ベタHI の場合       : ハードウェア上あり得ない										*/
/*			※正常の場合(20ms以内の周期で、HI→LO→HI・・)、AcOn1=TRUEとなる。						*/
/*																									*/
/*			ACON2信号																				*/
/*				HI→LO→HI・・の場合: 正常															*/
/*				ベタLOの場合        : 電源OFF or S相欠相 or T相欠相									*/
/*				ベタHIの場合        : ハードウェア上あり得ない										*/
/*			※正常の場合(20ms以内の周期で、HI→LO→HI・・)、AcOn2=TRUEとなる。						*/
/*																									*/
/*			AcOn1/AcOn2により、下記状態に分類される。												*/
/*				AcOn1= TRUE		AcOn2= TRUE ⇒ 正常													*/
/*				AcOn1=FALSE		AcOn2= TRUE ⇒ R相欠相												*/
/*				AcOn1= TRUE		AcOn2=FALSE ⇒ S相 or T相欠相										*/
/*				AcOn1=FALSE		AcOn2=FALSE ⇒ 電源OFF												*/
/*																									*/
/*			従って、欠相検出は、AcOn1とAcOn2の排他的論理和(XOR)で検出する。							*/
/*			欠相検出時間は1000msとし、電源欠相アラーム(A.F10)とする。								*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	void	LpxDetectAcOpenACON12( CHECK_MAIN_POWER *PowerChecker, POWER_STATUS *PowSts )
{
/*--------------------------------------------------------------------------------------------------*/
/*		ＡＣ電源欠相チェック処理																	*/
/*--------------------------------------------------------------------------------------------------*/
/* 2011.04.04 Y.Oka <V762> */
//	if( PowerChecker->conf.TestMode1 == FALSE )
	if( (PowerChecker->conf.TestMode1 == FALSE) && (PowerChecker->conf.Dcbus == FALSE) )
/* 2011.04.04 Y.Oka <V762> */
	{
	/*----------------------------------------------------------------------------------------------*/
	/*		ACON1信号検出																			*/
	/*----------------------------------------------------------------------------------------------*/
		if( PowSts->Acon1Sig != FALSE)
		{
			PowerChecker->var.DetOpenAcOn1 = TRUE;
			PowerChecker->var.AcOn1OffCount = 0;
		}
		else
		{
			if( PowerChecker->var.AcOn1OffCount > KPI_SCANC_MS( 20 ) )
			{
				PowerChecker->var.DetOpenAcOn1 = FALSE;
			}
			else
			{
				PowerChecker->var.AcOn1OffCount++;
			}
		}
	/*----------------------------------------------------------------------------------------------*/
	/*		ACON2信号検出 																			*/
	/*----------------------------------------------------------------------------------------------*/
		if( PowSts->Acon2Sig != FALSE)
		{
			PowerChecker->var.DetOpenAcOn2 = TRUE;
			PowerChecker->var.AcOn2OffCount = 0;
		}
		else
		{
			if( PowerChecker->var.AcOn2OffCount > KPI_SCANC_MS( 20 ) )
			{
				PowerChecker->var.DetOpenAcOn2 = FALSE;
			}
			else
			{
				PowerChecker->var.AcOn2OffCount++;
			}
		}
	/*----------------------------------------------------------------------------------------------*/
	/*	パワーオン状態 : 欠相チェックを行う															*/
	/*----------------------------------------------------------------------------------------------*/
		if( PowSts->PowerOn == TRUE )
		{
			if(PowerChecker->var.DetOpenAcOn1 ^ PowerChecker->var.DetOpenAcOn2)
			{
				PowerChecker->var.OpenChkTimer[0]++;			/* 欠相検出タイマーカウントアップ	*/
			}
			else
			{
				PowerChecker->var.OpenChkTimer[0] = 0;
		}
	/*----------------------------------------------------------------------------------------------*/
	/*	異常チェック : 欠相検出タイマー1000ms以上でアラーム											*/
	/*----------------------------------------------------------------------------------------------*/
			if( PowerChecker->var.OpenChkTimer[0] > ACOPEN_DETECT_WAIT )
			{
				ALMSetGlobalAlarm( ALM_PWPHASE );				/*  A.F10 : 電源欠相				*/
				PowerChecker->var.OpenChkTimer[0] = 0;
			}
		}
	/*----------------------------------------------------------------------------------------------*/
	/*	パワーオフ状態 : 欠相検出タイマークリア														*/
	/*----------------------------------------------------------------------------------------------*/
		else
		{
			PowerChecker->var.OpenChkTimer[0] = 0;
		}
	}
	return;
}


#endif	/* ( CSW_HW_DCBUS == FALSE ) */

/****************************************************************************************************/
/*																									*/
/*		ＤＣ電源不足電圧(UV)監視処理																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 不足電圧検出はモータの動作不良を防ぐために行う。										*/
/*			ＤＣ電源電圧が下がるとモータＥＭＦが飽和して、モータの回転数が制限されたり、			*/
/*			トルクが低下したりする。																*/
/*																									*/
/*			電源のバックパワーが小さいと起こると考えられるが、通常そんな小さい容量の電源は			*/
/*			無い。突入リレーの故障、ダイオードブリッジの故障、ヒューズ溶断、コンデンサの容			*/
/*			量抜け等部品の故障で起こる場合の方が多い。												*/
/*																									*/
/*			入力電源電圧の下限は、200 x 0.85 = 170VAC --> 240VDC であり、							*/
/*			リップル電圧を考えても、３相の場合 240 x sin60 ≒ 208V まで下がることはない。			*/
/*			単相の場合も相当と考え、マージンをみて、検出レベルを１９０Ｖとする。					*/
/*			他電源電圧対応も同様の割合で考える。													*/
/*			  １００Ｖ系	  ９５Ｖ																*/
/*			  ２００Ｖ系	１９０Ｖ																*/
/*			  ４００Ｖ系	３８０Ｖ																*/
/*																									*/
/*			テストモード(ACON常時でUV検出しないモード)の時はＵＶ検出しない。						*/
/*																									*/
/*																									*/
/*	補 足 : ＤＣ電源過電圧(OV)監視																	*/
/*			過電圧(OV)監視については、フル出力で回生する場合、電圧上昇が早いので、					*/
/*			スキャンＢ(Kernel : KpxCheckDcOverVoltage())で処理する。								*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	void	LpxCheckDcUnderVoltage( CHECK_MAIN_POWER *PowerChecker, POWER_STATUS *PowSts )
{
	if( (PowerChecker->conf.TestMode1 == FALSE) && (PowSts->PowerOn == TRUE) )
	{
		if ( PowSts->DcVolt < PowerChecker->conf.UvLevel )
		{
			if( PowerChecker->var.UvChkTimer >= PowerChecker->conf.UvFilter )
			{
				if( PowSts->DcVolt == 0 )
				{
					ALMSetGlobalAlarm( ALM_VDC );			/* A.030 : 主回路検出部異常				*/
				}
				else
				{
					ALMSetGlobalAlarm( ALM_PUV );			/* A.410 : 不足電圧						*/
				}
			}
			else
			{
				PowerChecker->var.UvChkTimer += KPI_SCCYCLEMS;
			}
		}
		else
		{
			PowerChecker->var.UvChkTimer = 0;
		}
		if ( PowSts->DcVolt < PowerChecker->conf.UvWrnLevel )
		{
			if( PowSts->DcVolt != 0 )
			{
				ALMSetGlobalAlarm( WRN_UV );				/* A.971 : 不足電圧ワーニング検出		*/
			}
		}
		else
		{
			ALMClearGlobalAlarm( WRN_UV );					/* A.971 : 不足電圧ワーニングクリア		*/
		}
	}
	else
	{
		PowerChecker->var.UvChkTimer = 0;
	}
	return;
}



/****************************************************************************************************/
/*																									*/
/*		回生処理 ( 実行周期 : 250us )																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : モータが減速するときエネルギーがアンプに帰ってくるので(回生動作)、DC電圧が上昇する。	*/
/*			DC電圧が上昇すると主回路コンデンサが壊れるので、上昇を止める処理が必要である。			*/
/*			DCラインに回生抵抗を入れ、回生エネルギーを抵抗で消費してDC電圧の上昇を抑える。			*/
/*			回生抵抗の電流は回生トランジスタでON/OFFされる。DC電圧を監視して回生トランジスタ		*/
/*			を制御する。																			*/
/*			小刻みにON/OFFすると回生トランジスタのスイッチングロスが大きくなるので、ヒステリシスを	*/
/*			設ける。																				*/
/*			200V系では、主回路コンデンサ耐圧は400Vであるので、390VでON，380VでOFFとする。他の電源	*/
/*			電圧対応も同じかもっとマージンがあるので、同じ割合の検出レベルで良しとする。			*/
/*																									*/
/*				２００Ｖ系 : ３９０Ｖ…オン、３８０Ｖ…オフ											*/
/*				１００Ｖ系 : １９５Ｖ…オン、１９０Ｖ…オフ											*/
/*				４００Ｖ系 : ７８０Ｖ…オン、７６０Ｖ…オフ											*/
/*																									*/
/****************************************************************************************************/
void	PcmRegeneControl( REGENE_CTRL *pRegCtrl, POWER_STATUS *PowSts, BOOL MotStop )
{

#if	( CSW_HW_REGENERATION_CTRL == TRUE )

#if 0 /* 2010.09.01 Y.Oka 呼び出し元で確認するよう変更 */
/*--------------------------------------------------------------------------------------------------*/
/*		主回路検出パラメータエラーの時、危険回避のため回生を実行しない								*/
/*--------------------------------------------------------------------------------------------------*/
	if( VdetPrmOk == FALSE )
	{
		return;											/* 回生せずreturn							*/
	}
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		回生過負荷アラーム発生チェック																*/
/*--------------------------------------------------------------------------------------------------*/
	if( ALMCheckGlobalAlarm( ALM_RGOL ) )				/* ユニット共通アラームのため、0軸目のみ	*/
	{ /* 回生過負荷(A.320)発生時 */
		if( pRegCtrl->RegAlmMotStop || MotStop )
		{
			pRegCtrl->RegAlmMotStop = TRUE;
		}
		else
		{
			pRegCtrl->RegAlmMotStop = FALSE;
		}
	}
	else
	{
		pRegCtrl->RegAlmMotStop = FALSE;
	}

/* 2010.08.17 Y.Oka 主回路検出部異常(A.030)時は回生処理を行わないよう変更する */
/*--------------------------------------------------------------------------------------------------*/
/*		主回路検出部異常アラーム発生チェック														*/
/*--------------------------------------------------------------------------------------------------*/
	if( ALMCheckGlobalAlarm( ALM_VDC ) )				/* ユニット共通アラームのため、0軸目のみ	*/
	{ /* 主回路検出部異常(A.030)発生時 */
		HALdrv_RegeneControl( FALSE );					/* 回生Trオフ指令							*/
		pRegCtrl->RegStopCnt = 0;						/* 回生動作終了待ちカウンタクリア			*/
		pRegCtrl->RegStartCnt = 0;						/* 回生動作開始待ちカウンタクリア			*/
	}
	else
	{ /* 主回路検出部異常(A.030)未発生時 */
/*--------------------------------------------------------------------------------------------------*/
/*		回生電圧レベルチェック																		*/
/*--------------------------------------------------------------------------------------------------*/
//		if( PowSts->DcVolt > pRegCtrl->conf.RegOnLevel )		/* <S21D> */
		if( PowSts->DcVolt > (LONG)pRegCtrl->conf.RegOnLevel )	/* <S21D> */
		{ /* DC電圧が回生OnLevelを超えた場合 */
			pRegCtrl->RegStopCnt = 0;					/* 回生動作終了待ちカウンタクリア			*/
#if 0	/* <S21D> */
			if( pRegCtrl->RegStartCnt > pRegCtrl->conf.RegStartFilter )
			{ /* 回生動作開始待ち時間経過時 */
				pRegCtrl->RegOn = TRUE;					/* 回生処理を行う							*/
			}
			else
			{ /* 回生動作開始待ち状態 */
				pRegCtrl->RegStartCnt++;				/* 回生動作開始待ちカウンタ更新				*/
			}
#else	/* <S21D> */
			if( pRegCtrl->conf.RegSyn == PNREGSYN )
			{	/* 回生同期有効（Pn00E.3=1）の場合 */
				if( pRegCtrl->RegStartCnt > (pRegCtrl->conf.RegStartFilter + (LONG)pRegCtrl->conf.RegSynWaitTime) )
				{ /* 回生動作開始待ち時間経過時 */
					pRegCtrl->RegOn = TRUE;					/* 回生処理を行う							*/
				}
				else
				{ /* 回生動作開始待ち状態 */
					pRegCtrl->RegStartCnt++;				/* 回生動作開始待ちカウンタ更新				*/
				}
			}
			else
			{
				if( pRegCtrl->RegStartCnt > pRegCtrl->conf.RegStartFilter )
				{ /* 回生動作開始待ち時間経過時 */
					pRegCtrl->RegOn = TRUE;				/* 回生処理を行う							*/
				}
				else
				{ /* 回生動作開始待ち状態 */
					pRegCtrl->RegStartCnt++;			/* 回生動作開始待ちカウンタ更新				*/
				}
			}
#endif
		}
		else
		{
			pRegCtrl->RegStartCnt = 0;					/* 回生動作開始待ちカウンタクリア			*/
//			if( PowSts->DcVolt < pRegCtrl->conf.RegOffLevel )		/* <S21D> */
			if( PowSts->DcVolt < (LONG)pRegCtrl->conf.RegOffLevel )	/* <S21D> */
			{ /* DC電圧が回生OffLevel以下になったら */
				if( pRegCtrl->RegStopCnt > pRegCtrl->conf.RegStopFilter )
				{ /* 回生動作終了待ち時間経過時 */
					pRegCtrl->RegOn = FALSE;			/* 回生処理を止める							*/
				}
				else
				{ /* 回生動作終了待ち状態 */
					pRegCtrl->RegStopCnt++;				/* 回生動作終了待ちカウンタ更新				*/
				}
			}
			else
			{ /* 2010.06.28 Y.Oka 2回連続回生OFFレベルを下回った場合に回生OFFとする。 */
				pRegCtrl->RegStopCnt = 0;				/* 回生動作終了待ちカウンタゼロクリア		*/
			}
		}

/*--------------------------------------------------------------------------------------------------*/
/*		回生Tr ON/OFF出力																			*/
/*--------------------------------------------------------------------------------------------------*/
/*		DC入力でなく、回生不動作モードでない場合に、回生Trがオンとなる条件は以下の通り。			*/
/*			1. 急速放電要求あり(PowSts->Discharge == TRUE)											*/
/*			2. 回生要求あり && 回生異常なし && (回生過負荷なし || 回生過負荷後モータ動作中)			*/
/*--------------------------------------------------------------------------------------------------*/
/*		A.320検出後、モータ停止したら回生Trをオフする。また、A.300検出後、回生Trをオフする			*/
/*		MD開発時は、外付け回生抵抗あり or 回生抵抗内蔵の場合のみ回生Trをオンする(設定ミス時はOV)	*/
/*		としていたが、中容量で回生抵抗を内蔵していないサーボパックで回生異常をマスクするためだけに	*/
/*		適当な抵抗を接続するようなケースがあり、その場合に急速放電しなくなるのはまずいということで	*/
/*		Σ-Ⅴと同等の処理に戻した。	*/
/*--------------------------------------------------------------------------------------------------*/
		if( (PowSts->Discharge == TRUE)
			|| ((pRegCtrl->RegOn == TRUE)
//				&& ((ALMCheckGlobalAlarm(ALM_RG) == FALSE) && (pRegCtrl->RegAlmMotStop == FALSE))) )	/* <S21D> */
			&& ((ALMCheckGlobalAlarm(ALM_RG) == FALSE) && (pRegCtrl->RegAlmMotStop == FALSE))			/* <S21D> */
			&& (ALMCheckGlobalAlarm(WRN_EXREG) == FALSE) ) )		/* A.924（回生警告）なし	*/			/* <S21D> */
		{ /* 回生 || 急速放電要求あり */
#if 0	/* <S21D> */
//			if( (pRegCtrl->conf.ResistorCapa != 0) || (pRegCtrl->conf.RegRxIn) )					  /* <S1CA> */
//			{
				HALdrv_RegeneControl( TRUE );
				if( pRegCtrl->RegOn )
				{
					pRegCtrl->RegOnCount[0] += 2;		/* 回生過負荷検出のため回生TrOnをカウント	*/
				}
//			}
//			else																					  /* <S1CA> */
//			{ /* 外付け回生抵抗なし && 回生抵抗内蔵なし */
//				HALdrv_RegeneControl( FALSE );
//			}
#else	/* <S21D> */
//			if( (pRegCtrl->conf.ResistorCapa != 0U) || (pRegCtrl->conf.RegRxIn == TRUE) )
//			{ /* 外付け回生抵抗あり || 回生抵抗内蔵の場合 */
				if( (pRegCtrl->conf.RegSyn == PNREGSYN)
				 && (ALMCheckGlobalAlarm(WRN_RGOLF) == TRUE) )
				{ /* 回生同期有効 && 回生過負荷ワーニング（A.920）発生の場合 */
					HALdrv_RegeneControl( FALSE );
				}
				else
				{
					HALdrv_RegeneControl( TRUE );
					if( pRegCtrl->RegOn == TRUE )
					{
						pRegCtrl->RegOnCount[0] += 2;		/* 回生過負荷検出のため回生TrOnをカウント	*/
					}
				}
//			}
//			else
//			{ /* 外付け回生抵抗なし && 回生抵抗内蔵なし */
//				HALdrv_RegeneControl( FALSE );
//			}
#endif
		}
		else
		{ /* 回生 || 急速放電要求なし */
			HALdrv_RegeneControl( FALSE );
		}
	}
/* 2011.04.04 Y.Oka <V764> */
/*--------------------------------------------------------------------------------------------------*/
	return;

#endif	//( CSW_HW_REGENERATION_CTRL == FALSE )
}



/****************************************************************************************************/
/*																									*/
/*		DC電源電圧検出処理 ( 実行周期 : 250us ) for SGDS & SGDX										*/
/*																									*/
/*	機能：	主回路電圧検出方式によって検出関数を使い分ける。主回路電圧(KioV.DcVoltx)を作成する。	*/
/*				・PWM - コンパレータ方式（固定追従方式）											*/
/*				・PWM - コンパレータ方式（可変追従方式）											*/
/*				・A/D変換方式																		*/
/*																									*/
/*	戻り値:																							*/
/*			DETECT_DC_NO_OPERATION	: 正常終了(処理不要)											*/
/*			ALM_VDC					: 全軸アラーム出力	A.030 : 主回路検出部異常					*/
/*																									*/
/****************************************************************************************************/
LONG	PcmDetectDcVolt( DET_DC_VOLT *pDetectDc, POWER_STATUS *PowSts )
{
	LONG		ret;
	LONG		DcVolt;

	/* ローカル変数の初期化 */
	ret = DETECT_DC_NO_OPERATION;

/*--------------------------------------------------------------------------------------------------*/
/*		ＤＣ電源電圧検出処理																		*/
/*--------------------------------------------------------------------------------------------------*/
	switch(pDetectDc->conf.VdetMode)
	{
	/*------------------------------------------------------------------------------------------*/
	/*		ＰＷＭ－コンパレータ方式															*/
	/*------------------------------------------------------------------------------------------*/
		/*--------------------------------------------------------------------------------------*/
		/*  固定±2v/250us(小容量),可変±16v/250(中容量)の場合									*/
		/*--------------------------------------------------------------------------------------*/
		case VDET_PWM_FIX_FOLLOW:
		case VDET_PWM_VAR_FOLLOW:
			/* 2010.06.08 Y.Oka 現状未対応 */
//			LpxPwmDetectDcVolt();
			break;
	/*------------------------------------------------------------------------------------------*/
	/*	Ａ／Ｄ変換方式																			*/
	/*------------------------------------------------------------------------------------------*/
		case VDET_AD:
			LpxAdDetectDcVolt( pDetectDc );
			break;
	/*------------------------------------------------------------------------------------------*/
	/*		検出なし	(SVFSEL_SWVDET != 0でここに来るのは異常 DC電圧=0としておく)				*/
	/*------------------------------------------------------------------------------------------*/
		default:
			pDetectDc->DcVoltx = 0;
			break;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		ＤＣ電源電圧検出値補正処理																	*/
/*--------------------------------------------------------------------------------------------------*/
	DcVolt = ((pDetectDc->DcVoltx * (0x100 + pDetectDc->conf.VdetAdadj)) >> 8);
	if( DcVolt >= 0 )
	{
		PowSts->DcVolt = DcVolt;
	}
	else
	{
		PowSts->DcVolt = 0;
	}


/*--------------------------------------------------------------------------------------------------*/
/*		ＤＣ電源電圧検出値異常チェック																*/
/*--------------------------------------------------------------------------------------------------*/
	if( PowSts->Acon == TRUE )
	{ /* Check AC Power ON */
		if( pDetectDc->DcvWaitCnt >= KPI_SCANC_MS(20) )
		{ /* Check DcVolt Wait Count */
			if( (pDetectDc->DcVoltx == 0) || (pDetectDc->DcVoltx == pDetectDc->conf.VdetMaxVolt ) )
			{
				if( pDetectDc->DcvErrCnt > KPI_SCANC_MS(2) )
				{ /* 8回 * 250μ = 2ms 連続でエラー */
//					KPI_BASE_BLOCK( );						/* ＡＳＩＣベースブロック				*/
//					KPI_MPCMD_SVOFF( );						/* マイクロプログラムサーボＯＦＦ		*/
					ALMSetGlobalAlarm( ALM_VDC );			/* A.030 : 主回路検出部異常				*/
					pDetectDc->DcvErrCnt = 0;				/* Reset Error Counter					*/
					pDetectDc->DcvWaitCnt = 0;				/* Reset DcVolt Wait Counter			*/
					ret = ALM_VDC;							/* A.030 : 主回路検出部異常				*/
				}
				else
				{
					pDetectDc->DcvErrCnt++;					/* Increment Error Count				*/
				}
			}
			else
			{
				pDetectDc->DcvErrCnt = 0;				/* Reset DcVolt Wait Counter			*/
			}
		}
		else
		{
			pDetectDc->DcvWaitCnt++;						/* Increment DcVolt Wait Count			*/
		}
	}
	/*----------------------------------------------------------------------------------------------*/
	else													/* AC Power OFF							*/
	{
		pDetectDc->DcvErrCnt = 0;			/* Reset Error Counter					*/
		pDetectDc->DcvWaitCnt = 0;			/* Reset DcVolt Wait Counter			*/
	}
/*--------------------------------------------------------------------------------------------------*/
	return ret;
}



/****************************************************************************************************/
/*																									*/
/*		OV検出処理 ( 実行周期 : 250us )																*/
/*																									*/
/*	機能:	過電圧検出は主回路部品の保護のために行う。												*/
/*			耐圧が最も低いのは主回路コンデンサである。												*/
/*			主回路コンデンサは耐圧を越えると爆発する危険がある。									*/
/*			通常回生処理が働いて、過電圧レベルまでDC電圧が上昇することはない。						*/
/*			回生処理回路の不具合または回生抵抗値の不整合等により過電圧になることがあり得る。		*/
/*			フル出力で回生すると電圧上昇が早いので、ScanBで処理する。								*/
/*			DC電圧レベルを監視して過電圧検出レベルを越えたら異常処理する。							*/
/*																									*/
/*	戻り値:																							*/
/*			DETECT_DC_NO_OPERATION	: 正常終了(処理不要)											*/
/*			ALM_VDC					: 全軸アラーム出力	A.030 : 主回路検出部異常					*/
/*			ALM_OV					: 全軸アラーム出力	A.400 : 過電圧(ＯＶ)						*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	@CHK@ : アラーム検出の論理がおかしい!!!!!														*/
/*			・過電圧(OV)チェックと不足電圧(UV)チェックで、0xFFと0x00をチェックしている				*/
/*			・ＤＣ電源電圧検出値異常チェック(0x00,0xFF)は、何のための異常チェックか？				*/
/*																									*/
/*																									*/
/****************************************************************************************************/
//LONG	PcmCheckOverVoltage( CHECK_MAIN_POWER *PowerChecker, DET_DC_VOLT *pDetectDc, POWER_STATUS *PowSts ) /* <S0F7> */
LONG	PcmCheckOverVoltage( CHECK_MAIN_POWER *PowerChecker, DET_DC_VOLT *pDetectDc,
														POWER_STATUS *PowSts, REGENE_CTRL *RegCtrl ) /* <S0F7> */
{
	LONG		ret;
	USHORT		OvLevel; /* <S0F7> */

	/* ローカル変数の初期化 */
	ret = DETECT_DC_NO_OPERATION;

/*--------------------------------------------------------------------------------------------------*/
/*		回生使用状況チェック														<S0F7>			*/
/*--------------------------------------------------------------------------------------------------*/
	if( (RegCtrl->conf.ResistorCapa != 0) || (RegCtrl->conf.RegRxIn) )
	{ /* 回生使用あり */
		OvLevel = PowerChecker->conf.OvLevel;
	}
	else
	{ /* 回生不使用 */
		OvLevel = PowerChecker->conf.NonRegOvLevel;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		ＤＣ電源過電圧チェック																		*/
/*--------------------------------------------------------------------------------------------------*/
//	if( PowSts->DcVolt > (LONG)PowerChecker->conf.OvLevel )	/* DC電圧がOVレベルを超えた場合			*//* <S0F7> */
	if( PowSts->DcVolt > (LONG)OvLevel )					/* DC電圧がOVレベルを超えた場合			*/
	{
		if( pDetectDc->DcVoltx == pDetectDc->conf.VdetMaxVolt )
		{
			if( pDetectDc->VdcdetAlmCnt > pDetectDc->conf.VdcdetAlmFilter )
			{
//				KPI_BASE_BLOCK( );							/* ＡＳＩＣベースブロック				*/
//				KPI_MPCMD_SVOFF( );							/* マイクロプログラムサーボＯＦＦ		*/
				ALMSetGlobalAlarm( ALM_VDC );				/* A.030 : 主回路検出部異常				*/
				ret = ALM_VDC;								/* A.030 : 主回路検出部異常				*/
			}
			else
			{
				pDetectDc->VdcdetAlmCnt++;					/* Increment Error Counter				*/
			}
		}
		/* 2010.06.08 Y.Oka マスクチェックは外で実施するよう変更 */
//		else if( !KPI_ALARMMSKCHK( ALM_OV ) )				/* アラームマスクチェック				*/
		else
		{
			if( pDetectDc->OvdetAlmCnt > PowerChecker->conf.OvdetAlmFilter )
			{
//				KPI_BASE_BLOCK( );							/* ＡＳＩＣベースブロック				*/
//				KPI_MPCMD_SVOFF( );							/* マイクロプログラムサーボＯＦＦ		*/
				ALMSetGlobalAlarm( ALM_OV );				/* A.400 : 過電圧(ＯＶ)					*/
				ret = ALM_OV;								/* A.400 : 主回路検出部異常				*/
			}
			else
			{
				pDetectDc->OvdetAlmCnt++;					/* Increment Error Counter				*/
			}
		}
	}
	else
	{
		pDetectDc->VdcdetAlmCnt = 0;						/* Reset Error Conter					*/
		pDetectDc->OvdetAlmCnt = 0;							/* Reset Error Conter					*/
	}

	return ret;
}



/****************************************************************************************************/
/*																									*/
/*		DC電源電圧検出処理 ( 実行周期 : 250us ) 													*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機能　: A/D変換された主回路検出電圧を検出する													*/
/*																									*/
/*	改版　:	2007.02.07	K.Ando		<V134>															*/
/*			ＡＤ読出し値に三重根フィルタを追加														*/
/*																									*/
/****************************************************************************************************/
static	void	LpxAdDetectDcVolt( DET_DC_VOLT *pDetectDc )
{
	LONG	AdRead;
	LONG	DcVoltx;

/*--------------------------------------------------------------------------------------------------*/
/*		ＤＣ電圧ＡＤ読込み																			*/
/*--------------------------------------------------------------------------------------------------*/
	AdRead = HALdrv_GetDcVoltFromAd( );						/* ＤＣ電圧ＡＤ読込み					*/
	DcVoltx = MlibMulgain( AdRead, pDetectDc->conf.VdetAdNorm );

/*--------------------------------------------------------------------------------------------------*/
/*		ＤＣ電圧のクランプ処理																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( DcVoltx < 0 )
	{
		DcVoltx = 0;
	}
	else if( DcVoltx > pDetectDc->conf.VdetMaxVolt )
	{
		DcVoltx = pDetectDc->conf.VdetMaxVolt;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		ＤＣ電圧のフィルタ処理																		*/
/*--------------------------------------------------------------------------------------------------*/
	pDetectDc->DcAdFili = DcVoltx;
	pDetectDc->DcAdFilo[0] =
		MlibLpfilter1( DcVoltx, pDetectDc->conf.VdetAdFil, pDetectDc->DcAdFilo[0] );
	pDetectDc->DcAdFilo[1] =
		MlibLpfilter1( pDetectDc->DcAdFilo[0], pDetectDc->conf.VdetAdFil, pDetectDc->DcAdFilo[1] );
	pDetectDc->DcAdFilo[2] =
		MlibLpfilter1( pDetectDc->DcAdFilo[1], pDetectDc->conf.VdetAdFil, pDetectDc->DcAdFilo[2] );
	pDetectDc->DcVoltx = pDetectDc->DcAdFilo[2];

	return;
}



/****************************************************************************************************/
/*																									*/
/*		回生過負荷チェック処理 (Call from ScanC)													*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 回生トランジスタのオン時間より、回生抵抗の消費電力を推定し、							*/
/*			過負荷レベルを越えた場合、回生過負荷アラームとする。									*/
/*			また、１０秒間の平均回生電力値を計算する。												*/
/*																									*/
/*	戻り値:																							*/
/*			CHK_REGOL_NO_OPERATION	: 回生過負荷なし(処理不要)										*/
/*			CHK_REGOL_SET_ALARM		: 全軸アラーム出力		A.320 : 回生過負荷						*/
/*			CHK_REGOL_SET_WARNING	: 全軸ワーニング出力	A.920 : 回生過負荷ワーニング			*/
/*			CHK_REGOL_CLR_WARNING	: 全軸ワーニングクリア	A.920 : 回生過負荷ワーニング			*/
/*																									*/
/****************************************************************************************************/
LONG	PcmCheckRegeneOverLoad( REGENE_CTRL *pRegCtrl,ULONG *AlmOLInfo, ANLG_UN_MON_DATA *MonData )
{
	LONG	RegPower;
	LONG	LastRgolPowSum;
	LONG	ret;

	/* ローカル変数の初期化 */
	ret = CHK_REGOL_NO_OPERATION;

/*--------------------------------------------------------------------------------------------------*/
/*		回生電力の計算																				*/
/*--------------------------------------------------------------------------------------------------*/
	if( (pRegCtrl->conf.ResistorCapa == 0) && (pRegCtrl->conf.RegRxIn == FALSE) )
	{
		RegPower = 0;
	}
	else
	{
#if (FLOAT_USE==TRUE)
		RegPower = pRegCtrl->dRegOnCount * pRegCtrl->RegOL.conf.Krgpower;
#else
		RegPower = MlibMulgain30( pRegCtrl->dRegOnCount, pRegCtrl->RegOL.conf.Krgpower );
#endif /* FLOAT_USE */
	}

/*--------------------------------------------------------------------------------------------------*/
/*		回生過負荷のチェック																		*/
/*--------------------------------------------------------------------------------------------------*/
	/* 2010.09.01 Y.Oka 呼び出し元で確認するよう変更 */
//	if( (TestMode2 == FALSE) && (prm_ptr->iprm_ptr->Dcbus == FALSE) )
	{
		/*	前回の回生電力積算値保存 */
		LastRgolPowSum = pRegCtrl->RegOL.var.RgolPowSum;
		pRegCtrl->RegOL.var.RgolPowSum += (RegPower - pRegCtrl->RegOL.conf.RgolPowBase);
		if( pRegCtrl->RegOL.var.RgolPowSum < 0 )
		{
			pRegCtrl->RegOL.var.RgolPowSum = 0;
		}
	/*----------------------------------------------------------------------------------------------*/
		/* 2010.11.16 Y.Oka Pn600 = 0設定対策 */
//		if( pRegCtrl->RegOL.var.RgolPowSum >= pRegCtrl->RegOL.conf.RgolAlmLevel )
		if( pRegCtrl->RegOL.var.RgolPowSum > pRegCtrl->RegOL.conf.RgolAlmLevel )
		{
//			KPI_ALARMSET( ALM_RGOL );					/* A.320 : 回生過負荷 						*/
			ret |= CHK_REGOL_SET_ALARM;
			pRegCtrl->RegOL.var.LastRgolDetect = FALSE;	/* 前回起動時の回生過負荷検出フラグオフ		*/
		}
		/* 2010.11.16 Y.Oka Pn600 = 0設定対策 */
//		if( pRegCtrl->RegOL.var.RgolPowSum >= (pRegCtrl->RegOL.conf.RgolAlmLevel>>1) )
		if( pRegCtrl->RegOL.var.RgolPowSum > (pRegCtrl->RegOL.conf.RgolAlmLevel>>1) )
		{
//			KPI_WARNINGSET( WRN_RGOLF );				/* A.920 : 回生過負荷ワーニング				*/
//			ret = CHK_REGOL_SET_WARNING;
			if( !pRegCtrl->RegOL.var.LastRgolDetect || pRegCtrl->RegOL.var.RgPowUp )
			{ /* 前回検出なし || 電力増加有り */
				ret |= CHK_REGOL_SET_WARNING;			/* A.920 : 回生過負荷ワーニング				*/
			}
			else if( LastRgolPowSum < pRegCtrl->RegOL.var.RgolPowSum )
			{ /* 前回回生電力から増加 */
				ret |= CHK_REGOL_SET_WARNING;			/* A.920 : 回生過負荷ワーニング				*/
				pRegCtrl->RegOL.var.RgPowUp = TRUE;		/* 回生電力増加フラグオン					*/
														/* RgPowUpはTRUE後、前回検出有無に関わらず、*/
														/* ワーニングのため、FALSEは不要			*/
			}
			else
			{ /* 前回検出有りで電力増加がない場合はワーニング出力しない */
				ret |= CHK_REGOL_CLR_WARNING;			/* A.920 : 回生過負荷ワーニング				*/
			}
		}
		else
		{
			if( pRegCtrl->RegOL.var.LastRgolDetect )
			{ /* 前回起動時の回生過負荷検出フラグオン */
				pRegCtrl->RegOL.var.LastRgolDetect = FALSE;	/* 前回起動時の回生過負荷検出フラグオフ	*/
				*AlmOLInfo &= ~ALMDEF_OLINFO_RGOL;			/* 回生過負荷情報リセット				*/
			}
//			KPI_WARNINGCLR( WRN_RGOLF );				/* A.920 : 回生過負荷ワーニング				*/
			ret |= CHK_REGOL_CLR_WARNING;				/* A.920 : 回生過負荷ワーニング				*/
		}
	}

/*--------------------------------------------------------------------------------------------------*/
/*		Un00A : 回生電力用データ	(10sec回生電力平均値)	CoutV.UnMeanRgPower						*/
/*		Un143 : 回生電力用データ	(2ms回生電力値)			CoutV.UnRgPower							*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	MonData->UnRgPower = RegPower * pRegCtrl->RegOL.conf.KunrgpowerCycle;
#else
	MonData->UnRgPower = MlibMulgain( RegPower, pRegCtrl->RegOL.conf.KunrgpowerCycle );
#endif /* FLOAT_USE */
	MonData->TrcRgPower = MonData->UnRgPower;
	pRegCtrl->RegOL.var.RgPowerSum += RegPower;

	pRegCtrl->RegOL.var.RgMeanCycle++;
	if( pRegCtrl->RegOL.var.RgMeanCycle >= KPI_SCANC_MS( 10000 ) )
	{
#if 0
#if (FLOAT_USE==TRUE)
		MonData->UnMeanRgPower = pRegCtrl->RegOL.var.RgPowerSum * pRegCtrl->RegOL.conf.Kunrgpower;
#else
		MonData->UnMeanRgPower = MlibMulgain( pRegCtrl->RegOL.var.RgPowerSum, pRegCtrl->RegOL.conf.Kunrgpower );
#endif /* FLOAT_USE */
#endif
/*<S123>--->*/
#if (FLOAT_USE==TRUE)
		pRegCtrl->UnMeanRgPower = pRegCtrl->RegOL.var.RgPowerSum * pRegCtrl->RegOL.conf.Kunrgpower;
#else
		pRegCtrl->UnMeanRgPower = MlibMulgain( pRegCtrl->RegOL.var.RgPowerSum, pRegCtrl->RegOL.conf.Kunrgpower );
#endif
/*<S123><---*/
		pRegCtrl->RegOL.var.RgPowerSum  = 0;
		pRegCtrl->RegOL.var.RgMeanCycle = 0;
	}

	return ret;
}


#if 0
/****************************************************************************************************/
/*																									*/
/*		回生過負荷異常アラーム／ワーニング設定処理 (Call from ScanC)								*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : LpxCheckRegeneOverLoad()にてチェックした回生過負荷状態に応じて、						*/
/*			回生過負荷アラームセット、回生過負荷ワーニングセット、回生過負荷ワーニングクリア		*/
/*			処理を実施する。																		*/
/*																									*/
/****************************************************************************************************/
void	SetRegeneOverLoadSts( LONG RegOlSts )
{
	if( RegOlSts & CHK_REGOL_SET_ALARM )
	{
		/* A.320 : 回生過負荷 */
		ALMSetGlobalAlarm( ALM_RGOL );
	}
	else if( RegOlSts & CHK_REGOL_SET_WARNING )
	{
		/* A.920 : 回生過負荷ワーニング */
		ALMSetGlobalAlarm( WRN_RGOLF );
	}
	else
	{
		/* A.920 : 回生過負荷ワーニング */
		ALMClearGlobalAlarm( WRN_RGOLF );
	}
	return;
}
#endif


/****************************************************************************************************/
/*																									*/
/*		突入抵抗過負荷チェック処理 (Call from ScanC)												*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 主回路電源のオン／オフの頻度のチェックを行い、											*/
/*			２分間に１０回オン／オフされた場合、突入抵抗過負荷アラームとする。						*/
/*																									*/
/*			２分間毎の単純チェックでは、１８回／２分間を見逃す可能性があるので、					*/
/*			半周期ずらした２段チェックとする。														*/
/*			これでも完全ではないが、まじめに処理すると大変である。									*/
/*																									*/
/*						   2min 																	*/
/*			Cnt[0] :  |<----------->|<----------->|<----------->|<----------->|<------ 				*/
/*			Cnt[1] :  ------>|<----------->|<----------->|<----------->|<----------->|				*/
/*								  2min  															*/
/*																									*/
/*	戻り値:																							*/
/*			CHK_RUSH_NO_OPERATION	: 突入抵抗過負荷なし(処理不要)									*/
/*			CHK_RUSH_SET_ALARM		: 全軸アラーム出力	A.740 : 突入抵抗過負荷						*/
/*																									*/
/****************************************************************************************************/
BOOL	PcmCheckRushRxOverLoad( CHECK_RUSH *pRush, POWER_STATUS *pPowerSts )
{
	BOOL	ret;

	/* ローカル変数の初期化 */
	ret = CHK_RUSH_NO_OPERATION;

/*--------------------------------------------------------------------------------------------------*/
/*		パワーＯＦＦ==>ＯＮ検出																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( pPowerSts->PowerOn == TRUE )
	{
		if( pRush->PowerOnChk == FALSE )
		{
			pRush->PowerOnCnt[0]++;						/* Count up Power Off ==> On				*/
			pRush->PowerOnCnt[1]++;						/* Count up Power Off ==> On				*/
		}
		pRush->PowerOnChk = TRUE;
	}
	else
	{
		pRush->PowerOnChk = FALSE;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		突入抵抗過負荷チェック																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( (pRush->PowerOnCnt[0] >= 10) || (pRush->PowerOnCnt[1] >= 10) )
	{
		/* A.740 : 突入抵抗過負荷 */
//		KPI_ALARMSET( ALM_RSOL );
		ret = CHK_RUSH_SET_ALARM;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		検出サイクル制御＆カウンタクリア処理														*/
/*--------------------------------------------------------------------------------------------------*/
	pRush->PowerOnChkCycle++;
	if( pRush->PowerOnChkCycle >= KPI_SCANC_MS( 2*60*1000 ) )
	{
		pRush->PowerOnCnt[0] = 0;
		pRush->PowerOnChkCycle = 0;
	}
	else if( pRush->PowerOnChkCycle == KPI_SCANC_MS( 1*60*1000 ) )
	{
		pRush->PowerOnCnt[1] = 0;
	}

	return ret;
}



/****************************************************************************************************/
/*																									*/
/*		回生異常チェック処理 (Call from ScanC)														*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 回生異常検出は回生Ｔｒの短絡故障を検出するものである。									*/
/*			パワーレディがアクティブの時の回生Ｔｒのコレクタ(ＩＧＢＴのドレイン)の電圧を			*/
/*			コンパレータで監視し、一定時間Ｌｏ状態が続いた場合、異常とみなす。						*/
/*			正常な回生動作時は短い周期でのオン／オフを繰り返すが、長時間オンが継続するこ			*/
/*			とはない。																				*/
/*																									*/
/*			1) 異常とみなす時間は４００ｍｓとする。これは、回生Ｔｒが短絡故障した場合、				*/
/*			   回生抵抗が焼損しない程度の時間である。												*/
/*			2) 回生抵抗を内蔵しないタイプがある。これはシステム定数により区別し、検出を				*/
/*			   パスする。																			*/
/*			3) Σ-Ⅱでは、ＤＣ電源入力モード時見ていなかったが、回生回路は内蔵しており、			*/
/*			   回生Ｔｒが短絡故障した場合焼損の危険があるので検出するようにする。					*/
/*			4) 従来は、回生抵抗の断線の検出も兼ねていたが、容量により回生抵抗が無いもの、			*/
/*			   回生抵抗外付けするものなどあり、断線検出するしないの判断が要って複雑にな				*/
/*			   るので、回生抵抗断線は回生過負荷検出、過電圧検出で行うものとする。					*/
/*			5) 回生ＴｒコレクタをＰラインに高抵抗でつないで、回生抵抗断線時もＨｉレベル				*/
/*			   に保つようにする。																	*/
/*																									*/
/*	戻り値 :																						*/
/*			CHK_REGENE_NO_OPERATION	: 回生異常なし(処理不要)										*/
/*			CHK_REGENE_SET_ALARM	: 全軸アラーム出力	A.300 : 回生異常							*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	2002.01.17 : ＤＣ入力モードおよび６KW以上でアラーム検出処理をしない(SGDH)対応					*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
BOOL	PcmCheckRegeneAlarm( REGENE_CTRL *pRegCtrl, BOOL PowerOn )
{
	BOOL	ret;

	/* ローカル変数の初期化 */
	ret = CHK_REGENE_NO_OPERATION;

/*--------------------------------------------------------------------------------------------------*/
/*		回生異常チェック処理																		*/
/*--------------------------------------------------------------------------------------------------*/
#if 0
	if( (TestMode1 == FALSE)
		&&	((pRegCtrl->conf.RegRxIn == TRUE) || (pRegCtrl->conf.ResistorCapa != 0))
		/* 2010.09.01 Y.Oka 電源入力モードは呼び出し元で確認。容量には依存しないよう変更。 */
//		&&	((iprm_ptr->Dcbus == FALSE ) || (uni_prm_ptr->pprm_ptr->ampw < 6000))
		&&	(PowerOn == TRUE) )
//　テストモード時は呼び出し先でPowerOn=FALSEとするなどして対処
#endif
	if( ((pRegCtrl->conf.RegRxIn == TRUE) || (pRegCtrl->conf.ResistorCapa != 0))
		&&	(PowerOn == TRUE) )
	{
		/*	(回生異常信号オン && 回生Tr指令オフ) || (回生異常信号オフ && 回生Tr指令オン) */
//		if( ((pRegCtrl->RegAlmSig == TRUE ) && ((pAsicHwReg->AREG_IOSTAT1 & 0x0002) != 0)) ||
//			((pRegCtrl->RegAlmSig == FALSE) && ((pAsicHwReg->AREG_IOSTAT1 & 0x0002) == 0)) )
		if( ((pRegCtrl->RegAlmSig == TRUE ) && (FALSE == HALdrv_GetRegeneControlState())) ||
			((pRegCtrl->RegAlmSig == FALSE) && (FALSE != HALdrv_GetRegeneControlState())) )
		{
			if( pRegCtrl->RegChk.RegeneErrCnt <= KPI_SCANC_MS( 400 ) )
			{
				pRegCtrl->RegChk.RegeneErrCnt++;
			}
			else
			{
				/* A.300 : 回生異常 */
//				KPI_ALARMSET( ALM_RG );
				ret = CHK_REGENE_SET_ALARM;
			}
			pRegCtrl->RegChk.RegeAlmSignal = TRUE;
		}
		else
		{
			pRegCtrl->RegChk.RegeneErrCnt = 0;
			pRegCtrl->RegChk.RegeAlmSignal = FALSE;
		}
	}
	else
	{
		pRegCtrl->RegChk.RegeneErrCnt = 0;
		pRegCtrl->RegChk.RegeAlmSignal = FALSE;
	}

	return ret;
}



/****************************************************************************************************/
/*																									*/
/*		サーボパック内蔵ファン停止チェック処理		<S0A3>											*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : サーボパックに内蔵されているファンの停止を監視する。									*/
/*			直接パワー制御には関係しないが、ユニット共通処理のため、PCMモジュールの一部とする。		*/
/*																									*/
/*	改版　:																							*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#define FANLOCK_START_TIME	KPI_SCANC_MS( 10000 )	/* ファン停止確認を開始するまでの時間(10s)		*/
/*--------------------------------------------------------------------------------------------------*/
LONG	PcmCheckFanLock( CHECK_FANLOCK *FanLock, BOOL FanStop, BOOL BeSts )
{
	LONG	ret;
	ULONG	CumulativeOperationTime;	/* 累積稼働時間[100ms](ファン停止ワーニング発生後) */	/* <S219> */

	ret = CHK_FANLOCK_NO_OPERATION;

	if( FanLock->var.FanLockStartCnt >= FANLOCK_START_TIME )
	{
		if( FanStop == TRUE )
		{
			if( FanLock->var.FanLockTimeCnt >= FanLock->conf.FanLockDetectTime )
			{
				if( BeSts == TRUE )
				{ /* SVON中の軸がある場合 */
					ret = CHK_FANLOCK_SET_WARNING;			/* A.923 : ファン停止ワーニング			*/

					/* <S219> Start */
					/* ファン停止ワーニング検出 1スキャン目の場合 */
					if( FanLock->var.FanLockWrnFlag == FALSE )
					{
						/* 現在累積稼働時間取得(ファン停止ワーニング発生時) */
						FanLock->var.WrnStartTime = KlibGetTimeStamp();

						/* ファン停止ワーニング発生フラグ = TRUE */
						FanLock->var.FanLockWrnFlag = TRUE;
					}

					/* 現在累積稼働時間取得(100[ms]単位) */
					CumulativeOperationTime = KlibGetTimeStamp();

					/* 現在累積稼働時間 - ファン停止ワーニング発生時の累積稼働時間取得 > PnEE6:サーボオン時内蔵ファン停止アラーム検出時間 */
					if( ( CumulativeOperationTime - FanLock->var.WrnStartTime) > FanLock->conf.FanLockAlmDetectTime )
					{
						ret = CHK_FANLOCK_SET_ALARM;			/* A.7AB : ファン停止アラーム			*/

						/* ファン停止ワーニング発生フラグ = FALSE */
						FanLock->var.FanLockWrnFlag = FALSE;
					}
					/* <S219> End */
				}
				else
				{ /* 全軸SVOFFの場合 */
					ret = CHK_FANLOCK_SET_ALARM;			/* A.7AB : ファン停止アラーム			*/

					/* ファン停止ワーニング発生フラグ = FALSE */	/* <S219> */
					FanLock->var.FanLockWrnFlag = FALSE;			/* <S219> */
				}
			}
			else
			{
				/* ファン停止時間カウント */
				FanLock->var.FanLockTimeCnt++;

				/* ファン停止ワーニング発生フラグ = FALSE */	/* <S219> */
				FanLock->var.FanLockWrnFlag = FALSE;			/* <S219> */
			}
		}
		else
		{
			/* ファン停止時間カウントクリア */
			FanLock->var.FanLockTimeCnt = 0;

			/* ファン停止ワーニング発生フラグ = FALSE */	/* <S219> */
			FanLock->var.FanLockWrnFlag = FALSE;			/* <S219> */
		}
	}
	else
	{
		/* 電源投入後、FANLOCK_START_TIME経過するまでファン停止確認しない */
		FanLock->var.FanLockStartCnt++;

		/* ファン停止ワーニング発生フラグ = FALSE */	/* <S219> */
		FanLock->var.FanLockWrnFlag = FALSE;			/* <S219> */

	}

	return	ret;
}



/****************************************************************************************************/
/*																									*/
/*		Initialize Unit Alarm Check Variables														*/
/*																									*/
/****************************************************************************************************/
void	PcmInitPowerManager( POW_MNG_HNDL *hPowerManager, ULONG AlmOLInfo )
{
	CHECK_MAIN_POWER	*pMainPowerChecker;
	CHECK_REGOL			*pRegOL;

	/* ローカル変数の初期化 */
	pMainPowerChecker = &(hPowerManager->MainPowerChecker);
	pRegOL = &(hPowerManager->RegeneCtrl.RegOL);

/*--------------------------------------------------------------------------------------------------*/
/*		主回路電源監視用変数初期化																	*/
/*--------------------------------------------------------------------------------------------------*/
	/* Reset Variable Data */
	MlibResetLongMemory( &(pMainPowerChecker->var), sizeof(pMainPowerChecker->var)/4 );

/*--------------------------------------------------------------------------------------------------*/
/*		回生過負荷検出用変数初期化																	*/
/*--------------------------------------------------------------------------------------------------*/
	/* Reset Variable Data */
	MlibResetLongMemory( &(pRegOL->var), sizeof(pRegOL->var)/4 );

	if( AlmOLInfo & ALMDEF_OLINFO_RGOL )
	{ /* 回生過負荷アラーム情報ありの場合 */
		/* 前回起動時の回生過負荷検出フラグオン */
		pRegOL->var.LastRgolDetect = TRUE;

		switch( pRegOL->conf.RgSumSel )
		{
		case REGPOWSUM_RATE95PER:					/*	回生過負荷アラームレベルの95％を設定		*/
			pRegOL->var.RgolPowSum = ( pRegOL->conf.RgolAlmLevel / 100 ) * 95;
			break;
		case REGPOWSUM_RATE50PER:					/*	回生過負荷アラームレベルの50％を設定		*/
			pRegOL->var.RgolPowSum = ( pRegOL->conf.RgolAlmLevel >> 1 );
			break;
		default:									/*	回生過負荷アラームレベルの50％を設定		*/
			pRegOL->var.RgolPowSum = ( pRegOL->conf.RgolAlmLevel >> 1 );
			break;
		}
	}

/*--------------------------------------------------------------------------------------------------*/
/*		突入防止抵抗過負荷検出用変数初期化															*/
/*--------------------------------------------------------------------------------------------------*/
	/* Reset Variable Data */
	MlibResetLongMemory( &(hPowerManager->Rush), sizeof(hPowerManager->Rush)/4 );

/*--------------------------------------------------------------------------------------------------*/
/*		回生異常検出用変数初期化																	*/
/*--------------------------------------------------------------------------------------------------*/
	/* Reset Variable Data */
	MlibResetLongMemory( &(hPowerManager->RegeneCtrl.RegChk), sizeof(hPowerManager->RegeneCtrl.RegChk)/4 );

	return;
}



/****************************************************************************************************/
/*																									*/
/*		Count Regene Transistor ON Time																*/
/*																									*/
/****************************************************************************************************/
void	PcmCountRegeneOnTime( POW_MNG_HNDL *hPowerManager )
{
	LONG	lwk0;

/*--------------------------------------------------------------------------------------------------*/
/*		Delta Regene.Tr On Count for ScanC Alarm Check												*/
/*--------------------------------------------------------------------------------------------------*/
	/* Current Regene ON Count */
	lwk0 = hPowerManager->RegeneCtrl.RegOnCount[0];
	/* Delta Regene ON Count */
	hPowerManager->RegeneCtrl.dRegOnCount = lwk0 - hPowerManager->RegeneCtrl.RegOnCount[1];
	/* Update Last Regene ON Count */
	hPowerManager->RegeneCtrl.RegOnCount[1] = lwk0;

	return;
}



/****************************************************************************************************/
/*																									*/
/*		Count Regene Transistor ON Time																*/
/*																									*/
/****************************************************************************************************/
void	PcmInputPowerStatus( POW_MNG_HNDL *hPowerManager, BOOL Acon1, BOOL RegeneAlm )
{
	hPowerManager->PowerSts.Acon1Sig_l = hPowerManager->PowerSts.Acon1Sig;
	hPowerManager->PowerSts.Acon1Sig = Acon1;
//	hPowerManager->PowerSts.Acon2Sig = Acon2;
	hPowerManager->RegeneCtrl.RegAlmSig = RegeneAlm;

	return;
}



/****************************************************************************************************/
/*																									*/
/*		回生電力演算ゲインの計算																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 回生電力演算ゲインの計算を行う。														*/
/*			回生電力は、[5000/コンバータ容量]の単位で、ScanC間の平均値で表現する。					*/
/*			ScanB周期で回生トランジスタのON/OFFをカウントするので、回生電力は、						*/
/*			下式となる。																			*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*	SGD7*,INGRAM																					*/
/*						 (DcVolt)^2  * SBCYCLEUS * 5000  											*/
/*			回生電力 = ----------------------------------- * dRegOnCount							*/
/*						(reger/1000) * SCCYCLEUS * ConvW  											*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*	SGDV																							*/
/*						 (DcVolt)^2  * SBCYCLEUS * 5000  											*/
/*			回生電力 = ----------------------------------- * dRegOnCount							*/
/*						(reger/1000) * SCCYCLEUS * ampw  											*/
/*--------------------------------------------------------------------------------------------------*/
/*			Pn600 : Prm.regres			回生抵抗容量				[10W]							*/
/*			Pn603 : Prm.exreger			回生抵抗値					[10mΩ]							*/
/*			PnE1A : Prm.reger			回生抵抗値(デフォルト)		[mΩ]							*/
/*			PnE1C : Prm.wre				回生抵抗容量(デフォルト)	[0.1%]							*/
/*			PnE72 : Prm.regonlvl		回生オンレベル				[V]								*/
/*			PnE7F : Prm.ConvW			コンバータ容量				[W]								*/
/****************************************************************************************************/
void	IprmcalRegenePowerGain( REGENE_CTRL *RegeneCtrl, BPRMDAT *Bprm, REGCFG_PRM *RegPrm )
{
	LONG		kx, sx;
	LONG		Vdcsqr;
	LONG		reger;		/* 回生抵抗値 */

/*--------------------------------------------------------------------------------------------------*/
/*		電圧の計算																					*/
/*--------------------------------------------------------------------------------------------------*/
	if( Bprm->AmpDcinType == 1 )							/* アンプＤＣ電源入力				*/
	{
		Vdcsqr = 0;												/* 回生なし							*/
	}
	else														/* アンプＤＣ電源入力				*/
	{
		/* Vdcsqr = (PnE72 ^ 2) */
		Vdcsqr = RegPrm->regonlvl * RegPrm->regonlvl;
		if( Bprm->AmpInVcode == 0x02 )
		{														/* 400V入力以外：回生抵抗値[mΩ]	*/
			Vdcsqr /= 10;										/* 400V入力	   ：回生抵抗値[10mΩ]	*/
		}
	}

/*--------------------------------------------------------------------------------------------------*/
/*		回生抵抗値の選択															<S0F4>			*/
/*--------------------------------------------------------------------------------------------------*/
	if( RegPrm->regres != 0 )
	{ /* Pn600外付け回生抵抗容量設定済み */
		if( RegPrm->exreger != 0 )
		{ /* Pn603外付け回生抵抗値設定済み */
			/* 外付け回生抵抗値を使用 */
			if( Bprm->AmpInVcode == 0x02 )
			{ /* 400V入力の場合 */
				reger = RegPrm->exreger;						/* 単位変換不要[10mΩ]				*/
			}
			else
			{ /* 400V入力以外の場合 */
				reger = RegPrm->exreger * 10U;					/* 単位変換[10mΩ]→ [mΩ]			*/
			}
		}
		else
		{
			/* デフォルト回生抵抗値を使用 */
			reger = RegPrm->reger;
		}
	}
	else
	{
		/* デフォルト回生抵抗値を使用 */
		reger = RegPrm->reger;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		回生電力演算ゲインの計算																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( Bprm->SvCycleNs != KPI_SBCYCLE_BASE_NS )
	{ /* ScanB周期 125us以外 */
		kx = MlibScalKxgain( Vdcsqr, Bprm->SvCycleUs/2, (KPI_SCCYCLEUS * reger), &sx, 0 );
	}
	else
	{ /* ScanB周期 125us */
		kx = MlibScalKxgain( Vdcsqr, Bprm->SvCycleUs, (KPI_SCCYCLEUS * reger), &sx, 0 );
	}

	kx = MlibScalKxgain( Vdcsqr, Bprm->SvCycleUs, (KPI_SCCYCLEUS * reger), &sx, 0 );
	kx = MlibPcalKxgain( kx, (1000 * 5000), RegPrm->ConvW, &sx, 24 );
	RegeneCtrl->RegOL.conf.Krgpower = kx;
}




/****************************************************************************************************/
/*																									*/
/*		回生過負荷レベルの計算																		*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 回生過負荷チェック用パラメータの計算を行う。											*/
/*																									*/
/*			1) 回生過負荷許容電力																	*/
/*			2) 回生過負荷アラームレベル																*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
BOOL	PcalRegeneOverLoadLevel( REGENE_CTRL *RegeneCtrl,USHORT regres, USHORT wre, USHORT ConvW )
{
	BOOL			PrmSetErr;
	LONG			kx, sx;
	CHECK_REGOL		*pRegOL;
//	CMN_PRMDATA		*Cprm;

	/* ローカル変数の初期化 */
	PrmSetErr = FALSE;
	pRegOL = &(RegeneCtrl->RegOL);

	/* 2010.09.02 Y.Oka (>_<) 現状コンバータ容量を知るすべがないため、固定値を代入 */
//	Cprm->ConvW = 8000;

/*--------------------------------------------------------------------------------------------------*/
/*		回生過負荷許容電力の計算																	*/
/*--------------------------------------------------------------------------------------------------*/
/*	Σ-7,INGRAM																						*/
/*		RgolPowBase = 回生抵抗容量 * 5000 / ConvW 		[5000/コンバータ容量]						*/
/*											(Σ-7,INGRAMに接続されたコンバータの容量値)				*/
/*--------------------------------------------------------------------------------------------------*/
/*	SGDV																							*/
/*		RgolPowBase = 回生抵抗容量 * 5000 / AmpW 		[5000/サーボ定格容量]						*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
	if( regres == 0 )									/* 設定値ゼロの場合(Pn600)					*/
	{
		pRegOL->conf.RgolPowBase = wre;					/* 回生抵抗容量(デフォルト)					*/
	}
//	else if( (Prm->regres * 10) <= Prm->ampw )			/* サーボ定格出力以内の場合					*/
	else if( (regres * 10) <= ConvW )					/* コンバータ容量以内の場合					*/
	{
//		pRegOL->conf.RgolPowBase = (Prm->regres * 50000) / Prm->ampw;
		pRegOL->conf.RgolPowBase = (regres * 50000) / ConvW;
	}
	else												/* コンバータ出力以上は設定できない			*/
	{
		/* パラメータ設定異常 */
		PrmSetErr = TRUE;
	}


/*--------------------------------------------------------------------------------------------------*/
/*		回生過負荷アラームレベルの計算																*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		SWPF :	RgolAlmLevel = RgolPowBase * 2 * 100 sec											*/
/*							 = RgolPowBase * 2 * 100 * 1000 / SCCYCLEMS								*/
/*																									*/
/*				if( RgolPowSum >= RgolAlmLevel ){ 回生過負荷アラーム;}								*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		ΣⅡ :	rgollv = rgbas * 2 * 100 sec														*/
/*					   = rgbas * 2 * 100 * 10^6 / (CYCLE*FORCNTLMT)									*/
/*					   = 2^14 * (rgbas * 12207) / (CYCLE*FORCNTLMT);								*/
/*																									*/
/*				if( rgolsm>>14 >= rgollv ){ 回生過負荷アラーム;}									*/
/*																									*/
/*				*1) 初期設定 : rgolsm = rgollv << 7;   --- 正しいの？, 50%から開始では？			*/
/*																									*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
	kx = MlibScalKxgain( pRegOL->conf.RgolPowBase, (2*100*1000), KPI_SCCYCLEMS, NULL, -30 );
	pRegOL->conf.RgolAlmLevel = kx;

/*--------------------------------------------------------------------------------------------------*/
/*		回生消費電力演算ゲイン(for Un00A)の計算														*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*								100																	*/
/*		Kunrgpower = -------------------------------------											*/
/*					  RgolPowBase * KPI_SCANC_MS( 10000 )											*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
	kx = MlibScalKxgain( 100, 1, pRegOL->conf.RgolPowBase, &sx,  0 );
	kx = MlibPcalKxgain( kx,  1, KPI_SCANC_MS( 10000 ),    &sx, 24 );
	pRegOL->conf.Kunrgpower = kx;

/*--------------------------------------------------------------------------------------------------*/
/*		回生消費電力演算ゲイン(for Un143)の計算	10[%]												*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*								100							   1									*/
/*		Kunrgpower = -------------------------------------	* -----									*/
/*					  RgolPowBase * KPI_SCANC_MS( 2 )		   10									*/
/*																									*/
/*		オーバーフロー対策で１０[%]表示にするため、１０で除算する									*/
/*--------------------------------------------------------------------------------------------------*/
	kx = MlibScalKxgain( 10, 1, pRegOL->conf.RgolPowBase,   &sx,  24 );
	pRegOL->conf.KunrgpowerCycle = kx;

/*--------------------------------------------------------------------------------------------------*/
/*		回生抵抗容量取得																			*/
/*--------------------------------------------------------------------------------------------------*/
	RegeneCtrl->conf.ResistorCapa = regres;

	return PrmSetErr;
}


/****************************************************************************************************/
/*																									*/
/*		主回路検出ＡＤ用三重根ローパスフィルタ	フィルタ時定数パラメータ計算						*/
/*																									*/
/****************************************************************************************************/
void	PcmCalculateVdetAdFil( DET_DC_VOLT *pDetectDc, USHORT vdetadfil, LONG SvCycleUs )
{
	if( (SvCycleUs*1000) != KPI_SBCYCLE_BASE_NS )
	{ /* ScanB周期 125us以外 */
		pDetectDc->conf.VdetAdFil = MlibPcalKf1gain( vdetadfil, SvCycleUs, 0 );
	}
	else
	{ /* ScanB周期 125us */
		pDetectDc->conf.VdetAdFil = MlibPcalKf1gain( vdetadfil, (2 * SvCycleUs), 0 );
	}
	return;
}


/****************************************************************************************************/
/*																									*/
/*		突入電流防止回路制御／ＤＢ回路制御										<S20E>				*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 主回路充電シーケンス、および、ＤＢシーケンスからの要求を受け、突入／ＤＢ制御方法選択	*/
/*			に従って、リレー、サイリスタを制御する。												*/
/*																									*/
/*	変 更 : INGRAMでは突防制御とDB制御を分離し、突防制御はユニット共通処理としていた。Σ-7シリーズ	*/
/*			においてもΣ-7Wが多軸となるが、H/W的にインターロックを取っているため軸毎制御として良い。*/
/*			このため、Σ-Ⅴと同等の処理に変更する。													*/
/*																									*/
/*			TestMode5は試験機用機能であり、レジスタアクセスにより強制的に突防制御を行うため、		*/
/*			本処理での突防制御は行わない。同様にTestMode7は強制的にDB制御を行うための機能。			*/
/*																									*/
/*			モータレステストモード有効時は、強制的に突防OFF、DBON状態とする。						*/
/*																									*/
/****************************************************************************************************/
void	PcmRlyDbControlProcess( ASIC_HW_IF_STR *pAsicHwReg, POW_MNG_HNDL *PowerManager,
								BOOL BbSts, BOOL DbOn, BOOL KnlMode, BOOL MotorLessTestMode )
{
	CHECK_MAIN_POWER	*PowerChecker;
	POWER_STATUS		*pPowSts;
	BOOL				RelaySts;
	BOOL				DbSts;

	PowerChecker = &PowerManager->MainPowerChecker;
	pPowSts = &PowerManager->PowerSts;
	RelaySts = FALSE;
	DbSts = TRUE;

/*--------------------------------------------------------------------------------------------------*/
/*		サーボ突入回路制御／ＤＢ回路制御															*/
/*--------------------------------------------------------------------------------------------------*/
	switch( PowerChecker->conf.RlyDbWay )
	{
	case DBCTRL_RLY_COMMON:				/* 突入＋ＤＢ：リレー共通 									*/
		if( (BbSts == TRUE)
			&& ((pPowSts->ChargeOk == FALSE) || ((KnlMode != KPI_KMD_ADJCURAD) && (DbOn == TRUE))) )
		{ /* BB && (充電未完 || (!電流AD調整中 && DBON要求あり) ) */
			RelaySts = FALSE;			/* Relay OFF / DB ON */
		}
		else
		{
			RelaySts = TRUE;			/* Relay ON / DB OFF */
		}
		break;

	/*----------------------------------------------------------------------------------------------*/
	/*	下記パラメータ設定時はパラメータ計算でパラメータ設定異常とする								*/
	/*	■突入＋ＤＢ：リレー共通																	*/
	/*	■突入＋ＤＢ:リレー+サイリスタ																*/
	/*----------------------------------------------------------------------------------------------*/
	case DBCTRL_TRIACK_SEPARATE:		/* 突入：リレー；ＤＢ：サイリスタ 							*/
	case DBCTRL_RLY_SEPARATE:			/* 突入：リレー；ＤＢ：リレー 								*/
		/* 突防制御 */
		if( (BbSts == TRUE) && (pPowSts->ChargeOk == FALSE) )
		{ /* BB && 充電未完 */
			RelaySts = FALSE;			/* Relay OFF */
		}
		else
		{ /* BE || 充電完了 */
			RelaySts = TRUE;			/* Relay ON */
		}

		/* DB制御 */
		if( (BbSts == TRUE) && ( (KnlMode != KPI_KMD_ADJCURAD) && (DbOn == TRUE)) )
		{ /* BB && (!電流AD調整中 && DBON要求あり) */
			DbSts = TRUE;				/* DB ON */
		}
		else
		{
			DbSts = FALSE;				/* DB OFF */
		}
		break;

	/*----------------------------------------------------------------------------------------------*/
	/*	突入＋ＤＢ：サイリスタの場合、DBONでパルス出力を制御、RelayOnで突防 or DBをONする仕組み。	*/
	/*	■DBOFF				：パルス出力しないため、RelayOn/Offにかかわらず突防OFF/DBON				*/
	/*	■DBON && RelayOn	：パルス出力あり、突防ON/DBOFF											*/
	/*	■DBON && RelayOff	：パルス出力あり、突防OFF/DBON											*/
	/*----------------------------------------------------------------------------------------------*/
	case DBCTRL_TRIACK_COMMON:			/* 突入＋ＤＢ：サイリスタ 									*/
		if( !((BbSts == TRUE)
			  && ((pPowSts->ChargeOk == FALSE) || ((KnlMode != KPI_KMD_ADJCURAD) && (DbOn == TRUE)))) )
		{
			RelaySts = TRUE;			/* Relay ON */
			DbSts = TRUE;				/* Pulse Output */
		}
		else if( DbOn == TRUE )
		{
			RelaySts = FALSE;			/* Relay OFF */
			DbSts = TRUE;				/* Pulse Output */
		}
		else
		{
			RelaySts = FALSE;			/* Relay OFF */
			DbSts = FALSE;				/* Pulse Stop */
		}
		break;

	default:
		break;
	}

	/*----------------------------------------------------------------------------------------------*/
	/*	モータレステストモード有効時は、強制的に突防OFF、DBON状態とする。							*/
	/*----------------------------------------------------------------------------------------------*/
	if( MotorLessTestMode == TRUE )
	{ /* モータレス有効時 */
		RelaySts = FALSE;				/* Relay OFF */
		DbSts = TRUE;					/* DB ON */
	}

	/*----------------------------------------------------------------------------------------------*/
	/*	突防回路 & DB回路出力設定																	*/
	/*----------------------------------------------------------------------------------------------*/
	if( PowerChecker->conf.TestMode5 == FALSE )
	{
		if( ((PowerChecker->conf.RlyDbWay == DBCTRL_TRIACK_COMMON) || (PowerChecker->conf.RlyDbWay == DBCTRL_RLY_COMMON))
			&& (PowerChecker->conf.TestMode7 == TRUE) )
		{ /* 突入＋ＤＢ：リレー共通の場合、DB制御に突防制御信号を使用する */
			; /* DB強制出力中は何もしない */
		}
		else
		{
			HALdrv_RelayControl( (void*)pAsicHwReg, RelaySts );	/* Relay ON/OFF */
		}
	}

//	if( (PowerChecker->conf.TestMode7 == FALSE) && (CSW_HW_DB_CTRL == TRUE) )
	if((( PowerChecker->conf.TestMode7 == FALSE )&&( CSW_HW_DB_CTRL == TRUE ))&&
									( PowerChecker->conf.RlyDbWay != DBCTRL_RLY_COMMON ))	/* <S212> */
	{
		if( (PowerChecker->conf.RlyDbWay == DBCTRL_TRIACK_COMMON)
			&& (PowerChecker->conf.TestMode5 == TRUE) )
		{ /* 突入＋ＤＢ：サイリスタの場合、突防制御にDBON信号を使用する */
			; /* 突防制御強制出力中は何もしない */
		}
		else
		{
			if( DbSts == TRUE )
			{
				if( PowerChecker->conf.RlyDbWay == DBCTRL_TRIACK_COMMON )
				{
					KlibWaitus( WAIT_RLY_DB );
				}
				SHalDrv_DynamicBrakeON( pAsicHwReg );			/* DBON */
			}
			else
			{
				SHalDrv_DynamicBrakeOFF( pAsicHwReg );			/* DBOFF */
			}
		}
	}
	return;
}


/****************************************************************************************************/
/*																									*/
/*		中間電位異常チェック処理(A.450)	<S053>														*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : ４００Ｖ級サーボパックの主回路コンデンサの中間電位がずれた事を示す信号（ＰＣＯＶ）		*/
/*			を受けて、信号ＯＮがPnE7A.2[100ms]間続いた場合A.450「主回路コンデンサ過電圧」とする。	*/
/*																									*/
/*																									*/
/****************************************************************************************************/
void	LpxCheckPcovAlarm( PCOV_ERR_DETECT *PcovErrDetect, POWER_STATUS *PowSts )
{
		if( PcovErrDetect->conf.PcovEna == TRUE )
		{
			if( (PcovErrDetect->var.Pcov == TRUE) && (PowSts->PowerOn == TRUE) )
			{
				if( PcovErrDetect->var.PcovErrCnt >= PcovErrDetect->conf.PcovErrCntLmt )
				{
					ALMSetGlobalAlarm( ALM_PCOV );		/* A.450 : 主回路コンデンサ過電圧アラーム	*/
				}
				else
				{
					PcovErrDetect->var.PcovErrCnt++;
				}
			}
			else
			{
				PcovErrDetect->var.PcovErrCnt = 0;
			}
		}
		return;
}



/****************************************************************************************************/
/*																									*/
/*		DC電圧取得用API		<S020>																	*/
/*																									*/
/****************************************************************************************************/
LONG	PcmGetDcVoltage( POWER_STATUS *PowSts )
{
	return	PowSts->DcVolt;
}
/***************************************** end of file **********************************************/
