/****************************************************************************************************/
/*																									*/
/*																									*/
/*		PcmdFilter.c : 位置指令フィルタ演算用	モジュール定義										*/
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
/*	Note	:	初版	2013.11.22	Y.Oka	For Mercury												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "PcmdFilter.h"
#include "Mlib.h"


/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
/*	void	PcmdFilInitialize( PCMDFIL *PcmdFil, VIBSUPFIL *VibSupFil );							*/
/*	LONG	PcmdFilRuntimeService( PCMDFIL *PcmdFil, LONG PcmdIn, LONG FilRest, BOOL FilStop );		*/
/*--------------------------------------------------------------------------------------------------*/
/*	void	PcmdFilCalculatePrmExpFilter( EXPFIL *ExpFil, USHORT PrmTime, KSGAIN KMotSpdConv,		*/
/*														 UCHAR MaxVel, LONG SvCycleUs, LONG Flag );	*/
/*	void	PcmdFilCalculatePrmExpFilBias( EXPFIL *ExpFil, USHORT expbias1,							*/
/*													LONG Egear_a, LONG Egear_b, LONG SvCycleUs );	*/
/*	void	PcmdFilCalculatePrmMoveAvergeFilter( MAFIL *MaFil, USHORT PrmTime, USHORT Highrad,		*/
/*																	LONG SvCycleUs, LONG Flag );	*//*<S0C3>*/
/*  void    PcmdFilCalculatePrmHighFilter( MAFIL *MaFil, USHORT PrmTime, USHORT Highrad,			*/
/*																	LONG SvCycleUs, LONG Flag );	*//*<S0C3>*/
/*--------------------------------------------------------------------------------------------------*/
/*	void	PcmdFilSetRefZStatus( PCMDFIL *PcmdFil, BOOL status );									*/
/*	BOOL	PcmdFilGetRefZStatus( PCMDFIL *PcmdFil );												*/
/*--------------------------------------------------------------------------------------------------*/
/*	LONG	PcmdFilVibSupFilter( VIBSUPFIL *VibSupFil, LONG dPcmd, BOOL *RefZSignal );				*/
/*	BOOL	PcmdFilCalculatePrmVibSupFilSW( VIBSUPFIL *VibSupFil, ULONG mdlsw,						*/
/*													ULONG ff_feq, ULONG ff_fil, LONG ScanTimeNs );	*/
/*	void	PcmdFilCalculatePrmVibSupFil( VIBSUPFIL *VibSupFil, ULONG ff_frq, ULONG ff_fil,			*/
/*																				LONG ScanTimeNs );	*/
/****************************************************************************************************/
static void	pcmdFilExpFilterPrmChange( EXPFIL *ExpFil, LONG PcmdIn, LONG FilRest );
static LONG	pcmdFilExpFilter( EXPFIL *ExpFil, LONG PcmdIn );
static LONG	pcmdFilMoveAverageFilter( MAFIL *MaFil, LONG PcmdIn );
static LONG	pcmdFilLongMAFilter( MAFIL *MaFil, LONG PcmdIn );
#if	(CSW_SERVO_PCMD_HIGH_FIL_USE == TRUE)/*<S0C3>*/
static LONG	pcmdFilHighFilter( MAFIL *MaFil, LONG PcmdIn );
static LONG	pcmdFilLongHFilter( MAFIL *MaFil, LONG PcmdIn );
#endif


#if( CSW_SERVO_PCMD_FIL_USE != 0 )
/****************************************************************************************************/
/*																									*/
/*		制振制御変数初期化																			*/
/*																									*/
/****************************************************************************************************/
void	PcmdFilInitialize( PCMDFIL *PcmdFil, VIBSUPFIL *VibSupFil )
{
	/* 指数加減速フィルタ用変数初期化 */
	MlibResetLongMemory( &(PcmdFil->ExpFil.var), sizeof(PcmdFil->ExpFil.var)/4 );
	PcmdFil->ExpFil.conf.Kf[0] = PcmdFil->ExpFil.conf.Kf[1];
	PcmdFil->ExpFil.conf.Pbias[0] = PcmdFil->ExpFil.conf.Pbias[1];

	/* 移動平均フィルタ用変数初期化 */
	MlibResetLongMemory( &(PcmdFil->MaFil.var), sizeof(PcmdFil->MaFil.var)/4 );
	PcmdFil->MaFil.conf.PmafNum[0] = PcmdFil->MaFil.conf.PmafNum[1];
	PcmdFil->MaFil.conf.ImafSft[0] = PcmdFil->MaFil.conf.ImafSft[1];

	/* high filter 用変数初期化*//*<S0C3>*/
	MlibResetLongMemory( &(PcmdFil->MaFil.var), sizeof(PcmdFil->MaFil.var)/4 );
	PcmdFil->MaFil.conf.PmafNumSec[0] = PcmdFil->MaFil.conf.PmafNumSec[1];
	PcmdFil->MaFil.conf.ImafSftSec[0] = PcmdFil->MaFil.conf.ImafSftSec[1];

	/* 振動抑制フィルタ用変数初期化 */
	MlibResetLongMemory( &VibSupFil->var, sizeof(VibSupFil->var)/4 );
	VibSupFil->conf.Pexe = VibSupFil->conf.VibSupPrm;

	return;
}
/* <S1D0> Start */
/*****************************************************************************
  Description: 速度フィードフォワード、トルクフィードフォワード移動平均機能用変数初期化
------------------------------------------------------------------------------
  Parameters:
	pMaFil;		移動平均フィルタ演算用データ
  Return:
	NONE
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
void	FFCmdFilInitialize( MAFIL *pMaFil )
{
	/* 移動平均フィルタ用変数初期化 */
	MlibResetLongMemory( &(pMaFil->var), sizeof(pMaFil->var)/4 );
	pMaFil->conf.PmafNum[0] = pMaFil->conf.PmafNum[1];
	pMaFil->conf.ImafSft[0] = pMaFil->conf.ImafSft[1];

	return;
}

/*****************************************************************************
  Description: 速度/トルクフィードフォワード指令移動平均フィルタ演算
------------------------------------------------------------------------------
  Parameters:
	pMaFil;		移動平均フィルタ演算用データ
	FFcmdIn		移動平均フィルタ前の速度/トルクフィードフォワード指令
	FilStop		速度/トルクフィードフォワード移動平均フィルタ無効
  Return:
	LONG		移動平均フィルタ後の速度/トルクフィードフォワード指令
------------------------------------------------------------------------------
  Note:
	機 能 : 速度/トルクフィードフォワード指令移動平均フィルタ処理を行う。
		1)移動平均時間 ≦ (MAFIL_BUFNUM * Cycle) : 移動平均フィルタ
		2)移動平均時間 ＞ (MAFIL_BUFNUM * Cycle) : 移動平均フィルタ ＋ 補間移動平均フィルタ
*****************************************************************************/
LONG	FFcmdFilMoveAverageFilter( MAFIL *pMaFil, LONG FFcmdIn, BOOL FilStop )
{
	/* <S1DF> Start */
	LONG SaveFFcmd;

	SaveFFcmd = FFcmdIn;
	/* 2＾24→15000 */
	FFcmdIn = MlibMulhigh32( FFcmdIn, 3840000 );
	/* <S1DF> End */
/*--------------------------------------------------------------------------------------------------*/
/*		移動平均フィルタ演算																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( pMaFil->conf.ImafSft[0] == 0 )
	{
		/* 移動平均フィルタ */
		pMaFil->var.Filo = MlibPcmdMafil( FFcmdIn,
										 pMaFil->conf.PmafNum[0],
										 &pMaFil->var.Pmafvar,
										 &pMaFil->var.Pmafbuf[0] );
	}
	else
	{
		/* 長時間移動平均フィルタ */
		pMaFil->var.Filo = pcmdFilLongMAFilter( pMaFil, FFcmdIn );
	}
	/* 15000→2＾24 *//* <S200> */
	pMaFil->var.Filo = MlibMulhigh32( (pMaFil->var.Filo<<16), 73300775 );		/* <S200> */
/*--------------------------------------------------------------------------------------------------*/
/*		パラメータ変更＆払出し完了信号処理															*/
/*--------------------------------------------------------------------------------------------------*/
	if ( FilStop == TRUE )
	{	/* 機能無効処理 */
//		pMaFil->var.Filo = FFcmdIn;		/* <S1DF> */
		pMaFil->var.Filo = SaveFFcmd;	/* <S1DF> */
	}
/*--------------------------------------------------------------------------------------------------*/
	/* 15000→2＾24 *//* <S1DF> */
//	pMaFil->var.Filo = MlibMulhigh32( (pMaFil->var.Filo<<16), 73300775 );		/* <S1DF> *//* <S200> */
	return( pMaFil->var.Filo );

}
/* <S1D0> End */


/****************************************************************************************************/
/*																									*/
/*		位置指令フィルタ処理メイン関数																*/
/*																									*/
/****************************************************************************************************/
LONG	PcmdFilRuntimeService( PCMDFIL *PcmdFil, LONG PcmdIn, LONG FilRest, BOOL FilStop )
{
	LONG	PcmdOut;
	LONG	OrgPcmdIn;

/*--------------------------------------------------------------------------------------------------*/
/*		位置指令フィルタ実行／停止処理																*/
/*--------------------------------------------------------------------------------------------------*/
	if ( FilStop == TRUE )
	{
		OrgPcmdIn = PcmdIn;
		PcmdIn = 0;
	}
	else
	{
		OrgPcmdIn = 0;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		位置指令フィルタ選択ユーザ定数によるフィルタ選択＆実行処理									*/
/*--------------------------------------------------------------------------------------------------*/
	/* 移動平均フィルタ演算 */
	PcmdOut = pcmdFilMoveAverageFilter( &(PcmdFil->MaFil), PcmdIn );

	/* high filter 演算 *//*<S0C3>*/
#if	(CSW_SERVO_PCMD_HIGH_FIL_USE == TRUE)
	PcmdOut = pcmdFilHighFilter( &(PcmdFil->MaFil), PcmdOut );
#endif

	/* 指数加減速フィルタ演算 */
	pcmdFilExpFilterPrmChange( &(PcmdFil->ExpFil), PcmdOut, FilRest );
	PcmdOut = pcmdFilExpFilter( &(PcmdFil->ExpFil), PcmdOut );

/*--------------------------------------------------------------------------------------------------*/
/*		フィルタ演算結果に実際の位置指令を加算														*/
/*--------------------------------------------------------------------------------------------------*/
	PcmdOut += OrgPcmdIn;

	return( PcmdOut );
}



/****************************************************************************************************/
/*																									*/
/*		位置指令指数加減速フィルタパラメータ変更処理												*/
/*																									*/
/****************************************************************************************************/
static void	pcmdFilExpFilterPrmChange( EXPFIL *ExpFil, LONG PcmdIn, LONG FilRest )
{
/*--------------------------------------------------------------------------------------------------*/
/*		パラメータ変更処理																			*/
/*--------------------------------------------------------------------------------------------------*/
	if ( (PcmdIn == 0) && (FilRest == 0) )
	{													/* 位置指令 = 0 && フィルタ溜まり = 0		*/
		ExpFil->conf.Kf[0] = ExpFil->conf.Kf[1];		/* パラメータの変更有効						*/
		ExpFil->conf.Pbias[0] = ExpFil->conf.Pbias[1];	/* パラメータの変更有効						*/
		if( ExpFil->conf.Kf[0] == 1 )					/* ゲイン1の時バイアス0に強制設定			*/
		{
			ExpFil->conf.Pbias[0] = 0;
		}
	}
//	else /* Σ-7にFilChgDisableを確認する処理がないため、削除 */
//	{
//		FilMng.FilChgDisable = TRUE;					/* フィルタ切替え禁止設定					*/
//	}
	/*--------------------------------------------------------------------------------------------------*/
	return;
}



/****************************************************************************************************/
/*																									*/
/*		位置指令指数加減速フィルタ演算																*/
/*																									*/
/****************************************************************************************************/
static LONG	pcmdFilExpFilter( EXPFIL *ExpFil, LONG PcmdIn )
{

/*--------------------------------------------------------------------------------------------------*/
/*		加減速フィルタ演算																			*/
/*--------------------------------------------------------------------------------------------------*/
	ExpFil->var.Pexfvar.sumx = ExpFil->var.Pexfvar.sumx + PcmdIn;
	if ( MlibAbs32( ExpFil->var.Pexfvar.sumx ) > ExpFil->conf.Pbias[0] )
	{
		ExpFil->var.Filo = MlibMulkxBiasrem( ExpFil->var.Pexfvar.sumx,
											 ExpFil->conf.Kf[0],
											 &ExpFil->var.Pexfvar.remx,
											 ExpFil->conf.Pbias[0] );
	}
	else
	{
		ExpFil->var.Filo = ExpFil->var.Pexfvar.sumx;
	}
	ExpFil->var.Pexfvar.sumx = ExpFil->var.Pexfvar.sumx - ExpFil->var.Filo;
/*--------------------------------------------------------------------------------------------------*/
/*		払出し完了信号処理																			*/
/*--------------------------------------------------------------------------------------------------*/
	if ( ( PcmdIn != 0 ) || ( ExpFil->var.Pexfvar.sumx != 0 ) )
	{
//		BoutV.f.Sv.Den = FALSE;							/* 位置指令払い出し未完了					*/
		ExpFil->var.RefZStatus = FALSE;					/* 位置指令払い出し未完了					*/
	}
/*--------------------------------------------------------------------------------------------------*/
	return( ExpFil->var.Filo );
}



/****************************************************************************************************/
/*																									*/
/*		位置指令移動平均フィルタ演算																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 位置指令移動平均フィルタ処理を行う。													*/
/*																									*/
/*			1)移動平均時間 ≦ (MAFIL_BUFNUM * Cycle) : 移動平均フィルタ								*/
/*			2)移動平均時間 ＞ (MAFIL_BUFNUM * Cycle) : 移動平均フィルタ ＋ 補間移動平均フィルタ		*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static LONG	pcmdFilMoveAverageFilter( MAFIL *MaFil, LONG PcmdIn )
{

/*--------------------------------------------------------------------------------------------------*/
/*		移動平均フィルタ演算																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( MaFil->conf.ImafSft[0] == 0 )
	{
		/* 移動平均フィルタ */
		MaFil->var.Filo = MlibPcmdMafil( PcmdIn,
										 MaFil->conf.PmafNum[0],
										 &MaFil->var.Pmafvar,
										 &MaFil->var.Pmafbuf[0] );
	}
	else
	{
		/* 長時間移動平均フィルタ */
		MaFil->var.Filo = pcmdFilLongMAFilter( MaFil, PcmdIn );
	}
/*--------------------------------------------------------------------------------------------------*/
/*		パラメータ変更＆払出し完了信号処理															*/
/*--------------------------------------------------------------------------------------------------*/
	if( (PcmdIn | MaFil->var.PcmdInX | MaFil->var.Pmafvar.zcntx | MaFil->var.Pimfvar.zcntx) == 0 )
	{
		MaFil->conf.PmafNum[0] = MaFil->conf.PmafNum[1];	/* 位置指令=0 && 払い出し完了で			*/
		MaFil->conf.ImafSft[0] = MaFil->conf.ImafSft[1];	/* パラメータの変更有効					*/
	}
	else
	{
//		BoutV.f.Sv.Den = FALSE;							/* 位置指令払い出し未完了					*/
		MaFil->var.RefZStatus = FALSE;					/* 位置指令払い出し未完了					*/
//		FilMng.FilChgDisable = TRUE;					/* フィルタ切替え禁止設定					*/
	}
/*--------------------------------------------------------------------------------------------------*/
	return( MaFil->var.Filo );

}

/****************************************************************************************************/
/*	                                                         位置指令 high filter 処理                                                                                                          *//*<S0C3>*/
/****************************************************************************************************/
#if	(CSW_SERVO_PCMD_HIGH_FIL_USE == TRUE)
static LONG	pcmdFilHighFilter( MAFIL *MaFil, LONG PcmdIn )
{

/*--------------------------------------------------------------------------------------------------*/
/*		移動平均フィルタ2演算																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( MaFil->conf.ImafSftSec[0] == 0 )
	{
		/* 移動平均フィルタ */
		MaFil->var.FiloSec = MlibPcmdMafilSec( PcmdIn,
										 MaFil->conf.PmafNumSec[0],
										 &MaFil->var.PmafvarSec,
										 &MaFil->var.PmafbufSec[0] );
	}
	else
	{
		/* 長時間移動平均フィルタ */
		MaFil->var.FiloSec = pcmdFilLongHFilter( MaFil, PcmdIn );
	}
/*--------------------------------------------------------------------------------------------------*/
/*		パラメータ変更＆払出し完了信号処理															*/
/*--------------------------------------------------------------------------------------------------*/
	if( (PcmdIn | MaFil->var.PcmdInXSec | MaFil->var.PmafvarSec.zcntxSec | MaFil->var.PimfvarSec.zcntxSec) == 0 )
	{
		MaFil->conf.PmafNumSec[0] = MaFil->conf.PmafNumSec[1];	/* 位置指令=0 && 払い出し完了で			*/
		MaFil->conf.ImafSftSec[0] = MaFil->conf.ImafSftSec[1];	/* パラメータの変更有効					*/
	}
	else
	{
//		BoutV.f.Sv.Den = FALSE;							/* 位置指令払い出し未完了					*/
		MaFil->var.RefZStatus = FALSE;					/* 位置指令払い出し未完了					*/
//		FilMng.FilChgDisable = TRUE;					/* フィルタ切替え禁止設定					*/
	}
/*--------------------------------------------------------------------------------------------------*/
	return( MaFil->var.FiloSec );

}
#endif


/****************************************************************************************************/
/*																									*/
/*		長時間移動平均フィルタ演算																	*/
/*																									*/
/****************************************************************************************************/
static LONG	pcmdFilLongMAFilter( MAFIL *MaFil, LONG PcmdIn )
{
	LONG	pcmdfilo;

/*--------------------------------------------------------------------------------------------------*/
/*		移動平均フィルタ : 出力開始まで、補間移動平均時間((1<<ImafSft)*Cycle)要する。				*/
/*--------------------------------------------------------------------------------------------------*/
	MaFil->var.PcmdInX += PcmdIn;
	if( ++MaFil->var.Index >= (1 << MaFil->conf.ImafSft[0]) )
	{
		MaFil->var.PcmdInY = MlibPcmdMafil( MaFil->var.PcmdInX,
											MaFil->conf.PmafNum[0],
											&MaFil->var.Pmafvar,
											&MaFil->var.Pmafbuf[0] );
	/*----------------------------------------------------------------------------------------------*/
		MaFil->var.Index   = 0;
		MaFil->var.PcmdInX = 0;
	}
/*--------------------------------------------------------------------------------------------------*/
/*		補間移動平均フィルタ																		*/
/*--------------------------------------------------------------------------------------------------*/
	pcmdfilo = MlibPcmdImafil( MaFil->var.PcmdInY,
							   MaFil->conf.ImafSft[0],
							   MaFil->var.Index,
							   &MaFil->var.Pimfvar );
/*--------------------------------------------------------------------------------------------------*/
	return( pcmdfilo );

}

/****************************************************************************************************/
/*		                                                        長時間移動平均フィルタ 演算				               			*/
/****************************************************************************************************//*<S0C3>*/
#if	(CSW_SERVO_PCMD_HIGH_FIL_USE == TRUE)
static LONG	pcmdFilLongHFilter( MAFIL *MaFil, LONG PcmdIn )
{
	LONG	pcmdfilo;

/*--------------------------------------------------------------------------------------------------*/
/*		                     移動平均フィルタ : 出力開始まで、補間移動平均時間((1<<ImafSft)*Cycle)要する。	     			*/
/*--------------------------------------------------------------------------------------------------*/
	MaFil->var.PcmdInXSec += PcmdIn;
	if( ++MaFil->var.IndexSec >= (1 << MaFil->conf.ImafSftSec[0]) )
	{
		MaFil->var.PcmdInYSec = MlibPcmdMafilSec( MaFil->var.PcmdInXSec,
												  MaFil->conf.PmafNumSec[0],
												  &MaFil->var.PmafvarSec,
												  &MaFil->var.PmafbufSec[0] );
	/*----------------------------------------------------------------------------------------------*/
			MaFil->var.IndexSec   = 0;
			MaFil->var.PcmdInXSec = 0;
	}
/*--------------------------------------------------------------------------------------------------*/
/*		        補間移動平均フィルタ																			*/
/*--------------------------------------------------------------------------------------------------*/
	pcmdfilo = MlibPcmdImafilSec( MaFil->var.PcmdInYSec,
								   MaFil->conf.ImafSftSec[0],
								   MaFil->var.IndexSec,
								   &MaFil->var.PimfvarSec );
/*--------------------------------------------------------------------------------------------------*/
	return( pcmdfilo );

}
#endif


/****************************************************************************************************/
/*																									*/
/*		位置指令加減速フィルタパラメータ計算														*/
/*																									*/
/****************************************************************************************************/
void	PcmdFilCalculatePrmExpFilter( EXPFIL *ExpFil, USHORT PrmTime, KSGAIN KMotSpdConv,
														UCHAR MaxVel, LONG SvCycleUs, LONG Flag )
{
	LONG	Unit;
	LONG	PrmTimeUs;
	LONG	kx,sx;

	Unit = ( !Flag ) ? 10 : 100;
	PrmTimeUs = PrmTime * Unit;

	/*----------------------------------------------------------------------------------------------*/
	/* kx(位置指令加減速フィルタの上限値)は制御周期によらない										*/
	/* 最高速度と演算ビット数(Σ-V/Σ-7の場合は31bit)に依存											*/
	/*----------------------------------------------------------------------------------------------*/
	kx = MlibScalKxkxks( SUMX_LIM, 100, KMotSpdConv, &sx, 0 );
	kx = MlibPcalKxgain( kx, 1, MaxVel*100, &sx, -24 );
	PrmTimeUs = ( kx < PrmTimeUs ) ? kx : PrmTimeUs ;
	/* <V507> add end */
	if( PrmTimeUs <= SvCycleUs )				/* 時定数 ≦ 制御周期までは、ゲイン＝１				*/
	{
		ExpFil->conf.Kf[1] = 1;
	}
	else										/* Kf = SvCycleUs / PrmTimeUsで近似計算				*/
	{
		ExpFil->conf.Kf[1] = MlibScalKxgain( SvCycleUs, 1, PrmTimeUs, NULL, 24 );
	}
/*--------------------------------------------------------------------------------------------------*/
	return;

}



/****************************************************************************************************/
/*																									*/
/*		位置指令加減速フィルタバイアスパラメータ計算												*/
/*																									*/
/****************************************************************************************************/
void	PcmdFilCalculatePrmExpFilBias( EXPFIL *ExpFil, USHORT expbias1,
														LONG Egear_a, LONG Egear_b, LONG SvCycleUs )
{
	LONG	kx;
	LONG	sx;

	kx = MlibScalKxgain( expbias1, Egear_b, Egear_a, &sx, 0 );
	kx = MlibPcalKxgain( kx, SvCycleUs, 1000000, &sx, 0 );
	ExpFil->conf.Pbias[1] = (sx<=31)? (kx>>sx) : 0;
	/*--------------------------------------------------------------------------------------------------*/
	return;

}



/****************************************************************************************************/
/*																									*/
/*		位置指令移動平均フィルタパラメータ計算														*/
/*																									*/
/****************************************************************************************************/
void	PcmdFilCalculatePrmMoveAvergeFilter( MAFIL *MaFil, USHORT PrmTime, USHORT Highrad, LONG SvCycleUs, LONG Flag )/*<S0C3>*/
{
	LONG	i,x;
	LONG	MaTimeUs;

#if	(CSW_SERVO_PCMD_HIGH_FIL_USE == TRUE)/*<S0C3>*/
	USHORT  rad; /*<S0C3>*/

	rad = Highrad>>1;
	MaTimeUs= (Flag==0)? (10*PrmTime-0.1*PrmTime*rad) : (100*PrmTime-PrmTime*rad);/* Flag=0:[0.01ms], Flag=1:[0.1ms]		*/
#else
	MaTimeUs= (Flag==0)? (10*PrmTime) : (100*PrmTime);		/* Flag=0:[0.01ms], Flag=1:[0.1ms]		*/
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		短時間フィルタ：通常移動平均フィルタ														*/
/*--------------------------------------------------------------------------------------------------*/
	if( MaTimeUs <= (MAFIL_BUFNUM * SvCycleUs) )
	{
		MaFil->conf.ImafSft[1] = 0;
		MaFil->conf.PmafNum[1] = MaTimeUs / SvCycleUs;
		if( MaFil->conf.PmafNum[1] < 1 ){ MaFil->conf.PmafNum[1] = 1;}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		長時間フィルタ：移動平均フィルタ＋補間移動平均フィルタ										*/
/*--------------------------------------------------------------------------------------------------*/
/*		最大の補間移動平均時間 = Cycle * (1<<imax) = 125us * 32 = 4ms								*/
/*		最大の合計移動平均時間 = 最大の補間移動平均時間 * MAFIL_BUFNUM = 4ms * 256 = 1024ms			*/
/*--------------------------------------------------------------------------------------------------*/
	else
	{
		x = MAFIL_BUFNUM * SvCycleUs;						/* 補間移動平均回数算出					*/
		for( i=1; i < 5; i++ )								/* 補間移動平均回数は、					*/
		{													/* (1<<i) = 2,4,8,...					*/
			if( (x << i) >= MaTimeUs ){ break;}				/*										*/
		}													/*										*/
	/*----------------------------------------------------------------------------------------------*/
		MaFil->conf.ImafSft[1] = (UCHAR)i;
		MaFil->conf.PmafNum[1] = MaTimeUs / (SvCycleUs << i);
		if( MaFil->conf.PmafNum[1] > MAFIL_BUFNUM ){ MaFil->conf.PmafNum[1] = MAFIL_BUFNUM;}
	}
/*--------------------------------------------------------------------------------------------------*/
	return;

}

#if	(CSW_SERVO_PCMD_HIGH_FIL_USE == TRUE)
/*-----------------------------------------------------------------------------------high filter---------*//*<S0C3>*/
void	PcmdFilCalculatePrmHighFilter( MAFIL *MaFil, USHORT PrmTime, USHORT Highrad, LONG SvCycleUs, LONG Flag )
{
	LONG	i,x;
	LONG	MaTimeUs;
	USHORT  rad;

    rad = Highrad>>1;
    MaTimeUs= (Flag==0)? (0.1*PrmTime*rad) : (PrmTime*rad);   /* Flag=0:[0.01ms], Flag=1:[0.1ms]		*/
/*--------------------------------------------------------------------------------------------------*/
/*		        短時間フィルタ：通常移動平均フィルタ                														*/
/*--------------------------------------------------------------------------------------------------*/
    if( MaTimeUs <= (MAFIL_BUFNUM * SvCycleUs) )
    {
    	MaFil->conf.ImafSftSec[1] = 0;
    	MaFil->conf.PmafNumSec[1] = MaTimeUs / SvCycleUs;
    	if( MaFil->conf.PmafNumSec[1] < 1 ){ MaFil->conf.PmafNumSec[1] = 1;}
    }
/*--------------------------------------------------------------------------------------------------*/
/*		長時間フィルタ：移動平均フィルタ＋補間移動平均フィルタ										*/
/*--------------------------------------------------------------------------------------------------*/
/*		最大の補間移動平均時間 = Cycle * (1<<imax) = 125us * 32 = 4ms								*/
/*		最大の合計移動平均時間 = 最大の補間移動平均時間 * MAFIL_BUFNUM = 4ms * 256 = 1024ms			*/
/*--------------------------------------------------------------------------------------------------*/
    else
    {
    	x = MAFIL_BUFNUM * SvCycleUs;						/*  補間移動平均回数算出 					*/
    	for( i=1; i < 5; i++ )								/*  補間移動平均回数は、				*/
    	{													/*  (1<<i) = 2,4,8,...					*/
    		if( (x << i) >= MaTimeUs ){ break;}				/*										*/
    	}													/*										*/
    /*----------------------------------------------------------------------------------------------*/
    		MaFil->conf.ImafSftSec[1] = (UCHAR)i;
    		MaFil->conf.PmafNumSec[1] = MaTimeUs / (SvCycleUs << i);
    		if( MaFil->conf.PmafNumSec[1] > MAFIL_BUFNUM ){ MaFil->conf.PmafNumSec[1] = MAFIL_BUFNUM;}
    }
/*--------------------------------------------------------------------------------------------------*/
    return;

}
#endif
#endif /* #if( CSW_SERVO_PCMD_FIL_USE != 0 ) */



/****************************************************************************************************/
/*																									*/
/*		位置指令フィルタ払い出し完了状態設定API														*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		位置制御モードの場合、払い出し完了状態(RefZ)が上位モジュールから設定される。				*/
/*		ただし、位置指令フィルタ演算の状態によっては払い出し未完了の場合がある。この場合には、		*/
/*		位置指令フィルタ払い出し完了状態取得API PcmdFilGetRefZStatus()により、状態を取得する。		*/
/*																									*/
/****************************************************************************************************/
void	PcmdFilSetRefZStatus( PCMDFIL *PcmdFil, BOOL status )
{
	PcmdFil->ExpFil.var.RefZStatus = status;
	PcmdFil->MaFil.var.RefZStatus = status;
	return;
}



/****************************************************************************************************/
/*																									*/
/*		位置指令フィルタ払い出し完了状態取得API														*/
/*																									*/
/****************************************************************************************************/
BOOL	PcmdFilGetRefZStatus( PCMDFIL *PcmdFil )
{
	return	(PcmdFil->ExpFil.var.RefZStatus & PcmdFil->MaFil.var.RefZStatus);
}



#if ( CSW_SVCF_RESVIB_FIL_USE == TRUE )
/****************************************************************************************************/
/*																									*/
/*		振動抑制フィルタ演算																		*/
/*																									*/
/*	概要:	振動抑制フィルタは，設定された周波数に対するノッチ特性を持ったフィルタである。			*/
/*			機台振動など位置決め時の振動を抑制する。												*/
/*			移動平均等位置指令フィルタ，MFCとも直列に接続可能。										*/
/*																									*/
/****************************************************************************************************/
LONG	PcmdFilVibSupFilter( VIBSUPFIL *VibSupFil, LONG dPcmd, BOOL *RefZSignal )
{
	LONG	AvffFili;
	LONG	AvffFilo;
	LONG	x1;
	LONG	x2;
	LONG	x3;
	LONG	wk = 0;

/*--------------------------------------------------------------------------------------------------*/
/*	指令完了＆払い出し完了チェック																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( (dPcmd == 0) && (VibSupFil->var.Buf == 0) )
	{ /* 指令完了 && フィルタ溜まりなし->払い出し完了 */
		VibSupFil->conf.Pexe = VibSupFil->conf.VibSupPrm;		/* パラメータ切替え実行				*/
	}
	AvffFili = dPcmd;											/* 振動抑制フィルタ←位置指令パルス	*/

/*--------------------------------------------------------------------------------------------------*/
/*		振動抑制フィルタ演算																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( VibSupFil->conf.Pexe.enable )
	{	/* 機能有効	*/
		x1 = MlibPfbkxremNolim( AvffFili, VibSupFil->conf.Pexe.Kff1, &VibSupFil->var.rem1 );
		x2 = MlibPfbkxremNolim( VibSupFil->var.wkbf1, VibSupFil->conf.Pexe.Kff2, &VibSupFil->var.rem2 );
		x3 = MlibPfbkxremNolim( VibSupFil->var.wkbf2, VibSupFil->conf.Pexe.Kff3, &VibSupFil->var.rem3 );

		VibSupFil->var.wkbf1 += (x1 - x3 - AvffFili);
		VibSupFil->var.wkbf2 += (x1 - x3 + x2);

		AvffFilo = x1 - x3;										/* 振動抑制フィルタ出力				*/

/*--------------------------------------------------------------------------------------------------*/
/*		フィルタ入出力偏差(溜まり)																	*/
/*--------------------------------------------------------------------------------------------------*/
		VibSupFil->var.Buf = MlibPerrcalx( AvffFili, AvffFilo, VibSupFil->var.FilioErr );
		if( VibSupFil->var.Buf != 0 )
		{ /* 位置指令払い出し未完了 */
			*RefZSignal = FALSE;
		}

/*--------------------------------------------------------------------------------------------------*/
/*		振動抑制フィルタ変数初期化																	*/
/*--------------------------------------------------------------------------------------------------*/
		if( (AvffFilo == 0)	&& (VibSupFil->var.wkbf1 == 0) && (VibSupFil->var.wkbf2 == 0) )
		{ /* 振動抑制フィルタ出力=0 && 振動抑制フィルタBUF=0 */
			/* 振動抑制フィルタ変数初期化 */
			MlibResetLongMemory( &VibSupFil->var, sizeof(VibSupFil->var)/4 );
		}

/*--------------------------------------------------------------------------------------------------*/
/*		フィルタ出力処理																			*/
/*--------------------------------------------------------------------------------------------------*/
		wk += AvffFilo;											/* 機能無効状態時は					*/
		VibSupFil->var.Filo = wk;								/* フィルタ出力＋指令入力を出力		*/
	}
	else
	{	/* 機能無効	*/
		wk = AvffFili;											/* 入力をそのまま出力				*/
		VibSupFil->var.Filo = wk;
	}

	return( wk );
}


/****************************************************************************************************/
/*																									*/
/*		振動抑制フィルタ選択																		*/
/*																									*/
/****************************************************************************************************/
BOOL	PcmdFilCalculatePrmVibSupFilSW( VIBSUPFIL *VibSupFil, ULONG mdlsw,
													ULONG ff_feq, ULONG ff_fil, LONG ScanTimeNs )
{
	BOOL	PrmSetErr;
	LONG	wk;

	PrmSetErr = FALSE;
	wk = (mdlsw >> 4) & 0x0F;

	switch( wk )
	{
	case 0x00: /* 機能無効 */
	case 0x01:
		VibSupFil->conf.VibSupSetting = FALSE;
		break;
	case 0x02: /* 振動抑制フィルタ有効 */
		VibSupFil->conf.VibSupSetting = TRUE;
		break;
	default :
		PrmSetErr = TRUE;
		break;
	}

	/*------------------------------------------------------------------------------------------*/
	/*	MFC スイッチ無効時は振動抑制フィルタOFF													*/
	/*------------------------------------------------------------------------------------------*/
	wk = mdlsw & 0x0F;
	switch( wk )
	{
	case 0x00: /* 機能無効 */
		/* 振動抑制フィルタ強制OFF */
		VibSupFil->conf.VibSupSetting = FALSE;
		break;
	}

	/* 実行用の機能選択スイッチへ設定反映 */
//	PosMngCalculatePrmVibSupFil( VibSupFil, ff_feq, ff_fil, ScanTimeNs ); /* <S065> */
	PcmdFilCalculatePrmVibSupFil( VibSupFil, ff_feq, ff_fil, ScanTimeNs );

	return	PrmSetErr;
}




/****************************************************************************************************/
/*																									*/
/*		振動抑制フィルタパラメータ計算																*/
/*																									*/
/****************************************************************************************************/
void	PcmdFilCalculatePrmVibSupFil( VIBSUPFIL *VibSupFil, ULONG ff_frq, ULONG ff_fil, LONG ScanTimeNs )
{
	VIBSUPFILPRM	wrkp;
	LONG			u1;
	LONG			s, sx;
	LONG			kx;
	LONG			wk;
	LONG			omega2;
//	LONG			scantime;

/*--------------------------------------------------------------------------------------------------*/
/*		演算周期設定																				*/
/*--------------------------------------------------------------------------------------------------*/
//	scantime = AVFILCALCYCLE;							/* 演算サイクルタイム	[ns]				*/

/*--------------------------------------------------------------------------------------------------*/
/*		機能選択																					*/
/*--------------------------------------------------------------------------------------------------*/
	wrkp.enable = VibSupFil->conf.VibSupSetting;
	u1 = ff_fil;

/*--------------------------------------------------------------------------------------------------*/
/*		ω２最大値																					*/
/*																									*/
/*					      (ff_frq/10)[Hz] * (ff_fil[%]/100) * (80/100)　* 10^9						*/
/*			omega2_limit = --------------------------------------------------------					*/
/*				   		               2 * PAI * CycleNs											*/
/*--------------------------------------------------------------------------------------------------*/
	wk = (LONG)ff_frq * (LONG)ff_fil;					/* パラメータ設定ω2*1000[Hz]				*/
	omega2 = MlibMIN( wk, (MAXOMEGA2(ScanTimeNs) * 1000) );	/* *1000[Hz]リミットとパラメータ値の最小値	*/

/*--------------------------------------------------------------------------------------------------*/
/*		振動抑制フィルタゲイン1 Avff.P.Kff1															*/
/*--------------------------------------------------------------------------------------------------*/
/*					      ff_fil^2																	*/
/*				Kff1 = --------------																*/
/*				   		   10000																	*/
/*--------------------------------------------------------------------------------------------------*/
	wrkp.Kff1 = MlibScalKxgain( u1 * u1, 1, 10000, NULL, 24 );

/*--------------------------------------------------------------------------------------------------*/
/*		振動抑制フィルタゲイン2 Avff.P.Kff2															*/
/*--------------------------------------------------------------------------------------------------*/
/*		振動抑制フィルタゲイン3 Avff.P.Kff3															*/
/*--------------------------------------------------------------------------------------------------*/
/*					     PAI * omega2 * CycleNs														*/
/*				Kff2 = --------------------------													*/
/*				   		       1000 * 10^9															*/
/*--------------------------------------------------------------------------------------------------*/
/*					     4*PAI * omega2 * CycleNs													*/
/*				Kff3 = -----------------------------												*/
/*				   		       1000 * 10^9															*/
/*--------------------------------------------------------------------------------------------------*/
	kx = MlibScalKxgain( omega2, ScanTimeNs, C10POW9, &s, 0 );
	kx = MlibPcalKxgain( kx, 1, 1000000000, &s, 0 );
	sx = s;

	wrkp.Kff2 = MlibPcalKxgain( kx, 3141593, 1, &s, 24 );				/* 3141593  =   PAI*1000000	*/
	wrkp.Kff3 = MlibPcalKxgain( kx, 12566371, 1, &sx , 24 );			/* 12566371 = 4*PAI*1000000	*/

/*--------------------------------------------------------------------------------------------------*/
/*		Set Parameters																				*/
/*--------------------------------------------------------------------------------------------------*/
//	KPI_DI();		/* Disable Interrupt	*/
	VibSupFil->conf.VibSupPrm = wrkp;
//	KPI_EI();		/* Enable Interrupt		*/

}

#endif /* #if( CSW_SVCF_RESVIB_FIL_USE != 0 ) */



/***************************************** end of file **********************************************/
