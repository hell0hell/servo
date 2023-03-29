/****************************************************************************************************/
/*																									*/
/*																									*/
/*		PosManager.c : 位置管理関連処理定義															*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 位置管理関連処理																		*/
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
/*				<S00C> ソースコード整理																*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"
#include "SerialEncIf.h"			/* <S053> */
#include "BaseControls.h"
#include "PosManager.h"
#include "ModelControl.h"

/* 2012.08.18 Y.Oka Make Mercury Base Software */
#include "SerialEncIf.h"
/* 2012.08.18 Y.Oka Make Mercury Base Software */

#include "KnlApi.h"	/* @@ 後で消す */

/****************************************************************************************************/
/*		for Position Reference Filter																*/
/****************************************************************************************************/
//#define	AVFILCALCYCLE	KPI_SBCYCLENS				/* 振動抑制フィルタ演算サイクルタイム(ScanB)	[ns]	*/
//#define	MAXOMEGA2(AvfCycleNs)		(127323954/AvfCycleNs)	/* 127323954 = 10^9 * 0.8 / (2*PAI)	周波数2最大値[Hz]	*//* <S065> */

/****************************************************************************************************/
/*		for Position Reference Filter																*/
/****************************************************************************************************/
#define MAFIL_BUFNUM	256									/* 移動平均フィルタバッファ個数			*/


/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
/*	void	PosMngInitPositionManager(BASE_CTRL_HNDL *BaseControls );									*/
/*	LONG	PosMngNetPosCmdManager( BASE_LOOPCTRLS *BaseLoops, SERVO_CONTROL_IF *SvControlIf );		*/
/*	void	PosMngSenOnReCalcAbsoPos( BASE_CTRL_HNDL *BaseControls, MENCV *MencV,					*/
/*																	UNI_PRM_STR *uni_prm_ptr );		*/
/*	void	PosMngLatchNetFbkPos( BASE_CTRL_HNDL *BaseControls,										*/
/*												 SERVO_CONTROL_IF *SvControlIf, BPRMDAT *Bprm  );	*/
/*	void	StlgInitSettlingTime( SETTLINGTIME *SettlingTime, LONG CycleTimeUs );					*/
/*	void	StlgCalculateSettlingTime( SETTLINGTIME *SettlingTime, LONG dPcmda, BOOL CoinSts );		*/
/*	void	StlgMakeMaxOverShoot( SETTLINGTIME *SettlingTime, POS_MNG_HNDL *PosManager,				*/
/*																			BOOL PosCtrlMode );		*/
/*	void	StlgMakeCoinOffTime( SETTLINGTIME *SettlingTime, LONG dPcmda,							*/
/*																BOOL CoinSts, BOOL PosCtrlMode );	*/
/****************************************************************************************************/
/*	void	BpiEgearFuncSwitch( POS_MNG_HNDL *PosManager, BOOL Switch );							*/
/*	void	BpiPcmdFilSwitch( POS_MNG_HNDL *PosManager, BOOL Switch );								*/
/****************************************************************************************************/
/*	void	BpiRsetMaxOverShoot( SETTLINGTIME *SettlingTime );										*/
/*	void	BpiRsetCoinOffTime( SETTLINGTIME *SettlingTime );										*/
/****************************************************************************************************/
//#if ( CSW_SVCF_RESVIB_FIL_USE == TRUE ) /* <S065> */
//static	void	posMngInitPcmdFilter( VIBSUPFIL *VibSupFil );
//static	LONG	posMngVibSupFilter( VIBSUPFIL *VibSupFil, LONG dPcmd, BOOL *RefZSignal );
//#endif
static	void	posMngCtrlPosSignal( POS_MNG_HNDL *PosManager, POS_CTRL_STS *PosCtrlSts
									, PCMDFIL *pPcmdFil /* <S081> */);

//現状未対応	static	void	BpxMakeMotABCphaseSignal( BASE_CTRL_HNDL *BaseControls );
/****************************************************************************************************/
static void	BpxPerClrSignalProcedure( BASE_LOOPCTRLS *BaseLoops, BPRMDAT *Bprm );/* @@ 位置偏差クリア信号処理 */
static void	BpxEdgeClrSvcPositionManager( BASE_LOOPCTRLS *BaseLoops ); 	/* @@ サーボ制御位置偏差エッジクリア変数初期化	*/
/****************************************************************************************************/




/****************************************************************************************************/
/*																									*/
/*		サーボ制御位置管理変数初期化																*/
/*																									*/
/****************************************************************************************************/
void	PosMngInitPositionManager( BASE_CTRL_HNDL *BaseControls )
{
/*--------------------------------------------------------------------------------------------------*/
/*		サーボ制御位置管理変数初期化																*/
/*--------------------------------------------------------------------------------------------------*/
	/* 位置管理用変数初期化 */
	MlibResetLongMemory( &(BaseControls->PosManager.var), sizeof(BaseControls->PosManager.var)/4 );
/*--------------------------------------------------------------------------------------------------*/
//	MlibResetLongMemory( &(BaseControls->PosManager.PosErrA), sizeof(PERRB)/4 );	/* @@ CLR信号 */ /* <S107> */
/*--------------------------------------------------------------------------------------------------*/
	BaseControls->PosManager.Egear = &(BaseControls->PosManager.conf.Egear);

/* 2012.09.07 Y.Oka Amon現状未対応 */
//	BoutV.AmondPosCmd  = 0;								/* 位置指令モニタ変数						*/
/* 2012.09.07 Y.Oka ★ScanA用変数はした二つで初期化されるため、ここでは初期化しない★ */
	BaseControls->PosManager.dPosRefi = 0;				/* 位置指令差分入力[Pulse/Scan]のクリア		*//* <S187> */
	BaseControls->PosManager.CompdPosRefi = 0;			/* 位置指令差分入力(ScanB->ScanA)			*/
	BaseControls->PosManager.CompdPcmda = 0;			/* 位置指令増分値入力(ScanB->ScanA)			*/

	/* ★SGDVに合わせた。不要であれば削除する★ */
	if( BaseControls->CtrlModeSet.CtrlMode.b.cm == CTRLMODE_POS )
	{
		BaseControls->PosCtrlSts.CoinSignal = TRUE;
		BaseControls->PosCtrlSts.NearSignal = TRUE;
	}
	else
	{
		BaseControls->PosCtrlSts.CoinSignal = FALSE;
		BaseControls->PosCtrlSts.NearSignal = FALSE;
	}

	/* 位置指令フィルタ初期化処理 */
//	posMngInitPcmdFilter( &(BaseControls->VibSupFil) );												  /* <S065> */
	PcmdFilInitialize( &(BaseControls->PcmdFil), &(BaseControls->VibSupFil) );
	/* MFC変数初期化 */
	MfcInitModelControl( &(BaseControls->MFControl) );

#if (CSW_SVCF_BACKLASHCOMP == TRUE)
		/* バックラッシ補正無効 */
		BlshInvaildCompensate( &(BaseControls->PosManager.BlshCmp) );	/* <S18B> */
#endif

	BaseControls->CtrlCmdMngr.dPFfFilo = 0;
	BaseControls->CtrlCmdMngr.PicvClrCmd = FALSE;
}

/****************************************************************************************************/
/*																									*/
/*		サーボ通信位置指令管理																		*/
/*																									*/
/*			※AP型もこっちを使う																	*/
/*																									*/
/****************************************************************************************************/
LONG	PosMngNetPosCmdManager( BASE_LOOPCTRLS *BaseLoops, SERVO_CONTROL_IF *SvControlIf,
															FUN_CMN_CONTROL *FnCmnCtrl, BPRMDAT *Bprm )/* <S04D> */
{
	LONG			dPosRefo;
	BASE_CTRL_HNDL	*BaseControls;
	POS_MNG_HNDL	*PosManager;
	MFCTRL			*MFControl;
	BOOL 			den;
#if ( CSW_SVCF_BACKLASHCOMP )
	BOOL			BlshActive;
#endif

	BaseControls = BaseLoops->BaseCtrls;	
	PosManager	= &(BaseControls->PosManager);
	MFControl	= &(BaseControls->MFControl);

/*--------------------------------------------------------------------------------------------------*/
/*		位置指令入力処理(通信,PJOG,etc)：PosManager->var.dPcmda	[指令単位/Scan]						*/
/*--------------------------------------------------------------------------------------------------*/
	if( BaseControls->TblDrive.var.TblCmd != TBLCMD_NOCMD )
	{ /* テーブル運転時 */
		BaseControls->CtrlCmdMngr.PicvClrCmd = FALSE;			/* 位置制御積分クリア指令リセット		*/
	/*----------------------------------------------------------------------------------------------*/
		if( BaseControls->BeComplete == TRUE )
		{
			/* テーブル運転位置指令 */
			dPosRefo = BaseControls->TblDrive.var.dPosRef;
			/* 位置偏差クリア要求解除 */
			BaseControls->CtrlCmdMngr.PerrClrReq = FALSE;
/* <S212> */
			/* 位置指令リングカウンタ更新 */
			BaseControls->CtrlCmdMngr.PosRef += dPosRefo;
			/* 分割位置指令の更新 */
			PosManager->var.dPcmda = dPosRefo;
//			BaseControls->AmondPosCmd = dPosRefo;		/* <S226> */
/* <S212> */
		}
		else
		{
			/* サーボ制御位置管理変数初期化 */
			PosMngInitPositionManager( BaseControls );
			/* 位置偏差クリア要求 */
			BaseControls->CtrlCmdMngr.PerrClrReq = TRUE;
			/* 位置制御終了 */
			return( 0 );
		}
	}
	else if( BaseControls->CtrlModeSet.CtrlMode.dw == CTRLMODE_PJOG )
	{ /* プログラムJOGモード時 */
		BaseControls->CtrlCmdMngr.PicvClrCmd = FALSE;			/* 位置制御積分クリア指令リセット		*/
		BaseControls->PosCtrlSts.DenSignal   = FALSE;			/* Den Signal							*/

		if( BaseControls->BeComplete == TRUE )
		{ /* 位置制御実行 */
			/* 位置指令作成処理	[指令単位/Scan]	*/
			den = PrgJogMakePositionReference( &BaseControls->PJogHdl, &dPosRefo );

			/* Den Signal */
			BaseControls->PosCtrlSts.DenSignal  = den;
			BaseControls->PosCtrlSts.RefZSignal = den; /* <S066> */

			/* 位置偏差クリア要求 */
			BaseControls->CtrlCmdMngr.PerrClrReq = FALSE;

			/* 位置指令リングカウンタ更新 */
			BaseControls->CtrlCmdMngr.PosRef += dPosRefo;	/* <S1BA> */
			/* 分割位置指令の更新 */
			PosManager->var.dPcmda = dPosRefo;				/* <S1BA> */
//			BaseControls->AmondPosCmd = dPosRefo;			/* <S1BA> */	/* <S226> */
		}
		else
		{ /* 位置制御終了 */
			/* サーボ制御位置管理変数初期化 */
			PosMngInitPositionManager( BaseControls );
			/* 位置偏差クリア要求 */
			BaseControls->CtrlCmdMngr.PerrClrReq = TRUE;
			return( 0 );							
		}
	}
	else
	{ /* 通常位置制御時 */
	/*----------------------------------------------------------------------------------------------*/
	/*	@@ 偏差ｸﾘｱ処理																				*/
	/*----------------------------------------------------------------------------------------------*/
/* <S1B6> */
//		if( ((SvControlIf->CmdCtrl.CmdCtrlBit) >> CLRPOSERR_BITNO) & 1 )
//		{
//			PosMngInitPositionManager( BaseControls ); 		/* サーボ制御位置管理変数初期化			*/
//			BaseControls->CtrlCmdMngr.PerClrEdge = FALSE; 	/* 位置偏差クリアエッジ信号リセット		*/
//			BaseControls->CtrlCmdMngr.PerrClrReq = TRUE; 	/* 位置偏差クリア要求 					*/
//			return( 0 );									/* 位置制御終了							*/
//		}
	/*----------------------------------------------------------------------------------------------*/
#if (CSW_CMDRENEW_PCMD_FIL_USE == TRUE) /* <S065> */
		/* 位置指令移動平均処理(ScanB周期とScanN周期が異なる場合の補間処理) */
		dPosRefo = MlibPcmdMafil( (SvControlIf->NetPosRef - BaseControls->CtrlCmdMngr.PosRefLo),
								  SvControlIf->CmdOptionIf.CycCtrl.CmdUpdateCycle,
								  &(BaseControls->CtrlCmdMngr.Pmafvar),
								  &(BaseControls->CtrlCmdMngr.Pmafbuf[0]) );
#else
		dPosRefo = SvControlIf->NetPosRef - BaseControls->CtrlCmdMngr.PosRefLo;
#endif

		/* 位置指令リングカウンタ更新 */
		BaseControls->CtrlCmdMngr.PosRef += dPosRefo;	/* <S1B6> */
		/* 分割位置指令の更新 */
		PosManager->var.dPcmda = dPosRefo;				/* <S1B6> */
//		BaseControls->AmondPosCmd = dPosRefo;			/* <S1B6> */	/* <S226> */
		
/* <S1B6> */
		if( ((SvControlIf->CmdCtrl.CmdCtrlBit) >> CLRPOSERR_BITNO) & 1 )
		{
			PosMngInitPositionManager( BaseControls ); 		/* サーボ制御位置管理変数初期化			*/
			BaseControls->CtrlCmdMngr.PerClrEdge = FALSE; 	/* 位置偏差クリアエッジ信号リセット		*/
			BaseControls->CtrlCmdMngr.PerrClrReq = TRUE; 	/* 位置偏差クリア要求 					*/
			return( 0 );									/* 位置制御終了							*/
		}

		/* 位置偏差クリア要求解除 */
		BaseControls->CtrlCmdMngr.PerrClrReq = FALSE;

	/*----------------------------------------------------------------------------------------------*/
	/*	@@ 位置偏差エッジクリア処理																	*/
	/*----------------------------------------------------------------------------------------------*/
		if( BaseControls->PerClrEdge )
		{
			BpxPerClrSignalProcedure( BaseLoops, Bprm ); /* <S04D>:引数追加 */
			BaseControls->CtrlCmdMngr.PerClrEdge = TRUE; /* 位置偏差クリアエッジ信号セット			*/
			dPosRefo = 0;
		}
		else
		{
			BaseControls->CtrlCmdMngr.PerClrEdge = FALSE; /* 位置偏差クリアエッジ信号リセット		*/
		}
	}

/*--------------------------------------------------------------------------------------------------*/
/*		@@ 偏差クリア時は、以下処理は無し															*/
/*--------------------------------------------------------------------------------------------------*/

	/* 位置指令リングカウンタ更新 */
//	BaseControls->CtrlCmdMngr.PosRef += dPosRefo;	/* <S1B6> */
	/* 分割位置指令の更新 */
//	PosManager->var.dPcmda = dPosRefo;				/* <S1B6> */

/*--------------------------------------------------------------------------------------------------*/
/*		位置指令電子ギア変換：PosManager->var.dPcmdEgear [Pulse/Scan]								*/
/*--------------------------------------------------------------------------------------------------*/
	PosManager->var.dPcmdEgear = MlibPcmdEgear( dPosRefo,
												PosManager->Egear,
												&(PosManager->var.dPcmdEgrem) );

/*--------------------------------------------------------------------------------------------------*/
/*		位置指令フィルタ演算(指数加減速,移動平均)：BaseControls->CtrlCmdMngr.dPcmdFilo [Pulse/Scan]	*//* <S065> */
/*--------------------------------------------------------------------------------------------------*/
	/* 払出し完了信号処理(払出し未完モジュールがFALSEにする) */
//	BaseControls->PosCtrlSts.RefZSignal = ( PosManager->var.dPcmdEgear != 0 ) ? FALSE : TRUE;		  /* <S065> */
	PcmdFilSetRefZStatus( &(BaseControls->PcmdFil), TRUE );
	/* 位置指令フィルタ処理 */
	BaseControls->CtrlCmdMngr.dPcmdFilo = PcmdFilRuntimeService( &(BaseControls->PcmdFil),
																 PosManager->var.dPcmdEgear,
																 PosManager->var.PosErrA.per32s,
																 PosManager->var.PcmdFilStop );
	/* 払出し完了状態取得 */
	BaseControls->PosCtrlSts.DenSignal = PcmdFilGetRefZStatus( &(BaseControls->PcmdFil) );
	BaseControls->PosCtrlSts.RefZSignal = BaseControls->PosCtrlSts.DenSignal;

/*--------------------------------------------------------------------------------------------------*/
/*		拡張(FF)制御演算(MFC,ゆれどめ制御)															*/
/*--------------------------------------------------------------------------------------------------*/
/*		BaseControls->CtrlCmdMngr.dPosRefo [Pulse/Scan]：位置制御位置指令差分入力					*/
/*--------------------------------------------------------------------------------------------------*/

	/*----------------------------------------------------------------------------------------------*/
	/*		バックラッシ補正処理																	*/
	/*----------------------------------------------------------------------------------------------*/
#if ( CSW_SVCF_BACKLASHCOMP )
	if( (BaseControls->BeComplete == FALSE)
		|| (BaseControls->MencV->PhaseReady == FALSE)		/* <S18B> */
		|| (BaseControls->OverTravel == TRUE) )
	{
		BlshActive = FALSE;
	}
	else
	{
		BlshActive = TRUE;
	}
	BlshCompensateBacklash( &(PosManager->BlshCmp),
							&(PosManager->conf.Egear),
//							&(PosManager->var.dPcmdEgear),											  /* <S065> */
							&(BaseControls->CtrlCmdMngr.dPcmdFilo),									  /* <S065> */
							&(PosManager->var.dPcmda),/* &(dPosRefo),*/			/* <S18B> */
							&(BaseControls->PosCtrlSts.RefZSignal),
							BlshActive );
#endif

	/*----------------------------------------------------------------------------------------------*/
	/*		振動抑制フィルタ処理																	*/
	/*----------------------------------------------------------------------------------------------*/
#if ( CSW_SVCF_RESVIB_FIL_USE == TRUE )
//	if( PosManager->var.PcmdFilStop == FALSE ) /* <S065> */
	{ /* 位置指令フィルタ無効要求なし */
		/* 振動抑制フィルタ強制無効化処理(位置制御以外は無効) */
		if( BaseControls->CtrlModeSet.CtrlModeLst.b.cm != CTRLMODE_POS )
		{
			BaseControls->VibSupFil.conf.Pexe.enable = 0;
		}
		/* 振動抑制フィルタ演算 */
//		BaseControls->CtrlCmdMngr.dPcmdFilo = posMngVibSupFilter(									  /* <S065> */
		BaseControls->CtrlCmdMngr.dPcmdFilo = PcmdFilVibSupFilter(
												&(BaseControls->VibSupFil),
//												PosManager->var.dPcmdEgear,							  /* <S065> */
												BaseControls->CtrlCmdMngr.dPcmdFilo,				  /* <S065> */
												&(BaseControls->PosCtrlSts.RefZSignal) );
	}
//	else /* <S065> */
//	{ /* 位置指令フィルタ無効要求あり */
//		BaseControls->CtrlCmdMngr.dPcmdFilo = PosManager->var.dPcmdEgear;
//	}
#else
//		BaseControls->CtrlCmdMngr.dPcmdFilo = PosManager->var.dPcmdEgear; /* <S065> */
#endif

	/*----------------------------------------------------------------------------------------------*/
	/*		MFC処理																					*/
	/*----------------------------------------------------------------------------------------------*/
#if ( CSW_SVCF_MFC_USE == TRUE )
	/* MFC強制無効化処理(位置制御以外は無効) */
	if( BaseControls->CtrlModeSet.CtrlModeLst.b.cm != CTRLMODE_POS )
	{ /* 前回の制御モードが位置制御でない場合 */
		/* MFC無効に初期化 */
		MFControl->conf.MfcPexe.MFCModel = 0;
	}
	/* MFC演算 */
	dPosRefo = MfcControlMain( &(BaseControls->MFControl),
							   BaseControls->CtrlCmdMngr.dPcmdFilo,
							   &(BaseControls->CtrlCmdMngr.SpdFFCx),
							   &(BaseControls->CtrlCmdMngr.TrqFFCx) );
#else
	MFControl->conf.MfcPexe.MFCModel = 0;
	dPosRefo = BaseControls->CtrlCmdMngr.dPcmdFilo;
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		制御位置信号処理																			*/
/*--------------------------------------------------------------------------------------------------*/
	/* Make Coin and Near Status */
	posMngCtrlPosSignal( PosManager, &(BaseControls->PosCtrlSts)
					,&(BaseControls->PcmdFil) /* <S081> */ );

	return (dPosRefo);
}

/****************************************************************************************************/
/*																									*/
/*		SEN ON時の絶対位置再計算																	*/
/*																									*/
/****************************************************************************************************/
void	PosMngSenOnReCalcAbsoScalePos(MENCV *MencV, APOSRG *EncPos, EGEAR *Egear, BOOL RvsDir)
{
	LONG	IniPos[2];
	UCHAR	Sftx;

	/* ローカル変数の初期化 */
	Sftx	= FSX_FPOS_SFT;

	if( MencV->AbsoEncType == MENC_INCRE_TYPE )
	{
		MlibResetLongMemory(EncPos, sizeof(APOSRG)/4);
	}
	else
	{
		IniPos[1] = (LONG)((SHORT)MencV->RxPosH[0] >> Sftx);		/* <S209> *//* <S211> */
		IniPos[0] = (LONG)( ((ULONG)MencV->RxPosH[0] << (32-Sftx))
						+ ((ULONG)MencV->RxPosL[0].dw >> Sftx) );	/* <S209> */

		if( MencV->RevAsmMotor ^ RvsDir )/* 反転接続 XOR 逆回転モード */
		{
			IniPos[1] = (~IniPos[1]);		/* 符号反転処理	*/
			IniPos[0] = (~IniPos[0]) + 1;	/* 符号反転処理 */
			if( IniPos[0] == 0 )
			{	/* Carryの場合は、上位に+1 */
				++IniPos[1];
			}
		}
		/*	処理時間大の関数だが使用する。
		 * (サーボOFF中の処理のため、処理時間多少余裕あり)	*/
		MlibAposRg64iv( IniPos[0], IniPos[1], Egear, EncPos );
	}
}

/****************************************************************************************************/
/*																									*/
/*		SEN ON時の絶対位置再計算																	*/
/*																									*/
/****************************************************************************************************/
void	PosMngSenOnReCalcAbsoPos( MENCV *MencV, APOSRG *EncPos,
								  EGEAR *Egear, DBYTEX bit_dp, USHORT limmlt, BOOL RvsDir )
{
	SencMakeAbsPosition(MencV, limmlt, bit_dp, RvsDir);
	MlibAposRg64iv( MencV->SenOnScalePosL, MencV->SenOnScalePosH, Egear, EncPos );
}



/****************************************************************************************************/
/*																									*/
/*		位置ＦＢラッチ処理																			*/
/*																									*/
/****************************************************************************************************/
void	PosMngLatchNetFbkPos( BASE_CTRL_HNDL *BaseControls, SERVO_CONTROL_IF *SvControlIf, BPRMDAT *Bprm )/* <S04D> */
{
	POS_MNG_HNDL	*PosManager;
	LONG			RspLtBit = 0;
	LONG			dPosCmp;
	LONG			motLpos, motPos;																/* <S004> */
	LONG			posSftR;																		/* <S04D> */
	LONG			rvsDir;																			/* <S221> */

	PosManager = &(BaseControls->PosManager);
	motPos = BaseControls->MencV->MotPosX[0];														/* <S007> */
	rvsDir = 1;																						/* <S221> */

	if(Bprm->FencUse != FALSE)/* <S04D>:ﾌﾙｸ処理追加 */
	{/* ﾌﾙｸ	*/
		posSftR = BaseControls->FencV->MposSftR;
	}
	else
	{/* ｾﾐｸ	*/
		posSftR = BaseControls->MencV->MposSftR;
		if(Bprm->RvsDir)/* <S221> */
		{
			rvsDir = -1;
		}
	}

/*--------------------------------------------------------------------------------------------------*/
/*		Ｃ相ラッチ処理																				*/
/*--------------------------------------------------------------------------------------------------*/
	if( SvControlIf->LtReqCphs != FALSE )
	{ /* ラッチ要求あり */
//		if(BaseControls->MotSts.MotCphPass != FALSE)/* <S04D> */
		if((Bprm->FencUse != FALSE)&&(BaseControls->MotSts.FencCphPass != FALSE)
			||(Bprm->FencUse == FALSE)&&(BaseControls->MotSts.MotCphPass != FALSE))/* <S04D>:ﾌﾙｸ対応 */
		{/* ラッチ完了 */
			dPosCmp = BaseControls->CtrlCmdMngr.FbkPosFromCpos;

			/* 絶対位置更新計算 */
			MlibAposRg64dp( -dPosCmp,
							&PosManager->conf.Egear,
							&PosManager->PgPos,
							SvControlIf->MonCphaLpos );

			SvControlIf->LtReqCphs = FALSE;
			RspLtBit |= ((SvControlIf->LtReqCphs^1) * C_PHASE_COMP_BIT);
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		Ext1ラッチ処理																				*/
/*--------------------------------------------------------------------------------------------------*/
	if( SvControlIf->LtReqExt1 != FALSE )
	{ /* ラッチ要求あり */
		if((SvControlIf->ExtLtSts[0]).Complete != FALSE)
		{/* ラッチ完了 */
			motLpos = (LONG)SvControlIf->ExtLtSts[0].LatchPosition << posSftR;	/* <S004> *//* <S04D> */
			dPosCmp = rvsDir * ((motPos - motLpos) >> posSftR);					/* <S004> *//* <S04D> *//* <S221> */
			/* 絶対位置更新計算 */
			MlibAposRg64dp( -dPosCmp,
							&PosManager->conf.Egear,
							&PosManager->PgPos,
							SvControlIf->MonExt1Lpos);

			SvControlIf->LtReqExt1 = FALSE;
			RspLtBit |= ((SvControlIf->LtReqExt1^1) * EXT_SIG1_COMP_BIT);
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		Ext2ラッチ処理																				*/
/*--------------------------------------------------------------------------------------------------*/
	if( SvControlIf->LtReqExt2 != FALSE )
	{ /* ラッチ要求あり */
		if((SvControlIf->ExtLtSts[1]).Complete != FALSE)
		{/* ラッチ完了 */
			motLpos = (LONG)SvControlIf->ExtLtSts[1].LatchPosition << posSftR;	/* <S004> *//* <S04D> */
			dPosCmp = rvsDir * ((motPos - motLpos) >> posSftR);					/* <S004> *//* <S04D> *//* <S221> */
			/* 絶対位置更新計算 */
			MlibAposRg64dp( -dPosCmp,
							&PosManager->conf.Egear,
							&PosManager->PgPos,
							SvControlIf->MonExt2Lpos);

			SvControlIf->LtReqExt2 = FALSE;
			RspLtBit |= ((SvControlIf->LtReqExt2^1) * EXT_SIG2_COMP_BIT);
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		Ext3ラッチ処理																				*/
/*--------------------------------------------------------------------------------------------------*/
	if( SvControlIf->LtReqExt3 != FALSE )
	{ /* ラッチ要求あり */
		if(SvControlIf->ExtLtSts[2].Complete != FALSE)
		{/* ラッチ完了 */
			motLpos = (LONG)SvControlIf->ExtLtSts[2].LatchPosition << posSftR;	/* <S004> *//* <S04D> */
			dPosCmp = rvsDir * ((motPos - motLpos) >> posSftR);					/* <S004> *//* <S04D> *//* <S221> */
			/* 絶対位置更新計算 */
			MlibAposRg64dp( -dPosCmp,
							&PosManager->conf.Egear,
							&PosManager->PgPos,
							SvControlIf->MonExt3Lpos);

			SvControlIf->LtReqExt3 = FALSE;
			RspLtBit |= ((SvControlIf->LtReqExt3^1) * EXT_SIG3_COMP_BIT);
		}
	}

	/* ラッチ完了状態設定 */
//	RspLtBit = ((SvControlIf->LtReqCphs^1) * C_PHASE_COMP_BIT)
//			 | ((SvControlIf->LtReqExt1^1) * EXT_SIG1_COMP_BIT)
//			 | ((SvControlIf->LtReqExt2^1) * EXT_SIG2_COMP_BIT)
//			 | ((SvControlIf->LtReqExt3^1) * EXT_SIG3_COMP_BIT);

	/* ラッチ要求レベル設定 */
	SvControlIf->RspLtBit = RspLtBit | (SvControlIf->CmdLtBit & 0xF);
}


/****************************************************************************************************/
/*																									*/
/*		制御位置信号処理																			*/
/*																									*/
/****************************************************************************************************/
static	void	posMngCtrlPosSignal( POS_MNG_HNDL *PosManager, POS_CTRL_STS *PosCtrlSts,
									PCMDFIL	*pPcmdFil /* <S081> */ )
{
	LONG	abs_poserr;

	abs_poserr = PosManager->var.PosErrA.per32a;

/*--------------------------------------------------------------------------------------------------*/
/*		位置決め完了信号作成																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( abs_poserr <= PosManager->conf.CoinLevel )
	{
		switch( PosManager->conf.CoinOutSel )
		{
		case POSERR:		/* 位置偏差で/COIN出力 */
			PosCtrlSts->CoinSignal = TRUE;
			break;

		case POSERR_REFOUT:	/* 位置偏差と位置指令フィルタ出力で/COIN出力 */
//<S081>			PosCtrlSts->CoinSignal = PosCtrlSts->RefZSignal;
			PosCtrlSts->CoinSignal = PcmdFilGetRefZStatus( pPcmdFil );	/* <S081> */
			break;

		case POSERR_REFIN:	/* 位置偏差と位置指令入力で/COIN出力 */
			PosCtrlSts->CoinSignal = (PosManager->var.dPcmda == 0) ? TRUE : FALSE;
			break;

		default:
			break;
		}
	}
	else
	{
		PosCtrlSts->CoinSignal = FALSE;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		位置決め近傍信号作成																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( abs_poserr <= PosManager->conf.NearLevel )
	{
		PosCtrlSts->NearSignal = TRUE;
	}
	else
	{
		PosCtrlSts->NearSignal = FALSE;
	}
}


#if 0 /* <S065> */
#if ( CSW_SVCF_RESVIB_FIL_USE == TRUE )
/****************************************************************************************************/
/*																									*/
/*		位置指令フィルタ初期化																		*/
/*																									*/
/****************************************************************************************************/
static	void	posMngInitPcmdFilter( VIBSUPFIL *VibSupFil )
{
	MlibResetLongMemory( &VibSupFil->var, sizeof(VibSupFil->var)/4 );
	VibSupFil->conf.Pexe = VibSupFil->conf.VibSupPrm;
}


/****************************************************************************************************/
/*																									*/
/*		振動抑制フィルタ演算																		*/
/*																									*/
/*	概要:	振動抑制フィルタは，設定された周波数に対するノッチ特性を持ったフィルタである。			*/
/*			機台振動など位置決め時の振動を抑制する。												*/
/*			移動平均等位置指令フィルタ，MFCとも直列に接続可能。										*/
/*																									*/
/****************************************************************************************************/
static	LONG	posMngVibSupFilter( VIBSUPFIL *VibSupFil, LONG dPcmd, BOOL *RefZSignal )
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
BOOL	PosMngCalculatePrmVibSupFilSW( VIBSUPFIL *VibSupFil, ULONG mdlsw, ULONG ff_feq, ULONG ff_fil, LONG ScanTimeNs )
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
	PosMngCalculatePrmVibSupFil( VibSupFil, ff_feq, ff_fil, ScanTimeNs );

	return	PrmSetErr;
}




/****************************************************************************************************/
/*																									*/
/*		振動抑制フィルタパラメータ計算																*/
/*																									*/
/****************************************************************************************************/
void	PosMngCalculatePrmVibSupFil( VIBSUPFIL *VibSupFil, ULONG ff_frq, ULONG ff_fil, LONG ScanTimeNs )
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
#endif /* #if 0 */ /* <S065> */


/****************************************************************************************************/
/*																									*/
/*		電子ギア機能有効/無効切り替え処理															*/
/*																									*/
/****************************************************************************************************/
void	BpiEgearFuncSwitch( POS_MNG_HNDL *PosManager, BOOL Switch )
{
	if( Switch == TRUE )
	{ /* 電子ギア機能有効化要求時 */
		PosManager->var.EgearDisable = FALSE;
		PosManager->Egear = &(PosManager->conf.Egear);
	}
	else
	{ /* 電子ギア機能無効化要求時 */
		PosManager->var.EgearDisable = TRUE;
		PosManager->Egear = &(PosManager->conf.InvalidEgear);
	}
}


/****************************************************************************************************/
/*																									*/
/*		位置指令フィルタ有効/無効切り替え処理														*/
/*																									*/
/****************************************************************************************************/
void	BpiPcmdFilSwitch( POS_MNG_HNDL *PosManager, BOOL Switch )
{
	if( Switch == TRUE )
	{ /* 位置指令フィルタ有効化要求時 */
		PosManager->var.PcmdFilStop = FALSE;
	}
	else
	{ /* 位置指令フィルタ無効化要求時 */
		PosManager->var.PcmdFilStop = TRUE;
	}
}


/****************************************************************************************************/
/*																									*/
/*		整定時間関連処理用変数初期化処理															*/
/*																									*/
/****************************************************************************************************/
void	StlgInitSettlingTime( SETTLINGTIME *SettlingTime, LONG CycleTimeUs )
{
	/* Reset Settling Time Variable */
	MlibResetLongMemory( SettlingTime, sizeof(*SettlingTime)/4 );

	/* Set Unit Change Gain [cyc] ⇒ [1us] */
	//SettlingTime->UnitChngGain = MlibPcalKxgain( CycleTimeUs, 1, 100, NULL, 24 );//kira修正_forAAT_110418
	SettlingTime->UnitChngGain = CycleTimeUs;

#if 0		/* <S0DA> */
	/* Start Settling Time Process */
	KlibRstLongTimer( &(SettlingTime->ProcessTimer) );
#endif

}


/****************************************************************************************************/
/*																									*/
/*		整定時間算出用関数																			*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	概要：位置指令払い出し完了から次の位置指令払い出し開始までの間または，カウンタがMAX値になるまで	*/
/*		　最後に/COIN信号がOFFになるまでの時間(カウンタ値)を測定する。								*/
/*																									*/
/****************************************************************************************************/
void	StlgCalculateSettlingTime( SETTLINGTIME *SettlingTime, LONG dPcmda, BOOL CoinSts )
{
#if 0		/* <S0DA> */
	/* Check Timing of Measuring Settling Time */
	if( 1 > KlibGetLongTimerMs(SettlingTime->ProcessTimer) )			/* 1[ms]毎に処理				*/
	{
		return;
	}
	/* Reset Timing of Measuring Settling Time */
	KlibRstLongTimer( &(SettlingTime->ProcessTimer) );
#endif

	/*----------------------------------------------------------------------------------------------*/
	/*	整定時間算出演算中フラグ作成																*/
	/*----------------------------------------------------------------------------------------------*/
	if( dPcmda != 0 )
	{ /* 動作中 */
		if( SettlingTime->StlgLastdPcmd == 0 )
		{ /* 位置指令が入力される最初のスキャンの場合 */
			/* 保持している整定時間を出力用変数に代入 */
			SettlingTime->RsltStlgTime = SettlingTime->TmpStlgTime;
		}

		/*------------------------------------------------------------------------------------------*/
		/*	動作中は変数をクリアする																*/
		/*------------------------------------------------------------------------------------------*/
		SettlingTime->StlgCalcReq = FALSE;				/* 整定時間演算要求フラグをOFF				*/
		SettlingTime->StlgLastCoinSts = FALSE;			/* 前回COIN信号をOFF						*/
		SettlingTime->StlgTimeCntr = 0;					/* カウンタをクリア							*/
	}
	else if( SettlingTime->StlgLastdPcmd != 0 )
	{ /* 位置指令払い出し完了時(最初だけ) */
		SettlingTime->StlgCalcReq = TRUE;				/* 整定時間演算要求フラグをON				*/
	}

	/*----------------------------------------------------------------------------------------------*/
	/*	整定時間算出演算																			*/
	/*----------------------------------------------------------------------------------------------*/
	if( SettlingTime->StlgCalcReq == TRUE )
	{
		/*------------------------------------------------------------------------------------------*/
		/*	SettlingTime->StlgTimeCntr = 65535の時に整定時間を出力用変数に代入	(約8[s])			*/
		/*------------------------------------------------------------------------------------------*/
		if( SettlingTime->StlgTimeCntr == 65535 )
		{
			SettlingTime->RsltStlgTime = SettlingTime->TmpStlgTime;
		}

		/*------------------------------------------------------------------------------------------*/
		/*	位置決め完了信号がONになった時のみ整定時間をバッファ									*/
		/*------------------------------------------------------------------------------------------*/
		if( ( CoinSts == TRUE ) && ( SettlingTime->StlgLastCoinSts == FALSE ) )
		{
			SettlingTime->TmpStlgTime = SettlingTime->StlgTimeCntr;
		}

		/*------------------------------------------------------------------------------------------*/
		/*	カウンタをインクリメント(ただし65535でリミット)											*/
		/*------------------------------------------------------------------------------------------*/
		if( SettlingTime->StlgTimeCntr != 65535 )
		{
			SettlingTime->StlgTimeCntr++;
		}
	}

	/*----------------------------------------------------------------------------------------------*/
	/*	前回値を保持																				*/
	/*----------------------------------------------------------------------------------------------*/
	SettlingTime->StlgLastdPcmd = dPcmda;						/* 差分位置指令を保持				*/
	SettlingTime->StlgLastCoinSts = CoinSts;					/* COIN信号を保持					*/

//	BoutV.OutSettlingTime = SettlingTime->RsltStlgTime;

}


/****************************************************************************************************/
/*																									*/
/*		最大オーバーシュート量作成 --> Un106														*/
/*																									*/
/****************************************************************************************************/
void	StlgMakeMaxOverShoot( SETTLINGTIME *SettlingTime, POS_MNG_HNDL *PosManager, BOOL PosCtrlMode )
{

	if( (PosCtrlMode == TRUE) && (PosManager->var.dPcmda == 0) )
	{
/*--------------------------------------------------------------------------------------------------*/
/*		位置決め方向判別																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( SettlingTime->LastdPcmda > 0 )		/* 正方向への位置決め　指令払い出し初回			*/
		{
			SettlingTime->UnMaxOverShoot = 0;		/* 最大オーバーシュート量クリア					*/
			SettlingTime->OverErrDir = -1;			/* オーバーシュートする偏差の符号(-)			*/
		}
		else if( SettlingTime->LastdPcmda < 0 )	/* 負方向への位置決め　指令払い出し初回			*/
		{
			SettlingTime->UnMaxOverShoot = 0;		/* 最大オーバーシュート量クリア					*/
			SettlingTime->OverErrDir = 1;			/* オーバーシュートする偏差の符号(+)			*/
		}

		SettlingTime->OverShoot = SettlingTime->OverErrDir * PosManager->var.PosErrA.per32s;

/*--------------------------------------------------------------------------------------------------*/
/*		オーバーシュート量判別																		*/
/*--------------------------------------------------------------------------------------------------*/
		if( SettlingTime->UnMaxOverShoot < SettlingTime->OverShoot )
		{
			SettlingTime->UnMaxOverShoot = SettlingTime->OverShoot;
		}
	}
	else
	{
		BpiRsetMaxOverShoot( SettlingTime );			/* オーバーシュート量クリア処理				*/
	}
	SettlingTime->LastdPcmda = PosManager->var.dPcmda;
}


/****************************************************************************************************/
/*																									*/
/*		位置決め完了失敗累積時間作成 --> Un10B														*/
/*																									*/
/****************************************************************************************************/
void	StlgMakeCoinOffTime( SETTLINGTIME *SettlingTime, LONG dPcmda, BOOL CoinSts, BOOL PosCtrlMode )
{

/*--------------------------------------------------------------------------------------------------*/
/*		位置決め完了確認																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( (PosCtrlMode == TRUE) && (dPcmda == 0) )
	{
		if( CoinSts == TRUE )
		{
			SettlingTime->StlgCoinLatch = TRUE;
		}
	}
	else
	{
		SettlingTime->StlgCoinLatch = FALSE;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		COIN割れ検出 --> 位置決め完了失敗累積時間作成												*/
/*--------------------------------------------------------------------------------------------------*/
	if( (SettlingTime->StlgCoinLatch == TRUE) && (CoinSts == FALSE) )
	{
		/* カウンタ上限リミット */
		if( SettlingTime->UnCoinOffTimer < 0x1000000 )
		{
			/* 位置決め完了失敗累積時間[cycle] */
			SettlingTime->UnCoinOffTimer++;
		}
	}

/*--------------------------------------------------------------------------------------------------*/
/*		位置決め完了失敗累積時間のクリア処理 														*/
/*			・Fn01C --> Un10B = 0																	*/
/*			・位置制御モード以外																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( PosCtrlMode == FALSE )
	{
		BpiRsetCoinOffTime( SettlingTime );
	}

}

/****************************************************************************************************/
/*																									*/
/*		許容オーバーシュート量パラメータ計算														*/
/*																									*/
/****************************************************************************************************/
void	StlgCalculateOverShootErrLevel( SETTLINGTIME *SettlingTime, LONG CoinLevel, LONG ovserrdetlvl )
{
	LONG	sx;
	sx = 0;
	SettlingTime->OvsErrDetLvl = MlibScalKxgain( CoinLevel,ovserrdetlvl, 100, &sx, -24 );
}


/****************************************************************************************************/
/*																									*/
/*		オーバーシュート量クリア処理																*/
/*																									*/
/****************************************************************************************************/
void	BpiRsetMaxOverShoot( SETTLINGTIME *SettlingTime )
{
	SettlingTime->UnMaxOverShoot = 0;
	SettlingTime->OverErrDir = 0;
}


/****************************************************************************************************/
/*																									*/
/*		位置決め完了失敗累積時間クリア処理															*/
/*																									*/
/****************************************************************************************************/
void	BpiRsetCoinOffTime( SETTLINGTIME *SettlingTime )
{
	SettlingTime->UnCoinOffTimer = 0;
	SettlingTime->StlgCoinLatch = FALSE;
	SettlingTime->StlgCoinLatch = FALSE;
}


/****************************************************************************************************/
/*																									*/
/*		位置決め完了失敗累積時間取得処理															*/
/*																									*/
/****************************************************************************************************/
LONG	BpiGetCoinOffTime( SETTLINGTIME *SettlingTime )
{
#if (FLOAT_USE==TRUE)
	return ( SettlingTime->UnCoinOffTimer * SettlingTime->UnitChngGain );
#else
	return ( MlibMulgain( SettlingTime->UnCoinOffTimer, SettlingTime->UnitChngGain) );
#endif /* FLOAT_USE */
}


/****************************************************************************************************/
/*																									*/
/*		最大オーバーシュート量[指令単位]取得処理													*/
/*																									*/
/****************************************************************************************************/
LONG	BpiGetMaxOverShoot( SETTLINGTIME *SettlingTime )
{
	return (SettlingTime->UnMaxOverShoot);
}


/****************************************************************************************************/
/*																									*/
/*		COINラッチ状態取得処理																		*/
/*																									*/
/****************************************************************************************************/
LONG	BpiGetCoinLatchSts( SETTLINGTIME *SettlingTime )
{
	return (SettlingTime->StlgCoinLatch);
}


/****************************************************************************************************/
/*																									*/
/*		整定時間演算結果取得処理																	*/
/*																									*/
/****************************************************************************************************/
LONG	BpiGetSettlingTime( SETTLINGTIME *SettlingTime )
{
	return (SettlingTime->RsltStlgTime);
}


/****************************************************************************************************/
/*																									*/
/*		サーボ制御ネット位置応答管理														<S14C>	*/
/*																									*/
/****************************************************************************************************/
void	PosMngResponseManager( BASE_CTRL_HNDL *BaseControls )
{

/*--------------------------------------------------------------------------------------------------*/
/*		エンコーダＡＢＣ相信号作成																	*/
/*--------------------------------------------------------------------------------------------------*/
	/* モータエンコーダＡＢＣ相信号作成 */
	/* 2009.06.04 Y.Oka (>_<) 現状未対応 */
//	BpxMakeMotABCphaseSignal( BaseControls );

/*--------------------------------------------------------------------------------------------------*/
/*		位置制御モード以外の時																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( BaseControls->CtrlModeSet.CtrlMode.b.cm != CTRLMODE_POS )
	{
		/* サーボ制御位置管理変数初期化 */
		PosMngInitPositionManager( BaseControls );
	}
/*--------------------------------------------------------------------------------------------------*/
/*		ゼロ速停止モード(スキャンＣで検出)の時														*/
/*--------------------------------------------------------------------------------------------------*/
	else if( BaseControls->CtrlModeSet.ZctrlMode.zm == CTRLMODE_ZSTOP )
	{
		;
	}

}


#if 0 /* 現状未対応 */
/****************************************************************************************************/
/*																									*/
/*		モータエンコーダＡＢＣ相信号作成															*/
/*																									*/
/****************************************************************************************************/
static	void	BpxMakeMotABCphaseSignal( BASE_CTRL_HNDL *BaseControls )
{
	MENCV			*MencV;
	POS_MNG_HNDL	*PosManager;
	BASE_CTRL_IO	*BaseCtrlIo;

	MencV		= BaseControls->MencV;
	PosManager		= BaseControls->sv_ctrl_ptr->PosMngV_ptr;
	BaseCtrlIo		= &( BaseControls->scanb_io );

/*--------------------------------------------------------------------------------------------------*/
/*		Ｃ相通過チェック																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( BaseControls->ScanBMotCposOk )
	{
		if( (BaseControls->ScanBMotCphPass == TRUE) )
		{
			/* Ｃ相出力時間 : 750[us] */
			PosManager->MCphOutTime = ( ((750) * US_CHANGE_MS) / KPI_SBCYCLENS );
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		Ｃ相出力信号作成																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( PosManager->MCphOutTime > 0 )
	{
		PosManager->MCphOutTime--;
		BaseCtrlIo->PgCphase = TRUE;
	}
	else
	{
		BaseCtrlIo->PgCphase = FALSE;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		Ａ相,Ｂ相信号作成																			*/
/*--------------------------------------------------------------------------------------------------*/
/*		MencV.ABphCntr	A相,B相																		*/
/*		  xxxxxx00  -->  1 , 1 																		*/
/*		  xxxxxx01	-->  1 , 0 																		*/
/*		  xxxxxx10	-->  0 , 0 																		*/
/*		  xxxxxx11	-->  0 , 1 																		*/
/*--------------------------------------------------------------------------------------------------*/
	switch( MencV->ABphCntr & 0x03 )
	{
	case 0x00: BaseCtrlIo->PgAphase = TRUE;  BaseCtrlIo->PgBphase = TRUE;  break;
	case 0x01: BaseCtrlIo->PgAphase = TRUE;  BaseCtrlIo->PgBphase = FALSE; break;
	case 0x02: BaseCtrlIo->PgAphase = FALSE; BaseCtrlIo->PgBphase = FALSE; break;
	default  : BaseCtrlIo->PgAphase = FALSE; BaseCtrlIo->PgBphase = TRUE;  break;
	}
	return;
}
#endif /* #if 0 */



/***************************************** ここからA/P **********************************************/



#if 0 //(CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)
/****************************************************************************************************/
/*																									*/
/*		サーボ制御位置指令管理																		*/
/*																									*/
/****************************************************************************************************/
LONG	BpxSvcPosCommandManager( BASE_CTRL_HNDL *BaseControls, FUN_CMN_CONTROL *FnCmnCtrl )
{
	POS_MNG_HNDL	*PosManager;
	MFCTRL			*MFControl;
	LONG			dPosRefo;
	LONG			rc;
#if 0 /* 2012.09.06 Y.Oka ★位置偏差クリア現状未対応★ */
	LONG			lwk_dPcmdHwCntr;
#endif

	PosManager	= &(BaseControls->PosManager);
	MFControl	= &(BaseControls->MFControl);

#if 0 /* 2012.09.06 Y.Oka ★位置偏差クリア信号入力未対応★ */
/*--------------------------------------------------------------------------------------------------*/
/*		位置偏差クリア信号監視																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( SVFSEL_LTM_PERCLR == 1 )
	{
		KpiChkPerClearSignal( );						/* カーネル位置偏差クリア信号監視			*/
	}
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		位置制御モード以外の時																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( BaseControls->CtrlModeSet.CtrlMode.b.cm != CTRLMODE_POS )
	{
		BaseControls->PosCtrlSts.DenSignal = FALSE;
		return( 0 );
	}

/*--------------------------------------------------------------------------------------------------*/
/*		払出し完了信号処理：払出し未完モジュールがＦＡＬＳＥにする。								*/
/*--------------------------------------------------------------------------------------------------*/
	BaseControls->PosCtrlSts.DenSignal = TRUE;

/*--------------------------------------------------------------------------------------------------*/
/*		位置指令入力処理(パルス列,PJOG,etc)：PosMngV.dPcmda	[指令単位/Scan]							*/
/*--------------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------*/
	/*		テーブル運転モード																		*/
	/*----------------------------------------------------------------------------------------------*/
	if( BaseControls->TblDrive.var.TblCmd != TBLCMD_NOCMD )
	{ /* テーブル運転時 */
#if 0 /* 2012.09.06 Y.Oka ★位置偏差クリア現状未対応★ */
		BoutV.f.PerClrEdge = FALSE;							/* 位置偏差クリアエッジ信号リセット		*/
		BoutK.f.PerClrEdge = FALSE;							/* 位置偏差クリアエッジ信号リセット		*/
#endif
	/*------------------------------------------------------------------------------------------*/
		if( BaseControls->BeComplete == TRUE )
		{
			/* テーブル運転位置指令 */
			dPosRefo = BaseControls->TblDrive.var.dPosRef;
			/* 位置偏差クリア指令 */
			BaseControls->CtrlCmdMngr.PerrClrReq = FALSE;
		}
		else
		{
#if ( SVFSEL_BLSH_COMMON_PROC )		/* <V868> 標準・Y522兼用	*/
			/* バックラッシ補正無効 */
			BlshInvaildCompensate( &(BaseControls->PosManager.BlshCmp) );
#endif
			/* サーボ制御位置管理変数初期化 */
			PosMngInitPositionManager( BaseControls );
			/* 位置偏差クリア要求 */
			BaseControls->CtrlCmdMngr.PerrClrReq = TRUE;
			/* ScanB位置偏差クリア */
			MlibResetLongMemory( &(BaseControls->PosManager.PosErrA), sizeof(BaseControls->PosManager.PosErrA)/4 ); 	/* <S107> */
			MlibResetLongMemory( &(BaseControls->PosManager.PosErrAmp), sizeof(BaseControls->PosManager.PosErrAmp)/4 );
			/* 位置制御終了 */
			return( 0 );
		}
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		プログラムJOG運転モード																	*/
	/*----------------------------------------------------------------------------------------------*/
	else if( FnCmnCtrl->FnCtrlMcmd == CTRLMODE_PJOG )
	{ /* プログラムJOGモード時 */
#if 0 /* 2012.09.06 Y.Oka ★位置偏差クリア現状未対応★ */
		BoutV.f.PerClrEdge = FALSE;						/* 位置偏差クリアエッジ信号リセット		*/
		BoutK.f.PerClrEdge = FALSE;						/* 位置偏差クリアエッジ信号リセット		*/
#endif
	/*------------------------------------------------------------------------------------------*/
		if( FnCmnCtrl->FnCtrlMcmd != CTRLMODE_PJOG )
		{												/* 制御モードが切り替わった時点			*/
			PrgJogReset( &(BaseControls->PJogHdl) );		/* プログラムJOG運転データリセット		*/
		}
	/*------------------------------------------------------------------------------------------*/
		if( BaseControls->BeComplete == TRUE )
		{ /* 位置制御実行 */

			/* 位置指令作成処理	[指令単位/Scan]	*/
			rc = PrgJogMakePositionReference( &BaseControls->PJogHdl, &dPosRefo );

			if( rc == TRUE )
			{/* プログラムJOG演算終了 */
				BaseControls->PosCtrlSts.DenSignal  = TRUE;
			}
			else
			{/* プログラムJOG演算中 */
				BaseControls->PosCtrlSts.DenSignal  = FALSE;
				BaseControls->PosCtrlSts.RefZSignal = FALSE;
			}
			BaseControls->CtrlCmdMngr.PerrClrReq = FALSE;
		}
		else
		{
#if ( SVFSEL_BLSH_COMMON_PROC )
			/* バックラッシ補正無効 */
			BlshInitializeCompensate( &(BaseControls->PosManager.BlshCmp) );
#endif
			/* サーボ制御位置管理変数初期化 */
			PosMngInitPositionManager( BaseControls );
			/* 位置偏差クリア要求 */
			BaseControls->CtrlCmdMngr.PerrClrReq = TRUE;
			/* ScanB位置偏差クリア */
			MlibResetLongMemory( &(BaseControls->PosManager.PosErrA), sizeof(BaseControls->PosManager.PosErrA)/4 );	/* <S107> */
			MlibResetLongMemory( &(BaseControls->PosManager.PosErrAmp), sizeof(BaseControls->PosManager.PosErrAmp)/4 );
			return( 0 );
		}
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		通常運転モード																			*/
	/*----------------------------------------------------------------------------------------------*/
	else
	{
/* 2012.09.06 Y.Oka ★パルス入力倍率変更機能現状未対応★ */
#if SVFSEL_PULSE_INPUT_SWITCHING == 1
	/*------------------------------------------------------------------------------------------*/
	/*		指令パルス倍率処理															<V677>	*/
	/*		BoutK.PcmdRingCntrの更新処理は、LpxMultiplyInputPulse()内にて実施					*/
	/*------------------------------------------------------------------------------------------*/
		/* ★★dPosRefoへの代入を忘れずに！！★★ */
		lwk_dPcmdHwCntr     = LpxMultiplyInputPulse( );	/* 位置指令差分値更新[指令単位/Scan]	*/
														/* 位置指令リングカウンタ更新			*/
#else
#if 0 /* 2012.09.06 Y.Oka ★位置偏差クリア現状未対応★ */
		lwk_dPcmdHwCntr = PosManager->var.dPcmdHwCntr;	/* 位置指令差分値更新[指令単位/Scan]	*/
#endif
//		BoutK.PcmdRingCntr += BinK.dPcmdInputHwCntr;	/* 位置指令リングカウンタ更新			*/
		dPosRefo = PosManager->var.dPcmdInputHwCntr;	/* 位置指令差分値更新[指令単位/Scan]	*/
#endif

#if 0 /* 2012.09.06 Y.Oka ★位置偏差クリア現状未対応★ */
	/*------------------------------------------------------------------------------------------*/
	/*		位置偏差クリア処理																	*/
	/*------------------------------------------------------------------------------------------*/
		BoutV.f.PerClrCmd = BinV.f.PerClrCmd;			/* 位置偏差クリア指令					*/
		if( BoutV.f.PerClrCmd )							/* 位置偏差クリア指令					*/
		{
			BpxEdgeClrSvcPositionManager( );			/* 位置偏差エッジクリア変数初期化		*/
		//	BoutV.AmondPosCmd = BinK.dPcmdHwCntr;		/* 位置指令モニタ用	[指令単位/Scan]		*/
			BoutV.AmondPosCmd =  lwk_dPcmdHwCntr;		/* 位置指令モニタ用	[指令単位/Scan]		*//*<V677>*/
			BoutV.f.PerClrEdge = FALSE;					/* 位置偏差クリアエッジ信号リセット		*/
			BoutK.f.PerClrEdge = FALSE;
			return( FALSE );							/* 位置制御終了							*/
		}
#endif

#if 0 /* 2012.09.06 Y.Oka ★INHIBIT機能現状未対応★ */
	/*------------------------------------------------------------------------------------------*/
	/*		パルス列位置指令入力																*/
	/*------------------------------------------------------------------------------------------*/
		dPosRefo = ( BinV.f.Sv.Inhibit ) ? 0 : dPosRefo;
#endif /* 2012.09.06 Y.Oka ★INHIBIT機能現状未対応★ */

#if 0 /* 2012.09.06 Y.Oka ★位置偏差クリア現状未対応★ */
	/*------------------------------------------------------------------------------------------*/
	/*		位置偏差エッジクリア処理															*/
	/*------------------------------------------------------------------------------------------*/
		if( Iprm.f.PerClrEdge && BinK.f.PerClrEdge )
		{
			BoutV.f.PerClrEdge = TRUE;					/* 位置偏差クリアエッジ信号セット		*/
			BpxPerClrSignalProcedure( );				/* 位置偏差エッジクリア信号処理			*/
			BoutK.f.PerClrEdge = TRUE;					/* FB位置・指令位置更新用フラグ			*/
		}
		else
		{
			BoutV.f.PerClrEdge = FALSE;					/* 位置偏差クリアエッジ信号リセット		*/
			BoutK.f.PerClrEdge = FALSE;					/* FB位置・指令位置更新用フラグ			*/
		}
	/*------------------------------------------------------------------------------------------*/
#endif
	}
	/* 位置指令リングカウンタ更新 */
	BaseControls->CtrlCmdMngr.PosRef += dPosRefo;
	/* 分割位置指令の更新 */
	PosManager->var.dPcmda = dPosRefo;


/*--------------------------------------------------------------------------------------------------*/
/*		位置指令電子ギア変換：PosMngV.dPcmdEgear [Pulse/Scan]										*/
/*--------------------------------------------------------------------------------------------------*/
	if( PosManager->var.EgearDisable )
	{ /* 電子ギヤ無効 */
		PosManager->var.dPcmdEgear = PosManager->var.dPcmda;
	}
	else
	{ /* 電子ギヤ有効 */
		/* 電子ギヤ演算 */
		PosManager->var.dPcmdEgear = MlibPcmdEgear( PosManager->var.dPcmda,
													PosManager->Egear,
													&(PosManager->var.dPcmdEgrem) );
	}

/*--------------------------------------------------------------------------------------------------*/
/*		払出し完了信号処理：払出し未完モジュールがFALSEにする。										*/
/*--------------------------------------------------------------------------------------------------*/
	BaseControls->PosCtrlSts.RefZSignal = ( PosManager->var.dPcmdEgear != 0 ) ? FALSE : TRUE;

/*--------------------------------------------------------------------------------------------------*/
/*		位置指令フィルタ演算(加減速,移動平均,振動抑制)：BoutV.dPcmdFilo  [Pulse/Scan]				*/
/*--------------------------------------------------------------------------------------------------*/
#if 0 /* 2012.08.07 Y.Oka 位置指令フィルタ現状未対応 */
	BaseControls->CtrlCmdMngr.dPcmdFilo = BpxPcmdFilter( PosManager->var.dPcmdEgear );
#else
	BaseControls->CtrlCmdMngr.dPcmdFilo = PosManager->var.dPcmdEgear;
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		拡張(ＦＦ)制御演算(ＭＦＣ,ゆれどめ制御)														*/
/*--------------------------------------------------------------------------------------------------*/
/*		BoutV.dPosRefi [Pulse/Scan]：位置制御位置指令差分入力										*/
/*--------------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------*/
	/*		バックラッシ補正処理																	*/
	/*----------------------------------------------------------------------------------------------*/
#if ( CSW_SVCF_BACKLASHCOMP )
	if( (BaseControls->BeComplete == FALSE)
		|| (BaseControls->PhaseReady == FALSE)
		|| (BaseControls->OverTravel == TRUE) )
	{
		BlshActive = FALSE;
	}
	else
	{
		BlshActive = TRUE;
	}
	BlshCompensateBacklash( &(PosManager->BlshCmp),
							&(PosManager->conf.Egear),
							&(PosManager->var.dPcmdEgear),
							&(dPosRefo),
							&(BaseControls->PosCtrlSts.RefZSignal),
							BlshActive );
#endif

	/*----------------------------------------------------------------------------------------------*/
	/*		振動抑制フィルタ処理																	*/
	/*----------------------------------------------------------------------------------------------*/
#if ( CSW_SVCF_RESVIB_FIL_USE == TRUE )
	if( PosManager->var.PcmdFilStop == FALSE )
	{ /* 位置指令フィルタ無効要求なし */
		/* 振動抑制フィルタ強制無効化処理(位置制御以外は無効) */
		if( BaseControls->CtrlModeSet.CtrlModeLst.b.cm != CTRLMODE_POS )
		{
			BaseControls->VibSupFil.conf.Pexe.enable = 0;
		}
		/* 振動抑制フィルタ演算 */
		BaseControls->CtrlCmdMngr.dPcmdFilo = posMngVibSupFilter(
												&(BaseControls->VibSupFil),
												PosManager->var.dPcmdEgear,
												&(BaseControls->PosCtrlSts.RefZSignal) );
	}
	else
	{ /* 位置指令フィルタ無効要求あり */
		BaseControls->CtrlCmdMngr.dPcmdFilo = PosManager->var.dPcmdEgear;
	}
#else
		BaseControls->CtrlCmdMngr.dPcmdFilo = PosManager->var.dPcmdEgear;
#endif

	/*----------------------------------------------------------------------------------------------*/
	/*		MFC処理																					*/
	/*----------------------------------------------------------------------------------------------*/
#if ( CSW_SVCF_MFC_USE == TRUE )
	/* MFC強制無効化処理(位置制御以外は無効) */
	if( BaseControls->CtrlModeSet.CtrlModeLst.b.cm != CTRLMODE_POS )
	{ /* 前回の制御モードが位置制御でない場合 */
		/* MFC無効に初期化 */
		MFControl->conf.MfcPexe.MFCModel = 0;
	}
	/* MFC演算 */
	dPosRefo = MfcControlMain( &(BaseControls->MFControl),
							   BaseControls->CtrlCmdMngr.dPcmdFilo,
							   &(BaseControls->CtrlCmdMngr.SpdFFCx),
							   &(BaseControls->CtrlCmdMngr.TrqFFCx) );
#else
	MFControl->conf.MfcPexe.MFCModel = 0;
	dPosRefo = BaseControls->CtrlCmdMngr.dPcmdFilo;
#endif


/*--------------------------------------------------------------------------------------------------*/
/*		制御位置信号処理																			*/
/*--------------------------------------------------------------------------------------------------*/
	/* Make Coin and Near Status */
	posMngCtrlPosSignal( PosManager, &(BaseControls->PosCtrlSts) );

	return dPosRefo;
}
#endif /* (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE) */



#if 0 /* 2012.08.07 Y.Oka 位置指令フィルタ現状未対応 */
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)
/****************************************************************************************************/
/*																									*/
/*		位置指令フィルタ処理メイン関数																*/
/*																									*/
/****************************************************************************************************/
LONG	BpxPcmdFilter( LONG pcmdin )
{
LONG	pcmdout;
LONG	orgpcmdin;

/*--------------------------------------------------------------------------------------------------*/
/*		位置指令フィルタ選択処理																	*/
/*--------------------------------------------------------------------------------------------------*/
	FilMng.FilSel = Iprm.PcmdFilter;

/*--------------------------------------------------------------------------------------------------*/
/*		位置指令フィルタ切替え禁止処理：切替え不可モジュールがＴＲＵＥにすること					*/
/*--------------------------------------------------------------------------------------------------*/
	FilMng.FilChgDisable = FALSE;

/*--------------------------------------------------------------------------------------------------*/
/*		位置指令フィルタ実行／停止処理																*/
/*--------------------------------------------------------------------------------------------------*/
	if ( FilMng.FilStop == TRUE )
	{
		orgpcmdin = pcmdin;
		pcmdin = 0;
	}
	else
	{
		orgpcmdin = 0;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		位置指令フィルタ選択ユーザ定数によるフィルタ選択＆実行処理									*/
/*--------------------------------------------------------------------------------------------------*/
	pcmdout = PcmdMoveAvergeFilter( pcmdin );			/* 移動平均フィルタ演算						*/
	pcmdout = PcmdExpFilter( pcmdout );					/* 加減速フィルタ演算						*/
	pcmdout += orgpcmdin;
	
	return( pcmdout );
}
#endif /* (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE) */
#endif /* 2012.08.07 Y.Oka 位置指令フィルタ現状未対応 */



/****************************************************************************************************/
/*																									*/
/*		位置偏差クリア信号処理																		*/
/*																									*/
/****************************************************************************************************/
void	BpxPerClrSignalProcedure( BASE_LOOPCTRLS *BaseLoops, BPRMDAT *Bprm )
{
BASE_CTRL_HNDL *BaseControls;
LONG	lwk;

		BaseControls = BaseLoops->BaseCtrls;	
		/*------------------------------------------------------------------------------------------*/
		/*	サーボ制御位置管理変数初期化															*/
		/*------------------------------------------------------------------------------------------*/
		BpxEdgeClrSvcPositionManager( BaseLoops );

		/*------------------------------------------------------------------------------------------*/
		/*	ＦＢ位置補正演算		-> (PlsIf.c)													*/
		/*------------------------------------------------------------------------------------------*/
		/*------------------------------------------------------------------------------------------*/
		/*	ＦＢ位置計算			-> (PlsIf.c)													*/
		/*------------------------------------------------------------------------------------------*/
		BaseControls->CtrlCmdMngr.ClrFbPos = BaseControls->ClrFbPos;	/* エッジ入力時のＦＢ位置	*/
		if(Bprm->FencUse != FALSE)/* <S04D>:ﾌﾙｸ処理追加 */
		{
			lwk = BaseControls->MotSts.FencPos - BaseControls->ClrFbPos;/* set /CLR Position 		*/
		}
		else
		{
			lwk = BaseControls->MotSts.MotPos - BaseControls->ClrFbPos;	/* set /CLR Position 		*/
		}
		MlibAposRg64dp(
					-lwk,
					&(BaseControls->PosManager.conf.Egear), 
					&(BaseControls->PosManager.PgPos), 
					&(BaseControls->MonPerrLpos[0]) );
		/*------------------------------------------------------------------------------------------*/
		/*	<S112> Set position error (Un008)														*/
		/*------------------------------------------------------------------------------------------*/
#if	0	/* 不要？ <S14C> */
		lwk = (BaseControls->PosManager.PgPos.apos[0] - BaseControls->MonPerrLpos[0]);
		BaseControls->PosManager.var.PosErrA.per32s = -lwk;			/* set position error [ref]		*/
		BaseControls->PosManager.var.PosErrA.per32a = MlibABS(lwk);	/* set abs. position error [ref]*/
#endif
		/*------------------------------------------------------------------------------------------*/
		return;
}

/****************************************************************************************************/
/*																									*/
/*		サーボ制御位置偏差エッジクリア変数初期化													*/
/*																									*/
/****************************************************************************************************/
void	BpxEdgeClrSvcPositionManager( BASE_LOOPCTRLS *BaseLoops )
{
BASE_CTRL_HNDL	*BaseControls;
POSCTRL			*PosCtrl;
POS_MNG_HNDL	*PosMngV;
	
		BaseControls = BaseLoops->BaseCtrls;	
		PosCtrl = &(BaseLoops->PosCtrl);
		PosMngV = &(BaseControls->PosManager);
/*--------------------------------------------------------------------------------------------------*/
/*		サーボ制御位置管理変数初期化																*/
/*--------------------------------------------------------------------------------------------------*/
//TODO:アナログモニタ用の位置指令もクリア	BoutV.AmondPosCmd  = 0;		/* 位置指令モニタ変数		*/
		/* Ｃ相出力時間もクリアしていたので、それ以外の変数をクリアする								*/
		MlibResetLongMemory( &(PosMngV->var.Per64),sizeof(PosMngV->var.Per64)/4); /* <S14D>			*/
		MlibResetLongMemory( &(PosMngV->var.PosErrA), sizeof(PERRA)/4 );/* 位置管理用変数初期化<S14C> */
		PosMngV->var.dPcmdEgear = 0;								/* 位置指令電子ギヤ出力			*/
		PosMngV->var.dPcmdEgrem = 0;								/* 位置指令電子ギヤ演算余り		*/
		PosMngV->var.dPcmda = 0;									/* 位置指令増分値入力[指令単位]	*/
/*--------------------------------------------------------------------------------------------------*/
/*		サーボ制御位置管理変数初期化																*/
/*--------------------------------------------------------------------------------------------------*/
		BaseControls->SettlingTime.LastdPcmda = 0;			/* 前回位置指令増分値[指令単位]			*/
		
		MlibResetLongMemory( &(PosMngV->BlshCmp.var), sizeof(PosMngV->BlshCmp.var)/4 );	/* バックラッシ補正変数初期化		*/

		PosCtrl->V.PerIvar64[0] = 0;						/* 位置積分バッファのクリア		<S14C>	*/
		PosCtrl->V.PerIvar64[1] = 0;						/* 位置積分バッファのクリア				*/
		PosCtrl->V.LastPacOut = 0;							/* 位置微分前回値のクリア				*/
		PosCtrl->V.dPFfFilo = 0;							/* 位置ＦＦフィルタバッファのクリア		*/

		BaseControls->CtrlCmdMngr.dPFfFilo = 0;				/* 位置微分前回値のクリア				*/

		BaseControls->PosManager.dPosRefi = 0;				/* 位置指令差分入力[Pulse/Scan]			*/
		BaseControls->PosManager.CompdPosRefi = 0;			/* 位置指令差分入力(ScanB->ScanA)		*/
		BaseControls->PosManager.CompdPcmda = 0;			/* 位置指令増分値入力(ScanB->ScanA)		*/
	/*----------------------------------------------------------------------------------------------*/
	if( BaseControls->CtrlModeSet.CtrlMode.b.cm == CTRLMODE_POS )
	{
		BaseControls->PosCtrlSts.CoinSignal = TRUE;			/* @CHK@:SGDSにあわせた					*/
		BaseControls->PosCtrlSts.NearSignal = TRUE;			/* @CHK@:SGDSにあわせた					*/
	}
	else
	{
		BaseControls->PosCtrlSts.CoinSignal = FALSE;		/* @CHK@:SGDSにあわせた					*/
		BaseControls->PosCtrlSts.NearSignal = FALSE;		/* @CHK@:SGDSにあわせた					*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		位置指令関連変数初期化：位置指令フィルタ,ＭＦＣなど											*/
/*--------------------------------------------------------------------------------------------------*/
	/* 位置指令フィルタ初期化処理 */
//	posMngInitPcmdFilter( &(BaseControls->VibSupFil) );												  /* <S065> */
	PcmdFilInitialize( &(BaseControls->PcmdFil), &(BaseControls->VibSupFil) );
	/* MFC変数初期化 */
	MfcInitModelControl( &(BaseControls->MFControl) );

	return;

}
/***************************************** end of file **********************************************/
