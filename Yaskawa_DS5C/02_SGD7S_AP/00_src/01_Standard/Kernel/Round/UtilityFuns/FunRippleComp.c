/****************************************************************************
  Description   : 速度リップル補償情報書き込みモード（200DH）
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corporation.
                  Copyright (c) 2014 All Rights Reserved

  Project       : Mercury

  Create Date   : 2014-02-08

****************************************************************************/
#include "Basedef.h"
#include "MercuryGlobals.h"
#include "FunManager.h"
#include "PnPrmListTbl.h"

/****************************************************************************/
/*		DEFINES																*/
/****************************************************************************/
#define	MOTOR_SN_DATA_SIZE			8		/* モータシリアルNo.データサイズ[word]		*/
#define	RI_PCOMP_NUM_ELE			6		/* 速度リップル補償情(電気角)成分数			*/
#define	EEPROM_WRITE_COMPLETE		0x0000	/* 速度リップル補償情報書込み確認 : 完了	*/
#define	EEPROM_WRITE_NOTCOMPLETE	0x0000	/* 速度リップル補償情報書込み確認 : 未完了	*/
#define	EEPROM_WRITE_ERROR			0x0002	/* 速度リップル補償情報書込み確認 : 失敗	*/


/****************************************************************************/
/*		PROTOTYPE															*/
/****************************************************************************/
static LONG WriteRippleCompPrmToEeprom( AXIS_HANDLE *Axis );
static LONG WriteMotorSerialNoToEeprom( AXIS_HANDLE *Axis );
static void WriteRippleCompDataToRam( AXIS_HANDLE *Axis );	/* <S163> */

/*******************************************************************************
 200DH: Entry function of Velocity Ripple Compensation Information Write Mode
********************************************************************************/
PRM_RSLT FnEnterRippleComp(FUNEXE *FunExe, AXIS_HANDLE *Axis)/*<S1FD>*/
{
//	PRM_RSLT rc;
//	FUN_CMN_CONTROL	*FnCmnCtrl;/* <S1FD> */
	RPLCMP			*RippleCmp;
	PRMDATA			*Prm;
	REG_ACC_T		AccessPrm;

//	FnCmnCtrl = Axis->FnCmnCtrl;
	RippleCmp = &(Axis->BaseLoops->RippleCmp);
	Prm       = Axis->UniPrms->Prm;

	/* RAM書き込み用レジスタ情報設定 */
	AccessPrm.MemAddr = 0;
	AccessPrm.hRegCfg = Axis->RegManager->hPnReg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

	/* 200Dh実行時は強制的に速度リップル補償機能を有効にする */
	RippleCmp->RplCmpPrm.RippleLstWrnMode = RippleCmp->RplCmpPrm.RippleWrnMode; /* 前回値保存 */
	RippleCmp->RplCmpPrm.RippleWrnMode    = TRUE/*FALSE*/;

	/* Pn423再計算処理 */
	TunePrmWriteRam( &AccessPrm, &pndef_RippleCompSw, (LONG)Prm->RippleCompSw, 2 );

	FunExe->HoldFnMode = TRUE;								/* Fnモード自動終了なし */
//	FnCmnCtrl->KnlMode = KPI_KMD_RIPPLECOMPWRITE;			/* 速度リップル補償情報書き込み中 *//* <S1FD> */
	RippleCmp->RippleCmpState = EEPROM_WRITE_NOTCOMPLETE;	/* 速度リップル補償情報書込み確認 = 01H:未完了 */

//	rc = PRM_RSLT_SUCCESS;
//	return (rc);
	return PRM_RSLT_SUCCESS;
}

/*******************************************************************************
 200DH: Execute function of Velocity Ripple Compensation Information Write Mode
********************************************************************************/
PRM_RSLT FnExecuteRippleComp(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	PRM_RSLT			errRes;
	LONG				WriteRslt;
//	MENCV				*MencV;		/*<S1FD>*/
//	FUN_CMN_CONTROL		*FnCmnCtrl;	/*<S1FD>*/
	RPLCMP				*RippleCmp;

//	FnCmnCtrl = Axis->FnCmnCtrl;	/*<S1FD>*/
//	MencV = Axis->MencV;			/*<S1FD>*/
	RippleCmp = &(Axis->BaseLoops->RippleCmp);

	switch(FunExe->State)
	{
	case 0:
		switch(FunExe->CmdCode)
		{
/* <S163> Start */
		case FCMD_EXEC:	/* RAM書込み処理	*/
			errRes = PRM_RSLT_SUCCESS;			/* 未初期化変数修正<S191> */
			WriteRippleCompDataToRam( Axis );
			break;
/* <S163> End */

		case FCMD_UP:	/* 保護処理	*/
			errRes = PRM_RSLT_SUCCESS;
			FunExe->State = 1;
			break;
		case FCMD_STOP:
		case FCMD_NONE:
			errRes = PRM_RSLT_SUCCESS;
			break;
		default:
			FunExe->HoldFnMode = FALSE;	/* Escape Fn mode */
			errRes = PRM_RSLT_CONDITION_ERR;
			break;
		}
		break;

	case 1:	/* 実行要求待ち */
		switch(FunExe->CmdCode)
		{
		case FCMD_EWRT:	/* EEPROM書込み処理	*/
			FunExe->State = 2;
			errRes = PRM_RSLT_CONTINUE;
			break;

		case FCMD_STOP:
		case FCMD_NONE:
			errRes = PRM_RSLT_SUCCESS;
			break;
		default:
			FunExe->HoldFnMode = FALSE;	/* Escape Fn mode */
			errRes = PRM_RSLT_CONDITION_ERR;
			break;
		}
		break;

	case 2:	/* 速度リップル補償情報書き込み処理 */

			/* 速度リップル補償用パラメータEEPROMに書き込み */
			WriteRslt = WriteRippleCompPrmToEeprom( Axis );

			if( WriteRslt == 0 )
			{/* EEPROMへの書き込み正常完了  */
				errRes = PRM_RSLT_SUCCESS;
				RippleCmp->RippleCmpState = EEPROM_WRITE_COMPLETE;	/* 速度リップル補償情報書込み確認 = 00H:完了 */
				FunExe->State = 1;
			}
			else
			{/* EEPROMへの書き込み異常完了  */
				errRes = PRM_RSLT_CONDITION_ERR;
				RippleCmp->RippleCmpState = EEPROM_WRITE_ERROR;		/* 速度リップル補償情報書込み確認 = 02H:失敗 */
			}

			/* 接続モータのシリアル番号EEPROMに書き込み */
			WriteRslt = WriteMotorSerialNoToEeprom( Axis );

			if( WriteRslt == 0 )
			{/* EEPROMへの書き込み正常完了  */
				errRes = PRM_RSLT_SUCCESS;
				FunExe->State = 1;
				RippleCmp->RippleCmpState = EEPROM_WRITE_COMPLETE;	/* 速度リップル補償情報書込み確認 = 00H:完了 */
			}
			else
			{/* EEPROMへの書き込み異常完了  */
				errRes = PRM_RSLT_CONDITION_ERR;
				RippleCmp->RippleCmpState = EEPROM_WRITE_ERROR;		/* 速度リップル補償情報書込み確認 = 02H:失敗 */
			}

		break;

	default:
		errRes = PRM_RSLT_SUCCESS;
		break;
	}

	return errRes;
}

/*******************************************************************************
 200DH: End function of Velocity Ripple Compensation Information Write Mode
********************************************************************************/
void FnLeaveRippleComp(FUNEXE *FunExe, AXIS_HANDLE *Axis)/* <S1FD> */
{
//	FUN_CMN_CONTROL	*FnCmnCtrl;
//	REGIF_CONFIG_T	*RegCfg;					/* <S1FD> */
	RPLCMP			*RippleCmp;
	PRMDATA			*Prm;
	REG_ACC_T		AccessPrm;

//	FnCmnCtrl = Axis->FnCmnCtrl;
	RippleCmp = &(Axis->BaseLoops->RippleCmp);
	Prm       = Axis->UniPrms->Prm;

	/* RAM書き込み用レジスタ情報設定 */
	AccessPrm.MemAddr = 0;
	AccessPrm.hRegCfg = Axis->RegManager->hPnReg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

	/* Pn423を200Dh実行前の状態に戻す */
	RippleCmp->RplCmpPrm.RippleWrnMode = RippleCmp->RplCmpPrm.RippleLstWrnMode; /* 前回値を戻す */

	/* Pn423再計算処理 */
	TunePrmWriteRam( &AccessPrm, &pndef_RippleCompSw, (LONG)Prm->RippleCompSw, 2 );

	FunExe->HoldFnMode = FALSE;				/* Escape Fn mode */
//	FnCmnCtrl->KnlMode = KPI_KMD_NORMAL;	/* normal mode *//* <S1FD> */
}

/* <S163> Start */
/****************************************************************************************************/
/*																									*/
/*		速度リップル補償用成分をRAMに書き込み処理													*/
/*																									*/
/*			機 能 :	SigmaWin+にて算出され、レジスタに書き込まれた									*/
/*					「速度リップル補償用成分」をRAMへ書き込む。										*/
/*																									*/
/****************************************************************************************************/
static void WriteRippleCompDataToRam( AXIS_HANDLE *Axis )
{
	PRMDATA			*Prm;
	RPLCMP			*RippleCmp;
	LONG			i;

	/* 初期化 */
	Prm = Axis->UniPrms->Prm;
	RippleCmp = &(Axis->BaseLoops->RippleCmp);

	/*----------------------------------------------------------------------------------------------*/
	/*		リップル補正パラメータコピー(レジスタ⇒RAM)												*/
	/*----------------------------------------------------------------------------------------------*/
	Prm->RippleCompGainE = RippleCmp->RplCmpPrm.RplCompGainEle;

	for( i = 0; i < RI_PCOMP_NUM_ELE; i++ )
	{	/* 電気角補正 */
		Prm->RippleCompCycleE[i]     = RippleCmp->RplCmpPrm.RplCompCycleEle[i];
		Prm->RippleCompAmplitudeE[i] = RippleCmp->RplCmpPrm.RplCompAmplitudeEle[i];
		Prm->RippleCompPhaseE[i]     = RippleCmp->RplCmpPrm.RplCompPhaseEle[i];
	}

	return;
}
/* <S163> End */

/****************************************************************************************************/
/*																									*/
/*		速度リップル補償用パラメータEEPROMに書き込み処理											*/
/*																									*/
/*			機 能 :	SigmaWin+にて算出され、PnF**へ書き込まれた										*/
/*					「速度リップル補償用パラメータ」をEepRomへ書き込む。							*/
/*																									*/
/****************************************************************************************************/
static LONG WriteRippleCompPrmToEeprom( AXIS_HANDLE *Axis )
{
	LONG			rc;
	REGIF_CONFIG_T	*RegCfg;
//	PRMDATA			*Prm;		/* <S163> Delete */
//	RPLCMP			*RippleCmp;	/* <S1FD> */
//	LONG			i;			/* <S1FD> */

	/* 初期化 */
	rc = PRM_RSLT_SUCCESS;
	RegCfg = Axis->RegManager->hPnReg;
//	Prm = Axis->UniPrms->Prm;					/* <S163> Delete */
//	RippleCmp = &(Axis->BaseLoops->RippleCmp);	/* <S1FD> */

/* <S163> Start */
/* リップル補正パラメータコピー(レジスタ⇒RAM)は「操作設定レジスタ(42H：0x80004002/40H：0x2001) = 0x0001(実行)」で実行するように仕様変更。 */
#if 0
	/*----------------------------------------------------------------------------------------------*/
	/*		リップル補正パラメータコピー(レジスタ⇒RAM)												*/
	/*----------------------------------------------------------------------------------------------*/
	Prm->RippleCompGainE = RippleCmp->RplCmpPrm.RplCompCycleEle[i];

	for( i = 0; i < RI_PCOMP_NUM_ELE; i++ )
	{	/* 電気角補正 */
		Prm->RippleCompCycleE[i] = RippleCmp->RplCmpPrm.RplCompCycleEle[i];
		Prm->RippleCompAmplitudeE[i] = RippleCmp->RplCmpPrm.RplCompAmplitudeEle[i];
		Prm->RippleCompPhaseE[i] = RippleCmp->RplCmpPrm.RplCompPhaseEle[i];
	}
#endif
/* <S163> End */

	/*----------------------------------------------------------*/
	/*		EEPROMに書き込み処理								*/
	/*----------------------------------------------------------*/
	/* PnFA0 速度リップル補償ゲイン(電気角) */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_RippleCompGainE, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* PnFA1 速度リップル補償第1成分周期(電気角) */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_RippleCompCycle1, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* PnFA2 速度リップル補償第1成分振幅値(電気角) */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_RippleCompAmplitude1, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* PnFA3 速度リップル補償第1成分位相(電気角) */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_RippleCompPhase1, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* PnFA4 速度リップル補償第2成分周期(電気角) */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_RippleCompCycle2, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* PnFA5 速度リップル補償第2成分振幅値(電気角) */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_RippleCompAmplitude2, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* PnFA6 速度リップル補償第2成分位相(電気角) */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_RippleCompPhase2, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* PnFA7 速度リップル補償第3成分周期(電気角) */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_RippleCompCycle3, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* PnFA8 速度リップル補償第3成分振幅値(電気角) */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_RippleCompAmplitude3, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* PnFA9 速度リップル補償第3成分位相(電気角) */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_RippleCompPhase3, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* PnFAA 速度リップル補償第4成分周期(電気角) */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_RippleCompCycle4, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* PnFAB 速度リップル補償第4成分振幅値(電気角) */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_RippleCompAmplitude4, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* PnFAC 速度リップル補償第4成分位相(電気角) */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_RippleCompPhase4, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* PnFAD 速度リップル補償第5成分周期(電気角) */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_RippleCompCycle5, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* PnFAE 速度リップル補償第5成分振幅値(電気角) */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_RippleCompAmplitude5, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* PnFAF 速度リップル補償第5成分位相(電気角) */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_RippleCompPhase5, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* PnFB0 速度リップル補償第6成分周期(電気角) */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_RippleCompCycle6, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* PnFB1 速度リップル補償第6成分振幅値(電気角) */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_RippleCompAmplitude6, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* PnFB2 速度リップル補償第6成分位相(電気角) */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_RippleCompPhase6, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	return(rc);
}

/****************************************************************************************************/
/*																									*/
/*		接続モータのシリアル番号EEPROMに書き込み処理												*/
/*																									*/
/*			機 能 :	接続モータのシリアル番号をEepRomへ書き込む。									*/
/*																									*/
/****************************************************************************************************/
static LONG WriteMotorSerialNoToEeprom( AXIS_HANDLE *Axis )
{

	LONG	rc;
//	EEP_DEVICE_HANDLE	*EepDevice;/* <Oka02> */
	UINT				AxisNo;
	USHORT				SerialNo[MOTOR_SN_DATA_SIZE];
	LONG				ArrayIdx;

	/* 初期化 */
//	EepDevice = (Axis->RegManager->hPnReg)->hEepDev;/* <Oka02> */
	AxisNo = Axis->AxisNo;
	rc = PRM_RSLT_SUCCESS;

	/* モータシリアルNo.取得 */
	for(ArrayIdx = 0;  ArrayIdx < MOTOR_SN_DATA_SIZE; ArrayIdx++)
	{
		SerialNo[ArrayIdx] = (USHORT)(Axis->MencV->MotInfo.SNo[ArrayIdx*2 + 1] & 0x00FF);
		SerialNo[ArrayIdx] |= (USHORT)((Axis->MencV->MotInfo.SNo[ArrayIdx*2] & 0x00FF) << 8);
	}

	/* モータシリアルNo.をEepRomへ書き込み */
	for(ArrayIdx = 0;  ArrayIdx < MOTOR_SN_DATA_SIZE; ArrayIdx++)
	{
		/* write to EEPROM */
		rc = EepdevWriteNoSumValues( 0,															/* EEPROM Device ID(0: SPI(IF), 1: I2C(SDA) */
									 EEP_NOSUM_ADDRESS(AxisNo, EEP_MOTORSN_OFS) + (ArrayIdx*2),	/* Eeprom Address in bytes */
									 &SerialNo[ArrayIdx],										/* Pointer to the buffer for writing */
									 1 );														/* writing size in words */

		if( rc != 0 )	/* EEPROMへの書き込み失敗 */
		{
			break;
		}

	}

	return (rc) ;
}
/***************************************** end of file **********************************************/
