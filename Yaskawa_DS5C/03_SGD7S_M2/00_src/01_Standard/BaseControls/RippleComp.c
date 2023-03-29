/****************************************************************************************************/
/*																									*/
/*																									*/
/*		RippleComp.c : リップル補正　モジュール														*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	初版	2013.09.25	Y.Oka	For Mercury												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include	"RippleComp.h"
#include 	"BaseCmnStruct.h"

/****************************************************************************************************/
/*																									*/
/*		定数定義																					*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		リップル補正次数定義																		*/
/*--------------------------------------------------------------------------------------------------*/
#define	RIPCOMPORDER_MECHA		RPLCMP_ORDER_MECHA		/* リップル補正最大次数					*/
#define	RIPCOMPORDER_ELE		RPLCMP_ORDER_ELE		/* リップル補正最大次数					*/

/*--------------------------------------------------------------------------------------------------*/
/*		リップルスペクトル解析モード																*/
/*--------------------------------------------------------------------------------------------------*/
#define	RPLSP_ANALYZE_STOP	0					/* リップルスペクトル測定停止					*/
#define	RPLSP_ANALYZE_SPD	2					/* 速度リップルスペクトル測定					*/
#define	RPLSP_ANALYZE_TRQ	3					/* トルクリップルスペクトル測定					*/

/*--------------------------------------------------------------------------------------------------*/
/*		 速度リップル補償情報書き込みモード															*/
/*--------------------------------------------------------------------------------------------------*/
#define	MOTOR_SN_DATA_SIZE	8					/* モータシリアルNo.データサイズ[word]			*/
#define	SN_READ_SUCCESS		0x0000				/* モータシリアルNo.読出し正常完了				*/
#define	SN_READ_ERROR		0x0001				/* モータシリアルNo.読出し異常完了				*/
#define	SN_DIFFERENT		0x0002				/* モータシリアルNo.が異なる					*/


/****************************************************************************************************/
/*		Local Function Prototype Definition															*/
/****************************************************************************************************/


/****************************************************************************************************/
/*																									*/
/*		速度リップル補償																			*/
/*																									*/
/*	概要:	パラメータに保存されたリップル情報(振幅、位相)を基に正弦波補償トルク信号を作成し、		*/
/*			補償トルク信号をトルク指令にＦＦ項として加算することによって速度リップルを低減する		*/
/*																									*/
/*			補償トルク	= Σ(補正振幅 * sin(モータ位相 * 次数 + 位相補正値))						*/
/*																									*/
/* arg		: USHORT	MotPhase		:	モータ位相			[65536/360deg]						*/
/* out		: LONG		TrqComp			:	トルク補正値		[2^24/MaxTrq]						*/
/* g-out	: 																						*/
/*																									*/
/****************************************************************************************************/
LONG	RippleCompensation( RPLCMP *RippleCmp, USHORT MechaPhase, USHORT MotPhase, BOOL BaseEnable )
{
	RPLCMP_PRM			*RippleCmpP;
//	RPLCMP_PRM_MECHA 	*RippleCmpP_mecha;	/*<S1FD>*/
	RPLCMP_PRM_ELE 		*RippleCmpP_ele;

	LONG				i, lwk, TrqComp;
//	LONG				TrqCompMecha		/*<S1FD>*/
	LONG				TrqCompEle;
	USHORT				uswk[RPLCMP_ORDER_MECHA];
	BOOL				RippleCompMon;

	RippleCmpP       = &(RippleCmp->RplCmpPrm);
//	RippleCmpP_mecha = &(RippleCmp->RplCmpPrm.RplCmpPrmMecha);/*<S1FD>*/
	RippleCmpP_ele   = &(RippleCmp->RplCmpPrm.RplCmpPrmEle);

/*--------------------------------------------------------------------------------------------------*/
/*		機械角補正																					*/
/*--------------------------------------------------------------------------------------------------*/
#if 0	/* 速度リップル補償 機械角補正処理(将来用としてコメントアウト) */
	lwk = 0;
	for(i=0; i<RIPCOMPORDER_MECHA; i++)
	{
		/*------------------------------------------------------------------------------------------*/
		/*		位相計算																			*/
		/*------------------------------------------------------------------------------------------*/
		uswk[i]  = RippleCmpP_mecha->PhaseOfset[i]  + (MechaPhase * RippleCmpP_mecha->CompFrq[i]);
		uswk[i]  = uswk[i] >> 6;									/* [65536/360] --> [1024/360]	*/

		/*------------------------------------------------------------------------------------------*/
		/*		補正トルク計算																		*/
		/*																							*/
		/*	[15000/MaxTrq]への正規化には >>14 が必要だが、処理時間対策のため、						*/
		/*	[2^24/ MaxTrq]への正規化時にまとめてシフト演算する										*/
		/*------------------------------------------------------------------------------------------*/
		lwk += (RippleCmpP_mecha->CompAmpValue[i] * MlibFASTSINS16(uswk[i]));
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		補正トルク正規化計算[15000/MaxTrq] -> [2^24/MaxTrq]										*/
	/*																								*/
	/*		MlibMulhigh32を用いて15000->2^24正規化変換を行うには <<16 が必要だが上の処理で			*/
	/*		本来処理すべき >>14 を実施していないのでトータルで <<2 を行う(処理時間対策)				*/
	/*----------------------------------------------------------------------------------------------*/
	lwk = MlibMulhigh32( (lwk << 2), 73300775 );					/*	[2^24/MaxTrq]				*/
	TrqCompMecha = MlibMulgainNolim( lwk, RippleCmpP_mecha->CompGain );
#endif	/* 速度リップル補償 機械角補正処理(将来用としてコメントアウト) */

	lwk = 0;

/*--------------------------------------------------------------------------------------------------*/
/*		電気角補正																					*/
/*--------------------------------------------------------------------------------------------------*/
	for(i=0; i<RIPCOMPORDER_ELE; i++)
	{
		/*------------------------------------------------------------------------------------------*/
		/*		位相計算																			*/
		/*------------------------------------------------------------------------------------------*/
		uswk[i]  = RippleCmpP_ele->PhaseOfset[i]  + (MotPhase * RippleCmpP_ele->CompFrq[i]);
		uswk[i]  = uswk[i] >> 6;									/* [65536/360] --> [1024/360]	*/

		/*------------------------------------------------------------------------------------------*/
		/*		補正トルク計算																		*/
		/*																							*/
		/*	[15000/MaxTrq]への正規化には >>14 が必要だが、処理時間対策のため、						*/
		/*	[2^24/ MaxTrq]への正規化時にまとめてシフト演算する										*/
		/*------------------------------------------------------------------------------------------*/
		lwk += (RippleCmpP_ele->CompAmpValue[i] * MlibFASTSINS16(uswk[i]));

	}

#if 0
	lwk = RippleCmp->var.TrqComp[0] + RippleCmp->var.TrqComp[1] + RippleCmp->var.TrqComp[5];
	if(RippleCmpP->RplCmpMode6f == TRUE)
	{ /* 1～6次使用モード選択時 */
		lwk = lwk + RippleCmp->var.TrqComp[2] + RippleCmp->var.TrqComp[3] + RippleCmp->var.TrqComp[4];
	}
#endif
	/*----------------------------------------------------------------------------------------------*/
	/*		補正トルク正規化計算[15000/MaxTrq] -> [2^24/MaxTrq]										*/
	/*																								*/
	/*		MlibMulhigh32を用いて15000->2^24正規化変換を行うには <<16 が必要だが上の処理で			*/
	/*		本来処理すべき >>14 を実施していないのでトータルで <<2 を行う(処理時間対策)				*/
	/*----------------------------------------------------------------------------------------------*/
	lwk = MlibMulhigh32( (lwk << 2), 73300775 );					/*	[2^24/MaxTrq]				*/
	TrqCompEle = MlibMulgainNolim( lwk, RippleCmpP_ele->CompGain );

/*--------------------------------------------------------------------------------------------------*/
/*		補正トルク計算	（機械角補正トルク ＋ 電気角補正トルク）									*/
/*--------------------------------------------------------------------------------------------------*/
//	RippleCmp->var.TrqCompAll = TrqCompMecha + TrqCompEle;	// 速度リップル補償 機械角補正処理(将来用としてコメントアウト)
	RippleCmp->var.TrqCompAll = TrqCompEle;

/*--------------------------------------------------------------------------------------------------*/
/*		補正実行判定																				*/
/*--------------------------------------------------------------------------------------------------*/
//	if( RippleCmpP->RplCmpUse == TRUE)/*<S1FD>*/
	if( (RippleCmpP->RplCmpUse == TRUE) && (RippleCmp->var.RippleCmpExe == TRUE) )
	{	/* 補正有効	*/
		TrqComp = RippleCmp->var.TrqCompAll;
		RippleCompMon = TRUE;
	}
	else
	{	/* 補正無効	*/
		TrqComp = 0;
		RippleCompMon = FALSE;
	}
/*--------------------------------------------------------------------------------------------------*/
/*		ベースブロック時の処理																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( BaseEnable == FALSE )
	{
		RippleCmp->var.TrqCompAll = 0;
		TrqComp = 0;
		RippleCompMon = FALSE;
	}
/*--------------------------------------------------------------------------------------------------*/
/*		速度リップル補償モニタ格納																	*/
/*--------------------------------------------------------------------------------------------------*/
	RippleCmp->RippleCompMon    = RippleCompMon;
	RippleCmp->RippleTrqCompMon = TrqComp;

	return(TrqComp);

}

/****************************************************************************************************/
/*																									*/
/*		速度リップル補償機能の有効条件チェック														*/
/*																									*/
/****************************************************************************************************/
void	RippleCompChkThresholdVal( RPLCMP *RippleCmp, USHORT CtrlMode, LONG SpdRef, LONG SpdFb, BOOL TuneLessCtrl )
{
	USHORT	RippleCmpThresholdSel;
	BOOL	RippleCmpExe;

	RippleCmpExe = FALSE;
	/*----------------------------------------------------------------------------------------------*/
	/*	速度リップル補償機能の有効化しきい値条件(Pn423.2=0(指令速度)/1(フィードバック速度))			*/
	/*		・位置制御モード	：Pn423.2に従う														*/
	/*		・速度制御モード	：Pn423.2に従う														*/
	/*		・トルク制御モード	：強制的に1(フィードバック速度)にする								*/
	/*----------------------------------------------------------------------------------------------*/
	if( CtrlMode== CTRLMODE_TRQ )	/* トルク制御モード */
	{
		RippleCmpThresholdSel = FB_SPD;
	}
	else
	{
		RippleCmpThresholdSel = RippleCmp->RippleCmpThresholdSel;
	}
	/*----------------------------------------------------------------------------------------------*/
	/*	符号処理																					*/
	/*----------------------------------------------------------------------------------------------*/
	if( SpdRef < 0 )
	{
		SpdRef = -SpdRef;
	}
	if( SpdFb < 0 )
	{
		SpdFb = -SpdFb;
	}
	/*----------------------------------------------------------------------------------------------*/
	/*	判定処理																					*/
	/*----------------------------------------------------------------------------------------------*/
	if( RippleCmp->VelRipplePhaseReady == TRUE ) 	/* 磁極検出完了(位相検出完了) */
	{
		if( RippleCmpThresholdSel == CMD_SPD )
		{	/* 速度リップル補償機能の有効化しきい値条件 = 「指令速度」 */
			if( RippleCmp->VelRippleCompSpd <= SpdRef )
			{
				RippleCmpExe = TRUE;
			}
			else
			{
				RippleCmpExe = FALSE;
			}
		}
		else
		{/* 速度リップル補償機能の有効化しきい値条件 = 「フィードバック速度」 */
			if( RippleCmp->VelRippleCompSpd <= SpdFb )
			{
				RippleCmpExe = TRUE;
			}
			else
			{
				RippleCmpExe = FALSE;
			}
		}
	}
	else	/* 磁極検出未完了(位相検出未完了) */
	{
		RippleCmpExe = FALSE;
	}

	/* 調整レス有効時は速度リップル補償しない */
	if(TuneLessCtrl == TRUE)
	{
		RippleCmpExe = FALSE;
	}

	/* 有効条件チェック結果を格納 */
	RippleCmp->var.RippleCmpExe = RippleCmpExe;
}

#if 0	/* モニタ出力選択 *//* @@現状未対応 */
/****************************************************************************************************/
/*																									*/
/*		リップル補正モニタ出力選択取得API															*/
/*																									*/
/****************************************************************************************************/
LONG	RippleCompGetMonOutSel( RPLCMP_PRM *RippleCmpP )
{
	return	(RippleCmpP->RplCmpDobsMon);
}
#endif	/* モニタ出力選択 *//* @@現状未対応 */

/****************************************************************************************************/
/*																									*/
/*		速度リップル補償パラメータ計算(電気角)														*/
/*																									*/
/****************************************************************************************************/
BOOL	CalVelRippleCompE( RPLCMP *RippleCmp, BPRMDAT *Bprm, USHORT RippleCompGainE, 
							USHORT *RippleCompCycleE, SHORT *RippleCompAmplitudeE, USHORT *RippleCompPhaseE)
{
//	RPLCMP_PRM	wrkp;	/*<S1FD>*/
	USHORT		RippleCompCycle_ele[6];
	SHORT		RippleCompAmplitude_ele[6];
	USHORT		RippleCompPhase_ele[6];

	LONG		i;
//	BOOL		err;

//	err = FALSE;

	/*----------------------------------------------------------------------------------------------*/
	/*		速度リップル補償パラメータコピー														*/
	/*----------------------------------------------------------------------------------------------*/
	for( i = 0; i < RIPCOMPORDER_ELE; i++ )
	{	/* 電気角補正 */
		RippleCompCycle_ele[i]     = RippleCompCycleE[i];
		RippleCompAmplitude_ele[i] = RippleCompAmplitudeE[i];
		RippleCompPhase_ele[i]     = RippleCompPhaseE[i];
	}

/*--------------------------------------------------------------------------------------------------*/
/*		電気角補正パラメータ 																		*/
/*--------------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------*/
	/*		補正ゲイン	[%] 																		*/
	/*----------------------------------------------------------------------------------------------*/
	RippleCmp->RplCmpPrm.RplCmpPrmEle.CompGain = MlibScalKxgain( RippleCompGainE, 1, 100, NULL, 24 );

	for(i=0; i<RIPCOMPORDER_ELE; i++)
	{
		RippleCmp->RplCmpPrm.RplCmpPrmEle.CompFrq[i] = RippleCompCycle_ele[i];
		/*------------------------------------------------------------------------------------------*/
		/*		補正振幅計算	[0.1%] --> [15000/MaxTrq]											*/
		/*------------------------------------------------------------------------------------------*/
		RippleCmp->RplCmpPrm.RplCmpPrmEle.CompAmpValue[i]  = BprmTorqueLevelCal( Bprm,
														(LONG)(RippleCompAmplitude_ele[i]), 1 );

		/*------------------------------------------------------------------------------------------*/
		/*		位相データ計算	[deg] --> [65536/360]												*/
		/*------------------------------------------------------------------------------------------*/
		RippleCmp->RplCmpPrm.RplCmpPrmEle.PhaseOfset[i]  = (USHORT)(SHORT)MlibScalKxgain
									((LONG)((SHORT)RippleCompPhase_ele[i]), 65536, 360, NULL,  -24 );
	}

//	return	(err);
	return	TRUE;
}

/****************************************************************************************************/
/*																									*/
/*		SigmaWin+からレジスタへリップル補償成分書き込み時のリップル補正パラメータ計算(電気角補正)	*/
/*																									*/
/****************************************************************************************************/
void	FnRegCalVelRippleCompE( BPRMDAT *Bprm, RPLCMP *RippleCmp )/*<S1FD>*/
{
//	RPLCMP_PRM	wrkp;		/*<S1FD>*/
	USHORT		RippleCompCycle_ele[6];
	SHORT		RippleCompAmplitude_ele[6];
	USHORT		RippleCompPhase_ele[6];
//	BPRMDAT		*Bprm;		/*<S1FD>*/
//	RPLCMP		*RippleCmp;	/*<S1FD>*/
	LONG		i;

//	Bprm = Axis->UniPrms->Bprm;/*<S1FD>*/
//	RippleCmp = &(Axis->BaseLoops->RippleCmp);


	/*----------------------------------------------------------------------------------------------*/
	/*		リップル補正パラメータコピー															*/
	/*----------------------------------------------------------------------------------------------*/
	for( i = 0; i < RIPCOMPORDER_ELE; i++ )
	{	/* 電気角補正 */
		RippleCompCycle_ele[i]     = RippleCmp->RplCmpPrm.RplCompCycleEle[i];
		RippleCompAmplitude_ele[i] = RippleCmp->RplCmpPrm.RplCompAmplitudeEle[i];
		RippleCompPhase_ele[i]     = RippleCmp->RplCmpPrm.RplCompPhaseEle[i];
	}

/*--------------------------------------------------------------------------------------------------*/
/*		電気角補正パラメータ 																		*/
/*--------------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------*/
	/*		補正ゲイン	[%] 																		*/
	/*----------------------------------------------------------------------------------------------*/
	RippleCmp->RplCmpPrm.RplCmpPrmEle.CompGain = MlibScalKxgain( RippleCmp->RplCmpPrm.RplCompGainEle,
																					1, 100, NULL, 24 );

	for(i=0; i<RIPCOMPORDER_ELE; i++)
	{
		RippleCmp->RplCmpPrm.RplCmpPrmEle.CompFrq[i] = RippleCompCycle_ele[i];
		/*------------------------------------------------------------------------------------------*/
		/*		補正振幅計算	[0.1%] --> [15000/MaxTrq]											*/
		/*------------------------------------------------------------------------------------------*/
		RippleCmp->RplCmpPrm.RplCmpPrmEle.CompAmpValue[i]  = BprmTorqueLevelCal( Bprm,
														(LONG)(RippleCompAmplitude_ele[i]), 1 );

		/*------------------------------------------------------------------------------------------*/
		/*		位相データ計算	[deg] --> [65536/360]												*/
		/*------------------------------------------------------------------------------------------*/
		RippleCmp->RplCmpPrm.RplCmpPrmEle.PhaseOfset[i]  = (USHORT)(SHORT)MlibScalKxgain
									((LONG)((SHORT)RippleCompPhase_ele[i]), 65536, 360, NULL,  -24 );
	}

//	return;	/*<S1FD>*/
}

/****************************************************************************************************/
/*																									*/
/*		今回接続モータと前回接続モータのシリアルNo.を比較 											*/
/*																									*/
/****************************************************************************************************/
ULONG ChkMotorSerialNo( UINT AxisNo, MENCV *MencV )/*<S1FD>*/
{
//	EEP_DEVICE_HANDLE	*EepDevice; /* <S1D2> */
	ULONG				rc;
	ULONG				ReadRslt;
//	UINT				AxisNo;/*<S1FD>*/
	USHORT				SerialNo[MOTOR_SN_DATA_SIZE];
	USHORT				Lst_SerialNo[MOTOR_SN_DATA_SIZE];
	LONG				ArrayIdx;

	/* 変数初期化 */
	rc = SN_READ_SUCCESS;
//	EepDevice = (Axis->RegManager->hPnReg)->hEepDev; /* <S1D2> */
//	AxisNo = Axis->AxisNo;/*<S1FD>*/

	/* 今回接続モータと前回接続モータのシリアルNo.を比較 */
	for(ArrayIdx = 0;  ArrayIdx < MOTOR_SN_DATA_SIZE; ArrayIdx++)
	{
		/* 今回接続モータのシリアルNo.取得 */
		SerialNo[ArrayIdx] = (USHORT)(MencV->MotInfo.SNo[ArrayIdx*2 + 1] & 0x00FF);
		SerialNo[ArrayIdx] |= (USHORT)((MencV->MotInfo.SNo[ArrayIdx*2] & 0x00FF) << 8);

		/* 前回接続モータのシリアルNo.取得 */
		/* read from EEPROM */
		ReadRslt = EepdevReadValues(															/* <S1D2> */
								0,																/* EEPROM Device ID(0: SPI(IF), 1: I2C(SDA) */
								EEP_NOSUM_ADDRESS(AxisNo, EEP_MOTORSN_OFS) + (ArrayIdx*2),		/* Eeprom Address in bytes */
								&Lst_SerialNo[ArrayIdx],										/* Pointer to the buffer for reading */
								1 );															/* reading size in words */
		if(ReadRslt == SN_READ_SUCCESS)	/* 前回接続モータのシリアルNo.読出し正常完了 */
		{
			if(Lst_SerialNo[ArrayIdx] != SerialNo[ArrayIdx] )	/* 前回接続モータと今回接続モータのシリアルNo.が異なる */
			{
				rc = SN_DIFFERENT;
				break;
			}
		}
		else	/* 前回接続モータのシリアルNo.読出し異常完了 */
		{
				rc = SN_READ_ERROR;
				break;
		}
	}

	return(rc);
}

/****************************************************************************************************/
/*																									*/
/*		速度リップル補償情報不一致ワーニング状態取得API												*/
/*																									*/
/****************************************************************************************************/
BOOL	RippleCompGetRippleWrnState( RPLCMP *RippleCmp )/*<S1FD>*/
{
	return	(RippleCmp->var.RippleWrnState);
}

/***************************************** end of file **********************************************/
