/****************************************************************************
Description	: Tuning Function Library
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
Y.Oka  2010.11.16    created

*****************************************************************************/
#include "TuneLib.h"
#include "PnPrmListTbl.h"
#include "ResVibCtrl.h"
#include "MLib.h"



/****************************************************************************************************/
/*																									*/
/*		Function Definition																			*/
/*																									*/
/****************************************************************************************************/
/* PRM_RSLT	TunePrmWriteRam( REG_ACC_T *AccessPrm, CPRM_ATTR_T *PrmAttr, LONG Data, LONG Size );	*/
/* USHORT	TunePrmLimitUnsigned( ULONG Input, CPRM_ATTR_T *PrmAttr );								*/
/* SHORT	TunePrmLimitSigned( LONG Input, CPRM_ATTR_T *PrmAttr );									*/
/* USHORT	TuneKvFuncTi( USHORT Kv, BOOL IPSpdControl );											*/
/* USHORT	TuneKvFuncKp( USHORT Kv, USHORT Ti, BOOL IPSpdControl );								*/
/* USHORT	TuneKvFuncDobsgn( USHORT Kv );															*/
/* SHORT	TuneKvFuncDobLpFil( USHORT Kv, USHORT Dobsgn );											*/
/* SHORT	TuneNotchQ( AUTONOTCH *ANotch, REGIF_CONFIG_T *RegCfg, USHORT kv,						*/
/*													USHORT jrate, UCHAR sel, PRMDATA *Prm );		*/
/* void		TuneGetNotchQSetting( AUTONOTCH *ANotch, USHORT *NotchQ1,								*/
/*															USHORT *NotchQ2, USHORT *TrqFil );		*/
/* USHORT	TuneNotchModKv( USHORT kv, USHORT jrate, USHORT j1rate );								*/
/* SHORT	TuneNotchQset( AUTONOTCH_PRM *ANotchPrm, USHORT kv, USHORT jrate, USHORT Nfreq,			*/
/*																USHORT Antfreq, USHORT *autoQ );	*/
/* USHORT	TuneNotchTfcalc( AUTONOTCH_PRM *ANotchPrm, USHORT kv,									*/
/*													USHORT jrate, USHORT Nfreq, USHORT autoQ_Q1 );	*/
/* USHORT	TuneOnePrmTfcalc( AUTONOTCH_PRM *ANotchPrm, USHORT kv, USHORT jrate );					*/
/* SHORT	TuneNotchQset2( USHORT freq, USHORT Nfreq, USHORT *autoQ );								*/
/* void		TuneFilGainCalbyFilType( AUTONOTCH *ANotch, UCHAR Type );								*/
/* void		TuneMfcActive( PRMDATA *Prm );															*/
/* void		TuneMfcActiveStMode( PRMDATA *Prm );													*/
/* void		TuneDobsActive( PRMDATA *Prm );															*/
/* SHORT	TuneATypeResSet( USHORT freq, USHORT auton_sw, PRMDATA *Prm );							*/
/* SHORT	TuneNotchFilterSetting( AUTONOTCH *ANotch, PRMDATA *Prm, REGIF_CONFIG_T *RegCfg );		*/
/* SHORT	TuneVibrationTrace( DETVIB *DetVib, FFTANALYZE *FftAna,							*/
/*											UCHAR TraceRequest, SHORT TrgLvl, BOOL TuneLessSts );	*/
/* USHORT	TuneMfcKpmCalcAntFreq( USHORT AntFreq );												*/
/****************************************************************************************************/



/****************************************************************************************************/
/*																									*/
/*		Constant Definition																			*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		AAT、ワンパラでのTf計算、ノッチQ値計算に使用する対Kv比、J1のテーブル。						*/
/*		剛性レベル設定によってテーブルが選択される。												*/
/*																									*/
/*			配列の要素																				*/
/*				①自動Tf計算のカットオフのKvとの比					[倍]							*/
/*				②ノッチON時の自動Tf計算のカットオフのKvとの比		[倍]							*/
/*				③ノッチQ値計算のカットオフのKvとの比				[倍]							*/
/*				④自動Tf計算のモータ側慣性モーメント比 				[%]								*/
/*				⑤ノッチON時の自動Tf計算のモータ側慣性モーメント比	[%]								*/
/*				⑥ノッチON時のQ値計算のモータ側慣性モーメント比		[%]								*/
/*				⑦自動Tf計算最小設定値								[0.01ms]						*/
/*				⑧ノッチQ値設定最小値								[0.01]							*/
/*																									*/
/****************************************************************************************************/
const AUTONOTCH_PRM ANotchTbl[ 4 ] =
{
	{	4,	6,	6,	65535,	65535,	65535,	10,		50},	/* 剛性 0 ゲイン */
	{	4,	6,	6,	200,	200,	400,	10,		50},	/* 剛性 1 ゲイン */
	{	4,	4,	6,	400,	400,	200,	10,		150},	/* 剛性 2 ゲイン */
	{	4,	4,	6,	65535,	65535,	65535,	10,		70},	/* 剛性 3 ゲイン */
};



/****************************************************************************************************/
/*																									*/
/*		パラメータ情報のRAM書き込み(パラメータ計算あり)												*/
/*																									*/
/****************************************************************************************************/
PRM_RSLT	TunePrmWriteRam( REG_ACC_T *AccessPrm, CPRM_ATTR_T *PrmAttr, LONG Data, LONG Size )
{
	PRM_RSLT	rslt;

	AccessPrm->cPrmDef = PrmAttr;
	rslt = PrmWriteValue( AccessPrm, &Data, Size );

	return	rslt;
}



/****************************************************************************************************/
/*																									*/
/*		SHORTサイズパラメータ設定値上下限リミット処理(符号なしパラメータ用)							*/
/*																									*/
/****************************************************************************************************/
USHORT	TunePrmLimitUnsigned( ULONG Input, CPRM_ATTR_T *PrmAttr )
{
	USHORT	Output;

	if( Input > (USHORT)(PrmAttr->UpperLimit) )
	{
		Output = (USHORT)(PrmAttr->UpperLimit);
	}
	else if( Input < (USHORT)(PrmAttr->LowerLimit) )
	{
		Output = (USHORT)(PrmAttr->LowerLimit);
	}
	else
	{
		Output = (USHORT)Input;
	}

	return	Output;
}



/****************************************************************************************************/
/*																									*/
/*		SHORTサイズパラメータ設定値上下限リミット処理(符号ありパラメータ用)							*/
/*																									*/
/****************************************************************************************************/
SHORT	TunePrmLimitSigned( LONG Input, CPRM_ATTR_T *PrmAttr )
{
	SHORT	Output;

	if( Input > (SHORT)(PrmAttr->UpperLimit) )
	{
		Output = (SHORT)(PrmAttr->UpperLimit);
	}
	else if( Input < (SHORT)(PrmAttr->LowerLimit) )
	{
		Output = (SHORT)(PrmAttr->LowerLimit);
	}
	else
	{
		Output = (SHORT)Input;
	}

	return	Output;
}



/****************************************************************************************************/
/*																									*/
/*		Kv連動のTi設定																				*/
/*																									*/
/****************************************************************************************************/
/*		概要:	AATとワンパラにて使用される。														*/
/****************************************************************************************************/
USHORT	TuneKvFuncTi( USHORT Kv, BOOL IPSpdControl )
{
	ULONG	work;
	USHORT	Ti;

	if( IPSpdControl )
	{ /* I-P制御時 */
		/* 速度ループ積分時定数設定 */
		work = (ULONG)(TUNE_IP_TIKV / Kv);
	}
	else
	{ /* PI制御時 */
		/* 速度ループ積分時定数設定 */
		work = (ULONG)(TUNE_PI_TIKV / Kv);
	}

	Ti = TunePrmLimitUnsigned( work, &pndef_pitime );

	return	Ti;
}


/****************************************************************************************************/
/*																									*/
/*		Kv連動のKp設定																				*/
/*																									*/
/****************************************************************************************************/
/*		概要:	AATのモデル追従ありのときのバランス、モデル追従ありのときの推奨バランス				*/
/*				AATとワンパラにて使用される。														*/
/****************************************************************************************************/
USHORT	TuneKvFuncKp( USHORT Kv, USHORT Ti, BOOL IPSpdControl )
{
	ULONG	work;
	USHORT	Kp;

	if( IPSpdControl )
	{ /* I-P制御時	ζ=1.0 */
		work = (ULONG)(TUNE_IP_KPTI / Ti);
	}
	else
	{ /* PI制御時	ζ=1.25 */
		/* Kp[1/s] = Kv[rad/s] / 4 ≒ 1.5 *Kv[Hz] */
		work = (ULONG)(Kv * 3) >> 1;
	}

	Kp = TunePrmLimitUnsigned( work, &pndef_posgn );

	return	Kp;
}


/****************************************************************************************************/
/*																									*/
/*		Kv連動の外乱オブザーバ設定																	*/
/*																									*/
/****************************************************************************************************/
/*		概要:	AATとワンパラにて使用される。														*/
/****************************************************************************************************/
USHORT	TuneKvFuncDobsgn( USHORT Kv )
{
	ULONG	work;
	USHORT	dobgn;

	/* Kds = Kv * 100% */
	work = TUNE_DOBGNRATIO;

	dobgn = TunePrmLimitUnsigned( work, &pndef_dobgn );

	return	dobgn;
}



/****************************************************************************************************/
/*																									*/
/*		Kv連動の外乱オブザーバ設定																	*/
/*																									*/
/****************************************************************************************************/
/*		概要:	AATとワンパラにて使用される。														*/
/****************************************************************************************************/
SHORT	TuneKvFuncDobLpFil( USHORT Kv, USHORT Dobsgn )
{
	ULONG	work;
	SHORT	dlpfil;

	/* Kv * 4 */
	work = 4 * Kv;

	/* 400Hz でリミット */
	if( work > TUNE_MAX_DLPFIL )
	{
		work = TUNE_MAX_DLPFIL;
	}

	dlpfil = (SHORT)( Dobsgn - (USHORT)work );
	dlpfil = TunePrmLimitSigned( (LONG)dlpfil, &pndef_dlpfil );

	return	dlpfil;
}




/****************************************************************************************************/
/*																									*/
/*		ノッチフィルタQ値自動計算選択																*/
/*																									*/
/****************************************************************************************************/
/*		概要:	1段目、2段目ノッチフィルタの有効／無効、ノッチ周波数の大小関係を判別し、			*/
/*				Kvとイナーシャに応じてQ値の再計算を行う。											*/
/*				ノッチフィルタ無効時は、現在の設定値を返す。										*/
/****************************************************************************************************/
SHORT	TuneNotchQ( AUTONOTCH *ANotch, REGIF_CONFIG_T *RegCfg, USHORT kv, USHORT jrate, UCHAR sel, PRMDATA *Prm )
{
	SHORT			rc;	
	USHORT			wk_notchQ;
	REG_ACC_T		AccessPrm;
	AUTONOTCH_PRM	*ANotchPrm;
	AUTONOTCH_DATA	*ANotchData;

	rc = OK;
	ANotchPrm = &(ANotch->ANotchPrm);
	ANotchData = &(ANotch->ANotchData);

	/* RAM書き込み用レジスタ情報設定 */
	AccessPrm.MemAddr = 0;
	AccessPrm.hRegCfg = RegCfg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

	/*----------------------------------------------------------------------------------*/
	/*	変数の初期化（現在の設定値）													*/
	/*----------------------------------------------------------------------------------*/
	/* 1段目ノッチQ値 */
	ANotchData->var.NotchQ1 = Prm->ntchq1;
	/* 2段目ノッチQ値 */
	ANotchData->var.NotchQ2 = Prm->ntchq2;
	/* 1段目トルク指令フィルタ */
	ANotchData->var.TrqFil = Prm->trqfil11;

	if( (Prm->tfuncsw & 0x000F) == 0x0001 )
	{ /* 1段目ノッチ有効 */
		if( (Prm->tfuncsw & 0x0F00) == 0x0100 )
		{ /* 1段目、2段目ノッチ有効 */
			/*--------------------------------------------------------------------------*/
			/*	ノッチ周波数の低い方は、Kv・JLに連動したQ値を計算する。					*/
			/*	ノッチ周波数の高い方は、低い方のノッチ周波数に連動したQ値を計算する。	*/
			/*--------------------------------------------------------------------------*/
			if( Prm->ntchfil1 > Prm->ntchfil2 )
			{
				rc = TuneNotchQset( ANotchPrm, kv, jrate, Prm->ntchfil2, TUNE_ANTFRQ_NTQ2, &wk_notchQ );
				/*----------------------------------------------------------------------*/
				/*	2段目ノッチQ値計算													*/
				/*----------------------------------------------------------------------*/
				if( (Prm->anotchsw & 0x0F00) == 0x0100 )
				{
					ANotchData->var.NotchQ2 = wk_notchQ;
					/*------------------------------------------------------------------*/
					/*	Tf計算															*/
					/*------------------------------------------------------------------*/
					ANotchData->var.TrqFil = 
						TuneNotchTfcalc( ANotchPrm, kv, jrate, Prm->ntchfil2, ANotchData->var.NotchQ2 );
				}
				else
				{
					/*------------------------------------------------------------------*/
					/*	Tf計算															*/
					/*------------------------------------------------------------------*/
					ANotchData->var.TrqFil = TuneOnePrmTfcalc( ANotchPrm, kv, jrate );
				}
				/*----------------------------------------------------------------------*/
				/*	1段目ノッチQ値計算													*/
				/*----------------------------------------------------------------------*/
				if( (Prm->anotchsw & 0x000F) == 0x0001 )
				{
					rc |= TuneNotchQset2( Prm->ntchfil2, Prm->ntchfil1, &ANotchData->var.NotchQ1 );
				}
			}
			else
			{
				rc = TuneNotchQset( ANotchPrm, kv, jrate, Prm->ntchfil1, TUNE_ANTFRQ_NTQ1, &wk_notchQ );
				/*----------------------------------------------------------------------*/
				/*	1段目ノッチQ値計算													*/
				/*----------------------------------------------------------------------*/
				if( (Prm->anotchsw & 0x000F) == 0x0001 )
				{
					ANotchData->var.NotchQ1 = wk_notchQ;
					/*------------------------------------------------------------------*/
					/*	Tf計算															*/
					/*------------------------------------------------------------------*/
					ANotchData->var.TrqFil = 
						TuneNotchTfcalc( ANotchPrm, kv, jrate, Prm->ntchfil1, ANotchData->var.NotchQ1 );
				}
				else
				{
					/*------------------------------------------------------------------*/
					/*	Tf計算															*/
					/*------------------------------------------------------------------*/
					ANotchData->var.TrqFil = TuneOnePrmTfcalc( ANotchPrm, kv, jrate );
				}
				/*----------------------------------------------------------------------*/
				/*	2段目ノッチQ値計算													*/
				/*----------------------------------------------------------------------*/
				if( (Prm->anotchsw & 0x0F00) == 0x0100 )
				{
					rc |= TuneNotchQset2( Prm->ntchfil1,Prm->ntchfil2,&ANotchData->var.NotchQ2 );
				}
			}
		}
		else
		{ /* 1段目ノッチのみ有効 */
			rc = TuneNotchQset( ANotchPrm, kv, jrate, Prm->ntchfil1, TUNE_ANTFRQ_NTQ1, &wk_notchQ );
			/*--------------------------------------------------------------------------*/
			/*	1段目ノッチQ値計算														*/
			/*--------------------------------------------------------------------------*/
			if( (Prm->anotchsw & 0x000F) == 0x0001 )
			{
				ANotchData->var.NotchQ1 = wk_notchQ;
			/*----------------------------------------------------------------------*/
			/*	Tf計算																*/
			/*----------------------------------------------------------------------*/
				ANotchData->var.TrqFil = 
					TuneNotchTfcalc( ANotchPrm, kv, jrate, Prm->ntchfil1, ANotchData->var.NotchQ1 );
			}
			else
			{
				/*----------------------------------------------------------------------*/
				/*	Tf計算																*/
				/*----------------------------------------------------------------------*/
				ANotchData->var.TrqFil = TuneOnePrmTfcalc( ANotchPrm, kv, jrate );
			}
		}
	}
	else
	{ /* 1段目ノッチ無効 */
		if( (Prm->tfuncsw & 0x0F00) == 0x0100 )
		{ /* 2段目ノッチのみ有効 */
			rc = TuneNotchQset( ANotchPrm, kv, jrate, Prm->ntchfil2, TUNE_ANTFRQ_NTQ2, &wk_notchQ );
			/*--------------------------------------------------------------------------*/
			/*	2段目ノッチQ値計算														*/
			/*--------------------------------------------------------------------------*/
			if( (Prm->anotchsw & 0x0F00) == 0x0100 )
			{
				ANotchData->var.NotchQ2 = wk_notchQ;
				/*----------------------------------------------------------------------*/
				/*	Tf計算																*/
				/*----------------------------------------------------------------------*/
				ANotchData->var.TrqFil = 
					TuneNotchTfcalc( ANotchPrm, kv, jrate, Prm->ntchfil2, ANotchData->var.NotchQ2 );
			}
			else
			{
				/*----------------------------------------------------------------------*/
				/*	Tf計算																*/
				/*----------------------------------------------------------------------*/
				ANotchData->var.TrqFil = TuneOnePrmTfcalc( ANotchPrm, kv, jrate );
			}
		}
		else
		{ /* 1段目、2段目ともに無効 */
			/* ノッチフィルタ無効 */
			rc = -3;
			ANotchData->var.TrqFil = TuneOnePrmTfcalc( ANotchPrm, kv, jrate );
		}
	}
	/*----------------------------------------------------------------------------------*/
	/*	計算値をRAMに反映																*/
	/*----------------------------------------------------------------------------------*/
	if( (sel == 1) || ((sel == 2) && (rc == OK)) ) /* sel==2 時は、計算OK時のみ変更する	*/
	{ /* 計算内容をRAMに反映(注意!! EEPROMには書かない) */
		/* Pn401 */
		TunePrmWriteRam( &AccessPrm, &pndef_trqfil11, (LONG)ANotchData->var.TrqFil, 2 );
		/* Pn40A */
		TunePrmWriteRam( &AccessPrm, &pndef_ntchq1, (LONG)ANotchData->var.NotchQ1, 2 );
		/* Pn40D */
		TunePrmWriteRam( &AccessPrm, &pndef_ntchq2, (LONG)ANotchData->var.NotchQ2, 2 );
	}
	return	rc;
}



/****************************************************************************************************/
/*																									*/
/*		ノッチフィルタQ値計算結果取得API															*/
/*																									*/
/****************************************************************************************************/
/*		概要:	ノッチフィルタQ値自動計算選択処理(TuneNotchQ)の計算結果を取得する。					*/
/*				1段目ノッチQ値、2段目ノッチQ値、1段目トルク指令フィルタを返す。						*/
/****************************************************************************************************/
void	TuneGetNotchQSetting( AUTONOTCH *ANotch, USHORT *NotchQ1, USHORT *NotchQ2, USHORT *TrqFil )
{
	/* 1段目ノッチQ値 */
	*NotchQ1 = ANotch->ANotchData.var.NotchQ1;
	/* 2段目ノッチQ値 */
	*NotchQ2 = ANotch->ANotchData.var.NotchQ2;
	/* 1段目トルク指令フィルタ */
	*TrqFil = ANotch->ANotchData.var.TrqFil;
}



/****************************************************************************************************/
/*																									*/
/*		Tf・ノッチQ値計算のためのKv補正計算															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		概要:	引数で指定されたKv、イナーシャ比、倍率からKvに対するイナーシャ補正を行う。			*/
/*																									*/
/*							Jrate[%] + 100															*/
/*				Kv' = SQRT(-----------------) * Kv													*/
/*								J1[%]																*/
/*													kv		：補正前Kv						[Hz]	*/
/*													Jrate	：補正用負荷慣性モーメント比	[%]		*/
/*													J1		：モータ側慣性モーメント比		[%]		*/
/*																									*/
/****************************************************************************************************/
USHORT	TuneNotchModKv( USHORT kv, USHORT jrate, USHORT j1rate )
{
	LONG	work;
	USHORT	TLV_kv;

	if( j1rate == 0 )
	{	/* J1が0[%]の時 → 共振、反共振周波数から計算 */
		/*----------------------------------------------------------*/
		/*	イナーシャ比補正計算									*/
		/*				  		Freq_res[Hz]						*/
		/*		Kv'[0.1Hz] = ----------------- * Kv[0.1Hz]			*/
		/*						Freq_ant[Hz]						*/
		/*----------------------------------------------------------*/
		work = (LONG)TUNE_RESFRQ_KV * (LONG)kv / (LONG)TUNE_ANTFRQ_KV;
	}
	else if( (jrate + 100) <= j1rate )
	{
		/* J1以下は剛体と見なす */
		work = (LONG)kv ;
	}
	else
	{
		/*----------------------------------------------------------*/
		/*	イナーシャ比補正計算									*/
		/*				  			Jrate[%] + 100					*/
		/*		Kv'[0.1Hz] = SQRT(-----------------) * Kv[0.1Hz]	*/
		/*								J1[%]						*/
		/*----------------------------------------------------------*/
		work = 
			(LONG)kv * (LONG)MlibSqrtu32(((LONG)jrate + 100) * 10000 / (LONG)j1rate) /100;
	}

	/*--------------------------------------------------------------*/
	/*	補正Kv 上下限チェック										*/
	/*--------------------------------------------------------------*/
	if( work > (LONG)(pndef_loophz.UpperLimit) )
	{
		TLV_kv = (USHORT)pndef_loophz.UpperLimit;
	}
	else if( work < (LONG)kv )
	{
		TLV_kv = kv;
	}
	else
	{
		TLV_kv = (USHORT)work;
	}

	return	TLV_kv;
}



/****************************************************************************************************/
/*																									*/
/*		ノッチQ値の自動計算																			*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		概要:	引数で指定されたKvから、ノッチ周波数に対するノッチQを計算する。						*/
/*				ノッチ上下限チェック、ノッチONの可否チェックを行う。								*/
/*																									*/
/*					   α * Kv * fn								fn：ノッチ周波数			[Hz]	*/
/*				Q = ------------------							kv：補正後Kv				[Hz]	*/
/*					 fn^2 - α^2 * Kv^2							α：ノッチQ値計算用対Kv比	[倍]	*/
/*																									*/
/****************************************************************************************************/
SHORT	TuneNotchQset( AUTONOTCH_PRM *ANotchPrm, USHORT kv, USHORT jrate, USHORT Nfreq, 
																	USHORT Antfreq, USHORT *autoQ )
{
	SHORT	check;
	USHORT	TLV_kv;
	LONG	work1, work2, work3;
	LONG	freq_cl;
	LONG	sx, sy;
	LONG	fcl_Tf;
	SHORT	swk;

	check = OK;

	/*--------------------------------------------------------------*/
	/*	イナーシャ比補正計算										*/
	/*--------------------------------------------------------------*/
	TLV_kv = TuneNotchModKv( kv, jrate, ANotchPrm->J1Q );

	/*--------------------------------------------------------------*/
	/*	ノッチ無効時Tf[0.01ms]計算									*/
	/*--------------------------------------------------------------*/
	swk = TuneOnePrmTfcalc( ANotchPrm, kv, jrate );

	/*--------------------------------------------------------------*/
	/*	Tfから逆算したノッチカットオフ周波数						*/
	/*	 fcl[Hz] = 1000 * 100 / (2πTf[0.01ms])						*/
	/*--------------------------------------------------------------*/
	/* 最小Tf分を考慮 */
	work1 = (LONG)swk - ((LONG)ANotchPrm->TfMin - 5);
	if(work1 <= 0)
	{ /* Tfがゼロ以下 */
		/* 設定不可のため周波数を最大値にする */
		fcl_Tf = 10000;
	}
	else
	{
		/* ノッチ周波数最小設定値 159160 = 1000 *100 /6.283 */
		fcl_Tf = (LONG)(15916 / work1);
	}

	/*--------------------------------------------------------------*/
	/*	ノッチカットオフ周波数計算									*/
	/*--------------------------------------------------------------*/
	if( ANotchPrm->J1Q == 0 )
	{ /* 共振、反共振周波数から計算 */
		/* (freq_res[Hz] + freq_ant[Hz]) / 2 */
		freq_cl = ((LONG)Nfreq + (LONG)Antfreq) / 2;
	}
	else
	{ /* 補正Kv値から計算 */
		/* α × Kv'[Hz] */
		freq_cl = (LONG)ANotchPrm->KvrateQ * (LONG)TLV_kv / TUNE_ANOTCH_DECA;
	}

	/*--------------------------------------------------------------*/
	/*	ノッチカットオフ周波数判定									*/
	/*		Tfからの計算値と補正Kvから計算した周波数の高い方を選択	*/
	/*--------------------------------------------------------------*/
	if( freq_cl < fcl_Tf )
	{ /* 最小設定周波数よりfreq_clが小さい→ノッチ設定不可 */
		freq_cl = fcl_Tf;
	}

	/*--------------------------------------------------------------*/
	/*	ノッチQ値計算												*/
	/*					  α * Kv'[Hz] * fn[Hz]						*/
	/*		Q[0.01] = -------------------------------				*/
	/*					fn[Hz]^2 - α^2 * Kv'[Hz]^2					*/
	/*																*/
	/*	共振反共振周波数から計算する場合、上式のα * Kv'[Hz]を		*/
	/*	ノッチカットオフ周波数(f_cl)とする							*/
	/*--------------------------------------------------------------*/
	if( freq_cl >= (LONG)Nfreq )
	{ /* 対Kv比でノッチQ値計算不能 */
		/* Q値上限値に設定 */
		work1 = TUNE_ANOTCH_Q_MAX;
		check = NG;
	}
	else
	{
		/* 分子の計算 */
		/* α×Kv[0.1Hz]'×fn (×100はQ値単位[0.01]のため) */
		sx = 0;
		work1 = MlibPcalKxmulx( freq_cl, (LONG)Nfreq, 100, &sx );

		/* 分母の計算 */
		/* fn[Hz]^2 */
		work2 = (LONG)Nfreq * (LONG)Nfreq;
		work3 = freq_cl;

		sy = 0;
		/* α^2×Kv[Hz]'^2 */
		work3 = MlibPcalKxmulx( work3, work3, 1, &sy );
		/* fn[Hz]^2 - α^2×Kv[Hz]'^2 */
		work2 = MlibPcalKxsubx( work2, 0, work3, sy, &sy );

		if( work2 <= 0 )
		{
			/* Q値上限値に設定 */
			work1 = TUNE_ANOTCH_Q_MAX;
		}
		else
		{
			/* ノッチQ値 [0.01] */
			work1 = MlibPcalKxdivx( work1, sx, work2, sy, 24 );
			work1 = MlibKsgain2Long(work1);
		}
		/*--------------------------------------------------------------*/
		/*	上下限チェック												*/
		/*--------------------------------------------------------------*/
		if( work1 < ANotchPrm->NotchQMin )
		{ /* Q値が下限値より小さい場合は、下限値にクランプ */
			work1 = ANotchPrm->NotchQMin;
		}
		else if( work1 > TUNE_ANOTCH_Q_MAX_DET )
		{ /* Q値が検出上限値より大きい場合は、上限値にクランプ */
			work1 = TUNE_ANOTCH_Q_MAX;
			check = -2;
		}
		else if( work1 > TUNE_ANOTCH_Q_MAX )
		{ /* Q値が上限値より大きい場合は、上限値にクランプ */
			work1 = TUNE_ANOTCH_Q_MAX;
		}
	}
	*autoQ = (USHORT)work1;

	return	check;
}



/****************************************************************************************************/
/*																									*/
/*		ノッチON時のトルク指令1次ローパスフィルタ自動計算											*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		概要:	引数で指定されたKvから、ノッチ周波数に対するトルク指令フィルタ時定数を計算する。	*/
/*				ノッチONの場合、ノッチの等価遅れを加味してTfを算出する。							*/
/*				補正は、ノッチOFF時のTfから等価遅れ分を引く。										*/
/*				ただし、ノッチ等価遅れ補正後のTfは最小値でクランプする。							*/
/*																									*/
/*								1																	*/
/*				Tn[s] = ------------------															*/
/*						 2 * π * f_cl[Hz]				f_cl：ノッチ等価カットオフ周波数	[Hz]	*/
/*																									*/
/****************************************************************************************************/
USHORT	TuneNotchTfcalc( AUTONOTCH_PRM *ANotchPrm, USHORT kv, USHORT jrate, USHORT Nfreq, USHORT autoQ_Q1 )
{
	USHORT	TLV_kv;
	LONG	work1, work2;
	LONG	sx;

	/*--------------------------------------------------------------*/
	/*	イナーシャ比補正計算										*/
	/*--------------------------------------------------------------*/
	TLV_kv = TuneNotchModKv( kv, jrate, ANotchPrm->J1TfonNt );

	/*--------------------------------------------------------------*/
	/*	ノッチフィルタ等価カットオフ周波数	[Hz]					*/
	/*																*/
	/*					 8*Q^2										*/
	/*		f_cl = ------------------- * fn[Hz]						*/
	/*				8*Q^2 + 4*Q + 1									*/
	/*																*/
	/*	ノッチフィルタ等価遅れ時定数	[sec]						*/
	/*																*/
	/*						1										*/
	/*		Tn[s] = ------------------								*/
	/*				 2 * π * f_cl[Hz]								*/
	/*--------------------------------------------------------------*/
	/*	autoQ_Q1 < 1000 */
	work1 = 8 * (LONG)autoQ_Q1 * (LONG)autoQ_Q1;
	/* 400、10000はautoQ_Q1が[0.01]単位のための換算 */
	work2 = work1 + 400 * (LONG)autoQ_Q1 + 10000;

	/* カットオフ周波数算出	[Hz] */
	sx = 0;
	work1 = MlibPcalKxgain(work1, (LONG)Nfreq, work2 ,&sx, 0);

	/* 15915 = 100000[s->0.01ms] / 2π */
	work1 = MlibPcalKxdivx( 15915, 0, work1, sx, 24 );
	/* ノッチ等価遅れ時定数	[0.01ms] */
	work1 = MlibKsgain2Long(work1);

	/*--------------------------------------------------------------*/
	/*	ノッチOFF時のフィルタ時定数計算								*/
	/*	 Tf[0.01ms] = 100000 * 10 / (2πKv'[0.1Hz]* AUTOTf_KVRAT_NT)*/
	/*--------------------------------------------------------------*/
	/* 159160 = 1000 * 100 / 6.283 * 10 */
	work2 = 159160 / (LONG)ANotchPrm->KvrateTfonNt / (LONG)TLV_kv;

	if( ANotchPrm->J1TfonNt == 0 )
	{ /* 共振、反共振周波数から計算 */
		/* ノッチ等価遅れ加味しない */
		work1 = work2;
	}
	else
	{
		/* ノッチ補正後トルク指令フィルタ時定数 [0.01ms] */
		work1 = work2 - work1;
	}

	if( work1 < (LONG)ANotchPrm->TfMin )
	{
		/* Tf設定最小値でクランプ */
		work1 = (LONG)ANotchPrm->TfMin;
	}

	return	(USHORT)work1;
}


/****************************************************************************************************/
/*																									*/
/*		トルク指令1次ローパスフィルタ自動計算														*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		概要:	引数で指定されたKvに対するトルク指令フィルタ時定数を計算する。						*/
/*																									*/
/*;							1																		*/
/*				Tf = ----------------																*/
/*						2πKv'×α																	*/
/*																									*/
/****************************************************************************************************/
USHORT	TuneOnePrmTfcalc( AUTONOTCH_PRM *ANotchPrm, USHORT kv, USHORT jrate )
{
	USHORT	TLV_kv;
	LONG	work1;

	/*--------------------------------------------------------------*/
	/*	イナーシャ比補正計算										*/
	/*--------------------------------------------------------------*/
	TLV_kv = TuneNotchModKv( kv, jrate, ANotchPrm->J1Tf );

	/*--------------------------------------------------------------*/
	/*	フィルタ時定数計算											*/
	/*	 Tf[0.01ms] = 100000 * 10 / (2πKv'[0.1Hz]* AUTOTf_KVRAT)	*/
	/*--------------------------------------------------------------*/
	/* 159160 = 1000 * 100 / 6.283 * 10 */
	work1 = 159160 / (LONG)ANotchPrm->KvrateTf / (LONG)TLV_kv;

	if( work1 < (LONG)ANotchPrm->TfMin )
	{
		/* Tf設定最小値でクランプ */
		work1 = (LONG)ANotchPrm->TfMin;
	}
	
	return	(USHORT)work1;
}



/****************************************************************************************************/
/*																									*/
/*		ノッチフィルタQ値自動計算2																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		概要:	引数で指定された周波数をカットオフ周波数とするノッチQ値を計算する。					*/
/*																									*/
/*;					  Nfreq × freq																	*/
/*				Q = -----------------																*/
/*					 Nfreq^2 - freq^2																*/
/*																									*/
/****************************************************************************************************/
SHORT	TuneNotchQset2( USHORT freq, USHORT Nfreq, USHORT *autoQ )
{
	SHORT	check;
	ULONG	autoQ_Q2;
	LONG	work1, work2;

	check = OK;

	if( freq >= Nfreq )
	{	/* 振動周波数が目標カットオフ周波数以上 → 計算不能 */
		autoQ_Q2 = TUNE_ANOTCH_Q_MAX;
		check = NG;
	}
	else
	{
		/* 100倍するのは、Q値の単位をパラメータに合わせるため */
		work1 = Nfreq * Nfreq - freq * freq;
		work2 = Nfreq * freq * 100;

		autoQ_Q2 = work2 /work1;

		/* 上下限チェック */
		if( autoQ_Q2 < TUNE_ANOTCH_Q_MIN )
		{ /* Q値が1.50以下の場合 */
			/* 1.50に設定 */
			autoQ_Q2 = TUNE_ANOTCH_Q_MIN;
		}
		else if( autoQ_Q2 > TUNE_ANOTCH_Q_MAX )
		{ /* Q値が3.00以上の場合 */
			/* 3.00に設定 */
			autoQ_Q2 = TUNE_ANOTCH_Q_MAX;
			check = -2;
		}
	}
	*autoQ = (USHORT)autoQ_Q2;

	return	check;
}


/****************************************************************************************************/
/*																									*/
/*		AAT・ワンパラ機能合成レベル選択処理															*/
/*																									*/
/****************************************************************************************************/
/*		概要:	AATやワンパラで設定された機械剛性レベルから内部テーブルを呼び出し、変数に設定する。	*/
/****************************************************************************************************/
void	TuneFilGainCalbyFilType( AUTONOTCH *ANotch, UCHAR Type )
{
	/* 自動Tf計算のカットオフのKvとの比[倍] */
	ANotch->ANotchPrm.KvrateTf = ANotchTbl[ Type ].KvrateTf;

	/* ノッチON時の自動Tf計算のカットオフのKvとの比[倍] */
	ANotch->ANotchPrm.KvrateTfonNt = ANotchTbl[ Type ].KvrateTfonNt;

	/* ノッチQ値計算のカットオフのKvとの比[倍] */
	ANotch->ANotchPrm.KvrateQ = ANotchTbl[ Type ].KvrateQ;

	/* 自動Tf計算の 側慣性モーメント比[%] */
	ANotch->ANotchPrm.J1Tf = ANotchTbl[ Type ].J1Tf;

	/* ノッチON時の自動Tf計算のモータ側慣性モーメント比[%] */
	ANotch->ANotchPrm.J1TfonNt = ANotchTbl[ Type ].J1TfonNt;

	/* ノッチON時のQ値計算のモータ側慣性モーメント比[%] */
	ANotch->ANotchPrm.J1Q = ANotchTbl[ Type ].J1Q;

	/* 自動Tf計算最小設定値[0.01ms] */
	ANotch->ANotchPrm.TfMin = ANotchTbl[ Type ].TfMin;

	/* ノッチQ値最小設定値[-] */
	ANotch->ANotchPrm.NotchQMin = ANotchTbl[ Type ].NotchQMin;
	
	return;
}



#if 0 /* SGDV不使用のためコメントアウト */
/****************************************************************************************************/
/*																									*/
/*		モデル追従/外乱オブザーバの開始																*/
/*																									*/
/****************************************************************************************************/
/*		概要:	モデル追従・外乱オブザーバを開始する。												*/
/****************************************************************************************************/
void	TuneMfcDobsActive( HMSGIF Hmsg )
{
	/* 両方使用中のとき、以外は強制ON＆パラメータ上書き */
	if( (Gprm.f.Dobs == FALSE) || (Gprm.MFCModel == 0) )
	{
		TuneMfcActive( Hmsg );
		TuneDobsActive( Hmsg );
	}

	return;
}
#endif



/****************************************************************************************************/
/*																									*/
/*		モデル追従の開始																			*/
/*																									*/
/****************************************************************************************************/
/*		概要:	モデル追従を開始する。このとき、ζを1、TFFを100、VFFを100に上書きする。				*/
/*				AATとワンパラにて使用される。														*/
/****************************************************************************************************/
//void	TuneMfcActive( PRMDATA *Prm )
void	TuneMfcActive( PRMDATA *Prm, REGIF_CONFIG_T *RegCfg )/*<S09E>*/
{
	USHORT		data;
	REG_ACC_T	AccessPrm;

	/*--------------------------------------------------------------*/
	/* RAM書き込み用レジスタ情報設定								*/
	/*--------------------------------------------------------------*/
	AccessPrm.MemAddr  = 0;
	AccessPrm.hRegCfg  = RegCfg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

	Prm->mdlzeta = TUNE_MDLZETA; /* Pn142 */
	Prm->mdlptff = TUNE_MFCKTFF; /* Pn143 */
	Prm->mdlntff = TUNE_MFCKTFF; /* Pn144 */
	Prm->mdlvff  = TUNE_MFCKVFF; /* Pn147 */

	/* Pn140 */
	data = Prm->mdlsw;
	data |= 0x0001;
	//Prm->mdlsw = data;
	TunePrmWriteRam( &AccessPrm, &pndef_mdlsw, (LONG)(data), 2 );

	return;
}



/****************************************************************************************************/
/*																									*/
/*		モデル追従の開始																			*/
/*																									*/
/****************************************************************************************************/
/*		概要:	モデル追従を開始する。このとき、ζを1、TFFを100、VFFを95に上書きする。				*/
/*				AATとワンパラにて使用される。														*/
/****************************************************************************************************/
//void	TuneMfcActiveStMode( PRMDATA *Prm )
void	TuneMfcActiveStMode( PRMDATA *Prm, REGIF_CONFIG_T *RegCfg )/*<S09E>*/
{
	USHORT		data;
	REG_ACC_T	AccessPrm;

	/*--------------------------------------------------------------*/
	/* RAM書き込み用レジスタ情報設定								*/
	/*--------------------------------------------------------------*/
	AccessPrm.MemAddr  = 0;
	AccessPrm.hRegCfg  = RegCfg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

	Prm->mdlzeta = TUNE_MDLZETA;  /* Pn142 */
	Prm->mdlptff = TUNE_MFCKTFF;  /* Pn143 */
	Prm->mdlntff = TUNE_MFCKTFF;  /* Pn144 */
	Prm->mdlvff  = TUNE_MFCKVFF2; /* Pn147 */

	/* Pn140 */
	data = Prm->mdlsw;
	data |= 0x0001;
	//Prm->mdlsw = data;
	TunePrmWriteRam( &AccessPrm, &pndef_mdlsw, (LONG)(data), 2 );

	return;
}



/****************************************************************************************************/
/*																									*/
/*		外乱オブザーバの開始																		*/
/*																									*/
/****************************************************************************************************/
/*		概要:	外乱オブザーバを開始する。このとき、FBゲインを85％、イナーシャ補正を100%に上書き	*/
/*				する。AATとワンパラにて使用される。													*/
/****************************************************************************************************/
//void	TuneDobsActive( PRMDATA *Prm )
void	TuneDobsActive( PRMDATA *Prm, REGIF_CONFIG_T *RegCfg )/*<S09E>*/
{
	USHORT		data;
	REG_ACC_T	AccessPrm;

	/*--------------------------------------------------------------*/
	/* RAM書き込み用レジスタ情報設定								*/
	/*--------------------------------------------------------------*/
	AccessPrm.MemAddr  = 0;
	AccessPrm.hRegCfg  = RegCfg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

	//Prm->dtrqgn = TUNE_DTRQGN; /* Pn123 */
	//Prm->dobjgn = TUNE_DOBJGN; /* Pn125 */
	TunePrmWriteRam( &AccessPrm, &pndef_dtrqgn,  (LONG)(TUNE_DTRQGN), 2 );	/* Pn123 */
	TunePrmWriteRam( &AccessPrm, &pndef_dobjgn,  (LONG)(TUNE_DOBJGN), 2 );	/* Pn125 */

	/* Pn408 */
	data = Prm->tfuncsw;
	data |= 0x1000;
	//Prm->tfuncsw = data;
	TunePrmWriteRam( &AccessPrm, &pndef_tfuncsw, (LONG)(data), 2 );			/* Pn408 */

	return;
}



/****************************************************************************************************/
/*																									*/
/*		A型制振制御設定処理																			*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		概要:	入力された周波数に対し、A型制振を有効とする。ダンピングゲインは固定値。				*/
/*																									*/
/*		arg	:	USHORT	freq			:	振動周波数		[Hz]									*/
/*				USHORT	auton_sw		:	自動調整SW		[-]										*/
/*																									*/
/*		out	:	SHORT	rc				:	0x01:正常設定完了										*/
/*											0x10:上下限異常											*/
/*											0x11:自動設定不可										*/
/*											0x12:パラメータ書き込み異常								*/
/*																									*/
/****************************************************************************************************/
//SHORT	TuneATypeResSet( USHORT freq, USHORT auton_sw, PRMDATA *Prm )
SHORT	TuneATypeResSet( USHORT freq, USHORT auton_sw, PRMDATA *Prm, REGIF_CONFIG_T *RegCfg )/*<S09E>*/
{
	SHORT		rc;
	USHORT		data;
	REG_ACC_T	AccessPrm;

	/*------------------------------------------------------------------*/
	/* RAM書き込み用レジスタ情報設定									*/
	/*------------------------------------------------------------------*/
	AccessPrm.MemAddr  = 0;
	AccessPrm.hRegCfg  = RegCfg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

	/*------------------------------------------------------------------*/
	/*	自動設定可能チェック											*/
	/*------------------------------------------------------------------*/
	data = Prm->avibsw;
	if( ((data & 0x0001) == 0x0001) || (auton_sw == FALSE) )
	{	/* 既にA型制振有効 or 自動調整無効 */
		rc = 0x11;
		return	rc;
	}

	/*------------------------------------------------------------------*/
	/*	周波数上下限チェック											*/
	/*------------------------------------------------------------------*/
	if( (freq >= AVIB_FRQ_MIN) && (freq <= AVIB_FRQ_MAX) )
	{
		/*--------------------------------------------------------------*/
		/*	A型制振パラメータ設定										*/
		/*--------------------------------------------------------------*/
		//Prm->avibfrq = freq * 10;	/* Pn161 */
		//Prm->avibdamp = AVIBDAMPGN; /* Pn163 */
		TunePrmWriteRam( &AccessPrm, &pndef_avibfrq,  (LONG)(freq * 10), 2 );	/* Pn161 */
		TunePrmWriteRam( &AccessPrm, &pndef_avibdamp, (LONG)AVIBDAMPGN,  2 );	/* Pn163 */

		/*--------------------------------------------------------------*/
		/*	A型制振機能スイッチ設定										*/
		/*--------------------------------------------------------------*/
		data = Prm->avibsw; /* Pn160 */
		data |= 0x0001;
		//Prm->avibsw = data;
		TunePrmWriteRam( &AccessPrm, &pndef_avibsw, (LONG)data,  2 );
		rc = 0x01;
	}
	else
	{
		rc = 0x10;
	}

	return	rc;
}



/****************************************************************************************************/
/*																									*/
/*		ノッチフィルタ設定処理																		*/
/*																									*/
/****************************************************************************************************/
/*		概要:	第1、第2ノッチフィルタを設定する。ノッチQ値は自動的に計算される。					*/
/****************************************************************************************************/
SHORT	TuneNotchFilterSetting( AUTONOTCH *ANotch, PRMDATA *Prm, REGIF_CONFIG_T *RegCfg )
{
	CHAR			NotchType;
	SHORT			rc;
	USHORT			NotchQ;
	USHORT			freq;
	USHORT			KvGain;
	USHORT			Jrate;
	REG_ACC_T		AccessPrm;
	AUTONOTCH_DATA	ANotchWk;
	AUTONOTCH_PRM	*ANotchPrm;

	NotchType = -1;
	rc = 0x00;
	ANotchPrm = &(ANotch->ANotchPrm);
	freq = ANotch->ANotchData.conf.freq;
	KvGain = ANotch->ANotchData.conf.KvGain;
	Jrate = ANotch->ANotchData.conf.Jrate;

	/* RAM書き込み用レジスタ情報設定 */
	AccessPrm.MemAddr = 0;
	AccessPrm.hRegCfg = RegCfg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

	/* 現在のRAM情報読み出し */
	ANotchWk.var.NotchSw = Prm->tfuncsw;	/* Pn408 */
	ANotchWk.var.NotchQ1 = Prm->ntchq1;		/* Pn40A */
	ANotchWk.var.NotchQ2 = Prm->ntchq2;		/* Pn40D */
	ANotchWk.var.NotchFil1 = Prm->ntchfil1;	/* Pn409 */
	ANotchWk.var.NotchFil2 = Prm->ntchfil2;	/* Pn40C */
	ANotchWk.var.TrqFil = Prm->trqfil11;	/* Pn401 */

	/* ノッチタイプ設定 */
	if( ((ANotchWk.var.NotchSw & 0x0F00) == 0x0000) && ((Prm->anotchsw & 0x0F00) == 0x0100) )
	{	/* 2段目ノッチが自動設定可能 */
		if( (ANotchWk.var.NotchSw & 0x000F) == 0x0000 )
		{	/* 1段目ノッチ未使用 -> 2段目ノッチを1つ目として入れる */
			NotchType = 0;
		}
		else
		{	/* 1段目ノッチ使用中 -> 2段目ノッチを2つ目として入れる */
			if( freq > ANotchWk.var.NotchFil1 )
			{
				NotchType = 1;
			}
			else
			{
				NotchType = 4;
			}
		}
	}
	else
	{
		if( ((ANotchWk.var.NotchSw & 0x000F) == 0x0000) && ((Prm->anotchsw & 0x000F) == 0x0001) )
		{ /* 1段目ノッチが自動設定可能 */
			if( (ANotchWk.var.NotchSw & 0x0F00) == 0x0000 )
			{	/* 2段目ノッチ未使用 -> 1段目ノッチを1つ目として入れる */
				NotchType = 3;
			}
			else
			{	/* 2段目ノッチ使用中 -> 1段目ノッチを2つ目として入れる */
				if( freq > ANotchWk.var.NotchFil2 )
				{
					NotchType = 2;
				}
				else
				{
					NotchType = 5;
				}
			}
		}
	}

	/*----------------------------------------------------------------------------------*/
	/*	ノッチフィルタ設定 自動Q値計算													*/
	/*----------------------------------------------------------------------------------*/
	switch ( NotchType )
	{
		case 0:
		case 4:
			/*------------------------------------------------------------------------------*/
			/*	第2ノッチ（低い周波数）設定													*/
			/*------------------------------------------------------------------------------*/
			if( freq > pndef_ntchfil2.UpperLimit )
			{ /* 上限値を超える場合は上限値とする */
				freq = (USHORT)pndef_ntchfil2.UpperLimit;
			}
			/*------------------------------------------------------------------------------*/
			/*	Q値計算 & パラメータ計算													*/
			/*------------------------------------------------------------------------------*/
			if( TuneNotchQset( ANotchPrm, KvGain, Jrate, freq, TUNE_ANTFRQ_NTQ2, &NotchQ ) != OK )
			{ /* Q値設定異常の場合は、設定値を元に戻しノッチ完了する */
				/* 2段目ノッチQ値異常 */
				rc = 0x08;
			}
			else
			{	/* Q値計算正常終了 */
				ANotchWk.var.TrqFil = TuneNotchTfcalc( ANotchPrm, KvGain, Jrate, freq, NotchQ );
				/* 2桁目を1にする */
				ANotchWk.var.NotchSw |= 0x0100;
				/* ノッチ周波数 */
				ANotchWk.var.NotchFil2 = freq;
				/* ノッチQ値 */
				ANotchWk.var.NotchQ2 = NotchQ;

				if( NotchType == 4 )
				{ /* 1段目ノッチのQ値更新 */
					TuneNotchQset2( ANotchWk.var.NotchFil2, ANotchWk.var.NotchFil1, &NotchQ );
					/* ノッチQ値 */
					ANotchWk.var.NotchQ1 = NotchQ;
				}

				ANotchWk.var.TrqFil = TunePrmLimitUnsigned( (ULONG)ANotchWk.var.TrqFil, &pndef_trqfil11 );

				/* Pn401 */
				TunePrmWriteRam( &AccessPrm, &pndef_trqfil11, (LONG)ANotchWk.var.TrqFil, 2 );
				/* Pn408 */
				TunePrmWriteRam( &AccessPrm, &pndef_tfuncsw, (LONG)ANotchWk.var.NotchSw, 2 );
				/* Pn40A */
				TunePrmWriteRam( &AccessPrm, &pndef_ntchq1, (LONG)ANotchWk.var.NotchQ1, 2 );
				/* Pn40C */
				TunePrmWriteRam( &AccessPrm, &pndef_ntchfil2, (LONG)ANotchWk.var.NotchFil2, 2 );
				/* Pn40D */
				TunePrmWriteRam( &AccessPrm, &pndef_ntchq2, (LONG)ANotchWk.var.NotchQ2, 2 );

				/* 2段目ノッチ正常完了 */
				rc = 0x02;
			}
			break;

		case 1:
			/*------------------------------------------------------------------------------*/
			/*	第2ノッチ（高い周波数）設定													*/
			/*------------------------------------------------------------------------------*/
			if( freq > pndef_ntchfil2.UpperLimit )
			{ /* 上限値でのリミット */
				freq = (USHORT)pndef_ntchfil2.UpperLimit;
			}
			/*------------------------------------------------------------------------------*/
			/*	Q値計算 & パラメータ計算													*/
			/*------------------------------------------------------------------------------*/
			if( TuneNotchQset2( ANotchWk.var.NotchFil1, freq, &NotchQ ) != OK )
			{ /* Q値設定異常の場合は、設定値を元に戻しノッチ完了する */
				/* 2段目ノッチQ値異常 */
				rc = 0x08;
			}
			else
			{ /* Q値計算正常終了 */
				/* 2桁目を1にする */
				ANotchWk.var.NotchSw |= 0x0100;
				/* ノッチ周波数 */
				ANotchWk.var.NotchFil2 = freq;
				/* ノッチQ値 */
				ANotchWk.var.NotchQ2 = NotchQ;

				/* Pn408 */
				TunePrmWriteRam( &AccessPrm, &pndef_tfuncsw, (LONG)ANotchWk.var.NotchSw, 2 );
				/* Pn40C */
				TunePrmWriteRam( &AccessPrm, &pndef_ntchfil2, (LONG)ANotchWk.var.NotchFil2, 2 );
				/* Pn40D */
				TunePrmWriteRam( &AccessPrm, &pndef_ntchq2, (LONG)ANotchWk.var.NotchQ2, 2 );

				/* 2段目ノッチ正常完了 */
				rc = 0x02;
			}
			break;

		case 2:
			/*------------------------------------------------------------------------------*/
			/*	第1ノッチ（高い周波数）設定													*/
			/*------------------------------------------------------------------------------*/
			if( freq > pndef_ntchfil1.UpperLimit )
			{ /* 上限値でのリミット */
				freq = (USHORT)pndef_ntchfil1.UpperLimit;
			}
			/*------------------------------------------------------------------------------*/
			/*	Q値計算 & パラメータ計算													*/
			/*------------------------------------------------------------------------------*/
			if( TuneNotchQset2( ANotchWk.var.NotchFil2, freq, &NotchQ ) != OK )
			{ /* Q値設定異常の場合は、設定値を元に戻しノッチ完了する */
				/* 1段目ノッチQ値異常 */
				rc = 0x04;
			}
			else
			{ /* Q値計算正常終了 */
				/* ０桁目を1にする */
				ANotchWk.var.NotchSw |= 0x0001;
				/* ノッチ周波数 */
				ANotchWk.var.NotchFil1 = freq;
				/* ノッチQ値 */
				ANotchWk.var.NotchQ1 = NotchQ;

				/* Pn408 */
				TunePrmWriteRam( &AccessPrm, &pndef_tfuncsw, (LONG)ANotchWk.var.NotchSw, 2 );
				/* Pn409 */
				TunePrmWriteRam( &AccessPrm, &pndef_ntchfil1, (LONG)ANotchWk.var.NotchFil1, 2 );
				/* Pn40A */
				TunePrmWriteRam( &AccessPrm, &pndef_ntchq1, (LONG)ANotchWk.var.NotchQ1, 2 );

				/* 1段目ノッチ正常完了 */
				rc = 0x01;
			}
			break;

		case 3:
		case 5:
			/*------------------------------------------------------------------------------*/
			/*	第1ノッチ（低い周波数）設定													*/
			/*------------------------------------------------------------------------------*/
			if( freq > pndef_ntchfil1.UpperLimit )
			{ /* 上限値でのリミット */
				freq = (USHORT)pndef_ntchfil1.UpperLimit;
			}
			/*------------------------------------------------------------------------------*/
			/*	Q値計算 & パラメータ計算													*/
			/*------------------------------------------------------------------------------*/
			if( TuneNotchQset( ANotchPrm, KvGain, Jrate, freq, TUNE_ANTFRQ_NTQ1, &NotchQ ) != OK )
			{ /* Q値設定異常の場合は、設定値を元に戻しノッチ完了する */
				/* 1段目ノッチQ値異常 */
				rc = 0x04;
			}
			else
			{ /* Q値計算正常終了 */
				ANotchWk.var.TrqFil = TuneNotchTfcalc( ANotchPrm, KvGain, Jrate, freq, NotchQ );
				/* 0桁目を1にする */
				ANotchWk.var.NotchSw |= 0x0001;
				/* ノッチ周波数 */
				ANotchWk.var.NotchFil1 = freq;
				/* ノッチQ値 */
				ANotchWk.var.NotchQ1 = NotchQ;

				if( NotchType == 5 )
				{ /* 2段目ノッチのQ値更新 */
					TuneNotchQset2( ANotchWk.var.NotchFil1, ANotchWk.var.NotchFil2, &NotchQ );
					/* ノッチQ値 */
					ANotchWk.var.NotchQ2 = NotchQ;
				}

				ANotchWk.var.TrqFil = TunePrmLimitUnsigned( (ULONG)ANotchWk.var.TrqFil, &pndef_trqfil11 );

				/* Pn401 */
				TunePrmWriteRam( &AccessPrm, &pndef_trqfil11, (LONG)ANotchWk.var.TrqFil, 2 );
				/* Pn408 */
				TunePrmWriteRam( &AccessPrm, &pndef_tfuncsw, (LONG)ANotchWk.var.NotchSw, 2 );
				/* Pn409 */
				TunePrmWriteRam( &AccessPrm, &pndef_ntchfil1, (LONG)ANotchWk.var.NotchFil1, 2 );
				/* Pn40A */
				TunePrmWriteRam( &AccessPrm, &pndef_ntchq1, (LONG)ANotchWk.var.NotchQ1, 2 );
				/* Pn40D */
				TunePrmWriteRam( &AccessPrm, &pndef_ntchq2, (LONG)ANotchWk.var.NotchQ2, 2 );

				/* 1段目ノッチ正常完了 */
				rc = 0x01;
			}
			break;

		default:
			/*------------------------------------------------------------------------------*/
			/* 1段目、2段目が既に有効の時 or 自動ノッチOFF									*/
			/*	→ 何もしない																*/
			/*------------------------------------------------------------------------------*/
			rc = 0x10;									/* 条件不満足						*/
			break;
	}
	return rc;
}



/****************************************************************************************************/
/*																									*/
/*			自動ノッチ用サーボ制御トレースコントロール（Round)										*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機能：	自動ノッチの振動解析に使用するデータをトレースによりサンプリングする。					*/
/*				①トレース初期化要求がある場合														*/
/*						ScanBへトレースを無効に通知													*/
/*						ScanCへトレース初期化完了を通知												*/
/*				②サーボ制御トレース実行要求のチェック												*/
/*					ScanCからのトレース実行要求の立ち上がりエッジを受けて以下の処理を行う。			*/
/*						ScanBのトレース設定を行う													*/
/*						ScanCへトレース開始完了を通知												*/
/*				③トレース完了のチェック															*/
/*					ScanBからのトレース完了を受けて以下の処理を行う。								*/
/*						ScanCへトレース完了を通知													*/
/*																									*/
/*			※本処理で得たサンプリングデータはこの後直ぐに振動解析で使用される。					*/
/*			  振動解析もRoundで行う。																*/
/*																									*/
/****************************************************************************************************/
SHORT	TuneVibrationTrace( DETVIB *DetVib, FFTANALYZE *FftAna, 
								UCHAR TraceRequest, SHORT TrgLvl, BOOL TuneLessSts )
{
	SHORT	rc;
	SHORT	status;
	SHORT	*DataAdr;

	rc = TUNE_FREQ_SEARCH;

	/* サーボ制御トレース初期化要求チェック */
	if( TraceRequest == FALSE )
	{
		/* サーボ制御トレース強制中断 */
		DetVibTraceStopReq( DetVib );
		rc = TUNE_FREQ_SEARCHINIT;
	}
	else
	{
		if( DetVibTraceGetStatus(DetVib) == DETVIBTRC_TRCEND )
		{
			/* トレースバッファアドレス取得 */
			DataAdr = DetVibTraceGetDataAdr( DetVib );

			/* 振動周波数解析用データ設定 */
//			FftSetAnaCondition( FftAna, 200, 2000, 80, TuneLessSts );	/* <S21A> */
			FftSetAnaCondition( FftAna, 200, 4000, 80, TuneLessSts );	/* <S21A> */

			/* 振動周波数解析 */
			status = FftAnalyzeExecute( FftAna, DataAdr );

			if( status != OK )
			{ /* 解析エラー */
				rc = TUNE_FREQ_SEARCHERR;
			}
			else
			{ /* 周波数解析正常終了 */
				rc = TUNE_FREQ_SEARCHEND;
			}
		}
		else if( DetVibTraceGetIndex(DetVib) == 0 )
		{ /* サーボ制御用トレース実行可能 */
			/*----------------------------------------------------------------------------------*/
			/*	サーボ制御トレース開始未完時、トレース設定し起動する							*/
			/*----------------------------------------------------------------------------------*/
			/* トレース設定(データ数:200, ScanB周期) */
			DetVibTraceSetCondition( DetVib, 200, 1, TrgLvl );
			/* トレース開始要求 */
			DetVibTraceStartReq( DetVib );
		}
	}
	return rc;
}



/****************************************************************************************************/
/*																									*/
/*		二慣性ＭＦＣ有効時のKpm設定																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		概要:	Pn11B設定の二次関数でKpmを算出する。												*/
/*				二次関数：Kpm[0.1s-1] = -0.0036 * Pn11B^2 + 3.8647 * Pn11B - 28.285					*/
/*					Pn11B =  1.0[Hz] → Kpm =   1.0[s-1]											*/
/*					Pn11B = 20.0[Hz] → Kpm =  60.0[s-1]											*/
/*					Pn11B = 50.0[Hz] → Kpm = 100.0[s-1]											*/
/*				となる。ただし、Pn11B > 50.0[Hz]はKpm = 100.0[s-1]でクランプする。					*/
/*																									*/
/****************************************************************************************************/
USHORT	TuneMfcKpmCalcAntFreq( USHORT AntFreq )
{
	LONG	wk, wk1, wk2, wk3;

	if( AntFreq > 500 )
	{ /* 周波数が50.0Hzより大きい場合 */
		/* モデルループゲインは100.0[s-1]固定 */
		wk = 1000;
	}
	else
	{ /* 周波数が50.0Hz以下の場合 */
		/*----------------------------------------------------------------------*/
		/* 注意）本二次関数は、周波数<50.0でのみ使用可。						*/
		/*		50Hzを超える場合は、Kpm値が徐々に小さくなる						*/
		/*----------------------------------------------------------------------*/
		/*	二次係数  362 = 0.0036 * 10000										*/
		/*----------------------------------------------------------------------*/
		wk1 = (LONG)AntFreq * (LONG)AntFreq * 36;

		/*----------------------------------------------------------------------*/
		/*	一次係数  38647 = 3.8647 * 10000									*/
		/*----------------------------------------------------------------------*/
		wk2 = (LONG)AntFreq * 38647;
		wk3 = -wk1 + wk2 - 282850;			/* 282850 = 28.285 *10000			*/
		wk = wk3 / 10000;
	}

	return (USHORT)wk;
}



/****************************************************************************************************/
/*																									*/
/*		ノッチフィルタ設定用入力情報設定API															*/
/*																									*/
/****************************************************************************************************/
/*		概要:	第1、第2ノッチフィルタを設定する。ノッチQ値は自動的に計算される。					*/
/****************************************************************************************************/
void	TuneSetNotchFilSettingData( AUTONOTCH *ANotch, USHORT freq, USHORT KvGain, USHORT Jrate )
{
	ANotch->ANotchData.conf.freq = freq;		/* 自動ノッチ周波数									*/
	ANotch->ANotchData.conf.KvGain = KvGain;	/* Tf, ノッチQ値算出用ゲイン						*/
	ANotch->ANotchData.conf.Jrate = Jrate;		/* Tf, ノッチQ値算出用負荷慣性モーメント			*/

	return;
}



/***************************************** end of file **********************************************/
