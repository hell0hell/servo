/****************************************************************************
  Description   : アナログ指令調整(Fn009, Fn00A, Fn00B)
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corporation.
                  Copyright (c) 2013 All Rights Reserved

  Project       : Mercury

  Create Date   : 2013-11-26

****************************************************************************/
#include "Basedef.h"
#include "MercuryGlobals.h"
#include "FunManager.h"
#include "JL086ServoIpHw.h"

#include "PnPrmListTbl.h"
#include "KLib.h"



/****************************************************************************************************/
/*		DEFINES																						*/
/****************************************************************************************************/
#define KPI_RC_NOTADJ	0		/* 調整実施条件不成立 */
#define KPI_RC_ADJOK	1		/* 調整完了 */
#define KPI_RC_ADJERR	-1		/* 調整失敗 */

#define OFFSET_ADJ		0		/* 電流検出オフセット調整 */


/* 速度指令オフセットのマニュアル調整値　最大値/最小値 */
//#define	ZADJV_MAX			32767	/* <S0A9>	Delete */
//#define	ZADJV_MIN			-32768	/* <S0A9>	Delete */

/* <S0A9>	Start */
#if	( SVFSEL_VREFAD_24BIT != 0 )
#define	ZADJV_MAX			52500000
#define	ZADJV_MIN			-52500000
#define	ZADJV_RATE			3500			/* 52500000÷15000 = 3500	*/
#else
#define	ZADJV_MAX			240000
#define	ZADJV_MIN			-240000
#define	ZADJV_RATE			16				/* 240000÷15000 = 16		*/
#endif
/* <S0A9>	End */


/* トルク指令オフセットのマニュアル調整値　最大値/最小値 */
//#define	ZADJT_MAX		0xffff	/* <S0A9> Delete */
//#define	ZADJT_MIN		0x0000	/* <S0A9> Delete */
#define	ZADJT_MAX			127		/* <S0A9> Add */
#define	ZADJT_MIN			-127	/* <S0A9> Add */


/* ユーザレベル */
//#define	ACCLVL_SYSTEM	3	/* ユーザレベル = システムレベル */	/* <S101> */
#define	ACCLVL_SYSTEM	4		/* ユーザレベル = システムレベル */	/* <S101> */


//#define	OK	1					/* 電流検出信号のオフセット手動調整　実行可能 *///<S0C7>
//#define	NG	0					/* 電流検出信号のオフセット手動調整　実行不可能 *///<S0C7>

#define	TREFZVADP		0x0C2	/* 正側トルク指令ＡＤゼロＶ入力時のＡＤ値				*/
#define	TREFZVADN		0x098	/* 負側トルク指令ＡＤゼロＶ入力時のＡＤ値				*/


/*		指令用ＡＤコントロール			*/
#define _UDL_REG_ADDR_  (ULONG)0xC0000000
#define KPX_REFAD_LATCH( )			(((ASIC_HW_IF_STR *)_UDL_REG_ADDR_)->AREG_ADSET1) |= (1<<14)

/* 加算(64bit+32bit) */
#define	ADD6432( a, b, xx )		*(INT64*)xx = *(INT64*)(a) + (INT64)(b)

/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
/* <S0BC> Start */
/* 下記の4つの関数は関数名が同じ関数が「\Kernel\Driver\AdcIf.c」にある為、関数名称を変更する。 */
//static LONG AdjustRefAdZero( AXIS_HANDLE *Axis );
//static void TciAdjustRefAdZero( AXIS_HANDLE *Axis );
//static void TciReadTrefVrefADMean( LONG AdMean[3] );
//static void	MlibAdd6432( LONG *a, LONG b, LONG *x );

static LONG FnAdjustRefAdZero( AXIS_HANDLE *Axis );
static void FnTciAdjustRefAdZero( AXIS_HANDLE *Axis );
static void FnTciReadTrefVrefADMean( LONG AdMean[3] );
static void	FnMlibAdd6432( LONG *a, LONG b, LONG *x );
/* <S0BC> End */


/****************************************************************************
 Fn009: Entry function of Analog Reference Offset Auto Adjustment
****************************************************************************/
PRM_RSLT FnEnterAnlgRefOffsetAutoAdj(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
//#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)	/* AnalogIF */		/* <S0A9> Delete:AP形限定の補助機能ではない為、削除 */

	FUN_CMN_CONTROL		*FnCmnCtrl;

	FnCmnCtrl = Axis->FnCmnCtrl;

	/* サーボオン中の場合は入らずに終了	*/
	if(FnCmnCtrl->State.FnSvCtrlDisable != FALSE)
	{
		return PRM_RSLT_CONDITION_ERR;
	}

	FnCmnCtrl->KnlMode = KPI_KMD_ADJREFAD;	/* reference A/D adjusting  */

//#endif	/* <S0A9> Delete:AP形限定の補助機能ではない為、削除 */

	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
 Fn009: Execute function of Analog Reference Offset Auto Adjustment
****************************************************************************/
PRM_RSLT FnExecuteAnlgRefOffsetAutoAdj(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
//#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)	/* AnalogIF */		/* <S0A9> Delete:AP形限定の補助機能ではない為、削除 */

	FUN_CMN_CONTROL	*FnCmnCtrl;
	SEQ_CTRL_OUT	*SeqCtrlOut;
	ASIC_HW_IF_STR	*AsicHwReg;

	LONG			AdjResult;
	FnCmnCtrl = Axis->FnCmnCtrl;
	SeqCtrlOut = Axis->SeqCtrlOut;
	AsicHwReg = Axis->SvAsicRegs->AsicHwReg;

	/* 実行要求がくるまで、待ち	*/
	if(FunExe->CmdCode != FCMD_EXEC)
	{
		return PRM_RSLT_CONTINUE;
	}

	/* サーボオン中の場合は入らずに終了	*/
//	if( (SeqCtrlOut->MainPowerOn == FALSE)	/* <S0FC> */
//		|| (SeqCtrlOut->HwbbSts != FALSE)	/* <S0FC> */
//		|| (SeqCtrlOut->MotStop == FALSE)	/* <S0FC> */
//		|| (SHalDrv_GetFaultStatus(AsicHwReg, SHAL_FAULT_BB) == 0) )	/* Base Enable */	/* <S0FC> */
	if( SHalDrv_GetFaultStatus(AsicHwReg, SHAL_FAULT_BB) == 0 )			/* Base Enable */	/* <S0FC> */
	{
		return PRM_RSLT_CONDITION_ERR;
	}

	/* 調整実行 */
//	AdjResult = AdjustRefAdZero(Axis);		/* <S0BC> 同じ名称の関数が存在する為、関数名を変更 */
	AdjResult = FnAdjustRefAdZero(Axis);	/* <S0BC> 同じ名称の関数が存在する為、関数名を変更 */

	/* 調整終了 */
	FnCmnCtrl->KnlMode = KPI_KMD_NORMAL;

	if(AdjResult == KPI_RC_NOTADJ){
		return PRM_RSLT_CONDITION_ERR;
	}
	else if(AdjResult == KPI_RC_ADJOK){
		/* 何もしない */
	}
	else if(AdjResult == KPI_RC_ADJERR){
		return PRM_RSLT_CALC_ERR;
	}

//#endif		/* <S0A9> Delete:AP形限定の補助機能ではない為、削除 */
	return PRM_RSLT_SUCCESS;
}
#if 0	/* <S0E8>	Delete:本関数は処理がない為、削除する。  */
/****************************************************************************
 Fn009: End function of Analog Reference Offset Auto Adjustment
****************************************************************************/
void FnLeaveAnlgRefOffsetAutoAdj(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
//#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)	/* AnalogIF */

	REGIF_CONFIG_T	*RegCfg;
	RegCfg = Axis->RegManager->hPnReg;

	/* 終了処理 */
	/* EEPROM更新処理 */
	//PrmStoreValueToEeprom( RegCfg, &pndef_zadjvl, 0 );		/* PnE53 : 速度指令ゼロ調 */					/* <S0A9>	Delete */
	//PrmStoreValueToEeprom( RegCfg, &pndef_zadjtp_n, 0 );		/* PnE56 : トルク指令ゼロ調(正入力/負入力) */	/* <S0A9>	Delete */

//#endif		/* <S0A9> Delete:AP形限定の補助機能ではない為、削除 */
}
#endif	/* <S0E8>	Delete:本関数は処理がない為、削除する。 */


/****************************************************************************
 Fn00A: Entry function of Velocity Reference Offset Manual Adjustment
****************************************************************************/
PRM_RSLT FnEnterAnlgVelocityRefManualAdj(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
//#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)	/* AnalogIF */		/* <S0A9> Delete:AP形限定の補助機能ではない為、削除 */
	FUN_CMN_CONTROL		*FnCmnCtrl;

	FnCmnCtrl = Axis->FnCmnCtrl;


//	FnCmnCtrl->KnlMode = KPI_KMD_ADJREFAD;	/* reference A/D adjusting  */
	FnCmnCtrl->KnlMode = KPI_KMD_NORMAL;	/* normal mode */	/* <S142> */

	FunExe->HoldFnMode = TRUE;				/* Fnモード自動終了なし */
//#endif		/* <S0A9> Delete:AP形限定の補助機能ではない為、削除 */

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
 Fn00A: Execute function of Velocity Reference Offset Manual Adjustment
****************************************************************************/
PRM_RSLT FnExecuteAnlgVelocityRefManualAdj(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	PRMDATA			*Prm;
	PRM_RSLT		rc;
	LONG			zadjvl_tmp;
	CHAR			Exe_flg;
	FUN_AXCOMMON	*AxCommon;
	CADC	*pAdc;	/* <S0F9> */

	Prm = Axis->UniPrms->Prm;
	AxCommon = ((AXIS_HANDLE*)Axis)->FnManager->AxCommon;
	pAdc = Axis->pAdc;	/* <S0F9> */

	rc = PRM_RSLT_SUCCESS;
	Exe_flg = NG;

//#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)	/* AnalogIF */	/* <S0A9> Delete:AP形限定の補助機能ではない為、削除 */
	/* ユーザレベル確認 */
#if 0
	if(AxCommon->AccessLevel < ACCLVL_SYSTEM )
	{
		Exe_flg = NG;
	}
	else
	{
		Exe_flg = OK;
	}
#else
	Exe_flg = OK;
#endif
		switch(FunExe->CmdCode)
		{
		case FCMD_UP:	/* UP,正転,正方向 */
			if(Exe_flg == OK)
			{
				/* PnE53 : 速度指令ゼロ調の値読み込み */
				//zadjvl_tmp = (LONG)(SHORT)((Prm->zadjvl) & 0xFFFF);/* <S0A9> Data型変更 */
				zadjvl_tmp = Prm->zadjvl;							/* <S0A9> Data型変更 */

//				zadjvl_tmp += 1;
				zadjvl_tmp += ZADJV_RATE;

				if( zadjvl_tmp > ZADJV_MAX )
				{
					zadjvl_tmp = ZADJV_MAX;
				}
				/*  PnE53 : 速度指令ゼロ調に更新した値をRAMへ書き込み */
				//Prm->zadjvl = (SHORT)zadjvl_tmp;
				Prm->zadjvl = zadjvl_tmp;	/* <S0A9> 自動試験機対応 */
				pAdc->P.ZadjVelo = zadjvl_tmp;							/* <S170> */

			}
			break;
		case FCMD_DOWN:	/* DOWN,逆転,逆方向 */
			if(Exe_flg == OK)
			{
				//zadjvl_tmp = (LONG)(SHORT)((Prm->zadjvl) & 0xFFFF);/* <S0A9> 自動試験機対応:Data型変更 */
				zadjvl_tmp = Prm->zadjvl;	/* <S0A9> Data型変更 */

//				zadjvl_tmp -= 1;
				zadjvl_tmp -= ZADJV_RATE;

				if( zadjvl_tmp < ZADJV_MIN )
				{
					zadjvl_tmp = ZADJV_MIN;
				}
				/*  PnE53 : 速度指令ゼロ調に更新した値を書き込み */
				//Prm->zadjvl = (SHORT)zadjvl_tmp;
				Prm->zadjvl = zadjvl_tmp;	/* <S0A9> */

				pAdc->P.ZadjVelo = Prm->zadjvl;	/*  PnE53 : 速度指令ゼロ調をZero Adjustに格納 */	/* <S0F9> */
			}
			break;

		default:	/* other command */
			rc = PRM_RSLT_CONDITION_ERR;	/* 条件エラー */
			break;
		}
//#endif		/* <S0A9> Delete:AP形限定の補助機能ではない為、削除 */

	return rc ;
}

/****************************************************************************
 Fn00A: End function of Velocity Reference Offset Manual Adjustment
****************************************************************************/
void FnLeaveAnlgVelocityRefManualAdj(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
//#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)	/* AnalogIF */	/* <S0A9> Delete:AP形限定の補助機能ではない為、削除 */

	FUN_CMN_CONTROL	*FnCmnCtrl;
	REGIF_CONFIG_T	*RegCfg;

	FnCmnCtrl = Axis->FnCmnCtrl;
	RegCfg = Axis->RegManager->hPnReg;

	/* EEPROM更新処理	*/
	PrmStoreValueToEeprom( RegCfg, &pndef_zadjvl, 0 );		/* PnE53 : 速度指令ゼロ調 */
//	PrmStoreValueToEeprom( RegCfg, &pndef_zadjvh, 0 );		/* PnE54 : 速度指令ゼロ調 */	/* <S0A9> 自動試験機対応 */


	/* 終了処理	*/
	FnCmnCtrl->KnlMode = KPI_KMD_NORMAL;					/* normal mode */

//#endif	/* <S0A9> Delete:AP形限定の補助機能ではない為、削除 */
}

/****************************************************************************
 Fn00B: Entry function of Torque Reference Offset Manual Adjustment
****************************************************************************/
PRM_RSLT FnEnterAnlgTorqueRefManualAdj(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
//#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)	/* AnalogIF */	/* <S0A9> Delete:AP形限定の補助機能ではない為、削除 */

	FUN_CMN_CONTROL		*FnCmnCtrl;

	FnCmnCtrl = Axis->FnCmnCtrl;


//	FnCmnCtrl->KnlMode = KPI_KMD_ADJREFAD;	/* reference A/D adjusting  */
	FnCmnCtrl->KnlMode = KPI_KMD_NORMAL;		/* normal mode */	/* <S142> */

	FunExe->HoldFnMode = TRUE;				/* Fnモード自動終了なし */

//#endif		/* <S0A9> Delete:AP形限定の補助機能ではない為、削除 */
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
 Fn00B: Execute function of Torque Reference Offset Manual Adjustment
****************************************************************************/
PRM_RSLT FnExecuteAnlgTorqueRefManualAdj(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	PRMDATA			*Prm;
	PRM_RSLT		rc;
//	USHORT			zadjtp_n_tmp;	/* <S0A9> Delete */
//	SHORT			zadjtp_n_tmp;	/* <S0A9> Add */
	DBYTEX			zadjtp_n_tmp;
	CHAR			Exe_flg;
	FUN_AXCOMMON	*AxCommon;
	CADC			*pAdc = Axis->pAdc;	/* <S170> */

	Prm = Axis->UniPrms->Prm;
	AxCommon = ((AXIS_HANDLE*)Axis)->FnManager->AxCommon;

	rc = PRM_RSLT_SUCCESS;
	Exe_flg = NG;

//#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)	/* AnalogIF */	/* <S0A9> Delete:AP形限定の補助機能ではない為、削除 */
	/* ユーザレベル確認 */
#if 0
	if(AxCommon->AccessLevel < ACCLVL_SYSTEM )
	{
		Exe_flg = NG;
	}
	else
	{
		Exe_flg = OK;
	}
#else
		Exe_flg = OK;
#endif
		switch(FunExe->CmdCode)
		{
		case FCMD_UP:	/* UP,正転,正方向 */
			if(Exe_flg == OK)
			{
				/* PnE56 : トルク指令ゼロ調(正入力/負入力)の値読み込み */
				//zadjtp_n_tmp = Prm->zadjtp_n.w;					/* <S0A9> Delete */
//				zadjtp_n_tmp = (SHORT)((Prm->zadjtp_n.w)& 0xFFFF);	/* <S0A9> Add */
				zadjtp_n_tmp.w = (SHORT)((Prm->zadjtp_n.w)& 0xFFFF);
//				if( zadjtp_n_tmp < ZADJT_MAX )
				if( ((CHAR)zadjtp_n_tmp.b.l < ZADJT_MAX) && ((CHAR)zadjtp_n_tmp.b.h < ZADJT_MAX) )
				{
//					zadjtp_n_tmp += 1;
					(CHAR)zadjtp_n_tmp.b.l++;
					(CHAR)zadjtp_n_tmp.b.h++;
				}
				/*  PnE56 : トルク指令ゼロ調(正入力/負入力)に更新した値を書き込み */
//				Prm->zadjtp_n.w = zadjtp_n_tmp;
				Prm->zadjtp_n.w = zadjtp_n_tmp.w;
				pAdc->P.ZadjTrqP = (CHAR)zadjtp_n_tmp.b.l;		/* <S170> */
				pAdc->P.ZadjTrqN = (CHAR)zadjtp_n_tmp.b.h;		/* <S170> */
			}
			break;

		case FCMD_DOWN:	/* DOWN,逆転,逆方向 */
			if(Exe_flg == OK)
			{
				/* PnE56 : トルク指令ゼロ調(正入力/負入力)の値読み込み */
				//zadjtp_n_tmp = Prm->zadjtp_n.w;					/* <S0A9> Delete */
//				zadjtp_n_tmp = (SHORT)((Prm->zadjtp_n.w)& 0xFFFF);	/* <S0A9> Add */
				zadjtp_n_tmp.w = (SHORT)((Prm->zadjtp_n.w)& 0xFFFF);
//				if(zadjtp_n_tmp > ZADJT_MIN)
				if( ((CHAR)zadjtp_n_tmp.b.l > ZADJT_MIN) && ((CHAR)zadjtp_n_tmp.b.h > ZADJT_MIN) )
				{
//					zadjtp_n_tmp -= 1;
					(CHAR)zadjtp_n_tmp.b.l--;
					(CHAR)zadjtp_n_tmp.b.h--;
				}
				/*  PnE56 : トルク指令ゼロ調(正入力/負入力)に更新した値を書き込み */
				Prm->zadjtp_n.w = zadjtp_n_tmp.w;
				pAdc->P.ZadjTrqP = (CHAR)zadjtp_n_tmp.b.l;		/* <S170> */
				pAdc->P.ZadjTrqN = (CHAR)zadjtp_n_tmp.b.h;		/* <S170> */
			}
			break;

		default:	/* other command */
			rc = PRM_RSLT_CONDITION_ERR;	/* 条件エラー */
			break;
		}
//#endif		/* <S0A9> Delete:AP形限定の補助機能ではない為、削除 */

	return rc ;

}

/****************************************************************************
 Fn00B: End function of Torque Reference Offset Manual Adjustment
****************************************************************************/
void FnLeaveAnlgTorqueRefManualAdj(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
//#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)	/* AnalogIF */		/* <S0A9> Delete:AP形限定の補助機能ではない為、削除 */

	FUN_CMN_CONTROL	*FnCmnCtrl;
	REGIF_CONFIG_T	*RegCfg;

	FnCmnCtrl = Axis->FnCmnCtrl;
	RegCfg = Axis->RegManager->hPnReg;

	/* EEPROM更新処理	*/
	PrmStoreValueToEeprom( RegCfg, &pndef_zadjtp_n, 0 );		/* PnE56 : トルク指令ゼロ調(正入力/負入力) */

	/* 終了処理	*/
	FnCmnCtrl->KnlMode = KPI_KMD_NORMAL;					/* normal mode */

//#endif	/* <S0A9> Delete:AP形限定の補助機能ではない為、削除 */
}


/****************************************************************************************************/
/*																									*/
/*		指令ＡＤ零調処理(オンライン用)																*/
/*																									*/
/****************************************************************************************************/
//static LONG	AdjustRefAdZero( AXIS_HANDLE *Axis )	/* <S0BC> 同じ名称の関数が存在する為、関数名を変更 */
static LONG	FnAdjustRefAdZero( AXIS_HANDLE *Axis )		/* <S0BC> 同じ名称の関数が存在する為、関数名を変更 */
{
//#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)	/* AnalogIF */	/* <S0BC> Delete:AP形限定の補助機能ではない為、削除 */

	LONG	i;
	CADC *pAdc;

	pAdc = Axis->pAdc;

/*--------------------------------------------------------------------------------------------------*/
/*		指令ＡＤ零調処理																			*/
/*--------------------------------------------------------------------------------------------------*/
		pAdc->f.AdjustError = FALSE;					/* Reset AdjustError Flag */
		pAdc->Mode = ADCMD_ADJREFAD;					/* set adjust mode */
//		TciAdjustRefAdZero( Axis  );					/* Adjust VrefAD and TrefAD */	/* <S0BC> 同じ名称の関数が存在する為、関数名を変更 */
		FnTciAdjustRefAdZero( Axis  );					/* Adjust VrefAD and TrefAD */	/* <S0BC> 同じ名称の関数が存在する為、関数名を変更 */
		pAdc->Mode = ADCMD_NORMAL;						/* reset adjust mode */	/* <S0E9> */

/*--------------------------------------------------------------------------------------------------*/
/*		終了処理																					*/
/*--------------------------------------------------------------------------------------------------*/
		if( pAdc->f.AdjustError == FALSE )
		{
			return( KPI_RC_ADJOK );					/* Adjust OK								*/
		}
		else
		{
			return( KPI_RC_ADJERR );					/* Adjust Error								*/
		}

//#endif	/* <S0BC> Delete:AP形限定の補助機能ではない為、削除 */
}

/****************************************************************************************************/
/*																									*/
/*		指令ＡＤ零調処理(オフライン／オンライン用)													*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 各指令ＡＤの入力ゼロ時の調整用データ(指令入力ゼロ時のオフセット量)を作成し、			*/
/*			それぞれパラメータに設定する。															*/
/*			なお、オフセット量が以下の範囲を外れる場合は、調整異常としてオフセット量を				*/
/*			ゼロにする。																			*/
/*																									*/
/*			1) T-ref P [ﾄﾙｸ指令A/D 正側] : -64 ≦ ｵﾌｾｯﾄ ≦ 64	(64/4096=1.56%)						*/
/*																									*/
/*			2) T-ref N [ﾄﾙｸ指令A/D 負側] : -64 ≦ ｵﾌｾｯﾄ ≦ 64	(64/4096=1.56%)						*/
/*																									*/
/*			3) V-ref   [速度指令A/D]	 : -52500000 ≦ ｵﾌｾｯﾄ ≦ 52500000	(24Bit AD の場合)		*/
/*										 : -200000   ≦ ｵﾌｾｯﾄ ≦ 200000		(16Bit AD の場合)		*/
/*																									*/
/*											24Bit AD の場合 ==> (52500000/(2^23*128)=4.889%)		*/
/*											16Bit AD の場合 ==> (  200000/(2^15*128)=4.768%)		*/
/*																									*/
/*																									*/
/****************************************************************************************************/
//static void TciAdjustRefAdZero( AXIS_HANDLE *Axis )	/* <S0BC> 同じ名称の関数が存在する為、関数名を変更 */
static void FnTciAdjustRefAdZero( AXIS_HANDLE *Axis )	/* <S0BC> 同じ名称の関数が存在する為、関数名を変更 */
{
//#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)	/* AnalogIF */		/* <S0BC> Delete:AP形限定の補助機能ではない為、削除 */

	LONG	Zerox;
	LONG	Zeroy;
	LONG	AdMean[3];
	SHORT	XSat;
	SHORT	YSat;
	CADC	*pAdc;
	PRMDATA	*Prm;
	REGIF_CONFIG_T	*RegCfg;	/* <S0A9>  */

	pAdc = Axis->pAdc;
	Prm = Axis->UniPrms->Prm;
	XSat = FALSE;
	YSat = FALSE;
	RegCfg = Axis->RegManager->hPnReg;/* <S0A9>  */

/*--------------------------------------------------------------------------------------------------*/
/*		ＡＤ平均値読込み																			*/
/*--------------------------------------------------------------------------------------------------*/
		KPX_SET_ADSEL_TREF( );					/* Select Tref AD Select Switch */
		KlibWaitms( 1 );						/* Reset Watchdog and Wait */
//		TciReadTrefVrefADMean( AdMean );		/* Read P-Tref, N-Tref, Vref AD Mean */	/* <S0BC> 同じ名称の関数が存在する為、関数名を変更 */
		FnTciReadTrefVrefADMean( AdMean );		/* Read P-Tref, N-Tref, Vref AD Mean */	/* <S0BC> 同じ名称の関数が存在する為、関数名を変更 */


/*--------------------------------------------------------------------------------------------------*/
/*		トルク指令零調データ演算＆格納																*/
/*--------------------------------------------------------------------------------------------------*/

		/* 正側/不側トルク指令ＡＤゼロＶ入力時のＡＤ値 作成＆確認 */
		Zerox = -AdMean[0] + TREFZVADP;			/* 正側トルク指令ＡＤゼロＶ入力時のＡＤ値 算出 */
		//if( (Zerox < -64) || (Zerox > 64) )		/* 正側トルク指令ＡＤゼロＶ入力時のＡＤ値 確認(10bit->12bit chk!!) */
		if( (Zerox < -127) || (Zerox > 127) )		/* 正側トルク指令ＡＤゼロＶ入力時のＡＤ値 確認(10bit->12bit chk!!) */ /* <S169> */
		{
			pAdc->f.AdjustError = TRUE;
		}

		Zeroy = +AdMean[1] - TREFZVADN;			/* 負側トルク指令ＡＤゼロＶ入力時のＡＤ値 算出 */
		//if( (Zeroy < -64) || (Zeroy > 64) )		/* 負側トルク指令ＡＤゼロＶ入力時のＡＤ値 確認(10bit->12bit chk!!) */
		if( (Zeroy < -127) || (Zeroy > 127) )		/* 負側トルク指令ＡＤゼロＶ入力時のＡＤ値 確認(10bit->12bit chk!!) */ /* <S169> */
		{
			pAdc->f.AdjustError = TRUE;
		}


		/* 正側/不側トルク指令ＡＤゼロＶ入力時のＡＤ値 作成＆確認 */
		if( Zerox >= TREFZVADP )				/* 算出した正側トルク指令ＡＤゼロＶ入力時のＡＤ値が設定値以上の場合 */
		{
			XSat = TRUE;
		}
		if( Zeroy <= -(TREFZVADN) )				/* 算出した不側トルク指令ＡＤゼロＶ入力時のＡＤ値が設定値以下の場合 */
		{
			YSat = TRUE;
		}
	/*----------------------------------------------------------------------------------------------*/
		if ( pAdc->f.AdjustError )
		{
			Zerox = 0;
			Zeroy = 0;
		}
		else if(XSat == TRUE && YSat == FALSE)	/* 正側トルク指令ＡＤゼロＶ入力時のＡＤ値が設定値以上 */
		{
			Zerox = Zeroy;
		}
		else if(XSat == FALSE && YSat == TRUE)	/* 算出した不側トルク指令ＡＤゼロＶ入力時のＡＤ値が設定値以下の場合 */
		{
			Zeroy = Zerox;
		}
		else if(XSat == TRUE && YSat == TRUE)	/* 正側トルク指令ＡＤゼロＶ入力時のＡＤ値が設定値以上、算出した不側トルク指令ＡＤゼロＶ入力時のＡＤ値が設定値以下の場合 */
		{
			pAdc->f.AdjustError = TRUE;
			Zerox = 0;
			Zeroy = 0;
		}
		pAdc->P.ZadjTrqP = Zerox;								/* set Zero Adjust Variable */
		pAdc->P.ZadjTrqN = Zeroy;								/* set Zero Adjust Variable */
		Zerox = (Zerox & 0xFF) + (Zeroy << 8);					/* Combine P-Tref and N-Tref		*/
		//TODO:		Prm.zadjtp_n.w = (USHORT)Zerox;				/* Write Adjust Data to RAM			*/
		/*  PnE56 : トルク指令ゼロ調(正入力/負入力)に更新した値をRAMへ書き込み */
		Prm->zadjtp_n.w = (USHORT)Zerox;
		PrmStoreValueToEeprom( RegCfg, &pndef_zadjtp_n, 0 );	/* Write Adjust Data to EepROM */		/* <S0A9>	TODO解除 */



/*--------------------------------------------------------------------------------------------------*/
/*		速度指令零調データ演算＆格納																*/
/*--------------------------------------------------------------------------------------------------*/
#if	( SVFSEL_VREFAD_24BIT != 0 )
		Zerox = -AdMean[2];									/* 速度指令ゼロ調の値 作成 */
		if ( (Zerox < -52500000) || (Zerox > 52500000) )	/* 作成した速度指令ゼロ調の値 確認 */
		{
			Zerox = 0;
			KnlV.f.AdjustError = TRUE;
		}
#else
/*--------------------------------------------------------------------------------------------------*/
/*		-240000～+240000以外の場合、エラーにする：	(15.36/0x7FFF)×(240000/128) = 879mV			*/
/*																									*/
/*		オフセット調整範囲は、マニュアル上では±750mVであるが、										*/
/*		Fn00Aの処理上、オフセット調整範囲が15000で割り切れる方が都合が良いので±240000にする		*/
/*--------------------------------------------------------------------------------------------------*/
		Zerox = -AdMean[2];									/* 速度指令ゼロ調の値 作成 */
		if ( (Zerox < -240000) || (Zerox > 240000) )		/* 作成した速度指令ゼロ調の値 確認 */
		{
			Zerox = 0;
			pAdc->f.AdjustError = TRUE;
		}
#endif
	/*----------------------------------------------------------------------------------------------*/
		pAdc->P.ZadjVelo = Zerox;
//TODO:		Prm.zadjvl = (USHORT)Zerox;						/* Write Adjust Data to RAM					*/
//TODO:		Prm.zadjvh = (USHORT)(Zerox>>16);				/* Write Adjust Data to RAM					*/
		Prm->zadjvl = Zerox;								/* Write Adjust Data to RAM					*/	/* <S0A9>	TODO解除 */
//		Prm->zadjvh = (USHORT)(Zerox>>16);					/* Write Adjust Data to RAM					*/	/* <S0A9>	TODO解除 */
		PrmStoreValueToEeprom( RegCfg, &pndef_zadjvl, 0 );	/* Write Adjust Data to EepROM				*/	/* <S0A9> */

//#endif	/* <S0BC> Delete:AP形限定の補助機能ではない為、削除 */
		return;
}
/****************************************************************************************************/
/*																									*/
/*		トルク指令ＡＤ＆速度指令ＡＤ平均値読込み(オフライン／オンライン用)							*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : トルク指令ＡＤ値及び速度指令ＡＤ値を４０９６回読込み、その平均値を返す。				*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
//static void TciReadTrefVrefADMean( LONG AdMean[3] )	/* <S0BC> 同じ名称の関数が存在する為、関数名を変更 */
static void FnTciReadTrefVrefADMean( LONG AdMean[3] )	/* <S0BC> 同じ名称の関数が存在する為、関数名を変更 */
{
//#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)	/* AnalogIF */	/* <S0BC> Delete:AP形限定の補助機能ではない為、削除 */

	LONG	i;
	LONG	Adlo;
	LONG	Adhi;
	LONG	TAdBuf[2];
	LONG	VAdBuf[2];


/*--------------------------------------------------------------------------------------------------*/
/*		ＡＤ読込み(4096回) : 30us * 4096 = 122.880 ms												*/
/*--------------------------------------------------------------------------------------------------*/
/*		<1>CPUADをRepeat Modeにしたため、TrefAdの起動処理を無くした。								*/
/*--------------------------------------------------------------------------------------------------*/
		TAdBuf[0] = TAdBuf[1] = 0;						/* Clear Tref AD Buffer						*/
		VAdBuf[0] = VAdBuf[1] = 0;						/* Clear Vref AD Buffer						*/
		for( i=0; i < 0x1000; i++ )						/* Read Loop 4096 Times						*/
		{
			KlibWaitus( 30 );							/* Wait 30us for AD Conversion				*/
			TAdBuf[0] += KPX_READ_CPUAD0();				/* Read and Add Buf	(P-Tref)				*/
			TAdBuf[1] += KPX_READ_CPUAD1();				/* Read and Add Buf (N-Tref)				*/
			KlibWaitus( 18 );							/* Reset Watchdog and Wait					*/
		/*------------------------------------------------------------------------------------------*/
			KPX_REFAD_LATCH( );									/* Latch Vref AD Conv. Data			*/

#if	( SVFSEL_VREFAD_24BIT != 0 )	/* @SKLT@ */
			Adlo = (USHORT)(ASIC_RD( AREG_ACVLL ));				/* Read Low Word					*/
			Adhi = ((LONG)(ASIC_RD( AREG_ACVLH )<<24))>>8;		/* Read High Word					*/
			//MlibAdd6432( VAdBuf, ((Adhi+Adlo)<<7), VAdBuf );	/* Add  Buffer	(128*AdData)		*/	/* <S0BC> 同じ名称の関数が存在する為、関数名を変更 */
			FnMlibAdd6432( VAdBuf, ((Adhi+Adlo)<<7), VAdBuf );	/* Add  Buffer	(128*AdData)		*/	/* <S0BC> 同じ名称の関数が存在する為、関数名を変更 */
#else		/* 16bit AD */
			//Adlo = (SHORT)(((ASIC_HW_IF_STR *)_UDL_REG_ADDR_)->AREG_ACUL);	/* 暫定 */	/* <S0BC> */
			Adlo = (USHORT)(((ASIC_HW_IF_STR *)_UDL_REG_ADDR_)->AREG_ACUL);		/* 暫定 */	/* <S0BC> */
			Adhi = ((( Adlo << 16 ) >> 16 ) & 0xFFFF0000 );		/* <VZADJ>符合拡張					*/
#endif
			//MlibAdd6432( VAdBuf, ((Adhi+Adlo)<<7), VAdBuf );	/* <V031>Add  Buffer (128*AdData)	*/	/* <S0BC> 同じ名称の関数が存在する為、関数名を変更 */
			FnMlibAdd6432( VAdBuf, ((Adhi+Adlo)<<7), VAdBuf );	/* <V031>Add  Buffer (128*AdData)	*/	/* <S0BC> 同じ名称の関数が存在する為、関数名を変更 */
		}

/*--------------------------------------------------------------------------------------------------*/
/*		平均値演算																					*/
/*--------------------------------------------------------------------------------------------------*/
		AdMean[0] = (((TAdBuf[0]>>11)+1)>>1);							/* P-Tref AD Mean			*/
		AdMean[1] = (((TAdBuf[1]>>11)+1)>>1);							/* N-Tref AD Mean			*/
	 	AdMean[2] = (VAdBuf[1]<<20) | ((((ULONG)VAdBuf[0]>>11)+1)>>1);	/* Vref AD Mean				*/

//#endif	/* <S0BC> Delete:AP形限定の補助機能ではない為、削除 */

		return;

}

/****************************************************************************************************/
/*																									*/
/*		加算(64bit+32bit)																			*/
/*																									*/
/*		TODO: 後で消す																				*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
//void	MlibAdd6432( LONG *a, LONG b, LONG *x )			/* <S0BC> 同じ名称の関数が存在する為、関数名を変更 */
static void FnMlibAdd6432( LONG *a, LONG b, LONG *x )	/* <S0BC> 同じ名称の関数が存在する為、関数名を変更 */
{
//#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)	/* AnalogIF */	/* <S0BC> Delete:AP形限定の補助機能ではない為、削除 */

		ADD6432( a, b, x );
//#endif														/* <S0BC> Delete:AP形限定の補助機能ではない為、削除 */
		return;

}

/******************************* End of File *******************************/
