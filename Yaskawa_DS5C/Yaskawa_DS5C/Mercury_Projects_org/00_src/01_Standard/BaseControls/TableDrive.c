/****************************************************************************************************/
/*																									*/
/*																									*/
/*		TableDrive.c : テーブル運転用	モジュール定義												*/
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
/*	Note	:	初版	2011.02.18	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "TableDrive.h"
#include "BaseCmnStruct.h"
#include "PosManager.h"
#include "ModelControl.h"
#include "DataTraceManager.h"
#include "MLib.h"


/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
/* void	TblDriveInitialize( TBLDRIVE *TblDrive );													*/
/* void	TblDriveGetTraceRegData( TBLDRIVE *TblDrive, USHORT DataSize, USHORT SampIntv,				*/
/*															ULONG DataSampIntv, ULONG DataNum );	*/
/* void	TblDriveExecute( TBLDRIVE *TblDrive, TBLCHK_SV_STS *TblChkSts, TRCHNDL *TrcHndl,			*/
/*													POS_MNG_HNDL *PosManager, MFCTRL *MFControl );	*/
/* PRM_RSLT	TblDriveCalculateParameter( TBLDRIVE *TblDrive, LONG XfbPulse1 );						*/
/* PRM_RSLT	TblDrivePrepareProc( TBLDRIVE *TblDrive );												*/
/* PRM_RSLT	TblDriveStartRequest( TBLDRIVE *TblDrive, LONG TblDir );								*/
/* PRM_RSLT	TblDriveSetStopRequest( TBLDRIVE *TblDrive );											*/
/****************************************************************************************************/
static	void tblDriveOutputRef( TBLDRIVE *TblDrive );
static	void tblDriveIPolatePosRef( TBLDRIVE *TblDrive );
static	void tblDriveSwitchPcmdFil( TBLDRIVE *TblDrive, POS_MNG_HNDL *PosManager,
																MFCTRL *MFControl, BOOL Switch );
static	void tblDriveSwitchEgear( TBLDRIVE *TblDrive, POS_MNG_HNDL *PosManager, BOOL Switch );
static	void tblDriveSetTrqFilter( TBLDRIVE *TblDrive, LONG TrqInputPos );


/****************************************************************************************************/
/*																									*/
/*		テーブル運転初期化処理																		*/
/*																									*/
/****************************************************************************************************/
void	TblDriveInitialize( TBLDRIVE *TblDrive, USHORT *BufferAddress , TRCHNDL *TrcHndl )
{
	/* Reset Table Drive Variables */
	MlibResetLongMemory( &(TblDrive->var), sizeof(TblDrive->var)/4 );
	/* Set Table Refference Buffer Pointer */
	TblDrive->conf.TblBuffer = BufferAddress;
	TblDrive->TrcHndl = TrcHndl;
}


/****************************************************************************************************/
/*																									*/
/*		テーブル運転実行パラメータ計算処理															*/
/*																									*/
/****************************************************************************************************/
void	TblDriveGetTraceRegData( TBLDRIVE *TblDrive, USHORT DataSize, USHORT SampIntv,
																ULONG DataSampIntv, ULONG DataNum )
{
	TblDrive->OpeReg.TblDriveDataSize = DataSize;
	TblDrive->OpeReg.TblDriveSampIntv = SampIntv;
	TblDrive->conf.DataSampIntv = DataSampIntv;
	TblDrive->conf.TblDataNum = DataNum;
}


/****************************************************************************************************/
/*																									*/
/*		テーブル運転実行パラメータ計算処理															*/
/*																									*/
/****************************************************************************************************/
PRM_RSLT	TblDriveCalculateParameter( TBLDRIVE *TblDrive, LONG XfbPulse1 )
{
	LONG	x;

	/* テーブル運転パラメータ計算完了フラグクリア */
	TblDrive->conf.TblPrmOk = FALSE;

/*--------------------------------------------------------------------------------------------------*/
/*		テーブル運転モード設定																		*/
/*--------------------------------------------------------------------------------------------------*/
	switch( TblDrive->OpeReg.TblMode )
	{
	case 0x01:
		TblDrive->conf.CtrlMode = CTRLMODE_TRQ;					/* トルク制御						*/
		TblDrive->conf.RefOutPx = &TblDrive->var.TrqRef;		/* 指令出力ポインタ					*/
		break;
	case 0x02:
		TblDrive->conf.CtrlMode = CTRLMODE_SPD;					/* 速度制御							*/
		TblDrive->conf.RefOutPx = &TblDrive->var.SpdRef;		/* 指令出力ポインタ					*/
		break;
	case 0x03:
		TblDrive->conf.CtrlMode = CTRLMODE_POS;					/* 位置制御：YEC制御				*/
		TblDrive->conf.RefOutPx = &TblDrive->var.dPosRef;		/* 指令出力ポインタ					*/
		break;
	case 0x04:
		TblDrive->conf.CtrlMode = CTRLMODE_POS;					/* 位置制御：NCT制御				*/
		TblDrive->conf.RefOutPx = &TblDrive->var.dPosRef;		/* 指令出力ポインタ					*/
		return( PRM_RSLT_CONDITION_ERR );						/* 仮処置(NCT未対応)				*/
	case 0x05:
		TblDrive->conf.CtrlMode = CTRLMODE_POS;					/* 位置制御：トルク外乱指令			*/
		TblDrive->conf.RefOutPx = &TblDrive->var.TrqRef;		/* 指令出力ポインタ					*/
		break;
	default:
		TblDrive->conf.CtrlMode = CTRLMODE_POS;					/* デフォルト						*/
		TblDrive->conf.RefOutPx = &TblDrive->var.dPosRef;		/* 指令出力ポインタ					*/
		return( PRM_RSLT_CONDITION_ERR );						/* 異常リターン						*/
	}
	TblDrive->conf.TblMode = (UCHAR)TblDrive->OpeReg.TblMode;

/*--------------------------------------------------------------------------------------------------*/
/*		テーブル運転：指令タイプ, 指令補間回数, 指令補間シフト数の設定								*/
/*--------------------------------------------------------------------------------------------------*/
	/* 指令補間回数 */
	TblDrive->conf.RefIpNum = TblDrive->OpeReg.TblDriveSampIntv;
	if( (TblDrive->conf.TblMode == 0x03) || (TblDrive->conf.TblMode == 0x04) )
	{
		/* 2の累乗チェック */
		x = MlibSrhbiton( TblDrive->conf.RefIpNum, SRH_FROM_LSB );
		if( (x < 0) || (TblDrive->conf.RefIpNum != (ULONG)(1 << x)) )
		{
			x = -1;
		}

		/* 補間回数設定 */
		if( TblDrive->conf.RefIpNum <= 1 )
		{
			/* 位置指令：補間無し */
			TblDrive->conf.RefType = TBLREFTYPE_POSIP1;
		}
		else if( x >= 0 )
		{
			/* 位置指令：補間回数 == 2^n */
			TblDrive->conf.RefType = TBLREFTYPE_POSIP2;
			/* 指令補間シフト数 */
			TblDrive->conf.RefIpSft = (UCHAR)x;
		}
		else
		{
			/* 位置指令：補間回数 != 2^n */
			TblDrive->conf.RefType = TBLREFTYPE_POSIP3;
		}
	}
	else
	{
		/* 速度指令／トルク指令 */
		TblDrive->conf.RefType = TBLREFTYPE_NOTPOS;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		テーブル運転：モータパルス数,マルチターンリミットの設定										*/
/*--------------------------------------------------------------------------------------------------*/
	/* モータパルス数 [p/rev,p/mm] */
	TblDrive->conf.MotPulse = XfbPulse1;
	/* マルチターンリミット [rev,mm] */
	TblDrive->conf.MtnLimit = TblDrive->OpeReg.TblLimit;

/*--------------------------------------------------------------------------------------------------*/
/*		テーブル運転：機能選択																		*/
/*--------------------------------------------------------------------------------------------------*/
	/* トルク入力位置 */
	TblDrive->conf.TrqInputPos = (TblDrive->OpeReg.TblMode == 0x05) ? 
										(UCHAR)TblDrive->OpeReg.TblFuncSel : 0;
	/* 調整レス制御無効 */
	TblDrive->conf.TuneLessDisable =
		(TblDrive->OpeReg.TblFuncSel & TBLFUNCBIT_TUNELESS)? TRUE : FALSE;
	/* 電子ギア比無効 */
	TblDrive->conf.EGearDisable =
		(TblDrive->OpeReg.TblFuncSel & TBLFUNCBIT_EGEAR)? TRUE : FALSE;
	/* 位置指令フィルタ無効 */
	TblDrive->conf.PcmdFilDisable =
		(TblDrive->OpeReg.TblFuncSel & TBLFUNCBIT_PCMDFIL)? TRUE : FALSE;

/*--------------------------------------------------------------------------------------------------*/
/*		テーブル運転パラメータ計算完了																*/
/*--------------------------------------------------------------------------------------------------*/
	/* テーブル運転パラメータOKセット */
	TblDrive->conf.TblPrmOk = TRUE;

	/* パラメータ計算完了リターン */
	return( PRM_RSLT_SUCCESS );
}


/****************************************************************************************************/
/*																									*/
/*		テーブル運転実行制御																		*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		テーブル運転指令出力マクロ																	*/
/*--------------------------------------------------------------------------------------------------*/
static	void tblDriveOutputRef( TBLDRIVE *TblDrive )
{
	USHORT	Idx;
	LONG	TblRef;

	Idx = TblDrive->var.TblIndx;

	if( (TblDrive->OpeReg.TblDriveDataSize == TBLREFLENGTH_31BIT_IO)
		|| (TblDrive->OpeReg.TblDriveDataSize == TBLREFLENGTH_32BIT) )
	{ /* LONG Data */
		TblRef = ((LONG *)TblDrive->conf.TblBuffer)[Idx] * TblDrive->conf.KoutDir;
	}
	else if( (TblDrive->OpeReg.TblDriveDataSize == TBLREFLENGTH_15BIT_IO)
			|| (TblDrive->OpeReg.TblDriveDataSize == TBLREFLENGTH_16BIT) )
	{ /* SHORT Data */
		TblRef = ((SHORT*)TblDrive->conf.TblBuffer)[Idx] * TblDrive->conf.KoutDir;
	}
	else /* TBLREFLENGTH_NOTDEF and Others */
	{ /* Data Setting Error (SHORT) */
		TblRef = ((SHORT*)TblDrive->conf.TblBuffer)[Idx] * TblDrive->conf.KoutDir;
	}

	if( TblDrive->conf.RefType == TBLREFTYPE_NOTPOS )
	{ /* 指令タイプ：速度/トルク指令 */
		TblRef = MlibLIMITUL( TblRef, 0x01000000, -0x01000000 );
		/* 速度/トルク指令出力 */
		*TblDrive->conf.RefOutPx = TblRef;
	}
	else
	{ /* 指令タイプ：位置指令 */
		/* 位置指令補間演算入力 */
		TblDrive->var.InPosRef = TblRef;
		/* テーブル運転位置指令補間処理 */
		tblDriveIPolatePosRef( TblDrive );
	}
}


/****************************************************************************************************/
/*																									*/
/*		テーブル運転実行制御関数																	*/
/*																									*/
/****************************************************************************************************/
void	TblDriveExecute( TBLDRIVE *TblDrive, TBLCHK_SV_STS *TblChkSts, 
													POS_MNG_HNDL *PosManager, MFCTRL *MFControl )
{

/*--------------------------------------------------------------------------------------------------*/
/*		テーブル運転実行指令チェック																*/
/*--------------------------------------------------------------------------------------------------*/
	if( TblDrive->var.TblCmd == TBLCMD_NOCMD )
	{
		TblDrive->var.TblStep = 0;
		TblDrive->var.TrqRef  = 0;
		TblDrive->var.SpdRef  = 0;
		TblDrive->var.dPosRef = 0;
		TblDrive->var.TblZmd.dw = 0x0000;
		return;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		テーブル運転実行シーケンス制御																*/
/*--------------------------------------------------------------------------------------------------*/
	switch( TblDrive->var.TblStep )
	{
	case 0: /* Initial Step */
		TblDrive->var.TrqRef  = 0;
		TblDrive->var.SpdRef  = 0;
		TblDrive->var.dPosRef = 0;
		TblDrive->var.MotMtn  = 0;
		TblDrive->var.MotPos  = 0;
		TblDrive->var.TblIndx = 0;
		TblDrive->var.TblStep = 1;
		TblDrive->var.TblZmd.dw = 0x0000;

		/* ZeroClamp Control */
		TblDrive->var.TblZmd.b.zm = CTRLMODE_ZCLMP;
		break;

	case 1: /* Wait Base Enable */
	case 2: /* ZeroClamp and Wait Start Table Drive */
		if( TblDrive->var.TblCmd == TBLCMD_TABLEDRIVE )
		{
			TblDrive->var.TblStep = 3;
			TblDrive->var.TblIndx = 0;
			TblDrive->var.TblSamp = 0;

			if( TblDrive->var.WithTrcReq )
			{ /* トレース同時実行要求あり */
				DtrcSetTrgPointToZero(  TblDrive->TrcHndl );
			}

			/* トルク指令フィルタ制御 */
			tblDriveSetTrqFilter( TblDrive, TblDrive->conf.TrqInputPos );
			/* 電子ギヤ制御 */
			tblDriveSwitchEgear( TblDrive, PosManager, !(TblDrive->conf.EGearDisable) );
			/* 位置指令フィルタ制御 */
			tblDriveSwitchPcmdFil( TblDrive, PosManager, MFControl, !(TblDrive->conf.PcmdFilDisable) );

			TblDrive->var.ErrState = TBLCMD_STS_INIT;
		}
		break;

	case 3: /* Table Drive */
		/*------------------------------------------------------------------------------------------*/
		/*	アラーム発生チェック処理																*/
		/*------------------------------------------------------------------------------------------*/
		if ( TblChkSts->AlmFlag )
		{
			TblDrive->var.ErrState = TBLCMD_STS_ALARM;
		}
		/*------------------------------------------------------------------------------------------*/
		/*	HWBB状態チェック処理																	*/
		/*------------------------------------------------------------------------------------------*/
		else if ( TblChkSts->HwbbSts )
		{
			TblDrive->var.ErrState = TBLCMD_STS_HWBB;
		}
		/*------------------------------------------------------------------------------------------*/
		/*	主回路電源オフチェック処理																*/
		/*------------------------------------------------------------------------------------------*/
		else if ( !(TblChkSts->MainPowerOn) )
		{
			TblDrive->var.ErrState = TBLCMD_STS_POWEROFF;
		}
		/*------------------------------------------------------------------------------------------*/
		/*	オーバトラベルチェック処理																*/
		/*------------------------------------------------------------------------------------------*/
		else if ( TblChkSts->OverTrvlSts )
		{
			TblDrive->var.ErrState = TBLCMD_STS_OT;
		}
		/*------------------------------------------------------------------------------------------*/
		/*	モータ通電チェック処理																	*/
		/*------------------------------------------------------------------------------------------*/
		else if ( !(TblChkSts->BeComplete) )
		{
			TblDrive->var.ErrState = TBLCMD_STS_BB;
		}
		/*------------------------------------------------------------------------------------------*/
		/*	トルク制限チェック処理																	*/
		/*------------------------------------------------------------------------------------------*/
		else if ( TblChkSts->TrqClamp )
		{
			TblDrive->var.ErrState = TBLCMD_STS_TRQLMT;
		}
		/*------------------------------------------------------------------------------------------*/
		/*	マルチターンリミットチェック処理														*/
		/*------------------------------------------------------------------------------------------*/
		if( TblDrive->conf.MtnLimit > 0 )
		{
			/* マルチターン演算 */
			TblDrive->var.MotPos += TblChkSts->dMotPos;
			if( TblDrive->var.MotPos >= TblDrive->conf.MotPulse )
			{
				TblDrive->var.MotMtn += 1;
				TblDrive->var.MotPos -= TblDrive->conf.MotPulse;
			}
			else if( TblDrive->var.MotPos <= -TblDrive->conf.MotPulse )
			{
				TblDrive->var.MotMtn -= 1;
				TblDrive->var.MotPos += TblDrive->conf.MotPulse;
			}
			if( (TblDrive->var.MotMtn >= TblDrive->conf.MtnLimit)
				|| (TblDrive->var.MotMtn <= -TblDrive->conf.MtnLimit) )
			{
				/* Table Drive Error Step */
				TblDrive->var.TblStep = 5;
				/* Clear Reference Output */
				*TblDrive->conf.RefOutPx = 0;
				/* TableDriveState = MturnErr */
				TblDrive->OpeReg.TblState = TBLCMD_STS_POSLMT;
				/* ZeroClamp Control */
				TblDrive->var.TblZmd.b.zm = CTRLMODE_ZCLMP;
				break;
			}
		}
		/*------------------------------------------------------------------------------------------*/
		/*	テーブル運転指令作成処理																*/
		/*------------------------------------------------------------------------------------------*/
		if( TblDrive->var.WithTrcReq )
		{
			if( (TblDrive->var.TblSamp == 0)
				|| (TblDrive->var.TblSamp > (TblDrive->OpeReg.TblDriveSampIntv - TblDrive->conf.DataSampIntv)) )
			{
				/* Set Trace Variable Function */
				DtrcSetTrcVar( TblDrive->TrcHndl );
			}
		}
		/*------------------------------------------------------------------------------------------*/
		if( TblDrive->var.TblSamp == 0 )
		{
			/* テーブル運転終了チェック */
			if( TblDrive->var.TblIndx >= TblDrive->conf.TblDataNum )
			{
				TblDrive->var.TblStep = 4;					/* Table Drive End Step					*/
				*TblDrive->conf.RefOutPx = 0;				/* Clear Reference Output				*/
				TblDrive->var.TblZmd.b.zm = CTRLMODE_ZCLMP;	/* ZeroClamp Control					*/
				break;
			}
			/* テーブル運転指令出力処理 */
			tblDriveOutputRef( TblDrive );

			/* データトレース実行処理 */
			if( TblDrive->var.WithTrcReq )
			{
				/* Data Trace Procedure */
				DtrcProcedure( TblDrive->TrcHndl, TblDrive->var.TblIndx );

				/* Reset Average Variable */
				DtrcResetAverageVar( TblDrive->TrcHndl );
			}

			/* テーブル運転インデクス更新 */
			TblDrive->var.TblIndx++;
		}
		else if( TblDrive->conf.RefType != TBLREFTYPE_NOTPOS )
		{ /* 指令タイプチェック */
			/* テーブル運転位置指令補間処理 */
			tblDriveIPolatePosRef( TblDrive );
		}
		/*------------------------------------------------------------------------------------------*/
		TblDrive->var.TblSamp++;
		if( TblDrive->var.TblSamp >= TblDrive->OpeReg.TblDriveSampIntv )
		{
			TblDrive->var.TblSamp = 0;
		}
		/*------------------------------------------------------------------------------------------*/
		TblDrive->OpeReg.TblState =TBLCMD_STS_RUN;			/* TableDriveState = Execute			*/
		TblDrive->var.TblZmd.b.zm = CTRLMODE_ZNONE;			/* NoneZero Control						*/
		break;

	case 4: /* Table Drive End Step */
		/* Clear Reference Output */
		*TblDrive->conf.RefOutPx = 0;
		/* トルク指令フィルタ制御 */
		tblDriveSetTrqFilter( TblDrive, FALSE );
		/* 電子ギヤ有効 */
		tblDriveSwitchEgear( TblDrive, PosManager, TRUE );
		/* 位置指令フィルタ有効 */
		tblDriveSwitchPcmdFil( TblDrive, PosManager, MFControl, TRUE );
		if ( TblDrive->var.ErrState != 0 )
		{
			/* TableDriveState = End(ErrState) */
			TblDrive->OpeReg.TblState = TblDrive->var.ErrState;
		}
		else
		{
			/* TableDriveState = End */
			TblDrive->OpeReg.TblState = TBLCMD_STS_FINISH;
		}
		TblDrive->var.TblCmd  = TBLCMD_TBLDRV_END;			/* テーブル運転正常終了					*/
		TblDrive->var.TblZmd.b.zm = CTRLMODE_ZCLMP;			/* ZeroClamp Control					*/
		TblDrive->var.TblStep = 2;
		break;

	case 5: /* Table Drive Error Step */
		/* Clear Reference Output */
		*TblDrive->conf.RefOutPx = 0;
		/* トルク指令フィルタ制御 */
		tblDriveSetTrqFilter( TblDrive, FALSE );
		/* 電子ギヤ有効 */
		tblDriveSwitchEgear( TblDrive, PosManager, TRUE );
		/* 位置指令フィルタ有効 */
		tblDriveSwitchPcmdFil( TblDrive, PosManager, MFControl, TRUE );

		TblDrive->var.TblCmd  = TBLCMD_TBLDRV_ERR;			/* テーブル運転異常終了					*/
		TblDrive->var.TblStep = 6;
		break;

	case 6: /* Table Drive Error Step */
		if( (TblChkSts->AlmFlag == TRUE) || (TblChkSts->BeComplete == FALSE) )
		{ /* アラーム発生 || BaseBlock */
			/* NoneZero Control */
			TblDrive->var.TblZmd.b.zm = CTRLMODE_ZNONE;
		}
		else
		{
			/* ZeroClamp Control */
			TblDrive->var.TblZmd.b.zm = CTRLMODE_ZCLMP;
		}
		break;

	default: /* Default */
		break;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		ゼロ制御モード指令処理																		*/
/*--------------------------------------------------------------------------------------------------*/
	TblDrive->var.TblZmd.b.zmchg  = TblDrive->var.TblZmd.b.zm ^ TblDrive->var.TblZmd.b.zmlast;
	TblDrive->var.TblZmd.b.zmlast = TblDrive->var.TblZmd.b.zm;

	return;
}


/****************************************************************************************************/
/*																									*/
/*		テーブル運転位置指令補間処理																*/
/*																									*/
/****************************************************************************************************/
static	void tblDriveIPolatePosRef( TBLDRIVE *TblDrive )
{
/*--------------------------------------------------------------------------------------------------*/
/*		位置指令補間 : 無し																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( TblDrive->conf.RefType == TBLREFTYPE_POSIP1 )
	{
		TblDrive->var.dPosRef = TblDrive->var.InPosRef;
	}
/*--------------------------------------------------------------------------------------------------*/
/*		位置指令補間 : 補間回数 == 2^n																*/
/*--------------------------------------------------------------------------------------------------*/
	else if( TblDrive->conf.RefType == TBLREFTYPE_POSIP2 )
	{
		if( TblDrive->var.TblSamp == 0 )
		{
			TblDrive->var.IxPosRef = (TblDrive->var.InPosRef >> TblDrive->conf.RefIpSft);
			TblDrive->var.IxPosRem =
				TblDrive->var.InPosRef - (TblDrive->var.IxPosRef << TblDrive->conf.RefIpSft);
			if( TblDrive->var.InPosRef < 0 )
			{
				TblDrive->var.IxPosRef += 1;
				TblDrive->var.IxPosRem -= TblDrive->conf.RefIpNum;
			}
			TblDrive->var.WkPosRem = 0;
		}
	/*------------------------------------------------------------------------------------------*/
		if( TblDrive->var.InPosRef >= 0 )
		{ /* 正の場合 */
			TblDrive->var.WkPosRem += TblDrive->var.IxPosRem;
			if( TblDrive->var.WkPosRem > 0 )
			{
				TblDrive->var.dPosRef = TblDrive->var.IxPosRef + 1;
				TblDrive->var.WkPosRem -= TblDrive->conf.RefIpNum;
			}
			else
			{
				TblDrive->var.dPosRef = TblDrive->var.IxPosRef;
			}
		}
	/*------------------------------------------------------------------------------------------*/
		else
		{ /* 負の場合 */
			TblDrive->var.WkPosRem += TblDrive->var.IxPosRem;
			if( TblDrive->var.WkPosRem < 0 )
			{
				TblDrive->var.dPosRef = TblDrive->var.IxPosRef - 1;
				TblDrive->var.WkPosRem += TblDrive->conf.RefIpNum;
			}
			else
			{
				TblDrive->var.dPosRef = TblDrive->var.IxPosRef;
			}
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		位置指令補間 : 補間回数 != 2^n																*/
/*--------------------------------------------------------------------------------------------------*/
	else
	{
		if( TblDrive->var.TblSamp == 0 )
		{
			TblDrive->var.IxPosRef = TblDrive->var.InPosRef / TblDrive->conf.RefIpNum;
			TblDrive->var.IxPosRem =
				TblDrive->var.InPosRef - (TblDrive->var.IxPosRef * TblDrive->conf.RefIpNum);
			TblDrive->var.WkPosRem = 0;
		}
	/*------------------------------------------------------------------------------------------*/
		if( TblDrive->var.InPosRef >= 0 )
		{ /* 正の場合 */
			TblDrive->var.WkPosRem += TblDrive->var.IxPosRem;
			if( TblDrive->var.WkPosRem > 0 )
			{
				TblDrive->var.dPosRef = TblDrive->var.IxPosRef + 1;
				TblDrive->var.WkPosRem -= TblDrive->conf.RefIpNum;
			}
			else
			{
				TblDrive->var.dPosRef = TblDrive->var.IxPosRef;
			}
		}
	/*------------------------------------------------------------------------------------------*/
		else
		{ /* 負の場合 */
			TblDrive->var.WkPosRem += TblDrive->var.IxPosRem;
			if( TblDrive->var.WkPosRem < 0 )
			{
				TblDrive->var.dPosRef = TblDrive->var.IxPosRef - 1;
				TblDrive->var.WkPosRem += TblDrive->conf.RefIpNum;
			}
			else
			{
				TblDrive->var.dPosRef = TblDrive->var.IxPosRef;
			}
		}
	}
	return;
}


/****************************************************************************************************/
/*																									*/
/*		テーブル運転トルク指令フィルタ制御(有効／無効)												*/
/*																									*/
/****************************************************************************************************/
static	void tblDriveSetTrqFilter( TBLDRIVE *TblDrive, LONG TrqInputPos )
{
	if( TblDrive->conf.CtrlMode == CTRLMODE_TRQ )
	{
		switch( TrqInputPos )
		{
		case 0x00:								/* なし												*/
		case 0x01:								/* トルク指令フィルタ前								*/
		default :
			TblDrive->var.TrqFilDisable = FALSE;	/* トルク指令フィルタ有効						*/
			TblDrive->var.NotchLpassDisable = FALSE;/* ノッチ＆ローパスフィルタ有効					*/
//			BpiEnableCPUTorqueFilter( );		
//			KpiEnableMicroTorqueFilter( );			/* マイクロトルク指令フィルタ有効				*/
			break;
		case 0x02:								/* ローパスフィルタ後、ノッチフィルタ前				*/
			TblDrive->var.TrqFilDisable = TRUE;		/* トルク指令フィルタ無効						*/
			TblDrive->var.NotchLpassDisable = FALSE;/* ノッチ＆ローパスフィルタ有効					*/
//			BpiDisableCPUTorqueFilter( );			/* CPUトルク指令フィルタ無効					*/
//			KpiEnableMicroTorqueFilter( );			/* マイクロトルク指令フィルタ有効				*/
			break;
		case 0x03:								/* トルク指令フィルタ後								*/
			TblDrive->var.TrqFilDisable = TRUE;		/* トルク指令フィルタ無効						*/
			TblDrive->var.NotchLpassDisable = TRUE;	/* ノッチ＆ローパスフィルタ無効					*/
//			BpiDisableCPUTorqueFilter( );			/* CPUトルク指令フィルタ無効					*/
//			KpiDisableMicroTorqueFilter( );			/* マイクロトルク指令フィルタ無効				*/
			break;
		}
	}
	else if( TblDrive->conf.CtrlMode == CTRLMODE_POS )
	{
		/* トルク外乱指令入力位置選択 */
		TblDrive->var.DisTrqInTiming = TrqInputPos;
	}
	return;
}


/****************************************************************************************************/
/*																									*/
/*		テーブル運転電子ギヤ制御(有効／無効)														*/
/*																									*/
/****************************************************************************************************/
static	void tblDriveSwitchEgear( TBLDRIVE *TblDrive, POS_MNG_HNDL *PosManager, BOOL Switch )
{
	if( TblDrive->conf.CtrlMode == CTRLMODE_POS )
	{
		BpiEgearFuncSwitch( PosManager, Switch );
	}
}


/****************************************************************************************************/
/*																									*/
/*		テーブル運転位置指令フィルタ制御(有効／無効)												*/
/*																									*/
/****************************************************************************************************/
static	void tblDriveSwitchPcmdFil( TBLDRIVE *TblDrive, POS_MNG_HNDL *PosManager,
																MFCTRL *MFControl, BOOL Switch )
{
	if( TblDrive->conf.CtrlMode == CTRLMODE_POS )
	{
		BpiPcmdFilSwitch( PosManager, Switch );
		MfcSetMfcCtrlSwitch( MFControl, Switch );
	}
}


/****************************************************************************************************/
/*																									*/
/*		テーブル運転スタート用API																	*/
/*																									*/
/****************************************************************************************************/
PRM_RSLT	TblDrivePrepareProc( TBLDRIVE *TblDrive )
{
/*--------------------------------------------------------------------------------------------------*/
/*		テーブル運転処理開始チェック																*/
/*--------------------------------------------------------------------------------------------------*/
	if( TblDrive->var.TblCmd != TBLCMD_NOCMD )
	{
		return( PRM_RSLT_CONDITION_ERR );
	}
#if 0 /* トレースと分離するため使用しないよう設計する */
	else if( TrcExeV.TrcCmd != TRCCMD_NOCMD )
	{
		return( PRM_RSLT_NOACCRIGHT_ERR );
	}
	else if( TblDrive->conf.TblPrmOk != TRUE )
	{
		return( PRM_RSLT_NOACCRIGHT_ERR );
	}
	else if( TrcExeP.TrcPrmOk != TRUE )
	{
		return( PRM_RSLT_NOACCRIGHT_ERR );
	}
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		テーブル運転処理開始指令																	*/
/*--------------------------------------------------------------------------------------------------*/
	/* データトレース専用変数リセット処理 */
//	LpxResetDataTraceVarX( );

	/* テーブル運転処理開始指令 */
	TblDrive->var.TblCmd = TBLCMD_TABLESTART;

	/* 2011.02.28 Y.Oka テーブル運転中トレースを強制連動とする */
	TblDrive->var.WithTrcReq = TRUE;

	return( PRM_RSLT_SUCCESS );
}


/****************************************************************************************************/
/*																									*/
/*		テーブル運転実行開始要求API																	*/
/*																									*/
/****************************************************************************************************/
PRM_RSLT	TblDriveStartRequest( TBLDRIVE *TblDrive, LONG TblDir )
{
	LONG	KoutDir;

/*--------------------------------------------------------------------------------------------------*/
/*		テーブル運転実行開始チェック																*/
/*--------------------------------------------------------------------------------------------------*/
	if( TblDrive->conf.TblPrmOk != TRUE )
	{
		return( PRM_RSLT_CONDITION_ERR );
	}
#if 0 /* トレースと分離するため使用しないよう設計する */
	else if( TrcExeP.TrcPrmOk != TRUE )
	{
		return( REGIF_DAMTHERR2 );
	}
	else if( (TblDrive->var.TblCmd != TBLCMD_TABLESTART) && (TblDrive->var.TblCmd != TBLCMD_TBLDRV_END) )
	{
		return( REGIF_DAMTHERR2 );
	}
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		テーブル運転指令出力ゲインの計算(正規化ゲイン＋指令方向)									*/
/*--------------------------------------------------------------------------------------------------*/
	KoutDir = (TblDir >= 0)? 1 : -1;					/* テーブル運転指令方向						*/
	if( TblDrive->conf.RefType == TBLREFTYPE_NOTPOS )	/* 指令タイプ：速度 or トルク				*/
	{
		if( (TblDrive->OpeReg.TblDriveDataSize != TBLREFLENGTH_31BIT_IO)
			&& (TblDrive->OpeReg.TblDriveDataSize != TBLREFLENGTH_32BIT) )
		{ /* SHORT Data */
			KoutDir = KoutDir << 10;					/* 16384 --> 2^24 : 2^10					*/
		}
	}
	TblDrive->conf.KoutDir = KoutDir;					/* テーブル運転指令出力ゲイン				*/
/*--------------------------------------------------------------------------------------------------*/
/*		テーブル運転実行開始指令処理																*/
/*--------------------------------------------------------------------------------------------------*/
#if 0 /* トレースと分離するため使用しないよう設計する */
	if( TrcReg.TrcMode.b.h == 0x00 )					/* 指令テーブル保護モード					*/
	{
		TrcExeP.TrcObj[0].TrcExe |= 0x10;				/* トレースCH0実行禁止						*/
	}
	else												/* 指令テーブル上書きモード					*/
	{
		TrcExeP.TrcObj[0].TrcExe &= 0x0F;				/* トレースCH0実行許可						*/
	}
#endif
	TblDrive->var.TblCmd = TBLCMD_TABLEDRIVE;			/* テーブル運転実行開始指令					*/
/*--------------------------------------------------------------------------------------------------*/
	return( PRM_RSLT_SUCCESS );
}


/****************************************************************************************************/
/*																									*/
/*		テーブル運転ストップ用API																	*/
/*																									*/
/****************************************************************************************************/
PRM_RSLT	TblDriveSetStopRequest( TBLDRIVE *TblDrive )
{
	TblDrive->var.TblCmd = TBLCMD_NOCMD;

	/* 2011.02.28 Y.Oka テーブル運転中トレースを強制連動とする */
	TblDrive->var.WithTrcReq = FALSE;

	return( PRM_RSLT_SUCCESS );
}


/****************************************************************************************************/
/*																									*/
/*		テーブル運転制御モード取得用API																*/
/*																									*/
/****************************************************************************************************/
USHORT	TblDriveGetControlMode( TBLDRIVE *TblDrive )
{
	return( TblDrive->conf.CtrlMode );
}


/****************************************************************************************************/
/*																									*/
/*		テーブル運転同時トレース実行要求設定用API													*/
/*																									*/
/****************************************************************************************************/
void	TblDriveSetWithTrcReq( TBLDRIVE *TblDrive, BOOL Switch )
{
	if( Switch == TRUE )
	{ /* 同時トレース実行要求あり */
		TblDrive->var.WithTrcReq = TRUE;
	}
	else
	{ /* 同時トレース実行要求なし */
		TblDrive->var.WithTrcReq = FALSE;
	}
}


/***************************************** end of file **********************************************/
