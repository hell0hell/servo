/****************************************************************************************************/
/*																									*/
/*																									*/
/*		CheckMotSts.c : モータ状態監視処理定義														*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : モータ動作状態監視処理																	*/
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
/*	Note	:	初版	2010.02.23	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"
#include "CheckMotSts.h"
#include "MLib.h"


/****************************************************************************************************/
/*																									*/
/*		モータ状態検出処理																			*/
/*																									*/
/****************************************************************************************************/
void SMotSts_InputMotorStatus( SEQ_MOT_STS *MotSts, BASE_CTRL_OUT *BaseCtrlOut,
                               SEQ_CTRL_OUT *SeqCtrlOut, BPRMDAT *Bprm )
{
//	MENCV				*MencV;
	LONG				lwk0;

//	MencV = AxisRscC->MencV;

/*--------------------------------------------------------------------------------------------------*/
/*		Input Motor Pulse Data																		*/
/*--------------------------------------------------------------------------------------------------*/
	lwk0 = BaseCtrlOut->MotPos - MotSts->var.MotPos;			/* Cal. Delta Position				*/
	MotSts->var.MotPos  = BaseCtrlOut->MotPos;					/* Update Motor Position			*/
	MotSts->var.dMotPos = lwk0;									/* Update Delta Motor Position		*/

/*--------------------------------------------------------------------------------------------------*/
/*		Input Pulse Command Data																	*/
/*--------------------------------------------------------------------------------------------------*/
	lwk0 = BaseCtrlOut->PcmdCntr - MotSts->var.PcmdCntr;		/* Cal. Delta Pulse					*/
	MotSts->var.PcmdCntr = BaseCtrlOut->PcmdCntr;				/* Update Pulse Command Counter		*/
	MotSts->var.dPcmdCnt = lwk0;								/* Update Delta Pulse Command		*/

/*--------------------------------------------------------------------------------------------------*/
/*		Calculate Motor Speed [2^24/OvrSpd]															*/
/*--------------------------------------------------------------------------------------------------*/
	MotSts->var.LstMotSpd = MotSts->var.MotSpd;
	if( BaseCtrlOut->MotSpdCnt > 0 )
	{
		MotSts->var.MotSpd = BaseCtrlOut->MotSpdSum / BaseCtrlOut->MotSpdCnt;
	}
#if 0
	if( MencV->EncDataNG )									/* Encoder Data NG					*/
	{
		MotSts->var.MotSpd = 0;						/* Signed Motor Speed				*/
	}
#endif
	/* Absolute Motor Speed */
	MotSts->var.AbsMotSpd = MlibLABS( MotSts->var.MotSpd );

	/* Make Acceleration for Mode Switch */
	SeqCtrlOut->ModeSwAcc = MotSts->var.MotSpd - MotSts->var.LstMotSpd;

/*--------------------------------------------------------------------------------------------------*/
/*		Detect Motor Status																			*/
/*--------------------------------------------------------------------------------------------------*/
	/* モータ回転検出処理(TGON) */
	SeqCtrlOut->TgOnSts = LpxCheckMotorRun( MotSts, SeqCtrlOut->TgOnSts );

	/* モータ停止検出処理(MOTSTOP) */
	SeqCtrlOut->MotStop = LpxCheckMotorStop( MotSts->var.AbsMotSpd );

	/* 速度一致検出処理(VCMP) */
	SeqCtrlOut->VcmpSts = LpxCheckMotorVcmp( MotSts,
											 BaseCtrlOut->SpdCtrlRef,
	/*										 (CTRLMODE_SPD == BaseCtrlOut->CtrlModeOut.b.cm) );		*/
										 (( CTRLMODE_SPD == BaseCtrlOut->CtrlModeOut.b.cm ) ||
										  ( CTRLMODE_JOG == BaseCtrlOut->CtrlModeOut.b.cm ) ));	/* <S144> */
      /*Motor Suspend Level Flag <S201>*/
	SeqCtrlOut->MotSuspLvl = LpxCheckMotorSuspendLevel(MotSts,Bprm->NorSuspSpd);
/*--------------------------------------------------------------------------------------------------*/
/*		Un000,Un007 : モータ速度,指令パルス速度														*/
/*--------------------------------------------------------------------------------------------------*/
	MotSts->var.Pfbk64Sum += MotSts->var.dMotPos;
	MotSts->var.Pcmd64Sum += MotSts->var.dPcmdCnt;
	MotSts->var.MotSpd64Sum += MotSts->var.MotSpd;

	MotSts->var.Pfbk64SumCnt++;
	if( MotSts->var.Pfbk64SumCnt >= SVCDEF_UNSPD_SCMEANNUM )		/* 64ms平均演算処理					*/
	{
#if (FLOAT_USE==TRUE)
		/* モータ速度		[pulse/64ms] -> [rpm, mm/s]	*/
		lwk0 = ((MotSts->var.MotSpd64Sum >> 4) + 1) >> 1;
		MotSts->var.UnMotSpd = lwk0 * Bprm->Kmonspd;
		MotSts->var.UnPfbk64ms = MotSts->var.Pfbk64Sum;

		/* 指令パルス速度	[pulse/64ms] -> [rpm, mm/s]	*/
		MotSts->var.UnPcmd64ms = MotSts->var.Pcmd64Sum * Bprm->Kmonpcmdspd ;
		MotSts->var.UnPcmd64Sum = MotSts->var.Pcmd64Sum;		/* <S1B6> */
#else		
		/* モータ速度		[pulse/64ms] -> [rpm, mm/s]	*/
		lwk0 = ((MotSts->var.MotSpd64Sum >> 4) + 1) >> 1;
		MotSts->var.UnMotSpd = MlibMulgainSx48( lwk0, Bprm->Kmonspd );
		MotSts->var.UnPfbk64ms = MotSts->var.Pfbk64Sum;

		/* 指令パルス速度	[pulse/64ms] -> [rpm, mm/s]	*/
		MotSts->var.UnPcmd64ms = MlibMulgain( MotSts->var.Pcmd64Sum, Bprm->Kmonpcmdspd );
		MotSts->var.UnPcmd64Sum = MotSts->var.Pcmd64Sum;		/* <S1B6> */
#endif /* FLOAT_USE */

		MotSts->var.Pfbk64Sum = 0;
		MotSts->var.Pcmd64Sum = 0;
		MotSts->var.Pfbk64SumCnt = 0;
		MotSts->var.MotSpd64Sum = 0;
	}

	MotSts->var.TrqRefMon = BaseCtrlOut->TrqRefMon;
/*--------------------------------------------------------------------------------------------------*/
/*		Un009 : 実効トルク用データ 	( 10sec２乗電流指令平均値 )			UnIsqrSum					*/
/*		Un142 : 実効トルク用データ	( 2ms２乗電流指令値)				UnCrntRef					*/
/*		Un145 : 実効トルク用データ	( 10sec２乗電流指令平均最大値)		UnIsqrSumMax				*/
/*--------------------------------------------------------------------------------------------------*/
	lwk0 = (BaseCtrlOut->IdRefMon * BaseCtrlOut->IdRefMon)
			+ (BaseCtrlOut->IqRefMon * BaseCtrlOut->IqRefMon);
	MotSts->var.UnCrntRef = MlibSqrtu32( lwk0 );
	MotSts->var.IsqrSum += (lwk0 >>14 );

	MotSts->var.IsqrSumCnt++;
	if( MotSts->var.IsqrSumCnt >= KPI_SCANC_MS( 10000 ) )
	{
		lwk0 = MlibSqrtu32( MotSts->var.IsqrSum );
#if (FLOAT_USE==TRUE)
		MotSts->var.UnIsqrSum = lwk0 * Bprm->Kcurload;
#else
		MotSts->var.UnIsqrSum = MlibMulgain( lwk0, Bprm->Kcurload );
#endif /* FLOAT_USE */

		if( MotSts->var.UnIsqrSum > (LONG)MotSts->var.UnIsqrSumMax )	/* ２乗電流積算の最大値				*/
		{
			MotSts->var.UnIsqrSumMax = MotSts->var.UnIsqrSum;
		}
		MotSts->var.IsqrSum = 0;
		MotSts->var.IsqrSumCnt = 0;
	}
}


/****************************************************************************************************/
/*																									*/
/*		モータ回転検出処理																			*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : モータ回転速度の絶対値と回転検出レベル(Pn502)とを比較し、モータ回転速度の方が			*/
/*			大きい場合に、回転検出出力(/TGON)をコネクタＣＮ１より出力する機能。						*/
/*			以前は、回転検出レベル(Pn502)をゼロ速レベルと言っていた。								*/
/*																									*/
/*			本関数では出力用変数のモータ回転検出信号フラグ(CoutV.f.Tgon)をたてる。					*/
/*			回転検出レベル(Pn502)の初期値が20rpmなのでこのまま使えば、								*/
/*			/TGON=LOWで回転中、/TGON=HIGHでほぼ停止中となる。										*/
/*																									*/
/*			Tgon = OFF→ON の条件は、																*/
/*				モータ速度が回転検出レベル(TgonSpdLevel)以上になった時								*/
/*				または、TgonSpdLevel が TGMGN 以上でモータ速度が TgonSpdLevel 以下の時				*/
/*																									*/
/*			Tgon = ON→OFF の条件は、																*/
/*				回転検出レベル(TgonSpdLevel)が TGMGN より大きく、且つ								*/
/*				モータ速度が検出レベル(TgonSpdLevel-TGMGN)以下になった時							*/
/*																									*/
/*			TgonSpdLevel : 回転検出レベル(Pn502)を正規化速度に変換したもの							*/
/*			TGMGN        : チャタリング防止のためのヒステリシス幅を含めたマージン					*/
/*							( 12710 / 16777216(24bit) * 6600(OS) ≒ 5rpm )							*/
/*																									*/
/*																									*/
/****************************************************************************************************/
BOOL LpxCheckMotorRun( SEQ_MOT_STS *MotSts, LONG LstTgonSts )
{
	BOOL	TgOn;
	TgOn = LstTgonSts;

	if( MotSts->var.AbsMotSpd >= MotSts->conf.TgonSpdLevel )
	{ /* モータ速度 ≧ 回転検出レベル */
		TgOn = TRUE;
	}
	else
	{ /* モータ速度 ＜ 回転検出レベル */
		if( MotSts->conf.TgonSpdLevel > MOTSPD_TGMGN )
		{ /* 回転検出レベル ＞ 5[min-1] */
			if( MotSts->var.AbsMotSpd < (MotSts->conf.TgonSpdLevel - MOTSPD_TGMGN) )
			{ /* モータ速度 ＜ (回転検出レベル - 5[min-1]) */
				TgOn = FALSE;
			}
		}
		else
		{ /* 回転検出レベル ≦ 5[min-1] */
			TgOn = TRUE;
		}
	}
	return TgOn;
}



/****************************************************************************************************/
/*																									*/
/*		モータ停止検出処理																			*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : モータが停止したことを検出する。														*/
/*																									*/
/*			モータ停止検出信号は、ＤＢシーケンス、ＯＴシーケンス、ゼロクランプシーケンス、			*/
/*			非常停止処理、ソフトリセット処理、モータ電流検出信号のオフセット自動調整(Fn00E)			*/
/*			で用いられる。																			*/
/*			ＤＢ回路はリレー＋ダイオードかトライアックによる整流により行われる。					*/
/*			ＤＢ電流が切れるのはモータ誘起電圧がダイオードまたはトライアックのオン電圧を			*/
/*			下回ったとき。																			*/
/*			モータ停止検出信号は、ＤＢ電流が切れたことを検出し、ゲート信号のドライブを許可			*/
/*			する条件として使用する。（他の条件にも使うがＤＢの条件に合わせておけばよい）			*/
/*																									*/
/*			ダイオードまたはトライアックが切れる電圧は約０.７Ｖくらいと見なし、ＯＳ回転数			*/
/*			の誘起電圧を電圧飽和レベル１７５Ｖと見なすと、											*/
/*																									*/
/*			MOTSPD_DBLVL = 16777216(OvrSpd) x 0.7/175 = 67108 --> 余裕を見て60000とする。			*/
/*																									*/
/*			モータ回転数がＤＢレベルを下回って充分時間が経ったら、ＤＢ回路は切れたと考えて			*/
/*			良い。																					*/
/*			ＳＧＤではＤＢの際、リレーでモータ線を直接短絡していたため、ＤＢ電流はモータが			*/
/*			完全に止まるまで流れ続ける。このため、モータ停止を検出しから40ms待てばモータが			*/
/*			完全に止まるものと考えている。															*/
/*			ＳＧＤＭではＤＢはダイオード、トライアックによるのでモータが完全に止まらなくても		*/
/*			ＤＢ電流は止まる。だから40ms待つ必要はない。											*/
/*																									*/
/*																									*/
/****************************************************************************************************/
BOOL LpxCheckMotorStop( LONG AbsMotSpd )
{
	BOOL	MotStop;

	if( AbsMotSpd < MOTSPD_DBLVL )
	{
		MotStop = TRUE;								/* モータ停止検出信号の設定(ON)					*/
	}
	else
	{
		MotStop = FALSE;							/* モータ停止検出信号の設定(OFF)				*/
	}

	return	MotStop;
}



/****************************************************************************************************/
/*																									*/
/*		速度一致検出処理																			*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : モータ回転速度と速度指令(ソフトスタート前)の差が速度一致検出幅(Pn503)より				*/
/*			小さい場合、速度一致検出信号フラグをたてる。											*/
/*			速度一致検出信号出力割付(/V-CMP)が設定されている場合、外部信号が出力される。			*/
/*																									*/
/*																									*/
/****************************************************************************************************/
BOOL	LpxCheckMotorVcmp( SEQ_MOT_STS *MotSts, LONG SpdRef, BOOL SpdCtrlMode )
{
	BOOL	Vcmp;

	if( TRUE == SpdCtrlMode )
	{
		if( MlibLABS(SpdRef - MotSts->var.MotSpd) <= MotSts->conf.VcmpSpdLevel )
		{
			Vcmp = TRUE;
		}
		else
		{
			Vcmp = FALSE;
		}
	}
	else
	{
		Vcmp = FALSE;
	}
	return Vcmp;
}



/****************************************************************************************************/
/*																									*/
/*		中断速度レベル検出処理	<S201>																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 :    																			            */
/*			モータ速度が最高速度の10％より大きい場合、中断速度検出信号フラグをTRUEにする。			*/
/*																									*/
/*			最高速度の10％(Bprm.NorSuspSpd) = Bprm.NorMaxSpd / 10									*/
/*																							        */
/****************************************************************************************************/
BOOL  LpxCheckMotorSuspendLevel( SEQ_MOT_STS *MotSts,LONG NorSuspSpd)
{
	BOOL MotSuspLvl;  /*define a BOOL */
	if( MotSts->var.AbsMotSpd > NorSuspSpd)
	{
		MotSuspLvl = TRUE;/* 中断速度検出信号の設定(TRUE)*/
	}
	else
	{
		MotSuspLvl = FALSE; /* 中断速度検出信号の設定(FALSE)*/
	}
	return MotSuspLvl;

}


/***************************************** end of file **********************************************/
