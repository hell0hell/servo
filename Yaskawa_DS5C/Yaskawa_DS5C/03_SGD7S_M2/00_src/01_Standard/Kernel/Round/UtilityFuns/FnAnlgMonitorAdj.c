/****************************************************************************
  Description   : アナログモニタ出力のマニュアルゼロ調整（Fn00C）
                : アナログモニタ出力のマニュアルゲイン調整（Fn00D）
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corporation.
                  Copyright (c) 2013 All Rights Reserved

  Project       : Mercury

  Create Date   : 2013-12-04

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
//#define KPI_RC_NOTADJ	0		/* 調整実施条件不成立 */
//#define KPI_RC_ADJOK	1		/* 調整完了 */
//#define KPI_RC_ADJERR	-1		/* 調整失敗 */
//#define OFFSET_ADJ		0		/* 電流検出オフセット調整 */

/* アナログモニタ出力のマニュアルゼロ調整　最大値/最小値 */
#define	ZADJAM_MAX			127
#define	ZADJAM_MIN			-128

/* アナログモニタ出力のマニュアルゲイン調整　最大値/最小値 */
#define	GADJAM_MAX			127
#define	GADJAM_MIN			-128


#define	OK	1					/* 実行可能 */
#define	NG	0					/* 実行不可能 */


/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/

/****************************************************************************
 Fn00C: Entry function of Analog Monitor Offset Manual Adjustment
****************************************************************************/
PRM_RSLT FnEnterAnlgMonOffsetsManualAdj(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	PRM_RSLT rc;
	FUN_CMN_CONTROL	*FnCmnCtrl;
	PRMDATA			*Prm;

	Prm = Axis->UniPrms->Prm;
	FnCmnCtrl = Axis->FnCmnCtrl;

	rc = PRM_RSLT_SUCCESS;

	/* ユーザ定数書込禁止 確認 */
	if(Prm->syssw3 & 0x0001)
	{
		rc = PRM_RSLT_RWACC_ERR;					/* アクセス制限エラー */
	}
	else
	{
//		FnCmnCtrl->KnlMode = KPI_KMD_ADJANLGMON;	/* アナログモニタ調整中  */
		FnCmnCtrl->KnlMode = KPI_KMD_NORMAL;		/* normal mode */			/* <S142> */
		FunExe->HoldFnMode = TRUE;					/* Fnモード自動終了なし */
		rc = PRM_RSLT_SUCCESS;
	}

	return (rc);
}

/****************************************************************************
 Fn00C: Execute function of Analog Monitor Offset Manual Adjustment
****************************************************************************/
PRM_RSLT FnExecuteAnlgMonOffsetsManualAdj(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	CMN_PRMDATA		*Cprm;
	PRMDATA			*Prm;
	PRM_RSLT		rc;
	LONG			mnaz_Ch1_tmp;
	LONG			mnaz_Ch2_tmp;
	UCHAR			Exe_flg;

	Prm = Axis->UniPrms->Prm;
	Cprm = Axis->UniPrms->Cprm;

	rc = PRM_RSLT_SUCCESS;
	Exe_flg = NG;

	/* ユーザ定数書込禁止 確認 */
#if 0
	if(Prm->syssw3 & 0x0001)
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

	switch( FunExe->State )
	{
	case 0:	/* アナログモニタ(CH1)ゼロ調用データ 調整	*/
		switch(FunExe->CmdCode)
		{
		case FCMD_UP:
			if(Exe_flg == OK)
			{
				/* PnE58:モニタ1,2ゼロ調(下位byte) */
				mnaz_Ch1_tmp = (LONG)(CHAR)((Cprm->mnaz0_1.b.l) & 0xFF);

				if(mnaz_Ch1_tmp < ZADJAM_MAX)
				{
					mnaz_Ch1_tmp += 1;
				}

				/* PnE58:モニタ1,2ゼロ調(下位byte)に更新した値をRAMへ書き込み */
				Cprm->mnaz0_1.b.l = (UCHAR)mnaz_Ch1_tmp;
			}
			else
			{
				rc = PRM_RSLT_RWACC_ERR;	/* アクセス制限エラー */
			}
			break;

		case FCMD_DOWN:
			if(Exe_flg == OK)
			{
				/* PnE58:モニタ1,2ゼロ調(下位byte) */
				mnaz_Ch1_tmp = (LONG)(CHAR)((Cprm->mnaz0_1.b.l) & 0xFF);

				if(mnaz_Ch1_tmp > ZADJAM_MIN)
				{
					mnaz_Ch1_tmp -= 1;
				}

				/* PnE58:モニタ1,2ゼロ調(下位byte)に更新した値をRAMへ書き込み */
				Cprm->mnaz0_1.b.l = (UCHAR)mnaz_Ch1_tmp;
			}
			else
			{
				rc = PRM_RSLT_RWACC_ERR;	/* アクセス制限エラー */
			}
			break;

		case FCMD_CH1:
			FunExe->State = 0;	/* アナログモニタ(CH1)ゼロ調用データ 調整 */
			break;

		case FCMD_CH2:
			FunExe->State = 1;	/* アナログモニタ(CH2)ゼロ調用データ 調整 */
			break;

		default:	/* other command */
			rc = PRM_RSLT_CONDITION_ERR;	/* 条件エラー */
			break;
		}
		break;

	case 1:	/* アナログモニタ(CH2)ゼロ調用データ 調整 */
		switch(FunExe->CmdCode)
		{
		case FCMD_UP:
			if(Exe_flg == OK)
			{
				/* PnE58:モニタ1,2ゼロ調(上位byte) */
				mnaz_Ch2_tmp = (LONG)(CHAR)((Cprm->mnaz0_1.b.h) & 0xFF);

				if(mnaz_Ch2_tmp < ZADJAM_MAX)
				{
					mnaz_Ch2_tmp += 1;
				}

				/* PnE58:モニタ1,2ゼロ調(上位byte)に更新した値をRAMへ書き込み */
				Cprm->mnaz0_1.b.h = (UCHAR)mnaz_Ch2_tmp;
			}
			else
			{
				rc = PRM_RSLT_RWACC_ERR;	/* アクセス制限エラー */
			}
			break;

		case FCMD_DOWN:
			if(Exe_flg == OK)
			{
				/* PnE58:モニタ1,2ゼロ調(上位byte) */
				mnaz_Ch2_tmp = (LONG)(CHAR)((Cprm->mnaz0_1.b.h) & 0xFF);

				if(mnaz_Ch2_tmp > ZADJAM_MIN)
				{
					mnaz_Ch2_tmp -= 1;
				}

				/* PnE58:モニタ1,2ゼロ調(上位byte)に更新した値をRAMへ書き込み */
				Cprm->mnaz0_1.b.h = (UCHAR)mnaz_Ch2_tmp;
			}
			else
			{
				rc = PRM_RSLT_RWACC_ERR;	/* アクセス制限エラー */
			}
			break;

		case FCMD_CH1:
			FunExe->State = 0;	/* アナログモニタ(CH1)ゼロ調用データ 調整 */
			break;

		default:	/* other command */
			rc = PRM_RSLT_CONDITION_ERR;	/* 条件エラー */
			break;
		}
		break;

	default:
		break;
	}

	return rc ;

}

/****************************************************************************
 Fn00C: End function of Analog Monitor Offset Manual Adjustment
****************************************************************************/
void FnLeaveAnlgMonOffsetsManualAdj(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	FUN_CMN_CONTROL	*FnCmnCtrl;
	REGIF_CONFIG_T	*RegCfg;

	FnCmnCtrl = Axis->FnCmnCtrl;
	RegCfg = Axis->RegManager->hPnReg;

	/* 終了処理		*/
	/* EEPROM更新処理	*/
	PrmStoreValueToEeprom( RegCfg, &pndef_mnaz0_1, 0 );		/* PnE58:モニタ1,2ゼロ調 */

	/* 終了処理	*/
	FnCmnCtrl->KnlMode = KPI_KMD_NORMAL;					/* normal mode */
}

/****************************************************************************
 Fn00D: Entry function of Analog Monitor Gain Manual Adjustment
****************************************************************************/
PRM_RSLT FnEnterAnlgMonGainManualAdj(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	PRM_RSLT rc;
	FUN_CMN_CONTROL	*FnCmnCtrl;
	PRMDATA			*Prm;

	Prm = Axis->UniPrms->Prm;
	FnCmnCtrl = Axis->FnCmnCtrl;

	rc = PRM_RSLT_SUCCESS;

	/* ユーザ定数書込禁止 確認 */
	if(Prm->syssw3 & 0x0001)
	{
		rc = PRM_RSLT_RWACC_ERR;					/* アクセス制限エラー */
	}
	else
	{
//		FnCmnCtrl->KnlMode = KPI_KMD_ADJANLGMON;	/* アナログモニタ調整中  */
		FnCmnCtrl->KnlMode = KPI_KMD_NORMAL;		/* normal mode */			/* <S142> */
		FunExe->HoldFnMode = TRUE;					/* Fnモード自動終了なし */
		rc = PRM_RSLT_SUCCESS;
	}


	return (rc);
}

/****************************************************************************
 Fn00D: Execute function of Analog Monitor Gain Manual Adjustment
****************************************************************************/
PRM_RSLT FnExecuteAnlgMonGainManualAdj(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	CMN_PRMDATA		*Cprm;
	PRMDATA			*Prm;
	PRM_RSLT		rc;
	LONG			mnag_Ch1_tmp;
	LONG			mnag_Ch2_tmp;
	UCHAR			Exe_flg;

	Prm = Axis->UniPrms->Prm;
	Cprm = Axis->UniPrms->Cprm;

	rc = PRM_RSLT_SUCCESS;
	Exe_flg = NG;

	/* ユーザ定数書込禁止 確認 */
#if 0
	if(Prm->syssw3 & 0x0001)
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

	switch( FunExe->State )
	{
	case 0:	/* アナログモニタ(CH1)ゲイン調用データ 調整	*/
		switch(FunExe->CmdCode)
		{
		case FCMD_UP:
			if(Exe_flg == OK)
			{
				/* PnE59:モニタ1,2ゲイン調(下位byte) */
				mnag_Ch1_tmp = (LONG)(CHAR)((Cprm->mnag0_1.b.l) & 0xFF);

				if(mnag_Ch1_tmp < GADJAM_MAX)
				{
					mnag_Ch1_tmp += 1;
				}

				/* PnE59:モニタ1,2ゲイン調(下位byte)に更新した値をRAMへ書き込み */
				Cprm->mnag0_1.b.l = (UCHAR)mnag_Ch1_tmp;
			}
			else
			{
				rc = PRM_RSLT_RWACC_ERR;	/* アクセス制限エラー */
			}
			break;

		case FCMD_DOWN:
			if(Exe_flg == OK)
			{
				/* PnE59:モニタ1,2ゲイン調(下位byte) */
				mnag_Ch1_tmp = (LONG)(CHAR)((Cprm->mnag0_1.b.l) & 0xFF);

				if(mnag_Ch1_tmp > GADJAM_MIN)
				{
					mnag_Ch1_tmp -= 1;
				}

				/* PnE59:モニタ1,2ゲイン調(下位byte)に更新した値をRAMへ書き込み */
				Cprm->mnag0_1.b.l = (UCHAR)mnag_Ch1_tmp;
			}
			else
			{
				rc = PRM_RSLT_RWACC_ERR;	/* アクセス制限エラー */
			}
			break;

		case FCMD_CH1:
			FunExe->State = 0;	/* アナログモニタ(CH1)ゲイン調用データ 調整 */
			break;

		case FCMD_CH2:
			FunExe->State = 1;	/* アナログモニタ(CH2)ゲイン調用データ 調整 */
			break;

		default:	/* other command */
			rc = PRM_RSLT_CONDITION_ERR;	/* 条件エラー */
			break;
		}
		break;

	case 1:	/* アナログモニタ(CH2)ゲイン調用データ 調整 */
		switch(FunExe->CmdCode)
		{
		case FCMD_UP:
			if(Exe_flg == OK)
			{
				/* PnE59:モニタ1,2ゲイン調(上位byte) */
				mnag_Ch2_tmp = (LONG)(CHAR)((Cprm->mnag0_1.b.h) & 0xFF);

				if(mnag_Ch2_tmp < GADJAM_MAX)
				{
					mnag_Ch2_tmp += 1;
				}

				/* PnE59:モニタ1,2ゲイン調(上位byte)に更新した値をRAMへ書き込み */
				Cprm->mnag0_1.b.h = (UCHAR)mnag_Ch2_tmp;
			}
			else
			{
				rc = PRM_RSLT_RWACC_ERR;	/* アクセス制限エラー */
			}
			break;

		case FCMD_DOWN:
			if(Exe_flg == OK)
			{
				/* PnE59:モニタ1,2ゲイン調(上位byte) */
				mnag_Ch2_tmp = (LONG)(CHAR)((Cprm->mnag0_1.b.h) & 0xFF);

				if(mnag_Ch2_tmp > GADJAM_MIN)
				{
					mnag_Ch2_tmp -= 1;
				}

				/* PnE59:モニタ1,2ゲイン調(上位byte)に更新した値をRAMへ書き込み */
				Cprm->mnag0_1.b.h = (UCHAR)mnag_Ch2_tmp;
			}
			else
			{
				rc = PRM_RSLT_RWACC_ERR;	/* アクセス制限エラー */
			}
			break;

		case FCMD_CH1:
			FunExe->State = 0;	/* アナログモニタ(CH1)ゲイン調用データ 調整 */
			break;

		default:	/* other command */
			rc = PRM_RSLT_CONDITION_ERR;	/* 条件エラー */
			break;
		}
		break;

	default:
		break;
	}

	return rc ;

}

/****************************************************************************
 Fn00D: End function of Analog Monitor Gain Manual Adjustment
****************************************************************************/
void FnLeaveAnlgMonGainManualAdj(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	FUN_CMN_CONTROL	*FnCmnCtrl;
	REGIF_CONFIG_T	*RegCfg;

	FnCmnCtrl = Axis->FnCmnCtrl;
	RegCfg = Axis->RegManager->hPnReg;

	/* 終了処理		*/
	/* EEPROM更新処理	*/
	PrmStoreValueToEeprom( RegCfg, &pndef_mnag0_1, 0 );		/* PnE59:モニタ1,2ゼロ調 */
	/* 終了処理	*/
	FnCmnCtrl->KnlMode = KPI_KMD_NORMAL;					/* normal mode */
}


/******************************* End of File *******************************/


