/****************************************************************************************************/
/*																									*/
/*																									*/
/*		BaseControls.c : サーボベースコントロール管理処理定義										*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
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
#include "BaseControls.h"
#include "SerialEncIf.h"
#include "PosManager.h"
#include "ResVibCtrl.h"
#include "GainChange.h"
//#include "SequenceControls.h"
#include "PowerManager.h"
#include "MotorPoleFind.h"
#include "MLib.h"
#include "HwDeviceIf.h"
#include "JL086ServoIpHw.h"
#include "AlarmManager.h"



/****************************************************************************************************/
/*		DEFINES																					*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Output Mapping Data and Output Macro Definition												*/
/*--------------------------------------------------------------------------------------------------*/
#define	SOUT00	5
#define	SOUT01	6
#define	SOUT02	7
#define	SOUT03	4

/*--------------------------------------------------------------------------------------------------*/
/*		Mechatrolink Monitor Table																	*/
/*--------------------------------------------------------------------------------------------------*/
#define MONTBL10	0x10
#define MONTBL20	0x20
#define MONTBL30	0x30
#define MONTBL40	0x40
#define MONTBL50	0x50
#define MONTBL60	0x60


/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
/*	void BpxBinputNetIf( BASE_CTRL_HNDL*, SERVO_CONTROL_IF*, TRQ_LMT_DATA* );						*/
/*	void BpxYecServoControl( BASE_CTRL_HNDL*, SERVO_CONTROL_IF*, CTRL_LOOP_OUT*, FUN_CMN_CONTROL* );*/
/*	void BpiMpoleFindMakePosCmd( BASE_LOOPCTRLS*, SERVO_CONTROL_IF*, FN_CMN_CONTROL* );		<S02F>	*/
/*	void BpxTorqueLimitControl( TRQ_LMT_DATA*, BASE_CTRL_HNDL*, BOOL );								*/
/*	void BpxBoutputNetIf( SERVO_CONTROL_IF*, SEQ_CTRL_OUT*, LONG, LONG );							*/
/*	void BpiChangeSpdDetMaFil( MOTSPDMAFIL*, LONG num );								<S02F>		*/
/****************************************************************************************************/
static void LpxNetScanBInputExtCommand( TRQ_LMT_DATA *TrqLimits, USHORT ExtSel,
																		EXTENDED_DATA ExtData );
#if	(CSW_SVCF_FBLATCH == TRUE)
static void LpxLatchRequestGet( SERVO_CONTROL_IF *SvControlIf, LONG CmdLtBit );
#endif
//static void ModeSwitch( MODE_SW *ModeSwData, LONG trq_ref, LONG spd_ref, LONG pos_err );<S0BA>
static LONG BaseSoftStartSpdRef( CTRL_CMD_PRM *CtrlCmdPrm, LONG SpdRef, LONG SpdRefx );
static LONG	BaseLinDecStopSpdRef( CTRL_CMD_PRM *CtrlCmdPrm, LONG SpdRefo ); /* <S1F5> */
static LONG ZeroSearchControl( ZX_CTRL_HNDL *ZeroClampCtrl, BASE_MOT_STS *MotSts,
																	KSGAIN ZcKp, LONG JogSpeed );
static LONG OverTorqueCalculation( CTRL_CMD_MNG *CtrlCmdMng, CTRL_CMD_PRM *CtrlCmdPrm );
static void LpxSetNetSelMonitor( LONG **MonTbl, LONG SelMon,
															LONG *RspSelMon, LONG *RspMonData );
static void PosRefRenewal( POS_MNG_HNDL *PosManager, CTRLMD CtrlMode );

static void	BpxBinputNetIf( BASE_CTRL_HNDL*, SERVO_CONTROL_IF*, TRQ_LMT_DATA*, LONG ax_no );/* <S1C0>：軸追加 */
static void	BpxBoutputNetIf( SERVO_CONTROL_IF*, SEQ_CTRL_OUT*, LONG, LONG );

extern void HALdrv_SetSOut( int axis, UINT x );
/****************************************************************************************************/


/****************************************************************************************************/
/*																									*/
/*		スキャンＢ拡張指令入力処理																	*/
/*																									*/
/****************************************************************************************************/
static void LpxNetScanBInputExtCommand( TRQ_LMT_DATA *TrqLimits, USHORT ExtSel, EXTENDED_DATA ExtData )
{
	switch ( ExtSel )
	{
	case	EXTCMD_TRQLMT_P :	/* 正側トルク制限	*/
		TrqLimits->var.FwTrqLmt = ExtData.dwVal;
		break ;

	case	EXTCMD_TRQLMT_N :	/* 逆側トルク制限	*/
		TrqLimits->var.RvTrqLmt = ExtData.dwVal;
		break ;

	default					:	/* その他			*/
		break ;
	}
}

#if	(CSW_SVCF_FBLATCH == TRUE)
/****************************************************************************************************/
/*																									*/
/*		ラッチ要求取得処理																			*/
/*																									*/
/****************************************************************************************************/
static void LpxLatchRequestGet( SERVO_CONTROL_IF *SvControlIf, LONG CmdLtBit )
{
	LONG	CmdLtEdge;

	CmdLtEdge = SvControlIf->CmdLtBit ^ CmdLtBit;
/*--------------------------------------------------------------------------------------------------*/
/*		Ｃ相ラッチ要求検出処理(変化エッジ)															*/
/*--------------------------------------------------------------------------------------------------*/
	if( 0 != (CmdLtEdge & REQ_C_PHASE_BIT) )
	{/* 前回ラッチ要求≠今回ラッチ要求 */
		SvControlIf->LtReqCphs = TRUE;
	}
/*--------------------------------------------------------------------------------------------------*/
/*		Ext1ラッチ要求検出処理(変化エッジ)															*/
/*--------------------------------------------------------------------------------------------------*/
	if( 0 != (CmdLtEdge & REQ_EXT_SIG1_BIT) )
	{/* 前回ラッチ要求≠今回ラッチ要求 */
		SvControlIf->LtReqExt1 = SvControlIf->ExtLtEnable[0];
	}
/*--------------------------------------------------------------------------------------------------*/
/*		Ext2ラッチ要求検出処理(変化エッジ)															*/
/*--------------------------------------------------------------------------------------------------*/
	if( 0 != (CmdLtEdge & REQ_EXT_SIG2_BIT) )
	{/* 前回ラッチ要求≠今回ラッチ要求 */
		SvControlIf->LtReqExt2 = SvControlIf->ExtLtEnable[1];
	}
/*--------------------------------------------------------------------------------------------------*/
/*		Ext3ラッチ要求検出処理(変化エッジ)															*/
/*--------------------------------------------------------------------------------------------------*/
	if( 0 != (CmdLtEdge & REQ_EXT_SIG3_BIT) )
	{/* 前回ラッチ要求≠今回ラッチ要求 */
		SvControlIf->LtReqExt3 = SvControlIf->ExtLtEnable[2];
	}

/*--------------------------------------------------------------------------------------------------*/
/*		前回ラッチ要求の保存																		*/
/*--------------------------------------------------------------------------------------------------*/
	SvControlIf->CmdLtBit = CmdLtBit;
}
#endif

/****************************************************************************************************/
/*																									*/
/*		通信モニタデータ設定																		*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	概 略 :	モニタ選択(RspSelMon)、モニタデータ(RspMonData)をいっしょに更新する。					*/
/*			低速用モニタデータのため、NetIfCmdMonSel[]にモニタ選択を格納する。						*/
/*																									*/
/****************************************************************************************************/
static void LpxSetNetSelMonitor( LONG **MonTbl, LONG SelMon, LONG *RspSelMon, LONG *RspMonData )
{
/* <S1DE>:start */
//		*RspSelMon  = (UCHAR)SelMon&(0x4F);		 /* <S1D7>:要素数変更 */
//		*RspMonData = *(MonTbl[SelMon&(0x4F)]);	 /* <S1D7>:要素数変更 */

	if(SelMon < 0x50)
	{
		*RspSelMon  = (UCHAR)SelMon;
		*RspMonData = *(MonTbl[SelMon]);
	}
	else
	{
		*RspSelMon  = (UCHAR)SelMon;
		*RspMonData = 0;
	}
/* <S1DE>:end */}


/****************************************************************************************************/
/*																									*/
/*		スキャンＢネットワークＩＦ入力処理															*/
/*																									*/
/****************************************************************************************************/
static void BpxBinputNetIf( BASE_CTRL_HNDL *BaseControls, SERVO_CONTROL_IF *SvControlIf,
																		TRQ_LMT_DATA *TrqLimits, LONG ax_no )
{
	SV_CYC_CMD	*CyclicCMD;
	SV_CYC_CTRL	*CyclicCTRL;

	CyclicCMD	= &( SvControlIf->CmdOptionIf.CycCmd );
	CyclicCTRL	= &( SvControlIf->CmdOptionIf.CycCtrl );

/*--------------------------------------------------------------------------------------------------*/
/*		通信：指令データ受信チェック																*/
/*--------------------------------------------------------------------------------------------------*/
	if( CyclicCTRL->CmdUpdate == FALSE )
	{
		return;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		シーケンス信号入力																			*/
/*--------------------------------------------------------------------------------------------------*/
	/* ネットワークからのPCL/NCL信号入力 */
	TrqLimits->var.PclSignal = (SvControlIf->CmdSeqBit >> ENB_EXTPTRQLMT_BITNO) & 0x1;
	TrqLimits->var.NclSignal = (SvControlIf->CmdSeqBit >> ENB_EXTNTRQLMT_BITNO) & 0x1;

/*--------------------------------------------------------------------------------------------------*/
/*		通信：ビット信号入力																		*/
/*--------------------------------------------------------------------------------------------------*/
	SvControlIf->CmdCtrl	= CyclicCMD->CmdCtrl;
	SvControlIf->CmdSeqBit	= CyclicCMD->CmdSeqBit;

/*--------------------------------------------------------------------------------------------------*/
/*		通信：スキャンＢ拡張指令入力処理															*/
/*--------------------------------------------------------------------------------------------------*/
	LpxNetScanBInputExtCommand( TrqLimits, (USHORT)CyclicCTRL->ExtCmdSel1, CyclicCMD->ExtCmd1 );
	LpxNetScanBInputExtCommand( TrqLimits, (USHORT)CyclicCTRL->ExtCmdSel2, CyclicCMD->ExtCmd2 );

/*--------------------------------------------------------------------------------------------------*/
/*		通信：モニタ選択指令入力処理																*/
/*--------------------------------------------------------------------------------------------------*/
	SvControlIf->MonSel.MonSel1 = CyclicCMD->SelMon1 ;
	SvControlIf->MonSel.MonSel2 = CyclicCMD->SelMon2 ;
	SvControlIf->MonSel.MonSel3 = CyclicCMD->SelMon3 ;
	SvControlIf->MonSel.MonSel4 = CyclicCMD->SelMon4 ;
	SvControlIf->MonSel.MonSel5 = CyclicCMD->SelMon5 ;
	SvControlIf->MonSel.MonSel6 = CyclicCMD->SelMon6 ;
	SvControlIf->MonSel.MonSel7 = CyclicCMD->SelMon7 ;
	SvControlIf->MonSel.MonSel8 = CyclicCMD->SelMon8 ;

/*--------------------------------------------------------------------------------------------------*/
/*		通信：I/O出力指令																			*/
/*--------------------------------------------------------------------------------------------------*/
//#if (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE)						/* <S045> */
	//ulwk = (CyclicCMD->DOSignal) & 0x0000000F;
	//SvControlIf->NetSout = ( (((ulwk >> 0) & 1) << SOUT01)
	//				  	   | (((ulwk >> 1) & 1) << SOUT02)
	//				  	   | (((ulwk >> 2) & 1) << SOUT03) );
//#else																/* <S045> */
//	HALdrv_SetSOut ( 0, (CyclicCMD->DOSignal) & 0x0000000F );		/* 後で消す */
	HALdrv_SetSOut ( (int)ax_no, (CyclicCMD->DOSignal) & 0x0000007F );		/* 後で消す *//* <S04B> *//* <S1C0>:軸追加 */
//#endif															/* <S045> */

/*--------------------------------------------------------------------------------------------------*/
/*		通信：ゲイン切替用ゲイン番号																*/
/*--------------------------------------------------------------------------------------------------*/
	if(CyclicCMD->CmdCtrl.SelBank < SVCDEF_NETGSEL_SETNUM)
	{	BaseControls->GselNum = (LONG)CyclicCMD->CmdCtrl.SelBank;}	/* <S145> */

#if	(CSW_SVCF_FBLATCH == TRUE)
/*--------------------------------------------------------------------------------------------------*/
/*		通信：ラッチ選択ビット信号(Cphase,Ext1,Ext2,Ext3)											*/
/*--------------------------------------------------------------------------------------------------*/
	LpxLatchRequestGet( SvControlIf, CyclicCMD->CmdLtBit);
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		通信：制御モード＆サーボ制御指令入力処理													*/
/*--------------------------------------------------------------------------------------------------*/
	switch( CyclicCMD->CmdCtrl.ModSel )
	{
	case NO_CTRL_MODE:	/*--- 制御モード無効 ---------------------------------------------------*/
		SvControlIf->NetCtrlMcmd	= CTRLMODE_TRQ;
		SvControlIf->NetTrqRef		= 0;
		SvControlIf->NetSpdLmt		= 0x01000000;
		TrqLimits->var.FwTrqLmt	= NORMAXVALUE;
		TrqLimits->var.RvTrqLmt	= NORMAXVALUE;
		SvControlIf->NetSpdRef		= 0;
		SvControlIf->CmdSeqBit		&= ~(SERVO_ON_BIT);
		break;
	case POS_CTRL_MODE:	/*--- 位置制御モード ---------------------------------------------------*/
		SvControlIf->NetCtrlMcmd	= CTRLMODE_POS;
		SvControlIf->NetPosRef		= CyclicCMD->Position;
		SvControlIf->NetSpdFFC		= CyclicCMD->Speed;
		SvControlIf->NetTrqFFC		= CyclicCMD->Torque;
		SvControlIf->NetSpdRef		= CyclicCMD->Speed;
		break;
	case SPD_CTRL_MODE:	/*--- 速度制御モード ---------------------------------------------------*/
		/* 注）トルク制限は、拡張指令で設定される。 */
		SvControlIf->NetCtrlMcmd	= CTRLMODE_SPD;
		SvControlIf->NetSpdRef		= CyclicCMD->Speed;
		SvControlIf->NetTrqFFC		= CyclicCMD->Torque;
		break;
	case TRQ_CTRL_MODE:	/*--- トルク制御モード -------------------------------------------------*/
		SvControlIf->NetCtrlMcmd	= CTRLMODE_TRQ;
		SvControlIf->NetTrqRef		= CyclicCMD->Torque;
		SvControlIf->NetSpdLmt		= CyclicCMD->Speed;
		TrqLimits->var.FwTrqLmt	= NORMAXVALUE;
		TrqLimits->var.RvTrqLmt	= NORMAXVALUE;
		SvControlIf->NetSpdRef		= CyclicCMD->Speed;
		break;
	default:	/*--------------- デフォルト(無効)モード ---------------------------------------*/
		SvControlIf->NetCtrlMcmd	= CTRLMODE_TRQ;
		SvControlIf->NetTrqRef		= 0;
		SvControlIf->NetSpdLmt		= 0;
		TrqLimits->var.FwTrqLmt	= NORMAXVALUE;
		TrqLimits->var.RvTrqLmt	= NORMAXVALUE;
		SvControlIf->CmdSeqBit		&= ~(SERVO_ON_BIT);
		SvControlIf->NetSpdRef		= 0;
		break;
	}

}


/****************************************************************************************************/
/*																									*/
/*		スキャンＢネットワークＩＦ出力処理															*/
/*																									*/
/****************************************************************************************************/
static void BpxBoutputNetIf( SERVO_CONTROL_IF *SvControlIf, SEQ_CTRL_OUT *SeqCtrlOut,
																		LONG apos, LONG PosErr )
{
	SV_CYC_RSP			*SvCycRsp;
	LONG				**MonTbl;

	SvCycRsp	= &( SvControlIf->CmdOptionIf.CycRsp );

/*--------------------------------------------------------------------------------------------------*/
/*		通信：応答データ送信チェック																*/
/*--------------------------------------------------------------------------------------------------*/
	if( SvControlIf->CmdOptionIf.CycCtrl.RspUpdate == FALSE )
	{
		return;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		通信：制御モード応答																		*/
/*--------------------------------------------------------------------------------------------------*/
	SvCycRsp->RspCtrl.ExeMode = SvControlIf->CmdCtrl.ModSel;
	SvCycRsp->RspCtrl.RspCtrlBit =
		(USHORT)(SeqCtrlOut->RspCtrlBits | SvControlIf->RspCtrl.RspCtrlBit);

/*--------------------------------------------------------------------------------------------------*/
/*		通信：応答ビット信号出力																	*/
/*--------------------------------------------------------------------------------------------------*/
	SvCycRsp->RspSeqBit	= SeqCtrlOut->RspSeqBits;
	SvCycRsp->RspLtBit	= SvControlIf->RspLtBit;
//	SvCycRsp->RspSigBit	= SvControlIf->RspSigBit;
	SvCycRsp->RspSigBit	= SeqCtrlOut->RspSigBits;

/*--------------------------------------------------------------------------------------------------*/
/*		通信：応答数値データ出力																	*/
/*--------------------------------------------------------------------------------------------------*/
	SvCycRsp->FbPosition = apos;
	SvCycRsp->PosError = PosErr;

/*--------------------------------------------------------------------------------------------------*/
/*		通信：モニタデータ出力																		*/
/*--------------------------------------------------------------------------------------------------*/
	MonTbl = SvControlIf->MonRamTable;
	LpxSetNetSelMonitor( MonTbl, SvControlIf->MonSel.MonSel1,
							&SvCycRsp->SelMon1, &SvCycRsp->Mon1 );
	LpxSetNetSelMonitor( MonTbl, SvControlIf->MonSel.MonSel2,
							&SvCycRsp->SelMon2, &SvCycRsp->Mon2 );
	LpxSetNetSelMonitor( MonTbl, SvControlIf->MonSel.MonSel3,
							&SvCycRsp->SelMon3, &SvCycRsp->Mon3 );
	LpxSetNetSelMonitor( MonTbl, SvControlIf->MonSel.MonSel4,
							&SvCycRsp->SelMon4, &SvCycRsp->Mon4 );
	LpxSetNetSelMonitor( MonTbl, SvControlIf->MonSel.MonSel5,
							&SvCycRsp->SelMon5, &SvCycRsp->Mon5 );
	LpxSetNetSelMonitor( MonTbl, SvControlIf->MonSel.MonSel6,
							&SvCycRsp->SelMon6, &SvCycRsp->Mon6 );
	LpxSetNetSelMonitor( MonTbl, SvControlIf->MonSel.MonSel7,
							&SvCycRsp->SelMon7, &SvCycRsp->Mon7 );
	LpxSetNetSelMonitor( MonTbl, SvControlIf->MonSel.MonSel8,
							&SvCycRsp->SelMon8, &SvCycRsp->Mon8 );

}



/****************************************************************************************************/

/****************************************************************************************************/
/*																									*/
/*		Kernel Input Service for ScanB																*/
/*																									*/
/****************************************************************************************************//* <S04D>:引数整理 *//* <S086>*/
void BpiInputMotorPosition(BASE_CTRL_HNDL *BaseControls, ASIC_U_IF_STR *pAsicMicroReg,
											BPRMDAT *Bprm, PRMDATA *Prm, CTRL_LOOP_OUT *CtrlLoopOut)
{
	LONG				dMotPos, MotPos;
	LONG			dFencPos, FencPos;	/* <S04D> */
	MENCV 			*MencV, *FencV;		/* <S04D> */
	POS_MNG_HNDL		*PosManager;

	PosManager		= &(BaseControls->PosManager);
	MencV	= BaseControls->MencV;	/* <S04D> */
	FencV	= BaseControls->FencV;	/* <S04D> */
/*--------------------------------------------------------------------------------------------------*/
/*		Calculate Motor Encoder Data for Servo Control												*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->AxisMotType == MOTTYPE_LINEAR )
	{ /* リニア型 */
		BaseControls->MotSts.MotCphPass = LmxSencScanServiceB( MencV,
							 	 	 	 	 	 	 	 	&(BaseControls->CLatchClrReq) );
	}
	else
	{ /* 回転型 */
		BaseControls->MotSts.MotCphPass = RmxSencScanServiceB( MencV,
															&(BaseControls->CLatchClrReq) );
	}
	BaseControls->MotSts.MposFromCpos = Bprm->DirSign * MencV->MposFromCpos;

/*--------------------------------------------------------------------------------------------------*/
/*		ＦＢ位置更新																				*/
/*--------------------------------------------------------------------------------------------------*/
//	MotPos = MencV->MotPos;									/* Load MotorPosition					*//* <S038> */
	MotPos = Bprm->DirSign * MencV->MotPos;					/* Load MotorPosition					*/
	dMotPos = MotPos - BaseControls->MotSts.MotPos;			/* Cal. Delta Position					*/
	BaseControls->MotSts.MotPos += dMotPos;					/* Update Motor Position				*/
	BaseControls->MotSts.dMotPos = dMotPos;					/* Update Delta Motor Position			*/

/*--------------------------------------------------------------------------------------------------*/
/*		<S0E5> ＦＢ速度更新 (シーケンス処理用)														*/
/*--------------------------------------------------------------------------------------------------*/
	BaseControls->MotSts.MotSpdForSeq = MlibMulgain( dMotPos, Bprm->KmotspdB );

/*--------------------------------------------------------------------------------------------------*/
/*		位置ＦＢ差分入力(Semi/Full切替え), モータ絶対位置更新, フルク絶対位置更新					*/
/*--------------------------------------------------------------------------------------------------*/
#if	(CSW_SVCF_FULC_MODE == CSW_FULC_BASIC_SUPPORT)/* <S04D> */
	if(Bprm->FencUse != FALSE)
	{
		/*------------------------------------------------------------------------------------------*/
		/*		ＦＢ位置更新																		*/
		/*------------------------------------------------------------------------------------------*/
		BaseControls->MotSts.FencCphPass = FencV->MotCphPass;	/* Set FencCphPass					*//* <S220> */
		FencPos = Bprm->DirSign * FencV->MotPos;								/* Load MotorPosition					*/
		dFencPos = FencPos - BaseControls->MotSts.FencPos;		/* Cal. Delta Position					*/
		BaseControls->MotSts.FencPos += dFencPos;				/* Update Motor Position				*/
		BaseControls->MotSts.dFencPos = dFencPos;				/* Update Delta Motor Position			*/

		MlibAposRg64dp( dFencPos, &(PosManager->conf.Egear), &(PosManager->FencPos), NULL );
#if (CSW_SVCF_BACKLASHCOMP == TRUE)
			MlibAposRg64dp( (dFencPos - PosManager->BlshCmp.var.BlshCmpAmountPls),		/* <S18B> */
							&(PosManager->conf.Egear),
							&(PosManager->PgPos),
							NULL );
#else
		PosManager->PgPos = PosManager->FencPos;										/* [指令単位]	*/
#endif/* (CSW_SVCF_BACKLASHCOMP == TRUE) */
		BaseControls->CtrlCmdMngr.dPosFbki = dFencPos;
		BaseControls->CtrlCmdMngr.PosFbki = BaseControls->MotSts.FencPos;
		BaseControls->CtrlCmdMngr.FbkPosFromCpos = BaseControls->MotSts.FposFromCpos;
	}
	else
#endif	/* (CSW_SVCF_FULC_MODE == CSW_FULC_BASIC_SUPPORT) */
	{
		if( MencV->ReCalcAbsPosReq != FALSE )			/* 絶対位置再計算要求あり？				*/
		{
			/* エンコーダ初期位置取得	*/
			/* RxPos*[0] -> RmxReadSencPositionで取得ずみ */
			/* SEN ON時の絶対位置再計算処理	*/
			PosMngSenOnReCalcAbsoPos( MencV,
			                          &(PosManager->MencPos),
			                          &(PosManager->conf.Egear),
			                          Prm->MencP.bit_dp,
			                          Prm->MencP.limmlt,
									  Bprm->RvsDir);
			MencV->ReCalcAbsPosReq = FALSE;
			PosManager->PgPos = PosManager->MencPos;								/* [指令単位]	*/
		}
		else
		{
			MlibAposRg64dp( dMotPos, &(PosManager->conf.Egear), &(PosManager->MencPos), NULL );
#if (CSW_SVCF_BACKLASHCOMP == TRUE)
			MlibAposRg64dp( (dMotPos - PosManager->BlshCmp.var.BlshCmpAmountPls),	/* <S18B> */
							&(PosManager->conf.Egear),
							&(PosManager->PgPos),
							NULL );
#else
			PosManager->PgPos = PosManager->MencPos;								/* [指令単位]	*/
#endif/* (CSW_SVCF_BACKLASHCOMP == TRUE) */
		}
		BaseControls->CtrlCmdMngr.dPosFbki = dMotPos;
		BaseControls->CtrlCmdMngr.PosFbki = BaseControls->MotSts.MotPos;
//		pBaseCtrl->CtrlCmdMngr.PosErr32s = CtrlLoopOut->MotPosErr;
		BaseControls->CtrlCmdMngr.FbkPosFromCpos = BaseControls->MotSts.MposFromCpos;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		モータ速度ＦＢ演算																			*/
/*--------------------------------------------------------------------------------------------------*/
#if	(CSW_SVCF_FULC_MODE == CSW_FULC_BASIC_SUPPORT)/* <S04D>:ﾌﾙｸ処理追加 */
	if(Bprm->PsFulcSelect != FALSE)
	{
		BaseControls->CtrlCmdMngr.SpdFbki = FencV->MotSpd;
		BaseControls->CtrlCmdMngr.SpdObsFbki = FencV->MotSpd;
	}
	else
#endif/* (CSW_SVCF_BACKLASHCOMP == TRUE) */
	{
//		BaseControls->CtrlCmdMngr.SpdFbki = MencV->MotSpd;
//		BaseControls->CtrlCmdMngr.SpdObsFbki = MencV->MotSpd;
		BaseControls->CtrlCmdMngr.SpdFbki = CtrlLoopOut->MotSpd_a;
		BaseControls->CtrlCmdMngr.SpdObsFbki = CtrlLoopOut->MotSpd_a;/*<S086>*/
	}

}

/****************************************************************************************************/
/*																									*/
/*		Output Network monitor value																*/
/*																									*/
/****************************************************************************************************/
void BpiOutputNetStatus(BASE_CTRL_HNDL *BaseControls, SERVO_CONTROL_IF *SvControlIf,
                        	CTRL_LOOP_OUT *CtrlLoopOut, SEQ_CTRL_OUT *SeqCtrlOut, BPRMDAT *Bprm, PLSIFV *pPls)/* <S04D> *//* <S121> */
{
//	LONG				dPcmda;
	LONG				MotSpd;
	LONG				VPosErrKp;
	USHORT				RspCtrlBits;
	POS_MNG_HNDL		*PosManager;

	PosManager		= &(BaseControls->PosManager);

	/* 仮想位置偏差補償用FB速度算出 */
	if(Bprm->PsFulcSelect != FALSE)/* <S04D>:ﾌﾙｸ処理追加 */
	{/* フルクローズド制御	*/
#if (FLOAT_USE==TRUE)
		MotSpd = (BaseControls->MotSts.FencSpd + BaseControls->CtrlCmdMngr.SpdFbki) / 2.0f;
#else
		MotSpd = (BaseControls->MotSts.FencSpd + BaseControls->CtrlCmdMngr.SpdFbki) >> 1;
#endif
		BaseControls->MotSts.FencSpd = BaseControls->CtrlCmdMngr.SpdFbki;
		BaseControls->MotSts.MotSpd = BaseControls->MencV->MotSpd;
	}
	else
	{/* セミクローズド制御	*/
#if (FLOAT_USE==TRUE)
	MotSpd = (BaseControls->MotSts.MotSpd + BaseControls->CtrlCmdMngr.SpdFbki) / 2.0f;
#else
	MotSpd = (BaseControls->MotSts.MotSpd + BaseControls->CtrlCmdMngr.SpdFbki) >> 1;
#endif
	BaseControls->MotSts.MotSpd = BaseControls->CtrlCmdMngr.SpdFbki;
	}
//<S0E5>	BaseControls->MotSts.MotSpdSumB += BaseControls->MotSts.MotSpd;
	BaseControls->MotSts.MotSpdSumB += BaseControls->MotSts.MotSpdForSeq;	/* <S0E5> */
	BaseControls->MotSts.MotSpdCntB++;

/*--------------------------------------------------------------------------------------------------*/
/*		仮想位置偏差更新																	<S14C>	*/
/*--------------------------------------------------------------------------------------------------*/
	if(BaseControls->CtrlModeSet.CtrlMode.b.cm != CTRLMODE_POS)
	{ /* 速度/トルク制御時 */
		/* 仮想位置ループゲイン取得 */
		VPosErrKp = 1 ^ (SeqCtrlOut->OverTrvlSts & SeqCtrlOut->MotStop);
		VPosErrKp *= (BaseControls->GainChange.GselGains[0].VirtualPosKp);

		/* 仮想位置偏差算出  */
#if (FLOAT_USE==TRUE)
		PosManager->VirtualPositionError = MotSpd * VPosErrKp;
#else
		PosManager->VirtualPositionError = MlibMulgain32( MotSpd, VPosErrKp );
#endif /* FLOAT_USE */
	}

/*--------------------------------------------------------------------------------------------------*/
/*		ステータス更新																				*/
/*--------------------------------------------------------------------------------------------------*/
	/*	電流制限フラグ : ベースイネーブル && μプログラム電流制限ステータス	*/
//	RspCtrlBits = (SeqCtrlOut->BeComplete & CtrlLoopOut->TrqClamp) << TRQ_CLAMP_BITNO; /* <S04B> */
//	BaseControls->CtrlCmdMngr.TrqClamp = (SeqCtrlOut->BeComplete & CtrlLoopOut->TrqClamp);	/* <S045> Torque Limit *//* <S04B> */
	RspCtrlBits = BaseControls->CtrlCmdMngr.TrqClamp << TRQ_CLAMP_BITNO; /* <S04B> */
	/* Speed Clamp Status */
	//RspCtrlBits |= ( 0 != CtrlLoopOut->SpdClamp ) << SPD_CLAMP_BITNO;
	RspCtrlBits |= ( 0 != BaseControls->CtrlCmdMngr.SpdClamp ) << SPD_CLAMP_BITNO;/* @@ VLT status */
/*--------------------------------------------------------------------------------------------------*/
	RspCtrlBits |= ( 0 != BaseControls->PosCtrlSts.CoinSignal ) << COIN_BITNO;	/* @@ COIN status */
/*--------------------------------------------------------------------------------------------------*/
	RspCtrlBits |= ( 0 != BaseControls->PerClrEdge ) << PERCLREDGE_BITNO;		/* @@ CLREDGE status */

	/* @@ CLREDGE status (AP ONLY) */
/*--------------------------------------------------------------------------------------------------*/
	SvControlIf->RspCtrl.RspCtrlBit = RspCtrlBits;

#if !defined(CSW_NETIF_SYNQNET_TYPE)

#if	(CSW_SVCF_FBLATCH == TRUE)
/*--------------------------------------------------------------------------------------------------*/
/*		位置ＦＢラッチ処理																			*/
/*--------------------------------------------------------------------------------------------------*/
	/* Cphase, Ext1 Latch */
	PosMngLatchNetFbkPos( BaseControls, SvControlIf, Bprm );									/* <S04D> */
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		ネットワークモニタデータ更新																*/
/*--------------------------------------------------------------------------------------------------*/
	BpxBoutputNetIf( SvControlIf,
					 SeqCtrlOut,
					 PosManager->PgPos.apos[0],
					 PosManager->var.PosErrA.per32s );
//					 pBaseCtrl->CtrlCmdMngr.PosErr32s );
#endif
}


/****************************************************************************************************/
/*																									*/
/*		Input ScanB Control command																	*/
/*																									*/
/****************************************************************************************************/
void BpiInputControlCMD( BASE_CTRL_HNDL *BaseControls, SERVO_CONTROL_IF *SvControlIf,
                         CTRL_LOOP_OUT *CtrlLoopOut, SEQ_CTRL_OUT *SeqCtrlOut, USHORT uwd_l, LONG ax_no )/* <S1C0>：軸追加 */
{
	LONG				GainChgTriger;
	TBLCHK_SV_STS		TblDriveChkSts;
	CTRLMD				CtrlMode;
//	POS_MNG_HNDL		*PosManager;	//<S0C7>
//	LONG				lwk0, lwk1;		//<S0C7>

//	PosManager			= &(BaseControls->PosManager);//<S0C7>


/*--------------------------------------------------------------------------------------------------*/
/*		電流制限状態取得																			*/
/*--------------------------------------------------------------------------------------------------*/
	/*	電流制限フラグ : ベースイネーブル && μプログラム電流制限ステータス	*/
	BaseControls->CtrlCmdMngr.TrqClamp = (SeqCtrlOut->BeComplete & CtrlLoopOut->TrqClamp);			  /* <S04B> */

#ifndef _IO_SERVO_ON_
/*--------------------------------------------------------------------------------------------------*/
/*		ネットワーク指令データ入力																	*/
/*--------------------------------------------------------------------------------------------------*/
#if !defined(CSW_NETIF_SYNQNET_TYPE)
	BpxBinputNetIf( BaseControls, SvControlIf, &(BaseControls->TrqLimitData), ax_no );/* <S1C0>：軸追加 */
#endif
#endif /* _IO_SERVO_ON_ */

	/* 位置積分クリア指令取得 */
	BaseControls->CtrlCmdMngr.PicvClrCmd =
			( SvControlIf->CmdCtrl.CmdCtrlBit >> CLRPOSINTG_BITNO ) & 1;

	/* モードスイッチ用加速度データ更新 */
	BaseControls->ModeSwData.var.Acceleration = SeqCtrlOut->ModeSwAcc;

/*--------------------------------------------------------------------------------------------------*/
/*		サーボ制御モード管理処理																	*/
/*--------------------------------------------------------------------------------------------------*/
	/* 制御モード：スキャンＣ指令 */
	CtrlMode.dw = SeqCtrlOut->CtrlModeReq.dw;

	/* スキャンＣ制御モード指令チェック */
	if( SeqCtrlOut->CtrlModeReq.us.uwd_l == CTRLMODE_NOCMD )
	{
		CtrlMode.us.uwd_l = (USHORT)SvControlIf->NetCtrlMcmd;
	}
	else
	{
		SvControlIf->CmdReady = FALSE;
	}
	/*----------------------------------------------------------------------------------------------*/
	/* 制御モード前回値 */
	BaseControls->CtrlModeSet.CtrlModeLst.dw = BaseControls->CtrlModeSet.CtrlMode.dw;
	/* 制御モード変更情報 */
	BaseControls->CtrlModeSet.CtrlModeChg.dw = CtrlMode.dw ^ BaseControls->CtrlModeSet.CtrlMode.dw;
	/* 制御モード今回値 */
	BaseControls->CtrlModeSet.CtrlMode.dw	 = CtrlMode.dw;

/*--------------------------------------------------------------------------------------------------*/
/*		Get Sequence Control Output																	*/
/*--------------------------------------------------------------------------------------------------*/
	BaseControls->BBRefClrReq = SeqCtrlOut->BBSvcRefClrReq;
	BaseControls->BeComplete = SeqCtrlOut->BeComplete;
	BaseControls->OverTravel = SeqCtrlOut->OverTrvlSts;			/* <S18B> */

#ifdef _VC_DEBUG_ /* 2012.09.11 Y.Oka ★強制サーボオン 暫定対応★ */
	BaseControls->BeComplete = TRUE;
#endif /* 2012.09.11 Y.Oka ★強制サーボオン 暫定対応★ */
	

/*--------------------------------------------------------------------------------------------------*/
/*		サーボゲイン切替処理																		*/
/*--------------------------------------------------------------------------------------------------*/
	/* サーボゲイン切替処理 */
	if( (BaseControls->GainChange.AtGsel == TRUE)
		&& (BaseControls->TuneLessCtrl.var.TuneLessAct == FALSE) 
		&& (SVFSEL_ATGAINCHANGE_USE == 1) )							/* <S09B> */
	{ /* 自動ゲイン切替 && 調整レス無効時 */
		/* 切り替え判定処理 */
		GainChgTriger = GselDetGainChngTiming( BaseControls->PosCtrlSts.CoinSignal,
											   BaseControls->PosCtrlSts.NearSignal,
											   BaseControls->PosCtrlSts.RefZSignal,
											   BaseControls->GainChange.AtGselTrgA );
		/* ゲイン切り替え演算処理 */
//<S09B>	GselAutoGainChange( &(BaseControls->GainChange),
//							&(BaseControls->MFControl),
//							GainChgTriger,
//							&(BaseControls->CtrlCmdPrm) );
			GselAutoGainChange( BaseControls, GainChgTriger );	/* <S09B> */
	}
	else
	{ /* マニュアルゲイン切り替え(ゲイン切り替えなし) */
		/* マニュアルゲイン切り替え処理 */
//<S09B>		GselManualGainChange( &(BaseControls->GainChange),
//							  &(BaseControls->MFControl),
//							  BaseControls->GselNum,
//							  BaseControls->TuneLessCtrl.var.TuneLessAct,
//							  &(BaseControls->CtrlCmdPrm) );
			GselManualGainChange( BaseControls );					/* <S09B> */
		/* 自動ゲイン切り替え初期化 */
//<S09B>	GselInitAutoGainChange(
//							&(BaseControls->GainChange.AtGselA),
//							&(BaseControls->GainChange.AtGselB) );
			GselRstAutoGainChange( &(BaseControls->GainChange) );	/* <S09B> */
	}

/*--------------------------------------------------------------------------------------------------*/
/*		テーブル運転実行処理																		*/
/*--------------------------------------------------------------------------------------------------*/
//	TblDriveChkSts.AlmFlag = AxisRscB->AlmManager->AlmFlag;
	TblDriveChkSts.AlmFlag = (SeqCtrlOut->RspSeqBits >> ALM_DETECTED_BITNO) & 0x1;
	TblDriveChkSts.BeComplete = BaseControls->BeComplete;
	TblDriveChkSts.HwbbSts = SeqCtrlOut->HwbbSts;
	TblDriveChkSts.MainPowerOn = SeqCtrlOut->MainPowerOn;
	TblDriveChkSts.OverTrvlSts = SeqCtrlOut->OverTrvlSts;
//	TblDriveChkSts.TrqClamp = CtrlLoopOut->TrqClamp; /* <S04B> */
	TblDriveChkSts.TrqClamp = BaseControls->CtrlCmdMngr.TrqClamp; /* <S04B> */
	TblDriveChkSts.dMotPos = BaseControls->MotSts.dMotPos;
	TblDriveExecute( &(BaseControls->TblDrive),
					 &TblDriveChkSts,
					 &(BaseControls->PosManager),
					 &(BaseControls->MFControl) );

/*--------------------------------------------------------------------------------------------------*/
/*		ゼロ制御モード指令処理：スキャンB共通入力処理からの指令										*/
/*--------------------------------------------------------------------------------------------------*/
	if( (BaseControls->TblDrive.var.TblCmd == TBLCMD_NOCMD)
		|| (BaseControls->CtrlModeSet.CtrlMode.b.zm != CTRLMODE_ZNONE) )
	{ /* テーブル運転なし || スキャンB共通入力処理指令あり */
		BaseControls->CtrlModeSet.ZctrlMode.zm    = BaseControls->CtrlModeSet.CtrlMode.b.zm;
		BaseControls->CtrlModeSet.ZctrlMode.zmchg = BaseControls->CtrlModeSet.CtrlModeChg.b.zm;
	}
	else
	{
		BaseControls->CtrlModeSet.ZctrlMode.zm    = BaseControls->TblDrive.var.TblZmd.b.zm;
		BaseControls->CtrlModeSet.ZctrlMode.zmchg = BaseControls->TblDrive.var.TblZmd.b.zmchg;
	}

	/* 拡張制御出力変数ゼロクリア */
	BaseControls->CtrlCmdMngr.SpdFFCx = 0;		/* 拡張制御：速度FF補償				*/
	BaseControls->CtrlCmdMngr.SpdFBCx = 0;		/* 拡張制御：速度FB補償				*/
	BaseControls->CtrlCmdMngr.TrqFFCx = 0;		/* 拡張制御：トルクFF補償			*/
	BaseControls->CtrlCmdMngr.TrqFBCx = 0;		/* 拡張制御：トルクFB補償			*/

}



/****************************************************************************************************/
/*																									*/
/*		安川サーボ制御処理メイン																	*/
/*																									*/
/****************************************************************************************************/
void BpiYecServoControl( BASE_LOOPCTRLS *BaseLoops, SERVO_CONTROL_IF *SvControlIf,
							CTRL_LOOP_OUT *CtrlLoopOut, FUN_CMN_CONTROL *FnCmnCtrl, BPRMDAT *Bprm )/* <S04D> */
{
	BASE_CTRL_HNDL		*BaseControls;
	POS_MNG_HNDL		*PosManager;
	MFCTRL				*MFControl;
	LONG				SpdFFx;
	KSGAIN				lwk;		/*<S00A>*/
	CMD_CTRLBITS		*CmdCtrl;			/* <S1D0> */

	/* ローカル変数の初期化 */
	BaseControls= BaseLoops->BaseCtrls;
	PosManager	= &(BaseControls->PosManager);
	MFControl	= &(BaseControls->MFControl);
	CmdCtrl = &(SvControlIf->CmdOptionIf.CycCmd.CmdCtrl);		/* <S1D0> */

/*--------------------------------------------------------------------------------------------------*/
/*		モータ速度ＦＢ演算																			*/
/*--------------------------------------------------------------------------------------------------*/
//	pBaseCtrl->CtrlCmdMngr.SpdFbki		= pBaseCtrl->MotSts.MotSpd;
//	pBaseCtrl->CtrlCmdMngr.SpdObsFbki	= pBaseCtrl->MotSts.MotSpd;

/*--------------------------------------------------------------------------------------------------*/
/*		位置指令管理																				*/
/*																									*/
/*		サーボ基本制御はScanAで処理する。															*/
/*		ここで更新された位置指令はScanAで補間した後、サーボ基本制御の入力となる。					*/
/*		位置指令の更新は125usの前半62.5us以内で必ず行うこと。										*/
/*		詳細は別途仕様書を参照する。仕様書Ｎｏ：○○○○○○○○○○								*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/

	/* トルク指令フィルタ積分初期化要求クリア */
	BaseControls->CtrlCmdMngr.TrqFilClrCmd = FALSE;

/*--------------------------------------------------------------------------------------------------*/
/*		制御モードによる分岐																		*/
/*--------------------------------------------------------------------------------------------------*/
	switch( BaseControls->CtrlModeSet.CtrlMode.b.cm )
	{
/****************************************************************************************************/
	case CTRLMODE_POS:	/* 位置制御モード															*/
/****************************************************************************************************/
	/*----------------------------------------------------------------------------------------------*/
	/*		制御切り替え時処理(速度制御 または トルク制御 ===> 位置制御)							*/
	/*----------------------------------------------------------------------------------------------*/
		switch(BaseControls->CtrlModeSet.CtrlModeLst.b.cm)
		{
		case CTRLMODE_TRQ:
			if( BaseControls->DobsCtrl.var.DobsAct != FALSE)
			{ /* 外乱オブザーバ有効 -> トルク指令フィルタ積分初期化 */
				BaseControls->CtrlCmdMngr.TrqFilClrCmd = TRUE;
			}

			/* 調整レス要求設定 */
			if( BaseControls->TuneLessCtrl.var.TuneLessInvldReq == FALSE )
			{
				BaseControls->TuneLessCtrl.var.TuneLessAct = BaseControls->TuneLessCtrl.conf.TuningLessUse;
			}
			/* 速度指令/速度FF位相進み補償フィルタ初期化はμプログラムにて実施 */
			/* breakなし、そのまま下へ */

		case CTRLMODE_SPD:		/* 位置偏差関連の見直しのため変更 <S14C> */
			/* 速度指令フィルタ出力初期化 */
			BaseControls->CtrlCmdMngr.SpdRfFilo = 0;

			/* 制御モード切り替えによる基準位置作成 */
			lwk = MlibPcmdEgear( PosManager->VirtualPositionError,
								 &(PosManager->conf.Egear),
								 &(PosManager->var.dPcmdEgrem) );
#if 0/* SigamV-MD Microに基準位置を譲渡する	*/								/* <S007> */
			BaseControls->CtrlCmdMngr.BaseFbkPos = lwk + BaseControls->CtrlCmdMngr.PosFbki;
#else/* Sigam7 ScanAに初回位置偏差を譲渡する。	*/							/* <S007> */
			PosManager->var.Per64[0] = 0;
			PosManager->var.Per64[1] = 0;
			MlibPerrcalx(lwk, 0, PosManager->var.Per64);
#endif
			MlibPerrcala( PosManager->VirtualPositionError, 0,
							&PosManager->conf.Egear, &PosManager->var.PosErrA );
			/* 現在指令位置初期化 */
			BaseControls->CtrlCmdMngr.PosRefLo = PosManager->VirtualPositionError + PosManager->PgPos.apos[0];

			/* 自動ノッチ周波数解析強制無効処理追加予定 */
			break;

		default:
			break;
		}

	/*----------------------------------------------------------------------------------------------*/
	/*		位置指令生成																			*/
	/*----------------------------------------------------------------------------------------------*/
		/* サーボ通信位置指令管理処理 */
		PosManager->dPosRefi = PosMngNetPosCmdManager( BaseLoops, SvControlIf, FnCmnCtrl, Bprm );  /* <S04D> *//* <S187> */

	/*----------------------------------------------------------------------------------------------*/
	/*		速度FF補償/トルクFF補償生成																*/
	/*----------------------------------------------------------------------------------------------*/
		/* 位置指令FF演算 --> pBaseCtrl->CtrlCmdMngr.SpdFFCx */
		if( MFControl->var.CurMFCModel == 0 )
		{ /* MFC無効 -> MFC有効時はFF機能を無効にする */
			/* 位置指令FF演算 */
#if (FLOAT_USE==TRUE)
			SpdFFx = (float)PosManager->dPosRefi * BaseControls->CtrlCmdPrm.KdPff;
#else
			SpdFFx = MlibMulgain( PosManager->dPosRefi, BaseControls->CtrlCmdPrm.KdPff );
#endif /* FLOAT_USE */

#if (FLOAT_USE==TRUE)
			/* 一次ローパスフィルタ演算 */
			BaseControls->CtrlCmdMngr.dPFfFilo = FlibLpfilter1(SpdFFx,
														BaseControls->CtrlCmdPrm.KdPffFil,
														BaseControls->CtrlCmdMngr.dPFfFilo );
#else
			/* 一次ローパスフィルタ演算 */
			BaseControls->CtrlCmdMngr.dPFfFilo = MlibLpfilter1(SpdFFx,
														BaseControls->CtrlCmdPrm.KdPffFil,
														BaseControls->CtrlCmdMngr.dPFfFilo );
#endif /* FLOAT_USE */

			/* 速度FF補償 [2^24/OvrSpd] */
			BaseControls->CtrlCmdMngr.SpdFFCx += BaseControls->CtrlCmdMngr.dPFfFilo;
		}
		if( (BaseControls->TblDrive.var.TblCmd == TBLCMD_NOCMD)
			&& ((MFControl->var.CurMFCModel == 0) || (MFControl->conf.MFCFF == TRUE)) )
		{ /* テーブル運転以外 && (MFC無効 || MFC中外部FF有効) */
			/* 速度フィードフォワード指令移動平均フィルタ処理メイン関数		*/			/* <S1D0> */
			SvControlIf->NetSpdFFC = FFcmdFilMoveAverageFilter( &(BaseControls->VFFcmdFil),
																SvControlIf->NetSpdFFC,
																CmdCtrl->vffFilSts);
			BaseControls->CtrlCmdMngr.SpdRfFilo = SvControlIf->NetSpdFFC;
			/* 速度FF指令加算 */
			BaseControls->CtrlCmdMngr.SpdFFCx += BaseControls->CtrlCmdMngr.SpdRfFilo;
			/* 	トルクフィードフォワード指令移動平均フィルタ処理メイン関数		*/			/* <S1D0> */
			SvControlIf->NetTrqFFC = FFcmdFilMoveAverageFilter( &(BaseControls->TFFcmdFil),
																SvControlIf->NetTrqFFC,
																CmdCtrl->tffFilSts);
			/* トルクFF指令加算 */
			BaseControls->CtrlCmdMngr.TrqFFCx += SvControlIf->NetTrqFFC;
		}

	/*----------------------------------------------------------------------------------------------*/
	/*		Get Speed Reference for Monitor															*/
	/*----------------------------------------------------------------------------------------------*/
		BaseControls->CtrlCmdMngr.SpdRefi = CtrlLoopOut->SpdRefo;
		BaseControls->CtrlCmdMngr.SpdRefo = CtrlLoopOut->SpdRefo;

	/*------------------------------------------------------------------------------------------*/
	/*		オフラインイナーシャ同定															*/
	/*------------------------------------------------------------------------------------------*/
		BpxJatOfflineCalc(	&BaseControls->JatHdl, 
							&BaseControls->PosManager.conf.Egear,
							&BaseControls->CtrlCmdPrm,
							&BaseControls->CtrlCmdMngr,
							BaseLoops->TrqRefo_a );									/* <S160> */
//<S0BA>
#if 0
	/*----------------------------------------------------------------------------------------------*/
	/*		モードスイッチ演算																		*/
	/*----------------------------------------------------------------------------------------------*/
		ModeSwitch( &(BaseControls->ModeSwData),
					CtrlLoopOut->TrqRefo,
					BaseControls->CtrlCmdMngr.SpdRefo,
//					(CtrlLoopOut->SpdRefo + CtrlLoopOut->SpdFFC),
					BaseControls->PosManager.PosErrA.per32s );
#endif
	/*----------------------------------------------------------------------------------------------*/
	/*		Get Speed Reference for Monitor															*/
	/*----------------------------------------------------------------------------------------------*/
		BaseControls->CtrlCmdMngr.SpdRefo = CtrlLoopOut->SpdRefo;
/* <S1F5> Start */
	/*----------------------------------------------------------------------------------------------*/
	/*		直線減速停止用速度値保存																*/
	/*----------------------------------------------------------------------------------------------*/
		if( BaseControls->CtrlModeSet.ZctrlMode.zm != CTRLMODE_LDSTOP )
		{
			BaseControls->CtrlCmdMngr.SpdLdstpRefo = BaseControls->CtrlCmdMngr.SpdRefo;
		}
/* <S1F5> End */

		break;


/****************************************************************************************************/
	case CTRLMODE_SPD:	/* 速度制御モード															*/
/****************************************************************************************************/
	/*----------------------------------------------------------------------------------------------*/
	/*		制御切り替え時処理(位置制御 または トルク制御 ===> 速度制御)							*/
	/*----------------------------------------------------------------------------------------------*/
		switch(BaseControls->CtrlModeSet.CtrlModeLst.b.cm)
		{
		case CTRLMODE_TRQ:
			if( BaseControls->DobsCtrl.var.DobsAct != FALSE)
			{ /* 外乱オブザーバ有効 -> トルク指令フィルタ積分初期化 */
				BaseControls->CtrlCmdMngr.TrqFilClrCmd = TRUE;
			}

			/* 調整レス要求設定 */
			if( BaseControls->TuneLessCtrl.var.TuneLessInvldReq == FALSE )
			{
				BaseControls->TuneLessCtrl.var.TuneLessAct = BaseControls->TuneLessCtrl.conf.TuningLessUse;
			}

			/* 制御モード切り替え時のショック低減処理 */
			lwk = SvControlIf->NetSpdRef - BaseControls->CtrlCmdMngr.SpdObsFbki;
#if (FLOAT_USE==TRUE)
			lwk = FlibIntegral( lwk,
								BaseControls->GainChange.GselGains[0].Kvi,
								BaseControls->CtrlCmdMngr.VelIvar64 );
#else
			lwk = MlibIntegral( lwk,
								BaseControls->GainChange.GselGains[0].Kvi,
								BaseControls->CtrlCmdMngr.VelIvar64 );
#endif /* FLOAT_USE */

			/* 速度指令/速度FF位相進み補償フィルタ初期化はμプログラムにて実施 */
			/* breakなし、そのまま下へ */

		case CTRLMODE_POS:
			/* 速度指令filter出力 = 速度FB, 速度指令SFS出力 = 速度FB */
			BaseControls->CtrlCmdMngr.SpdRfFilo = BaseControls->CtrlCmdMngr.SpdFbki;
			BaseControls->CtrlCmdMngr.SpdRefo = BaseControls->CtrlCmdMngr.SpdFbki;
			/* 自動ノッチ周波数解析強制無効処理追加予定 */
			break;

		default:
			break;
		}

	/*----------------------------------------------------------------------------------------------*/
	/*		速度指令生成																			*/
	/*----------------------------------------------------------------------------------------------*/
		if( BaseControls->TblDrive.var.TblCmd != TBLCMD_NOCMD )
		{ /* テーブル運転時 */
			/* テーブル運転速度指令設定 */
			BaseControls->CtrlCmdMngr.SpdRfFilo = 0;
			BaseControls->CtrlCmdMngr.SpdRefi = BaseControls->TblDrive.var.SpdRef;
			BaseControls->VrefInSts = FALSE; /* <S0A1> */
		}
		else
		{ /* 通常運転時 */
			/* 指令入力状態設定 */
			BaseControls->VrefInSts = (SvControlIf->NetSpdRef != 0) ? TRUE : FALSE; /* <S0A1> */

#if (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE) /* ★暫定 共通処理とすること★ */
			/* 一次ローパスフィルタ演算 */
			BaseControls->CtrlCmdMngr.SpdRfFilo = MlibLpfilter1(
													SvControlIf->NetSpdRef,
													BaseControls->CtrlCmdPrm.KVrfFil,
													BaseControls->CtrlCmdMngr.SpdRfFilo );
#else /* (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE) */
			/* 一次ローパスフィルタは通信側で掛けた */
			BaseControls->CtrlCmdMngr.SpdRfFilo = SvControlIf->NetSpdRef;

#endif /* (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE) */

			/* 速度指令取得 */
			BaseControls->CtrlCmdMngr.SpdRefi = BaseControls->CtrlCmdMngr.SpdRfFilo;
			/* 	トルクフィードフォワード指令移動平均フィルタ処理メイン関数		*/			/* <S1D0> */
			SvControlIf->NetTrqFFC = FFcmdFilMoveAverageFilter( &(BaseControls->TFFcmdFil),
																SvControlIf->NetTrqFFC,
																CmdCtrl->tffFilSts);
			/* トルクFF補償生成 */
			BaseControls->CtrlCmdMngr.TrqFFCx += SvControlIf->NetTrqFFC;
		}

	/*----------------------------------------------------------------------------------------------*/
	/*		ソフトスタート処理																		*/
	/*----------------------------------------------------------------------------------------------*/
		BaseControls->CtrlCmdMngr.SpdRefo = BaseSoftStartSpdRef(
												&(BaseControls->CtrlCmdPrm),
												BaseControls->CtrlCmdMngr.SpdRefi,
												BaseControls->CtrlCmdMngr.SpdRefo );
//<S0BA>
/* <S1F5> Start*/
	/*----------------------------------------------------------------------------------------------*/
	/*		直線減速停止用速度値保存																*/
	/*----------------------------------------------------------------------------------------------*/
		if( BaseControls->CtrlModeSet.ZctrlMode.zm != CTRLMODE_LDSTOP )
		{
			BaseControls->CtrlCmdMngr.SpdLdstpRefo = BaseControls->CtrlCmdMngr.SpdRefo;
		}
/* <S1F5> End */
#if 0
	/*----------------------------------------------------------------------------------------------*/
	/*		モードスイッチ演算																		*/
	/*----------------------------------------------------------------------------------------------*/
		ModeSwitch( &(BaseControls->ModeSwData),
					CtrlLoopOut->TrqRefo,
					BaseControls->CtrlCmdMngr.SpdRefo,
					BaseControls->PosManager.PosErrA.per32s );
#endif
		break;


/****************************************************************************************************/
	case CTRLMODE_TRQ:	/* トルク制御モード  (ゼロ速停止／ゼロクランプ用に速度制御を実行)			*/
/****************************************************************************************************/
	/*----------------------------------------------------------------------------------------------*/
	/*		トルク指令生成																			*/
	/*----------------------------------------------------------------------------------------------*/
		/* 指令入力状態設定 */
		BaseControls->TrefInSts = (SvControlIf->NetTrqRef != 0) ? TRUE : FALSE; /* <S0A1> */
		/* トルク指令取得 */
		BaseControls->CtrlCmdMngr.TrqRefo = SvControlIf->NetTrqRef;
		/* ローパスフィルタ処理 */
		BaseControls->CtrlCmdMngr.SpdRfFilo = MlibLpfilter1(
												SvControlIf->NetSpdLmt,
												BaseControls->CtrlCmdPrm.KVrfFil,
												BaseControls->CtrlCmdMngr.SpdRfFilo );
		/* トルク制御時調整レスOFF設定 */
		BaseControls->TuneLessCtrl.var.TuneLessAct = FALSE;

	/*----------------------------------------------------------------------------------------------*/
	/*		トルク制御時の速度制限演算																*/
	/*----------------------------------------------------------------------------------------------*/
		BaseControls->CtrlCmdMngr.OverTrq = OverTorqueCalculation(
												&(BaseControls->CtrlCmdMngr),
												&(BaseControls->CtrlCmdPrm) );
		break;


/****************************************************************************************************/
	case CTRLMODE_JOG:	/* ＪＯＧ運転モード															*/
/****************************************************************************************************/
		SvControlIf->NetSpdRef = FnCmnCtrl->JogSpeed;
		BaseControls->CtrlCmdMngr.SpdRefi = SvControlIf->NetSpdRef;

		/* 調整レス機能スイッチを元に戻す <S0DA> */
		BaseControls->TuneLessCtrl.var.TuneLessAct = BaseControls->TuneLessCtrl.conf.TuningLessUse;

		/* ソフトスタート処理 */
		BaseControls->CtrlCmdMngr.SpdRefo = BaseSoftStartSpdRef(
												&(BaseControls->CtrlCmdPrm),
												BaseControls->CtrlCmdMngr.SpdRefi,
												BaseControls->CtrlCmdMngr.SpdRefo );
//<S0BA>
/* <S1F5> Start */
	/*----------------------------------------------------------------------------------------------*/
	/*		直線減速停止用速度値保存																*/
	/*----------------------------------------------------------------------------------------------*/
		if( BaseControls->CtrlModeSet.ZctrlMode.zm != CTRLMODE_LDSTOP )
		{
			BaseControls->CtrlCmdMngr.SpdLdstpRefo = BaseControls->CtrlCmdMngr.SpdRefo;
		}
/* <S1F5> End */
#if 0
	/*----------------------------------------------------------------------------------------------*/
	/*		モードスイッチ演算																		*/
	/*----------------------------------------------------------------------------------------------*/
		ModeSwitch( &(BaseControls->ModeSwData),
					CtrlLoopOut->TrqRefo,
					BaseControls->CtrlCmdMngr.SpdRefo,
					BaseControls->PosManager.PosErrA.per32s );
#endif
		break;


/****************************************************************************************************/
	case CTRLMODE_ZSRCH:/* 原点サーチ																*/
/****************************************************************************************************/
		/*	変数初期化 : 制御モード変更初回時 */
		if( (0 != BaseControls->CtrlModeSet.CtrlModeChg.b.cm)
			|| (FALSE == BaseControls->BeComplete) )
		{
			BaseControls->ZeroClampCtrl.var.PosError = 0;
			BaseControls->ZeroClampCtrl.var.OrgSrchFinish = FALSE;
		}

		/* 調整レス機能スイッチを元に戻す <S0DA> */
		BaseControls->TuneLessCtrl.var.TuneLessAct = BaseControls->TuneLessCtrl.conf.TuningLessUse;

		BaseControls->CtrlCmdMngr.SpdRefi = ZeroSearchControl(
											&(BaseControls->ZeroClampCtrl),
											&(BaseControls->MotSts),
											BaseControls->GainChange.GselGains[0].ZcKp,
											FnCmnCtrl->JogSpeed );
		BaseControls->CtrlCmdMngr.SpdRefo = BaseControls->CtrlCmdMngr.SpdRefi;

//<S0BA>
/* <S1F5> Start */
	/*----------------------------------------------------------------------------------------------*/
	/*		直線減速停止用速度値保存																*/
	/*----------------------------------------------------------------------------------------------*/
		if( BaseControls->CtrlModeSet.ZctrlMode.zm != CTRLMODE_LDSTOP )
		{
			BaseControls->CtrlCmdMngr.SpdLdstpRefo = BaseControls->CtrlCmdMngr.SpdRefo;
		}
/* <S1F5> End */
#if 0
	/*----------------------------------------------------------------------------------------------*/
	/*		モードスイッチ演算																		*/
	/*----------------------------------------------------------------------------------------------*/
		ModeSwitch( &(BaseControls->ModeSwData),
					CtrlLoopOut->TrqRefo,
					BaseControls->CtrlCmdMngr.SpdRefo,
					BaseControls->PosManager.PosErrA.per32s );
#endif
		break;


/****************************************************************************************************/
	default:			/* 本ケースは無し															*/
/****************************************************************************************************/
		break;
	}

#if 0
/*--------------------------------------------------------------------------------------------------*/
/*		Update Speed Clamp Status																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( 0 != hTrqCtrl->var.SpdClamp )
	{
		SvControlIf->RspCtrl.RspCtrlBit |= SPD_CLAMP_BITMSK;
	}
	else
	{
		SvControlIf->RspCtrl.RspCtrlBit &= ~(SPD_CLAMP_BITMSK);
	}
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		位置指令更新																				*/
/*--------------------------------------------------------------------------------------------------*/
	/* 位置指令更新処理 */
#if 0 /* 2012.08.31 Y.Oka ★μScanA→CpuScanA★ */
	BaseControls->CtrlCmdMngr.dPosRefo = dPosRefo;
#else /* 2012.08.31 Y.Oka ★μScanA→CpuScanA★ */
	PosRefRenewal( PosManager, BaseControls->CtrlModeSet.CtrlMode );	/* <S187> */
#endif /* 2012.08.31 Y.Oka ★μScanA→CpuScanA★ */

	BaseControls->CtrlCmdMngr.PosRefLo = SvControlIf->NetPosRef;

/*--------------------------------------------------------------------------------------------------*/
/*		位置制御モード以外及びゼロ速停止モードの時の位置制御変数初期化								*/
/*--------------------------------------------------------------------------------------------------*/
	if( (BaseControls->CtrlModeSet.CtrlMode.b.cm != CTRLMODE_POS) ||
		(BaseControls->CtrlModeSet.ZctrlMode.zm == CTRLMODE_ZSTOP) ||  /* <S1F5> */
		(BaseControls->CtrlModeSet.ZctrlMode.zm == CTRLMODE_LDSTOP) )   /* <S1F5> */
	{
		/* 位置制御変数初期化 <S14C> */
		InitBasePositionControl( &(BaseLoops->PosCtrl) );
	}
/* <S1F5> Start */

	/*----------------------------------------------------------------------------------------------*/
	/*		直線減速停止速度指令演算																*/
	/*----------------------------------------------------------------------------------------------*/
	if( BaseControls->CtrlModeSet.ZctrlMode.zm == CTRLMODE_LDSTOP )
	{
		BaseControls->CtrlCmdMngr.SpdLdstpRefo = BaseLinDecStopSpdRef( &(BaseControls->CtrlCmdPrm),
															BaseControls->CtrlCmdMngr.SpdLdstpRefo );
	}
/* <S1F5> End */

	BaseControls->CtrlCmdMngr.CtrlMode = BaseControls->CtrlModeSet.CtrlMode.b.cm;

}


/* <S1F5> Start */
/*****************************************************************************
  Description: 直線減速度による減速停止速度指令演算
------------------------------------------------------------------------------
  Parameters:
	SpdRefo;			速度制御：速度指令ＳＦＳ出力
	SpdLinZeroStopDec;	直線減速停止減速量
  Return:
	SpdRefx				速度指令
------------------------------------------------------------------------------
  Note:
    速度指令ソフトスタート演算メイン関数BaseSoftStartSpdRefを参照
*****************************************************************************/
static LONG	BaseLinDecStopSpdRef( CTRL_CMD_PRM *CtrlCmdPrm, LONG SpdRefo )
{
	if( SpdRefo > 0 )
	{
		SpdRefo = SpdRefo - CtrlCmdPrm->SpdLdstpDec;
		if( SpdRefo < 0 )
		{
			SpdRefo = 0;
		}
	}
	else if( SpdRefo < 0 )
	{
		SpdRefo = SpdRefo + CtrlCmdPrm->SpdLdstpDec;
		if( SpdRefo > 0 )
		{
			SpdRefo = 0;
		}
	}
	else
	{
		SpdRefo = 0;
	}
	
	return( SpdRefo );
}
/* <S1F5> End */


/****************************************************************************************************/
/*																									*/
/*		位置制御変数初期化																	<S14C>	*/
/*																									*/
/****************************************************************************************************/
void InitBasePositionControl( POSCTRL *PosCtrl )
{

	MlibResetLongMemory( &(PosCtrl->V), sizeof(PosCtrl->V)/4 );

}

#if	0
#define NRFMAX	16014615
/****************************************************************************************************/
/*																									*/
/*		位置制御演算	未使用																		*/
/*																									*/
/****************************************************************************************************/
LONG BpiBasePositionControl( POS_LOOP_CTRL *PosLoop, CTRL_CMD_MNG *CtrlCmd,
							 POS_MNG_HNDL *PosManager, KSGAIN Kp, KSGAIN Kpi )
{
	LONG	PosErr;
	LONG	PacOut, IacOut;
	LONG	SpdRefx;

	if(CtrlCmd->CtrlMode != CTRLMODE_POS)
	{
		PosLoop->var.PerIvar64[0] = 0;
		PosLoop->var.PerIvar64[1] = 0;
		PosLoop->var.Per64[0] = 0;
		PosLoop->var.Per64[1] = 0;
//		CtrlCmd->PosErr32s = 0;
		PosManager->PosErrA.per32s = 0;
		PosLoop->var.LoopStart = FALSE;
		return( CtrlCmd->SpdRefo );
	}

/*--------------------------------------------------------------------------------------------------*/
/*		位置偏差クリア処理																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( CtrlCmd->PerrClrReq != FALSE )
	{
		PosLoop->var.PerIvar64[0] = 0;
		PosLoop->var.PerIvar64[1] = 0;
		PosLoop->var.Per64[0] = 0;
		PosLoop->var.Per64[1] = 0;
		CtrlCmd->SpdRefo = 0;
		CtrlCmd->PosErr32s = 0;
		PosManager->PosErrA.per32s = 0;
		return( 0 );
	}
/*--------------------------------------------------------------------------------------------------*/
/*		位置制御積分クリア処理																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( CtrlCmd->PicvClrCmd != FALSE )
	{
		PosLoop->var.PerIvar64[0] = 0;
		PosLoop->var.PerIvar64[1] = 0;
	}
/*--------------------------------------------------------------------------------------------------*/
/*		位置制御：位置偏差計算																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( PosLoop->var.LoopStart == FALSE )
	{
		PosLoop->var.LoopStart = TRUE;
#if (FLOAT_USE==TRUE)
		PosErr = FlibPerrcalx( CtrlCmd->BaseFbkPos + CtrlCmd->dPosRefo,
		                       CtrlCmd->PosFbki, PosLoop->var.Per64 );
		PosManager->PosErrA.per32s += (PosManager->var.dPcmda);
	}
	else
	{
		PosErr = FlibPerrcalx( CtrlCmd->dPosRefo, CtrlCmd->dPosFbki, PosLoop->var.Per64 );
		PosManager->PosErrA.per32s += (PosManager->var.dPcmda - PosManager->PgPos.dposix);
	}
#else
		PosErr = MlibPerrcalx( CtrlCmd->BaseFbkPos + CtrlCmd->dPosRefo,
		                       CtrlCmd->PosFbki, PosLoop->var.Per64 );
		PosManager->PosErrA.per32s += (PosManager->var.dPcmda);
	}
	else
	{
		PosErr = MlibPerrcalx( CtrlCmd->dPosRefo, CtrlCmd->dPosFbki, PosLoop->var.Per64 );
		PosManager->PosErrA.per32s += (PosManager->var.dPcmda - PosManager->PgPos.dposix);
	}
#endif /* FLOAT_USE */
/*--------------------------------------------------------------------------------------------------*/
/*		位置制御：比例制御演算																		*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	PacOut = ( (float)PosErr * Kp );
#else
	PacOut = MlibMulgain( PosErr, Kp );
#endif /* FLOAT_USE */
/*--------------------------------------------------------------------------------------------------*/
/*		位置制御：積分制御演算																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( Kpi == 0 )
	{
		IacOut = ((PosLoop->var.PerIvar64[1] + 1) >> 1);
	}
	else
	{
		IacOut = MlibIntegral( PacOut, Kpi, PosLoop->var.PerIvar64 );
	}
/*--------------------------------------------------------------------------------------------------*/
/*		位置制御：ＰＩ制御出力																	*/
/*--------------------------------------------------------------------------------------------------*/
	SpdRefx = PacOut + IacOut;

/*--------------------------------------------------------------------------------------------------*/
/*		速度バイアス加算演算																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( SpdRefx == 0 )
	{
		if( PosErr > 0 )
		{
			SpdRefx =  1;
		}
		else if( PosErr < 0 )
		{
			SpdRefx = -1;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		位置制御出力リミット																		*/
/*--------------------------------------------------------------------------------------------------*/
	else if( SpdRefx > NRFMAX )
	{
		SpdRefx =  NRFMAX;
	}
	else if( SpdRefx < -NRFMAX )
	{
		SpdRefx = -NRFMAX;
	}

	CtrlCmd->SpdRefo = SpdRefx;
	CtrlCmd->PosErr32s = PosErr;

	return SpdRefx;
}
#endif

/****************************************************************************************************/
/*																									*/
/*		磁極検出制御時位置指令管理処理																*/
/*																									*/
/****************************************************************************************************/
void	BpiMpoleFindMakePosCmd( BASE_LOOPCTRLS *BaseLoops, SERVO_CONTROL_IF *SvControlIf,
													FUN_CMN_CONTROL *FnCmnCtrl, BPRMDAT *Bprm)
{
	BASE_CTRL_HNDL *BaseControls;
	LONG	dPosRefo;
	LONG	lwk;

	/* ローカル変数の初期化 */
	BaseControls = BaseLoops->BaseCtrls;	
	dPosRefo = 0;

	/* 速度ＦＢ演算 -> モータ差分速度固定 */
//	pBaseCtrl->CtrlCmdMngr.SpdFbki = pBaseCtrl->MotSts.MotSpd;

	if( BaseControls->CtrlModeSet.CtrlMode.b.cm == CTRLMODE_POS )
	{
		if( BaseControls->CtrlModeSet.CtrlModeLst.b.cm != CTRLMODE_POS )
		{
			/* 制御モード切り替えによる基準位置作成 */
			/* 位置偏差関連の見直しのため変更 <S14C> */
			lwk = MlibPcmdEgear( BaseControls->PosManager.VirtualPositionError,
								 &(BaseControls->PosManager.conf.Egear),
								 &(BaseControls->PosManager.var.dPcmdEgrem) );
#if 0/* SigamV-MD Microに基準位置を譲渡する	*/								/* <S02F> */
			BaseControls->CtrlCmdMngr.BaseFbkPos = lwk + BaseControls->MotSts.MotPos;
#else/* Sigam7 ScanAに初回位置偏差を譲渡する。	*/							/* <S02F> */
			BaseControls->PosManager.var.Per64[0] = 0;
			BaseControls->PosManager.var.Per64[1] = 0;
			MlibPerrcalx(lwk, 0, BaseControls->PosManager.var.Per64);
#endif
			MlibPerrcala( BaseControls->PosManager.VirtualPositionError, 0,
							&BaseControls->PosManager.conf.Egear, &BaseControls->PosManager.var.PosErrA );
			/* 現在指令位置初期化 */
			BaseControls->CtrlCmdMngr.PosRefLo = BaseControls->PosManager.VirtualPositionError
											+ BaseControls->PosManager.PgPos.apos[0];
		}
		else
		{
			BaseControls->CtrlCmdMngr.BaseFbkPos += BaseControls->CtrlCmdMngr.dPosRefo;
		}

		/* サーボ通信位置指令管理(dPosRefo,etc) */
		dPosRefo = PosMngNetPosCmdManager( BaseLoops, SvControlIf, FnCmnCtrl, Bprm );/* <S04D> */
	}
	BaseControls->CtrlCmdMngr.dPosRefo = dPosRefo;
	BaseControls->CtrlCmdMngr.PosRefLo = SvControlIf->NetPosRef;

/*--------------------------------------------------------------------------------------------------*/
/*		位置制御モード以外の時の位置制御変数初期化													*/
/*--------------------------------------------------------------------------------------------------*/
	if( BaseControls->CtrlModeSet.CtrlMode.b.cm != CTRLMODE_POS )
	{
		/* 位置管理変数初期化処理 */
		PosMngInitPositionManager( BaseControls );
	}
}

//<S0BA>
#if 0
/****************************************************************************************************/
/*																									*/
/*		モードスイッチ演算																			*/
/*																									*/
/****************************************************************************************************/
static void ModeSwitch( MODE_SW *ModeSwData, LONG trq_ref, LONG spd_ref, LONG pos_err )
{
	LONG	ModeSWFlag;
	LONG	wk;

	/* ローカル変数の初期化 */
	ModeSWFlag = FALSE;
	wk = 0;

	switch( ModeSwData->conf.ModeSWSel )
	{
	case MODESW_TRQREF: /* モードスイッチ トルク指令 */
		wk = trq_ref;
		if( wk < 0 )
		{
			wk = -wk;
		}
		if( wk >= ModeSwData->conf.MSWTrqLevel )
		{
			ModeSWFlag = TRUE;
		}
		break;

	case MODESW_SPDREF: /* モードスイッチ 速度指令 */
		wk = spd_ref;
		if( wk < 0 )
		{
			wk = -wk;
		}
		if( wk >= ModeSwData->conf.MSWSpdLevel )
		{
			ModeSWFlag = TRUE;
		}
		break;

	case MODESW_MOTACC: /* モードスイッチ 加速度 */
		wk = ModeSwData->var.Acceleration;
		if( wk < 0 )
		{
			wk = -wk;
		}
		if( wk >= ModeSwData->conf.MSWAccLevel )
		{
			ModeSWFlag = TRUE;
		}
		break;

	case MODESW_POSERR: /* モードスイッチ 偏差パルス */
		wk = pos_err;
		if( wk < 0 )
		{
			wk = -wk;
		}
		if( wk >= ModeSwData->conf.MSWErrLevel )
		{
			ModeSWFlag = TRUE;
		}
		break;

	case MODESW_NOTUSE: /* モードスイッチ 不使用 */
	default:
		break;
	}
	ModeSwData->var.ModeSWFlag = ModeSWFlag;
}
#endif


/****************************************************************************************************/
/*																									*/
/*		速度指令ソフトスタート演算																	*/
/*																									*/
/****************************************************************************************************/
static LONG BaseSoftStartSpdRef( CTRL_CMD_PRM *CtrlCmdPrm, LONG SpdRef, LONG SpdRefx )
{
	LONG	LastSpdRefx;

	LastSpdRefx = SpdRefx;

/*--------------------------------------------------------------------------------------------------*/
/*		ソフトスタート演算																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( SpdRef > SpdRefx )
	{
		if( SpdRefx >= 0 )
		{
			SpdRefx = SpdRefx + CtrlCmdPrm->SpdSfsAcc;
			if( SpdRefx > SpdRef )
			{
				SpdRefx = SpdRef;
			}
		}
		else
		{
			SpdRefx = SpdRefx + CtrlCmdPrm->SpdSfsDec;
			if( SpdRefx > SpdRef )
			{
				SpdRefx = SpdRef;
			}
		}
	}
	/*----------------------------------------------------------------------------------------------*/
	else if( SpdRef < SpdRefx )
	{
		if( SpdRefx <= 0 )
		{
			SpdRefx = SpdRefx - CtrlCmdPrm->SpdSfsAcc;
			if( SpdRefx < SpdRef )
			{
				SpdRefx = SpdRef;
			}
		}
		else
		{
			SpdRefx = SpdRefx - CtrlCmdPrm->SpdSfsDec;
			if( SpdRefx < SpdRef )
			{
				SpdRefx = SpdRef;
			}
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		ゼロ速経由処理 : 何で必要か良く分からないが、取り敢えず、ＳＧＤＳに合わせる					*/
/*--------------------------------------------------------------------------------------------------*/
//	if( (SpdRef != SpdRefx) && (CtrlCmdPrm->SpdSfsAcc != CtrlCmdPrm->SpdSfsDec) )/* SGDVの不具合水平展開 */
	if(CtrlCmdPrm->SpdSfsAcc != CtrlCmdPrm->SpdSfsDec)
	{
		if( (LastSpdRefx != 0) && ((LastSpdRefx ^ SpdRefx) < 0) )
		{
			SpdRefx = 0;
		}
	}
	return( SpdRefx );
}



/****************************************************************************************************/
/*																									*/
/*		原点サーチ制御演算処理																		*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : ６０rpmの速度指令で原点をサーチする。													*/
/*			スキャンＢでは原点(Ｃ相)の検出とゼロクランプ指令を行なう。								*/
/*			ラウンドで、キー操作のシーケンス処理を行う。											*/
/*																									*/
/*	注 意 : モータ位置制御であり、フルクローズド制御はサポートしていないので注意すること。			*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static LONG ZeroSearchControl( ZX_CTRL_HNDL *ZeroClampCtrl, BASE_MOT_STS *MotSts, KSGAIN ZcKp, LONG JogSpeed )
{
	LONG	spd_ref;

/*--------------------------------------------------------------------------------------------------*/
/*		ＪＯＧ速度運転：原点を検出するまで															*/
/*--------------------------------------------------------------------------------------------------*/
	if( !(MotSts->MotCphPass || ZeroClampCtrl->var.OrgSrchFinish) )	/* C相通過チェック				*/
	{
		ZeroClampCtrl->var.PosError = 0;
		spd_ref = JogSpeed;											/* 原点サーチ速度指令			*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		ゼロクランプ制御：原点サーチ完了															*/
/*--------------------------------------------------------------------------------------------------*/
	else															/* C相通過検出後				*/
	{
		if( ZeroClampCtrl->var.OrgSrchFinish == FALSE )
		{ /* 特殊モータの原点バラツキ対策用 */
			/* 位置偏差ラッチ(初回) */
			ZeroClampCtrl->var.PosError = -MotSts->MposFromCpos;
		}
		else
		{ /* ２回目以降はモータ位置差分を使用する */
			/* 位置偏差演算(２回目以降) */
			ZeroClampCtrl->var.PosError -= MotSts->dMotPos;
		}
	/*----------------------------------------------------------------------------------------------*/
		/* 原点サーチ完了Flag */
		ZeroClampCtrl->var.OrgSrchFinish = TRUE;
		/* 位置制御演算(Ｐ制御) */
#if (FLOAT_USE==TRUE)
		spd_ref = (LONG)((float)ZeroClampCtrl->var.PosError * ZcKp);
#else
		spd_ref = MlibMulgain( ZeroClampCtrl->var.PosError, ZcKp );
#endif /* FLOAT_USE */
	/*----------------------------------------------------------------------------------------------*/
		if( spd_ref == 0 )											/* 速度指令ゼロ時の補正			*/
		{
			if( ZeroClampCtrl->var.PosError > 0 )
			{
				spd_ref =  1;
			}
			else if( ZeroClampCtrl->var.PosError < 0 )
			{
				spd_ref = -1;
			}
		}
	/*----------------------------------------------------------------------------------------------*/
		else if( spd_ref > ZeroClampCtrl->conf.SpdLimit )			/* 速度指令上限リミット			*/
		{
			spd_ref =  ZeroClampCtrl->conf.SpdLimit;
		}
	/*----------------------------------------------------------------------------------------------*/
		else if( spd_ref < -ZeroClampCtrl->conf.SpdLimit )			/* 速度指令下限リミット			*/
		{
			spd_ref = -ZeroClampCtrl->conf.SpdLimit;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
	return( spd_ref );

}



/****************************************************************************************************/
/*																									*/
/*		トルク制御時の速度制限用トルク（余剰トルク）演算											*/
/*																									*/
/****************************************************************************************************/
#if 0
/* Mercury(SGDV-MD)の処理 (速度制限が効かない) */
static LONG OverTorqueCalculation( LONG SpdRfFilo, LONG TConSpdLmtLevel, BOOL VrefSpdLmt )
{
	LONG	vref_limit;

	/*----------------------------------------------------------------------------------------------*/
	/*		速度制限値 TrqCtrl.V.SpdLimit 選択														*/
	/*----------------------------------------------------------------------------------------------*/
	if( VrefSpdLmt )							/* V-REFを外部速度制限入力として使用する場合		*/
	{
		vref_limit = MlibLABS( SpdRfFilo );

		/* 外部､内部の小さい速度で制限 */
		if( vref_limit > TConSpdLmtLevel )
		{
			vref_limit = TConSpdLmtLevel;
		}
	}
	else
	{
		/* Pn407による内部速度制限 */
		vref_limit = TConSpdLmtLevel;
	}

	return vref_limit;
}

#else

/* SGDVの処理 */
/****************************************************************************************************/
/*																									*/
/*		トルク制御時の速度制限用トルク（余剰トルク）演算											*/
/*			意味のわからない点があるので、変数置き換え以外はSGDSソフトのままにしておく				*/
/*																	2003.04.16	S.Takamatsu			*/
/****************************************************************************************************/
#define	SPDLMTSW	0x40000					/* OS速度の1/64											*/
#define	INTDECVAL	(2*SPDLMTSW)			/* OS速度の1/32(SPDLMTSWより大きくなければいけない)		*/
#define	OVERTRQLMT	19573419				/* 余剰トルクの上限値(=2^24*17500/15000)				*/
LONG OverTorqueCalculation( CTRL_CMD_MNG *CtrlCmdMng, CTRL_CMD_PRM *CtrlCmdPrm )
{
LONG	SpdLimitx;
LONG	nerp;
LONG	neri;
LONG	nerpi;
LONG	lwrk;
LONG	overtorque;
LONG	Status;
LONG	SpdFbk = CtrlCmdMng->SpdFbki;
LONG	TrqRefi = CtrlCmdMng->TrqRefo;
	/*----------------------------------------------------------------------------------------------*/
	/*		速度制限値 TrqCtrl.V.SpdLimit 選択														*/
	/*----------------------------------------------------------------------------------------------*/
		if( CtrlCmdPrm->VrefSpdLmt )			/* V-REFを外部速度制限入力として使用する場合		*/
		{
			SpdLimitx = MlibAbs32( CtrlCmdMng->SpdRfFilo );	/* 外部､内部の小さい速度で制限			*/
			if( SpdLimitx < CtrlCmdPrm->TConSpdLmtLevel )	
			{ 
				CtrlCmdMng->SpdLimit = SpdLimitx; 
			}
			else
			{ 
				CtrlCmdMng->SpdLimit = CtrlCmdPrm->TConSpdLmtLevel; 
			}
		}
		else
		{
			CtrlCmdMng->SpdLimit = CtrlCmdPrm->TConSpdLmtLevel;	/* Pn407による内部速度制限 					*/
		}

	/*----------------------------------------------------------------------------------------------*/
	/*		速度ＦＢと制限速度との速度偏差作成														*/
	/*----------------------------------------------------------------------------------------------*/
		if ( SpdFbk > 0 )
		{
			nerp = SpdFbk - CtrlCmdMng->SpdLimit;
		}
		else if ( SpdFbk < 0 )
		{
			nerp = SpdFbk + CtrlCmdMng->SpdLimit;
		}
		else
		{
			nerp = ( TrqRefi >= 0 ) ?	-CtrlCmdMng->SpdLimit : CtrlCmdMng->SpdLimit;
		}

		/* 速度偏差比例項 */
		lwrk = MlibMulgain29( nerp, CtrlCmdPrm->OverTrqLoopGain[0] );
		
		/* 速度偏差積分項 */
		neri = MlibIntegral( nerp, CtrlCmdPrm->OverTrqIntegGain[0], CtrlCmdPrm->OverTrqIntegBuf );

		nerpi = lwrk + neri;	/* 2^29+2^24：オーバーフローはしない */

		/*  速度偏差から計算した積分値が実際のモータ回転方向と違う場合の処理 */
		if ( (((SpdFbk ^ nerpi) < 0) && (TrqRefi >= 0)) || ((((SpdFbk - 1) ^ nerpi) < 0) && (TrqRefi < 0)) )
		{
			/* 速度制限が低い場合の処理 */
			if ( CtrlCmdMng->SpdLimit <= SPDLMTSW )
			{
				/* 徐々に減速 */
				if ( neri >= 0 )
				{
					lwrk = -INTDECVAL;
					neri = MlibIntegral( lwrk, CtrlCmdPrm->OverTrqIntegGain[0], CtrlCmdPrm->OverTrqIntegBuf );

					/* 積分溜まりが0になるまで、速度制限 */
					Status = ( neri >= 0 ) ? TRUE : FALSE;
				}
				else
				{
					lwrk = INTDECVAL;
					neri = MlibIntegral( lwrk, CtrlCmdPrm->OverTrqIntegGain[0], CtrlCmdPrm->OverTrqIntegBuf );

					/* 積分溜まりが0になるまで、速度制限 */
					Status = ( neri < 0 ) ? TRUE : FALSE;
				}
			}
			else
			{
				Status = FALSE;
			}
		}
		else
		{
			Status = TRUE;
		}

		if ( Status )
		{
			CtrlCmdMng->SpdClamp = TRUE;
		}
		else
		{
			CtrlCmdPrm->OverTrqIntegBuf[ 1 ] = 0;
			CtrlCmdPrm->OverTrqIntegBuf[ 0 ] = 0;
			nerpi = 0;
			CtrlCmdMng->SpdClamp = FALSE;
		}

		/* 逆向き最大トルクが入っても減速できるであろう大きさ */
		overtorque = MlibLimitul( nerpi, OVERTRQLMT, -OVERTRQLMT );

		return overtorque;

}
#endif


/****************************************************************************************************/
/*																									*/
/*		不足電圧時のトルクリミット演算処理															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 不足電圧ワーニング発生時、トルク制限をする場合 かつ オーバトラベルでない場合			*/
/*				ワーニング発生中	：Pn424に従ったトルク制限										*/
/*				ワーニング解除中	：Pn425に従ったトルク制限増加									*/
/*				ワーニング解除後	：MAXトルクによる制限											*/
/*					＝Pn425時間経過後																*/
/*			制限しない場合			：MAXトルクによる制限											*/
/*																									*/
/****************************************************************************************************/
static LONG LpxUvTrqLmtControl( UV_TLIM_DATA *UvTrqLimits, BOOL OtStatus, ALMMAN_HANDLE *AlmMngr ) /* <S145> */
{
	if ( (UvTrqLimits->conf.UvTrqLmtCtrl != FALSE) && (OtStatus == FALSE) )
	{
		if ( FALSE != ALMCheckEachState( AlmMngr, WRN_UV ) ) /* <S145> */
		{
			UvTrqLimits->var.UvTrqLmt = UvTrqLimits->conf.UvTLimSetting;
			UvTrqLimits->var.UvTrqLmtWait = UvTrqLimits->conf.UvTrqLmtOffFil;
		}
		else
		{
			if ( UvTrqLimits->var.UvTrqLmtWait == 0 )
			{
				UvTrqLimits->var.UvTrqLmt = NORMAXVALUE;
			}
			else
			{
				UvTrqLimits->var.UvTrqLmtWait--;
				UvTrqLimits->var.UvTrqLmt += UvTrqLimits->conf.DeltaUvTrqLmt;
			}
		}
	}
	else
	{
		UvTrqLimits->var.UvTrqLmt = NORMAXVALUE;
		UvTrqLimits->var.UvTrqLmtWait = 0;
	}
	return UvTrqLimits->var.UvTrqLmt;
}


/****************************************************************************************************/
/*																									*/
/*		トルクリミット演算処理																		*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : パラメータで設定される内部トルク制限(Pn402,Pn403), 外部トルク制限(Pn404,Pn405)			*/
/*			およびトルク指令ＡＤから、下記に示す条件にてトルク制限値を演算する。					*/
/*			なお、ＯＴ時(ＯＴシーケンスからの信号)は、非常停止トルク(Pn406)にて制限する。			*/
/*																									*/
/*																									*/
/*		正転側トルク制限演算ブロック図 (逆転側も同様)												*/
/*																									*/
/*		Pn406 : EstopTrqLmt -----||-------------------------------+-------> BoutV.FwTrqLmt			*/
/*							    OT-TrqLmt   			 		  | 								*/
/*													   			  | 								*/
/*												+-------+   	  | 								*/
/*		Pn402 : FwIntTrqLmt ------------------->|     	|   	  | 								*/
/*												|   	|   	  | 								*/
/*												|   	|   	  | 								*/
/*		Pn404 : FwExtTrqLmt -----||------------>|   	|   	  | 								*/
/*							    P-CL			|  Min  +---|/|---+ 								*/
/*												|   	|  OT-TrqLmt								*/
/*				TrqRefAd    -----||------------>|   	|   										*/
/*							  Pn002.0=1  		|   	|   										*/
/*												|   	|   										*/
/*				TrqRefAd    -----||-------||--->|   	|   										*/
/*							  Pn002.0=3  P-CL	+-------+   										*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
void BpiTorqueLimitControl( TRQ_LMT_DATA *TrqLimits, BASE_CTRL_HNDL *pBaseCtrl,	BOOL OtTrqLmtReq, ALMMAN_HANDLE *AlmMngr ) /* <S145> */
{
	LONG			lwk;
	LONG			FwTrqLmt;
	LONG			RvTrqLmt;
	LONG			UvTrqLmt;

/*--------------------------------------------------------------------------------------------------*/
/*		ＯＴ時のトルクリミット演算																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( OtTrqLmtReq )
	{
		FwTrqLmt = TrqLimits->conf.EstopTrqLmt;
		RvTrqLmt = TrqLimits->conf.EstopTrqLmt;

		LpxUvTrqLmtControl( &(TrqLimits->UvTrqLimits), TRUE, AlmMngr );	/* <S145> */
	}
	else
	{
/*--------------------------------------------------------------------------------------------------*/
/*		主回路電源電圧降下時のトルクリミット演算													*/
/*--------------------------------------------------------------------------------------------------*/
		UvTrqLmt = LpxUvTrqLmtControl( &(TrqLimits->UvTrqLimits), FALSE, AlmMngr );	/* <S145> */

/*--------------------------------------------------------------------------------------------------*/
/*		非ＯＴ時のトルクリミット演算																*/
/*--------------------------------------------------------------------------------------------------*/
		if( TrqLimits->var.PclSignal )
		{
			FwTrqLmt = ( TrqLimits->conf.FwIntTrqLmt < TrqLimits->conf.FwExtTrqLmt ) ?
						 TrqLimits->conf.FwIntTrqLmt:
						 TrqLimits->conf.FwExtTrqLmt;
		}
		else
		{
			FwTrqLmt = TrqLimits->conf.FwIntTrqLmt;
		}

		if( TrqLimits->var.NclSignal )
		{
			RvTrqLmt = ( TrqLimits->conf.RvIntTrqLmt < TrqLimits->conf.RvExtTrqLmt ) ?
						 TrqLimits->conf.RvIntTrqLmt:
						 TrqLimits->conf.RvExtTrqLmt;
		}
		else
		{
			RvTrqLmt = TrqLimits->conf.RvIntTrqLmt;
		}
	/*------------------------------------------------------------------------------------------*/
		if( UvTrqLmt < FwTrqLmt )
		{
			FwTrqLmt = UvTrqLmt;
		}

		if( UvTrqLmt < RvTrqLmt )
		{
			RvTrqLmt =  UvTrqLmt;
		}
	/*------------------------------------------------------------------------------------------*/
		if( (pBaseCtrl->CtrlModeSet.CtrlMode.b.cm != CTRLMODE_TRQ)
			&& (pBaseCtrl->CtrlModeSet.CtrlMode.b.cm != CTRLMODE_JOG) )
		{
			if( TrqLimits->conf.TrefTrqLmt || (TrqLimits->conf.TrefTrqLmtCL && TrqLimits->var.PclSignal) )
			{ /* (Pn002.0=1:TLIMによるトルク制限有効) || ((Pn002.0=3:PCL/NCLによるトルク制限有効) && (PCL状態)) */
				lwk = MlibLABS( TrqLimits->var.FwTrqLmt );
				if( lwk < FwTrqLmt )
				{
					FwTrqLmt = lwk;
				}
			}
		/*--------------------------------------------------------------------------------------*/
			if( TrqLimits->conf.TrefTrqLmt || (TrqLimits->conf.TrefTrqLmtCL && TrqLimits->var.NclSignal) )
			{ /* (Pn002.0=1:TLIMによるトルク制限有効) || ((Pn002.0=3:PCL/NCLによるトルク制限有効) && (NCL状態)) */
				lwk = MlibLABS( TrqLimits->var.RvTrqLmt );
				if( lwk < RvTrqLmt )
				{
					RvTrqLmt = lwk;
				}
			}
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		トルクリミット演算結果出力 --> Kernel														*/
/*--------------------------------------------------------------------------------------------------*/
	TrqLimits->var.PosTrqLmtOut =  FwTrqLmt;				/* 正転側トルクリミット					*/
	TrqLimits->var.NegTrqLmtOut = -RvTrqLmt;				/* 逆転側トルクリミット					*/

}



/****************************************************************************************************/
/*																									*/
/*		モータ速度検出用移動平均時定数変更サービス		<V259>										*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : モータ速度検出用移動平均フィルタの移動平均回数をオンラインで変更する。					*/
/*			引数に0～4以外が指定された場合は、パラメータPn009.2とエンコーダ分解能から				*/
/*			決まる初期値が設定される。																*/
/*																									*/
/*		（注意）																					*/
/*			速度があるときに変更を行うと、移動平均用バッファのクリアも行われるため、				*/
/*			速度が一瞬ゼロになってしまう。移動平均回数の変更は速度＝０で行うこと。					*/
/*																									*/
/****************************************************************************************************/
void BpiChangeSpdDetMaFil( MOTSPDMAFIL *MotSpdMafil, LONG num )		/* <S02F> */
{
	LONG	lwk0;

	if( ( num >= 0 ) && ( num <= 4 ) )
	{
		lwk0 = num;
	}
	else
	{
		lwk0 = MotSpdMafil->defaultNumBit;
	}
/*--------------------------------------------------------------------------------------------------*/
	if( MotSpdMafil->manumBit != lwk0 )
	{
		MotSpdMafil->manumBit = lwk0;
		MotSpdMafil->idx = 0;
		MotSpdMafil->ksub = 0;
		MotSpdMafil->mabufSumx = 0;
	}
}



/****************************************************************************************************/
/*																									*/
/*		位置指令更新処理																			*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*			パラメータ再計算や電流零位相情報書込みなどでScanAが停止すると、							*/
/*			ScanAが位置指令を受け取れなくなり、位置ずれの原因となる。								*/
/*			ScanAが位置指令を受け取れなかった場合は、ScanBで位置指令を溜めておく処理とする			*/
/****************************************************************************************************/
void	PosRefRenewal( POS_MNG_HNDL *PosManager, CTRLMD CtrlMode )
{
	if( CtrlMode.b.cm == CTRLMODE_POS )
	{ /* ScanAが位置指令を受け取れない場合 */
		if( PosManager->RefRenewCntA != PosManager->RefRenewCntB )
		{
			PosManager->CompdPosRefi += PosManager->dPosRefi;							/* <S187>	*/
			PosManager->CompdPcmda   += PosManager->var.dPcmda;
		}
		else												/* 通常処理								*/
		{
			PosManager->CompdPosRefi = PosManager->dPosRefi;							/* <S187>	*/
			PosManager->CompdPcmda   = PosManager->var.dPcmda;
		}
	}
	else													/* 位置制御以外の場合はクリアする		*/
	{
			PosManager->CompdPosRefi = 0;
			PosManager->CompdPcmda   = 0;
	}
	PosManager->RefRenewCntB++;							/* ScanA位置指令更新用 					*/

	return;
}



/***************************************** end of file **********************************************/
