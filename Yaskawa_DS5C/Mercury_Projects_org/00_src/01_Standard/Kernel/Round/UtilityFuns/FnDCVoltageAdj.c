/****************************************************************************
  Description   : DCVolatage Gain Auto Adjustment
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corporation. 
                  Copyright (c) 2010 All Rights Reserved

  Project       : INGRAM

----------------------------------------------------------------------------
  Changes		:
	Name   Date          Description
----------------------------------------------------------------------------
	F.Ueda   2011.04.26   created

****************************************************************************/
#include "Basedef.h"
#include "MercuryGlobals.h"
#include "FunManager.h"
#include "JL086ServoIpHw.h"
#include "PnPrmListTbl.h"
#include "TuneLib.h"
#include "MLib.h"
#include "FnDCVoltageAdj.h"


/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
/* SGDV 自動調整モードとは仕様が異なるので注意！ */
static ADJUST_STATUS TciAdjustMainDcVoltage( REGIF_CONFIG_T *hPnReg, CMN_PRMDATA *cmnprm_ptr, 
											 ASIC_HW_IF_STR *pAsicHwReg, CHECK_MAIN_POWER *pMainPowerChecker, DET_DC_VOLT *pDetectDc );
static LONG TciDetectTestVdc( LONG RepNum, CHECK_MAIN_POWER *MainPowerChecker, 
							  DET_DC_VOLT *pDetectDc );
/****************************************************************************************************/
/*		DEFINES																						*/
/****************************************************************************************************/


/****************************************************************************************************/
/*		STATIC VARIABLES																			*/
/****************************************************************************************************/


/****************************************************************************
 0x2012: Entry function of DCVolatage Gain Auto Adjustment
****************************************************************************/
PRM_RSLT FnEnterDcVoltageAutoAdj( FUNEXE *FunExe, AXIS_HANDLE *Axis )
{
	FUN_CMN_CONTROL		*FnCmnCtrl;
	LONG				no;

	/* サーボオン中の場合は入らずに終了	*/
	for(no = 0; no < NumberOfAxes; no++)
	{
		FnCmnCtrl = ((AXIS_HANDLE*)GetAxisHandle(no))->FnCmnCtrl;
		if(FnCmnCtrl->State.FnSvCtrlDisable != FALSE)
		{
			return PRM_RSLT_CONDITION_ERR;
		}
	}
	
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
 0x2012: Execute function of DCVolatage Gain Auto Adjustment
****************************************************************************/
PRM_RSLT FnExecuteDcVoltageAutoAdj( FUNEXE *FunExe, AXIS_HANDLE *Axis )
{
	SEQ_CTRL_OUT				*SeqCtrlOut;
	REGIF_CONFIG_T				*hPnReg;
	CMN_PRMDATA					*cmnprm_ptr;
	ASIC_HW_IF_STR				*AsicHwReg;
	CHECK_MAIN_POWER 			*MainPowerChecker;
	DET_DC_VOLT					*DetectDc;
	ADJUST_STATUS				rslt;
	FUN_CMN_CONTROL				*FnCmnCtrl;
	LONG						no;

	/* Initialize */
	SeqCtrlOut				=	Axis->SeqCtrlOut;
	hPnReg					=	Axis->RegManager->hPnReg;
	cmnprm_ptr				=	Axis->UniPrms->Cprm;
	AsicHwReg				=	Axis->SvAsicRegs->AsicHwReg;
	MainPowerChecker		=	&(Axis->PowerManager->MainPowerChecker);
	DetectDc				=	&(Axis->PowerManager->DetectDcVolt);


	/* サーボオン中の場合は入らずに終了	*/
	for(no = 0; no < NumberOfAxes; no++)
	{
		FnCmnCtrl = ((AXIS_HANDLE*)GetAxisHandle(no))->FnCmnCtrl;
		if(FnCmnCtrl->State.FnSvCtrlDisable != FALSE)
		{
			return PRM_RSLT_CONDITION_ERR;
		}
	}
	
	/* 実行要求がくるまで、待ち	*/
	if(FunExe->CmdCode != FCMD_EXEC)
	{
		return PRM_RSLT_CONTINUE;
	}

	/* 主回路電源ON/OFFに関係なく実施可能とするため、MainPowerOnは監視しない				*/
	if ( (SeqCtrlOut->MotStop == FALSE)									/* Motor Moving		*/
		|| (SHalDrv_GetFaultStatus(AsicHwReg, SHAL_FAULT_BB) == 0) )	/* Base Enable		*/
	{
		return PRM_RSLT_CONDITION_ERR;
	}
	
	rslt = TciAdjustMainDcVoltage( hPnReg,
								   cmnprm_ptr, 
								   AsicHwReg, 
								   MainPowerChecker,
								   DetectDc );
	
	if( rslt == ADJ_SUCCESS )
	{
		;		/* 何もしない */
	}
	else if( rslt == ADJ_FAILURE )
	{
		return PRM_RSLT_CALC_ERR;
	}
	
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************************************/
/*																									*/
/*		主回路電源電圧検出レベル調整																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 :	試験機からＤＣ電圧(PnE72)を入れて、その時のＤＣ電圧検出値が所定の値(PnE72)になる		*/
/*			ように、調整パラメータを設定する。(調整分解能：1/256)									*/
/*			なお、電圧検出値が異常の場合及び電圧調整レベルが調整基準範囲(±20%)を外れる場合は、		*/
/*			調整異常として電圧調整レベルをゼロにする。												*/
/*																									*/
/*			1. 調整手順																				*/
/*				1) 正規化基準電圧演算 (基準電圧：回生オンレベル(PnE72))								*/
/*				2) 主回路電源電圧平均値の取得 (電圧検出4096回の平均)								*/
/*				3) 主電源電圧調整レベルの計算 (分解能：1/256)										*/
/*				4) 主電源電圧調整レベルの格納 (==>PnE5B：Prm.pnadj/EEPROM)							*/
/*																									*/
/*			2. 関連パラメータ																		*/
/*				1) PnE70 : P-N電圧検出レベル  [V]		(最大検出値の時の電圧)						*/
/*		 		2) PnE71 : ＯＶ検出レベル     [V]													*/
/*				3) PnE72 : 回生オンレベル     [V]		(試験機から入れるＤＣ電圧)					*/
/*		 		4) PnE73 : ＵＶ検出レベル     [V]													*/
/*				5) PnE5B : 下位:P-N電圧検出レベル微調	(調整結果を格納)							*/
/*																									*/
/*			3. 調整レベル演算式																		*/
/*				1) 記号定義																			*/
/*					VdcBase：基準ＤＣ電圧,  VdcMean：調整時読込電圧平均値							*/
/*					VdcAdj ：調整レベル値,  VdcDet ：ＤＣ電圧検出値(調整レベルで補正後の値)			*/
/*				2) ＤＣ電圧検出値補正式																*/
/*				   VdcDet = VdcRead * (256 + VdcAdj) / 256											*/
/*						  = VdcRead + (VdcRead * VdcAdj / 256)										*/
/*				3) 調整レベル演算式																	*/
/*				   読込電圧値が VdcMean の場合に、電圧検出値が VdcBase になるように補正する。		*/
/*				   VdcBase = VdcMean + (VdcMean * VdcAdj / 256) より								*/
/*				   VdcAdj  = (VdcBase - VdcMean) * 256 / VdcMean									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static ADJUST_STATUS TciAdjustMainDcVoltage( REGIF_CONFIG_T *hPnReg, CMN_PRMDATA *cmnprm_ptr, 
								    ASIC_HW_IF_STR *pAsicHwReg, CHECK_MAIN_POWER *pMainPowerChecker, DET_DC_VOLT *pDetectDc )
{
	LONG			i;
	LONG			VdcAdj;
	LONG			VdcBase;
	LONG			VdcMean;
	LONG			VdcTest;
	LONG			ErrFlag = FALSE;
	ADJUST_STATUS	rslt;
	REG_ACC_T		AccessPrm;

	/* Initialize */
	rslt = ADJ_SUCCESS;

	/* RAM書き込み用レジスタ情報設定 */
	AccessPrm.MemAddr	= 0;
	AccessPrm.hRegCfg	= hPnReg;
	AccessPrm.ArrayIdx	= 0;
	AccessPrm.EepWrite	= TRUE;		/* EEPROM Write */
	AccessPrm.IfHandle	= NULL;

/*--------------------------------------------------------------------------------------------------*/
/*		開始チェック																				*/
/*			ソフトウェアによる電圧検出でない場合は調整異常にてリターンする							*/
/*--------------------------------------------------------------------------------------------------*/
/* IprmcalSystemSwitch( )：VdetMode == 0の時A.040とするため、ここでは監視しない						*/
#if 0
		if( pDetectDc->conf.VdetMode == VDET_NONE )
		{
			rslt = ADJ_NONE;								/* 調整異常フラグ設定					*/
			return rslt;									/* 処理実行せずに戻る					*/
		}
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		調整基準電圧取り込み																		*/
/*			調整基準は回生オン電圧																	*/
/*--------------------------------------------------------------------------------------------------*/
		/* INGRAMでは、任意の入力電圧で調整可能とする												*/
		VdcBase = pMainPowerChecker->conf.TestDcLevel;		/* 調整基準電圧[V]						*/
/*--------------------------------------------------------------------------------------------------*/
/*		主電源電圧平均値の取得処理																	*/
/*--------------------------------------------------------------------------------------------------*/
		VdcTest = TciDetectTestVdc( 100, pMainPowerChecker, pDetectDc );
		if( VdcTest < 0 ){ ErrFlag = TRUE;}					/* 異常チェック							*/
	/*----------------------------------------------------------------------------------------------*/
		for( VdcMean=0,i=0; i<4096; i++ )					/* 主電源電圧取得処理(4096回)			*/
		{
			if( ErrFlag == TRUE ){ break;}					/* 異常時は終了							*/
			VdcTest = TciDetectTestVdc( 1, pMainPowerChecker, pDetectDc );
			if( VdcTest > 0 )								/* 主電源電圧取得正常					*/
			{
				VdcMean += VdcTest;							/* 主電源電圧加算						*/
			}
			else											/* 主電源電圧取得異常					*/
			{
				ErrFlag = TRUE;								/* 平均値取得異常						*/
				break;										/* 異常時は終了							*/
			}
		}
		VdcMean = (((VdcMean>>11)+1)>>1);					/* 平均値作成							*/

/*--------------------------------------------------------------------------------------------------*/
/*		主電源電圧調整レベルの計算																	*/
/*--------------------------------------------------------------------------------------------------*/
		if( (ErrFlag == FALSE) && (VdcMean > 0) )			/* 平均値取得正常						*/
		{
			VdcAdj = ((VdcBase - VdcMean)<<8) / VdcMean;	/* 調整レベル計算						*/
			if( MlibABS( VdcAdj ) > VDCADJ_ERRLVL )			/* 調整レベル上下限チェック				*/
			{
				VdcAdj = 0;									/* 調整レベル＝ゼロ						*/
				rslt = ADJ_FAILURE;
			}
		}
		else												/* 平均値取得失敗						*/
		{
			VdcAdj = 0;										/* 調整レベル＝ゼロ						*/
			rslt = ADJ_FAILURE;
		}

/*--------------------------------------------------------------------------------------------------*/
/*		調整レベルの格納																			*/
/*--------------------------------------------------------------------------------------------------*/
		/* パラメータ計算関数をcallして調整結果を格納												*/
		VdcAdj = (VdcAdj & 0x00FF);							/* Make Byte Data						*/
		/* PnE5B 下位:P-N電圧検出レベル微調, 上位:P-N電圧検出レベルゼロ調							*/
		TunePrmWriteRam( &AccessPrm, &pndef_pnadj, VdcAdj, 2 );

		return rslt;
}


/****************************************************************************************************/
/*																									*/
/*		試験機用ＤＣ電源電圧検出処理																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : Ｖ８５０のＰＷＭ出力をＰＨＣ絶縁してＲＣフィルタを通してアナログ電圧とし、				*/
/*			主回路検出部コンパレータ基準電源とする。												*/
/*																									*/
/*	補 足 : 基準電源電圧とＤＣ電圧をコンパレータで比較し、結果をＰＨＣ絶縁して、					*/
/*			Ｖ８５０の入力信号として、Ｖ８５０に伝える。											*/
/*																									*/
/*			1) コンパレータ出力がＬｏならば、基準電源電圧の方がＤＣ電圧より低い。					*/
/*			   この場合、ＰＷＭ出力データを+1する。													*/
/*			2) コンパレータ出力がＨｉならば、基準電源電圧の方がＤＣ電圧より高い。					*/
/*			   この場合、ＰＷＭ出力データを-1する。													*/
/*																									*/
/*			これにより、基準電源電圧は常にＤＣ電圧値に対して±１の誤差で追従する。					*/
/*			ＰＷＭデータ≒ＤＣ電圧とする。															*/
/*			なお、データが最大値になったら検出異常とする。											*/
/*																									*/
/*			ＰＷＭデータ : ２５５／２２５Ｖ…AC100V仕様の場合										*/
/*			ＰＷＭデータ : ２５５／４５０Ｖ…AC200V仕様の場合										*/
/*			ＰＷＭデータ : ２５５／９００Ｖ…AC400V仕様の場合										*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static LONG	TciDetectTestVdc( LONG RepNum, CHECK_MAIN_POWER *pMainPowerChecker, 
							  DET_DC_VOLT *pDetectDc )
{
	LONG		i;
	LONG		AdRead;
	LONG		VdcTest;
	
	/* Initialize */
	VdcTest = 0;
//	VdcTest	=	(LONG)(pMainPowerChecker->conf.TestDcLevel);

/*--------------------------------------------------------------------------------------------------*/
/*		ＤＣ電源電圧検出処理																		*/
/*--------------------------------------------------------------------------------------------------*/
		switch( pDetectDc->conf.VdetMode )
		{
		/*------------------------------------------------------------------------------------------*/
		/*		ＰＷＭ－コンパレータ方式															*/
		/*------------------------------------------------------------------------------------------*/
			/*--------------------------------------------------------------------------------------*/
			/*  固定±2v/250us(小容量),可変±16v/250(中容量)の場合があるがここでは追従時間は		*/
			/*	関係ないため固定追従させる															*/
			/*--------------------------------------------------------------------------------------*/
			case VDET_PWM_FIX_FOLLOW:
			case VDET_PWM_VAR_FOLLOW:
#if 0														/* 未対応であるため、削除				*/
				/*----------------------------------------------------------------------------------*/
				/*	PWM Dummy Output																*/
				/*----------------------------------------------------------------------------------*/
				KPX_START_VDCPWM( );						/* 電源電圧検出用PWM開始				*/
				KpiResetWatchdogWaitus( 125 );				/* Reset Watchdog and Wait				*/
				/*----------------------------------------------------------------------------------*/
				/*	実際の測定用 PWM Output															*/
				/*----------------------------------------------------------------------------------*/
				for( i=0; i < RepNum; i++ )
				{
					KPX_OUT_VDCPWM( (USHORT)VdcTest );		/* PWM出力設定							*/
					KpiResetWatchdogWaitus( 125 );			/* Reset Watchdog and Wait				*/
				/*----------------------------------------------------------------------------------*/
					if( KPX_CHK_VDCLOW( ) )					/* Lowの場合は、PWM出力はDC電圧より低い	*/
					{
						if( VdcTest < Bprm.VdetMax )		/* 上限確認								*/
						{
							++VdcTest;
						}
						else
						{
							return( -1 );					/* Maxまで上がるのは異常				*/
						}
					}
				/*----------------------------------------------------------------------------------*/
					else									/* Hiの場合は、PWM出力はDC電圧より高い	*/
					{
						if( VdcTest > 0 )					/* 下限確認								*/
						{
							--VdcTest;
						}
						else
						{
							return( -1 );					/* 0(=0V)まで下がるのは異常				*/
						}
					}
				}
#endif														/* 未対応であるため、削除				*/
				VdcTest = -1;								/* ここに来たら異常応答とする			*/
				break;
		/*------------------------------------------------------------------------------------------*/
		/*	Ａ／Ｄ変換方式																			*/
		/*------------------------------------------------------------------------------------------*/
			case VDET_AD:
			/*--------------------------------------------------------------------------------------*/
			/*		ＤＣ電圧ＡＤ読込み																*/
			/*--------------------------------------------------------------------------------------*/
				for( i=0; i < RepNum; i++ )
				{
					AdRead = HALdrv_GetDcVoltFromAd( );				/* ＤＣ電圧ＡＤ読込み			*/
#if (FLOAT_USE==TRUE)
					VdcTest = (SHORT)(( AdRead * pDetectDc->conf.VdetAdNorm ));
#else
					VdcTest = (SHORT)(MlibMulgain( AdRead, pDetectDc->conf.VdetAdNorm ));
#endif /* FLOAT_USE */

					if( VdcTest >= pDetectDc->conf.VdetMaxVolt )
					{
						return( -1 );								/* Max値まで上がるのは異常		*/
					}
				}
				break;
		/*------------------------------------------------------------------------------------------*/
		/*		検出なし																			*/
		/*------------------------------------------------------------------------------------------*/
			default:
				VdcTest = -1;										/* 検出なしは異常				*/
				break;
		}

		return( VdcTest );
}

