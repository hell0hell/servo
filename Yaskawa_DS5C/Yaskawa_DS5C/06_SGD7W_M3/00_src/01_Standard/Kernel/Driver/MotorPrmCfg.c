/****************************************************************************************************/
/*																									*/
/*																									*/
/*		MotorPrmCfg.c : モータエンコーダ関連処理モジュール											*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : モータエンコーダ関連処理関数を定義。													*/
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
/*	Note	:	初版	2009.06.04	Y.Oka															*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"
#include "SerialEncIf.h"
#include "JL086ServoIpHw.h"
#include "MLib.h"
#include "KLib.h"
#include "PnPrmListTbl.h"

#define	SVCDEF_DIVOUT_MAXFREQ		1802240				/* 分周出力パルス上限周波数定義				*/
#define	SVFSEL_INCPOUT				1					/* インクレパルス出力						*/
#define	SVFSEL_PAOPOUT				1					/* ＰＡＯ相パルス出力						*//*<S02D>*/

#define	UNIT_FEMT					-15
#define	UNIT_PICO					-12
#define	UNIT_NANO					-9

#define	EvenAsciiCode_A				( 'A'  + 0x00 )		/* 0 100 0001	 <S14F> */
#define	EvenAsciiCode_L				( 'L'  + 0x80 )		/* 1 100 1100	 <S14F> */
#define	EvenAsciiCode_M				( 'M'  + 0x00 )		/* 0 100 1101	 <S14F> */
#define	EvenAsciiCode_P				( 'P'  + 0x00 )		/* 0 101 0000	 <S14F> */
#define	EvenAsciiCode_Puls			( '+'  + 0x00 )		/* 0 010 1011	 <S14F> */
#define	EvenAsciiCode_Conm			( ','  + 0x80 )		/* 1 010 1100	 <S14F> */
#define	EvenAsciiCode_Mins			( '-'  + 0x00 )		/* 0 010 1101	 <S14F> */
#define	EvenAsciiCode_Prio			( '.'  + 0x00 )		/* 0 010 1110	 <S14F> */
#define	EvenAsciiCode_Cr			( 0x0D + 0x80 )		/* 1 000 1101	 <S14F> */

/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
static	LONG	RmxSencCalMencDivOverSpeed( LONG pgrate, KSGAIN OvrSpd );
static DIVSET_ERROR RmxSencSetMencDividedPulse( ASIC_HW_IF_STR *AsicHwReg, MENCV *MencV, UNI_PRM_STR *UniPrms );
static DIVSET_ERROR LmxSencSetMencDividedPulse( ASIC_HW_IF_STR *AsicHwReg, MENCV *MencV, UNI_PRM_STR *UniPrms );
static DIVSET_ERROR LmxSencCalMencDivOverSpeed( MENCV *MencV, UNI_PRM_STR *UniPrms );

static LONG	SencCheckMturnLimit( LONG MtnCur, MENCV *MencV );							  			  /*<S02D>*/
static void	LmxSencGetLinearAbsPos( MENCV  *MencV, BOOL RvsDir ); 									  /*<S02D>*/
static void	SencOutputPaoMtdata( MENCV *MencV, ASIC_HW_IF_STR *pAsicHwReg, ULONG digit );  			  /*<S02D>*/
static DIVSET_ERROR RmxSencSetMencDividedPulseHw( ASIC_HW_IF_STR *AsicHwReg, MENCV *MencV, UNI_PRM_STR *UniPrms );/*<S02D>*/
static DIVSET_ERROR LmxSencSetMencDividedPulseHw( ASIC_HW_IF_STR *AsicHwReg, MENCV *MencV, UNI_PRM_STR *UniPrms );/*<S079>*/
static BOOL SencSetSensorType( MENCV *MencV, MENCPRM *MencP, USHORT b_prm2 );//<S0C7>
void SencOutputPsoSerial    (MENCV*,ASIC_HW_IF_STR*,SHORT,BOOL);/*<S189>*//*<S0C2>*/
void FencSencOutputPsoSerial(MENCV*,ASIC_HW_IF_STR*,SHORT,BOOL);/*<189>*//*<S0C2>*/
static void	OutputPsoMturnIncre( MENCV*,ULONG, ULONG,ASIC_HW_IF_STR*);/*<S189>*//*<S0C2>*/
static void	OutputPsoLinearIncre(LONG *PsoIncPulseOut, ULONG digit, ASIC_HW_IF_STR *pAsicHwReg);	/* <S20A>:引数見直し */


/****************************************************************************************************/
/*																									*/
/*		リニアモータ：スケールピッチ算出処理														*/
/*																									*/
/****************************************************************************************************/
static BOOL	SencScalePitchCalc( MENCV *MencV )
{
	LONG			i;
	LONG			Index;
	ULONG			ulwk64[2];
	ULONG			ScalePitch;
	ULONG			ScaleExp;

/*--------------------------------------------------------------------------------------------------*/
/*		スケールピッチ計算	単位：[fm]																*/
/*--------------------------------------------------------------------------------------------------*/
	ScalePitch = (MencV->ScalePitch[2]<<16) | (MencV->ScalePitch[1]<<8) | MencV->ScalePitch[0];
	ScaleExp = 1;
	for( i=0; i < MencV->ScalePitchExp; i++ )
	{
		ScaleExp = 10 * ScaleExp;
	}
	MlibMulu32u32( ScalePitch, ScaleExp, ulwk64 );

/*--------------------------------------------------------------------------------------------------*/
/*		スケールピッチ上下限チェック																*/
/*			下限値：0x0000F4240(=1000000)[fm]														*/
/*			上限値：0x5AF3FFFFFFFF(=100004018520063)[fm]											*/
/*--------------------------------------------------------------------------------------------------*/
	if( ulwk64[1]==0 )
	{
		if( ulwk64[0] < 0x000F4240 )
		{ /* 下限値より小さい */
			return( FALSE );
		}
	}
	else
	{
		if( ulwk64[1] > 0x00005AF3 )
		{ /* 上限値より大きい */
			return( FALSE );
		}
	}

/*--------------------------------------------------------------------------------------------------*/
/*		モニタ用スケールピッチ、スケールピッチ指数設定	単位：[pm]									*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->MonScalePitch = ScalePitch;
	MencV->MonScaleExp = UNIT_FEMT - UNIT_PICO + MencV->ScalePitchExp;/* [pm]単位の指数に変換	*/

/*--------------------------------------------------------------------------------------------------*/
/*		スケールピッチ、スケールピッチ指数設定	単位：[nm]											*/
/*--------------------------------------------------------------------------------------------------*/
	Index = UNIT_FEMT - UNIT_NANO + MencV->ScalePitchExp;				/* [nm]単位の指数に変換	*/

	if( Index >= 0 )
	{
		ScaleExp = 1;
		for( i=0; i < Index; i++ )
		{
			ScaleExp = 10 * ScaleExp;
		}
		MencV->NormScalePitch = MlibScalKxgain( ScalePitch, ScaleExp, 1, NULL, -30 );
		MencV->NormScaleExp   = 1;
	}
	else
	{
		ScaleExp = 1;
		Index = -Index;
		for( i=0; i < Index; i++ )
		{
			ScaleExp = 10 * ScaleExp;
		}

		if( ScaleExp >= 10000)
		{
			MencV->NormScalePitch = MlibScalKxgain( ScalePitch, 10000, ScaleExp, NULL, -30 );
			MencV->NormScaleExp   = 10000;
		}
		else
		{
			MencV->NormScalePitch = ScalePitch;
			MencV->NormScaleExp   = ScaleExp;
		}
	}
	return( TRUE );
}


/****************************************************************************************************/
/*																									*/
/*		回転型：エンコーダ基本パラメータ計算														*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : サーボ基本パラメータ計算(BprmCalc.c)に必要なエンコーダパラメータの計算を行う。			*/
/*			基本パラメータ計算結果(Bprm.Xxxx/Hprm.Xxxx)および初期パラメータ計算結果(Iprm.Xxxx)		*/
/*			を使用しないパラメータ計算は、ここで行っても良い。										*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static SENC_ERROR	SencBaseParamCalcForRotary( MENCV *MencV, MENC_INIT_PRM *MencInitPrm )
{
	/*----------------------------------------------------------------------------------------------*/
	/*		通信異常検出レベル設定																	*/
	/*----------------------------------------------------------------------------------------------*/
	MencV->ComErrLimit = MencInitPrm->spgfail.b.l;

	/*----------------------------------------------------------------------------------------------*/
	/*		エンコーダパルス数の設定																*/
	/*----------------------------------------------------------------------------------------------*/
	/*																								*/
	/*		PulseNo = 2^BitNo	[pulse/rev]															*/
	/*																								*/
	/*----------------------------------------------------------------------------------------------*/
	MencV->PulseNo = (1 << MencInitPrm->MencP->bit_dp.b.l);			/* PulseNo = 2^BitNo			*/
	MencV->HalfPulseNo = (MencV->PulseNo >> 1);						/* HalfPulseNo = PulseNo / 2	*/

	MencV->EncBitNo = MencInitPrm->MencP->bit_dp.b.l;				/* エンコーダビット数			*/	/* <S14B> */

#if 0 /*<S02D>*/
	if( MencInitPrm->MencP->bit_dp.b.l <= 13 )						/* パルス変換13bit指定			*/
	{
		MencV->PlsTrn13bit = TRUE;
	}
	else
	{
		MencV->PlsTrn13bit = FALSE;
	}
#else/*<S02D>*/
	if( MencInitPrm->MencP->bit_dp.b.l <= 13 )						/* パルス変換13bit指定			*/
	{
		MencV->DivInfo.PlsTrn13bit = TRUE;
	}
	else
	{
		MencV->DivInfo.PlsTrn13bit = FALSE;
	}

#endif/*<S02D>*/
/*--------------------------------------------------------------------------------------------------*/
/*		パワー出力モータ相順設定																	*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->PhaseOrderUWV = FALSE;								/* 現在、不使用					*/
/*--------------------------------------------------------------------------------------------------*/
/*		ポールセンサ有無設定												<V250>					*/
/*			回転型他社モータ対応を考慮しない形である。												*/
/*			安川モータはポールセンサありのためホールセンサ有無を固定にする							*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->withPoleSensor = TRUE;								/* 安川モータのみ対応			*/
/*--------------------------------------------------------------------------------------------------*/
/*		マルチターンリミットチェック																*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->DivInfo.Limmlt = MencInitPrm->MencP->limmlt;   /*<S02D>*/
	MencV->DivInfo.Bit_dp = MencInitPrm->MencP->bit_dp;   /*<S02D>*/

#if !defined(CSW_NETIF_SYNQNET_TYPE)
	if( (MencV->AbsoEncType == MENC_MABSO_TYPE)
	/*  ||((MencV->AbsoEncType == MENC_SABSO_TYPE)&&(Kprm.NetId == NETID_DPRAM)) */ )	/*<S03F>*/
	{
		if(/* ( MencV->DivOut == TRUE ) &&*/ ( MencInitPrm->mtlmt != MencInitPrm->MencP->limmlt ) )
		{
			return SENCERR_MTLMT;/* A.CC0 : マルチターンリミット値不一致 */
		}
	}
#endif
	return SENCERR_SUCCESS;
}


/****************************************************************************************************/
/*																									*/
/*		リニアモータ：エンコーダ基本パラメータ計算													*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : サーボ基本パラメータ計算(BprmCalc.c)に必要なエンコーダパラメータの計算を行う。			*/
/*			基本パラメータ計算結果(Bprm.Xxxx/Hprm.Xxxx)および初期パラメータ計算結果(Iprm.Xxxx)		*/
/*			を使用しないパラメータ計算は、ここで行っても良い。										*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static SENC_ERROR	SencBaseParamCalcForLinear( MENCV *MencV, MENC_INIT_PRM *MencInitPrm )
{
	/*----------------------------------------------------------------------------------------------*/
	/*		通信異常検出レベル設定																	*/
	/*----------------------------------------------------------------------------------------------*/
	MencV->ComErrLimit = MencInitPrm->spgfail.b.l;

	/*----------------------------------------------------------------------------------------------*/
	/*		スケールピッチの設定																	*/
	/*----------------------------------------------------------------------------------------------*/
	if(MencV->ScalePitchExp == 0xFF)
	{ /* シリアルコンバータのスケールピッチ情報無効 */
		/*------------------------------------------------------------------------------------------*/
		/*	Pn282よりスケールピッチ設定																*/
		/*------------------------------------------------------------------------------------------*/
		MencV->MonScalePitch = MencInitPrm->scalepitch2l;
		MencV->MonScaleExp = SCALE_EXP_PM; /* [pm]単位の指数			*/
		MencV->NormScalePitch = MencInitPrm->scalepitch2l * 10;
		MencV->NormScaleExp = SCALE_EXP_UM; /* [nm]単位の指数			*/
	}
	else
	{ /* シリアルコンバータのスケールピッチ情報有効 */
		/* シリアルコンバータの情報よりスケールピッチ設定 */
		if(FALSE == SencScalePitchCalc(MencV))
		{
			/* 計算結果がＮＧの場合クリア */
			MencV->MonScalePitch = 0;
			MencV->MonScaleExp = 0;
			MencV->NormScalePitch = 0;
			MencV->NormScaleExp = 0;
		}
	}

	/*----------------------------------------------------------------------------------------------*/
	/*		エンコーダパルス数の設定																*/
	/*----------------------------------------------------------------------------------------------*/
	/*																								*/
	/*					(2*PolePitch) * (100000) * (2^BitNo)		PulseNo    : [pulse/360deg]		*/
	/*		PulseNo = -----------------------------------------		PolePitch  : [0.1mm/180deg]		*/
	/*								ScalePitch						ScalePitch : [nm]				*/
	/*																								*/
	/*		注意：PulseNoに余りがある場合、通常の演算では、磁極位置の演算誤差が累積する。			*/
	/*			  1) PulseNo = 460800 [pulse/360deg], PolePitch=18.0mm, ScalePitch=20.00um			*/
	/*			  2) PoleErr = 360 / 460800  [deg/(2*PolePitch)]									*/
	/*			  3) PoleErr = (360/460800) * 1000/(2*18.0) = 0.02 [deg/m]							*/
	/*			  4) 累積誤差が小さい(0.02deg/m)ので、当面は無視する。								*/
	/*																								*/
	/*----------------------------------------------------------------------------------------------*/
	if(MencV->NormScalePitch != 0)
	{
		MencV->PulseNo = MlibDiv6432( MencInitPrm->MencP->ppitch * MencV->NormScaleExp,
									  (200000 << MencInitPrm->MencP->bit_dp.b.l),
									  MencV->NormScalePitch );
		/*------------------------------------------------------------------------------------------*/
#if 0
		if( 0 ) /* PulseNo余りチェック：累積誤差が小さいので、当面は無視する(SGDV仕様)。	*/
		{
			MlibMulu32u32( MencV->PulseNo, MencV->NormScalePitch, xx );
			MlibMulu32u32((prm_ptr->ppitch*MencV->NormScaleExp),(200000 << prm_ptr->bit_dp.b.l), yy );
			if( (xx[0] != yy[0]) || (xx[1] != yy[1]) ) /* 余りチェック							*/
			{
				KpiSetParamError( &pndef_ppitch ); /* A.040 : パラメータエラーの設定		*/
			}
		}
#endif
	}
	/*----------------------------------------------------------------------------------------------*/
	else
	{
		return SENCERR_SCALE;								/* A.080 : スケールピッチ設定異常		*/
	}
	/*----------------------------------------------------------------------------------------------*/
	MencV->HalfPulseNo = (MencV->PulseNo >> 1);				/* HalfPulseNo = PulseNo / 2			*/
//	MencV->PlsTrn13bit = FALSE;								/* パルス変換13bit指定Flag				*/
	MencV->DivInfo.PlsTrn13bit = FALSE;						/* パルス変換13bit指定Flag				*//*<S02D>*/

	/*----------------------------------------------------------------------------------------------*/
	/*		原点チェック用パルス数の設定															*/
	/*----------------------------------------------------------------------------------------------*/
	MencV->PulseNo30deg = MencV->PulseNo / 12;				/* PulseNo30deg [Pulse/30deg]			*/

	/*----------------------------------------------------------------------------------------------*/
	/*		Pn080.0 : ポールセンサ有無設定															*/
	/*----------------------------------------------------------------------------------------------*/
	MencV->withPoleSensor = (0 == (MencInitPrm->b_prm80 & 0x0F)) ? TRUE : FALSE;
	/*----------------------------------------------------------------------------------------------*/
	/*		Pn080.1 : モータ相順設定																*/
	/*----------------------------------------------------------------------------------------------*/
	MencV->PhaseOrderUWV = (0 == ((MencInitPrm->b_prm80 >> 4) & 0x0F)) ? FALSE : TRUE;
	/*----------------------------------------------------------------------------------------------*/
	/*		Pn080.2 : Ｕ相ラッチ監視選択設定														*/
	/*----------------------------------------------------------------------------------------------*/
	MencV->MotOrgChkAlways = (0 == ((MencInitPrm->b_prm80 >> 8) & 0x0F)) ? TRUE : FALSE;
	/*----------------------------------------------------------------------------------------------*/
	/*		Pn080.3 : 設定可能な最大速度・分周比計算選択設定										*/
	/*----------------------------------------------------------------------------------------------*/
//	MencV->PhaseOrderUWV = (0 == ((b_prm80 >> 12) & 0x0F)) ? FALSE : TRUE;

	return SENCERR_SUCCESS;
}

/****************************************************************************************************/
/*																									*/
/*		シリアルエンコーダの変数初期化処理															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : イニシャル処理において、シリアルエンコーダの初回位置データを読み込み、					*/
/*			位置データ、前回位置の設定を行う。														*/
/*			また、エンコーダタイプによってＣ相出力、初期パルスを出さない設定をする。				*/
/*																									*/
/****************************************************************************************************/
SENC_ERROR	SencCalcBaseParameters(MENCV *MencV, MENC_INIT_PRM *MencInitPrm )
{
	SENC_ERROR	err;

	/*----------------------------------------------------------------------------------------------*/
	/*		分周パルス出力選択Flagの設定															*/
	/*----------------------------------------------------------------------------------------------*/
	/* 2011.01.11 Y.Oka 分周パルス出力未対応 */
	//	MencV->DivOut = TRUE;
#if 0 /*<S02D>*/
    	MencV->DivOut = FALSE;
	/*----------------------------------------------------------------------------------------------*/
	/*		サンプル版ハード対応																	*/
	/*----------------------------------------------------------------------------------------------*/
	if( MencV->DivOutEnable == TRUE )
	{
		MencV->DivOut = TRUE;
	}
#endif /*<S02D>*/
	/*----------------------------------------------------------------------------------------------*/
	/*		エンコーダ未接続時の処理																*/
	/*----------------------------------------------------------------------------------------------*/
	if( MencV->EncConnect == FALSE )
	{
		MencV->AbsoEncType = MENC_INCRE_TYPE;
		MencV->AxisMotType = MOTTYPE_ROTARY;
	}

	/*----------------------------------------------------------------------------------------------*/
	/*		シリアルエンコーダ基本パラメータ計算													*/
	/*----------------------------------------------------------------------------------------------*/
	if( MencV->AxisMotType == MOTTYPE_LINEAR )
	{ /* リニア型 */
		err = SencBaseParamCalcForLinear( MencV, MencInitPrm );
	}
	else
	{ /* 回転型 */
		err = SencBaseParamCalcForRotary( MencV, MencInitPrm );
	}

	return err;
}


/****************************************************************************************************/
/*																									*/
/*		モータ電圧コード変換																		*/
/*																									*/
/****************************************************************************************************/
static UCHAR	SencGetMotorVoltCode( USHORT volt )
{
	switch( volt )
	{
	case 100 : return( 0x00 );								/* AC 100V								*/
	case 200 : return( 0x01 );								/* AC 200V								*/
	case 400 : return( 0x02 );								/* AC 400V								*/
	case  24 : return( 0x03 );								/* DC  24V								*/
	case  48 : return( 0x04 );								/* DC  48V								*/
	default  : return( 0x00 );								/* Default : AC 100V					*/
	}
}


#pragma arm section code = "CALL_SCANA" /* <S1D2> */
/****************************************************************************************************/
/*																									*/
/*		マルチターンデータ取得処理																	*/
/*																									*/
/****************************************************************************************************/
static	USHORT	SencGetMturnData(MENCV *MencV, USHORT limmlt, DBYTEX bit_dp, USHORT PosH, ULONG PosL )
{
	USHORT	Mturn;

/*--------------------------------------------------------------------------------------------------*/
/*		多回転アブソエンコーダ																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->AbsoEncType == MENC_MABSO_TYPE )
	{
		Mturn = (USHORT)(((PosH<<16) | ((ULONG)PosL>>16)) >> (bit_dp.b.h - 64));
		if( MencV->RevAsmMotor )
		{
			if( limmlt == 65535 )
			{
				Mturn = -Mturn;
			}
			else if( Mturn != 0 )
			{
				Mturn = limmlt - Mturn;
			}
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		１回転アブソエンコーダ																		*/
/*--------------------------------------------------------------------------------------------------*/
	else
	{
		Mturn = 0;
	}
/*--------------------------------------------------------------------------------------------------*/
	return( Mturn );

}
#pragma arm section code


/****************************************************************************************************/
/*																									*/
/*		アブソエンコーダタイプの設定																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : エンコーダ形式が、s-abs(***A*-*****)のとき呼び出され 、Pn002.2より						*/
/*			アブソコーダタイプ(MencP.AbsoEncType)を設定する。										*/
/*																									*/
/*			Pn002.2 = 0 : エンコーダの仕様通りに使用												*/
/*			Pn002.2 = 1 : アブソをインクレとして使用												*/
/*			Pn002.2 = 2 : 一回転アブソエンコーダとして使用				 <S16A> 					*/
/****************************************************************************************************/
static void	SencSetAbsoEncType( MENCV *MencV, USHORT b_prm2, USHORT typm_e, USHORT *limmlt )
{
	if( 1 == ((b_prm2>>8) & 0x0F) )
	{	/* アブソをインクレとして使用	*/
		MencV->AbsoEncType = MENC_INCRE_TYPE;
	}
	else if(( 2 == ((b_prm2>>8) & 0x0F) ) && (MencV->SerialConvType == SCONVTYPE_NONE))
	{	/* シリアル一回転アブソエンコーダとして使用	*/
		*limmlt = 0;
		MencV->AbsoEncType = MENC_SABSO_TYPE;
	}
	else
	{	/* エンコーダの仕様通りに使用 */
		if(typm_e & 0x1000)
		{
			MencV->AbsoEncType = MENC_MABSO_TYPE;
		}
		else if(typm_e & 0x2000)
		{
			MencV->AbsoEncType = MENC_SABSO_TYPE;
		}
		else
		{
			MencV->AbsoEncType = MENC_INCRE_TYPE;
		}
	}
}

/****************************************************************************************************/
/*																									*/
/*		パラメータ指数部演算																		*/
/*																									*/
/****************************************************************************************************/
static BOOL	SencExpConvert( UCHAR prm, UCHAR *buf )
{
	BOOL	rc;
	SHORT	swrk;

	rc = TRUE;

/*--------------------------------------------------------------------------------------------------*/
/*		指数部の解析																				*/
/*--------------------------------------------------------------------------------------------------*/
	if ( prm == 0xFF )										/* 指数部：ゼロ(10^0)					*/
	{
		swrk = 0x0000;
	}
	else if ( prm & 0x80 )									/* 指数部：負(10^-n)					*/
	{
		swrk = -(prm & 0x7F);
	}
	else													/* 指数部：正(10^+n)					*/
	{
		swrk = (SHORT)prm;
	}
/*--------------------------------------------------------------------------------------------------*/
/*		指数部のチェック																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( (swrk > 8) || (swrk < -8) )							/* 指数部範囲チェック					*/
	{
		rc = FALSE;
	}
/*--------------------------------------------------------------------------------------------------*/
	*buf = (UCHAR)swrk;
	return( rc );
}

/****************************************************************************************************/
/*																									*/
/*		エンコーダタイプコード設定																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : エンコーダＩＤ情報より、アブソ／インクレ／コンバータの判断、及びΣ-3					*/
/*			エンコーダの判断を内部設定を行う。														*/
/*																									*/
/****************************************************************************************************/
BOOL	SencSetEncoderType( MENCV *MencV, MENCPRM *MencP, USHORT b_prm2, BOOL SencFbOptUse ) /* <S16A> */
{
	UCHAR 	*Model;
	BOOL	PrmError;

	Model = &(MencV->EncInfo.Model[0]);
	PrmError = FALSE;

	/*----------------------------------------------------------------------------------------------*/
	/*		アブソ／インクレ／コンバータの判断														*/
	/*----------------------------------------------------------------------------------------------*/
	MencV->SerialConvType = SCONVTYPE_NONE; /* シリアルコンバータタイプ初期化		*/
	MencV->OrgClrByWrAdrCmd = FALSE; /* WRADR Commandによる原点クリア無効	*/

	if( SencFbOptUse == FALSE )
	{/*	フィードバックオプション不使用時の場合（従来の処理）										*/
	switch(Model[3])
	{
	case 'A': /* シリアル多回転アブソエンコーダ												*/
		MencV->AxisMotType = MOTTYPE_ROTARY;
		MencV->AbsoEncoder = TRUE;
		MencP->typm_e.b.h  |= 0x10; /* <S16A> */
		SencSetAbsoEncType(MencV, b_prm2, MencP->typm_e.w, &(MencP->limmlt));	/* <S179> */
		break;
			/*--------------------------------------------------------------------------------------*/
	case 'B': /* シリアル一回転アブソエンコーダ(アブソ仕様)									*/
		MencV->AxisMotType = MOTTYPE_ROTARY;
		MencV->AbsoEncoder = TRUE;
		MencP->limmlt = 0;
		MencP->typm_e.b.h  |= 0x20; /* <S16A> */
		SencSetAbsoEncType(MencV, b_prm2, MencP->typm_e.w, &(MencP->limmlt));	/* <S16A> */
		break;
			/*--------------------------------------------------------------------------------------*/
	case 'S': /* シリアル一回転アブソエンコーダ(インクレ仕様)									*/
		MencV->AxisMotType = MOTTYPE_ROTARY;
		MencV->AbsoEncoder = TRUE;
		MencP->limmlt = 0;
		if(((b_prm2>>8) & 0x0F)== 2)
		{
			MencP->typm_e.b.h |= 0x20;
		}
		else
		{
			MencP->typm_e.b.h &= 0x0F;
		}
		SencSetAbsoEncType(MencV, b_prm2, MencP->typm_e.w, &(MencP->limmlt));	/* <S16A> */
		break;
			/*--------------------------------------------------------------------------------------*/
	case 'I': /* シリアルインクレエンコーダ													*/
		MencV->AxisMotType = MOTTYPE_ROTARY;
		MencV->AbsoEncoder = FALSE;
		MencV->AbsoEncType = MENC_INCRE_TYPE;
		MencP->typm_e.b.h &= 0x0F;
		break;
			/*--------------------------------------------------------------------------------------*/
	case 'P': /* シリアルコンバータ for リニアモータ											*/
			PrmError = SencSetSensorType( MencV, MencP, b_prm2 );									/* <S04D> 関数化 */
			break;
			/*--------------------------------------------------------------------------------------*/
		default: // デフォルト：パラメータ異常
			MencV->AxisMotType = MOTTYPE_ROTARY;
			/* A.CA0 : エンコーダパラメータ異常 */
			PrmError = TRUE;
			break;
			/*--------------------------------------------------------------------------------------*/
		}
	
		/*------------------------------------------------------------------------------------------*/
		/*		Σ-3エンコーダ判別																	*/
		/*	<S04D>：シリアル変換ユニットまたは社外回転形エンコーダ(形式：JZDP)の場合の処理を追加	*/
		/*------------------------------------------------------------------------------------------*/
		if( Model[3] == 'P')
		{/* シリアル変換ユニットまたは社外回転形エンコーダ(形式：JZDP)の場合						*/
			if( MencV->SenSpecEnable == TRUE )
			{/* Encoder Prm 0x43 が有効の場合は、0x43のbit4,5によってフォーマットを設定				*/
				switch( (MencV->SenSpec >> 4) & 0x03 )
				{
			/*----------------------------------------------------------------------------------*/
				case 0x00 :	/* Σ-Ⅱフォーマット	*/
					MencV->Sigma3Encoder = FALSE;
			break;
			/*----------------------------------------------------------------------------------*/
				case 0x01 :	/* Σ-Ⅲフォーマット	*/
					MencV->Sigma3Encoder = TRUE;
			break;
			/*----------------------------------------------------------------------------------*/
				case 0x02 :	/* Σ-Ⅴフォーマット	*/
					MencV->Sigma3Encoder = TRUE;
			PrmError = TRUE;
			break;
				/*----------------------------------------------------------------------------------*/
				default	  :	/* デフォルト			*/
					MencV->Sigma3Encoder = FALSE;
		PrmError = TRUE;
		break;
	}
			}
			else
			{/* Encoder Prm 0x43 が無効の場合は、Σ－Ⅱフォーマットとする */
				MencV->Sigma3Encoder = FALSE;
			}
		}
		else
		{/* 安川標準回転形エンコーダ（形式：UT**)の場合												*/
	switch(Model[2])
	{
	case 'D':
	case 'S':
		MencV->Sigma3Encoder = FALSE;
		break;
	case 'T':
		MencV->Sigma3Encoder = TRUE;
		break;
	default:
		PrmError = TRUE;
		MencV->Sigma3Encoder = FALSE;
		break;
	}
		}

		if( Model[3] == 'P')
		{/* JZDPの場合 */
			if( MencV->SenSpecEnable == TRUE )
			{
				/* ﾊﾟﾗﾒｰﾀ43Hが有効な場合は43Hのビット3で判断 */
				MencV->PoleWriteEnable = ((MencV->SenSpec & 0x08) == 0x08)? TRUE : FALSE;
			}
			else
			{
				/* ﾊﾟﾗﾒｰﾀ43Hが無効な場合はタイプで判断 */
				if(( MencV->SerialConvType == SCONVTYPE_MITUTOYO_ABS )||( MencV->SerialConvType == SCONVTYPE_ENDAT_ABS ))
		        {
		            MencV->PoleWriteEnable = TRUE;
				}
				else
				{
					MencV->PoleWriteEnable = FALSE;
				}
			}
		}
		else
		{/* 安川標準回転形エンコーダの場合は無効 */
			MencV->PoleWriteEnable = FALSE;
		}
	}
	else/* <S04D> */
	{/*	フィードバックオプション使用時の場合														*/
		/*------------------------------------------------------------------------------------------*/
		/*	エンコーダ位置センサタイプ設定															*/
		/*------------------------------------------------------------------------------------------*/
		PrmError = SencSetSensorType( MencV, MencP, b_prm2 );
		/*------------------------------------------------------------------------------------------*/
		/*	Σ－Ⅱ／Σ－Ⅲエンコーダ判別															*/
		/*------------------------------------------------------------------------------------------*/
		/*	フィードバックオプション使用時の場合、Σ－Ⅱタイプのみのため判別不要。FALSE設定とする。	*/
		/*------------------------------------------------------------------------------------------*/
		MencV->Sigma3Encoder = FALSE;
		/*------------------------------------------------------------------------------------------*/
		/*		電流位相パラメータスケール書き込み可否設定											*/
		/*------------------------------------------------------------------------------------------*/
		/*	MencP.SenSpec(Addr:0x43) が bit7=1,bit6=0 のときのみパラメータ情報が有効				*/
		/*------------------------------------------------------------------------------------------*/
		if( MencV->SenSpecEnable == TRUE )
		{
			MencV->PoleWriteEnable = ((MencV->SenSpec & 0x08) == 0x08)? TRUE : FALSE;
		}
		else
		{/* ﾊﾟﾗﾒｰﾀ43Hが無効な場合はタイプで判断 */
			if(( MencV->SerialConvType == SCONVTYPE_MITUTOYO_ABS )||( MencV->SerialConvType == SCONVTYPE_ENDAT_ABS ))
	        {
	            MencV->PoleWriteEnable = TRUE;
			}
			else
			{
				MencV->PoleWriteEnable = FALSE;
			}
		}
	}
#if 0
/* <S04D>:フィードバックオプションによるセミク等に対応するため、関数化する。				*/
/* SencSetMotorAndEncoderID()関数でフィードバックオプションモジュール情報設定後に実施する。	*/
		/*------------------------------------------------------------------------------------------*/
	/*		モータ定数スケール書込み可否判別														*/
		/*------------------------------------------------------------------------------------------*/
	if(MencV->AxisMotType == MOTTYPE_ROTARY)
	{
		MencV->ScaleWriteEnable = FALSE;
	}
	else if(MencV->AxisMotType == MOTTYPE_LINEAR)
	{
		if(((MencV->TypeCode & 0x0080) == FALSE) ||
			((MencV->EncConnect == TRUE) && (Model[1] == '\0') && (Model[0] == '\0')))
		{
			MencV->ScaleWriteEnable = TRUE;
		}
		else
		{
			MencV->ScaleWriteEnable = FALSE;
		}
	}
	else
	{
		MencV->ScaleWriteEnable = FALSE;
	}
#endif
	return PrmError;
}

/****************************************************************************************************/
/*																									*/
/*		エンコーダ位置センサタイプ設定																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : エンコーダパラメータより、アブソ／インクレ／コンバータなど判断し、内部変数を設定する。	*/
/*																									*/
/****************************************************************************************************//* <S04D> */
static BOOL SencSetSensorType( MENCV *MencV, MENCPRM *MencP, USHORT b_prm2 )
{
	BOOL	PrmError;
	
	MencV->AxisMotType = MOTTYPE_LINEAR;
	PrmError = FALSE;
	
	switch(MencV->TypeCode)
	{
	/*----------------------------------------------------------------------------------*/
	case 'A': /* ミツトヨアブソタイプ													*/
		MencV->SerialConvType = SCONVTYPE_MITUTOYO_ABS;
		MencV->AbsoEncoder = TRUE;
		MencP->typm_e.b.h |= 0x10; /* <S16A> */
		SencSetAbsoEncType(MencV, b_prm2, MencP->typm_e.w, &(MencP->limmlt));	/* <S16A> */
		MencV->AxisMotType = MOTTYPE_LINEAR;
		break;
	/*----------------------------------------------------------------------------------*/
	case 'B': /* 回転形インクレエンコーダ、INITコマンド対応								*/
		MencV->SerialConvType = SCONVTYPE_SONY_INC;
		MencV->AbsoEncoder = FALSE;
		MencP->typm_e.b.h &= 0x0F;
		MencV->AbsoEncType = MENC_INCRE_TYPE;
		MencV->AxisMotType = MOTTYPE_ROTARY;
		break;
	/*----------------------------------------------------------------------------------*/
	case 'C': /* 回転形絶対値エンコーダ、	原点設定可能								*/
		MencV->SerialConvType = SCONVTYPE_MITUTOYO_ABS;
		MencV->AbsoEncoder = TRUE;
		if( (MencV->SenSpecEnable == TRUE)&&((MencV->SenSpec & 0x02) == 0x02) )
		{
			MencP->typm_e.b.h |= 0x20;					/* マルチターンなし(1回転絶対値)*/
			MencP->limmlt = 0;
		}
		else
		{
			MencP->typm_e.b.h |= 0x10;					/* マルチターンあり(多回転絶対値)*/
		}
		SencSetAbsoEncType(MencV, b_prm2, MencP->typm_e.w, &(MencP->limmlt));	/* <S16A> */
		MencV->AxisMotType = MOTTYPE_ROTARY;
		break;
	/*----------------------------------------------------------------------------------*/
	case 'D': /* ＳＭＳインクレタイプ													*/
		MencV->SerialConvType = SCONVTYPE_SONY_INC;
		MencV->AbsoEncoder = FALSE;
		MencP->typm_e.b.h &= 0x0F;
		MencV->AbsoEncType = MENC_INCRE_TYPE;
		MencV->AxisMotType = MOTTYPE_LINEAR;
		break;
	/*----------------------------------------------------------------------------------*/
	case 'E': /* ＥｎＤａｔタイプ														*/
		MencV->SerialConvType = SCONVTYPE_ENDAT_ABS;
		MencV->AbsoEncoder = TRUE;
		MencP->typm_e.b.h |= 0x10;  /* <S16A> */
		SencSetAbsoEncType(MencV, b_prm2, MencP->typm_e.w, &(MencP->limmlt));	/* <S16A> */
		MencV->AxisMotType = MOTTYPE_LINEAR;
		break;
	/*----------------------------------------------------------------------------------*/
	case 'G': /* 回転形絶対値エンコーダ、原点設定不可									*/
		MencV->SerialConvType = SCONVTYPE_ENDAT_ABS;
		MencV->AbsoEncoder = TRUE;
		if( (MencV->SenSpecEnable == TRUE)&&((MencV->SenSpec & 0x02) == 0x02) )
		{
			MencP->typm_e.b.h |= 0x20;					/* マルチターンなし(1回転絶対値)*/
			MencP->limmlt = 0;
		}
		else
		{
			MencP->typm_e.b.h |= 0x10;					/* マルチターンあり(多回転絶対値)*/
		}
		SencSetAbsoEncType(MencV, b_prm2, MencP->typm_e.w, &(MencP->limmlt));	/* <S16A> */
		MencV->AxisMotType = MOTTYPE_ROTARY;
		break;
/*--------------------------------------------------------------------------------------*/
	case 'J': /* 回転インクレエンコーダ、WRITE_ADRコマンド対応							*/
		MencV->SerialConvType = SCONVTYPE_STANDARD_INC;
		MencV->AbsoEncoder = FALSE;
		MencP->typm_e.b.h &= 0x0F;
		MencV->AbsoEncType = MENC_INCRE_TYPE;
		MencV->OrgClrByWrAdrCmd = TRUE;		/* WRITE_ADRコマンドによる原点クリア有効*/
		MencV->AxisMotType = MOTTYPE_ROTARY;
		break;
	/*----------------------------------------------------------------------------------*/
	case 'L': /* 標準インクレタイプ														*/
		MencV->SerialConvType = SCONVTYPE_STANDARD_INC;
		MencV->AbsoEncoder = FALSE;
		MencV->AbsoEncType = MENC_INCRE_TYPE;
		MencP->typm_e.b.h &= 0x0F;
		MencV->OrgClrByWrAdrCmd = TRUE; 		/* WRADR Commandによる原点クリア有効	*/
		MencV->AxisMotType = MOTTYPE_LINEAR;
		break;
	/*----------------------------------------------------------------------------------*/
	default:
		/* A.CA0 : エンコーダパラメータ異常 */
		PrmError = TRUE;
		break;
	}
	return PrmError;
}

/****************************************************************************************************/
/*																									*/
/*		モータタイプコード設定																		*/
/*																									*/
/****************************************************************************************************/
static void	SencSetMotorType( MENCV *MencV, MENCPRM *MencP )
 {
	UCHAR *Model;
	UCHAR MotType; /* <S03A> */
	UCHAR Seriese; /* <S03A> */

	Model = &(MencV->MotInfo.Model[0]);
	MencV->TuningLessLowGainMotor = FALSE; /* 調整レス機種判別しないに初期化		*/
	MencV->WeakenFieldEnable = FALSE; /* 弱め界磁を強制有効判別しないに初期化	*/

	/*----------------------------------------------------------------------------------------------*/
	/*		回転形モータシリーズ判別												<S03A>			*/
	/*----------------------------------------------------------------------------------------------*/
	if( Model[3] == '7' )
	{ /* Σ-7モータ */
		Seriese = 3;
		MotType = 4;
	}
	else
	{
		Seriese = 4;
		MotType = 3;
	}

	/*----------------------------------------------------------------------------------------------*/
	/*		特殊モータ																				*/
	/*----------------------------------------------------------------------------------------------*/
	if((Model[0] != 'S') || (Model[2] != 'M'))
	{
		MencP->typm_e.b.l = 0xFF;
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		標準モータ																				*/
	/*----------------------------------------------------------------------------------------------*/
	else
	{
//		switch(Model[3]) /* <S03A> */
		switch(Model[MotType])
		{
		/*------------------------------------------------------------------------------------------*/
		case 'A':
//			switch(Model[4]) /* <S03A> */
			switch(Model[Seriese])
			{
			case 'J':
				MencP->typm_e.b.l = 0x0D;
				break;
			default:
				MencP->typm_e.b.l = 0x00;
				break;
			}
			break;
			/*--------------------------------------------------------------------------------------*/
		case 'P':
			MencP->typm_e.b.l = 0x01;
			break;
			/*--------------------------------------------------------------------------------------*/
		case 'S':
			MencP->typm_e.b.l = 0x02;
			/* SGMS*モータは弱め界磁を強制有効とする */
			MencV->WeakenFieldEnable = TRUE;
			break;
			/*--------------------------------------------------------------------------------------*/
		case 'G':
			switch(Model[10])
			{
			case 'A':
			case 'C':
				MencP->typm_e.b.l = 0x03;
				/* SGMG*モータは調整レス低騒音設定とする */
				MencV->TuningLessLowGainMotor = TRUE;
				break;
			case 'B':
			case 'D':
				MencP->typm_e.b.l = 0x04;
				/* SGMG*モータは調整レス低騒音設定とする */
				MencV->TuningLessLowGainMotor = TRUE;
				break;
			default:
				break;
			}
			break;
			/*--------------------------------------------------------------------------------------*/
		case 'D':
			MencP->typm_e.b.l = 0x05;
			/* SGMD*モータは調整レス低騒音設定とする */
			MencV->TuningLessLowGainMotor = TRUE;
			break;
			/*--------------------------------------------------------------------------------------*/
		case 'U':
			MencP->typm_e.b.l = 0x06;
			/* SGMU*モータは調整レス低騒音設定とする */
			MencV->TuningLessLowGainMotor = TRUE;
			/* SGMU*モータは弱め界磁を強制有効とする */
			MencV->WeakenFieldEnable = TRUE;
			break;
			/*--------------------------------------------------------------------------------------*/
		case 'M':
//			switch(Model[4]) /* <S03A> */
			switch(Model[Seriese])
			{
			case 'J':
				MencP->typm_e.b.l = 0x11;
				break;
			default:
				MencP->typm_e.b.l = 0x07;
				break;
			}
			break;
			/*--------------------------------------------------------------------------------------*/
		case 'B':
			switch(Model[10])
			{
			case 'A':
				MencP->typm_e.b.l = 0x08;
				break;
			case 'B':
				MencP->typm_e.b.l = 0x09;
				break;
			case 'C':
				MencP->typm_e.b.l = 0x0B;
				break;
			case 'D':
				MencP->typm_e.b.l = 0x0C;
				break;
			default:
				break;
			}
			break;
			/*--------------------------------------------------------------------------------------*/
		case 'T':
			MencP->typm_e.b.l = 0x0A;
			break;
			/*--------------------------------------------------------------------------------------*/
		case 'J':
			MencP->typm_e.b.l = 0x0D;
			break;
			/*--------------------------------------------------------------------------------------*/
		case 'K':
			MencP->typm_e.b.l = 0x11;
			break;
			/*--------------------------------------------------------------------------------------*/
		case 'C':
			switch(Model[8])
			{
			case 'C':
				MencP->typm_e.b.l = 0x12;
				break;
			case 'D':
				MencP->typm_e.b.l = 0x13;
				break;
			case 'B':
				MencP->typm_e.b.l = 0x14;
				break;
			case 'E':
				MencP->typm_e.b.l = 0x15;
				break;
			case 'L':
				MencP->typm_e.b.l = 0x16;
				break;
			case 'M':
				MencP->typm_e.b.l = 0x17;
				break;
			case 'N':
				MencP->typm_e.b.l = 0x18;
				break;
			case 'R':
				MencP->typm_e.b.l = 0x19;
				break;
			default:
				MencP->typm_e.b.l = 0x1F;
				break;
			}
			break;
			/*--------------------------------------------------------------------------------------*/
		case 'E':
			MencP->typm_e.b.l = 0x0E;
			break;
			/*--------------------------------------------------------------------------------------*/
		case 'Z':
			MencP->typm_e.b.l = 0x0F;
			break;
			/*--------------------------------------------------------------------------------------*/
		default:
			MencP->typm_e.b.l = 0xFF;
			break;
		}
		/*------------------------------------------------------------------------------------------*/
//		switch(Model[4]) /* <S03A> */
		switch(Model[Seriese])
		{
		case 'S':
			MencP->typm_e.b.l |= 0x20;
			/* インクレエンコーダ温度検出無効 */
			MencV->IncDetOhFlg = FALSE; /* <S03A> */
			break;
		case 'V':
			MencP->typm_e.b.l |= 0x60;
			/* Vモータは調整レス機種判別しない */
			MencV->TuningLessLowGainMotor = FALSE;
			/* Vモータは弱め界磁を強制有効判別しない */
			MencV->WeakenFieldEnable = FALSE;
			/* インクレエンコーダ温度検出無効 */
			MencV->IncDetOhFlg = FALSE; /* <S03A> */
			break;
		case '7': /* <S03A> */
			MencP->typm_e.b.l |= 0xA0;
			/* 7モータは調整レス機種判別しない */
			MencV->TuningLessLowGainMotor = FALSE;
			/* 7モータは弱め界磁を強制有効判別しない */
			MencV->WeakenFieldEnable = FALSE;
			/* 7モータはインクレエンコーダ温度検出有効 */
			MencV->IncDetOhFlg = TRUE;
			break;
		default:
			break;
		}
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		リニアモータ																			*/
	/*----------------------------------------------------------------------------------------------*/
	if(MencV->AxisMotType == MOTTYPE_LINEAR)
	{
		if(MencV->TypeCode == 'E')
		{
			MencP->typm_e.b.l = 0x41;
		}
		else
		{
			MencP->typm_e.b.l = 0x40;
		}
		/* リニアモータは調整レス機種判別しないに初期化 */
		MencV->TuningLessLowGainMotor = FALSE;
		/* リニアモータは弱め界磁を強制有効判別しない */
		MencV->WeakenFieldEnable = FALSE;
		/*------------------------------------------------------------------------------------------*/
		/*	コアレスリニアモータ形式判別	<V108>													*/
		/*------------------------------------------------------------------------------------------*/
		switch(Model[3])
		{
		case 'G':
			MencV->CoreLessLinearMotor = TRUE; /* コアレスリニア				*/
			break;
		default:
			MencV->CoreLessLinearMotor = FALSE;
			break;
		}
		/*------------------------------------------------------------------------------------------*/
		/*	Σ-Trac形式判別		<V335>																*/
		/*------------------------------------------------------------------------------------------*/
		switch(Model[2])
		{
		case 'T':
			switch(Model[3])
			{
			case 'M':
			case 'F':
				MencV->SigmaTracMotor = TRUE; /* Σ-Trac μ, MAG				*/
				break;
			default:
				MencV->SigmaTracMotor = FALSE;
				break;
			}
			break;
		default:
			MencV->SigmaTracMotor = FALSE;
			break;
		}
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		低速モータフラグ設定																	*/
	/*----------------------------------------------------------------------------------------------*/
	if(MencV->AxisMotType == MOTTYPE_ROTARY)
	{
//		kx = MlibExpCal(MencP->exp_motw_spd.b.h);
//		kx = MlibScalKskxkx(kx, (100 * MencP->vrat_max.b.h), 1, NULL, -30);
		MencV->LowSpdMotor = (Model[3] == 'C') ? TRUE : FALSE;
	}
	else
	{
		MencV->LowSpdMotor = FALSE;
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		反転接続フラグ設定																		*/
	/*----------------------------------------------------------------------------------------------*/
	if(Model[20] == 'R')
	{
		MencV->RevAsmMotor = TRUE;
	}
	else
	{
		MencV->RevAsmMotor = FALSE;
	}

	/*----------------------------------------------------------------------------------------------*/
	/*	リニア時：反転接続フラグ無効設定															*/
	/*----------------------------------------------------------------------------------------------*/
	if(MencV->AxisMotType == MOTTYPE_LINEAR)
	{
		MencV->RevAsmMotor = FALSE;
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		弱め界磁制御強制有効設定																*/
	/*----------------------------------------------------------------------------------------------*/
	if(MencV->WeakenFieldEnable == TRUE)
	{ /* 弱め界磁有効フラグON */
		MencP->flg_wf |= 0x0001; /* 電圧FB方式弱め界磁制御有効			*/
	}
}


/****************************************************************************************************/
/*																									*/
/*		シリアルエンコーダ : エンコーダ＆モータID取得・設定処理										*/
/*																									*/
/****************************************************************************************************/
static BOOL	SencSetMotorAndEncoderID( MENCV *MencV, MENCPRM *MencP, OPFBK_HNDL *FbkOpt,			/* <S04D>:FbkOpt追加 */
										 USHORT b_prm2, UCHAR *EncPrmBuf, BOOL SencFbOptUse )	/* <S04D>:SencFbOptUse追加 */
{
	BOOL err;

	/*----------------------------------------------------------------------------------------------*/
	/*		リニアタイプコード設定																	*/
	/*----------------------------------------------------------------------------------------------*/
	MencV->TypeCode = EncPrmBuf[0x40];

	/*----------------------------------------------------------------------------------------------*/
	/*		エンコーダタイプ設定																	*/
	/* <S04D> mod( EncInfo -> pActEncInfo )														*/
	/*----------------------------------------------------------------------------------------------*/
	SencSetIDINFO( 0x00, &EncPrmBuf[0x00], MencV->pActEncInfo );
	SencSetIDINFO( 0x08, &EncPrmBuf[0x08], MencV->pActEncInfo );
	SencSetIDINFO( 0x10, &EncPrmBuf[0x10], MencV->pActEncInfo );
	SencSetIDINFO( 0x18, &EncPrmBuf[0x18], MencV->pActEncInfo );

	SencFillNullIDINFO( MencV->pActEncInfo );
	err = SencSetEncoderType( MencV, MencP, b_prm2, SencFbOptUse );/* <S04D>:SencFbOptUse追加 */

	/*----------------------------------------------------------------------------------------------*/
	/*		モータタイプ設定																		*/
	/* <S04D> mod( MotInfo -> pActMotInfo )														*/
	/*----------------------------------------------------------------------------------------------*/
	SencSetIDINFO( 0x20, &EncPrmBuf[0x20], MencV->pActMotInfo );
	SencSetIDINFO( 0x28, &EncPrmBuf[0x28], MencV->pActMotInfo );
	SencSetIDINFO( 0x30, &EncPrmBuf[0x30], MencV->pActMotInfo );
	SencSetIDINFO( 0x38, &EncPrmBuf[0x38], MencV->pActMotInfo );
	SencSetIDINFO( 0x98, &EncPrmBuf[0x98], MencV->pActMotInfo );

	SencSetMotorType( MencV, MencP);
	SencFillNullIDINFO( MencV->pActMotInfo );
	
	/*--------------------------------------------------------------------------------------------------*/
	/*		フィードバックオプションモジュール情報設定													*/
	/*--------------------------------------------------------------------------------------------------*/
	/*		現状のシリアル変換型フィードバックオプションモジュールのID情報はエンコーダID情報領域に		*/
	/*		入っているため、エンコーダID情報から作成する。										<S04D>	*/
	/*--------------------------------------------------------------------------------------------------*/
	if( SencFbOptUse == TRUE )
	{
		FbOptSetIDInfo(FbkOpt, MencV, SencFbOptUse );
	}
	

	return err;
}

/****************************************************************************************************/
/*																									*/
/*		シリアルエンコーダ : リニアモータ非対応判定処理												*/
/*																									*/
/****************************************************************************************************//*<S1BE>*/
static SENC_ERROR SencCheckLinearMotor( MENCV *MencV, USHORT syssw1 )
{
	SENC_ERROR	err;
	err = SENCERR_SUCCESS;
	/*----------------------------------------------------------------------------------------------*/
	/*  リニアモータ非対応判定処理																	*/
	/*		PnE0B.1 =	0	: 回転型、リニア対応													*/
	/*				=	1	: 回転型対応、リニア非対応(A.051発生)									*/
	/*----------------------------------------------------------------------------------------------*/
	if(syssw1 & 0x0002)
	{
		if(MencV->AxisMotType == MOTTYPE_LINEAR)
		{/* A.051 : 製品未サポートアラーム	*/
			err = SENCERR_NOTSUPPORT;
		}
	}
	return err;
}

/****************************************************************************************************/
/*																									*/
/*		シリアルエンコーダ : 全パラメータ初期設定処理												*/
/*																									*/
/****************************************************************************************************/
static SENC_ERROR	SencInitEncParameters( MENCV *MencV, MENCPRM *MencP, UCHAR  *EncPrmBuf)
{
	USHORT		addr;
	UCHAR		*PrmBuf;
	SENC_ERROR	err;
	LONG 		kx;

	err = SENCERR_SUCCESS;
	/*----------------------------------------------------------------------------------------------*/
	/* エンコーダパラメータ設定処理 */
	/*----------------------------------------------------------------------------------------------*/
	/* Bank0から値を取得した値を設定する。															*/
	for( addr = 0x0048; addr < 0x00A0; addr += 8 )
	{
		PrmBuf = &EncPrmBuf[addr];
		switch(addr)
		{
		case 0x48: /* エンコーダビット数、小数点位置、マルチターンリミット					*/
			MencV->DivInfo.IncreBit_dp = (UCHAR)PrmBuf[0]; /* 対応機種：21-24ビットインクレ *//*<S161>*/
			MencP->bit_dp.b.l = ( PrmBuf[2] == (UCHAR)0xFF )? 0 : PrmBuf[2];
			MencP->bit_dp.b.h = PrmBuf[3];
			MencP->limmlt = ((USHORT) PrmBuf[7] << 8) | (USHORT) PrmBuf[6];

			/* スケールピッチ																*/
			if(MencV->AxisMotType == MOTTYPE_LINEAR)
			{
				MencV->ScalePitch[0] = PrmBuf[4]; /* スケールピッチ下位				*/
				MencV->ScalePitch[1] = PrmBuf[5]; /* スケールピッチ中位				*/
				MencV->ScalePitch[2] = PrmBuf[6]; /* スケールピッチ上位				*/
				MencV->ScalePitchExp = PrmBuf[7]; /* スケールピッチ指数				*/
			}

			/* エンコーダ位置補正用サンプリング周期設定 */
			MencV->EncLchNum = 0;
			if(MencV->AxisMotType == MOTTYPE_ROTARY)
			{
				if(MencP->bit_dp.b.l > 13)
				{
					if(MencP->typm_e.b.l == 0x11)
					{/* Sigma3 motor : SGMMJ */
						MencV->EncLchNum = 28; /* 16bit,17bit : 28/6M =4.67us(for SGMMJ)	*/
					}
					else if(MencP->typm_e.b.l < 0x60)
					{/* Sigma2, Sigma3 motor */
						MencV->EncLchNum = 16; /* 16bit,17bit : 16/6M =2.67us				*/
					}
					else
					{/* Sigma5 motor or later */
						;
					}
				}
			}

			break;
			/*--------------------------------------------------------------------------------------*/
		case 0x50: /* 位相角補正値															*/
			if(MencV->AxisMotType == MOTTYPE_LINEAR)
			{
				MencP->phscmpM2 = ((USHORT) PrmBuf[1] << 8) | (USHORT) PrmBuf[0];
			}
			else
			{
				MencP->phscmpM2 = 0;
			}
			break;
			/*--------------------------------------------------------------------------------------*/
		case 0x58: /* 通信速度設定、パラメータ拡張領域選択、エンコーダソフトウェアバージョン	*/
			MencP->exmotprm = (USHORT) ~PrmBuf[4];				/* 0で有効のためビット反転		*/
			MencP->chng_baudrate = (USHORT) ~PrmBuf[5];			/* 0で有効のためビット反転		*/
			MencP->encver = (USHORT) PrmBuf[7];
			break;
			/*--------------------------------------------------------------------------------------*/
		case 0x60: /* 定格出力、モータポール数、定格トルク									*/
			MencP->motw = ((USHORT) PrmBuf[1] << 8) | (USHORT) PrmBuf[0];
			MencP->oslv_pol.b.h = PrmBuf[2];
			MencP->rattrq = ((USHORT) PrmBuf[5] << 8) | (USHORT) PrmBuf[4];
			if(TRUE != SencExpConvert(PrmBuf[6], &(MencP->exp_trat_jmot.b.l)))
			{
				err = SENCERR_PRMDATA;
			}
			break;
			/*--------------------------------------------------------------------------------------*/
		case 0x68: /* 最大トルク、定格電流、最大電流、ＥＭＦ補償							*/
			MencP->maxtrq = (((USHORT) PrmBuf[2] << 8) | (USHORT) PrmBuf[1]);
			MencP->irat = (((USHORT) PrmBuf[4] << 8) | (USHORT) PrmBuf[3]);
			MencP->imax = (((USHORT) PrmBuf[6] << 8) | (USHORT) PrmBuf[5]);
			MencP->emfcmp = (MencP->emfcmp & 0xFF00U) | (USHORT) PrmBuf[7];
			break;
			/*--------------------------------------------------------------------------------------*/
		case 0x70: /* ＥＭＦ補償,定格速度,最大速度,ＯＳレベル,イナーシャ,巻線抵抗			*/
			MencP->emfcmp = (MencP->emfcmp & 0x00FFU) | ((USHORT) PrmBuf[0] << 8);
			MencP->vrat_max.b.l = PrmBuf[1];
			MencP->vrat_max.b.h = PrmBuf[2];
			MencP->oslv_pol.b.l = PrmBuf[3];
			MencP->jmot = ((USHORT) PrmBuf[5] << 8) | (USHORT) PrmBuf[4];
			if(TRUE != SencExpConvert(PrmBuf[6], &(MencP->exp_trat_jmot.b.h)))
			{
				err = SENCERR_PRMDATA;
			}
			MencP->motr = (MencP->motr & 0xFF00U) | (USHORT) PrmBuf[7];
			break;
			/*--------------------------------------------------------------------------------------*/
		case 0x78: /* 巻線抵抗、インダクタンス、べーストルク、中間トルク					*/
			MencP->motr = (MencP->motr & 0x00FFU) | ((USHORT) PrmBuf[0] << 8);
			MencP->motl = ((USHORT) PrmBuf[2] << 8) | (USHORT) PrmBuf[1];
			MencP->bastrq = (USHORT) PrmBuf[5];
			MencP->midtrq = ((USHORT) PrmBuf[7] << 8) | (USHORT) PrmBuf[6];
			break;
			/*--------------------------------------------------------------------------------------*/
		case 0x80: /* 中間時間、中間時間２、中間トルク２									*/
			MencP->midolt = (USHORT) PrmBuf[0];
			MencP->mtolt2 = (USHORT) PrmBuf[1];
			MencP->mtrq2 = ((USHORT) PrmBuf[3] << 8) | (USHORT) PrmBuf[2];
			MencP->flg_wf = (USHORT) PrmBuf[4];
			break;
			/*--------------------------------------------------------------------------------------*/
		case 0x88: /* 位相補償１、位相補償２(2004.06.29)、モータ電圧、ポールピッチ			*/
			MencP->phscmpM1.b.l = PrmBuf[2];
			MencP->poleoffset = ((USHORT) PrmBuf[3] >= 180) ? 0 : ((USHORT) PrmBuf[3] << 1);
			MencP->typm_e.b.h
					|= SencGetMotorVoltCode(((USHORT) PrmBuf[5] << 8) | (USHORT) PrmBuf[4]);
			MencP->ppitch = ((USHORT) PrmBuf[7] << 8) | (USHORT) PrmBuf[6];
			break;
			/*--------------------------------------------------------------------------------------*/
		case 0x90: /* ｑ軸インダクタンス(Lq0,Lq1)、指数、位相補償開始速度(2004.06.29)		*/
			MencP->motlq0 = ((USHORT) PrmBuf[1] << 8) | (USHORT) PrmBuf[0];
			MencP->motlq1 = ((USHORT) PrmBuf[3] << 8) | (USHORT) PrmBuf[2];

			if(TRUE != SencExpConvert(PrmBuf[4], &(MencP->exp_motw_spd.b.l)))
			{
				err = SENCERR_PRMDATA;
			}

			if(TRUE != SencExpConvert(PrmBuf[5], &(MencP->exp_motw_spd.b.h)))
			{
				err = SENCERR_PRMDATA;
			}

			MencP->phscmpM1.b.h = 0;
			if((ULONG) PrmBuf[6] < 50)
			{
				MencP->motresfrq = (USHORT) (PrmBuf[6] * 100);
			}
			else if((ULONG) PrmBuf[6] > 250)
			{
				MencP->motresfrq = 0;
			}
			else
			{
				MencP->motresfrq = (USHORT) (PrmBuf[6] * 10);
			}
			break;
		default:
			break;
		}
	}

///	if( MencP->exmotprm == TRUE )		/* Bank6有効確認<S16E> */
	if( (SENCBR_EXAREA1 & MencP->exmotprm) == TRUE )		/* Bank6有効確認<S16E> *//* <S1A6> */
	{
		/* Bank6から値を取得した値を設定する。															*//* <S0D7> */
		for( addr = 0x00A0; addr < 0x00E0; addr += 8 )
		{
			PrmBuf = &EncPrmBuf[addr];
			switch(addr - 0x00A0)/* 0x600アドレスから取得した値は、0x00A0以降に格納されているため		*/
			{
			case 0x00:	/* 機能フラグ２、PWM周波数、低速過負荷ベース電流・中間電流１/２・中間時間		*/
				MencP->motfuncf.ul	=  (USHORT) PrmBuf[0];										/* PnF24	*//*モータキャリア周波数切り替え対応<S117>*/
				MencP->mot_pwmf	= ((USHORT) PrmBuf[2] << 8) | (USHORT) PrmBuf[1];			/* PnF25	*/
				MencP->mot_ibs	 = (USHORT) PrmBuf[3];										/* PnF26	*/
				MencP->mot_imd	= ((USHORT) PrmBuf[5] << 8) | (USHORT) PrmBuf[4];			/* PnF27	*/
				MencP->mot_imdt	=  (USHORT) PrmBuf[6];										/* PnF28	*/
				MencP->mot_imd2	=  (USHORT) PrmBuf[7];										/* PnF29	*/
				break;
				/*--------------------------------------------------------------------------------------*/
			case 0x08:	/* 低速過負荷中間時間２、回転速度閾値、モータOC検出電流、調整レスモード選択		*/
						/* 電圧指令制限レベル(電圧指令制限レベル、d軸電流指令リミット用電圧制限レベル)	*/
				MencP->mot_imdt2		= ((USHORT) PrmBuf[1] << 8) | (USHORT) PrmBuf[0];	/* PnF2A	*/
				MencP->mot_ichgspd		=  (USHORT) PrmBuf[2];								/* PnF2B	*/
				MencP->mot_oc			= ((USHORT) PrmBuf[4] << 8) | (USHORT) PrmBuf[3];	/* PnF2C	*/
				MencP->mot_TunLess		=  (USHORT) PrmBuf[5];								/* PnF2D	*/
				MencP->RefAdjust_ForWF.b.l	=  PrmBuf[6];									/* PnF2E.l	*/
				MencP->RefAdjust_ForWF.b.h	=  PrmBuf[7];									/* PnF2E.h	*/
				break;
				/*--------------------------------------------------------------------------------------*/
			case 0x10:	/* 弱め界磁L補正係数、回生時位相進み補償(開始速度、最大回転速度)				*/
				MencP->LCompForWF = (USHORT) PrmBuf[0];										/* PnF2F	*/
				MencP->RegPhscmp.b.l = PrmBuf[1];											/* PnF30.l	*/
				MencP->RegPhscmp.b.h = PrmBuf[2];											/* PnF30.h	*/
				break;
				/*--------------------------------------------------------------------------------------*/
			case 0x18:	/* 予備領域																		*/

				break;
				/*--------------------------------------------------------------------------------------*/
			case 0x20:	/* IPM制御パラメータ領域予定													*/
			case 0x28:
			case 0x30:
			case 0x38:

				break;
				/*--------------------------------------------------------------------------------------*/
			default:
				break;
			}
		}
	}
	else
	{/* bank6無効時は初期値0xFFFFとする */
		MencP->motfuncf.ul	= 0xFFFF;													/* PnF24	*//*モータキャリア周波数切り替え対応<S117>*/
		MencP->mot_pwmf	= 0xFFFF;														/* PnF25	*/
		MencP->mot_ibs	 = 0xFFFF;														/* PnF26	*/
		MencP->mot_imd	= 0xFFFF;														/* PnF27	*/
		MencP->mot_imdt	= 0xFFFF;														/* PnF28	*/
		MencP->mot_imd2	= 0xFFFF;														/* PnF29	*/
		MencP->mot_imdt2		= 0xFFFF;												/* PnF2A	*/
		MencP->mot_ichgspd		= 0xFFFF;												/* PnF2B	*/
		MencP->mot_oc			= 0xFFFF;												/* PnF2C	*/
		MencP->mot_TunLess		= 0xFFFF;												/* PnF2D	*/
		MencP->RefAdjust_ForWF.b.l	= 0xFFFF;											/* PnF2E.l	*/
		MencP->RefAdjust_ForWF.b.h	= 0xFFFF;											/* PnF2E.h	*/
		MencP->LCompForWF = 0xFFFF;														/* PnF2F	*/
		MencP->RegPhscmp.b.l = 0xFFFF;													/* PnF30.l	*/
		MencP->RegPhscmp.b.h = 0xFFFF;													/* PnF30.h	*/
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		低速モータフラグ設定																	*/
	/*----------------------------------------------------------------------------------------------*/
	if(MencV->AxisMotType == MOTTYPE_ROTARY)
	{
		kx = MlibExpCal(MencP->exp_motw_spd.b.h);
		kx = MlibScalKskxkx(kx, (100 * MencP->vrat_max.b.h), 1, NULL, -30);
		MencV->LowSpdMotor |= ((kx <= 500) ? TRUE : FALSE);
	}

	return err;
}

/****************************************************************************************************/
/*																									*/
/*		シリアルエンコーダ : 通信接続処理															*/
/*																									*/
/****************************************************************************************************/
SENC_ERROR	SencInitMotorEncoder( MENCV *MencV, MENC_INIT_PRM *MencInitPrm,	ASIC_HW_IF_STR *AsicHwReg,
										OPFBK_HNDL *FbkOpt, USHORT EncBaudrate, BOOL SencFbOptUse )/* <S04D>:FbkOpt,SencFbOptUse追加 */
{
//	UCHAR 		EncPrmBuf[160];	/* 0xA0	*//* <S0D7> */
	UCHAR 		EncPrmBuf[224];	/* 0xE0	*//* <S0D7> */
	SENC_ERROR	err;
	err = SENCERR_SUCCESS;

	/*----------------------------------------------------------------------------------------------*/
	/*		CONNECT																					*/
	/*----------------------------------------------------------------------------------------------*/
	err |= SencConnect( MencV, AsicHwReg, EncBaudrate, FbkOpt, SencFbOptUse );

	if( SENCERR_SUCCESS == err )	/* Execute in case success encoder connect */
	{
		/*------------------------------------------------------------------------------------------*/
		/*		Read Encoder parameters																*/
		/*------------------------------------------------------------------------------------------*/
		err |= SencReadParamAll( MencV, EncPrmBuf );
		if( SENCERR_SUCCESS != err )
		{	/* Try again... */
			err |= SencReadParamAll( MencV, EncPrmBuf );
		}
		if( SENCERR_SUCCESS == err )	/* Execute in case success read encoder parameter */
		{
			/*--------------------------------------------------------------------------------------*/
			/*		エンコーダ/モータID設定処理														*/
			/*--------------------------------------------------------------------------------------*/
			if( FALSE != SencSetMotorAndEncoderID(MencV, MencInitPrm->MencP, FbkOpt,			/* <S04D>:FbkOpt追加 */
												MencInitPrm->b_prm2, EncPrmBuf, SencFbOptUse) )	/* <S04D>:SencFbOptUse追加 */
			{
				err |= SENCERR_PRMDATA;
			}
			/*--------------------------------------------------------------------------------------*/
			/*		リニアモータ非対応判定処理														*/
			/*--------------------------------------------------------------------------------------*//* <S1BE> */
			if( FALSE != SencCheckLinearMotor(MencV, MencInitPrm->syssw1) )
			{
				err |= SENCERR_NOTSUPPORT;
			}
			/*--------------------------------------------------------------------------------------*/
			/*		原点リセットコマンドの送信														*/
			/*--------------------------------------------------------------------------------------*/
			/* <S04D> 条件文に「フィードバックオプションによるセミク」と「絶対値ｴﾝｺｰﾀﾞ」を追加		*/
			if( (( MencV->AxisMotType == MOTTYPE_LINEAR ) || ( SencFbOptUse == TRUE ))
																&&( MencV->AbsoEncoder == TRUE ) )
			{
				err |= SencSendRstOrgCmd( MencV );
			}
			/*--------------------------------------------------------------------------------------*/
			/*		エンコーダパラメータ初期化処理													*/
			/*--------------------------------------------------------------------------------------*/
			err |= SencInitEncParameters( MencV, MencInitPrm->MencP, EncPrmBuf );

			/*--------------------------------------------------------------------------------------*/
			/*		エンコーダベースパラメータ計算処理												*/
			/*--------------------------------------------------------------------------------------*/
			err |= SencCalcBaseParameters( MencV, MencInitPrm );
		}
	}
	else/* <S04D> */
	{
		/*--------------------------------------------------------------------------------------*/
		/*		エンコーダベースパラメータ計算処理												*/
		/*--------------------------------------------------------------------------------------*/
		err |= SencCalcBaseParameters( MencV, MencInitPrm );
	}

	/* 2013.08.29 Y.Oka P1モード以降は8MBps切り替え後に変更 */
	/*----------------------------------------------------------------------------------------------*/
	/* P1モード設定(上記処理の成功/失敗に限らずP1モード（62.5us通信周期）に変更する->Scan周期を変えないため) */
	/*----------------------------------------------------------------------------------------------*/
//	SencChangeP1Mode( MencV );

	return err;
}



#if 0
/****************************************************************************************************/
/*																									*/
/*		パルスＩＦ初期化																			*/
/*																									*/
/*		注 引数を5つとしたが、SV_OFF中の処理のため処理時間には余裕あり。Y.Oka						*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		ASIC 指令カウンタ設定値テーブル																*/
/*--------------------------------------------------------------------------------------------------*/
const	USHORT	AsicPocsetTable[7] =
{
		0x0650,		/* 0 : ~A,~B, 101=Sign+Pulse		*/
		0x0640,		/* 1 : ~A,~B, 100=CW+CCW			*/
		0x0630,		/* 2 : ~A,~B, 011=(A+B)*1			*/
		0x0610,		/* 3 : ~A,~B, 001=(A+B)*2			*/
		0x0600,		/* 4 : ~A,~B, 000=(A+B)*4			*/
		0x0050,		/* 5 :  A, B, 101=Sign+Pulse		*/
		0x0040,		/* 6 :  A, B, 100=CW+CCW			*/
};
/*--------------------------------------------------------------------------------------------------*/
static void	InitAsicPulseIF( ALMMAN_HANDLE *AlmMngr, UNI_PRM_STR *UniPrms, ASIC_HW_IF_STR *AsicHwReg, MENCV *MencV )
{
	PRMDATA		*prm_ptr;
	LONG		i;
	USHORT		uswk;
	BOOL		err;
	prm_ptr = UniPrms->Prm;

/*--------------------------------------------------------------------------------------------------*/
/*		指令カウンタ設定																			*/
/*--------------------------------------------------------------------------------------------------*/
	i = prm_ptr->prstsw & 0x0F;							/* 指令パルス形態 (Pn200)					*/
	if( i > 6 )											/* パラメータ設定範囲チェック				*/
	{
		uswk = 0;
		/* パラメータ設定エラー */
		ALMSetPramError( AlmMngr, &pndef_prstsw );
	}
	else
	{
		uswk = AsicPocsetTable[i];						/* カウントモード,etc						*/
	}
	/*----------------------------------------------------------------------------------------------*/
	if( ((prm_ptr->prstsw >> 4) & 0x0F) < 2 )
	{
		uswk = ( uswk | 0x0800 );							/* CC極性設定							*/
	}

/*--------------------------------------------------------------------------------------------------*/
/*		CA,CB フィルタ設定																			*/
/*--------------------------------------------------------------------------------------------------*/
	switch( (prm_ptr->prstsw >> 12) & 0x0F )
	{
	case 0x00:											/* ラインドライバ信号用指令入力フィルタ１	*/
		/* (PnE0B.4-5)にて設定 */
		 uswk = (USHORT)(uswk | ((prm_ptr->syssw1 >> 4) & 0x3) );
		 break;
	case 0x01:											/* オープンコレクタ信号用指令入力フィルタ	*/
		/* (PnE0E.0-1)にて設定 */
		 uswk = (USHORT)(uswk | ((prm_ptr->comfil)      & 0x3) );
		 break;
	case 0x02:											/* ラインドライバ信号用指令入力フィルタ２	*/
		/* (PnE0E.8-9)にて設定 */
		 uswk = (USHORT)(uswk | ((prm_ptr->comfil >> 8) & 0x3) );
		 break;
	default:
		/* (PnE0B.4-5)にて設定 */
		 uswk = (USHORT)(uswk | ((prm_ptr->syssw1 >> 4) & 0x3) );
		 break;
	}
/*--------------------------------------------------------------------------------------------------*/
	AsicHwReg->AREG_POCSET	= uswk;						/* 指令カウンタ設定							*/
	uswk = ( uswk | 0x0100 );							/* CLMラッチイネーブル設定					*/
	AsicHwReg->AREG_POCSET	= uswk ;					/* 指令カウンタ設定							*/
	AsicHwReg->AREG_LODSET	= AREG_LODSET_SETV;			/* カウンタロード設定						*/

/*--------------------------------------------------------------------------------------------------*/
/*		指令フィルタ設定																			*/
/*--------------------------------------------------------------------------------------------------*/
	switch( (prm_ptr->prstsw >> 12) & 0x0F )
	{
	case 0x00:											/* ラインドライバ信号用指令入力フィルタ１	*/
		/* (PnE0B.8-11)にて設定 */
		 AsicHwReg->AREG_FCCSET	= (USHORT)((prm_ptr->syssw1 & 0x0F00) );
		 break;
	case 0x01:											/* オープンコレクタ信号用指令入力フィルタ	*/
		/* (PnE0E.4-7)にて設定 */
		 AsicHwReg->AREG_FCCSET	= (USHORT)((prm_ptr->comfil << 4) & 0x0F00);
		 break;
	case 0x02:											/* ラインドライバ信号用指令入力フィルタ２	*/
		/* (PnE0E.12-15)にて設定 */
		 AsicHwReg->AREG_FCCSET	= (USHORT)((prm_ptr->comfil >> 4) & 0x0F00);
		 break;
	default:
		/* (PnE0B.8-11)にて設定 */
		 AsicHwReg->AREG_FCCSET	= (USHORT)(prm_ptr->syssw1 & 0x0F00);
		 break;
	}
/*--------------------------------------------------------------------------------------------------*/
/*		シリアルエンコーダパルス変換設定(Bit数,変換周期) ＠JL-056相当の設定＠						*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->PlsTrn13bit == TRUE )					/* パルス変換13bit							*/
	{
		AsicHwReg->AREG_PLSET	= 0x3100;					/* 変換Bit数:13bit, 変換周期:64us			*/
	}
	else
	{
		AsicHwReg->AREG_PLSET	= 0x3400;					/* 変換Bit数:16bit, 変換周期:64us			*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		シリアルエンコーダパルス変換設定(位置,原点,etc)												*/
/*--------------------------------------------------------------------------------------------------*/
	// パルス変換Ｃ相マスク(念のため)
	AsicHwReg->AREG_WDT1SET	|=	BIT8;
	MencV->DivCMaskStatus = DIV_CMASK_ON;

	AsicHwReg->AREG_DIVSET	= 0x0102;					/* パルス変換書込み側設定:HostCPU			*/
	AsicHwReg->AREG_POSET0	= 0xFFFF;					/* パルス変換位置設定						*/
	AsicHwReg->AREG_POSET0H	= 0x0000;				/* パルス変換位置設定bit16-23				*/
	AsicHwReg->AREG_POTEST	= 0xFFFF;					/* パルス変換カウンタ設定					*/
	AsicHwReg->AREG_POSET1	= 0x0000;					/* パルス変換原点補正１						*/
	AsicHwReg->AREG_POSET2	= 0xFFFF;					/* パルス変換原点補正２						*/

/*--------------------------------------------------------------------------------------------------*/
/*		分周入力フィルタ設定																		*/
/*--------------------------------------------------------------------------------------------------*/
	AsicHwReg->AREG_FBCSET	= 0x0005;					/* 分周入力フィルタ設定						*/

/*--------------------------------------------------------------------------------------------------*/
/*		モータ種別処理：分周パルス計算＆ＡＳＩＣ設定												*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->AxisMotType == MOTTYPE_LINEAR )
	{ /* リニア型 */
		err = LmxSencSetMencDividedPulse( AsicHwReg, MencV, UniPrms );
	}
	else
	{ /* 回転型 */
		err = RmxSencSetMencDividedPulse( AsicHwReg, MencV, UniPrms );
	}

	if(err != FALSE)
	{
		ALMSetServoAlarm(AlmMngr, ALM_DIVPRM );		/* A.041 : 分周パルス出力設定異常		*/
	}
}
#endif

//#if 0 /* MENCV関連の構造体を変更する必要あり？ *//*<S02D>削除*/
/****************************************************************************************************/
/*																									*/
/*		分周パルスＩＦ初期化																		*/
/*																									*/
/****************************************************************************************************/
DIVSET_ERROR SencInitDivPulseIF( ASIC_HW_IF_STR* AsicHwReg, MENCV *MencV, UNI_PRM_STR *UniPrms )
{
	DIVSET_ERROR	err;
	LONG	BitNo;  
	LONG	pgrate;        /*<S079>*/

	BitNo  = UniPrms->Prm->MencP.bit_dp.b.l;								/* PnF03				*//*<S02D>*/

	if( MencV->AxisMotType != MOTTYPE_LINEAR )								/*<S079>*/
	{ /* 回転型 */
		pgrate = (1 << BitNo) / (UniPrms->Prm->pgrat2l << 2);				/* Pn212(Pn213)			*/
		if( pgrate > 65536 )
		{ /* 分周比範囲:1/65536 以外の場合は、ソフト分周とする  */
			MencV->DivInfo.DivOutASIC = FALSE;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		シリアルエンコーダパルス変換設定(Bit数,変換周期) ＠JL-056相当の設定＠						*/
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/*	PLSET:パルス変換設定																			*/
/*--------------------------------------------------------------------------------------------------*/
/*	VULONG	AREG_PLSET;		 0x0168 パルス変換設定													*/
								/*------------------------------------------------------------------*/
								/*  bit.7-0  :パルス変換ヒステリシス設定							*/
								/*------------------------------------------------------------------*/
								/*	bit.11-8 :パルス変換ビット数設定								*/
								/*             0000：12bit											*/
								/*             0001：13bit											*/
								/*                |													*/
								/*             1011：23bit											*/
								/*       1100-1111：24bit											*/
								/*------------------------------------------------------------------*/
								/*  bit.14-12 :パルス変換周期選択									*/
								/*					  周期             動作クロック DDAビット数		*/
								/*				000: 16us              32MHz          9				*/
								/*				001: 32us              32MHz         10				*/
								/*				010: 64us              32MHz         11				*/
								/*				011: 64us              16MHz         10（初期値）	*/
								/*				100: 128us              8MHz         10				*/
								/*				101-111: 1.024ms   1MHz							*/
								/*------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->DivInfo.DivOutASIC != FALSE )
	{
        AsicHwReg->AREG_PLSET	= 0x00003000 + ((BitNo - 12) << 8); /* 変換Bit数:BitNo-12, 変換周期:64us*//*<S079>*/
	}
	else
	{
		if( MencV->DivInfo.PlsTrn13bit == TRUE )			/* パルス変換13bit							*/
		{
			AsicHwReg->AREG_PLSET	= 0x00003100;			/* 変換Bit数:13bit, 変換周期:64us			*/
		}
		else
		{
			AsicHwReg->AREG_PLSET	= 0x00003400;			/* 変換Bit数:16bit, 変換周期:64us			*/
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		シリアルエンコーダパルス変換設定(位置,原点,etc)												*/
/*--------------------------------------------------------------------------------------------------*/
	// パルス変換Ｃ相マスク(念のため)
	AsicHwReg->AREG_WDT1SET	|=	BIT8;
	MencV->DivInfo.DivCMaskStatus = DIV_CMASK_ON;

/*--------------------------------------------------------------------------------------------------*/
/*	DIVSET:分周機能設定																				*/
/*--------------------------------------------------------------------------------------------------*/
/*	VULONG	AREG_DIVSET;		 0x0164 分周機能設定												*/
								/*------------------------------------------------------------------*/
								/*  bit.1-0 :ﾃﾞｰﾀﾊﾟﾙｽ変換位置ﾃﾞｰﾀ書込み選択						*/
								/*            0：ｼﾘｱﾙPGIFﾁｬﾝﾈﾙ０から (初期値)						*/
								/*  		  1：ｼﾘｱﾙPGIFﾁｬﾝﾈﾙ１から								*/
								/*  		  2：ﾎｽﾄCPUから											*/
								/*  		  3：内蔵演算部から										*/
								/*------------------------------------------------------------------*/
								/*  bit.2 : 分周ｸﾛｯｸ選択											*/
								/*            0：26.7MHz (初期値)									*/
								/*  		  1：32MHz												*/
								/*------------------------------------------------------------------*/
								/*  bit.4-3 :パルス変換原点補正値設定元選択						*/
								/*            0：ｼﾘｱﾙPGIFﾁｬﾝﾈﾙ０から (初期値)						*/
								/*  		  1：ｼﾘｱﾙPGIFﾁｬﾝﾈﾙ１から								*/
								/*  		  2：ﾎｽﾄCPUから											*/
								/*  		  3：内蔵演算部から										*/
								/*------------------------------------------------------------------*/
								/*  bit.8 : DIVSEL0													*/
								/*            0：PA,PB,PC				 							*/
								/*  		  1：ﾃﾞｰﾀﾊﾟﾙｽ変換出力（初期値）							*/
								/*------------------------------------------------------------------*/
								/*  bit.11-10 :DCSEL												*/
								/*            0：通常C相 (初期値)									*/
								/*  		  1：分周C相											*/
								/*  		  2：分周C相2											*/
								/*  		  3：分周C相2											*/
								/*------------------------------------------------------------------*/
								/*  bit.13 : DIVASEL												*/
								/*            0：DA出力 (初期値)									*/
								/*  		  1：ｼﾘｱﾙﾃﾞｰﾀｽﾙｰ出力									*/
								/*------------------------------------------------------------------*/
								/*  bit.15 : DIVCBIT												*/
								/*            0：DC出力 (初期値)									*/
								/*  		  1：DCﾊｲﾚﾍﾞﾙ強制出力									*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->DivInfo.DivOutASIC != FALSE )
	{
		if(MencV->AbsoEncType == MENC_INCRE_TYPE)		/*<S079>*/
		{
			if( MencV->AbsoEncoder == FALSE ) /*<S0D3>*/
			{
				AsicHwReg->AREG_DIVSET	= 0x00000900;  		/* ﾊﾟﾙｽ変換書込み側設定:ｼﾘｱﾙPGIFﾁｬﾝﾈﾙ０から	*/
			}
			else
			{
				AsicHwReg->AREG_DIVSET	= 0x00000910;  		/* ﾊﾟﾙｽ変換書込み側設定:ｼﾘｱﾙPGIFﾁｬﾝﾈﾙ０から	*/
			}
		}
		else
		{ /* ｱﾌﾟｿｴﾝｺｰﾀﾞの場合は、初期ｲﾝｸﾚ出力完了後にｼﾘｱﾙPGIFﾁｬﾝﾈﾙ０から */
			AsicHwReg->AREG_DIVSET	= 0x00000912;  		/* ﾊﾟﾙｽ変換書込み側設定:ﾎｽﾄCPU				*/
		}
	}
	else
	{
		AsicHwReg->AREG_DIVSET	= 0x00000112;  			/* ﾊﾟﾙｽ変換書込み側設定:ﾎｽﾄCPU,				*/
	}
/*--------------------------------------------------------------------------------------------------*/
	AsicHwReg->AREG_POSET0	= 0xFFFFFFFF;				/* パルス変換位置設定						*/
	AsicHwReg->AREG_POSET1	= 0x00000000;				/* パルス変換原点補正１						*/
	AsicHwReg->AREG_POSET2	= 0xFFFFFFFF;				/* パルス変換原点補正２						*/
	AsicHwReg->AREG_POTEST	= 0xFFFFFFFF;				/* パルス変換カウンタ設定					*/
/*--------------------------------------------------------------------------------------------------*/
/*		分周入力フィルタ設定																		*/
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/*	FBCSET:ﾌｨｰﾄﾞﾊﾞｯｸﾊﾟﾙｽｶｳﾝﾀ																		*/
/*--------------------------------------------------------------------------------------------------*/
/*	VULONG	AREG_FBCSET;	     0x0040 分周入力フィルタ設定										*/
								/*------------------------------------------------------------------*/
								/*  bit.1-0 :PA,PBﾌｨﾙﾀ選択											*/
								/*            0：26.7MHz同期 (初期値)								*/
								/*  		  1：37.5ns												*/
								/*  		  2：75ns												*/
								/*  		  3：素通し												*/
								/*------------------------------------------------------------------*/
								/*  bit.3-2 : PCﾌｨﾙﾀ選択											*/
								/*            0：26.7MHz同期 (初期値)								*/
								/*  		  1：37.5ns												*/
								/*  		  2：75ns												*/
								/*  		  3：素通し												*/
								/*------------------------------------------------------------------*/
								/*  bit.5-15 : 省略													*/
/*--------------------------------------------------------------------------------------------------*/
	AsicHwReg->AREG_FBCSET	= 0x00000005;				/* 分周入力フィルタ設定						*/

/*--------------------------------------------------------------------------------------------------*/
/*		逆回転モード(Pn000.0 : 回転方向選択(逆回転モード))設定										*//* <S21B> */
/*--------------------------------------------------------------------------------------------------*/
	MencV->DivInfo.RvsDir = UniPrms->Bprm->RvsDir;
/*--------------------------------------------------------------------------------------------------*/
/*		モータ種別処理：分周パルス計算＆ＡＳＩＣ設定												*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->DivInfo.DivOutASIC != FALSE )
	{
		if( MencV->AxisMotType == MOTTYPE_LINEAR )
		{ /* リニア型 */
			err = LmxSencSetMencDividedPulseHw( AsicHwReg, MencV, UniPrms ); /*<S079>*/
		}
		else
		{ /* 回転型 */
			err = RmxSencSetMencDividedPulseHw( AsicHwReg, MencV, UniPrms );
		}
	}
	else
	{
		if( MencV->AxisMotType == MOTTYPE_LINEAR )
		{ /* リニア型 */
			err = LmxSencSetMencDividedPulse( AsicHwReg, MencV, UniPrms );
		}
		else
		{ /* 回転型 */
			err = RmxSencSetMencDividedPulse( AsicHwReg, MencV, UniPrms );
		}
	}

	return( err );
}

/****************************************************************************************************/
/*																									*/
/*		回転型：シリアルエンコーダ分周出力パルス計算 ＆ ＡＳＩＣ設定								*//*<S02D>*/
/*				(分周出力の位置ﾃﾞｰﾀをJL-086で直接ﾊﾟﾙｽ変換)											*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 分周パルス出力の分周比，分周モードの演算を行う。										*/
/*																									*/
/*																									*/
/*			BitNo				32-BitNo 															*/
/*		|<------------------>|<---------->|		PulseNo = 2^BitNo									*/
/*		+----------------+----------------+		 													*/
/*		|	PulseData		 |000000000000|		RcvPosX : [2^32/rev]								*/
/*		+----------------+----------------+															*/
/*		|<---------------->|<------------>| 	PulseOut = RcvPosX >> DivOutSft						*/
/*			PulseOut		   DivOutSft															*/
/*																									*/
/*							   <---- i ---|															*/
/*		|0000000001XXXXXXXXXXXX10000000000|		pgrate												*/
/*		|00000000000000000001XXXXXXXXXXXX1|		ratex = pgrate >> i									*/
/*						    <---- j ------|															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*				   DivRate        2^32																*/
/*		DivOut  = --------- * ------------- = (4 * pgrate)		[4逓倍pulse/rev]					*/
/*				    65536      2^DivOutSft															*/
/*																									*/
/*				   4 * pgrate * 65536																*/
/*		DivRate = -------------------- = pgrate >> (14 - DivOutSft) = pgrate >> RateSft				*/
/*				   2^(32 - DivOutSft)																*/
/*																									*/
/*		RateSft = 14 - DivOutSft,	DivOutSft = 14 - RateSft										*/
/*																									*/
/*		Condition : DivRate < 65536 , (DivRate % 4) = 0												*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static DIVSET_ERROR	RmxSencSetMencDividedPulseHw( ASIC_HW_IF_STR *AsicHwReg, MENCV *MencV, UNI_PRM_STR *UniPrms )
{
	LONG	i,j,x;
	LONG	BitNo;
	LONG	pulsno;
	LONG	pgrate;
	LONG	IncreSftR;   /*<S079>*/
	LONG	IncreSftX;   /*<S079>*/
	LONG	IncreLeng;   /*<S079>*/
	UCHAR 	*Model;  	 /*<S0D3>*/


	BitNo  = UniPrms->Prm->MencP.bit_dp.b.l;								/* PnF03				*/
	pgrate = UniPrms->Prm->pgrat2l;											/* Pn212(Pn213)			*/
/*--------------------------------------------------------------------------------------------------*/
/*		分周出力パルス上限値設定																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->EncConnect )
	{
		pulsno = (MencV->PulseNo >> 2);
	}
	else
	{
		pulsno = pndef_pgrat2l.UpperLimit;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		分周出力パルス周波数上限速度の計算															*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->DivInfo.DivOvrSpd = RmxSencCalMencDivOverSpeed( (pgrate -1), UniPrms->Bprm->OvrSpd );
	if( MencV->DivInfo.DivOvrSpd == 0 )
	{
		return ( DIVSET_PRMERR_R );                        /* 戻り値：パラメータ設定異常(回転):A.040*/
	}

/*--------------------------------------------------------------------------------------------------*/
/*		パルス数チェック																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( MlibCheckPowerOf2( pulsno ) < 0 )					/* pulsno : ２の累乗チェック			*/
	{
		return ( DIVSET_DIVERR );							/* 戻り値：分周パルス設定異常:A.041		*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		ＰＧ分周比設定範囲チェック																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( (pgrate > pulsno) || (pgrate < 16) )				/* pgrate : 範囲チェック				*/
	{
		return ( DIVSET_DIVERR );							/* 戻り値：分周パルス設定異常:A.041		*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		ＰＧ分周比設定条件のチェック																*/
/*--------------------------------------------------------------------------------------------------*/
	i = MlibSrhbiton( pgrate, SRH_FROM_LSB );				/*						i<---- Search	*/
	x = ((ULONG)pgrate >> i);								/* pgrate : xxxxxxxxxxxx100000			*/
	if( (i < 0) || (x > 16383) )							/* 設定範囲 : 1 <= x <= 16383			*/
	{														/*										*/
		return ( DIVSET_DIVERR );							/*										*/
	}														/*										*/
/*--------------------------------------------------------------------------------------------------*/
/*		分周パルス出力割合の演算 : 11bit (for 日特高速モータ)										*/
/*--------------------------------------------------------------------------------------------------*/
	if( BitNo == 11 )
	{
		MencV->DivInfo.DivMode = FALSE;						/* Mode : 不使用						*/
		MencV->DivInfo.DivPass = FALSE;						/* Pass : 分周モード					*/
		if( pgrate < pulsno )								/* 分周モード : pgrate < pulsno			*/
		{
			MencV->DivInfo.DivRate = (USHORT)(pgrate << 5);	/* Rate = pgrate << (16-(BitNo-2)-2)	*/
		}
		else
		{
			MencV->DivInfo.DivRate = (USHORT)(pulsno << 5);	/* Rate = pulsno << (16-(BitNo-2)-2)	*/
		}
		MencV->DivInfo.DivOutSft = 19;						/* OutSft = 32 - BitNo - 2				*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		分周パルス出力割合の演算 : 16bit未満 (for 13bit Encoder)									*/
/*--------------------------------------------------------------------------------------------------*/
	else if( BitNo < 16 )
	{
		if( pgrate < pulsno )								/* 分周モード : pgrate < pulsno			*/
		{
			MencV->DivInfo.DivMode = FALSE;					/* Mode : 不使用						*/
			MencV->DivInfo.DivPass = FALSE;					/* Pass : 分周モード					*/
			j = 18 - BitNo;									/* j = -(14 - (32-BitNo)) = 18 - BitNo	*/
			MencV->DivInfo.DivRate = (USHORT)(pgrate << j);	/* Rate = pgrate << (16-(BitNo-2))		*/
			MencV->DivInfo.DivOutSft = (UCHAR)(32 - BitNo);	/* OutSft = 32 - BitNo					*/
		}
	/*----------------------------------------------------------------------------------------------*/
		else												/* スルーモード : pgrate == pulsno		*/
		{
			MencV->DivInfo.DivMode = FALSE;					/* Mode : 不使用						*/
			MencV->DivInfo.DivPass = TRUE;					/* Pass : スルーモード					*/
			MencV->DivInfo.DivRate = 0;						/* Rate = 65536 = pgrate << (18-BitNo)	*/
			MencV->DivInfo.DivOutSft = (UCHAR)(32 - BitNo);	/* OutSft = 32 - BitNo					*/
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		分周パルス出力割合の演算 : 16bit以上														*/
/*--------------------------------------------------------------------------------------------------*/
	else if( x != 1 )										/* 分周モード１ : pgrate != 2^i			*/
	{														/* Search ------------------>j			*/
		j = MlibSrhbiton( x, SRH_FROM_MSB );				/*    x : 000000...00000000001xxxx1		*/
		if( (i + j) < 13 ){ j = 13 - i;}					/* Limit j by (OutSft <= 16)			*/
		MencV->DivInfo.DivMode = FALSE;						/* Mode : 不使用						*/
		MencV->DivInfo.DivPass = FALSE;						/* Pass : 分周モード					*/
#if 0 /*<S0C8>*/
		MencV->DivInfo.DivRate = (USHORT)(x << (15-j));		/* Rate = (pgrate>>i)<<(15-j)			*/
#else/*<S0C8>*/
		MencV->DivInfo.DivRate = (USHORT)((x << (15-j)) >> (BitNo - 16));
		j = BitNo - 16;										/* j = BitNo -16						*/
#endif/*<S0C8>*/
		MencV->DivInfo.DivOutSft = (UCHAR)(29 - i - j);		/* OutSft = 14 - i + 15 - j				*/
	}
	/*----------------------------------------------------------------------------------------------*/
	else if( i < 14 )										/* 分周モード２ : pgrate == 2^i			*/
	{
		MencV->DivInfo.DivMode = FALSE;						/* Mode : 不使用						*/
		MencV->DivInfo.DivPass = FALSE;						/* Pass : 分周モード					*/
#if 0 /*<S079>*/
		MencV->DivInfo.DivRate = (USHORT)(pgrate << 2);		/* Rate = pgrate << 2 					*/
#else/*<S079>*/
		MencV->DivInfo.DivOutSft = 16;						/* OutSft = 16							*/
		j = BitNo - 16;										/* j = BitNo -16						*/
		MencV->DivInfo.DivRate = (USHORT)((pgrate << 2)>>j);/* Rate = pgrate << (16-(BitNo-2))		*/
		if( MencV->DivInfo.DivRate == 0 )
		{
			MencV->DivInfo.DivRate = 1;
		}
#endif/*<S079>*/
	}
	/*----------------------------------------------------------------------------------------------*/
	else													/* スルーモード : pgrate == 2^i			*/
	{
		j = BitNo - 16;										/* j = BitNo -16						*//*<S0C8>*/
		MencV->DivInfo.DivMode = FALSE;						/* Mode : 不使用						*/
		MencV->DivInfo.DivPass = TRUE;						/* Pass : スルーモード					*/
		MencV->DivInfo.DivRate = 0;							/* Rate = 65536 = ((pgrate>>i)<<16)		*/
		MencV->DivInfo.DivOutSft = (UCHAR)(30 - i);			/* OutSft = 14 - i + 16 = 30 - i		*/
	}
#if 0/*<S079>*/
/*--------------------------------------------------------------------------------------------------*/
/*		分周Ｃ相出力マスク制御Flagの設定															*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->DivInfo.DivPcmaskCtrl = (MencV->DivInfo.DivOutSft < 16)? TRUE : FALSE;
/*--------------------------------------------------------------------------------------------------*/
/*		分周Ｃ相出力マスク範囲の設定																*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->DivInfo.DivPcmaskCtrl )
	{
		MencV->DivInfo.DivOutCmskChk = (ULONG)0x80000000 >> (BitNo - 16);
	}
#endif/*<S079>*/
/*--------------------------------------------------------------------------------------------------*/
/*		ＡＳＩＣ分周機能設定																		*/
/*--------------------------------------------------------------------------------------------------*/
/*	DINCSET（0x0048）:分周機能																		*/
/*--------------------------------------------------------------------------------------------------*/
/*	VULONG	AREG_DINCSET;	     0x0048 分周機能設定												*/
								/*------------------------------------------------------------------*/
								/*  bit.0  : DA,DBスルーモード設定									*/
								/*            0：分周出力（初期値）									*/
								/*  		  1：PA PBスルー出力									*/
								/*------------------------------------------------------------------*/
								/*  bit.1  : DCスルーモード設定										*/
								/*            0：PCスルー（初期値）									*/
								/*  		  1：PA＆PB＆PC											*/
								/*------------------------------------------------------------------*/
								/*  bit.2  : DB逆転モード設定										*/
								/*            0：DBスルー出力（初期値）								*/
								/*  		  1：DB反転出力											*/
								/*------------------------------------------------------------------*/
								/*  bit.3  : 分周モード選択											*/
								/*            0：16bitDDAモード(初期値)分周比範囲:1/65536,65535/65536*/
								/*  		  1：65005												*/
								/*------------------------------------------------------------------*/
								/*  bit.4  : 新インクレデコーダC相通過 1:エッジ通過					*/
								/*            0：新インクレデコーダC相通過（初期値）				*/
								/*  		  1：エッジ通過											*/
								/*------------------------------------------------------------------*/
								/*  bit.5  : CLSETビット											*/
								/*            0：原点再同期不可（初期値）							*/
								/*  		  1：原点再同期可										*/
								/*------------------------------------------------------------------*/
								/*  bit.10  : 分周常時クリア設定									*/
								/*            0：常時クリアディセーブル（初期値）					*/
								/*  		  1：常時クリアイネーブル								*/
								/*------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
	x = (MencV->DivInfo.DivMode << 4) | MencV->DivInfo.DivPass | 0x0400;
	AsicHwReg->AREG_DINCSET	= (USHORT)x;							/* 分周機能設定					*/
	AsicHwReg->AREG_DIVCSEV	= (USHORT)(MencV->DivInfo.DivRate);		/* 分周比設定					*/

/*--------------------------------------------------------------------------------------------------*/
/*		パルス変換原点補正設定																		*/
/*--------------------------------------------------------------------------------------------------*/
/*	VULONG	DCTRIM;	    0x0190 分周出力原点補正設定(Bit0-23)										*/
/*--------------------------------------------------------------------------------------------------*/
								/*  bit.1-0  : ”0 ”												*/
								/*------------------------------------------------------------------*/
								/*  bit.23-2 : DVCP23-2												*/
								/*  		  分周出力原点補正設定ビット23-2						*/
/*--------------------------------------------------------------------------------------------------*/
	if( BitNo == 13 )
	{
		AsicHwReg->AREG_DCTRIM	= (ULONG)((MencV->DivInfo.DivRate>>3)-1);
	}
	else
	{
		AsicHwReg->AREG_DCTRIM	= (ULONG)( (MencV->DivInfo.DivRate<<j) - 1 );
	}

/*--------------------------------------------------------------------------------------------------*/
/*  DINCSET bit.2 DB逆転モード	(0：DBスルー出力;    １：DB反転出力)								*/
/*--------------------------------------------------------------------------------------------------*/
	if( UniPrms->Bprm->RvsDir )
	{
		AsicHwReg->AREG_DINCSET |= BIT2;					/* DB逆転モード設定						*/
	}

/*--------------------------------------------------------------------------------------------------*/
/*		パルス変換原点補正設定																		*/
/*--------------------------------------------------------------------------------------------------*/
/*	VULONG	AREG_ZDR;	     0x0150 パルス変換原点補正設定											*/
/*--------------------------------------------------------------------------------------------------*/
								/*  bit.3-0  : インクレ補正データ 右シフト量						*/
								/*     ZDRSFT  シリアル受信データD10-D11の16bit中のLSBを決定		*/
								/*------------------------------------------------------------------*/
								/*  bit.7-4  : インクレ補正データ 左シフト量						*/
								/*     ZDLSFT インクレ補正データの下位データに設定値分の0を埋める）*/
								/*------------------------------------------------------------------*/
								/*  bit.11-8 : インクレ補正データ長									*/
								/*     ZDLNG   データ長を（ビット数-1）で設定						*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->AbsoEncoder == FALSE )/*<S0D3>*/
	{
		if( BitNo > 20 )				/*<S079>*/
		{
			IncreSftX = 7;
//			IncreLeng = 8;
		 	IncreLeng = 95 - MencV->DivInfo.IncreBit_dp; /*<S161>*/
		}
		else if( BitNo == 13 )
		{
			IncreSftX = 0;
			IncreLeng = 10;
		}
		else if( BitNo == 17)   /*<S0D3>*/
		{
			IncreSftX = 0;
			Model = &(MencV->EncInfo.Model[0]);
			if( Model[3] == 'J' )
			{/* UTSJ□-B17 の場合 */
				IncreLeng = 9;
			}
			else if((Model[9] == 'C') && (Model[10] == 'C'))
			{/* UTSIH-B17CC（ΣⅡ）*/
				IncreLeng = 10;
			}
			else
			{
				IncreLeng = 8;
			}
		}
		else
		{
			IncreSftX = 0;
			IncreLeng = 8;
		}
		if( MencV->Sigma3Encoder )
		{
			IncreSftX =  IncreSftX + 4;
		}
		IncreSftR = BitNo-(IncreLeng+1); /*<S0D3>*/
	
		AsicHwReg->AREG_ZDR   =	  ( IncreSftX << 0 )			/* インクレ補正データ 右シフト量		*/
								+ ( IncreSftR << 4 )			/* インクレ補正データ 左シフト量		*/
								+ ( IncreLeng << 8 );			/* インクレ補正データ長				*/
	/*--------------------------------------------------------------------------------------------------*/
	/*	VULONG	AREG_ZENBR;	     0x0154 パルス変換原点補正設定											*/
	/*--------------------------------------------------------------------------------------------------*/
									/*------------------------------------------------------------------*/
									/*  bit.5-0  : 原点通過ビット位置設定								*/
									/*   受信データD7-D11の40bit中の原点通過を示すビット位置を決定） */
									/*------------------------------------------------------------------*/
									/*  bit.6  : インクレ補正データ常時更新モード設定					*/
									/*            0：最初の原点通過時のみ設定							*/
									/*  		  1：原点通過時に毎回更新								*/
									/*------------------------------------------------------------------*/
									/*  bit.7  : インクレ補正データセット完了ビットクリア				*/
									/*            0：セット未完											*/
									/*  		  1：セット完了											*/
									/*------------------------------------------------------------------*/
									/*  bit.8  : インクレ補正データセットイネーブル1					*/
									/*      （原点未通過フラグ=0でインクレ補正データをセット）          */
									/*            0：無効												*/
									/*  		  1：有効												*/
									/*------------------------------------------------------------------*/
									/*  bit.9  : インクレ補正データセットイネーブル2					*/
									/*      （インクレ補正ﾃﾞｰﾀの前回値からの変化でインクレ補正ﾃﾞｰﾀをｾｯﾄ)*/
									/*            0：無効												*/
									/*  		  1：有効												*/
									/*------------------------------------------------------------------*/
									/*  bit.10  : インクレ補正データセットイネーブル3					*/
									/*      （原点通過ビットZ=1でインクレ補正データをセット)			*/
									/*            0：無効												*/
									/*  		  1：有効												*/
	/*--------------------------------------------------------------------------------------------------*/
	//	AsicHwReg->AREG_ZENBR = 0x0400;
		AsicHwReg->AREG_ZENBR = 0x0200;/*<S0D3>*/
		AsicHwReg->AREG_WDT1SET &= ~BIT8;				/* パルス変換C相マスクOFF設定			*/
	}
/*--------------------------------------------------------------------------------------------------*/
	return ( DIVSET_SUCCESS );
}


/****************************************************************************************************/
/*																									*/
/*		回転型：シリアルエンコーダ分周出力パルス計算 ＆ ＡＳＩＣ設定								*//*<S02D>*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 分周パルス出力の分周比，分周モードの演算を行う。										*/
/*																									*/
/*																									*/
/*			BitNo				32-BitNo 															*/
/*		|<------------------>|<---------->|		PulseNo = 2^BitNo									*/
/*		+----------------+----------------+		 													*/
/*		|	PulseData		 |000000000000|		RcvPosX : [2^32/rev]								*/
/*		+----------------+----------------+															*/
/*		|<---------------->|<------------>| 	PulseOut = RcvPosX >> DivOutSft						*/
/*			PulseOut		   DivOutSft															*/
/*																									*/
/*							   <---- i ---|															*/
/*		|0000000001XXXXXXXXXXXX10000000000|		pgrate												*/
/*		|00000000000000000001XXXXXXXXXXXX1|		ratex = pgrate >> i									*/
/*						    <---- j ------|															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*				   DivRate        2^32																*/
/*		DivOut  = --------- * ------------- = (4 * pgrate)		[4逓倍pulse/rev]					*/
/*				    65536      2^DivOutSft															*/
/*																									*/
/*				   4 * pgrate * 65536																*/
/*		DivRate = -------------------- = pgrate >> (14 - DivOutSft) = pgrate >> RateSft				*/
/*				   2^(32 - DivOutSft)																*/
/*																									*/
/*		RateSft = 14 - DivOutSft,	DivOutSft = 14 - RateSft										*/
/*																									*/
/*		Condition : DivRate < 65536 , (DivRate % 4) = 0												*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static DIVSET_ERROR	RmxSencSetMencDividedPulse( ASIC_HW_IF_STR *AsicHwReg, MENCV *MencV, UNI_PRM_STR *UniPrms )
{
	LONG	i,j,x;
	LONG	BitNo;
	LONG	pulsno;
	LONG	pgrate;

	BitNo  = UniPrms->Prm->MencP.bit_dp.b.l;								/* PnF03				*/
	pgrate = UniPrms->Prm->pgrat2l;											/* Pn212(Pn213)			*/
/*--------------------------------------------------------------------------------------------------*/
/*		分周出力パルス上限値設定																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->EncConnect )
	{
		pulsno = (MencV->PulseNo >> 2);
	}
	else
	{
		pulsno = pndef_pgrat2l.UpperLimit;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		分周出力パルス周波数上限速度の計算															*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->DivInfo.DivOvrSpd = RmxSencCalMencDivOverSpeed( (pgrate -1), UniPrms->Bprm->OvrSpd );
	if( MencV->DivInfo.DivOvrSpd == 0 )
	{
		return ( DIVSET_PRMERR_R );                        /* 戻り値：パラメータ設定異常(回転):A.040*/
	}

/*--------------------------------------------------------------------------------------------------*/
/*		パルス数チェック																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( MlibCheckPowerOf2( pulsno ) < 0 )					/* pulsno : ２の累乗チェック			*/
	{
		return ( DIVSET_DIVERR );							/* 戻り値：分周パルス設定異常:A.041		*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		ＰＧ分周比設定範囲チェック																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( (pgrate > pulsno) || (pgrate < 16) )				/* pgrate : 範囲チェック				*/
	{
		return ( DIVSET_DIVERR );							/* 戻り値：分周パルス設定異常:A.041		*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		ＰＧ分周比設定条件のチェック																*/
/*--------------------------------------------------------------------------------------------------*/
	i = MlibSrhbiton( pgrate, SRH_FROM_LSB );				/*						i<---- Search	*/
	x = ((ULONG)pgrate >> i);								/* pgrate : xxxxxxxxxxxx100000			*/
	if( (i < 0) || (x > 16383) )							/* 設定範囲 : 1 <= x <= 16383			*/
	{														/*										*/
		return ( DIVSET_DIVERR );							/*										*/
	}														/*										*/
/*--------------------------------------------------------------------------------------------------*/
/*		分周パルス出力割合の演算 : 11bit (for 日特高速モータ)										*/
/*--------------------------------------------------------------------------------------------------*/
	if( BitNo == 11 )
	{
		MencV->DivInfo.DivMode = FALSE;						/* Mode : 不使用						*/
		MencV->DivInfo.DivPass = FALSE;						/* Pass : 分周モード					*/
		if( pgrate < pulsno )								/* 分周モード : pgrate < pulsno			*/
		{
			MencV->DivInfo.DivRate = (USHORT)(pgrate << 5);	/* Rate = pgrate << (16-(BitNo-2)-2)	*/
		}
		else
		{
			MencV->DivInfo.DivRate = (USHORT)(pulsno << 5);	/* Rate = pulsno << (16-(BitNo-2)-2)	*/
		}
		MencV->DivInfo.DivOutSft = 19;						/* OutSft = 32 - BitNo - 2				*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		分周パルス出力割合の演算 : 16bit未満 (for 13bit Encoder)									*/
/*--------------------------------------------------------------------------------------------------*/
	else if( BitNo < 16 )
	{
		if( pgrate < pulsno )								/* 分周モード : pgrate < pulsno			*/
		{
			MencV->DivInfo.DivMode = FALSE;					/* Mode : 不使用						*/
			MencV->DivInfo.DivPass = FALSE;					/* Pass : 分周モード					*/
			j = 18 - BitNo;									/* j = -(14 - (32-BitNo)) = 18 - BitNo	*/
			MencV->DivInfo.DivRate = (USHORT)(pgrate << j);	/* Rate = pgrate << (16-(BitNo-2))		*/
			MencV->DivInfo.DivOutSft = (UCHAR)(32 - BitNo);	/* OutSft = 32 - BitNo					*/
		}
	/*----------------------------------------------------------------------------------------------*/
		else												/* スルーモード : pgrate == pulsno		*/
		{
			MencV->DivInfo.DivMode = FALSE;					/* Mode : 不使用						*/
			MencV->DivInfo.DivPass = TRUE;					/* Pass : スルーモード					*/
			MencV->DivInfo.DivRate = 0;						/* Rate = 65536 = pgrate << (18-BitNo)	*/
			MencV->DivInfo.DivOutSft = (UCHAR)(32 - BitNo);	/* OutSft = 32 - BitNo					*/
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		分周パルス出力割合の演算 : 16bit以上														*/
/*--------------------------------------------------------------------------------------------------*/
	else if( x != 1 )										/* 分周モード１ : pgrate != 2^i			*/
	{														/* Search ------------------>j			*/
		j = MlibSrhbiton( x, SRH_FROM_MSB );				/*    x : 000000...00000000001xxxx1		*/
		if( (i + j) < 13 ){ j = 13 - i;}					/* Limit j by (OutSft <= 16)			*/
		MencV->DivInfo.DivMode = FALSE;						/* Mode : 不使用						*/
		MencV->DivInfo.DivPass = FALSE;						/* Pass : 分周モード					*/
		MencV->DivInfo.DivRate = (USHORT)(x << (15-j));		/* Rate = (pgrate>>i)<<(15-j)			*/
		MencV->DivInfo.DivOutSft = (UCHAR)(29 - i - j);		/* OutSft = 14 - i + 15 - j				*/
	}
	/*----------------------------------------------------------------------------------------------*/
	else if( i < 14 )										/* 分周モード２ : pgrate == 2^i			*/
	{
		MencV->DivInfo.DivMode = FALSE;						/* Mode : 不使用						*/
		MencV->DivInfo.DivPass = FALSE;						/* Pass : 分周モード					*/
		MencV->DivInfo.DivRate = (USHORT)(pgrate << 2);		/* Rate = pgrate << 2 					*/
		MencV->DivInfo.DivOutSft = 16;						/* OutSft = 16							*/
	}
	/*----------------------------------------------------------------------------------------------*/
	else													/* スルーモード : pgrate == 2^i			*/
	{
		MencV->DivInfo.DivMode = FALSE;						/* Mode : 不使用						*/
		MencV->DivInfo.DivPass = TRUE;						/* Pass : スルーモード					*/
		MencV->DivInfo.DivRate = 0;							/* Rate = 65536 = ((pgrate>>i)<<16)		*/
		MencV->DivInfo.DivOutSft = (UCHAR)(30 - i);			/* OutSft = 14 - i + 16 = 30 - i		*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		分周Ｃ相出力マスク制御Flagの設定															*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->DivInfo.DivPcmaskCtrl = (MencV->DivInfo.DivOutSft < 16)? TRUE : FALSE;
/*--------------------------------------------------------------------------------------------------*/
/*		分周Ｃ相出力マスク範囲の設定																*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->DivInfo.DivPcmaskCtrl )
	{
		MencV->DivInfo.DivOutCmskChk = (ULONG)0x80000000 >> (BitNo - 16);
	}
/*--------------------------------------------------------------------------------------------------*/
/*		ＡＳＩＣ分周機能設定																		*/
/*--------------------------------------------------------------------------------------------------*/
	x = (MencV->DivInfo.DivMode << 4) | MencV->DivInfo.DivPass | 0x0400;
	AsicHwReg->AREG_DINCSET	= (USHORT)x;						/* 分周機能設定						*/
	AsicHwReg->AREG_DIVCSEV	= (USHORT)(MencV->DivInfo.DivRate); /* 分周比設定						*/
	/*----------------------------------------------------------------------------------------------*/
	/* 分周出力原点補正 */
	if( BitNo <= 13 )
	{
		AsicHwReg->AREG_DCTRIM	= (ULONG)((MencV->DivInfo.DivRate>>3)-1);
	}
	else
	{
		AsicHwReg->AREG_DCTRIM	= (ULONG)( MencV->DivInfo.DivRate - 1 );
	}
	/*----------------------------------------------------------------------------------------------*/
	if( UniPrms->Bprm->RvsDir )
	{
		AsicHwReg->AREG_DINCSET |= BIT2;					/* DB逆転モード設定						*/
	}
	return ( DIVSET_SUCCESS );
}

/****************************************************************************************************/
/*																									*/
/*		回転型：シリアルエンコーダ分周出力パルス設定値の調整										*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 分周出力パルス設定において、設定値が設定可能な値となるように調整する。					*/
/*																									*/
/****************************************************************************************************/
//<S149>#define	BITNOCHK	14							/* 2^14 = 16384									*/
//<S149>#define	BITNOCHK		18							/* 2^18 = 262144								*//*<S02D>*/
/*--------------------------------------------------------------------------------------------------*/
#if 0			/* <S149> */
PRM_RSLT	RmxSencAdjustMencDivPulse( PRMDATA *Prm, MENCV *MencV, PRM_ACCCMD Cmd, LONG Data )
{
	ULONG	MaskX;
	ULONG	DataX;
	ULONG	BitNoX;
	ULONG	PulseNoX;
	ULONG	DataMinX = pndef_pgrat2l.LowerLimit;

/*--------------------------------------------------------------------------------------------------*/
/*		分周出力パルス上限値設定																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->EncConnect )
	{
		PulseNoX = (MencV->PulseNo >> 2);
	}
	else
	{
		PulseNoX = pndef_pgrat2l.UpperLimit;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		パラメータ計算処理(パラメータチェック)														*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->AxisMotType == MOTTYPE_LINEAR )
	{
		return( PRM_RSLT_SUCCESS );
	}
	else
	{
		DataX = Prm->pgrat2l;
		BitNoX = MlibSrhbiton( DataX, SRH_FROM_MSB );
		if( DataX > PulseNoX  ){ return( PRM_RSLT_LIMIT_ERR );}
		if( DataX < DataMinX  ){ return( PRM_RSLT_LIMIT_ERR );}
		if( BitNoX < BITNOCHK ){ return( PRM_RSLT_SUCCESS  );}
		MaskX = ( 0xFFFFFFFF << (BitNoX + 1 - BITNOCHK) );
		if( (DataX & MaskX) == DataX ){ return( PRM_RSLT_SUCCESS );}
		return( PRM_RSLT_CALC_ERR );
	}
}
#endif
/****************************************************************************************************/
/*																									*/
/*		回転型：シリアルエンコーダ分周出力パルス設定値の調整 (現在、不使用) <S149>					*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 分周出力パルス設定において、設定値が設定可能な値となるように調整する。					*/
/*																									*/
/*	用 途 : オペレータでの分周パルス設定値時の設定値調整用											*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#define	BITNOCHK	14							/* 2^14 = 16384										*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT	RmxSencAdjustMencDivPulse( PRMDATA *Prm, MENCV *MencV, PRM_ACCCMD Cmd, LONG Data )
{
ULONG	MaskX;
ULONG	DataX;
ULONG	BitNoX;
ULONG	PulseNoX;
ULONG	DataMinX = pndef_pgrat2l.LowerLimit;

/*--------------------------------------------------------------------------------------------------*/
/*		分周出力パルス上限値設定																	*/
/*--------------------------------------------------------------------------------------------------*/
		if( MencV->EncConnect )
		{
			PulseNoX = (MencV->PulseNo >> 2);
		}
		else
		{
			PulseNoX = pndef_pgrat2l.UpperLimit;
		}

/*--------------------------------------------------------------------------------------------------*/
/*		パラメータ計算処理(パラメータチェック)														*/
/*--------------------------------------------------------------------------------------------------*/
		if( ( Cmd == PRM_ACCCMD_WRITE) || ( Cmd == PRM_ACCCMD_EEPWRITE) 
			|| ( Cmd == PRM_ACCCMD_RECALC ) )
		{
			if( MencV->AxisMotType == MOTTYPE_LINEAR )
			{
				return( TRUE );
			}
			else
			{
				DataX = Prm->pgrat2l;
				BitNoX = MlibSrhbiton( DataX, SRH_FROM_MSB );
				if( DataX > PulseNoX  ){ return( PRM_RSLT_LIMIT_ERR );}
				if( DataX < DataMinX  ){ return( PRM_RSLT_LIMIT_ERR );}
				if( BitNoX < BITNOCHK ){ return( PRM_RSLT_SUCCESS );}
				MaskX = ( 0xFFFFFFFF << (BitNoX + 1 - BITNOCHK) );
				if( (DataX & MaskX) == DataX ){ return( PRM_RSLT_SUCCESS );}
				return( PRM_RSLT_CALC_ERR );
			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Inc/Decデータ調整処理																		*/
/*--------------------------------------------------------------------------------------------------*/
		DataX = Data;
		BitNoX = MlibSrhbiton( DataX, SRH_FROM_MSB );
/*--------------------------------------------------------------------------------------------------*/
/*		設定範囲チェック																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( DataX >= PulseNoX )
		{
			DataX = PulseNoX;									/* パルス数で上限リミット			*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Data >= 2^BITNOCHK の場合は、設定値を調整する												*/
/*--------------------------------------------------------------------------------------------------*/
		else if( BitNoX >= BITNOCHK )
		{
			MaskX = ( 0xFFFFFFFF << (BitNoX + 1 - BITNOCHK) );
			if( Cmd == PRM_ACCCMD_ADJINCDATA )					/* Adjust Inc. Data					*/
			{
				DataX = (DataX + ~MaskX) & MaskX;				/* Round Up							*/
			}
			else if( Cmd == PRM_ACCCMD_ADJDECDATA )				/* Adjust Dec. Data					*/
			{
				DataX = DataX & MaskX;							/* Round Down						*/
			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Data < 2^BITNOCHK の場合は、すべて設定可能なので、何もしない								*/
/*--------------------------------------------------------------------------------------------------*/
		else
		{
			;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Upper/Lower Limit																			*/
/*--------------------------------------------------------------------------------------------------*/
		DataX = MlibLIMITUL( DataX, PulseNoX, DataMinX );
		return( DataX );
}



/****************************************************************************************************/
/*																									*/
/*		リニアモータ : 分周出力パルス計算 ＆ ＡＳＩＣ設定								<S079>		*/
/*				(分周出力の位置ﾃﾞｰﾀをJL-086で直接ﾊﾟﾙｽ変換)											*/
/****************************************************************************************************/
static DIVSET_ERROR LmxSencSetMencDividedPulseHw( ASIC_HW_IF_STR *AsicHwReg, MENCV *MencV, UNI_PRM_STR *UniPrms )
{
	LONG			x;
	LONG			BitNo;
	DIVSET_ERROR	err;

	BitNo  = UniPrms->Prm->MencP.bit_dp.b.l;

/*--------------------------------------------------------------------------------------------------*/
/*		分周出力パルス周波数上限速度の計算															*/
/*--------------------------------------------------------------------------------------------------*/
	err = LmxSencCalMencDivOverSpeed( MencV, UniPrms );

/*--------------------------------------------------------------------------------------------------*/
/*		分周パルス出力ゲインの演算																	*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->DivInfo.DivMode = FALSE;
	MencV->DivInfo.DivPass = TRUE;
	MencV->DivInfo.DivRate = 0;
	MencV->DivInfo.DivOutGain = MlibScalKxgain( UniPrms->Prm->scalepgrat, 1, (1 << BitNo), NULL, 24 );
	MencV->DivInfo.DivOutCmskChk = MlibScalKxkxks( 0x1000, 1, MencV->DivInfo.DivOutGain, NULL, -30 );
/*--------------------------------------------------------------------------------------------------*/
/*		ＡＳＩＣ分周機能設定																		*/
/*--------------------------------------------------------------------------------------------------*/
	x = (MencV->DivInfo.DivMode << 4) | MencV->DivInfo.DivPass | 0x0400;
	AsicHwReg->AREG_DINCSET = (USHORT)x;						/* 分周機能設定						*/
	AsicHwReg->AREG_DIVCSEV = (USHORT)MencV->DivInfo.DivRate;	/* 分周比設定						*/
	AsicHwReg->AREG_DCTRIMH = 0x00;
	AsicHwReg->AREG_DCTRIM  = (USHORT)(MencV->DivInfo.DivRate - 1);
	if( UniPrms->Bprm->RvsDir )
	{
		AsicHwReg->AREG_DINCSET |= BIT2;						/* DB逆転モード設定					*/
	}
	return (err);
}

/****************************************************************************************************/
/*																									*/
/*		リニアモータ : 分周出力パルス計算 ＆ ＡＳＩＣ設定											*/
/*																									*/
/****************************************************************************************************/
static DIVSET_ERROR LmxSencSetMencDividedPulse( ASIC_HW_IF_STR *AsicHwReg, MENCV *MencV, UNI_PRM_STR *UniPrms )
{
	LONG			x;
	LONG			BitNo;
	DIVSET_ERROR	err;

	BitNo  = UniPrms->Prm->MencP.bit_dp.b.l;

/*--------------------------------------------------------------------------------------------------*/
/*		分周出力パルス周波数上限速度の計算															*/
/*--------------------------------------------------------------------------------------------------*/
	err = LmxSencCalMencDivOverSpeed( MencV, UniPrms );

/*--------------------------------------------------------------------------------------------------*/
/*		分周パルス出力ゲインの演算																	*/
/*--------------------------------------------------------------------------------------------------*/
#if 0 /*<S02D>*/
	MencV->DivMode = FALSE;
	MencV->DivPass = TRUE;
	MencV->DivRate = 0;
	MencV->DivOutGain = MlibScalKxgain( UniPrms->Prm->scalepgrat, 1, (1 << BitNo), NULL, 24 );
	MencV->DivOutCmskChk = MlibScalKxkxks( 0x1000, 1, MencV->DivOutGain, NULL, -30 );
/*--------------------------------------------------------------------------------------------------*/
/*		ＡＳＩＣ分周機能設定																		*/
/*--------------------------------------------------------------------------------------------------*/
	x = (MencV->DivMode << 4) | MencV->DivPass | 0x0400;
	AsicHwReg->AREG_DINCSET = (USHORT)x;							/* 分周機能設定						*/
	AsicHwReg->AREG_DIVCSEV = (USHORT)MencV->DivRate;			/* 分周比設定						*/
	AsicHwReg->AREG_DCTRIMH = 0x00;
	AsicHwReg->AREG_DCTRIM  = (USHORT)(MencV->DivRate - 1);
	if( UniPrms->Bprm->RvsDir )
	{
		AsicHwReg->AREG_DINCSET |= BIT2;							/* DB逆転モード設定					*/
	}
#else/*<S02D>*/
	MencV->DivInfo.DivMode = FALSE;
	MencV->DivInfo.DivPass = TRUE;
	MencV->DivInfo.DivRate = 0;
	MencV->DivInfo.DivOutGain = MlibScalKxgain( UniPrms->Prm->scalepgrat, 1, (1 << BitNo), NULL, 24 );
	MencV->DivInfo.DivOutCmskChk = MlibScalKxkxks( 0x1000, 1, MencV->DivInfo.DivOutGain, NULL, -30 );
/*--------------------------------------------------------------------------------------------------*/
/*		ＡＳＩＣ分周機能設定																		*/
/*--------------------------------------------------------------------------------------------------*/
	x = (MencV->DivInfo.DivMode << 4) | MencV->DivInfo.DivPass | 0x0400;
	AsicHwReg->AREG_DINCSET = (USHORT)x;						/* 分周機能設定						*/
	AsicHwReg->AREG_DIVCSEV = (USHORT)MencV->DivInfo.DivRate;	/* 分周比設定						*/
	AsicHwReg->AREG_DCTRIMH = 0x00;
	AsicHwReg->AREG_DCTRIM  = (USHORT)(MencV->DivInfo.DivRate - 1);
	if( UniPrms->Bprm->RvsDir )
	{
		AsicHwReg->AREG_DINCSET |= BIT2;						/* DB逆転モード設定					*/
	}

#endif/*<S02D>*/
	return (err);
}
//#endif /* MENCV関連の構造体を変更する必要あり？ *//*<S02D>削除*/
/****************************************************************************************************/
/*																									*/
/*		回転型：分周出力パルス周波数上限速度の計算													*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 分周出力パルスが最大周波数となる回転速度を算出する。									*/
/*																									*/
/*			パルス周波数上限(SVCDEF_DIVOUT_MAXFREQ) :												*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*			MaxFreq [pps] = pgrate [pulse/rev] * DivOvrSpd [rev/s]									*/
/*																									*/
/*									       MaxFreq * 2^24											*/
/*			DivOvrSpd[2^24/OvrSpd] = ----------------------------	 								*/
/*									  pgrate  * (OvrSpd / 2*PAI) 									*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	LONG	RmxSencCalMencDivOverSpeed( LONG pgrate, KSGAIN OvrSpd )
{
#if (FLOAT_USE==TRUE)
	float	fw;
#else
	LONG	kx, sx;
#endif /* FLOAT_USE */
	LONG	MaxFreq;											/* [pulse/sec]						*/
	LONG	i;
	LONG	pulsno;
	LONG	rc;

	MaxFreq = SVCDEF_DIVOUT_MAXFREQ;
	pulsno = 1;
/*--------------------------------------------------------------------------------------------------*/
/*		分周上限速度基準値算出																		*/
/*--------------------------------------------------------------------------------------------------*/
	for ( i = 0; pgrate > 0; i++ )
	{
		pgrate = pgrate / 2;
		pulsno = pulsno * 2;
	}
/*--------------------------------------------------------------------------------------------------*/
/*		分周出力パルス周波数上限速度の計算															*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	fw = (float)MaxFreq * (float)0x01000000 / (float)pulsno;
	fw = fw / OvrSpd;
	fw = fw * 62832.0F / 10000.0F;
	if( fw > (float)0x01000000 )
	{
		rc = 0x01000000;
	}
	else
	{
		rc = (LONG)fw;
	}
#else
	kx = MlibScalKxgain( MaxFreq, 0x01000000, pulsno, &sx,   0 );
	kx = MlibPcalKxkxks( kx,      1,		  OvrSpd, &sx,   0 );
	rc = MlibPcalKxgain( kx,      62832,      10000,  &sx, -24 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
	return( rc );
}

/****************************************************************************************************/
/*																									*/
/*		リニアモータ : 分周出力パルス周波数上限速度の計算											*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 分周出力パルスが最大周波数となる回転速度を算出する。									*/
/*																									*/
/*			パルス周波数上限(SVCDEF_DIVOUT_MAXFREQ) : 												*/
/*																									*/
/*			※ SGDS Linear Source では DIVPLSMAXFRQ = 1802240 [pulse/sec]							*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*			MaxFreq [pps] = pgrate [pulse/m] * DivOvrSpd [m/s]										*/
/*																									*/
/*									       MaxFreq * 2^24											*/
/*			DivOvrSpd[2^24/OvrSpd] = ----------------------------	 								*/
/*									      pgrate  * OvrSpd 											*/
/*																									*/
/*								 scalepgrat * 100000000												*/
/*			pgrate[pulse/m] = ---------------------------- 											*/
/*								     ScalePitch * 4													*/
/*																									*/
/*								 MaxFreq * 1000														*/
/*			MaxDivSpd[mm/s] = --------------------	 												*/
/*								     pgrate 														*/
/*																									*/
/*										 MaxFreq * 2^BitNo											*/
/*			MaxDivRat[pulse/spitch] = -----------------------										*/
/*										    MaxPspd / 4												*/
/*																									*/
/*			OvrSpd     : 最大速度 [rad/s], [m/s]													*/
/*			ScalePitch : スケールピッチ [0.01um]													*/
/*			scalepgrat : エンコーダ出力分解能 [pulse/spitch]										*/
/*			MaxPspd    : モータ最高速度 [pulse/sec]													*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static DIVSET_ERROR LmxSencCalMencDivOverSpeed( MENCV *MencV, UNI_PRM_STR *UniPrms )
{

#if (FLOAT_USE==TRUE)
	float	fw;
#else	
	LONG	kx, sx;
#endif /* FLOAT_USE */

	KSGAIN			pgrate;
	KSGAIN			MaxFreq; /* [pulse/sec] */
	LONG			MaxSpd;  /* [r/min], [mm/s] */
	LONG			BitNo;
	DIVSET_ERROR	err;

	err = DIVSET_SUCCESS;

	MaxFreq = SVCDEF_DIVOUT_MAXFREQ;
	MaxSpd  = UniPrms->Bprm->MaxVel * 100;
	BitNo   = UniPrms->Prm->MencP.bit_dp.b.l;

/*--------------------------------------------------------------------------------------------------*/
/*		ＰＧ分周比 [pulse/m]																		*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	fw = (float)UniPrms->Prm->scalepgrat * 250000000.0f / (float)MencV->NormScalePitch;
	pgrate = fw * (float)MencV->NormScaleExp;
#else
	kx     = MlibScalKxgain( UniPrms->Prm->scalepgrat, 250000000, MencV->NormScalePitch, &sx, 0 );
	pgrate = MlibPcalKxgain( kx, MencV->NormScaleExp, 1, &sx, -1 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		分周出力パルス周波数上限速度 [2^24/OvrSpd] の計算											*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	fw = (float)MaxFreq * (float)0x01000000 / pgrate;
	fw = fw / (float)UniPrms->Bprm->OvrSpd;
	if( fw > (float)0x01000000 )
	{
		MencV->DivInfo.DivOvrSpd = 0x01000000;
	}
	else
	{
		MencV->DivInfo.DivOvrSpd = (LONG)fw;
	}
#else
	kx = MlibScalKxkxks( MaxFreq, 0x01000000, pgrate, &sx, 0 );
	kx = MlibPcalKxkxks( kx, 1, UniPrms->Bprm->OvrSpd, &sx, -24 );
	if( kx > 0x01000000 )
	{
		MencV->DivInfo.DivOvrSpd = 0x01000000;
	}
	else
	{
		MencV->DivInfo.DivOvrSpd = kx;
	}
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		分周出力パルス周波数上限時の最高速度 [mm/s] の計算											*/
/*--------------------------------------------------------------------------------------------------*/
#if 0 /* 2010.04.13 Y.Oka MlibGAINRDを使用したくないため、floatにて演算する */

	fw = (float)MaxFreq * 1000.0f / pgrate;
	if( fw > (float)MaxSpd )
	{
		MencV->MaxDivSpd = MaxSpd;
	}
	else
	{
		MencV->MaxDivSpd = (LONG)fw;
	}
#endif
#if (FLOAT_USE==TRUE)
	fw = (float)MaxFreq * 1000.0f / pgrate;
	if( fw > (float)MaxSpd )
	{
		MencV->DivInfo.MaxDivSpd = MaxSpd;
	}
	else
	{
		MencV->DivInfo.MaxDivSpd = (LONG)fw;
	}
#else
	kx = MlibScalKxkxks( MaxFreq, 1000, pgrate, NULL, 24 );
	kx = MlibGAINRD( kx );
	if( kx > MaxSpd )
	{
		MencV->DivInfo.MaxDivSpd = MaxSpd;
	}
	else
	{
		MencV->DivInfo.MaxDivSpd = kx;
	}
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		最高速度時の分周比設定上限値 [pulse/spitch] の計算											*/
/*--------------------------------------------------------------------------------------------------*/
#if 0 /* 2010.04.13 Y.Oka MlibGAINRDを使用したくないため、floatにて演算する */
	fw = (float)MaxFreq * (float)(4 << BitNo) / Bprm->MaxPspd;
	if( fw > (float)(1<<BitNo) )
	{
		MencV->MaxDivRat = 1 << BitNo;
	}
	else
	{
		MencV->MaxDivRat = (LONG)fw;
	}
#endif
#if (FLOAT_USE==TRUE)
	fw = (float)MaxFreq * (float)(4 << BitNo) / UniPrms->Bprm->MaxPspd;
	if( fw > (float)(1<<BitNo) )
	{
		MencV->DivInfo.MaxDivRat = 1 << BitNo;
	}
	else
	{
		MencV->DivInfo.MaxDivRat = (LONG)fw;
	}
#else
	kx = MlibScalKxkxks( MaxFreq, (4 << BitNo), UniPrms->Bprm->MaxPspd, NULL, 24 );
	kx = MlibGAINRD( kx );
	if( kx > (1<<BitNo) )
	{
		MencV->DivInfo.MaxDivRat = 1 << BitNo;
	}
	else
	{
		MencV->DivInfo.MaxDivRat = kx;
	}
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		分周パルス出力設定異常																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( (MencV->DivInfo.MaxDivRat < (LONG)UniPrms->Prm->scalepgrat) || (MencV->DivInfo.MaxDivSpd < MaxSpd) )
	{
		err = DIVSET_DIVERR;							/* A.041 : 分周パルス出力設定異常	*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		パラメータ設定異常																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->DivInfo.DivOvrSpd == 0 )
	{
		err = DIVSET_PRMERR_L;							/* A.040 : パラメータエラーの設定	*/
	}
	return (err);
}






/****************************************************************************************************/
/*																									*/
/*		回転型：エンコーダ初期パラメータ計算														*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : サーボ基本パラメータ計算(BprmCalc.c)後、エンコーダＩＦ処理に必要なパラメータの			*/
/*			計算を行う。																			*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static void	SencInitParamCalcForRotary( MENCV *MencV, MENCPRM *MencP, MENC_REF_PRM *MencIPrm )
{
#if (FLOAT_USE==TRUE)
	float	fw;
#else
	LONG	sx;
#endif
	LONG	kx;
	UCHAR	BitNo;
	UCHAR	Dpoint;

	BitNo  = MencP->bit_dp.b.l;
	Dpoint = MencP->bit_dp.b.h;

/*--------------------------------------------------------------------------------------------------*/
/*		位相原点演算用パラメータの計算																*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->MorgSft      = 100 - Dpoint;
	MencV->MorgPassSft  =  96 - Dpoint;
	if( MencV->Sigma3Encoder )
	{
		MencV->IncreCompSft = 4;
	}
	else
	{
		MencV->IncreCompSft = 0;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		位置演算用パラメータの計算																	*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->MposSftR = 32 - BitNo;
	MencV->MposSftX = MencV->MposSftR - (80 - Dpoint);
	MencV->MposSign = (MencV->RevAsmMotor == TRUE)?  -1 : 1;
	MencV->MultiturnSft = MencP->bit_dp.b.h - 64;

/*--------------------------------------------------------------------------------------------------*/
/*		Ｃ相レスフラグ設定																			*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->CphaseLess = FALSE;

/*--------------------------------------------------------------------------------------------------*/
/*		モータ磁極位相演算係数の計算																*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		MotPhase [65536/360] = (PoleNo/2) * (MotPosX [2^32/Rev] >> 16)								*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->Kmotphase = (MencP->oslv_pol.b.h >> 1);

/*--------------------------------------------------------------------------------------------------*/
/*		加速度異常検出レベル計算																	*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*					  MaxTrq * 2^32	*  Ts  *  Ts			MaxTrq * Ts * Ts						*/
/*		AccErrLvl = ------------------------------ * Cx	 = ------------------- * Cx			 		*/
/*					   Jmot * 2*PAI	* 10^9 * 10^9			Jmot * 1462918079						*/
/*																									*/
/*		MaxTrq [Nm],[N]					: 最大トルク												*/
/*		Jmot   [kgm^2],[kg]				: モータイナーシャ											*/
/*		Kfbpls [rad/pulse],[m/pulse]	: ＦＢパルス変換係数										*/
/*		Ts     [ns]	   					: 演算周期													*/
/*		Cx     [-]	   					: 安全係数 (=400.000) 										*/
/*																									*/
/*		※安全係数は回転型SGDSの約400倍に合わせた。													*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	fw = (float)KPI_SACYCLENS * (float)KPI_SACYCLENS / 1462918.0F;
	fw = fw * MencIPrm->MaxTrq / MencIPrm->Jmot;
	fw = fw * 400000.0F / 1000000.0F;
#else	
	kx = MlibScalKxgain( KPI_SACYCLENS, KPI_SACYCLENS, 1462918, &sx,  0 );
	kx = MlibPcalKxksks( kx, MencIPrm->MaxTrq, MencIPrm->Jmot, &sx,   0 );
	kx = MlibPcalKxgain( kx, 400000,        1000000,   &sx, -30 );
#endif /* FLOAT_USE */

	if( MencV->PosCompMode == POSCOMP_SW )/* <S004> *//*<S05C>rename*/
	{/* SWにてエンコーダ位置補正(Σ-Ⅴ互換):32bit正規化 */
		MencV->AccErrLvl = kx;
	}
	else
	{/* JL-086にてエンコーダ位置補正:24bit正規化		*/
		MencV->AccErrLvl = kx >> ( 32 - 24);
		if( MencV->AccErrLvl > 0x007FFFFF )
		{
			MencV->AccErrLvl = 0x007FFFFF;
		}
	}

/*--------------------------------------------------------------------------------------------------*/
/*		速度換算ゲイン計算	[pulse/ScanA]→[2^24/]													*/
/*--------------------------------------------------------------------------------------------------*/
//	MencV->MotspdA = MlibScalKskxkx( bprm_ptr->Kmotspd, 1000000000, KPI_SACYCLENS, NULL, 24 );

/*--------------------------------------------------------------------------------------------------*/
/*		移動量変換ゲインの計算																		*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*				     1000																			*/
/*		Kmovpos = -----------		[0.001rev/pulse]												*/
/*				   (2^BitNo) 																		*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	fw = 1000.0f / (1 << BitNo);
#else
	kx = MlibScalKxgain( 1000, 1, (1 << BitNo), NULL, 24 );
#endif /* FLOAT_USE */

#if (FLOAT_USE==TRUE)
	MencV->Kmovpos =  fw ;
#else
	MencV->Kmovpos = kx;
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		移動量変換ゲインの計算																		*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*					 pole * 65536																	*/
/*		Kphasepos = --------------		[65536/(360deg)/pulse]										*/
/*					(2^BitNo) * 2 																	*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	fw = MencP->oslv_pol.b.h * 32768.0f / (1 << BitNo);
#else
	kx = MlibScalKxgain( MencP->oslv_pol.b.h, 32768, (1 << BitNo), NULL, 24 );
#endif /* FLOAT_USE */

#if (FLOAT_USE==TRUE)
	MencV->Kphasepos = fw;
#else
	MencV->Kphasepos = kx;
#endif

}



/****************************************************************************************************/
/*																									*/
/*		リニアモータ：エンコーダ初期パラメータ計算													*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : サーボ基本パラメータ計算(BprmCalc.c)後、エンコーダＩＦ処理に必要なパラメータの			*/
/*			計算を行う。																			*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static void	SencInitParamCalcForLinear( MENCV *MencV, MENCPRM *MencP, MENC_REF_PRM *MencIPrm )
{
#if (FLOAT_USE==TRUE)
	float	fw;
#endif
	LONG	kx, sx;
	UCHAR	BitNo;
	UCHAR	Dpoint;

	BitNo  = MencP->bit_dp.b.l;
	Dpoint = MencP->bit_dp.b.h;

/*--------------------------------------------------------------------------------------------------*/
/*		位相原点演算用パラメータの計算																*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->MorgSft      = 100 - Dpoint;
	MencV->MorgPassSft  =  96 - Dpoint;
	if( MencV->Sigma3Encoder )
	{
		MencV->IncreCompSft = 4;
	}
	else
	{
		MencV->IncreCompSft = 0;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		位置演算用パラメータの計算																	*/
/*--------------------------------------------------------------------------------------------------*/
//	MencV->MposSftR = 32 - BitNo;											/* <S004> */
//	MencV->MposSftX = MencV->MposSftR - (80 - Dpoint);			/* <S004> */
	MencV->MposSftR = LMX_MPOS_SFT;									/* <S004> */
	MencV->MposSftX = LMX_MPOS_SFT;									/* <S004> */

	if( MencV->RevAsmMotor == TRUE )
	{
		MencV->MposSign = -1;
	}
	else
	{
		MencV->MposSign = 1;
	}
	MencV->MultiturnSft = MencP->bit_dp.b.h - 64;

/*--------------------------------------------------------------------------------------------------*/
/*		Ｃ相レスフラグ設定																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->AbsoEncoder )
	{
		MencV->CphaseLess = TRUE;
	}
	else
	{
		MencV->CphaseLess = FALSE;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		モータ磁極位相演算係数の計算																*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*						 	   65536							PulseNo  : [pulse/360deg]			*/
/*			Kmotphase	= ---------------						MotPhase : [65536/360deg]			*/
/*						      PulseNo																*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	MencV->Kmotphase = 65536.0f / (float)MencV->PulseNo;
	MencV->Kinvphase = (float)MencV->PulseNo / 65536.0f;
#else
	MencV->Kmotphase = MlibScalKxgain( 65536, 1, MencV->PulseNo, NULL, 24 );
	MencV->Kinvphase = MlibScalKxgain( MencV->PulseNo, 1, 65536, NULL, 24 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		加速度異常検出レベル計算																	*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*					  MaxTrq * 2^12	*  Ts  *  Ts			       MaxTrq * Ts * Ts					*/
/*		AccErrLvl = ------------------------------ * Cx	 = --------------------------------- * Cx	*/
/*					  Jmot * Kfbpls * 10^9 * 10^9			Jmot * Kfbpls * 244140625000000			*/
/*																									*/
/*		MaxTrq [Nm],[N]					: 最大トルク												*/
/*		Jmot   [kgm^2],[kg]				: モータイナーシャ											*/
/*		Kfbpls [rad/pulse],[m/pulse]	: ＦＢパルス変換係数										*/
/*		Ts     [ns]	   					: 演算周期													*/
/*		Cx     [-]	   					: 安全係数 (=400.000) 										*/
/*																									*/
/*		※安全係数は回転型SGDSの約400倍に合わせた。													*/
/*		※ただし、SGDSリニアは位置データがパルス単位であるのに回転型と同じ計算値を使っているので、	*/
/*		  リニアの場合、SGDSとSVOSとで異なる。														*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	fw = (float)KPI_SACYCLENS * (float)KPI_SACYCLENS / 244140625.0F;
	fw = fw * MencIPrm->MaxTrq / MencIPrm->Jmot;
	fw = fw / MencIPrm->Kfbpls;
	fw = fw * 400.0F / 1000000.0F;
#else
	kx = MlibScalKxgain( KPI_SACYCLENS, KPI_SACYCLENS, 244140625, &sx, 0 );
	kx = MlibPcalKxksks( kx, MencIPrm->MaxTrq, MencIPrm->Jmot, &sx, 0 );
	kx = MlibPcalKxkxks( kx, 1, MencIPrm->Kfbpls, &sx, 0 );
	kx = MlibPcalKxgain( kx, 400, 1000000, &sx, -30 );
#endif /*FLOAT_USE==TRUE*/

	if( MencV->PosCompMode == POSCOMP_SW )/* <S004> *//*<S0C5>rename*/
	{/* SWにてエンコーダ位置補正(Σ-Ⅴ互換):32bit正規化 */
		MencV->AccErrLvl = kx;
	}
	else
	{/* JL-086にてエンコーダ位置補正:24bit正規化		*/
		MencV->AccErrLvl = (kx >> (32 - 24));
		if( MencV->AccErrLvl > 0x007FFFFF )
		{
			MencV->AccErrLvl = 0x007FFFFF;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		移動量変換ゲインの計算																		*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*					      ScalePitch					ScalePitch : [nm]							*/
/*		Kmovpos  = ------------------------				Kmovpos    : [0.01mm/pulse]					*/
/*					   (2^BitNo) * 10000															*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
	kx = MlibScalKxgain( MencV->NormScalePitch, 1, (10000<< BitNo), &sx, 0 );
	kx = MlibPcalKxgain( kx, 1, MencV->NormScaleExp, &sx, 24 );

	MencV->Kmovpos = kx;

/*--------------------------------------------------------------------------------------------------*/
/*		移動量変換ゲインの計算																		*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*					   (ScalePitch/10^9) * 65536		ScalePitch : [nm]							*/
/*		Kphasepos = ---------------------------------	[65536/(360deg)/pulse]						*/
/*					(2^BitNo) * (PolePitch/10^4) * 2 	PolePitch  : [0.1mm/180deg]					*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
	kx = MlibScalKxgain( MencV->NormScalePitch, 32768, 100000, &sx, 0 );
	kx = MlibPcalKxgain( kx, 1, (1 << BitNo), &sx, 0 );
	kx = MlibPcalKxgain( kx, 1, MencP->ppitch, &sx, 0 );
	kx = MlibPcalKxgain( kx, 1, MencV->NormScaleExp, &sx, 24 );

	MencV->Kphasepos = kx;

/*--------------------------------------------------------------------------------------------------*/
/*		速度換算ゲイン計算	[pulse/ScanA]→[2^24/]													*/
/*--------------------------------------------------------------------------------------------------*/
//	MencV->MotspdA = MlibScalKskxkx( bprm_ptr->Kmotspd, 1000000000, KPI_SACYCLENS, NULL, 24 );

/*--------------------------------------------------------------------------------------------------*/
/*		PnE8C   : シリアル変換ユニット原点オフセット												*/
/*																									*/
/*		MencP.OrgDetDelayComp = PnE8C [0.01deg]×2^BITNO / 100 / 360[deg]							*/
/*--------------------------------------------------------------------------------------------------*/
/*		※C相位置計算で使用するため、LMX_MPOS_SFTだけシフトしておく									*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->OrgDetDelayComp = ( ((MencIPrm->scnvorgoffset << BitNo) / 100) << LMX_MPOS_SFT ) / 360;

}



/****************************************************************************************************/
/*																									*/
/*		SEN ON時の絶対位置再計算																	*/
/*																									*/
/****************************************************************************************************/
void	SencMakeAbsPosition(MENCV *MencV, USHORT limmlt, DBYTEX bit_dp, BOOL RvsDir)
{
	LONG	IniPos[2];
	LONG	MaxPos[2];
	UCHAR	BitNo;
	UCHAR	Sftx;

	/* ローカル変数の初期化 */
	BitNo	= bit_dp.b.l;
	Sftx	= bit_dp.b.h - BitNo - 48;

	IniPos[1] = (LONG)((USHORT)MencV->RxPosH[0] >> Sftx);			/* <S209> */
	IniPos[0] = (LONG)( ((ULONG)MencV->RxPosH[0] << (32-Sftx))
						 + ((ULONG)MencV->RxPosL[0].dw >> Sftx) );	/* <S209> */

	if( MencV->AbsoEncType == MENC_SABSO_TYPE )		/* 1回転アブソ								*/
	{
		IniPos[1] = 0;
		IniPos[0] = ((ULONG)(IniPos[0]<<(32-BitNo)) >> (32-BitNo));
	}
	else												/* 多回転アブソ								*/
	{
		if( limmlt == 0xFFFF )							/* マルチターンリミットで符号拡張判断		*/
		{												/* 多回転量符号付き							*/
			if( BitNo > 16 )							/* 符号ビットで上位ビットを設定				*/
			{
				IniPos[1] = (LONG)(IniPos[1] << (48-BitNo)) >> (48-BitNo);
			}
			else										/* 下位ワードの符号で上位ワードを設定		*/
			{
				IniPos[1] = IniPos[0] >> 31;			/* 上位ワード設定(下位ワード符号)			*/
			}
		}
	}

	/*----------------------------------------------------------------------------------------------*/
	if( MencV->RevAsmMotor ^ RvsDir )					/* 反転接続 XOR 逆回転モード				*/
	{
		IniPos[1] = (~IniPos[1]);						/* 符号反転処理								*/
		IniPos[0] = (~IniPos[0]) + 1;					/* 符号反転処理								*/
		if( IniPos[0] == 0 )
		{	/* Carryの場合は、上位に+1 */
			++IniPos[1];
		}
	/*----------------------------------------------------------------------------------------------*/
		if( (limmlt != 0xFFFF) && ((IniPos[0] | IniPos[1]) != 0) )
		{
			MaxPos[1] = ((LONG)limmlt + 1) >> (32-BitNo);
			MaxPos[0] = ((LONG)limmlt + 1) <<  BitNo;
			MlibAdd6464( MaxPos, IniPos, IniPos );
		}
	}


	/*	初回マルチターンデータの設定 */
	if( (MencV->AbsoEncoder == FALSE)						/* インクレエンコーダ					*/
		|| (MencV->AbsoEncType == MENC_INCRE_TYPE) )		/* アブソをインクレとして使用			*/
	{
		MencV->MotAbsPos = 0;
		MencV->SenOnScalePosL = 0;
		MencV->SenOnScalePosH = 0;
		MencV->SenOnMtnData = 0;
	}
	else													/* アブソエンコーダ						*/
	{
		MencV->MotAbsPos = IniPos[0];
		MencV->SenOnScalePosL = IniPos[0];
		MencV->SenOnScalePosH = IniPos[1];
		MencV->SenOnMtnData = SencGetMturnData(MencV,
		                                        limmlt, bit_dp,
		                                        (USHORT)MencV->RxPosH[0],
		                                        MencV->RxPosL[0].dw );
	}
	MencV->SenOnIncPulse = (ULONG)(MencV->RxPosL[0].dw >> MencV->MposSftX);
}





/****************************************************************************************************/
/*																									*/
/*		回転型：シリアルエンコーダ変数初期化処理													*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Phase Table for Initial Phase																*/
/*--------------------------------------------------------------------------------------------------*/
static	const	USHORT	PhaseTableUVW[8] = {
/*	UVW   deg	Cal.					65536/360deg												*/
/*--------------------------------------------------------------------------------------------------*/
/*	000 :  NG : 0				*/		    0,
/*	001 : 330 : 65536*330/360	*/		60075,
/*	010 : 210 : 65536*210/360	*/		38229,
/*	011 : 270 : 65536*270/360	*/		49152,
/*	100 :  90 : 65536* 90/360	*/		16384,
/*	101 :  30 : 65536* 30/360	*/		 5461,
/*	110 : 150 : 65536*150/360	*/		27307,
/*	111 :  NG : 0				*/		    0
};
/*--------------------------------------------------------------------------------------------------*/
static SENC_ERROR	SencInitVariablesForRotary( MENCV *MencV, MENCPRM *MencP, ASIC_HW_IF_STR *AsicHwReg )
{
	LONG	i;
	LONG	lwk;
	LONG	RcvPosX;

/*--------------------------------------------------------------------------------------------------*/
/*		初回位置データ(今回,前回,前々回)の設定														*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->MotPos = 0;									/* Motor Position [32bitRingPulse]		*/
	/*----------------------------------------------------------------------------------------------*/
	RcvPosX = MencV->MposSign * (LONG)((MencV->RxPosL[0].dw>>MencV->MposSftX)<<MencV->MposSftR);
	MencV->RcvPosX[0] = MencV->RcvPosX[1] = MencV->RcvPosX[2] = RcvPosX;
	MencV->MotPosX[0] = MencV->MotPosX[1] = RcvPosX;				/* 補正後位置初期化 		*/

/*--------------------------------------------------------------------------------------------------*/
/*		初回マルチターンデータの設定																*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->AbsoEncoder == FALSE )					/* インクレエンコーダ					*/
	{
		MencV->IniMturn = 0;
	}
	else if( MencV->AbsoEncType == MENC_INCRE_TYPE )		/* アブソをインクレとして使用			*/
	{
		MencV->IniMturn = 0;
	}
	else													/* アブソエンコーダ						*/
	{
		MencV->IniMturn = SencGetMturnData(MencV,
	                                        MencP->limmlt, MencP->bit_dp,
		                                    (USHORT)MencV->RxPosH[0],
		                                    MencV->RxPosL[0].dw);
	}
	MencV->IniIncre = ((ULONG)RcvPosX >> MencV->MposSftR);

/*--------------------------------------------------------------------------------------------------*/
/*		Ｃ相フラグ関連,位相原点の初期設定：インクレエンコーダ										*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->AbsoEncoder != TRUE )						/* インクレエンコーダ					*/
	{
		MencV->Cset     = FALSE;							/* Ｃ相設定Flag							*/
		MencV->Cpass    = FALSE;							/* Ｃ相通過Flag							*/
//		MencV->DivCpass = FALSE;							/* 分周Ｃ相通過Flag						*/
		MencV->DivInfo.DivCpass = FALSE;					/* 分周Ｃ相通過Flag						*//*<S02D>*/
		MencV->OrgOk    = FALSE;							/* 原点設定完了Flag						*/
		MencV->CposOk   = FALSE;							/* Ｃ相位置設定完了Flag					*/
	/*----------------------------------------------------------------------------------------------*/
	/*	初期モータ位相原点演算：ＵＶＷ信号より演算													*/
	/*----------------------------------------------------------------------------------------------*/
		if( MencV->Sigma3Encoder )
		{
			i = MencV->RxPosH[0] >> 1;
		}
		else
		{
			i = MencV->RxPosL[0].w.l >> 9;
		}
		lwk = (PhaseTableUVW[(i & 0x07)] * (65536/MencV->Kmotphase));
	/*------------------------------------------------------------------------------------------*/
		MencV->MotOrgX  = RcvPosX - lwk;					/* モータ原点位置						*/
		MencV->MotCposX = 0;								/* モータＣ相位置						*/
		MencV->MotAngle = 0;								/* モータ回転角(パルス)					*/
	/*----------------------------------------------------------------------------------------------*/
	/*	分周出力初期設定																			*/
	/*----------------------------------------------------------------------------------------------*/
//		if( MencV->DivOut == TRUE )						/* モータエンコーダ分周出力選択時		*/
		if( MencV->DivInfo.DivOut == TRUE )					/* モータエンコーダ分周出力選択時		*//*<S02D>*/
		{
			/* 通常Ｃ相出力選択(INGRAM強制出力未対応) */
//			KPI_DIVSET_DCNORMAL( );
#if 0 /*<S02D>*/

			MencV->DivModeStatus = DIV_MODE_NORMAL ;

			AsicHwReg->AREG_WDT1SET |= BIT8;				/* パルス変換Ｃ相マスク					*/
			MencV->DivCMaskStatus = DIV_CMASK_ON;
			AsicHwReg->AREG_POSET0	= (USHORT)(RcvPosX >> MencV->DivOutSft);
			AsicHwReg->AREG_POTEST	= (USHORT)(RcvPosX >> MencV->DivOutSft);
#else/*<S02D>*/
			/* 通常Ｃ相出力選択(INGRAM強制出力未対応) */
			MencV->DivInfo.DivModeStatus = DIV_MODE_NORMAL ;

			AsicHwReg->AREG_WDT1SET |= BIT8;				/* パルス変換Ｃ相マスク					*/
			MencV->DivInfo.DivCMaskStatus = DIV_CMASK_ON;
			AsicHwReg->AREG_POSET0	= (ULONG)(RcvPosX >> MencV->DivInfo.DivOutSft);
			AsicHwReg->AREG_POTEST	= (ULONG)(RcvPosX >> MencV->DivInfo.DivOutSft);
			if(( MencV->DivInfo.DivPass != FALSE )&&(MencV->DivInfo.DivOutASIC != FALSE))          /*<S0D3>*/
			{/* 分周スルーモード、H/W分周の場合　*/
				/* 通常Ｃ相出力選択 */
				AsicHwReg->AREG_DIVSET &= 0xfffff3ff;
				MencV->DivInfo.DivModeStatus = DIV_MODE_NORMAL ;
			}
#endif/*<S02D>*/
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		Ｃ相フラグ関連,位相原点の初期設定：アブソをインクレとして使用								*/
/*--------------------------------------------------------------------------------------------------*/
	else if( MencV->AbsoEncType == MENC_INCRE_TYPE )		/* アブソをインクレとして使用			*/
	{
		MencV->Cset     = TRUE;							/* Ｃ相設定Flag							*/
		MencV->Cpass    = FALSE;							/* Ｃ相通過Flag							*/
//		MencV->DivCpass = FALSE;							/* 分周Ｃ相通過Flag						*/
		MencV->DivInfo.DivCpass = FALSE;					/* 分周Ｃ相通過Flag						*//*<S02D>*/
		MencV->OrgOk    = FALSE;							/* 原点設定完了Flag						*/
		MencV->CposOk   = FALSE;							/* Ｃ相位置設定完了Flag					*/
	/*----------------------------------------------------------------------------------------------*/
		MencV->MotOrgX  = 0;								/* モータ原点位置						*/
		MencV->MotCposX = 0;								/* モータＣ相位置						*/
		MencV->MotAngle = ((ULONG)(RcvPosX) >> MencV->MposSftR); /* モータ回転角(パルス)		*/
	/*----------------------------------------------------------------------------------------------*/
	/*	分周出力初期設定																			*/
	/*----------------------------------------------------------------------------------------------*/
#if 0 /*<S02D>*/
		if( MencV->DivOut == TRUE )						/* モータエンコーダ分周出力選択時		*/
		{
			AsicHwReg->AREG_POSET0	= (USHORT)(RcvPosX >> MencV->DivOutSft);
			AsicHwReg->AREG_POTEST	= (USHORT)(RcvPosX >> MencV->DivOutSft);
			/* 通常Ｃ相出力選択 */
//			KPI_DIVSET_DCNORMAL( );
			MencV->DivModeStatus = DIV_MODE_NORMAL ;

			/* Ｃ相再同期可設定 */
			AsicHwReg->AREG_DINCSET	|= BIT5;

			/* 分周常時クリア設定OFF(必要か？) */
			AsicHwReg->AREG_DINCSET	&= ~(BIT10);

			/* Ｃ相再同期可解除 */
			AsicHwReg->AREG_DINCSET	&= ~(BIT5);

			/* パルス変換Ｃ相マスクＯＮ */
			AsicHwReg->AREG_WDT1SET |= BIT8;
			MencV->DivCMaskStatus = DIV_CMASK_ON;

			/* パルス変換Ｃ相マスクＯＦＦ(INGRAM強制出力未対応) */
//			KPI_PCMASK_OFF( );
			MencV->DivCMaskStatus  = DIV_CMASK_OFF ;
		}
#else/*<S02D>*/
		if( MencV->DivInfo.DivOut == TRUE )					/* モータエンコーダ分周出力選択時		*/
		{
			AsicHwReg->AREG_POSET0	= (ULONG)(RcvPosX >> MencV->DivInfo.DivOutSft);
			AsicHwReg->AREG_POTEST	= (ULONG)(RcvPosX >> MencV->DivInfo.DivOutSft);
			/* 通常Ｃ相出力選択				*/
			AsicHwReg->AREG_DIVSET &= 0xfffff3ff;
			MencV->DivInfo.DivModeStatus = DIV_MODE_NORMAL ;

			/* Ｃ相再同期可設定 */
			AsicHwReg->AREG_DINCSET	|= BIT5;

			/* 分周常時クリア設定OFF(必要か？) */
			AsicHwReg->AREG_DINCSET	&= ~(BIT10);

			/* Ｃ相再同期可解除 */
			AsicHwReg->AREG_DINCSET	&= ~(BIT5);

			/* パルス変換Ｃ相マスクＯＮ */
			AsicHwReg->AREG_WDT1SET |= BIT8;
			MencV->DivInfo.DivCMaskStatus = DIV_CMASK_ON;

			/* パルス変換Ｃ相マスクＯＦＦ */
			AsicHwReg->AREG_WDT1SET &= ~BIT8;
			MencV->DivInfo.DivCMaskStatus  = DIV_CMASK_OFF ;
		}
#endif/*<S02D>*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		Ｃ相フラグ関連,位相原点の初期設定：アブソをアブソとして使用									*/
/*--------------------------------------------------------------------------------------------------*/
	else													/* アブソをアブソとして使用				*/
	{
#if 0/*<S02D>*/
		MencV->Cset     = TRUE;							/* Ｃ相設定Flag							*/
		MencV->Cpass    = TRUE;							/* Ｃ相通過Flag							*/
		MencV->DivCpass = TRUE;							/* 分周Ｃ相通過Flag						*/
		MencV->OrgOk    = TRUE;							/* 原点設定完了Flag						*/
		MencV->CposOk   = FALSE;							/* Ｃ相位置設定完了Flag					*/
	/*----------------------------------------------------------------------------------------------*/
		MencV->MotOrgX  = 0;								/* モータ原点位置						*/
		MencV->MotCposX = 0;								/* モータＣ相位置						*/
		MencV->MotAngle = ((ULONG)(RcvPosX) >> MencV->MposSftR); /* モータ回転角(パルス)		*/
	/*----------------------------------------------------------------------------------------------*/
	/*	分周出力初期設定																			*/
	/*----------------------------------------------------------------------------------------------*/
		if( MencV->DivOut == TRUE )						/* モータエンコーダ分周出力選択時		*/
		{
			if( MencV->DivPass == FALSE )					/* 分周スルーモード？					*/
			{
				/* 分周Ｃ相出力選択２(INGRAM強制出力未対応) */
//				KPI_DIVSET_DCC2( );
				MencV->DivModeStatus = DIV_MODE_DCC2 ;
			}
			else
			{
				/* 通常Ｃ相出力選択(INGRAM強制出力未対応) */
//				KPI_DIVSET_DCNORMAL( );
				MencV->DivModeStatus = DIV_MODE_NORMAL ;
			}
		}
#else/*<S02D>*/
		MencV->Cset     = TRUE;								/* Ｃ相設定Flag							*/
		MencV->Cpass    = TRUE;								/* Ｃ相通過Flag							*/
		MencV->DivInfo.DivCpass = TRUE;						/* 分周Ｃ相通過Flag						*/
		MencV->OrgOk    = TRUE;								/* 原点設定完了Flag						*/
		MencV->CposOk   = FALSE;							/* Ｃ相位置設定完了Flag					*/
	/*----------------------------------------------------------------------------------------------*/
		MencV->MotOrgX  = 0;								/* モータ原点位置						*/
		MencV->MotCposX = 0;								/* モータＣ相位置						*/
		MencV->MotAngle = ((ULONG)(RcvPosX) >> MencV->MposSftR);		/* モータ回転角(パルス)		*/
	/*----------------------------------------------------------------------------------------------*/
	/*	分周出力初期設定																			*/
	/*----------------------------------------------------------------------------------------------*/
		if( MencV->DivInfo.DivOut == TRUE )					/* モータエンコーダ分周出力選択時		*/
		{
			if( MencV->DivInfo.DivPass == FALSE )			/* 分周スルーモード？					*/
			{
				/* 分周Ｃ相出力選択２ */
				AsicHwReg->AREG_DIVSET |= (BIT11 | BIT8);
				MencV->DivInfo.DivModeStatus = DIV_MODE_DCC2 ;
			}
			else
			{
				/* 通常Ｃ相出力選択 */
				AsicHwReg->AREG_DIVSET &= 0xfffff3ff;
				MencV->DivInfo.DivModeStatus = DIV_MODE_NORMAL ;
			}
		}
#endif/*<S02D>*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		エンコーダレディ設定 : エンコーダ初期化・パラメータ初期化の後で設定							*/
/*--------------------------------------------------------------------------------------------------*/
#if 0/*<S02D>*/
	if( MencV->EncConnect == FALSE )						/* エンコーダ未接続時					*/
	{
		MencV->SenReady = FALSE;							/* SEN信号処理完了Flag					*/
		MencV->IncPulseReq = FALSE;						/* 初期インクレパルス出力要求			*/
	}
	/*----------------------------------------------------------------------------------------------*/
	else if( MencV->DivOut == FALSE )					/* モータエンコーダ分周出力：非選択時	*/
	{
		MencV->SenReady = TRUE;							/* SEN信号処理完了Flag					*/
		MencV->IncPulseReq = FALSE;						/* 初期インクレパルス出力要求			*/
	}
	/*----------------------------------------------------------------------------------------------*/
	else if( MencV->AbsoEncType )						/* アブソエンコーダ時					*/
	{
		/* シリアル出力を行わないため、初期化時にTRUEとする。										*/
		MencV->SenReady = TRUE;							/* SEN信号処理完了Flag					*/
		MencV->IncPulseReq = TRUE;						/* 初期インクレパルス出力要求			*/
	}
	/*----------------------------------------------------------------------------------------------*/
	else													/* インクレエンコーダ時					*/
	{
		MencV->SenReady = TRUE;							/* SEN信号処理完了Flag					*/
		MencV->IncPulseReq = FALSE;						/* 初期インクレパルス出力要求			*/
	}
#else/*<S02D>*/
	if( MencV->EncConnect == FALSE )						/* エンコーダ未接続時					*/
	{
		MencV->SenReady = FALSE;							/* SEN信号処理完了Flag					*/
		MencV->DivInfo.IncPulseReq = FALSE;					/* 初期インクレパルス出力要求			*/
	}
	/*----------------------------------------------------------------------------------------------*/
	else if( MencV->DivInfo.DivOut == FALSE )				/* モータエンコーダ分周出力：非選択時	*/
	{
		MencV->SenReady = TRUE;								/* SEN信号処理完了Flag					*/
		MencV->DivInfo.IncPulseReq = FALSE;					/* 初期インクレパルス出力要求			*/
	}
	/*----------------------------------------------------------------------------------------------*/
	else if( MencV->AbsoEncType )							/* アブソエンコーダ時					*/
	{
		/* シリアル出力を行わないため、初期化時にTRUEとする。										*/
		MencV->SenReady = TRUE;								/* SEN信号処理完了Flag					*/
		MencV->DivInfo.IncPulseReq = TRUE;					/* 初期インクレパルス出力要求			*/
	}
	/*----------------------------------------------------------------------------------------------*/
	else													/* インクレエンコーダ時					*/
	{
		MencV->SenReady = TRUE;								/* SEN信号処理完了Flag					*/
		MencV->DivInfo.IncPulseReq = FALSE;					/* 初期インクレパルス出力要求			*/
	}

#endif/*<S02D>*/
/*--------------------------------------------------------------------------------------------------*/
/*		エンコーダフラグ関連：共通設定																*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->MotCphPass		= FALSE;					/* Ｃ相通過Flag							*/
	MencV->IncreCposLatch	= FALSE;					/* Ｃ相位置ラッチ信号(ScanA-->ScanB)	*/
	/*----------------------------------------------------------------------------------------------*/
	MencV->PhaseReady		= TRUE;						/* 位相検出完了Flag						*/
//	MencV->EncDisable		= FALSE;					/* エンコーダディスエーブルFlag			*/
	MencV->AccChkWaitCnt		= 0;						/* 加速度検出開始カウントクリア			*/
	MencV->AccChkEnable		= FALSE;					/* 加速度チェックFlag					*/
	MencV->SpdCmpEnable		= FALSE;					/* 速度補正処理有効Flag					*/
//	MencV->DivRatFix			= FALSE;					/* 最大速度、分周比関係					*/
	MencV->DivInfo.DivRatFix = FALSE;						/* 最大速度、分周比関係					*//*<S02D>*/
	MencV->MpfPhaseOffset	= 0;						/* 磁極情報の初期値設定		<V251>		*/
	/*----------------------------------------------------------------------------------------------*/
	MencV->PaoseqNormal = FALSE;							/* PAO出力Seq 初回処理設定				*/
/*--------------------------------------------------------------------------------------------------*/
/*		機能有無選択：インクレパルス出力要求リセット												*/
/*--------------------------------------------------------------------------------------------------*/
#if 0/*<S02D>*/
	if( SVFSEL_INCPOUT == 0 )
	{
		MencV->IncPulseReq = FALSE;						/* 初期インクレパルス出力要求			*/
	}
#else/*<S02D>*/
	if( SVFSEL_INCPOUT == 0 )
	{
		MencV->DivInfo.IncPulseReq = FALSE;						/* 初期インクレパルス出力要求			*/
	}
	else
	{
		if( MencV->DivInfo.DivOut == TRUE) /* && (Kprm.f.MotorLessTestMode==FALSE) )*/
		{
			MencV->DivInfo.IncPulseReq = TRUE;					/* 初期インクレパルス出力要求			*/
		}
	}
#endif/*<S02D>*/
/*--------------------------------------------------------------------------------------------------*/
/*		回転速度データの初期値設定																	*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->MotSpd = 0;									/* Motor Speed [2^24/OvrSpd]			*/

	return SENCERR_SUCCESS;
}



/****************************************************************************************************/
/*																									*/
/*		リニアモータ：シリアルエンコーダ変数初期化処理												*/
/*																									*/
/****************************************************************************************************/
static SENC_ERROR	SencInitVariablesForLinear( MENCV *MencV, MENCPRM *MencP,
												MENC_REF_PRM *MencIPrm, ASIC_HW_IF_STR *AsicHwReg )
{
	LONG		idx;
	LONG		lwk;
	LONG		RcvPosX;
	SENC_ERROR	err;

	err = SENCERR_SUCCESS;
/*--------------------------------------------------------------------------------------------------*/
/*		初回位置データ(今回,前回,前々回)の設定														*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->MotPos  = 0;									/* Motor Position	[32bitRing]			*/
	MencV->MotCposX = 0;									/* Motor CphasePos.	[32bitRing]			*/
	/*----------------------------------------------------------------------------------------------*/
	MencV->RxPosH[1]  = MencV->RxPosH[0];				/* 前回インクレ補正値初期設定			*/
	/* LposToXpos */
	RcvPosX = MencV->MposSign * ( (MencV->RxPosL[0].Long >> LMX_MPOS_SFT) << LMX_MPOS_SFT );
	MencV->RcvPosX[0] = MencV->RcvPosX[1] = MencV->RcvPosX[2] = RcvPosX;
	MencV->MotPosX[0] = MencV->MotPosX[1] = RcvPosX;

/*--------------------------------------------------------------------------------------------------*/
/*		初回絶対位置データ(絶対位置36Bitデータの下位32Bitデータを取り出す)							*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->AbsoEncoder == TRUE )
	{
		MencV->MotAbsPos = (MencV->RxPosH[0]<<(32-LMX_MPOS_SFT)) | ((ULONG)MencV->RxPosL[0].dw>>LMX_MPOS_SFT);
		MencV->MotAbsPos =  MencV->MposSign * MencV->MotAbsPos;
	}
	else
	{
		MencV->MotAbsPos = 0;
	}
/*--------------------------------------------------------------------------------------------------*/
/*		位相角初期値		0 ≦ MencV.MotPhasX < MencP.PulseNo										*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->MotPhaX = MencV->MotAbsPos % MencV->PulseNo;	/* Motor PhasePos [PuleseNo/360deg]	*/
	if( MencV->MotPhaX < 0 )
	{
		MencV->MotPhaX += MencV->PulseNo;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		初回マルチターンデータの設定																*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->IniMturn = 0;
	MencV->IniIncre = RcvPosX >> LMX_MPOS_SFT;			/* XposToUpos							*/

/*--------------------------------------------------------------------------------------------------*/
/*		位相原点の初期設定：ポールセンサ有り														*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->withPoleSensor )							/* ポールセンサ有り						*/
	{
	/*----------------------------------------------------------------------------------------------*/
	/*	初期モータ位相原点演算：ＵＶＷ信号より演算													*/
	/*----------------------------------------------------------------------------------------------*/
		if( MencV->Sigma3Encoder )
		{
			lwk = MencV->RxPosH[0] >> 1;
		}
		else
		{
			lwk = MencV->RxPosL[0].w.l >> 9;
		}
		idx = MencV->MotPoleInfo[0] = MencV->MotPoleInfo[1] = (UCHAR)(lwk & 0x07);
#if (FLOAT_USE==TRUE)
		lwk = (LONG)((float)PhaseTableUVW[idx] * MencV->Kinvphase );
#else
		lwk = MlibMulgainNolim( PhaseTableUVW[idx], MencV->Kinvphase );
#endif /* FLOAT_USE */

	/*----------------------------------------------------------------------------------------------*/
		if( MencV->PhaseOrderUWV == FALSE )				/* ＵＶＷ位相順							*/
		{
			/* MotOrgX [PulseNo/360deg] */
			MencV->MotOrgX = MencV->MotPhaX - lwk;
			/* ホールセンサオフセット量 */
			MencV->PhaseOffset = (MencP->poleoffset<<16) / 360;
		}
		else												/* ＵＷＶ位相順							*/
		{
			/* MotOrgX [PulseNo/360deg] */
			MencV->MotOrgX = MencV->MotPhaX + lwk;
			/* ホールセンサオフセット量*/
			MencV->PhaseOffset = ((360-MencP->poleoffset)<<16) / 360;
		}
		MencV->PhaseReady = TRUE;						/* 位相検出完了Flag						*/
	/*----------------------------------------------------------------------------------------------*/
	/*	ポールセンサ異常チェック：オールＬｏまたはオールＨｉは異常									*/
	/*----------------------------------------------------------------------------------------------*/
		if( (MencV->MotPoleInfo[1]==0) || (MencV->MotPoleInfo[1]==7) )
		{
			err = SENCERR_POLESENSOR;
		}
		MencV->MpfPhaseOffset = 0;							/* 磁極情報の初期値設定				*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		位相原点の初期設定：ポールセンサ無し														*/
/*--------------------------------------------------------------------------------------------------*/
	else													/* ポールセンサ無し						*/
	{
		MencV->MotOrgX = 0;									/* モータ原点						*/
		MencV->PhaseOffset = 0;								/* ホールセンサオフセット量 		*/
		MencV->MotPoleInfo[0] = MencV->MotPoleInfo[1] = 0;/* ホールセンサ情報					*/
		if( MencV->AbsoEncType == MENC_INCRE_TYPE )			/* インクレ使用時					*/
		{
			MencV->PhaseReady = FALSE;						/* 位相検出完了Flag					*/
			MencV->MpfPhaseOffset = 0;						/* 磁極情報の初期値設定				*/
		}
		else													/* アブソ使用時						*/
		{
			/* 位相検出完了Flag */
			MencV->PhaseReady = (MencIPrm->syssw3 & 0x0020) ? TRUE : FALSE;
			MencV->MpfPhaseOffset = MencIPrm->phsofst;		/* 磁極情報の初期値(PnE8D)設定		*/
			/*--------------------------------------------------------------------------------------*/
			/* スケール側EEPROMとアンプ側EEPROMの位相情報不一致検出									*/
			/*--------------------------------------------------------------------------------------*/
			if( MencIPrm->phsofst != MencP->phscmpM2 )			/* PnF1DとPnE8Dの比較				*/
			{
				err = SENCERR_PHASEANGLE;
				MencV->PhaseReady = FALSE;
			}
		}
	}

/*--------------------------------------------------------------------------------------------------*/
/*		モータ原点チェック用変数の初期化															*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->MotOrgChkEnd = FALSE;
	MencV->MotOrgChkCnt   = 0;
	MencV->MotOrgXchk[0]  = MencV->MotOrgXchk[1] = MencV->MotOrgX;

/* 2009.09.24 Y.Oka 分周パルス出力機能非対応としたい。 */
/*--------------------------------------------------------------------------------------------------*/
/*		分周パルス初期設定																			*/
/*--------------------------------------------------------------------------------------------------*/
//	MencV->DivOutRem = 0;
//	MencV->DivOutPos = MlibPfbkxremNolim( MencV->IniIncre, MencV->DivOutGain, &MencV->DivOutRem );
//	MencV->InitDivPosOffset = MencV->DivOutPos ;
#if 0 /*<S02D>*/
/*--------------------------------------------------------------------------------------------------*/
/*		Ｃ相フラグ関連：インクレエンコーダ															*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->AbsoEncoder == FALSE )
	{
		MencV->Cset = FALSE;								/* Ｃ相設定Flag							*/
		MencV->CposOk = FALSE;							/* Ｃ相位置設定完了Flag					*/
	/*----------------------------------------------------------------------------------------------*/
	/*	分周出力初期設定																			*/
	/*----------------------------------------------------------------------------------------------*/
		if( MencV->DivOut == TRUE )
		{
			/* パルス変換Ｃ相マスクＯＮ				*/
//			KPI_PCMASK_ON( );
			AsicHwReg->AREG_WDT1SET |= BIT8;
			MencV->DivCMaskStatus = DIV_CMASK_ON;

			/* 通常Ｃ相出力選択(INGRAM強制出力未対応) */
//			KPI_DIVSET_DCNORMAL( );
			MencV->DivModeStatus = DIV_MODE_NORMAL;

			AsicHwReg->AREG_POSET0 = (USHORT)MencV->DivOutPos;
			AsicHwReg->AREG_POTEST = (USHORT)MencV->DivOutPos;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		Ｃ相フラグ関連：絶対値エンコーダ (アブソをインクレとして使用)								*/
/*--------------------------------------------------------------------------------------------------*/
	else if( MencV->AbsoEncType == MENC_INCRE_TYPE )
	{
		MencV->Cset = TRUE;								/* Ｃ相設定Flag							*/
		MencV->CposOk = FALSE;							/* Ｃ相位置設定完了Flag					*/
	/*----------------------------------------------------------------------------------------------*/
	/*	分周出力初期設定																			*/
	/*----------------------------------------------------------------------------------------------*/
		if( MencV->DivOut == TRUE )
		{
			/* パルス変換Ｃ相マスクＯＮ				*/
//			KPI_PCMASK_ON( );
			AsicHwReg->AREG_WDT1SET |= BIT8;
			MencV->DivCMaskStatus = DIV_CMASK_ON;

			/* 通常Ｃ相出力選択						*/
//			KPI_DIVSET_DCNORMAL( );
			MencV->DivModeStatus = DIV_MODE_NORMAL;

			AsicHwReg->AREG_POSET0 = (USHORT)MencV->DivOutPos;
			AsicHwReg->AREG_POTEST = (USHORT)MencV->DivOutPos;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		Ｃ相フラグ関連：絶対値エンコーダ (アブソをアブソとして使用)									*/
/*--------------------------------------------------------------------------------------------------*/
	else
	{
		MencV->Cset = TRUE;								/* Ｃ相設定Flag							*/
		MencV->CposOk = FALSE;							/* Ｃ相位置設定完了Flag					*/
	/*----------------------------------------------------------------------------------------------*/
	/*	分周出力初期設定																			*/
	/*----------------------------------------------------------------------------------------------*/
		if( MencV->DivOut == TRUE )
		{
			/* パルス変換Ｃ相マスクＯＮ				*/
//			KPI_PCMASK_ON( );
			AsicHwReg->AREG_WDT1SET |= BIT8;
			MencV->DivCMaskStatus = DIV_CMASK_ON;

			/* 通常Ｃ相出力選択(INGRAM強制出力未対応) */
//			KPI_DIVSET_DCNORMAL( );
			MencV->DivModeStatus = DIV_MODE_NORMAL;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		エンコーダレディ設定 : エンコーダ初期化・パラメータ初期化の後で設定							*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->EncConnect == FALSE )						/* エンコーダ未接続時					*/
	{
		MencV->SenReady = FALSE;							/* SEN信号処理完了Flag					*/
		MencV->IncPulseReq = FALSE;						/* 初期インクレパルス出力要求			*/
	}
	/*----------------------------------------------------------------------------------------------*/
	else if( MencV->DivOut == FALSE )					/* モータエンコーダ分周出力：非選択時	*/
	{
		MencV->SenReady = TRUE;							/* SEN信号処理完了Flag					*/
		MencV->IncPulseReq = FALSE;						/* 初期インクレパルス出力要求			*/
	}
	/*----------------------------------------------------------------------------------------------*/
	else if( MencV->AbsoEncType )						/* アブソエンコーダ時					*/
	{
//		MencV->SenReady = FALSE;							/* SEN信号処理完了Flag					*/
		MencV->SenReady = TRUE;								/* SEN信号処理完了Flag					*/
		MencV->IncPulseReq = TRUE;						/* 初期インクレパルス出力要求			*/
	}
	/*----------------------------------------------------------------------------------------------*/
	else													/* インクレエンコーダ時					*/
	{
		MencV->SenReady = TRUE;							/* SEN信号処理完了Flag					*/
		MencV->IncPulseReq = FALSE;						/* 初期インクレパルス出力要求			*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		エンコーダフラグ設定 : 共通処理																*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->MotCphPass		= FALSE;					/* Ｃ相通過Flag							*/
	MencV->IncreCposLatch	= FALSE;					/* Ｃ相位置ラッチ信号(ScanA-->ScanB)	*/
//	MencV->EncDisable		= FALSE;					/* エンコーダディスエーブルFlag			*/
	MencV->AccChkEnable		= FALSE;					/* 加速度チェックFlag					*/
	MencV->SpdCmpEnable		= FALSE;					/* 速度補正処理有効Flag					*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->PaoseqNormal		= FALSE;					/* PAO出力Seq 初回処理設定				*/

/* 2009.09.24 Y.Oka 初期インクレパルス出力なし */
/*--------------------------------------------------------------------------------------------------*/
/*		機能有無選択：インクレパルス出力要求リセット												*/
/*--------------------------------------------------------------------------------------------------*/
//	if( SVFSEL_INCPOUT == 0 )
//	{
//		MencV->IncPulseReq = FALSE;						/* 初期インクレパルス出力要求			*/
//	}
#else/*<S02D>*/
/* <S21B>:Start */
	MencV->DivInfo.DivOutRem = 0;
	MencV->DivInfo.DivOutPos = MlibPfbkxremNolim( MencV->IniIncre, MencV->DivInfo.DivOutGain, &MencV->DivInfo.DivOutRem );
	MencV->DivInfo.InitDivPosOffset = MencV->DivInfo.DivOutPos ;
/* <S21B>:end */
/*--------------------------------------------------------------------------------------------------*/
/*		Ｃ相フラグ関連：インクレエンコーダ															*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->AbsoEncoder == FALSE )
	{
		MencV->Cset = FALSE;								/* Ｃ相設定Flag							*/
		MencV->CposOk = FALSE;								/* Ｃ相位置設定完了Flag					*/
	/*----------------------------------------------------------------------------------------------*/
	/*	分周出力初期設定																			*/
	/*----------------------------------------------------------------------------------------------*/
		if( MencV->DivInfo.DivOut == TRUE )
		{
			/* パルス変換Ｃ相マスクＯＮ				*/
			AsicHwReg->AREG_WDT1SET |= BIT8;
			MencV->DivInfo.DivCMaskStatus = DIV_CMASK_ON;

			/* 通常Ｃ相出力選択(INGRAM強制出力未対応) */
			MencV->DivInfo.DivModeStatus = DIV_MODE_NORMAL;
			AsicHwReg->AREG_POSET0 = (ULONG)MencV->DivInfo.DivOutPos;
			AsicHwReg->AREG_POTEST = (ULONG)MencV->DivInfo.DivOutPos;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		Ｃ相フラグ関連：絶対値エンコーダ (アブソをインクレとして使用)								*/
/*--------------------------------------------------------------------------------------------------*/
	else if( MencV->AbsoEncType == MENC_INCRE_TYPE )
	{
		MencV->Cset = TRUE;								/* Ｃ相設定Flag							*/
		MencV->CposOk = FALSE;							/* Ｃ相位置設定完了Flag					*/
	/*----------------------------------------------------------------------------------------------*/
	/*	分周出力初期設定																			*/
	/*----------------------------------------------------------------------------------------------*/
		if( MencV->DivInfo.DivOut == TRUE )
		{
			/* パルス変換Ｃ相マスクＯＮ				*/
			AsicHwReg->AREG_WDT1SET |= BIT8;
			MencV->DivInfo.DivCMaskStatus = DIV_CMASK_ON;

			/* 通常Ｃ相出力選択						*/
			MencV->DivInfo.DivModeStatus = DIV_MODE_NORMAL;
			AsicHwReg->AREG_POSET0 = (ULONG)MencV->DivInfo.DivOutPos;
			AsicHwReg->AREG_POTEST = (ULONG)MencV->DivInfo.DivOutPos;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		Ｃ相フラグ関連：絶対値エンコーダ (アブソをアブソとして使用)									*/
/*--------------------------------------------------------------------------------------------------*/
	else
	{
		MencV->Cset = TRUE;								/* Ｃ相設定Flag							*/
		MencV->CposOk = FALSE;							/* Ｃ相位置設定完了Flag					*/
	/*----------------------------------------------------------------------------------------------*/
	/*	分周出力初期設定																			*/
	/*----------------------------------------------------------------------------------------------*/
		if( MencV->DivInfo.DivOut == TRUE )
		{
			/* パルス変換Ｃ相マスクＯＮ				*/
			AsicHwReg->AREG_WDT1SET |= BIT8;
			MencV->DivInfo.DivCMaskStatus = DIV_CMASK_ON;

			/* 通常Ｃ相出力選択(INGRAM強制出力未対応) */
			MencV->DivInfo.DivModeStatus = DIV_MODE_NORMAL;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		エンコーダレディ設定 : エンコーダ初期化・パラメータ初期化の後で設定							*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->EncConnect == FALSE )						/* エンコーダ未接続時					*/
	{
		MencV->SenReady = FALSE;							/* SEN信号処理完了Flag					*/
		MencV->DivInfo.IncPulseReq = FALSE;					/* 初期インクレパルス出力要求			*/
	}
	/*----------------------------------------------------------------------------------------------*/
	else if( MencV->DivInfo.DivOut == FALSE )				/* モータエンコーダ分周出力：非選択時	*/
	{
		MencV->SenReady = TRUE;								/* SEN信号処理完了Flag					*/
		MencV->DivInfo.IncPulseReq = FALSE;					/* 初期インクレパルス出力要求			*/
	}
	/*----------------------------------------------------------------------------------------------*/
	else if( MencV->AbsoEncType )							/* アブソエンコーダ時					*/
	{
		MencV->SenReady = TRUE;								/* SEN信号処理完了Flag					*/
		MencV->DivInfo.IncPulseReq = TRUE;					/* 初期インクレパルス出力要求			*/
	}
	/*----------------------------------------------------------------------------------------------*/
	else													/* インクレエンコーダ時					*/
	{
		MencV->SenReady = TRUE;								/* SEN信号処理完了Flag					*/
		MencV->DivInfo.IncPulseReq = FALSE;					/* 初期インクレパルス出力要求			*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		エンコーダフラグ設定 : 共通処理																*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->MotCphPass		= FALSE;						/* Ｃ相通過Flag							*/
	MencV->IncreCposLatch	= FALSE;						/* Ｃ相位置ラッチ信号(ScanA-->ScanB)	*/
	MencV->AccChkEnable		= FALSE;						/* 加速度チェックFlag					*/
	MencV->SpdCmpEnable		= FALSE;						/* 速度補正処理有効Flag					*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->PaoseqNormal		= FALSE;						/* PAO出力Seq 初回処理設定				*/

	if( SVFSEL_INCPOUT == 0 )
	{
		MencV->DivInfo.IncPulseReq = FALSE;					/* 初期インクレパルス出力要求			*/
	}
	else
	{
		MencV->DivInfo.IncPulseReq = TRUE;					/* 初期インクレパルス出力要求			*/
	}
#endif/*<S02D>*/

/*--------------------------------------------------------------------------------------------------*/
/*		回転速度データの初期値設定																	*/
/*--------------------------------------------------------------------------------------------------*/
	MencV->MotSpd = 0;									/* Motor Speed [2^24/OvrSpd]			*/

	return err;
}



/****************************************************************************************************/
/*																									*/
/*		シリアルエンコーダの変数初期化処理															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : イニシャル処理において、シリアルエンコーダの初回位置データを読み込み、					*/
/*			位置データ、前回位置の設定を行う。														*/
/*			また、エンコーダタイプによってＣ相出力、初期パルスを出さない設定をする。				*/
/*																									*/
/****************************************************************************************************/
SENC_ERROR	SencInitMencVariables(MENCV *MencV, MENCPRM *MencP, MENC_REF_PRM *MencInPrm )
{
	SENC_ERROR		err, rc_err;

	/*----------------------------------------------------------------------------------------------*/
	/*		シリアルエンコーダからのデータ読み込み													*/
	/*----------------------------------------------------------------------------------------------*/
	rc_err = SencReadInitialP1Data(MencV);

	/*----------------------------------------------------------------------------------------------*/
	/*		シリアルエンコーダ変数初期化処理, 初期絶対位置([pulse]＆[指令単位])計算					*/
	/*----------------------------------------------------------------------------------------------*/
	if( MencV->AxisMotType == MOTTYPE_LINEAR )
	{ /* リニア型 */

		/* 初期パラメータ計算 */
		SencInitParamCalcForLinear( MencV, MencP, MencInPrm );

		/* 変数初期化処理 */
		err = SencInitVariablesForLinear( MencV, MencP, MencInPrm, MencV->EncIf.SencSetReg );/* <S04D> 引数変更 */
		if(rc_err == SENCERR_SUCCESS)
		{	rc_err = err;}
	}
	else
	{ /* 回転型 */

		/* 初期パラメータ計算 */
		SencInitParamCalcForRotary( MencV, MencP, MencInPrm );

		/* 変数初期化処理 */
		err = SencInitVariablesForRotary( MencV, MencP, MencV->EncIf.SencSetReg );/* <S04D> 引数変更 */
		if(rc_err == SENCERR_SUCCESS)
		{	rc_err = err;}
	}

	return rc_err;
}


#pragma arm section code = "CALL_SCANA" /* <S1D2> */
//#if	(SVFSEL_DIVOUT_USE == TRUE)  /* <S02D> *//* <S09F> */
/****************************************************************************************************/
/*																									*/
/*		初期インクレパルス出力処理																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : シリアルエンコーダＰＡＯ相シリアル出力シーケンス(SencOutputPaoSerial())					*/
/*			からの指令に従い、アブソエンコーダ接続に、ＳＥＮ信号立ち上がり時に出力する				*/
/*			マルチターンデータ取得と初期インクレパルス出力処理を行う。								*/
/*																									*/
/*																									*/
/*	補 足 : サーボオフ時の処理であるので、処理時間的には多少余裕がある。							*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#define MAXPULSEOUT		170
/*--------------------------------------------------------------------------------------------------*/
void	 SencOutIniIncPulse( MENCV  *MencV, ASIC_HW_IF_STR  *pAsicHwReg )
{
LONG	lwk;
LONG	MtnCur;
LONG	MtnSub;
LONG	MtnSepos;
#if	(SVFSEL_DIVOUT_USE == TRUE)       /* <S09F> */
/*--------------------------------------------------------------------------------------------------*/
/*		通信異常チェック																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( MencV->SeRxErr == TRUE )
		{
			return;											/* 通信異常時は、何もしない				*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		マルチターンデータ取得処理																	*/
/*--------------------------------------------------------------------------------------------------*/
		if( MencV->DivInfo.PaoSeqMcmd == KPX_MTLATCH_REQUEST )	/* ラッチ要求コマンドチェック		*/
		{
			MencV->DivInfo.MturnLatch = SencGetMturnData( MencV,
			                                      MencV->DivInfo.Limmlt, 
			                                      MencV->DivInfo.Bit_dp,
												  (USHORT)MencV->RxPosH[0],
												  MencV->RxPosL[0].dw );
			MencV->DivInfo.IncPulseOut = 0;				/* インクレパルス出力リセット			*/
			pAsicHwReg->AREG_WDT1SET &= ~BIT8;				/* パルス変換C相マスクOFF設定			*/
			MencV->DivInfo.DivCMaskStatus  = DIV_CMASK_OFF ;
			MencV->DivInfo.PaoSeqMcmd = KPX_MTLATCH_FINISH;/* 応答 ==> SencOutputPaoSerial()		*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		初期インクレパルス出力処理																	*/
/*--------------------------------------------------------------------------------------------------*/
		else if( MencV->DivInfo.PaoSeqMcmd == KPX_IPLSOUT_REQUEST )/* パルス出力コマンドチェック	*/
		{
		/*------------------------------------------------------------------------------------------*/
		/*		初期インクレパルス出力データ演算													*/
		/*------------------------------------------------------------------------------------------*/
			MtnCur = SencGetMturnData( MencV,
                                       MencV->DivInfo.Limmlt, 
                                       MencV->DivInfo.Bit_dp,
									   (USHORT)MencV->RxPosH[0],
									   MencV->RxPosL[0].dw );

			MtnSub = SencCheckMturnLimit( MtnCur, MencV );
			MtnSepos = ((ULONG)MencV->RcvPosX[0]>>MencV->DivInfo.DivOutSft) + (MtnSub<<(32-MencV->DivInfo.DivOutSft));
		/*------------------------------------------------------------------------------------------*/
		/*		初期インクレパルス出力データチェック＆出力データ設定								*/
		/*------------------------------------------------------------------------------------------*/
			lwk = MtnSepos - MencV->DivInfo.IncPulseOut;
			if( lwk > MAXPULSEOUT )							/* パルス出力上限						*/
			{
				MencV->DivInfo.IncPulseOut += MAXPULSEOUT;	/* パルス出力データ設定					*/
			}
			else if( lwk < -MAXPULSEOUT )					/* パルス出力下限						*/
			{
				MencV->DivInfo.IncPulseOut -= MAXPULSEOUT;	/* パルス出力データ設定					*/
			}
			else											/* 最終パルス出力						*/
			{
				MencV->DivInfo.IncPulseOut = MtnSepos;		/* パルス出力データ設定					*/
				MencV->DivInfo.PaoSeqMcmd = KPX_IPLSOUT_FINISH;/* 応答 ==> SencOutputPaoSerial		*/
				MencV->DivInfo.IncPulseReq = FALSE;			/* 初期インクレパルス出力完了			*/
			}
		/*------------------------------------------------------------------------------------------*/
			pAsicHwReg->AREG_POSET0	= MencV->DivInfo.IncPulseOut;	/* パルス出力回路初期化			*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		コマンド無し																				*/
/*--------------------------------------------------------------------------------------------------*/
		else
		{
			;												/* コマンド待ち							*/
		}
/*--------------------------------------------------------------------------------------------------*/
#endif                           /* <S09F> */
		return;

}
#pragma arm section code

#pragma arm section code = "CALL_SCANA"
/****************************************************************************************************/
/*																									*/
/*		リニアモータ(フィードバックオプション)：初期インクレパルス出力処理							*/
/*																									*/
/****************************************************************************************************//* <S209> */
void	LmxOutSencIniIncPulse( MENCV  *MencV, ASIC_HW_IF_STR  *pAsicHwReg, UCHAR Sftx )
{
LONG	lwk;
LONG	MtnCur;
LONG	MtnSub;
LONG	MtnRem;
LONG	MtnSepos;
SHORT	MturnLatchCur;

#if	(SVFSEL_DIVOUT_USE == TRUE)
/*--------------------------------------------------------------------------------------------------*/
/*		通信異常チェック																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( MencV->SeRxErr == TRUE )
		{
			return;											/* 通信異常時は、何もしない				*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		マルチターンデータ取得処理																	*/
/*--------------------------------------------------------------------------------------------------*/
		if( MencV->DivInfo.PaoSeqMcmd == KPX_MTLATCH_REQUEST )	/* ラッチ要求コマンドチェック		*/
		{
		/*------------------------------------------------------------------------------------------*/
		/*		絶対位置36Bitデータの上位32Bitデータを取り出し、マルチターンの分周値を計算する		*/
		/*------------------------------------------------------------------------------------------*/
			MtnCur    = ((USHORT)MencV->RxPosH[0] << 16 ) | ((USHORT)MencV->RxPosL[0].w.h );
			MtnRem    = 0;
			MtnCur    = MlibPfbkxremNolim( MtnCur, MencV->DivInfo.DivOutGain, &MtnRem );
			MtnSepos = (MencV->RxPosH[0]<<(32-Sftx)) | ((ULONG)MencV->RxPosL[0].dw>>Sftx);
			MencV->DivInfo.DivOutRem = 0;														
			MtnSepos =  MlibPfbkxremNolim( MtnSepos, MencV->DivInfo.DivOutGain, &MencV->DivInfo.DivOutRem );
			if( ( (MtnCur>>15) & 0x00000001 ) ^ ( (MtnSepos>>19) & 0x00000001 ) )
			{
				if( (MtnCur>>15) & 0x00000001 )
				{
					MtnCur = MtnCur + 1;
				}
				else if( (MtnSepos>>19) & 0x00000001 )
				{
					MtnCur = MtnCur - 1;
				}
			}
			MencV->DivInfo.MturnLatch = (SHORT)( MtnCur >> 16);
		/*------------------------------------------------------------------------------------------*/
		/*		分周パルスリセット処理																*/
		/*------------------------------------------------------------------------------------------*/
			MencV->DivInfo.DivOutPos   = 0;					/* 分周出力パルスリセット				*/
			MencV->DivInfo.IncPulseOut = 0;					/* インクレパルス出力リセット			*/
			MencV->DivInfo.PaoSeqMcmd  = KPX_MTLATCH_FINISH;/* 応答 ==> LpxOutputSencPaoSerial()	*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		初期インクレパルス出力処理																	*/
/*--------------------------------------------------------------------------------------------------*/
		else if( MencV->DivInfo.PaoSeqMcmd == KPX_IPLSOUT_REQUEST )	/* パルス出力コマンドチェック	*/
		{
		/*------------------------------------------------------------------------------------------*/
		/*		マルチターンデータの変化分演算														*/
		/*		絶対位置36Bitデータの下位32Bitデータを取り出し、初期インクレの分周値を計算する		*/
		/*------------------------------------------------------------------------------------------*/
			MtnCur        = ((USHORT)MencV->RxPosH[0] << 16 ) | ((USHORT)MencV->RxPosL[0].w.h );
			MtnRem        = 0;
			MtnCur        = MlibPfbkxremNolim( MtnCur, MencV->DivInfo.DivOutGain, &MtnRem );
			MtnSepos = (MencV->RxPosH[0]<<(32-Sftx)) | ((ULONG)MencV->RxPosL[0].dw>>Sftx);
			MencV->DivInfo.DivOutRem   = 0;						/* 分周出力パルス余りリセット		*/
			MtnSepos =  MlibPfbkxremNolim( MtnSepos, MencV->DivInfo.DivOutGain, &MencV->DivInfo.DivOutRem );
			if( ( (MtnCur>>15) & 0x00000001 ) ^ ( (MtnSepos>>19) & 0x00000001 ) )
			{
				if( (MtnCur>>15) & 0x00000001 )
				{
					MtnCur = MtnCur + 1;
				}
				else if( (MtnSepos>>19) & 0x00000001 )
				{
					MtnCur = MtnCur - 1;
				}
			}
			MturnLatchCur = (SHORT)( MtnCur >> 16);
			MtnSub = (USHORT)MturnLatchCur - (USHORT)MencV->DivInfo.MturnLatch;
			MtnSepos = (MtnSepos & 0x000FFFFF );
			if( MencV->RevAsmMotor ^ MencV->DivInfo.RvsDir )/* 反転接続 XOR 逆回転モード			*/
			{
				MtnSepos = MtnSepos | 0xFFF00000;			/* 符号反転処理							*/
				MtnSepos = (ULONG)MtnSepos - ( MtnSub << (32-Sftx) );
			}
			else
			{
				MtnSepos = (ULONG)MtnSepos + ( MtnSub << (32-Sftx) );
			}
		/*------------------------------------------------------------------------------------------*/
		/*		初期インクレパルス出力データチェック＆出力データ設定								*/
		/*------------------------------------------------------------------------------------------*/
			lwk = MtnSepos - MencV->DivInfo.IncPulseOut;
			if( lwk > MAXPULSEOUT )							/* パルス出力上限						*/
			{
				MencV->DivInfo.IncPulseOut += MAXPULSEOUT;	/* パルス出力データ設定					*/
			}
			else if( lwk < -MAXPULSEOUT )					/* パルス出力下限						*/
			{
				MencV->DivInfo.IncPulseOut -= MAXPULSEOUT;	/* パルス出力データ設定					*/
			}
			else											/* 最終パルス出力						*/
			{
				MencV->DivInfo.IncPulseOut = MtnSepos;		/* パルス出力データ設定					*/
				MencV->DivInfo.PaoSeqMcmd = KPX_IPLSOUT_FINISH;/* 応答 ==> SencOutputPaoSerial		*/
				MencV->DivInfo.IncPulseReq = FALSE;			/* 初期インクレパルス出力完了			*/
			}
			MencV->DivInfo.DivOutPos = MencV->DivInfo.IncPulseOut;	/* 分周出力パルス更新			*/
		/*------------------------------------------------------------------------------------------*/
			pAsicHwReg->AREG_POSET0	= MencV->DivInfo.IncPulseOut;	/* インクレパルス出力設定		*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		コマンド無し																				*/
/*--------------------------------------------------------------------------------------------------*/
		else
		{
			;												/* コマンド待ち							*/
		}
/*--------------------------------------------------------------------------------------------------*/
#endif 
		return;
}
#pragma arm section code

#pragma arm section code = "CALL_SCANA" /* <S1D2> */
#if	(SVFSEL_DIVOUT_USE == TRUE)       /* <S09F> */
/****************************************************************************************************/
/*																									*/
/*		マルチターンデータの変化分演算		<S02D>													*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : パルス出力時の位置データと多回転ラッチ時の位置データから変化分を求める。				*/
/*			パルス出力データについては、マルチターンリミット値を超えているかどうかの				*/
/*			チェックを行い、パルス出力データに反映する。											*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static LONG	 SencCheckMturnLimit( LONG MtnCur, MENCV *MencV )
{
LONG	MtnSub;
LONG	limmlt = MencV->DivInfo.Limmlt;

/*--------------------------------------------------------------------------------------------------*/
/*		マルチターン変化量演算																		*/
/*--------------------------------------------------------------------------------------------------*/
		MtnSub = (USHORT)MtnCur - (USHORT)MencV->DivInfo.MturnLatch;
		if( MtnSub < 0 )									/* 負の場合								*/
		{
			if( MtnSub < -((limmlt>>1) + 1) )				/* マルチターンリミットを超えた場合,	*/
			{												/* マルチターンリミットを超えた分の 	*/
				MtnSub += (limmlt + 1);						/* 多回転量を加算						*/
			}
		}
		else												/* 正の場合								*/
		{
			if ( MtnSub > (limmlt>>1) )						/* マルチターンリミットを超えた場合, 	*/
			{												/* マルチターンリミットを超えた分の		*/
				MtnSub -= (limmlt + 1);						/* 多回転量を加算						*/
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		return ( MtnSub );
}
#pragma arm section code


/****************************************************************************************************/
/*																									*/
/*		シリアルエンコーダＰＡＯ相シリアル出力			<S02D>										*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : アブソエンコーダ使用時、ＳＥＮ信号によるマルチターンデータ及びアラームの				*/
/*			ＰＡＯからのシリアル出力処理を行う。													*/
/*																									*/
/*			1) 出力フォーマット : ７bit 偶数パリティ												*/
/*				 ＡＳＩＣのＴＸＤは、パリティの付加及び７ビットデータ長送信の機能が					*/
/*				 ないので、パリティをソフト的に付加した８ビットデータとして送信する					*/
/*																									*/
/*			2) マルチターン : Ｐ±＠＠＠＠＠[CR]	( ±@@@@@ : -32768 - +32768 )					*/
/*																									*/
/*			3) アラーム		: ＡＬＭ＠＠．[CR]		( @@ : A.@@ )									*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
void	SencOutputPaoSerial( MENCV *MencV, BPRMDAT *Bprm, ASIC_HW_IF_STR *pAsicHwReg,
							     BOOL DbBeReq, BOOL SenReq, USHORT AlmCode, BOOL AlmFlag )
{

	USHORT	uswk;
	ASIC_HW_IF_STR *pAsicHwReg2;/*<S0C2>*/
	pAsicHwReg2 = (ASIC_HW_IF_STR*)((UCHAR*)pAsicHwReg + 0x1000);/*2-axis pointing address <S0C2>*/
/*--------------------------------------------------------------------------------------------------*/
/*		機能有無チェック																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( SVFSEL_PAOPOUT == 0 )		{ return;}				/* Mask Function					*/
/*--------------------------------------------------------------------------------------------------*/
/*		サーボオン時シーケンス初期化																*/
/*--------------------------------------------------------------------------------------------------*/
		if( DbBeReq == TRUE )
		{
			MencV->DivInfo.PaoSeqStep = 0;
			return;
		}

/*--------------------------------------------------------------------------------------------------*/
/*		ＰＡＯシリアル出力シーケンス																*/
/*--------------------------------------------------------------------------------------------------*/
		switch( MencV->DivInfo.PaoSeqStep )
		{
	/*----------------------------------------------------------------------------------------------*/
		case  0: /* Initial Step																	*/
	/*----------------------------------------------------------------------------------------------*/
				MencV->DivInfo.PaoSeqWait = 0;
				MencV->DivInfo.PaoSeqMcmd = KPX_PAOSEQ_NOCMD;
				if( MencV->PaoseqNormal == TRUE )				/* 電源投入後初回動作-				*/
				{
					MencV->DivInfo.PaoSeqStep = (SenReq == FALSE)? 1 : 20;
				}
				else
				{
					MencV->DivInfo.PaoSeqStep = 1;				/* 電源投入後初回はSEN Lo->Hiを待つ	*/
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case  1: /* Wait SEN Lo --> Hi (Output Multi-turn Data)										*/
	/*----------------------------------------------------------------------------------------------*/
				MencV->PaoseqNormal = TRUE;							/* 初回動作モード解除			*/
				MencV->SenReady = FALSE;
				MencV->DivInfo.PaoSeqMcmd = KPX_PAOSEQ_NOCMD;
				MencV->DivInfo.IncPulseReq = TRUE;
				if( SenReq == TRUE )
				{
					/*------------------------------------------------------------------------------*/
					/*		ＤＡ出力モード：ＴＸＤ出力												*/
					/*------------------------------------------------------------------------------*/
					/*		Bit13 :	DASEL(分周出力DA端子選択)	:  0 : DA,  1: TXD_1				*/
					/*------------------------------------------------------------------------------*/
					pAsicHwReg->AREG_PFNCSET  |= BIT13;			/* ＤＡ出力モード：ＴＸＤ出力		*/
					pAsicHwReg2->AREG_PFNCSET &= ~BIT13;		/*PSO NO way <S0C2>	    */
					/*------------------------------------------------------------------------------*/
					/*	回転型の場合																*/
					/*------------------------------------------------------------------------------*/
					if( MencV->AxisMotType == MOTTYPE_ROTARY )
					{
						pAsicHwReg->AREG_WDT1SET &= ~BIT8;		/* パルス変換C相マスクOFF設定		*/
						MencV->DivInfo.DivCMaskStatus  = DIV_CMASK_OFF ;

						pAsicHwReg->AREG_POSET0	= 0x00000000;	/* パルス変換ｶｳﾝﾀに0000hを強制ﾛｰﾄﾞ	*/
						pAsicHwReg->AREG_POTEST	= 0x00000000;	/* パルス変換位置を0000hに設定		*/
					}
					/*------------------------------------------------------------------------------*/
					/*	リニア型の場合：JL-076分周機能不使用(スルーモード)の場合					*/
					/*------------------------------------------------------------------------------*/
					else if( MencV->AxisMotType == MOTTYPE_LINEAR )
					{
						pAsicHwReg->AREG_POSET0	= 0xFFFFFFFF;	/* パルス出力回路初期化				*/
						pAsicHwReg->AREG_POTEST	= 0xFFFFFFFF;	/* パルス出力回路初期化				*/
						pAsicHwReg->AREG_DINCSET  |= BIT10;		/* 分周常時クリアビット立てる		*/
						pAsicHwReg->AREG_WDT1SET |= BIT8;		/* パルス変換Ｃ相マスクＯＮ			*/
						MencV->DivInfo.DivCMaskStatus = DIV_CMASK_ON;	/* パルス変換Ｃ相マスクＯＮ	*/
					}
					MencV->DivInfo.PaoSeqStep = 2;
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case  2: /* Wait 120ms																		*/
	/*----------------------------------------------------------------------------------------------*/
				/*----------------------------------------------------------------------------------*/
				/*	回転型の場合：																	*/
				/*----------------------------------------------------------------------------------*/
				if( MencV->AxisMotType == MOTTYPE_ROTARY )
				{
					/*------------------------------------------------------------------------------*/
					/*	POSET0に0000hセットから2ms後にC相再同期可セット								*/
					/*------------------------------------------------------------------------------*/
					if( MencV->DivInfo.PaoSeqWait == 0 )
					{
						pAsicHwReg->AREG_DINCSET	|= BIT5;	/* Ｃ相再同期可						*/
						pAsicHwReg->AREG_POSET0	= 0xFFFFFFFF;	/* パルス出力回路初期化				*/
					}
					if( ++MencV->DivInfo.PaoSeqWait > KPI_SCANC_MS( 120 ) )
					{
						pAsicHwReg->AREG_DINCSET	&= ~(BIT10);/* 分周常時クリア落とす				*/
						pAsicHwReg->AREG_DINCSET	&= ~(BIT5); /* Ｃ相再同期可解除					*/
						MencV->DivInfo.PaoSeqWait = 0;
						MencV->DivInfo.PaoSeqStep = 3;
					}
				}
				/*----------------------------------------------------------------------------------*/
				/*	リニア型の場合：分周機能不使用(スルーモード)の場合								*/
				/*----------------------------------------------------------------------------------*/
				else if( MencV->AxisMotType == MOTTYPE_LINEAR )
				{
					if( ++MencV->DivInfo.PaoSeqWait > KPI_SCANC_MS( 120 ) )
					{
						pAsicHwReg->AREG_DINCSET	|= BIT5;	/* Ｃ相再同期可						*/
						pAsicHwReg->AREG_DINCSET	&= ~(BIT10);/* 分周常時クリア落とす				*/
						pAsicHwReg->AREG_DINCSET	&= ~(BIT5); /* Ｃ相再同期可解除					*/
						MencV->DivInfo.PaoSeqWait = 0;
						MencV->DivInfo.PaoSeqStep = 3;
					}
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case  3: /* Multiturn Latch Request															*/
	/*----------------------------------------------------------------------------------------------*/
				MencV->DivInfo.PaoSeqMcmd = KPX_MTLATCH_REQUEST;	/* MultiturnLatch指令 ==> ScanA	*/
				MencV->DivInfo.PaoSeqStep = 4;
				break;
	/*----------------------------------------------------------------------------------------------*/
		case  4: /* Wait Multiturn Latch															*/
	/*----------------------------------------------------------------------------------------------*/
				if( MencV->DivInfo.PaoSeqMcmd == KPX_MTLATCH_FINISH )	/* MultiturnLatch完了？		*/
				{
					MencV->DivInfo.PaoSeqData = MencV->DivInfo.MturnLatch;
					MencV->DivInfo.PaoSeqStep = 5;
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case  5: /* Output 'P'																		*/
	/*----------------------------------------------------------------------------------------------*/
				/*----------------------------------------------------------------------------------*/
				/*	UARTXD:汎用非同期ポート送信データ：汎用非同期ポート送信データビット７-0   	    */
				/*----------------------------------------------------------------------------------*/
				//pAsicHwReg->AREG_UARTXD  = 'P';
				pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_P;		/* <S14F> */
				/*----------------------------------------------------------------------------------*/
				MencV->DivInfo.PaoSeqStep = 6;
				break;
	/*----------------------------------------------------------------------------------------------*/
		case  6: /* Output '+' or '-'																*/
	/*----------------------------------------------------------------------------------------------*/
				/*----------------------------------------------------------------------------------*/
				/*	回転型の場合																	*/
				/*----------------------------------------------------------------------------------*/
				if( MencV->AxisMotType == MOTTYPE_ROTARY )
				{
					if( (MencV->DivInfo.Limmlt != 65535) || ((SHORT)MencV->DivInfo.PaoSeqData >= 0) )
					{
						//pAsicHwReg->AREG_UARTXD  = '+';
						pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_Puls;		/* <S14F> */
					}
					else
					{
						//pAsicHwReg->AREG_UARTXD  = '-';
						pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_Mins;		/* <S14F> */
						MencV->DivInfo.PaoSeqData = -MencV->DivInfo.PaoSeqData;
					}
				}
				/*----------------------------------------------------------------------------------*/
				/*	リニア型の場合																	*/
				/*----------------------------------------------------------------------------------*/
				else if( MencV->AxisMotType == MOTTYPE_LINEAR )
				{
					/*------------------------------------------------------------------------------*/
					/*	逆回転モード																*/
					/*------------------------------------------------------------------------------*/
					if( MencV->RevAsmMotor ^ Bprm->RvsDir )			/* 反転接続 XOR 逆回転モード	*/
					{
						if( (SHORT)MencV->DivInfo.PaoSeqData >= 0 )
						{
							//pAsicHwReg->AREG_UARTXD  = '-';
							pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_Mins;		/* <S14F> */
							MencV->DivInfo.PaoSeqData = MencV->DivInfo.PaoSeqData + 1;
						}
						else
						{
							//pAsicHwReg->AREG_UARTXD  = '+';
							pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_Puls;		/* <S14F> */
							MencV->DivInfo.PaoSeqData = ~MencV->DivInfo.PaoSeqData;
						}
					}
					/*------------------------------------------------------------------------------*/
					/*	正回転モード																*/
					/*------------------------------------------------------------------------------*/
					else
					{
						if( (SHORT)MencV->DivInfo.PaoSeqData >= 0 )
						{
							//pAsicHwReg->AREG_UARTXD  = '+';
							pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_Puls;		/* <S14F> */
						}
						else
						{
							//pAsicHwReg->AREG_UARTXD  = '-';
							pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_Mins;		/* <S14F> */
							MencV->DivInfo.PaoSeqData = ~MencV->DivInfo.PaoSeqData + 1;
						}
					}
				}
				MencV->DivInfo.PaoSeqStep = 7;
				break;
	/*----------------------------------------------------------------------------------------------*/
	/*			１０進データ出力 : ５桁目,４桁目,３桁目,２桁目,１桁目								*/
	/*----------------------------------------------------------------------------------------------*/
		case  7: SencOutputPaoMtdata( MencV, pAsicHwReg, 5 ); MencV->DivInfo.PaoSeqStep =  8; break;
		case  8: SencOutputPaoMtdata( MencV, pAsicHwReg, 4 ); MencV->DivInfo.PaoSeqStep =  9; break;
		case  9: SencOutputPaoMtdata( MencV, pAsicHwReg, 3 ); MencV->DivInfo.PaoSeqStep = 10; break;
		case 10: SencOutputPaoMtdata( MencV, pAsicHwReg, 2 ); MencV->DivInfo.PaoSeqStep = 11; break;
		case 11: SencOutputPaoMtdata( MencV, pAsicHwReg, 1 ); MencV->DivInfo.PaoSeqStep = 12; break;
	/*----------------------------------------------------------------------------------------------*/
		case 12: /* Output '0x0D'																	*/
	/*----------------------------------------------------------------------------------------------*/
				//pAsicHwReg->AREG_UARTXD  = (0x0d + 0x80);
				pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_Cr;		/* <S14F> */
				MencV->DivInfo.PaoSeqStep = 13;
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 13: /* Wait 2ms																		*/
	/*----------------------------------------------------------------------------------------------*/
				MencV->DivInfo.PaoSeqStep = 14;
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 14: /* IncPulse Output Request															*/
	/*----------------------------------------------------------------------------------------------*/
				/*----------------------------------------------------------------------------------*/
				/*		ＤＡ出力モード：パルス出力										   		    */
				/*----------------------------------------------------------------------------------*/
				/*		Bit13 :	DASEL(分周出力DA端子選択)	:  0 : DA,  1: TXD_1				    */
				/*----------------------------------------------------------------------------------*/
				pAsicHwReg->AREG_PFNCSET  &= ~BIT13;			/* ＤＡ出力モード：パルス出力		*/
				/*----------------------------------------------------------------------------------*/
				MencV->DivInfo.PaoSeqMcmd = KPX_IPLSOUT_REQUEST;/* 初期インクレ出力指令 ==> ScanA	*/
				MencV->DivInfo.PaoSeqStep = 15;
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 15: /* Check IncPulse Output Complete													*/
	/*----------------------------------------------------------------------------------------------*/
				if( MencV->DivInfo.PaoSeqMcmd == KPX_IPLSOUT_FINISH )	/* 初期インクレ出力完了？	*/
				{
					/*------------------------------------------------------------------------------*/
					/*	回転型の場合																*/
					/*------------------------------------------------------------------------------*/
					if( MencV->AxisMotType == MOTTYPE_ROTARY )
					{
						if( MencV->EncDisable == FALSE )
						{
							MencV->SenReady = TRUE;
							MencV->ReCalcAbsPosReq = TRUE;		/* 絶対位置再計算要求セット			*/
							SencMakeAbsPosition( MencV, 		/* 多回転量及び1回転内パルス数の取得*/
												 MencV->DivInfo.Limmlt,
												 MencV->DivInfo.Bit_dp,
												 Bprm->RvsDir );
						}
					}
					/*------------------------------------------------------------------------------*/
					/*	リニア型の場合																*/
					/*------------------------------------------------------------------------------*/
					else if( MencV->AxisMotType == MOTTYPE_LINEAR )
					{
						if( MencV->EncDisable == FALSE )
						{
							MencV->SenReady = TRUE;
							LmxSencGetLinearAbsPos( MencV, Bprm->RvsDir );/* 絶対位置データの取得	*/
						}
						pAsicHwReg->AREG_WDT1SET &= ~BIT8;		/* ﾊﾟﾙｽ変換Ｃ相ﾏｽｸＯＦＦ  			*/
						MencV->DivInfo.DivCMaskStatus  = DIV_CMASK_OFF ;
					}
					/*------------------------------------------------------------------------------*/
					MencV->DivInfo.PaoSeqStep = 0;
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 20: /* Wait SEN Hi --> Lo (Output Alarm)												*/
	/*----------------------------------------------------------------------------------------------*/
				if( SenReq == FALSE )
				{
					MencV->SenReady = FALSE;
					MencV->DivInfo.PaoSeqMcmd = KPX_PAOSEQ_NOCMD;
					MencV->DivInfo.IncPulseReq = TRUE;
					if( AlmFlag )								/* アラーム発生中					*/
					{
						pAsicHwReg->AREG_PFNCSET  |= BIT13;		/* ＤＡ出力モード：ＴＸＤ出力		*/
						pAsicHwReg->AREG_POSET0	= 0xFFFFFFFF;	/* パルス出力回路初期化				*/
						pAsicHwReg->AREG_POTEST	= 0xFFFFFFFF;	/* パルス出力回路初期化				*/

						MencV->DivInfo.PaoSeqData = AlmCode;	/* アラームコード取得				*/
						MencV->DivInfo.PaoSeqStep = 21;			/* アラーム出力ステップに遷移		*/
					}
					else										/* アラーム発生なし					*/
					{
						MencV->DivInfo.PaoSeqStep =  1;			/* Lo-->Hi監視ステップに遷移		*/
					}
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 21: /* Wait 120ms																		*/
	/*----------------------------------------------------------------------------------------------*/
				if( SenReq == FALSE )
				{
					if( ++MencV->DivInfo.PaoSeqWait > KPI_SCANC_MS( 120 ) )
					{
						MencV->DivInfo.PaoSeqWait = 0;
						MencV->DivInfo.PaoSeqStep = 22;
					}
				}
				else
				{
					MencV->DivInfo.PaoSeqWait = 0;
					MencV->DivInfo.PaoSeqStep = 1;
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 22: /* Output 'A'																		*/
	/*----------------------------------------------------------------------------------------------*/
				if( SenReq == FALSE )
				{
					//pAsicHwReg->AREG_UARTXD  = 'A';
					pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_A;		/* <S14F> */
					MencV->DivInfo.PaoSeqStep = 23;
				}
				else
				{
					MencV->DivInfo.PaoSeqStep = 1;
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 23: /* Output 'L'																		*/
	/*----------------------------------------------------------------------------------------------*/
				if( SenReq == FALSE )
				{
					//pAsicHwReg->AREG_UARTXD  = ('L' + 0x80);
					pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_L;		/* <S14F> */
					MencV->DivInfo.PaoSeqStep = 24;
				}
				else
				{
					MencV->DivInfo.PaoSeqStep = 1;
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 24: /* Output 'M'																		*/
	/*----------------------------------------------------------------------------------------------*/
				if( SenReq == FALSE )
				{
					//pAsicHwReg->AREG_UARTXD  = 'M';
					pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_M;		/* <S14F> */
					MencV->DivInfo.PaoSeqStep = 25;
				}
				else
				{
					MencV->DivInfo.PaoSeqStep = 1;
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 25: /* Output Alarm Code																*/
	/*----------------------------------------------------------------------------------------------*/
				if( SenReq == FALSE )
				{
					uswk = MlibEvenAsciiTbl[((MencV->DivInfo.PaoSeqData >> 8) & 0x0F)];
					//pAsicHwReg->AREG_UARTXD  = uswk;
					pAsicHwReg->AREG_UARTXD  = MlibEvenAsciiTbl[ uswk ];		/* <S14F> */
					MencV->DivInfo.PaoSeqStep = 26;
				}
				else
				{
					MencV->DivInfo.PaoSeqStep = 1;
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 26: /* Output Alarm Code																*/
	/*----------------------------------------------------------------------------------------------*/
				if( SenReq == FALSE )
				{
					uswk = MlibEvenAsciiTbl[((MencV->DivInfo.PaoSeqData>> 4) & 0x0F)];
					//pAsicHwReg->AREG_UARTXD  = uswk;
					pAsicHwReg->AREG_UARTXD  = MlibEvenAsciiTbl[ uswk ];		/* <S14F> */
					MencV->DivInfo.PaoSeqStep = 27;
				}
				else
				{
					MencV->DivInfo.PaoSeqStep = 1;
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 27: /* Output '.'																		*/
	/*----------------------------------------------------------------------------------------------*/
				if( SenReq == FALSE )
				{
					//pAsicHwReg->AREG_UARTXD  = '.';
					pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_Prio;		/* <S14F> */
					MencV->DivInfo.PaoSeqStep = 28;
				}
				else
				{
					MencV->DivInfo.PaoSeqStep = 1;
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 28: /* Output 0x0D																		*/
	/*----------------------------------------------------------------------------------------------*/
				if( SenReq == FALSE )
				{
					//pAsicHwReg->AREG_UARTXD  = (0x0d + 0x80);
					pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_Cr;		/* <S14F> */
					MencV->DivInfo.PaoSeqStep = 29;
				}
				else
				{
					MencV->DivInfo.PaoSeqStep = 1;
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 29: /* End of Output Alarm																*/
	/*----------------------------------------------------------------------------------------------*/
				pAsicHwReg->AREG_PFNCSET  &= ~BIT13;			/* ＤＡ出力モード：パルス出力		*/
				MencV->DivInfo.PaoSeqStep = 1;
				break;
	/*----------------------------------------------------------------------------------------------*/
		default: /* Default																			*/
	/*----------------------------------------------------------------------------------------------*/
				MencV->DivInfo.PaoSeqStep = 0;
				break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;

}

/****************************************************************************************************/
/*																									*/
/*		ＰＡＯ相マルチターンデータ出力						<S02D>									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : マルチターンデータを指定桁で割った商をアスキー文字に変換して指定ポートへ出力し、		*/
/*			その桁を引いた残りを保存します。指定桁がデータの最上位である事が必要です。				*/
/*																									*/
/*			例）*data = 32767 の ５桁目(digit=5)を出力する場合										*/
/*																									*/
/*				出力データ = '3'																	*/
/*				保存データ = 2767																	*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static void SencOutputPaoMtdata( MENCV *MencV, ASIC_HW_IF_STR *pAsicHwReg, ULONG digit )
{
ULONG	x;
ULONG	outx;

/*--------------------------------------------------------------------------------------------------*/
/*		桁データ変換																				*/
/*--------------------------------------------------------------------------------------------------*/
		switch( digit )
		{
		case  1: digit = 1;     break;
		case  2: digit = 10;    break;
		case  3: digit = 100;   break;
		case  4: digit = 1000;  break;
		case  5: digit = 10000; break;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		マルチターンデータ出力																		*/
/*--------------------------------------------------------------------------------------------------*/
		x = MencV->DivInfo.PaoSeqData / digit;
		outx = MlibEvenAsciiTbl[ x ];
		pAsicHwReg->AREG_UARTXD  = (USHORT)outx;
		MencV->DivInfo.PaoSeqData -= (USHORT)(x * digit);
/*--------------------------------------------------------------------------------------------------*/
		return;

}

/****************************************************************************************************/
/*																									*/
/*		リニアモータ：絶対位置データ(64bit)取得														*/
/*																									*/
/****************************************************************************************************/
static void LmxSencGetLinearAbsPos( MENCV  *MencV, BOOL RvsDir )
{
ULONG	PosL;
USHORT	PosH;
LONG	AbsPos[2];
UCHAR	Sftx = LMX_MPOS_SFT;

/*--------------------------------------------------------------------------------------------------*/
/*		エンコーダデータラッチ																		*/
/*--------------------------------------------------------------------------------------------------*/
		PosH = MencV->RxPosH[0];						/* スキャンＡで読込んだデータを使用して、	*/
		PosL = MencV->RxPosL[0].dw;						/* 絶対位置(36Bit)を計算する。				*/

		AbsPos[1] = (LONG)((SHORT)PosH) >> Sftx;		
		AbsPos[0] = (PosH<<(32-Sftx))+((ULONG)PosL>>Sftx);

		/*------------------------------------------------------------------------------------------*/
		if( MencV->RevAsmMotor ^ RvsDir )				/* 反転接続 XOR 逆回転モード				*/
		{
			AbsPos[1] = (~AbsPos[1]);					/* 符号反転処理								*/
			AbsPos[0] = (~AbsPos[0]) + 1;				/* 符号反転処理								*/
			if( AbsPos[0] == 0 ){ ++AbsPos[1];}			/* Carryの場合は、上位に+1					*/
		}

		MencV->SenOnScalePosH = AbsPos[1];
		MencV->SenOnScalePosL = AbsPos[0];
/*--------------------------------------------------------------------------------------------------*/
		return;
}

#if	(CSW_SVCF_FULC_MODE != CSW_FULC_NO_SUPPORT)
//以下のフルク関連処理は暫定であり、フルク専用ファイルへ移動する必要がある　<S02D>
/****************************************************************************************************/
/*																									*/
/*		フルクエンコーダ ：分周出力パルス周波数上限速度の計算							<S02D>		*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 分周出力パルスが最大周波数となる回転速度を算出する。									*/
/*																									*/
/*			パルス周波数上限(SVCDEF_DIVOUT_MAXFREQ) :												*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*			MaxFreq [pps] = pgrate [pulse/rev] * DivOvrSpd [rev/s]									*/
/*																									*/
/*									       MaxFreq * 2^24											*/
/*			DivOvrSpd[2^24/OvrSpd] = ----------------------------	 								*/
/*									  pgrate  * (OvrSpd / 2*PAI) 									*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	DIVSET_ERROR FencSencCalMencDivOverSpeed( MENCV *FencV, UNI_PRM_STR *UniPrms )
{
#if (FLOAT_USE==TRUE)
	float	fw;
#else
	LONG	kx, sx;
#endif /* FLOAT_USE */
LONG	pgrate;													/* [pulse/rev]						*/
LONG	i;
LONG	BitNo = UniPrms->Prm->MencP.BitnoDpoint.b.l;
LONG	MaxFreq = SVCDEF_DIVOUT_MAXFREQ;						/* [pulse/sec]						*/
LONG	pulsno = 1;
DIVSET_ERROR	err;

/*--------------------------------------------------------------------------------------------------*/
/*		分周出力パルスレートの計算 [pulse/rev]														*/
/*--------------------------------------------------------------------------------------------------*/
		pgrate = (((FencV->PulseNo >> BitNo) * UniPrms->Prm->scalepgrat) >> 2) - 1;
/*--------------------------------------------------------------------------------------------------*/
/*		分周上限速度基準値算出																		*/
/*--------------------------------------------------------------------------------------------------*/
		for ( i = 0; pgrate > 0; i++ )
		{
			pgrate = pgrate / 2;
			pulsno = pulsno * 2;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		分周出力パルス周波数上限速度の計算															*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
		fw = (float)MaxFreq * (float)0x01000000 / (float)pulsno;
		fw = fw / OvrSpd;
		fw = fw * 62832.0F / 10000.0F;
		if( fw > (float)0x01000000 )
		{
			FencV->DivInfo.DivOvrSpd = 0x01000000;
		}
		else
		{
			FencV->DivInfo.DivOvrSpd = (LONG)fw;
		}
#else
		kx = MlibScalKxgain( MaxFreq, 0x01000000, pulsno, &sx,   0 );
		kx = MlibPcalKxkxks( kx,      1,     UniPrms->Bprm->OvrSpd, &sx,   0 );
		kx = MlibPcalKxgain( kx,      62832,      10000,  &sx, -24 );
		FencV->DivInfo.DivOvrSpd = kx;
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		パラメータ設定異常																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( FencV->DivInfo.DivOvrSpd == 0 )
		{
			err = DIVSET_PRMERR_L;								/* A.040 : パラメータエラーの設定	*/
		}
/*--------------------------------------------------------------------------------------------------*/
	return( err );
}

/****************************************************************************************************/
/*																									*/
/*		フルクエンコーダ : 分周出力パルス計算 ＆ ＡＳＩＣ設定					<S02D>				*/
/*																									*/
/****************************************************************************************************/
static DIVSET_ERROR	FencSencSetMencDividedPulse( ASIC_HW_IF_STR *AsicHwReg, MENCV *FencV, UNI_PRM_STR *UniPrms )
{
LONG	x;
LONG	BitNo = UniPrms->Prm->MencP.BitnoDpoint.b.l;
LONG	pulsno;
LONG	pgrate = UniPrms->Prm->scalepgrat;
DIVSET_ERROR	err;

/*--------------------------------------------------------------------------------------------------*/
/*		分周出力パルス上限値設定																	*/
/*--------------------------------------------------------------------------------------------------*/
		if( BitNo > 0 )
		{
			pulsno = (1<<BitNo);
		}
		else
		{
			pulsno = pndef_scalepgrat.UpperLimit;
		}

/*--------------------------------------------------------------------------------------------------*/
/*		分周出力パルス周波数上限速度の計算															*/
/*--------------------------------------------------------------------------------------------------*/
		err = FencSencCalMencDivOverSpeed( FencV, UniPrms );

/*--------------------------------------------------------------------------------------------------*/
/*		ＰＧ分周比設定範囲チェック																	*/
/*--------------------------------------------------------------------------------------------------*/
		if( (pgrate > pulsno) || (pgrate < 1) )				/* pgrate : 範囲チェック				*/
		{
			return ( DIVSET_DIVERR );						/* A.041 : 分周パルス出力設定異常		*/
		}

/*--------------------------------------------------------------------------------------------------*/
/*		分周パルス出力ゲインの演算																	*/
/*--------------------------------------------------------------------------------------------------*/
		FencV->DivInfo.DivMode = FALSE;
		FencV->DivInfo.DivPass = TRUE;
		FencV->DivInfo.DivRate = 0;
		FencV->DivInfo.DivOutGain = MlibScalKxgain( pgrate, 1, (1 << BitNo), NULL, 24 );
		FencV->DivInfo.DivOutCmskChk = MlibScalKxkxks( 0x1000, 1, FencV->DivInfo.DivOutGain, NULL, -30 );
/*--------------------------------------------------------------------------------------------------*/
/*		ＡＳＩＣ分周機能設定																		*/
/*--------------------------------------------------------------------------------------------------*/
		x = (FencV->DivInfo.DivMode << 4) | FencV->DivInfo.DivPass | 0x0400;
		AsicHwReg->AREG_DINCSET	= (ULONG)x;						     /* 分周機能設定				*/
		AsicHwReg->AREG_DIVCSEV	= (ULONG)(FencV->DivInfo.DivRate);  /* 分周比設定					*/
		AsicHwReg->AREG_DCTRIM  = (ULONG)FencV->DivInfo.DivRate - 1;/* 分周出力原点補正			*/
		if( UniPrms->Bprm->RvsDir )
		{
			AsicHwReg->AREG_DINCSET |= BIT2;						/* DB逆転モード設定				*/
		}
/*--------------------------------------------------------------------------------------------------*/
		return (err);

}

/****************************************************************************************************/
/*																									*/
/*		フルクエンコーダ : 分周パルスＩＦ初期化														*/
/*																									*/
/****************************************************************************************************/
DIVSET_ERROR FencSencInitDivPulseIF( ASIC_HW_IF_STR* AsicHwReg, MENCV *FencV, UNI_PRM_STR *UniPrms )
{
	DIVSET_ERROR	err;

/*--------------------------------------------------------------------------------------------------*/
/*		シリアルエンコーダパルス変換設定(Bit数,変換周期) ＠JL-056相当の設定＠						*/
/*--------------------------------------------------------------------------------------------------*/
	AsicHwReg->AREG_PLSET	= 0x00003400;				/* 変換Bit数:16bit, 変換周期:64us			*/
/*--------------------------------------------------------------------------------------------------*/
/*		シリアルエンコーダパルス変換設定(位置,原点,etc)												*/
/*--------------------------------------------------------------------------------------------------*/
	AsicHwReg->AREG_WDT1SET	|=	BIT8;
	FencV->DivInfo.DivCMaskStatus = DIV_CMASK_ON;
/*--------------------------------------------------------------------------------------------------*/
/*	DIVSET:分周機能設定																				*/
/*--------------------------------------------------------------------------------------------------*/
/*	VULONG	AREG_DIVSET;		 0x0164 分周機能設定												*/
								/*------------------------------------------------------------------*/
								/*  bit.1-0 :ﾃﾞｰﾀﾊﾟﾙｽ変換位置ﾃﾞｰﾀ書込み選択						*/
								/*            0：ｼﾘｱﾙPGIFﾁｬﾝﾈﾙ０から (初期値)						*/
								/*  		  1：ｼﾘｱﾙPGIFﾁｬﾝﾈﾙ１から								*/
								/*  		  2：ﾎｽﾄCPUから											*/
								/*  		  3：内蔵演算部から										*/
								/*------------------------------------------------------------------*/
								/*  bit.2 : 分周ｸﾛｯｸ選択											*/
								/*            0：26.7MHz (初期値)									*/
								/*  		  1：32MHz												*/
								/*------------------------------------------------------------------*/
								/*  bit.4-3 :パルス変換原点補正値設定元選択						*/
								/*            0：ｼﾘｱﾙPGIFﾁｬﾝﾈﾙ０から (初期値)						*/
								/*  		  1：ｼﾘｱﾙPGIFﾁｬﾝﾈﾙ１から								*/
								/*  		  2：ﾎｽﾄCPUから											*/
								/*  		  3：内蔵演算部から										*/
								/*------------------------------------------------------------------*/
								/*  bit.8 : DIVSEL0													*/
								/*            0：PA,PB,PC				 							*/
								/*  		  1：ﾃﾞｰﾀﾊﾟﾙｽ変換出力（初期値）							*/
								/*------------------------------------------------------------------*/
								/*  bit.11-10 :DCSEL												*/
								/*            0：通常C相 (初期値)									*/
								/*  		  1：分周C相											*/
								/*  		  2：分周C相2											*/
								/*  		  3：分周C相2											*/
								/*------------------------------------------------------------------*/
								/*  bit.13 : DIVASEL												*/
								/*            0：DA出力 (初期値)									*/
								/*  		  1：ｼﾘｱﾙﾃﾞｰﾀｽﾙｰ出力									*/
								/*------------------------------------------------------------------*/
								/*  bit.15 : DIVCBIT												*/
								/*            0：DC出力 (初期値)									*/
								/*  		  1：DCﾊｲﾚﾍﾞﾙ強制出力									*/
/*--------------------------------------------------------------------------------------------------*/
	AsicHwReg->AREG_DIVSET	= 0x00000112;  				/* ﾊﾟﾙｽ変換書込み側設定:ﾎｽﾄCPU,				*/
/*--------------------------------------------------------------------------------------------------*/
	AsicHwReg->AREG_POSET0	= 0xFFFFFFFF;				/* パルス変換位置設定						*/
	AsicHwReg->AREG_POSET1	= 0x00000000;				/* パルス変換原点補正１						*/
	AsicHwReg->AREG_POSET2	= 0xFFFFFFFF;				/* パルス変換原点補正２						*/
	AsicHwReg->AREG_POTEST	= 0xFFFFFFFF;				/* パルス変換カウンタ設定					*/
/*--------------------------------------------------------------------------------------------------*/
/*		分周入力フィルタ設定																		*/
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/*	FBCSET:ﾌｨｰﾄﾞﾊﾞｯｸﾊﾟﾙｽｶｳﾝﾀ																		*/
/*--------------------------------------------------------------------------------------------------*/
/*	VULONG	AREG_FBCSET;	     0x0040 分周入力フィルタ設定										*/
								/*------------------------------------------------------------------*/
								/*  bit.1-0 :PA,PBﾌｨﾙﾀ選択											*/
								/*            0：26.7MHz同期 (初期値)								*/
								/*  		  1：37.5ns												*/
								/*  		  2：75ns												*/
								/*  		  3：素通し												*/
								/*------------------------------------------------------------------*/
								/*  bit.3-2 : PCﾌｨﾙﾀ選択											*/
								/*            0：26.7MHz同期 (初期値)								*/
								/*  		  1：37.5ns												*/
								/*  		  2：75ns												*/
								/*  		  3：素通し												*/
								/*------------------------------------------------------------------*/
								/*  bit.5-15 : 省略													*/
/*--------------------------------------------------------------------------------------------------*/
	AsicHwReg->AREG_FBCSET	= 0x00000005;				/* 分周入力フィルタ設定						*/
/*--------------------------------------------------------------------------------------------------*/
/*		分周パルス計算＆ＡＳＩＣ設定																*/
/*--------------------------------------------------------------------------------------------------*/
	err = FencSencSetMencDividedPulse( AsicHwReg, FencV, UniPrms );
/*--------------------------------------------------------------------------------------------------*/

	return( err );
}
#endif	/*(CSW_SVCF_FULC_MODE != CSW_FULC_NO_SUPPORT)*/
#endif	/*(SVFSEL_DIVOUT_USE == TRUE)*/




/*<S189>_Start*//*<S0C2>*/
#define   PS0_Use   /*PS0 Use*/
#ifdef    PS0_Use
/*********************************************************************************/
/*Explanation: This is a function for PSO data output , used by Full Closed loop*/
/*********************************************************************************/
void FencSencOutputPsoSerial(
								MENCV*           FencV,      /*Incremental */
                                ASIC_HW_IF_STR*  pAsicHwReg, /*Register AREG_UARTXD  Selection*/
                                SHORT limmlt,    /*Pn205 Set*/
                                BOOL  RvsDir     /*逆回転モード判断*/
                             )
{
		ASIC_HW_IF_STR *pAsicHwReg2;
		pAsicHwReg2 = (ASIC_HW_IF_STR*)((UCHAR*)pAsicHwReg + 0x1000);/*2-axis pointing address*/
		if(FencV->SenReady == FALSE)  /*waiting for the SenReady time*/
		{
			FencV->DivInfo.PsoSeqStep = 0;
			return;
		}
		if(FencV->EncReady==FALSE)/* if Encoder Ready==FALSE Retrun*/
		{
			FencV->DivInfo.PsoSeqStep = 0;
			return;
		}
	  switch( FencV->DivInfo.PsoSeqStep )/*Step One by One*/
	  {
		case  0: /* Initial Step*/
				FencV->DivInfo.PsoSeqWait = 0;
				FencV->DivInfo.PsoSeqStep = 1;
		break;
		case  1: /* Get  Multiturn  and Incremental Data*/
				 FencV->DivInfo.PsoLinearData_H=  (USHORT)FencV->RxPosH[0];
				 FencV->DivInfo.PsoLinearData_L = ((ULONG)FencV->RxPosL[0].dw)>>FencV->MposSftR;/*Linear Value*/
				 FencV->DivInfo.PsoSeqStep = 2;
		break;
		case  2: /*Output 'P'*/
				pAsicHwReg->AREG_PFNCSET &= ~BIT13; /*Output Data Set DA2*/
				pAsicHwReg2->AREG_PFNCSET|= BIT13;  /*Output Data Set PSO  Register Selection*/
				pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_P;		/* <S14F> P*/
				FencV->DivInfo.PsoSeqStep = 3;
		break;
		case 3 : /* Output '+' or '-'*/
				/*----------------------------------------------------------------------------------*/
				/*	逆回転モード																	    */
				/*----------------------------------------------------------------------------------*/
				if( FencV->DivInfo.RvsDir )
				{
					if( (SHORT)FencV->DivInfo.PsoMturnLatch >= 0 )
					{
						pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_Mins;		/* <S14F>  -*/
					}
					else
					{
						pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_Puls;		/* <S14F>  +*/
					}
				}
				/*----------------------------------------------------------------------------------*/
				/*	正回転モード																	*/
				/*----------------------------------------------------------------------------------*/
				else
				{
					if( (SHORT)FencV->DivInfo.PsoMturnLatch >= 0 )
					{
						pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_Puls;		/* <S14F>  +*/
					}
					else
					{
						pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_Mins;		/* <S14F>  -*/
					}
				}
				FencV->DivInfo.PsoSeqStep = 4;
		break;
/* <S20A>：引数見直し：[MencV->DivInfo.PsoLinearData_*]をポインタに */
		case 4:  OutputPsoLinearIncre((LONG*)&FencV->DivInfo.PsoLinearData_H, 5, pAsicHwReg); FencV->DivInfo.PsoSeqStep=  5; break;
		case 5:  OutputPsoLinearIncre((LONG*)&FencV->DivInfo.PsoLinearData_H, 4, pAsicHwReg); FencV->DivInfo.PsoSeqStep=  6; break;
		case 6:  OutputPsoLinearIncre((LONG*)&FencV->DivInfo.PsoLinearData_H, 3, pAsicHwReg); FencV->DivInfo.PsoSeqStep=  7; break;
		case 7:  OutputPsoLinearIncre((LONG*)&FencV->DivInfo.PsoLinearData_H, 2, pAsicHwReg); FencV->DivInfo.PsoSeqStep=  8; break;
		case 8:  OutputPsoLinearIncre((LONG*)&FencV->DivInfo.PsoLinearData_H, 1, pAsicHwReg); FencV->DivInfo.PsoSeqStep=  9; break;
		case 9:  OutputPsoLinearIncre((LONG*)&FencV->DivInfo.PsoLinearData_L, 6, pAsicHwReg); FencV->DivInfo.PsoSeqStep=  10; break;
		case 10: OutputPsoLinearIncre((LONG*)&FencV->DivInfo.PsoLinearData_L, 5, pAsicHwReg); FencV->DivInfo.PsoSeqStep=  11; break;
		case 11: OutputPsoLinearIncre((LONG*)&FencV->DivInfo.PsoLinearData_L, 4, pAsicHwReg); FencV->DivInfo.PsoSeqStep=  12; break;
		case 12: OutputPsoLinearIncre((LONG*)&FencV->DivInfo.PsoLinearData_L, 3, pAsicHwReg); FencV->DivInfo.PsoSeqStep=  13; break;
		case 13: OutputPsoLinearIncre((LONG*)&FencV->DivInfo.PsoLinearData_L, 2, pAsicHwReg); FencV->DivInfo.PsoSeqStep=  14; break;
		case 14: OutputPsoLinearIncre((LONG*)&FencV->DivInfo.PsoLinearData_L, 1, pAsicHwReg); FencV->DivInfo.PsoSeqStep=  15; break;
/* <S20A>：ここまで */
		case 15: /* Output 0x0D   CR*/
				 pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_Cr;		/* <S14F> 0x0d*/
				 FencV->DivInfo.PsoSeqStep = 16;break;
		case 16: /* Wait 2ms*/
				 FencV->DivInfo.PsoSeqStep = 17;break;
		case 17: /* Wait 2ms*/
				 FencV->DivInfo.PsoSeqStep = 18;break;
		case 18: /* Wait 2ms*/
				FencV->DivInfo.PsoSeqStep = 19;break;
		case 19: /* Wait 2ms*/
				FencV->DivInfo.PsoSeqStep = 20;break;
		case 20: /* End Step*/
				FencV->DivInfo.PsoSeqStep = 1;break;
		default: /* Default Step*/
				FencV->DivInfo.PsoSeqStep = 0;break;
 }/* End if MencV->AxisMotType == MOTTYPE_LINEAR*/
}
/*********************************************************************************/
/*Explanation: This is a function for PSO data output , used by Motor Closed loop*/
/*********************************************************************************/
void SencOutputPsoSerial(MENCV* MencV,               /*Multiturn  and Incremental step*/
		                 ASIC_HW_IF_STR* pAsicHwReg, /*Register AREG_UARTXD  Selection*/
		                 SHORT limmlt,                /*Pn205 Set  マルチターンリミット*/
		                 BOOL  RvsDir)                /*逆回転モード判断します*/
{
	ASIC_HW_IF_STR *pAsicHwReg2;
	pAsicHwReg2 = (ASIC_HW_IF_STR*)((UCHAR*)pAsicHwReg + 0x1000);/*2-axis pointing address*/
	if(MencV->SenReady==FALSE)  /*waiting for the SenReady time*/
	{
		MencV->DivInfo.PsoSeqStep = 0;
		return;
	}
	if(MencV->EncReady==FALSE)/* if Encoder Ready==FALSE Retrun*/
	{
		MencV->DivInfo.PsoSeqStep = 0;
		return;
	}
	/* MOTTYPE_ROTARY */
   if(MencV->AxisMotType == MOTTYPE_ROTARY)
   {

	   switch( MencV->DivInfo.PsoSeqStep )/*Step One by One*/
		{
			case  0: /* Initial Step*/
					MencV->DivInfo.PsoSeqWait = 0;
					MencV->DivInfo.PsoSeqStep = 1;
			break;
			case  1: /* Get  Multiturn  and Incremental Data	*/
					MencV->DivInfo.PsoMturnLatch= SencGetMturnData(MencV,MencV->DivInfo.Limmlt, MencV->DivInfo.Bit_dp,(USHORT)MencV->RxPosH[0],MencV->RxPosL[0].dw );
					MencV->DivInfo.PsoIncPulseOut = ((ULONG)MencV->RcvPosX[0])>>MencV->MposSftR;/*Determine the decimal point position*/
					MencV->DivInfo.PsoSeqStep = 2;
			break;
			case  2: /*Output 'P'*/
				    pAsicHwReg->AREG_PFNCSET &= ~BIT13; /*Output Data Set DA2*/
				    pAsicHwReg2->AREG_PFNCSET|= BIT13;  /*Output Data Set PSO Register Selection*/
				    /*SGDV未搭載処理、Σ7追加レジスタ処理*/
					pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_P;		/* <S14F> P*/
					MencV->DivInfo.PsoSeqStep = 3;
			break;
			case 3 : /* Output '+' or '-'*/
					if( (limmlt != 65535)/*Pn205 Set*/
						|| ((SHORT)MencV->DivInfo.PsoMturnLatch >= 0) )
					{
						pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_Puls;/* <S14F> + */
					}
					else
					{
						pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_Mins;/* <S14F>  - */
						MencV->DivInfo.PsoMturnLatch = -MencV->DivInfo.PsoMturnLatch;
					}
					MencV->DivInfo.PsoSeqStep = 4;
			break;
		    /*多回転データ*/
			case  4: OutputPsoMturnIncre(MencV, 5, 0,pAsicHwReg); MencV->DivInfo.PsoSeqStep =  5; break;
			case  5: OutputPsoMturnIncre(MencV, 4, 0,pAsicHwReg); MencV->DivInfo.PsoSeqStep =  6; break;
			case  6: OutputPsoMturnIncre(MencV, 3, 0,pAsicHwReg); MencV->DivInfo.PsoSeqStep =  7; break;
			case  7: OutputPsoMturnIncre(MencV, 2, 0,pAsicHwReg); MencV->DivInfo.PsoSeqStep =  8; break;
			case  8: OutputPsoMturnIncre(MencV, 1, 0,pAsicHwReg); MencV->DivInfo.PsoSeqStep =  9; break;
			case  9: pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_Conm; MencV->DivInfo.PsoSeqStep =  10; break;	/* <S147> , */
		     /*１回転内位置データ*/
			case 10: OutputPsoMturnIncre(MencV, 8, 1, pAsicHwReg); MencV->DivInfo.PsoSeqStep= 11; break;
			case 11: OutputPsoMturnIncre(MencV, 7, 1, pAsicHwReg); MencV->DivInfo.PsoSeqStep= 12; break;
			case 12: OutputPsoMturnIncre(MencV, 6, 1, pAsicHwReg); MencV->DivInfo.PsoSeqStep= 13; break;
			case 13: OutputPsoMturnIncre(MencV, 5, 1, pAsicHwReg); MencV->DivInfo.PsoSeqStep= 14; break;
			case 14: OutputPsoMturnIncre(MencV, 4, 1, pAsicHwReg); MencV->DivInfo.PsoSeqStep= 15; break;
			case 15: OutputPsoMturnIncre(MencV, 3, 1, pAsicHwReg); MencV->DivInfo.PsoSeqStep= 16; break;
			case 16: OutputPsoMturnIncre(MencV, 2, 1, pAsicHwReg); MencV->DivInfo.PsoSeqStep= 17; break;
			case 17: OutputPsoMturnIncre(MencV, 1, 1, pAsicHwReg); MencV->DivInfo.PsoSeqStep= 18; break;
			case 18: /* Output 0x0D*/
					 pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_Cr;/* <S147> 0x0D */
					 MencV->DivInfo.PsoSeqStep = 19;break;
			case 19: /* Wait 2ms*/
				     MencV->DivInfo.PsoSeqStep = 20;break;
			case 20: /* End Step*/
				     MencV->DivInfo.PsoSeqStep = 1;break;
			default: /* Default Step*/
				     MencV->DivInfo.PsoSeqStep = 0;break;
	 }/*End MencV.PsoSeqStep*/
  }/*End (MencV->AxisMotType == MOTTYPE_ROTARY)*/
/*-----------------------------------------------------------------------------------------------------------------------*/
 /* MOTTYPE_LINEAR */
	 if(MencV->AxisMotType == MOTTYPE_LINEAR)
	 {
		  switch( MencV->DivInfo.PsoSeqStep )/*Step One by One*/
		  {
			case  0: /* Initial Step*/
					MencV->DivInfo.PsoSeqWait = 0;
					MencV->DivInfo.PsoSeqStep = 1;
			break;
			case  1: /* Get  Multiturn  and Incremental Data	*/
				    MencV->DivInfo.PsoMturnLatch= SencGetMturnData(MencV,MencV->DivInfo.Limmlt, MencV->DivInfo.Bit_dp,(USHORT)MencV->RxPosH[0],MencV->RxPosL[0].dw );
				    MencV->DivInfo.PsoLinearData_H=  (USHORT)MencV->RxPosH[0];/* DATA_High*/
				    MencV->DivInfo.PsoLinearData_L = ((ULONG)MencV->RxPosL[0].dw)>>MencV->MposSftR;/*DATA_LOW*/
					MencV->DivInfo.PsoSeqStep = 2;
			break;
			case  2: /*Output 'P'*/
				     pAsicHwReg->AREG_PFNCSET &= ~BIT13; /*Output Data Set DA2*/
					 pAsicHwReg2->AREG_PFNCSET|= BIT13;  /*Output Data Set PSO Register Selection*/
					 /*SGDV未搭載処理、Σ7追加レジスタ処理*/
					 pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_P;/* <S147> P*/
					 MencV->DivInfo.PsoSeqStep = 3;
			break;
			case 3 : /* Output '+' or '-'*/
				    /*------------------------------------------------------------------------------*/
					/*	逆回転モード																*/
					/*------------------------------------------------------------------------------*/
					if( MencV->RevAsmMotor ^ RvsDir)	/* 反転接続 XOR 逆回転モード	*/
					{
						if( (SHORT)MencV->DivInfo.PsoMturnLatch >= 0 )
						{
							pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_Mins;/*-*/
						}
						else
						{
							pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_Puls;/*+*/
						}
					}
					/*------------------------------------------------------------------------------*/
					/*	正回転モード																    */
					/*------------------------------------------------------------------------------*/
					else
					{
						if( (SHORT)MencV->DivInfo.PsoMturnLatch >= 0 )
						{
							pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_Puls;/*+*/
						}
						else
						{
							pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_Mins;/*-*/
						}
					}
					MencV->DivInfo.PsoSeqStep = 4;
			break;
/* <S20A>：引数見直し：[MencV->DivInfo.PsoLinearData_*]をポインタに */
			case 4:  OutputPsoLinearIncre((LONG*)&MencV->DivInfo.PsoLinearData_H, 5, pAsicHwReg); MencV->DivInfo.PsoSeqStep= 5; break;
			case 5:  OutputPsoLinearIncre((LONG*)&MencV->DivInfo.PsoLinearData_H, 4, pAsicHwReg); MencV->DivInfo.PsoSeqStep= 6; break;
			case 6:  OutputPsoLinearIncre((LONG*)&MencV->DivInfo.PsoLinearData_H, 3, pAsicHwReg); MencV->DivInfo.PsoSeqStep=  7; break;
			case 7:  OutputPsoLinearIncre((LONG*)&MencV->DivInfo.PsoLinearData_H, 2, pAsicHwReg); MencV->DivInfo.PsoSeqStep=  8; break;
			case 8:  OutputPsoLinearIncre((LONG*)&MencV->DivInfo.PsoLinearData_H, 1, pAsicHwReg); MencV->DivInfo.PsoSeqStep=  9; break;
			case 9:  OutputPsoLinearIncre((LONG*)&MencV->DivInfo.PsoLinearData_L, 6, pAsicHwReg); MencV->DivInfo.PsoSeqStep=  10; break;
			case 10: OutputPsoLinearIncre((LONG*)&MencV->DivInfo.PsoLinearData_L, 5, pAsicHwReg); MencV->DivInfo.PsoSeqStep=  11; break;
			case 11: OutputPsoLinearIncre((LONG*)&MencV->DivInfo.PsoLinearData_L, 4, pAsicHwReg); MencV->DivInfo.PsoSeqStep=  12; break;
			case 12: OutputPsoLinearIncre((LONG*)&MencV->DivInfo.PsoLinearData_L, 3, pAsicHwReg); MencV->DivInfo.PsoSeqStep=  13; break;
		    case 13: OutputPsoLinearIncre((LONG*)&MencV->DivInfo.PsoLinearData_L, 2, pAsicHwReg); MencV->DivInfo.PsoSeqStep=  14; break;
			case 14: OutputPsoLinearIncre((LONG*)&MencV->DivInfo.PsoLinearData_L, 1, pAsicHwReg); MencV->DivInfo.PsoSeqStep=  15; break;
/* <S20A>:ここまで */
			case 15: /* Output 0x0D   CR*/
					 pAsicHwReg->AREG_UARTXD  = EvenAsciiCode_Cr; /* <S147>  0x0D */
					 MencV->DivInfo.PsoSeqStep = 16;break;
			case 16: /* Wait 2ms*/
				     MencV->DivInfo.PsoSeqStep = 17;break;
			case 17: /* Wait 2ms*/
				     MencV->DivInfo.PsoSeqStep = 18;break;
			case 18: /* Wait 2ms*/
				     MencV->DivInfo.PsoSeqStep = 19;break;
			case 19: /* Wait 2ms*/
				     MencV->DivInfo.PsoSeqStep = 20;break;
			case 20: /* End Step*/
				     MencV->DivInfo.PsoSeqStep = 1;break;
			default: /* Default Step*/
				     MencV->DivInfo.PsoSeqStep = 0;break;
		  }/*End MencV.PsoSeqStep*/
	 }/* End if(MencV->AxisMotType == MOTTYPE_LINEAR)*/
}

/************************************************************************************************/
/* Linear Get Incremental Data                                                                	*/
/************************************************************************************************/
static void	OutputPsoLinearIncre(LONG *PsoIncPulseOut, ULONG digit, ASIC_HW_IF_STR *pAsicHwReg)	/* <S20A>:引数見直し */
{
	 long	x;
	 long	outx;
/*--------------------------------------------------------------------------------------------------*/
/*		Convert digit data																			*/
/*--------------------------------------------------------------------------------------------------*/
		switch( digit )
		{
			case  1: digit =  1;        break;
			case  2: digit =  10;       break;
			case  3: digit =  100;      break;
			case  4: digit =  1000;     break;
			case  5: digit =  10000;    break;
			case  6: digit =  100000;   break;
			default: break;
		}
/*		Output Incremental data		*/
/*--------------------------------------------------------------------------------------------------*/
	x = *PsoIncPulseOut/digit;	/* <S20A> */
	outx = MlibEvenAsciiTbl[ x ];		/* <S14F> */
	pAsicHwReg->AREG_UARTXD = (UCHAR)outx; /*Send the Data*/
	*PsoIncPulseOut -= (x * digit);	/* <S20A> */
}


/************************************************************************************************/
/*Get Multiturn and Incremental Data*/
/****************************************************************************************************/
static void	OutputPsoMturnIncre( MENCV *MencV,ULONG digit, ULONG outsel,ASIC_HW_IF_STR* pAsicHwReg)
{
	ULONG	x;
	ULONG	outx;
/*--------------------------------------------------------------------------------------------------*/
/*		Convert digit data	*/
/*--------------------------------------------------------------------------------------------------*/
		switch( digit )
		{
			case  1: digit = 1;        break;
			case  2: digit = 10;       break;
			case  3: digit = 100;      break;
			case  4: digit = 1000;     break;
			case  5: digit = 10000;    break;
			case  6: digit = 100000;   break;
			case  7: digit = 1000000;  break;
			case  8: digit = 10000000; break;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Output Multiturn data	 */
/*--------------------------------------------------------------------------------------------------*/
		if( outsel == 0 )
		{
			x = MencV->DivInfo.PsoMturnLatch/digit;
			outx =MlibEvenAsciiTbl[x];		/* <S14F> */
			pAsicHwReg->AREG_UARTXD = (UCHAR)outx;
			/*Send the Data SGDV未搭載処理、Σ7追加レジスタ処理*/
			MencV->DivInfo.PsoMturnLatch -= (USHORT)(x * digit);
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Output Incremental data		*/
/*--------------------------------------------------------------------------------------------------*/
		else
		{
			x = MencV->DivInfo.PsoIncPulseOut/digit;
			outx =MlibEvenAsciiTbl[ x ];		/* <S14F> */
			pAsicHwReg->AREG_UARTXD = (UCHAR)outx; /*Send the Data*/
			/*Send the Data SGDV未搭載処理、Σ7追加レジスタ処理*/
			MencV->DivInfo.PsoIncPulseOut -= (x * digit);
		}
}
#endif
/*End <S189>*/
/***************************************** end of file **********************************************/
