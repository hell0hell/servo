/****************************************************************************
Description	: SPOSING Motion Generator
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : Mercury

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------

****************************************************************************/
#include "Basedef.h"
#include "MtGenerator.h"
#include "MtPRM.h"
#include "MtApi.h"
#include "MLib.h"



/*------------------------------------------------------------------------------------------*/
/*		SPOSING functions																	*/
/*------------------------------------------------------------------------------------------*/
//static void mtgSpSetPrm( MG_SPOSING* sposing, MG_PRM* prm, LONG TargetSpd );
static void mtgSpCalAccPrm( MG_SPOSING* sposing, LONG CurrentSpd );
//static BOOL mtgSpCalDecPrm(MG_SPOSING* sposing, MC_BASE_PRM* bPrm, BOOL ChkReverse,
//								LONG CurrentSpd, LONG *StopDistance, ULONG *StopDistance0 );
static LONG mtgSpAccelerator( MG_SPOSING* sposing, LONG CurrentSpd );
static LONG mtgSpDecelerator( MG_SPOSING* sposing, LONG CurrentSpd, LONG *Segment );
static LONG mtgSpCheckDecelaration(MG_S_POSWK* wk, MC_BASE_PRM* bPrm);
static LONG mtgSpCalSpdRemDist( MG_SPOSING* sposing, LONG Td, LONG* work );


/*********************************/	
//#ifdef	__ghs__
//#pragma ghs section bss=".fixed_buffer"
//#endif

//static ULONG debug_xx[1000][2];
//static ULONG i;

//#ifdef	__ghs__
//#pragma ghs section bss=default
//#endif
/*********************************/	

/****************************************************************************
 SHOLDモーション処理
****************************************************************************/
BOOL MtgShold( MG_HNDL *hdl, MG_INOUT *inOut )
{
	MG_PRM *prm;
	BOOL	rc;

	prm = hdl->prm;

	if( inOut->Ctrl == MG_HOLD_CTRL2 )
	{
		hdl->prm = &hdl->basePrm->StopPrm;
	}
	
	rc = MtgSposing( hdl, inOut );

	hdl->prm = prm;

	return rc;
}


/*********************************************************************************************
 SPOSINGモーション処理
*********************************************************************************************/
BOOL MtgSposing( MG_HNDL *hdl, MG_INOUT *inOut )
{
	MG_S_POSWK wk;
	MG_SPOSING *sposing;
	KSGAIN	tmp;
	LONG 	buf;
	LONG 	ChkDecDistance;
	BOOL 	ChkEnd;				/* SPOSING終了OK：TRUE、SPOSING終了しない:FALSE				*/
	BOOL 	ChkOverflow;		/* 減速距離オーバフロー:TRUE、オーバフローなし:FALSE		*/
	BOOL	ChkReverse;			/* 目標位置が回転方向と同じ向き:TRUE、回転方向と逆:FALSE	*/

	sposing = &hdl->sposing;
	ChkEnd = TRUE;
//	ChkOverflow = FALSE;

	hdl->dposRem    = 0;
	inOut->VrefOut  = 0;
	inOut->TrefOut  = 0;
	hdl->VrefSum[0] = 0;
	hdl->VrefSum[1] = 0;

#if (FLOAT_USE==TRUE)
	tmp = (float)inOut->Tspd * hdl->basePrm->PosingSpdKx;
#else
	tmp = MlibMulgain32( inOut->Tspd, hdl->basePrm->PosingSpdKx );
#endif /* FLOAT_USE */
	if( (ULONG)tmp >= 0x40000000UL )
	{
		wk.TargetSpd = 0x3FFFFFFF;
	}
	else
	{
		wk.TargetSpd = (LONG)tmp;
	}

	wk.CurrentSpd  = hdl->Cspd;				/* 指令速度をコピ－***.*[ref/cyc]				*/
	wk.LastSpd     = wk.CurrentSpd;			/* 指令速度を保管								*/
	wk.Segment     = wk.CurrentSpd;			/* 指令速度(dpos)を保管							*/
	wk.ErrorDistance = inOut->DposIn;		/* 残距離計算（方向付き）						*/
	/*--------------------------------------------------------------------------------------*/
	/* 回転方向チェック																		*/
	/*--------------------------------------------------------------------------------------*/
	/* 現在の指令位置から目標位置の方向を決定 */
	hdl->tMinusDir = FALSE;					/* +方向 										*/
	if( wk.ErrorDistance < 0 )
	{
		hdl->tMinusDir   = TRUE;			/* -方向 										*/
		wk.ErrorDistance = -inOut->DposIn;	/* 残距離										*/
	}
	/*--------------------------------------------------------------------------------------*/
	/* SPOSING開始・終了チェック															*/
	/*--------------------------------------------------------------------------------------*/
	if( wk.CurrentSpd == 0 )
	{	/* 指令速度=0なら, SPOSING初期状態へリセット */
		hdl->cMinusDir = hdl->tMinusDir;
		hdl->dpos0     = 0;					/* DPOSの少数部(指令速度(少数部)の積算)			*/
		inOut->DposOut = 0;					/* 出力移動量[ref](Vel->Pos)					*/

		/* 残距離=0で払い出し完了 */
		if( wk.ErrorDistance == 0 )
		{
			/* 払い出し完了 */
			sposing->State = SPOSING_INI_MODE;
			hdl->Cspd = 0;
			return TRUE;
		}
	}
	/*--------------------------------------------------------------------------------------*/
	/* 逆回転チェック																		*/
	/*--------------------------------------------------------------------------------------*/
	/* 目標位置が回転方向と同じ向き：TRUE、回転方向と逆:FALSE 								*/
	ChkReverse = !(hdl->tMinusDir ^ hdl->cMinusDir);
	/*--------------------------------------------------------------------------------------*/
	/* 前回余り補正 																		*/
	/*--------------------------------------------------------------------------------------*/
	if( hdl->dpos0 != 0 )
	{
		/* dposの少数部が0でなかった場合は、余りが少数で残っているので予め-1しておく */
		wk.ErrorDistance = wk.ErrorDistance - 1;
	}
	wk.ErrorDistance0 = (0 - hdl->dpos0) & VELREM_BIT_MSK;

	/*--------------------------------------------------------------------------------------*/
	/*			SPOSING モード選択															*/
	/*--------------------------------------------------------------------------------------*/
	if( (sposing->State != SPOSING_DEC_MODE1) && (sposing->State != SPOSING_DEC_MODE2) )
	{
		/* 目標速度、加速度、減速度、S字比率初期化 */
		mtgSpSetPrm( sposing, hdl->prm, wk.TargetSpd );

		/* 減速距離計算 */
		ChkOverflow = mtgSpCalDecPrm(sposing, hdl->basePrm, FALSE,
								wk.CurrentSpd, &wk.StopDistance, &wk.StopDistance0);

		/* 減速判定距離 = 残距離 - 減速距離 - 指令速度 */
		ChkDecDistance = mtgSpCheckDecelaration(&wk, hdl->basePrm);

		/* 目標位置が逆方向？or 減速距離オーバ？ → 減速 */
		if( (ChkReverse == FALSE) || (ChkOverflow != FALSE) )
		{
			ChkEnd = FALSE; 									/* SPOSING終了しない		*/
			sposing->StopDistRem = 0;							/* 残距離端数は払いださない	*/
			sposing->State = SPOSING_DEC_MODE2;					/* 次周期 減速モード2へ		*/
		}
		/* 初期化モードのとき */
		else if( sposing->State == SPOSING_INI_MODE )
		{
			/* 目標速度 ≧ 現在速度？ */
			if( (sposing->TargetSpd >= wk.CurrentSpd) || ChkDecDistance < 0)
			{
				/* 加速パラメータ計算処理	*/
				mtgSpCalAccPrm( sposing, wk.CurrentSpd );	
				sposing->State = SPOSING_ACC_MODE;				/* 加速 定速モードへ		*/
			}
			else
			{	/* 減速距離計算、減速パラメータ計算処理		*/
				mtgSpCalDecPrm( sposing, hdl->basePrm, ChkReverse,
										wk.CurrentSpd, &wk.StopDistance, &wk.ErrorDistance0);
				sposing->State = SPOSING_DEC_MODE1;				/*減速モード1へ(目標速度変更)*/
			}
		}
		else
		{
			/* 通常処理(モード変更なし) */
		}
	}

	/*--------------------------------------------------------------------------------------*/
	/*		SPOSING 指令生成処理															*/
	/*--------------------------------------------------------------------------------------*/
	switch( sposing->State )
	{	/*----------------------------------------------------------------------------------*/
		/*	加速＆定速モード																*/
		/*----------------------------------------------------------------------------------*/
		case SPOSING_ACC_MODE :

			/* 残距離≦減速距離？ → 減速 */
			if( ChkDecDistance < 0 )
			{
				/* 残距離端数を計算 */
				sposing->StopDistRem = ChkDecDistance + wk.CurrentSpd;

				/* 残距離端数(分割)を計算 */
				if( sposing->TmpDecVal != 0 )
				{
					sposing->jStopDistRem = ((ULONG)sposing->StopDistRem / (ULONG)sposing->TmpDecVal);
					sposing->rStopDistRem = sposing->StopDistRem 
									- ( sposing->jStopDistRem * sposing->TmpDecVal);
					sposing->rdStopDistRem = (sposing->rStopDistRem / sposing->DecCnt) + 1;
				}
				else
				{
					sposing->jStopDistRem = sposing->StopDistRem;
				}

				if( sposing->StopDistRem < 0 )
				{/* 行き過ぎ */
					sposing->StopDistRem = 0;					/* 残距離端数は払いださない	*/
					ChkEnd = FALSE;								/* SPOSING終了しない		*/
				}
				sposing->State = SPOSING_DEC_MODE2;				/* 次周期減速モード2へ		*/
				wk.Segment = wk.CurrentSpd;						/* セグメント払い出し		*/
			}
			/* 残距離＜減速距離？ */
			else
			{
				/* 現在速度 < 目標速度に到達していない */
				if( wk.CurrentSpd < sposing->TargetSpd )
				{
					/* 加速処理		*/
					wk.CurrentSpd = mtgSpAccelerator( sposing, wk.CurrentSpd );
					/* 残距離計算	*/
					ChkOverflow   = mtgSpCalDecPrm( sposing, hdl->basePrm, ChkReverse, 
													wk.CurrentSpd, &wk.StopDistance, &wk.StopDistance0 );

					/* 減速判定距離 = 残距離 - 減速距離 - 指令速度 */
					ChkDecDistance = mtgSpCheckDecelaration(&wk, hdl->basePrm);

					/* 加速定数で加速できる？ */
					if((ChkDecDistance < 0) || (ChkOverflow != 0))
					{
						/* 加速定数で加速できない場合	*/
						/* 前回速度は0か？				*/
						if(wk.LastSpd != 0)
						{	/* 前回の速度にする */
							wk.Segment = wk.CurrentSpd = wk.LastSpd;
						}
						else
						{/* 前回速度=0なら数パルス送り */
							/* ミニマム定数で加速 */
							if( sposing->InitDec >= sposing->InitAcc )
							{
								wk.CurrentSpd = wk.LastSpd + sposing->InitAcc;
							}
							else
							{
								wk.CurrentSpd = wk.LastSpd + sposing->InitDec;
							}
							buf = wk.ErrorDistance << hdl->basePrm->vel_shift;
							if( wk.CurrentSpd >= buf )
							{
								/* 残距離を速度にする */
								wk.Segment = wk.CurrentSpd = buf;
							}
							else
							{
								/* ミニマム加速度で加速した速度にする */
								wk.Segment = wk.CurrentSpd;
							}
						}
					}
					else
					{	/* 加速定数で加速できる場合は加速 */
						wk.Segment = wk.CurrentSpd;
						if( wk.CurrentSpd == wk.LastSpd )
						{
							sposing->StopDistRem = ( ChkDecDistance << hdl->basePrm->vel_shift );
							if( hdl->sposing.StopDistRem  > wk.LastSpd )
							{
								wk.Segment = sposing->StopDistRem;
							}
						}

					}
				}
				else
				{	/* 現在速度 = 目標速度に到達している場合は速度維持 */
					wk.Segment = wk.CurrentSpd;
				}
			}
			break;
		/*----------------------------------------------------------------------------------*/
		/*	減速モード1(目標速度変更)														*/
		/*----------------------------------------------------------------------------------*/
		case SPOSING_DEC_MODE1 : 							/* 目標速度 < 現在速度 			*/
			sposing->StopDistRem = 0;						/* 残距離端数は払い出さない		*/
			/* 減速処理	*/
			wk.CurrentSpd = mtgSpDecelerator( sposing, wk.CurrentSpd, &wk.Segment );

			if( wk.Segment == sposing->TargetSpd )
			{	/*--------------------------------------------------------------------------*/
				/* 目標速度到達で、次周期加速モードへ										*/
				/*--------------------------------------------------------------------------*/
				mtgSpSetPrm( sposing, hdl->prm, wk.TargetSpd );	/* 初期化 					*/
				/* 加速パラメータ計算処理	*/
				mtgSpCalAccPrm( sposing, wk.CurrentSpd );
				sposing->State = SPOSING_ACC_MODE;				/* 次周期加速モードへ 		*/
			}
			ChkEnd = FALSE;										/* SPOSING終了しない		*/
			break;
		/*----------------------------------------------------------------------------------*/
		/*	減速モード2(位置決め)															*/
		/*----------------------------------------------------------------------------------*/
		case SPOSING_DEC_MODE2 : 								/* 現在速度≠0(0 < Vt <= V)	*/
			if( (wk.CurrentSpd == 0) && (wk.ErrorDistance != 0) )
			{
				//if(wk.ErrorDistance != 1)
				//{
					//int a=0;
				//}
				/*--------------------------------------------------------------------------*/
				/* 現在速度=0でも残距離があったら再加速(次周期加速モードへ) 				*/
				/*--------------------------------------------------------------------------*/
				mtgSpSetPrm( sposing, hdl->prm, wk.TargetSpd );	/* 初期化					*/
				/* 加速パラメータ計算処理	*/
				mtgSpCalAccPrm( sposing, wk.CurrentSpd );	
				sposing->State = SPOSING_ACC_MODE;				/* 次周期加速モードへ		*/
//				ChkEnd = FALSE;									/* SPOSING終了しない		*/
			}

			/* 減速処理	*/
			wk.CurrentSpd = mtgSpDecelerator( sposing, wk.CurrentSpd, &wk.Segment );
			break;
		default:
			 break;
	}
	/*--------------------------------------------------------------------------------------*/
	/* 指令速度を格納																		*/
	/*--------------------------------------------------------------------------------------*/
	hdl->Cspd = wk.CurrentSpd;
	/*--------------------------------------------------------------------------------------*/
	/* 少数部を積算																			*/
	/*--------------------------------------------------------------------------------------*/
	buf = hdl->dpos0 + ( (wk.Segment << hdl->basePrm->vel_shift_s) & VELREM_BIT_MSK );
	hdl->dpos0 = buf & VELREM_BIT_MSK;
	if( buf > VELREM_BIT_MSK )
	{
		buf = ( wk.Segment >> hdl->basePrm->vel_shift ) + 1;
	}
	else
	{
		buf = ( wk.Segment >> hdl->basePrm->vel_shift );
	}
	/*--------------------------------------------------------------------------------------*/
	/* 払い出し方向による符号拡張して払い出し												*/
	/*--------------------------------------------------------------------------------------*/
	if( (inOut->DposIn == 1) || (inOut->DposIn == -1) )
	{/* 強制終了処理(残距離1のときは強制定期に1を払い出す) */
		inOut->DposOut = inOut->DposIn;
	}
	else if( hdl->cMinusDir )
	{/* 符号拡張 */
		inOut->DposOut = (~buf) + 1;
	}
	else
	{
		inOut->DposOut = buf;
	}
	/* モニタ用(for debug) */
	sposing->AccMonitor = wk.Segment - sposing->LastSegment;
	sposing->LastSegment = wk.Segment;
	/*--------------------------------------------------------------------------------------*/
	/* 終了処理																				*/
	/*--------------------------------------------------------------------------------------*/
	if((inOut->DposOut == inOut->DposIn) && (ChkEnd == TRUE) )
//	if( (inOut->DposOut == inOut->DposIn) && (ChkEnd == TRUE) && (hdl->Cspd == 0) )
	{
		sposing->State = SPOSING_INI_MODE;
		hdl->Cspd = 0;
		return TRUE;
	}

	return FALSE;
}

/*********************************************************************************************
 SPOSING 加減速、S字パラメータセット

	速度、位置が変更されたときに速度初期化、加減速初期化、S字時間比率初期化を行う。
*********************************************************************************************/
void mtgSpSetPrm(MG_SPOSING* sposing, MG_PRM* prm, LONG TargetSpd)
{
	sposing->TargetSpd = TargetSpd;							/* 目標速度初期化				*/
	sposing->InitAcc = prm->accrate2;						/* 加速度初期化					*/
	sposing->InitDec = prm->decrate2;						/* 減速度初期化					*/
	sposing->CurrentSratio = prm->Sratio;					/* S字時間比率初期化			*/
	
	/* S字加減速比率LIMIT  */
	if( prm->Sratio <= S_RATIO_MIN )
	{
		sposing->CurrentSratio = S_RATIO_MIN;
	}
	if( prm->Sratio >= S_RATIO_MAX )
	{
		sposing->CurrentSratio = S_RATIO_MAX;
	}

}

/*********************************************************************************************
	SPOSING 加速パラメータ計算処理

	SPOSING処理に必要な内部パラメータの設定。
	S字加速時間 = 0、Jerk < 1となる場合には、POSINGとする(S字がつくれないため)
*********************************************************************************************/
static void mtgSpCalAccPrm( MG_SPOSING* sposing, LONG CurrentSpd )
{
	LONG work;
	LONG Ta,Taj;
	LONG spd;
	LONG xx[2] = {0};

	/*--------------------------------------------------------------------------------------*/
	/* 目標速度の設定																		*/
	/*--------------------------------------------------------------------------------------*/
	sposing->InitSpdAcc = CurrentSpd;							/* SPOSING開始初速度		*/
	spd = sposing->TargetSpd - CurrentSpd;						/* SPOSING目標速度(差)		*/
	if( sposing->InitAcc == 0 )
	{	
		sposing->InitAcc = 1;
	}
	/*--------------------------------------------------------------------------------------*/
	/* 加速回数算出																			*/
	/*--------------------------------------------------------------------------------------*/
	Ta  = (LONG)( spd / sposing->InitAcc );						/* 加速回数計算 加速時間 	*/
	Taj = (LONG)( (Ta * sposing->CurrentSratio) / 100 );		/* S時加減速時間		 	*/
	MlibMul3232( Taj, (Ta - Taj), xx );
	/*--------------------------------------------------------------------------------------*/
	/* Jerk(加速)、加速切り替え速度の算出													*/
	/*--------------------------------------------------------------------------------------*/
	if( (Taj != 0) && (xx[1] == 0) )
	{
		sposing->dJerkAcc = (LONG)((ULONG)spd / (ULONG)xx[0]);
	}
	else
	{
		sposing->dJerkDec = 0;
	}
//	if( (Taj != 0) && (sposing->dJerkAcc != 0) )
	if( sposing->dJerkAcc != 0 )
	{	/*----------------------------------------------------------------------------------*/
		/*	SPOSINGのとき																	*/
		/*----------------------------------------------------------------------------------*/
		sposing->JerkAccRate = sposing->dJerkAcc * Taj;			/* SPOSINGでの加速度の最大値*/
		work = (sposing->JerkAccRate * Taj) >> 1;
		sposing->SpdRemAcc = spd - sposing->JerkAccRate * (Ta-Taj);	/* 速度端数(加速) 		*/
		sposing->AltSpd_a1 = sposing->InitSpdAcc + work;			/* 加速切り替え速度1	*/
		sposing->AltSpd_a2 = sposing->TargetSpd						/* 加速切り替え速度2	*/
									- work - sposing->SpdRemAcc;
	}
	else
	{	/*----------------------------------------------------------------------------------*/
		/*	POSINGのとき																	*/
		/*----------------------------------------------------------------------------------*/
		sposing->dJerkAcc    = sposing->InitAcc;				/* 加速度は一定				*/
		sposing->JerkAccRate = sposing->InitAcc;				/* 加速度は一定				*/
		sposing->SpdRemAcc = 0;									/* 速度端数(加速)			*/
		sposing->AltSpd_a1 = sposing->InitSpdAcc;				/* 加速切り替え速度1		*/
		sposing->AltSpd_a2 = sposing->TargetSpd;				/* 加速切り替え速度2		*/
	}
	/*--------------------------------------------------------------------------------------*/
	/*	共通変数初期化 																		*/
	/*--------------------------------------------------------------------------------------*/
	sposing->CurrentAccRate = 0;								/* 加速度指令				*/
	sposing->dSpdRemAcc = sposing->SpdRemAcc;
	if( Ta != 0 )
	{
		sposing->dSpdRemAcc = (sposing->SpdRemAcc / Ta) + 1;
	}
}


/********************************************************************************************
	SPOSING 減速時パラメータ計算処理

	SPOSING処理に必要な内部パラメータの設定
	減速距離算出のために必要なデータもここで算出されるので減速距離の算出もここで行う
	S字加速時間 = 0、Jerk < 1となる場合には、POSINGとする(S字がつくれないため)
*********************************************************************************************/
//static BOOL mtgSpCalDecPrm(MG_SPOSING* sposing, MC_BASE_PRM* bPrm, BOOL ChkReverse,
//								LONG CurrentSpd, LONG *StopDistance, ULONG *StopDistance0 )
BOOL mtgSpCalDecPrm(MG_SPOSING* sposing, MC_BASE_PRM* bPrm, BOOL ChkReverse,
								LONG CurrentSpd, LONG *StopDistance, ULONG *StopDistance0 )
{
	LONG tmp;
	LONG Td,Tdj;
	LONG spd,tspd,tdec;
	LONG work[2]  = {0}; 									/* 演算結果格納用ワークデータ */
	LONG work1[2] = {0};
	LONG work2[2] = {0};
	LONG xx[2] = {0};

	tdec = 0;
	sposing->TmpDecVal = 0;
	sposing->DecCnt  = 0;
	sposing->dStopDistRem = 0;
	sposing->jStopDistRem = 0;
	sposing->rStopDistRem = 0;
	sposing->rdStopDistRem = 0;
	
	if( CurrentSpd != 0 )
	{
		/*----------------------------------------------------------------------------------*/
		/* 目標速度の設定																	*/
		/*----------------------------------------------------------------------------------*/
		if( (sposing->TargetSpd >= CurrentSpd) || (ChkReverse == FALSE) )
		{	/* ChkReverse = FALSEのとき、および												*/
			/* 目標位置が移動方向と反対の場合は速度0まで強制的に減速させる					*/
			tspd = 0;
		}
		else
		{
			tspd = sposing->TargetSpd;
		}
		/*----------------------------------------------------------------------------------*/
		/* 減速回数算出																		*/
		/*----------------------------------------------------------------------------------*/
		spd = CurrentSpd - tspd;
		if( sposing->InitDec==0 )
		{
			sposing->InitDec = 1;
		}
		Td  = (LONG)(spd / sposing->InitDec);				/* 減速回数計算					*/
		Tdj = (LONG)(Td * sposing->CurrentSratio / 100);	/* S時加減速時間				*/
		MlibMul3232( Tdj, (Td - Tdj), xx );
		/*----------------------------------------------------------------------------------*/
		/* Jerk(減速)、減速切り替え速度の算出												*/
		/*----------------------------------------------------------------------------------*/
		if( (Tdj != 0) && (xx[1] == 0) )
		{
			sposing->dJerkDec  = (LONG)((ULONG)spd / (ULONG)xx[0]);
			sposing->TmpDecVal = xx[0];
		}
		else
		{
			sposing->dJerkDec  = 0;
			sposing->TmpDecVal = 0xFFFFFFFF;
		}

//		if( (Tdj != 0) && (sposing->dJerkDec != 0) )
		if( sposing->dJerkDec != 0 )
		{	/*------------------------------------------------------------------------------*/
			/*	SPOSINGのとき																*/
			/*------------------------------------------------------------------------------*/
			sposing->JerkDecRate = sposing->dJerkDec * Tdj;	/* 減速度最大値					*/
			tmp = (sposing->JerkDecRate * Tdj) >> 1;
			sposing->SpdRemDec = spd - (sposing->JerkDecRate * (Td-Tdj));/* 速度端数(減速)	*/
			sposing->AltSpd_d1 = tspd + tmp + sposing->SpdRemDec;/* 減速切り替え速度1		*/
			sposing->AltSpd_d2 = CurrentSpd - tmp;				 /* 減速切り替え速度2		*/
////			work1[0] = ((sposing->SpeedRemDec - spdrem) * (tcnt + 1) * 2) 
////															+ (spdrem * (tcnt + 1));
			tdec = mtgSpCalSpdRemDist( sposing, Td, work1 );	/* 速度端数の払い出し回数	*/
			sposing->DecCnt = Td;						/* 減速端数分割処理のための減速回数	*/
			Td = Td - 1;
		}
		else
		{	/*------------------------------------------------------------------------------*/
			/*	POSINGのとき																*/
			/*------------------------------------------------------------------------------*/
			if( Td == 0 )
			{
				sposing->dJerkDec	 = spd;					/* 減速度						*/
				sposing->JerkDecRate = spd;					/* 減速度						*/
				sposing->SpdRemDec = 0;						/* 減速度端数					*/
				Td = 1;
			}
			else
			{
				sposing->dJerkDec	 = spd / Td;			/* 減速度						*/
				sposing->JerkDecRate = sposing->dJerkDec;	/* 減速度						*/
				sposing->SpdRemDec = spd - (sposing->JerkDecRate * Td);/* 速度端数(減速)	*/
				tdec = mtgSpCalSpdRemDist( sposing, Td, work1 );/* 速度端数の払い出し回数	*/
			}
			sposing->AltSpd_d1 = tspd;						/* 減速切り替え速度1			*/
			sposing->AltSpd_d2 = CurrentSpd;				/* 減速切り替え速度2			*/
			sposing->DecCnt = Td;						/* 減速端数分割処理のための減速回数	*/
		}
		/*----------------------------------------------------------------------------------*/
		/*	共通変数初期化 																	*/
		/*----------------------------------------------------------------------------------*/
		sposing->InitSpdDec = CurrentSpd;					/* SPOSING開始初速度			*/
		sposing->EndSpdDec  = tspd;							/* SPOSING終了速度				*/
		sposing->CurrentDecRate = 0;						/* 減速度初期化					*/
		sposing->dSpdRemDec = tdec;							/* 速度端数分割速度(減速)		*/
		/*----------------------------------------------------------------------------------*/
		/* 減速距離計算 																	*/
		/*----------------------------------------------------------------------------------*/
		MlibMul3232( (spd - sposing->SpdRemDec), (Td + 1), work2 );
		MlibAdd6464( work1, work2, work );
	}
/*********************************/
//	debug_xx[i][0]=work2[0];
//	debug_xx[i][1]=work2[1];
//	i++;
//	if(i>1000){i=0;}
/*********************************/	
	/*--------------------------------------------------------------------------------------*/
	/* 減速距離(整数部) = (減速距離計算結果(整数部)) / 2 									*/
	/*--------------------------------------------------------------------------------------*/
	*StopDistance  = ((work[1] << (31-bPrm->vel_shift))
								 | ((work[0] >> (bPrm->vel_shift+1)) & (bPrm->vel_max-1)));
	/*--------------------------------------------------------------------------------------*/
	/* 減速距離(小数部) = (減速距離計算結果(小数部)) / 2 									*/
	/*--------------------------------------------------------------------------------------*/
	*StopDistance0 = (work[0] << (bPrm->vel_shift_s-1)) & VELREM_BIT_MSK;
	/*--------------------------------------------------------------------------------------*/
	/* 減速距離オーバフローチェック 														*/
	/*--------------------------------------------------------------------------------------*/
	if( (work[1] >> (bPrm->vel_shift-1)) != 0 )
	{
		return TRUE;										/* オーバーフロー				*/
	}
	return FALSE;
}

/*********************************************************************************************
  SPOSING 速度端数（減速）処理

  速度端数（減速）の停止距離を算出する。
  速度端数（減速）の払い出し回数を返す。
*********************************************************************************************/
static LONG mtgSpCalSpdRemDist( MG_SPOSING* sposing, LONG Td, LONG* work )
{
	LONG tdec,tcnt;
	LONG spdrem;

	tdec = (sposing->SpdRemDec / Td) + 1;
	tcnt = sposing->SpdRemDec / tdec;
	spdrem = tcnt * tdec;
	MlibMul3232( ((sposing->SpdRemDec * 2) - spdrem), (tcnt + 1), work );
	return tdec;
}

/*********************************************************************************************
 SPOSING  加速処理
 *********************************************************************************************/
static LONG mtgSpAccelerator( MG_SPOSING* sposing, LONG CurrentSpd )
{
	LONG CurrentAccRate;										/* 今回加速度 				*/
	LONG tspd,dSpdRem;

	tspd = sposing->TargetSpd; 									/* 目標速度 				*/
	dSpdRem = 0;

	/*--------------------------------------------------------------------------------------*/
	/* Jerk++																				*/
	/*--------------------------------------------------------------------------------------*/
	if( (sposing->InitSpdAcc <= CurrentSpd) && (CurrentSpd <= sposing->AltSpd_a1) )
	{
		sposing->CurrentAccRate += sposing->dJerkAcc;			/* 加速度をdJerkAccだけ加算	*/
		if( sposing->CurrentAccRate > sposing->JerkAccRate )
		{
			sposing->CurrentAccRate = sposing->JerkAccRate;		/* LIMIT処理				*/
		}
		CurrentAccRate = sposing->CurrentAccRate;				/* 今回加速度				*/
	}
	/*--------------------------------------------------------------------------------------*/
	/* Jerk constant																		*/
	/*--------------------------------------------------------------------------------------*/
	else if( CurrentSpd < sposing->AltSpd_a2 )	
	{
		CurrentAccRate = sposing->JerkAccRate;					/* 今回加速度				*/
	}
	/*--------------------------------------------------------------------------------------*/
	/* Jerk--																				*/
	/*--------------------------------------------------------------------------------------*/
//	else if( CurrentSpd <= tspd )	
	else
	{
		sposing->CurrentAccRate -= sposing->dJerkAcc;			/* 加速度をdJerkAccだけ減算	*/
		if( sposing->CurrentAccRate < 0 )
		{
			sposing->CurrentAccRate = sposing->dJerkAcc;		/* LIMIT処理 				*/
		}
		CurrentAccRate = sposing->CurrentAccRate;				/* 今回加速度 				*/
	}
	/*--------------------------------------------------------------------------------------*/
	/* 速度端数払い出し処理（分割方式）														*/
	/*--------------------------------------------------------------------------------------*/
	if( sposing->SpdRemAcc != 0 )
	{
		if( sposing->SpdRemAcc > sposing->dSpdRemAcc )
		{
			dSpdRem = sposing->dSpdRemAcc;
			sposing->SpdRemAcc -= sposing->dSpdRemAcc;
		}
		else
		{
			dSpdRem = sposing->SpdRemAcc;
			sposing->SpdRemAcc = 0;
		}
	}
	/*--------------------------------------------------------------------------------------*/
	/* 指令速度を算出																		*/
	/*--------------------------------------------------------------------------------------*/
	CurrentSpd = CurrentSpd + CurrentAccRate + dSpdRem;
	/*--------------------------------------------------------------------------------------*/
	/* 切り替え速度を調整																	*/
	/*--------------------------------------------------------------------------------------*/
	sposing->AltSpd_a1 += dSpdRem;
	sposing->AltSpd_a2 += dSpdRem;
	/*--------------------------------------------------------------------------------------*/
	/* LIMIT処理 																			*/
	/*--------------------------------------------------------------------------------------*/
	if( CurrentSpd >= tspd )
	{
		CurrentSpd = tspd;
	}
	return CurrentSpd;
}

/*********************************************************************************************
  SPOSING 減速処理

  現在の指令速度から減速させた指令速度を計算する。
  速度端数、残距離端数はここで払い出す。
*********************************************************************************************/
static LONG mtgSpDecelerator( MG_SPOSING* sposing, LONG CurrentSpd, LONG *Segment )
{
	LONG lRem;
	LONG CurrentDecRate;
	LONG dSpdRem, dStopDistRem;

	lRem  = sposing->StopDistRem;
	dSpdRem = 0;
	dStopDistRem = 0;

	/*--------------------------------------------------------------------------------------*/
	/* Jerk++																				*/
	/*--------------------------------------------------------------------------------------*/
	if( sposing->AltSpd_d2 <= CurrentSpd )
	{
		sposing->CurrentDecRate += sposing->dJerkDec;			/* 減速度をdJerkDecだけ加算	*/
		if( sposing->CurrentDecRate > sposing->JerkDecRate )	/* LIMIT処理 				*/
		{
			sposing->CurrentDecRate = sposing->JerkDecRate;
		}
		CurrentDecRate = sposing->CurrentDecRate;				/* 今回減速度 				*/
		sposing->dStopDistRem += sposing->jStopDistRem;	/* 残距離端数をjStopDistRemだけ加算	*/
	}
	/*--------------------------------------------------------------------------------------*/
	/* Jerk constant 																		*/
	/*--------------------------------------------------------------------------------------*/
	else if( sposing->AltSpd_d1 < CurrentSpd )
	{
		CurrentDecRate = sposing->JerkDecRate;					/* 今回減速度				*/
	}
	/*--------------------------------------------------------------------------------------*/
	/* Jerk--																				*/
	/*--------------------------------------------------------------------------------------*/
// 	else if( sposing->EndSpdDec <= CurrentSpd )
 	else
	{
 		sposing->CurrentDecRate -= sposing->dJerkDec;			/* 減速度をdJerkDecだけ減算	*/
		if( sposing->CurrentDecRate < 0 )						/* LIMIT処理 				*/
		{
			sposing->CurrentDecRate = 0;
		}
		CurrentDecRate = sposing->CurrentDecRate;				/* 今回減速度				*/
		sposing->dStopDistRem -= sposing->jStopDistRem;	/* 残距離端数をjStopDistRemだけ減算	*/
	}
	/*--------------------------------------------------------------------------------------*/
	/* 速度端数払い出し処理（分割方式）														*/
	/*--------------------------------------------------------------------------------------*/
	if( sposing->SpdRemDec != 0 )
	{
		if( sposing->SpdRemDec > sposing->dSpdRemDec )
		{
			dSpdRem = sposing->dSpdRemDec;						/* 速度端数今回払い出し量	*/
			sposing->SpdRemDec -= sposing->dSpdRemDec;
		}
		else
		{
			dSpdRem = sposing->SpdRemDec;						/* 速度端数今回払い出し量	*/
			sposing->SpdRemDec = 0;
		}
	}
	/*--------------------------------------------------------------------------------------*/
	/* 指令速度を算出																		*/
	/*--------------------------------------------------------------------------------------*/
	CurrentSpd  = CurrentSpd - CurrentDecRate - dSpdRem;
	/*--------------------------------------------------------------------------------------*/
	/* 残距離端数払い出し処理（分割方式）													*/
	/*--------------------------------------------------------------------------------------*/
	if( sposing->rStopDistRem > sposing->rdStopDistRem )
	{	/* 残距離端数のさらに余り分の分割量を加算		*/
		dStopDistRem += sposing->rdStopDistRem;	
		sposing->rStopDistRem -= sposing->rdStopDistRem;
	}
	else
	{	/* 残距離端数のさらに余り分を払い出す最後の1回 */
		dStopDistRem += sposing->rStopDistRem;
		sposing->rStopDistRem  = 0;
	}

	if( sposing->StopDistRem != 0 )
	{	/* 残距離端数の今回払い出し量を算出		*/
		dStopDistRem += sposing->dStopDistRem;
		sposing->StopDistRem -= dStopDistRem;
		if( sposing->StopDistRem < 0 )
		{	/* 残距離端数を払い出す最後の1回	*/
			sposing->dStopDistRem = lRem;
			sposing->StopDistRem  = 0;
		}
	}
	/*--------------------------------------------------------------------------------------*/
	/* 切り替え速度を調整																	*/
	/*--------------------------------------------------------------------------------------*/
	sposing->AltSpd_d1 -= dSpdRem;
	sposing->AltSpd_d2 -= dSpdRem;
	/*--------------------------------------------------------------------------------------*/
	/* LIMIT処理 																			*/
	/*--------------------------------------------------------------------------------------*/
	if( CurrentSpd < sposing->EndSpdDec )
	{
		CurrentSpd = sposing->EndSpdDec;
	}
	if( CurrentSpd < 0 )
	{
		CurrentSpd = 0;
	}
	/*--------------------------------------------------------------------------------------*/
	/* 今回の払い出し量を更新、現在速度を返す												*/
	/*--------------------------------------------------------------------------------------*/
	*Segment = CurrentSpd + dStopDistRem;
	return CurrentSpd;
}

/*********************************************************************************************
 減速判別用データ演算処理
  概要:  残距離－減速距離－指令速度(残距離端数の最大値)を求める。
         マイナスとなる時小数部までを計算する。
         プラスとなるときはそのまま（減速チェックでは小数点以下不要）
         減速距離が巨大の時はミニマム値(-2147483648)とする。
         -2147483648の移動方向を実現する為,残距離が-2147483648の時は
         強制的に214748367にする。
*********************************************************************************************/
static LONG mtgSpCheckDecelaration(MG_S_POSWK* wk, MC_BASE_PRM* bPrm)
{
	LONG ChkDecDistance;
	LONG CheckDecDistance0;

	/* 減速判定距離 = 残距離 - 停止距離 - 指令速度(残距離端数の最大値) */
	ChkDecDistance = wk->ErrorDistance - wk->StopDistance - (wk->CurrentSpd >> bPrm->vel_shift);

	if( ChkDecDistance > -bPrm->vel_max )/* 最大速度(=2^(31-vel_shift))[ref/cyc] */
	{
		/* 減速距離が巨大 */
		if( ChkDecDistance <= 1 )
		{
			/* 残距離(ErrorDistance)が巨大だと減速する必要なし、1より大きい時は、少数部を引いてもマイナスにならない。*/
			CheckDecDistance0 = (LONG)(wk->ErrorDistance0 - wk->StopDistance0)
									- ((wk->CurrentSpd << bPrm->vel_shift_s) & VELREM_BIT_MSK);  /* 小数部 */

			/* 減速判定距離 = 残距離 - 停止距離 - 残距離端数の最大値 */
			ChkDecDistance = MlibSatAdd32( (ChkDecDistance << bPrm->vel_shift),
														(CheckDecDistance0 >> bPrm->vel_shift_s) );

		}
		if( ChkDecDistance > 0 )
		{
			/* 結果プラスなら小数点以下不要 */
			ChkDecDistance = ChkDecDistance >> bPrm->vel_shift;
		}
	}
	else
	{
		if( wk->ErrorDistance == 0x80000000/*-2147483648*/ )
		{
			/* これは-2147483648の移動量を実現するため */
			ChkDecDistance = 0x7fffffff/*2147483647*/;
		}
		else
		{
			ChkDecDistance = 0x80000000/*-2147483648*/;
		}
	}
	return ChkDecDistance;
}

/****************************************************************************
  減速度で加減速判断
  概要:  現在の速度から減速定数で加速できるか判断し、速度指令を出力する。
         ＜加速できる条件＞
         減速定数＜加速定数なら加速できそう。
         目標速度未満ならＯＫ（目標速度は加速度でチェック済み）
         加速度変更速度未満ならＯＫ（加速度変更速度は加速度でチェック済み）
****************************************************************************/
#if 0
static LONG mtgAccelerateDecRateSposing(MG_S_POSWK*	wk, MG_SPOSING* SposingPrm, MC_BASE_PRM* bPrm)
{

	LONG spd,S,work;

	// 減速距離計算
	S = (wk->ErrorDistance << bPrm->vel_shift) / SposingPrm->InitDec;//wk->ErrorDistanceが巨大だったらオーバーフロー！！
	work = 1 + 8 * S;
	spd  = (-1 + MlibSqrtu32u32( work, 1))>>1;
	
	if(spd <= 1)
	{
		wk->Segment = wk->ErrorDistance << bPrm->vel_shift;
		wk->CurrentSpeed = 0;
	}
	else
	{
		wk->CurrentSpeed = ((spd-1) * SposingPrm->InitDec);
		wk->Segment = wk->CurrentSpeed;
	}

	return spd;
}
#endif


/***************************************** end of file **********************************************/
