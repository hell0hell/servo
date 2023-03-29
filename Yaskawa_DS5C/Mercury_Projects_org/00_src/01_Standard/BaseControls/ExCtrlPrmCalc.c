/****************************************************************************************************/
/*																									*/
/*																									*/
/*		拡張制御演算用パラメータ計算関数定義														*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*			1. 調整レス制御関連パラメータ計算														*/
/*			2. 外乱オブザーバ(摩擦補償)パラメータ計算												*/
/*			3. 位相補償速度オブザーバパラメータ計算													*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	初版	2010.11.15	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"
#include "ExCtrlPrmCalc.h"
#include "CtrlPrmTransducer.h"

/* 外部関数 */
extern void TuneLessInitRobustCompensator( TUNELESS_CTRL *TuneLessCtrl, TUNELESS_DRCOMP *Drcomp );


/****************************************************************************************************/
/*		Table Data Definition																		*/
/****************************************************************************************************/
/* 調整レスで使用するゲイン。調整レベル設定によってテーブルが選択される */
const TUNELESS_GAIN_TBL TuneLessGainTbl[ TLESSGLVLMAX ] =
{ /*    Kv    F1rate */
	{	50	,	50	},	/* レベル0 */
	{	100	,	50	},	/* レベル1 */
	{	200	,	70	},	/* レベル2 */
	{	300	,	80	},	/* レベル3 */
	{	400	,	100	},	/* レベル4 *//* Σ-Ⅴ互換調整レスではレベル4まで */
	{	400	,	100	},	/* レベル5 *//* <S1C7> */
	{	400	,	100	},	/* レベル6 *//* <S1C7> */
	{	400	,	100	},	/* レベル7 *//* <S1C7> */
	{	400	,	100	},	/* レベル8 *//* <S1C7> */
	{	400	,	100	},	/* レベル9 *//* <S1C7> */
};

/* 新調整レスで使用するゲイン。調整レベル設定によってテーブルが選択される *//* <S1C7> */
const TUNELESS_GAIN_TBL TuneLessExGainTbl[ TLESSGLVLMAX ] =
{ /*    Kv    F1rate */
	{	50	,	50	},	/* レベル0 */
	{	100	,	50	},	/* レベル1 */
	{	200	,	70	},	/* レベル2 */
	{	300	,	80	},	/* レベル3 */
	{	400	,	100	},	/* レベル4 */
	{	500	,	100	},	/* レベル5 */
	{	600	,	100	},	/* レベル6 */
	{	700	,	100	},	/* レベル7 *//* 新調整レスではレベル7まで */
	{	800	,	100	},	/* レベル8 */
	{	900	,	100	},	/* レベル9 */
};

/* 調整レスで使用するイナーシャ比。調整レベル設定によってテーブルが選択される */
const TUNELESS_JRATE_TBL TuneLessJrateTbl1[ TLESSJLVLMAX ] =
{
	{	0		},		/* レベル0 */
	{	0		},		/* レベル1 */
	{	100		},		/* レベル2 *//* Σ-7の製品仕様ではレベル2まで */
	{	200		},		/* レベル3 */
	{	300		},		/* レベル4 */
};

/* 調整レスで使用するイナーシャ比(コアレス用)。調整レベル設定によってテーブルが選択される */
const TUNELESS_JRATE_TBL TuneLessJrateTbl2[ TLESSJLVLMAX ] =
{
	{	0		},		/* レベル0 */
	{	100		},		/* レベル1 */
	{	200		},		/* レベル2 *//* Σ-7の製品仕様ではレベル2まで */
	{	300		},		/* レベル3 */
	{	400		},		/* レベル4 */
};



/****************************************************************************************************/
/*		Local Function Definition																	*/
/****************************************************************************************************/
/* ロバスト補償器パラメータ計算 */
static	BOOL	tuneLessCalRobustCompPrm( TUNELESS_CTRL *TuneLessCtrl, BOOL LpassFil3,				  /* <S066> */
										  BPRMDAT *Bprm, TUNELESS_SETPRM *Prm, TRANSDUCE_CTRL *TransduceCtr );
/* 位相補償速度オブザーバパラメータ計算 */
static	BOOL	tuneLessCalEhSpdObsPrm( TUNELESS_CTRL *TuneLessCtrl, USHORT trqfil11, BPRMDAT *Bprm,  /* <S066> */
																		TRANSDUCE_CTRL *TransduceCtrl );
/* 正規化オブザーバイナーシャゲイン計算 */
static	LONG	tuneLessCalNorObsJGain( TUNELESS_PRM *TuneLessPrm, KSGAIN Kvx, LONG scantime );
/* 電流ループ近似モデル2次フィルタパラメータ計算(双一次変換) */
static	LONG	PcalCurLoopLowfilter2( LONG hz, LONG s_hz, LONG zn1, LONG zn2, LONG ts, LONG kn[5] );
/* 2次バンドパスフィルタパラメータ計算(双一次変換) */
static	LONG	PcalBandPassfilter( LONG hz, LONG zn, LONG ts, LONG kn[5] );
/* 2次ハイパスフィルタパラメータ計算(双一次変換) */
static	LONG	PcalHighPassfilter( LONG hz, LONG zn, LONG ts, LONG kn[5] );

#if (FLOAT_USE==TRUE)
/* 電流ループ近似モデル2次フィルタパラメータ計算(双一次変換) */
static	float	f_PcalCurLoopLowfilter2( float hz, float s_hz, float zn1, float zn2, float ts, float kn[5] );
#endif /* FLOAT_USE */

/****************************************************************************************************/
/*																									*/
/*		調整レス機能選択スイッチ																	*/
/*																									*/
/*			TuningLessMode																			*/
/*				0桁： 負荷ﾚﾍﾞﾙ	0:標準	1:大負荷対策(負荷ﾚﾍﾞﾙ初期値大)								*/
/*				1桁： ﾛﾊﾞｽﾄ性	0:標準	1:安定性重視(ﾛﾊﾞｽﾄ性やや低)	2:安定性重視(ﾛﾊﾞｽﾄ性低)			*/
/*								4:安定性重視(ﾛﾊﾞｽﾄ性極低)											*/
/*				2桁： 速度検出	0:標準	1:速度検出切替えあり										*/
/*																									*/
/****************************************************************************************************/
/*		戻り値	：	Error Status																	*/
/*			PRM_SETTING_OK		：エラーなし														*/
/*			PRMERR_TUNELESS_LVL	：Pn170 設定異常													*/
/*			PRMERR_TUNELESS_TYPE：Pn14F 設定異常													*/
/****************************************************************************************************/
LONG	TuneLessCalculateInitPrm( BASE_CTRL_HNDL *BaseCtrlData, BPRMDAT *Bprm, 
								  MENCV *MencV, ASICS *SvAsicRegs, TUNELESS_CFGPRM *CfgPrm )
{
	TUNELESS_CTRL		*TuneLessCtrl;
	LONG				PrmSetting;

	/* ローカル変数の初期化 */
	TuneLessCtrl = &(BaseCtrlData->TuneLessCtrl);
	PrmSetting = PRM_SETTING_OK;

	/*------------------------------------------------------------------------------------------*/
	/*	調整レスモード初期化																	*/
	/*------------------------------------------------------------------------------------------*/
	TuneLessCtrl->conf.TuningLessMode = TLMODE_NORMAL;

	/*------------------------------------------------------------------------------------------*/
	/*	調整レス機能スイッチ処理																*/
	/*------------------------------------------------------------------------------------------*/
#if 0 /* 2013.08.16 Y.Oka 制御モード切り替え要確認 @@Check */
	if( (Iprm.CtrlModeA == CTRLMODE_TRQ) && (Iprm.CtrlModeB == CTRLMODE_TRQ) )
	{ /* トルク制御設定は、調整レス無効 */
		TuneLessCtrl->conf.TuningLessUse = FALSE;
		TuneLessCtrl->var.TuneLessAct = FALSE;
	}
	else
	{
#endif /* #if 0 */
		switch( (CfgPrm->DatLevel) & 0x0F )
		{
		case 0x00:
			/* 調整レス無効 */
			TuneLessCtrl->conf.TuningLessUse = FALSE;
			TuneLessCtrl->conf.TuningLessEx = FALSE;
			TuneLessCtrl->var.TuneLessAct = FALSE;
			break;
		case 0x01:
			/* 調整レス有効 */
			TuneLessCtrl->conf.TuningLessUse = TRUE;
			TuneLessCtrl->var.TuneLessAct = TRUE;
			if( ((CfgPrm->mfctype >> 4) & 0x0F) == 0x02 ) /* <S1C7> */
			{
				TuneLessCtrl->conf.TuningLessEx = TRUE;
			}
			else
			{
				TuneLessCtrl->conf.TuningLessEx = FALSE;
			}

			break;
#if 0 /* <S1C7> */
		case 0x02:
			/* 調整レス有効 */
			TuneLessCtrl->conf.TuningLessUse = TRUE;
			TuneLessCtrl->conf.TuningLessEx = FALSE;	/* <S1A5> */
			TuneLessCtrl->var.TuneLessAct = TRUE;
			break;
#endif /* <S1C7> */
		default :
			PrmSetting |= PRMERR_TUNELESS_LVL;
			break;
		}
#if 0 /* 2013.08.16 Y.Oka 制御モード切り替え要確認 @@Check */
	}
#endif /* #if 0 */

	/*------------------------------------------------------------------------------------------*/
	/*	速度制御方式処理																		*/
	/*------------------------------------------------------------------------------------------*/
	if( TuneLessCtrl->conf.TuningLessUse == FALSE )
	{	/* 調整レス無効時 */
		/* 調整レス速度制御時位相補償無効 */
		TuneLessCtrl->conf.TuningLessVComp = FALSE;
		TuneLessCtrl->var.TuneLessVCompReq = FALSE;
	}
	else
	{
		switch( (CfgPrm->DatLevel >> 4) & 0x0F )
		{
		case 0x00:
			/* 調整レス速度制御時位相補償無効　*/
			TuneLessCtrl->conf.TuningLessVComp = FALSE;
			TuneLessCtrl->var.TuneLessVCompReq = FALSE;
			break;
		case 0x01:
			/* 調整レス速度制御時位相補償有効　*/
			TuneLessCtrl->conf.TuningLessVComp = TRUE;
			TuneLessCtrl->var.TuneLessVCompReq = TRUE;
			break;
		default :
			PrmSetting |= PRMERR_TUNELESS_LVL;
			break;
		}
	}

	/*------------------------------------------------------------------------------------------*/
	/*	モータ種別処理																			*/
	/*------------------------------------------------------------------------------------------*/
	if( TuneLessCtrl->conf.TuningLessEx == FALSE )
	{ /* other than New Tuning Less Mode */
		if( (CfgPrm->flg_wf & 0x0004) || (MencV->TuningLessLowGainMotor == TRUE) )
		{ /* モータ静音設定選択フラグ有効 || 調整レス制御低騒音設定モータ */
			TuneLessCtrl->conf.TuningLessMode |= TLMODE_SILENT;
		}
		if( MencV->AxisMotType == MOTTYPE_LINEAR )
		{ /* Σ Trac -μ MAG */
			if( MencV->SigmaTracMotor == TRUE )
			{
				TuneLessCtrl->conf.TuningLessMode |= TLMODE_STABLE_L;
			}
	
			if( MencV->CoreLessLinearMotor == TRUE )
			{ /* コアレスリニアモータ */
				TuneLessCtrl->conf.TuningLessMode |= TLMODE_CORELESS;
			}
		}
		else
		{
			if( MencV->LowSpdMotor == TRUE )
			{ /* DDモータ */
				TuneLessCtrl->conf.TuningLessMode |= TLMODE_STABLE_R;
			}
		}
	}
	else
	{ /* New Tuning Less Mode */
		if( MencV->AxisMotType == MOTTYPE_LINEAR )
		{
			if( MencV->CoreLessLinearMotor == TRUE )
			{ /* コアレスリニアモータ */
				TuneLessCtrl->conf.TuningLessMode |= TLMODE_CORELESS;
			}
		}
	}

	/*------------------------------------------------------------------------------------------*/
	/*	エンコーダ種別処理																		*/
	/*------------------------------------------------------------------------------------------*/
	if( MencV->AxisMotType == MOTTYPE_LINEAR )
	{
		if( Bprm->SpdDetectUnit >= TLSPDDET200UM )
		{
			/* 安定性重視 */
			TuneLessCtrl->conf.TuningLessMode |= TLMODE_STABLE_L;
			/* 速度検出切り替えあり */
			TuneLessCtrl->conf.TuningLessMode |= TLMODE_SPDDETCHNG;
		}
	}
	else
	{
		if( Bprm->SpdDetectUnit >= TLSPDDET13BIT )
		{
			/* 安定性重視 */
			TuneLessCtrl->conf.TuningLessMode |= TLMODE_STABLE_R;
			/* 速度検出切り替えあり */
			TuneLessCtrl->conf.TuningLessMode |= TLMODE_SPDDETCHNG;
		}
	}

	/*------------------------------------------------------------------------------------------*/
	/*	調整レスタイプ選択処理																	*/
	/*------------------------------------------------------------------------------------------*/
	switch( (CfgPrm->mfctype >> 4) & 0x0F )
	{
	case 0x00:
		/* ロバスト性重視(フィルタなし設定) */
		TuneLessCtrl->conf.TuningLessType = 0;
		break;
	case 0x01:
	case 0x02: /* <S1C7> */
		/* 低騒音重視(フィルタあり設定) */
		TuneLessCtrl->conf.TuningLessType = 1;
		break;
	default :
		PrmSetting |= PRMERR_TUNELESS_TYPE;
		break;
	}

	/*------------------------------------------------------------------------------------------*/
	/*	組合せ不可機能スイッチパラメータ計算													*/
	/*------------------------------------------------------------------------------------------*/
#if( CSW_SVCF_ATYPE_RES_VIB_USE != 0 )
	/* Ａ型制振制御(パラメータ設定エラーは設定時に確認済み) */
	ResVibCalculatePrmSW( &(BaseCtrlData->ResVib),
						  CfgPrm->avibsw,
						  TuneLessCtrl->conf.TuningLessUse,
						  SvAsicRegs );
#endif

	/* 外乱オブザーバ(パラメータ設定エラーは自モジュールのパラメータ計算にて検出) */
	DobsCalculatePrmSW( &(BaseCtrlData->DobsCtrl.var.DobsAct),
						BaseCtrlData->TuneLessCtrl.conf.TuningLessUse,
						(CfgPrm->tfuncsw >> 12) );

	/* 位相補償速度オブザーバ(パラメータ設定エラーは自モジュールのパラメータ計算にて検出) */
	EhVobsCalculatePrmSW( &(BaseCtrlData->EhVobsCtrl.conf.EhVobsUse),
						  BaseCtrlData->TuneLessCtrl.conf.TuningLessUse,
						  (CfgPrm->gnmode >> 12) );

	/*------------------------------------------------------------------------------------------*/
	/*	調整レス無効時初期化処理																*/
	/*------------------------------------------------------------------------------------------*/
	if( TuneLessCtrl->conf.TuningLessUse == FALSE )
	{ /* 調整レス無効 */
		/* 調製レスモード初期化 */
		TuneLessCtrl->conf.TuningLessMode = TLMODE_NORMAL;
	}

	return	PrmSetting;
}



/****************************************************************************************************/
/*																									*/
/*		調整レストルク補償後トルクフィルタ選択														*/
/*																									*/
/****************************************************************************************************/
BOOL	TuneLessSetTrqFil3( BOOL TuningLess, BOOL TuningLessEx, ULONG flg_wf, BOOL TuningLessLowGainMotor )
{
	BOOL	LpassFil3;

	if( (TuningLess == TRUE) && (TuningLessEx == FALSE) )
	{
		if( (flg_wf & 0x0004) || (TuningLessLowGainMotor == TRUE) )
		{
			/* トルク補償後トルクフィルタあり */
			LpassFil3 = TRUE;
		}
		else
		{
			LpassFil3 = FALSE;
		}
	}
	else
	{ /* Invalid Tuning Less or New Tuning Less Mode */
		LpassFil3 = FALSE;
	}
	return	LpassFil3;
}



/****************************************************************************************************/
/*																									*/
/*		調整レス関連ワンパラパラメータ計算															*/
/*																									*/
/****************************************************************************************************/
BOOL	TuneLessCalculatePrm( BASE_LOOPCTRLS *BaseLoops, BPRMDAT *Bprm,
								TUNELESS_SETPRM *Prm, BASE_SPDLOOP_PRM *SpdCtrlPrm )
{
//	AXIS_HANDLE *Axis
	BOOL				ret;
	LONG				temp1, temp2;
	LONG				sx, sx1, sx2, sx3;
	BASE_CTRL_HNDL		*BaseCtrlData;
	TUNELESS_PRM		*TuneLessPrm;
	TUNELESS_CTRL		*TuneLessCtrl;
//	BASE_SPDLOOP_PRM 	SpdCtrlPrm = {0};	/* <S01E> */
	TL2PRM				*pTl2Prm;			/* <S1A5> */

	/* ローカル変数の初期化 */
	BaseCtrlData = BaseLoops->BaseCtrls;
	TuneLessPrm = &(BaseCtrlData->TuneLessCtrl.TuneLessPrm);
	pTl2Prm = &(TuneLessPrm->Tl2Prm);		/* <S1A5> */
	TuneLessCtrl = &(BaseCtrlData->TuneLessCtrl);
	ret = TRUE;

	if( TuneLessCtrl->conf.TuningLessUse == FALSE )
	{ /* 調整レス無効時 */
		return	ret;
	}

	/*----------------------------------------------------------------------------------------------*/
	/*	ゲインレベル、負荷レベル設定(レジスタ処理)	for Fn200										*/
	/*----------------------------------------------------------------------------------------------*/
	RpiRegSetTuneLessLevel( Prm->DatLevel, TuneLessCtrl );

	/*----------------------------------------------------------------------------------------------*/
	/*	Parameter Set																				*/
	/*		・調整レベルからゲインパラメータを設定													*/
	/*		・ノッチフィルタ、トルクフィルタは、調整レス機能SWに関わらず、ユーザー定数を使用		*/
	/*		・コアレスリニアはJmが小さいのでデフォルト設定でJLを設定する							*/
	/*----------------------------------------------------------------------------------------------*/
	if( TuneLessCtrl->conf.TuningLessEx == FALSE )
	{ /* other than New Tuning Less Mode */
		TuneLessPrm->kv = TuneLessGainTbl[ TuneLessCtrl->OpeReg.DatGnLevel ].Kv;					  /* <S1C7> */
		TuneLessPrm->F1rate = TuneLessGainTbl[ TuneLessCtrl->OpeReg.DatGnLevel ].F1rate;
	}
	else
	{ /* New Tuning Less Mode */
		TuneLessPrm->kv = TuneLessExGainTbl[ TuneLessCtrl->OpeReg.DatGnLevel ].Kv;					  /* <S1C7> */
		TuneLessPrm->F1rate = 100;
	}

	if( TuneLessCtrl->conf.TuningLessMode & TLMODE_CORELESS )
	{
		TuneLessPrm->jrate = TuneLessJrateTbl2[ TuneLessCtrl->OpeReg.DatJratLevel ].Jrate;
	}
	else
	{
		TuneLessPrm->jrate = TuneLessJrateTbl1[ TuneLessCtrl->OpeReg.DatJratLevel ].Jrate;
		if( TuneLessCtrl->OpeReg.DatJratLevel == 0 )
		{ /* 負荷レベル == 0 */
			/* ロバスト性1/2設定 */
			if( TuneLessCtrl->conf.TuningLessEx == FALSE )		/* <S1F0> */
			{ /* 新調整レスでは発信気味になるため、以下は行わない。再設計必要う。 */
			TuneLessPrm->F1rate = TuneLessPrm->F1rate >> 1;
			}
		}
	}

	/*----------------------------------------------------------------------------------------------*/
	/*		VLoop PITime	PI	：	Ti[0.01ms] = 5 * 1000000 / 2πKv[0.1Hz]							*/
	/*						I-P	：	Ti[0.01ms] = 2 * 1000000 / 2πKv[0.1Hz]							*/
	/*																								*/
	/*----------------------------------------------------------------------------------------------*/
	if( BaseCtrlData->CtrlCmdPrm.IPSpdControl == TRUE )
	{
		TuneLessPrm->Ti = 320000 / TuneLessPrm->kv;			/* 320000 ≒ 2 * 1000000 / 2π			*/
		TuneLessPrm->kp = TUNE_IP_KPTI / TuneLessPrm->Ti;	/* I-P制御時	ζ=1.0					*/
	}
	else
	{
		TuneLessPrm->Ti = 800000 / TuneLessPrm->kv;			/* 800000 ≒ 5 *1000000 / 2π			*/
		TuneLessPrm->kp = TuneLessPrm->kv;
	}

	/*----------------------------------------------------------------------------------------------*/
	/*		外乱オブザーバ関連パラメータ															*/
	/*			f1[Hz] = n * Kv[Hz](Kv=40[Hz]固定) / 2	：ロバスト性を調整							*/
	/*			f2[Hz] = f1[Hz] (ζ=0.50)															*/
	/*			Kj[%] = 100% 固定																	*/
	/*																								*/
	/*	※	DDモータでは騒音が大きいためf2を標準より低くする。										*/
	/*		制御の安定性確保のため、f1も下げる必要がある。											*/
	/*----------------------------------------------------------------------------------------------*/
	if( Prm->TuneLessGain <= 0 )
	{
		Prm->TuneLessGain = DISOBSFSTD;
	}
	if( Prm->TuneLessStiff <= 0 )
	{
		Prm->TuneLessStiff = TUNELESS_STIFFSTD;
	}

	if( TuneLessCtrl->conf.TuningLessMode & TLMODE_SILENT )
	{
		/* 低ロバスト性：f1[0.1Hz](騒音対策) */
		TuneLessPrm->f1 = MlibMIN( Prm->TuneLessGain, DISOBSF12 );
		/* f2[0.1Hz] */
		TuneLessPrm->f2 = DISOBSF22;
		/* ロバスト性：低い方を選択 */
		TuneLessPrm->Stiff = MlibMIN( Prm->TuneLessStiff, TUNELESS_STIFF3 );
	}
	else if( TuneLessCtrl->conf.TuningLessMode & TLMODE_STABLE_R )
	{
		/* 高ロバスト性：f1[0.1Hz](標準) */
		TuneLessPrm->f1 = Prm->TuneLessGain;
		/* f2[0.1Hz] */
		TuneLessPrm->f2 = DISOBSF21;
		/* ロバスト性：低い方を選択 */
		TuneLessPrm->Stiff = MlibMIN( Prm->TuneLessStiff, TUNELESS_STIFF3 );
	}
	else if( TuneLessCtrl->conf.TuningLessMode & TLMODE_STABLE_L )
	{
		/* 高ロバスト性：f1[0.1Hz](標準) */
		TuneLessPrm->f1 = Prm->TuneLessGain;
		/* f2[0.1Hz] */
		TuneLessPrm->f2 = DISOBSF21;
		/* ロバスト性：低い方を選択 */
		TuneLessPrm->Stiff = MlibMIN( Prm->TuneLessStiff, TUNELESS_STIFF2 );
	}
	else
	{
		/* 高ロバスト性：f1[0.1Hz](標準) */
		TuneLessPrm->f1 = Prm->TuneLessGain;
		/* f2[0.1Hz] */
		TuneLessPrm->f2 = DISOBSF21;
		/* ロバスト性：100% */
		TuneLessPrm->Stiff = Prm->TuneLessStiff;
	}

	if( TuneLessCtrl->conf.TuningLessEx == TRUE )
	{ /* New Tuning Less Mode */
		TuneLessPrm->f1 = TLEX_DISOBSFRQ;
	}

	/* トルク補償後トルクフィルタ時定数 [0.01ms] */
	TuneLessPrm->klpf = TUNLESS_TREFFIL;

	/* Kj[%] */
	TuneLessPrm->kj_dist = DISOBSKJ;
	/* Kd[%] */
	if( TuneLessCtrl->conf.TuningLessEx == FALSE )
	{ /* other than New Tuning Less Mode */
		TuneLessPrm->kd_dist = DISOBSKD;
	}
	else
	{ /* New Tuning Less Mode */
		TuneLessPrm->kd_dist = TLEX_DISOBSKD;
	}

	/*----------------------------------------------------------------------------------------------*/
	/*		位相補償速度オブザーバ関連パラメータ													*/
	/*			Ks[Hz] = 1.5 * Kv[Hz](Kv=40[Hz]固定)												*/
	/*			Kj[%] = 100% 固定																	*/
	/*			Tph[s] = n * 40 /(2πf1[Hz] * Kv[Hz])												*/
	/*															Tph[0.01ms]							*/
	/*															64000000 = 100000 * 40 * 100 / 2π	*/
	/*----------------------------------------------------------------------------------------------*/
	/* Ks[Hz] */
	if( TuneLessCtrl->conf.TuningLessEx == FALSE )
	{ /* other than New Tuning Less Mode */
		TuneLessPrm->ks_vobs = VOBSKS;
	}
	else
	{ /* New Tuning Less Mode */
		TuneLessPrm->ks_vobs = TLEX_VOBSKS;
	}

	/* Kj[%] */
	TuneLessPrm->kj_vobs = VOBSKJ;

	/* 位相補償速度オブザーバLPF時定数[0.01ms] */
	if( TuneLessCtrl->conf.TuningLessEx == FALSE )
	{ /* other than New Tuning Less Mode */
		TuneLessPrm->vobsLpf = 64000000 / VOBSLPFGNHZ / TuneLessPrm->kv;
	}
	else
	{ /* New Tuning Less Mode */
		TuneLessPrm->vobsLpf = TLEX_VOBSLPFGAIN;
	}

	/*----------------------------------------------------------------------------------------------*/
	/*		フィードフォワード関連パラメータ														*/
	/*			VFF1[Hz] = 1/2πTph[s] 																*/
	/*			VFF2[Hz] = Kv[Hz](Kv=40[Hz]固定) * 12.5												*/
	/*													VFF1[0.1Hz]	128000 ≒ 1000000 / 2π / 1.243	*/
	/*													VFF2[0.1Hz]									*/
	/*----------------------------------------------------------------------------------------------*/
	if( TuneLessPrm->vobsLpf != 0 )
	{
		/* VFF1[Hz] */
		if( TuneLessCtrl->conf.TuningLessEx == FALSE )
		{ /* other than New Tuning Less Mode */
			TuneLessPrm->vffPhCmpFrq1 = 128000 / TuneLessPrm->vobsLpf;
		}
		else
		{ /* New Tuning Less Mode */
			TuneLessPrm->vffPhCmpFrq1 = TLEX_VPHASEF1;
		}
	}
	else
	{
		TuneLessPrm->vffPhCmpFrq1 = VPHASEF2;
	}

	/* VFF2[Hz] */
	if( TuneLessCtrl->conf.TuningLessEx == FALSE )
	{ /* other than New Tuning Less Mode */
		TuneLessPrm->vffPhCmpFrq2 = VPHASEF2;
	}
	else
	{ /* New Tuning Less Mode */
		TuneLessPrm->vffPhCmpFrq2 = TLEX_VPHASEF2;
	}

	if( TuneLessPrm->vffPhCmpFrq1 <= 0 )
	{
		/* 下限値設定 */
		TuneLessPrm->vffPhCmpFrq1 = VPHASEFULIM;
	}

	/*----------------------------------------------------------------------------------------------*/
	/*		調整レス専用パラメータ計算																*/
	/*																								*/
	/*		位置、速度ゲインは調整レス専用パラメータ設定値を使用。									*/
	/*		トルクフィルタはPn401設定値を使用。														*/
	/*		(注意：ここではトルクフィルタのワンパラ計算はしない。ここでワンパラ計算すると			*/
	/*				Pn401設定が全く反映されなくなってしまうため)									*/
	/*																								*/
	/*----------------------------------------------------------------------------------------------*/
	/* 速度制御ゲイン */
	TuneLessPrm->kv2 = TuneLessPrm->kv * TuneLessPrm->Stiff / 100;
	/* 速度制御パラメータ計算 */
//	PcalBaseSpeedControl( BaseCtrlData, Bprm, &SpdCtrlPrm, 5 );	/* <S01E> */
	PcalBaseSpeedControl( BaseCtrlData, Bprm, SpdCtrlPrm, 5 );	/* <S01E> */

	/* 位置制御ゲイン */
	PcalBasePositionPControl( BaseCtrlData,
							  Bprm,
							  BaseCtrlData->PosManager.Egear,
							  TuneLessPrm->kp,
							  5 );

	/* ゼロクランプ位置制御ゲイン計算 */
	PcalZeroPositionControl( BaseCtrlData, Bprm, TuneLessPrm->kp, 5 );

	/* トルクフィルタ */
	PcalBaseTrqLpassFilter( &(BaseLoops->TrqFil), BaseCtrlData, Prm->trqfil11, Prm->trqfil11, 5 );
	BaseCtrlData->CtrlCmdPrm.TLPrm.Klpf3 = PcalBaseTrqLpassFilter3( TuneLessPrm->klpf, KPI_SACYCLEUS );

	/* 外乱オブザーバ */
	TuneLessPrm->kj_dist2 = TuneLessPrm->kj_dist * TuneLessPrm->Stiff / 100;
	TuneLessPrm->f1Comp = TuneLessPrm->f1 * TuneLessPrm->F1rate / 100;
	TuneLessPrm->f1Comp = MlibMAX( TuneLessPrm->f1Comp, DISOBSF1MIN );
	if( FALSE == tuneLessCalRobustCompPrm(TuneLessCtrl,
										  BaseCtrlData->CtrlCmdPrm.LpassFil3,
										  Bprm,
										  Prm,
										  &(BaseCtrlData->TransduceCtrl)) )
	{
		ret = FALSE;
	}

	if( TuneLessCtrl->conf.TuningLessEx == TRUE )
	{ /* New Tuning Less Mode */
//<S1A5>		TuneLessPrm->KrlpfResSup = MlibPcalKf1gain( (LONG)TLEX_RESSUPFIL, KPI_SACYCLEUS , 1 );
		pTl2Prm->KrlpfResSup = MlibPcalKf1gain( (LONG)TLEX_RESSUPFIL, KPI_SACYCLEUS , 1 );	/* <S1A5> */


		/* 外乱オブザーバ2次フィルタゲイン(バンドパスフィルタ用)計算 */
		PcalBandPassfilter( TuneLessPrm->f1/10, (LONG)TLEX_BNDPASSFILZN, KPI_SACYCLEUS, pTl2Prm/* <S1A5> TuneLessPrm */->KrlpfDisObs1 );
		/* 外乱オブザーバ2次フィルタゲイン(ハイパスフィルタ用)計算 */
		PcalHighPassfilter( TuneLessPrm->f1/10, (LONG)TLEX_BNDPASSFILZN, KPI_SACYCLEUS, pTl2Prm/* <S1A5> TuneLessPrm */->KrlpfDisObs2 );

		/* Ki/(La*Tic) */
		sx = 0;
		temp1 = MlibPcalKxgain( Prm->ignq, C2PAIE7, 10000000, &sx, 0 );
		/* 電流ループモデル周波数 */
		pTl2Prm/* <S1A5> TuneLessPrm */->KrCurfreq = MlibPcalKxgain( temp1, 1000000, Prm->kiq, &sx, 0 );

		/* (Ra+Ki)/La */
		sx1 = 0;
		temp1 = MlibPcalKxgain( Prm->ignq, Prm->MencP->motl, 100000, &sx1, 0 );
		temp1 = MlibPcalKxgain( temp1, C2PAIE7, 10000000, &sx1, 0 );
		sx2 = 0;
		temp2 = MlibPcalKxgain( Prm->MencP->motr, 1, 1000, &sx2, 0 );
		pTl2Prm/* <S1A5> TuneLessPrm */->KrCurZeta1 = MlibPcalKxaddx( temp1, sx1, temp2, sx2, &sx3 );
		pTl2Prm/* <S1A5> TuneLessPrm */->KrCurZeta1 = MlibPcalKxgain( pTl2Prm /* <S1A5> TuneLessPrm */->KrCurZeta1, 100000, Prm->MencP->motl, &sx3, 24 );
		/* 電流ループモデル減衰係数1 */
		pTl2Prm/* <S1A5> TuneLessPrm */->KrCurZeta1 = MlibMulgain32( 1, pTl2Prm /* <S1A5> TuneLessPrm */->KrCurZeta1 );

		/* Ki/La */
		sx1 = 0;
		temp1 = MlibPcalKxgain( Prm->ignq, Prm->MencP->motl, 100000, &sx1, 0 );
		temp1 = MlibPcalKxgain( temp1, C2PAIE7, 10000000, &sx1, 0 );
		temp1 = MlibPcalKxgain( temp1, 100000, Prm->MencP->motl, &sx1, 24 );
		/* 電流ループモデル減衰係数2 */
		pTl2Prm/* <S1A5> TuneLessPrm */->KrCurZeta2 = MlibMulgain32( 1, temp1 );

		/* 電流ループ近似モデル2次フィルタゲイン計算 */
#if (FLOAT_USE==TRUE)
		f_PcalCurLoopLowfilter2( pTl2Prm /* <S1A5> TuneLessPrm */->KrCurfreq,
							   sx,
							   pTl2Prm /* <S1A5> TuneLessPrm */->KrCurZeta1,
							   pTl2Prm /* <S1A5> TuneLessPrm */->KrCurZeta2,
							   KPI_SACYCLEUS,
							   pTl2Prm /* <S1A5> TuneLessPrm*/->KrlpfCur );
#else
		PcalCurLoopLowfilter2( pTl2Prm /* <S1A5> TuneLessPrm */->KrCurfreq,
							   sx,
							   pTl2Prm /* <S1A5> TuneLessPrm->*/->KrCurZeta1,
							   pTl2Prm /* <S1A5> TuneLessPrm */->KrCurZeta2,
							   KPI_SACYCLEUS,
							   pTl2Prm /* <S1A5> TuneLessPrm */->KrlpfCur );
#endif /* FLOAT_USE */
	}

	/* 位相補償速度オブザーバ */
	TuneLessPrm->kj_vobs2 = TuneLessPrm->kj_vobs * 100 / TuneLessPrm->Stiff;
	if( FALSE == tuneLessCalEhSpdObsPrm(TuneLessCtrl,
										Prm->trqfil11,
										Bprm,
										&(BaseCtrlData->TransduceCtrl)) )
	{
		ret = FALSE;
	}

	/* 位相補償フィルタ */
#if (FLOAT_USE==TRUE) 
	BaseCtrlData->CtrlCmdPrm.TLPrm.SpdRefPhFil =
		FlibPcalKf1gain( TuneLessPrm->vffPhCmpFrq2 , KPI_SACYCLEUS , 1 );
	BaseCtrlData->CtrlCmdPrm.TLPrm.SpdRefPhFilGn =
		((float)TuneLessPrm->vffPhCmpFrq2 / (float)TuneLessPrm->vffPhCmpFrq1);
#else
	BaseCtrlData->CtrlCmdPrm.TLPrm.SpdRefPhFil =
		MlibPcalKf1gain( TuneLessPrm->vffPhCmpFrq2, KPI_SACYCLEUS , 1 );
	BaseCtrlData->CtrlCmdPrm.TLPrm.SpdRefPhFilGn =
		MlibScalKxgain( TuneLessPrm->vffPhCmpFrq2, 1, TuneLessPrm->vffPhCmpFrq1, NULL, 24 );
#endif /* FLOAT_USE */

	/* Notch & Torque Filter */
	/* イナーシャは不明だがとりあえず負荷レベルを設定 */
	AutoNotchCalculatePrm( Prm->ANotchSeq, TuneLessPrm->kv, TuneLessPrm->jrate );

	/* 振動検出用フィルタ設定 */
	/* イナーシャ値が合っていないので低周波振動分をカット */
	DetVibObsCalculateGains( &(Prm->DetVib->DetVibObs),
							 VIBOBS_KS, DETVIBLP, DETVIBHP,
							 Bprm->SvCycleNs );

	/* 差速度ピーク値クリア */
	/* 2010.09.29 Y.Oka 振動検出現状未対応 */
	DetVibResetMaxMinSpeedError( Prm->DetVib );
	DetVibResetMaxMinSpeedErrorFn( Prm->DetVib );


#if 0 /* 2012.08.31 Y.Oka ★新調整レス★ */
	if( TuneLessCtrl->conf.TuningLessEx == TRUE )
	{
		/* Set New Tuning Less Mode */
		SvAsicRegs->AsicMicroReg->MREG_TLMODE2SEL = (USHORT)TuneLessCtrl->conf.TuningLessEx;

		/* Set ResSupFilter Gain */
		tbuf[0] = (USHORT)(TuneLessPrm->KrlpfResSup);
		tbuf[1] = (USHORT)(TuneLessPrm->KrlpfResSup >> 16);
		MicroTranslatePrmReq( SvAsicRegs, tbuf,
							  &(SvAsicRegs->AsicMicroReg->MREG_TLKRESSUP), 2 );

		/* Set Bandpass Filter Gain */
		tbuf[0] = (USHORT)(TuneLessPrm->KrlpfDisObs1[0]);
		tbuf[1] = (USHORT)(TuneLessPrm->KrlpfDisObs1[0] >> 16);
		tbuf[2] = (USHORT)(TuneLessPrm->KrlpfDisObs1[1]);
		tbuf[3] = (USHORT)(TuneLessPrm->KrlpfDisObs1[1] >> 16);
		tbuf[4] = (USHORT)(TuneLessPrm->KrlpfDisObs1[2]);
		tbuf[5] = (USHORT)(TuneLessPrm->KrlpfDisObs1[2] >> 16);
		tbuf[6] = (USHORT)(TuneLessPrm->KrlpfDisObs1[3]);
		tbuf[7] = (USHORT)(TuneLessPrm->KrlpfDisObs1[3] >> 16);
		tbuf[8] = (USHORT)(TuneLessPrm->KrlpfDisObs1[4]);
		tbuf[9] = (USHORT)(TuneLessPrm->KrlpfDisObs1[4] >> 16);
		MicroTranslatePrmReq( SvAsicRegs, tbuf,
							  &(SvAsicRegs->AsicMicroReg->MREG_TLKBPFIL0), 10 );

		/* Set Highpass Filter Gain */
		tbuf[0] = (USHORT)(TuneLessPrm->KrlpfDisObs2[0]);
		tbuf[1] = (USHORT)(TuneLessPrm->KrlpfDisObs2[0] >> 16);
		tbuf[2] = (USHORT)(TuneLessPrm->KrlpfDisObs2[1]);
		tbuf[3] = (USHORT)(TuneLessPrm->KrlpfDisObs2[1] >> 16);
		tbuf[4] = (USHORT)(TuneLessPrm->KrlpfDisObs2[2]);
		tbuf[5] = (USHORT)(TuneLessPrm->KrlpfDisObs2[2] >> 16);
		tbuf[6] = (USHORT)(TuneLessPrm->KrlpfDisObs2[3]);
		tbuf[7] = (USHORT)(TuneLessPrm->KrlpfDisObs2[3] >> 16);
		tbuf[8] = (USHORT)(TuneLessPrm->KrlpfDisObs2[4]);
		tbuf[9] = (USHORT)(TuneLessPrm->KrlpfDisObs2[4] >> 16);
		MicroTranslatePrmReq( SvAsicRegs, tbuf,
							  &(SvAsicRegs->AsicMicroReg->MREG_TLKHPFIL0), 10 );

		/* Set Highpass Filter Gain */
		tbuf[0] = (USHORT)(TuneLessPrm->KrlpfCur[0]);
		tbuf[1] = (USHORT)(TuneLessPrm->KrlpfCur[0] >> 16);
		tbuf[2] = (USHORT)(TuneLessPrm->KrlpfCur[1]);
		tbuf[3] = (USHORT)(TuneLessPrm->KrlpfCur[1] >> 16);
		tbuf[4] = (USHORT)(TuneLessPrm->KrlpfCur[2]);
		tbuf[5] = (USHORT)(TuneLessPrm->KrlpfCur[2] >> 16);
		tbuf[6] = (USHORT)(TuneLessPrm->KrlpfCur[3]);
		tbuf[7] = (USHORT)(TuneLessPrm->KrlpfCur[3] >> 16);
		tbuf[8] = (USHORT)(TuneLessPrm->KrlpfCur[4]);
		tbuf[9] = (USHORT)(TuneLessPrm->KrlpfCur[4] >> 16);
		MicroTranslatePrmReq( SvAsicRegs, tbuf,
							  &(SvAsicRegs->AsicMicroReg->MREG_TLKCURLPF0), 10 );

		/* Set Current Loop Approach Model Parameters */
		tbuf[0] = (USHORT)(TuneLessPrm->KrCurfreq);
		tbuf[1] = (USHORT)(TuneLessPrm->KrCurfreq >> 16);
		tbuf[2] = (USHORT)(TuneLessPrm->KrCurZeta1);
		tbuf[3] = (USHORT)(TuneLessPrm->KrCurZeta1 >> 16);
		tbuf[4] = (USHORT)(TuneLessPrm->KrCurZeta2);
		tbuf[5] = (USHORT)(TuneLessPrm->KrCurZeta2 >> 16);
		MicroTranslatePrmReq( SvAsicRegs, tbuf,
							  &(SvAsicRegs->AsicMicroReg->MREG_TLKCURFRQ), 6 );
	}
#endif /* 2012.08.31 Y.Oka ★μScanA→CpuScanA★ */

	return	ret;
}



/****************************************************************************************************/
/*																									*/
/*		ロバスト補償器パラメータ計算																*/
/*																									*/
/****************************************************************************************************/
static	BOOL	tuneLessCalRobustCompPrm( TUNELESS_CTRL *TuneLessCtrl, BOOL LpassFil3,				  /* <S066> */
										  BPRMDAT *Bprm, TUNELESS_SETPRM *Prm, TRANSDUCE_CTRL *TransduceCtrl )
{
	BOOL				ret;
	LONG				sx;
	LONG				lwk;
	LONG				TuneLessSpdFbLpf;
	LONG				TuneLessTrqLpf;
	LONG				TuneLessVfbLpf2;
	LONG				TuneLessTrqLpf2;
	TUNELESS_DRCOMP		wrkp;
	TUNELESS_PRM		*TuneLessPrm;

	TuneLessPrm = &(TuneLessCtrl->TuneLessPrm);
	ret = TRUE;

	/*----------------------------------------------------------------------------------------------*/
	/*		ローパスフィルタ無効処理																*/
	/*			低ロバスト設定時、およびSGMAV,SGMJVモータ接続時ローパスフィルタを０とする			*/
	/*			13bit以下のエンコーダ分解能時は、調整ﾚｽｹﾞｲﾝが低くなっているのでフィルタ無効			*/
	/*----------------------------------------------------------------------------------------------*/
	if( TuneLessCtrl->conf.TuningLessType == 1 )
	{
		/* フィルタ時定数 = PnEDC */
		TuneLessTrqLpf2 = Prm->TuneLessTrqLpf;
		/* フィルタ時定数 = PnEDD */
		TuneLessVfbLpf2 = Prm->TuneLessVfbLpf;

		if( TuneLessCtrl->conf.TuningLessEx == FALSE )
		{ /* other than New Tuning Less Mode */
			/* SGMAV SGMJVモータ判別 */
			if( ((Prm->MencP->typm_e.b.l & 0xFF) == 0x60) || ((Prm->MencP->typm_e.b.l & 0xFF) == 0x6D) )
			{ /* "SGMAV"モータ || "SGMJV"モータ */
				/* 小容量はフィルタなし	*/
				if( Prm->ampw <= 750 )
				{
					/* PnEDC設定無効 */
					TuneLessTrqLpf2 = 0;
					/* PnEDD設定無効 */
					TuneLessVfbLpf2 = 0;
				}
			}

			if( Bprm->LinearMotorType != FALSE )
			{
				if( Bprm->SpdDetectUnit >= TLSPDDET13BIT )
				{
					/* PnEDC設定無効 */
					TuneLessTrqLpf2 = 0;
					/* PnEDD設定無効 */
					TuneLessVfbLpf2 = 0;
				}
			}
		}
		else
		{ /* New Tuning Less Mode */
			/* PnEDC設定無効 */
			TuneLessTrqLpf2 = 0;
			/* PnEDD設定無効 */
			TuneLessVfbLpf2 = 0;
		}
	}
	else
	{	/*低騒音設定で無い場合は、フィルタ無効 */
		/* PnEDC設定無効 */
		TuneLessTrqLpf2 = 0;
		/* PnEDD設定無効 */
		TuneLessVfbLpf2 = 0;
	}

/*----------------------------------------------------------------------------------------------*/
/*		速度FBフィルタゲイン [0.01ms] : Drcomp.conf.KVfbFil										*/
/*			PnEA8とPnEDDの大きい方を選択														*/
/*----------------------------------------------------------------------------------------------*/
	/*------------------------------------------------------------------------------------------*/
	/*		PnF1E bit3 = 1 では、速度FBフィルタ、トルクフィルタゲイン [0.01ms] にPnEA8を使用	*/
	/*			PnEA8とPnEDDの大きい方を選択													*/
	/*------------------------------------------------------------------------------------------*/
	if( Prm->MencP->flg_wf & 0x0008 )
	{ /* モータ静音設定選択フラグ */
		if( TuneLessCtrl->conf.TuningLessEx == FALSE )
		{ /* other than New Tuning Less Mode */
			/* 調整レス速度FBフィルタ追加 */
			TuneLessSpdFbLpf = Prm->fbfil2;
			/* 調整レストルクフィルタ追加 */
			TuneLessTrqLpf = Prm->fbfil2;
		}
		else
		{
			/* 調整レス速度FBフィルタなし */
			TuneLessSpdFbLpf = 0;
			/* 調整レストルクフィルタなし */
			TuneLessTrqLpf = 0;
		}
	}
	else
	{
		/* 調整レス速度FBフィルタクリア */
		TuneLessSpdFbLpf = 0;
		/* 調整レストルクフィルタクリア */
		TuneLessTrqLpf = 0;
	}

	lwk = MlibMAX( Bprm->SpdFbLpassFil, TuneLessSpdFbLpf );
	lwk = MlibMAX( lwk, TuneLessVfbLpf2 );
#if (FLOAT_USE==TRUE)
	wrkp.conf.KVfbFil = FlibPcalKf1gain( (lwk * 10), (TULESCYCLE / 1000), 0 );
#else
	wrkp.conf.KVfbFil = MlibPcalKf1gain( (lwk * 10), (TULESCYCLE / 1000), 0 );
#endif /* FLOAT_USE */

/*----------------------------------------------------------------------------------------------*/
/*		補償トルクローパスフィルタゲイン [0.01ms] : Drcomp.conf.KCmpOutlpf						*/
/*			PnEA8とPnEDCの大きい方を選択														*/
/*----------------------------------------------------------------------------------------------*/
	if( LpassFil3 == TRUE )
	{ /* トルク補償後トルクフィルタ有効 */
		/* トルク補償後トルクフィルタ有効時はフィルタなし */
		wrkp.conf.KCmpOutlpf = 0;
	}
	else
	{
		if( TuneLessCtrl->conf.TuningLessEx == FALSE )
		{ /* other than New Tuning Less Mode */
			lwk = MlibMAX( TuneLessTrqLpf, TuneLessTrqLpf2 );
		}
		else
		{ /* New Tuning Less Mode */
			/* 1/(2π×2000[Hz])×100000 */
			lwk = TLEX_TRQLPFIL;
		}
#if (FLOAT_USE==TRUE)
		wrkp.conf.KCmpOutlpf = FlibPcalKf1gain( (lwk * 10), (TULESCYCLE / 1000), 0 );
#else
		wrkp.conf.KCmpOutlpf = MlibPcalKf1gain( (lwk * 10), (TULESCYCLE / 1000), 0 );
#endif /* FLOAT_USE */
	}

/*----------------------------------------------------------------------------------------------*/
/*		ロバスト補償器ローパスフィルタゲイン１ [0.1Hz] : Drcomp.conf.Krlpf1						*/
/*		ロバスト補償器ローパスフィルタゲイン２ [0.1Hz] : Drcomp.conf.Krlpf2						*/
/*----------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	wrkp.conf.Krlpf1 = FlibPcalKf1gain( TuneLessPrm->f1Comp, (TULESCYCLE / 1000), 1 );
	wrkp.conf.Krlpf2 = FlibPcalKf1gain( TuneLessPrm->f2, (TULESCYCLE / 1000), 1 );
#else
	wrkp.conf.Krlpf1 = MlibPcalKf1gain( TuneLessPrm->f1Comp, (TULESCYCLE / 1000), 1 );
	wrkp.conf.Krlpf2 = MlibPcalKf1gain( TuneLessPrm->f2, (TULESCYCLE / 1000), 1 );
#endif /* FLOAT_USE */

/*----------------------------------------------------------------------------------------------*/
/*		ロバスト補償器係数 [%] : Drcomp.conf.Kd													*/
/*----------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE) 
	wrkp.conf.Krd = ((float)TuneLessPrm->kd_dist / 100.0F);
#else
	wrkp.conf.Krd = MlibScalKxgain( TuneLessPrm->kd_dist, 1, 100, (LONG*)NULL, 24 );
#endif /* FLOAT_USE */

/*----------------------------------------------------------------------------------------------*/
/*		ロバスト補償器トルク補正ゲイン f1[0.1Hz], kj [%]) : Drcomp.conf.Krsj					*/
/*																								*/
/*					  OvrSpd * Jmot * (100+Prm.jrate)/100 * 2*PAI * ObsJGain[%]	* Lpf[0.1Hz]	*/
/*				  = -----------------------------------------------------------					*/
/*					     		 			MaxTrq 												*/
/*																								*/
/*				  =  kvx * (100+Prm.jrate)/100 * ObsJGain[%] * Lpf[0.1Hz]						*/
/*																								*/
/*----------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE) 
	wrkp.conf.Krsj = Bprm->Kvx * ((100+TuneLessPrm->jrate) * TuneLessPrm->f1Comp )/1000.0f * (TuneLessPrm->kj_dist2 / 100.0F) ;
#else
	wrkp.conf.Krsj = MlibScalKskxkx( Bprm->Kvx, ((100+TuneLessPrm->jrate)*TuneLessPrm->f1Comp), 1000, &sx, 0 );
	wrkp.conf.Krsj = MlibPcalKxgain( wrkp.conf.Krsj, TuneLessPrm->kj_dist2, 100, &sx, 24 );
#endif /* FLOAT_USE */

	/*--------------------------------------------------------------------------------------------------*/
	/*		<S1A5> 																						*/
	/*--------------------------------------------------------------------------------------------------*/
	TuneLessInitRobustCompensator( TuneLessCtrl, &wrkp );

	/*--------------------------------------------------------------------------------------------------*/
	/*		Set Parameters																				*/
	/*--------------------------------------------------------------------------------------------------*/
//	#if 0 /* CPU定数一括変更処理現状未対応 */ /* <S01E> *//* <S066> */
	/* Set Tuning Less Robust Comp Parameter */
	if( TR_READY != TranlateCtrlPrmRequest( TransduceCtrl,
											&wrkp,
											(ULONG*)&(TuneLessCtrl->Drcomp.conf),
											(sizeof(TuneLessCtrl->Drcomp.conf)>>2),
											TR_REQ_SCANA ) )
	{
		ret = FALSE;
	}
//	#else /* <S01E> */
//		TuneLessCtrl->Drcomp.conf = wrkp.conf;
//	#endif /* <S01E> */

	return	ret;
}



/****************************************************************************************************/
/*																									*/
/*		位相補償速度オブザーバパラメータ計算														*/
/*																									*/
/****************************************************************************************************/
static	BOOL	tuneLessCalEhSpdObsPrm( TUNELESS_CTRL *TuneLessCtrl, USHORT trqfil11, BPRMDAT *Bprm,
																		TRANSDUCE_CTRL *TransduceCtrl )
{
	BOOL				ret;
	LONG				wk;
	LONG				sx;
	TUNELESS_EHVOBS		wrkp;
	TUNELESS_PRM		*TuneLessPrm;

	TuneLessPrm = &(TuneLessCtrl->TuneLessPrm);
	ret = TRUE;

	/*----------------------------------------------------------------------------------------------*/
	/*		位相補償速度オブザーバゲイン(Pn127 [Hz]) : EhVobs.conf.KstfX	( X = 1～3)				*/
	/*		トルク指令フィルタ時定数の乗算は，バンク切替え対応のため，								*/
	/*		制御演算中に行うので，ここでは，トルクフィルタ時定数以外を計算する。					*/
	/*----------------------------------------------------------------------------------------------*/
	/*			EhVobs.conf.Kstf1	(wrkp.conf.Kstf1)												*/
	/*																								*/
	/*					 9*(Prm.evobgn)*2π   (Tf[0.01ms])											*/
	/*	 		kstf1 = ------------------- * ------------											*/
	/*							2         	 	 10^5												*/
	/*----------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE) 
	wrkp.conf.Kstf1 = (float)(TuneLessPrm->ks_vobs * 9.0 * PAI / 100000.0f);
#else
	wk = MlibScalKxgain( TuneLessPrm->ks_vobs, 28274, 1000, &sx, 0 );	/* 28274 = 9*π*1000		*/
	wrkp.conf.Kstf1 = MlibPcalKxgain( wk, 1, 100000, &sx, 24 );
#endif /* FLOAT_USE */


	/*----------------------------------------------------------------------------------------------*/
	/*			EhVobs.conf.Kstf2	(wrkp.conf.Kstf2)												*/
	/*																								*/
	/*					  27*(Prm.evobgn)^2*4π^2*SvCycleUs     (Tf[0.01ms])						*/
	/*			kstf2 = ------------------------------------ * --------------						*/
	/*								4*10^6                	  	   10^5								*/
	/*----------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE) 
	wrkp.conf.Kstf2 = (float)TuneLessPrm->ks_vobs * (float)TuneLessPrm->ks_vobs * 27*PAI*PAI* (float)TULESCYCLEUS / 1000000.0f / 100000.0f;
#else
	wk = TuneLessPrm->ks_vobs * TuneLessPrm->ks_vobs;
	wk = MlibScalKxgain( wk, 266479, 1000, &sx, 0 );					/* 266479 = 27*π^2*1000	*/
	wk = MlibPcalKxgain( wk, TULESCYCLEUS, 1000000, &sx, 0 );
	wrkp.conf.Kstf2 = MlibPcalKxgain( wk, 1, 100000, &sx, 24 );
#endif /* FLOAT_USE */

	/*----------------------------------------------------------------------------------------------*/
	/*			EhVobs.conf.Kstf3	(wrkp.conf.Kstf3)												*/
	/*																								*/
	/*					  27*(Prm.evobgn)^3*8π^3*SvCycleUs^2    (Tf[0.01ms])						*/
	/*			kstf3 = -------------------------------------- * ------------						*/
	/*				          			8*10^12				   		10^5							*/
	/*----------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE) 
		wrkp.conf.Kstf3 = (27.0F * (float)TuneLessPrm->ks_vobs * (float)TuneLessPrm->ks_vobs * (float)TuneLessPrm->ks_vobs * 
							PAI * PAI * PAI * 10.0f * (float)TULESCYCLEUS * (float)TULESCYCLEUS) / 1000000.0f / 1000000.0f / 1000000.0f;
#else
	wk = TuneLessPrm->ks_vobs * TuneLessPrm->ks_vobs * TuneLessPrm->ks_vobs;
	wk = MlibScalKxgain( wk, 837169, 1000, &sx, 0 );					/* 837169 = 27*π^3*1000	*/
	wk = MlibPcalKxgain( wk, TULESCYCLEUS, 1000000, &sx, 0 );
	wk = MlibPcalKxgain( wk, TULESCYCLEUS, 1000000, &sx, 0 );
	wrkp.conf.Kstf3 = MlibPcalKxgain( wk, 1, 100000, &sx, 24 );
#endif /* FLOAT_USE */

	/*----------------------------------------------------------------------------------------------*/
	/*		位相補償速度オブザーバイナーシャゲイン(Pn128 [%]) : EhVobs.conf.Kj						*/
	/*----------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	wrkp.conf.Kj = 2 * PAI * (TuneLessPrm->kj_vobs/100.0F) * TULESCYCLEUS / (Bprm->Kvx * ((100+TuneLessPrm->jrate)/100.0F ))/1000000.0f;
#else
	wrkp.conf.Kj = tuneLessCalNorObsJGain( TuneLessPrm, Bprm->Kvx, (TULESCYCLEUS * 1000) );
#endif /* FLOAT_USE */

	/*----------------------------------------------------------------------------------------------*/
	/*		位相補償速度オブザーバローパスフィルタゲイン [0.01ms] : EhVobs.conf.Lpf					*/
	/*		フィルタゲインは( vobsLpf+Pn401 [0.01ms])で計算する										*/
	/*----------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	wrkp.conf.Lpf = FlibPcalKf1gain( (TuneLessPrm->vobsLpf + trqfil11) * 10, TULESCYCLEUS, 0 );
#else
	wrkp.conf.Lpf = MlibPcalKf1gain( (TuneLessPrm->vobsLpf + trqfil11) * 10, TULESCYCLEUS, 0 );
#endif /* FLOAT_USE */

	/*----------------------------------------------------------------------------------------------*/
	/* 		トルクフィルタ時定数算出 																*/
	/* 		フィルタゲインは指数（シフトsx）が24 固定のため24bitシフト								*/
	/* 		Tf = scantime * ((1 / Tf_Gain) -1)														*/
	/*----------------------------------------------------------------------------------------------*/
	if( wrkp.conf.Lpf > 0 )
	{ /* トルク指令フィルタ時定数≠0[ms] */
#if (FLOAT_USE==TRUE)
		wrkp.conf.Tf = (TULESCYCLEUS / wrkp.conf.Lpf) - TULESCYCLEUS;
#else
		wrkp.conf.Tf = (0x1000000 * TULESCYCLEUS / wrkp.conf.Lpf) - TULESCYCLEUS;
#endif /* FLOAT_USE */

		/* [us] -> [0.01ms]単位変換 */
		wrkp.conf.Tf = wrkp.conf.Tf / 10;

		if( wrkp.conf.Tf < EHVOBSTFULIM )
		{ /* トルク指令フィルタ時定数 < 20[us]の場合 */
			wrkp.conf.Tf = EHVOBSTFULIM;
		}
	}
	else
	{ /* トルク指令フィルタ時定数 == 0[ms]の場合 */
		wrkp.conf.Tf = EHVOBSTFULIM;
	}

	/*----------------------------------------------------------------------------------------------*/
	/*		Set Parameters																			*/
	/*----------------------------------------------------------------------------------------------*/
	/* Set Tuning Less EhSpeed Observer Parameter */
//#if 0 /* CPU定数一括変更処理現状未対応 */ /* <S01E> *//* <S066> */
	if( TR_READY != TranlateCtrlPrmRequest( TransduceCtrl,
							&wrkp,
							(ULONG*)&(TuneLessCtrl->EhVobs.conf),
							(sizeof(TuneLessCtrl->EhVobs.conf)>>2),
							TR_REQ_SCANA) )
	{
		ret = FALSE;
	}
//#else /* <S01E> */
//	TuneLessCtrl->EhVobs.conf = wrkp.conf;
//#endif /* <S01E> */
	return	ret;
}



/****************************************************************************************************/
/*																									*/
/*		正規化オブザーバイナーシャゲイン計算 : ObsJGain [%] --> NormalizedObsJGain 					*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*					      MaxTrq * 100 * CycleNs[ns] * ObsJGain[%]									*/
/*				x = ------------------------------------------------								*/
/*					  OvrSpd * Jmot * (100+Prm.jrate) * 10^9 * 100									*/
/*																									*/
/*					  2*PAI * 100 * CycleNs * ObsJGain												*/
/*				  = --------------------------------------											*/
/*					  Kvx * (100+Prm.jrate) * 10^9 * 100											*/
/*																									*/
/****************************************************************************************************/
static	LONG	tuneLessCalNorObsJGain( TUNELESS_PRM *TuneLessPrm, KSGAIN Kvx, LONG scantime )
{
	LONG				x;
	LONG				kx, sx;
	LONG				ky, sy;

	sx = 0;
	kx = MlibPcalKxmulx( TuneLessPrm->kj_vobs2, scantime, C2PAIE7, &sx );
	ky = MlibScalKskxkx( Kvx, C10POW7, 1, &sy, 0 );
	ky = MlibPcalKxmulx( ky, C10POW9, (100+TuneLessPrm->jrate), &sy );
	x = MlibPcalKxdivx( kx, sx, ky, sy, 24 );

	return( x );

}


#if (FLOAT_USE==TRUE)
/****************************************************************************************************/
/*																									*/
/*		電流ループ近似モデル2次フィルタパラメータ計算(双一次変換)									*/
/*		調整レスstep3																				*/
/*																									*/
/*			(Ki/La)*s + Ki/(La*Tic)																	*/
/*		----------------------------------															*/
/*		s^2 + ((Ra+Ki)/La)*s + Ki/(La*Tic)															*/
/*																									*/
/*		Ki=2*pi*q軸電流ループゲイン[Hz]*La															*/
/*		Tic=q軸電流ループ積分時定数[sec]															*/
/*		La=モータ巻き線インダクタンス[H]															*/
/*		Ra=モータ巻き線抵抗[Ω]																		*/
/*																									*/
/****************************************************************************************************/
static float	f_PcalCurLoopLowfilter2(
		float	hz,					/* Ki/(La*Tic)													*/
		float	s_hz,				/* hzのシフト量													*/
		float	zn1,				/* (Ra+Ki)/La													*/
		float	zn2,				/* Ki/La														*/
		float	ts,					/* 演算周期				[us]									*/
		float	kn[5] )				/* 計算結果格納													*/
{
	float	ka, kb, kc, kd, ke;
	float	kx, ky;

	/*----------------------------------------------------------------------------------------------*/
	/*	 周波数補正																					*/
	/*----------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------*/
	/*	 周波数補正																					*/
	/*----------------------------------------------------------------------------------------------*/
	kc = (hz * ts / 1000000) * ts / 1000000; 
	
	kb = (2 * ts / 1000000) * zn1; 

	kx = 4 + kc; 
	ky = 4 - kc; 
	
	ka = kx + kb; 
	kb = kx - kb; 

	kd = kc + (2*zn2*ts/1000000); 
	ke = kc - (2*zn2*ts/1000000); 

	kn[0] = kd / ka;
	kn[1] = 2*kc/ka;
	kn[2] = ke / ka;		
	kn[3] = ky / ka;
	kn[4] = kb / ka;

	return( 0 );
}
#else
/****************************************************************************************************/
/*																									*/
/*		電流ループ近似モデル2次フィルタパラメータ計算(双一次変換)									*/
/*		調整レスstep3																				*/
/*																									*/
/*			(Ki/La)*s + Ki/(La*Tic)																	*/
/*		----------------------------------															*/
/*		s^2 + ((Ra+Ki)/La)*s + Ki/(La*Tic)															*/
/*																									*/
/*		Ki=2*pi*q軸電流ループゲイン[Hz]*La															*/
/*		Tic=q軸電流ループ積分時定数[sec]															*/
/*		La=モータ巻き線インダクタンス[H]															*/
/*		Ra=モータ巻き線抵抗[Ω]																		*/
/*																									*/
/****************************************************************************************************/
static LONG	PcalCurLoopLowfilter2(
		LONG	hz,					/* Ki/(La*Tic)													*/
		LONG	s_hz,				/* hzのシフト量													*/
		LONG	zn1,				/* (Ra+Ki)/La													*/
		LONG	zn2,				/* Ki/La														*/
		LONG	ts,					/* 演算周期				[us]									*/
		LONG	kn[5] )				/* 計算結果格納													*/
{
	LONG	ka, sa, kb, sb, kc, sc, kd, sd, ke, se;
	LONG	kx, sx, ky, sy;
	LONG	k_tmp, s_tmp;

	/*----------------------------------------------------------------------------------------------*/
	/*	 周波数補正																					*/
	/*----------------------------------------------------------------------------------------------*/
	sc = s_hz;
	kc = MlibPcalKxgain( hz,  ts,  1000000,   &sc, 0 );
	kc = MlibPcalKxgain( kc,  ts,  1000000,   &sc, 0 );			/* hz * ts * ts						*/

	sb = 0;
	kb = MlibPcalKxgain( 2,      ts,     1000000, &sb, 0 );
	kb = MlibPcalKxgain( kb,     zn1,    1,       &sb, 0 );		/* 2 * zn1 * ts						*/

	kx = MlibPcalKxaddx( 4<<24,  24,  kc,  sc,  &sx );			/* 4 + (hz * ts * ts)				*/
	ky = MlibPcalKxsubx( 4<<24,  24,  kc,  sc,  &sy );			/* 2 * (4 - hz*ts*ts)				*/
	sy = sy - 1;
	ka = MlibPcalKxaddx( kx,     sx,  kb,  sb,  &sa );			/* 4 + (2 * zn1 * ts + hz * ts * ts)*/
	kb = MlibPcalKxsubx( kx,     sx,  kb,  sb,  &sb );			/* 4 - (2 * zn1 * ts + hz * ts * ts)*/

	s_tmp = 0;
	k_tmp = MlibPcalKxgain( 2, zn2, 1, &s_tmp, 0 );
	k_tmp = MlibPcalKxgain( k_tmp, ts, 1000000, &s_tmp, 0 );
	kd = MlibPcalKxaddx( kc, sc, k_tmp, s_tmp, &sd );			/* (+2 * zn2 * ts) + (hz * ts * ts)	*/
	ke = MlibPcalKxsubx( kc, sc, k_tmp, s_tmp, &se );			/* (-2 * zn2 * ts) + (hz * ts * ts)	*/

	kn[0] = MlibPcalKxdivx( kd,  sd,  ka,  sa,  24  );			/* {kd,sd} / {ka,sa}				*/
	kn[1] = MlibPcalKxdivx( kc,  (sc - 1),  ka,  sa,  24  );	/*  2 * kc / ka						*/
	kn[2] = MlibPcalKxdivx( ke,  se,  ka,  sa,  24  );			/* {ke,se} / {ka,sa}				*/
	kn[3] = MlibPcalKxdivx( ky,  sy,  ka,  sa,  24  );			/* {ky,sy} / {ka,sa}				*/
	kn[4] = MlibPcalKxdivx( kb,  sb,  ka,  sa,  24  );			/* {kb,sb} / {ka,sa}				*/

	return( 0 );
}
#endif /* FLOAT_USE */

/****************************************************************************************************/
/*																									*/
/*		２次バンドパスフィルタパラメータ計算(双一次変換)											*/
/*		調整レスstep3																				*/
/*																									*/
/*			2*zn*(2*pi*hz)*s + (2*pi*hz)^2															*/
/*		------------------------------------														*/
/*		s^2 + 2*zn*(2*pi*hz)*s + (2*pi*hz)^2														*/
/*																									*/
/****************************************************************************************************/
static LONG	PcalBandPassfilter(
		LONG	hz,					/* フィルタ周波数		[Hz]									*/
		LONG	zn,					/* 減衰係数				[0.001]									*/
		LONG	ts,					/* 演算周期				[us]									*/
		LONG	kn[5] )				/* 計算結果格納													*/
{
	LONG	ka, sa, kb, sb, kc, sc, kd, sd, ke, se;
	LONG	kx, sx, ky, sy;

	/*----------------------------------------------------------------------------------------------*/
	/*	 周波数補正																					*/
	/*----------------------------------------------------------------------------------------------*/
	if ( hz > 2000 )
	{
		hz = (LONG)( (SHORT)hz * (SHORT)(3 * hz - 1894) + 11788000) / 10000;
	}

	sc = 15;
	kc = MlibPcalKxgain( hz*hz,  ts*ts,  773019,   &sc, 0 );	/* wn * wn * ts * ts				*/
																/* 773019 = 1000000^2/(2π^2)/2^15	*/
	sb = 0;
	kb = MlibPcalKxgain( hz,     ts,     1000000, &sb, 0 );
	kb = MlibPcalKxgain( kb,     2,      1,       &sb, 0 );
	kb = MlibPcalKxgain( kb,     zn,     1000,    &sb, 0 );
	kb = MlibPcalKxgain( kb,     125664, 10000,   &sb, 0 );		/* 2 * 2 * zn * wn * ts				*/
																/* 125664 = 2 * 2π * 10000			*/

	kx = MlibPcalKxaddx( 4<<24,  24,  kc,  sc,  &sx );			/* 4 + (wn * wn * ts * ts)			*/
	ky = MlibPcalKxsubx( 4<<24,  24,  kc,  sc,  &sy );			/* 2 * (4 - wn * wn * ts * ts)		*/
	sy = sy - 1;
	ka = MlibPcalKxaddx( kx,     sx,  kb,  sb,  &sa );			/* 4 + 2*2*zn*wn*ts + wn*wn*ts*ts	*/
	kb = MlibPcalKxsubx( kx,     sx,  kb,  sb,  &sb );			/* 4 - 2*2*zn*wn*ts + wn*wn*ts*ts	*/

	sd = 0;
	kd = MlibPcalKxsubx( ka,  sa,  4<<24,  24,  &sd );			/* +(2*2*zn*wn*ts) + (wn*wn*ts*ts)	*/

	se = 0;
	ke = MlibPcalKxsubx( kb,  sb,  4<<24,  24,  &se );			/* -(2*2*zn*wn*ts) + (wn*wn*ts*ts)	*/

	kn[0] = MlibPcalKxdivx( kd,  sd,  ka,  sa,  24  );			/* {kd,sd} / {ka,sa}				*/
	kn[1] = MlibPcalKxdivx( kc,  (sc - 1),  ka,  sa,  24  );	/*  2 * K[0]						*/
	kn[2] = MlibPcalKxdivx( ke,  se,  ka,  sa,  24  );			/* {ke,se} / {ka,sa}				*/
	kn[3] = MlibPcalKxdivx( ky,  sy,  ka,  sa,  24  );			/* {ky,sy} / {ka,sa}				*/
	kn[4] = MlibPcalKxdivx( kb,  sb,  ka,  sa,  24  );			/* {kb,sb} / {ka,sa}				*/

	return( 0 );
}



/****************************************************************************************************/
/*																									*/
/*		2次ハイパスフィルタパラメータ計算(双一次変換)												*/
/*		調整レスstep3																				*/
/*																									*/
/*				2*zn*(s^2) + (2*pi*hz)*s															*/
/*		------------------------------------														*/
/*		s^2 + 2*zn*(2*pi*hz)*s + (2*pi*hz)^2														*/
/*																									*/
/****************************************************************************************************/
static LONG	PcalHighPassfilter(
		LONG	hz,					/* フィルタ周波数		[Hz]									*/
		LONG	zn,					/* 減衰係数				[0.001]									*/
		LONG	ts,					/* 演算周期				[us]									*/
		LONG	kn[5] )				/* 計算結果格納													*/
{
	LONG	ka, sa, kb, sb, kc, sc, kd, sd, ke, se, kf, sf;
	LONG	kx, sx, ky, sy;
	LONG	k_tmp1, s_tmp1, k_tmp2, s_tmp2;

	/*----------------------------------------------------------------------------------------------*/
	/*	 周波数補正																					*/
	/*----------------------------------------------------------------------------------------------*/
	if ( hz > 2000 )
	{
		hz = (LONG)( (SHORT)hz * (SHORT)(3 * hz - 1894) + 11788000) / 10000;
	}

	sc = 15;
	kc = MlibPcalKxgain( hz*hz,  ts*ts,  773019,   &sc, 0 );	/* wn * wn * ts * ts				*/
																/* 773019 = 1000000^2/(2π^2)/2^15	*/
	sb = 0;
	kb = MlibPcalKxgain( hz,     ts,     1000000, &sb, 0 );
	kb = MlibPcalKxgain( kb,     2,      1,       &sb, 0 );
	kb = MlibPcalKxgain( kb,     zn,     1000,    &sb, 0 );
	kb = MlibPcalKxgain( kb,     125664, 10000,   &sb, 0 );		/* 2 * 2 * zn * wn * ts				*/
																/* 125664 = 2 * 2π * 10000			*/

	kx = MlibPcalKxaddx( 4<<24,  24,  kc,  sc,  &sx );			/* 4 + (wn * wn * ts * ts)			*/
	ky = MlibPcalKxsubx( 4<<24,  24,  kc,  sc,  &sy );			/* 2 * (4 - (wn * wn * ts * ts))	*/
	sy = sy - 1;
	ka = MlibPcalKxaddx( kx,     sx,  kb,  sb,  &sa );			/* 4 + 2*2*zn*wn*ts + wn*wn*ts*ts	*/
	kb = MlibPcalKxsubx( kx,     sx,  kb,  sb,  &sb );			/* 4 - 2*2*zn*wn*ts + wn*wn*ts*ts	*/

	s_tmp1 = 0;
	k_tmp1 = MlibPcalKxgain( 8, zn, 1000, &s_tmp1, 0 );			/* 8 * zn							*/
	s_tmp2 = 0;
	k_tmp2 = MlibPcalKxgain( 1256637, hz, 100000, &s_tmp2, 0 );	/* 2 * (2 * pi * hz)				*/
																/* 1256637 = 2 * 2 * pi * 100000	*/
	k_tmp2 = MlibPcalKxgain( k_tmp2, ts, 1000000, &s_tmp2, 0 );	/* 2 * (2 * pi * hz) * ts			*/

	kd = MlibPcalKxaddx( k_tmp1, s_tmp1, k_tmp2, s_tmp2, &sd );	/* (8 * zn) + 2 * (2 * pi * hz) * ts*/
	kf = MlibPcalKxsubx( k_tmp1, s_tmp1, k_tmp2, s_tmp2, &sf );	/* (8 * zn) - 2 * (2 * pi * hz) * ts*/

	se = 0;
	ke = MlibPcalKxgain( -16, zn, 1000, &se, 0 );				/* -16 * zn							*/

	kn[0] = MlibPcalKxdivx( kd,  sd,  ka,  sa,  24  );			/* {kd,sd} / {ka,sa}				*/
	kn[1] = MlibPcalKxdivx( ke,  se,  ka,  sa,  24  );			/* {ke,se} / {ka,sa}				*/
	kn[2] = MlibPcalKxdivx( kf,  sf,  ka,  sa,  24  );			/* {kf,sf} / {ka,sa}				*/
	kn[3] = MlibPcalKxdivx( ky,  sy,  ka,  sa,  24  );			/* {ky,sy} / {ka,sa}				*/
	kn[4] = MlibPcalKxdivx( kb,  sb,  ka,  sa,  24  );			/* {kb,sb} / {ka,sa}				*/

	return( 0 );
}



/****************************************************************************************************/
/*																									*/
/*		外乱オブザーバ選択スイッチ(Pn408.3)															*/
/*																									*/
/****************************************************************************************************/
/*		戻り値	：	Error Status (PRM_SETTING_OK:エラーなし, PRM_SETTING_NG:パラメータ設定異常)		*/
/****************************************************************************************************/
LONG	DobsCalculatePrmSW( BOOL *DobsAct, BOOL TuningLess, USHORT DobsSel )
{
	LONG	PrmSetting;
	PrmSetting = PRM_SETTING_OK;

	if( TuningLess == FALSE )
	{
		switch( DobsSel & 0x0F )
		{
			case 0x00:
				*DobsAct = FALSE;
				break;
			case 0x01:
				*DobsAct = TRUE;
				break;
			default :
				*DobsAct = FALSE;
				PrmSetting = PRM_SETTING_NG;
				break;
		}
	}
	return	PrmSetting;
}



/****************************************************************************************************/
/*																									*/
/*		外乱オブザーバパラメータ計算																*/
/*																									*/
/****************************************************************************************************/
VOID	DobsCalculatePrm( DOBS_PRM *DobsPrm, ASICS *SvAsicRegs,
						  BPRMDAT *Bprm, DOBS_CFGPRM *CfgPrm, LONG GselNo )
{
#if (FLOAT_USE==TRUE)
	float		sx;
	DOBS_PRM	wrkp;
	float		wk;
	float		wrk_dobsgn;
	float		wrk_dobsgn2;
	float		Loophz;							/* 速度ループゲイン[0.1Hz]							*/
	float		DobsGain;						/* 外乱オブザーバKsゲイン割合[%]					*/
	float		Loophz2;						/* 第2速度ループゲイン[0.1Hz]						*/
	float		DobsGain2;						/* 第2外乱オブザーバKsゲイン割合[%]					*/
	float		DtrqGain;						/* 外乱オブザーバ補償係数[%]						*/
	float		Dlpfil;							/* 外乱オブザーバLPFフィルタ周波数補正[0.1Hz]		*/
	float		DobsjGain;						/* 外乱オブザーバイナーシャ補正	[%]					*/
#else
	LONG		sx;
	DOBS_PRM	wrkp;
	LONG		wk;
	LONG		wrk_dobsgn;
	LONG		wrk_dobsgn2;
	ULONG		Loophz;							/* 速度ループゲイン[0.1Hz]							*/
	ULONG		DobsGain;						/* 外乱オブザーバKsゲイン割合[%]					*/
	ULONG		Loophz2;						/* 第2速度ループゲイン[0.1Hz]						*/
	ULONG		DobsGain2;						/* 第2外乱オブザーバKsゲイン割合[%]					*/
	ULONG		DtrqGain;						/* 外乱オブザーバ補償係数[%]						*/
	LONG		Dlpfil;							/* 外乱オブザーバLPFフィルタ周波数補正[0.1Hz]		*/
	ULONG		DobsjGain;						/* 外乱オブザーバイナーシャ補正	[%]					*/
#endif /* FLOAT_USE */

#if 0 /* 2012.08.31 Y.Oka ★μScanA→CpuScanA★ */
	USHORT		tbuf[10];
	VUSHORT		*MpReg;
#endif

	/* ローカル変数の初期化 */
	sx = 0;
	Loophz		= CfgPrm->loophz;
	DobsGain	= CfgPrm->dobgn;
	Loophz2		= CfgPrm->loophz2;
	DobsGain2	= CfgPrm->dobgn2;
	DtrqGain	= CfgPrm->dtrqgn;
	Dlpfil		= (LONG)((SHORT)CfgPrm->dlpfil);
	DobsjGain	= CfgPrm->dobjgn;

#if 0 /* 2012.08.31 Y.Oka ★μScanA→CpuScanA★ */
	if( (GselNo + 1) == 1 )
	{ /* 第1ゲイン計算時 */
		MpReg = &(SvAsicRegs->AsicMicroReg->MREG_BNK1DOBSKS);
	}
	else if( (GselNo + 1) == 2 )
	{ /* 第2ゲイン計算時 */
		MpReg = &(SvAsicRegs->AsicMicroReg->MREG_BNK2DOBSKS);
	}
	else
	{
		MpReg = NULL;
	}
#endif

	/*----------------------------------------------------------------------------------------------*/
	/*		外乱オブザーバゲイン(Pn121 [0.1Hz]) : Dobs.P.Ks	= Kv(Prm.loophz)[0.1Hz]					*/
	/*----------------------------------------------------------------------------------------------*/

#if (FLOAT_USE==TRUE)
			switch( GselNo )	// <V228>
		{
		case 0 :
			wrk_dobsgn = DobsGain * Loophz/100.0f;
			break;
		case 1 :
			wrk_dobsgn = DobsGain2 * Loophz2/100.0f;
			break;
		default: /* 異常時 */
			wrk_dobsgn = DobsGain * Loophz/100.0f;
			break;
		}
		wrkp.Ks = f_BpxNorObsGainCal( wrk_dobsgn, DOBSCYCLE);
#else
		switch( GselNo )	// <V228>
		{
		case 0 :
			wrk_dobsgn = MlibScalKxgain( (LONG)DobsGain, (LONG)Loophz, 100, NULL, -24 );
			break;
		case 1 :
			wrk_dobsgn = MlibScalKxgain( (LONG)DobsGain2, (LONG)Loophz2, 100, NULL, -24 );
			break;
		default: /* 異常時 */
			wrk_dobsgn = MlibScalKxgain( (LONG)DobsGain, (LONG)Loophz, 100, NULL, -24 );
			break;
		}
		wrkp.Ks = BpxNorObsGainCal( wrk_dobsgn, DOBSCYCLE);
#endif /* FLOAT_USE */
	/*----------------------------------------------------------------------------------------------*/
	/*		外乱オブザーバイナーシャゲイン(Pn125 [%]) : Dobs.P.Kj									*/
	/*----------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
		wrkp.Kj = f_BpxNorObsJGainCal( Bprm->Kvx, CfgPrm->jrate, DobsjGain, DOBSCYCLE );
#else
	wrkp.Kj = BpxNorObsJGainCal( Bprm->Kvx, CfgPrm->jrate, DobsjGain, DOBSCYCLE );
#endif /* FLOAT_USE */
	/*----------------------------------------------------------------------------------------------*/
	/*		外乱オブザーバトルクゲイン(Pn121 [0.1Hz], Pn125 [%]) : Dobs.P.Ksj						*/
	/*																								*/
	/*					  OvrSpd * Jmot * (100+Prm.jrate)/100 * 2*PAI * ObsGain[%]					*/
	/*				  = -----------------------------------------------------------					*/
	/*					      			MaxTrq * ObsJGain											*/
	/*																								*/
	/*					  kvx * (100+Prm.jrate)/100 * ObsGain[%]									*/
	/*				  = --------------------------------------------								*/
	/*					      			ObsJGain[%]													*/
	/*----------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	wrkp.Ksj = Bprm->Kvx * (float)(100+CfgPrm->jrate) * wrk_dobsgn / 1000.0f * 100.0f / (DobsjGain/100.0F) ;
#else
	wrkp.Ksj = MlibScalKskxkx( Bprm->Kvx, (100 + CfgPrm->jrate ) * wrk_dobsgn,  1000, &sx, 0 );
	wrkp.Ksj = MlibPcalKxgain( wrkp.Ksj, 100, DobsjGain, &sx, 24 );
#endif /* FLOAT_USE */

	/*----------------------------------------------------------------------------------------------*/
	/*		外乱トルク係数(Pn123 [%]) : Dobs.P.Kd													*/
	/*----------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	wrkp.Kd = DtrqGain / 100.0F;
#else
	wrkp.Kd = MlibScalKxgain( DtrqGain, 1, 100, NULL, 24 );
#endif /* FLOAT_USE */

	/*----------------------------------------------------------------------------------------------*/
	/*		外乱オブザーバローパスフィルタゲイン(ｵﾌﾞｻﾞｰﾊﾞｹﾞｲﾝ[0.1Hz],補正値 [0.1Hz]) : Dobs.P.Klpf	*/
	/*																								*/
	/*			フィルタ周波数 = オブザーバゲイン×４＋補正											*/
	/*						   = Prm.dlpfil + (Prm.dobgn * 4)										*/
	/*																								*/
	/*----------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	wrk_dobsgn = (DobsGain) * Loophz/100.0F;
	wrk_dobsgn2 = (DobsGain2) * Loophz2/100.0F;
#else
	wrk_dobsgn = MlibScalKxgain( (LONG)DobsGain, (LONG)Loophz, 100, NULL, -24 );
	wrk_dobsgn2 = MlibScalKxgain( (LONG)DobsGain2, (LONG)Loophz2, 100, NULL, -24 );
#endif /* FLOAT_USE */
	if( wrk_dobsgn > wrk_dobsgn2 )
	{
		/*wrk_dobsgn = wrk_dobsgn;*/
	}
	else
	{
		wrk_dobsgn = wrk_dobsgn2;
	}
//	wk = (LONG)((LONG)Axis->Prm->dlpfil) + (wrk_dobsgn * 4);
	wk = Dlpfil + (wrk_dobsgn * 4);
	if(wk < 0)
	{
		wk = 0;														/* 下限リミット					*/
	}
#if (FLOAT_USE==TRUE)
	wrkp.Klpf = FlibPcalKf1gain( wk, (DOBSCYCLE/1000), 1 );
#else
	wrkp.Klpf = MlibPcalKf1gain( wk, (DOBSCYCLE/1000), 1 );
#endif /* FLOAT_USE */

	/*----------------------------------------------------------------------------------------------*/
	/* ゲイン切り替え用データ更新(CPU保管用)														*/
	/*----------------------------------------------------------------------------------------------*/
	*DobsPrm = wrkp;

}



/****************************************************************************************************/
/*																									*/
/*		位相補償速度オブザーバ選択スイッチ															*/
/*																									*/
/****************************************************************************************************/
BOOL	EhVobsCalculatePrmSW( BOOL *EhVobsUse, BOOL TuningLess, USHORT EhVobsSel )					  /* <S066> */
{
	UCHAR	ucwk;
	LONG	PrmSetting;

	PrmSetting = PRM_SETTING_OK;

	if( TuningLess == FALSE )
	{
		ucwk = EhVobsSel & 0x0F;
		switch( ucwk )
		{
		case 0x00:
		case 0x01:
		case 0x02:
		case 0x03:
		case 0x04:
			/* 位相補償速度オブザーバ無効 */
			*EhVobsUse = FALSE;
			break;
		case 0x05:
			/* 位相補償速度オブザーバ有効 */
			*EhVobsUse = TRUE;
			break;
		default :
			*EhVobsUse = FALSE;
			PrmSetting = PRM_SETTING_NG;
			break;
		}
	}
	else
	{
		*EhVobsUse = FALSE;
	}

	return	PrmSetting;
}



/****************************************************************************************************/
/*																									*/
/*		位相補償速度オブザーバパラメータ計算														*/
/*																									*/
/****************************************************************************************************/
BOOL	EhVobsCalculatePrm( EHVOBS_CTRL *EhVobsCtrl, BPRMDAT *Bprm,									  /* <S066> */
											EHVOBS_CFGPRM *CfgPrm, TRANSDUCE_CTRL *TransduceCtrl )
{
	BOOL			ret;
	EHVOBS_PRM		wrkp;
	LONG			wk;
	LONG			sx;

	ret = TRUE;

	/*----------------------------------------------------------------------------------------------*/
	/*		位相補償速度オブザーバゲイン(Pn127 [Hz]) : EhVobs.P.KstfX	( X = 1～3)					*/
	/*		トルク指令フィルタ時定数の乗算は，バンク切替え対応のため，								*/
	/*		制御演算中に行うので，ここでは，トルクフィルタ時定数以外を計算する。					*/
	/*----------------------------------------------------------------------------------------------*/
	/*			EhVobs.P.Kstf1	(wrkp.Kstf1)														*/
	/*																								*/
	/*					 9*(Prm.evobgn)*2π   (Tf[0.01ms])											*/
	/*	 		kstf1 = ------------------- * ------------											*/
	/*							2         	 	 10^5												*/
	/*----------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	wrkp.Kstf1 = 9.0F * (float)CfgPrm->evobgn * PAI / 100000;
#else
	wk = MlibScalKxgain( (LONG)(CfgPrm->evobgn), 28274, 1000, &sx, 0 );	/* 28274 = 9*π*1000		*/
	wrkp.Kstf1 = MlibPcalKxgain( wk, 1, 100000, &sx, 24 );
#endif /* FLOAT_USE */

	/*----------------------------------------------------------------------------------------------*/
	/*			EhVobs.P.Kstf2	(wrkp.Kstf2)														*/
	/*																								*/
	/*					  27*(Prm.evobgn)^2*4π^2*SvCycleUs     (Tf[0.01ms])						*/
	/*			kstf2 = ------------------------------------ * --------------						*/
	/*								4*10^6                	  	   10^5								*/
	/*----------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	wrkp.Kstf2 = 27.0F * (float)CfgPrm->evobgn * (float)CfgPrm->evobgn * PAI * PAI * (float)EHSOBSCYCLE  / 1000000.0f / 100000.0f;
#else
	wk = (LONG)(CfgPrm->evobgn) * (LONG)(CfgPrm->evobgn);
	wk = MlibScalKxgain( wk, 266479, 1000, &sx, 0 );					/* 266479 = 27*π^2*1000	*/
	wk = MlibPcalKxgain( wk, EHSOBSCYCLE, 1000000, &sx, 0 );
	wrkp.Kstf2 = MlibPcalKxgain( wk, 1, 100000, &sx, 24 );
#endif /* FLOAT_USE */

	/*----------------------------------------------------------------------------------------------*/
	/*			EhVobs.P.Kstf3	(wrkp.Kstf3)														*/
	/*																								*/
	/*					  27*(Prm.evobgn)^3*8π^3*SvCycleUs^2    (Tf[0.01ms])						*/
	/*			kstf3 = -------------------------------------- * ------------						*/
	/*				          			8*10^12				   		10^5							*/
	/*----------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	wrkp.Kstf3 = 27.0f * (float)CfgPrm->evobgn * (float)CfgPrm->evobgn * (float)CfgPrm->evobgn * (float)PAI * (float)PAI * (float)PAI * (float)EHSOBSCYCLE * (float)EHSOBSCYCLE / 1000000000000.0f / 100000.0f;
#else
	wk = (LONG)(CfgPrm->evobgn) * (LONG)(CfgPrm->evobgn) * (LONG)(CfgPrm->evobgn);
	wk = MlibScalKxgain( wk, 837169, 1000, &sx, 0 );					/* 837169 = 27*π^3*1000	*/
	wk = MlibPcalKxgain( wk, EHSOBSCYCLE, 1000000, &sx, 0 );
	wk = MlibPcalKxgain( wk, EHSOBSCYCLE, 1000000, &sx, 0 );
	wrkp.Kstf3 = MlibPcalKxgain( wk, 1, 100000, &sx, 24 );
#endif /* FLOAT_USE */

	/*----------------------------------------------------------------------------------------------*/
	/*		位相補償速度オブザーバイナーシャゲイン(Pn128 [%]) : EhVobs.P.Kj							*/
	/*----------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	wrkp.Kj = f_BpxNorObsJGainCal( Bprm->Kvx, CfgPrm->jrate, CfgPrm->evobjgn , (EHSOBSCYCLE * 1000) );
#else
	wrkp.Kj = BpxNorObsJGainCal( Bprm->Kvx, CfgPrm->jrate, CfgPrm->evobjgn , (EHSOBSCYCLE * 1000) );
#endif /* FLOAT_USE */

	/*----------------------------------------------------------------------------------------------*/
	/*		位相補償速度オブザーバローパスフィルタゲイン(Pn13B [0.01ms]) : EhVobs.P.Lpf				*/
	/*		フィルタゲインは( (Pn13B:Prm.evobfil)+(Pn401:Prm.trqfil11) [0.01ms])で計算する			*/
	/*----------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	wrkp.Lpf = FlibPcalKf1gain( (CfgPrm->trqfil11)*10, EHSOBSCYCLE, 0 );
#else
	wrkp.Lpf = MlibPcalKf1gain( (CfgPrm->trqfil11)*10, EHSOBSCYCLE, 0 );
#endif /* FLOAT_USE */

	/*----------------------------------------------------------------------------------------------*/
	/*		位相補償速度オブザーバトルク指令ローパスフィルタゲイン(Pn13C[0.01ms]) 					*/
	/*----------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	wrkp.KTrqReflpf = FlibPcalKf1gain( 0.0, EHSOBSCYCLE, 0 );
#else
	wrkp.KTrqReflpf = MlibPcalKf1gain( 0, EHSOBSCYCLE, 0 );
#endif /* FLOAT_USE */

	/*----------------------------------------------------------------------------------------------*/
	/* 		トルクフィルタ時定数算出 																*/
	/* 		フィルタゲインは指数（シフトsx）が24 固定のため24bitシフト								*/
	/* 		Tf = scantime * ((1 / Tf_Gain) -1)														*/
	/*----------------------------------------------------------------------------------------------*/
	if( wrkp.Lpf > 0 )
	{ /* トルク指令フィルタ時定数≠0[ms] */
		wrkp.Tf = (0x1000000 * EHSOBSCYCLE / wrkp.Lpf) - EHSOBSCYCLE;
		/* [us] -> [0.01ms]単位変換 */
		wrkp.Tf = wrkp.Tf / 10;

		if( wrkp.Tf < EHVOBSTFULIM )
		{ /* トルク指令フィルタ時定数 < 20[us]の場合 */
			wrkp.Tf = EHVOBSTFULIM;
		}
	}
	else
	{ /* トルク指令フィルタ時定数 == 0[ms]の場合 */
		wrkp.Tf = EHVOBSTFULIM;
	}


//#if 0 /* <S01E> *//* <S066> */
	/*----------------------------------------------------------------------------------------------*/
	/*		Set Parameters																			*/
	/*----------------------------------------------------------------------------------------------*/
	/* Set Tuning Less EhSpeed Observer Parameter */
	if( TR_READY != TranlateCtrlPrmRequest( TransduceCtrl,
											&wrkp,
											(ULONG*)&(EhVobsCtrl->EhVobsPrm),
											(sizeof(EhVobsCtrl->EhVobsPrm)>>2),
											TR_REQ_SCANA ) )
	{
		ret = FALSE;
	}
//#else /* <S01E> */
//	EhVobsCtrl->EhVobsPrm = wrkp;
//#endif /* <S01E> */
	/*----------------------------------------------------------------------------------------------*/

	return	ret;
}



/***************************************** end of file **********************************************/
