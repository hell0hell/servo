/****************************************************************************************************/
/*																									*/
/*																									*/
/*		PowerMonitor.c : 消費電力演算処理定義		<S020>											*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 消費電力モニタ用演算処理を行う。														*/
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
/*	Note	:	初版	2013.10.7	Y.Oka	For Mercury												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "PowerManager.h"



/****************************************************************************************************/
/*		ロス演算用係数																				*/
/****************************************************************************************************/
#define		LOS_SLOPE1		166.0F						/* 0.162 * 1024								*/
#define		LOS_SLOPE2		4915.0F						/* 4.800 * 1024								*/
/****************************************************************************************************/
/*		ScanB周期の消費電力を平均化する換算係数		<S039>											*/
/****************************************************************************************************/
#define		CONVERT_AVR		KPI_SBCYCLEUS / KPI_SCCYCLEUS
														/* ScanC周期 / ScanB周期					*/
/****************************************************************************************************/
/*		ScanC周期当たりの消費電力量[Wh]換算係数(平均化含む)											*/
/****************************************************************************************************/
#define		CONVERT_WH		KPI_SCCYCLEMS / 60.0F / 60.0F / 1000.0F
														/* [h]→[2ms]								*/
/****************************************************************************************************/
/*		制御電源による消費電力量[W]																	*/
/****************************************************************************************************/
//#define		ONLY_CTRLLOSS	504 / MAX_AXIS_NUMBER	/* 軸数回足されるためあらかじめ割っておく	*//* <S039> */
//														/* 1分間で0.28[Wh]となる値					*/
#define		ONLY_CTRLLOSS	17 / MAX_AXIS_NUMBER		/* カタログ記載の制御電源消費電力			*//* <S039> */
														/* 軸数回足されるためあらかじめ割っておく	*/


/****************************************************************************************************/
/*																									*/
/*		消費電力演算定数演算処理																	*/
/*																									*/
/****************************************************************************************************/
void	PrmCalcPowerMonitor( CALC_WATTAGE *CalcWattage, USHORT AmpImax, USHORT MotImax )
{
	CalcWattage->conf.CurrentNorm1 = 1.0f / 800000.0F;		/* (1 / 3125) >> 8						*/
	CalcWattage->conf.CurrentNorm2 = 1.0f / 5625.0F;		/* (1 / 1875) >> 3						*/
	CalcWattage->conf.CurrentNorm3 = 1.0f / 1024.0F;		/* 1 >> 10								*/
	CalcWattage->conf.Imax = ( MotImax <= AmpImax ) ? MotImax : AmpImax;
	return;
}



/****************************************************************************************************/
/*																									*/
/*		消費電力演算定数演算処理																	*/
/*																									*/
/****************************************************************************************************/
void	InitializePowerMonitor( POWER_MONITOR *PowMon )
{
	MlibResetLongMemory( &PowMon->var, sizeof(PowMon->var)/4 );
	return;
}



/****************************************************************************************************/
/*																									*/
/*		消費電力演算処理																			*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		消費電力モニタ出力演算(情報落ち回避)								< POWER_MONITOR >		*/
/*																									*/
/*				   Vq * Vdc		Iqd * Imax	   Vd * Vdc	    Idd * Imax								*/
/*		PowTmp5 = ---------- * ------------ + ---------- * ------------								*/
/*					 2^5		   2^5			 2^5		   2^5									*/
/*						 PowTmp5		↑V,Iの計算で情報落ちの無いようシフト量を変更				*/
/*		PowTmp1 =	  ------------																	*/
/*					   3125 * 2^8																	*/
/*							   Iqd * Imax					 Iqd * Imax								*/
/*		PowTmp4 = Losslp1 * ( ------------ )^2 + Losslp2 *  ------------ 	：アンプ内部損失分		*/
/*								 15000						   15000								*/
/*		PowMonPower = PowTmp1 + PowTmp4		：出力側消費電力量 + アンプ内部損失						*/
/*																									*/
/****************************************************************************************************/
//void	CalcPowerMoniter( CALC_WATTAGE *CalcWattage, USHORT PowMonVq, USHORT PowMonIqd,				  /* <S039> */
//									USHORT PowMonVd, USHORT PowMonIdd, LONG DcVolt, BOOL BaseEnable )
void	CalcPowerMoniter( CALC_WATTAGE *CalcWattage, SHORT PowMonVq, SHORT PowMonIqd,
									SHORT PowMonVd, SHORT PowMonIdd, LONG DcVolt, BOOL BaseEnable )
{
	LONG	PowTmpL1, PowTmpL2, PowTmpL3;
	float	PowTmpF1/*, PowTmpF2, PowTmpF3*/;

	if( BaseEnable == FALSE )
	{
		PowMonVq = 0;
		PowMonIqd = 0;
		PowMonVd = 0;
		PowMonIdd = 0;
	}

	PowTmpL1 = ( PowMonVq * DcVolt ) >> 5;
	PowTmpL2 = ( PowMonIqd * CalcWattage->conf.Imax ) >> 5;
	PowTmpL3 = PowTmpL1 * PowTmpL2;

	PowTmpL1 = ( PowMonVd * DcVolt ) >> 5;
	PowTmpL2 = ( PowMonIdd * CalcWattage->conf.Imax ) >> 5;
	PowTmpL3 += ( PowTmpL1 * PowTmpL2 );

	/* 電流値正規化：[0.1A]/15000, 3相分換算:3/2 */
	PowTmpF1 = (float)PowTmpL3 * CalcWattage->conf.CurrentNorm1;
	if( PowTmpF1 < 0 ) /* <S039> */
	{
		PowTmpF1 = 0;
	}

	/* Iqd * Imax / 15000 */
	/* @@PowMonIqdは絶対値にする必要がある？？ */
//	PowTmpF2 = (float)PowMonIqd * (float)CalcWattage->conf.Imax * CalcWattage->conf.CurrentNorm2;
//	PowTmpF3 = (( LOS_SLOPE1 * PowTmpF2 * PowTmpF2 ) + ( LOS_SLOPE2 * PowTmpF2 )) * CalcWattage->conf.CurrentNorm3;

	/* 出力消費電力 + アンプ内損失 *//* @@平均値の方がよい */
#if 0 /* アンプ内損失は実機確認後に適用する *//* <S039> */
	CalcWattage->var.PowerConsumption += PowTmpF1 + PowTmpF3;
#else /* <S039> */
	CalcWattage->var.PowerConsumption += PowTmpF1;
#endif /* <S039> */

	/* 制御電源消費分加算 */
	CalcWattage->var.PowerConsumption += ONLY_CTRLLOSS;

	return;
}



/****************************************************************************************************/
/*																									*/
/*		消費電力モニタ設定処理																		*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Un032 : 消費電力[W]																			*/
/*		Un033 : 消費電力量[Wh]																		*/
/*		Un034 : 累積消費電力量(整数部)[Wh]															*/
/*		Un035 : 累積消費電力量(小数部)[0.001Wh]														*/
/*--------------------------------------------------------------------------------------------------*/
void	SetPowerMoniter( POWER_MONITOR *PowMon, float PowerConsumption )
{
	/* ScanC周期当たりの消費電力量計算 */
	if( PowerConsumption < 0.0f ) /* <S039> */
	{ /* 負の場合は0とする */
		PowerConsumption = 0.0f;
	}
	PowMon->var.CumulativeEnergy += (PowerConsumption * CONVERT_WH);

	if( KlibGetLongTimerMs(PowMon->var.TimerCnt1s) > 1000 )
	{
		/* Un032 : 消費電力[W] */
		PowMon->var.UnConsumedPower = (LONG)PowerConsumption;
		/* Un033 : 消費電力量[Wh] */
		PowMon->var.UnEnergyConsumption = (LONG)PowMon->var.CumulativeEnergy;
		/* 制御電源投入からの消費電力量計算 */
		PowMon->var.PonCumulativeEnergy += PowMon->var.CumulativeEnergy;

		PowMon->var.CumulativeEnergy = 0.0f;
		KlibRstLongTimer( &PowMon->var.TimerCnt1s );
	}

	if( KlibGetLongTimerMs(PowMon->var.TimerCnt10s) > 10000 )
	{
		if( PowMon->var.PonCumulativeEnergy > LONG_MAX )
		{ /* オーバーフロー判定 */
			PowMon->var.PonCumulativeEnergy -= LONG_MAX;
		}
		/* Un034 : 累積消費電力量(整数部)[Wh] */
		PowMon->var.UnCumulativeEnergyI = (LONG)PowMon->var.PonCumulativeEnergy;
		/* Un035 : 累積消費電力量(小数部)[0.001Wh] */
		PowMon->var.UnCumulativeEnergyD =
				(PowMon->var.PonCumulativeEnergy - PowMon->var.UnCumulativeEnergyI) * 1000; /* <S0A5> */

		KlibRstLongTimer( &PowMon->var.TimerCnt10s );
	}
	return;
}



/****************************************************************************************************/
/*																									*/
/*		消費電力モニタ平均化用API	<S039>															*/
/*																									*/
/****************************************************************************************************/
void	CalcPowerConsumptionAvr( CALC_WATTAGE *CalcWattage )
{
	CalcWattage->var.PowerConsumption_a = CalcWattage->var.PowerConsumption * (float)CONVERT_AVR;
	return;
}



/****************************************************************************************************/
/*																									*/
/*		多軸演算用消費電力初期化用API																*/
/*																									*/
/****************************************************************************************************/
void	ResetPowerConsumption( CALC_WATTAGE *CalcWattage )
{
	CalcWattage->var.PowerConsumption = 0;
	return;
}



/****************************************************************************************************/
/*																									*/
/*		消費電力取得用API																			*/
/*																									*/
/****************************************************************************************************/
float	GetPowerConsumption( CALC_WATTAGE *CalcWattage )
{
//	return	(CalcWattage->var.PowerConsumption); /* <S039> */
	return	(CalcWattage->var.PowerConsumption_a);
}


/***************************************** end of file **********************************************/
