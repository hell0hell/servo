/****************************************************************************************************/
/*																									*/
/*																									*/
/*		AutoGainChange.c : Automatic Gain Change													*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		1) Automatic Gain Change Initialization														*/
/*																									*/
/*		2) Automatic Gain Change Parameter Calculation												*/
/*																									*/
/*		3) Automatic Gain Change Execution 															*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.1.00 : 2013.12.20  K.Ozaki	<S09B> 														*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"
#include "GainChange.h"
#include "MLib.h"

/****************************************************************************************************/
/*																									*/
/*		Macro Definition																			*/
/*																									*/
/****************************************************************************************************/
#define _GainLauFilter( pact, pnxt, pat, ks )                             							\
		(pact)->##ks = GainLauFilter( (pact)->##ks, (pnxt)->##ks, (pat)->conf.dGain.##ks )
/*--------------------------------------------------------------------------------------------------*/
#define _FilGnLauFilter( pact, pnxt, pat, ks )                             							\
		(pact)->##ks = FilGnLauFilter( (pact)->##ks, (pnxt)->##ks, (pat)->conf.dGain.##ks, 			\
						(pat)->conf.GselTrqFilMin )
/*--------------------------------------------------------------------------------------------------*/
#define _GainLauFilterD( pact, pnxt, pat, ks )                             							\
		(pact)->##ks = GainLauFilter( (pact)->##ks, (pnxt)->##ks, (pat)->conf.dGainDobs.##ks )
/*--------------------------------------------------------------------------------------------------*/
#define _DeltaFilGnCalc( pat, gselbuf, ks, cycle )  												\
		gselbuf.##ks = DeltaFilGnCalc( (pat)->var.prevGain->##ks, (pat)->var.nextGain->##ks, 		\
						(pat)->conf.SwTime, (pat)->conf.GselTrqFilMin, cycle )
/*--------------------------------------------------------------------------------------------------*/
#define _DeltaGainCalc( pat, gselbuf, ks )  														\
		gselbuf.##ks = DeltaGainCalc( (pat)->var.prevGain->##ks, (pat)->var.nextGain->##ks, 		\
						(pat)->conf.SwTime, (pat)->conf.SvCycleUs )
/*--------------------------------------------------------------------------------------------------*/
extern void __EI( void );									/* 割り込み許可設定						*/
extern void __DI( void );									/* 割り込み禁止設定						*/
/*--------------------------------------------------------------------------------------------------*/

/****************************************************************************************************/
/*																									*/
/*		Function Prototypes																			*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Public Functions																			*/
/*--------------------------------------------------------------------------------------------------*/
void	GselInitAutoGainChange( ATGSEL *pAtGsel, 		/* Initialize Auto Gain Change				*/
			GSELGAINS 	*prevGain,						/* prevGain 								*/
			GSELGAINS 	*nextGain,						/* nextGain 								*/
			GSELDOBS	*prevGainDobs,					/* prevGain(Dobs) 							*/
			GSELDOBS	*nextGainDobs );				/* nextGain(Dobs) 							*/
/*--------------------------------------------------------------------------------------------------*/
void	GselAutoGainChange( 							/* Auto Gain Change Main Function			*/
			BASE_CTRL_HNDL *BaseControls, 				/*											*/
			LONG trigger );								/*											*/
/*--------------------------------------------------------------------------------------------------*/
void	GselRstAutoGainChange( 							/* Reset Auto Gain Change 					*/
			GAIN_CHNG_HNDL	*pGainChange );				/*											*/
/*--------------------------------------------------------------------------------------------------*/
void	PcalAutoGainChange( 							/* Auto Gain Change Parameter Calculation	*/
			ATGSEL *pAtGsel, 							/* Auto Gain Change Variable				*/
			LONG swtime, 								/* Gain Switch Time (Pn131/Pn132)[ms]		*/
			LONG dlytime, 								/* Change Delay Time (Pn135/Pn136)[ms] 		*/
			LONG svcycleus );							/* Execute Cycle [us]						*/
/*--------------------------------------------------------------------------------------------------*/
void	PcalAtGseldKpSet( 								/* 自動ゲイン切替時の�Kpゲイン計算			*/
			ATGSEL *pAtGsel, 							/* Auto Gain Change Variable				*/
			GSELGAINS *dstGain );						/* Destination Gains						*/
/*--------------------------------------------------------------------------------------------------*/
void	PcalAtGseldKviSet( 								/* 自動ゲイン切替時の�Kv,�Kviゲイン計算	*/
			ATGSEL *pAtGsel, 							/* Auto Gain Change Variable				*/
			GSELGAINS *dstGain );						/* Destination Gains						*/
/*--------------------------------------------------------------------------------------------------*/
void	PcalAtGseldKlpfSet( 							/* 自動ゲイン切替時の�Klpfゲイン計算		*/
			ATGSEL *pAtGsel, 							/* Auto Gain Change Variable				*/
			GSELGAINS *dstGain );						/* Destination Gains						*/
/*--------------------------------------------------------------------------------------------------*/
void	PcalAtGseldKsSet( 								/* 自動ゲイン切替時の�Ks,Ksjゲイン計算		*/
			ATGSEL *pAtGsel, 							/* Auto Gain Change Variable				*/
			GSELDOBS *dstGain );						/* Destination Gains						*/
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		Private Functions																			*/
/*--------------------------------------------------------------------------------------------------*/
static void	 LpxDeltaGainChange( 						/* delta gain calc. for auto gain change 	*/
				ATGSEL *pAtGsel, 						/* Auto Gain Change Variables 				*/
				GSELGAINS *pActGains ); 				/* Auto Gain Change Actual Gains 			*/
/*--------------------------------------------------------------------------------------------------*/
static void	 LpxDeltaGainDobs( 							/* delta gain calc. for Dist. Observer		*/
				ATGSEL *pAtGsel, 						/* Auto Gain Change Variables 				*/
				GSELDOBS *pActDobs ); 					/* Auto Gain Change Actual Gains 			*/
/*--------------------------------------------------------------------------------------------------*/
static void	 LpxDeltaAmonGain( 							/* delta gain calc. for Analog Monitor		*/
				ATGSEL *pAtGsel, 						/* Auto Gain Change Variables 				*/
				LONG *pActGain,							/* Auto Gain Change Actual Gain				*/
				LONG PrvGain, 							/* Auto Gain Change Previous Gain			*/
				LONG NxtGain );							/* Auto Gain Change Next Gain				*/
/*--------------------------------------------------------------------------------------------------*/
static LONG GainLauFilter( 								/* ゲイン直線切替演算						*/
				LONG Src, 								/* source gain								*/
				LONG Dst, 								/* destination gain							*/
				LONG Delta );							/* delta gain								*/
/*--------------------------------------------------------------------------------------------------*/
static LONG	FilGnLauFilter( 							/* フィルタゲイン直線切替演算				*/
				LONG Src, 								/* source gain								*/
				LONG Dst, 								/* destination gain							*/
				LONG Delta, 							/* delta gain								*/
				LONG Min );								/* minimum gain								*/
/*--------------------------------------------------------------------------------------------------*/
static LONG	DeltaGainCalc( 								/* �ゲイン計算								*/
				LONG prev_gn, 							/* previous gain 							*/
				LONG next_gn, 							/* next gain 								*/
				USHORT time, 							/* switch time [ms] 						*/
				LONG SvCycleUs );						/* cycle time [us] 							*/
/*--------------------------------------------------------------------------------------------------*/
static LONG	DeltaFilGnCalc( 							/* �フィルタゲイン計算						*/
				LONG prev_gn, 							/* previous gain 							*/
				LONG next_gn, 							/* next gain 								*/
				USHORT time, 							/* switch time [ms] 						*/
				LONG Min, 								/* minimum gain 							*/
				LONG SvCycleUs );						/* cycle time [us] 							*/


/****************************************************************************************************/
/*																									*/
/*		Initialize Auto Gain Change																	*/
/*																									*/
/****************************************************************************************************/
void	GselInitAutoGainChange( ATGSEL *pAtGsel, 		/* Initialize Auto Gain Change				*/
			GSELGAINS 	*prevGain,						/* prevGain 								*/
			GSELGAINS 	*nextGain,						/* nextGain 								*/
			GSELDOBS	*prevGainDobs,					/* prevGain(Dobs) 							*/
			GSELDOBS	*nextGainDobs )					/* nextGain(Dobs) 							*/
{
/*--------------------------------------------------------------------------------------------------*/
	MlibResetLongMemory( &(pAtGsel->var), sizeof(pAtGsel->var)/4 );		   /* reset memory 			*/
/*--------------------------------------------------------------------------------------------------*/
	pAtGsel->var.prevGain = prevGain;									   /* set previous gain 	*/
	pAtGsel->var.nextGain = nextGain;									   /* set next gain 		*/
	pAtGsel->var.prevGainDobs = prevGainDobs;							   /* set previous gain 	*/
	pAtGsel->var.nextGainDobs = nextGainDobs;							   /* set next gain 		*/
/*--------------------------------------------------------------------------------------------------*/
	pAtGsel->conf.GselTrqFilMin = MlibPcalKf1gain( 10, KPI_SACYCLEUS, 0 ); /* set minimum 			*/
/*--------------------------------------------------------------------------------------------------*/
}

/****************************************************************************************************/
/*																									*/
/*		Auto Gain Change Main Function																*/
/*																									*/
/****************************************************************************************************/
void	GselAutoGainChange( 
			BASE_CTRL_HNDL *BaseControls, 				/*											*/
			LONG trigger )								/*											*/
{
ATGSEL	*pAtGsel;
GSELGAINS *pActGains;
GSELDOBS *pActDobs;
LONG	*pActAmon;
/*--------------------------------------------------------------------------------------------------*/
GAIN_CHNG_HNDL *hGainChg	= &(BaseControls->GainChange);
MFCTRL *pMfcData			= &(BaseControls->MFControl);
CTRL_CMD_PRM *CtrlCmdPrm 	= &(BaseControls->CtrlCmdPrm);
//DOBS_CTRL *pDobs			= &(BaseControls->DobsCtrl);/*<S0C7>*/
/*--------------------------------------------------------------------------------------------------*/
	pActGains = &(hGainChg->GselGains[0]);
	pActDobs = &(hGainChg->GselDobs[0]);
	pActAmon = &(hGainChg->AmonActGain);
/*--------------------------------------------------------------------------------------------------*/
/*		条件Ａ成立で第２ゲイン遷移または第２ゲイン選択中											*/
/*--------------------------------------------------------------------------------------------------*/
	if ( trigger )
	{
		pAtGsel = &(hGainChg->AtGselA);							/* 第１→第２ゲイン切替				*/
		if ( pAtGsel->var.Timer >= pAtGsel->conf.Wait ) 		/* 第１→第２ゲイン待ち時間経過後	*/
		{
			LpxDeltaGainChange( pAtGsel, pActGains );			/* delta gain change 				*/
			LpxDeltaGainDobs( pAtGsel, pActDobs );				/* delta gain change (DISOBS) 		*/
			LpxDeltaAmonGain( pAtGsel, pActAmon, GAIN1, GAIN2 );/* delta gain change (AMON)			*/
			hGainChg->ActGain = 2;								/* set monitor 						*/
			hGainChg->AtGselB.var.Timer = 0;					/* 待ち時間２タイマ初期化			*/
		}
		else
		{
			(pAtGsel->var.Timer)++;								/* 待ち時間１タイマカウントアップ	*/
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		条件Ｂ成立で第１ゲイン遷移または第１ゲイン選択中											*/
/*--------------------------------------------------------------------------------------------------*/
	else
	{
		pAtGsel = &(hGainChg->AtGselB);							/* 第２→第１ゲイン切替				*/
		if ( pAtGsel->var.Timer >= pAtGsel->conf.Wait ) 		/* 第２→第１ゲイン待ち時間経過後	*/
		{
			LpxDeltaGainChange( pAtGsel, pActGains );			/* delta gain change 				*/
			LpxDeltaGainDobs( pAtGsel, pActDobs );				/* delta gain change (DISOBS) 		*/
			LpxDeltaAmonGain( pAtGsel, pActAmon, GAIN2, GAIN1 );/* delta gain change (AMON)			*/
			hGainChg->ActGain = 1;								/* set monitor 						*/
			hGainChg->AtGselA.var.Timer = 0;					/* 待ち時間１タイマ初期化			*/
		}
		else
		{
			(pAtGsel->var.Timer)++;								/* 待ち時間２タイマカウントアップ	*/
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		条件ＡでもＢでもない状態の時は前回のゲインのまま											*/
/*--------------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------*/
	/* MFCは自動ゲイン切替えなし。第1ゲインのまま 													*/
	/*----------------------------------------------------------------------------------------------*/
	pMfcData->conf.MfcPrm[0] = pMfcData->conf.MfcPrm[1];
	/*----------------------------------------------------------------------------------------------*/
	/* 速度制限用ゲインは、自動ゲイン切替なし。第１ゲインのまま										*/
	/*----------------------------------------------------------------------------------------------*/
	CtrlCmdPrm->OverTrqLoopGain[0]	= CtrlCmdPrm->OverTrqLoopGain[1];
	CtrlCmdPrm->OverTrqIntegGain[0]	= CtrlCmdPrm->OverTrqIntegGain[1];
/*--------------------------------------------------------------------------------------------------*/
	return;
}

/****************************************************************************************************/
/*																									*/
/*		Delta Gain Change 																			*/
/*																									*/
/****************************************************************************************************/
void	GselRstAutoGainChange( 							/* Reset Auto Gain Change 					*/
			GAIN_CHNG_HNDL	*pGainChange )				/*											*/
{
		pGainChange->AtGselA.var.Timer = 0;
		pGainChange->AtGselB.var.Timer = 0;
}

/****************************************************************************************************/
/*																									*/
/*		Delta Gain Change 																			*/
/*																									*/
/****************************************************************************************************/
void	 LpxDeltaGainChange( ATGSEL *pAtGsel, GSELGAINS *pActGains )
{
GSELGAINS *pNxtGains = pAtGsel->var.nextGain;
	/*----------------------------------------------------------------------------------------------*/
	if ( pAtGsel->conf.SwTime != 0 )				/* Δゲイン切替									*/
	{
	/*----------------------------------------------------------------------------------------------*/
	/* 	Delta Gain Change 																			*/
	/*----------------------------------------------------------------------------------------------*/
		_GainLauFilter( pActGains, pNxtGains, pAtGsel, Kp );
		_GainLauFilter( pActGains, pNxtGains, pAtGsel, Kv );
		_GainLauFilter( pActGains, pNxtGains, pAtGsel, Kvi );
		_GainLauFilter( pActGains, pNxtGains, pAtGsel, Kv2 );
		_GainLauFilter( pActGains, pNxtGains, pAtGsel, Kv2Inv );
		_FilGnLauFilter( pActGains, pNxtGains, pAtGsel, Klpf );
//		_GainLauFilter( pActGains, pNxtGains, pAtGsel, KpInv );
		_GainLauFilter( pActGains, pNxtGains, pAtGsel, VirtualPosKp );
		_GainLauFilter( pActGains, pNxtGains, pAtGsel, ZcKp );
	}
	else											/* Δゲイン切替なし								*/
	{
		*pActGains = *pNxtGains;
	}
	return;
}

/****************************************************************************************************/
/*																									*/
/*		Delta Gain Change for the Disturbance Observer 												*/
/*																									*/
/****************************************************************************************************/
void	 LpxDeltaGainDobs( 								/* delta gain calc. for Dist. Observer		*/
				ATGSEL *pAtGsel, 						/* Auto Gain Change Variables 				*/
				GSELDOBS *pActDobs ) 					/* Auto Gain Change Actual Gains 			*/
{
GSELDOBS *pNxtDobs = pAtGsel->var.nextGainDobs;
	/*----------------------------------------------------------------------------------------------*/
	if ( pAtGsel->conf.SwTime != 0 )					/* Δゲイン切替								*/
	{
	/*----------------------------------------------------------------------------------------------*/
	/*	Delta Gain Change (Disturbance Observer)													*/
	/*----------------------------------------------------------------------------------------------*/
		_GainLauFilterD( pActDobs, pNxtDobs, pAtGsel, Ks );
		_GainLauFilterD( pActDobs, pNxtDobs, pAtGsel, Ksj );
	}
	else												/* Δゲイン切替なし							*/
	{
		*pActDobs = *pNxtDobs;
	}
	return;
}

/****************************************************************************************************/
/*																									*/
/*		Delta Gain Change for Analog Monitor														*/
/*																									*/
/****************************************************************************************************/
void	 LpxDeltaAmonGain( 								/* delta gain calc. for Analog Monitor		*/
				ATGSEL *pAtGsel, 						/* Auto Gain Change Variables 				*/
				LONG *pActGain,							/* Auto Gain Change Actual Gain				*/
				LONG PrvGain, 							/* Auto Gain Change Previous Gain			*/
				LONG NxtGain )							/* Auto Gain Change Next Gain				*/
{
LONG	wkx;
	/*----------------------------------------------------------------------------------------------*/
	if ( pAtGsel->conf.SwTime != 0 )					/* Δゲイン切替								*/
	{
		if ( NxtGain > PrvGain )						/* Forward 									*/
		{
			wkx = (*pActGain) + pAtGsel->conf.AmondGain;
			if ( wkx > NxtGain )						/* Limit 									*/
			{
				*pActGain = NxtGain;		
			}
			else
			{
				*pActGain = wkx;		
			}
		}
		else											/* Backward 								*/
		{
			wkx = (*pActGain) - pAtGsel->conf.AmondGain;
			if ( wkx < NxtGain )						/* Limit 									*/
			{
				*pActGain = NxtGain;		
			}
			else
			{
				*pActGain = wkx;		
			}
		}
	}
	else												/* Δゲイン切替なし							*/
	{
		*pActGain = NxtGain;
	}
	return;
}

/****************************************************************************************************/
/*																									*/
/*		ゲイン直線切替演算  																		*/
/*																									*/
/****************************************************************************************************/
LONG	GainLauFilter( LONG Src, LONG Dst, LONG Delta )
{
LONG	wrk;
LONG	sft;
LONG	kx;
LONG	sx;
LONG	sdx;
LONG	RetGain;
LONG	wrk_Src;

	if ( ( Src == Dst ) || ( Delta == 0 ) )
	{
		RetGain = Dst;
	}
	else
	{
		sft = ( ( Src >> 24 ) - ( Delta >> 24 ) );
		
		sx = ( Src >> 24 );
		sdx = ( Dst >> 24 );
		if(sft <= 0)
		{
			wrk = ( ( Delta & 0x00FFFFFF ) >> (-sft) );
		}
		else
		{
			wrk_Src = ( ( Src & 0x00FFFFFF ) >> sft );
			sx = Delta >> 24;
			Src = ( ( sx << 24 ) | wrk_Src );
			wrk = Delta & 0x00FFFFFF;
		}
		
		if( wrk == 0 )	{	wrk = 1;	}
		
		if ( ( sx < sdx ) || ( ( sx == sdx ) && ( Src > Dst ) ) )
		{
			kx = ( ( Src & 0x00FFFFFF ) - wrk );
			
			if ( (kx < 0x400000) && (sx < sdx) )
			{
				kx <<= 1;
				sx += 1;
			}
			RetGain = ( ( sx << 24 ) | kx );
			
			if ( ((sx == sdx) && (RetGain <= Dst)) || (kx <= 0) )	RetGain = Dst;
		}
		else
		{
			kx = ( ( Src & 0x00FFFFFF ) + wrk );
			
			if ( kx > 0x7FFFFF )
			{
				kx >>= 1;
				sx -= 1;
			}
			RetGain = ( ( sx << 24 ) | kx );
			
			if ( ((sx == sdx) && (RetGain >= Dst)) || (sx < sdx) )	RetGain = Dst;
		}
	}

	return( RetGain );
}


/****************************************************************************************************/
/*																									*/
/*		フィルタゲイン直線切替演算 																	*/
/*		※最小時定数パラメータを引数にした。														*/
/*																									*/
/****************************************************************************************************/
LONG	FilGnLauFilter( LONG Src, LONG Dst, LONG Delta, LONG Min )
{
LONG	wrk;
LONG	kx;
LONG	sx;
LONG	RetGain;
LONG	wrk_Dst;

	if( Src == Dst )
	{
		RetGain = Dst;
	}
	else
	{
/*--------------------------------------------------------------------------------------------------*/
/*		Src = 0, Dst = 0 の場合の処理を追加する														*/
/*		フィルタゲインが０の場合、この中の演算は最小時定数(0.01ms)設定時のゲインを用いて行う		*/
/*--------------------------------------------------------------------------------------------------*/
		if( (Src & 0x00FFFFFF) == 0 )
		{
			Src = Min;
		}
		wrk_Dst = Dst;
		if( (Dst & 0x00FFFFFF) == 0 )
		{
			Dst = Min;
		}
/*--------------------------------------------------------------------------------------------------*/
		sx = ( Src >> 24 );
		wrk = ( Delta & 0x00FFFFFF );
		
		if( wrk == 0 )	{	wrk = 1;	}
		
		if ( Src > Dst )
		{
			kx = ( ( Src & 0x00FFFFFF ) - wrk );
			RetGain = ( ( sx << 24 ) | kx );
			
			if ( RetGain <= Dst )	RetGain = Dst;
		}
		else
		{
			kx = ( ( Src & 0x00FFFFFF ) + wrk );
			RetGain = ( ( sx << 24 ) | kx );
			
			if( RetGain >= Dst )
			{
				if( (wrk_Dst & 0x00FFFFFF) == 0 )	/* Dst = 0 の場合、RetGainも０にする			*/
				{
					RetGain = wrk_Dst;
				}
				else
				{
					RetGain = Dst;
				}
			}
		}
	}

	return( RetGain );
}

/****************************************************************************************************/
/*																									*/
/*		自動ゲイン切替パラメータ計算																*/
/*																									*/
/****************************************************************************************************/
void	PcalAutoGainChange( 							/* Auto Gain Change Parameter Calculation	*/
				ATGSEL *pAtGsel, 						/* Auto Gain Change Variable				*/
				LONG swtime, 							/* Gain Switch Time (Pn131/Pn132)[ms]		*/
				LONG dlytime, 							/* Change Delay Time (Pn135/Pn136)[ms] 		*/
				LONG svcycleus )						/* Execute Cycle [us]						*/
{
/*--------------------------------------------------------------------------------------------------*/
/*		set parameter																				*/
/*--------------------------------------------------------------------------------------------------*/
		pAtGsel->conf.SwTime = swtime;								/* set gain switch time 		*/
		pAtGsel->conf.SvCycleUs = svcycleus;						/* set cycle 					*/
		pAtGsel->conf.Wait = dlytime * 1000 / svcycleus;			/* set wait time 				*/
/*--------------------------------------------------------------------------------------------------*/
/*		set delta gain																				*/
/*--------------------------------------------------------------------------------------------------*/
		PcalAtGseldKpSet( pAtGsel, &(pAtGsel->conf.dGain) );		/* calculate delta Kp 			*/
		PcalAtGseldKviSet( pAtGsel, &(pAtGsel->conf.dGain) );		/* calculate delta Kv, Kvi 		*/
		PcalAtGseldKlpfSet( pAtGsel, &(pAtGsel->conf.dGain) );		/* calculate delta Klpf 		*/
		PcalAtGseldKsSet( pAtGsel, &(pAtGsel->conf.dGainDobs) );	/* calculate delta Ks 			*/
/*--------------------------------------------------------------------------------------------------*/
/*		set analog monitor dgain																	*/
/*--------------------------------------------------------------------------------------------------*/
		if ( swtime != 0 )
		{
			pAtGsel->conf.AmondGain = ( GAIN1 * svcycleus ) / ( 1000 * swtime );
		}
		else
		{
			pAtGsel->conf.AmondGain = GAIN1;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}


/****************************************************************************************************/
/*																									*/
/*		 自動ゲイン切替時の�Kpゲイン計算															*/
/*																									*/
/****************************************************************************************************/
void	PcalAtGseldKpSet( ATGSEL *pAtGsel, GSELGAINS *dstGain )
{
GSELGAINS GselGbufx;
/*--------------------------------------------------------------------------------------------------*/
	_DeltaGainCalc( pAtGsel, GselGbufx, Kp );
	_DeltaGainCalc( pAtGsel, GselGbufx, ZcKp );
//	_DeltaGainCalc( pAtGsel, GselGbufx, KpInv );
	_DeltaGainCalc( pAtGsel, GselGbufx, VirtualPosKp );
/*--------------------------------------------------------------------------------------------------*/
	dstGain->Kp = GselGbufx.Kp;
	dstGain->ZcKp = GselGbufx.ZcKp;
//	dstGain->KpInv = GselGbufx.KpInv;
	dstGain->VirtualPosKp = GselGbufx.VirtualPosKp;
/*--------------------------------------------------------------------------------------------------*/
	return;
}

/****************************************************************************************************/
/*																									*/
/*		 自動ゲイン切替時の�Kv,�Kviゲイン計算														*/
/*																									*/
/****************************************************************************************************/
void	PcalAtGseldKviSet( ATGSEL *pAtGsel, GSELGAINS *dstGain )
{
GSELGAINS GselGbufx;
/*--------------------------------------------------------------------------------------------------*/
	_DeltaGainCalc( pAtGsel, GselGbufx, Kv );
	_DeltaGainCalc( pAtGsel, GselGbufx, Kvi );
	_DeltaGainCalc( pAtGsel, GselGbufx, Kv2 );
	_DeltaGainCalc( pAtGsel, GselGbufx, Kv2Inv );
/*--------------------------------------------------------------------------------------------------*/
	KPI_DI();
	dstGain->Kv = GselGbufx.Kv;
	dstGain->Kvi = GselGbufx.Kvi;
	dstGain->Kv2 = GselGbufx.Kv2;
	dstGain->Kv2Inv = GselGbufx.Kv2Inv;
	KPI_EI();
/*--------------------------------------------------------------------------------------------------*/
	return;
}


/****************************************************************************************************/
/*																									*/
/*		自動ゲイン切替時の�Kipfゲイン計算															*/
/*																									*/
/****************************************************************************************************/
void	PcalAtGseldKlpfSet( ATGSEL *pAtGsel, GSELGAINS *dstGain )
{
	dstGain->Klpf = DeltaFilGnCalc( 
				pAtGsel->var.prevGain->Klpf, 
				pAtGsel->var.nextGain->Klpf, 
				pAtGsel->conf.SwTime,
				pAtGsel->conf.GselTrqFilMin,
				pAtGsel->conf.SvCycleUs );
	return;
}

/****************************************************************************************************/
/*																									*/
/*		 自動ゲイン切替時の�Ks,�Ksjゲイン計算														*/
/*																									*/
/****************************************************************************************************/
void	PcalAtGseldKsSet( ATGSEL *pAtGsel, GSELDOBS *dstGain )
{
GSELDOBS GselDobsBufx;
/*--------------------------------------------------------------------------------------------------*/
	GselDobsBufx.Ks 	= DeltaGainCalc( pAtGsel->var.prevGainDobs->Ks, 
										pAtGsel->var.nextGainDobs->Ks, 
										pAtGsel->conf.SwTime,
										pAtGsel->conf.SvCycleUs );
	GselDobsBufx.Ksj 	= DeltaGainCalc( pAtGsel->var.prevGainDobs->Ksj, 
										pAtGsel->var.nextGainDobs->Ksj, 
										pAtGsel->conf.SwTime,
										pAtGsel->conf.SvCycleUs );
/*--------------------------------------------------------------------------------------------------*/
	KPI_DI();
	dstGain->Ks = GselDobsBufx.Ks;
	dstGain->Ksj = GselDobsBufx.Ksj;
	KPI_EI();
/*--------------------------------------------------------------------------------------------------*/
	return;
}

/****************************************************************************************************/
/*																									*/
/*		自動ゲイン切替時の�ゲイン計算																*/
/*		※演算周期を引数にした																		*/
/*																									*/
/****************************************************************************************************/
LONG	DeltaGainCalc( LONG prev_gn, LONG next_gn, USHORT time, LONG SvCycleUs )
{
LONG	s;
LONG	sft;
LONG	wrk;

	sft = ( ( prev_gn >> 24 ) - ( next_gn >> 24 ) );

	if ( sft < 0 )
	{
		s = ( prev_gn >> 24 );
		wrk = MlibAbs32( ( (prev_gn & 0x00FFFFFF) - ( (next_gn & 0x00FFFFFF ) >> (-sft) )) );
	}
	else
	{
		s = ( next_gn >> 24 );
		wrk = MlibAbs32( ( ((prev_gn & 0x00FFFFFF) >> sft ) - (next_gn & 0x00FFFFFF ) ) );
	}
	if ( time == 0 )
	{
		wrk = MlibPcalKxgain( wrk, 1, 1, &s, 24 );
	}
	else
	{
		wrk = MlibPcalKxgain( wrk, SvCycleUs, 1000*time, &s, 24 );
	}
	return( wrk );
}


/****************************************************************************************************/
/*																									*/
/*		自動ゲイン切替時の�フィルタゲイン計算														*/
/*		※最小時定数、演算周期を引数にした															*/
/*																									*/
/****************************************************************************************************/
LONG	DeltaFilGnCalc( LONG prev_gn, LONG next_gn, USHORT time, LONG Min, LONG SvCycleUs )
{
LONG	sx;
LONG	kx;

/*--------------------------------------------------------------------------------------------------*/
/*		prev_gn = 0, next_gn = 0 の場合の処理を追加する												*/
/*		フィルタゲインが０の場合は、最小時定数(0.01ms)設定時のゲインを用いて�ゲインを計算する		*/
/*--------------------------------------------------------------------------------------------------*/
	if( (prev_gn & 0x00FFFFFF) == 0 )
	{
		prev_gn = Min;
	}
	if( (next_gn & 0x00FFFFFF) == 0 )
	{
		next_gn = Min;
	}
/*--------------------------------------------------------------------------------------------------*/
	sx = ( prev_gn >> 24 );
	kx = MlibAbs32( ( (prev_gn & 0x00FFFFFF) - (next_gn & 0x00FFFFFF) ) );

	if ( time != 0)
		kx = (SvCycleUs * kx) / ( 1000 * time );

	return ( (sx << 24) | kx );
}



/***************************************** end of file **********************************************/
