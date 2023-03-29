/****************************************************************************************************/
/*																									*/
/*																									*/
/*		PowerMonitor.c : ����d�͉��Z������`		<S020>											*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : ����d�̓��j�^�p���Z�������s���B														*/
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
/*	Note	:	����	2013.10.7	Y.Oka	For Mercury												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "PowerManager.h"



/****************************************************************************************************/
/*		���X���Z�p�W��																				*/
/****************************************************************************************************/
#define		LOS_SLOPE1		166.0F						/* 0.162 * 1024								*/
#define		LOS_SLOPE2		4915.0F						/* 4.800 * 1024								*/
/****************************************************************************************************/
/*		ScanB�����̏���d�͂𕽋ω����銷�Z�W��		<S039>											*/
/****************************************************************************************************/
#define		CONVERT_AVR		KPI_SBCYCLEUS / KPI_SCCYCLEUS
														/* ScanC���� / ScanB����					*/
/****************************************************************************************************/
/*		ScanC����������̏���d�͗�[Wh]���Z�W��(���ω��܂�)											*/
/****************************************************************************************************/
#define		CONVERT_WH		KPI_SCCYCLEMS / 60.0F / 60.0F / 1000.0F
														/* [h]��[2ms]								*/
/****************************************************************************************************/
/*		����d���ɂ�����d�͗�[W]																	*/
/****************************************************************************************************/
//#define		ONLY_CTRLLOSS	504 / MAX_AXIS_NUMBER	/* �����񑫂���邽�߂��炩���ߊ����Ă���	*//* <S039> */
//														/* 1���Ԃ�0.28[Wh]�ƂȂ�l					*/
#define		ONLY_CTRLLOSS	17 / MAX_AXIS_NUMBER		/* �J�^���O�L�ڂ̐���d������d��			*//* <S039> */
														/* �����񑫂���邽�߂��炩���ߊ����Ă���	*/


/****************************************************************************************************/
/*																									*/
/*		����d�͉��Z�萔���Z����																	*/
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
/*		����d�͉��Z�萔���Z����																	*/
/*																									*/
/****************************************************************************************************/
void	InitializePowerMonitor( POWER_MONITOR *PowMon )
{
	MlibResetLongMemory( &PowMon->var, sizeof(PowMon->var)/4 );
	return;
}



/****************************************************************************************************/
/*																									*/
/*		����d�͉��Z����																			*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		����d�̓��j�^�o�͉��Z(��񗎂����)								< POWER_MONITOR >		*/
/*																									*/
/*				   Vq * Vdc		Iqd * Imax	   Vd * Vdc	    Idd * Imax								*/
/*		PowTmp5 = ---------- * ------------ + ---------- * ------------								*/
/*					 2^5		   2^5			 2^5		   2^5									*/
/*						 PowTmp5		��V,I�̌v�Z�ŏ�񗎂��̖����悤�V�t�g�ʂ�ύX				*/
/*		PowTmp1 =	  ------------																	*/
/*					   3125 * 2^8																	*/
/*							   Iqd * Imax					 Iqd * Imax								*/
/*		PowTmp4 = Losslp1 * ( ------------ )^2 + Losslp2 *  ------------ 	�F�A���v����������		*/
/*								 15000						   15000								*/
/*		PowMonPower = PowTmp1 + PowTmp4		�F�o�͑�����d�͗� + �A���v��������						*/
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

	/* �d���l���K���F[0.1A]/15000, 3�������Z:3/2 */
	PowTmpF1 = (float)PowTmpL3 * CalcWattage->conf.CurrentNorm1;
	if( PowTmpF1 < 0 ) /* <S039> */
	{
		PowTmpF1 = 0;
	}

	/* Iqd * Imax / 15000 */
	/* @@PowMonIqd�͐�Βl�ɂ���K�v������H�H */
//	PowTmpF2 = (float)PowMonIqd * (float)CalcWattage->conf.Imax * CalcWattage->conf.CurrentNorm2;
//	PowTmpF3 = (( LOS_SLOPE1 * PowTmpF2 * PowTmpF2 ) + ( LOS_SLOPE2 * PowTmpF2 )) * CalcWattage->conf.CurrentNorm3;

	/* �o�͏���d�� + �A���v������ *//* @@���ϒl�̕����悢 */
#if 0 /* �A���v�������͎��@�m�F��ɓK�p���� *//* <S039> */
	CalcWattage->var.PowerConsumption += PowTmpF1 + PowTmpF3;
#else /* <S039> */
	CalcWattage->var.PowerConsumption += PowTmpF1;
#endif /* <S039> */

	/* ����d��������Z */
	CalcWattage->var.PowerConsumption += ONLY_CTRLLOSS;

	return;
}



/****************************************************************************************************/
/*																									*/
/*		����d�̓��j�^�ݒ菈��																		*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Un032 : ����d��[W]																			*/
/*		Un033 : ����d�͗�[Wh]																		*/
/*		Un034 : �ݐϏ���d�͗�(������)[Wh]															*/
/*		Un035 : �ݐϏ���d�͗�(������)[0.001Wh]														*/
/*--------------------------------------------------------------------------------------------------*/
void	SetPowerMoniter( POWER_MONITOR *PowMon, float PowerConsumption )
{
	/* ScanC����������̏���d�͗ʌv�Z */
	if( PowerConsumption < 0.0f ) /* <S039> */
	{ /* ���̏ꍇ��0�Ƃ��� */
		PowerConsumption = 0.0f;
	}
	PowMon->var.CumulativeEnergy += (PowerConsumption * CONVERT_WH);

	if( KlibGetLongTimerMs(PowMon->var.TimerCnt1s) > 1000 )
	{
		/* Un032 : ����d��[W] */
		PowMon->var.UnConsumedPower = (LONG)PowerConsumption;
		/* Un033 : ����d�͗�[Wh] */
		PowMon->var.UnEnergyConsumption = (LONG)PowMon->var.CumulativeEnergy;
		/* ����d����������̏���d�͗ʌv�Z */
		PowMon->var.PonCumulativeEnergy += PowMon->var.CumulativeEnergy;

		PowMon->var.CumulativeEnergy = 0.0f;
		KlibRstLongTimer( &PowMon->var.TimerCnt1s );
	}

	if( KlibGetLongTimerMs(PowMon->var.TimerCnt10s) > 10000 )
	{
		if( PowMon->var.PonCumulativeEnergy > LONG_MAX )
		{ /* �I�[�o�[�t���[���� */
			PowMon->var.PonCumulativeEnergy -= LONG_MAX;
		}
		/* Un034 : �ݐϏ���d�͗�(������)[Wh] */
		PowMon->var.UnCumulativeEnergyI = (LONG)PowMon->var.PonCumulativeEnergy;
		/* Un035 : �ݐϏ���d�͗�(������)[0.001Wh] */
		PowMon->var.UnCumulativeEnergyD =
				(PowMon->var.PonCumulativeEnergy - PowMon->var.UnCumulativeEnergyI) * 1000; /* <S0A5> */

		KlibRstLongTimer( &PowMon->var.TimerCnt10s );
	}
	return;
}



/****************************************************************************************************/
/*																									*/
/*		����d�̓��j�^���ω��pAPI	<S039>															*/
/*																									*/
/****************************************************************************************************/
void	CalcPowerConsumptionAvr( CALC_WATTAGE *CalcWattage )
{
	CalcWattage->var.PowerConsumption_a = CalcWattage->var.PowerConsumption * (float)CONVERT_AVR;
	return;
}



/****************************************************************************************************/
/*																									*/
/*		�������Z�p����d�͏������pAPI																*/
/*																									*/
/****************************************************************************************************/
void	ResetPowerConsumption( CALC_WATTAGE *CalcWattage )
{
	CalcWattage->var.PowerConsumption = 0;
	return;
}



/****************************************************************************************************/
/*																									*/
/*		����d�͎擾�pAPI																			*/
/*																									*/
/****************************************************************************************************/
float	GetPowerConsumption( CALC_WATTAGE *CalcWattage )
{
//	return	(CalcWattage->var.PowerConsumption); /* <S039> */
	return	(CalcWattage->var.PowerConsumption_a);
}


/***************************************** end of file **********************************************/
