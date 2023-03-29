/****************************************************************************
Description	: ProgramJog.c : プログラムJOG関連モジュール
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
T.Kira 2011.03.8    初版
****************************************************************************/
//#include "SystemSetting.h"
#include "ProgramJog.h"

/****************************************************************************
		プログラムJOG初期パラメータ計算
****************************************************************************/
/****************************************************************************
	MlibIpcalPcmdMaker()の第1引数はkxgain{kx,sx}:-2^23≦kx≦2^23，0≦sx≦24であるが，	
	中間変数のkxgain{kx,sx}:-2^23≦kx≦2^23，-127≦sx≦127が可能になった場合は<V507>を	
	削除する必要がある<V527>
****************************************************************************/
void	IprmcalPrgJog( PJOGHNDL *PJogHdl, BPRMDAT *Bprm, POS_MNG_HNDL *PosManager )
{
	LONG PcmdOvrSpd, kx, sx;
	LONG Egear_a, Egear_b;

	PCMKPV  *PJogPcmk;
	PJOGV	*PJogV;

	PJogV = &PJogHdl->PJogV;
	PJogPcmk = &PJogHdl->PJogPcmk;
	Egear_a = PosManager->conf.Egear.a;
	Egear_b = PosManager->conf.Egear.b;

	kx = MlibScalKxksks( 1,  Bprm->OvrSpd,    Bprm->Kfbpls,  &sx,  0 );
	kx = MlibPcalKxgain( kx, Egear_a, Egear_b, &sx, 0 );
	kx = MlibPcalKxgain( kx, Bprm->SvCycleNs, 1000000000,   &sx, 24 );
	PcmdOvrSpd = kx;

	MlibIpcalPcmdMaker( PcmdOvrSpd, Bprm->NorMaxSpd, Bprm->NorOvrSpd, FALSE, &PJogPcmk->P );
	/* <V507> add start */
	if( PcmdOvrSpd == 0x7FFFFF )
	{
		PJogV->Cnst.PrmUnMatch2 = TRUE;
	}
	/* <V507> add end */
}

/****************************************************************************
		プログラムJOG運転指令作成変数リセット
****************************************************************************/
void PrgJogReset( PJOGHNDL *PJogHdl )
{
	PJogHdl->PJogV.Var.AbsPosCmd = 0;
	MlibResetLongMemory( &PJogHdl->PJogPcmk.V, sizeof( PJogHdl->PJogPcmk.V )>>2 );
}
/****************************************************************************************************/
/*																									*/
/*		プログラムJOG運転指令シーケンス(ScanCでCALL)												*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : ラウンドからのプログラムJOG運転操作から、定数設定された運転パターンの					*/
/*			指令をスキャンBへ送る。																	*/
/*																									*/
/*		 シーケンス 0: 運転開始待ち																	*/
/*				ラウンドからの運転開始指令により、運転パターン実施のシーケンス						*/
/*				処理をおこなう。シーケンス1へ。														*/
/*																									*/
/*		 シーケンス 1: 待ち時間経過待ち																*/
/*				待ち時間経過後、最初の運転指令をスキャンBへ送る。									*/
/*				シーケンス2へ。																		*/
/*																									*/
/*		 シーケンス 2: 運転指令払出し待ち															*/
/*				スキャンBからの指令払い出し完了を待つ。												*/
/*				指令払い出し完了により、シーケンス3へ。												*/
/*																									*/
/*		 シーケンス 3: 運転終了判定																	*/
/*				繰返し回数更新と運転回数の比較から運転が終了したか判定する。						*/
/*				運転終了の場合、シーケンス5へ。														*/
/*				運転未終了の場合、シーケンス4へ。													*/
/*																									*/
/*		 シーケンス 4: 運転パターンに依る分岐														*/
/*				運転パターンに従って、運転指令をスキャンBへ送る。									*/
/*				シーケンス2へ。異常系はシーケンス5へ。												*/
/*																									*/
/*		 シーケンス 5: 運転終了																		*/
/*				処理後、シーケンス0へ。																*/
/*																									*/
/*		 シーケンス 6: 運転中断																		*/
/*				処理後、シーケンス0へ。																*/
/*																									*/
/*		 シーケンス 7: アラーム発生時																*/
/*				処理後、シーケンス0へ。																*/
/*																									*/
/*																									*/
/****************************************************************************************************/
void	CpxPrgJogPosSequence( PJOGHNDL *PJogHdl, SEQ_CTRL_OUT *SeqCtrlOut,
													ALMMAN_HANDLE *AlmManager, LONG FnCtrlMcmd ) 
{
	PJOGV	*PJogV;
	PCMKPV  *PJogPcmk;

	PJogV = &PJogHdl->PJogV;
	PJogPcmk = &PJogHdl->PJogPcmk;

	/* プログラムJOGモードチェック */
	if( FnCtrlMcmd != CTRLMODE_PJOG )
	{
		return;
	}
	
	/* プログラムJOG運転条件チェック						*/
	/* アラーム発生中/HWBB状態中/主回路遮断/運転中のOT有効	*/	
	if( ((AlmManager)->Status.AlmFlag != FALSE)			/* <S0E5> */ /* <S0FF> */
		|| (SeqCtrlOut->HwbbSts == TRUE)
		|| (SeqCtrlOut->MainPowerOn != TRUE)
		|| ((PJogV->Var.State == PJOG_START) && (SeqCtrlOut->OverTrvlSts == TRUE)) )
	{
		PJogV->Var.SeqPtr = 7;
	}
	else if( PJogV->Var.RoutCmd == PJOGCMD_ABORT )	/* 運転中断 */
	{
		PJogV->Var.SeqPtr = 6;
	}
	else if( (PJogV->Var.State == PJOG_START) && (SeqCtrlOut->BaseEnableReq == FALSE) )	/* 運転中のサーボオフ */
	{
		PJogV->Var.SeqPtr = 6;
	}
	if( PJogV->Var.RoutCmd == PJOGCMD_INIT )
	{
		PJogV->Var.SeqPtr = 0;
		PJogV->Var.CoutCmd = PJOGCMD_INIT;
		PJogV->Var.State = PJOG_INIT;
	}

	/* プログラムJOG運転指令シーケンス */
	switch ( PJogV->Var.SeqPtr )
	{
		case 0 :	/* 運転初期状態	*/
			if( (PJogV->Var.RoutCmd == PJOGCMD_START) && (SeqCtrlOut->BaseEnableReq == TRUE) )
			{
				PJogV->Var.MCntr = 0;
				KlibRstLongTimer(&PJogV->Var.TimeWork);
				if( (PJogV->Cnst.MTimes == 0) && ((PJogV->Cnst.Pattern == 2) || (PJogV->Cnst.Pattern == 3)) )
				{
					PJogV->Var.SeqPtr = 7;
				}
				else
				{
					PJogV->Var.SeqPtr = 1;
				}
				PJogV->Var.State = PJOG_START;
			}
			break;

		case 1 :	/* 運転待ち状態	*/
			if( KlibGetLongTimerMs(PJogV->Var.TimeWork) >= (ULONG)PJogV->Cnst.WaitTime )
			{
				PrgJogReset( PJogHdl );
				PJogV->Var.CoutCmd = PJOGCMD_START;
				if( PJogV->Cnst.Pattern & 0x01 )
				{
					PJogV->Var.AbsPosCmd = -PJogV->Cnst.Distance;
				}
				else
				{
					PJogV->Var.AbsPosCmd = PJogV->Cnst.Distance;
				}
				PJogV->Var.SeqPtr = 2;
			}
			break;

		case 2 :	/* 位置指令払出し中	*/
			if( PJogPcmk->V.calendf )
			{
				PJogV->Var.SeqPtr = 3;
			}
			break;

		case 3 :	/* 運転終了判定	*/
			if( (++PJogV->Var.MCntr >= PJogV->Cnst.MTimes) && (PJogV->Cnst.MTimes != 0) )
			{
				PJogV->Var.SeqPtr = 5;
			}
			else
			{
				KlibRstLongTimer(&PJogV->Var.TimeWork);
				PJogV->Var.SeqPtr = 4;
			}
			break;

		case 4 :	/* 運転パターン分岐	*/
			if( KlibGetLongTimerMs(PJogV->Var.TimeWork) >= (ULONG)PJogV->Cnst.WaitTime )
			{
				switch ( PJogV->Cnst.Pattern )
				{
					case 0 :
						PJogV->Var.AbsPosCmd += PJogV->Cnst.Distance;
						break;
					case 1 :
						PJogV->Var.AbsPosCmd -= PJogV->Cnst.Distance;
						break;
					case 2 :
						if( PJogV->Var.MCntr < (PJogV->Cnst.MTimes>>1) )
						{
							PJogV->Var.AbsPosCmd += PJogV->Cnst.Distance;
						}
						else
						{
							PJogV->Var.AbsPosCmd -= PJogV->Cnst.Distance;
						}
						break;
					case 3 :
						if( PJogV->Var.MCntr < (PJogV->Cnst.MTimes>>1) )
						{
							PJogV->Var.AbsPosCmd -= PJogV->Cnst.Distance;
						}
						else
						{
							PJogV->Var.AbsPosCmd += PJogV->Cnst.Distance;
						}
						break;
					case 4 :
						if( PJogV->Var.MCntr & 0x01 )
						{
							PJogV->Var.AbsPosCmd -= PJogV->Cnst.Distance;
						}
						else
						{
							PJogV->Var.AbsPosCmd += PJogV->Cnst.Distance;
						}
						break;
					case 5 :
						if( PJogV->Var.MCntr & 0x01 )
						{
							PJogV->Var.AbsPosCmd += PJogV->Cnst.Distance;
						}
						else
						{
							PJogV->Var.AbsPosCmd -= PJogV->Cnst.Distance;
						}
						break;
					default :
						break;
				}
				if( PJogV->Cnst.Pattern < 6 )
				{
					PJogV->Var.SeqPtr = 2;
				}
				else
				{
					PJogV->Var.SeqPtr = 5;
				}
			}
			break;

		case 5 :	/* 運転終了	*/
			PJogV->Var.State = PJOG_END;
			PJogV->Var.CoutCmd = PJOGCMD_INIT;
			PJogV->Var.SeqPtr = 0;
			break;

		case 6 :	/* 運転中断	*/
			PrgJogReset( PJogHdl );		/* 停止	*/
			PJogV->Var.CoutCmd = PJOGCMD_ABORT;
			PJogV->Var.State = PJOG_ABORT;
			PJogV->Var.SeqPtr = 0;
			break;

		case 7 :	/* アラーム発生中	*/
			PrgJogReset( PJogHdl );		/* 停止	*/
			PJogV->Var.CoutCmd = PJOGCMD_ABORT;
			PJogV->Var.State = PJOG_ERROR;
			PJogV->Var.SeqPtr = 0;
			break;

		default :
			break;
	}

	/* 操作レジスタ設定	*/
	PJogHdl->PJogState = (PJogV->Var.State & 0x0F);

	/* ScanCでON,RoundでOFFするフラグ	*/
	PJogV->Var.ScanCRunFlg = TRUE;
}

/****************************************************************************
		位置指令入力処理(プログラムJOG運転モード)(ScanBでCALL)
****************************************************************************/
BOOL PrgJogMakePositionReference( PJOGHNDL *PJogHdl, LONG *dPosRefo )
{
	PJOGV	*PJogV;
	PCMKPV  *PJogPcmk;

	PJogV = &PJogHdl->PJogV;
	PJogPcmk = &PJogHdl->PJogPcmk;

	/* 位置指令作成処理	[指令単位/Scan]	*/
	*dPosRefo = MlibPcmdMaker( PJogV->Var.AbsPosCmd, 0, PJogPcmk->P.maxspd, PJogPcmk, PCMKMODE_POS );
	return( (BOOL)(PJogPcmk->V.calendf ));
}

/****************************************************************************************************/

